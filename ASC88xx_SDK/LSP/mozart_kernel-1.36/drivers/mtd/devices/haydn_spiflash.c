/*
 * Copyright (C) 2012  VN Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <mach/spi.h>
#include <asm/cacheflush.h>
#include <linux/spinlock.h> // spin_lock_irqsave(), spin_lock_irqrestore()
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()
#include <linux/interrupt.h>
#include <linux/delay.h> // msleep

static struct mutex   spiflash_mutex;
static  void haydn_write_enable(void);

#undef DEBUG
//#define DEBUG_PRINTK
#ifdef DEBUG_PRINTK
#define DEBUG(args...) printk(KERN_DEBUG args)
#else
#define DEBUG(args...)
#endif

//#define SPANSION_4BYTEMODE
/* ......................................................................... */
#define DATAFLASH_MAX_DEVICES	4	/* max number of dataflash devices */
#undef	DATAFLASH_ALWAYS_ADD_DEVICE	/* always add whole device when using partitions? */

/* ......................................................................... */
enum
{
	//Instruction set
	SPI_FLASH_INS_WREN		= 0x06,		// write enable
	SPI_FLASH_INS_WRDI		= 0x04,		// write disable
	SPI_FLASH_INS_RDSR		= 0x05,		// read status register
	SPI_FLASH_INS_WRSR		= 0x01,		// write status register
	SPI_FLASH_INS_READ		= 0x03,		// read data bytes
	SPI_FLASH_INS_FAST_READ	= 0x0B,		// read data bytes at higher speed
	SPI_FLASH_INS_PP			= 0x02,		// page program
	SPI_FLASH_INS_SE			= 0xD8,		// sector erase
	SPI_FLASH_INS_BE        = 0xC7, // bulk erase
	SPI_FLASH_INS_EN4B      = 0xB7, // enter 4-byte address mode
	SPI_FLASH_INS_EX4B      = 0xE9, // exit 4-byte address mode
	SPI_FLASH_INS_RDSCUR    = 0x2B, // read security register
	SPI_FLASH_INS_RDCR			= 0x15, // MSIC configuration register
	SPI_FLASH_INS_BRRD		= 0x16,
	SPI_FLASH_INS_BRWR		= 0x17,
	SPI_FLASH_INS_BRAC		= 0xB9,
	SPI_FLASH_INS_RDID		= 0x9F,
};

/* ......................................................................... */
struct dataflash_local
{
	int spi;			/* SPI chip-select number */

	unsigned int page_size;		/* number of bytes per page */
	unsigned short page_offset;	/* page offset in flash address */
};


enum {
	ADDR_4_BYTE_SUPPORT = 0x1,
	ADDR_4_BYTE_ENTERED = 0x2,
};

static unsigned char phy_flashes[DATAFLASH_MAX_DEVICES] =
{
#if CONFIG_MTD_SPI_FLASH_NO1_SIZE > SZ_16M
	ADDR_4_BYTE_SUPPORT,
#else
	0,
#endif
#ifdef CONFIG_MTD_SPI_FLASH_NUM_2
#if CONFIG_MTD_SPI_FLASH_NO2_SIZE > SZ_16M
	ADDR_4_BYTE_SUPPORT,
#else
	0,
#endif
#endif
};


/* Detected DataFlash devices */
static struct mtd_info* mtd_devices[DATAFLASH_MAX_DEVICES];
static int nr_devices = 0;
static int spi_chip_select = 0 ;

/* ......................................................................... */

#ifdef CONFIG_MTD_PARTITIONS

#ifdef CONFIG_MTD_SPI_FLASH
static struct mtd_partition board_sf_default_partition[] =
{
    {
        .name       = CONFIG_MTD_SPI_FLASH_MAP_0_NAME,
        .offset     = CONFIG_MTD_SPI_FLASH_MAP_0_OFFSET,
        .size       = CONFIG_MTD_SPI_FLASH_MAP_0_SIZE,
        //.mask_flags   = MTD_WRITEABLE,
    },
    {
        .name       = CONFIG_MTD_SPI_FLASH_MAP_1_NAME,
        .offset     = CONFIG_MTD_SPI_FLASH_MAP_1_OFFSET,
        .size       = CONFIG_MTD_SPI_FLASH_MAP_1_SIZE,
        //.mask_flags   = MTD_WRITEABLE,
#if defined(CONFIG_MTD_SPI_FLASH_MAP_3_PARTITION) || defined(CONFIG_MTD_SPI_FLASH_MAP_4_PARTITION)
    }, {
        .name =     CONFIG_MTD_SPI_FLASH_MAP_2_NAME,
        .offset =   CONFIG_MTD_SPI_FLASH_MAP_2_OFFSET,
        .size =     CONFIG_MTD_SPI_FLASH_MAP_2_SIZE,
#endif

#if defined(CONFIG_MTD_SPI_FLASH_MAP_4_PARTITION)
    }, {
        .name =     CONFIG_MTD_SPI_FLASH_MAP_3_NAME,
        .offset =   CONFIG_MTD_SPI_FLASH_MAP_3_OFFSET,
        .size =     CONFIG_MTD_SPI_FLASH_MAP_3_SIZE,
#endif

    }
};

#define CONFIG_MTD_SPI_FLASH_NO1_PAGE_NUM   (CONFIG_MTD_SPI_FLASH_NO1_SIZE / CONFIG_MTD_SPI_FLASH_MAP_PAGE_SIZE)
#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
  #define CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE CONFIG_MTD_SPI_FLASH_NO1_SIZE
  #define CONFIG_MTD_SPI_FLASH_MAP_PAGE_NUM CONFIG_MTD_SPI_FLASH_NO1_PAGE_NUM
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
  #define CONFIG_MTD_SPI_FLASH_NO2_PAGE_NUM   (CONFIG_MTD_SPI_FLASH_NO2_SIZE / CONFIG_MTD_SPI_FLASH_MAP_PAGE_SIZE)
  #define CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE (CONFIG_MTD_SPI_FLASH_NO1_SIZE + CONFIG_MTD_SPI_FLASH_NO2_SIZE)
  #define CONFIG_MTD_SPI_FLASH_MAP_PAGE_NUM (CONFIG_MTD_SPI_FLASH_NO1_PAGE_NUM + CONFIG_MTD_SPI_FLASH_NO2_PAGE_NUM)
#else
  #error "You must choose the number of spi flashes"
#endif

/* Check if the size of partitions does not match flash size */
#if defined (CONFIG_MTD_SPI_FLASH_MAP_2_PARTITION)
#if CONFIG_MTD_SPI_FLASH_MAP_0_SIZE+CONFIG_MTD_SPI_FLASH_MAP_1_SIZE > CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE
#error "Total size of 2 partitions > flash size"
#endif//CONFIG_MTD_SPI_FLASH_MAP_0_SIZE+CONFIG_MTD_SPI_FLASH_MAP_1_SIZE > CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE

