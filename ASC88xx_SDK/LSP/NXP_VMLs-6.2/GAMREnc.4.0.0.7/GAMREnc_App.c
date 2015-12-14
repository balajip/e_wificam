/*
 * $Header: /rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App/GAMREnc_App.c 2     10/11/08 10:56a Yc.lu $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * gamrenc_app.c :
 *
 *  GSM AMR speech encoder sample code
 *
 * $History: GAMREnc_App.c $
 * 
 * *****************  Version 2  *****************
 * User: Yc.lu        Date: 10/11/08   Time: 10:56a
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 1  *****************
 * User: Angel        Date: 09/12/11   Time: 4:22p
 * Created in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 7  *****************
 * User: Angel        Date: 09/02/27   Time: 11:58a
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 6  *****************
 * User: Angel        Date: 09/01/08   Time: 1:43p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 5  *****************
 * User: Angel        Date: 08/01/22   Time: 3:27p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 4  *****************
 * User: Angel        Date: 08/01/22   Time: 2:35p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 3  *****************
 * User: Angel        Date: 08/01/03   Time: 7:54p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMREnc_App
 * 
 * *****************  Version 2  *****************
 * User: Aren         Date: 03/05/16   Time: 6:48p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRENC_APP
 * FEATURE: For version 2.0.0.0
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/03/17   Time: 4:26p
 * Created in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRENC_APP
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/02/19   Time: 8:53p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/GAMREnc_App
 * New Version
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 02/05/07   Time: 2:11p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/enc_app
 *
 *
 */

/* ------------------------------------------------------------------------ */
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "amrctrlfile.h"
#include "amrmodefile.h"
#include "GAMREnc.h"

#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* ------------------------------------------------------------------------ */
typedef struct {
	BYTE byMajor;
	BYTE byMinor;
	BYTE byBuild;
	BYTE byRevision;
} TVERSION;

/* ------------------------------------------------------------------------ */
SCODE DLLAPI Str2Mode(const char* str, DWORD *dwMode);

/* ------------------------------------------------------------------------ */
#define SIZE_SPEECH_IN	320

/* ------------------------------------------------------------------------ */
void DisplayOptions(TAMRCtrlFilePublic *pamrcf)
{
    assert(pamrcf);
    DBSTR2("\r\n\r\n---- GSM-AMR Encoder Build:%s, %s ----\r\n", __DATE__, __TIME__);
    DBSTR1("Input file name  = %s\r\n", pamrcf->pszInFileName);
    DBSTR1("Output file name = %s\r\n", pamrcf->pszOutFileName);
    DBSTR1("Enable DTX       = %s\r\n", pamrcf->bEnableDTX ? "ON" : "OFF");
    DBSTR1("Output format    = %s\r\n", pamrcf->iOutputFormat == 0 ? "G.192 Serial" : "Packed");

    if (pamrcf->bUseModeFile == 1)
    {
        DBSTR1("Mode rate file   = %s\r\n", pamrcf->pszModeRateFile);
    }
    else if (pamrcf->bUseModeFile == 0)
    {
        DBSTR1("Mode rate        = %s\r\n", pamrcf->pszModeRate);
    }

    DBSTR1("VAD Option       = %d\r\n", pamrcf->iVadOption);
    DBSTR0("-----------------------------------------------------\r\n");
}

/* ------------------------------------------------------------------------ */
void DisplayUsage(int argc, char *argv[])
{
	printf("Usage: %s input-file output-file use-DTX VAD-moode output-format\n", argv[0]);
	printf("	input-file: input PCM file w/ data sampled in 8 KHz\n");
	printf("	output-file: output file either in packed bitstream or G.192 format (depending on \"output-format\").\n");
	printf("	use-DTX: enable discontinuous transmission (1) or not (0).\n");
	printf("	VAD-mode: voice activity detection (VAD) algorithm, the first (1) or the second (2).\n");
	printf("	output-format: packed bitstream (1) or G.192 (0) format\n");
}

