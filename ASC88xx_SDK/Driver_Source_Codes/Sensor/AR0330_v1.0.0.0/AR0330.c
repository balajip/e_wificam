/*
 * AR0330
 * Driver for AR0330 sensor.
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

#include "AR0330.h"
#include "AR0330_locals.h"
#include "IICCtrl.h" 
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR AR0330_ID[] = "$Version: "AR0330_ID_VERSION"  (AR0330 DRIVER) $";

static TAR0330Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore AR0330_mutex;

static DWORD dwAR0330Hsync = AR0330_HSYNC_1920x1080_30HZ;
static DWORD dwAR0330Vsync = AR0330_VSYNC_1920x1080_30HZ;
static DWORD AR0330_PIXEL_CLOCK_M_100 = (74250000/10000);

static SOCKET video_sensor_width = 1920;
static SOCKET video_sensor_height = 1080;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "AR0330 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "AR0330 capture height");

/*==================================================================*/
MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("AR0330 driver");
MODULE_LICENSE("GPL");

const DWORD adwAR0330AnalogGainTbl[29][2] =
{
	{1000,0x0000},
	{1030,0x0101},
	{1070,0x0202},
	{1100,0x0303},
	{1140,0x0404},
	{1190,0x0505},
	{1230,0x0606},
	{1280,0x0707},
	{1330,0x0808},
	{1390,0x0909},
	{1450,0x0A0A},
	{1520,0x0B0B},
	{1600,0x0C0C},
	{1680,0x0D0D},
	{1780,0x0E0E},
	{1880,0x0F0F},
	{2000,0x1010},
	{2130,0x1212},
	{2290,0x1414},
	{2460,0x1616},
	{2670,0x1818},
	{2910,0x1A1A},
	{3200,0x1C1C},
	{3560,0x1E1E},
	{4000,0x2020},
	{4570,0x2424},
	{5330,0x2828},
	{6400,0x2C2C},
	{8000,0x3030},
};

/*==================================================================*/
 static SOCKET AR0330_WriteBuf(TAR0330Info *ptInfo, EAR0330Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[3];

	abyData[0] = (BYTE)(eRegAddr&0x00FF);
	abyData[1] = (BYTE)((dwData&0xFF00)>>8);
	abyData[2] = (BYTE)(dwData&0x00FF);
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), 3, abyData);
}

