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
 * RtspSrvrApp_local.h
 *
 * \brief
 * Rtsp Server Application local header file
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
 
#ifndef __RTSP_SERVER_APPLICATION_LOCAL_H__
#define __RTSP_SERVER_APPLICATION_LOCAL_H__
#include "RtspSrvrApp.h"
#include "RtspSrvr.h"
#include "SockSrc.h"
#include "FileSrc.h"
#ifdef _ENABLE_SHAREDBUFF_
#include "ShrdBufSrc.h"
#endif // _ENABLE_SHAREDBUFF_
#ifdef _ENABLE_REGSRC_
#include "RegSrc.h"
#endif // _ENABLE_REGSRC_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef struct lmsrc_setting_information
{
	CHAR	*szMechanism;
	CHAR	*szPath;
	CHAR	*szCmdFiFoPath;
} TLMSrcSettingInfo;

typedef struct rtsp_server_application
{
	HANDLE				hRtspSrvrObj;

	USHORT				usRtspServerPort;
	USHORT				usBlockSize;
	DWORD				httpserver_type;
	CHAR				*szAuthMode;
	CHAR				*szHttpFdipcPath;
	
	// Video Sources Information
	DWORD				dwTotalVTrackNum;
	TLMSrcSettingInfo	*atVideoSettings;
	HANDLE				*ahVideoSrcs;
	
	// Audio Sources Information
	DWORD				dwTotalATrackNum;
	TLMSrcSettingInfo	*atAudioSettings;
	HANDLE				*ahAudioSrcs;
	
	// MData Sources Information
	DWORD				dwTotalDTrackNum;
	TLMSrcSettingInfo	*atMDataSettings;
	HANDLE				*ahMDataSrcs;
	
	// Access File Information
	DWORD				dwTotalAcsFileNum;
	TAcsFileInfo		*atAcsFileInfo;

#ifdef __HISTORY_STREAM_SUPPORT__
	// UPlayer Channel Information
	CHAR				*szHistAccessName;
	CHAR				*szUPlayerCommandPath;
	DWORD				dwTotalUPlayerChNum;
	TUPlayerChInfo		*atUPlayerChInfo;
#endif // __HISTORY_STREAM_SUPPORT__
#ifdef _QOS_	
	ERateCtrlFlags		eRateCtrlFlag;
#endif
} TRtspSrvrApp;

SCODE RtspSrvrApp_ParseXmlFile(TRtspSrvrApp *ptRtspSrvrApp, const char* pzConfigFile);

#endif // __RTSP_SERVER_APPLICATION_LOCAL_H__
