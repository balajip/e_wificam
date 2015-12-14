/*
 * Serial Peripheral Interface (SPI) driver for the Atmel AT91RM9200
 *
 * (c) SAN People (Pty) Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#define SPI_MAJOR		153	/* registered device number */

#define DEFAULT_SPI_CLK		6000000


/* Maximum number of buffers in a single SPI transfer.
 *  DataFlash uses maximum of 2
 *  spidev interface supports up to 8.
 */
#define MAX_SPI_TRANSFERS	8
#define NR_SPI_DEVICES		4	/* number of devices on SPI bus */

/*
 * Describes the buffers for a SPI transfer.
 * A transmit & receive buffer must be specified for each transfer
 */
struct spi_transfer_list {
	void* tx[MAX_SPI_TRANSFERS];	/* transmit */
	int txlen[MAX_SPI_TRANSFERS];
	void* rx[MAX_SPI_TRANSFERS];	/* receive */
	int rxlen[MAX_SPI_TRANSFERS];
	int nr_transfers;		/* number of transfers */
	int curr;			/* current transfer */
};

struct spi_local {
	unsigned int pcs;		/* Peripheral Chip Select value */

	struct spi_transfer_list *xfers;	/* current transfer list */
	dma_addr_t tx, rx;		/* DMA address for current transfer */
	dma_addr_t txnext, rxnext;	/* DMA address for next transfer */
};


// char stream definition for 
typedef struct _structCharStream
{
	unsigned char* pChar;                                // buffer address that holds the streams
	int length;                               // length of the stream in bytes
}CharStream;

/* Exported functions */
#ifdef SPI_DEBUG
extern int haydn_spi_test_read(const CharStream* char_stream_send, CharStream* char_stream_recv);
#endif
#ifdef SPI_USE_DMA
extern int spi_transfer_dma(const CharStream* char_stream_send, CharStream* char_stream_recv);
#else
extern int spi_transfer(const CharStream* char_stream_send, CharStream* char_stream_recv);
#endif


/****************** for Haydn Synopsis SPI******************/
#include <asm/io.h>


#define SPI_USE_DMA
//#define SPI_USE_INTERRUPT

//#define SPI_DEBUG
//#define SPI_DEBUG_TMR
//#define SPI_DEBUG_INTERRUPT

#define HAYDN_SPIC_BASE			HAYDN_SSI_MMR_BASE
#define HAYDN_SPIC_CPU_BASE		0xF4080000
#define HAYDN_APBC_CPU_BASE		0xF9950000
#define SNPS_SPI_READ(reg)			__raw_readl(HAYDN_SPIC_CPU_BASE + (reg))
#define SNPS_SPI_WRITE(reg, val)		__raw_writel((val), HAYDN_SPIC_CPU_BASE + (reg))
#define SNPS_APBC_READ(reg)		__raw_readl(HAYDN_APBC_CPU_BASE + (reg))
#define SNPS_APBC_WRITE(reg,val)	__raw_writel((val), HAYDN_APBC_CPU_BASE + (reg))

#define SNPS_SPI_CTRLR0		0x0
#define SNPS_SPI_CTRLR1		0x04
#define SNPS_SPI_SSIENR		0x08
#define SNPS_SPI_MWCR		0x0C
#define SNPS_SPI_SER		0x10
#define SNPS_SPI_BAUDR		0x14
#define SNPS_SPI_TXFTLR		0x18
#define SNPS_SPI_RXFTLR		0x1C
#define SNPS_SPI_TXFLR		0x20
#define SNPS_SPI_RXFLR		0x24
#define SNPS_SPI_SR			0x28
#define SNPS_SPI_IMR		0x2C
#define SNPS_SPI_ISR		0x30
#define SNPS_SPI_RISR		0x34
#define SNPS_SPI_TXOICR		0x38
#define SNPS_SPI_RXOICR		0x3C
#define SNPS_SPI_RXUICR		0x40
#define SNPS_SPI_MSTICR		0x44
#define SNPS_SPI_ICR		0x48
#define SNPS_SPI_DMACR		0x4C
#define SNPS_SPI_DMATDLR	0x50
#define SNPS_SPI_DMARDLR	0x54
#define SNPS_SPI_IDR		0x58
#define SNPS_SPI_SSI_COMP_VERSION	0x5C
#define SNPS_SPI_DR			0x60

// SPI status
#define SNPS_SPI_SR_DCOL	0x40
#define SNPS_SPI_SR_TXE		0x20
#define SNPS_SPI_SR_RFF		0x10
#define SNPS_SPI_SR_RFNE	0x08
#define SNPS_SPI_SR_TFE		0x04
#define SNPS_SPI_SR_TFNF	0x02
#define SNPS_SPI_SR_BUSY	0x01

