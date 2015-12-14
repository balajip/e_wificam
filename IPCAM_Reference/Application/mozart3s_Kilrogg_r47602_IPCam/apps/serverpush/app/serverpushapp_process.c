/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2006 VN Inc. All rights reserved.
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
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * serverpushapp_process.c
 *
 * \brief
 * Implement server push process.
 * 
 * \date
 * 2007/01/05
 *
 * \author
 * Jessie Lin
 *
 *******************************************************************************
 */
 
#include "serverpush_local.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/uio.h>
#include "fdipc.h"
#include "dbgdefs.h"

#ifdef _DUMP_

DWORD   g_dwFileNo;

#define DUMP_PATH       "dump/%03d.jpg"
#define MAX_DUMP_NUM    555

#endif //_DUMP_ 

#define HTTP_503_MSG    "HTTP/1.1 503 Service Unavailable\r\n"
#define HTTP_503_CONTENT "<HTML><HEAD><TITLE>503 Service Unavailable</TITLE></HEAD>\n<BODY><H1>503 Service Unavailable</H1>\nThere are too many connections in use right now.\nPlease try again later.\n</BODY></HTML>\n"
#define HTTP_200_MSG    "HTTP/1.1 200 OK\r\n"

#define MAX_BUFFER      1024
#define SELECT_TIMES	30

/* =========================================================================================== */

static void ServerPush_Setfds(TServerPushAppInfo* pThis);
static SCODE ServerPush_fdsCheck(TServerPushAppInfo* pThis);
static SCODE ServerPush_AddClient(TServerPushAppInfo* pThis, DWORD dwTrackNo, int fdCliSck, char *pchIPAddr);
static SCODE ServerPush_RmClient(TServerPushAppInfo* pThis, DWORD dwTrackNo, DWORD dwCliNo);
static inline void ServerPush_fdsSET(TServerPushAppInfo* pThis, fd_set* pfdsModel, int fd);
static inline void ServerPush_fdsCLR(TServerPushAppInfo* pThis, fd_set* pfdsModel, int fd);
static SCODE ServerPush_RcvHTTPMsg(TServerPushAppInfo* pThis);
static SCODE ServerPush_SendToCli(TServerPushAppInfo* pThis, DWORD dwTrackNo, DWORD dwCliNo, TSharedBuffMgrState* tState);
static SCODE ServerPush_SendStartCmdToEnc(TServerPushAppInfo* pThis, DWORD dwTrackNo);


/* =========================================================================================== */

