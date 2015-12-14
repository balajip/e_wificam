/*

/* ============================================================================================== */
#ifndef __KERNEL__
    #define __KERNEL__
#endif //!__KERNEL__

#ifndef MODULE
    #define MODULE
#endif //!MODULE

/* ============================================================================================== */
#include "vpl_vic_locals.h"
#include <linux/delay.h>//eveworkaround
/* ============================================================================================== */
const CHAR VPL_VIC_ID[] = "$Version: "VPL_VIC_ID_VERSION"  (VPL_VIC) $";
static TVPLVICDevInfo *gptDevInfo = NULL;
static SDWORD gsdwMajor = 0;
static SDWORD gsdwBusFreq = 20000000;
static SDWORD gsdwSignalWaitTime = 4000;
static DWORD gdwBlankFrameLumThres = 1;
#ifdef __VIC_ENABLE_CALIBRATION__
static DWORD gdwCalThr = 200000;
#endif  //__VIC_ENABLE_CALIBRATION__
/* Sensor light frequency (50_Hz:1 or 60_Hz:2) */
static int video_sensor_frequency = VIDEO_SIGNAL_FREQUENCY_60HZ;
static int video_sensor_remainingline = 0;

/* SWAE enable */
static int bAEEn = 0;  
/* DC-Iris enable */
static int bIrisEn = 0; 
/* AE Numbers of AE operating every N frames (SWAEMode) */
static int dwAEopframes = 1;	
/* AE Target luminace (SWAEMode) */
static int dwTargetLuminance = 90;
/* AE Luminace offset (SWAEMode) */
static int dwTargetOffset = 20; // should be greater than FINE_TUNE_RANGE (5)
/* AE Shutter default width */
static int dwDefaultShutter = 120;
/* AE Gain default value (SWAEMode: related to DefaultGainIdx) */
static int dwDefaultGain = 1;
/* AE Shutter width lower bound (SWAEMode: related to MinShutterIdx), 0 stands for "lower bound don't care" */
static int dwMinShutter = 0;
/* AE Shutter width upper bound (SWAEMode: related to MaxShutterIdx), 1/2/4/8/15/30/60/120 */
static int dwMaxShutter = 1;
/* AE Analog gain lower bound (SWAEMode: MinGainIdx), 1-32 */
static int dwMinGain = 1;
/* AE Analog gain upper bound (SWAEMode: MaxGainIdx), 1-32 */
static int dwMaxGain = 64;
/* Version 11.0.0.4 modification, 2012.11.22 */
/* Get Bayer raw data enable */
static int bGetBayerRawEn = 0;  
/* ======================================== */

/* Identify sensor IIC setting timming must Sync Vsync Falling */
static int bIICSyncVsyncFallingEn = 0;

/* AF enable */
static int bAFEn = 0;
/* AF process every dwFrameIntv frame */
static int dwFrameIntv = 1;
/* AF noise threshold , 0 (smallest) ~ 7 (largest */
static int dwThresNoise = 6;
/* AF init step size 0 ~ 3 */
static int dwInitialStep = 2;
/* AF max return 0 ~ 3, the actual fine tune return number in library is (dwMaxReturnNum*5) */
static int dwMaxReturnNum = 0;
/* AF fine tune threshold */
static int dwFineTuneFinishThresh = 0;
/* AF restart upper diff  0 (smallest) ~ 3 (largest) */
static int dwRestartUpperDiff = 0;
/* AF restart lower diff  0 (smallest) ~ 3 (largest) */
static int dwRestartLowerDiff = 0;
/* AF default zoom ratio */
static int dwDefaultZoomRatio = 1;
/* Get AF zoom tracking size */
static int dwZtTbSize = 0;
static int dwCFAHVBlendEn = 0;
static int dwCFAHVAvgThrd = 0;
static int dwCFAHVBlendMaxThrd = 0;
static int dwCFAHVBlendMinThrd = 0;
static int dwCFAFCSEdgeThrd = 0;
static int dwCFAFCSMaxThrd = 0;
static int dwCFAFCSMinThrd = 0;
static int dwCFAFCSEn = 0;
static int dwCFAFCSExcludeEdge = 0;
static int dwCFAFCSAlpha = 0;
static int dwIrisCtlType = 0;
/* Version 11.0.0.1 modification, 2012.09.21 */
/* ========================================= */
/* VIC IO pad voltage level selection (2.5~3.3V:0, 1.8V:1) */
static int dwIOPadVoltageLevel = 0;

/* SW AWB enable */
static int bAWBEn = 0;
/* SW AWB initial mode */
static int dwAWBInitMode = 0;
/* SW AWB operating every N frames */
static int dwAWBopframes = 1;

/* ============================================================================================== */
MODULE_AUTHOR ("VN Inc.");
MODULE_LICENSE("GPL");

module_param (gsdwMajor, int, 0644);
MODULE_PARM_DESC (gsdwMajor, "Major number for VPL_VIC module");
module_param (gsdwBusFreq, int, 0644);
MODULE_PARM_DESC (gsdwBusFreq, "System bus frequency in Hz");
module_param (gsdwSignalWaitTime, int, 0644);
MODULE_PARM_DESC (gsdwSignalWaitTime, "Wait input video signal time in ms");
#ifdef __VIC_ENABLE_CALIBRATION__
module_param(gdwCalThr, ulong, 0644);
MODULE_PARM_DESC (gdwCalThr, "Threashold for driver time calibration in millisecond (default: 200ms)");
#endif  //__VIC_ENABLE_CALIBRATION__
module_param(gdwBlankFrameLumThres, ulong, 0644);
MODULE_PARM_DESC (gdwBlankFrameLumThres, "Threashold for the avg. luma in one frame to drop blank frames  (default: 1)");
module_param(video_sensor_frequency, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_frequency, "Sensor light frequency (set 1 for 50Hz or 2 for 60Hz)");

module_param(bAEEn, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (bAEEn, "SWAE enable");
module_param(bIrisEn, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (bIrisEn, "DC-Iris enable");
module_param(dwAEopframes, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (dwAEopframes, "AE Numbers of AE operating every N frames (SWAEMode)");
module_param(dwTargetLuminance, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwTargetLuminance, "AE Target luminace (SWAEMode)");
module_param(dwTargetOffset, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwTargetOffset, "AE Luminace offset (SWAEMode)");
module_param(dwDefaultShutter, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwDefaultShutter, "AE Shutter default width");
module_param(dwDefaultGain, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwDefaultGain, "AE Gain default value (SWAEMode: related to DefaultGainIdx)");
module_param(dwMinShutter, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwMinShutter, "AE Shutter width lower bound (SWAEMode: related to MinShutterIdx), 0 stands for [lower bound don't care]");
module_param(dwMaxShutter, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwMaxShutter, "AE Shutter width upper bound (SWAEMode: related to MaxShutterIdx), 1/2/4/8/15/30/60/120");
module_param(dwMinGain, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwMinGain, "AE Analog gain lower bound (SWAEMode: MinGainIdx), 1-32");
module_param(dwMaxGain, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwMaxGain, "AE Analog gain upper bound (SWAEMode: MaxGainIdx), 1-32");
module_param(bIICSyncVsyncFallingEn, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (bIICSyncVsyncFallingEn, "Identify sensor IIC setting timming must Sync Vsync Falling");
/* Version 11.0.0.4 modification, 2012.11.22 */
module_param(bGetBayerRawEn, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (bGetBayerRawEn, "Get Bayer raw data enable/disable");
/* ======================================== */

module_param(bAFEn, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (bAFEn, " AF enable/disable");
module_param(dwFrameIntv, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwFrameIntv, " AF process every dwFrameIntv frame");
module_param(dwThresNoise, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwThresNoise, " AF noise threshold , 0 (smallest) ~ 7 (largest)");
module_param(dwInitialStep, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwInitialStep, " AF init step size 0 ~ 3");
module_param(dwMaxReturnNum, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwMaxReturnNum, " AF max return 0 ~ 3, the actual fine tune return number in library is (dwMaxReturnNum*5)");
module_param(dwFineTuneFinishThresh, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwFineTuneFinishThresh, " AF fine tune threshold");
module_param(dwRestartUpperDiff, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwRestartUpperDiff, " AF restart upper diff  0 (smallest) ~ 3 (largest)");
module_param(dwRestartLowerDiff, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwRestartLowerDiff, " AF restart lower diff  0 (smallest) ~ 3 (largest)");
module_param(dwDefaultZoomRatio, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwDefaultZoomRatio, " AF default zoom ratio  1 (smallest) ~ 10 (largest)");

module_param(dwCFAHVBlendEn, int, S_IRUGO|S_IWUSR);
module_param(dwCFAHVAvgThrd, int, S_IRUGO|S_IWUSR);
module_param(dwCFAHVBlendMaxThrd, int, S_IRUGO|S_IWUSR);
module_param(dwCFAHVBlendMinThrd, int, S_IRUGO|S_IWUSR);
module_param(dwCFAFCSEdgeThrd, int, S_IRUGO|S_IWUSR);
module_param(dwCFAFCSMaxThrd, int, S_IRUGO|S_IWUSR);
module_param(dwCFAFCSMinThrd, int, S_IRUGO|S_IWUSR);
module_param(dwCFAFCSEn, int, S_IRUGO|S_IWUSR);
module_param(dwCFAFCSExcludeEdge, int, S_IRUGO|S_IWUSR);
module_param(dwCFAFCSAlpha, int, S_IRUGO|S_IWUSR);

module_param(dwIrisCtlType, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwIrisCtlType, " Auto iris control type  0 (DAC for DC iris) ~ 1 (PWM for DC iris) ~ 2(PIris)");
/* Version 11.0.0.1 modification, 2012.09.21 */
/* ========================================= */
module_param(dwIOPadVoltageLevel, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (dwIOPadVoltageLevel, "IO pad voltage level selection (set 0 for 2.5~3.3V or 1 for 1.8V)");

module_param(bAWBEn, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (bAWBEn, "SW AWB enable");
module_param(dwAWBInitMode, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (dwAWBInitMode, "SW AWB initial mode");
module_param(dwAWBopframes, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (dwAWBopframes, "SW AWB operating every N frames");

/* sensor device ops */
extern struct video_sensor_device sensor_dev_ops;
static struct video_sensor_device *sensor_dev = &sensor_dev_ops;

/* Version 11.0.0.1 modification, 2012.09.21 */
/* auto-exposure driver ops */
static TAutoExposureModule *ae_dev = NULL;

/* DC-Iris driver ops */
static TIrisMotorDevice *iris_motor_dev = NULL;

/* auto-focus driver ops */
static TAutoFocusModule *af_dev = NULL;

/* auto-focus motor driver ops */
static TAutoFocusMotorDevice *af_motor_dev = NULL;

/* auto-white balance driver ops */
static TAutoWhiteBalanceModule *awb_dev = NULL;
/* ========================================= */
void VIC_AEWBOneframeWQ(unsigned long);
void VIC_AutoFocusOneframeWQ(unsigned long);


/* ============================================================================================== */
static void VIC_Disable(void)
{
    writel(readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0xFFFFFFFE, &gptDevInfo->ptMMRInfo->dwCtrl);
    PDEBUG("VIC is disabled !!\n");
}

/* ============================================================================================== */
static void VIC_Start(void)
{
    writel(readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0xFFFFFFFE, &gptDevInfo->ptMMRInfo->dwCtrl);
    writel(readl(&gptDevInfo->ptMMRInfo->dwCtrl)|0x00000001, &gptDevInfo->ptMMRInfo->dwCtrl);
    PDEBUG("VIC is Start !!\n");
}

/* ============================================================================================== */
static void VIC_DisableOutput(DWORD dwNum)
{
    writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0xFFFFFEFF, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
    PDEBUG("Channel %lu is disabled !!\n", dwNum);
}

/* ============================================================================================== */
static void Intr_Clear(DWORD dwNum)
{
    writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0xFFFFFF80, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
    PDEBUG("Interrupt of channel %lu is cleared !!\n", dwNum);
}

/* ============================================================================================== */
static void Intr_Disable(DWORD dwNum)
{
    writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0xFFFFFF7F, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
    PDEBUG("Interrupt of channel %lu disabled !!\n", dwNum);
}

/* =========================================================================== */
static void VIC_Initial(TVPLVICInitOptions *ptInitOptions, DWORD dwNum, TVPLVICObjInfo *ptObjInfo)
{
    TVideoCapBufInfo *ptBufInfo;
    TVPLVICChnInfo *ptChnInfo = (TVPLVICChnInfo *)&gptDevInfo->ptMMRInfo->atChnInfo[dwNum];
    TVPLVICDevChnInfo *ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
    DWORD dwYFrameSize, dwCFrameSize, dwBufSize, dwPhyBaseAddr;
    DWORD dwData;
    DWORD i;
    DWORD dwCapHeight;
    DWORD dwStatBuffAddr, dwBayerBitWidth;
#ifdef __VIC_ENABLE_CALIBRATION__
    struct timeval tv;
#endif //__VIC_ENABLE_CALIBRATION__
    DWORD *pdwSYSCVirtAddr;

    pdwSYSCVirtAddr = (DWORD *)IO_ADDRESS(VPL_SYSC_MMR_BASE);
    writel(((readl(pdwSYSCVirtAddr+17)&0xFF1FFFFF)|0x0018000C), (pdwSYSCVirtAddr+17));

    if (((gptDevInfo->ptMMRInfo->adwDevCtrl[0]>>5)&0x3)==0x2) {		// mux 4 channels    
        //VIC_DEV_0_CLK_CFG : quarter speed
        writel(((readl(pdwSYSCVirtAddr+11)&0xFCFFFFFF)|0x02000000), (pdwSYSCVirtAddr+11));	
    }
    else {		// single channel
        //VIC_DEV_0_CLK_CFG : full speed
        writel((readl(pdwSYSCVirtAddr+11)&0xFCFFFFFF), (pdwSYSCVirtAddr+11));
    }

    if (((gptDevInfo->ptMMRInfo->adwDevCtrl[1]>>5)&0x3)==0x2) {		// mux 4 channels
        //VIC_DEV_1_CLK_CFG : quarter speed
        writel(((readl(pdwSYSCVirtAddr+11)&0xF3FFFFFF)|0x08000000), (pdwSYSCVirtAddr+11));
    }
    else {		// single channel    
        //VIC_DEV_1_CLK_CFG : full speed
        writel((readl(pdwSYSCVirtAddr+11)&0xF3FFFFFF), (pdwSYSCVirtAddr+11));
    }

/* Version 11.0.0.3 modification, 2012.11.02 */
    if (dwIOPadVoltageLevel == 1) {    // IO pad voltage level is 1.8V 
        writel(((readl(pdwSYSCVirtAddr+18)&0xFFFFFFF7) | 0x00000008), (pdwSYSCVirtAddr+18));	        
    }
    else {    // IO pad voltage level is 2.5V~3.3V 
        writel((readl(pdwSYSCVirtAddr+18)&0xFFFFFFF7), (pdwSYSCVirtAddr+18));	                
    }		
/* ======================================== */

    // set BayerCFAHVAvgThrd
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAHVAvgThrd);
    dwData &= ~(0x00000001<<31);  // clear CFAHVBlendEn
    dwData |= (dwCFAHVBlendEn&0x00000001)<<31;  // set CFAHVBlendEn
    dwData &= ~(0x0003FFFF<<0); // clear CFAHVAvgThrd
    dwData |= (dwCFAHVAvgThrd&0x0003FFFF)<<0;  // set CFAHVAvgThrd
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAHVAvgThrd);    

    // set BayerCFAHVBlendMaxThrd
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMaxThrd);    
    dwData &= ~(0x0003FFFF<<0); // clear CFAHVBlendMaxThrd
    dwData |= (dwCFAHVBlendMaxThrd&0x0003FFFF)<<0;  // set CFAHVBlendMaxThrd
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMaxThrd);    

    // set BayerCFAHVBlendMinThrd
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMinThrd);    
    dwData &= ~(0x0003FFFF<<0); // clear CFAHVBlendMinThrd
    dwData |= (dwCFAHVBlendMinThrd&0x0003FFFF)<<0;  // set CFAHVBlendMinThrd
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMinThrd);    

    // set BayerCFAFCSEdgeThrd
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSEdgeThrd);    
    dwData &= ~(0x0003FFFF<<0); // clear CFAFCSEdgeThrd
    dwData |= (dwCFAFCSEdgeThrd&0x0003FFFF)<<0;  // set CFAFCSEdgeThrd
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSEdgeThrd);    

    // set BayerCFAFCSMaxThrd
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSMaxThrd);    
    dwData &= ~(0x0003FFFF<<0); // clear CFAFCSMaxThrd
    dwData |= (dwCFAFCSMaxThrd&0x0003FFFF)<<0;  // set CFAFCSMaxThrd
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSMaxThrd);    
	
    // set BayerCFAFCSMinThrd
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSMinThrd);    
    dwData &= ~(0x0003FFFF<<0); // clear CFAFCSMinThrd
    dwData |= (dwCFAFCSMinThrd&0x0003FFFF)<<0;  // set CFAFCSMinThrd
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSMinThrd);        

    // set BayerCFAFCSCtrl
    dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSCtrl);
    dwData &= ~(0x00000001<<10);  // clear CFAFCSEn
    dwData |= (dwCFAFCSEn&0x00000001)<<10;  // set CFAFCSEn
    dwData &= ~(0x00000001<<9); // clear CFAFCSExcludeEdge
    dwData |= (dwCFAFCSExcludeEdge&0x00000001)<<9;  // set CFAFCSExcludeEdge
    dwData &= ~(0x000001FF<<0); // clear CFAFCSAlpha
    dwData |= (dwCFAFCSAlpha&0x000001FF)<<0;  // set CFAFCSAlpha
    writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSCtrl);   

    ptDevChnInfo->dwMaxFrameWidth = ptInitOptions->dwMaxFrameWidth;
    ptDevChnInfo->dwMaxFrameHeight = ptInitOptions->dwMaxFrameHeight;
    //ptDevChnInfo->bInterlace = ptInitOptions->bInterlace;
    ptDevChnInfo->bOutFormat = ptInitOptions->bOutFormat;
    ptDevChnInfo->dwPhyBaseAddr = ptInitOptions->dwPhyBaseAddr;
    ptDevChnInfo->dwBufNum = ptInitOptions->dwBufNum;
    ptDevChnInfo->ptBufInfo = kmalloc(ptDevChnInfo->dwBufNum*sizeof(TVideoCapBufInfo), GFP_KERNEL);
    ptDevChnInfo->dwMask = ptInitOptions->dwMask;
    ptDevChnInfo->dwInWidth = readl(&ptChnInfo->dwInSize) & 0x0000FFFF;

    dwYFrameSize = ptDevChnInfo->dwMaxFrameWidth * ptDevChnInfo->dwMaxFrameHeight;
    dwCFrameSize = dwYFrameSize >> (1+ptDevChnInfo->bOutFormat);
    dwBufSize = dwYFrameSize + (2*dwCFrameSize);
    dwPhyBaseAddr = ptDevChnInfo->dwPhyBaseAddr;

    for (i=0; i<ptDevChnInfo->dwBufNum; i++, dwPhyBaseAddr+=dwBufSize)
    {
        ptBufInfo = &ptDevChnInfo->ptBufInfo[i];
        ptBufInfo->dwBufY = dwPhyBaseAddr;
        ptBufInfo->dwBufCb= ptBufInfo->dwBufY + dwYFrameSize;
        ptBufInfo->dwBufCr = ptBufInfo->dwBufCb + dwCFrameSize;
        ptBufInfo->dwFrameCount = 0;
        ptBufInfo->bReadyForProcess = FALSE;
        ptBufInfo->bReadyForSlvProcess = FALSE;
        ptBufInfo->bReadyForCapture = TRUE;
        ptBufInfo->dwSecond = 0;
        ptBufInfo->bTimeStampCal = FALSE;
        ptBufInfo->dwErrACK = 0;
        ptBufInfo->dwInUseNum = 0;
        ptBufInfo->dwOutWidth = readl(&ptChnInfo->dwCapH) >> 16;
        ptBufInfo->dwOutHeight = readl(&ptChnInfo->dwCapV) >> 16;
        ptBufInfo->dwOutStride = readl(&ptChnInfo->dwStride) & 0x0000FFFF;
        ptBufInfo->dwIndex = i;
        ptBufInfo->bFlip = FALSE;
        ptDevChnInfo->dwShutter = dwDefaultShutter;
        ptDevChnInfo->dwGain = dwDefaultGain;
        ptDevChnInfo->dwZoomRatio = dwDefaultZoomRatio;
        ptDevChnInfo->bAEStable = FALSE;
        ptDevChnInfo->dwCurrentEV = 0;

        ptDevChnInfo->tVPLVICISPParam.bAECtrl = FALSE;
        ptDevChnInfo->tVPLVICISPParam.bCCCtrl = FALSE;
        ptDevChnInfo->tVPLVICISPParam.bCECtrl = FALSE;
        ptDevChnInfo->tVPLVICISPParam.bAntialiasingCtrl = FALSE;
        ptDevChnInfo->tVPLVICISPParam.bCMOSCtrl = FALSE;
        ptDevChnInfo->tVPLVICISPParam.bSBCCtrl = FALSE;
        ptDevChnInfo->tVPLVICISPParam.bTMCtrl = FALSE;

        if (i==(ptDevChnInfo->dwBufNum-1))
        {
            ptBufInfo->ptNextBuf = ptDevChnInfo->ptBufInfo;
        }
        else
        {
            ptBufInfo->ptNextBuf = &ptDevChnInfo->ptBufInfo[(i+1)];
        }

        if (i==0)
        {
            ptBufInfo->ptPreBuf = &ptDevChnInfo->ptBufInfo[(ptDevChnInfo->dwBufNum-1)];
        }
        else
        {
            ptBufInfo->ptPreBuf = &ptDevChnInfo->ptBufInfo[(i-1)];
        }
        PDEBUG("Buf %lu, dwBufY: %08lX\n", i, ptBufInfo->dwBufY);
        PDEBUG("Buf %lu, dwBufCb: %08lX\n", i, ptBufInfo->dwBufCb);
        PDEBUG("Buf %lu, dwBufCr: %08lX\n", i, ptBufInfo->dwBufCr);
    }


    ptDevChnInfo->ptProcessBuf = &ptDevChnInfo->ptBufInfo[0];
    for (i=0; i<MAX_VIC_OPEN_NUM_DEFAULT; i++)
    {
        ptDevChnInfo->abIntFlag[i] = FALSE;
    }
    ptDevChnInfo->dwFrameCountAll = 0;
    ptDevChnInfo->ptCaptureBuf = &ptDevChnInfo->ptBufInfo[0];
    ptDevChnInfo->dwErrACK = 0;
    ptDevChnInfo->bSetOption = FALSE;
    ptDevChnInfo->bStopCapture = FALSE;
    ptDevChnInfo->dwFrameNumCnt = 0;
    ptDevChnInfo->dwRealFrameNum = 0;
    ptDevChnInfo->dwFrameNum = 0;
    ptDevChnInfo->dwFrameNumDelay = 0;
    ptDevChnInfo->ptMasterBuf = NULL;
    ptDevChnInfo->bSetSize = FALSE;

    ptObjInfo->aptProcBuf = kmalloc (ptDevChnInfo->dwBufNum*sizeof(TVideoCapBufInfo *),  GFP_KERNEL);
    for (i=0; i<ptDevChnInfo->dwBufNum; i++)
    {
		ptObjInfo->aptProcBuf[i] = NULL;
	}
