#ifndef __TW2866_H__
#define __TW2866_H__

#include <i2c.h>

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

#define TW2866x1_CODEC01_ADDR 0x29 
#define TW2866x2_CODEC01_ADDR 0x28 
#define TW2866x2_CODEC02_ADDR 0x29
#define MUX_DEV_ADDR_1ST 0x35
#define MUX_DEV_ADDR_2ND 0x37
#define MUX_DEVICE_NUM 2 //There are two mux.
#define MUX_OUTPUT_NUM 4 //Each has 4 output.


static void tw2866_write_data(uchar codec_addr, unsigned long reg, uchar data)
{		
    i2c_write(codec_addr, reg, 1, &data, 1) ;
}

static uchar tw2866_read_data(uchar codec_addr, uchar reg)
{
	uchar data ;
	
	i2c_read(codec_addr, reg, 1, &data, 1) ;
	
	return data ;
}

#define TW2866_RESET_AUDIO (1 << 5)
#define TW2866_RESET_VOUT  (1 << 4)
#define TW2866_RESET_VDEC4 (1 << 3)
#define TW2866_RESET_VDEC3 (1 << 2)
#define TW2866_RESET_VDEC2 (1 << 1)
#define TW2866_RESET_VDEC1 (1 << 0)
#define TW2866ID 0x19
#define TW2866_REG_ID01 0xFE
#define TW2866_REG_ID02 0xFF
static int tw2866_version_check_func(uchar codec_addr)
{
	int result = 0 ;
    uint tmp_val ;
	unsigned long id_val = 0 ;

	//We will repeate START singal in tw2866x1
    CONFIG_SOFT_I2C_READ_REPEATED_START = 0 ;

	//get ID[6:5]
	tmp_val = tw2866_read_data(codec_addr, TW2866_REG_ID01) ;	
	tmp_val = (tmp_val >> 6) & 0x3;
	id_val = tmp_val << 5 ;

	//get ID[4:0]
	tmp_val = tw2866_read_data(codec_addr, TW2866_REG_ID02) ;	
	tmp_val = (tmp_val >> 3) & 0x1f ;
	id_val |= tmp_val ;

	//check ID
	if(id_val != TW2866ID) 		
		result = -1 ;

	return result ;
}

#endif