void *ServerPushApp_Process(HANDLE hObject)
{
	TServerPushAppInfo  *pThis = (TServerPushAppInfo *)(hObject);
	TTrackInfo          *ptThisTrack = NULL;
	fd_set               fdsRead;
	int                  iRes = 0, hdr_len = 0;

	DWORD                dwMaxTrack = pThis->dwTrackNum;
	DWORD                dwTrackNo = 0;
	DWORD                dwCliNo = 0;
	TSharedBuffMgrState  tState, tLocalState;
	TUBuffer            *ptUBuf;

	tLocalState.pbyHdrAddr = (BYTE*) malloc(1024);
	tLocalState.pbyDataAddr = (BYTE*) malloc(1024*1024);

	ServerPush_Setfds(pThis);


#ifdef _DUMP_
	g_dwFileNo = 0;
#endif //_DUMP_ 

	DBPRINT0("[SERV_PUSH] server push come Process function ....\n");
	while (1)
	{
		if (pThis->bTerminate == TRUE) break ;

		// ====== select readfds ======
		memcpy((void *)&fdsRead, (void *)&pThis->fdsModelR, sizeof(fd_set));

		iRes = select(pThis->fdMaxSck + 1, &fdsRead, NULL, NULL, NULL);

		if (iRes < 0) 
		{
			switch (errno)
			{
			case EBADF:
				DBPRINT1("[SERV_PUSH]%sselect() EBADF: An invalid file descriptor was given in one of the sets.\n", 
						get_commonlog_time());
				ServerPush_fdsCheck(pThis);
				break;
			case EINTR:
				DBPRINT1("[SERV_PUSH]%sselect() EINTR: A non blocked signal was caught.\n", 
						get_commonlog_time());
				break;
			case EINVAL:
				DBPRINT1("[SERV_PUSH]%sselect() EINVAL: n is negative or the value contained within timeout is  invalid.\n", 
						get_commonlog_time());
				ServerPush_fdsCheck(pThis);
				break;
				//            case ENOMEM:
				//                break;
			default:
				break;
			}
			continue;
		}

		//DBPRINT2("[SERV_PUSH]%sselect() iRes: %d\n", get_commonlog_time(), iRes);
		if (FD_ISSET(pThis->iHttpFdipcSck, &fdsRead))
		{
			ServerPush_RcvHTTPMsg(pThis);
		}
		for (dwTrackNo = 0; dwTrackNo < dwMaxTrack; dwTrackNo++)
		{
			ptThisTrack = &(pThis->tTrackInfo[dwTrackNo]);
			for (dwCliNo = 0; dwCliNo < MAX_CLIENT; dwCliNo++)
			{
				if (!(ptThisTrack->tClients[dwCliNo].fdCliSck < 0))
				{
					if (FD_ISSET(ptThisTrack->tClients[dwCliNo].fdCliSck, &fdsRead))
					{
						// the remote side has closed the connection
						ServerPush_RmClient(pThis, dwTrackNo, dwCliNo);
					}
				} // end if fdCliSck < 0
			} // end for dwCliNo

			if (FD_ISSET(ptThisTrack->fdEnc, &fdsRead)) 
			{
				if (ptThisTrack->hJPEGEncObj != NULL)
				{
					if (SharedBuffMgr_GetBuffer(ptThisTrack->hJPEGEncObj, &tState) == S_OK)
					{
						ptUBuf = (TUBuffer *)tState.pbyHdrAddr;
						// if no client connect to this track, just release sharedbuffer.
						// if this data is not JPEG bitstream, ignore it.
						if (ptThisTrack->dwCliNum == 0 || ptUBuf->dwDataType != FOURCC_JPEG)
						{
							SharedBuffMgr_ReleaseBuffer(ptThisTrack->hJPEGEncObj, &tState);
							continue;
						}

						hdr_len = sizeof(TUBuffer) + ptUBuf->dwUserDataSize;
						memcpy(tLocalState.pbyHdrAddr, tState.pbyHdrAddr, hdr_len);
						memcpy(tLocalState.pbyDataAddr, tState.pbyDataAddr, ptUBuf->dwSize - hdr_len);
						SharedBuffMgr_ReleaseBuffer(ptThisTrack->hJPEGEncObj, &tState);

						for (dwCliNo = 0; dwCliNo < MAX_CLIENT; dwCliNo++)
						{
							if (!(ptThisTrack->tClients[dwCliNo].fdCliSck < 0))
								ServerPush_SendToCli(pThis, dwTrackNo, dwCliNo, &tState);
						} // end for dwCliNo
					}
				}
			}
		} // end for TrackNo
	}

	free(tLocalState.pbyHdrAddr);
	free(tLocalState.pbyDataAddr);
    return 0;
}


/* =========================================================================================== */
static void ServerPush_Setfds(TServerPushAppInfo* pThis)
{
    DWORD dwMaxTrack = pThis->dwTrackNum;
    DWORD dwTrackNo = 0;
	    
	FD_ZERO(&pThis->fdsModelR);

    // ====== set fd to HTTP server ======
    ServerPush_fdsSET(pThis, &pThis->fdsModelR, pThis->iHttpFdipcSck);
    
    // ====== set fd to MJPEG encoder ======
	for (dwTrackNo = 0; dwTrackNo < dwMaxTrack; dwTrackNo++)
	{
		pThis->tTrackInfo[dwTrackNo].hJPEGEncObj = NULL;
		//SharedBuffMgr_GetFileDescriptor(pThis->tTrackInfo[dwTrackNo].hJPEGEncObj, &pThis->tTrackInfo[dwTrackNo].fdEnc);
	    //ServerPush_fdsSET(pThis, &pThis->fdsModelR, pThis->tTrackInfo[dwTrackNo].fdEnc);
    }
}

