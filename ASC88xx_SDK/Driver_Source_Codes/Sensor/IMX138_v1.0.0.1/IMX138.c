/*
 * IMX138
 * Driver for IMX138 sensor.
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
#include "IMX138.h"
#include "IMX138_locals.h"
#include "IICCtrl.h" 
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR IMX138[] = "$Version: "IMX138_ID_VERSION"  (IMX138 DRIVER) $";
/*==================================================================*/
static TIMX138Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore IMX138_mutex;

static DWORD dwIMX138Hsync = IMX138_HSYNC_1280x1024;
static DWORD dwIMX138Vsync = IMX138_VSYNC_1280x1024;

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("IMX138 driver");
MODULE_LICENSE("GPL");

BYTE IMX138_DEAFULT_DEVICE_ADDR=0x02;

/*==================================================================*/
const WORD IMX138_InitialTBL1[61][2] = 
{
	{0x04,0x10},//  FIXED
	{0x05,0x01},//  AD 12BIT, NO SHIFT
	{0x06,0x00},//  DRIVE MODE = ALL PIXEL SCAN MODE
	{0x07,0x00},//  NO H,V REVERSE , WIN MODE = ALL PIXEL
	{0x08,0x10},//  FIXED
	{0x09,0x02},//  FRAME RATE = 30 FPS
/* Version 1.0.0.1 modification, 2013.05.20 */
	{0x0A,0x20},//  12BIT BLACK LEVEL[0:7]
/* ========================== */
	{0x0B,0x00},//  12BIT BLACK LEVEL[8]
	{0x0C,0x00},//  FIXED
	{0x0D,0x20},//  FIXED
	{0x0E,0x01},//  FIXED
	{0x0F,0x01},//  FIXED
	{0x10,0x39},//  FIXED
	{0x11,0x00},//  FIXED
	{0x12,0x50},//  FIXED
	{0x13,0x00},//  FIXED
	{0x14,0x00},//  GAIN = 0 DB
	{0x15,0x00},//  FIXED
	{0x16,0x08},//  FIXED
	{0x17,0x01},//  FIXED
	{0x18,0x4C},//  VMAX[0:7]
	{0x19,0x04},//  VMAX[8:15]
	{0x1A,0x00},//  VMAX[16]
	{0x1B,0x94},//  HMAX[0:7]
	{0x1C,0x11},//  HMAX[8:15]
	{0x1D,0xFF},//  FIXED
	{0x1E,0x01},//  FIXED
	{0x1F,0x00},//  FIXED
	{0x20,0x00},//  SUB CONTROL [0:7]
	{0x21,0x00},//  SUB CONTROL [8:15]
	{0x22,0x00},//  SUB CONTROL [16]
	{0x36,0x14},//  VOPB SIZE
	{0x37,0x00},//  FIXED
	{0x38,0x00},//  WINPV[0:7] , V CROPPING POSITION
	{0x39,0x00},//  WINPV[8:10]
	{0x3A,0x19},//  WINWV[0:7] , V CROPPING SIZE
	{0x3B,0x04},//  WINWV[8:10]
	{0x3C,0x00},//  WINPH[0:7] , H CROPPING POSITION
	{0x3D,0x00},//  WINPV[8:10]
	{0x3E,0x1C},//  WINWH[0:7] , H CROPPING SIZE
	{0x3F,0x05},//  WINWH[8:10]
	{0x40,0x00},//  FIXED
	{0x41,0x00},//  FIXED
	{0x42,0x00},//  FIXED
	{0x43,0x00},//  FIXED
	{0x44,0x01},//  12 BIT NUMBER,PARALLEL CMOS O/P
	{0x45,0x01},
	{0x46,0x30},//  XVS PULSE WIDTH
	{0x47,0x18},//  XHS PULSE MIN. O/P
	{0x49,0x00},//  XVS AND XHS O/P FIXED AT HI
	{0x54,0x63},//  FIXED
	{0x5B,0x00},//  INCKSEL1
	{0x5C,0x20},//  FIXED
	{0x5D,0x00},//  INCKSEL2
	{0x5E,0x2C},//  FIXED
	{0x5F,0x00},//  INCKSEL3
	{0x60,0x00},//  FIXED
	{0x61,0x21},//  FIXED
	{0x62,0x08},//  FIXED by SONY
	{0xA1,0x45},//  FIXED
	{0xBF,0x1F},//  FIXED
};

