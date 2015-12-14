/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/AAC4Enc/AAC4Enc_App/AAC4Enc_App.c 25    12/01/19 2:39p Angel $
 *
 * Copyright 2009 ______, Inc.  All rights reserved.
 *
 */

/* =========================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include "AAC4Enc.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* =========================================================================================== */
#define PBBUFFERSIZE     12288
#define INPUTFRAMESZ      1024

/* =========================================================================================== */
typedef struct wave_header
{
    /* RIFF chunk */
    DWORD dwRiffID;
    DWORD dwRiffSz;
    DWORD dwWAVEID;

    /* Format chunk */
    DWORD dwFmtID;
    DWORD dwFmtSz;
    WORD wFormatTag;
    WORD wChannels;
    DWORD dwSamplesPerSec;
    DWORD dwAvgBytesPerSec;
    WORD wBlockAlign;
    WORD wBitsPerSample;

    /* Data chunk */
    DWORD dwDataID;	// that value should be 1635017060
    DWORD dwDataSz;
} TWaveHeader;

/* =========================================================================================== */
int main(int argc, char* argv[])
{
	HANDLE 				hEncObject;
    TAAC4EncInitOptions tEncInitOptions;
    TAAC4EncState 		tEncState;
    TAAC4EncOptions 	tEncOptions;
    TWaveHeader 		tWaveHeader;
	
	DWORD 				dwBitRate;
    DWORD 				dwBitConsumption = 0;
	BOOL 				bPow34En;

    FILE                *pfInput;
    FILE                *pfOutput;

    WORD                awInBuff[INPUTFRAMESZ*2]; // stereo.
	BYTE                abyOutBuffer[PBBUFFERSIZE]; // bitstream buffer.

#ifdef __USE_PROFILE__
	HANDLE				hTimerObject;
	TTimerInitOptions	tTimerInitOptions;
	DWORD				dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

	if (argc != 4)
	{
		printf("Usage: aac4enc input_waveform_file\n");
		printf("               output_bitstream_file\n");
		printf("               encode_bit_rate\n");
		exit(1);
	}

	if ((pfInput=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open input waveform file %s fail !!\n", argv[1]);
		exit(1);
	}

	if ((pfOutput=fopen(argv[2], "wb")) == NULL)
	{
		printf("Open output bitstream file %s fail !!\n", argv[2]);
		exit(1);
	}

	dwBitRate = atoi(argv[3]);
	bPow34En = 1;

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

    fread(&tWaveHeader, 1, 44, pfInput);

    if (tWaveHeader.dwDataID != 1635017060)
	{
        printf("The input waveform file format is illegal!\n");
        exit(1);
    }
    else
	{
        tEncInitOptions.dwSampleRate = tWaveHeader.dwSamplesPerSec;
        tEncInitOptions.wChannel = tWaveHeader.wChannels;
    }

	tEncInitOptions.wADTS = 1;			// ADTS format.
    tEncInitOptions.bMpeg4 = 0;			// MPEG-2.
    tEncInitOptions.swStereoMode = -1;	// Auto select.
    tEncInitOptions.swBandWidth = -1;	// Auto select.
    tEncInitOptions.sdwBitRate = dwBitRate;
    tEncInitOptions.bPow34 = bPow34En;
  
    tEncInitOptions.dwFlags = AAC4ENC_NONCIRCULAR_BUFFER;
    tEncInitOptions.dwVersion = AAC4ENC_VERSION;
    tEncInitOptions.pswInBuffer = (SWORD *)awInBuff;
    tEncInitOptions.dwEncBufSize = 0;
    tEncInitOptions.pObjectMem = NULL;
    tEncInitOptions.dwSemaphore = (DWORD)NULL;
                                    
	if (AAC4Enc_Initial(&hEncObject, &tEncInitOptions) != S_OK)
	{
		printf("Initialize MPEG-4 AAC encoder object fail !!\n");
		exit(1);
	}

    tEncOptions.dwVersion = AAC4ENC_VERSION;
    tEncOptions.dwFlags = AAC4ENC_SET_OUTPUTBUFFER;
	tEncOptions.pInterOutBuffer = abyOutBuffer;
    tEncOptions.dwOutBufSize = PBBUFFERSIZE;

    if (AAC4Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
	{
		printf("Set MPEG-4 AAC encoder option fail !!\n");
		exit(1);
	}

#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__

    memset(awInBuff, 0, INPUTFRAMESZ*tEncInitOptions.wChannel*sizeof(WORD));
    if (AAC4Enc_OneFrame(hEncObject, &tEncState) != S_OK)
	{
		printf("Encoding %d frame fail!\n", (int)(tEncState.dwFrameIndx-1));
		exit(1);
	}
	

    while (fread(awInBuff, 1, INPUTFRAMESZ*tEncInitOptions.wChannel*sizeof(WORD), pfInput) == (DWORD)(1024*2*tEncInitOptions.wChannel))
	{
        if (AAC4Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		{
			printf("Set MPEG-4 AAC encoder option fail !!\n");
			exit(1);
		}

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

		if (AAC4Enc_OneFrame(hEncObject, &tEncState) != S_OK)
        {
		    printf("Encoding %d frame fail!\n", (int)(tEncState.dwFrameIndx-1));
		    break;
        }

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

        fwrite(abyOutBuffer, sizeof(BYTE), tEncState.dwBitreamSize, pfOutput);

		dwBitConsumption = dwBitConsumption + tEncState.dwBitreamSize;

#ifndef __USE_PROFILE__
		printf("%5dth frame: %6d bytes (total: %8d bytes).\n", 
			   (int)(tEncState.dwFrameIndx-1),
			   (int)tEncState.dwBitreamSize,
			   (int)dwBitConsumption);
#endif // __USE_PROFILE__

	} 

	if (AAC4Enc_Release(&hEncObject) != S_OK)
	{
		printf("Release MPEG-4 AAC encoder object fail !!\n");
		exit(1);
	}

#ifdef __USE_PROFILE__
	printf("Total cycles in APB/AHB cycle = %ld/%ld ticks\n", dwTotalTicks, dwTotalTicks<<1);
	{
		double dbTemp = tWaveHeader.dwSamplesPerSec;
		dbTemp /= (INPUTFRAMESZ*(tEncState.dwFrameIndx-1));
		printf("Conversion factor: %0.2f.\n", dbTemp);
		dbTemp *= (double)dwTotalTicks * 2.0 / 1000000.0;
		printf("AAC4Enc consumes %0.2f MHz in %s.\n", dbTemp, argv[1]);
	}
	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
	printf("%s generated.\n\n", argv[2]);
#endif //__USE_PROFILE__

    fclose(pfOutput);
    fclose(pfInput);

    return 0;
}

/* =========================================================================================== */

