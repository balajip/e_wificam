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
 * audioin.c
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include "audioin.h"
#include "typedef.h"
#ifndef SND_PCM_TSTAMP_ENABLE /* old versions of ALSA called this  
something different */
#define SND_PCM_TSTAMP_ENABLE SND_PCM_TSTAMP_MMAP
#endif
#if 0 //Not support configfile reloading by Jeff Liao 
/* set sample rate */
inline void Set_DevSampRate(HANDLE hAudioInDev, HANDLE hAudioHWParam, int samp_rate)
{
	int	val;
	int dir=0;
    int	ret;
    snd_pcm_t* handle = (snd_pcm_t *) hAudioInDev;
    snd_pcm_hw_params_t* params = (snd_pcm_hw_params_t*) hAudioHWParam;
    
	val = samp_rate;
      
	ret = snd_pcm_hw_params_set_rate_near(handle, params, (UINT *)&val, &dir);
    if (ret < 0) 
	{
		fprintf(stderr, 	"[AENC] unable to set sample rate parameters: %s\n", snd_strerror(ret));
	}
	/* Write the parameters to the driver */
	ret = snd_pcm_hw_params(handle, params);
	if (ret < 0) 
	{
		fprintf(stderr, 	"[AENC] unable to set hw parameters: %s\n", snd_strerror(ret));
	}
	DBPRINT3("%s:%d samplerate %u\n", __func__, __LINE__, val);
}


inline void Set_DevChNum(HANDLE hAudioInDev, HANDLE hAudioHWParam, int ch_num)
{
	int	tmp;
	int	ret;
	snd_pcm_t* handle = (snd_pcm_t *) hAudioInDev;
	snd_pcm_hw_params_t* params = (snd_pcm_hw_params_t*) hAudioHWParam;
	/* set channel number */
	tmp = ch_num;
  	/* Two channels (stereo) */
	ret = snd_pcm_hw_params_set_channels(handle, params, tmp);
    if (ret < 0) 
	{
		fprintf(stderr, 	"unable to set channels parameters: %s\n", snd_strerror(ret));
	}
	/* Write the parameters to the driver */
	ret = snd_pcm_hw_params(handle, params);
	if (ret < 0) 
	{
		fprintf(stderr, 	"unable to set hw parameters: %s\n", snd_strerror(ret));
	}
}

inline void Set_DevSampFmt(HANDLE hAudioInDev, HANDLE hAudioHWParam, int samp_fmt)
{
	int	tmp;
	int	ret;
	snd_pcm_t* handle = (snd_pcm_t *) hAudioInDev;
	snd_pcm_hw_params_t* params = (snd_pcm_hw_params_t*) hAudioHWParam;
	/* set sample format */
	tmp = samp_fmt;
    ret = snd_pcm_hw_params_set_format(handle, params, tmp);
    if (ret < 0) 
	{
		fprintf(stderr, 	"unable to set format parameters: %s\n", snd_strerror(ret));
	}
	/* Write the parameters to the driver */
	ret = snd_pcm_hw_params(handle, params);
	if (ret < 0) 
	{
		fprintf(stderr, 	"unable to set hw parameters: %s\n", snd_strerror(ret));
	}
}
#endif 

#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__)	
int set_control_by_channel(const char* item, long val,char *cap_name,int  cap_num,int channelID)
{
  
	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_value_t *control;
	
	snd_hctl_open(&hctl, cap_name,cap_num);
	snd_hctl_load(hctl);	

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

	elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	snd_ctl_elem_value_set_integer(control, 0, cap_num);
	snd_ctl_elem_value_set_integer(control, 1, channelID);
	snd_ctl_elem_value_set_integer(control, 2, val);
	snd_hctl_elem_write(elem, control);

	snd_hctl_close(hctl);
	return 0;
}
int get_control_by_channel(const char* item,char *cap_name,int  cap_num,int channelID)
{
	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_value_t *control;
	int result=0;
	
	
	snd_hctl_open(&hctl, cap_name, cap_num);
	snd_hctl_load(hctl);	

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

	elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	
	snd_ctl_elem_value_set_integer(control, 0, cap_num);
	snd_ctl_elem_value_set_integer(control, 1, channelID);
	if(snd_hctl_elem_read(elem, control) < 0)
		return -1 ;

	result = snd_ctl_elem_value_get_integer(control,0) ;

	snd_hctl_close(hctl);

	return result ;
}


