/*

/* ============================================================================================== */
#ifndef __VPL_VIC_H__
#define __VPL_VIC_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "global_codec.h"

/* ============================================================================================== */
#define VPL_VIC_VERSION MAKEFOURCC(12, 0, 0, 3)
#define VPL_VIC_ID_VERSION "12.0.0.3"

#define SENSOR_INTERFACE_VERSION MAKEFOURCC(1, 0, 0, 0)
#define SENSOR_INTERFACE_ID_VERSION "1.0.0.0"

/* ============================================================================================== */
#define VPL_VIC_BUS_NUM 1

#define VPL_VIC_MASTER_NUM  0x01

/* ============================================================================================== */
#define VPL_VIC_IOC_MAGIC 227

#define VPL_VIC_IOC_GET_VERSION_NUMBER			_IOR    (VPL_VIC_IOC_MAGIC, 0, DWORD)
#define VPL_VIC_IOC_WAIT_COMPLETE				_IO     (VPL_VIC_IOC_MAGIC, 1)
#define VPL_VIC_IOC_INITIAL						_IOW    (VPL_VIC_IOC_MAGIC, 2, TVPLVICInitOptions)
#define VPL_VIC_IOC_GET_BUF						_IOR    (VPL_VIC_IOC_MAGIC, 3, TVPLVICState)
#define VPL_VIC_IOC_RELEASE_BUF					_IOW    (VPL_VIC_IOC_MAGIC, 4, DWORD)
#define VPL_VIC_IOC_SETOPTIONS					_IOW    (VPL_VIC_IOC_MAGIC, 5, TVPLVICOptions)
#define VPL_VIC_IOC_GET_BANDWIDTH				_IOR    (VPL_VIC_IOC_MAGIC, 6, DWORD)
#define VPL_VIC_IOC_GET_RG_INTERVAL				_IOR    (VPL_VIC_IOC_MAGIC, 7, DWORD)
#define VPL_VIC_IOC_GET_REQ_TIMES				_IOR    (VPL_VIC_IOC_MAGIC, 8, DWORD)
#define VPL_VIC_IOC_CLEAR_PROFILE				_IO     (VPL_VIC_IOC_MAGIC, 18)
#define VPL_VIC_IOC_STOP_CAPTURE                _IO     (VPL_VIC_IOC_MAGIC, 9)
#define VPL_VIC_IOC_CHECK_INIT                  _IO     (VPL_VIC_IOC_MAGIC, 10)
#define VPL_VIC_IOC_GET_FB_BASEADDR             _IOR    (VPL_VIC_IOC_MAGIC, 11, DWORD)
#define VPL_VIC_IOC_GET_FB_SIZE                 _IOR    (VPL_VIC_IOC_MAGIC, 12, DWORD)
#define VPL_VIC_IOC_WAIT_TBL_COMPLETE           _IO     (VPL_VIC_IOC_MAGIC, 13)
#define VPL_VIC_IOC_WAIT_CE_TBL_COMPLETE        _IO     (VPL_VIC_IOC_MAGIC, 14)
#define VPL_VIC_IOC_WAIT_PHOTO_LDC_TBL_COMPLETE	_IO     (VPL_VIC_IOC_MAGIC, 15)
#define VPL_VIC_IOC_IIC_EN                      _IO     (VPL_VIC_IOC_MAGIC, 16)
#define VPL_VIC_IOC_IIC_DE                      _IO     (VPL_VIC_IOC_MAGIC, 17)
#define VPL_VIC_IOC_GET_AF_TBL_SIZE     _IOR    (VPL_VIC_IOC_MAGIC, 18, DWORD)
#define VPL_VIC_IOC_START_VIC     _IO    (VPL_VIC_IOC_MAGIC, 19)
#define VPL_VIC_IOC_SET_COLOR_TEMPERATURE	_IOW    (VPL_VIC_IOC_MAGIC, 20, DWORD*)
/* Version 11.0.0.3 modification, 2012.11.02 */
#define VPL_VIC_IOC_GET_STAT_GRID_BUFFER_BASEADDR	_IOR    (VPL_VIC_IOC_MAGIC, 21, DWORD)
#define VPL_VIC_IOC_GET_STAT_GRID_BUFFER_SIZE	_IOR    (VPL_VIC_IOC_MAGIC, 22, DWORD)
#define VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_BASEADDR	_IOR    (VPL_VIC_IOC_MAGIC, 23, DWORD)
#define VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_SIZE	_IOR    (VPL_VIC_IOC_MAGIC, 24, DWORD)
#define VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_BASEADDR	_IOR    (VPL_VIC_IOC_MAGIC, 25, DWORD)
#define VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_SIZE	_IOR    (VPL_VIC_IOC_MAGIC, 26, DWORD)

