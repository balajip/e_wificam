/*
 * OV9710
 * Driver for OV9710 sensor.
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

#include "OV9710.h"
#include "OV9710_locals.h"
/* Version 1.0.0.4 modification, 2010.09.17 */
#include "IICCtrl.h"
/* ========================================= */
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()
/* ========================================= */

const CHAR OV9710_ID[] = "$Version: "OV9710_ID_VERSION"  (OV9710 DRIVER) $";
/*==================================================================*/
static TOV9710Info *ptInfo;
TVideoSensorDevice sensor_dev_ops;

/* Version 2.0.0.1 modification, 2011.11.24 */
static DWORD dwOV9710Hsync = OV9710_HSYNC_1280x800_30FPS;
static DWORD dwOV9710Vsync = OV9710_VSYNC_1280x800_30FPS;
/* ========================================= */

/* Sensor capture width */
static SOCKET video_sensor_width = 1280;

/* Sensor capture height */
static SOCKET video_sensor_height = 800;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "OV9710 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "OV9710 capture height");

static int iEnDSP=0;
module_param(iEnDSP, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnDSP, "Enable DSP");

static int iEnAE=0;
module_param(iEnAE, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnAE, "Enable AE");

/* Version 2.0.0.4 modification, 2012.08.06 */
static int iEnAWB=0;
module_param(iEnAWB, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnAWB, "Enable sensor AWB");
/* ======================================== */

static struct semaphore ov9710_mutex;

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("OV9710 driver");
MODULE_LICENSE("GPL");

/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */

/* ============================================================================================== */
const TOV9710RegAddrData atOV9710regTbl[OV9710_TBL_SIZE] =
{
	{_09_CommonControl2,	0x10},
	{_1E_CommonControl11,	0x07},
	{_5F_Reserved,			0x18},
	{_69_Reserved,			0x04},
	{_65_Reserved,			0x2A},
	{_68_Reserved,			0x0A},
	{_39_Reserved,			0x28},
	{_4D_Reserved,			0x90},
	{_C1_YAvgControl4,		0x80},

/* Version 2.0.0.2 modification, 2011.12.15 */
	{_0C_CommonControl3,		0x30},
	{_6D_Reserved,		0x02},
/* ======================================== */

	{_96_DSPControl0,		0xF1},
	{_BC_WBCControl0,		0x68},
	{_12_CommonControl7,	0x00},
	{_3B_Reserved,			0x00},
	{_97_DSPControl1,		0x80},
	{_17_HorzWindowStart,	0x25},
	{_18_AHSize,			0xA2},
	{_19_VertWindowStart,	0x01},
	{_1A_AVSize,			0xCA},
/* Version 1.0.0.7 modification, 2010.11.02 */
	//{_03_Reg03,				0x8A},
	{_03_Reg03,				0x0A},//The default max sutter speed is 30 fps.
/*==========================================*/	
	{_32_Reg32,				0x07},
	{_98_DSPControl2,		0x00},
	{_99_DSPControl3,		0x00},
	{_9A_DSPControl4,		0x00},
	{_57_Reg57,				0x01},
	{_58_Reg58,				0xC8},
	{_59_Reg59,				0xA0},
	{_4C_Reserved,			0x13},
	{_4B_Reserved,			0x36},
	{_3D_RENDL,				0x3C},
	{_3E_RENDH,				0x03},
	{_BD_YAvgControl0,		0xA0},
	{_BE_YAvgControl1,		0xC8},
	{_4E_Reg4E,				0x55},
	{_4F_Reg4F,				0x55},
	{_50_Reg50,				0x55},
	{_51_Reg51,				0x55},
	{_24_WPT,				0x55},
	{_25_BPT,				0x40},	
	{_26_VPT,				0xA1},
	{_5C_Reg5C,				0x5A},// 27MHz; 30fps
	{_5D_Reg5D,				0x00},
	{_11_ClockControl,		0x00},
	{_2B_Reg2B,				0x06},
	{_2A_Reg2A,				0x5E},
	{_2D_Reg2D,				0x00},
	{_2E_Reg2E,				0x00},
	//{_13_CommonControl8,	0xF5},
	{_13_CommonControl8,	0xFD},
	//{_13_CommonControl8,	0x00},	
	{_14_CommonControl9,	0x88},
	{_0E_CommonControl5,	0x48},
	{_C2_DVPControl00,		0x81},
	{_CB_DVPControl09,		0xAD},
	{_D0_DVPControl0E,		0x06},
	{_D1_DVPControl0F,		0x4A},
	{_4A_Reg4A,				0x00},
	{_49_Reg49,				0xCF},
	{_22_CommonControl14,	0x04},
	{_09_CommonControl2,	0x00}
};