#ifndef __USE_LAST_FRAME__
	ptObjInfo->aptProcBuf[0] = ptDevChnInfo->ptProcessBuf;
#endif
    ptDevChnInfo->bHalfSizedOutput = FALSE;
    ptDevChnInfo->bHalfSize = FALSE;
    ptDevChnInfo->dwInputInterval = 0;
    ptDevChnInfo->dwCapHeight = readl(&ptChnInfo->dwCapV) >> 16;
    ptDevChnInfo->dwFrameRate = ptInitOptions->dwFrameRate;
    ptDevChnInfo->dwPrevTicks = 0;
    ptDevChnInfo->bStartOutput = FALSE;
    ptDevChnInfo->bFlip = FALSE;
    ptDevChnInfo->dwDiffResidue = 0;
    if (ptDevChnInfo->dwFrameRate != 0)
    {
        ptDevChnInfo->dwInterval = 1000000 / ptDevChnInfo->dwFrameRate;
        ptDevChnInfo->dwIntervalResidue = 1000000 - ((1000000/ptDevChnInfo->dwFrameRate)*ptDevChnInfo->dwFrameRate);
    }
    ptDevChnInfo->dwFrameCount = 0;
    ptBufInfo = &ptDevChnInfo->ptBufInfo[0];
    writel(ptBufInfo->dwBufY, &ptChnInfo->dwYAddr0);
    writel(ptBufInfo->dwBufCb, &ptChnInfo->dwCbAddr0);
    writel(ptBufInfo->dwBufCr, &ptChnInfo->dwCrAddr0);
    writel(ptBufInfo->ptNextBuf->dwBufY, &ptChnInfo->dwYAddr1);
    writel(ptBufInfo->ptNextBuf->dwBufCb, &ptChnInfo->dwCbAddr1);
    writel(ptBufInfo->ptNextBuf->dwBufCr, &ptChnInfo->dwCrAddr1);
    ptDevChnInfo->bStateUpdate = TRUE;
    if (sensor_dev->get_remaining_line_num) {
        dwCapHeight = (readl(&ptChnInfo->dwCapV)&0xFFFF0000)>>16;
        dwCapHeight += sensor_dev->get_remaining_line_num();
        video_sensor_remainingline = sensor_dev->get_remaining_line_num();
        writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(dwCapHeight<<16), &ptChnInfo->dwCapV);
    }

    ptDevChnInfo->eFormat = ((readl(&gptDevInfo->ptMMRInfo->dwCtrl)>>2)&0x07);
/* Version 11.0.0.4 modification, 2012.11.22 */
    if ((ptDevChnInfo->eFormat==VIDEO_SIGNAL_FORMAT_BAYER_PATTERN) || bGetBayerRawEn)
/* ======================================== */
    {
        dwStatBuffAddr = readl(&gptDevInfo->ptMMRInfo->dwStatBuffAddr);
        gptDevInfo->pdwStatBuff = (DWORD *)ioremap((int)dwStatBuffAddr, (sizeof(DWORD)*1088));
    
        gptDevInfo->pdwStatAEWBBuf = (DWORD *)ioremap((int)ptInitOptions->dwStatAEWBPhyBaseAddr, (sizeof(DWORD)*720));
        gptDevInfo->pdwStatHistoBuf = (DWORD *)ioremap((int)ptInitOptions->dwStatHistoPhyBaseAddr, (sizeof(DWORD)*192));
        gptDevInfo->pdwStatFocusBuf = (DWORD *)ioremap((int)ptInitOptions->dwStatFocusPhyBaseAddr, (sizeof(DWORD)*98));
/* Version 11.0.0.3 modification, 2012.11.02 */
        gptDevInfo->dwStatAEWBPhyBaseAddr = ptInitOptions->dwStatAEWBPhyBaseAddr;
        gptDevInfo->dwStatHistoPhyBaseAddr = ptInitOptions->dwStatHistoPhyBaseAddr;
        gptDevInfo->dwStatFocusPhyBaseAddr = ptInitOptions->dwStatFocusPhyBaseAddr;
/* ======================================== */
    }

    if (bAFEn) 
    {
        writel((readl(&gptDevInfo->ptMMRInfo->dwAFCtrl)|((1&0x1) << 8)), &gptDevInfo->ptMMRInfo->dwAFCtrl);
        writel(((readl(&gptDevInfo->ptMMRInfo->dwAFCtrl)&0xFFFFFFF0)|(dwFrameIntv&0x0F)), &gptDevInfo->ptMMRInfo->dwAFCtrl);
    }
    
    if (bAWBEn)
    {
        writel((readl(&gptDevInfo->ptMMRInfo->dwAWBCtrl)&0xFFFFF7FF)|((1)<<11), &gptDevInfo->ptMMRInfo->dwAWBCtrl);
        writel((readl(&gptDevInfo->ptMMRInfo->dwManualGain)&0xF0000000)|((1024&0x3FF)<<10)|(1024&0x3FF), &gptDevInfo->ptMMRInfo->dwManualGain);

/* Version 12.0.0.1 modification, 2013.06.21 */
        dwBayerBitWidth = ((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)>>16)&0x1F);
        if (awb_dev->set_bayer_bitwidth)	
        	awb_dev->set_bayer_bitwidth(dwBayerBitWidth);
/* ======================================== */
    }
    
    if (bAEEn)
    {
        dwBayerBitWidth = ((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)>>16)&0x1F);
        if (ae_dev->set_bayer_bitwidth)	
        	ae_dev->set_bayer_bitwidth(dwBayerBitWidth);
    }

#ifdef __VIC_ENABLE_CALIBRATION__
    do_gettimeofday(&tv);
    ptDevChnInfo->dwBaseSecond = tv.tv_sec;
    ptDevChnInfo->dwBaseMicroSecond = tv.tv_usec;
    PDEBUG("Base Time: %ld:%ld\n", tv.tv_sec, tv.tv_usec);
#endif //__VIC_ENABLE_CALIBRATION__
}

/* ============================================================================================== */
static SCODE VIC_Get_Buf(TVPLVICState *ptState, DWORD dwNum, TVPLVICObjInfo *ptObjInfo)
{
    TVideoCapBufInfo *ptBufInfo;
    TVPLVICDevChnInfo *ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
    BOOL bIsMaster = ptObjInfo->bIsMaster;

    if (ptObjInfo->bInitialized == FALSE)
    {
        if (bIsMaster==TRUE)
        {
            ptObjInfo->aptProcBuf[ptObjInfo->dwBufNum] = ptDevChnInfo->ptProcessBuf;
			ptObjInfo->ptCurrGetBuf = ptDevChnInfo->ptProcessBuf;
            ptDevChnInfo->ptMasterBuf = ptDevChnInfo->ptProcessBuf;
        }
        else
        {
            if (ptDevChnInfo->ptMasterBuf==NULL)
            {
                PDEBUG("VIC_Get_Buf fail master full!!\n");
                return S_FAIL;
            }
            ptObjInfo->aptProcBuf[ptObjInfo->dwBufNum] = ptDevChnInfo->ptMasterBuf->ptPreBuf;
			ptObjInfo->ptCurrGetBuf =  ptDevChnInfo->ptMasterBuf->ptPreBuf;
        }
        ptObjInfo->bInitialized = TRUE;
    }
    else
    {
#ifndef __USE_LAST_FRAME__
        ptObjInfo->aptProcBuf[ptObjInfo->dwBufNum] = ptObjInfo->ptCurrGetBuf->ptNextBuf;
#else
        ptObjInfo->aptProcBuf[ptObjInfo->dwBufNum] = ptDevChnInfo->ptProcessBuf;
#endif
    }

    ptBufInfo = ptObjInfo->aptProcBuf[ptObjInfo->dwBufNum];
    //ptObjInfo->ptCurrGetBuf = ptBufInfo;
#ifdef _DEBUG
//#if 1
    DWORD dwTmp, dwTmp1;
    dwTmp = (gptDevInfo->atDevChnInfo[dwNum].ptCaptureBuf->dwIndex<<28) |
            (ptBufInfo->dwIndex<<24) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[5].bReadyForSlvProcess<<21) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[4].bReadyForSlvProcess<<20) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[3].bReadyForSlvProcess<<19) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[2].bReadyForSlvProcess<<18) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[1].bReadyForSlvProcess<<17) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[0].bReadyForSlvProcess<<16) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[5].bReadyForProcess<<13) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[4].bReadyForProcess<<12) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[3].bReadyForProcess<<11) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[2].bReadyForProcess<<10) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[1].bReadyForProcess<<9) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[0].bReadyForProcess<<8) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[5].bReadyForCapture<<5) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[4].bReadyForCapture<<4) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[3].bReadyForCapture<<3) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[2].bReadyForCapture<<2) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[1].bReadyForCapture<<1) |
            (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[0].bReadyForCapture);
     dwTmp1 = (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[5].dwInUseNum<<20) |
               (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[4].dwInUseNum<<16) |
               (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[3].dwInUseNum<<12) |
               (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[2].dwInUseNum<<8) |
               (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[1].dwInUseNum<<4) |
               (gptDevInfo->atDevChnInfo[dwNum].ptBufInfo[0].dwInUseNum);
    if (bIsMaster)
    {
        printk("Master <Ch %lu>: %08lX %08lX\n", dwNum, dwTmp, dwTmp1);
    }
    else
    {
        printk("Slave <Ch %lu>: %08lX %08lX\n", dwNum, dwTmp, dwTmp1);
    }
#endif //_DEBUG

    ptState->dwIndex = ptBufInfo->dwIndex;
    ptState->dwFrameCount = ptBufInfo->dwFrameCount;
    ptState->dwSecond = ptBufInfo->dwSecond;
    ptState->dwMilliSecond = ptBufInfo->dwMicroSecond / 1000;
    ptState->dwMicroSecond = ptBufInfo->dwMicroSecond % 1000;
    ptState->dwOutWidth = ptBufInfo->dwOutWidth;
    ptState->dwOutHeight = ptBufInfo->dwOutHeight;
    ptState->dwOutStride = ptBufInfo->dwOutStride & 0x0000FFFF;
    ptState->dwYFrameAddr = ptBufInfo->dwBufY;
    ptState->dwCbFrameAddr = ptBufInfo->dwBufCb;
    ptState->dwCrFrameAddr = ptBufInfo->dwBufCr;

    if ((ptBufInfo->bFlip) && (sensor_dev->get_remaining_line_num)) {
        ptState->dwYFrameAddr += (video_sensor_remainingline*(ptState->dwOutStride));
        ptState->dwCbFrameAddr += ((video_sensor_remainingline*(ptState->dwOutStride))>>2);
        ptState->dwCrFrameAddr += ((video_sensor_remainingline*(ptState->dwOutStride))>>2);		
    }		
    ptState->dwInWidth = ptBufInfo->dwInWidth;
    ptState->bTimeStampCal = ptBufInfo->bTimeStampCal;

    ptState->dwAFFocusValueHigh = ptBufInfo->dwAFFocusValueHigh;
    ptState->dwAFFocusValueLow = ptBufInfo->dwAFFocusValueLow;
    ptState->dwAFCurrentStatus = ptBufInfo->dwAFCurrentStatus;
    ptState->qwAWBRedSum = ptBufInfo->qwAWBRedSum;
    ptState->qwAWBGreenSum = ptBufInfo->qwAWBGreenSum;
    ptState->qwAWBBlueSum = ptBufInfo->qwAWBBlueSum;
    ptState->dwAWBGain = ptBufInfo->dwAWBGain;
    ptState->dwShutter = ptDevChnInfo->dwShutter;
    ptState->dwGain = ptDevChnInfo->dwGain;
    ptState->dwZoomRatio = ptDevChnInfo->dwZoomRatio;
    ptState->bAEStable = ptDevChnInfo->bAEStable;

    if (((ptBufInfo->bReadyForProcess==FALSE)&&(bIsMaster==TRUE)) ||
        ((ptBufInfo->bReadyForSlvProcess==FALSE)&&(bIsMaster==FALSE)) ||
        ((ptDevChnInfo->dwErrACK&0x04)!=0))
    {
        ptState->dwErrACK = ptDevChnInfo->dwErrACK;
        ptDevChnInfo->abIntFlag[ptObjInfo->dwProcIndex] = FALSE;
		PDEBUG("Buffer is N/A or erroneous, idx:%lu\n", ptState->dwIndex);
        return S_FAIL;
    }
    if (ptObjInfo->dwPreFrameCount == ptBufInfo->dwFrameCount)
    {
        ptDevChnInfo->abIntFlag[ptObjInfo->dwProcIndex] = FALSE;
    }
    ptObjInfo->dwPreFrameCount = ptBufInfo->dwFrameCount;
    if (ptBufInfo->dwInUseNum < MAX_VIC_OPEN_NUM_DEFAULT)
    {
        ptBufInfo->dwInUseNum++;
    }
    else
    {
        printk("VIC_Get_Buf fail use num full!!\n");
        return S_FAIL;
    }
    PDEBUG("VIC_Get_Buf success %d, Idx: %lu (%lu)\n", bIsMaster, ptBufInfo->dwIndex, ptObjInfo->dwPreFrameCount);
    ptState->dwErrACK = ptBufInfo->dwErrACK;
    ptObjInfo->dwBufNum = ptObjInfo->dwBufNum + 1;
    ptObjInfo->ptCurrGetBuf = ptBufInfo;
    /*// once error occurs
    if ((ptState->dwErrACK&0x10) != 0x00)
    {
        PDEBUG("Get buffer failed! (0x%08lx)\n", ptState->dwErrACK);
        return S_FAIL;
    }*/
    if (bIsMaster==TRUE)
    {
        ptBufInfo->bReadyForCapture = FALSE;
        ptDevChnInfo->ptMasterBuf = ptBufInfo;
    }

    return S_OK;
}

/* ============================================================================================== */
static void VIC_Release_Buf(DWORD dwIndex, DWORD dwNum, TVPLVICObjInfo *ptObjInfo)
{
    TVPLVICDevChnInfo *ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
    TVideoCapBufInfo *ptBufInfo = &ptDevChnInfo->ptBufInfo[dwIndex];
    BOOL bIsMaster = ptObjInfo->bIsMaster;
    BOOL bReleaseBuf = FALSE;
    DWORD i, j;
    //wake up all slave process
	PDEBUG("Release_Buf %d, Idx:%d\n", bIsMaster, dwIndex);
    for (i=0; i<ptObjInfo->dwBufNum; i++)
    {
        if (ptObjInfo->aptProcBuf[i]==ptBufInfo)
        {
            for (j=i; j<ptObjInfo->dwBufNum; j++)
            {
                ptObjInfo->aptProcBuf[j] = ptObjInfo->aptProcBuf[j+1];
            }
            ptObjInfo->dwBufNum--;
            bReleaseBuf = TRUE;
            break;
        }
    }
    if (bReleaseBuf == TRUE)
    {
        if (bIsMaster == TRUE)
        {
            ptBufInfo->bReadyForSlvProcess = TRUE;
            ptBufInfo->bReadyForCapture = TRUE;
            for (i=1; i<MAX_VIC_OPEN_NUM_DEFAULT; i++)
            {
                ptDevChnInfo->abIntFlag[i] = TRUE;
            }
            wake_up_interruptible(&ptDevChnInfo->wq);

#ifndef __USE_LAST_FRAME__
            ptDevChnInfo->ptMasterBuf = ptDevChnInfo->ptMasterBuf->ptNextBuf;
#endif
        }

        if (ptBufInfo->dwInUseNum > 0)
        {
            ptBufInfo->dwInUseNum--;
        }
    }

}

int VIC_GetDevNum(DWORD* dwNum)
{
	int i;
	DWORD dwVICMmrCtrl;

	for (i=0; i<VPL_VIC_CHN_NUM; i++) {
		dwVICMmrCtrl = readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[i]);

		if ((dwVICMmrCtrl&0x00000007) != 0) {
			*dwNum = i;
			return 0;
		}
	}
	return -1;
}	

/* ============================================================================================== */
static void VIC_SetSizeToVIC(DWORD width, DWORD height, DWORD stride, DWORD dwNum) {
	TVPLVICChnInfo *ptChnInfo;
	TVPLVICDevChnInfo *ptDevChnInfo;
	DWORD dwVICMmrCtrl, dwVICMmrCtrl1;
/* Version 11.0.0.4 modification, 2012.11.22 */
	DWORD dwInFmt, dwStartPixel, dwStartLine;
/* ======================================== */

	ptChnInfo = (TVPLVICChnInfo *)&gptDevInfo->ptMMRInfo->atChnInfo[dwNum];
	ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];

/* Version 11.0.0.4 modification, 2012.11.22 */
	dwStartLine = (readl(&ptChnInfo->dwCapV)&0x000003FF);
	dwStartPixel = (readl(&ptChnInfo->dwCapH)&0x000007FF);

	writel(((width+dwStartPixel)&0x0000FFFF)|((height+dwStartLine)<<16), &ptChnInfo->dwInSize);
	ptDevChnInfo->dwInWidth = width + dwStartPixel;
/* ======================================== */
	writel((readl(&ptChnInfo->dwCapH)&0x0000FFFF)|(width<<16), &ptChnInfo->dwCapH);
	writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(height<<16), &ptChnInfo->dwCapV);
/* Version 11.0.0.4 modification, 2012.11.22 */
	writel(((width+dwStartPixel)&0x0000FFFF)|((height+dwStartLine)<<16), &ptChnInfo->dwInSize);
/* ======================================== */
	writel(stride&0x0000FFFF, &ptChnInfo->dwStride);	// also set stride

	ptDevChnInfo->dwCapHeight = height;
	ptDevChnInfo->bHalfSize = FALSE;
	if (((ptChnInfo->dwInSize&0x0000FFFF)>MAX_EnhancedBayer_WIDTH)&&((gptDevInfo->ptMMRInfo->dwCMOSCtrl&0x00000003)==0x01)) {
		writel((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)&0xFFFFFFFC), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);
	}
	else if (((ptChnInfo->dwInSize&0x0000FFFF)<=MAX_EnhancedBayer_WIDTH)&&((gptDevInfo->ptMMRInfo->dwCMOSCtrl&0x00000003)==0x00)) {
		writel((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)|0x00000001), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);
	}

	dwVICMmrCtrl = readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);	
	writel((dwVICMmrCtrl|0x200), &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
	while ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x200) != 0); 	
	dwInFmt = (readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0x1c)>>2;	
	if ((dwInFmt==1) || (dwInFmt==3) || (dwInFmt==4)) {  // (IN_FORMAT=="16-bits RAW") || (IN_FORMAT=="16-bits with SYNC") || (IN_FORMAT=="Bayer")
		dwVICMmrCtrl1 = readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[1]);	
		writel((dwVICMmrCtrl1|0x200), &gptDevInfo->ptMMRInfo->adwChanCtrl[1]);	// update chn#1 MMRs
		while ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[1])&0x200) != 0); 
	}
}

/* ============================================================================================== */
static int VIC_CompareHeight(DWORD newh, DWORD dwNum) {
	TVPLVICChnInfo *ptChnInfo;
	DWORD orgh, dwStartLine;
	
	ptChnInfo = (TVPLVICChnInfo *)&gptDevInfo->ptMMRInfo->atChnInfo[dwNum];
	orgh = (readl(&ptChnInfo->dwInSize)&0xFFFF0000)>>16;
/* Version 11.0.0.4 modification, 2012.11.22 */
	dwStartLine = (readl(&ptChnInfo->dwCapV)&0x000003FF);

	return ((newh+dwStartLine) < orgh);
/* ======================================== */
}

/* ============================================================================================== */
static void VIC_SetSize(TVPLVICOptions tOptions, DWORD dwNum) {
	TVideoSignalOptions tSensorOptions;

	// Set Sensor, AE, and  VIC
	tSensorOptions.eOptionFlags = tOptions.eOptionFlags;
	tSensorOptions.adwUserData[0] = tOptions.adwUserData[0];	
	tSensorOptions.adwUserData[1] = tOptions.adwUserData[1];		

	if (VIC_CompareHeight(tOptions.adwUserData[1], dwNum)) { // change size from big to small 
		VIC_SetSizeToVIC(tOptions.adwUserData[0], tOptions.adwUserData[1], tOptions.adwUserData[6], dwNum);
		sensor_dev->ioctl(&tSensorOptions, dwNum);			
	}
	else { 
		sensor_dev->ioctl(&tSensorOptions, dwNum);			 
		VIC_SetSizeToVIC(tOptions.adwUserData[0], tOptions.adwUserData[1], tOptions.adwUserData[6], dwNum);
	}
	if (bAEEn || bIrisEn)
	{
		ae_dev->set_options(&tSensorOptions);
	}
}

