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
 * aenc_aac4.c
 *
 * \brief
 * Audio encoder - AAC4
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
#ifdef __AAC4_Enc__
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
#include "AAC4Enc.h"

//=====================================================================================
#define	MAX_USER_DATA_SIZE	15
#define SAMPLE_PER_FRAME	1024
//=====================================================================================
typedef struct aenc_aac4_info{
	HANDLE	*phCoreEnc;
	DWORD	dwSeqNo;
	DWORD   dwHandleNum;
	DWORD	dwFrameCount;
	//void 	*pvOut; /* saved value, not allocated by myself */
} TAencAAC4Info;

//==function pointer====================================================================
static SCODE aenc_AAC4_init(HANDLE hObj);
static SCODE aenc_AAC4_release(HANDLE *phObj);
static SCODE aenc_AAC4_get_conf(HANDLE hObj, void *pvOut, DWORD *pdwSize);
static DWORD aenc_AAC4_qr_out_size(HANDLE hObj);
static DWORD aenc_AAC4_qr_sp_per_ch_buf(HANDLE hObj);
//static SCODE aenc_AAC4_setio(HANDLE hObj, void *pvIn, void *pvOut, DWORD dwOutSize);
static SCODE aenc_AAC4_encode(HANDLE hObj, HANDLE pvIn, HANDLE pvOut, DWORD dwOutSize, DWORD *pdwSize);
static SCODE aenc_AAC4_reset_framecount(HANDLE hObj);
/* XML elements -- implementation */
/**********************************/
SCODE AxeXmlTree_AAC4(void *userData, const char *name, const char **atts);

/* Regular functions */
/*********************/
SCODE register_AAC4(TCodecOperation *pOper, TCodecOpt *pOpt)
{
	memset(pOper, 0, sizeof(TCodecOperation));
    memset(pOpt, 0, sizeof(TCodecOpt));
	/* Codec Operation set-up */
	pOper->dwFourCC = FOURCC_AAC4;

	/* Codec functions setup */
	pOper->fnInit = &aenc_AAC4_init;
	pOper->fnRelease = &aenc_AAC4_release;
	pOper->fnGetConf = &aenc_AAC4_get_conf;
	pOper->fnQrOutSz = &aenc_AAC4_qr_out_size;
	pOper->fnQrSpPerBufCh= &aenc_AAC4_qr_sp_per_ch_buf;
	pOper->fnEncode = &aenc_AAC4_encode;
	pOper->fnRetsetFrameCount = &aenc_AAC4_reset_framecount;
	/* Default codec option */
	pOpt->dwFourCC = FOURCC_AAC4;
    //pOpt->dwBitRate= ; set later
    //pOpt->dwSampRate= ; set later
    //pOpt->dwChanNum= ; set later
    pOpt->dwFramePerBuffer = 1;
	pOpt->ptOper = pOper;	

	return S_OK;
}

