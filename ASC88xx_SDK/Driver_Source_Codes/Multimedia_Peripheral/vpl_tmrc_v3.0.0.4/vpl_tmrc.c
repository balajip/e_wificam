/*
 * $Header: /rd_2/project/Mozart/Components/TMRC/Device_Driver/TMRC/vpl_tmrc.c 1     13/02/04 3:39p Yiming.liu $
 *
 * Copyright 2012 VN, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: vpl_tmrc.c $
 * 
 * *****************  Version 1  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 3:39p
 * Created in $/rd_2/project/Mozart/Components/TMRC/Device_Driver/TMRC
 * 
 * *****************  Version 1  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 3:38p
 * Created in $/rd_2/project/Mozart/Components/TMRC/TMRC
 * 
 * *****************  Version 12  *****************
 * User: Alan         Date: 09/11/26   Time: 7:50p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Change TVplTMRCInfo to TVPLTMRCInfo - DONE.
 * MODIFICATION: Support up to 8 timers and only TM1 will request_irq
 * while others are used for profiling - DONE.
 * MODIFICATION: Move MAX_TMRC_DEV_NUM to vpl_tmrc.h - DONE.
 *
 * *****************  Version 11  *****************
 * User: Alan         Date: 09/11/21   Time: 6:15p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Use LINUX_VERSION_CODE to decide the related codes for
 * Linux 2.6.24 and above - DONE.
 *
 * *****************  Version 10  *****************
 * User: Alan         Date: 09/11/17   Time: 5:59p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * FEATURE: Add support for __ASM_ARCH_PLATFORM_ANDES_H__ and
 * __ASM_ARCH_PLATFORM_MOZART_H__ (Linux 2.6.26/28) - DONE.
 *
 * *****************  Version 9  *****************
 * User: Aniki        Date: 08/09/11   Time: 3:16p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * BUG: Remove static in ID string declaration to prevent ID string being
 * removed during compilation - FIXED.
 *
 * *****************  Version 8  *****************
 * User: Aniki        Date: 08/09/11   Time: 3:04p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * BUG: Fix the bug of TMRC_Get_Counter() with incorrect read address -
 * FIXED.
 *
 * *****************  Version 7  *****************
 * User: Aniki        Date: 07/08/31   Time: 4:17p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 6  *****************
 * User: Aniki        Date: 06/08/02   Time: 2:57p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * FEATURE: Update TMRC driver as the format B2FE driver - DONE.
 * FEATURE: Support multiple open for Timer 0 - DONE.
 *
 * *****************  Version 5  *****************
 * User: Aniki        Date: 05/11/18   Time: 2:31p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Update the definition of TMRC MMR base address - DONE.
 * MODIFICATION: Update the definition of TMRC IRQ number - DONE.
 *
 * *****************  Version 4  *****************
 * User: Aniki        Date: 05/06/06   Time: 9:30p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 3  *****************
 * User: Aniki        Date: 05/05/23   Time: 2:17p
 * Updated in $/rd_2/Training/SoC_Development/Reggie/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 2  *****************
 * User: Aniki        Date: 05/05/20   Time: 2:51p
 * Updated in $/rd_2/Training/SoC_Development/Reggie/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 1  *****************
 * User: Aniki        Date: 05/05/11   Time: 9:36p
 * Created in $/rd_2/Training/SoC_Development/Reggie/TMRC/Device_Driver/TMRC
 *
 */

/* ============================================================================================= */
#ifndef __KERNEL__
	#define __KERNEL__
#endif
#ifndef MODULE
	#define MODULE
#endif

/* ============================================================================================= */
#include "vpl_tmrc_locals.h"

/* ============================================================================================= */
/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.4 modification, 2009.11.26 */
static TVPLTMRCInfo *pvTMRCBase;
/* ======================================== */
/* ======================================== */

