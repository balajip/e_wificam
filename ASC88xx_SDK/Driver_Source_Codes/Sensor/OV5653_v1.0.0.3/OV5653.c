/*
 * OV5653
 * Driver for OV5653 sensor.
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

#include "OV5653.h"
#include "OV5653_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h>

const CHAR OV5653_ID[] = "$Version: "OV5653_ID_VERSION"  (OV5653 DRIVER) $";

/*==================================================================*/
static TOV5653Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore OV5653_mutex;

static DWORD dwOV5653Hsync = OV5653_HSYNC_1920x1080_30FPS;
static DWORD dwOV5653Vsync = OV5653_VSYNC_1920x1080;

/* Sensor capture width */
static SOCKET video_sensor_width = 1920;
/* Sensor capture height */
static SOCKET video_sensor_height = 1080;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "OV5653 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "OV5653 capture height");

/* Version 1.0.0.2 modification, 2012.08.06 */
static int iEnAWB=0;
module_param(iEnAWB, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnAWB, "Enable sensor AWB");
/* ======================================== */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("OV5653 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TOV5653RegAddrData atOV5653regTbl[OV5653_TBL_SIZE] = 
{
	{_3008_SystemControl   ,0x42},
	{_3103_PLLClockSelect  ,0x93},
	{_3b07_StrobeModeSelect,0x0c},
	{_3017_PADOutputEnable0,0xff},
	{_3018_PADOutputEnable1,0xfc},
	{_3706_                ,0x41},
	{_3630_                ,0x22},
	{_3605_                ,0x4 },
	{_3606_                ,0x3f},
	{_3712_AnalogControl12 ,0x13},
	{_370e_AnalogControl0E ,0x0 },
	{_370b_AnalogControl0B ,0x40},
	{_3600_AnalogControl00 ,0x54},
	{_3601_AnalogControl01 ,0x5 },
	{_3631_AnalogControl31 ,0x22},
	{_3612_AnalogControl12 ,0x1a},
	{_3604_AnalogControl04 ,0x40},
	{_3710_AnalogControl10 ,0x28},
	{_3702_AnalogControl02 ,0x3a},
	{_3704_AnalogControl04 ,0x18},
	{_3a18_AECDebugMode    ,0x0 },
	{_3a19_AECGainCeiling  ,0xf8},
	{_3a00_AECMode         ,0x38},
	{_3830_AGCManual       ,0x50},
	{_3a08_AECB50Step1     ,0x12},
	{_3a09_AECB50Step0     ,0x70},
	{_3a0a_AECB60Step1     ,0x0f},
	{_3a0b_AECB60Step0     ,0x60},
	{_3a0d_AECB60Max       ,0x6 },
	{_3a0e_AECB50Max       ,0x6 },
	{_3a13_AECPreGain      ,0x54},
	{_3815_TimmingTCReg15,0x82},
	{_5059_                ,0x80},
	{_3615_                ,0x52},
	{_3a08_AECB50Step1     ,0x16},
	{_3a09_AECB50Step0     ,0x48},
	{_3a0a_AECB60Step1     ,0x12},
	{_3a0b_AECB60Step0     ,0x90},
	{_3a0d_AECB60Max       ,0x3 },
	{_3a0e_AECB50Max       ,0x3 },
	{_3a1a_AECDiffMax      ,0x6 },
	{_3623_AnalogControl23 ,0x1 },
	{_3633_AnalogControl33 ,0x24},
	{_3c01_5060HzCtrl01    ,0x34},
	{_3c04_5060HzSumLowThr ,0x28},
	{_3c05_5060HzSumHighThr,0x98},
	{_3c07_5060HzSumLM1Thr ,0x7 },
	{_3c09_5060HzSumLM2Thr ,0xc2},
	{_4000_BLCCtrl00       ,0x5 },
	{_401d_BLCCtrl1D       ,0x28},
	{_4001_BLCCtrl01       ,0x2 },
	{_401c_BLCCtrl1C       ,0x46},
	{_5046_ISPCtrl46       ,0x9 },
	{_3810_TimmingHVOffs   ,0x40},
	{_3836_TimmingHVPad    ,0x41},
	{_505f_LENCCtrl5F      ,0x4 },
	{_5000_ISPCtrl00       ,0x2 }, //0x0 enable White DPC
	{_5001_ISPCtrl01       ,0x1 },
	{_5181_AWBControl01,        0x20}, 
	{_503d_ISPCtrl3d       ,0x0 },
	{_585a_LencBRHscale1   ,0x1 },
	{_585b_LencBRHscale0   ,0x2c},
	{_585c_LencBRVscale1   ,0x1 },
	{_585d_LencBRVscale0   ,0x93},
	{_585e_LencGHscale1    ,0x1 },
	{_585f_LencGHscale0    ,0x90},
	{_5860_LencGVscale1    ,0x1 },
	{_5861_LencGVscale0    ,0x0d},
	{_5180_AWBFastAwb      ,0xc0},
	{_5184_AWBFrameCtrl    ,0x00 },
	{_470a_DVPBypCtrl1     ,0x0 },
	{_470b_DVPBypCtrl0     ,0x0 },
	{_470c_DVPBypSel       ,0x0 },
	{_300f_Pllctrl00       ,0x8e},
	{_3603_                ,0xa7},
	{_3632_AnalogControl32 ,0x55},
	{_3620_AnalogControl20 ,0x56},
	{_3631_AnalogControl31 ,0x36},
	{_3632_AnalogControl32 ,0x5f},
	{_3711_AnalogControl11 ,0x24},
	{_401f_                ,0x3 },
	{_3010_Pllctrl01       ,0x10},
	{_3011_Pllctrl02       ,0x10},
	{_3406_AWBManual       ,0x1 },
	{_3400_AWBRGain1       ,0x4 },
	{_3401_AWBRGain0       ,0x0 },
	{_3402_AWBGGain1       ,0x4 },
	{_3403_AWBGGain0       ,0x0 },
	{_3404_AWBBGain1       ,0x4 },
	{_3405_AWBBGain0       ,0x0 },
	{_3503_AEAGManual      ,0x07}, //0x17  No Latch delay Enable Manual AGC&AEC
	{_3500_AECLongExpo2    ,0x0 },
	{_3501_AECLongExpo1    ,0x18},
	{_3502_AECLongExpo0    ,0x0 },
	{_350a_AGCAdj1        ,0x0 },
	{_350b_AGCAdj0          ,0x38},
	{_4704_DVPVsyncMode    ,0x0 },
	{_4708_DVPPolCtrl      ,0x3 },
	{_3008_SystemControl   ,0x2 },
	{_302C_PADOutputDrv   ,0x22 }, // change pad driving from x1 to x2 to make PCLK rise to 3.3V
	{_4006_BLCLongTarget1   ,0x0 },
	{_4007_BLCLongTarget0   ,0x10 },//BLC targer to 0x10
	{_3030_PowerCtrl   ,0x20 }, // bypass internal DVDD regulator

};

EOV5653Regs aeOV5653CaptureSizeReg[40] = 
{
	_3800_TimmingHS1        	,
	_3801_TimmingHS0        	,
	_3803_TimmingVS0        	,
	_3804_TimmingHW1        	,
	_3805_TimmingHW0        	,
	_3806_TimmingVH1        	,
	_3807_TimmingVH0        	,
	_3808_TimmingHO1        	,
	_3809_TimmingHO0        	,
	_380A_TimmingVO1        	,
	_380B_TimmingVO0        	,
	_380E_TimmingVTS1       	,
	_380F_TimmingVTS0       	,
	_3815_TimmingTCReg15    	,
	_3818_TimmingTCReg18    	,
	_381A_TimmingTCHSMirrAdj	,
	_381C_TimmingTCReg1C    	,
	_381D_TimmingTCVsCropL  	,
	_381E_TimmingTCVHCrop1  	,
	_381F_TimmingTCVHCrop0  	,
	_3820_TimmingTCReg20    	,
	_3821_TimmingTCReg21    	,
	_3824_TimmingHRefstMan1 	,
	_3825_TimmingHRefstMan0 	,
	_3613_AnalogControl13   	,
	_3621_ArrayCtrl01       	,
	_3703_AnalogControl03   	,
	_3705_AnalogControl05   	,
	_370A_AnalogControl0A   	,
	_370C_AnalogControl0C   	,
	_370D_AnalogControl0D   	,
	_3713_AnalogControl13   	,
	_3714_AnalogControl14   	,
	_401c_BLCCtrl1C           	,
	_5002_ISPCtrl02         	,
	_505A_LencOffxMan1      	,
	_505B_LencOffxMan0      	,
	_5901_VAPCtrl01         	,
	_3010_Pllctrl01         	,
	_3011_Pllctrl02         	,
};

const BYTE abyOV5653CaptureSizeData[40][8] =
{
/* Version 1.0.0.2 modification, 2012.08.06 */
	{	0x02,	0x02,	0x02,	0x02,	0x02,	0x02,	0x02,	0x02,	},
	{	0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	},
	{	0x0C,	0x0C,	0x0C,	0x0C,	0x0C,	0x0C,	0x0A,	0x0A,	},
	{	0x0A,	0x07,	0x06,	0x05,	0x05,	0x03,	0x05,	0x02,	},
	{	0x00,	0x80,	0x40,	0x00,	0x00,	0x20,	0x00,	0x80,	},
	{	0x07,	0x04,	0x04,	0x04,	0x02,	0x02,	0x01,	0x00,	},
	{	0x80,	0x38,	0xB0,	0x00,	0xD0,	0x58,	0xE0,	0xF0,	},
	{	0x0A,	0x07,	0x06,	0x05,	0x05,	0x03,	0x02,	0x01,	},
	{	0x00,	0x80,	0x40,	0x00,	0x00,	0x20,	0x80,	0x40,	},
	{	0x07,	0x04,	0x04,	0x04,	0x02,	0x02,	0x01,	0x00,	},
	{	0x80,	0x38,	0xB0,	0x00,	0xD0,	0x58,	0xE0,	0xF0,	},
	{	0x07,	0x04,	0x04,	0x04,	0x02,	0x02,	0x01,	0x01,	},
	{	0x90,	0x48,	0xC0,	0x10,	0xE0,	0x68,	0xF0,	0x00,	},
	{	0x82,	0x82,	0x82,	0x82,	0x81,	0x81,	0x81,	0x81,	},
	{	0xC0,	0xC0,	0xC0,	0xC0,	0xC1,	0xC1,	0xC2,	0xC2,	},
	{	0x1C,	0x1C,	0x1C,	0x1C,	0x00,	0x00,	0x00,	0x00,	},
	{	0x30,	0x31,	0x31,	0x31,	0x31,	0x31,	0x30,	0x31,	},

	{	0x1F,	0xC3,	0x87,	0xDF,	0x03,	0x7B,	0x13,	0xF3,	},

	{	0x07,	0x04,	0x04,	0x04,	0x05,	0x04,	0x07,	0x03,	},
	{	0xA0,	0x58,	0xD0,	0x20,	0xC0,	0xD0,	0xA0,	0xE0,	},
	{	0x00,	0x05,	0x07,	0x09,	0x00,	0x07,	0x00,	0x09,	},
	{	0x20,	0x18,	0x14,	0x10,	0x20,	0x14,	0x20,	0x10,	},
	{	0x22,	0x22,	0x22,	0x22,	0x23,	0x23,	0x23,	0x23,	},
	{	0x64,	0x94,	0x94,	0x94,	0x2A,	0x42,	0x2A,	0x42,	},
	{	0x44,	0x44,	0x44,	0x44,	0xC4,	0xC4,	0xC4,	0xC4,	},
	{	0x2F,	0x2F,	0x2F,	0x2F,	0xAF,	0xAF,	0xAF,	0xAF,	},
	{	0xE6,	0xE6,	0xE6,	0xE6,	0x9A,	0x9A,	0x9A,	0x9A,	},
	{	0xDA,	0xDA,	0xDA,	0xDA,	0xDB,	0xDB,	0xDC,	0xDC,	},
	{	0x80,	0x80,	0x80,	0x80,	0x81,	0x81,	0x81,	0x81,	},
	{	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xC8,	0xC8,	},
	{	0x04,	0x04,	0x04,	0x04,	0x42,	0x42,	0x42,	0x42,	},
	{	0x22,	0x22,	0x22,	0x22,	0x92,	0x92,	0x92,	0x92,	},
	{	0x27,	0x27,	0x27,	0x27,	0x17,	0x17,	0x17,	0x17,	},
	{	0x46,	0x46,	0x46,	0x46,	0x42,	0x42,	0x42,	0x42,	},
	{	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x02,	0x02,	},
	{	0x0A,	0x0A,	0x0A,	0x0A,	0x0A,	0x0A,	0x0A,	0x0A,	},
	{	0x2E,	0x2E,	0x2E,	0x2E,	0x2E,	0x2E,	0x2E,	0x2E,	},
	{	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x04,	0x04,	},
	{	0x10,	0x10,	0x10,	0x10,	0x10,	0x10,	0x00,	0x00,	},
	{	0x10,	0x10,	0x10,	0x10,	0x10,	0x10,	0x10,	0x10,	},
/* ======================================== */
};

/*==================================================================*/
static SOCKET OV5653_WriteBuf(TOV5653Info *ptInfo, EOV5653Regs eRegAddr, BYTE byData)
{
	BYTE abyData[2];

	abyData[0] = (BYTE)(eRegAddr&0xFF);
	abyData[1] = byData;
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), 2, abyData);
}

