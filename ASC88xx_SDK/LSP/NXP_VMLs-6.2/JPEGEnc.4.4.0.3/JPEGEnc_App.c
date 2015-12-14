/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/JPEGEnc/JPEGEnc_App/JPEGEnc_App.c 21    11/07/13 5:13p Bernard $
 *
 * Copyright 2008 ______ Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: JPEGEnc_App.c $
 * 
 * *****************  Version 21  *****************
 * User: Bernard      Date: 11/07/13   Time: 5:13p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * 
 * *****************  Version 20  *****************
 * User: Bernard      Date: 10/06/08   Time: 5:56p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * BUG: Fix errror that self-defined quantization table can't be adjusted
 * by quality_factor. - FIXED.
 * 
 * *****************  Version 19  *****************
 * User: Bernard      Date: 10/06/07   Time: 4:06p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * 
 * *****************  Version 17  *****************
 * User: Bernard      Date: 10/02/08   Time: 1:44p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * 
 * *****************  Version 16  *****************
 * User: Bernard      Date: 09/12/07   Time: 6:10p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 15  *****************
 * User: Bernard      Date: 09/09/24   Time: 10:34a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 13  *****************
 * User: Bernard      Date: 08/09/18   Time: 1:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * FEATURE: Remove JPEGENC_SINGLE_DRAM_EN in JEPGEnc.h to enable single
 * DRAM usage instead of dual DRAM usage. - DONE.
 *
 * *****************  Version 12  *****************
 * User: Bernard      Date: 08/09/18   Time: 11:28a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * MODIFICATION: move cache issue of data in bus 0 to JPEGEnc_App.c -
 * DONE.
 * FEATURE: Add macro JPEGENC_SINGLE_DRAM_EN in JEPGEnc.h to enable single
 * DRAM usage instead of dual DRAM usage. - DONE.
 *
 * *****************  Version 11  *****************
 * User: Bernard      Date: 08/08/22   Time: 2:09p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 10  *****************
 * User: Bernard      Date: 08/07/15   Time: 11:59a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * Synchronize with VMA_JEBE_Lib version 5.0.0.0 - DONE.
 *
 * *****************  Version 9  *****************
 * User: Bernard      Date: 08/07/02   Time: 5:59p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * FEATURE: Support format YUV444 of input frame. - DONE.
 *
 * *****************  Version 8  *****************
 * User: Bernard      Date: 08/03/18   Time: 12:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 * MODIFICATION: Update header description for SourceSafe - DONE.
 *
 * *****************  Version 7  *****************
 * User: Bernard      Date: 08/03/18   Time: 11:54a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 6  *****************
 * User: Bernard      Date: 08/01/10   Time: 7:54p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 5  *****************
 * User: Bernard      Date: 07/12/09   Time: 5:57p
 * Updated in $/rd_2/Training/SoC_Development/Bernard/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 4  *****************
 * User: Bernard      Date: 07/12/09   Time: 5:57p
 * Updated in $/rd_2/Training/SoC_Development/Bernard/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 3  *****************
 * User: Bernard      Date: 07/10/12   Time: 2:11p
 * Updated in $/rd_2/Training/SoC_Development/Bernard/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 2  *****************
 * User: Bernard      Date: 06/12/22   Time: 3:05p
 * Updated in $/rd_2/Training/SoC_Development/Bernard/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 * *****************  Version 1  *****************
 * User: Bernard      Date: 06/12/11   Time: 9:00p
 * Created in $/rd_2/Training/SoC_Development/Bernard/SoC/Linux_Libraries/JPEGEnc/JPEGEnc_App
 *
 */

/* ============================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ============================================================================================== */
#define PB_BUFFER_SIZE      2*1024*1024
#define PACKET_SIZE         256*1024
#define USER_DATA_LENGTH    1
#define CALIBRATE_TIME      2

/* ============================================================================================== */
//#define __SINGLE_THREAD_CIRCULAR__
#define __SINGLE_THREAD_NON_CIRCULAR__

//#define __OVER_NIGHT_TEST__
//#define __OPTIONS_TEST__
/* ============================================================================================== */
#ifdef __SINGLE_THREAD_CIRCULAR__

/* ============================================================================================== */
#include "JPEGEnc.h"
#include "MemMgr.h"

/* ============================================================================================== */
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* ============================================================================================== */
#ifdef __OVER_NIGHT_TEST__
static BYTE abyRefBuf[PB_BUFFER_SIZE];
static BOOL bFirst = TRUE;
static DWORD k;
#endif //__OVER_NIGHT_TEST__
/* ============================================================================================== */
typedef struct send_info
{
	TJPEGEncOptions *ptEncOptions;
    TJPEGEncState *ptEncState;

    HANDLE hEncObject;

	BYTE *pbyNetbuf0;
	BYTE *pbyNetbuf1;
	FILE *pfOutput;
	DWORD dwNetbufSel;		//If 0, select Netbuf0 to receive encoded data. Otherwise, select Nutbuf1 to receive encoded data.
	DWORD dwEncFrameSize;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	DWORD dwTotalTicks;
#endif //__USE_PROFILE__

}TSendInfo;

