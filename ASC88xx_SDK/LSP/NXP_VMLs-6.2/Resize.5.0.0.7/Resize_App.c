/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App/Resize_App.c 9     11-07-14 13:30 Kensuke.chen $
 *
 * * Copyright 2009 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: Resize_App.c $
 * 
 * *****************  Version 9  *****************
 * User: Kensuke.chen Date: 11-07-14   Time: 13:30
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 8  *****************
 * User: Kensuke.chen Date: 11-06-22   Time: 11:12
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 7  *****************
 * User: Kensuke.chen Date: 11-04-14   Time: 11:09
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 6  *****************
 * User: Kensuke.chen Date: 11-03-08   Time: 14:26
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 5  *****************
 * User: Kensuke.chen Date: 10-12-08   Time: 16:21
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 4  *****************
 * User: Kensuke.chen Date: 10-11-12   Time: 14:12
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 3  *****************
 * User: Kensuke.chen Date: 10-11-12   Time: 13:59
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 2  *****************
 * User: Kensuke.chen Date: 10-07-09   Time: 16:42
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 1  *****************
 * User: Kensuke.chen Date: 10-02-11   Time: 10:28
 * Created in $/rd_2/project/Mozart/Linux_Libraries/RESIZE/Resize_App
 * 
 * *****************  Version 1  *****************
 * User: Kensuke.chen Date: 10-02-11   Time: 10:27
 * Created in $/rd_2/project/Mozart/Linux_Libraries/LINUX_LIBRARIES/Resize/Resize_App
 * 
 * *****************  Version 1  *****************
 * User: Kensuke.chen Date: 10-02-11   Time: 10:26
 * Created in $/rd_2/project/Mozart/Linux_Libraries/LINUX_LIBRARIES/Resize/Resize_App
 * 
 * *****************  Version 7  *****************
 * User: Ronald       Date: 09/04/20   Time: 2:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 6  *****************
 * User: Alan         Date: 08/12/28   Time: 4:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * MODIFICATION: Change the application interface to support cropping
 * window resize - DONE.
 * 
 * *****************  Version 5  *****************
 * User: Ronald       Date: 08/01/16   Time: 5:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 4  *****************
 * User: Ronald       Date: 07/09/07   Time: 5:28p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 3  *****************
 * User: Fred         Date: 07/05/01   Time: 3:15p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 2  *****************
 * User: Fred         Date: 06/12/20   Time: 6:10p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * 
 * *****************  Version 1  *****************
 * User: Fred         Date: 06/12/20   Time: 5:48p
 * Created in $/rd_2/project/SoC/Linux_Libraries/Resize/Resize_App
 * 
 */

/* =========================================================================================== */
#include <stdlib.h>
#include <stdio.h>

