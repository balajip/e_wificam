/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib/VPL_VOC_Tools.c 27    13/01/16 10:52a Vincent $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: VPL_VOC_Tools.c $
 * 
 * *****************  Version 27  *****************
 * User: Vincent      Date: 13/01/16   Time: 10:52a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Add profile to queue method
 *
 * *****************  Version 26  *****************
 * User: Vincent      Date: 13/01/15   Time: 6:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Replace S_FAIL with error codes and refine the error
 * description for what-if analysis in the application - DONE.
 *
 * *****************  Version 25  *****************
 * User: Vincent      Date: 12/10/15   Time: 4:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 6.0.0.0
 * 2. FEATURE: Add VideoDisp_QueueBuf() and VideoDisp_DeQueueBuf() to
 * support dynamically feed display buffer address
 *
 * *****************  Version 24  *****************
 * User: Alan         Date: 12/06/26   Time: 1:43p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Change version from 4.1.0.0 to 4.0.0.3 - DONE.
 *
 * *****************  Version 23  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:33p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Modify profile mechanism. - DONE.
 *
 * *****************  Version 22  *****************
 * User: Bernard      Date: 12/02/07   Time: 3:43p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 21  *****************
 * User: Evelyn       Date: 11/11/29   Time: 4:00p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 20  *****************
 * User: Evelyn       Date: 11/09/14   Time: 6:22p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 19  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 4.3.0.0.
 * 2. MODIFICATION: Support 16-bit YUV with embedded SYNC - DONE.
 *
 * *****************  Version 18  *****************
 * User: Alan         Date: 11/07/14   Time: 3:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Support 16-bit YUV with separate or embedded SYNC - DONE.
 *
 * *****************  Version 17  *****************
 * User: Alan         Date: 11/07/06   Time: 11:59a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add VideoDisp_WaitComplete function - DONE.
 *
 * *****************  Version 16  *****************
 * User: Albert.shen  Date: 11/03/08   Time: 11:40a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 15  *****************
 * User: Albert.shen  Date: 11/03/07   Time: 9:04p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 14  *****************
 * User: Albert.shen  Date: 11/01/26   Time: 1:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 13  *****************
 * User: Albert.shen  Date: 11/01/14   Time: 6:17p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 12  *****************
 * User: Albert.shen  Date: 11/01/12   Time: 5:39p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 11  *****************
 * User: Albert.shen  Date: 11/01/02   Time: 10:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION : VideoDisp_GetBufVOC would not wait complete - DONE
 *
 * *****************  Version 10  *****************
 * User: Albert.shen  Date: 10/09/17   Time: 4:05p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 10/04/29   Time: 12:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 10/03/10   Time: 4:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 10/03/10   Time: 4:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 10/03/10   Time: 3:50p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 2:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. MODIFICATION: Use ioctl to start VOC and stop VOC.
 * 2. MODIFICATION: Pass the frequency of output clock to the driver.
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 11:21a
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
SCODE VideoDisp_OpenVOC(TVideoDispInfo *ptInfo, TVideoDispInitOptions *ptInitOptions)
{
    if ((ptInfo->hVOCDev=(HANDLE)open("/dev/vpl_voc", O_RDWR)) < 0)
    {
        DBPRINT0("Open VOC device err!\n");
        return S_FAIL;
    }
    ptInfo->ptVOCInfo = (TVPLVOCInfo *)mmap(0,
                                            sizeof(TVPLVOCInfo),
                                            PROT_READ|PROT_WRITE,
                                            MAP_SHARED,
                                            (int)ptInfo->hVOCDev,
                                            0);
    if (((DWORD)ptInfo->ptVOCInfo) == 0xFFFFFFFF)
    {
        DBPRINT0("MMAP VOC MMR err!\n");
        return S_FAIL;
    }
    return S_OK;
}

/* =========================================================================================== */
SCODE VideoDisp_CloseVOC(TVideoDispInfo *ptInfo)
{
/* Version 2.0.0.2 modification, 2010.04.26 */
    if (munmap((void *)ptInfo->ptVOCInfo, sizeof(TVPLVOCInfo)) != 0)
    {
        DBPRINT0("Unmap MMR failed !!\n");
        return S_FAIL;
    }
/* ======================================== */

    if (close((int)ptInfo->hVOCDev) != S_OK)
    {
        return S_FAIL;
    }
    return S_OK;
}

