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

/*!
 *******************************************************************************
 * Copyright 2007-2010 VN, Inc. All rights reserved.
 *
 * \file
 * adec_config.c
 *
 * \brief
 * Adec configuration handler functions implement.
 *
 * \date
 * 2008/3/20
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#include "adec_config.h"
#include <stdio.h>
#include <sys/stat.h>

/* ========================================================================== */
void Adec_GenPIDFile(HANDLE hObject, const CHAR *szParam, SDWORD len);
void Adec_SetDeviceNode(HANDLE hObject, const CHAR *szParam, SDWORD len);
void Adec_SetSocketPath(HANDLE hObject, const CHAR *szParam, SDWORD len);
void Adec_SetSampleRate(HANDLE hObject, const CHAR *szParam, SDWORD len);

/* ========================================================================== */
SCODE Adec_InitConfigHandler(TAdecInfo *ptAdecInfo)
{
    TXmlWrapperInitOptions tInitOptions;
	HANDLE hXmlWrapperObj;

    TXmlWrapperTreeMap ptTreeMap[] = 
    {
        {"/root/adec/pidfile", NULL, &Adec_GenPIDFile, NULL},
        {"/root/adec/device", NULL, &Adec_SetDeviceNode, NULL},
        {"/root/adec/socket", NULL, &Adec_SetSocketPath, NULL},
        {"/root/adec/sample_rate", NULL, &Adec_SetSampleRate, NULL},
        {NULL, NULL, NULL}
    };
    
    tInitOptions.dwVersion = XMLWRAPPER_VERSION;

    if ( XmlWrapper_Initial(&hXmlWrapperObj, &tInitOptions) != S_OK)
    {
        DBPRINT0("[ADEC] Initial XmlWrapper object fail! \n");
        return S_FAIL;
    }
    // Set tree map
    if ( XmlWrapper_SetHandler(hXmlWrapperObj, ptTreeMap,NULL) != S_OK)
    {
        DBPRINT0("[ADEC] Set XmlWrapper Handler fail! \n");
        return S_FAIL;
    }
    // Load configuration
    if (XmlWrapper_ReadFile_UsrDefFunc(ptAdecInfo->szConfigFile, hXmlWrapperObj, ptAdecInfo) != S_OK)
    {
        DBPRINT1("[ADEC][%s] XmlWrapper read config fail! \n", __FUNCTION__);
        return S_FAIL;
    }

	XmlWrapper_Release(&hXmlWrapperObj);
    return S_OK;
}

/* ========================================================================== */
void Adec_GenPIDFile(HANDLE hObject, const CHAR *szParam, SDWORD len)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)hObject;
    FILE *pfPid;
    if (len < FILEPATH_LENGTH)
    {
        snprintf(ptAdecInfo->szPidFile, len+1, "%s", szParam);
    }
    else
    {
        return;
    }
    
    if (ptAdecInfo->szPidFile == NULL)
    {
        DBPRINT1("[ADEC] Can't open %s", ptAdecInfo->szPidFile);
    }
    else
    {
        printf("%s\n", ptAdecInfo->szPidFile);
        pfPid = fopen(ptAdecInfo->szPidFile, "w");
        fprintf (pfPid, "%d\n", getpid());
        fchmod (fileno(pfPid), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        fclose (pfPid);
    }
    DBPRINT1("[ADEC] Generate PID File %s\n", ptAdecInfo->szPidFile);
}

/* ========================================================================== */
void Adec_SetDeviceNode(HANDLE hObject, const CHAR *szParam, SDWORD len)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)hObject;
    if (len < FILEPATH_LENGTH)
    {
        snprintf(ptAdecInfo->szDeviceNode, len+1, "%s", szParam);
        DBPRINT1("[ADEC] Set Device Node %s\n", ptAdecInfo->szDeviceNode);
    }
    else
    {
        DBPRINT1("[ADEC] Device node path length is too long %s\n", ptAdecInfo->szDeviceNode);
        return;
    }
}

/* ========================================================================== */
void Adec_SetSocketPath(HANDLE hObject, const CHAR *szParam, SDWORD len)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)hObject;
    if (len < FILEPATH_LENGTH)
    {
        snprintf(ptAdecInfo->szSocketPath, len+1, "%s", szParam);
        DBPRINT1("[ADEC] Set socket path %s\n", ptAdecInfo->szSocketPath);
    }
    else
    {
        DBPRINT1("[ADEC] Socket path length is too long %s\n", ptAdecInfo->szSocketPath);
        return;
    }
}
/* ========================================================================== */
void Adec_SetSampleRate(HANDLE hObject, const CHAR *szParam, SDWORD len)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)hObject;
    ptAdecInfo->dwSampleRate = strtoul(szParam, NULL, 10);
}