/* ============================================================================================== */
SCODE App_Send(HANDLE hAppSendObject)
{
	TSendInfo *ptSendInfo = hAppSendObject;
	TJPEGEncOptions *ptEncOptions = ptSendInfo->ptEncOptions;
	TJPEGEncState *ptEncState = ptSendInfo->ptEncState;

	HANDLE hEncObject;

	BYTE *pbyFullNetbuf;
	BYTE *pbyEmptyNetbuf;
	FILE *pfOutput;
	DWORD dwEncSize;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject = ptSendInfo->hTimerObject;
	DWORD dwTicks;

    Timer_GetInterval(hTimerObject, &dwTicks);
    ptSendInfo->dwTotalTicks = ptSendInfo->dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

	/* Set empty buffer for HW. */
	hEncObject = ptSendInfo->hEncObject;
	ptSendInfo->dwNetbufSel = (ptSendInfo->dwNetbufSel+1) & 0x1;
	pbyEmptyNetbuf = (ptSendInfo->dwNetbufSel == 0) ? ptSendInfo->pbyNetbuf0 : ptSendInfo->pbyNetbuf1;

    ptEncOptions->eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
    ptEncOptions->adwUserData[0] = (DWORD)pbyEmptyNetbuf;
    ptEncOptions->adwUserData[1] = PACKET_SIZE;

    if (JPEGEnc_SetOptions(hEncObject, ptEncOptions) != S_OK)
    {
        printf("Set options fail !!\n");
        exit(1);
    }


	/* Another process is used to move full buffer to network. */
	pbyFullNetbuf = (ptSendInfo->dwNetbufSel == 1) ? ptSendInfo->pbyNetbuf0 : ptSendInfo->pbyNetbuf1;
	pfOutput = ptSendInfo->pfOutput;
    dwEncSize = ptEncState->dwEncSize;
#ifdef __OVER_NIGHT_TEST__
	if (bFirst == TRUE)
	{
		DWORD m;
		for (m=0; m<dwEncSize; m++, k++)
		{
			abyRefBuf[k] = pbyFullNetbuf[m];
		}
	}
	else
	{
		DWORD m;
		for (m=0; m<dwEncSize; m++, k++)
		{
			if(abyRefBuf[k] != pbyFullNetbuf[m])
			{
				printf("Error\n");
			}
			printf("%02x", abyRefBuf[k]);
		}
	}
#else
	fwrite(pbyFullNetbuf, sizeof(BYTE), dwEncSize, pfOutput);
#endif //__OVER_NIGHT_TEST__
	ptSendInfo->dwEncFrameSize = ptSendInfo->dwEncFrameSize + dwEncSize;

#ifdef __USE_PROFILE__
    Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

	return S_OK;
}