/* =========================================================================================== */
void VideoDisp_StartVOC(TVideoDispInfo *ptInfo)
{
    DWORD dwInFormat, dwOutFormat, dwStride, dwHeight;
    volatile TVPLVOCInfo *ptVOCInfo = ptInfo->ptVOCInfo;
/* Version 1.1.0.0 modification, 2010.02.26 */
    TVideoOutDevInfo tVideoOutDevInfo;
/* ======================================== */
    DWORD adwUsrData[2];

#ifdef __USE_PROFILE__
/* Version 4.0.0.3 modification, 2012.06.26 */
/* ======================================== */
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_CLEAR_PROFILE);
#endif //__USE_PROFILE__

/* Version 4.0.0.3 modification, 2012.06.26 */
/* ======================================== */

    dwInFormat = (ptInfo->epixelInFormat==YUV422) ? VOC_INPUT_YUV422 : VOC_INPUT_YUV420;
/* Version 3.4.0.0 modification, 2011.07.13 */
/* Version 2.0.0.0 modification, 2010.03.10 */
    dwOutFormat = ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120) ||
                   (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
                ? VOC_OUTPUT_PROGRESSIVE_YUV_16BITS
                : ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24)
/* ======================================== */
                  ? VOC_OUTPUT_RGB24
                  : VOC_OUTPUT_INTERLACE_CCIR656);
/* ======================================== */