#define VPL_VIC_IOC_MAX_NUMBER  27
/* ======================================== */
/* ============================================================================================== */
#define VPL_VIC_CHN_NUM     8

/* ============================================================================================== */
typedef struct vpl_vic_init_options
{
    DWORD dwBufNum;
    DWORD dwMask;
    DWORD dwPhyBaseAddr;
    BOOL bOutFormat;
    DWORD dwMaxFrameWidth;
    DWORD dwMaxFrameHeight;
    DWORD dwFrameRate;
    DWORD dwStatAEWBPhyBaseAddr;
    DWORD dwStatHistoPhyBaseAddr;
    DWORD dwStatFocusPhyBaseAddr;
} TVPLVICInitOptions;

typedef struct vpl_vic_options
{
    EVideoSignalOptionFlags eOptionFlags;
    DWORD adwUserData[7];
} TVPLVICOptions;

typedef struct vpl_vic_state
{
    DWORD dwIndex;
    DWORD dwFrameCount;
    DWORD dwSecond;
    DWORD dwMilliSecond;
    DWORD dwMicroSecond;
    BOOL bTimeStampCal;
    DWORD dwInWidth;
    DWORD dwOutWidth;
    DWORD dwOutHeight;
    DWORD dwOutStride;
    DWORD dwErrACK;
    DWORD dwYFrameAddr;
    DWORD dwCbFrameAddr;
    DWORD dwCrFrameAddr;
    DWORD dwAFFocusValueHigh;
    DWORD dwAFFocusValueLow;
    DWORD dwAFCurrentStatus;
    QWORD qwAWBRedSum;
    QWORD qwAWBGreenSum;
    QWORD qwAWBBlueSum;
    DWORD dwAWBGain;
    DWORD dwShutter; 
    DWORD dwGain; 	
    DWORD dwZoomRatio; 	
    BOOL bAEStable;
} TVPLVICState;

typedef struct vpl_vic_chn_info
{
    DWORD dwMilliSecond;
    DWORD dwYAddr0;
    DWORD dwCbAddr0;
    DWORD dwCrAddr0;
    DWORD dwYAddr1;
    DWORD dwCbAddr1;
    DWORD dwCrAddr1;
    DWORD dwInSize;
    DWORD dwCapH;
    DWORD dwCapV;
    DWORD dwStride;
    DWORD dwSBC;
    DWORD dwCECtrl;
    DWORD dwCETblAddr;
} TVPLVICChnInfo;

