/*
 * $Header: /rd_2/project/Mozart/Components/IRE/Device_Driver/IRE/vma_ire_driver.h 3     12/07/09 5:32p Kensuke.chen $
 *
 * vma_ire
 * Driver for VMA IRE
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
 * $History: vma_ire_driver.h $
 * 
 * *****************  Version 3  *****************
 * User: Kensuke.chen Date: 12/07/09   Time: 5:32p
 * Updated in $/rd_2/project/Mozart/Components/IRE/Device_Driver/IRE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_IRE_DRIVER_H__
#define __VMA_IRE_DRIVER_H__

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

#include "vma_ire.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VMA_IRE: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VMA_IRE_MMR_BUFF_NUM 1024
#define VMA_IRE_MMR_BUFF_MASK 0x000003FF
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)
#define VMA_IRE_CLK_EN_NUM 0x17

/* ============================================================================================== */
extern struct file_operations vma_ire_fops;

/* ============================================================================================== */
typedef struct vma_ire_obj_info
{
	HANDLE hProfileInfo;

	TVMAIREInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVMAIREObjInfo;

/* ============================================================================================== */
typedef struct vma_ire_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VMA_IRE_MMR_BUFF_NUM];

	volatile BOOL abIntr[VMA_IRE_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VMA_IRE_MMR_BUFF_NUM];
} TVMAIRESharedInfo;

/* ============================================================================================== */
DWORD VMA_IRE_GetDevInfoSize(void);
SCODE VMA_IRE_SetMMRInfo(HANDLE, volatile TVMAIREInfo *, volatile DWORD *);
DWORD VMA_IRE_GetMMRInfo(HANDLE);
SCODE VMA_IRE_InitProfileInfo(TVMAIREObjInfo *);
SCODE VMA_IRE_GetProfileInfo(HANDLE, TVMAIREObjInfo *, DWORD);
SCODE VMA_IRE_InitProfile(HANDLE);
SCODE VMA_IRE_CloseProfile(HANDLE);
SCODE VMA_IRE_SetupProfile(TVMAIREObjInfo *, DWORD, DWORD);
DWORD VMA_IRE_GetVersion(HANDLE);
DWORD VMA_IRE_StartHead(HANDLE);
void VMA_IRE_StartTail(HANDLE, DWORD, TVMAIREInfo *);
void VMA_IRE_IntrEnable(HANDLE);
void VMA_IRE_IntrDisable(HANDLE);
void VMA_IRE_IntrClear(HANDLE);
void VMA_IRE_Reset(HANDLE);
void VMA_IRE_Open(HANDLE);
DWORD VMA_IRE_ISRHead(HANDLE);
void VMA_IRE_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VMA_IRE_DRIVER_H__

/* ============================================================================================== */