/*-------------------------------------------------------------------------------------*/ 
static SOCKET AR0330_ReadBuf(TAR0330Info *ptInfo, EAR0330Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen)
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
static SOCKET AR0330_CompBuf(TAR0330Info *ptInfo, EAR0330Regs eRegAddr, DWORD dwMask, DWORD dwData)
{
	DWORD dwReadData;

	AR0330_ReadBuf(ptInfo, eRegAddr, &dwReadData, 1);
	return ((dwReadData&dwMask) == dwData);
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0330_SetSize(DWORD dwWidth, DWORD dwHeight)
{
	if ((dwWidth==2304) && (dwHeight==1536))
	{
		AR0330_WriteBuf(ptInfo, _302A_VtPixClkDiv, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _302C_VtSysClkDiv, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _302E_PrePllClkDiv, 0x0009, 1, 0);
		AR0330_WriteBuf(ptInfo, _3030_PllMultiplier, 0x00C4, 1, 0);
		AR0330_WriteBuf(ptInfo, _3036_OpPixClkDiv, 0x000C, 1, 0);
		AR0330_WriteBuf(ptInfo, _3038_OpSysClkDiv, 0x0001, 1, 0);

		AR0330_WriteBuf(ptInfo, _3042_ExtraDelay, 0x0280, 1, 0);

		AR0330_WriteBuf(ptInfo, _3012_CoarseIntegrationTime, 0x051C, 1, 0);

		AR0330_WriteBuf(ptInfo, _3002_YAddrStart, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _3004_XAddrStart, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _3006_YAddrEnd, 0x0605, 1, 0);
		AR0330_WriteBuf(ptInfo, _3008_XAddrEnd, 0x0905, 1, 0);

		AR0330_WriteBuf(ptInfo, _3040_ReadMode, 0x0000, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A2_XOddInc, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A6_YOddInc, 0x0001, 1, 0);

		AR0330_PIXEL_CLOCK_M_100 = (98000000/10000);
		ptInfo->dwWidth = dwWidth;
		ptInfo->dwHeight = dwHeight;
	}
	else if ((dwWidth==2304) && (dwHeight==1296))
	{
		AR0330_WriteBuf(ptInfo, _302A_VtPixClkDiv, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _302C_VtSysClkDiv, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _302E_PrePllClkDiv, 0x0009, 1, 0);
		AR0330_WriteBuf(ptInfo, _3030_PllMultiplier, 0x00C4, 1, 0);
		AR0330_WriteBuf(ptInfo, _3036_OpPixClkDiv, 0x000C, 1, 0);
		AR0330_WriteBuf(ptInfo, _3038_OpSysClkDiv, 0x0001, 1, 0);

		AR0330_WriteBuf(ptInfo, _3042_ExtraDelay, 0x03B5, 1, 0);

		AR0330_WriteBuf(ptInfo, _3012_CoarseIntegrationTime, 0x051C, 1, 0);

		AR0330_WriteBuf(ptInfo, _3002_YAddrStart, 0x007E, 1, 0);
		AR0330_WriteBuf(ptInfo, _3004_XAddrStart, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _3006_YAddrEnd, 0x058D, 1, 0);
		AR0330_WriteBuf(ptInfo, _3008_XAddrEnd, 0x0905, 1, 0);

		AR0330_WriteBuf(ptInfo, _3040_ReadMode, 0x0000, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A2_XOddInc, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A6_YOddInc, 0x0001, 1, 0);

		AR0330_PIXEL_CLOCK_M_100 = (98000000/10000);
		ptInfo->dwWidth = dwWidth;
		ptInfo->dwHeight = dwHeight;
	}
	else if ((dwWidth==1920) && (dwHeight==1080))
	{
		AR0330_WriteBuf(ptInfo, _302A_VtPixClkDiv, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _302C_VtSysClkDiv, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _302E_PrePllClkDiv, 0x0002, 1, 0);
		AR0330_WriteBuf(ptInfo, _3030_PllMultiplier, 0x0021, 1, 0);
		AR0330_WriteBuf(ptInfo, _3036_OpPixClkDiv, 0x000C, 1, 0);
		AR0330_WriteBuf(ptInfo, _3038_OpSysClkDiv, 0x0001, 1, 0);

		//enable shortening sequence
		AR0330_WriteBuf(ptInfo, _3088_SeqCtrlPort, 0x80BA, 1, 0);
		AR0330_WriteBuf(ptInfo, _3086_SeqDataPort, 0x0253, 1, 0);

		AR0330_WriteBuf(ptInfo, _3042_ExtraDelay, 0x00A0, 1, 0);

		AR0330_WriteBuf(ptInfo, _3012_CoarseIntegrationTime, 0x0118, 1, 0);

		AR0330_WriteBuf(ptInfo, _3002_YAddrStart, 0x00E6, 1, 0);
		AR0330_WriteBuf(ptInfo, _3004_XAddrStart, 0x00C2, 1, 0);
		AR0330_WriteBuf(ptInfo, _3006_YAddrEnd, 0x0525, 1, 0);
		AR0330_WriteBuf(ptInfo, _3008_XAddrEnd, 0x0849, 1, 0);

		AR0330_PIXEL_CLOCK_M_100 = (74250000/10000);

		AR0330_WriteBuf(ptInfo, _3040_ReadMode, 0x0000, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A2_XOddInc, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A6_YOddInc, 0x0001, 1, 0);

		ptInfo->dwWidth = dwWidth;
		ptInfo->dwHeight = dwHeight;
	}
	else if ((dwWidth==1152) && (dwHeight==648))
	{
		AR0330_WriteBuf(ptInfo, _302A_VtPixClkDiv, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _302C_VtSysClkDiv, 0x0001, 1, 0);
		AR0330_WriteBuf(ptInfo, _302E_PrePllClkDiv, 0x0009, 1, 0);
		AR0330_WriteBuf(ptInfo, _3030_PllMultiplier, 0x00C4, 1, 0);
		AR0330_WriteBuf(ptInfo, _3036_OpPixClkDiv, 0x000C, 1, 0);
		AR0330_WriteBuf(ptInfo, _3038_OpSysClkDiv, 0x0001, 1, 0);

		AR0330_WriteBuf(ptInfo, _3042_ExtraDelay, 0x03B5, 1, 0);

		AR0330_WriteBuf(ptInfo, _3012_CoarseIntegrationTime, 0x051C, 1, 0);

		AR0330_WriteBuf(ptInfo, _3002_YAddrStart, 0x007E, 1, 0);
		AR0330_WriteBuf(ptInfo, _3004_XAddrStart, 0x0006, 1, 0);
		AR0330_WriteBuf(ptInfo, _3006_YAddrEnd, 0x058D, 1, 0);
		AR0330_WriteBuf(ptInfo, _3008_XAddrEnd, 0x0905, 1, 0);

		AR0330_WriteBuf(ptInfo, _3040_ReadMode, 0x0000, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A2_XOddInc, 0x0003, 1, 0);
		AR0330_WriteBuf(ptInfo, _30A6_YOddInc, 0x0003, 1, 0);

		AR0330_PIXEL_CLOCK_M_100 = (98000000/10000);
		ptInfo->dwWidth = dwWidth;
		ptInfo->dwHeight = dwHeight;
	}
	else
	{
		printk("[AR0330] : Size is not supported (width=%d, height=%d).\n", (int)ptInfo->dwWidth, (int)ptInfo->dwHeight);
	}
	return 0;
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0330_SetFrequence(TAR0330Info *ptInfo)
{
	if ((ptInfo->dwWidth==2304) && (ptInfo->dwHeight==1536))
	{
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0330Hsync = AR0330_HSYNC_2304x1536_15HZ;
			dwAR0330Vsync = AR0330_VSYNC_2304x1536_15HZ;
		}
		else
		{
			dwAR0330Hsync = AR0330_HSYNC_2304x1536_25HZ;
			dwAR0330Vsync = AR0330_VSYNC_2304x1536_25HZ;
		}
	}
	else if ((ptInfo->dwWidth==2304) && (ptInfo->dwHeight==1296))
	{
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0330Hsync = AR0330_HSYNC_2304x1296_30HZ;
			dwAR0330Vsync = AR0330_VSYNC_2304x1296_30HZ;
		}
		else
		{
			dwAR0330Hsync = AR0330_HSYNC_2304x1296_25HZ;
			dwAR0330Vsync = AR0330_VSYNC_2304x1296_25HZ;
		}
	}
	else if ((ptInfo->dwWidth==1152) && (ptInfo->dwHeight==648))
	{
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0330Hsync = AR0330_HSYNC_1152x648_30HZ;
			dwAR0330Vsync = AR0330_VSYNC_1152x648_30HZ;
		}
		else
		{
			dwAR0330Hsync = AR0330_HSYNC_1152x648_25HZ;
			dwAR0330Vsync = AR0330_VSYNC_1152x648_25HZ;
		}
	}else
	{
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0330Hsync = AR0330_HSYNC_1920x1080_30HZ;
			dwAR0330Vsync = AR0330_VSYNC_1920x1080_30HZ;
		}
		else
		{
			dwAR0330Hsync = AR0330_HSYNC_1920x1080_25HZ;
			dwAR0330Vsync = AR0330_VSYNC_1920x1080_25HZ;
		}
	}

	AR0330_WriteBuf(ptInfo, _300A_FrameLengthLines, dwAR0330Vsync, 1, 0); // FRAME_LENGTH_LINES
	AR0330_WriteBuf(ptInfo, _300C_LineLengthPck, (dwAR0330Hsync>>1), 1, 0); // LINE_LENGTH_PCK

	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void AR0330_Reset(TAR0330Info *ptInfo)
{
	AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x10DD, 1, 0);
	mdelay(100);

	AR0330_WriteBuf(ptInfo, _31AE_SerialFormat, 0x0301, 1, 0);
	AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x0058, 1, 0);
	AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x0058, 1, 0);
	mdelay(100);
	
	AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x00D8, 1, 0);
	AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x10D8, 1, 0);
	AR0330_WriteBuf(ptInfo, _3064_SmiaTest, 0x1802, 1, 0);
	AR0330_WriteBuf(ptInfo, _3078_TestDataGb, 0x0001, 1, 0);
	AR0330_WriteBuf(ptInfo, _3046_Flash, 0x4038, 1, 0);
	AR0330_WriteBuf(ptInfo, _3048_Flash2, 0x8480, 1, 0);

	// slew rate
	AR0330_WriteBuf(ptInfo, _306E_DatapathSelect, 0xFC10, 1, 0);

	//enable 2D defect pixel correction
	AR0330_WriteBuf(ptInfo, 0x31E0, 0x0200, 1, 0);

	// enable dither
	AR0330_WriteBuf(ptInfo, _30BA_DigitalCtrl, 0x002C, 1, 0);

	AR0330_WriteBuf(ptInfo, _3014_FineIntegrationTime, 0x0000, 1, 0);

	AR0330_WriteBuf(ptInfo, _301E_DataPedestal, 0x0020, 1, 0);	//Black Level

	ptInfo->dwAnalogGain = 0xFFFF;
	ptInfo->dwDigitalGain = 0xFFFF;

	printk("AR0330_Reset Done \n");
}

