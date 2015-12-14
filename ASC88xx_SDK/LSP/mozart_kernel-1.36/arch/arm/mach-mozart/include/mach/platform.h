/*
 * $Header: $
 *
 * Copyright (C) 2007-2013 VN Inc. All rights reserved.
 *
 * Description:
 * 	headfile for board specific
 *
 *
 * $History: $
 *
 */
/* ======================================================================== */

#ifndef __ASM_ARCH_PLATFORM_H__
#define __ASM_ARCH_PLATFORM_H__
/* ======================================================================== */

#include <linux/types.h>
#include <mach/hardware.h>
#include <mach/chipset.h>
#include <mach/interrupt.h>
//#include <mach/rtc.h>
#include <mach/mozart.h>
extern struct platform_device mozart_device_mmc0;
extern struct platform_device mozart_device_mmc1;
extern struct platform_device mozart_device_i2c_bus_0;
extern struct platform_device mozart_device_i2c_bus_1;
extern struct platform_device mozart_device_spi;
extern struct platform_device mozart_dw_spi_dev;
extern struct platform_device mozart_device_dwmac;
//-------------------------------------------------
//[jam patch add] Add some global info
extern struct platform_device mozart_device_nand;
#include <mach/nand.h>
//-------------------------------------------------

#include <mach/sysc64timer.h>


/* ======================================================================== */
#endif	// __ASM_ARCH_PLATFORM_H__
