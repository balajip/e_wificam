/*
 * Mozart_Audio_IT6604
 * Driver for Mozart IT6604 Audio Codec Driver
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
 */

#ifndef __IT6604_H__
#define __IT6604_H__

#define IT6604_ADDR			0x90

#define IT6604_REG_VID_L		0x00
#define	IT6604_REG_VID_H		0x01
#define IT6604_REG_DEVID_L		0x02
#define IT6604_REG_DEVID_H		0x03
#define	IT6604_REG_DEVREV		0x04
#define	IT6604_REG_RESET		0x05
#define	IT6604_REG_AIOCTRL		0x09
#define IT6604_REG_INTERRUPT3	0x15
#define IT6604_REG_INTMASK3	0x18
#define	IT6604_REG_INTCLEAR	0x19
#define IT6604_REG_I2SCTRL		0x75
#define	IT6604_REG_I2SMAP		0x76
#define	IT6604_REG_AUDIOCTRL	0x77
#define	IT6604_REG_MCLKCTRL	0x78
#define	IT6604_REG_FSSET		0x7E
#define	IT6604_REG_HWMUTECTRL	0x87
#define IT6604_REG_TRISTATECTRL	0x89

#define IT6604_REG_NUMBER		256

#define HDMI_RX_I2C_SLAVE_ADDR 0x90

#define REG_VID_L	0x00
#define REG_VID_H	0x01
#define REG_DEVID_L	0x02
#define REG_DEVID_H	0x03
#define REG_DEVREV	0x04
#define REG_RST_CTRL	0x05

#define B_ENAUTOHDCPRST (1<<7)
#define B_EN_CDRAUTORST (1<<6)
#define B_EN_AUTOVDORST (1<<5)
#define B_REGRST        (1<<4)
#define B_HDCPRST       (1<<3)
#define B_AUDRST        (1<<2)
#define B_VDORST        (1<<1)
#define B_SWRST         (1<<0)

#define REG_PWD_CTRL0	0x06
#define REG_PWD_CTRL1	0x07

#define B_PWD_AFEALL	(1<<3)
#define B_PWDC_ETC	(1<<2)
#define B_PWDC_SRV	(1<<1)
#define B_EN_AUTOPWD	(1<<0)




#define REG_VIO_CTRL	0x08
#define REG_AIO_CTRL	0x09
#define REG_OTP_CTRL	0x0A
#define REG_OTP_XOR	0x0B
#define REG_BIST_CTRL	0x0C
#define REG_BIST_RESULT1	0x0D
#define REG_BIST_RESULT2	0x0E
//#define  REG_RSVD	0x0F
#define REG_SYS_STATE	0x10

#define B_RXPLL_LOCK (1<<7)
#define B_RXCK_SPEED (1<<6)
#define B_RXCK_VALID (1<<5)
#define B_HDMI_MODE (1<<4)
#define B_VSYNC (1<<3)
#define B_SCDT (1<<2)
#define B_VCLK_DET (1<<1)
#define B_PWR5V_DET (1<<0)


#define REG_HDCP_CTRL	0x11
#define REG_HDCP_STATUS	0x12
#define REG_INTERRUPT1	0x13

#define B_VIDMODE_CHG   (1<<5)
#define B_HDMIMODE_CHG  (1<<4)
#define B_SCDTOFF       (1<<3)
#define B_SCDTON        (1<<2)
#define B_PWR5VOFF      (1<<1)
#define B_PWR5VON       (1<<0)

#define REG_INTERRUPT2	0x14
#define B_PKT_CLR_MUTE (1<<7)
#define B_NEW_AUD_PKT (1<<6)
#define B_NEW_ACP_PKT (1<<5)
#define B_NEW_SPD_PKT (1<<4)
#define B_NEW_MPG_PKG (1<<3)
#define B_NEW_AVI_PKG (1<<2)
#define B_NO_AVI_RCV (1<<1)
#define B_PKT_SET_MUTE (1<<0)

