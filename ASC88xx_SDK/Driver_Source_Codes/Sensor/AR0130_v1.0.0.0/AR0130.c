/*
 * AR0130
 * Driver for AR0130 sensor.
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

#include "AR0130.h"
#include "AR0130_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR AR0130_ID[] = "$Version: "AR0130_ID_VERSION"  (AR0130 DRIVER) $";

static TAR0130Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore AR0130_mutex;

static DWORD dwAR0130Hsync = AR0130_HSYNC_1280x720_60FPS;
static DWORD dwAR0130Vsync = AR0130_VSYNC_1280x720_60FPS;

static SOCKET video_sensor_width = 1280;
static SOCKET video_sensor_height = 720;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "AR0130 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "AR0130 capture height");

/*==================================================================*/
MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("AR0130 driver");
MODULE_LICENSE("GPL");

const TAR0130RegAddrData atAR0130SequenceTbl[AR0130_SEQUENCE_TBL_SIZE] = 
{
	{0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{0x3086, 0x0225}, 	// SEQ_DATA_PORT
	{0x3086, 0x5050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x0828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0926}, 	// SEQ_DATA_PORT
	{0x3086, 0x0028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0xA728}, 	// SEQ_DATA_PORT
	{0x3086, 0x0725}, 	// SEQ_DATA_PORT
	{0x3086, 0x8080}, 	// SEQ_DATA_PORT
	{0x3086, 0x2917}, 	// SEQ_DATA_PORT
	{0x3086, 0x0525}, 	// SEQ_DATA_PORT
	{0x3086, 0x0040}, 	// SEQ_DATA_PORT
	{0x3086, 0x2702}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1736}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A6}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A4}, 	// SEQ_DATA_PORT
	{0x3086, 0x171F}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x2620}, 	// SEQ_DATA_PORT
	{0x3086, 0x2804}, 	// SEQ_DATA_PORT
	{0x3086, 0x2520}, 	// SEQ_DATA_PORT
	{0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1E25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2117}, 	// SEQ_DATA_PORT
	{0x3086, 0x1028}, 	// SEQ_DATA_PORT
	{0x3086, 0x051B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x1747}, 	// SEQ_DATA_PORT
	{0x3086, 0x2660}, 	// SEQ_DATA_PORT
	{0x3086, 0x17AE}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x9027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0026}, 	// SEQ_DATA_PORT
	{0x3086, 0x1828}, 	// SEQ_DATA_PORT
	{0x3086, 0x002E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2A28}, 	// SEQ_DATA_PORT
	{0x3086, 0x081E}, 	// SEQ_DATA_PORT
	{0x3086, 0x0831}, 	// SEQ_DATA_PORT
	{0x3086, 0x1440}, 	// SEQ_DATA_PORT
	{0x3086, 0x4014}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1410}, 	// SEQ_DATA_PORT
	{0x3086, 0x1034}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x1014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x4013}, 	// SEQ_DATA_PORT
	{0x3086, 0x1802}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7003}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7017}, 	// SEQ_DATA_PORT
	{0x3086, 0x2002}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2002}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5022}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
};

