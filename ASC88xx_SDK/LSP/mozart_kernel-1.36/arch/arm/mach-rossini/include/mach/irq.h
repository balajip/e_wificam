
/*
 * mach-cpe/irq.h:
 */

#ifndef __ASM_ARCH_IRQ_H__
#define __ASM_ARCH_IRQ_H__

#include <mach/hardware.h>
#include <asm/io.h>
#include <mach/irq.h>
#include <mach/irqs.h>

#define fixup_irq(x) (x)

extern void fLib_SetIntTrig(unsigned int intNum, int intMode, int intLevel);
extern void rossini_mask_irq(unsigned int irq);
extern void rossini_unmask_irq(unsigned int irq);
extern void rossini_mask_ack_irq(unsigned int irq);
extern void rossini_clear_irq(unsigned int intNum);
extern void rossini_int_init(void);

/* add for MSI support */
extern int rossini_retrigger_irq(unsigned int);
extern void rossini_msi_unmask_irq(unsigned int irq);
extern void rossini_ack_irq(unsigned int irq);

#endif /* __ASM_ARCH_IRQ_H__ */
