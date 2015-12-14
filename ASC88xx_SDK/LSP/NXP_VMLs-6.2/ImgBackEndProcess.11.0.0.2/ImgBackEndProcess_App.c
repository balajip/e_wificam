/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App/ImgBackEndProcess_App.c 21    13/08/30 9:14p Linus.huang $
 *
 * Copyright 2010-2011 ______ INC.. All rights reserved.
 *
 * Description:
 *
 * $History: ImgBackEndProcess_App.c $
 * 
 * *****************  Version 21  *****************
 * User: Linus.huang  Date: 13/08/30   Time: 9:14p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 19  *****************
 * User: Linus.huang  Date: 13/02/26   Time: 5:41p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 18  *****************
 * User: Linus.huang  Date: 13/01/18   Time: 2:01a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 17  *****************
 * User: Linus.huang  Date: 13/01/03   Time: 1:16p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 16  *****************
 * User: Linus.huang  Date: 12/12/03   Time: 5:23p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 15  *****************
 * User: Linus.huang  Date: 12/12/03   Time: 2:54p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 14  *****************
 * User: Linus.huang  Date: 12/11/08   Time: 4:47p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 1  *****************
 * User: Linus.huang  Date: 12/11/08   Time: 3:08p
 * Created in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 12  *****************
 * User: Linus.huang  Date: 12/11/08   Time: 2:30a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 11  *****************
 * User: Linus.huang  Date: 12/10/09   Time: 12:06p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 10  *****************
 * User: Linus.huang  Date: 12/10/02   Time: 2:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 9  *****************
 * User: Linus.huang  Date: 12/09/17   Time: 8:31p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 8  *****************
 * User: Linus.huang  Date: 12/08/14   Time: 4:03p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 7  *****************
 * User: Linus.huang  Date: 12/08/09   Time: 2:08p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 6  *****************
 * User: Linus.huang  Date: 12/08/09   Time: 10:59a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 5  *****************
 * User: Linus.huang  Date: 12/08/09   Time: 10:37a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 4  *****************
 * User: Linus.huang  Date: 12/08/09   Time: 8:43a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 3  *****************
 * User: Linus.huang  Date: 12/08/06   Time: 9:06p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 2  *****************
 * User: Linus.huang  Date: 12/07/30   Time: 4:36p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 1  *****************
 * User: Linus.huang  Date: 12/07/02   Time: 4:25p
 * Created in $/rd_2/project/Mozart/Linux_Libraries/ImgBackEndProcess_3.0.0.0/ImgBackEndProcess_App
 * 
 * *****************  Version 21  *****************
 * User: Linus.huang  Date: 11/04/12   Time: 1:20p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/ImgBackEndProcess/ImgBackEndProcess_App
 * 
 * *****************  Version 20  *****************
 * User: Linus.huang  Date: 11/03/31   Time: 1:20p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/ImgBackEndProcess/ImgBackEndProcess_App
 * 
 * *****************  Version 19  *****************
 * User: Linus.huang  Date: 11/02/16   Time: 11:58a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/ImgBackEndProcess/ImgBackEndProcess_App
 * 
 * *****************  Version 18  *****************
 * User: Linus.huang  Date: 11/02/09   Time: 5:11p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/ImgBackEndProcess/ImgBackEndProcess_App
 * 
 */


/* ============================================================================================== */
#include <stdio.h>
#include <stdlib.h>

#include "ImgBackEndProcess.h"
#include "MemMgr.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* ============================================================================================== */
SDWORD GetParam(FILE *pfInput)
{
	CHAR acTmpText[200];
	SDWORD sdwRet;

	fscanf(pfInput, "%s", acTmpText);
	sdwRet = atoi(acTmpText);
	fgets(acTmpText, 200, pfInput);

	return sdwRet;
}
/* ============================================================================================== */
int main(int argc, char **argv)

