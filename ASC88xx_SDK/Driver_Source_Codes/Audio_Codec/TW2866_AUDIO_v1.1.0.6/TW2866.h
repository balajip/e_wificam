/*
 * Copyright (C) 2007  ______ Inc.
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

#ifndef __TW2866_H__
#define __TW2866_H__

/* TW2866 Codec Register definitions */
#define TW2866_REG_ADACCT1     0x50
#define TW2866_REG_DAC_LPFBIAS 0x51
#define TW2866_REG_ADACLPF     0x52
#define TW2866_REG_ANADACTEST  0x53
#define TW2866_REG_ASAVE       0x54
#define TW2866_REG_PLL1        0x60
#define TW2866_REG_PLL2        0x61
#define TW2866_REG_CLK2MD      0x6A
#define TW2866_REG_CLK3MD      0x6B
#define TW2866_REG_CLK4MD      0x6C
#define TW2866_REG_CLKPNPOL    0x6D
#define TW2866_REG_ACLKPOL     0x70
#define TW2866_REG_AINCTL      0x71
#define TW2866_REG_MRATIOMD    0x72
#define TW2866_REG_A5NUM       0x73
#define TW2866_REG_A5DETST     0x74
#define TW2866_REG_AADC5OFS_H  0x75
#define TW2866_REG_AADC50FS_L  0x76
#define TW2866_REG_AUD5ADC_H   0x77
#define TW2866_REG_AUD5ADC_L   0x78
#define TW2866_REG_ADJAADC5_H  0x79
#define TW2866_REG_ADJAADC5_L  0x7A
#define TW2866_REG_I2SO_RSEL   0x7B
#define TW2866_REG_I2SO_LSEL   0x7C
#define TW2866_REG_RECSEL5     0x7D
#define TW2866_REG_ADATMI2S    0x7E
#define TW2866_REG_AIGAIN5     0x7F
#define TW2866_REG_SRST        0x80
#define TW2866_REG_AIN5MD      0x89
#define TW2866_REG_AADCOFS_H   0xB3
#define TW2866_REG_AADC1OFS_L  0xB4 
#define TW2866_REG_AADC2OFS_L  0XB5
#define TW2866_REG_AADC3OFS_L  0XB6
#define TW2866_REG_AADC4OFS_L  0XB7
#define TW2866_REG_AUDADC_H    0XB8
#define TW2866_REG_AUD1ADC_L   0XB9
#define TW2866_REG_AUD2ADC_L   0XBA
#define TW2866_REG_AUD3ADC_L   0XBB
#define TW2866_REG_AUD4ADC_L   0XBC
#define TW2866_REG_ADJAADC_H   0XBD
#define TW2866_REG_ADJAADC1_L  0XBE
#define TW2866_REG_ADJAADC2_L  0XBF
#define TW2866_REG_ADJAADC3_L  0XC0
#define TW2866_REG_ADJAADC4_L  0XC1
#define TW2866_REG_ANAPWDN     0XCE
#define TW2866_REG_SMD         0XCF
#define TW2866_REG_AIGAIN21    0XD0
#define TW2866_REG_AIGAIN43    0XD1
#define TW2866_REG_R_MULTCH    0XD2
#define TW2866_REG_R_SEQ10     0XD3
#define TW2866_REG_R_SEQ32     0XD4
#define TW2866_REG_R_SEQ54     0XD5
#define TW2866_REG_R_SEQ76     0XD6
#define TW2866_REG_R_SEQ98     0XD7
#define TW2866_REG_R_SEQBA     0XD8
#define TW2866_REG_R_SEQDC     0XD9
#define TW2866_REG_R_SEQFE     0XDA
#define TW2866_REG_AMASTER     0XDB
#define TW2866_REG_MIX_MUTE    0XDC
#define TW2866_REG_MIX_RATIO21 0XDD
#define TW2866_REG_MIX_RATIO43 0XDE
#define TW2866_REG_MIX_RATIOP  0XDF
#define TW2866_REG_MIX_OUTSEL  0XE0
#define TW2866_REG_ADET        0XE1
#define TW2866_REG_ADET_TH21   0XE2
#define TW2866_REG_ADET_TH43   0XE3
#define TW2866_REG_ACKI_L      0XF0
#define TW2866_REG_ACKI_M      0XF1
#define TW2866_REG_ACKI_H      0XF2
#define TW2866_REG_ACKN_L      0XF3
#define TW2866_REG_ACKN_M      0XF4
#define TW2866_REG_ACKN_H      0XF5
#define TW2866_REG_SDIV        0XF6
#define TW2866_REG_LRDIV       0XF7
#define TW2866_REG_ACCNTL      0XF8
#define TW2866_REG_CLKOCTL     0xFA
#define TW2866_REG_AVDET_MODE  0xFB
#define TW2866_REG_AVDET_ENA   0xFC
#define TW2866_REG_AVDET_STATE 0XFD
#define TW2866_REG_DEV_ID      0XFF