const TAR0130RegAddrData atAR0130Tbl[AR0130_TBL_SIZE] = 
{ 
	{0x309E, 0x0000},	// RESERVED_MFR_309E
	{0x30E4, 0x6372},	// RESERVED_MFR_30E4
	{0x30E2, 0x7253},	// RESERVED_MFR_30E2
	{0x30E0, 0x5470},	// RESERVED_MFR_30E0
	{0x30E6, 0xC4CC},	// RESERVED_MFR_30E6
	{0x30E8, 0x8050},	// RESERVED_MFR_30E8
	{0x3082, 0x0029},	// OPERATION_MODE_CTRL
	{0x3EE6, 0x4303},	// RESERVED_MFR_3EE6
	{0x301E, 0x00C8},	// DATA_PEDESTAL 0xC8
	{0x3EDA, 0x0F03},	// RESERVED_MFR_3EDA
	{0x3EDE, 0xC005},	// RESERVED_MFR_3EDE
	{0x3ED8, 0x09EF},	// RESERVED_MFR_3ED8
	{0x3EE2, 0xA46B},	// RESERVED_MFR_3EE2
	{0x3EE0, 0x047D},	// RESERVED_MFR_3EE0
	{0x3EDC, 0x0070},	// RESERVED_MFR_3EDC
	{0x3044, 0x0404},	// DARK_CONTROL
	{0x3EE4, 0xD208},	// DAC_LD_24_25
	{0x3ED6, 0x00BD},	// RESERVED_MFR_3ED6
	{0x3180, 0x0000},	// DELTA_DK_CTRL_OFF
	{0x3100, 0x0000},	// DISABLE_AE_DCG1x
	{0x30B0, 0x1300},	// DIGITAL_TEST_AGAIN1x
	{0x30D4, 0xC00F},	// COLUMN_CORRECTION
	{0x30BA, 0x0000},	// DIGITAL_TEST
	{0x30EA, 0x8C00},	// GAIN OFFSET CTRL
	{0x301A, 0x10DC},	// RESET_REGISTER
	{0x301A, 0x10D8},	// RESET_REGISTER
	{0x3044, 0x0404},	// DARK_CONTROL
	{0x3012, 0x02A0},	// COARSE_INTEGRATION_TIME
	{0x3032, 0x0000},	// DIGITAL_BINNING
	{0x3064, 0x1802},	// EMBEDDED DATA OFF 
	{0x301E, 0x0020},	// DATA_PEDESTAL = 0x20 
	{0x31D0, 0x0001},	// HDR_COMP
	{0x301A, 0x10D8},	// RESET_REGISTER
	{0x302A, 0x0008},	//VT_PIX_CLK_DIV = 8
	{0x302C, 0x0001},	//VT_SYS_CLK_DIV = 1
	{0x302E, 0x0002},	//PRE_PLL_CLK_DIV = 2
	{0x3030, 0x002C},	//PLL_MULTIPLIER = 44
	{0x301A, 0x10DC},	// RESET_REGISTER
};

/*==================================================================*/
static SOCKET AR0130_WriteBuf(TAR0130Info *ptInfo, EAR0130Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[3];

	abyData[0] = (BYTE)(eRegAddr&0x00FF);
	abyData[1] = (BYTE)((dwData&0xFF00)>>8);
	abyData[2] = (BYTE)(dwData&0x00FF);
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), 3, abyData);
}

/*-------------------------------------------------------------------------------------*/ 
static SOCKET AR0130_ReadBuf(TAR0130Info *ptInfo, EAR0130Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen)
{
	BYTE abyData[30];
	DWORD dwIndex;

	abyData[0] = (BYTE)(eRegAddr&0x00FF);
	abyData[1] = 0;
	if (ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, ((eRegAddr&0xFF00)>>8), (2*dwDataLen), abyData) < 0)
	{
		return S_FAIL;
	}
	else
	{
		for (dwIndex = 0; dwIndex < dwDataLen; dwIndex++)
		{
			pdwData[dwIndex] = (abyData[2*dwIndex+1]&0xFF) + ((abyData[2*dwIndex]<<8)&0xFF00);
		}		
		return S_OK;
	}
}

/*-------------------------------------------------------------------------------------*/
static SOCKET AR0130_CompBuf(TAR0130Info *ptInfo, EAR0130Regs eRegAddr, DWORD dwMask, DWORD dwData)
{
	DWORD dwReadData;

	AR0130_ReadBuf(ptInfo, eRegAddr, &dwReadData, 1);
	return ((dwReadData&dwMask) == dwData);
}