/*-------------------------------------------------------------------------------------*/ 
static SOCKET OV5653_ReadBuf(TOV5653Info *ptInfo, EOV5653Regs eRegAddr, BYTE *pbyData)
{
	*pbyData = (BYTE)(eRegAddr&0xFF);
	return ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, ((eRegAddr&0xFF00)>>8), 1, pbyData);	
}
 
/*-------------------------------------------------------------------------------------*/
static SOCKET OV5653_CompBuf(TOV5653Info *ptInfo, EOV5653Regs eRegAddr, BYTE byMask, BYTE byData)
{
	BYTE byReadData;

	OV5653_ReadBuf(ptInfo, eRegAddr, &byReadData);
	return ((byReadData&byMask) == byData);
}

/*-------------------------------------------------------------------------------------*/
static SOCKET OV5653_UpdateBuf(TOV5653Info *ptInfo, EOV5653Regs eRegAddr, BYTE byMask, BYTE byData)
{
	BYTE byReadData, byWriteData;

	if (OV5653_ReadBuf(ptInfo, eRegAddr, &byReadData) < 0)
	{
		return -1;
	}
	byWriteData = ((byReadData&byMask) | byData);
	if (OV5653_WriteBuf(ptInfo, eRegAddr, byWriteData) < 0)
	{
		return -1;
	}
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void OV5653_ReadCompGain(TOV5653Info *ptInfo, EOV5653Regs eRegAddrH, EOV5653Regs eRegAddrL, DWORD *pdwData)
{
	BYTE byData;

	OV5653_ReadBuf(ptInfo, eRegAddrH, &byData) ;
	*pdwData = ((byData&0x0F) << 8);
	OV5653_ReadBuf(ptInfo, eRegAddrL, &byData) ;
	*pdwData = *pdwData + byData;
}

/*-------------------------------------------------------------------------------------*/
static void OV5653_Reset(TOV5653Info *ptInfo)
{
	DWORD dwIndex;

	OV5653_WriteBuf(ptInfo, _3008_SystemControl, 0x82);
	mdelay(1000);
	while (OV5653_CompBuf(ptInfo, _3008_SystemControl, 0x80, 0x00) != 1);

	for (dwIndex=0; dwIndex<OV5653_TBL_SIZE; dwIndex++)
	{
		OV5653_WriteBuf(ptInfo, atOV5653regTbl[dwIndex].eRegAddr, atOV5653regTbl[dwIndex].byData);
	}

/* Version 1.0.0.2 modification, 2012.08.06 */
	if (!ptInfo->bOV5653DSPEnable)
	{
		OV5653_WriteBuf(ptInfo, _5001_ISPCtrl01, 0x00);
	}
/* ======================================== */
}

/*-------------------------------------------------------------------------------------*/
void OV5653_SetShutter(DWORD dwShutter)
{
	DWORD dwData;

/* Version 1.0.0.1 modification, 2011.09.07 */
	dwData = (dwShutter*16*OV5653_PIXEL_CLOCK_M) / dwOV5653Hsync;
/* ======================================== */
	if (dwData > 0x000FFFFF)
	{
		dwData = 0x000FFFFF;
	}
	if (dwData <= 64)
	{
		dwData = 64;
	}

	OV5653_UpdateBuf(ptInfo, _3500_AECLongExpo2, 0xF0, (BYTE)((dwData>>16)&0x0F)) ;
	OV5653_WriteBuf(ptInfo, _3501_AECLongExpo1, (BYTE)((dwData>>8)&0xFF)) ;
	OV5653_WriteBuf(ptInfo, _3502_AECLongExpo0, (BYTE)(dwData&0xF0)) ;

	dwData = (dwData/16/dwOV5653Vsync*dwOV5653Vsync) + 6;
	OV5653_WriteBuf(ptInfo, _350C_AECVTSDiff1, (BYTE)((dwData>>8)&0xFF)) ;
	OV5653_WriteBuf(ptInfo, _350D_AECVTSDiff0, (BYTE)(dwData&0xFF)) ;
}

/*-------------------------------------------------------------------------------------*/
void OV5653_SetGain(DWORD dwGain)
{
	DWORD dwData;

/* Version 1.0.0.2 modification, 2012.08.06 */
	if (dwGain > OV5653_MAX_GAIN)
	{
		dwGain = OV5653_MAX_GAIN;
/* ======================================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	if (dwGain < 2000)
	{
		dwData = (dwGain*16/1000) - 16;
	}
	else if (dwGain < 4000)
	{
		dwData = (dwGain*8/1000-16) + 16;
	}
	else if (dwGain < 8000)
	{
		dwData = (dwGain*4/1000-16) + 48;
	}
	else if (dwGain < 16000)
	{
		dwData = (dwGain*2/1000-16) + 112;
	}
	else if (dwGain < 32000)
	{
		dwData = (dwGain/1000-16) + 240;
	}
	else if (dwGain < 64000)
	{
		dwData = (dwGain/1000/2-16) + 496;
	}
	else
	{
		dwData = (dwGain/1000/4-16) + 511;
	}

	//OV5653_UpdateBuf(ptInfo, _350a_AGCAdj1, 0xFE, ((dwData>>8)&0x01));//Suggest do not use
	OV5653_WriteBuf(ptInfo, _350b_AGCAdj0, (dwData&0xFF));
}

/*-------------------------------------------------------------------------------------*/
SOCKET OV5653_SetSize(DWORD dwWidth, DWORD dwHeight)
{
	DWORD dwSizeType = 1, dwIndex = 0;

	if ((dwWidth==2560) && (dwHeight==1920))
	{
		dwSizeType = 0;
		dwOV5653Vsync = OV5653_VSYNC_2560x1920;
	}
	else if ((dwWidth==1920) && (dwHeight==1080))
	{
		dwSizeType = 1;
		dwOV5653Vsync = OV5653_VSYNC_1920x1080;
	}			
	else if ((dwWidth==1600) && (dwHeight==1200))
	{
		dwSizeType = 2;
		dwOV5653Vsync = OV5653_VSYNC_1600x1200;
	}
	else if ((dwWidth==1280) && (dwHeight==1024))
	{
		dwSizeType = 3;
		dwOV5653Vsync = OV5653_VSYNC_1280x1024;
	}
	else if ((dwWidth==1280) && (dwHeight==720))
	{
		dwSizeType = 4;
		dwOV5653Vsync = OV5653_VSYNC_1280x720;
	}
	else if ((dwWidth==800) && (dwHeight==600))
	{
		dwSizeType = 5;
		dwOV5653Vsync = OV5653_VSYNC_800x600;
	}
	else if ((dwWidth==640) && (dwHeight==480))
	{
		dwSizeType = 6;
		dwOV5653Vsync = OV5653_VSYNC_640x480;
	}
	else if ((dwWidth==320) && (dwHeight==240))
	{
		dwSizeType = 7;
		dwOV5653Vsync = OV5653_VSYNC_320x240;
	}
	else	
	{
		return -1;			
	}

	for (dwIndex=0; dwIndex<40; dwIndex++)
	{
		OV5653_WriteBuf(ptInfo, aeOV5653CaptureSizeReg[dwIndex], abyOV5653CaptureSizeData[dwIndex][dwSizeType]);
	}

	ptInfo->dwWidth = dwWidth;
	ptInfo->dwHeight = dwHeight;		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
SOCKET OV5653_SetFrequence(void)
{
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_50HZ)
	{
/* Version 1.0.0.1 modification, 2011.09.07 */
		if ((ptInfo->dwWidth==2560) && (ptInfo->dwHeight==1920))
		{
			dwOV5653Hsync = OV5653_HSYNC_2560x1920_15FPS;
		}
		else if ((ptInfo->dwWidth==1920) && (ptInfo->dwHeight==1080))
		{
			dwOV5653Hsync = OV5653_HSYNC_1920x1080_25FPS;
		}			
		else if ((ptInfo->dwWidth==1600) && (ptInfo->dwHeight==1200))
		{
			dwOV5653Hsync = OV5653_HSYNC_1600x1200_25FPS;
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==1024))
		{
			dwOV5653Hsync = OV5653_HSYNC_1280x1024_25FPS;
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
		{
			dwOV5653Hsync = OV5653_HSYNC_1280x720_25FPS;
		}
		else if ((ptInfo->dwWidth==800) && (ptInfo->dwHeight==600))
		{
			dwOV5653Hsync = OV5653_HSYNC_800x600_25FPS;
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
		{
			dwOV5653Hsync = OV5653_HSYNC_640x480_50FPS;
		}
		else if ((ptInfo->dwWidth==320) && (ptInfo->dwHeight==240))
		{
			dwOV5653Hsync = OV5653_HSYNC_320x240_100FPS;
		}
	}
	else
	{
		if ((ptInfo->dwWidth==2560) && (ptInfo->dwHeight==1920))
		{
			dwOV5653Hsync = OV5653_HSYNC_2560x1920_15FPS;
		}
		else if ((ptInfo->dwWidth==1920) && (ptInfo->dwHeight==1080))
		{
			dwOV5653Hsync = OV5653_HSYNC_1920x1080_30FPS;
		}			
		else if ((ptInfo->dwWidth==1600) && (ptInfo->dwHeight==1200))
		{
			dwOV5653Hsync = OV5653_HSYNC_1600x1200_30FPS;
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==1024))
		{
			dwOV5653Hsync = OV5653_HSYNC_1280x1024_30FPS;
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
		{
			dwOV5653Hsync = OV5653_HSYNC_1280x720_30FPS;
		}
		else if ((ptInfo->dwWidth==800) && (ptInfo->dwHeight==600))
		{
			dwOV5653Hsync = OV5653_HSYNC_800x600_30FPS;
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
		{
			dwOV5653Hsync = OV5653_HSYNC_640x480_60FPS;
		}
		else if ((ptInfo->dwWidth==320) && (ptInfo->dwHeight==240))
		{
			dwOV5653Hsync = OV5653_HSYNC_320x240_120FPS;
		}
	}
	OV5653_WriteBuf(ptInfo, _380C_TimmingHTS1, (BYTE)((dwOV5653Hsync>>8)&0xFF));
	OV5653_WriteBuf(ptInfo, _380D_TimmingHTS0, (BYTE)(dwOV5653Hsync&0xFF));
/* ======================================== */
	return 0;
}

/*-------------------------------------------------------------------------------------*/
void OV5653_GroupAccess(SOCKET sckStatus)
{
	down_interruptible(&OV5653_mutex);

	if (sckStatus == 0)
	{
		// initiate group access
		OV5653_WriteBuf(ptInfo, _3212_GroupAccess, 0x00);		// enable group 0
	}
	else { // sckStatus == 1
		// launch group access
		OV5653_WriteBuf(ptInfo, _3212_GroupAccess, 0x10);		// end group 0
		OV5653_WriteBuf(ptInfo, _3212_GroupAccess, 0xA0);		// launch group 0
	}
	up(&OV5653_mutex);	
}

/*-------------------------------------------------------------------------------------*/
SOCKET  OV5653_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET sckRet = 0;
	DWORD dwData, dwRgain, dwGgain, dwBgain;

	if (down_interruptible(&OV5653_mutex))
	{
		return -ERESTARTSYS;	
	}

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		OV5653_Reset(ptInfo);
		// Reset captured size
		if (OV5653_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
		{
			printk("[OV5653] : Reset width/height error !!\n");			
			sckRet =  -EPERM;
		}
		else
		{
			OV5653_SetFrequence();
		}
		break;
		case VIDEO_SIGNAL_OPTION_SET_SIZE:
		if (OV5653_SetSize(ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]) < 0)
		{
			printk("[OV5653] : Size is not supported (width=%ld, height=%ld).\n", ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]);			
			sckRet = -EPERM;
		}
		else
		{
			OV5653_SetFrequence();
		}
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		OV5653_SetShutter(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		OV5653_SetGain(ptIoctlArg->adwUserData[0]);				
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			ptInfo->dwFreq = VIDEO_SIGNAL_FREQUENCY_60HZ;
		}
		else 
		{
			ptInfo->dwFreq = VIDEO_SIGNAL_FREQUENCY_50HZ;
		}
		OV5653_SetFrequence();
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
/* Version 1.0.0.2 modification, 2012.08.06 */
		if (ptInfo->bOV5653DSPEnable)
		{
/* ======================================== */
			switch (ptIoctlArg->adwUserData[0])
			{
				case 0:	// auto WB
				OV5653_UpdateBuf(ptInfo, _3406_AWBManual, 0xFE, 0x00);
				break;
				case 1:	// manual WB
				OV5653_UpdateBuf(ptInfo, _3406_AWBManual, 0xFE, 0x01);
				OV5653_WriteBuf(ptInfo, _3406_AWBManual, 0x01);
				dwRgain = (ptIoctlArg->adwUserData[1] & 0xFF);
				dwBgain = (ptIoctlArg->adwUserData[2] & 0xFF);
				dwGgain = ((ptIoctlArg->adwUserData[1]&0xFF00) >> 8);

				dwRgain = dwRgain * (OV5653_WB_GAIN_UNIT/64);
				dwBgain = dwBgain * (OV5653_WB_GAIN_UNIT/64);
				dwGgain = dwGgain * (OV5653_WB_GAIN_UNIT/64);

				dwData = (dwRgain >> 8);
				OV5653_WriteBuf(ptInfo, _3400_AWBRGain1, dwData);
				dwData = (dwRgain & 0x0FF);
				OV5653_WriteBuf(ptInfo, _3401_AWBRGain0, dwData);
				dwData = (dwGgain >> 8);
				OV5653_WriteBuf(ptInfo, _3402_AWBGGain1, dwData);
				dwData = (dwGgain & 0x0FF);
				OV5653_WriteBuf(ptInfo, _3403_AWBGGain0, dwData);
				dwData = (dwBgain >> 8);
				OV5653_WriteBuf(ptInfo, _3404_AWBBGain1, dwData);
				dwData = (dwBgain & 0x0FF);
				OV5653_WriteBuf(ptInfo, _3405_AWBBGain0, dwData);
				break;
				default:
				OV5653_ReadCompGain(ptInfo, _3400_AWBRGain1, _3401_AWBRGain0, &dwBgain);
				OV5653_ReadCompGain(ptInfo, _3402_AWBGGain1, _3403_AWBGGain0, &dwRgain);
				OV5653_ReadCompGain(ptInfo, _3404_AWBBGain1, _3405_AWBBGain0, &dwGgain);
				dwBgain = (dwBgain*64) / OV5653_WB_GAIN_UNIT;
				dwRgain = (dwRgain*64) / OV5653_WB_GAIN_UNIT;
				dwGgain = (dwGgain*64) / OV5653_WB_GAIN_UNIT;
					
				ptIoctlArg->adwUserData[1] = ((dwGgain<<8) | dwRgain);
				ptIoctlArg->adwUserData[2] = dwBgain;

				OV5653_UpdateBuf(ptInfo, _3406_AWBManual, 0xFE, 0x01);
				break;
/* Version 1.0.0.2 modification, 2012.08.06 */	
			}
		}
		sckRet = -1;
/* ========================================= */
		break;
		default:
		sckRet = -EPERM;
	}
	up(&OV5653_mutex);	

	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
void OV5653_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET OV5653_Open(TVideoSensorInitialParam* ptOV5653InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptOV5653InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(OV5653_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptOV5653InitialParam->dwVideoSensorVersion&0x0000FF00)>(OV5653_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid OV5653 device driver version %d.%d.%d.%d !!\n", 
				(int)(OV5653_INTERFACE_VERSION&0xFF), 
				(int)((OV5653_INTERFACE_VERSION>>8)&0xFF), 
				(int)((OV5653_INTERFACE_VERSION>>16)&0xFF), 
				(int)(OV5653_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TOV5653Info *)kmalloc(sizeof(TOV5653Info), GFP_KERNEL)))
	{
		printk("[OV5653] : Allocate %d bytes memory fail\n", sizeof(TOV5653Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[OV5653] : Initial IICCtrl object fail !!\n");
		OV5653_Release(dwDevNum);
		return -ENODEV;
	}
	
	// init private data
	ptInfo->dwDeviceAddr = OV5653_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptOV5653InitialParam->dwVideoSensorFrequency;
	ptInfo->bOV5653DSPEnable = iEnAWB;
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;

	// Detect device
	if (!OV5653_CompBuf(ptInfo, _300A_PIDH, 0xFF, ((OV5653_PID>>8)&0xFF)))
	{
		printk("[OV5653] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
		OV5653_Release(dwDevNum);	
		return -ENODEV;
	}
	if (!OV5653_CompBuf(ptInfo, _300B_PIDL, 0xFF, (OV5653_PID&0xFF))) 
	{
		printk("[OV5653] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
		OV5653_Release(dwDevNum);				
		return -ENODEV;
	}
	// Reset device
	OV5653_Reset(ptInfo);

	// init captured size
	if (OV5653_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
	{
		printk("[OV5653] : initial width/height error !!\n");			
		OV5653_Release(dwDevNum);				
		return -EPERM;
	}
	OV5653_SetFrequence();
	return 0;
}

/* Version 1.0.0.2 modification, 2012.08.06 */
/*-------------------------------------------------------------------------------------*/
DWORD OV5653_GetMaxGain(void)
{
	return (OV5653_MAX_GAIN);
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	OV5653_Open,
	.release = 	OV5653_Release,
	.ioctl = 	OV5653_Ioctl,
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = OV5653_GroupAccess,
/* Version 1.0.0.2 modification, 2012.08.06 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = OV5653_GetMaxGain,
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct OV5653_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t OV5653_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct OV5653_attribute *OV5653_attr = container_of(attr, struct OV5653_attribute, attr);
	int result = 0;
	
	if (OV5653_attr->show)
		result = OV5653_attr->show(kobj, buf);	
	return result;
}

ssize_t OV5653_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "OV5653");
	return retval;
}

ssize_t OV5653_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t OV5653_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "2560");
	return retval;
}

ssize_t OV5653_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject OV5653_kobj;

struct OV5653_attribute OV5653_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5653_SensorName_attr_show,
};

struct OV5653_attribute OV5653_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5653_dwBufNum_attr_show,
};

struct OV5653_attribute OV5653_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5653_MaxFrameWidth_attr_show,
};

struct OV5653_attribute OV5653_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV5653_MaxFrameHeight_attr_show,
};

struct sysfs_ops OV5653_sysfs_ops = {
	.show	= OV5653_default_attr_show,
};

struct kobj_type OV5653_ktype = {
	.sysfs_ops	= &OV5653_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET OV5653_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[OV5653] : kobj set name fail!\n");
		return -1;
	}

	OV5653_kobj.ktype = &OV5653_ktype;	
	sckResult = kobject_init_and_add(&OV5653_kobj, &OV5653_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[OV5653] : Cannot register kobject [OV5653_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV5653_kobj, &OV5653_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5653] : Cannot create OV5653_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&OV5653_kobj, &OV5653_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5653] : Cannot create OV5653_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV5653_kobj, &OV5653_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5653] : Cannot create OV5653_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV5653_kobj, &OV5653_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV5653] : Cannot create OV5653_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&OV5653_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void OV5653_Exit(void)
{
	sysfs_remove_file(&OV5653_kobj, &(OV5653_SensorName_attr.attr));
	sysfs_remove_file(&OV5653_kobj, &(OV5653_dwBufNum_attr.attr));	
	sysfs_remove_file(&OV5653_kobj, &(OV5653_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&OV5653_kobj, &(OV5653_MaxFrameHeight_attr.attr));		
	kobject_put(&OV5653_kobj);	
	printk(KERN_DEBUG"[OV5653] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(OV5653_Init);
module_exit(OV5653_Exit);
