/*
 * NVP1114A
 * Driver for NVP1114A video encoder.
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
/* Version 1.0.0.3 modification, 2010.08.02 */	
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()
/* ========================================= */	

#include "NVP1114A.h"
#include "NVP1114A_locals.h"
#include "IICCtrl.h"

const CHAR NVP1114A_ID[] = "$Version: "NVP1114A_ID_VERSION"  (NVP1114A DRIVER) $";
/*==================================================================*/
static TNVP1114AInfo *ptInfo[2]; // 0 for NVP1114A address 0x70, 1 for NVP1114A address 0x72
static DWORD NVP1114A_DevInUse[2];
static DWORD NVP1114A_DevAddr[2] = {NVP1114A_DEAFULT_DEVICE_ADDR0, NVP1114A_DEAFULT_DEVICE_ADDR1};
TVideoSensorDevice sensor_dev_ops;
/* Version 1.0.0.3 modification, 2010.08.02 */	
static struct semaphore nvp1114a_mutex;
/* Version 3.0.0.0 modification, 2012.12.13 */
/* default standard */
static int dwDefaultStandard = TVSTD_PAL;  
module_param(dwDefaultStandard, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (dwDefaultStandard, "Default standard. (0:NTSC, 1:PAL)");
/* ========================================= */	
/* ========================================= */	

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("NVP1114A driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
/* Version 3.0.0.0 modification, 2012.12.13 */
#define STD_NUM 2
const TNVP1114ARegAddrData atNVP1114AregStdTbl[STD_NUM][NVP1114A_STD_TBL_SIZE]=
{
/*NTSC*/
 {
		{_20_						,0x60},
 },
/*PAL*/
 {
		{_20_						,0xBD},
 },
};
/* ========================================= */	
const TNVP1114ARegAddrData atNVP1114AregInterlaceCCIR656Tbl[NVP1114A_INTERLACE_CCIR656_TBL_SIZE] = {
		{_F0_						,0x00},
		{_F1_						,0x00},
		{_F2_						,0xA0},
		{_F3_						,0x04},
		{_F4_						,0x00},
		{_F5_						,0x00},
		{_F6_						,0x80},
		{_F7_						,0x58},
		{_F8_						,0x7B},
		{_F9_						,0x20},
		{_FA_						,0x00},
		{_FB_						,0x80},
		{_FC_						,0x80},
		{_FD_						,0x49},
		{_FE_						,0x37},
		{_FF_						,0x00},		
		{_C0_						,0x13},
		{_C1_						,0x13},
		{_C2_						,0x13},
		{_C3_						,0x13},
		{_C4_						,0x00},
		{_C5_						,0x00},
		{_C6_						,0x71},
		{_C7_						,0x71},
		{_C8_						,0x71},
		{_C9_						,0x71},
		{_CA_						,0x1C},
		{_CB_						,0x1C},
		{_CC_						,0x1C},
		{_CD_						,0x1C},
		{_CE_						,0x87},
		{_CF_						,0x87},
		{_D0_						,0x87},
		{_D1_						,0x87},
		{_D2_						,0x00},
		{_D3_						,0x00},
		{_D4_						,0x00},
		{_D5_						,0x00},
		{_D6_						,0x00},
		{_D7_						,0x00},
		{_D8_						,0x00},
		{_D9_						,0x00},
		{_DA_						,0x80},
		{_DB_						,0x10},
		{_DC_						,0x80},
		{_DD_						,0x10},
		{_DE_						,0x00},
		{_DF_						,0x00},
		{_E0_						,0x00},
		{_E1_						,0x00},
		{_E2_						,0x00},
		{_E3_						,0x00},
		{_E4_						,0x00},
		{_E5_						,0x00},
		{_E6_						,0x00},
		{_E7_						,0x00},
		{_E8_						,0x10},
		{_E9_						,0x11},
		{_EA_						,0x01},
		{_EB_						,0x40},
		{_EC_						,0x00},
		{_ED_						,0x00},
		{_EE_						,0x00},
		{_EF_						,0x00},
		{_A0_						,0x00},		

		// 108MHz
		{_00_						,0x00},
		{_01_						,0x00},
		{_02_						,0x00},
		{_03_						,0x00},
		{_04_						,0x00},
		{_05_						,0x10},
		{_06_						,0x32},
		{_07_						,0x10},
		{_08_						,0x32},
		{_09_						,0x10},
		{_0A_						,0x32},
		{_0B_						,0x10},
		{_0C_						,0x32},
		{_0D_						,0x00},
		{_0E_						,0x00},
		{_0F_						,0x00},
		{_10_						,0x00},
		{_11_						,0xEF},
		{_12_						,0xFF},
		{_13_						,0xFF},
		{_14_						,0xFF},
		{_15_						,0xFF},
		{_16_						,0xFF},
		{_17_						,0xFF},
		{_18_						,0xFF},
		{_19_						,0x00},
		{_1A_						,0x00},
		{_1B_						,0x72},
		{_1C_						,0x00},
		{_1D_						,0x00},
		{_1E_						,0x00},
		{_1F_						,0x00},
		{_20_						,0x60},
		{_21_						,0x00},
		{_22_						,0xAB},
		{_23_						,0x02},
		{_24_						,0x90},
		{_25_						,0x00},
		{_26_						,0x00},
		{_27_						,0x00},
		{_28_						,0x06},
		{_29_						,0x10},
		{_2A_						,0x95},
		{_2B_						,0x00},
		{_2C_						,0x00},
		{_2D_						,0x8A},
		{_2E_						,0x65},
		{_2F_						,0x06},
		{_30_						,0x60},
		{_31_						,0x00},
		{_32_						,0xAB},
		{_33_						,0x02},
		{_34_						,0x90},
		{_35_						,0x00},
		{_36_						,0x00},
		{_37_						,0x00},
		{_38_						,0x06},
		{_39_						,0x10},
		{_3A_						,0x95},
		{_3B_						,0x00},
		{_3C_						,0x00},
		{_3D_						,0x8A},
		{_3E_						,0x65},
		{_3F_						,0x16},
		{_40_						,0x60},
		{_41_						,0x00},
		{_42_						,0xAB},
		{_43_						,0x02},
		{_44_						,0x90},
		{_45_						,0x00},
		{_46_						,0x00},
		{_47_						,0x00},
		{_48_						,0x06},
		{_49_						,0x10},
		{_4A_						,0x95},
		{_4B_						,0x00},
		{_4C_						,0x00},
		{_4D_						,0x8A},
		{_4E_						,0x65},
		{_4F_						,0x26},
		{_50_						,0x60},
		{_51_						,0x00},
		{_52_						,0xAB},
		{_53_						,0x02},
		{_54_						,0x90},
		{_55_						,0x00},
		{_56_						,0x00},
		{_57_						,0x00},
		{_58_						,0x06},
		{_59_						,0x10},
		{_5A_						,0x95},
		{_5B_						,0x00},
		{_5C_						,0x00},
		{_5D_						,0x8A},
		{_5E_						,0x65},
		{_5F_						,0x36},
		{_60_						,0xD0},
		{_61_						,0x80},
		{_62_						,0x40},
		{_63_						,0x7C},
		{_64_						,0x9F},
		{_65_						,0x00},
		{_66_						,0x20},
		{_67_						,0x40},
		{_68_						,0x80},
		{_69_						,0x50},
		{_6A_						,0x38},
		{_6B_						,0x0F},
		{_6C_						,0x0C},
		{_6D_						,0x01},
		{_6E_						,0x15},
		{_6F_						,0x0A},
		{_70_						,0x80},
		{_71_						,0x23},
		{_72_						,0x88},
		{_73_						,0x04},
		{_74_						,0x2A},
		{_75_						,0xCC},
		{_76_						,0xF0},
		{_77_						,0x2F},
		{_78_						,0x57},
		{_79_						,0x43},
		{_7A_						,0x10},
		{_7B_						,0x88},
		{_7C_						,0x82},
		{_7D_						,0x63},
		{_7E_						,0x01},
		{_7F_						,0x00},
		{_80_						,0x80},
		{_81_						,0x00},
		{_82_						,0x00},
		{_83_						,0x81},
		{_84_						,0x01},
		{_85_						,0x00},
		{_86_						,0x00},
		{_87_						,0x00},
		{_88_						,0x00},
		{_89_						,0x20},
		{_8A_						,0x04},
		{_8B_						,0x2E},
		{_8C_						,0x00},
		{_8D_						,0x30},
		{_8E_						,0xB8},
		{_8F_						,0x01},
		{_90_						,0x06},
		{_91_						,0x06},
		{_92_						,0x11},
		{_93_						,0xB9},
		{_94_						,0xB2},
		{_95_						,0x05},
		{_96_						,0x00},
		{_97_						,0x28},
		{_98_						,0x50},
		{_99_						,0x51},
		{_9A_						,0xB5},
		{_9B_						,0x13},
		{_9C_						,0x03},
		{_9D_						,0x22},
		{_9E_						,0xFF},
		{_9F_						,0x00},

/*
		// 54MHz
		{_00_						,0x00},
		{_01_						,0x00},
		{_02_						,0x00},
		{_03_						,0x00},
		{_04_						,0x00},
		{_05_						,0x10},
		{_06_						,0x32},
		{_07_						,0x10},
		{_08_						,0x32},
		{_09_						,0x10},
		{_0A_						,0x32},
		{_0B_						,0x10},
		{_0C_						,0x32},
		{_0D_						,0x00},
		{_0E_						,0x00},
		{_0F_						,0x00},
		{_10_						,0x00},
		{_11_						,0xEF},
		{_12_						,0xFF},
		{_13_						,0xFF},
		{_14_						,0xFF},
		{_15_						,0xFF},
		{_16_						,0xFF},
		{_17_						,0xFF},
		{_18_						,0xFF},
		{_19_						,0x00},
		{_1A_						,0x00},
		{_1B_						,0x72},
		{_1C_						,0x00},
		{_1D_						,0x00},
		{_1E_						,0x00},
		{_1F_						,0x00},
		{_20_						,0x60},
		{_21_						,0x00},
		{_22_						,0xAB},
		{_23_						,0x02},
		{_24_						,0x90},
		{_25_						,0x00},
		{_26_						,0x00},
		{_27_						,0x00},
		{_28_						,0x06},
		{_29_						,0x10},
		{_2A_						,0x95},
		{_2B_						,0x00},
		{_2C_						,0x00},
		{_2D_						,0x8A},
		{_2E_						,0x40},
		{_2F_						,0x04},
		{_30_						,0x60},
		{_31_						,0x00},
		{_32_						,0xAB},
		{_33_						,0x02},
		{_34_						,0x90},
		{_35_						,0x00},
		{_36_						,0x00},
		{_37_						,0x00},
		{_38_						,0x06},
		{_39_						,0x10},
		{_3A_						,0x95},
		{_3B_						,0x00},
		{_3C_						,0x00},
		{_3D_						,0x8A},
		{_3E_						,0x40},
		{_3F_						,0x15},
		{_40_						,0x60},
		{_41_						,0x00},
		{_42_						,0xAB},
		{_43_						,0x02},
		{_44_						,0x90},
		{_45_						,0x00},
		{_46_						,0x00},
		{_47_						,0x00},
		{_48_						,0x06},
		{_49_						,0x10},
		{_4A_						,0x95},
		{_4B_						,0x00},
		{_4C_						,0x00},
		{_4D_						,0x8A},
		{_4E_						,0x40},
		{_4F_						,0x24},
		{_50_						,0x60},
		{_51_						,0x00},
		{_52_						,0xAB},
		{_53_						,0x02},
		{_54_						,0x90},
		{_55_						,0x00},
		{_56_						,0x00},
		{_57_						,0x00},
		{_58_						,0x06},
		{_59_						,0x10},
		{_5A_						,0x95},
		{_5B_						,0x00},
		{_5C_						,0x00},
		{_5D_						,0x8A},
		{_5E_						,0x40},
		{_5F_						,0x35},
		{_60_						,0xD0},
		{_61_						,0x80},
		{_62_						,0x40},
		{_63_						,0x7C},
		{_64_						,0x9F},
		{_65_						,0x00},
		{_66_						,0x20},
		{_67_						,0x40},
		{_68_						,0x80},
		{_69_						,0x50},
		{_6A_						,0x38},
		{_6B_						,0x0F},
		{_6C_						,0x0C},
		{_6D_						,0x01},
		{_6E_						,0x15},
		{_6F_						,0x0A},
		{_70_						,0x80},
		{_71_						,0x23},
		{_72_						,0x88},
		{_73_						,0x04},
		{_74_						,0x2A},
		{_75_						,0xCC},
		{_76_						,0xF0},
		{_77_						,0x2F},
		{_78_						,0x57},
		{_79_						,0x43},
		{_7A_						,0x10},
		{_7B_						,0x88},
		{_7C_						,0x82},
		{_7D_						,0x63},
		{_7E_						,0x01},
		{_7F_						,0x00},
		{_80_						,0x80},
		{_81_						,0x00},
		{_82_						,0x00},
		{_83_						,0x81},
		{_84_						,0x01},
		{_85_						,0x00},
		{_86_						,0x00},
		{_87_						,0x00},
		{_88_						,0x00},
		{_89_						,0x20},
		{_8A_						,0x04},
		{_8B_						,0x2E},
		{_8C_						,0x00},
		{_8D_						,0x30},
		{_8E_						,0xB8},
		{_8F_						,0x01},
		{_90_						,0x06},
		{_91_						,0x06},
		{_92_						,0x11},
		{_93_						,0xB9},
		{_94_						,0xB2},
		{_95_						,0x05},
		{_96_						,0x00},
		{_97_						,0x28},
		{_98_						,0x50},
		{_99_						,0x51},
		{_9A_						,0xB5},
		{_9B_						,0x13},
		{_9C_						,0x03},
		{_9D_						,0x22},
		{_9E_						,0xFF},
		{_9F_						,0x00},
*/
/*
		// 27MHz
		{_00_						,0x00},
		{_01_						,0x00},
		{_02_						,0x00},
		{_03_						,0x00},
		{_04_						,0x00},
		{_05_						,0x10},
		{_06_						,0x32},
		{_07_						,0x10},
		{_08_						,0x32},
		{_09_						,0x10},
		{_0A_						,0x32},
		{_0B_						,0x10},
		{_0C_						,0x32},
		{_0D_						,0x00},
		{_0E_						,0x00},
		{_0F_						,0x00},
		{_10_						,0x00},
		{_11_						,0xEF},
		{_12_						,0xFF},
		{_13_						,0xFF},
		{_14_						,0xFF},
		{_15_						,0xFF},
		{_16_						,0xFF},
		{_17_						,0xFF},
		{_18_						,0xFF},
		{_19_						,0x00},
		{_1A_						,0x00},
		{_1B_						,0x72},
		{_1C_						,0x00},
		{_1D_						,0x00},
		{_1E_						,0x00},
		{_1F_						,0x00},
		{_20_						,0x60},
		{_21_						,0x00},
		{_22_						,0xAB},
		{_23_						,0x02},
		{_24_						,0x90},
		{_25_						,0x00},
		{_26_						,0x00},
		{_27_						,0x00},
		{_28_						,0x06},
		{_29_						,0x10},
		{_2A_						,0x95},
		{_2B_						,0x00},
		{_2C_						,0x00},
		{_2D_						,0x8A},
		{_2E_						,0x10},
		{_2F_						,0x00},
		{_30_						,0x60},
		{_31_						,0x00},
		{_32_						,0xAB},
		{_33_						,0x02},
		{_34_						,0x90},
		{_35_						,0x00},
		{_36_						,0x00},
		{_37_						,0x00},
		{_38_						,0x06},
		{_39_						,0x10},
		{_3A_						,0x95},
		{_3B_						,0x00},
		{_3C_						,0x00},
		{_3D_						,0x8A},
		{_3E_						,0x10},
		{_3F_						,0x11},
		{_40_						,0x60},
		{_41_						,0x00},
		{_42_						,0xAB},
		{_43_						,0x02},
		{_44_						,0x90},
		{_45_						,0x00},
		{_46_						,0x00},
		{_47_						,0x00},
		{_48_						,0x06},
		{_49_						,0x10},
		{_4A_						,0x95},
		{_4B_						,0x00},
		{_4C_						,0x00},
		{_4D_						,0x8A},
		{_4E_						,0x10},
		{_4F_						,0x22},
		{_50_						,0x60},
		{_51_						,0x00},
		{_52_						,0xAB},
		{_53_						,0x02},
		{_54_						,0x90},
		{_55_						,0x00},
		{_56_						,0x00},
		{_57_						,0x00},
		{_58_						,0x06},
		{_59_						,0x10},
		{_5A_						,0x95},
		{_5B_						,0x00},
		{_5C_						,0x00},
		{_5D_						,0x8A},
		{_5E_						,0x10},
		{_5F_						,0x33},
		{_60_						,0xD0},
		{_61_						,0x80},
		{_62_						,0x40},
		{_63_						,0x7C},
		{_64_						,0x9F},
		{_65_						,0x00},
		{_66_						,0x20},
		{_67_						,0x40},
		{_68_						,0x80},
		{_69_						,0x50},
		{_6A_						,0x38},
		{_6B_						,0x0F},
		{_6C_						,0x0C},
		{_6D_						,0x01},
		{_6E_						,0x15},
		{_6F_						,0x0A},
		{_70_						,0x80},
		{_71_						,0x23},
		{_72_						,0x88},
		{_73_						,0x04},
		{_74_						,0x2A},
		{_75_						,0xCC},
		{_76_						,0xF0},
		{_77_						,0x2F},
		{_78_						,0x57},
		{_79_						,0x43},
		{_7A_						,0x10},
		{_7B_						,0x88},
		{_7C_						,0x82},
		{_7D_						,0x63},
		{_7E_						,0x01},
		{_7F_						,0x00},
		{_80_						,0x80},
		{_81_						,0x00},
		{_82_						,0x00},
		{_83_						,0x81},
		{_84_						,0x01},
		{_85_						,0x00},
		{_86_						,0x00},
		{_87_						,0x00},
		{_88_						,0x00},
		{_89_						,0x20},
		{_8A_						,0x04},
		{_8B_						,0x2E},
		{_8C_						,0x00},
		{_8D_						,0x30},
		{_8E_						,0xB8},
		{_8F_						,0x01},
		{_90_						,0x06},
		{_91_						,0x06},
		{_92_						,0x11},
		{_93_						,0xB9},
		{_94_						,0xB2},
		{_95_						,0x05},
		{_96_						,0x00},
		{_97_						,0x28},
		{_98_						,0x50},
		{_99_						,0x51},
		{_9A_						,0xB5},
		{_9B_						,0x13},
		{_9C_						,0x03},
		{_9D_						,0x22},
		{_9E_						,0xFF},
		{_9F_						,0x00},
*/
		{_88_						,0x00},		
		{_FF_						,0x01},		
		{_00_						,0x00},
		{_01_						,0x08},
		{_02_						,0x00},
		{_03_						,0x00},
		{_04_						,0x86},
		{_05_						,0x7A},
		{_06_						,0x7E},
		{_07_						,0x7A},
		{_08_						,0x00},
		{_09_						,0x00},
		{_10_						,0x00},
		{_11_						,0x00},
		{_12_						,0x00},
		{_13_						,0x00},
		{_14_						,0x00},
		{_15_						,0x00},
		{_16_						,0x00},
		{_17_						,0x00},
		{_18_						,0x00},
		{_19_						,0x00},
		{_1A_						,0x00},
		{_1B_						,0x00},
		{_1C_						,0x00},
		{_1D_						,0x00},
		{_1E_						,0x00},
		{_1F_						,0x00},
		{_20_						,0x00},
		{_21_						,0x00},
		{_22_						,0x00},
		{_23_						,0x00},
		{_24_						,0x00},
		{_25_						,0x00},
		{_26_						,0x00},
		{_27_						,0x00},
		{_28_						,0x00},
		{_29_						,0x00},
		{_2A_						,0x00},
		{_2B_						,0x00},
		{_2C_						,0x00},
		{_2D_						,0x00},
		{_2E_						,0x00},
		{_2F_						,0x00},
		{_30_						,0x00},
		{_31_						,0x00},
		{_32_						,0x00},
		{_33_						,0x00},
		{_34_						,0x00},
		{_35_						,0x00},
		{_36_						,0x00},
		{_37_						,0x00},
		{_38_						,0x00},
		{_39_						,0x00},
		{_3A_						,0x00},
		{_3B_						,0x00},
		{_3C_						,0x00},
		{_3D_						,0x00},
		{_3E_						,0x00},
		{_3F_						,0x00},
		{_40_						,0x00},
		{_41_						,0x00},
		{_42_						,0x00},
		{_43_						,0x00},
		{_44_						,0x00},
		{_45_						,0x00},
		{_46_						,0x00},
		{_47_						,0x00},
		{_48_						,0x00},
		{_49_						,0x00},
		{_4A_						,0x00},
		{_4B_						,0x00},
		{_4C_						,0x00},
		{_4D_						,0x00},
		{_4E_						,0x00},
		{_4F_						,0x00},
		{_50_						,0x00},
		{_51_						,0x00},
		{_52_						,0x00},
		{_53_						,0x00},
		{_54_						,0x00},
		{_55_						,0x00},
		{_56_						,0x00},
		{_57_						,0x00},
		{_58_						,0x00},
		{_59_						,0x00},
		{_5A_						,0x00},
		{_5B_						,0x00},
		{_5C_						,0x00},
		{_5D_						,0x00},
		{_5E_						,0x00},
		{_5F_						,0x00},
		{_60_						,0x00},
		{_61_						,0x00},
		{_62_						,0x00},
		{_63_						,0x00},
		{_64_						,0x00},
		{_65_						,0x00},
		{_66_						,0x00},
		{_67_						,0x00},
		{_68_						,0x00},
		{_69_						,0x00},
		{_6A_						,0x00},
		{_6B_						,0x00},
		{_6C_						,0x00},
		{_6D_						,0x00},
		{_6E_						,0x00},
		{_6F_						,0x00},
		{_70_						,0x00},
		{_71_						,0x00},
		{_72_						,0x00},
		{_73_						,0x00},
		{_74_						,0x00},
		{_75_						,0x00},
		{_76_						,0x00},
		{_77_						,0x00},
		{_78_						,0x00},
		{_79_						,0x00},
		{_7A_						,0x00},
		{_7B_						,0x00},
		{_7C_						,0x00},
		{_7D_						,0x00},
		{_7E_						,0x00},
		{_7F_						,0x00},
		{_80_						,0x00},
		{_81_						,0x00},
		{_82_						,0x00},
		{_83_						,0x00},
		{_84_						,0x00},
		{_85_						,0x00},
		{_86_						,0x00},
		{_87_						,0x00},
		{_88_						,0x00},
		{_89_						,0x00},
		{_8A_						,0x00},
		{_8B_						,0x00},
		{_8C_						,0x00},
		{_8D_						,0x00},
		{_8E_						,0x00},
		{_8F_						,0x00},
		{_90_						,0x00},
		{_91_						,0x00},
		{_A0_						,0x00},		
		{_FF_						,0x00},		
		{_F6_						,0x80},
		{_88_						,0x00}
};

/*==================================================================*/
static int NVP1114A_WriteReg(TNVP1114AInfo *ptInfo, ENVP1114ARegs eRegAddr, DWORD dwData)
{
	return ptInfo->pfnWriteReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, dwData);
}

/* Version 3.0.0.0 modification, 2012.12.13 */
static int NVP1114A_ReadReg(TNVP1114AInfo *ptInfo, ENVP1114ARegs eRegAddr, DWORD *pdwData)
{
	return ptInfo->pfnReadReg((DWORD)(ptInfo->dwFuncUserData), (DWORD)(ptInfo->dwDeviceAddr), eRegAddr, (DWORD*)pdwData);
}
/* ========================================= */	
/*-------------------------------------------------------------------------------------*/
static int NVP1114A_CompReg(TNVP1114AInfo *ptInfo, ENVP1114ARegs eRegAddr, DWORD dwMask, DWORD dwData)
{
	DWORD dwReadData;

	ptInfo->pfnReadReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, &dwReadData);
	return ((dwReadData&dwMask) == dwData);
}

