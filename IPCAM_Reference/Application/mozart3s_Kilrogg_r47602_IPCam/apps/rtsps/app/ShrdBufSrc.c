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
 * ShrdBufSrc.c
 *
 * \brief
 * A Live Media Source Implementation
 * The media is from SharedBuffMgr in ubuffer format
 *
 * \date
 * 2008/10/08
 *
 * \author
 * DJhow.Tu
 *
 *
 *******************************************************************************
 */

#ifdef _ENABLE_SHAREDBUFF_
#include "ShrdBufSrc.h"
#include "LiveMediaSrc_protect.h"
#include "ubuffer.h"
#include "ubuffer_limit.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <alloca.h>
#include "SharedBuffMgr.h"

#ifdef USE_NEW_IPC
#include <sync_ring_buffer.h>
#define UBUFFER_HEADERSIZE 256
#endif

SCODE ShrdBufSrc_Release(HANDLE *phShrdBufSrc);
SCODE ShrdBufSrc_GetUBuffer(HANDLE hShrdBufSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE ShrdBufSrc_ReleaseUBuffer(HANDLE hShrdBufSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE ShrdBufSrc_EventHandler(HANDLE hShrdBufSrc, ELMSrcEventType eType);
SCODE ShrdBufSrc_WakeUpToTerminate(HANDLE hShrdBufSrc);

static TLMSrcCBMap	tShrdBufSrcCBMap = 
{
	ShrdBufSrc_Release,
	ShrdBufSrc_GetUBuffer, 
	ShrdBufSrc_ReleaseUBuffer, 
	ShrdBufSrc_EventHandler,
	ShrdBufSrc_WakeUpToTerminate
};
// -- Convention -- end

typedef struct sharedbuffer_source
{
	TLiveMediaSrc		tLiveMediaSrc;
	int					iCmdFiFo;
	int					iChNo;
#ifdef USE_NEW_IPC
    srb_handle_t*       srb_handle;
    srb_buffer_t        srb_buf;
#else
	int                 iSrcFd;
	fd_set				tReadfdsRec;
	fd_set				tReadfds;
	HANDLE				hSharedBuffMgr;
	TSharedBuffMgrState	tSBState;
#endif
} TShrdBufSrc;

extern int g_iCommand;

SCODE ShrdBufSrc_Initial(HANDLE *phShrdBufSrc, TShrdBufSrcInitOpts *ptInitOpts)
{
	TShrdBufSrc					*ptShrdBufSrc;
	TSharedBuffMgrInitOptions	tSBInitOpts;

	assert(phShrdBufSrc && ptInitOpts);

	ptShrdBufSrc = (TShrdBufSrc *)malloc(sizeof(TShrdBufSrc));
	if (ptShrdBufSrc == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		return S_FAIL;
	}
	memset(ptShrdBufSrc, 0, sizeof(TShrdBufSrc));
	*phShrdBufSrc = ptShrdBufSrc;
	
	((TLiveMediaSrc *)ptShrdBufSrc)->ptCBMap = &tShrdBufSrcCBMap;

	if ((ptInitOpts->szSharedBuffer == NULL) ||
		(sscanf(ptInitOpts->szSharedBuffer, "/dev/buff_mgr%u", &tSBInitOpts.dwMinorNum) != 1))
	{
		debug_printf("[%s:%s:%d] Invalid Shared Buffer device %s!\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szSharedBuffer);
		goto error_handle;
	}
#ifdef USE_NEW_IPC
	{
		char srb_name[64];
		sprintf(srb_name, "venc_srb_%d", tSBInitOpts.dwMinorNum);
		ptShrdBufSrc->srb_handle = SRB_Reader(srb_name);
		memset(&ptShrdBufSrc->srb_buf, 0, sizeof(srb_buffer_t));
	}
#else
	ptShrdBufSrc->iSrcFd    = -1;
	tSBInitOpts.dwVersion		= SHAREDBUFFMGR_VERSION;
	tSBInitOpts.pObjectMem		= NULL;
	tSBInitOpts.eProcessRole	= esbmprReader;

	if (SharedBuffMgr_Initial(&ptShrdBufSrc->hSharedBuffMgr, &tSBInitOpts) != S_OK)
	{
		debug_printf("[%s:%s:%d] SharedBuffMgr_Initial error!!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}
	if (SharedBuffMgr_GetFileDescriptor(ptShrdBufSrc->hSharedBuffMgr, &ptShrdBufSrc->iSrcFd) != S_OK)
	{
		debug_printf("[%s:%s:%d] SharedBuffMgr_GetFileDescriptor error!!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	FD_ZERO(&ptShrdBufSrc->tReadfdsRec);
	FD_SET(ptShrdBufSrc->iSrcFd, &ptShrdBufSrc->tReadfdsRec);
	FD_ZERO(&ptShrdBufSrc->tReadfds);
#endif

	ptShrdBufSrc->iCmdFiFo  = -1;
	// Create command fifo
	if (ptInitOpts->szCmdFiFoPath != NULL)
	{
		CHAR	*pcTmp;
		CHAR	*szCmdFiFoPath;

		if ((pcTmp=strstr(ptInitOpts->szCmdFiFoPath, "@")) != NULL)
		{
			int	iPathLen;

			ptShrdBufSrc->iChNo	= atoi(++pcTmp);
			iPathLen			= pcTmp-ptInitOpts->szCmdFiFoPath-1;
			szCmdFiFoPath		= (CHAR *)alloca(iPathLen+1);
			memcpy(szCmdFiFoPath, ptInitOpts->szCmdFiFoPath, iPathLen);
			szCmdFiFoPath[iPathLen] = '\0';
		}
		else
		{
			ptShrdBufSrc->iChNo	= 0;
			szCmdFiFoPath		= ptInitOpts->szCmdFiFoPath;
		}
		debug_printf("[%s:%s:%d] path name is %s, channel no is %d\n", 
						__func__, __FILE__, __LINE__, szCmdFiFoPath, ptShrdBufSrc->iChNo);

		if(access(szCmdFiFoPath, F_OK) == -1)
		{
			if (mkfifo(szCmdFiFoPath, 0777) != 0)
			{
				debug_printf("[%s:%s:%d] Could not create fifo %s\n", 
								__func__, __FILE__, __LINE__, szCmdFiFoPath);
				goto error_handle;
			}
		}

		while (g_iCommand)
		{
			ptShrdBufSrc->iCmdFiFo = open(szCmdFiFoPath, O_WRONLY|O_NONBLOCK);
		
			if(ptShrdBufSrc->iCmdFiFo > 0)
				break;

			if (errno == ENXIO)
			{
				usleep(1000000);
				continue;
			}

			debug_printf("[%s:%s:%d] open fifo %s failed: %d\r\n", 
					__func__, __FILE__, __LINE__, 
					szCmdFiFoPath, errno);
			goto error_handle;
		}
		debug_printf("[%s:%s:%d] open fifo %s ok: %d\r\n", 
						__func__, __FILE__, __LINE__, 
						szCmdFiFoPath, ptShrdBufSrc->iCmdFiFo);
	}

	return S_OK;
error_handle:

	ShrdBufSrc_Release(phShrdBufSrc);
	return S_FAIL;
}

SCODE ShrdBufSrc_Release(HANDLE *phShrdBufSrc)
{
	TShrdBufSrc *ptShrdBufSrc;
	ptShrdBufSrc = (TShrdBufSrc *)*phShrdBufSrc;

	if (ptShrdBufSrc->iCmdFiFo >= 0)
	{
		close(ptShrdBufSrc->iCmdFiFo);
	}
#ifndef USE_NEW_IPC
	if (ptShrdBufSrc->hSharedBuffMgr != NULL)
	{
		SharedBuffMgr_Release(&ptShrdBufSrc->hSharedBuffMgr);
	}
#endif
	free(ptShrdBufSrc);
	*phShrdBufSrc = NULL;

	return S_OK;
}

#define CONTROL_MSG_START           "<content>start</content>"
#define CONTROL_MSG_STOP            "<content>stop</content>"
#define CONTROL_MSG_FORCECI         "<content>forceCI</content>"
#define CONTROL_MSG_FORCEINTRA      "<content>forceIntra</content>"
#define CONTROL_MSG_PREFIX			"<control><request>"
#define CONTROL_MSG_POSTFIX			"</request></control>"

SCODE ShrdBufSrc_EventHandler(HANDLE hShrdBufSrc, ELMSrcEventType eType)
{
	TShrdBufSrc	*ptShrdBufSrc;
	CHAR		acTemp[512];
	int iLen;
	
	ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;

	switch(eType)
	{
		case letNeedIntra:
			printf("Need Intra\n");
			if (ptShrdBufSrc->iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
									CONTROL_MSG_PREFIX, 
									ptShrdBufSrc->iChNo, 
									CONTROL_MSG_FORCEINTRA,
									CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptShrdBufSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letNeedConf:
			printf("Need Conf\n");
			if (ptShrdBufSrc->iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
									CONTROL_MSG_PREFIX, 
									ptShrdBufSrc->iChNo, 
									CONTROL_MSG_FORCECI,
									CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptShrdBufSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letBitstrmStart:
			printf("Bitstrm start\n");
			if (ptShrdBufSrc->iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
									CONTROL_MSG_PREFIX, 
									ptShrdBufSrc->iChNo, 
									CONTROL_MSG_START,
									CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptShrdBufSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letBitstrmStop:
			printf("Bitstrm stop\n");
			if (ptShrdBufSrc->iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
									CONTROL_MSG_PREFIX, 
									ptShrdBufSrc->iChNo, 
									CONTROL_MSG_STOP,
									CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptShrdBufSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		default:
			return S_FAIL;
	}

	return S_OK;
}

SCODE ShrdBufSrc_GetUBuffer(HANDLE hShrdBufSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	TShrdBufSrc	*ptShrdBufSrc;
	TUBuffer	*ptUBuffer;
	DWORD   	dwBsBuffOffset = 0;
	WORD		wHdrTag = 0;
	WORD		wHdrLen = 0;
	ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;

#ifdef USE_NEW_IPC
	if (!SRB_ReleaseAndReceive(ptShrdBufSrc->srb_handle, &ptShrdBufSrc->srb_buf))
		return S_FAIL;
#else
	ptShrdBufSrc->tReadfds = ptShrdBufSrc->tReadfdsRec;

	if ((select(ptShrdBufSrc->iSrcFd+1, &ptShrdBufSrc->tReadfds, NULL, NULL, NULL) > 0) &&
			FD_ISSET(ptShrdBufSrc->iSrcFd, &ptShrdBufSrc->tReadfds))
	{
		if (SharedBuffMgr_GetBuffer(ptShrdBufSrc->hSharedBuffMgr, &ptShrdBufSrc->tSBState) == S_OK)
		{
#endif

#ifdef USE_NEW_IPC
			ptUBuffer = (TUBuffer *)ptShrdBufSrc->srb_buf.buffer;
#else
			ptUBuffer = (TUBuffer *)ptShrdBufSrc->tSBState.pbyHdrAddr;
#endif

			ptUBuffInfo->pbyUBuffHdr = (BYTE*) ptUBuffer;
			if (ptUBuffer->dwDataType == FOURCC_CONF)
			{
				ptUBuffInfo->iUBuffHdrLen		= ptUBuffer->dwSize;
				ptUBuffInfo->pbyUBuffPayload	= NULL;
				ptUBuffInfo->iUBuffPayloadLen	= 0;
			}
			else
			{
#if 0
				CHAR *pcCodec;

				pcCodec = (CHAR *)&ptUBuffer->dwDataType;
				//printf("Get ubuffer , hdr pointer %p, payload pointer %p\n", ptShrdBufSrc->tSBState.pbyHdrAddr, ptShrdBufSrc->tSBState.pbyDataAddr);
				printf("Get ubuffer bitstream codec %c%c%c%c, isSyc %u\n", pcCodec[0], pcCodec[1], pcCodec[2], pcCodec[3], ptUBuffer->bIsSync);
#endif
				
				ptUBuffInfo->iUBuffHdrLen		= sizeof(TUBuffer)+(int)ptUBuffer->dwUserDataSize;
				
				if (ptUBuffer->dwUserDataSize > 0)
				{
					if (ptUBuffer->dwDataType == FOURCC_MP4V)
					{
						wHdrTag = *((WORD *)(ptUBuffInfo->pbyUBuffHdr+sizeof(TUBuffer)));
						if (wHdrTag == TAG_MP4V_EXTENINFO)
						{
							wHdrLen = *((WORD *)(ptUBuffInfo->pbyUBuffHdr+sizeof(TUBuffer)+sizeof(WORD)));
							dwBsBuffOffset = *((DWORD *)(ptUBuffInfo->pbyUBuffHdr+sizeof(TUBuffer)+wHdrLen));	
						}
					}
				}
				//printf("[ShrdBufSrc](%d)dwBsBuffOffset= %d\n",__LINE__,dwBsBuffOffset);
#ifdef USE_NEW_IPC
				ptUBuffInfo->pbyUBuffPayload    = ptShrdBufSrc->srb_buf.buffer + UBUFFER_HEADERSIZE + dwBsBuffOffset;
#else
				ptUBuffInfo->pbyUBuffPayload	= ptShrdBufSrc->tSBState.pbyDataAddr + dwBsBuffOffset;
#endif
				ptUBuffInfo->iUBuffPayloadLen	= ptUBuffer->dwSize-ptUBuffInfo->iUBuffHdrLen;
//				printf("[Acer debug] Time stamp = %u:%u \n", ptUBuffer->dwSec, ptUBuffer->dwUSec);
			}
			return S_OK;
#ifndef USE_NEW_IPC
		}
	}

	return S_FAIL;
#endif
}

SCODE ShrdBufSrc_ReleaseUBuffer(HANDLE hShrdBufSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
#ifdef USE_NEW_IPC
	if (ptUBuffInfo)
		return S_OK;

	{
		TShrdBufSrc *ptShrdBufSrc;
		ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;
		SRB_Release(ptShrdBufSrc->srb_handle, &ptShrdBufSrc->srb_buf);
	}
#else
	if (ptUBuffInfo)
	{
		TShrdBufSrc	*ptShrdBufSrc;
		ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;
		SharedBuffMgr_ReleaseBuffer(ptShrdBufSrc->hSharedBuffMgr, &ptShrdBufSrc->tSBState);
	}
#endif
	
	return S_OK;
}

SCODE ShrdBufSrc_WakeUpToTerminate(HANDLE hShrdBufSrc)
{
#ifdef USE_NEW_IPC
	TShrdBufSrc *ptShrdBufSrc;
	ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;
	if (ptShrdBufSrc->srb_handle)
		SRB_WakeUpToTerminate(ptShrdBufSrc->srb_handle);
#endif

    return S_OK;
}
#endif // _ENABLE_SHAREDBUFF_