#define REG_INTERRUPT3	0x15
#define B_AUTOAUDMUTE (1<<4)
#define B_AUDFIFOERR (1<<3)
#define B_ECCERR (1<<2)
#define B_AUTH_DONE (1<<1)
#define B_AUTH_START (1<<0)

#define REG_INTERRUPT_MASK1	0x16
#define REG_INTERRUPT_MASK2	0x17
#define REG_INTERRUPT_MASK3	0x18
#define REG_INTERRUPT_CTRL	0x19

#define	B_CLR_MODE_INT	(1<<0)
#define	B_CLR_PKT_INT	(1<<1)
#define	B_CLR_HDCP_INT	(1<<2)
#define	B_CLR_AUDIO_INT	(1<<3)
#define	B_INTPOL	    (1<<4)
#define	B_INTROUTTYPE	(1<<5)

#define LO_ACTIVE TRUE
#define HI_ACTIVE FALSE

#define REG_MISC_CTRL	0x1A

#define B_RXCK_FILT (1<<5)
#define B_TIMEOUT_EN (1<<4)
#define B_EN_DEBUG (1<<3)
#define B_DE_POL (1<<2)
#define B_VSYNC_OUT_POL (1<<1)
#define B_HSYNC_OUT_POL (1<<0)


#define REG_VIDEO_MAP	0x1B
#define B_OUTPUT_16BIT (1<<5)
#define B_SWAP_CH422 (1<<4)
#define B_SWAP_OUTRB (1<<3)
#define B_SWAP_ML (1<<2)
#define B_SWAP_POL (1<<1)
#define B_SWAP_RB (1<<0)

#define REG_VIDEO_CTRL1	0x1C
#define B_DNFREE_GO (1<<7)
#define B_SYNC_EMBEDDED (1<<6)
#define B_EN_DITHER (1<<5)
#define B_EN_UDFILTER (1<<4)
#define B_DDR_OUTPUT (1<<3) // used for 12bit output of RGB24 or YUV444
#define B_CCIR565    (1<<2)
#define B_565FFRST   (1<<1)
#define B_EN_AVMUTE_RST (1<<0)

#define REG_VCLK_CTRL	0x1D
#define REG_I2CIO_CTRL	0x1E
#define REG_REGPKTFLAG_CTRL	0x1F
#define B_INT_EVERYAVI	1

#define REG_CSC_CTRL	0x20

#define B_FORCE_COLOR_MODE (1<<4)
#define O_INPUT_COLOR_MODE 2
#define M_INPUT_COLOR_MASK 3
#define B_INPUT_RGB24      0
#define B_INPUT_YUV422     1
#define B_INPUT_YUV444     2

#define O_CSC_SEL          0
#define M_CSC_SEL_MASK     3
#define B_CSC_BYPASS       0
#define B_CSC_RGB2YUV      2
#define B_CSC_YUV2RGB      3

#define REG_CSC_YOFF	0x21
#define REG_CSC_COFF	0x22
#define REG_CSC_RGBOFF	0x23
#define REG_CSC_MTX11_L	0x24
#define REG_CSC_MTX11_H	0x25
#define REG_CSC_MTX12_L	0x26
#define REG_CSC_MTX12_H	0x27
#define REG_CSC_MTX13_L	0x28
#define REG_CSC_MTX13_H	0x29
#define REG_CSC_MTX21_L	0x2A
#define REG_CSC_MTX21_H	0x2B
#define REG_CSC_MTX22_L	0x2C
#define REG_CSC_MTX22_H	0x2D
#define REG_CSC_MTX23_L	0x2E
#define REG_CSC_MTX23_H	0x2F
#define REG_CSC_MTX31_L	0x30
#define REG_CSC_MTX31_H	0x31
#define REG_CSC_MTX32_L	0x32
#define REG_CSC_MTX32_H	0x33
#define REG_CSC_MTX33_L	0x34
#define REG_CSC_MTX33_H	0x35
#define REG_CSC_GAIN1V_L	0x36
#define REG_CSC_GAIN1V_H	0x37
#define REG_CSC_GAIN2V_L	0x38
#define REG_CSC_GAIN2V_H	0x39
#define REG_CSC_GAIN3V_L	0x3A
#define REG_CSC_GAIN3V_H	0x3B
#define REG_PG_CTRL1	0x3C
#define REG_PG_CTRL2	0x3D
// output color mode is located this register D[7:6]
#define O_OUTPUT_COLOR_MODE 6
#define M_OUTPUT_COLOR_MASK 3
#define B_OUTPUT_RGB24      0
#define B_OUTPUT_YUV422     1
#define B_OUTPUT_YUV444     2

