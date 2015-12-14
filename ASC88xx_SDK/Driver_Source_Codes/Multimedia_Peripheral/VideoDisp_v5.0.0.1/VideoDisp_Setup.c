/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib/VideoDisp_Setup.c 20    13/01/15 6:52p Vincent $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: VideoDisp_Setup.c $
 * 
 * *****************  Version 20  *****************
 * User: Vincent      Date: 13/01/15   Time: 6:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Replace S_FAIL with error codes and refine the error
 * description for what-if analysis in the application - DONE.
 *
 * *****************  Version 19  *****************
 * User: Vincent      Date: 12/10/15   Time: 4:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 6.0.0.0
 * 2. FEATURE: Add VideoDisp_QueueBuf() and VideoDisp_DeQueueBuf() to
 * support dynamically feed display buffer address
 *
 * *****************  Version 18  *****************
 * User: Vincent      Date: 12/07/10   Time: 8:25p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 5.0.0.3.
 * 2. FEATURE: Add Mozart v3 to /proc/cpuinfo Revision check - DONE.
 *
 * *****************  Version 17  *****************
 * User: Alan         Date: 12/06/26   Time: 1:43p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Change version from 4.1.0.0 to 4.0.0.3 - DONE.
 *
 * *****************  Version 16  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:33p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Modify profile mechanism. - DONE.
 *
 * *****************  Version 15  *****************
 * User: Bernard      Date: 12/02/07   Time: 3:43p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 14  *****************
 * User: Evelyn       Date: 11/09/22   Time: 5:51p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 13  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 4.3.0.0.
 * 2. MODIFICATION: Support 16-bit YUV with embedded SYNC - DONE.
 *
 * *****************  Version 12  *****************
 * User: Alan         Date: 11/07/14   Time: 8:27p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add Mozart v1/v2 platform and model check mechanism - DONE.
 *
 * *****************  Version 11  *****************
 * User: Alan         Date: 11/07/14   Time: 3:02p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Support 16-bit YUV with separate or embedded SYNC - DONE.
 *
 * *****************  Version 10  *****************
 * User: Alan         Date: 11/07/01   Time: 11:32a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add 704x480 to NTSC output size - DONE.
 *
 * BUG: Minor version check is wrong - FIXED.
 *
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 10/06/07   Time: 3:33p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. BUG: Use epixelInFormat as eOutFormat in VideoDisp_Setup.c - FIXED.
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 10/04/29   Time: 12:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 10/04/26   Time: 11:35a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 10/03/10   Time: 3:50p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 2:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Add support VIDEO_SIGNAL_SIZE_720x480, VIDEO_SIGNAL_SIZE_1280x720 ,
 * VIDEO_SIGNAL_SIZE_1920x1080 - DONE.
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 09/08/11   Time: 10:08a
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
const CHAR VIDEODISP_ID[] = "$Version: "VIDEODISP_ID_VERSION"  (VIDEODISP) $";

/* =========================================================================================== */
SCODE VideoDisp_Initial(HANDLE *phObject, TVideoDispInitOptions *ptInitOptions)
{
    TVideoDispInfo *ptInfo;
    TMemMgrInitOptions tMemMgrInitOptions;
    DWORD dwObjectMemSize, dwOrgAddress;
    DWORD dwYSize, dwCbCrSize, dwFrameSize, dwInBufferAddr, i;
    const DWORD *pdwTimingTable;
/* Version 3.4.0.0 modification, 2011.07.14 */
    HANDLE hEDMCDev;
    FILE *fpCPUInfo;
    CHAR  acTmp[200];
/* ======================================== */

/* Version 5.0.0.1 modification, 2013.01.14 */
    SCODE scError;
/* ======================================== */
    if (((ptInitOptions->dwVersion&0x00FF00FF)!=(VIDEODISP_VERSION&0x00FF00FF)) |
/* Version 3.2.0.0 modification, 2011.07.01 */
        ((ptInitOptions->dwVersion&0x0000FF00)>(VIDEODISP_VERSION&0x0000FF00)))
/* ======================================== */
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Invalid video display library version %ld.%ld.%ld.%ld !!\n",
                ptInitOptions->dwVersion&0xFF,
                (ptInitOptions->dwVersion>>8)&0xFF,
                (ptInitOptions->dwVersion>>16)&0xFF,
                (ptInitOptions->dwVersion>>24)&0xFF);
        return _______VIDEODISP_ERR_INVALID_VERSION;
