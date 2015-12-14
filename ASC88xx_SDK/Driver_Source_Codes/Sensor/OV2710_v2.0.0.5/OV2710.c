/*
 * OV2710
 * Driver for OV2710 sensor.
 *
 * Copyright (C) 2010  VN Inc.
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
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "OV2710.h"
#include "OV2710_locals.h"
#include "IICCtrl.h"
/* Version 1.1.0.2 modification, 2010.06.03 */	
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()
/* ========================================= */

const CHAR OV2710_ID[] = "$Version: "OV2710_ID_VERSION"  (OV2710 DRIVER) $";
/* Version 1.1.0.2 modification, 2010.06.03 */	
static struct semaphore ov2710_mutex;
/* ========================================= */

/*==================================================================*/
static TOV2710Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;

/* Version 2.0.0.0 modification, 2011.01.21 */
static DWORD dwOV2710Hsync = OV2710_HSYNC_1920x1080_60Hz;
/* ======================================== */

/* Version 2.0.0.4 modification, 2012.08.06 */
static int iEnAWB=0;
module_param(iEnAWB, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (iEnAWB, "Enable sensor AWB");
/* ======================================== */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("OV2710 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TOV2710RegAddrData atOV2710regTbl[OV2710_TBL_SIZE] = {
	{_3103_PLLClockSelect,      0x03}, // default 0x91
	{_3008_SystemControl,       0x82}, // default 0x02
	{_3706_,                    0x61}, // default 0x21
	{_3712_,                    0x0C}, // default 0x06
	{_3630_,                    0x6D}, // default 0x6B
	{_3801_HREFStartXLow,       0xB4}, // default 0x7C
	{_3621_ArrayControl,        0x04}, // default 0x17
	{_3604_,                    0x60}, // default 0xE4
	{_3603_,                    0xA7}, // default 0x27
	{_3631_,                    0x26}, // default 0x24
	{_3600_,                    0x04}, // default 0x54
	//{_3620_,                    0x37}, // default 0x37
	//{_3623_,                    0x00}, // default 0x00
	{_3702_,                    0x9E}, // default 0x3A
	{_3703_,                    0x74}, // default 0x6C
	{_3704_,                    0x10}, // default 0x5C
	{_370D_SensorReg0D,         0x07}, // default 0x0C
	{_3713_,                    0x8B}, // default 0x08
	{_3714_,                    0x74}, // default 0x24
	{_3710_,                    0x9E}, // default 0x14
	{_3801_HREFStartXLow,       0xC4}, // default 0x7C
	{_3605_,					0x05}, // default 0x01
	{_3606_,					0x12}, // default 0x00
	{_302D_,					0x90}, // default 0x10
	{_370B_,					0x40}, // default 0x00
	//{_380C_HorzTotalHigh,		0x09}, // default 0x09
	{_380D_HorzTotalLow,        0x74}, // default 0x60
	{_3809_HorzWidthLow,        0x81}, // default 0x80
	{_380B_VertHeightLow,       0x39}, // default 0x38
	{_4708_DVPCtrl02,           0x03}, // default 0x01, change VSYNC polarity
	{_5181_AWBControl01,        0x20}, // default 0x02
	{_518F_AWBFrameInterval,    0x00}, // default 0x04
	{_4301_,                    0xFF}, // default 0xF0
	{_4303_,                    0x00}, // default 0x10
	//{_3A00_AECControl00,        0x7C}, // default 0x7C, [2] night mode; [3] fractional line disable [4] auto band [5] banding ON [6] LAEC
	{_300F_PLLControl00,        0x88}, // default 0x8A, [1:0]: PLL SELD5 divider; 2'b0X bypass, 2'b10 divided by 4 in 8-bit, 2'b11 divided by 5 in 10-bit [7:6]: Div1to2p5 00/01/10/11=>1/1/2/2.5
	//{_3010_PLLControl02,        0x00}, // default 0x00, [7:4]: system divider DIVS=0; [3:0]: MIPI divider DIVM=0
	{_3011_PLLControl02,        0x20}, // default 0x0A, [5:0]: DIVP, [7]: PLL bypass. XVCLK/PRE_DIV*DIVP/(DIVS+1)/Div1to2p5/4 24/1.5*40/1/2/4 = 20/1*32/1/2/4 = 80MHz
	{_3012_PLLPreDivider,       0x00}, // default 0x0A, [2:0]: PRE_DIV=1, XVCLK/PRE_DIV>6MHz, XVCLK/PRE_DIV*DIVP=300~1000MHz
	{_3A0E_AECControl0E,		0x04}, // default 0x03
	//{_3A0D_AECControl0D,		0x04}, // default 0x04
	{_3A08_AECB50StepHigh,		0x11}, // default 0x14, 1/100*25*1104*16=4416 (0x1140)
	{_3A09_AECB50StepLow,		0x40}, // default 0xC0
	//{_3A0A_AECB60StepHigh,		0x11}, // default 0x11, 1/120*30*1104*16=4416 (0x1140)
	//{_3A0B_AECB60StepLow,		0x40}, // default 0x40
	//{_3A02_AECMaxExpo60H,		0x02}, // default 0x02
	//{_3A03_AECMaxExpo60M,		0x28}, // default 0x28
	//{_3A04_AECMaxExpo60L,		0x00}, // default 0x00, max exposure time: 8 frames for 60Hz night mode
	//{_3A14_AECMaxExpo50H,		0x01}, // default 0x01
	//{_3A15_AECMaxExpo50M,		0xF2}, // default 0xF2
	//{_3A16_AECMaxExpo50L,		0x00}, // default 0x00, max exposure time: 8 frames for 50Hz night mode
	{_3A1A_,                    0x06}, // default 0x02
	{_3A18_AECGainCeilH,        0x00}, // default 0x03
	{_3A19_AECGainCeilL,        0x7A}, // default 0xE0, max gain 8x
	//{_3503_AECManualMode,       0x00}, // default 0x00
	{_3A13_AECControl13,        0x54}, // default 0x50
	{_382E_,					0x0F}, // default 0x04
	{_381A_,					0x1A}, // default 0x00
	{_5688_,                    0x03}, // default 0x02
	//{_5684_AVGStartYHigh,       0x07}, // default 0x07
	//{_5685_AVGStartYLow,        0x80}, // default 0x80
	//{_5686_AVGEndYHigh,         0x04}, // default 0x04
	//{_5687_AVGEndYLow,          0x38}, // default 0x38
	{_3A0F_AECControl0F,        0x40}, // default 0x78
	{_3A10_AECControl10,        0x38}, // default 0x68
	{_3A1B_AECControl1B,        0x48}, // default 0x78
	{_3A1E_AECControl1E,        0x30}, // default 0x68
	{_3A11_AECControl11,        0x90}, // default 0xD0
	{_3A1F_AECControl1F,        0x10}, // default 0x40
	//{_3A05_,					0x30}, // default 0x30, [7]: 0/1=60Hz/50Hz
	//{_503D_ISPTest,				0x00}, // default 0x00, A0/A4 [7] Color bar enable, [5:4] 2'b10 color bar pattern, [2] color bar rolling
	{_3200_GroupAddr0,			0x00}, // default 0x40, move all space to group 0 and total 40 registers
	{_3201_GroupAddr1,			0x7F}, // default 0x4A
	{_3202_GroupAddr2,			0x7F}, // default 0x54
	{_3203_GroupAddr3,			0x7F}, // default 0x5E
/* Version 2.0.0.3 modification, 2011.09.05 */
	{_302C_PADOutputDrv,		0x02}, // default 0x02
/* ======================================== */
	{_3017_PADOutputEnable01,   0x7F}, // default 0x00, enable VSYNC/HREF/PCLK/D[9:6]
	{_3018_PADOutputEnable02,   0xFC}  // default 0x00, enable D[5:0]
};

/*-------------------------------------------------------------------------------------*/
const TOV2710RegAddrData atOV2710regDisableDSPTbl[OV2710_NO_DSP_TBL_SIZE] =
{
	{_3103_PLLClockSelect,      0x03}, // default 0x91
	{_3008_SystemControl,       0x82}, // default 0x02
	{_3706_,                    0x61}, // default 0x21
	{_3712_,                    0x0C}, // default 0x06
	{_3630_,                    0x6D}, // default 0x6B
	{_3801_HREFStartXLow,       0xB4}, // default 0x7C
	{_3621_ArrayControl,        0x04}, // default 0x17
	{_3604_,                    0x60}, // default 0xE4
	{_3603_,                    0xA7}, // default 0x27
	{_3631_,                    0x26}, // default 0x24
	{_3600_,                    0x04}, // default 0x54
	//{_3620_,                    0x37}, // default 0x37
	//{_3623_,                    0x00}, // default 0x00
	{_3702_,                    0x9E}, // default 0x3A
	{_3703_,                    0x74}, // default 0x6C
	{_3704_,                    0x10}, // default 0x5C
	{_370D_SensorReg0D,         0x07}, // default 0x0C
	{_3713_,                    0x8B}, // default 0x08
	{_3714_,                    0x74}, // default 0x24
	{_3710_,                    0x9E}, // default 0x14
	{_3801_HREFStartXLow,       0xC4}, // default 0x7C
	{_3605_,					0x05}, // default 0x01
	{_3606_,					0x12}, // default 0x00
	{_302D_,					0x90}, // default 0x10
	{_370B_,					0x40}, // default 0x00
	//{_380C_HorzTotalHigh,		0x09}, // default 0x09
	{_380D_HorzTotalLow,        0x74}, // default 0x60
	{_3809_HorzWidthLow,        0x81}, // default 0x80
	{_380B_VertHeightLow,       0x39}, // default 0x38
	{_4708_DVPCtrl02,           0x03}, // default 0x01, change VSYNC polarity
	{_5181_AWBControl01,        0x20}, // default 0x02
	{_518F_AWBFrameInterval,    0x00}, // default 0x04
	{_4301_,                    0xFF}, // default 0xF0
	{_4303_,                    0x00}, // default 0x10
	//{_3A00_AECControl00,        0x7C}, // default 0x7C, [2] night mode; [3] fractional line disable [4] auto band [5] banding ON [6] LAEC
	{_300F_PLLControl00,        0x88}, // default 0x8A, [1:0]: PLL SELD5 divider; 2'b0X bypass, 2'b10 divided by 4 in 8-bit, 2'b11 divided by 5 in 10-bit
	//{_3010_PLLControl02,        0x00}, // default 0x00, [7:4]: system divider DIVS=0; [3:0]: MIPI divider DIVM=0
	{_3011_PLLControl02,        0x20}, // default 0x0A, [5:0]: DIVP, [7]: PLL bypass. XVCLK/PRE_DIV*DIVP/(DIVS+1) 24/1.5*40 = 20/1*32
	{_3012_PLLPreDivider,       0x00}, // default 0x0A, [2:0]: PRE_DIV=1
	{_3A0E_AECControl0E,		0x04}, // default 0x03
	//{_3A0D_AECControl0D,		0x04}, // default 0x04
	{_3A08_AECB50StepHigh,		0x11}, // default 0x14, 1/100*25*1104*16=4416 (0x1140)
	{_3A09_AECB50StepLow,		0x40}, // default 0xC0
	//{_3A0A_AECB60StepHigh,		0x11}, // default 0x11, 1/120*30*1104*16=4416 (0x1140)
	//{_3A0B_AECB60StepLow,		0x40}, // default 0x40
	//{_3A02_AECMaxExpo60H,		0x02}, // default 0x02
	//{_3A03_AECMaxExpo60M,		0x28}, // default 0x28
	//{_3A04_AECMaxExpo60L,		0x00}, // default 0x00, max exposure time: 8 frames for 60Hz night mode
	//{_3A14_AECMaxExpo50H,		0x01}, // default 0x01
	//{_3A15_AECMaxExpo50M,		0xF2}, // default 0xF2
	//{_3A16_AECMaxExpo50L,		0x00}, // default 0x00, max exposure time: 8 frames for 50Hz night mode
	{_3A1A_,                    0x06}, // default 0x02
	{_3A18_AECGainCeilH,        0x00}, // default 0x03
	{_3A19_AECGainCeilL,        0x7A}, // default 0xE0, max gain 8x
	//{_3503_AECManualMode,       0x00}, // default 0x00
	{_3A13_AECControl13,        0x54}, // default 0x50
	{_382E_,					0x0F}, // default 0x04
	{_381A_,					0x1A}, // default 0x00
	{_5688_,                    0x03}, // default 0x02
	//{_5684_AVGStartYHigh,       0x07}, // default 0x07
	//{_5685_AVGStartYLow,        0x80}, // default 0x80
	//{_5686_AVGEndYHigh,         0x04}, // default 0x04
	//{_5687_AVGEndYLow,          0x38}, // default 0x38
	{_3A0F_AECControl0F,        0x40}, // default 0x78
	{_3A10_AECControl10,        0x38}, // default 0x68
	{_3A1B_AECControl1B,        0x48}, // default 0x78
	{_3A1E_AECControl1E,        0x30}, // default 0x68
	{_3A11_AECControl11,        0x90}, // default 0xD0
	{_3A1F_AECControl1F,        0x10}, // default 0x40
	//{_3A05_,					0x30}, // default 0x30, [7]: 0/1=60Hz/50Hz
	//{_503D_ISPTest,				0x00}, // default 0x00, A0/A4 [7] Color bar enable, [5:4] 2'b10 color bar pattern, [2] color bar rolling
	{_3406_AWBManualCtrl,		0x01}, // default 0x00, [0]: 0/1=auto/manual WB
	//{_5001_ISPControl1,			0x4E}, // default 0x4F, [0]: 0/1=AWB disable/enable
	{_3400_RedGainH,			0x04}, // default 0x04
	{_3401_RedGainL,			0x00}, // default 0x00
	{_3402_GreenGainH,			0x04}, // default 0x04
	{_3403_GreenGainL,			0x00}, // default 0x00
	{_3404_BlueGainH,			0x04}, // default 0x04
	{_3405_BlueGainL,			0x00}, // default 0x00
	{_3200_GroupAddr0,			0x00}, // default 0x40, move all space to group 0 and total 40 registers
	{_3201_GroupAddr1,			0x7F}, // default 0x4A
	{_3202_GroupAddr2,			0x7F}, // default 0x54
	{_3203_GroupAddr3,			0x7F}, // default 0x5E
/* Version 2.0.0.3 modification, 2011.09.05 */
	{_302C_PADOutputDrv,		0x02}, // default 0x02
/* ======================================== */
	{_3017_PADOutputEnable01,   0x7F}, // default 0x00, enable VSYNC/HREF/PCLK/D[9:6]
	{_3018_PADOutputEnable02,   0xFC}  // default 0x00, enable D[5:0]
};

/*-------------------------------------------------------------------------------------*/
const BYTE abyOV2710Brightness[13] =
{
	0x30,
	0x40,
	0x50,
	0x60,
	0x70,
	0x80,
	0x90,
	0xA0,
	0xB0,
	0xC0,
	0xD0,
	0xE0,
	0xF0
};

/*-------------------------------------------------------------------------------------*/
const BYTE abyOV2710Contrast[11] =
{
	0x50,
	0x40,
	0x30,
	0x20,
	0x10,
	0x00,
	0x90,
	0xA0,
	0xB0,
	0xC0,
	0xD0
};

/*-------------------------------------------------------------------------------------*/
const BYTE abyOV2710Saturation[11] =
{
	0x00,
	0x02,
	0x04,
	0x05,
	0x06,
	0x07,
	0x08,
	0x09,
	0x0B,
	0x0D,
	0x0F
};

/*-------------------------------------------------------------------------------------*/
const BYTE abyOV2710Hue[11] =
{
	0x20,
	0x23,
	0x26,
	0x29,
	0x2C,
	0x30,
	0x33,
	0x36,
	0x39,
	0x3C,
	0x3F
};

/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*==================================================================*/
static SOCKET OV2710_WriteBuf(TOV2710Info *ptInfo, EOV2710Regs eRegAddr, BYTE byData)
{
	BYTE abyData[2];

	abyData[0] = (BYTE)(eRegAddr&0xFF);
	abyData[1] = byData;
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), 2, abyData);
}

