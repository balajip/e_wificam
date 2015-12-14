/*

/* ============================================================================================== */
#include "VideoCap_Locals.h"

/* ============================================================================================== */
/* Version 8.0.0.0 modification, 2010.04.28 */
SCODE VideoCap_SetOptions(HANDLE hObject, TVideoSignalOptions *ptOptions)
{
    TVideoCapInfo *ptInfo = (TVideoCapInfo *)hObject;
    TVPLVICChnInfo *ptChnInfo;
    TVPLVICOptions tVICOptions;
	DWORD dwSSH, dwSSV, dwCtrl;
	DWORD dwDeviceNum = ptInfo->dwDeviceNum;
/* Version 7.3.0.4 modification, 2010.03.05 */
    DWORD *pdwCapH, *pdwCapV, *pdwCtrl, *pdwStride, *pdwYAddr, *pdwCbAddr, *pdwCrAddr;
/* ======================================== */
	SWORD swContrast, swSaturation;
/* Version 5.4.0.0 modification, 2008.12.22 */
	DWORD i, m;
	FLOAT *pflColorCoeff;
/* ======================================== */
/* Version 5.7.0.0 modification, 2009.02.23 */
	BYTE *pbyGammaTbl, j;
/* ======================================== */
/* Version 5.8.0.4 modification, 2009.04.22 */
/* Version 5.10.0.1 modification, 2009.05.05 */
	DWORD dwSkipFrame;
/* ========================================= */
/* ======================================== */
/* Version 6.0.0.1 modification, 2009.06.19 */
	DWORD dwQuotient;
/* ======================================== */
/* Version 6.2.0.0 modification, 2009.09.29 */
	BYTE *pbyUserCETbl;
/* Version 7.2.0.2 modification, 2009.11.09 */
	DWORD dwPrevCETblAddr;
	DWORD dwCurrCETblAddr;
/* ======================================== */
/* ======================================== */
	SCODE scResult;
/* Version 6.3.0.0 modification, 2009.10.05 */
	DWORD k;
	DWORD dwMaxPxlValue;
	DWORD dwPhotoLDCTblSize;
	WORD *pwPhotoLDCTbl;
	BYTE *pbyYFrame;
/* ======================================== */
	DWORD dwLDCTblWidth, dwLDCOffset, dwScaleNum, dwMemWidth;
/* Version 12.1.0.0 modification, 2013.03.06 */
	SDWORD sdwPixelCnt, sdwLineCnt, sdwStartX, sdwStartY;
	DWORD dwAverage, dwAverCnt, dwMaxPixel, dwMinPixel, dwLSCRatio;
	DWORD adwHistogram[256];
/* ======================================== */
/* Version 7.1.0.0 modification, 2009.10.16 */
	DWORD dwVideoSize;
/* ======================================== */
/* Version 7.0.0.0 modification, 2009.10.11 */
/* Version 12.0.0.0 modification, 2012.06.28 */
	DWORD adwAEMeasureWinTbl[2];
	DWORD dwAEWinStartX;
	DWORD dwAEWinStartY;
/* ======================================== */
/* Version 10.0.0.0 modification, 2011.07.15 */
	DWORD adwAFMeasureWinTbl[2];	
/* ======================================== */
/* Version 10.0.0.5 modification, 2011.10.25 */
	DWORD dwAFWinWidth;
	DWORD dwAFWinHeight;
	DWORD dwAFWinStartX;
	DWORD dwAFWinStartY;
/* ======================================== */
	DWORD dwAFGridWidth, dwAFGridHeight;
	DWORD dwAFShiftWidth, dwAFShiftHeight;
/* Version 10.0.0.2 modification, 2011.08.26 */
	DWORD dwMemSize;
/* ======================================== */
/* Version 10.0.0.3 modification, 2011.09.21 */
	DWORD dwCFAAntiAliasingTbl[10];
/* ======================================== */
/* Version 11.0.0.0 modification, 2011.11.02 */
	TVideoCapIspParam tVideoCapIspTune;
	TVPLVICISPParam  tVPLVICISPParam;
	BYTE *pbyRGBGammaLut;
	DWORD *pdwUsrCELut;
/* ======================================== */
/* ======================================== */
/* Version 13.0.0.1 modification, 2013.08.19 */
	DWORD *pdwColorCoeff;
/* ======================================== */

	ptChnInfo = (TVPLVICChnInfo*)&ptInfo->ptVICInfo->atChnInfo[dwDeviceNum];
	pdwCtrl = (DWORD *)&(ptInfo->ptVICInfo->adwChanCtrl[dwDeviceNum]);
	pdwCapH = &(ptChnInfo->dwCapH);
	pdwCapV = &(ptChnInfo->dwCapV);
	pdwStride = &(ptChnInfo->dwStride);
/* Version 7.3.0.4 modification, 2010.03.05 */
/* ======================================== */
	pdwYAddr = &(ptChnInfo->dwYAddr0);
	pdwCbAddr = &(ptChnInfo->dwCbAddr0);
	pdwCrAddr = &(ptChnInfo->dwCrAddr0);

	dwCtrl = *pdwCtrl;
	dwSSV = (dwCtrl>>17) & 0x3;

/* Version 2.0.0.2 modification, 2006.08.24 */
	tVICOptions.eOptionFlags = ptOptions->eOptionFlags;
/* Version 5.2.0.4 modification, 2008.11.27 */
/* ======================================== */
/* Version 3.1.0.0 modification, 2008.02.02 */
	if (ptInfo->bIsMaster == FALSE)
/* ======================================== */
	{
		switch (ptOptions->eOptionFlags)
		{
			case VIDEO_SIGNAL_OPTION_SET_FRAME_RATE:
				ptInfo->dwFrameRate = ptOptions->adwUserData[0];
				if (ptInfo->dwFrameRate != 0)
				{
					ptInfo->dwFrameCount = 0;
					ptInfo->dwDiffResidue = 0;

/* Version 6.0.0.1 modification, 2009.06.19 */
					ptInfo->dwInterval = INTERVEL / ptInfo->dwFrameRate;
					dwQuotient = (INTERVEL*PRECISION) / ptInfo->dwFrameRate;
					ptInfo->dwIntvResAccu = (INTERVEL*PRECISION) - (dwQuotient*ptInfo->dwFrameRate);
					ptInfo->dwIntervalResidue = dwQuotient - (ptInfo->dwInterval*PRECISION);
/* ======================================== */
				}
				break;
/* Version 5.2.0.5 modification, 2008.11.27 */
			case VIDEO_SIGNAL_OPTION_RESET_FRAME_RATE_CTRL:
				ptInfo->dwFrameCount = 0;
				ptInfo->dwDiffResidue = 0;
				break;
/* ======================================== */
/* Version 11.0.0.0 modification, 2011.11.02 */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE:
				memcpy(&tVideoCapIspTune, (TVideoCapIspParam *)ptOptions->adwUserData[0], sizeof(TVideoCapIspParam));

				tVPLVICISPParam.bAECtrl = FALSE;
				tVPLVICISPParam.bCCCtrl = FALSE;
				tVPLVICISPParam.bCECtrl = FALSE; 
				tVPLVICISPParam.bAntialiasingCtrl = FALSE; 
				tVPLVICISPParam.bCMOSCtrl = FALSE;
				tVPLVICISPParam.bSBCCtrl = FALSE;
				tVPLVICISPParam.bTMCtrl = FALSE; 
/* Version 12.0.0.1 modification, 2012.11.02 */
				tVPLVICISPParam.bDeimpulseCtrl = FALSE;
				tVPLVICISPParam.bBlackClampCtrl = FALSE;
				tVPLVICISPParam.bCFAModeCtrl = FALSE;
/* ======================================== */
/* Version 13.0.0.0 modification, 2013.04.19 */
				tVPLVICISPParam.bAWBCtrl = FALSE;
/* ======================================== */

				if (tVideoCapIspTune.pbyGammaLut != NULL)
				{
					tVPLVICISPParam.bCMOSCtrl = TRUE;
					pbyRGBGammaLut = (BYTE *)ptOptions->adwUserData[1];
					memcpy(pbyRGBGammaLut, tVideoCapIspTune.pbyGammaLut, 4096*3*sizeof(BYTE));
					ptInfo->ptVICInfo->dwGammaTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)pbyRGBGammaLut);
					MemMgr_CacheCopyBack(ptInfo->hEDMCDev, (DWORD)pbyRGBGammaLut, sizeof(BYTE)*4096*3);
				}
				else if (tVideoCapIspTune.pdwTMParam != NULL)
				{
					tVPLVICISPParam.bTMCtrl = TRUE;
					tVPLVICISPParam.dwTMCtrl = (tVideoCapIspTune.pdwTMParam[0]&0x00000001) | ((tVideoCapIspTune.pdwTMParam[1]&0x7)<<1);
				}
				
				if (tVideoCapIspTune.pdwCEParam != NULL)
				{
					tVPLVICISPParam.bCECtrl = TRUE;
/* Version 13.0.0.0 modification, 2013.04.19 */
					tVPLVICISPParam.dwCECtrl = (tVideoCapIspTune.pdwCEParam[0]) | (tVideoCapIspTune.pdwCEParam[1]<<1);

					if (tVideoCapIspTune.pdwCEParam[4])
					{
						pdwUsrCELut = (DWORD *)tVideoCapIspTune.pdwCEParam[3];
						memcpy(pdwUsrCELut, (DWORD *)(tVideoCapIspTune.pdwCEParam[2]), 256*sizeof(DWORD));
						tVPLVICISPParam.dwCETblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)pdwUsrCELut);
						MemMgr_CacheCopyBack(ptInfo->hEDMCDev, (DWORD)pdwUsrCELut, 256*sizeof(DWORD));
						tVPLVICISPParam.dwCECtrl = (tVPLVICISPParam.dwCECtrl&0xFFFFFF9F) | (0x20);
					}
