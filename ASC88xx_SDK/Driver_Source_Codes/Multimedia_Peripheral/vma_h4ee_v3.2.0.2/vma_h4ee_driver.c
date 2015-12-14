/*
 * $Header: /rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE/vma_h4ee_driver.c 6     12/09/11 10:52a Jaja $
 *
 * vma_h4ee
 * Driver for VMA H4EE
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
 * $History: vma_h4ee_driver.c $
 * 
 * *****************  Version 6  *****************
 * User: Jaja         Date: 12/09/11   Time: 10:52a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 5  *****************
 * User: Jaja         Date: 12/09/07   Time: 8:35a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 4  *****************
 * User: Jaja         Date: 12/09/07   Time: 8:23a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 3  *****************
 * User: Jaja         Date: 12/08/08   Time: 10:52a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 2  *****************
 * User: Jaja         Date: 12/06/20   Time: 5:54p
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 1  *****************
 * User: Jaja         Date: 12/01/11   Time: 7:03p
 * Created in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
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
#include "vma_h4ee_driver.h"

/* ============================================================================================== */
const CHAR VMA_H4EE_ID[] = "$Version: "VMA_H4EE_ID_VERSION"  (VMA_H4EE) $";
static TVMAH4EESharedInfo *gptSharedInfo = NULL;
static SDWORD gsdwMajor = 0;

/* ============================================================================================== */
MODULE_AUTHOR ("VN Inc.");
MODULE_LICENSE ("GPL");
module_param (gsdwMajor, int, 0644);
MODULE_PARM_DESC (gsdwMajor, "Major number for VMA_H4EE module");

/* ============================================================================================== */
static irqreturn_t ISR(int irq, void *dev_id)
{
	DWORD dwReadIndex;

	PDEBUG("Enter ISR function...\n");

	dwReadIndex = VMA_H4EE_ISRHead(gptSharedInfo->hDevInfo);

	gptSharedInfo->abIntr[dwReadIndex] = TRUE;

	wake_up_interruptible(&gptSharedInfo->atWaitQueueHead[dwReadIndex]);

	VMA_H4EE_ISRTail(gptSharedInfo->hDevInfo, dwReadIndex);

	PDEBUG("Exit ISR function !!\n");

	return IRQ_HANDLED;
}

/* ============================================================================================== */
static SCODE Start(TVMAH4EEObjInfo *ptObjInfo)
{
	DWORD dwWriteIndex;

	PDEBUG("Enter Start function...\n");

	dwWriteIndex = VMA_H4EE_StartHead(gptSharedInfo->hDevInfo);

	if (gptSharedInfo->abWriteEn[dwWriteIndex] == FALSE)
	{
		return S_FAIL;
	}

	gptSharedInfo->abWriteEn[dwWriteIndex] = FALSE;

	ptObjInfo->dwWriteIndex = dwWriteIndex;

	VMA_H4EE_StartTail(gptSharedInfo->hDevInfo, dwWriteIndex, ptObjInfo->ptMMRInfo);

	PDEBUG("Exit Start function !!\n");

	return S_OK;
}

/* ============================================================================================== */
static SCODE WaitComplete(TVMAH4EEObjInfo *ptObjInfo)
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

	VMA_H4EE_GetProfileInfo(gptSharedInfo->hDevInfo, ptObjInfo, dwWriteIndex);

	PDEBUG("Exit WaitComplete function !!\n");

	return S_OK;
}