/* =========================================================================================== */
static SCODE ServerPush_fdsCheck(TServerPushAppInfo* pThis)
{
    TTrackInfo *pThisTrack = NULL;
    DWORD       dwMaxTrack = pThis->dwTrackNum;
    DWORD       dwTrackNo = 0;
    DWORD       dwCliNo = 0;
    DWORD       dwCliNum = 0;
    struct stat statbuf; 
    
    
    // select error, check all fds
    
    // ====== check fd to HTTP server ======
    if (fstat(pThis->iHttpFdipcSck, &statbuf) != 0)
    {
        // Bad fd to HTTP server, reset socket server
        ServerPush_fdsCLR(pThis, &pThis->fdsModelR, pThis->iHttpFdipcSck);
        if (ServerPush_SetHTTPSck(pThis) != S_OK)
        {
            fprintf(stderr, "[SERV_PUSH]%sReset HTTP server faild\n", get_commonlog_time());
            return S_FAIL;
        }
    }
    
    // ====== check each track's r/w fds ======
    for (dwTrackNo = 0; dwTrackNo < dwMaxTrack; dwTrackNo++)
	{
	    pThisTrack = &pThis->tTrackInfo[dwTrackNo];
	    
	    // ====== check fd to MJPEG encoder ======
//        if (fstat(pThisTrack->fdEncSck, &statbuf) !=0)
//        {
            // Bad fd to Encoder, reset file descriptor
//            ServerPush_fdsCLR(pThis, &pThis->fdsModelR, pThisTrack->fdEncSck);
//            ServerPush_SetMPEGEncSck(pThis, dwTrackNo);
//            return S_FAIL;
//        }
        
        // ====== check fd to clients for each track ======
        for (dwCliNo = 0; dwCliNo < MAX_CLIENT; dwCliNo++)
        {
            if (pThisTrack->tClients[dwCliNo].fdCliSck < 0) continue;
            
            if (fstat(pThisTrack->tClients[dwCliNo].fdCliSck, &statbuf) !=0)
            {
                // Bad fd to client, removd client
                ServerPush_RmClient(pThis, dwTrackNo, dwCliNo);
            }
            else
            {
                dwCliNum++;
            }
        }
        if (dwCliNum != pThisTrack->dwCliNum)
        {
            DBPRINT2("[SERV_PUSH]%sRest Clinum %d\n", get_commonlog_time(), dwCliNum);
        }
    }
    return S_OK;
}

/* =========================================================================================== */
static SCODE ServerPush_AddClient(TServerPushAppInfo* pThis, DWORD dwTrackNo, int fdCliSck, char *pchIPAddr)
{
    TTrackInfo  *pThisTrack = &pThis->tTrackInfo[dwTrackNo];
    DWORD        dwCliNo = 0;
	CHAR szBoundary[MAX_BUFFER];
    struct iovec iov[3];
	struct msghdr MsgHeader;
    
    //ServerPush_fdsCheck(pThis);
    
    // check client count
    if (pThisTrack->dwCliNum == MAX_CLIENT)
    {
    	fprintf(stderr, 
                "[SERV_PUSH]%sExceed max client %d, connection refused\n", 
                get_commonlog_time(), MAX_CLIENT);
	   	sprintf(szBoundary, 
           "Content-Type: text/html\r\n"
           "Content-Length: %d\r\n\r\n", 
           strlen(HTTP_503_CONTENT));

        // feed HTTP/1.1 503 and close
		MsgHeader.msg_name = NULL;
		MsgHeader.msg_namelen = 0;
		MsgHeader.msg_iov = iov;
		MsgHeader.msg_iovlen = 3;
		MsgHeader.msg_control = NULL;
		MsgHeader.msg_controllen = 0;
		MsgHeader.msg_flags = 0;
						
	 	iov[0].iov_base	= HTTP_503_MSG;
	   	iov[0].iov_len  = strlen(HTTP_503_MSG);
		iov[1].iov_base = szBoundary;
		iov[1].iov_len  = strlen(szBoundary);
		iov[2].iov_base = HTTP_503_CONTENT;
		iov[2].iov_len  = strlen(HTTP_503_CONTENT);

	    sendmsg(fdCliSck, &MsgHeader, 0);
        close(fdCliSck);                
        return S_FAIL;
#ifdef _HTTP_SERVER_LIGHTHTTP_     
    } else {
		sprintf(szBoundary, 
           "Content-Type: multipart/x-mixed-replace;boundary=myboundary\r\n\r\n"
           );

        // feed HTTP/1.1 503 and close
		MsgHeader.msg_name = NULL;
		MsgHeader.msg_namelen = 0;
		MsgHeader.msg_iov = iov;
		MsgHeader.msg_iovlen = 2;
		MsgHeader.msg_control = NULL;
		MsgHeader.msg_controllen = 0;
		MsgHeader.msg_flags = 0;
						
	 	iov[0].iov_base	= HTTP_200_MSG;
	   	iov[0].iov_len  = strlen(HTTP_200_MSG);
		iov[1].iov_base = szBoundary;
		iov[1].iov_len  = strlen(szBoundary);
	    sendmsg(fdCliSck, &MsgHeader, 0);
#endif
	}
    
    // find an empty slot
    for (dwCliNo = 0; dwCliNo < MAX_CLIENT; dwCliNo++)
    {
        if (pThisTrack->tClients[dwCliNo].fdCliSck < 0) break;
    }
    
    if (dwCliNo == MAX_CLIENT) 
    {
        // TODO : means dwCliNum error check FD?
        return S_FAIL;
    }
    else
    {
        memset((void *)&pThisTrack->tClients[dwCliNo], 0, sizeof(TClientInfo));
        pThisTrack->tClients[dwCliNo].fdCliSck = fdCliSck;
        strncpy(pThisTrack->tClients[dwCliNo].aszIPAddr, pchIPAddr, sizeof(pThisTrack->tClients[dwCliNo].aszIPAddr));
    }
    
    // for check the connection
    ServerPush_fdsSET(pThis, &pThis->fdsModelR, fdCliSck);

    // update client count
    pThisTrack->dwCliNum++;
	
	if (pThisTrack->dwCliNum == 1)
	{
		ServerPush_SendStartCmdToEnc(pThis, dwTrackNo);
		if (pThisTrack->hJPEGEncObj == NULL)
		{
			ServerPush_InitSharedBuffMgr(pThis, dwTrackNo);
			SharedBuffMgr_GetFileDescriptor(pThisTrack->hJPEGEncObj, &pThisTrack->fdEnc);
			ServerPush_fdsSET(pThis, &pThis->fdsModelR, pThisTrack->fdEnc);
		}
	}
    return S_OK;
}

