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
 * amr_rtp2storage.c
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

#include "typedef.h"
#include "errordef.h"
#include <string.h>
#include "amr_rtp2storage.h"

//=====================================================================================
#define MAX_PACKET	16

const BYTE g_abyFT2Size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 
	5, 0, 0, 0, 0, 0, 0, 0};

//=====================================================================================
SCODE AMR_RTP2Storage(BYTE *pbyOut, BYTE *pbyIn)
{
	BYTE	abyToC[MAX_PACKET];
	DWORD	dwPacketNum;
	int	i;

	/* omit CMR */
	pbyIn ++;

	/* Scan the ToC entries */
	i = 0;
	while ((*pbyIn >> 7) && (i < MAX_PACKET))
	{
		BYTE	byTmp;
		byTmp = *pbyIn ++;
		/* Clear the follow bit */
		abyToC[i] = byTmp & 0x7F; 
		i ++;
	}

	/* MAX_PACKET is too small */
	if (*pbyIn >> 7)
	{
		return S_FAIL;
	}

	abyToC[i] = (*pbyIn ++) & 0x7F; 
	dwPacketNum = i + 1;

	for (i = 0; i < dwPacketNum; i ++)
	{
		BYTE	byToC = abyToC[i];
		DWORD	dwSize = g_abyFT2Size[byToC >> 3];

		*pbyOut = byToC; 
		pbyOut ++;
		memcpy(pbyOut, pbyIn, dwSize);
		pbyOut += dwSize;
		pbyIn += dwSize;
	}	

	return S_OK;
}