/* ======================================== */
    }

    if (ptInitOptions->pObjectMem == NULL)
    {
        dwObjectMemSize = VideoDisp_QueryMemSize(ptInitOptions);

        if ((ptInfo=(TVideoDispInfo *)calloc(sizeof(BYTE), dwObjectMemSize)) == NULL)
        {
/* Version 5.0.0.1 modification, 2013.01.14 */
            ERRPRINT("Allocate video display object memory fail !!\n");
            return _______VIDEODISP_ERR_OUT_OF_SYSTEM_MEMORY;
/* ======================================== */
        }
        dwOrgAddress = (DWORD)ptInfo;

        ptInfo = (TVideoDispInfo *)(((DWORD)ptInfo+ALIGN_PADDING) & ALIGN_MASK);
        ptInfo->dwOrgAddress = dwOrgAddress;
        ptInfo->bAllocateExternal = FALSE;
    }
    else
    {
        memset(ptInitOptions->pObjectMem, 0, VideoDisp_QueryMemSize(ptInitOptions));
        ptInfo = (TVideoDispInfo *)ptInitOptions->pObjectMem;
        ptInfo->bAllocateExternal = TRUE;
    }

    *phObject = (HANDLE)ptInfo;

/* Version 3.4.0.0 modification, 2011.07.14 */
    ptInfo->dwChipVersion = 0;
    if ((fpCPUInfo=fopen("/proc/cpuinfo", "r")) == NULL)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Open /proc/cpuinfo fail !!\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_PROCINFO_NOTEXIST;
/* ======================================== */
    }
    while (!feof(fpCPUInfo))
    {
        fscanf(fpCPUInfo, "%s", acTmp);
        if (strcmp(acTmp, "Revision") == 0)
        {
            fscanf(fpCPUInfo, "%s", acTmp); // ":"
            fscanf(fpCPUInfo, "%s", acTmp); // SYSC version
/* Version 4.0.0.4 modification, 2012.07.10 */
            if (strcmp(acTmp, "8040001") == 0)
            {
                DBPRINT0("[VideoDisp]: Mozart v3.\n");
                ptInfo->dwChipVersion = 3;
                break;
            }
/* ======================================== */
            else if (strcmp(acTmp, "8010c02") == 0)
            {
                DBPRINT0("[VideoDisp]: Mozart v2.\n");
                ptInfo->dwChipVersion = 2;
                break;
            }
            else if (strcmp(acTmp, "8010002") == 0)
            {
                DBPRINT0("[VideoDisp]: Mozart v1.\n");
                ptInfo->dwChipVersion = 1;
                break;
            }
            else
            {
/* Version 5.0.0.1 modification, 2013.01.14 */
                ERRPRINT("Unsupported SoC platform is detected !!\n");
                VideoDisp_Release(phObject);
                fclose(fpCPUInfo);
                return _______VIDEODISP_ERR_NOT_SUPPORT_SOC_PLATFORM;
/* ======================================== */
            }
        }
    }
    fclose(fpCPUInfo);
    if (ptInfo->dwChipVersion == 0)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Unsupported SoC platform is detected !!\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_NOT_SUPPORT_SOC_PLATFORM;
/* ======================================== */
    }

    if ((hEDMCDev=(HANDLE)open("/dev/vpl_edmc", O_RDWR)) < 0 )
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Fail to open vpl_edmc device driver !!\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_OPEN_DEVICE_DRIVER;
/* ======================================== */
    }
    ioctl((int)hEDMCDev, VPL_EDMC_IOC_QUERY_CHIP_SUBTYPE, &ptInfo->dwChipSubType);
    if (close((int)hEDMCDev)!=0)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Fail to close vpl_edmc device driver !!\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_CLOSE_DEVICE_DRIVER;
