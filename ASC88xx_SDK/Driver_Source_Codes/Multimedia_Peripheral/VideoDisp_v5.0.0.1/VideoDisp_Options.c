/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib/VideoDisp_Options.c 13    13/01/15 6:52p Vincent $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: VideoDisp_Options.c $
 * 
 * *****************  Version 13  *****************
 * User: Vincent      Date: 13/01/15   Time: 6:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Replace S_FAIL with error codes and refine the error
 * description for what-if analysis in the application - DONE.
 *
 * *****************  Version 12  *****************
 * User: Vincent      Date: 12/10/15   Time: 4:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 6.0.0.0
 * 2. FEATURE: Add VideoDisp_QueueBuf() and VideoDisp_DeQueueBuf() to
 * support dynamically feed display buffer address
 *
 * *****************  Version 11  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:32p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * Fix complier warnings. - DONE.
 *
 * *****************  Version 10  *****************
 * User: Evelyn       Date: 11/09/14   Time: 6:22p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 9  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 4.3.0.0.
 * 2. MODIFICATION: Support 16-bit YUV with embedded SYNC - DONE.
 *
 * *****************  Version 8  *****************
 * User: Alan         Date: 11/07/14   Time: 8:25p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add Mozart v1/v2 platform and model check mechanism - DONE.
 *
 * *****************  Version 7  *****************
 * User: Alan         Date: 11/07/14   Time: 3:01p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Support 16-bit YUV with separate or embedded SYNC - DONE.
 *
 * *****************  Version 6  *****************
 * User: Alan         Date: 11/07/01   Time: 11:32a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add 704x480 to NTSC output size - DONE.
 *
 * BUG: The output size of PAL should be 704x576 and 720x576 instead of
 * 768x576 - FIXED.
 *
 * MODIFICATION: Use printf insted of DBPRINT in VideoDisp_Options.c -
 * DONE.
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 11/01/12   Time: 5:39p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 10/04/29   Time: 12:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/12/05   Time: 9:28p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 *
 *
 */
/* ============================================================================================== */
#include "VideoDisp_Locals.h"

/* =========================================================================================== */
DWORD VideoDisp_QueryMemSize(TVideoDispInitOptions *ptInitOptions)
{
    return (sizeof(TVideoDispInfo)+ALIGN_PADDING);
}

/* =========================================================================================== */
SCODE VideoDisp_SetOptions(HANDLE hObject, TVideoDispOptions *ptOptions)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);
/* Version 3.0.0.0 modification, 2010.04.28 */
    TVideoDispOutCtrl *ptDispOutCtrl;
/* ======================================== */
    DWORD dwYSize, dwCbCrSize, dwFrameSize, dwInAddr;
    DWORD i;
/* Version 3.1.0.0 modification, 2011.01.12 */
    const DWORD *pdwTimingTable;
