/*
 * MT9P031
 * Driver for MT9P031 sensor.
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

#include "MT9P031.h"
#include "MT9P031_locals.h"
#include "IICCtrl.h"
/* Version 1.0.0.4 modification, 2010.08.02 */
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()
/* ========================================= */

const CHAR MT9P031_ID[] = "$Version: "MT9P031_ID_VERSION"  (MT9P031 DRIVER) $";
/*==================================================================*/
static TMT9P031Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore mt9p031_mutex;

/* Version 2.0.0.0 modification, 2011.01.21 */
static DWORD dwMT9P031Hsync = MT9P031_HSYNC_1920x1080;
/* ======================================== */

/* Version 1.0.0.4 modification, 2010.08.02 */
/* Sensor capture width */
static SOCKET video_sensor_width = 1920;
/* Sensor capture height */
static SOCKET video_sensor_height = 1080;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "MT9P031 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "MT9P031 capture height");

/* ========================================= */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("MT9P031 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TMT9P031RegAddrData atMT9P031regProgressRawTbl[MT9P031_PROGRESS_RAW_TBL_SIZE] = 
{
	{_03_RowSize					, 0x0437},
	{_04_ColumnSize				, 0x0781},
/* Version 1.0.0.7 modification, 2010.11.01 */
	{_05_HorzBlanking				, 0x0001}, 
	{_06_VertBlanking				, 0x0035}, // defaule 0x0019
/* ========================================= */
	{_09_ShutterWidthLower			, MT9P031_SHUTTER_WIDTH_LOWER},
	{_10_PLLCtrl					, 0x0051},
	{_11_PLLConfigl					, 0x3004},	// M: 0x30, N: 0x04 (M:16~255, N: 0~63)
	{_12_PLLConfig2				, 0x0001}	// P1: 0x01 (P1: 0~127)	
};


/*==================================================================*/
static SOCKET MT9P031_WriteReg(TMT9P031Info *ptInfo, EMT9P031Regs eRegAddr, DWORD dwData)
{
	return ptInfo->pfnWriteReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, 0, dwData);
}

/*-------------------------------------------------------------------------------------*/
#if 0 // unused function
static SOCKET MT9P031_read_reg(TMT9P031Info *ptInfo, EMT9P031Regs eRegAddr, DWORD *pdwData)
{
	return ptInfo->pfnReadReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, 0, pdwData);
}

/*-------------------------------------------------------------------------------------*/
static SOCKET MT9P031_comp_reg(TMT9P031Info *ptInfo, EMT9P031Regs eRegAddr, DWORD dwMask, DWORD dwData)
{
	DWORD dwReadData;

	ptInfo->pfnReadReg(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, eRegAddr, 0, &dwReadData);

	return ((dwReadData&dwMask) == dwData);
}
#endif
/*-------------------------------------------------------------------------------------*/
static void MT9P031_Reset(TMT9P031Info *ptInfo)
{
	DWORD dwIndex;

	MT9P031_WriteReg(ptInfo, _0D_Reset, 0xFFFF);
	MT9P031_WriteReg(ptInfo, _0D_Reset, 0x0000);

	for (dwIndex=0; dwIndex<MT9P031_PROGRESS_RAW_TBL_SIZE; dwIndex++)
	{
		MT9P031_WriteReg(ptInfo, atMT9P031regProgressRawTbl[dwIndex].eRegAddr, atMT9P031regProgressRawTbl[dwIndex].dwData);
	}
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_50HZ)
	{
		MT9P031_WriteReg(ptInfo, _09_ShutterWidthLower, 1361);
	}
/* Version 2.0.0.4 modification, 2012.11.23 */
	MT9P031_WriteReg(ptInfo, 0x49, 0x0028);
/* ======================================== */

	udelay(1000);
	MT9P031_WriteReg(ptInfo, _10_PLLCtrl, 0x0053);
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
void MT9P031_SetShutter(DWORD dwShutter)
{
	DWORD dwValue, dwShutterWidthLower, dwShutterWidthUpper;

	dwValue = (dwShutter*MT9P031_PIXEL_CLOCK_M) / dwMT9P031Hsync;

	if (dwValue > 0x0000FFFF)
	{
		dwValue = 0x0000FFFF;
	}
	else if (dwValue < 2)
	{
		dwValue = 2;
	}
/* ======================================== */

	dwShutterWidthLower = (dwValue & 0x0000FFFF);
	dwShutterWidthUpper = (dwValue >> 16);
	MT9P031_WriteReg(ptInfo, _09_ShutterWidthLower, dwShutterWidthLower);
	if (dwShutterWidthUpper != 0)
	{
		MT9P031_WriteReg(ptInfo, _08_ShutterWidthUpper, dwShutterWidthUpper);
	}
	
/* Version 2.0.0.2 modification, 2012.02.09 */
	ptInfo->dwShutter = dwShutter;
/* ======================================== */
}

