/*
 * (C) Copyright 2011-2013 VN Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation's version 2 of
 * the License.
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

//#define DEBUG

#include <config.h>
#include <common.h>
#include <asm/types.h>
#include <asm/io.h>
#include <mmc.h>
#include "dw_mmc.h"
#include <asm/arch/platform.h>

#define MMC_SEND_CMD_RETRY 100

#ifdef DEBUG_MMC
static void
dump_regs(dw_mmc_host_t *host)
{
	printf("\thost addr: %p\n", host);
	printf("\tCTRL:    0x%08x\n", readl(&host->ctrl));
	printf("\tPWREN:   0x%08x\n", readl(&host->pwren));
	printf("\tCLKDIV:  0x%08x\n", readl(&host->clkdiv));
	printf("\tCLKSRC:  0x%08x\n", readl(&host->clksrc));
	printf("\tCLKENA:  0x%08x\n", readl(&host->clkena));
	printf("\tTMOUT:   0x%08x\n",  readl(&host->tmout));
	printf("\tCTYPE:   0x%08x\n",  readl(&host->ctype));
	printf("\tCMDARG:  0x%08x\n", readl(&host->cmdarg));
	printf("\tCMD:     0x%08x\n", readl(&host->cmd));
	printf("\tRINTSTS: 0x%08x\n", readl(&host->rintsts));
	printf("\tMINTSTS: 0x%08x\n", readl(&host->mintsts));
	printf("\tSTATUS:  0x%08x\n", readl(&host->status));
}
#else
#define dump_regs(x)
#endif

int SD_Card_Detect(int print_msg)
{
    unsigned long reg = readl(VPL_MSHC_0 + 0x50) ;
    if ( reg == 0 ) {
        if(print_msg) 
            printf("  Card Status  : SD card exists!\n") ;                
    }
    else {
        if(print_msg)
            printf("  Card Status  : SD card does not exist!\n" ) ;
        return 0 ;
    }

    return 1 ;
}

static void
update_clock(dw_mmc_t *vmmc)
{
	writel(CMD_START_CMD | CMD_UPDATE_CLOCK_REGISTERS_ONLY
	     | CMD_CARD_NUMBER(vmmc->cardnum) | CMD_INDEX(MMC_CMD_GO_IDLE_STATE)
	      , &vmmc->host->cmd);
}



static int
mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	uint   bcnt = 0;
	dw_mmc_host_t *   host = ((dw_mmc_t *)mmc->priv)->host;
	unsigned int   cmdreg = 0;
	unsigned int   rintsts;
	int resp_timeout_retry_counter = 0 ;

	u32 from, cur;
#define INT_CD_TIMEOUT  2000 /* ms */

	debug("[CMD %d] ARG 0x%08x\n", cmd->cmdidx, cmd->cmdarg);

retry_cmd:	
	resp_timeout_retry_counter++ ;	

	writel(0xffffffff, &host->rintsts);
	writel(cmd->cmdarg, &host->cmdarg);

	/* assemble CMD register */
	cmdreg |= (cmd->resp_type & (MMC_RSP_CRC | MMC_RSP_136 | MMC_RSP_PRESENT)) << 6;
	if (data)
	{
		writel(data->blocksize, &host->blksiz);
		bcnt = data->blocksize * data->blocks;
		writel(bcnt, &host->bytcnt);
		cmdreg |= CMD_DATA_EXPECTED
		        | CMD_BLOCK_XFER
		        | (data->flags == MMC_DATA_READ ? CMD_READ_CARD : CMD_WRITE_CARD);

		debug("\tblksiz %d, bytcnt %d\n", data->blocksize, data->blocksize * data->blocks);
	}
	debug("\tcmdreg 0x%08x\n", cmdreg);
	writel(CMD_START_CMD
	     | CMD_CARD_NUMBER(mmc->block_dev.dev)
	     | cmdreg
	     | CMD_INDEX(cmd->cmdidx)
	      , &host->cmd);

	/* wait for command done */
	from = get_timer(0);
	for (;;)
	{
		rintsts = readl(&host->rintsts);
		if (rintsts & INT_HLE)
		{
			dump_regs(host);
			return -1; /* TODO: error code */
		}
		if (rintsts & INT_CD)
			break;
		cur = get_timer(from);
		if (cur > INT_CD_TIMEOUT)
		{
			dump_regs(host);
			return cur;
		}
	}
	writel(INT_CD, &host->rintsts);

	/* copy response */
	if (cmd->resp_type & MMC_RSP_PRESENT)
	{
		if (rintsts & (INT_RTO | INT_RCRC | INT_RE))
		{
				dump_regs(host);
				if((rintsts & INT_RTO) && (resp_timeout_retry_counter != MMC_SEND_CMD_RETRY))
					goto retry_cmd ;

				return -1;  /* TODO: error code */
		}
		if (cmd->resp_type & MMC_RSP_136)
		{
			/* MMC framework require cmd->response[0] as MSB */
			cmd->response[0] = readl(&host->resp3);
			cmd->response[1] = readl(&host->resp2);
			cmd->response[2] = readl(&host->resp1);
			cmd->response[3] = readl(&host->resp0);

			debug("\tresponse 0: 0x%08x\n", cmd->response[0]);
			debug("\tresponse 1: 0x%08x\n", cmd->response[1]);
			debug("\tresponse 2: 0x%08x\n", cmd->response[2]);
			debug("\tresponse 3: 0x%08x\n", cmd->response[3]);
		}
		else
		{
			cmd->response[0] = readl(&host->resp0);
			debug("\tresponse 0: 0x%08x\n", cmd->response[0]);
		}
	}


	/* TODO: status check */
	/* TODO: status busy status of R1b response */

	/* non-DMA */
	if (data)
	{
		int i;
		u32 *mem = (u32 *)(data->dest); /* either src or dest */

		from = get_timer(0);
		do
		{
			rintsts = readl(&host->rintsts);

			if (data->flags == MMC_DATA_READ && (rintsts & INT_RXDR))
			{
				i = STATUS_FIFO_COUNT(readl(&host->status));
				debug("\tread fifo cnt %d\n", i);

				/* TODO: sanity check between FIFO width and block size */
				//for (i = 0; i < FIFO_RX_THRESHOLD + 1; i++)
				for (; i > 0 ; i--)
					*mem++ = readl(&host->data);
				writel(INT_RXDR, &host->rintsts);
			}

			if (data->flags == MMC_DATA_WRITE && (rintsts & INT_TXDR))
			{
				i = FIFO_DEPTH - STATUS_FIFO_COUNT(readl(&host->status));
#ifdef DEBUG
				int j = i ;
#endif				
				/* TODO: sanity check between FIFO width and block size */
				for (; i > 0 && bcnt; i--, bcnt -= 4)
					writel(*mem++, &host->data);
				debug("\twrite fifo cnt %d\n", j - i);							
				udelay(5000) ;
				writel(INT_TXDR, &host->rintsts);
			}
		} while (!(rintsts & (INT_DTO | INT_HTO)));

		if (rintsts & INT_DRTO)
		{
			dump_regs(host);
			return -1;
		}

		if (data->flags == MMC_DATA_READ && rintsts & INT_DTO)
		{
			/* copy remaining data in FIFO */
			i = STATUS_FIFO_COUNT(readl(&host->status));
			debug("\tread remaining fifo cnt %d\n", i);
			for (; i > 0 ; i--)
				*mem++ = readl(&host->data);
		}
		
		writel(INT_DTO, &host->rintsts);
	}


	return 0;
}



