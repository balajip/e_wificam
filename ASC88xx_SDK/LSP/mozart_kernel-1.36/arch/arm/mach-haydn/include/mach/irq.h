
/*
 * asm/arch-cpe/irq.h:
 */

#ifndef __ASM_ARCH_IRQ_H__
#define __ASM_ARCH_IRQ_H__

#include <mach/hardware.h>
#include <asm/io.h>
#include <mach/irq.h>
#include <mach/irqs.h>

#define fixup_irq(x) (x)

extern void fLib_SetIntTrig(unsigned int intNum, int intMode, int intLevel);
extern void haydn_mask_irq(unsigned int irq);
extern void haydn_unmask_irq(unsigned int irq);
extern void haydn_mask_ack_irq(unsigned int irq);
extern void haydn_clear_irq(unsigned int intNum);
extern void haydn_int_init(void);
/*
static __inline__ void irq_init_irq(void)
{
	unsigned long flags;
	int irq;

	save_flags_cli(flags);
	haydn_int_init();
	restore_flags(flags);

	for (irq = 0; irq < NR_IRQS; irq++) 
	{
		irq_desc[irq].valid	= 1;
		irq_desc[irq].probe_ok	= 1;
		irq_desc[irq].mask_ack	= haydn_mask_ack_irq;
		irq_desc[irq].mask	= haydn_mask_irq;
		irq_desc[irq].unmask	= haydn_unmask_irq;
	}
}
*/
#endif /* __ASM_ARCH_IRQ_H__ */