/* ======================================== */
    }
/* Version 4.0.0.1 modification, 2011.09.22 */
/* ======================================== */

    if (((ptInitOptions->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120)||
         (ptInitOptions->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS)) &&
        (ptInfo->dwChipVersion==1))
    {
        ERRPRINT("BT1120/16-bit YUV Separate Sync are not supported by this SoC platform.\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_INVALID_ARG;
    }
    if (((ptInitOptions->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24)||
         (ptInitOptions->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120)||
         (ptInitOptions->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS)) &&
        (ptInfo->dwChipSubType==5))
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("RGB24 Separate sync/BT1120/16-bit YUV Separate Sync are not supported by this model. Only 8-bit YUV is supported.\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
    }
/* ======================================== */

    tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
    tMemMgrInitOptions.pObjectMem = NULL;
    if ((scError=MemMgr_Initial(&ptInfo->hEDMCDev, &tMemMgrInitOptions)) != S_OK)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Initial memory management object fail !!\n");
        VideoDisp_Release(phObject);
        return scError;
/* ======================================== */
    }

    ptInfo->dwProcessIndex = 1;
    ptInfo->epixelInFormat = ptInitOptions->epixelInFormat;
    ptInfo->eOutFormat = ptInitOptions->eOutFormat;
    ptInfo->dwInWidth = ptInitOptions->dwInWidth;
    ptInfo->dwInHeight = ptInitOptions->dwInHeight;
/* Version 3.0.0.0 modification, 2010.04.28 */
    ptInfo->dwInStride = ptInitOptions->dwMaxInWidth;
/* ======================================== */
    ptInfo->bPALEn = ptInitOptions->bPALEn;
    ptInfo->byBrightness = (BYTE)((ptInitOptions->sdwBrightness&0x000000FF));
    ptInfo->byContrast = (BYTE)((ptInitOptions->sdwContrast&0x000000FF));
    ptInfo->dwSaturation = ptInitOptions->dwSaturation & 0x000001FF;

/* Version 3.0.0.0 modification, 2010.04.28 */
/* Version 3.0.0.1 modification, 2010.06.07 */
/* Version 3.4.0.0 modification, 2011.07.13 */
    if ((ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_RGB24) ||
        (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120) ||
        (ptInfo->eOutFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS))
