/*
 * ap1513
 * Driver for ap1513.
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
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "ap1513.h"
#include "ap1513_locals.h"

const CHAR AP1513_ID[] = "$Version: "AP1513_ID_VERSION"  (AP1513 DRIVER) $";
/* ============================================================================================== */

static TAP1513Info *ptInfo;

static SOCKET dwAgpoVINNode = 0;
static SOCKET dwAgpoBRCNode = 1;

module_param(dwAgpoVINNode, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwAgpoVINNode, "AP1513 VIN agpo node");
module_param(dwAgpoBRCNode, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwAgpoBRCNode, "AP1513 BRC agpo node");

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("AP1513 driver");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
/* Version 1.0.0.1 modification, 2012.10.24 */
void AP1513_UpdatePattern(DWORD dwDutyCycle, AGPO_Info *tAGPOInfo)
{
	DWORD dwShiftBit = 128 - dwDutyCycle;
/* ======================================== */
	if (dwShiftBit <= 32)
	{
		tAGPOInfo->dwPattern_3 = ((0xFFFFFFFF>>dwShiftBit)&0xFFFFFFFF);
		tAGPOInfo->dwPattern_2 = 0xFFFFFFFF;
		tAGPOInfo->dwPattern_1 = 0xFFFFFFFF;
		tAGPOInfo->dwPattern_0 = 0xFFFFFFFF; 
	}
	else if (dwShiftBit <= 64)
	{
		tAGPOInfo->dwPattern_3 = 0x0; 
		dwShiftBit = dwShiftBit - 32;
		tAGPOInfo->dwPattern_2 = ((0xFFFFFFFF>>dwShiftBit)&0xFFFFFFFF);
		tAGPOInfo->dwPattern_1 = 0xFFFFFFFF;
		tAGPOInfo->dwPattern_0 = 0xFFFFFFFF; 
	}
	else if (dwShiftBit <= 96)
	{
		tAGPOInfo->dwPattern_3 = 0x0; 
		tAGPOInfo->dwPattern_2 = 0x0; 
		dwShiftBit = dwShiftBit - 64;
		tAGPOInfo->dwPattern_1 = ((0xFFFFFFFF>>dwShiftBit)&0xFFFFFFFF);
		tAGPOInfo->dwPattern_0 = 0xFFFFFFFF; 
	}
	else if (dwShiftBit <= 128)
	{
		tAGPOInfo->dwPattern_3 = 0x0; 
		tAGPOInfo->dwPattern_2 = 0x0;
		tAGPOInfo->dwPattern_1 = 0x0;
		dwShiftBit = dwShiftBit - 96;
		tAGPOInfo->dwPattern_0 = ((0xFFFFFFFF>>dwShiftBit)&0xFFFFFFFF);
	}
}

/* ============================================================================================== */
static void AP1513_WriteSpeedToDevice(DWORD dwSpeed)
{
/* Version 1.0.0.1 modification, 2012.10.24 */
	DWORD dwCurrDuty;	

	dwCurrDuty = (dwSpeed&0xFFFF);

	AP1513_UpdatePattern(dwCurrDuty, &ptInfo->tCurrAGPOInfo);

	agpo_enable(ptInfo->dwAgpoCurrNode, FALSE);
	agpo_setup(ptInfo->dwAgpoCurrNode, &ptInfo->tCurrAGPOInfo);
/* ======================================== */
	agpo_enable(ptInfo->dwAgpoCurrNode, TRUE);
}

/* ============================================================================================== */
void AP1513_Release(void)
{
	agpo_free(ptInfo->dwAgpoCurrNode);
	agpo_free(ptInfo->dwAgpoTargetNode);

	kfree(ptInfo);

	module_put(THIS_MODULE);
	return;
}