/*-------------------------------------------------------------------------------------*/
static void NVP1114A_Reset(TNVP1114AInfo *ptInfo)
{
	DWORD i;

	for (i=0; i<NVP1114A_INTERLACE_CCIR656_TBL_SIZE; i++)
	{
		NVP1114A_WriteReg(ptInfo, atNVP1114AregInterlaceCCIR656Tbl[i].eRegAddr, atNVP1114AregInterlaceCCIR656Tbl[i].dwData);
	}
}

/* Version 3.0.0.0 modification, 2012.12.13 */
/*-------------------------------------------------------------------------------------*/
static void NVP1114A_SetStandard1(TNVP1114AInfo *ptInfo, ETVStd std, DWORD dwDevNum)
{
	DWORD chn, i;	

	chn = (dwDevNum>3) ? (dwDevNum-4) : dwDevNum;
	for (i=0; i<NVP1114A_STD_TBL_SIZE; i++)
	{
		NVP1114A_WriteReg(ptInfo, atNVP1114AregStdTbl[std][i].eRegAddr+(chn<<4), atNVP1114AregStdTbl[std][i].dwData);
	}
}

/*-------------------------------------------------------------------------------------*/
void NVP1114A_SetStandard(DWORD dwDevNum, ETVStd std)
{
	DWORD NVP1114A_dev;
	down_interruptible(&nvp1114a_mutex);
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		NVP1114A_dev = 0;
	}
	else
	{ // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		NVP1114A_dev = 1;
	}
	NVP1114A_SetStandard1(ptInfo[NVP1114A_dev], std, dwDevNum);
	up(&nvp1114a_mutex); 
}