/*-------------------------------------------------------------------------------------*/
static void AR0130_Reset(TAR0130Info *ptInfo)
{
	DWORD dwIndex;

	// RESET_REGISTER	
	AR0130_WriteBuf(ptInfo, 0x301A, 0x0058, 1, 0);
	mdelay(200);
	AR0130_WriteBuf(ptInfo, 0x301A, 0x0001, 1, 0);
	mdelay(200);
	AR0130_WriteBuf(ptInfo, 0x301A, 0x10D8, 1, 0);
	mdelay(200);

	for (dwIndex=0; dwIndex<AR0130_SEQUENCE_TBL_SIZE; dwIndex++)
	{
		AR0130_WriteBuf(ptInfo, atAR0130SequenceTbl[dwIndex].eRegAddr, atAR0130SequenceTbl[dwIndex].dwData, 1, 0);
	}

	for (dwIndex=0; dwIndex<AR0130_TBL_SIZE; dwIndex++)
	{
		AR0130_WriteBuf(ptInfo, atAR0130Tbl[dwIndex].eRegAddr, atAR0130Tbl[dwIndex].dwData, 1, 0);

		if (atAR0130Tbl[dwIndex].eRegAddr == 0x301A)
		{
			mdelay(300);		
		}
	}

	if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==960))
	{
		AR0130_WriteBuf(ptInfo, 0x3032, 0x0000, 1, 0);// DIGITAL_BINNING
		AR0130_WriteBuf(ptInfo, 0x3002, 0x0002, 1, 0);// Y_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3004, 0x0002, 1, 0);// X_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3006, 0x03C5, 1, 0);// Y_ADDR_END
		AR0130_WriteBuf(ptInfo, 0x3008, 0x0501, 1, 0);// X_ADDR_END

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x960_45FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x960_45FPS;
		}
		else
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x960_40FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x960_40FPS;
		}
	}
	else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
	{
		AR0130_WriteBuf(ptInfo, 0x3032, 0x0000, 1, 0);// DIGITAL_BINNING
		AR0130_WriteBuf(ptInfo, 0x3002, 0x007C, 1, 0);// Y_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3004, 0x0002, 1, 0);// X_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3006, 0x034F, 1, 0);// Y_ADDR_END
		AR0130_WriteBuf(ptInfo, 0x3008, 0x0501, 1, 0);// X_ADDR_END

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x720_60FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x720_60FPS;
		}
		else
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x720_50FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x720_50FPS;
		}
	}
	else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==800))
	{
		AR0130_WriteBuf(ptInfo, 0x3032, 0x0000, 1, 0);// DIGITAL_BINNING
		AR0130_WriteBuf(ptInfo, 0x3002, 0x0054, 1, 0);// Y_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3004, 0x0002, 1, 0);// X_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3006, 0x0377, 1, 0);// Y_ADDR_END
		AR0130_WriteBuf(ptInfo, 0x3008, 0x0501, 1, 0);// X_ADDR_END

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x800_60FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x800_60FPS;
		}
		else
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x800_50FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x800_50FPS;
		}
	}
	else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
	{
		AR0130_WriteBuf(ptInfo, 0x3032, 0x0002, 1, 0);// DIGITAL_BINNING  Horizontal and vertical binning
		AR0130_WriteBuf(ptInfo, 0x3002, 0x0002, 1, 0);// Y_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3004, 0x0002, 1, 0);// X_ADDR_START
		AR0130_WriteBuf(ptInfo, 0x3006, 0x03C9, 1, 0);// Y_ADDR_END
		AR0130_WriteBuf(ptInfo, 0x3008, 0x0501, 1, 0);// X_ADDR_END

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x960_45FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x960_45FPS;
		}
		else
		{
			dwAR0130Hsync = AR0130_HSYNC_1280x960_40FPS;
			dwAR0130Vsync = AR0130_VSYNC_1280x960_40FPS;
		}
	}
	else
	{
		printk("[AR0130] : Size is not supported (width=%d, height=%d).\n", (int)ptInfo->dwWidth, (int)ptInfo->dwHeight);
		return;
	}

	AR0130_WriteBuf(ptInfo, 0x300A, dwAR0130Vsync, 1, 0);// FRAME_LENGTH_LINES
	AR0130_WriteBuf(ptInfo, 0x300C, dwAR0130Hsync, 1, 0);// LINE_LENGTH_PCK

	AR0130_WriteBuf(ptInfo, 0x301E, 0x0018, 1, 0); // DATA_PEDESTAL
	AR0130_WriteBuf(ptInfo, 0x30EA, 0x8C00, 1, 0); // Manual black clamp
	AR0130_WriteBuf(ptInfo, 0x3180, 0x0000, 1, 0);

	// [Column Retriggering at start up]
	AR0130_WriteBuf(ptInfo, 0x30B0, 0x1300, 1, 0);
	AR0130_WriteBuf(ptInfo, 0x30D4, 0xE007, 1, 0);
	AR0130_WriteBuf(ptInfo, 0x30BA, 0x0008, 1, 0);
	AR0130_WriteBuf(ptInfo, 0x301A, 0x10DC, 1, 0);
	mdelay(200);
	AR0130_WriteBuf(ptInfo, 0x301A, 0x10D8, 1, 0);
	mdelay(200);

	AR0130_WriteBuf(ptInfo, 0x301A, 0x10DC, 1, 0);

	ptInfo->dwColumnGain = 0xFFFF;
	ptInfo->dwDACGain = 0xFFFF;
	ptInfo->dwDCGGain = 0xFFFF;
	ptInfo->dwGlobalGain = 0xFFFF;

	printk("AR0130_Reset Done \n");
}