#endif
int set_control(const char* item, long val,char *cap_name,int  cap_num)
{

	
#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__)
	int i=0;
	//The channel num of TW2866 and NVP1114A on a device is 4
	for(i=0;i<4;i++)
	{
	  set_control_by_channel(item, val,cap_name,cap_num,i);
	}
#else
      #ifdef __SSM2603__
	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_value_t *control;
	
	//printf("%s:%d cap_name=%s cap_num=%d\n", __func__, __LINE__,cap_name,cap_num);
	//snd_hctl_open(&hctl, "hw:0", 0);
	snd_hctl_open(&hctl, cap_name, cap_num);
	snd_hctl_load(hctl);	

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

	elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	snd_ctl_elem_value_set_integer(control, 0, val);//ctrl#0
	snd_hctl_elem_write(elem, control);

	snd_hctl_close(hctl);
      #endif
#endif
	return 0;
}

int get_control(const char* item,char *cap_name,int  cap_num)
{
	int result = -1;

#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__)
	int i=0;
	//The channel num of TW2866 and NVP1114A on a device is 4
	for(i=0;i<4;i++)
	{
	  printf("The capture volume of dev#%d, capture channel#%d is %d\n", cap_num, i,get_control_by_channel(item,cap_name,cap_num,i));
	  
	}
	return 0;
#else
    #ifdef __SSM2603__
	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_value_t *control;

	//printf("%s:%d cap_name=%s cap_num=%d\n", __func__, __LINE__,cap_name,cap_num);

	//snd_hctl_open(&hctl, "hw:0", 0);
	snd_hctl_open(&hctl, cap_name, cap_num);
	snd_hctl_load(hctl);	

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

	elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	if(snd_hctl_elem_read(elem, control) < 0)
		return -1 ;

	result = snd_ctl_elem_value_get_integer(control,0);
	snd_hctl_close(hctl);
    #endif	    
	
#endif
	
	return result;
}


int set_control_double_r(const char* item, long val, char *cap_name,int  cap_num)
{
	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_value_t *control;
	
	snd_hctl_open(&hctl, cap_name, cap_num);
	snd_hctl_load(hctl);	

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

	elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	snd_ctl_elem_value_set_integer(control, 0, val);//ctrl#0
	snd_ctl_elem_value_set_integer(control, 1, val);//ctrl#0
	snd_hctl_elem_write(elem, control);

	snd_hctl_close(hctl);
	return 0;
}

SCODE set_mute(BOOL bMute,char *node,int inputSel)
{
	char *cap_name = strtok(node, ",");
	int cap_num = strtol(strtok(NULL,","), NULL, 10);
#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__)
	//The following code  does not work !! 
	if (bMute == TRUE) 
	{
	  set_control("Capture Volume", 0,cap_name,cap_num);
	}
	else
	{
	  printf("%s:%d:Not set as mute\n", __func__, __LINE__);
	}
#else
	if (bMute == TRUE) {
		if(inputSel == 1) {
			//0 : BYPASS, 1 : Line-in, 2 : Mic-in
			set_control("Input Selection", 1, cap_name, cap_num);
			//The capture volume control is just for Line-in.		
			set_control("Capture Volume", 0, cap_name, cap_num);	
			printf("%s:%d (Line-in) Mute Enable, Capture Volume = %d\n", __func__, __LINE__,get_control("Capture Volume",cap_name,cap_num));
		} else if(inputSel == 2) {
			//0 : BYPASS, 1 : Line-in, 2 : Mic-in
			set_control("Input Selection", 2, cap_name, cap_num);
			// ------------- MIC-IN ----------------
			//= ssm2603 Mic Mute =
			//0 : no mute, 1 : mute
			set_control("Mic Mute", 1, cap_name, cap_num);
			printf("%s:%d (MIC) Mute Enable=%d\n", __func__, __LINE__, get_control("Mic Mute", cap_name, cap_num));
		} else {
			printf("%s:%d Not support Input:%d \n", __func__, __LINE__,inputSel);
		}
	} else {
		printf("%s:%d:Not set as mute\n", __func__, __LINE__);
	}
#endif	
	return S_OK;
}

