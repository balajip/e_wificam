/*
 * MN34030PL
 * Driver for MN34030PL sensor.
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

#include "MN34030PL.h"
#include "MN34030PL_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR MN34030PL_ID[] = "$Version: "MN34030PL_ID_VERSION"  (MN34030PL DRIVER) $";

/*==================================================================*/
static TMN34030PLInfo *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
/* Version 1.0.0.5 modification, 2010.08.02 */
static struct semaphore mn34030pl_mutex;
/* ========================================= */		

/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 2.0.0.5 modification, 2011.11.09 */
static DWORD dwMN34030PLHsyncX10 = MN34030PL_HSYNC_1280x1024_60HZ_X10;
static DWORD dwMN34030PLVsync = MN34030PL_VSYNC_1280x1024_60HZ;
/* ======================================== */
static DWORD dwMN34030PLPixelClock = MN34030PL_PIXEL_CLOCK_30FPS_M;

const DWORD adwLog10Tbl[32] = 
{
	102,104,107,109,111,114,116,119,121,124,127,130,132,135,138,141, 
	144,147,151,154,157,161,164,168,172,175,179,183,187,191,195,200 
};
/* ======================================== */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("MN34030PL driver");
MODULE_LICENSE("GPL");

/* Version 1.0.0.4 modification, 2010.08.02 */
static SOCKET iEnSerialIF = 0;
module_param(iEnSerialIF, int, S_IRUGO|S_IWUSR);
const TMN34030PLRegAddrData atMN34030PLSerialRegTbl[MN34030PL_SERIAL_TBL_SIZE] =
{

	//	216MHz serial clock, 108 parallel clock
	{_0001_					, 0x0030},
	{_0002_					, 0x0002},
	{_0004_					, 0x0546},
	{_0000_					, 0x0003},
	{_0003_					, 0x4226},
	{_0000_					, 0x0013},
	{_0005_					, 0x0000},
	{_0006_					, 0x2AAA},
	{_0007_					, 0x0000},
	{_0008_					, 0x0000},
	{_0100_					, 0x034B},
	{_0101_					, 0x034B},
	{_0102_					, 0x034B},
	{_0103_					, 0x01A5},
	{_0104_					, 0x0000},
	{_0105_					, 0x0000},
	{_0106_					, 0x0000},
	{_0107_					, 0x0000},
	{_0108_					, 0x0008},
	{_0109_					, 0x0009},
	{_010A_					, 0x034A},
	{_010B_					, 0x0008},
	{_010C_					, 0x0000},
	{_010D_					, 0x0008},
	{_010E_					, 0x0009},
	{_010F_					, 0x034A},
	{_0110_					, 0x0000},
	{_0111_					, 0x0003},
	{_0112_					, 0x0009},
	{_0113_					, 0x034A},
	{_0114_					, 0x033E},
	{_0115_					, 0x0341},
	{_0116_					, 0x07FF},
	{_0117_					, 0x0000},
	{_0118_					, 0x0000},
	{_0119_					, 0x0005},
	{_011A_					, 0x001F},
	{_011B_					, 0x0000},
	{_011C_					, 0x0005},
	{_011D_					, 0x0013},
	{_011E_					, 0x001D},
	{_011F_					, 0x001F},
	{_0120_					, 0x0000},
	{_0121_					, 0x0007},
	{_0122_					, 0x0006},
	{_0123_					, 0x0000},
	{_0124_					, 0x032B},
	{_0125_					, 0x0330},
	{_0126_					, 0x0002},
	{_0127_					, 0x0343},
	{_0130_					, 0x0000},
	{_0131_					, 0x0000},
	{_0132_					, 0x0000},
	{_0133_					, 0x0000},
	{_0134_					, 0x0000},
	{_0135_					, 0x07FF},
	{_0136_					, 0x0000},
	{_0137_					, 0x0000},
	{_0138_					, 0x0000},
	{_0139_					, 0x0000},
	{_013A_					, 0x0000},
	{_0140_					, 0x0009},
	{_0141_					, 0x00D0},
	{_0142_					, 0x0007},
	{_0143_					, 0x031D},
	{_0144_					, 0x0008},
	{_0145_					, 0x00CF},
	{_0146_					, 0x0006},
	{_0147_					, 0x031C},
	{_0148_					, 0x0000},
	{_0149_					, 0x0000},
	{_014A_					, 0x0008},
	{_014B_					, 0x0006},
	{_014C_					, 0x0000},
	{_014D_					, 0x0002},
	{_014E_					, 0x0002},
	{_014F_					, 0x0007},
	{_0150_					, 0x0008},
	{_0151_					, 0x0006},
	{_0152_					, 0x0009},
	{_0160_					, 0x0005},
	{_0161_					, 0x0008},
	{_0162_					, 0x0003},
	{_0163_					, 0x0005},
	{_0164_					, 0x0007},
	{_0165_					, 0x0017},
	{_0166_					, 0x001F},
	{_0167_					, 0x0163},
	{_0168_					, 0x0003},
	{_0169_					, 0x0004},
	{_016A_					, 0x0000},
	{_016B_					, 0x0000},
	{_0170_					, 0x0335},
	{_0171_					, 0x0343},
	{_0172_					, 0x0000},
	{_0173_					, 0x0000},
	{_0174_					, 0x0000},
	{_0175_					, 0x0000},
	{_0176_					, 0x0000},
	{_0177_					, 0x0000},
	{_0178_					, 0x0000},
	{_0179_					, 0x0000},
	{_017C_					, 0x0000},
	{_017D_					, 0x0000},
	{_017E_					, 0x0000},
	{_017F_					, 0x0000},
	{_0190_					, 0x0007},
	{_0191_					, 0x0181},
	{_0192_					, 0x0013},
	{_0193_					, 0x0015},
	{_0194_					, 0x0016},
	{_0195_					, 0x0017},
	{_0196_					, 0x0015},
	{_0197_					, 0x0173},
	{_0198_					, 0x0173},
	{_0199_					, 0x0173},
	{_019A_					, 0x0015},
	{_019B_					, 0x0173},
	{_01A0_					, 0x0429},
	{_01A1_					, 0x0000},
	{_01A2_					, 0x0429},
	{_01A3_					, 0x041B},
	{_01A4_					, 0x0429},
	{_01A5_					, 0x041B},
	{_01A6_					, 0x0000},
	{_01A7_					, 0x041C},
	{_01A8_					, 0x0001},
	{_01A9_					, 0x0009},
	{_01AA_					, 0x0008},
	{_01AB_					, 0x0009},
	{_01AC_					, 0x0000},
	{_01AD_					, 0x041C},
	{_01AE_					, 0x0000},
	{_01AF_					, 0x0000},
	{_01B0_					, 0x0000},
	{_01B1_					, 0x041B},
	{_01B2_					, 0x0000},
	{_01B3_					, 0x0000},
	{_01C0_					, 0x0010},
	{_01C1_					, 0x0417},
	{_01C2_					, 0x0010},
	{_01C3_					, 0x0417},
	{_01C4_					, 0x0000},
	{_01C5_					, 0x0000},
	{_01C6_					, 0x0000},
	{_01C7_					, 0x0000},
	{_01C8_					, 0x000F},
	{_01D0_					, 0x0000},
	{_01D1_					, 0x0000},
	{_01D2_					, 0x0000},
	{_01D3_					, 0x0000},
	{_01E0_					, 0x000A},
	{_01E1_					, 0x0023},
	{_01E2_					, 0x0023},
	{_01E3_					, 0x0023},
	{_01E4_					, 0x0026},
	{_01E5_					, 0x0026},
	{_01E6_					, 0x0026},
	{_01E7_					, 0x004A},
	{_01E8_					, 0x008C},
	{_01E9_					, 0x008C},
	{_01EA_					, 0x008C},
	{_01EB_					, 0x0087},
	{_01EC_					, 0x00CF},
	{_01ED_					, 0x00D8},
	{_01EE_					, 0x0116},
	{_01EF_					, 0x0324},
	{_01F0_					, 0x0003},
	{_01F1_					, 0x001F},
	{_01F2_					, 0x00B3},
	{_01F3_					, 0x01BC},
	{_0020_					, 0x0080},
	{_0021_					, 0x0000},
	{_0022_					, 0x0000},
	{_0023_					, 0x000F},
	{_0024_					, 0x0000},
	{_0025_					, 0x0000},
	{_0026_					, 0x0000},
	{_0027_					, 0x0000},
	{_0028_					, 0x0000},
	{_0029_					, 0x0000},
	{_0030_					, 0x5420},
	{_0031_					, 0x111A},
	{_0032_					, 0x0460},
	{_0033_					, 0x0000},
	{_0034_					, 0x0000},
	{_0035_					, 0x20E0},
	{_0036_					, 0x0400},
	{_0037_					, 0x0000},
	{_003E_					, 0x014E},
	{_003F_					, 0x0000},
	{_0040_					, 0x0000},
	{_0041_					, 0x0200},
/* Version 2.0.0.6 modification, 2012.09.04 */
	{_0042_					, 0x0000},
/* ======================================== */
	{_0043_					, 0x7055},
	{_0046_					, 0x0000},
	{_004C_					, 0x0000},
	{_004E_					, 0x0FFF},
	{_004F_					, 0x0000},
	{_0050_					, 0x0FFE},
	{_0057_					, 0x0010},
	{_005C_					, 0x0038},
	{_0060_					, 0x0000},
	{_0061_					, 0x0200},
/* Version 2.0.0.6 modification, 2012.09.04 */
	{_0062_					, 0x0000},
/* ======================================== */
	{_0063_					, 0x7055},
	{_0066_					, 0x0000},
	{_006C_					, 0x0000},
	{_006D_					, 0x0000},
	{_006E_					, 0x0FFF},
	{_006F_					, 0x0000},
	{_0070_					, 0x0FFE},
	{_0077_					, 0x0010},
	{_007C_					, 0x0038},
	{_0080_					, 0x443C},
	{_0081_					, 0xFFFF},
	{_0082_					, 0x0000},
	{_0083_					, 0x0000},
	{_0084_					, 0x0000},
	{_0085_					, 0x0004},
	{_0086_					, 0x1032},
	{_0087_					, 0x0000},
	{_0088_					, 0x0001},
	{_0089_					, 0x0000},
	{_0090_					, 0x0012},
	{_0091_					, 0x0013},
	{_0092_					, 0x0010},
	{_0093_					, 0x0011},
	{_0094_					, 0x0016},
	{_0095_					, 0x0017},
	{_0096_					, 0x0014},
	{_0097_					, 0x0015},
	{_0098_					, 0x0002},
	{_0099_					, 0x0003},
	{_009A_					, 0x0000},
	{_009B_					, 0x0001},
	{_009C_					, 0x0006},
	{_009D_					, 0x0007},
	{_009E_					, 0x0004},
	{_009F_					, 0x0005},
	{_00A0_					, 0x0000},
	{_00A1_					, 0x0320},
	{_00A2_					, 0x0000},
	{_00A3_					, 0x0000},
	{_00A4_					, 0x0000},
	{_00A9_					, 0x0000},
	{_00C0_					, 0x1155},
	{_00C1_					, 0xC5D5},
	{_00C2_					, 0x93D5},
	{_00C3_					, 0x03FC},
	{_00C4_					, 0xF041},
	{_00C5_					, 0x7FC4},
	{_00C6_					, 0x0F54},
	{_00C7_					, 0x0051},
	{_00C8_					, 0x1541},
	{_00C9_					, 0x0200},
	{_00CA_					, 0x0100},
	{_00CB_					, 0x0000},
	{_00CC_					, 0x0000},
	{_00CE_					, 0x0000},
	{_00D1_					, 0x041B},
	{_00D2_					, 0x0000},
	{_00D3_					, 0x041B},
	{_0000_					, 0x0113}
	// 243MHz serial clock, 121.5 parallel clock
	//{_0001_					, 0x0036},
	//{_0002_					, 0x0002},
	//{_0004_					, 0x0546},
	//{_0000_					, 0x0003},
	//{_0003_					, 0x4206},
	//{_0000_					, 0x0013},
	//{_0005_					, 0x0000},
	//{_0100_					, 0x02CF},
	//{_0101_					, 0x02CF},
	//{_0102_					, 0x0383},
	//{_0103_					, 0x01C1},
	//{_0109_					, 0x0009},
	//{_010A_					, 0x02CE},
	//{_010B_					, 0x0008},
	//{_010E_					, 0x0009},
	//{_010F_					, 0x02CE},
	//{_0111_					, 0x0003},
	//{_0112_					, 0x0009},
	//{_0113_					, 0x02CE},
	//{_0114_					, 0x02C2},
	//{_0115_					, 0x02C5},
	//{_0116_					, 0x07FF},
	//{_0117_					, 0x0000},
	//{_0119_					, 0x0005},
	//{_011A_					, 0x001F},
	//{_011C_					, 0x0005},
	//{_011D_					, 0x0013},
	//{_011E_					, 0x001D},
	//{_011F_					, 0x001F},
	//{_0121_					, 0x0007},
	//{_0122_					, 0x0006},
	//{_0124_					, 0x02AF},
	//{_0125_					, 0x02B4},
	//{_0127_					, 0x02C7},
	//{_0135_					, 0x07FF},
	//{_0136_					, 0x0000},
	//{_0137_					, 0x0000},
	//{_0138_					, 0x0000},
	//{_0139_					, 0x0000},
	//{_013A_					, 0x0000},
	//{_0141_					, 0x00FE},
	//{_0143_					, 0x034A},
	//{_0145_					, 0x00FD},
	//{_0147_					, 0x0349},
	//{_014F_					, 0x0007},
	//{_0152_					, 0x0009},
	//{_0160_					, 0x0005},
	//{_0161_					, 0x0008},
	//{_0162_					, 0x0003},
	//{_0163_					, 0x0005},
	//{_0164_					, 0x0007},
	//{_0165_					, 0x0017},
	//{_0166_					, 0x001F},
	//{_0167_					, 0x0163},
	//{_0168_					, 0x0003},
	//{_0169_					, 0x0004},
	//{_0170_					, 0x02B9},
	//{_0171_					, 0x02C7},
	//{_0174_					, 0x0000},
	//{_0175_					, 0x0000},
	//{_0178_					, 0x0000},
	//{_0179_					, 0x0000},
	//{_0190_					, 0x0007},
	//{_0191_					, 0x0181},
	//{_0192_					, 0x0013},
	//{_0193_					, 0x0015},
	//{_0194_					, 0x0016},
	//{_0195_					, 0x0017},
	//{_0196_					, 0x0015},
	//{_0197_					, 0x0173},
	//{_0198_					, 0x0173},
	//{_0199_					, 0x0173},
	//{_019A_					, 0x0015},
	//{_019B_					, 0x0173},
	//{_01A0_					, 0x0464},
	//{_01A2_					, 0x0464},
	//{_01A3_					, 0x042D},
	//{_01A4_					, 0x0464},
	//{_01A5_					, 0x042D},
	//{_01A7_					, 0x042E},
	//{_01A8_					, 0x0001},
	//{_01A9_					, 0x0009},
	//{_01AA_					, 0x0008},
	//{_01AB_					, 0x0009},
	//{_01AC_					, 0x0000},
	//{_01AD_					, 0x042D},
	//{_01AE_					, 0x0000},
	//{_01AF_					, 0x0000},
	//{_01B1_					, 0x042D},
	//{_01B3_					, 0x0000},
	//{_01C0_					, 0x0010},
	//{_01C1_					, 0x0417},
	//{_01C2_					, 0x0010},
	//{_01C3_					, 0x0417},
	//{_01C6_					, 0x0000},
	//{_01C7_					, 0x0000},
	//{_01C8_					, 0x000F},
	//{_01E0_					, 0x000A},
	//{_01E1_					, 0x0023},
	//{_01E2_					, 0x0023},
	//{_01E3_					, 0x0023},
	//{_01E4_					, 0x0036},
	//{_01E5_					, 0x0036},
	//{_01E6_					, 0x0036},
	//{_01E7_					, 0x0059},
	//{_01E8_					, 0x0095},
	//{_01E9_					, 0x0095},
	//{_01EA_					, 0x0095},
	//{_01EB_					, 0x00B5},
	//{_01EC_					, 0x00FF},
	//{_01ED_					, 0x00D4},
	//{_01EE_					, 0x0143},
	//{_01EF_					, 0x02A8},
	//{_01F1_					, 0x001F},
	//{_01F2_					, 0x00B3},
	//{_01F3_					, 0x01BC},
	//{_0020_					, 0x0080},
	//{_0021_					, 0x0000},
	//{_0023_					, 0x000F},
	//{_0030_					, 0x5420},
	//{_0031_					, 0x111A},
	//{_0032_					, 0x0460},
	//{_0035_					, 0x20E0},
	//{_0036_					, 0x0400},
	//{_003E_					, 0x014E},
	//{_0041_					, 0x0200},
	//{_0043_					, 0x7057},
	//{_004E_					, 0x0FFF},
	//{_0050_					, 0x0FFF},
	//{_0057_					, 0x0010},
	//{_005C_					, 0x0038},
	//{_0061_					, 0x0200},
	//{_0063_					, 0x7057},
	//{_006E_					, 0x0FFF},
	//{_0070_					, 0x0FFF},
	//{_0077_					, 0x0010},
	//{_007C_					, 0x0038},
	//{_0080_					, 0x443C},
	//{_0086_					, 0x1032},
	//{_0088_					, 0x0001},
	//{_0090_					, 0x0012},
	//{_0091_					, 0x0013},
	//{_0092_					, 0x0010},
	//{_0093_					, 0x0011},
	//{_0094_					, 0x0016},
	//{_0095_					, 0x0017},
	//{_0096_					, 0x0014},
	//{_0097_					, 0x0015},
	//{_0098_					, 0x0002},
	//{_0099_					, 0x0003},
	//{_009A_					, 0x0000},
	//{_009B_					, 0x0001},
	//{_009C_					, 0x0006},
	//{_009D_					, 0x0007},
	//{_009E_					, 0x0004},
	//{_009F_					, 0x0005},
	//{_00A0_					, 0x0000},
	//{_00A1_					, 0x0000},
	//{_00A3_					, 0x0000},
	//{_00A9_					, 0x0000},
	//{_00C1_					, 0xC5D5},
	//{_00C2_					, 0x93D5},
	//{_00C3_					, 0x03FC},
	//{_00C7_					, 0x0051},
	//{_00C8_					, 0x1541},
	//{_00C9_					, 0x0200},
	//{_00CA_					, 0x0100},
	//{_00CE_					, 0x0000},
	//{_0000_					, 0x0113}
};
/* ================================================================= */
/*==================================================================*/
const TMN34030PLRegAddrData atMN34030PLregTbl[MN34030PL_TBL_SIZE] = {
	{_0001_                 , 0x0030},
	{_0002_                 , 0x0002},
	{_0004_                 , 0x0546},
	{_0000_                 , 0x0003},
	{_0003_                 , 0x8220},
	{_0000_                 , 0x0013},
	{_0005_                 , 0x0000},
	{_0020_                 , 0x0080},
	{_0021_                 , 0x0000},
	{_0022_                 , 0x0000},
	{_0023_                 , 0x000F},
	{_0024_                 , 0x0000},
	{_0025_                 , 0x0000},
	{_0026_                 , 0x0000},
	{_0027_                 , 0x0000},
	{_0028_                 , 0x0008},
	{_0029_                 , 0x0000},
	{_0030_                 , 0x5420},
	{_0031_                 , 0x111A},
	{_0032_                 , 0x0460},
	{_0033_                 , 0x0000},
	{_0034_                 , 0x0000},
	{_0035_                 , 0x20E0},
	{_0036_                 , 0x0400},
	{_0037_                 , 0x0000},
	{_003E_                 , 0x014E},
	{_003F_                 , 0x0000},
	{_0040_                 , 0x0000},
	{_0041_                 , 0x0200},
/* Version 2.0.0.6 modification, 2012.09.04 */
	{_0042_                 , 0x0000},
/* ======================================== */
	{_0043_                 , 0x7057},
	{_0046_                 , 0x0000},
	{_004C_                 , 0x0000},
	{_004E_                 , 0x0FFF},
	{_004F_                 , 0x0000},
	{_0050_                 , 0x0FFE},
	{_0057_                 , 0x0010},
	{_005C_                 , 0x0038},
	{_0060_                 , 0x0000},
	{_0061_                 , 0x0200},
/* Version 2.0.0.6 modification, 2012.09.04 */
	{_0062_                 , 0x0000},
/* ======================================== */
	{_0063_                 , 0x7057},
	{_0066_                 , 0x0000},
	{_006C_                 , 0x0000},
	{_006D_                , 0x0000},
	{_006E_                 , 0x0FFF},
	{_006F_                 , 0x0000},
	{_0070_                 , 0x0FFE},
	{_0077_                 , 0x0010},
	{_007C_                 , 0x0038},
	{_0080_                 , 0x000C},
	{_0081_                 , 0xFFFF},
	{_0082_                 , 0x0000},
	{_0083_                 , 0x0000},
	{_0084_                 , 0x0000},
	{_0085_                 , 0x0004},
	{_0086_                 , 0x1032},
	{_0087_                 , 0x0000},
	{_0088_                 , 0x0001},
	{_0089_                 , 0x0000},
	{_0090_                 , 0x0012},
	{_0091_                 , 0x0013},
	{_0092_                 , 0x0010},
	{_0093_                 , 0x0011},
	{_0094_                 , 0x0016},
	{_0095_                 , 0x0017},
	{_0096_                 , 0x0014},
	{_0097_                 , 0x0015},
	{_0098_                 , 0x0002},
	{_0099_                 , 0x0003},
	{_009A_                 , 0x0000},
	{_009B_                 , 0x0001},
	{_009C_                 , 0x0006},
	{_009D_                 , 0x0007},
	{_009E_                 , 0x0004},
	{_009F_                 , 0x0005},
	{_00A0_                 , 0x0000},
	{_00A1_                 , 0x0000},
	{_00A2_                 , 0x0000},
	{_00A3_                 , 0x0000},
	{_00A4_                 , 0x0000},
	{_00A9_                 , 0x0000},
	{_00C0_                 , 0x1155},
	{_00C1_                 , 0xC5D5},
	{_00C2_                 , 0x93D5},
	{_00C3_                 , 0x01FC},
	{_00C4_                 , 0xF041},
	{_00C5_                 , 0x7FC4},
	{_00C6_                 , 0x0F54},
	{_00C7_                 , 0x0051},
	{_00C8_                 , 0x1541},
	{_00C9_                 , 0x0200},
	{_00CA_                 , 0x0100},
	{_00CB_                 , 0x0000},
	{_00CC_                 , 0x0000},
	{_00CD_                 , 0x0000},
	{_00CE_                 , 0x0000},
	{_00D1_                 , 0x041B},
	{_00D2_                 , 0x0000},
	{_00D3_                 , 0x041B},
	{_0100_                 , 0x0697},
	{_0101_                 , 0x0697},
	{_0102_                 , 0x0697},
	{_0103_                 , 0x01A5},
	{_0104_                 , 0x0000},
	{_0105_                 , 0x0000},
	{_0106_                 , 0x0000},
	{_0107_                 , 0x0000},
	{_0108_                 , 0x0008},
	{_0109_                 , 0x0009},
	{_010A_                 , 0x0696},
	{_010B_                 , 0x0008},
	{_010C_                 , 0x0000},
	{_010D_                 , 0x0008},
	{_010E_                 , 0x0009},
	{_010F_                 , 0x0696},
	{_0110_                 , 0x0000},
	{_0111_                 , 0x0003},
	{_0112_                 , 0x0009},
	{_0113_                 , 0x0696},
	{_0114_                 , 0x0330},
	{_0115_                 , 0x0333},
	{_0116_                 , 0x033F},
	{_0117_                 , 0x05EF},
	{_0118_                 , 0x0000},
	{_0119_                 , 0x0005},
	{_011A_                 , 0x001F},
	{_011B_                 , 0x0000},
	{_011C_                 , 0x0005},
	{_011D_                 , 0x0013},
	{_011E_                 , 0x001D},
	{_011F_                 , 0x001F},
	{_0120_                 , 0x0000},
	{_0121_                 , 0x0007},
	{_0122_                 , 0x0006},
	{_0123_                 , 0x0000},
	{_0124_                 , 0x033F},
	{_0125_                 , 0x05F9},
	{_0126_                 , 0x0002},
	{_0127_                 , 0x068F},
	{_0130_                 , 0x0000},
	{_0131_                 , 0x0000},
	{_0132_                 , 0x0000},
	{_0133_                 , 0x0000},
	{_0134_                 , 0x0000},
	{_0135_                 , 0x0605},
	{_0136_                 , 0x0341},
	{_0137_                 , 0x0000},
	{_0138_                 , 0x0000},
	{_0139_                 , 0x0000},
	{_013A_                 , 0x0000},
	{_0140_                 , 0x0009},
	{_0141_                 , 0x00D0},
	{_0142_                 , 0x0007},
	{_0143_                 , 0x030D},
	{_0144_                 , 0x0008},
	{_0145_                 , 0x00CF},
	{_0146_                 , 0x0006},
	{_0147_                 , 0x030C},
	{_0148_                 , 0x0000},
	{_0149_                 , 0x0000},
	{_014A_                 , 0x0008},
	{_014B_                 , 0x0006},
	{_014C_                 , 0x0000},
	{_014D_                 , 0x0002},
	{_014E_                 , 0x0002},
	{_014F_                 , 0x0007},
	{_0150_                 , 0x0008},
	{_0151_                 , 0x0006},
	{_0152_                 , 0x0009},
	{_0160_                 , 0x0001},
	{_0161_                 , 0x0004},
	{_0162_                 , 0x01A5},
	{_0163_                 , 0x0001},
	{_0164_                 , 0x003B},
	{_0165_                 , 0x004B},
	{_0166_                 , 0x0053},
	{_0167_                 , 0x0197},
	{_0168_                 , 0x01A4},
	{_0169_                 , 0x01A5},
	{_016A_                 , 0x0000},
	{_016B_                 , 0x0000},
	{_0170_                 , 0x0327},
	{_0171_                 , 0x0335},
	{_0172_                 , 0x0000},
	{_0173_                 , 0x0000},
	{_0174_                 , 0x0000},
	{_0175_                 , 0x0000},
	{_0176_                 , 0x0000},
	{_0177_                 , 0x0000},
	{_0178_                 , 0x0000},
	{_0179_                 , 0x0000},
	{_017C_                 , 0x0000},
	{_017D_                 , 0x0000},
	{_017E_                 , 0x0000},
	{_017F_                 , 0x0000},
	{_0190_                 , 0x0007},
	{_0191_                 , 0x0181},
	{_0192_                 , 0x0047},
	{_0193_                 , 0x0049},
	{_0194_                 , 0x004A},
	{_0195_                 , 0x004B},
	{_0196_                 , 0x004F},
	{_0197_                 , 0x0002},
/* Version 2.0.0.3 modification, 2011.08.11 */
	{_0198_                 , 0x0003},
/* ======================================== */
	{_0199_                 , 0x0002},
	{_019A_                 , 0x004F},
	{_019B_                 , 0x0002},
	{_01A0_                 , 0x0429},
	{_01A1_                 , 0x0000},
	{_01A2_                 , 0x0429},
	{_01A3_                 , 0x041A},
	{_01A4_                 , 0x0429},
	{_01A5_                 , 0x041A},
	{_01A6_                 , 0x0000},
	{_01A7_                 , 0x041B},
	{_01A8_                 , 0x0001},
	{_01A9_                 , 0x0009},
	{_01AA_                 , 0x0008},
	{_01AB_                 , 0x0009},
	{_01AC_                 , 0x0000},
	{_01AD_                 , 0x041C},
	{_01AE_                 , 0x0000},
	{_01AF_                 , 0x0000},
	{_01B0_                 , 0x0000},
	{_01B1_                 , 0x041C},
	{_01B2_                 , 0x0000},
	{_01B3_                 , 0x0000},
	{_01C0_                 , 0x0010},
	{_01C1_                 , 0x0417},
	{_01C2_                 , 0x0010},
	{_01C3_                 , 0x0417},
	{_01C4_                 , 0x0000},
	{_01C5_                 , 0x0000},
	{_01C6_                 , 0x0000},
	{_01C7_                 , 0x0000},
	{_01C8_                 , 0x000F},
	{_01D0_                 , 0x0000},
	{_01D1_                 , 0x0000},
	{_01D2_                 , 0x0000},
	{_01D3_                 , 0x0000},
	{_01E0_                 , 0x000A},
	{_01E1_                 , 0x0023},
	{_01E2_                 , 0x0023},
	{_01E3_                 , 0x0023},
	{_01E4_                 , 0x000E},
	{_01E5_                 , 0x000E},
	{_01E6_                 , 0x000E},
	{_01E7_                 , 0x0028},
	{_01E8_                 , 0x006B},
	{_01E9_                 , 0x006B},
	{_01EA_                 , 0x006B},
	{_01EB_                 , 0x0087},
	{_01EC_                 , 0x00CF},
	{_01ED_                 , 0x00D8},
	{_01EE_                 , 0x0106},
	{_01EF_                 , 0x0316},
	{_01F0_                 , 0x0003},
	{_01F1_                 , 0x001F},
	{_01F2_                 , 0x00B3},
	{_01F3_                 , 0x01BC},
	{_0000_                 , 0x0113},
	{_0003_                 , 0x8260}
};

