/*
 * $Header: /rd_2/project/Mozart/Components/IRE/Device_Driver/IRE/vma_ire.h 4     12/07/09 6:08p Kensuke.chen $
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
 * $History: vma_ire.h $
 * 
 * *****************  Version 4  *****************
 * User: Kensuke.chen Date: 12/07/09   Time: 6:08p
 * Updated in $/rd_2/project/Mozart/Components/IRE/Device_Driver/IRE
 * 
 * *****************  Version 3  *****************
 * User: Kensuke.chen Date: 12/07/09   Time: 5:32p
 * Updated in $/rd_2/project/Mozart/Components/IRE/Device_Driver/IRE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_IRE_H__
#define __VMA_IRE_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VMA_IRE_VERSION MAKEFOURCC(2, 0, 0, 11)
#define VMA_IRE_ID_VERSION "2.0.0.11"

/* ============================================================================================== */
#define VMA_IRE_BUS_NUM	2

#define VMA_IRE_MASTER_NUM	0x0D

/* ============================================================================================== */
typedef struct vma_ire_info
{
	DWORD dwVersion; // MMR 0
	DWORD dwCycles; // MMR 1
	DWORD dwCtrl; // MMR 2
	DWORD dwStat; // MMR 3
	DWORD dwYInAddr; // MMR 4
	DWORD dwCbInAddr; // MMR 5
	DWORD dwCrInAddr; // MMR 6
	DWORD dwYOutAddr; // MMR 7
	DWORD dwCbOutAddr; // MMR 8
	DWORD dwCrOutAddr; // MMR 9
	DWORD dwInSize; // MMR 10
	DWORD dwStride; // MMR 11
	DWORD dwOutSize; // MMR 12
	DWORD dwYOutInverse; // MMR 13
	DWORD dwCbCrOutInverse; // MMR 14
	DWORD dwHorzBlockNum; // MMR 15
	DWORD dwScaleInt; // MMR 16
} TVMAIREInfo;

/* ============================================================================================== */
#define VMA_IRE_IOC_MAGIC 253

#define VMA_IRE_IOC_START						_IO		(VMA_IRE_IOC_MAGIC, 0)
#define VMA_IRE_IOC_WAIT_COMPLETE				_IO		(VMA_IRE_IOC_MAGIC, 1)
#define VMA_IRE_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VMA_IRE_IOC_MAGIC, 2, DWORD)
#define VMA_IRE_IOC_GET_VERSION_NUMBER			_IOR	(VMA_IRE_IOC_MAGIC, 3, DWORD)
#define VMA_IRE_IOC_GET_BANDWIDTH				_IOR	(VMA_IRE_IOC_MAGIC, 4, DWORD)
#define VMA_IRE_IOC_GET_RG_INTERVAL				_IOR	(VMA_IRE_IOC_MAGIC, 5, DWORD)
#define VMA_IRE_IOC_GET_REQ_TIMES				_IOR	(VMA_IRE_IOC_MAGIC, 6, DWORD)
#define VMA_IRE_IOC_CLEAR_PROFILE				_IO		(VMA_IRE_IOC_MAGIC, 7)

#define VMA_IRE_IOC_MAX_NUMBER	7

/* ============================================================================================== */
#endif //__VMA_IRE_H__

/* ============================================================================================== */
