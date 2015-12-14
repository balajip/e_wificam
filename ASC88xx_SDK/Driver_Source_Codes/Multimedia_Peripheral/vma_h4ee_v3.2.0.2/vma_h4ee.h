/*
 * $Header: /rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE/vma_h4ee.h 19    12/09/11 10:52a Jaja $
 *
 * vma_h4ee
 * Driver for VMA H4EE
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
 * $History: vma_h4ee.h $
 * 
 * *****************  Version 19  *****************
 * User: Jaja         Date: 12/09/11   Time: 10:52a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 18  *****************
 * User: Jaja         Date: 12/09/07   Time: 8:22a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 17  *****************
 * User: Jaja         Date: 12/08/08   Time: 10:52a
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 16  *****************
 * User: Jaja         Date: 12/06/20   Time: 5:54p
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 * *****************  Version 15  *****************
 * User: Jaja         Date: 12/01/11   Time: 7:02p
 * Updated in $/rd_2/project/SoC/Components/H4EE/Device_Driver/H4EE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_H4EE_H__
#define __VMA_H4EE_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VMA_H4EE_VERSION MAKEFOURCC(3, 2, 0, 2)
#define VMA_H4EE_ID_VERSION "3.2.0.2"

/* ============================================================================================== */
#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
//#define VMA_H4EE_MASTER_0_BUS_BASE_ADDR	VPL_AHBC_0_MMR_BASE			// MozartI,
//#define VMA_H4EE_MASTER_1_BUS_BASE_ADDR	VPL_AHBC_2_MMR_BASE			// MozartI,
//
//#define VMA_H4EE_MASTER_0_MASTER_NUM	0x0B	// MozartI,
//#define VMA_H4EE_MASTER_1_MASTER_NUM	0x0B	// MozartI,
////
//#define VMA_H4EE_MASTER_0_BUS_BASE_ADDR	VPL_AHBC_1_MMR_BASE				// Mozart III FPGA
//#define VMA_H4EE_MASTER_1_BUS_BASE_ADDR	VPL_AHBC_2_MMR_BASE           	// Mozart III FPGA
//#define VMA_H4EE_MASTER_2_BUS_BASE_ADDR	VPL_AHBC_3_MMR_BASE           	// Mozart III FPGA
//#define VMA_H4EE_MASTER_3_BUS_BASE_ADDR	VPL_AHBC_4_MMR_BASE           	// Mozart III FPGA
//
//#define VMA_H4EE_MASTER_0_MASTER_NUM	0x0A	// Mozart III FPGA
//#define VMA_H4EE_MASTER_1_MASTER_NUM	0x0A	// Mozart III FPGA
//#define VMA_H4EE_MASTER_2_MASTER_NUM	0x0A	// Mozart III FPGA
//#define VMA_H4EE_MASTER_3_MASTER_NUM	0x0A	// Mozart III FPGA

#define VMA_H4EE_MASTER_0_BUS_BASE_ADDR	VPL_AHBC_0_MMR_BASE				// Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_1_BUS_BASE_ADDR	VPL_AHBC_1_MMR_BASE             	// Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_2_BUS_BASE_ADDR	VPL_AHBC_2_MMR_BASE             	// Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_3_BUS_BASE_ADDR	VPL_AHBC_2_MMR_BASE             	// Mozart III, Paganini Chip
                                                  // Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_0_MASTER_NUM	0x0B      	// Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_1_MASTER_NUM	0x03      	// Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_2_MASTER_NUM	0x0A      	// Mozart III, Paganini Chip
#define VMA_H4EE_MASTER_3_MASTER_NUM	0x0B      	// Mozart III, Paganini Chip

#endif //__ASM_ARCH_PLATFORM_MOZART_H__

#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
#define VMA_H4EE_MASTER_0_BUS_BASE_ADDR	VPL_AHBC_1_MMR_BASE
#define VMA_H4EE_MASTER_1_BUS_BASE_ADDR	VPL_AHBC_2_MMR_BASE
#define VMA_H4EE_MASTER_2_BUS_BASE_ADDR	VPL_AHBC_3_MMR_BASE
#define VMA_H4EE_MASTER_3_BUS_BASE_ADDR	VPL_AHBC_5_MMR_BASE

#define VMA_H4EE_MASTER_0_MASTER_NUM	0x0A
#define VMA_H4EE_MASTER_1_MASTER_NUM	0x0A
#define VMA_H4EE_MASTER_2_MASTER_NUM	0x0A
#define VMA_H4EE_MASTER_3_MASTER_NUM	0x0A
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