/* ------------------------------------------------------------------------ */
int main (int argc, char *argv[])
{
    FILE				*pfIn = NULL; 
    FILE				*pfOut = NULL;
    FILE				*pfComp = NULL;
    HANDLE				hAMRCtrlFile = NULL, hAMRModeFile = NULL;
    HANDLE 				hGAMREnc = NULL;
    TAMRCtrlFilePublic 	*pamrcf = NULL;
    BOOL    			bUseModeFile = 0;
    TGAMREncInitOptions tGAMREncInitOptions;
    TGAMREncOptions     tGAMREncOpt;
    TGAMREncState       tGAMREncState;
	BOOL 				bUseCircularBuf = TRUE;	// initial setting, may be change in following.
	BOOL 				bUseExternalMem = FALSE;	// initial setting, may be change in following.
	void 				*pObjectMem = NULL;
	TVERSION 			tVersion;
	BOOL 				bShowVersion = FALSE;		// initial setting, may be change in following.
    const DWORD    		dwInSize = SIZE_SPEECH_IN;
    DWORD 				dwMode;
    DWORD 				dwOutSize;
    PVOID 				pvSpeech = malloc(dwInSize);
    PVOID 				pvOutput = malloc(500+12);
    PVOID 				pvCompbuf = NULL;
    DWORD 				dwFrameCount = 0;

#ifdef __USE_PROFILE__
	HANDLE 				hTimerObject;
	TTimerInitOptions 	tTimerInitOptions;
	DWORD 				dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

    DBOPENFILE("EncDBOut.txt");
    OPENPROFILE("EncProfile.txt");

	/* You can show the version or not */
	bShowVersion = FALSE;
	if (bShowVersion)
	{
		GAMREnc_GetVersionInfo(&tVersion.byMajor,
							   &tVersion.byMinor,
							   &tVersion.byBuild,
							   &tVersion.byRevision);
		printf("GAMREnc version: %d.%d.%d.%d\n",
			   tVersion.byMajor,
			   tVersion.byMinor,
			   tVersion.byBuild,
			   tVersion.byRevision);
	}

    assert(pvSpeech);
    assert(pvOutput);
    memset(pvOutput, 0, 500);

	if (argc == 1)
	{
		// initialize control file object, and read
		if (AMRCtrlFile_Init(&hAMRCtrlFile, "amr_encoder.ctl") != S_OK)
		{
			DBSTR0("Error open control file 'amr_encoder.ctl'");
			DisplayUsage(argc, argv);
			return 1;
		}
		pamrcf = (TAMRCtrlFilePublic *)AMRCtrlFile_GetPubVar(hAMRCtrlFile);
	}
	else if (argc == 6)
	{
		pamrcf = malloc(sizeof(TAMRCtrlFilePublic));
		strcpy(pamrcf->pszInFileName, argv[1]);
		strcpy(pamrcf->pszOutFileName, argv[2]);
		pamrcf->bEnableDTX = atoi(argv[3]);
        pamrcf->iVadOption = atoi(argv[4]);
		pamrcf->iOutputFormat = atoi(argv[5]);		
		pamrcf->bUseModeFile = 0;
		pamrcf->bUseCompFile = 0;		
	}
	else
	{
		DisplayUsage(argc, argv);
		return 1;
	}

    // display control options
    DisplayOptions(pamrcf);
	

    // initialize mode file object
    bUseModeFile = pamrcf->bUseModeFile;
    if(bUseModeFile == 1)
    {
        if(GAMRModeFile_Init(&hAMRModeFile, pamrcf->pszModeRateFile) != S_OK)
        {
            DBSTR1("Error open mode file %s\r\n", pamrcf->pszModeRateFile);
            AMRCtrlFile_Release(&hAMRCtrlFile);
            return 1;
        }
    }
    else if(bUseModeFile == -1)
    {
        DBSTR0("Please specify either mode rate or mode rate file in control file!\r\n");
        AMRCtrlFile_Release(&hAMRCtrlFile);
        return 1;
    }

    // open input & output file
    if((pfIn = fopen(pamrcf->pszInFileName, "rb")) == NULL)
    {
        DBSTR1("Error open input file %s\r\n", pamrcf->pszInFileName);
        if (hAMRModeFile)
		{
			GAMRModeFile_Release(&hAMRModeFile);
		}
        AMRCtrlFile_Release(&hAMRCtrlFile);
        return 1;
    }

    if((pfOut = fopen(pamrcf->pszOutFileName, "wb")) == NULL)
    {
        DBSTR1("Error open output file %s\r\n", pamrcf->pszOutFileName);
        if (hAMRModeFile)
		{
			GAMRModeFile_Release(&hAMRModeFile);
		}
        AMRCtrlFile_Release(&hAMRCtrlFile);
        fclose(pfIn);
        return 1;
    }

	/* if compliance file is used, initialize related buffers */
	if (pamrcf->bUseCompFile)
	{
		printf("%s  ",pamrcf->pszCompFileName);
		if((pfComp = fopen(pamrcf->pszCompFileName, "rb")) == NULL)
		{
			DBSTR1("Error open compliance file %s\n", pamrcf->pszCompFileName);
			if (hAMRModeFile)
			{
				GAMRModeFile_Release(&hAMRModeFile);
			}
			AMRCtrlFile_Release(&hAMRCtrlFile);
			fclose(pfIn);
			fclose(pfOut);
			exit(0);
		}
		pvCompbuf = malloc(500);
		assert(pvCompbuf);
		memset(pvCompbuf, 0, 500);
	}

    // initialize GAMR encoder
    memset(&tGAMREncInitOptions, 0, sizeof(tGAMREncInitOptions));

    Str2Mode(pamrcf->pszModeRate, &dwMode);

	/* You can select buffer type */
	bUseCircularBuf = FALSE;

    // set initial options
    tGAMREncInitOptions.dwVersion				= GAMRENC_VERSION;
    tGAMREncInitOptions.pObjectMem				= NULL;
    tGAMREncInitOptions.dwFlags					= GAMRENC_DTX |
												  GAMRENC_VAD |
												  GAMRENC_MODE |
												  GAMRENC_OUTPUTFORMAT |
												  GAMRENC_INPUT_BUFFER;
	tGAMREncInitOptions.pfnOnEnterCriticalSec	= NULL;
	tGAMREncInitOptions.pfnOnLeaveCriticalSec	= NULL;
	tGAMREncInitOptions.pfnOnSendData			= NULL;
	tGAMREncInitOptions.dwSemaphore				= (DWORD)NULL;
	tGAMREncInitOptions.dwUserData				= (DWORD)NULL;
	tGAMREncInitOptions.bUseCircularBuf			= bUseCircularBuf;
	tGAMREncInitOptions.dwBufferSize			= (bUseCircularBuf) ? 512 : 0;
	tGAMREncInitOptions.bEnableDTX				= pamrcf->bEnableDTX;
    tGAMREncInitOptions.dwVADOption				= pamrcf->iVadOption;
    tGAMREncInitOptions.dwOutputFormat			= pamrcf->iOutputFormat;
	tGAMREncInitOptions.eMode					= (EGAMREncModeRate)dwMode;
	tGAMREncInitOptions.pvSpeechIn				= pvSpeech;

	/* You can select the object memory created externally or not. */
	bUseExternalMem = FALSE;
	if (bUseExternalMem)
	{
		DWORD dwObjMemSize;

		dwObjMemSize = GAMREnc_QueryMemSize(&tGAMREncInitOptions);
		pObjectMem = (void *)malloc(dwObjMemSize);
		if (pObjectMem == NULL)
		{
			printf("malloc error!\n");
			exit(1);
		}

		/* To test non-align			*
		 * pObjectMem + 0~3	*/
		tGAMREncInitOptions.pObjectMem = (void *)((BYTE *)pObjectMem + 0);
	}
	
	if(GAMREnc_Initial(&hGAMREnc, &tGAMREncInitOptions)!=S_OK)
	{
		DBSTR0("Error GAMREnc_Initial \n");
		exit(1);
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
	dwTotalTicks = 0;
    dwFrameCount = 0;
#endif //__USE_PROFILE__

    memset(&tGAMREncOpt, 0, sizeof(tGAMREncOpt));
    tGAMREncOpt.dwVersion = GAMRENC_VERSION;

    // start encoding loop
    while(1)
    {
        DBSTR0("AMR-Encode ");
        DBSTR1("Frame #%3d: ", dwFrameCount);
        dwFrameCount++;
        if(bUseModeFile)
        {
            if(GAMRModeFile_ReadMode(hAMRModeFile, &dwMode) != S_OK)
            {
                DBSTR0("End of mode rate file!\r\n");
                break;
            }
			tGAMREncOpt.dwFlags = GAMRENC_MODE;
			tGAMREncOpt.eMode = (EGAMREncModeRate)dwMode;
            GAMREnc_SetOptions(hGAMREnc, &tGAMREncOpt);
        }

        // read new input speech
        if (fread(pvSpeech, 1, dwInSize, pfIn) != dwInSize)
        {
            break;
        }

		if (!bUseCircularBuf)
		{
			tGAMREncOpt.dwFlags = GAMRENC_OUTPUT_BUFFER;
			tGAMREncOpt.pvOutBuffer = pvOutput;
			tGAMREncOpt.dwOutBufferSize = 512;
            GAMREnc_SetOptions(hGAMREnc, &tGAMREncOpt);
		}

        // encode one frame
#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

        GAMREnc_ProcessOneFrame(hGAMREnc, &tGAMREncState);

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

		dwOutSize = GAMREnc_QueryFilledBufferSpace(hGAMREnc);
		if (bUseCircularBuf)
		{
			/* Get output bitstream */
			GAMREnc_TakeData(hGAMREnc, pvOutput, dwOutSize);
		}

        // write out stream
        fwrite(pvOutput, 1, dwOutSize, pfOut);

		if (pamrcf->bUseCompFile)
		{
			fread(pvCompbuf, 1, dwOutSize, pfComp);
			if (memcmp(pvOutput, pvCompbuf, dwOutSize) != 0)
			{
				printf("not compliant!\n");
				break;
			}
		}
    }

#ifdef __USE_PROFILE__
	printf("Total cycles in APB unit = %d\n", dwTotalTicks);
	printf("MHz @ 8000fs = %3.2fMHz\n", ((float)dwTotalTicks*2)*8000/((float)(160*dwFrameCount)*1000000));
	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

    // free memory
    free(pvSpeech);
    free(pvOutput);

    // close input/output files
    fclose(pfIn);
	fclose(pfOut);

	if (pamrcf->bUseCompFile)
		fclose(pfComp);

    // release GAMR encoder
    if (hGAMREnc)
		GAMREnc_Release(&hGAMREnc);
	
	if (bUseExternalMem && pObjectMem!=NULL)
		free(pObjectMem);
	
    // release mode file object
    if (hAMRModeFile)
		GAMRModeFile_Release(&hAMRModeFile);
	
    // release control file object
    if (hAMRCtrlFile)
		AMRCtrlFile_Release(&hAMRCtrlFile);
	
	if ((argc!=1) && (pamrcf!=NULL))
		free(pamrcf);

    DBCLOSEFILE();
    CLOSEPROFILE();

    return 0;
}
//---------------------------------------------------------------------------


