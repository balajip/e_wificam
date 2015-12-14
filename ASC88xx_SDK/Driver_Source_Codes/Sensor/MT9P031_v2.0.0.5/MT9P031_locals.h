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

#ifndef __MT9P031_LOCALS_H__
#define __MT9P031_LOCALS_H__
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
#define MT9P031_CHIP_VERSION 0x1801
/* Version 1.0.0.7 modification, 2010.11.01 */
#define MT9P031_PROGRESS_RAW_TBL_SIZE 8
/* ========================================= */
/* Version 1.0.0.6 modification, 2010.08.23 */
#define MT9P031_SHUTTER_WIDTH_LOWER 1131
/* ========================================= */

#define MT9P031_DEFAULT_OUTPUT_WIDTH	1920
#define MT9P031_DEFAULT_OUTPUT_HEIGHT	1080

/* Version 2.0.0.3 modification, 2012.08.06 */
#define MT9P031_MAX_GAIN 128000
/* ======================================== */

/* Version 2.0.0.0 modification, 2011.01.21 */
#define MT9P031_PIXEL_CLOCK				96000000
#define MT9P031_PIXEL_CLOCK_M				(MT9P031_PIXEL_CLOCK/1000000)

/* Version 2.0.0.1 modification, 2012.01.05 */
#define MT9P031_HSYNC_2560x1920			3462
#define MT9P031_VSYNC_2560x1920_15FPS	1945
#define MT9P031_VSYNC_2560x1920_12FPS	2309

#define MT9P031_HSYNC_1920x1080			2822
#define MT9P031_VSYNC_1920x1080_30FPS	1132
#define MT9P031_VSYNC_1920x1080_25FPS	1359

#define MT9P031_HSYNC_1600x1200			2502
#define MT9P031_VSYNC_1600x1200_30FPS	1277
#define MT9P031_VSYNC_1600x1200_25FPS	1533

#define MT9P031_HSYNC_1280x1024			2182
#define MT9P031_VSYNC_1280x1024_30FPS	1465
#define MT9P031_VSYNC_1280x1024_25FPS	1758

#define MT9P031_HSYNC_1280x720			2834
#define MT9P031_VSYNC_1280x720_30FPS		1128
#define MT9P031_VSYNC_1280x720_25FPS		1353

#define MT9P031_HSYNC_800x600				2354
#define MT9P031_VSYNC_800x600_30FPS		1358
#define MT9P031_VSYNC_800x600_25FPS		1630

#define MT9P031_HSYNC_640x480				3558
#define MT9P031_VSYNC_640x480_60FPS		898
#define MT9P031_VSYNC_640x480_50FPS		1078
/* ========================================= */

/* ======================================== */

typedef enum MT9P031_registers
{
	_00_ChipVersion					= 0x00,
	_01_RowStart					= 0x01,
	_02_ColumnStart					= 0x02,
	_03_RowSize						= 0x03,
	_04_ColumnSize					= 0x04,
	_05_HorzBlanking				= 0x05,
	_06_VertBlanking				= 0x06,
	_07_OutputCtrl					= 0x07,
	_08_ShutterWidthUpper			= 0x08,
	_09_ShutterWidthLower			= 0x09,
	_0A_PixClkCtrl					= 0x0A,
	_0B_Restart						= 0x0B,
	_0C_ShutterDelay				= 0x0C,
	_0D_Reset						= 0x0D,
	_10_PLLCtrl						= 0x10,
	_11_PLLConfigl					= 0x11,
	_12_PLLConfig2					= 0x12,
	_1E_ReadMode1					= 0x1E,
	_20_ReadMode2					= 0x20,
	_22_RowAddrMode					= 0x22,
	_23_ColumnAddrMode				= 0x23,
	_2B_Green1Gain					= 0x2B,
	_2C_BlueGain					= 0x2C,
	_2D_RedGain						= 0x2D,
	_2E_Green2Gain					= 0x2E,
	_35_GlobalGain					= 0x35,
	_A0_TestPatternCtrl				= 0xA0,
	_A1_TestPatternGreen			= 0xA1,
	_A2_TestPatternRead				= 0xA2,
	_A3_TestPatternBlue				= 0xA3,
	_A4_TestPatternBarWidth			= 0xA4,
	_FF_ChipVersionAlt				= 0xFF
} EMT9P031Regs;

typedef struct MT9P031_reg_addr_data
{
    EMT9P031Regs eRegAddr;
    DWORD dwData;
} TMT9P031RegAddrData;

typedef struct MT9P031_info
{
	spinlock_t lock;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadReg pfnReadReg;
	FOnWriteReg pfnWriteReg;

/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwFreq;
	
/* Version 2.0.0.2 modification, 2012.02.09 */
	DWORD dwShutter;
/* ======================================== */
} TMT9P031Info;