/* ======================================== */
/* ======================================== */
    {
        if (ptInitOptions->eOutputRefreshRate==VIDEO_SIGNAL_FREQUENCY_60HZ)
        {
            switch (ptInitOptions->eOutputSize)
            {
                case VIDEO_SIGNAL_SIZE_480x272:
                    pdwTimingTable = adwQQHDTV60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_640x480:
                    pdwTimingTable = adwVGA60Hz;
                break;
/* Version 3.2.0.0 modification, 2011.06.27 */
                case VIDEO_SIGNAL_SIZE_704x480:
                    pdwTimingTable = adwNTSC60Hz;
                break;
/* ======================================== */
/* Version 1.1.0.0 modification, 2010.02.26 */
                case VIDEO_SIGNAL_SIZE_720x480:
                    pdwTimingTable = adwNTSC60Hz;
                break;
/* ======================================== */
                case VIDEO_SIGNAL_SIZE_768x576:
                    pdwTimingTable = adwPAL60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_800x480:
                    pdwTimingTable = adwWVGA60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_800x600:
                    pdwTimingTable = adwSVGA60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_1024x768:
                    pdwTimingTable = adwXGA60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_1280x768:
                    pdwTimingTable = adwWXGA60Hz;
                break;
/* Version 1.1.0.0 modification, 2010.02.26 */
                case VIDEO_SIGNAL_SIZE_1280x720:
                    pdwTimingTable = adw720p60Hz;
                break;
/* ======================================== */
                case VIDEO_SIGNAL_SIZE_1280x960:
                    pdwTimingTable = adwSXGA_960_60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_1280x1024:
                    pdwTimingTable = adwSXGA60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_1400x1050:
                    pdwTimingTable = adwSXGAPLUS60Hz;
                break;
                case VIDEO_SIGNAL_SIZE_1920x1080:
                    pdwTimingTable = adw1080p60Hz;
                break;
/* ======================================== */
                default:
/* Version 5.0.0.1 modification, 2013.01.14 */
                    ERRPRINT("Unsupport output size %d at refresh rate %d !!\n", ptInitOptions->eOutputSize, ptInitOptions->eOutputRefreshRate);
                    ERRPRINT("Please refer to the user guide for detailed actions.\n");
                    return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
            }
        }
        else if (ptInitOptions->eOutputRefreshRate==VIDEO_SIGNAL_FREQUENCY_24HZ)
        {
            switch (ptInitOptions->eOutputSize)
            {
                case VIDEO_SIGNAL_SIZE_1920x1080:
                    pdwTimingTable = adw1080p24Hz;
                break;
                default:
/* Version 5.0.0.1 modification, 2013.01.14 */
                    ERRPRINT("Unsupport output size %d at refresh rate %d !!\n", ptInitOptions->eOutputSize, ptInitOptions->eOutputRefreshRate);
                    ERRPRINT("Please refer to the user guide for detailed actions.\n");
/* ======================================== */
                    return _______VIDEODISP_ERR_INVALID_ARG;
            }
        }
/* Version 3.2.0.0 modification, 2011.06.27 */
        else if (ptInitOptions->eOutputRefreshRate==VIDEO_SIGNAL_FREQUENCY_30HZ)
        {
            switch (ptInitOptions->eOutputSize)
            {
                case VIDEO_SIGNAL_SIZE_1920x1080:
                    pdwTimingTable = adw1080p30Hz;
                break;
                default:
/* Version 5.0.0.1 modification, 2013.01.14 */
                    ERRPRINT("Unsupport output size %d at refresh rate %d !!\n", ptInitOptions->eOutputSize, ptInitOptions->eOutputRefreshRate);
                    ERRPRINT("Please refer to the user guide for detailed actions.\n");
                    return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
            }
        }
/* ======================================== */
        else
        {
/* Version 5.0.0.1 modification, 2013.01.14 */
            ERRPRINT("Unsupport output refresh rate %d !!\n", ptInitOptions->eOutputRefreshRate);
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
/* Version 1.1.0.0 modification, 2010.02.26 */
        ptInfo->dwClkFreq = pdwTimingTable[6];
/* Version 3.4.0.1 modification, 2011.08.31 */
        ptInfo->dwVICID = pdwTimingTable[7];
/* ======================================== */
        ptInfo->dwClkPol = 0x1;
/* ======================================== */
    }
    else
    {
        ptInfo->dwClkFreq = 27;
        ptInfo->dwClkPol = 0x1;
    }
/* ======================================== */

    dwYSize = ptInitOptions->dwMaxInWidth * ptInitOptions->dwMaxInHeight;
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
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Unsupported input format %d. Only YUV420 (1) and YUV422 (2) are supported !!\n", ptInfo->epixelInFormat);
        return _______VIDEODISP_ERR_INVALID_ARG;
/* ======================================== */
    }

/* Version 3.0.0.0 modification, 2010.04.28 */
    dwFrameSize = (dwYSize+(dwCbCrSize<<1)+ALIGN_PADDING) & ALIGN_MASK;
/* ======================================== */
    if (ptInitOptions->pbyInBuffer==NULL)
    {
/* Version 5.0.0.0 modification, 2012.10.15 */
        printf("[VideoDisp][Warning]: No pre allocated buffers, use VideoDisp_QueueBuf/VideoDisp_DeQueueBuf operation!!\n");
/* ======================================== */
    }
    else
    {
/* Version 3.0.0.0 modification, 2010.04.28 */
        dwInBufferAddr = (DWORD)((DWORD)(ptInitOptions->pbyInBuffer+ALIGN_PADDING) & ALIGN_MASK);
/* ======================================== */
        for (i=0; i<4; i++)
        {
/* Version 5.0.0.0 modification, 2012.10.15 */
            ptInfo->atVideoBuffer[i].dwIndex = i;
/* ======================================== */
            ptInfo->atVideoBuffer[i].pbyYFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, dwInBufferAddr + dwFrameSize*i);
            ptInfo->atVideoBuffer[i].pbyCbFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, dwInBufferAddr + dwYSize + dwFrameSize*i);
            ptInfo->atVideoBuffer[i].pbyCrFrame = (BYTE *)MemMgr_GetPhysAddr(ptInfo->hEDMCDev, dwInBufferAddr + dwYSize + dwCbCrSize + dwFrameSize*i);
        }