{
	HANDLE hObject;
	HANDLE hMemMgrObject;
	TImgBackEndProcessInitOptions tInitOptions;	
	TImgBackEndProcessState tState;
	TMemMgrInitOptions tMemMgrInitOptions;	
	TMemMgrState tMemMgrState;
	TImgBackEndProcessOptions tOptions;
	TImgBackEndProcessBuffInfo *ptInBuffQueue;

	BYTE *pbyYOutBuff;
	BYTE *pbyCbOutBuff;
	BYTE *pbyCrOutBuff;


#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

	DWORD dwMaxInBuffNum;
	DWORD dwTotalInBuffNum;

	DWORD dwValidRefBuffNum;
	DWORD dwFramePadNum;
	DWORD dwFrameCount;
	DWORD dwFrameNum;
	BOOL bOutBufferValid;
	DWORD dwInYSize, dwInCSize;
	DWORD dwOutYSize, dwOutCSize;
	DWORD dwInCHeight;
	DWORD dwOutCHeight;

	DWORD dwInWidth, dwInHeight;	
	DWORD dwInStride, dwOutStride;
	
	EPixelFormatFlags eInPixelFormat;
	EPixelFormatFlags eOutPixelFormat;
	BOOL bColorFlag;

	FILE *pfCfg;

	BOOL bYComponentEn;
	BOOL bCComponentEn;
	BOOL bYOutEn;
	BOOL bCOutEn;
	BOOL bMotionDetectEn;
	BOOL bMotionDetectPrivacyMaskEn;

	BOOL bNoiseReductionEn;
	EImgBackEndProcessNoiseReductionMode eNoiseReductionMode;
	DWORD dwNoiseReductionNormalStrength;
	DWORD dwNoiseReductionStrongRefStrength;
	DWORD dwNoiseReductionStrongCurrWeight;
	BOOL bNoiseReductionStrongMotionAdaptiveEn;

	BOOL bEdgeEnhanceEn;
	DWORD dwEdgeEnhanceStrength;
	DWORD dwEdgeEnhanceClip;
	DWORD dwEdgeEnhanceEdgeSensitivity;
	DWORD dwEdgeEnhanceNoiseSensitivity;

	BOOL bGeometricLensDistortionCorrectionEn;

	SDWORD sdwGeometricLensDistortionCorrectionYParam1;
	SDWORD sdwGeometricLensDistortionCorrectionYParam2;
	SDWORD sdwGeometricLensDistortionCorrectionYParam3;
	SDWORD sdwGeometricLensDistortionCorrectionCParam1;
	SDWORD sdwGeometricLensDistortionCorrectionCParam2;
	SDWORD sdwGeometricLensDistortionCorrectionCParam3;

	BOOL bDeInterlaceEn;
	EImgBackEndProcessDeInterlaceMode eDeInterlaceMode;
	EImgBackEndProcessInterlacePicFormat eDeInterlacePicFormat;

	BOOL bOutFramePrivacyMaskEn;
	BYTE *pbyPrivacyMaskBuff;
	DWORD dwPrivacyMaskYConstant;
	DWORD dwPrivacyMaskCbConstant;
	DWORD dwPrivacyMaskCrConstant;


	/* Motion detection */
	TMotionDetectWindow tWindowInfo;
	DWORD dwHorzMDGrid, dwVertMDGrid, dwMaxWindowNum;
	DWORD adwWindowSN[MOTION_DTCT_MAX_WIN_NUM];

#ifdef _DRAW_WINDOW_
	DWORD dwWindowHorzStart, dwWindowVertStart, dwWindowWidth, dwWindowHeight;
	BYTE *pbyWindow;
#endif //_DRAW_WINDOW_

	DWORD i, j, k, l, h, f;
	DWORD dwTmp;
	BOOL bIfWindowFillCenter;
	BOOL bIfWindowFillFrame;
	BOOL bIfOneWindowOrig = FALSE;
	BOOL bIfOneWindowShift = FALSE;

	/* Privacy mask */
	DWORD dwMaskBuffWidth;
	DWORD dwMaskBuffStride;
	DWORD dwMaskBuffSize;
	DWORD dwpx, dwpy;


	BYTE *pbyTempInBuff;
	TImgBackEndProcessBuffInfo tTempBuff;

	BOOL bIfFrameEn = TRUE;
	DWORD dwStartFrame = 0;

	//SDWORD n;

	FILE *pfInput;
	FILE *pfOutput;
#ifdef _MD_DEBUG_
	FILE *pfMDOut;
#endif //_MD_DEBUG_

#ifdef _MD_DEBUG_
	if (argc != 13)
#else
	if (argc != 12)
#endif 
	{
		printf("Usage: ImgBackEndProcess input_filename\n");
		printf("                         output_filename\n");
		printf("                         frame_number\n");
		printf("                         input_width\n");
		printf("                         input_height\n");
		printf("                         input_stride\n");
		printf("                         output_stride\n");
		printf("                         color format (1: YCbCr, 0: gray level)\n");
		printf("                         input pixel format (1: YUV420, 2: YUV422)\n");
		printf("                         output pixel format (1: YUV420, 2: YUV422)\n");
		printf("                         IBPE_cfg_file \n");
#ifdef _MD_DEBUG_
		printf("                         MD_txt_filename \n");
#endif 
		printf("\n");
		exit(1);
	}

	if ((pfInput=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open input file %s fail !!\n", argv[1]);
		exit(1);
	}

	if ((pfOutput=fopen(argv[2], "wb")) == NULL)
	{
		printf("Open output file %s fail !!\n", argv[2]);
		exit(1);
	}

	dwFrameNum = atoi(argv[3]);
	dwInWidth = atoi(argv[4]);
	dwInHeight = atoi(argv[5]);	
	dwInStride = atoi(argv[6]);
	dwOutStride = atoi(argv[7]);
	bColorFlag = atoi(argv[8]);

	if (atoi(argv[9]) == 1)
	{
		eInPixelFormat = YUV420;
	}
	else if (atoi(argv[9]) == 2)
	{
		eInPixelFormat = YUV422;
	}
	else //unsupported
	{
		eInPixelFormat = YUV420;
		printf("Unsupported format, please use YUV420/422.\n");
		exit(1);
	}

	if (atoi(argv[10]) == 1)
	{
		eOutPixelFormat = YUV420;
	}
	else if (atoi(argv[10]) == 2)
	{
		eOutPixelFormat = YUV422;
	}
	else //unsupported
	{
		eOutPixelFormat = YUV420;
		printf("Unsupported format, please use YUV420/422.\n");
		exit(1);
	}

	if ((dwInStride%16) != 0)
	{
		dwInStride = ((dwInStride+15)>>4) << 4;
		printf("Modify input stride to multiple of 16: %ld.\n", dwInStride);
	}
	
	if ((dwOutStride%16) != 0)
	{
		dwOutStride = ((dwOutStride+15)>>4) << 4;
		printf("Modify output stride to multiple of 16: %ld.\n", dwOutStride);
	}

	if (argc <= 13) 
    {
        if ((pfCfg=fopen(argv[11], "r")) == NULL)
        {
            printf("Open input configuration file %s fail !!\n", argv[11]);
            exit(1);
        }

		dwValidRefBuffNum = (DWORD)GetParam(pfCfg);	
		bYComponentEn = (BOOL)GetParam(pfCfg);
		bCComponentEn = (BOOL)GetParam(pfCfg);
		bYOutEn = (BOOL)GetParam(pfCfg);
		bCOutEn = (BOOL)GetParam(pfCfg);	

		bMotionDetectEn = (BOOL)GetParam(pfCfg);
		bMotionDetectPrivacyMaskEn = (BOOL)GetParam(pfCfg);
		dwHorzMDGrid = (DWORD)GetParam(pfCfg);
		dwVertMDGrid = (DWORD)GetParam(pfCfg);

		bNoiseReductionEn = (BOOL)GetParam(pfCfg);
		eNoiseReductionMode = (EImgBackEndProcessNoiseReductionMode)GetParam(pfCfg);
		dwNoiseReductionNormalStrength = (DWORD)GetParam(pfCfg);
		dwNoiseReductionStrongRefStrength = (DWORD)GetParam(pfCfg);
		dwNoiseReductionStrongCurrWeight = (DWORD)GetParam(pfCfg);
		bNoiseReductionStrongMotionAdaptiveEn = (BOOL)GetParam(pfCfg);

		bEdgeEnhanceEn = (BOOL)GetParam(pfCfg);
		dwEdgeEnhanceStrength = (DWORD)GetParam(pfCfg);
		dwEdgeEnhanceClip = (DWORD)GetParam(pfCfg);
		dwEdgeEnhanceEdgeSensitivity = (DWORD)GetParam(pfCfg);
		dwEdgeEnhanceNoiseSensitivity = (DWORD)GetParam(pfCfg);

		bGeometricLensDistortionCorrectionEn = (BOOL)GetParam(pfCfg);

		sdwGeometricLensDistortionCorrectionYParam1 = (SDWORD)GetParam(pfCfg);
		sdwGeometricLensDistortionCorrectionYParam2 = (SDWORD)GetParam(pfCfg);
		sdwGeometricLensDistortionCorrectionYParam3 = (SDWORD)GetParam(pfCfg);
		sdwGeometricLensDistortionCorrectionCParam1 = (SDWORD)GetParam(pfCfg);
		sdwGeometricLensDistortionCorrectionCParam2 = (SDWORD)GetParam(pfCfg);
		sdwGeometricLensDistortionCorrectionCParam3 = (SDWORD)GetParam(pfCfg);

		bDeInterlaceEn = (BOOL)GetParam(pfCfg);
		eDeInterlaceMode = (EImgBackEndProcessDeInterlaceMode)GetParam(pfCfg);
		eDeInterlacePicFormat = (EImgBackEndProcessInterlacePicFormat)GetParam(pfCfg);

		bOutFramePrivacyMaskEn = (BOOL)GetParam(pfCfg);
		dwPrivacyMaskYConstant = (DWORD)GetParam(pfCfg);
		dwPrivacyMaskCbConstant = (DWORD)GetParam(pfCfg);
		dwPrivacyMaskCrConstant = (DWORD)GetParam(pfCfg);


		dwTotalInBuffNum = dwValidRefBuffNum + 1;
		dwMaxInBuffNum = dwTotalInBuffNum;
		ptInBuffQueue = (TImgBackEndProcessBuffInfo*)calloc(dwMaxInBuffNum, sizeof(TImgBackEndProcessBuffInfo));
		tState.ptRefInBuffQueue = (TImgBackEndProcessBuffInfo*)calloc(dwMaxInBuffNum, sizeof(TImgBackEndProcessBuffInfo));

#ifdef _MD_DEBUG_
		if (bMotionDetectEn == TRUE)
		{
			if ((pfMDOut=fopen(argv[12], "w")) == NULL)
			{
				printf("Opening output bitstream file MD_info.txt fails !!\n");
				exit(1);
			}
		}
#endif //_MD_DEBUG_

	}
	else
	{
		printf("No config file!\n\n");
		exit(1);
	}

	if ((dwHorzMDGrid==333) && (dwVertMDGrid==333))
	{
		// 16-window-fill-all-frame mode
		bIfWindowFillFrame = TRUE;
		bIfWindowFillCenter = FALSE;
		dwMaxWindowNum = 16;
	}
	else if ((dwHorzMDGrid==444) && (dwVertMDGrid==333))
	{
		bIfWindowFillFrame = FALSE;
		bIfWindowFillCenter = TRUE;
		dwMaxWindowNum = 16;
	}
	else if ((dwHorzMDGrid==555) && (dwVertMDGrid==333))
	{
		bIfWindowFillFrame = FALSE;
		bIfWindowFillCenter = FALSE;
		bIfOneWindowOrig = TRUE;
		bIfOneWindowShift = FALSE;
		dwMaxWindowNum = 16;
	}
	else if ((dwHorzMDGrid==666) && (dwVertMDGrid==333))
	{
		bIfWindowFillFrame = FALSE;
		bIfWindowFillCenter = FALSE;
		bIfOneWindowOrig = FALSE;
		bIfOneWindowShift = TRUE;
		dwMaxWindowNum = 16;
	}
	else
	{
		// normal setting mode
		bIfWindowFillFrame = FALSE;
		bIfWindowFillCenter = FALSE;
		dwMaxWindowNum = dwVertMDGrid * dwHorzMDGrid;

		if ((bMotionDetectEn==TRUE) && (dwMaxWindowNum>16))
		{
			printf("Up to 16 motion detection grids are supported, but %ld (%ldx%ld) grids are assigned !!\n", dwMaxWindowNum, dwHorzMDGrid, dwVertMDGrid);
			exit(1);
		}
	}

	dwInYSize = dwInHeight * dwInStride;
	dwOutYSize = dwInHeight * dwOutStride;

	if (bColorFlag == TRUE)
	{
		if (eInPixelFormat == YUV420)
		{
			dwInCSize = dwInYSize >> 1;
			dwInCHeight = dwInHeight >> 1;
		}
		else //YUV422
		{
			dwInCSize = dwInYSize;
			dwInCHeight = dwInHeight;
		}	
		//out
		if (eOutPixelFormat == YUV420)
		{
			dwOutCSize = dwOutYSize >> 1;
			dwOutCHeight = dwInHeight >> 1;
		}
		else //YUV422
		{
			dwOutCSize = dwOutYSize;
			dwOutCHeight = dwInHeight;
		}	
	} 
	else //MONO
	{
		dwInCSize = 0;
		dwOutCSize = 0;
		dwInCHeight = 0;
		dwOutCHeight = 0;
	}

	// Privacy mask size, copy from DMAC
	dwMaskBuffWidth = (dwInWidth+15) >> 4;
	dwMaskBuffStride = ((dwMaskBuffWidth+7)>>3) << 3;
	dwMaskBuffSize = dwMaskBuffStride*((dwInHeight+1)>>1);

	tInitOptions.dwPrivacyMaskMapStride = dwMaskBuffStride;

/*	Initial Timer object. Timer object is used to do profile and you have to make sure that the timer 
	library is ready before you want to use it to profile your library. */
#ifdef __USE_PROFILE__
	tTimerInitOptions.dwVersion = TIMER_VERSION;
	tTimerInitOptions.eTimerType = TIMER_TYPE_PROFILE;
	tTimerInitOptions.pObjectMem = NULL;

	if (Timer_Initial(&hTimerObject, &tTimerInitOptions) != S_OK)
	{
		printf("Initial timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

/* Initial MemMgr object. MemMgr object is used to allocate memory that hardware module can access
   directly without MMU in SoC platform. All memory section get from this object should be 64-byte
   aligned. */
    tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;    
	tMemMgrInitOptions.pObjectMem = (void *)NULL;

    if (MemMgr_Initial(&hMemMgrObject, &tMemMgrInitOptions) != S_OK)
	{
		printf("Initial memory manager object fail !!\n");
		exit(1);
	}		

	// allocate input buffer
	for (f=0; f<dwMaxInBuffNum; f++)
	{
		tMemMgrState.dwBusNum = IMGBACKENDPROCESS_IN_MEM_RANK_NUM;
		tMemMgrState.dwSize = dwInYSize;
		tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;

		if ((ptInBuffQueue[f].pbyYBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
		{
			printf("Allocate input Y frame buffer %ld fail !!\n", f);
			exit(1);
		}
		else
		{
			printf("Allocate input Y frame buffer %ld succeed !!\n", f);
		}
		memset(ptInBuffQueue[f].pbyYBuff, 0, dwInYSize);

		tMemMgrState.dwBusNum = IMGBACKENDPROCESS_IN_MEM_RANK_NUM;

		tMemMgrState.dwSize = (dwInCSize>>1);
		tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;

		if ((ptInBuffQueue[f].pbyCbBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
		{
			printf("Allocate input Cb frame buffer %ld fail !!\n", f);
			exit(1);
		}
		else
		{
			printf("Allocate input Cb frame buffer %ld succeed !!\n", f);
		}
		memset(ptInBuffQueue[f].pbyCbBuff, 0, (dwInCSize>>1));

		tMemMgrState.dwBusNum = IMGBACKENDPROCESS_IN_MEM_RANK_NUM;
		tMemMgrState.dwSize = (dwInCSize>>1);
		tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;

		if ((ptInBuffQueue[f].pbyCrBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
		{
			printf("Allocate input Cr frame buffer %ld fail !!\n", f);
			exit(1);
		}
		else
		{
			printf("Allocate input Cr frame buffer %ld succeed !!\n", f);
		}
		memset(ptInBuffQueue[f].pbyCrBuff, 0, (dwInCSize>>1));
	}

	// allocate output Y buffer
	tMemMgrState.dwBusNum = IMGBACKENDPROCESS_OUT_MEM_RANK_NUM;
	tMemMgrState.dwSize = dwOutYSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyYOutBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate output Y frame buffer fail !!\n");
		exit(1);
	}
	else
	{
		printf("Allocate output Y frame buffer succeed !!\n");
	}
	memset(pbyYOutBuff, 0, dwOutYSize);

	// allocate output C buffer
	tMemMgrState.dwBusNum = IMGBACKENDPROCESS_OUT_MEM_RANK_NUM;
	tMemMgrState.dwSize = dwOutCSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyCbOutBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate output CbCr frame buffer fail !!\n");
		exit(1);
	}
	else
	{
		printf("Allocate output CbCr frame buffer succeed !!\n");
	}
	memset(pbyCbOutBuff, 0, dwOutCSize);

	// allocate output Cr buffer
	tMemMgrState.dwBusNum = IMGBACKENDPROCESS_OUT_MEM_RANK_NUM;
	tMemMgrState.dwSize = dwOutCSize>>1;
	tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyCrOutBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate output Cr frame buffer fail !!\n");
		exit(1);
	}
	else
	{
		printf("Allocate output Cr frame buffer succeed !!\n");
	}
	memset(pbyCrOutBuff, 0, dwOutCSize>>1);

	// Allocate motion detection info(result) buffer
	if ((tState.tOutBuff.ptMotionDetectInfo=(TMotionDetectWindow*)calloc(sizeof(BYTE), 16*sizeof(TMotionDetectWindow))) == NULL)
	{
		printf("Allocate motion detection info(result) buffer fail !!\n");
		return S_FAIL;
	}
	tState.tOutBuff.bIsMotionDetectResultValid = FALSE;

	// allocate privacy mask buffer
	tMemMgrState.dwBusNum = IMGBACKENDPROCESS_MAP_MEM_RANK_NUM;
	tMemMgrState.dwSize = dwMaskBuffSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyPrivacyMaskBuff=(BYTE*)MemMgr_GetMemory(hMemMgrObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate privacy mask buffer fail !!\n");
		exit(1);
	}
	else
	{
		printf("Allocate privacy mask buffer succeed !!\n");
	}
	memset(pbyPrivacyMaskBuff, 0, dwMaskBuffSize);

	tState.bUpdateEn = TRUE;

	tState.dwWidth = dwInWidth;
	tState.dwHeight = dwInHeight;
	tState.eInPixelFormat = eInPixelFormat;
	tState.eOutPixelFormat = eOutPixelFormat;

	tState.dwValidRefBuffNum = dwValidRefBuffNum;

	//tState.pbyYOutBuff = pbyYOutBuff;
	//tState.pbyCbOutBuff = pbyCbOutBuff;
	//tState.pbyCrOutBuff = pbyCrOutBuff;
	tState.tOutBuff.pbyYBuff = pbyYOutBuff;
	tState.tOutBuff.pbyCbBuff = pbyCbOutBuff;
	tState.tOutBuff.pbyCrBuff = pbyCrOutBuff;
	tState.tOutBuff.tFrameTimeStamp.dwSecond = 0;
	tState.tOutBuff.tFrameTimeStamp.dwMilliSecond = 0;
	tState.tOutBuff.tFrameTimeStamp.dwMicroSecond = 0;
	tState.tOutBuff.tMotionDetectTimeStamp.dwSecond = 0;
	tState.tOutBuff.tMotionDetectTimeStamp.dwMilliSecond = 0;
	tState.tOutBuff.tMotionDetectTimeStamp.dwMicroSecond = 0;
	tState.tOutBuff.bIsMotionDetectResultValid = FALSE;

	tState.bYComponentEn = bYComponentEn;
	tState.bCComponentEn = bCComponentEn;
	tState.bYOutEn = bYOutEn;
	tState.bCOutEn = bCOutEn;

	tState.bMotionDetectEn = bMotionDetectEn;
	tState.bMotionDetectPrivacyMaskEn = bMotionDetectPrivacyMaskEn;

	tState.bNoiseReductionEn = bNoiseReductionEn;
	tState.eNoiseReductionMode = eNoiseReductionMode;
	tState.dwNoiseReductionNormalStrength = dwNoiseReductionNormalStrength;
	tState.dwNoiseReductionStrongRefStrength = dwNoiseReductionStrongRefStrength;
	tState.dwNoiseReductionStrongCurrWeight = dwNoiseReductionStrongCurrWeight;
	tState.bNoiseReductionStrongMotionAdaptiveEn = bNoiseReductionStrongMotionAdaptiveEn;

	tState.bEdgeEnhanceEn = bEdgeEnhanceEn;
	tState.dwEdgeEnhanceStrength = dwEdgeEnhanceStrength;
	tState.dwEdgeEnhanceClip = dwEdgeEnhanceClip;
	tState.dwEdgeEnhanceEdgeSensitivity = dwEdgeEnhanceEdgeSensitivity;
	tState.dwEdgeEnhanceNoiseSensitivity = dwEdgeEnhanceNoiseSensitivity;

	//test
	/*
	tState.dwEdgeEnhanceStrength = -1;
	tState.dwEdgeEnhanceClip = -1;
	tState.dwEdgeEnhanceEdgeSensitivity = -1;
	tState.dwEdgeEnhanceNoiseSensitivity = -1;
	*/

	tState.bGeometricLensDistortionCorrectionEn = bGeometricLensDistortionCorrectionEn;
	
	tState.sdwGeometricLensDistortionCorrectionYParam1 = sdwGeometricLensDistortionCorrectionYParam1;
	tState.sdwGeometricLensDistortionCorrectionYParam2 = sdwGeometricLensDistortionCorrectionYParam2;
	tState.sdwGeometricLensDistortionCorrectionYParam3 = sdwGeometricLensDistortionCorrectionYParam3;
	tState.sdwGeometricLensDistortionCorrectionCParam1 = sdwGeometricLensDistortionCorrectionCParam1;
	tState.sdwGeometricLensDistortionCorrectionCParam2 = sdwGeometricLensDistortionCorrectionCParam2;
	tState.sdwGeometricLensDistortionCorrectionCParam3 = sdwGeometricLensDistortionCorrectionCParam3;

	tState.bDeInterlaceEn = bDeInterlaceEn;
	tState.eDeInterlaceMode = eDeInterlaceMode;
	tState.eDeInterlacePicFormat = eDeInterlacePicFormat;

	tState.bOutFramePrivacyMaskEn = bOutFramePrivacyMaskEn;
	tState.dwPrivacyMaskYConstant  = dwPrivacyMaskYConstant;
	tState.dwPrivacyMaskCbConstant = dwPrivacyMaskCbConstant;
	tState.dwPrivacyMaskCrConstant = dwPrivacyMaskCrConstant;

	tState.pbyPrivacyMaskBuff = pbyPrivacyMaskBuff;

	/* Initial */

	tInitOptions.dwMaxWidth = dwInWidth;
	tInitOptions.dwMaxHeight = dwInHeight;
	tInitOptions.dwInStride = dwInStride;
	tInitOptions.dwOutStride = dwOutStride;

	/* DIE */
	tInitOptions.bInitStaticMapBuff = (bDeInterlaceEn==TRUE) &&
									  ((eDeInterlaceMode==IMGBACKENDPROCESS_DEINTERLACER_MOTION_ADAPTIVE_BASIC)||
									   (eDeInterlaceMode==IMGBACKENDPROCESS_DEINTERLACER_MOTION_ADAPTIVE_ADVANCED));
	/* V3 enhancement */
	tInitOptions.bInitNrStrongBlendBuff = TRUE;
	tInitOptions.bIfNrStrongBlendBuffFormat420 = FALSE;
	
	tInitOptions.dwMaxWindowNum = dwMaxWindowNum;

	tState.dwInBusNum  = IMGBACKENDPROCESS_IN_BUS_NUM;
	tState.dwOutBusNum = IMGBACKENDPROCESS_OUT_BUS_NUM;
	tState.dwMapBusNum = IMGBACKENDPROCESS_MAP_BUS_NUM;
	tState.dwRefBusNum = IMGBACKENDPROCESS_REF_BUS_NUM;

	tInitOptions.dwMapMemRankNum = IMGBACKENDPROCESS_MAP_MEM_RANK_NUM;
	tInitOptions.dwRefMemRankNum = IMGBACKENDPROCESS_REF_MEM_RANK_NUM;

	/* Initializing library object. */
    tInitOptions.dwVersion = IMGBACKENDPROCESS_VERSION;
    tInitOptions.pObjectMem = NULL;

	if (ImgBackEndProcess_Initial(&hObject, &tInitOptions) != S_OK)
    {
        printf("Initialize image back-end processing object fail !!\n");
        exit(1);
    }

	if (ImgBackEndProcess_CheckParam(hObject, &tState) != S_OK)
	{
		printf("State parameters setting error !!\n");
		printf("Error code = 0x%08X\n", ImgBackEndProcess_CheckParam(hObject, &tState));
		goto PROCESS_QUIT;
	}

	tOptions.eOptionFlags = VIDEO_DELETE_ALL_WINDOWS;
	ImgBackEndProcess_SetOptions(hObject, &tOptions);

	if (bMotionDetectEn == TRUE)
	{
		if (bIfWindowFillFrame == TRUE)
		{
			// Setting window 0 ~ 15 to fill the whole frame
			for (i=0; i<4; i++)
			{
				for (j=0; j<4; j++)
				{
					if ((i==3) && (j==3))
					{
						tWindowInfo.dwHorzStart = 0;
						tWindowInfo.dwVertStart = 0;
						tWindowInfo.dwWidth = dwInWidth;
						tWindowInfo.dwHeight = dwInHeight;
						tWindowInfo.byPixDiffThr = 2;
						tWindowInfo.dwMotionPercentThr = 1;
					}
					else
					{
						tWindowInfo.dwHorzStart = i * (dwInWidth>>2);
						tWindowInfo.dwVertStart = j * (dwInHeight>>2);
						tWindowInfo.dwWidth = dwInWidth >> 2;
						tWindowInfo.dwHeight = dwInHeight >> 2;
						tWindowInfo.byPixDiffThr = 2;
						tWindowInfo.dwMotionPercentThr = 1;
					}

					tOptions.eOptionFlags = VIDEO_ADD_WINDOW;
					tOptions.apvUserData[0] = (PVOID)(&tWindowInfo);
					if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
					{
						printf("Set VIDEO_ADD_WINDOW fail !!\n");
						goto PROCESS_QUIT;
					}
					adwWindowSN[i*4+j] = tWindowInfo.dwSerialNum;
					printf("Window serial number: %lu\n", adwWindowSN[i*4+j]);

				}
			}
		}
		else if (bIfWindowFillCenter == TRUE)
		{
			// Setting window 0 ~ 15 to fill the whole frame
			for (i=0; i<4; i++)
			{
				for (j=0; j<4; j++)
				{
					if ((i==3) && (j==3))
					{
						tWindowInfo.dwHorzStart = dwInWidth >> 3;
						tWindowInfo.dwVertStart = dwInHeight >> 3;
						tWindowInfo.dwWidth = dwInWidth * 3 / 4;
						tWindowInfo.dwHeight = dwInHeight * 3 / 4;
						tWindowInfo.byPixDiffThr = 2;
						tWindowInfo.dwMotionPercentThr = 1;
					}
					else
					{
						tWindowInfo.dwHorzStart = (dwInWidth>>3) + (i*((dwInWidth*3/4)>>2));
						tWindowInfo.dwVertStart = (dwInHeight>>3) + (j*((dwInHeight*3/4)>>2));
						tWindowInfo.dwWidth = (dwInWidth*3/4) >> 2;
						tWindowInfo.dwHeight = (dwInHeight*3/4) >> 2;
						tWindowInfo.byPixDiffThr = 2;
						tWindowInfo.dwMotionPercentThr = 1;
					}

					tOptions.eOptionFlags = VIDEO_ADD_WINDOW;
					tOptions.apvUserData[0] = (PVOID)(&tWindowInfo);
					if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
					{
						printf("Set VIDEO_ADD_WINDOW fail !!\n");
						goto PROCESS_QUIT;
					}
					adwWindowSN[i*4+j] = tWindowInfo.dwSerialNum;
					printf("Window serial number: %lu\n", adwWindowSN[i*4+j]);

				}
			}
		}
		else if (bIfOneWindowOrig == TRUE)
		{

			tWindowInfo.dwHorzStart = 0;
			tWindowInfo.dwVertStart = 0;
			tWindowInfo.dwWidth = dwInWidth>>2;
			tWindowInfo.dwHeight = dwInHeight>>2;
			tWindowInfo.byPixDiffThr = 5;
			tWindowInfo.dwMotionPercentThr = 1;

			tOptions.eOptionFlags = VIDEO_ADD_WINDOW;
			tOptions.apvUserData[0] = (PVOID)(&tWindowInfo);
			if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
			{
				printf("Set VIDEO_ADD_WINDOW fail !!\n");
				goto PROCESS_QUIT;
			}
			adwWindowSN[i*4+j] = tWindowInfo.dwSerialNum;
			printf("Window serial number: %lu\n", adwWindowSN[i*4+j]);
		}
		else if (bIfOneWindowShift == TRUE)
		{

			tWindowInfo.dwHorzStart = 1;
			tWindowInfo.dwVertStart = 1;
			tWindowInfo.dwWidth = dwInWidth>>2;
			tWindowInfo.dwHeight = dwInHeight>>2;
			tWindowInfo.byPixDiffThr = 5;
			tWindowInfo.dwMotionPercentThr = 1;

			tOptions.eOptionFlags = VIDEO_ADD_WINDOW;
			tOptions.apvUserData[0] = (PVOID)(&tWindowInfo);
			if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
			{
				printf("Set VIDEO_ADD_WINDOW fail !!\n");
				goto PROCESS_QUIT;
			}
			adwWindowSN[i*4+j] = tWindowInfo.dwSerialNum;
			printf("Window serial number: %lu\n", adwWindowSN[i*4+j]);
		}
		else
		{
			// Setting window information
			for (i=0; i<dwHorzMDGrid; i++)
			{
				for (j=0; j<dwVertMDGrid; j++)
				{
					tWindowInfo.dwHorzStart = (i*3+1)*dwInWidth/(dwHorzMDGrid*3);
					tWindowInfo.dwVertStart = (j*3+1)*dwInHeight/(dwVertMDGrid*3);
					tWindowInfo.dwWidth = dwInWidth/(dwHorzMDGrid*3);
					tWindowInfo.dwHeight = dwInHeight/(dwVertMDGrid*3);
					tWindowInfo.byPixDiffThr = 10;
					tWindowInfo.dwMotionPercentThr = 5;

					tOptions.eOptionFlags = VIDEO_ADD_WINDOW;
					tOptions.apvUserData[0] = (PVOID)(&tWindowInfo);
					if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
					{
						printf("Set VIDEO_ADD_WINDOW fail !!\n");
						goto PROCESS_QUIT;
					}
					adwWindowSN[i*dwVertMDGrid+j] = tWindowInfo.dwSerialNum;
					printf("Window serial number: %lu\n", adwWindowSN[i*dwVertMDGrid+j]);
				}
			}

			/*
			// setting another window to test new privacy mask function
			dwPMaskWinL = 112;
			dwPMaskWinR = 500;
			dwPMaskWinU = 62;
			dwPMaskWinD = 400;

			tOptions.eOptionFlags = VIDEO_CHANGE_WINDOW_POSITION;
			tOptions.apvUserData[0] = 7;
			//tOptions.apvUserData[1] = dwPMaskWinL;
			//tOptions.apvUserData[2] = dwPMaskWinU;
			tOptions.apvUserData[1] = 248;
			tOptions.apvUserData[2] = 52;
			if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
			{
				printf("Modify window position fail !!\n");
				goto PROCESS_QUIT;
			}
			tOptions.eOptionFlags = VIDEO_CHANGE_WINDOW_SIZE;
			tOptions.apvUserData[0] = 7;
			//tOptions.apvUserData[1] = abs(dwPMaskWinR-dwPMaskWinL);
			//tOptions.apvUserData[2] = abs(dwPMaskWinD-dwPMaskWinU);
			tOptions.apvUserData[1] = 30;
			tOptions.apvUserData[2] = 12;
			if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
			{
				printf("Modify window size fail !!\n");
				goto PROCESS_QUIT;
			}
			*/
			
			// switch horz and vert grid
			dwTmp = dwHorzMDGrid;
			dwHorzMDGrid = dwVertMDGrid;
			dwVertMDGrid = dwTmp;
		}
	}
	k = 0;
	l = 0;

	dwFramePadNum = 0;

#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__

	for (dwFrameCount=0; dwFrameCount<(dwFrameNum+dwFramePadNum); dwFrameCount++)
	{	
		//tState.bUpdateEn = TRUE;
		dwStartFrame = 0;
		bIfFrameEn = dwFrameCount >= dwStartFrame;

		if ((bMotionDetectPrivacyMaskEn==TRUE) || (bOutFramePrivacyMaskEn==TRUE))
		{
            // Mask Setting
            for (dwpy = 0; dwpy < (dwInHeight/2); dwpy ++)
            {
                for (dwpx = 0; dwpx < dwMaskBuffStride; dwpx ++)
                {
					// random mask
					pbyPrivacyMaskBuff[(dwpy*dwMaskBuffStride)+dwpx] = (dwFrameCount*dwpy+dwpx) % 256;	
					
					// rectangular mask					
					/*
					if ((dwpx>=(dwMaskBuffStride>>2)) &&
						(dwpx<=(dwMaskBuffStride>>1)) &&
						(dwpy>=(dwInHeight>>3)) &&
						(dwpy<=(dwInHeight>>2)))
					{
						pbyPrivacyMaskBuff[(dwpy*dwMaskBuffStride)+dwpx] = 255;
					} 
					else
					{
						pbyPrivacyMaskBuff[(dwpy*dwMaskBuffStride)+dwpx] = 0;
					}
					*/
                }
            }
		}

		if ((bMotionDetectEn==TRUE) && (bIfWindowFillFrame==FALSE) && (bIfWindowFillCenter==FALSE))
		{
			// motion detection window setting
			//printf("Processing %lu frame ....%lu %lu %lu\n", dwFrameCount, k, l, dwHorzMDGrid);		
			if ((dwFrameCount>=10) && (k<dwHorzMDGrid))
			{
				tOptions.eOptionFlags = VIDEO_DELETE_WINDOW;
				tOptions.apvUserData[0] = (PVOID)adwWindowSN[k*dwVertMDGrid+l];
				printf("Deleting window S/N: %lu\n", adwWindowSN[k*dwVertMDGrid+l]);
				if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
				{
					printf("Set VIDEO_DELETE_WINDOW fail !!\n");
					goto PROCESS_QUIT;
				}
				printf("Delete window S/N: %lu\n", adwWindowSN[k*dwVertMDGrid+l]);

				tOptions.eOptionFlags = VIDEO_ADD_WINDOW;
				tOptions.apvUserData[0] = (PVOID)(&tWindowInfo);
				if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
				{
					printf("Set VIDEO_ADD_WINDOW fail !!\n");
					goto PROCESS_QUIT;
				}
				adwWindowSN[k*dwVertMDGrid+l] = tWindowInfo.dwSerialNum;
				printf("Window serial number: %lu\n", adwWindowSN[k*dwVertMDGrid+l]);

				tOptions.eOptionFlags = VIDEO_CHANGE_MOTION_DETECTION_THRESHOLD;
				tOptions.apvUserData[0] = (PVOID)adwWindowSN[k*dwVertMDGrid+l];
				tOptions.apvUserData[1] = (PVOID)7;	// pixel difference threshold
				tOptions.apvUserData[2] = (PVOID)7;	// motion percentage
				if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
				{
					printf("Set VIDEO_CHANGE_MOTION_DETECTION_THRESHOLD fail !!\n");
					goto PROCESS_QUIT;
				}

				tOptions.eOptionFlags = VIDEO_CHANGE_WINDOW_POSITION;
				tOptions.apvUserData[0] = (PVOID)(adwWindowSN[k*dwVertMDGrid+l]);
				tOptions.apvUserData[1] = (PVOID)((k*3+1)*dwInWidth/(dwHorzMDGrid*3));	// Start X
				tOptions.apvUserData[2] = (PVOID)((l*3+1)*dwInHeight/(dwVertMDGrid*3));	// Start Y
				if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
				{
					printf("Set VIDEO_CHANGE_WINDOW_POSITION fail !!\n");
					goto PROCESS_QUIT;
				}
				else
				{
					printf("Set Start X:%lu, Start Y:%lu\n", 
							(DWORD)tOptions.apvUserData[1], 
							(DWORD)tOptions.apvUserData[2]);
				}

				tOptions.eOptionFlags = VIDEO_CHANGE_WINDOW_SIZE;
				tOptions.apvUserData[0] = (PVOID)(adwWindowSN[k*dwVertMDGrid+l]);
				tOptions.apvUserData[1] = (PVOID)(dwInWidth/(dwHorzMDGrid*3));	// Range X
				tOptions.apvUserData[2] = (PVOID)(dwInHeight/(dwVertMDGrid*3));	// Range Y
				if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
				{
					printf("Set VIDEO_CHANGE_WINDOW_SIZE fail !!\n");
					goto PROCESS_QUIT;
				}
				else
				{
					printf("Set Range X:%lu, Range Y:%lu\n", (DWORD)tOptions.apvUserData[1], (DWORD)tOptions.apvUserData[2]);
				}
				l++;
				if (l == dwVertMDGrid)
				{
					l = 0;
					k++;
				}
			}
			
			// End motion detection setting
		}				

		// read input data 
		if (dwFrameCount == 0)
		{
			for (f=0; f<dwTotalInBuffNum; f++)
			{
				ptInBuffQueue[f].tFrameTimeStamp.dwSecond = 0;
				ptInBuffQueue[f].tFrameTimeStamp.dwMilliSecond = 0;
				ptInBuffQueue[f].tFrameTimeStamp.dwMicroSecond = 0;
				ptInBuffQueue[f].tMotionDetectTimeStamp.dwSecond = 0;
				ptInBuffQueue[f].tMotionDetectTimeStamp.dwMilliSecond = 0;
				ptInBuffQueue[f].tMotionDetectTimeStamp.dwMicroSecond = 0;

				pbyTempInBuff = ptInBuffQueue[f].pbyYBuff;
				for (h=0; h<dwInHeight; h++)
				{
					//if (fread(pbyTempInBuff+(h*dwInStride), sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
					if (fread(pbyTempInBuff+(h*dwInStride), sizeof(BYTE), dwInStride, pfInput) != dwInStride)
					{
						printf("Insufficient input Y data !!\n");
						goto PROCESS_QUIT;
					}
				}
				pbyTempInBuff = ptInBuffQueue[f].pbyCbBuff;
				for (h=0; h<dwInCHeight; h++)
				{
					//if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
					if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInStride>>1), pfInput) != (dwInStride>>1))
					{
						printf("Insufficient input Cb data !!\n");
						goto PROCESS_QUIT;
					}
				}
				pbyTempInBuff = ptInBuffQueue[f].pbyCrBuff;
				for (h=0; h<dwInCHeight; h++)
				{
					//if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
					if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInStride>>1), pfInput) != (dwInStride>>1))
					{
						printf("Insufficient input Cr data !!\n");
						goto PROCESS_QUIT;
					}
				}
			}

		}
#ifndef _LOOP_TEST_
		else
		{
			pbyTempInBuff = ptInBuffQueue[dwTotalInBuffNum-1].pbyYBuff;
			for (h=0; h<dwInHeight; h++)
			{
				//if (fread(pbyTempInBuff+(h*dwInStride), sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
				if (fread(pbyTempInBuff+(h*dwInStride), sizeof(BYTE), dwInStride, pfInput) != dwInStride)
				{
					printf("Insufficient input Y data !!\n");
					goto PROCESS_QUIT;
				}
			}
			pbyTempInBuff = ptInBuffQueue[dwTotalInBuffNum-1].pbyCbBuff;
			for (h=0; h<dwInCHeight; h++)
			{
				//if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
				if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInStride>>1), pfInput) != (dwInStride>>1))
				{
					printf("Insufficient input Cb data !!\n");
					goto PROCESS_QUIT;
				}
			}
			pbyTempInBuff = ptInBuffQueue[dwTotalInBuffNum-1].pbyCrBuff;
			for (h=0; h<dwInCHeight; h++)
			{
				//if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
				if (fread(pbyTempInBuff+(h*(dwInStride>>1)), sizeof(BYTE), (dwInStride>>1), pfInput) != (dwInStride>>1))
				{
					printf("Insufficient input Cr data !!\n");
					goto PROCESS_QUIT;
				}
			}
		}