/*-------------------------------------------------------------------------------------*/
static void NVP1114A_AutoDetect1(TNVP1114AInfo *ptInfo, TAutoDetectStdInfo *ptStdInfo, DWORD dwDevNum)
{
	DWORD chn;	
	DWORD reg, data1 = 0, data = 0;

	chn = (dwDevNum>3) ? (dwDevNum-4) : dwDevNum;
	// check data loss 
	reg = 0x00;
	NVP1114A_ReadReg(ptInfo, reg, &data);
	reg = 0x04;
	NVP1114A_ReadReg(ptInfo, reg, &data1);
	if (!((data>>4)&(0x1<<chn)))
	{	// video detect
		ptStdInfo->bDataLost = 0;
		// detect standard
		// PAL
		if ((data1>>4)&(0x1<<chn))
		{ 
			ptStdInfo->eDetectStd = TVSTD_PAL;
		}
		// NTSC
		else
		{ 
			ptStdInfo->eDetectStd = TVSTD_NTSC;
		}
	}
	else
	{ // video lost
		ptStdInfo->bDataLost = 1;
	}
} 

/*-------------------------------------------------------------------------------------*/
void NVP1114A_AutoDetect(DWORD dwDevNum, TAutoDetectStdInfo *ptStdInfo)
{
	DWORD NVP1114A_dev;
	
	down_interruptible(&nvp1114a_mutex);
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		NVP1114A_dev = 0;
	}
	else
	{ // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		NVP1114A_dev = 1;
	}
	NVP1114A_AutoDetect1(ptInfo[NVP1114A_dev], ptStdInfo, dwDevNum);
	up(&nvp1114a_mutex); 
}
/* ======================================== */		

