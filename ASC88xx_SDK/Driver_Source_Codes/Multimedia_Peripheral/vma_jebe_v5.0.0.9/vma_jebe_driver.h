/*
 * $Header: /rd_2/project/Mozart/Components/JEBE/Device_Driver/JEBE/vma_jebe_driver.h 1     12/06/25 4:34p Jaja $
 *
 * vma_jebe
 * Driver for VMA JEBE
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
 * $History: vma_jebe_driver.h $
 * 
 * *****************  Version 1  *****************
 * User: Jaja         Date: 12/06/25   Time: 4:34p
 * Created in $/rd_2/project/Mozart/Components/JEBE/Device_Driver/JEBE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_JEBE_DRIVER_H__
#define __VMA_JEBE_DRIVER_H__

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

#include "vma_jebe.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VMA_JEBE: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VMA_JEBE_MMR_BUFF_NUM 8
#define VMA_JEBE_MMR_BUFF_MASK 0x00000007
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)
#define VMA_JEBE_CLK_EN_NUM 0x18

/* ============================================================================================== */
extern struct file_operations vma_jebe_fops;

/* ============================================================================================== */
typedef struct vma_jebe_obj_info
{
	TVMAJEBEInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVMAJEBEObjInfo;

/* ============================================================================================== */
typedef struct vma_jebe_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VMA_JEBE_MMR_BUFF_NUM];

	volatile BOOL abIntr[VMA_JEBE_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VMA_JEBE_MMR_BUFF_NUM];
} TVMAJEBESharedInfo;

/* ============================================================================================== */
DWORD VMA_JEBE_GetDevInfoSize(void);
SCODE VMA_JEBE_SetMMRInfo(HANDLE, volatile TVMAJEBEInfo *, volatile DWORD *);
DWORD VMA_JEBE_GetMMRInfo(HANDLE);
SCODE VMA_JEBE_InitProfile(HANDLE);
SCODE VMA_JEBE_CloseProfile(HANDLE);
SCODE VMA_JEBE_SetupProfile(HANDLE, DWORD, DWORD);
DWORD VMA_JEBE_GetVersion(HANDLE);
DWORD VMA_JEBE_StartHead(HANDLE);
void VMA_JEBE_StartTail(HANDLE, DWORD, TVMAJEBEInfo *);
void VMA_JEBE_IntrEnable(HANDLE);
void VMA_JEBE_IntrDisable(HANDLE);
void VMA_JEBE_IntrClear(HANDLE);
void VMA_JEBE_Reset(HANDLE);
void VMA_JEBE_Open(HANDLE);
DWORD VMA_JEBE_ISRHead(HANDLE);
void VMA_JEBE_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VMA_JEBE_DRIVER_H__

/* ============================================================================================== */
