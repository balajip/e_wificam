/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App/SVCEnc_App.c 7     13/09/14 5:19p Jaja $
 *
 * Copyright 2007-2011 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: SVCEnc_App.c $
 *
 * *****************  Version 7  *****************
 * User: Jaja         Date: 13/09/14   Time: 5:19p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 * *****************  Version 6  *****************
 * User: Jaja         Date: 13/08/30   Time: 10:10a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 * *****************  Version 5  *****************
 * User: Jaja         Date: 11/11/23   Time: 7:19p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 * *****************  Version 4  *****************
 * User: Jaja         Date: 11/07/20   Time: 12:25p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 * *****************  Version 3  *****************
 * User: Jaja         Date: 11/07/06   Time: 10:40a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 * *****************  Version 2  *****************
 * User: Jaja         Date: 11/06/30   Time: 2:12p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 * *****************  Version 1  *****************
 * User: Jaja         Date: 11/06/23   Time: 4:30p
 * Created in $/rd_2/project/Mozart/Linux_Libraries/SVCEnc/SVCEnc_App
 *
 *
 */

/* =========================================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* =========================================================================================== */
#include "SVCEnc.h"
#include "MemMgr.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* =========================================================================================== */
#define PB_BUFFER_SIZE		1024*1024*4
#define PACKET_SIZE			1460
#define MAX_WIDTH			1920
#define MAX_HEIGHT			1088
#define USER_DATA_LENGTH	1
#define CALIBRATE_TIME		1

/* =========================================================================================== */
#define SVCENC_STREAM_BUS_NUM	0
#define SVCENC_FRMAE_BUS_NUM	1

//#define SVCENC_LOOP
#define __ASM_ARCH_PLATFORM_MOZART_V2_H__

/* =========================================================================================== */
#include "SVCEnc.h"
#include "MemMgr.h"

extern FLOAT  fBW0, fBW1;
extern FLOAT  fOp;