/* Version 5.0.0.0 modification, 2012.10.15 */
/* ======================================== */
/* Version 2.0.0.1 modification, 2010.03.10 */
/* Version 3.4.0.0 modification, 2011.07.13 */
    if ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24) ||
        (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120) ||
        (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
/* ======================================== */
/* ======================================== */
    {
        ptVOCInfo->dwOutSize = (ptInfo->dwOutWidth&0x0000FFFF) | ((ptInfo->dwOutHeight&0x0000FFFF)<<16);
        ptVOCInfo->dwHSyncCtrl =  ((ptInfo->dwHsyncBackPorch&0x000007FF) |
                                  (0x01<<11) |
                                  ((ptInfo->dwHsyncFrontPorch&0x00007FF)<<12));
        ptVOCInfo->dwVSyncCtrl =  ((ptInfo->dwVsyncBackPorch&0x000007FF) |
                                  (0x01<<11) |
                                  ((ptInfo->dwVsyncFrontPorch&0x00007FF)<<12));
        ptVOCInfo->dwOutSizeCtrl = (ptInfo->dwOutWidth&0x0000FFFF);
    }
    else // ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656
    {
        dwHeight = ((ptVOCInfo->dwInSize & 0xFFFF0000) >> 16);
        if (ptInfo->bPALEn)
        {
            ptVOCInfo->dwOutSize = (625<<16) | 864;
            ptVOCInfo->dwOutSizeCtrl = 312 | (2<<16) | (2<<20);
            if (dwHeight >= 576)
            {
                ptVOCInfo->dwInSize = (576<<16) | (ptVOCInfo->dwInSize&0x0000FFFF);
            }
        }
        else
        {
            ptVOCInfo->dwOutSize = (525<<16) | 858;
            ptVOCInfo->dwOutSizeCtrl = 262 | (2<<16) | (3<<20);
            if (dwHeight >= 480)
            {
                ptVOCInfo->dwInSize = (480<<16) | (ptVOCInfo->dwInSize&0x0000FFFF);
            }
        }
    }

    ptVOCInfo->dwSBCCtrl = (ptInfo->byBrightness<<8) | (ptVOCInfo->dwSBCCtrl & 0xFFFF00FF);
    ptVOCInfo->dwSBCCtrl = (ptInfo->byContrast) | (ptVOCInfo->dwSBCCtrl & 0xFFFFFF00);
    ptVOCInfo->dwSBCCtrl = ((ptInfo->dwSaturation&0x000001FF)<<16) | (ptVOCInfo->dwSBCCtrl & 0xFE00FFFF);
    ptVOCInfo->dwYCbCr2RGBCoeff0 = ((dwYCbCr2RGBBT601[2]&0x000003FF)<<20) |
                                   ((dwYCbCr2RGBBT601[1]&0x000003FF)<<10) |
                                    (dwYCbCr2RGBBT601[0]&0x000003FF);
    ptVOCInfo->dwYCbCr2RGBCoeff1 = ((dwYCbCr2RGBBT601[5]&0x000003FF)<<20) |
                                    ((dwYCbCr2RGBBT601[4]&0x000003FF)<<10) |
                                    (dwYCbCr2RGBBT601[3]&0x000003FF);
    ptVOCInfo->dwYCbCr2RGBCoeff2 = ((dwYCbCr2RGBBT601[9]&0x000000FF)<<24) |
                                    ((dwYCbCr2RGBBT601[8]&0x000000FF)<<16) |
                                    ((dwYCbCr2RGBBT601[7]&0x0000003F)<<10) |
                                    (dwYCbCr2RGBBT601[6]&0x000003FF);

    ptVOCInfo->dwInSize = ((ptInfo->dwInHeight&0x0000FFFF)<<16) | (ptInfo->dwInWidth&0x0000FFFF);

/* Version 3.0.0.0 modification, 2010.04.28 */
    if (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)
/* ======================================== */
    {
        dwStride = ((ptInfo->dwInStride<<1)&0xFFFF);
    }
    else
    {
        dwStride = (ptInfo->dwInStride&0xFFFF);
    }

    ptVOCInfo->dwCtrl = (((dwStride<<16)&0xFFFF0000) |
/* Version 3.4.0.0 modification, 2011.07.13 */
                         (ptVOCInfo->dwCtrl&0x0000C42D) |
/* ======================================== */
/* Version 3.0.0.0 modification, 2010.04.28 */
/* ======================================== */
/* Version 3.4.0.0 modification, 2011.07.13 */
                         ((((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120)||
                            (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))?0x5:0x0)<<11) |
/* ======================================== */
                         (dwInFormat<<7) |
/* Version 3.4.0.0 modification, 2011.07.13 */
                         (((dwOutFormat==VOC_OUTPUT_RGB24)?0x0:0x1)<<6) |
/* ======================================== */
/* Version 1.1.0.0 modification, 2010.02.26 */
                         (ptInfo->dwClkPol<<4) |
/* ======================================== */
                         (0x1<<1));

/* Version 1.1.0.0 modification, 2010.02.26 */
    tVideoOutDevInfo.dwVideoOutDev = VOC_DEVICE_NULL;
    tVideoOutDevInfo.dwDispWidth = ptInfo->dwInWidth;
    tVideoOutDevInfo.dwDispHeight = ptInfo->dwInHeight;
/* Version 3.4.0.1 modification, 2011.08.31 */
    tVideoOutDevInfo.dwVICID = ptInfo->dwVICID;
/* ======================================== */
    tVideoOutDevInfo.dwClkFreq = ptInfo->dwClkFreq;
/* Version 3.4.0.1 modification, 2011.08.31 */
    tVideoOutDevInfo.eOutFormat = ptInfo->eOutFormat;
/* ======================================== */
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_START, &tVideoOutDevInfo);
/* Version 3.0.0.0 modification, 2010.04.28 */
    if (ptInfo->bOutCtlModifiedFlag==TRUE)
    {
        adwUsrData[0] = ptInfo->dwRatio;
        adwUsrData[1] = (ptInfo->dwDivider > 0x1F) ? 0x1F : ptInfo->dwDivider;
        ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_PLL_CONFIG, &adwUsrData);
        ptInfo->bOutCtlModifiedFlag = FALSE;
    }