/* ========================================= */
/* ============================================================================================== */
static irqreturn_t ISR(int irq, void *dev_id)
{
    TVideoCapBufInfo *ptBufInfo;
    TVPLVICChnInfo *ptChnInfo;
    TVPLVICDevChnInfo *ptDevChnInfo;

    DWORD dwNum = 0;
    DWORD i;
    DWORD dwVICMmrAllCtrl = readl(&gptDevInfo->ptMMRInfo->dwCtrl);
    DWORD dwVICMmrCtrl;
    DWORD dwVICMmrStat0 = readl(&gptDevInfo->ptMMRInfo->dwStat0);
    DWORD dwVICMmrStat1 = readl(&gptDevInfo->ptMMRInfo->dwStat1);
    DWORD dwVICMmrStat;
    DWORD dwStride;
    DWORD *pdwYAddr;
    DWORD *pdwCbAddr;
    DWORD *pdwCrAddr;
    DWORD dwMask;
    DWORD dwCurrTicks, dwDiffTicks, dwInterval, dwTmp;
    BOOL bEnableOutput;
    QWORD qwAELum;
    DWORD dwAFFocusValueHigh, dwAFFocusValueLow;
    DWORD dwData, dwAEPixel;
    TVideoCapBufInfo *ptTmpBufInfo, *ptBufAInfo, *ptBufBInfo, *ptBufCInfo, *ptBufDInfo, *ptBufEInfo;
    struct timeval tv;
#ifdef __VIC_ENABLE_CALIBRATION__
    DWORD dwSysSec, dwSysMSec, dwCapSec, dwCapMSec;
    SDWORD sdwDiffSec, sdwDiffMSec;
#endif //__VIC_ENABLE_CALIBRATION__
    BOOL bWakeUp, bDevErr;
    DWORD dwRptWidth;
    BOOL bDarkFrame;
#ifdef __USE_PROFILE__
    DWORD *pdwTMRCVirtAddr;
    DWORD dwFirstTick, dwSecondTick, dwMaxTick;
    SDWORD sdwISRInterval;
    pdwTMRCVirtAddr = (DWORD *)IO_ADDRESS(VPL_TMRC_MMR_BASE);
    dwFirstTick = *(pdwTMRCVirtAddr+9);
    dwMaxTick = *(pdwTMRCVirtAddr+10);
#endif //__USE_PROFILE__
    bWakeUp = FALSE;
    bDevErr = FALSE;

    if (VIC_GetDevNum(&dwNum) < 0)
    {
         printk("Error!! No device issues interrupt, but ISR is entered !!\n");
    }
    PDEBUG("Enter device %lu ISR function ...\n", dwNum);		
    dwVICMmrCtrl = readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
    if (dwNum<4)
    {
        dwVICMmrStat =(dwVICMmrStat0>>(8*dwNum));
    }
    else
    {
        dwVICMmrStat =(dwVICMmrStat1>>(6*(dwNum-4)));
    }
    if (!(dwVICMmrCtrl&0x00000100)) { // output enable not set
        if ( !(dwVICMmrStat&0x16)) {  // no error
            dwVICMmrCtrl |= 0x100;    // set output enable and return
            writel(dwVICMmrCtrl, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
            return IRQ_HANDLED;		
        }
        else { // frame error
            return IRQ_HANDLED; // do nothing, just return			
        }
    }
    gptDevInfo->dwNum = dwNum;

    ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
    ptBufInfo = ptDevChnInfo->ptCaptureBuf;
    ptChnInfo = (TVPLVICChnInfo *)&gptDevInfo->ptMMRInfo->atChnInfo[dwNum];

	
    pdwYAddr = ((dwVICMmrStat&0x00000020)==0) ? (&ptChnInfo->dwYAddr0) : (&ptChnInfo->dwYAddr1);
    pdwCbAddr = ((dwVICMmrStat&0x00000020)==0) ? (&ptChnInfo->dwCbAddr0) : (&ptChnInfo->dwCbAddr1);
    pdwCrAddr = ((dwVICMmrStat&0x00000020)==0) ? (&ptChnInfo->dwCrAddr0) : (&ptChnInfo->dwCrAddr1);

    dwStride = readl(&ptChnInfo->dwStride);
    dwRptWidth = (dwStride&0xFFFF0000) >> 16;
    if (dwRptWidth != 0)
    {
        writel(dwStride&0x0000FFFF, &ptChnInfo->dwStride);
    }
    else
    {
        dwRptWidth = ptDevChnInfo->dwInWidth;
    }
    if (ptDevChnInfo->dwFlipDelayCnt != 0)
    {
        if (ptDevChnInfo->dwFlipDelayCnt < 2)
        {
            ptDevChnInfo->dwFlipDelayCnt++;
        }
        else 
        {
            ptDevChnInfo->dwFlipDelayCnt = 0;
            ptDevChnInfo->bFlip = (dwVICMmrCtrl&(0x1<<13)) ? TRUE : FALSE;
        }
    }
    ptBufInfo->bFlip = ptDevChnInfo->bFlip;	
    if ((ptDevChnInfo->bSetOption==TRUE) && (ptDevChnInfo->dwInputInterval!=0))
    {
        switch (ptDevChnInfo->tOptions.eOptionFlags)
        {
            case VIDEO_SIGNAL_OPTION_SET_SIZE:
/* Version 12.0.0.0 modification, 2013.04.19 */
/* ======================================== */
                break;
            case VIDEO_SIGNAL_OPTION_SET_TONE_MAPPING:
                writel((readl(&gptDevInfo->ptMMRInfo->dwTMCtrl)&0xFFFFFFF0)|ptDevChnInfo->tOptions.adwUserData[0], &gptDevInfo->ptMMRInfo->dwTMCtrl);
            break;
            case VIDEO_SIGNAL_OPTION_SET_CONTRAST_ENHANCEMENT:
                writel(ptDevChnInfo->tOptions.adwUserData[1], &ptChnInfo->dwCETblAddr);
                writel(((readl(&ptChnInfo->dwCECtrl)&0xFFFFFFC0)|(ptDevChnInfo->tOptions.adwUserData[0]&0x3F)), &ptChnInfo->dwCECtrl);
            break;
            case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS:
                writel(ptDevChnInfo->adwAEMeasureWinTbl[0], &gptDevInfo->ptMMRInfo->dwAWBWindowStart);
                writel(ptDevChnInfo->adwAEMeasureWinTbl[1], &gptDevInfo->ptMMRInfo->dwAWBWindowRange);
                break;
/* Version 11.0.0.6 modification, 2013.01.30 */
/* ======================================== */
            case VIDEO_SIGNAL_OPTION_SET_CAPTURE_AREA:
                writel((readl(&ptChnInfo->dwCapH)&0x0000FFFF)|(ptDevChnInfo->tOptions.adwUserData[0]<<16), &ptChnInfo->dwCapH);
                writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(ptDevChnInfo->tOptions.adwUserData[1]<<16), &ptChnInfo->dwCapV);
                ptDevChnInfo->dwCapHeight = ptDevChnInfo->tOptions.adwUserData[1];
                ptDevChnInfo->bHalfSize = FALSE;
                break;
            case VIDEO_SIGNAL_OPTION_SET_START_PIXEL:
                writel((readl(&ptChnInfo->dwCapH)&0xFFFF0000)|ptDevChnInfo->tOptions.adwUserData[0], &ptChnInfo->dwCapH);
                writel((readl(&ptChnInfo->dwCapV)&0xFFFF0000)|ptDevChnInfo->tOptions.adwUserData[1], &ptChnInfo->dwCapV);
                break;
            case VIDEO_SIGNAL_OPTION_SET_FLIP:
                dwVICMmrCtrl = (dwVICMmrCtrl&0xFFFFDFFF) | ((ptDevChnInfo->tOptions.adwUserData[0]==TRUE)<<13);
                if (sensor_dev->get_remaining_line_num) 
                {
                    ptDevChnInfo->dwFlipDelayCnt = 1;
                }
                break;
            case VIDEO_SIGNAL_OPTION_SET_MIRROR:
                dwVICMmrCtrl = (dwVICMmrCtrl&0xFFFFBFFF) | ((ptDevChnInfo->tOptions.adwUserData[0]==TRUE)<<14);
                break;
            case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_WINDOW:
                writel(ptDevChnInfo->adwAFMeasureWinTbl[0], &gptDevInfo->ptMMRInfo->dwAFWindowStartPosition);
                writel(ptDevChnInfo->adwAFMeasureWinTbl[1], &gptDevInfo->ptMMRInfo->dwAFWindowSize);             
                if (ptDevChnInfo->dwAFState  == AF_STATECTRL_IDLE) // if AF is at idle state
                {
                    ptDevChnInfo->dwAFState = AF_STATECTRL_WAITE; // set to wait state  
                }
                break;				
            case VIDEO_SIGNAL_OPTION_SET_FOCUS_POSITION:
                if (ptDevChnInfo->dwAFState == AF_STATECTRL_IDLE) // if AF is at idle state
                {
                    ptDevChnInfo->dwAFState = AF_STATECTRL_MF; // manual focus state                
                    if (ptDevChnInfo->tOptions.adwUserData[0] == 0) 
                    {
                        ptDevChnInfo->dwManualDir = (DWORD)LENS_MOVE_NEAR;
                    }
                    else 
                    {
                        ptDevChnInfo->dwManualDir = (DWORD)LENS_MOVE_FAR;
                    }						
                }
                break;		
            case VIDEO_SIGNAL_OPTION_SET_ZOOM_POSITION:
                if (ptDevChnInfo->dwAFState == AF_STATECTRL_IDLE) // if AF is at idle state
                {
                    ptDevChnInfo->dwAFState = AF_STATECTRL_ZOOM; // zoom control state
                    // zoom in
                    if (ptDevChnInfo->tOptions.adwUserData[0] == 0) 
                    {
                        ptDevChnInfo->dwManualDir = (DWORD)LENS_MOVE_FAR; // zoom-in
                    }
                    // zoom out
                    else
                    {
                        ptDevChnInfo->dwManualDir = (DWORD)LENS_MOVE_NEAR; // zoom-out
                    }						
                }
                break;	
            case VIDEO_SIGNAL_OPTION_SET_CONTRAST:
                writel((readl(&ptChnInfo->dwSBC)&0xFE00FF00) | (ptDevChnInfo->tOptions.adwUserData[0]&0x01FF00FF), &ptChnInfo->dwSBC);
                break;
            case VIDEO_SIGNAL_OPTION_SET_BRIGHTNESS:
                writel((readl(&ptChnInfo->dwSBC)&0xFFFF00FF)|((ptDevChnInfo->tOptions.adwUserData[0]&0x000000FF)<<8), &ptChnInfo->dwSBC);
                break;
            case VIDEO_SIGNAL_OPTION_SET_SATURATION:
                writel((readl(&ptChnInfo->dwSBC)&0xFE00FFFF)|((ptDevChnInfo->tOptions.adwUserData[0]&0x000001FF)<<16), &ptChnInfo->dwSBC);
                break;
            case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
                writel((readl(&gptDevInfo->ptMMRInfo->dwAWBCtrl)&0xFFFFF7FF)|((ptDevChnInfo->tOptions.adwUserData[0])<<11), &gptDevInfo->ptMMRInfo->dwAWBCtrl);
                writel((readl(&gptDevInfo->ptMMRInfo->dwManualGain)&0xF0000000)|(ptDevChnInfo->tOptions.adwUserData[1]), &gptDevInfo->ptMMRInfo->dwManualGain);
                break;
            case VIDEO_SIGNAL_OPTION_SET_COLOR_CORRECTION:
                writel(ptDevChnInfo->tOptions.adwUserData[0],
                      &gptDevInfo->ptMMRInfo->dwCCCoeffR);
                writel(ptDevChnInfo->tOptions.adwUserData[1],
                      &gptDevInfo->ptMMRInfo->dwCCCoeffG);
                writel(ptDevChnInfo->tOptions.adwUserData[2],
                      &gptDevInfo->ptMMRInfo->dwCCCoeffB);
                break;
/* Version 12.0.0.2 modification, 2013.08.19 */
            case VIDEO_SIGNAL_OPTION_SET_COLOR_TRANSFORM:
                writel(ptDevChnInfo->tOptions.adwUserData[0],
                      &gptDevInfo->ptMMRInfo->dwRGB2YCbCrCoeff0);
                writel(ptDevChnInfo->tOptions.adwUserData[1],
                      &gptDevInfo->ptMMRInfo->dwRGB2YCbCrCoeff1);
                writel(ptDevChnInfo->tOptions.adwUserData[2],
                      &gptDevInfo->ptMMRInfo->dwRGB2YCbCrCoeff2);
                break;
/* ======================================== */
            case VIDEO_SIGNAL_OPTION_SET_GAMMA_TABLE:
                if ((dwVICMmrAllCtrl&0x0000001C)!=0x10)
                {
                    break;
                }
                else
                {
                    writel((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)| 0x00000008), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);
                }
                break;
            case VIDEO_SIGNAL_OPTION_SET_FRAME_RATE:
                ptDevChnInfo->dwFrameRate = ptDevChnInfo->tOptions.adwUserData[0];
                if (ptDevChnInfo->dwFrameRate != 0)
                {
                    ptDevChnInfo->dwFrameCount = 0;
                    ptDevChnInfo->dwInterval = 1000 / ptDevChnInfo->dwFrameRate;
                    ptDevChnInfo->dwIntervalResidue = 1000 - ((1000/ptDevChnInfo->dwFrameRate)*ptDevChnInfo->dwFrameRate);
                }
                //printk("<Ch %d>: Set frame rate : %d; Input interval: %d ms\n", dwNum, ptDevChnInfo->dwFrameRate, ptDevChnInfo->dwInputInterval);
                break;
            case VIDEO_SIGNAL_OPTION_SET_HALF_SIZED_OUTPUT:
                ptDevChnInfo->bHalfSizedOutput = ptDevChnInfo->tOptions.adwUserData[0];
                //printk("<Ch %d>: Set half sized output : %d\n", dwNum, ptDevChnInfo->bHalfSizedOutput);
                break;
            case VIDEO_SIGNAL_OPTION_SET_ANTI_ALIASING:
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAHVAvgThrd);
                dwData &= ~(0x00000001<<31);  // clear CFAHVBlendEn
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[0]&0x00000001)<<31;  // set CFAHVBlendEn
                dwData &= ~(0x0003FFFF<<0); // clear CFAHVAvgThrd
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[1]&0x0003FFFF)<<0;  // set CFAHVAvgThrd
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAHVAvgThrd);    
            
                // set BayerCFAHVBlendMaxThrd
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMaxThrd);    
                dwData &= ~(0x0003FFFF<<0); // clear CFAHVBlendMaxThrd
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[3]&0x0003FFFF)<<0;  // set CFAHVBlendMaxThrd
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMaxThrd);    
            
                // set BayerCFAHVBlendMinThrd
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMinThrd);    
                dwData &= ~(0x0003FFFF<<0); // clear CFAHVBlendMinThrd
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[2]&0x0003FFFF)<<0;  // set CFAHVBlendMinThrd
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMinThrd);    
            
                // set BayerCFAFCSEdgeThrd
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSEdgeThrd);    
                dwData &= ~(0x0003FFFF<<0); // clear CFAFCSEdgeThrd
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[5]&0x0003FFFF)<<0;  // set CFAFCSEdgeThrd
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSEdgeThrd);    
            
                // set BayerCFAFCSMaxThrd
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSMaxThrd);    
                dwData &= ~(0x0003FFFF<<0); // clear CFAFCSMaxThrd
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[7]&0x0003FFFF)<<0;  // set CFAFCSMaxThrd
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSMaxThrd);    
            	
                // set BayerCFAFCSMinThrd
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSMinThrd);    
                dwData &= ~(0x0003FFFF<<0); // clear CFAFCSMinThrd
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[6]&0x0003FFFF)<<0;  // set CFAFCSMinThrd
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSMinThrd);        
            
                // set BayerCFAFCSCtrl
                dwData = readl(&gptDevInfo->ptMMRInfo->dwBayerCFAFCSCtrl);
                dwData &= ~(0x00000001<<10);  // clear CFAFCSEn
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[4]&0x00000001)<<10;  // set CFAFCSEn
                dwData &= ~(0x00000001<<9); // clear CFAFCSExcludeEdge
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[8]&0x00000001)<<9;  // set CFAFCSExcludeEdge
                dwData &= ~(0x000001FF<<0); // clear CFAFCSAlpha
                dwData |= (ptDevChnInfo->dwCFAAntiAliasingTbl[9]&0x000001FF)<<0;  // set CFAFCSAlpha
                writel(dwData, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSCtrl);   
                break;
            case VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE:
                if (ptDevChnInfo->tVPLVICISPParam.bCMOSCtrl == TRUE)
                {
                    if ((dwVICMmrAllCtrl&0x0000001C)!=0x10)
                    {
                        break;
                    }
                    else
                    {
                        writel((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)| 0x00000008), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);
                    }
                }
                else if (ptDevChnInfo->tVPLVICISPParam.bTMCtrl == TRUE)
                {
                    writel((readl(&gptDevInfo->ptMMRInfo->dwTMCtrl)&0xFFFFFFF0)|ptDevChnInfo->tVPLVICISPParam.dwTMCtrl, &gptDevInfo->ptMMRInfo->dwTMCtrl);
                }
				
                if (ptDevChnInfo->tVPLVICISPParam.bCECtrl == TRUE)
                {
                    writel(ptDevChnInfo->tVPLVICISPParam.dwCETblAddr, &ptChnInfo->dwCETblAddr);
                    writel(((readl(&ptChnInfo->dwCECtrl)&0xFFFFFFC0)|(ptDevChnInfo->tVPLVICISPParam.dwCECtrl&0x3F)), &ptChnInfo->dwCECtrl);				
                }

                if (ptDevChnInfo->tVPLVICISPParam.bCCCtrl == TRUE)
                {
                    writel(ptDevChnInfo->tVPLVICISPParam.dwCCCoeffR, &gptDevInfo->ptMMRInfo->dwCCCoeffR);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwCCCoeffG, &gptDevInfo->ptMMRInfo->dwCCCoeffG);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwCCCoeffB, &gptDevInfo->ptMMRInfo->dwCCCoeffB);
                }

                if (ptDevChnInfo->tVPLVICISPParam.bSBCCtrl == TRUE)
                {
                    writel((readl(&ptChnInfo->dwSBC)&0xFE000000) | (ptDevChnInfo->tVPLVICISPParam.dwSBC), &ptChnInfo->dwSBC);         
                }

                if (ptDevChnInfo->tVPLVICISPParam.bAntialiasingCtrl == TRUE)
                {
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAHVAvgThrd, &gptDevInfo->ptMMRInfo->dwBayerCFAHVAvgThrd);    
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAHVBlendMaxThrd, &gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMaxThrd);    
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAHVBlendMinThrd, &gptDevInfo->ptMMRInfo->dwBayerCFAHVBlendMinThrd);    
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAFCSEdgeThrd, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSEdgeThrd);    
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAFCSMaxThrd, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSMaxThrd);    
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAFCSMinThrd, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSMinThrd);        
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBayerCFAFCSCtrl, &gptDevInfo->ptMMRInfo->dwBayerCFAFCSCtrl);
                }

/* Version 11.0.0.3 modification, 2012.11.02 */
                if (ptDevChnInfo->tVPLVICISPParam.bDeimpulseCtrl == TRUE)
                {
                    writel((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)&0xFFFFFFFB)|((ptDevChnInfo->tVPLVICISPParam.dwDeimpulseCtrl&0x01)<<2), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);

                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseThrd, &gptDevInfo->ptMMRInfo->dwDenoiseG0Thrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMaxThrd, &gptDevInfo->ptMMRInfo->dwDenoiseG0MaxThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMinThrd, &gptDevInfo->ptMMRInfo->dwDenoiseG0MinThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseThrd, &gptDevInfo->ptMMRInfo->dwDenoiseRThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMaxThrd, &gptDevInfo->ptMMRInfo->dwDenoiseRMaxThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMinThrd, &gptDevInfo->ptMMRInfo->dwDenoiseRMinThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseThrd, &gptDevInfo->ptMMRInfo->dwDenoiseBThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMaxThrd, &gptDevInfo->ptMMRInfo->dwDenoiseBMaxThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMinThrd, &gptDevInfo->ptMMRInfo->dwDenoiseBMinThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseThrd, &gptDevInfo->ptMMRInfo->dwDenoiseG1Thrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMaxThrd, &gptDevInfo->ptMMRInfo->dwDenoiseG1MaxThrd);
                    writel(ptDevChnInfo->tVPLVICISPParam.dwDenoiseMinThrd, &gptDevInfo->ptMMRInfo->dwDenoiseG1MinThrd);		   
                }

                if (ptDevChnInfo->tVPLVICISPParam.bBlackClampCtrl == TRUE)
                {
                    writel(ptDevChnInfo->tVPLVICISPParam.dwBlackClamp, &gptDevInfo->ptMMRInfo->dwBayerClamp);
                }

                if (ptDevChnInfo->tVPLVICISPParam.bCFAModeCtrl == TRUE)
                {
                    writel(((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)&0xFFFFFFFC)|(ptDevChnInfo->tVPLVICISPParam.dwCFAMode&0x03)), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);
                }