#elif defined (CONFIG_MTD_SPI_FLASH_MAP_3_PARTITION)
#if CONFIG_MTD_SPI_FLASH_MAP_0_SIZE+CONFIG_MTD_SPI_FLASH_MAP_1_SIZE+CONFIG_MTD_SPI_FLASH_MAP_2_SIZE > CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE
#error "Total size of 3 partitions > flash size"
#endif//CONFIG_MTD_SPI_FLASH_MAP_0_SIZE+CONFIG_MTD_SPI_FLASH_MAP_1_SIZE+CONFIG_MTD_SPI_FLASH_MAP_2_SIZE > CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE

#elif defined (CONFIG_MTD_SPI_FLASH_MAP_4_PARTITION)
#if CONFIG_MTD_SPI_FLASH_MAP_0_SIZE+CONFIG_MTD_SPI_FLASH_MAP_1_SIZE+CONFIG_MTD_SPI_FLASH_MAP_2_SIZE+CONFIG_MTD_SPI_FLASH_MAP_3_SIZE > CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE
#error "Total size of 4 partitions > flash size"
#endif//CONFIG_MTD_SPI_FLASH_MAP_0_SIZE+CONFIG_MTD_SPI_FLASH_MAP_1_SIZE+CONFIG_MTD_SPI_FLASH_MAP_2_SIZE+CONFIG_MTD_SPI_FLASH_MAP_3_SIZE > CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE
#endif

#else /* CONFIG_MTD_SPI_FLASH */
static struct mtd_partition static_partitions_8M[] =
{
    {
        .name       = "read-only code section",
        .offset     = 0,
        .size           = 0x400000,
        .mask_flags = MTD_WRITEABLE,
    },
    {
        .name       = "jffs2 image section",
        .offset     = 0x400000,
        .size           = 0x400000,
        //.mask_flags   = MTD_WRITEABLE,
    }

};
#endif /* CONFIG_MTD_SPI_FLASH */

#endif

/* ......................................................................... */

/* Allocate a single SPI transfer descriptor.  We're assuming that if multiple
   SPI transfers occur at the same time, spi_access_bus() will serialize them.
   If this is not valid, then either (i) each dataflash 'priv' structure
   needs it's own transfer descriptor, (ii) we lock this one, or (iii) use
   another mechanism.   */
static struct spi_transfer_list* spi_transfer_desc;

//#define CONFIG_SPIFLASH_CPUMODE_OPERATION_ONLY

static DECLARE_COMPLETION(spiflash_rx_complete);
static DECLARE_COMPLETION(spiflash_tx_complete);

static uint8_t spiflash_read_security(int cs);
static uint8_t spiflash_read_configuration(int cs);
static uint8_t winbond_read_status(int cs);

/* ......................................................................... */
int haydn_spi_interrupt(int irq, void *dev_id)
{
	unsigned long dma_ctrl_data, interrupt_chn;

	interrupt_chn = SNPS_APBC_READ(APBC_DMA_CHN_MONITOR);
	// chn3 interrupts
	if (interrupt_chn & (0x1<<APBC_SPITX_DMA_CHN))
	{
		dma_ctrl_data = SNPS_APBC_READ(APBC_SPITX_DMA_CTRL);
		SNPS_APBC_WRITE(APBC_SPITX_DMA_CTRL, dma_ctrl_data & ~0x2);
		SNPS_APBC_WRITE(APBC_DMA_CHN_MONITOR, ~(0x1<<APBC_SPITX_DMA_CHN));
		complete(&spiflash_tx_complete);
		return IRQ_HANDLED;
	}
	// chn2 interrupts
	if (interrupt_chn & (0x1<<APBC_SPIRX_DMA_CHN))
	{
		dma_ctrl_data = SNPS_APBC_READ(APBC_SPIRX_DMA_CTRL);
		SNPS_APBC_WRITE(APBC_SPIRX_DMA_CTRL, dma_ctrl_data & ~0x2);
		SNPS_APBC_WRITE(APBC_DMA_CHN_MONITOR, ~(0x1<<APBC_SPIRX_DMA_CHN));
		complete(&spiflash_rx_complete);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}


int _ReadID(int cs, unsigned char silence)
{
	int iRet = 0, i, data_num ;
	char buf[5] = {0};
			
	if (!silence) printk("\n _ReadID: %x \n", iRet);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0);
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x4);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x3c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x5);
	SNPS_SPI_WRITE(SNPS_SPI_TXFTLR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_RXFTLR, 0x3);
	
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
  SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_RDID); //
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave

	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);	
  while ((SNPS_SPI_READ(SNPS_SPI_SR) & 0x1) != 0) ; // wait till SSI idle
	//while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE) == 0);   // wait untill RX FIFO Not Empty

	data_num = SNPS_SPI_READ(SNPS_SPI_RXFLR);
	i = 0;
	do {
		buf[i++] = SNPS_SPI_READ(SNPS_SPI_DR);	
	}while (data_num--);   // wait untill RX FIFO Not Empty
	
			
	if (!silence) printk("\n id: %x %x %x %x %x \n", buf[0], buf[1], buf[2], buf[3], buf[4]);
		
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0);   //disable	 slave
	return iRet;
}

int _ReadBankRegister(int cs, unsigned char silence)
{
	int iRet = 0;
			
	if (!silence) printk("\n _ReadBankRegister: %x \n", iRet);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0);
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x4);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x3c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x0);
	SNPS_SPI_WRITE(SNPS_SPI_TXFTLR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_RXFTLR, 0x0);
	
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
  SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_BRRD); //
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave

	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);	
  while ((SNPS_SPI_READ(SNPS_SPI_SR) & 0x1) != 0) ; // wait till SSI idle
	//while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE) == 0);   // wait untill RX FIFO Not Empty

	iRet = SNPS_SPI_READ(SNPS_SPI_DR);
	if (!silence) printk("\n iRet: %x \n", iRet);
	
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0);   //enable slave
	return iRet;
}
      

int _WriteBankRegister(int cs, unsigned char value, unsigned char silence)
{
	int Ret = 0;
	static int bankRegisterSwitchStatus = 0;
	
	if ( bankRegisterSwitchStatus == value ) return Ret;
		
	if (!silence) printk("\n _WriteBankRegister: %x \n", Ret);
	
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0);
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x4);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x1c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x0);
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_BRWR); //
	SNPS_SPI_WRITE(SNPS_SPI_DR, value); //BA
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave
	bankRegisterSwitchStatus = value;
	
	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);
  while ((SNPS_SPI_READ(SNPS_SPI_SR) & 0x1) != 0) ; // wait till SSI idle

	SNPS_SPI_WRITE(SNPS_SPI_SER, 0);   //Disable slave
	return Ret;
}    

