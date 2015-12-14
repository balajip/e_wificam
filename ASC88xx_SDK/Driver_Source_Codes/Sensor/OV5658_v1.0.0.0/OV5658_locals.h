/*
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

#ifndef __OV5658_LOCALS_H__
#define __OV5658_LOCALS_H__
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <asm/arch/irq.h>
#include <asm/arch/platform.h>
#include <linux/mutex.h> // spinlock
#include "vivo_codec.h"
#include "video_error.h"


/* ============================================================================*/
#define OV5658_PID			0x5656
#define OV5658_REVISION	0xB0
#define OV5658_TBL_SIZE		(279)

#define OV5658_WB_GAIN_UNIT			1024

#define OV5658_DEFAULT_OUTPUT_WIDTH		2560
#define OV5658_DEFAULT_OUTPUT_HEIGHT	1920

#define OV5658_PRE_CHARGING_LINE	6

#define OV5658_MAX_GAIN 15938

#define OV5658_PIXEL_CLOCK			192000000
#define OV5658_PIXEL_CLOCK_M		(OV5658_PIXEL_CLOCK/1000000)

#define OV5658_HSYNC_2560x1920_30FPS	3225
#define OV5658_HSYNC_2560x1920_25FPS	3870
#define OV5658_VSYNC_2560x1920		1984

#define OV5658_HSYNC_2432x1728_30FPS	3225
#define OV5658_HSYNC_2432x1728_25FPS	3870
#define OV5658_VSYNC_2432x1728		1984

