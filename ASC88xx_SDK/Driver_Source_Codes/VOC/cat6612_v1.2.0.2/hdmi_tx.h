/*
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

#ifndef __HDMI_TX_H__
#define __HDMI_TX_H__
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

/* Color mode */
#define FORMAT_RGB444	0x00  // RGB444
#define FORMAT_YUV422	0x01  // YUV422
#define FORMAT_YUV444	0x02  // YUV422

/* Input Signal Type */
#define INSIG_SYNCSEP   0x00  // for external HSYNC, VSYNC and DE
#define INSIG_SYNCEMB   0x01  // for 16bit sync embedded, color is YUV422 only

/* Video Identification (ID) Code defined in CEA-861 */
typedef enum CEA_VID
{
    HDMI_Unkown = 0 ,
    HDMI_640x480p60 = 1 ,
    HDMI_480p60,
    HDMI_480p60_16x9,
    HDMI_720p60,
    HDMI_1080i60,
    HDMI_480i60,
    HDMI_480i60_16x9,
    HDMI_1080p60 = 16,
    HDMI_576p50,
    HDMI_576p50_16x9,
    HDMI_720p50,
    HDMI_1080i50,
    HDMI_576i50,
    HDMI_576i50_16x9,
    HDMI_1080p50 = 31,
    HDMI_1080p24,
    HDMI_1080p25,
    HDMI_1080p30,
    HDMI_720p24 = 60,
    HDMI_720p25,
    HDMI_720p30

} HDMI_Video_ID;

struct hdmi_tx_device
{
	int (*open)(HDMI_Video_ID VICID, int clockfreq, unsigned char inputColorMode, unsigned char inputSignalType, unsigned char outputColorMode);
	void (*release)(void);
	int (*ioctl)(unsigned char ioctl_arg);
	unsigned char (*read_reg)(unsigned char reg_addr);
	void (*write_reg)(unsigned char reg_addr, unsigned char val);
};
