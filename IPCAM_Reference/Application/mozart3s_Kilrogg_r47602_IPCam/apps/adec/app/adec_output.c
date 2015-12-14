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
 * adec_output.c
 *
 * \brief
 * Adec audio out device handler implement.
 *
 * \date
 * 2008/3/24
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#include "adec_local.h"
#include <sys/time.h>

void Adec_PrintALSAStatus(snd_pcm_t *handle)
{
	int err;
	snd_pcm_status_t *status;
	snd_pcm_uframes_t frames;
	
	snd_pcm_status_alloca(&status);
	if ((err = snd_pcm_status(handle, status)) < 0) 
	{
    	printf("Stream status error: %s\n", snd_strerror(err));
        exit(0);
    }
    
    frames = snd_pcm_status_get_avail(status);
    printf("number of frames available from a PCM status container: %ld\n", frames);
}

/* ========================================================================== */
SCODE Adec_ResetAudioOut(TAdecInfo *ptAdecInfo)
{
    int rc;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    unsigned int exact_rate;
    unsigned int rate = ptAdecInfo->dwSampleRate;
    int dir = 0;

    handle = ptAdecInfo->pSndPCMHandle;

	snd_pcm_drop(handle);
	snd_pcm_reset(handle);

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */
    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    /* 44100 bits/second sampling rate (CD quality) */
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(handle, params, &exact_rate, &dir) < 0)
    {
		fprintf(stderr, "Error setting rate.\n");
    }
    if (rate != exact_rate) {
      fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set period size to 32 frames. */
    frames = 64;
    if (snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir) < 0)
    {
    	printf("snd_pcm_hw_params_set_period_size_near < 0 \n");
    }

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0)
    {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    return S_OK;
}

/* ========================================================================== */
SCODE Adec_InitAudioOut(TAdecInfo *ptAdecInfo)
{
    int rc;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    unsigned int exact_rate;
    unsigned int rate = ptAdecInfo->dwSampleRate;
    int dir = 0;

    rc = snd_pcm_open(&ptAdecInfo->pSndPCMHandle, ptAdecInfo->szDeviceNode, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0)
    {
        fprintf(stderr,     "unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }

    handle = ptAdecInfo->pSndPCMHandle;

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */
    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    /* 44100 bits/second sampling rate (CD quality) */
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(handle, params, &exact_rate, &dir) < 0)
    {
		fprintf(stderr, "Error setting rate.\n");
    }
    if (rate != exact_rate) {
      fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set period size to 32 frames. */
    frames = 64;
    if (snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir) < 0)
    {
    	printf("snd_pcm_hw_params_set_period_size_near < 0 \n");
    }

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0)
    {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    return S_OK;
}

/* ========================================================================== */
SCODE Adec_WriteAudioOut(TAdecInfo *ptAdecInfo, BYTE *pbyOutFrame, DWORD dwOutSize)
{
    int rc;
    int frames;
    BYTE *pbyDummy;
    frames = (dwOutSize>>2);

    rc = snd_pcm_writei(ptAdecInfo->pSndPCMHandle, pbyOutFrame, frames);
    if (rc == -EPIPE)
    {
        // EPIPE means underrun 
        pbyDummy = calloc(4, frames*4);
        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(ptAdecInfo->pSndPCMHandle);
        snd_pcm_writei(ptAdecInfo->pSndPCMHandle, pbyDummy, frames*4);
        free(pbyDummy);
    }
    else if (rc < 0)
    {
        fprintf(stderr, "error from writei: %s\n",
        snd_strerror(rc));
        return S_FAIL;
    }
    else if (rc != (int)frames)
    {
        fprintf(stderr, "short write, write %d frames\n", rc);
        return S_FAIL;
    }
    return S_OK;
}