#include "Resize.h"
#include "MemMgr.h" 
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* =========================================================================================== */
int main (int argc, char **argv)
{
    HANDLE hObject;
	HANDLE hMemObject;
    TResizeInitOptions tInitOptions;
	TMemMgrInitOptions tMemMgrInitOptions;	
	TResizeState tState;
	TMemMgrState tMemMgrState;


#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

	DWORD dwInYSize, dwInUVSize;
	DWORD dwOutYSize, dwOutUVSize;
	DWORD dwInWidth, dwInHeight;
	DWORD dwStartX, dwStartY;
	DWORD dwResizeWidth, dwResizeHeight;
	DWORD dwOutWidth, dwOutHeight;
	DWORD dwPixelFormat, dwImageMode;
	DWORD dwInStride;
	DWORD dwOutStride;
	DWORD dwFrameNum, dwFrameCount;
	DWORD dwInBuffSize, dwOutBuffSize;	
	DWORD dwOffsetY, dwOffsetC;
	DWORD dwCmptNum;

	FILE *pfInput, *pfOutput;

	BYTE *pbyInput, *pbyOutput;
	BYTE *pbyYInFrame, *pbyCbInFrame, *pbyCrInFrame;
	BYTE *pbyYOutFrame, *pbyCbOutFrame, *pbyCrOutFrame;
	BYTE *pbyYOrgFrame, *pbyCbOrgFrame, *pbyCrOrgFrame;


 	if (argc != 17)
	{
		printf("Usage: Resize_App input_filename\n");
		printf("                  output_filename\n");
		printf("                  input_image_mode (0: Frame mode, 1: Block mode)\n");
		printf("                  input_pixel_format (1: YCbCr420, 2: YCbCr422, 7: RGB24, 11: YCbCr444)\n");
		printf("                  component number (1: Y only, 3: YCbCr)\n");
		printf("                  frame_number\n");
		printf("                  input_width\n");
		printf("                  input_height\n");
		printf("                  input_stride (if input is block mode, input_stride = input_width)\n");		
		printf("                  start_x\n");
		printf("                  start_y\n");
		printf("                  resize_width\n");
		printf("                  resize_height\n");
		printf("                  output_width\n");
		printf("                  output_height\n");
		printf("                  output_stride\n");
		exit(1);
	}

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

	dwImageMode = atoi(argv[3]);
	dwPixelFormat = atoi(argv[4]);
	dwCmptNum = atoi(argv[5]);
	dwFrameNum = atoi(argv[6]);
	dwInWidth = atoi(argv[7]);
	dwInHeight = atoi(argv[8]);
	dwInStride = atoi(argv[9]);
	dwStartX = atoi(argv[10]);
	dwStartY = atoi(argv[11]);
	dwResizeWidth = atoi(argv[12]);
	dwResizeHeight  = atoi(argv[13]);
	dwOutWidth = atoi(argv[14]);
	dwOutHeight = atoi(argv[15]);
	dwOutStride = atoi(argv[16]);

	dwStartX = ((dwStartX+15)>>4)<<4;
	dwStartY = ((dwStartY+15)>>4)<<4;
/* Version 5.0.0.3 modification, 2011.04.14 */
/* ======================================== */
	printf("Start(%ld, %ld), In(%ld, %ld), Resize(%ld, %ld), Out(%ld, %ld)\n", dwStartX, dwStartY, dwInWidth, dwInHeight, dwResizeWidth, dwResizeHeight, dwOutWidth, dwOutHeight);

	dwInYSize = dwInStride * dwInHeight;
	dwOutYSize = dwOutStride * dwOutHeight;
	dwOffsetY = (dwStartY*dwInStride) + dwStartX;

	if(dwPixelFormat == YUV444 || dwPixelFormat == RGB24)
	{
		dwInUVSize = dwInStride * dwInHeight;
		dwOutUVSize = dwOutStride * dwOutHeight;
		dwOffsetC = dwOffsetY;
	}
	else if(dwPixelFormat == YUV422)
	{
		dwInUVSize = dwInStride * dwInHeight / 2;
		dwOutUVSize = dwOutStride * dwOutHeight / 2;
/* Version 5.0.0.2 modification, 2011.03.08 */
		dwOffsetC = ((dwStartY)*dwInStride>>1) + (dwStartX>>1);
/* ======================================== */
	}
	else 
	{
		dwInUVSize = dwInStride * dwInHeight / 4;
		dwOutUVSize = dwOutStride * dwOutHeight / 4;
/* Version 5.0.0.2 modification, 2011.03.08 */
		dwOffsetC = ((dwStartY>>1)*dwInStride>>1) + (dwStartX>>1);
/* ======================================== */
	}

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

	dwInBuffSize = (dwCmptNum==1) ? dwInYSize : (dwInYSize+dwInUVSize*2);
	dwOutBuffSize = (dwCmptNum==1) ? dwOutYSize: (dwOutYSize+dwOutUVSize*2);

/* Version 3.0.0.0 modification, 2007.09.07 */
	tMemMgrState.dwBusNum = RESIZE_IN_BUS_NUM;
/* ======================================== */
	tMemMgrState.dwSize = dwInBuffSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_16_BYTE;
    if ((pbyInput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate input frame buffer fail !!\n");
		exit(1);
	}
	else
	{
		printf("Allocate input frame buffer succeed !!\n");
	}

/* Version 3.0.0.0 modification, 2007.09.07 */
	tMemMgrState.dwBusNum = RESIZE_OUT_BUS_NUM;
/* ======================================== */
	tMemMgrState.dwSize = dwOutBuffSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_16_BYTE;
    if ((pbyOutput=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate output frame buffer fail !!\n");
		exit(1);
	}
	else
	{
		printf("Allocate output frame buffer succeed !!\n");
	}
	memset(pbyOutput, 0, dwOutBuffSize);

/* Initial library object. */
    tInitOptions.dwVersion = RESIZE_VERSION;
/* Version 5.0.0.0 modification, 2010.07.09 */
/* ======================================== */
    tInitOptions.pObjectMem = NULL;
	
	if (Resize_Initial(&hObject, &tInitOptions) != S_OK)
    {
        printf("Initialize resize object fail !!\n");
        exit(1);
    }

/* Version 5.0.0.1 modification, 2010.12.08 */
	if (dwPixelFormat == YUV420)
	{
		pbyYOrgFrame = &pbyInput[0];
		pbyCbOrgFrame = (dwCmptNum==1) ? &pbyInput[0]: &pbyInput[dwInHeight*dwInStride];
		pbyCrOrgFrame = (dwCmptNum==1) ? &pbyInput[0]: &pbyInput[dwInHeight*dwInStride*5/4];
	}
	else if (dwPixelFormat == YUV422)
	{
		pbyYOrgFrame = &pbyInput[0];
		pbyCbOrgFrame = (dwCmptNum==1) ? &pbyInput[0]: &pbyInput[dwInHeight*dwInStride];
		pbyCrOrgFrame = (dwCmptNum==1) ? &pbyInput[0]: &pbyInput[dwInHeight*dwInStride*3/2];
	}
	else
	{
		pbyYOrgFrame = &pbyInput[0];
		pbyCbOrgFrame = (dwCmptNum==1) ? &pbyInput[0]: &pbyInput[dwInHeight*dwInStride];
		pbyCrOrgFrame = (dwCmptNum==1) ? &pbyInput[0]: &pbyInput[dwInHeight*dwInStride*2];
	}

/* ======================================== */
	pbyYInFrame = pbyYOrgFrame + dwOffsetY;
	pbyCbInFrame = pbyCbOrgFrame + dwOffsetC;
	pbyCrInFrame = pbyCrOrgFrame + dwOffsetC;

	if (dwPixelFormat == YUV420)
	{
		pbyYOutFrame = &pbyOutput[0];
		pbyCbOutFrame = (dwCmptNum==1) ? &pbyOutput[0]: &pbyOutput[dwOutHeight*dwOutStride];
		pbyCrOutFrame = (dwCmptNum==1) ? &pbyOutput[0]: &pbyOutput[dwOutHeight*dwOutStride*5/4];
	}
	else if (dwPixelFormat == YUV422)
	{
		pbyYOutFrame = &pbyOutput[0];
		pbyCbOutFrame = (dwCmptNum==1) ? &pbyOutput[0]: &pbyOutput[dwOutHeight*dwOutStride];
		pbyCrOutFrame = (dwCmptNum==1) ? &pbyOutput[0]: &pbyOutput[dwOutHeight*dwOutStride*3/2];
	}
	else
	{
		pbyYOutFrame = &pbyOutput[0];
		pbyCbOutFrame = (dwCmptNum==1) ? &pbyOutput[0]: &pbyOutput[dwOutHeight*dwOutStride];
		pbyCrOutFrame = (dwCmptNum==1) ? &pbyOutput[0]: &pbyOutput[dwOutHeight*dwOutStride*2];
	}

/* Version 5.0.0.0 modification, 2010.07.09 */	
	tState.dwInStride = dwInStride;
	tState.dwOutStride = dwOutStride;
	tState.dwInWidth = dwResizeWidth;
	tState.dwInHeight = dwResizeHeight;
	tState.dwOutWidth = dwOutWidth;
	tState.dwOutHeight = dwOutHeight;
	tState.dwImageMode = dwImageMode;
	tState.ePixelFormat = dwPixelFormat;
	tState.dwCmptNum = dwCmptNum;
/* ======================================== */

	tState.pbyYInFrame = pbyYInFrame;
	tState.pbyCbInFrame = pbyCbInFrame;
	tState.pbyCrInFrame = pbyCrInFrame;
	tState.pbyYOutFrame = pbyYOutFrame;
	tState.pbyCbOutFrame = pbyCbOutFrame;
	tState.pbyCrOutFrame = pbyCrOutFrame;

#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__
	
	if (Resize_CheckParam(hObject, &tState) != S_OK)
	{
		printf("State parameters setting error !!\n");
/* Version 5.0.0.5 modification, 2011.06.22 */
/* ======================================== */
		goto PROCESS_QUIT;
	}

	for (dwFrameCount=0; dwFrameCount<dwFrameNum; dwFrameCount++)
	{			
		if (fread (pbyInput, sizeof(BYTE), dwInBuffSize, pfInput) != dwInBuffSize)
		{
			printf("Resize finish !!\n");
			goto PROCESS_QUIT;
		}
		//rewind(pfInput);

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

		if (Resize_ProcessOneFrame(hObject, &tState) != S_OK)
		{
			printf("Process error in frame %ld !!\n", dwFrameCount);
		}

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

		fwrite(pbyOutput, sizeof(BYTE), dwOutBuffSize, pfOutput);
/* Version 5.0.0.5 modification, 2011.06.22 */
/* ======================================== */
		printf("%ld Frame process complete !!\n", dwFrameCount);	
	}

PROCESS_QUIT:

#ifdef __USE_PROFILE__
	printf("Total cycles in APB unit = %ld\n", dwTotalTicks);
	printf("TotalTicks@15fps         = %3.2fMHz\n", ((float)dwTotalTicks*2/((float)dwFrameNum))*15/1000000);
	printf("TotalTicks@25fps         = %3.2fMHz\n", ((float)dwTotalTicks*2/((float)dwFrameNum))*25/1000000);
	printf("TotalTicks@30fps         = %3.2fMHz\n", ((float)dwTotalTicks*2/((float)dwFrameNum))*30/1000000);

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

    if (Resize_Release(&hObject) != S_OK)
    {
        printf("Release resize object fail !!\n");
        exit(1);
    }

    fclose(pfInput);
    fclose(pfOutput);
	

	return 0;
}
/* =========================================================================================== */
