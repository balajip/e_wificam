
/*
 * linux/include/asm-armnommu/arch-p52/memory.h
 *
 * Copyright (c) 1999 Nicolas Pitre <nico@cam.org>
 * 2001 Mindspeed
 */

#ifndef __ASM_ARCH_MEMORY_H__
#define __ASM_ARCH_MEMORY_H__

//#include <asm/page.h>


//#define TASK_SIZE		(0xc0000000UL)	// modified by syma
//#define TASK_SIZE_26		TASK_SIZE

#define DRAM_BASE		0x00000000
///#define DRAM_SIZE		0x1000000
#define DRAM_SIZE		0x02000000 /* 32MB SDRAM */ // aren 20040511

#define PHYS_OFFSET		(DRAM_BASE)								// physical start address of memory
//#define PAGE_OFFSET 	PHYS_OFFSET								// virtual start address of memory (在 uClinux 下和 PHYS_OFFSET 是一樣的)
#define PAGE_OFFSET 	0xc0000000//(0xc0000000UL)								// virtual start address of memory (在 uClinux 下和 PHYS_OFFSET 是一樣的)
#define END_MEM     	(DRAM_BASE + DRAM_SIZE)
//#define DMA_SIZE		0x400000
#define DMA_SIZE		0xffffffff

#define __virt_to_phys__is_a_macro
#define __virt_to_phys(vpage) ((vpage) - PAGE_OFFSET)
#define __phys_to_virt__is_a_macro
#define __phys_to_virt(ppage) ((ppage) + PAGE_OFFSET)

#define __virt_to_bus__is_a_macro
#define __virt_to_bus(x)	(x - PAGE_OFFSET + PHYS_OFFSET)
//#define __virt_to_bus(x)	(x - PAGE_OFFSET)
#define __bus_to_virt__is_a_macro
#define __bus_to_virt(x)	(x - PHYS_OFFSET + PAGE_OFFSET)
//#define __bus_to_virt(x)	(x - 0x80000000 + PAGE_OFFSET)

//#define TASK_UNMAPPED_BASE (TASK_SIZE / 3)

/*
#define FA510_REGION_SIZE_4K    0x17
#define FA510_REGION_SIZE_8K    0x19
#define FA510_REGION_SIZE_16K   0x1B
#define FA510_REGION_SIZE_32K   0x1D
#define FA510_REGION_SIZE_64K   0x1F
#define FA510_REGION_SIZE_128K  0x21
#define FA510_REGION_SIZE_256K  0x23
#define FA510_REGION_SIZE_512K  0x25
#define FA510_REGION_SIZE_1M    0x27
#define FA510_REGION_SIZE_2M    0x29
#define FA510_REGION_SIZE_4M    0x2B
#define FA510_REGION_SIZE_8M    0x2D
#define FA510_REGION_SIZE_16M   0x2F
#define FA510_REGION_SIZE_32M   0x31
#define FA510_REGION_SIZE_64M   0x33
#define FA510_REGION_SIZE_128M  0x35
#define FA510_REGION_SIZE_256M  0x37
#define FA510_REGION_SIZE_512M  0x39
#define FA510_REGION_SIZE_1G    0x3B
#define FA510_REGION_SIZE_2G    0x3D
#define FA510_REGION_SIZE_4G    0x3F
*/

#endif  //__ASM_ARCH_MEMORY_H__
