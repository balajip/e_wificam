/* linux/include/asm-arm/arch-integrator/debug-macro.S
 *
 * Debugging macro include header
 *
 *  Copyright (C) 1994-1999 Russell King
 *  Moved from linux/arch/arm/kernel/debug.S by Ben Dooks
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <mach/platform.h>

		.macro	addruart,rx
		mrc	p15, 0, \rx, c1, c0
		tst	\rx, #1			@ MMU enabled?
		moveq	\rx, #HAYDN_UARTC0_MMR_BASE	@ physical base address of UARTC0
		movne	\rx, #0xf0000000	@ virtual base
		addne	\rx, \rx, #HAYDN_UARTC0_MMR_BASE >> 4
		.endm

		.macro	senduart,rd,rx
		strb	\rd, [\rx, #SERIAL_THR]
		.endm

		.macro	waituart,rd,rx
1001:		ldr	\rd, [\rx, #SERIAL_LSR]
		tst	\rd, #SERIAL_LSR_THRE
		bne	1001b
		.endm

		.macro	busyuart,rd,rx
		.endm