/* ======================================== */
				}

				if (tVideoCapIspTune.psdwColorParam != NULL)
				{
					tVPLVICISPParam.bCCCtrl = TRUE;
					tVPLVICISPParam.dwCCCoeffR = ((((DWORD)tVideoCapIspTune.psdwColorParam[9])&0x0000000F)<<28) |
												 (((tVideoCapIspTune.psdwColorParam[0]>=0)?0:1)<<27) |
												 ((((DWORD)(((tVideoCapIspTune.psdwColorParam[0]>=0)?tVideoCapIspTune.psdwColorParam[0]:tVideoCapIspTune.psdwColorParam[0]*(-1))))&0x000001FF)<<18) |
												 (((tVideoCapIspTune.psdwColorParam[1]>=0)?0:1)<<17) |
												 ((((DWORD)(((tVideoCapIspTune.psdwColorParam[1]>=0)?tVideoCapIspTune.psdwColorParam[1]:tVideoCapIspTune.psdwColorParam[1]*(-1))))&0x000000FF)<<9) |
												 (((tVideoCapIspTune.psdwColorParam[2]>=0)?0:1)<<8) |
												 (((DWORD)(((tVideoCapIspTune.psdwColorParam[2]>=0)?tVideoCapIspTune.psdwColorParam[2]:tVideoCapIspTune.psdwColorParam[2]*(-1))))&0x000000FF);
					tVPLVICISPParam.dwCCCoeffG = ((((DWORD)tVideoCapIspTune.psdwColorParam[10])&0x0000000F)<<28) |
												 (((tVideoCapIspTune.psdwColorParam[3]>=0)?0:1)<<27) |
												 ((((DWORD)(((tVideoCapIspTune.psdwColorParam[3]>=0)?tVideoCapIspTune.psdwColorParam[3]:tVideoCapIspTune.psdwColorParam[3]*(-1))))&0x000000FF)<<19) |
												 (((tVideoCapIspTune.psdwColorParam[4]>=0)?0:1)<<18) |
												 ((((DWORD)(((tVideoCapIspTune.psdwColorParam[4]>=0)?tVideoCapIspTune.psdwColorParam[4]:tVideoCapIspTune.psdwColorParam[4]*(-1))))&0x000001FF)<<9) |
												 (((tVideoCapIspTune.psdwColorParam[5]>=0)?0:1)<<8) |
												 (((DWORD)(((tVideoCapIspTune.psdwColorParam[5]>=0)?tVideoCapIspTune.psdwColorParam[5]:tVideoCapIspTune.psdwColorParam[5]*(-1))))&0x000000FF);
					tVPLVICISPParam.dwCCCoeffB = ((((DWORD)tVideoCapIspTune.psdwColorParam[11])&0x0000000F)<<28) |
												 (((tVideoCapIspTune.psdwColorParam[6]>=0)?0:1)<<27) |
												 ((((DWORD)(((tVideoCapIspTune.psdwColorParam[6]>=0)?tVideoCapIspTune.psdwColorParam[6]:tVideoCapIspTune.psdwColorParam[6]*(-1))))&0x000000FF)<<19) |
												 (((tVideoCapIspTune.psdwColorParam[7]>=0)?0:1)<<18) |
												 ((((DWORD)(((tVideoCapIspTune.psdwColorParam[7]>=0)?tVideoCapIspTune.psdwColorParam[7]:tVideoCapIspTune.psdwColorParam[7]*(-1))))&0x000000FF)<<10) |
												 (((tVideoCapIspTune.psdwColorParam[8]>=0)?0:1)<<9) |
												 (((DWORD)(((tVideoCapIspTune.psdwColorParam[8]>=0)?tVideoCapIspTune.psdwColorParam[8]:tVideoCapIspTune.psdwColorParam[8]*(-1))))&0x000001FF);

				}
				if (tVideoCapIspTune.pdwSBCParam != NULL)
				{
					tVPLVICISPParam.bSBCCtrl = TRUE;
					ptInfo->sdwBrightness = ((SDWORD)(tVideoCapIspTune.pdwSBCParam[0]<<24)) >> 24;
					ptInfo->dwSaturation = tVideoCapIspTune.pdwSBCParam[1];
					ptInfo->sdwContrast = ((SDWORD)(tVideoCapIspTune.pdwSBCParam[2]<<24)) >> 24;

					tVPLVICISPParam.dwSBC = ((ptInfo->sdwBrightness&0xff)<<8) | ((ptInfo->dwSaturation&0xff)<<16) | (ptInfo->sdwContrast&0xff);
				}
				if (tVideoCapIspTune.pdwAntialiasingParam != NULL)
				{
					tVPLVICISPParam.bAntialiasingCtrl = TRUE;
					memcpy(dwCFAAntiAliasingTbl, tVideoCapIspTune.pdwAntialiasingParam, sizeof(DWORD)*10);
					tVPLVICISPParam.dwBayerCFAHVAvgThrd =  ((dwCFAAntiAliasingTbl[0]&0x00000001)<<31)|(dwCFAAntiAliasingTbl[1]&0x0003FFFF);
					tVPLVICISPParam.dwBayerCFAHVBlendMaxThrd = (dwCFAAntiAliasingTbl[3]&0x0003FFFF);
					tVPLVICISPParam.dwBayerCFAHVBlendMinThrd = (dwCFAAntiAliasingTbl[2]&0x0003FFFF);
					tVPLVICISPParam.dwBayerCFAFCSEdgeThrd = (dwCFAAntiAliasingTbl[5]&0x0003FFFF);
					tVPLVICISPParam.dwBayerCFAFCSMaxThrd = (dwCFAAntiAliasingTbl[7]&0x0003FFFF);
					tVPLVICISPParam.dwBayerCFAFCSMinThrd = (dwCFAAntiAliasingTbl[6]&0x0003FFFF);
					tVPLVICISPParam.dwBayerCFAFCSCtrl = ((dwCFAAntiAliasingTbl[4]&0x00000001)<<10)|((dwCFAAntiAliasingTbl[8]&0x00000001)<<9)|(dwCFAAntiAliasingTbl[9]&0x000001FF);
				}
				if (tVideoCapIspTune.pdwAEParam != NULL)
				{
					tVPLVICISPParam.bAECtrl = TRUE;
					tVPLVICISPParam.dwTargetLuma = tVideoCapIspTune.pdwAEParam[0];
					tVPLVICISPParam.dwTargetOffset = tVideoCapIspTune.pdwAEParam[1];
					tVPLVICISPParam.dwIrisActiveTime = tVideoCapIspTune.pdwAEParam[2];
					tVPLVICISPParam.dwMinShutter = tVideoCapIspTune.pdwAEParam[3];
					tVPLVICISPParam.dwMaxShutter = tVideoCapIspTune.pdwAEParam[4];
					tVPLVICISPParam.dwMinGain = tVideoCapIspTune.pdwAEParam[5];
					tVPLVICISPParam.dwMaxGain = tVideoCapIspTune.pdwAEParam[6];
/* Version 13.0.0.0 modification, 2013.04.19 */
					tVPLVICISPParam.dwAEMode = tVideoCapIspTune.pdwAEParam[7];
					tVPLVICISPParam.bAELock = (BOOL)tVideoCapIspTune.pdwAEParam[8];
					tVPLVICISPParam.dwAESpeed = tVideoCapIspTune.pdwAEParam[9];
					tVPLVICISPParam.dwIrisStatus = tVideoCapIspTune.pdwAEParam[10];
/* ======================================== */
				}			
