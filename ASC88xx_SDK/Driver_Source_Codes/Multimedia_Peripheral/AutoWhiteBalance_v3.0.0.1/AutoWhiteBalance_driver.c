/*
 * AutoWhiteBalance
 * Driver for AutoWhiteBalance.
 *
 * Copyright (C) 2010  ______ Inc.
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

/* ============================================================================================== */
#include <linux/moduleparam.h>
#include "AutoWhiteBalance_driver.h"

const CHAR AUTOWHITEBALANCE_ID[] = "$Version: "AUTOWHITEBALANCE_ID_VERSION"  (AUTOWHITEBALANCE) $";

static HANDLE ghAWBInfo = NULL;

MODULE_AUTHOR("______ Inc.");
MODULE_DESCRIPTION("AutoWhiteBalance driver");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
SOCKET OneFrame(TAutoWhiteBalanceState *ptState)
{
	SOCKET sckRet = 0;
	sckRet = AutoWhiteBalance_OneFrame(ghAWBInfo, ptState);
	return sckRet;
}

/* ============================================================================================== */
SOCKET SetOptions(TVideoSignalOptions *ptArg)
{
	SOCKET sckRet = 0;
	DWORD adwstatisticPrior[240];	

	if (ptArg->eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_WINDOW_PRIORITY)
	{
		copy_from_user(&(adwstatisticPrior[0]), (DWORD *)(ptArg->adwUserData[0]), (240*sizeof(DWORD)));
		ptArg->adwUserData[0] = (DWORD)&adwstatisticPrior[0];
	}

	sckRet = AutoWhiteBalance_SetOptions(ghAWBInfo, ptArg);

	return sckRet;
}

/* ============================================================================================== */
SOCKET Open(TAutoWhiteBalanceInitialParam* ptAWBInitialParam)
{
	DWORD dwDevInfoSize;
	SOCKET sckRet = 0;

	dwDevInfoSize = AutoWhiteBalance_GetDevInfoSize();

	if ((ghAWBInfo=(HANDLE)kmalloc(dwDevInfoSize, GFP_KERNEL)) == NULL)
	{
		printk("[AutoWhiteBalance] : Allocate %d bytes memory fail\n", (int)dwDevInfoSize);
		return -ENOMEM;
	}	
	memset(ghAWBInfo, 0, dwDevInfoSize);

	sckRet = AutoWhiteBalance_Initial(ghAWBInfo, ptAWBInitialParam);

	if (try_module_get(THIS_MODULE) == 0)
	{
		return -EBUSY;
	}
	return sckRet;
}

/* ============================================================================================== */
SOCKET Release(void)
{
	kfree(ghAWBInfo);

	module_put(THIS_MODULE);
	return 0;
}

/* ============================================================================================== */
void SetColorTemperature(DWORD *pdwParam)
{
	AutoWhiteBalance_SetColorTemperature(ghAWBInfo, pdwParam);
}

/* Version 3.0.0.0 modification, 2013.06.21 */
/* ============================================================================================== */
static void SetBayerBitWidth(DWORD dwBayerBitWidth)
{
	AutoWhiteBalance_SetBayerBitWidth(ghAWBInfo, dwBayerBitWidth);
}
/* ======================================== */

/* ============================================================================================== */
static TAutoWhiteBalanceModule auto_white_balance_module_ops = 
{
	.open = Open,
	.release = Release,
	.set_options = SetOptions,	
	.one_frame = OneFrame,
	.set_color_temperature = SetColorTemperature,
/* Version 3.0.0.0 modification, 2013.06.21 */
	.set_bayer_bitwidth = SetBayerBitWidth,
/* ======================================== */
};

extern TAutoWhiteBalanceModule *awb_dev;

/* ============================================================================================== */
static SOCKET AutoWhiteBalance_Init(void)
{
	awb_dev = &auto_white_balance_module_ops;	

	printk("Install autowhitebalance driver version %d.%d.%d.%d complete !!\n",
		(int)(AUTOWHITEBALANCE_VERSION&0xFF),
		(int)((AUTOWHITEBALANCE_VERSION>>8)&0xFF),
		(int)((AUTOWHITEBALANCE_VERSION>>16)&0xFF),
		(int)((AUTOWHITEBALANCE_VERSION>>24)&0xFF));

	return 0;
}

/* ============================================================================================== */
static void AutoWhiteBalance_Exit(void)
{
	awb_dev = NULL;
	printk(KERN_DEBUG "[AutoWhiteBalance] : bye\n");
	return;
}

module_init(AutoWhiteBalance_Init);
module_exit(AutoWhiteBalance_Exit);
/* ============================================================================================== */
