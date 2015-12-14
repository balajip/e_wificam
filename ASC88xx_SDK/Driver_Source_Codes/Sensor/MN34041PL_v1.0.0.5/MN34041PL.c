/*
 * MN34041PL
 * Driver for MN34041PL sensor.
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

#include "MN34041PL.h"
#include "MN34041PL_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR MN34041PL_ID[] = "$Version: "MN34041PL_ID_VERSION"  (MN34041PL DRIVER) $";

/*==================================================================*/
static TMN34041PLInfo *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore MN34041PL_mutex;

static DWORD dwMN34041PLHsync = MN34041PL_HSYNC_1920x1080_60FPS;
/* Version 1.0.0.2 modification, 2011.11.09 */
static DWORD dwMN34041PLVsync = MN34041PL_VSYNC_1920x1080_60FPS;
/* ========================== */
static DWORD dwMN34041PLPixelClock = MN34041PL_PIXEL_CLOCK_60FPS_M;

const DWORD adwLog10Tbl[32] = 
{
	102,104,107,109,111,114,116,119,121,124,127,130,132,135,138,141, 
	144,147,151,154,157,161,164,168,172,175,179,183,187,191,195,200 
};

static SOCKET iEnSerialIF = 1;
module_param(iEnSerialIF, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnSerialIF, "MN34041PL  Serial Mode On/Off");

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("MN34041PL driver");
MODULE_LICENSE("GPL");