const WORD IMX138_InitialTBL2[137][2]  =
{
	{0x03,0xCD},
	{0x07,0x4B},
	{0x09,0xE9},
	{0x13,0x1B},
	{0x15,0xED},
	{0x16,0x01},
	{0x18,0x09},
	{0x1A,0x19},
	{0x1B,0xA1},
	{0x1C,0x11},
	{0x27,0x00},
	{0x28,0x05},
	{0x29,0xEC},
	{0x2A,0x40},
	{0x2B,0x11},
	{0x2D,0x22},
	{0x2E,0x00},
	{0x2F,0x05},
	{0x31,0xEC},
	{0x32,0x40},
	{0x33,0x11},
	{0x35,0x23},
	{0x36,0xB0},
	{0x37,0x04},
	{0x39,0x24},
	{0x3A,0x30},
	{0x3B,0x04},
	{0x3C,0xED},
	{0x3D,0xC0},
	{0x3E,0x10},
	{0x40,0x44},
	{0x41,0xA0},
	{0x42,0x04},
	{0x43,0x0D},
	{0x44,0x31},
	{0x45,0x11},
	{0x47,0xEC},
	{0x48,0xD0},
	{0x49,0x1D},
	{0x55,0x03},
	{0x56,0x54},
	{0x57,0x60},
	{0x58,0x1F},
	{0x5A,0xA9},
	{0x5B,0x50},
	{0x5C,0x0A},
	{0x5D,0x25},
	{0x5E,0x11},
	{0x5F,0x12},
	{0x61,0x9B},
	{0x66,0xD0},
	{0x67,0x08},
	{0x6A,0x20},
	{0x6B,0x0A},
	{0x6E,0x20},
	{0x6F,0x0A},
	{0x72,0x20},
	{0x73,0x0A},
	{0x75,0xEC},
	{0x7D,0xA5},
	{0x7E,0x20},
	{0x7F,0x0A},
	{0x81,0xEF},
	{0x82,0xC0},
	{0x83,0x0E},
	{0x85,0xF6},
	{0x8A,0x60},
	{0x8B,0x1F},
	{0x8D,0xBB},
	{0x8E,0x90},
	{0x8F,0x0D},
	{0x90,0x39},
	{0x91,0xC1},
	{0x92,0x1D},
	{0x94,0xC9},
	{0x95,0x70},
	{0x96,0x0E},
	{0x97,0x47},
	{0x98,0xA1},
	{0x99,0x1E},
	{0x9B,0xC5},
	{0x9C,0xB0},
	{0x9D,0x0E},
	{0x9E,0x43},
	{0x9F,0xE1},
	{0xA0,0x1E},
	{0xA2,0xBB},
	{0xA3,0x10},
	{0xA4,0x0C},
	{0xA6,0xB3},
	{0xA7,0x30},
	{0xA8,0x0A},
	{0xA9,0x29},
	{0xAA,0x91},
	{0xAB,0x11},
	{0xAD,0xB4},
	{0xAE,0x40},
	{0xAF,0x0A},
	{0xB0,0x2A},
	{0xB1,0xA1},
	{0xB2,0x11},
	{0xB4,0xAB},
	{0xB5,0xB0},
	{0xB6,0x0B},
	{0xB7,0x21},
	{0xB8,0x11},
	{0xB9,0x13},
	{0xBB,0xAC},
	{0xBC,0xC0},
	{0xBD,0x0B},
	{0xBE,0x22},
	{0xBF,0x21},
	{0xC0,0x13},
	{0xC2,0xAD},
	{0xC3,0x10},
	{0xC4,0x0B},
	{0xC5,0x23},
	{0xC6,0x71},
	{0xC7,0x12},
	{0xC9,0xB5},
	{0xCA,0x90},
	{0xCB,0x0B},
	{0xCC,0x2B},
	{0xCD,0xF1},
	{0xCE,0x12},
	{0xD0,0xBB},
	{0xD1,0x10},
	{0xD2,0x0C},
	{0xD4,0xE7},
	{0xD5,0x90},
	{0xD6,0x0E},
	{0xD8,0x45},
	{0xD9,0x11},
	{0xDA,0x1F},
	{0xEB,0xA4},
	{0xEC,0x60},
	{0xED,0x1F},	
};

