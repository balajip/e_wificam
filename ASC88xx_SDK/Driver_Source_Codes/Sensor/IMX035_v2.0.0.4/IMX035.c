/*
 * IMX035
 * Driver for IMX035 sensor.
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

#include "IMX035.h"
#include "IMX035_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR IMX035_ID[] = "$Version: "IMX035_ID_VERSION"  (IMX035 DRIVER) $";
/*==================================================================*/
static TIMX035Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
/* Version 1.0.0.4 modification, 2010.08.02 */
static struct semaphore imx035_mutex;
/* ========================== */

/* Version 2.0.0.0 modification, 2011.01.21 */
static DWORD dwIMX035Hsync = IMX035_HSYNC_1280x1024_60Hz;
static DWORD dwIMX035Vsync = IMX035_VSYNC_1280x1024_60Hz;
/* ======================================== */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("IMX035 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TIMX035RegAddrData atIMX035regProgressRawTbl[IMX035_PROGRESS_RAW_TBL_SIZE] = {
	{_26_XMSTA,				0x01},
	{_00_Stby,				0x00},
	{_00_Stby,				0x0C},
	{_62_Reg62,				0x32},
	{_90_Reg90,				0x48},
	{_FE_RegFE,				0x08},
	{_01_Mode,				0x00},
	{_03_ADRES,				0x02},
	{_04_FRSEL,				0x13},
	{_09_SHS1H,				0x01},
	{_08_SHS1L,				0x0F},
	{_06_SVSL,				0x00},
	{_07_SVSH,				0x00},
/* Version 2.0.0.0 modification, 2011.01.21 */
	{_12_SPL1L,				0x00},
/* ======================================== */
	{_1E_DigitalGain,			0x00},
	{_26_XMSTA,				0x00},
/* Version 2.0.0.2 modification, 2011.09.16 */
	{_1F_BlackLevelL,			0x10}
/* ======================================== */
};

/*==================================================================*/
static BYTE IMX035_BitReverse(BYTE byData)
{
	BYTE byOut;

	byOut = ((byData<<7)&0x80) | ((byData<<5)&0x40) | ((byData<<3)&0x20) | ((byData<<1)&0x10) |
			((byData>>1)&0x08) | ((byData>>3)&0x04) | ((byData>>5)&0x02) | ((byData>>7)&0x01);

	return byOut;
}

/*-------------------------------------------------------------------------------------*/
static int IMX035_WriteReg(TIMX035Info *ptInfo, EIMX035Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BOOL bWaitCmpt)
{
	BYTE abyData[6];
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);

	byOutRegAddr = IMX035_BitReverse(IMX035_DEAFULT_DEVICE_ADDR);
	abyData[0] = IMX035_BitReverse((BYTE)eRegAddr);
	abyData[1] = IMX035_BitReverse(dwData&0xFF);
	abyData[2] = IMX035_BitReverse((dwData>>8)&0xFF);
	abyData[3] = IMX035_BitReverse((dwData>>16)&0xFF);
	abyData[4] = IMX035_BitReverse((dwData>>24)&0xFF);
	abyData[dwLength] = (BYTE)bWaitCmpt;

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, (DWORD)byOutRegAddr, (dwLength+1), abyData);
}