SCODE set_volume(int iVolume,char *node,int inputSel)
{
	char *cap_name = strtok(node,",");
	int cap_num = strtol(strtok(NULL,","), NULL, 10);
#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__)
	//The volume value of TW2866 and NVP1114A is 0~15.
	if(iVolume>15)
	{
	  iVolume=15;
	}
	set_control("Capture Volume", iVolume,cap_name,cap_num);
	get_control("Capture Volume", cap_name, cap_num);
#elif defined(__WAU8822__) || defined(__WM8978__) || defined(__AIC3104__) || defined(__ALC5623__)
	
	// Input Volume
	//  0 : Mute
	//  1 ~ 64 : -12dB ~ +35.25dB
	set_control_double_r("Input Volume", iVolume,cap_name,cap_num);
#if 0
	// Input Boost
	// 0 : +  0dB
	// 1 : + 20dB
	set_control_double_r("Input Boost", 1);
#endif
	// Input Type
	// 0 : mic-in
	// 1 : line-in
	set_control("Input Type", inputSel, cap_name, cap_num);
	
#else
	if (inputSel == 1) {
		//0 : BYPASS, 1 : Line-in, 2 : Mic-in
		set_control("Input Selection", 1,cap_name,cap_num);
		//The capture volume control is just for Line-in.
		// ------------- LINE-IN ----------------
		//= ssm2603 capture volume =
		//1 ~ 46 : normal
		//0 : mute
		set_control("Capture Volume", iVolume, cap_name, cap_num);
		//printf("%s:%d Line-in:%d \n", __func__, __LINE__, inputSel);	
		//printf("%s:%d  Capture Volume = %d\n", __func__, __LINE__, get_control("Capture Volume", cap_name, cap_num));
	} else if(inputSel == 2) {	//0 : BYPASS, 1 : Line-in, 2 : Mic-in
		set_control("Input Selection", 2, cap_name, cap_num);
		// ------------- MIC-IN ----------------
		//= ssm2603 Mic Mute =
		//0 : no mute, 1 : mute
		set_control("Mic Mute", 0, cap_name, cap_num);
		//printf("[audioin.c]%s:%d MIC-in:%d \n", __func__, __LINE__, inputSel);
	} else {
		printf("%s:%d Not support Input:%d \n", __func__, __LINE__, inputSel);
	}
#endif
	return S_OK;
}

