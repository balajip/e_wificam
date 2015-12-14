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
 * aenc_g711.c
 *
 * \brief
 * Audio encoder - G711
 *
 * \date
 * 2007/01/25
 *
 * \author
 * Ming-Jing Tsai
 *
 *
 *******************************************************************************
 */
#ifdef __G711_Enc__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "global_codec.h"
#include "errordef.h"
#include "typedef.h"
#include "ubuffer.h"
#include "aenc.h"
#include "dbgdefs.h"
#include "xmlwrapper.h"   //Error code
#include "G711Enc.h"
//=================================================================================================
#define	MAX_USER_DATA_SIZE	15
#define SAMPLE_PER_FRAME	1024//1024 sample out for VLC use

//=================================================================================================
typedef struct aenc_g711_info{
	HANDLE	*phCoreEnc;	
	DWORD	dwSeqNo;
	DWORD   dwHandleNum;
	DWORD	dwFrameCount;
	//void 	*pvOut; /* saved value, not allocated by myself */
} TAencG711Info;

//==function pointer===============================================================================
static SCODE aenc_G711_init(HANDLE hObj);
static SCODE aenc_G711_release(HANDLE *phObj);
static SCODE aenc_G711_get_conf(HANDLE hObj, void *pvOut, DWORD *pdwSize);
static DWORD aenc_G711_qr_out_size(HANDLE hObj);
static DWORD aenc_G711_qr_sp_per_ch_buf(HANDLE hObj);
static SCODE aenc_G711_encode(HANDLE hObj,void *pvIn, void *pvOut, DWORD dwOutSize, DWORD *pdwSize);
static SCODE aenc_G711_reset_framecount(HANDLE hObj);
//==XML call back function=========================================================================
SCODE AxeXmlTree_G711(void *userData, const char *name, const char **atts);
void AxeXmlTree_G711_Mode(void *userData, const char *name, int len);

//=== Codec register===============================================================================
SCODE register_G711(TCodecOperation *pOper, TCodecOpt *pOpt)
{
	memset(pOper, 0, sizeof(TCodecOperation));

	/* Codec Operation set-up */
	pOper->dwFourCC = FOURCC_G711;

	/* Codec functions setup */
	pOper->fnInit = &aenc_G711_init;
	pOper->fnRelease = &aenc_G711_release;
	pOper->fnGetConf = &aenc_G711_get_conf;
	pOper->fnQrOutSz = &aenc_G711_qr_out_size;
	pOper->fnQrSpPerBufCh= &aenc_G711_qr_sp_per_ch_buf;	
	pOper->fnEncode = &aenc_G711_encode;
	pOper->fnRetsetFrameCount = &aenc_G711_reset_framecount;
	/* Default codec option */
	pOpt->dwFourCC = FOURCC_G711;	
	pOpt->dwChanNum= 1;//no channel setting
	pOpt->dwFramePerBuffer = 1;//no frame setting
	pOpt->ptOper = pOper;	

	return S_OK;
}
static SCODE aenc_G711_reset_framecount(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencG711Info  *ptInfo = (TAencG711Info *)pCodecOpt->hOpt;
	
	ptInfo->dwFrameCount = 0;
	
	return S_OK;
}
static SCODE aenc_G711_init(HANDLE hObj)
{
	TCodecOpt *ptOpt = (TCodecOpt *) hObj;
	TG711EncInitOptions tInitOptions;	
	TAencG711Info *ptInfo;	
	int i = 0;
	if ((ptInfo = malloc(sizeof(TAencG711Info))) == NULL)
	{
		fprintf(stderr, "Memory is not Enough!\n");
	}
	memset(ptInfo, 0, sizeof(TAencG711Info));

	ptInfo->dwHandleNum = ptOpt->dwCodecHanldeNum;  

	memset(&tInitOptions, 0, sizeof(TG711EncInitOptions));
	tInitOptions.dwVersion = G711ENC_VERSION;
	//OutputSize:for one channel, if read more than one channel, it's needed to be modified!!
	tInitOptions.dwInFrameSize = SAMPLE_PER_FRAME;
	tInitOptions.eEncMode = ptOpt->eEncMode;	
	/*internal mem*/
	tInitOptions.pObjectMem = NULL;
	ptInfo->phCoreEnc = (HANDLE *)malloc(sizeof(HANDLE)*ptInfo->dwHandleNum);
	for (i = 0; i < ptInfo->dwHandleNum; i++)
	{
		if (G711Enc_Initial(&(ptInfo->phCoreEnc[i]), &tInitOptions) != S_OK)
		{
			printf("Initialize G711 FSail !!\n");
			return S_FAIL;
		}

		//printf("%s:%d: %p G711Enc initialized!\n", __FILE__, __LINE__, ptInfo->phCoreEnc[i]);
	}
	ptOpt->hOpt = (HANDLE)ptInfo;
	return S_OK;
}