const DWORD IMX138_GAINTBL[161][2] = 
{
	{  1000,   0},//	 0.0 	dB
	{  1035,   1},//	 0.3	dB	           
	{  1072,   2},//	 0.6	dB	           
	{  1110,   3},//	 0.9	dB	           
	{  1149,   4},//	 1.2	dB	           
	{  1189,   5},//	 1.5	dB	           
	{  1231,   6},//	 1.81	dB           
	{  1274,   7},//	 2.11	dB
	{  1319,   8},//	 2.41	dB
	{  1366,   9},//	 2.71	dB
	{  1414,  10},//	 3.01	dB
	{  1464,  11},//	 3.31	dB
	{  1516,  12},//	 3.61	dB
	{  1569,  13},//	 3.91	dB
	{  1624,  14},//	 4.21	dB
	{  1682,  15},//	 4.51	dB
	{  1741,  16},//	 4.82	dB
	{  1802,  17},//	 5.12	dB
	{  1866,  18},//	 5.42	dB
	{  1932,  19},//	 5.72	dB
	{  2000,  20},//	 6.02	dB
	{  2070,  21},//	 6.32	dB
	{  2143,  22},//	 6.62	dB
	{  2219,  23},//	 6.92	dB
	{  2297,  24},//	 7.22	dB
	{  2378,  25},//	 7.52	dB
	{  2462,  26},//	 7.82	dB
	{  2548,  27},//	 8.13	dB
	{  2638,  28},//	 8.43	dB
	{  2731,  29},//	 8.73	dB
	{  2828,  30},//	 9.03	dB
	{  2927,  31},//	 9.33	dB
	{  3031,  32},//	 9.63	dB
	{  3137,  33},//	 9.93	dB
	{  3248,  34},//	10.23	dB
	{  3362,  35},//	10.53	dB
	{  3481,  36},//	10.83	dB
	{  3604,  37},//	11.14	dB
	{  3731,  38},//	11.44	dB
	{  3862,  39},//	11.74	dB
	{  4000,  40},//	12.04	dB
	{  4139,  41},//	12.34	dB
	{  4285,  42},//	12.64	dB
	{  4436,  43},//	12.94	dB
	{  4593,  44},//	13.24	dB
	{  4755,  45},//	13.54	dB
	{  4922,  46},//	13.84	dB
	{  5096,  47},//	14.14	dB
	{  5276,  48},//	14.45	dB
	{  5462,  49},//	14.75	dB
	{  5654,  50},//	15.05	dB
	{  5853,  51},//	15.35	dB
	{  6060,  52},//	15.65	dB
	{  6274,  53},//	15.95	dB
	{  6495,  54},//	16.25	dB
	{  6724,  55},//	16.55	dB
	{  6961,  56},//	16.85	dB
	{  7206,  57},//	17.15	dB
	{  7460,  58},//	17.45	dB
	{  7723,  59},//	17.76	dB
	{  8000,  60},//	18.06	dB
	{  8277,  61},//	18.36	dB
	{  8569,  62},//	18.66	dB
	{  8871,  63},//	18.96	dB
	{  9184,  64},//	19.26	dB
	{  9508,  65},//	19.56	dB
	{  9843,  66},//	19.86	dB
	{ 10190,  67},//	20.16	dB
	{ 10549,  68},//	20.46	dB
	{ 10921,  69},//	20.77	dB
	{ 11306,  70},//	21.07	dB
	{ 11705,  71},//	21.37	dB
	{ 12117,  72},//	21.67	dB
	{ 12545,  73},//	21.97	dB
	{ 12987,  74},//	22.27	dB
	{ 13445,  75},//	22.57	dB
	{ 13919,  76},//	22.87	dB
	{ 14409,  77},//	23.17	dB
	{ 14917,  78},//	23.47	dB
	{ 15443,  79},//	23.77	dB
	{ 16000,  80},//	24.08	dB
	{ 16551,  81},//	24.38	dB
	{ 17135,  82},//	24.68	dB
	{ 17739,  83},//	24.98	dB
	{ 18364,  84},//	25.28	dB
	{ 19012,  85},//	25.58	dB
	{ 19682,  86},//	25.88	dB
	{ 20376,  87},//	26.18	dB
	{ 21094,  88},//	26.48	dB
	{ 21838,  89},//	26.78	dB
	{ 22608,  90},//	27.09	dB
	{ 23405,  91},//	27.39	dB
	{ 24230,  92},//	27.69	dB
	{ 25084,  93},//	27.99	dB
	{ 25969,  94},//	28.29	dB
	{ 26884,  95},//	28.59	dB
	{ 27832,  96},//	28.89	dB
	{ 28813,  97},//	29.19	dB
	{ 29829,  98},//	29.49	dB
	{ 30881,  99},//	29.79	dB
	{ 32000, 100},//	30.09	dB
	{ 33096, 101},//	30.4	dB	
	{ 34263, 102},//	30.7	dB	
	{ 35471, 103},//	31.0	dB	
	{ 36722, 104},//	31.3	dB	
	{ 38016, 105},//	31.6	dB	
	{ 39357, 106},//	31.9	dB	
	{ 40744, 107},//	32.2	dB	
	{ 42181, 108},//	32.5	dB	
	{ 43668, 109},//	32.8	dB	
	{ 45207, 110},//	33.1	dB	
	{ 46801, 111},//	33.41	dB
	{ 48451, 112},//	33.71	dB
	{ 50159, 113},//	34.01	dB
	{ 51927, 114},//	34.31	dB
	{ 53758, 115},//	34.61	dB
	{ 55653, 116},//	34.91	dB
	{ 57616, 117},//	35.21	dB
	{ 59647, 118},//	35.51	dB
	{ 61750, 119},//	35.81	dB
	{ 64000, 120},//	36.11	dB
	{ 66257, 121},//	36.3	dB
	{ 68594, 122},//	36.6	dB
	{ 71013, 123},//	36.9	dB
	{ 73517, 124},//	37.2	dB
	{ 76110, 125},//	37.5	dB
	{ 78794, 126},//	37.8	dB
	{ 81573, 127},//	38.1	dB
	{ 84450, 128},//	38.4	dB
	{ 87428, 129},//	38.7	dB
	{ 90511, 130},//	39.0	dB
	{ 93703, 131},//	39.3	dB
	{ 97007, 132},//	39.6	dB
	{100428, 133},//	39.9	dB
	{103970, 134},//	40.2	dB
	{107636, 135},//	40.5	dB
	{111432, 136},//	40.8	dB
	{115362, 137},//	41.1	dB
	{119430, 138},//	41.4	dB
	{123642, 139},//	41.7	dB
	{128000, 140},//	42.0	dB
/* Version 1.0.0.1 modification, 2013.05.20 */
	{132514, 141},//	42.3	dB
	{137187, 142},//	42.6	dB
	{142025, 143},//	42.9	dB
	{147034, 144},//	43.2	dB
	{152219, 145},//	43.5	dB
	{157587, 146},//	43.8	dB
	{163144, 147},//	44.1	dB
	{168897, 148},//	44.4	dB
	{174853, 149},//	44.7	dB
	{181019, 150},//	45.0	dB
	{187403, 151},//	45.3	dB
	{194012, 152},//	45.6	dB
	{200854, 153},//	45.9	dB
	{207937, 154},//	46.3	dB
	{215270, 155},//	46.5	dB
	{222862, 156},//	46.8	dB
	{230721, 157},//	47.1	dB
	{238858, 158},//	47.4	dB
	{247282, 159},//	47.7	dB
	{256000, 160},//	48.0	dB
/* ========================== */
};