/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.1 modification, 2008.09.11 */
const CHAR VPL_TMRC_ID[] = "$Version: "VPL_TMRC_ID_VERSION"  (VPL_TMRC) $";
/* ======================================== */
/* ======================================== */
static TTMRCDevInfo *ptTMRCDevInfo;	/* This driver instance */
/* Version 3.0.0.0 modification, 2007.08.31 */
static SDWORD g_sdwTMRCMajor = TMRC_MAJOR_DEFAULT;
/* ======================================== */
static DWORD g_dwTMRCTick;

/* ============================================================================================= */
inline void TMRC_Enable(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData|(TMRC_EN<<(dwNum*4)));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_Disable(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_EN<<(dwNum*4))));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_Overflow_Enable(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData|(TMRC_OVERFLOW_EN<<(dwNum*4)));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_OVERFLOW_Disable(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_OVERFLOW_EN<<(dwNum*4))));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_MATCH_Enable(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData|(TMRC_MATCH_EN<<(dwNum*4)));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_MATCH_Disable(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_MATCH_EN<<(dwNum*4))));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_Set_Increment(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_COUNT_DIR<<(dwNum*4))));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_Set_Decrement(DWORD dwNum)
{
	DWORD dwData;

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData|(TMRC_COUNT_DIR<<(dwNum*4)));
/* ======================================== */

	PDEBUG("Timer control = 0x%08x\n", READ_TMRC_MMR(TMRC_CONTROL_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_Set_Counter(DWORD dwNum, DWORD dwData)
{
	WRITE_TMRC_MMR((dwNum*12)+TMRC_COUNTER_OFFSET, dwData);
	PDEBUG("Timer counter = 0x%08x\n", READ_TMRC_MMR((dwNum*12)+TMRC_COUNTER_OFFSET));
}

/* ============================================================================================= */
inline DWORD TMRC_Get_Counter(DWORD dwNum)
{
/* Version 3.0.0.1 modification, 2008.09.11 */
	return READ_TMRC_MMR((dwNum*12)+TMRC_COUNTER_OFFSET);
/* ======================================== */
}

/* ============================================================================================= */
inline void TMRC_Set_Load(DWORD dwNum, DWORD dwData)
{
	WRITE_TMRC_MMR((dwNum*12)+TMRC_LOAD_OFFSET, dwData);
	PDEBUG("Timer load = 0x%08x\n", READ_TMRC_MMR((dwNum*12)+TMRC_LOAD_OFFSET));
}

/* ============================================================================================= */
inline void TMRC_Set_Match(DWORD dwNum, DWORD dwData)
{
	WRITE_TMRC_MMR((dwNum*12)+TMRC_MATCH_OFFSET, dwData);
	PDEBUG("Timer match = 0x%08x\n", READ_TMRC_MMR((dwNum*12)+TMRC_MATCH_OFFSET));
}

/* ============================================================================================= */
static int TMRC_Ioctl(struct inode *inode, struct file *filp, unsigned int dwCmd, unsigned long dwArg)
{
/* Version 2.0.0.0 modification, 2006.07.28 */
	TTMRCDevInfo *ptDevInfo = (TTMRCDevInfo *)filp->private_data;
/* ======================================== */
	DWORD dwNum, dwMajor;
	int scResult = 0;

	PDEBUG("Enter TMRC_Ioctl\n");
	if (ptDevInfo == NULL)
	{
		PDEBUG("driver error\n");
		return -ENODEV;
	}

	dwNum = MINOR(inode->i_rdev);
	dwMajor = MAJOR(inode->i_rdev);

	if (dwNum >= MAX_TMRC_DEV_NUM)
	{
		return -ENODEV;
	}

	/*-------------------------------------------------------------------
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 *-------------------------------------------------------------------*/

	if (_IOC_TYPE(dwCmd) != TMRC_MAGIC)
	{
		PDEBUG("ioctl command error\n");
		return -ENOTTY;
	}
/* Version 2.0.0.0 modification, 2006.07.28 */
	if (_IOC_NR(dwCmd) > VPL_TMRC_IOC_MAX_NUMBER)
/* ======================================== */
	{
		PDEBUG("ioctl command number error\n");
		return -ENOTTY;
	}

	/*-------------------------------------------------------------------
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 *-------------------------------------------------------------------*/

	if (_IOC_DIR(dwCmd) & _IOC_READ)
	{
		scResult = !access_ok(VERIFY_WRITE, (void *)dwArg, _IOC_SIZE(dwCmd));
	}
	else if (_IOC_DIR(dwCmd) & _IOC_WRITE)
	{
		scResult =  !access_ok(VERIFY_READ, (void *)dwArg, _IOC_SIZE(dwCmd));
	}

	if (scResult != 0)
	{
		PDEBUG("ioctl command type\n");
		return -EFAULT;
	}

	switch (dwCmd)
	{
		case TMRC_ENABLE:
			TMRC_Enable(dwNum);
			break;
		case TMRC_DISABLE:
			TMRC_Disable(dwNum);
			break;
		case TMRC_OVERFLOW_ENABLE:
			TMRC_Enable(dwNum);
			break;
		case TMRC_OVERFLOW_DISABLE:
			TMRC_Disable(dwNum);
			break;
		case TMRC_MATCH_ENABLE:
			TMRC_Enable(dwNum);
			break;
		case TMRC_MATCH_DISABLE:
			TMRC_Disable(dwNum);
			break;
		case TMRC_SET_COUNTER:
			TMRC_Set_Counter(dwNum, dwArg);
			break;
		case TMRC_GET_COUNTER:
			return TMRC_Get_Counter(dwNum);
		case TMRC_SET_LOAD:
			TMRC_Set_Load(dwNum, dwArg);
			break;
		case TMRC_SET_MATCH:
			TMRC_Set_Match(dwNum, dwArg);
			break;
		case TMRC_SET_INCR_COUNT:
			TMRC_Set_Increment(dwNum);
			break;
		case TMRC_SET_DECR_COUNT:
			TMRC_Set_Decrement(dwNum);
			break;
		case TMRC_GET_TICK:
			return g_dwTMRCTick;
		default:  /* redundant, as cmd was checked against MAXNR */
			return -ENOTTY;
	}

	return 0;
}

/* ============================================================================================= */
/* Version 3.0.0.0 modification, 2007.08.31 */
static irqreturn_t ISR(int irq, void *dev_id)
{
	PDEBUG("Enter ISR function...\n");

	g_dwTMRCTick++;

	PDEBUG("Exit ISR function !!\n");

	return IRQ_HANDLED;
}
/* ======================================== */

/* ============================================================================================= */
static int TMRC_Open(struct inode *inode, struct file *filp)
{
	TTMRCDevInfo *ptDevInfo; /* device information */
	DWORD dwNum, dwMajor;
	DWORD dwData;
	int scResult;

	dwNum = MINOR(inode->i_rdev);
	dwMajor = MAJOR(inode->i_rdev);

	ptDevInfo = (TTMRCDevInfo *)filp->private_data;
	if (ptDevInfo == NULL)
	{
		if (dwNum >= MAX_TMRC_DEV_NUM)
		{
			return -ENODEV;
		}

		ptDevInfo = ptTMRCDevInfo;
		filp->private_data = ptTMRCDevInfo; /* for other methods */
	}

/* Version 2.0.0.0 modification, 2006.07.28 */
	if (dwNum == 1)
	{
/* ======================================== */
		if (ptDevInfo->requested & (1 << dwNum))
		{
			PDEBUG(KERN_ERR ": driver already opened\n");
			return -EACCES;
		}
/* Version 2.0.0.0 modification, 2006.07.28 */
	}
/* ======================================== */

	/* OK to open driver. */
	ptDevInfo->requested |= (1 << dwNum);

/* Version 3.0.0.4 modification, 2009.11.26 */
	if (dwNum != 1)
/* ======================================== */
	{
		dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);
/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.0 modification, 2007.08.31 */
		if (module_refcount(THIS_MODULE) == 0)
/* ======================================== */
		{
/* ======================================== */
/* Version 3.0.0.4 modification, 2009.11.26 */
			WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_EN<<(dwNum*4)))&(~(TMRC_OVERFLOW_EN<<(dwNum*4))));
/* ======================================== */
/* Version 2.0.0.0 modification, 2006.07.28 */
		}