static SCODE aenc_G711_release(HANDLE *phObj)
{
	TAencG711Info *ptInfo = (TAencG711Info *)*phObj;
	int i = 0;
	if (ptInfo != NULL)
	{
		for (i = 0; i < ptInfo->dwHandleNum; i++)
		{
			if (G711Enc_Release(&(ptInfo->phCoreEnc[i])) != S_OK)
			{
				fprintf(stderr, "%s Fail!\n", __func__);			
			}	
		}	
		free(*phObj);		
		phObj = NULL;		
	}

	return S_OK;
}

static SCODE aenc_G711_get_conf(HANDLE hObj, HANDLE pvOut, DWORD *pdwSize)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TUBufferConfG711	*ptConf = (TUBufferConfG711	*) pvOut;	

	memset(ptConf, 0, sizeof(TUBufferConfG711)); 
	ptConf->dwSize = sizeof(TUBufferConfG711);
	ptConf->dwDataType = FOURCC_CONF;
	ptConf->dwFollowingDataType = FOURCC_G711;
	ptConf->dwTrackID = 2;				
	if (pCodecOpt->eEncMode == G711ENC_MODE_U_LAW)
	{
		ptConf->dwCompressionLaw = FOURCC_ULAW;	
	}
	else
	{
		ptConf->dwCompressionLaw = FOURCC_ALAW;	
	}
	ptConf->dwChannelNumber = pCodecOpt->dwChanNum;
	*pdwSize = sizeof(TUBufferConfG711);
				
	return S_OK;
}

static DWORD aenc_G711_qr_sp_per_ch_buf(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
	return pCodecOpt->dwFramePerBuffer * SAMPLE_PER_FRAME;
}

static DWORD aenc_G711_qr_out_size(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
				
	DWORD	dwData = sizeof(TUBuffer) + 
		SAMPLE_PER_FRAME * pCodecOpt->dwFramePerBuffer + MAX_USER_DATA_SIZE;
	DWORD	dwConf = sizeof(TUBufferConfG711);

	return (dwData > dwConf) ? dwData : dwConf;	
}

static SCODE aenc_G711_encode(HANDLE hObj, void *pvIn, void *pvOut, DWORD dwOutSize, DWORD *pdwSize)
{	
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencG711Info  *ptInfo = (TAencG711Info *) pCodecOpt->hOpt;
	TG711EncState       tState;
	TUBuffer			*ptUB;
	
	memset(&tState, 0, sizeof(TG711EncState));
	tState.pswInFrame = pvIn;
	tState.pbyOutFrame = pvOut + sizeof(TUBuffer);
	if (G711Enc_ProcessOneFrame(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tState) != S_OK)
	{
		printf("G711Enc Fail \n");
		return S_FAIL;
	}
	ptInfo->dwSeqNo ++;

	ptUB = (TUBuffer *)pvOut;
	ptUB->dwDataType = FOURCC_G711;
	ptUB->dwSeqNo = ptInfo->dwSeqNo;
	ptUB->bIsSync = TRUE;
	ptUB->dwCompositionOffset = 0;
	ptUB->bIsBoundary = TRUE;
	ptUB->dwUserDataSize = 0;
	*pdwSize = ptUB->dwSize = sizeof(TUBuffer) + SAMPLE_PER_FRAME * pCodecOpt->dwFramePerBuffer;
	
	return S_OK;
}

//==XML call back function===========================================================================
SCODE AxeXmlTree_G711(void *userData, const char *name, const char **atts)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;			
	TCodecOpt			*ptOpt = &(ptaxHandle->ptAencInfo->atcoSaved[0]);	
	int					i;
	
	if (ptaxHandle->bFirstLoad)
	{
		DBPRINT1("%s:\n",__func__)
		for (i = 0; i < MAX_CODEC_NUM; i ++) {
			if (ptOpt->dwFourCC == FOURCC_G711) {
				ptaxHandle->hTmp = ptOpt;
				return S_OK;
			}
			ptOpt ++;
		}

		ptaxHandle->hTmp = NULL;
	}
	return IGNORE_CHILD_CONFIG;	
}

void AxeXmlTree_G711_Mode(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;
	TAFConf *ptacConf = ptaxHandle->ptConfTmp;
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;
	
	EG711EncMode eEncMode;

	DBPRINT2("%s Opt %x:",__func__, *ptOpt)
	if (ptOpt == NULL) {
		return;
	}
	
	if (strncmp("ulaw", name, len) == 0)		
	{
		eEncMode = G711ENC_MODE_U_LAW;
		DBPRINT1("ulaw %d\n", ptOpt->eEncMode)
	}
	else
	{
		eEncMode = G711ENC_MODE_A_LAW;
		DBPRINT1("alaw %d\n", ptOpt->eEncMode)
	}
	
	if (ptOpt->eEncMode != eEncMode)
	{
		ptOpt->eEncMode = eEncMode;
		ptacConf->bEncMode = TRUE;
	}

}

#endif