const TMN34041PLRegAddrData atMN34041PLSerialRegTbl[MN34041PL_SERIAL_TBL_SIZE] =
{
	{_0001_,	0x0036},
	{_0002_,	0x0002},
	{_0004_,	0x01C2},
	{_0000_,	0x0003},
	{_0003_,	0x0209},
	{_0006_,	0x442A},
	{_0005_,	0x0000},
		
	{_0000_,	0x0013},
	{_0036_,	0x0021},
	{_0037_,	0x0300},
		
	{_0020_,	0x0080},
	{_0021_,	0x0080},
	{_0022_,	0x0000},
	{_0023_,	0x0000},
	{_0024_,	0x0010},
	{_0025_,	0x0011},
	{_0026_,	0x0012},
	{_0027_,	0x0013},
	{_0028_,	0x0000},
	{_0030_,	0x0424},
	{_0031_,	0x110A},
	{_0032_,	0x7450},
	{_0033_,	0x0000},
	{_0034_,	0x0000},
	{_0035_,	0x0281},
	
	{_0038_,	0x0001},
	{_0039_,	0x0210},
	{_003A_,	0x0333},
	{_003B_,	0x0111},
	{_003F_,	0x0000},
	{_0040_,	0x0004},
	{_0041_,	0x0200},
/* Version 1.0.0.3 modification, 2012.09.04 */
	{_0042_,	0x0000},
/* ========================== */
/* Version 1.0.0.1 modification, 2010.08.17 */
/* Version 1.0.0.5 modification, 2013.05.24 */
	{_0043_,	0x0070},
/* ========================== */
/* ========================== */
	{_0044_,	0x0000},
	{_0045_,	0x0000},
	{_0046_,	0x0000},
	{_0047_,	0x0010},
	{_0048_,	0x0000},
	{_0049_,	0x0002},
	{_004A_,	0x0FFE},
	{_004B_,	0x0004},
	{_0056_,	0x0000},
	{_0057_,	0x1FFF},
	{_0058_,	0x2000},
	{_0059_,	0x0000},
	{_005A_,	0x0011},
	{_0070_,	0x2084},
	{_0071_,	0xFFFF},
	{_0072_,	0x0000},
	{_0073_,	0x0000},
	{_0074_,	0x0000},
	{_0075_,	0x0004},
	{_0076_,	0x0230},
	{_0077_,	0x0541},
	{_0078_,	0x0001},
	{_0079_,	0x0011},
	{_0080_,	0x0002},
	{_0081_,	0x0003},
	{_0082_,	0x0000},
	{_0083_,	0x0001},
	{_0084_,	0x0012},
	{_0085_,	0x0013},
	{_0086_,	0x0010},
	{_0087_,	0x0011},
	{_0088_,	0x000A},
	{_0089_,	0x000B},
	{_008A_,	0x0008},
	{_008B_,	0x0009},
	{_008C_,	0x0006},
	{_008D_,	0x0007},
	{_008E_,	0x0004},
	{_008F_,	0x0005},
	{_0090_,	0x0016},
	{_0091_,	0x0017},
	{_0092_,	0x0014},
	{_0093_,	0x0015},
	{_0094_,	0x001A},
	{_0095_,	0x001B},
	{_0096_,	0x0018},
	{_0097_,	0x0019},
	{_00A0_,	0x3000},
	{_00A1_,	0x0000},
	{_00A2_,	0x0002},
	{_00A3_,	0x0000},
	{_00A4_,	0x0000},
	{_00A5_,	0x0000},
	{_00A6_,	0x0000},
	{_00A7_,	0x0000},
	{_00A8_,	0x000F},
	{_00A9_,	0x0022},
	{_00C0_,	0x5540},
	{_00C1_,	0x5FD5},
	{_00C2_,	0xF757},
	{_00C3_,	0xDF5F},
	{_00C4_,	0x208A},
	{_00C5_,	0x0071},
	{_00C6_,	0x0557},
	{_00C7_,	0x0000},
	{_00CA_,	0x0080},
	{_00CB_,	0x0000},
	{_00CC_,	0x0000},
	{_00CD_,	0x0000},
	{_00CE_,	0x0000},
	{_0100_,	0x03A8},
	{_0101_,	0x03A8},
	{_0102_,	0x02C0},
	{_0103_,	0x037A},
	{_0104_,	0x002B},
	{_0105_,	0x00DE},
	{_0106_,	0x00FA},
	{_0170_,	0x0002},
	{_0171_,	0x000D},
	{_0172_,	0x0007},
	{_0173_,	0x0004},
	{_0174_,	0x002A},
	{_0175_,	0x0062},
	{_0176_,	0x0079},
	{_0177_,	0x0326},
	{_0178_,	0x0003},
	{_0179_,	0x0033},
	{_017A_,	0x0360},
	{_017B_,	0x0002},
	{_017C_,	0x000D},
	{_0190_,	0x0000},
	{_0191_,	0x0000},
	{_0192_,	0x0000},
	{_0193_,	0x0000},
	{_0194_,	0x0000},
	{_0195_,	0x0000},
	{_0196_,	0x0000},
	{_0197_,	0x01BA},
	{_0198_,	0xB060},
	{_0199_,	0x7C0A},
	{_019A_,	0x0000},
	{_019B_,	0x0313},
	{_019C_,	0x0B08},
	{_019D_,	0x3906},
	{_019E_,	0x0000},
	{_01A0_,	0x0464},
	{_01A1_,	0x0000},
	{_01A2_,	0x0000},
	{_01A3_,	0x0464},
	{_01A4_,	0x0000},
	{_01A5_,	0x0453},
	{_01A6_,	0x0000},
	{_01A7_,	0x0464},
	{_01A8_,	0x0000},
	{_01A9_,	0x0454},
	{_01AA_,	0x0000},
	{_01AB_,	0x0000},
	{_01AC_,	0x0000},
	{_01AD_,	0x0454},
	{_01AE_,	0x0000},
	{_01AF_,	0x0000},
	{_01B0_,	0x0000},
	{_01B1_,	0x0454},
	{_01B2_,	0x0000},
	{_01B3_,	0x0000},
	{_01B4_,	0x0000},
	{_01B5_,	0x0454},
	{_01B6_,	0x0000},
	{_01B7_,	0x0000},
	{_01B8_,	0x0000},
	{_01B9_,	0x0453},
	{_01BA_,	0x0000},
	{_01BB_,	0x0000},
	{_01BC_,	0x0000},
	{_01BD_,	0x0453},
	{_01BE_,	0x0000},
	{_01C4_,	0x0000},
	{_01C5_,	0x0000},
	{_01C6_,	0x0011},
	{_0200_,	0x03A8},
	{_0201_,	0x03A8},
	{_0202_,	0x02C0},
	{_0203_,	0x037A},
	{_0204_,	0x002B},
	{_0205_,	0x00DE},
	{_0206_,	0x00FA},
	{_0270_,	0x0002},
	{_0271_,	0x000D},
	{_0272_,	0x0007},
	{_0273_,	0x0004},
	{_0274_,	0x002A},
	{_0275_,	0x0062},
	{_0276_,	0x0079},
	{_0277_,	0x0326},
	{_0278_,	0x0003},
	{_0279_,	0x0033},
	{_027A_,	0x0360},
	{_027B_,	0x0002},
	{_027C_,	0x000D},
	{_0290_,	0x0000},
	{_0291_,	0x0000},
	{_0292_,	0x0000},
	{_0293_,	0x0000},
	{_0294_,	0x0000},
	{_0295_,	0x0000},
	{_0296_,	0x0000},
	{_0297_,	0x01BA},
	{_0298_,	0xB060},
	{_0299_,	0x7C0A},
	{_029A_,	0x0000},
	{_029B_,	0x0313},
	{_029C_,	0x0B08},
	{_029D_,	0x3906},
	{_029E_,	0x0000},
	{_02A0_,	0x0464},
	{_02A1_,	0x0000},
	{_02A2_,	0x0000},
	{_02A3_,	0x0464},
	{_02A4_,	0x0000},
	{_02A5_,	0x0453},
	{_02A6_,	0x0000},
	{_02A7_,	0x0464},
	{_02A8_,	0x0000},
	{_02A9_,	0x0454},
	{_02AA_,	0x0000},
	{_02AB_,	0x0000},
	{_02AC_,	0x0000},
	{_02AD_,	0x0454},
	{_02AE_,	0x0000},
	{_02AF_,	0x0000},
	{_02B0_,	0x0000},
	{_02B1_,	0x0454},
	{_02B2_,	0x0000},
	{_02B3_,	0x0000},
	{_02B4_,	0x0000},
	{_02B5_,	0x0454},
	{_02B6_,	0x0000},
	{_02B7_,	0x0000},
	{_02B8_,	0x0000},
	{_02B9_,	0x0453},
	{_02BA_,	0x0000},
	{_02BB_,	0x0000},
	{_02BC_,	0x0000},
	{_02BD_,	0x0453},
	{_02BE_,	0x0000},
	{_02C4_,	0x0000},
	{_02C5_,	0x0000},
	{_02C6_,	0x0011},
	{_0108_,	0x0000},
	{_0109_,	0x000F},
	{_010A_,	0x0009},
	{_010B_,	0x0000},
	{_010C_,	0x0016},
	{_010D_,	0x0000},
	{_010E_,	0x000F},
	{_010F_,	0x0000},
	{_0110_,	0x0009},
	{_0111_,	0x0000},
	{_0112_,	0x0016},
	{_0113_,	0x0003},
	{_0114_,	0x000A},
	{_0115_,	0x0000},
	{_0116_,	0x0009},
	{_0117_,	0x0000},
	{_0118_,	0x0016},
	{_0119_,	0x0018},
	{_011A_,	0x0017},
	{_011B_,	0x0000},
	{_011C_,	0x0002},
	{_011D_,	0x0009},
	{_011E_,	0x0012},
	{_011F_,	0x0001},
	{_0120_,	0x003A},
	{_0121_,	0x0000},
	{_0122_,	0x0000},
	{_0123_,	0x0000},
	{_0124_,	0x0011},
	{_0125_,	0x0000},
	{_0126_,	0x0003},
	{_0127_,	0x0003},
	{_0128_,	0x0000},
	{_0129_,	0x0010},
	{_012A_,	0x0000},
	{_012B_,	0x0003},
	{_012C_,	0x0000},
	{_012D_,	0x0011},
	{_012E_,	0x0000},
	{_012F_,	0x0009},
	{_0130_,	0x0009},
	{_0131_,	0x0012},
	{_0132_,	0x0000},
	{_0133_,	0x0000},
	{_0134_,	0x0009},
	{_0135_,	0x0009},
	{_0136_,	0x0012},
	{_0137_,	0x0006},
	{_0138_,	0x0000},
	{_0139_,	0x0010},
	{_013A_,	0x0000},
	{_0140_,	0x0020},
	{_0141_,	0x0036},
	{_0142_,	0x0000},
	{_0143_,	0x0001},
	{_0144_,	0x0003},
	{_0145_,	0x0000},
	{_0146_,	0x0000},
	{_0150_,	0x0011},
	{_0151_,	0x0001},
	{_0152_,	0x0001},
	{_0153_,	0x0001},
	{_0154_,	0x0010},
	{_0155_,	0x0000},
	{_0156_,	0x0003},
	{_0157_,	0x0000},
	{_0158_,	0x0013},
	{_0159_,	0x0000},
	{_015A_,	0x0000},
	{_015B_,	0x0008},
	{_015C_,	0x0000},
	{_015D_,	0x0004},
	{_015E_,	0x0005},
	{_015F_,	0x000A},
	{_0160_,	0x0006},
	{_0161_,	0x000F},
	{_0162_,	0x000D},
	{_0163_,	0x0004},
	{_0164_,	0x0002},
	{_0165_,	0x0002},
	{_0166_,	0x0000},
	{_0167_,	0x0010},
	{_0168_,	0x0009},
	{_0169_,	0x0003},
	{_016A_,	0x0000},
	{_016B_,	0x0000},
	{_0180_,	0x0017},
	{_0181_,	0x00C5},
	{_0182_,	0x0000},
	{_0183_,	0x0000},
	{_0184_,	0x00FA},
	{_0185_,	0x00A5},
	{_0186_,	0x01EF},
	{_0187_,	0x03D9},
	{_0188_,	0x01FB},
	{_0189_,	0x035F},
	{_018A_,	0x0000},
	{_018B_,	0x0000},
	{_018C_,	0x0000},
	{_018D_,	0x0000},
	{_01D0_,	0x0000},
	{_01D1_,	0x0000},
	{_01D2_,	0x0000},
	{_01D3_,	0x0000},
	{_01D4_,	0x0000},
	{_01D5_,	0x0000},
	{_01D6_,	0x0000},
	{_01D7_,	0x0000},
	{_01D8_,	0x0006},
	{_01D9_,	0x0005},
	{_01DA_,	0x0001},
	{_01DB_,	0x0006},
	{_01DC_,	0x0001},
	{_01DD_,	0x0007},
	{_01DE_,	0x0001},
	{_01DF_,	0x0002},
	{_01E0_,	0x0001},
	{_01E1_,	0x0001},
	{_01E2_,	0x00C9},
	{_01E3_,	0x8000},
	{_01E4_,	0x003E},
	{_01E5_,	0x0015},
	{_01E6_,	0x003E},
	{_01E7_,	0x00C8},
	{_01E8_,	0x0043},
	{_01E9_,	0x00A9},
	{_01EA_,	0x00A9},
	{_01EB_,	0x00A9},
	{_01EC_,	0x00FB},
	{_01ED_,	0x00B0},
	{_01EE_,	0x00B9},
	{_01EF_,	0x01BB},
	{_01F0_,	0x02EC},
	{_01F1_,	0x02EC},
	{_01F2_,	0x02EC},
	{_01F3_,	0x01BD},
	{_01F4_,	0x034A},
	{_01F5_,	0x03D8},
	{_01F6_,	0x03FC},
	{_0000_,	0x0113},
	//{_0042_,	0x0100},sensor digital ouotput offset. [11-0] 0~4095 LSB
	//{_0043_,	0x0070},control OB accumulation. [0]0/1=On/Off. [1]setting timming 0/1=V/H. [6-4]7:0=Slow:early
	//{_004A_,	0x0FFE},set clipping value of Max outpot value. [11-0]0x001~0xFFE
	//{_004B_,	0x0004},set clipping value of Min outpot value. [11-0]0x001~0xFFE(Recommend 0x004)
	//{_00A0_,	0x3000},[15]Flip mode 1/0=Off/On
	//{_01A0_,	0x0464},[15-0]Vertical line number in mode1 = (PCLK/(2400*FPS))-1
	//{_01A1_,	0x0000},[0]Vertical line number in mode1 = (PCLK/(2400*FPS))-1
	//{_01A3_,	0x0464},[15-0]Vertical timming setting of pixel drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_01A4_,	0x0000},[0]Vertical timming setting of pixel drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_01A7_,	0x0464},[15-0]Vertical timming setting of ADC drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_01A8_,	0x0000},[0]Vertical timming setting of ADC drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_02A0_,	0x0464},[15-0]Vertical line number in mode2 = (PCLK/(2400*FPS))-1
	//{_02A1_,	0x0000},[0]Vertical line number in mode2 = (PCLK/(2400*FPS))-1
	//{_02A3_,	0x0464},[15-0]Vertical timming setting of pixel drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
	//{_02A4_,	0x0000},[0]Vertical timming setting of pixel drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
	//{_02A7_,	0x0464},[15-0]Vertical timming setting of ADC drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
	//{_02A8_,	0x0000},[0]Vertical timming setting of ADC drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
};
/*
const TMN34041PLRegAddrData atMN34041PLregTbl[MN34041PL_TBL_SIZE] = {
};
*/

