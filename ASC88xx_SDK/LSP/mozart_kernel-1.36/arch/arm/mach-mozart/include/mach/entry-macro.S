/*
 * include/asm-arm/arch-integrator/entry-macro.S
 *
 * Low-level IRQ helper macros for Integrator platforms
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <mach/hardware.h>
#include <mach/irqs.h>

 		.macro	disable_fiq
		.endm

		.macro  get_irqnr_preamble, base, tmp
		/*ldr	\base, =IO_ADDRESS(MOZART_INTC_MMR_BASE)+IRQ_STATUS_LO_REG*/
		.endm

		.macro  arch_ret_to_user, tmp1, tmp2
		.endm

		.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
                ldr             \base, =(IO_ADDRESS(MOZART_INTC_MMR_BASE)+IRQ_STATUS_LO_REG)
                ldr             \irqstat, [\base]
                mov             \irqnr, #0
2003:
                tst             \irqstat, #1
                bne             2005f
                add             \irqnr, \irqnr, #1
                mov             \irqstat, \irqstat, lsr #1
                cmp             \irqnr, #32
                bcc             2003b
                
				ldr             \base, =(IO_ADDRESS(MOZART_INTC_MMR_BASE)+IRQ_STATUS_HI_REG)
                ldr             \irqstat, [\base]
2004:
                tst             \irqstat, #1
                bne             2005f
                add             \irqnr, \irqnr, #1
                mov             \irqstat, \irqstat, lsr #1
                cmp             \irqnr, #64
                bcc             2004b

2005:
                .endm
                                                                                
