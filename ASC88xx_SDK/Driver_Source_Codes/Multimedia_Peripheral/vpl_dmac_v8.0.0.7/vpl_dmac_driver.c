/*
 * $Header:$
 *
 * vpl_dmac
 * Driver for VPL DMAC
 *
 * Copyright (C) 2007-2012  VN Inc.
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
 * $History:$
 *
 */

/* ============================================================================================== */
#ifndef __KERNEL__
	#define __KERNEL__
#endif //!__KERNEL__

#ifndef MODULE
	#define MODULE
#endif //!MODULE

/* ============================================================================================== */
#include "vpl_dmac_driver.h"

/* ============================================================================================== */
const CHAR VPL_DMAC_ID[] = "$Version: "VPL_DMAC_ID_VERSION"  (VPL_DMAC) $";
static TVPLDMACSharedInfo *gptSharedInfo = NULL;
static SDWORD gsdwMajor = 0;

/* ============================================================================================== */
MODULE_AUTHOR ("VN Inc.");
MODULE_LICENSE ("GPL");
module_param (gsdwMajor, int, 0644);
MODULE_PARM_DESC (gsdwMajor, "Major number for VPL_DMAC module");

/* ============================================================================================== */
static irqreturn_t ISR(int irq, void *dev_id)
{
	DWORD dwReadIndex;

	PDEBUG("Enter ISR function...\n");

	dwReadIndex = VPL_DMAC_ISRHead(gptSharedInfo->hDevInfo);

	gptSharedInfo->abIntr[dwReadIndex] = TRUE;

	wake_up_interruptible(&gptSharedInfo->atWaitQueueHead[dwReadIndex]);

	VPL_DMAC_ISRTail(gptSharedInfo->hDevInfo, dwReadIndex);

	PDEBUG("Exit ISR function !!\n");

	return IRQ_HANDLED;
}

/* ============================================================================================== */
static SCODE Start(TVPLDMACObjInfo *ptObjInfo)
{
	DWORD dwWriteIndex;

	PDEBUG("Enter Start function...\n");

	dwWriteIndex = VPL_DMAC_StartHead(gptSharedInfo->hDevInfo);

	if (gptSharedInfo->abWriteEn[dwWriteIndex] == FALSE)
	{
		return S_FAIL;
	}

	gptSharedInfo->abWriteEn[dwWriteIndex] = FALSE;

	ptObjInfo->dwWriteIndex = dwWriteIndex;

	VPL_DMAC_StartTail(gptSharedInfo->hDevInfo, dwWriteIndex, ptObjInfo->ptMMRInfo);

	PDEBUG("Exit Start function !!\n");

	return S_OK;
}

/* ============================================================================================== */
static SCODE WaitComplete(TVPLDMACObjInfo *ptObjInfo)
{
	DWORD dwWriteIndex;

	PDEBUG("Enter WaitComplete function...\n");

	dwWriteIndex = ptObjInfo->dwWriteIndex;

	if (wait_event_interruptible(gptSharedInfo->atWaitQueueHead[dwWriteIndex], (gptSharedInfo->abIntr[dwWriteIndex]==TRUE)) != 0)
	{
		PDEBUG("Exit WaitComplete function by signal !!\n");
		return S_FAIL;
	}

	gptSharedInfo->abIntr[dwWriteIndex] = FALSE;

	gptSharedInfo->abWriteEn[dwWriteIndex] = TRUE;
	ptObjInfo->dwWriteIndex = 0xFFFFFFFF;

	VPL_DMAC_GetProfileInfo(gptSharedInfo->hDevInfo, ptObjInfo, dwWriteIndex);

	PDEBUG("Exit WaitComplete function !!\n");

	return S_OK;
}