#endif

		// assign current and reference frame buffer
		// current version
		tState.tCurrInBuff = ptInBuffQueue[0];	
		tState.ptRefInBuffQueue = ptInBuffQueue + 1;


		// Clean reference frame buffer in NR strong/blend modes
		if ((dwFrameCount==0) && (tInitOptions.bInitNrStrongBlendBuff==TRUE))
		{
			tOptions.eOptionFlags = VIDEO_CLEAN_INTERNAL_BUFFER;
			tOptions.apvUserData[0] = (PVOID)(&tState);

			if (ImgBackEndProcess_SetOptions(hObject, &tOptions) != S_OK)
			{
				printf("Failed to clean internal reference frame buffer!\n");
				goto PROCESS_QUIT;
			}
		}

		// latency-improved  version
		/*
		if ((bDeInterlaceEn==TRUE) && 
			((eDeInterlaceMode==IMGBACKENDPROCESS_DEINTERLACER_MOTION_ADAPTIVE_BASIC)||
			 (eDeInterlaceMode==IMGBACKENDPROCESS_DEINTERLACER_MOTION_ADAPTIVE_ADVANCED)))
		{
			tState.tCurrInBuff = ptInBuffQueue[0];
			for (f=0; f<dwValidRefBuffNum; f++)
			{
				tState.ptRefInBuffQueue[f] = ptInBuffQueue[f+1];
			}
		}
		else
		{
			tState.tCurrInBuff = ptInBuffQueue[dwValidRefBuffNum];
			for (f=0; f<dwValidRefBuffNum; f++)
			{
				tState.ptRefInBuffQueue[f] = ptInBuffQueue[f];
			}
		}
		*/