/*-------------------------------------------------------------------------------------*/

/* Version 2.0.0.0 modification, 2011.01.21 */
void MT9P031_SetGain(DWORD dwGain)
{
	DWORD dwValue = 0, dwAnalogGain, dwDigitalGain;
	BOOL  dwAnalogMutipier;

/* Version 2.0.0.3 modification, 2012.08.06 */
	if (dwGain > MT9P031_MAX_GAIN)
	{
		dwGain = MT9P031_MAX_GAIN;
/* ======================================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	if (dwGain <= 4000)
	{
		dwAnalogGain = (dwGain*8) / 1000;
		dwAnalogMutipier = 0;
		dwDigitalGain = 0;
	}
	else if (dwGain <= 8000)
	{
		dwAnalogGain = ((dwGain*8)/2) / 1000;
		dwAnalogMutipier = 1;
		dwDigitalGain = 0;    
	}
	else
	{
		dwAnalogGain = 32;
		dwAnalogMutipier = 1;
		dwDigitalGain = (dwGain-8000) / 1000;     
	}
	dwValue |= dwAnalogGain;
	dwValue |= (dwAnalogMutipier<<6);
	dwValue |= (dwDigitalGain<<8);
/* ======================================== */
	MT9P031_WriteReg(ptInfo, _35_GlobalGain, (dwValue&0x0000FFFF));
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.1 modification, 2012.01.05 */
SOCKET MT9P031_SetFrequence(void)
{
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_50HZ)
	{
		if ((ptInfo->dwWidth==2560) && (ptInfo->dwHeight==1920))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 381);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==1920) && (ptInfo->dwHeight==1080))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 271);
/* ======================================== */
		}			
		else if ((ptInfo->dwWidth==1600) && (ptInfo->dwHeight==1200))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 325);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==1024))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 726);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 625);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==800) && (ptInfo->dwHeight==600))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 1022);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 590);
/* ======================================== */
		}
	}
	else
	{
		if ((ptInfo->dwWidth==2560) && (ptInfo->dwHeight==1920))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 17);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==1920) && (ptInfo->dwHeight==1080))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 44);
/* ======================================== */
		}			
		else if ((ptInfo->dwWidth==1600) && (ptInfo->dwHeight==1200))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 69);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==1024))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 433);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 400);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==800) && (ptInfo->dwHeight==600))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 750);
/* ======================================== */
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
		{
/* Version 2.0.0.3 modification, 2012.08.06 */
			MT9P031_WriteReg(ptInfo, _06_VertBlanking, 410);
/* ======================================== */
		}
	}
	return 0;
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
SOCKET MT9P031_SetSize(DWORD dwWidth, DWORD dwHeight)
{
	if ((dwWidth==2560) && (dwHeight==1920))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x0786);
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0A00);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0000);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0000);
/* Version 1.0.0.8 modification, 2010.11.01 */
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 32); // (2592-2560)/2 + 16
		MT9P031_WriteReg(ptInfo, _01_RowStart, 66); // (1944-1920)/2 + 54
/* ========================================= */	
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_2560x1920;
/* ======================================== */
	}
	else if ((dwWidth==1920) && (dwHeight==1080))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x043E); //0x0437
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0780);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0000);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0000);
/* Version 1.0.0.8 modification, 2010.11.01 */
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 352); // (2592-1920)/2 +16
		MT9P031_WriteReg(ptInfo, _01_RowStart, 486); // (1944-1080)/2 + 54
/* ========================================= */	
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_1920x1080;
/* ======================================== */
	}			
	else if ((dwWidth==1600) && (dwHeight==1200))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x04B6);
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0640);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0000);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0000);
/* Version 1.0.0.8 modification, 2010.11.01 */
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 512); // (2592-1600)/2 + 16
		MT9P031_WriteReg(ptInfo, _01_RowStart, 426); // (1944-1200)/2 + 54
/* ========================================= */	
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_1600x1200;
/* ======================================== */
	}
	else if ((dwWidth==1280) && (dwHeight==1024))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x0406);
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0500);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0000);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0000);
/* Version 1.0.0.8 modification, 2010.11.01 */
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 672); // (2592-1280)/2 + 16
		MT9P031_WriteReg(ptInfo, _01_RowStart, 514); // (1944-1024)/2 + 54