/* ============================================================================================== */
static int Close(struct inode *pinode, struct file *pfile)
{
	TVPLDMACObjInfo *ptObjInfo = (TVPLDMACObjInfo *)pfile->private_data;

	PDEBUG("Enter Close function...\n");

	if (ptObjInfo != NULL)
	{
		if (ptObjInfo->dwWriteIndex != 0xFFFFFFFF)
		{
			while (1)
			{
				if (gptSharedInfo->abIntr[ptObjInfo->dwWriteIndex] == TRUE)
				{
					break;
				}
			}
			gptSharedInfo->abIntr[ptObjInfo->dwWriteIndex] = FALSE;
			gptSharedInfo->abWriteEn[ptObjInfo->dwWriteIndex] = TRUE;
		}

		iounmap(ptObjInfo->ptMMRInfo);
		release_mem_region(ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVPLDMACInfo));

		kfree(ptObjInfo);

		pfile->private_data = NULL;
	}

	module_put(THIS_MODULE);

	if (module_refcount(THIS_MODULE) == 0)
	{
		writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VPL_DMAC_CLK_EN_NUM), SYSC_CLK_EN_MMR);

		VPL_DMAC_IntrClear(gptSharedInfo->hDevInfo);
		VPL_DMAC_IntrDisable(gptSharedInfo->hDevInfo);

		if (gptSharedInfo->dwIrq != (DWORD)NULL)
		{
			free_irq(VPL_DMAC_IRQ_NUM, gptSharedInfo);
		}

		gptSharedInfo->dwIrq = (DWORD)NULL;
		writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VPL_DMAC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	}

	PDEBUG("Exit Close function !!\n");

	return 0;
}

/* ============================================================================================== */
static int Open(struct inode *pinode, struct file *pfile)
{
	TVPLDMACObjInfo *ptObjInfo;
	int scResult;
	DWORD i;

	PDEBUG("Enter Open function...\n");

	if (pfile->private_data == NULL)
	{
		if ((pfile->private_data=(TVPLDMACObjInfo *)kmalloc(sizeof(TVPLDMACObjInfo), GFP_KERNEL)) == NULL)
		{
			PDEBUG("Insufficient kernel memory space !!\n");
			PDEBUG("Exit Open function !!\n");
			return -EFAULT;
		}

		ptObjInfo = (TVPLDMACObjInfo *)pfile->private_data;
		ptObjInfo->dwWriteIndex = 0xFFFFFFFF;

		if (VPL_DMAC_InitProfileInfo(ptObjInfo) != S_OK)
		{
			PDEBUG("Fail to initialize profile info !!\n");
			PDEBUG("Exit Open function !!\n");
			return -EFAULT;
		}
	}
	else
	{
		PDEBUG("Exit Open function !!\n");
		return -EBUSY;
	}

	if (module_refcount(THIS_MODULE) == 0)
	{
		writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VPL_DMAC_CLK_EN_NUM), SYSC_CLK_EN_MMR);

		VPL_DMAC_Reset(gptSharedInfo->hDevInfo);

		if (gptSharedInfo->dwIrq == (DWORD)NULL)
		{
			VPL_DMAC_IntrDisable(gptSharedInfo->hDevInfo);

			fLib_SetIntTrig(VPL_DMAC_IRQ_NUM, LEVEL, H_ACTIVE);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
			scResult = request_irq(VPL_DMAC_IRQ_NUM, &ISR, SA_INTERRUPT, "vpl_dmac", gptSharedInfo);
#else
			scResult = request_irq(VPL_DMAC_IRQ_NUM, &ISR, IRQF_DISABLED, "vpl_dmac", gptSharedInfo);
#endif

			if (scResult < 0)
			{
				PDEBUG("Cannot get irq %d !!\n", VPL_DMAC_IRQ_NUM);
				if (try_module_get(THIS_MODULE) == 0)
				{
					PDEBUG("Exit Open function !!\n");
					return -EBUSY;
				}

				Close(pinode, pfile);
				PDEBUG("Exit Open function !!\n");
				return scResult;
			}

			disable_irq(VPL_DMAC_IRQ_NUM);
			enable_irq(VPL_DMAC_IRQ_NUM);
			gptSharedInfo->dwIrq = VPL_DMAC_IRQ_NUM;

			VPL_DMAC_IntrClear(gptSharedInfo->hDevInfo);
			VPL_DMAC_IntrEnable(gptSharedInfo->hDevInfo);

			VPL_DMAC_Open(gptSharedInfo->hDevInfo);

			for (i=0; i<VPL_DMAC_MMR_BUFF_NUM; i++)
			{
				init_waitqueue_head(&gptSharedInfo->atWaitQueueHead[i]);
				gptSharedInfo->abIntr[i] = FALSE;
				gptSharedInfo->abWriteEn[i] = TRUE;
			}
		}
		writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VPL_DMAC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	}

	if (try_module_get(THIS_MODULE) == 0)
	{
		PDEBUG("Exit Open function !!\n");
		return -EBUSY;
	}

	PDEBUG("Exit Open function !!\n");

	return 0;
}

