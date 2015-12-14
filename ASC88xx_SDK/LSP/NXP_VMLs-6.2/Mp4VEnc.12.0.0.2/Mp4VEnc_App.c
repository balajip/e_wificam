/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/Mp4VEnc/Mp4VEnc_App/Mp4VEnc_App.c 40    13/04/16 11:24a Jaja $
 *
 * Copyright 2008 ______, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: Mp4VEnc_App.c $
 *
 * *****************  Version 40  *****************
 * User: Jaja         Date: 13/04/16   Time: 11:24a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 39  *****************
 * User: Jaja         Date: 12/04/11   Time: 7:14p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 4  *****************
 * User: Jaja         Date: 11/09/26   Time: 11:23a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 3  *****************
 * User: Jaja         Date: 11/07/11   Time: 5:03p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 2  *****************
 * User: Jaja         Date: 10/06/01   Time: 6:05p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 1  *****************
 * User: Jaja         Date: 10/04/06   Time: 8:16p
 * Created in $/rd_2/project/Mozart/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 37  *****************
 * User: Jaja         Date: 10/02/02   Time: 10:40a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 36  *****************
 * User: Jaja         Date: 09/11/25   Time: 3:22p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 35  *****************
 * User: Jaja         Date: 09/10/12   Time: 5:49p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 34  *****************
 * User: Jaja         Date: 09/01/08   Time: 5:49p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 33  *****************
 * User: Sor          Date: 08/01/27   Time: 12:39a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 32  *****************
 * User: Sor          Date: 08/01/26   Time: 7:57p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 31  *****************
 * User: Sor          Date: 08/01/26   Time: 7:56p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 30  *****************
 * User: Jaja         Date: 08/01/23   Time: 2:56p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 29  *****************
 * User: Jaja         Date: 08/01/21   Time: 10:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 28  *****************
 * User: Jaja         Date: 08/01/19   Time: 12:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 27  *****************
 * User: Jaja         Date: 08/01/19   Time: 12:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 26  *****************
 * User: Jaja         Date: 08/01/03   Time: 11:04a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 25  *****************
 * User: Jaja         Date: 08/01/03   Time: 11:03a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 24  *****************
 * User: Jaja         Date: 08/01/02   Time: 1:23p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 23  *****************
 * User: Jaja         Date: 07/12/25   Time: 8:06p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 22  *****************
 * User: Jaja         Date: 07/03/08   Time: 9:06p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 21  *****************
 * User: Sor          Date: 06/08/08   Time: 6:24p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 20  *****************
 * User: Jaja         Date: 06/08/05   Time: 3:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 19  *****************
 * User: Jaja         Date: 06/08/05   Time: 3:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 18  *****************
 * User: Sor          Date: 06/08/01   Time: 2:08p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 17  *****************
 * User: Sor          Date: 06/07/28   Time: 9:46p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 16  *****************
 * User: Jaja         Date: 06/07/13   Time: 1:25p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 15  *****************
 * User: Jaja         Date: 06/06/08   Time: 11:09a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 14  *****************
 * User: Jaja         Date: 06/05/29   Time: 10:35a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 13  *****************
 * User: Sor          Date: 06/04/28   Time: 3:40p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 12  *****************
 * User: Sor          Date: 06/04/27   Time: 4:38p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 11  *****************
 * User: Sor          Date: 06/04/27   Time: 3:44p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 10  *****************
 * User: Sor          Date: 06/04/27   Time: 2:47p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 * FEATURE: Remove multiple stream support - DONE.
 *
 * *****************  Version 9  *****************
 * User: Jaja         Date: 06/04/26   Time: 7:26p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 8  *****************
 * User: Jaja         Date: 06/04/26   Time: 4:18p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 7  *****************
 * User: Jaja         Date: 06/04/21   Time: 11:02p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 6  *****************
 * User: Jaja         Date: 06/04/21   Time: 2:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 * MODIFICATION: Modify MemMgr_Lib usage.
 * *****************  Version 5  *****************
 * User: Jaja         Date: 06/04/17   Time: 10:51p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 4  *****************
 * User: Jaja         Date: 06/03/06   Time: 12:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 3  *****************
 * User: Jaja         Date: 06/02/09   Time: 11:25a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 * *****************  Version 2  *****************
 * User: Jaja         Date: 06/01/26   Time: 2:55p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VEnc/Mp4VEnc_App
 *
 */