/* ============================================================================================== */
SOCKET AP1513_Open(void)
{
	SDWORD sdwResult = 0;
	DWORD dwIndex, adwAGPOPIN[2] = {0};

	if (!(ptInfo=(TAP1513Info *)kmalloc(sizeof(TAP1513Info), GFP_KERNEL)))
	{
		printk("[TAP1513] : Allocate %d bytes memory fail\n", sizeof(TAP1513Info));
		return -ENOMEM;
	}

	memset(ptInfo, 0, sizeof(TAP1513Info));

	ptInfo->dwAgpoCurrNode = dwAgpoVINNode;
	adwAGPOPIN[0] = dwAgpoVINNode;
	ptInfo->dwAgpoTargetNode = dwAgpoBRCNode;
	adwAGPOPIN[1] = dwAgpoBRCNode;

/* Version 1.0.0.1 modification, 2012.10.24 */
	ptInfo->tCurrAGPOInfo.dwFlag = 0;
	ptInfo->tTargetAGPOInfo.dwFlag = 0;

	ptInfo->tCurrAGPOInfo.dwPeriod = 0x00000013;
	ptInfo->tCurrAGPOInfo.dwDataLength = 0x07E;
	ptInfo->tCurrAGPOInfo.dwIntvRepeat = 0xFFF0007F;

	ptInfo->tTargetAGPOInfo.dwPeriod = 0x00000013;
	ptInfo->tTargetAGPOInfo.dwDataLength = 0x07E;
	ptInfo->tTargetAGPOInfo.dwIntvRepeat = 0xFFF0007F;
/* ======================================== */

	for (dwIndex=0; dwIndex<2; dwIndex++)
	{
		if ((agpo_request(adwAGPOPIN[dwIndex],NULL)) == -1)
		{
			printk("[AP1513] fail to open /dev/agpo%d.\n", (int)adwAGPOPIN[dwIndex]);
			AP1513_Release();
			return -ENODEV;
		}
		sdwResult |= agpo_set_default_val(adwAGPOPIN[dwIndex], 0);
		sdwResult |= agpo_intr_mask(adwAGPOPIN[dwIndex], TRUE);
		sdwResult |= agpo_set_direction(adwAGPOPIN[dwIndex], 1);
	}

	if (sdwResult < 0)
	{
		printk("[AP1513] fail to init agpo dev driver.\n");
		AP1513_Release();
		return -ENODEV;
	}

/* Version 1.0.0.1 modification, 2012.10.24 */
	ptInfo->dwCurrDuty = 128;	
	ptInfo->dwTargetDuty = 90; // Fixed it	

	AP1513_UpdatePattern(ptInfo->dwCurrDuty, &ptInfo->tCurrAGPOInfo);
	agpo_enable(ptInfo->dwAgpoCurrNode, FALSE);
	agpo_set_mode(ptInfo->dwAgpoCurrNode, &ptInfo->tCurrAGPOInfo);		
	agpo_setup(ptInfo->dwAgpoCurrNode, &ptInfo->tCurrAGPOInfo);

	AP1513_UpdatePattern(ptInfo->dwTargetDuty, &ptInfo->tTargetAGPOInfo);
	agpo_enable(ptInfo->dwAgpoTargetNode, FALSE);
	agpo_set_mode(ptInfo->dwAgpoTargetNode, &ptInfo->tTargetAGPOInfo);		
	agpo_setup(ptInfo->dwAgpoTargetNode, &ptInfo->tTargetAGPOInfo);

	agpo_enable(ptInfo->dwAgpoTargetNode, TRUE);
	agpo_enable(ptInfo->dwAgpoCurrNode, TRUE);
/* ======================================== */

	if (try_module_get(THIS_MODULE) == 0)
	{
/* Version 1.0.0.1 modification, 2012.10.24 */
		AP1513_Release();
/* ======================================== */
		return -EBUSY;
	}
	printk ("[AP1513] : Reset Done \n");

	return 0;
}

/* ============================================================================================== */
SOCKET  AP1513_DCIrisControl(DWORD dwCmd,  DWORD dwArg)
{
	SOCKET sckRet = 0;
	TIrisCtrlInfo* ptAP1513Info;

	switch (dwCmd)
	{
		case LENS_CTRL_IRIS_OPEN:
		case LENS_CTRL_IRIS_CLOSE:
		case LENS_CTRL_IRIS_OPEN_LARGEST:
		ptAP1513Info = (TIrisCtrlInfo*)dwArg;
		AP1513_WriteSpeedToDevice(ptAP1513Info->dwSpeed);
		break;
		case LENS_CTRL_IRIS_CLOSE_SMALLEST:
		case LENS_CTRL_IRIS_STOP:
		default:
		sckRet = -EPERM;		
	}
	return sckRet;		
}

/* ============================================================================================== */
SOCKET AP1513_SetOptions(TVideoSignalOptions *ptArg)
{
	SOCKET sckRet = 0;

	switch (ptArg->eOptionFlags)
	{
/* Version 1.0.0.1 modification, 2012.10.24 */
/* ======================================== */
		default:
		return -1;
	}
	return sckRet;
}

/* ============================================================================================== */
static TIrisMotorDevice DCIris_dev_ops =
{
	.open = AP1513_Open,
	.release = AP1513_Release,
	.set_options = AP1513_SetOptions,
	.control = AP1513_DCIrisControl,
};

extern TIrisMotorDevice *iris_motor_dev;

/* ============================================================================================== */
static SOCKET AP1513_Init(void)
{
	iris_motor_dev = &DCIris_dev_ops;

	printk("Install ap1513 driver version %d.%d.%d.%d complete !!\n",
		(int)(AP1513_VERSION&0xFF),
		(int)((AP1513_VERSION>>8)&0xFF),
		(int)((AP1513_VERSION>>16)&0xFF),
		(int)((AP1513_VERSION>>24)&0xFF));
	return 0;
}

/* ============================================================================================== */
static void AP1513_Exit(void)
{
	iris_motor_dev = NULL;
	printk(KERN_DEBUG "[AP1513] : bye\n");
	return;
}

module_init(AP1513_Init);
module_exit(AP1513_Exit);
/* ============================================================================================== */