/* =========================================================================================== */
int main (int argc, char **argv)
{
	TSVCEncInitOptions tEncInitOptions;
	TSVCEncOptions tEncOptions;
	TSVCEncState tEncState;
	TMemMgrInitOptions tMemMgrInitOptions;
	TMemMgrState tMemMgrState;

	HANDLE hMemObject;
	HANDLE hEncObject;

	DWORD dwFrameCount, dwFrameSize, dwFrameBytes;
	DWORD dwFrameNum;
	DWORD dwFrameSkip, dwSkipCount;
	DWORD dwPadWidth, dwPadHeight;
	DWORD dwInWidth, dwInHeight, dwEncWidth, dwEncHeight, dwQP, dwRefFrameNum;
    DWORD dwMaxFrameRate;
	DWORD dwTotalBytes;
	DWORD dwProfile, dwBitRate;
	DWORD dwIntraSliceInterval, dwIntraRefreshMBNum;
	DWORD dwSearchRangeX, dwSearchRangeY;
	DWORD dwConstrainedIntraPredEn, dwAdvancedSkipModeEn;
	DWORD dwIntra16x16PredEn, dwInterChmaSADEn, dwFrameperSecond;
	DWORD j, i;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

	FILE *pfOutput;
	FILE *pfRecOutput;
	FILE *pfInput;
	BYTE *pbyNetbuf;
	BYTE *pbyFrame, *pbySrc;
	BYTE *pbyEncUserData;

#ifdef __ASM_ARCH_PLATFORM_MOZART_V2_H__
	FILE *pfObjectInput;
	BYTE *pbyObjectInfo;
#endif //__ASM_ARCH_PLATFORM_MOZART_V2_H__

#ifdef __ASM_ARCH_PLATFORM_MOZART_V2_H__
//#ifdef __USE_PROFILE__
//	if (argc != 29)
//#else //!__USE_PROFILE__
	if (argc != 27)
//#endif //!__USE_PROFILE__
	{
		printf("Usage:	svcenc\n");
		printf("	input_yuv_file\n");
		printf("	output_yuv_file\n");
		printf("	output_bitstream_file\n");
		printf("	encode_frame_num\n");
		printf("	input_frame_width\n");
		printf("	input_frame_height\n");
		printf("	encoded_frame_width\n");
		printf("	encoded_frame_height\n");
		printf("	frame_skip\n");
		printf("	reference_frame_number 1 or 2(default)\n");
		printf("	init_qp(0-51)\n");
		printf("	profile(0: baseline profile, 1: main profile, 2: high profile(default))\n");
		printf("	P_interval\n");
		printf("	search_range_x(1:16, 2:32, 4:64(default))\n");
		printf("	intra_16x16_Enable(0:Disable, 1:Enable(default))\n");
		printf("	refresh_MB_number (0:No Refresh MBs)\n");
		printf("	FME_skip_option_enable(0: Disable, 1:Enable(default))\n");
		printf("	constrainedIntraPred (0: Disable(default), 1:Enable)\n");
		printf("	chma_pel_satd (0: Disable(default), 1:Enable)\n");
		printf("	search_range_y(1:16, 2:32, 4:64(default))\n");
		printf("	bit_rate\n");
		printf("	frame_per_second(60000:30fps(default), 50000: 25fps\n");
        printf("	svc_layer_num(1:single layer, 2:two layer, 3:three layer, 4:four layer\n");
        printf("	svc_sei_header(0:disable, 1:enable\n");
		printf("	object_qp_delta(-26~+25, for Mozart V2/V3)\n");
		printf("    object_input(for Mozart V2/V3)\n");
#ifdef __USE_PROFILE__
		printf("	frame rate\n");
#endif //__USE_PROFILE__
		exit(1);
	}
#else //__ASM_ARCH_PLATFORM_MOZART_V2_H__
	if (argc != 25)
	{
		printf("Usage:	svcenc\n");
		printf("	input_yuv_file\n");
		printf("	output_yuv_file\n");
		printf("	output_bitstream_file\n");
		printf("	encode_frame_num\n");
		printf("	input_frame_width\n");
		printf("	input_frame_height\n");
		printf("	encoded_frame_width\n");
		printf("	encoded_frame_height\n");
		printf("	frame_skip\n");
		printf("	reference_frame_number 1 or 2(default)\n");
		printf("	init_qp(0-51)\n");
		printf("	profile(0: baseline profile, 1: main profile, 2: high profile(default))\n");
		printf("	P_interval\n");
		printf("	search_range_x(1:16, 2:32, 4:64(default))\n");
		printf("	intra_16x16_Enable(0:Disable, 1:Enable(default))\n");
		printf("	refresh_MB_number (0:No Refresh MBs)\n");
		printf("	FME_skip_option_enable(0: Disable, 1:Enable(default))\n");
		printf("	constrainedIntraPred (0: Disable(default), 1:Enable)\n");
		printf("	chma_pel_satd (0: Disable(default), 1:Enable)\n");
		printf("	search_range_y(1:16, 2:32, 4:64(default))\n");
		printf("	bit_rate\n");
		printf("	frame_per_second(60000:30fps(default), 50000: 25fps\n");
        printf("	svc_layer_num(1:single layer, 2:two layer, 3:three layer, 4:four layer)\n");
        printf("	svc_sei_header(0:disable, 1:enable\n");
		exit(1);
	}
#endif //__ASM_ARCH_PLATFORM_MOZART_V2_H__

	if ((pfInput=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open input YUV file %s fail !!\n", argv[1]);
		exit(1);
	}

	if ((pfRecOutput=fopen(argv[2], "wb")) == NULL)
	{
		printf("Open output YUV file %s fail !!\n", argv[2]);
		goto RECFILE_FAIL;
	}

	if ((pfOutput=fopen(argv[3], "wb")) == NULL)
	{
		printf("Open output bitstream file %s fail !!\n", argv[3]);
		goto OUTFILE_FAIL;
	}

#ifdef __ASM_ARCH_PLATFORM_MOZART_V2_H__
	if ((pfObjectInput=fopen(argv[26], "rb")) == NULL)
	{
		printf("Open input YUV file %s fail !!\n", argv[1]);
		exit(1);
	}
#endif //__ASM_ARCH_PLATFORM_MOZART_V2_H__

	dwFrameNum = atoi(argv[4]);
	dwInWidth = atoi(argv[5]);
	dwInHeight = atoi(argv[6]);
	dwEncWidth = atoi(argv[7]);
	dwEncHeight = atoi(argv[8]);
	dwFrameSkip = atoi(argv[9]);
	dwRefFrameNum = atoi(argv[10]);
	dwQP = atoi(argv[11]);
	dwProfile = atoi(argv[12]);
	dwIntraSliceInterval = atoi(argv[13]);
	dwSearchRangeX = atoi(argv[14]);
	dwIntra16x16PredEn = atoi(argv[15]);
	dwIntraRefreshMBNum = atoi(argv[16]);
	dwAdvancedSkipModeEn = atoi(argv[17]);
	dwConstrainedIntraPredEn = atoi(argv[18]);
	dwInterChmaSADEn = atoi(argv[19]);
	dwSearchRangeY = atoi(argv[20]);
	dwBitRate = atoi(argv[21]);
	dwFrameperSecond = atoi(argv[22]);
	dwPadWidth = (((dwInWidth + 15)>>4)<<4);
	dwPadHeight = (((dwInHeight + 15)>>4)<<4);
	dwFrameSize = dwPadWidth*dwPadHeight;
	//dwFrameSize = dwPadWidth*dwPadHeight * 3/2;

	if ((pbyEncUserData=(BYTE *)calloc(USER_DATA_LENGTH, sizeof(BYTE))) == NULL)
	{
		printf("Allocate user data buffer fail !!\n");
		goto USRDATA_FAIL;
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
		goto MEMMGR_INITIAL_FAIL;
	}

#ifdef __ASM_ARCH_PLATFORM_MOZART_V2_H__
    tMemMgrState.dwBusNum = SVCENC_FRMAE_BUS_NUM;
	tMemMgrState.dwSize = (((((MAX_WIDTH+15)>>4) + 1)>>1)<<1)*((MAX_HEIGHT+15)>>4)*4;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyObjectInfo = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate object buffer %ld fail !!\n", tMemMgrState.dwSize);
		goto OBJECTBUF_FAIL;
	}
	else
	{
        printf("Allocate object buffer %ld succeed !!\n", tMemMgrState.dwSize);
	}

	tMemMgrState.dwBusNum = SVCENC_FRMAE_BUS_NUM;
	tMemMgrState.dwSize = (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)*3/2;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyFrame = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate input frame buffer %ld fail !!\n", tMemMgrState.dwSize);
		goto FRAMEBUF_FAIL;
	}
	else
	{
        printf("Allocate input frame buffer %ld succeed !!\n", tMemMgrState.dwSize);
	}
	memset(pbyFrame, 128, tMemMgrState.dwSize);