static SCODE aenc_AAC4_reset_framecount(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencAAC4Info  *ptInfo = (TAencAAC4Info  *) pCodecOpt->hOpt;
	ptInfo->dwFrameCount = 0;
	
	return S_OK;
}
static SCODE aenc_AAC4_init(HANDLE hObj)
{
	SCODE scRet;
	TAencAAC4Info  *ptInfo;
	TAAC4EncInitOptions		tAAC4EncInitOpt;
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
	int i = 0;
	ptInfo = (TAencAAC4Info *) malloc(sizeof(TAencAAC4Info));
	if (ptInfo == NULL) {
		return ERR_OUT_OF_MEMORY;
	}
	memset(ptInfo, 0, sizeof(TAencAAC4Info));

	ptInfo->dwHandleNum = pCodecOpt->dwCodecHanldeNum;  

	// Initial AAC4 encoder
	memset(&tAAC4EncInitOpt, 0, sizeof(TAAC4EncInitOptions));
	tAAC4EncInitOpt.dwVersion = AAC4ENC_VERSION;
	tAAC4EncInitOpt.dwFlags	= AAC4ENC_NONCIRCULAR_BUFFER;
    tAAC4EncInitOpt.pfnOnEnterCriticalSec = NULL;
    tAAC4EncInitOpt.pfnOnLeaveCriticalSec = NULL;
    tAAC4EncInitOpt.pfnOnSendData = NULL;
	tAAC4EncInitOpt.dwSemaphore	= (DWORD)NULL;
    tAAC4EncInitOpt.dwUserData = (DWORD)NULL;
    tAAC4EncInitOpt.pObjectMem = NULL;
    tAAC4EncInitOpt.pswInBuffer = NULL;
    tAAC4EncInitOpt.pbyBuf = NULL;
	tAAC4EncInitOpt.bMpeg4 = 1;	// use MPEG4
	tAAC4EncInitOpt.bPow34 = 1;
	tAAC4EncInitOpt.dwEncBufSize = 0; // noncircular mode
	tAAC4EncInitOpt.sdwBitRate = pCodecOpt->dwBitRate;
    tAAC4EncInitOpt.dwSampleRate = pCodecOpt->dwSampRate;
	tAAC4EncInitOpt.swBandWidth = -1;
    tAAC4EncInitOpt.wADTS = 0; // 0:raw data 1:ADTS format (be playable)
    // tAAC4EncInitOpt.wADTS = 1;
	if (pCodecOpt->dwChanNum == 2)
	{
		tAAC4EncInitOpt.swStereoMode = 0;
		tAAC4EncInitOpt.wChannel = 2;		
	}
	else
	{
		tAAC4EncInitOpt.swStereoMode = 3;
		tAAC4EncInitOpt.wChannel = 1;
	}
	//printf("%d ptInfo->dwHandleNum = %d\n",__LINE__,ptInfo->dwHandleNum);
	
	ptInfo->phCoreEnc = (HANDLE *)malloc(sizeof(HANDLE)*ptInfo->dwHandleNum);
	//printf("[AENC]Before calling AAC4Enc_Initial():pCodecOpt->dwChanNum=%d pCodecOpt->dwBitRate=%d pCodecOpt->dwSampRate=%d\n",pCodecOpt->dwChanNum,pCodecOpt->dwBitRate,pCodecOpt->dwSampRate);
	for (i = 0; i < ptInfo->dwHandleNum; i++)
	{
	
		if((scRet = AAC4Enc_Initial(&(ptInfo->phCoreEnc[i]), &tAAC4EncInitOpt)) != S_OK)
		{
			if (scRet == AUDIO_S_OUTPUTBUFFER)
			{
				fprintf(stderr, "Warning : the output buffer (TAAC4EncInitOptions::dwEncBufSize) is not enough\n");
			}
			else if (scRet == AUDIO_E_BITRATE)
			{
				fprintf(stderr, "Warning : bitrate is too high, output buffer is not enough\n");
				return S_FAIL;
			}		
			else
			{
				fprintf(stderr, "%s:%d:Error initialize AAC4 encoder : %x!\n", __FILE__, __LINE__, scRet);
				return S_FAIL;
			}
		}
		
	}
	//printf("%s:%d: %p AAC4Enc initialized!\n", __FILE__, __LINE__, ptInfo->hCoreEnc);

	pCodecOpt->hOpt = (HANDLE)ptInfo;
	return S_OK;
}

static SCODE aenc_AAC4_release(HANDLE *phObj)
{
	TAencAAC4Info  *ptInfo = *phObj;
	int i = 0;
	if (*phObj != NULL) {
		//printf("%d ptInfo->dwHandleNum = %d\n",__LINE__,ptInfo->dwHandleNum);
		for (i = 0; i < ptInfo->dwHandleNum; i++)
		{
			if (AAC4Enc_Release(&ptInfo->phCoreEnc[i]) != S_OK)
			{
				fprintf(stderr, "%s Fail!\n", __func__);
			}
		}
		free(*phObj);
		*phObj = NULL;
	}
	
	if (ptInfo->phCoreEnc != NULL)
	{
		free(ptInfo->phCoreEnc);
		ptInfo->phCoreEnc=NULL;
	}

	return S_OK;
}

static SCODE aenc_AAC4_get_conf(HANDLE hObj, void *pvOut, DWORD *pdwSize)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencAAC4Info  *ptInfo = pCodecOpt->hOpt;
	DWORD	dwSpecConfSize;
	TUBufferConfAAC4	*ptConf = (TUBufferConfAAC4	*) pvOut;
	
	memset(ptConf, 0, sizeof(TUBufferConfAAC4));
	// TODO : the last field
	if (AAC4Enc_SpecificConfig(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], pvOut + sizeof(TUBufferConfAAC4), &dwSpecConfSize, &(ptConf->dwProfileLevel), 4) != S_OK)
	{
		return S_FAIL;
	}
	*pdwSize = ptConf->dwSize = sizeof(TUBufferConfAAC4) + dwSpecConfSize;
	ptConf->dwDataType = FOURCC_CONF;
	ptConf->dwFollowingDataType = FOURCC_AAC4;
	ptConf->dwTrackID = 2;
	ptConf->dwSampleRate = pCodecOpt->dwSampRate;
	ptConf->dwChannelNumber = pCodecOpt->dwChanNum;

	return S_OK;
}

