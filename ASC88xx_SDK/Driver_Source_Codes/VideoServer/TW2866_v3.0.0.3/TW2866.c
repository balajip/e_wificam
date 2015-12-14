/*
 * TW2866
 * Driver for TW2866 video encoder.
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
/* Version 2.0.0.0 modification, 2010.08.02 */	
#include <linux/spinlock.h> // spin_lock_irqsave(), spin_lock_irqrestore()
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()
/* ========================================= */

#include "TW2866.h"
#include "TW2866_locals.h"
#include "IICCtrl.h"

const CHAR TW2866_ID[] = "$Version: "TW2866_ID_VERSION"  (TW2866 DRIVER) $";
/*==================================================================*/
static TTW2866Info *ptInfo[2]; // 0 for TW2866 address 0x70, 1 for TW2866 address 0x72
static DWORD TW2866_DevInUse[2];
static DWORD TW2866_DevAddr[2] = {TW2866_DEAFULT_DEVICE_ADDR0, TW2866_DEAFULT_DEVICE_ADDR1};
TVideoSensorDevice sensor_dev_ops;
/* Version 2.0.0.0 modification, 2010.08.02 */	
static struct semaphore tw2866_mutex;

extern struct mutex AV_SYNC_LOCK;	// mutex exclusion with TW2866 audio driver