/*==================================================================*/
int  NVP1114A_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	int ret = 0;
	DWORD NVP1114A_dev;

/* Version 1.0.0.3 modification, 2010.08.02 */	
	if (down_interruptible(&nvp1114a_mutex))
	{
		return -ERESTARTSYS;	
	}
/* ========================================= */	

	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		NVP1114A_dev = 0;
	}
	else
	{ // (devNum==4) || (devNum==5) || (devNum==6) || (devNum==7)
		NVP1114A_dev = 1;
	}	

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:		
		NVP1114A_Reset(ptInfo[NVP1114A_dev]);
		break;
/* Version 3.0.0.0 modification, 2012.12.13 */
		case VIDEO_SIGNAL_OPTION_AUTO_DETECT_STD:
		NVP1114A_AutoDetect1(ptInfo[NVP1114A_dev], (TAutoDetectStdInfo *)(ptIoctlArg->adwUserData[0]), dwDevNum);		
		break;
/* ======================================== */				
		default:
		ret = -EPERM;
	}
/* Version 1.0.0.3 modification, 2010.08.02 */	
	up(&nvp1114a_mutex);	
/* ========================================= */	
	
	return ret;
}

/*-------------------------------------------------------------------------------------*/
void NVP1114A_Release(DWORD dwDevNum)
{
	DWORD NVP1114A_dev;

/* Version 1.0.0.4 modification, 2010.08.16 */	
	down_interruptible(&nvp1114a_mutex);
/* ========================================= */	

	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		NVP1114A_dev = 0;
	}
	else
	{ // (devNum==4) || (devNum==5) || (devNum==6) || (devNum==7)
		NVP1114A_dev = 1;
	}
