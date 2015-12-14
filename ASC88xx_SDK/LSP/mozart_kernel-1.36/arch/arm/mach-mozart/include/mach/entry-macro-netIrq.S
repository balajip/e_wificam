/*
 * include/asm-arm/arch-integrator/entry-macro.S
 *
 * Low-level IRQ helper macros for Integrator platforms
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <asm/hardware.h>
#include <asm/arch/irqs.h>

 		.macro	disable_fiq
		.endm
//__freq_irq_num: .word 0x00000000
//__freq_irq_stat: .word 0x00000000

		.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
                ldr             \base, =(IO_ADDRESS(BACH_INTC_MMR_BASE)+IRQ_STATUS_REG)
                ldr             \irqstat, [\base]

		//		ldr				\irqnr, __freq_irq_stat
				mov				\irqnr, #0x00000002
				tst				\irqnr, \irqstat
				mov				\irqnr, #1
				bne				2004f

                mov             \irqnr, #0


2003:
                tst             \irqstat, #1
                bne             2004f
                add             \irqnr, \irqnr, #1
                mov             \irqstat, \irqstat, lsr #1
                cmp             \irqnr, #32
                bcc             2003b
2004:
	//			movne			\irqstat, #1
	//			movne			\irqstat, \irqstat, lsl \irqnr
	//			strne			\irqnr, __last_irq_num
	//			strne			\irqstat, __last_irq_stat
//2005:				
                .endm
                                                                                
