/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2004 VN Inc. All rights reserved.
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
 */

/*!
 *******************************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * aenc_task.c
 *
 * \brief
 * Audio encoder - Manage task
 *
 * \date
 * 2006/07/04
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <syslog.h>
#include <math.h>
#include "global_codec.h"
#include "errordef.h"
#include "typedef.h"
#include "ubuffer.h"
#include "dbgdefs.h"
#include "aenc.h"
#include "aenc_task.h"
#include "audioin.h"
//#include "usck.h"

typedef struct src_ubuf_info
{
	BYTE	*pbyUBuffHdr;
	int		iUBuffHdrLen;
	BYTE	*pbyUBuffPayload;
	int		iUBuffPayloadLen;
} TSrcUBufInfo;

//=================================================================================================
//#define _DEBUG_TIMESTAMP_
//=================================================================================================

#ifdef _ASYNC_IO_
	#define MAX_BUFF_ITEM_NUM 5
#endif
DWORD dwDeadDev = 0;
extern DWORD dwLiveDev;
extern BOOL bStop;
//=================================================================================================

static void theDevTask(void * hDev);
//=================================================================================================
inline SCODE initDevfd(TDevOpt *ptDevOpt)
{
	SCODE sRet = S_FAIL;
	//printf("[aenc_task]szNode=%s, dwFourCC=%d, dwSampRate=%d dwChanNum=%d dwSampFormat=%d eAudioFrameReadType=%d\n", ptDevOpt->szNode,ptDevOpt->tCodecOpt.dwFourCC,ptDevOpt->tCodecOpt.dwSampRate,ptDevOpt->dwChanNum,ptDevOpt->dwSampFormat,ptDevOpt->eAudioFrameReadType);
	sRet = open_audioin(&ptDevOpt->hAudioInDev, &ptDevOpt->hAudioHWParam, ptDevOpt->szNode,
			ptDevOpt->tCodecOpt.dwFourCC,
			(int)ptDevOpt->tCodecOpt.dwSampRate, 
			(int)ptDevOpt->dwChanNum, (int)ptDevOpt->dwSampFormat,ptDevOpt->bMute,(int)ptDevOpt->dwVolume,(int)ptDevOpt->dwInputSel, ptDevOpt->eAudioFrameReadType, &ptDevOpt->dwRingBufferSize, &ptDevOpt->dwPeriodSize);

	if (ptDevOpt->hAudioInDev == NULL) {
		ptDevOpt->edtsState = edtsInitFailed;
		sRet = S_FAIL;
	}

	return sRet;
}

void initClientSocket(TIO* ptIO)
{
	/* Create the unix socket */
	ptIO->fdOut = socket(AF_UNIX, SOCK_DGRAM, 0);
	memset(&ptIO->dest_addr, 0, sizeof(struct sockaddr_un));
	ptIO->dest_addr.sun_family = AF_UNIX;
	strncpy(ptIO->dest_addr.sun_path, ptIO->szOut, sizeof(ptIO->dest_addr.sun_path) - 1);

	if(fchmod(ptIO->fdOut, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) == -1)
	{
		fprintf(stderr,"Error chmod unix domain socket: %s\n", strerror(errno));
		ptIO->fdOut = -1;
		return;
	}

	// Because this is datagram socket, we don't need to call listen and accept.

	// Avoid the child process references the file descriptor and do not close it.
	fcntl(ptIO->fdOut, F_SETFD, FD_CLOEXEC | fcntl(ptIO->fdOut, F_GETFD));
}
SCODE initIOSocket(TDevOpt *ptDevOpt)
{
	SCODE   sRet;
	TIO     *ptIO, **ptIOSrc;

	sRet = S_FAIL;   
	ptIOSrc = &(ptDevOpt->ptioHead);
	ptIO = *ptIOSrc;
	while (ptIO) 
	{
		//printf("[aenc_task]ptIO->szOut=%s  ptIO->dwIOId=%d\n",ptIO->szOut, ptIO->dwIOId);
		initClientSocket(ptIO);
		if (ptIO->fdOut < 0) 
		{ 
			/* remove from io list */
			*ptIOSrc = ptIO->ptioNext;
			/* move to wait list head */
			ptIO->ptioNext = ptDevOpt->ptioWait;
			ptDevOpt->ptioWait = ptIO;          
		}       
		else
		{ 
			sRet = S_OK;            
			ptIOSrc = &(ptIO->ptioNext);
		}   
		ptIO = *ptIOSrc;
	}   

	/* if NO IO socket succeed, close all the opened file descriptor */
	if (sRet != S_OK) 
	{
		if (ptDevOpt->hAudioInDev != NULL)
		{
			snd_pcm_drop(ptDevOpt->hAudioInDev);
			ptDevOpt->hAudioInDev = NULL;
		}       
		ptDevOpt->edtsState = edtsInitFailed;
		printf("Init Socket Fail!\n");
		return S_FAIL;
	}

	return S_OK;
}

inline SCODE initEnc(TDevOpt *ptDevOpt)
{
	TCodecOperation *ptOper;    
	SCODE sRet;

	ptOper = ptDevOpt->tCodecOpt.ptOper;    

	if (ptOper->fnInit != NULL) 
	{
		sRet = ptOper->fnInit(&(ptDevOpt->tCodecOpt));
		ptDevOpt->dwPID = getpid();
		if (sRet != S_OK) 
		{ 
			return S_FAIL;              
		}           
	}
	return S_OK;
}

//=================================================================================================
SCODE initDevTask(TDevOpt *ptDevOpt)
{
	//init Device
	if(initDevfd(ptDevOpt) != S_OK)
	{
		printf("(%s)(%d) Failed!\n", __func__,__LINE__);
		return S_FAIL;
	}

	//init client Socket
	if (initIOSocket(ptDevOpt) != S_OK)
	{   
		printf("(%s)(%d) Failed!\n", __func__,__LINE__);
		return S_FAIL;
	}

	//init GAMR/AAC4
	if (initEnc(ptDevOpt) != S_OK)
	{
		printf("(%s)(%d) Failed!\n", __func__,__LINE__);
		return S_FAIL;
	}

#if 0
	if (sem_init(&(ptDevOpt->smMsgOptions), 0, 1) != 0) {
		printf("[aenc_task](%d) Failed! Because sem_init() is failed.\n",__LINE__);
		return S_FAIL;
	}
#endif    

	ptDevOpt->edtsState = edtsInitialized;
	printf("(%s)(%d) Success!\n", __func__,__LINE__);

	return S_OK;    
}

