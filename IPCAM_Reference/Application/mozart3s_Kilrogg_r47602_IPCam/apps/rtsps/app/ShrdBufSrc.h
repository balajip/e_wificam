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
 * ShrdBufSrc.h
 *
 * \brief
 * ShrdBufSrc header file
 * A Live Media Source Implementation 
 * The media is from SharedBuffMgr in ubuffer format
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

#ifndef __SHAREDBUFFER_SOURCE_H__
#define __SHAREDBUFFER_SOURCE_H__

#include "LiveMediaSrc.h"

typedef struct shrdbufsrc_init_options
{
	CHAR	*szSharedBuffer;
	CHAR	*szCmdFiFoPath;
} TShrdBufSrcInitOpts;

SCODE ShrdBufSrc_Initial(HANDLE *phShrdBufSrc, TShrdBufSrcInitOpts *ptInitOpts);
SCODE ShrdBufSrc_Release(HANDLE *phShrdBufSrc);

SCODE ShrdBufSrc_GetUBuffer(HANDLE hShrdBufSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE ShrdBufSrc_ReleaseUBuffer(HANDLE hShrdBufSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE ShrdBufSrc_EventHandler(HANDLE hShrdBufSrc, ELMSrcEventType eType);
SCODE ShrdBufSrc_WakeUpToTerminate(HANDLE hShrdBufSrc);

#endif // __SHAREDBUFFER_SOURCE_H__