const TOV9710RegAddrData atOV9710regDisableDSPTbl[OV9710_NO_DSP_TBL_SIZE] =
{

	{_09_CommonControl2,	0x10},
	{_1E_CommonControl11,	0x07},
	{_5F_Reserved,			0x18},
	{_69_Reserved,			0x04},
	{_65_Reserved,			0x2A},
	{_68_Reserved,			0x0A},
	{_39_Reserved,			0x28},
	{_4D_Reserved,			0x90},
	{_C1_YAvgControl4,		0x80},

/* Version 2.0.0.2 modification, 2011.12.15 */
	{_0C_CommonControl3,		0x30},
	{_6D_Reserved,		0x02},
/* ======================================== */

	{_96_DSPControl0,		0xE1}, 
	{_BC_WBCControl0,		0x68},
	{_12_CommonControl7,	0x00},
	{_3B_Reserved,			0x00},
	{_97_DSPControl1,		0x80},
	{_17_HorzWindowStart,	0x25},
	{_18_AHSize,			0xA2},
	{_19_VertWindowStart,	0x01},
	{_1A_AVSize,			0xCA},
/* Version 1.0.0.7 modification, 2010.11.02 */
	//{_03_Reg03,				0x8A},
	{_03_Reg03,				0x0A},//The default max sutter speed is 30 fps.
/*==========================================*/
	{_32_Reg32,				0x07},
	{_98_DSPControl2,		0x00},
	{_99_DSPControl3,		0x00},
	{_9A_DSPControl4,		0x00},
	{_57_Reg57,				0x01},
	{_58_Reg58,				0xC8},
	{_59_Reg59,				0xA0},
	{_4C_Reserved,			0x13},
	{_4B_Reserved,			0x36},
	{_3D_RENDL,				0x3C},
	{_3E_RENDH,				0x03},
	{_BD_YAvgControl0,		0xA0},
	{_BE_YAvgControl1,		0xC8},
	{_4E_Reg4E,				0x55},
	{_4F_Reg4F,				0x55},
	{_50_Reg50,				0x55},
	{_51_Reg51,				0x55},
	{_24_WPT,				0x55},
	{_25_BPT,				0x40},
	{_26_VPT,				0xA1},
	{_5C_Reg5C,				0x5A},// 27MHz; 30fps
	{_5D_Reg5D,				0x00},
	{_11_ClockControl,		0x00},
	{_2B_Reg2B,				0x06},
	{_2A_Reg2A,				0x5E},
	{_2D_Reg2D,				0x00},
	{_2E_Reg2E,				0x00},
	//{_13_CommonControl8,	0x00}, // Disable AE
	{_13_CommonControl8,	0xFD}, // Enable AE only
	{_14_CommonControl9,	0x88},
	{_0E_CommonControl5,	0x48},
	{_C2_DVPControl00,		0x81},
	{_CB_DVPControl09,		0xAD},
	{_D0_DVPControl0E,		0x06},
	{_D1_DVPControl0F,		0x4A},
	{_38_CommonControl25,   0x00},
	{_01_BlueGainControlH,	0x60},
	{_02_RedGainControlH,	0x60},
	{_4A_Reg4A,				0x00},
	{_49_Reg49,				0xCF},
	{_22_CommonControl14,	0x04},
	{_09_CommonControl2,	0x00}
};

/* Version 2.0.0.3 modification, 2011.12.28 */
EOV9710Regs aeOV9710CaptureSizeReg[23] = 
{
	_12_CommonControl7,	
	_3B_Reserved,		
	_97_DSPControl1,	
	_17_HorzWindowStart,	
	_18_AHSize,		
	_19_VertWindowStart,	
	_1A_AVSize,		
	_03_Reg03,		
	_32_Reg32,		
	_98_DSPControl2,	
	_99_DSPControl3,	
	_9A_DSPControl4,	
	_57_Reg57,		
	_58_Reg58,		
	_59_Reg59,
	_4B_Reserved,
	_4C_Reserved,				
	_3D_RENDL,		
	_3E_RENDH,		
	_BD_YAvgControl0,	
	_BE_YAvgControl1,	
	_2C_CommonControl19,
	_23_CommonControl15,
};

const BYTE abyOV9710CaptureSizeData[23][2] =
{
	{0x00,	0x40,},
	{0x00,	0x01,},
	{0x80,	0x80,},
	{0x25,	0x25,},
	{0xA2,	0xA2,},
	{0x01,	0x00,},
	{0xCA,	0x66,},
/* Version 2.0.0.4 modification, 2012.08.06 */
	{0x0B,	0x01,},
/* ======================================== */
	{0x07,	0x06,},
	{0x00,	0x00,},
	{0x00,	0x00,},
	{0x00,	0x00,},
/* Version 2.0.0.4 modification, 2012.08.06 */
	{0x00,	0x00,},
	{0xC9,	0x66,},
/* ======================================== */
	{0xA0,	0x51,},
	{0x36,	0x9A,},
	{0x13,	0x09,},
	{0x3C,	0x9E,},
	{0x03,	0x01,},
	{0xA0,	0x50,},
	{0xC8,	0x64,},
	{0x50,	0x60,},
	{0x00,	0x10,},
};
/* ======================================== */
/*==================================================================*/