/* ======================================== */
                break;
            case VIDEO_SIGNAL_OPTION_SET_BLACK_CLAMP:
                writel(ptDevChnInfo->tOptions.adwUserData[0], &gptDevInfo->ptMMRInfo->dwBayerClamp);
                break;
            case VIDEO_SIGNAL_OPTION_SET_IMPULSE_NOISE_REMOVAL:
                writel((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)&0xFFFFFFFB)|((ptDevChnInfo->tOptions.adwUserData[0])<<2), &gptDevInfo->ptMMRInfo->dwCMOSCtrl);

                writel(ptDevChnInfo->dwDeImpulseTbl[0], &gptDevInfo->ptMMRInfo->dwDenoiseG0Thrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[1], &gptDevInfo->ptMMRInfo->dwDenoiseG0MaxThrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[2], &gptDevInfo->ptMMRInfo->dwDenoiseG0MinThrd);
 
                writel(ptDevChnInfo->dwDeImpulseTbl[3], &gptDevInfo->ptMMRInfo->dwDenoiseRThrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[4], &gptDevInfo->ptMMRInfo->dwDenoiseRMaxThrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[5], &gptDevInfo->ptMMRInfo->dwDenoiseRMinThrd);
 
                writel(ptDevChnInfo->dwDeImpulseTbl[6], &gptDevInfo->ptMMRInfo->dwDenoiseBThrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[7], &gptDevInfo->ptMMRInfo->dwDenoiseBMaxThrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[8], &gptDevInfo->ptMMRInfo->dwDenoiseBMinThrd);
 
                writel(ptDevChnInfo->dwDeImpulseTbl[9], &gptDevInfo->ptMMRInfo->dwDenoiseG1Thrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[10], &gptDevInfo->ptMMRInfo->dwDenoiseG1MaxThrd);
                writel(ptDevChnInfo->dwDeImpulseTbl[11], &gptDevInfo->ptMMRInfo->dwDenoiseG1MinThrd);		   
                break;
            default:
                break;
        }
        ptDevChnInfo->bSetOption = FALSE;
        bWakeUp = TRUE;
    }

    bEnableOutput = (((dwVICMmrCtrl&0x00000100)!=0) && (ptDevChnInfo->dwInputInterval!=0)) ? TRUE : FALSE;

    // Just use whole image Green sum
    qwAELum = ((QWORD)(readl(&gptDevInfo->ptMMRInfo->dwAWBGreenHighSum))<<32) + readl(&gptDevInfo->ptMMRInfo->dwAWBGreenLowSum);
    dwAEPixel = ptDevChnInfo->dwCapHeight * ptDevChnInfo->dwMaxFrameWidth;

    bDarkFrame = FALSE;
    if (qwAELum < (gdwBlankFrameLumThres*dwAEPixel))
    {
        if (ptDevChnInfo->dwFrameNumCnt < ptDevChnInfo->dwRealFrameNum)
        {
            bDarkFrame = TRUE;
            ptBufInfo->bReadyForProcess = FALSE;
            ptDevChnInfo->dwFrameNumCnt++;
            PDEBUG("<VIC>drop black, %d, %d, %d\n", ptBufInfo->dwIndex, qwAELum/dwAEPixel, ptDevChnInfo->dwFrameCountAll);
        }
        else
        {
            ptDevChnInfo->dwFrameNumCnt = 0;
        }
    }
    else
    {
		if (ptDevChnInfo->dwRealFrameNum != ptDevChnInfo->dwFrameNum)
		{
			PDEBUG("FR %d->%d\n", ptDevChnInfo->dwRealFrameNum, ptDevChnInfo->dwFrameNum);
		}
        ptDevChnInfo->dwRealFrameNum = ptDevChnInfo->dwFrameNum;
        ptDevChnInfo->dwFrameNumCnt = 0;
    }
    dwAFFocusValueHigh	= readl(&gptDevInfo->ptMMRInfo->dwAFFocusValueHigh);
    dwAFFocusValueLow	= readl(&gptDevInfo->ptMMRInfo->dwAFFocusValueLow);
    ptDevChnInfo->qwLatestAFFocusValue = (((QWORD)dwAFFocusValueHigh)<<32) + (QWORD)dwAFFocusValueLow;	
    // If no ccir or width check error and frame complete
    if (((dwVICMmrStat&0x17)!=0x10) && ((dwVICMmrCtrl&0x07)==0x01))
    //if (((dwVICMmrStat&0x17)!=0x10) &&
    //	((dwVICMmrCtrl&0x07)==0x01) &&
    //	(dwRptWidth==ptDevChnInfo->dwInWidth))
    {
        ptDevChnInfo->dwErrACK = dwVICMmrStat & 0x1F;
        if ((dwVICMmrStat&0x17) == 0x00)
        {
            // system is too busy and the status is cleared by HW because the new frame is being captured.
            // MMRs should not be changed now, but *_ACK still need to be cleared.
            PDEBUG("<Ch %lu> Index: %lu, dwVICMmrStat: %08lX, dwVICMmrCtrl: %08lX (too slow to get buffer)\n", dwNum, ptDevChnInfo->ptCaptureBuf->dwIndex, dwVICMmrStat, dwVICMmrCtrl);
        }
        // When system is too busy, the 16th frame will be dropped.
        // It will cause the framerate-control fail. (dwInputInterval==0)
        //else
        {
            /* Two cases for ptDevChnInfo->dwErrACK:
             * zero: No error occured during capturing.
             * non-zero: Some kind of error (mainly for FIFO full) occurred during capturing.
             *            The intterrupt ack of this kind of error is disabled to complete capturing.
             */

            ptBufInfo->dwErrACK = ptDevChnInfo->dwErrACK | (dwVICMmrStat&0x1F);
            dwTmp = ptDevChnInfo->ptCaptureBuf->ptNextBuf->ptNextBuf->dwIndex;
            if (((ptDevChnInfo->ptCaptureBuf->ptNextBuf->ptNextBuf->bReadyForCapture == TRUE) &&
                 (ptDevChnInfo->ptCaptureBuf->ptNextBuf->ptNextBuf->dwInUseNum==0)) ||
                (ptDevChnInfo->bStateUpdate==TRUE))
            {
                dwCurrTicks = readl(&ptChnInfo->dwMilliSecond);
                do_gettimeofday(&tv);

                if (bEnableOutput == TRUE) // output enable is on
                {
                    if ((ptDevChnInfo->bStateUpdate==TRUE) && (bDarkFrame==FALSE))
                    { //state update
	                    ptBufInfo->dwFrameCount = ptDevChnInfo->dwFrameCountAll;
#ifdef __VIC_ENABLE_CALIBRATION__
	                    dwSysSec = tv.tv_sec;
	                    dwSysMSec = tv.tv_usec;
	                    dwDiffTicks = dwCurrTicks - ptDevChnInfo->dwPrevTicks;
	                    dwCapSec = dwDiffTicks / 1000000;
	                    dwCapMSec = dwDiffTicks % 1000000;
	                    sdwDiffSec = dwSysSec - dwCapSec - ptDevChnInfo->dwBaseSecond;
	                    sdwDiffMSec = dwSysMSec - dwCapMSec - ptDevChnInfo->dwBaseMicroSecond + (sdwDiffSec*1000000);
	                    sdwDiffMSec = (sdwDiffMSec<0) ? (-sdwDiffMSec) : sdwDiffMSec;
	                    if (sdwDiffMSec >= (gdwCalThr*1000))
	                    {
	                        ptBufInfo->bTimeStampCal = TRUE;
	                        ptBufInfo->dwSecond = dwSysSec;
	                        ptBufInfo->dwMicroSecond = dwSysMSec;
	                    }
	                    else
	                    {
	                        ptBufInfo->bTimeStampCal = FALSE;
	                        ptBufInfo->dwSecond = ptDevChnInfo->dwBaseSecond + dwCapSec;
	                        ptBufInfo->dwMicroSecond = ptDevChnInfo->dwBaseMicroSecond + dwCapMSec;
	                        if (ptBufInfo->dwMicroSecond >= 1000000)
	                        {
	                            ptBufInfo->dwMicroSecond = ptBufInfo->dwMicroSecond - 1000000;
	                            ptBufInfo->dwSecond = ptBufInfo->dwSecond + 1;
	                        }
	                    }
	                    ptDevChnInfo->dwBaseSecond = ptBufInfo->dwSecond;
	                    ptDevChnInfo->dwBaseMicroSecond = ptBufInfo->dwMicroSecond;
#else
	                    ptBufInfo->dwSecond = tv.tv_sec;
	                    ptBufInfo->dwMicroSecond = tv.tv_usec;
#endif //__VIC_ENABLE_CALIBRATION__

                           ptBufInfo->dwInWidth = dwRptWidth;
                           //printk("Index : %d ptBufInfo->dwAEWin0Lum :%d\n", ptBufInfo->dwIndex, ptBufInfo->dwAEWin0Lum);
                           ptBufInfo->dwAFFocusValueHigh = dwAFFocusValueHigh;
                           ptBufInfo->dwAFFocusValueLow = dwAFFocusValueLow;

                           ptBufInfo->qwAWBRedSum = (((QWORD)readl(&gptDevInfo->ptMMRInfo->dwAWBRedHighSum))<<32) + 
						   					(QWORD)readl(&gptDevInfo->ptMMRInfo->dwAWBRedLowSum);	

                           ptBufInfo->qwAWBGreenSum = (((QWORD)readl(&gptDevInfo->ptMMRInfo->dwAWBGreenHighSum))<<32) + 
						   					(QWORD)readl(&gptDevInfo->ptMMRInfo->dwAWBGreenLowSum);	

                           ptBufInfo->qwAWBBlueSum = (((QWORD)readl(&gptDevInfo->ptMMRInfo->dwAWBBlueHighSum))<<32) + 
						   					(QWORD)readl(&gptDevInfo->ptMMRInfo->dwAWBBlueLowSum);	

                           ptBufInfo->dwAWBGain = readl(&gptDevInfo->ptMMRInfo->dwManualGain)&0x0FFFFFFF;
                    }

                    if (((ptDevChnInfo->ptCaptureBuf->ptNextBuf->ptNextBuf->bReadyForCapture == TRUE) &&
                         (ptDevChnInfo->ptCaptureBuf->ptNextBuf->ptNextBuf->dwInUseNum==0)) ||
                         (bDarkFrame==TRUE))
                    	// (bDarkFrame==TRUE): recycle dark frame and re-assign the linked list
                    {
                        ptDevChnInfo->bStateUpdate = TRUE;
                        if (bDarkFrame == FALSE)
                        {
                            // part of buffer switch and address upadte

                            ptBufInfo->bReadyForProcess = TRUE;
                            ptDevChnInfo->ptProcessBuf = ptDevChnInfo->ptCaptureBuf;
#ifdef __USE_LAST_FRAME__
                            ptDevChnInfo->ptCaptureBuf->ptPreBuf->bReadyForProcess = FALSE;
#endif

                            ptDevChnInfo->ptCaptureBuf = ptBufInfo->ptNextBuf;
                        }
                        else							
                        {	// re-assign the linked list and index, B is black frame (ptBufInfo.
                            ptBufAInfo = ptBufInfo->ptPreBuf;
                            ptBufBInfo = ptBufInfo;
                            ptBufCInfo = ptBufInfo->ptNextBuf;
                            ptBufDInfo = ptBufCInfo->ptNextBuf;
                            ptBufEInfo = ptBufDInfo->ptNextBuf;
                            
                            // A<->B<->C<->D<->E => A<->C<->B<->D<->E
                            // change linked list
                            ptBufAInfo->ptNextBuf = ptBufCInfo;
                            ptBufBInfo->ptPreBuf = ptBufCInfo;
                            ptBufBInfo->ptNextBuf = ptBufDInfo;
                            ptBufCInfo->ptPreBuf = ptBufAInfo;
                            ptBufCInfo->ptNextBuf = ptBufBInfo;
                            ptBufDInfo->ptPreBuf = ptBufBInfo;
                            
                            ptBufInfo = ptBufCInfo;
                            
                            ptTmpBufInfo = ptBufInfo;
#ifdef _DEBUG
                            printk("<VIC>re-assign linked list, %d<->%d\n", ptBufBInfo->dwIndex, ptBufCInfo->dwIndex);
                            for (i=0; i<ptDevChnInfo->dwBufNum; i++)
                            {
                                if (i==0)
                                {
                                	printk("%d", ptTmpBufInfo->dwIndex);
                                }
                                else
                                {
                                	printk("->%d", ptTmpBufInfo->dwIndex);
                                }
                                ptTmpBufInfo = ptTmpBufInfo->ptNextBuf;
                            }
                            printk("\n");
#endif //_DEBUG

                            ptDevChnInfo->ptCaptureBuf = ptBufInfo;
                        }
                        // recycle the black frame
                        ptBufInfo = ptDevChnInfo->ptCaptureBuf->ptNextBuf;
                        //printk("Yaddr: %08lx, CbAddr: %08lx, CrAddr: %08lx\n", ptBufInfo->dwBufY, ptBufInfo->dwBufCb, ptBufInfo->dwBufCr);
                        ptBufInfo->bReadyForProcess = FALSE;
                        ptBufInfo->bReadyForSlvProcess = FALSE;
                        ptBufInfo->dwErrACK = 0;
                        writel(ptBufInfo->dwBufY, (&ptChnInfo->dwYAddr0));
                        writel(ptBufInfo->dwBufY, (&ptChnInfo->dwYAddr1));
                        writel(ptBufInfo->dwBufCb, (&ptChnInfo->dwCbAddr0));
                        writel(ptBufInfo->dwBufCb, (&ptChnInfo->dwCbAddr1));
                        writel(ptBufInfo->dwBufCr, (&ptChnInfo->dwCrAddr0));
                        writel(ptBufInfo->dwBufCr, (&ptChnInfo->dwCrAddr1));
                    }
                    else
                    {
                    	PDEBUG("No buffer update, %d\n", ptDevChnInfo->dwFrameCountAll);
                        ptDevChnInfo->bStateUpdate = FALSE;
                    }
                }

                if ((ptDevChnInfo->dwFrameRate!=0) || (ptDevChnInfo->dwInputInterval==0))
                {
                    if (ptDevChnInfo->bStartOutput == FALSE) // estimate the input frame interval
                    {
                        if (ptDevChnInfo->dwFrameCountAll > 0)
                        {
                            if (ptDevChnInfo->dwFrameCountAll <= 16)
                            {
                                ptDevChnInfo->dwDiffResidue = ptDevChnInfo->dwDiffResidue + (dwCurrTicks-ptDevChnInfo->dwPrevTicks);
                                if (ptDevChnInfo->dwFrameCountAll == 16)
                                {
                                    ptDevChnInfo->dwInputInterval = ptDevChnInfo->dwDiffResidue >> 4;
                                    PDEBUG("<Ch %lu>: 16 input frame interval : %lu us\n", dwNum, ptDevChnInfo->dwDiffResidue);
                                    ptDevChnInfo->dwDiffResidue = 0;
                                    PDEBUG("<Ch %lu>: Input frame interval : %lu us\n", dwNum, ptDevChnInfo->dwInputInterval);
                                    PDEBUG("<Ch %lu>: Required frame interval : %lu us\n", dwNum, ptDevChnInfo->dwInterval);
                                    if (ptDevChnInfo->dwFrameRate==0)
                                    {
                                        dwVICMmrCtrl = dwVICMmrCtrl | 0x00000100;
                                        ptDevChnInfo->bStartOutput = TRUE;
                                        PDEBUG("<Ch %lu>: Start Output at %lu frame (%lu). Input interval: %lu us. %ld:%ld\n", dwNum, ptDevChnInfo->dwFrameCountAll, ptDevChnInfo->dwFrameCount, ptDevChnInfo->dwInputInterval, tv.tv_sec, tv.tv_usec);
                                    }
                                    else
                                    {
                                        ptDevChnInfo->bStartOutput = TRUE;
                                        //printk("<Ch %lu>: Start Output at %lu frame (%lu). Input interval: %lu us. %ld:%ld\n", dwNum, ptDevChnInfo->dwFrameCountAll, ptDevChnInfo->dwFrameCount, ptDevChnInfo->dwInputInterval, tv.tv_sec, tv.tv_usec);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        /*if (dwCurrTicks >= ptDevChnInfo->dwPrevTicks)
                        {
                            dwDiffTicks = dwCurrTicks - ptDevChnInfo->dwPrevTicks;
                        }
                        else
                        {
                            dwDiffTicks = (DWORD)((DWORD)4294967295-ptDevChnInfo->dwPrevTicks) + dwCurrTicks;
                        }*/
                        dwDiffTicks = ptDevChnInfo->dwInputInterval;
                        dwInterval = (ptDevChnInfo->dwFrameCount!=0) ? ptDevChnInfo->dwInterval : (ptDevChnInfo->dwInterval+ptDevChnInfo->dwIntervalResidue);
                        ptDevChnInfo->dwLatestTS = dwCurrTicks + ((ptDevChnInfo->bHalfSize==TRUE)?(ptDevChnInfo->dwInputInterval>>1):0);
                        dwTmp = ptDevChnInfo->dwDiffResidue + dwDiffTicks;
                        if ((dwTmp>=dwInterval) && (bEnableOutput==TRUE) && (ptDevChnInfo->bHalfSize==FALSE))
                        {
                            ptDevChnInfo->dwDiffResidue = dwTmp - dwInterval;
                            ptDevChnInfo->dwFrameCount = ((ptDevChnInfo->dwFrameCount+1)<ptDevChnInfo->dwFrameRate)
                                                       ? (ptDevChnInfo->dwFrameCount+1)
                                                       : 0;
                            if ((ptDevChnInfo->dwFrameCount==0) && (ptDevChnInfo->dwDiffResidue>ptDevChnInfo->dwInterval))
                            {   // if the residue accumulation after one second is longer than one frame interval,
                                // this is not the normal case and reset residue.
                                ptDevChnInfo->dwDiffResidue = 0;
                            }
                        }
                        else
                        {
                            ptDevChnInfo->dwDiffResidue = dwTmp;
                        }
                        //printk("Resd:%d, HS:%d, OE:%d, Int:%d\n", ptDevChnInfo->dwDiffResidue, ptDevChnInfo->bHalfSize, bEnableOutput, ptDevChnInfo->dwInterval);

                        if (ptDevChnInfo->dwDiffResidue >= ptDevChnInfo->dwInterval)
                        {   // if the last estimation is wrong, this input frame will be wrongly discarded and
                            // we shall get the incoming frame.
                            if (ptDevChnInfo->bHalfSize == TRUE)
                            {
                                writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(ptDevChnInfo->dwCapHeight<<16), &ptChnInfo->dwCapV);
                                ptDevChnInfo->bHalfSize = FALSE;
                            }
                            dwVICMmrCtrl = dwVICMmrCtrl | 0x00000100;
                        }
                        else if ((ptDevChnInfo->dwDiffResidue+dwDiffTicks) >= ptDevChnInfo->dwInterval)
                        {   // estimate if next frame needs to be written to DRAM.
                            if (ptDevChnInfo->bHalfSize == TRUE)
                            {
                                writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(ptDevChnInfo->dwCapHeight<<16), &ptChnInfo->dwCapV);
                                ptDevChnInfo->bHalfSize = FALSE;
                            }
                            dwVICMmrCtrl = dwVICMmrCtrl | 0x00000100;
                        }
                        else
                        {   // the incoming frame should be discarded.
                            dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFEFF;
                        }
                    }
                }
                else
                {   // no SW frame rate control and always output frame
                    if (ptDevChnInfo->bHalfSize == TRUE)
                    {
                        writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(ptDevChnInfo->dwCapHeight<<16), &ptChnInfo->dwCapV);
                        ptDevChnInfo->bHalfSize = FALSE;
                    }
                    dwVICMmrCtrl = dwVICMmrCtrl | 0x00000100;
                }
                ptDevChnInfo->dwPrevTicks = dwCurrTicks;
            }
        }
        // clear interrupt and set interrupt ack
        // leave FIFO full ack enable intact because it might be disabled by ISR previously.
        // clear interrupt
        dwVICMmrCtrl = (dwVICMmrCtrl&0xFFFFFFF8) | 0x00000028;
        ptDevChnInfo->dwFrameCountAll++;
    }
    else
    {
        // If an error occurs, save the error status in ptDevChnInfo->dwErrAck.
        ptBufInfo->bReadyForProcess = FALSE;
        ptDevChnInfo->dwErrACK = ptDevChnInfo->dwErrACK | (dwVICMmrStat&0x1F) | 0x20;
        ptBufInfo->dwErrACK = dwVICMmrStat & 0x1F;
        ptBufInfo->dwInWidth = dwRptWidth;
        if ((dwVICMmrStat&0x17) == 0x10) // ccir656 protocol error or width check error
        //if (((dwVICMmrStat&0x17)==0x10) ||
        //	(dwRptWidth!=ptDevChnInfo->dwInWidth)) // ccir656 protocol error or width check error
        {
            if (ptDevChnInfo->bSetSize == TRUE) {
                dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFFFE;
                ptDevChnInfo->bSetSize = FALSE;
            }
            else {			
                // application should stop video capture
                ptDevChnInfo->dwErrACK = 0x30;
                ptBufInfo->dwErrACK = 0x10;
                dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFFFE;
                printk("<Ch %lu> Index: %lu, dwVICMmrStat: %08lX, dwVICMmrCtrl: %08lX (CCIR or width(%lu) chck fail)\n", dwNum, ptDevChnInfo->ptCaptureBuf->dwIndex, dwVICMmrStat, dwVICMmrCtrl, dwRptWidth);
            }
        }		
        else
        {
            // Disable the correspondent interrupt to prevent the interrupt happening again and
            // continue video capture.
            dwMask = ~((ptDevChnInfo->dwErrACK&0x07)<<3);
            PDEBUG("Current dwVICMmrCtrl: %08lX, dwErrACK: %08lX, dwMask: %08lX\n", dwVICMmrCtrl, ptDevChnInfo->dwErrACK, dwMask);
            dwTmp = dwVICMmrCtrl;
            dwVICMmrCtrl = (dwVICMmrCtrl&0xFFFFFF80) |
                           (dwMask&0x00000038) | //nosig, fifo_full, op_cmpt
                           0x00000068; //fifo_full_out_en, nosig, op_cmpt

            PDEBUG("<Ch %lu> dwVICMmrCtrl: %08lX -> %08lX, dwErrACK(Dev): %08lX, dwErrACK(Buf): %08lX, dwMask: %08lX\n", dwNum, dwTmp, dwVICMmrCtrl, ptDevChnInfo->dwErrACK, ptBufInfo->dwErrACK, dwMask);
        }
        bDevErr = TRUE;
    }

    ptBufInfo->dwOutWidth = readl(&ptChnInfo->dwCapH) >> 16;
    ptBufInfo->dwOutHeight = readl(&ptChnInfo->dwCapV) >> 16;
    if (sensor_dev->get_remaining_line_num) {
	    //ptBufInfo->dwOutHeight -= sensor_dev->get_remaining_line_num();
	    ptBufInfo->dwOutHeight -= video_sensor_remainingline;
    }
    ptBufInfo->dwOutStride = readl(&ptChnInfo->dwStride) & 0x0000FFFF;

    if (ptDevChnInfo->bStopCapture == TRUE)
    {
        /* Disable INT_EN */
        dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFF7F;
        /* Disable OUT_EN */
        dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFEFF;
        /* Clear interrupts */
        dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFF80;
        ptDevChnInfo->bStopCapture = FALSE;
        bWakeUp = TRUE;
    }
#ifdef __USE_PROFILE__
    dwMask = readl(gptDevInfo->pdwReqTimes);
    if (dwMask >= gptDevInfo->dwPrevRqTimes)
    {
        gptDevInfo->dwTotalRqTimes += (dwMask-gptDevInfo->dwPrevRqTimes);
    }
    else
    {
        gptDevInfo->dwTotalRqTimes += (65536-gptDevInfo->dwPrevRqTimes+dwMask);
    }
    gptDevInfo->dwPrevRqTimes = dwMask;
    dwMask = readl(gptDevInfo->pdwRGInterval);
    if (dwMask >= gptDevInfo->dwPrevRqGntInterval)
    {
		dwTmp = dwMask-gptDevInfo->dwPrevRqGntInterval;
    }
    else
    {
		dwTmp = 65536-gptDevInfo->dwPrevRqGntInterval+dwMask;
    }
    gptDevInfo->dwTotalRqGntInterval += dwTmp;
    gptDevInfo->dwPrevRqGntInterval = dwMask;
    dwMask = readl(gptDevInfo->pdwBandwidth);
    if (dwMask >= gptDevInfo->dwPrevBandwidth)
    {
        gptDevInfo->dwTotalBandwidth += (dwMask-gptDevInfo->dwPrevBandwidth);
    }
    else
    {
        //gptDevInfo->dwTotalBandwidth += ((DWORD)((DWORD)4294967295-gptDevInfo->dwPrevBandwidth)+dwMask+1);
        gptDevInfo->dwTotalBandwidth += ((~gptDevInfo->dwPrevBandwidth)+dwMask+1);
    }
    gptDevInfo->dwPrevBandwidth = dwMask;
#endif //__USE_PROFILE__
    if (bDevErr == TRUE)
    {
        // set vic_en  to 0 to clear no-signal counter in VIC and then re-enable VIC

        dwVICMmrCtrl = dwVICMmrCtrl & 0xFFFFFBFF;
        writel(dwVICMmrCtrl, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
        dwVICMmrCtrl = dwVICMmrCtrl | 0x00000400;
    }
    writel(dwVICMmrCtrl, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);

#ifdef __USE_PROFILE__
    dwSecondTick = *(pdwTMRCVirtAddr+9);
    sdwISRInterval = dwFirstTick - dwSecondTick;
    if (dwFirstTick > dwSecondTick)
    {
        if (sdwISRInterval > gptDevInfo->dwMaxISRInterval)
        {
            gptDevInfo->dwMaxISRInterval = sdwISRInterval;
        }
        if (sdwISRInterval < gptDevInfo->dwMinISRInterval)
        {
            gptDevInfo->dwMinISRInterval = sdwISRInterval;
        }
    }
    else
    {
        sdwISRInterval = dwMaxTick + sdwISRInterval;
        if (sdwISRInterval > gptDevInfo->dwMaxISRInterval)
        {
            gptDevInfo->dwMaxISRInterval = sdwISRInterval;
        }
        if (sdwISRInterval < gptDevInfo->dwMinISRInterval)
        {
            gptDevInfo->dwMinISRInterval = sdwISRInterval;
        }
    }
#endif //__USE_PROFILE__
    if (bDevErr == TRUE)
    {
        // once bDevErr, wake up all sleep processes
        for (i=0; i<MAX_VIC_OPEN_NUM_DEFAULT; i++)
        {
            ptDevChnInfo->abIntFlag[i] = TRUE;
        }
        ptDevChnInfo->bSetOption = FALSE;
        ptDevChnInfo->bStopCapture = FALSE;
        bWakeUp = TRUE;
    }
    if ((ptDevChnInfo->bStartOutput==TRUE) && (bEnableOutput==TRUE))
    {
        // wake up master process
        ptDevChnInfo->abIntFlag[0] = TRUE;
        bWakeUp = TRUE;
    }
    if (bWakeUp == TRUE)
    {
        wake_up_interruptible(&ptDevChnInfo->wq);
    }

    // auto focus
    if ((!bDarkFrame) && (!gptDevInfo->bAFOneFrameOnGoing) && (ptDevChnInfo->eFormat==VIDEO_SIGNAL_FORMAT_BAYER_PATTERN))
    {
        gptDevInfo->bAFOneFrameOnGoing = TRUE; // start to do AF      
        schedule_work(&gptDevInfo->af_oneframe_work);                    
    }

    // ae & awb
    if ((!bDarkFrame) && (!gptDevInfo->bAEWBOneFrameOnGoing) && ((ptDevChnInfo->eFormat==VIDEO_SIGNAL_FORMAT_BAYER_PATTERN) || bGetBayerRawEn))
    {
        gptDevInfo->bAEWBOneFrameOnGoing = TRUE;
        schedule_work(&gptDevInfo->aewb_oneframe_work);
    }

    if (sensor_dev->detect_standard)
    {
        if (gptDevInfo->bAutoDetectOnGoing == FALSE)
        {
            gptDevInfo->bAutoDetectOnGoing = TRUE;
            schedule_work(&gptDevInfo->auto_detect_work);
        }
    }
    PDEBUG("Exit device %lu ISR function !!\n", dwNum);
    return IRQ_HANDLED;
}

/* ============================================================================================== */
void VIC_AEWBOneframeWQ(unsigned long dummy) 
{
	TVideoSignalOptions sensor_options;
	TVPLVICDevChnInfo *ptDevChnInfo;
	TAutoExposureState ae_oneframe_info;
	DWORD dwNum = gptDevInfo->dwNum;	
	DWORD dwStatWinSize, dwBlackClamp;
	TVideoSensorStatistic ptAEWBStatisticInfo;
/* Version 11.0.0.2 modification, 2012.10.24 */
	DWORD dwSensorMaxGain, dwISPGain = 1024;
	static DWORD dwPreISPGain = 1024;
/* ========================================= */
	int ret = 0;
	TAutoWhiteBalanceState awb_oneframe_info;
/* Version 11.0.0.4 modification, 2012.11.22 */
	DWORD dwBayerBitWidth, dwDownShift, dwCFAPatternShift;
	BOOL byCFAPatternVShift, byCFAPatternHShift;
/* ======================================== */
/* Version 12.0.0.0 modification, 2013.04.19 */
	DWORD dwIndex;
	DWORD *pdwHistoBuff;
/* ======================================== */

	if ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x100) == 0x100) { // VIC output is enalbed
		ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];

		memcpy(gptDevInfo->pdwStatAEWBBuf, (DWORD *)(gptDevInfo->pdwStatBuff+16), sizeof(DWORD)*240);
		memcpy((gptDevInfo->pdwStatAEWBBuf+240), (DWORD *)(gptDevInfo->pdwStatBuff+272), sizeof(DWORD)*240);
		memcpy((gptDevInfo->pdwStatAEWBBuf+480), (DWORD *)(gptDevInfo->pdwStatBuff+528), sizeof(DWORD)*240);

