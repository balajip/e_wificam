/*
* $Header: /rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App/MassMemAccess_App.c 47    13/02/04 1:19p Yiming.liu $
*
* Copyright 2009 ______, Inc.  All rights reserved.
*
* Description:
*
* $History: MassMemAccess_App.c $
 * 
 * *****************  Version 47  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 1:19p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * 
 * *****************  Version 46  *****************
 * User: Yiming.liu   Date: 13/01/16   Time: 11:40a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * 
 * *****************  Version 45  *****************
 * User: Yiming.liu   Date: 12/10/08   Time: 5:31p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * FEATURE: Add bKeepCacheCoherence in TMassMemAccessInfo - DONE.
 * FEATURE: Add VIDEO_SIGNAL_OPTION_SET_CACHE_COHERENCE option in
 * MassMemAccess_SetOptions() - DONE.
 * MODIFICATION: Restore cache operations - DONE.
 * 
 * *****************  Version 44  *****************
 * User: Yiming.liu   Date: 12/09/26   Time: 3:44p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * 
 * *****************  Version 43  *****************
 * User: Yiming.liu   Date: 12/09/26   Time: 3:37p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * BUG: Fix privacy mask physical address bug in
 * MassMemAccess_SetOptions() - FIXED.
 * 
 * *****************  Version 42  *****************
 * User: Yiming.liu   Date: 12/09/25   Time: 11:21a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Remove cache operations - DONE.
 * MODIFICATION: Add profile information for MassMemAccess_SetOptions() in
 * MassMemAccess_App.c - DONE.
 * 
 * *****************  Version 41  *****************
 * User: Sean         Date: 12/02/15   Time: 1:32p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * 
 * *****************  Version 39  *****************
 * User: Sean         Date: 11/07/15   Time: 7:22p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * BUG: In MassMemAccess_Options.c, before using
 * MassMemAccess_ProcessOneFrame, tState.bUsePhysAddr should be set -
 * FIXED.
 * 
 * *****************  Version 38  *****************
 * User: Sean         Date: 11/06/11   Time: 12:02a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * 
 * *****************  Version 37  *****************
 * User: Sean         Date: 11/06/10   Time: 3:27p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * 
 * *****************  Version 36  *****************
 * User: Alan         Date: 10/12/02   Time: 5:35p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 *
 * *****************  Version 35  *****************
 * User: Alan         Date: 10/09/10   Time: 3:21p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/MassMemAccess/MassMemAccess_App
 *
 * *****************  Version 33  *****************
 * User: Lawrence.hu  Date: 09/10/13   Time: 4:04p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * UPDATE: Update the version number to 7.1.0.0 - DONE.
 *
 * *****************  Version 32  *****************
 * User: Lawrence.hu  Date: 09/10/13   Time: 10:44a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * FEATURE: Add MassMemAccess_CheckParam() in MassMemAccess_Options.c -
 * DONE.
 *
 * *****************  Version 31  *****************
 * User: Lawrence.hu  Date: 09/06/12   Time: 4:22p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * BUG: Enable Timer_ProfileStart(hTimerObject) - DONE.
 *
 * *****************  Version 30  *****************
 * User: Lawrence.hu  Date: 09/06/12   Time: 10:35a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * FEATURE: Add dwMaxBurstLength in TMassMemAccessState - DONE.
 *
 * *****************  Version 29  *****************
 * User: Lawrence.hu  Date: 09/06/10   Time: 10:38a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * UPDATE: Update the annotation for 1D/2D/CF/PM modes - DONE.
 *
 * *****************  Version 28  *****************
 * User: Lawrence.hu  Date: 09/04/02   Time: 4:41p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Remove redundant code - DONE.
 *
 * *****************  Version 27  *****************
 * User: Lawrence.hu  Date: 09/04/01   Time: 10:09p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * BUG: Rename bl2DEnable and blFillingEnable to b2DEnable and
 * bFillingEnable - DONE.
 *
 * *****************  Version 26  *****************
 * User: Lawrence.hu  Date: 09/03/31   Time: 1:49p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Add __USE_PROFILE__ for profiling - DONE.
 *
 * *****************  Version 25  *****************
 * User: Lawrence.hu  Date: 09/03/31   Time: 9:38a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Replace AHB bus number by DRAM number for the new bus
 * architecture of Mozart - Done.
 *
 * *****************  Version 22  *****************
 * User: Lawrence.hu  Date: 09/02/27   Time: 12:51p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 *
 * *****************  Version 21  *****************
 * User: Lawrence.hu  Date: 09/01/13   Time: 2:44p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Add __ASM_ARCH_PLATFORM_BACH_H__ and
 * __ASM_ARCH_PLATFORM_HAYDN_H__ for Bach and Haydn SoC
 *
 * *****************  Version 20  *****************
 * User: Lawrence.hu  Date: 09/01/12   Time: 10:14p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Remove dwBurstNum from TMassMemAccessState - DONE.
 *
 * *****************  Version 19  *****************
 * User: Lawrence.hu  Date: 09/01/12   Time: 2:03p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * FEATURE: Synchronize with VPL_DMAC device driver version 5.0.0.0 on
 * Haydn SoC.
 * FEATURE: Synchronize with VPL_DMAC device driver version 7.1.0.0 on
 * Mozart SoC.
 *
 * *****************  Version 18  *****************
 * User: Lawrence.hu  Date: 09/01/10   Time: 4:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Add the stride to the privacy mask buffer - DONE.
 *
 * *****************  Version 17  *****************
 * User: Lawrence.hu  Date: 08/11/07   Time: 8:36p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Remove redundant code - DONE.
 *
 * *****************  Version 16  *****************
 * User: Lawrence.hu  Date: 08/11/07   Time: 8:34p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Remove redundant code - DONE.
 *
 * *****************  Version 15  *****************
 * User: Lawrence.hu  Date: 08/10/27   Time: 12:22p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * FEATURE: Add burst_num to set burst number in DMAC - DONE.
 *
 * *****************  Version 14  *****************
 * User: Lawrence.hu  Date: 08/08/29   Time: 10:42a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 *
 * *****************  Version 13  *****************
 * User: Lawrence.hu  Date: 08/07/29   Time: 12:30p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
 * MODIFICATION: Add SrcStride, DstWidth, DstHeight and DstStride MMR to
 * support 2D DMA - DONE.
 * MODIFICATION: Add FillingConstant to support Constant_Filling mode -
 * DONE.
 * MODIFICATION: Add MaskAddr MMR to support Privacy_Mask mode - DONE.
*
* *****************  Version 11  *****************
* User: Sor          Date: 08/01/29   Time: 12:01a
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
*
* *****************  Version 10  *****************
* User: Sor          Date: 08/01/28   Time: 11:59p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
*
* *****************  Version 9  *****************
* User: Sor          Date: 08/01/13   Time: 3:58p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
*
* *****************  Version 8  *****************
* User: Sor          Date: 07/12/23   Time: 8:38p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* Modify dwDstBusNum to dwDstBusNum and pbyDstAddr to pbyDstAddr -
* DONE.
*
* *****************  Version 7  *****************
* User: Sor          Date: 06/07/11   Time: 6:45p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* FEATURE: Synchronize with MemMgr_Lib version 4.0.0.0 - DONE.
*
* *****************  Version 6  *****************
* User: Sor          Date: 06/04/24   Time: 4:37p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* MODIFICATION: Add transfer number in application code - DONE.
* MODIFICATION: Add standard profile procedure - DONE.
*
* *****************  Version 5  *****************
* User: Sor          Date: 06/04/24   Time: 3:42p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* FEATURE: Synchronize with MemMgr_Lib version 3.0.0.0 - DONE.
*
* *****************  Version 4  *****************
* User: Alan         Date: 06/04/13   Time: 7:43a
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* MODIFICATION: Add two arguments, dwSrcBusNum and dwDstBusNum.
*
* *****************  Version 3  *****************
* User: Sor          Date: 05/10/13   Time: 4:29p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* UPDATE: Update version number from 1.0.0.0 to 2.0.0.0 - DONE.
*
* *****************  Version 2  *****************
* User: Sor          Date: 05/10/07   Time: 4:45p
* Updated in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
*
* *****************  Version 1  *****************
* User: Sor          Date: 05/09/27   Time: 9:39a
* Created in $/rd_2/project/SoC/Linux_Libraries/MassMemAccess/MassMemAccess_App
* Mass Memory Access Linux Library Version 1.0.0.0.
*
*/

