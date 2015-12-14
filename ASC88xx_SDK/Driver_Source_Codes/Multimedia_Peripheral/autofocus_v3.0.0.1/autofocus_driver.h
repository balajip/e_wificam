/*
 * autofocus
 * Driver for autofocus.
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
#ifndef __AUTOFOCUS_DRIVER_H__
#define __AUTOFOCUS_DRIVER_H__

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
#include <linux/string.h>
#include <asm/io.h>
#include "vivo_codec.h"
#include "errordef.h"

#include "autofocus.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "AutoFocus: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
typedef struct autofocus_shared_info
{
	HANDLE hAFInfo;
	HANDLE hZtInfo;
} TAutofocusSharedInfo;

/* ============================================================================================== */
DWORD AutoFocus_GetDevInfoSize(void);
SOCKET AutoFocus_Initial(TAutofocusSharedInfo* ptSharedInfo , TAutoFocusInitOptions* ptAFInitialParam);
SOCKET AutoFocus_Reset(TAutofocusSharedInfo* ptSharedInfo , TAutoFocusInitOptions* ptAFInitialParam);
SOCKET AutoFocus_SetOptions(TAutofocusSharedInfo* ptSharedInfo , TVideoSignalOptions *ptArg);
SOCKET AutoFocus_OneFrame(TAutofocusSharedInfo* ptSharedInfo , TAutoFocusState *ptState);
SCODE AutoFocus_WaitComplete(TAutofocusSharedInfo* ptSharedInfo);
DWORD AutoFocus_GetCalibrateSize(TAutofocusSharedInfo* ptSharedInfo);
/* Version 3.0.0.1 modification, 2013.01.10 */
DWORD AutoFocus_GetZtInfoSize(TAutofocusSharedInfo* ptSharedInfo);
/* ======================================== */
/* ============================================================================================== */
#endif //__AUTOFOCUS_DRIVER_H__
/* ============================================================================================== */
