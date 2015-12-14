/*
 * AutoWhiteBalance
 * Driver for AutoWhiteBalance.
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
#ifndef __AUTOWHITEBALANCE_DRIVER_H__
#define __AUTOWHITEBALANCE_DRIVER_H__

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
#include <asm/uaccess.h>
#include "global_codec.h"
#include "errordef.h"
#include <linux/fs.h>

#include "AutoWhiteBalance.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "AutoWhiteBalance: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
DWORD AutoWhiteBalance_GetDevInfoSize(void);
SOCKET AutoWhiteBalance_Initial(HANDLE hAWBInfo, TAutoWhiteBalanceInitialParam* ptAWBInitialParam);
SOCKET AutoWhiteBalance_SetOptions(HANDLE hAWBInfo, TVideoSignalOptions *ptArg);
SOCKET AutoWhiteBalance_OneFrame(HANDLE hAWBInfo, TAutoWhiteBalanceState *ptState);
void AutoWhiteBalance_SetColorTemperature(HANDLE hAWBInfo, DWORD *pdwParam);
/* Version 3.0.0.0 modification, 2013.06.21 */
void AutoWhiteBalance_SetBayerBitWidth(HANDLE hAWBInfo, DWORD dwBayerBitWidth);
/* ======================================== */
/* ============================================================================================== */
#endif //__AUTOWHITEBALANCE_DRIVER_H__
/* ============================================================================================== */
