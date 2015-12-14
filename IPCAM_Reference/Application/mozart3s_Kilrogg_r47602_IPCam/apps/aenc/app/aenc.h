/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2006 VN Inc. All rights reserved.
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
 * aenc.h
 *
 * \brief
 * Audio encoder header file
 *
 * \date
 * 2006/05/30
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */
#ifndef _AENC_H_
#define _AENC_H_
#include <alsa/asoundlib.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "global_codec.h"
#include "errordef.h"
#include "pthread.h"
#ifdef __G711_Enc__
	#include "G711Enc.h"//encode mode
#endif
#ifdef __G726_Enc__
	#include "G726Enc.h" // encode mode
#endif

//=====================================================================================
#define MAX_CODEC_NUM  4
#define MAX_NODE_NUM   4
#define NODE_LENGTH	   16
#define ID_LENGTH	   8
#define OUT_LENGTH	   64
#define CHANNEL_LENGTH 8
#define CMD_LENGTH     15


extern BOOL g_bTerminate;
//=====================================================================================
typedef enum dev_task_state {
	edtsStopped = 0, 
	edtsInitialized, 
	edtsInitFailed, 
	edtsRunning, 
	edtsStopping 
} EDevTaskState;

//cmdfifo service type
typedef enum Service_Type {
	URecorder = 0, 
	RTSP
} EServiceType;

//=====================================================================================
typedef struct Service_Socket_Info
{	
	CHAR* szName;	
	DWORD dwLength;
}TServiceSocketInfo;

//=====================================================================================

typedef SCODE (*FnCodecInit)(HANDLE hObj);
typedef SCODE (*FnCodecRelease)(HANDLE *phObj);
typedef SCODE (*FnCodecGetConf)(HANDLE hObj, void *pvOut, DWORD *pdwSize);
typedef DWORD (*FnCodecQrOutSz)(HANDLE hObj);
/* Query sample per channel per buffer */
typedef DWORD (*FnCodecQrSpPerBufCh)(HANDLE hObj);
typedef SCODE (*FnCodecEncode)(HANDLE hObj,void *pvIn, void *pvOut, DWORD dwOutSize, DWORD *pdwSize);
typedef SCODE (*FnCodecRetsetFrameCount)(HANDLE hObj);

typedef struct codec_operation_t {
	DWORD		dwFourCC;	
	FnCodecInit		fnInit;
	FnCodecRelease	fnRelease;
	FnCodecGetConf	fnGetConf;
	FnCodecQrOutSz	fnQrOutSz;
	FnCodecQrSpPerBufCh fnQrSpPerBufCh;
	FnCodecEncode	fnEncode;
	FnCodecRetsetFrameCount fnRetsetFrameCount;
} TCodecOperation;

typedef struct codec_opt_t {
	DWORD	dwFourCC;
	DWORD	dwBitRate;
	DWORD	dwSampRate;
	DWORD	dwChanNum;
	DWORD	dwFramePerBuffer;
#ifdef __G711_Enc__
	EG711EncMode eEncMode;
#endif
	TCodecOperation *ptOper;	
	/*handle of codec*/
	void	*hOpt;	
	DWORD dwCodecHanldeIndex;
	DWORD dwCodecHanldeNum;	
} TCodecOpt;

//=====================================================================================
typedef struct io_t {
	struct io_t	*ptioNext;
	/* dwChannelMask is used to indicate which channel is selected by this 
	   io setting of an interleaved audio source.
	   For example, the input from /dev/dsp0 is 2-channel, interleaved. You 
	   want to get the left channel of it, dwChannelMask has to be set as 1. 
	   If the requested channel is right, it has to be 2. 
	   If the requested channel is both left and right (stereo), 
	   it has to be (1 | 2), it is 3. 
	 */
	//DWORD	dwChannelMask;
	/* abyChannel is used to indicate which channel is selected by this 
	   io setting of an interleaved audio source.
	   For example, the input from /dev/dsp0 is 2-channel, interleaved. You 
	   want to get the left channel of it, abyChannel[0] has to be 1, and 
	   others are 0. 
	   If the requested channel is right, abyChannel[1] has to be 1, and 
	   others are 0.
	   If the requested channel is both left and right (stereo), 
	   abyChannel[0] and abyChannel[1] are both 1, and others are 0. 
	 */
	BYTE	abyChannel[CHANNEL_LENGTH];
	char	szOut[OUT_LENGTH];
	//DWORD 	dwOutId;
	DWORD 	dwIOId;
	BOOLEAN bDoEncode;
	BOOLEAN	bSendConf;
	/* socket of szOut */
	int		fdOut;	
	struct sockaddr_un	dest_addr;
} TIO;

//=====================================================================================
//reload config new codec
typedef struct aenc_conf_flag
{	
	//conf changed
	BOOL bSetOptions;

	//change codec type
	BOOL bCodec;

	//set samplerate
	BOOL bSampRate;

	//set bitrate
	BOOL bBitRate;

	//FramePerBuf
	BOOL bSetIO;

    //ForceCI
    BOOL bForceCI;	
	//G711Enc mode
	BOOL bEncMode;
#ifdef _CHECK_RTSPS_STATUS_	
	BOOL bCallReload;
#endif
}TAFConf;

