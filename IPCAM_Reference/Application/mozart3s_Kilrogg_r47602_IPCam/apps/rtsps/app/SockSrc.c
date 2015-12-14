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
 * SockSrc.c
 *
 * \brief
 * A Live Media Source Implementation
 * The media is from unix domain socket (DGRAM) in ubuffer format
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

#include "SockSrc.h"
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

// -- Convention -- start
SCODE SockSrc_Release(HANDLE *phSockSrc);
SCODE SockSrc_GetUBuffer(HANDLE hSockSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE SockSrc_ReleaseUBuffer(HANDLE hSockSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE SockSrc_EventHandler(HANDLE hSockSrc, ELMSrcEventType eType);
SCODE SockSrc_WakeUpToTerminate(HANDLE hSockSrc)
{
	return S_OK;
}

static TLMSrcCBMap	tSockSrcCBMap = 
{
	SockSrc_Release,
	SockSrc_GetUBuffer, 
	SockSrc_ReleaseUBuffer, 
	SockSrc_EventHandler,
	SockSrc_WakeUpToTerminate
};

// -- Convention -- end

typedef struct socket_source
{
	TLiveMediaSrc		tLMSrc;
	int					iSrcFd;
	int					iCmdFiFo;
	fd_set				tReadfdsRec;
	fd_set				tReadfds;
	BYTE				*pbyBuf;
	int					iBufSz;
	int					iIOId;
} TSockSrc;

DWORD getIOID(char *szSockPath)
{
  char * pch;
  char  *tmp;
  DWORD dwIOID = 0; 
  int i = 0;
  
  pch = strtok (szSockPath, "/");
  while (pch != NULL)
  {
	  if (i == 3) 
	  {
		  tmp = strtok (pch, "audio.sck");
		  if(tmp == NULL)
		  {
			  dwIOID = 0;  
		  }
		  else
		  {
			  dwIOID = strtol(tmp,NULL,10);
		  }
		  break;
	  }
	  i++;
	  pch = strtok (NULL, "/");
  }
  return dwIOID;
}

SCODE SockSrc_Initial(HANDLE *phSockSrc, TSockSrcInitOpts *ptInitOpts)
{
	TSockSrc *ptSockSrc;
	CHAR *szRecvSockPathPrefix=NULL;
	CHAR *szRecvSockPath=NULL;
	CHAR *szID=NULL;
	assert(phSockSrc && ptInitOpts);

	ptSockSrc = (TSockSrc *)malloc(sizeof(TSockSrc));
	if (ptSockSrc == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		return S_FAIL;
	}
	memset(ptSockSrc, 0, sizeof(TSockSrc));
	*phSockSrc = ptSockSrc;

	((TLiveMediaSrc *)ptSockSrc)->ptCBMap = &tSockSrcCBMap;

	ptSockSrc->iSrcFd	= -1;
	ptSockSrc->iCmdFiFo	= -1;
	
	if (ptInitOpts->szRecvSockPath == NULL)
	{
		debug_printf("[%s:%s:%d] Invalid sock path %s!\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szRecvSockPath);
		goto error_handle;
	}
	
	if (strchr(ptInitOpts->szRecvSockPath,'@') != NULL)
	{
	    szRecvSockPathPrefix = strtok(ptInitOpts->szRecvSockPath, "@");
	    szID = strtok(NULL,"@");
	    if (szID != NULL)
	    {
			ptSockSrc->iIOId = strtol(szID, (char **)NULL,10);
			szRecvSockPath = (CHAR *)malloc(sizeof(CHAR)*(strlen(szRecvSockPathPrefix)+2));
			memset(szRecvSockPath, 0x0, (strlen(szRecvSockPathPrefix)+2));
			snprintf(szRecvSockPath, (strlen(szRecvSockPathPrefix)+2), "%s%d", szRecvSockPathPrefix, ptSockSrc->iIOId);
	    }
	    else
	    {		
			ptSockSrc->iIOId = 0;
	    }
	    if ((ptSockSrc->iSrcFd=UnixDgram_CreateSock(szRecvSockPath)) < 0)
	    {
		    debug_printf("[%s:%s:%d] UnixDgram_CreateSock error!\n", 
						    __func__, __FILE__, __LINE__);
		    goto error_handle;
	    }
	    debug_printf("[SockSrc](%d)socket path=%s\n",__LINE__,szRecvSockPath);
	    if (szRecvSockPath != NULL)
	    {
			free(szRecvSockPath);
			szRecvSockPath=NULL;
	    }
	}
	else
	{
	    debug_printf("[SockSrc](%d)socket path=%s\n",__LINE__,ptInitOpts->szRecvSockPath);
	    if ((ptSockSrc->iSrcFd=UnixDgram_CreateSock(ptInitOpts->szRecvSockPath)) < 0)
	    {
		    debug_printf("[%s:%s:%d] UnixDgram_CreateSock error!\n", 
						    __func__, __FILE__, __LINE__);
		    goto error_handle;
	    }
	    ptSockSrc->iIOId=getIOID(ptInitOpts->szRecvSockPath);
	    debug_printf("[SockSrc](%d)IOID=%d\n",__LINE__,ptSockSrc->iIOId);
	}    
	    
	
	ptSockSrc->iBufSz = MAX_MP4V_UBUFFER;
	ptSockSrc->pbyBuf = (BYTE *)malloc(ptSockSrc->iBufSz);
	if (ptSockSrc->pbyBuf == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	// Create command fifo
	if (ptInitOpts->szCmdFiFoPath != NULL)
	{
		if(access(ptInitOpts->szCmdFiFoPath, F_OK) == -1)
		{
			if (mkfifo(ptInitOpts->szCmdFiFoPath, 0777) != 0)
			{
				debug_printf("[%s:%s:%d] Could not create fifo %s\n", 
								__func__, __FILE__, __LINE__, 
								ptInitOpts->szCmdFiFoPath);
				goto error_handle;
			}
		}

		ptSockSrc->iCmdFiFo = open(ptInitOpts->szCmdFiFoPath, O_WRONLY);//|O_NONBLOCK);
		if(ptSockSrc->iCmdFiFo < 0)
		{
			debug_printf("[%s:%s:%d] open fifo %s failed: %d\r\n", 
							__func__, __FILE__, __LINE__, 
							ptInitOpts->szCmdFiFoPath, errno);
			goto error_handle;
		}
		debug_printf("[%s:%s:%d] open fifo %s ok: %d\r\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szCmdFiFoPath, ptSockSrc->iCmdFiFo);
	}

	FD_ZERO(&ptSockSrc->tReadfdsRec);
	FD_SET(ptSockSrc->iSrcFd, &ptSockSrc->tReadfdsRec);
	FD_ZERO(&ptSockSrc->tReadfds);

	return S_OK;
error_handle:

	SockSrc_Release(phSockSrc);
	return S_FAIL;
}

SCODE SockSrc_Release(HANDLE *phSockSrc)
{
	TSockSrc *ptSockSrc;

	ptSockSrc = (TSockSrc *)*phSockSrc;

	if (ptSockSrc->iSrcFd >= 0)
	{
		close(ptSockSrc->iSrcFd);
	}
	if (ptSockSrc->iCmdFiFo >= 0)
	{
		close(ptSockSrc->iCmdFiFo);
	}
	if (ptSockSrc->pbyBuf != NULL)
	{
		free(ptSockSrc->pbyBuf);
	}

	free(ptSockSrc);
	*phSockSrc = NULL;

	return S_OK;
}

//#define CONTROL_MSG_START			"<control id=\"0\"><output>start</output></control>"
//#define CONTROL_MSG_STOP			"<control id=\"0\"><output>stop</output></control>"
//#define CONTROL_MSG_FORCECI			"<control id=\"0\"><forceCI/></control>"
//#define CONTROL_MSG_FORCEINTRA		"<control id=\"0\"><forceIntra/></control>"

#define CONTROL_MSG_START		"<output>start</output>"
#define CONTROL_MSG_STOP		"<output>stop</output>"	
#define CONTROL_MSG_FORCECI		"<forceCI/>"	
#define CONTROL_MSG_FORCEINTRA		"<forceIntra/>"	
#define CONTROL_MSG_LEN 		128
// Begin - Socket Source related functions
SCODE SockSrc_EventHandler(HANDLE hSockSrc, ELMSrcEventType eType)
{
	TSockSrc	*ptSockSrc;
	
	//CHAR		acTemp[100];
	CHAR		acTemp[512];
	int iLen=0;
	CHAR szMsg[CONTROL_MSG_LEN];
	ptSockSrc = (TSockSrc *)hSockSrc;
	
	
	memset(szMsg,0x0,CONTROL_MSG_LEN);
	
	switch(eType)
	{
		case letNeedIntra:
			printf("Need Intra\n");
			if (ptSockSrc->iCmdFiFo >= 0)
			{
			  #if 0
				acTemp[1] = sprintf(acTemp+2, CONTROL_MSG_FORCEINTRA);
				acTemp[0] = 0x02;
				write(ptSockSrc->iCmdFiFo, acTemp, acTemp[1]+2);
				printf("write command %d bytes: %s\r\n", acTemp[1], acTemp+2);
			  #endif
				
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;
				snprintf(szMsg,CONTROL_MSG_LEN,"<control id=\"%d\">%s</control>", ptSockSrc->iIOId,CONTROL_MSG_FORCEINTRA);
				sprintf(&(acTemp[6]),szMsg );
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptSockSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letNeedConf:
			printf("Need Conf\n");
			if (ptSockSrc->iCmdFiFo >= 0)
			{
			  #if 0
				acTemp[1] = sprintf(acTemp+2, CONTROL_MSG_FORCECI);
				acTemp[0] = 0x02;
				write(ptSockSrc->iCmdFiFo, acTemp, acTemp[1]+2);
				printf("write command %d bytes: %s\r\n", acTemp[1], acTemp+2);
			  #endif
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;
				snprintf(szMsg,CONTROL_MSG_LEN,"<control id=\"%d\">%s</control>", ptSockSrc->iIOId,CONTROL_MSG_FORCECI);
				sprintf(&(acTemp[6]),szMsg );
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptSockSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			  
			  
			}
			break;
		case letBitstrmStart:
			printf("Bitstrm start\n");
			if (ptSockSrc->iCmdFiFo >= 0)
			{
			  #if 0
				acTemp[1] = sprintf(acTemp+2, CONTROL_MSG_START);
				acTemp[0] = 0x02;
				write(ptSockSrc->iCmdFiFo, acTemp, acTemp[1]+2);
				printf("write command %d bytes: %s\r\n", acTemp[1], acTemp+2);
			  #endif
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;
				snprintf(szMsg,CONTROL_MSG_LEN,"<control id=\"%d\">%s</control>", ptSockSrc->iIOId,CONTROL_MSG_START);
				sprintf(&(acTemp[6]),szMsg );
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptSockSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			      
			}
			break;
		case letBitstrmStop:
			printf("Bitstrm stop\n");
			if (ptSockSrc->iCmdFiFo >= 0)
			{
			  #if 0
				acTemp[1] = sprintf(acTemp+2, CONTROL_MSG_STOP);
				acTemp[0] = 0x02;
				write(ptSockSrc->iCmdFiFo, acTemp, acTemp[1]+2);
				printf("write command %d bytes: %s\r\n", acTemp[1], acTemp+2);
			  #endif
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;
				snprintf(szMsg,CONTROL_MSG_LEN,"<control id=\"%d\">%s</control>", ptSockSrc->iIOId,CONTROL_MSG_STOP);
				sprintf(&(acTemp[6]),szMsg );
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(ptSockSrc->iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			  
			}
			break;
		default:
			return S_FAIL;
	}

	return S_OK;
}

SCODE SockSrc_GetUBuffer(HANDLE hSockSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	TSockSrc	*ptSockSrc;
	TUBuffer	*ptUBuffer;
	BYTE		*pbyBuf;
	int			iRet;

	ptSockSrc			= (TSockSrc *)hSockSrc;
	ptSockSrc->tReadfds	= ptSockSrc->tReadfdsRec;
	pbyBuf				= ptSockSrc->pbyBuf;

	iRet = select(ptSockSrc->iSrcFd+1, &ptSockSrc->tReadfds, NULL, NULL, NULL);
	if ((iRet > 0) && FD_ISSET(ptSockSrc->iSrcFd, &ptSockSrc->tReadfds))
	{
		iRet = recv(ptSockSrc->iSrcFd, pbyBuf, ptSockSrc->iBufSz, 0);
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

SCODE SockSrc_ReleaseUBuffer(HANDLE hSockSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	return S_OK;
}

