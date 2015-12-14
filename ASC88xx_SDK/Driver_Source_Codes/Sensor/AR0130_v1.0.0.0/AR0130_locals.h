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
#ifndef __AR0130_LOCALS_H__
#define __AR0130_LOCALS_H__
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
#define AR0130_CHIP_VERSION	0x2402

#define AR0130_SEQUENCE_TBL_SIZE (80)
#define AR0130_TBL_SIZE	(38)

#define AR0130_DEFAULT_OUTPUT_WIDTH	1280
#define AR0130_DEFAULT_OUTPUT_HEIGHT	720

#define AR0130_MAX_GAIN 115200 // 184320

#define AR0130_PIXEL_CLOCK	 74250000
#define AR0130_PIXEL_CLOCK_M_100	(AR0130_PIXEL_CLOCK/10000)

#define AR0130_HSYNC_1280x960_45FPS	1500
#define AR0130_VSYNC_1280x960_45FPS	1100	

#define AR0130_HSYNC_1280x960_40FPS	1500
#define AR0130_VSYNC_1280x960_40FPS	1237	

#define AR0130_HSYNC_1280x720_60FPS	1500	
#define AR0130_VSYNC_1280x720_60FPS	825	

#define AR0130_HSYNC_1280x720_50FPS	1500	
#define AR0130_VSYNC_1280x720_50FPS	990	

#define AR0130_HSYNC_1280x800_60FPS	1500	
#define AR0130_VSYNC_1280x800_60FPS	830	

#define AR0130_HSYNC_1280x800_50FPS	1500	
#define AR0130_VSYNC_1280x800_50FPS	991	