/*-------------------------------------------------------------------------------------*/
void AR0330_SetShutter(DWORD dwShutter)
{
	DWORD dwValue;
	QWORD qwValue;

	qwValue = dwShutter * AR0330_PIXEL_CLOCK_M_100;
	do_div(qwValue, 100);
	do_div(qwValue, dwAR0330Hsync);
	dwValue = (DWORD)qwValue;

	if (dwValue > 0x0000FFFF)
	{
		dwValue = 0x0000FFFF;
	}
	else if (dwValue < 10)
	{
		dwValue = 10;
	}

	AR0330_WriteBuf(ptInfo, 0x3012, dwValue, 1, 0);
	ptInfo->dwCurShutter = dwShutter;
}

/*-------------------------------------------------------------------------------------*/
void AR0330_SetGain(DWORD dwGain)
{
	DWORD dwAnalogGain = 0, dwDigitalGain = 0, dwTblIdx, dwIndex;

	dwGain = (dwGain > AR0330_MAX_GAIN) ?
			AR0330_MAX_GAIN :
			(dwGain < 1000) ?
			1000 :
			dwGain;

	if (dwGain < 2000)
		dwTblIdx = 0;
	else if (dwGain < 4000)
		dwTblIdx = 16;
	else
		dwTblIdx = 24;

	for (dwIndex=dwTblIdx; dwIndex<29; dwIndex++)
	{
		if (dwGain <= adwAR0330AnalogGainTbl[dwIndex][0])
			break;
	}

	dwIndex = (dwIndex==0) ? 0 : (dwIndex-1);

	dwAnalogGain = adwAR0330AnalogGainTbl[dwIndex][1];
	dwDigitalGain = (dwGain<<7)/adwAR0330AnalogGainTbl[dwIndex][0];


	if (ptInfo->dwDigitalGain != dwDigitalGain)
	{
		AR0330_WriteBuf(ptInfo, 0x305E, (dwDigitalGain&0xFFFF), 1, 0); //D Gain
		ptInfo->dwDigitalGain = dwDigitalGain;
	}
	if (ptInfo->dwAnalogGain != dwAnalogGain)
	{
		AR0330_WriteBuf(ptInfo, 0x3060, (dwAnalogGain&0x003F), 1, 0); //A Gain
		ptInfo->dwAnalogGain = dwAnalogGain;
	}
}

