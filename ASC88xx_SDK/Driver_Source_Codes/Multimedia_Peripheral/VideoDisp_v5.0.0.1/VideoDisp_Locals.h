/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib/VideoDisp_Locals.h 17    13/01/15 6:52p Vincent $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: VideoDisp_Locals.h $
 * 
 * *****************  Version 17  *****************
 * User: Vincent      Date: 13/01/15   Time: 6:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Replace S_FAIL with error codes and refine the error
 * description for what-if analysis in the application - DONE.
 *
 * *****************  Version 16  *****************
 * User: Vincent      Date: 12/10/15   Time: 4:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 6.0.0.0
 * 2. FEATURE: Add VideoDisp_QueueBuf() and VideoDisp_DeQueueBuf() to
 * support dynamically feed display buffer address
 *
 * *****************  Version 15  *****************
 * User: Alan         Date: 12/06/26   Time: 1:43p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Change version from 4.1.0.0 to 4.0.0.3 - DONE.
 *
 * *****************  Version 14  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:33p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Modify profile mechanism. - DONE.
 *
 * *****************  Version 13  *****************
 * User: Evelyn       Date: 11/09/14   Time: 6:22p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 12  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 4.3.0.0.
 * 2. MODIFICATION: Support 16-bit YUV with embedded SYNC - DONE.
 *
 * *****************  Version 11  *****************
 * User: Alan         Date: 11/07/14   Time: 8:25p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add Mozart v1/v2 platform and model check mechanism - DONE.
 *
 * *****************  Version 10  *****************
 * User: Alan         Date: 11/07/14   Time: 2:59p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Remove video_error.h inclusion in VideoDisp_Locals.h -
 * DONE.
 *
 * *****************  Version 9  *****************
 * User: Alan         Date: 11/07/06   Time: 11:59a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add VideoDisp_WaitComplete function - DONE.
 *
 * *****************  Version 8  *****************
 * User: Alan         Date: 11/06/28   Time: 12:42a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Add adwPAL50Hz table - DONE.
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 11/01/12   Time: 5:39p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 10/04/29   Time: 12:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 10/04/26   Time: 11:35a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 2:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Add support VIDEO_SIGNAL_SIZE_720x480, VIDEO_SIGNAL_SIZE_1280x720 ,
 * VIDEO_SIGNAL_SIZE_1920x1080 - DONE.
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/12/05   Time: 9:28p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 *
 *
 */

/* ============================================================================================== */
#ifndef __VIDEODISP_LOCALS_H__
#define __VIDEODISP_LOCALS_H__

/* =========================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "typedef.h"
/* Version 3.4.0.0 modification, 2011.07.14 */
/* ======================================== */
/* Version 4.0.0.0 modification, 2011.09.14 */
#include "global_codec.h"
#include "MemMgr.h"
/* ======================================== */
#include "dbgdefs.h"

#include "VideoDisp.h"
/* Version 5.0.0.1 modification, 2013.01.14 */
#include "VideoDisp_Error.h"
/* ======================================== */
#include "vpl_voc.h"
/* Version 3.4.0.0 modification, 2011.07.14 */
#include "vpl_edmc.h"
/* ======================================== */

/* =========================================================================================== */
#define ALIGN_VALUE 2
#define ALIGN_PADDING 7
#define ALIGN_MASK 0xFFFFFFF8

/* Version 5.0.0.1 modification, 2013.01.14 */
#define ERRPRINT(fmt, args...)  printf("[VideoDisp][Error]: " fmt, ## args);
/* When updating VideoDisp_Error.h, please do not forget to update below macros !! */
#ifdef __NO_ERROR_CODE__
#undef _______VIDEODISP_ERR_INVALID_VERSION
#undef _______VIDEODISP_ERR_INVALID_ARG
#undef _______VIDEODISP_ERR_NOT_SUPPORT_SOC_PLATFORM
#undef _______VIDEODISP_ERR_NOT_SUPPORT_SOC_MODEL
#undef _______VIDEODISP_ERR_NOT_SUPPORT_OPTION_FLAGS
#undef _______VIDEODISP_ERR_OUT_OF_SYSTEM_MEMORY
#undef _______VIDEODISP_ERR_OUT_OF_HARDWARE_MEMORY
#undef _______VIDEODISP_ERR_OPEN_DEVICE_DRIVER
#undef _______VIDEODISP_ERR_CLOSE_DEVICE_DRIVER
#undef _______VIDEODISP_ERR_DEVICE_IO_ERROR

