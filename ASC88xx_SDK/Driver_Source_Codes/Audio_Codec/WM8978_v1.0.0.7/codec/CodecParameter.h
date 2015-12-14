/*
 * Mozart_Audio_WM8978
 * Driver for Mozart WM8978 Audio Codec Driver
 *
 * Copyright (C) 2010  ______ Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CODECPARAMETER_H__
#define __CODECPARAMETER_H__

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

//Please fill these variables in codec-driver source code.
//These will be referenced in machine-driver source code.
extern unsigned long vpl_codec_param_mclksel_reg ;
extern unsigned long vpl_codec_param_mclkdiv_reg ;
extern unsigned long vpl_codec_param_bclkdiv_reg ;
extern unsigned long vpl_codec_param_bclkdiv_val ;
extern char vpl_codec_param_name[] ;

struct vpl_codec_param_setup_data {
	unsigned short i2c_address;
};

//Please implement these variables in codec-driver src code.
extern struct snd_soc_dai vpl_codec_param_codec_dai;
extern struct snd_soc_codec_device vpl_codec_param_codec_dev;

#endif

