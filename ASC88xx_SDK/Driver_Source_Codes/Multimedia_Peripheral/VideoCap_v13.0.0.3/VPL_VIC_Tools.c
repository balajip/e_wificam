/*

/* ============================================================================================== */
#include "VideoCap_Locals.h"

/* ============================================================================================== */
SCODE VideoCap_GetBufVIC(TVideoCapInfo *ptInfo, TVideoCapState *ptState)
{
	TVPLVICState tVICState;
	SCODE scStatus;

	scStatus = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_BUF, (DWORD)(&tVICState));

/* Version 3.4.0.0 modification, 2008.09.24 */
/* Version 4.0.0.1 modification, 2008.10.03 */
/* Version 7.0.0.0 modification, 2009.10.08 */
/* ======================================== */
/* Version 6.4.0.0 modification, 2009.10.06 */
	ptState->dwAWBRedGain = (tVICState.dwAWBGain>>16) & 0x1FFF;
	ptState->dwAWBBlueGain = tVICState.dwAWBGain & 0x1FFF;
/* ======================================== */
/* Version 8.1.0.0 modification, 2011.06.21 */
/* Version 12.0.0.0 modification, 2012.06.28 */
	ptState->qwAWBRedSum   = tVICState.qwAWBRedSum;
	ptState->qwAWBGreenSum = tVICState.qwAWBGreenSum;
	ptState->qwAWBBlueSum  = tVICState.qwAWBBlueSum;
/* ======================================== */
/* ======================================== */
	ptState->dwAFCurrentStatus = tVICState.dwAFCurrentStatus;
/* Version 9.0.0.0 modification, 2011.07.04 */
	ptState->dwAEShutter = tVICState.dwShutter;
	ptState->dwAEGain    = tVICState.dwGain;
/* ======================================== */
/* Version 10.0.0.0 modification, 2011.07.15 */
	ptState->dwZoomRatio = tVICState.dwZoomRatio;
/* ======================================== */
/* Version 11.0.0.0 modification, 2011.11.02 */
	ptState->bAEStable    = tVICState.bAEStable;
/* ======================================== */
/* Version 5.4.0.0 modification, 2008.12.19 */
	ptState->dwAFFocusValueHigh = tVICState.dwAFFocusValueHigh;
	ptState->dwAFFocusValueLow = tVICState.dwAFFocusValueLow;
/* ======================================== */
/* ======================================== */
/* ======================================== */

	ptState->dwIndex = tVICState.dwIndex;
/* Version 2.0.0.0 modification, 2006.06.21 */
/* Version 3.0.0.0 modification, 2007.12.06 */
/* Version 3.0.0.2 modification, 2008.01.07 */
/* Version 3.1.0.4 modification, 2008.03.04 */
/* Version 7.0.0.0 modification, 2009.10.08 */
	ptState->bFifoFull = ((tVICState.dwErrACK&0x12) == 0x12);
	ptState->bNoSignal = ((tVICState.dwErrACK&0x14) == 0x14);
/* ======================================== */
	ptState->bCCIRErr = ((tVICState.dwErrACK&0x17) == 0x10);
	DBPRINT1("dwErrACK of captured buffer: %08X\n", tVICState.dwErrACK);
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 5.2.0.1 modification, 2008.11.10 */
	ptState->dwSecond = tVICState.dwSecond;
	ptState->dwMilliSecond = tVICState.dwMilliSecond;
/* Version 5.2.0.3 modification, 2008.11.18 */
	ptState->dwMicroSecond = tVICState.dwMicroSecond;
/* ======================================== */
	ptState->bTimeStampCal = tVICState.bTimeStampCal;
/* ======================================== */
    ptState->dwFrameCount = tVICState.dwFrameCount;
/* Version 5.6.0.0 modification, 2009.01.16 */
	/* Enable AF FSM OP START */
	if (tVICState.dwFrameCount==16)
	{
		ptInfo->ptVICInfo->dwAFCtrl = ptInfo->ptVICInfo->dwAFCtrl | 0x00000300;
	}
/* ======================================== */
	ptState->dwOutWidth = tVICState.dwOutWidth;
	ptState->dwOutHeight = tVICState.dwOutHeight;
/* Version 5.7.0.2 modification, 2009.03.16 */
	ptState->dwOutStride = tVICState.dwOutStride;
/* ======================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
	ptState->dwInWidth = tVICState.dwInWidth;
/* Version 5.8.0.3 modification, 2009.04.21 */
	DBPRINT1("tVICState.dwYFrameAddr:%08x\n", tVICState.dwYFrameAddr);
/* ======================================== */
	ptState->pbyYFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, tVICState.dwYFrameAddr);
	ptState->pbyCbFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, tVICState.dwCbFrameAddr);
	ptState->pbyCrFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, tVICState.dwCrFrameAddr);
/* ======================================== */

	return scStatus;
}

