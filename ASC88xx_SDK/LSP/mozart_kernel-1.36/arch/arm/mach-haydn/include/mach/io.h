
/*
 * linux/include/asm-armnommu/arch-dsc21/io.h
 *
 * Copyright (C) 1997-1999 Russell King
 *
 * Modifications:
 *  06-12-1997	RMK	Created.
 *  07-04-1999	RMK	Major cleanup
 *  02-19-2001  gjm     Leveraged for armnommu/dsc21
 */
#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H


/*
 * kernel/resource.c uses this to initialize the global ioport_resource struct
 * which is used in all calls to request_resource(), allocate_resource(), etc.
 * --gmcnutt
 */
#define IO_SPACE_LIMIT 0xffffffff

/* define IN_BYTE OUT_BYTE for IDE, syma */
#define HAVE_ARCH_OUT_BYTE
#define OUT_BYTE(b,p)          outb((b),(IO_ADDRESS((p))))
#define OUT_WORD(w,p)          outw((w),(IO_ADDRESS((p))))
#define OUT_LONG(l,p)          outl((l),(IO_ADDRESS((p))))
#define HAVE_ARCH_IN_BYTE
#define IN_BYTE(p)             (byte)inb(IO_ADDRESS((p)))
#define IN_WORD(p)             (short)inw(IO_ADDRESS((p)))
#define IN_LONG(p)             (short)inl(IO_ADDRESS((p)))

#define __io(a) 			(a)

#define __mem_pci(a)	(a) //jon((unsigned long)(a))

//#define __arch_getw(a) 		(*(volatile unsigned short *)(a))
//#define __arch_putw(v,a) 	(*(volatile unsigned short *)(a) = (v))


/*
 * Defining these two gives us ioremap for free. See asm/io.h.
 * --gmcnutt
 */
#define iomem_valid_addr(iomem,sz) (1)
#define iomem_to_phys(iomem) (iomem)

#endif