static SOCKET  sensor_read_reg(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwSpecRegAddr, DWORD *pdwData)
{
	BYTE byData;

	if (IICCtrl_ReadReg((HANDLE)dwUserData, (BYTE)dwDeviceAddr, (BYTE)dwRegAddr, &byData) == -1)
	{
		return -1;
	}

	*pdwData = byData;

	return 0;
}

static SOCKET  sensor_write_reg(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwSpecRegAddr, DWORD dwData)
{
	if (IICCtrl_WriteReg((HANDLE)dwUserData, (BYTE)dwDeviceAddr, (BYTE)dwRegAddr, (BYTE)dwData) == -1)
	{
		return -1;
	}
	return 0;
}

static SOCKET OV9710_WriteReg(TOV9710Info *ptInfo, EOV9710Regs eRegAddr, DWORD dwData)
{
	return ptInfo->pfnWriteReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, 0, dwData);


}

/* Version 2.0.0.1 modification, 2011.11.24 */
static SOCKET OV9710_ReadReg(TOV9710Info *ptInfo, EOV9710Regs eRegAddr, DWORD *pdwData)
{
	return ptInfo->pfnReadReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, 0, pdwData);
}
/* ======================================== */

static SOCKET OV9710_CompReg(TOV9710Info *ptInfo, EOV9710Regs eRegAddr, DWORD dwMask, DWORD dwData)
{
	DWORD dwReadData;

	ptInfo->pfnReadReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, 0, &dwReadData);

	if ((dwReadData&dwMask) == dwData)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.2 modification, 2011.12.15 */