SCODE open_audioin(HANDLE* phAudioInDev, HANDLE* phAudioHWParam, char *node, DWORD fourcc, int samp_rate, int ch_num, int samp_fmt,BOOL bMute,int iVolume,int inputSel,EAudioFrameReadType eAudioFrameReadType, DWORD *pdwRingBufferSize, DWORD *pdwPeriodSize)
{
    snd_pcm_uframes_t frames = 0;   // frames per period
    int err;
	
    snd_pcm_t** phandle = (snd_pcm_t **) phAudioInDev;
    snd_pcm_hw_params_t** pparams = (snd_pcm_hw_params_t **) phAudioHWParam;
	snd_pcm_sw_params_t*  swparams_c;
    char str[128];
	
	int exact_rate;   /* Sample rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
	int periods = 0;//ch_num;  /* Number of periods*/
	
	snd_pcm_uframes_t buff_size = 0;
	
    /* Open PCM device for capturing. */
	printf("[open_audioin](%d) node = %s\n",__LINE__,node);
	
	memset(str,0x0,128);
    memcpy(str,node,strlen(node)+1); 
	
	if (bMute == TRUE) {
#if defined(__TW2866_AUDIO__) ||  defined(__NVP1114A_AUDIO__) || defined(__WAU8822__) || defined(__WM8978__) || defined(__AIC3104__) || defined(__ALC5623__)
	    set_volume(0,str,inputSel);
#else
	    set_mute(bMute,node,inputSel);
#endif	  
	} else { 
   		set_volume(iVolume,str,inputSel);
	}
	
	while ((err = snd_pcm_open(phandle, node, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
	    if (errno == EBUSY) {
		fprintf(stderr, "[%s][%d] Audio Dev busy (%s)\n", __func__,__LINE__,snd_strerror(err));
		usleep(50000);
	    } else {
		fprintf(stderr,	"unable to open pcm device: %s\n", snd_strerror(err));
		    return S_FAIL;
	    }
	}
	
	//0 = block, 1 = nonblock mode, 2 = abort 
	if (snd_pcm_nonblock(*phandle, 0) < 0)
		return S_FAIL;
	
	if((*phandle) == NULL)
	{
		printf("[open_audioin](%d)pcm handle is NULL\n",__LINE__);
		return S_FAIL;
	}
	else
	{
		printf("[open_audioin](%d)pcm handle %p\n",__LINE__ ,(*phandle));
	}
	
    if ((err = snd_pcm_hw_params_malloc(pparams)) < 0) {
	    fprintf(stderr, "Init: cannot allocate hardware parameter structure (%s) ...\n", snd_strerror(err));
	    return S_FAIL;
	}
    // Fill it in with default values. 
	if ((err = snd_pcm_hw_params_any(*phandle, *pparams)) < 0) {
		fprintf(stderr, "Init: cannot initialize hardware parameter structure (%s) ...\n", snd_strerror(err));
		return S_FAIL;
	}
	
    // Set the desired hardware parameters. 
    if (eAudioFrameReadType == eInterleavedRead)
	{	
		// Interleaved mode 
		if ((err = snd_pcm_hw_params_set_access(*phandle, *pparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		{
			fprintf(stderr, "Init: cannot set access type (%s) ...\n", snd_strerror(err));
			return S_FAIL;
		}
		
		printf("[open_audioin](%d)access type = SND_PCM_ACCESS_RW_INTERLEAVED\n",__LINE__);
	}
	else
	{
		if ((err = snd_pcm_hw_params_set_access(*phandle, *pparams, SND_PCM_ACCESS_RW_NONINTERLEAVED)) < 0)
		{
			fprintf(stderr, "Init: cannot set access type (%s) ...\n", snd_strerror(err));
			return S_FAIL;
		}
		printf("[open_audioin](%d)access type = SND_PCM_ACCESS_RW_NONINTERLEAVED\n",__LINE__);
	}
	
	// Signed 16-bit little-endian format 
	if ((err = snd_pcm_hw_params_set_format(*phandle, *pparams, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf(stderr, "Init: cannot set sample format (%s) ...\n", snd_strerror(err));
		return S_FAIL;
	}
	printf("[open_audioin](%d)bits per sample = %d (bits)\n",__LINE__,16);
	
     /* Set number of channels */
	if ((err = snd_pcm_hw_params_set_channels(*phandle, *pparams, ch_num)) < 0) {
		fprintf(stderr, "Init: cannot set channel count (%s) ...\n", snd_strerror(err));
		return S_FAIL;	
	}
	printf("[open_audioin](%d)channel number = %d\n",__LINE__,ch_num);
	
	/* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */ 
	exact_rate = samp_rate;
	if ((err = snd_pcm_hw_params_set_rate_near(*phandle, *pparams, (UINT *)&exact_rate, 0)) < 0) {
		fprintf(stderr, "Init: cannot set sample rate (%s) ...\n", snd_strerror(err));
		return S_FAIL;	
	}
	if (samp_rate != exact_rate) {
      fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n" 
                      "==> Using %d Hz instead.\n", samp_rate, exact_rate);
    }
    printf("[open_audioin](%d)sample rate = %d (sample/second)\n",__LINE__,exact_rate);
    
	//syslog(LOG_INFO, "[AENC]Capture Sample Rate : %d \n", samp_rate);
    /* Set period size to 80 frames. 
     * We set the period to 0.02sec according to codec type and sample rate.
     * Too small period (< 0.01sec) might get same timestamps while reading from device
     * */
#ifdef _ASYNC_IO_
#ifdef _RESTART_AENC_
	if ((fourcc == MAKEFOURCC('G','A','M','R')) /*|| (exact_rate == 8000)*/) 
	{
        frames = 640;
    }
    else
	{
		frames = 1024;
	}
	periods = 8;
#else
	#ifdef __GAMR_Enc__
		//GMAR needs 640 frames but AAC4 needs 1024 frames. 128 is the greatest common divisor of 1024 and 640.
		frames = 128;
		periods = 8*8;
	#else
		//AAC4, G726, or G711 just needs 1024 frames.
		frames = 1024;
		periods = 8;
	#endif
#endif
#else
	if ((fourcc == MAKEFOURCC('G','A','M','R')) /*|| (exact_rate == 8000)*/) 
	{
        frames = 160*4;
    }
    else
	{
		frames = 1024;
	}
	periods = 8;
#endif	
	
	/* Set number of periods. Periods used to be called fragments. */ 
    if (snd_pcm_hw_params_set_periods(*phandle, *pparams, periods, 0) < 0) {
      fprintf(stderr, "Error setting periods.\n");
      return S_FAIL;
    }
	printf("[open_audioin](%d)period per ring buffer = %d (periods)\n",__LINE__,periods);
	
	
	if ((err = snd_pcm_hw_params_set_period_size_near(*phandle, *pparams, &frames, 0)) < 0) {
		fprintf(stderr, "Init: cannot set period_size (%s) ...\n", snd_strerror(err));
		return S_FAIL;	
	}
	printf("[open_audioin](%d)frames per period = %d (frames)\n",__LINE__,(int)frames);
    *pdwPeriodSize = frames;
	
	
	err = snd_pcm_hw_params_get_buffer_size(*pparams, &buff_size);
	if (err < 0) {
		printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
		return err;
	}
    printf("[open_audioin](%d)ring buff size = %d (frames)\n",__LINE__,(int)buff_size);
	*pdwRingBufferSize = buff_size;
	
  	/* Write the parameters to the driver */
	if ((err = snd_pcm_hw_params(*phandle, *pparams)) < 0) {
		fprintf(stderr, "Init: cannot set parameters (%s) ...\n", snd_strerror(err));
		return S_FAIL;		
	}
	
	{
		snd_pcm_uframes_t boundary = 0;
		snd_pcm_sw_params_alloca(&swparams_c);
		/* get the current swparams */
		err = snd_pcm_sw_params_current(*phandle, swparams_c);
		if (err < 0) {
			printf("Unable to determine current swparams_c: %s\n", snd_strerror(err));
			return S_FAIL;
		}

		/* enable tstamp */
		err = snd_pcm_sw_params_set_tstamp_mode(*phandle, swparams_c, SND_PCM_TSTAMP_ENABLE);
		if (err < 0) {
			printf("Unable to set tstamp mode : %s\n", snd_strerror(err));
			return S_FAIL;
		}

		
		err = snd_pcm_sw_params_set_avail_min(*phandle, swparams_c, frames);
		if (err < 0) {
			printf("Unable to call snd_pcm_sw_params_set_avail_min(): %s\n", snd_strerror(err));
			return S_FAIL;
		}
		
		err = snd_pcm_sw_params_set_start_threshold(*phandle, swparams_c, (buff_size / frames) * frames);
		if (err < 0) {
			printf("Unable to call snd_pcm_sw_params_set_start_threshold(): %s\n", snd_strerror(err));
			return S_FAIL;
		}
		
		err =  snd_pcm_sw_params_get_boundary(swparams_c, &boundary);
		if (err < 0) {
			printf("Unable to call snd_pcm_sw_params_get_boundary(): %s\n", snd_strerror(err));
			return S_FAIL;
		}
		
			
		err = snd_pcm_sw_params_set_stop_threshold(*phandle, swparams_c, boundary);
		if (err < 0) {
			printf("Unable to call snd_pcm_sw_params_set_stop_threshold(): %s\n", snd_strerror(err));
			return S_FAIL;
		}
		
		/* align all transfers to 1 sample */
		err = snd_pcm_sw_params_set_xfer_align(*phandle, swparams_c, 1);
		if (err < 0) {
			printf("Unable to set transfer align for playback: %s\n", snd_strerror(err));
			return err;
		}
		
		/* write the sw parameters */
		err = snd_pcm_sw_params(*phandle, swparams_c);
		if (err < 0) {
			printf("Unable to set swparams_c : %s\n", snd_strerror(err));
			return S_FAIL;
		}
		
	}
    return S_OK;
}

