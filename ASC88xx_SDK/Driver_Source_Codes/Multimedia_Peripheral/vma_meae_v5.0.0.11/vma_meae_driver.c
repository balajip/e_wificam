/*
 * $Header: /rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE/vma_meae_driver.c 4     13/05/28 11:11a Jaja $
 *
 * vma_meae
 * Driver for VMA MEAE
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
 * $History: vma_meae_driver.c $
 * 
 * *****************  Version 4  *****************
 * User: Jaja         Date: 13/05/28   Time: 11:11a
 * Updated in $/rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE
 *
 * *****************  Version 3  *****************
 * User: Angel        Date: 12/06/21   Time: 6:05p
 * Updated in $/rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE
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
#include "vma_meae_driver.h"

/* ============================================================================================== */
const CHAR VMA_MEAE_ID[] = "$Version: "VMA_MEAE_ID_VERSION"  (VMA_MEAE) $";
static TVMAMEAESharedInfo *gptSharedInfo = NULL;
static SDWORD gsdwMajor = 0;

/* ============================================================================================== */
MODULE_AUTHOR ("VN Inc.");
MODULE_LICENSE ("GPL");
module_param (gsdwMajor, int, 0644);
MODULE_PARM_DESC (gsdwMajor, "Major number for VMA_MEAE module");

/* ============================================================================================== */
static irqreturn_t ISR(int irq, void *dev_id)
{
	DWORD dwReadIndex;

	PDEBUG("Enter ISR function...\n");

	dwReadIndex = VMA_MEAE_ISRHead(gptSharedInfo->hDevInfo);

	gptSharedInfo->abIntr[dwReadIndex] = TRUE;

	wake_up_interruptible(&gptSharedInfo->atWaitQueueHead[dwReadIndex]);

	VMA_MEAE_ISRTail(gptSharedInfo->hDevInfo, dwReadIndex);

	PDEBUG("Exit ISR function !!\n");

	return IRQ_HANDLED;
}

/* ============================================================================================== */
static SCODE Start(TVMAMEAEObjInfo *ptObjInfo)
{
	DWORD dwWriteIndex;

	PDEBUG("Enter Start function...\n");

	dwWriteIndex = VMA_MEAE_StartHead(gptSharedInfo->hDevInfo);

	if (gptSharedInfo->abWriteEn[dwWriteIndex] == FALSE)
	{
		return S_FAIL;
	}

	gptSharedInfo->abWriteEn[dwWriteIndex] = FALSE;

	ptObjInfo->dwWriteIndex = dwWriteIndex;

	VMA_MEAE_StartTail(gptSharedInfo->hDevInfo, dwWriteIndex, ptObjInfo->ptMMRInfo);

	PDEBUG("Exit Start function !!\n");

	return S_OK;
}

/* ============================================================================================== */
static SCODE WaitComplete(TVMAMEAEObjInfo *ptObjInfo)
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

	VMA_MEAE_GetProfileInfo(gptSharedInfo->hDevInfo, ptObjInfo, dwWriteIndex);

	PDEBUG("Exit WaitComplete function !!\n");

	return S_OK;
}

/* ============================================================================================== */
static int Close(struct inode *pinode, struct file *pfile)
{
	TVMAMEAEObjInfo *ptObjInfo = (TVMAMEAEObjInfo *)pfile->private_data;

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
		release_mem_region(ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVMAMEAEInfo));

		kfree(ptObjInfo);

		pfile->private_data = NULL;
	}

	module_put(THIS_MODULE);

	if (module_refcount(THIS_MODULE) == 0)
	{
		writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_MEAE_CLK_EN_NUM), SYSC_CLK_EN_MMR);

		VMA_MEAE_IntrClear(gptSharedInfo->hDevInfo);
		VMA_MEAE_IntrDisable(gptSharedInfo->hDevInfo);

		if (gptSharedInfo->dwIrq != (DWORD)NULL)
		{
			free_irq(VMA_MEAE_IRQ_NUM, gptSharedInfo);
		}

		gptSharedInfo->dwIrq = (DWORD)NULL;
		writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_MEAE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	}

	PDEBUG("Exit Close function !!\n");

	return 0;
}

