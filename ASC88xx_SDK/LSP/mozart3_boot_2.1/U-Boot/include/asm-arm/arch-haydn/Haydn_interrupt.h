#ifndef __HAYDN_INTERRUPT_H__
#define __HAYDN_INTERRUPT_H__

#define EVM_INTC_BASE					0x98600000		/*   Interrupt Controller 	*/
#define EVM_INTC_SRC_LO 0x04
#define EVM_INTC_SRC_HI 0x08
#define EVM_INTC_STAT_LO 0x0C
#define EVM_INTC_STAT_HI 0x10
#define EVM_INTC_MASK_LO 0x14
#define EVM_INTC_MASK_HI 0x18
#define EVM_INTC_CLEAR_LO 0x1C
#define EVM_INTC_CLEAR_HI 0x20
#define EVM_INTC_TRIGGER_MODE_LO 0x24
#define EVM_INTC_TRIGGER_MODE_HI 0x28

#define IRQ_TMR 0x40 //num = 38

#endif