/* Version 12.0.0.1 modification, 2012.11.02 */
				if (tVideoCapIspTune.pdwDeimpulseParam != NULL)
				{
					tVPLVICISPParam.bDeimpulseCtrl = TRUE;
					tVPLVICISPParam.dwDeimpulseCtrl = tVideoCapIspTune.pdwDeimpulseParam[0];
					tVPLVICISPParam.dwDenoiseThrd = ((tVideoCapIspTune.pdwDeimpulseParam[1]<<16) | tVideoCapIspTune.pdwDeimpulseParam[2]);
					tVPLVICISPParam.dwDenoiseMaxThrd = ((tVideoCapIspTune.pdwDeimpulseParam[3]<<16) | tVideoCapIspTune.pdwDeimpulseParam[4]);
					tVPLVICISPParam.dwDenoiseMinThrd = ((tVideoCapIspTune.pdwDeimpulseParam[5]<<16) | tVideoCapIspTune.pdwDeimpulseParam[6]);
				}
				if (tVideoCapIspTune.psdwBlackClampParam != NULL)
				{
					tVPLVICISPParam.bBlackClampCtrl = TRUE;
/* Version 12.0.0.2 modification, 2012.11.22 */
					if ((!ptInfo->byCFAPatternVShift) && (!ptInfo->byCFAPatternHShift))
					{
						tVPLVICISPParam.dwBlackClamp = ((((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?0:1)<<31) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?tVideoCapIspTune.psdwBlackClampParam[0]:tVideoCapIspTune.psdwBlackClampParam[0]*(-1))&0x7F)<<24) |
														(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?0:1)<<23) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?tVideoCapIspTune.psdwBlackClampParam[1]:tVideoCapIspTune.psdwBlackClampParam[1]*(-1))&0x7F)<<16) |
														(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?0:1)<<15) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?tVideoCapIspTune.psdwBlackClampParam[3]:tVideoCapIspTune.psdwBlackClampParam[3]*(-1))&0x7F)<<8) |
														(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?0:1)<<7) |
														(DWORD)(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?tVideoCapIspTune.psdwBlackClampParam[2]:tVideoCapIspTune.psdwBlackClampParam[2]*(-1))&0x7F));
					}
					else if ((ptInfo->byCFAPatternVShift)&&(!ptInfo->byCFAPatternHShift))
					{
						tVPLVICISPParam.dwBlackClamp = ((((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?0:1)<<31) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?tVideoCapIspTune.psdwBlackClampParam[3]:tVideoCapIspTune.psdwBlackClampParam[3]*(-1))&0x7F)<<24) |
														(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?0:1)<<23) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?tVideoCapIspTune.psdwBlackClampParam[2]:tVideoCapIspTune.psdwBlackClampParam[2]*(-1))&0x7F)<<16) |
														(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?0:1)<<15) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?tVideoCapIspTune.psdwBlackClampParam[0]:tVideoCapIspTune.psdwBlackClampParam[0]*(-1))&0x7F)<<8) |
														(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?0:1)<<7) |
														(DWORD)(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?tVideoCapIspTune.psdwBlackClampParam[1]:tVideoCapIspTune.psdwBlackClampParam[1]*(-1))&0x7F));
					}
					else if ((!ptInfo->byCFAPatternVShift)&&(ptInfo->byCFAPatternHShift))
					{
						tVPLVICISPParam.dwBlackClamp = ((((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?0:1)<<31) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?tVideoCapIspTune.psdwBlackClampParam[1]:tVideoCapIspTune.psdwBlackClampParam[1]*(-1))&0x7F)<<24) |
														(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?0:1)<<23) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?tVideoCapIspTune.psdwBlackClampParam[0]:tVideoCapIspTune.psdwBlackClampParam[0]*(-1))&0x7F)<<16) |
														(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?0:1)<<15) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?tVideoCapIspTune.psdwBlackClampParam[2]:tVideoCapIspTune.psdwBlackClampParam[2]*(-1))&0x7F)<<8) |
														(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?0:1)<<7) |
														(DWORD)(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?tVideoCapIspTune.psdwBlackClampParam[3]:tVideoCapIspTune.psdwBlackClampParam[3]*(-1))&0x7F));
					}
					else
					{
						tVPLVICISPParam.dwBlackClamp = ((((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?0:1)<<31) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[2]>=0)?tVideoCapIspTune.psdwBlackClampParam[2]:tVideoCapIspTune.psdwBlackClampParam[2]*(-1))&0x7F)<<24) |
														(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?0:1)<<23) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[3]>=0)?tVideoCapIspTune.psdwBlackClampParam[3]:tVideoCapIspTune.psdwBlackClampParam[3]*(-1))&0x7F)<<16) |
														(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?0:1)<<15) |
														((DWORD)(((tVideoCapIspTune.psdwBlackClampParam[1]>=0)?tVideoCapIspTune.psdwBlackClampParam[1]:tVideoCapIspTune.psdwBlackClampParam[1]*(-1))&0x7F)<<8) |
														(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?0:1)<<7) |
														(DWORD)(((tVideoCapIspTune.psdwBlackClampParam[0]>=0)?tVideoCapIspTune.psdwBlackClampParam[0]:tVideoCapIspTune.psdwBlackClampParam[0]*(-1))&0x7F));
					}
