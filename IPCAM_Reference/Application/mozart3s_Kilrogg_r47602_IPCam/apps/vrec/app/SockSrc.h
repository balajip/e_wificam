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

#ifndef __SOCKET_SOURCE_H__
#define __SOCKET_SOURCE_H__

#include "SrcCommon.h"

typedef struct socksrc_init_options
{
	CHAR	*szRecvSockPath;
	CHAR	*szCmdFiFoPath;
	DWORD  	dwIOID;
} TSockSrcInitOpts;

int   SockSrc_GetFd(HANDLE hSockSrc);
SCODE SockSrc_Initial(HANDLE *phSockSrc, TSockSrcInitOpts *ptInitOpts);
SCODE SockSrc_Release(HANDLE *phSockSrc);

SCODE SockSrc_GetUBuffer(HANDLE hSockSrc, TSrcUBufInfo *ptUBuffInfo);
SCODE SockSrc_ReleaseUBuffer(HANDLE hSockSrc, TSrcUBufInfo *ptUBuffInfo);
SCODE SockSrc_EventHandler(HANDLE hSockSrc, ESrcEventType eType);

#endif // __SOCKET_SOURCE_H__
