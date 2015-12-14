/*
 *
 * Copyright (C) 2010  VN Inc.
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
 *
 */

/* ============================================================================================== */
#ifndef __PWMIRISCONTROL_LOCALS_H__
#define __PWMIRISCONTROL_LOCALS_H__

/* ============================================================================================== */
#include "vivo_codec.h"
#include "mach/agpo.h"

/* ============================================================================================== */
typedef enum dciris_aperture_sz
{
    DCIRIS_APERTURE_CLOSE = 0,
    DCIRIS_APERTURE_MID = 1,
    DCIRIS_APERTURE_OPEN = 2,
} EDCIrisApertureSz;

typedef enum dciris_speed
{
    DCIRIS_ZERO_SPEED = 0,
    DCIRIS_SLOWEST_SPEED = 1,
    DCIRIS_SLOW_SPEED = 2,
    DCIRIS_NORMAL_SPEED = 3,
    DCIRIS_FAST_SPEED = 4,
    DCIRIS_FASTEST_SPEED = 5,
    DCIRIS_SPEED_NUM = 6,
} EDCIrisSpeed;

typedef struct pwm_iris_control_info
{
	AGPO_Info tAGPOInfo;

	DWORD dwAISpeed;

	DWORD dwAgpoCurrNode;
} TPWMIrisControlInfo;
/* ============================================================================================== */
#endif //__PWMIRISCONTROL_LOCALS_H__
