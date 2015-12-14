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
 *
 */
#ifndef __IICCTRL_H__
#define __IICCTRL_H__
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

/* ============================================================================================== */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */

/* ============================================================================================== */
typedef enum iicctrl_trans_type
{
	IICCTRL_TYPE_ADDR_DATA = 1,
	IICCTRL_TYPE_ADDR_CMD_DATA = 2,
	IICCTRL_TYPE_SEQUENTIAL_DATA = 3,
	IICCTRL_TYPE_MICRON_16_BITS = 4,
	IICCTRL_TYPE_ADDR_DATA_NOSTOP = 5,
	IICCTRL_TYPE_MICRON_16_BITS_TYPE_0 = 6,
	IICCTRL_TYPE_WRITEBUF_NO_ACK = 7,
	IICCTRL_TYPE_WRITEBUF_FAST_NOACK = 8,
	IICCTRL_TYPE_ADDR_16_BITS_DATA = 9,		// W: ID(w), 2B addr, 1B data; R: ID(w), 2B addr, ID(r), 1B data
} EIICCtrlTransType;

typedef struct iicctrl_init_options
{
	unsigned long dwTransType;
	unsigned long dwMaxDataLength;
	unsigned long dwBusNum;
} TIICCtrlInitOptions;

/* ============================================================================================== */
// conform to the definition in GPIO_I2C device driver
#define IICCTRL_STOP_BIT_ATTACHED		0x0000
#define IICCTRL_NO_STOP_BIT_ATTACHED	0x0001
#define IICCTRL_NO_ACK					0x0002
#define IICCTRL_FAST					0x0004				
#define IICCTRL_FAST_NO_ACK				(IICCTRL_FAST|IICCTRL_NO_ACK)
#define IICCTRL_IGNORE_ACK_CHECK		0x0008
/* ============================================================================================== */
typedef struct iicctrl_info
{
	unsigned long dwTransType;
	unsigned long dwDeviceAddr;
	unsigned long dwMaxDataLength;
	unsigned long dwBusNum;
	unsigned char *pbyData;
} TIICCtrlInfo;

/* ============================================================================================== */
long IICCtrl_Initial(void * *phObject, TIICCtrlInitOptions *ptInitOptions);
long IICCtrl_Release(void * *phObject);

unsigned long IICCtrl_QueryMemSize(TIICCtrlInitOptions *ptInitOptions);

long IICCtrl_ReadReg(void * hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char *pbyRegData);
long IICCtrl_WriteReg(void * hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char byRegData);

long IICCtrl_ReadWordReg(void * hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char bySpecRegAddr, unsigned short *pwRegData);
long IICCtrl_WriteWordReg(void * hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char bySpecRegAddr, unsigned short wRegData);

long IICCtrl_ReadBuf(void * hObject, unsigned char byDevAddr, unsigned char byCmdRegAddr, unsigned long dwLength, unsigned char *pbyData);
long IICCtrl_WriteBuf(void * hObject, unsigned char byDevAddr, unsigned char byCmdRegAddr, unsigned long dwLength, const unsigned char *pbyData);

/* ======================================== */
#define IICCtrl_ID_VERSION "1.0.0.2"


