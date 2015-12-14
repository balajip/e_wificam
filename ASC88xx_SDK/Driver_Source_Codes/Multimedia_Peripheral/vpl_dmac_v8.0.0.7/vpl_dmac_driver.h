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
#ifndef __VPL_DMAC_DRIVER_H__
#define __VPL_DMAC_DRIVER_H__

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

#include "vpl_dmac.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VPL_DMAC: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
#define VPL_DMAC_MMR_BUFF_NUM 64
#define VPL_DMAC_MMR_BUFF_MASK 0x0000003F
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24)
#define VPL_DMAC_CLK_EN_NUM 0x0A

/* ============================================================================================== */
extern struct file_operations vpl_dmac_fops;

/* ============================================================================================== */
typedef struct vpl_dmac_obj_info
{
	HANDLE hProfileInfo;

	TVPLDMACInfo *ptMMRInfo;

	DWORD dwMMRInfoPhyAddr;
	DWORD dwWriteIndex;
} TVPLDMACObjInfo;

/* ============================================================================================== */
typedef struct vpl_dmac_shared_info
{
	HANDLE hDevInfo;

	DWORD dwIrq;

	wait_queue_head_t atWaitQueueHead[VPL_DMAC_MMR_BUFF_NUM];

	volatile BOOL abIntr[VPL_DMAC_MMR_BUFF_NUM];
	volatile BOOL abWriteEn[VPL_DMAC_MMR_BUFF_NUM];
} TVPLDMACSharedInfo;

/* ============================================================================================== */
DWORD VPL_DMAC_GetDevInfoSize(void);
SCODE VPL_DMAC_SetMMRInfo(HANDLE, volatile TVPLDMACInfo *, volatile DWORD *);
DWORD VPL_DMAC_GetMMRInfo(HANDLE);
SCODE VPL_DMAC_InitProfileInfo(TVPLDMACObjInfo *);
SCODE VPL_DMAC_GetProfileInfo(HANDLE, TVPLDMACObjInfo *, DWORD);
SCODE VPL_DMAC_InitProfile(HANDLE);
SCODE VPL_DMAC_CloseProfile(HANDLE);
SCODE VPL_DMAC_SetupProfile(TVPLDMACObjInfo *, DWORD, DWORD);
DWORD VPL_DMAC_GetVersion(HANDLE);
DWORD VPL_DMAC_StartHead(HANDLE);
void VPL_DMAC_StartTail(HANDLE, DWORD, TVPLDMACInfo *);
void VPL_DMAC_IntrEnable(HANDLE);
void VPL_DMAC_IntrDisable(HANDLE);
void VPL_DMAC_IntrClear(HANDLE);
void VPL_DMAC_Reset(HANDLE);
void VPL_DMAC_Open(HANDLE);
DWORD VPL_DMAC_ISRHead(HANDLE);
void VPL_DMAC_ISRTail(HANDLE, DWORD);

/* ============================================================================================== */
#endif //__VPL_DMAC_DRIVER_H__

/* ============================================================================================== */