/* =========================================================================================== */
static SCODE ServerPush_RmClient(TServerPushAppInfo* pThis, DWORD dwTrackNo, DWORD dwCliNo)
{
    TTrackInfo  *pThisTrack = &pThis->tTrackInfo[dwTrackNo];
    TClientInfo *pThisClient = &pThisTrack->tClients[dwCliNo];

    ServerPush_fdsCLR(pThis, &pThis->fdsModelR, pThisClient->fdCliSck);

    // reset client information
    if (pThisClient->pbyBase != NULL)
    {
        free(pThisClient->pbyBase);
        pThisClient->pbyBase = NULL;
    }
    // added at 2008/09/04
    close(pThisClient->fdCliSck);
    pThisClient->fdCliSck = -1;
    
    // update client count
    pThisTrack->dwCliNum--;
    
    printf("[SERV_PUSH][%s]%strack%02u remains %u client\n", 
            __FUNCTION__, get_commonlog_time(), dwTrackNo, pThis->tTrackInfo[dwTrackNo].dwCliNum);  
    if (pThisTrack->dwCliNum ==0)
    {
		ServerPush_SendStopCmdToEnc(pThis, dwTrackNo);
		if (pThisTrack->hJPEGEncObj != NULL)
		{
			SharedBuffMgr_GetFileDescriptor(pThisTrack->hJPEGEncObj, &pThisTrack->fdEnc);
			ServerPush_fdsCLR(pThis, &pThis->fdsModelR, pThisTrack->fdEnc);
			SharedBuffMgr_Release(&pThisTrack->hJPEGEncObj);
			pThisTrack->hJPEGEncObj = NULL;
		}
    }
    return S_OK;
}

/* =========================================================================================== */
static inline void ServerPush_fdsSET(TServerPushAppInfo* pThis, fd_set* pfdsModel, int fd)
{
    // set fd to fd_set model and update pThis->fdMaxSck
    FD_SET(fd, pfdsModel);
    pThis->fdMaxSck = (fd > pThis->fdMaxSck) ? fd : pThis->fdMaxSck;
}

/* =========================================================================================== */
static inline void ServerPush_fdsCLR(TServerPushAppInfo* pThis, fd_set* pfdsModel, int fd)
{
    int fdMaxSck = pThis->fdMaxSck;
    
    // clear fd in fd_set model and update pThis->fdMaxSck
    FD_CLR(fd, pfdsModel);
    if (fd == fdMaxSck)
    {
        do
        {
            fdMaxSck--;
        }
        while ((fdMaxSck >= 0) && !FD_ISSET(fdMaxSck , &pThis->fdsModelR));
        pThis->fdMaxSck = fdMaxSck;
    }
}