/* ============================================================================================== */
static int Ioctl(struct inode *pinode, struct file *pfile, unsigned int dwCmd, unsigned long dwArg)
{
	TVPLDMACObjInfo *ptObjInfo = (TVPLDMACObjInfo *)pfile->private_data;
	DWORD dwVersionNum;
	int scError;
	CHAR acMemRegionName[16];

	PDEBUG("Enter Ioctl function...\n");

	if (pfile->private_data == NULL)
	{
		PDEBUG("Device does not exist !!\n");
		PDEBUG("Exit Ioctl function !!\n");
		return -ENODEV;
	}

	if ((_IOC_TYPE(dwCmd)!=VPL_DMAC_IOC_MAGIC) || (_IOC_NR(dwCmd)>VPL_DMAC_IOC_MAX_NUMBER))
	{
		PDEBUG("Incorrect ioctl command !!\n");
		PDEBUG("Exit Ioctl function !!\n");

		return -ENOTTY;
	}

	if (_IOC_DIR(dwCmd) & _IOC_READ)
	{
		scError = !access_ok(VERIFY_WRITE, (void *)dwArg, _IOC_SIZE(dwCmd));
	}
	else if (_IOC_DIR(dwCmd) & _IOC_WRITE)
	{
		scError = !access_ok(VERIFY_READ, (void *)dwArg, _IOC_SIZE(dwCmd));
	}
	else
	{
		scError = 0;
	}

	if (scError != 0)
	{
		PDEBUG("Unsupport ioctl command %d !!\n", dwCmd);
		PDEBUG("Exit Ioctl function !!\n");

		return -EFAULT;
	}

	switch (dwCmd)
	{
		case VPL_DMAC_IOC_START:
			if (Start(ptObjInfo) != S_OK)
			{
				PDEBUG("Exit Ioctl function !!\n");
				return S_FAIL;
			}
		break;
		case VPL_DMAC_IOC_WAIT_COMPLETE:
			if (WaitComplete(ptObjInfo) != S_OK)
			{
				PDEBUG("Exit Ioctl function !!\n");
				return S_FAIL;
			}
		break;
		case VPL_DMAC_IOC_SHARE_MMR_INFO_SPACE:
			ptObjInfo->dwMMRInfoPhyAddr = dwArg;
			sprintf(acMemRegionName, "%d", (int)ptObjInfo->dwMMRInfoPhyAddr);
			request_mem_region(ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVPLDMACInfo), acMemRegionName);
			ptObjInfo->ptMMRInfo = (TVPLDMACInfo *)ioremap((int)ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVPLDMACInfo));
		break;
		case VPL_DMAC_IOC_GET_VERSION_NUMBER:
			dwVersionNum = VPL_DMAC_VERSION;
			copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
		break;
		case VPL_DMAC_IOC_MASTER_0_GET_BANDWIDTH:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_0_GET_RG_INTERVAL:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_0_GET_REQ_TIMES:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_0_CLEAR_PROFILE:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_1_GET_BANDWIDTH:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_1_GET_RG_INTERVAL:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_1_GET_REQ_TIMES:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_1_CLEAR_PROFILE:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_2_GET_BANDWIDTH:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_2_GET_RG_INTERVAL:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_2_GET_REQ_TIMES:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VPL_DMAC_IOC_MASTER_2_CLEAR_PROFILE:
			VPL_DMAC_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		default:
			PDEBUG("Exit Ioctl function !!\n");
			return -ENOTTY;
	}

	PDEBUG("Exit Ioctl function !!\n");

	return 0;
}