/* ============================================================================================== */
int main (int argc, char **argv)
{
    TJPEGEncInitOptions tEncInitOptions;
    TJPEGEncOptions tEncOptions;
    TJPEGEncState tEncState;

    TMemMgrInitOptions tMemMgrInitOptions;
    TMemMgrState tMemMgrState;

	TSendInfo tSendInfo;

#ifdef __USE_PROFILE__
    HANDLE hTimerObject;
    TTimerInitOptions tTimerInitOptions;
    DWORD dwTicks;
#endif //__USE_PROFILE__

	HANDLE hAppSendObject;
    HANDLE hEncObject;
    HANDLE hMemObject;

    DWORD dwFrameCount, dwFrameSize;
    DWORD dwFrameNum;

    DWORD dwFrameSkip, dwSkipCount;
    DWORD dwInWidth, dwInHeight, dwEncWidth, dwEncHeight, dwQualityFactor, dwPixFormat, dwRstIntNum;
    DWORD dwInPadFrameSize; //F2BE outputs the padded block data.
	DWORD dwTotalSize;

    FILE *pfOutput;
    FILE *pfInput;
    BYTE *pbyFrame;
    BYTE *pbyNetbuf0;
    BYTE *pbyNetbuf1;
    BYTE *pbyUserData;

    if (argc != 12)
    {
        printf("Usage: jpegenc input_yuv_file\n");
        printf("               output_bitstream_file\n");
        printf("               encode_frame_num\n");
        printf("               input_frame_width\n");
        printf("               input_frame_height\n");
        printf("               encoded_frame_width\n");
        printf("               encoded_frame_height\n");
        printf("               frame_skip\n");
        printf("               quality_factor\n");
        printf("               pixel_format(1:YUV420, 2:YUV422, 11:YUV444)\n");
        printf("               restart_interval(positive number 0~65535, 0:disable)\n");
        exit(1);
    }

    if ((pfInput=fopen(argv[1], "rb")) == NULL)
    {
        printf("Open input YUV file %s fail !!\n", argv[1]);
        exit(1);
    }

    if ((pfOutput=fopen(argv[2], "wb")) == NULL)
    {
        printf("Open output bitstream file %s fail !!\n", argv[2]);
        exit(1);
    }

    dwFrameNum = atoi(argv[3]);
    dwInWidth = atoi(argv[4]);
    dwInHeight = atoi(argv[5]);
    dwEncWidth = atoi(argv[6]);
    dwEncHeight = atoi(argv[7]);
    dwFrameSkip = atoi(argv[8]);
    dwQualityFactor = atoi(argv[9]);
    dwPixFormat = atoi(argv[10]);
    dwRstIntNum = atoi(argv[11]);

    if (dwRstIntNum > (dwEncWidth*dwEncHeight>>6))
    {
        printf("Warning: The restart interval value works uselessly.\n");
        printf("         No restart information will be in the bitstream\n");
        printf("         Restart interval works when value <= (dwEncWidth*dwEncHeight/8/8)\n");
    }

    if (dwPixFormat == 11)
    {
        dwInPadFrameSize = ((dwInWidth+15)&0xfffffff0) * ((dwInHeight+7)&0xfffffff8); //Haydn Platform
        //dwInPadFrameSize = ((dwInWidth+7)&0xfffffff8) * ((dwInHeight+7)&0xfffffff8); //Mozart Platform
    }
    else if (dwPixFormat == 2)
    {
        dwInPadFrameSize = ((dwInWidth+15)&0xffffff0) * ((dwInHeight+7)&0xfffffff8);
    }
    else
    {
        dwInPadFrameSize = ((dwInWidth+15)&0xfffffff0) * ((dwInHeight+15)&0xffffff0);
    }

    if (dwPixFormat == 11)
    {
        dwFrameSize = dwInPadFrameSize*3;
    }
    else if (dwPixFormat == 2)
    {
        dwFrameSize = dwInPadFrameSize*2;
    }
    else
    {
        dwFrameSize = dwInPadFrameSize*3/2;
    }

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

    tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
    tMemMgrInitOptions.pObjectMem = (void *)NULL;

    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
    {
        printf("Initial memory manager object fail !!\n");
        exit(1);
    }

    tMemMgrState.dwBusNum = JPEGENC_IN_BUS_NUM;
    tMemMgrState.dwSize = dwFrameSize;
    //tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;//Mozart Platform
	tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;//Haydn Platform
    if ((pbyFrame=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate input frame buffer %ld fail !!\n", tMemMgrState.dwSize);
        exit(1);
    }
    else
    {
        printf("Allocate input frame buffer %ld succeed !!\n", tMemMgrState.dwSize);
    }

    tMemMgrState.dwBusNum = JPEGENC_OUT_BUS_NUM;
	tMemMgrState.dwSize = PACKET_SIZE;
    tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyNetbuf0=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate bit stream buffer 0 fail !! Size = %ld\n", tMemMgrState.dwSize);
        exit(1);
    }
	else
	{
		printf("Allocate bit stream buffer 0 succeed !!\n");
	}

    tMemMgrState.dwBusNum = JPEGENC_OUT_BUS_NUM;
	tMemMgrState.dwSize = PACKET_SIZE;
    tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyNetbuf1=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate bit stream buffer 1 fail !! Size = %ld\n", tMemMgrState.dwSize);
        exit(1);
    }
	else
	{
		printf("Allocate bit stream buffer 1 succeed !!\n");
	}


    if ((pbyUserData=(BYTE *)calloc(USER_DATA_LENGTH, sizeof(BYTE))) == NULL)
    {
        printf("Allocate user data buffer fail !!\n");
        exit(1);
    }

	tSendInfo.dwNetbufSel = 0;
	tSendInfo.pbyNetbuf0 = pbyNetbuf0;
	tSendInfo.pbyNetbuf1 = pbyNetbuf1;
	tSendInfo.pfOutput = pfOutput;
	tSendInfo.ptEncOptions = &tEncOptions;
	tSendInfo.ptEncState = &tEncState;
	hAppSendObject = &tSendInfo;

    tEncInitOptions.dwVersion = JPEGENC_VERSION;
    tEncInitOptions.dwEncWidth = dwEncWidth;
    tEncInitOptions.dwEncHeight = dwEncHeight;
    tEncInitOptions.dwInWidth = dwInWidth;
    tEncInitOptions.dwInHeight = dwInHeight;
    // 4:2:0 dwInHorzOffset and dwInVertOffset must be a multiple by 16
    // 4:2:2 dwInHorzOffset is a multiple by 16. dwInVertOffet is a multiple by 8.
    // MONO  dwInHorzOffset is a multiple by 8. dwInVertOffet is a multiple by 8.
    tEncInitOptions.dwInHorzOffset = 0;
    tEncInitOptions.dwInVertOffset = 0;
    tEncInitOptions.dwQualityFactor = dwQualityFactor;
    tEncInitOptions.ePixelFormatFlags = dwPixFormat;
    tEncInitOptions.dwRstIntNum = dwRstIntNum;
    tEncInitOptions.bJFIFHdr = FALSE;
    //bHighCompression:TRUE: enable adaptive entropy coding; FALSE: disable adaptive entropy coding
    tEncInitOptions.bHighCompression = TRUE;

    tEncInitOptions.dwBSBufSize = 0;
    tEncInitOptions.dwTimeOut = 1;
    tEncInitOptions.dwSemaphore = (DWORD)NULL;
    tEncInitOptions.dwUserData = 0;
    tEncInitOptions.eColorFlags = COLOR;
    tEncInitOptions.eQTableType = JPEGENC_Q_TABLE_TYPE_STANDARD;
    tEncInitOptions.eBufTypeFlags = JPEGCIRCULAR;
    tEncInitOptions.pfnSend = App_Send;
    tEncInitOptions.pfnEnter = NULL;
    tEncInitOptions.pfnLeave = NULL;
	tEncInitOptions.hAppSendObject = hAppSendObject;

    tEncInitOptions.pbyUserData = pbyUserData;
    tEncInitOptions.pbyYFrame = pbyFrame;
    tEncInitOptions.pbyCbFrame = pbyFrame + dwInPadFrameSize;

    if (tEncInitOptions.ePixelFormatFlags == YUV444)
    {
        tEncInitOptions.pbyCrFrame = (BYTE *)((DWORD)pbyFrame + dwInPadFrameSize + dwInPadFrameSize);
    }
    else if (tEncInitOptions.ePixelFormatFlags == YUV422)
    {
        tEncInitOptions.pbyCrFrame = (BYTE *)((DWORD)pbyFrame + dwInPadFrameSize + ((dwInPadFrameSize)>>1));
    }
    else
    {
        tEncInitOptions.pbyCrFrame = (BYTE *)((DWORD)pbyFrame + dwInPadFrameSize + ((dwInPadFrameSize)>>2));
    }

    tEncInitOptions.pObjectMem = NULL;

    if (JPEGEnc_Initial(&hEncObject, &tEncInitOptions) != S_OK)
    {
        printf("Initialize JPEG encoder object fail !!\n");
        exit(1);
    }

	tSendInfo.hEncObject = hEncObject;

    tEncState.dwUserDataLength = USER_DATA_LENGTH;

	dwTotalSize = 0;

    tEncOptions.eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
	tEncOptions.adwUserData[0] = (tSendInfo.dwNetbufSel == 0) ? (DWORD)pbyNetbuf0 : (DWORD) pbyNetbuf1;

    printf("Single-thread circular encoding process start...\n");

