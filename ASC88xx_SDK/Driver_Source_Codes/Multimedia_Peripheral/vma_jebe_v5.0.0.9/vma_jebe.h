/*
 * $Header: /rd_2/project/Mozart/Components/JEBE/Device_Driver/JEBE/vma_jebe.h 22    12/06/25 4:34p Jaja $
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
 * $History: vma_jebe.h $
 * 
 * *****************  Version 22  *****************
 * User: Jaja         Date: 12/06/25   Time: 4:34p
 * Updated in $/rd_2/project/Mozart/Components/JEBE/Device_Driver/JEBE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_JEBE_H__
#define __VMA_JEBE_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VMA_JEBE_VERSION MAKEFOURCC(5, 0, 0, 9)
#define VMA_JEBE_ID_VERSION "5.0.0.9"

/* ============================================================================================== */
#define VMA_JEBE_MASTER_0_BUS_NUM	0
#define VMA_JEBE_MASTER_1_BUS_NUM	2

#define VMA_JEBE_MASTER_0_MASTER_NUM	0x07
#define VMA_JEBE_MASTER_1_MASTER_NUM	0x07

/* ============================================================================================== */
typedef struct vma_jebe_info
{
	DWORD dwVersion; // MMR 0
	DWORD dwCycles; // MMR 1
	DWORD dwCtrl; // MMR 2
	DWORD dwStat; // MMR 3
	DWORD dwCodingCtrl; // MMR 4
	DWORD dwYFrameAddr; // MMR 5
	DWORD dwCbFrameAddr; // MMR 6
	DWORD dwCrFrameAddr; // MMR 7
	DWORD dwBSLowBoundAddr; // MMR 8
	DWORD dwBSUpBoundAddr; // MMR 9
	DWORD dwLumQuantTblAddr; // MMR 10
	DWORD dwChrQuantTblAddr; // MMR 11
	DWORD dwLumHufIndxTblAddr; // MMR 12
	DWORD dwChrHufIndxTblAddr; // MMR 13
	DWORD dwLumHufCntTblAddr; // MMR 14
	DWORD dwChrHufCntTblAddr; // MMR 15
	DWORD dwTotalBlkNum; // MMR 16
	DWORD dwBuffSize; // MMR 17
	DWORD dwLumSizeInfo; // MMR 18
	DWORD dwPrevSliceInfo0; // MMR 19
	DWORD dwPrevSliceInfo1; // MMR 20
	DWORD dwUnAlignBytesMSBs; // MMR 21
	DWORD dwUnAlignBytesLSBs; // MMR 22
} TVMAJEBEInfo;

/* ============================================================================================== */
#define VMA_JEBE_IOC_MAGIC 215

#define VMA_JEBE_IOC_START						_IO		(VMA_JEBE_IOC_MAGIC, 0)
#define VMA_JEBE_IOC_WAIT_COMPLETE				_IO		(VMA_JEBE_IOC_MAGIC, 1)
#define VMA_JEBE_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VMA_JEBE_IOC_MAGIC, 2, DWORD)
#define VMA_JEBE_IOC_GET_VERSION_NUMBER			_IOR	(VMA_JEBE_IOC_MAGIC, 3, DWORD)
#define VMA_JEBE_IOC_MASTER_0_GET_BANDWIDTH		_IOR	(VMA_JEBE_IOC_MAGIC, 4, DWORD)
#define VMA_JEBE_IOC_MASTER_0_GET_RG_INTERVAL	_IOR	(VMA_JEBE_IOC_MAGIC, 5, DWORD)
#define VMA_JEBE_IOC_MASTER_0_GET_REQ_TIMES		_IOR	(VMA_JEBE_IOC_MAGIC, 6, DWORD)
#define VMA_JEBE_IOC_MASTER_0_CLEAR_PROFILE		_IO		(VMA_JEBE_IOC_MAGIC, 7)
#define VMA_JEBE_IOC_MASTER_1_GET_BANDWIDTH		_IOR	(VMA_JEBE_IOC_MAGIC, 8, DWORD)
#define VMA_JEBE_IOC_MASTER_1_GET_RG_INTERVAL	_IOR	(VMA_JEBE_IOC_MAGIC, 9, DWORD)
#define VMA_JEBE_IOC_MASTER_1_GET_REQ_TIMES		_IOR	(VMA_JEBE_IOC_MAGIC, 10, DWORD)
#define VMA_JEBE_IOC_MASTER_1_CLEAR_PROFILE		_IO		(VMA_JEBE_IOC_MAGIC, 11)

#define VMA_JEBE_IOC_MAX_NUMBER	11

/* ============================================================================================== */
#endif //__VMA_JEBE_H__

/* ============================================================================================== */
