/*
 * $Header: /rd_2/project/Mozart/Components/IBPE_3.0.0.0/Device_Driver/IBPE/vma_ibpe.h 2     12/09/24 7:35p Linus.huang $
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
 * $History: vma_ibpe.h $
 * 
 * *****************  Version 2  *****************
 * User: Linus.huang  Date: 12/09/24   Time: 7:35p
 * Updated in $/rd_2/project/Mozart/Components/IBPE_3.0.0.0/Device_Driver/IBPE
 *
 */

/* ============================================================================================== */
#ifndef __VMA_IBPE_H__
#define __VMA_IBPE_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VMA_IBPE_VERSION MAKEFOURCC(7, 0, 0, 2)
#define VMA_IBPE_ID_VERSION "7.0.0.2"

/* ============================================================================================== */
#define VMA_IBPE_MASTER_0_BUS_NUM	1
#define VMA_IBPE_MASTER_1_BUS_NUM	2

#define VMA_IBPE_MASTER_0_MASTER_NUM	0x04
#define VMA_IBPE_MASTER_1_MASTER_NUM	0x04

/* ============================================================================================== */
typedef struct vma_ibpe_info
{
	DWORD dwVersion; // MMR 0
	DWORD dwCycles; // MMR 1
	DWORD dwCtrl; // MMR 2
	DWORD dwStat; // MMR 3
	DWORD dwInSize; // MMR 4
	DWORD dwStride; // MMR 5
	DWORD dwNRDeimpulseCtrl; // MMR 6
	DWORD dwNRNonLocalMeansCtrl; // MMR 7
	DWORD dwEdgeEnhanceCtrl; // MMR 8
	DWORD dwInCurrTopYAddr; // MMR 9
	DWORD dwInCurrBotYAddr; // MMR 10
	DWORD dwInCurrTopCAddr; // MMR 11
	DWORD dwInCurrBotCAddr; // MMR 12
	DWORD dwInNextTopYAddr; // MMR 13
	DWORD dwInPrevBotYAddr; // MMR 14
	DWORD dwInFut0TopYAddr; // MMR 15
	DWORD dwInFut0BotYAddr; // MMR 16
	DWORD dwInFut1TopYAddr; // MMR 17
	DWORD dwInFut1BotYAddr; // MMR 18
	DWORD dwOut0YAddr; // MMR 19
	DWORD dwOut0CAddr; // MMR 20
	DWORD dwOut1YAddr; // MMR 21
	DWORD dwOut1CAddr; // MMR 22
	DWORD dwStaticMapAddr; // MMR 23
	DWORD dwMCMapAddr; // MMR 24
	DWORD dwStaticMapSize; // MMR 25
	DWORD dwMCMapSize; // MMR 26
	DWORD dwBndPixBuffAddr; // MMR 27
	DWORD dwPMaskMapAddr; // MMR 28
	DWORD dwColumnNum; // MMR 29
	DWORD dwFwdMpgParam1Y; // MMR 30
	DWORD dwFwdMpgParam2Y; // MMR 31
	DWORD dwFwdMpgParam3Y; // MMR 32
	DWORD dwFwdMpgParam1CbCr; // MMR 33
	DWORD dwFwdMpgParam2CbCr; // MMR 34
	DWORD dwFwdMpgParam3CbCr; // MMR 35
	DWORD dwLDCBuffStartShift; // MMR 36
	DWORD dwDIECtrl; // MMR 37
	DWORD dwTemporalThreshold; // MMR 38
	DWORD dwSpatialDIEConfig; // MMR 39
	DWORD dwStaticDtctConfig; // MMR 40
	DWORD dwMorphVertRegion; // MMR 41
	DWORD dwMorphHorzRegion; // MMR 42
	DWORD dwMCConfig; // MMR 43
	DWORD dwMCRegion; // MMR 44
	DWORD dwDIEPDConfig; // MMR 45
	DWORD dwDIEPDVertDiv; // MMR 46
	DWORD dwDIEPDHorzDiv; // MMR 47
	DWORD dwDIEPDBlk0Num; // MMR 48
	DWORD dwDIEPDBlk1Num; // MMR 49
	DWORD dwDIEPDBlk2Num; // MMR 50
	DWORD dwDIEPDBlk3Num; // MMR 51
	DWORD dwDIEPDTopBlk4Num; // MMR 52
	DWORD dwDIEPDBotBlk4Num; // MMR 53
	DWORD dwDIEPDBlk5Num; // MMR 54
	DWORD dwDIEPDBlk6Num; // MMR 55
	DWORD dwDIEPDBlk7Num; // MMR 56
	DWORD dwDIEPDBlk8Num; // MMR 57
	DWORD dwDIE22PDNum0; // MMR 58
	DWORD dwDIE22PDNum1; // MMR 59
	DWORD dwPMaskFillValue; // MMR 60
	DWORD dwPMaskMapConfig; // MMR 61
	DWORD dwMDWin00Start; // MMR 62
	DWORD dwMDWin00End; // MMR 63
	DWORD dwMDWin00Ctrl; // MMR 64
	DWORD dwMDWin00Ctrl2; // MMR 65
	DWORD dwMDWin01Start; // MMR 66
	DWORD dwMDWin01End; // MMR 67
	DWORD dwMDWin01Ctrl; // MMR 68
	DWORD dwMDWin01Ctrl2; // MMR 69
	DWORD dwMDWin02Start; // MMR 70
	DWORD dwMDWin02End; // MMR 71
	DWORD dwMDWin02Ctrl; // MMR 72
	DWORD dwMDWin02Ctrl2; // MMR 73
	DWORD dwMDWin03Start; // MMR 74
	DWORD dwMDWin03End; // MMR 75
	DWORD dwMDWin03Ctrl; // MMR 76
	DWORD dwMDWin03Ctrl2; // MMR 77
	DWORD dwMDWin04Start; // MMR 78
	DWORD dwMDWin04End; // MMR 79
	DWORD dwMDWin04Ctrl; // MMR 80
	DWORD dwMDWin04Ctrl2; // MMR 81
	DWORD dwMDWin05Start; // MMR 82
	DWORD dwMDWin05End; // MMR 83
	DWORD dwMDWin05Ctrl; // MMR 84
	DWORD dwMDWin05Ctrl2; // MMR 85
	DWORD dwMDWin06Start; // MMR 86
	DWORD dwMDWin06End; // MMR 87
	DWORD dwMDWin06Ctrl; // MMR 88
	DWORD dwMDWin06Ctrl2; // MMR 89
	DWORD dwMDWin07Start; // MMR 90
	DWORD dwMDWin07End; // MMR 91
	DWORD dwMDWin07Ctrl; // MMR 92
	DWORD dwMDWin07Ctrl2; // MMR 93
	DWORD dwMDWin08Start; // MMR 94
	DWORD dwMDWin08End; // MMR 95
	DWORD dwMDWin08Ctrl; // MMR 96
	DWORD dwMDWin08Ctrl2; // MMR 97
	DWORD dwMDWin09Start; // MMR 98
	DWORD dwMDWin09End; // MMR 99
	DWORD dwMDWin09Ctrl; // MMR 100
	DWORD dwMDWin09Ctrl2; // MMR 101
	DWORD dwMDWin10Start; // MMR 102
	DWORD dwMDWin10End; // MMR 103
	DWORD dwMDWin10Ctrl; // MMR 104
	DWORD dwMDWin10Ctrl2; // MMR 105
	DWORD dwMDWin11Start; // MMR 106
	DWORD dwMDWin11End; // MMR 107
	DWORD dwMDWin11Ctrl; // MMR 108
	DWORD dwMDWin11Ctrl2; // MMR 109
	DWORD dwMDWin12Start; // MMR 110
	DWORD dwMDWin12End; // MMR 111
	DWORD dwMDWin12Ctrl; // MMR 112
	DWORD dwMDWin12Ctrl2; // MMR 113
	DWORD dwMDWin13Start; // MMR 114
	DWORD dwMDWin13End; // MMR 115
	DWORD dwMDWin13Ctrl; // MMR 116
	DWORD dwMDWin13Ctrl2; // MMR 117
	DWORD dwMDWin14Start; // MMR 118
	DWORD dwMDWin14End; // MMR 119
	DWORD dwMDWin14Ctrl; // MMR 120
	DWORD dwMDWin14Ctrl2; // MMR 121
	DWORD dwMDWin15Start; // MMR 122
	DWORD dwMDWin15End; // MMR 123
	DWORD dwMDWin15Ctrl; // MMR 124
	DWORD dwMDWin15Ctrl2; // MMR 125
	DWORD dwNRStrongCtrl; // MMR 126
	DWORD dwEEHPFCoeff0; // MMR 127
	DWORD dwEEHPFCoeff1; // MMR 128
	DWORD dwEEHPFCoeff2; // MMR 129
	DWORD dwEETableBuffAddr; // MMR 130
	DWORD dwNRStrongMotionRefCtrl0; // MMR 131
	DWORD dwNRStrongMotionRefCtrl1; // MMR 132
	DWORD dwNRStrongMotionRefCtrl2; // MMR 133
	DWORD dwNRStrongMotionRefCtrl3; // MMR 134
	DWORD dwNRStrongMotionOutCtrl0; // MMR 135
	DWORD dwNRStrongMotionOutCtrl1; // MMR 136
	DWORD dwNRStrongMotionOutCtrl2; // MMR 137
	DWORD dwNRStrongMotionOutCtrl3; // MMR 138
	DWORD dwIBPEBusCfg; // MMR 139
	DWORD adwDummy[116]; // MMR 140~255
	DWORD adwEETable[64]; // MMR 256~320	
} TVMAIBPEInfo;