/* ============================================================================================== */
SCODE VideoCap_ReleaseBufVIC(TVideoCapInfo *ptInfo, TVideoCapState *ptState)
{
/* Version 12.0.0.3 modification, 2013.01.16 */
	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_RELEASE_BUF, (DWORD)ptState->dwIndex);

	return S_OK;
/* ======================================== */
}

/* ============================================================================================== */
SCODE VideoCap_WaitVIC(TVideoCapInfo *ptInfo)
{
	DWORD dwNum = ptInfo->dwDeviceNum;
/* Version 5.6.0.2 modification, 2009.02.20 */
#ifdef __USE_PROFILE__
	DWORD dwBandwidth;
	DWORD dwRGInterval;
	DWORD dwReqTimes;
#endif //__USE_PROFILE__

/* ======================================== */
/* Version 3.0.0.0 modification, 2007.09.03 */
	while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_WAIT_COMPLETE) != S_OK);
/* ======================================== */

/* Version 5.6.0.2 modification, 2009.02.20 */
#ifdef __USE_PROFILE__
	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_BANDWIDTH, (DWORD)&dwBandwidth);
/* Version 5.10.0.4 modification, 2009.05.23 */
	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_RG_INTERVAL, (DWORD)&dwRGInterval);
	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_REQ_TIMES, (DWORD)&dwReqTimes);
/* ======================================== */
#ifndef __USE_SEQ_PROFILE__
	printf("Request times = %d\n", dwReqTimes);
	printf("Req-Grant interval = %d\n", dwRGInterval);
	printf("Bandwidth = %d\n", dwBandwidth);
#endif //__USE_SEQ_PROFILE__
	ptInfo->dwBusCycles = ptInfo->dwBusCycles + dwBandwidth;
	ptInfo->dwBusLatency = ptInfo->dwBusLatency + dwRGInterval;
	ptInfo->dwBusReqTimes = ptInfo->dwBusReqTimes + dwReqTimes;
#endif //__USE_PROFILE__
/* ========================================= */

	return S_OK;
}

/* ============================================================================================== */
SCODE VideoCap_StartVIC(TVideoCapInfo *ptInfo)
{
	DWORD dwNum = ptInfo->dwDeviceNum;

#ifdef __USE_PROFILE__
	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_CLEAR_PROFILE);
#endif //__USE_PROFILE__

/* Version 2.0.0.0 modification, 2006.06.21 */
/* Version 5.10.0.2 modification, 2009.05.09 */
	if ((ptInfo->ptVICInfo->adwChanCtrl[dwNum]&0x00000100) == 0x00000100)
/* ======================================== */
	{
		// device is enabled
		DBPRINT1("Device %d is already enabled !!\n", dwNum);
		return S_OK;
	}
/* ======================================== */
/* Version 10.0.0.1 modification, 2011.08.05 */
/* Version 10.0.0.4 modification, 2011.10.21 */
/* ======================================== */
/* ======================================== */
/* Version 5.10.0.2 modification, 2009.05.09 */
	/* Enable output enable */
/* Version 10.0.0.4 modification, 2011.10.21 */
	if (ptInfo->dwChipVersion == 1) // V1 shoud set OUT_EN here, but V2 set OUT_EN in VIC ISR
	{
/* ======================================== */
		ptInfo->ptVICInfo->adwChanCtrl[dwNum] = ptInfo->ptVICInfo->adwChanCtrl[dwNum] | 0x00000500;
/* Version 10.0.0.4 modification, 2011.10.21 */
	}
/* ======================================== */
/* ======================================== */
	/* Enable interrupt */
/* Version 8.0.0.9 modification, 2010.11.15 */
	ptInfo->ptVICInfo->adwChanCtrl[dwNum] = ptInfo->ptVICInfo->adwChanCtrl[dwNum] | 0x000000E8;
/* ======================================== */
	/* Enable VIC */
	while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_START_VIC) != S_OK);

/* Version 2.0.0.0 modification, 2006.06.21 */
/* ======================================== */

	return S_OK;
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_OpenVIC(TVideoCapInfo *ptInfo)
/* ======================================== */
{
/* Version 2.0.0.0 modification, 2006.06.21 */
/* ======================================== */
	CHAR acDevPathName[20];
	DWORD dwNum = ptInfo->dwDeviceNum;

	sprintf(acDevPathName, "/dev/vpl_vic%d", dwNum);
	if ((ptInfo->hVICDev=(HANDLE)open(acDevPathName, O_RDWR)) < 0 )
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to open vpl_vic device (%s) driver !!\n", acDevPathName);
		return _______VIDEOCAP_ERR_OPEN_DEVICE_DRIVER;
/* ======================================== */
	}
	else
	{
		DBPRINT1("Open vpl_vic device (%s) success !!\n", acDevPathName);
	}

	ptInfo->ptVICInfo = (TVPLVICInfo *)mmap(0,
                                            sizeof(TVPLVICInfo),
	                                        PROT_READ|PROT_WRITE,
	                                        MAP_SHARED,
	                                        (int)ptInfo->hVICDev,
	                                        0);