/*==================================================================*/
static BYTE MN34030PL_BitReverse(BYTE byData)
{
	BYTE byOut;

	byOut = ((byData<<7)&0x80) | ((byData<<5)&0x40) | ((byData<<3)&0x20) | ((byData<<1)&0x10) |
			((byData>>1)&0x08) | ((byData>>3)&0x04) | ((byData>>5)&0x02) | ((byData>>7)&0x01);

	return byOut;
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.2 modification, 2011.06.22 */
static SOCKET MN34030PL_WriteReg(TMN34030PLInfo *ptInfo, EMN34030PLRegs eRegAddr, QWORD qwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[8];
/* ======================================== */
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);

	byOutRegAddr = MN34030PL_BitReverse((BYTE)(eRegAddr&0xFF));
	abyData[0] = MN34030PL_BitReverse((BYTE)((eRegAddr>>8)&0xFF));
	abyData[1] = MN34030PL_BitReverse(qwData&0xFF);
	abyData[2] = MN34030PL_BitReverse((qwData>>8)&0xFF);
	abyData[3] = MN34030PL_BitReverse((qwData>>16)&0xFF);
	abyData[4] = MN34030PL_BitReverse((qwData>>24)&0xFF);

/* Version 2.0.0.2 modification, 2011.06.22 */
	abyData[5] = MN34030PL_BitReverse((qwData>>32)&0xFF);
	abyData[6] = MN34030PL_BitReverse((qwData>>40)&0xFF);
/* ======================================== */

	abyData[dwLength] = (BYTE)bWaitCmpt;

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr/* 0 */, (DWORD)byOutRegAddr, (dwLength+1), abyData);
}