/* ========================================= */	
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_1280x1024;
/* ======================================== */
	}
	else if ((dwWidth==1280) && (dwHeight==720))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x05AC);
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0A00);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0011);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0011);
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 16);
		MT9P031_WriteReg(ptInfo, _01_RowStart, 294);
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_1280x720;
/* ======================================== */
	}
	else if ((dwWidth==800) && (dwHeight==600))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x04BC);
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0640);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0011);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0011);
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 496);
		MT9P031_WriteReg(ptInfo, _01_RowStart, 414);
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_800x600;
/* ======================================== */
	}
	else if ((dwWidth==640) && (dwHeight==480))
	{
/* Version 2.0.0.3 modification, 2012.08.06 */
		MT9P031_WriteReg(ptInfo, _03_RowSize, 0x0798);
		MT9P031_WriteReg(ptInfo, _04_ColumnSize, 0x0A00);
/* ======================================== */
		MT9P031_WriteReg(ptInfo, _22_RowAddrMode, 0x0033);
		MT9P031_WriteReg(ptInfo, _23_ColumnAddrMode, 0x0033);
		MT9P031_WriteReg(ptInfo, _02_ColumnStart, 16);
		MT9P031_WriteReg(ptInfo, _01_RowStart, 54);
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwMT9P031Hsync = MT9P031_HSYNC_640x480;
/* ======================================== */
	}
	else	
	{
		return -1;			
	}
	ptInfo->dwWidth = dwWidth;
	ptInfo->dwHeight = dwHeight;		
	return 0;
}

/*==================================================================*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET  MT9P031_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
/* ========================================= */
{
	SOCKET SCKRet = 0;

/* Version 1.0.0.4 modification, 2010.08.02 */
	if (down_interruptible(&mt9p031_mutex))
	{
		return -ERESTARTSYS;	
	}
/* ========================================= */

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		MT9P031_Reset(ptInfo);
/* Version 2.0.0.1 modification, 2012.01.05 */
		MT9P031_SetFrequence();
/* ========================================= */
		break;
		case VIDEO_SIGNAL_OPTION_SET_SIZE:
		// 1. set size
		if (MT9P031_SetSize(ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]) < 0)
		{
			printk("[MT9P031] : Size is not supported (width=%ld, height=%ld).\n", ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]);			
			SCKRet = -EPERM;
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ========================================= */
		}
/* Version 2.0.0.1 modification, 2012.01.05 */
		else
		{
			MT9P031_SetFrequence();
/* Version 2.0.0.2 modification, 2012.02.09 */
			MT9P031_SetShutter(ptInfo->dwShutter);
/* ======================================== */
		}
/* ======================================== */
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 1.0.0.5 modification, 2010.08.18 */
/* ======================================== */
/* ========================================= */
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		MT9P031_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		MT9P031_SetGain(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[MT9P031] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			SCKRet = -EPERM;
			break;
		}
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 1.0.0.5 modification, 2010.08.18 */
/* ======================================== */
/* ========================================= */
/* Version 2.0.0.1 modification, 2012.01.05 */
		MT9P031_SetFrequence();
/* ========================================= */
/* Version 2.0.0.2 modification, 2012.02.09 */
		MT9P031_SetShutter(ptInfo->dwShutter);
/* ======================================== */
		break;
		default:
		SCKRet = -EPERM;
	}
/* Version 1.0.0.4 modification, 2010.08.02 */
	up(&mt9p031_mutex);	
/* ========================================= */

	return SCKRet;
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
void MT9P031_Release(DWORD dwDevNum)
/* ========================================= */
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET MT9P031_Open(TVideoSensorInitialParam* ptMT9P031InitialParam, DWORD dwDevNum)
/* ========================================= */
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