/* ======================================== */
/* ======================================== */

    DBPRINT0("Check all MMRs:\n");
    DBPRINT1("dwInSize : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwInSize);
    DBPRINT1("dwOutSize : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwOutSize);
    DBPRINT1("dwOutSizeCtrl : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwOutSizeCtrl);
    DBPRINT1("dwYCbCr2RGBCoeff0 : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwYCbCr2RGBCoeff0);
    DBPRINT1("dwYCbCr2RGBCoeff1 : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwYCbCr2RGBCoeff1);
    DBPRINT1("dwYCbCr2RGBCoeff2 : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwYCbCr2RGBCoeff2);
    DBPRINT1("dwSBCCtrl : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwSBCCtrl);
    DBPRINT1("dwHSyncCtrl : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwHSyncCtrl);
    DBPRINT1("dwVSyncCtrl : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwVSyncCtrl);
    DBPRINT1("dwYBuff0Addr : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwYBuff0Addr);
    DBPRINT1("dwYBuff1Addr : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwYBuff1Addr);
    DBPRINT1("dwCbBuff0Addr : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwCbBuff0Addr);
    DBPRINT1("dwCbBuff1Addr : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwCbBuff1Addr);
    DBPRINT1("dwCrBuff0Addr : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwCrBuff0Addr);
    DBPRINT1("dwCrBuff1Addr : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwCrBuff1Addr);
    DBPRINT1("dwCtrl : 0x%08x\n", (unsigned int)ptInfo->ptVOCInfo->dwCtrl);
}

/* ============================================================================================== */
/* Version 3.3.0.0 modification, 2011.07.05 */
void VideoDisp_WaitVOC(TVideoDispInfo *ptInfo)
{
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_WAIT_COMPLETE);
}
/* ======================================== */

/* =========================================================================================== */
void VideoDisp_StopVOC(TVideoDispInfo *ptInfo)
{
/* Version 1.1.0.0 modification, 2010.02.26 */
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_STOP);
/* ======================================== */
/* Version 3.0.0.0 modification, 2010.04.28 */
    ptInfo->bOutCtlModifiedFlag = FALSE;
/* ======================================== */
}

/* =========================================================================================== */
SCODE VideoDisp_SetOptionsVOC(TVideoDispInfo *ptInfo)
{
    volatile TVPLVOCInfo *ptVOCInfo = ptInfo->ptVOCInfo;
    DWORD dwInFormat, dwOutFormat, dwStride, dwHeight;
    DWORD adwUsrData[2];
/* Version 3.1.0.1 modification, 2011.01.14 */
    TVideoOutDevInfo tVideoOutDevInfo;
/* ======================================== */
/* Version 3.1.0.3 modification, 2011.03.07 */
    DWORD i, dwBufSize;
/* ======================================== */

    switch (ptInfo->eOptionFlags)
    {
        case VIDEO_DISP_OPTION_SET_INPUT:
            ptVOCInfo->dwInSize = ((ptInfo->dwInHeight&0x0000FFFF)<<16) |
                                  (ptInfo->dwInWidth&0x0000FFFF);
            dwStride = ((ptInfo->dwInStride<<(ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656))&0xFFFF);
            ptVOCInfo->dwCtrl = ((ptVOCInfo->dwCtrl&0x0000FFFF) | ((dwStride<<16)&0xFFFF0000));
            dwInFormat = (ptInfo->epixelInFormat==YUV422) ? VOC_INPUT_YUV422 : VOC_INPUT_YUV420;
            ptVOCInfo->dwCtrl = (ptVOCInfo->dwCtrl&0xFFFFFC7F) |
                                (dwInFormat<<7);
            ptVOCInfo->dwYBuff0Addr = (DWORD)ptInfo->atVideoBuffer[0].pbyYFrame;
            ptVOCInfo->dwCbBuff0Addr = (DWORD)ptInfo->atVideoBuffer[0].pbyCbFrame;
            ptVOCInfo->dwCrBuff0Addr = (DWORD)ptInfo->atVideoBuffer[0].pbyCrFrame;
            ptVOCInfo->dwYBuff1Addr = (DWORD)ptInfo->atVideoBuffer[1].pbyYFrame;
            ptVOCInfo->dwCbBuff1Addr = (DWORD)ptInfo->atVideoBuffer[1].pbyCbFrame;
            ptVOCInfo->dwCrBuff1Addr = (DWORD)ptInfo->atVideoBuffer[1].pbyCrFrame;
            break;
        case VIDEO_DISP_OPTION_SET_OUTPUT_CTRL:
/* Version 2.0.0.1 modification, 2010.03.10 */
/* Version 3.4.0.0 modification, 2011.07.13 */
            if ((ptInfo->eOutFormat == VIDEO_SIGNAL_FORMAT_RGB24) ||
                (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120) ||
                (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
/* ======================================== */
/* ======================================== */
            {
                ptVOCInfo->dwOutSize = (ptInfo->dwOutWidth&0x0000FFFF) | ((ptInfo->dwOutHeight&0x0000FFFF)<<16);
/* Version 3.1.0.0 modification, 2011.01.12 */
                ptVOCInfo->dwHSyncCtrl = ((ptInfo->dwHsyncBackPorch&0x000007FF) |
                                          (0x01<<11) |
                                          ((ptInfo->dwHsyncFrontPorch&0x00007FF)<<12));
                ptVOCInfo->dwVSyncCtrl = ((ptInfo->dwVsyncBackPorch&0x000007FF) |
                                          (0x01<<11) |
                                          ((ptInfo->dwVsyncFrontPorch&0x00007FF)<<12));
/* ======================================== */
                ptVOCInfo->dwOutSizeCtrl = (ptInfo->dwOutWidth&0x0000FFFF);

/* Version 3.0.0.0 modification, 2010.04.28 */
                adwUsrData[0] = ptInfo->dwRatio;
                adwUsrData[1] = (ptInfo->dwDivider > 0x1F) ? 0x1F : ptInfo->dwDivider;
                ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_PLL_CONFIG, &adwUsrData);
                ptInfo->bOutCtlModifiedFlag = FALSE;
/* ======================================== */
            }
            break;
        case VIDEO_DISP_OPTION_SET_PIXEL_FORMAT:
/* Version 3.4.0.0 modification, 2011.07.13 */
/* Version 2.0.0.0 modification, 2010.03.10 */
            dwOutFormat = ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120)||
                           (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
                        ? VOC_OUTPUT_PROGRESSIVE_YUV_16BITS
                        : ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24)
/* ======================================== */
                          ? VOC_OUTPUT_RGB24
                          : VOC_OUTPUT_INTERLACE_CCIR656);
/* Version 4.0.0.2 modification, 2011.11.29 */
            ptVOCInfo->dwCtrl = ((ptVOCInfo->dwCtrl&0xFFFFC7BF) |
/* ======================================== */
                                 ((((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120)||
                                    (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))?0x5:0x0)<<11) |
                                 (((dwOutFormat==VOC_OUTPUT_RGB24)?0x0:0x1)<<6));
/* ======================================== */
            break;
        case VIDEO_DISP_OPTION_SET_PAL:
            if (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)
            {
                dwHeight = ((ptVOCInfo->dwInSize & 0xFFFF0000) >> 16);
                if (ptInfo->bPALEn)
                {
                    ptVOCInfo->dwOutSize = (625<<16) | 864;
                    ptVOCInfo->dwOutSizeCtrl = 312 | (2<<16) | (2<<20);
                    if (dwHeight >= 576)
                    {
                        ptVOCInfo->dwInSize = (576<<16) |
                                              (ptVOCInfo->dwInSize&0x0000FFFF);
                    }
                }
                else
                {
                    ptVOCInfo->dwOutSize = (525<<16) | 858;
                    ptVOCInfo->dwOutSizeCtrl = 262 | (2<<16) | (3<<20);
                    if (dwHeight >= 480)
                    {
                        ptVOCInfo->dwInSize = (480<<16) |
                                              (ptVOCInfo->dwInSize&0x0000FFFF);
                    }
                }
/* Version 3.0.0.2 modification, 2010.09.17 */
                ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_SET_PAL, (int)ptInfo->bPALEn);
/* ======================================== */
            }
            break;
        case VIDEO_DISP_OPTION_SET_DISPLAY_BUFFER:
            ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_WRITE_BUF_INFO, ptInfo->atVideoBuffer);
            break;
        case VIDEO_DISP_OPTION_SET_BRIGHTNESS:
            ptVOCInfo->dwSBCCtrl = (ptInfo->byBrightness<<8) | (ptVOCInfo->dwSBCCtrl & 0xFFFF00FF);
            break;
        case VIDEO_DISP_OPTION_SET_CONTRAST:
            ptVOCInfo->dwSBCCtrl = (ptInfo->byContrast) | (ptVOCInfo->dwSBCCtrl & 0xFFFFFF00);
            break;
        case VIDEO_DISP_OPTION_SET_SATURATION:
            ptVOCInfo->dwSBCCtrl = ((ptInfo->dwSaturation&0x000001FF)<<16) | (ptVOCInfo->dwSBCCtrl & 0xFE00FFFF);
            break;
/* Version 3.1.0.0 modification, 2011.01.12 */
        case VIDEO_DISP_OPTION_SET_OUTPUT_SIZE:
            ptVOCInfo->dwInSize = ((ptInfo->dwInHeight&0x0000FFFF)<<16) |
                                  (ptInfo->dwInWidth&0x0000FFFF);
            ptVOCInfo->dwOutSize = (ptInfo->dwOutWidth&0x0000FFFF) | ((ptInfo->dwOutHeight&0x0000FFFF)<<16);
            ptVOCInfo->dwHSyncCtrl = ((ptInfo->dwHsyncBackPorch&0x000007FF) |
                                     (0x01<<11) |
                                     ((ptInfo->dwHsyncFrontPorch&0x00007FF)<<12));
            ptVOCInfo->dwVSyncCtrl = ((ptInfo->dwVsyncBackPorch&0x000007FF) |
                                     (0x01<<11) |
                                     ((ptInfo->dwVsyncFrontPorch&0x00007FF)<<12));

/* Version 3.4.0.0 modification, 2011.07.13 */
            if ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24) ||
                (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120) ||
                (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
/* ======================================== */
            {
                ptVOCInfo->dwOutSizeCtrl = (ptInfo->dwOutWidth&0x0000FFFF);
/* Version 3.1.0.2 modification, 2011.01.14 */
                dwStride = (ptInfo->dwInStride&0xFFFF);
            }
            else //(ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)
            {
                dwStride = ((ptInfo->dwInStride<<1)&0xFFFF);
            }
            ptVOCInfo->dwCtrl = (((dwStride<<16)&0xFFFF0000) | (ptVOCInfo->dwCtrl&0x0000FFFF));
/* ======================================== */
/* Version 3.1.0.3 modification, 2011.03.07 */
            dwBufSize = ptInfo->dwInHeight*ptInfo->dwInStride;
/* ======================================== */
/* Version 3.1.0.1 modification, 2011.01.14 */
            ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_STOP);
            tVideoOutDevInfo.dwVideoOutDev = VOC_DEVICE_NULL;
            tVideoOutDevInfo.dwDispWidth = ptInfo->dwInWidth;
            tVideoOutDevInfo.dwDispHeight = ptInfo->dwInHeight;
/* Version 3.4.0.0 modification, 2011.07.13 */
/* Version 3.4.0.1 modification, 2011.08.31 */
            tVideoOutDevInfo.eOutFormat = ptInfo->eOutFormat;
/* ======================================== */
/* ======================================== */
/* Version 3.4.0.1 modification, 2011.08.31 */
            tVideoOutDevInfo.dwVICID = ptInfo->dwVICID;
/* ======================================== */
            tVideoOutDevInfo.dwClkFreq = ptInfo->dwClkFreq;
/* Version 5.0.0.0 modification, 2012.10.15 */
            if (ptInfo->PreBufferMode)
/* ======================================== */
            {
/* Version 3.1.0.3 modification, 2011.03.07 */
                for (i=0; i<4 ; i++)
                {
/* Version 3.1.0.4 modification, 2011.03.08 */
                    memset((BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->atVideoBuffer[i].pbyYFrame), 0x10, dwBufSize);
                    memset((BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->atVideoBuffer[i].pbyCbFrame), 0x80, (dwBufSize>>(1+(ptInfo->epixelInFormat==YUV420))));
                    memset((BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->atVideoBuffer[i].pbyCrFrame), 0x80, (dwBufSize>>(1+(ptInfo->epixelInFormat==YUV420))));
/* ======================================== */
                }
/* ======================================== */
            }
            ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_START, &tVideoOutDevInfo);
/* ======================================== */
/* ======================================== */
            break;
        default:
            return S_FAIL;
    }
    return S_OK;
}
/* =========================================================================================== */
SCODE VideoDisp_GetBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState)
{

/* Version 4.0.0.3 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    DWORD dwFrameCnt;
    DWORD dwBusReqTimes;
    DWORD dwBusRGInterval;
    DWORD dwBusCycles;
#endif //__USE_PROFILE__
/* ======================================== */

/* Version 3.0.0.3 modification, 2010.12.31 */
    if (ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_BUF, &(ptInfo->dwProcessIndex))!= S_OK)
    {
        ERRPRINT("Get empty buffer for stuff fail !!\n");
        return _______VIDEODISP_ERR_DEVICE_BUSY;
    }
/* ======================================== */

/* Version 4.0.0.3 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_FRAME_CNT, (DWORD)&dwFrameCnt);
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_REQ_TIMES, (DWORD)&dwBusReqTimes);
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_RG_INTERVAL, (DWORD)&dwBusRGInterval);
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_BANDWIDTH, (DWORD)&dwBusCycles);

#ifndef __USE_SEQ_PROFILE__
    printf("Frame Count                 = %ld\n", dwFrameCnt);
    printf("Bus 2 Request time          = %ld\n", dwBusReqTimes);
    printf("Bus 2 Req-Grant interval    = %ld\n", dwBusRGInterval);
    printf("Bus 2 Bandwidth             = %ld\n", dwBusCycles);
#endif //__USE_SEQ_PROFILE__

    if (dwBusReqTimes >= ptInfo->dwPrevBusReqTimes)
    {
        ptInfo->dwTotalBusReqTimes += (dwBusReqTimes-ptInfo->dwPrevBusReqTimes);
    }
    else
    {
        ptInfo->dwTotalBusReqTimes += (65536-ptInfo->dwPrevBusReqTimes+dwBusReqTimes);
    }
    ptInfo->dwPrevBusReqTimes = dwBusReqTimes;

    if (dwBusRGInterval >= ptInfo->dwPrevBusLatency)
    {
        ptInfo->dwTotalBusLatency += (dwBusRGInterval-ptInfo->dwPrevBusLatency);
    }
    else
    {
        ptInfo->dwTotalBusLatency += (65536-ptInfo->dwPrevBusLatency+dwBusRGInterval);
    }
    ptInfo->dwPrevBusLatency = dwBusRGInterval;

    if (dwBusCycles >= ptInfo->dwPrevBusCycles)
    {
        ptInfo->flTotalBusCycles += ((FLOAT)(dwBusCycles-ptInfo->dwPrevBusCycles));
    }
    else
    {
        ptInfo->flTotalBusCycles += ((FLOAT)((4294967295-ptInfo->dwPrevBusCycles)+dwBusCycles+1));
    }
    ptInfo->dwPrevBusCycles = dwBusCycles;
    ptInfo->dwFrameCnt = dwFrameCnt;
#endif //__USE_PROFILE__
/* ======================================== */

    ptState->dwIndex = ptInfo->dwProcessIndex;
    ptState->dwFrameCount = ptInfo->atVideoBuffer[ptInfo->dwProcessIndex].dwFrameCount;

    ptState->pbyCbFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->atVideoBuffer[ptInfo->dwProcessIndex].pbyCbFrame);
    ptState->pbyCrFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->atVideoBuffer[ptInfo->dwProcessIndex].pbyCrFrame);
    ptState->pbyYFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)ptInfo->atVideoBuffer[ptInfo->dwProcessIndex].pbyYFrame);

    return S_OK;
}

/* =========================================================================================== */
SCODE VideoDisp_ReleaseBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState)
{
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_RELEASE_BUF);
    return S_OK;
}

