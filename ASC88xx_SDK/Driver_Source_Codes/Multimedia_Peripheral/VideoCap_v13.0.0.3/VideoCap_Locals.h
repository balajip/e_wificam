/*

/* ============================================================================================== */
#ifndef __VIDEOCAP_LOCALS_H__
#define __VIDEOCAP_LOCALS_H__

/* ============================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <linux/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "typedef.h"
#include "global_codec.h"
#include "dbgdefs.h"
#include "video_error.h"

/* ============================================================================================== */
#include "VideoCap.h"
/* Version 12.0.0.3 modification, 2013.01.16 */
#include "VideoCap_Error.h"
/* ======================================== */
/* Version 3.0.0.0 modification, 2007.09.03 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
#include "VideoCap_Tables.h"
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */
#include "vpl_vic.h"
/* Version 8.0.0.2 modification, 2010.05.17 */
#include "vpl_edmc.h"
/* ======================================== */

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
/* ======================================== */

/* ============================================================================================== */
#define ALIGN_VALUE 2
#define ALIGN_PADDING 3
#define ALIGN_MASK 0xFFFFFFFC

/* Version 12.0.0.3 modification, 2013.01.16 */
#define ERRPRINT(fmt, args...) printf("[VIDEOCAP][ERROR]: " fmt, ## args);
/* ======================================== */

/* Version 6.0.0.1 modification, 2009.06.19 */
#define INTERVEL 1000 // in millisecond
#define PRECISION 1024
/* ======================================== */

/* Version 7.0.0.0 modification, 2009.10.08 */
/* Version 7.2.0.3 modification, 2009.11.10 */
#define ACS_STEP 8
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.0 modification, 2008.10.14 */
/* ============================================================================================== */
typedef enum video_cap_cfa_flags
{
	CFA_BasicBayer = 0,
	CFA_EnhancedBayer = 1,
	CFA_CMYG = 2
} EVideoCapCFAFlags;
/* ======================================== */