//=================================================================================================
SCODE startDevTask(TDevOpt *ptDevOpt)
{
	int iRet;   

	DBPRINT2("%s:%d\n", __func__, __LINE__);
#ifdef _RESTART_AENC_
	#ifdef _ASYNC_IO_
	{
        pthread_attr_t      attr1;
        struct sched_param  param1;
        if (ptDevOpt->tCodecOpt.dwFourCC == MAKEFOURCC('G','A','M','R'))
			param1.sched_priority = 40;
        else
			param1.sched_priority = 60;
		pthread_attr_init(&attr1);

    	//! An inherit value of PTHREAD_INHERIT_SCHED (the default) 
    	//! means that the scheduling policies defined in the creating 
    	//! thread are to be used, and any scheduling attributes defined 
    	//! in the pthread_create() call are to be ignored. 
    	//! If PTHREAD_EXPLICIT_SCHED is used, the attributes from the 
        //! pthread_create() call are to be used. 
        pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
		if (ptDevOpt->tCodecOpt.dwFourCC == MAKEFOURCC('G','A','M','R'))
		{ 
			pthread_attr_setschedpolicy (&attr1, SCHED_FIFO);
			printf("[aenc_task]Set thread %d as SCHED_FIFO (%d)\n", ptDevOpt->thread_id,param1.sched_priority);
			syslog(LOG_INFO,"[aenc_task]Set thread %d as SCHED_FIFO (%d)\n", ptDevOpt->thread_id,param1.sched_priority); 	
		}
		else
		{
			pthread_attr_setschedpolicy (&attr1, SCHED_FIFO);
			printf("[aenc_task]Set thread %d as SCHED_FIFO (%d)\n", ptDevOpt->thread_id, param1.sched_priority);
			syslog(LOG_INFO,"[aenc_task]Set thread %d as SCHED_FIFO (%d)\n", ptDevOpt->thread_id,param1.sched_priority);
			
		}
		 pthread_attr_setschedparam  (&attr1, &param1);
		
		iRet = pthread_create(&(ptDevOpt->thread_id), &attr1, (HANDLE)&theDevTask, (HANDLE)ptDevOpt);
		if (iRet != 0){
			fprintf(stderr, "pthread_create Fail : %s!\n", strerror(iRet));
			return S_FAIL;
		}  
	}
	#endif
#else
	iRet = pthread_create(&(ptDevOpt->thread_id), NULL, (HANDLE)&theDevTask, (HANDLE)ptDevOpt);
	if (iRet != 0){
		fprintf(stderr, "pthread_create Fail : %s!\n", strerror(iRet));
		return S_FAIL;
	}   
#endif
	return S_OK;
}

//=================================================================================================
SCODE stopDevTask(TDevOpt *ptDevOpt)
{
	int iRet;

	DBPRINT2("%s:%d\n", __func__, __LINE__);
	ptDevOpt->edtsState = edtsStopping; 
	iRet = pthread_join(ptDevOpt->thread_id, NULL);
	if (iRet != 0)  {
		fprintf(stderr, "pthread_join Fail, %s\n", strerror(iRet));
	}       
	return S_OK;
}

//=================================================================================================
inline void releaseDevfd(TDevOpt *ptDevOpt)
{

	DBPRINT2("%s:%d\n", __func__, __LINE__);
	if (ptDevOpt->hAudioInDev != NULL) {
		//For stopping the PCM stream immediately, use snd_pcm_drop() instead.
		snd_pcm_drop(ptDevOpt->hAudioInDev);
		//For capture stop PCM permitting to retrieve residual frames.
		//snd_pcm_drain(ptDevOpt->hAudioInDev);
		snd_pcm_close(ptDevOpt->hAudioInDev);
		ptDevOpt->hAudioInDev = NULL;
	} 
	
}

inline void releaseIOSocket(TDevOpt *ptDevOpt)
{
	TIO     *ptIO;

	DBPRINT2("%s:%d\n", __func__, __LINE__);
	ptIO = ptDevOpt->ptioHead;
	while (ptIO) {
		if (ptIO->fdOut > 0) { 
			close(ptIO->fdOut);
			ptIO->fdOut = -1;
		}
		ptIO = ptIO->ptioNext;
	}   
}

inline void releaseEnc(TDevOpt *ptDevOpt)
{   
	TCodecOperation *ptOper = ptDevOpt->tCodecOpt.ptOper;       

	DBPRINT2("%s:%d ", __func__, __LINE__)

		if (ptOper->fnRelease != NULL) {

			ptOper->fnRelease(&(ptDevOpt->tCodecOpt.hOpt));     
		}   
}

SCODE releaseDevTask(TDevOpt *ptDevOpt)
{   
	TIO     *ptIO, *ptIOPtr;

	//device    
	releaseDevfd(ptDevOpt);

	//client socket 
	releaseIOSocket(ptDevOpt);

	//release encoder   
	releaseEnc(ptDevOpt);

	//free IO   
	ptIO = ptDevOpt->ptioHead;
	while (ptIO) {
		ptIOPtr = ptIO;
		ptIO = ptIO->ptioNext;
		free(ptIOPtr);
	}   

	// sem_destroy(&(ptDevOpt->smMsgOptions));
	printf("%s Succeed!\n", __func__);
	return S_OK;
}

//=====================================================================================================================================
static DWORD stripAudioIn(void *  pStrip, void * const pIn, DWORD dwSize, 
		DWORD dwSampSize, DWORD dwChNo, BYTE *pbyCh)
{
	SWORD   *pswOut = (SWORD *)pStrip; 
	SWORD   *pswIn  = (SWORD *)pIn; 
	SDWORD  sdwCnt;
	DWORD   dwOutSamp;
	BOOL    bAllCh = TRUE;
	int i;


	// TODO: fix this to support other sample size
	assert(dwSampSize == 2);

	sdwCnt = dwSize / dwSampSize;
	for (i = 0; i < dwChNo; i ++) {
		if (pbyCh[i] == 0) {
			//printf("[aenc_task.c](%d) i=%d\n",__LINE__,i);
			bAllCh = FALSE;
		}
	}
	if (bAllCh) {
		memcpy(pStrip, pIn, dwSize);
		return dwSize;
	}

	i = 0;
	dwOutSamp = 0;
	for (;sdwCnt > 0; sdwCnt --) {
		if (pbyCh[i]) {
			*pswOut = *pswIn;
			pswOut ++;
			dwOutSamp ++;
		}
		pswIn ++;
		/* update index of pbyCh */
		i ++; 
		i = (i < dwChNo) ? i : 0;
	}

	return dwOutSamp * dwSampSize; 
}


void SendConfToSck(TIO* ptIO,TCodecOpt tCodecOpt)
{
	DWORD dwRetSize=0;
	TCodecOperation *ptOper=NULL;
	DWORD  dwOutSize=0;
	HANDLE hOut  = NULL;
//	printf("%d\n",__LINE__);
	ptOper = tCodecOpt.ptOper;
	assert(ptOper != NULL);
	dwOutSize = ptOper->fnQrOutSz(&tCodecOpt);
	hOut  = malloc(dwOutSize);
	assert(hOut != NULL);
	ptOper->fnGetConf(&tCodecOpt, hOut, &dwRetSize);
	
	if (ptIO != NULL)
	{
		sendto(ptIO->fdOut, hOut, dwRetSize, 0, (struct sockaddr*) &ptIO->dest_addr, sizeof(ptIO->dest_addr));
	}
	//printf("[aenc_task](%d)Writ Conf output %u bytes\n",__LINE__,dwRetSize);
	if(hOut!=NULL)
	{
		free(hOut);
	}
}
#if 0
int GetOutFd(TDevOpt *ptDevOpt,DWORD dwIOId)
{
	TIO     *ptIO;
	ptIO = ptDevOpt->ptioHead;
	while (ptIO) {

		if (dwIOId==ptIO->dwIOId) { 

			return ptIO->fdOut;
		}
		ptIO = ptIO->ptioNext;
	}   
	return -1;
}
#endif

