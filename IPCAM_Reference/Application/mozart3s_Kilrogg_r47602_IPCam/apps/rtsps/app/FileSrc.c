/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2006-2010 VN Inc. All rights reserved.
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
 *  | VN INC.                                                    |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 * 
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2006-2010 VN, Inc. All rights reserved.
 *
 * \file
 * FileSrc.c
 *
 * \brief
 * A Live Media Source Implementation
 * The media is from file which stored concatenated ubuffers
 *
 * \date
 * 2008/10/08
 *
 * \author
 * DJhow.Tu
 *
 *
 *******************************************************************************
 */

#include "FileSrc.h"
#include "LiveMediaSrc_protect.h"
#include "ubuffer.h"
#include "ubuffer_limit.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

// -- Convention -- start
SCODE FileSrc_Release(HANDLE *phFileSrc);
SCODE FileSrc_EventHandler(HANDLE hFileSrc, ELMSrcEventType eType);
SCODE FileSrc_GetUBuffer(HANDLE hFileSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE FileSrc_ReleaseUBuffer(HANDLE hFileSrc, TLMSrcUBufInfo *ptUBuffInfo);
SCODE FileSrc_WakeUpToTerminate(HANDLE hObject)
{
    return S_OK;
}

static TLMSrcCBMap	tFileSrcCBMap = 
{
	FileSrc_Release,
	FileSrc_GetUBuffer, 
	FileSrc_ReleaseUBuffer, 
	FileSrc_EventHandler,
	FileSrc_WakeUpToTerminate
};
// -- Convention -- end

typedef struct file_source
{
	TLiveMediaSrc		tLMSrc;
	CHAR				*szFilePath;
	FILE				*fpInFile;
	BYTE				*pbyBuf;
	int					iBufSz;
	pthread_mutex_t		tMutex;
} TFileSrc;

SCODE FileSrc_Initial(HANDLE *phFileSrc, TFileSrcInitOpts *ptInitOpts)
{
	TFileSrc *ptFileSrc;

	assert(phFileSrc && ptInitOpts);

	ptFileSrc = (TFileSrc *)malloc(sizeof(TFileSrc));
	if (ptFileSrc == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		return S_FAIL;
	}
	memset(ptFileSrc, 0, sizeof(TFileSrc));
	*phFileSrc = ptFileSrc;

	
	((TLiveMediaSrc *)ptFileSrc)->ptCBMap = &tFileSrcCBMap;

	pthread_mutex_init(&ptFileSrc->tMutex, NULL);

	ptFileSrc->fpInFile = NULL;
	
	if (ptInitOpts->szFilePath == NULL)
	{
		debug_printf("[%s:%s:%d] Invalid file path %s!\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szFilePath);
		goto error_handle;
	}

	if ((ptFileSrc->szFilePath=strdup(ptInitOpts->szFilePath)) == NULL)
	{
		debug_printf("[%s:%s:%d] strdup error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	if ((ptFileSrc->fpInFile=fopen(ptInitOpts->szFilePath, "rb")) == NULL)
	{
		debug_printf("[%s:%s:%d] open file %s error!\n", 
						__func__, __FILE__, __LINE__, 
						ptInitOpts->szFilePath);
		goto error_handle;
	}

	ptFileSrc->iBufSz = 1024*1024;//MAX_MP4V_UBUFFER;
	ptFileSrc->pbyBuf = (BYTE *)malloc(ptFileSrc->iBufSz);
	if (ptFileSrc->pbyBuf == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", 
						__func__, __FILE__, __LINE__);
		goto error_handle;
	}

	return S_OK;
error_handle:
	FileSrc_Release(phFileSrc);
	return S_FAIL;
}

SCODE FileSrc_Release(HANDLE *phFileSrc)
{
	TFileSrc *ptFileSrc;

	ptFileSrc = *phFileSrc;

	if (ptFileSrc->szFilePath != NULL)
	{
		free(ptFileSrc->szFilePath);
	}
	if (ptFileSrc->fpInFile != NULL)
	{
		fclose(ptFileSrc->fpInFile);
	}
	if (ptFileSrc->pbyBuf != NULL)
	{
		free(ptFileSrc->pbyBuf);
	}

	pthread_mutex_destroy(&ptFileSrc->tMutex);

	free(ptFileSrc);
	*phFileSrc = NULL;

	return S_OK;
}

SCODE FileSrc_EventHandler(HANDLE hFileSrc, ELMSrcEventType eType)
{
	TFileSrc *ptFileSrc;
	
	ptFileSrc = (TFileSrc *)hFileSrc;

	switch(eType)
	{
		case letNeedIntra:
			printf("Need Intra\n");
			pthread_mutex_lock(&ptFileSrc->tMutex);
			rewind(ptFileSrc->fpInFile);
			pthread_mutex_unlock(&ptFileSrc->tMutex);
			break;
		case letNeedConf:
			printf("Need Conf\n");
			break;
		case letBitstrmStart:
			printf("Bitstrm start\n");
			break;
		case letBitstrmStop:
			printf("Bitstrm stop\n");
			break;
		default:
			return S_FAIL;
	}

	return S_OK;
}

#include <sys/time.h>
SCODE FileSrc_GetUBuffer(HANDLE hFileSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	TFileSrc		*ptFileSrc;
	TUBuffer		*ptUBuffer;
	BYTE			*pbyBuf;
	int				iRet;
	struct timeval	tTimeNow;

	ptFileSrc	= (TFileSrc *)hFileSrc;
	pbyBuf		= ptFileSrc->pbyBuf;
	
	pthread_mutex_lock(&ptFileSrc->tMutex);
	iRet = fread(pbyBuf, 1, sizeof(DWORD), ptFileSrc->fpInFile);
	if (iRet != sizeof(DWORD))
	{
		rewind(ptFileSrc->fpInFile);
		iRet = fread(pbyBuf, 1, sizeof(DWORD), ptFileSrc->fpInFile);
		if (iRet != sizeof(DWORD))
		{
			printf("[%s:%s:%d] read error!\n", __func__, __FILE__, __LINE__);
			pthread_mutex_unlock(&ptFileSrc->tMutex);
			return S_FAIL;
		}
	}

	iRet += fread(pbyBuf+sizeof(DWORD), 1, *((DWORD *)pbyBuf)-sizeof(DWORD), ptFileSrc->fpInFile);
	if (iRet != *((DWORD *)pbyBuf))
	{
		printf("[%s:%s:%d] read error!\n", __func__, __FILE__, __LINE__);
		pthread_mutex_unlock(&ptFileSrc->tMutex);
		return S_FAIL;
	}
	pthread_mutex_unlock(&ptFileSrc->tMutex);

	ptUBuffer = (TUBuffer *)pbyBuf;

	if (ptUBuffer->dwSize != iRet)
	{
		printf("[%s:%s:%d] recv a broken ubuffer!\n", __func__, __FILE__, __LINE__);
		return S_FAIL;
	}

	if (ptUBuffer->dwDataType == FOURCC_CONF)
	{
		ptUBuffInfo->pbyUBuffHdr = pbyBuf;
		ptUBuffInfo->iUBuffHdrLen = ptUBuffer->dwSize;
		ptUBuffInfo->pbyUBuffPayload = NULL;
		ptUBuffInfo->iUBuffPayloadLen = 0;
	}
	else
	{
		gettimeofday(&tTimeNow, NULL);
		ptUBuffer->dwSec = tTimeNow.tv_sec;
		ptUBuffer->dwUSec = tTimeNow.tv_usec;

		ptUBuffInfo->pbyUBuffHdr = pbyBuf;
		ptUBuffInfo->iUBuffHdrLen = sizeof(TUBuffer)+(int)ptUBuffer->dwUserDataSize;
		ptUBuffInfo->pbyUBuffPayload = pbyBuf+ptUBuffInfo->iUBuffHdrLen;
		ptUBuffInfo->iUBuffPayloadLen = ptUBuffer->dwSize-ptUBuffInfo->iUBuffHdrLen;
	}
		
	return S_OK;
}

SCODE FileSrc_ReleaseUBuffer(HANDLE hFileSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
	// ugly code for testing
	if (strcmp(((TFileSrc *)hFileSrc)->szFilePath, "fake.mdat") == 0)
	{
		sleep(5);
		return S_OK;
	}
	
	usleep(66666);
	return S_OK;
}

