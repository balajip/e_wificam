/*
 * OV5658
 * Driver for OV5658 sensor.
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

#include "OV5658.h"
#include "OV5658_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h>

const CHAR OV5658_ID[] = "$Version: "OV5658_ID_VERSION"  (OV5658 DRIVER) $";

/*==================================================================*/
static TOV5658Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore OV5658_mutex;

static DWORD dwOV5658Hsync = OV5658_HSYNC_2560x1920_30FPS;
static DWORD dwOV5658Vsync = OV5658_VSYNC_2560x1920;

/* Sensor capture width */
static SOCKET video_sensor_width = 2560;
/* Sensor capture height */
static SOCKET video_sensor_height = 1920;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "OV5658 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "OV5658 capture height");

static int iEnAWB=0;
module_param(iEnAWB, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnAWB, "Enable sensor AWB");

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("OV5658 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TOV5658RegAddrData atOV5658regTbl[OV5658_TBL_SIZE] = 
{
	{0x0103, 0x01},
	{0x3210, 0x43},
	{0x3001, 0x0e},
	{0x3002, 0xc0},
	{0x3011, 0x41},
	{0x3012, 0x0a},
	{0x3013, 0x50},
	{0x3015, 0x09},
	{0x3018, 0xf0},
	{0x3021, 0x40},
	{0x3500, 0x00},
	{0x3501, 0x7b},
	{0x3502, 0x00},
	{0x3503, 0x07},
	{0x3505, 0x00},
	{0x3506, 0x00},
	{0x3507, 0x02},
	{0x3508, 0x00},
	{0x3509, 0x10},
	{0x350a, 0x00},
	{0x350b, 0x80},
	{0x3600, 0x4b},
	{0x3602, 0x3c},
	{0x3605, 0x14},
	{0x3606, 0x09},
	{0x3612, 0x04},
	{0x3613, 0x66},
	{0x3614, 0x39},
	{0x3615, 0x33},
	{0x3616, 0x46},
	{0x361a, 0x0a},
	{0x361c, 0x76},
	{0x3620, 0x40},
	{0x3640, 0x03},
	{0x3641, 0x60},
	{0x3642, 0x00},
	{0x3643, 0x90},
	{0x3660, 0x04},
	{0x3665, 0x00},
	{0x3666, 0x20},
	{0x3667, 0x00},
	{0x366a, 0x80},
	{0x3680, 0xe0},
	{0x3692, 0x80},
	{0x3700, 0x42},
	{0x3701, 0x14},
	{0x3702, 0xe8},
	{0x3703, 0x20},
	{0x3704, 0x5e},
	{0x3705, 0x02},
	{0x3708, 0xe3},
	{0x3709, 0xc3},
	{0x370a, 0x00},
	{0x370b, 0x20},
	{0x370c, 0x0c},
	{0x370d, 0x11},
	{0x370e, 0x00},
	{0x370f, 0x40},
	{0x3710, 0x00},
	{0x3715, 0x09},
	{0x371a, 0x04},
	{0x371b, 0x05},
	{0x371c, 0x01},
	{0x371e, 0xa1},
	{0x371f, 0x18},
	{0x3721, 0x00},
	{0x3726, 0x00},
	{0x372a, 0x01},
	{0x3730, 0x10},
	{0x3738, 0x22},
	{0x3739, 0xe5},
	{0x373a, 0x50},
	{0x373b, 0x02},
	{0x373c, 0x2c},
	{0x373f, 0x02},
	{0x3740, 0x42},
	{0x3741, 0x02},
	{0x3743, 0x01},
	{0x3744, 0x02},
	{0x3747, 0x00},
	{0x3754, 0xc0},
	{0x3755, 0x07},
	{0x3756, 0x1a},
	{0x3759, 0x0f},
	{0x375c, 0x04},
	{0x3767, 0x00},
	{0x376b, 0x44},
	{0x377b, 0x44},
	{0x377c, 0x30},
	{0x377e, 0x30},
	{0x377f, 0x08},
	{0x3781, 0x0c},
	{0x3785, 0x1e},
	{0x378f, 0xf5},
	{0x3791, 0xb0},
	{0x379c, 0x0c},
	{0x379d, 0x20},
	{0x379e, 0x00},
	{0x3796, 0x72},
	{0x379a, 0x07},
	{0x379b, 0xb0},
	{0x37c5, 0x00},
	{0x37c6, 0x00},
	{0x37c7, 0x00},
	{0x37c9, 0x00},
	{0x37ca, 0x00},
	{0x37cb, 0x00},
	{0x37cc, 0x00},
	{0x37cd, 0x00},
	{0x37ce, 0x01},
	{0x37cf, 0x00},
	{0x37d1, 0x00},
	{0x37de, 0x00},
	{0x37df, 0x00},
	{0x3800, 0x00},
	{0x3801, 0x00},
	{0x3802, 0x00},
	{0x3803, 0x00},
	{0x3804, 0x0a},
	{0x3805, 0x3f},
	{0x3806, 0x07},
	{0x3807, 0xa3},
	{0x3808, 0x09},
	{0x3809, 0x80},
	{0x380a, 0x06},
	{0x380b, 0xc0},
	{0x380c, 0x0c},
	{0x380d, 0x98},
	{0x380e, 0x07},
	{0x380f, 0xc0},
	{0x3810, 0x00},
	{0x3811, 0x10},
	{0x3812, 0x00},
	{0x3813, 0x06},
	{0x3814, 0x11},
	{0x3815, 0x11},
	{0x3820, 0x10},
	{0x3821, 0x1e},
	{0x3823, 0x00},
	{0x3824, 0x00},
	{0x3825, 0x00},
	{0x3826, 0x00},
	{0x3827, 0x00},
	{0x3829, 0x0b},
	{0x382a, 0x04},
	{0x382c, 0x34},
	{0x382d, 0x00},
	{0x3a04, 0x06},
	{0x3a05, 0x14},
	{0x3a06, 0x00},
	{0x3a07, 0xfe},
	{0x3b00, 0x00},
	{0x3b02, 0x00},
	{0x3b03, 0x00},
	{0x3b04, 0x00},
	{0x3b05, 0x00},
	{0x4000, 0x18},
	{0x4001, 0x04},
	{0x4002, 0x45},
	{0x4004, 0x04},
	{0x4005, 0x18},
	{0x4006, 0x20},
	{0x4007, 0x98},
	{0x4008, 0x24},
	{0x4009, 0x10},
	{0x400c, 0x00},
	{0x400d, 0x00},
	{0x404e, 0x37},
	{0x404f, 0x8f},
	{0x4058, 0x00},
	{0x4100, 0x50},
	{0x4101, 0x34},
	{0x4102, 0x34},
	{0x4104, 0xde},
	{0x4300, 0xff},
	{0x4307, 0x30},
	{0x4311, 0x04},
	{0x4315, 0x01},
	{0x4501, 0x08},
	{0x4502, 0x08},
	{0x4816, 0x52},
	{0x481f, 0x30},
	{0x4826, 0x28},
	{0x4837, 0x0d},
	{0x4a00, 0xaa},
	{0x4a02, 0x00},
	{0x4a03, 0x01},
	{0x4a05, 0x40},
	{0x4a0a, 0x88},
	{0x5000, 0x06},
	{0x5001, 0x01},
	{0x5002, 0x00},
	{0x5003, 0x20},
	{0x5013, 0x00},
	{0x501f, 0x00},
	{0x5043, 0x48},
	{0x5780, 0x1c},
	{0x5786, 0x20},
	{0x5788, 0x18},
	{0x578a, 0x04},
	{0x578b, 0x02},
	{0x578c, 0x02},
	{0x578e, 0x06},
	{0x578f, 0x02},
	{0x5790, 0x02},
	{0x5791, 0xff},
	{0x5e00, 0x00},
	{0x5e10, 0x0c},
	{0x0100, 0x01},
	{0x5800, 0x22},
	{0x5801, 0x11},
	{0x5802, 0x0d},
	{0x5803, 0x0d},
	{0x5804, 0x12},
	{0x5805, 0x26},
	{0x5806, 0x09},
	{0x5807, 0x07},
	{0x5808, 0x05},
	{0x5809, 0x05},
	{0x580a, 0x07},
	{0x580b, 0x0a},
	{0x580c, 0x07},
	{0x580d, 0x02},
	{0x580e, 0x00},
	{0x580f, 0x00},
	{0x5810, 0x03},
	{0x5811, 0x07},
	{0x5812, 0x06},
	{0x5813, 0x02},
	{0x5814, 0x00},
	{0x5815, 0x00},
	{0x5816, 0x03},
	{0x5817, 0x07},
	{0x5818, 0x09},
	{0x5819, 0x05},
	{0x581a, 0x04},
	{0x581b, 0x04},
	{0x581c, 0x07},
	{0x581d, 0x09},
	{0x581e, 0x1d},
	{0x581f, 0x0e},
	{0x5820, 0x0b},
	{0x5821, 0x0b},
	{0x5822, 0x0f},
	{0x5823, 0x1e},
	{0x5824, 0x59},
	{0x5825, 0x46},
	{0x5826, 0x37},
	{0x5827, 0x36},
	{0x5828, 0x45},
	{0x5829, 0x39},
	{0x582a, 0x46},
	{0x582b, 0x44},
	{0x582c, 0x45},
	{0x582d, 0x28},
	{0x582e, 0x38},
	{0x582f, 0x52},
	{0x5830, 0x60},
	{0x5831, 0x51},
	{0x5832, 0x26},
	{0x5833, 0x38},
	{0x5834, 0x43},
	{0x5835, 0x42},
	{0x5836, 0x34},
	{0x5837, 0x18},
	{0x5838, 0x05},
	{0x5839, 0x27},
	{0x583a, 0x27},
	{0x583b, 0x27},
	{0x583c, 0x0a},
	{0x583d, 0xbf},
	{0x5842, 0x01},
	{0x5843, 0x2b},
	{0x5844, 0x01},
	{0x5845, 0x92},
	{0x5846, 0x01},
	{0x5847, 0x8f},
	{0x5848, 0x01},
	{0x5849, 0x0c},
};

/*==================================================================*/
static SOCKET OV5658_WriteBuf(TOV5658Info *ptInfo, EOV5658Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[6];
	DWORD dwLength = (dwDataLen+1);

	abyData[0] = (BYTE)(eRegAddr&0xFF);

	if (dwDataLen == 1)
	{
		abyData[1] = (BYTE)(dwData&0xFF);
	}
	else if (dwDataLen == 2)
	{
		abyData[1] = (BYTE)((dwData>>8)&0xFF);
		abyData[2] = (BYTE)(dwData&0xFF);
	}
	else if (dwDataLen == 3)
	{
		abyData[1] = (BYTE)((dwData>>16)&0xFF);
		abyData[2] = (BYTE)((dwData>>8)&0xFF);
		abyData[3] = (BYTE)(dwData&0xFF);
	}
	else
	{
		abyData[1] = (BYTE)((dwData>>24)&0xFF);
		abyData[2] = (BYTE)((dwData>>16)&0xFF);
		abyData[3] = (BYTE)((dwData>>8)&0xFF);
		abyData[4] = (BYTE)(dwData&0xFF);
	}

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), dwLength, abyData);
}