#define REG_PG_COLR	0x3E
#define REG_PG_COLG	0x3F
#define REG_PG_COLB	0x40
#define REG_PG_COLBLANK	0x41
#define REG_PG_COLBLANKY	0x42
#define REG_PG_HACTST_L	0x43
#define REG_PG_HACTST_H	0x44
#define REG_PG_HACTED_H	0x45
#define REG_PG_VACTST_L	0x46
#define REG_PG_VACTST_H	0x47
#define REG_PG_VACTED_H	0x48
#define REG_PG_VACTST2ND_L	0x49
#define REG_PG_VACTST2ND_H	0x4A
#define REG_PG_VACTED2ND_H	0x4B
#define REG_PG_HTOTAL_L	0x4C
#define REG_PG_HTOTAL_H	0x4D
#define REG_PG_HSYNCST_H	0x4E
#define REG_PG_HSYNCED_L	0x4F
#define REG_PG_VTOTAL_L	0x50
#define REG_PG_VTOTAL_H	0x51
#define REG_PG_VSYNCST_H	0x52
#define REG_PG_VSYNCED	0x53
#define REG_PG_CHINC	0x54
#define REG_PG_CVINC	0x55
//#define  REG_RSV	0x56

#define REG_I2C_CTRL    0x57
#define B_FILT_TAP      (1<<5)
#define B_FILT_TYPE     (1<<4)
#define B_DEGLITCH      (1<<3)
#define B_CMD_FILTTAP   (1<<2)
#define B_CMD_FILTTYPE  (1<<1)
#define B_CMD_DEGLITCH  (1<<0)

#define REG_VID_MODE    0x58
#define B_PX_VIDEOSTABLE (1<<3)
#define B_VIDFIELD       (1<<2)
#define B_INTERLACE      (1<<1)
// #define B_VIDMODE_CHG    (1<<0)

#define REG_VID_HTOTAL_L     0x59
#define REG_VID_HTOTAL_H     0x5A
// D[6:4] - HActive [10:8]
// D[3:0] - HTotal  [11:8]
#define O_HTOTAL_H 0
#define M_HTOTAL_H 0xF
#define O_HACT_H   4
#define M_HACT_H   0x7

#define REG_VID_HACT_L       0x5B

#define REG_VID_HSYNC_WID_L  0x5C
#define REG_VID_HSYNC_WID_H  0x5D
// D[0] - HSYNC_Width[8]
// D[7:4] HFrontPorch[11:8]
#define O_HSYNC_WID_H 0
#define M_HSYNC_WID_H 1
#define O_H_FT_PORCH 4
#define M_H_FT_PORCH 0xF

#define REG_VID_H_FT_PORCH_L 0x5E
#define REG_VID_VTOTAL_L     0x5F
#define REG_VID_VTOTAL_H     0x60
#define O_VTOTAL_H 0
#define M_VTOTAL_H 0xF
#define O_VACT_H   4
#define M_VACT_H   0x7

#define REG_VID_VACT_L       0x61
#define REG_VID_VSYNC2DE     0x62
#define REG_VID_V_FT_PORCH   0x63
#define REG_VID_XTALCNT_128PEL 0x64
#define REG_VID_INPUT_ST     0x65

#define O_PIXEL_REPEAT 4
#define M_PIXEL_REPEAT 0xF
#define B_AVMUTE (1<<2)
#define B_VSYNC_IN_POL (1<<1)
#define B_HSYNC_IN_POL (1<<0)