TIO* GetOutFd(TDevOpt *ptDevOpt,DWORD dwIOId)
{
	TIO     *ptIO;
	ptIO = ptDevOpt->ptioHead;
	while (ptIO) {

		if (dwIOId==ptIO->dwIOId) { 

			return ptIO;
		}
		ptIO = ptIO->ptioNext;
	}   
	return NULL;
}

/* Write configuration */
//=====================================================================================================================================
void WriteConf(TAUC **phtauc)
{
	DWORD dwRetSize;      
	TIO     *ptIO;  
	TAUC *ptauc = *phtauc;  
	TDevOpt *ptDevOpt = ptauc->ptDevOpt;
	TCodecOperation *ptOper = ptDevOpt->tCodecOpt.ptOper;

	DBPRINT2("%s:%d\n", __func__, __LINE__) 
		ptIO = ptDevOpt->ptioHead;
	while (ptIO) {
		ptOper->fnGetConf(&(ptDevOpt->tCodecOpt), ptauc->hOut, &dwRetSize);
		sendto(ptIO->fdOut, ptauc->hOut, dwRetSize, 0, (struct sockaddr*) &ptIO->dest_addr, sizeof(ptIO->dest_addr));

//		fprintf(stderr, "%s:%d: [%d] output %u bytes\n", __func__, __LINE__, getpid(), dwRetSize);
		ptIO = ptIO->ptioNext;      
	}       
}

//=====================================================================================================================================
SCODE SendConf(TAUC **phtauc)
{   
	TAUC *ptauc = *phtauc;  
	TDevOpt *ptDevOpt = ptauc->ptDevOpt;
	TCodecOperation *ptOper = ptDevOpt->tCodecOpt.ptOper;

	DBPRINT2("%s:%d ", __func__, __LINE__);
	if ((ptOper->fnGetConf != NULL) 
			&& (ptOper->fnQrOutSz != NULL) 
			&& (ptOper->fnEncode != NULL)
			&& (ptOper->fnQrSpPerBufCh!= NULL)) {
		//bCodecOper = TRUEa
		ptauc->dwPeriodFrames = ptOper->fnQrSpPerBufCh(&(ptDevOpt->tCodecOpt));
		if (ptDevOpt->eAudioFrameReadType == eInterleavedRead)
		{
			//ptauc->dwInSize = ptOper->fnQrSpPerBufCh(&(ptDevOpt->tCodecOpt)) * ptDevOpt->dwSampSize * ptDevOpt->dwChanNum;
			ptauc->dwInSize =1024 * ptDevOpt->dwSampSize * ptDevOpt->dwChanNum;

			
		}
		else
		{
			//ptauc->dwInSize = ptOper->fnQrSpPerBufCh(&(ptDevOpt->tCodecOpt)) * ptDevOpt->dwSampSize * 1;
			ptauc->dwInSize = 1024 * ptDevOpt->dwSampSize * 1;	
		}
		
		//printf("ptOper->fnQrSpPerBufCh(&(ptDevOpt->tCodecOpt)) = %d\n",ptOper->fnQrSpPerBufCh(&(ptDevOpt->tCodecOpt)));
		
		ptauc->dwOutSize = ptOper->fnQrOutSz(&(ptDevOpt->tCodecOpt));
		//printf("InSize: %d OutSize: %d\n", ptauc->dwInSize, ptauc->dwOutSize);
		//printf("[aenc_task]dwPeriodFrames=%d, dwInSize=%d, dwOutSize=%d\n",ptauc->dwPeriodFrames,ptauc->dwInSize, ptauc->dwOutSize);

		if (ptauc->hOut == NULL)
		{
			if ((ptauc->hOut  = malloc(ptauc->dwOutSize)) == NULL) {
				fprintf(stderr, "%s:%d Memory is not enough!\n", __func__, __LINE__);
				return S_FAIL;
			}
		}

		/* Write configuration */   
		WriteConf(phtauc);
	} else {
#if 0
		bCodecOper = FALSE;
		ptauc->dwInSize = ptauc->dwOutSize = 0;
#endif  
		return S_FAIL;
	}

	return S_OK;
}

SCODE ResetEncoder(TAUC **phtauc)
{
    TDevOpt *ptDevOpt = (*phtauc)->ptDevOpt;
    SCODE sRet;
//    TAUC *ptauc;
    DBPRINT2("%s:%d ", __func__, __LINE__);

    //==release encoder 
    releaseEnc(ptDevOpt);   
    /* if change codec , io size maybe changed */
	if ((*phtauc)->hOut != NULL)
	{		
		free((*phtauc)->hOut); 
		(*phtauc)->hOut = NULL;
		
	}
	//free((*phtauc)->hIn);  
    
    //change to new codec
    memcpy(&(ptDevOpt->tCodecOpt), &(ptDevOpt->tNewOpt), sizeof(TCodecOpt));
    DBPRINT4("New Codec %c%c%c%c\n", 
        (CHAR)(ptDevOpt->tCodecOpt.dwFourCC), 
        (CHAR)(ptDevOpt->tCodecOpt.dwFourCC >> 8), 
        (CHAR)(ptDevOpt->tCodecOpt.dwFourCC >> 16), 
        (CHAR)(ptDevOpt->tCodecOpt.dwFourCC >> 24));            
   // printf("bitrate %ld\n", ptDevOpt->tCodecOpt.dwBitRate);
    //printf("channum %ld\n", ptDevOpt->tCodecOpt.dwChanNum); 
    //printf("samprate %ld\n", ptDevOpt->tCodecOpt.dwSampRate);   
    //printf("frameperbuf %ld\n", ptDevOpt->tCodecOpt.dwFramePerBuffer);  
	ptDevOpt->tCodecOpt.dwCodecHanldeNum = ptDevOpt->dwSavedCodecHandleNum;
	printf("[ResetEncoder]dwCodecHanldeNum = %d\n",ptDevOpt->tCodecOpt.dwCodecHanldeNum);
	ptDevOpt->tCodecOpt.dwCodecHanldeIndex =0;
	
    /*set samprate*/
//    Set_DevSampRate(ptDevOpt->hAudioInDev, ptDevOpt->hAudioHWParam, ptDevOpt->tCodecOpt.dwSampRate);
    //==init encoder
    if (initEnc(ptDevOpt) != S_OK)
    {
        fprintf(stderr, "%s:%d: Re-Init Encoder Fail!!\n", __FILE__, __LINE__);
        return S_FAIL;
    }   

   
    sRet = SendConf(phtauc);
	
    return S_OK;
}

void gettimestamp(snd_pcm_t *handle, snd_timestamp_t *timestamp) 
{
	int i_snd_rc = -1;
	
	snd_pcm_status_t *p_status;

	
    /* Get the status */
    snd_pcm_status_alloca(&p_status);
    i_snd_rc = snd_pcm_status(handle, p_status );
    if( i_snd_rc < 0 )
    {
        printf("Stream status error: %s\n", snd_strerror(i_snd_rc));
		return;
    }
    /* Handle buffer underruns and get the status again */
    if( snd_pcm_status_get_state( p_status ) == SND_PCM_STATE_XRUN )
    {
        /* Prepare the device */
        i_snd_rc = snd_pcm_prepare( handle );
        if( i_snd_rc )
        {
            printf("cannot recover from buffer underrun\n");
            return;
        }
        printf("recovered from buffer underrun\n" );
        /* Get the new status */
        i_snd_rc = snd_pcm_status( handle, p_status );
        if( i_snd_rc < 0 )
        {
            printf("cannot get device status after recovery: %s\n", snd_strerror(i_snd_rc) );
            return;
        }
    }
    else
    {
		snd_pcm_status_get_tstamp(p_status, timestamp);
    }
	return;
}
#ifdef _ASYNC_IO_
typedef struct async_private_data {
	char ***buffers;
	DWORD dwPeriodSize;
	DWORD *pdwPeriodFrames;
	DWORD *pdwReadFrames;
	EAudioFrameReadType eAudioFrameReadType;
	int fd_for_write_pcm;
	DWORD	*pdwBufferIndex;
	DWORD 	*pdwSampleSize;
	DWORD	*pdwChannelNum;
	DWORD	dwBufferNum;
	DWORD	*pdwPreBufferIndex; 
}TAsyncPrivateData;


