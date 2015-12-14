/*
 *  linux/arch/arm/kernel/dma-arc.c
 *
 *  Copyright (C) 1998-1999 Dave Gilbert / Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  DMA functions specific to bach architecture
 */
//#include <linux/config.h>
#include <linux/sched.h>
#include <linux/init.h>

#include <asm/dma.h>
#include <asm/fiq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>


#define dma_t unsigned int


void __init arch_dma_init(dma_t * dma)
{
	return;
}