/* =========================================================================================== */
#include <stdio.h>
#include <stdlib.h>

#include "Mp4VEnc.h"
#include "MemMgr.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* =========================================================================================== */
#define PB_BUFFER_SIZE		1024*1024*4
#define PACKET_SIZE			1280
#define MAX_WIDTH			1920
#define MAX_HEIGHT			1088
#define USER_DATA_LENGTH	1
#define CALIBRATE_TIME		2

/* =========================================================================================== */
// Mozart 1
#define H264ENC_STREAM_BUS_NUM	1
#define H264ENC_FRMAE_BUS_NUM	1

/* =========================================================================================== */
int main (int argc, char **argv)
{
	TMp4VEncInitOptions tEncInitOptions;
    TMemMgrInitOptions tMemMgrInitOptions;
	TMp4VEncState tEncState;
	TMp4VEncOptions tEncOptions;
	TMemMgrState tMemMgrState;

	HANDLE hMemObject;
	HANDLE hEncObject;

	FILE *pfOutput;
	FILE *pfInput;

	BYTE *pbyNetbuf;

	BYTE *pbyFrame, *pbySrc;
	BYTE *pbyEncUserData;

	DWORD dwFrameCount, dwFrameSize,dwFrameBytes;
	DWORD dwFrameNum, dwPacketCount;
	DWORD dwFrameSkip, dwSkipCount;
	DWORD dwBitRate, dwPacketSize;
	DWORD dwInWidth, dwInHeight, dwEncWidth, dwEncHeight, dwQP, dwMode;
    DWORD dwTotalSize;
	DWORD dwPadWidth, dwPadHeight;
	DWORD j;
	DWORD dwMediaMemSize;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

	if (argc != 12)
	{
		printf("Usage: mp4venc input_yuv_filename\n");
		printf("               output_bitstream_filename\n");
		printf("               encode_frame_number\n");
		printf("               input_frame_width\n");
		printf("               input_frame_height\n");
		printf("               encoded_frame_width\n");
		printf("               encoded_frame_height\n");
		printf("               frame_skip\n");
		printf("               bit_rate\n");
		printf("               init_qp\n");
		printf("               mode(0: NM, 1: SH, 2: VP, 6: DP, 14: RVLC)\n");
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
	dwBitRate = atoi(argv[9]);
	dwQP = atoi(argv[10]);
	dwMode = atoi(argv[11]);
	dwPadWidth = (((dwInWidth + 15)>>4)<<4);
	dwPadHeight = (((dwInHeight + 15)>>4)<<4);
	dwFrameSize = dwPadWidth*dwPadHeight;

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

	if ((pbyEncUserData=(BYTE *)calloc(USER_DATA_LENGTH, sizeof(BYTE))) == NULL)
	{
		printf("Allocate user data buffer fail !!\n");
		exit(1);
	}

/* Version 3.0.0.0 modification, 2006.04.20 */
	tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
	tMemMgrInitOptions.pObjectMem = (void *)NULL;

    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
	{
		printf("Initial memory manager object fail !!\n");
		exit(1);
	}

	tMemMgrState.dwBusNum = H264ENC_FRMAE_BUS_NUM;
	tMemMgrState.dwSize = (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)*3/2;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyFrame=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate input frame buffer %ld fail !!\n", tMemMgrState.dwSize);
		goto END_LEBEL_2;
	}
	else
	{
		printf("Allocate input frame buffer %ld succeed !!\n", tMemMgrState.dwSize);
	}
	memset(pbyFrame, 0,  tMemMgrState.dwSize);
/* ======================================== */

	tMemMgrState.dwBusNum = H264ENC_STREAM_BUS_NUM;
	tMemMgrState.dwSize = PB_BUFFER_SIZE + 2;
	tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;

	if ((pbyNetbuf=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate bit stream buffer %ld fail !!\n", tMemMgrState.dwSize);
		goto END_LEBEL_1;
	}
	else
	{
		printf("Allocate bit stream buffer %ld succeed !!\n", tMemMgrState.dwSize);
	}

	tEncInitOptions.dwVersion = MP4VENC_VERSION;
	tEncInitOptions.dwMaxFrameWidth = MAX_WIDTH;
	tEncInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
	tEncInitOptions.dwEncWidth = dwEncWidth;
	tEncInitOptions.dwEncHeight = dwEncHeight;
	tEncInitOptions.dwInWidth = dwInWidth;
	tEncInitOptions.dwInHeight = dwInHeight;
	tEncInitOptions.dwInHorzOffset = 0;
	tEncInitOptions.dwInVertOffset = 0;
	tEncInitOptions.dwStreamBusNum =  H264ENC_STREAM_BUS_NUM;
	tEncInitOptions.dwFrameBusNum = H264ENC_FRMAE_BUS_NUM;
	tEncInitOptions.dwTimeResolution = 30;
	tEncInitOptions.dwTicksPerTR = 1;
	tEncInitOptions.dwIntraVOPInterval = 30;
	tEncInitOptions.dwIntraMBRefreshNum = 2;
	tEncInitOptions.dwQuant = dwQP;
	tEncInitOptions.dwPacketSize = PACKET_SIZE;
	tEncInitOptions.dwBitRate = dwBitRate;

	tEncInitOptions.bEnableIntraVOPRefresh = TRUE;
	tEncInitOptions.bEnableShortHeader = dwMode & 0x00000001;
	tEncInitOptions.bUseGVOP = FALSE;
	tEncInitOptions.bUseCI = TRUE;
	tEncInitOptions.bDisableResyncMarker = 1 - ((dwMode>>1)&0x00000001);
	tEncInitOptions.bEnableDataPartitioned = (dwMode>>2) & 0x00000001;
	tEncInitOptions.bEnableRVLC = (dwMode>>3) & 0x00000001;
	tEncInitOptions.bUseDetailedVPHeader = FALSE;
	tEncInitOptions.eRCType = (dwBitRate!=0) ? RATE_CTRL_VQCB : RATE_CTRL_NONE;

	tEncInitOptions.dwBSBufSize = PB_BUFFER_SIZE;
	tEncInitOptions.dwTimeOut = 1;
	tEncInitOptions.dwSemaphore = (DWORD)NULL;
	tEncInitOptions.dwUserData = 0;
	tEncInitOptions.eBufTypeFlags = MPEGNONCIRCULAR;
	tEncInitOptions.pfnSend = NULL;
	tEncInitOptions.pfnEnter = NULL;
	tEncInitOptions.pfnLeave = NULL;

	tEncInitOptions.pbyEncUserData = pbyEncUserData;
#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
	tEncInitOptions.pbyYFrame = pbyFrame;
	tEncInitOptions.pbyCbFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
	tEncInitOptions.pbyCrFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
#else //!__ASM_ARCH_PLATFORM_BEETHOVEN_H__
	tEncInitOptions.pbyYFrame = pbyFrame;
	tEncInitOptions.pbyCbFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
	tEncInitOptions.pbyCrFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4) + (((((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4))>>2);
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

	tEncInitOptions.pObjectMem = NULL;

	dwMediaMemSize = Mp4VEnc_QueryMediaMemSize(&tEncInitOptions);

	tEncInitOptions.dwMediaBuffSize = dwMediaMemSize;
	tMemMgrState.dwBusNum = H264ENC_FRMAE_BUS_NUM;
	tMemMgrState.dwSize = dwMediaMemSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_1_BYTE;
	if ((tEncInitOptions.pbyMediaBuff=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate media buffer for encoder fail !! Size = %ld\n", tMemMgrState.dwSize);
		goto END_LEBEL_0;
	}
	else
	{
		printf("Allocate media buffer for encoder succeed !! Size = %ld\n", tMemMgrState.dwSize);
	}
	//tEncInitOptions.pbyMediaBuff = NULL;

	if (Mp4VEnc_Initial(&hEncObject, &tEncInitOptions) != S_OK)
	{
		printf("Initialize MPEG-4 encoder object fail !!\n");
		goto END_LEBEL_0;
	}

	tEncState.dwRoundingType = 0;
	tEncState.dwSeconds = 0;
	tEncState.dwTicks = 0;
	tEncState.dwEncUserDataLength = USER_DATA_LENGTH;
	tEncState.pdwPacketSize = (DWORD *)calloc(tEncInitOptions.dwMaxPacketCount, sizeof(DWORD));

#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__

	dwTotalSize = 0;
	printf("Single-thread non-circular encoding process start...\n");

	for (dwFrameCount=0; dwFrameCount<dwFrameNum; dwFrameCount++)
	{
		for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
		{
#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
			//NV12
			pbySrc = tEncInitOptions.pbyYFrame;
			if (fread(pbySrc, sizeof(BYTE), dwInHeight*dwInWidth, pfInput) != dwInHeight*dwInWidth)
			{
				printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
			pbySrc = tEncInitOptions.pbyCbFrame;
			if (fread(pbySrc, sizeof(BYTE), (dwInHeight*dwInWidth>>1), pfInput) != (dwInHeight*dwInWidth>>1))
			{
				printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
#else //!__ASM_ARCH_PLATFORM_BEETHOVEN_H__
			//printf("Mozart Input frame base frame\n");
			pbySrc = tEncInitOptions.pbyYFrame;
			for (j = 0; j < dwInHeight; j++, pbySrc+=dwPadWidth)
			{
				if (fread(pbySrc, sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
				{
					printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
					goto ENCODE_END;
				}
			}
			pbySrc = tEncInitOptions.pbyCbFrame;
			for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
			{
				if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
				{
					printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
					goto ENCODE_END;
				}
			}
			pbySrc = tEncInitOptions.pbyCrFrame;
			for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
			{
				if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
				{
					printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
					goto ENCODE_END;
				}
			}
			/*printf("Bach and Haydn Input block base frame\n");
			if (fread(pbyFrame, sizeof(BYTE), dwFrameSize, pfInput) != dwFrameSize)
			{
				printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}*/
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__
		}

		tEncState.dwSeconds = tEncState.dwSeconds + tEncState.dwTicks / 30;
		tEncState.dwTicks = tEncState.dwTicks % 30;

		tEncOptions.eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
		//if (tEncInitOptions.bEnableShortHeader == FALSE)
		//{
		//	tEncOptions.adwUserData[0] = (DWORD)&pbyNetbuf[2];
		//}
		//else
		//{
		//	tEncOptions.adwUserData[0] = (DWORD)&pbyNetbuf[1];
		//}
		tEncOptions.adwUserData[0] = (DWORD)pbyNetbuf;
		tEncOptions.adwUserData[1] = PB_BUFFER_SIZE;

		if (Mp4VEnc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		{
			printf("Mp4VEnc set options fail !!\n");
			break;
		}

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
		if (Mp4VEnc_OneFrame(hEncObject, &tEncState) != S_OK)
		{
			printf("Encode %ld frame fail !!\n", dwFrameCount);
			break;
		}

		//if (Mp4VEnc_StartOneFrame(hEncObject, &tEncState) != S_OK)
		//{
		//	printf("Encode %ld frame fail !!\n", dwFrameCount);
		//	break;
		//}
        //
		//if (Mp4VEnc_WaitOneFrame(hEncObject, &tEncState) != S_OK)
		//{
		//	printf("Encode %ld frame fail !!\n", dwFrameCount);
		//	break;
		//}

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

		tEncState.dwTicks+=(dwFrameSkip+1);

		dwFrameBytes = 0;
		for (dwPacketCount=0; dwPacketCount<tEncState.dwPacketCount; dwPacketCount++)
		{
			dwPacketSize = tEncState.pdwPacketSize[dwPacketCount];
			dwFrameBytes = dwFrameBytes + dwPacketSize;
		}
		//if (tEncInitOptions.bEnableShortHeader == FALSE)
		//{
		//	fwrite(&pbyNetbuf[2+tEncState.dwBsBufOffset], sizeof(BYTE), dwFrameBytes, pfOutput);
		//}
		//else
		//{
		//	fwrite(&pbyNetbuf[1+tEncState.dwBsBufOffset], sizeof(BYTE), dwFrameBytes, pfOutput);
		//}
		//printf("dwBsBuffOffset = %d\n", tEncState.dwBsBuffOffset);
		//{
		//	DWORD ii;
		//	for (ii = 0; ii < 16;ii++)
		//	{
		//		printf("%2x ", pbyNetbuf[ii]);
		//	}
		//	printf("\n");
		//	for (ii = 0; ii < 16;ii++)
		//	{
		//		printf("%2x ", pbyNetbuf[ii+16]);
		//	}
		//	printf("\n");
		//	for (ii = 0; ii < 16;ii++)
		//	{
		//		printf("%2x ", pbyNetbuf[ii+32]);
		//	}
		//	printf("\n");
		//	for (ii = 0; ii < 16;ii++)
		//	{
		//		printf("%2x ", pbyNetbuf[ii+48]);
		//	}
		//	printf("\n");
		//	for (ii = 0; ii < 16;ii++)
		//	{
		//		printf("%2x ", pbyNetbuf[ii+64]);
		//	}
		//	printf("\n");
		//}
		fwrite(&pbyNetbuf[tEncState.dwBsBuffOffset], sizeof(BYTE), dwFrameBytes, pfOutput);
		dwTotalSize = dwTotalSize + dwFrameBytes;

		printf("%2d Frame: Encoded bytes = %8d\n", dwFrameCount, dwFrameBytes);
        //Write_Frame(hEncObject, pfOutput);
	}

ENCODE_END:

#ifdef __USE_PROFILE__
	printf("Total cycles in AHB unit = %ld\n", dwTotalTicks*2);
	printf("TotalTicks@30fps         = %3.2f\n", ((float)dwTotalTicks*2/((float)dwFrameCount))*30/1000000);
	printf("TotalTicks@25fps         = %3.2f\n", ((float)dwTotalTicks*2/((float)dwFrameCount))*25/1000000);

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

	printf("Total Encoded bytes = %ld\n", dwTotalSize);
	printf("Bit rate = %lf\n", (((float)dwTotalSize*8/(float)(dwFrameCount))*25));

END_LEBEL_0:
	if (Mp4VEnc_Release(&hEncObject) != S_OK)
	{
		printf("Release MPEG-4 encoder object fail !!\n");
		exit(1);
	}

END_LEBEL_1:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyNetbuf) != S_OK)
	{
		exit(1);
	}

	if (MemMgr_Release(&hMemObject) != S_OK)
	{
		printf("Release memory manager object fail !!\n");
		exit(1);
	}

END_LEBEL_2:
	free(tEncState.pdwPacketSize);
	free(pbyEncUserData);
	fclose(pfOutput);
	fclose(pfInput);

	return 0;
}

/* =========================================================================================== */
