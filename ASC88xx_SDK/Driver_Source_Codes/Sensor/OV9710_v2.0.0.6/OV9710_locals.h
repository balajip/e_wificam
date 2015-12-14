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

#ifndef __OV9710_LOCALS_H__
#define __OV9710_LOCALS_H__
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


#define OV9710_PID 0x9711
#define OV9710_MID 0x7FA2
/* Version 2.0.0.2 modification, 2011.12.15 */
#define OV9710_TBL_SIZE (59)
#define OV9710_NO_DSP_TBL_SIZE (62)
/*=========================================*/	  

#define OV9710_DEFAULT_OUTPUT_WIDTH	1280
#define OV9710_DEFAULT_OUTPUT_HEIGHT	800

/* Version 2.0.0.4 modification, 2012.08.06 */
#define OV9710_MAX_GAIN 31000
/*=========================================*/	  

/* Version 2.0.0.1 modification, 2011.11.24 */
#define OV9710_WB_GAIN_UNIT			1024

#define OV9710_PIXEL_CLOCK				40500000
#define OV9710_PIXEL_CLOCK_M_10		(OV9710_PIXEL_CLOCK/100000)

#define OV9710_HSYNC_1280x800_30FPS	1630
#define OV9710_HSYNC_1280x800_25FPS	1630

#define OV9710_VSYNC_1280x800_30FPS	828
#define OV9710_VSYNC_1280x800_25FPS	994
/* Version 2.0.0.3 modification, 2011.12.28 */
#define OV9710_HSYNC_640x400_60FPS	1630	
#define OV9710_HSYNC_640x400_50FPS	1630

#define OV9710_VSYNC_640x400_60FPS	414
#define OV9710_VSYNC_640x400_50FPS	497
/* ======================================== */
#define OV9710_PRE_CHARGING_LINE	7
/*=========================================*/	  

