/*
 * $Header: /rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE/vma_meae_driver.h 4     13/05/28 11:11a Jaja $
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
 * $History: vma_meae_driver.h $
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
#ifndef __VMA_MEAE_DRIVER_H__
#define __VMA_MEAE_DRIVER_H__

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

#include "vma_meae.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VMA_MEAE: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VMA_MEAE_MMR_BUFF_NUM 16
#define VMA_MEAE_MMR_BUFF_MASK 0x0000000F
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)
#define VMA_MEAE_CLK_EN_NUM 0x19

/* ============================================================================================== */
extern struct file_operations vma_meae_fops;

/* ============================================================================================== */
typedef struct vma_meae_obj_info
{
	HANDLE hProfileInfo;

	TVMAMEAEInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVMAMEAEObjInfo;

/* ============================================================================================== */
typedef struct vma_meae_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VMA_MEAE_MMR_BUFF_NUM];

	volatile BOOL abIntr[VMA_MEAE_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VMA_MEAE_MMR_BUFF_NUM];
} TVMAMEAESharedInfo;

/* ============================================================================================== */
DWORD VMA_MEAE_GetDevInfoSize(void);
SCODE VMA_MEAE_SetMMRInfo(HANDLE, volatile TVMAMEAEInfo *, volatile DWORD *);
DWORD VMA_MEAE_GetMMRInfo(HANDLE);
SCODE VMA_MEAE_InitProfileInfo(TVMAMEAEObjInfo *);
SCODE VMA_MEAE_GetProfileInfo(HANDLE, TVMAMEAEObjInfo *, DWORD);
SCODE VMA_MEAE_InitProfile(HANDLE);
SCODE VMA_MEAE_CloseProfile(HANDLE);
SCODE VMA_MEAE_SetupProfile(TVMAMEAEObjInfo *, DWORD, DWORD);
DWORD VMA_MEAE_GetVersion(HANDLE);
DWORD VMA_MEAE_StartHead(HANDLE);
void VMA_MEAE_StartTail(HANDLE, DWORD, TVMAMEAEInfo *);
void VMA_MEAE_IntrEnable(HANDLE);
void VMA_MEAE_IntrDisable(HANDLE);
void VMA_MEAE_IntrClear(HANDLE);
void VMA_MEAE_Reset(HANDLE);
void VMA_MEAE_Open(HANDLE);
DWORD VMA_MEAE_ISRHead(HANDLE);
void VMA_MEAE_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VMA_MEAE_DRIVER_H__

/* ============================================================================================== */
