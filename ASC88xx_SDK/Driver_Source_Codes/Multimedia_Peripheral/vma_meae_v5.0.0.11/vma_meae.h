/*
 * $Header: /rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE/vma_meae.h 19    13/05/28 11:11a Jaja $
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
 * $History: vma_meae.h $
 * 
 * *****************  Version 19  *****************
 * User: Jaja         Date: 13/05/28   Time: 11:11a
 * Updated in $/rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE
 *
 * *****************  Version 18  *****************
 * User: Angel        Date: 12/06/21   Time: 6:05p
 * Updated in $/rd_2/project/SoC/Components/MEAE/Device_Driver/MEAE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_MEAE_H__
#define __VMA_MEAE_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VMA_MEAE_VERSION MAKEFOURCC(5, 0, 0, 11)
#define VMA_MEAE_ID_VERSION "5.0.0.11"

/* ============================================================================================== */
#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
#define VMA_MEAE_BUS_NUM	1
#define VMA_MEAE_MASTER_NUM	0x0F
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define VMA_MEAE_BUS_NUM	1
#define VMA_MEAE_MASTER_NUM	0x0D
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
#define VMA_MEAE_BUS_NUM	2
#define VMA_MEAE_MASTER_NUM	0x0D
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__

/* ============================================================================================== */
typedef struct vma_meae_info
{
	DWORD dwVersion; // MMR 0
	DWORD dwCycles; // MMR 1
	DWORD dwCtrl; // MMR 2
	DWORD dwStat; // MMR 3
	DWORD dwBrate; // MMR 4
	DWORD dwIrAddr; // MMR 5
	DWORD dwXrAddr; // MMR 6
	DWORD dwSfAddr; // MMR 7
	DWORD dwCbAddr; // MMR 8
} TVMAMEAEInfo;

/* ============================================================================================== */
#define VMA_MEAE_IOC_MAGIC 239

#define VMA_MEAE_IOC_START						_IO		(VMA_MEAE_IOC_MAGIC, 0)
#define VMA_MEAE_IOC_WAIT_COMPLETE				_IO		(VMA_MEAE_IOC_MAGIC, 1)
#define VMA_MEAE_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VMA_MEAE_IOC_MAGIC, 2, DWORD)
#define VMA_MEAE_IOC_GET_VERSION_NUMBER			_IOR	(VMA_MEAE_IOC_MAGIC, 3, DWORD)
#define VMA_MEAE_IOC_GET_BANDWIDTH				_IOR	(VMA_MEAE_IOC_MAGIC, 4, DWORD)
#define VMA_MEAE_IOC_GET_RG_INTERVAL				_IOR	(VMA_MEAE_IOC_MAGIC, 5, DWORD)
#define VMA_MEAE_IOC_GET_REQ_TIMES				_IOR	(VMA_MEAE_IOC_MAGIC, 6, DWORD)
#define VMA_MEAE_IOC_CLEAR_PROFILE				_IO		(VMA_MEAE_IOC_MAGIC, 7)
#define VMA_MEAE_IOC_RESET						_IO		(VMA_MEAE_IOC_MAGIC, 8)

#define VMA_MEAE_IOC_MAX_NUMBER	8

/* ============================================================================================== */
#endif //__VMA_MEAE_H__

/* ============================================================================================== */
