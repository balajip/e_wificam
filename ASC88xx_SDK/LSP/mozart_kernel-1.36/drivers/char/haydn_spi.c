/*
 * Serial Peripheral Interface (SPI) driver for the Atmel AT91RM9200 (Thunder)
 *
 *  Copyright (C) SAN People (Pty) Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <mach/spi.h>
#include <linux/pci.h>

/* ......................................................................... */
char haydn_spi_dev;
static int __init haydn_spi_init(void)
{
	//unsigned long reg;

	printk("[SPI debug]init~\n");

	// initialize 
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		// disable SSIC
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable all slaves
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x71c7);	// CTRLR0 : Transmit-Only 
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x1f);		// BaudR 
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts 
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);		// enalbe SSIC  

	if(request_irq (APBC_IRQ_NUM, haydn_spi_interrupt, IRQF_SHARED, "APBC DMA CHN2/CHN3", &haydn_spi_dev))
	{
		printk("irq request fail~!\n");
	}

	// increase the priority of SSIC TX/RX APBC DMA channels
	//SNPS_APBC_WRITE(APBC_DMA_CHN_PRIORITY,  (1<<APBC_SPIRX_DMA_CHN) | (1<<APBC_SPITX_DMA_CHN));	
	// increase the priority of APBC on AHB bus1
	//reg = __raw_readl(0xfc100004);
	//reg |= 0x2000000;
	//__raw_writel(reg, 0xfc100004);
	
	return 0;
}

/* ......................................................................... */
static void __exit haydn_spi_exit(void)
{

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		// disable SSIC
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable all slaves

	printk("[SPI debug]exit~\n");
}

/* ......................................................................... */
//EXPORT_SYMBOL(spi_transfer_dma);

module_init(haydn_spi_init);
module_exit(haydn_spi_exit);

MODULE_LICENSE("GPL")
MODULE_AUTHOR("Evelyn")
MODULE_DESCRIPTION("SPI driver for VN Haydn")

