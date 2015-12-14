/*
 * Copyright (C) 2007-2013  VN Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/clk.h>
#include <asm/arch/platform.h>
#include "dw_spi.h"


#define FIFO_DEPTH   (32)
#define DMA_BURST_LEN            (16)

struct dw_spi_regs {
	uint32_t	ctrlr0;		/* 0x00 */
	uint32_t	ctrlr1;		/* 0x04 */
	uint32_t	ssienr;		/* 0x08 */
	uint32_t	mwcr;		/* 0x0c */
	uint32_t	ser;		/* 0x10 */
	uint32_t	baudr;		/* 0x14 */
	uint32_t	txftlr;		/* 0x18 */
	uint32_t	rxftlr;		/* 0x1c */
	uint32_t	txflr;		/* 0x20 */
	uint32_t	rxflr;		/* 0x24 */
	uint32_t	sr;			/* 0x28 */
	uint32_t	imr;        /* 0x2c */
	uint32_t	isr;		/* 0x30 */
	uint32_t	risr;		/* 0x34 */
	uint32_t	txoicr;		/* 0x38 */
	uint32_t	rxoicr;		/* 0x3c */
	uint32_t	rxuicr;		/* 0x40 */
	uint32_t	msticr;		/* 0x44 */
	uint32_t	icr;		/* 0x48 */
	uint32_t	dmacr;		/* 0x4c */
	uint32_t	dmatdlr;    /* 0x50 */
	uint32_t	dmardlr;    /* 0x54 */
	uint32_t	idr;		/* 0x58 */
	uint32_t	rsv;        /* 0x5c */
	uint32_t	dr;         /* 0x60 */
};


struct dw_spi_slave
{
	struct spi_slave         slave;
	struct dw_spi_regs * regs;
	u32                      sckdv; /* SCLK divider, must be even */
	u8                       mode;  /* Mode0~3 */
};


u16  _txfifo[5]; /* for 2-byte alignment */
u8   *TxFifo = (u8 *)_txfifo; /* byte[0-1]: length, byte[2]: command, byte[3~max]: parameters */


/* Internal functions */
static inline struct dw_spi_slave * to_dw_spi(struct spi_slave *);
static inline void dw_spi_disable(struct dw_spi_slave *);
static int dw_spi_read(struct dw_spi_slave *, unsigned int, u8 *, unsigned long);
static int dw_spi_write(struct dw_spi_slave *, unsigned int, const u8 *, unsigned long);


void spi_init()
{
    return ;
}



int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return (bus == 0 && cs < SSI_NUM_SLAVES);
}



struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct dw_spi_slave	*ds;


	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ds = malloc(sizeof(*ds));
	if (!ds)
		return NULL;

	ds->slave.bus = bus;
	ds->slave.cs = cs;
	ds->mode = mode;
	ds->sckdv = ((APB_CLOCK / max_hz) + 1) & ~1UL; /* must be even */
	ds->regs = (struct dw_spi_regs *)SSI_MMR_BASE;

	return &ds->slave ;
}



void spi_free_slave(struct spi_slave *slave)
{
	struct dw_spi_slave *ds = to_dw_spi(slave);

	free(ds);
}

int spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
}

/*
 * from Davinci_spi.c
 */
int spi_xfer(struct spi_slave *slave,
             unsigned int bitlen,
             const void *dout,
             void *din,
             unsigned long flags)
{
	struct dw_spi_slave *ds = container_of(slave, struct dw_spi_slave, slave);
	unsigned int len;


	/*
	 * It's not clear how non-8-bit-aligned transfers are supposed to be
	 * represented as a stream of bytes...this is a limitation of
	 * the current SPI interface - here we terminate on receiving such a
	 * transfer request.
	 */
	if (bitlen % 8)
		goto out;

	if (flags & SPI_XFER_BEGIN)
	{
		ds->regs->ssienr = 0;
		ds->regs->ser = 0;
		ds->regs->baudr = ds->sckdv;
		ds->regs->ctrlr0 = SSIC_CTRLR0_TMOD((flags & DW_SPI_XFER_MODE) >> 2)
		                 | SSIC_CTRLR0_MODE(SPI_MODE_3)
		                 | SSIC_CTRLR0_DFS(flags & DW_SPI_DSF_16 ? 16 : 8);
		ds->regs->txftlr= 1;
		ds->regs->rxftlr = FIFO_DEPTH - 1;
		ds->regs->imr = SSIC_IMR_RXFIM | SSIC_IMR_TXEIM;
		ds->regs->dmatdlr = DMA_BURST_LEN;
		ds->regs->dmardlr = DMA_BURST_LEN - 1;
		ds->regs->dmacr = 0;
	}

	len = bitlen / 8;

#ifndef CONFIG_CMD_SPI
	if (dout == NULL)
		return dw_spi_read(ds, len, din, flags);
	else if (din == NULL)
		return dw_spi_write(ds, len, dout, flags);

	printf("SPI full-duplex not implemented\n");
#else
	{
		int ret;

		if (dout)
		{
			ret = dw_spi_write(ds, len, dout, flags);
			if (ret)
				return ret;
		}
		if (din)
		{
			ret = dw_spi_read(ds, len, din, flags);
			if (ret)
				return ret;
		}

		if (dout || din)
			return ret;
	}
#endif

out:
	flags |= SPI_XFER_END;
	if (flags & SPI_XFER_END)
		dw_spi_disable(ds);

	return 1;
}


