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

#ifndef __SRC_COMMON_H__
#define __SRC_COMMON_H__

#include "ubuffer.h"
#include "ubuffer_limit.h"
#include "avicontainer.h"
#if 0
typedef struct src_ubuf_info
{
	BYTE	*pbyUBuffHdr;
	int		iUBuffHdrLen;
	BYTE	*pbyUBuffPayload;
	int		iUBuffPayloadLen;
} TSrcUBufInfo;
#endif
typedef enum src_event_type
{
	letNeedIntra,
	letNeedConf,
	letBitstrmStart,
	letBitstrmStop
} ESrcEventType;

#endif

