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

#ifndef __OV5653_LOCALS_H__
#define __OV5653_LOCALS_H__
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


/* ============================================================================*/
#define OV5653_PID			0x5651
#define OV5653_TBL_SIZE		(103)

#define OV5653_WB_GAIN_UNIT			1024

#define OV5653_DEFAULT_OUTPUT_WIDTH	1920
#define OV5653_DEFAULT_OUTPUT_HEIGHT	1080

/* Version 1.0.0.2 modification, 2012.08.06 */
#define OV5653_MAX_GAIN 31000
/* ======================================== */

#define OV5653_PIXEL_CLOCK				96000000
#define OV5653_PIXEL_CLOCK_M			(OV5653_PIXEL_CLOCK/1000000)

/* Version 1.0.0.2 modification, 2012.08.06 */
#define OV5653_HSYNC_2560x1920_15FPS	3305
#define OV5653_VSYNC_2560x1920		1944

#define OV5653_HSYNC_1920x1080_30FPS	2919
#define OV5653_HSYNC_1920x1080_25FPS	3503
#define OV5653_VSYNC_1920x1080		1120

#define OV5653_HSYNC_1600x1200_30FPS	2631
#define OV5653_HSYNC_1600x1200_25FPS	3157
#define OV5653_VSYNC_1600x1200		1224

#define OV5653_HSYNC_1280x1024_30FPS	3076
#define OV5653_HSYNC_1280x1024_25FPS	3692
#define OV5653_VSYNC_1280x1024		1048

#define OV5653_HSYNC_1280x720_30FPS	4347
#define OV5653_HSYNC_1280x720_25FPS	5217
#define OV5653_VSYNC_1280x720			744

#define OV5653_HSYNC_800x600_30FPS	5194
#define OV5653_HSYNC_800x600_25FPS	6233
#define OV5653_VSYNC_800x600			624

#define OV5653_HSYNC_640x480_60FPS	6451
#define OV5653_HSYNC_640x480_50FPS	7741
#define OV5653_VSYNC_640x480			504

#define OV5653_HSYNC_320x240_120FPS	6250
#define OV5653_HSYNC_320x240_100FPS	7500
#define OV5653_VSYNC_320x240			264
/* ======================================== */

