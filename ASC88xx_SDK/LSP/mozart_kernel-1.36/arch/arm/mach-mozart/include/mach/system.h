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
#include <mach/hardware.h>


#define WDT_BASE (IO_ADDRESS(MOZART_WDTC_MMR_BASE))
#define WDT_LOAD        (WDT_BASE+0x10)
#define WDT_RESTART     (WDT_BASE+0x18)
#define WDT_CR          (WDT_BASE+0x4)
#define WDT_RSTLEN      (WDT_BASE+0x1c)
#define WDT_RESTART_PASSWD 0x28791166
#define WDT_CR_ENABLE   0x4       /* WDT enable */

#define MSHC_BASE	(IO_ADDRESS(MOZART_MSHC0_MMR_BASE))
#define MSHC_INTC	(MSHC_BASE+0x24)
static inline void arch_idle(void)
{
	cpu_do_idle();
//	while (!current->need_resched && !hlt_counter);
}

extern inline void arch_reset(char mode)
{
	/* MSHC clean */
	writel(0x0, MSHC_INTC);
	writel(0x4, MSHC_BASE);
	writel(0x2, MSHC_BASE);
	writel(0x1, MSHC_BASE);
	/* REVISIT --gmcnutt */
	writel(0xffff, WDT_RSTLEN); //set reset signal length
	writel(0x0, WDT_CR); //disable first
	writel(10, WDT_LOAD); //count down from 10
	writel(WDT_RESTART_PASSWD, WDT_RESTART); //load it to counter
	writel(WDT_CR_ENABLE, WDT_CR); //count down start
	while(1);
}


#endif  //__ASM_ARCH_SYSTEM_H__