#ifdef __USE_PROFILE__
	tSendInfo.dwTotalTicks = 0;
	tSendInfo.hTimerObject = hTimerObject;
#endif //__USE_PROFILE__

#ifdef __OVER_NIGHT_TEST__
    if (fread(pbyFrame, sizeof(BYTE), dwFrameSize, pfInput) != dwFrameSize)
    {
        printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
        goto ENCODE_END;
    }
    for (dwFrameCount=0; ;dwFrameCount++)
    {
		DWORD dwRefSize = 0;
		bFirst = TRUE;
		k = 0;
#else
    for (dwFrameCount=0; dwFrameCount<dwFrameNum; dwFrameCount++)
    {
        for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
        {
            if (fread(pbyFrame, sizeof(BYTE), dwFrameSize, pfInput) != dwFrameSize)
            {
                printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
                goto ENCODE_END;
            }
        }
#endif //__OVER_NIGHT_TEST__

        tEncOptions.adwUserData[1] = PACKET_SIZE;
        if (JPEGEnc_SetOptions(hEncObject, &tEncOptions) != S_OK)
        {
            printf("Set options fail !!\n");
            exit(1);
        }

		tSendInfo.dwEncFrameSize = 0;

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

        if (JPEGENC_IN_BUS_NUM == 0)
        {
            MemMgr_CacheFlush(hMemObject, (DWORD)pbyFrame, dwFrameSize);
        }

        if (JPEGEnc_OneFrame(hEncObject, &tEncState) != S_OK)
        {
            printf("Encode %ld frame fail !!\n", dwFrameCount);
            break;
        }

		if (App_Send(hAppSendObject) != S_OK)
		{
			printf("Send data to App fail !!\n");
		}

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
        tSendInfo.dwTotalTicks = tSendInfo.dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

#ifdef __OVER_NIGHT_TEST__
		{
			if (bFirst == TRUE)
			{
				if (dwFrameCount == 1)
				{
					dwRefSize = tSendInfo.dwEncFrameSize;
					bFirst = FALSE;
                    printf("%ld Frame: Encoded bytes = %ld\n", dwFrameCount, tSendInfo.dwEncFrameSize);
				}
			}
			else
			{
				if (dwRefSize != tSendInfo.dwEncFrameSize)
				{
					printf("Error Size(%d) in %d Frames (RefSize=%d)\n", tSendInfo.dwEncFrameSize, dwFrameCount, dwRefSize);
				}
			}
		}
		printf("%d Frame: Encoded bytes = %d\n", dwFrameCount, tSendInfo.dwEncFrameSize);
#else
        printf("%d Frame: Encoded bytes = %d\n", dwFrameCount, tSendInfo.dwEncFrameSize);
#endif //__OVER_NIGHT_TEST__
		dwTotalSize = dwTotalSize + tSendInfo.dwEncFrameSize;
    }

ENCODE_END:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyFrame) != S_OK)
	{
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyNetbuf0) != S_OK)
	{
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyNetbuf1) != S_OK)
	{
		exit(1);
	}