/* Version 1.0.0.4 modification, 2010.08.16 */	
	if (NVP1114A_DevInUse[NVP1114A_dev] > 1)
	{ // NVP1114A_DEV0 is still used by some other streams
/* ========================================= */	
		NVP1114A_DevInUse[NVP1114A_dev]--;	
	}
	else
	{
		IICCtrl_Release((void*)(&(ptInfo[NVP1114A_dev]->dwFuncUserData)));
		if (ptInfo[NVP1114A_dev])
		{
			kfree(ptInfo[NVP1114A_dev]);
		}
		NVP1114A_DevInUse[NVP1114A_dev] = 0;
	}	
/* Version 1.0.0.3 modification, 2010.08.02 */	
	up(&nvp1114a_mutex);	
/* ========================================= */	

}

/*-------------------------------------------------------------------------------------*/
int NVP1114A_Open(TVideoSensorInitialParam* ptNVP1114AInitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;
	DWORD NVP1114A_dev;

/* Version 2.0.0.0 modification, 2011.01.27 */
	if (((ptNVP1114AInitialParam->dwVideoSensorVersion&0x00FF00FF)!=(NVP1114A_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptNVP1114AInitialParam->dwVideoSensorVersion&0x0000FF00)>(NVP1114A_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid NVP1114A device driver version %d.%d.%d.%d !!\n", 
				(int)(NVP1114A_INTERFACE_VERSION&0xFF), 
				(int)((NVP1114A_INTERFACE_VERSION>>8)&0xFF), 
				(int)((NVP1114A_INTERFACE_VERSION>>16)&0xFF), 
				(int)(NVP1114A_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

/* Version 1.0.0.3 modification, 2010.08.02 */	
	if (down_interruptible(&nvp1114a_mutex))
	{
		return -ERESTARTSYS;
	}
/* ========================================= */	

	// 1 determine which hardware device should be opened
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		NVP1114A_dev = 0;
	}
	else
	{ // (devNum==4) || (devNum==5) || (devNum==6) || (devNum==7)
		NVP1114A_dev = 1;
	}
	// 2 check if this hardware has been opened
	if (NVP1114A_DevInUse[NVP1114A_dev] )
	{ // NVP1114A_DEV has been opened
/* Version 3.0.0.0 modification, 2012.12.13 */
		NVP1114A_SetStandard1(ptInfo[NVP1114A_dev], (ETVStd)dwDefaultStandard, dwDevNum);
/* ========================================= */
		NVP1114A_DevInUse[NVP1114A_dev]++;		
/* Version 1.0.0.3 modification, 2010.08.02 */	
		up(&nvp1114a_mutex);			
/* ========================================= */		
		return 0;
	}
	// 3 open NVP1114A_DEV
	// 3.1 memory allocate
	if (!(ptInfo[NVP1114A_dev] = (TNVP1114AInfo *)kmalloc(sizeof(TNVP1114AInfo), GFP_KERNEL)))
	{
		printk("[NVP1114A] : Allocate %d bytes memory fail\n", sizeof(TNVP1114AInfo));
		NVP1114A_Release(dwDevNum);							
/* Version 1.0.0.3 modification, 2010.08.02 */	
		up(&nvp1114a_mutex);	
/* ========================================= */											
		return -ENOMEM;
	}
	// 3.2 init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_DATA_NOSTOP;
	tIICCtrlInitOptions.dwMaxDataLength = 2;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[NVP1114A] : Initial IICCtrl object fail !!\n");
		NVP1114A_Release(dwDevNum);				
/* Version 1.0.0.3 modification, 2010.08.02 */	
		up(&nvp1114a_mutex);	
/* ========================================= */															
		return -ENODEV;
	}
	// 3.3 init private data		
	ptInfo[NVP1114A_dev]->dwDeviceAddr = NVP1114A_DevAddr[NVP1114A_dev];
	ptInfo[NVP1114A_dev]->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo[NVP1114A_dev]->pfnReadReg = (FOnReadReg_t)IICCtrl_ReadReg;
	ptInfo[NVP1114A_dev]->pfnWriteReg = (FOnWriteReg_t)IICCtrl_WriteReg;		
	// 3.4 detect device
	if (!NVP1114A_CompReg(ptInfo[NVP1114A_dev], _1B_, 0xFF, 0x72))
	{
		printk("[NVP1114A] : device doesn't exist in device address 0x%02lX\n", ptInfo[NVP1114A_dev]->dwDeviceAddr);
		NVP1114A_Release(dwDevNum);				
/* Version 1.0.0.3 modification, 2010.08.02 */	
		up(&nvp1114a_mutex);				
/* ========================================= */												
		return -ENODEV;
	}
	// 3.5 reset device
	NVP1114A_Reset(ptInfo[NVP1114A_dev]);
/* Version 3.0.0.0 modification, 2012.12.13 */
	// 3.6 set default standard
	NVP1114A_SetStandard1(ptInfo[NVP1114A_dev], (ETVStd)dwDefaultStandard, dwDevNum);
/* ========================================= */	
	NVP1114A_DevInUse[NVP1114A_dev] = 1;	
/* Version 1.0.0.3 modification, 2010.08.02 */	
	up(&nvp1114a_mutex);						
/* ========================================= */							
	return 0;
}

/*-------------------------------------------------------------------------------------*/
TVideoSensorDevice sensor_dev_ops = {
	.open = 	NVP1114A_Open,
	.release = 	NVP1114A_Release,
	.ioctl = 	NVP1114A_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
	.get_shutter_value = NULL,
/* Version 1.0.0.1 modification, 2010.06.03 */	
	.get_remaining_line_num = NULL,
/* ========================================= */	
/* Version 1.0.0.2 modification, 2010.06.21 */	
	.group_access = NULL,		
/* ========================================= */
/* Version 2.0.0.2 modification, 2012.09.04 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = NULL,
/* ======================================== */
/* Version 3.0.0.0 modification, 2012.12.13 */
	.setup_standard = NVP1114A_SetStandard,	
	.detect_standard = NVP1114A_AutoDetect,
/* ========================================= */	
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct NVP1114A_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t NVP1114A_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct NVP1114A_attribute *NVP1114A_attr = container_of(attr, struct NVP1114A_attribute, attr);
	int result = 0;
	
	if (NVP1114A_attr->show)
		result = NVP1114A_attr->show(kobj, buf);	
	return result;
}

ssize_t NVP1114A_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "NVP1114A");
	return retval;
}