/* ======================================== */

    ptInfo->eOptionFlags = ptOptions->eOptionFlags;

    switch (ptInfo->eOptionFlags)
    {
        case VIDEO_DISP_OPTION_SET_INPUT:
/* Version 3.0.0.0 modification, 2010.04.28 */
            ptInfo->dwInWidth = ptOptions->adwUserData[0];
            ptInfo->dwInHeight = ptOptions->adwUserData[1];
            ptInfo->dwInStride = ptOptions->adwUserData[2];
            ptInfo->epixelInFormat = ptOptions->adwUserData[3];
/* ======================================== */
            break;
        case VIDEO_DISP_OPTION_SET_OUTPUT_CTRL:
/* Version 3.0.0.0 modification, 2010.04.28 */
            ptDispOutCtrl = (TVideoDispOutCtrl *)ptOptions->adwUserData[0];
            if (ptDispOutCtrl==NULL)
            {
/* Version 3.2.0.0 modification, 2011.07.01 */
/* Version 5.0.0.1 modification, 2013.01.14 */
                ERRPRINT("The user data adwUserData[0] could not be null.\n");
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.1 modification, 2013.01.14 */
                return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
            }
            ptInfo->dwOutWidth = ptDispOutCtrl->dwHsyncTotalLength;
            ptInfo->dwOutHeight = ptDispOutCtrl->dwVsyncTotalLength;
            ptInfo->dwVsyncFrontPorch = ptDispOutCtrl->dwVsyncFrontPorch;
            ptInfo->dwVsyncBackPorch = ptDispOutCtrl->dwVsyncBackPorch;
            ptInfo->dwHsyncFrontPorch = ptDispOutCtrl->dwHsyncFrontPorch;
            ptInfo->dwHsyncBackPorch = ptDispOutCtrl->dwHsyncBackPorch;
            ptInfo->dwRatio = ptDispOutCtrl->dwRatio;
            ptInfo->dwDivider = ptDispOutCtrl->dwDivider;
            ptInfo->bOutCtlModifiedFlag = TRUE;
/* ======================================== */
            break;
        case VIDEO_DISP_OPTION_SET_PIXEL_FORMAT:
/* Version 3.4.0.0 modification, 2011.07.14 */
            if ((((EVideoSignalFormat)ptOptions->adwUserData[0]==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120)||
                 ((EVideoSignalFormat)ptOptions->adwUserData[0]==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS)) &&
                (ptInfo->dwChipVersion==1))
            {
/* Version 5.0.0.1 modification, 2013.01.14 */
                ERRPRINT("BT1120/16-bit YUV Separate Sync are not supported by this SoC platform.\n");
                return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
            }
/* ======================================== */
            ptInfo->eOutFormat = ptOptions->adwUserData[0];
            break;
        case VIDEO_DISP_OPTION_SET_PAL:
            ptInfo->bPALEn = ptOptions->adwUserData[0];
            break;
        case VIDEO_DISP_OPTION_SET_DISPLAY_BUFFER:
            dwYSize = ptOptions->adwUserData[0] * ptOptions->adwUserData[1];
/* Version 3.0.0.0 modification, 2010.04.28 */
/* ======================================== */
            if (ptInfo->epixelInFormat == YUV420)
            {
                dwCbCrSize = dwYSize >> 2;
            }
            else if (ptInfo->epixelInFormat == YUV422)
            {
                dwCbCrSize = dwYSize >> 1;
            }
            else
            {

/* Version 3.2.0.0 modification, 2011.07.01 */
/* Version 5.0.0.1 modification, 2013.01.14 */
                ERRPRINT("Unsupported input format %d. Only YUV420 (1) and YUV422 (2) are supported !!\n", ptInfo->epixelInFormat);
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.1 modification, 2013.01.14 */
                return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
            }
/* Version 3.0.0.0 modification, 2010.04.28 */
            dwFrameSize = (dwYSize+(dwCbCrSize<<1)+ALIGN_PADDING) & ALIGN_MASK;
            dwInAddr = (((DWORD)ptOptions->adwUserData[2]+ALIGN_PADDING)&ALIGN_MASK);
/* ======================================== */
            for (i=0; i<4; i++)
            {
/* Version 5.0.0.0 modification, 2012.10.15 */
                ptInfo->atVideoBuffer[i].dwIndex = i;
/* ======================================== */
                ptInfo->atVideoBuffer[i].pbyYFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev,  dwInAddr+ dwFrameSize*i);
                ptInfo->atVideoBuffer[i].pbyCbFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, dwInAddr + dwYSize + dwFrameSize*i);
                ptInfo->atVideoBuffer[i].pbyCrFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, dwInAddr + dwYSize + dwCbCrSize + dwFrameSize*i);
            }
            break;
        case VIDEO_DISP_OPTION_SET_BRIGHTNESS:
            ptInfo->byBrightness = ptOptions->adwUserData[0] & 0xFF;
            break;
        case VIDEO_DISP_OPTION_SET_CONTRAST:
            ptInfo->byContrast = ptOptions->adwUserData[0] & 0xFF;
            break;
        case VIDEO_DISP_OPTION_SET_SATURATION:
            ptInfo->dwSaturation = ptOptions->adwUserData[0];
            break;
