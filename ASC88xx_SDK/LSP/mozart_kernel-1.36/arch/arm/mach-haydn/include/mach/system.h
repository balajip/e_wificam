/*
 * linux/include/asm-arm/arch-bach/system.h
 *
 * Copyright (c) 1999 Nicolas Pitre <nico@cam.org>
 * Copyright (c) 2001 RidgeRun, Inc (http://www.ridgerun.org)
 *
 */
#ifndef __ASM_ARCH_SYSTEM_H__
#define __ASM_ARCH_SYSTEM_H__

#include <asm/io.h>
#include <mach/platform.h>
//#ifndef BACH_WDTC_MMR_BASE
//  #define BACH_WDTC_MMR_BASE 0x98500000
//#endif

#define WDT_BASE (IO_ADDRESS(HAYDN_WDTC_MMR_BASE))
#define WDT_LOAD        (WDT_BASE+0x10)
#define WDT_RESTART     (WDT_BASE+0x18)
#define WDT_CR          (WDT_BASE+0x4)
#define WDT_RESTART_PASSWD 0x70703897
#define WDT_CR_ENABLE   0x4       /* WDT enable */
//#define WDT_CR_RST      2       /* WDT system reset enable */

static inline void arch_idle(void)
{
	cpu_do_idle();
//	while (!current->need_resched && !hlt_counter);
}

extern inline void arch_reset(char mode)
{
	/* REVISIT --gmcnutt */
	writel(0x0, WDT_CR); //disable first
	writel(10, WDT_LOAD); //count down from 10
	writel(WDT_RESTART_PASSWD, WDT_RESTART); //load it to counter
	writel(WDT_CR_ENABLE, WDT_CR); //count down start
	while(1);
}


extern int SharedBus_HoldBus(void);
extern void SharedBus_LooseBus(void);

#endif  //__ASM_ARCH_SYSTEM_H__