/* TW2866 default standard */
static int dwDefaultStandard = TVSTD_PAL;  
module_param(dwDefaultStandard, int, S_IRUGO|S_IWUSR); 
MODULE_PARM_DESC (dwDefaultStandard, "TW2866 default standard. (0:NTSC, 1:PAL)");
/* ========================================= */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("TW2866 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
/* Version 2.0.0.5 modification, 2010.11.23 */	
const TTW2866RegAddrData atTW2866regTbl[TW2866_TBL_SIZE] =
{
		{0x4b, 0x00},		//DAC no power low 

		{0x60,0x15},		//XTI&XTO (27MHz)

		{0x9e,0x52},		// below settings are for 108MHz 1chan D1
		{0xca,0xaa},
		{0xcb,0x00},
		{0xfa,0x4a},
		{0xfb,0x2f},
		{0x6a,0x0a},
		{0x6b,0x0a},
		{0x6c,0x0a},			
		{0x9f,0x00},        // clk delay chn0 
		{0x67,0x00},        // clk delay chn1 
		{0x68,0x00},        // clk delay chn2          	  
		{0x69,0x00},        // clk delay chn3          		
		{0x5b,0xff},
		{0x96,0xe6},
		{0x41,0xd4},	
		{0x97,0xc5},		// free run mode : 60Hz
		
		{0x08,0x02},		//vertical delay
		{0x0a,0x02},		//horizontal delay
		{0x18,0x02},
		{0x1a,0x02},
		{0x28,0x02},
		{0x2a,0x02},
		{0x38,0x02},
		{0x3a,0x02},

		{0x0e, 0x0f},		// disable the shadow registers
		{0x1e, 0x0f},
		{0x2e, 0x0f},
		{0x3e, 0x0f},		
#if 0
		//54MHz MUX4 CIF
                {_9E_NOVID                                                                      ,0x72},
                {_CA_VideoChannelOutputControl                          ,0x01},
                {_CB_FourChannelCIFTimeMultiplexedFormat        ,0x01},
                {_FA_ClockOutputControl                                         ,0x45},
                {_FB_                                                                           ,0x4F},
                {0x9f,0xf},
#endif
};

const TTW2866RegAddrData atTW2866regStdTbl[2][TW2866_STD_TBL_SIZE] =
{
	// NTSC
	{
		{0x07,0x12},
		{0x09,0x06},
	},
	// PAL
	{
		{0x07,0x12},
		{0x09,0x38},
	},
};
/* ========================================= */

/*==================================================================*/
static int TW2866_WriteReg(TTW2866Info *ptInfo, DWORD dwRegAddr, DWORD dwData)
{
/* Version 2.0.0.0 modification, 2010.08.02 */	
	DWORD ret;
	
	mutex_lock(&AV_SYNC_LOCK);
	ret = ptInfo->pfnWriteReg((DWORD)(ptInfo->dwFuncUserData), (DWORD)(ptInfo->dwDeviceAddr), (DWORD)dwRegAddr, (DWORD)dwData);
	mutex_unlock(&AV_SYNC_LOCK);
	
	return ret;
/* ========================================= */
}

/*-------------------------------------------------------------------------------------*/ 
static int TW2866_ReadReg(TTW2866Info *ptInfo, DWORD dwRegAddr, DWORD *pdwData)
{
/* Version 2.0.0.0 modification, 2010.08.02 */	
	DWORD ret;

	mutex_lock(&AV_SYNC_LOCK);
	ret = ptInfo->pfnReadReg((DWORD)(ptInfo->dwFuncUserData), (DWORD)(ptInfo->dwDeviceAddr), (DWORD)dwRegAddr, (DWORD*)pdwData);
	mutex_unlock(&AV_SYNC_LOCK);

	return ret;
/* ========================================= */
}
 
/*-------------------------------------------------------------------------------------*/
static int TW2866_CompReg(TTW2866Info *ptInfo, DWORD dwRegAddr, DWORD dwMask, DWORD dwData)
{
	DWORD dwReadData = 0;

	TW2866_ReadReg(ptInfo, dwRegAddr, &dwReadData);
	return ((dwReadData&dwMask) == dwData);
}

/*-------------------------------------------------------------------------------------*/
#if 0 // marked unused function
static int TW2866_update_reg(TTW2866Info *ptInfo, unsigned long dwRegAddr, unsigned long dwMask, unsigned long dwData)
{
	unsigned long dwReadData, dwWriteData;

	if (TW2866_ReadReg(ptInfo, dwRegAddr, &dwReadData) < 0)
		return -1;
	dwWriteData = (dwReadData&dwMask) | dwData;
	if (TW2866_WriteReg(ptInfo, dwRegAddr, dwWriteData) < 0)
		return -1;

	return 0;
}
#endif

/*-------------------------------------------------------------------------------------*/
static void TW2866_Reset(TTW2866Info *ptInfo)
{
	DWORD i;

	for (i=0; i<TW2866_TBL_SIZE; i++)
	{
		TW2866_WriteReg(ptInfo, atTW2866regTbl[i].dwRegAddr, atTW2866regTbl[i].dwData);
	}
/* Version 2.0.0.3 modification, 2010.10.18 */	
	msleep(130);
/* ========================================= */
#if 0 // for debug
	for (i=0; i<TW2866_TBL_SIZE; i++) {
		TW2866_ReadReg(ptInfo, atTW2866regTbl[i].dwRegAddr, &data);
		printk("	{0x%02x,0x%02x},\n", atTW2866regTbl[i].dwRegAddr, data&0xFF);
	}
	printk("\n");
#endif
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.1 modification, 2010.08.13 */	
static void TW2866_SetStandard1(TTW2866Info *ptInfo, ETVStd std, DWORD dwDevNum)
{
	DWORD i;
	DWORD chn;	

	chn = (dwDevNum>3) ? (dwDevNum-4) : dwDevNum;
	for (i=0; i<TW2866_STD_TBL_SIZE; i++)
	{
		TW2866_WriteReg(ptInfo, atTW2866regStdTbl[std][i].dwRegAddr+(chn<<4), atTW2866regStdTbl[std][i].dwData);
	}
#if 0 // for debug
	for (i=0; i<TW2866_STD_TBL_SIZE; i++) {
		TW2866_ReadReg(ptInfo, atTW2866regStdTbl[ptInfo->tStdInfo.eDetectStd][i].dwRegAddr, &data);
		printk("	{0x%02x,0x%02x},\n", atTW2866regStdTbl[ptInfo->tStdInfo.eDetectStd][i].dwRegAddr, data&0xFF);
	}
	printk("\n");
#endif
}

/*-------------------------------------------------------------------------------------*/
void TW2866_SetStandard(DWORD dwDevNum, ETVStd std)
{
	DWORD TW2866_dev;

	down_interruptible(&tw2866_mutex);
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		TW2866_dev = 0;
	}
	else
	{ // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		TW2866_dev = 1;
	}
	TW2866_SetStandard1(ptInfo[TW2866_dev], std, dwDevNum);
	up(&tw2866_mutex);
}

/*-------------------------------------------------------------------------------------*/
static void TW2866_AutoDetect1(TTW2866Info *ptInfo, TAutoDetectStdInfo *ptStdInfo, DWORD dwDevNum)
{
	DWORD data, data1, reg;
	DWORD chn;
	
	chn = (dwDevNum>3) ? (dwDevNum-4) : dwDevNum;

	// check data loss 
	reg = 0x00 + (chn<<4);
	TW2866_ReadReg(ptInfo, reg, &data);
	reg = 0x0E + (chn<<4);
	TW2866_ReadReg(ptInfo, reg, &data1);
	if ((data&0x88) == 0x08)
	{	// video detect
		ptStdInfo->bDataLost = 0;
		// detect standard
		// PAL
		if ((data1&0x70) == 0x10)
		{ 
			ptStdInfo->eDetectStd = TVSTD_PAL;
		}
		// NTSC
		else if ((data1&0x70) == 0x00)
		{ 
			ptStdInfo->eDetectStd = TVSTD_NTSC;
		}
		// unknown standard
		else
		{ 
			ptStdInfo->eDetectStd = TVSTD_UNKNOWN;
		}
	}
	else
	{ // video lost
		ptStdInfo->bDataLost = 1;
	}
#if 0 // for debug
	if (devNum == 0) {
		if (ptStdInfo->eDetectStd == TVSTD_NTSC) {
			printk("detect NTSC\n");
		}
		else if (ptStdInfo->eDetectStd == TVSTD_PAL) {
			printk("detect PAL\n");
		}
		else {
			printk("data lost\n");
		}
	}
#endif
}

/*-------------------------------------------------------------------------------------*/
void TW2866_AutoDetect(DWORD dwDevNum, TAutoDetectStdInfo *ptStdInfo)
{
	DWORD TW2866_dev;

	down_interruptible(&tw2866_mutex);
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		TW2866_dev = 0;
	}
	else
	{ // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		TW2866_dev = 1;
	}			
	TW2866_AutoDetect1(ptInfo[TW2866_dev], ptStdInfo, dwDevNum);
	up(&tw2866_mutex);		
}
/* ========================================= */
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
int  TW2866_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
/* ========================================= */
{
	int ret = 0;
	DWORD TW2866_dev;

/* Version 2.0.0.0 modification, 2010.08.02 */	
	if (down_interruptible(&tw2866_mutex))
	{
		return -ERESTARTSYS;	
	}
/* ========================================= */

	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		TW2866_dev = 0;
	}
	else
	{ // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		TW2866_dev = 1;
	}	

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:		
		TW2866_Reset(ptInfo[TW2866_dev]);
		break;
