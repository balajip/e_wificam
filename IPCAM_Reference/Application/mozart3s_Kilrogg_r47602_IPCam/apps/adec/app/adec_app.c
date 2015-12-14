/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2007-2010 VN Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 *
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2007-2010 VN, Inc. All rights reserved.
 *
 * \file
 * adec_app.c
 *
 * \brief
 * Audio decoder application interface implement file.
 *
 * \date
 * 2008/3/19
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include "adec_local.h"
#include "adec_init.h"
#include "adec_config.h"
#include "adec_output.h"
#include "ubuffer.h"

SCODE InitServerSocket(TAdecInfo *ptAdecInfo);
BYTE* Adec_MonoToStereo(BYTE *pbyMonoIn, BYTE *pbyStereoOut, DWORD* pdwSize, DWORD dwChNo);
BYTE* adecStereoUpSampleAudioBuffer(BYTE *pbyStereoBaseIn, BYTE *pbyUpSampleOut, DWORD* pdwSize, DWORD dwTwiceRatio);
/* ========================================================================== */
SCODE AdecApp_Initial(HANDLE *phObject, TAdecInitOpt tAdecInitOpt)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)malloc(sizeof(TAdecInfo));
    if (ptAdecInfo == NULL)
    {
        return ERR_OUT_OF_MEMORY;
    }
    memset(ptAdecInfo, 0, sizeof(TAdecInfo));
    *phObject = (HANDLE) ptAdecInfo;

    if (!tAdecInitOpt.szConfigFile)
    {
        DBPRINT0("[ADEC] Empty configuration file path !!\n");
        return S_FAIL;
    }
    else
    {
        ptAdecInfo->szConfigFile = tAdecInitOpt.szConfigFile;
        if (access(ptAdecInfo->szConfigFile, F_OK) != 0)
        {
            DBPRINT1("[ADEC] Configuration file %s not exist!!\n", ptAdecInfo->szConfigFile);
            return S_FAIL;
        }
        DBPRINT1("[ADEC] Configuration file %s !!\n", ptAdecInfo->szConfigFile);
    }
    
   	if (sem_init(&(ptAdecInfo->smOptions), 0, 1) != 0) {
    	return S_FAIL;
	}

	ptAdecInfo->bReConf_flag = FALSE;
	ptAdecInfo->bTerminal	= FALSE;

    /* Initial Configuration handler */
    if (Adec_InitConfigHandler(ptAdecInfo) != S_OK)
    {
        DBPRINT0("[ADEC] Initial Configuration handler fail !!\n");
        return S_FAIL;
    }

    ptAdecInfo->pbyInBuff = (BYTE *)malloc(ADEC_UBUFFER_SIZE);
    if (ptAdecInfo->pbyInBuff == NULL)
    {
        DBPRINT0("[ADEC] Allocate input buffer fail !!\n");
        return ERR_OUT_OF_MEMORY;
    }

    ptAdecInfo->pbyOutFrame = (BYTE *)malloc(MAX_AUDIOIN_BUFSIZE);
    if (ptAdecInfo->pbyOutFrame == NULL)
    {
        DBPRINT0("[ADEC] Allocate output frame buffer fail !!\n");
        return ERR_OUT_OF_MEMORY;
    }

    ptAdecInfo->pbyStereoOutFrame = (BYTE *)malloc(MAX_AUDIOIN_BUFSIZE);
    if (ptAdecInfo->pbyStereoOutFrame == NULL)
    {
        DBPRINT0("[ADEC] Allocate stereo frame buffer fail !!\n");
        return ERR_OUT_OF_MEMORY;
    }

    if (Adec_InitAudioOut(ptAdecInfo) != S_OK)
    {
        return S_FAIL;
    }

    if (InitServerSocket(ptAdecInfo) != S_OK)
    {
        return S_FAIL;
    }

    if (Adec_InitG711Dec(&ptAdecInfo->hG711DecObjALAW, "A_LAW") != S_OK)
    {
        return S_FAIL;
    }

    if (Adec_InitG711Dec(&ptAdecInfo->hG711DecObjULAW, "U_LAW") != S_OK)
    {
        return S_FAIL;
    }

//    Adec_LoadConfig(pAdecInfo);
    return S_OK;
}

