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
 * RegSrc.c
 *
 * \brief
 * RegSrc header file
 * A Live Media Source Implementation
 * Request media bitstream from BSD by register mechanism
 * The media is from unix domain socket (DGRAM) in ubuffer format
 *
 * \date
 * 2009/05/15
 *
 * \author
 * DJhow.Tu
 *
 *
 *******************************************************************************
 */

#ifndef __REG_SOURCE_H__
#define __REG_SOURCE_H__

#include "LiveMediaSrc.h"

typedef struct regsrc_init_options
{
	CHAR	*szRegPath;
	DWORD	dwTrackNo;
	DWORD	dwMediaType;
} TRegSrcInitOpts;

SCODE RegSrc_Initial(HANDLE *phRegSrc, TRegSrcInitOpts *ptInitOpts);
SCODE RegSrc_Release(HANDLE *phRegSrc);

SCODE RegSrc_GetUBuffer(HANDLE hRegSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE RegSrc_ReleaseUBuffer(HANDLE hRegSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE RegSrc_EventHandler(HANDLE hRegSrc, ELMSrcEventType eType);
SCODE RegSrc_WakeUpToTerminate(HANDLE hRegSrc);

#endif // __REG_SOURCE_H__