/*-------------------------------------------------------------------------------------*/ 
static SOCKET OV2710_ReadBuf(TOV2710Info *ptInfo, EOV2710Regs eRegAddr, BYTE *pbyData)
{
	*pbyData = (BYTE)(eRegAddr&0xFF);
	return ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, ((eRegAddr&0xFF00)>>8), 1, pbyData);	
}
 
/*-------------------------------------------------------------------------------------*/
static SOCKET OV2710_CompBuf(TOV2710Info *ptInfo, EOV2710Regs eRegAddr, BYTE byMask, BYTE byData)
{
	BYTE byReadData;

	OV2710_ReadBuf(ptInfo, eRegAddr, &byReadData);
	return ((byReadData&byMask) == byData);
}

/*-------------------------------------------------------------------------------------*/
static SOCKET OV2710_UpdateBuf(TOV2710Info *ptInfo, EOV2710Regs eRegAddr, BYTE byMask, BYTE byData)
{
	BYTE byReadData, byWriteData;

	if (OV2710_ReadBuf(ptInfo, eRegAddr, &byReadData) < 0)
	{
		return -1;
	}
	byWriteData = ((byReadData&byMask) | byData);
	if (OV2710_WriteBuf(ptInfo, eRegAddr, byWriteData) < 0)
	{
		return -1;
	}
	return 0;
}