#if 0
static SCODE aenc_AAC4_setio(HANDLE hObj, void *pvIn, void *pvOut, DWORD dwOutSize)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencAAC4Info  *ptInfo = pCodecOpt->hOpt;	
	TAAC4EncOptions 	tAAC4EncOpt;

	ptInfo->pvOut = pvOut;

	tAAC4EncOpt.dwVersion		= AAC4Enc_VERSION;
	tAAC4EncOpt.dwFlags			= AAC4Enc_SET_INPUTPCMBUFFER | AAC4Enc_SET_OUTPUTBUFFER;
	tAAC4EncOpt.pInBuffer		= pvIn;
	tAAC4EncOpt.pInterOutBuffer	= pvOut + sizeof(TUBuffer);
	tAAC4EncOpt.dwOutBufSize	= dwOutSize;

	if (AAC4Enc_SetOptions(ptInfo->hCoreEnc, &tAAC4EncOpt) != S_OK)
	{
		return S_FAIL;
	}
	return S_OK;
}
#endif

static SCODE aenc_AAC4_encode(HANDLE hObj, HANDLE pvIn, HANDLE pvOut, DWORD dwOutSize, DWORD *pdwSize)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencAAC4Info  *ptInfo = (TAencAAC4Info  *)pCodecOpt->hOpt;
	TAAC4EncState       tAAC4EncState;
	TUBuffer			*ptUB;
	//===Set IO
	//aenc_AAC4_setio(hObj, pvIn, pvOut, dwOutSize);
	//TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	//TAencAAC4Info  *ptInfo = pCodecOpt->hOpt;	
	TAAC4EncOptions 	tAAC4EncOpt;

	//ptInfo->pvOut = pvOut;

	tAAC4EncOpt.dwVersion		= AAC4ENC_VERSION;
	tAAC4EncOpt.dwFlags			= AAC4ENC_SET_INPUTPCMBUFFER | AAC4ENC_SET_OUTPUTBUFFER;
	tAAC4EncOpt.pInBuffer		= pvIn;
	tAAC4EncOpt.pInterOutBuffer	= (BYTE*)pvOut + sizeof(TUBuffer);
	tAAC4EncOpt.dwOutBufSize	= dwOutSize;
	
	if (AAC4Enc_SetOptions(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tAAC4EncOpt) != S_OK)
	{
		return S_FAIL;
	}
	memset(&tAAC4EncState, 0, sizeof(TAAC4EncState));
	AAC4Enc_FlushBuffer(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex]);
	if(AAC4Enc_OneFrame(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tAAC4EncState) != S_OK)
	{
		printf("AAC4Enc_OneFrame failed\n");
		return S_FAIL;
	}
	AAC4Enc_FlushBuffer(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex]);
	ptInfo->dwSeqNo ++;
	ptInfo->dwFrameCount ++;
	//ptUB = (TUBuffer *)ptInfo->pvOut;
	//printf("AAC bitstream size:%d seqNo:%d frame_count:%d\n",tAAC4EncState.dwBitreamSize,ptInfo->dwSeqNo,ptInfo->dwFrameCount);
    ptUB = (TUBuffer *)pvOut;
	
	if ((ptInfo->dwFrameCount == 1) ||  (ptInfo->dwFrameCount == 2))
	{
		tAAC4EncState.dwBitreamSize = 0;
	}
	
	*pdwSize = ptUB->dwSize = sizeof(TUBuffer) + tAAC4EncState.dwBitreamSize*pCodecOpt->dwFramePerBuffer;
	ptUB->dwDataType = FOURCC_AAC4;
	ptUB->dwSeqNo = ptInfo->dwSeqNo;
	ptUB->bIsSync = TRUE;
	ptUB->dwCompositionOffset = 0;
	ptUB->bIsBoundary = TRUE;
	ptUB->dwUserDataSize = 0;
	
	return S_OK;
}

static DWORD aenc_AAC4_qr_sp_per_ch_buf(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	return pCodecOpt->dwFramePerBuffer * SAMPLE_PER_FRAME;
}

static DWORD aenc_AAC4_qr_out_size(HANDLE hObj)
{
	// TODO : how to determine size
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	SWORD	swStereoMode = (pCodecOpt->dwChanNum == 2) ? 0 : 3 ;
	DWORD	dwData = sizeof(TUBuffer) + AAC4Enc_GetReqBuffSize(pCodecOpt->dwSampRate, pCodecOpt->dwBitRate, swStereoMode ) * pCodecOpt->dwFramePerBuffer + MAX_USER_DATA_SIZE;
	DWORD	dwConf = sizeof(TUBufferConfAAC4);

	return (dwData > dwConf) ? dwData : dwConf;
}

SCODE AxeXmlTree_AAC4(void *userData, const char *name, const char **atts)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;			
	TCodecOpt			*ptOpt = &(ptaxHandle->ptAencInfo->atcoSaved[0]);	
	int					i;

	
	if (ptaxHandle->bFirstLoad)
	{
		DBPRINT1("%s:\n",__func__)
		for (i = 0; i < MAX_CODEC_NUM; i ++) {
			if (ptOpt->dwFourCC == FOURCC_AAC4) {
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