/* ============================================================================================== */
static int MMap(struct file *file, struct vm_area_struct *vma)
{
	DWORD dwSize;

	dwSize = vma->vm_end - vma->vm_start;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_RESERVED;

	if (remap_pfn_range(vma, vma->vm_start, (VPL_DMAC_MMR_BASE>>PAGE_SHIFT), dwSize, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	PDEBUG("Start address = 0x%08lX, end address = 0x%08lX\n", vma->vm_start, vma->vm_end);

	return 0;
}

/* ============================================================================================== */
struct file_operations vpl_dmac_fops =
{
	ioctl:		Ioctl,
	mmap:		MMap,
	open:		Open,
	release:	Close,
};

/* ============================================================================================== */
static void CleanupModule(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	int scResult;
#endif
	TVPLDMACInfo *ptMMRInfo;

	PDEBUG("Enter CleanupModule function...\n");

	if (gptSharedInfo != NULL)
	{
		if (gptSharedInfo->hDevInfo != NULL)
		{
			if (gsdwMajor != 0)
			{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
				scResult = unregister_chrdev(gsdwMajor, "vpl_dmac");

				if (scResult < 0)
				{
					PDEBUG("Cannot release major number %d !!\n", (int)gsdwMajor);
					PDEBUG("Exit CleanupModule function !!\n");
					return;
				}
#else
				unregister_chrdev(gsdwMajor, "vpl_dmac");
#endif
			}

			VPL_DMAC_CloseProfile(gptSharedInfo->hDevInfo);
			ptMMRInfo = (TVPLDMACInfo *)VPL_DMAC_GetMMRInfo(gptSharedInfo->hDevInfo);

			if (ptMMRInfo != NULL)
			{
				iounmap(ptMMRInfo);
				release_mem_region(VPL_DMAC_MMR_BASE, sizeof(TVPLDMACInfo));
			}

			kfree(gptSharedInfo->hDevInfo);
		}

		kfree(gptSharedInfo);
	}

	PDEBUG("Exit CleanupModule function !!\n");

	return;
}

/* ============================================================================================== */
static int InitialModule(void)
{
	int scResult;
	DWORD dwDevInfoSize, dwVersionNum;
	volatile TVPLDMACInfo *ptMMRInfo = (TVPLDMACInfo *)ioremap((int)VPL_DMAC_MMR_BASE, sizeof(TVPLDMACInfo));
	volatile DWORD *pdwClkEnMmr = (DWORD *)(IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24);

	PDEBUG("Enter InitialModule function...\n");

	if ((gptSharedInfo=(TVPLDMACSharedInfo *)kmalloc(sizeof(TVPLDMACSharedInfo), GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate shared info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptSharedInfo, 0, sizeof(TVPLDMACSharedInfo));

	dwDevInfoSize = VPL_DMAC_GetDevInfoSize();

	if ((gptSharedInfo->hDevInfo=(HANDLE)kmalloc(dwDevInfoSize, GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate device info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptSharedInfo->hDevInfo, 0, dwDevInfoSize);

	writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VPL_DMAC_CLK_EN_NUM), SYSC_CLK_EN_MMR);
	request_mem_region(VPL_DMAC_MMR_BASE, sizeof(TVPLDMACInfo), "VPL_DMAC");

	if (VPL_DMAC_SetMMRInfo(gptSharedInfo->hDevInfo, ptMMRInfo, pdwClkEnMmr) != S_OK)
	{
		scResult = -ENODEV;
		goto FAIL;
	}

	VPL_DMAC_InitProfile(gptSharedInfo->hDevInfo);

	scResult = register_chrdev(gsdwMajor, "vpl_dmac", &vpl_dmac_fops);

	if (scResult < 0)
	{
		PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
		goto FAIL;
	}

	if (gsdwMajor == 0)
	{
		gsdwMajor = scResult;
	}

	dwVersionNum = VPL_DMAC_GetVersion(gptSharedInfo->hDevInfo);

	printk("Install VPL_DMAC device driver version %d.%d.%d.%d on VPL_DMAC hardware version %d.%d.%d.%d complete !!\n",
	       (int)(VPL_DMAC_VERSION&0xFF),
	       (int)((VPL_DMAC_VERSION>>8)&0xFF),
	       (int)((VPL_DMAC_VERSION>>16)&0xFF),
	       (int)((VPL_DMAC_VERSION>>24)&0xFF),
	       (int)(dwVersionNum>>24)&0xFF,
	       (int)(dwVersionNum>>16)&0xFF,
	       (int)(dwVersionNum>>8)&0xFF,
	       (int)dwVersionNum&0xFF);

	writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VPL_DMAC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);

	PDEBUG("Exit InitialModule function !!\n");

	return 0;

FAIL:
	writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VPL_DMAC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	CleanupModule();
	PDEBUG("Exit InitialModule function !!\n");

	return scResult;
}

/* ============================================================================================== */
module_init(InitialModule);
module_exit(CleanupModule);

/* ============================================================================================== */
