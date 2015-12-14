/*
 * loader.lds.S
 *
 * Copyright 2011 VN Inc. All rights reserved.
 *
 */

#include <loader.h>


OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{
#if WHERE_IS_BOOTLOADER == BOOT_NF2K
	. = VPL_BRC_RAM_BASE + 0x200;
#else
	. = VPL_BRC_RAM_BASE;
#endif

	. = ALIGN(4);
	.text : { *(.text) }

	. = ALIGN(4);
	.rodata : { *(.rodata) }

	. = ALIGN(4);
	.data : { *(.data) }

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) }

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) }
	__bss_end = .;

	.got : { *(.got) }

#if CFG_0_BASE_ADDR != 0x0
	__remapper_start = .;
	.remapper VPL_USBC_RAM_BASE : AT (__remapper_start)
	{
		*(.remapper)
	}

	__remapper_size = SIZEOF(.remapper);
#endif

#if WHERE_IS_BOOTLOADER == BOOT_SF || WHERE_IS_BOOTLOADER == BOOT_NF
#if CFG_0_BASE_ADDR != 0x0
	.dualboot __remapper_start + SIZEOF(.remapper) : AT (__remapper_start + SIZEOF(.remapper))
#else
	.dualboot :
#endif
	{
		*(.dualboot)
	}
#endif

	_end = .;

	__fdstack = (VPL_USBC_RAM_BASE + USBC_RAM_SIZE) - 0x40;
}