static SOCKET OV9710_UpdateBuf(TOV9710Info *ptInfo, EOV9710Regs eRegAddr, BYTE byMask, BYTE byData)
{
	DWORD dwReadData, dwWriteData;

	if (OV9710_ReadReg(ptInfo, eRegAddr, &dwReadData) < 0)
	{
		return -1;
	}
	dwWriteData = ((dwReadData&byMask) | byData);

	if (OV9710_WriteReg(ptInfo, eRegAddr, dwWriteData) < 0)
	{
		return -1;
	}
	return 0;
}
/* ======================================== */
/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.3 modification, 2011.12.28 */
SOCKET OV9710_SetFrequence(void)
{
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_50HZ)
	{
		if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==800))
		{
			dwOV9710Hsync = OV9710_HSYNC_1280x800_25FPS;
			dwOV9710Vsync = OV9710_VSYNC_1280x800_25FPS;
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==400))
		{
			dwOV9710Hsync = OV9710_HSYNC_640x400_50FPS;
			dwOV9710Vsync = OV9710_VSYNC_640x400_50FPS;
		}
	}
	else
	{
		if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==800))
		{
			dwOV9710Hsync = OV9710_HSYNC_1280x800_30FPS;
			dwOV9710Vsync = OV9710_VSYNC_1280x800_30FPS;
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==400))
		{
			dwOV9710Hsync = OV9710_HSYNC_640x400_60FPS;
			dwOV9710Vsync = OV9710_VSYNC_640x400_60FPS;
		}
	}

	if (OV9710_WriteReg(ptInfo, _3D_RENDL, (BYTE)(dwOV9710Vsync&0xFF)) == -1)
	{
		printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _3D_RENDL);
	} 
	if (OV9710_WriteReg(ptInfo, _3E_RENDH, (BYTE)((dwOV9710Vsync>>8)&0xFF)) == -1)
	{
		printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _3E_RENDH);
	} 

	if (OV9710_WriteReg(ptInfo, _D0_DVPControl0E, (BYTE)((dwOV9710Hsync>>8)&0xFF)) == -1)
	{
		printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _D0_DVPControl0E);
	} 
	if (OV9710_WriteReg(ptInfo, _D1_DVPControl0F, (BYTE)(dwOV9710Hsync&0xFF)) == -1)
	{
		printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _D1_DVPControl0F);
	}
	return 0;
}
/* ======================================== */
/*-------------------------------------------------------------------------------------*/
static void OV9710_Reset(TOV9710Info *ptInfo)
{
	int i = 0;
	
	//printk(KERN_INFO"[OV9710]Enter OV9710_Reset()..\n");
	 
	if (OV9710_WriteReg(ptInfo, _12_CommonControl7, 0x80) == -1)
	{
	    printk(KERN_INFO"[OV9710](%d) OV9710_Reset() write reg error!!\n", __LINE__);
	
	}
	//Put udelay 100 here for preventing the "get ack fail" error.
	udelay(100);

	if (OV9710_CompReg(ptInfo, _12_CommonControl7, 0x80, 0x00) == -1)
	{
		printk(KERN_INFO"[OV9710](%d) OV9710_Reset() comp reg error!!\n", __LINE__);

	}

	if (ptInfo->eFormat == VIDEO_SIGNAL_FORMAT_BAYER_PATTERN)
	{
		if (ptInfo->bOV9710DSPEnable == TRUE) // DSP function is enabled
		{
			for (i=0; i<OV9710_TBL_SIZE; i++)
			{
				if(atOV9710regTbl[i].eRegAddr == _13_CommonControl8)
				{
					if (iEnAE)
					{
						if (OV9710_WriteReg(ptInfo, atOV9710regTbl[i].eRegAddr, 0xFD) == -1)
						{
							printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n", __LINE__, i);
						}
					}
					else
					{
						if (OV9710_WriteReg(ptInfo, atOV9710regTbl[i].eRegAddr, 0x00) == -1)
						{
							printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n", __LINE__, i);
						}
					}
				}
				else
				{
					if (OV9710_WriteReg(ptInfo, atOV9710regTbl[i].eRegAddr, atOV9710regTbl[i].dwData) == -1)
					{
						printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n",__LINE__,i);
					}
				}	
			}
		}
		else // DSP function is disabled
		{
			for (i=0; i<OV9710_NO_DSP_TBL_SIZE; i++)
			{	
				if (atOV9710regDisableDSPTbl[i].eRegAddr == _13_CommonControl8)
				{
					if (iEnAE)
					{
						if (OV9710_WriteReg(ptInfo, atOV9710regDisableDSPTbl[i].eRegAddr, 0xFD) == -1)
						{
							printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n", __LINE__, i);
						}
					}
					else
					{
						if (OV9710_WriteReg(ptInfo, atOV9710regDisableDSPTbl[i].eRegAddr, 0x00) == -1)
						{
							printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n", __LINE__, i);
						}
					}
				}
				else
				{
					if (OV9710_WriteReg(ptInfo, atOV9710regDisableDSPTbl[i].eRegAddr, atOV9710regDisableDSPTbl[i].dwData) == -1)
					{
						printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n", __LINE__, i);
					} 
				}		
			}
		}
	}
	else 
	{
		printk(KERN_INFO"[OV9710]Unsupport output signal type !!\n");
		return ;
	}

/* Version 2.0.0.4 modification, 2012.08.06 */
	if (!ptInfo->bOV9710AWBEnable)
	{
		OV9710_UpdateBuf(ptInfo, _96_DSPControl0, 0xCF, 0x00);
	}
/* ======================================== */
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.3 modification, 2011.12.28 */
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.5 modification, 2012.09.25 */
	if (OV9710_WriteReg(ptInfo, _37_CommonControl24, 0x02) == -1)
	{
		printk(KERN_INFO"[OV9710](%d)(index=%d) OV9710_Reset() write reg error!!\n", __LINE__, i);
	} 
/* ======================================== */
	return;
	//printk(KERN_INFO"[OV9710]Exit OV9710_Reset()..\n");
}

/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
void OV9710_SetShutter(DWORD dwShutter)
{
/* Version 2.0.0.1 modification, 2011.11.24 */
	DWORD  dwDummyLinesReg = 0;
	DWORD dwExpLine = 0;
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
	dwExpLine = (((dwShutter*OV9710_PIXEL_CLOCK_M_10/10)*10)/dwOV9710Hsync+5)/10;

	dwExpLine = (dwExpLine>0x0000FFFF) ? 0x0000FFFF : (dwExpLine<10) ? 10 : dwExpLine;
	
	if (dwExpLine > (dwOV9710Vsync-OV9710_PRE_CHARGING_LINE))
	{
		dwDummyLinesReg = dwExpLine  - (dwOV9710Vsync-OV9710_PRE_CHARGING_LINE);
		OV9710_WriteReg(ptInfo, _10_AutoExposureControlL, (BYTE)((dwOV9710Vsync-OV9710_PRE_CHARGING_LINE)&0xFF));		
		OV9710_WriteReg(ptInfo, _16_AutoExposureControlH, (BYTE)(((dwOV9710Vsync-OV9710_PRE_CHARGING_LINE)>>8)&0xFF));
		OV9710_WriteReg(ptInfo, _2D_Reg2D, (BYTE)(dwDummyLinesReg&0xFF));			
		OV9710_WriteReg(ptInfo, _2E_Reg2E, (BYTE)((dwDummyLinesReg>>8)&0xFF));
	}
	else
	{
		OV9710_WriteReg(ptInfo, _10_AutoExposureControlL, (BYTE)(dwExpLine&0xFF));
		OV9710_WriteReg(ptInfo, _16_AutoExposureControlH, (BYTE)((dwExpLine>>8)&0xFF));
		OV9710_WriteReg(ptInfo, _2D_Reg2D, (BYTE)(dwDummyLinesReg&0x00)) ;			
		OV9710_WriteReg(ptInfo, _2E_Reg2E, (BYTE)((dwDummyLinesReg>>8)&0x00));		
/* ======================================== */
	}
	//printk(KERN_INFO"[OV9710]Exit OV9710_SetShutter()..\n");
}