/*------------------------------------------------------------------------------
 * _enter_4_byte_mode()
 *
 * Entering 4-byte address mode, and check the 4-byte mode bit.
 * Return: the 4-byte mode bit.
 *----------------------------------------------------------------------------*/
static uint8_t
_enter_4_byte_mode(int cs)
{
	unsigned char ret = 0;
	
	#ifndef SPANSION_4BYTEMODE
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);    // disable SSI
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x1c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x0);

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_EN4B);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave

	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_BUSY) == 1);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);   // disable slave
	
#ifdef LEGACY_MXIC_16MB
	ret = spiflash_read_security(cs) & 0x4;
	if (ret) 
	{
		return ret;
	}
	else		//may be rev.F, check RDCR
	{	
		ret = spiflash_read_configuration(cs);
		
		return (ret & 0x20);
	}
#else
	return winbond_read_status(cs) & 0x1;
#endif

	#else
	
	_ReadBankRegister(spi_chip_select, 1);
	_WriteBankRegister(spi_chip_select, 0x80, 1);
	ret = _ReadBankRegister(spi_chip_select, 1);
	
  return ret&0x80;
	#endif
}

/*------------------------------------------------------------------------------
 * enter_4_byte_mode()
 *
 * Entering 4-byte address mode if capable.
 * Return: 0, if not enter 4-byte mode; 1, if enter 4-byte mode.
 *----------------------------------------------------------------------------*/
static uint8_t
enter_4_byte_mode(int cs)
{
	if ((phy_flashes[cs] & (ADDR_4_BYTE_SUPPORT|ADDR_4_BYTE_ENTERED))
		== ADDR_4_BYTE_SUPPORT)
	{
		if (_enter_4_byte_mode(cs))
			phy_flashes[cs] |= ADDR_4_BYTE_ENTERED;
		DEBUG("phy_flashes[%d] = 0x%x", cs, phy_flashes[cs]);
	}

	return (phy_flashes[cs] & ADDR_4_BYTE_ENTERED) == ADDR_4_BYTE_ENTERED;
}


/*------------------------------------------------------------------------------
 * spiflash_read_security()
 *
 * Read the security register.
 * Return: value of thr security register.
 *----------------------------------------------------------------------------*/
static uint8_t
spiflash_read_security(int cs)
{
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);    // disable SSI
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x3c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x0);

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_RDSCUR);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave

	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE) == 0);   // wait untill RX FIFO Not Empty

	return SNPS_SPI_READ(SNPS_SPI_DR);
}

/*------------------------------------------------------------------------------
 * winbond_read_status()
 *
 * Read the status register.
 * Return: value of thr status register.
 *----------------------------------------------------------------------------*/
static uint8_t
winbond_read_status(int cs)
{
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);    // disable SSI
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x3c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x0);

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_RDCR);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave

	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE) == 0);   // wait untill RX FIFO Not Empty

	return SNPS_SPI_READ(SNPS_SPI_DR);
}

/*------------------------------------------------------------------------------
 * spiflash_read_security()
 *
 * Read the security register.
 * Return: value of thr security register.
 *----------------------------------------------------------------------------*/
static uint8_t
spiflash_read_configuration(int cs)
{
	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);    // disable SSI
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);       // disable slave
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);       // mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);  // set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x3c7);  // 8-bit
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, 0x0);

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_RDCR);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << cs);   //enable slave

	// wait for SPI TX FIFO empty
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_TFE) == 0);
	while ((SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE) == 0);   // wait untill RX FIFO Not Empty

	return SNPS_SPI_READ(SNPS_SPI_DR);
}

/* ......................................................................... */
/*
 * Return the status of the DataFlash device.
 */
static unsigned short haydn_spiflash_status(void)
{
	volatile unsigned long status;
	volatile unsigned char ret;

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			//disable slave
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);	// set BaudR
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x73c7);
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR1,  0x0);

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_RDSR);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);			//enable slave

	// wait for SPI TX FIFO empty
	do
	{
		status = SNPS_SPI_READ(SNPS_SPI_SR);
	}while (!(status & SNPS_SPI_SR_TFE));

	while ((SNPS_SPI_READ(SNPS_SPI_SR) & 0x8) == 0);	// wait untill RX FIFO Not Empty
	ret = SNPS_SPI_READ(SNPS_SPI_DR);
	return ret;
}


/* ......................................................................... */

/*
 * Erase blocks of flash.
 */
static int haydn_spiflash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct dataflash_local *priv = (struct dataflash_local *) mtd->priv;
	volatile unsigned long status;
	unsigned long erase_addr ;
	int8_t   addr_4byte;

	DEBUG("$sfeb$\n") ;//Serial Flash Erase Begin

	/* Sanity checks */
	if (instr->addr + instr->len > mtd->size)
	{
		DEBUG("$sfee1$\n") ;//Serial Flash Erase End
		return -EINVAL;
	}
	if ((instr->len % (mtd->erasesize) != 0) || (instr->len % (priv->page_size) != 0))
	{
		DEBUG("$sfee2$\n") ;//Serial Flash Erase End
		return -EINVAL;
	}
	if ((instr->addr % (priv->page_size)) != 0)
	{
		DEBUG("$sfee3$\n") ;//Serial Flash Erase End
		return -EINVAL;
	}

	mutex_lock(&spiflash_mutex);

	while (instr->len > 0)
	{
		erase_addr = instr->addr ;
		DEBUG("erase_addr 0x%lx", erase_addr);

#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
		spi_chip_select = 0 ;
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
		if(erase_addr >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
			spi_chip_select = 1 ;
			erase_addr -= CONFIG_MTD_SPI_FLASH_NO1_SIZE ;
		}
		else {
			spi_chip_select = 0 ;
		}
#endif

		addr_4byte = enter_4_byte_mode(spi_chip_select);
		DEBUG("addr_4byte %d", addr_4byte);

		haydn_write_enable();
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
		SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x71c7);
		SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts
		SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);	// set BaudR
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
		SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_SE);
		if (addr_4byte)
			SNPS_SPI_WRITE(SNPS_SPI_DR, (erase_addr & 0xFF000000) >> 24);
		SNPS_SPI_WRITE(SNPS_SPI_DR, (erase_addr & 0x00FF0000) >> 16);
		SNPS_SPI_WRITE(SNPS_SPI_DR, (erase_addr & 0x0000FF00) >> 8);
		SNPS_SPI_WRITE(SNPS_SPI_DR, erase_addr & 0x000000FF);

		SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);

		do
		{
			status = SNPS_SPI_READ(SNPS_SPI_SR);
		}while (!(status & SNPS_SPI_SR_TFE));
		do
		{
			status = SNPS_SPI_READ(SNPS_SPI_SR);
		}while (status & SNPS_SPI_SR_BUSY);
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);

		while(haydn_spiflash_status() & 0x1)
		{
			msleep(25);
		} // wait while WriteInProgress

		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);                      // disable slave
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);           //disable SSI

		instr->addr += priv->page_size;		/* next page */
		instr->len -= priv->page_size;
	}

	mutex_unlock(&spiflash_mutex);

	/* Inform MTD subsystem that erase is complete */
	instr->state = MTD_ERASE_DONE;
	if (instr->callback)
	{
		instr->callback(instr);
	}

	DEBUG("$sfee5$\n") ;//Serial Flash Erase End
	return 0;

}