/*-------------------------------------------------------------------------------------*/
static void OV2710_ReadCompGain(TOV2710Info *ptInfo, EOV2710Regs eRegAddrH, EOV2710Regs eRegAddrL, DWORD *pdwData)
{
	BYTE byData;

	OV2710_ReadBuf(ptInfo, eRegAddrH, &byData) ;
	*pdwData = ((byData&0x0F) << 8);
	OV2710_ReadBuf(ptInfo, eRegAddrL, &byData) ;
	*pdwData = *pdwData + byData;
}

/*-------------------------------------------------------------------------------------*/
static void OV2710_Reset(TOV2710Info *ptInfo)
{
	DWORD dwIndex;

	OV2710_WriteBuf(ptInfo, _3008_SystemControl, 0x82);
	while (OV2710_CompBuf(ptInfo, _3008_SystemControl, 0x80, 0x00) != 1);
	if (ptInfo->bOV2710DSPEnable == TRUE)   // AWB
	{
		for (dwIndex=0; dwIndex<OV2710_TBL_SIZE; dwIndex++)
		{
			OV2710_WriteBuf(ptInfo, atOV2710regTbl[dwIndex].eRegAddr, atOV2710regTbl[dwIndex].byData);
		}
/* Version 2.0.0.4 modification, 2012.08.06 */
/* ======================================== */
	}
	else 
	{
 		for (dwIndex=0; dwIndex<OV2710_NO_DSP_TBL_SIZE; dwIndex++)
		{
			OV2710_WriteBuf(ptInfo, atOV2710regDisableDSPTbl[dwIndex].eRegAddr, atOV2710regDisableDSPTbl[dwIndex].byData);
		}

/* Version 2.0.0.4 modification, 2012.08.06 */
		OV2710_WriteBuf(ptInfo, _5001_ISPControl1, 0x4E); //disable AWB		
/* ======================================== */
	}

/* Version 2.0.0.4 modification, 2012.08.06 */
	OV2710_WriteBuf(ptInfo, _3503_AECManualMode, 0x07); //disable AE		

	OV2710_WriteBuf(ptInfo, 0x3803, 0x0B);
	OV2710_WriteBuf(ptInfo, 0x3807, 0x38);
	OV2710_WriteBuf(ptInfo, 0x380B, 0x38);

	OV2710_WriteBuf(ptInfo, 0x3809, 0x80);
/* ======================================== */
/* Version 2.0.0.0 modification, 2011.01.21 */	
	OV2710_WriteBuf(ptInfo, _5000_ISPControl0, 0xDB);
/* ======================================== */
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_50HZ)
	{
		// enable group 0
		OV2710_WriteBuf(ptInfo, _3212_GroupAccess, 0x00);
		OV2710_WriteBuf(ptInfo, _3A05_, 0xB0);
		OV2710_WriteBuf(ptInfo, _380C_HorzTotalHigh, 0x0B);
/* Version 2.0.0.2 modification, 2011.08.18 */
		OV2710_WriteBuf(ptInfo, _380D_HorzTotalLow, 0x58);
/* ======================================== */
		// end group 0
		OV2710_WriteBuf(ptInfo, _3212_GroupAccess, 0x10);
		// launch group 0
		OV2710_WriteBuf(ptInfo, _3212_GroupAccess, 0xA0);
/* Version 2.0.0.0 modification, 2011.01.21 */
		dwOV2710Hsync = OV2710_HSYNC_1920x1080_50Hz;
/* ======================================== */
	}
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
void OV2710_SetShutter(DWORD dwShutter)
{
	DWORD dwData;

/* Version 2.0.0.1 modification, 2011.04.20 */
/* Version 2.0.0.2 modification, 2011.08.18 */
	dwData = (dwShutter*16*OV2710_PIXEL_CLOCK_M) / dwOV2710Hsync;
/* ======================================== */
/* ======================================== */
	if (dwData > 0x000FFFFF)
	{
		dwData = 0x000FFFFF;
	}
/* Version 2.0.0.2 modification, 2011.08.18 */
	if (dwData < 160)
	{
		dwData = 160;
/* ======================================== */
	}
/* ======================================== */
	OV2710_UpdateBuf(ptInfo, _3500_AECExposureH, 0xF0, (BYTE)((dwData>>16)&0x0F)) ;
	OV2710_WriteBuf(ptInfo, _3501_AECExposureM, (BYTE)((dwData>>8)&0xFF)) ;
	OV2710_WriteBuf(ptInfo, _3502_AECExposureL, (BYTE)(dwData&0xFF)) ;
	
/* Version 2.0.0.0 modification, 2011.01.21 */
	dwData = (dwData/16/OV2710_VSYNC_1920x1080) * OV2710_VSYNC_1920x1080;
/* ======================================== */
	OV2710_WriteBuf(ptInfo, _350C_AECPKVTS, (BYTE)((dwData>>8)&0xFF)) ;
	OV2710_WriteBuf(ptInfo, _350D_AECPKVTS, (BYTE)(dwData&0xFF)) ;	
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
void OV2710_SetGain(DWORD dwGain)
{
	DWORD dwData;

/* Version 2.0.0.4 modification, 2012.08.06 */
	if (dwGain > OV2710_MAX_GAIN)
	{
		dwGain = OV2710_MAX_GAIN;
/* ======================================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

	if (dwGain < 2000)
	{
		dwData = (dwGain*16/1000) - 16;
	}
	else if (dwGain < 4000)
	{
		dwData = (dwGain*8/1000-16) + 16;
	}
	else if (dwGain < 8000)
	{
		dwData = (dwGain*4/1000-16) + 48;
	}
	else if (dwGain < 16000)
	{
		dwData = (dwGain*2/1000-16) + 112;
	}
	else if (dwGain < 32000)
	{
		dwData = (dwGain/1000-16) + 240;
	}
	else if (dwGain < 64000)
	{
		dwData = (dwGain/1000/2-16) + 496;
	}
	else
	{
		dwData = (dwGain/1000/4-16) + 511;
	}
/* ======================================== */
	OV2710_UpdateBuf(ptInfo, _350A_AECManualGainH, 0xFE, ((dwData>>8)&0x01));
	OV2710_WriteBuf(ptInfo, _350B_AECManualGainL, (dwData&0xFF));
}

/*==================================================================*/
void OV2710_GroupAccess(SOCKET sckStatus)
{
/* Version 1.1.0.2 modification, 2010.06.03 */	
	down_interruptible(&ov2710_mutex);
/* ========================================= */

	if (sckStatus == 0)
	{
		// initiate group access
		OV2710_WriteBuf(ptInfo, _3212_GroupAccess, 0x00);		// enable group 0
	}
	else { // sckStatus == 1
		// launch group access
		OV2710_WriteBuf(ptInfo, _3212_GroupAccess, 0x10);		// end group 0
		OV2710_WriteBuf(ptInfo, _3212_GroupAccess, 0xA0);		// launch group 0
	}
/* Version 1.1.0.2 modification, 2010.06.03 */	
	up(&ov2710_mutex);	
/* ========================================= */
}
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET  OV2710_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
/* ========================================= */
{
	SOCKET sckRet = 0;
	DWORD dwData, dwRgain, dwGgain, dwBgain;

/* Version 1.1.0.2 modification, 2010.06.03 */	
	if (down_interruptible(&ov2710_mutex))
	{
		return -ERESTARTSYS;	
	}
/* ========================================= */

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		OV2710_Reset(ptInfo);
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		OV2710_SetShutter(ptIoctlArg->adwUserData[0]);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		OV2710_SetGain(ptIoctlArg->adwUserData[0]);				
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			OV2710_UpdateBuf(ptInfo, _3A05_, 0x7F, 0x00);
			OV2710_UpdateBuf(ptInfo, _380C_HorzTotalHigh, 0xF0, 0x09);
			OV2710_WriteBuf(ptInfo, _380D_HorzTotalLow, 0x74);
			ptInfo->dwFreq	= VIDEO_SIGNAL_FREQUENCY_60HZ;
/* Version 2.0.0.0 modification, 2011.01.21 */
			dwOV2710Hsync = OV2710_HSYNC_1920x1080_60Hz;
/* ======================================== */
		}
		else 
		{
			OV2710_UpdateBuf(ptInfo, _3A05_, 0x7F, 0x80);
			OV2710_UpdateBuf(ptInfo, _380C_HorzTotalHigh, 0xF0, 0x0B);
/* Version 2.0.0.2 modification, 2011.08.18 */
			OV2710_WriteBuf(ptInfo, _380D_HorzTotalLow, 0x58);
/* ======================================== */
			ptInfo->dwFreq = VIDEO_SIGNAL_FREQUENCY_50HZ;
/* Version 2.0.0.0 modification, 2011.01.21 */
			dwOV2710Hsync = OV2710_HSYNC_1920x1080_50Hz;
/* ======================================== */
		}
/* Version 2.0.0.0 modification, 2011.01.21 */
/* Version 1.1.0.3 modification, 2010.08.18 */
/* ======================================== */
/* ========================================= */		
		break;
#if 0	// reserved for auto-mode	
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER:
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwData = (1104*30) / ptIoctlArg->adwUserData[0] * 16; // Vertical total size
			OV2710_UpdateBuf(ptInfo, _3A02_AECMaxExpo60H, 0xF0, (BYTE)((dwData>>16)&0x0F));
			OV2710_WriteBuf(ptInfo, _3A03_AECMaxExpo60M, (BYTE)((dwData>>8)&0xFF));
			OV2710_WriteBuf(ptInfo, _3A04_AECMaxExpo60L, (BYTE)(dwData&0xFF));
		}
		else 
		{
			dwData = (1104*25) / ptIoctlArg->adwUserData[0] * 16;
			OV2710_UpdateBuf(ptInfo, _3A14_AECMaxExpo50H, 0xF0, (BYTE)((dwData>>16)&0x0F));
			OV2710_WriteBuf(ptInfo, _3A15_AECMaxExpo50M, (BYTE)((dwData>>8)&0xFF));
			OV2710_WriteBuf(ptInfo, _3A16_AECMaxExpo50L, (BYTE)(dwData&0xFF));
		}		
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN:
		if (ptIoctlArg->adwUserData[0] == 2)
		{
			dwData = 0x01F;
		}
		else if (ptIoctlArg->adwUserData[0] == 4)
		{
			dwData = 0x03D;
		}
		else if (ptIoctlArg->adwUserData[0] == 8)
		{
			dwData = 0x07A;
		}
		else if (ptIoctlArg->adwUserData[0] == 16)
		{
			dwData = 0x0F4;
		}
		else if (ptIoctlArg->adwUserData[0] == 32)
		{
			dwData = 0x1E8;
		}
		else // (ptIoctlArg->adwUserData[0] == 64)
		{
			dwData = 0x3D0;
		}
		OV2710_UpdateBuf(ptInfo, _3A18_AECGainCeilH, 0xFC, (BYTE)((dwData>>8)&0x03));
		OV2710_WriteBuf(ptInfo, _3A19_AECGainCeilL, (BYTE)(dwData&0xFF));
		break;
		case VIDEO_SIGNAL_OPTION_SET_EXPOSURE_LEVEL:
		dwData = ptIoctlArg->adwUserData[0] + ptIoctlArg->adwUserData[1];
		dwData = (dwData>255) ? 255 : dwData;
		OV2710_WriteBuf(ptInfo, _3A1B_AECControl1B, (BYTE)dwData);
		dwData = ptIoctlArg->adwUserData[0] + ptIoctlArg->adwUserData[1]/3;
		dwData = (dwData>255) ? 255 : dwData;
		OV2710_WriteBuf(ptInfo, _3A0F_AECControl0F, (BYTE)dwData);
		dwData = (ptIoctlArg->adwUserData[0]<ptIoctlArg->adwUserData[1]/3)
			   ? 0
			   : (ptIoctlArg->adwUserData[0]-ptIoctlArg->adwUserData[1]/3);
		OV2710_WriteBuf(ptInfo, _3A10_AECControl10, (BYTE)dwData);
		dwData = (ptIoctlArg->adwUserData[0]<ptIoctlArg->adwUserData[1])
			   ? 0
			   : (ptIoctlArg->adwUserData[0]-ptIoctlArg->adwUserData[1]);
		OV2710_WriteBuf(ptInfo, _3A1E_AECControl1E, (BYTE)dwData);		
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER:
		if (ptIoctlArg->adwUserData[0] == 0)	// exposure can be less than 1/120 or 1/100 sec.
		{
			OV2710_UpdateBuf(ptInfo, _3A00_AECControl00, 0xEF, 0x10);
		}
		else 
		{
			OV2710_UpdateBuf(ptInfo, _3A00_AECControl00, 0xEF, 0x00);
		}		
		break;
#endif		
		case VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_EN:
		if (ptIoctlArg->adwUserData[0] == 1)
		{
			OV2710_UpdateBuf(ptInfo, _5000_ISPControl0, 0x7F, 0x80);
		}
		else
		{
			OV2710_UpdateBuf(ptInfo, _5000_ISPControl0, 0x7F, 0x00);
		}
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL:
/* Version 2.0.0.4 modification, 2012.08.06 */
		if (ptInfo->bOV2710DSPEnable)
		{
/* ======================================== */
			switch (ptIoctlArg->adwUserData[0])
			{
				case 0:	// auto WB
				OV2710_UpdateBuf(ptInfo, _3406_AWBManualCtrl, 0xFE, 0x00);
				break;
				case 1:	// manual WB
				// ptIoctlArg->adwUserData[1] = (R_gain<<16) + B_gain; (from the application)
				OV2710_UpdateBuf(ptInfo, _3406_AWBManualCtrl, 0xFE, 0x01);
	/* Version 1.1.0.4 modification, 2010.10.13 */	
				// ptIoctlArg->adwUserData[1] = (G_gain<<8) | R_gain; (from the application)
				// ptIoctlArg->adwUserData[2] = B_gain; (from the application)
				dwRgain = (ptIoctlArg->adwUserData[1] & 0xFF);
				dwBgain = (ptIoctlArg->adwUserData[2] & 0xFF);
				dwGgain = ((ptIoctlArg->adwUserData[1]&0xFF00) >> 8);
				//printk("[OV2710](%d) ptIoctlArg->adwUserData[1]=%08x \n",__LINE__,ptIoctlArg->adwUserData[1]);
				//printk("[OV2710](%d)dwRgain=%d,dwBgain=%d,dwGgain=%d \n",__LINE__,dwRgain,dwBgain,dwGgain);
					
				//The WB GAIN UNIT of VIC is 64. 
				dwRgain = dwRgain * (OV2710_WB_GAIN_UNIT/64);
				dwBgain = dwBgain * (OV2710_WB_GAIN_UNIT/64);
				dwGgain = dwGgain * (OV2710_WB_GAIN_UNIT/64);
				//printk("[OV2710](%d)dwRgain=%d,dwBgain=%d,dwGgain=%d\n",__LINE__,dwRgain,dwBgain,dwGgain);
	/* ========================================= */				
				dwData = (dwRgain >> 8);
				OV2710_WriteBuf(ptInfo, _3400_RedGainH, dwData);
				dwData = (dwRgain & 0x0FF);
				OV2710_WriteBuf(ptInfo, _3401_RedGainL, dwData);
				dwData = (dwGgain >> 8);
				OV2710_WriteBuf(ptInfo, _3402_GreenGainH, dwData);
				dwData = (dwGgain & 0x0FF);
				OV2710_WriteBuf(ptInfo, _3403_GreenGainL, dwData);
				dwData = (dwBgain >> 8);
				OV2710_WriteBuf(ptInfo, _3404_BlueGainH, dwData);
				dwData = (dwBgain & 0x0FF);
				OV2710_WriteBuf(ptInfo, _3405_BlueGainL, dwData);
				break;
				default: // auto track white, store current R/G/B gains and set to manual WB
				// ptIoctlArg->adwUserData[1] = (R_gain<<16) + B_gain; (to the application)
				// ptIoctlArg->adwUserData[2] = G_gain; (to the application)
				OV2710_ReadCompGain(ptInfo, _3400_RedGainH, _3401_RedGainL, &dwBgain);
				OV2710_ReadCompGain(ptInfo, _3402_GreenGainH, _3403_GreenGainL, &dwRgain);
				OV2710_ReadCompGain(ptInfo, _3404_BlueGainH, _3405_BlueGainL, &dwGgain);
	/* Version 1.1.0.4 modification, 2010.10.13 */					
				dwBgain = (dwBgain*64) / OV2710_WB_GAIN_UNIT;
				dwRgain = (dwRgain*64) / OV2710_WB_GAIN_UNIT;
				dwGgain = (dwGgain*64) / OV2710_WB_GAIN_UNIT;
					
				ptIoctlArg->adwUserData[1] = ((dwGgain<<8) | dwRgain);
				ptIoctlArg->adwUserData[2] = dwBgain;

				// set to manual WB
				OV2710_UpdateBuf(ptInfo, _3406_AWBManualCtrl, 0xFE, 0x01);
				break;
			}
/* Version 2.0.0.4 modification, 2012.08.06 */
		}
		sckRet = -1;
/* ========================================= */
		break;		
		default:
		sckRet = -EPERM;
	}