/* ============================================================================================== */
typedef struct video_cap_info
{
	volatile TVPLVICInfo *ptVICInfo;

	HANDLE hVICDev;
	HANDLE hVideoSignalObject;
/* Version 2.0.0.0 modification, 2006.06.21 */
	HANDLE hEDMCDev;
/* ======================================== */
/* Version 8.0.0.0 modification, 2010.04.28 */
/* ======================================== */
	DWORD dwMaxFrameWidth;
	DWORD dwMaxFrameHeight;
/* Version 7.0.0.0 modification, 2009.10.11 */
	DWORD dwStartPixel;
	DWORD dwStartLine;
/* ======================================== */
	DWORD dwCapWidth;
	DWORD dwCapHeight;
	DWORD dwVideoWidth;
	DWORD dwVideoHeight;
	DWORD dwVertSS;
	DWORD dwHorzSS;
/* Version 8.0.0.0 modification, 2010.04.28 */
/* ======================================== */
/* Version 5.2.0.4 modification, 2008.11.27 */
/* Version 5.4.0.0 modification, 2008.12.19 */
/* ======================================== */
/* ======================================== */
	DWORD dwBufNum;

/* Frame rate control members */
/* Version 6.0.0.1 modification, 2009.06.19 */
	DWORD dwFrameRate;
/* Version 3.1.0.0 modification, 2008.02.02 */
/* ======================================== */
	DWORD dwPrevTicks;
/* Version 5.2.0.5 modification, 2008.11.27 */
	BOOL bInitPrevTicks;
/* ======================================== */
	DWORD dwInterval;
	DWORD dwIntervalResidue;
/* Version 6.0.0.1 modification, 2009.06.19 */
	DWORD dwIntvResAccu;
/* ======================================== */
	DWORD dwDiffResidue;
	DWORD dwFrameCount;
/* ======================================== */
/* Version 3.1.0.0 modification, 2008.02.02 */
	BOOL bHalfSizedOutput;
	BOOL bIsMaster;
/* ======================================== */

	BOOL bMono;
	DWORD dwSaturation;
/* Version 7.3.0.4 modification, 2010.03.05 */
	SDWORD sdwBrightness;
	SDWORD sdwContrast;
/* ======================================== */

	DWORD dwDeviceNum;

    EVideoSignalFormat eFormat;

/* Version 7.0.0.0 modification, 2009.10.08 */
    /* Automatic color suppression */
    BOOL bACSEn;
    DWORD dwACSLumaMax;
    DWORD dwACSLumaMin;
    DWORD dwACSLumaRange;
    DWORD dwACSFactor;
/* ======================================== */
    BOOL bAllocateExternal;
/* Version 3.0.0.0 modification, 2007.09.03 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
	BYTE *pbyGammaTbl;

	BYTE *pbyStatBufUsrBaseAddr;

/* Version 5.0.0.0 modification, 2008.10.09 */
	BOOL bGammaEn;

	/* ! Bayer Bitwdith */
	BYTE byBayerBitwidth;

	/*! Load gamma table enable
	* - 0 => Normal mode
	* - 1 => Load gamma table mode
	*/
	BOOL bLoadTbl_en;

	/* ! CFA method */
	EVideoCapCFAFlags eCFAMode;

	/*! Padding enable
	* - 0 => Disable padding
	* - 1 => Enable padding
	*/
	BOOL bAdditionalPad_en;

	/*! De-noise enable
	* - 0 => Disable impulse removal
	* - 1 => Enable impulse removal
	*/
	BOOL bDenoise_en;

	/* ! To select coefficients for RGB to YCbCr */
	EVideoCapRGB2YCbCrFlags eRGB2YCbCrFlags;

	/*! De-noise threshold */
	DWORD dwThr1;
	DWORD dwThr0;
	DWORD dwDeNoiseThrMax1;
	DWORD dwDeNoiseThrMax2;
	DWORD dwDeNoiseThrMin1;
	DWORD dwDeNoiseThrMin2;

	/*! Manual WB enable
	* - 0 => Enable auto white balance
	* - 1 => Enable manual white balance
	*/
	BOOL bManualWBEn;
/* Version 7.3.0.0 modification, 2009.11.24 */
	BOOL bAWBAfterCC;
/* ======================================== */

	/*! AWB adjustment step */
	BYTE byAWBMaxStep;
	BYTE byAWBMinStep;

	/*! AWB mode enable */
//	BOOL bRESERVED0En;
//	BOOL bRESERVED1En;
	BOOL bAWBEn;

	/*! AWB Max/Min threshold for GW */
	BYTE byAWBMax;
	BYTE byAWBMin;
	BYTE byAWBCbUpperBond;
	BYTE byAWBCbLowerBond;
	BYTE byAWBCrUpperBond;
	BYTE byAWBCrLowerBond;

	/*! AWB threshold for Gamut */
	DWORD dwRESERVED0;
	DWORD dwRESERVED1;
	DWORD dwRESERVED2;
	DWORD dwRESERVED3;

	/*! AWB manual gain */
/* Version 12.0.0.1 modification, 2012.10.15 */
	DWORD dwAWBGainUpperBond;
	DWORD dwAWBGainLowerBond;
	DWORD dwAWBRGain;
	DWORD dwAWBBGain;
/* ======================================== */

	/*! Color correction coefficients */
	DWORD dwCoeffRR;
	DWORD dwCoeffGR;
	DWORD dwCoeffBR;
	DWORD dwCoeffRG;
	DWORD dwCoeffGG;
	DWORD dwCoeffBG;
	DWORD dwCoeffRB;
	DWORD dwCoeffGB;
	DWORD dwCoeffBB;
/* Version 8.1.0.0 modification, 2011.06.21 */
	DWORD dwCoeffROffset;
	DWORD dwCoeffGOffset;
	DWORD dwCoeffBOffset;
	
	DWORD dwGammaOverflowSlope;

	DWORD dwCFAHVAvgThrd;
	DWORD dwCFAHVBlendMaxThrd;
	DWORD dwCFAHVBlendMinThrd;
	DWORD dwCFAFCSEdgeThrd;
	DWORD dwCFAFCSMaxThrd;
	DWORD dwCFAFCSMinThrd;
	DWORD dwCFAFCSCtrl;
/* ======================================== */

	DWORD dwAEWBWindowLocationX;
	DWORD dwAEWBWindowLocationY;
	DWORD dwAEWBWindowWidth;
	DWORD dwAEWBWindowHeight;

	DWORD dwBlackClamp;
	DWORD dwHorBlanking;

	/*! AF MMRs */
	BOOL bAFRefFrameSel;
	BOOL bAFEn;
	BOOL bAFRestart;
	BOOL bAFZoomTrackAFStart;
	BOOL bAFCtrlValid;
	DWORD dwAFNoiseThresh;
	DWORD dwAFRestartUpperDiff;
	DWORD dwAFRestartLowerDiff;
	DWORD dwAFInitStepSize;
	DWORD dwAFTotalPlaneNum;
	DWORD dwAFInitPlaneNum;
	DWORD dwAFWindowStartPosX;
	DWORD dwAFWindowStartPosY;
	DWORD dwAFWindowWidth;
	DWORD dwAFWindowHeight;

/* Version 5.8.0.0 modification, 2009.04.08 */
	BYTE byClipYUpper;
	BYTE byClipYLower;
	BYTE byClipCbCrUpper;
	BYTE byClipCbCrLower;

	BYTE byYKiller;

	DWORD dwCMYGEdgeThr;
	BYTE byCFAPatternVShift;
	BYTE byCFAPatternHShift;
	BYTE byCMYGAttenuateMethod;
	DWORD dwCMYGEdgeDiffThr;
	DWORD dwCMYGChromaDiffThr;
/* Version 5.8.0.0 modification, 2009.04.08 */
	DWORD dwCFAEnhancedBayerHorPenThreshold;
	DWORD dwCFAEnhancedBayerVerPenThreshold;
/* ======================================== */
/* ======================================== */

/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.6 modification, 2008.03.19 */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* Version 5.2.0.0 modification, 2008.10.29 */
	DWORD dwAFFrameIntv;
/* ======================================== */
/* Version 5.6.0.0 modification, 2009.01.16 */
	DWORD dwAFMaxReturnNum;
	DWORD dwAFFineTuneFinishThrsh;
/* ======================================== */

/* Version 5.0.0.0 modification, 2008.10.09 */
	BOOL bTMEn;
	BOOL bTMKeyTuningEn;
	BYTE byTMKeyValue;
	BYTE byTMExpValue;
	DWORD dwTMLSBMask;
	DWORD dwTMAvg;

	BOOL bCEEn;
	BOOL bCEAutoEn;
	BYTE byCEDLMethod;
	BYTE byCEMethod;
	BYTE byCEBrightness;
/* Version 6.2.0.0 modification, 2009.09.29 */
	BOOL bCEUserTblEn;
	DWORD dwCETblAddr;
	DWORD dwCEUserTblAddr;
	DWORD *pdwCEUserTbl;
/* ======================================== */
	BOOL bPhotoLDCEn;
/* Version 6.3.0.0 modification, 2009.10.05 */
/* ======================================== */

	BYTE *pbySmallExpTbl;
	DWORD *pdwInvTbl;
	DWORD *pdwExpTbl;
/* ======================================== */

/* Version 3.2.0.0 modification, 2008.04.24 */
/* ======================================== */
/* Version 4.0.0.2 modification, 2008.10.06 */
	DWORD *pdwCETbl0;
	DWORD *pdwCETbl1;
	DWORD *pdwCETbl2;
	DWORD *pdwCETbl3;
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
	WORD *pwPhotoLDCTbl;
	DWORD dwPhotoLDCTblSize;
/* ======================================== */
/* Version 5.6.0.2 modification, 2009.02.20 */
	DWORD dwBusCycles;
	DWORD dwBusLatency;
	DWORD dwBusReqTimes;
/* ======================================== */
	DWORD dwPreSkipFrames;
/* Version 8.0.0.2 modification, 2010.05.17 */
    DWORD dwChipSubType;
/* Version 10.0.0.4 modification, 2011.10.21 */
    DWORD dwChipVersion;
/* ======================================== */
/* ======================================== */
} TVideoCapInfo;