typedef enum OV5653_registers
{
	_3008_SystemControl   =0x3008,
	_300A_PIDH				= 0x300A,
	_300B_PIDL				= 0x300B,
	_300f_Pllctrl00       =0x300f,
	_3010_Pllctrl01       =0x3010,
	_3011_Pllctrl02       =0x3011,
	_3017_PADOutputEnable0=0x3017, 
	_3018_PADOutputEnable1=0x3018, 
	_302C_PADOutputDrv		= 0x302C,
	_3030_PowerCtrl		= 0x3030,
	_3103_PLLClockSelect  =0x3103,
	_3212_GroupAccess	= 0x3212,
	_3400_AWBRGain1       =0x3400,
	_3401_AWBRGain0       =0x3401,
	_3402_AWBGGain1       =0x3402,
	_3403_AWBGGain0       =0x3403,
	_3404_AWBBGain1       =0x3404,
	_3405_AWBBGain0       =0x3405,
	_3406_AWBManual       =0x3406,
	_3500_AECLongExpo2    =0x3500,
	_3501_AECLongExpo1    =0x3501,
	_3502_AECLongExpo0    =0x3502,
	_3503_AEAGManual      =0x3503,
	_350a_AGCAdj1        =0x350a,
	_350b_AGCAdj0          =0x350b,
	_350C_AECVTSDiff1	=0x350c,
	_350D_AECVTSDiff0	=0x350d,
	_3600_AnalogControl00 =0x3600,
	_3601_AnalogControl01 =0x3601,
	_3603_                =0x3603,
	_3604_AnalogControl04 =0x3604,
	_3605_                =0x3605,
	_3606_                =0x3606,
	_3612_AnalogControl12 =0x3612,
	_3613_AnalogControl13 =0x3613,
	_3615_                =0x3615,
	_3620_AnalogControl20 =0x3620,
	_3621_ArrayCtrl01     =0x3621,
	_3623_AnalogControl23 =0x3623,
	_3630_                =0x3630,
	_3631_AnalogControl31 =0x3631,
	_3632_AnalogControl32 =0x3632,
	_3633_AnalogControl33 =0x3633,
	_3702_AnalogControl02 =0x3702,
	_3703_AnalogControl03 =0x3703,
	_3704_AnalogControl04 =0x3704,
	_3705_AnalogControl05 =0x3705,
	_3706_                =0x3706,
	_370A_AnalogControl0A =0x370A,
	_370b_AnalogControl0B =0x370b,
	_370C_AnalogControl0C =0x370C,
	_370D_AnalogControl0D =0x370D,
	_370e_AnalogControl0E =0x370e,
	_3710_AnalogControl10 =0x3710,
	_3711_AnalogControl11 =0x3711,
	_3712_AnalogControl12 =0x3712,
	_3713_AnalogControl13    =0x3713,
	_3714_AnalogControl14    =0x3714,
	_3800_TimmingHS1         =0x3800,
	_3801_TimmingHS0         =0x3801,
	_3803_TimmingVS0         =0x3803,
	_3804_TimmingHW1         =0x3804,
	_3805_TimmingHW0         =0x3805,
	_3806_TimmingVH1         =0x3806,
	_3807_TimmingVH0         =0x3807,
	_3808_TimmingHO1         =0x3808,
	_3809_TimmingHO0         =0x3809,
	_3810_TimmingHVOffs   	 =0x3810,
	_380A_TimmingVO1         =0x380A,
	_380B_TimmingVO0         =0x380B,
	_380C_TimmingHTS1        =0x380C,
	_380D_TimmingHTS0        =0x380D,
	_380E_TimmingVTS1        =0x380E,
	_380F_TimmingVTS0        =0x380F,
	_3815_TimmingTCReg15     =0x3815,
	_3818_TimmingTCReg18     =0x3818,
	_381A_TimmingTCHSMirrAdj =0x381A,
	_381C_TimmingTCReg1C     =0x381C,
	_381D_TimmingTCVsCropL   =0x381D,
	_381E_TimmingTCVHCrop1   =0x381E,
	_381F_TimmingTCVHCrop0   =0x381F,
	_3820_TimmingTCReg20     =0x3820,
	_3821_TimmingTCReg21     =0x3821,
	_3824_TimmingHRefstMan1  =0x3824,
	_3825_TimmingHRefstMan0  =0x3825,
	_3830_AGCManual       =0x3830, 
	_3836_TimmingHVPad    =0x3836,
	_3a00_AECMode         =0x3a00,
	_3a08_AECB50Step1     =0x3a08,
	_3a09_AECB50Step0     =0x3a09,
	_3a0a_AECB60Step1     =0x3a0a,
	_3a0b_AECB60Step0     =0x3a0b,
	_3a0d_AECB60Max       =0x3a0d,
	_3a0e_AECB50Max       =0x3a0e,
	_3a13_AECPreGain      =0x3a13,
	_3a18_AECDebugMode    =0x3a18,
	_3a19_AECGainCeiling  =0x3a19,
	_3a1a_AECDiffMax      =0x3a1a,
	_3b07_StrobeModeSelect=0x3b07,
	_3c01_5060HzCtrl01    =0x3c01,
	_3c04_5060HzSumLowThr =0x3c04,
	_3c05_5060HzSumHighThr=0x3c05,
	_3c07_5060HzSumLM1Thr =0x3c07,
	_3c09_5060HzSumLM2Thr =0x3c09,
	_4000_BLCCtrl00       =0x4000,
	_4001_BLCCtrl01       =0x4001,
	_4006_BLCLongTarget1       =0x4006,
	_4007_BLCLongTarget0       =0x4007,
	_401d_BLCCtrl1D       =0x401d,
	_401c_BLCCtrl1C       =0x401c,
	_401f_                =0x401f,
	_4704_DVPVsyncMode    =0x4704,
	_4708_DVPPolCtrl      =0x4708,
	_470a_DVPBypCtrl1     =0x470a,
	_470b_DVPBypCtrl0     =0x470b,
	_470c_DVPBypSel       =0x470c,
	_5000_ISPCtrl00       =0x5000,
	_5001_ISPCtrl01       =0x5001,
	_5002_ISPCtrl02       =0x5002,
	_503d_ISPCtrl3d       =0x503d,
	_5046_ISPCtrl46       =0x5046,
	_5059_                =0x5059,
	_505A_LencOffxMan1    =0x505A,
	_505B_LencOffxMan0    =0x505B,
	_505f_LENCCtrl5F      =0x505f,
	_5180_AWBFastAwb      =0x5180,
	_5181_AWBControl01    =0x5184,
	_5184_AWBFrameCtrl    =0x5184,
	_585a_LencBRHscale1   =0x585a,
	_585b_LencBRHscale0   =0x585b,
	_585c_LencBRVscale1   =0x585c,
	_585d_LencBRVscale0   =0x585d,
	_585e_LencGHscale1    =0x585e,
	_585f_LencGHscale0    =0x585f,
	_5860_LencGVscale1    =0x5860,
	_5861_LencGVscale0    =0x5861,
	_5901_VAPCtrl01       =0x5901,
} EOV5653Regs;

typedef struct OV5653_reg_addr_data
{
    EOV5653Regs eRegAddr;
    BYTE byData;
} TOV5653RegAddrData;

typedef struct OV5653_info
{
	spinlock_t lock;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadBuf pfnReadBuf;
	FOnWriteBuf pfnWriteBuf;

	DWORD dwWidth;
	DWORD dwHeight;

	DWORD dwFreq;
	
	BOOL bOV5653DSPEnable;
}TOV5653Info;