/* ======================================== */
				}
				if (tVideoCapIspTune.pdwCFAMode != NULL)
				{
					tVPLVICISPParam.bCFAModeCtrl = TRUE;
					tVPLVICISPParam.dwCFAMode = tVideoCapIspTune.pdwCFAMode[0];	
				}
/* ======================================== */
/* Version 13.0.0.0 modification, 2013.04.19 */
				if (tVideoCapIspTune.pdwAWBParam != NULL)
				{
					tVPLVICISPParam.bAWBCtrl = TRUE;
					tVPLVICISPParam.bAWBLock = (BOOL)tVideoCapIspTune.pdwAWBParam[0];
					tVPLVICISPParam.dwAWBMode = tVideoCapIspTune.pdwAWBParam[1];
					tVPLVICISPParam.dwCustomGainR = tVideoCapIspTune.pdwAWBParam[2];
					tVPLVICISPParam.dwCustomGainB = tVideoCapIspTune.pdwAWBParam[3];
				}

				if (tVideoCapIspTune.pdwLSCParam != NULL)
				{
					if (tVideoCapIspTune.pdwLSCParam[1])
					{
						ptInfo->bPhotoLDCEn = FALSE;
						ptInfo->ptVICInfo->dwPhotoLDCCtrl = (ptInfo->ptVICInfo->dwPhotoLDCCtrl&0xFFFFFFFE) | ptInfo->bPhotoLDCEn;

						ptInfo->pwPhotoLDCTbl = (WORD *)tVideoCapIspTune.pdwLSCParam[2];					

						ptInfo->ptVICInfo->dwPhotoLDCTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pwPhotoLDCTbl);
					}

					ptInfo->bPhotoLDCEn = (BOOL)tVideoCapIspTune.pdwLSCParam[0];
					ptInfo->ptVICInfo->dwPhotoLDCCtrl = (ptInfo->ptVICInfo->dwPhotoLDCCtrl&0xFFFFFFFE) | ptInfo->bPhotoLDCEn;
				}
/* ======================================== */
				tVICOptions.adwUserData[0] = (DWORD)(&tVPLVICISPParam);
				while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_SETOPTIONS, &tVICOptions) != S_OK);
				break;
/* ======================================== */
/* Version 12.0.0.1 modification, 2012.11.02 */
			case VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE:
				while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_SETOPTIONS, &tVICOptions) != S_OK);
				ptOptions->adwUserData[0] = tVICOptions.adwUserData[0];
				return S_OK;
/* ======================================== */
			default:
/* Version 12.0.0.3 modification, 2013.01.16 */
				ERRPRINT("Unsupported option flag %d !!\n", ptOptions->eOptionFlags);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_NOT_SUPPORT_OPTION_FLAGS;
