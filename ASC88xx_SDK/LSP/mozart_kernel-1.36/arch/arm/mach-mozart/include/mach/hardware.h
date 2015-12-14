
/********************************************************/
/*							*/
/*							*/
/********************************************************/
#ifndef __ASM_ARCH_HARDWARE_H__
#define __ASM_ARCH_HARDWARE_H__

//#include <mach/platform.h>
#include <mach/mozart.h>
#include <mach/chipset.h>
#include <mach/interrupt.h>

// --------------------------------------------------------------------
#define TIMER_COUNTER		(PCLK_HZ/HZ)

//#define VPint	*(volatile unsigned int *)
//#define VPshort	*(volatile unsigned short *)
//#define VPchar	*(volatile unsigned char *)


#define IO_BASE			0xF0000000                  // VA of IO 
#define IO_ADDRESS(x) (((x >> 4)&0xffff0000) + (x & 0xffff) + IO_BASE) 

#define PCIO_BASE		PCI_IO_VADDR
#define PCIMEM_BASE		PCI_MEMORY_VADDR

#define pcibios_assign_all_busses()	1

#define PCIBIOS_MIN_IO		0x6000
#define PCIBIOS_MIN_MEM 	0x00100000
#endif /* __ASM_ARCH_HARDWARE_H__ */