/*==================================================================*/
static BYTE MN34041PL_BitReverse(BYTE byData)
{
	BYTE byOut;

	byOut = ((byData<<7)&0x80) | ((byData<<5)&0x40) | ((byData<<3)&0x20) | ((byData<<1)&0x10) |
			((byData>>1)&0x08) | ((byData>>3)&0x04) | ((byData>>5)&0x02) | ((byData>>7)&0x01);

	return byOut;
}

/*-------------------------------------------------------------------------------------*/
static SOCKET MN34041PL_WriteBuf(TMN34041PLInfo *ptInfo, EMN34041PLRegs eRegAddr, DWORD dwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[6];
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);

	byOutRegAddr = MN34041PL_BitReverse((BYTE)(eRegAddr&0xFF));
	abyData[0] = MN34041PL_BitReverse((BYTE)((eRegAddr>>8)&0xFF));
	abyData[1] = MN34041PL_BitReverse(dwData&0xFF);
	abyData[2] = MN34041PL_BitReverse((dwData>>8)&0xFF);
	abyData[3] = MN34041PL_BitReverse((dwData>>16)&0xFF);
	abyData[4] = MN34041PL_BitReverse((dwData>>24)&0xFF);
	abyData[dwLength] = (BYTE)bWaitCmpt;

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, (DWORD)byOutRegAddr, (dwLength+1), abyData);
}
#if 0
/*-------------------------------------------------------------------------------------*/
static SOCKET MN34041PL_ReadBuf(TMN34041PLInfo *ptInfo, EMN34041PLRegs eRegAddr, DWORD dwDataLen, DWORD *dwReadData)
{
	BYTE abyData[6];
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);

	byOutRegAddr = MN34041PL_BitReverse((BYTE)((eRegAddr|0x8000)&0xFF));
	abyData[0] = MN34041PL_BitReverse((BYTE)(((eRegAddr|0x8000)>>8)&0xFF));
	abyData[dwLength] = 0;

	ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, 0, byOutRegAddr, (dwLength+1), abyData);

	printk("0x%02x 0x%02x 0x%02x 0x%02x\n", MN34041PL_BitReverse(abyData[0]&~0x1), MN34041PL_BitReverse(byOutRegAddr), 
			MN34041PL_BitReverse(abyData[2]), MN34041PL_BitReverse(abyData[1]));
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static SOCKET MN34041PL_CompBuf(TMN34041PLInfo *ptInfo, EMN34041PLRegs eRegAddr, DWORD byMask, DWORD byData)
{
	DWORD dwReadData = 0;

	MN34041PL_ReadBuf(ptInfo, eRegAddr, 2, &dwReadData);
	return ((dwReadData&byMask) == byData);
}
#endif
/*-------------------------------------------------------------------------------------*/
static void MN34041PL_Reset(TMN34041PLInfo *ptInfo)
{
	DWORD dwIndex;

	// release sensor reset, RSTN
	MN34041PL_WriteBuf(ptInfo, 0x1001, 0x3100, 2, 0);
	udelay(60);

	for (dwIndex=0; dwIndex<7; dwIndex++)
	{
		if (iEnSerialIF) 
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLSerialRegTbl[dwIndex].eRegAddr, atMN34041PLSerialRegTbl[dwIndex].dwData, 2, 0);
		}
