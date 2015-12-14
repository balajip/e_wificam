/*
 * nf2k_preloader/nf2k_preloader.lds.S
 *
 * Copyright 2011 VN Inc.  All rights reserved.
 *
 */

#include <config.h>
#include <loader.h>
#undef VPL_USBC_RAM_BASE
#define VPL_USBC_RAM_BASE   0x80020000

OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = VPL_USBC_RAM_BASE + 0xa0;

    . = ALIGN(4);
	.text      :
	{
	  	*(.text)
	}

    . = ALIGN(4);
    .rodata : { *(.rodata) }

    . = ALIGN(4);
    .data : { *(.data) }
}