/* ========================================================================== */
SCODE AdecApp_Release(HANDLE *phObject)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)(*phObject);

	if (ptAdecInfo != NULL) {
		sem_destroy(&(ptAdecInfo->smOptions));

		// release VML
		if (ptAdecInfo->hG711DecObjALAW != NULL)
			G711Dec_Release(&(ptAdecInfo->hG711DecObjALAW));

		if (ptAdecInfo->hG711DecObjULAW != NULL)
			G711Dec_Release(&(ptAdecInfo->hG711DecObjULAW));

		// free memory
		if (ptAdecInfo->pbyInBuff != NULL)
			free(ptAdecInfo->pbyInBuff);

		if (ptAdecInfo->pbyOutFrame != NULL)
			free(ptAdecInfo->pbyOutFrame);

		if (ptAdecInfo->pbyStereoOutFrame != NULL)
			free(ptAdecInfo->pbyStereoOutFrame);
				
		free(ptAdecInfo);
		*phObject = NULL;
	}

    return S_OK;
}

/* ============================================================================================= */
SCODE Adec_ResetConfigFlag(HANDLE hObject)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)(hObject);

    sem_wait(&(ptAdecInfo->smOptions));
	ptAdecInfo->bReConf_flag = TRUE;
    sem_post(&(ptAdecInfo->smOptions));
    return S_OK;
}

/* ============================================================================================= */
SCODE Adec_ResetTerminalFlag(HANDLE hObject)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)(hObject);

	ptAdecInfo->bTerminal = TRUE;
    return S_OK;
}

/* ========================================================================== */
void Adec_SampleRate(HANDLE hObject, const CHAR *szParam, SDWORD len)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)hObject;
    DWORD dwSampleRate = strtoul(szParam, NULL, 10);

	if (dwSampleRate != ptAdecInfo->dwSampleRate) {
		printf("sample change from %d to %d\n", ptAdecInfo->dwSampleRate, dwSampleRate);	
		ptAdecInfo->dwSampleRate = dwSampleRate;
		Adec_ResetAudioOut(ptAdecInfo);	
	}
}

/* ============================================================================================= */
SCODE Adec_LoadConfig(HANDLE hObject)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)(hObject);
    HANDLE	hXmlWrapperObj;
    
    TXmlWrapperInitOptions tInitOptions;
    TXmlWrapperTreeMap ptTreeMap[] = 
    {
        {"/root/adec/sample_rate", NULL, &Adec_SampleRate, NULL},
        {NULL, NULL, NULL}
    };
    
    tInitOptions.dwVersion = XMLWRAPPER_VERSION;

    if (XmlWrapper_Initial(&hXmlWrapperObj, &tInitOptions) != S_OK)
    {
        DBPRINT0("[ADEC] Initial XmlWrapper object fail! \n");
        return S_FAIL;
    }
    // Set tree map
    if ( XmlWrapper_SetHandler(hXmlWrapperObj, ptTreeMap,NULL) != S_OK)
    {
        DBPRINT0("[ADEC] Set XmlWrapper Handler fail! \n");
        return S_FAIL;
    }
    // Load configuration
    if (XmlWrapper_ReadFile_UsrDefFunc(ptAdecInfo->szConfigFile, hXmlWrapperObj, ptAdecInfo) != S_OK)
    {
        DBPRINT1("[ADEC][%s] XmlWrapper read config fail! \n", __FUNCTION__);
        return S_FAIL;
    }

	XmlWrapper_Release(&hXmlWrapperObj);
    return S_OK;    
}

