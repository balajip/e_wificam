/*
 * Mozart_Audio_WAU8822
 * Driver for Mozart WAU8822 Audio Codec Driver
 *
 * Copyright (C) 2010  ______ Inc.
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

#ifndef _WAU8822_H
#define _WAU8822_H

/* WAU8822 register space */

#define WAU8822_RESET		0x0
#define WAU8822_POWER1		0x1
#define WAU8822_POWER2		0x2
#define WAU8822_POWER3		0x3
#define WAU8822_IFACE		0x4
#define WAU8822_COMP			0x5
#define WAU8822_CLOCK		0x6
#define WAU8822_ADD			0x7
#define WAU8822_GPIO			0x8
#define WAU8822_JACK1        0x9
#define WAU8822_DAC			0xa
#define WAU8822_DACVOLL	    0xb
#define WAU8822_DACVOLR      0xc
#define WAU8822_JACK2        0xd
#define WAU8822_ADC			0xe
#define WAU8822_ADCVOLL		0xf
#define WAU8822_ADCVOLR      0x10
#define WAU8822_EQ1			0x12
#define WAU8822_EQ2			0x13
#define WAU8822_EQ3			0x14
#define WAU8822_EQ4			0x15
#define WAU8822_EQ5			0x16
#define WAU8822_DACLIM1		0x18
#define WAU8822_DACLIM2		0x19
#define WAU8822_NOTCH1		0x1b
#define WAU8822_NOTCH2		0x1c
#define WAU8822_NOTCH3		0x1d
#define WAU8822_NOTCH4		0x1e
#define WAU8822_ALC1			0x20
#define WAU8822_ALC2			0x21
#define WAU8822_ALC3			0x22
#define WAU8822_NGATE		0x23
#define WAU8822_PLLN			0x24
#define WAU8822_PLLK1		0x25
#define WAU8822_PLLK2		0x26
#define WAU8822_PLLK3		0x27
#define WAU8822_VIDEO		0x28
#define WAU8822_3D           0x29
#define WAU8822_BEEP         0x2b
#define WAU8822_INPUT		0x2c
#define WAU8822_INPPGAL  	0x2d
#define WAU8822_INPPGAR      0x2e
#define WAU8822_ADCBOOSTL	0x2f
#define WAU8822_ADCBOOSTR    0x30
#define WAU8822_OUTPUT		0x31
#define WAU8822_MIXL	        0x32
#define WAU8822_MIXR         0x33
#define WAU8822_HPVOLL		0x34
#define WAU8822_HPVOLR       0x35
#define WAU8822_SPKVOLL      0x36
#define WAU8822_SPKVOLR      0x37
#define WAU8822_OUT3MIX		0x38
#define WAU8822_MONOMIX      0x39
#define WAU8822_SPKBOOST	0x31
#define WAU8822_AUX1BOOST    0x31
#define WAU8822_AUX2BOOST	0x31
#define WAU8822_BIASGEN	0x3

#define WAU8822_CACHEREGNUM 	58

/*
 * WAU8822 Clock dividers
 */
#define WAU8822_MCLKDIV 		0
#define WAU8822_BCLKDIV		1
#define WAU8822_OPCLKDIV		2
#define WAU8822_DACOSR		3
#define WAU8822_ADCOSR		4
#define WAU8822_MCLKSEL		5

#define WAU8822_MCLK_MCLK		(0 << 8)
#define WAU8822_MCLK_PLL			(1 << 8)

#define WAU8822_MCLK_DIV_1		(0 << 5)
#define WAU8822_MCLK_DIV_1_5		(1 << 5)
#define WAU8822_MCLK_DIV_2		(2 << 5)
#define WAU8822_MCLK_DIV_3		(3 << 5)
#define WAU8822_MCLK_DIV_4		(4 << 5)
#define WAU8822_MCLK_DIV_5_5		(5 << 5)
#define WAU8822_MCLK_DIV_6		(6 << 5)

#define WAU8822_BCLK_DIV_1		(0 << 2)
#define WAU8822_BCLK_DIV_2		(1 << 2)
#define WAU8822_BCLK_DIV_4		(2 << 2)
#define WAU8822_BCLK_DIV_8		(3 << 2)
#define WAU8822_BCLK_DIV_16		(4 << 2)
#define WAU8822_BCLK_DIV_32		(5 << 2)

#define WAU8822_DACOSR_64		(0 << 3)
#define WAU8822_DACOSR_128		(1 << 3)

#define WAU8822_ADCOSR_64		(0 << 3)
#define WAU8822_ADCOSR_128		(1 << 3)

#define WAU8822_OPCLK_DIV_1		(0 << 4)
#define WAU8822_OPCLK_DIV_2		(1 << 4)
#define WAU8822_OPCLK_DIV_3		(2 << 4)
#define WAU8822_OPCLK_DIV_4		(3 << 4)
#ifdef SPKVDD_5V
	#define WAU8822_5V_MODE 1
#else
	#define WAU8822_5V_MODE 0
#endif // #if definf SPKVDD_5V

//extern struct snd_soc_dai wau8822_dai;
//extern struct snd_soc_codec_device soc_codec_dev_wau8822;

#endif