static int create_fifo(int *pWfd, int *pRfd)
{
	int fd = -1, hw_fd = -1, alsa_hw_fd = -1;
	char tmp_name[20];

	memset(tmp_name, 0x0, sizeof(tmp_name));

	/* Create a temporary filename for our FIFO,
	 * Use mkstemp() instead of mktemp() although we need a FIFO not a
	 * regular file. We do this since glibc barfs at mktemp() and this
	 * scares the users :-)
	 */
	strcpy(tmp_name, "/tmp/aencXXXXXX");
	fd = mkstemp(tmp_name);
	close(fd);

	/* Start the race! */
	unlink(tmp_name);
	if (mknod(tmp_name, S_IFIFO | S_IRUSR | S_IWUSR, 0)) {
		printf("could not create FIFO %s\n", tmp_name);
		return -1;
	}
	/* Phew, we won the race ... */

	/* Open the pipe and hand it to LIRC ... */
	hw_fd = open(tmp_name, O_RDWR);
	if (hw_fd < 0) {
		printf("could not open pipe %s\n", tmp_name);
		unlink(tmp_name);
	
		return -1;
	}

	/* Open the other end of the pipe and hand it to ALSA code.
	 * We're opening it in non-blocking mode to avoid lockups.
	 */
	alsa_hw_fd = open(tmp_name, O_RDWR | O_NONBLOCK);
	/* Ok, we don't need the FIFO visible in the filesystem anymore ... */
	unlink(tmp_name);
	
	*pRfd = hw_fd;
	*pWfd = alsa_hw_fd;
	
	return 0;
}
#if 0
static int waitfordata(unsigned long maxusec, int fd)
{
	fd_set fds;
	int ret;
	struct timeval tv;

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		do {
			do {
				if (maxusec > 0) {
					tv.tv_sec = maxusec / 1000000;
					tv.tv_usec = maxusec % 1000000;
					ret = select(fd + 1, &fds, NULL, NULL, &tv);
					if (ret == 0)
						return (0);
				} else {
					ret = select(fd + 1, &fds, NULL, NULL, NULL);
				}
			}
			while (ret == -1 && errno == EINTR);
			if (ret == -1) {
				printf("select() failed\n");
				continue;
			}
		}
		while (ret == -1);

		if (FD_ISSET(fd, &fds)) {
			/* we will read later */
			return (1);
		}
	}
}
#endif
#define MAX_BUFF_NUM 4

static void async_callback(snd_async_handler_t *ahandler)
{
	snd_pcm_t *hAudioInDev = snd_async_handler_get_pcm(ahandler);
	struct async_private_data *pdata = snd_async_handler_get_callback_private(ahandler);
	int ret = -1;
	snd_pcm_sframes_t delay_frames = 0;
	snd_pcm_sframes_t avail = 0;
	DWORD 	dwIndex = *(pdata->pdwBufferIndex);
	DWORD	dwSampleSize = *(pdata->pdwSampleSize);  //Little endian signed 16 bits (2 bytes)
	DWORD	dwSamplesPerFrame = 0;
	DWORD	dwOffset  = 0;
	char 	*aBuffers[MAX_BUFF_NUM];
	
	if (pdata->eAudioFrameReadType == eInterleavedRead)
		dwSamplesPerFrame = *(pdata->pdwChannelNum);
	else
		dwSamplesPerFrame = 1;
	
	dwOffset = (*(pdata->pdwReadFrames)) * dwSampleSize * dwSamplesPerFrame;
	
	if (snd_pcm_hwsync(hAudioInDev) == 0) 
	{
			/*
			 For capture the delay is defined as the time that a frame that was digitized by the audio device takes until it can be read from the PCM stream shortly after this call returns. 
			 It is as such the overall latency from the initial ADC to the read call.
			 ref: http://www.alsa-project.org/alsa-doc/alsa-lib/group___p_c_m.html#ga012e8b999070e72ab23514f25e7d6482
			 */
			snd_pcm_delay(hAudioInDev, &delay_frames);
			avail = snd_pcm_avail_update(hAudioInDev);
	}
	
	if (pdata->eAudioFrameReadType == eInterleavedRead)
	{
			char * ptr = (char *)pdata->buffers[dwIndex][0];
			ret = snd_pcm_readi(hAudioInDev, ptr+dwOffset, pdata->dwPeriodSize);
	}
	else
	{
			int j = 0;
			char *ptr = NULL; 
			for (j = 0; j < pdata->dwBufferNum; j++)
			{
				ptr = (char *)pdata->buffers[dwIndex][j];
				aBuffers[j] = (char *)(ptr + dwOffset);
			}
			ret = snd_pcm_readn(hAudioInDev, (void **)(aBuffers), pdata->dwPeriodSize);
	}
	
	//printf("ggj periodFrames =%d ret = %d periodSize =%d\n",*(pdata->pdwPeriodFrames),ret,pdata->dwPeriodSize);
	if (ret == -EPIPE)
	{
		/* EPIPE means overrun */
		fprintf(stderr, "overrun occurred\n");
		snd_pcm_prepare(hAudioInDev);
		//continue;
	}
	else if (ret < 0)
	{
		fprintf(stderr, "error from read: %s\n", snd_strerror(ret));
		//continue;
	}
	else if (ret != pdata->dwPeriodSize)
	{
		fprintf(stderr, "%s:%d: [%d] short read, read %d frames\n", __FILE__, __LINE__, getpid(), ret);
	}
	
	if (ret >= 0)
	{	
//		int count = 0;
		
		*(pdata->pdwReadFrames) += ret;
		
		if (*(pdata->pdwReadFrames)  == *(pdata->pdwPeriodFrames))
		{
			*(pdata->pdwReadFrames)  = 0;
			//printf("============================\n");
			//printf("write : dwIndex = %d\n",*(pdata->pdwBufferIndex));
			//count = write(pdata->fd_for_write_pcm, pdata->pdwBufferIndex, sizeof(DWORD));
			*(pdata->pdwPreBufferIndex) = *(pdata->pdwBufferIndex);
			//printf("write:%d ret:%d dwPeriodSiz=%d\n",count, ret, pdata->dwPeriodSize);
			*(pdata->pdwBufferIndex) += 1;
			if ((*(pdata->pdwBufferIndex)) == MAX_BUFF_ITEM_NUM)
			{
				*(pdata->pdwBufferIndex) = 0;
			}
			
		}
		else
		{
			//DWORD dwNotReadyIndex = MAX_BUFF_ITEM_NUM;
			//count = write(pdata->fd_for_write_pcm, &dwNotReadyIndex ,sizeof(DWORD));
			*(pdata->pdwPreBufferIndex) = MAX_BUFF_ITEM_NUM;
		}
		
	}

}
#endif

