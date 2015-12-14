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
#ifndef __VMA_DCE_H__
#define __VMA_DCE_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VMA_DCE_VERSION MAKEFOURCC(4, 0, 0, 3)
#define VMA_DCE_ID_VERSION "4.0.0.3"

/* ============================================================================================== */
#define VMA_DCE_BUS_NUM	1

#define VMA_DCE_MASTER_NUM	0x0B

/* ============================================================================================== */
typedef struct vma_dce_info
{
	DWORD dwVersion; // MMR 0
	DWORD dwCycles; // MMR 1
	DWORD dwCtrl; // MMR 2
	DWORD dwStat; // MMR 3
	DWORD dwTextSize; // MMR 4
	DWORD dwTextAddr; // MMR 5
	DWORD dwIVAddr; // MMR 6
	DWORD dwKeyAddr; // MMR 7
	DWORD dwOutAddr; // MMR 8
} TVMADCEInfo;

/* ============================================================================================== */
#define VMA_DCE_IOC_MAGIC 235

#define VMA_DCE_IOC_START						_IO		(VMA_DCE_IOC_MAGIC, 0)
#define VMA_DCE_IOC_WAIT_COMPLETE				_IO		(VMA_DCE_IOC_MAGIC, 1)
#define VMA_DCE_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VMA_DCE_IOC_MAGIC, 2, DWORD)
#define VMA_DCE_IOC_GET_VERSION_NUMBER			_IOR	(VMA_DCE_IOC_MAGIC, 3, DWORD)
#define VMA_DCE_IOC_GET_BANDWIDTH				_IOR	(VMA_DCE_IOC_MAGIC, 4, DWORD)
#define VMA_DCE_IOC_GET_RG_INTERVAL				_IOR	(VMA_DCE_IOC_MAGIC, 5, DWORD)
#define VMA_DCE_IOC_GET_REQ_TIMES				_IOR	(VMA_DCE_IOC_MAGIC, 6, DWORD)
#define VMA_DCE_IOC_CLEAR_PROFILE				_IO		(VMA_DCE_IOC_MAGIC, 7)

#define VMA_DCE_IOC_MAX_NUMBER	7

/* ============================================================================================== */
#endif //__VMA_DCE_H__

/* ============================================================================================== */