/* Version 12.0.0.0 modification, 2013.04.19 */
		pdwHistoBuff = (DWORD *)(gptDevInfo->pdwStatBuff+768);
		for (dwIndex=0; dwIndex<96; dwIndex++)
		{
			gptDevInfo->pdwStatHistoBuf[2*dwIndex] = pdwHistoBuff[2*dwIndex+1];
			gptDevInfo->pdwStatHistoBuf[2*dwIndex+1] = pdwHistoBuff[2*dwIndex];
		}
/* ======================================== */

		if (!gptDevInfo->bAFOneFrameOnGoing)
		{
			if ((bAEEn==TRUE) || (bIrisEn==TRUE))
			{
				if (sensor_dev->get_exp_statistic)
				{
					sensor_dev->get_exp_statistic(&ptAEWBStatisticInfo, dwNum);

					ae_oneframe_info.dwLumaHorSubWindowNum = ptAEWBStatisticInfo.dwLumaHorSubWindowNum;
					ae_oneframe_info.dwLumaVerSubWindowNum = ptAEWBStatisticInfo.dwLumaVerSubWindowNum;
					ae_oneframe_info.dwLumaWinSize = 1;
					ae_oneframe_info.pdwStatisticY = ptAEWBStatisticInfo.pdwStatisticY;

					ae_oneframe_info.dwWBHorSubWindowNum = ptAEWBStatisticInfo.dwWBHorSubWindowNum;
					ae_oneframe_info.dwWBVerSubWindowNum = ptAEWBStatisticInfo.dwWBVerSubWindowNum;
					ae_oneframe_info.dwWBWinSize = 1;
					ae_oneframe_info.pdwStatisticR = ptAEWBStatisticInfo.pdwStatisticR;
					ae_oneframe_info.pdwStatisticG = ptAEWBStatisticInfo.pdwStatisticG;
					ae_oneframe_info.pdwStatisticB = ptAEWBStatisticInfo.pdwStatisticB;

					ae_oneframe_info.dwHistogramBinNum = ptAEWBStatisticInfo.dwHistogramBinNum;
					ae_oneframe_info.pdwHistogram = ptAEWBStatisticInfo.pdwHistogram;
				}
				else
				{
					ae_oneframe_info.dwLumaHorSubWindowNum = 0;
					ae_oneframe_info.dwLumaVerSubWindowNum = 0;
					ae_oneframe_info.dwLumaWinSize = 0;
					ae_oneframe_info.pdwStatisticY = NULL;

					ae_oneframe_info.dwWBHorSubWindowNum = 16;
					ae_oneframe_info.dwWBVerSubWindowNum = 15;

					dwStatWinSize = readl(&gptDevInfo->ptMMRInfo->dwAWBWindowRange);
/* Version 11.0.0.2 modification, 2012.10.24 */
					ae_oneframe_info.dwWBWinSize = ((dwStatWinSize&0x1FFF)*((dwStatWinSize>>16)&0x1FFF)>>8);
/* ========================================= */

					ae_oneframe_info.pdwStatisticR = gptDevInfo->pdwStatAEWBBuf;
					ae_oneframe_info.pdwStatisticG = gptDevInfo->pdwStatAEWBBuf + 240;
					ae_oneframe_info.pdwStatisticB = gptDevInfo->pdwStatAEWBBuf + 480;

					ae_oneframe_info.dwHistogramBinNum = 64;
					ae_oneframe_info.pdwHistogram = gptDevInfo->pdwStatHistoBuf;
				}
				ae_oneframe_info.dwISPGain = dwPreISPGain;

				ae_dev->one_frame(&ae_oneframe_info);
				// 0-0. make sure any AE access or not
				if ((ae_oneframe_info.bShutterValid == 1) || (ae_oneframe_info.bGainValid == 1))
				{
					// 0-1. initiate group access (for sensors which support group access) 
					if (sensor_dev->group_access) {
						sensor_dev->group_access(0);
					}
					// 0-2. set shutter/gain
					if (ae_oneframe_info.bShutterValid) //Set shutter
					{
						sensor_options.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL;
						sensor_options.adwUserData[0] = ae_oneframe_info.dwCurrShutter;
						sensor_dev->ioctl(&sensor_options, dwNum);
						if (sensor_dev->get_shutter_value) {
							ptDevChnInfo->dwFrameNum = sensor_dev->get_shutter_value();
						}
						ptDevChnInfo->dwShutter = ae_oneframe_info.dwCurrShutter;
					}
					if (ae_oneframe_info.bGainValid) //Set global gain
					{
						sensor_options.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL;
						sensor_options.adwUserData[0] = ae_oneframe_info.dwCurrGain;
						sensor_dev->ioctl(&sensor_options, dwNum);
						ptDevChnInfo->dwGain = ae_oneframe_info.dwCurrGain;

						if (sensor_dev->get_max_gain)
						{
							dwSensorMaxGain = sensor_dev->get_max_gain();
/* Version 11.0.0.2 modification, 2012.10.24 */
							if (ae_oneframe_info.dwCurrGain > dwSensorMaxGain)
							{
								dwISPGain = (ae_oneframe_info.dwCurrGain<<10) / dwSensorMaxGain;
								dwISPGain = (dwISPGain<=1024) ? 1024 : (dwISPGain>=16320) ? 16320 : dwISPGain;
							}

							dwPreISPGain = dwISPGain;
							dwISPGain = ((dwISPGain>>6)&0xFF);
/* ========================================= */
							writel(((dwISPGain<<24)|(dwISPGain<<16)|(dwISPGain<<8)|(dwISPGain)), &gptDevInfo->ptMMRInfo->dwBayerGain);
						}
					}
					// 0-3. launch group access (for sensors which support group access) 
					if (sensor_dev->group_access) {
						sensor_dev->group_access(1);
					}
				}
				// 1-0. or any DC-Iris access
/* Version 11.0.0.1 modification, 2012.09.21 */
				if (ae_oneframe_info.bIrisCtrlValid)
				{
					// 1-1. control DCIris
					if (iris_motor_dev)
						iris_motor_dev->control(ae_oneframe_info.eIrisCtrlCmd, (DWORD)(&ae_oneframe_info.tIrisCtrlInfo));
				}
/* ========================================= */
				ptDevChnInfo->bAEStable = ae_oneframe_info.bAEStable;
				ptDevChnInfo->dwCurrentEV = ae_oneframe_info.dwCurrentEV;
			}

			if (bAWBEn == TRUE)
			{
				if (sensor_dev->get_wb_statistic)
				{
					sensor_dev->get_wb_statistic(&ptAEWBStatisticInfo, dwNum);

					awb_oneframe_info.dwWBHorSubWindowNum = ptAEWBStatisticInfo.dwWBHorSubWindowNum;
					awb_oneframe_info.dwWBVerSubWindowNum = ptAEWBStatisticInfo.dwWBVerSubWindowNum;

					awb_oneframe_info.dwWBWinSize = 1;

					awb_oneframe_info.pdwStatisticR = ptAEWBStatisticInfo.pdwStatisticR;
					awb_oneframe_info.pdwStatisticG = ptAEWBStatisticInfo.pdwStatisticG;
					awb_oneframe_info.pdwStatisticB = ptAEWBStatisticInfo.pdwStatisticB;

/* Version 11.0.0.2 modification, 2012.10.24 */
					awb_oneframe_info.dwBlackClampR = 0;
					awb_oneframe_info.dwBlackClampG = 0;
					awb_oneframe_info.dwBlackClampB = 0;
/* ========================================= */
				}
				else
				{	
					awb_oneframe_info.dwWBHorSubWindowNum = 16;
					awb_oneframe_info.dwWBVerSubWindowNum = 15;

					dwStatWinSize = readl(&gptDevInfo->ptMMRInfo->dwAWBWindowRange);
/* Version 11.0.0.2 modification, 2012.10.24 */
					awb_oneframe_info.dwWBWinSize = (((dwStatWinSize&0x1FFF)*((dwStatWinSize>>16)&0x1FFF))>>8);
/* ========================================= */
					awb_oneframe_info.pdwStatisticR = gptDevInfo->pdwStatAEWBBuf;
					awb_oneframe_info.pdwStatisticG = gptDevInfo->pdwStatAEWBBuf + 240;
					awb_oneframe_info.pdwStatisticB = gptDevInfo->pdwStatAEWBBuf + 480;

/* Version 11.0.0.2 modification, 2012.10.24 */
					dwBlackClamp = readl(&gptDevInfo->ptMMRInfo->dwBayerClamp);
					dwBayerBitWidth = ((readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl)>>16)&0x1F);				
					dwDownShift = (dwBayerBitWidth>12) ? (dwBayerBitWidth-12) : 0;

/* Version 11.0.0.4 modification, 2012.11.22 */
					dwCFAPatternShift = readl(&gptDevInfo->ptMMRInfo->dwCMYGCtrl0);
					byCFAPatternVShift = ((dwCFAPatternShift>>2)&0x01);
					byCFAPatternHShift = (dwCFAPatternShift&0x01);
					if ((!byCFAPatternVShift) && (!byCFAPatternHShift))
					{
						awb_oneframe_info.dwBlackClampR = ((dwBlackClamp>>16)&0xFF)>>dwDownShift;
						awb_oneframe_info.dwBlackClampG = ((((dwBlackClamp>>24)&0xFF)+(dwBlackClamp&0xFF))>>1)>>dwDownShift;
						awb_oneframe_info.dwBlackClampB = ((dwBlackClamp>>8)&0xFF)>>dwDownShift;
					} 
					else if ((byCFAPatternVShift)&&(!byCFAPatternHShift))
					{
						awb_oneframe_info.dwBlackClampR = (dwBlackClamp&0xFF)>>dwDownShift;
						awb_oneframe_info.dwBlackClampG = ((((dwBlackClamp>>8)&0xFF)+((dwBlackClamp>>16)&0xFF))>>1)>>dwDownShift;
						awb_oneframe_info.dwBlackClampB = ((dwBlackClamp>>24)&0xFF)>>dwDownShift;
					}
					else if ((!byCFAPatternVShift)&&(byCFAPatternHShift))
					{
						awb_oneframe_info.dwBlackClampR = ((dwBlackClamp>>24)&0xFF)>>dwDownShift;
						awb_oneframe_info.dwBlackClampG = ((((dwBlackClamp>>16)&0xFF)+((dwBlackClamp>>8)&0xFF))>>1)>>dwDownShift;
						awb_oneframe_info.dwBlackClampB = (dwBlackClamp&0xFF)>>dwDownShift;
					}
					else
					{
						awb_oneframe_info.dwBlackClampR = ((dwBlackClamp>>8)&0xFF)>>dwDownShift;
						awb_oneframe_info.dwBlackClampG = (((dwBlackClamp&0xFF)+((dwBlackClamp>>24)&0xFF))>>1)>>dwDownShift;
						awb_oneframe_info.dwBlackClampB = ((dwBlackClamp>>16)&0xFF)>>dwDownShift;
					}
/* ======================================== */
/* ========================================= */
				}
				awb_oneframe_info.dwShutter = ptDevChnInfo->dwShutter; 
				awb_oneframe_info.dwGain = ptDevChnInfo->dwGain; 
				awb_oneframe_info.dwCurrentEV = ptDevChnInfo->dwCurrentEV;
/* Version 11.0.0.2 modification, 2012.10.24 */
				awb_oneframe_info.bAEStable = ptDevChnInfo->bAEStable;
/* ========================================= */
/* Version 12.0.0.1 modification, 2013.06.21 */
				awb_oneframe_info.dwISPGain = dwPreISPGain;
/* ======================================== */

				awb_dev->one_frame(&awb_oneframe_info);

				// make sure AWB access or not
				if (awb_oneframe_info.bAWBSetValid == TRUE)
				{
		
					sensor_options.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL;
					sensor_options.adwUserData[0] = awb_oneframe_info.dwCurrGainR;
					sensor_options.adwUserData[1] = awb_oneframe_info.dwCurrGainB;

					ret = sensor_dev->ioctl(&sensor_options, dwNum);

					if (ret != S_OK)
					{
						writel((readl(&gptDevInfo->ptMMRInfo->dwManualGain)&0xE000E000)|((awb_oneframe_info.dwCurrGainR&0x1FFF)<<16)|(awb_oneframe_info.dwCurrGainB&0x1FFF), &gptDevInfo->ptMMRInfo->dwManualGain);
					}
				}
			}
		}
	}
	gptDevInfo->bAEWBOneFrameOnGoing = FALSE;
}

/* ============================================================================================== */
void VIC_AutoFocusOneframeWQ(DWORD dummy) 
{
	TVPLVICDevChnInfo *ptDevChnInfo;
	TAutoFocusState af_oneframe_info;
	DWORD dwNum = gptDevInfo->dwNum;	
	DWORD dwStatWinSize;
	DWORD dwFocusPosition = 0, dwZoomPosition = 0;
	DWORD *pdwFocusValue;

	DWORD dwIndexX, dwIndexY, dwStartLine = 0;
	QWORD qwMaxSDA = 0, qwFV;
	DWORD adwMaxSDAIdx[8] = {0};

	if ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x100) == 0x100) // VIC output is enalbed
	{
		ptDevChnInfo = &gptDevInfo->atDevChnInfo[gptDevInfo->dwNum];

		pdwFocusValue = gptDevInfo->pdwStatBuff + 960;
		for (dwIndexX=0; dwIndexX<8; dwIndexX++)
		{
			qwMaxSDA = 0;
			for (dwIndexY=0; dwIndexY<8; dwIndexY++)
			{
				qwFV = (QWORD)(pdwFocusValue[2*(8*dwIndexY+dwIndexX)+1])<<32 | (pdwFocusValue[2*(8*dwIndexY+dwIndexX)]);			
				if (qwFV >= qwMaxSDA)
				{
					qwMaxSDA = qwFV;
					adwMaxSDAIdx[dwIndexX] = dwIndexY;
				}
			}
		}

		if ((adwMaxSDAIdx[0]==adwMaxSDAIdx[1])&&(adwMaxSDAIdx[1]==adwMaxSDAIdx[2])&&
			(adwMaxSDAIdx[2]==adwMaxSDAIdx[3])&&(adwMaxSDAIdx[3]==adwMaxSDAIdx[4])&&
			(adwMaxSDAIdx[4]==adwMaxSDAIdx[5])&&(adwMaxSDAIdx[5]==adwMaxSDAIdx[6])&&
			(adwMaxSDAIdx[6]==adwMaxSDAIdx[7]))
		{
			dwStartLine = adwMaxSDAIdx[0]+1;
		}

		dwStartLine = (dwStartLine<=1) ? 1 : (dwStartLine>=8) ? 8 : dwStartLine;

		pdwFocusValue = gptDevInfo->pdwStatBuff + 960 + (dwStartLine*16);
		for (dwIndexX=0; dwIndexX<(8-dwStartLine); dwIndexX++)
		{
			memcpy((gptDevInfo->pdwStatFocusBuf+14*dwIndexX), (DWORD *)(pdwFocusValue+(16*dwIndexX+2)), sizeof(DWORD)*14);
		}

		pdwFocusValue = gptDevInfo->pdwStatBuff + 960;
		for (dwIndexY=0; dwIndexY<(dwStartLine-1); dwIndexY++)
		{
			memcpy((gptDevInfo->pdwStatFocusBuf+14*(dwIndexX+dwIndexY)), (DWORD *)(pdwFocusValue+(16*dwIndexY+2)), sizeof(DWORD)*14);
		}

		// auto focus
		if (bAFEn)
		{
			switch (ptDevChnInfo->dwAFState)
			{
				case AF_STATECTRL_WAITE: // wait state
				if ((!gptDevInfo->bAEWBOneFrameOnGoing) && (ptDevChnInfo->bAEStable))
				{
					ptDevChnInfo->dwAFState = AF_STATECTRL_AF; // AF state
				}
				break;			

				case AF_STATECTRL_AF: // AF state
				case AF_STATECTRL_MF: // manual focus state
				case AF_STATECTRL_ZOOM: // zoom control state
				break;

				case AF_STATECTRL_ADJUST:
				if ((gptDevInfo->bAEWBOneFrameOnGoing) || (!ptDevChnInfo->bAEStable))
				{
					gptDevInfo->bAFOneFrameOnGoing = FALSE;	
					return;
				}			
				default: //idle
					gptDevInfo->bAFOneFrameOnGoing = FALSE;	
					return;
				break;
			}

			af_oneframe_info.dwFocusHorSubWindowNum = 7;
			af_oneframe_info.dwFocusVerSubWindowNum = 7;

			dwStatWinSize = readl(&gptDevInfo->ptMMRInfo->dwAFWindowSize);
/* Version 11.0.0.2 modification, 2012.10.24 */
			af_oneframe_info.dwFocusWinSize = ((dwStatWinSize&0x1FFF)*((dwStatWinSize>>16)&0x1FFF)>>6);
/* ========================================= */

			af_oneframe_info.pdwFocusValue = gptDevInfo->pdwStatFocusBuf;

			af_motor_dev->control(LENS_CTRL_FOCUS_GET_POSITION, (DWORD)(&(dwFocusPosition)));
			af_oneframe_info.dwFocusMotorPosition = dwFocusPosition;
			af_motor_dev->control(LENS_CTRL_ZOOM_GET_POSITION, (DWORD)(&(dwZoomPosition)));
			af_oneframe_info.dwZoomMotorPosition = dwZoomPosition;

			af_oneframe_info.bFocusFinish = FALSE;

			af_oneframe_info.dwAFUserCMD = ptDevChnInfo->dwAFState;

/* Version 11.0.0.1 modification, 2012.09.21 */
			af_oneframe_info.tAFCtrlInfo.dwFocusDirection = ptDevChnInfo->dwManualDir;
			af_oneframe_info.tAFCtrlInfo.dwZoomDirection = ptDevChnInfo->dwManualDir;
/* ========================================= */

			af_oneframe_info.bAEStable = ptDevChnInfo->bAEStable;

			af_dev->one_frame(&af_oneframe_info);

/* Version 11.0.0.1 modification, 2012.09.21 */
			if (af_oneframe_info.bAFCtrlValid)
			{
				af_motor_dev->control(af_oneframe_info.eAFCtrlCmd, (DWORD)(&af_oneframe_info.tAFCtrlInfo));
			}
/* ========================================= */
			if (ptDevChnInfo->dwAFState==AF_STATECTRL_ZOOM)
			{
				ptDevChnInfo->dwAFState = AF_STATECTRL_AF;  // back to AF state
			}

			if (af_oneframe_info.bFocusFinish == TRUE)
			{
				ptDevChnInfo->dwAFState = AF_STATECTRL_IDLE; // back to idle state
			
				af_motor_dev->control(LENS_CTRL_FOCUS_ZOOM_STOP, 0);
				ptDevChnInfo->dwZoomRatio = af_oneframe_info.dwZoomRatio;
			}
		}
	}
	gptDevInfo->bAFOneFrameOnGoing = FALSE;	
}

