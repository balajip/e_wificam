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
 * aenc_g726.c
 *
 * \brief
 * Audio encoder - G726
 *
 * \date
 * 2010/02/25
 *
 * \author
 * Acer Tsai
 *
 *******************************************************************************
 */

#ifdef __G726_Enc__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include "global_codec.h"
#include "errordef.h"
#include "typedef.h"
#include "ubuffer.h"
#include "aenc.h"
#include "dbgdefs.h"
#include "xmlwrapper.h"
#include "G726Enc.h"
//=================================================================================================
#define	MAX_USER_DATA_SIZE	15
#define SAMPLE_PER_FRAME	1024	//1024 sample out for VLC use

//=================================================================================================
typedef struct aenc_g726_info{
	HANDLE	*phCoreEnc;	
	DWORD	dwSeqNo;
	DWORD   dwHandleNum;
	DWORD	dwFrameCount;
	//void 	*pvOut; /* saved value, not allocated by myself */
} TAencG726Info;

//==function pointer===============================================================================
static SCODE aenc_G726_init(HANDLE hObj);
static SCODE aenc_G726_release(HANDLE *phObj);
static SCODE aenc_G726_get_conf(HANDLE hObj, void *pvOut, DWORD *pdwSize);
static DWORD aenc_G726_qr_out_size(HANDLE hObj);
static DWORD aenc_G726_qr_sp_per_ch_buf(HANDLE hObj);
static SCODE aenc_G726_encode(HANDLE hObj,void *pvIn, void *pvOut, DWORD dwOutSize, DWORD *pdwSize);
static SCODE aenc_G726_reset_framecount(HANDLE hObj);

//==XML call back function=========================================================================
SCODE AxeXmlTree_G726(void *userData, const char *name, const char **atts);

//=== Codec register===============================================================================
SCODE register_G726(TCodecOperation *pOper, TCodecOpt *pOpt)
{
	memset(pOper, 0, sizeof(TCodecOperation));

	/* Codec Operation set-up */
	pOper->dwFourCC = FOURCC_G726;

	/* Codec functions setup */
	pOper->fnInit = &aenc_G726_init;
	pOper->fnRelease = &aenc_G726_release;
	pOper->fnGetConf = &aenc_G726_get_conf;
	pOper->fnQrOutSz = &aenc_G726_qr_out_size;
	pOper->fnQrSpPerBufCh= &aenc_G726_qr_sp_per_ch_buf;	
	pOper->fnEncode = &aenc_G726_encode;
	pOper->fnRetsetFrameCount = &aenc_G726_reset_framecount;
	/* Default codec option */
	pOpt->dwFourCC = FOURCC_G726;	
    pOpt->dwSampRate = 8000;
	pOpt->dwChanNum = 1;	//no channel setting
	pOpt->dwFramePerBuffer = 1;	//no frame setting
	pOpt->ptOper = pOper;	

	return S_OK;
}

static SCODE aenc_G726_reset_framecount(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencG726Info  *ptInfo = (TAencG726Info *)pCodecOpt->hOpt;
	
	ptInfo->dwFrameCount = 0;
	
	return S_OK;
	
}
static SCODE aenc_G726_init(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
	TG726EncInitOptions tInitOptions;	
	TAencG726Info *ptInfo;	
	int i = 0;
	if ((ptInfo = malloc(sizeof(TAencG726Info))) == NULL) {
		fprintf(stderr, "Memory is not Enough!\n");
	}
	memset(ptInfo, 0, sizeof(TAencG726Info));
	
	ptInfo->dwHandleNum = pCodecOpt->dwCodecHanldeNum;  
	
	memset(&tInitOptions, 0, sizeof(TG726EncInitOptions));

    tInitOptions.dwVersion = G726ENC_VERSION;
    tInitOptions.dwBufferSize = 0;
    tInitOptions.pInBuffer = NULL;
    tInitOptions.dwSemaphore = (DWORD)NULL;
    tInitOptions.dwUserData = (DWORD)NULL;
    tInitOptions.pfnSend = NULL;
    tInitOptions.pfnEnter = NULL;
    tInitOptions.pfnLeave = NULL;
    tInitOptions.pObjectMem = NULL;
    tInitOptions.dwBlockSize = SAMPLE_PER_FRAME;
    tInitOptions.dwMode = 0;

	/*! Output ADPCM data bitrate: 2 for 16 kbps, 3 for 24 kbps, 4 for 32 kbps,and 5 for 40 kbps G.726 ADPCM bitstream. */
	if (pCodecOpt->dwBitRate == 16000) {
		tInitOptions.swBitRate = 2;
	} else if (pCodecOpt->dwBitRate == 24000) {
		tInitOptions.swBitRate = 3;
	} else if (pCodecOpt->dwBitRate == 32000) {
		tInitOptions.swBitRate = 4;
	} else { // default set to 40kbps
		tInitOptions.swBitRate = 5;	
	}
			
    tInitOptions.bFrameBased = TRUE;	// Use frame-based mode(TRUE) or not(FALSE)
    tInitOptions.pObjectMem = NULL;

	ptInfo->phCoreEnc = (HANDLE *)malloc(sizeof(HANDLE)*ptInfo->dwHandleNum);
	
	for (i = 0; i < ptInfo->dwHandleNum; i++)
	{
		//OutputSize:for one channel, if read more than one channel, it's needed to be modified!!
		if (G726Enc_Initial(&(ptInfo->phCoreEnc[i]), &tInitOptions) != S_OK) {
			printf("[AENC] Initialize G726 Fail !!\n");
			return S_FAIL;
		}

		//printf("%s:%d: %p G726Enc initialized!\n", __FILE__, __LINE__, ptInfo->phCoreEnc[i]);
	}
	pCodecOpt->hOpt = (HANDLE)ptInfo;

	return S_OK;
}