/* Version 1.1.0.2 modification, 2010.06.03 */	
	up(&ov2710_mutex);	
/* ========================================= */

	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
void OV2710_Release(DWORD dwDevNum)
/* ========================================= */
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
SOCKET OV2710_Open(TVideoSensorInitialParam* ptOV2710InitialParam, DWORD dwDevNum)
/* ========================================= */
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

/* Version 2.0.0.0 modification, 2011.01.21 */
	if (((ptOV2710InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(OV2710_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptOV2710InitialParam->dwVideoSensorVersion&0x0000FF00)>(OV2710_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid OV2710 device driver version %d.%d.%d.%d !!\n", 
				(int)(OV2710_INTERFACE_VERSION&0xFF), 
				(int)((OV2710_INTERFACE_VERSION>>8)&0xFF), 
				(int)((OV2710_INTERFACE_VERSION>>16)&0xFF), 
				(int)(OV2710_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}
/* ======================================== */

	if (!(ptInfo=(TOV2710Info *)kmalloc(sizeof(TOV2710Info), GFP_KERNEL)))
	{
		printk("[OV2710] : Allocate %d bytes memory fail\n", sizeof(TOV2710Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[OV2710] : Initial IICCtrl object fail !!\n");
/* Version 1.0.0.1 modification, 2010.06.01 */
		OV2710_Release(dwDevNum);
/* ========================================= */						
		return -ENODEV;
	}
	
	// init private data
	ptInfo->dwDeviceAddr = OV2710_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptOV2710InitialParam->dwVideoSensorFrequency;
/* Version 2.0.0.4 modification, 2012.08.06 */
	ptInfo->bOV2710DSPEnable = iEnAWB;
/* ======================================== */
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	// Detect device
	if (!OV2710_CompBuf(ptInfo, _300A_PIDH, 0xFF, ((OV2710_PID>>8)&0xFF)))
	{
		printk("[OV2710] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
/* Version 1.0.0.1 modification, 2010.06.01 */
		OV2710_Release(dwDevNum);	
/* ========================================= */			
		return -ENODEV;
	}
	if (!OV2710_CompBuf(ptInfo, _300B_PIDL, 0xFF, (OV2710_PID&0xFF))) 
	{
		printk("[OV2710] : device doesn't exist in device address 0x%02lX\n", ptInfo->dwDeviceAddr);
/* Version 1.0.0.1 modification, 2010.06.01 */
		OV2710_Release(dwDevNum);				
/* ========================================= */
		return -ENODEV;
	}
	// Reset device
	OV2710_Reset(ptInfo);
	return 0;
}

/* Version 2.0.0.4 modification, 2012.08.06 */
/*-------------------------------------------------------------------------------------*/
DWORD OV2710_GetMaxGain(void)
{
	return (OV2710_MAX_GAIN);
}
/* ========================================= */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	OV2710_Open,
	.release = 	OV2710_Release,
	.ioctl = 	OV2710_Ioctl,
/* Version 2.0.0.0 modification, 2011.01.21 */
/* ======================================== */
	.get_shutter_value = NULL,
/* Version 1.0.0.2 modification, 2010.06.03 */	
	.get_remaining_line_num = NULL,
/* ========================================= */
	.group_access = OV2710_GroupAccess,
/* Version 2.0.0.4 modification, 2012.08.06 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = OV2710_GetMaxGain,
/* ========================================= */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct OV2710_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t OV2710_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct OV2710_attribute *OV2710_attr = container_of(attr, struct OV2710_attribute, attr);
	int result = 0;
	
	if (OV2710_attr->show)
		result = OV2710_attr->show(kobj, buf);	
	return result;
}

ssize_t OV2710_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "OV2710");
	return retval;
}

ssize_t OV2710_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t OV2710_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}

ssize_t OV2710_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1080");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject OV2710_kobj;

struct OV2710_attribute OV2710_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV2710_SensorName_attr_show,
};

struct OV2710_attribute OV2710_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV2710_dwBufNum_attr_show,
};