/*
		else 
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLregTbl[dwIndex].eRegAddr, atMN34041PLregTbl[dwIndex].dwData, 2, 0);
		}
*/
	}
	// sleep longer than 150us
	udelay(200);

	for (dwIndex=7; dwIndex<10; dwIndex++)
	{
		if (iEnSerialIF)
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLSerialRegTbl[dwIndex].eRegAddr, atMN34041PLSerialRegTbl[dwIndex].dwData, 2, 0);
		}
/*
		else 
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLregTbl[dwIndex].eRegAddr, atMN34041PLregTbl[dwIndex].dwData, 2, 0);
		}
*/
	}
	// sleep longer than 30ms
	mdelay(40); 
	if (iEnSerialIF)
	{
		for (dwIndex=11; dwIndex<MN34041PL_SERIAL_TBL_SIZE; dwIndex++) 
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLSerialRegTbl[dwIndex].eRegAddr, atMN34041PLSerialRegTbl[dwIndex].dwData, 2, 0);
		}
		// Clear DCM reset
		MN34041PL_WriteBuf(ptInfo, 0x1001, 0x1100, 2, 0);
		// Wait DCM stable
		mdelay(2);
		// Clear SW reset and inverse pixel clock output
/* Version 1.0.0.1 modification, 2010.08.17 */
		MN34041PL_WriteBuf(ptInfo, 0x1001, 0x0980, 2, 0);