#ifdef _CHECK_RTSPS_STATUS_

#define RTSPS_PROGRAM_NAME "rtsps"
typedef  struct rtsps_status
{
	BOOL isExist;
	int pid;
}TRtspsStatus;

static SCODE create_tmpfile(char *szTmpfileName)
{

  int fd;
  int len = -1;
  
  if(szTmpfileName==NULL)
  {
      return S_FAIL;
  }
  //printf("[sdmgr](%d)\n",__LINE__);
  /* maybe get the tmpdir env variable instead of using /tmp */
  strcpy(szTmpfileName,"/tmp/checkRTSPS-XXXXXX"); 

 // printf("[sdmgr](%d)\n",__LINE__);
  umask(0077); // ensure exclusive access on buggy implementations of mkstemp
  fd = mkstemp(szTmpfileName);
  
  len = strlen(szTmpfileName);
  szTmpfileName[len+1]='\0';
  
  //printf("[sdmgr](%d)szTmpfileName=%s\n",__LINE__,szTmpfileName);
  close(fd);
  
  return S_OK;
 
}

void checkRTSPS(TRtspsStatus *ptStatus)
{
   char  szCommand[128];
   char szTmpFileName[64];
   struct stat stFile;
   FILE *fp = NULL;
   int pid = -1;
   int ret = -1;
   
   ptStatus->isExist = FALSE;
   ptStatus->pid = -1;
   
   create_tmpfile(szTmpFileName);
   ret =snprintf(szCommand,sizeof(szCommand),"/bin/pidof  %s  > %s",RTSPS_PROGRAM_NAME,szTmpFileName);
   if (ret < 0)
   {
	 goto exit;
   }
   szCommand[ret+1]='\0';
   //printf("szTmpFileName=(%s),szCommand=(%s)\n",szTmpFileName,szCommand);
   system(szCommand);
   stat(szTmpFileName,&stFile);
   if (stFile.st_size !=0)
   {
		fp=fopen(szTmpFileName,"r");
		if(fp != NULL)
		{
			 fscanf(fp,"%d",&pid);
			// printf("pid = %d\n",pid);
			 ptStatus->isExist = TRUE;
			 ptStatus->pid = pid;
		}
   }
   
exit:   
   if(fp!=NULL)
   {
     fclose(fp);
   }
   
   unlink(szTmpFileName);
}

#endif