/* Version 2.0.0.0 modification, 2006.06.21 */
	DBPRINT1("MMR setting, base: %lx\n", ptInfo->ptVICInfo);
/* ======================================== */
	return S_OK;
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_InitVIC(TVideoCapInfo *ptInfo, TVideoCapInitOptions *ptInitOptions)
{
	TVPLVICChnInfo *ptChnInfo;
	TVPLVICInitOptions tVICInitOptions;
	DWORD dwCtrl;
	SWORD swContrast, swSaturation;
	DWORD dwNum = ptInfo->dwDeviceNum;
	BYTE byMux0Num, byMux1Num;
/* Version 6.2.0.0 modification, 2009.09.29 */
/* ======================================== */
	DWORD dwAFWinStartX, dwAFWinStartY;
	DWORD dwAFGridWidth, dwAFGridHeight;
	DWORD dwAFShiftWidth, dwAFShiftHeight;
	SDWORD sdwResult;

	ptChnInfo = (TVPLVICChnInfo *)&ptInfo->ptVICInfo->atChnInfo[dwNum];

	dwCtrl = (ptInitOptions->bFieldInvert<<18) |
			 (ptInitOptions->bFieldMode<<17) |
			 ((ptInitOptions->eOutPixelFormat==YUV420)<<16) |
			 (0x0<<15) |		// eccfvh
			 (ptInitOptions->bMirror<<14) |
			 (ptInitOptions->bFlip<<13) |
/* Version 5.10.0.0 modification, 2009.04.24 */
			 ((((ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)||(ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_BT1120))?0x0:0x1)<<12) |		// ccir656p
/* ======================================== */
/* Version 5.10.0.7 modification, 2009.06.06 */
			 (0x0<<10) |		// no_sig_cnt_en
/* ======================================== */
/* Version 5.10.0.2 modification, 2009.05.09 */
			 (0x0<<8) |		// out_en
/* ======================================== */
			 (0x0<<7) |			// int_en
			 (0x1<<6) |			// fifo_full_out_en
			 (0x1<<5) |			// nosig_err_ack_en
			 (0x0<<4) |			// fifo_full_ack_en
			 (0x1<<3) |			// op_cmpt_ack_en
/* Version 5.10.0.6 modification, 2009.06.03 */
			 (0x0<<2) |			// nosig_err_ack
			 (0x0<<1) |			// fill_full_ack
			 (0x0);			// op_cmpt_ack
/* ======================================== */

	ptInfo->ptVICInfo->adwChanCtrl[dwNum] = dwCtrl;

/* Version 7.3.0.3 modification, 2010.01.12 */
	switch (ptInitOptions->byChNum0)
	{
		case 0:
			byMux0Num = 3;
			break;
		case 1:
			byMux0Num = 0;
			break;
		case 2:
			byMux0Num = 1;
			break;
		default:
			byMux0Num = 2;
			break;
	}

	switch (ptInitOptions->byChNum1)
	{
		case 0:
			byMux1Num = 3;
			break;
		case 1:
			byMux1Num = 0;
			break;
		case 2:
			byMux1Num = 1;
			break;
		default:
			byMux1Num = 2;
			break;
	}

	ptInfo->ptVICInfo->adwDevCtrl[0] = (ptInitOptions->eChIDType0<<7) |
									   (byMux0Num<<5) |
									   (ptInitOptions->bChClkEdge0<<4);
	ptInfo->ptVICInfo->adwDevCtrl[1] = (ptInitOptions->eChIDType1<<7) |
									   (byMux1Num<<5) |
									   (ptInitOptions->bChClkEdge1<<4);
/* ======================================== */

	ptInfo->ptVICInfo->adwSyncDelay[0] = 0;
	ptInfo->ptVICInfo->adwSyncDelay[1] = 0;
/* Version 5.10.0.0 modification, 2009.04.24 */
	ptInfo->ptVICInfo->dwCtrl = ((ptInfo->ptVICInfo->dwCtrl)&0xFFFFFFC3) |
/* Version 7.3.0.3 modification, 2010.01.12 */
								((((ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_BT1120)||(ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120))?0x1:0x0)<<5) |	// header_ref for bt1120
/* ======================================== */
								(ptInfo->eFormat<<2);
/* ======================================== */

	ptChnInfo->dwInSize = ((ptInfo->dwVideoHeight&0x0000FFFF)<<16) |
						(ptInfo->dwVideoWidth&0x0000FFFF);
	ptChnInfo->dwCapH = ((ptInitOptions->dwCapWidth&0x0000FFFF)<<16) |
						(ptInitOptions->dwStartPixel&0x0000FFFF);
	ptChnInfo->dwCapV = ((ptInitOptions->dwCapHeight&0x0000FFFF)<<16) |
						(ptInitOptions->dwStartLine&0x0000FFFF);
	ptChnInfo->dwStride = (ptInitOptions->dwMaxFrameWidth&0x0000FFFF);

/* Version 7.3.0.4 modification, 2010.03.05 */
	swContrast = ptInfo->sdwContrast;
/* ======================================== */
	swSaturation = ptInitOptions->dwSaturation;
	swSaturation = (((swContrast+128)*swSaturation)>>7);

	ptChnInfo->dwSBC = ((swSaturation&0x000001FF)<<16) |
		               ((ptInitOptions->dwBrightness&0x000000FF)<<8) |
		               (ptInitOptions->dwContrast&0x000000FF);

/* Version 3.0.0.0 modification, 2007.09.03 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
/* Version 5.0.0.0 modification, 2008.10.09 */
	ptInfo->ptVICInfo->dwCMOSCtrl = ((0x0)<<22) |	 // ob
									((ptInfo->bAdditionalPad_en)<<21) |			 // AdditionalPad_En
									((ptInfo->byBayerBitwidth&0x0000001F)<<16) | // BayerBitwidth
									((ptInfo->dwHorBlanking&0x00000FFF)<<4) |	 // Hori-blanking
/* Version 5.10.0.6 modification, 2009.06.03 */
									((0x0)<<3) |								 // LoadTbl_en
/* ======================================== */
									((ptInfo->bDenoise_en)<<2) |				 // Denoise_en
									((ptInfo->eCFAMode)&0x00000003);			 // CFA Mode
/* ======================================== */
	ptInfo->ptVICInfo->dwRGB2YCbCrCoeff0 = (abyRGB2YCbCrBT601[3]<<24) |
										   (abyRGB2YCbCrBT601[2]<<16) |
										   (abyRGB2YCbCrBT601[1]<<8) |
										   (abyRGB2YCbCrBT601[0]);
	ptInfo->ptVICInfo->dwRGB2YCbCrCoeff1 = (abyRGB2YCbCrBT601[7]<<24) |
										   (abyRGB2YCbCrBT601[6]<<16) |
										   (abyRGB2YCbCrBT601[5]<<8) |
										   (abyRGB2YCbCrBT601[4]);
/* Version 5.0.0.0 modification, 2008.10.09 */
	ptInfo->ptVICInfo->dwRGB2YCbCrCoeff2 = ((ptInfo->byYKiller&0xFF)<<13) |
										   (abyRGB2YCbCrBT601[9]<<8) |
										   (abyRGB2YCbCrBT601[8]);

	ptInfo->ptVICInfo->dwDenoiseG0Thrd = ((ptInfo->dwThr1&0x0000FFFF)<<16) |
											(ptInfo->dwThr0&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseG0MaxThrd = ((ptInfo->dwDeNoiseThrMax1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMax2&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseG0MinThrd = ((ptInfo->dwDeNoiseThrMin1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMin2&0x0000FFFF);

	ptInfo->ptVICInfo->dwDenoiseRThrd = ((ptInfo->dwThr1&0x0000FFFF)<<16) |
											(ptInfo->dwThr0&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseRMaxThrd = ((ptInfo->dwDeNoiseThrMax1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMax2&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseRMinThrd = ((ptInfo->dwDeNoiseThrMin1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMin2&0x0000FFFF);

	ptInfo->ptVICInfo->dwDenoiseBThrd = ((ptInfo->dwThr1&0x0000FFFF)<<16) |
											(ptInfo->dwThr0&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseBMaxThrd = ((ptInfo->dwDeNoiseThrMax1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMax2&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseBMinThrd = ((ptInfo->dwDeNoiseThrMin1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMin2&0x0000FFFF);

	ptInfo->ptVICInfo->dwDenoiseG1Thrd = ((ptInfo->dwThr1&0x0000FFFF)<<16) |
											(ptInfo->dwThr0&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseG1MaxThrd = ((ptInfo->dwDeNoiseThrMax1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMax2&0x0000FFFF);
	ptInfo->ptVICInfo->dwDenoiseG1MinThrd = ((ptInfo->dwDeNoiseThrMin1&0x0000FFFF)<<16) |
											(ptInfo->dwDeNoiseThrMin2&0x0000FFFF);

/* Version 5.1.0.0 modification, 2008.10.20 */
/* ======================================== */

/* Version 5.8.0.0 modification, 2009.04.08 */
	ptInfo->ptVICInfo->dwCMOSHorPenaltyThr = ((ptInfo->dwCFAEnhancedBayerHorPenThreshold&0x0003FFFF));
	ptInfo->ptVICInfo->dwCMOSVerPenaltyThr = ((ptInfo->dwCFAEnhancedBayerVerPenThreshold&0x0003FFFF));
/* ======================================== */

/* ======================================== */
/* Version 5.1.0.0 modification, 2008.10.20 */
/* ======================================== */
/* Version 5.2.0.0 modification, 2008.10.29 */
/* Version 5.2.0.2 modification, 2008.11.13 */
/* Version 5.6.0.0 modification, 2009.01.16 */
	//ptInfo->ptVICInfo->dwAFCtrl = (((ptInfo->bAFZoomTrackAFStart)<<27) |
	ptInfo->ptVICInfo->dwAFCtrl = ((0x1)<<9) |
								   ((ptInfo->bAFEn)<<8) |
									(ptInfo->dwAFFrameIntv&0x000000FF);

	dwAFGridWidth = ptInfo->dwAFWindowWidth / 8;
	dwAFGridHeight = ptInfo->dwAFWindowHeight / 8;

	dwAFWinStartX = ptInfo->dwAFWindowStartPosX;
	dwAFWinStartY = ptInfo->dwAFWindowStartPosY;

	ptInfo->dwAFWindowStartPosX = ((SDWORD)(dwAFWinStartX-dwAFGridWidth) <= 0) ? 0 : (dwAFWinStartX-dwAFGridWidth);
	ptInfo->dwAFWindowStartPosY = ((SDWORD)(dwAFWinStartY-dwAFGridHeight) <= 0) ? 0 : (dwAFWinStartY-dwAFGridHeight);

	dwAFShiftWidth = dwAFWinStartX - ptInfo->dwAFWindowStartPosX;
	dwAFShiftHeight = dwAFWinStartY - ptInfo->dwAFWindowStartPosY;

	ptInfo->dwAFWindowWidth = ((ptInfo->dwAFWindowWidth+dwAFShiftWidth) >= ptInfo->dwCapWidth) ? ptInfo->dwCapWidth : (ptInfo->dwAFWindowWidth+dwAFShiftWidth);
	ptInfo->dwAFWindowHeight = ((ptInfo->dwAFWindowHeight+dwAFShiftHeight) >= ptInfo->dwCapHeight) ? ptInfo->dwCapHeight : (ptInfo->dwAFWindowHeight+dwAFShiftHeight);

	ptInfo->ptVICInfo->dwAFWindowStartPosition = (((ptInfo->dwAFWindowStartPosX&0x00001FFF)<<16) |
												  (ptInfo->dwAFWindowStartPosY&0x00001FFF));
	ptInfo->ptVICInfo->dwAFWindowSize = (((ptInfo->dwAFWindowWidth&0x00001FFF)<<16) |
										 (ptInfo->dwAFWindowHeight&0x00001FFF));
/* ======================================== */

/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
/* Version 12.0.0.1 modification, 2012.10.15 */
	ptInfo->ptVICInfo->dwABGGainBond = (((ptInfo->dwAWBGainUpperBond>>8)&0xFF)<<24) |
										   (((ptInfo->dwAWBGainLowerBond>>8)&0xFF)<<16) |
										   ((ptInfo->dwAWBGainUpperBond&0xFF)<<8) |
										   (ptInfo->dwAWBGainLowerBond&0xFF);
/* ======================================== */

	ptInfo->ptVICInfo->dwAWBCtrl = ((ptInfo->byAWBMax&0x000000FF)<<24) |
									((ptInfo->byAWBMin&0x000000FF)<<16) |
/* Version 8.1.0.0 modification, 2011.06.21 */
/* Version 7.3.0.0 modification, 2009.11.24 */
									((((ptInfo->bAWBAfterCC)==1)?0:1)<<13) |	// AWB before CFA
									((ptInfo->bAWBAfterCC)<<12) |
/* ======================================== */
/* ======================================== */
									((ptInfo->bManualWBEn)<<11) |
									((ptInfo->byAWBMaxStep&0x0000000F)<<7) |
									((ptInfo->byAWBMinStep&0x0000000F)<<3) |
//									((ptInfo->bRESERVED0En)<<2) |
//									((ptInfo->bRESERVED1En)<<1) |
									(ptInfo->bAWBEn);
	ptInfo->ptVICInfo->dwAWBThr = ((ptInfo->byAWBCbUpperBond&0x000000FF)<<24) |
									 ((ptInfo->byAWBCbLowerBond&0x000000FF)<<16) |
									 ((ptInfo->byAWBCrUpperBond&0x000000FF)<<8) |
									 (ptInfo->byAWBCrLowerBond&0x000000FF);
/* Version 8.1.0.0 modification, 2011.06.21 */
	ptInfo->ptVICInfo->dwRESERVED0 = ptInfo->dwRESERVED0&0x00FFFFFF | 0x1;
/* ======================================== */
	ptInfo->ptVICInfo->dwRESERVED1 = ptInfo->dwRESERVED2&0x00FFFFFF;
	ptInfo->ptVICInfo->dwRESERVED2 = ptInfo->dwRESERVED0&0x00FFFFFF;
	ptInfo->ptVICInfo->dwRESERVED3 = ptInfo->dwRESERVED1&0x00FFFFFF;
/* Version 12.0.0.1 modification, 2012.10.15 */
	ptInfo->ptVICInfo->dwManualGain = ((ptInfo->dwAWBRGain&0x00001FFF)<<16) |
									  (ptInfo->dwAWBBGain&0x00001FFF);
/* ======================================== */

/* Version 8.1.0.0 modification, 2011.06.21 */
	ptInfo->ptVICInfo->dwCCCoeffR = ((ptInfo->dwCoeffROffset&0x0000000F)<<28) |
								    ((ptInfo->dwCoeffRR&0x000003FF)<<18) |
									((ptInfo->dwCoeffGR&0x000001FF)<<9) |
									 (ptInfo->dwCoeffBR&0x000001FF);
	ptInfo->ptVICInfo->dwCCCoeffG = ((ptInfo->dwCoeffGOffset&0x0000000F)<<28) |
									 ((ptInfo->dwCoeffRG&0x000001FF)<<19) |
									 ((ptInfo->dwCoeffGG&0x000003FF)<<9) |
									 (ptInfo->dwCoeffBG&0x000001FF);
	ptInfo->ptVICInfo->dwCCCoeffB = ((ptInfo->dwCoeffBOffset&0x0000000F)<<28) |
									 ((ptInfo->dwCoeffRB&0x000001FF)<<19) |
									 ((ptInfo->dwCoeffGB&0x000001FF)<<10) |
									 (ptInfo->dwCoeffBB&0x000003FF);
/* ======================================== */

	ptInfo->ptVICInfo->dwGammaTblAddr = (DWORD)ptInfo->pbyGammaTbl;
//	ptInfo->ptVICInfo->dwGammaTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyGammaTbl);

	ptInfo->ptVICInfo->dwYCbCrClip = (((ptInfo->byClipYUpper&0xFF)<<24) |
									  ((ptInfo->byClipYLower&0xFF)<<16) |
									  ((ptInfo->byClipCbCrUpper&0xFF)<<8) |
									  (ptInfo->byClipCbCrLower&0xFF));
	ptInfo->ptVICInfo->dwCMYGCtrl0 = ((ptInfo->dwCMYGEdgeThr&0x3FF)<<22) |
									  ((ptInfo->byCFAPatternVShift&0x3)<<2) |
								      (ptInfo->byCFAPatternHShift&0x3);
	ptInfo->ptVICInfo->dwCMYGCtrl1 = ((ptInfo->byCMYGAttenuateMethod&0x7)<<26) |
									  ((ptInfo->dwCMYGEdgeDiffThr&0x3FF)<<16) |
									  (ptInfo->dwCMYGChromaDiffThr&0xFFFF);
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */

	ptInfo->ptVICInfo->dwAWBWindowStart = (ptInfo->dwAEWBWindowLocationY<<16) | (ptInfo->dwAEWBWindowLocationX);
	ptInfo->ptVICInfo->dwAWBWindowRange = (((ptInfo->dwAEWBWindowHeight/32*32)&0x0000FFFF)<<16) |
		                  					((ptInfo->dwAEWBWindowWidth/32*32)&0x0000FFFF);
	ptInfo->ptVICInfo->dwBayerGain = 0x10101010;
	ptInfo->ptVICInfo->dwBayerClamp = ((ptInfo->dwBlackClamp&0x000000FF)<<24) |
										((ptInfo->dwBlackClamp&0x000000FF)<<16) |
										((ptInfo->dwBlackClamp&0x000000FF)<<8) |
										(ptInfo->dwBlackClamp&0x000000FF);

	ptInfo->ptVICInfo->dwStatBuffAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyStatBufUsrBaseAddr);

	tVICInitOptions.dwStatAEWBPhyBaseAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInitOptions->pbyStatAEWBBufUsrBaseAddr);
	tVICInitOptions.dwStatHistoPhyBaseAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInitOptions->pbyStatHistoBufUsrBaseAddr);
	tVICInitOptions.dwStatFocusPhyBaseAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInitOptions->pbyStatFocusBufUsrBaseAddr);

/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* Version 3.0.0.1 modification, 2007.12.26 */
/* Version 5.8.0.1 modification, 2009.04.15 */
/* Version 8.1.0.0 modification, 2011.06.21 */
	ptInfo->ptVICInfo->dwTMCtrl = ((ptInfo->dwGammaOverflowSlope&0x0000000F)<<28) |
								  ((ptInfo->byTMKeyValue)<<1) |
								   (ptInfo->bTMEn);
/* ======================================== */
/* ======================================== */

/* Version 4.0.0.2 modification, 2008.10.06 */
/* Version 5.10.0.5 modification, 2009.05.26 */
/* Version 6.2.0.0 modification, 2009.09.29 */
	ptChnInfo->dwCECtrl = (((ptInfo->byCEBrightness)<<1) |
						   (ptInfo->bCEEn));

	if (ptInfo->bCEAutoEn>0)
	{
		{
			ptInfo->dwCETblAddr = (DWORD) ptInfo->pdwCETbl3;
		}
	}
	else
	{
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
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
	ptInfo->ptVICInfo->dwPhotoLDCCtrl = ((0x0)<<2) |
/* Version 5.10.0.6 modification, 2009.06.03 */
										((0x0)<<1) |
/* ======================================== */
										(ptInfo->bPhotoLDCEn&0x00000001);
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

	// alocate frame buffer and initializing device driver
/* Version 6.0.0.3 modification, 2009.07.28 */
	tVICInitOptions.dwBufNum = ptInitOptions->dwBufNum;
/* ======================================== */
	tVICInitOptions.dwMask = 0x03;
/* Version 2.0.0.0 modification, 2006.06.21 */
	tVICInitOptions.dwPhyBaseAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInitOptions->pbyCapBufUsrBaseAddr);
/* ======================================== */
	tVICInitOptions.bOutFormat = (ptInitOptions->eOutPixelFormat==YUV420);
	tVICInitOptions.dwMaxFrameWidth = ptInfo->dwMaxFrameWidth;
	tVICInitOptions.dwMaxFrameHeight = ptInfo->dwMaxFrameHeight;
/* Version 3.1.0.0 modification, 2008.02.02 */
	tVICInitOptions.dwFrameRate = ptInfo->dwFrameRate;
/* ======================================== */

/* Version 12.0.0.3 modification, 2013.01.16 */
	sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_INITIAL, (DWORD)&tVICInitOptions);
	if (sdwResult != 0)
	{
		ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_INITIAL);
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
	}
/* ======================================== */

	printf("Before update MMR\n");
	// Update MMR
/* Version 5.10.0.7 modification, 2009.06.03 */

	ptInfo->ptVICInfo->adwChanCtrl[dwNum] = ptInfo->ptVICInfo->adwChanCtrl[dwNum] | 0x00000200;
	while ((ptInfo->ptVICInfo->adwChanCtrl[dwNum]&0x00000200) != 0);

	if ((ptInfo->eFormat==1)||(ptInfo->eFormat==3)||(ptInfo->eFormat==4))
	{
		ptInfo->ptVICInfo->adwChanCtrl[1] = ptInfo->ptVICInfo->adwChanCtrl[1] | 0x00000200;
		while ((ptInfo->ptVICInfo->adwChanCtrl[1]&0x00000200) != 0);
	}

/*
	ptInfo->ptVICInfo->dwCtrl = ptInfo->ptVICInfo->dwCtrl | 0x00000002;
	while ((ptInfo->ptVICInfo->dwCtrl&0x00000002) != 0);
*/
/* ======================================== */
	printf("After update MMR\n");

/* Version 3.0.0.0 modification, 2007.09.06 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
/* Version 5.10.0.7 modification, 2009.06.05 */
	if (ptInfo->eFormat==4)
	{
		VideoCap_LoadTblVIC(ptInfo);
	}
/* ========================================= */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

/* Version 3.1.0.6 modification, 2008.03.19 */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* Version 4.0.0.2 modification, 2008.10.06 */
/* Version 5.10.0.3 modification, 2009.05.15 */
/* Version 5.10.0.5 modification, 2009.05.26 */
/* Version 6.2.0.0 modification, 2009.09.29 */
	VideoCap_LoadCETblVIC(ptInfo, ptChnInfo);
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

/* Version 3.3.0.0 modification, 2008.09.19 */
	VideoCap_LoadPhotoLDCTblVIC(ptInfo);

/* ======================================== */

	return S_OK;
}
/* ======================================== */

/* Version 3.0.0.0 modification, 2007.09.03 */
/* =========================================================================================== */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
SCODE VideoCap_LoadTblVIC(TVideoCapInfo *ptInfo)
{
	ptInfo->ptVICInfo->dwGammaTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyGammaTbl);

	MemMgr_CacheCopyBack(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyGammaTbl, 256*3*16*sizeof(BYTE));

	ptInfo->ptVICInfo->dwCMOSCtrl = ptInfo->ptVICInfo->dwCMOSCtrl | 0x00000008;

	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_WAIT_TBL_COMPLETE);

	ptInfo->ptVICInfo->dwCMOSCtrl = ptInfo->ptVICInfo->dwCMOSCtrl & 0xFFFFFFF7;

	return S_OK;
}
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

/* Version 3.1.0.6 modification, 2008.03.19 */
/* =========================================================================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* Version 4.0.0.2 modification, 2008.10.06 */
/* Version 5.10.0.5 modification, 2009.05.26 */
/* Version 6.2.0.0 modification, 2009.09.29 */
SCODE VideoCap_LoadCETblVIC(TVideoCapInfo *ptInfo, TVPLVICChnInfo *ptChnInfo)
{
	ptChnInfo->dwCETblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD) ptInfo->dwCETblAddr);

	MemMgr_CacheCopyBack(ptInfo->hEDMCDev, (DWORD) ptInfo->dwCETblAddr, 256*sizeof(DWORD));

	ptChnInfo->dwCECtrl = ptChnInfo->dwCECtrl | 0x00000020;

	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_WAIT_CE_TBL_COMPLETE);

	return S_OK;
}
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

/* =========================================================================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
SCODE VideoCap_LoadPhotoLDCTblVIC(TVideoCapInfo *ptInfo)
{
	ptInfo->ptVICInfo->dwPhotoLDCTblAddr = MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->pwPhotoLDCTbl);

	DBPRINT1("Phy address:0x%08x !!\n", ptInfo->ptVICInfo->dwPhotoLDCTblAddr);
//memset(ptInfo->ptVICInfo->dwPhotoLDCTblAddr , 1024, ptInfo->dwPhotoLDCTblSize*sizeof(BYTE));
	MemMgr_CacheCopyBack(ptInfo->hEDMCDev, (DWORD)ptInfo->pwPhotoLDCTbl, ptInfo->dwPhotoLDCTblSize*sizeof(BYTE));

	ptInfo->ptVICInfo->dwPhotoLDCCtrl = ptInfo->ptVICInfo->dwPhotoLDCCtrl | 0x00000002;

	ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_WAIT_PHOTO_LDC_TBL_COMPLETE);

	ptInfo->ptVICInfo->dwPhotoLDCCtrl = ptInfo->ptVICInfo->dwPhotoLDCCtrl & 0xFFFFFFFD;

	return S_OK;
}
/* ======================================== */

/* ============================================================================================== */
SCODE VideoCap_CloseVIC(TVideoCapInfo *ptInfo)
{
/* Version 12.0.0.3 modification, 2013.01.16 */
	munmap((TVPLVICInfo *)ptInfo->ptVICInfo, sizeof(TVPLVICInfo));
/* ======================================== */
	if (close((int)(ptInfo->hVICDev)) != 0)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to close vpl_vic device driver !!\n");
		return _______VIDEOCAP_ERR_CLOSE_DEVICE_DRIVER;
/* ======================================== */
	}

	ptInfo->dwPrevTicks = 0;
/* Version 3.1.0.0 modification, 2008.02.02 */
/* ======================================== */
	ptInfo->dwFrameCount = 0;
	ptInfo->dwDiffResidue = 0;

	return S_OK;
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_StopVIC(TVideoCapInfo *ptInfo)
{
	DWORD dwNum = ptInfo->dwDeviceNum;

/* Version 3.1.0.4 modification, 2008.03.04 */
	while (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_STOP_CAPTURE) != S_OK);
/* ======================================== */
	return S_OK;
}
/* ======================================== */

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_CheckVIC(TVideoCapInfo *ptInfo)
{
	DWORD dwVersionNum;
/* Version 12.0.0.3 modification, 2013.01.16 */
	SDWORD sdwResult;

	sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_VERSION_NUMBER, &dwVersionNum);
	if (sdwResult != 0)
	{
		ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_VERSION_NUMBER);
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
	}
/* ======================================== */

	if (((dwVersionNum&0x00FF00FF)!=(VPL_VIC_VERSION&0x00FF00FF)) ||
		((dwVersionNum&0x0000FF00)<(VPL_VIC_VERSION&0x0000FF00)))
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Invalid vpl_vic device driver version %d.%d.%d.%d !!\n", dwVersionNum&0xFF, (dwVersionNum>>8)&0xFF, (dwVersionNum>>16)&0xFF, (dwVersionNum>>24)&0xFF);
		ERRPRINT("Please use vpl_vic device driver version %d.%d.%d.%d or compatible versions !!\n", VPL_VIC_VERSION&0xFF, (VPL_VIC_VERSION>>8)&0xFF, (VPL_VIC_VERSION>>16)&0xFF, (VPL_VIC_VERSION>>24)&0xFF);
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_INVALID_VERSION;
/* ======================================== */
	}
	else
	{
		printf("Open vpl_vic device driver version %d.%d.%d.%d successful !!\n", dwVersionNum&0xFF, (dwVersionNum>>8)&0xFF, (dwVersionNum>>16)&0xFF, (dwVersionNum>>24)&0xFF);
	}

	return S_OK;
}
/* ======================================== */

/* Version 3.2.0.0 modification, 2008.04.24 */
/* Version 3.3.0.1 modification, 2008.09.23 */
/* ======================================== */
/* ======================================== */

/* ============================================================================================== */
