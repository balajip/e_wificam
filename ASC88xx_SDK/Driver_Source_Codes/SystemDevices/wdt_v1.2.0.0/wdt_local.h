/*
 * wdt
 * Driver for watchdog timer.
 *
 * Copyright (C) 2007-2011  VN Inc.
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

#ifndef __WDT_LOCAL_H__
#define __WDT_LOCAL_H__

#include <mach/platform.h>

#include "wdt.h"

#define DEVICE_NAME                "wdt"
#if defined(__ASM_ARCH_PLATFORM_BEETHOVEN_H__) || defined(__ASM_ARCH_PLATFORM_ROSSINI_H__)
#  define WDT_CLKFREQ              (APB_CLK / 64)
#else
#  define WDT_CLKFREQ              (APB_CLK)
#endif

#define WDT_MMR_BASE               IO_ADDRESS(VPL_WDTC_MMR_BASE)

#define WDT_MMR_WDT_VERSION        (WDT_MMR_BASE)
#define WDT_MMR_CTRL               (WDT_MMR_BASE + 0x4)
#define WDT_MMR_STAT               (WDT_MMR_BASE + 0x8)
#define WDT_MMR_COUNT              (WDT_MMR_BASE + 0xc)
#define WDT_MMR_RELOAD_VALUE       (WDT_MMR_BASE + 0x10)
#define WDT_MMR_MATCH_VALUE        (WDT_MMR_BASE + 0x14)
#define WDT_MMR_RELOAD_CTRL        (WDT_MMR_BASE + 0x18)
#define WDT_MMR_RST_LEN            (WDT_MMR_BASE + 0x1c)


#define WDT_CTRL_ACK_EN 0x2
#define WDT_CTRL_OP_EN  0x4

#define WDT_RELOAD_PASSWD 0x28791166

#define WDT_RESET_LEN 0xffff

#undef PDEBUG

#ifdef DEBUG
#  ifdef __KERNEL__
#    define LOGLEVEL    KERN_EMERG
#    define PDEBUG(fmt, args...) printk( LOGLEVEL "wdt: " fmt, ## args)
#  else
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* do nothing */
#endif

#endif /* __WDT_LOCAL_H__ */