/* ============================================================================================== */
void VIC_DetectStd(unsigned long dwNum) 
{
    TVPLVICChnInfo *ptChnInfo;
    TVPLVICDevChnInfo *ptDevChnInfo;	
    TAutoDetectStdInfo stdInfo;

    ptChnInfo = (TVPLVICChnInfo *)&gptDevInfo->ptMMRInfo->atChnInfo[dwNum];	
    ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];	
    sensor_dev->detect_standard(dwNum, &stdInfo);

    if ((stdInfo.eDetectStd == TVSTD_PAL) && (stdInfo.bDataLost == FALSE)) { // detect PAL and NotDataLost
        // set to PAL
        if (ptDevChnInfo->eStd != TVSTD_PAL)
        {
            // setup encoder
            sensor_dev->setup_standard(dwNum, TVSTD_PAL);
            // setup VIC input size			
            writel((720&0x0000FFFF)|(576<<16), &ptChnInfo->dwInSize);					
            // setup VIC capture size			
            writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(576<<16), &ptChnInfo->dwCapV);
            // wait for update MMR			
            writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]) | 0x200, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
            while ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x200) != 0);
            // update standard
            ptDevChnInfo->eStd = TVSTD_PAL;
        }
    }
    else { // dataLost==TRUE or detect NTSC or detect UNKNOWN
        // set to NTSC
        if (ptDevChnInfo->eStd != TVSTD_NTSC)
        {    
            // setup VIC input size			
            writel((720&0x0000FFFF)|(480<<16), &ptChnInfo->dwInSize);					
            // setup VIC capture size			
            writel((readl(&ptChnInfo->dwCapV)&0x0000FFFF)|(480<<16), &ptChnInfo->dwCapV);            
            // wait for update MMR			
            writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]) | 0x200, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);			
            while ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x200)  != 0);
            // setup encoder	
            sensor_dev->setup_standard(dwNum, TVSTD_NTSC);
            // update standard
            ptDevChnInfo->eStd = TVSTD_NTSC;
        }			
    }
}

/* ============================================================================================== */
void VIC_AutoDetectStdTasklet(unsigned long dummy) 
{
    DWORD dwNum = gptDevInfo->dwNum;

    if ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x100) == 0x100) { // VIC output is enalbed
        VIC_DetectStd(dwNum);
    }
    gptDevInfo->bAutoDetectOnGoing = FALSE;
}

/* ============================================================================================== */
static SCODE WaitComplete(DWORD dwNum, DWORD dwProcIndex)
{
    TVPLVICDevChnInfo *ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];

    PDEBUG("Enter device %lu WaitComplete function...\n", dwNum);

    if (wait_event_interruptible(ptDevChnInfo->wq, (ptDevChnInfo->abIntFlag[dwProcIndex]==TRUE)))
    {
        PDEBUG("Exit WaitComplete function by signal !!\n");
        return S_FAIL;
    }
    ptDevChnInfo->abIntFlag[dwProcIndex] = FALSE;

    PDEBUG("Exit device %lu WaitComplete function !!\n", dwNum);

    return S_OK;
}

static void WaitTblComplete(DWORD dwNum)
{
    DWORD dwVICMmrStat = readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl);

    PDEBUG("Enter device %d WaitTblComplete function...\n", dwNum);
    // Polling
    while ((dwVICMmrStat&0x08)==1)
    {
        dwVICMmrStat = readl(&gptDevInfo->ptMMRInfo->dwCMOSCtrl);
    }

    PDEBUG("Exit device %d WaitTblComplete function !!\n", dwNum);
}
static void WaitCeTblComplete(DWORD dwNum)
{
    DWORD dwVICMmrStat = readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCECtrl);

    PDEBUG("Enter device %d WaitCeTblComplete function...\n", dwNum);
    // Polling
    while ((dwVICMmrStat&0x40)==0)
    {
        dwVICMmrStat = readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCECtrl);
    }

    PDEBUG("Exit device %d WaitCeTblComplete function !!\n", dwNum);
}
static void WaitPhotoLdcTblComplete(DWORD dwNum)
{
    DWORD dwVICMmrStat = readl(&gptDevInfo->ptMMRInfo->dwPhotoLDCCtrl);

    PDEBUG("Enter device %d WaitPhotoLdcTblComplete function...\n", dwNum);
    // Polling
    while ((dwVICMmrStat&0x06)!=4)
    {
        dwVICMmrStat = readl(&gptDevInfo->ptMMRInfo->dwPhotoLDCCtrl);
    }

    PDEBUG("Exit device %d WaitPhotoLdcTblComplete function !!\n", dwNum);
}