/*-------------------------------------------------------------------------------------*/
void AR0130_SetShutter(DWORD dwShutter)
{
	DWORD dwValue;

	dwValue = (dwShutter*AR0130_PIXEL_CLOCK_M_100/100) / dwAR0130Hsync;

	if (dwValue > 0x0000FFFF)
	{
		dwValue = 0x0000FFFF;
	}
	else if (dwValue < 10)
	{
		dwValue = 10;
	}

	AR0130_WriteBuf(ptInfo, 0x3012, dwValue, 1, 0);
	ptInfo->dwCurShutter = dwShutter;
}

/*-------------------------------------------------------------------------------------*/
void AR0130_SetGain(TAR0130Info *ptInfo, DWORD dwGain)
{
	DWORD dwAnalogGain0 = 0;
	DWORD dwDigitalGain = 0, dwDCGGain = 0;

	if (dwGain > AR0130_MAX_GAIN)
	{
		dwGain = AR0130_MAX_GAIN;
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	if (dwGain < 2000)
	{
		dwAnalogGain0 = 0x1300; // 1X
		dwDCGGain = 0x001A; // 1X
		dwDigitalGain = dwGain*32/1000;
	}
	else if (dwGain < 2880)
	{
		dwAnalogGain0 = 0x1310; // 2X
		dwDCGGain = 0x001A; // 1X
		dwDigitalGain = dwGain*32/2000;
	}
	else if (dwGain < 5760)
	{
		dwAnalogGain0 = 0x1300;  // 1X
		dwDCGGain = 0x001E; // 2.88X
		dwDigitalGain = dwGain*32/2880;
	}
	else if (dwGain < 11520)
	{
		dwAnalogGain0 = 0x1310;  // 2X
		dwDCGGain = 0x001E; // 2.88X
		dwDigitalGain = dwGain*32/5760;
	}
	else if (dwGain < 23040)
	{
		dwAnalogGain0 = 0x1320; // 4X
		dwDCGGain = 0x001E; // 2.88X
		dwDigitalGain = dwGain*32/11520;
	}
	else
	{
		dwAnalogGain0 = 0x1330; // 8X
		dwDCGGain = 0x001E; // 2.88X
		dwDigitalGain = dwGain*32/23040;
	}

	if (ptInfo->dwGlobalGain != dwDigitalGain)
	{	
		AR0130_WriteBuf(ptInfo, 0x305E, (dwDigitalGain&0xFF), 1, 0); //D Gain
		ptInfo->dwGlobalGain = dwDigitalGain;
	}
	if (ptInfo->dwDCGGain != dwDCGGain)
	{
		AR0130_WriteBuf(ptInfo, 0x3100, (dwDCGGain&0xFFFF), 1, 0); //DGC Gain	
		ptInfo->dwDCGGain = dwDCGGain;
	}
	if (ptInfo->dwColumnGain != dwAnalogGain0)
	{
		AR0130_WriteBuf(ptInfo, 0x30B0, (dwAnalogGain0&0xFFFF), 1, 0); //A0 Gain
		ptInfo->dwColumnGain = dwAnalogGain0;
	}
}

/*==================================================================*/
SOCKET  AR0130_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET SCKRet = 0;

	if (down_interruptible(&AR0130_mutex))
	{
		return -ERESTARTSYS;	
	}

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		AR0130_Reset(ptInfo);
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		AR0130_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		AR0130_SetGain(ptInfo, ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[AR0130] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			SCKRet = -EPERM;
			break;
		}

		if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==960))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x960_45FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x960_45FPS;
			}
			else
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x960_40FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x960_40FPS;
			}
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x720_60FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x720_60FPS;
			}
			else
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x720_50FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x720_50FPS;
			}
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==800))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x800_60FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x800_60FPS;
			}
			else
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x800_50FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x800_50FPS;
			}
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x960_45FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x960_45FPS;
			}
			else
			{
				dwAR0130Hsync = AR0130_HSYNC_1280x960_40FPS;
				dwAR0130Vsync = AR0130_VSYNC_1280x960_40FPS;
			}
		}

		AR0130_WriteBuf(ptInfo, 0x300A, dwAR0130Vsync, 1, 0);// FRAME_LENGTH_LINES
		AR0130_WriteBuf(ptInfo, 0x300C, dwAR0130Hsync, 1, 0);// LINE_LENGTH_PCK

		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
		break;
		default:
		SCKRet = -EPERM;
	}
	up(&AR0130_mutex);

	return SCKRet;
}