static int haydn_spiflash_read_CPUMODE(loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	volatile unsigned long status;
	unsigned long nSize, length, data_num;
	unsigned long i;

	unsigned short tmpdata;
	unsigned short* tmpbuf;
	unsigned long odd_len = 0;

	unsigned long read_addr ;
	int8_t   addr_4byte;

	*retlen = 0;

	tmpbuf = (unsigned short *)buf;
	do{
		nSize = (len > 16*2) ? 16*2 : len; // FIFO depth is 16. FIFO width is 16 bits (2 byte).

		read_addr = (unsigned long)from ;

#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
		spi_chip_select = 0 ;
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
		if(read_addr >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
			spi_chip_select = 1 ;
			read_addr -= CONFIG_MTD_SPI_FLASH_NO1_SIZE ;
		}
		else if((read_addr + nSize) >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
			//[jam] : If this READ will cross the boundary in the process,
			//we force it to read only 1st chip in this turn.
			//i.e. In this case, from is the addr of chip "1st"
			nSize = CONFIG_MTD_SPI_FLASH_NO1_SIZE - read_addr ;
			spi_chip_select = 0 ;
		}
		else {
			spi_chip_select = 0 ;
		}
#endif

		addr_4byte = enter_4_byte_mode(spi_chip_select);

		if (nSize & 0x1)
			odd_len = 1;
		else
			odd_len = 0;

		// config spi
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
		SNPS_SPI_WRITE(SNPS_SPI_DMACR, 0x0);		// disable DMA
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
		SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts

		// In FAST_READ case, we should transmit 5 bytes (one byte FAST_READ command, 3 bytes address, and one dummy byte)
		// to TX FIFO. Because the FIFO width is 2 bytes, there's one more garbage byte being sent into FIFO in this case.
		// That would cause the first byte read from serial flash to be dropped by SPI (in EEPROM read mode).
		// In the event, the read data will shift one byte. So we workaround this case by substracting the "from" address by one byte.
		// If the "from" address is zero, we would take this as a special case and handle this by READ command rather than FAST_READ.
		SNPS_SPI_WRITE(SNPS_SPI_BAUDR, (read_addr) ? 0x0004 : 0x0004);	// set BaudR (4 for READ command, 2 for FAST_READ command)
		SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x73cF);	// DFS = 16 bit
		// Because the DFS is 16 bits (2 byte), data number should be an even number.
		// If the data number is an odd, we would take this as a special case and handle this case by reading one more data.
		if (!odd_len)
			SNPS_SPI_WRITE(SNPS_SPI_CTRLR1,  (nSize>>1)-1);
		else //if (!odd_len)
			SNPS_SPI_WRITE(SNPS_SPI_CTRLR1,  ((nSize+1)>>1)-1);
		SNPS_SPI_WRITE(SNPS_SPI_RXFTLR, 0x3);

		// enable SSIC and send read command, address
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
		if (read_addr || addr_4byte)
		{
			unsigned long a = addr_4byte ? read_addr : (read_addr-1);
			int shifts = addr_4byte ? 24 : 16;
			tmpdata = (SPI_FLASH_INS_FAST_READ<<8) | ((a >> shifts) & 0xFF);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			shifts -= 16;
			tmpdata = (a >> shifts) & 0x0000FFFF;
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			SNPS_SPI_WRITE(SNPS_SPI_DR, (unsigned short)(a << 8));           // including dummy data
		}
		else //if (from)
		{
			tmpdata = (SPI_FLASH_INS_READ<<8) | ((read_addr & 0x00FF0000)>>16);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			tmpdata = read_addr & 0x0000FFFF;
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
		}
		// enable SER
		SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);

		// wait for SPI TX FIFO empty
		do
		{
			status = SNPS_SPI_READ(SNPS_SPI_SR);
		}while (!(status & SNPS_SPI_SR_TFE));

		//  RX
		length = (odd_len) ? (nSize-1) : nSize;
		while(length > 0)
		{
			data_num = SNPS_SPI_READ(SNPS_SPI_RXFLR);
			for (i = 0; (i<data_num) && (length>0); i++)
			{
				tmpdata = SNPS_SPI_READ(SNPS_SPI_DR);
				// The 16 bit data stored in FIFO is arranged by Big-Endian.
				// We swapped it to be Little-Endian.
				*(tmpbuf++) = ((tmpdata&0xFF)<<8) | ((tmpdata&0xFF00)>>8);
				length = length - 2;
			}
		}
		if (odd_len)
		{
			while(!(SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE));
			// If the data num is an odd number, we will read one more data for the 16 bits FIFO width.
			// So we drop the redundant data here.
			*((u_char *)tmpbuf++) = (u_char)(SNPS_SPI_READ(SNPS_SPI_DR)>>8);
		}
		len -= nSize;
		from += nSize;
		*retlen += nSize;
	}while (len > 0);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave

	return 0 ;
}

/*
 * Read from the DataFlash device.
 *   from   : Start offset in flash device
 *   len    : Amount to read
 *   retlen : About of data actually read
 *   buf    : Buffer containing the data
 */
static unsigned long descAddrArray[50] ;
static int haydn_spiflash_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	volatile unsigned long status;
	unsigned long nSize, data_num;
	unsigned long i, alignment;

	unsigned short tmpdata;
	unsigned long odd_len = 0;

	unsigned long DMATransfer, CPUTransfer = 0, openRXDMA = 0, DMADestAddr, dmaSize, dmaTurn = 0;
	unsigned long *descAddr = NULL;
	unsigned long descLen = 0;
	volatile unsigned long flags;

	unsigned long read_addr ;
	int8_t   addr_4byte;


	DEBUG("#sfrb#\n") ;//Serial Flash Read Begin

	*retlen = 0;
	// Sanity checks
	if (!len) {
		DEBUG("$sfre1$\n") ;//Serial Flash Read End
		return 0;
	}
	if (from + len > mtd->size) {
		DEBUG("$sfre2$\n") ;//Serial Flash Read End
		return -EINVAL;
	}

	mutex_lock(&spiflash_mutex);

#ifdef CONFIG_SPIFLASH_CPUMODE_OPERATION_ONLY
	haydn_spiflash_read_CPUMODE(from, len, retlen, buf) ;
