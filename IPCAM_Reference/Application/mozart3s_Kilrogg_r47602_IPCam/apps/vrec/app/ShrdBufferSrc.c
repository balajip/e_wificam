/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2007-2010 VN Inc. All rights reserved.
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
 *  | VN INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 *
 *******************************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <alloca.h>

#include "ShrdBufferSrc.h"

int ShrdBufferSrc_GetFd(HANDLE hShrdBufSrc)
{
  return ((TShrdBufSrc *)hShrdBufSrc)->iSrcFd;
}
SCODE ShrdBufferSrc_Initial(HANDLE *phShrdBufSrc, TShrdBufSrcInitOpts *ptInitOpts)
{
	TShrdBufSrc					*ptShrdBufSrc;
	TSharedBuffMgrInitOptions	tSBInitOpts;

	assert(phShrdBufSrc && ptInitOpts);

	ptShrdBufSrc = (TShrdBufSrc *)malloc(sizeof(TShrdBufSrc));
	if (ptShrdBufSrc == NULL)
	{
		printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		return S_FAIL;
	}
	memset(ptShrdBufSrc, 0, sizeof(TShrdBufSrc));
	*phShrdBufSrc = ptShrdBufSrc;
	


	ptShrdBufSrc->iSrcFd	= -1;
	ptShrdBufSrc->iCmdFiFo	= -1;
	
	if ((ptInitOpts->szSharedBuffer == NULL) ||
		(sscanf(ptInitOpts->szSharedBuffer, "/dev/buff_mgr%u", &tSBInitOpts.dwMinorNum) != 1))
	{
		printf("[%s:%s:%d] Invalid Shared Buffer device %s!\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szSharedBuffer);
		goto error_handle;
	}
	tSBInitOpts.dwVersion		= SHAREDBUFFMGR_VERSION;
	tSBInitOpts.pObjectMem		= NULL;
	tSBInitOpts.eProcessRole	= esbmprReader;

	if (SharedBuffMgr_Initial(&ptShrdBufSrc->hSharedBuffMgr, &tSBInitOpts) != S_OK)
	{
		printf("[%s:%s:%d] SharedBuffMgr_Initial error!!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}
	if (SharedBuffMgr_GetFileDescriptor(ptShrdBufSrc->hSharedBuffMgr, &ptShrdBufSrc->iSrcFd) != S_OK)
	{
		printf("[%s:%s:%d] SharedBuffMgr_GetFileDescriptor error!!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

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
		printf("[%s:%s:%d] path name is %s, channel no is %d\n", 
						__func__, __FILE__, __LINE__, szCmdFiFoPath, ptShrdBufSrc->iChNo);

		if(access(szCmdFiFoPath, F_OK) == -1)
		{
			if (mkfifo(szCmdFiFoPath, 0777) != 0)
			{
				printf("[%s:%s:%d] Could not create fifo %s\n", 
								__func__, __FILE__, __LINE__, szCmdFiFoPath);
				goto error_handle;
			}
		}

		ptShrdBufSrc->iCmdFiFo = open(szCmdFiFoPath, O_WRONLY);//|O_NONBLOCK);
		if(ptShrdBufSrc->iCmdFiFo < 0)
		{
			printf("[%s:%s:%d] open fifo %s failed: %d\r\n", 
							__func__, __FILE__, __LINE__, 
							szCmdFiFoPath, errno);
			goto error_handle;
		}
		printf("[%s:%s:%d] open fifo %s ok: %d\r\n", 
						__func__, __FILE__, __LINE__, 
						szCmdFiFoPath, ptShrdBufSrc->iCmdFiFo);
	}

	//FD_ZERO(&ptShrdBufSrc->tReadfdsRec);
	//FD_SET(ptShrdBufSrc->iSrcFd, &ptShrdBufSrc->tReadfdsRec);
	//FD_ZERO(&ptShrdBufSrc->tReadfds);



	return S_OK;
error_handle:

	ShrdBufferSrc_Release(phShrdBufSrc);
	return S_FAIL;
}

SCODE ShrdBufferSrc_Release(HANDLE *phShrdBufSrc)
{
	TShrdBufSrc *ptShrdBufSrc;

	ptShrdBufSrc = (TShrdBufSrc *)*phShrdBufSrc;

	if (ptShrdBufSrc->iCmdFiFo >= 0)
	{
		close(ptShrdBufSrc->iCmdFiFo);
	}
	if (ptShrdBufSrc->hSharedBuffMgr != NULL)
	{
		SharedBuffMgr_Release(&ptShrdBufSrc->hSharedBuffMgr);
	}
	free(ptShrdBufSrc);
	*phShrdBufSrc = NULL;

	return S_OK;
}

SCODE ShrdBufferSrc_GetUBuffer(HANDLE hShrdBufSrc, TSrcUBufInfo *ptUBuffInfo)
{
  	TShrdBufSrc	*ptShrdBufSrc;
	TUBuffer	*ptUBuffer;
//	int			iRet;
	
	DWORD   	dwBsBuffOffset = 0;
	WORD		wHdrTag = 0;
	WORD		wHdrLen = 0;
	
	ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;

	ptShrdBufSrc->tReadfds = ptShrdBufSrc->tReadfdsRec;
	//printf("ptShrdBufSrc->iSrcFd=%d\n",ptShrdBufSrc->iSrcFd);
	//iRet = select(ptShrdBufSrc->iSrcFd+1, &ptShrdBufSrc->tReadfds, NULL, NULL, NULL);
	//if ((iRet > 0) && FD_ISSET(ptShrdBufSrc->iSrcFd, &ptShrdBufSrc->tReadfds))
	//{
		if (SharedBuffMgr_GetBuffer(ptShrdBufSrc->hSharedBuffMgr, &ptShrdBufSrc->tSBState) == S_OK)
		{
			ptUBuffer = (TUBuffer *)ptShrdBufSrc->tSBState.pbyHdrAddr;

			if (ptUBuffer->dwDataType == FOURCC_CONF)
			{
				//printf("[%s:%s:%d]Get ubuffer conf\n",__func__, __FILE__, __LINE__);
				ptUBuffInfo->pbyUBuffHdr = ptShrdBufSrc->tSBState.pbyHdrAddr;
				ptUBuffInfo->iUBuffHdrLen = ptUBuffer->dwSize;
				ptUBuffInfo->pbyUBuffPayload = NULL;
				ptUBuffInfo->iUBuffPayloadLen = 0;
			}
			else
			{
				//printf("[%s:%s:%d]Get ubuffer bitstream\n",__func__, __FILE__, __LINE__);
				ptUBuffInfo->pbyUBuffHdr = ptShrdBufSrc->tSBState.pbyHdrAddr;
				ptUBuffInfo->iUBuffHdrLen = sizeof(TUBuffer)+(int)ptUBuffer->dwUserDataSize;
				
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
				
				ptUBuffInfo->pbyUBuffPayload = ptShrdBufSrc->tSBState.pbyDataAddr+dwBsBuffOffset;
				ptUBuffInfo->iUBuffPayloadLen = ptUBuffer->dwSize-ptUBuffInfo->iUBuffHdrLen;
				//printf("[Get ubuffer bitstream] Time stamp = %u:%u \n", ptUBuffer->dwSec, ptUBuffer->dwUSec);
			}
			return S_OK;
		}
	//}

	return S_FAIL;


}
SCODE ShrdBufferSrc_ReleaseUBuffer(HANDLE hShrdBufSrc, TSrcUBufInfo *ptUBuffInfo)
{
	TShrdBufSrc	*ptShrdBufSrc;
	ptShrdBufSrc = (TShrdBufSrc *)hShrdBufSrc;
	SharedBuffMgr_ReleaseBuffer(ptShrdBufSrc->hSharedBuffMgr, &ptShrdBufSrc->tSBState);
	
	return S_OK;

}
#define CONTROL_MSG_START           "<content>start</content>"
#define CONTROL_MSG_STOP            "<content>stop</content>"
#define CONTROL_MSG_FORCECI         "<content>forceCI</content>"
#define CONTROL_MSG_FORCEINTRA      "<content>forceIntra</content>"
#define CONTROL_MSG_PREFIX			"<control><request>"
#define CONTROL_MSG_POSTFIX			"</request></control>"
SCODE ShrdBufferSrc_EventHandler(HANDLE hShrdBufSrc, ESrcEventType eType)
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
				//printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
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
				//printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);


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
				//printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
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
				//printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		default:
			return S_FAIL;
	}

	return S_OK;


}