static inline struct dw_spi_slave *
to_dw_spi(struct spi_slave *slave)
{
	return container_of(slave, struct dw_spi_slave, slave);
}


static inline void
dw_spi_disable(struct dw_spi_slave *ds)
{
	writel(0, &ds->regs->ssienr);
	writel(0, &ds->regs->ser);
	writel(SSIC_CTRLR0_MODE(ds->mode), &ds->regs->ctrlr1);
	writel(0, &ds->regs->dmacr);
	writel(0, &ds->regs->imr);
}


static int
dw_spi_read(struct dw_spi_slave *ds, unsigned int len, u8 *rxp, unsigned long flags)
{
	u32   ctrlr0 = readl(&ds->regs->ctrlr0);
	u8    dfs = ((ctrlr0 & SSIC_CTRLR0_DFS(16)) + 1) >> 3;
	u32   dma_cycles = (len >> (dfs == 1 ? 0 : 1)) / DMA_BURST_LEN;
	u8    eeprom_read = (ctrlr0 & SSIC_CTRLR0_TMOD(0x3)) == SSIC_CTRLR0_TMOD(CTRLR0_EEPROM_READ);
	struct apb_dma_desc *   llp = NULL; /* backup to be freed */
	int   fr, ret = 0;


	debug("len:%d, flags:0x%lx, ctrlr0:0x%x, dfs:%d\n", len, flags, ctrlr0, dfs);

	if (dfs == 0x2 && (u32)rxp & 0x1)
	{
		printf("addr 0x%p must be aligned to %d byte\n", rxp, dfs);
		ret = len;
		goto out;
	}

	/* TODO: check read length (max. 0xffff) */
	fr = len >> (dfs == 1 ? 0 : 1);
	if ((len & 1) && dfs == 2)
		fr++;
	writel(fr - 1, &ds->regs->ctrlr1);
	if (fr > SSIC_CTRLR1_NDF_MAX)
	{
		printf("exceeds max. frame count %d \n", SSIC_CTRLR1_NDF_MAX);
		return -1;
	}

	/* setup DMA engine */
	if (dma_cycles)
	{
		u32 cycles = dma_cycles;
		volatile struct apb_dma_desc * desc =
			(struct apb_dma_desc *)APBC_DMA_n_SRC_ADDR(APBC_DMA_CHANNEL_SSIC_RX);


		debug("DMA mode: %d cycles\n", dma_cycles);

		/* setup APBC DMA descriptors */
		do
		{
			u32   cyc;

			cyc = cycles > DMA_CYC_MAX ? DMA_CYC_MAX : cycles;

			desc->src = (void *)&ds->regs->dr;
			desc->des = rxp;
			desc->llp = NULL;
			desc->ctrl = DMA_CTRL_CYC(cyc)
			           | DMA_CTRL_DATA_SZ(dfs == 1 ? SZ_8_BITS: SZ_16_BITS)
			           | DMA_CTRL_BYTE_SWAP(SWAP_YES)
			           | DMA_CTRL_REQ_TYPE(1)
			           | DMA_CTRL_REQ_SEL(1)
			           | DMA_CTRL_DES_ADDR_INC(ADDR_POSITIVE_INC)
			           | DMA_CTRL_SRC_ADDR_INC(ADDR_NO_INC)
			           | DMA_CTRL_TRANS_TYPE(TRANS_APB2AHB)
			           | DMA_CTRL_BURST_MODE(BURST_16)
			           | DMA_CTRL_INTR_OP_EN;
			debug("desc->des: 0x%p\n", desc->des);
			debug("desc->ctrl: 0x%x\n", desc->ctrl);

			rxp += cyc * DMA_BURST_LEN * dfs;
			if ((cycles -= cyc) == 0)
				break;

			desc->llp = malloc(sizeof(struct apb_dma_desc));
			desc = desc->llp;
			debug("desc->llp: 0x%p\n", desc);
			if (llp == NULL)
				llp = (struct apb_dma_desc *)desc; /* head of linked descriptors to be freed */
			if (desc == NULL)
			{
				debug("OOM!\n");
				apb_free_dmadesc(llp);
				ret = cycles;
				goto out;
			}
		} while (cycles);

		writel(SSIC_DMACR_RDMAE, &ds->regs->dmacr);
		len -= (dma_cycles * dfs * DMA_BURST_LEN);
	}

	if (flags & SPI_XFER_BEGIN || eeprom_read)
	{
		uint32_t imr = readl(&ds->regs->imr);

		debug("Enable SSIC\n");
		writel(1, &ds->regs->ssienr);
		writel(1 << ds->slave.cs, &ds->regs->ser);

#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b
		if (eeprom_read)  /* write command after enable SSIC */
		{
			void * txf = TxFifo + 2;
			u16 l= *(u16 *)TxFifo;

			/* ugly workaround */
			if (dfs == 2 && (l & 1)) // 16-bit mode & odd length of command/parameters
			{
				/* TxFifo has been converted 16-bit value in dw_spi_write */
				u32 offset;
				debug("TxFifo[2]:%04x, TxFifo[4]:%04x, TxFifo[6]:%04x\n",
						*(u16 *)(TxFifo+2), *(u16 *)(TxFifo+4), *(u16 *)(TxFifo+6));
				switch (TxFifo[3])
				{
					case CMD_READ_ARRAY_FAST: // 1cmd + 3addrs + 1dummy
						offset = (TxFifo[2]<<16) + *(u16 *)(TxFifo+4);
						if (offset)
						{
							offset--;
							TxFifo[2] = offset >> 16; // addr1
							*(u16 *)(TxFifo+4) = (u16)offset; // addr2,addr3
						}
						else
						{
							l = *(u16 *)TxFifo = 4;
							TxFifo[3] = CMD_READ_ARRAY_SLOW;
						}
						break;
					case CMD_READ_ARRAY_SLOW: // 1cmd + 4addrs
						offset = (TxFifo[2]<<24) + ((*(u16 *)(TxFifo+4))<<8) + TxFifo[7];
						if (offset)
						{
							offset--;
							TxFifo[2] = offset >> 24; // addr1
							*(u16 *)(TxFifo+4) = offset >> 8; // addr2,addr3
							TxFifo[7] = offset; // addr4
						}
						else
						{
							l = *(u16 *)TxFifo = 6;
							TxFifo[2] = CMD_READ_ARRAY_FAST;
						}
						break;
					default:
						printf("16-bit mode & odd length of command/parameters\n");
						return -EINVAL;
				}
				debug("TxFifo[2]:%04x, TxFifo[4]:%04x, TxFifo[6]:%04x\n",
						*(u16 *)(TxFifo+2), *(u16 *)(TxFifo+4), *(u16 *)(TxFifo+6));
			}

			while (l--)
			{
				if (dfs == 1)
				{
					debug("EEPROM_READ: write 0x%02x (%d)\n", *(u8 *)txf, l+1);
					writel(*(u8 *)txf++, &ds->regs->dr);
				}
				else
				{
					debug("EEPROM_READ: write 0x%04x (%d)\n", *(u16 *)txf, l+1);
					writel(*(u16 *)txf, &ds->regs->dr);
					txf += sizeof(u16);
					if (l) l--;
				}
			}
			*(u16 *)TxFifo = 0; /* clear length to avoid writing again */

			writel(imr & ~SSIC_IMR_TXEIM, &ds->regs->imr); /* un-gate SCLK for Tx */
		}
	}

	/* DMA mode */
	if (dma_cycles)
	{
		debug("wait DMA completion...");
		/* TODO: timeout */
		while ((readl(APBC_DMA_n_CTRL(APBC_DMA_CHANNEL_SSIC_RX)) & DMA_CTRL_INTR_CMPT) == 0)
		{
			debug("apbc dma ctrl:%#x\n", readl(APBC_DMA_n_CTRL(APBC_DMA_CHANNEL_SSIC_RX)));
			debug("apbc dma src:%#x\n", readl(APBC_DMA_n_SRC_ADDR(APBC_DMA_CHANNEL_SSIC_RX)));
			debug("apbc dma dest:%#x\n", readl(APBC_DMA_n_DES_ADDR(APBC_DMA_CHANNEL_SSIC_RX)));
			debug("apbc dma llp:%#x\n", readl(APBC_DMA_n_LLP(APBC_DMA_CHANNEL_SSIC_RX)));
			debug("ctrlr0:%#x\n", ds->regs->ctrlr0);
			debug("ctrlr1:%#x\n", ds->regs->ctrlr1);
			debug("ssienr:%#x\n", ds->regs->ssienr);
			debug("ser:%#x\n", ds->regs->ser);
			debug("baudr:%#x\n", ds->regs->baudr);
			debug("txflr:%#x\n", ds->regs->txflr);
			debug("rxflr:%#x\n", ds->regs->rxflr);
			debug("sr:%#x\n", ds->regs->sr);
			debug("imr:%#x\n", ds->regs->imr);
			debug("risr:%#x\n", ds->regs->risr);
			debug("dmacr:%#x\n", ds->regs->dmacr);
		};
		debug(" done\n");
		apb_free_dmadesc(llp);
	}

	/* CPU mode */
	while (len--)
	{
		u16 data;

		/* TODO: timeout mechanism */
		while (readl(&ds->regs->rxflr) == 0)
		{
			/*
			debug("ctrlr0:%x\n", ds->regs->ctrlr0);
			debug("ctrlr1:%x\n", ds->regs->ctrlr1);
			debug("ssienr:%x\n", ds->regs->ssienr);
			debug("ser:%x\n", ds->regs->ser);
			debug("baudr:%x\n", ds->regs->baudr);
			debug("txflr:%x\n", ds->regs->txflr);
			debug("rxflr:%x\n", ds->regs->rxflr);
			debug("sr:%x\n", ds->regs->sr);
			debug("imr:%x\n", ds->regs->imr);
			debug("risr:%x\n", ds->regs->risr);
			debug("dmacr:%x\n", ds->regs->dmacr);
			*/
		}

		data = readl(&ds->regs->dr);

		if (dfs == 2) /* 16-bit width */
		{
			debug("CPU read 0x%04x (%d)\n", data, len+1);
			*rxp++ = (u8)(data >> 8);
			if (len) len--;
		}
		else
		{
			debug("CPU read 0x%02x (%d)\n", (u8)data, len+1);
		}
		*rxp = (u8)data;
		rxp++;
	}

out:
	if (ret)
		flags |= SPI_XFER_END;

	if (flags & SPI_XFER_END)
	{
		writel(0, &ds->regs->imr); /* SCLK ungating */
		/* TODO: timeout machanism */
		while ((readl(&ds->regs->sr) & (SSIC_SR_BUSY | SSIC_SR_TFE)) != SSIC_SR_TFE);
		dw_spi_disable(ds);
	}

	return ret;
}

