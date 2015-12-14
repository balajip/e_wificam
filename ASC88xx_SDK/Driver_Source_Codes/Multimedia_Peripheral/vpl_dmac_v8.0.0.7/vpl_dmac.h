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
#ifndef __VPL_DMAC_H__
#define __VPL_DMAC_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VPL_DMAC_VERSION MAKEFOURCC(8, 0, 0, 7)
#define VPL_DMAC_ID_VERSION "8.0.0.7"

/* ============================================================================================== */
#define VPL_DMAC_MASTER_0_BUS_NUM	0
#define VPL_DMAC_MASTER_1_BUS_NUM	1
#define VPL_DMAC_MASTER_2_BUS_NUM	2

#define VPL_DMAC_MASTER_0_MASTER_NUM	0x0E
#define VPL_DMAC_MASTER_1_MASTER_NUM	0x0E
#define VPL_DMAC_MASTER_2_MASTER_NUM	0x0E

/* ============================================================================================== */
typedef struct vpl_dmac_info
{
	DWORD dwVersion; // MMR 0
	DWORD dwCycles; // MMR 1
	DWORD dwCtrl; // MMR 2
	DWORD dwStat; // MMR 3
	DWORD dwSrcAddr; // MMR 4
	DWORD dwDstAddr; // MMR 5
	DWORD dwTransSize; // MMR 6
	DWORD dwSrcStride; // MMR 7
	DWORD dwDstWidth; // MMR 8
	DWORD dwDstHeight; // MMR 9
	DWORD dwDstStride; // MMR 10
	DWORD dwFillingConstant; // MMR 11
	DWORD dwMaskAddr; // MMR 12
	DWORD dwMaxBurstLength; // MMR 13
} TVPLDMACInfo;

/* ============================================================================================== */
#define VPL_DMAC_IOC_MAGIC 222

#define VPL_DMAC_IOC_START						_IO		(VPL_DMAC_IOC_MAGIC, 0)
#define VPL_DMAC_IOC_WAIT_COMPLETE				_IO		(VPL_DMAC_IOC_MAGIC, 1)
#define VPL_DMAC_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VPL_DMAC_IOC_MAGIC, 2, DWORD)
#define VPL_DMAC_IOC_GET_VERSION_NUMBER			_IOR	(VPL_DMAC_IOC_MAGIC, 3, DWORD)
#define VPL_DMAC_IOC_MASTER_0_GET_BANDWIDTH		_IOR	(VPL_DMAC_IOC_MAGIC, 4, DWORD)
#define VPL_DMAC_IOC_MASTER_0_GET_RG_INTERVAL	_IOR	(VPL_DMAC_IOC_MAGIC, 5, DWORD)
#define VPL_DMAC_IOC_MASTER_0_GET_REQ_TIMES		_IOR	(VPL_DMAC_IOC_MAGIC, 6, DWORD)
#define VPL_DMAC_IOC_MASTER_0_CLEAR_PROFILE		_IO		(VPL_DMAC_IOC_MAGIC, 7)
#define VPL_DMAC_IOC_MASTER_1_GET_BANDWIDTH		_IOR	(VPL_DMAC_IOC_MAGIC, 8, DWORD)
#define VPL_DMAC_IOC_MASTER_1_GET_RG_INTERVAL	_IOR	(VPL_DMAC_IOC_MAGIC, 9, DWORD)
#define VPL_DMAC_IOC_MASTER_1_GET_REQ_TIMES		_IOR	(VPL_DMAC_IOC_MAGIC, 10, DWORD)
#define VPL_DMAC_IOC_MASTER_1_CLEAR_PROFILE		_IO		(VPL_DMAC_IOC_MAGIC, 11)
#define VPL_DMAC_IOC_MASTER_2_GET_BANDWIDTH		_IOR	(VPL_DMAC_IOC_MAGIC, 12, DWORD)
#define VPL_DMAC_IOC_MASTER_2_GET_RG_INTERVAL	_IOR	(VPL_DMAC_IOC_MAGIC, 13, DWORD)
#define VPL_DMAC_IOC_MASTER_2_GET_REQ_TIMES		_IOR	(VPL_DMAC_IOC_MAGIC, 14, DWORD)
#define VPL_DMAC_IOC_MASTER_2_CLEAR_PROFILE		_IO		(VPL_DMAC_IOC_MAGIC, 15)

#define VPL_DMAC_IOC_MAX_NUMBER	15

/* ============================================================================================== */
#endif //__VPL_DMAC_H__

/* ============================================================================================== */
