/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/G726Enc/G726Enc_App/G726Enc_App.c 6     12/09/06 4:55p Angel $
 *
 * Copyright 2007-2013 ______, Inc. All rights reserved.
 *
 * g726enc_app.c : Sample program for G.726 encoder (single-thread)
 *
 * $History: G726Enc_App.c $
 * 
 * *****************  Version 6  *****************
 * User: Angel        Date: 12/09/06   Time: 4:55p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G726Enc/G726Enc_App
 * 
 * *****************  Version 5  *****************
 * User: Yc.lu        Date: 10/11/15   Time: 5:04p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G726Enc/G726Enc_App
 * Add DisplayUsage().
 * 
 * *****************  Version 4  *****************
 * User: Yc.lu        Date: 10/03/03   Time: 9:11a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G726Enc/G726Enc_App
 * Demostrate the use of circular and non-circular (frame-based) bitstream
 * (output) buffer. D?efine __EN_CIRCULAR_BS_BUFFER__ added.
 * 
 * *****************  Version 3  *****************
 * User: Yc.lu        Date: 09/12/15   Time: 11:30a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G726Enc/G726Enc_App
 * do Mozart-3 profiling
 * 
 * *****************  Version 1  *****************
 * User: Yc.lu        Date: 09/10/05   Time: 6:38p
 * Created in $/rd_2/speech/G726/ARM/Encoder/G726Enc/G726Enc_App
 * G726Enc 1.0.0.0 created.
 * 
 * *****************  Version 4  *****************
 * User: Chikuang     Date: 04/05/03   Time: 10:54a
 * Updated in $/rd_2/speech/G726/ARM/G726_ARM/enc_app
 * 
 * *****************  Version 3  *****************
 * User: Chikuang     Date: 03/01/07   Time: 11:08a
 * Updated in $/rd_2/speech/G726/ARM/G726_ARM/enc_app
 * QueryMemSize bug fixed (due to new GB/PB engine)
 * 
 * *****************  Version 2  *****************
 * User: Chikuang     Date: 02/12/03   Time: 9:38p
 * Updated in $/rd_2/speech/G726/ARM/G726_ARM/enc_app
 * add "bFrameBased" option to support new GB/PB engine
 * 
 * *****************  Version 1  *****************
 * User: Chikuang     Date: 02/11/19   Time: 6:47p
 * Created in $/rd_2/speech/G726/ARM/G726_ARM/enc_app
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "G726Enc.h"

// Enable circular bitstream buffer, disable define when adopting frame-based
//#define __EN_CIRCULAR_BS_BUFFER__ 

#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

#define BSBUFFERSIZE	1024
#define IPPACKETSIZE	 256

// ============================================================================
void SoCOPCycle(char	*pcSoCModule, // (i) "Mozart" or "Haydn"
				int		iFrameSz, // (i) in sample
				int		iFrameCnt, // (i) in frame
				int		iSampleRate, // (i) in sample
				int		iAPBClk, // (i) in Hz
				double	*pdbAHBOPCycle, // (o)
				double	*pdbCPUOPCycle) // (o)
{
	double dbRatioCPU2AHB = 0.0;

	// Mozart CPU/AHB/APB: 500/166/83 MHz
	if (strstr(pcSoCModule,"Mozart") != NULL) 
	{
		dbRatioCPU2AHB = 3.0;	
	}
	// Haydn CPU/AHB/APB: 266/133/67 MHz
	else if (strstr(pcSoCModule,"Haydn") != NULL) 
	{
		dbRatioCPU2AHB = 2.0;
	}
	else
	{
		printf("SoCOPCycle: [error] wrong SoC module.");
		return;
	}

	double dbOPCycle = (double)iAPBClk * 2.0 / 1000000.0;	// AHBCLK in 10^-6
	dbOPCycle *= iSampleRate;
	dbOPCycle /= iFrameSz * iFrameCnt;

	(*pdbAHBOPCycle) = dbOPCycle;
	(*pdbCPUOPCycle) = dbOPCycle * dbRatioCPU2AHB;

	return;
}