/* Version 2.0.0.1 modification, 2010.08.13 */	
		case VIDEO_SIGNAL_OPTION_AUTO_DETECT_STD:
		TW2866_AutoDetect1(ptInfo[TW2866_dev], (TAutoDetectStdInfo *)(ptIoctlArg->adwUserData[0]), dwDevNum);
		break;
/* ========================================= */
	default:
		ret = -EPERM;
	}
/* Version 2.0.0.0 modification, 2010.08.02 */	
	up(&tw2866_mutex);	
/* ========================================= */
	
	return ret;
}

/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
void TW2866_Release(DWORD dwDevNum)
/* ========================================= */
{
	DWORD TW2866_dev;

/* Version 2.0.0.0 modification, 2010.08.02 */	
	down_interruptible(&tw2866_mutex);
/* ========================================= */
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		TW2866_dev = 0;
	}
	else { // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		TW2866_dev = 1;
	}
	if (TW2866_DevInUse[TW2866_dev] > 1)
	{ // TW2866_DEV0 is still used by some other streams
		TW2866_DevInUse[TW2866_dev]--;	
	}
	else
	{
		IICCtrl_Release((void*)(&(ptInfo[TW2866_dev]->dwFuncUserData)));
		if (ptInfo[TW2866_dev])
		{
			kfree(ptInfo[TW2866_dev]);
		}
		TW2866_DevInUse[TW2866_dev] = 0;
	}	
/* Version 2.0.0.0 modification, 2010.08.02 */	
	up(&tw2866_mutex);	
/* ========================================= */
}