#define MAX_SOCK_CLIENT_NUM 1024
typedef struct anec_msg_record
{
    DWORD   dwIOId;
    DWORD   adwVisitors[MAX_SOCK_CLIENT_NUM];
    BOOL    bSendConf;
 
} TAENCMsgOptRec;

typedef enum audio_frame_read_type {
	eNonInterleavedRead = 0, 
	eInterleavedRead
} EAudioFrameReadType;
typedef struct dev_opt_t {
	struct	dev_opt_t	*ptdoNext;	
	char	szID[ID_LENGTH];
	char 	szNode[NODE_LENGTH];
	DWORD	dwChanNum;
	EAudioFrameReadType eAudioFrameReadType;
	DWORD	dwSampFormat;
	DWORD	dwSampSize; /* in byte */
	
	DWORD	dwVolume;
	BOOL 	bMute;
	DWORD	dwInputSel;
	
	BOOL bVolumeChanged;
	BOOL bMuteChanged;
	BOOL bInputSelChanged;	
	
	TIO		*ptioHead;
	/* waiting list with disconnected socket*/
	TIO		*ptioWait;
	TCodecOpt tCodecOpt;
	/* new codec */
	TCodecOpt tNewOpt;
	/* conf flag */
	TAFConf tacConf;
	/* device handle, usually for "hw:0,0" in open_audioin */
	HANDLE		hAudioInDev;
     /* device hardware parameter handle */
    HANDLE      hAudioHWParam;
	/* Mark this device task state */
	EDevTaskState	edtsState;
	pthread_t	thread_id;
	DWORD		dwPID;	
	//! if command fifo (Message interface) receive data, it will change to TRUE
	BOOL		bReMsg_flag;
	//! message semaphore to protect share data (bReMsg_flag)
	sem_t              	smMsgOptions;
	TAENCMsgOptRec      tMsgOptRec;
	DWORD 				dwRingBufferSize;
	DWORD				dwPeriodSize;
	
	int  	fd_for_read_pcm;
	int 	fd_for_write_pcm;
	
	DWORD	dwBufferIndex;
	DWORD	dwPreBufferIndex;
	DWORD	dwSavedCodecHandleNum;
	DWORD 	dwReadFrames;
} TDevOpt;

//=====================================================================================

typedef struct aenc_info_t {	
	TCodecOperation	atcpRegistered[MAX_CODEC_NUM];
	TCodecOpt		atcoSaved[MAX_CODEC_NUM];
	TDevOpt			*ptdoHead;
    HANDLE          hMsgRdrObj;
	//The following fields are for message thread.
	pthread_t	thread_id;
	EDevTaskState	edtsState;
	
} TAencInfo; 

/* This structure is for XML tree traverse. aenc proprietary. */
typedef struct aenc_xml_tt_handle_t {
	/* handle of xmlwrapper */
	HANDLE    hXmlWrapperObject;

	//if first time load config 
	BOOL bFirstLoad;

	TAencInfo 	*ptAencInfo;	

	BYTE		abyChanTmp[CHANNEL_LENGTH];
	
	TAFConf *ptConfTmp;
	/* This pointer will point to a specific structure when parsing 
	   relevant element. The struct/element pair is as follow: 

	   struct    | XML element
	   ----------+-------------
	   TDevOpt   | device	   
	   TCodecOpt | GAMR, AAC4
	   */
	void		*hTmp;
} TAencXMLTTh;
//=====================================================================================

/* aenc_conf.c */
SCODE readConf(const char *filename, TAencXMLTTh *ptaxHandle);

/* aenc_codec.c */
void registerCodecs(TAencInfo *ptaencInfo);

#ifdef __GAMR_Enc__
/* aenc_gamr.c */
SCODE register_GAMR(TCodecOperation	*pOpr, TCodecOpt *pOpt);
#endif

#ifdef __AAC4_Enc__
/* aenc_aac4.c */
SCODE register_AAC4(TCodecOperation	*pOpr, TCodecOpt *pOpt);
#endif
#ifdef __G711_Enc__
/* aenc_g711.c */
SCODE register_G711(TCodecOperation	*pOpr, TCodecOpt *pOpt);
#endif

#ifdef __G726_Enc__
/* aenc_g726.c */
SCODE register_G726(TCodecOperation	*pOpr, TCodecOpt *pOpt);
#endif

/* aenc_task.c */
SCODE initDevTask(TDevOpt *ptDevOpt);
SCODE startDevTask(TDevOpt *ptDevOpt);
SCODE stopDevTask(TDevOpt *ptDevOpt);
SCODE releaseDevTask(TDevOpt *ptDevOpt);
SCODE aenc_msg_initial(TAencInfo *ptaencInfo, CHAR* szMsgFIFO);
SCODE aenc_msg_start(TAencInfo *ptaencInfo);
SCODE aenc_msg_stop(TAencInfo *ptaencInfo);
#endif //_AENC_H_