/* =========================================================================================== */
SCODE VideoDisp_ResetVOC(TVideoDispInfo *ptInfo)
{
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_RESET);
    return S_OK;
}

/* =========================================================================================== */
SCODE VideoDisp_CheckVOC(TVideoDispInfo *ptInfo)
{
    DWORD dwVersionNum;

    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_VERSION_NUMBER, &dwVersionNum);

    if (((dwVersionNum&0x00FF00FF)!=(VPL_VOC_VERSION&0x00FF00FF)) ||
        ((dwVersionNum&0x0000FF00)<(VPL_VOC_VERSION&0x0000FF00)))
    {
        ERRPRINT("Invalid vpl_voc device driver version %d.%d.%d.%d !!\n",
                (int)dwVersionNum&0xFF,
                (int)(dwVersionNum>>8)&0xFF,
                (int)(dwVersionNum>>16)&0xFF,
                (int)(dwVersionNum>>24)&0xFF);
        ERRPRINT("Please use vpl_voc device driver version %d.%d.%d.%d or compatible versions !!\n",
                (int)VPL_VOC_VERSION&0xFF,
                (int)(VPL_VOC_VERSION>>8)&0xFF,
                (int)(VPL_VOC_VERSION>>16)&0xFF,
                (int)(VPL_VOC_VERSION>>24)&0xFF);
        return S_FAIL;
    }
    else
    {
        printf("Open VOC device driver version %d.%d.%d.%d successful !!\n",
                (int)dwVersionNum&0xFF,
                (int)(dwVersionNum>>8)&0xFF,
                (int)(dwVersionNum>>16)&0xFF,
                (int)(dwVersionNum>>24)&0xFF);
    }

    return S_OK;
}