#define TW2866_CACHEREGNUM 	256
static const unsigned char tw2866_all_regs[TW2866_CACHEREGNUM] = {
         // 0     // 1    // 2    // 3     // 4    // 5    // 6     // 7   // 8     // 9    // A     // B    // C    // D     // E    // F
/*0x00*/ 0x68,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0F,   0xD0,   0x00,   0x10,   0x07,   0x7F,  
/*0x10*/ 0x87,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0C,   0xD0,   0xD0,   0x00,   0x27,   0x7F,  
/*0x20*/ 0x97,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0C,   0xD0,   0xD0,   0x00,   0x27,   0x7F,  
/*0x30*/ 0x87,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0C,   0xD0,   0x10,   0x00,   0x17,   0x7F,  
/*0x40*/ 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
/*0x50*/ 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x7F,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
/*0x60*/ 0x15,   0x03,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
/*0x70*/ 0x00,   0x00,   0x00,   0x01,   0x01,   0x03,   0xEF,   0x03,  0xDD,   0x03,   0xCA,   0x15,   0x15,   0xE4,   0xA3,   0xB0,
/*0x80*/ 0x00,   0x02,   0x00,   0xCC,   0x00,   0x80,   0x44,   0x50,  0x22,   0x02,   0xD8,   0xBC,   0xB8,   0x44,   0x38,   0x00,
/*0x90*/ 0x00,   0x78,   0x44,   0x3D,   0x14,   0xA5,   0xE0,   0x05,  0x00,   0x28,   0x44,   0x44,   0xA0,   0x8A,   0x42,   0x00,
/*0xA0*/ 0x08,   0x08,   0x08,   0x08,   0x1A,   0x1A,   0x1A,   0x1A,  0x00,   0x00,   0x00,   0xF0,   0xF0,   0xF0,   0xF0,   0x00,
/*0xB0*/ 0x00,   0x4A,   0x00,   0xFF,   0xEF,   0xEF,   0xEF,   0xEF,  0xFF,   0xDD,   0xDD,   0xDD,   0xDB,   0xFF,   0xCA,   0xCB,
/*0xC0*/ 0xCA,   0xCC,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x00,   0x01,   0x00,   0x39,   0xE4,   0x00,   0x00,
/*0xD0*/ 0x88,   0x88,   0x01,   0x01,   0x99,   0x99,   0x99,   0x23,  0x99,   0x99,   0x99,   0xE1,   0x0f,   0x00,   0x00,   0x40,
/*0xE0*/ 0x14,   0xE0,   0xBB,   0xBB,   0x00,   0x11,   0x00,   0x00,  0x11,   0x00,   0x00,   0x11,   0x00,   0x00,   0x11,   0x00,
/*0xF0*/ 0x65,   0x85,   0x35,   0x00,   0x72,   0x03,   0x01,   0x20,  0xC4,   0x11,   0x4a,   0x2F,   0xFF,   0xF1,   0x00,   0x63
};

//static unsigned char tw2866_firsttime_regs[TW2866_CACHEREGNUM] = {0} ;