#ifdef __USE_PROFILE__
		Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__	

		if (bIfFrameEn == TRUE)				
		if (ImgBackEndProcess_OneFrame(hObject, &tState) != S_OK)
		{
			printf("Process error in frame %lu !!\n", dwFrameCount);
		}

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

		bOutBufferValid = (dwFrameCount>=(dwTotalInBuffNum-1)) ? TRUE : FALSE;

		if ((bMotionDetectEn==TRUE) && 
			//(tState.tCurrInBuff.bIsMotionDetectResultValid == TRUE) &&
			(tState.tOutBuff.bIsMotionDetectResultValid == TRUE) &&
			(dwMaxWindowNum>0) && (dwFrameCount!=0))
		{
			// draw window
			for (i=0; i<dwMaxWindowNum; i++)
			{
				tWindowInfo = *(tState.tOutBuff.ptMotionDetectInfo + i);
				/*
				tWindowInfo.dwSerialNum = adwWindowSN[i];
				if (ImgBackEndProcess_GetWindowInfo(hObject, &tWindowInfo) != S_OK)
				{
					printf("Fail to find window %lu, S/N %lu\n", i, adwWindowSN[i]);
				}
				else
				{
				*/
#ifdef _MD_DEBUG_
					fprintf(pfMDOut, "dwSerialNum		    : %lu (App)\n", tWindowInfo.dwSerialNum			);
					fprintf(pfMDOut, "dwHorzStart           : %lu (App)\n", tWindowInfo.dwHorzStart			);
					fprintf(pfMDOut, "dwVertStart           : %lu (App)\n", tWindowInfo.dwVertStart			);
					fprintf(pfMDOut, "dwWidth               : %lu (App)\n", tWindowInfo.dwWidth				);
					fprintf(pfMDOut, "dwHeight              : %lu (App)\n", tWindowInfo.dwHeight				);
					fprintf(pfMDOut, "byPixDiffThr          : %lu (App)\n", tWindowInfo.byPixDiffThr			);
					fprintf(pfMDOut, "dwInMaskPixNum	    : %lu (App)\n", tWindowInfo.dwInMaskPixNum		);
					fprintf(pfMDOut, "dwOutMaskPixNum		: %lu (App)\n", tWindowInfo.dwOutMaskPixNum		);
					fprintf(pfMDOut, "dwMotionPercentThr    : %lu (App)\n", tWindowInfo.dwMotionPercentThr	);
					fprintf(pfMDOut, "bIsMotion				: %lu (App)\n", tWindowInfo.bIsMotion			);
					fprintf(pfMDOut, "bIsOutMaskMotion      : %lu (App)\n", tWindowInfo.bIsOutMaskMotion		);
					fprintf(pfMDOut, "\n");
#endif //_MD_DEBUG_

#ifdef _DRAW_WINDOW_
					if (i == 0)
					{
						printf("Draw motion detected windows!\n");
					}
					if ((tWindowInfo.bIsMotion==TRUE) && (bOutBufferValid==TRUE))
					{
						dwWindowHorzStart = tWindowInfo.dwHorzStart;
						dwWindowVertStart = tWindowInfo.dwVertStart;
						dwWindowWidth = tWindowInfo.dwWidth;
						dwWindowHeight = tWindowInfo.dwHeight;
						pbyWindow = pbyYOutBuff+dwWindowHorzStart+dwWindowVertStart*dwOutStride;
						for (j=0; j<dwWindowWidth; j++)
						{
							*(pbyWindow+j) = 0;
							*(pbyWindow+j+(dwWindowHeight-1)*dwOutStride) = 0;
						}
						for (j=0; j<dwWindowHeight; j++)
						{
							*(pbyWindow+j*dwOutStride) = 0;
							*(pbyWindow+j*dwOutStride+dwWindowWidth-1) = 0;
						}
					}
#endif //_DRAW_WINDOW_

			}
		}
		
		printf("%ld Frame process complete !!\n", dwFrameCount);