/* ======================================== */
		}
	}
	else
	{
/* ======================================== */
		switch (ptOptions->eOptionFlags)
		{
			case VIDEO_SIGNAL_OPTION_RESET:
				break;
			case VIDEO_SIGNAL_OPTION_SET_SIZE:
/* Version 5.7.0.2 modification, 2009.03.16 */
				ptInfo->dwCapWidth = (ptOptions->adwUserData[0] & 0x0000FFFF);
/* Version 8.0.0.10 modification, 2010.12.07 */
				ptInfo->dwCapHeight = (ptOptions->adwUserData[0] >> 16);
/* ======================================== */
				ptInfo->dwVideoWidth = ptInfo->dwCapWidth + ptInfo->dwStartPixel;
				ptInfo->dwVideoHeight = ptInfo->dwCapHeight + ptInfo->dwStartLine;
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = ptOptions->adwUserData[1];
/* Version 3.1.0.4 modification, 2008.03.04 */
				break;
/* ======================================== */
/* Version 5.4.0.0 modification, 2008.12.19 */
/* Version 7.3.0.4 modification, 2010.03.05 */
			case VIDEO_SIGNAL_OPTION_SET_BRIGHTNESS:
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				ptInfo->sdwBrightness = ((SDWORD)(ptOptions->adwUserData[0]<<24)) >> 24;
				break;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_FLIP:
			case VIDEO_SIGNAL_OPTION_SET_MIRROR:
/* Version 7.3.0.4 modification, 2010.03.05 */
/* ======================================== */
/* Version 5.8.0.1 modification, 2009.04.17 */
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_LOW_PASS_FILTER:
/* Version 8.0.0.11 modification, 2010.12.15 */
			case VIDEO_SIGNAL_OPTION_SET_WDR:
/* ======================================== */				
/* Version 8.0.0.13 modification, 2011.01.12 */	
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MODE:
			case VIDEO_SIGNAL_OPTION_SET_HUE:
			case VIDEO_SIGNAL_OPTION_SET_SHARPNESS:
/* ======================================== */
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				break;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
/* Version 13.0.0.3 modification, 2013.12.24 */
			case VIDEO_SIGNAL_OPTION_SENSOR_DIRECT_ACCESS:
/* ======================================== */
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = ptOptions->adwUserData[1];
				tVICOptions.adwUserData[2] = ptOptions->adwUserData[2];				
				break;
/* Version 13.0.0.1 modification, 2013.08.19 */
			case VIDEO_SIGNAL_OPTION_SET_COLOR_TRANSFORM:
				pdwColorCoeff = (DWORD *) ptOptions->adwUserData[0];
				tVICOptions.adwUserData[0] = (((pdwColorCoeff[3]&0xFF)<<24) |
											((pdwColorCoeff[2]&0xFF)<<16) |
											((pdwColorCoeff[1]&0xFF)<<8) |
											(pdwColorCoeff[0]&0xFF));
				tVICOptions.adwUserData[1] = (((pdwColorCoeff[7]&0xFF)<<24) |
											((pdwColorCoeff[6]&0xFF)<<16) |
											((pdwColorCoeff[5]&0xFF)<<8) |
											(pdwColorCoeff[4]&0xFF));
				tVICOptions.adwUserData[2] = (((ptInfo->byYKiller&0xFF)<<13) |
											((pdwColorCoeff[9]&0x1F)<<8) |
											(pdwColorCoeff[8]&0xFF));
				break;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_COLOR_CORRECTION:
/* Version 7.0.0.0 modification, 2009.10.13 */
				pflColorCoeff = (FLOAT *) ptOptions->adwUserData[0];
/* Version 5.6.0.1 modification, 2009.02.11 */
				tVICOptions.adwUserData[0] = ((((DWORD)pflColorCoeff[9])&0x0000000F)<<28) |
											 (((pflColorCoeff[0]>=0)?0:1)<<27) |
											 ((((DWORD)(((pflColorCoeff[0]>=0)?pflColorCoeff[0]:pflColorCoeff[0]*(-1))*128))&0x000001FF)<<18) |
											 (((pflColorCoeff[1]>=0)?0:1)<<17) |
											 ((((DWORD)(((pflColorCoeff[1]>=0)?pflColorCoeff[1]:pflColorCoeff[1]*(-1))*128))&0x000000FF)<<9) |
											 (((pflColorCoeff[2]>=0)?0:1)<<8) |
											 (((DWORD)(((pflColorCoeff[2]>=0)?pflColorCoeff[2]:pflColorCoeff[2]*(-1))*128))&0x000000FF);
				tVICOptions.adwUserData[1] = ((((DWORD)pflColorCoeff[10])&0x0000000F)<<28) |
											 (((pflColorCoeff[3]>=0)?0:1)<<27) |
											 ((((DWORD)(((pflColorCoeff[3]>=0)?pflColorCoeff[3]:pflColorCoeff[3]*(-1))*128))&0x000000FF)<<19) |
											 (((pflColorCoeff[4]>=0)?0:1)<<18) |
											 ((((DWORD)(((pflColorCoeff[4]>=0)?pflColorCoeff[4]:pflColorCoeff[4]*(-1))*128))&0x000001FF)<<9) |
											 (((pflColorCoeff[5]>=0)?0:1)<<8) |
											 (((DWORD)(((pflColorCoeff[5]>=0)?pflColorCoeff[5]:pflColorCoeff[5]*(-1))*128))&0x000000FF);
				tVICOptions.adwUserData[2] = ((((DWORD)pflColorCoeff[11])&0x0000000F)<<28) |
											 (((pflColorCoeff[6]>=0)?0:1)<<27) |
											 ((((DWORD)(((pflColorCoeff[6]>=0)?pflColorCoeff[6]:pflColorCoeff[6]*(-1))*128))&0x000000FF)<<19) |
											 (((pflColorCoeff[7]>=0)?0:1)<<18) |
											 ((((DWORD)(((pflColorCoeff[7]>=0)?pflColorCoeff[7]:pflColorCoeff[7]*(-1))*128))&0x000000FF)<<10) |
											 (((pflColorCoeff[8]>=0)?0:1)<<9) |
											 (((DWORD)(((pflColorCoeff[8]>=0)?pflColorCoeff[8]:pflColorCoeff[8]*(-1))*128))&0x000001FF);
				DBPRINT3("CoeffR:%08X, CoeffG:%08X, CoeffG:%08X\n", tVICOptions.adwUserData[0], tVICOptions.adwUserData[1], tVICOptions.adwUserData[2]);
				printf("CoeffR:%08X, CoeffG:%08X, CoeffG:%08X\n", tVICOptions.adwUserData[0], tVICOptions.adwUserData[1], tVICOptions.adwUserData[2]);
/* ======================================== */
/* ======================================== */
				break;
/* ======================================== */
/* ======================================== */
/* Version 5.7.0.0 modification, 2009.02.23 */
			case VIDEO_SIGNAL_OPTION_SET_GAMMA_TABLE:
				pbyGammaTbl = (BYTE *) ptOptions->adwUserData[0];
				memcpy(ptInfo->pbyGammaTbl, pbyGammaTbl, 4096*3*sizeof(BYTE));
				ptInfo->ptVICInfo->dwGammaTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyGammaTbl);
				MemMgr_CacheCopyBack(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyGammaTbl, 4096*3*sizeof(BYTE));
				break;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_CAPTURE_AREA:
				ptInfo->dwCapWidth = ptOptions->adwUserData[0];
				ptInfo->dwCapHeight = ptOptions->adwUserData[1];
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = ptOptions->adwUserData[1];
				break;
			case VIDEO_SIGNAL_OPTION_SET_START_PIXEL:
/* Version 5.2.0.4 modification, 2008.11.27 */
/* Version 5.4.0.0 modification, 2008.12.19 */
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = ptOptions->adwUserData[1];
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.4 modification, 2008.03.04 */
				break;
/* ======================================== */
/* Version 5.5.0.0 modification, 2008.12.29 */
			case VIDEO_SIGNAL_OPTION_SET_MONO:
				ptInfo->bMono = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];	//set mono / reset mono
				tVICOptions.adwUserData[1] = ptInfo->dwSaturation;
				tVICOptions.adwUserData[2] = ptInfo->sdwContrast;				
				break;
/* Version 5.8.0.1 modification, 2009.04.17 */
			case VIDEO_SIGNAL_OPTION_SET_CONTRAST:
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];	// contrast
/* Version 8.0.0.7 modification, 2010.09.09 */
				if (ptInfo->bMono == TRUE)
				{
					tVICOptions.adwUserData[1] = 0;
				}
				else
				{
					tVICOptions.adwUserData[1] = ptInfo->dwSaturation;	//saturation
				}
/* ======================================== */
				ptInfo->sdwContrast = ((SDWORD)(ptOptions->adwUserData[0]<<24)) >> 24;
				break;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_SATURATION:
				ptInfo->dwSaturation = ptOptions->adwUserData[0];
				if (ptInfo->bMono == TRUE)
				{
					return S_OK;
				}
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = (ptOptions->adwUserData[0]*ptInfo->dwACSFactor)/ACS_STEP;
				tVICOptions.adwUserData[2] = ptInfo->sdwContrast;
				break;
/* ======================================== */
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_FRAME_RATE:
				ptInfo->dwFrameRate = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[0] = ptInfo->dwFrameRate;
				break;
/* Version 12.0.0.3 modification, 2013.01.16 */
/* ======================================== */
/* Version 5.2.0.5 modification, 2008.11.27 */
			case VIDEO_SIGNAL_OPTION_RESET_FRAME_RATE_CTRL:
				ptInfo->dwFrameCount = 0;
				ptInfo->dwDiffResidue = 0;
				return S_OK;
/* ======================================== */
/* Version 6.0.0.0 modification, 2009.06.09 */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS: // change AE measure windows
/* ======================================== */
/* Version 7.0.0.0 modification, 2009.10.11 */
/* Version 7.0.0.1 modification, 2009.10.16 */
/* Version 12.0.0.0 modification, 2012.06.28 */
				memcpy(adwAEMeasureWinTbl, (DWORD *)ptOptions->adwUserData[0], sizeof(DWORD)*2);
				dwAEWinStartX = ptInfo->dwStartPixel + ((adwAEMeasureWinTbl[0]&0x00001FFF));
				dwAEWinStartY = ptInfo->dwStartLine + (((adwAEMeasureWinTbl[0]>>16)&0x00001FFF));
				adwAEMeasureWinTbl[0] = (dwAEWinStartY<<16) | dwAEWinStartX;