static SCODE aenc_G726_release(HANDLE *phObj)
{
	TAencG726Info *ptInfo = (TAencG726Info *)*phObj;
	int i = 0;
	if (ptInfo != NULL)
	{
		for (i = 0; i < ptInfo->dwHandleNum; i++)
		{
			if (G726Enc_Release(&(ptInfo->phCoreEnc[i])) != S_OK)
			{
				fprintf(stderr, "%s Fail!\n", __func__);			
			}
		}
		free(*phObj);		
		phObj = NULL;		
	}
	return S_OK;
}

static SCODE aenc_G726_get_conf(HANDLE hObj, HANDLE pvOut, DWORD *pdwSize)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TUBufferConfG726	*ptConf = (TUBufferConfG726	*) pvOut;	

	memset(ptConf, 0, sizeof(TUBufferConfG726)); 
	ptConf->dwSize = sizeof(TUBufferConfG726);
	ptConf->dwDataType = FOURCC_CONF;
	ptConf->dwFollowingDataType = FOURCC_G726;
	ptConf->dwTrackID = 2;				

	ptConf->dwCodewordBits = pCodecOpt->dwBitRate / pCodecOpt->dwSampRate;
	ptConf->dwChannelNumber = pCodecOpt->dwChanNum;
	*pdwSize = sizeof(TUBufferConfG726);
				
	//printf("%s:%d: G726Enc write conf!\n", __FILE__, __LINE__);
	return S_OK;
}

static DWORD aenc_G726_qr_sp_per_ch_buf(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
	return pCodecOpt->dwFramePerBuffer * SAMPLE_PER_FRAME;
}

static DWORD aenc_G726_qr_out_size(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
				
	DWORD	dwData = sizeof(TUBuffer) + 
		SAMPLE_PER_FRAME * pCodecOpt->dwFramePerBuffer + MAX_USER_DATA_SIZE;
	DWORD	dwConf = sizeof(TUBufferConfG726);

	return (dwData > dwConf) ? dwData : dwConf;	
}

static SCODE aenc_G726_encode(HANDLE hObj, void *pvIn, void *pvOut, DWORD dwOutSize, DWORD *pdwSize)
{	
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencG726Info  *ptInfo = (TAencG726Info *) pCodecOpt->hOpt;
	TG726EncState       tG726EncState;
	TG726EncOptions		tG726EncOpt;
	TUBuffer			*ptUB;

    tG726EncOpt.dwVersion = G726ENC_VERSION;
	if (pCodecOpt->dwBitRate == 16000) {
		tG726EncOpt.swBitRate = 2;
	} else if (pCodecOpt->dwBitRate == 24000) {
		tG726EncOpt.swBitRate = 3;
	} else if (pCodecOpt->dwBitRate == 32000) {
		tG726EncOpt.swBitRate = 4;
	} else { // default set to 40kbps
		tG726EncOpt.swBitRate = 5;	
	}

    tG726EncOpt.dwMode = 0;
    tG726EncOpt.dwBlockSize = SAMPLE_PER_FRAME;
	tG726EncOpt.dwBufSize = SAMPLE_PER_FRAME;

    tG726EncOpt.pInBuffer = pvIn;
    tG726EncOpt.pOutBuffer = (BYTE*)pvOut + sizeof(TUBuffer);

	if (G726Enc_SetOptions(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tG726EncOpt) != S_OK) {
		return S_FAIL;
	}


	memset(&tG726EncState, 0, sizeof(TG726EncState));
	// encode one frame
	if (G726Enc_OneFrame(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tG726EncState) != S_OK) {
		printf("[AENC] G726Enc_OneFrame failed\n");
		return S_FAIL;
	}

//	printf("Encode successfully!! tG726EncState.dwOutSize = %u \n", tG726EncState.dwOutSize);

	ptInfo->dwSeqNo++;
	
	ptUB = (TUBuffer *)pvOut;
	ptUB->dwDataType = FOURCC_G726;
	ptUB->dwSeqNo = ptInfo->dwSeqNo;
	ptUB->bIsSync = TRUE;
	ptUB->dwCompositionOffset = 0;
	ptUB->bIsBoundary = TRUE;
	ptUB->dwUserDataSize = 0;
	*pdwSize = ptUB->dwSize = sizeof(TUBuffer) + tG726EncState.dwOutSize;

	return S_OK;
}

//==XML call back function===========================================================================
SCODE AxeXmlTree_G726(void *userData, const char *name, const char **atts)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;			
	TCodecOpt			*ptOpt = &(ptaxHandle->ptAencInfo->atcoSaved[0]);	
	int					i;
	
	if (ptaxHandle->bFirstLoad)
	{
		DBPRINT1("%s:\n",__func__)
		for (i = 0; i < MAX_CODEC_NUM; i ++) {
			if (ptOpt->dwFourCC == FOURCC_G726) {
				ptaxHandle->hTmp = ptOpt;
				return S_OK;
			}
			ptOpt ++;
		}

		ptaxHandle->hTmp = NULL;
	}
	return IGNORE_CHILD_CONFIG;	
}

#endif