//#define  REG_RSV	0x66
#define REG_DEBUG_CTRL	0x67
#define REG_PLL_CTRL	0x68
#define REG_TERM_CTRL1	0x69
#define REG_TERM_CTRL2	0x6A
//#define  REG_RSV	0x6B
#define REG_EQUAL_CTRL2	0x6C
#define REG_DES_CTRL1	0x6D
#define REG_DES_CTRL2	0x6E
#define REG_DES_PHASE_B	0x6F
#define REG_DES_PHASE_G	0x70
#define REG_DES_PHASE_R	0x71
#define REG_BCH_MODE	0x72
#define O_BCH_THRSH 3
#define M_BCH_THRSH ((1<<(6-3))-1)

#define B_TERC_INV (1<<2)
#define B_TERC_CHECK (1<<1)
#define B_DIS_BCH (1<<0)

//#define  REG_RSV	0x73
#define REG_APLL_CTRL	0x74
#define B_APLL_ENI2A (1<<7)
#define B_APLL_EC1 (1<<6)
#define B_APLL_ENIA (1<<5)
#define B_RSVED (1<<4)
#define B_DIEN (1<<3)
#define B_DIS_SDM (1<<2)
#define B_APLL_REFSW (1<<1)
#define B_APLL_AUTOSW (1<<0)

#define REG_I2S_CTRL	0x75

#define REG_I2S_MAP	0x76
#define O_I2S_CH3SEL 6
#define O_I2S_CH2SEL 4
#define O_I2S_CH1SEL 2
#define O_I2S_CH0SEL 0

#define M_I2S_CHSEL 3
#define B_I2S_CHSEL0 0 // map channel n to output select 0
#define B_I2S_CHSEL1 1 // map channel n to output select 1
#define B_I2S_CHSEL2 2 // map channel n to output select 2
#define B_I2S_CHSEL3 3 // map channel n to output select 3

#define REG_AUDIO_CTRL	0x77
#define B_DIS_FALSE_DE (1<<5)
#define B_AUD_INFO_FORCE (1<<4)
#define B_AVMUTE_VALUE (1<<3)
#define B_FORCE_AVMUTE (1<<2)
#define B_DIS_VDO_MUTE (1<<1)
#define B_DIS_AUD_MUTE (1<<0)

#define REG_MCLK_CTRL	0x78
#define B_OSC_EN (1<<7)
#define B_OSCSEL (1<<6)
#define B_FORCE_FS (1<<5)
#define B_FORCE_CTS (1<<4)
#define B_FORCE_CTSMODE (1<<3)
#define O_MCLKSEL 0
#define M_MCLKSEL 0x07
#define B_128FS 0
#define B_256FS 1
#define B_512FS 3
#define B_1024FS 7

#define REG_N_SET1	0x79
#define REG_N_SET2	0x7A
#define REG_N_SET3	0x7B
#define REG_CTS_SET2	0x7C
#define REG_CTS_SET3	0x7D
#define REG_FS_SET	0x7E
#define REG_N_RCV1	0x7F
#define REG_N_RCV2	0x80
#define REG_N_RCV3	0x81
#define REG_CTS_RCV2	0x82
#define REG_CTS_RCV3	0x83
#define REG_FS	0x84
    #define B_Fs_44p1KHz    0
    #define B_Fs_48KHz  2
    #define B_Fs_32KHz  3
    #define B_Fs_88p2KHz    8
    #define B_Fs_96KHz  0xA
    #define B_Fs_176p4KHz   0xC
    #define B_Fs_192KHz 0xE

#define REG_CHANNEL_ERR	0x85
#define REG_HWMUTE_RATE	0x86

#define REG_HWMUTE_CTRL	0x87
#define B_HW_FORCE_MUTE (1<<6)
#define B_HW_AUDMUTE_CLR_MODE (1<<5)
#define B_HW_MUTE_CLR (1<<4)
#define B_HW_MUTE_EN (1<<3)

#define M_HWMUTE_RATE_MASK 7