typedef enum AR0130_registers
{
	_2400_AltmControl	=	0x2400,
	_2422_AltmStatsExWinXStart	=	0x2422,
	_2424_AltmStatsExWinWidth	=	0x2424,
	_2426_AltmStatsExWinYStart	=	0x2426,
	_2428_AltmStatsExWinHeight	=	0x2428,
	_2438_AltmControlMinFactor	=	0x2438,
	_243A_AltmControlMaxFactor	=	0x243A,
	_243C_AltmControlDarkFloor	=	0x243C,
	_243E_AltmControlBrightFloor	=	0x243E,
	_2440_AltmControlDamper	=	0x2440,
	_2442_AltmControlKeyK0	=	0x2442,
	_2444_AltmControlKeyK01Lo	=	0x2444,
	_2446_AltmControlKeyK01Hi	=	0x2446,
	_2450_AltmOutPedestal	=	0x2450,
	_3000_ChipVersion	=	0x3000,
	_3002_YAddrStart	=	0x3002,
	_3004_XAddrStart	=	0x3004,
	_3006_YAddrEnd	=	0x3006,
	_3008_XAddrEnd	=	0x3008,
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
	_3026_GpiStatus	=	0x3026,
	_3028_RowSpeed	=	0x3028,
	_302A_VtPixClkDiv	=	0x302A,
	_302C_VtSysClkDiv	=	0x302C,
	_302E_PrePllClkDiv	=	0x302E,
	_3030_PllMultiplier	=	0x3030,
	_3036_OpPixClkDiv	=	0x3036,
	_3038_OpSysClkDiv	=	0x3038,
	_303A_FrameCount	=	0x303A,
	_303C_FrameStatus	=	0x303C,
	_303E_LineLengthPckCb	=	0x303E,
	_3040_ReadMode	=	0x3040,
	_3042_ExtraDelay	=	0x3042,
	_3046_Flash	=	0x3046,
	_3048_Flash2	=	0x3048,
	_3056_Green1Gain	=	0x3056,
	_3058_BlueGain	=	0x3058,
	_305A_RedGain	=	0x305A,
	_305C_Green2Gain	=	0x305C,
	_305E_GlobalGain	=	0x305E,
	_3060_AnalogGain	=	0x3060,
	_3064_SmiaTest	=	0x3064,
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
	_30A0_XEvenInc	=	0x30A0,
	_30A2_XOddInc	=	0x30A2,
	_30A4_YEvenInc	=	0x30A4,
	_30A6_YOddInc	=	0x30A6,
	_30A8_YOddIncCb	=	0x30A8,
	_30AA_FrameLengthLinesCb	=	0x30AA,
	_30AE_XOddIncCb	=	0x30AE,
	_30B0_DigitalTest	=	0x30B0,
	_30B2_TempsensData	=	0x30B2,
	_30B4_TempsensCtrl	=	0x30B4,
	_30B6_Spare0x30b6	=	0x30B6,
	_30B8_Spare0x30b8	=	0x30B8,
	_30BA_DigitalCtrl	=	0x30BA,
	_30BC_Green1GainCb	=	0x30BC,
	_30BE_BlueGainCb	=	0x30BE,
	_30C0_RedGainCb	=	0x30C0,
	_30C2_Green2GainCb	=	0x30C2,
	_30C4_GlobalGainCb	=	0x30C4,
	_30C6_TempsensCalib1	=	0x30C6,
	_30C8_TempsensCalib2	=	0x30C8,
	_30CA_TempsensCalib3	=	0x30CA,
	_30CC_TempsensCalib4	=	0x30CC,
	_30CE_GrrControl1	=	0x30CE,
	_30D0_GrrControl2	=	0x30D0,
	_30D2_GrrControl3	=	0x30D2,
	_30DA_GrrControl4	=	0x30DA,
	_30FE_NoisePedestal	=	0x30FE,
	_3140_AERoiXStartOffset	=	0x3140,
	_3142_AERoiYStartOffset	=	0x3142,
	_3144_AERoiXSize	=	0x3144,
	_3146_AERoiYSize	=	0x3146,
	_3148_AEHistBeginPerc	=	0x3148,
	_314A_AEHistEndPerc	=	0x314A,
	_314C_AEHistDiv	=	0x314C,
	_314E_AENormWidthMin	=	0x314E,
	_3150_AEMeanH	=	0x3150,
	_3152_AEMeanL	=	0x3152,
	_3154_AEHistBeginH	=	0x3154,
	_3156_AEHistBeginL	=	0x3156,
	_3158_AEHistEndH	=	0x3158,
	_315A_AEHistEndL	=	0x315A,
	_315C_AEHistEndMeanH	=	0x315C,
	_315E_AEHistEndMeanL	=	0x315E,
	_3160_AEPercLowEnd	=	0x3160,
	_3162_AENormAbsDev	=	0x3162,
	_3180_DeltaDkControl	=	0x3180,
	_318A_HdrMcCtrl1	=	0x318A,
	_318C_HdrMcCtrl2	=	0x318C,
	_318E_HdrMcCtrl3	=	0x318E,
	_3190_HdrMcCtrl4	=	0x3190,
	_3192_HdrMcCtrl5	=	0x3192,
	_3194_HdrMcCtrl6	=	0x3194,
	_3196_HdrMcCtrl7	=	0x3196,
	_3198_HdrMcCtrl8	=	0x3198,
	_319E_HdrMcCtrl9	=	0x319E,
	_31A2_HdrMcCtrl11	=	0x31A2,
	_31AC_DataFormatBits	=	0x31AC,
	_31AE_SerialFormat	=	0x31AE,
	_31C0_HispiTiming	=	0x31C0,
	_31C2_HispiBlanking	=	0x31C2,
	_31C4_HispiSyncPatt	=	0x31C4,
	_31C6_HispiControlStatus	=	0x31C6,
	_31C8_HispiCrc0	=	0x31C8,
	_31CA_HispiCrc1	=	0x31CA,
	_31CC_HispiCrc2	=	0x31CC,
	_31CE_HispiCrc3	=	0x31CE,
	_31D0_Companding	=	0x31D0,
	_31D2_StatFrameID	=	0x31D2,
	_31D6_I2cWrtChecksum	=	0x31D6,
	_31E8_HorizontalCursorPosition	=	0x31E8,
	_31EA_VerticalCursorPosition	=	0x31EA,
	_31EC_HorizontalCursorWidth	=	0x31EC,
	_31EE_VerticalCursorWidth	=	0x31EE,
	_31F4_FuseId1	=	0x31F4,
	_31F6_FuseId2	=	0x31F6,
	_31F8_FuseId3	=	0x31F8,
	_31FA_FuseId4	=	0x31FA,
	_31FC_CciIds	=	0x31FC,
	_3200_AdacdControl	=	0x3200,
	_3202_AdacdNoiseModel1	=	0x3202,
	_320A_AdacdPedestal	=	0x320A,
	_3780_PolyScEnable	=	0x3780,
} EAR0130Regs;

typedef struct AR0130_reg_addr_data
{
    EAR0130Regs eRegAddr;
    DWORD dwData;
} TAR0130RegAddrData;

typedef struct AR0130_info
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

	DWORD dwColumnGain;
	DWORD dwDACGain;
	DWORD dwDCGGain;
	DWORD dwGlobalGain;
} TAR0130Info;

