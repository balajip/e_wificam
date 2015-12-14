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
 * serverpushapp.c
 *
 * \brief
 * Server push process implement.
 * 
 * \date
 * 2007/01/04
 *
 * \author
 * Jessie Lin
 *
 *******************************************************************************
 */

#include "serverpush_local.h"

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "xmlwrapper.h"
#include "dbgdefs.h"

/* =========================================================================================== */
SCODE ServerPushApp_Initial(HANDLE *phObject, TArgInfo tArgInfo)
{
    TServerPushAppInfo *pThis;

    pThis = (TServerPushAppInfo *)malloc(sizeof(TServerPushAppInfo));
    memset(pThis, 0, sizeof(TServerPushAppInfo));
    
    DBPRINT2("[SERV_PUSH][%s] malloc(sizeof(TServerPushAppInfo))=%d\n", 
             __FUNCTION__, sizeof(TServerPushAppInfo));
    
    *phObject = (HANDLE)(pThis);
    
    // ====== set config and socket file path ======
    pThis->szConfigPath = strdup(tArgInfo.szConfigPath);

    // ====== Read XMLConfig File ======
    if (XmlWrapper_Setting(pThis) != S_OK)
	{
		fprintf(stderr, "[SERV_PUSH] Initial XMLWrapper object fail !!\n");
		return S_FAIL;
	}
	
	// ====== Load configuration ======
	if (XmlWrapper_ReadFile_UsrDefFunc(pThis->szConfigPath, 
	                                 pThis->hXMLWrapperObject, pThis) != S_OK)
	{
		fprintf(stderr, "[SERV_PUSH] XmlWrapper read config fail! \n");
		return S_FAIL;
	}
    if (pThis->bTerminate == TRUE)
    {
		fprintf(stderr, "[SERV_PUSH] config error! \n");
		return S_FAIL;
	}
    
    fprintf(stdout, "[SERV_PUSH] Initial process\n");
    return S_OK;
}

/* =========================================================================================== */
SCODE ServerPushApp_Start(HANDLE hObject)
{
    ServerPushApp_Process(hObject);
    return S_OK;
}

/* =========================================================================================== */
SCODE ServerPushApp_Release(HANDLE *phObject)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)(*phObject);
    TTrackInfo         *ptThisTrack = NULL;
    DWORD               dwTrackNo = 0;
    DWORD               dwCliNo = 0;
    DWORD				dwCounter;
    pThis->bTerminate = TRUE;
    fprintf(stdout, "[SERV_PUSH] Release process\n");
    
    // ====== close sockets and free paths ======
    close(pThis->iHttpFdipcSck);
    if (pThis->szHttpFdipcPath) free(pThis->szHttpFdipcPath);
    if (pThis->szBoundary)      free(pThis->szBoundary);
	if (pThis->szConfigPath)    free(pThis->szConfigPath);

    // ====== releast TrackInfo array objects ======
	for(dwTrackNo=0; dwTrackNo< pThis->dwTrackNum; dwTrackNo++)
	{
	    ptThisTrack = &(pThis->tTrackInfo[dwTrackNo]);
	    
	    for (dwCounter = 0; dwCounter < ptThisTrack->dwCmdCounter; dwCounter++) {
			ServerPush_SendStopCmdToEnc(pThis, dwTrackNo);
	    }
	    
   		if (SharedBuffMgr_Release(&ptThisTrack->hJPEGEncObj) != S_OK)
		{
			printf("Release shared buffer manager object fail !!\n");
		}
		ptThisTrack->hJPEGEncObj = NULL;
	    close(ptThisTrack->iFdFIFO);
	    if (ptThisTrack->szFIFOPathName)
	    {
	    	free(ptThisTrack->szFIFOPathName);
	    }
	    if (ptThisTrack->szURI)  
	    {
	        free(ptThisTrack->szURI);
	    }
	    if(pThis->tTrackInfo[dwTrackNo].szSckPath)
	    {
	        free(pThis->tTrackInfo[dwTrackNo].szSckPath);
	    }
	    
	    for (dwCliNo = 0; dwCliNo < MAX_CLIENT; dwCliNo++)
	    {
	        if (ptThisTrack->tClients[dwCliNo].fdCliSck > 0)
	        {
	            close(ptThisTrack->tClients[dwCliNo].fdCliSck);
	        }
	        if (ptThisTrack->tClients[dwCliNo].pbyBase) 
	        {
	            free(ptThisTrack->tClients[dwCliNo].pbyBase);
	        }
	    }
	}
    free(pThis->tTrackInfo);
    pThis->tTrackInfo = NULL;

	if (XmlWrapper_Release(&(pThis->hXMLWrapperObject)) != S_OK) 
	{
		fprintf(stderr, "[SERV_PUSH] Release XmlWrapper object fail !!\n");
		return S_FAIL;
	}

    free(pThis);
    *phObject = NULL;    
    return S_OK;    
}

/* =========================================================================================== */
SCODE ServerPushApp_Stop(HANDLE *phObject)
{
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)(*phObject);
    
    pThis->bTerminate = TRUE;
    return S_OK;
}

/* =========================================================================================== */
SCODE ServerPushApp_ReConfig(HANDLE hObject)
{
    // TODO: check something can't be reload (ex. socket)
    
    TServerPushAppInfo *pThis = (TServerPushAppInfo *)(hObject);

	if( XmlWrapper_ReadFile_UsrDefFunc(pThis->szConfigPath, pThis->hXMLWrapperObject, pThis) != S_OK)
	{
		fprintf(stderr, "[SERV_PUSH] XmlWrapper read config fail! \n");
		return S_FAIL;
	}
    return S_OK;
}

/* =========================================================================================== */