/*-------------------------------------------------------------------------------------*/
void OV9710_SetGain(DWORD dwGain)
{
	DWORD dwValue = 0;	
	//printk(KERN_INFO"[OV9710]Enter OV9710_SetGain()..\n");
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */

/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
	dwGain = (dwGain>OV9710_MAX_GAIN) ? OV9710_MAX_GAIN : (dwGain<1000) ? 1000 : dwGain;
/* ======================================== */

	if (dwGain < 2000)
	{
		dwValue = (dwGain*16/1000) - 16;
	}
	else if (dwGain < 4000)
	{
		dwValue = (dwGain*8/1000-16) + 16;
	}
	else if (dwGain < 8000)
	{
		dwValue = (dwGain*4/1000-16) + 48;
	}
	else if (dwGain < 16000)
	{
		dwValue = (dwGain*2/1000-16) + 112;
	}
	else
	{
		dwValue = (dwGain/1000-16) + 240;
	}
/* ======================================== */

	OV9710_WriteReg(ptInfo, _00_GainControl, (dwValue&0xFF));
	//printk(KERN_INFO"[OV9710]Exit OV9710_SetGain()..\n");
}


/*-------------------------------------------------------------------------------------*/
int OV9710_SetSize(DWORD dwWidth, DWORD dwHeight)
{
	//printk(KERN_INFO"[OV9710]Enter OV9710_SetSize()..\n");
/* Version 2.0.0.3 modification, 2011.12.28 */
	DWORD dwIndex = 0;

	if ((dwWidth==1280) && (dwHeight==800))
	{
		for (dwIndex=0; dwIndex<23; dwIndex++)
		{
			OV9710_WriteReg(ptInfo, aeOV9710CaptureSizeReg[dwIndex], abyOV9710CaptureSizeData[dwIndex][0]);
		}
	}
	else if ((dwWidth==640) && (dwHeight==400))
	{
		for (dwIndex=0; dwIndex<23; dwIndex++)
		{
			OV9710_WriteReg(ptInfo, aeOV9710CaptureSizeReg[dwIndex], abyOV9710CaptureSizeData[dwIndex][1]);
		}
	}
	else	
	{
		return -1;			
	}
/* ======================================== */
	ptInfo->dwWidth = dwWidth;
	ptInfo->dwHeight = dwHeight;
	//printk(KERN_INFO"[OV9710]Exit OV9710_SetSize()..\n");		
	return 0;
}

/*==================================================================*/
int  OV9710_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	int ret = 0;
/* Version 1.0.0.5 modification, 2010.09.20 */
	DWORD dwData = 0;
/* ========================================= */	
/* Version 2.0.0.1 modification, 2011.11.24 */
	DWORD dwRgain = 0, dwGgain = 0, dwBgain = 0;
/* ========================================= */	

	if (down_interruptible(&ov9710_mutex))
	{
		return -ERESTARTSYS;	
	}
	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		OV9710_Reset(ptInfo);
		// Reset captured size
/* Version 2.0.0.3 modification, 2011.12.28 */
		if (OV9710_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
		{
			printk("[OV9710] : Reset width/height error !!\n");			
			ret =  -EPERM;
		}
		else
		{
			if (!iEnAE)
			{
				OV9710_SetFrequence();
			}
		}
/* ======================================== */
		break;
		case VIDEO_SIGNAL_OPTION_SET_SIZE:
		// 1. set size
		if (OV9710_SetSize(ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]) < 0)
		{
			printk("[OV9710] : Size is not supported (width=%ld, height=%ld).\n", ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]);			
			ret = -EPERM;
/* Version 2.0.0.3 modification, 2011.12.28 */
		}
		else
		{
			if (!iEnAE)
			{
				OV9710_SetFrequence();
			}
/* ======================================== */
		}
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
/* Version 1.0.0.4 modification, 2010.09.17 */
/* Version 1.0.0.1 modification, 2010.09.17 */
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
/* ========================================= */
/* ========================================= */
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		OV9710_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		OV9710_SetGain(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
/* Version 1.0.0.6 modification, 2010.09.28 */
/* Version 2.0.0.1 modification, 2011.11.24 */
		if (iEnAE)
		{
/*=========================================*/
			dwData = (ptIoctlArg->adwUserData[0]==VIDEO_SIGNAL_FREQUENCY_60HZ) ? 207 : 248;
			if (OV9710_WriteReg(ptInfo, _49_Reg49, dwData) == -1)
			{
				printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _49_Reg49);
				ret = -EPERM;
			}

			if (OV9710_WriteReg(ptInfo, _4A_Reg4A, 0x0) == -1)
			{
				printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _4A_Reg4A);
				ret = -EPERM;
			}