#else
	dmac_flush_range(buf, (buf+len-1));

	// [note]: DMA is 2-bytes aligned now.
	// Because address of "buf" may not be 2 byte aligned, we should take care of the non-2byte-aligned case.
	alignment = 4-(((unsigned long)buf)&0x3);
	if((alignment != 4) && (len >= 16*2)) {
		haydn_spiflash_read_CPUMODE( from, alignment, retlen, buf);
		// Update the related variables.
		buf += alignment;
		len -= alignment;
		from += alignment;
	}

	do {
		nSize = (len > (64*1024*2)) ? (64*1024*2) : len; // SNPS SPI CTRL1 : maximum value is 64K(data number). FIFO width is 16 bits (data size).

		read_addr = (unsigned long)from ;

#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
		spi_chip_select = 0 ;
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
		if(read_addr >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
			spi_chip_select = 1 ;
			read_addr -= CONFIG_MTD_SPI_FLASH_NO1_SIZE ;
		}
		else if((read_addr + nSize) >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
			//[jam] : If this READ will cross the boundary in the process,
			//we force it to read only 1st chip in this turn.
			//i.e. In this case, from is the addr of chip "1st"
			nSize = CONFIG_MTD_SPI_FLASH_NO1_SIZE - read_addr ;
			spi_chip_select = 0 ;
		}
		else {
			spi_chip_select = 0 ;
		}
#endif

		addr_4byte = enter_4_byte_mode(spi_chip_select);

		odd_len = nSize & 0x1;

		local_irq_save(flags);

		// config spi
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
		SNPS_SPI_WRITE(SNPS_SPI_DMACR, 0x0);		// disable DMA
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
		SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts
		// In FAST_READ case, we should transmit 5 bytes (one byte FAST_READ command, 3 bytes address, and one dummy byte)
		// to TX FIFO. Because the FIFO width is 2 bytes, there's one more garbage byte being sent into FIFO in this case.
		// That would cause the first byte read from serial flash to be dropped by SPI (in EEPROM read mode).
		// In the event, the read data will shift one byte. So we workaround this case by substracting the "from" address by one byte.
		// If the "from" address is zero, we would take this as a special case and handle this by READ command rather than FAST_READ.
		SNPS_SPI_WRITE(SNPS_SPI_BAUDR, (read_addr) ? 0x0004 : 0x0004);       // set BaudR (4 for READ command, 2 for FAST_READ command)
		SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x73cF); // DFS = 16 bit
		// Because the DFS is 16 bits (2 byte), data number should be an even number.
		// If the data number is an odd, we would take this as a special case and handle this case by reading one more data.
		if (!odd_len)
			SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, (nSize>>1)-1);
		else //if (!odd_len)
			SNPS_SPI_WRITE(SNPS_SPI_CTRLR1, ((nSize+1)>>1)-1);
		SNPS_SPI_WRITE(SNPS_SPI_RXFTLR, 31); //RxThreshold

		DMATransfer = nSize ;
		CPUTransfer = DMATransfer % (16*2); // CPU transmit, burst: INCR16
		DMATransfer -= CPUTransfer;	// DMA transmit
		openRXDMA = (DMATransfer == 0) ? 0 : 1;
		DMADestAddr = virt_to_bus((void *)buf) ;
		dmaTurn = 0;
		if (openRXDMA) {
			descLen = 16 * (DMATransfer / (0xFFF*16*2)); // burst: INCR16
			if (descLen > 0) {
				//descAddr = kmalloc(descLen, GFP_ATOMIC);
				descAddr = descAddrArray ;
			}
			while (DMATransfer > 0) {
				dmaSize = (DMATransfer > (0xFFF*16*2)) ? (0xFFF*16*2) : DMATransfer; // APB DMA maximum transfer unit, burst: INCR16
				DMATransfer -= dmaSize;
				if (dmaTurn == 0) {
					SNPS_APBC_WRITE(APBC_SPIRX_DMA_SRC_ADDR, VPL_SPIC_BASE + SNPS_SPI_DR);
					SNPS_APBC_WRITE(APBC_SPIRX_DMA_DES_ADDR, DMADestAddr);
					SNPS_APBC_WRITE(APBC_SPIRX_DMA_LLP, (DMATransfer) ? virt_to_bus(descAddr) : 0x0);
					SNPS_APBC_WRITE(APBC_SPIRX_DMA_CTRL, 0x714B5 | (dmaSize/(16*2))<<20);//handle 16 bits FIFO width, burst: INCR16
				}
				else { // prepare LLP
					descAddr[4*(dmaTurn-1)+3] = 0x714B5 | (dmaSize/(16*2))<<20; //CTRL, burst: INCR16
					descAddr[4*(dmaTurn-1)+2] = (DMATransfer) ? (virt_to_bus(descAddr)+16*dmaTurn) : 0x0; //LLP
					descAddr[4*(dmaTurn-1)+1] = DMADestAddr; //DEST
					descAddr[4*(dmaTurn-1)] = VPL_SPIC_BASE + SNPS_SPI_DR; //SRC
				}
				DMADestAddr += dmaSize;
				dmaTurn++;
			}
			SNPS_SPI_WRITE(SNPS_SPI_DMARDLR, 0xF); //burst:INCR16
			if (descLen > 0) {
				dmac_flush_range(descAddr, descAddr + descLen);
			}
		}

		// enable SSIC and send read command, address
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);
		if (read_addr || addr_4byte)
		{
			unsigned long a = addr_4byte ? read_addr : (read_addr-1);
			int shifts = addr_4byte ? 24 : 16;
			tmpdata = (SPI_FLASH_INS_FAST_READ<<8) | ((a >> shifts) & 0xFF);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			shifts -= 16;
			tmpdata = (a >> shifts) & 0x0000FFFF;
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			SNPS_SPI_WRITE(SNPS_SPI_DR, (unsigned short)(a << 8));           // including dummy data
		}
		else { //if (from) read from address 0x0, we use READ instruction
			tmpdata = (SPI_FLASH_INS_READ<<8) | ((read_addr & 0x00FF0000)>>16);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			tmpdata = read_addr & 0x0000FFFF;
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
		}

		// enable SSIC RX DMA
		SNPS_SPI_WRITE(SNPS_SPI_DMACR, 0x1);
		// enable SSIC RX FIFO Full Interrupt, evelyn added at 05.16.09
		SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x10);
		// enable slave
		SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);

		// wait for SPI TX FIFO empty
		do {
			status = SNPS_SPI_READ(SNPS_SPI_SR);
		}while (!(status & SNPS_SPI_SR_TFE));

		local_irq_restore(flags);
		//  RX
		if (openRXDMA) {
			wait_for_completion(&spiflash_rx_complete);
			//kfree(descAddr);
			descAddr = NULL ;
		}
		SNPS_SPI_WRITE(SNPS_SPI_DMACR, 0x0);		// disable dma
		SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// disable SSIC RX FIFO Full Interrupt, evelyn added at 05.16.09
		//handle 16 bits and odd data number
		//if (openRXDMA)
		//	dmac_inv_range((unsigned long)buf, (unsigned long)(buf + nSize - CPUTransfer-1));

		buf = buf + nSize - CPUTransfer;
		CPUTransfer = (odd_len) ? (CPUTransfer-1) : CPUTransfer;

		while(CPUTransfer > 0) {
			data_num = SNPS_SPI_READ(SNPS_SPI_RXFLR);
			for (i = 0; (i < data_num) && (CPUTransfer > 0); i++) {
				tmpdata = SNPS_SPI_READ(SNPS_SPI_DR);
				// The 16 bit data stored in FIFO is arranged by Big-Endian.
				// We swapped it to be Little-Endian.
				*(buf++) = (tmpdata&0xFF00)>>8;
				*(buf++) = tmpdata&0xFF;
				CPUTransfer = CPUTransfer - 2;
			}
		}

		if (odd_len) {
			while(!(SNPS_SPI_READ(SNPS_SPI_SR) & SNPS_SPI_SR_RFNE));
			// If the data num is an odd number, we will read one more data for the 16 bits FIFO width.
			// So we drop the redundant data here.
			*(buf++) = (u_char)(SNPS_SPI_READ(SNPS_SPI_DR)>>8);
		}
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI

		len -= nSize;
		from += nSize;
		*retlen += nSize;
	}while (len > 0);
