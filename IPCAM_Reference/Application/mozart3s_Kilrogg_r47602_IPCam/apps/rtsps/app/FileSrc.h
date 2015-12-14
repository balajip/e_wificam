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
 * FileSrc.c
 *
 * \brief
 * FileSrc header file
 * A Live Media Source Implementation
 * The media is from file which stored concatenated ubuffers
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

#ifndef __FILE_SOURCE_H__
#define __FILE_SOURCE_H__

#include "LiveMediaSrc.h"

typedef struct filesrc_init_options
{
	CHAR	*szFilePath;
} TFileSrcInitOpts;

SCODE FileSrc_Initial(HANDLE *phFileSrc, TFileSrcInitOpts *ptInitOpts);
SCODE FileSrc_Release(HANDLE *phFileSrc);

SCODE FileSrc_GetUBuffer(HANDLE hFileSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE FileSrc_ReleaseUBuffer(HANDLE hFileSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE FileSrc_EventHandler(HANDLE hFileSrc, ELMSrcEventType eType);
SCODE FileSrc_WakeUpToTerminate(HANDLE hFileSrc);

#endif // __FILE_SOURCE_H__