/*==================================================================*/
SOCKET  AR0330_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET SCKRet = 0;

	if (down_interruptible(&AR0330_mutex))
	{
		return -ERESTARTSYS;
	}

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		AR0330_Reset(ptInfo);
		if (AR0330_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
		{
			printk("[AR0330] : Reset width/height error !!\n");
			SCKRet =  -EPERM;
		}
		else
		{
			AR0330_SetFrequence(ptInfo);
		}
		// Start streaming	
		AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x10DC, 1, 0);
		break;
		case VIDEO_SIGNAL_OPTION_SET_SIZE:
		AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x10D8, 1, 0);
		if (AR0330_SetSize(ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]) < 0)
		{
			printk("[AR0330] : Size is not supported (width=%ld, height=%ld).\n", ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]);
			SCKRet = -EPERM;
		}
		else
		{
			AR0330_SetFrequence(ptInfo);
		}
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		AR0330_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		AR0330_SetGain(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[AR0330] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			SCKRet = -EPERM;
			break;
		}
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
		AR0330_SetFrequence(ptInfo);
		break;
		default:
		SCKRet = -EPERM;
	}
	up(&AR0330_mutex);

	return SCKRet;
}

/*-------------------------------------------------------------------------------------*/
void AR0330_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0330_Open(TVideoSensorInitialParam* ptAR0330InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptAR0330InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(AR0330_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptAR0330InitialParam->dwVideoSensorVersion&0x0000FF00)>(AR0330_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid AR0330 device driver version %d.%d.%d.%d !!\n",
				(int)(AR0330_INTERFACE_VERSION&0xFF),
				(int)((AR0330_INTERFACE_VERSION>>8)&0xFF),
				(int)((AR0330_INTERFACE_VERSION>>16)&0xFF),
				(int)(AR0330_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TAR0330Info *)kmalloc(sizeof(TAR0330Info), GFP_KERNEL)))
	{
		printk("[AR0330] : Allocate %d bytes memory fail\n", sizeof(TAR0330Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 30;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[AR0330] : Initial IICCtrl object fail !!\n");
		AR0330_Release(dwDevNum);
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = AR0330_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = (DWORD)ptAR0330InitialParam->dwVideoSensorFrequency;
	ptInfo->dwCurShutter = 33333;
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;

	// Detect sensor device
	if (!AR0330_CompBuf(ptInfo, _3000_ChipVersion, 0xFFFF, AR0330_CHIP_VERSION))
	{
		printk("\n[AR0330] : Sensor device doesn't exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
		AR0330_Release(dwDevNum);
		return -ENODEV;
	}
	else
	{
		printk("\n[AR0330] : Sensor device exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
	}

	// Reset device
	AR0330_Reset(ptInfo);

	// init captured size
	if (AR0330_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
	{
		printk("[AR0330] : initial width/height error !!\n");
		AR0330_Release(dwDevNum);
		return -EPERM;
	}
	AR0330_SetFrequence(ptInfo);

	// RESET_REGISTER	
	AR0330_WriteBuf(ptInfo, _301A_ResetRegister, 0x10DC, 1, 0);

	return 0;
}

/*-------------------------------------------------------------------------------------*/
DWORD AR0330_GetMaxGain(void)
{
	return (AR0330_MAX_GAIN);
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	AR0330_Open,
	.release = 	AR0330_Release,
	.ioctl = 	AR0330_Ioctl,
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,	
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = AR0330_GetMaxGain,
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct AR0330_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t AR0330_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct AR0330_attribute *AR0330_attr = container_of(attr, struct AR0330_attribute, attr);
	int result = 0;
	
	if (AR0330_attr->show)
		result = AR0330_attr->show(kobj, buf);	
	return result;
}

ssize_t AR0330_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "AR0330");
	return retval;
}

ssize_t AR0330_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t AR0330_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "2304");
	return retval;
}

ssize_t AR0330_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1536");
	return retval;
}

