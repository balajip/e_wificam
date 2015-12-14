/*
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
 */
#ifndef __AR0330_LOCALS_H__
#define __AR0330_LOCALS_H__
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

/*==================================================================*/
#define AR0330_CHIP_VERSION		0x2604

#define AR0330_DEFAULT_OUTPUT_WIDTH	1920
#define AR0330_DEFAULT_OUTPUT_HEIGHT	1080

#define AR0330_MAX_GAIN 		127000

#define AR0330_HSYNC_2304x1536_25HZ	2496
#define AR0330_VSYNC_2304x1536_25HZ	1570
#define AR0330_HSYNC_2304x1536_15HZ	2496
#define AR0330_VSYNC_2304x1536_15HZ	2616

#define AR0330_HSYNC_2304x1296_30HZ	2496
#define AR0330_VSYNC_2304x1296_30HZ	1308
#define AR0330_HSYNC_2304x1296_25HZ	2496
#define AR0330_VSYNC_2304x1296_25HZ	1569

#define AR0330_HSYNC_1920x1080_30HZ	2200
#define AR0330_VSYNC_1920x1080_30HZ	1125
#define AR0330_HSYNC_1920x1080_25HZ	2200
#define AR0330_VSYNC_1920x1080_25HZ	1350

#define AR0330_HSYNC_1152x648_30HZ	2496
#define AR0330_VSYNC_1152x648_30HZ	1308
#define AR0330_HSYNC_1152x648_25HZ	2496
#define AR0330_VSYNC_1152x648_25HZ	1569

typedef enum AR0330_registers
{
	_3000_ChipVersion	=	0x3000,
	_3002_YAddrStart	=	0x3002,
	_3004_XAddrStart	=	0x3004,
	_3006_YAddrEnd		=	0x3006,
	_3008_XAddrEnd		=	0x3008,
	_300A_FrameLengthLines	=	0x300A,
	_300C_LineLengthPck	=	0x300C,
	_300E_RevisionNumber	=	0x300E,
	_3010_LockControl	=	0x3010,
	_3012_CoarseIntegrationTime	=	0x3012,
	_3014_FineIntegrationTime	=	0x3014,
	_3016_CoarseIntegrationTimeCb	=	0x3016,
	_3018_FineIntegrationTimeCb	=	0x3018,
	_301A_ResetRegister	=	0x301A,
	_301C_ModeSelect	=	0x301C,
	_301D_ImageOrientation	=	0x301D,
	_301E_DataPedestal	=	0x301E,
	_3021_SoftwareReset	=	0x3021,
	_3026_GpiStatus		=	0x3026,
	_3028_RowSpeed		=	0x3028,
	_302A_VtPixClkDiv	=	0x302A,
	_302C_VtSysClkDiv	=	0x302C,
	_302E_PrePllClkDiv	=	0x302E,
	_3030_PllMultiplier	=	0x3030,
	_3036_OpPixClkDiv	=	0x3036,
	_3038_OpSysClkDiv	=	0x3038,
	_303A_FrameCount	=	0x303A,
	_303C_FrameStatus	=	0x303C,
	_303E_LineLengthPckCb	=	0x303E,
	_3040_ReadMode		=	0x3040,
	_3042_ExtraDelay	=	0x3042,
	_3046_Flash		=	0x3046,
	_3048_Flash2		=	0x3048,
	_3056_Green1Gain	=	0x3056,
	_3058_BlueGain		=	0x3058,
	_305A_RedGain		=	0x305A,
	_305C_Green2Gain	=	0x305C,
	_305E_GlobalGain	=	0x305E,
	_3060_AnalogGain	=	0x3060,
	_3064_SmiaTest		=	0x3064,
	_306E_DatapathSelect	=	0x306E,
	_3070_TestPatternMode	=	0x3070,
	_3072_TestDataRed	=	0x3072,
	_3074_TestDataGr	=	0x3074,
	_3076_TestDataBlue	=	0x3076,
	_3078_TestDataGb	=	0x3078,
	_307A_TestRawMode	=	0x307A,
	_3082_OperationModeCtrl	=	0x3082,
	_3084_OperationModeCtrlCb	=	0x3084,
	_3086_SeqDataPort	=	0x3086,
	_3088_SeqCtrlPort	=	0x3088,
	_308A_XAddrStartCb	=	0x308A,
	_308C_YAddrStartCb	=	0x308C,
	_308E_XAddrEndCb	=	0x308E,
	_3090_YAddrEndCb	=	0x3090,
	_309A_RatioActualT1T2	=	0x309A,
	_30A0_XEvenInc		=	0x30A0,
	_30A2_XOddInc		=	0x30A2,
	_30A4_YEvenInc		=	0x30A4,
	_30A6_YOddInc		=	0x30A6,
	_30A8_YOddIncCb		=	0x30A8,
	_30AA_FrameLengthLinesCb=	0x30AA,
	_30AE_XOddIncCb		=	0x30AE,
	_30B0_DigitalTest	=	0x30B0,
	_30B2_TempsensData	=	0x30B2,
	_30B4_TempsensCtrl	=	0x30B4,
	_30B6_Spare0x30b6	=	0x30B6,
	_30B8_Spare0x30b8	=	0x30B8,
	_30BA_DigitalCtrl	=	0x30BA,
	_30BC_Green1GainCb	=	0x30BC,
	_30BE_BlueGainCb	=	0x30BE,
	_30C0_RedGainCb		=	0x30C0,
	_30C2_Green2GainCb	=	0x30C2,
	_30C4_GlobalGainCb	=	0x30C4,
	_31AE_SerialFormat	=	0x31AE,
} EAR0330Regs;

typedef struct AR0330_info
{
	spinlock_t lock;	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;
	FOnReadBuf pfnReadBuf;
	FOnWriteBuf pfnWriteBuf;
	DWORD dwFreq;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwCurShutter;

	DWORD dwAnalogGain;
	DWORD dwDigitalGain;
} TAR0330Info;

