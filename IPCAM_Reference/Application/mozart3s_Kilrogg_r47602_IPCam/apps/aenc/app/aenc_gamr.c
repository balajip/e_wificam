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
 * Audio encoder - GAMR
 *
 * \date
 * 2006/06/28
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */
#ifdef __GAMR_Enc__
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
#include "GAMREnc.h"
#include "rtpamrpack.h"
#include "amr_rtp2storage.h"
#include "dbgdefs.h"
#include "xmlwrapper.h"   //Error code

//=====================================================================================
#define MAX_FRAME_SIZE      32
#define MAX_USER_DATA_SIZE  16
#define SAMPLE_PER_FRAME    160

//=====================================================================================
typedef struct aenc_gamr_info{
    HANDLE  *phCoreEnc;
    HANDLE  hPack;
    DWORD   dwSeqNo;
    void    *pvBuf;
    DWORD   dwBufSize;
    //void  *pvOut; /* saved value, not allocated by myself */
    BYTE    *pbyAencOut;
    BYTE    *pbyAudioIn;
    DWORD	dwReservedSize;
	DWORD   dwHandleNum;
	DWORD	dwFrameCount;
} TAencGAMRInfo;

//==function pointer====================================================================
static SCODE aenc_GAMR_init(HANDLE hObj);
static SCODE aenc_GAMR_release(HANDLE *phObj);
static SCODE aenc_GAMR_get_conf(HANDLE hObj, void *pvOut, DWORD *pdwSize);
static DWORD aenc_GAMR_qr_out_size(HANDLE hObj);
static DWORD aenc_GAMR_qr_sp_per_ch_buf(HANDLE hObj);
//static SCODE aenc_GAMR_setio(HANDLE hObj, void *pvIn, void *pvOut, DWORD dwOutSize);
static SCODE aenc_GAMR_encode(HANDLE hObj, HANDLE pvIn, HANDLE pvOut, DWORD dwOutSize, DWORD *pdwSize);
static SCODE aenc_GAMR_reset_framecount(HANDLE hObj);
/* XML elements -- declaration */
/*******************************/
SCODE AxeXmlTree_GAMR(void *userData, const char *name, const char **atts);
void AxeXmlTree_GAMR_FrPerBuf(void *userData, const char *name, int len);
//=====================================================================================

/* Codec register */
SCODE register_GAMR(TCodecOperation *pOper, TCodecOpt *pOpt)
{
    memset(pOper, 0, sizeof(TCodecOperation));
    memset(pOpt, 0, sizeof(TCodecOpt));
    /* Codec Operation set-up */
    pOper->dwFourCC = FOURCC_GAMR;

    /* Codec functions setup */
    pOper->fnInit = &aenc_GAMR_init;
    pOper->fnRelease = &aenc_GAMR_release;
    pOper->fnGetConf = &aenc_GAMR_get_conf;
    pOper->fnQrOutSz = &aenc_GAMR_qr_out_size;
    pOper->fnQrSpPerBufCh= &aenc_GAMR_qr_sp_per_ch_buf;
    pOper->fnEncode = &aenc_GAMR_encode;
	pOper->fnRetsetFrameCount = &aenc_GAMR_reset_framecount;
    /* Default codec option */
    pOpt->dwFourCC = FOURCC_GAMR;
    //pOpt->dwBitRate= ; set later
    pOpt->dwSampRate = 8000;
    pOpt->dwChanNum= 1;
    pOpt->dwFramePerBuffer = 4;
    pOpt->ptOper = pOper;

    return S_OK;
}
static SCODE aenc_GAMR_reset_framecount(HANDLE hObj)
{
	TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;	
	TAencGAMRInfo  *ptInfo = (TAencGAMRInfo *)pCodecOpt->hOpt;
	
	ptInfo->dwFrameCount = 0;
	
	return S_OK;
}

