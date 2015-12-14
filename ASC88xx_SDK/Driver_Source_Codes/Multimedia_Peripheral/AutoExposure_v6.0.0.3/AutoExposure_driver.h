/*
 * AutoExposure
 * Driver for AutoExposure.
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
 *
 */

/* ============================================================================================== */
#ifndef __AUTOEXPOSURE_DRIVER_H__
#define __AUTOEXPOSURE_DRIVER_H__

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

/* ============================================================================================== */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include "global_codec.h"
#include <asm/uaccess.h>
#include "errordef.h"
#include <linux/fs.h>

#include "AutoExposure.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "AutoExposure: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
DWORD AutoExposure_GetDevInfoSize(void);
SOCKET AutoExposure_Initial(HANDLE hAEInfo, TAutoExposureInitialParam* ptAEInitialParam);
SOCKET AutoExposure_SetOptions(HANDLE hAEInfo, TVideoSignalOptions *ptArg);
void AutoExposure_OneFrame(HANDLE hAEInfo, TAutoExposureState *ptState);
void AutoExposure_SetBayerBitWidth(HANDLE hAEInfo, DWORD dwBayerBitWidth);
/* ============================================================================================== */
#endif //__AUTOEXPOSURE_DRIVER_H__
/* ============================================================================================== */