/* ============================================================================================== */
static int Close(struct inode *pinode, struct file *pfile)
{
    TVPLVICDevChnInfo *ptDevChnInfo;
    DWORD dwNum;
    DWORD dwClockRate;
    DWORD dwVICMmrCtrl;
    DWORD i, dwModuleRefcnt;
    TVideoCapBufInfo *ptBufInfo;
    TVPLVICObjInfo *ptObjInfo = (TVPLVICObjInfo *)pfile->private_data;
    DWORD *pdwSYSCVirtAddr;
#ifdef __USE_PROFILE__
    DWORD dwFrameCount;
#endif //__USE_PROFILE__

    module_put(THIS_MODULE);

    dwNum = MINOR(pinode->i_rdev);

    PDEBUG("Enter device %lu Close function...\n", dwNum);

    ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
    ptDevChnInfo->dwChnInUse = ptDevChnInfo->dwChnInUse - 1;
    dwVICMmrCtrl = readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
    if ((ptObjInfo->bIsMaster==TRUE) && (((dwVICMmrCtrl>>7)&0x01)==TRUE))
    {
        ptDevChnInfo->abIntFlag[0] = FALSE;
        wait_event_interruptible(ptDevChnInfo->wq, (ptDevChnInfo->abIntFlag[0]==TRUE));
    }
#ifdef __USE_PROFILE__
    dwFrameCount = ptDevChnInfo->dwFrameCountAll;

/* Version 11.0.0.1 modification, 2012.09.21 */
    dwModuleRefcnt = (af_dev) ? 1 : 0;
    dwModuleRefcnt = (af_motor_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (ae_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (iris_motor_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (awb_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;

    if (module_refcount(THIS_MODULE) == dwModuleRefcnt)
/* ========================================= */
    {
        printk("Max ISR interval: %lu ticks\n", gptDevInfo->dwMaxISRInterval);
        printk("Min ISR interval: %lu ticks\n", gptDevInfo->dwMinISRInterval);
    }
#endif //__USE_PROFILE__
    if (ptObjInfo != NULL)
    {
        if (ptObjInfo->dwProcIndex < MAX_VIC_OPEN_NUM_DEFAULT)
        {
            PDEBUG("<close>: IntFlagUsed: %d, ProcIndex: %d\n", ptDevChnInfo->dwIntFlagUsed, ptObjInfo->dwProcIndex);
            ptDevChnInfo->dwIntFlagUsed = ptDevChnInfo->dwIntFlagUsed & (~(1<<ptObjInfo->dwProcIndex));
            PDEBUG("<close>: IntFlagUsed: %d\n", ptDevChnInfo->dwIntFlagUsed);
            // wake up all slave process when master process is terminated.
            if (ptObjInfo->bIsMaster == TRUE)
            {
                for (i=1; i<MAX_VIC_OPEN_NUM_DEFAULT; i++)
                {
                    ptDevChnInfo->abIntFlag[i] = TRUE;
                }
            }
            ptDevChnInfo->abIntFlag[ptObjInfo->dwProcIndex] = TRUE;
            wake_up_interruptible(&ptDevChnInfo->wq);
        }

        for (i=0; i<ptObjInfo->dwBufNum; i++)
        {
            ptBufInfo = ptObjInfo->aptProcBuf[i];
            if (ptBufInfo != NULL)
            {
                if (ptBufInfo->dwInUseNum > 0)
                {
                    ptBufInfo->dwInUseNum--;
                }
            }
        }
        kfree(ptObjInfo);
    }
    if (ptDevChnInfo->dwChnInUse == 1)
    {
        // in case the buffer is not released when closing
        for (i=0; i<ptDevChnInfo->dwBufNum; i++)
        {
            ptDevChnInfo->ptBufInfo[i].bReadyForCapture = TRUE;
        }
        PDEBUG("Only one user left for channel %lu !!\n", dwNum);
    }

    if (ptDevChnInfo->dwChnInUse == 0)
    {
        if (ptDevChnInfo->ptBufInfo!=NULL)
        {
            kfree(ptDevChnInfo->ptBufInfo);
        }
        VIC_DisableOutput(dwNum);
        Intr_Disable(dwNum);
        Intr_Clear(dwNum);
        PDEBUG("No user for channel %lu !!\n", dwNum);
        ptDevChnInfo->bSetOption = FALSE;
        ptDevChnInfo->bInitialized = FALSE;
        ptDevChnInfo->bStopCapture = FALSE;
    }

    PDEBUG("Exit device %lu Close function !!\n", dwNum);
    if (ptObjInfo->bIsMaster == TRUE) {
        if (bAEEn || bIrisEn || bAWBEn)
        {
            while (gptDevInfo->bAEWBOneFrameOnGoing)
            {
                msleep(1); // wait until AEWB one frame processing over
            };
        }
        sensor_dev->release(dwNum);
    } // 	if (ptObjInfo->bIsMaster == TRUE)

/* Version 11.0.0.1 modification, 2012.09.21 */
    dwModuleRefcnt = (af_dev) ? 1 : 0;
    dwModuleRefcnt = (af_motor_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (ae_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (iris_motor_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (awb_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
 
    if (module_refcount(THIS_MODULE) == dwModuleRefcnt)
/* ========================================= */
    {
        PDEBUG("No device is in use.\n");
        if (gptDevInfo->dwIrq != (DWORD)NULL)
        {
            free_irq(VPL_VIC_IRQ_NUM, gptDevInfo);
            PDEBUG("Release IRQ %d.\n", VPL_VIC_IRQ_NUM);
        }
        gptDevInfo->dwIrq = (DWORD)NULL;
        VIC_Disable();
        dwVICMmrCtrl = readl(&gptDevInfo->ptMMRInfo->dwCtrl);
        dwClockRate = ((readl(&gptDevInfo->ptMMRInfo->dwStat1))>>24);
        printk("Max Rq-Grant interval: %08lX\n", dwClockRate);
        dwVICMmrCtrl  = dwVICMmrCtrl | 0x80000000; // clear max rq-gnt interval
        writel(dwVICMmrCtrl, &gptDevInfo->ptMMRInfo->dwCtrl);
        dwClockRate = ((readl(&gptDevInfo->ptMMRInfo->dwStat1))>>24);
        printk("Max Rq-Grant interval: %08lX\n", dwClockRate);
#ifdef __USE_PROFILE__
        printk("Frame Count: %lu\n", dwFrameCount);
        printk("RqTimes: %08lX\n", gptDevInfo->dwTotalRqTimes);
        printk("Bandwidth: %08lX\n", gptDevInfo->dwTotalBandwidth);
        printk("Rq-Grant interval: %08lX\n", gptDevInfo->dwTotalRqGntInterval);
#endif //__USE_PROFILE__
        pdwSYSCVirtAddr = (DWORD *)IO_ADDRESS(VPL_SYSC_MMR_BASE);
        if (bAEEn || bIrisEn) {
            ae_dev->release();
        }
/* Version 11.0.0.1 modification, 2012.09.21 */
        if (bIrisEn) {
            if (iris_motor_dev)
                iris_motor_dev->release();
/* ========================================= */
        }
        if (bAFEn)
        {
            while (gptDevInfo->bAFOneFrameOnGoing)
            {
                msleep(1); // wait until AF one frame processing over
            }
            af_dev->release();
            af_motor_dev->release();
        }

        if (bAWBEn)
        {
            awb_dev->release();
        }
    }

    return 0;
}

/* ============================================================================================== */
static void VIC_SetAFInitialParams(TAutoFocusInitOptions* AF_initial_param)
{
	AF_initial_param->dwFrameIntv = dwFrameIntv;
	AF_initial_param->dwThresNoise = dwThresNoise;
	AF_initial_param->dwInitialStep = dwInitialStep;
	AF_initial_param->dwMaxReturnNum = dwMaxReturnNum;
	AF_initial_param->dwFineTuneFinishThresh = dwFineTuneFinishThresh;
	AF_initial_param->dwRestartUpperDiff = dwRestartUpperDiff;
	AF_initial_param->dwRestartLowerDiff = dwRestartLowerDiff;
	AF_initial_param->dwZtTbSize = 0;

	// synchronising with lens motor
	af_motor_dev->control(LENS_CTRL_FOCUS_GET_POSITION, (DWORD)(&(AF_initial_param->dwInitialFocalPlaneIndx)));
	af_motor_dev->control(LENS_CTRL_ZOOM_GET_POSITION, (DWORD)(&(AF_initial_param->dwInitialZoomMotorPosition)));
	af_motor_dev->control(LENS_CTRL_FOCUS_GET_RANGE, (DWORD)(&(AF_initial_param->dwFocalPlaneTotalNumber)));
	af_motor_dev->control(LENS_CTRL_ZOOM_GET_RANGE, (DWORD)(&(AF_initial_param->dwZoomPlaneTotalNumber)));
	af_motor_dev->control(LENS_CTRL_ZOOM_GET_MAXRATIO, (DWORD)(&(AF_initial_param->dwZoomMaxRatio)));
}

/* ============================================================================================== */
static void VIC_SetAEInitialParams(struct auto_exposure_initial_param* AE_initial_param)
{
	/* AE Numbers of AE operating every N frames (SWAEMode) */
	AE_initial_param->dwAEopframes = dwAEopframes;	
	/* AE Target luminace (SWAEMode) */
	AE_initial_param->dwTargetLuminance = dwTargetLuminance;
	/* AE Luminace offset (SWAEMode) */
	AE_initial_param->dwTargetOffset = dwTargetOffset; // should be greater than FINE_TUNE_RANGE (5)
	/* AE Shutter default width */
	AE_initial_param->dwDefaultShutter = dwDefaultShutter;
	/* AE Gain default value (SWAEMode: related to DefaultGainIdx) */
	AE_initial_param->dwDefaultGain = dwDefaultGain;
	/* AE Shutter width lower bound (SWAEMode: related to MinShutterIdx), 0 stands for "lower bound don't care" */
	AE_initial_param->dwMinShutter = dwMinShutter;
	/* AE Shutter width upper bound (SWAEMode: related to MaxShutterIdx), 1/2/4/8/15/30/60/120 */
	AE_initial_param->dwMaxShutter = dwMaxShutter;
	/* AE Analog gain lower bound (SWAEMode: MinGainIdx), 1-32 */
	AE_initial_param->dwMinGain = dwMinGain;
	/* AE Analog gain upper bound (SWAEMode: MaxGainIdx), 1-32 */
	AE_initial_param->dwMaxGain = dwMaxGain;
	/* AE cooperation scheme
			shutter & gain | iris
		0:  on			   | on 
		1:  on			   | off
		2:  off			   | on */
	AE_initial_param->dwAECoopMode = (!bIrisEn) | ((!bAEEn)<<1);

	/* Identify sensor IIC setting timming must Sync Vsync Falling */
	AE_initial_param->bIICSyncVsyncFallingEn = bIICSyncVsyncFallingEn;
	AE_initial_param->dwIrisCtlType = dwIrisCtlType;

/* Version 11.0.0.1 modification, 2012.09.21 */
	// synchronising position if using P iris
	if ((dwIrisCtlType==2)&&(iris_motor_dev))
	{
		iris_motor_dev->control(LENS_CTRL_IRIS_GET_RANGE, (DWORD)(&(AE_initial_param->dwPIrisTotalStep)));
		iris_motor_dev->control(LENS_CTRL_IRIS_GET_POSITION, (DWORD)(&(AE_initial_param->dwPIrisCurStep)));
	}
/* ========================================= */
}

/* ============================================================================================== */
static void VIC_SetAWBInitialParams(TAutoWhiteBalanceInitialParam* AWB_initial_param)
{
/* Version 11.0.0.2 modification, 2012.10.24 */
	AWB_initial_param->dwAWBInitMode = dwAWBInitMode;
	AWB_initial_param->dwAWBOpFrames = dwAWBopframes;
/* ========================================= */
}

/* ============================================================================================== */
static void VIC_SetAWBColorTemperature(DWORD dwArg)
{
	DWORD adwCTTbl[10];
	if (bAWBEn)
	{
		copy_from_user(adwCTTbl, (DWORD *)dwArg, sizeof(DWORD)*10);
		if (awb_dev->set_color_temperature)	
			awb_dev->set_color_temperature(adwCTTbl);
	}
}

/* ============================================================================================== */
static int Open(struct inode *pinode, struct file *pfile)
{
    TVPLVICObjInfo *ptObjInfo;
    TVPLVICDevChnInfo *ptDevChnInfo;
    int scResult;
    DWORD dwNum;
    DWORD i, dwModuleRefcnt;
    struct video_sensor_initial_param sensor_initial_param;
    struct auto_exposure_initial_param AE_initial_param;
    struct autofocus_init_options af_initial_param;
    struct auto_white_Balance_initial_param awb_initial_param;
    dwNum = MINOR(pinode->i_rdev);

    PDEBUG("Enter device %lu Open function...\n", dwNum);

    ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];

    if (pfile->private_data == NULL)
    {
        if ((pfile->private_data=(TVPLVICObjInfo *)kmalloc(sizeof(TVPLVICObjInfo), GFP_KERNEL)) == NULL)
        {
            PDEBUG("Insufficient kernel memory space !!\n");
            return -EFAULT;
        }

        ptObjInfo = (TVPLVICObjInfo *)pfile->private_data;
        ptObjInfo->dwProcIndex = MAX_VIC_OPEN_NUM_DEFAULT;
        ptObjInfo->dwPreFrameCount = 0;
        ptObjInfo->dwBufNum = 0;

		if (ptDevChnInfo->bInitialized==TRUE)
		{
        	ptObjInfo->aptProcBuf = kmalloc (ptDevChnInfo->dwBufNum*sizeof(TVideoCapBufInfo *),  GFP_KERNEL);
        	for (i=0; i<ptDevChnInfo->dwBufNum; i++)
        	{
            	ptObjInfo->aptProcBuf[i] = NULL;
        	}
#ifndef __USE_LAST_FRAME__
			ptObjInfo->aptProcBuf[0] = ptDevChnInfo->ptProcessBuf;
        	ptObjInfo->ptCurrGetBuf = ptDevChnInfo->ptProcessBuf;
#endif
        }

        PDEBUG("<open>: IntFlagUsed: %d\n", ptDevChnInfo->dwIntFlagUsed);
        for (i=0; i<MAX_VIC_OPEN_NUM_DEFAULT; i++)
        {
            if ((ptDevChnInfo->dwIntFlagUsed&(1<<i)) == 0)
            {
                ptObjInfo->dwProcIndex = i;
                break;
            }
        }
        if (ptObjInfo->dwProcIndex < MAX_VIC_OPEN_NUM_DEFAULT)
        {
            ptDevChnInfo->dwIntFlagUsed = ptDevChnInfo->dwIntFlagUsed | (1<<ptObjInfo->dwProcIndex);
            PDEBUG("<open>: IntFlagUsed: %d, ProcIndex: %d\n", ptDevChnInfo->dwIntFlagUsed, ptObjInfo->dwProcIndex);
        }
        else
        {
            PDEBUG("Device opening has reached the maximum number !!\n");
            ptDevChnInfo->dwChnInUse = ptDevChnInfo->dwChnInUse + 1;
            // library should call Close after returnning
            return -EBUSY;
        }
    }
    else
    {
        return -EBUSY;
    }

/* Version 11.0.0.1 modification, 2012.09.21 */
    dwModuleRefcnt = (af_dev) ? 1 : 0;
    dwModuleRefcnt = (af_motor_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (ae_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (iris_motor_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
    dwModuleRefcnt = (awb_dev) ? (dwModuleRefcnt+1) : dwModuleRefcnt;
 
    if (module_refcount(THIS_MODULE) == dwModuleRefcnt)
/* ========================================= */
    {
        writel(readl(SYSC_CLK_EN_MMR)|(0x3<<VPL_VIC_CLK_EN_NUM), SYSC_CLK_EN_MMR);

        if (gptDevInfo->dwIrq == (DWORD)NULL)
        {
            fLib_SetIntTrig(VPL_VIC_IRQ_NUM, LEVEL, H_ACTIVE);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
            scResult = request_irq(VPL_VIC_IRQ_NUM, &ISR, SA_INTERRUPT, "vpl_vic", gptDevInfo);
#else
            scResult = request_irq(VPL_VIC_IRQ_NUM, &ISR, IRQF_DISABLED, "vpl_vic", gptDevInfo);
#endif

            if (scResult < 0)
            {
                PDEBUG("Cannot get irq %d !!\n", VPL_VIC_IRQ_NUM);
                ptDevChnInfo->dwChnInUse = ptDevChnInfo->dwChnInUse + 1;
                if (try_module_get(THIS_MODULE) == 0)
                {
                    return -EBUSY;
                }
                // library should call Close after returnning
                return scResult;
            }

            disable_irq(VPL_VIC_IRQ_NUM);
            enable_irq(VPL_VIC_IRQ_NUM);
            gptDevInfo->dwIrq = VPL_VIC_IRQ_NUM;
        }
#ifdef __USE_PROFILE__
        writel((0x1<<VPL_VIC_MASTER_NUM), gptDevInfo->pdwProfileClr);
        gptDevInfo->dwTotalRqTimes = 0;
        gptDevInfo->dwPrevRqTimes = 0;
        gptDevInfo->dwTotalRqGntInterval = 0;
        gptDevInfo->dwPrevRqGntInterval = 0;
        gptDevInfo->dwTotalBandwidth = 0;
        gptDevInfo->dwPrevBandwidth = 0;
        gptDevInfo->dwMaxISRInterval = 0;
        gptDevInfo->dwMinISRInterval = gsdwBusFreq >> 1;
#endif //__USE_PROFILE__
        writel((readl(&gptDevInfo->ptMMRInfo->dwCtrl)|0x80000000), &gptDevInfo->ptMMRInfo->dwCtrl); // clear max rq-gnt interval
    }

    if (ptDevChnInfo->dwChnInUse == 0)
    {
        ptDevChnInfo->bInitialized = FALSE;
        ptObjInfo->bIsMaster = TRUE;
        ptObjInfo->bInitialized = FALSE;
    }
    else
    {
        ptObjInfo->bIsMaster = FALSE;
        ptObjInfo->bInitialized = FALSE;
    }

    ptDevChnInfo->dwChnInUse = ptDevChnInfo->dwChnInUse + 1;
    ptDevChnInfo->abIntFlag[ptObjInfo->dwProcIndex] = FALSE;

    if (try_module_get(THIS_MODULE) == 0)
    {
        return -EBUSY;
    }

    if (ptObjInfo->bIsMaster == TRUE) 
    {
        // open sensor device
        sensor_initial_param.dwVideoSensorFrequency = video_sensor_frequency;
        sensor_initial_param.dwVideoSensorVersion = SENSOR_INTERFACE_VERSION;
        if (sensor_dev->open(&sensor_initial_param, dwNum) != 0)
        {
            printk("Open Sensor Device Failed!\n");
            return -ENODEV;
        }
        if (bAEEn || bIrisEn) {
/* Version 11.0.0.1 modification, 2012.09.21 */
            if (bIrisEn) {
                if (iris_motor_dev)
                    iris_motor_dev->open();
/* ========================================= */
            }

/* Version 11.0.0.1 modification, 2012.09.21 */
            if (ae_dev == NULL)
            {
                printk("Open AE Device Failed!\n");
                return -ENODEV;
            }
/* ========================================= */
            /* 1. set AE initial params */
            VIC_SetAEInitialParams(&AE_initial_param);
            /* 3. finally, initiate AE */
            if (ae_dev->open(&AE_initial_param) != 0)
            {
                printk("Open AE Device Failed!\n");
                return -ENODEV;
            }
        }
        if (bAFEn) {
/* Version 11.0.0.1 modification, 2012.09.21 */
            if ((af_motor_dev==NULL) || (af_dev==NULL))
            {
                printk("Open AF Motor Device Failed!\n");
                return -ENODEV;
            }
/* ========================================= */
            if (af_motor_dev->open() < 0) {
                printk("Open AF Motor Device Failed!\n");
                return -ENODEV;
            }
            VIC_SetAFInitialParams(&af_initial_param);
            if (af_dev->open(&af_initial_param) != 0)
            {
                printk("Open AF Motor Device Failed!\n");
                return -ENODEV;				
            }
            dwZtTbSize = af_initial_param.dwZtTbSize;
            ptDevChnInfo->dwAFState = AF_STATECTRL_WAITE;			
            ptDevChnInfo->dwManualDir = LENS_NO_MOVE;
        }
        if (bAWBEn) {
/* Version 11.0.0.1 modification, 2012.09.21 */
            if (awb_dev == NULL)
            {
                printk("Open AWB Device Failed!\n");
                return -ENODEV;
            }
/* ========================================= */
            VIC_SetAWBInitialParams(&awb_initial_param);
            if (awb_dev->open(&awb_initial_param) != 0)
            {
                printk("Open AWB Device Failed!\n");
                return -ENODEV;				
            }
        }        
            //for auto detec standard
            if (sensor_dev->detect_standard) {
                ptDevChnInfo->eStd = TVSTD_UNKNOWN; // initial 			
            }
    }// if (ptObjInfo->bIsMaster == TRUE)
    PDEBUG("Exit device %lu Open function !!\n", dwNum);
    printk("[VIC] : open success!!\n");
    return 0;
}

/* ============================================================================================== */
static int VIC_AutoExposureSetOptions(TVideoSignalOptions ae_options, DWORD dwNum)
{
	int ret = 0;
/* Version 12.0.0.0 modification, 2013.04.19 */
	DWORD adwAEParamTbl[11];	
/* ======================================== */
	TVPLVICDevChnInfo *ptDevChnInfo;
	ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
	
	switch (ae_options.eOptionFlags)
	{
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN:    
	case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
	case VIDEO_SIGNAL_OPTION_SET_WDR:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MODE:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_EN:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_SPEED:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_ACTIVE_TIME:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOW_PRIORITY:
		ret = ae_dev->set_options(&ae_options);
		break;
	case VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE:
		if (ptDevChnInfo->tVPLVICISPParam.bAECtrl)
		{
			adwAEParamTbl[0] = ptDevChnInfo->tVPLVICISPParam.dwTargetLuma;
			adwAEParamTbl[1] = ptDevChnInfo->tVPLVICISPParam.dwTargetOffset;
			adwAEParamTbl[2] = ptDevChnInfo->tVPLVICISPParam.dwIrisActiveTime;
			adwAEParamTbl[3] = ptDevChnInfo->tVPLVICISPParam.dwMinShutter;
			adwAEParamTbl[4] = ptDevChnInfo->tVPLVICISPParam.dwMaxShutter;
			adwAEParamTbl[5] = ptDevChnInfo->tVPLVICISPParam.dwMinGain;
			adwAEParamTbl[6] = ptDevChnInfo->tVPLVICISPParam.dwMaxGain;
/* Version 12.0.0.0 modification, 2013.04.19 */
			adwAEParamTbl[7] = ptDevChnInfo->tVPLVICISPParam.dwAEMode;
			adwAEParamTbl[8] = (DWORD)ptDevChnInfo->tVPLVICISPParam.bAELock;
			adwAEParamTbl[9] = ptDevChnInfo->tVPLVICISPParam.dwAESpeed;
			adwAEParamTbl[10] = ptDevChnInfo->tVPLVICISPParam.dwIrisStatus;
/* ======================================== */
			ae_options.adwUserData[0] = (DWORD)adwAEParamTbl;
			ae_dev->set_options(&ae_options);
		}
		ret = -1;
		break;
	default:
		return -1;
	}
	return ret;
}

/* ============================================================================================== */
/* Version 11.0.0.3 modification, 2012.11.02 */
static int VIC_AutoWhiteBalanceSetOptions(TVideoSignalOptions *awb_options, DWORD dwNum)
/* ======================================== */
{
	int ret = 0;
	TVPLVICDevChnInfo *ptDevChnInfo;
/* Version 12.0.0.0 modification, 2013.04.19 */
	DWORD adwAWBParamTbl[4];	
/* ======================================== */

	ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
	
/* Version 11.0.0.3 modification, 2012.11.02 */
	switch (awb_options->eOptionFlags)
/* ======================================== */
	{
	case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_WINDOW_PRIORITY:
/* Version 11.0.0.3 modification, 2012.11.02 */
	case VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE:
		ret = awb_dev->set_options(awb_options);
/* ======================================== */
		break;
/* Version 12.0.0.0 modification, 2013.04.19 */
	case VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE:
		if (ptDevChnInfo->tVPLVICISPParam.bAWBCtrl)
		{
			adwAWBParamTbl[0] = ptDevChnInfo->tVPLVICISPParam.bAWBLock;
			adwAWBParamTbl[1] = ptDevChnInfo->tVPLVICISPParam.dwAWBMode;
			adwAWBParamTbl[2] = ptDevChnInfo->tVPLVICISPParam.dwCustomGainR;
			adwAWBParamTbl[3] = ptDevChnInfo->tVPLVICISPParam.dwCustomGainB;

			awb_options->adwUserData[0] = (DWORD)adwAWBParamTbl;
			awb_dev->set_options(awb_options);
		}
		ret = -1;
		break;
/* ======================================== */
	default:
		return -1;
	}
	return ret;
}
/* ============================================================================================== */
static int VIC_AutoFocusSetOptions(TVideoSignalOptions af_options, DWORD dwNum)
{
	int ret = 0;
	TVPLVICDevChnInfo *ptDevChnInfo;
	DWORD dwAFWindowStartPosition, dwAFWindowSize;
	ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
	switch (af_options.eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_SET_FOCUS_SPEED:
		case VIDEO_SIGNAL_OPTION_SET_FOCUS_NOISE_THRES:
		case VIDEO_SIGNAL_OPTION_SET_ZOOM_SPEED:
		case VIDEO_SIGNAL_OPTION_SET_ZOOMTRACKING_FOCUS_EN:
		case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_TABLE:
		ret = af_dev->set_options(&af_options);
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_CALIBRATE:
		// 1. set calibrate AF window
/* Version 11.0.0.6 modification, 2013.01.30 */
		dwAFWindowStartPosition = (((readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCapH)>>16)/2-100)<<16) + ((readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCapV)>>16)/2-100); 
/* ======================================== */
		dwAFWindowSize = (300<<16) + 300;
	
		writel(dwAFWindowStartPosition, &gptDevInfo->ptMMRInfo->dwAFWindowStartPosition);
		writel(dwAFWindowSize, &gptDevInfo->ptMMRInfo->dwAFWindowSize);             

		ptDevChnInfo->dwAFState = AF_STATECTRL_ADJUST; // set to adjust state  
		ret = af_dev->set_options(&af_options);
		break;
		default:
		return -1;
	}
	return ret;
}

/* ============================================================================================== */
/* Version 11.0.0.1 modification, 2012.09.21 */
static int VIC_IrisSetOptions(TVideoSignalOptions iris_options, unsigned long dwNum)
{
	int ret = 0;
	TVPLVICDevChnInfo *ptDevChnInfo;
	ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
	
	switch (iris_options.eOptionFlags)
	{
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE:
		if (iris_motor_dev)
		{
			if (iris_motor_dev->set_options)
				iris_motor_dev->set_options(&iris_options);
		}
		ret = -1;
		break;
	case VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE:
		if (ptDevChnInfo->tVPLVICISPParam.bAECtrl)
		{
			iris_options.adwUserData[0] = ptDevChnInfo->tVPLVICISPParam.dwTargetLuma;
			if (iris_motor_dev)
			{
				if (iris_motor_dev->set_options)
					iris_motor_dev->set_options(&iris_options);
			}
		}
		ret = -1;
		break;
	default:
		return -1;
	}
	return ret;
}
/* ========================================= */

/* ============================================================================================== */
static int VIC_SensorSetOptions(TVideoSignalOptions sensor_options, unsigned long dwNum)
{
	int ret = 0;
	TVPLVICDevChnInfo *ptDevChnInfo;
	ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
	switch (sensor_options.eOptionFlags)
	{
	case VIDEO_SIGNAL_OPTION_RESET:
		sensor_dev->ioctl(&sensor_options, dwNum);
		break;
	case VIDEO_SIGNAL_OPTION_SET_FLIP:
	case VIDEO_SIGNAL_OPTION_SET_MIRROR:
	case VIDEO_SIGNAL_OPTION_SET_BRIGHTNESS:
	case VIDEO_SIGNAL_OPTION_SET_LOW_PASS_FILTER:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
	case VIDEO_SIGNAL_OPTION_SET_MONO:
	case VIDEO_SIGNAL_OPTION_SET_CONTRAST:
	case VIDEO_SIGNAL_OPTION_SET_SATURATION:
	case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
	case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER:
	case VIDEO_SIGNAL_OPTION_SET_EXPOSURE_LEVEL:
	case VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_EN:
	case VIDEO_SIGNAL_OPTION_AUTO_DETECT_STD:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN: 
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE:
	case VIDEO_SIGNAL_OPTION_SET_WDR:
	case VIDEO_SIGNAL_OPTION_SET_COLOR_CORRECTION:
	case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
/* Version 12.0.0.2 modification, 2013.08.19 */
	case VIDEO_SIGNAL_OPTION_SET_SHARPNESS:
	case VIDEO_SIGNAL_OPTION_SET_COLOR_TRANSFORM:
/* ======================================== */
/* Version 12.0.0.3 modification, 2013.12.24 */
	case VIDEO_SIGNAL_OPTION_SENSOR_DIRECT_ACCESS:
/* ======================================== */
		ret = sensor_dev->ioctl(&sensor_options, dwNum);
		break;
	case VIDEO_SIGNAL_OPTION_SET_FRAME_RATE:
		if (sensor_options.adwUserData[0] != 0)
		{
			ret = sensor_dev->ioctl(&sensor_options, dwNum);
		}
		break;
/* Version 11.0.0.6 modification, 2013.01.30 */
/* ======================================== */
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS:
		sensor_options.adwUserData[0] = (DWORD)&ptDevChnInfo->adwAEMeasureWinTbl[0];
		ret = sensor_dev->ioctl(&sensor_options, dwNum);
		break;
	default:
		return -1;
	}
	return ret;
}

/* ============================================================================================== */
static void VIC_SetOptions(TVideoSignalOptions options, TVPLVICOptions* vic_options, unsigned long dwNum)
{
	SDWORD sdwContrast, sdwSaturation;
	DWORD dwCapHeight,dwInHeight;
	DWORD dwInFmt;
	DWORD dwWBWinWidth, dwWBWinHeight;
/* Version 11.0.0.4 modification, 2012.11.22 */
	BOOL byCFAPatternVShift, byCFAPatternHShift;
/* ======================================== */
/* Version 12.0.0.0 modification, 2013.04.19 */
	TVPLVICDevChnInfo *ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];
	TVPLVICOptions tOptions;
/* ======================================== */

	vic_options->eOptionFlags = options.eOptionFlags;
	switch (options.eOptionFlags)
	{
	case VIDEO_SIGNAL_OPTION_SET_SIZE:
		/*
			adwUserData[0](15:0) == CapWidth				
			adwUserData[0](31:16) == CapHeight
			adwUserData[1] == Stride
		*/
/* Version 12.0.0.0 modification, 2013.04.19 */
		tOptions.eOptionFlags = options.eOptionFlags;
		tOptions.adwUserData[0] = (options.adwUserData[0] & 0x0000FFFF);
		tOptions.adwUserData[1] = (options.adwUserData[0] >> 16);
		tOptions.adwUserData[6] = (options.adwUserData[1] & 0x0000FFFF);
		VIC_SetSize(tOptions, dwNum);
		ptDevChnInfo->bSetSize = TRUE;
/* ======================================== */
		break;
	case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
		/*
			adwUserData[0] == AWB ON/OFF (TRUE => manual white balance, FALSE => auto white balance)
			adwUserData[1] == red gain (used by manual white balance)
			adwUserData[2] == blue gain (used by manual white balance)
		*/
		vic_options->adwUserData[0] = options.adwUserData[0] & 0x02;
		vic_options->adwUserData[1] = ((options.adwUserData[1]&0x1FFF)<<16) | (options.adwUserData[2]&0x1FFF);
		break;
	case VIDEO_SIGNAL_OPTION_SET_MONO:
		/*
			adwUserData[0] == MONO ON/OFF
			adwUserData[1] == saturation restored value (used for VIC MONO OFF) 
			adwUserData[2] == contrast value
		*/
		vic_options->eOptionFlags = VIDEO_SIGNAL_OPTION_SET_SATURATION;
		if (options.adwUserData[0] == TRUE) // MONO ON
		{
			// set saturation to zero
			sdwContrast = ((SDWORD)(options.adwUserData[2]<<24))>>24;
			sdwSaturation = 0x00;
			sdwSaturation = ((sdwContrast+128)*sdwSaturation)>>7; // function in VIC datasheet
			vic_options->adwUserData[0] = (DWORD)(sdwSaturation&0x000001FF);
		}
		else // MONO OFF
		{
			// restore old saturation value
			sdwContrast = ((SDWORD)(options.adwUserData[2]<<24))>>24;
			sdwSaturation = (SDWORD)(options.adwUserData[1]);
			sdwSaturation = (((sdwContrast+128)*sdwSaturation)>>7);
			vic_options->adwUserData[0] = (DWORD)(sdwSaturation&0x000001FF);
		}
		break;
	case VIDEO_SIGNAL_OPTION_SET_CONTRAST:
		/*
			adwUserData[0] == contrast value(for sensor setup)
			adwUserData[1] == saturation value(for VIC setup)					
		*/
		sdwContrast = ((SDWORD)(options.adwUserData[0]<<24))>>24;
		sdwSaturation = (SDWORD)(options.adwUserData[1]);
		sdwSaturation = ((sdwContrast+128)*sdwSaturation)>>7;
		vic_options->adwUserData[0] =(DWORD)((sdwContrast&0xFF) | ((sdwSaturation&0x01FF)<<16));
		break;
	case VIDEO_SIGNAL_OPTION_SET_SATURATION:
		/*
			adwUserData[0] == saturation value(for sensor setup)
			adwUserData[1] == saturation value after ACS (for VIC setup)
			adwUserData[2] == contrast value
		*/
		sdwContrast = ((SDWORD)(options.adwUserData[2]<<24))>>24;
		sdwSaturation = (SDWORD)(options.adwUserData[1]);
		sdwSaturation = ((sdwContrast+128)*sdwSaturation)>>7;					
		vic_options->adwUserData[0] = (DWORD)(sdwSaturation&0x000001FF);
		break;
	case VIDEO_SIGNAL_OPTION_SET_FLIP:
	case VIDEO_SIGNAL_OPTION_SET_MIRROR:
	case VIDEO_SIGNAL_OPTION_SET_BRIGHTNESS:
	case VIDEO_SIGNAL_OPTION_SET_LOW_PASS_FILTER:
	case VIDEO_SIGNAL_OPTION_SET_COLOR_CORRECTION:
	case VIDEO_SIGNAL_OPTION_SET_CAPTURE_AREA:
	case VIDEO_SIGNAL_OPTION_SET_START_PIXEL:
	case VIDEO_SIGNAL_OPTION_SET_FRAME_RATE:
	case VIDEO_SIGNAL_OPTION_SET_HALF_SIZED_OUTPUT:
	case VIDEO_SIGNAL_OPTION_SET_TONE_MAPPING:
	case VIDEO_SIGNAL_OPTION_SET_CONTRAST_ENHANCEMENT:
	case VIDEO_SIGNAL_OPTION_SET_WDR:		
	case VIDEO_SIGNAL_OPTION_SET_FOCUS_POSITION:
	case VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_WINDOW:
	case VIDEO_SIGNAL_OPTION_SET_ZOOM_POSITION:
/* Version 12.0.0.2 modification, 2013.08.19 */
	case VIDEO_SIGNAL_OPTION_SET_COLOR_TRANSFORM:
/* ======================================== */
/* Version 11.0.0.6 modification, 2013.01.30 */
/* ======================================== */
/* Version 11.0.0.4 modification, 2012.11.22 */
/* ======================================== */
		vic_options->adwUserData[0] = options.adwUserData[0];
		vic_options->adwUserData[1] = options.adwUserData[1];		
		vic_options->adwUserData[2] = options.adwUserData[2];		
		break;   
/* Version 11.0.0.4 modification, 2012.11.22 */
	case VIDEO_SIGNAL_OPTION_SET_BLACK_CLAMP:
		byCFAPatternVShift = ((readl(&gptDevInfo->ptMMRInfo->dwCMYGCtrl0)>>2)&0x01);
		byCFAPatternHShift = (readl(&gptDevInfo->ptMMRInfo->dwCMYGCtrl0)&0x01);
		if ((!byCFAPatternVShift) && (!byCFAPatternHShift))
		{
			vic_options->adwUserData[0] = (((options.adwUserData[0]&0xFF)<<24) | (((options.adwUserData[0]>>8)&0xFF)<<16) | 
										(((options.adwUserData[0]>>24)&0xFF)<<8) |((options.adwUserData[0]>>16)&0xFF));
		} 
		else if ((byCFAPatternVShift)&&(!byCFAPatternHShift))
		{
			vic_options->adwUserData[0] = ((((options.adwUserData[0]>>24)&0xFF)<<24) | (((options.adwUserData[0]>>16)&0xFF)<<16) | 
										((options.adwUserData[0]&0xFF)<<8) |((options.adwUserData[0]>>8)&0xFF));
		}
		else if ((!byCFAPatternVShift)&&(byCFAPatternHShift))
		{
			vic_options->adwUserData[0] = ((((options.adwUserData[0]>>8)&0xFF)<<24) | ((options.adwUserData[0]&0xFF)<<16) | 
										(((options.adwUserData[0]>>16)&0xFF)<<8) |((options.adwUserData[0]>>24)&0xFF));
		}
		else
		{
			vic_options->adwUserData[0] = ((((options.adwUserData[0]>>16)&0xFF)<<24) | (((options.adwUserData[0]>>24)&0xFF)<<16) | 
										(((options.adwUserData[0]>>8)&0xFF)<<8) |(options.adwUserData[0]&0xFF));
		}		
		break;   
/* ======================================== */
	case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS:
                dwWBWinWidth = (gptDevInfo->atDevChnInfo[dwNum].adwAEMeasureWinTbl[1]) & 0x00000FFF;
                dwWBWinHeight = (gptDevInfo->atDevChnInfo[dwNum].adwAEMeasureWinTbl[1]>>16) & 0x00000FFF;
                dwWBWinWidth = (dwWBWinWidth/32) * 32;
                dwWBWinHeight = (dwWBWinHeight/32) * 32;
                gptDevInfo->atDevChnInfo[dwNum].adwAEMeasureWinTbl[1] = (dwWBWinHeight<<16) | dwWBWinWidth;
		break;
	case VIDEO_SIGNAL_OPTION_SET_ANTI_ALIASING:
		copy_from_user(&(gptDevInfo->atDevChnInfo[dwNum].dwCFAAntiAliasingTbl[0]), (DWORD *)(options.adwUserData[0]), (10*sizeof(DWORD)));
		break;
	case VIDEO_SIGNAL_OPTION_SET_IMPULSE_NOISE_REMOVAL:
		vic_options->adwUserData[0] = options.adwUserData[0] & 0x01;
		copy_from_user(&(gptDevInfo->atDevChnInfo[dwNum].dwDeImpulseTbl[0]), (DWORD *)(options.adwUserData[1]), (12*sizeof(DWORD)));
		break;
	case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if (sensor_dev->get_remaining_line_num) {
			dwCapHeight = (readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCapV)&0xFFFF0000)>>16;
			dwCapHeight = dwCapHeight + sensor_dev->get_remaining_line_num() - video_sensor_remainingline;

			dwInHeight = (readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwInSize)&0xFFFF0000)>>16;
			dwInHeight = dwInHeight + sensor_dev->get_remaining_line_num() - video_sensor_remainingline;

			//setup VIC input size
			writel((readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwInSize)&0x0000FFFF)|(dwInHeight<<16), &gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwInSize);

			// setup VIC capture size			
			writel((readl(&gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCapV)&0x0000FFFF)|(dwCapHeight<<16), &gptDevInfo->ptMMRInfo->atChnInfo[dwNum].dwCapV);

			// wait for update MMR	
			writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]) | 0x200, &gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum]);
			while ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[dwNum])&0x200) != 0); 
			dwInFmt = (readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0x1c)>>2;	
			if ((dwInFmt==1) || (dwInFmt==3) || (dwInFmt==4)) {  // (IN_FORMAT=="16-bits RAW") || (IN_FORMAT=="16-bits with SYNC") || (IN_FORMAT=="Bayer")
				writel(readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[1]) | 0x200, &gptDevInfo->ptMMRInfo->adwChanCtrl[1]);	// update chn#1 MMRs
				while ((readl(&gptDevInfo->ptMMRInfo->adwChanCtrl[1])&0x200) != 0); 
			}
			video_sensor_remainingline = sensor_dev->get_remaining_line_num();

		}
		break;
	default:
		break;
	}
}

