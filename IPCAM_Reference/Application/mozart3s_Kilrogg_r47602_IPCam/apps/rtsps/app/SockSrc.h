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
 * SockSrc header file
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

#ifndef __SOCKET_SOURCE_H__
#define __SOCKET_SOURCE_H__

#include "LiveMediaSrc.h"

typedef struct socksrc_init_options
{
	CHAR	*szRecvSockPath;
	CHAR	*szCmdFiFoPath;
} TSockSrcInitOpts;

SCODE SockSrc_Initial(HANDLE *phSockSrc, TSockSrcInitOpts *ptInitOpts);
SCODE SockSrc_Release(HANDLE *phSockSrc);

SCODE SockSrc_GetUBuffer(HANDLE hSockSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE SockSrc_ReleaseUBuffer(HANDLE hSockSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE SockSrc_EventHandler(HANDLE hSockSrc, ELMSrcEventType eType);
SCODE SockSrc_WakeUpToTerminate(HANDLE hSockSrc);

#endif // __SOCKET_SOURCE_H__