/*-------------------------------------------------------------------------------------*/
static BYTE IMX138_BitReverse(BYTE byData)
{
	BYTE byOut;

	byOut = ((byData<<7)&0x80) | ((byData<<5)&0x40) | ((byData<<3)&0x20) | ((byData<<1)&0x10) |
			((byData>>1)&0x08) | ((byData>>3)&0x04) | ((byData>>5)&0x02) | ((byData>>7)&0x01);

	return byOut;
}

/*-------------------------------------------------------------------------------------*/
static int IMX138_WriteReg(TIMX138Info *ptInfo, EIMX138Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BOOL bWaitCmpt)
{
	BYTE abyData[6];
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);

	byOutRegAddr = IMX138_BitReverse(IMX138_DEAFULT_DEVICE_ADDR);
	abyData[0] = IMX138_BitReverse((BYTE)eRegAddr);
	abyData[1] = IMX138_BitReverse(dwData&0xFF);
	abyData[2] = IMX138_BitReverse((dwData>>8)&0xFF);
	abyData[3] = IMX138_BitReverse((dwData>>16)&0xFF);
	abyData[4] = IMX138_BitReverse((dwData>>24)&0xFF);
	abyData[dwLength] = (BYTE)bWaitCmpt;

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, (DWORD)byOutRegAddr, (dwLength+1), abyData);
}

