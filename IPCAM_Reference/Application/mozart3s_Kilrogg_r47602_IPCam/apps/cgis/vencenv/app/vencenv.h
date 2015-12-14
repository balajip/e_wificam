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
 * vencenv.h
 *
 * \brief
 * Header file of vencenv CGI
 *
 * \date
 * 2011/07/25
 *
 * \author
 * Acer Tsai
 *
 *******************************************************************************
 */

#ifndef _VENCENV_H_
#define _VENCENV_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "dbgdefs.h"
#include "errordef.h"
#include "typedef.h"
#include "xmlwrapper.h"
#include "xmlmgr.h"

struct post_data {
  char szSupportCodec[32];
//  char szResolution[16];
  char szStreamNum[4];
};

typedef enum error_case 
{
  eError_MemoryNotEnough = 1,
  eError_XmlWrapperFail,
  eError_ContentLengthFail,
  eError_XmlMgrFail,  
} EERRCase;

#define VIDEO_CODEC_JPEG_INIT_BIT 0x1
#define VIDEO_CODEC_MP4V_INIT_BIT 0x2
#define VIDEO_CODEC_H264_INIT_BIT 0x4
#define VIDEO_CODEC_SVC_INIT_BIT  0x8

#endif //_VENCENV_H_

