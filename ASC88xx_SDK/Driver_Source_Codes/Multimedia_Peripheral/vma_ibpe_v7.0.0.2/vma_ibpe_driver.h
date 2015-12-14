/*
 * $Header: /rd_2/project/Mozart/Components/IBPE_3.0.0.0/Device_Driver/IBPE/vma_ibpe_driver.h 1     12/09/24 6:53p Linus.huang $
 *
 * vma_ibpe
 * Driver for VMA IBPE
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
 * $History: vma_ibpe_driver.h $
 * 
 * *****************  Version 1  *****************
 * User: Linus.huang  Date: 12/09/24   Time: 6:53p
 * Created in $/rd_2/project/Mozart/Components/IBPE_3.0.0.0/Device_Driver/IBPE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_IBPE_DRIVER_H__
#define __VMA_IBPE_DRIVER_H__

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

#include "vma_ibpe.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VMA_IBPE: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VMA_IBPE_MMR_BUFF_NUM 1024
#define VMA_IBPE_MMR_BUFF_MASK 0x000003FF
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)
#define VMA_IBPE_CLK_EN_NUM 0x14

/* ============================================================================================== */
extern struct file_operations vma_ibpe_fops;

/* ============================================================================================== */
typedef struct vma_ibpe_obj_info
{
	HANDLE hProfileInfo;

	TVMAIBPEInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVMAIBPEObjInfo;

/* ============================================================================================== */
typedef struct vma_ibpe_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VMA_IBPE_MMR_BUFF_NUM];

	volatile BOOL abIntr[VMA_IBPE_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VMA_IBPE_MMR_BUFF_NUM];
} TVMAIBPESharedInfo;

/* ============================================================================================== */
DWORD VMA_IBPE_GetDevInfoSize(void);
SCODE VMA_IBPE_SetMMRInfo(HANDLE, volatile TVMAIBPEInfo *, volatile DWORD *);
DWORD VMA_IBPE_GetMMRInfo(HANDLE);
SCODE VMA_IBPE_InitProfileInfo(TVMAIBPEObjInfo *);
SCODE VMA_IBPE_GetProfileInfo(HANDLE, TVMAIBPEObjInfo *, DWORD);
SCODE VMA_IBPE_InitProfile(HANDLE);
SCODE VMA_IBPE_CloseProfile(HANDLE);
SCODE VMA_IBPE_SetupProfile(TVMAIBPEObjInfo *, DWORD, DWORD);
DWORD VMA_IBPE_GetVersion(HANDLE);
DWORD VMA_IBPE_StartHead(HANDLE);
void VMA_IBPE_StartTail(HANDLE, DWORD, TVMAIBPEInfo *);
void VMA_IBPE_IntrEnable(HANDLE);
void VMA_IBPE_IntrDisable(HANDLE);
void VMA_IBPE_IntrClear(HANDLE);
void VMA_IBPE_Reset(HANDLE);
void VMA_IBPE_Open(HANDLE);
DWORD VMA_IBPE_ISRHead(HANDLE);
void VMA_IBPE_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VMA_IBPE_DRIVER_H__

/* ============================================================================================== */