/*-------------------------------------------------------------------------------------*/
static void IMX138_Reset(TIMX138Info *ptInfo)
{
	DWORD dwFlags;
	WORD tempcount;

	local_irq_save(dwFlags);

	IMX138_DEAFULT_DEVICE_ADDR = 0x02;
	IMX138_WriteReg(ptInfo, 0x00,0x01, 1, false);// STANDBY
	IMX138_WriteReg(ptInfo, 0x01,0x00, 1, false);// REG NOT HOLD
	IMX138_WriteReg(ptInfo, 0x02,0x01, 1, false);// MASTER MODE STOP
	IMX138_WriteReg(ptInfo, 0x03,0x01, 1, false);// SW RESET START
	mdelay(50);
	IMX138_WriteReg(ptInfo, 0x03,0x00, 1, false);// SW RESET STOP
	mdelay(50);
	for (tempcount=0; tempcount<61; tempcount++)
		IMX138_WriteReg(ptInfo, IMX138_InitialTBL1[tempcount][0], IMX138_InitialTBL1[tempcount][1], 1, false);

	IMX138_DEAFULT_DEVICE_ADDR=0x03;
	IMX138_WriteReg(ptInfo, 0x12,0x00, 1, false);
	IMX138_WriteReg(ptInfo, 0x1D,0x07, 1, false);
	IMX138_WriteReg(ptInfo, 0x23,0x07, 1, false);
	IMX138_WriteReg(ptInfo, 0x26,0xDF, 1, false);
	IMX138_WriteReg(ptInfo, 0x47,0x87, 1, false);

	IMX138_DEAFULT_DEVICE_ADDR=0x04;
	for (tempcount=0; tempcount<137; tempcount++)
		IMX138_WriteReg(ptInfo, IMX138_InitialTBL2[tempcount][0], IMX138_InitialTBL2[tempcount][1], 1, false);

	IMX138_DEAFULT_DEVICE_ADDR=0x02;

	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
	{
		dwIMX138Hsync = IMX138_HSYNC_1280x1024;
		dwIMX138Vsync = IMX138_VSYNC_1280x1024;	

	}
	else
	{
		dwIMX138Hsync = IMX138_HSYNC_1280x1024_50;
		dwIMX138Vsync = IMX138_VSYNC_1280x1024_50;
	} 
 	IMX138_WriteReg(ptInfo, 0x1B, (dwIMX138Hsync<<1), 2, false);
 	IMX138_WriteReg(ptInfo, 0x18, dwIMX138Vsync, 3, false);
	IMX138_WriteReg(ptInfo, 0x00, 0, 1, false);// STANDBY CANCEL
	mdelay(50);
	IMX138_WriteReg(ptInfo, 0x02, 0, 1, false);// Master mode start
	IMX138_WriteReg(ptInfo, 0x49, 0x0A, 1, false);// XVS AND XHS O/P start
	mdelay(50);

	local_irq_restore(dwFlags);

	ptInfo->dwPreExpFrame = 0xFFFF;
}

/*-------------------------------------------------------------------------------------*/
static void IMX138_SetShutter(DWORD dwShutter)
{
	DWORD dwExpoLine, dwExpoLineSend, dwVMAX;

	if (dwShutter > 1000000)
		dwShutter = 1000000;

	dwExpoLine = ((dwShutter*IMX138_PIXEL_CLOCK_M_X100)/100) / dwIMX138Hsync;

	dwVMAX = (dwExpoLine>dwIMX138Vsync) ? dwExpoLine : dwIMX138Vsync;

	dwExpoLineSend = dwVMAX - dwExpoLine;

	if (dwExpoLineSend >= (dwVMAX-2))
		dwExpoLineSend = dwVMAX - 2;
	else if (dwExpoLineSend <= 3)
		dwExpoLineSend = 3;

	dwExpoLineSend = (dwExpoLineSend>=0x1FFFF) ? 0x1FFFF : dwExpoLineSend;
	dwVMAX = (dwVMAX>=0x1FFFF) ? 0x1FFFF : dwVMAX;

	if (ptInfo->dwPreExpFrame != dwVMAX)
	{
		IMX138_WriteReg(ptInfo, 0x18, dwVMAX, 3, TRUE);
		IMX138_WriteReg(ptInfo, 0x20, dwExpoLineSend, 3, FALSE);
		ptInfo->dwPreExpFrame = dwVMAX;
	}
	else
	{
		IMX138_WriteReg(ptInfo, 0x20, dwExpoLineSend, 3, TRUE);
	}

	ptInfo->dwCurShutter = (dwExpoLine&0xFFFF);	// shutter frame num
}