/* =========================================================================================== */
static SCODE ServerPush_RcvHTTPMsg(TServerPushAppInfo* pThis)
{
    int          fdCliSck;
    int          iReadSz;
    BYTE         abyBuf[MAX_BUFFER];
    DWORD        dwMaxTrack = pThis->dwTrackNum;
    DWORD        dwTrackNo = 0;
    
    // ====== read msg form HTTP server ======
    iReadSz = fdipc_recv(pThis->iHttpFdipcSck, abyBuf, MAX_BUFFER, &fdCliSck);
    
    if (iReadSz <= 0) return S_FAIL;
    
#ifdef _DEBUG
    printf("[SERV_PUSH][%s]%siHttpCliSck = %d, iReadSz = %d, abyBuf = %s\n", 
           __FUNCTION__, get_commonlog_time(), fdCliSck, iReadSz, abyBuf);
    int i;
    for (i = 0; i < iReadSz; i++)
    {
        printf("%c", abyBuf[i]);
        if (abyBuf[i]=='\0')
            printf("\n");
    }
    printf("\n");
#endif //_DEBUG
    
    // ====== check the requested track number ======
	for (dwTrackNo = 0; dwTrackNo < dwMaxTrack; dwTrackNo++)
    {
#ifdef _HTTP_SERVER_LIGHTHTTP_
        if ((pThis->tTrackInfo[dwTrackNo].szURI) &&
            (strstr((CHAR *)(abyBuf), pThis->tTrackInfo[dwTrackNo].szURI) != NULL)) {
            break;
        }
#else
        if ((pThis->tTrackInfo[dwTrackNo].szURI) &&
            (!strcmp((CHAR *)(abyBuf+1), pThis->tTrackInfo[dwTrackNo].szURI))) {
            break;
        }
#endif
	}

    if (dwTrackNo == dwMaxTrack)
    {
        fprintf(stderr, "[SERV_PUSH]%sError uri: %s\n", get_commonlog_time(), abyBuf);
        return S_FAIL;
    }

	{
#ifdef _HTTP_SERVER_LIGHTHTTP_
		char *pchIPAddr = "172.17.4.36";    // a fake IP, because I can't query client IP address from lighthttp.
#else
		char *pchTemp = NULL;
        char *pchIPAddr = NULL;
		pchTemp = abyBuf;
		pchTemp = pchTemp + strlen(pchTemp) + 1;	//ignore videoX.mjpg

		pchTemp = strstr(pchTemp, "&http_remote_addr");
		pchTemp = strstr(pchTemp, "=");
		pchIPAddr = pchTemp+1; 

		pchTemp = strstr(pchIPAddr, "&");
		*pchTemp = '\0';
#endif
        // ====== add client ======
        if (ServerPush_AddClient(pThis, dwTrackNo, fdCliSck, pchIPAddr) == S_OK)
        {
            printf("[SERV_PUSH]%strack%02u: has %u client\n", 
                    get_commonlog_time(), dwTrackNo, pThis->tTrackInfo[dwTrackNo].dwCliNum);    
        }

	}

    return S_OK;
}

