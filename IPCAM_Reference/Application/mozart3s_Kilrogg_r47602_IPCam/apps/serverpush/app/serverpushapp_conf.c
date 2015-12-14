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
 * serverpush_config.c
 *
 * \brief
 * Video encoder slave process setoption implement.
 * 
 * \date
 * 2007/01/03
 *
 * \author
 * Jessie Lin
 *
 *******************************************************************************
 */

#include "serverpush_local.h"

#include <stdio.h>
#include <string.h>
#include <libgen.h>     // dirname
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>  // chmod
#include <sys/stat.h>   // chmod
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "typedef.h"
#include "dbgdefs.h"
#include "xmlwrapper.h"
#include "fdipc.h"



// ==== static to serverpush_conf.c ====
static DWORD dwTrackNo;     // Record the currnet TrackNo

/* ===========================================================================================
 * CharDataHandler
 * =========================================================================================== */
void ServerPushApp_SetHttpFdipcSck(HANDLE hObject, const CHAR* szFdipcSck, SDWORD len);
void ServerPushApp_SetBoundary(HANDLE hObject, const CHAR* szBoundary, SDWORD len);
void ServerPushApp_SetTrackNum(HANDLE hObject, const CHAR* szTrackNum, SDWORD len);
SCODE ServerPushApp_SetTrackInfo(HANDLE hObject, const CHAR* szData, const CHAR** atts);
void ServerPushApp_SetAccessName(HANDLE hObject, const CHAR* szData, SDWORD len);
void ServerPushApp_SetTrackSck(HANDLE hObject, const CHAR* szTrackSck, SDWORD len);
void ServerPushApp_SetFifoPath(HANDLE hObject, const CHAR* szFifoPath, SDWORD len);

/* =========================================================================================== */
TXmlWrapperTreeMap ptTreeMap[] =
{
    {"/root/server_push/httpfdipcsock", NULL, &ServerPushApp_SetHttpFdipcSck, NULL},
    {"/root/server_push/boundary", NULL, &ServerPushApp_SetBoundary, NULL},
    {"/root/server_push/tracknum", NULL, &ServerPushApp_SetTrackNum, NULL},
    {"/root/server_push/video", &ServerPushApp_SetTrackInfo, NULL, NULL},
    {"/root/server_push/video/accessname", NULL, &ServerPushApp_SetAccessName, NULL},
    {"/root/server_push/video/sharebuffer", NULL, &ServerPushApp_SetTrackSck, NULL},
    {"/root/server_push/video/fifo", NULL, &ServerPushApp_SetFifoPath, NULL},
    {NULL, NULL, NULL}
};

/* =========================================================================================== */
void ServerPushApp_SetHttpFdipcSck(HANDLE hObject, const CHAR* szFdipcSck, SDWORD len)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
    
    pThis->szHttpFdipcPath = malloc(len+1);
    memset(pThis->szHttpFdipcPath, '\0', len+1);
    DBPRINT2("[SERV_PUSH][%s] malloc(len+1)=%d\n", __FUNCTION__, len+1);
    strncpy(pThis->szHttpFdipcPath, szFdipcSck, len);
    
    ServerPush_SetHTTPSck(pThis);
}

/* =========================================================================================== */
void ServerPushApp_SetBoundary(HANDLE hObject, const CHAR* szBoundary, SDWORD len)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
    
    pThis->szBoundary = malloc(len+1);
    memset(pThis->szBoundary, '\0', len+1);
    DBPRINT2("[SERV_PUSH][%s] malloc(len+1)=%d\n", __FUNCTION__, len+1);
    strncpy(pThis->szBoundary, szBoundary, len);
}

/* =========================================================================================== */
void ServerPushApp_SetTrackNum(HANDLE hObject, const CHAR* szTrackNum, SDWORD len)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
    
    pThis->dwTrackNum = atoi(szTrackNum);
    
    // ====== dymanic allocate memory to TrackInfo ======
    pThis->tTrackInfo = calloc(pThis->dwTrackNum, sizeof(TTrackInfo));
}