#ifndef _LOOP_TEST_
		// Write-out Y
		if (bIfFrameEn == TRUE)
		if (bOutBufferValid == TRUE)
		{
			for (h=0; h<dwInHeight; h++)
			{
				fwrite(pbyYOutBuff+(h*dwOutStride), sizeof(BYTE), dwInWidth, pfOutput);
			}
		}
		// Write-out C
		if (bIfFrameEn == TRUE)
		if (bOutBufferValid == TRUE)
		{
			for (h=0; h<dwOutCHeight; h++)
			{
				fwrite(pbyCbOutBuff+(h*(dwOutStride>>1)), sizeof(BYTE), (dwInWidth>>1), pfOutput);
			}
			for (h=0; h<dwOutCHeight; h++)
			{
				fwrite(pbyCrOutBuff+(h*(dwOutStride>>1)), sizeof(BYTE), (dwInWidth>>1), pfOutput);
			}
		}
#endif //_LOOP_TEST_

		// circular buffer
		tTempBuff = ptInBuffQueue[0];
		for (f=0; f<(dwTotalInBuffNum-1); f++)
		{
			ptInBuffQueue[f]= ptInBuffQueue[f+1];
		}
		ptInBuffQueue[dwTotalInBuffNum-1] = tTempBuff;
	}