typedef struct vpl_vic_info
{
    DWORD dwVersion;
    DWORD dwCtrl;
    DWORD adwDevCtrl[2];
    DWORD adwChanCtrl[8];
    DWORD dwStat0;
    DWORD dwStat1;
    DWORD adwSyncDelay[2];
    TVPLVICChnInfo atChnInfo[8];
    DWORD dwNoSig;
    DWORD dwCMOSCtrl;               // MMR 129
    DWORD dwRGB2YCbCrCoeff0;        // MMR 130
    DWORD dwRGB2YCbCrCoeff1;        // MMR 131
    DWORD dwRGB2YCbCrCoeff2;        // MMR 132
    DWORD dwABGGainBond;             // MMR 133
    DWORD dwAWBCtrl;                // MMR 134
    DWORD dwAWBThr;               // MMR 135
    DWORD dwRESERVED0;         // MMR 136
    DWORD dwRESERVED1;         // MMR 137
    DWORD dwRESERVED2;       // MMR 138
    DWORD dwRESERVED3;       // MMR 139
    DWORD dwManualGain;             // MMR 140
    DWORD dwCCCoeffR;               // MMR 141
    DWORD dwCCCoeffG;               // MMR 142
    DWORD dwCCCoeffB;               // MMR 143
    DWORD dwGammaTblAddr;           // MMR 144
    DWORD dwAFCtrl;                // MMR 145
    DWORD dwAFWindowStartPosition;  // MMR 146
    DWORD dwAFWindowSize;           // MMR 147
    DWORD dwAFFocusValueHigh;       // MMR 148
    DWORD dwAFFocusValueLow;        // MMR 149
    DWORD dwDenoiseG0Thrd;        // MMR 150
    DWORD dwDenoiseG0MaxThrd;         // MMR 151
    DWORD dwDenoiseG0MinThrd;         // MMR 152
    DWORD dwDenoiseRThrd;         // MMR 153
    DWORD dwDenoiseRMaxThrd;         // MMR 154
    DWORD dwDenoiseRMinThrd;         // MMR 155
    DWORD dwDenoiseBThrd;         // MMR 156
    DWORD dwDenoiseBMaxThrd;         // MMR 157
    DWORD dwDenoiseBMinThrd;         // MMR 158
    DWORD dwDenoiseG1Thrd;         // MMR 159
    DWORD dwDenoiseG1MaxThrd;         // MMR 160
    DWORD dwDenoiseG1MinThrd;         // MMR 161
    DWORD dwCMOSHorPenaltyThr;      // MMR 162
    DWORD dwCMOSVerPenaltyThr;      // MMR 163
    DWORD dwBayerCFAHVAvgThrd;      // MMR 164
    DWORD dwBayerCFAHVBlendMaxThrd;      // MMR 165
    DWORD dwBayerCFAHVBlendMinThrd;      // MMR 166
    DWORD dwBayerCFAFCSEdgeThrd;      // MMR 167
    DWORD dwBayerCFAFCSMaxThrd;      // MMR 168
    DWORD dwBayerCFAFCSMinThrd ;      // MMR 169
    DWORD dwBayerCFAFCSCtrl;      // MMR 170	
    DWORD dwCMYGCtrl0;              // MMR 171
    DWORD dwCMYGCtrl1;              // MMR 172
    DWORD dwYCbCrClip;              // MMR 173
    DWORD dwTMCtrl;                 // MMR 174
    DWORD dwPhotoLDCCtrl;           // MMR 175
    DWORD dwPhotoLDCTblAddr;        // MMR 176
    DWORD dwStatBuffAddr;     // MMR 177
    DWORD dwBayerClamp;        // MMR 178
    DWORD dwBayerGain;     // MMR 179
    DWORD dwAWBWindowStart;    // MMR 180	
    DWORD dwAWBWindowRange;    // MMR 181
    DWORD dwAWBRedHighSum;   // MMR 182
    DWORD dwAWBRedLowSum;               // MMR 183
    DWORD dwAWBGreenHighSum;               // MMR 184
    DWORD dwAWBGreenLowSum;     // MMR 185
    DWORD dwAWBBlueHighSum;  // MMR 186
    DWORD dwAWBBlueLowSum;                // MMR 187
} TVPLVICInfo;


typedef struct vpl_vic_isp_param
{
    BOOL bCMOSCtrl;
    DWORD dwCMOSCtrl;

    BOOL bCECtrl;
    DWORD dwCECtrl;
    DWORD dwCETblAddr;

    BOOL bTMCtrl;
    DWORD dwTMCtrl;

    BOOL bCCCtrl;
    DWORD dwCCCoeffR;
    DWORD dwCCCoeffG;
    DWORD dwCCCoeffB;

    BOOL bSBCCtrl;
    DWORD dwSBC;

    BOOL bAntialiasingCtrl;
    DWORD dwBayerCFAHVAvgThrd;
    DWORD dwBayerCFAHVBlendMaxThrd;
    DWORD dwBayerCFAHVBlendMinThrd;
    DWORD dwBayerCFAFCSEdgeThrd;
    DWORD dwBayerCFAFCSMaxThrd;
    DWORD dwBayerCFAFCSMinThrd;
    DWORD dwBayerCFAFCSCtrl;

    BOOL bAECtrl;
    DWORD dwTargetLuma;
    DWORD dwTargetOffset;
    DWORD dwIrisActiveTime;
    DWORD dwMinShutter;
    DWORD dwMaxShutter;
    DWORD dwMinGain;
    DWORD dwMaxGain;
/* Version 12.0.0.0 modification, 2013.04.19 */
    DWORD dwAEMode;
    BOOL bAELock;
    DWORD dwAESpeed;
    BOOL dwIrisStatus;
/* ======================================== */
/* Version 11.0.0.3 modification, 2012.11.02 */
    BOOL bDeimpulseCtrl;
    DWORD dwDeimpulseCtrl;
    DWORD dwDenoiseThrd;
    DWORD dwDenoiseMaxThrd;
    DWORD dwDenoiseMinThrd;

    BOOL bBlackClampCtrl;
    DWORD dwBlackClamp;

    BOOL bCFAModeCtrl;
    DWORD dwCFAMode;	
/* ======================================== */

/* Version 12.0.0.0 modification, 2013.04.19 */
    BOOL bAWBCtrl;
    BOOL bAWBLock;
    DWORD dwAWBMode;
    DWORD dwCustomGainR;
    DWORD dwCustomGainB;
/* ======================================== */
} TVPLVICISPParam;

/* ============================================================================================== */
#endif //__VPL_VIC_H__

/* ============================================================================================== */
