/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2004 VN Inc. All rights reserved.
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
 */

/*!
 *******************************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * aenc_gamr.c
 *
 * \brief
 * Audio encoder - Codec interface
 *
 * \date
 * 2006/07/03
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */

#include "aenc.h"
#include "dbgdefs.h"
#include <errno.h>
#include <assert.h>
#include <string.h>

//==XML Call Back Function===================================================================================
void AxeXmlTree_Codec_Bitrate(void *userData, const char *name, int len);
void AxeXmlTree_Codec_SampRate(void *userData, const char *name, int len);

//===========================================================================================================
void registerCodecs(TAencInfo *ptaencInfo)
{
	/* GAMR */
#ifdef __GAMR_Enc__
	register_GAMR(&(ptaencInfo->atcpRegistered[0]), &(ptaencInfo->atcoSaved[0]));
#endif	
	/* AAC4 */
#ifdef __AAC4_Enc__
	register_AAC4(&(ptaencInfo->atcpRegistered[1]), &(ptaencInfo->atcoSaved[1]));
#endif
	/* G711 */
#ifdef __G711_Enc__
	register_G711(&(ptaencInfo->atcpRegistered[2]), &(ptaencInfo->atcoSaved[2]));
#endif
	
	/* Add another codec here */
	/* G726 */
#ifdef __G726_Enc__
	register_G726(&(ptaencInfo->atcpRegistered[3]), &(ptaencInfo->atcoSaved[3]));
#endif

	/*
	for (i = 0; i < MAX_CODEC_NUM; i ++) {
		taencInfo.atcoSaved[i].dwFourCC = taencInfo.atcpRegistered[i].dwFourCC;
	}
	*/
}

//==XML Codec call back Function====================================================================================
void AxeXmlTree_Codec_Bitrate(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;
	char				szTmp[16];
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szTmp) - 1)) 
								? len : (sizeof(szTmp) - 1);
	
	DBPRINT1("%s:",__func__)
	if (ptOpt == NULL) {
		return;
	}
	
	memcpy(szTmp, name, dwLen);
	szTmp[dwLen] = '\0';
	dwTmp = strtol(szTmp, NULL, 0);
	assert(errno != ERANGE);	
	ptOpt->dwBitRate = dwTmp;	
	DBPRINT1("%ld\n", ptOpt->dwBitRate);
}

void AxeXmlTree_Codec_SampRate(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;			
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;
	char				szTmp[16];
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szTmp) - 1)) 
								? len : (sizeof(szTmp) - 1);
	
	DBPRINT1("%s:",__func__)
	if (ptOpt == NULL) {
		return;
	}

	memcpy(szTmp, name, dwLen);
	szTmp[dwLen] = '\0';
	dwTmp = strtol(szTmp, NULL, 0);
	assert(errno != ERANGE);
	
	ptOpt->dwSampRate = dwTmp;
	DBPRINT1("%ld\n", ptOpt->dwSampRate);
}

void AxeXmlTree_Codec_ChNum(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;			
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;
	char				szTmp[16];
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szTmp) - 1)) 
								? len : (sizeof(szTmp) - 1);

	DBPRINT1("%s:",__func__)
	if (ptOpt == NULL) {
		return;
	}

	memcpy(szTmp, name, dwLen);
	szTmp[dwLen] = '\0';
	dwTmp = strtol(szTmp, NULL, 0);
	assert(errno != ERANGE);
	ptOpt->dwChanNum = dwTmp;
	DBPRINT1("%ld\n", ptOpt->dwChanNum);
}
