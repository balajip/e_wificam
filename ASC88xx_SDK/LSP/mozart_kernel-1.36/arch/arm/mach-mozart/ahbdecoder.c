/* initial configuration for AHB decoder */
#include <linux/config.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/init.h>


#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/arch/mozart/ahbdecoder.h>
#include <asm/io.h>


void ahb_decoder_init(int size)
{
	int sdram_reg;
	int sdram_base;
	sdram_reg = IO_ADDRESS((MOZART_AHBC_0_BASE+AHB_DECODER_SDRAM_REG));
	sdram_base = inl(sdram_reg);
	sdram_base &= 0xFFFFFF00; // remove size information
	switch (size) {
		case 1:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_1M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 1 Mbytes\n");
			break;
		case 2:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_2M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 2 Mbytes\n");
			break;
		case 4:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_4M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 4 Mbytes\n");
			break;
		case 8:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_8M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 8 Mbytes\n");
			break;
		case 16:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_16M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 16 Mbytes\n");
			break;
		case 32:
			printk("SDRAM SIZE register: 0x%08X\n", sdram_reg);
			printk("SDRAM BASE and SIZE: 0x%08X\n", sdram_base|AHB_DECODER_SDRAM_SIZE_32M);
			printk(KERN_INFO "AHB decoder SDRAM size set to 32 Mbytes\n");
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_32M), sdram_reg);
			break;
		case 64:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_64M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 64 Mbytes\n");
			break;
		case 128:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_128M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 128 Mbytes\n");
			break;
		case 256:
			outl((sdram_base|AHB_DECODER_SDRAM_SIZE_256M), sdram_reg);
			printk(KERN_INFO "AHB decoder SDRAM size set to 256 Mbytes\n");
			break;
		default:
			printk(KERN_ERR "ahb_decoder_init: SDRAM size must be power of 2 and in unit of mega bytes (1-256).\n");
	}
	return;
}