/* Version 3.0.0.0 modification, 2007.08.31 */
		try_module_get(THIS_MODULE);
/* ======================================== */
/* ======================================== */
	}
	else
	{
/* Version 1.1.0.1 modification, 2005.11.18 */
/* Version 3.0.0.4 modification, 2009.11.26 */
/* ======================================== */
		fLib_SetIntTrig(VPL_TMRC_TM1_IRQ_NUM, EDGE, L_ACTIVE);
/* Version 3.0.0.0 modification, 2007.08.31 */
/* Version 3.0.0.2 modification, 2009.11.17 */
/* Version 3.0.0.3 modification, 2009.11.21 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
		scResult = request_irq(VPL_TMRC_TM1_IRQ_NUM, ISR, SA_INTERRUPT, "vpl_tmrc", NULL);
#else
		scResult = request_irq(VPL_TMRC_TM1_IRQ_NUM, ISR, IRQF_DISABLED, "vpl_tmrc", NULL);
#endif
/* ======================================== */
/* ======================================== */
/* ======================================== */
		if (scResult != 0)
		{
			PDEBUG("Unable to allocate TIMER IRQ=0x%X\n", VPL_TMRC_TM1_IRQ_NUM);
			return scResult;
		}

/* Version 3.0.0.4 modification, 2009.11.26 */
		disable_irq(VPL_TMRC_TM1_IRQ_NUM);
/* ======================================== */
		enable_irq(VPL_TMRC_TM1_IRQ_NUM);
/* ======================================== */

		g_dwTMRCTick = 0;
		dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);
