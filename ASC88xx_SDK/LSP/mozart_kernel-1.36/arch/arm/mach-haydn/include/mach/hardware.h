
/********************************************************/
/*							*/
/*							*/
/********************************************************/
#ifndef __ASM_ARCH_HARDWARE_H__
#define __ASM_ARCH_HARDWARE_H__

//#include <asm/arch/platform.h>
#include <mach/haydn.h>
#include <mach/chipset.h>
#include <mach/interrupt.h>
//#include <asm/arch/haydn/pci.h>

// --------------------------------------------------------------------
#define TIMER_COUNTER		(PCLK_HZ/HZ)

//#define VPint	*(volatile unsigned int *)
//#define VPshort	*(volatile unsigned short *)
//#define VPchar	*(volatile unsigned char *)



//#ifndef CSR_WRITE
//	#define CSR_WRITE(addr,data)	(VPint(addr) = (data))
//#endif

//#ifndef CSR_READ
//	#define CSR_READ(addr)			(VPint(addr))
//#endif


#define DEBUG_CONSOLE	(0)

#define IO_BASE			0xF0000000                  // VA of IO 
#define IO_SIZE			0x0B000000                  // How much?
#define IO_START		CPE_HDR_BASE                // PA of IO
#define IO_ADDRESS(x) (((x >> 4)&0xffff0000) + (x & 0xffff) + IO_BASE) 

#define PCI_MEMORY_VADDR        0xe0000000
#define PCI_CONFIG_VADDR        0xf9180000
#define PCI_V3_VADDR            0xf9180000
#define PCI_IO_VADDR            0xf9190000

#define PCIO_BASE		PCI_IO_VADDR
#define PCIMEM_BASE		PCI_MEMORY_VADDR

#define pcibios_assign_all_busses()	1

#define PCIBIOS_MIN_IO		0x6000
#define PCIBIOS_MIN_MEM 	0x00100000

#endif /* __ASM_ARCH_HARDWARE_H__ */