/*-------------------------------------------------------------------------------------*/
static void IMX138_SetGain(DWORD dwGain)
{
	WORD tblidx,tempcount;

	if (dwGain > IMX138_MAX_GAIN)
		dwGain = IMX138_MAX_GAIN;
	else if (dwGain < 1000)
		dwGain = 1000;

	if (dwGain <= 2000)      //6dB
		tblidx = 0;
	else if (dwGain <= 4000) //12dB
		tblidx = 20;
	else if (dwGain <= 8000) //18dB
		tblidx = 40;
	else if (dwGain <= 16000)//24dB
		tblidx = 60;
	else if (dwGain <= 32000)//30dB
		tblidx = 80;
	else if (dwGain <= 64000)//36dB
		tblidx =100;
/* Version 1.0.0.1 modification, 2013.05.20 */
	else if (dwGain <= 128000)//42dB
		tblidx =120;
	else
		tblidx =140;
/* ========================== */
  
	for (tempcount=0; tempcount<20; tempcount++,tblidx++)
	{
		if (dwGain <= IMX138_GAINTBL[tblidx][0])
			break;
	}

	IMX138_WriteReg(ptInfo, 0x14,  IMX138_GAINTBL[tblidx][1], 1, TRUE);
}

/*==================================================================*/
SOCKET  IMX138_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET sckRet = 0;

	if (down_interruptible(&IMX138_mutex))
		return -ERESTARTSYS;

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		IMX138_Reset(ptInfo);
		break;

		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		IMX138_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
            
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		IMX138_SetGain(ptIoctlArg->adwUserData[0]);
		break;
           
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ)){
				printk("[IMX138.ko] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			sckRet = -EPERM;
			break;
		}
		if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwIMX138Hsync = IMX138_HSYNC_1280x1024;
			dwIMX138Vsync = IMX138_VSYNC_1280x1024;
        	}
		else
		{
			dwIMX138Hsync = IMX138_HSYNC_1280x1024_50;
			dwIMX138Vsync = IMX138_VSYNC_1280x1024_50;
		}

		if(ptInfo->dwCurShutter > (dwIMX138Vsync-1))
		{
			ptInfo->dwCurShutter = dwIMX138Vsync-1;
			IMX138_WriteReg(ptInfo, 0x20, ptInfo->dwCurShutter, 3, false);
		}
	 	IMX138_WriteReg(ptInfo, 0x18, dwIMX138Vsync, 3, false);		
	 	IMX138_WriteReg(ptInfo, 0x1B, (dwIMX138Hsync<<1), 2, false);
		mdelay(40);
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];	
		break;

		default:
		sckRet = -EPERM;
	}
	up(&IMX138_mutex);
	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
DWORD IMX138_VICGetShutterValue(void)
{
	DWORD dwRet;

	dwRet = ptInfo->dwCurShutter;

	return dwRet;
}