/* ========================== */
	}
/*
	else 
	{	
		for (dwIndex=11; dwIndex<MN34041PL_TBL_SIZE; dwIndex++)
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLregTbl[dwIndex].eRegAddr, atMN34041PLregTbl[dwIndex].dwData, 2, 0);
		}
	}
*/
/* Version 1.0.0.2 modification, 2011.11.09 */
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
	{
		dwMN34041PLHsync = MN34041PL_HSYNC_1920x1080_60FPS;
		dwMN34041PLVsync = MN34041PL_VSYNC_1920x1080_60FPS;		
	}
	else
	{
		dwMN34041PLHsync = MN34041PL_HSYNC_1920x1080_50FPS;
		dwMN34041PLVsync = MN34041PL_VSYNC_1920x1080_50FPS;		
	}

	MN34041PL_WriteBuf(ptInfo, _01A0_, (dwMN34041PLVsync-1), 2, 0);
	MN34041PL_WriteBuf(ptInfo, _01A1_, 0x0000, 2, 0);
	MN34041PL_WriteBuf(ptInfo, _01A3_, (dwMN34041PLVsync-1), 2, 0);
	MN34041PL_WriteBuf(ptInfo, _01A4_, 0x0000, 2, 0);
	MN34041PL_WriteBuf(ptInfo, _01A7_, (dwMN34041PLVsync-1), 2, 0);
	MN34041PL_WriteBuf(ptInfo, _01A8_, 0x0000, 2, 0);
	MN34041PL_WriteBuf(ptInfo, _02A0_, (dwMN34041PLVsync-1), 2, 0);
	MN34041PL_WriteBuf(ptInfo, _02A1_, 0x0000, 2, 0);
	MN34041PL_WriteBuf(ptInfo, _02A3_, (dwMN34041PLVsync-1), 2, 0);
	MN34041PL_WriteBuf(ptInfo, _02A4_, 0x0000, 2, 0);
	MN34041PL_WriteBuf(ptInfo, _02A7_, (dwMN34041PLVsync-1), 2, 0);
	MN34041PL_WriteBuf(ptInfo, _02A8_, 0x0000, 2, 0);	
