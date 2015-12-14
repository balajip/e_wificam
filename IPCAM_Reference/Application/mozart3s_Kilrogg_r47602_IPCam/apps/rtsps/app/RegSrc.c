/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2006-2010 VN Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN INC.                                                    |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 * 
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2006-2010 VN, Inc. All rights reserved.
 *
 * \file
 * RegSrc.c
 *
 * \brief
 * A Live Media Source Implementation
 * Request media bitstream from BSD by register mechanism
 * The media is from unix domain socket (DGRAM) in ubuffer format
 *
 * \date
 * 2009/05/13
 *
 * \author
 * DJhow.Tu
 *
 *
 *******************************************************************************
 */

#include "RegSrc.h"
#include "LiveMediaSrc_protect.h"
#include "UnixSockUtil.h"
#include "ubuffer.h"
#include "ubuffer_limit.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "msgdef.h"
#include "msg_util.h"

// -- Convention -- start
SCODE RegSrc_Release(HANDLE *phRegSrc);
SCODE RegSrc_GetUBuffer(HANDLE hRegSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE RegSrc_ReleaseUBuffer(HANDLE hRegSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE RegSrc_EventHandler(HANDLE hRegSrc, ELMSrcEventType eType);
SCODE RegSrc_WakeUpToTerminate(HANDLE hObject)
{
    return S_OK;
}

static TLMSrcCBMap	tRegSrcCBMap = 
{
	RegSrc_Release,
	RegSrc_GetUBuffer, 
	RegSrc_ReleaseUBuffer, 
	RegSrc_EventHandler,
	RegSrc_WakeUpToTerminate
};

// -- Convention -- end

typedef struct register_source
{
	TLiveMediaSrc		tLMSrc;
	int					iSrcFd;
	CHAR				*szRegPath;
	CHAR				*szRcvBSPath;
	CHAR				*szRcvAckPath;
	DWORD				dwRegTrackNo;
	DWORD				dwRegMType;
	DWORD				dwIsRcvConf;
	fd_set				tReadfdsRec;
	fd_set				tReadfds;
	BYTE				*pbyBuf;
	int					iBufSz;
} TRegSrc;

static int iRecvSockNo = 0;

SCODE RegSrc_Initial(HANDLE *phRegSrc, TRegSrcInitOpts *ptInitOpts)
{
	TRegSrc *ptRegSrc;
	CHAR	acRecvPath[128];

	assert(phRegSrc && ptInitOpts);

	ptRegSrc = (TRegSrc *)malloc(sizeof(TRegSrc));
	if (ptRegSrc == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		return S_FAIL;
	}
	memset(ptRegSrc, 0, sizeof(TRegSrc));
	*phRegSrc = ptRegSrc;

	((TLiveMediaSrc *)ptRegSrc)->ptCBMap = &tRegSrcCBMap;

	ptRegSrc->iSrcFd	= -1;
	
	if (ptInitOpts->szRegPath == NULL)
	{
		debug_printf("[%s:%s:%d] Invalid reg sock path %s!\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szRegPath);
		goto error_handle;
	}

	ptRegSrc->szRegPath = strdup(ptInitOpts->szRegPath);
	if (ptRegSrc->szRegPath == NULL)
	{
		debug_printf("[%s:%s:%d] strdup error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	sprintf(acRecvPath, "/tmp/rtsps_%d.%d.sck", getpid(), iRecvSockNo);
	unlink(acRecvPath);
	ptRegSrc->szRcvBSPath = strdup(acRecvPath);
	if (ptRegSrc->szRcvBSPath == NULL)
	{
		debug_printf("[%s:%s:%d] strdup error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	sprintf(acRecvPath, "/tmp/rtsps_%d_ack.%d.sck", getpid(), iRecvSockNo++);
	unlink(acRecvPath);
	ptRegSrc->szRcvAckPath = strdup(acRecvPath);
	if (ptRegSrc->szRcvAckPath == NULL)
	{
		debug_printf("[%s:%s:%d] strdup error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	if ((ptRegSrc->iSrcFd=UnixDgram_CreateSock(ptRegSrc->szRcvBSPath )) < 0)
	{
		debug_printf("[%s:%s:%d] UnixDgram_CreateSock error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	ptRegSrc->dwRegTrackNo	= ptInitOpts->dwTrackNo;
	ptRegSrc->dwRegMType	= ptInitOpts->dwMediaType;
	ptRegSrc->dwIsRcvConf	= 0;

	ptRegSrc->iBufSz = MAX_MP4V_UBUFFER;
	ptRegSrc->pbyBuf = (BYTE *)malloc(ptRegSrc->iBufSz);
	if (ptRegSrc->pbyBuf == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	FD_ZERO(&ptRegSrc->tReadfdsRec);
	FD_SET(ptRegSrc->iSrcFd, &ptRegSrc->tReadfdsRec);
	FD_ZERO(&ptRegSrc->tReadfds);

	return S_OK;
error_handle:

	RegSrc_Release(phRegSrc);
	return S_FAIL;
}

SCODE RegSrc_Release(HANDLE *phRegSrc)
{
	TRegSrc *ptRegSrc;

	ptRegSrc = (TRegSrc *)*phRegSrc;

	if (ptRegSrc->iSrcFd >= 0)
	{
		close(ptRegSrc->iSrcFd);
	}
	if (ptRegSrc->szRegPath != NULL)
	{
		free(ptRegSrc->szRegPath);
	}
	if (ptRegSrc->szRcvBSPath != NULL)
	{
		free(ptRegSrc->szRcvBSPath);
	}
	if (ptRegSrc->szRcvAckPath != NULL)
	{
		free(ptRegSrc->szRcvAckPath);
	}
	if (ptRegSrc->pbyBuf != NULL)
	{
		free(ptRegSrc->pbyBuf);
	}

	free(ptRegSrc);
	*phRegSrc = NULL;

	return S_OK;
}

// Begin - Socket Source related functions
SCODE RegSrc_EventHandler(HANDLE hRegSrc, ELMSrcEventType eType)
{
	TRegSrc			*ptRegSrc;
	TDataRegBSState	tRegBSState;
	TMsgState		tMsgState;
	DWORD			dwBufSz;
	BYTE			abyBuf[MSG_MAX_LENGTH];
	int				iTimes;

	ptRegSrc = (TRegSrc *)hRegSrc;
	
	switch(eType)
	{
		case letNeedIntra:
			printf("Need Intra\n");
			break;
		case letNeedConf:
			printf("Need Conf\n");

			memset(&tRegBSState, 0, sizeof(TDataRegBSState));
			tRegBSState.dwTrackNo		= (ptRegSrc->dwRegTrackNo << 8) | (0x00000001);
			tRegBSState.byMediaType		= (BYTE)ptRegSrc->dwRegMType;
			tRegBSState.bySetNonBlock	= 0;
			tRegBSState.szDataPath		= ptRegSrc->szRcvBSPath;

			dwBufSz = MSG_MAX_LENGTH;
			if (MsgData_RegBS_Compose(abyBuf, &dwBufSz, &tRegBSState) != S_OK)
			{
				printf("MsgData_RegBS_Compose error\n");
				return S_FAIL;
			}

			memset(&tMsgState, 0, sizeof(TMsgState));
			tMsgState.usCmd         = GOLD_MSG_REG;
			tMsgState.byProcID      = GOLD_PROC_RTSPS;
			tMsgState.byMsgID       = 0x01;
			tMsgState.byDataType    = 0x00;
			tMsgState.szAckPath     = NULL;
			tMsgState.pbyData       = abyBuf;
			tMsgState.dwDataLen     = dwBufSz;

			if (Msg_Send(ptRegSrc->iSrcFd, &tMsgState, GOLD_CMDPATH_BSD) <= 0)
			{
				printf("Msg_Send error\n");
				return S_FAIL;
			}
#if 0
			iTimes = 10;
			while ((ptRegSrc->dwIsRcvConf == 0) && (iTimes > 0))
			{
				usleep(50000);
				--iTimes;
			}
			
			memset(&tMsgState, 0, sizeof(TMsgState));
			tMsgState.usCmd         = GOLD_MSG_UNREG;
			tMsgState.byProcID      = GOLD_PROC_RTSPS;
			tMsgState.byMsgID       = 0x01;
			tMsgState.byDataType    = 0x00;
			tMsgState.szAckPath     = NULL;
			tMsgState.pbyData       = abyBuf;
			tMsgState.dwDataLen     = dwBufSz;

			if (Msg_Send(ptRegSrc->iSrcFd, &tMsgState, GOLD_CMDPATH_BSD) <= 0)
			{
				printf("Msg_Send error\n");
				return S_FAIL;
			}
#endif
			ptRegSrc->dwIsRcvConf = 0;

			break;
		case letBitstrmStart:
			printf("Bitstrm start\n");
			
			memset(&tRegBSState, 0, sizeof(TDataRegBSState));
			tRegBSState.dwTrackNo		= (ptRegSrc->dwRegTrackNo << 8) | (0x00000001);
			tRegBSState.byMediaType		= (BYTE)ptRegSrc->dwRegMType;
			tRegBSState.bySetNonBlock	= 0;
			tRegBSState.szDataPath		= ptRegSrc->szRcvBSPath;

			dwBufSz = MSG_MAX_LENGTH;
			if (MsgData_RegBS_Compose(abyBuf, &dwBufSz, &tRegBSState) != S_OK)
			{
				printf("MsgData_RegBS_Compose error\n");
				return S_FAIL;
			}

			memset(&tMsgState, 0, sizeof(TMsgState));
			tMsgState.usCmd         = GOLD_MSG_REG;
			tMsgState.byProcID      = GOLD_PROC_RTSPS;
			tMsgState.byMsgID       = 0x01;
			tMsgState.byDataType    = 0x00;
			tMsgState.szAckPath     = NULL;
			tMsgState.pbyData       = abyBuf;
			tMsgState.dwDataLen     = dwBufSz;

			if (Msg_Send(ptRegSrc->iSrcFd, &tMsgState, GOLD_CMDPATH_BSD) <= 0)
			{
				printf("Msg_Send error\n");
				return S_FAIL;
			}

			break;
		case letBitstrmStop:
			printf("Bitstrm stop\n");
#if 0			
			memset(&tRegBSState, 0, sizeof(TDataRegBSState));
			tRegBSState.dwTrackNo		= ptRegSrc->dwRegTrackNo;
			tRegBSState.byMediaType		= (BYTE)ptRegSrc->dwRegMType;
			tRegBSState.bySetNonBlock	= 0;
			tRegBSState.szDataPath		= ptRegSrc->szRcvBSPath;

			dwBufSz = MSG_MAX_LENGTH;
			if (MsgData_RegBS_Compose(abyBuf, &dwBufSz, &tRegBSState) != S_OK)
			{
				printf("MsgData_RegBS_Compose error\n");
				return S_FAIL;
			}

			memset(&tMsgState, 0, sizeof(TMsgState));
			tMsgState.usCmd         = GOLD_MSG_UNREG;
			tMsgState.byProcID      = GOLD_PROC_RTSPS;
			tMsgState.byMsgID       = 0x01;
			tMsgState.byDataType    = 0x00;
			tMsgState.szAckPath     = NULL;
			tMsgState.pbyData       = abyBuf;
			tMsgState.dwDataLen     = dwBufSz;

			if (Msg_Send(ptRegSrc->iSrcFd, &tMsgState, GOLD_CMDPATH_BSD) <= 0)
			{
				printf("Msg_Send error\n");
				return S_FAIL;
			}
#endif
			break;
		default:
			return S_FAIL;
	}

	return S_OK;
}

SCODE RegSrc_GetUBuffer(HANDLE hRegSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	TRegSrc		*ptRegSrc;
	TUBuffer	*ptUBuffer;
	BYTE		*pbyBuf;
	int			iRet;

	ptRegSrc			= (TRegSrc *)hRegSrc;
	ptRegSrc->tReadfds	= ptRegSrc->tReadfdsRec;
	pbyBuf				= ptRegSrc->pbyBuf;

	iRet = select(ptRegSrc->iSrcFd+1, &ptRegSrc->tReadfds, NULL, NULL, NULL);
	if ((iRet > 0) && FD_ISSET(ptRegSrc->iSrcFd, &ptRegSrc->tReadfds))
	{
		iRet = recv(ptRegSrc->iSrcFd, pbyBuf, ptRegSrc->iBufSz, 0);
		if (iRet > 0)
		{
			ptUBuffer = (TUBuffer *)pbyBuf;

			if (ptUBuffer->dwSize != iRet)
			{
				printf("[%s:%s:%d] recv a broken ubuffer!\n", __func__, __FILE__, __LINE__);
				return S_FAIL;
			}

			if (ptUBuffer->dwDataType == FOURCC_CONF)
			{
				ptRegSrc->dwIsRcvConf = 1;

				ptUBuffInfo->pbyUBuffHdr = pbyBuf;
				ptUBuffInfo->iUBuffHdrLen = ptUBuffer->dwSize;
				ptUBuffInfo->pbyUBuffPayload = NULL;
				ptUBuffInfo->iUBuffPayloadLen = 0;
			}
			else
			{
				ptUBuffInfo->pbyUBuffHdr = pbyBuf;
				ptUBuffInfo->iUBuffHdrLen = sizeof(TUBuffer)+(int)ptUBuffer->dwUserDataSize;
				ptUBuffInfo->pbyUBuffPayload = pbyBuf+ptUBuffInfo->iUBuffHdrLen;
				ptUBuffInfo->iUBuffPayloadLen = ptUBuffer->dwSize-ptUBuffInfo->iUBuffHdrLen;
			}
		
			return S_OK;
		}
	}
	
	return S_FAIL;
}

SCODE RegSrc_ReleaseUBuffer(HANDLE hRegSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	return S_OK;
}