/* ============================================================================================== */
static int Ioctl(struct inode *pinode, struct file *pfile, unsigned int dwCmd, unsigned long dwArg)
{
    int scError, scRet;
    DWORD dwNum;
    TVPLVICInitOptions tInitOptions;
    TVPLVICState tState;
    TVPLVICDevChnInfo *ptDevChnInfo;
    DWORD dwVersionNum;
    DWORD dwSize;

    DWORD *pdwSYSCVirtAddr;
	/* for "VIC_SensorSetOptions()", copied from user space */
	TVideoSignalOptions tOptions;
	int ret;

    TVPLVICObjInfo *ptObjInfo = (TVPLVICObjInfo *)pfile->private_data;

    scRet = 0;
    dwNum = MINOR(pinode->i_rdev);

    PDEBUG("Enter device %lu Ioctl function...\n", dwNum);

    if (pfile->private_data == NULL)
    {
        PDEBUG("Device error !!\n");
        return -ENODEV;
    }

    if ((_IOC_TYPE(dwCmd)!=VPL_VIC_IOC_MAGIC) || (_IOC_NR(dwCmd)>VPL_VIC_IOC_MAX_NUMBER))
    {
        PDEBUG("Incorrect ioctl command !!\n");

        return -ENOTTY;
    }

    if (_IOC_DIR(dwCmd) & _IOC_READ)
    {
        // access_ok() is kernel-oriented.
        scError = !access_ok(VERIFY_WRITE, (void *)dwArg, _IOC_SIZE(dwCmd));
    }
    else if (_IOC_DIR(dwCmd) & _IOC_WRITE)
    {
        scError = !access_ok(VERIFY_READ, (void *)dwArg, _IOC_SIZE(dwCmd));
    }
    else
    {
        scError = 0;
    }

    if (scError != 0)
    {
        PDEBUG("Unsupport ioctl command %d !!\n", dwCmd);

        return -EFAULT;
    }

    ptDevChnInfo = &gptDevInfo->atDevChnInfo[dwNum];

    switch (dwCmd)
    {
        case VPL_VIC_IOC_START_VIC:
            VIC_Start();
            break;
        case VPL_VIC_IOC_WAIT_COMPLETE:
            if (WaitComplete(dwNum, ptObjInfo->dwProcIndex) != S_OK)
            {
                return -ERESTARTSYS;
            }
            break;
        case VPL_VIC_IOC_CHECK_INIT:
            scRet = ptDevChnInfo->bInitialized;
            break;
        case VPL_VIC_IOC_INITIAL:
            PDEBUG("VIC initial !!\n");
            if (ptDevChnInfo->bInitialized == FALSE)
            {
                copy_from_user(&tInitOptions, (TVPLVICInitOptions *)dwArg, sizeof(TVPLVICInitOptions));
                VIC_Initial(&tInitOptions, dwNum, ptObjInfo);
                ptDevChnInfo->bInitialized = TRUE;
            }
            break;
        case VPL_VIC_IOC_GET_BUF:
            scRet = VIC_Get_Buf(&tState, dwNum, ptObjInfo);
            copy_to_user((TVPLVICState *)dwArg, &tState, sizeof(TVPLVICState));
            break;
        case VPL_VIC_IOC_RELEASE_BUF:
            PDEBUG("Release buffer !!\n");
            VIC_Release_Buf(dwArg, dwNum, ptObjInfo);
            PDEBUG("<Ch %lu> release semaphore\n", dwNum);
            break;
        case VPL_VIC_IOC_SETOPTIONS:	
            PDEBUG("VIC SetOption !!\n");
            copy_from_user(&tOptions, (TVideoSignalOptions *)dwArg, sizeof(TVideoSignalOptions));

            if (tOptions.eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE)
            {
                copy_from_user(&(ptDevChnInfo->tVPLVICISPParam), (TVPLVICISPParam *)(tOptions.adwUserData[0]), (sizeof(TVPLVICISPParam)));
            }

/* Version 11.0.0.1 modification, 2012.09.21 */
            if (bIrisEn) {
                ret = VIC_IrisSetOptions(tOptions, dwNum);
                if (ret == 0) { // iris module completed this setoption
                    return S_OK;
                }
            }
/* ========================================= */

            if (bAEEn) {
                ret = VIC_AutoExposureSetOptions(tOptions, dwNum);
                if (ret == 0) { // auto-exposure module completed this setoption
                    return S_OK;
                }
            }
   
            if (bAWBEn) {
/* Version 11.0.0.3 modification, 2012.11.02 */
                ret = VIC_AutoWhiteBalanceSetOptions(&tOptions, dwNum);
                if (tOptions.eOptionFlags == VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE)
                {
                    copy_to_user((DWORD *)dwArg, &tOptions, sizeof(TVideoSignalOptions));
                }
/* ======================================== */
                if (ret == 0) { // auto-white balance module completed this setoption
                    return S_OK;
                }
            }

            if (bAFEn) {
                ret = VIC_AutoFocusSetOptions(tOptions, dwNum);
                if (ret == 0) { // auto-focus module completed this setoption
                    return S_OK;
                }
            }

            if (tOptions.eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS)
            {
                copy_from_user(&(ptDevChnInfo->adwAEMeasureWinTbl[0]), (DWORD *)(tOptions.adwUserData[0]), (2*sizeof(DWORD)));
            }

            ret = VIC_SensorSetOptions(tOptions, dwNum);
            if (ret == 0) { // sensor module completed this setoption
                return S_OK;
            }

            if (tOptions.eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_WINDOW)
            {
                copy_from_user(&(ptDevChnInfo->adwAFMeasureWinTbl[0]), (DWORD *)(tOptions.adwUserData[0]), (2*sizeof(DWORD)));
            }

		//wait for ISR
            if (gptDevInfo->atDevChnInfo[dwNum].bSetOption == TRUE)
            {
                if (wait_event_interruptible(ptDevChnInfo->wq, (gptDevInfo->atDevChnInfo[dwNum].bSetOption==FALSE)) != 0)
                {
                    PDEBUG("Exit VPL_VIC_IOC_SETOPTIONS by signal !!\n");
                    return -ERESTARTSYS;
                }
            }
            VIC_SetOptions(tOptions, &(ptDevChnInfo->tOptions), dwNum);
            gptDevInfo->atDevChnInfo[dwNum].bSetOption = TRUE;
            return S_OK;
            break;
        case VPL_VIC_IOC_GET_BANDWIDTH:
            copy_to_user((DWORD *)dwArg, gptDevInfo->pdwBandwidth, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_RG_INTERVAL:
            copy_to_user((DWORD *)dwArg, gptDevInfo->pdwRGInterval, sizeof(DWORD));
        	break;
        case VPL_VIC_IOC_GET_REQ_TIMES:
            copy_to_user((DWORD *)dwArg, gptDevInfo->pdwReqTimes, sizeof(DWORD));
        	break;
        case VPL_VIC_IOC_CLEAR_PROFILE:
            writel((0x1<<VPL_VIC_MASTER_NUM), gptDevInfo->pdwProfileClr);
            break;
        case VPL_VIC_IOC_STOP_CAPTURE:
            PDEBUG("Stop VIC !!\n");
            gptDevInfo->atDevChnInfo[dwNum].bStopCapture = TRUE;
            if (wait_event_interruptible(ptDevChnInfo->wq, (gptDevInfo->atDevChnInfo[dwNum].bStopCapture==FALSE)) != 0)
            {
                PDEBUG("Exit VPL_VIC_IOC_STOP_CAPTURE by signal !!\n");
                return -ERESTARTSYS;
            }
            return S_OK;
            break;
        case VPL_VIC_IOC_GET_VERSION_NUMBER:
            dwVersionNum = VPL_VIC_VERSION;
            copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_FB_BASEADDR:
            copy_to_user((DWORD *)dwArg, &ptDevChnInfo->dwPhyBaseAddr, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_FB_SIZE:
            dwSize = ptDevChnInfo->dwMaxFrameWidth * ptDevChnInfo->dwMaxFrameHeight;
            dwSize = dwSize + 2*(dwSize>>(1+ptDevChnInfo->bOutFormat));
            dwSize = dwSize * ptDevChnInfo->dwBufNum;
            copy_to_user((DWORD *)dwArg, &dwSize, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_WAIT_TBL_COMPLETE:
            WaitTblComplete(dwNum);
            break;
        case VPL_VIC_IOC_WAIT_CE_TBL_COMPLETE:
            WaitCeTblComplete(dwNum);
            break;
        case VPL_VIC_IOC_WAIT_PHOTO_LDC_TBL_COMPLETE:
              WaitPhotoLdcTblComplete(dwNum);
            break;
        case VPL_VIC_IOC_IIC_EN:
            pdwSYSCVirtAddr = (DWORD *)IO_ADDRESS(VPL_SYSC_MMR_BASE);
            writel(0x00000013, (pdwSYSCVirtAddr+21));
            break;
        case VPL_VIC_IOC_IIC_DE:
            pdwSYSCVirtAddr = (DWORD *)IO_ADDRESS(VPL_SYSC_MMR_BASE);
            writel(0x00000011, (pdwSYSCVirtAddr+21));
            break;
        case VPL_VIC_IOC_GET_AF_TBL_SIZE:
            copy_to_user((DWORD *)dwArg, &dwZtTbSize, sizeof(DWORD));		
            break;
        case VPL_VIC_IOC_SET_COLOR_TEMPERATURE:
            VIC_SetAWBColorTemperature(dwArg);
            break;
/* Version 11.0.0.3 modification, 2012.11.02 */
        case VPL_VIC_IOC_GET_STAT_GRID_BUFFER_BASEADDR:
            copy_to_user((DWORD *)dwArg, &gptDevInfo->dwStatAEWBPhyBaseAddr, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_STAT_GRID_BUFFER_SIZE:
            dwSize = sizeof(DWORD) * 720;
            copy_to_user((DWORD *)dwArg, &dwSize, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_BASEADDR:
            copy_to_user((DWORD *)dwArg, &gptDevInfo->dwStatHistoPhyBaseAddr, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_SIZE:
            dwSize = sizeof(DWORD) * 192;
            copy_to_user((DWORD *)dwArg, &dwSize, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_BASEADDR:
            copy_to_user((DWORD *)dwArg, &gptDevInfo->dwStatFocusPhyBaseAddr, sizeof(DWORD));
            break;
        case VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_SIZE:
            dwSize = sizeof(DWORD) * 98;
            copy_to_user((DWORD *)dwArg, &dwSize, sizeof(DWORD));
            break;
/* ======================================== */
        default:
            return -ENOTTY;
    }

    PDEBUG("Exit device %lu Ioctl function !!\n", dwNum);

    return scRet;
}

/* ============================================================================================== */
static int MMap(struct file *file, struct vm_area_struct *vma)
{
    DWORD dwSize;

    dwSize = vma->vm_end - vma->vm_start;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_flags |= VM_RESERVED;

    if (remap_pfn_range(vma, vma->vm_start, (VPL_VIC_MMR_BASE>>PAGE_SHIFT), dwSize, vma->vm_page_prot))
    {
        return -EAGAIN;
    }

    PDEBUG("Start address = 0x%08lX, end address = 0x%08lX\n", vma->vm_start, vma->vm_end);

    return 0;
}

/* ============================================================================================== */
struct file_operations vpl_vic_fops =
{
    ioctl:      Ioctl,
    mmap:       MMap,
    open:       Open,
    release:    Close,
};

/* ============================================================================================== */
static void Cleanup_Module(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
    int scResult;
#endif

    PDEBUG("Enter Cleanup_Module function...\n");

    if (gptDevInfo != NULL)
    {
        if (gsdwMajor != 0)
        {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
            scResult = unregister_chrdev(gsdwMajor, "vpl_vic");

            if (scResult < 0)
            {
                PDEBUG("Cannot release major number %d !!\n", (int)gsdwMajor);
                return;
            }
#else
			unregister_chrdev(gsdwMajor, "vpl_vic");
#endif
        }

        if (gptDevInfo->pdwAHBCCtrl != NULL)
        {
            iounmap(gptDevInfo->pdwAHBCCtrl);
            release_mem_region(VPL_AHBC_1_MMR_BASE+0x04, sizeof(DWORD));
        }

        if (gptDevInfo->pdwProfileClr != NULL)
        {
            iounmap(gptDevInfo->pdwProfileClr);
            release_mem_region(VPL_AHBC_1_MMR_BASE+0x108, sizeof(DWORD));
        }

        if (gptDevInfo->pdwBandwidth != NULL)
        {
            iounmap(gptDevInfo->pdwBandwidth);
            release_mem_region(VPL_AHBC_1_MMR_BASE+0x10C+((VPL_VIC_MASTER_NUM-1)<<2), sizeof(DWORD));
        }

        if (gptDevInfo->pdwRGInterval != NULL)
        {
            iounmap(gptDevInfo->pdwRGInterval);
            release_mem_region(VPL_AHBC_1_MMR_BASE+0x148+((VPL_VIC_MASTER_NUM-1)<<2), sizeof(DWORD));
        }
        if (gptDevInfo->pdwReqTimes != NULL)
        {
            iounmap(gptDevInfo->pdwReqTimes);
            release_mem_region(VPL_AHBC_1_MMR_BASE+0x184+((VPL_VIC_MASTER_NUM-1)<<2), sizeof(DWORD));
        }

        if (gptDevInfo->ptMMRInfo != NULL)
        {
            iounmap(gptDevInfo->ptMMRInfo);
            release_mem_region(VPL_VIC_MMR_BASE, sizeof(TVPLVICInfo));
        }

        kfree(gptDevInfo);
    }

    PDEBUG("Exit Cleanup_Module function !!\n");

    return;
}

/* ============================================================================================== */
static int Initial_Module(void)
{
    int scResult;
    DWORD i;
    DWORD dwVICMmrCtrl;

    PDEBUG("Enter Initial_Module function...\n");

    if ((gptDevInfo=kmalloc(sizeof(TVPLVICDevInfo), GFP_KERNEL)) == NULL)
    {
        PDEBUG("Allocate device info buffer fail !!\n");
        scResult = -ENOMEM;
        goto FAIL;
    }
    memset(gptDevInfo, 0, sizeof(TVPLVICDevInfo));

    writel(readl(SYSC_CLK_EN_MMR)|(0x3<<VPL_VIC_CLK_EN_NUM), SYSC_CLK_EN_MMR);

    if (request_mem_region(VPL_VIC_MMR_BASE, sizeof(TVPLVICInfo), "VPL_VIC") == NULL)
    {
        PDEBUG("Request MMR memory region fail !!\n");
        scResult = -EBUSY;
        goto FAIL;
    }
    gptDevInfo->ptMMRInfo = (TVPLVICInfo *)ioremap((int)VPL_VIC_MMR_BASE, sizeof(TVPLVICInfo));
    PDEBUG("Remapped base address = 0x%08X\n", (int)gptDevInfo->ptMMRInfo);

    if ((readl(&gptDevInfo->ptMMRInfo->dwVersion)&0xFF000000) == 0)
    {
        printk("VPL_VIC: Device does not exist !!\n");
        scResult = -ENODEV;
        goto FAIL;
    }

    request_mem_region(VPL_AHBC_1_MMR_BASE+0x108, sizeof(DWORD), "VPL_VIC Profile Clear");
    gptDevInfo->pdwProfileClr = (DWORD *)ioremap((int)(VPL_AHBC_1_MMR_BASE+0x108), sizeof(DWORD));

    request_mem_region(VPL_AHBC_1_MMR_BASE+0x10C+((VPL_VIC_MASTER_NUM-1)<<2), sizeof(DWORD), "VPL_VIC Bandwidth");
    gptDevInfo->pdwBandwidth = (DWORD *)ioremap((int)(VPL_AHBC_1_MMR_BASE+0x10C+((VPL_VIC_MASTER_NUM-1)<<2)), sizeof(DWORD));

    request_mem_region(VPL_AHBC_1_MMR_BASE+0x148+((VPL_VIC_MASTER_NUM-1)<<2), sizeof(DWORD), "VPL_VIC Request Grant Interval");
    gptDevInfo->pdwRGInterval = (DWORD *)ioremap((int)(VPL_AHBC_1_MMR_BASE+0x148+((VPL_VIC_MASTER_NUM-1)<<2)), sizeof(DWORD));

    request_mem_region(VPL_AHBC_1_MMR_BASE+0x184+((VPL_VIC_MASTER_NUM-1)<<2), sizeof(DWORD), "VPL_VIC Request Times");
    gptDevInfo->pdwReqTimes = (DWORD *)ioremap((int)(VPL_AHBC_1_MMR_BASE+0x184+((VPL_VIC_MASTER_NUM-1)<<2)), sizeof(DWORD));

    request_mem_region(VPL_AHBC_1_MMR_BASE+0x04, sizeof(DWORD), "VPL AHBC 1 CTRL");
    gptDevInfo->pdwAHBCCtrl = (DWORD *)ioremap((int)(VPL_AHBC_1_MMR_BASE+0x04), sizeof(DWORD));
    if (((readl(gptDevInfo->pdwAHBCCtrl)>>(16+VPL_VIC_MASTER_NUM))&0x01) != 0x01)
    {
        writel(readl(gptDevInfo->pdwAHBCCtrl)|(0x1<<(16+VPL_VIC_MASTER_NUM)), gptDevInfo->pdwAHBCCtrl);
    }
    PDEBUG("Priority: 0x%08x !!\n", readl(gptDevInfo->pdwAHBCCtrl));

    scResult = register_chrdev(gsdwMajor, "vpl_vic", &vpl_vic_fops);

    if (scResult < 0)
    {
        PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
        goto FAIL;
    }

    if (gsdwMajor == 0)
    {
        gsdwMajor = scResult;
    }

    dwVICMmrCtrl = readl(&gptDevInfo->ptMMRInfo->dwCtrl);
    dwVICMmrCtrl = (dwVICMmrCtrl& 0x8000001F) | ((((gsdwBusFreq/1000000)-1)&0x007FFFFF) <<8);
    writel(dwVICMmrCtrl, &gptDevInfo->ptMMRInfo->dwCtrl);
    writel(((gsdwBusFreq/1000)-1)*gsdwSignalWaitTime, &gptDevInfo->ptMMRInfo->dwNoSig);

    for (i=0; i<VPL_VIC_CHN_NUM; i++)
    {
        init_waitqueue_head(&gptDevInfo->atDevChnInfo[i].wq);
    }

    // initialize work queue	
        INIT_WORK(&gptDevInfo->aewb_oneframe_work, (work_func_t)VIC_AEWBOneframeWQ);
        INIT_WORK(&gptDevInfo->af_oneframe_work, (work_func_t)VIC_AutoFocusOneframeWQ);

    if (sensor_dev->detect_standard) {
        INIT_WORK(&gptDevInfo->auto_detect_work, (work_func_t)VIC_AutoDetectStdTasklet);
    }
    printk("Install VPL_VIC device driver version %d.%d.%d.%d on VPL_VIC hardware version %d.%d.%d.%d complete !!\n",
           (int)(VPL_VIC_VERSION&0xFF),
           (int)((VPL_VIC_VERSION>>8)&0xFF),
           (int)((VPL_VIC_VERSION>>16)&0xFF),
           (int)((VPL_VIC_VERSION>>24)&0xFF),
           (readl(&gptDevInfo->ptMMRInfo->dwVersion)>>24)&0xFF,
           (readl(&gptDevInfo->ptMMRInfo->dwVersion)>>16)&0xFF,
           (readl(&gptDevInfo->ptMMRInfo->dwVersion)>>8)&0xFF,
           readl(&gptDevInfo->ptMMRInfo->dwVersion)&0xFF);

    PDEBUG("Exit Initial_Module function !!\n");

    return 0;

FAIL:
    writel(readl(SYSC_CLK_EN_MMR)&(~(0x3<<VPL_VIC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
    Cleanup_Module();

    return scResult;
}

/* ============================================================================================== */
/* Version 11.0.0.1 modification, 2012.09.21 */
EXPORT_SYMBOL(ae_dev);
EXPORT_SYMBOL(iris_motor_dev);
EXPORT_SYMBOL(af_dev);
EXPORT_SYMBOL(af_motor_dev);
EXPORT_SYMBOL(awb_dev);
/* ========================================= */

module_init(Initial_Module);
module_exit(Cleanup_Module);

/* ============================================================================================== */