/* =========================================================================================== */
SCODE ServerPushApp_SetTrackInfo(HANDLE hObject, const CHAR* szData, const CHAR** atts)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
    TTrackInfo         *ptInfo = NULL;
    int                 i = 0;
        
    if (!strcmp(atts[0], "TrackNo"))
    {
        dwTrackNo = atoi(atts[1]);
        if (dwTrackNo >= pThis->dwTrackNum)
        {
            //fprintf(stderr, "[Config] Error! TrackNo attribute \"%d\" error! \n", dwTrackNo);
            //pThis->bTerminate = TRUE;
            return IGNORE_CHILD_CONFIG;
        }
        else
        {
            ptInfo = &pThis->tTrackInfo[dwTrackNo];
            
            // set Track Number
            ptInfo->dwTrackNo = dwTrackNo;
            
            // set init client sck to -1
            for (i = 0; i < MAX_CLIENT; i++)
            {
                ptInfo->tClients[i].fdCliSck = -1;
            }
        }
    }
    else
    {
        fprintf(stderr, "[Config] Error! NOT giving TrackNo attribute! \n");
        return IGNORE_CHILD_CONFIG;
    }
    return S_OK;
}

/* =========================================================================================== */
void ServerPushApp_SetAccessName(HANDLE hObject, const CHAR* szAccessName, SDWORD len)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
    
    pThis->tTrackInfo[dwTrackNo].szURI = malloc(len+1);
    memset(pThis->tTrackInfo[dwTrackNo].szURI, '\0', len+1);
    DBPRINT2("[SERV_PUSH][%s] malloc(len+1)=%d\n", __FUNCTION__, len+1);
    strncpy(pThis->tTrackInfo[dwTrackNo].szURI, szAccessName, len);
}

/* =========================================================================================== */
void ServerPushApp_SetTrackSck(HANDLE hObject, const CHAR* szTrackSck, SDWORD len)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
    
    pThis->tTrackInfo[dwTrackNo].szSckPath = malloc(len+1);
    memset(pThis->tTrackInfo[dwTrackNo].szSckPath, '\0', len+1);
    DBPRINT2("[SERV_PUSH][%s] malloc(len+1)=%d\n", __FUNCTION__, len+1);
    strncpy(pThis->tTrackInfo[dwTrackNo].szSckPath, szTrackSck, len);
    
    //ServerPush_InitSharedBuffMgr(pThis, dwTrackNo);
}
/* =========================================================================================== */
void ServerPushApp_SetFifoPath(HANDLE hObject, const CHAR* szFifoPath, SDWORD len)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)hObject;
	CHAR	*szStreamNo;
	pThis->tTrackInfo[dwTrackNo].szFIFOPathName = malloc(len+1);
	memset(pThis->tTrackInfo[dwTrackNo].szFIFOPathName, '\0', len+1);
	strncpy(pThis->tTrackInfo[dwTrackNo].szFIFOPathName, szFifoPath, len);
	
	szStreamNo = strstr(pThis->tTrackInfo[dwTrackNo].szFIFOPathName, "@");
	if (szStreamNo == NULL) {
		printf("Fifo path error!!, no stream info !!\n");
	} else {
		*szStreamNo = '\0';
		++szStreamNo;
		pThis->tTrackInfo[dwTrackNo].dwStreamNo = (DWORD)atoi(szStreamNo);
//		printf("szStreamNo = %s , len = %d!!\n", szStreamNo, strlen(szStreamNo));
	}
	
//	printf("Fifo path: %s !!\n", pThis->tTrackInfo[dwTrackNo].szFIFOPathName);
	
	// mkfifo
	if (ServerPush_CreateFifo(pThis->tTrackInfo[dwTrackNo].szFIFOPathName, &(pThis->tTrackInfo[dwTrackNo].iFdFIFO), O_WRONLY) != S_OK)
	{
		DBPRINT0("[SERV_PUSH] Create Fifo fail...\n");
	}

}

/* =========================================================================================== */