/*-------------------------------------------------------------------------------------*/
DWORD IMX138_GetRemainingLineNum(void)
{
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		return (IMX138_VSYNC_1280x1024    - IMX138_DEFAULT_OUTPUT_HEIGHT - 16 - 8);
	else
		return (IMX138_VSYNC_1280x1024_50 - IMX138_DEFAULT_OUTPUT_HEIGHT - 16 - 8);
}
/*-------------------------------------------------------------------------------------*/
void IMX138_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET IMX138_Open(TVideoSensorInitialParam* ptIMX138InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptIMX138InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(IMX138_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptIMX138InitialParam->dwVideoSensorVersion&0x0000FF00)>(IMX138_INTERFACE_VERSION&0x0000FF00))){
		printk("Invalid IMX138 device driver version %d.%d.%d.%d !!\n", 
				(int)(IMX138_INTERFACE_VERSION&0xFF), 
				(int)((IMX138_INTERFACE_VERSION>>8)&0xFF), 
				(int)((IMX138_INTERFACE_VERSION>>16)&0xFF), 
				(int)(IMX138_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TIMX138Info *)kmalloc(sizeof(TIMX138Info), GFP_KERNEL))){
		printk("[IMX138.ko] : Allocate %d bytes memory fail\n", sizeof(TIMX138Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_WRITEBUF_FAST_NOACK;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[IMX138.ko] : Initial IICCtrl object fail !!\n");
		IMX138_Release(dwDevNum);					
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = IMX138_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptIMX138InitialParam->dwVideoSensorFrequency;
	ptInfo->bInit = FALSE;

	// Reset device
	IMX138_Reset(ptInfo);
	ptInfo->bInit = TRUE;	
	return 0;
}

/*-------------------------------------------------------------------------------------*/
DWORD IMX138_GetMaxGain(void)
{
	return (IMX138_MAX_GAIN);
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open			= IMX138_Open,
	.release		= IMX138_Release,
	.ioctl			= IMX138_Ioctl,
	.get_shutter_value	= NULL,//IMX138_VICGetShutterValue,
	.get_remaining_line_num	= NULL,//IMX138_GetRemainingLineNum,
	.group_access		= NULL,	
	.get_exp_statistic	= NULL,
	.get_wb_statistic	= NULL,
	.get_max_gain		= IMX138_GetMaxGain,
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct IMX138_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t IMX138_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct IMX138_attribute *IMX138_attr = container_of(attr, struct IMX138_attribute, attr);
	int result = 0;
	
	if (IMX138_attr->show)
		result = IMX138_attr->show(kobj, buf);	
	return result;
}

ssize_t IMX138_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "IMX138");
	return retval;
}

ssize_t IMX138_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t IMX138_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1280");
	return retval;
}

ssize_t IMX138_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1024");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject IMX138_kobj;

struct IMX138_attribute IMX138_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX138_SensorName_attr_show,
};

struct IMX138_attribute IMX138_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX138_dwBufNum_attr_show,
};

struct IMX138_attribute IMX138_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX138_MaxFrameWidth_attr_show,
};

struct IMX138_attribute IMX138_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX138_MaxFrameHeight_attr_show,
};

struct sysfs_ops IMX138_sysfs_ops = {
	.show	= IMX138_default_attr_show,
};

struct kobj_type IMX138_ktype = {
	.sysfs_ops	= &IMX138_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int IMX138_Init(void)
{
	int result = 0;
	
	if (result < 0){
		printk("[IMX138] : kobj set name fail!\n");
		return -1;
	}

	IMX138_kobj.ktype = &IMX138_ktype;	
	result = kobject_init_and_add(&IMX138_kobj, &IMX138_ktype, NULL, "VideoSensorInfo");

	if (result < 0){
		printk("[IMX138] : Cannot register kobject [IMX138_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&IMX138_kobj, &IMX138_SensorName_attr.attr);		
	if (result < 0){
		printk("[IMX138] : Cannot create IMX138_SensorName_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX138_kobj, &IMX138_dwBufNum_attr.attr);		
	if (result < 0){
		printk("[IMX138] : Cannot create IMX138_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX138_kobj, &IMX138_MaxFrameWidth_attr.attr);		
	if (result < 0){
		printk("[IMX138] : Cannot create IMX138_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX138_kobj, &IMX138_MaxFrameHeight_attr.attr);		
	if (result < 0){
		printk("[IMX138] : Cannot create IMX138_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&IMX138_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void IMX138_Exit(void)
{
	sysfs_remove_file(&IMX138_kobj, &(IMX138_SensorName_attr.attr));
	sysfs_remove_file(&IMX138_kobj, &(IMX138_dwBufNum_attr.attr));	
	sysfs_remove_file(&IMX138_kobj, &(IMX138_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&IMX138_kobj, &(IMX138_MaxFrameHeight_attr.attr));		
	kobject_put(&IMX138_kobj);	
	printk(KERN_DEBUG "[IMX138] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(IMX138_Init);
module_exit(IMX138_Exit);

