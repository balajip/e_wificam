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

#ifndef __OV2710_LOCALS_H__
#define __OV2710_LOCALS_H__
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
#define OV2710_PID 0x2710
#define OV2710_TBL_SIZE 57
#define OV2710_NO_DSP_TBL_SIZE 65
/* Version 1.1.0.4 modification, 2010.10.13 */
#define OV2710_WB_GAIN_UNIT 1024
/*=========================================*/

#define OV2710_DEFAULT_OUTPUT_WIDTH	1920
#define OV2710_DEFAULT_OUTPUT_HEIGHT	1080

/* Version 2.0.0.4 modification, 2012.08.06 */
#define OV2710_MAX_GAIN 64000
/*=========================================*/

/* Version 2.0.0.0 modification, 2011.01.21 */
#define OV2710_PIXEL_CLOCK	80000000
#define OV2710_PIXEL_CLOCK_M	(OV2710_PIXEL_CLOCK/1000000)

/* Version 2.0.0.2 modification, 2011.07.19 */
#define OV2710_HSYNC_1920x1080_50Hz	2904
/*=========================================*/
#define OV2710_HSYNC_1920x1080_60Hz	2420
#define OV2710_VSYNC_1920x1080	1104

#define OV2710_HSYNC_1280x720	1792
#define OV2710_VSYNC_1280x720	744

#define OV2710_HSYNC_640x480	1344
#define OV2710_VSYNC_640x480	480

#define OV2710_HSYNC_320x240	1324
#define OV2710_VSYNC_320x240	240
/* ======================================== */

typedef enum OV2710_registers
{
	_3008_SystemControl    	= 0x3008,
	_300A_PIDH				= 0x300A,
	_300B_PIDL				= 0x300B,
	_300F_PLLControl00    	= 0x300F,
	_3011_PLLControl02    	= 0x3011,
	_3012_PLLPreDivider		= 0X3012,
	_3017_PADOutputEnable01	= 0x3017,
	_3018_PADOutputEnable02	= 0x3018,
	_302C_PADOutputDrv		= 0x302C,
	_302D_               	= 0x302D,
	_3103_PLLClockSelect 	= 0x3103,
	_3200_GroupAddr0		= 0x3200,
	_3201_GroupAddr1		= 0x3201,
	_3202_GroupAddr2		= 0x3202,
	_3203_GroupAddr3		= 0x3203,
	_3212_GroupAccess		= 0x3212,
	_3400_RedGainH			= 0x3400,
	_3401_RedGainL			= 0x3401,
	_3402_GreenGainH		= 0x3402,
	_3403_GreenGainL		= 0x3403,
	_3404_BlueGainH			= 0x3404,
	_3405_BlueGainL			= 0x3405,
	_3406_AWBManualCtrl		= 0x3406,
	_3500_AECExposureH    	= 0x3500,
	_3501_AECExposureM    	= 0x3501,
	_3502_AECExposureL    	= 0x3502,
	_3503_AECManualMode    	= 0x3503,
	_350A_AECManualGainH	= 0x350A,
	_350B_AECManualGainL	= 0x350B,
	_350C_AECPKVTS			= 0x350C,
	_350D_AECPKVTS			= 0x350D,
	_3600_                 	= 0x3600,
	_3603_                 	= 0x3603,
	_3604_                 	= 0x3604,
	_3605_					= 0x3605,
	_3606_					= 0x3606,
	_3620_					= 0x3620,
	_3621_ArrayControl     	= 0x3621,
	_3623_					= 0x3623,
	_3630_                 	= 0x3630,
	_3631_                 	= 0x3631,
	_3702_                 	= 0x3702,
	_3703_                 	= 0x3703,
	_3704_                 	= 0x3704,
	_3706_                 	= 0x3706,
	_370B_                 	= 0x370B,
	_370D_SensorReg0D      	= 0x370D,
	_3710_                 	= 0x3710,
	_3712_                 	= 0x3712,
	_3713_                 	= 0x3713,
	_3714_                 	= 0x3714,
	_3801_HREFStartXLow    	= 0x3801,
	_3808_HorzWidthHigh    	= 0x3808,
	_3809_HorzWidthLow     	= 0x3809,
	_380A_VertHeightHigh   	= 0x380A,
	_380B_VertHeightLow    	= 0x380B,
	_380C_HorzTotalHigh    	= 0x380C,
	_380D_HorzTotalLow     	= 0x380D,
	_380E_VertTotalHigh    	= 0x380E,
	_380F_VertTotalLow     	= 0x380F,
	_381A_                 	= 0x381A,
	_382E_                 	= 0x382E,
	_3A00_AECControl00     	= 0x3A00,
	_3A02_AECMaxExpo60H    	= 0x3A02,
	_3A03_AECMaxExpo60M    	= 0x3A03,
	_3A04_AECMaxExpo60L    	= 0x3A04,
	_3A05_               	= 0x3A05,
	_3A08_AECB50StepHigh   	= 0x3A08,
	_3A09_AECB50StepLow    	= 0x3A09,
	_3A0A_AECB60StepHigh   	= 0x3A0A,
	_3A0B_AECB60StepLow    	= 0x3A0B,
	_3A0D_AECControl0D		= 0x3A0D,
	_3A0E_AECControl0E		= 0x3A0E,
	_3A0F_AECControl0F     	= 0x3A0F,
	_3A10_AECControl10     	= 0x3A10,
	_3A11_AECControl11     	= 0x3A11,
	_3A13_AECControl13     	= 0x3A13,
	_3A14_AECMaxExpo50H    	= 0x3A14,
	_3A15_AECMaxExpo50M    	= 0x3A15,
	_3A16_AECMaxExpo50L    	= 0x3A16,
	_3A18_AECGainCeilH     	= 0x3A18,
	_3A19_AECGainCeilL     	= 0x3A19,
	_3A1A_                 	= 0x3A1A,
	_3A1B_AECControl1B     	= 0x3A1B,
	_3A1E_AECControl1E     	= 0x3A1E,
	_3A1F_AECControl1F     	= 0x3A1F,
	_4301_                 	= 0x4301,
	_4303_                 	= 0x4303,
	_4708_DVPCtrl02			= 0x4708,
	_5000_ISPControl0		= 0x5000,
	_5001_ISPControl1		= 0x5001,
	_503D_ISPTest			= 0x503D,
	_5181_AWBControl01     	= 0x5181,
	_518F_AWBFrameInterval 	= 0x518F,
	_5684_AVGStartYHigh    	= 0x5684,
	_5685_AVGStartYLow     	= 0x5685,
	_5686_AVGEndYHigh      	= 0x5686,
	_5687_AVGEndYLow       	= 0x5687,
	_5688_                 	= 0x5688
} EOV2710Regs;

typedef struct OV2710_reg_addr_data
{
    EOV2710Regs eRegAddr;
    BYTE byData;
} TOV2710RegAddrData;

typedef struct OV2710_info
{
	spinlock_t lock;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadBuf pfnReadBuf;
	FOnWriteBuf pfnWriteBuf;

	DWORD dwFreq;
	
	BOOL bOV2710DSPEnable; // for testing	
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	
}TOV2710Info;