/* ============================================================================================== */
#define VMA_IBPE_IOC_MAGIC 228

#define VMA_IBPE_IOC_START						_IO		(VMA_IBPE_IOC_MAGIC, 0)
#define VMA_IBPE_IOC_WAIT_COMPLETE				_IO		(VMA_IBPE_IOC_MAGIC, 1)
#define VMA_IBPE_IOC_SHARE_MMR_INFO_SPACE		_IOW	(VMA_IBPE_IOC_MAGIC, 2, DWORD)
#define VMA_IBPE_IOC_GET_VERSION_NUMBER			_IOR	(VMA_IBPE_IOC_MAGIC, 3, DWORD)
#define VMA_IBPE_IOC_MASTER_0_GET_BANDWIDTH		_IOR	(VMA_IBPE_IOC_MAGIC, 4, DWORD)
#define VMA_IBPE_IOC_MASTER_0_GET_RG_INTERVAL	_IOR	(VMA_IBPE_IOC_MAGIC, 5, DWORD)
#define VMA_IBPE_IOC_MASTER_0_GET_REQ_TIMES		_IOR	(VMA_IBPE_IOC_MAGIC, 6, DWORD)
#define VMA_IBPE_IOC_MASTER_0_CLEAR_PROFILE		_IO		(VMA_IBPE_IOC_MAGIC, 7)
#define VMA_IBPE_IOC_MASTER_1_GET_BANDWIDTH		_IOR	(VMA_IBPE_IOC_MAGIC, 8, DWORD)
#define VMA_IBPE_IOC_MASTER_1_GET_RG_INTERVAL	_IOR	(VMA_IBPE_IOC_MAGIC, 9, DWORD)
#define VMA_IBPE_IOC_MASTER_1_GET_REQ_TIMES		_IOR	(VMA_IBPE_IOC_MAGIC, 10, DWORD)
#define VMA_IBPE_IOC_MASTER_1_CLEAR_PROFILE		_IO		(VMA_IBPE_IOC_MAGIC, 11)

#define VMA_IBPE_IOC_MAX_NUMBER	11

/* ============================================================================================== */
#endif //__VMA_IBPE_H__

/* ============================================================================================== */