/* ========================================================================== */
SCODE AdecApp_Process_Loop(HANDLE hObject)
{
    TAdecInfo *ptAdecInfo = (TAdecInfo *)hObject;
  	TG711DecState tG711DecState;
    HANDLE hG711Dec;
    DWORD dwSize, dwInSize, dwOutSize;
    fd_set readfds;
    int nfds;
    struct  timeval timeout;
    TUBuffer *ptUBuff = (TUBuffer *)ptAdecInfo->pbyInBuff;
    TUBufferConfG711 *ptG711UBufConf = NULL;
    BOOL bHaveClient = FALSE;
    DWORD dwTimeOutCounter = 0;
   	DWORD dwDoubleFreqRatio = 2;
   	BYTE byUpSampleBuffer[16000];

#ifdef _DUMP_FILE
    FILE *pfOutPut;
    if ((pfOutPut =  fopen("test/audio.pcm", "wb")) == NULL)
    {
        DBPRINT0("Open dumped file error!!\n");
        exit(1);
    }
#endif    
    while(1)
    {
    	if (ptAdecInfo->bTerminal == TRUE) break;
    		
		if (ptAdecInfo->bReConf_flag == TRUE) {
			sem_wait(&(ptAdecInfo->smOptions));
			Adec_LoadConfig(hObject);
			ptAdecInfo->bReConf_flag = FALSE;
			sem_post(&(ptAdecInfo->smOptions));
		}

        timeout.tv_sec = 0;
        timeout.tv_usec = 70000;
        FD_ZERO(&readfds);
        FD_SET(ptAdecInfo->ifdUSock, &readfds);
        
        if (bHaveClient)
        	nfds = select(ptAdecInfo->ifdUSock+1, &readfds, NULL, NULL, &timeout);
        else
        	nfds = select(ptAdecInfo->ifdUSock+1, &readfds, NULL, NULL, NULL);

        if (nfds == 0)
        {
			dwTimeOutCounter++;
			if (dwTimeOutCounter > MAX_TIMEOUT_NUM)
				bHaveClient = FALSE;
            continue;
        }
        if (nfds < 0)
        {
        	continue;		
        }

        if (FD_ISSET(ptAdecInfo->ifdUSock, &readfds))
        {
        	bHaveClient = TRUE;
        	dwTimeOutCounter = 0;
            if ((dwSize = recv(ptAdecInfo->ifdUSock, ptAdecInfo->pbyInBuff, ADEC_UBUFFER_SIZE, 0)) > 0)
            {
                if (ptUBuff->dwDataType == FOURCC_CONF)
                {
                    if (ptUBuff->dwFollowingDataType == FOURCC_G711)
                    {
                        ptG711UBufConf = (TUBufferConfG711 *)ptAdecInfo->pbyInBuff;
                        if ((ptG711UBufConf->dwCompressionLaw == FOURCC_ALAW) ||
                            (ptG711UBufConf->dwCompressionLaw == FOURCC_ULAW))
                        {
                            ptAdecInfo->dwCompressionLaw = ptG711UBufConf->dwCompressionLaw;
                            ptAdecInfo->dwChannelNumber = ptG711UBufConf->dwChannelNumber;
                            DBPRINT2("[ADEC] Received CI of G.711 with %s_LAW, %u channel\n",
                                   (ptAdecInfo->dwCompressionLaw == FOURCC_ALAW)?"A":"U",
                                   ptAdecInfo->dwChannelNumber);
                        }
                        else
                        {
                            DBPRINT0("[ADEC] Wrong G.711 compression mode\n");
                        }
                    }
                    else
                    {
                        DBPRINT0("[ADEC] Unknown Ubuffer conf type\n");
                    }
                }
                else if (ptUBuff->dwDataType == FOURCC_G711)
                {
					dwDoubleFreqRatio = (DWORD)(((FLOAT)ptAdecInfo->dwSampleRate / 8000) * 2);

                    dwInSize = ptUBuff->dwSize - sizeof(TUBuffer) - ptUBuff->dwUserDataSize;
                    dwOutSize = dwInSize * 2;
                    //DBPRINT1("[ADEC] Received G.711 raw data Ubuffer %ld\n", dwInSize);
                    tG711DecState.pbyInFrame = ptAdecInfo->pbyInBuff + sizeof(TUBuffer) + ptUBuff->dwUserDataSize;
                    tG711DecState.pswOutFrame = (SWORD *)ptAdecInfo->pbyOutFrame;
                    if (ptAdecInfo->dwCompressionLaw == FOURCC_ALAW)
                    {
                        hG711Dec = ptAdecInfo->hG711DecObjALAW;
                    }
                    else if(ptAdecInfo->dwCompressionLaw == FOURCC_ULAW)
                    {
                        hG711Dec = ptAdecInfo->hG711DecObjULAW;
                    }
                    else
                    {
                        DBPRINT0("[ADEC] G711Dec invalid mode \n");
                        continue;
                    }

                    if (G711Dec_ProcessOneFrame(hG711Dec, &tG711DecState) != S_OK)
                    {
                        DBPRINT0("[ADEC] G711Dec decoded error \n");
                        continue;
                    }
                   	Adec_MonoToStereo(ptAdecInfo->pbyOutFrame, ptAdecInfo->pbyStereoOutFrame, &dwOutSize, ptAdecInfo->dwChannelNumber);
                   	if (dwDoubleFreqRatio == 2) { // it means the same sample rate, it needn't upsample
	                   	Adec_WriteAudioOut(ptAdecInfo, ptAdecInfo->pbyStereoOutFrame, dwOutSize);
                   	} else {
	                   	adecStereoUpSampleAudioBuffer(ptAdecInfo->pbyStereoOutFrame, byUpSampleBuffer, &dwOutSize, dwDoubleFreqRatio);
	                   	Adec_WriteAudioOut(ptAdecInfo, byUpSampleBuffer, dwOutSize);
                   	}

#ifdef _DUMP_FILE    
                    fwrite(ptAdecInfo->pbyStereoOutFrame, sizeof(BYTE), dwOutSize, pfOutPut);
#endif
                }
            }

        }
    }
#ifdef _DUMP_FILE
    fclose(pfOutPut);
#endif
    return S_OK;
}