/* ============================================================================================== */
static int Close(struct inode *pinode, struct file *pfile)
{
	TVMAH4EEObjInfo *ptObjInfo = (TVMAH4EEObjInfo *)pfile->private_data;

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
		release_mem_region(ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVMAH4EEInfo));

		kfree(ptObjInfo);

		pfile->private_data = NULL;
	}

	module_put(THIS_MODULE);

	if (module_refcount(THIS_MODULE) == 0)
	{
		writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_H4EE_CLK_EN_NUM), SYSC_CLK_EN_MMR);

		VMA_H4EE_IntrClear(gptSharedInfo->hDevInfo);
		VMA_H4EE_IntrDisable(gptSharedInfo->hDevInfo);

		if (gptSharedInfo->dwIrq != (DWORD)NULL)
		{
			free_irq(VMA_H4EE_IRQ_NUM, gptSharedInfo);
		}

		gptSharedInfo->dwIrq = (DWORD)NULL;
		writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_H4EE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	}

	PDEBUG("Exit Close function !!\n");

	return 0;
}

/* ============================================================================================== */
static int Open(struct inode *pinode, struct file *pfile)
{
	TVMAH4EEObjInfo *ptObjInfo;
	int scResult;
	DWORD i;

	PDEBUG("Enter Open function...\n");

	if (pfile->private_data == NULL)
	{
		if ((pfile->private_data=(TVMAH4EEObjInfo *)kmalloc(sizeof(TVMAH4EEObjInfo), GFP_KERNEL)) == NULL)
		{
			PDEBUG("Insufficient kernel memory space !!\n");
			PDEBUG("Exit Open function !!\n");
			return -EFAULT;
		}

		ptObjInfo = (TVMAH4EEObjInfo *)pfile->private_data;
		ptObjInfo->dwWriteIndex = 0xFFFFFFFF;

		if (VMA_H4EE_InitProfileInfo(ptObjInfo) != S_OK)
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
		writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_H4EE_CLK_EN_NUM), SYSC_CLK_EN_MMR);

		VMA_H4EE_Reset(gptSharedInfo->hDevInfo);

		if (gptSharedInfo->dwIrq == (DWORD)NULL)
		{
			VMA_H4EE_IntrDisable(gptSharedInfo->hDevInfo);

			fLib_SetIntTrig(VMA_H4EE_IRQ_NUM, LEVEL, H_ACTIVE);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
			scResult = request_irq(VMA_H4EE_IRQ_NUM, &ISR, SA_INTERRUPT, "vma_h4ee", gptSharedInfo);
#else
			scResult = request_irq(VMA_H4EE_IRQ_NUM, &ISR, IRQF_DISABLED, "vma_h4ee", gptSharedInfo);
#endif

			if (scResult < 0)
			{
				PDEBUG("Cannot get irq %d !!\n", VMA_H4EE_IRQ_NUM);
				if (try_module_get(THIS_MODULE) == 0)
				{
					PDEBUG("Exit Open function !!\n");
					return -EBUSY;
				}

				Close(pinode, pfile);
				PDEBUG("Exit Open function !!\n");
				return scResult;
			}

			disable_irq(VMA_H4EE_IRQ_NUM);
			enable_irq(VMA_H4EE_IRQ_NUM);
			gptSharedInfo->dwIrq = VMA_H4EE_IRQ_NUM;

			VMA_H4EE_IntrClear(gptSharedInfo->hDevInfo);
			VMA_H4EE_IntrEnable(gptSharedInfo->hDevInfo);

			VMA_H4EE_Open(gptSharedInfo->hDevInfo);

			for (i=0; i<VMA_H4EE_MMR_BUFF_NUM; i++)
			{
				init_waitqueue_head(&gptSharedInfo->atWaitQueueHead[i]);
				gptSharedInfo->abIntr[i] = FALSE;
				gptSharedInfo->abWriteEn[i] = TRUE;
			}
		}
		writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_H4EE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
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
	TVMAH4EEObjInfo *ptObjInfo = (TVMAH4EEObjInfo *)pfile->private_data;
	DWORD dwVersionNum;
	int scError;
	CHAR acMemRegionName[16];
	DWORD *pdwSysCtrl;

	PDEBUG("Enter Ioctl function...\n");

	if (pfile->private_data == NULL)
	{
		PDEBUG("Device does not exist !!\n");
		PDEBUG("Exit Ioctl function !!\n");
		return -ENODEV;
	}

	if ((_IOC_TYPE(dwCmd)!=VMA_H4EE_IOC_MAGIC) || (_IOC_NR(dwCmd)>VMA_H4EE_IOC_MAX_NUMBER))
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
		case VMA_H4EE_IOC_START:
			if (Start(ptObjInfo) != S_OK)
			{
				PDEBUG("Exit Ioctl function !!\n");
				return S_FAIL;
			}
		break;
		case VMA_H4EE_IOC_WAIT_COMPLETE:
			if (WaitComplete(ptObjInfo) != S_OK)
			{
				PDEBUG("Exit Ioctl function !!\n");
				return S_FAIL;
			}
		break;
		case VMA_H4EE_IOC_SHARE_MMR_INFO_SPACE:
			ptObjInfo->dwMMRInfoPhyAddr = dwArg;
			sprintf(acMemRegionName, "%d", (int)ptObjInfo->dwMMRInfoPhyAddr);
			request_mem_region(ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVMAH4EEInfo), acMemRegionName);
			ptObjInfo->ptMMRInfo = (TVMAH4EEInfo *)ioremap((int)ptObjInfo->dwMMRInfoPhyAddr, sizeof(TVMAH4EEInfo));
		break;
		case VMA_H4EE_IOC_GET_VERSION_NUMBER:
			dwVersionNum = VMA_H4EE_VERSION;
			copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
		break;
		case VMA_H4EE_IOC_GET_CHIP_VERSION_NUMBER:
			copy_to_user((DWORD *)dwArg, (DWORD *)IO_ADDRESS(VPL_SYSC_MMR_BASE), sizeof(DWORD));
		break;
		case VMA_H4EE_IOC_GET_CHIP_MODEL_INFO:
			pdwSysCtrl = (DWORD *)IO_ADDRESS(VPL_SYSC_MMR_BASE);
			if (readl(pdwSysCtrl)==0x08040001)
			{	// Mozart v3
/* Version 3.2.0.1 modification, 2012.09.07 */
				copy_to_user((DWORD *)dwArg, (DWORD *)IO_ADDRESS((VPL_SYSC_MMR_BASE + 60)), sizeof(DWORD));
/* ======================================== */
			}
			else
			{																					// V2
				copy_to_user((DWORD *)dwArg, (DWORD *)IO_ADDRESS((VPL_SYSC_MMR_BASE + 60 + ((readl(pdwSysCtrl)==0x8010C02)*4))), sizeof(DWORD));
			}
		break;
		case VMA_H4EE_IOC_MASTER_0_GET_BANDWIDTH:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_0_GET_RG_INTERVAL:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_0_GET_REQ_TIMES:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_0_CLEAR_PROFILE:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_1_GET_BANDWIDTH:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_1_GET_RG_INTERVAL:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_1_GET_REQ_TIMES:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_1_CLEAR_PROFILE:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_2_GET_BANDWIDTH:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_2_GET_RG_INTERVAL:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_2_GET_REQ_TIMES:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_2_CLEAR_PROFILE:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_3_GET_BANDWIDTH:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_3_GET_RG_INTERVAL:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_3_GET_REQ_TIMES:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_MASTER_3_CLEAR_PROFILE:
			VMA_H4EE_SetupProfile(ptObjInfo, dwArg, dwCmd);
		break;
		case VMA_H4EE_IOC_RESET:
			writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_H4EE_CLK_EN_NUM), SYSC_CLK_EN_MMR);
			VMA_H4EE_Reset(gptSharedInfo->hDevInfo);
			writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_H4EE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
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

	if (remap_pfn_range(vma, vma->vm_start, (VMA_H4EE_MMR_BASE>>PAGE_SHIFT), dwSize, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	PDEBUG("Start address = 0x%08lX, end address = 0x%08lX\n", vma->vm_start, vma->vm_end);

	return 0;
}

/* ============================================================================================== */
struct file_operations vma_h4ee_fops =
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
	TVMAH4EEInfo *ptMMRInfo;

	PDEBUG("Enter CleanupModule function...\n");

	if (gptSharedInfo != NULL)
	{
		if (gptSharedInfo->hDevInfo != NULL)
		{
			if (gsdwMajor != 0)
			{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
				scResult = unregister_chrdev(gsdwMajor, "vma_h4ee");

				if (scResult < 0)
				{
					PDEBUG("Cannot release major number %d !!\n", (int)gsdwMajor);
					PDEBUG("Exit CleanupModule function !!\n");
					return;
				}
#else
				unregister_chrdev(gsdwMajor, "vma_h4ee");
#endif
			}

			VMA_H4EE_CloseProfile(gptSharedInfo->hDevInfo);
			ptMMRInfo = (TVMAH4EEInfo *)VMA_H4EE_GetMMRInfo(gptSharedInfo->hDevInfo);

			if (ptMMRInfo != NULL)
			{
				iounmap(ptMMRInfo);
				release_mem_region(VMA_H4EE_MMR_BASE, sizeof(TVMAH4EEInfo));
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
	volatile TVMAH4EEInfo *ptMMRInfo = (TVMAH4EEInfo *)ioremap((int)VMA_H4EE_MMR_BASE, sizeof(TVMAH4EEInfo));
	volatile DWORD *pdwClkEnMmr = (DWORD *)(IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24);

	PDEBUG("Enter InitialModule function...\n");

	if ((gptSharedInfo=(TVMAH4EESharedInfo *)kmalloc(sizeof(TVMAH4EESharedInfo), GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate shared info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptSharedInfo, 0, sizeof(TVMAH4EESharedInfo));

	dwDevInfoSize = VMA_H4EE_GetDevInfoSize();

	if ((gptSharedInfo->hDevInfo=(HANDLE)kmalloc(dwDevInfoSize, GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate device info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptSharedInfo->hDevInfo, 0, dwDevInfoSize);

	writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VMA_H4EE_CLK_EN_NUM), SYSC_CLK_EN_MMR);
	request_mem_region(VMA_H4EE_MMR_BASE, sizeof(TVMAH4EEInfo), "VMA_H4EE");

	if (VMA_H4EE_SetMMRInfo(gptSharedInfo->hDevInfo, ptMMRInfo, pdwClkEnMmr) != S_OK)
	{
		scResult = -ENODEV;
		goto FAIL;
	}

	VMA_H4EE_InitProfile(gptSharedInfo->hDevInfo);

	scResult = register_chrdev(gsdwMajor, "vma_h4ee", &vma_h4ee_fops);

	if (scResult < 0)
	{
		PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
		goto FAIL;
	}

	if (gsdwMajor == 0)
	{
		gsdwMajor = scResult;
	}

	dwVersionNum = VMA_H4EE_GetVersion(gptSharedInfo->hDevInfo);

	printk("Install VMA_H4EE device driver version %d.%d.%d.%d on VMA_H4EE hardware version %d.%d.%d.%d complete !!\n",
	       (int)(VMA_H4EE_VERSION&0xFF),
	       (int)((VMA_H4EE_VERSION>>8)&0xFF),
	       (int)((VMA_H4EE_VERSION>>16)&0xFF),
	       (int)((VMA_H4EE_VERSION>>24)&0xFF),
	       (int)(dwVersionNum>>24)&0xFF,
	       (int)(dwVersionNum>>16)&0xFF,
	       (int)(dwVersionNum>>8)&0xFF,
	       (int)dwVersionNum&0xFF);

	writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_H4EE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);

	PDEBUG("Exit InitialModule function !!\n");

	return 0;

FAIL:
	writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VMA_H4EE_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
	CleanupModule();
	PDEBUG("Exit InitialModule function !!\n");

	return scResult;
}

/* ============================================================================================== */
module_init(InitialModule);
module_exit(CleanupModule);

/* ============================================================================================== */