#else //!__ASM_ARCH_PLATFORM_MOZART_V2_H__
	tMemMgrState.dwBusNum = SVCENC_FRMAE_BUS_NUM;
	tMemMgrState.dwSize = (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)*3/2;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyFrame = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate input frame buffer %ld fail !!\n", tMemMgrState.dwSize);
		goto FRAMEBUF_FAIL;
	}
	else
	{
		printf("Allocate v1 input frame buffer %ld succeed !!\n", tMemMgrState.dwSize);
	}
    memset(pbyFrame, 128, tMemMgrState.dwSize);
#endif //__ASM_ARCH_PLATFORM_MOZART_V2_H__

	tMemMgrState.dwBusNum = SVCENC_STREAM_BUS_NUM;
	tMemMgrState.dwSize = PB_BUFFER_SIZE;
	tMemMgrState.eAlignType = ALIGN_TYPE_32_BYTE;
	if ((pbyNetbuf=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate bit stream buffer fail !! Size = %ld\n", tMemMgrState.dwSize);
		goto NETBUF_FAIL;
	}
	else
	{
		printf("Allocate bit stream buffer succeed !! Size = %ld\n", tMemMgrState.dwSize);
	}

	tEncInitOptions.dwVersion = SVCENC_VERSION;
	tEncInitOptions.dwMaxFrameWidth = MAX_WIDTH;
	tEncInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
	tEncInitOptions.dwEncWidth = dwEncWidth;
	tEncInitOptions.dwEncHeight = dwEncHeight;
	tEncInitOptions.dwInWidth = dwInWidth;
	tEncInitOptions.dwInHeight = dwInHeight;
	tEncInitOptions.dwInHorzOffset = 0;
	tEncInitOptions.dwInVertOffset = 0;
	tEncInitOptions.dwRefFrameNum = dwRefFrameNum;			// 1 = one reference, 2 = two reference
	tEncInitOptions.dwSearchRangeX = dwSearchRangeX;		// 1 => +-16, 2 => +-32, 4 => +-60
	tEncInitOptions.dwSearchRangeY = dwSearchRangeY;		// 1 => +-16, 2 => +-32, 4 => +-60
	tEncInitOptions.dwStreamBusNum = SVCENC_STREAM_BUS_NUM;
	tEncInitOptions.dwFrameBusNum = SVCENC_FRMAE_BUS_NUM;
	//printf("stream bus/frame bus = %d, %d !!\n", tEncInitOptions.dwStreamBusNum, tEncInitOptions.dwFrameBusNum);
	tEncInitOptions.dwTicksPerSecond = dwFrameperSecond;	// 60000 = 30fps, 50000 = 25fps
	tEncInitOptions.dwTicksPerFrame = 1001;
	tEncInitOptions.dwQuant = dwQP;
	tEncInitOptions.dwIntraSliceInterval = dwIntraSliceInterval;
	tEncInitOptions.bIntraSliceRefreshEn = TRUE;
	tEncInitOptions.bAdvancedSkipModeEn = (dwAdvancedSkipModeEn==1) ? TRUE: FALSE;
	tEncInitOptions.bConstrainedIntraPredEn = (dwConstrainedIntraPredEn==1) ? TRUE: FALSE;
	tEncInitOptions.bIntra16x16PredEn = (dwIntra16x16PredEn==1) ? TRUE: FALSE;
	tEncInitOptions.bInterChmaSADEn = dwInterChmaSADEn;
	tEncInitOptions.dwProfile = dwProfile;
	tEncInitOptions.dwBitRate = dwBitRate;
	tEncInitOptions.eRCType = (dwBitRate!=0) ? RATE_CTRL_CVBR : RATE_CTRL_NONE;
	//tEncInitOptions.eRCType = (dwBitRate!=0) ? RATE_CTRL_VQCB : RATE_CTRL_NONE;

	tEncInitOptions.dwIntraRefreshMBNum = dwIntraRefreshMBNum;

	tEncInitOptions.bEncByteStreamHdr = TRUE;

    tEncInitOptions.dwLayerNum = atoi(argv[23]);
	tEncInitOptions.bSVCSEIEn = atoi(argv[24]);
	tEncInitOptions.bKeyFrameOnlyMode = FALSE;
#ifdef __ASM_ARCH_PLATFORM_MOZART_V2_H__
	tEncInitOptions.sdwObjectQp = atoi(argv[25]);
	tEncInitOptions.pbyObjectMask = pbyObjectInfo;
	//tEncInitOptions.bFastModeEnable = 1;
	tEncInitOptions.dwSpeedNum = 0;
#else //!__ASM_ARCH_PLATFORM_MOZART_V2_H__
	tEncInitOptions.sdwObjectQp = 0;
	tEncInitOptions.pbyObjectMask = NULL;
	//tEncInitOptions.bFastModeEnable = 0;
	tEncInitOptions.dwSpeedNum = 0;
#endif //__ASM_ARCH_PLATFORM_MOZART_V2_H__

	tEncInitOptions.dwBSBufSize = 0;
	tEncInitOptions.dwTimeOut = 1;
	tEncInitOptions.dwSemaphore = (DWORD)NULL;
	tEncInitOptions.dwUserData = 0;
	tEncInitOptions.eBufTypeFlags = H264NONCIRCULAR;
	tEncInitOptions.pfnSend = NULL;
	tEncInitOptions.pfnEnter = NULL;
	tEncInitOptions.pfnLeave = NULL;

	tEncInitOptions.pbyEncUserData = pbyEncUserData;
	tEncInitOptions.pbyYFrame = pbyFrame;
	tEncInitOptions.pbyCbFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
	tEncInitOptions.pbyCrFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4) + (((((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4))>>2);

	tEncInitOptions.pObjectMem = NULL;

	tEncInitOptions.dwMediaBuffSize = SVCEnc_QueryMediaMemSize(&tEncInitOptions);
	tMemMgrState.dwBusNum = SVCENC_FRMAE_BUS_NUM;
	tMemMgrState.dwSize = tEncInitOptions.dwMediaBuffSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_32_BYTE;
	if ((tEncInitOptions.pbyMediaBuff=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate bit stream buffer fail !! Size = %ld\n", tMemMgrState.dwSize);
		goto NETBUF_FAIL;
	}
	else
	{
		printf("Allocate bit stream buffer succeed !! Size = %ld\n", tMemMgrState.dwSize);
	}
	tEncInitOptions.pbyMediaBuff = NULL;

	if (SVCEnc_Initial(&hEncObject, &tEncInitOptions) != S_OK)
	{
		printf("Initialize H.264 encoder object fail !!\n");
		goto SVCENC_INITIAL_FAIL;
	}

    dwMaxFrameRate = (tEncInitOptions.dwTicksPerSecond+(tEncInitOptions.dwTicksPerFrame>>1)) / tEncInitOptions.dwTicksPerFrame / 2;

	tEncState.dwSeconds = 0;
	tEncState.dwTicks = 0;
	tEncState.dwEncUserDataLength = USER_DATA_LENGTH;

#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__

#ifdef SVCENC_LOOP
	for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
	{
        pbySrc = tEncInitOptions.pbyYFrame;
		for (j = 0; j < dwInHeight; j++, pbySrc+=dwPadWidth)
		{
			if (fread(pbySrc, sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
			{
				printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
		}

		pbySrc = tEncInitOptions.pbyCbFrame;
		for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
		{
			if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
			{
				printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
		}

		pbySrc = tEncInitOptions.pbyCrFrame;
		for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
		{
			if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
			{
				printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
		}
	}
#endif //SVCENC_LOOP

#ifdef SVCENC_LOOP
	for (dwFrameCount=0, dwTotalBytes=0; ; dwFrameCount++)
#else //!SVCENC_LOOP
	for (dwFrameCount=0, dwTotalBytes=0; dwFrameCount<dwFrameNum; dwFrameCount++)
#endif //SVCENC_LOOP
	{
		//fseek(pfInput, 0, SEEK_SET);
#ifndef SVCENC_LOOP
		for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
		{
			pbySrc = tEncInitOptions.pbyYFrame;
			for (j = 0; j < dwInHeight; j++, pbySrc+=dwPadWidth)
			{
				if (fread(pbySrc, sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
				{
					printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
					goto ENCODE_END;
				}
			}

			pbySrc = tEncInitOptions.pbyCbFrame;
			for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
			{
				if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
				{
					printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
					goto ENCODE_END;
				}
			}

			pbySrc = tEncInitOptions.pbyCrFrame;
			for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
			{
				if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
				{
					printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
					goto ENCODE_END;
				}
			}
	        //pbySrc = tEncInitOptions.pbyObjectMask;
	        //if (fread(pbySrc, sizeof(BYTE), ((((dwPadWidth>>4)+1)>>1)<<1)*(dwPadHeight>>4)*4, pfObjectInput) != (((((dwPadWidth>>4)+1)>>1)<<1)*(dwPadHeight>>4)*4))
	        //{
	        //    printf("There are only %ld frames in the input YUV sequence !!\n", dwFrameCount);
	        //    goto ENCODE_END;
	        //}
		}

#endif //SVCENC_LOOP

		tEncState.dwSeconds = tEncState.dwSeconds + tEncState.dwTicks / dwMaxFrameRate;
		tEncState.dwTicks = tEncState.dwTicks % dwMaxFrameRate;
		tEncState.bLast = (dwFrameCount>=(dwFrameNum-1));

		tEncOptions.eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
		tEncOptions.adwUserData[0] = (DWORD)pbyNetbuf;
		tEncOptions.adwUserData[1] = PB_BUFFER_SIZE;

		if (SVCEnc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		{
			printf("Set options fail !!\n");
			goto ENCODE_END;
		}

		//if (dwFrameCount == 0)
		//{
    	//	tEncOptions.eOptionFlags = VIDEO_CHANGE_REGION_OF_INTEREST_INFO;
    	//	tEncOptions.adwUserData[0] = (DWORD)tEncInitOptions.pbyObjectMask;
    	//	tEncOptions.adwUserData[1] = (DWORD)-24;
        //
    	//	if (SVCEnc_SetOptions(hEncObject, &tEncOptions) != S_OK)
    	//	{
    	//		printf("Set options fail !!\n");
    	//		goto ENCODE_END;
    	//	}
    	//}

    	//if (dwFrameCount == 0)
		//{
    	//	tEncOptions.eOptionFlags = VIDEO_CHANGE_SCALABILITY_TEMPORAL_LAYER_NUM;
    	//	tEncOptions.adwUserData[0] = 2;
    	//
    	//	if (SVCEnc_SetOptions(hEncObject, &tEncOptions) != S_OK)
    	//	{
    	//		printf("Set options fail !!\n");
    	//		goto ENCODE_END;
    	//	}
    	//}

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

		if (SVCEnc_OneFrame(hEncObject, &tEncState) != S_OK)
		{
			printf("Encode %ld frame fail !!\n", dwFrameCount);
			break;
		}

		/*
		if (SVCEnc_StartOneFrame(hEncObject, &tEncState) != S_OK)
		{
			printf("Encode %ld frame fail !!\n", dwFrameCount);
			break;
		}

		if (SVCEnc_WaitOneFrame(hEncObject, &tEncState) != S_OK)
		{
			printf("Encode %ld frame fail !!\n", dwFrameCount);
			break;
		}*/

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

		/*
		{
			DWORD i;

			for (i=0; i<tEncState.dwNALCount; i++)
			{
				printf("NAL type = %d, NAL byte count = %d\n", tEncState.aeNALType[i], tEncState.adwNALBytes[i]);
			}
		}
		*/
        tEncState.dwTicks+=(dwFrameSkip+1);

		dwFrameBytes = tEncState.dwEncBytes;

		//Write_Frame(hEncObject, pfRecOutput);
#ifndef SVCENC_LOOP
		fwrite(pbyNetbuf, sizeof(BYTE), dwFrameBytes, pfOutput);
#endif //SVCENC_LOOP
		printf("%ld Frame: Encoded bytes/bits = %ld, %ld\n", dwFrameCount, dwFrameBytes, dwFrameBytes<<3);
		dwTotalBytes = dwTotalBytes + dwFrameBytes;
	}


ENCODE_END:
	printf("Bit rate = %.2f\n", dwTotalBytes*8*30.0/dwFrameCount);
	if (SVCEnc_Release(&hEncObject) != S_OK)
	{
		printf("Release H.264 encoder object fail !!\n");
		exit(1);
	}
#ifdef __USE_PROFILE__
	printf("Total cycles in AHB unit = %ld\n", dwTotalTicks*2);

	//{
	//	DWORD dwTargetFrame = atoi(argv[28]);
	//	printf("xx %3.2f  MHz (H4EE HW: %3.2f, SW:  %2.2f MHz, Bus bandwidth(0, 2): %1.2f, %2.2f MHz),  FullHD1080p@%ldfps, 2 reference frames, quality factor 20, bitrate@%ldfps %5.2f kbits \n",
	//		((FLOAT)dwTotalTicks*2*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		(fOp*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		((FLOAT)dwTotalTicks*2*dwTargetFrame/(FLOAT)dwFrameCount - fOp*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount)/1000000,
	//		(fBW0*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		(fBW1*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		dwTargetFrame,
	//		dwTargetFrame,
	//		(FLOAT)dwTotalBytes*8*dwTargetFrame/(FLOAT)dwFrameCount/1000);
	//}

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

SVCENC_INITIAL_FAIL:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyNetbuf) != S_OK)
	{
		exit(1);
	}

NETBUF_FAIL:
#ifndef __SEQ_10FRAME_PROFILE__
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyFrame) != S_OK)
	{
		exit(1);
	}
#endif
#ifdef __ASM_ARCH_PLATFORM_MOZART_V2_H__
OBJECTBUF_FAIL:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyObjectInfo) != S_OK)
	{
		printf("Release memory manager object fail !!\n");
		exit(1);
	}
#endif //__ASM_ARCH_PLATFORM_MOZART_V2_H__

FRAMEBUF_FAIL:
	if (MemMgr_Release(&hMemObject) != S_OK)
	{
		printf("Release memory manager object fail !!\n");
		exit(1);
	}

MEMMGR_INITIAL_FAIL:
	free(pbyEncUserData);

USRDATA_FAIL:
	fclose(pfOutput);

OUTFILE_FAIL:
	fclose(pfRecOutput);

RECFILE_FAIL:
	fclose(pfInput);

	return 0;
}
/* =========================================================================================== */
