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
 * RtspSrvrApp.h
 *
 * \brief
 * Rtsp Server Application header file
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

#ifndef __RTSP_SERVER_APPLICATION_H__
#define __RTSP_SERVER_APPLICATION_H__

#include "common.h"
#include "typedef.h"
#include "global_codec.h"
#include "dbgdefs.h"

#define RTSPSERVER_DEFAULT_CONFIG_FILE		"stream_server_config.xml"
#define	RTSPSERVER_DEFAULT_PORT				554

SCODE RtspSrvrApp_Initial(HANDLE *phRtspSrvrApp, const char *szConfFileName);
SCODE RtspSrvrApp_Release(HANDLE *phRtspSrvrApp);
SCODE RtspSrvrApp_Start(HANDLE hRtspSrvrApp);
SCODE RtspSrvrApp_Stop(HANDLE hRtspSrvrApp);
SCODE RtspSrvrApp_Reconfig(HANDLE hRtspSrvrApp, CHAR *szConfigFile);

#endif // __RTSP_SERVER_APPLICATION_H__
