/*
 * $Header:$
 *
 * vma_dce
 * Driver for VMA DCE
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
#ifndef __VMA_DCE_DRIVER_H__
#define __VMA_DCE_DRIVER_H__

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

#include "vma_dce.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VMA_DCE: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VMA_DCE_MMR_BUFF_NUM 16
#define VMA_DCE_MMR_BUFF_MASK 0x0000000F
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)
#define VMA_DCE_CLK_EN_NUM 0x07

/* ============================================================================================== */
extern struct file_operations vma_dce_fops;

/* ============================================================================================== */
typedef struct vma_dce_obj_info
{
	HANDLE hProfileInfo;

	TVMADCEInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVMADCEObjInfo;

/* ============================================================================================== */
typedef struct vma_dce_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VMA_DCE_MMR_BUFF_NUM];

	volatile BOOL abIntr[VMA_DCE_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VMA_DCE_MMR_BUFF_NUM];
} TVMADCESharedInfo;

/* ============================================================================================== */
DWORD VMA_DCE_GetDevInfoSize(void);
SCODE VMA_DCE_SetMMRInfo(HANDLE, volatile TVMADCEInfo *, volatile DWORD *);
DWORD VMA_DCE_GetMMRInfo(HANDLE);
SCODE VMA_DCE_InitProfileInfo(TVMADCEObjInfo *);
SCODE VMA_DCE_GetProfileInfo(HANDLE, TVMADCEObjInfo *, DWORD);
SCODE VMA_DCE_InitProfile(HANDLE);
SCODE VMA_DCE_CloseProfile(HANDLE);
SCODE VMA_DCE_SetupProfile(TVMADCEObjInfo *, DWORD, DWORD);
DWORD VMA_DCE_GetVersion(HANDLE);
DWORD VMA_DCE_StartHead(HANDLE);
void VMA_DCE_StartTail(HANDLE, DWORD, TVMADCEInfo *);
void VMA_DCE_IntrEnable(HANDLE);
void VMA_DCE_IntrDisable(HANDLE);
void VMA_DCE_IntrClear(HANDLE);
void VMA_DCE_Reset(HANDLE);
void VMA_DCE_Open(HANDLE);
DWORD VMA_DCE_ISRHead(HANDLE);
void VMA_DCE_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VMA_DCE_DRIVER_H__

/* ============================================================================================== */
