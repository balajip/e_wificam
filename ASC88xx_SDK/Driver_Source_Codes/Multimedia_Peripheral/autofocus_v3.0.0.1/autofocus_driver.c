/*
 * autofocus
 * Driver for autofocus.
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
 
/* ============================================================================================== */
#include <linux/moduleparam.h>
#include "autofocus_driver.h"

const CHAR AUTOFOCUS_ID[] = "$Version: "AUTOFOCUS_ID_VERSION"  (AUTOFOCUS) $";

static TAutofocusSharedInfo *gptSharedInfo = NULL;

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("autofocus driver");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
SOCKET OneFrame(TAutoFocusState *ptState)
{
	SOCKET sckRet = 0;

	sckRet = AutoFocus_OneFrame(gptSharedInfo, ptState);

	return sckRet;
}

/* ============================================================================================== */
SOCKET SetOptions(TVideoSignalOptions *ptArg)
{
	SOCKET sckRet = 0;
	DWORD *dwBaseAddr;

	if (ptArg->eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_TABLE)
	{
		 dwBaseAddr = (DWORD *)ioremap((int)ptArg->adwUserData[0], AutoFocus_GetCalibrateSize(gptSharedInfo));
		 ptArg->adwUserData[0] = (DWORD)(&(dwBaseAddr[0]));
	}

	sckRet = AutoFocus_SetOptions(gptSharedInfo, ptArg);

	if (ptArg->eOptionFlags == VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_CALIBRATE)
	{
		while (!AutoFocus_WaitComplete(gptSharedInfo))
		{
			yield();
		}
	}

	return sckRet;
}

/* ============================================================================================== */
SOCKET Release(void)
{
	if (gptSharedInfo != NULL)
	{
		if (gptSharedInfo->hZtInfo != NULL)
		{
			kfree(gptSharedInfo->hZtInfo);
		}

		if (gptSharedInfo->hAFInfo != NULL)
		{
			kfree(gptSharedInfo->hAFInfo);
		}
		kfree(gptSharedInfo);
	}

	module_put(THIS_MODULE);

	return 0;
}

/* ============================================================================================== */
SOCKET Open(TAutoFocusInitOptions* ptAFInitialParam)
{
	SOCKET sckRet = 0;

	DWORD dwDevInfoSize, dwZtInfoSize;

	if ((gptSharedInfo=(TAutofocusSharedInfo *)kmalloc(sizeof(TAutofocusSharedInfo), GFP_KERNEL)) == NULL)
	{
		printk("Allocate shared info buffer fail !!\n");
		return -ENOMEM;
	}

	memset(gptSharedInfo, 0, sizeof(TAutofocusSharedInfo));

	dwDevInfoSize = AutoFocus_GetDevInfoSize();

	if ((gptSharedInfo->hAFInfo=(HANDLE)kmalloc(dwDevInfoSize, GFP_KERNEL)) == NULL)
	{
		printk("[AutoFocus] : Allocate %d bytes memory fail\n", (int)dwDevInfoSize);
		Release();
		return -ENOMEM;
	}	
	memset(gptSharedInfo->hAFInfo, 0, dwDevInfoSize);

	sckRet = AutoFocus_Initial(gptSharedInfo, ptAFInitialParam);

	if (sckRet == S_OK)
	{
/* Version 3.0.0.1 modification, 2013.01.10 */
		dwZtInfoSize = AutoFocus_GetZtInfoSize(gptSharedInfo);
		if ((gptSharedInfo->hZtInfo=(HANDLE)kmalloc(dwZtInfoSize, GFP_KERNEL)) == NULL)
		{
			printk("[AutoFocus] : Allocate %d bytes memory fail\n", (int)dwZtInfoSize);
			Release();
			return -ENOMEM;
		}	
		memset(gptSharedInfo->hZtInfo, 0, dwZtInfoSize);
/* ======================================== */

		AutoFocus_Reset(gptSharedInfo, ptAFInitialParam);
	}

	if (try_module_get(THIS_MODULE) == 0)
	{
		return -EBUSY;
	}
	return sckRet;
}

/* ============================================================================================== */
static TAutoFocusModule auto_focus_module_ops =
{
	.open = Open,
	.release = Release,
	.set_options = SetOptions,	
	.one_frame = OneFrame,
};

extern TAutoFocusModule *af_dev;

/* ============================================================================================== */
static SOCKET AutoFocus_Init(void)
{
	af_dev = &auto_focus_module_ops;	

	printk("Install autofocus driver version %d.%d.%d.%d complete !!\n",
		(int)(AUTOFOCUS_VERSION&0xFF),
		(int)((AUTOFOCUS_VERSION>>8)&0xFF),
		(int)((AUTOFOCUS_VERSION>>16)&0xFF),
		(int)((AUTOFOCUS_VERSION>>24)&0xFF));

	return 0;
}

/* ============================================================================================== */
static void AutoFocus_Exit(void)
{
	af_dev = NULL;
	printk(KERN_DEBUG "[AutoFocus] : bye\n");
	return;
}

module_init(AutoFocus_Init);
module_exit(AutoFocus_Exit);
/* ============================================================================================== */
