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

#ifndef __IMX035_LOCALS_H__
#define __IMX035_LOCALS_H__
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
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 2.0.0.2 modification, 2011.09.16 */
#define IMX035_PROGRESS_RAW_TBL_SIZE 17
/* ======================================== */
/* ======================================== */

#define IMX035_DEFAULT_OUTPUT_WIDTH	1280
#define IMX035_DEFAULT_OUTPUT_HEIGHT	1024

/* Version 2.0.0.3 modification, 2012.09.04 */
#define IMX035_MAX_GAIN 64000
/* ======================================== */

/* Version 2.0.0.0 modification, 2011.01.21 */
#define IMX035_PIXEL_CLOCK	54000000
#define IMX035_PIXEL_CLOCK_M	(IMX035_PIXEL_CLOCK/1000000)
#define IMX035_DELTA_1280x1024_12BIT	640

#define IMX035_HSYNC_1280x1024_60Hz	1600
#define IMX035_VSYNC_1280x1024_60Hz	1125

#define IMX035_HSYNC_1280x1024_50Hz	1728
#define IMX035_VSYNC_1280x1024_50Hz	1250
/* ======================================== */

typedef enum IMX035_registers
{
	_00_Stby				= 0x00,
	_01_Mode				= 0x01,
	_03_ADRES				= 0x03,
	_04_FRSEL				= 0x04,
	_05_SSBRK				= 0x05,
	_06_SVSL				= 0x06,
	_07_SVSH				= 0x07,
	_08_SHS1L				= 0x08,
	_09_SHS1H				= 0x09,
	_12_SPL1L				= 0x12,
	_13_SPL1H				= 0x13,
	_16_WINPHL				= 0x16,
	_17_WINPHH_WINPVL	= 0x17,
	_18_WINPVH			= 0x18,
	_19_WINWHL			= 0x19,
	_1A_WINWHH_WINWVL	= 0x1A,
	_1B_WINWVH			= 0x1B,
	_1C_AnalogGainL		= 0x1C,  
	_1D_AnalogGainH		= 0x1D,  
	_1E_DigitalGain			= 0x1E,
	_1F_BlackLevelL			= 0x1F,
	_20_BlackLevelH			= 0x20,
	_26_XMSTA				= 0x26,
	_53_XHSLNG_XVSLNG		= 0x53,
	_62_Reg62				= 0x62,
	_7C_VMAXL				= 0x7C,
	_7D_VMAXH				= 0x7D,
	_7E_HMAXL				= 0x7E,
	_7F_HMAXH				= 0x7F,
	_90_Reg90				= 0x90,
	_FE_RegFE				= 0xFE,
} EIMX035Regs;

typedef struct IMX035_reg_addr_data
{
    EIMX035Regs eRegAddr;
    DWORD dwData;
} TIMX035RegAddrData;

typedef struct IMX035_info
{
	spinlock_t lock;

	DWORD bInit;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnWriteBuf pfnWriteBuf;

/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	DWORD dwFreq;

	DWORD dwCurShutter;
	
} TIMX035Info;

