/*
 * pwmiriscontrol
 * Driver for pwmiriscontrol.
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

#include "pwmiriscontrol.h"
#include "pwmiriscontrol_locals.h"

const CHAR PWMIRISCONTROL_ID[] = "$Version: "PWMIRISCONTROL_ID_VERSION"  (PWM IRIS CONTROL DRIVER) $";
/* ============================================================================================== */

static TPWMIrisControlInfo *ptInfo;

static SOCKET dwAgpoVINNode = 0;

module_param(dwAgpoVINNode, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (dwAgpoVINNode, "PWM iris control VIN agpo node");

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("PWM iris control driver");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
void PWMIrisControl_UpdatePattern(DWORD dwShiftBit, AGPO_Info *tAGPOInfo)
{
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
static void PWMIrisControl_WriteSpeedToDevice(DWORD dwSpeed)
{
	//ptInfo->tAGPOInfo.dwFlag = 0;

	PWMIrisControl_UpdatePattern(dwSpeed, &ptInfo->tAGPOInfo);

	ptInfo->tAGPOInfo.dwPeriod = 0x00000013;
	ptInfo->tAGPOInfo.dwDataLength = 0x07E;
	ptInfo->tAGPOInfo.dwIntvRepeat = 0xFFF0007F;

	agpo_enable(ptInfo->dwAgpoCurrNode, FALSE);
	agpo_setup(ptInfo->dwAgpoCurrNode, &ptInfo->tAGPOInfo);
	agpo_enable(ptInfo->dwAgpoCurrNode, TRUE);
}

/* ============================================================================================== */
void PWMIrisControl_Release(void)
{
	agpo_enable(ptInfo->dwAgpoCurrNode, FALSE);
	agpo_free(ptInfo->dwAgpoCurrNode);

	kfree(ptInfo);

	module_put(THIS_MODULE);
	return;
}

/* ============================================================================================== */
SOCKET PWMIrisControl_Open(void)
{
	SDWORD sdwResult = 0;

	if (!(ptInfo=(TPWMIrisControlInfo *)kmalloc(sizeof(TPWMIrisControlInfo), GFP_KERNEL)))
	{
		printk("[PWMIrisControl] : Allocate %d bytes memory fail\n", sizeof(TPWMIrisControlInfo));
		return -ENOMEM;
	}

	memset(ptInfo, 0, sizeof(TPWMIrisControlInfo));

	ptInfo->dwAgpoCurrNode = dwAgpoVINNode;

	if ((agpo_request(ptInfo->dwAgpoCurrNode,NULL)) == -1)
	{
		printk("[PWMIrisControl] fail to open /dev/agpo%d.\n", (int)ptInfo->dwAgpoCurrNode);
		PWMIrisControl_Release();
		return -ENODEV;
	}
	sdwResult |= agpo_set_default_val(ptInfo->dwAgpoCurrNode, 1);
	sdwResult |= agpo_intr_mask(ptInfo->dwAgpoCurrNode, TRUE);
	sdwResult |= agpo_set_direction(ptInfo->dwAgpoCurrNode, 1);

	if (sdwResult < 0)
	{
		printk("[PWMIrisControl] fail to init PWM iris control driver.\n");
		PWMIrisControl_Release();
		return -ENODEV;
	}

	ptInfo->tAGPOInfo.dwFlag = 0; // fixed

	ptInfo->tAGPOInfo.dwPeriod = 0x00000FFF;
	ptInfo->tAGPOInfo.dwDataLength = 0x07E;
	ptInfo->tAGPOInfo.dwIntvRepeat = 0x0FF0007F;

	ptInfo->tAGPOInfo.dwPattern_0 = 0xFFFFFFFF;
	ptInfo->tAGPOInfo.dwPattern_1 = 0xFFFFFFFF;
	ptInfo->tAGPOInfo.dwPattern_2 = 0xFFFFFFFF;
	ptInfo->tAGPOInfo.dwPattern_3 = 0xFFFFFFFF;

	agpo_enable(ptInfo->dwAgpoCurrNode, FALSE);
	agpo_setup(ptInfo->dwAgpoCurrNode, &ptInfo->tAGPOInfo);
	agpo_enable(ptInfo->dwAgpoCurrNode, TRUE);

	if (try_module_get(THIS_MODULE) == 0)
	{
		return -EBUSY;
	}

	printk ("[PWMIrisControl] : Reset Done \n");
	return 0;
}

/* ============================================================================================== */
SOCKET  PWMIrisControl_DCIrisControl(DWORD dwCmd,  DWORD dwArg)
{
	SOCKET sckRet = 0;
	TIrisCtrlInfo* ptIrisCtrlInfo;

	switch (dwCmd)
	{
		case LENS_CTRL_IRIS_OPEN:
		case LENS_CTRL_IRIS_CLOSE:
		case LENS_CTRL_IRIS_OPEN_LARGEST:
		ptIrisCtrlInfo = (TIrisCtrlInfo*)dwArg;
		PWMIrisControl_WriteSpeedToDevice(ptIrisCtrlInfo->dwSpeed);
		break;
		case LENS_CTRL_IRIS_CLOSE_SMALLEST:
		case LENS_CTRL_IRIS_STOP:
		default:
		sckRet = -EPERM;		
	}
	return sckRet;		
}

/* ============================================================================================== */
SOCKET PWMIrisControl_SetOptions(TVideoSignalOptions *ptArg)
{
	SOCKET sckRet = 0;

	switch (ptArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE:
		default:
		return -1;
	}
	return sckRet;
}

/* ============================================================================================== */
static TIrisMotorDevice DCIris_dev_ops =
{
	.open = PWMIrisControl_Open,
	.release = PWMIrisControl_Release,
	.set_options = PWMIrisControl_SetOptions,
	.control = PWMIrisControl_DCIrisControl,
};

extern TIrisMotorDevice *iris_motor_dev;

/* ============================================================================================== */
static SOCKET PWMIrisControl_Init(void)
{
	iris_motor_dev = &DCIris_dev_ops;

	printk("Install pwmiriscontrol driver version %d.%d.%d.%d complete !!\n",
		(int)(PWMIRISCONTROL_VERSION&0xFF),
		(int)((PWMIRISCONTROL_VERSION>>8)&0xFF),
		(int)((PWMIRISCONTROL_VERSION>>16)&0xFF),
		(int)((PWMIRISCONTROL_VERSION>>24)&0xFF));
	return 0;
}

/* ============================================================================================== */
static void PWMIrisControl_Exit(void)
{
	iris_motor_dev = NULL;
	printk(KERN_DEBUG "[PWMIrisControl] : bye\n");
	return;
}

module_init(PWMIrisControl_Init);
module_exit(PWMIrisControl_Exit);
/* ============================================================================================== */
