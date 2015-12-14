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
 * serverpushapp.h
 *
 * \brief
 * Head file of server push process implement.
 * 
 * \date
 * 2007/01/04
 *
 * \author
 * Jessie Lin
 *
 *******************************************************************************
 */

#ifndef _SERVER_PUSH_APP_H_
#define _SERVER_PUSH_APP_H_
#include "typedef.h"
#include "errordef.h"

#define PID_FILE               "/var/run/serverpush.pid"

typedef struct arginfo
{
    char* szConfigPath;
} TArgInfo;

#if 0
/* ===========================================================================================
 * Function List
 * =========================================================================================== */
SCODE ServerPushApp_Initial(HANDLE *phObject, TArgInfo tArgInfo);
SCODE ServerPushApp_Start(HANDLE hObject);
SCODE ServerPushApp_Release(HANDLE *phObject);
#endif

/*!
 *********************************************************************
 * \brief
 * Create handle of TServerPushAppInfo object, 
 * this function should be called before calling any other function
 *
 * \param phObject
 * a (o) pointer of TServerPushAppInfo Handle
 *
 * \param tArgInfo
 * a (i) tArgInfo about config file path
 *
 * \retval S_OK
 * Create object ok
 *
 * \retval S_FAIL
 * Create object faild
 *
 * \Remark
 * See ServerPushApp_Release
 *
 ******************************************************************** */
SCODE ServerPushApp_Initial(HANDLE *phObject, TArgInfo tArgInfo);

/*!
 *********************************************************************
 * \brief
 * Start server push process, 
 * one for receiving HTTP client socket and uri from HTTP server (boa),
 * the other for receiving encoded JPEG data and sending to HTTP clients
 *
 * \param phObject
 * a (i) pointer of TServerPushAppInfo Handle
 *
 * \retval S_OK
 * Start processing
 *
 * \retval S_FAIL
 * Create processing faild
 *
 ******************************************************************** */
SCODE ServerPushApp_Start(HANDLE hObject);

/*!
 *********************************************************************
 * \brief
 * Release TServerPushAppInfo object : 
 * free memories of xml parser and xml handler tree
 *
 * \param phObject
 * a (o) pointer of TServerPushAppInfo Handle
 *
 * \retval S_OK
 * Release succeed
 *
 * \retval S_FAIL
 * Release fail
 *
 * \remark
 * see ServerPushApp_Initial
 
 ******************************************************************** */
SCODE ServerPushApp_Release(HANDLE *phObject);

SCODE ServerPushApp_Stop(HANDLE *phObject);
//SCODE ServerPushApp_ReConfig(HANDLE hObject);

#endif //_SERVER_PUSH_APP_H_