/* =========================================================================================== */
static SCODE ServerPush_SendToCli(TServerPushAppInfo* pThis, DWORD dwTrackNo, DWORD dwCliNo, TSharedBuffMgrState* tState)
{
    TTrackInfo  *pThisTrack  = &pThis->tTrackInfo[dwTrackNo];
    TClientInfo *pThisClient = &pThisTrack->tClients[dwCliNo];
    DWORD dwTotalSendSz = 0;
    DWORD dwTotalDataSz = 0;
    BOOL bShouldSend = TRUE;
    CHAR szBoundary[MAX_BUFFER];
    CHAR *pEnding = "\r\n\0";    
    struct iovec iov[3];
	struct msghdr MsgHeader;
    DWORD dwHeaderSz = 0;
    int iWriteSz = 0;

    BOOL bNeedSelect = FALSE;
    DWORD dwSelectTimes = 0;

    fd_set               fdsWrite;
    fd_set               fdsRead;
    struct timeval       timeout;
    int                  iRes = 0;
		
	// write a new frame form read buffer    
	if (pThisClient->dwSeqNo == ((TUBufferInfo *)tState->pbyHdrAddr)->tuBuf.dwSeqNo)
	{
		// no new frame to send
		return S_OK;
	}

   // write header
   sprintf(szBoundary, 
           "--%s\r\n"
           "Content-Type: image/jpeg\r\n"
           "Content-Length: %u\r\n\r\n", 
           pThis->szBoundary, ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize + 2);  // add "\r\n"			
    dwHeaderSz = strlen(szBoundary);
	dwTotalDataSz = dwHeaderSz + ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize + 2;
#ifdef _DEBUG
	printf("dwJPEGSize = %u ..", ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize);
#endif //_DEBUG	
    while (bShouldSend)
    {
    	while (bNeedSelect)
    	{
			FD_ZERO(&fdsWrite);
			FD_ZERO(&fdsRead);
			FD_SET(pThisClient->fdCliSck, &fdsWrite);
			FD_SET(pThisClient->fdCliSck, &fdsRead);
	        timeout.tv_sec = 1;
	        timeout.tv_usec = 0;
	        dwSelectTimes++;
	        if (dwSelectTimes > SELECT_TIMES)
				return ServerPush_RmClient(pThis, dwTrackNo, dwCliNo);      	
	        iRes = select(pThisClient->fdCliSck + 1, &fdsRead, &fdsWrite, NULL, &timeout);	        
	        if (iRes < 0) 
	        {
	            switch (errno)
	            {
	            case EBADF:
	                DBPRINT1("[SERV_PUSH]%sselect() EBADF: An invalid file descriptor was given in one of the sets.\n", 
	                         get_commonlog_time());
	                ServerPush_fdsCheck(pThis);
	                break;
	            case EINTR:
	                DBPRINT1("[SERV_PUSH]%sselect() EINTR: A non blocked signal was caught.\n", 
	                         get_commonlog_time());
	                break;
	            case EINVAL:
	                DBPRINT1("[SERV_PUSH]%sselect() EINVAL: n is negative or the value contained within timeout is  invalid.\n", 
	                         get_commonlog_time());
	                ServerPush_fdsCheck(pThis);
	                break;
	//            case ENOMEM:
	//                break;
	            default:
	                break;
	            }
	            continue;
	        }
	        else if(iRes == 0)
	        {
	            DBPRINT0("[SERV_PUSH]select() time out\n");
	            continue;
	        }
	        if (FD_ISSET(pThisClient->fdCliSck, &fdsRead))
	        {
	        	return ServerPush_RmClient(pThis, dwTrackNo, dwCliNo);
	        }	        
	        if (FD_ISSET(pThisClient->fdCliSck, &fdsWrite))
	        {
	        	bNeedSelect = FALSE;
	        	dwSelectTimes = 0;
	            break;
	        }
	        
    	}
    	if (dwTotalSendSz < dwHeaderSz) // header + data + padding
    	{
			MsgHeader.msg_name = NULL;
			MsgHeader.msg_namelen = 0;
			MsgHeader.msg_iov = iov;
			MsgHeader.msg_iovlen = 3;
			MsgHeader.msg_control = NULL;
			MsgHeader.msg_controllen = 0;
			MsgHeader.msg_flags = 0;
						
		 	iov[0].iov_base	= szBoundary+dwTotalSendSz;
		   	iov[0].iov_len =  dwHeaderSz-dwTotalSendSz;
			iov[1].iov_base = tState->pbyDataAddr;
			iov[1].iov_len = ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize;
			iov[2].iov_base = pEnding;
			iov[2].iov_len = 2;
    	}
    	else if (dwTotalSendSz < (dwHeaderSz + ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize))
    	{
			MsgHeader.msg_name = NULL;
			MsgHeader.msg_namelen = 0;
			MsgHeader.msg_iov = iov;
			MsgHeader.msg_iovlen = 2;
			MsgHeader.msg_control = NULL;
			MsgHeader.msg_controllen = 0;
			MsgHeader.msg_flags = 0;

			iov[0].iov_base = tState->pbyDataAddr + (dwTotalSendSz-dwHeaderSz);
			iov[0].iov_len = ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize - (dwTotalSendSz-dwHeaderSz);
			iov[1].iov_base = pEnding;
			iov[1].iov_len = 2;
    	}
    	else if (dwTotalSendSz < (dwHeaderSz + ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize + 2))
    	{
			MsgHeader.msg_name = NULL;
			MsgHeader.msg_namelen = 0;
			MsgHeader.msg_iov = iov;
			MsgHeader.msg_iovlen = 1;
			MsgHeader.msg_control = NULL;
			MsgHeader.msg_controllen = 0;
			MsgHeader.msg_flags = 0;
    		
			iov[0].iov_base = pEnding + (dwTotalSendSz - dwHeaderSz - ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize);
			iov[0].iov_len = 2 - (dwTotalSendSz - dwHeaderSz - ((TUBufferInfo *)tState->pbyHdrAddr)->dwJPEGSize);
    	}
    	
	    iWriteSz = sendmsg(pThisClient->fdCliSck, &MsgHeader, 0);
#ifdef _DEBUG	    
	    printf("iWriteSz = %d ..\n", iWriteSz);
#endif //_DEBUG	    
	    if (iWriteSz < 0)
	    {
	    	bNeedSelect = TRUE;
        	DBPRINT3("[ServerPush]: write fail, fd %d errno:(%d) msg: %s\n", pThisClient->fdCliSck, errno, strerror(errno));
        }
//	    if ((iWriteSz < 0) && (errno == EPIPE))
//	        return ServerPush_RmClient(pThis, dwTrackNo, dwCliNo);
	    if (iWriteSz > 0)
		    dwTotalSendSz += iWriteSz;
	    if (dwTotalSendSz >= dwTotalDataSz)
			bShouldSend = FALSE;
    }

    // record sequence number        
    pThisClient->dwSeqNo = ((TUBufferInfo *)tState->pbyHdrAddr)->tuBuf.dwSeqNo;
	return S_OK;
}