static SCODE aenc_GAMR_init(HANDLE hObj)
{
    TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
    TAencGAMRInfo  *ptInfo;
    TGAMREncInitOptions     tGAMREncInitOpt;
    TRTPAMRPackInitOptions      tRTPAMRPackInitOpt;
	int i = 0;
    ptInfo = (TAencGAMRInfo *) malloc(sizeof(TAencGAMRInfo));

    if (ptInfo == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    memset(ptInfo, 0, sizeof(TAencGAMRInfo));
	
	ptInfo->dwHandleNum = pCodecOpt->dwCodecHanldeNum;  
	
    /* multiply by 2 is for IO buffer of RTPAMRPack */
    ptInfo->dwBufSize = 
        (MAX_FRAME_SIZE * pCodecOpt->dwFramePerBuffer + MAX_USER_DATA_SIZE) * 2;
    ptInfo->pvBuf = malloc(ptInfo->dwBufSize);

    if (ptInfo->pvBuf == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    // Initial GAMR encoder
    memset(&tGAMREncInitOpt, 0, sizeof(tGAMREncInitOpt));
    tGAMREncInitOpt.dwVersion   = GAMRENC_VERSION;
    tGAMREncInitOpt.dwFlags     = GAMRENC_DTX | GAMRENC_VAD | GAMRENC_OUTPUTFORMAT;
    tGAMREncInitOpt.pObjectMem  = NULL;
    tGAMREncInitOpt.pfnOnEnterCriticalSec       = NULL;
    tGAMREncInitOpt.pfnOnLeaveCriticalSec       = NULL;
    tGAMREncInitOpt.pfnOnSendData               = NULL;
    tGAMREncInitOpt.dwSemaphore                 = (DWORD)NULL;
    tGAMREncInitOpt.dwUserData                  = (DWORD)NULL;
    tGAMREncInitOpt.bUseCircularBuf             = FALSE;
    tGAMREncInitOpt.dwBufferSize                = (tGAMREncInitOpt.bUseCircularBuf) ? 512 : 0;
    tGAMREncInitOpt.bEnableDTX                  = FALSE;
    tGAMREncInitOpt.dwVADOption                 = 1;
    tGAMREncInitOpt.dwOutputFormat              = GAMROF_PACKED;
    tGAMREncInitOpt.pvSpeechIn                   = NULL;
    switch (pCodecOpt->dwBitRate) 
    {
        case 4750:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR475;
            break;
        case 5150:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR515;
            break;
        case 5900:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR59;
            break;
        case 6700:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR67;
            break;
        case 7400:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR74;
            break;
        case 7950:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR795;
            break;
        case 10200:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR102;
            break;
        case 12200:
        default:
            tGAMREncInitOpt.eMode = GAMRMODEOFMR122;
            break;
    }
	
	ptInfo->phCoreEnc = (HANDLE *)malloc(sizeof(HANDLE)*ptInfo->dwHandleNum);
	
	for (i = 0; i < ptInfo->dwHandleNum; i++)
	{
		if(GAMREnc_Initial(&ptInfo->phCoreEnc[i], &tGAMREncInitOpt)!= S_OK)
		{
			fprintf(stderr, "%s:%d:Error initialize GAMR encoder!\n", __FILE__, __LINE__);
			return S_FAIL;
		}

		//printf("%s:%d: %p GAMREnc initialized!\n", 
			//	__FILE__, __LINE__, ptInfo->phCoreEnc[i]);
	}

    memset(&tRTPAMRPackInitOpt, 0, sizeof(tRTPAMRPackInitOpt));
    tRTPAMRPackInitOpt.dwVersion = RTPAMRPACK_VERSION;
    tRTPAMRPackInitOpt.pObjectMem = NULL;
    tRTPAMRPackInitOpt.dwAMR_IF = AMR_IF1;
    tRTPAMRPackInitOpt.dwPayloadFormat = RTP_AMR_FORMAT_STORAGE; 
    // BandwidthEfficient or OctetAligned
    tRTPAMRPackInitOpt.byModeSet = MODESET_ALL;                             
    // bit 7~0 represent mode 7~0, respectively.
    tRTPAMRPackInitOpt.dwModeChangePeriod = 4;//NUM_FRAMEBLOCK;
    tRTPAMRPackInitOpt.bModeChangeNeighbor = FALSE;
    tRTPAMRPackInitOpt.dwMaxPTime = 4 * 20;     
    // in millisecond, should be multiple of 20ms
    tRTPAMRPackInitOpt.bUseCRC = FALSE;
    tRTPAMRPackInitOpt.bRobustSorting = FALSE;
    tRTPAMRPackInitOpt.dwInterleavingMaxNum = 0;// interleaving
    tRTPAMRPackInitOpt.dwNumChannel = 1;

    if(RTPAMRPack_Initial(&ptInfo->hPack, &tRTPAMRPackInitOpt) != S_OK)
    {
        fprintf(stderr, "%s:%d:Error initialize RTP AMR Packtizer!\n", __FILE__, __LINE__);
        return S_FAIL;
    }
   // printf("%s:%d: RTP AMR Pack initialized!\n", __FILE__, __LINE__);

	// SetOption to set Output buffer size
	{
	    TRTPAMRPackOptions  tRTPAMRPackOpt;
	    tRTPAMRPackOpt.dwVersion  = RTPAMRPACK_VERSION;
	    tRTPAMRPackOpt.dwCMR      = CMR_FRAMETYPE_DEFAULT;
	    tRTPAMRPackOpt.dwAUNumber = pCodecOpt->dwFramePerBuffer;
	    tRTPAMRPackOpt.pIOBuffer = NULL;
	    if (RTPAMRPack_SetOptions(ptInfo->hPack, &tRTPAMRPackOpt) != S_OK)
	    {
	        return S_FAIL;
	    }
	    ptInfo->dwReservedSize = RTPAMRPack_GetReservedSize(ptInfo->hPack);
	}

    pCodecOpt->hOpt = (HANDLE)ptInfo;
    return S_OK;
}

static SCODE aenc_GAMR_release(HANDLE *phObj)
{
    TAencGAMRInfo  *ptInfo = *phObj;
	int i = 0;
    //fprintf(stderr, "%s:%d:\n", __FILE__, __LINE__);
    if (*phObj != NULL) {
		
		for (i = 0; i < ptInfo->dwHandleNum; i++)
		{
			if (GAMREnc_Release(&(ptInfo->phCoreEnc[i])) != S_OK)
			{
				fprintf(stderr, "GAMREnc_Release Fail!\n");         
			}
		}
        if (RTPAMRPack_Release(&ptInfo->hPack) != S_OK)
        {
            fprintf(stderr, "RTPAMRPack_Release Fail!\n");
        }
        free(ptInfo->pvBuf);
        free(*phObj);
        *phObj = NULL;
    }

    return S_OK;
}

static SCODE aenc_GAMR_get_conf(HANDLE hObj, void *pvOut, DWORD *pdwSize)
{
    TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;  
    TUBufferConfGAMR    *ptConf = (TUBufferConfGAMR *) pvOut;

    memset(ptConf, 0, sizeof(TUBufferConfGAMR)); 
    ptConf->dwSize = sizeof(TUBufferConfGAMR);
    ptConf->dwDataType = FOURCC_CONF;
    ptConf->dwFollowingDataType = FOURCC_GAMR;
    ptConf->dwTrackID = 2;
    ptConf->dwSampleRate = pCodecOpt->dwSampRate;
    strncpy(&(ptConf->szVendor[0]), "VIVO", 4);
    ptConf->wModeSet = 0x81FF;
    ptConf->byModeChangePeriod = 0;
    ptConf->byFramesPerSample = pCodecOpt->dwFramePerBuffer;
    *pdwSize = sizeof(TUBufferConfGAMR);
    return S_OK;
}

#if 0
static SCODE aenc_GAMR_setio(HANDLE hObj, void *pvIn, void *pvOut, DWORD dwOutSize)
{
    TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
    TAencGAMRInfo       *ptInfo = pCodecOpt->hOpt;      
    TGAMREncOptions     tGAMREncOpt;
    TRTPAMRPackOptions  tRTPAMRPackOpt;
    DWORD               dwReservedSize; 

    //ptInfo->pvOut = pvOut;

    tRTPAMRPackOpt.dwVersion  = RTPAMRPACK_VERSION;
    tRTPAMRPackOpt.dwCMR      = CMR_FRAMETYPE_DEFAULT;
    tRTPAMRPackOpt.dwAUNumber = pCodecOpt->dwFramePerBuffer;
    tRTPAMRPackOpt.pIOBuffer = ptInfo->pvBuf;
    if (RTPAMRPack_SetOptions(ptInfo->hPack, &tRTPAMRPackOpt) != S_OK)
    {
        return S_FAIL;
    }

    dwReservedSize = 
        RTPAMRPack_GetReservedSize(ptInfo->hPack);

//  fprintf(stderr, "%s:%d:%d, %d\n", __FILE__, __LINE__, dwReservedSize, ptInfo->dwBufSize);

    tGAMREncOpt.dwVersion = GAMRENC_VERSION;
    tGAMREncOpt.dwFlags = GAMRENC_INPUT_BUFFER | GAMRENC_OUTPUT_BUFFER;
    tGAMREncOpt.pSpeechIn   = pvIn;
    tGAMREncOpt.pOutBuffer  = ((BYTE *) ptInfo->pvBuf) + dwReservedSize;
    tGAMREncOpt.dwOutBufferSize = ptInfo->dwBufSize - dwReservedSize;
    if (GAMREnc_SetOptions(ptInfo->hCoreEnc, &tGAMREncOpt) != S_OK)
    {
        return S_FAIL;
    }
    ptInfo->pbyAudioIn = pvIn;
    ptInfo->pbyAencOut = ((BYTE *) ptInfo->pvBuf) + dwReservedSize;
    return S_OK;
}
#endif

static SCODE aenc_GAMR_encode(HANDLE hObj, HANDLE pvIn, HANDLE pvOut, DWORD dwOutSize, DWORD *pdwSize)
{
    int i;
    TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
    TAencGAMRInfo       *ptInfo = pCodecOpt->hOpt;  
    TGAMREncOptions     tGAMREncOpt;
    TGAMREncState       tGAMREncState;
    TRTPAMRPackState    tRTPAMRPackState;
    TUBuffer            *ptUB;
    DWORD               dwOutputSize;
    BYTE                *pbyAudioSrc;
    BYTE                *pbyRawAMR;

    //===Set IO
    //aenc_GAMR_setio(hObj, pvIn, pvOut, dwOutSize);
//  TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
//  TAencGAMRInfo       *ptInfo = pCodecOpt->hOpt;      
//  TGAMREncOptions     tGAMREncOpt;
    TRTPAMRPackOptions  tRTPAMRPackOpt;
    //ptInfo->pvOut = pvOut;

    tRTPAMRPackOpt.dwVersion  = RTPAMRPACK_VERSION;
    tRTPAMRPackOpt.dwCMR      = CMR_FRAMETYPE_DEFAULT;
    tRTPAMRPackOpt.dwAUNumber = pCodecOpt->dwFramePerBuffer;
    tRTPAMRPackOpt.pIOBuffer = ((BYTE *)pvOut) + sizeof(TUBuffer);
    if (RTPAMRPack_SetOptions(ptInfo->hPack, &tRTPAMRPackOpt) != S_OK)
    {
        return S_FAIL;
    }

//  fprintf(stderr, "%s:%d:%d, %d\n", __FILE__, __LINE__, dwReservedSize, ptInfo->dwBufSize);

    tGAMREncOpt.dwVersion = GAMRENC_VERSION;
    tGAMREncOpt.dwFlags = GAMRENC_INPUT_BUFFER | GAMRENC_OUTPUT_BUFFER;
    tGAMREncOpt.pvSpeechIn   = pvIn;
    tGAMREncOpt.pvOutBuffer  = ((BYTE *)pvOut) + sizeof(TUBuffer) + ptInfo->dwReservedSize;
    tGAMREncOpt.dwOutBufferSize = ptInfo->dwBufSize - ptInfo->dwReservedSize;
    if (GAMREnc_SetOptions(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tGAMREncOpt) != S_OK)
    {
        return S_FAIL;
    }
    ptInfo->pbyAudioIn = pvIn;
    ptInfo->pbyAencOut = ((BYTE *)pvOut) + sizeof(TUBuffer) + ptInfo->dwReservedSize;


//  fprintf(stderr, "%s:%d:%p\n", __FILE__, __LINE__, ptInfo->hCoreEnc);
    pbyAudioSrc = ptInfo->pbyAudioIn;
    pbyRawAMR = ptInfo->pbyAencOut;
    for (i=0; i<pCodecOpt->dwFramePerBuffer; i++)
    {
        tGAMREncOpt.dwVersion = GAMRENC_VERSION;
        tGAMREncOpt.dwFlags = GAMRENC_INPUT_BUFFER | GAMRENC_OUTPUT_BUFFER;
        tGAMREncOpt.pvSpeechIn   = pbyAudioSrc;
        tGAMREncOpt.pvOutBuffer  = pbyRawAMR;
        if(GAMREnc_SetOptions(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tGAMREncOpt)!= S_OK)
        {
            fprintf(stderr, "%s:%d: Error set GAMR Options!\n", __FILE__, __LINE__);
            return S_FAIL;
        }
        if(GAMREnc_ProcessOneFrame(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex], &tGAMREncState) != S_OK)
        {
            fprintf(stderr, "%s:%d: GAMREnc_ProcessOneFrame failed\n", __FILE__, __LINE__);
            return S_FAIL;
        }
        dwOutputSize = GAMREnc_QueryFilledBufferSpace(ptInfo->phCoreEnc[pCodecOpt->dwCodecHanldeIndex]);
        pbyAudioSrc += SAMPLE_PER_FRAME * pCodecOpt->dwChanNum * 2;
        pbyRawAMR += dwOutputSize;
    }
//  fprintf(stderr, "%s:%d:\n", __FILE__, __LINE__);
    if (RTPAMRPack_OneFrame(ptInfo->hPack, &tRTPAMRPackState) != S_OK)
    {
        return S_FAIL;
    }
    //if (AMR_RTP2Storage(((BYTE *)ptInfo->pvOut) + sizeof(TUBuffer), 

    ptInfo->dwSeqNo ++;

    //ptUB = (TUBuffer *)ptInfo->pvOut;

    ptUB = (TUBuffer *)pvOut;

    *pdwSize = ptUB->dwSize = sizeof(TUBuffer) + tRTPAMRPackState.dwPacketSize;
    ptUB->dwDataType = FOURCC_GAMR;
    ptUB->dwSeqNo = ptInfo->dwSeqNo;
    ptUB->bIsSync = TRUE;
    ptUB->dwCompositionOffset = 0;
    ptUB->bIsBoundary = TRUE;
    ptUB->dwUserDataSize = 0;
    return S_OK;
}

static DWORD aenc_GAMR_qr_sp_per_ch_buf(HANDLE hObj)
{
    TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;

    DBPRINT2("dwFramePerBuffer%ld SAMPLE_PER_FRAME%d\n", pCodecOpt->dwFramePerBuffer, SAMPLE_PER_FRAME)
    return pCodecOpt->dwFramePerBuffer * SAMPLE_PER_FRAME;
}

static DWORD aenc_GAMR_qr_out_size(HANDLE hObj)
{
    TCodecOpt *pCodecOpt = (TCodecOpt *) hObj;
    TAencGAMRInfo       *ptInfo = pCodecOpt->hOpt;      

    DWORD   dwData = sizeof(TUBuffer) + 
        (MAX_FRAME_SIZE * pCodecOpt->dwFramePerBuffer + MAX_USER_DATA_SIZE) + ptInfo->dwReservedSize;
    DWORD   dwConf = sizeof(TUBufferConfGAMR);
   
    return (dwData > dwConf) ? dwData : dwConf;

}

SCODE AxeXmlTree_GAMR(void *userData, const char *name, const char **atts)
{
    TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;          
    TCodecOpt           *ptOpt = &(ptaxHandle->ptAencInfo->atcoSaved[0]);
    int                 i;
    
    if (ptaxHandle->bFirstLoad)
    {
        DBPRINT1("%s:\n",__func__)
        for (i = 0; i < MAX_CODEC_NUM; i ++) {
            if (ptOpt->dwFourCC == FOURCC_GAMR) {
                ptaxHandle->hTmp = ptOpt;
                return S_OK;                
            }
            ptOpt ++;
        }

        /* error handling */
        ptaxHandle->hTmp = NULL;
    }
    return IGNORE_CHILD_CONFIG; 
}

void AxeXmlTree_GAMR_FrPerBuf(void *userData, const char *name, int len)
{
    TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;          
    TCodecOpt           *ptOpt = ptaxHandle->hTmp;
    char                szTmp[16];
    DWORD               dwTmp;
    DWORD               dwLen = (len < (sizeof(szTmp) - 1)) 
                                ? len : (sizeof(szTmp) - 1);
        
    DBPRINT1("%s:",__func__)
    if (ptOpt == NULL) {
        return;
    }

    memcpy(szTmp, name, dwLen);
    szTmp[dwLen] = '\0';
    dwTmp = strtol(szTmp, NULL, 0);
    assert(errno != ERANGE);
    ptOpt->dwFramePerBuffer = dwTmp;
    DBPRINT1("%ld\n", ptOpt->dwFramePerBuffer);
}
#endif