/* ========================================================================== */
SCODE InitServerSocket(TAdecInfo *ptAdecInfo)
{
    struct sockaddr_un sunx;
    int fd;
    unlink(ptAdecInfo->szSocketPath);
    memset(&sunx, 0, sizeof(sunx));
    sunx.sun_family = AF_UNIX;
    strncpy(sunx.sun_path, ptAdecInfo->szSocketPath, strlen(ptAdecInfo->szSocketPath));
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (fd < 0 ||
        bind(fd, (struct sockaddr *) &sunx, sizeof(sunx.sun_family)+strlen(sunx.sun_path)) < 0 ||
        chmod(ptAdecInfo->szSocketPath, 0666) < 0)
    {
        DBPRINT3("[ADEC] Cannot create %s (%d).%s\n", ptAdecInfo->szSocketPath, errno, strerror(errno));
        close(fd);
        return -1;
    }

    ptAdecInfo->ifdUSock = fd;
    return S_OK;
}

/* ========================================================================== */
BYTE* Adec_MonoToStereo(BYTE *pbyMonoIn, BYTE *pbyStereoOut, DWORD* pdwSize, DWORD dwChNo)
{
    SWORD *pswInput = (SWORD *) pbyMonoIn;
	SWORD *pswOutput = (SWORD *) pbyStereoOut;
	DWORD dwInputSize = *pdwSize ;
	DWORD i;

	//Dual Channel
	if(dwChNo == 2)
	{
		return (BYTE*) pbyMonoIn;
	}

	for(i=0;i<dwInputSize/2;i++)
	{
		pswOutput[(i<<1)] = pswOutput[(i<<1)+1] = pswInput[i];
	}
	*pdwSize = 2*dwInputSize ;
	return (BYTE*) pbyStereoOut;
}

/* ========================================================================== */
BYTE* adecStereoUpSampleAudioBuffer(BYTE *pbyStereoBaseIn, BYTE *pbyUpSampleOut, DWORD* pdwSize, DWORD dwTwiceRatio)
{
	DWORD *pdwInput = (DWORD *) pbyStereoBaseIn;
	DWORD *pdwOutput = (DWORD *) pbyUpSampleOut;
	DWORD dwInputSize = *pdwSize ;
	DWORD i;
	int iIndex;
	int iFirstFactor ;
	int iSecondFactor ;
	DWORD dwNumofDW = dwInputSize / sizeof(DWORD);

	if( dwTwiceRatio % 2 == 0) {
		iFirstFactor = iSecondFactor = dwTwiceRatio / 2;
	} else {
		iFirstFactor = dwTwiceRatio / 2 ;
		iSecondFactor = (int) dwTwiceRatio - iFirstFactor ;
	}

	for(i=0;i<dwNumofDW;i=i+2) {
		for(iIndex = 0; iIndex < iFirstFactor; iIndex++) {
			pdwOutput[(i*dwTwiceRatio/2)+iIndex] = pdwInput[i];
		}
		for(iIndex = 0; iIndex < iSecondFactor; iIndex++) {
			pdwOutput[(i*dwTwiceRatio/2)+iFirstFactor+iIndex] = pdwInput[i+1];
		}
	}
	*pdwSize = dwTwiceRatio*(dwInputSize>>1);
	return (void*) pbyUpSampleOut;
}
