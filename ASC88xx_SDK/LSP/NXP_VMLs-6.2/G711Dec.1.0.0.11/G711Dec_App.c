/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/G711Dec/G711Dec_App/G711Dec_App.c 2     09/02/27 11:06a Angel $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * Description:
 *
 * $History: G711Dec_App.c $
 * 
 * *****************  Version 2  *****************
 * User: Angel        Date: 09/02/27   Time: 11:06a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G711Dec/G711Dec_App
 * 
 * *****************  Version 1  *****************
 * User: Angel        Date: 06/11/03   Time: 3:57p
 * Created in $/rd_2/project/SoC/Linux_Libraries/G711Dec/G711Dec_App
 * 
 * *****************  Version 3  *****************
 * User: Angel        Date: 06/11/01   Time: 11:41a
 * Updated in $/rd_2/speech/G711/ARM/Decoder/G711Dec/G711Dec_App
 * 
 * *****************  Version 2  *****************
 * User: Angel        Date: 06/10/30   Time: 9:25p
 * Updated in $/rd_2/speech/G711/ARM/Decoder/G711Dec/G711Dec_App
 * 
 * *****************  Version 1  *****************
 * User: Angel        Date: 06/10/26   Time: 11:28a
 * Created in $/rd_2/speech/G711/ARM/Decoder/G711Dec/G711Dec_App
 * 
 */
 /* =========================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

/* ============================================================================================== */
#include "G711Dec.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* ============================================================================================== */
#define InFrameSize	65536

/* ============================================================================================== */
int main (int argc, char **argv)
{
	HANDLE hObject;

	TG711DecInitOptions tInitOptions;
	TG711DecState tState;
	DWORD dwObjectMemSize;
    DWORD dwFrameCount;
	EG711DecMode eDecMode;

	#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
	#endif //__USE_PROFILE__

	void *pObjectMem;
	FILE *pfOutput;
	FILE *pfInput;

	BYTE *pby8bitPCM;
	SWORD *pswOutFrame;


	if (argc != 4)
	{
		printf("Usage: G711dec  input_filename\n");
		printf("                  output_filename\n");
		printf("                  Decode_mode (0:A_Law, 1:U_Law)\n");
		exit(1);
	}

	if ((pfInput=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open input file %s fail !!\n", argv[1]);
		exit(1);
	}

	if ((pfOutput=fopen(argv[2], "wb")) == NULL)
	{
		printf("Open output file %s fail !!\n", argv[2]);
		exit(1);
	}

	eDecMode = (EG711DecMode)atoi(argv[3]);

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

	tInitOptions.dwVersion = G711DEC_VERSION;
	tInitOptions.dwInFrameSize = InFrameSize;
	tInitOptions.eDecMode = eDecMode;

	dwObjectMemSize = G711Dec_QueryMemSize(&tInitOptions);
	pObjectMem = (void *)calloc(sizeof(BYTE), dwObjectMemSize); 
	tInitOptions.pObjectMem = pObjectMem;

	if (G711Dec_Initial(&hObject, &tInitOptions) != S_OK)
	{
		printf("Initialize g.711 decoder fail !!\n");
		exit(1);
	}

	pby8bitPCM = (BYTE *)malloc((sizeof(*pby8bitPCM))*InFrameSize);
	pswOutFrame = (SWORD *)malloc((sizeof(*pswOutFrame))*InFrameSize);

	tState.pbyInFrame = pby8bitPCM;
	tState.pswOutFrame = pswOutFrame;

	#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
	#endif //__USE_PROFILE__
    dwFrameCount = 0;
    while(fread(pby8bitPCM, sizeof(BYTE)*InFrameSize, 1, pfInput)==1)
	{	
        dwFrameCount++;
		#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
		#endif //__USE_PROFILE__

        if (G711Dec_ProcessOneFrame(hObject, &tState) != S_OK)
		{
			printf("Process error ");
			break;
		}
		#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
		#endif //__USE_PROFILE__
		fwrite(tState.pswOutFrame, sizeof(SWORD), InFrameSize, pfOutput);
	}

	#ifdef __USE_PROFILE__
	printf("Total cycles in APB unit = %d\n", dwTotalTicks);
	printf("MHz @ 8000fs = %3.2fMHz\n", ((float)dwTotalTicks*2)*8000/((float)(InFrameSize*dwFrameCount)*1000000));

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
	#endif //__USE_PROFILE__

	if (G711Dec_Release(&hObject) != S_OK)
	{
		printf("Release g711 decoder object fail !!\n");
		exit(1);
	}

    free(pby8bitPCM);
	free(pswOutFrame);

	fclose(pfInput);
	fclose(pfOutput); 
	exit(0);
	
}