#ifdef __USE_PROFILE__
    printf("Total cycles in APB unit = %ld\n", tSendInfo.dwTotalTicks);
    printf("Total cycles@30fps       = %3.2f\n", ((float)tSendInfo.dwTotalTicks*2/((float)(dwFrameCount)))*30);
    printf("Total cycles@25fps       = %3.2f\n", ((float)tSendInfo.dwTotalTicks*2/((float)(dwFrameCount)))*25);

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

    if (JPEGEnc_Release(&hEncObject) != S_OK)
    {
        printf("Release JPEG encoder object fail !!\n");
        exit(1);
    }

	printf("Total Encoded bytes = %ld\n", dwTotalSize);
	printf("Bit rate @ 25fps = %lf\n", (((float)dwTotalSize*8/(float)(dwFrameCount))*25));
	printf("Bit rate @ 30fps = %lf\n", (((float)dwTotalSize*8/(float)(dwFrameCount))*30));

    free(pbyUserData);
    fclose(pfOutput);
    fclose(pfInput);
    return 0;
}

/* ============================================================================================== */
#endif //__SINGLE_THREAD_CIRCULAR__

#ifdef __SINGLE_THREAD_NON_CIRCULAR__

/* ============================================================================================== */
#include "JPEGEnc.h"
#include "MemMgr.h"

/* ============================================================================================== */
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE_

/* ============================================================================================== */
#ifdef __OVER_NIGHT_TEST__
static BYTE abyRefBuf[PB_BUFFER_SIZE];
static BOOL bFirst = TRUE;
static DWORD k;
#endif //__OVER_NIGHT_TEST__
/* ============================================================================================== */
typedef struct send_info
{
    TJPEGEncState *ptEncState;

	BYTE *pbyNetbuf;
	FILE *pfOutput;
	DWORD dwEncFrameSize;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	DWORD dwTotalTicks;
#endif //__USE_PROFILE__

}TSendInfo;

/* =========================================================================================== */
SCODE App_Send(HANDLE hAppSendObject)
{
	TSendInfo *ptSendInfo = hAppSendObject;
	TJPEGEncState *ptEncState = ptSendInfo->ptEncState;

	BYTE *pbyNetbuf;
	FILE *pfOutput;
	DWORD dwEncSize;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject = ptSendInfo->hTimerObject;
	DWORD dwTicks;

    Timer_GetInterval(hTimerObject, &dwTicks);
    ptSendInfo->dwTotalTicks = ptSendInfo->dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

	pbyNetbuf = ptSendInfo->pbyNetbuf;
	pfOutput = ptSendInfo->pfOutput;
    dwEncSize = ptEncState->dwEncSize;

#ifdef __OVER_NIGHT_TEST__
#else
	fwrite(pbyNetbuf, sizeof(BYTE), dwEncSize, pfOutput);
#endif //__OVER_NIGHT_TEST__
	ptSendInfo->dwEncFrameSize = ptSendInfo->dwEncFrameSize + dwEncSize;

#ifdef __USE_PROFILE__
    Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

	return S_OK;
}