#define REG_HWAMP_CTRL	0x88
#define M_HWAMP_CTRL_MASK 3

#define REG_TRISTATE_CTRL	0x89

#define O_TRI_VIDEO 5
#define O_TRI_SPDIF 4
#define O_TRI_I2S3 3
#define O_TRI_I2S2 2
#define O_TRI_I2S1 1
#define O_TRI_I2S0 0

#define B_TRI_VIDEO (1<<5)
#define B_TRI_SPDIF (1<<4)
#define B_TRI_I2S3 (1<<3)
#define B_TRI_I2S2 (1<<2)
#define B_TRI_I2S1 (1<<1)
#define B_TRI_I2S0 (1<<0)

#define B_TRI_ALL  (B_TRI_VIDEO|B_TRI_SPDIF|B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0)
#define B_TRI_AUDIO  (B_TRI_SPDIF|B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0)
#define B_TRI_MASK  ~(B_TRI_VIDEO|B_TRI_SPDIF|B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0)


#define REG_BKSV0	0x8A
#define REG_BKSV1	0x8B
#define REG_BKSV2	0x8C
#define REG_BKSV3	0x8D
#define REG_BKSV4	0x8E
#define REG_RDROM_OFFSET	0x8F
#define REG_RDROM_REQBYTE	0x90
#define REG_RDROM_CTRL	0x91

#define REG_RDROM_STATUS	0x92
#define B_ROMIDLE   1
#define REG_RDROM_CLKCTRL	0x9B
#define B_ROM_CLK_SEL_REG   (1<<7)
#define B_ROM_CLK_VALUE     (1<<6)
#define B_VDO_MUTE_DISABLE  (1<<4)

#define REG_PKT_REC_TYPE	0xAA

#define SPD_INFOFRAME           0
#define ISRC1_PKT               1
#define ISRC2_PKT               2
#define DSD_PKT                 3
#define AVI_INFOFRAME           4
#define AUDIO_INFOFRAME         5
#define MPEG_INFOFRAME          6
#define VENDORSPEC_INFOFRAME    7
#define ACP_PKT                 8
#define UNRECOGNIZED_PKT        9
#define AUDIO_SAMPLE_PKT        10
#define GENERAL_CONTROL_PKT     11

// #define REG_PKT_TYPE	0xAB

