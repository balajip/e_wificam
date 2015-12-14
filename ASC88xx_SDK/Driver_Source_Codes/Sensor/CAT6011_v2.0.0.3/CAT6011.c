/*
 * CAT6011
 * Driver for CAT6011.
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

#include "CAT6011.h"
#include "CAT6011_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR CAT6011_ID[] = "$Version: "CAT6011_ID_VERSION"  (CAT6011 DRIVER) $";
/*==================================================================*/
static TCAT6011Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore CAT6011_mutex;

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("CAT6011 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TCAT6011RegAddrData atCAT6011regTbl[CAT6011_TBL_SIZE] =
{
	{_1A_MiscCtrl					, 0x14},
	{_1B_VideoMap					, 0x20},
	{_1C_VideoCtrl1					, 0x40},
	{_1D_VclkCtrl					, 0x10},
	{_08_VIOCtrl					, 0xAB},
/* Version 2.0.0.1 modification, 2011.07.07 */
	{_20_CSCCtrl					, 0x00},
/* ======================================== */
	{_3C_PGCtrl1					, 0x00},
	{_3D_PGCtrl2					, 0x40},
	{_3E_PGColR					, 0x10},
	{_3F_PGColG					, 0x80},
	{_40_PGColB					, 0xF0}
};

/*==================================================================*/
static int CAT6011_WriteReg(TCAT6011Info *ptInfo, ECAT6011Regs eRegAddr, DWORD dwData)
{
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, eRegAddr, dwData);
}

/*-------------------------------------------------------------------------------------*/
static void CAT6011_Reset(TCAT6011Info *ptInfo)
{
	DWORD i;

	CAT6011_WriteReg(ptInfo, _05_RstCtrl, 0x3F);
	CAT6011_WriteReg(ptInfo, _05_RstCtrl, 0x20);
	for (i=0; i<CAT6011_TBL_SIZE; i++)
	{
		CAT6011_WriteReg(ptInfo, atCAT6011regTbl[i].eRegAddr, atCAT6011regTbl[i].dwData);
	}
}

/*==================================================================*/
int  CAT6011_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	int ret = 0;

	if (down_interruptible(&CAT6011_mutex))
	{
		return -ERESTARTSYS;
	}
	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		CAT6011_Reset(ptInfo);
		break;	
		default:
		ret = -EPERM;
	}
	up(&CAT6011_mutex);	
	
	return ret;
}

/*-------------------------------------------------------------------------------------*/
void CAT6011_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
int CAT6011_Open(TVideoSensorInitialParam* ptCAT6011InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

/* Version 2.0.0.0 modification, 2011.01.27 */
	if (((ptCAT6011InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(CAT6011_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptCAT6011InitialParam->dwVideoSensorVersion&0x0000FF00)>(CAT6011_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid CAT6011 device driver version %d.%d.%d.%d !!\n", 
				(int)(CAT6011_INTERFACE_VERSION&0xFF), 
				(int)((CAT6011_INTERFACE_VERSION>>8)&0xFF), 
				(int)((CAT6011_INTERFACE_VERSION>>16)&0xFF), 
				(int)(CAT6011_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

	if (!(ptInfo = (TCAT6011Info *)kmalloc(sizeof(TCAT6011Info), GFP_KERNEL)))
	{
		printk("[CAT6011.ko] : Allocate %d bytes memory fail\n", sizeof(TCAT6011Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 3;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[CAT6011.ko] : Initial IICCtrl object fail !!\n");
		CAT6011_Release(dwDevNum);				
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = CAT6011_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnWriteBuf = (FOnWriteReg_t)IICCtrl_WriteReg;
	// Reset device
	CAT6011_Reset(ptInfo);
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	CAT6011_Open,
	.release = 	CAT6011_Release,
	.ioctl = 	CAT6011_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.27 */
/* ======================================== */
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,	
/* Version 2.0.0.2 modification, 2012.09.04 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = NULL,
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct CAT6011_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t CAT6011_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct CAT6011_attribute *CAT6011_attr = container_of(attr, struct CAT6011_attribute, attr);
	int result = 0;
	
	if (CAT6011_attr->show)
		result = CAT6011_attr->show(kobj, buf);	
	return result;
}

ssize_t CAT6011_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "CAT6011");
	return retval;
}

ssize_t CAT6011_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t CAT6011_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}

ssize_t CAT6011_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1080");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject CAT6011_kobj;

struct CAT6011_attribute CAT6011_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = CAT6011_SensorName_attr_show,
};

struct CAT6011_attribute CAT6011_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = CAT6011_dwBufNum_attr_show,
};

struct CAT6011_attribute CAT6011_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = CAT6011_MaxFrameWidth_attr_show,
};

struct CAT6011_attribute CAT6011_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = CAT6011_MaxFrameHeight_attr_show,
};

struct sysfs_ops CAT6011_sysfs_ops = {
	.show	= CAT6011_default_attr_show,
};

struct kobj_type CAT6011_ktype = {
	.sysfs_ops	= &CAT6011_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int CAT6011_Init(void)
{
	int result = 0;
	
	if (result < 0){
		printk("[CAT6011] : kobj set name fail!\n");
		return -1;
	}

	CAT6011_kobj.ktype = &CAT6011_ktype;	
	result = kobject_init_and_add(&CAT6011_kobj, &CAT6011_ktype, NULL, "VideoSensorInfo");

	if (result < 0) {
		printk("[CAT6011] : Cannot register kobject [CAT6011_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&CAT6011_kobj, &CAT6011_SensorName_attr.attr);		
       if (result < 0) {
		printk("[CAT6011] : Cannot create CAT6011_SensorName_attr.attr.\n");
		return -1;
       }

	result = sysfs_create_file(&CAT6011_kobj, &CAT6011_dwBufNum_attr.attr);		
       if (result < 0) {
		printk("[CAT6011] : Cannot create CAT6011_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&CAT6011_kobj, &CAT6011_MaxFrameWidth_attr.attr);		
       if (result < 0) {
		printk("[CAT6011] : Cannot create CAT6011_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&CAT6011_kobj, &CAT6011_MaxFrameHeight_attr.attr);		
       if (result < 0) {
		printk("[CAT6011] : Cannot create CAT6011_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&CAT6011_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void CAT6011_Exit(void)
{
	sysfs_remove_file(&CAT6011_kobj, &(CAT6011_SensorName_attr.attr));
	sysfs_remove_file(&CAT6011_kobj, &(CAT6011_dwBufNum_attr.attr));	
	sysfs_remove_file(&CAT6011_kobj, &(CAT6011_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&CAT6011_kobj, &(CAT6011_MaxFrameHeight_attr.attr));		
	kobject_put(&CAT6011_kobj);	
	printk(KERN_DEBUG "[CAT6011] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(CAT6011_Init);
module_exit(CAT6011_Exit);