PROCESS_QUIT:

#ifdef __USE_PROFILE__
	printf("Total cycles in APB unit = %lu\n", dwTotalTicks);	
	printf("TotalTicks@30fps         = %3.2fMHz\n", ((float)dwTotalTicks*2/((float)dwFrameNum))*30/1000000);
	printf("TotalTicks@25fps         = %3.2fMHz\n", ((float)dwTotalTicks*2/((float)dwFrameNum))*25/1000000);
	printf("TotalTicks@15fps         = %3.2fMHz\n", ((float)dwTotalTicks*2/((float)dwFrameNum))*15/1000000);

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

	for (f=0; f<dwMaxInBuffNum; f++)
	{
		if (MemMgr_FreeMemory(hMemMgrObject, (DWORD)ptInBuffQueue[f].pbyYBuff) != S_OK)
		{
			printf("Release input buffer Y fail !!\n");
			exit(1);
		}

		if (MemMgr_FreeMemory(hMemMgrObject, (DWORD)ptInBuffQueue[f].pbyCbBuff) != S_OK)
		{
			printf("Release input buffer Cb fail !!\n");
			exit(1);
		}

		if (MemMgr_FreeMemory(hMemMgrObject, (DWORD)ptInBuffQueue[f].pbyCrBuff) != S_OK)
		{
			printf("Release input buffer Cr fail !!\n");
			exit(1);
		}
	}

	if (MemMgr_FreeMemory(hMemMgrObject, (DWORD)pbyYOutBuff) != S_OK)
	{
		printf("Release output frame buffer fail !!\n");
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemMgrObject, (DWORD)pbyCbOutBuff) != S_OK)
	{
		printf("Release output frame buffer fail !!\n");
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemMgrObject, (DWORD)pbyCrOutBuff) != S_OK)
	{
		printf("Release output Cr frame buffer fail !!\n");
		exit(1);
	}

	if (MemMgr_Release(&hMemMgrObject) != S_OK)
	{
		printf("Release memory manager object fail !!\n");
		exit(1);
	}

	if (ImgBackEndProcess_Release(&hObject) != S_OK)
    {
        printf("Release memory fail !!\n");
        exit(1);
    }

    fclose(pfInput);
    fclose(pfOutput);

	return 0;
}
/* ============================================================================================= */