/* ============================================================================================== */
SCODE VideoCap_ReleaseBufVIC(TVideoCapInfo *ptInfo, TVideoCapState *ptState);
SCODE VideoCap_GetBufVIC(TVideoCapInfo *ptInfo, TVideoCapState *ptState);
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_OpenVIC(TVideoCapInfo *ptInfo);
/* ======================================== */
SCODE VideoCap_CloseVIC(TVideoCapInfo *ptInfo);
SCODE VideoCap_WaitVIC(TVideoCapInfo *ptInfo);
SCODE VideoCap_StartVIC(TVideoCapInfo *ptInfo);
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_InitVIC(TVideoCapInfo *ptInfo, TVideoCapInitOptions *ptInitOptions);
SCODE VideoCap_StopVIC(TVideoCapInfo *ptInfo);
SCODE VideoCap_CheckVIC(TVideoCapInfo *ptInfo);
/* ======================================== */
/* Version 3.0.0.0 modification, 2007.09.03 */
SCODE VideoCap_LoadTblVIC(TVideoCapInfo *ptInfo);
/* ======================================== */
/* Version 3.1.0.6 modification, 2008.03.19 */
/* Version 4.0.0.2 modification, 2008.10.06 */
/* Version 5.10.0.5 modification, 2009.05.26 */
/* Version 6.2.0.0 modification, 2009.09.29 */
SCODE VideoCap_LoadCETblVIC(TVideoCapInfo *ptInfo, TVPLVICChnInfo *ptChnInfo);
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.6 modification, 2008.03.19 */
SCODE VideoCap_LoadPhotoLDCTblVIC(TVideoCapInfo *ptInfo);
/* ======================================== */
/* Version 3.2.0.0 modification, 2008.04.24 */
SCODE VideoCap_StartVICIs(TVideoCapInfo *ptInfo);
SCODE VideoCap_StopVICIs(TVideoCapInfo *ptInfo);
/* ======================================== */
/* ============================================================================================== */
#endif /*__VIDEOCAP_LOCALS_H__*/