/* Version 2.0.0.1 modification, 2011.11.24 */
		}
		else
		{
/* Version 2.0.0.3 modification, 2011.12.28 */
			if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				ptInfo->dwFreq = VIDEO_SIGNAL_FREQUENCY_60HZ;
			}
			else 
			{
				ptInfo->dwFreq = VIDEO_SIGNAL_FREQUENCY_50HZ;
			}
			OV9710_SetFrequence();
/*=========================================*/
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
		}
/*=========================================*/
/*=========================================*/
		break;		
/* Version 1.0.0.5 modification, 2010.09.20 */
		case VIDEO_SIGNAL_OPTION_SET_EXPOSURE_LEVEL:
/* Version 1.0.0.7 modification, 2010.11.02 */
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE:
/*=========================================*/	  
		//printk("[OV9710](%d)VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE\n",__LINE__);
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
		dwData = ptIoctlArg->adwUserData[0] + ptIoctlArg->adwUserData[1];
		dwData = (dwData>255) ? 255 : dwData;
		if (OV9710_WriteReg(ptInfo, _24_WPT, dwData) == -1)			
		{
			printk("[OV9710]: Write OV9710 register 0x%02X fail !!\n", _24_WPT);
			ret = -EPERM;
		}
		dwData = (ptIoctlArg->adwUserData[0]<ptIoctlArg->adwUserData[1]) ? 0
				   : (ptIoctlArg->adwUserData[0]-ptIoctlArg->adwUserData[1]);
		if (OV9710_WriteReg(ptInfo, _25_BPT, dwData) != S_OK)			
		{
			printk("[OV9710]: Write OV9710 register 0x%02X fail !!\n", _25_BPT);
			ret = -EPERM;
		}
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER:
		//printk("[OV9710](%d)VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER\n",__LINE__);
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
		if (ptIoctlArg->adwUserData[0] == 4)
		{
			dwData = 0x8A;
		}
		else if (ptIoctlArg->adwUserData[0] == 10)
		{
			dwData = 0x6A;
		}
		else if (ptIoctlArg->adwUserData[0] == 15)
		{
			dwData = 0x4A;
		}
		else // 30fps and others
		{
			dwData = 0xA;
		}
		if (OV9710_WriteReg(ptInfo, _03_Reg03, dwData) == -1)
		{
			printk("[OV9710]: Write OV9710 register 0x%02X fail !!\n", _03_Reg03);
			ret = -EPERM;
		}
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
		break;
/* Version 1.0.0.8 modification, 2010.11.02 */
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER:
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
	     break;//printk("[OV9710](%d)VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER\n",__LINE__);
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN: 
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
	     break;//printk("[OV9710](%d)VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN\n",__LINE__);
/* ========================================= */		
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN: 
	      //printk("[OV9710](%d)VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN\n",__LINE__);
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
		if (ptIoctlArg->adwUserData[0] == 2)
		{
			dwData = 0x08;
		}
		else if (ptIoctlArg->adwUserData[0] == 4)
		{
			dwData = 0x28;
		}
		else if (ptIoctlArg->adwUserData[0] == 8)
		{
			dwData = 0x48;
		}
		else if (ptIoctlArg->adwUserData[0] == 16)
		{
			dwData = 0x68;
		}
		else // (ptOptions->adwUserData[0] == 32)
		{
			dwData = 0x88;
		}
		if (OV9710_WriteReg(ptInfo, _14_CommonControl9, dwData) == -1)
		{
			printk("[OV9710]: Write OV9710 register 0x%02X fail !!\n", _14_CommonControl9);
			ret = -EPERM;
		}
/* Version 2.0.0.1 modification, 2011.11.24 */
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
/*=========================================*/	  
		break;
/* ========================================= */
/* Version 2.0.0.1 modification, 2011.11.24 */
		case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
