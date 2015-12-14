/*
 * Copyright (C) 2012  VN Inc.
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
 * Description: IRCUT Driver for Mozart EVM and RDK
 *
 * $History: ircut_local.h $
 * 
 * *****************  Version 1.0.0.1  *****************
 * User: SC. FanChiang       Date: 12/08/07   Time: 11:38a
 * First Release.
 * 
 *
 */

/* ============================================================================================== */
#ifndef __IRCUTCTRL_LOCALS_H__
#define __IRCUTCTRL_LOCALS_H__

/* ============================================================================================== */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <mach/ugpio.h>
#include <asm/arch/platform.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include "ircut.h"

#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <linux/mm.h>
#include <linux/init.h>
#include <linux/ioport.h>

//	AP1511A : IR Filter Driver
#define IRCUT_ENB	10	
#define IRCUT_FBC	11
//#define GPIO_IRCUT		10
//#define UART_IRCUT		PIN_UGPIO_nDTR
#define ONE_WIRE			1
#define TWO_WIRE			2

extern struct file_operations gpio_ircut_fops;

/* ============================================================================================== */
#endif //__IRCUTCTRL_LOCALS_H__

/* ============================================================================================== */