/*-------------------------------------------------------------------------------------*/
struct kobject AR0330_kobj;

struct AR0330_attribute AR0330_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0330_SensorName_attr_show,
};

struct AR0330_attribute AR0330_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0330_dwBufNum_attr_show,
};

struct AR0330_attribute AR0330_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0330_MaxFrameWidth_attr_show,
};

struct AR0330_attribute AR0330_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0330_MaxFrameHeight_attr_show,
};

struct sysfs_ops AR0330_sysfs_ops = {
	.show	= AR0330_default_attr_show,
};

struct kobj_type AR0330_ktype = {
	.sysfs_ops	= &AR0330_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET AR0330_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[AR0330] : kobj set name fail!\n");
		return -1;
	}
	
	printk(KERN_INFO "AR0330() sensor_width=%d sensor_height=%d\n", video_sensor_width, video_sensor_height);
	AR0330_kobj.ktype = &AR0330_ktype;	
	sckResult = kobject_init_and_add(&AR0330_kobj, &AR0330_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[AR0330] : Cannot register kobject [AR0330_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0330_kobj, &AR0330_SensorName_attr.attr);
	if (sckResult < 0)
	{
		printk("[AR0330] : Cannot create AR0330_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&AR0330_kobj, &AR0330_dwBufNum_attr.attr);
	if (sckResult < 0)
	{
		printk("[AR0330] : Cannot create AR0330_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0330_kobj, &AR0330_MaxFrameWidth_attr.attr);	
	if (sckResult < 0)
	{
		printk("[AR0330] : Cannot create AR0330_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0330_kobj, &AR0330_MaxFrameHeight_attr.attr);
	if (sckResult < 0)
	{
		printk("[AR0330] : Cannot create AR0330_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&AR0330_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void AR0330_Exit(void)
{
	sysfs_remove_file(&AR0330_kobj, &(AR0330_SensorName_attr.attr));
	sysfs_remove_file(&AR0330_kobj, &(AR0330_dwBufNum_attr.attr));	
	sysfs_remove_file(&AR0330_kobj, &(AR0330_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&AR0330_kobj, &(AR0330_MaxFrameHeight_attr.attr));
	kobject_put(&AR0330_kobj);	
	printk(KERN_DEBUG "[AR0330] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(AR0330_Init);
module_exit(AR0330_Exit);