/* Version 2.0.0.4 modification, 2012.08.06 */
		if (ptInfo->bOV9710AWBEnable)
		{
/* ======================================== */
			switch (ptIoctlArg->adwUserData[0])
			{
				case 0:	// auto WB
/* Version 2.0.0.2 modification, 2011.12.15 */
				OV9710_UpdateBuf(ptInfo, _96_DSPControl0, 0xEF, 0x10);
				OV9710_UpdateBuf(ptInfo, _38_CommonControl25, 0xEF, 0x10);
				break;
				case 1:	// manual WB
				OV9710_UpdateBuf(ptInfo, _96_DSPControl0, 0xEF, 0x00);
				OV9710_UpdateBuf(ptInfo, _38_CommonControl25, 0xEF, 0x00);
/* ========================================= */

				dwRgain = (ptIoctlArg->adwUserData[1] & 0xFF);
				dwBgain = (ptIoctlArg->adwUserData[2] & 0xFF);
				dwGgain = ((ptIoctlArg->adwUserData[1]&0xFF00)>>8);

				//The WB GAIN UNIT of VIC is 64. 
				dwRgain = dwRgain * (OV9710_WB_GAIN_UNIT/64);
				dwBgain = dwBgain * (OV9710_WB_GAIN_UNIT/64);
				dwGgain = dwGgain * (OV9710_WB_GAIN_UNIT/64);

				OV9710_WriteReg(ptInfo, _02_RedGainControlH, (BYTE)(dwRgain>>4));
				OV9710_WriteReg(ptInfo, _01_BlueGainControlH, (BYTE)(dwBgain>>4));
				OV9710_WriteReg(ptInfo, _06_BRGainControlL, (BYTE)(((dwBgain&0x0F)<<4)|(dwRgain&0x0F)));
				OV9710_WriteReg(ptInfo, _05_GreenGainControlH, (BYTE)(dwGgain>>4));
				OV9710_WriteReg(ptInfo, _07_GreenGainControlL, ((dwGgain&0x0F)<<4));
				break;
				default: // auto track white, store current R/G/B gains and set to manual WB
				OV9710_ReadReg(ptInfo, _02_RedGainControlH, &dwRgain);
				OV9710_ReadReg(ptInfo, _01_BlueGainControlH, &dwBgain);
				OV9710_ReadReg(ptInfo, _06_BRGainControlL, &dwData);
				dwRgain = (dwRgain<<4) | (dwData&0x0F);
				dwBgain = (dwBgain<<4) | ((dwData&0xF0)>>4);

				OV9710_ReadReg(ptInfo, _05_GreenGainControlH, &dwGgain);
				OV9710_ReadReg(ptInfo, _07_GreenGainControlL, &dwData);
				dwGgain = (dwGgain<<4) | ((dwData&0xF0)>>4);

				dwBgain = (dwBgain*64) / OV9710_WB_GAIN_UNIT;
				dwRgain = (dwRgain*64) / OV9710_WB_GAIN_UNIT;
				dwGgain = (dwGgain*64) / OV9710_WB_GAIN_UNIT;
					
				ptIoctlArg->adwUserData[1] = ((dwGgain<<8) | dwRgain);
				ptIoctlArg->adwUserData[2] = dwBgain;

				// set to manual WB
/* Version 2.0.0.2 modification, 2011.12.15 */
				OV9710_UpdateBuf(ptInfo, _96_DSPControl0, 0xEF, 0x00);
				OV9710_UpdateBuf(ptInfo, _38_CommonControl25, 0xEF, 0x00);
/*=========================================*/	  
				break;
			}
/* Version 2.0.0.4 modification, 2012.08.06 */
		}
/* ======================================== */
		ret = -1;
		break;
/*=========================================*/	  
		default:
		ret = -EPERM;
	}

	up(&ov9710_mutex);	

	return ret;
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */

void OV9710_Release(DWORD dwDevNum)
/* ========================================= */
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}


