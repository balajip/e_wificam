/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2005 VN Inc. All rights reserved.
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
 * Copyright 2000-2005 VN, Inc. All rights reserved.
 *
 * \file
 * amr_rtp2storage.h
 *
 * \brief
 * Transform the RTP AMR packet into storage format specified in RFC 3267
 *
 * \date
 * 2005/11/04
 *
 * \author
 * Cheng-Ching Huang
 *
 *******************************************************************************
 */

#ifndef _AMR_RTP2STORAGE_
#define _AMR_RTP2STORAGE_

SCODE AMR_RTP2Storage(BYTE *pbyOut, BYTE *pbyIn);

#endif // _AMR_RTP2STORAGE_