/* ============================================================================================== */
int main (int argc, char **argv)
{
    TJPEGEncInitOptions tEncInitOptions;
    TJPEGEncOptions tEncOptions;
    TJPEGEncState tEncState;

    TMemMgrInitOptions tMemMgrInitOptions;
    TMemMgrState tMemMgrState;

	TSendInfo tSendInfo;

#ifdef __USE_PROFILE__
    HANDLE hTimerObject;
    TTimerInitOptions tTimerInitOptions;
    DWORD dwTicks;
#endif //__USE_PROFILE__

	HANDLE hAppSendObject;
    HANDLE hEncObject;
    HANDLE hMemObject;

    DWORD dwFrameCount, dwFrameSize;
    DWORD dwFrameNum;
    DWORD dwFrameSkip, dwSkipCount;
    DWORD dwInWidth, dwInHeight, dwEncWidth, dwEncHeight, dwQualityFactor, dwPixFormat, dwRstIntNum;
    DWORD dwInPadFrameSize; //F2BE outputs the padded block data.
	DWORD dwTotalSize;
	DWORD k;

	BYTE abyQTable[2][64];
    BYTE *pbyNetbuf;

    FILE *pfOutput;
    FILE *pfInput;
    BYTE *pbyFrame;
    BYTE *pbyUserData;

    if (argc != 12)
    {
        printf("Usage: jpegenc input_yuv_file\n");
        printf("               output_bitstream_file\n");
        printf("               encode_frame_num\n");
        printf("               input_frame_width\n");
        printf("               input_frame_height\n");
        printf("               encoded_frame_width\n");
        printf("               encoded_frame_height\n");
        printf("               frame_skip\n");
        printf("               quality_factor\n");
        printf("               pixel_format(1:YUV420, 2:YUV422, 11:YUV444)\n");
        printf("               restart_interval(positive number 0~65535, 0:disable)\n");
        exit(1);
    }

    if ((pfInput=fopen(argv[1], "rb")) == NULL)
    {
        printf("Open input YUV file %s fail !!\n", argv[1]);
        exit(1);
    }

    if ((pfOutput=fopen(argv[2], "wb")) == NULL)
    {
        printf("Open output bitstream file %s fail !!\n", argv[2]);
        exit(1);
    }

    dwFrameNum = atoi(argv[3]);
    dwInWidth = atoi(argv[4]);
    dwInHeight = atoi(argv[5]);
    dwEncWidth = atoi(argv[6]);
    dwEncHeight = atoi(argv[7]);
    dwFrameSkip = atoi(argv[8]);
    dwQualityFactor = atoi(argv[9]);
    dwPixFormat = atoi(argv[10]);
    dwRstIntNum = atoi(argv[11]);

    if (dwRstIntNum > (dwEncWidth*dwEncHeight>>6))
    {
        printf("Warning: The restart interval value works uselessly.\n");
        printf("         No restart information will be in the bitstream\n");
        printf("         Restart interval works when value <= (dwEncWidth*dwEncHeight/8/8)\n");
    }

    if (dwPixFormat == 11)
    {
        //dwInPadFrameSize = ((dwInWidth+15)&0xfffffff0) * ((dwInHeight+7)&0xfffffff8); //Haydn Platform
        dwInPadFrameSize = ((dwInWidth+7)&0xfffffff8) * ((dwInHeight+7)&0xfffffff8); //Mozart Platform
    }
    else if (dwPixFormat == 2)
    {
        dwInPadFrameSize = ((dwInWidth+15)&0xffffff0) * ((dwInHeight+7)&0xfffffff8);
    }
    else
    {
        dwInPadFrameSize = ((dwInWidth+15)&0xfffffff0) * ((dwInHeight+15)&0xffffff0);
    }

    if (dwPixFormat == 11)
    {
        dwFrameSize = dwInPadFrameSize*3;
    }
    else if (dwPixFormat == 2)
    {
        dwFrameSize = dwInPadFrameSize*2;
    }
    else
    {
        dwFrameSize = dwInPadFrameSize*3/2;
    }

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

    tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
    tMemMgrInitOptions.pObjectMem = (void *)NULL;

    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
    {
        printf("Initial memory manager object fail !!\n");
        exit(1);
    }

    tMemMgrState.dwBusNum = JPEGENC_IN_BUS_NUM;
    tMemMgrState.dwSize = dwFrameSize;
    tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;//Mozart Platform
	//tMemMgrState.eAlignType = ALIGN_TYPE_64_BYTE;//Haydn Platform
    if ((pbyFrame=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate input frame buffer %ld fail !!\n", tMemMgrState.dwSize);
        exit(1);
    }
    else
    {
        printf("Allocate input frame buffer %ld succeed !!\n", tMemMgrState.dwSize);
    }

    tMemMgrState.dwBusNum = JPEGENC_OUT_BUS_NUM;
	tMemMgrState.dwSize = PB_BUFFER_SIZE;
    tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyNetbuf=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate bit stream buffer fail !! Size = %ld\n", tMemMgrState.dwSize);
        exit(1);
    }
	else
	{
		printf("Allocate bit stream buffer succeed !!\n");
	}

    if ((pbyUserData=(BYTE *)calloc(USER_DATA_LENGTH, sizeof(BYTE))) == NULL)
    {
        printf("Allocate user data buffer fail !!\n");
        exit(1);
    }

	for (k=0; k<64; k++)
	{
		abyQTable[0][k] = k + 1;
		abyQTable[1][k] = k + 100;
	}

	tSendInfo.pbyNetbuf = pbyNetbuf;
	tSendInfo.pfOutput = pfOutput;
	tSendInfo.ptEncState = &tEncState;
	hAppSendObject = &tSendInfo;

    tEncInitOptions.dwVersion = JPEGENC_VERSION;
    tEncInitOptions.dwEncWidth = dwEncWidth;
    tEncInitOptions.dwEncHeight = dwEncHeight;
    tEncInitOptions.dwInWidth = dwInWidth;
    tEncInitOptions.dwInHeight = dwInHeight;
    // 4:2:0 dwInHorzOffset and dwInVertOffset must be a multiple by 16
    // 4:2:2 dwInHorzOffset is a multiple by 16. dwInVertOffet is a multiple by 8.
    tEncInitOptions.dwInHorzOffset = 0;
    tEncInitOptions.dwInVertOffset = 0;
    tEncInitOptions.dwQualityFactor = dwQualityFactor;
    tEncInitOptions.ePixelFormatFlags = dwPixFormat;
    tEncInitOptions.dwRstIntNum = dwRstIntNum;
    tEncInitOptions.bJFIFHdr = FALSE;
    //bHighCompression:TRUE: enable adaptive entropy coding; FALSE: disable adaptive entropy coding
    tEncInitOptions.bHighCompression = TRUE;

    tEncInitOptions.dwBSBufSize = 0;
    tEncInitOptions.dwTimeOut = 1;
    tEncInitOptions.dwSemaphore = (DWORD)NULL;
    tEncInitOptions.dwUserData = 0;
    tEncInitOptions.eColorFlags = COLOR;
    tEncInitOptions.eQTableType = JPEGENC_Q_TABLE_TYPE_STANDARD;
    tEncInitOptions.eBufTypeFlags = JPEGNONCIRCULAR;
    tEncInitOptions.pfnSend = App_Send;
    tEncInitOptions.pfnEnter = NULL;
    tEncInitOptions.pfnLeave = NULL;
	tEncInitOptions.hAppSendObject = hAppSendObject;

    tEncInitOptions.pbyUserData = pbyUserData;
    tEncInitOptions.pbyYFrame = pbyFrame;
    tEncInitOptions.pbyCbFrame = pbyFrame + dwInPadFrameSize;

    if (tEncInitOptions.ePixelFormatFlags == YUV444)
    {
        tEncInitOptions.pbyCrFrame = (BYTE *)((DWORD)pbyFrame + dwInPadFrameSize + dwInPadFrameSize);
    }
    else if (tEncInitOptions.ePixelFormatFlags == YUV422)
    {
        tEncInitOptions.pbyCrFrame = (BYTE *)((DWORD)pbyFrame + dwInPadFrameSize + ((dwInPadFrameSize)>>1));
    }
    else
    {
        tEncInitOptions.pbyCrFrame = (BYTE *)((DWORD)pbyFrame + dwInPadFrameSize + ((dwInPadFrameSize)>>2));
    }

    tEncInitOptions.pObjectMem = NULL;

    if (JPEGEnc_Initial(&hEncObject, &tEncInitOptions) != S_OK)
    {
        printf("Initialize JPEG encoder object fail !!\n");
        exit(1);
    }

    tEncState.dwUserDataLength = USER_DATA_LENGTH;

	dwTotalSize = 0;
    printf("Single-thread non-circular encoding process start...\n");

#ifdef __USE_PROFILE__
    tSendInfo.dwTotalTicks = 0;
	tSendInfo.hTimerObject = hTimerObject;
#endif //__USE_PROFILE__

    tEncOptions.eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
    tEncOptions.adwUserData[0] = (DWORD)pbyNetbuf;

#ifdef __OVER_NIGHT_TEST__
    if (fread(pbyFrame, sizeof(BYTE), dwFrameSize, pfInput) != dwFrameSize)
    {
        printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
        goto ENCODE_END;
    }
    for (dwFrameCount=0; ;dwFrameCount++)
    {
		DWORD dwRefSize = 0;
		bFirst = TRUE;
#else
    for (dwFrameCount=0; dwFrameCount<dwFrameNum; dwFrameCount++)
    {
        for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
        {
            if (fread(pbyFrame, sizeof(BYTE), dwFrameSize, pfInput) != dwFrameSize)
            {
                printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
                goto ENCODE_END;
            }
        }
#endif //__OVER_NIGHT_TEST__

#ifdef __OPTIONS_TEST__
		if (dwFrameCount == 0)
		{	
			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT_TABLE;
			tEncOptions.adwUserData[0] = JPEGENC_Q_TABLE_TYPE_PELI;
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);
		}
		else if (dwFrameCount == 1)
		{
			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT_TABLE;
			tEncOptions.adwUserData[0] = JPEGENC_Q_TABLE_TYPE_CUSTOM_YCBCR;
			tEncOptions.adwUserData[1] = (DWORD)&abyQTable[0][0];
			tEncOptions.adwUserData[2] = (DWORD)&abyQTable[1][0];
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);

			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT; //Change dwQualityFactor to 1.
			tEncOptions.adwUserData[0] = 1;
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);
		}
		else if (dwFrameCount == 2)
		{
			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT_TABLE; //Inherit dwQualityFactor=1
			tEncOptions.adwUserData[0] = JPEGENC_Q_TABLE_TYPE_STANDARD;
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);
		}
		else if (dwFrameCount == 3)
		{
			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT; //Change dwQualityFactor to 50.
			tEncOptions.adwUserData[0] = 50;
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);

			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT_TABLE;
			tEncOptions.adwUserData[0] = JPEGENC_Q_TABLE_TYPE_CUSTOM_Y;
			tEncOptions.adwUserData[1] = (DWORD)&abyQTable[1][0];
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);
		}
		else if (dwFrameCount == 4)
		{
			tEncOptions.eOptionFlags = VIDEO_CHANGE_QUANT_TABLE; //Inherit dwQualityFactor=50
			tEncOptions.adwUserData[0] = JPEGENC_Q_TABLE_TYPE_CUSTOM_CBCR;
			tEncOptions.adwUserData[1] = (DWORD)&abyQTable[0][0];
			JPEGEnc_SetOptions(hEncObject, &tEncOptions);
		}