/* Version 3.1.0.0 modification, 2011.01.12 */
        case VIDEO_DISP_OPTION_SET_OUTPUT_SIZE:
/* Version 3.4.0.0 modification, 2011.07.13 */
            if ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24) ||
                (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120) ||
                (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
/* ======================================== */
            {
                if (ptOptions->adwUserData[1]==VIDEO_SIGNAL_FREQUENCY_60HZ)
                {
                    switch (ptOptions->adwUserData[0])
                    {
                        case VIDEO_SIGNAL_SIZE_480x272:
                            pdwTimingTable = adwQQHDTV60Hz;
                            ptInfo->dwInWidth = 480;
                            ptInfo->dwInHeight = 272;
                        break;
                        case VIDEO_SIGNAL_SIZE_640x480:
                            pdwTimingTable = adwVGA60Hz;
                            ptInfo->dwInWidth = 640;
                            ptInfo->dwInHeight = 480;
                        break;
/* Version 3.2.0.0 modification, 2011.06.27 */
                        case VIDEO_SIGNAL_SIZE_704x480:
                            pdwTimingTable = adwNTSC60Hz;
                            ptInfo->dwInWidth = 704;
                            ptInfo->dwInHeight = 480;
                        break;
/* ======================================== */
                        case VIDEO_SIGNAL_SIZE_720x480:
                            pdwTimingTable = adwNTSC60Hz;
                            ptInfo->dwInWidth = 720;
                            ptInfo->dwInHeight = 480;
                        break;
                        case VIDEO_SIGNAL_SIZE_768x576:
                            pdwTimingTable = adwPAL60Hz;
                            ptInfo->dwInWidth = 768;
                            ptInfo->dwInHeight = 576;
                        break;
                        case VIDEO_SIGNAL_SIZE_800x480:
                            pdwTimingTable = adwWVGA60Hz;
                            ptInfo->dwInWidth = 800;
                            ptInfo->dwInHeight = 480;
                        break;
                        case VIDEO_SIGNAL_SIZE_800x600:
                            pdwTimingTable = adwSVGA60Hz;
                            ptInfo->dwInWidth = 800;
                            ptInfo->dwInHeight = 600;
                        break;
                        case VIDEO_SIGNAL_SIZE_1024x768:
                            pdwTimingTable = adwXGA60Hz;
                            ptInfo->dwInWidth = 1024;
                            ptInfo->dwInHeight = 768;
                        break;
                        case VIDEO_SIGNAL_SIZE_1280x768:
                            pdwTimingTable = adwWXGA60Hz;
                            ptInfo->dwInWidth = 1280;
                            ptInfo->dwInHeight = 768;
                        break;
                        case VIDEO_SIGNAL_SIZE_1280x720:
                            pdwTimingTable = adw720p60Hz;
                            ptInfo->dwInWidth = 1280;
                            ptInfo->dwInHeight = 720;
                        break;
                        case VIDEO_SIGNAL_SIZE_1280x960:
                            pdwTimingTable = adwSXGA_960_60Hz;
                            ptInfo->dwInWidth = 1280;
                            ptInfo->dwInHeight = 960;
                        break;
                        case VIDEO_SIGNAL_SIZE_1280x1024:
                            pdwTimingTable = adwSXGA60Hz;
                            ptInfo->dwInWidth = 1280;
                            ptInfo->dwInHeight = 1024;
                        break;
                        case VIDEO_SIGNAL_SIZE_1400x1050:
                            pdwTimingTable = adwSXGAPLUS60Hz;
                            ptInfo->dwInWidth = 1400;
                            ptInfo->dwInHeight = 1050;
                        break;
                        case VIDEO_SIGNAL_SIZE_1920x1080:
                            pdwTimingTable = adw1080p60Hz;
                            ptInfo->dwInWidth = 1920;
                            ptInfo->dwInHeight = 1080;
                        break;
                        default:
/* Version 3.2.0.0 modification, 2011.07.01 */
/* Version 5.0.0.1 modification, 2013.01.14 */
                            ERRPRINT("Unsupport output size %ld at refresh rate %ld !!\n", ptOptions->adwUserData[0], ptOptions->adwUserData[1]);
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.1 modification, 2013.01.14 */
                            ERRPRINT("Please refer to the user guide for detailed actions.\n");
                            return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
                    }
                }
                else if (ptOptions->adwUserData[1]==VIDEO_SIGNAL_FREQUENCY_24HZ)
                {
                    switch (ptOptions->adwUserData[0])
                    {
                        case VIDEO_SIGNAL_SIZE_1920x1080:
                            pdwTimingTable = adw1080p24Hz;
                            ptInfo->dwInWidth = 1920;
                            ptInfo->dwInHeight = 1080;
                        break;
                        default:
/* Version 3.2.0.0 modification, 2011.07.01 */
/* Version 5.0.0.1 modification, 2013.01.14 */
                            ERRPRINT("Unsupport output size %ld at refresh rate %ld !!\n", ptOptions->adwUserData[0], ptOptions->adwUserData[1]);
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.1 modification, 2013.01.14 */
                            ERRPRINT("Please refer to the user guide for detailed actions.\n");
                            return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
                    }
                }
                else if (ptOptions->adwUserData[1]==VIDEO_SIGNAL_FREQUENCY_30HZ)
                {
                    switch (ptOptions->adwUserData[0])
                    {
                        case VIDEO_SIGNAL_SIZE_1920x1080:
                            pdwTimingTable = adw1080p30Hz;
                            ptInfo->dwInWidth = 1920;
                            ptInfo->dwInHeight = 1080;
                        break;
                        default:
/* Version 3.2.0.0 modification, 2011.07.01 */
/* Version 5.0.0.1 modification, 2013.01.14 */
                            ERRPRINT("Unsupport output size %ld at refresh rate %ld !!\n", ptOptions->adwUserData[0], ptOptions->adwUserData[1]);
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.1 modification, 2013.01.14 */
                            ERRPRINT("Please refer to the user guide for detailed actions.\n");
                            return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
                    }
                }
                else
                {
/* Version 3.2.0.0 modification, 2011.07.01 */
/* Version 5.0.0.1 modification, 2013.01.14 */
                    ERRPRINT("Unsupport refresh rate %ld !!\n", ptOptions->adwUserData[1]);
/* ======================================== */
/* ======================================== */
/* Version 5.0.0.1 modification, 2013.01.14 */
                    ERRPRINT("Please refer to the user guide for detailed actions.\n");
                    return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
                }
                ptInfo->dwOutWidth = pdwTimingTable[0];
                ptInfo->dwOutHeight = pdwTimingTable[1];
                ptInfo->dwVsyncFrontPorch = pdwTimingTable[2];
                ptInfo->dwVsyncBackPorch = pdwTimingTable[3];
                ptInfo->dwHsyncFrontPorch = pdwTimingTable[4];
                ptInfo->dwHsyncBackPorch = pdwTimingTable[5];
                ptInfo->dwClkFreq = pdwTimingTable[6];
/* Version 3.4.0.1 modification, 2011.08.31 */
                ptInfo->dwVICID =  pdwTimingTable[7];
/* ======================================== */
                ptInfo->dwClkPol = 0x1;
            }
            else// (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)
            {
                switch (ptOptions->adwUserData[0])
                {
/* Version 3.2.0.0 modification, 2011.06.27 */
                    case VIDEO_SIGNAL_SIZE_704x480:
                        pdwTimingTable = adwNTSC60Hz;
                        ptInfo->dwInWidth = 704;
                        ptInfo->dwInHeight = 480;
                        ptInfo->dwOutWidth = 858;
                        ptInfo->dwOutHeight = 525;
                        ptInfo->ptVOCInfo->dwOutSizeCtrl = 262 | (2<<16) | (3<<20);
                        ptInfo->bPALEn = FALSE;
                    break;
/* ======================================== */
                    case VIDEO_SIGNAL_SIZE_720x480:
                        pdwTimingTable = adwNTSC60Hz;
                        ptInfo->dwInWidth = 720;
                        ptInfo->dwInHeight = 480;
                        ptInfo->dwOutWidth = 858;
                        ptInfo->dwOutHeight = 525;
                        ptInfo->ptVOCInfo->dwOutSizeCtrl = 262 | (2<<16) | (3<<20);
                        ptInfo->bPALEn = FALSE;
                    break;
/* Version 3.2.0.0 modification, 2011.06.27 */
                    case VIDEO_SIGNAL_SIZE_704x576:
                        pdwTimingTable = adwPAL50Hz;
                        ptInfo->dwInWidth = 704;
                        ptInfo->dwInHeight = 576;
                        ptInfo->dwOutWidth = 864;
                        ptInfo->dwOutHeight = 625;
                        ptInfo->ptVOCInfo->dwOutSizeCtrl = 312 | (2<<16) | (2<<20);
                        ptInfo->bPALEn = TRUE;
                    break;
                    case VIDEO_SIGNAL_SIZE_720x576:
                        pdwTimingTable = adwPAL50Hz;
                        ptInfo->dwInWidth = 720;
                        ptInfo->dwInHeight = 576;
                        ptInfo->dwOutWidth = 864;
                        ptInfo->dwOutHeight = 625;
                        ptInfo->ptVOCInfo->dwOutSizeCtrl = 312 | (2<<16) | (2<<20);
                        ptInfo->bPALEn = TRUE;
                    break;
                    break;
                    default:
/* Version 5.0.0.1 modification, 2013.01.14 */
                        ERRPRINT("Unsupport output size %ld !!\n", ptOptions->adwUserData[0]);
                        ERRPRINT("Please refer to the user guide for detailed actions.\n");
                        return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
/* ======================================== */
                }
                ptInfo->dwVsyncFrontPorch = 0;
                ptInfo->dwVsyncBackPorch = 0;
                ptInfo->dwHsyncFrontPorch = 0;
                ptInfo->dwHsyncBackPorch = 0;
                ptInfo->dwClkFreq = 27;
                ptInfo->dwClkPol = 0x1;
            }
            break;
/* ======================================== */
/* Version 4.0.0.0 modification, 2011.09.14 */
        case VIDEO_DISP_OPTION_CABLE_DETECT:
            if (VideoDisp_CableDetectVOC(ptInfo) != S_OK)
            {
/* Version 5.0.0.1 modification, 2013.01.14 */
                ERRPRINT("Get analog video encoder cable detect status fail !!\n");
                return _______VIDEODISP_ERR_DEVICE_IO_ERROR;
/* ======================================== */
            }
            else
            {
                *((BOOL *)(ptOptions->adwUserData[0])) = ptInfo->bCableDetected;
                return S_OK;
            }
/* ======================================== */
        default:
/* Version 5.0.0.1 modification, 2013.01.14 */
            ERRPRINT("Unsupported Option !!\n");
            return _______VIDEODISP_ERR_NOT_SUPPORT_OPTION_FLAGS;
/* ======================================== */
    }

    if(VideoDisp_SetOptionsVOC(ptInfo) != S_OK)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Unsupported Option !!\n");
        return _______VIDEODISP_ERR_NOT_SUPPORT_OPTION_FLAGS;
/* ======================================== */
    }

    return S_OK;
}

/* =========================================================================================== */
SCODE VideoDisp_GetVersionInfo(BYTE *pbyMajor, BYTE *pbyMinor, BYTE *pbyBuild, BYTE *pbyRevision)
{
    *pbyMajor = VIDEODISP_VERSION & 0x000000FF;
    *pbyMinor = (VIDEODISP_VERSION>>8) & 0x000000FF;
    *pbyBuild = (VIDEODISP_VERSION>>16) & 0x000000FF;
    *pbyRevision = (VIDEODISP_VERSION>>24) & 0x000000FF;

    return S_OK;
}

/* =========================================================================================== */