/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
int TW2866_Open(TVideoSensorInitialParam* ptTW2866InitialParam, DWORD dwDevNum)
/* ========================================= */
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;
	DWORD TW2866_dev;

/* Version 3.0.0.0 modification, 2011.01.27 */
	if (((ptTW2866InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(TW2866_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptTW2866InitialParam->dwVideoSensorVersion&0x0000FF00)>(TW2866_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid TW2866 device driver version %d.%d.%d.%d !!\n", 
				(int)(TW2866_INTERFACE_VERSION&0xFF), 
				(int)((TW2866_INTERFACE_VERSION>>8)&0xFF), 
				(int)((TW2866_INTERFACE_VERSION>>16)&0xFF), 
				(int)(TW2866_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

/* Version 2.0.0.0 modification, 2010.08.02 */	
	if (down_interruptible(&tw2866_mutex))
	{
		return -ERESTARTSYS;	
	}
/* ========================================= */

	// 1 determine which hardware device should be opened
	if ((dwDevNum==0) || (dwDevNum==1) || (dwDevNum==2) || (dwDevNum==3))
	{
		TW2866_dev = 0;
	}
	else
	{ // (dwDevNum==4) || (dwDevNum==5) || (dwDevNum==6) || (dwDevNum==7)
		TW2866_dev = 1;
	}
	// 2 check initialization
	if (TW2866_DevInUse[TW2866_dev])
	{
/* Version 2.0.0.1 modification, 2010.08.13 */	
		TW2866_SetStandard1(ptInfo[TW2866_dev], (ETVStd)dwDefaultStandard, dwDevNum);
/* ========================================= */
		TW2866_DevInUse[TW2866_dev]++;
/* Version 2.0.0.0 modification, 2010.08.02 */	
		up(&tw2866_mutex);		
/* ========================================= */
		return 0;
	}
	// 3 open TW2866_DEV
	// 3.1 memory allocate
	if (!(ptInfo[TW2866_dev] = (TTW2866Info *)kmalloc(sizeof(TTW2866Info), GFP_KERNEL)))
	{
		printk("[TW2866] : Allocate %d bytes memory fail\n", sizeof(TTW2866Info));
		TW2866_Release(dwDevNum);
/* Version 2.0.0.0 modification, 2010.08.02 */	
		up(&tw2866_mutex);	
/* ========================================= */			
		return -ENOMEM;
	}
	// 3.2 init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 2;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[TW2866] : Initial IICCtrl object fail !!\n");
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
		TW2866_Release(dwDevNum);	
/* ========================================= */	
/* Version 2.0.0.0 modification, 2010.08.02 */	
		up(&tw2866_mutex);	
/* ========================================= */	
		return -ENODEV;
	}
	// 3.3 init private data		
	ptInfo[TW2866_dev]->dwDeviceAddr = TW2866_DevAddr[TW2866_dev];
	ptInfo[TW2866_dev]->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo[TW2866_dev]->pfnReadReg = (FOnReadReg_t)IICCtrl_ReadReg;
	ptInfo[TW2866_dev]->pfnWriteReg = (FOnWriteReg_t)IICCtrl_WriteReg;
	// 3.4 detect device
	if ((!TW2866_CompReg(ptInfo[TW2866_dev], 0xFE, 0xFF, 0x00)) ||
		(!TW2866_CompReg(ptInfo[TW2866_dev], 0xFF, 0xFF, 0xc8)))
	{
		printk("[TW2866] : device doesn't exist in device address 0x%02lX\n", ptInfo[TW2866_dev]->dwDeviceAddr);
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
		TW2866_Release(dwDevNum);
/* ========================================= */		
/* Version 2.0.0.0 modification, 2010.08.02 */	
		up(&tw2866_mutex);		
/* ========================================= */
		return -ENODEV;
	}
	// 3.5 reset device
	TW2866_Reset(ptInfo[TW2866_dev]);
/* Version 2.0.0.0 modification, 2010.08.02 */	
/* Version 2.0.0.1 modification, 2010.08.13 */		
	// 3.6 set default standard
	TW2866_SetStandard1(ptInfo[TW2866_dev], (ETVStd)dwDefaultStandard, dwDevNum);
/* ========================================= */
	TW2866_DevInUse[TW2866_dev] = 1;		
/* Version 2.0.0.0 modification, 2010.08.02 */
	up(&tw2866_mutex);				
/* ========================================= */
	return 0;
}

/*-------------------------------------------------------------------------------------*/
TVideoSensorDevice sensor_dev_ops =
{
	.open = 	TW2866_Open,
	.release = 	TW2866_Release,
	.ioctl = 	TW2866_Ioctl,
/* Version 3.0.0.0 modification, 2011.01.27 */
/* ======================================== */
	.get_shutter_value = NULL,
/* Version 1.0.0.2 modification, 2010.06.03 */	
	.get_remaining_line_num = NULL,
/* ========================================= */
	.group_access = NULL,
/* Version 2.0.0.1 modification, 2010.08.13 */	
	.setup_standard = TW2866_SetStandard,
	.detect_standard = TW2866_AutoDetect,
/* ========================================= */
/* Version 3.0.0.2 modification, 2012.09.04 */	
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = NULL,	
/* ========================================= */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct TW2866_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t TW2866_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct TW2866_attribute *TW2866_attr = container_of(attr, struct TW2866_attribute, attr);
	int result = 0;
	
	if (TW2866_attr->show)
		result = TW2866_attr->show(kobj, buf);	
	return result;
}

ssize_t TW2866_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "TW2866");
	return retval;
}

ssize_t TW2866_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
/* Version 3.0.0.1 modification, 2012.07.12 */	
	retval = sprintf (buf, "%s\n", "6");
/* ========================================= */
	return retval;
}