#endif //__OPTIONS_TEST__

		tEncOptions.eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
		tEncOptions.adwUserData[0] = (DWORD)pbyNetbuf;
        tEncOptions.adwUserData[1] = PB_BUFFER_SIZE;

        if (JPEGEnc_SetOptions(hEncObject, &tEncOptions) != S_OK)
        {
            printf("Set options fail !!\n");
        }

		tSendInfo.dwEncFrameSize = 0;

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

        if (JPEGENC_IN_BUS_NUM == 0)
        {
            MemMgr_CacheFlush(hMemObject, (DWORD)pbyFrame, dwFrameSize);
        }
/*
        if (JPEGEnc_OneFrame(hEncObject, &tEncState) != S_OK)
        {
            printf("Encode %ld frame fail !!\n", dwFrameCount);
            break;
        }
*/

        if (JPEGEnc_StartOneFrame(hEncObject, &tEncState) != S_OK)
        {
            printf("Start to Encode %ld frame fail !!\n", dwFrameCount);
            break;
        }
        if (JPEGEnc_WaitOneFrameComplete(hEncObject, &tEncState) != S_OK)
        {
            printf("Wait to Encode %ld frame fail !!\n", dwFrameCount);
            break;
        }

		if (App_Send(hAppSendObject) != S_OK)
		{
			printf("Send data to App fail !!\n");
		}

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
        tSendInfo.dwTotalTicks = tSendInfo.dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