#define REG_AVI_VER	0xAC
#define REG_AVI_DB0	0xAD
// for AVI Info Frame
// D[6:5] AVI Info frame color mode Y1Y0
// RGB    - 00
// YUV422 - 01
// YUV444 - 02
#define O_AVI_COLOR_MODE 5
#define M_AVI_COLOR_MASK   3
#define B_AVI_COLOR_RGB24  0
#define B_AVI_COLOR_YUV422 1
#define B_AVI_COLOR_YUV444 2
// D[4] A0 - Active format information present
// 0 - No Data
// 1 - Active Format Inforation valid
#define B_AVI_PRESENT (1<<4)
// D[3:2] B1B0 - Bar Info
// 00 - bar uc not valid
// 01 - vert. bar info valid
// 10 - horz. bar info valid
// 11 - vert and horz bar info valid
#define O_AVI_BAR_INFO 2
#define M_AVI_BAR_INFO_MASK 3
#define B_AVI_BAR_NOT_VALID 0
#define B_AVI_BAR_VINFO_VALID 1
#define B_AVI_BAR_HINFO_VALID 2
#define B_AVI_BAR_VHINFO_VALID 3
// D[1:0] Scan Information
// 00 - no uc
// 01 - Overscanned (Television)
// 10 - Underscanned (Computer)
// 11 - Future (reserved)
#define O_AVI_SCAN_INFO 0
#define M_AVI_SCAN_INFO_MASK 3
#define M_AVI_SCAN_NODATA    0
#define M_AVI_SCAN_OVER_SCAN 1
#define M_AVI_SCAN_UNDER_SCAN 2
#define REG_AVI_DB1	0xAE
// D[7:6] C1C0 Colorimetry
#define O_AVI_CLRMET 6
#define M_AVI_CLRMET_MASK 3
#define B_AVI_CLRMET_NODATA 0
#define B_AVI_CLRMET_ITU601  1
#define B_AVI_CLRMET_ITU709  2
// D[5:4] M1M0 Picture Aspect Ratio
#define O_AVI_PIC_ASRATIO 4
#define M_AVI_PIC_ASRATIO_MASK 3
#define B_AVI_PIC_ASRATIO_NODATA 0
#define B_AVI_PIC_ASRATIO_4_3    1
#define B_AVI_PIC_ASRATIO_16_9   2
// D[3:0] R3R2R1R0 Active Format Aspect Ratio
#define O_AVI_FMT_ASRATIO 0
#define M_AVI_FMT_ASRATIO_MASK  0xF
#define M_AVI_FMT_ASRATIO_EQPIC 0x8
#define M_AVI_FMT_ASRATIO_4_3 0x9
#define M_AVI_FMT_ASRATIO_16_9 0xA
#define M_AVI_FMT_ASRATIO_14_9 0xB
#define REG_AVI_DB2	0xAF
// D[1:0] SC1SC0 Non-uniform picture scaling
#define O_AVI_NUNI_SCALE 0
#define M_AVI_NUNI_SCALE_MASK 3
#define B_AVI_NUNI_SCALE_NODATA 0
#define B_AVI_NUNI_SCALE_HORZ 1
#define B_AVI_NUNI_SCALE_VERT 2
#define B_AVI_NUNI_SCALE_HORZVERT 3
// the following code is valid when AVI Infoframe ver 2.0
#define REG_AVI_DB3	0xB0
// D[6:0] - Video Codes
#define O_AVI_VIDCODE 0
#define O_AVI_VIDCODE_MASK 0x7f
#define REG_AVI_DB4	0xB1
// D[3:0] - Pixel Repeating Info
#define O_AVI_PIXREPT 0
#define M_AVI_PIXREPT_MASK 0xF
// pixel repeating value is the D[3:0]
#define REG_AVI_DB5	0xB2
#define REG_AVI_DB6	0xB3
#define REG_AVI_DB7	0xB4
#define REG_AVI_DB8	0xB5
#define REG_AVI_DB9	0xB6
#define REG_AVI_DB10	0xB7
#define REG_AVI_DB11	0xB8
#define REG_AVI_DB12	0xB9
#define REG_AVI_DB13	0xBA
#define REG_AVI_DB14	0xBB
#define REG_AVI_DB15	0xBC
#define REG_GENPKT_HB0	0xBD
#define REG_GENPKT_HB1	0xBE
#define REG_GENPKT_HB2	0xBF
#define REG_GENPKT_DB0	0xC0
#define REG_GENPKT_DB1	0xC1
#define REG_GENPKT_DB2	0xC2
#define REG_GENPKT_DB3	0xC3
#define REG_GENPKT_DB4	0xC4
#define REG_GENPKT_DB5	0xC5
#define REG_GENPKT_DB6	0xC6
#define REG_GENPKT_DB7	0xC7
#define REG_GENPKT_DB8	0xC8
#define REG_GENPKT_DB9	0xC9
#define REG_GENPKT_DB10	0xCA
#define REG_GENPKT_DB11	0xCB
#define REG_GENPKT_DB12	0xCC
#define REG_GENPKT_DB13	0xCD
#define REG_GENPKT_DB14	0xCE
#define REG_GENPKT_DB15	0xCF
#define REG_GENPKT_DB16	0xD0
#define REG_GENPKT_DB17	0xD1
#define REG_GENPKT_DB18	0xD2
#define REG_GENPKT_DB19	0xD3
#define REG_GENPKT_DB20	0xD4
#define REG_GENPKT_DB21	0xD5
#define REG_GENPKT_DB22	0xD6
#define REG_GENPKT_DB23	0xD7
#define REG_GENPKT_DB24	0xD8
#define REG_GENPKT_DB25	0xD9
#define REG_GENPKT_DB26	0xDA
#define REG_GENPKT_DB27	0xDB
#define REG_AUDIO_VER	0xDC
#define REG_AUDIO_DB0	0xDD
// D[7:4] CT3~CT0 Audio Coding Type
// 0000 - Refer to Stream Header
// 0001 - IEC60958 PCM
// 0010 - AC-3
// 0011 - MPEG1 (Layers 1 & 2)
// 0100 - MP3 (MPEG1 Layer 3)
// 0101 - MPEG2 (Multichannel)
// 0110 - AAC
// 0111 - DTS
// 1000 - ATRAC
#define O_AUD_CODTYPE 4
#define M_AUD_CODTYPE_MASK 0xF
#define B_AUD_CODTYPE_STREAMDEF 0
#define B_AUD_CODTYPE_IEC60958  1
#define B_AUD_CODTYPE_AC3       2
#define B_AUD_CODTYPE_MPEG1     3
#define B_AUD_CODTYPE_MP3       4
#define B_AUD_CODTYPE_MPEG2     5
#define B_AUD_CODTYPE_AAC       6
#define B_AUD_CODTYPE_DTS       7
#define B_AUD_CODTYPE_ATRAC     8
// D[2:0] Audio Channel Count
// 000 - Refer to stream header
// 001 ~ 111 channel # -1
#define O_AUD_CHCNT 0
#define M_AUD_CHCNT_MASK 0x7
#define B_AUD_CHCNT_STREAMDEF 0
#define B_AUD_CHCNT_CH2       1
#define B_AUD_CHCNT_CH3       2
#define B_AUD_CHCNT_CH4       3
#define B_AUD_CHCNT_CH5       4
#define B_AUD_CHCNT_CH6       5
#define B_AUD_CHCNT_CH7       6
#define B_AUD_CHCNT_CH8       7
#define REG_AUDIO_DB1	0xDE
// D[4:2] Sampling Freq
// 000 - Refer to stream definition
// 001 - 32KHz
// 010 - 44.1 KHz
// 011 - 48 KHz
// 100 - 88.2 KHz
// 101 - 96 KHz
// 110 - 176.4 KHz
// 111 - 192 KHz
#define O_AUD_SMP_FREQ  2
#define M_AUD_SMP_FREQ_MASK 7
#define B_AUD_SMP_FREQ_STREAMDEF 0
#define B_AUD_SMP_32K            1
#define B_AUD_SMP_44K            2
#define B_AUD_SMP_48K            3
#define B_AUD_SMP_88K            4
#define B_AUD_SMP_96K            5
#define B_AUD_SMP_176K           6
#define B_AUD_SMP_192K           7
// D[1:0] Sample Size
#define O_AUD_SMP_SIZE 0
#define O_AUD_SMP_SIZE_MASK 0x3
#define O_AUD_SMP_SIZE_STREAMDEF 0
#define O_AUD_SMP_16BIT 1
#define O_AUD_SMP_20BIT 2
#define O_AUD_SMP_24BIT 3
#define REG_AUDIO_DB2	0xDF
// depend the audio color code
// most case define the maximum bit rate / 8KHz
#define REG_AUDIO_DB3	0xE0
// CA - define the speak layout
#define REG_AUDIO_DB4	0xE1
// D[7] - Describes whether the down mixed stereo output is permitted or not
#define B_PROHIBITED (1<<7)
// D[6:3] Audio Level shift
#define O_AUD_LEVEL_SHIFT_VALUE 3
#define O_AUD_LEVEL_SHIFT_MASK 0xF
#define REG_AUDIO_DB5	0xE2
#define REG_MPEG_VER	0xE3
#define REG_MPEG_DB0	0xE4
#define REG_MPEG_DB1	0xE5
#define REG_MPEG_DB2	0xE6
#define REG_MPEG_DB3	0xE7
#define REG_MPEG_DB4	0xE8
#define REG_MPEG_DB5	0xE9
#define REG_VS_VER	0xEA
// 2007/05/30 added by jj_tseng@chipadvanced.com
#define REG_AUDIO_FMT 0xEB
#define B_LPCM 1
//~jj_tseng@chipadvanced.com
#define REG_VS_DB0	0xEB
#define REG_VS_DB1	0xEC
#define REG_VS_DB2	0xED
#define REG_VS_DB3	0xEE
#define REG_VS_DB4	0xEF
#define REG_VS_DB5	0xF0
#define REG_VS_DB6	0xF1
#define REG_VS_DB7	0xF2
#define REG_VS_DB8	0xF3
#define REG_ACP_TYPE	0xF4
#define REG_ACP_DB0	0xF5
#define REG_ACP_DB1	0xF6
#define REG_ACP_DB2	0xF7
#define REG_ACP_DB3	0xF8
#define REG_ACP_DB4	0xF9
#define REG_ACP_DB5	0xFA
#define REG_ACP_DB6	0xFB
#define REG_ACP_DB7	0xFC
#define REG_ACP_DB8	0xFD
#define REG_SRC_VER	0xFE
#define REG_SRC_PB25	0xFF

