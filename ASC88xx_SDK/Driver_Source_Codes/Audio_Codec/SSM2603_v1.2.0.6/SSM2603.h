/*
 * Mozart_Audio_SSM2603
 * Driver for Mozart Audio Codec SSM2603.
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

#ifndef __SSM2603_H__
#define __SSM2603_H__

#define LEFT_ADC_VOLUME_REG		0x00
#define RIGHT_ADC_VOLUME_REG	0x01
#define LEFT_DAC_VOLUME_REG		0x02
#define RIGHT_DAC_VOLUME_REG	0x03
#define ANA_AUDIO_PATH_REG		0x04
#define DIG_AUDIO_PATH_REG		0x05
#define POWER_MANAGE_REG		0x06
#define DIGI_AUDIO_IF_REG		0x07
#define	SAMPLE_RATE_REG			0x08
#define ACTIVE_REG				0x09
#define	SW_RESET_REG			0x0F
#define	ALC_CTRL1_REG			0x10
#define	ALC_CTRL2_REG			0x11
#define	NOISE_GATE_REG			0x12

static int ssm2603_reg_cache[18] ={ 0 } ;

#endif