static int
dw_spi_write(struct dw_spi_slave *ds, unsigned int len, const u8 *txp, unsigned long flags)
{
	u32   ctrlr0 = readl(&ds->regs->ctrlr0);
	u8    dfs = ((ctrlr0 & SSIC_CTRLR0_DFS(16)) + 1) >> 3;
	u32   dma_cycles = len / dfs / DMA_BURST_LEN;
	u8    eeprom_read = (ctrlr0 & SSIC_CTRLR0_TMOD(0x3)) == SSIC_CTRLR0_TMOD(CTRLR0_EEPROM_READ);
	struct apb_dma_desc *   llp = NULL; /* backup to be freed */
	int   ret = 0;
	void *  txf = TxFifo;


	debug("len:%d, flags:0x%lx, ctrlr0:0x%x, dfs:%d\n", len, flags, ctrlr0, dfs);

	if (!eeprom_read && dfs == 0x2 && (u32)txp & 0x1)
	{
		printf("addr 0x%p must be aligned to %d byte\n", txp, dfs);
		ret = len;
		goto out;
	}

	/* setup DMA engine */
	if (dma_cycles)
	{
		u32 cycles = dma_cycles;
		volatile struct apb_dma_desc * desc =
			(struct apb_dma_desc *)APBC_DMA_n_SRC_ADDR(APBC_DMA_CHANNEL_SSIC_TX);


		debug("DMA mode: %d cycles\n", dma_cycles);

		/* setup APBC DMA descriptors */
		do
		{
			u32   cyc;

			cyc = cycles > DMA_CYC_MAX ? DMA_CYC_MAX : cycles;

			desc->src = (void *)txp;
			desc->des = (void *)&ds->regs->dr;
			desc->llp = NULL;
			desc->ctrl = DMA_CTRL_CYC(cyc)
			           | DMA_CTRL_DATA_SZ(dfs == 1 ? SZ_8_BITS: SZ_16_BITS)
			           | DMA_CTRL_BYTE_SWAP(SWAP_YES)
			           | DMA_CTRL_REQ_TYPE(1)
			           | DMA_CTRL_REQ_SEL(1)
			           | DMA_CTRL_DES_ADDR_INC(ADDR_NO_INC)
			           | DMA_CTRL_SRC_ADDR_INC(ADDR_POSITIVE_INC)
			           | DMA_CTRL_TRANS_TYPE(TRANS_AHB2APB)
			           | DMA_CTRL_BURST_MODE(BURST_16)
			           | DMA_CTRL_INTR_CMPT_EN
			           | DMA_CTRL_INTR_OP_EN;

			txp += cyc * DMA_BURST_LEN * dfs;
			if ((cycles -= cyc) == 0)
				break;

			desc->llp = malloc(sizeof(struct apb_dma_desc));
			desc = desc->llp;
			if (llp == NULL)
				llp = (struct apb_dma_desc *)desc; /* head of linked descriptors to be freed */
			if (desc == NULL)
			{
				debug("OOM!\n");
				apb_free_dmadesc(llp);
				ret = cycles;
				goto out;
			}
		} while (cycles);

		writel(SSIC_DMACR_TDMAE, &ds->regs->dmacr);
		len -= (dma_cycles * dfs * DMA_BURST_LEN);
	}

	if (flags & SPI_XFER_BEGIN && !eeprom_read)
	{
		debug("Enable SSIC\n");
		writel(1, &ds->regs->ssienr);
		writel(1 << ds->slave.cs, &ds->regs->ser);
	}

	/* DMA Mode */
	if (dma_cycles)
	{
		debug("wait DMA completion\n");
		/* TODO: timeout */
		while ((readl(APBC_DMA_CHN_MONITER) & (1 << APBC_DMA_CHANNEL_SSIC_TX)) == 0)
		{
			/*
			debug("apbc dma ctrl:%#x\n", readl(APBC_DMA_n_CTRL(APBC_DMA_CHANNEL_SSIC_TX)));
			debug("ctrlr0:%#x\n", ds->regs->ctrlr0);
			debug("ctrlr1:%#x\n", ds->regs->ctrlr1);
			debug("ssienr:%#x\n", ds->regs->ssienr);
			debug("ser:%#x\n", ds->regs->ser);
			debug("baudr:%#x\n", ds->regs->baudr);
			debug("txflr:%#x\n", ds->regs->txflr);
			debug("rxflr:%#x\n", ds->regs->rxflr);
			debug("sr:%#x\n", ds->regs->sr);
			debug("imr:%#x\n", ds->regs->imr);
			debug("risr:%#x\n", ds->regs->risr);
			debug("dmacr:%#x\n", ds->regs->dmacr);
			*/
		};
		writel(~(1 << APBC_DMA_CHANNEL_SSIC_TX), APBC_DMA_CHN_MONITER);
		apb_free_dmadesc(llp);
	}

	/* pre-buffer EEPROM command without writing to data port */
	if (eeprom_read)
	{
		if ((len + 1) > sizeof(_txfifo))
			return len;
		*(u16 *)TxFifo = (u16)len; /* length of command/parameters */
		txf = TxFifo + 2;
		debug("length of cmd/para: %d\n", *(u16 *)TxFifo);
	}

	/* CPU mode */
	while (len--)
	{
		u16 data = *txp++;

		if (dfs == 2) /* 16-bit width */
		{
			data = (data << 8) + *txp++;
			if (len)
				len--;
		}

		if (!eeprom_read)
		{
			writel(data, &ds->regs->dr);
			debug("CPU write 0x%04x (%d)\n", data, len+1);
		}
		else if (dfs == 1)
			*(u8 *)txf++ = (u8)data;
		else
		{
			*(u16 *)txf = data;
			txf += sizeof(u16);
		}
	}

out:
	if (ret)
		flags |= SPI_XFER_END;

	if (flags & SPI_XFER_END)
	{
		writel(0, &ds->regs->imr); /* SCLK ungating */
		/* TODO: timeout machanism */
		while ((readl(&ds->regs->sr) & (SSIC_SR_BUSY | SSIC_SR_TFE)) != SSIC_SR_TFE);
		dw_spi_disable(ds);
	}

	return ret;
}