/* Version 5.0.0.0 modification, 2012.10.15 */
        ptInfo->PreBufferMode = TRUE;
/* ======================================== */
    }

/* Version 4.0.0.3 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    ptInfo->dwFrameCnt = 0;
    ptInfo->dwOpCycles = 0;
    ptInfo->dwTotalBusLatency = 0;
    ptInfo->dwPrevBusLatency = 0;
    ptInfo->dwTotalBusReqTimes = 0;
    ptInfo->dwPrevBusReqTimes = 0;
    ptInfo->flTotalBusCycles = 0;
    ptInfo->dwPrevBusCycles = 0;
#endif //__USE_PROFILE__
/* ======================================== */

    if (VideoDisp_OpenVOC(ptInfo, ptInitOptions) != S_OK)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        ERRPRINT("Fail to open vpl_voc device driver !!\n");
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_OPEN_DEVICE_DRIVER;
/* ======================================== */
    }

    if (VideoDisp_CheckVOC(ptInfo) != S_OK)
    {
/* Version 5.0.0.1 modification, 2013.01.14 */
        VideoDisp_Release(phObject);
        return _______VIDEODISP_ERR_INVALID_VERSION;
/* ======================================== */
    }
/* Version 5.0.0.0 modification, 2012.10.15 */
    if (ptInfo->PreBufferMode == TRUE)
    {
        for (i=0; i<4; i++)
            ioctl((int)ptInfo->hVOCDev, VPL_VOC_IOC_QUEUE_BUF, &ptInfo->atVideoBuffer[i]);
    }
/* ======================================== */
    return S_OK;
}

/* =========================================================================================== */
SCODE VideoDisp_Release(HANDLE *phObject)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(*phObject);
/* Version 5.0.0.1 modification, 2013.01.14 */
    SCODE scError;
/* ======================================== */
/* Version 4.0.0.3 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    printf("Total VOC Frame Count = %ld\n", ptInfo->dwFrameCnt);
    printf("Total VOC Bus 2 Request time  = %ld\n", ptInfo->dwTotalBusReqTimes);
    printf("Total VOC Bus 2 latency       = %ld\n", ptInfo->dwTotalBusLatency);
    printf("Total VOC Bus 2 BandWidth     = %lf\n", ptInfo->flTotalBusCycles);
    printf("Total VOC Bus 2 Bandwidth consumption @ 60fps = %lf\n", (ptInfo->flTotalBusCycles/(FLOAT)ptInfo->dwFrameCnt)*60/1000000);
#endif //__USE_PROFILE__
/* ======================================== */

    if (ptInfo != NULL)
    {
        if ((SDWORD)ptInfo->hVOCDev > 0)
        {
            if (VideoDisp_CloseVOC(ptInfo) != S_OK)
            {
/* Version 5.0.0.1 modification, 2013.01.14 */
                ERRPRINT("Fail to close vpl_voc device driver !!\n");
                return _______VIDEODISP_ERR_CLOSE_DEVICE_DRIVER;
/* ======================================== */
            }
        }

        if ((scError = MemMgr_Release(&ptInfo->hEDMCDev)) != S_OK)
        {
/* Version 5.0.0.1 modification, 2013.01.14 */
            ERRPRINT("Release memory management object fail !!\n");
            return scError;
/* ======================================== */
        }

        if (ptInfo->bAllocateExternal == FALSE)
        {
            ptInfo = (TVideoDispInfo *)ptInfo->dwOrgAddress;
            free(ptInfo);
        }
    }
    *phObject = (HANDLE)NULL;

    return S_OK;
}

/* =========================================================================================== */