/* ------------------------------------------------------------------------ */
void DisplayUsage(int argc, char *argv[])
{
	printf("Usage: %s input-file output-file output-bit-rate input-format input-block-size\n", argv[0]);
	printf("	input-file: input PCM file w/ data sampled in 8 KHz.\n");
	printf("	output-file: output bitstream file.\n");
	printf("	output-bit-rate: 16 kbps (2), 24 kbps (3), 32 kbps (4), and 40 kbps (5).\n");
	printf("	input-format: linear PCM (0), u-Law (1), and A-Law (2).\n");
	printf("	input-block-size: input PCM block size in sample for one frame process.\n");
}

// ============================================================================
int main(int argc, char* argv[])
{
    TG726EncInitOptions g726_enc_init_options;
    TG726EncOptions     g726_enc_options;
	TG726EncState	    EncState;
	HANDLE				hG726EncObject;
	int					iCount=0;
	FILE				*fpIn;
	FILE				*fpOut;
    BYTE                *pbyInBuffer;
    BYTE                abyOutBuffer[IPPACKETSIZE];
    DWORD               dwObjectSize;
#ifdef __EN_CIRCULAR_BS_BUFFER__
    DWORD               dwResidue;
#endif // __EN_CIRCULAR_BS_BUFFER__
    char                pszInFile[255];
    char				pszOutFile[255];
    char				pszTmp[80];
    SDWORD              swBitRate;
    DWORD               dwMode,dwBlockSize;
    BOOL                bLastBlock = FALSE;
    BYTE                *pbyInternalBuffer;
#ifdef __USE_PROFILE__
	HANDLE 				hTimerObject;
	TTimerInitOptions 	tTimerInitOptions;
	DWORD 				dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__    

    if (argc == 6)
	{
		strcpy(pszInFile, argv[1]);
		strcpy(pszOutFile, argv[2]);
		swBitRate = atoi(argv[3]);
        dwMode = atoi(argv[4]);
		dwBlockSize = atoi(argv[5]);				
	}
	else if (argc == 1)
	{
		if ((fpIn=fopen("g726enc.ctl", "r")) == NULL)
		{
			printf("Error opening control file g726enc.ctl\n");
			DisplayUsage(argc, argv);
			return S_FAIL;
		}
    
		fgets(pszInFile, 80, fpIn);
		fgets(pszOutFile, 80, fpIn);
		if (pszInFile[strlen(pszInFile)-1] == '\n')
		{
    		pszInFile[strlen(pszInFile)-1] = '\0';
		}
		if (pszOutFile[strlen(pszOutFile)-1]=='\n')
		{
    		pszOutFile[strlen(pszOutFile)-1]='\0';
		}
		fgets(pszTmp, 80, fpIn);
		swBitRate = atoi(pszTmp);
		fgets(pszTmp, 80, fpIn);
		dwMode = atoi(pszTmp);
		fgets(pszTmp, 80, fpIn);
		dwBlockSize = atoi(pszTmp);
		fclose(fpIn);

	}
	else
	{
		DisplayUsage(argc, argv);
		return S_FAIL;
	}

	// ------------------------------------------------------------------------
    if ((fpIn=fopen(pszInFile,"rb")) == NULL)
    {
        printf("Error opening input file %s.\n", pszInFile);
        return S_FAIL;
    }
    
    if ((fpOut=fopen(pszOutFile,"wb")) == NULL)
    {
        printf("Error opening output file %s\n",pszOutFile);
        return S_FAIL;        
    }
    
    pbyInBuffer = (BYTE *)malloc(dwBlockSize*(1<<(dwMode==0)));
    g726_enc_init_options.pInBuffer = pbyInBuffer;
    g726_enc_init_options.dwVersion = G726ENC_VERSION;
#ifdef __EN_CIRCULAR_BS_BUFFER__
    g726_enc_init_options.dwBufferSize = BSBUFFERSIZE;
	g726_enc_init_options.bFrameBased = FALSE;    
#else // __EN_CIRCULAR_BS_BUFFER__
	g726_enc_init_options.dwBufferSize = 0;
	g726_enc_init_options.bFrameBased = TRUE;    
#endif // __EN_CIRCULAR_BS_BUFFER__
    g726_enc_init_options.dwSemaphore = (DWORD)NULL;
    g726_enc_init_options.dwUserData = 1;
    g726_enc_init_options.pfnSend = NULL;
    g726_enc_init_options.pfnEnter = NULL;
    g726_enc_init_options.pfnLeave = NULL;
    g726_enc_init_options.pObjectMem = NULL;
    g726_enc_init_options.dwBlockSize = dwBlockSize;
    g726_enc_init_options.dwMode = dwMode;
    g726_enc_init_options.swBitRate = swBitRate;
    dwObjectSize = G726Enc_QueryMemSize(&g726_enc_init_options);
    pbyInternalBuffer = malloc(dwObjectSize);
    printf("One instance created. (%d bytes)\n", (int)dwObjectSize);
    g726_enc_init_options.pObjectMem = pbyInternalBuffer;
    g726_enc_options.dwVersion = G726ENC_VERSION;
    g726_enc_options.dwBlockSize = dwBlockSize;
    g726_enc_options.dwMode = dwMode;
    g726_enc_options.swBitRate = swBitRate;
    g726_enc_options.pInBuffer = NULL;
    g726_enc_options.pOutBuffer = NULL;

    if (G726Enc_Initial(&hG726EncObject, &g726_enc_init_options) != S_OK)
	{
		printf("Fail to initialize G.726 Encoder!\r\n");
		return S_FAIL;
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
    
#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__
	
	do
	{	
        DWORD dwInSize = fread(pbyInBuffer, 1<<(dwMode==0), dwBlockSize, fpIn);
		//printf("--------- Encoding frame\t#%d ----------.\n", iCount);
		bLastBlock = (dwInSize!=dwBlockSize) ? TRUE : FALSE;

        g726_enc_options.dwBlockSize = dwInSize;
#ifndef __EN_CIRCULAR_BS_BUFFER__
		g726_enc_options.pOutBuffer = abyOutBuffer;
		g726_enc_options.pInBuffer = pbyInBuffer;
		g726_enc_options.dwBufSize = dwInSize;
#endif // __EN_CIRCULAR_BS_BUFFER__
        if (G726Enc_SetOptions(hG726EncObject, &g726_enc_options) != S_OK)
			return S_FAIL;
                
#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
        
		// encode one frame
		if(G726Enc_OneFrame(hG726EncObject, &EncState) != S_OK)	break;

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

#ifdef __EN_CIRCULAR_BS_BUFFER__
		// activating file write if half output buffer is filled up.
        while (G726Enc_QueryFilledBufferSpace(hG726EncObject) >= (BSBUFFERSIZE>>1))
        {
            G726Enc_TakeData(hG726EncObject, abyOutBuffer, IPPACKETSIZE);
			fwrite(abyOutBuffer, sizeof(BYTE), IPPACKETSIZE, fpOut);
        }
#else // __EN_CIRCULAR_BS_BUFFER__
		fwrite(abyOutBuffer, sizeof(BYTE), EncState.dwOutSize, fpOut);
#endif // __EN_CIRCULAR_BS_BUFFER__
        
		iCount++;
		printf("Frame %d encoded (%d bytes). \n", iCount, EncState.dwOutSize);
        if (bLastBlock) break;               
        
	} while(1);//(iCount<157);//while(1);
	
#ifdef __EN_CIRCULAR_BS_BUFFER__
    dwResidue = G726Enc_QueryFilledBufferSpace(hG726EncObject);
    while (dwResidue >= IPPACKETSIZE)
    {
        G726Enc_TakeData(hG726EncObject,abyOutBuffer, IPPACKETSIZE);
        fwrite(abyOutBuffer, sizeof(BYTE), IPPACKETSIZE, fpOut);
        dwResidue = dwResidue - IPPACKETSIZE;
    }
    G726Enc_TakeData(hG726EncObject, abyOutBuffer, dwResidue);
    fwrite(abyOutBuffer, sizeof(BYTE), dwResidue, fpOut);
#endif // __EN_CIRCULAR_BS_BUFFER__
#ifdef __USE_PROFILE__

	{
		double dbAHBOPCycle, dbCPUOPCycle;

		SoCOPCycle("Mozart",
			       dwBlockSize,
				   iCount,
				   8000,
				   (int)dwTotalTicks,
				   &dbAHBOPCycle,
				   &dbCPUOPCycle);
		
		printf("G726Enc consumed %0.2f MHz (APBCLK:%d) in %s.\n", dbAHBOPCycle, (int)dwTotalTicks, pszOutFile);
	}
	

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

	// free InBuffer

	free(pbyInBuffer);
	G726Enc_Release(&hG726EncObject);
	fclose(fpIn);
	fclose(fpOut);

    return S_OK;
}