#endif

	mutex_unlock(&spiflash_mutex);

	DEBUG("$sfre4$\n") ;//Serial Flash Read End

	return 0 ;
}

static  void haydn_write_enable()
{
	volatile unsigned long status;

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
	SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x71c7);
	SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts
	SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);	// set BaudR

	SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);//0x08
	SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_WREN);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);//0x10

	do
	{
		status = SNPS_SPI_READ(SNPS_SPI_SR);
	}while (!(status & SNPS_SPI_SR_TFE));
	do
	{
		status = SNPS_SPI_READ(SNPS_SPI_SR);
	}while (status & SNPS_SPI_SR_BUSY);

	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
}


/*
 * haydn_spiflash_write_CPUMODE()
 *
 *               | 4-byte address mode | 3-byte address mode |
 *   ------------+---------------------+---------------------+
 *   odd length  |  16-bit width FIFO  |  8-bit width FIFO   |
 *   ------------+---------------------+---------------------+
 *   even length |  8-bit width FIFO   |  16-bit width FIFO  |
 *   ------------+---------------------+---------------------+
 */
static int haydn_spiflash_write_CPUMODE(loff_t to, size_t len, size_t *retlen, const u_char *buf)
{
	unsigned long nSize, i;
	volatile unsigned long status;
	unsigned short tmpdata;
	unsigned long odd_len = 0;
	int8_t   addr_4byte;

#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
        spi_chip_select = 0 ;
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
        if(to >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
            spi_chip_select = 1 ;
            to -= CONFIG_MTD_SPI_FLASH_NO1_SIZE ;
        }
        else {
            spi_chip_select = 0 ;
        }
#endif

	addr_4byte = enter_4_byte_mode(spi_chip_select);
	DEBUG("addr_4byte %d", addr_4byte);

    //printk("cpu() to = 0x%08lx, len = %d, retlen = %d\n", (unsigned long)to, len, *retlen) ;

	//tmpbuf = (unsigned short*)buf;
	do {
#ifdef CONFIG_SPIFLASH_CPUMODE_OPERATION_ONLY
        // We need to send 4 bytes(3-byte address) / 5 bytes(4-byte address) into FIFO for PP command.
		// For the FIFO depth is 16, and FIFO width is 2 bytes, there are only 28/27 bytes to fill data in FIFO.
		nSize = len > (addr_4byte ? 27 : 14*2) ? (addr_4byte ? 27 : 14*2) : len;
		// Page_Program command only supports one page inside programming.
		// If we want to write between pages, we have to send PP command in each page program.
		if ((nSize+(to&255)) > 256)
			nSize = 256-(to&255);

		odd_len = (nSize&0x1) ? 1 : 0;
#else
		nSize = len;

		odd_len = nSize & 0x1;
#endif
		haydn_write_enable();

		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
		SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, (odd_len ^ addr_4byte) ? 0x71c7 : 0x71cF);
		SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts
		SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);	// set BaudR
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);

		if (odd_len ^ addr_4byte) { /* 8-bit width */
			// We coudn't write odd number of data to 2 byte width FIFO.
			// Therefore, we write one byte to 8bit width configuration FIFO in this case.
			nSize = 1;	// only write one byte
			// send PP command and address
			SNPS_SPI_WRITE(SNPS_SPI_DR, SPI_FLASH_INS_PP);
			DEBUG("to 0x%llx", to);
			if (addr_4byte)
				SNPS_SPI_WRITE(SNPS_SPI_DR, (to & 0xFF000000) >> 24);
			SNPS_SPI_WRITE(SNPS_SPI_DR, (to & 0x00FF0000) >>16);
			SNPS_SPI_WRITE(SNPS_SPI_DR, (to & 0x0000FF00) >>8);
			SNPS_SPI_WRITE(SNPS_SPI_DR, to & 0x000000FF);
			/* TODO: why fill only 1 byte ????? */
			// send data
			DEBUG("data: 0x%02x", *buf);
			SNPS_SPI_WRITE(SNPS_SPI_DR, *buf++);
		}
		else { /* 16-bit width */
			// send PP command and address
			int shifts = addr_4byte ? 24 : 16;
			tmpdata = (SPI_FLASH_INS_PP << 8) | ((to >> shifts) & 0xFF);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			DEBUG("tmpdata 0x%04x", tmpdata);
			shifts -= 16;
			tmpdata = (to >> shifts) & 0x0000FFFF;
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			DEBUG("tmpdata 0x%04x", tmpdata);
			if (addr_4byte)
			{
				tmpdata = (unsigned short)(to << 8) | *buf++;
				SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);   // including 1 byte data
				DEBUG("tmpdata 0x%04x", tmpdata);
				nSize--;
				len--;
				to++;
				(*retlen)++;
			}

			// send data
			for (i = 0; i < nSize; i = i + 2)
			{
				tmpdata = ((*buf++) << 8);
				tmpdata |= *buf++;
				SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);
			}
		}
		SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);
		do {
			status = SNPS_SPI_READ(SNPS_SPI_SR);
		}while (!(status & SNPS_SPI_SR_TFE));
		do {
			status = SNPS_SPI_READ(SNPS_SPI_SR);
		}while (status & SNPS_SPI_SR_BUSY);
		while(haydn_spiflash_status() & 0x1);  			// wait while WriteInProgress
		SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
		SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI

		to += nSize;
		len -= nSize;
		*retlen += nSize;
        //printk(" len = %d, retlen = %d\n", len, *retlen) ;
	} while (len > 0);
	SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave

	return 0 ;
}