/* ======================================== */
/* ======================================== */
/* Version 7.3.0.1 modification, 2009.12.10 */
/* ======================================== */
/* ======================================== */
/* Version 7.2.0.1 modification, 2009.11.09 */
/* ======================================== */
				tVICOptions.adwUserData[0] = (DWORD)adwAEMeasureWinTbl;
/* ======================================== */
				break;
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOW_PRIORITY:
			case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_WINDOW_PRIORITY:
/* Version 12.0.0.0 modification, 2012.06.28 */
				tVICOptions.adwUserData[0] = (DWORD)ptOptions->adwUserData[0];
/* ======================================== */
				break;
/* Version 6.4.0.1 modification, 2009.10.07 */
/* Version 6.2.0.0 modification, 2009.09.29 */
			case VIDEO_SIGNAL_OPTION_SET_TONE_MAPPING:
/* Version 8.0.0.2 modification, 2010.05.17 */
/* Version 13.0.0.2 modification, 2013.12.09 */
/* Version 12.0.0.3 modification, 2013.01.16 */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 7.2.0.1 modification, 2009.11.09 */
				tVICOptions.adwUserData[0] = (ptOptions->adwUserData[0]&0x00000001) |
											 ((ptOptions->adwUserData[1]&0x7)<<1);
/* ======================================== */
				break;
/* Version 7.2.0.2 modification, 2009.11.09 */
			case VIDEO_SIGNAL_OPTION_SET_CONTRAST_ENHANCEMENT:
/* Version 8.0.0.8 modification, 2010.11.01 */
				ptInfo->bCEEn = (BOOL) (ptOptions->adwUserData[0] & 0x1);
				ptInfo->bCEAutoEn = (BOOL) ((ptOptions->adwUserData[0]>>1) & 0x1);
				ptInfo->bCEUserTblEn = (BOOL) ((ptOptions->adwUserData[0]>>2) & 0x1);
				ptInfo->byCEMethod = (BYTE) ((ptOptions->adwUserData[0]>>3) & 0x3);
				ptInfo->byCEBrightness = (BYTE) ((ptOptions->adwUserData[0]>>5) & 0x7);

				dwPrevCETblAddr = (DWORD)(ptInfo->dwCETblAddr);

				ptInfo->dwCEUserTblAddr = (DWORD) (ptOptions->adwUserData[1]);

				if (ptInfo->bCEUserTblEn == TRUE)
				{
					pbyUserCETbl = (BYTE *)(ptInfo->dwCEUserTblAddr);
					for (i=0; i<256; i++)
					{
						ptInfo->pdwCEUserTbl[i] = (DWORD)(pbyUserCETbl[i]<<24) |
												  (DWORD)(pbyUserCETbl[i]<<16) |
												  (DWORD)(pbyUserCETbl[i]<<8) |
												  (DWORD)(pbyUserCETbl[i]);
					}
					ptInfo->dwCETblAddr = (DWORD)(ptInfo->pdwCEUserTbl);
				}
				else if (ptInfo->bCEAutoEn == TRUE)
				{
						{
							ptInfo->dwCETblAddr = (DWORD) ptInfo->pdwCETbl3;
						}
				}
				else	//Use default 4 CE tables
				{
					//printf("Use default Tables %d\n", ptInfo->byCEMethod);
					if (ptInfo->byCEMethod == 0)
					{
						ptInfo->dwCETblAddr = (DWORD) ptInfo->pdwCETbl0;
					}
					else if (ptInfo->byCEMethod == 1)
					{
						ptInfo->dwCETblAddr = (DWORD) ptInfo->pdwCETbl1;
					}
					else if (ptInfo->byCEMethod == 2)
					{
						ptInfo->dwCETblAddr = (DWORD) ptInfo->pdwCETbl2;
					}
					else
					{
						ptInfo->dwCETblAddr = (DWORD) ptInfo->pdwCETbl3;
					}
				}

				tVICOptions.adwUserData[0] = (tVICOptions.adwUserData[0]&0xFFFFFFFE) |
											 (ptOptions->adwUserData[0] & 0x00000001);
				tVICOptions.adwUserData[0] = (tVICOptions.adwUserData[0]&0xFFFFFFE1) |
											 (((ptOptions->adwUserData[0]>>5) & 0x7)<<1);


				tVICOptions.adwUserData[1] = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->dwCETblAddr);

				dwCurrCETblAddr = (DWORD)(ptInfo->dwCETblAddr);
				if (dwPrevCETblAddr != dwCurrCETblAddr)
				{
					tVICOptions.adwUserData[0] = (tVICOptions.adwUserData[0]&0xFFFFFF9F) |
												 (0x20);		//Load CE table enable
					printf("Update CE Table !!\n");
				}
				break;	
/* ======================================== */			
/* ======================================== */
/* Version 6.3.0.0 modification, 2009.10.05 */
			case VIDEO_SIGNAL_OPTION_PHOTO_LDC_CALIBRATE:
/* Version 8.0.0.2 modification, 2010.05.17 */
/* Version 13.0.0.2 modification, 2013.12.09 */
/* Version 12.0.0.3 modification, 2013.01.16 */
/* ======================================== */
/* ======================================== */
/* ======================================== */
				ptInfo->bPhotoLDCEn = FALSE;
				ptInfo->ptVICInfo->dwPhotoLDCCtrl = (ptInfo->ptVICInfo->dwPhotoLDCCtrl&0xFFFFFFFE) | ptInfo->bPhotoLDCEn;

/* Version 7.1.0.0 modification, 2009.10.16 */
				dwVideoSize = ptInfo->dwVideoWidth*ptInfo->dwVideoHeight;
/* Version 12.0.0.2 modification, 2012.11.22 */
				if (ptInfo->dwVideoWidth <= 512)
				{
					dwScaleNum = 0;
				}
				else if (ptInfo->dwVideoWidth <= 1024)
				{
					dwScaleNum = 1;
				}
				else if (ptInfo->dwVideoWidth <= 2048)
				{
					dwScaleNum = 2;
				}
				else
				{
					dwScaleNum = 3;
				}

				dwPhotoLDCTblSize = dwVideoSize >> dwScaleNum; // size of WORD
				dwLDCTblWidth = (ptInfo->dwVideoWidth>>dwScaleNum);
				dwLDCOffset = (ptInfo->dwStartLine*dwLDCTblWidth) + (ptInfo->dwStartPixel>>dwScaleNum);

				pwPhotoLDCTbl = (WORD *)ptOptions->adwUserData[0];
				pbyYFrame = (BYTE *)ptOptions->adwUserData[1];
/* Version 12.1.0.0 modification, 2013.03.06 */
				dwMemWidth = (ptOptions->adwUserData[2]&0xFFFF);
				dwLSCRatio = ((ptOptions->adwUserData[2]>>16)&0xFFFF);
/* ======================================== */
/* ======================================== */

/* Version 12.1.0.0 modification, 2013.03.06 */
				for (k=0; k<255; k++)
					adwHistogram[k] = 0; 

				for (k=0; k<(ptInfo->dwCapWidth*ptInfo->dwCapHeight); k++)
				{
					adwHistogram[pbyYFrame[k]]++;				
				}			
				m = 0;
				for (k=0; k<255; k++)
				{
					if (adwHistogram[k] >= m)
					{
						m = adwHistogram[k];
						dwMaxPxlValue = k;
					}
				}