/* =========================================================================================== */
/* Version 4.0.0.0 modification, 2011.09.14 */
SCODE VideoDisp_CableDetectVOC(TVideoDispInfo *ptInfo)
{
    int ret;
    ret = ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_CABLE_DETECT, (DWORD)&(ptInfo->bCableDetected));
    return (ret<0) ? S_FAIL : S_OK;
}

/* =========================================================================================== */
/* Version 5.0.0.0 modification, 2012.10.15 */
SCODE VideoDisp_QueueBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState)
{
/* Version 5.0.0.1 modification, 2013.01.14 */
    int ret;
/* ======================================== */
    TVideoBuffer tVideoBuf;
    tVideoBuf.dwIndex = ptState->dwIndex;
    tVideoBuf.pbyYFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptState->pbyYFrame);
    tVideoBuf.pbyCbFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptState->pbyCbFrame);
    tVideoBuf.pbyCrFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, (DWORD)ptState->pbyCrFrame);
/* Version 5.0.0.1 modification, 2013.01.14 */
    if ((ret = ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_QUEUE_BUF, &tVideoBuf)) <0)
    {
        ERRPRINT("Queue Buffer for processing fail !!\n");
        return _______VIDEODISP_ERR_DEVICE_BUSY;
    }
    return S_OK;