/* =========================================================================================== */
static SCODE ServerPush_SendStartCmdToEnc(TServerPushAppInfo* pThis, DWORD dwTrackNo)
{
    TTrackInfo  *pThisTrack  = &pThis->tTrackInfo[dwTrackNo];
	char acTemp[100];
	int iRet;
	int iLen;
	
	acTemp[0] = 1;
	// force it using a long mode
	acTemp[1] = 0x84;
	sprintf(&(acTemp[6]), "<control><request><host>encoder%u</host><content>start</content></request></control>", pThisTrack->dwStreamNo);

	iLen = strlen(&(acTemp[6]));

	acTemp[5] =  iLen >> 24;
	acTemp[4] = (iLen & 0x00FF0000) >> 16;
	acTemp[3] = (iLen & 0x0000FF00) >> 8;
	acTemp[2] = (iLen & 0x000000FF);
	
    iRet = write(pThisTrack->iFdFIFO, acTemp, iLen + 6);
    if (iRet == -1)
	{
    	DBPRINT1("[SERV_PUSH] send fifo %s fail ...\n", acTemp+6);
    	return S_FAIL;
    }
    pThisTrack->dwCmdCounter++;
   	DBPRINT1("[SERV_PUSH] send fifo %s success ...\n", acTemp+6);
    return S_OK;
}

/* =========================================================================================== */
SCODE ServerPush_SendStopCmdToEnc(TServerPushAppInfo* pThis, DWORD dwTrackNo)
{
    TTrackInfo  *pThisTrack  = &pThis->tTrackInfo[dwTrackNo];
	char acTemp[100];
	int iRet;
	int iLen;

	acTemp[0] = 1;
	// force it using a long mode
	acTemp[1] = 0x84;
	sprintf(&(acTemp[6]), "<control><request><host>encoder%u</host><content>stop</content></request></control>", pThisTrack->dwStreamNo);

	iLen = strlen(&(acTemp[6]));

	acTemp[5] =  iLen >> 24;
	acTemp[4] = (iLen & 0x00FF0000) >> 16;
	acTemp[3] = (iLen & 0x0000FF00) >> 8;
	acTemp[2] = (iLen & 0x000000FF);

	
    iRet = write(pThisTrack->iFdFIFO, acTemp, iLen + 6);
    if (iRet == -1)
	{
    	DBPRINT1("[SERV_PUSH] send fifo %s fail ...\n", acTemp+6);
    	return S_FAIL;
    }
    pThisTrack->dwCmdCounter--;
   	DBPRINT1("[SERV_PUSH] send fifo %s success ...\n", acTemp+6);
    return S_OK;
}
