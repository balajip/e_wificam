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
 * adec_local.h
 *
 * \brief
 * Audio decoder local header file.
 *
 * \date
 * 2008/3/19
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#ifndef _ADEC_LOCAL_H_
#define _ADEC_LOCAL_H_

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "typedef.h"
#include "errordef.h"
#include "dbgdefs.h"
#include "adec_app.h"
#include "xmlwrapper.h"

#define ALSA_PCM_NEW_HW_PARAMS_API
#define FILEPATH_LENGTH     64
#define PCM_S8_MONO8K_SIZE  640
#define MAX_AUDIOIN_BUFSIZE 16000
#define ADEC_UBUFFER_SIZE        (64 * 1024)
#define MAX_TIMEOUT_NUM		10

#include <alsa/asoundlib.h>

typedef struct adec_info
{
    HANDLE     	hG711DecObjALAW;
    HANDLE     	hG711DecObjULAW;
    CHAR       	*szConfigFile;
    CHAR       	szPidFile[FILEPATH_LENGTH];
    CHAR       	szDeviceNode[FILEPATH_LENGTH];
    CHAR       	szSocketPath[FILEPATH_LENGTH];
    BYTE       	*pbyInBuff;
    BYTE       	*pbyOutFrame;
    BYTE       	*pbyStereoOutFrame;
    int        	ifdUSock;
    snd_pcm_t  	*pSndPCMHandle;
    DWORD      	dwCompressionLaw;
    DWORD      	dwChannelNumber;
    BOOL		bReConf_flag;
	BOOL		bTerminal;
    sem_t       smOptions;
	DWORD		dwSampleRate;
} TAdecInfo;

#endif //define _ADEC_LOCAL_H_