/* ======================================== */
}

/* =========================================================================================== */
SCODE VideoDisp_DeQueueBufVOC(TVideoDispInfo *ptInfo, TVideoDispState *ptState)
{
    int ret;
    TVideoBuffer tVideoBuf;
/* Version 5.0.0.1 modification, 2013.01.14 */
#ifdef __USE_PROFILE__
    DWORD dwFrameCnt;
    DWORD dwBusReqTimes;
    DWORD dwBusRGInterval;
    DWORD dwBusCycles;
#endif //__USE_PROFILE__
/* ======================================== */
    if ((ret = ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_DEQUEUE_BUF, &tVideoBuf)) < 0)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Dequeue processed buffer back fail !!\n");
        return _______VIDEODISP_ERR_DEVICE_BUSY;
/* ======================================== */
    }
/* Version 5.0.0.1 modification, 2013.01.14 */
#ifdef __USE_PROFILE__
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_FRAME_CNT, (DWORD)&dwFrameCnt);
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_REQ_TIMES, (DWORD)&dwBusReqTimes);
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_RG_INTERVAL, (DWORD)&dwBusRGInterval);
    ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_GET_BANDWIDTH, (DWORD)&dwBusCycles);

#ifndef __USE_SEQ_PROFILE__
    printf("Frame Count                 = %ld\n", dwFrameCnt);
    printf("Bus 2 Request time          = %ld\n", dwBusReqTimes);
    printf("Bus 2 Req-Grant interval    = %ld\n", dwBusRGInterval);
    printf("Bus 2 Bandwidth             = %ld\n", dwBusCycles);