/* ======================================== */
/* Version 12.0.0.2 modification, 2012.11.22 */
 				for (k=0; k<ptInfo->dwCapHeight; k++)
				{
					i = k*(dwLDCTblWidth) + dwLDCOffset;
					for (m=0; m<(ptInfo->dwCapWidth>>dwScaleNum); m++)
					{
						if ((i+m) >= dwPhotoLDCTblSize)	 break;
						pwPhotoLDCTbl[i+m] = (WORD)((pbyYFrame[m<<dwScaleNum]<=0)
												? (dwMaxPxlValue<<10)
/* Version 12.1.0.0 modification, 2013.03.06 */
												: (dwMaxPxlValue>=pbyYFrame[m<<dwScaleNum])
												? ((1<<10)+((((dwMaxPxlValue-pbyYFrame[m<<dwScaleNum])<<10)*dwLSCRatio)/pbyYFrame[m<<dwScaleNum]/100))
												: ((1<<10)-((((pbyYFrame[m<<dwScaleNum]-dwMaxPxlValue)<<10)*dwLSCRatio)/pbyYFrame[m<<dwScaleNum]/100)));
/* ======================================== */
					}

					if ((i+m) >= dwPhotoLDCTblSize)	 break;
					pbyYFrame += dwMemWidth;
				}
/* ======================================== */
/* Version 12.1.0.0 modification, 2013.03.06 */
				for (k=ptInfo->dwStartLine; k<(ptInfo->dwCapHeight+ptInfo->dwStartLine); k++)
				{
					for (m=(ptInfo->dwStartPixel>>dwScaleNum); m<((ptInfo->dwStartPixel+ptInfo->dwCapWidth)>>dwScaleNum); m++)
					{
						sdwStartX = m - 2;
						sdwStartY = k - 2;
						dwAverage = 0;
						dwAverCnt = 0;
						dwMaxPixel  = 0;
						dwMinPixel = 0xFFFFFFFF;

						for (sdwLineCnt=sdwStartY; sdwLineCnt<(sdwStartY+5); sdwLineCnt++)
						{
							for (sdwPixelCnt=sdwStartX; sdwPixelCnt<(sdwStartX+5); sdwPixelCnt++)
							{
								if ((sdwLineCnt>=ptInfo->dwStartLine) &&
									(sdwLineCnt<(ptInfo->dwCapHeight+ptInfo->dwStartLine)) && 
									(sdwPixelCnt>=(ptInfo->dwStartPixel>>dwScaleNum)) && 
									(sdwPixelCnt<((ptInfo->dwStartPixel+ptInfo->dwCapWidth)>>dwScaleNum)))
								{
									dwAverage += pwPhotoLDCTbl[(sdwLineCnt*dwLDCTblWidth)+sdwPixelCnt];
									dwAverCnt++;
									 
									if (pwPhotoLDCTbl[(sdwLineCnt*dwLDCTblWidth)+sdwPixelCnt] <= dwMinPixel)
									{
										dwMinPixel = pwPhotoLDCTbl[(sdwLineCnt*dwLDCTblWidth)+sdwPixelCnt];
									}

									if (pwPhotoLDCTbl[(sdwLineCnt*dwLDCTblWidth)+sdwPixelCnt] >= dwMaxPixel)
									{
										dwMaxPixel = pwPhotoLDCTbl[(sdwLineCnt*dwLDCTblWidth)+sdwPixelCnt];
									}
								}
							}
						}
						dwAverage = dwAverage - dwMaxPixel - dwMinPixel;
						pwPhotoLDCTbl[(k*dwLDCTblWidth)+m] = (dwAverCnt>2) ? (dwAverage/(dwAverCnt-2)) : (dwAverage/dwAverCnt);
					}

				}
/* ======================================== */
				ptInfo->pwPhotoLDCTbl = pwPhotoLDCTbl;
				ptInfo->ptVICInfo->dwPhotoLDCTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pwPhotoLDCTbl);
				return S_OK;
			case VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_TABLE:
				ptInfo->bPhotoLDCEn = FALSE;
				ptInfo->ptVICInfo->dwPhotoLDCCtrl = (ptInfo->ptVICInfo->dwPhotoLDCCtrl&0xFFFFFFFE) | ptInfo->bPhotoLDCEn;

				ptInfo->pwPhotoLDCTbl = (WORD *)ptOptions->adwUserData[0];
				ptInfo->ptVICInfo->dwPhotoLDCTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pwPhotoLDCTbl);

				ptInfo->bPhotoLDCEn = TRUE;
				ptInfo->ptVICInfo->dwPhotoLDCCtrl = (ptInfo->ptVICInfo->dwPhotoLDCCtrl&0xFFFFFFFE) | ptInfo->bPhotoLDCEn;
				return S_OK;
/* ======================================== */
/* ======================================== */
/* Version 7.0.0.0 modification, 2009.10.08 */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_COLOR_SUPPRESSION:
				ptInfo->bACSEn = (ptOptions->adwUserData[0]==1) ? TRUE : FALSE;
				ptInfo->dwACSLumaMax = ptOptions->adwUserData[1];
				ptInfo->dwACSLumaMin = ptOptions->adwUserData[2];
/* Version 7.2.0.1 modification, 2009.11.09 */
				if (ptInfo->bACSEn == TRUE)
				{
/* ======================================== */
    				if (ptInfo->dwACSLumaMax < ptInfo->dwACSLumaMin)
    				{
/* Version 12.0.0.3 modification, 2013.01.16 */
    					ptInfo->bACSEn = FALSE;
    					ERRPRINT("adwUserData[1] should be larger than adwUserData[2] !!\n");
    					return _______VIDEOCAP_ERR_INVALID_ARG;
/* ======================================== */
    				}
    				else
    				{
    					ptInfo->dwACSLumaRange = ptInfo->dwACSLumaMax - ptInfo->dwACSLumaMin;
    					return S_OK;
    				}
/* Version 7.2.0.1 modification, 2009.11.09 */
				}
/* ======================================== */
				break;
/* ======================================== */
/* Version 7.2.0.0 modification, 2009.11.04 */
			case VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_EN:
				ptInfo->bPhotoLDCEn = (BOOL)ptOptions->adwUserData[0];
				ptInfo->ptVICInfo->dwPhotoLDCCtrl = (ptInfo->ptVICInfo->dwPhotoLDCCtrl&0xFFFFFFFE) | ptInfo->bPhotoLDCEn;
				return S_OK;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER:		
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN:
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER:
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN:
/* Version 8.0.0.4 modification, 2010.08.13 */
			case VIDEO_SIGNAL_OPTION_AUTO_DETECT_STD:
/* ======================================== */
/* Version 8.0.0.5 modification, 2010.08.18 */
			case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
/* ======================================== */
/* Version 11.0.0.0 modification, 2011.11.02 */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_SPEED:
			case VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_ACTIVE_TIME:
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_BLACK_CLAMP:
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				break;								
			case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE:
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = ptOptions->adwUserData[1];
				break;