//=====================================================================================================================================
/* the encoding loop, launched by pthread_create. one dev, one thread. */
static void theDevTask(HANDLE htDevOpt)
{   
	TDevOpt *ptDevOpt = (TDevOpt *) htDevOpt;
	TCodecOperation *ptOper = ptDevOpt->tCodecOpt.ptOper; 
	TAFConf *ptacConf = (TAFConf *) &(ptDevOpt->tacConf); 
	TAUC *ptauc;
	TIO     *ptIO;  
#ifdef _CHECK_RTSPS_STATUS_
	TRtspsStatus tRtspsStatusSaved;
#endif	
	HANDLE hInStrip=NULL; 
	TUBuffer    *ptubOut;
	ssize_t ssRdSize;
	DWORD   dwRetSize, dwStripSize;
	BYTE abyPreChan[CHANNEL_LENGTH];
	DWORD dwFrameSec = 0, dwFrameUSec = 0;
	DWORD dwSampleRate;
	DWORD  dwMsgApplyIOId=0;
	BOOLEAN bDoEncode=FALSE;
	//BOOLEAN bMsgApplyOutput=FALSE;
	//BOOLEAN bOutputChange=FALSE;
	BOOLEAN bMsgApplySendConf=FALSE;
	//BOOLEAN bSendConfChange=FALSE;
	BOOLEAN bMsgApply=FALSE;
	int dwCounter=0;	
#ifdef _ASYNC_IO_
	char **aBuffers[MAX_BUFF_ITEM_NUM];
	DWORD	dwBufferIndex = -1;
#else
	int ret;
	char **buffers = NULL ;
#endif
	int buff_number = 0;
	int buffer_size = 0;
	int buffer_size_saved = 0;
	BOOL  bSendPayload = FALSE;
#if defined (__DUMP_PCM__)    
	FILE *pfDumpPCM=NULL;
	CHAR szDumpPCMFilePath[1024];
#endif

#ifdef  _DEBUG_TIMESTAMP_	
	DWORD dwPreSecond = 0;
	DWORD dwPreMicroSecond = 0;
#endif
	snd_timestamp_t c_tstamp;
	snd_pcm_status_t *status = NULL;	
#ifdef _ASYNC_IO_	
	TAsyncPrivateData tAsyncPrivateData;
	snd_async_handler_t *ahandler;
#else
	DWORD dwSampleUsec = 0;
	DWORD dwSleepUsec = 0;
	DWORD dwSleepUsecDivisor = 16;
	snd_pcm_sframes_t delay_frames = 0;
	snd_pcm_sframes_t avail = 0;
#endif	
	snd_pcm_status_alloca(&status);
	
	//=========init dev
	if (initDevTask(ptDevOpt) != S_OK)
	{       
		dwDeadDev ++;
		DBPRINT1("dwDeadDev %ld\n", dwDeadDev);
		//===if no device is alive, exit
		if (dwLiveDev == dwDeadDev)
		{           
			printf("No device socket is available!\nStop aenc\n");
			bStop = TRUE;
		}
		return;
	}

	//=========start dev
	ptauc = (TAUC*) malloc (sizeof(TAUC));
	memset(ptauc, 0, sizeof(TAUC));
	ptauc->ptDevOpt = ptDevOpt;

	/*send conf*/   
	SendConf(&ptauc);  

	if (ptDevOpt->eAudioFrameReadType == eInterleavedRead)
	{
		buff_number = 1;
	}
	else
	{
		buff_number = ptDevOpt->dwChanNum;
	}
	buffer_size = ptauc->dwInSize;
#ifdef _ASYNC_IO_
	{
		int i = 0, j = 0; 
		for (i = 0; i < MAX_BUFF_ITEM_NUM; i++)
		{
			aBuffers[i] = (char **)malloc(buff_number * sizeof(char*));
			for(j = 0 ; j < buff_number ; j++) {
				aBuffers[i][j] = (char *) malloc(buffer_size);
				memset(aBuffers[i][j], 0, buffer_size) ;
			}
			
		}
		
	}
#else
	buffers = malloc(buff_number * sizeof(char*));
	{
		int i =0;
	for(i = 0 ; i < buff_number ; i++) {
		printf("[aenc_task](%d)bytes per period = %d\n",i ,buffer_size);
		buffers[i] = (char *) malloc(buffer_size);
		memset(buffers[i], 0, buffer_size) ;
	}
	}
#endif
	if ((hInStrip = malloc(buffer_size)) == NULL) {
		fprintf(stderr, "%s:%d Memory is not enough!\n", __func__, __LINE__);
		goto exit;
	} else {
		memset((BYTE *)hInStrip, 0x0, buffer_size);
	}
	
	buffer_size_saved = buffer_size;

#if defined (__DUMP_PCM__)
	memset(szDumpPCMFilePath,0x0,sizeof(szDumpPCMFilePath));
	snprintf(szDumpPCMFilePath,sizeof(szDumpPCMFilePath),"/tmp/capd%sch0.pcm",ptDevOpt->szID);
	if ((pfDumpPCM = fopen(szDumpPCMFilePath, "wb")) == NULL) {
		printf("[aenc_task] open output pcm0 file fail..... !!!\n");
	} else {
		printf("[aenc_task] open output pcm0 file success..... !!!\n");
	}
#endif

	ptubOut = (TUBuffer *) ptauc->hOut;
	ptDevOpt->edtsState = edtsRunning;

	if (snd_pcm_start(ptDevOpt->hAudioInDev) != 0)
	{
		goto exit;
	}
#ifdef _ASYNC_IO_

	ptDevOpt->dwReadFrames = 0;
	tAsyncPrivateData.dwBufferNum = buff_number;
	tAsyncPrivateData.pdwReadFrames = &ptDevOpt->dwReadFrames;
	tAsyncPrivateData.pdwSampleSize = &ptDevOpt->dwSampSize; 
	tAsyncPrivateData.pdwChannelNum = &ptDevOpt->dwChanNum;
	tAsyncPrivateData.buffers = aBuffers;
	tAsyncPrivateData.dwPeriodSize = ptDevOpt->dwPeriodSize;
	tAsyncPrivateData.pdwPeriodFrames = &ptauc->dwPeriodFrames;
	tAsyncPrivateData.eAudioFrameReadType = ptDevOpt->eAudioFrameReadType;
	create_fifo(&(ptDevOpt->fd_for_write_pcm), &(ptDevOpt->fd_for_read_pcm));
	tAsyncPrivateData.fd_for_write_pcm  = ptDevOpt->fd_for_write_pcm;
	ptDevOpt->dwBufferIndex = 0;
	ptDevOpt->dwPreBufferIndex = 0;
	tAsyncPrivateData.pdwBufferIndex = &ptDevOpt->dwBufferIndex;
	tAsyncPrivateData.pdwPreBufferIndex = &ptDevOpt->dwPreBufferIndex;
	
	if (snd_async_add_pcm_handler(&ahandler, ptDevOpt->hAudioInDev, async_callback, &tAsyncPrivateData) < 0)
	{
		printf("Unable to register async handler\n");
		goto exit;
	}
	
#endif

#ifdef _CHECK_RTSPS_STATUS_
	checkRTSPS(&tRtspsStatusSaved);
#endif
	/* Encoding loop */ 
	while (ptDevOpt->edtsState == edtsRunning)
	{
#ifdef _CHECK_RTSPS_STATUS_			
		if(ptacConf->bCallReload)
		{
			TRtspsStatus tRtspsStatus;
			BOOL bRestartRtsps = FALSE;
			checkRTSPS(&tRtspsStatus);
			bRestartRtsps = ((tRtspsStatus.isExist !=  tRtspsStatusSaved.isExist) || (tRtspsStatus.pid !=  tRtspsStatusSaved.pid));
			
			if (bRestartRtsps)
			{
//					printf("[aenc_task](%d)bRestartRtsps = %d\n",__LINE__,bRestartRtsps);
					releaseIOSocket(ptDevOpt);
					if (initIOSocket(ptDevOpt) != S_OK)
					{
						goto exit;
					}
					
			}
			ptacConf->bCallReload = FALSE;
		}
#endif
		 if (ptacConf->bSetOptions)
		 {
			ptacConf->bSetOptions = FALSE;
			
			if (ptDevOpt->bVolumeChanged || ptDevOpt->bMuteChanged || ptDevOpt->bInputSelChanged)
			{
				char str[128];
				char node[128];
				memset(str,0x0,sizeof(str));
				memset(node,0x0,sizeof(node));
				memcpy(node,ptDevOpt->szNode,strlen(ptDevOpt->szNode));
				memcpy(str,ptDevOpt->szNode,strlen(ptDevOpt->szNode)+1);
				if (ptDevOpt->bMute == TRUE) {
					#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__) || defined(__WAU8822__) || defined(__WM8978__) || defined(__AIC3104__) || defined(__ALC5623__)
						set_volume(0,str,ptDevOpt->dwInputSel);
					#else
						set_mute(ptDevOpt->bMute,node,ptDevOpt->dwInputSel);
					#endif	  
				} else { 
					set_volume((int)ptDevOpt->dwVolume,str,(int)ptDevOpt->dwInputSel);
				}
				if (ptDevOpt->bVolumeChanged)
				{
					ptDevOpt->bVolumeChanged = FALSE;
				}
				
				if (ptDevOpt->bMuteChanged)
				{
					ptDevOpt->bMuteChanged = FALSE;
				}
				
				if (ptDevOpt->bInputSelChanged)
				{
					ptDevOpt->bInputSelChanged = FALSE;
				}
			}
			//Set Codec
			if (ptacConf->bCodec || ptacConf->bBitRate || ptacConf->bEncMode)
			{                     
                //=== Codec Type Set Option
                if (ResetEncoder(&ptauc) != S_OK)
                {
                    fprintf(stderr, "%s:%d: Reset encoder Fail!!\n", __func__, __LINE__);
                    goto exit;
                }
                
                ptDevOpt->dwReadFrames = 0;
                
                buffer_size = ptauc->dwInSize;
				printf("buffer_size = %d, buffer_size_saved =%d\n",buffer_size,buffer_size_saved);
				if (buffer_size > buffer_size_saved)
				{
					//=== change to New Codec ,io size maybe changed
					if (hInStrip != NULL)
					{
						hInStrip = realloc(hInStrip, buffer_size);
					}
				
					#ifdef _ASYNC_IO_
					{
						int i = 0, j = 0; 
						for (i = 0; i < MAX_BUFF_ITEM_NUM; i++)
						{
							for(j = 0 ; j < buff_number ; j++) {
								if (aBuffers[i][j] != NULL)
								{	
									aBuffers[i][j] = (char *) realloc(aBuffers[i][j] ,buffer_size);
									//memset(aBuffers[i][j], 0, buffer_size);
								}
							}
						}
						
					}
					#else
					{
						int i = 0;
						//buffers = malloc(buff_number * sizeof(char*));
						for(i = 0 ; i < buff_number ; i++) {
							printf("[aenc_task](%d)bytes per period = %d\n",i ,buffer_size);
							if (buffers[i] != NULL)
							{
								buffers[i] = (char *) realloc(buffers[i] ,buffer_size);
							}
							//memset(buffers[i], 0, buffer_size) ;
						}
					}
					#endif
					buffer_size_saved = buffer_size;
				
				}
				ptubOut = (TUBuffer *) ptauc->hOut;         
                
                ptOper =  ptDevOpt->tCodecOpt.ptOper;
				if (ptacConf->bCodec)
				{
					ptacConf->bCodec = FALSE;
				}
				if (ptacConf->bBitRate)
				{
					ptacConf->bBitRate = FALSE;
				}
				if (ptacConf->bEncMode)
				{
					ptacConf->bEncMode = FALSE;
				}
                printf("Reset encoder Success!\n");
            } 
		 }
		
		if (ptDevOpt->bReMsg_flag == TRUE) 
		{
			sem_wait(&(ptDevOpt->smMsgOptions));
			
			dwMsgApplyIOId= (int)ptDevOpt->tMsgOptRec.dwIOId;
			bDoEncode = (ptDevOpt->tMsgOptRec.adwVisitors[ptDevOpt->tMsgOptRec.dwIOId] > 0);
			
			bMsgApplySendConf=ptDevOpt->tMsgOptRec.bSendConf;

			bMsgApply=ptDevOpt->bReMsg_flag; 
			ptDevOpt->bReMsg_flag = FALSE;
				
			if (bMsgApplySendConf == TRUE) {
				
				ptDevOpt->tCodecOpt.dwCodecHanldeIndex =0;
				SendConfToSck(GetOutFd(ptDevOpt,dwMsgApplyIOId),ptDevOpt->tCodecOpt);    
				bMsgApplySendConf=FALSE;
				ptDevOpt->tMsgOptRec.bSendConf=FALSE;
				
			}
			sem_post(&(ptDevOpt->smMsgOptions));
		}
		
		assert(ptauc != NULL);
#ifdef _ASYNC_IO_
#if 0
		if (waitfordata(-1, ptDevOpt->fd_for_read_pcm) == 1)
		{
			int ret = -1;
			
			ret = read(ptDevOpt->fd_for_read_pcm, &dwBufferIndex, sizeof(DWORD));
			//printf("read:%d dwPeriodSize=%d\n",ret,ptDevOpt->dwPeriodSize);
			//printf("pbyEncodedata[0]= %02X\n",pbyEncodedata[0]);
			//printf("read: dwIndex = %d\n",dwBufferIndex);
			if (ret != sizeof(DWORD)) 
			{
				printf("error reading from fifo!!\n");
				continue;
			}
			else
			{
				if (dwBufferIndex == MAX_BUFF_ITEM_NUM)
				{
					continue;
				}
			}
		}
		else
		{
			printf("waitfordata failed!!\n");
			continue;
		}
#endif
		select(0,NULL,NULL,NULL,NULL);
		dwBufferIndex = ptDevOpt->dwPreBufferIndex;
		if (dwBufferIndex == MAX_BUFF_ITEM_NUM)
		{
			//dwSampleUsec = (ptDevOpt->dwPeriodSize*1000000)/ptDevOpt->tCodecOpt.dwSampRate;
			//dwSleepUsec = (dwSampleUsec/dwSleepUsecDivisor)/2;
			//usleep(dwSleepUsec);
			continue;
		}


#else
		
		if (snd_pcm_hwsync(ptDevOpt->hAudioInDev) == 0) 
		{
			/*
			 For capture the delay is defined as the time that a frame that was digitized by the audio device takes until it can be read from the PCM stream shortly after this call returns. 
			 It is as such the overall latency from the initial ADC to the read call.
			 ref: http://www.alsa-project.org/alsa-doc/alsa-lib/group___p_c_m.html#ga012e8b999070e72ab23514f25e7d6482
			 */
			if (snd_pcm_delay(ptDevOpt->hAudioInDev, &delay_frames) == 0)
			{
				if ((unsigned long)delay_frames < ptDevOpt->dwPeriodSize)
				{
					dwSampleUsec = (ptDevOpt->dwPeriodSize*1000000)/ptDevOpt->tCodecOpt.dwSampRate;
					dwSleepUsec = dwSampleUsec/dwSleepUsecDivisor;
					usleep(dwSleepUsec);
					continue;
				}
			}
			avail = snd_pcm_avail_update(ptDevOpt->hAudioInDev);
			#if 0
			if ((unsigned long)avail < ptDevOpt->dwPeriodSize)
			{
				continue;
			}
			#endif	
		}
		
		if (ptDevOpt->eAudioFrameReadType == eInterleavedRead)
		{
			ret = snd_pcm_readi(ptDevOpt->hAudioInDev, buffers[0], ptDevOpt->dwPeriodSize);
		}
		else
		{
			ret = snd_pcm_readn(ptDevOpt->hAudioInDev, (void **)buffers, ptDevOpt->dwPeriodSize);
		}

		if (ret == -EPIPE)
		{
			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred\n");
			snd_pcm_prepare(ptDevOpt->hAudioInDev);
			continue;
		}
		else if (ret < 0)
		{
			fprintf(stderr, "error from read: %s\n", snd_strerror(ret));
			continue;
		}
		else if (ret != ptDevOpt->dwPeriodSize)
		{
			fprintf(stderr, "%s:%d: [%d] short read, read %d frames\n", __FILE__, __LINE__, getpid(), ret);
		}
#endif
#if defined (__DUMP_PCM__)
		if(pfDumpPCM!=NULL)
		{
#ifdef _ASYNC_IO_
			fwrite(aBuffers[dwBufferIndex][0], 1, buffer_size, pfDumpPCM); 
#else
			fwrite(buffers[0], 1, buffer_size, pfDumpPCM);                 
#endif
			fflush(pfDumpPCM);
		}
#endif
		
#ifdef _ASYNC_IO_
		ssRdSize = (ptauc->dwPeriodFrames << 2);
//		printf("(%d)ret = %d, ssRdSize = %d,  dwSampSize = %d,  dwPeriodSize = %d  dwPeriodFrames = %d\n",__LINE__,ptauc->dwPeriodFrames,ssRdSize,ptDevOpt->dwSampSize,ptDevOpt->dwPeriodSize,ptauc->dwPeriodFrames);

#else
		ssRdSize= (ret<<2);
//		printf("(%d)ret = %d, ssRdSize = %d,  dwSampSize = %d,  dwPeriodSize = %d  dwPeriodFrames = %d\n",__LINE__,ret,ssRdSize,ptDevOpt->dwSampSize,ptDevOpt->dwPeriodSize,ptauc->dwPeriodFrames);
#endif
		dwSampleRate = (ptDevOpt->tCodecOpt.dwSampRate==44100)?44000:ptDevOpt->tCodecOpt.dwSampRate;
		
		//! set time stamp
		{
			gettimestamp(ptDevOpt->hAudioInDev, &c_tstamp);
			dwFrameSec  = (DWORD)(c_tstamp.tv_sec);
			dwFrameUSec = (DWORD)(c_tstamp.tv_usec );
		}


#ifdef  _DEBUG_TIMESTAMP_
		{
			DWORD dwTmpSecond = dwFrameSec;
			DWORD dwTmpMicroSecond = dwFrameUSec ;
			SDWORD sdwDiff = ((SDWORD)dwTmpSecond - (SDWORD)dwPreSecond) * 1000000+ ((SDWORD)dwTmpMicroSecond - (SDWORD)dwPreMicroSecond);
			printf("interval = %d (usec)\n", sdwDiff);
			dwPreSecond = dwTmpSecond;
			dwPreMicroSecond = dwTmpMicroSecond;
			
		}
#endif

		ptubOut->dwSec = dwFrameSec;
		ptubOut->dwUSec = dwFrameUSec; 
		ptubOut->dwTrackID = 2; 
		
		/* for each IO */       
		ptIO = ptDevOpt->ptioHead;	
		dwCounter = 0;
		dwStripSize = 0;
		
		while(ptIO)
		{
			if (hInStrip != NULL)
			{
				HANDLE hIn = NULL;

#ifdef _ASYNC_IO_
				if (dwCounter >= buff_number)
				{
					hIn = aBuffers[dwBufferIndex][0];
				}
				else
				{
					hIn = aBuffers[dwBufferIndex][dwCounter];
				}
#else			
				if (dwCounter >= buff_number)
				{
					hIn = buffers[0];
				}
				else
				{
					hIn = buffers[dwCounter];
				}
#endif
			
				
				if (ptDevOpt->eAudioFrameReadType == eInterleavedRead) //INTERLEAVED
				{
					dwStripSize = stripAudioIn(hInStrip, hIn, ssRdSize, 
								ptDevOpt->dwSampSize, ptDevOpt->dwChanNum, ptIO->abyChannel);
				}
				else//NONINTERLEAVED
				{
					hInStrip = hIn;
				}
				
			}

			memcpy(abyPreChan, ptIO->abyChannel, CHANNEL_LENGTH);
			
			if (strcmp((CHAR *)(ptIO->abyChannel), (CHAR *)abyPreChan) != 0) {
						break;
			}
			
			ptDevOpt->tCodecOpt.dwCodecHanldeIndex = dwCounter;
			if (ptDevOpt->tCodecOpt.dwCodecHanldeIndex >= ptDevOpt->tCodecOpt.dwCodecHanldeNum)
			{
						fprintf(stderr, "%d: Use non-exist codec handle (index,num)=(%d,%d)!!\n", __LINE__,ptDevOpt->tCodecOpt.dwCodecHanldeIndex,ptDevOpt->tCodecOpt.dwCodecHanldeNum);
						//ptDevOpt->tCodecOpt.dwCodecHanldeIndex = 0;
						break;
			}
			
			if (bMsgApply)
			{
				if (ptIO->dwIOId == dwMsgApplyIOId)
				{
					ptIO->bDoEncode = bDoEncode;
					printf("[aenc_task](%d)ptIO->dwIOId=%d ptIO->bDoEncode=%d ptIO->szOut=%s dwCounter=%d\n",__LINE__,ptIO->dwIOId,ptIO->bDoEncode,ptIO->szOut,dwCounter);
					if (ptIO->bDoEncode == FALSE)
					{
						ptOper->fnRetsetFrameCount(&(ptDevOpt->tCodecOpt));
					}
					bMsgApply=FALSE;
				}
			}	
				
			if (ptIO->bDoEncode)
			{
	    
				if (hInStrip != NULL) {
#if 1
					//printf("%d ptauc->hOut = %p ptauc->dwOutSize = %d\n",__LINE__,ptauc->hOut,ptauc->dwOutSize);
					if (ptOper->fnEncode(&(ptDevOpt->tCodecOpt), hInStrip, ptauc->hOut, ptauc->dwOutSize, &dwRetSize) != S_OK) {
						fprintf(stderr, "%s:%d: Enc encode fail\n", __FILE__, __LINE__);
						break;
					}
					else
					{
						//printf("encode: dwRetSize = %d\n",dwRetSize);
					}
					//printf("%d\n",__LINE__);
#endif
				}
				
				{
					TUBuffer *ptUBuffer = (TUBuffer *) ptauc->hOut;
					TSrcUBufInfo tSrcUBufInfo;
					
					tSrcUBufInfo.pbyUBuffHdr = NULL;
					tSrcUBufInfo.iUBuffHdrLen = 0;
					tSrcUBufInfo.pbyUBuffPayload = NULL;
					tSrcUBufInfo.iUBuffPayloadLen = 0;
					
					if (ptUBuffer != NULL)
					{
						
						if (ptUBuffer->dwDataType == FOURCC_CONF)
						{
							tSrcUBufInfo.pbyUBuffHdr = ptauc->hOut;
							tSrcUBufInfo.iUBuffHdrLen = ptUBuffer->dwSize;
							tSrcUBufInfo.pbyUBuffPayload = NULL;
							tSrcUBufInfo.iUBuffPayloadLen = 0;
#ifdef __DEBUG__				
							printf("(%d)(hdr, hlen, pl, plen)=(%p, %d, %p ,%d)\n",
									__LINE__,
									tSrcUBufInfo.pbyUBuffHdr,
									tSrcUBufInfo.iUBuffHdrLen,
									tSrcUBufInfo.pbyUBuffPayload,
									tSrcUBufInfo.iUBuffPayloadLen
  								);
#endif
						}
						else
						{
							tSrcUBufInfo.pbyUBuffHdr = ptauc->hOut;
							tSrcUBufInfo.iUBuffHdrLen = sizeof(TUBuffer)+(int)ptUBuffer->dwUserDataSize;
							tSrcUBufInfo.pbyUBuffPayload = ptauc->hOut+tSrcUBufInfo.iUBuffHdrLen;
							tSrcUBufInfo.iUBuffPayloadLen = ptUBuffer->dwSize-tSrcUBufInfo.iUBuffHdrLen;
#ifdef __DEBUG__								
							printf("(%d)(hdr, hlen, pl, plen)=(%p, %d, %p ,%d)\n",
									__LINE__,
									tSrcUBufInfo.pbyUBuffHdr,
									tSrcUBufInfo.iUBuffHdrLen,
									tSrcUBufInfo.pbyUBuffPayload,
									tSrcUBufInfo.iUBuffPayloadLen
  								);
#endif				
						}
						
					}
					if ((tSrcUBufInfo.iUBuffPayloadLen  == 0 ) || (tSrcUBufInfo.pbyUBuffPayload == NULL))
					{
						bSendPayload = FALSE;
					}
					else
					{
						bSendPayload = TRUE;
					}
				}
				//  printf("[aenc_task](%d)write out %d bytes!!\n",__LINE__,dwRetSize);
				if (bSendPayload == TRUE)
				{
					sendto(ptIO->fdOut, ptauc->hOut, dwRetSize, 0, (struct sockaddr*) &ptIO->dest_addr, sizeof(ptIO->dest_addr));
				}

			}
			ptIO = ptIO->ptioNext;

			dwCounter++;

		}//while IO
		
		
	}
	
exit:	
	fprintf(stderr, "%s:%d: Stopped (%d)\n", __FILE__, __LINE__, getpid());
	
	if (hInStrip != NULL) {
		free(hInStrip);
		hInStrip = NULL;
	}
	if (ptauc->hOut != NULL) {
		free(ptauc->hOut);
		ptauc->hOut = NULL;
	}
#ifdef _ASYNC_IO_	
	{
		int i = 0, j = 0; 
		for (i = 0; i < MAX_BUFF_ITEM_NUM; i++)
		{
			for(j = 0 ; j < buff_number ; j++) 
			{
				if (aBuffers[i][j] != NULL)
				{
					free(aBuffers[i][j]);
					aBuffers[i][j] = NULL;
				}
			}
			if (aBuffers != NULL)
			{
				free(aBuffers[i]);
				aBuffers[i] = NULL;
			}
		}
	}
#else
	{	
		int i =0;
		for (i=0; i < buff_number; i++) {
			if (buffers[i] != NULL)
			{
				free(buffers[i]);
				buffers[i] = NULL;
			}
		}
		if (buffers!=NULL)
		{
			free(buffers) ;
			buffers = NULL;
		}
	}
#endif
#if defined (__DUMP_PCM__)
	if (pfDumpPCM!=NULL)
	{
		fclose(pfDumpPCM);
	}
#endif
	ptDevOpt->edtsState = edtsStopped;
	return;
}