/*-------------------------------------------------------------------------------------*/
void AR0130_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0130_Open(TVideoSensorInitialParam* ptAR0130InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptAR0130InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(AR0130_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptAR0130InitialParam->dwVideoSensorVersion&0x0000FF00)>(AR0130_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid AR0130 device driver version %d.%d.%d.%d !!\n", 
				(int)(AR0130_INTERFACE_VERSION&0xFF), 
				(int)((AR0130_INTERFACE_VERSION>>8)&0xFF), 
				(int)((AR0130_INTERFACE_VERSION>>16)&0xFF), 
				(int)(AR0130_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TAR0130Info *)kmalloc(sizeof(TAR0130Info), GFP_KERNEL)))
	{
		printk("[AR0130] : Allocate %d bytes memory fail\n", sizeof(TAR0130Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[AR0130] : Initial IICCtrl object fail !!\n");
		AR0130_Release(dwDevNum);			
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = AR0130_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = (DWORD)ptAR0130InitialParam->dwVideoSensorFrequency;
	ptInfo->dwCurShutter = 33333;
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;

	// Detect sensor device
	if (!AR0130_CompBuf(ptInfo, _3000_ChipVersion, 0xFFFF, AR0130_CHIP_VERSION))
	{
		printk("\n[AR0130] : Sensor device doesn't exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
		AR0130_Release(dwDevNum);				
		return -ENODEV;
	}
	else
	{
		printk("\n[AR0130] : Sensor device exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
	}

	// Reset device
	AR0130_Reset(ptInfo);

	return 0;
}

/*-------------------------------------------------------------------------------------*/
DWORD AR0130_GetMaxGain(void)
{
	return (AR0130_MAX_GAIN);
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	AR0130_Open,
	.release = 	AR0130_Release,
	.ioctl = 	AR0130_Ioctl,
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,	
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = AR0130_GetMaxGain,
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct AR0130_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t AR0130_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct AR0130_attribute *AR0130_attr = container_of(attr, struct AR0130_attribute, attr);
	int result = 0;
	
	if (AR0130_attr->show)
		result = AR0130_attr->show(kobj, buf);	
	return result;
}

ssize_t AR0130_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "AR0130");
	return retval;
}

ssize_t AR0130_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t AR0130_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1280");
	return retval;
}

ssize_t AR0130_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "960");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject AR0130_kobj;

struct AR0130_attribute AR0130_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0130_SensorName_attr_show,
};

struct AR0130_attribute AR0130_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0130_dwBufNum_attr_show,
};

struct AR0130_attribute AR0130_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0130_MaxFrameWidth_attr_show,
};

struct AR0130_attribute AR0130_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0130_MaxFrameHeight_attr_show,
};

struct sysfs_ops AR0130_sysfs_ops = {
	.show	= AR0130_default_attr_show,
};

struct kobj_type AR0130_ktype = {
	.sysfs_ops	= &AR0130_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET AR0130_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[AR0130] : kobj set name fail!\n");
		return -1;
	}
	
	printk(KERN_INFO "AR0130() sensor_width=%d sensor_height=%d\n", video_sensor_width, video_sensor_height);
	AR0130_kobj.ktype = &AR0130_ktype;	
	sckResult = kobject_init_and_add(&AR0130_kobj, &AR0130_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[AR0130] : Cannot register kobject [AR0130_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0130_kobj, &AR0130_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0130] : Cannot create AR0130_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&AR0130_kobj, &AR0130_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0130] : Cannot create AR0130_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0130_kobj, &AR0130_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0130] : Cannot create AR0130_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0130_kobj, &AR0130_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0130] : Cannot create AR0130_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&AR0130_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void AR0130_Exit(void)
{
	sysfs_remove_file(&AR0130_kobj, &(AR0130_SensorName_attr.attr));
	sysfs_remove_file(&AR0130_kobj, &(AR0130_dwBufNum_attr.attr));	
	sysfs_remove_file(&AR0130_kobj, &(AR0130_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&AR0130_kobj, &(AR0130_MaxFrameHeight_attr.attr));	
	kobject_put(&AR0130_kobj);	
	printk(KERN_DEBUG "[AR0130] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(AR0130_Init);
module_exit(AR0130_Exit);