/* ============================================================================================== */
typedef struct vma_h4ee_info
{
	DWORD dwVersion; // MMR 0					0
	DWORD dwCycles; // MMR 1                    4
	DWORD dwCtrl; // MMR 2                      8
	DWORD dwStat; // MMR 3                      C
	DWORD dwCodingCtrl; // MMR 4                10
	DWORD dwYCurrAddr; // MMR 5                 14
	DWORD dwCbCurrAddr; // MMR 6                18
	DWORD dwCrCurrAddr; // MMR 7                1C
	DWORD dwLumaDeblockAddr; // MMR 8           20
	DWORD dwChmaDeblockAddr; // MMR 9           24
	DWORD dwLumaFwdRef0Addr; // MMR 10          28
	DWORD dwChmaFwdRef0Addr; // MMR 11          2C
	DWORD dwLumaFwdRef1Addr; // MMR 12          30
	DWORD dwChmaFwdRef1Addr; // MMR 13          34
	DWORD dwMBInfoAddr; // MMR 14               38
	DWORD dwBSLowBoundAddr; // MMR 15           3C
	DWORD dwBSUpBoundAddr; // MMR 16            40
	DWORD dwCMTblAddr; // MMR 17                44
	DWORD dwIntraPredAddr; // MMR 18            48
	DWORD dwMBNumInfo; // MMR 19                4C
	DWORD dwSizeInfo; // MMR 20                 50
	DWORD dwQuantInfo; // MMR 21                54
	DWORD dwBSSizeInfo; // MMR 22               58
	DWORD dwPrevSliceInfo0; // MMR 23           5C
	DWORD dwPrevSliceInfo1; // MMR 24           60
	DWORD dwPrevSliceInfo2; // MMR 25           64
	DWORD dwPrevSliceInfo3; // MMR 26           68
	DWORD dwUnalignMSBs; // MMR 27              6C
	DWORD dwUnalignLSBs; // MMR 28              70
	DWORD dwBinsCnt; // MMR 29                  74
	DWORD dwCoeffCost; // MMR 30                78
	DWORD dwMEBECodingCtrl; // MMR 31			7C
	DWORD dwMEBEVLCOutAddr; // MMR 32           80
	DWORD dwMEBETblAddr; // MMR 33              84
	DWORD dwMEBEMBNum; // MMR 34                88
	DWORD dwMEBEWidthBuffSize; // MMR 35        8C
	DWORD dwMEBEQuantBuffSize; // MMR 36        90
	DWORD dwMEBEVPPacketSize; // MMR 37         94
	DWORD dwMEBEVPInfoVPNum; // MMR 38          98
	DWORD dwMEBEVPHdrInfo; // MMR 39            9C
	DWORD dwMEBEVPExtHdrInfo; // MMR 40         A0
	DWORD dwIntraPredModeEnCtrl; // MMR 41      A4
	DWORD dwMBRefreshInfo; // MMR 42            A8
	DWORD dwFmeBlkDisCtrl; // MMR 43            AC
	DWORD dwLambdaQModeCost; // MMR 44          B0
	DWORD dwObjectInfoAddr; // MMR 45           B4
	DWORD dwObjectInfoReserve; // MMR 46        B8
	DWORD dwObjectMBsStride; // MMR 47          BC
	DWORD dwDeblocAhbTsfConfig; // MMR 48       C0
	DWORD dwH4eeConfigInfo0; // MMR 49          C4
	DWORD dwH4eeConfigInfo1; // MMR 50          C8
	DWORD dwH4eeConfigInfo2; // MMR 51          CC
	DWORD dwH4eeConfigInfo3; // MMR 52          D0
	DWORD dwH4eeConfigInfo4; // MMR 53          D4
	DWORD dwH4eeConfigInfo5; // MMR 54			D8
	DWORD dwColIntraRefresh0Info; // MMR 55     DC
	DWORD dwColIntraRefresh1Info; // MMR 56     E0
	DWORD dwCMTblAddr2; // MMR 57               E4
	DWORD dwIntraPredAddr2; // MMR 58           E8
	DWORD dwLumaUpDeblockAddr2; // MMR 59       EC
	DWORD dwMozartV3CodingCtrl; // MMR 60       F0
	DWORD dwMozartV3MeSadThr; // MMR 61         F4
	DWORD dwMozartV3MeMvFactor; // MMR 62       F8
	DWORD dwMozartV3BndBoxQpd0; // MMR 63       FC
	DWORD dwMozartV3BndBoxQpd1; // MMR 64       100
	DWORD dwMozartV3InBndBoxMode; // MMR 65     104
	DWORD dwMozartV3OutBndBoxMode; // MMR 66    108
	DWORD dwPrevSlice2Info2; // MMR 67          10C
	DWORD dwPrevSlice2Info3; // MMR 68          110
	DWORD dwUnalignMSBs2; // MMR 69             114
	DWORD dwUnalignLSBs2; // MMR 70				118
	DWORD dwBinsCnt2; // MMR 71                 11C
	DWORD dwCoeffCost2; // MMR 72               120
	DWORD dwIntraPredModeEnCtrl2; // MMR 73     124
	DWORD dwMBRefreshInfo2; // MMR 74           128
	DWORD dwFmeBlkDisCtrl2; // MMR 75           12C
	DWORD dwLambdaQModeCost2; // MMR 76			130
	DWORD dwObject2InfoAddr; // MMR 77          134
	DWORD dwMeDisableTbl; // MMR 78             138
	DWORD dwMeDisableTbl2; // MMR 79            13C
	DWORD adwObjBndBoxInfo[32];//80,81,82,83,84,85,86,87,88,89			// start 140
	                           //90,91,92,93,94,95,96,97,98,99
	                           //100,101,102,103,104,105,106,107,108,109
	                           //110,111
	DWORD dwConfigCycleUsed; // MMR 112	//1C0
} TVMAH4EEInfo;

