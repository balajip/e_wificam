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

#ifndef __SHRD_BUFFER_SOURCE_H__
#define __SHRD_BUFFER_SOURCE_H__


#include "SharedBuffMgr.h"
#include "SrcCommon.h"


typedef struct shrdbufsrc_init_options
{
	CHAR	*szSharedBuffer;
	CHAR	*szCmdFiFoPath;
} TShrdBufSrcInitOpts;

typedef struct sharedbuffer_source
{
	
	int				iSrcFd;
	int				iCmdFiFo;
	int				iChNo;
	fd_set				tReadfdsRec;
	fd_set				tReadfds;
	HANDLE				hSharedBuffMgr;
	TSharedBuffMgrState	tSBState;
} TShrdBufSrc;

int   ShrdBufferSrc_GetFd(HANDLE hShrdBufSrc);
SCODE ShrdBufferSrc_Initial(HANDLE *phShrdBufSrc, TShrdBufSrcInitOpts *ptInitOpts);
SCODE ShrdBufferSrc_Release(HANDLE *phShrdBufSrc);
SCODE ShrdBufferSrc_GetUBuffer(HANDLE hShrdBufSrc, TSrcUBufInfo *ptUBuffInfo);
SCODE ShrdBufferSrc_ReleaseUBuffer(HANDLE hShrdBufSrc, TSrcUBufInfo *ptUBuffInfo);
SCODE ShrdBufferSrc_EventHandler(HANDLE hShrdBufSrc, ESrcEventType eType);

#endif // __SHRD_BUFFER_SOURCE_H__