static void
mmc_set_ios(struct mmc *mmc)
{
	dw_mmc_t *vmmc = mmc->priv;
	dw_mmc_host_t *host = vmmc->host;
	unsigned int ctype;


	/* set bus width */
	ctype = readl(&host->ctype);
	ctype &= ~(CTYPE_WIDTH_MASK << vmmc->cardnum);	/* default to 1-bit */
	switch (mmc->bus_width)
	{
		case 8:
			ctype |= (CTYPE_8_BIT << vmmc->cardnum);
			break;
		case 4:
			ctype |= (CTYPE_4_BIT << vmmc->cardnum);
			break;
		default:
			mmc->bus_width = 1;
	}
	writel(ctype, &host->ctype);

	/* set bus clock */
	if (mmc->clock == 0)
		mmc->clock = 390625;
	writel(MMC_REFERENCE_CLOCK / (mmc->clock*2), &host->clkdiv);
	writel(readl(&host->clkena) | CLKENA_CCLK_ENABLE(vmmc->cardnum), &host->clkena);
	update_clock(vmmc);
}



static int
mmc_init_setup(struct mmc *mmc)
{
	dw_mmc_t *vmmc = mmc->priv;
	dw_mmc_host_t *host = vmmc->host;


	/* power off && on */
	writel(readl(&host->pwren) & ~(1 << vmmc->cardnum), &host->pwren);
	udelay(100);
	writel(readl(&host->pwren) | (1 << vmmc->cardnum), &host->pwren);

	dump_regs(host);
	return 0;
}



void
reset_mmc_host(dw_mmc_host_t *host)
{
	unsigned long ctrl;


	/* disable power to all cards */
	writel(0, &host->pwren);

	/* reset controller/fifo/dma */
	writel(readl(&host->ctrl) | CTRL_FIFO_RESET | CTRL_DMA_RESET | CTRL_CONTROLLER_RESET
	       , &host->ctrl);
	while ((ctrl = readl(&host->ctrl) & (CTRL_FIFO_RESET | CTRL_DMA_RESET | CTRL_CONTROLLER_RESET)) != 0);

	/* disable interrupt */
	writel(ctrl & ~CTRL_INT_ENABLE, &host->rintsts);

	/* set interrupt mask */
	writel(0xffffffff, &host->intmask);
	writel(0xffffffff, &host->rintsts);

	/* timeout, max. value */
	writel(0xffffffff, &host->tmout);

	/* disable clock */
	writel(0x0, &host->clkena);
	/* clock source default to divider 0 */
	writel(0x0, &host->clksrc);
	/* setup clock divider (only divider 0 is valid) */
	writel((MMC_REFERENCE_CLOCK / (390625* 2)) << 0, &host->clkdiv);

	/* FIFO threshold */
	writel(FIFOTH_RX_WMARK(FIFO_RX_THRESHOLD) | FIFOTH_TX_WMARK(FIFO_TX_THRESHOLD),
	       &host->fifoth);
}



int
dw_mmc_init(struct mmc *mmc)
{
	sprintf(mmc->name, "DW SD/MMC");
	mmc->send_cmd = mmc_send_cmd;
	mmc->set_ios = mmc_set_ios;
	mmc->init = mmc_init_setup;

	/* TODO: check the following initialization */
	mmc->voltages = MMC_VDD_35_36 | MMC_VDD_34_35 | MMC_VDD_33_34 | MMC_VDD_32_33
	              | MMC_VDD_31_32 | MMC_VDD_30_31 | MMC_VDD_29_30 | MMC_VDD_28_29
	              | MMC_VDD_27_28;
	mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS;

	mmc->f_min = 400000;
	mmc->f_max = 25000000;

	mmc_register(mmc);

	return 0;
}