ssize_t TW2866_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "720");
	return retval;
}

ssize_t TW2866_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
/* Version 2.0.0.0 modification, 2010.08.02 */	
	retval = sprintf (buf, "%s\n", "576");
/* ========================================= */
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject TW2866_kobj;

struct TW2866_attribute TW2866_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = TW2866_SensorName_attr_show,
};

struct TW2866_attribute TW2866_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = TW2866_dwBufNum_attr_show,
};

struct TW2866_attribute TW2866_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = TW2866_MaxFrameWidth_attr_show,
};

struct TW2866_attribute TW2866_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = TW2866_MaxFrameHeight_attr_show,
};

struct sysfs_ops TW2866_sysfs_ops = {
	.show	= TW2866_default_attr_show,
};

struct kobj_type TW2866_ktype = {
	.sysfs_ops	= &TW2866_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int TW2866_init(void)
{
	int result = 0;
	
	if (result < 0){
		printk("[TW2866] : kobj set name fail!\n");
		return -1;
	}

	TW2866_kobj.ktype = &TW2866_ktype;	
	result = kobject_init_and_add(&TW2866_kobj, &TW2866_ktype, NULL, "VideoSensorInfo");

	if (result < 0) {
		printk("[TW2866] : Cannot register kobject [TW2866_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&TW2866_kobj, &TW2866_SensorName_attr.attr);		
       if (result < 0) {
		printk("[TW2866] : Cannot create TW2866_SensorName_attr.attr.\n");
		return -1;
       }

	result = sysfs_create_file(&TW2866_kobj, &TW2866_dwBufNum_attr.attr);		
       if (result < 0) {
		printk("[TW2866] : Cannot create TW2866_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&TW2866_kobj, &TW2866_MaxFrameWidth_attr.attr);		
       if (result < 0) {
		printk("[TW2866] : Cannot create TW2866_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&TW2866_kobj, &TW2866_MaxFrameHeight_attr.attr);		
       if (result < 0) {
		printk("[TW2866] : Cannot create TW2866_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	
/* Version 2.0.0.0 modification, 2010.08.02 */	
	sema_init(&tw2866_mutex, 1);
/* ========================================= */
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void TW2866_exit(void)
{
	sysfs_remove_file(&TW2866_kobj, &(TW2866_SensorName_attr.attr));
	sysfs_remove_file(&TW2866_kobj, &(TW2866_dwBufNum_attr.attr));	
	sysfs_remove_file(&TW2866_kobj, &(TW2866_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&TW2866_kobj, &(TW2866_MaxFrameHeight_attr.attr));	
	kobject_put(&TW2866_kobj);	
	printk(KERN_DEBUG "[TW2866] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(TW2866_init);
module_exit(TW2866_exit);