/* ============================================================================================== */
static int Open(struct inode *pinode, struct file *pfile)
{
	TVMAMEAEObjInfo *ptObjInfo;
	int scResult;
	DWORD i;

	PDEBUG("Enter Open function...\n");

	if (pfile->private_data == NULL)
	{
		if ((pfile->private_data=(TVMAMEAEObjInfo *)kmalloc(sizeof(TVMAMEAEObjInfo), GFP_KERNEL)) == NULL)
		{
			PDEBUG("Insufficient kernel memory space !!\n");
			PDEBUG("Exit Open function !!\n");
			return -EFAULT;
		}

		ptObjInfo = (TVMAMEAEObjInfo *)pfile->private_data;
		ptObjInfo->dwWriteIndex = 0xFFFFFFFF;

		if (VMA_MEAE_InitProfileInfo(ptObjInfo) != S_OK)
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
		writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_MEAE_CLK_EN_NUM), SYSC_CLK_EN_MMR);

		VMA_MEAE_Reset(gptSharedInfo->hDevInfo);

		if (gptSharedInfo->dwIrq == (DWORD)NULL)
		{
			VMA_MEAE_IntrDisable(gptSharedInfo->hDevInfo);

			fLib_SetIntTrig(VMA_MEAE_IRQ_NUM, LEVEL, H_ACTIVE);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
			scResult = request_irq(VMA_MEAE_IRQ_NUM, &ISR, SA_INTERRUPT, "vma_meae", gptSharedInfo);
#else
			scResult = request_irq(VMA_MEAE_IRQ_NUM, &ISR, IRQF_DISABLED, "vma_meae", gptSharedInfo);
#endif

			if (scResult < 0)
			{
				PDEBUG("Cannot get irq %d !!\n", VMA_MEAE_IRQ_NUM);
				if (try_module_get(THIS_MODULE) == 0)
				{
					PDEBUG("Exit Open function !!\n");
					return -EBUSY;
				}

				Close(pinode, pfile);
				PDEBUG("Exit Open function !!\n");
				return scResult;
			}

			disable_irq(VMA_MEAE_IRQ_NUM);
			enable_irq(VMA_MEAE_IRQ_NUM);
			gptSharedInfo->dwIrq = VMA_MEAE_IRQ_NUM;

			VMA_MEAE_IntrClear(gptSharedInfo->hDevInfo);
			VMA_MEAE_IntrEnable(gptSharedInfo->hDevInfo);

			VMA_MEAE_Open(gptSharedInfo->hDevInfo);

			for (i=0; i<VMA_MEAE_MMR_BUFF_NUM; i++)
			{
				init_waitqueue_head(&gptSharedInfo->atWaitQueueHead[i]);
				gptSharedInfo->abIntr[i] = FALSE;
				gptSharedInfo->abWriteEn[i] = TRUE;
			}
		}
		writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_MEAE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
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
	TVMAMEAEObjInfo *ptObjInfo = (TVMAMEAEObjInfo *)pfile->private_data;
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

	if ((_IOC_TYPE(dwCmd)!=VMA_MEAE_IOC_MAGIC) || (_IOC_NR(dwCmd)>VMA_MEAE_IOC_MAX_NUMBER))
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
		case VMA_MEAE_IOC_START:
			if (Start(ptObjInfo) != S_OK)
			{
				PDEBUG("Exit Ioctl function !!\n");
				return S_FAIL;
			}
		break;
		case VMA_MEAE_IOC_WAIT_COMPLETE:
			if (WaitComplete(ptObjInfo) != S_OK)
			{
				PDEBUG("Exit Ioctl function !!\n");
				return S_FAIL;
			}
		break;
		case VMA_MEAE_IOC_SHARE_MMR_INFO_SPACE:
			ptObjInfo->dwMMRInfoPhyAddr = dwArg;
			sprintf(acMemRegionName, "%d", (int)ptObjInfo->dwMMRInfoPhyAddr);
			request_mem_region(ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVMAMEAEInfo), acMemRegionName);
			ptObjInfo->ptMMRInfo = (TVMAMEAEInfo *)ioremap((int)ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVMAMEAEInfo));
		break;
		case VMA_MEAE_IOC_GET_VERSION_NUMBER:
			dwVersionNum = VMA_MEAE_VERSION;
			copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
		break;
		case VMA_MEAE_IOC_GET_BANDWIDTH:
			VMA_MEAE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_MEAE_IOC_GET_RG_INTERVAL:
			VMA_MEAE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_MEAE_IOC_GET_REQ_TIMES:
			VMA_MEAE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_MEAE_IOC_CLEAR_PROFILE:
			VMA_MEAE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_MEAE_IOC_RESET:
			VMA_MEAE_Reset(gptSharedInfo->hDevInfo);
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

	if (remap_pfn_range(vma, vma->vm_start, (VMA_MEAE_MMR_BASE>>PAGE_SHIFT), dwSize, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	PDEBUG("Start address = 0x%08lX, end address = 0x%08lX\n", vma->vm_start, vma->vm_end);

	return 0;
}

/* ============================================================================================== */
struct file_operations vma_meae_fops =
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
	TVMAMEAEInfo *ptMMRInfo;

	PDEBUG("Enter CleanupModule function...\n");

	if (gptSharedInfo != NULL)
	{
		if (gptSharedInfo->hDevInfo != NULL)
		{
			if (gsdwMajor != 0)
			{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
				scResult = unregister_chrdev(gsdwMajor, "vma_meae");

				if (scResult < 0)
				{
					PDEBUG("Cannot release major number %d !!\n", (int)gsdwMajor);
					PDEBUG("Exit CleanupModule function !!\n");
					return;
				}
#else
				unregister_chrdev(gsdwMajor, "vma_meae");
#endif
			}

			VMA_MEAE_CloseProfile(gptSharedInfo->hDevInfo);
			ptMMRInfo = (TVMAMEAEInfo *)VMA_MEAE_GetMMRInfo(gptSharedInfo->hDevInfo);

			if (ptMMRInfo != NULL)
			{
				iounmap(ptMMRInfo);
				release_mem_region(VMA_MEAE_MMR_BASE, sizeof(TVMAMEAEInfo));
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
	volatile TVMAMEAEInfo *ptMMRInfo = (TVMAMEAEInfo *)ioremap((int)VMA_MEAE_MMR_BASE, sizeof(TVMAMEAEInfo));
	volatile DWORD *pdwClkEnMmr = (DWORD *)(IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24);

	PDEBUG("Enter InitialModule function...\n");

	if ((gptSharedInfo=(TVMAMEAESharedInfo *)kmalloc(sizeof(TVMAMEAESharedInfo), GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate shared info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptSharedInfo, 0, sizeof(TVMAMEAESharedInfo));

	dwDevInfoSize = VMA_MEAE_GetDevInfoSize();

	if ((gptSharedInfo->hDevInfo=(HANDLE)kmalloc(dwDevInfoSize, GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate device info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptSharedInfo->hDevInfo, 0, dwDevInfoSize);

	writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_MEAE_CLK_EN_NUM), SYSC_CLK_EN_MMR);
	request_mem_region(VMA_MEAE_MMR_BASE, sizeof(TVMAMEAEInfo), "VMA_MEAE");

	if (VMA_MEAE_SetMMRInfo(gptSharedInfo->hDevInfo, ptMMRInfo, pdwClkEnMmr) != S_OK)
	{
		scResult = -ENODEV;
		goto FAIL;
	}

	VMA_MEAE_InitProfile(gptSharedInfo->hDevInfo);

	scResult = register_chrdev(gsdwMajor, "vma_meae", &vma_meae_fops);

	if (scResult < 0)
	{
		PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
		goto FAIL;
	}

	if (gsdwMajor == 0)
	{
		gsdwMajor = scResult;
	}

	dwVersionNum = VMA_MEAE_GetVersion(gptSharedInfo->hDevInfo);

	printk("Install VMA_MEAE device driver version %d.%d.%d.%d on VMA_MEAE hardware version %d.%d.%d.%d complete !!\n",
	       (int)(VMA_MEAE_VERSION&0xFF),
	       (int)((VMA_MEAE_VERSION>>8)&0xFF),
	       (int)((VMA_MEAE_VERSION>>16)&0xFF),
	       (int)((VMA_MEAE_VERSION>>24)&0xFF),
	       (int)(dwVersionNum>>24)&0xFF,
	       (int)(dwVersionNum>>16)&0xFF,
	       (int)(dwVersionNum>>8)&0xFF,
	       (int)dwVersionNum&0xFF);

	writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_MEAE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);

	PDEBUG("Exit InitialModule function !!\n");

	return 0;

FAIL:
	writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_MEAE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	CleanupModule();
	PDEBUG("Exit InitialModule function !!\n");

	return scResult;
}

/* ============================================================================================== */
module_init(InitialModule);
module_exit(CleanupModule);

/* ============================================================================================== */