typedef enum ov9710_registers
{
    _00_GainControl					= 0x00,
    _01_BlueGainControlH			= 0x01,
    _02_RedGainControlH				= 0x02,
    _03_Reg03						= 0x03,
    _04_Reg04              			= 0x04,
    _05_GreenGainControlH			= 0x05,
    _06_BRGainControlL				= 0x06,
    _07_GreenGainControlL			= 0x07,
	_08_Reg08						= 0x08,
	_09_CommonControl2				= 0x09,
    _0A_PIDNumberH                  = 0x0A,  /* Read only */
    _0B_PIDNumberL                  = 0x0B,  /* Read only */
	_0C_CommonControl3				= 0x0C,
	_0D_CommonControl4				= 0x0D,
	_0E_CommonControl5				= 0x0E,
	_0F_CommonControl6				= 0x0F,
    _10_AutoExposureControlL 		= 0x10,
    _11_ClockControl				= 0x11,
    _12_CommonControl7              = 0x12,
    _13_CommonControl8              = 0x13,
    _14_CommonControl9              = 0x14,
    _15_CommonControl10             = 0x15,
    _16_AutoExposureControlH		= 0x16,
    _17_HorzWindowStart				= 0x17,
    _18_AHSize						= 0x18,
    _19_VertWindowStart				= 0x19,
    _1A_AVSize						= 0x1A,
    _1B_PixelShift                  = 0x1B,
    _1C_ManufactureIDHighByte		= 0x1C,  /* Read only */
    _1D_ManufactureIDLowByte		= 0x1D,  /* Read only */
    _1E_CommonControl11				= 0x1E,
    _1F_Reg1F						= 0x1F,
    _20_CommonControl12				= 0x20,
    _21_CommonControl13				= 0x21,
    _22_CommonControl14				= 0x22,
    _23_CommonControl15				= 0x23,
    _24_WPT                         = 0x24,
    _25_BPT                         = 0x25,
    _26_VPT							= 0x26,
    _27_CommonControl16				= 0x27,
    _28_CommonControl17				= 0x28,
    _29_CommonControl18             = 0x29,
    _2A_Reg2A						= 0x2A,
    _2B_Reg2B						= 0x2B,
    _2C_CommonControl19				= 0x2C,
    _2D_Reg2D						= 0x2D,
    _2E_Reg2E						= 0x2E,
    _2F_YAvg						= 0x2F,
    _30_HSYNCRisingDelay			= 0x30,
    _31_HSYNCFallingDelay			= 0x31,
	_32_Reg32						= 0x32,
    _33_Reserved					= 0x33,
	_34_Reserved					= 0x34,
	_35_Reserved					= 0x35,
	_36_Reserved					= 0x36,
	_37_CommonControl24				= 0x37,
	_38_CommonControl25				= 0x38,
	_39_Reserved					= 0x39,
	_3A_Reserved					= 0x3A,
	_3B_Reserved					= 0x3B,
	_3C_CommonControl28				= 0x3C,
	_3D_RENDL						= 0x3D,
	_3E_RENDH						= 0x3E,
	_3F_Reserved					= 0x3F,
	_40_Reserved					= 0x40,
	_41_Reg41						= 0x41,
	_42_Reg42						= 0x42,
	_43_Reg43						= 0x43,
	_44_ROFFS						= 0x44,
	_45_BOFFS						= 0x45,
	_46_GrOFFS						= 0x46,
	_47_GbOFFS						= 0x47,
	_48_HOFFS						= 0x48,
	_49_Reg49						= 0x49,
	_4A_Reg4A						= 0x4A,
	_4B_Reserved					= 0x4B,
	_4C_Reserved					= 0x4C,
	_4D_Reserved					= 0x4D,
	_4E_Reg4E						= 0x4E,
	_4F_Reg4F						= 0x4F,
	_50_Reg50						= 0x50,
	_51_Reg51						= 0x51,
	_52_Reserved					= 0x52,
	_53_Reserved					= 0x53,
	_54_Reserved					= 0x54,
	_55_Reg55						= 0x55,
	_56_Reg56						= 0x56,
	_57_Reg57						= 0x57,
	_58_Reg58						= 0x58,
	_59_Reg59						= 0x59,
	_5A_Reserved					= 0x5A,
	_5B_Reserved					= 0x5B,
	_5C_Reg5C						= 0x5C,
	_5D_Reg5D						= 0x5D,
	_5E_Reserved					= 0x5E,
	_5F_Reserved					= 0x5F,
	_60_Reserved					= 0x60,
	_61_Reserved					= 0x61,
	_62_Reserved					= 0x62,
	_63_Reserved					= 0x63,
	_64_Reserved					= 0x64,
	_65_Reserved					= 0x65,
	_66_Reserved					= 0x66,
	_67_Reserved					= 0x67,
	_68_Reserved					= 0x68,
	_69_Reserved					= 0x69,
	_6A_Reserved					= 0x6A,
	_6B_Reserved					= 0x6B,
	_6C_Reserved					= 0x6C,
	_6D_Reserved					= 0x6D,
	_6E_Reserved					= 0x6E,
	_6F_Reserved					= 0x6F,
	_70_Reserved					= 0x70,
	_71_Reserved					= 0x71,
	_72_Reg72						= 0x72,
	_73_Reg73						= 0x73,
	_74_Reg74						= 0x74,
	_75_Reg75						= 0x75,
	_76_Reg76						= 0x76,
	_77_Reg77						= 0x77,
	_78_Reg78						= 0x78,
	_79_Reg79						= 0x79,
	_7A_Reserved					= 0x7A,
	_7B_Reserved					= 0x7B,
	_7C_Reserved					= 0x7C,
	_7D_Reserved					= 0x7D,
	_7E_Reserved					= 0x7E,
	_7F_Reserved					= 0x7F,
	_80_Reserved					= 0x80,
	_81_Reserved					= 0x81,
	_82_Reserved					= 0x82,
	_83_Reserved					= 0x83,
	_84_Reserved					= 0x84,
	_85_Reserved					= 0x85,
	_86_Reserved					= 0x86,
	_87_Reserved					= 0x87,
	_88_Reserved					= 0x88,
	_89_Reserved					= 0x89,
	_8A_Reserved					= 0x8A,
	_8B_Reserved					= 0x8B,
	_8C_Reserved					= 0x8C,
	_8D_Reserved					= 0x8D,
	_8E_Reserved					= 0x8E,
	_8F_Reserved					= 0x8F,
	_90_Reserved					= 0x90,
	_91_Reserved					= 0x91,
	_92_Reserved					= 0x92,
	_93_Reserved					= 0x93,
	_94_Reserved					= 0x94,
	_95_Reserved					= 0x95,
	_96_DSPControl0					= 0x96,
	_97_DSPControl1					= 0x97,
	_98_DSPControl2					= 0x98,
	_99_DSPControl3					= 0x99,
	_9A_DSPControl4					= 0x9A,
	_9B_DSPControl5					= 0x9B,
	_9C_DSPControl6					= 0x9C,
	_9D_DSPControl7					= 0x9D,
	_9E_LensCorrectControl00		= 0x9E,
	_9F_LensCorrectControl01		= 0x9F,
	_A0_LensCorrectControl02		= 0xA0,
	_A1_LensCorrectControl03		= 0xA1,
	_A2_LensCorrectControl04		= 0xA2,
	_A3_LensCorrectControl05		= 0xA3,
	_A4_LensCorrectControl06		= 0xA4,
	_A5_LensCorrectControl07		= 0xA5,
	_A6_LensCorrectControl08		= 0xA6,
	_A7_LensCorrectControl09		= 0xA7,
	_A8_LensCorrectControl10		= 0xA8,
	_A9_LensCorrectControl11		= 0xA9,
	_AA_LensCorrectControl12		= 0xAA,
	_AB_LensCorrectControl13		= 0xAB,
	_AC_LensCorrectControl14		= 0xAC,
	_AD_LensCorrectControl15		= 0xAD,
	_AE_LensCorrectControl16		= 0xAE,
	_AF_LensCorrectControl17		= 0xAF,
	_B0_LensCorrectControl18		= 0xB0,
	_B1_LensCorrectControl19		= 0xB1,
	_B2_LensCorrectControl20		= 0xB2,
	_B3_LensCorrectControl21		= 0xB3,
	_B4_LensCorrectControl22		= 0xB4,
	_B5_LensCorrectControl23		= 0xB5,
	_B6_AWBControl0					= 0xB6,
	_B7_AWBControl1					= 0xB7,
	_B8_AWBControl2					= 0xB8,
	_B9_AWBControl3					= 0xB9,
	_BA_AWBControl4					= 0xBA,
	_BB_AWBControl5					= 0xBB,
	_BC_WBCControl0					= 0xBC,
	_BD_YAvgControl0				= 0xBD,
	_BE_YAvgControl1				= 0xBE,
	_BF_YAvgControl2				= 0xBF,
	_C0_YAvgControl3				= 0xC0,
	_C1_YAvgControl4				= 0xC1,
	_C2_DVPControl00				= 0xC2,
	_C3_DVPControl01				= 0xC3,
	_C4_DVPControl02				= 0xC4,
	_C5_DVPControl03				= 0xC5,
	_C6_DVPControl04				= 0xC6,
	_C7_DVPControl05				= 0xC7,
	_C8_DVPControl06				= 0xC8,
	_C9_DVPControl07				= 0xC9,
	_CA_DVPControl08				= 0xCA,
	_CB_DVPControl09				= 0xCB,
	_CC_DVPControl0A				= 0xCC,
	_CD_DVPControl0B				= 0xCD,
	_CE_DVPControl0C				= 0xCE,
	_CF_DVPControl0D				= 0xCF,
	_D0_DVPControl0E				= 0xD0,
	_D1_DVPControl0F				= 0xD1,
	_D2_DVPControl10				= 0xD2,
	_D3_DVPControl11				= 0xD3,
	_D4_DVPControl12				= 0xD4,
	_D5_Reserved					= 0xD5,
	_D6_SCControl0					= 0xD6
	/*D7 ~ FF Reserved*/

} EOV9710Regs;

typedef struct OV971_reg_addr_data
{
    EOV9710Regs eRegAddr;
    DWORD dwData;
} TOV9710RegAddrData;

typedef struct OV971_info
{
	spinlock_t lock;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadReg pfnReadReg;
	FOnWriteReg pfnWriteReg;
	EVideoSignalFormat eFormat;
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwFreq;
	BOOL bOV9710DSPEnable;
/* Version 2.0.0.4 modification, 2012.08.06 */
	BOOL bOV9710AWBEnable;
/*=========================================*/	  
}TOV9710Info;
