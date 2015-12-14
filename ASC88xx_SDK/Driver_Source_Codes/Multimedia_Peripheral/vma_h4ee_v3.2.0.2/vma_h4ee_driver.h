/*
 * $Header: /rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE/vma_h4ee_driver.h 2     12/06/20 5:54p Jaja $
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
 * $History: vma_h4ee_driver.h $
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
#ifndef __VMA_H4EE_DRIVER_H__
#define __VMA_H4EE_DRIVER_H__

/* ============================================================================================== */
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/cache.h>
#include <asm/arch/platform.h>
#include <asm/arch/irq.h>

#include "vma_h4ee.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VMA_H4EE: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VMA_H4EE_MMR_BUFF_NUM 16
#define VMA_H4EE_MMR_BUFF_MASK 0x0000000F
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)

#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define VMA_H4EE_CLK_EN_NUM	0x0D
#endif //!__ASM_ARCH_PLATFORM_MOZART_H__

#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
#define VMA_H4EE_CLK_EN_NUM	0x19
#endif //!__ASM_ARCH_PLATFORM_BEETHOVEN_H__

/* ============================================================================================== */
extern struct file_operations vma_h4ee_fops;

/* ============================================================================================== */
typedef struct vma_h4ee_obj_info
{
	HANDLE hProfileInfo;

	TVMAH4EEInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVMAH4EEObjInfo;

/* ============================================================================================== */
typedef struct vma_h4ee_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VMA_H4EE_MMR_BUFF_NUM];

	volatile BOOL abIntr[VMA_H4EE_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VMA_H4EE_MMR_BUFF_NUM];
} TVMAH4EESharedInfo;

/* ============================================================================================== */
DWORD VMA_H4EE_GetDevInfoSize(void);
SCODE VMA_H4EE_SetMMRInfo(HANDLE, volatile TVMAH4EEInfo *, volatile DWORD *);
DWORD VMA_H4EE_GetMMRInfo(HANDLE);
SCODE VMA_H4EE_InitProfileInfo(TVMAH4EEObjInfo *);
SCODE VMA_H4EE_GetProfileInfo(HANDLE, TVMAH4EEObjInfo *, DWORD);
SCODE VMA_H4EE_InitProfile(HANDLE);
SCODE VMA_H4EE_CloseProfile(HANDLE);
SCODE VMA_H4EE_SetupProfile(TVMAH4EEObjInfo *, DWORD, DWORD);
DWORD VMA_H4EE_GetVersion(HANDLE);
DWORD VMA_H4EE_StartHead(HANDLE);
void VMA_H4EE_SetChipVersionInfo(HANDLE);
void VMA_H4EE_StartTail(HANDLE, DWORD, TVMAH4EEInfo *);
void VMA_H4EE_IntrEnable(HANDLE);
void VMA_H4EE_IntrDisable(HANDLE);
void VMA_H4EE_IntrClear(HANDLE);
void VMA_H4EE_Reset(HANDLE);
void VMA_H4EE_Open(HANDLE);
DWORD VMA_H4EE_ISRHead(HANDLE);
void VMA_H4EE_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VMA_H4EE_DRIVER_H__

/* ============================================================================================== */