/* Version 3.0.0.4 modification, 2009.11.26 */
		WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, (dwData&(~(TMRC_EN<<4)))|(TMRC_OVERFLOW_EN<<4));
/* ======================================== */
	}

/* Version 2.0.0.0 modification, 2006.07.28 */
/* ======================================== */

	PDEBUG("TMRC module open success\n");

	return 0;          /* success */
}

/* ============================================================================================= */
static int TMRC_Release(struct inode *inode, struct file *filp)
{
	TTMRCDevInfo *ptDevInfo; /* device information */
	DWORD dwNum, dwMajor;
	DWORD dwData;

	dwNum = MINOR(inode->i_rdev);
	dwMajor = MAJOR(inode->i_rdev);

	ptDevInfo = (TTMRCDevInfo *)filp->private_data;

/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.4 modification, 2009.11.26 */
	if (dwNum != 1)
/* ======================================== */
	{
/* Version 3.0.0.0 modification, 2007.08.31 */
		module_put(THIS_MODULE);
/* ======================================== */

/* Version 3.0.0.0 modification, 2007.08.31 */
		if (module_refcount(THIS_MODULE) == 0)
/* ======================================== */
		{
/* ======================================== */
			ptDevInfo->requested &= ~(1 << dwNum);
/* Version 2.0.0.0 modification, 2006.07.28 */
		}
	}
	else
	{
		ptDevInfo->requested &= ~(1 << dwNum);
	}
/* ======================================== */

	dwData = READ_TMRC_MMR(TMRC_CONTROL_OFFSET);

/* Version 3.0.0.4 modification, 2009.11.26 */
	if (dwNum != 1)
/* ======================================== */
	{
/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.0 modification, 2007.08.31 */
		if (module_refcount(THIS_MODULE) == 0)
/* ======================================== */
		{
/* ======================================== */
/* Version 3.0.0.4 modification, 2009.11.26 */
			WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_EN<<(dwNum*4))));