/* ======================================== */
/* Version 8.0.0.14 modification, 2011.02.08 */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_EN:
/* Version 10.0.0.0 modification, 2011.07.15 */
			case VIDEO_SIGNAL_OPTION_SET_FOCUS_POSITION:
			case VIDEO_SIGNAL_OPTION_SET_FOCUS_SPEED: 				
			case VIDEO_SIGNAL_OPTION_SET_ZOOM_POSITION:			
			case VIDEO_SIGNAL_OPTION_SET_ZOOM_SPEED:
			case VIDEO_SIGNAL_OPTION_SET_FOCUS_NOISE_THRES:
			case VIDEO_SIGNAL_OPTION_SET_ZOOMTRACKING_FOCUS_EN:
/* ======================================== */
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				break;
/* ======================================== */
/* Version 10.0.0.0 modification, 2011.07.15 */
			case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_WINDOW: // change AF measure window and trigger auto-focus
				memcpy(adwAFMeasureWinTbl, (DWORD *)ptOptions->adwUserData[0], sizeof(DWORD)*2);
/* Version 10.0.0.5 modification, 2011.10.25 */
				dwAFWinWidth = (adwAFMeasureWinTbl[1]>>16)&0x1FFF;
				dwAFWinHeight = adwAFMeasureWinTbl[1]&0x1FFF;
				if (dwAFWinWidth > 2048)
				{
/* Version 12.0.0.3 modification, 2013.01.16 */
					ERRPRINT("The horizontal width of the focus statistic window should not be larger than 2048(your setting is %d).\n", ptOptions->adwUserData[1]>>16);
					return _______VIDEOCAP_ERR_INVALID_ARG;
/* ======================================== */
				}
				dwAFWinStartX = (adwAFMeasureWinTbl[0]>>16)&0x1FFF; 
				dwAFWinStartY = adwAFMeasureWinTbl[0]&0x1FFF; 			
				if (((dwAFWinStartX+dwAFWinWidth) > ptInfo->dwCapWidth) || ((dwAFWinStartY+dwAFWinHeight) > ptInfo->dwCapHeight))
				{
/* Version 12.0.0.3 modification, 2013.01.16 */
					ERRPRINT("The sum of the AF window height and the AF window start line must be less or equal to capture height.\n");
					ERRPRINT("The sum of the AF window width and the AF window start pixel must be less or equal to capture width.\n");
					return _______VIDEOCAP_ERR_INVALID_ARG;
/* ======================================== */
				}	
/* ======================================== */
				dwAFGridWidth = dwAFWinWidth / 8;
				dwAFGridHeight = dwAFWinHeight / 8;

				ptInfo->dwAFWindowStartPosX = ((SDWORD)(dwAFWinStartX-dwAFGridWidth) <= 0) ? 0 : (dwAFWinStartX-dwAFGridWidth);
				ptInfo->dwAFWindowStartPosY = ((SDWORD)(dwAFWinStartY-dwAFGridHeight) <= 0) ? 0 : (dwAFWinStartY-dwAFGridHeight);

				dwAFShiftWidth = dwAFWinStartX - ptInfo->dwAFWindowStartPosX;
				dwAFShiftHeight = dwAFWinStartY - ptInfo->dwAFWindowStartPosY;

				ptInfo->dwAFWindowWidth = ((dwAFWinWidth+dwAFShiftWidth) >= ptInfo->dwCapWidth) ? ptInfo->dwCapWidth : (dwAFWinWidth+dwAFShiftWidth);
				ptInfo->dwAFWindowHeight = ((dwAFWinHeight+dwAFShiftHeight) >= ptInfo->dwCapHeight) ? ptInfo->dwCapHeight : (dwAFWinHeight+dwAFShiftHeight);

				adwAFMeasureWinTbl[0] = (((ptInfo->dwAFWindowStartPosX&0x00001FFF)<<16) |
										(ptInfo->dwAFWindowStartPosY&0x00001FFF));
				adwAFMeasureWinTbl[1] = (((ptInfo->dwAFWindowWidth&0x00001FFF)<<16) |
										(ptInfo->dwAFWindowHeight&0x00001FFF));

				tVICOptions.adwUserData[0] = (DWORD)adwAFMeasureWinTbl;
				break;
/* ======================================== */
/* Version 10.0.0.2 modification, 2011.08.26 */
			case VIDEO_SIGNAL_OPTION_GET_AUTO_FOCUS_TABLE_SIZE:
				while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_AF_TBL_SIZE, &dwMemSize) != S_OK);
				ptOptions->adwUserData[0] = dwMemSize;
				break;
			case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_CALIBRATE:
				break;
			case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_TABLE:
				tVICOptions.adwUserData[0] = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptOptions->adwUserData[0]);
				break;
/* ======================================== */
/* Version 10.0.0.3 modification, 2011.09.21 */
			case VIDEO_SIGNAL_OPTION_SET_ANTI_ALIASING:
				memcpy(dwCFAAntiAliasingTbl, (DWORD *)ptOptions->adwUserData[0], sizeof(DWORD)*10);
				tVICOptions.adwUserData[0] = (DWORD)dwCFAAntiAliasingTbl;
				break;
/* ======================================== */
			case VIDEO_SIGNAL_OPTION_SET_IMPULSE_NOISE_REMOVAL:
				tVICOptions.adwUserData[0] = ptOptions->adwUserData[0];
				tVICOptions.adwUserData[1] = (DWORD)ptOptions->adwUserData[1];
				break;
/* Version 12.0.0.1 modification, 2012.11.02 */
			case VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE:
				while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_SETOPTIONS, &tVICOptions) != S_OK);
				ptOptions->adwUserData[0] = tVICOptions.adwUserData[0];
				return S_OK;
/* ======================================== */
			default:
/* Version 5.5.0.0 modification, 2008.12.29 */
/* Version 12.0.0.3 modification, 2013.01.16 */
				ERRPRINT("Unsupported option flag %d !!\n", ptOptions->eOptionFlags);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_NOT_SUPPORT_OPTION_FLAGS;
/* ======================================== */
/* ======================================== */
		}
/* Version 3.1.0.4 modification, 2008.03.04 */
/* Version 5.8.0.0 modification, 2009.04.08 */
		while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_SETOPTIONS, &tVICOptions) != S_OK);
/* ======================================== */
		return S_OK;
/* ======================================== */
/* Version 2.0.0.2 modification, 2006.08.24 */
	}
/* ======================================== */
	return S_OK;
}
/* ======================================== */
/* ============================================================================================== */
DWORD VideoCap_QueryMemSize(TVideoCapInitOptions *ptInitOptions)
{
	return (sizeof(TVideoCapInfo));
}

/* ============================================================================================== */
SCODE VideoCap_GetVersionInfo(TVersionNum *ptVersion)
{
	ptVersion->byMajor = VIDEOCAP_VERSION & 0x000000FF;
	ptVersion->byMinor = (VIDEOCAP_VERSION>>8) & 0x000000FF;
	ptVersion->byBuild = (VIDEOCAP_VERSION>>16) & 0x000000FF;
	ptVersion->byRevision = (VIDEOCAP_VERSION>>24) & 0x000000FF;

	return S_OK;
}

/* ============================================================================================== */