typedef enum OV5658_registers
{
	_0100_ModeSelect = 0x0100,
	_0102_FastStandby = 0x0102,
	_0103_SWReset = 0x0103,
	_3001_SCPadCtrl = 0x3001,
	_3002_SCPadOEN0 = 0x3002,
	_3006_SCPadOut2 = 0x3006,
	_300A_SCChipID1 = 0x300A,
	_300B_SCChipID0 = 0x300B,
	_300C_SCSCCBID = 0x300C,
	_3011_SCMipiScCtrl0 = 0x3011,
	_3012_SCMipiPhy = 0x3012,
	_3013_SCMipiPhy = 0x3013,
	_3014_SCMipiScCtrl1 = 0x3014,
	_3015_SCMipiScCtrl2 = 0x3015,
	_3016_SCClkRst0 = 0x3016,
	_3017_SCClkRst1 = 0x3017,
	_3018_SCClkRst2 = 0x3018,
	_3019_SCClkRst3 = 0x3019,
	_301A_SCClkRst4 = 0x301A,
	_301B_SCClkRst5 = 0x301B,
	_301D_SCFrexRstMask0 = 0x301D,
	_301E_SCClockSel = 0x301E,
	_3021_SCAPwcPK = 0x3021,
	_3022_SCAPwcPK = 0x3022,
	_3023_SCLowPwrCtr = 0x3023,
	_3024_SCPadSel2 = 0x3024,
	_3025_SCOtpRevID = 0x3025,
	_3028_SCPumpClkCtrl = 0x3028,
	_302A_SCChipRevision = 0x302A,
	_303D_SCGPIoIN0 = 0x303D,
	_303E_SCGPIoIN1 = 0x303E,
	_3100_SBSccbCtrl = 0x3100,
	_3101_SBSccbOpt = 0x3101,
	_3102_SBSccbFilter = 0x3102,
	_3103_SBSccbSysReg = 0x3103,
	_3104_SBPwupDis = 0x3104,
	_3105_SBPadClkDiv = 0x3105,
	_3106_SBSrbHostInputDis = 0x3106,
	_3200_GroupAdr0 = 0x3200,
	_3201_GroupAdr1 = 0x3201,
	_3202_GroupAdr2 = 0x3202,
	_3203_GroupAdr3 = 0x3203,
	_3204_GroupLen0 = 0x3204,
	_3205_GroupLen1 = 0x3205,
	_3206_GroupLen2 = 0x3206,
	_3207_GroupLen3 = 0x3207,
	_3208_GroupAccess = 0x3208,
	_3209_Group0Period = 0x3209,
	_320A_Group1Period = 0x320A,
	_320B_GrpSwCtrl = 0x320B,
	_320C_SramTest = 0x320C,
	_320D_GrpAct = 0x320D,
	_320E_FmCntGrp0 = 0x320E,
	_320F_FmCntGrp1 = 0x320F,
	_3210_LcRegCtrl = 0x3210,
	_3211_LcRegDebug = 0x3211,
	_3212_LcRegCtrl = 0x3212,
	_3400_AwbRedGain1 = 0x3400,
	_3401_AwbRedGain0 = 0x3401,
	_3402_AwbGrnGain1 = 0x3402,
	_3403_AwbGrnGain0 = 0x3403,
	_3404_AwbBluGain1 = 0x3404,
	_3405_AwbBluGain0 = 0x3405,
	_3406_AwbManualCtrl = 0x3406,
	_3500_AECLongExp2 = 0x3500,
	_3501_AECLongExp1 = 0x3501,
	_3502_AECLongExp0 = 0x3502,
	_3503_AECManual = 0x3503,
	_3504_ManualSNRGain1 = 0x3504,
	_3505_ManualSNRGain0 = 0x3505,
	_3506_AECShortExp2 = 0x3506,
	_3507_AECShortExp1 = 0x3507,
	_3508_AECShortExp0 = 0x3508,
	_3509_AECGainConvert = 0x3509,
	_350A_AECAgcAdj1 = 0x350A,
	_350B_AECAgcAdj0 = 0x350B,
	_3600_PLL10 = 0x3600,
	_3601_PLL11 = 0x3601,
	_3602_PLL12 = 0x3602,
	_3603_PLL13 = 0x3603,
	_3604_PLL20 = 0x3604,
	_3605_PLL21 = 0x3605,
	_3606_PLL22 = 0x3606,
	_3667_AnaCore7 = 0x3667,
	_37C5_SensorFrexExp2 = 0x37C5,
	_37C6_SensorFrexExp1 = 0x37C6,
	_37C7_SensorFrexExp0 = 0x37C7,
	_37C9_SensorStrobWidth2 = 0x37C9,
	_37CA_SensorStrobWidth1 = 0x37CA,
	_37CB_SensorStrobWidth0 = 0x37CB,
	_37CC_SensorShutterDly2 = 0x37CC,
	_37CD_SensorShutterDly1 = 0x37CD,
	_37CE_SensorFrexPchgWidth1 = 0x37CE,
	_37CF_SensorFrexPchgWidth0 = 0x37CF,
	_37D0_SensorDatoutDly1 = 0x37D0,
	_37D1_SensorDatoutDly0 = 0x37D1,
	_37D2_SensorStrobDly1 = 0x37D2,
	_37D3_SensorStrobDly0 = 0x37D3,
	_37DE_SensorFrex1E = 0x37DE,
	_37DF_SensorFrexReq = 0x37DF,
	_3800_TimingXAddrStart1 = 0x3800,
	_3801_TimingXAddrStart0 = 0x3801,
	_3802_TimingYAddrStart1 = 0x3802,
	_3803_TimingYAddrStart0 = 0x3803,
	_3804_TimingXAddrEnd1 = 0x3804,
	_3805_TimingXAddrEnd0 = 0x3805,
	_3806_TimingYAddrEnd1 = 0x3806,
	_3807_TimingYAddrEnd0 = 0x3807,
	_3808_TimingXOutputSize1 = 0x3808,
	_3809_TimingXOutputSize0 = 0x3809,
	_380A_TimingYOutputSize1 = 0x380A,
	_380B_TimingYOutputSize0 = 0x380B,
	_380C_TimingHTS1 = 0x380C,
	_380D_TimingHTS0 = 0x380D,
	_380E_TimingVTS1 = 0x380E,
	_380F_TimingVTS0 = 0x380F,
	_3810_TimingIspXWin1 = 0x3810,
	_3811_TimingIspXWin0 = 0x3811,
	_3812_TimingIspYWin1 = 0x3812,
	_3813_TimingIspYWin0 = 0x3813,
	_3814_TimingXInc = 0x3814,
	_3815_TimingYInc = 0x3815,
	_3816_TimingHsyncStart1 = 0x3816,
	_3817_TimingHsyncStart0 = 0x3817,
	_3818_TimingHsyncEnd1 = 0x3818,
	_3819_TimingHsyncEnd0 = 0x3819,
	_381A_TimingHsyncFirst1 = 0x381A,
	_381B_TimingHsyncFirst0 = 0x381B,
	_3820_TimingFormat1 = 0x3820,
	_3821_TimingFormat2 = 0x3821,
	_3B00_StrobCtrl = 0x3B00,
	_3B02_StrobDelayLine1 = 0x3B02,
	_3B03_StrobDelayLine0 = 0x3B03,
	_3B04_StrobCtrl = 0x3B04,
	_3B05_StrobWidth = 0x3B05,
	_3D80_OtpProgramCtrl = 0x3D80,
	_3D81_OptLoadCtrl = 0x3D81,
	_3D82_OtpProgramPulse = 0x3D82,
	_3D83_OtpLoadPulse = 0x3D83,
	_3D84_OptModeCtrl = 0x3D84,
	_3D85_OptStartAddr = 0x3D85,
	_3D86_OptEndAddr = 0x3D86,
	_3D87_OptPs2cs = 0x3D87,
	_4000_BlcBypass = 0x4000,
	_4001_BlcStartLine = 0x4001,
	_4002_BlcAuto = 0x4002,
	_4003_BlcFreeze = 0x4003,
	_4004_BlcBlcNum = 0x4004,
	_4005_BlcManualCtrl = 0x4005,
	_4006_ZLineCoef = 0x4006,
	_4007_BlcWin = 0x4007,
	_4008_BlcFlipReg = 0x4008,
	_4009_BlcTarget = 0x4009,
	_400C_BlcManualLevel01 = 0x400C,
	_400D_BlcManualLevel00 = 0x400D,
	_400E_BlcManualLevel11 = 0x400E,
	_400F_BlcManualLevel10 = 0x400F,
	_4010_BlcManualLevel21 = 0x4010,
	_4011_BlcManualLevel20 = 0x4011,
	_4012_BlcManualLevel31 = 0x4012,
	_4013_BlcManualLevel30 = 0x4013,
	_4014_ShortBlcManualLevel01 = 0x4014,
	_4015_ShortBlcManualLevel00 = 0x4015,
	_4016_ShortBlcManualLevel11 = 0x4016,
	_4017_ShortBlcManualLevel10 = 0x4017,
	_4018_ShortBlcManualLevel21 = 0x4018,
	_4019_ShortBlcManualLevel20 = 0x4019,
	_401A_ShortBlcManualLevel31 = 0x401A,
	_401B_ShortBlcManualLevel30 = 0x401B,
	_404E_BlcBlcMax = 0x404E,
	_404F_BlcStableRange = 0x404F,
	_4200_FrameCtrl0 = 0x4200,
	_4201_FrameOnNum = 0x4201,
	_4202_FrameOffNum = 0x4202,
	_4203_FrameCtrl = 0x4203,
	_4300_DataClipMax1 = 0x4300,
	_4301_DataClipMin1 = 0x4301,
	_4302_DataClip0 = 0x4302,
	_4303_FormatCtrl3 = 0x4303,
	_4304_FormatCtrl4 = 0x4304,
	_4305_PadLow1 = 0x4305,
	_4306_PadLow2 = 0x4306,
	_4307_EmbedCtrl = 0x4307,
	_4308_TstXStart1 = 0x4308,
	_4309_TstXStart0 = 0x4309,
	_430A_TstYStart1 = 0x430A,
	_430B_TstYStart0 = 0x430B,
	_430C_TstWidth1 = 0x430C,
	_430D_TstWidth0 = 0x430D,
	_430E_TstHeight1 = 0x430E,
	_430F_TstHeight0 = 0x430F,
	_4311_VsyncWidth1 = 0x4311,
	_4312_VsyncWidth0 = 0x4312,
	_4313_VsyncCtrl = 0x4313,
	_4314_VsyncDelay2 = 0x4314,
	_4315_VsyncDelay1 = 0x4315,
	_4316_VsyncDelay0 = 0x4316,
	_4320_TestCtrl0 = 0x4320,
	_4322_TestDataB1 = 0x4322,
	_4323_TestDataB0 = 0x4323,
	_4324_TestDataGb1 = 0x4324,
	_4325_TestDataGb0 = 0x4325,
	_4326_TestDataR1 = 0x4326,
	_4327_TestDataR0 = 0x4327,
	_4328_TestDataGr1 = 0x4328,
	_4329_TestDataGr0 = 0x4329,
	_4501_InputSwapManEn = 0x4501,
	_4600_VFifoR0 = 0x4600,
	_4800_MipiCtrl00 = 0x4800,
	_4801_MipiCtrl01 = 0x4801,
	_4802_MipiCtrl02 = 0x4802,
	_4803_MipiCtrl03 = 0x4803,
	_4804_MipiCtrl04 = 0x4804,
	_4805_MipiCtrl05 = 0x4805,
	_4806_MipiRegRwCtrl = 0x4806,
	_4810_MipiMaxFrameCnt1 = 0x4810,
	_4811_MipiMaxFrameCnt0 = 0x4811,
	_4813_MipiCtrl13 = 0x4813,
	_4814_MipiCtrl14 = 0x4814,
	_4815_MipiDTSpkt = 0x4815,
	_4816_MipiDTSel = 0x4816,
	_4818_HsZeroMin1 = 0x4818,
	_4819_HsZeroMin0 = 0x4819,
	_481A_HsTrailMin1 = 0x481A,
	_481B_HsTrailMin0 = 0x481B,
	_481C_ClkZeroMin1 = 0x481C,
	_481D_ClkZeroMin0 = 0x481D,
	_481E_ClkPrepareMax = 0x481E,
	_481F_ClkPrepareMin = 0x481F,
	_4820_ClkPostMin1 = 0x4820,
	_4821_ClkPostMin0 = 0x4821,
	_4822_ClkTrailMin1 = 0x4822,
	_4823_ClkTrailMin0 = 0x4823,
	_4824_LpxPMin1 = 0x4824,
	_4825_LpxPMin0 = 0x4825,
	_4826_HsPrepareMin = 0x4826,
	_4827_HsPrepareMax = 0x4827,
	_4828_HsExitMin1 = 0x4828,
	_4829_HsExitMin0 = 0x4829,
	_482A_UIHsZeroMin = 0x482A,
	_482B_UIHsTrailMin = 0x482B,
	_482C_UIClckZeroMin = 0x482C,
	_482D_UIClckPrepare = 0x482D,
	_482E_UIClckPostMin = 0x482E,
	_482F_UIClckTrailMin = 0x482F,
	_4830_UILpxPMin = 0x4830,
	_4831_UIHsPrepareMin = 0x4831,
	_4832_UIHsExitMin = 0x4832,
	_4836_GlbModeSel = 0x4836,
	_4837_PclkPeriod = 0x4837,
	_4838_MipiLpGpio0 = 0x4838,
	_4839_MipiLpGpio1 = 0x4839,
	_483A_MipiLpGpio2 = 0x483A,
	_483B_MipiLpGpio3 = 0x483B,
	_483C_MipiCtrl3C = 0x483C,
	_483D_MipiLpGpio4 = 0x483D,
	_4840_StartOffset1 = 0x4840,
	_4841_StartOffset0 = 0x4841,
	_4842_StartMode = 0x4842,
	_4846_SelMipiCtrl46 = 0x4846,
	_4847_SelMipiCtrl47 = 0x4847,
	_4848_SelMipiCtrl48 = 0x4848,
	_4849_SelMipiCtrl49 = 0x4849,
	_484A_SelMipiCtrl4A = 0x484A,
	_484B_SelMipiCtrl4B = 0x484B,
	_484C_SelMipiCtrl4C = 0x484C,
	_484D_TestPatternData = 0x484D,
	_484E_FEDly = 0x484E,
	_484F_TestPatternCkData = 0x484F,
	_4864_MipiLcnt1 = 0x4864,
	_4865_MipiLcnt0 = 0x4865,
	_4866_TGlbTim1 = 0x4866,
	_4867_TGlbTim0 = 0x4867,
	_5000_IspCtrl0 = 0x5000,
	_5001_IspCtrl1 = 0x5001,
	_5002_IspCtrl2 = 0x5002,
	_5005_IspBiasCtrl = 0x5005,
	_5012_IspCtrl12 = 0x5012,
	_5013_IspCtrl13 = 0x5013,
	_5016_IspCtrl16 = 0x5016,
	_5017_IspCtrl17 = 0x5017,
	_501F_IspBypass = 0x501F,
	_5025_IspAvgSel = 0x5025,
	_5041_IspCtrl41 = 0x5041,
	_5046_IspSofSel = 0x5046,
	_5068_ScaleHInvt = 0x5068,
	_506A_ScaleVInvt = 0x506A,
	_5600_ScaleHFactor1 = 0x5600,
	_5601_ScaleHFactor0 = 0x5601,
	_5602_ScaleVFactor1 = 0x5602,
	_5603_ScaleVFactor0 = 0x5603,
	_5604_ScaleAuto = 0x5604,
	_5605_ReadScaleHFactor1 = 0x5605,
	_5606_ReadScaleHFactor0 = 0x5606,
	_5607_ReadScaleVFactor1 = 0x5607,
	_5608_ReadScaleVFactor0 = 0x5608,
	_5680_AvgXStart1 = 0x5680,
	_5681_AvgXStart0 = 0x5681,
	_5682_AvgYStart1 = 0x5682,
	_5683_AvgYStart0 = 0x5683,
	_5684_AvgWinWidth1 = 0x5684,
	_5685_AvgWinWidth0 = 0x5685,
	_5686_AvgWinWidth1 = 0x5686,
	_5687_AvgWinHeight0 = 0x5687,
	_5688_AvgManualCtrl = 0x5688,
	_5800_LencG00 = 0x5800,
	_5801_LencG01 = 0x5801,
	_5802_LencG02 = 0x5802,
	_5803_LencG03 = 0x5803,
	_5804_LencG04 = 0x5804,
	_5805_LencG05 = 0x5805,
	_5806_LencG10 = 0x5806,
	_5807_LencG11 = 0x5807,
	_5808_LencG12 = 0x5808,
	_5823_LencG55 = 0x5823,
	_5824_LencBR00 = 0x5824,
	_5825_LencBR01 = 0x5825,
	_5826_LencBR02 = 0x5826,
	_5827_LencBR03 = 0x5827,
	_5828_LencBR04 = 0x5828,
	_583D_LencBROffset = 0x583D,
	_583E_LencSenGainThrld1 = 0x583E,
	_583F_LencSenGainThrld2 = 0x583F,
	_5840_MinLencGain = 0x5840,
	_5841_LencCtrl = 0x5841,
	_5842_LencBRHScale1 = 0x5842,
	_5843_LencBRHScale0 = 0x5843,
	_5844_LencBRVScale1 = 0x5844,
	_5845_LencBRVScale0 = 0x5845,
	_5846_LencGHScale1 = 0x5846,
	_5847_LencGHScale0 = 0x5847,
	_5848_LencGVScale1 = 0x5848,
	_5849_LencGVScale0 = 0x5849,
	_5A00_WinXStartOffset1 = 0x5A00,
	_5A01_WinXStartOffset0 = 0x5A01,
	_5A02_WinYStartOffset1 = 0x5A02,
	_5A03_WinYStartOffset0 = 0x5A03,
	_5A04_WinWidth1 = 0x5A04,
	_5A05_WinWidth0 = 0x5A05,
	_5A06_WinHeight1 = 0x5A06,
	_5A07_WinHeight0 = 0x5A07,
	_5A08_WinManual = 0x5A08,
	_5A09_WinPixelCnt1 = 0x5A09,
	_5A0A_WinPixelCnt0 = 0x5A0A,
	_5A0B_WinLineCnt1 = 0x5A0B,
	_5A0C_WinLineCnt0 = 0x5A0C,
	_5D00_GainFmtCrl00 = 0x5D00,
	_5D01_GainFmtCrl01 = 0x5D01,
	_5E00_PreIspTestCtrl = 0x5E00,
	_5E01_PreIspWin = 0x5E01,
} EOV5658Regs;

typedef struct OV5658_reg_addr_data
{
    EOV5658Regs eRegAddr;
    BYTE byData;
} TOV5658RegAddrData;

typedef struct OV5658_info
{
	spinlock_t lock;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnReadBuf pfnReadBuf;
	FOnWriteBuf pfnWriteBuf;

	DWORD dwWidth;
	DWORD dwHeight;

	DWORD dwFreq;
	
	BOOL bEnOV5658AWB;
}TOV5658Info;