#define _______VIDEODISP_ERR_INVALID_VERSION            S_FAIL
#define _______VIDEODISP_ERR_INVALID_ARG                S_FAIL
#define _______VIDEODISP_ERR_NOT_SUPPORT_SOC_PLATFORM   S_FAIL
#define _______VIDEODISP_ERR_NOT_SUPPORT_SOC_MODEL      S_FAIL
#define _______VIDEODISP_ERR_NOT_SUPPORT_OPTION_FLAGS   S_FAIL
#define _______VIDEODISP_ERR_OUT_OF_SYSTEM_MEMORY       S_FAIL
#define _______VIDEODISP_ERR_OUT_OF_HARDWARE_MEMORY     S_FAIL
#define _______VIDEODISP_ERR_OPEN_DEVICE_DRIVER         S_FAIL
#define _______VIDEODISP_ERR_CLOSE_DEVICE_DRIVER        S_FAIL
#define _______VIDEODISP_ERR_DEVICE_IO_ERROR            S_FAIL
#endif //__NO_ERROR_CODE__
/* ======================================== */
/* =========================================================================================== */
/* Version 1.1.0.0 modification, 2010.02.26 */
/* Version 3.4.0.1 modification, 2011.08.31 */
extern const DWORD adwQQHDTV60Hz[8];
extern const DWORD adwVGA60Hz[8];
extern const DWORD adwNTSC60Hz[8];
/* ======================================== */
/* Version 3.2.0.0 modification, 2011.06.27 */
/* Version 3.4.0.1 modification, 2011.08.31 */
extern const DWORD adwPAL50Hz[8];
/* ======================================== */
/* ======================================== */
/* Version 3.4.0.1 modification, 2011.08.31 */
extern const DWORD adwPAL60Hz[8];
extern const DWORD adwWVGA60Hz[8];
extern const DWORD adw720p60Hz[8];
extern const DWORD adwSVGA60Hz[8];
extern const DWORD adwXGA60Hz[8];
extern const DWORD adwSXGA60Hz[8];
extern const DWORD adwWXGA60Hz[8];
extern const DWORD adwSXGA_960_60Hz[8];
extern const DWORD adwSXGAPLUS60Hz[8];
extern const DWORD adw1080p60Hz[8];
extern const DWORD adw1080p24Hz[8];
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.0 modification, 2011.01.12 */
/* Version 3.4.0.1 modification, 2011.08.31 */
extern const DWORD adw1080p30Hz[8];
/* ======================================== */
/* ======================================== */
extern const DWORD dwYCbCr2RGBBT601[10];

/* =========================================================================================== */
typedef struct video_display_info
{
    HANDLE hVOCDev;
    HANDLE hEDMCDev;
/* This structure pointer should be point to the MMR base address directly. */
    volatile TVPLVOCInfo *ptVOCInfo;
    BOOL bAllocateExternal;
    DWORD dwOrgAddress;
/* Version 3.4.0.0 modification, 2011.07.14 */
    DWORD dwChipVersion;
    DWORD dwChipSubType;
/* ======================================== */

    EPixelFormatFlags epixelInFormat;
    EVideoSignalFormat eOutFormat;
    DWORD dwProcessIndex;
    TVideoBuffer atVideoBuffer[4];
    EVideoDispOptionFlags eOptionFlags;
    DWORD dwInWidth;
    DWORD dwInHeight;
    DWORD dwInStride;
/* Version 3.0.0.0 modification, 2010.04.28 */
/* ======================================== */
    BOOL bPALEn;
    BYTE byBrightness;
    BYTE byContrast;
    DWORD dwSaturation;
    DWORD dwCoeffYR;
    DWORD dwCoeffYG;
    DWORD dwCoeffYB;
    DWORD dwCoeffCbG;
    DWORD dwCoeffCbB;
    DWORD dwCoeffCrR;
    DWORD dwCoeffCrG;
    DWORD dwYOffset;
    DWORD dwCbOffset;
    DWORD dwCrOffset;

    DWORD dwOutWidth;
    DWORD dwOutHeight;
    DWORD dwVsyncFrontPorch;
    DWORD dwVsyncBackPorch;
    DWORD dwHsyncFrontPorch;
    DWORD dwHsyncBackPorch;
/* Version 1.1.0.0 modification, 2010.02.26 */
    DWORD dwClkFreq;
/* Version 3.4.0.1 modification, 2011.08.31 */
    DWORD dwVICID;
    DWORD dwClkPol;
/* ======================================== */
/* Version 3.0.0.0 modification, 2010.04.28 */
    DWORD dwRatio;
    DWORD dwDivider;
    BOOL bOutCtlModifiedFlag;
/* ======================================== */
/* ======================================== */
/* Version 4.0.0.0 modification, 2011.09.14 */
    BOOL bCableDetected;
/* ======================================== */
/* Version 5.0.0.0 modification, 2012.10.15 */
    BOOL PreBufferMode;
/* ======================================== */
/* Version 4.0.0.3 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    DWORD dwFrameCnt;
    DWORD dwOpCycles;
    DWORD dwTotalBusLatency;
    DWORD dwPrevBusLatency;
    DWORD dwTotalBusReqTimes;
    DWORD dwPrevBusReqTimes;
    FLOAT flTotalBusCycles;
    DWORD dwPrevBusCycles;
#endif //__USE_PROFILE__
/* ======================================== */

} TVideoDispInfo;

/* =========================================================================================== */
SCODE VideoDisp_OpenVOC(TVideoDispInfo *ptInfo, TVideoDispInitOptions *ptInitOptions);
SCODE VideoDisp_CloseVOC(TVideoDispInfo *ptInfo);
SCODE VideoDisp_SetOptionsVOC(TVideoDispInfo *ptInfo);
SCODE VideoDisp_GetBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState);
SCODE VideoDisp_LoadTblVOC(TVideoDispInfo *ptInfo);
void VideoDisp_CalcBrightContrast(TVideoDispInfo *ptInfo);
SCODE VideoDisp_ReleaseBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState);
void VideoDisp_StartVOC(TVideoDispInfo *ptInfo);
/* Version 3.3.0.0 modification, 2011.07.05 */
void VideoDisp_WaitVOC(TVideoDispInfo *ptInfo);
/* ======================================== */
void VideoDisp_StopVOC(TVideoDispInfo *ptInfo);
SCODE VideoDisp_ResetVOC(TVideoDispInfo *ptInfo);
SCODE VideoDisp_CheckVOC(TVideoDispInfo *ptInfo);
SCODE VideoDisp_CableDetectVOC(TVideoDispInfo *ptInfo);
/* Version 5.0.0.0 modification, 2012.10.15 */
SCODE VideoDisp_QueueBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState);
SCODE VideoDisp_DeQueueBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState);
/* ======================================== */
/* =========================================================================================== */
#endif //__VIDEODISP_LOCALS_H__