/* ======================================== */
/* Version 2.0.0.0 modification, 2006.07.28 */
		}
/* ======================================== */
	}
	else
	{
/* Version 1.1.0.1 modification, 2005.11.18 */
		free_irq(VPL_TMRC_TM1_IRQ_NUM, NULL);
/* ======================================== */
/* Version 3.0.0.4 modification, 2009.11.26 */
		WRITE_TMRC_MMR(TMRC_CONTROL_OFFSET, dwData&(~(TMRC_EN<<4)));
/* ======================================== */
	}

/* Version 2.0.0.0 modification, 2006.07.28 */
/* ======================================== */

	PDEBUG("TMRC module close success\n");

	return 0;
}

/* ======================================================================== */
/* Version 1.1.0.0 modification, 2005.06.03 */
static int TMRC_MMap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size;
	size = vma->vm_end - vma->vm_start;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_flags |= VM_RESERVED;

/* Version 3.0.0.0 modification, 2007.08.31 */
	if (remap_pfn_range(vma, vma->vm_start, (VPL_TMRC_MMR_BASE >> PAGE_SHIFT), size, vma->vm_page_prot))
/* ======================================== */
    {
		return -EAGAIN;
	}

	PDEBUG("vm_start: 0x%08lX, vm_end: 0x%08lX\n", vma->vm_start, vma->vm_end);
	return 0;
}
/* ======================================== */

/* ============================================================================================= */
static struct file_operations TMRC_fops =
{
	ioctl:		TMRC_Ioctl,
/* Version 1.1.0.0 modification, 2005.06.03 */
	mmap:		TMRC_MMap,
/* ======================================== */
	open:		TMRC_Open,
	release:	TMRC_Release,
};

/* ============================================================================================= */
void TMRC_Cleanup_Module(void)
{
/* Version 3.0.0.2 modification, 2009.11.17 */
/* Version 3.0.0.3 modification, 2009.11.21 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
/* ======================================== */
	int scResult;
#endif
/* ======================================== */
	if (ptTMRCDevInfo != NULL)
	{
		kfree(ptTMRCDevInfo);
	}

/* Version 3.0.0.0 modification, 2007.08.31 */
	if (g_sdwTMRCMajor != 0)
/* ======================================== */
	{
/* Version 3.0.0.2 modification, 2009.11.17 */
/* Version 3.0.0.3 modification, 2009.11.21 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
		scResult = unregister_chrdev((int)g_sdwTMRCMajor, "vpl_tmrc");
		if (scResult < 0)
		{
			PDEBUG("ERROR: can't release major %d\n", (int)g_sdwTMRCMajor);
			return;
		}
#else
		unregister_chrdev((int)g_sdwTMRCMajor, "vpl_tmrc");
#endif
/* ======================================== */
/* ======================================== */
	}

	if (pvTMRCBase != NULL)
	{
		iounmap(pvTMRCBase);
	}

	PDEBUG("TMRC module cleanup success\n");
}