/*-------------------------------------------------------------------------------------*/
static void IMX035_Reset(TIMX035Info *ptInfo)
{
	DWORD dwIndex, dwFlags, dwMaxSize;

	IMX035_WriteReg(ptInfo, atIMX035regProgressRawTbl[0].eRegAddr, atIMX035regProgressRawTbl[0].dwData, 1, ptInfo->bInit);
	local_irq_save(dwFlags);
	for (dwIndex=1; dwIndex<IMX035_PROGRESS_RAW_TBL_SIZE-1; dwIndex++)
	{
		IMX035_WriteReg(ptInfo, atIMX035regProgressRawTbl[dwIndex].eRegAddr, atIMX035regProgressRawTbl[dwIndex].dwData, 1, 0);
	}
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
	{
		dwMaxSize = (0x64<<24) + (0x00<<16) + (0x04<<8) + 0x65;
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwIMX035Hsync = IMX035_HSYNC_1280x1024_60Hz;
		dwIMX035Vsync = IMX035_VSYNC_1280x1024_60Hz;	
/* ======================================== */
	}
	else 
	{
		dwMaxSize = (0x6C<<24) + (0x00<<16) + (0x04<<8) + 0xE2;
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwIMX035Hsync = IMX035_HSYNC_1280x1024_50Hz;
		dwIMX035Vsync = IMX035_VSYNC_1280x1024_50Hz;
/* ======================================== */
	}
 	IMX035_WriteReg(ptInfo, _7C_VMAXL, dwMaxSize, 4, 0);
 	IMX035_WriteReg(ptInfo, atIMX035regProgressRawTbl[IMX035_PROGRESS_RAW_TBL_SIZE-1].eRegAddr, atIMX035regProgressRawTbl[IMX035_PROGRESS_RAW_TBL_SIZE-1].dwData, 1, 0);
	mdelay(1000);
	local_irq_restore(dwFlags);	
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
static void IMX035_SetShutter(DWORD dwShutter)
{
/* Version 2.0.0.2 modification, 2011.09.16 */
	DWORD dwExpoFrame = 0, dwExpoLine = 0, dwExpSub = 0, dwMaxSize, dwSubLine;
	static DWORD dwExpoFrameDB = 0;
/* ======================================== */
	dwExpoLine = ((dwShutter*IMX035_PIXEL_CLOCK_M)+IMX035_DELTA_1280x1024_12BIT) / dwIMX035Hsync;

/* ======================================== */
/* Version 2.0.0.2 modification, 2011.09.16 */
	dwExpoFrame = dwExpoLine / dwIMX035Vsync;
	dwSubLine = dwExpoLine - (dwExpoFrame*dwIMX035Vsync);

	dwExpoFrame = (dwSubLine==0) ? dwExpoFrame : (dwExpoFrame+1);
	dwMaxSize = dwIMX035Vsync*dwExpoFrame;

	dwMaxSize = (dwMaxSize>=8192) ? 8192 : dwMaxSize;

	dwExpSub = dwMaxSize - dwExpoLine;
	dwExpSub = (dwExpoLine<=0) ? 0 : (dwExpSub>(dwIMX035Vsync-5)) ? (dwIMX035Vsync-5) : dwExpSub;
	//printk("S  0: %d %d %d\n", dwShutter, dwExpSub, dwExpoFrame);

	if (dwExpoFrame != dwExpoFrameDB)
	{
		IMX035_WriteReg(ptInfo, _7C_VMAXL, dwMaxSize, 2, 1);
		IMX035_WriteReg(ptInfo, _08_SHS1L, dwExpSub, 2, 0);
		dwExpoFrameDB = dwExpoFrame;
		//printk("S  0: %d %d %d\n", dwShutter, dwExpSub, dwExpoFrame);
	}else
	{
		IMX035_WriteReg(ptInfo, _08_SHS1L, dwExpSub, 2, 1);
		//printk("S 1: %d %d %d\n", dwShutter, dwExpSub, dwExpoFrame);
	}

	//ptInfo->dwCurShutter = (dwValue & 0x000003FF);	// shutter frame num
	//ptInfo->dwCurShutter = dwExpoLine;	// shutter frame num
	ptInfo->dwCurShutter = dwMaxSize/dwIMX035Vsync;	// shutter frame num
	//printk("Frame %d %d %d\n", ptInfo->dwCurShutter, dwMaxSize, dwExpoLine);
/* ======================================== */
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
static void IMX035_SetGain(DWORD dwGain)
{
	DWORD dwValue;

/* Version 2.0.0.3 modification, 2012.09.04 */
	if (dwGain > IMX035_MAX_GAIN)
	{
		dwGain = IMX035_MAX_GAIN;
/* ======================================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	if (dwGain <= 8000)
	{
		dwValue = 4096000 / dwGain;
	}
	else if (dwGain <= 16000)
	{
/* Version 2.0.0.2 modification, 2011.09.16 */
		dwValue = 0x150000 + (8192000/dwGain);
/* ======================================== */
	}
	else if (dwGain <= 32000)
	{
/* Version 2.0.0.2 modification, 2011.09.16 */
		dwValue = 0x2A0000 + (16384000/dwGain);
/* ======================================== */
	}
	else
	{
/* Version 2.0.0.2 modification, 2011.09.16 */
		dwValue = 0x3F0000 + (32768000/dwGain);
/* ======================================== */
	}
	dwValue = (dwValue&0x003F1FFF);
/* ======================================== */
	IMX035_WriteReg(ptInfo, _1C_AnalogGainL, dwValue, 3, 1);
}

/*==================================================================*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET  IMX035_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
/* ========================================= */
{
	SOCKET sckRet = 0;
	DWORD dwMaxSize;	

/* Version 1.0.0.4 modification, 2010.08.02 */
	if (down_interruptible(&imx035_mutex))
	{
		return -ERESTARTSYS;
	}
/* ========================== */	

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		IMX035_Reset(ptInfo);
		break;	
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		IMX035_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		IMX035_SetGain(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[IMX035.ko] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			sckRet = -EPERM;
			break;
		}
		if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
/* Version 2.0.0.2 modification, 2011.09.16 */
/* ======================================== */
/* Version 2.0.0.0 modification, 2011.01.21 */
			dwIMX035Hsync = IMX035_HSYNC_1280x1024_60Hz;
			dwIMX035Vsync = IMX035_VSYNC_1280x1024_60Hz;
/* ======================================== */
		}
		else 
		{
/* Version 2.0.0.2 modification, 2011.09.16 */
/* ======================================== */
/* Version 2.0.0.0 modification, 2011.01.21 */
			dwIMX035Hsync = IMX035_HSYNC_1280x1024_50Hz;
			dwIMX035Vsync = IMX035_VSYNC_1280x1024_50Hz;
/* ======================================== */
		}
/* Version 2.0.0.2 modification, 2011.09.16 */
		ptInfo->dwCurShutter = (ptInfo->dwCurShutter<=0) ? 1 : ptInfo->dwCurShutter;
		dwMaxSize = (dwIMX035Hsync<<20) + (dwIMX035Vsync*ptInfo->dwCurShutter);
/* ======================================== */
		IMX035_WriteReg(ptInfo, _7C_VMAXL, dwMaxSize, 4, 1);
/* Version 2.0.0.0 modification, 2011.01.21 */
		mdelay(40);
/* ======================================== */
		// update AE shutter table				
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 1.0.0.5 modification, 2010.08.18 */
/* ======================================== */
/* ========================================= */		
		break;
		default:
		sckRet = -EPERM;
	}
/* Version 1.0.0.4 modification, 2010.08.02 */
	up(&imx035_mutex);	
/* ========================== */

	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
DWORD IMX035_VICGetShutterValue(void)
{
/* Version 1.0.0.4 modification, 2010.08.02 */
	DWORD dwRet;

	if (down_interruptible(&imx035_mutex))
	{
		return -ERESTARTSYS;	
	}
	dwRet = ptInfo->dwCurShutter;
	up(&imx035_mutex);
	
	return dwRet;
/* ========================== */
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.2 modification, 2010.06.03 */
//workaround for VIC AE delay issue
DWORD IMX035_GetRemainingLineNum(void)
{
/* Version 2.0.0.0 modification, 2011.01.21 */
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
	{
		return (1042-1024);
	}
	else
	{
		return (1210-1024);
	}
/* ======================================== */
}
/* ========================================= */
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
void IMX035_Release(DWORD dwDevNum)
/* ========================================= */
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET IMX035_Open(TVideoSensorInitialParam* ptIMX035InitialParam, DWORD dwDevNum)
/* ========================================= */
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

/* Version 2.0.0.0 modification, 2011.01.21 */
	if (((ptIMX035InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(IMX035_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptIMX035InitialParam->dwVideoSensorVersion&0x0000FF00)>(IMX035_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid IMX035 device driver version %d.%d.%d.%d !!\n", 
				(int)(IMX035_INTERFACE_VERSION&0xFF), 
				(int)((IMX035_INTERFACE_VERSION>>8)&0xFF), 
				(int)((IMX035_INTERFACE_VERSION>>16)&0xFF), 
				(int)(IMX035_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

	if (!(ptInfo=(TIMX035Info *)kmalloc(sizeof(TIMX035Info), GFP_KERNEL)))
	{
		printk("[IMX035.ko] : Allocate %d bytes memory fail\n", sizeof(TIMX035Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_WRITEBUF_FAST_NOACK;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[IMX035.ko] : Initial IICCtrl object fail !!\n");
/* Version 1.0.0.1 modification, 2010.06.01 */
		IMX035_Release(dwDevNum);				
/* ========================================= */		
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = IMX035_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptIMX035InitialParam->dwVideoSensorFrequency;
	ptInfo->bInit = FALSE;
/* Version 2.0.0.2 modification, 2011.09.16 */
	ptInfo->dwCurShutter = 1;
/* ======================================== */

/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

	// Reset device
	IMX035_Reset(ptInfo);
	ptInfo->bInit = TRUE;	
	return 0;
}

/* Version 2.0.0.3 modification, 2012.09.04 */
/*-------------------------------------------------------------------------------------*/
DWORD IMX035_GetMaxGain(void)
{
	return (IMX035_MAX_GAIN);
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open =	IMX035_Open,
	.release =	IMX035_Release,
	.ioctl =	IMX035_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
/* Version 2.0.0.2 modification, 2011.09.16 */
	.get_shutter_value =	NULL,
/* Version 1.0.0.2 modification, 2010.06.03 */
	.get_remaining_line_num =	NULL,
/* ========================== */
/* ========================== */
/* Version 1.0.0.3 modification, 2010.06.21 */
	.group_access =	NULL,	
/* ========================== */
/* Version 2.0.0.3 modification, 2012.09.04 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = IMX035_GetMaxGain,
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct IMX035_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t IMX035_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct IMX035_attribute *IMX035_attr = container_of(attr, struct IMX035_attribute, attr);
	int result = 0;
	
	if (IMX035_attr->show)
		result = IMX035_attr->show(kobj, buf);	
	return result;
}

ssize_t IMX035_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "IMX035");
	return retval;
}

ssize_t IMX035_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "6");
	return retval;
}

ssize_t IMX035_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1280");
	return retval;
}

ssize_t IMX035_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
/* Version 1.0.0.2 modification, 2010.06.03 */
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 2.0.0.1 modification, 2011.05.17 */
	retval = sprintf (buf, "%s\n", "1210");
/* ======================================== */
/* ======================================== */
/* ========================== */
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject IMX035_kobj;

struct IMX035_attribute IMX035_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX035_SensorName_attr_show,
};

struct IMX035_attribute IMX035_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX035_dwBufNum_attr_show,
};

struct IMX035_attribute IMX035_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX035_MaxFrameWidth_attr_show,
};

struct IMX035_attribute IMX035_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX035_MaxFrameHeight_attr_show,
};

struct sysfs_ops IMX035_sysfs_ops = {
	.show	= IMX035_default_attr_show,
};

struct kobj_type IMX035_ktype = {
	.sysfs_ops	= &IMX035_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int IMX035_Init(void)
{
	int result = 0;
	
	if (result < 0)
	{
		printk("[IMX035] : kobj set name fail!\n");
		return -1;
	}

	IMX035_kobj.ktype = &IMX035_ktype;	
	result = kobject_init_and_add(&IMX035_kobj, &IMX035_ktype, NULL, "VideoSensorInfo");

	if (result < 0)
	{
		printk("[IMX035] : Cannot register kobject [IMX035_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&IMX035_kobj, &IMX035_SensorName_attr.attr);		
	if (result < 0) 
	{
		printk("[IMX035] : Cannot create IMX035_SensorName_attr.attr.\n");
		return -1;
       }

	result = sysfs_create_file(&IMX035_kobj, &IMX035_dwBufNum_attr.attr);		
	if (result < 0)
	{
		printk("[IMX035] : Cannot create IMX035_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX035_kobj, &IMX035_MaxFrameWidth_attr.attr);		
	if (result < 0)
	{
		printk("[IMX035] : Cannot create IMX035_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX035_kobj, &IMX035_MaxFrameHeight_attr.attr);		
	if (result < 0)
	{
		printk("[IMX035] : Cannot create IMX035_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
/* Version 1.0.0.4 modification, 2010.08.02 */
	sema_init(&imx035_mutex, 1);
/* ========================== */
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void IMX035_Exit(void)
{
	sysfs_remove_file(&IMX035_kobj, &(IMX035_SensorName_attr.attr));
	sysfs_remove_file(&IMX035_kobj, &(IMX035_dwBufNum_attr.attr));	
/* Version 1.0.0.3 modification, 2010.06.21 */
	sysfs_remove_file(&IMX035_kobj, &(IMX035_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&IMX035_kobj, &(IMX035_MaxFrameHeight_attr.attr));		
/* ========================== */
	kobject_put(&IMX035_kobj);	
	printk(KERN_DEBUG "[IMX035] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(IMX035_Init);
module_exit(IMX035_Exit);

