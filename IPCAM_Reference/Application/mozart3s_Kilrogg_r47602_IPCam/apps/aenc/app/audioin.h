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
 * audioin.h
 *
 * \brief
 * Audio in module
 *
 * \date
 * 2006/07/19
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */
#ifndef _AUDIOIN_H_
#define _AUDIOIN_H_
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include "errordef.h"
#include "dbgdefs.h"
#include "typedef.h"
#include "global_codec.h"
#include "aenc.h"
#define VIDEO_SERVER_AUDIO_CHANNEL 4
#if 0 //Not support configfile reloading by Jeff Liao 
inline void Set_DevSampRate(HANDLE hAudioInDev, HANDLE hAudioHWParam, int samp_rate);
inline void Set_DevChNum(HANDLE hAudioInDev, HANDLE hAudioHWParam, int ch_num);
inline void Set_DevSampFmt(HANDLE hAudioInDev, HANDLE hAudioHWParam, int samp_fmt);
#endif

SCODE open_audioin(HANDLE* phAudioInDev, HANDLE* phAudioHWParam, char *node, DWORD fourcc, int samp_rate, int ch_num, int samp_fmt,BOOL bMute,int iVolume,int inputSel,EAudioFrameReadType eAudioFrameReadType, DWORD *pdwRingBufferSize, DWORD *pdwPeriodSize);

SCODE set_volume(int iVolume,char *node,int inputSel);
SCODE set_mute(BOOL bMute,char *node,int inputSel);

#endif //_AUDIOIN_H_

