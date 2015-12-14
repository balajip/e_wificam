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

#ifndef __NVP1114A_LOCALS_H__
#define __NVP1114A_LOCALS_H__
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
typedef SCODE (* FOnReadReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAdd, DWORD *pdwData);
typedef SCODE (* FOnWriteReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwData);

#define NVP1114A_INTERLACE_CCIR656_TBL_SIZE (371)
/* Version 3.0.0.0 modification, 2012.12.13 */
#define NVP1114A_STD_TBL_SIZE	1 
/* ========================================= */	
typedef enum nvp1114a_registers
{
_00_ 	= 0x00,
_01_ 	= 0x01,
_02_ 	= 0x02,
_03_ 	= 0x03,
_04_ 	= 0x04,
_05_ 	= 0x05,
_06_ 	= 0x06,
_07_ 	= 0x07,
_08_ 	= 0x08,
_09_ 	= 0x09,
_0A_ 	= 0x0A,
_0B_ 	= 0x0B,
_0C_ 	= 0x0C,
_0D_ 	= 0x0D,
_0E_ 	= 0x0E,
_0F_ 	= 0x0F,
_10_ 	= 0x10,
_11_ 	= 0x11,
_12_ 	= 0x12,
_13_ 	= 0x13,
_14_ 	= 0x14,
_15_ 	= 0x15,
_16_ 	= 0x16,
_17_ 	= 0x17,
_18_ 	= 0x18,
_19_ 	= 0x19,
_1A_ 	= 0x1A,
_1B_ 	= 0x1B,
_1C_ 	= 0x1C,
_1D_ 	= 0x1D,
_1E_ 	= 0x1E,
_1F_ 	= 0x1F,
_20_ 	= 0x20,
_21_ 	= 0x21,
_22_ 	= 0x22,
_23_ 	= 0x23,
_24_ 	= 0x24,
_25_ 	= 0x25,
_26_ 	= 0x26,
_27_ 	= 0x27,
_28_ 	= 0x28,
_29_ 	= 0x29,
_2A_ 	= 0x2A,
_2B_ 	= 0x2B,
_2C_ 	= 0x2C,
_2D_ 	= 0x2D,
_2E_ 	= 0x2E,
_2F_ 	= 0x2F,
_30_ 	= 0x30,
_31_ 	= 0x31,
_32_ 	= 0x32,
_33_ 	= 0x33,
_34_ 	= 0x34,
_35_ 	= 0x35,
_36_ 	= 0x36,
_37_ 	= 0x37,
_38_ 	= 0x38,
_39_ 	= 0x39,
_3A_ 	= 0x3A,
_3B_ 	= 0x3B,
_3C_ 	= 0x3C,
_3D_ 	= 0x3D,
_3E_ 	= 0x3E,
_3F_ 	= 0x3F,
_40_ 	= 0x40,
_41_ 	= 0x41,
_42_ 	= 0x42,
_43_ 	= 0x43,
_44_ 	= 0x44,
_45_ 	= 0x45,
_46_ 	= 0x46,
_47_ 	= 0x47,
_48_ 	= 0x48,
_49_ 	= 0x49,
_4A_ 	= 0x4A,
_4B_ 	= 0x4B,
_4C_ 	= 0x4C,
_4D_ 	= 0x4D,
_4E_ 	= 0x4E,
_4F_ 	= 0x4F,
_50_ 	= 0x50,
_51_ 	= 0x51,
_52_ 	= 0x52,
_53_ 	= 0x53,
_54_ 	= 0x54,
_55_ 	= 0x55,
_56_ 	= 0x56,
_57_ 	= 0x57,
_58_ 	= 0x58,
_59_ 	= 0x59,
_5A_ 	= 0x5A,
_5B_ 	= 0x5B,
_5C_ 	= 0x5C,
_5D_ 	= 0x5D,
_5E_ 	= 0x5E,
_5F_ 	= 0x5F,
_60_ 	= 0x60,
_61_ 	= 0x61,
_62_ 	= 0x62,
_63_ 	= 0x63,
_64_ 	= 0x64,
_65_ 	= 0x65,
_66_ 	= 0x66,
_67_ 	= 0x67,
_68_ 	= 0x68,
_69_ 	= 0x69,
_6A_ 	= 0x6A,
_6B_ 	= 0x6B,
_6C_ 	= 0x6C,
_6D_ 	= 0x6D,
_6E_ 	= 0x6E,
_6F_ 	= 0x6F,
_70_ 	= 0x70,
_71_ 	= 0x71,
_72_ 	= 0x72,
_73_ 	= 0x73,
_74_ 	= 0x74,
_75_ 	= 0x75,
_76_ 	= 0x76,
_77_ 	= 0x77,
_78_ 	= 0x78,
_79_ 	= 0x79,
_7A_ 	= 0x7A,
_7B_ 	= 0x7B,
_7C_ 	= 0x7C,
_7D_ 	= 0x7D,
_7E_ 	= 0x7E,
_7F_ 	= 0x7F,
_80_ 	= 0x80,
_81_ 	= 0x81,
_82_ 	= 0x82,
_83_ 	= 0x83,
_84_ 	= 0x84,
_85_ 	= 0x85,
_86_ 	= 0x86,
_87_ 	= 0x87,
_88_ 	= 0x88,
_89_ 	= 0x89,
_8A_ 	= 0x8A,
_8B_ 	= 0x8B,
_8C_ 	= 0x8C,
_8D_ 	= 0x8D,
_8E_ 	= 0x8E,
_8F_ 	= 0x8F,
_90_ 	= 0x90,
_91_ 	= 0x91,
_92_ 	= 0x92,
_93_ 	= 0x93,
_94_ 	= 0x94,
_95_ 	= 0x95,
_96_ 	= 0x96,
_97_ 	= 0x97,
_98_ 	= 0x98,
_99_ 	= 0x99,
_9A_ 	= 0x9A,
_9B_ 	= 0x9B,
_9C_ 	= 0x9C,
_9D_ 	= 0x9D,
_9E_ 	= 0x9E,
_9F_ 	= 0x9F,
_A0_ 	= 0xA0,
_A1_ 	= 0xA1,
_A2_ 	= 0xA2,
_A3_ 	= 0xA3,
_A4_ 	= 0xA4,
_A5_ 	= 0xA5,
_A6_ 	= 0xA6,
_A7_ 	= 0xA7,
_A8_ 	= 0xA8,
_A9_ 	= 0xA9,
_AA_ 	= 0xAA,
_AB_ 	= 0xAB,
_AC_ 	= 0xAC,
_AD_ 	= 0xAD,
_AE_ 	= 0xAE,
_AF_ 	= 0xAF,
_B0_ 	= 0xB0,
_B1_ 	= 0xB1,
_B2_ 	= 0xB2,
_B3_ 	= 0xB3,
_B4_ 	= 0xB4,
_B5_ 	= 0xB5,
_B6_ 	= 0xB6,
_B7_ 	= 0xB7,
_B8_ 	= 0xB8,
_B9_ 	= 0xB9,
_BA_ 	= 0xBA,
_BB_ 	= 0xBB,
_BC_ 	= 0xBC,
_BD_ 	= 0xBD,
_BE_ 	= 0xBE,
_BF_ 	= 0xBF,
_C0_ 	= 0xC0,
_C1_ 	= 0xC1,
_C2_ 	= 0xC2,
_C3_ 	= 0xC3,
_C4_ 	= 0xC4,
_C5_ 	= 0xC5,
_C6_ 	= 0xC6,
_C7_ 	= 0xC7,
_C8_ 	= 0xC8,
_C9_ 	= 0xC9,
_CA_ 	= 0xCA,
_CB_ 	= 0xCB,
_CC_ 	= 0xCC,
_CD_ 	= 0xCD,
_CE_ 	= 0xCE,
_CF_ 	= 0xCF,
_D0_ 	= 0xD0,
_D1_ 	= 0xD1,
_D2_ 	= 0xD2,
_D3_ 	= 0xD3,
_D4_ 	= 0xD4,
_D5_ 	= 0xD5,
_D6_ 	= 0xD6,
_D7_ 	= 0xD7,
_D8_ 	= 0xD8,
_D9_ 	= 0xD9,
_DA_ 	= 0xDA,
_DB_ 	= 0xDB,
_DC_ 	= 0xDC,
_DD_ 	= 0xDD,
_DE_ 	= 0xDE,
_DF_ 	= 0xDF,
_E0_ 	= 0xE0,
_E1_ 	= 0xE1,
_E2_ 	= 0xE2,
_E3_ 	= 0xE3,
_E4_ 	= 0xE4,
_E5_ 	= 0xE5,
_E6_ 	= 0xE6,
_E7_ 	= 0xE7,
_E8_ 	= 0xE8,
_E9_ 	= 0xE9,
_EA_ 	= 0xEA,
_EB_ 	= 0xEB,
_EC_ 	= 0xEC,
_ED_ 	= 0xED,
_EE_ 	= 0xEE,
_EF_ 	= 0xEF,
_F0_ 	= 0xF0,
_F1_ 	= 0xF1,
_F2_ 	= 0xF2,
_F3_ 	= 0xF3,
_F4_ 	= 0xF4,
_F5_ 	= 0xF5,
_F6_ 	= 0xF6,
_F7_ 	= 0xF7,
_F8_ 	= 0xF8,
_F9_ 	= 0xF9,
_FA_ 	= 0xFA,
_FB_ 	= 0xFB,
_FC_ 	= 0xFC,
_FD_ 	= 0xFD,
_FE_ 	= 0xFE,
_FF_ 	= 0xFF
} ENVP1114ARegs;

typedef struct NVP1114A_reg_addr_data
{
    ENVP1114ARegs eRegAddr;
    DWORD dwData;
} TNVP1114ARegAddrData;

typedef struct nvp1114a_info
{	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadReg_t pfnReadReg;
	FOnWriteReg_t pfnWriteReg;
}TNVP1114AInfo;