/*
 * Write to the DataFlash device.
 *   to     : Start offset in flash device
 *   len    : Amount to write
 *   retlen : Amount of data actually written
 *   buf    : Buffer containing the data
 *
 *                   | 4-byte address mode | 3-byte address mode |
 *   ----------------+---------------------+---------------------+
 *   src address     |     PIO 1 byte      |         DMA         |
 *   2-byte aligned  |   + DMA             |                     |
 *   ----------------+---------------------+---------------------+
 *   src address NOT |     preload 1 byte  |      PIO 1 byte     |
 *   2-byte aligned  |   + DMA             |    + DMA            |
 *   ----------------+---------------------+---------------------+
 */
static int haydn_spiflash_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf)
{
	unsigned long nSize;
	volatile unsigned long status;
	unsigned long flags;

	unsigned short tmpdata;
	unsigned long DMATransfer = 0, CPUTransfer = 0 ;
	//unsigned long i;

	unsigned long write_addr ;
	int8_t   addr_4byte, preload;

	DEBUG("$sfwb$\n") ;//Serial Flash Write Begin


	*retlen = 0;

	if (!len) {
		DEBUG("$sfwe1$\n") ;//Serial Flash Write End
		return 0;
	}
	if (to + len > mtd->size) {
		DEBUG("$sfwe2$\n") ;//Serial Flash Write End
		return -EINVAL;
	}
	if ((unsigned long)buf & 0x1)
		DEBUG("buf address is not 2 bytes aligned..\n");

	mutex_lock(&spiflash_mutex);

#ifdef CONFIG_SPIFLASH_CPUMODE_OPERATION_ONLY
	haydn_spiflash_write_CPUMODE(to, len, retlen, buf);
#else
	dmac_flush_range(buf, (buf + len -1));
	do{
		write_addr = (unsigned long)to ;
		DEBUG("to 0x%llx", to);

#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
		spi_chip_select = 0 ;
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
		if(write_addr >= CONFIG_MTD_SPI_FLASH_NO1_SIZE) {
			spi_chip_select = 1 ;
			write_addr -= CONFIG_MTD_SPI_FLASH_NO1_SIZE ;
		}
		else {
			spi_chip_select = 0 ;
		}
#endif

		addr_4byte = enter_4_byte_mode(spi_chip_select);
		DEBUG("addr_4byte %d", addr_4byte);
		preload = 0;

		// Page_Program command only supports one page inside programming.
		// If we want to write between pages, we have to send PP command in each page program.
		nSize = len;
		if ((nSize+(write_addr&255)) > 256) {
			nSize = 256 - (write_addr&255);
		}

		DEBUG("phy src buf: 0x%lx", virt_to_bus((void *)buf));
		/* alignment of source address */
		if (addr_4byte ^ (virt_to_bus((void *)buf) & 0x1))
		{
			haydn_spiflash_write_CPUMODE(to, 1, retlen, buf);
			buf++;
			to++;
			write_addr++;
			len--;
			nSize--;
			DEBUG("write_addr 0x%08lx", write_addr);
			DEBUG("nSize 0x%08lx", nSize);
		}

		//else if (addr_4byte && (virt_to_bus((void *)buf) & 0x1))
		if (len && addr_4byte)
		{
			/* Preload 1 byte into FIFO for DMA */
			preload = 1;
			buf++;
			to++;
			len--;
			nSize--;
			(*retlen)++;
			DEBUG("to 0x%08llx", to);
			DEBUG("nSize 0x%08lx", nSize);
		}

		DMATransfer = nSize ;
		CPUTransfer = nSize%(16*2);	// data_size = 2 bte, INCR=16
		DMATransfer -= CPUTransfer;
		if ( DMATransfer != 0 ) {
			int shifts = addr_4byte ? 24 : 16;

			local_irq_save(flags);

			// prepare APBC DMA
			SNPS_APBC_WRITE(APBC_SPITX_DMA_SRC_ADDR, virt_to_bus((void *)buf));
			SNPS_APBC_WRITE(APBC_SPITX_DMA_DES_ADDR, VPL_SPIC_BASE + SNPS_SPI_DR);
			SNPS_APBC_WRITE(APBC_SPITX_DMA_CTRL, 0x71175 | (DMATransfer/(16*2))<<20);
			SNPS_APBC_WRITE(APBC_SPITX_DMA_LLP, 0x0); // if use linked list description
			SNPS_SPI_WRITE(SNPS_SPI_DMATDLR, 0x10);
			haydn_write_enable();

			SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI
			SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
			SNPS_SPI_WRITE(SNPS_SPI_CTRLR0, 0x71cF);	// data_size = 16bit
			SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// mask all interrupts
			SNPS_SPI_WRITE(SNPS_SPI_TXFTLR, 0x1);		// TFE is asserted when TXFLR=1, evelyn added at 05.16.09
			SNPS_SPI_WRITE(SNPS_SPI_BAUDR, 0x0004);	// set BaudR
			SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x1);

			// send PP command and address
			tmpdata = (SPI_FLASH_INS_PP << 8) | ((write_addr >> shifts) & 0xFF);
			DEBUG("tmpdata 0x%04x", tmpdata);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);

			shifts -= 16;
			tmpdata = (write_addr >> shifts) & 0x0000FFFF;
			DEBUG("tmpdata 0x%04x", tmpdata);
			SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);

			if (preload) /* 4-byte mode + src buffer not 2-byte alignment */
			{
				tmpdata = (unsigned short)(write_addr << 8) | *(buf-1);
				SNPS_SPI_WRITE(SNPS_SPI_DR, tmpdata);   // including 1 byte data
				DEBUG("tmpdata 0x%04x", tmpdata);
			}

			SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x1);			// TFE interrupt enable, evelyn added at 05.16.09

			SNPS_SPI_WRITE(SNPS_SPI_DMACR, 0x2);
			SNPS_SPI_WRITE(SNPS_SPI_SER, 1 << spi_chip_select);

			local_irq_restore(flags);
			wait_for_completion(&spiflash_tx_complete);
			SNPS_SPI_WRITE(SNPS_SPI_IMR, 0x0);			// TFE interrupt disable, evelyn added at 05.16.09

			do {
				status = SNPS_SPI_READ(SNPS_SPI_SR);
			}while (!(status & SNPS_SPI_SR_TFE));
			do {
				status = SNPS_SPI_READ(SNPS_SPI_SR);
			}while (status & SNPS_SPI_SR_BUSY);

			while(haydn_spiflash_status() & 0x1);			// wait while WriteInProgress

			SNPS_SPI_WRITE(SNPS_SPI_DMACR, 0x0);		// disable dma
			SNPS_SPI_WRITE(SNPS_SPI_SER, 0x0);			// disable slave
			SNPS_SPI_WRITE(SNPS_SPI_SSIENR, 0x0);		//disable SSI

			//update the related settings
			buf += DMATransfer ;
			len -= DMATransfer ;
			*retlen += DMATransfer ;
			to += DMATransfer ;
		}
		else if (preload == 1) /* forget the preloaded 1 byte */
		{
			buf--;
			to--;
			len++;
			nSize++;
			(*retlen)--;
			CPUTransfer++;
		}

		//handle odd length
		if ( CPUTransfer != 0 ) {
			haydn_spiflash_write_CPUMODE( to, CPUTransfer, retlen, buf);
			buf += CPUTransfer ;
			len -= CPUTransfer;
			to += CPUTransfer ;
		}

	} while (len > 0);