/*-------------------------------------------------------------------------------------*/
static SOCKET OV5658_ReadBuf(TOV5658Info *ptInfo, EOV5658Regs eRegAddr, BYTE *pbyData)
{
	*pbyData = (BYTE)(eRegAddr&0xFF);
	return ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, ((eRegAddr&0xFF00)>>8), 1, pbyData);
}
 
/*-------------------------------------------------------------------------------------*/
static SOCKET OV5658_CompBuf(TOV5658Info *ptInfo, EOV5658Regs eRegAddr, BYTE byMask, BYTE byData)
{
	BYTE byReadData;

	OV5658_ReadBuf(ptInfo, eRegAddr, &byReadData);
	return ((byReadData&byMask) == byData);
}

/*-------------------------------------------------------------------------------------*/
static SOCKET OV5658_UpdateBuf(TOV5658Info *ptInfo, EOV5658Regs eRegAddr, BYTE byMask, BYTE byData)
{
	BYTE byReadData, byWriteData;

	if (OV5658_ReadBuf(ptInfo, eRegAddr, &byReadData) < 0)
	{
		return -1;
	}
	byWriteData = ((byReadData&byMask) | byData);
	if (OV5658_WriteBuf(ptInfo, eRegAddr, byWriteData, 1, 0) < 0)
	{
		return -1;
	}
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void OV5658_ReadCompGain(TOV5658Info *ptInfo, EOV5658Regs eRegAddrH, EOV5658Regs eRegAddrL, DWORD *pdwData)
{
	BYTE byData;

	OV5658_ReadBuf(ptInfo, eRegAddrH, &byData) ;
	*pdwData = ((byData&0x0F) << 8);
	OV5658_ReadBuf(ptInfo, eRegAddrL, &byData) ;
	*pdwData = *pdwData + byData;
}

/*-------------------------------------------------------------------------------------*/
static void OV5658_Reset(TOV5658Info *ptInfo)
{
	DWORD dwIndex;

	// SW standby
	OV5658_WriteBuf(ptInfo, _0100_ModeSelect, 0x00, 1, 0);
	mdelay(30);

	// SW reset
	OV5658_WriteBuf(ptInfo, _0103_SWReset, 0x01, 1, 0);
	while (!OV5658_CompBuf(ptInfo, _0103_SWReset, 0x01, 0x00));

	for (dwIndex=0; dwIndex<OV5658_TBL_SIZE; dwIndex++)
	{
		OV5658_WriteBuf(ptInfo, atOV5658regTbl[dwIndex].eRegAddr, atOV5658regTbl[dwIndex].byData, 1, 0);
	}

	if (!ptInfo->bEnOV5658AWB)
	{
		OV5658_WriteBuf(ptInfo, _5001_IspCtrl1, 0x01, 1, 0);
	}

	OV5658_WriteBuf(ptInfo, _3600_PLL10, 0x70, 1, 0);
	OV5658_WriteBuf(ptInfo, _3601_PLL11, 0x23, 1, 0);

	printk("OV5658_Reset Done \n");
}

/*-------------------------------------------------------------------------------------*/
void OV5658_SetShutter(DWORD dwShutter)
{
	DWORD dwExpLine;

	dwExpLine = (dwShutter*OV5658_PIXEL_CLOCK_M) / dwOV5658Hsync;

	dwExpLine = (dwExpLine > 0x000FFFFF)
				? 0x000FFFFF :
				(dwExpLine < 5)
				? 5
				: dwExpLine;

	dwExpLine = dwExpLine << 4;

	OV5658_WriteBuf(ptInfo, _3500_AECLongExp2, (dwExpLine&0x0FFFF0), 3, 0);
}

/*-------------------------------------------------------------------------------------*/
void OV5658_SetGain(DWORD dwGain)
{
	DWORD dwData;

	dwGain = (dwGain > OV5658_MAX_GAIN) ?
				OV5658_MAX_GAIN :
				(dwGain < 1000) ?
				1000 :
				dwGain;

	dwData = (dwGain<<4) / 1000;

	OV5658_WriteBuf(ptInfo, _350B_AECAgcAdj0, (dwData&0xFF), 1, 0);
}

/*-------------------------------------------------------------------------------------*/
SOCKET OV5658_SetSize(DWORD dwWidth, DWORD dwHeight)
{
	if ((dwWidth==2560) && (dwHeight==1920))	// No bin No sklp
	{
		OV5658_WriteBuf(ptInfo, _3820_TimingFormat1, 0x10, 1, 0); // vertical bin
		OV5658_WriteBuf(ptInfo, _3821_TimingFormat2, 0x1E, 1, 0); // hori. bin
		OV5658_WriteBuf(ptInfo, _4501_InputSwapManEn, 0x00, 1, 0); // sum or avergae for bin

		OV5658_WriteBuf(ptInfo, _3808_TimingXOutputSize1, 0x0A, 1, 0);
		OV5658_WriteBuf(ptInfo, _3809_TimingXOutputSize0, 0x08, 1, 0);
		OV5658_WriteBuf(ptInfo, _380A_TimingYOutputSize1, 0x07, 1, 0);
		OV5658_WriteBuf(ptInfo, _380B_TimingYOutputSize0, 0x88, 1, 0);

		OV5658_WriteBuf(ptInfo, _3800_TimingXAddrStart1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3801_TimingXAddrStart0, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3802_TimingYAddrStart1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3803_TimingYAddrStart0, 0x00, 1, 0);

		OV5658_WriteBuf(ptInfo, _3804_TimingXAddrEnd1, 0x0A, 1, 0);
		OV5658_WriteBuf(ptInfo, _3805_TimingXAddrEnd0, 0x3F, 1, 0);
		OV5658_WriteBuf(ptInfo, _3806_TimingYAddrEnd1, 0x07, 1, 0);
		OV5658_WriteBuf(ptInfo, _3807_TimingYAddrEnd0, 0xA3, 1, 0);

		OV5658_WriteBuf(ptInfo, _3810_TimingIspXWin1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3811_TimingIspXWin0, 0x10, 1, 0);
		OV5658_WriteBuf(ptInfo, _3812_TimingIspYWin1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3813_TimingIspYWin0, 0x05, 1, 0);

		dwOV5658Vsync = OV5658_VSYNC_2560x1920;
	}
	else if ((dwWidth==2432) && (dwHeight==1728))	// No bin No sklp
	{
		OV5658_WriteBuf(ptInfo, _3820_TimingFormat1, 0x10, 1, 0); // vertical bin
		OV5658_WriteBuf(ptInfo, _3821_TimingFormat2, 0x1E, 1, 0); // hori. bin
		OV5658_WriteBuf(ptInfo, _4501_InputSwapManEn, 0x00, 1, 0); // sum or avergae for bin

		OV5658_WriteBuf(ptInfo, _3808_TimingXOutputSize1, 0x09, 1, 0);
		OV5658_WriteBuf(ptInfo, _3809_TimingXOutputSize0, 0x88, 1, 0);
		OV5658_WriteBuf(ptInfo, _380A_TimingYOutputSize1, 0x06, 1, 0);
		OV5658_WriteBuf(ptInfo, _380B_TimingYOutputSize0, 0xC8, 1, 0);

		OV5658_WriteBuf(ptInfo, _3800_TimingXAddrStart1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3801_TimingXAddrStart0, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3802_TimingYAddrStart1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3803_TimingYAddrStart0, 0x01, 1, 0);

		OV5658_WriteBuf(ptInfo, _3804_TimingXAddrEnd1, 0x0A, 1, 0);
		OV5658_WriteBuf(ptInfo, _3805_TimingXAddrEnd0, 0x3F, 1, 0);
		OV5658_WriteBuf(ptInfo, _3806_TimingYAddrEnd1, 0x07, 1, 0);
		OV5658_WriteBuf(ptInfo, _3807_TimingYAddrEnd0, 0xA3, 1, 0);

		OV5658_WriteBuf(ptInfo, _3810_TimingIspXWin1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3811_TimingIspXWin0, 0x10, 1, 0);
		OV5658_WriteBuf(ptInfo, _3812_TimingIspYWin1, 0x00, 1, 0);
		OV5658_WriteBuf(ptInfo, _3813_TimingIspYWin0, 0x05, 1, 0);

		dwOV5658Vsync = OV5658_VSYNC_2432x1728;
	}			

	else	
	{
		return -1;			
	}

	ptInfo->dwWidth = dwWidth;
	ptInfo->dwHeight = dwHeight;		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
SOCKET OV5658_SetFrequence(void)
{
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_50HZ)
	{
		if ((ptInfo->dwWidth==2560) && (ptInfo->dwHeight==1920))
		{
			dwOV5658Hsync = OV5658_HSYNC_2560x1920_25FPS;
		}
		else if ((ptInfo->dwWidth==2432) && (ptInfo->dwHeight==1728))
		{
			dwOV5658Hsync = OV5658_HSYNC_2432x1728_25FPS;
		}			
	}
	else
	{
		if ((ptInfo->dwWidth==2560) && (ptInfo->dwHeight==1920))
		{
			dwOV5658Hsync = OV5658_HSYNC_2560x1920_30FPS;
		}
		else if ((ptInfo->dwWidth==2432) && (ptInfo->dwHeight==1728))
		{
			dwOV5658Hsync = OV5658_HSYNC_2432x1728_30FPS;
		}			
	}

	OV5658_WriteBuf(ptInfo, _380C_TimingHTS1, (BYTE)((dwOV5658Hsync>>8)&0xFF), 1, 0);
	OV5658_WriteBuf(ptInfo, _380D_TimingHTS0, (BYTE)(dwOV5658Hsync&0xFF), 1, 0);

	OV5658_WriteBuf(ptInfo, _380E_TimingVTS1, (BYTE)((dwOV5658Vsync>>8)&0xFF), 1, 0);
	OV5658_WriteBuf(ptInfo, _380F_TimingVTS0, (BYTE)(dwOV5658Vsync&0xFF), 1, 0);

	return 0;
}

/*-------------------------------------------------------------------------------------*/
void OV5658_GroupAccess(SOCKET sckStatus)
{
	down_interruptible(&OV5658_mutex);

	if (sckStatus == 0)
	{
		// initiate group access
		OV5658_WriteBuf(ptInfo, _3208_GroupAccess, 0x00, 1, 0);		// Group hold start
	}
	else { // sckStatus == 1
		// launch group access
		OV5658_WriteBuf(ptInfo, _3208_GroupAccess, 0x10, 1, 0);		// Group hold end
		OV5658_WriteBuf(ptInfo, _3208_GroupAccess, 0xA0, 1, 0);		// Group launch at vertical blank
	}
	up(&OV5658_mutex);	
}

/*-------------------------------------------------------------------------------------*/
SOCKET  OV5658_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET sckRet = 0;
	DWORD dwData, dwRgain, dwGgain, dwBgain;

	if (down_interruptible(&OV5658_mutex))
	{
		return -ERESTARTSYS;	
	}

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		OV5658_Reset(ptInfo);
		// Reset captured size
		if (OV5658_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
		{
			printk("[OV5658] : Reset width/height error !!\n");			
			sckRet =  -EPERM;
		}
		else
		{
			OV5658_SetFrequence();
		}
		break;
		case VIDEO_SIGNAL_OPTION_SET_SIZE:
		if (OV5658_SetSize(ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]) < 0)
		{
			sckRet = -EPERM;
		}
		else
		{
			OV5658_SetFrequence();
		}
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		OV5658_SetShutter(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		OV5658_SetGain(ptIoctlArg->adwUserData[0]);				
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[OV5658] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			sckRet = -EPERM;
			break;
		}
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
		OV5658_SetFrequence();
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
		if (ptInfo->bEnOV5658AWB)
		{
			switch (ptIoctlArg->adwUserData[0])
			{
				case 0:	// auto WB
				OV5658_UpdateBuf(ptInfo, _3406_AwbManualCtrl, 0xFE, 0x00);
				break;
				case 1:	// manual WB
				OV5658_UpdateBuf(ptInfo, _3406_AwbManualCtrl, 0xFE, 0x01);
				dwRgain = (ptIoctlArg->adwUserData[1] & 0xFF);
				dwBgain = (ptIoctlArg->adwUserData[2] & 0xFF);
				dwGgain = ((ptIoctlArg->adwUserData[1]&0xFF00) >> 8);

				dwRgain = dwRgain * (OV5658_WB_GAIN_UNIT/64);
				dwBgain = dwBgain * (OV5658_WB_GAIN_UNIT/64);
				dwGgain = dwGgain * (OV5658_WB_GAIN_UNIT/64);

				dwData = (dwRgain >> 8);
				OV5658_WriteBuf(ptInfo, _3400_AwbRedGain1, dwData, 1, 0);
				dwData = (dwRgain & 0x0FF);
				OV5658_WriteBuf(ptInfo, _3401_AwbRedGain0, dwData, 1, 0);
				dwData = (dwGgain >> 8);
				OV5658_WriteBuf(ptInfo, _3402_AwbGrnGain1, dwData, 1, 0);
				dwData = (dwGgain & 0x0FF);
				OV5658_WriteBuf(ptInfo, _3403_AwbGrnGain0, dwData, 1, 0);
				dwData = (dwBgain >> 8);
				OV5658_WriteBuf(ptInfo, _3404_AwbBluGain1, dwData, 1, 0);
				dwData = (dwBgain & 0x0FF);
				OV5658_WriteBuf(ptInfo, _3405_AwbBluGain0, dwData, 1, 0);
				break;
				default:
				OV5658_ReadCompGain(ptInfo, _3400_AwbRedGain1, _3401_AwbRedGain0, &dwBgain);
				OV5658_ReadCompGain(ptInfo, _3402_AwbGrnGain1, _3403_AwbGrnGain0, &dwRgain);
				OV5658_ReadCompGain(ptInfo, _3404_AwbBluGain1, _3405_AwbBluGain0, &dwGgain);
				dwBgain = (dwBgain*64) / OV5658_WB_GAIN_UNIT;
				dwRgain = (dwRgain*64) / OV5658_WB_GAIN_UNIT;
				dwGgain = (dwGgain*64) / OV5658_WB_GAIN_UNIT;
					
				ptIoctlArg->adwUserData[1] = ((dwGgain<<8) | dwRgain);
				ptIoctlArg->adwUserData[2] = dwBgain;

				OV5658_UpdateBuf(ptInfo, _3406_AwbManualCtrl, 0xFE, 0x01);
				break;
			}
		}
		sckRet = -1;
		break;
		default:
		sckRet = -EPERM;
	}
	up(&OV5658_mutex);	

	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