/* ============================================================================================== */
#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define VMA_H4EE_IOC_MAGIC 219
#endif //__ASM_ARCH_PLATFORM_MOZART_H__
#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
#define VMA_H4EE_IOC_MAGIC 234
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

#define VMA_H4EE_IOC_START						_IO		(VMA_H4EE_IOC_MAGIC, 0)
#define VMA_H4EE_IOC_WAIT_COMPLETE				_IO		(VMA_H4EE_IOC_MAGIC, 1)
#define VMA_H4EE_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VMA_H4EE_IOC_MAGIC, 2, DWORD)
#define VMA_H4EE_IOC_GET_VERSION_NUMBER			_IOR	(VMA_H4EE_IOC_MAGIC, 3, DWORD)
#define VMA_H4EE_IOC_GET_CHIP_VERSION_NUMBER	_IOR	(VMA_H4EE_IOC_MAGIC, 4, DWORD)
#define VMA_H4EE_IOC_MASTER_0_GET_BANDWIDTH		_IOR	(VMA_H4EE_IOC_MAGIC, 5, DWORD)
#define VMA_H4EE_IOC_MASTER_0_GET_RG_INTERVAL	_IOR	(VMA_H4EE_IOC_MAGIC, 6, DWORD)
#define VMA_H4EE_IOC_MASTER_0_GET_REQ_TIMES		_IOR	(VMA_H4EE_IOC_MAGIC, 7, DWORD)
#define VMA_H4EE_IOC_MASTER_0_CLEAR_PROFILE		_IO		(VMA_H4EE_IOC_MAGIC, 8)
#define VMA_H4EE_IOC_MASTER_1_GET_BANDWIDTH		_IOR	(VMA_H4EE_IOC_MAGIC, 9, DWORD)
#define VMA_H4EE_IOC_MASTER_1_GET_RG_INTERVAL	_IOR	(VMA_H4EE_IOC_MAGIC, 10, DWORD)
#define VMA_H4EE_IOC_MASTER_1_GET_REQ_TIMES		_IOR	(VMA_H4EE_IOC_MAGIC, 11, DWORD)
#define VMA_H4EE_IOC_MASTER_1_CLEAR_PROFILE		_IO		(VMA_H4EE_IOC_MAGIC, 12)
#define VMA_H4EE_IOC_MASTER_2_GET_BANDWIDTH		_IOR	(VMA_H4EE_IOC_MAGIC, 13, DWORD)
#define VMA_H4EE_IOC_MASTER_2_GET_RG_INTERVAL	_IOR	(VMA_H4EE_IOC_MAGIC, 14, DWORD)
#define VMA_H4EE_IOC_MASTER_2_GET_REQ_TIMES		_IOR	(VMA_H4EE_IOC_MAGIC, 15, DWORD)
#define VMA_H4EE_IOC_MASTER_2_CLEAR_PROFILE		_IO		(VMA_H4EE_IOC_MAGIC, 16)
#define VMA_H4EE_IOC_MASTER_3_GET_BANDWIDTH		_IOR	(VMA_H4EE_IOC_MAGIC, 17, DWORD)
#define VMA_H4EE_IOC_MASTER_3_GET_RG_INTERVAL	_IOR	(VMA_H4EE_IOC_MAGIC, 18, DWORD)
#define VMA_H4EE_IOC_MASTER_3_GET_REQ_TIMES		_IOR	(VMA_H4EE_IOC_MAGIC, 19, DWORD)
#define VMA_H4EE_IOC_MASTER_3_CLEAR_PROFILE		_IO		(VMA_H4EE_IOC_MAGIC, 20)
#define VMA_H4EE_IOC_GET_CHIP_MODEL_INFO    	_IOR	(VMA_H4EE_IOC_MAGIC, 21, DWORD)
#define VMA_H4EE_IOC_RESET						_IO		(VMA_H4EE_IOC_MAGIC, 22)

#define VMA_H4EE_IOC_MAX_NUMBER	22

/* ============================================================================================== */
#endif //__VMA_H4EE_H__

/* ============================================================================================== */