ssize_t NVP1114A_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
/* Version 2.0.0.1 modification, 2012.07.12 */	
	retval = sprintf (buf, "%s\n", "6");
/* ========================================= */	
	return retval;
}

ssize_t NVP1114A_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "720");
	return retval;
}

ssize_t NVP1114A_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
/* Version 2.0.0.3 modification, 2012.09.04 */		
	retval = sprintf (buf, "%s\n", "576");
/* ========================================= */	
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject NVP1114A_kobj;

struct NVP1114A_attribute NVP1114A_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NVP1114A_SensorName_attr_show,
};

struct NVP1114A_attribute NVP1114A_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NVP1114A_dwBufNum_attr_show,
};

struct NVP1114A_attribute NVP1114A_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NVP1114A_MaxFrameWidth_attr_show,
};

struct NVP1114A_attribute NVP1114A_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NVP1114A_MaxFrameHeight_attr_show,
};

struct sysfs_ops NVP1114A_sysfs_ops = {
	.show	= NVP1114A_default_attr_show,
};

struct kobj_type NVP1114A_ktype = {
	.sysfs_ops	= &NVP1114A_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int NVP1114A_Init(void)
{
	int result = 0;
	
	if (result < 0){
		printk("[NVP1114A] : kobj set name fail!\n");
		return -1;
	}

	NVP1114A_kobj.ktype = &NVP1114A_ktype;	
	result = kobject_init_and_add(&NVP1114A_kobj, &NVP1114A_ktype, NULL, "VideoSensorInfo");

	if (result < 0) {
		printk("[NVP1114A] : Cannot register kobject [NVP1114A_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&NVP1114A_kobj, &NVP1114A_SensorName_attr.attr);		
       if (result < 0) {
		printk("[NVP1114A] : Cannot create NVP1114A_SensorName_attr.attr.\n");
		return -1;
       }

	result = sysfs_create_file(&NVP1114A_kobj, &NVP1114A_dwBufNum_attr.attr);		
       if (result < 0) {
		printk("[NVP1114A] : Cannot create NVP1114A_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&NVP1114A_kobj, &NVP1114A_MaxFrameWidth_attr.attr);		
       if (result < 0) {
		printk("[NVP1114A] : Cannot create NVP1114A_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&NVP1114A_kobj, &NVP1114A_MaxFrameHeight_attr.attr);		
       if (result < 0) {
		printk("[NVP1114A] : Cannot create NVP1114A_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
/* Version 1.0.0.3 modification, 2010.08.02 */	
	sema_init(&nvp1114a_mutex, 1);
/* ========================================= */	
			   
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void NVP1114A_Exit(void)
{
	sysfs_remove_file(&NVP1114A_kobj, &(NVP1114A_SensorName_attr.attr));
	sysfs_remove_file(&NVP1114A_kobj, &(NVP1114A_dwBufNum_attr.attr));	
/* Version 1.0.0.2 modification, 2010.06.21 */	
	sysfs_remove_file(&NVP1114A_kobj, &(NVP1114A_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&NVP1114A_kobj, &(NVP1114A_MaxFrameHeight_attr.attr));	
/* ========================================= */		

	kobject_put(&NVP1114A_kobj);	
	printk(KERN_DEBUG "[NVP1114A] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(NVP1114A_Init);
module_exit(NVP1114A_Exit);