void OV5658_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET OV5658_Open(TVideoSensorInitialParam* ptOV5658InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptOV5658InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(OV5658_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptOV5658InitialParam->dwVideoSensorVersion&0x0000FF00)>(OV5658_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid OV5658 device driver version %d.%d.%d.%d !!\n", 
				(int)(OV5658_INTERFACE_VERSION&0xFF), 
				(int)((OV5658_INTERFACE_VERSION>>8)&0xFF), 
				(int)((OV5658_INTERFACE_VERSION>>16)&0xFF), 
				(int)(OV5658_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TOV5658Info *)kmalloc(sizeof(TOV5658Info), GFP_KERNEL)))
	{
		printk("[OV5658] : Allocate %d bytes memory fail\n", sizeof(TOV5658Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[OV5658] : Initial IICCtrl object fail !!\n");
		OV5658_Release(dwDevNum);
		return -ENODEV;
	}
	
	// init private data
	ptInfo->dwDeviceAddr = OV5658_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptOV5658InitialParam->dwVideoSensorFrequency;
	ptInfo->bEnOV5658AWB = iEnAWB;
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;

	// Detect device
	if (!OV5658_CompBuf(ptInfo, _300A_SCChipID1, 0xFF, ((OV5658_PID>>8)&0xFF)))
	{
		printk("[OV5658] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
		OV5658_Release(dwDevNum);	
		return -ENODEV;
	}
	if (!OV5658_CompBuf(ptInfo, _300B_SCChipID0, 0xFF, (OV5658_PID&0xFF))) 
	{
		printk("[OV5658] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
		OV5658_Release(dwDevNum);				
		return -ENODEV;
	}
	// Reset device
	OV5658_Reset(ptInfo);

	// init captured size
	if (OV5658_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
	{
		printk("[OV5658] : initial width/height error !!\n");			
		OV5658_Release(dwDevNum);				
		return -EPERM;
	}
	OV5658_SetFrequence();

	return 0;
}

/*-------------------------------------------------------------------------------------*/
DWORD OV5658_GetMaxGain(void)
{
	return (OV5658_MAX_GAIN);
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	OV5658_Open,
	.release = 	OV5658_Release,
	.ioctl = 	OV5658_Ioctl,
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = OV5658_GetMaxGain,
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct OV5658_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t OV5658_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct OV5658_attribute *OV5658_attr = container_of(attr, struct OV5658_attribute, attr);
	int result = 0;
	
	if (OV5658_attr->show)
		result = OV5658_attr->show(kobj, buf);	
	return result;
}

ssize_t OV5658_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "OV5658");
	return retval;
}

ssize_t OV5658_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t OV5658_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "2560");
	return retval;
}