#endif //__USE_SEQ_PROFILE__

    if (dwBusReqTimes >= ptInfo->dwPrevBusReqTimes)
    {
        ptInfo->dwTotalBusReqTimes += (dwBusReqTimes-ptInfo->dwPrevBusReqTimes);
    }
    else
    {
        ptInfo->dwTotalBusReqTimes += (65536-ptInfo->dwPrevBusReqTimes+dwBusReqTimes);
    }
    ptInfo->dwPrevBusReqTimes = dwBusReqTimes;

    if (dwBusRGInterval >= ptInfo->dwPrevBusLatency)
    {
        ptInfo->dwTotalBusLatency += (dwBusRGInterval-ptInfo->dwPrevBusLatency);
    }
    else
    {
        ptInfo->dwTotalBusLatency += (65536-ptInfo->dwPrevBusLatency+dwBusRGInterval);
    }
    ptInfo->dwPrevBusLatency = dwBusRGInterval;

    if (dwBusCycles >= ptInfo->dwPrevBusCycles)
    {
        ptInfo->flTotalBusCycles += ((FLOAT)(dwBusCycles-ptInfo->dwPrevBusCycles));
    }
    else
    {
        ptInfo->flTotalBusCycles += ((FLOAT)((4294967295-ptInfo->dwPrevBusCycles)+dwBusCycles+1));
    }
    ptInfo->dwPrevBusCycles = dwBusCycles;
    ptInfo->dwFrameCnt = dwFrameCnt;
#endif //__USE_PROFILE__
/* ======================================== */
    ptState->dwIndex = tVideoBuf.dwIndex;
    ptState->pbyYFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)tVideoBuf.pbyYFrame);
    ptState->pbyCbFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)tVideoBuf.pbyCbFrame);
    ptState->pbyCrFrame = (BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, (DWORD)tVideoBuf.pbyCrFrame);
    return S_OK;
}
/* ======================================== */