int OV9710_Open(TVideoSensorInitialParam* ptOV9710InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

/* Version 2.0.0.0 modification, 2011.01.27 */
	if (((ptOV9710InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(OV9710_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptOV9710InitialParam->dwVideoSensorVersion&0x0000FF00)>(OV9710_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid OV9710 device driver version %d.%d.%d.%d !!\n", 
				(int)(OV9710_INTERFACE_VERSION&0xFF), 
				(int)((OV9710_INTERFACE_VERSION>>8)&0xFF), 
				(int)((OV9710_INTERFACE_VERSION>>16)&0xFF), 
				(int)(OV9710_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

	if (!(ptInfo = (TOV9710Info *)kmalloc(sizeof(TOV9710Info), GFP_KERNEL)))
	{
		printk("[OV9710] : Allocate %d bytes memory fail\n", sizeof(TOV9710Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[OV9710] : Initial IICCtrl object fail !!\n");
		OV9710_Release(dwDevNum);			
		
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = OV9710_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadReg = (FOnReadReg)sensor_read_reg;
	ptInfo->pfnWriteReg = (FOnWriteReg)sensor_write_reg;

	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;

	ptInfo->dwFreq = (DWORD)(ptOV9710InitialParam->dwVideoSensorFrequency);
	if (iEnDSP == 1)
	{
		ptInfo->bOV9710DSPEnable = TRUE;
	}
	else
	{	
		ptInfo->bOV9710DSPEnable = FALSE;
		
	}	
	ptInfo->eFormat = VIDEO_SIGNAL_FORMAT_BAYER_PATTERN;

/* Version 2.0.0.4 modification, 2012.08.06 */
	ptInfo->bOV9710AWBEnable = iEnAWB;
/* ======================================== */

/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */

	// Reset device
	OV9710_Reset(ptInfo);

	// init captured size
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
	if (OV9710_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0) 
	{
		printk("[OV9710] : initial width/height error !!\n");			
		OV9710_Release(dwDevNum);				
		return -EPERM;
	}
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */

/* Version 2.0.0.3 modification, 2011.12.28 */
	if (!iEnAE)
	{

		if (OV9710_WriteReg(ptInfo, _0E_CommonControl5, 0x40) == -1)
		{
			printk("[OV9710] : Write OV9710 register 0x%02X fail !!\n", _0E_CommonControl5);
		} 

		OV9710_SetFrequence();
	}
/* ======================================== */
	return 0;
}

/* Version 2.0.0.4 modification, 2012.08.06 */
/*-------------------------------------------------------------------------------------*/
DWORD OV9710_GetMaxGain(void)
{
	return (OV9710_MAX_GAIN);
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
TVideoSensorDevice sensor_dev_ops =
{
	.open = 	OV9710_Open,
	.release = 	OV9710_Release,
	.ioctl = 	OV9710_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,		
/* Version 2.0.0.4 modification, 2012.08.06 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = OV9710_GetMaxGain,	
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);


/*==================================================================*/
struct OV9710_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t OV9710_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct OV9710_attribute *OV9710_attr = container_of(attr, struct OV9710_attribute, attr);
	int result = 0;
	
	if (OV9710_attr->show)
		result = OV9710_attr->show(kobj, buf);	
	return result;
}

ssize_t OV9710_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "OV9710");
	return retval;
}

ssize_t OV9710_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t OV9710_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1280");
	return retval;
}

ssize_t OV9710_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "800");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject OV9710_kobj;

struct OV9710_attribute OV9710_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV9710_SensorName_attr_show,
};

struct OV9710_attribute OV9710_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV9710_dwBufNum_attr_show,
};

struct OV9710_attribute OV9710_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV9710_MaxFrameWidth_attr_show,
};

struct OV9710_attribute OV9710_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV9710_MaxFrameHeight_attr_show,
};

struct sysfs_ops OV9710_sysfs_ops = {
	.show	= OV9710_default_attr_show,
};

struct kobj_type OV9710_ktype = {
	.sysfs_ops	= &OV9710_sysfs_ops,
};


/*-------------------------------------------------------------------------------------*/
static int OV9710_init(void)
{
	int result = 0;
/* Version 2.0.0.3 modification, 2011.12.28 */
	printk("[OV9710] : init...iEnDSP=%d iEnAE=%d\n",iEnDSP,iEnAE);
	printk("[OV9710] : init...video_sensor_width=%d video_sensor_height=%d\n",video_sensor_width,video_sensor_height);
/* ======================================== */
	if (result < 0){
		printk("[OV9710] : kobj set name fail!\n");
		return -1;
	}


	OV9710_kobj.ktype = &OV9710_ktype;	
	result = kobject_init_and_add(&OV9710_kobj, &OV9710_ktype, NULL, "VideoSensorInfo");

	if (result < 0) {
		printk("[OV9710] : Cannot register kobject [OV9710_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&OV9710_kobj, &OV9710_SensorName_attr.attr);		
       if (result < 0) {
		printk("[OV9710] : Cannot create OV9710_SensorName_attr.attr.\n");
		return -1;
       }

	result = sysfs_create_file(&OV9710_kobj, &OV9710_dwBufNum_attr.attr);		
       if (result < 0) {
		printk("[OV9710] : Cannot create OV9710_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&OV9710_kobj, &OV9710_MaxFrameWidth_attr.attr);		
       if (result < 0) {
		printk("[OV9710] : Cannot create OV9710_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&OV9710_kobj, &OV9710_MaxFrameHeight_attr.attr);		
       if (result < 0) {
		printk("[OV9710] : Cannot create OV9710_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   

	sema_init(&ov9710_mutex, 1);

	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void OV9710_exit(void)
{

	printk(KERN_DEBUG "[OV9710] : bye\n");
	sysfs_remove_file(&OV9710_kobj, &(OV9710_SensorName_attr.attr));
	sysfs_remove_file(&OV9710_kobj, &(OV9710_dwBufNum_attr.attr));	
	sysfs_remove_file(&OV9710_kobj, &(OV9710_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&OV9710_kobj, &(OV9710_MaxFrameHeight_attr.attr));	

	kobject_put(&OV9710_kobj);
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(OV9710_init);
module_exit(OV9710_exit);