/* Version 2.0.0.0 modification, 2011.01.21 */
	if (((ptMT9P031InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(MT9P031_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptMT9P031InitialParam->dwVideoSensorVersion&0x0000FF00)>(MT9P031_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid MT9P031 device driver version %d.%d.%d.%d !!\n", 
				(int)(MT9P031_INTERFACE_VERSION&0xFF), 
				(int)((MT9P031_INTERFACE_VERSION>>8)&0xFF), 
				(int)((MT9P031_INTERFACE_VERSION>>16)&0xFF), 
				(int)(MT9P031_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

	if (!(ptInfo=(TMT9P031Info *)kmalloc(sizeof(TMT9P031Info), GFP_KERNEL)))
	{
		printk("[MT9P031] : Allocate %d bytes memory fail\n", sizeof(TMT9P031Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_MICRON_16_BITS_TYPE_0;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[MT9P031] : Initial IICCtrl object fail !!\n");
/* Version 1.0.0.1 modification, 2010.06.01 */
		MT9P031_Release(dwDevNum);			
/* ========================================= */			
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = MT9P031_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadReg = (FOnReadReg)IICCtrl_ReadWordReg;
	ptInfo->pfnWriteReg = (FOnWriteReg)IICCtrl_WriteWordReg;
/* Version 1.0.0.4 modification, 2010.08.02 */
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;
/* ========================================= */
	ptInfo->dwFreq = ptMT9P031InitialParam->dwVideoSensorFrequency;
/* Version 2.0.0.2 modification, 2012.02.09 */
	ptInfo->dwShutter = 33333;
/* ======================================== */
	 
/* Version 1.0.0.1 modification, 2010.06.01 */
/* ========================================= */
	
	// init shutter table
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 1.0.0.5 modification, 2010.08.18 */
/* ======================================== */
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	// Detect device
/* Version 1.0.0.1 modification, 2010.06.01 */
#if 0 //[FIXME] : sometimes cannot get correct VERSION_ID
	if (!MT9P031_comp_reg(ptInfo, _00_ChipVersion, 0xFFFF, MT9P031_CHIP_VERSION))
	{
		printk("[MT9P031] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
		MT9P031_Release(dwDevNum);				
		return -ENODEV;
	}
#endif	
/* ========================================= */
	// Reset device
	MT9P031_Reset(ptInfo);
/* Version 1.0.0.1 modification, 2010.06.01 */
	// init captured size
/* Version 1.0.0.8 modification, 2010.11.01 */
/* ========================================= */	
	if (MT9P031_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
	{
		printk("[MT9P031] : initial width/height error !!\n");			
		MT9P031_Release(dwDevNum);				
		return -EPERM;
	}
/* Version 2.0.0.1 modification, 2012.01.05 */
	MT9P031_SetFrequence();
/* ========================================= */
/* ========================================= */
	return 0;
}

/* Version 2.0.0.3 modification, 2012.08.06 */
/*-------------------------------------------------------------------------------------*/
DWORD MT9P031_GetMaxGain(void)
{
	return (MT9P031_MAX_GAIN);
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	MT9P031_Open,
	.release = 	MT9P031_Release,
	.ioctl = 	MT9P031_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	.get_shutter_value = NULL,
/* Version 1.0.0.2 modification, 2010.06.03 */	
	.get_remaining_line_num = NULL,
/* ========================================= */	
/* Version 1.0.0.3 modification, 2010.06.21 */
	.group_access = NULL,		
/* ========================== */	
/* Version 2.0.0.3 modification, 2012.08.06 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = MT9P031_GetMaxGain,
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct MT9P031_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t MT9P031_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct MT9P031_attribute *MT9P031_attr = container_of(attr, struct MT9P031_attribute, attr);
	int result = 0;
	
	if (MT9P031_attr->show)
		result = MT9P031_attr->show(kobj, buf);	
	return result;
}

ssize_t MT9P031_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "MT9P031");
	return retval;
}

ssize_t MT9P031_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t MT9P031_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "2560");
	return retval;
}

ssize_t MT9P031_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject MT9P031_kobj;

struct MT9P031_attribute MT9P031_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9P031_SensorName_attr_show,
};

struct MT9P031_attribute MT9P031_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9P031_dwBufNum_attr_show,
};

struct MT9P031_attribute MT9P031_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9P031_MaxFrameWidth_attr_show,
};

struct MT9P031_attribute MT9P031_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9P031_MaxFrameHeight_attr_show,
};

struct sysfs_ops MT9P031_sysfs_ops = {
	.show	= MT9P031_default_attr_show,
};

struct kobj_type MT9P031_ktype = {
	.sysfs_ops	= &MT9P031_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET MT9P031_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[MT9P031] : kobj set name fail!\n");
		return -1;
	}

	MT9P031_kobj.ktype = &MT9P031_ktype;	
	sckResult = kobject_init_and_add(&MT9P031_kobj, &MT9P031_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[MT9P031] : Cannot register kobject [MT9P031_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MT9P031_kobj, &MT9P031_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9P031] : Cannot create MT9P031_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&MT9P031_kobj, &MT9P031_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9P031] : Cannot create MT9P031_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MT9P031_kobj, &MT9P031_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9P031] : Cannot create MT9P031_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MT9P031_kobj, &MT9P031_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9P031] : Cannot create MT9P031_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
/* Version 1.0.0.4 modification, 2010.08.02 */
	sema_init(&mt9p031_mutex, 1);
/* ========================================= */
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void MT9P031_Exit(void)
{
	sysfs_remove_file(&MT9P031_kobj, &(MT9P031_SensorName_attr.attr));
	sysfs_remove_file(&MT9P031_kobj, &(MT9P031_dwBufNum_attr.attr));	
/* Version 1.0.0.3 modification, 2010.06.21 */
	sysfs_remove_file(&MT9P031_kobj, &(MT9P031_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&MT9P031_kobj, &(MT9P031_MaxFrameHeight_attr.attr));	
/* ========================== */	
	kobject_put(&MT9P031_kobj);	
	printk(KERN_DEBUG "[MT9P031] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(MT9P031_Init);
module_exit(MT9P031_Exit);