#define SAMPLE_RATE_TABLE_48KHZ 0
#define SAMPLE_RATE_TABLE_44KHZ 1
#define SAMPLE_RATE_TABLE_32KHZ 2
#define SAMPLE_RATE_TABLE_16KHZ 3
#define SAMPLE_RATE_TABLE_08KHZ 4
static unsigned int tw2866_sample_rate_table_50MHz[5][6] = {
		{ 0x15, 0x41, 0x3A, 0x00, 0xC0, 0x03},
		{ 0x65, 0x85, 0x35, 0x00, 0x72, 0x03},
		{ 0x0E, 0xD6, 0x26, 0x00, 0x80, 0x02},
		{ 0x07, 0x6B, 0x13, 0x00, 0x40, 0x01},
		{ 0x83, 0xB5, 0x09, 0x00, 0xA0, 0x00}		
} ;

static unsigned int tw2866_sample_rate_table_60MHz[5][6] = {
		{ 0x15, 0x41, 0x3A, 0xCD, 0x20, 0x03},
		{ 0x65, 0x85, 0x35, 0xBC, 0xDF, 0x02},
		{ 0x0E, 0xD6, 0x26, 0xDE, 0x15, 0x02},
		{ 0x07, 0x6B, 0x13, 0xEF, 0x0A, 0x01},
		{ 0x83, 0xB5, 0x09, 0x78, 0x85, 0x00}		
} ;

// There are total 256 regs for TW2866.
// But a lot of them are for Video purposes.
// Here we put Audio-Related-Regs together to prevent from accessing Video regs.
// !! They should be in a increasing order.
static const unsigned char tw2866_audio_regs_index[] = {
        TW2866_REG_ADACCT1     ,
        TW2866_REG_DAC_LPFBIAS ,
        TW2866_REG_ADACLPF     ,
        TW2866_REG_ANADACTEST  ,
        TW2866_REG_ASAVE       ,
        TW2866_REG_ACLKPOL     ,
        TW2866_REG_AINCTL      ,
        TW2866_REG_AIN5MD      ,
        TW2866_REG_AIGAIN21    ,
        TW2866_REG_AIGAIN43    ,
        TW2866_REG_AIGAIN5     ,
        TW2866_REG_R_MULTCH    ,
        TW2866_REG_R_SEQ10     ,
        TW2866_REG_R_SEQ32     ,
        TW2866_REG_R_SEQ54     ,
        TW2866_REG_R_SEQ76     ,
        TW2866_REG_R_SEQ98     ,
        TW2866_REG_R_SEQBA     ,
        TW2866_REG_R_SEQDC     ,
        TW2866_REG_R_SEQFE     ,
        TW2866_REG_AMASTER     ,
        TW2866_REG_MIX_MUTE    ,
        TW2866_REG_SMD         ,
        TW2866_REG_ADET        ,
        TW2866_REG_ADET_TH21   ,
        TW2866_REG_ADET_TH43   ,
        TW2866_REG_ACKI_L      ,
        TW2866_REG_ACKI_M      ,
        TW2866_REG_ACKI_H      ,
        TW2866_REG_ACKN_L      ,
        TW2866_REG_ACKN_M      ,
        TW2866_REG_ACKN_H      ,
        TW2866_REG_SDIV        ,
        TW2866_REG_LRDIV       ,
        TW2866_REG_ACCNTL      ,
        TW2866_REG_AVDET_ENA   ,
        TW2866_REG_A5NUM       ,
        TW2866_REG_AVDET_STATE ,
        TW2866_REG_A5DETST     ,
        TW2866_REG_SRST        ,
        TW2866_REG_ANAPWDN     ,
        TW2866_REG_PLL1        ,
        TW2866_REG_PLL2        ,
        0x7b, 0x7c, 0x7d,
        0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0x75, 0x76,
        0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0x77, 0x78, 
        0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0x79, 0x7a, 
        0x72, 0xdd, 0xde, 0xdf, 0xe0,
        0xfa, 0xfb
} ;

static unsigned char tw2866_1st_reg_cache[TW2866_CACHEREGNUM] = {0} ;
static unsigned char tw2866_2nd_reg_cache[TW2866_CACHEREGNUM] = {0} ;

#endif

