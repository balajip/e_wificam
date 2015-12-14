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
 * aenc_conftree.h
 *
 * \brief
 * Audio encoder xmlwrapper treemap and function decleration
 *
 * \date
 * 2006/12/01
 *
 * \author
 * Ming Jing Tsai
 *
 *
 *******************************************************************************
 */

#ifndef _AENC_CONF_TREE_H_
#define _AENC_CONF_TREE_H_
#include "xmlwrapper.h"
//=====================================================================================

/* XML_StartElementHandler */
SCODE AxeXmlTree_Device(void *userData, const char *name, const char **atts);
SCODE AxeXmlTree_IO(void *userData, const char *name, const char **atts);
//SCODE AxeXmlTree_IO_Output_ID(void *userData, const char *name, const char **atts);
SCODE AxeXmlTree_IO_ID(void *userData, const char *name, const char **atts);

/* XML_CharacterDataHandler */
/* Dev */
void AxeXmlTree_Node(void *userData, const char *name, int len);
void AxeXmlTree_ReadType(void *userData, const char *name, int len);
void AxeXmlTree_ChNum(void *userData, const char *name, int len);
void AxeXmlTree_SampFmt(void *userData, const char *name, int len);
void AxeXmlTree_InputSelection(void *userData, const char *name, int len);
void AxeXmlTree_Mute(void *userData, const char *name, int len);
void AxeXmlTree_Volume(void *userData, const char *name, int len);


/* Dev Codec */
void AxeXmlTree_Codec(void *userData, const char *name, int len);
void AxeXmlTree_Bitrate(void *userData, const char *name, int len);
void AxeXmlTree_SampRate(void *userData, const char *name, int len);
void AxeXmlTree_ChannelNum(void *userData, const char *name, int len);

/* IO */
void AxeXmlTree_IO_Input(void *userData, const char *name, int len);
void AxeXmlTree_IO_Output(void *userData, const char *name, int len);

/* Codec */
void AxeXmlTree_Codec_Bitrate(void *userData, const char *name, int len);
void AxeXmlTree_Codec_SampRate(void *userData, const char *name, int len);
void AxeXmlTree_Codec_ChNum(void *userData, const char *name, int len);

#ifdef __AAC4_Enc__
/*aenc_aac4.c*/
SCODE AxeXmlTree_AAC4(void *userData, const char *name, const char **atts);
//void AxeXmlTree_AAC4_ChNum(void *userData, const char *name, int len);
#endif

#ifdef __GAMR_Enc__
/*aenc_gamr.c*/
SCODE AxeXmlTree_GAMR(void *userData, const char *name, const char **atts);
void AxeXmlTree_GAMR_FrPerBuf(void *userData, const char *name, int len);
#endif

#ifdef __G711_Enc__
/*aenc_g711.c*/
SCODE AxeXmlTree_G711(void *userData, const char *name, const char **atts);
void AxeXmlTree_G711_Mode(void *userData, const char *name, int len);
#endif

#ifdef __G726_Enc__
/*aenc_g726.c*/
SCODE AxeXmlTree_G726(void *userData, const char *name, const char **atts);
#endif
//=====================================================================================

TXmlWrapperTreeMap ptTreeMap[] = 
{	
	
	/* Device */
	{"root/audio_encoder/devices/device", &AxeXmlTree_Device, NULL, NULL},
	{"root/audio_encoder/devices/device/read_type", NULL, &AxeXmlTree_ReadType, NULL},
	{"root/audio_encoder/devices/device/channel_number", NULL, &AxeXmlTree_ChNum, NULL},
	{"root/audio_encoder/devices/device/sample_format", NULL, &AxeXmlTree_SampFmt, NULL},
	{"root/audio_encoder/devices/device/input_selection",NULL,&AxeXmlTree_InputSelection,NULL},
	{"root/audio_encoder/devices/device/mute",NULL,&AxeXmlTree_Mute,NULL},
	{"root/audio_encoder/devices/device/volume",NULL,&AxeXmlTree_Volume,NULL},

	/* Codec Type */
	{"root/audio_encoder/devices/device/codec/type", NULL, &AxeXmlTree_Codec, NULL},
	{"root/audio_encoder/devices/device/codec/bitrate", NULL, &AxeXmlTree_Bitrate, NULL},
	{"root/audio_encoder/devices/device/codec/sample_rate", NULL, &AxeXmlTree_SampRate, NULL},
	{"root/audio_encoder/devices/device/codec/channel_number", NULL, &AxeXmlTree_ChannelNum, NULL},
#ifdef __G711_Enc__
	{"root/audio_encoder/devices/device/codec/mode", NULL, &AxeXmlTree_G711_Mode, NULL},
#endif

	/* IO list */
	{"root/audio_encoder/ios/io", &AxeXmlTree_IO_ID, NULL, NULL},
	{"root/audio_encoder/ios/io/input", &AxeXmlTree_IO, &AxeXmlTree_IO_Input, NULL},
	{"root/audio_encoder/ios/io/output", NULL, &AxeXmlTree_IO_Output, NULL},
	
#if 0	//Not support configfile reloading by Jeff Liao 
#ifdef __AAC4_Enc__
	/*AAC4*/
	{"root/audio_encoder/codec/AAC4", &AxeXmlTree_AAC4, NULL, NULL},
	{"root/audio_encoder/codec/AAC4/bitrate", NULL, &AxeXmlTree_Codec_Bitrate, NULL},
	{"root/audio_encoder/codec/AAC4/sample_rate", NULL, &AxeXmlTree_Codec_SampRate, NULL},
	{"root/audio_encoder/codec/AAC4/channel_number", NULL, &AxeXmlTree_Codec_ChNum, NULL},
#endif
#ifdef __GAMR_Enc__
	/*GAMR*/	
	{"root/audio_encoder/codec/GAMR", &AxeXmlTree_GAMR, NULL, NULL},
	{"root/audio_encoder/codec/GAMR/bitrate", NULL, &AxeXmlTree_Codec_Bitrate, NULL},
	{"root/audio_encoder/codec/GAMR/sample_rate", NULL, &AxeXmlTree_Codec_SampRate, NULL},	
	{"root/audio_encoder/codec/GAMR/frame_per_buffer", NULL, &AxeXmlTree_GAMR_FrPerBuf, NULL},
#endif
	
	/*G711*/
#ifdef __G711_Enc__
	{"root/audio_encoder/codec/G711", &AxeXmlTree_G711, NULL, NULL},	
	{"root/audio_encoder/codec/G711/mode", NULL, &AxeXmlTree_G711_Mode, NULL},
//	{"root/audio_encoder/codec/G711/bitrate", NULL, &AxeXmlTree_Codec_Bitrate, NULL},	
	{"root/audio_encoder/codec/G711/sample_rate", NULL, &AxeXmlTree_Codec_SampRate, NULL},	
	{"root/audio_encoder/codec/G711/channel_number", NULL, &AxeXmlTree_Codec_ChNum, NULL},
#endif

	/*G726*/
#ifdef __G726_Enc__
	{"root/audio_encoder/codec/G726", &AxeXmlTree_G726, NULL, NULL},
	{"root/audio_encoder/codec/G726/bitrate", NULL, &AxeXmlTree_Codec_Bitrate, NULL},
	{"root/audio_encoder/codec/G726/sample_rate", NULL, &AxeXmlTree_Codec_SampRate, NULL},
	{"root/audio_encoder/codec/G726/channel_number", NULL, &AxeXmlTree_Codec_ChNum, NULL},
#endif
#endif
	{NULL, NULL, NULL}
};

#endif //_AENC_CONF_TREE_H_
