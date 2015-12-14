/*
 * AutoExposure
 * Driver for AutoExposure.
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
#include "AutoExposure_driver.h"

const CHAR AUTOEXPOSURE_ID[] = "$Version: "AUTOEXPOSURE_ID_VERSION"  (AUTOEXPOSURE) $";

static HANDLE ghAEInfo = NULL;

MODULE_AUTHOR("______ Inc.");
MODULE_DESCRIPTION("AutoExposure driver");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
void OneFrame(TAutoExposureState *ptState)
{
	AutoExposure_OneFrame(ghAEInfo, ptState);
}

/* ============================================================================================== */
SOCKET SetOptions(TVideoSignalOptions *ptArg)
{
	SOCKET sckRet = 0;
	DWORD adwstatisticPrior[240];	

	if (ptArg->eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOW_PRIORITY)
	{
		copy_from_user(&(adwstatisticPrior[0]), (DWORD *)(ptArg->adwUserData[0]), (240*sizeof(DWORD)));
		ptArg->adwUserData[0] = (DWORD)&adwstatisticPrior[0];
	}

	sckRet = AutoExposure_SetOptions(ghAEInfo, ptArg);

	return sckRet;
}

/* ============================================================================================== */
static SOCKET Open(TAutoExposureInitialParam* ptAEInitialParam)
{
	DWORD dwDevInfoSize;
	SOCKET sckRet = 0;

	dwDevInfoSize = AutoExposure_GetDevInfoSize();

	if ((ghAEInfo=(HANDLE)kmalloc(dwDevInfoSize, GFP_KERNEL)) == NULL)
	{
		printk("[AutoExposure] : Allocate %d bytes memory fail\n", (int)dwDevInfoSize);
		return -ENOMEM;
	}	
	memset(ghAEInfo, 0, dwDevInfoSize);

	sckRet = AutoExposure_Initial(ghAEInfo, ptAEInitialParam);

	if (try_module_get(THIS_MODULE) == 0)
	{
		return -EBUSY;
	}

	return sckRet;
}

/* ============================================================================================== */
static SOCKET Release(void)
{
	kfree(ghAEInfo);

	module_put(THIS_MODULE);

	return 0;
}

/* ============================================================================================== */
static void SetBayerBitWidth(DWORD dwBayerBitWidth)
{
	AutoExposure_SetBayerBitWidth(ghAEInfo, dwBayerBitWidth);
}

/* ============================================================================================== */
static TAutoExposureModule auto_exposure_module_ops = 
{
	.open = Open,
	.release = Release,
	.set_options = SetOptions,	
	.one_frame = OneFrame,
	.set_bayer_bitwidth = SetBayerBitWidth,
};

extern TAutoExposureModule *ae_dev;

/* ============================================================================================== */
static SOCKET AutoExposure_Init(void)
{
	ae_dev = &auto_exposure_module_ops;	

	printk("Install autoexposure driver version %d.%d.%d.%d complete !!\n",
		(int)(AUTOEXPOSURE_VERSION&0xFF),
		(int)((AUTOEXPOSURE_VERSION>>8)&0xFF),
		(int)((AUTOEXPOSURE_VERSION>>16)&0xFF),
		(int)((AUTOEXPOSURE_VERSION>>24)&0xFF));

	return 0;
}

/* ============================================================================================== */
static void AutoExposure_Exit(void)
{
	ae_dev = NULL;
	printk(KERN_DEBUG "[AutoExposure] : bye\n");
	return;
}

module_init(AutoExposure_Init);
module_exit(AutoExposure_Exit);
/* ============================================================================================== */