/* ========================== */
}

/*-------------------------------------------------------------------------------------*/
static void MN34041PL_SetShutter(DWORD dwShutter)
{
	DWORD dwExpFrame, dwExpLine, dwExpSub;
	DWORD dwShutterLongExp, dwShutterPos;
	DWORD alphax10=6;

	dwExpLine = ((dwShutter*dwMN34041PLPixelClock*10)/dwMN34041PLHsync+5)/10;
/* Version 1.0.0.2 modification, 2011.11.09 */
	dwExpLine = (dwExpLine<=8) ? 8 : dwExpLine;
/* ========================== */
	dwExpFrame = dwExpLine / dwMN34041PLVsync;
	dwExpLine -= (dwExpFrame*dwMN34041PLVsync);

/* Version 1.0.0.2 modification, 2011.11.09 */
	dwExpFrame = (dwExpLine==0) ? dwExpFrame : (dwExpFrame+1); 
/* ========================== */
	dwExpSub = (dwExpLine<=0) ? 0 : (((dwMN34041PLVsync-3-dwExpLine)*10 + alphax10)/10);
	dwExpSub = (dwExpSub>=(dwMN34041PLVsync-3)) ? (dwMN34041PLVsync-3) : dwExpSub;

/* Version 1.0.0.2 modification, 2011.11.09 */
/* ========================== */

	dwShutterLongExp = dwExpFrame - 1;
	dwShutterPos = dwExpSub;

	MN34041PL_WriteBuf(ptInfo, _00A1_, dwShutterPos, 4, 1);
	MN34041PL_WriteBuf(ptInfo, _00A5_, dwShutterLongExp, 2, 0);
}

/*-------------------------------------------------------------------------------------*/
DWORD MN34041PL_RatioToDB(DWORD dwDevider)
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
	return ((dwRet*6021) + (dwIdx*9375*2)/100);
}

