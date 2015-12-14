/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_App/G711Enc_App.c 7     11/05/05 3:16p Yc.lu $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * Description:
 *
 * $History: G711Enc_App.c $
 * 
 * *****************  Version 7  *****************
 * User: Yc.lu        Date: 11/05/05   Time: 3:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_App
 * Add profiling option for 16-channel G711Enc (8 MHz at FPGA Beethoven
 * A9).
 * 
 * *****************  Version 6  *****************
 * User: Angel        Date: 09/02/27   Time: 10:38a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_App
 * 
 * *****************  Version 5  *****************
 * User: Angel        Date: 08/02/15   Time: 3:42p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_App
 * 
 * *****************  Version 3  *****************
 * User: Angel        Date: 06/11/02   Time: 7:23p
 * Updated in $/rd_2/speech/G711/ARM/Encoder/G711Enc/G711Enc_App
 * 
 * *****************  Version 2  *****************
 * User: Angel        Date: 06/11/01   Time: 11:49a
 * Updated in $/rd_2/speech/G711/ARM/Encoder/G711Enc/G711Enc_App
 * 
 * *****************  Version 1  *****************
 * User: Angel        Date: 06/10/26   Time: 10:29a
 * Created in $/rd_2/speech/G711/ARM/Encoder/G711Enc/G711Enc_App
 * 
 */

/* ============================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

/* ============================================================================================== */
#include "G711Enc.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* ============================================================================================== */
#define InFrameSize	65536

/* ============================================================================================== */
int main (int argc, char **argv)
{
	HANDLE hObject;

	TG711EncInitOptions tInitOptions;
	TG711EncState tState;
	DWORD dwObjectMemSize;
    DWORD dwFrameCount;
	EG711EncMode eEncMode;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__
	
	void *pObjectMem;
	FILE *pfOutput;
	FILE *pfInput;
	SWORD *psw16bitPCM;
	BYTE *pbyOutFrame;

    if (argc != 4)
	{
		printf("Usage: G711enc  input_filename\n");
		printf("                  output_filename\n");
		printf("                  Encode_mode (0:A_Law, 1:U_Law)\n");
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

	eEncMode = (EG711EncMode)atoi(argv[3]);

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

	//eEncMode = (EG711EncMode)atoi(argv[3]);

	tInitOptions.dwVersion = G711ENC_VERSION;
	tInitOptions.dwInFrameSize = InFrameSize;
	tInitOptions.eEncMode = eEncMode;

	dwObjectMemSize = G711Enc_QueryMemSize(&tInitOptions);
	pObjectMem = (void *)calloc(sizeof(BYTE), dwObjectMemSize); 
	tInitOptions.pObjectMem = pObjectMem;

	if (G711Enc_Initial(&hObject, &tInitOptions) != S_OK)
	{
		printf("Initialize g.711 encoder fail !!\n");
		exit(1);
	}

	psw16bitPCM = (SWORD *)malloc((sizeof(SWORD)) * InFrameSize);
	pbyOutFrame = (BYTE *)malloc((sizeof(BYTE)) * InFrameSize);
	
	tState.pswInFrame = psw16bitPCM;
	tState.pbyOutFrame = pbyOutFrame; 
#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__
	dwFrameCount = 0;
    while (fread(psw16bitPCM, sizeof(SWORD)*InFrameSize, 1, pfInput)== 1)
	{   
        dwFrameCount++;
        /*Version 1.0.0.5 modification, Angel Hu, 2008.02.15*/
        #ifdef __USE_BIG_ENDIAN__
	    for (i=0; i<InFrameSize; i++)
        {
            tState.pswInFrame[i] = ((tState.pswInFrame[i]>>8)&0x00FF) + ((tState.pswInFrame[i]<<8)&0xFF00);
        }
        #endif
        /***************************************************/
		#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
		#endif //__USE_PROFILE__
		if (G711Enc_ProcessOneFrame(hObject, &tState) != S_OK)
		{
			printf("Process error \n");
			break;
		}
		#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
		#endif //__USE_PROFILE__
		fwrite(tState.pbyOutFrame, sizeof(BYTE), InFrameSize, pfOutput);
	}

#ifdef __USE_PROFILE__
    
	printf("Total cycles in APB unit (@ 8 KHz) = %ld\n", dwTotalTicks);
	printf("%3.2f MHz (AHB bus).\n", ((float)dwTotalTicks*2)*8000/((float)(InFrameSize*dwFrameCount)*1000000));
	printf("%3.2f MHz (16-chn encoding at FPGA Beethoven A9).\n", ((float)dwTotalTicks*64)*8000/((float)(InFrameSize*dwFrameCount)*1000000));

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__
    
	if (G711Enc_Release(&hObject) != S_OK)
	{
		printf("Release g711 encoder object fail !!\n");
		exit(1);
	}
	
    free(psw16bitPCM);
	free(pbyOutFrame);
	fclose(pfInput);
	fclose(pfOutput);
	exit(0);	
}