#endif

	mutex_unlock(&spiflash_mutex);

	DEBUG("$sfwe4$\n") ;//Serial Flash Write End

	return 0 ;
}

/* ......................................................................... */

/*
 * Initialize and register DataFlash device with MTD subsystem.
 */
static int __init add_dataflash(int channel, char *name, int IDsize,
		int nr_pages, int pagesize, int pageoffset)
{
	struct mtd_info *device;
	struct dataflash_local *priv;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *mtd_parts = 0;
	int mtd_parts_nr = 0;
#endif

	if (nr_devices >= DATAFLASH_MAX_DEVICES) {
		//printk(KERN_ERR "at91_dataflash: Too many devices detected\n");
		return 0;
	}

	device = kmalloc(sizeof(struct mtd_info) + strlen(name) + 8, GFP_KERNEL);
	if (!device)
		return -ENOMEM;
	memset(device, 0, sizeof(struct mtd_info));

	sprintf((char *)&device[1], "%s.spi%d", name, channel);
	device->name = (char *)&device[1];
	device->size = nr_pages * pagesize;
	device->erasesize = pagesize;
	device->writesize = pagesize;

	device->owner = THIS_MODULE;
	device->type = MTD_NORFLASH;
	device->flags = MTD_CAP_NORFLASH;
	device->erase = haydn_spiflash_erase;
	device->read = haydn_spiflash_read;
	device->write = haydn_spiflash_write;

	priv = (struct dataflash_local *) kmalloc(sizeof(struct dataflash_local), GFP_KERNEL);
	if (!priv) {
		kfree(device);
		return -ENOMEM;
	}
	memset(priv, 0, sizeof(struct dataflash_local));

	priv->spi = channel;
	priv->page_size = pagesize;
	priv->page_offset = pageoffset;
	device->priv = priv;

	mtd_devices[nr_devices] = device;
	nr_devices++;
	//printk("haydn_spiflash: %s detected [spi%i] (%i bytes)\n", name, channel, device->size);

#ifdef CONFIG_MTD_SPI_FLASH_NUM_1
	printk("We have 1 Serial Flash!\n") ;
#elif defined(CONFIG_MTD_SPI_FLASH_NUM_2)
	printk("We have 2 Serial Flashes!\n") ;
#endif

#ifdef CONFIG_MTD_PARTITIONS
#if 0
	mtd_parts_nr = parse_mtd_partitions(device, part_probes, &mtd_parts, 0);
#endif
	if (mtd_parts_nr <= 0) {
#ifndef CONFIG_MTD_SPI_FLASH
		switch (IDsize) {
/*
			case SZ_2M:
				mtd_parts = static_partitions_2M;
				mtd_parts_nr = ARRAY_SIZE(static_partitions_2M);
				break;
			case SZ_4M:
				mtd_parts = static_partitions_4M;
				mtd_parts_nr = ARRAY_SIZE(static_partitions_4M);
				break;
*/
			case SZ_8M:
				mtd_parts = static_partitions_8M;
				mtd_parts_nr = ARRAY_SIZE(static_partitions_8M);
				break;
		}
#else
		printk(KERN_DEBUG "Using the default nand partition..\n") ;
        mtd_parts = board_sf_default_partition;
        mtd_parts_nr = ARRAY_SIZE(board_sf_default_partition);
#endif /* CONFIG_MTD_SPI_FLASH */
	}

	if (mtd_parts_nr > 0) {
#ifdef DATAFLASH_ALWAYS_ADD_DEVICE
		add_mtd_device(device);
#endif
		return add_mtd_partitions(device, mtd_parts, mtd_parts_nr);
	}
#endif
	return add_mtd_device(device);		/* add whole device */
}

/*
 * Detect and initialize DataFlash device connected to specified SPI channel.
 *
 *   Device            Density         ID code                 Nr Pages        Page Size       Page offset
 *   AT45DB011B        1Mbit   (128K)  xx0011xx (0x0c)         512             264             9
 *   AT45DB021B        2Mbit   (256K)  xx0101xx (0x14)         1025            264             9
 *   AT45DB041B        4Mbit   (512K)  xx0111xx (0x1c)         2048            264             9
 *   AT45DB081B        8Mbit   (1M)    xx1001xx (0x24)         4096            264             9
 *   AT45DB0161B       16Mbit  (2M)    xx1011xx (0x2c)         4096            528             10
 *   AT45DB0321B       32Mbit  (4M)    xx1101xx (0x34)         8192            528             10
 *   AT45DB0642        64Mbit  (8M)    xx1111xx (0x3c)         8192            1056            11
 *   AT45DB1282        128Mbit (16M)   xx0100xx (0x10)         16384           1056            11
 */
static int __init haydn_spiflash_detect(int channel)
{
	int res;

#ifdef CONFIG_MTD_SPI_FLASH
    res = add_dataflash(channel,
                        "SPI Flash",
                        CONFIG_MTD_SPI_FLASH_MAP_WINDOW_SIZE,
                        CONFIG_MTD_SPI_FLASH_MAP_PAGE_NUM,
                        CONFIG_MTD_SPI_FLASH_MAP_PAGE_SIZE,
                        0);
#else
    res = add_dataflash(channel, "SPI Flash", SZ_8M, 128, 65536, 0);
#endif

	return res;
}

static int __init haydn_spiflash_init(void)
{
	/* DataFlash (SPI chip select 0) */
	haydn_spiflash_detect(0);
	mutex_init(&spiflash_mutex);

    INIT_COMPLETION(spiflash_tx_complete) ;
    INIT_COMPLETION(spiflash_rx_complete) ;

	return 0;
}

static void __exit haydn_spiflash_exit(void)
{
	int i;

	for (i = 0; i < DATAFLASH_MAX_DEVICES; i++) {
		if (mtd_devices[i]) {
#ifdef CONFIG_MTD_PARTITIONS
			del_mtd_partitions(mtd_devices[i]);
#else
			del_mtd_device(mtd_devices[i]);
#endif
			kfree(mtd_devices[i]->priv);
			kfree(mtd_devices[i]);
		}
	}
	nr_devices = 0;
	kfree(spi_transfer_desc);
}


module_init(haydn_spiflash_init);
module_exit(haydn_spiflash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("evelyn");
MODULE_DESCRIPTION("SPIFlash driver for VIATICS SoC");