/*-------------------------------------------------------------------------------------*/
static void MN34041PL_SetGain(DWORD dwGain)
{
	DWORD dwValue, dwDBX1000, dwAnalogGain, dwDigitalGain, dwColumnGain;

/* Version 1.0.0.3 modification, 2012.09.04 */
	if (dwGain > MN34041PL_MAX_GAIN)
	{
		dwGain = MN34041PL_MAX_GAIN;
/* ========================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	dwDBX1000 = MN34041PL_RatioToDB(dwGain);

	if (dwDBX1000 <= 5990)
	{
		dwColumnGain = 0x0000;
		dwAnalogGain = 0x0080;
		dwDigitalGain = ((dwDBX1000*100)/9375)+0x0080;
	}
	else if (dwDBX1000 <= 11990)
	{
		dwColumnGain = 0x8000;
		dwAnalogGain = 0x0080;
		dwDigitalGain = (((dwDBX1000-6000)*100)/9375)+0x0080;
	}
	else if (dwDBX1000 <= 17990)
	{
		dwColumnGain = 0xC000;
		dwAnalogGain = 0x0080;
		dwDigitalGain = (((dwDBX1000-12000)*100)/9375)+0x0080;
	}
	else // 18DB ~ 30DB
	{
		dwColumnGain = 0xC000;
		dwAnalogGain = 0x00C0;
		dwDigitalGain = (((dwDBX1000-18000)*100)/9375)+0x0080;
	}
	dwValue = dwColumnGain + dwAnalogGain + (dwDigitalGain<<16);
	MN34041PL_WriteBuf(ptInfo, _0020_, dwValue, 4, 1);	
}

/*-------------------------------------------------------------------------------------*/
SOCKET  MN34041PL_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET sckRet = 0;

	if (down_interruptible(&MN34041PL_mutex))
	{
		return -ERESTARTSYS;	
	}	

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		MN34041PL_Reset(ptInfo);
		break;		
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		MN34041PL_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		MN34041PL_SetGain(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[MN34041PL] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			sckRet = -EPERM;
			break;
		}
/* Version 1.0.0.2 modification, 2011.11.09 */
		if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwMN34041PLHsync = MN34041PL_HSYNC_1920x1080_60FPS;
			dwMN34041PLVsync = MN34041PL_VSYNC_1920x1080_60FPS;		
		}
		else
		{
			dwMN34041PLHsync = MN34041PL_HSYNC_1920x1080_50FPS;
			dwMN34041PLVsync = MN34041PL_VSYNC_1920x1080_50FPS;		
		}

		MN34041PL_WriteBuf(ptInfo, _01A0_, (dwMN34041PLVsync-1), 2, 0);
		MN34041PL_WriteBuf(ptInfo, _01A1_, 0x0000, 2, 0);
		MN34041PL_WriteBuf(ptInfo, _01A3_, (dwMN34041PLVsync-1), 2, 0);
		MN34041PL_WriteBuf(ptInfo, _01A4_, 0x0000, 2, 0);
		MN34041PL_WriteBuf(ptInfo, _01A7_, (dwMN34041PLVsync-1), 2, 0);
		MN34041PL_WriteBuf(ptInfo, _01A8_, 0x0000, 2, 0);
		MN34041PL_WriteBuf(ptInfo, _02A0_, (dwMN34041PLVsync-1), 2, 0);
		MN34041PL_WriteBuf(ptInfo, _02A1_, 0x0000, 2, 0);
		MN34041PL_WriteBuf(ptInfo, _02A3_, (dwMN34041PLVsync-1), 2, 0);
		MN34041PL_WriteBuf(ptInfo, _02A4_, 0x0000, 2, 0);
		MN34041PL_WriteBuf(ptInfo, _02A7_, (dwMN34041PLVsync-1), 2, 0);
		MN34041PL_WriteBuf(ptInfo, _02A8_, 0x0000, 2, 0);	
/* ========================== */
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
		break;
		default:
		sckRet = -EPERM;
	}
	up(&MN34041PL_mutex);	

	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
