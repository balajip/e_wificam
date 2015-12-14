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
 * adec_output.h
 *
 * \brief
 * Adec audio out device handler header file.
 *
 * \date
 * 2008/3/24
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#ifndef _ADEC_OUTPUT_H_
#define _ADEC_OUTPUT_H_
#include "adec_local.h"

SCODE Adec_InitAudioOut(TAdecInfo *ptAdecInfo);
SCODE Adec_ResetAudioOut(TAdecInfo *ptAdecInfo);
SCODE Adec_WriteAudioOut(TAdecInfo *ptAdecInfo, BYTE *pbyOutFrame, DWORD dwOutSize);
#endif //define _ADEC_OUTPUT_H_