/* ============================================================================================== */
#if 0
SCODE MN34030PL_ReadReg(TMN34030PLInfo *ptInfo, EMN34030PLRegs eRegAddr, DWORD dwDataLen)
{
	BYTE abyData[6];
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);
	DWORD dwResult;

	byOutRegAddr = MN34030PL_BitReverse((BYTE)((eRegAddr|0x8000)&0xFF));
	abyData[0] = MN34030PL_BitReverse((unsigned char)(((eRegAddr|0x8000)>>8)&0xFF));
	abyData[dwLength] = 0;

	IICCtrl_ReadBuf(ptInfo->dwFuncUserData, 0, byOutRegAddr, (dwLength+1), abyData);
	printk("0x%02x 0x%02x 0x%02x 0x%02x\n", MN34030PL_BitReverse(abyData[0]&~0x1), MN34030PL_BitReverse(byOutRegAddr), 
			MN34030PL_BitReverse(abyData[2]), MN34030PL_BitReverse(abyData[1]));
	return 0;
}

#endif
/*-------------------------------------------------------------------------------------*/
static void MN34030PL_Reset(TMN34030PLInfo *ptInfo)
{
	DWORD dwIndex;
	/* Version 1.0.0.7 modification, 2010.08.30 */
	// release sensor reset, RSTN
	MN34030PL_WriteReg(ptInfo, 0x1001, 0x3000, 2, 0);
	mdelay(30);
	MN34030PL_WriteReg(ptInfo, 0x1001, 0x3100, 2, 0);
	udelay(60);
	/*==========================================*/

	for (dwIndex=0; dwIndex<4; dwIndex++)
	{
		/* Version 1.0.0.4 modification, 2010.08.02 */
		if (iEnSerialIF) 
		{
			MN34030PL_WriteReg(ptInfo, atMN34030PLSerialRegTbl[dwIndex].eRegAddr, atMN34030PLSerialRegTbl[dwIndex].dwData, 2, 0);
		}
		/*==========================================*/		
		else 
		{
			MN34030PL_WriteReg(ptInfo, atMN34030PLregTbl[dwIndex].eRegAddr, atMN34030PLregTbl[dwIndex].dwData, 2, 0);
		}
	}
	// sleep longer than 150us
	udelay(200);

	for (dwIndex=4; dwIndex<7; dwIndex++)
	{
		/* Version 1.0.0.4 modification, 2010.08.02 */
		if (iEnSerialIF)
		{
			MN34030PL_WriteReg(ptInfo, atMN34030PLSerialRegTbl[dwIndex].eRegAddr, atMN34030PLSerialRegTbl[dwIndex].dwData, 2, 0);
		}
		/*==========================================*/		
		else 
		{
			MN34030PL_WriteReg(ptInfo, atMN34030PLregTbl[dwIndex].eRegAddr, atMN34030PLregTbl[dwIndex].dwData, 2, 0);
		}
	}
	// sleep longer than 30ms
	mdelay(40); 
	/* Version 1.0.0.4 modification, 2010.08.02 */
	if (iEnSerialIF)
	{
		for (dwIndex=7; dwIndex<MN34030PL_SERIAL_TBL_SIZE; dwIndex++) 
		{
			MN34030PL_WriteReg(ptInfo, atMN34030PLSerialRegTbl[dwIndex].eRegAddr, atMN34030PLSerialRegTbl[dwIndex].dwData, 2, 0);
		}
/* Version 2.0.0.7 modification, 2012.10.05 */
/* Version 2.0.0.6 modification, 2012.09.04 */
/*==========================================*/
/*==========================================*/
		/* Version 1.0.0.7 modification, 2010.08.30 */
		// Clear DCM reset
		MN34030PL_WriteReg(ptInfo, 0x1001, 0x1100, 2, 0);
		// Wait DCM stable
		mdelay(2);
		// Clear SW reset and inverse pixel clock output
		/* Version 2.0.0.4 modification, 2011.08.17 */
		MN34030PL_WriteReg(ptInfo, 0x1001, 0x0980, 2, 0);
/* Version 2.0.0.7 modification, 2012.10.05 */
			MN34030PL_WriteReg(ptInfo, 0x100D, 0x0108, 2, 0); //Disable FPGA AWB
			MN34030PL_WriteReg(ptInfo, 0x100E, 0x0000, 2, 0); //Black clamp = CLAMP * 16.
/*==========================================*/
		/*==========================================*/
		/*==========================================*/
	}
	/*==========================================*/	
	else 
	{	
		for (dwIndex=7; dwIndex<MN34030PL_TBL_SIZE; dwIndex++)
		{
			MN34030PL_WriteReg(ptInfo, atMN34030PLregTbl[dwIndex].eRegAddr, atMN34030PLregTbl[dwIndex].dwData, 2, 0);
		}
	}
/* Version 2.0.0.5 modification, 2011.11.09 */
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
	{
		dwMN34030PLVsync = MN34030PL_VSYNC_1280x1024_60HZ;
		dwMN34030PLHsyncX10 = MN34030PL_HSYNC_1280x1024_60HZ_X10;
	}
	else
	{
		dwMN34030PLVsync = MN34030PL_VSYNC_1280x1024_50HZ;		
		dwMN34030PLHsyncX10 = MN34030PL_HSYNC_1280x1024_50HZ_X10;
	}
	MN34030PL_WriteReg(ptInfo, _01A0_, (dwMN34030PLVsync-1), 2, 0);
	MN34030PL_WriteReg(ptInfo, _02A0_, (dwMN34030PLVsync-1), 2, 0);
/* ======================================== */
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
/*===========================*/	

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
static void MN34030PL_SetShutter(DWORD dwShutter)
{
	DWORD dwExpFrame, dwExpLine, dwExpSub;
	DWORD dwShutterLongExp, dwShutterPos;
	QWORD qwData = 0;
	//DWORD alphax100=64,betax100=0, dwExpoLinex10;

/* Version 2.0.0.5 modification, 2011.11.09 */
	dwExpLine = (dwShutter*dwMN34030PLPixelClock*10) / dwMN34030PLHsyncX10;
	dwExpLine = (dwExpLine<=8) ? 8 : dwExpLine;
/* ======================================== */

	dwExpFrame = dwExpLine / dwMN34030PLVsync;
	dwExpLine -= (dwExpFrame*dwMN34030PLVsync);

/* Version 2.0.0.5 modification, 2011.11.09 */
	dwExpFrame = (dwExpLine==0) ? dwExpFrame : (dwExpFrame+1); 
/* ======================================== */
	dwExpSub = (dwExpLine<=0) ? 0 : (dwMN34030PLVsync-2-dwExpLine/*+ alphax100/100 + betax100/100*/);
/* Version 2.0.0.1 modification, 2011.01.26 */
	dwExpSub = (dwExpSub>(dwMN34030PLVsync-8)) ? (dwMN34030PLVsync-8) : dwExpSub;
/* ======================================== */

/* Version 2.0.0.5 modification, 2011.11.09 */
/* ======================================== */

	dwShutterLongExp = dwExpFrame - 1;
	dwShutterPos = dwExpSub;

/* Version 2.0.0.2 modification, 2011.06.22 */
	qwData =  (((QWORD)dwShutterLongExp)<<32) + (QWORD)dwShutterPos;
	MN34030PL_WriteReg(ptInfo, _00A1_, qwData, 6, 1);
/* ======================================== */
}

/*-------------------------------------------------------------------------------------*/
DWORD MN34030PL_RatioToDB(DWORD dwDevider)
{
	DWORD dwRatio, dwIdx, dwRet;

	dwRet = 0;
	dwRatio = (dwDevider*100) / 1000;
	while (dwRatio >= 200)
	{
		dwRatio /= 2;
		dwRet += 1;
	}

	for (dwIdx=0; dwIdx<32; dwIdx++)
	{
		if (dwRatio < adwLog10Tbl[dwIdx])
		{
			break;
		}
	}
	return ((dwRet*602) + (dwIdx*187500)/10000);
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static void MN34030PL_SetGain(DWORD dwGain)
{
/* Version 2.0.0.0 modification, 2011.01.21 */
	DWORD dwValue, dwDB, dwDigitalGain, dwColumnGain;

/* Version 2.0.0.6 modification, 2012.09.04 */
	if (dwGain > MN34030PL_MAX_GAIN)
	{
		dwGain = MN34030PL_MAX_GAIN;
/* ======================================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	dwDB = MN34030PL_RatioToDB(dwGain);
	if (dwDB < 600)
	{
		dwColumnGain = 0x80;
		dwDigitalGain = ((dwDB*100)/1875) * 2;
	}
	else if (dwDB < 1194)
	{
		dwColumnGain = 0x2080;
		dwDigitalGain = (((dwDB-594)*100)/1875) * 2;
	}
	else if (dwDB < 1763)
	{
		dwColumnGain = 0x6080;
		dwDigitalGain = (((dwDB-1182)*100)/1875) * 2;
	}
	else
	{
		dwColumnGain = 0xE080;
		dwDigitalGain = (((dwDB-1760)*100)/1875) * 2;
	}
	dwValue = dwColumnGain + (dwDigitalGain<<16);
/* ======================================== */
	MN34030PL_WriteReg(ptInfo, _0020_, dwValue, 4, 1);
}

/*==================================================================*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET  MN34030PL_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
/* ========================================= */
{
	SOCKET sckRet = 0;
/* Version 1.0.0.5 modification, 2010.08.02 */
	if (down_interruptible(&mn34030pl_mutex))
	{
		return -ERESTARTSYS;	
	}	
/* ========================================= */		

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		MN34030PL_Reset(ptInfo);
		break;		
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		MN34030PL_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		MN34030PL_SetGain(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[MN34030PL] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			sckRet = -EPERM;
			break;
		}
/* Version 2.0.0.5 modification, 2011.11.09 */
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwMN34030PLVsync = MN34030PL_VSYNC_1280x1024_60HZ;		
			dwMN34030PLHsyncX10 = MN34030PL_HSYNC_1280x1024_60HZ_X10;
		}
		else
		{
			dwMN34030PLVsync = MN34030PL_VSYNC_1280x1024_50HZ;		
			dwMN34030PLHsyncX10 = MN34030PL_HSYNC_1280x1024_50HZ_X10;
		}
		MN34030PL_WriteReg(ptInfo, _01A0_, (dwMN34030PLVsync-1), 2, 0);
		MN34030PL_WriteReg(ptInfo, _02A0_, (dwMN34030PLVsync-1), 2, 0);
/* ======================================== */	
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 1.0.0.6 modification, 2010.08.18 */
/* ======================================== */
/* ========================================= */		
		break;
		default:
		sckRet = -EPERM;
	}
/* Version 1.0.0.5 modification, 2010.08.02 */
	up(&mn34030pl_mutex);	
/* ========================================= */		

	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
void MN34030PL_Release(DWORD dwDevNum)
/* ========================================= */
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET MN34030PL_Open(TVideoSensorInitialParam* ptMN34030PLInitialParam, DWORD dwDevNum)
/* ========================================= */
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

/* Version 2.0.0.0 modification, 2011.01.21 */
	if (((ptMN34030PLInitialParam->dwVideoSensorVersion&0x00FF00FF)!=(MN34030PL_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptMN34030PLInitialParam->dwVideoSensorVersion&0x0000FF00)>(MN34030PL_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid MN34030PL device driver version %d.%d.%d.%d !!\n", 
				(int)(MN34030PL_INTERFACE_VERSION&0xFF), 
				(int)((MN34030PL_INTERFACE_VERSION>>8)&0xFF), 
				(int)((MN34030PL_INTERFACE_VERSION>>16)&0xFF), 
				(int)(MN34030PL_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

	if (!(ptInfo = (TMN34030PLInfo *)kmalloc(sizeof(TMN34030PLInfo), GFP_KERNEL)))
	{
		printk("[MN34030PL] : Allocate %d bytes memory fail\n", sizeof(TMN34030PLInfo));
		return -ENOMEM;
	}

	memset(ptInfo, 0, sizeof(TMN34030PLInfo));

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_WRITEBUF_FAST_NOACK;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[MN34030PL] : Initial IICCtrl object fail !!\n");
/* Version 1.0.0.1 modification, 2010.06.01 */
		MN34030PL_Release(dwDevNum);			
/* ========================================= */			
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = MN34030PL_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptMN34030PLInitialParam->dwVideoSensorFrequency;
	
/* Version 2.0.0.0 modification, 2011.01.21 */
	if (iEnSerialIF)
	{
		dwMN34030PLPixelClock = MN34030PL_PIXEL_CLOCK_60FPS_M;	
	}
	else
	{
		dwMN34030PLPixelClock = MN34030PL_PIXEL_CLOCK_30FPS_M;
	}
/* ======================================== */

	// Reset device
	MN34030PL_Reset(ptInfo);

	return 0;
}
	
/* Version 2.0.0.6 modification, 2012.09.04 */
/*-------------------------------------------------------------------------------------*/
DWORD MN34030PL_GetMaxGain(void)
{
	return (MN34030PL_MAX_GAIN);
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open =	MN34030PL_Open,
	.release = 	MN34030PL_Release,
	.ioctl = 	MN34030PL_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	.get_shutter_value = NULL,	
/* Version 1.0.0.2 modification, 2010.06.03 */	
	.get_remaining_line_num = NULL,
/* ========================================= */	
/* Version 1.0.0.3 modification, 2010.06.21 */
	.group_access = NULL,	
/* ========================== */	
/* Version 2.0.0.6 modification, 2012.09.04 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = MN34030PL_GetMaxGain,
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct MN34030PL_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t MN34030PL_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct MN34030PL_attribute *MN34030PL_attr = container_of(attr, struct MN34030PL_attribute, attr);
	int result = 0;
	
	if (MN34030PL_attr->show)
		result = MN34030PL_attr->show(kobj, buf);	
	return result;
}

ssize_t MN34030PL_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "MN34030PL");
	return retval;
}

ssize_t MN34030PL_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t MN34030PL_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1280");
	return retval;
}

ssize_t MN34030PL_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1024");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject MN34030PL_kobj;

struct MN34030PL_attribute MN34030PL_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34030PL_SensorName_attr_show,
};

struct MN34030PL_attribute MN34030PL_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34030PL_dwBufNum_attr_show,
};

struct MN34030PL_attribute MN34030PL_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34030PL_MaxFrameWidth_attr_show,
};

struct MN34030PL_attribute MN34030PL_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34030PL_MaxFrameHeight_attr_show,
};

struct sysfs_ops MN34030PL_sysfs_ops = {
	.show	= MN34030PL_default_attr_show,
};

struct kobj_type MN34030PL_ktype = {
	.sysfs_ops	= &MN34030PL_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET MN34030PL_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[MN34030PL] : kobj set name fail!\n");
		return -1;
	}
	/* Version 1.0.0.4 modification, 2010.08.02 */
	printk(KERN_INFO "MN34030PL_Init() iEnSerialIF=%d\n", iEnSerialIF);
	/*=========================================*/
	MN34030PL_kobj.ktype = &MN34030PL_ktype;	
	sckResult = kobject_init_and_add(&MN34030PL_kobj, &MN34030PL_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[MN34030PL] : Cannot register kobject [MN34030PL_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MN34030PL_kobj, &MN34030PL_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34030PL] : Cannot create MN34030PL_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&MN34030PL_kobj, &MN34030PL_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34030PL] : Cannot create MN34030PL_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MN34030PL_kobj, &MN34030PL_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34030PL] : Cannot create MN34030PL_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MN34030PL_kobj, &MN34030PL_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34030PL] : Cannot create MN34030PL_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
/* Version 1.0.0.5 modification, 2010.08.02 */
	sema_init(&mn34030pl_mutex, 1);
/* ========================================= */				
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void MN34030PL_Exit(void)
{
	sysfs_remove_file(&MN34030PL_kobj, &(MN34030PL_SensorName_attr.attr));
	sysfs_remove_file(&MN34030PL_kobj, &(MN34030PL_dwBufNum_attr.attr));	
/* Version 1.0.0.3 modification, 2010.06.21 */
	sysfs_remove_file(&MN34030PL_kobj, &(MN34030PL_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&MN34030PL_kobj, &(MN34030PL_MaxFrameHeight_attr.attr));	
/* ========================== */	

	kobject_put(&MN34030PL_kobj);	
	printk(KERN_DEBUG "[MN34030PL] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(MN34030PL_Init);
module_exit(MN34030PL_Exit);