/* ============================================================================================= */
int TMRC_Init_Module(void)
{
	int scResult;
/* Version 2.0.0.0 modification, 2006.07.28 */
/* ======================================== */
	/* Use ioremap to get a handle on register */
/* Version 3.0.0.0 modification, 2007.08.31 */
/* Version 3.0.0.4 modification, 2009.11.26 */
	if ((pvTMRCBase=(TVPLTMRCInfo *)ioremap((unsigned long)VPL_TMRC_MMR_BASE, sizeof(TVPLTMRCInfo))) == NULL)
/* ======================================== */
/* ======================================== */
	{
		PDEBUG("IOREMAP fail.\n");
		scResult = -ENOMEM;
		goto fail_init;
	}

	/*****************************************************
 	* Register the character driver.
	*****************************************************/
/* Version 3.0.0.0 modification, 2007.08.31 */
	scResult = register_chrdev(g_sdwTMRCMajor, "vpl_tmrc", &TMRC_fops);
/* ======================================== */

	if (scResult < 0)
	{
/* Version 3.0.0.0 modification, 2007.08.31 */
		PDEBUG("ERROR: can't get major %d\n", (int)g_sdwTMRCMajor);
/* ======================================== */
		goto fail_init;
	}

/* Version 3.0.0.0 modification, 2007.08.31 */
	if (g_sdwTMRCMajor == 0)
	{
		g_sdwTMRCMajor = scResult; /* dynamic */
	}
	PDEBUG("TMRC Major Number = %d\n", (int)g_sdwTMRCMajor);
	PDEBUG("make node with 'mknod /dev/vpl_tmrc0 c %d 0'\n", (int)g_sdwTMRCMajor);
	PDEBUG("make node with 'mknod /dev/vpl_tmrc1 c %d 1'\n", (int)g_sdwTMRCMajor);
/* Version 3.0.0.4 modification, 2009.11.26 */
	PDEBUG("make node with 'mknod /dev/vpl_tmrc2 c %d 1'\n", (int)g_sdwTMRCMajor);
#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
	PDEBUG("make node with 'mknod /dev/vpl_tmrc3 c %d 1'\n", (int)g_sdwTMRCMajor);
#endif //__ASM_ARCH_PLATFORM_MOZART_H__
/* ======================================== */
/* ======================================== */

	/*******************************************************
 	* Allocate space for each devices.
 	*******************************************************/
	ptTMRCDevInfo = kmalloc(sizeof(TTMRCDevInfo), GFP_KERNEL);
	if (ptTMRCDevInfo == NULL)
	{
		scResult = -ENOMEM;
		goto fail_init;
	}
	memset(ptTMRCDevInfo, 0, sizeof(TTMRCDevInfo));
/* Version 2.0.0.0 modification, 2006.07.28 */
	printk("Install VPL_TMRC device driver version %d.%d.%d.%d on VPL_TMRC hardware version %d.%d.%d.%d complete !!\n",
	       (int)(VPL_TMRC_VERSION&0xFF),
	       (int)((VPL_TMRC_VERSION>>8)&0xFF),
	       (int)((VPL_TMRC_VERSION>>16)&0xFF),
	       (int)((VPL_TMRC_VERSION>>24)&0xFF),
	       (readl(&pvTMRCBase->dwVersion)>>24)&0xFF,
	       (readl(&pvTMRCBase->dwVersion)>>16)&0xFF,
	       (readl(&pvTMRCBase->dwVersion)>>8)&0xFF,
	       readl(&pvTMRCBase->dwVersion)&0xFF);
/* ======================================== */
	PDEBUG("TMRC module init success\n");
	return 0; /* succeed */

fail_init:
	TMRC_Cleanup_Module();

	return scResult;
}

/* ============================================================================================= */
/* Version 3.0.0.0 modification, 2007.08.31 */
module_param(g_sdwTMRCMajor, int, 0644);
/* ======================================== */
/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.0 modification, 2007.08.31 */
MODULE_PARM_DESC(g_sdwTMRCMajor, "Major number for VPL_TMRC module");
/* ======================================== */
/* ======================================== */
MODULE_AUTHOR("VN Inc.");
/* Version 2.0.0.0 modification, 2006.07.28 */
MODULE_DESCRIPTION("Device driver for VPL_TMRC module");
/* ======================================== */
MODULE_LICENSE("GPL");
/* Version 3.0.0.0 modification, 2007.08.31 */
/* ======================================== */

/* ============================================================================================= */
module_init(TMRC_Init_Module);
module_exit(TMRC_Cleanup_Module);

/* ============================================================================================= */
