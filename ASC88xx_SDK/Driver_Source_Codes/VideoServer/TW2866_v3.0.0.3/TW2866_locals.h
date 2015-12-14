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

#ifndef __TW2866_LOCALS_H__
#define __TW2866_LOCALS_H__
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <asm/arch/irq.h>
#include <asm/arch/platform.h>
#include <linux/mutex.h> // spinlock
#include "vivo_codec.h"
#include "video_error.h"


/* ============================================================================
*/
/* Version 2.0.0.5 modification, 2010.11.23 */	
#define TW2866_TBL_SIZE 30
#define TW2866_STD_TBL_SIZE	2
/* ========================================= */

#define TW2866_DEFAULT_OUTPUT_WIDTH	1920
#define TW2866_DEFAULT_OUTPUT_HEIGHT	1080

typedef SCODE (* FOnReadReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAdd, DWORD *pdwData);
typedef SCODE (* FOnWriteReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwData);

typedef struct TW2866_reg_addr_data
{
    DWORD dwRegAddr;
    DWORD dwData;
} TTW2866RegAddrData;

typedef struct TW2866_info
{
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadReg_t pfnReadReg;
	FOnWriteReg_t pfnWriteReg;
} TTW2866Info;