void MN34041PL_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET MN34041PL_Open(TVideoSensorInitialParam* ptMN34041PLInitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptMN34041PLInitialParam->dwVideoSensorVersion&0x00FF00FF)!=(MN34041PL_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptMN34041PLInitialParam->dwVideoSensorVersion&0x0000FF00)>(MN34041PL_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid MN34041PL device driver version %d.%d.%d.%d !!\n", 
				(int)(MN34041PL_INTERFACE_VERSION&0xFF), 
				(int)((MN34041PL_INTERFACE_VERSION>>8)&0xFF), 
				(int)((MN34041PL_INTERFACE_VERSION>>16)&0xFF), 
				(int)(MN34041PL_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo = (TMN34041PLInfo *)kmalloc(sizeof(TMN34041PLInfo), GFP_KERNEL)))
	{
		printk("[MN34041PL] : Allocate %d bytes memory fail\n", sizeof(TMN34041PLInfo));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_WRITEBUF_FAST_NOACK;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[MN34041PL] : Initial IICCtrl object fail !!\n");
		MN34041PL_Release(dwDevNum);			
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = MN34041PL_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->dwFreq = ptMN34041PLInitialParam->dwVideoSensorFrequency;

	// Detect device
/*
	if (!MN34041PL_CompBuf(ptInfo, _0036_, 0xF000, MN34041PL_VER))
	{
		printk("[MN34041PL] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
		MN34041PL_Release(dwDevNum);	
		return -ENODEV;
	}
	else
	{
		printk("[MN34041PL] : Detect device success %d \n", iEnSerialIF);
	}
*/
	if (iEnSerialIF)
	{
		dwMN34041PLPixelClock = MN34041PL_PIXEL_CLOCK_60FPS_M;	
	}
	else
	{
		dwMN34041PLPixelClock = MN34041PL_PIXEL_CLOCK_30FPS_M;
	}

	// Reset device
	MN34041PL_Reset(ptInfo);
	
	return 0;
}

/* Version 1.0.0.3 modification, 2012.09.04 */
/*-------------------------------------------------------------------------------------*/
DWORD MN34041PL_GetMaxGain(void)
{
	return (MN34041PL_MAX_GAIN);
}
/* ========================== */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open =	MN34041PL_Open,
	.release = 	MN34041PL_Release,
	.ioctl = 	MN34041PL_Ioctl,
	.get_shutter_value = NULL,	
	.get_remaining_line_num = NULL,
	.group_access = NULL,	
/* Version 1.0.0.3 modification, 2012.09.04 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = MN34041PL_GetMaxGain,
/* ========================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*-------------------------------------------------------------------------------------*/
struct MN34041PL_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t MN34041PL_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct MN34041PL_attribute *MN34041PL_attr = container_of(attr, struct MN34041PL_attribute, attr);
	int result = 0;
	
	if (MN34041PL_attr->show)
		result = MN34041PL_attr->show(kobj, buf);	
	return result;
}

ssize_t MN34041PL_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "MN34041PL");
	return retval;
}

ssize_t MN34041PL_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t MN34041PL_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}

ssize_t MN34041PL_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1080");
	return retval;
}

/*-------------------------------------------------------------------------------------*/
struct kobject MN34041PL_kobj;

struct MN34041PL_attribute MN34041PL_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34041PL_SensorName_attr_show,
};

struct MN34041PL_attribute MN34041PL_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34041PL_dwBufNum_attr_show,
};

struct MN34041PL_attribute MN34041PL_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34041PL_MaxFrameWidth_attr_show,
};

struct MN34041PL_attribute MN34041PL_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MN34041PL_MaxFrameHeight_attr_show,
};

struct sysfs_ops MN34041PL_sysfs_ops = {
	.show	= MN34041PL_default_attr_show,
};

struct kobj_type MN34041PL_ktype = {
	.sysfs_ops	= &MN34041PL_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET MN34041PL_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[MN34041PL] : kobj set name fail!\n");
		return -1;
	}
	printk(KERN_INFO "MN34041PL_Init() iEnSerialIF=%d\n", iEnSerialIF);
	MN34041PL_kobj.ktype = &MN34041PL_ktype;	
	sckResult = kobject_init_and_add(&MN34041PL_kobj, &MN34041PL_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[MN34041PL] : Cannot register kobject [MN34041PL_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MN34041PL_kobj, &MN34041PL_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34041PL] : Cannot create MN34041PL_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&MN34041PL_kobj, &MN34041PL_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34041PL] : Cannot create MN34041PL_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MN34041PL_kobj, &MN34041PL_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34041PL] : Cannot create MN34041PL_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MN34041PL_kobj, &MN34041PL_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MN34041PL] : Cannot create MN34041PL_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&MN34041PL_mutex, 1);
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void MN34041PL_Exit(void)
{
	sysfs_remove_file(&MN34041PL_kobj, &(MN34041PL_SensorName_attr.attr));
	sysfs_remove_file(&MN34041PL_kobj, &(MN34041PL_dwBufNum_attr.attr));	
	sysfs_remove_file(&MN34041PL_kobj, &(MN34041PL_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&MN34041PL_kobj, &(MN34041PL_MaxFrameHeight_attr.attr));	

	kobject_put(&MN34041PL_kobj);	
	printk(KERN_DEBUG "[MN34041PL] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(MN34041PL_Init);
module_exit(MN34041PL_Exit);