#ifdef __OVER_NIGHT_TEST__
		if (bFirst == TRUE)
		{
			if (dwFrameCount == 1)
			{
				for (k=0; k<tSendInfo.dwEncFrameSize; k++)
				{
					abyRefBuf[k] = pbyNetbuf[k];
				}
				dwRefSize = tSendInfo.dwEncFrameSize;
				bFirst = FALSE;
        printf("%ld Frame: Encoded bytes = %ld\n", dwFrameCount, tSendInfo.dwEncFrameSize);
			}
		}
		else
		{
			for (k=0; k<tSendInfo.dwEncFrameSize; k++)
			{
				if(abyRefBuf[k] != pbyNetbuf[k])
				{
					printf("Error in %d Frames\n", dwFrameCount);
				}
				printf("%02x", pbyNetbuf[k]);
			}
			if (dwRefSize != tSendInfo.dwEncFrameSize)
			{
				printf("Error Size(%d) in %d Frames (RefSize=%d)\n", tSendInfo.dwEncFrameSize, dwFrameCount, dwRefSize);
			}
		}
		printf("%ld Frame: Encoded bytes = %ld\n", dwFrameCount, tSendInfo.dwEncFrameSize);
#else
        printf("%ld Frame: Encoded bytes = %ld\n", dwFrameCount, tSendInfo.dwEncFrameSize);
#endif //__OVER_NIGHT_TEST__
		dwTotalSize = dwTotalSize + tSendInfo.dwEncFrameSize;
    }

ENCODE_END:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyFrame) != S_OK)
	{
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyNetbuf) != S_OK)
	{
		exit(1);
	}

#ifdef __USE_PROFILE__
    printf("Total cycles in APB unit = %ld\n", tSendInfo.dwTotalTicks);
    printf("Total cycles@30fps       = %3.2f\n", ((float)tSendInfo.dwTotalTicks*2/((float)(dwFrameCount)))*30);
    printf("Total cycles@25fps       = %3.2f\n", ((float)tSendInfo.dwTotalTicks*2/((float)(dwFrameCount)))*25);

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

    if (JPEGEnc_Release(&hEncObject) != S_OK)
    {
        printf("Release JPEG encoder object fail !!\n");
        exit(1);
    }

	printf("Total Encoded bytes = %ld\n", dwTotalSize);
	printf("Bit rate @ 25fps = %lf\n", (((float)dwTotalSize*8/(float)(dwFrameCount))*25));
	printf("Bit rate @ 30fps = %lf\n", (((float)dwTotalSize*8/(float)(dwFrameCount))*30));

    free(pbyUserData);
    fclose(pfOutput);
    fclose(pfInput);
    return 0;
}

/* =========================================================================================== */
#endif //__SINGLE_THREAD_NON_CIRCULAR__

/* =========================================================================================== */