struct OV2710_attribute OV2710_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV2710_MaxFrameWidth_attr_show,
};

struct OV2710_attribute OV2710_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = OV2710_MaxFrameHeight_attr_show,
};

struct sysfs_ops OV2710_sysfs_ops = {
	.show	= OV2710_default_attr_show,
};

struct kobj_type OV2710_ktype = {
	.sysfs_ops	= &OV2710_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET OV2710_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[OV2710] : kobj set name fail!\n");
		return -1;
	}

	OV2710_kobj.ktype = &OV2710_ktype;	
	sckResult = kobject_init_and_add(&OV2710_kobj, &OV2710_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[OV2710] : Cannot register kobject [OV2710_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV2710_kobj, &OV2710_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV2710] : Cannot create OV2710_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&OV2710_kobj, &OV2710_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV2710] : Cannot create OV2710_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV2710_kobj, &OV2710_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV2710] : Cannot create OV2710_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&OV2710_kobj, &OV2710_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[OV2710] : Cannot create OV2710_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
/* Version 1.1.0.2 modification, 2010.06.03 */	
	sema_init(&ov2710_mutex, 1);
/* ========================================= */
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void OV2710_Exit(void)
{
	sysfs_remove_file(&OV2710_kobj, &(OV2710_SensorName_attr.attr));
	sysfs_remove_file(&OV2710_kobj, &(OV2710_dwBufNum_attr.attr));	
/* Version 1.1.0.1 modification, 2010.06.21 */	
	sysfs_remove_file(&OV2710_kobj, &(OV2710_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&OV2710_kobj, &(OV2710_MaxFrameHeight_attr.attr));		
/* ========================== */				
	kobject_put(&OV2710_kobj);	
	printk(KERN_DEBUG"[OV2710] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(OV2710_Init);
module_exit(OV2710_Exit);
