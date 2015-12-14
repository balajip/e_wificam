/***************************************************************************
* Porting to Linux on 20030620                                             *
* Author: Paul Chiang                                                      *
* Version: 0.1                                                             *
* History:                                                                 *
*          0.1 Porting for CPE PCI+Promise IDE                             *
* Todo:                                                                    *
****************************************************************************/


#include <asm/irq.h>

/*
 * Set up a hw structure for a specified data port, control port and IRQ.
 * This should follow whatever the default interface uses.
 */
static __inline__ void
ide_init_hwif_ports(hw_regs_t *hw, int data_port, int ctrl_port, int *irq)
{
	ide_ioreg_t reg = (ide_ioreg_t) data_port;
	int i;

	for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++) {
		hw->io_ports[i] = reg;
		reg += 1;
	}
	hw->io_ports[IDE_CONTROL_OFFSET] = (ide_ioreg_t) ctrl_port;
	
	//hw->irq=	FIQ_PCI;
	//printk("irq=0x%X\n",FIQ_PCI);
	//if (irq)
	//	*irq = 0;
}

/*
 * This registers the standard ports for this architecture with the IDE
 * driver.
 */
static __inline__ void ide_init_default_hwifs(void)
{
	/* There are no standard ports */
}