/* =========================================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "MassMemAccess.h"
#include "MemMgr.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* ============================================================================================== */
#define MAX_WIDTH           2560
#define MAX_HEIGHT          1920

#define MASSMEMACCESS_MASK_BUS_NUM          0
//#define __USE_PHYSADDR__

/* =========================================================================================== */
int main (int argc, char **argv)
{
    HANDLE hObject;
    HANDLE hMemObject;
    TMassMemAccessInitOptions tInitOptions;
    TMemMgrInitOptions tMemMgrInitOptions;
    TMassMemAccessState tState;
    TMemMgrState tMemMgrState;
    //TVideoSignalOptions tOptions;
	TMassMemAccessOptions tOptions;

/* Version 6.1.0.2 modification, 2009.03.30 */
    DWORD dwSrcDramNum, dwDstDramNum;
/* ======================================== */
    DWORD dwSrcBusNum, dwDstBusNum;
    DWORD dwFrameNum, dwFrameCount;
    DWORD dwMaskXStart, dwMaskXEnd, dwMaskYStart, dwMaskYEnd;

    FILE *pfInput;
    FILE *pfOutput;

    BYTE *pbyInput;
    BYTE *pbyOutput;
    BYTE *pbyMask;

#ifdef __USE_PHYSADDR__
	DWORD dwInput;
	DWORD dwOutput;
	DWORD dwMask;
#endif //__USE_PHYSADDR__

    DWORD dwBuffSize;
    DWORD dwInBuffSize;
    DWORD dwOutBuffSize;
    DWORD dwMaskBuffSize;

    DWORD dwFormat;
    DWORD dwSrcStride;
    DWORD dwSrcHeight;
    DWORD dwDstWidth;
    DWORD dwDstHeight;
    DWORD dwDstStride;
    DWORD dwFillingConstant;
    DWORD i, j;
    DWORD dwRemainder;
/* Version 6.0.0.0 modification, 2008.11.19 */
    DWORD dwMaskRemainder;
/* ======================================== */
/* Version 9.1.0.0 modification, 2012.10.08 */
	BOOL bKeepCacheCoherence;
/* ======================================== */

#ifdef __USE_PROFILE__
    HANDLE hTimerObject;
    TTimerInitOptions tTimerInitOptions;
    DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

    switch(argc)
    {
    case 10:
        // 1D mode
		printf("==========1D mode==========\n");
        if ((pfInput=fopen(argv[1], "rb")) == NULL)
        {
            printf("Open input image file fail !!\n");
            exit(1);
        }
        if ((pfOutput=fopen(argv[2], "wb")) == NULL)
        {
            printf("Open output image file fail !!\n");
            exit(1);
        }

        dwFrameNum = atoi(argv[4]);
        dwBuffSize = atoi(argv[5]);
/* Version 6.1.0.2 modification, 2009.03.30 */
        dwSrcDramNum = atoi(argv[6]);
        dwDstDramNum = atoi(argv[7]);
        dwSrcBusNum = atoi(argv[8]);
        dwDstBusNum = atoi(argv[9]);
/* ======================================== */

        /* Initial Timer object. Timer object is used to do profile and you have to make sure that the timer
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

        if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
        {
            printf("Initial memory manager object fail !!\n");
            exit(1);
        }

/* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwSrcDramNum;
/* ======================================== */
        tMemMgrState.dwSize = dwBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;
        if ((pbyInput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate source data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate source data buffer succeed !!\n");
        }

        /* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwDstDramNum;
        /* ======================================== */
        tMemMgrState.dwSize = dwBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;
        if ((pbyOutput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate Destination data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate Destination data buffer succeed !!\n");
        }

#ifdef __USE_PHYSADDR__
		dwInput  = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyInput);
		dwOutput = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyOutput);
#endif //__USE_PHYSADDR__

        /* Initial library object. */
        tInitOptions.dwVersion = MASSMEMACCESS_VERSION;
        tInitOptions.dwMaxFrameWidth = MAX_WIDTH;
        tInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
        tInitOptions.dwMaskBusNum = MASSMEMACCESS_MASK_BUS_NUM;
        tInitOptions.bMaskInit = FALSE;
        tInitOptions.pObjectMem = NULL;

        if (MassMemAccess_Initial(&hObject, &tInitOptions) != S_OK)
        {
            printf("Initial mass memory access object fail !!\n");
            exit(1);
        }

        tState.b2DEnable = FALSE;
        tState.bFillingEnable = FALSE;
		
#ifdef __USE_PHYSADDR__
		tState.bUsePhysAddr = TRUE;
		tState.pbySrcAddr = (BYTE *)dwInput;
		tState.pbyDstAddr = (BYTE *)dwOutput;
#else
		tState.bUsePhysAddr = FALSE;
        tState.pbySrcAddr = pbyInput;
        tState.pbyDstAddr = pbyOutput;
#endif //__USE_PHYSADDR__
        tState.dwSrcBusNum = dwSrcBusNum;
        tState.dwDstBusNum = dwDstBusNum;
        tState.dwTransSize = dwBuffSize;
        tState.bMaskEnable = FALSE;
/* Version 6.3.0.0 modification, 2009.06.12 */
        tState.dwMaxBurstLength = MAX_BURST_LENGTH;
/* ======================================== */
/* Version 9.1.0.0 modification, 2012.10.08 */
		bKeepCacheCoherence = TRUE;
		//bKeepCacheCoherence = FALSE;
		tOptions.adwUserData[0] = (DWORD)bKeepCacheCoherence;
        tOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_CACHE_COHERENCE;
        if (MassMemAccess_SetOptions(hObject, &tOptions) != S_OK)
        {
            printf("Fail to set up cache operation !!\n");
            goto PROCESS_1D_QUIT;
        }
        else
        {
            printf("Set up cache operation successfully !!\n");
        }
/* ======================================== */

#ifdef __USE_PROFILE__
        dwTotalTicks = 0;
#endif //__USE_PROFILE__

        for (dwFrameCount=0; dwFrameCount<dwFrameNum; dwFrameCount++)
        {
/* Version 7.1.0.0 modification, 2009.10.13 */
            if (MassMemAccess_CheckParam(hObject, &tState) != S_OK)
            {
                goto PROCESS_1D_QUIT;
            }
/* ======================================== */

            if (fread(pbyInput, sizeof(BYTE), dwBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_1D_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwBuffSize, pfOutput);
#endif //__USE_PROFILE__

            printf("%ld transfer complete !!\n", dwFrameCount);
        }

PROCESS_1D_QUIT:
#ifdef __USE_PROFILE__
        printf("Total cycles in AHB unit = %ld, %f MHz @ 30fps\n", dwTotalTicks*2, dwTotalTicks*2/(FLOAT)dwFrameNum/1000000*30);

        if (Timer_Release(&hTimerObject) != S_OK)
        {
            printf("Release timer object fail !!\n");
            exit(1);
        }
#endif //__USE_PROFILE__

        if (MemMgr_Release(&hMemObject) != S_OK)
        {
            printf("Release memory manager object fail !!\n");
            exit(1);
        }

        if (MassMemAccess_Release(&hObject) != S_OK)
        {
            printf("Release mass memory access object fail !!\n");
            exit(1);
        }

        fclose(pfInput);
        fclose(pfOutput);
        break;
        // Constant Filling mode
    case 9:
		printf("==========CF mode==========\n");
        if ((pfOutput=fopen(argv[1], "wb")) == NULL)
        {
            printf("Open output image file fail !!\n");
            exit(1);
        }

        dwDstWidth = atoi(argv[3]);
        dwDstHeight = atoi(argv[4]);
        dwDstStride = atoi(argv[5]);
        dwFillingConstant = atoi(argv[6]);
/* Version 6.1.0.2 modification, 2009.03.30 */
        dwDstDramNum = atoi(argv[7]);
        dwDstBusNum = atoi(argv[8]);
/* ======================================== */

        /* Initial Timer object. Timer object is used to do profile and you have to make sure that the timer
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

        if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
        {
            printf("Initial memory manager object fail !!\n");
            exit(1);
        }

        // INPUT/OUTPUT BUFFER FOR Y
        dwOutBuffSize = dwDstStride * dwDstHeight;
/* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwDstDramNum;
/* ======================================== */
        tMemMgrState.dwSize = dwOutBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;

        if ((pbyOutput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate destination data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate destination data buffer succeed !!\n");
        }

        for (dwFrameNum = 0; dwFrameNum < dwOutBuffSize; dwFrameNum ++)
        {
            pbyOutput[dwFrameNum] = 0;
        }

#ifdef __USE_PHYSADDR__		
		dwOutput = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyOutput);
#endif //__USE_PHYSADDR__

        /* Initial library object. */
        tInitOptions.dwVersion = MASSMEMACCESS_VERSION;
        tInitOptions.dwMaxFrameWidth = MAX_WIDTH;
        tInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
        tInitOptions.dwMaskBusNum = MASSMEMACCESS_MASK_BUS_NUM;
        tInitOptions.bMaskInit = FALSE;
        tInitOptions.pObjectMem = NULL;

        if (MassMemAccess_Initial(&hObject, &tInitOptions) != S_OK)
        {
            printf("Initial mass memory access object fail !!\n");
            exit(1);
        }

        // Transfer Y
        tState.b2DEnable = FALSE;
        tState.bFillingEnable = TRUE;
#ifdef __USE_PHYSADDR__
		tState.bUsePhysAddr = TRUE;
#else
		tState.bUsePhysAddr = FALSE;
#endif //__USE_PHYSADDR__

        tState.pbySrcAddr = NULL;
        
#ifdef __USE_PHYSADDR__
		tState.pbyDstAddr = (BYTE *)dwOutput;
#else
		tState.pbyDstAddr = pbyOutput;
#endif //__USE_PHYSADDR__
        tState.dwDstBusNum = dwDstBusNum;
        tState.dwTransSize = 0;
        tState.dwDstWidth = dwDstWidth;
        tState.dwDstHeight = dwDstHeight;
        tState.dwDstStride = dwDstStride;
        tState.dwFillingConstant = dwFillingConstant;
        tState.bMaskEnable = FALSE;
/* Version 6.3.0.0 modification, 2009.06.12 */
        tState.dwMaxBurstLength = MAX_BURST_LENGTH;
/* ======================================== */

        dwOutBuffSize = dwDstStride * dwDstHeight;

/* Version 9.1.0.0 modification, 2012.10.08 */
		bKeepCacheCoherence = TRUE;
		//bKeepCacheCoherence = FALSE;
		tOptions.adwUserData[0] = (DWORD)bKeepCacheCoherence;
        tOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_CACHE_COHERENCE;
        if (MassMemAccess_SetOptions(hObject, &tOptions) != S_OK)
        {
            printf("Fail to set up cache operation !!\n");
            goto PROCESS_CF_QUIT;
        }
        else
        {
            printf("Set up cache operation successfully !!\n");
        }
/* ======================================== */
/* Version 7.1.0.0 modification, 2009.10.13 */
        if (MassMemAccess_CheckParam(hObject, &tState) != S_OK)
        {
            goto PROCESS_CF_QUIT;
        }
/* ======================================== */

#ifdef __USE_PROFILE__
        dwTotalTicks = 0;
#endif //__USE_PROFILE__

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

        if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
        {
            printf("Process error !!\n");
        }

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
        dwTotalTicks = dwTotalTicks + dwTicks;
#else
        fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

PROCESS_CF_QUIT:
#ifdef __USE_PROFILE__
/* Version 9.1.0.1 modification, 2013.01.16 */
        printf("Total cycles in AHB unit = %ld, %f MHz @ 30fps\n", dwTotalTicks*2, dwTotalTicks*2/(FLOAT)dwFrameNum/1000000*30);
/* ======================================== */

        if (Timer_Release(&hTimerObject) != S_OK)
        {
            printf("Release timer object fail !!\n");
            exit(1);
        }
#endif //__USE_PROFILE__

        if (MemMgr_Release(&hMemObject) != S_OK)
        {
            printf("Release memory manager object fail !!\n");
            exit(1);
        }

        if (MassMemAccess_Release(&hObject) != S_OK)
        {
            printf("Release mass memory access object fail !!\n");
            exit(1);
        }
        fclose(pfOutput);
        break;
    case 15:
        // 2D mode
		printf("==========2D mode==========\n");
        if ((pfInput=fopen(argv[1], "rb")) == NULL)
        {
            printf("Open input image file fail !!\n");
            exit(1);
        }

        if ((pfOutput=fopen(argv[2], "wb")) == NULL)
        {
            printf("Open output image file fail !!\n");
            exit(1);
        }

        dwFrameNum = atoi(argv[4]);
        dwFormat = atoi(argv[5]);
        dwSrcStride = atoi(argv[6]);
        dwSrcHeight = atoi(argv[7]);
        dwDstWidth = atoi(argv[8]);
        dwDstHeight = atoi(argv[9]);
        dwDstStride = atoi(argv[10]);
/* Version 6.1.0.2 modification, 2009.03.30 */
        dwSrcDramNum = atoi(argv[11]);
        dwDstDramNum = atoi(argv[12]);
        dwSrcBusNum = atoi(argv[13]);
        dwDstBusNum = atoi(argv[14]);
/* ======================================== */

        /* Initial Timer object. Timer object is used to do profile and you have to make sure that the timer
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

        if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
        {
            printf("Initial memory manager object fail !!\n");
            exit(1);
        }

        // INPUT/OUTPUT BUFFER FOR Y
        dwInBuffSize = dwSrcStride * dwSrcHeight;
/* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwSrcDramNum;
/* ======================================== */
        tMemMgrState.dwSize = dwInBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;
        if ((pbyInput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate source data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate source data buffer succeed !!\n");
        }

        dwOutBuffSize = dwDstStride * dwDstHeight;
/* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwDstDramNum;
/* ======================================== */
        tMemMgrState.dwSize = dwOutBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;
        if ((pbyOutput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate destination data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate destination data buffer succeed !!\n");
        }

#ifdef __USE_PHYSADDR__
		dwInput  = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyInput);
		dwOutput = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyOutput);
#endif //__USE_PHYSADDR__

        /* Initial library object. */
        tInitOptions.dwVersion = MASSMEMACCESS_VERSION;
        tInitOptions.dwMaxFrameWidth = MAX_WIDTH;
        tInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
        tInitOptions.dwMaskBusNum = MASSMEMACCESS_MASK_BUS_NUM;
        tInitOptions.bMaskInit = FALSE;
        tInitOptions.pObjectMem = NULL;

        if (MassMemAccess_Initial(&hObject, &tInitOptions) != S_OK)
        {
            printf("Initial mass memory access object fail !!\n");
            exit(1);
        }

        tState.b2DEnable = TRUE;
        tState.bFillingEnable = FALSE;
				
        tState.dwSrcBusNum = dwSrcBusNum;
        tState.dwDstBusNum = dwDstBusNum;
#ifdef __USE_PHYSADDR__
		tState.bUsePhysAddr = TRUE;
		tState.pbySrcAddr = (BYTE *)dwInput;
		tState.pbyDstAddr = (BYTE *)dwOutput;
#else
		tState.bUsePhysAddr = FALSE;
		tState.pbySrcAddr = pbyInput;
		tState.pbyDstAddr = pbyOutput;
#endif //__USE_PHYSADDR__        
        tState.dwTransSize = 0;
        tState.bMaskEnable = FALSE;
/* Version 6.3.0.0 modification, 2009.06.12 */
        tState.dwMaxBurstLength = MAX_BURST_LENGTH;
/* ======================================== */
/* Version 9.1.0.0 modification, 2012.10.08 */
		bKeepCacheCoherence = TRUE;
		//bKeepCacheCoherence = FALSE;
		tOptions.adwUserData[0] = (DWORD)bKeepCacheCoherence;
        tOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_CACHE_COHERENCE;
        if (MassMemAccess_SetOptions(hObject, &tOptions) != S_OK)
        {
            printf("Fail to set up cache operation !!\n");
            goto PROCESS_2D_QUIT;
        }
        else
        {
            printf("Set up cache operation successfully !!\n");
        }
/* ======================================== */

#ifdef __USE_PROFILE__
        dwTotalTicks = 0;
#endif //__USE_PROFILE__

        for (dwFrameCount = 0; dwFrameCount < dwFrameNum; dwFrameCount ++)
        {
            // Transfer Y
            tState.dwSrcStride = dwSrcStride;
            tState.dwDstWidth = dwDstWidth;
            tState.dwDstHeight = dwDstHeight;
            tState.dwDstStride = dwDstStride;

            dwInBuffSize = dwSrcStride * dwSrcHeight;
            dwOutBuffSize = dwDstStride * dwDstHeight;

            if (fread(pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_2D_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

            // Transfer Cb
            if (dwFormat == 444)
            {
                tState.dwSrcStride = dwSrcStride;
                tState.dwDstWidth = dwDstWidth;
                tState.dwDstHeight = dwDstHeight;
                tState.dwDstStride = dwDstStride;
                dwInBuffSize = dwSrcStride * dwSrcHeight;
                dwOutBuffSize = dwDstStride * dwDstHeight;
            }
            else if (dwFormat == 422)
            {
                tState.dwSrcStride = dwSrcStride * 0.5;
                tState.dwDstWidth = dwDstWidth;
                tState.dwDstHeight = dwDstHeight * 0.5;
                tState.dwDstStride = dwDstStride;
                dwInBuffSize = dwSrcStride * dwSrcHeight * 0.5;
                dwOutBuffSize = dwDstStride * dwDstHeight * 0.5;
            }
            else
            {
                tState.dwSrcStride = dwSrcStride * 0.5;
                tState.dwDstWidth = dwDstWidth * 0.5;
                tState.dwDstHeight = dwDstHeight * 0.5;
                tState.dwDstStride = dwDstStride * 0.5;
                dwInBuffSize = dwSrcStride * dwSrcHeight * 0.25;
                dwOutBuffSize = dwDstStride * dwDstHeight * 0.25;
            }

/* Version 7.1.0.0 modification, 2009.10.13 */
            if (MassMemAccess_CheckParam(hObject, &tState) != S_OK)
            {
                goto PROCESS_2D_QUIT;
            }
/* ======================================== */

            if (fread(pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_2D_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

            // Transfer Cr
            if (fread(pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_2D_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

            printf("%ld transfer complete !!\n", dwFrameCount);
        }
PROCESS_2D_QUIT:
#ifdef __USE_PROFILE__
/* Version 9.1.0.1 modification, 2013.01.16 */
        printf("Total cycles in AHB unit = %ld, %f MHz @ 30fps\n", dwTotalTicks*2, dwTotalTicks*2/(FLOAT)dwFrameNum/1000000*30);
/* ======================================== */

        if (Timer_Release(&hTimerObject) != S_OK)
        {
            printf("Release timer object fail !!\n");
            exit(1);
        }
#endif //__USE_PROFILE__

        if (MemMgr_Release(&hMemObject) != S_OK)
        {
            printf("Release memory manager object fail !!\n");
            exit(1);
        }

        if (MassMemAccess_Release(&hObject) != S_OK)
        {
            printf("Release mass memory access object fail !!\n");
            exit(1);
        }
        fclose(pfInput);
        fclose(pfOutput);
        break;
    // Privacy Mask mode
    case 16:
		printf("==========PM mode==========\n");
        if ((pfInput=fopen(argv[1], "rb")) == NULL)
        {
            printf("Open input image file fail !!\n");
            exit(1);
        }

        if ((pfOutput=fopen(argv[2], "wb")) == NULL)
        {
            printf("Open output image file fail !!\n");
            exit(1);
        }

        dwFrameNum = atoi(argv[4]);
        dwFormat = atoi(argv[5]);
        dwSrcStride = atoi(argv[6]);
        dwSrcHeight = atoi(argv[7]);
        dwDstWidth = atoi(argv[8]);
        dwDstHeight = atoi(argv[9]);
        dwDstStride = atoi(argv[10]);
/* Version 6.1.0.2 modification, 2009.03.30 */
        dwSrcDramNum = atoi(argv[11]);
        dwDstDramNum = atoi(argv[12]);
        dwSrcBusNum = atoi(argv[13]);
        dwDstBusNum = atoi(argv[14]);
/* ======================================== */
        dwFillingConstant = atoi(argv[15]);

        /* Initial Timer object. Timer object is used to do profile and you have to make sure that the timer
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

        if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
        {
            printf("Initial memory manager object fail !!\n");
            exit(1);
        }

        /* INPUT BUFFER */
        dwInBuffSize = dwSrcStride * dwSrcHeight;
/* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwSrcDramNum;
/* ======================================== */
        tMemMgrState.dwSize = dwInBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;
        if ((pbyInput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate source data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate source data buffer succeed !!\n");
        }

        /* OUTPUT BUFFER */
        dwOutBuffSize = dwDstStride * dwDstHeight;
/* Version 6.1.0.2 modification, 2009.03.30 */
        tMemMgrState.dwBusNum = dwDstDramNum;
/* ======================================== */
        tMemMgrState.dwSize = dwOutBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;
        if ((pbyOutput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate destination data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate destination data buffer succeed !!\n");
        }

        /* MASK BUFFER */
        if((dwDstWidth%8) == 0)
        {
            dwRemainder = 0;
        }
        else
        {
            dwRemainder = 8 - (dwDstWidth%8);
        }
/* Version 6.0.0.0 modification, 2008.11.07 */
        if((((dwDstWidth+dwRemainder)>>3)%8) == 0)
        {
            dwMaskRemainder = 0;
        }
        else
        {
            dwMaskRemainder = 8 - (((dwDstWidth+dwRemainder)>>3)%8);
        }
/* ======================================== */
        //dwMaskBuffSize = (((dwDstWidth+dwRemainder)>>3)+dwMaskRemainder) * dwDstHeight;
        dwMaskBuffSize = (dwDstWidth>>3) * dwDstHeight;
        tMemMgrState.dwBusNum = MASSMEMACCESS_MASK_BUS_NUM;
        tMemMgrState.dwSize = dwMaskBuffSize;
        tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;

        if ((pbyMask=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
        {
            printf("Allocate mask data buffer fail !!\n");
            exit(1);
        }
        else
        {
            printf("Allocate mask data buffer succeed !!\n");
        }
        {
            for (i = 0; i < dwMaskBuffSize; i ++)
            {
                pbyMask[i] = 0;
            }
        }

#ifdef __USE_PHYSADDR__
		dwInput  = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyInput);
		dwOutput = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyOutput);
		dwMask = MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyMask);
#endif //__USE_PHYSADDR__

        /* Initial library object. */
        tInitOptions.dwVersion = MASSMEMACCESS_VERSION;
        tInitOptions.dwMaxFrameWidth = MAX_WIDTH;
        tInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
        tInitOptions.dwMaskBusNum = MASSMEMACCESS_MASK_BUS_NUM;
        tInitOptions.bMaskInit = TRUE;
        tInitOptions.pObjectMem = NULL;

        if (MassMemAccess_Initial(&hObject, &tInitOptions) != S_OK)
        {
            printf("Initial mass memory access object fail !!\n");
            exit(1);
        }

        tState.b2DEnable = FALSE;
        tState.bFillingEnable = FALSE;
		
        tState.dwSrcBusNum = dwSrcBusNum;
        tState.dwDstBusNum = dwDstBusNum;
#ifdef __USE_PHYSADDR__
		tState.bUsePhysAddr = TRUE;
		tState.pbySrcAddr = (BYTE *)dwInput;
		tState.pbyDstAddr = (BYTE *)dwOutput;
#else
		tState.bUsePhysAddr = FALSE;
		tState.pbySrcAddr = pbyInput;
		tState.pbyDstAddr = pbyOutput;
#endif //__USE_PHYSADDR__        
        tState.dwTransSize = 0;
        tState.bMaskEnable = TRUE;
/* Version 6.3.0.0 modification, 2009.06.12 */
        tState.dwMaxBurstLength = MAX_BURST_LENGTH;
/* ======================================== */
/* Version 9.1.0.0 modification, 2012.10.08 */
		bKeepCacheCoherence = TRUE;
		//bKeepCacheCoherence = FALSE;
		tOptions.adwUserData[0] = (DWORD)bKeepCacheCoherence;
        tOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_CACHE_COHERENCE;
        if (MassMemAccess_SetOptions(hObject, &tOptions) != S_OK)
        {
            printf("Fail to set up cache operation !!\n");
            goto PROCESS_PM_QUIT;
        }
        else
        {
            printf("Set up cache operation successfully !!\n");
        }
/* ======================================== */

#ifdef __USE_PROFILE__
        dwTotalTicks = 0;
#endif //__USE_PROFILE__

        /* Mask Range Setting */
		//dwMaskYStart = (dwDstHeight/8) & 0xFFFFFFFE;
		dwMaskYStart = 0;
        dwMaskYEnd = (dwDstHeight/4) & 0xFFFFFFFE;
        dwMaskXStart = ((dwDstWidth>>3)*3/4) & 0xFFFFFFFE;
		//dwMaskXStart = 0;
        dwMaskXEnd = ((dwDstWidth>>3)*7/8) & 0xFFFFFFFE;

        for (dwFrameCount = 0; dwFrameCount < dwFrameNum; dwFrameCount ++)
        {
            memset(pbyMask, 0, dwMaskBuffSize);
            /* Transfer Y */
            for (i = dwMaskYStart; i < dwMaskYEnd; i ++)
            {
                for (j = dwMaskXStart; j < dwMaskXEnd; j ++)
                {
                    pbyMask[i*(dwDstWidth>>3)+j] = 255;
                }
            }
            tOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_PRIVACY_MASK;
/* Version 9.0.0.5 modification, 2012.09.26 */
#ifdef __USE_PHYSADDR__
            tOptions.adwUserData[0] = dwMask;
#else
            tOptions.adwUserData[0] = (DWORD)pbyMask;
#endif //__USE_PROFILE__
/* ======================================== */
            tOptions.adwUserData[1] = dwDstWidth;
            tOptions.adwUserData[2] = dwDstHeight;
			tOptions.adwUserData[3] = (DWORD)tState.bUsePhysAddr;

/* Version 9.0.0.5 modification, 2012.09.25 */
#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
/* ======================================== */
            if (MassMemAccess_SetOptions(hObject, &tOptions) != S_OK)
            {
                printf("Fail to set up Y privacy mask !!\n");
                goto PROCESS_PM_QUIT;
            }
            else
            {
                printf("Set up Y privacy mask successfully !!\n");
            }
/* Version 9.0.0.5 modification, 2012.09.25 */
#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__
/* ======================================== */

            tState.dwSrcStride = dwSrcStride;
            tState.dwDstWidth = dwDstWidth;
            tState.dwDstHeight = dwDstHeight;
            tState.dwDstStride = dwDstStride;
            dwInBuffSize = dwSrcStride * dwSrcHeight;
            dwOutBuffSize = dwDstStride * dwDstHeight;
            tState.dwFillingConstant = dwFillingConstant;

/* Version 7.1.0.0 modification, 2009.10.13 */
            if (MassMemAccess_CheckParam(hObject, &tState) != S_OK)
            {
                goto PROCESS_PM_QUIT;
            }
/* ======================================== */

            if (fread(pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_PM_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

            memset(pbyMask, 0, dwMaskBuffSize);
            /* Transfer Cb & Cr */
            for (i = (dwMaskYStart>>(dwFormat==420)); i < (dwMaskYEnd>>(dwFormat==420)); i ++)
            {
                for (j = (dwMaskXStart>>1); j < (dwMaskXEnd>>1); j ++)
                {
                    pbyMask[i*(dwDstWidth>>4)+j] = 255;
                }
            }
            tOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_PRIVACY_MASK;
/* Version 9.0.0.5 modification, 2012.09.26 */
#ifdef __USE_PHYSADDR__
            tOptions.adwUserData[0] = dwMask;
#else
            tOptions.adwUserData[0] = (DWORD)pbyMask;
#endif //__USE_PROFILE__
/* ======================================== */
            tOptions.adwUserData[1] = dwDstWidth >> 1;
            tOptions.adwUserData[2] = dwDstHeight >> (dwFormat==420);
			tOptions.adwUserData[3] = (DWORD)tState.bUsePhysAddr;

/* Version 9.0.0.5 modification, 2012.09.26 */
#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
/* ======================================== */
            if (MassMemAccess_SetOptions(hObject, &tOptions) != S_OK)
            {
                printf("Fail to set up Cb/Cr privacy mask !!\n");
                goto PROCESS_PM_QUIT;
            }
            else
            {
                printf("Set up Cb/Cr privacy mask successfully !!\n");
            }
/* Version 9.0.0.5 modification, 2012.09.26 */
#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__
/* ======================================== */
            if (dwFormat == 444)
            {
                tState.dwSrcStride = dwSrcStride;
                tState.dwDstWidth = dwDstWidth;
                tState.dwDstHeight = dwDstHeight;
                tState.dwDstStride = dwDstStride;
                dwInBuffSize = dwSrcStride * dwSrcHeight;
                dwOutBuffSize = dwDstStride * dwDstHeight;
            }
            else if (dwFormat == 422)
            {
                tState.dwSrcStride = dwSrcStride * 0.5;
                tState.dwDstWidth = dwDstWidth;
                tState.dwDstHeight = dwDstHeight * 0.5;
                tState.dwDstStride = dwDstStride;
                dwInBuffSize = dwSrcStride * dwSrcHeight * 0.5;
                dwOutBuffSize = dwDstStride * dwDstHeight * 0.5;
            }
            else
            {
                tState.dwSrcStride = dwSrcStride * 0.5;
                tState.dwDstWidth = dwDstWidth * 0.5;
                tState.dwDstHeight = dwDstHeight * 0.5;
                tState.dwDstStride = dwDstStride * 0.5;
                dwInBuffSize = dwSrcStride * dwSrcHeight * 0.25;
                dwOutBuffSize = dwDstStride * dwDstHeight * 0.25;
            }

            /* Transfer Cb */
            if (fread(pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_PM_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

            /* Transfer Cr */
            if (fread(pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) == 0)
            {
                printf("Input file is too small !!\n");
                goto PROCESS_PM_QUIT;
            }

#ifdef __USE_PROFILE__
            Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

            if (MassMemAccess_ProcessOneFrame(hObject, &tState) != S_OK)
            {
                printf("Process error !!\n");
            }

#ifdef __USE_PROFILE__
            Timer_GetInterval(hTimerObject, &dwTicks);
            dwTotalTicks = dwTotalTicks + dwTicks;
#else
            fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
#endif //__USE_PROFILE__

            printf("%ld transfer complete !!\n", dwFrameCount);
        }
PROCESS_PM_QUIT:
#ifdef __USE_PROFILE__
/* Version 9.1.0.1 modification, 2013.01.16 */
        printf("Total cycles in AHB unit = %ld, %f MHz @ 30fps\n", dwTotalTicks*2, dwTotalTicks*2/(FLOAT)dwFrameNum/1000000*30);
/* ======================================== */

        if (Timer_Release(&hTimerObject) != S_OK)
        {
            printf("Release timer object fail !!\n");
            exit(1);
        }
#endif //__USE_PROFILE__

        if (MemMgr_Release(&hMemObject) != S_OK)
        {
            printf("Release memory manager object fail !!\n");
            exit(1);
        }

        if (MassMemAccess_Release(&hObject) != S_OK)
        {
            printf("Release mass memory access object fail !!\n");
            exit(1);
        }
        fclose(pfInput);
        fclose(pfOutput);
        break;
    default:
		printf("=== MOZART PLATFORM ===\n");
		printf("1D mode: massmemaccess input_filename\n");
		printf("                       output_filename\n");
		printf("                       mode (0: 1D DMA, 1: 2D DMA, 2: Constant Filling 3: Privacy mask)\n");
		printf("                       transfer_times\n");
		printf("                       size_per_transfer\n");
		printf("                       source_dram_num(0/1)\n");
		printf("                       destination_dram_num(0/1)\n");
		printf("                       source_bus_num(0/1/2)\n");
		printf("                       destination_bus_num(0/1/2)\n\n");
		printf("2D mode: massmemaccess input_filename\n");
		printf("                       output_filename\n");
		printf("                       mode (0: 1D DMA, 1: 2D DMA, 2: Constant Filling 3: Privacy mask)\n");
		printf("                       frame_number\n");
		printf("                       format\n");
		printf("                       source_stride\n");
		printf("                       source_height\n");
		printf("                       destination_width\n");
		printf("                       destination_height\n");
		printf("                       destination_stride\n");
		printf("                       source_dram_num(0/1)\n");
		printf("                       destination_dram_num(0/1)\n");
		printf("                       source_ahb_bus_num(0/1/2)\n");
		printf("                       destination_ahb_bus_num(0/1/2)\n\n");
		printf("Constant Filling mode: massmemaccess output_filename\n");
		printf("                                     mode (0: 1D DMA, 1: 2D DMA, 2: Constant Filling 3: Privacy mask)\n");;
		printf("                                     destination_width\n");
		printf("                                     destination_height\n");
		printf("                                     destination_stride\n");
		printf("                                     filling_constant(0~255)\n");
		printf("                                     destination_dram_num(0/1)\n");
		printf("                                     destination_ahb_bus_num(0/1/2)\n");
		printf("Privacy Masking mode: massmemaccess input_filename\n");
		printf("                                    output_filename\n");
		printf("                                    mode (0: 1D DMA, 1: 2D DMA, 2: Constant Filling 3: Privacy mask)\n");;
		printf("                                    frame_number\n");
		printf("                                    format\n");
		printf("                                    source_stride\n");
		printf("                                    source_height\n");
		printf("                                    destination_width\n");
		printf("                                    destination_height\n");
		printf("                                    destination_stride\n");
		printf("                                    source_dram_num(0/1)\n");
		printf("                                    destination_dram_num(0/1)\n");
		printf("                                    source_ahb_bus_num(0/1/2)\n");
		printf("                                    destination_ahb_bus_num(0/1/2)\n");
		printf("                                    filling_constant(0~255)\n");
		exit(1);
		break;
    }
    return 0;
}

/* =========================================================================================== */
