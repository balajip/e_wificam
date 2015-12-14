/*
 * NULLSensor
 * Driver for NULLSensor.
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

#include "NULLSensor.h"
#include "NULLSensor_locals.h"

const CHAR NULLSensor_ID[] = "$Version: "NULLSensor_ID_VERSION"  (NULLSensor DRIVER) $";

/*==================================================================*/
static TVideoSensorDevice sensor_dev_ops;

/* Sensor capture width */
static int video_sensor_width = 1280;
/* Sensor capture height */
static int video_sensor_height = 1024;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "NULL sensor capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "NULL sensor capture height");

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("NULLSensor driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
inline int  NULLSensor_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	return -EPERM;
}

/*-------------------------------------------------------------------------------------*/
inline void NULLSensor_Release(DWORD dwDevNum)
{
}

/*-------------------------------------------------------------------------------------*/
inline int NULLSensor_Open(TVideoSensorInitialParam* ptSensorInitialParam, DWORD dwDevNum)
{
/* Version 2.0.0.0 modification, 2011.01.27 */
	if (((ptSensorInitialParam->dwVideoSensorVersion&0x00FF00FF)!=(NULLSensor_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptSensorInitialParam->dwVideoSensorVersion&0x0000FF00)>(NULLSensor_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid NULLSensor device driver version %d.%d.%d.%d !!\n", 
				(int)(NULLSensor_INTERFACE_VERSION&0xFF), 
				(int)((NULLSensor_INTERFACE_VERSION>>8)&0xFF), 
				(int)((NULLSensor_INTERFACE_VERSION>>16)&0xFF), 
				(int)(NULLSensor_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops = 
{
	.open = 	NULLSensor_Open,
	.release = 	NULLSensor_Release,
	.ioctl = 	NULLSensor_Ioctl,
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
struct NULLSensor_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t NULLSensor_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct NULLSensor_attribute *NULLSensor_attr = container_of(attr, struct NULLSensor_attribute, attr);
	int result = 0;
	
	if (NULLSensor_attr->show)
		result = NULLSensor_attr->show(kobj, buf);	
	return result;
}

ssize_t NULLSensor_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "NULLSensor");
	return retval;
}

ssize_t NULLSensor_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
/* Version 2.0.0.1 modification, 2012.08.13 */	
	retval = sprintf (buf, "%s\n", "6");
/* ========================================= */	
	return retval;
}

ssize_t NULLSensor_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%d\n", video_sensor_width);
	return retval;
}

ssize_t NULLSensor_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%d\n", video_sensor_height);
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject NULLSensor_kobj;

struct NULLSensor_attribute NULLSensor_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NULLSensor_SensorName_attr_show,
};

struct NULLSensor_attribute NULLSensor_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NULLSensor_dwBufNum_attr_show,
};

struct NULLSensor_attribute NULLSensor_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NULLSensor_MaxFrameWidth_attr_show,
};

struct NULLSensor_attribute NULLSensor_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = NULLSensor_MaxFrameHeight_attr_show,
};

struct sysfs_ops NULLSensor_sysfs_ops = {
	.show	= NULLSensor_default_attr_show,
};

struct kobj_type NULLSensor_ktype = {
	.sysfs_ops	= &NULLSensor_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int NULLSensor_Init(void)
{
	int result = 0;
	
	if (result < 0){
		printk("[NULLSensor] : kobj set name fail!\n");
		return -1;
	}

	NULLSensor_kobj.ktype = &NULLSensor_ktype;	
	result = kobject_init_and_add(&NULLSensor_kobj, &NULLSensor_ktype, NULL, "VideoSensorInfo");

	if (result < 0) {
		printk("[NULLSensor] : Cannot register kobject [NULLSensor_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&NULLSensor_kobj, &NULLSensor_SensorName_attr.attr);		
       if (result < 0) {
		printk("[NULLSensor] : Cannot create NULLSensor_SensorName_attr.attr.\n");
		return -1;
       }

	result = sysfs_create_file(&NULLSensor_kobj, &NULLSensor_dwBufNum_attr.attr);		
       if (result < 0) {
		printk("[NULLSensor] : Cannot create NULLSensor_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&NULLSensor_kobj, &NULLSensor_MaxFrameWidth_attr.attr);		
       if (result < 0) {
		printk("[NULLSensor] : Cannot create NULLSensor_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&NULLSensor_kobj, &NULLSensor_MaxFrameHeight_attr.attr);		
       if (result < 0) {
		printk("[NULLSensor] : Cannot create NULLSensor_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void NULLSensor_Exit(void)
{
	sysfs_remove_file(&NULLSensor_kobj, &(NULLSensor_SensorName_attr.attr));
	sysfs_remove_file(&NULLSensor_kobj, &(NULLSensor_dwBufNum_attr.attr));	
	sysfs_remove_file(&NULLSensor_kobj, &(NULLSensor_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&NULLSensor_kobj, &(NULLSensor_MaxFrameHeight_attr.attr));	

	kobject_put(&NULLSensor_kobj);	
	printk(KERN_DEBUG "[NULLSensor] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(NULLSensor_Init);
module_exit(NULLSensor_Exit);