ssize_t OV5658_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject OV5658_kobj;

struct OV5658_attribute OV5658_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5658_SensorName_attr_show,
};

struct OV5658_attribute OV5658_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5658_dwBufNum_attr_show,
};

struct OV5658_attribute OV5658_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5658_MaxFrameWidth_attr_show,
};

struct OV5658_attribute OV5658_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5658_MaxFrameHeight_attr_show,
};

struct sysfs_ops OV5658_sysfs_ops = {
	.show	= OV5658_default_attr_show,
};

struct kobj_type OV5658_ktype = {
	.sysfs_ops	= &OV5658_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET OV5658_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[OV5658] : kobj set name fail!\n");
		return -1;
	}

	printk("[OV5658] : init...video_sensor_width=%d video_sensor_height=%d\n",video_sensor_width,video_sensor_height);

	OV5658_kobj.ktype = &OV5658_ktype;	
	sckResult = kobject_init_and_add(&OV5658_kobj, &OV5658_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[OV5658] : Cannot register kobject [OV5658_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV5658_kobj, &OV5658_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5658] : Cannot create OV5658_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&OV5658_kobj, &OV5658_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5658] : Cannot create OV5658_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV5658_kobj, &OV5658_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5658] : Cannot create OV5658_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV5658_kobj, &OV5658_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5658] : Cannot create OV5658_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&OV5658_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void OV5658_Exit(void)
{
	sysfs_remove_file(&OV5658_kobj, &(OV5658_SensorName_attr.attr));
	sysfs_remove_file(&OV5658_kobj, &(OV5658_dwBufNum_attr.attr));	
	sysfs_remove_file(&OV5658_kobj, &(OV5658_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&OV5658_kobj, &(OV5658_MaxFrameHeight_attr.attr));		
	kobject_put(&OV5658_kobj);	
	printk(KERN_DEBUG"[OV5658] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(OV5658_Init);
module_exit(OV5658_Exit);