///////////////////////////////////////////////////////////
// mode uc type and structure
///////////////////////////////////////////////////////////

#define CAPS_MIN_PROGRESS_VFREQ 2300  // 23Hz
#define CAPS_MAX_PROGRESS_VFREQ 10000 // 100Hz
#define CAPS_MIN_PROGRESS_HFREQ 300   // 30.0 KHz
#define CAPS_MAX_PROGRESS_HFREQ 1000  // 100.0 KHz
#define CAPS_MIN_INTERLACE_VFREQ 2500 // 25Hz
#define CAPS_MAX_INTERLACE_VFREQ 6000 // 55Hz
#define CAPS_MIN_INTERLACE_HFREQ 150  // 15.0 KHz
#define CAPS_MAX_INTERLACE_HFREQ 550  // 60.0 KHz

//typedef struct _Sync_Info {
//    USHORT HRes, VRes ;
//    SHORT VFreq ; // in 0.01Hz unit
//    USHORT HTotal, VTotal ;
//    USHORT PCLK ; // in 100KHz unit
//    BYTE PixelRep ;
//    BYTE Mode ; // D[0] - interlace
//} SYNC_INFO, *PSYNC_INFO ;
//

#define F_MODE_INTERLACE  (1<<0) 
#define F_MODE_RGB24  (0<<1)
#define F_MODE_YUV422 (1<<1)
#define F_MODE_YUV444 (2<<1)
#define F_MODE_CLRMOD_MASK (3<<1)
#define F_MODE_ITU709  (1<<3)
#define F_MODE_16_235  (1<<4)
#define F_MODE_EN_UDFILT (1<<5) // output mode only, and loaded from EEPROM
#define F_MODE_EN_DITHER  (1<<6) // output mode only, and loaded from EEPROM
#define HDMI_RX_I2C_SLAVE_ADDR 0x90

/////////////////////////////////////////
// PC Implementation
/////////////////////////////////////////
// 1. SCL => output => Pin-2,  D0
// 2. SDA-write => output => Pin1, ~C0
// 3. SDA-read => input => Pin15, S3
// 4. ResetN=> output => Pin14, ~C1
// 5. SCDT => input => Pin13, S4
// 6. Vsync => input => Pin12, S5
// 7. INT => input => Pin11, ~S7
/////////////////////////////////////////

#define PORT_PRN_DATA 0x378
#define PORT_PRN_STAT 0x379
#define PORT_PRN_CTRL 0x37A

#define FLAG_CLEAR_INT_MODE 1
#define FLAG_CLEAR_INT_PKG 2
#define FLAG_CLEAR_INT_HDCP 4
#define FLAG_CLEAR_INT_AUDIO 8    
#define FLAG_CLEAR_INT_ALL (FLAG_CLEAR_INT_MODE|FLAG_CLEAR_INT_PKG|FLAG_CLEAR_INT_HDCP|FLAG_CLEAR_INT_AUDIO)
#define FLAG_CLEAR_INT_MASK (~FLAG_CLEAR_INT_ALL)

#endif

