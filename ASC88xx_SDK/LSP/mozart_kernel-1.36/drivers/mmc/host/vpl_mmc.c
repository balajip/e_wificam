/*
 * vpl_mmc
 * Driver for MSHC
 *
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
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/dma-mapping.h>
#include <linux/timer.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/pci.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/sizes.h>
#include <mach/mmc.h>
#include <asm/cacheflush.h>//flush_cache_all()
#include "vpl_mmc.h"
#define dma_alloc_consistent(d,s,p) pci_alloc_consistent(d,s,p)
#define dma_free_consistent(d,s,v,p)  pci_free_consistent(d,s,v,p)
#define DRIVER_NAME "mozart-sdhc"
#define POP_WRITE_READ_COMPARE 0
#define NON_CACHE 1
#define USE_SPIN 1
#define DMUTEX 1
#define MSHC_DMA 1
#define CHAIN_MODE	0
#define NR_SG 32
#define NR_HWSG NR_SG
#define NR_BUF 32
#define DMA_TRANSFER_SIZE	0x1000
#define DMA_TRANSFER_SIZE_N	0x1000
#define FIFO_DEPTH 16
#define BURST_SIZE	0
#if CHAIN_MODE
#define	DSL	0
#else
#define	DSL	0
#endif
/*
parameter
	write_protect:
		write_protect pin connect to physical socket or not. if write_protect=0, we ignore write_protect pin.
	clk_div:
		clock divider, you can check following table:
			clk_div value,	source clock divid value
				1					0x0
				2					0x1
				3					0x2
				4					0x3
				5					0x20
	pull_high_cmd:
		pull high resister on pin CMD enable or not
	pull_high_data:
		pull high resister on pin DATA0~3 enable or not
	pull_high_wpcd:
		pull high resister on pin WPCD enable or not
	cd_share_pin:
		card_detect and data[3] share pin or not
*/
static int write_protect = 1;
static int clk_div = 1;
static int pull_high_cmd = 1;
static int pull_high_data = 1;
static int pull_high_wpcd = 1;
static int cd_share_pin = 0;
const char id[] = "$MSHC: "DRIVER_VERSION" "PLATFORM" "__DATE__" $";

static void
vpl_sdhc_dma_done(struct vpl_sdhc_host * host,struct mmc_data * data);
static void
vpl_sdhc_xfer_done(struct vpl_sdhc_host *host, struct mmc_data *data);
static void vpl_sdhc_request(struct mmc_host *mmc, struct mmc_request *req);
/*-------- VPL function---------------*/
/**
  * Set the particular bits of the specified register.
  * @param[in] reg	The particular register to which the bits are to be set
  * @param[in] val	The bitmask for the bits which are to be set.
  * \return 	The new value of the register
  */
u32 vpl_set_bits(struct vpl_sdhc_host *host, Controller_Reg reg, u32 val)
{
	u32 *reg_addr ;
	reg_addr  = (u32 *)(host->virt_base + reg) ;

	*((volatile u32 *)reg_addr) |= val ;
	return *reg_addr ;
}


/**
  * Clear the particular bits of the specified register.
  * @param[in] reg	The particular register to which the bits are to be cleared.
  * @param[in] val	The bitmask for the bits which are to be cleared.
  * \return 	The new value of the register
  */
u32 vpl_clear_bits(struct vpl_sdhc_host *host, Controller_Reg reg, u32 val)
{
	u32 *reg_addr ;
	reg_addr  = (u32 *)(host->virt_base + reg) ;

	*((volatile u32 *)reg_addr) &= (~val) ;
	return *reg_addr ;
}


/**
  * Set the value of the specified register.
  * @param[in] reg	The particular register for which values are to be set.
  * @param[in] val	The value.
  * \return 	The new value of the register
  */
u32 vpl_set_register(struct vpl_sdhc_host *host, Controller_Reg reg, u32 val)
{
	u32 *reg_addr ;
	reg_addr  = (u32 *)(host->virt_base + reg) ;
	*((volatile u32 *)reg_addr) = val ;
	return 0 ;

}


/**
  * Read the value of the specified register.
  * @param[in] reg	The particular register which is to be read.
  * \return 	The value of the register
  */
u32 vpl_read_register(struct vpl_sdhc_host *host, Controller_Reg reg)
{
	u32 *reg_addr ;
	volatile u32 retval;

	reg_addr  = (u32 *)(host->virt_base + reg) ;
	retval = *((volatile u32 *)reg_addr);
	return retval ;
}

/**
  * Returns the address to the fifo.
  *
  */
void *vpl_get_fifo_address(struct vpl_sdhc_host *host) {
	void *retval;
	retval = (void *)(host->virt_base + FIFODAT);
	return retval;
}

void vpl_send_clock_only_cmd(struct vpl_sdhc_host *host)
{
	vpl_set_register(host, CMDARG, 0x0);
	vpl_set_register(host, CMD, 0x80200000);
}
/**
  * Enables all clocks to all card slots in the controller.
  */
u32 vpl_enable_all_clocks(struct vpl_sdhc_host *host)
{
	u32 clock_val;
	u32 clk_div_val;
	POSDIODEBUG("[pop]file= %s func = %s\n", __FILE__, __func__);

	clock_val = (1 << 1) - 1;
	clock_val |= 0x00010000;
	vpl_set_register(host, CLKENA, clock_val);
	switch (clk_div)
	{
		case 1:
			clk_div_val = 0x0;// 25Mhz/ 1 = 25 KHz			100%
			break;
		case 2:
			clk_div_val = 0x1;//25Mhz/ 0x1*2 = 12.5 MHz		50%
			break;
		case 3:
			clk_div_val = 0x2;// 25Mhz/ 0x2*2 = 6.25 MHz	25%
			break;
		case 4:
			clk_div_val = 0x3;//25Mhz/ 0x3*2 = 4.17 MHz		16.67%
			break;
		case 5:
			clk_div_val = 0x20;//25Mhz/ 0x20*2 = 390.63 KHz		1.56%
			break;
		default:
			clk_div_val = 0x0;// 25Mhz/ 1 = 25 MHz
			break;

	}
	vpl_set_register(host, CLKDIV, clk_div_val);
	vpl_send_clock_only_cmd(host);
	return 0; // poplar modify
}

static void vpl_sdhc_dma_reset(struct vpl_sdhc_host *host)
{
	u32 buffer_reg;
	unsigned long flags;
#if USE_SPIN
	spin_lock_irqsave(&host->lock, flags);
#endif
	disable_irq(host->irq);

	vpl_clear_bits(host, CTRL, IDMA_ENABLE);
	vpl_set_register(host, BMOD, 0x00000001);
	udelay(100);
	vpl_set_register(host, IDSTS, 0x1FFF);
	udelay(100);
	vpl_set_register(host, BMOD, 0x00000080|(BURST_SIZE<<8)|(DSL<<2));
	buffer_reg = vpl_read_register(host, CTRL);
	vpl_set_register(host, CTRL, (buffer_reg|0x4));// DMA reset
	wmb();
	do{
		buffer_reg = vpl_read_register(host, CTRL);
		udelay(100);
	}while((buffer_reg & 0x4) ==0x4);
	wmb();
	vpl_set_register(host, CTRL, (buffer_reg|0x2));// FIFO reset
	wmb();
	do{
		buffer_reg = vpl_read_register(host, CTRL);
		udelay(100);
	}while((buffer_reg & 0x2) ==0x2);
	wmb();
	vpl_set_bits(host, CTRL, IDMA_ENABLE);
	wmb();
#if USE_SPIN
		spin_unlock_irqrestore(&host->lock, flags);
#endif
	enable_irq(host->irq);
}

/**
  * The initialisation procedure for the whole host controller IP.
  * This function sets up all the values for the host controller to function.
  * A large part of the driver depends on the setup values which are used here.
  * The steps taken are;
  *	- Initialise the platform dependent segment of the driver. This maps the
  *	the device into OS memory/IO space which can be eventually used to access
  *	the registers for the device.
  *	- Determine the maximum number of cards/slots for the host controller.
  *	- Enable the power for the cards.
  *	- Enable the clock on all the interfaces.
  *	- Clear all pending interrupts.
  *	- Set up the interrupt map.
  *	- Setup the FIFO watermarks. FIFODEPTH/2 is setup as the watermark.
  *	- Setup a a large debouncing value.
  *	- The cards are then enumerated.
  */
u32 vpl_init_controller(struct vpl_sdhc_host *host)
{
	u32 buffer_reg = 0;	/* multipurpose buffer register */
	u32 num_of_cards, fifo_thresh;
	u32 retval = 0;
	int use_dma;
	use_dma = host->use_dma;

	/*Befoer proceeding further lets reset the hostcontroller IP
	  This can be achieved by writing 0x00000001 to CTRL register*/
	  buffer_reg = 0x4;
	  vpl_set_bits(host, CTRL,buffer_reg);
		wmb();
	  udelay(100);
	  buffer_reg = 0x00000002;//poplar modify for FIFO reset.
	  vpl_set_bits(host, CTRL,buffer_reg);
		wmb();
	  udelay(100);
	  buffer_reg = 0x00000007;//poplar modify for DMA, FIFO, Controller reset.
	  vpl_set_bits(host, CTRL, buffer_reg);
//		wmb();
	  udelay(100);

	  /* Now make CTYPE to default i.e, all the cards connected will work in 1 bit mode initially*/
	  buffer_reg = 0xffffffff;
	  vpl_clear_bits(host, CTYPE, buffer_reg);


	/* No. of cards supported by the IP */
	buffer_reg = vpl_read_register(host, HCON);
	num_of_cards = HCON_NUM_CARDS(buffer_reg);

	/* Power up only those cards/devices which are connected
	    - Shut-down the card/device once wait for some time
	    - Enable the power to the card/Device. wait for some time
	*/
	buffer_reg = (1 << num_of_cards) - 1;
	vpl_clear_bits(host, PWREN, buffer_reg);
	udelay(100);			/*some SDIO cards need more time to power up so changed from 10 to 1000*/
	vpl_set_register(host, PWREN, buffer_reg);
	udelay(100);

	/* Set up the interrupt mask.
 	   - Clear the interrupts in any pending Wrinting 1's to RINTSTS
	   - Enable all the interrupts other than ACD in INTMSK register
	   - Enable global interrupt in control register
	*/
	vpl_set_register(host, RINTSTS, 0xffffffff);
	if (use_dma)
	{
		buffer_reg = INTMSK_ALL_ENABLED & ~INTMSK_ACD & ~INTMSK_TXDR & ~INTMSK_RXDR &~INTMSK_SDIO_INTR;
	}
	else
	{
		buffer_reg = INTMSK_ALL_ENABLED & ~INTMSK_ACD &~INTMSK_SDIO_INTR;// poplar modify 20090110
	}
	vpl_set_register(host, INTMSK, buffer_reg);
	if (use_dma)
	{
		vpl_set_bits(host, CTRL, IDMA_ENABLE);
	}
	vpl_set_bits(host, CTRL, INT_ENABLE);

	/* Set Data and Response timeout to Maximum Value*/
	vpl_set_register(host, TMOUT, 0xffffffff);
//	vpl_set_register(host, TMOUT, 0xffffff40);

	if (use_dma)
	{
		vpl_sdhc_dma_reset(host);
	}

	/* Enable the clocks to the all connected cards/drives
	   - Note this command is to CIU of host controller ip
	   - the command is not sent on the command bus
	   - it emplys the polling method to accomplish the task
	   - it also emplys wait prev data complete in CMD register
        */
	vpl_enable_all_clocks(host);

	/* Set the card Debounce to allow the CDETECT fluctuations to settle down*/
	vpl_set_register(host, DEBNCE, DEFAULT_DEBNCE_VAL);

	/* update the global structure of the ip with the no of cards present at this point of time */

	/* Update the watermark levels to half the fifo depth
	   - while reset bitsp[27..16] contains FIFO Depth
	   - Setup Tx Watermark
	   - Setup Rx Watermark
        */
	fifo_thresh = FIFO_DEPTH/2;
	/* Tx Watermark */
	vpl_clear_bits(host, FIFOTH, 0xfff);
	vpl_set_bits(host, FIFOTH, fifo_thresh);
	/* Rx Watermark */
	fifo_thresh -= 1;
	vpl_clear_bits(host, FIFOTH, 0x7fff0000);
	vpl_set_bits(host, FIFOTH, fifo_thresh << 16);
	vpl_set_bits(host, FIFOTH, BURST_SIZE << 28);

	return retval;
}

/*-------- VPL function end------------*/

static void vpl_sdhc_stop_clock(struct vpl_sdhc_host *host)
{
}

static int vpl_sdhc_start_clock(struct vpl_sdhc_host *host)
{
	return 0;
}

static void vpl_sdhc_softreset(void)
{
}
void
vpl_sdhc_prepare_dma(struct vpl_sdhc_host *host, struct mmc_data *data)
{
	struct scatterlist *sg;
	u32 count, des_addr, buf_reg, phy_addr, dma_status;
	volatile mshc_idmac_info * pdescriptor;
	volatile mshc_idmac_info * ptmpdesc;
	int descript_number = 0;
	unsigned sg_len;
	unsigned int blockcount = data->blocks;
	unsigned int blksz = data->blksz;
	unsigned int bytecount = blockcount * blksz;
	unsigned int size = bytecount;
	int i;
	pdescriptor = host->pThis;
#if NON_CACHE
#else
	dmac_flush_range(host->pThis, (char*)(host->pThis)+NR_BUF*sizeof(mshc_idmac_info));
#endif
	ptmpdesc = pdescriptor;
	if (host->dma_sts == 1)
	{
		for (i=0; i< NR_SG; i++)
		{
			if((ptmpdesc->des0 & 0x4) != 0x4)
			{
				ptmpdesc++;
			}
			else
				break;
		}

#if 1
		if ((ptmpdesc->des0 & 0x4)== 0x4)
		{
			count = 6000;
			while(ptmpdesc->des0 & 0x80000000)
			{
#if NON_CACHE
#else
				dmac_flush_range(ptmpdesc, (char*)ptmpdesc+sizeof(mshc_idmac_info));
#endif
				count--;
				udelay(100);
				if (count == 0)
				{
				//	POSDIODEBUG("count == 0\n");
					vpl_init_controller(host);
					break;
				}
			}
		}
	}
	else
	{
		POSDIODEBUG("dma_sts = 0\n");
		host->dma_sts = 1;
	}
#endif
	dma_status = vpl_read_register(host, IDSTS);
	dma_status &= 0x0001E000;
	if (dma_status != 0x0)
	{
		POSDIODEBUG("dma status is not idle\n\n\n");
	}
	host->sg_idx = 0;
	sg_len = (data->blocks == 1) ? 1 : data->sg_len;
	sg = &data->sg[host->sg_idx];
	if (data->flags & MMC_DATA_WRITE)
		host->dma_data_dir = DMA_TO_DEVICE;
	else
		host->dma_data_dir = DMA_FROM_DEVICE;
	host->sg_len = dma_map_sg(mmc_dev(host->mmc), data->sg,
				sg_len, host->dma_data_dir);
	host->total_bytes_left = 0;
	if(host->sg_len == 0)
		POSDIODEBUG("host->sg_len = 0\n");
	while(host->sg_idx < host->sg_len)
	{
		sg = &data->sg[host->sg_idx];


/*
poplar 2010.12.28
*/



		count = min(size, sg_dma_len(sg));
		size -= count;
		if (sg_dma_len(sg) != count)
		{
			POSDIODEBUG("count(%d) and sg_dma_len(sg)(%d) are different\n", count, sg_dma_len(sg));
		}
		phy_addr = sg_dma_address(sg);
		if(size%32)
		{
			printk("Size is not align\n");
		}
		if (phy_addr%4)
		{

			POSDIODEBUG("Not align\n");
		}
		while (count >=	DMA_TRANSFER_SIZE_N)
		{
#if CHAIN_MODE
			pdescriptor->des0 = 0x80000010;
#else
			pdescriptor->des0 = 0x80000002;
#endif
			pdescriptor->des1 = DMA_TRANSFER_SIZE_N;
			pdescriptor->des2 = phy_addr;
			ptmpdesc = pdescriptor;
			pdescriptor++;
//			pdescriptor = bus_to_virt(pdescriptor->des3);
			phy_addr += DMA_TRANSFER_SIZE_N;
			count -= DMA_TRANSFER_SIZE_N;
			descript_number++;
		}
		if (count > 0 )
		{
			if(count%4)
			{
				POSDIODEBUG("count not align\n");
				count = count - count%4 + 4;
			}
			descript_number++;
#if CHAIN_MODE
			pdescriptor->des0 = 0x80000010;
#else
			pdescriptor->des0 = 0x80000002;
#endif
			pdescriptor->des1 = count;
			pdescriptor->des2 = phy_addr ;
			ptmpdesc = pdescriptor;
			phy_addr += DMA_TRANSFER_SIZE_N;
			pdescriptor++;
//			pdescriptor = bus_to_virt(pdescriptor->des3);
//			des_addr = vpl_read_register(host, BYTCNT);
		}
		host->sg_idx++;
	}
	ptmpdesc->des0 |= 0x4;
#if CHAIN_MODE
#else
	ptmpdesc->des0 &= ~0x2;
#endif
	pdescriptor = host->pThis;
	pdescriptor->des0 |= 0x8;
	host->data = data;
#if NON_CACHE
	des_addr = host->kmalloced_dma ;
#else
	des_addr = virt_to_bus(host->pThis);
#endif
#if CHAIN_MODE
	buf_reg = 0x210;
#else
	buf_reg = 0x210;
#endif
	wmb();
#if NON_CACHE
#else
	dmac_flush_range(host->pThis, host->pThis+sizeof(mshc_idmac_info)*(descript_number));
#endif

	vpl_set_register(host, IDINTEN, buf_reg);
	vpl_set_register(host, DBADDR, des_addr);
	vpl_set_register(host, BLKSIZ, blksz);
	vpl_set_register(host, BYTCNT, bytecount);
//	mod_timer(&host->dma_abort_timer, jiffies + HZ*30);
}


/* A scatterlist segment completed */
#if SCHEDULE
#if TASKLET
static void vpl_sdhc_dma_cb(unsigned long param)
{
	struct vpl_sdhc_host *host = (struct vpl_sdhc_host *) param;
#else
static void vpl_sdhc_dma_cb(struct work_struct *work)
{
	struct vpl_sdhc_host *host = container_of(work, struct vpl_sdhc_host,
								dma_done_work);
#endif
#else
static void vpl_sdhc_dma_cb(struct vpl_sdhc_host *host)
{
#endif
	struct mmc_data *mdata = host->data;
	struct scatterlist *sg;
	int sg_idx = 0;
 	unsigned buf_reg;

	volatile mshc_idmac_info * pdescriptor;
	volatile mshc_idmac_info * ptmpdesc;
	volatile unsigned long timeout;
#if SCHEDULE
#if TASKLET
#else
	mutex_lock(&host->wq_mutex);
#endif
#endif
	if (host->data == NULL) {
#if SCHEDULE
#if TASKLET
#else
		mutex_unlock(&host->wq_mutex);
#endif
#endif
		return;
	}
	mdata->bytes_xfered = 0;
	timeout = 0;
	while(vpl_read_register(host, STATUS) & STATUS_DAT_BUSY_BIT)
	{
		udelay(100);
		timeout++;
		if(timeout > 1800000){ //3 mins: 3 * 60 (sec) * 1000 (ms)* 10 (100 us)
			printk("[vpl_mmc]SD card busy polling timeout!\n");
			break;
		}
	}

#if 1

#if NON_CACHE
#else
		dmac_flush_range(host->pThis, (char*)(host->pThis)+sizeof(mshc_idmac_info)*NR_BUF);
#endif
		pdescriptor = host->pThis;
		ptmpdesc = pdescriptor;
#if CHAIN_MODE
		while((ptmpdesc->des0 & 0x80000010) == 0x10)
#else
		while((ptmpdesc->des0 & 0x80000002) == 0x2)
#endif
		{
			if ((ptmpdesc->des0 & 0x4) != 0x4)
			{
				mdata->bytes_xfered += ptmpdesc->des1;
				ptmpdesc++;
//				ptmpdesc = bus_to_virt(ptmpdesc->des3);
			}
			else
				break;
#if NON_CACHE
			if (ptmpdesc->des3 == host->kmalloced_dma)
#else
			if (ptmpdesc->des3 == virt_to_bus(host->pThis))
#endif
				break;
		}
		if (((ptmpdesc->des0 & 0x4) == 0x4) && ((ptmpdesc->des0 & 0x80000000) != 0x80000000))
		{
			mdata->bytes_xfered += ptmpdesc->des1;
		}
		if (mdata->bytes_xfered != mdata->blocks * mdata->blksz)
		{
			pdescriptor = host->pThis;
			printk("(%x, %x, %x, %x)\n", pdescriptor->des0, pdescriptor->des1, pdescriptor->des2, pdescriptor->des3);
		}


		if (host->data->flags & MMC_DATA_READ)
		{
			sg_idx = 0;

			while(sg_idx < host->sg_len)
			{
				sg = &mdata->sg[sg_idx];

				sg_idx++;
			}

		}
		buf_reg = vpl_read_register(host, IDSTS);
		if (buf_reg & 0x103)
		{
			vpl_set_register(host, IDSTS, 0x103);
		}
#else

	sg_idx = 0;
	while(sg_idx < host->sg_len)
	{
		sg = &mdata->sg[sg_idx];
		mdata->bytes_xfered += sg_dma_len(sg);
		sg_idx++;
	}
	vpl_set_register(host, IDSTS, ch_status);
#endif

	{
		vpl_sdhc_dma_done(host, host->data);
	}
#if SCHEDULE
#if TASKLET
#else
	mutex_unlock(&host->wq_mutex);
#endif
#endif
}

static void vpl_sdhc_setup_data(struct vpl_sdhc_host *host, struct mmc_request *req)
{
	struct mmc_data *data = req->data;
	unsigned int blockcount = data->blocks;
	unsigned int blksz = data->blksz;
	unsigned int bytecount = blockcount * blksz;
	int use_dma = host->use_dma;

	if (data->flags & MMC_DATA_STREAM)
	{
		blockcount = 0x0;
		POSDIODEBUG("MMC_DATA_STREAM, blockcount = 0x0\n");
	}
//	host->data = data;
	if(data == NULL)
	{
		POSDIODEBUG("[pop]NULL data pointer\n");
	}
	wmb();
	if (use_dma)
	{
		vpl_sdhc_prepare_dma(host, req->data);

	}
	else
	{
		struct scatterlist *sg;
		int sg_len;
		host->sg_idx = 0;
		sg_len = (data->blocks == 1) ? 1 : data->sg_len;
		sg = &data->sg[host->sg_idx];
		if (data->flags & MMC_DATA_WRITE)
			host->dma_data_dir = DMA_TO_DEVICE;
		else
			host->dma_data_dir = DMA_FROM_DEVICE;
		host->sg_len = dma_map_sg(mmc_dev(host->mmc), data->sg,
					sg_len, host->dma_data_dir);
		host->data = data;
		host->total_bytes_left = bytecount;
		sg = &host->data->sg[host->sg_idx];
		//sg = host->data->sg + host->sg_idx;
		host->buffer_bytes_left = sg->length;
		host->buffer = sg_virt(sg);
		if (host->buffer_bytes_left > host->total_bytes_left)
			host->buffer_bytes_left = host->total_bytes_left;
		host->buffer_bytes_xfered = 0;
		vpl_set_register(host, BLKSIZ, blksz);
		vpl_set_register(host, BYTCNT, bytecount);
	}
	wmb();
}

static void vpl_sdhc_start_cmd(struct vpl_sdhc_host *host, struct mmc_command *cmd)
{

	u32 cmdreg;
	u32 resptype;
	u32 cmdtype;
//	WARN_ON(host->cmd != NULL);
	resptype = 0;
	cmdtype = 0;
	host->cmd = cmd;
	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_NONE:
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1B:
		/* resp 1, 1b, 6, 7 */
		resptype = 5;
		break;
	case MMC_RSP_R2:
		resptype = 7;
		break;
	case MMC_RSP_R3:
		resptype = 1;
		break;
	default:
		dev_err(mmc_dev(host->mmc), "Invalid response type: %04x\n", mmc_resp_type(cmd));
		break;
	}

	if (mmc_cmd_type(cmd) == MMC_CMD_ADTC) {
		cmdtype = VPL_MMC_CMDTYPE_ADTC;
	} else if (mmc_cmd_type(cmd) == MMC_CMD_BC) {
		cmdtype = VPL_MMC_CMDTYPE_BC;
	} else if (mmc_cmd_type(cmd) == MMC_CMD_BCR) {
		cmdtype = VPL_MMC_CMDTYPE_BCR;
	} else {
		cmdtype = VPL_MMC_CMDTYPE_AC;
	}

	POSDIODEBUG("[pop]CMD %d Arguments = 0x%08lx\n", cmd->opcode, cmd->arg);

	cmdreg = cmd->opcode | (resptype << 6) | cmdtype;
	if(cmd->opcode == 9)
		host->card_address = cmd->arg;
	if (cmd->opcode == 12)
	{
		cmdreg |= 1 << 14;
//		if (host->dma_data_dir == DMA_FROM_DEVICE)
//			vpl_sdhc_dma_reset(host);
	}else if (cmd->flags & MMC_RSP_BUSY)
	{
		cmdreg |= 1 << 13;
	}
	if (host->stop_data == NULL)
	{
		if (host->data && (host->data->flags & MMC_DATA_WRITE))
			cmdreg |= 1 << 10;
		if (host->data && (host->data->flags & MMC_DATA_STREAM))
			cmdreg |= 1 << 11;
//		if ((cmd->opcode == 18) || (cmd->opcode == 25))
//			cmdreg |= 1 << 12;

	}
	SET_BITS(cmdreg, CMD_DONE_BIT);
	wmb();
//	if (cmd->opcode == 25)
//		mdelay(10000);
	vpl_set_register(host, CMDARG, cmd->arg);
	mb();
	vpl_set_register(host, CMD, cmdreg);
	wmb();
}

static void vpl_sdhc_finish_request_data(struct vpl_sdhc_host *host, struct mmc_request *req)
{
	volatile unsigned long timeout;

	timeout = 0;
	while(vpl_read_register(host, STATUS) & STATUS_DAT_BUSY_BIT)
	{

		udelay(100);
		timeout++;
		if(timeout > 1800000){ //3 mins: 3 * 60 (sec) * 1000 (ms)* 10 (100 us)
			printk("[vpl_mmc]vpl_sdhc_finish_request_data: SD card busy polling timeout!\n");
			break;
		}
	}
	host->req = NULL;
	host->cmd = NULL;
#if DMUTEX
	if (host->use_dma )
	vpl_sdhc_dma_reset(host);
	up(&(host->mutex));
#endif
	if(req != NULL)
		mmc_request_done(host->mmc, req);
}
static void vpl_sdhc_finish_request(struct vpl_sdhc_host *host, struct mmc_request *req)
{
	host->req = NULL;
	host->cmd = NULL;
//	up(&(host->lock_sem));
#if DMUTEX
//	up(&(host->mutex));
#endif
	if(req != NULL)
		mmc_request_done(host->mmc, req);
}

static int vpl_sdhc_cmd_done(struct vpl_sdhc_host *host, struct mmc_command *cmd)
{
	POSDIODEBUG("CMD done\n");
	if (host->data == NULL)
	{
		host->cmd = NULL;
		if(cmd == NULL)
		{
			struct mmc_request *req;
			req = host->req;
			vpl_sdhc_finish_request_data(host, req);
			return 1;
		}
	}

	if (cmd->flags & MMC_RSP_PRESENT) {
		if (cmd->flags & MMC_RSP_136) {
			/* response type 2 */
			cmd->resp[0] = vpl_read_register(host, RESP3);
			cmd->resp[1] = vpl_read_register(host, RESP2);
			cmd->resp[2] = vpl_read_register(host, RESP1);
			cmd->resp[3] = vpl_read_register(host, RESP0);
		} else {
			/* response types 1, 1b, 3, 4, 5, 6 */
			cmd->resp[0] = vpl_read_register(host, RESP0);
		}
	}
	wmb();
	if (host->data == NULL || cmd->error) {
		struct mmc_request *req;
		req = host->req;
		if (host->data != NULL)
		{
//			POSDIODEBUG("data != NULL cmd-> error = %d\n", cmd->error);
//		printk("host->data->error = %x, cmd->error= %x\n", host->data->error, cmd->error);
//			if (host->data->error)
				vpl_sdhc_xfer_done(host, host->data);
			return 1;
		}
		if (host->stop_data != NULL)
		{
			host->stop_data = NULL;
			vpl_sdhc_finish_request_data(host, req);
		}
		else
		{
			vpl_sdhc_finish_request(host, req);
		}
	}
	else
	{
//		printk("host->data->error = %x\n", host->data->error);
		if(host->data->error)
		{
			POSDIODEBUG("data != NULL\n");
			vpl_sdhc_xfer_done(host, host->data);
		}
	}
	return 1;
}

#if SCHEDULE
#if TASKLET
static void vpl_sdhc_done_command(unsigned long param)
{
	struct vpl_sdhc_host *host = (struct vpl_sdhc_host *) param;
#else
static void vpl_sdhc_done_command(struct work_struct *work)
{
	struct vpl_sdhc_host *host = container_of(work, struct vpl_sdhc_host,
						  cmd_done_work);
#endif
#else
static void vpl_sdhc_done_command(struct vpl_sdhc_host *host)
{
#endif
#if SCHEDULE
#if TASKLET
#else
	mutex_lock(&host->wq_mutex);
#endif
#endif
	if (host->stop_data == NULL)
	{
		vpl_sdhc_cmd_done(host, host->cmd);
	}
	else
	{
		vpl_sdhc_cmd_done(host, host->stop_data->stop);
	}
	wmb();
#if SCHEDULE
#if TASKLET
#else
	mutex_unlock(&host->wq_mutex);
#endif
#endif
}
static void vpl_sdhc_send_abort(struct vpl_sdhc_host *host)
{
	u32 buffer_reg;
	int count=0;
	volatile unsigned long timeout = 0;

	vpl_set_register(host, RINTSTS, vpl_read_register(host, MINTSTS));
	vpl_set_register(host, CMD, 0x8000414c);
	do
	{
		buffer_reg = vpl_read_register(host, MINTSTS);
//		if (count > 100)
//			break;
		count++;
		udelay(100);
		timeout++;
		if(timeout > 1800000){ //3 mins: 3 * 60 (sec) * 1000 (ms)* 10 (100 us)
			printk("[vpl_mmc]vpl_sdhc_send_abort: SD card command done polling timeout!\n");
			break;
		}
	}while((buffer_reg & INTMSK_CMD_DONE)!= INTMSK_CMD_DONE);
	vpl_set_register(host, RINTSTS, vpl_read_register(host, MINTSTS));

}
#if SCHEDULE_ABORT
#if TASKLET
static void vpl_sdhc_abort_command(unsigned long param)
{
	struct vpl_sdhc_host *host = (struct vpl_sdhc_host *) param;
#else
static void vpl_sdhc_abort_command(struct work_struct *work)
{
	struct vpl_sdhc_host *host = container_of(work, struct vpl_sdhc_host,
						  cmd_abort_work);
#endif
#else
static void vpl_sdhc_abort_command(struct vpl_sdhc_host *host)
{
#endif
//	printk("vpl_sdhc_abort_command host->cmd=%x\n", host->cmd);
#if SCHEDULE_ABORT
#if TASKLET
#else
	mutex_lock(&host->wq_mutex);
#endif
#endif
	if (host->cmd == NULL)
	{
		POSDIODEBUG("%s host->cmd = NULL\n", __func__);
		goto abort_done;
	}
	BUG_ON(!host->cmd);
	if (host->cmd->error == 0)
	{
		if (host->req->data!= 0)
		{
			if (host->req->data->error == 0)
				host->cmd->error = -ETIMEDOUT;
		}
		else
		host->cmd->error = -ETIMEDOUT;

	}

	if (host->data == NULL) {
		struct mmc_command *cmd;
		struct mmc_host    *mmc;

		cmd = host->cmd;
		host->cmd = NULL;
//		vpl_sdhc_send_abort(host);

		host->req = NULL;
		mmc = host->mmc;
		vpl_sdhc_finish_request(host, cmd->mrq);
//		mmc_request_done(mmc, cmd->mrq);
	} else {
		vpl_sdhc_cmd_done(host, host->cmd);
	}
abort_done:
	host->abort = 0;
#if SCHEDULE_ABORT
#if TASKLET
#else
	mutex_unlock(&host->wq_mutex);
#endif
#endif
}

static void vpl_sdhc_send_stop_work(struct vpl_sdhc_host *host, struct mmc_data *data)
{
	struct mmc_data *data1 = host->stop_data;
//	printk("%s %d\n", __func__, __LINE__);
	vpl_sdhc_start_cmd(host, data1->stop);
}

static void
vpl_release_dma(struct vpl_sdhc_host *host, struct mmc_data *data,
		     int abort)
{
	enum dma_data_direction dma_data_dir;
	volatile mshc_idmac_info * pdescriptor;
	pdescriptor = host->pThis;

	if (data->flags & MMC_DATA_WRITE)
		dma_data_dir = DMA_TO_DEVICE;
	else
		dma_data_dir = DMA_FROM_DEVICE;
	dma_unmap_sg(mmc_dev(host->mmc), data->sg, host->sg_len,
		     dma_data_dir);
}
static void
vpl_sdhc_xfer_done(struct vpl_sdhc_host *host, struct mmc_data *data)
{
	int use_dma = host->use_dma;
	struct mmc_data *mdata;
	if(host->data == NULL)
	{
		POSDIODEBUG("host->data = NULL\n");
		return;
	}
	mdata = host->data;
	if(use_dma)
	{

		if (host->req->data->error)
		{
//			POSDIODEBUG("host->req->data->error\n");
			mdata->bytes_xfered = 0;
//			vpl_init_controller(host);
		}
	}
	vpl_release_dma(host, data, data->error);
	host->data = NULL;
	host->sg_len = 0;

	/* NOTE:  MMC layer will sometimes poll-wait CMD13 next, issuing
	 * dozens of requests until the card finishes writing data.
	 * It'd be cheaper to just wait till an EOFB interrupt arrives...
	 */
	if (!data->stop) {
		struct mmc_host *mmc;
		struct mmc_request *req;
		req = host->req;
		host->req = NULL;
		mmc = host->mmc;
		vpl_sdhc_finish_request_data(host, req);
		return;
	}
	host->stop_data = data;
	vpl_sdhc_send_stop_work(host, host->stop_data);

}
static void
vpl_sdhc_dma_done(struct vpl_sdhc_host * host,struct mmc_data * data)
{
	int done;
	done = 1;
	if (done)
		vpl_sdhc_xfer_done(host, data);
}
/* PIO only */
static void
vpl_sdhc_data_done(struct vpl_sdhc_host *host, struct mmc_data *data)
{
	int done;

	done = 1;
	if (done)
		vpl_sdhc_xfer_done(host, data);
}
static void
vpl_sdhc_sg_to_buf(struct vpl_sdhc_host *host)
{
	struct scatterlist *sg;

	sg = &host->data->sg[host->sg_idx];
//	sg = host->data->sg + host->sg_idx;
	host->buffer_bytes_left = sg->length;
	host->buffer = sg_virt(sg);
	if (host->buffer_bytes_left > host->total_bytes_left)
		host->buffer_bytes_left = host->total_bytes_left;
}

static int vpl_sdhc_cpu_driven_data(struct vpl_sdhc_host *host, int write)
{
	int n, fifo_thresh, count, pend_data_number, dangling_dwords;
	u8 *u8_buffer;
	u32 buffer = 0;
	fifo_thresh = vpl_read_register(host, FIFOTH);
	fifo_thresh = GET_FIFO_DEPTH(fifo_thresh);
	if (host->buffer_bytes_left == 0) {
		host->sg_idx++;
		BUG_ON(host->sg_idx == host->sg_len);
		vpl_sdhc_sg_to_buf(host);
		host->buffer_bytes_xfered = 0;
	}
	wmb();
	u8_buffer = (host->buffer + host->buffer_bytes_xfered);

	n = fifo_thresh * FIFO_WIDTH;
	if (n > host->buffer_bytes_left)
	{
		pend_data_number = host->buffer_bytes_left / FIFO_WIDTH;
		n = pend_data_number * FIFO_WIDTH;
		dangling_dwords = 0;
	}
	else
	{
		pend_data_number = fifo_thresh;
		dangling_dwords = 0;
	}
	host->buffer_bytes_left -= (n+dangling_dwords);
	host->total_bytes_left -= (n+dangling_dwords);
	host->data->bytes_xfered += (n+dangling_dwords);

	if (write) {// pending data may have BUG ==>the_ip_status.fifo_depth - the_ip_status.fifo_threshold);
		for (count = 0; count < pend_data_number; count++)
		{
			vpl_set_register(host, FIFODAT, *((u32 *) (u8_buffer + count * FIFO_WIDTH)));
			POSDIODEBUG("0x%08lx ",*((u32 *) (u8_buffer + count * FIFO_WIDTH)));
			if((count%4)==3)
				POSDIODEBUG("\n");
		}
		if (dangling_dwords)
		{
				u8_buffer = (host->buffer + host->buffer_bytes_xfered);
				buffer = 0;
				memcpy((void *) &buffer, (void *) u8_buffer, dangling_dwords);
				vpl_set_register(host, FIFODAT, *((u32 *)buffer));
				POSDIODEBUG("0x%08lx\n", *((u32 *)buffer));
		}
	} else {
		for (count = 0; count < pend_data_number; count++)
		{
			*((u32 *) (u8_buffer + count * FIFO_WIDTH)) = vpl_read_register(host, FIFODAT);
			POSDIODEBUG("0x%08lx ",*((u32 *) (u8_buffer + count * FIFO_WIDTH)));
			if((count%4)==3)
				POSDIODEBUG("\n");
		}
		if (dangling_dwords)
		{
				u8_buffer = (host->buffer + host->buffer_bytes_xfered);
				buffer = vpl_read_register(host, FIFODAT);
				memcpy((void *) u8_buffer, (void *) &buffer, dangling_dwords);
				POSDIODEBUG("0x%08lx\n", *((u32 *)buffer));
		}
	}
	host->buffer_bytes_xfered += (n+dangling_dwords);
	return 1;
}

static irqreturn_t vpl_sdhc_irq(int irq, void *dev_id)
{
	struct vpl_sdhc_host * host = (struct vpl_sdhc_host *)dev_id;
	u32 cmd_status, rinsts_reg, error_status, transfer_error, end_transfer, cmd_error, end_command, dma_status, data_error, buf_reg;
	struct mmc_host *mmc;
	int delay_count;
	volatile mshc_idmac_info * pdescriptor;
	volatile mshc_idmac_info * ptmpdesc;
	int use_dma = host->use_dma;
	rinsts_reg = vpl_read_register(host, MINTSTS);
	mmc = host->mmc;
	cmd_status = 0;
	error_status = 0;
	transfer_error = 0;
	end_transfer = 0;
	cmd_error = 0;
	data_error = 0;
	end_command = 0;
#if 1
	dma_status = vpl_read_register(host, IDSTS);
	if (dma_status & 0x103)
	{
		vpl_set_register(host, IDSTS, 0x103);
	}
#if 0
	if (dma_status & 0x1efc)
	{
		POSDIODEBUG("dma status = %x\n", dma_status);
		POSDIODEBUG("rinsts_reg = %x\n", rinsts_reg);
		host->present = 0;
		pdescriptor = host->pThis;
		ptmpdesc = pdescriptor;
		POSDIODEBUG("DBADDR = %x\n", vpl_read_register(host,DBADDR));
		do
		{
			ptmpdesc->des0 = 0;
			ptmpdesc++;
//			ptmpdesc = bus_to_virt(ptmpdesc->des3);
#if NON_CACHE
		}while (((ptmpdesc->des0 & 0x4) != 0x4)&(ptmpdesc->des3 != host->kmalloced_dma));
#else
		}while (((ptmpdesc->des0 & 0x4) != 0x4)&(ptmpdesc->des3 != virt_to_bus(host->pThis)));
#endif
		vpl_set_register(host, PLDMND, 0x1);
		udelay(100);

		buf_reg = vpl_read_register(host, CTRL);
		vpl_set_register(host, CTRL, (buf_reg|0x4));// DMA reset
		udelay(100);
		vpl_set_register(host, CTRL, (buf_reg|0x2));// FIFO reset
		udelay(100);

		do{
			buf_reg = vpl_read_register(host, CTRL);
			udelay(100);
		}while((buf_reg & 0x2) ==0x2);
		vpl_set_register(host, BMOD, 0x00000001);
		udelay(100);
		vpl_set_register(host, IDSTS, 0x1FFF);
		udelay(100);
		vpl_set_register(host, BMOD, 0x00000082|(BURST_SIZE<<8)|(DSL<<2));
		host->present = 1;
		host->dma_sts = 0;
		vpl_sdhc_finish_request_data(host, host->req);
		return IRQ_HANDLED;

	}
#endif
#endif
	if (rinsts_reg & INTMSK_CDETECT)
	{
		vpl_set_register(host, RINTSTS, rinsts_reg);
//		vpl_set_register(host, RINTSTS, INTMSK_CDETECT);
		rinsts_reg &= ~INTMSK_CDETECT;
#if 1
		if (cd_share_pin == 1)
		{

			if (vpl_read_register(host, CDETECT) == 0x1)// card insert
			{
				if (host->host_sts == 0)
				{
					host->present = 0;
					host->host_sts = 1;
					mod_timer(&host->cmd_abort_timer, jiffies + HZ*3/2);
				}
			}
			else
			{
				host->present = 0;
				if (host->req) {
				host->req->cmd->error = -ENOMEDIUM;
				if (host->req->data != NULL)
					host->req->data->error = -ENOMEDIUM;
				}
//data
				if (host->cmd)
				{
					host->cmd->error = -ENOMEDIUM;
					host->abort = 1;
#if SCHEDULE_ABORT
#if TASKLET
					tasklet_schedule(&host->cmd_abort_work);
#else
					schedule_work(&host->cmd_abort_work);
#endif
#else
					vpl_sdhc_abort_command(host);
#endif
				}
				host->host_sts = 0;
				delay_count = 0;
				buf_reg = vpl_read_register(host, INTMSK);
				buf_reg &= ~INTMSK_CDETECT;
				vpl_set_register(host, INTMSK, buf_reg);
				mod_timer(&host->cmd_abort_timer, jiffies + HZ*3/2);
				mmc_detect_change(host->mmc, msecs_to_jiffies(delay_count));//msecs_to_jiffies(500) is delay to detect card
			}

		}
		else
		{
			host->present = 0;
			if (host->req) {
				host->req->cmd->error = -ENOMEDIUM;
				if (host->req->data != NULL)
					host->req->data->error = -ENOMEDIUM;
			}
//data
			if (host->cmd)
			{
				host->cmd->error = -ENOMEDIUM;
				host->abort = 1;
#if SCHEDULE_ABORT
#if TASKLET
				tasklet_schedule(&host->cmd_abort_work);
#else
				schedule_work(&host->cmd_abort_work);
#endif
#else
				vpl_sdhc_abort_command(host);
#endif
			}
			if (vpl_read_register(host, CDETECT) == 0x0)// card insert
			{
				host->host_sts = 1;
				mod_timer(&host->cmd_abort_timer, jiffies + HZ*3/2);
			}
			else
			{
				host->host_sts = 0;
				delay_count = 0;
				buf_reg = vpl_read_register(host, INTMSK);
				buf_reg &= ~INTMSK_CDETECT;
				vpl_set_register(host, INTMSK, buf_reg);
				mod_timer(&host->cmd_abort_timer, jiffies + HZ*3/2);
				mmc_detect_change(host->mmc, msecs_to_jiffies(delay_count));//msecs_to_jiffies(500) is delay to detect card
			}
		}
		return IRQ_HANDLED;
#endif
	}
	else
	{
		if (host->req == NULL)
		{
			vpl_set_register(host, RINTSTS, rinsts_reg);
			return IRQ_HANDLED;
		}
		if (rinsts_reg & INTMSK_FRUN)//data
		{
			error_status = ERROVERREAD;
			data_error = 1;
		}
		if (rinsts_reg & INTMSK_RESP_ERR)//cmd
		{
			error_status = ERRRESPRECEP;
			host->req->cmd->error = -EILSEQ;
			end_command = 1;
		}
		if (rinsts_reg & INTMSK_RCRC)//cmd
		{
			error_status = ERRRESPRECEP;
			cmd_error = 1;
			host->req->cmd->error = -EILSEQ;
		}
		if (rinsts_reg & INTMSK_DCRC)//data
		{
			error_status = ERRDCRC;
			transfer_error = 1;
			data_error = 1;
			host->req->data->error = -EILSEQ;
		}
		if (rinsts_reg & INTMSK_RTO)//cmd
		{
			error_status = ERRRESPTIMEOUT;
			end_command = 1;
		}
		if (rinsts_reg & INTMSK_DRTO)//data read timeout // data
		{
			host->req->data->error = -ETIMEDOUT;
			error_status = ERRDATATIMEOUT;
			transfer_error = 1;
			data_error = 1;
		}
		if (rinsts_reg & INTMSK_HTO)//data
		{
			error_status = ERRUNDERWRITE;
			data_error = 1;
		}
		if (rinsts_reg & INTMSK_HLE)
		{
			error_status = ERRRESPTIMEOUT;
		}
		if (rinsts_reg & INTMSK_SBE)
		{
			error_status = ERRSTARTBIT;
			transfer_error = 1;
			data_error = 1;
		}
		if (rinsts_reg & INTMSK_ACD)
		{
			error_status = ERRRESPTIMEOUT;
		}
		if (rinsts_reg & INTMSK_EBE)//data
		{
			error_status = ERRENDBITERR;
			transfer_error = 1;
			data_error = 1;
			host->req->data->error = -EILSEQ;
		}
		if (error_status != 0)
		{
			goto HOUSEKEEP;
		}

		vpl_set_register(host, RINTSTS, rinsts_reg);
		if (rinsts_reg & INTMSK_CMD_DONE)
		{
#if SCHEDULE
#if TASKLET
			tasklet_schedule(&host->cmd_done_work);
#else
			schedule_work(&host->cmd_done_work);
#endif
#else
			vpl_sdhc_done_command(host);
#endif
		}
		if (rinsts_reg & INTMSK_TXDR)
		{
			if (host->total_bytes_left)
				vpl_sdhc_cpu_driven_data(host, 1);

		}
		if (rinsts_reg & INTMSK_RXDR)
		{
			if (host->total_bytes_left)
				vpl_sdhc_cpu_driven_data(host, 0);
		}
		if (rinsts_reg & INTMSK_DAT_OVER)
		{
			if (use_dma)
			{

#if SCHEDULE
#if TASKLET
			tasklet_schedule(&host->dma_done_work);
#else
			schedule_work(&host->dma_done_work);
#endif
#else


			if (host->data == NULL)
			{
				POSDIODEBUG("%s host->data == NULL\n", __func__);
//pop				del_timer_sync(&host->dma_abort_timer);
			}else{
				vpl_sdhc_dma_cb(host);
			}
#endif
			}
			else
			{
				if(host->total_bytes_left)
				{
					if (host->data && (host->data->flags & MMC_DATA_WRITE))
					{
							vpl_sdhc_cpu_driven_data(host, 1);
					}
					else
					{
	//					POSDIODEBUG("Read data\n");
						vpl_sdhc_cpu_driven_data(host, 0);
						if (!(vpl_read_register(host, STATUS) & 0x00000004))
							vpl_sdhc_cpu_driven_data(host, 0);
					}
				}
			end_transfer = 1;

			}
		}

	}
HOUSEKEEP:
	if (transfer_error)
	{
			POSDIODEBUG("Transfer error\n");
			vpl_set_register(host, RINTSTS, rinsts_reg);
			if (host->data != NULL)
			{
				host->abort = 1;
#if SCHEDULE_ABORT
#if TASKLET
			tasklet_schedule(&host->cmd_abort_work);
#else
			schedule_work(&host->cmd_abort_work);
#endif
#else
			vpl_sdhc_abort_command(host);
#endif
			}
			else
			{
				POSDIODEBUG("data NULL!!\n");
			}
			return IRQ_HANDLED;
	}
	if ((cmd_error && host->data) || (end_command)) {
		vpl_set_register(host, RINTSTS, rinsts_reg);
		host->abort = 1;
#if SCHEDULE_ABORT
#if TASKLET
			tasklet_schedule(&host->cmd_abort_work);
#else
			schedule_work(&host->cmd_abort_work);
#endif
#else
			vpl_sdhc_abort_command(host);
#endif

	}
	if(error_status)
	{
		POSDIODEBUG("error = 0x%08lx\n", error_status);
	}
	if (rinsts_reg & INTMSK_SDIO_INTR)
	{
		vpl_set_register(host, RINTSTS, rinsts_reg);
		mmc_signal_sdio_irq(host->mmc);
	}
	if (use_dma)
	{
	}
	else
	{
		if (host->data != NULL) {
			if (transfer_error)
				vpl_sdhc_xfer_done(host, host->data);
			else if (end_transfer)
			{
				vpl_sdhc_data_done(host, host->data);
			}
		}
	}

	return IRQ_HANDLED;
}
struct mmc_command stop;

static void
vpl_sdhc_dma_timer(unsigned long data)
{
	struct vpl_sdhc_host *host = (struct vpl_sdhc_host *) data;
	POSDIODEBUG("DMA abort timer\n");
		host->host_sts = 0;
		host->present = 0;
	if (host->data)
	{
		disable_irq(host->irq);
		host->host_sts = 0;
		host->present = 0;
		vpl_set_register(host, RINTSTS, vpl_read_register(host, RINTSTS));
		vpl_set_register(host, INTMSK, 0xbfcf);
		enable_irq(host->irq);
		host->req->data->error = -ETIMEDOUT;
		vpl_sdhc_xfer_done(host, host->data);
	}
}
static void
vpl_sdhc_cmd_timer(unsigned long data)
{
	struct vpl_sdhc_host *host = (struct vpl_sdhc_host *) data;
	u32 buf_reg;
	int delay_count;
	if (host->present == 0)
	{
		if (cd_share_pin == 1)
		{
	 		if (vpl_read_register(host, CDETECT) == 0x1)// card insert
			{
				delay_count = 500;
				host->present = 1;
			}
			else
			{
				delay_count = 10;
			}
		}
		else
		{
	 		if (vpl_read_register(host, CDETECT) == 0x0)// card insert
			{
				delay_count = 500;
				host->present = 1;
			}
			else
			{
				delay_count = 10;
			}
		}
		buf_reg = vpl_read_register(host, INTMSK);
		buf_reg |= INTMSK_CDETECT;
		vpl_set_register(host, INTMSK, buf_reg);
	}
	if (cd_share_pin == 1)
	{
		if (vpl_read_register(host, CDETECT) == 0x1)// card insert
		{
			mmc_detect_change(host->mmc, msecs_to_jiffies(500));//msecs_to_jiffies(500) is delay to detect card
		}
	}
	else
	{
		if (vpl_read_register(host, CDETECT) == 0x0)// card insert
		{
			mmc_detect_change(host->mmc, msecs_to_jiffies(500));//msecs_to_jiffies(500) is delay to detect card
		}
	}
}
int enter_count=0;
static void vpl_sdhc_request(struct mmc_host *mmc, struct mmc_request *req)
{
	struct vpl_sdhc_host *host = mmc_priv(mmc);

	unsigned long flags;
#if USE_SPIN
	spin_lock_irqsave(&host->lock, flags);
#endif
	enter_count++;
/*
	if (down_interruptible(&(host->lock_sem)))
	{
		POSDIODEBUG("Err host->lock %d\n", __LINE__);
		req->cmd->error = -ETIMEDOUT;
		host->data = NULL;
		vpl_sdhc_cmd_done(host, req->cmd);
		return;
	}
*/
	WARN_ON(host->req != NULL);
	if (host->present == 0)
	{
		host->req = req;
		req->cmd->error = -ETIMEDOUT;
//		host->data = NULL;
		vpl_sdhc_finish_request(host, req);
//		vpl_sdhc_cmd_done(host, req->cmd);
		if(host->host_sts == 1)
		{
			mod_timer(&host->cmd_abort_timer, jiffies + HZ*3/2);
		}
//		up(&(host->lock_sem));
		enter_count--;
#if USE_SPIN
		spin_unlock_irqrestore(&host->lock, flags);
#endif
		return;
	}
	if (req->data) {
#if DMUTEX
		if (down_trylock(&(host->mutex)))
//		while (down_interruptible(&(host->mutex)))
		{
//			POSDIODEBUG("Err mutex\n");
			req->cmd->error = -ETIMEDOUT;
			vpl_sdhc_finish_request(host, req);
			enter_count--;
#if USE_SPIN
			spin_unlock_irqrestore(&host->lock, flags);
#endif
			return;
		}
#endif
	wmb();
		vpl_sdhc_setup_data(host, req);
	}
	else
	{
		if(host->data)
//			POSDIODEBUG("host->data = %x\n", host->data);
		host->data = NULL;
	}
	host->req = req;

	wmb();
	vpl_sdhc_start_cmd(host, req->cmd);
	mmiowb();
//	up(&(host->lock_sem));
	enter_count--;
#if USE_SPIN
	spin_unlock_irqrestore(&host->lock, flags);
#endif
}

static void vpl_sdhc_enable_cd_irq(struct mmc_host *mmc, int enable)
{
	struct vpl_sdhc_host *host = mmc_priv(mmc);
	unsigned long flags;
	u32 ier;
	host = mmc_priv(mmc);
	if (cd_share_pin == 1)
	{
#if USE_SPIN
	spin_lock_irqsave(&host->lock, flags);
#endif
	ier = vpl_read_register(host, INTMSK);
	if (host->host_sts == 1)
	ier &= ~INTMSK_CDETECT;
	if (enable)
		ier |= INTMSK_CDETECT;

	vpl_set_register(host, INTMSK, ier);
	mmiowb();
#if USE_SPIN
	spin_unlock_irqrestore(&host->lock, flags);
#endif
	}
}

static void vpl_sdhc_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct vpl_sdhc_host *host = mmc_priv(mmc);
	unsigned long flags;
	u32 ier;

	host = mmc_priv(mmc);

#if USE_SPIN
	spin_lock_irqsave(&host->lock, flags);
#endif
	ier = vpl_read_register(host, INTMSK);
	ier &= ~INTMSK_SDIO_INTR;
	if (enable)
		ier |= INTMSK_SDIO_INTR;

	vpl_set_register(host, INTMSK, ier);
	vpl_set_bits(host, CTRL, INT_ENABLE);

	mmiowb();

#if USE_SPIN
	spin_unlock_irqrestore(&host->lock, flags);
#endif
}

#define CLK_RATE 19200000
static int vpl_sdhc_get_ro(struct mmc_host *mmc)
{
	struct vpl_sdhc_host *host = mmc_priv(mmc);
	unsigned long flags;
	int present;

	host = mmc_priv(mmc);

#if USE_SPIN
	spin_lock_irqsave(&host->lock, flags);
#endif
/*
	while (down_trylock(&(host->lock)))
	{
		POSDIODEBUG("Err host->lock %d\n", __LINE__);
	}
*/
	if(host->write_protect_mode)
	{
		present = vpl_read_register(host, WRTPRT);
	}
	else
	{
		present = 0;
	}
//	up(&(host->lock));
#if USE_SPIN
	spin_unlock_irqrestore(&host->lock, flags);
#endif

	return present;
}

static void vpl_sdhc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct vpl_sdhc_host *host = mmc_priv(mmc);

	unsigned long flags;
#if USE_SPIN
	spin_lock_irqsave(&host->lock, flags);
#endif
/*
	while (down_trylock(&(host->lock)))
	{
		POSDIODEBUG("Err host->lock %d\n", __LINE__);
	}
*/
	if( ios->bus_width==MMC_BUS_WIDTH_4 ) {
		host->actual_bus_width = MMC_BUS_WIDTH_4;
		vpl_clear_bits(host, CTYPE, (1 << 0) || (1 << (0 + 16)));
		vpl_set_bits(host, CTYPE, (1 << 0));
	}else{
		host->actual_bus_width = MMC_BUS_WIDTH_1;
		vpl_clear_bits(host, CTYPE, (1 << 0) || (1 << (0 + 16)));
	}

	if ( host->power_mode != ios->power_mode ) {
		host->power_mode = ios->power_mode;
	}
	if ( ios->clock ) {
		vpl_sdhc_start_clock(host);
	} else {
		vpl_sdhc_stop_clock(host);
	}
//	up(&(host->lock));
#if USE_SPIN
	spin_unlock_irqrestore(&host->lock, flags);
#endif
}

static const struct mmc_host_ops vpl_sdhc_ops = {
	.request	= vpl_sdhc_request,
	.set_ios	= vpl_sdhc_set_ios,
	.get_ro		= vpl_sdhc_get_ro,
	.enable_sdio_irq = vpl_sdhc_enable_sdio_irq,
	.enable_cd_irq = vpl_sdhc_enable_cd_irq,
};

static int vpl_sdhc_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct vpl_sdhc_host *host = NULL;
	struct resource *r;
	int ret = 0, irq, retval, i;
	int use_dma;
	unsigned int sysc_base, reg_val, ddr_base, ddr_ctrl, sys_cfg_info;
	volatile mshc_idmac_info * pdescriptor;

	printk(KERN_INFO "VPL sdhc driver\n");
	sysc_base = (unsigned int)ioremap(VPL_SYSC_MMR_BASE, SZ_4K);

#if 1
	if (cd_share_pin == 1)
	{
		pull_high_wpcd = 0;
		pull_high_data = 0;
	}
	switch (pdev->id)
	{
		case 0:
				reg_val = readl((unsigned int)(sysc_base+0x50));
				reg_val &= ~0xe;
				writel(reg_val|((pull_high_data<<3)|(pull_high_cmd << 2)|(pull_high_wpcd<<1)), (unsigned int)(sysc_base+0x50));
				break;
		case 1:
				reg_val = readl((unsigned int)(sysc_base+0x50));
				reg_val &= ~0x70;
				writel(reg_val|((pull_high_data<<6)|(pull_high_cmd << 5)|(pull_high_wpcd<<4)), (unsigned int)(sysc_base+0x50));
				reg_val = readl((unsigned int)(sysc_base+0x24));
				writel(reg_val|(0x1<<27), (unsigned int)(sysc_base+0x24));
				reg_val = readl((unsigned int)(sysc_base+0x44));
				writel(reg_val|(0x1<<11), (unsigned int)(sysc_base+0x44));
				reg_val = readl((unsigned int)(sysc_base+0x30));
				reg_val &= 0xff;
				writel(reg_val|0x1500, (unsigned int)(sysc_base+0x30));
				break;
		default:
				break;
	}
#endif
//evelyn modified: For 30MHz XTAL (M330 with DDR3), MSHC_0_CLK_CFG/MSHC_1_CLK_CFG(SYSC_CLK_GEN_CFG) should be set to 19.23MHz.
// Actually the mshc_clk will be 30*10/13 = 23.08MHz.
	// SYSC_PROT_INFO_0 [2:0]
	sys_cfg_info = readl((unsigned int)(sysc_base+0x3c));

	ddr_base = (unsigned int)ioremap(VPL_DDRSDMC_0_MMR_BASE, SZ_4K);
	ddr_ctrl = readl((unsigned int)(ddr_base+0x04));

	if (((sys_cfg_info & 0x7) == 0x5) && (ddr_ctrl & 0x400))  // M330 && DDR3
	{
		reg_val = readl((unsigned int)(sysc_base+0x2c));
		writel(reg_val | (0x3<<28), (unsigned int)(sysc_base+0x2c)); // MSHC_0/1_clock_speed is configured to 19.23MHz
	}
	iounmap((unsigned int*)ddr_base);
	iounmap((unsigned int*)sysc_base);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0); // not yet check 20090424
	if (!r || irq < 0)
		return -ENXIO;

	if (!request_mem_region(r->start, SZ_4K, pdev->name))
		return -EBUSY;

	mmc = mmc_alloc_host(sizeof(struct vpl_sdhc_host), &pdev->dev);
	if (!mmc) {
		ret = -ENOMEM;
		goto out1;
	}

	mmc->ops = &vpl_sdhc_ops;

	mmc->f_min = 150000;
	mmc->f_max = CLK_RATE/2;
	mmc->ocr_avail = MMC_VDD_32_33;
	mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SDIO_IRQ;

	/* MMC core transfer sizes tunable parameters. Our hardware not support */
	mmc->max_hw_segs = NR_HWSG;
	/*
	 * We can do SG-DMA, but we don't because we never know how much
	 * data we successfully wrote to the card.
	 */
	mmc->max_phys_segs = NR_SG;
	mmc->max_seg_size =  DMA_TRANSFER_SIZE;	/* default PAGE_CACHE_SIZE */
	/*
	 * Our hardware DMA can handle a maximum of one page per SG entry.
	 */
	mmc->max_req_size = NR_SG*DMA_TRANSFER_SIZE;	/* default PAGE_CACHE_SIZE */
	/*
	 * Block length register is 16 bits.
	 */
//	mmc->max_blk_size = 4096;
	mmc->max_blk_size = 512;
	/*
	 * Block count register is 32 bits.
	 */
	mmc->max_blk_count = 65536;
//	mmc->max_req_size       = mmc->max_blk_size * mmc->max_blk_count;

	host = mmc_priv(mmc);
	host->mmc = mmc;
	host->use_dma = MSHC_DMA;
	host->write_protect_mode = write_protect;
	mutex_init(&(host->wq_mutex));
	use_dma = host->use_dma;
	if (use_dma)
	{
		int descript_max_number = NR_BUF;
//		pdescriptor = dma_alloc_coherent(host->dev, (descript_max_number) * sizeof(mshc_idmac_info), &host->kmalloced_dma, GFP_KERNEL);
#if NON_CACHE
		pdescriptor = dma_alloc_consistent(NULL, (descript_max_number) * sizeof(mshc_idmac_info), &host->kmalloced_dma);
#else
		pdescriptor = (mshc_idmac_info *)kzalloc( (descript_max_number) * sizeof(mshc_idmac_info),GFP_KERNEL| GFP_DMA);
#endif
		if (!pdescriptor)
		{
			POSDIODEBUG("kzalloc fail\n");
			return -ENOMEM;
		}
		host->pThis = pdescriptor;
		for (i=0; i< descript_max_number; i++)
		{
#if NON_CACHE
			pdescriptor->des3 = host->kmalloced_dma + sizeof(mshc_idmac_info) *(i+1);
#else
			pdescriptor->des3 = virt_to_bus(pdescriptor+1);
#endif
			pdescriptor++;
		}
		pdescriptor--;
#if NON_CACHE
		pdescriptor->des3 = host->kmalloced_dma;
#else
		pdescriptor->des3 = virt_to_bus(host->pThis);
#endif

	}
	else
	{
		host->pThis = NULL;
	}
	host->pdata = pdev->dev.platform_data;
#if DMUTEX
	sema_init(&(host->mutex), 1);
#endif
	spin_lock_init(&host->lock);
	sema_init(&(host->lock_sem), 1);
	host->id = pdev->id;
	host->res = r;
	host->irq = irq;
	host->dev = pdev->dev;
	host->clk = 0;

	if (IS_ERR(host->clk)) {
		ret = PTR_ERR(host->clk);
		goto out_dma;
	}

	vpl_sdhc_softreset();

	host->virt_base = ioremap(r->start, SZ_4K);
//	POSDIODEBUG("probe host->virt_base = %x\n", host->virt_base);
	if (!host->virt_base) {
		ret = -ENOMEM;
		goto err_ioremap;
	}
#if 0
	if(vpl_read_register(host, VERID) != VENDORID)
	{
		POSDIODEBUG("[pop] No MSHC device\n");
		ret = -ENXIO;
		goto err_ioremap;
	}
#endif
#if SCHEDULE_ABORT
#if TASKLET
	tasklet_init(&host->cmd_abort_work, vpl_sdhc_abort_command, (unsigned long) host);
#else
	INIT_WORK(&host->cmd_abort_work, vpl_sdhc_abort_command);
#endif
#endif

#if SCHEDULE
#if TASKLET
	tasklet_init(&host->cmd_done_work, vpl_sdhc_done_command, (unsigned long) host);
	tasklet_init(&host->dma_done_work, vpl_sdhc_dma_cb, (unsigned long) host);
#else
	INIT_WORK(&host->cmd_done_work, vpl_sdhc_done_command);
	INIT_WORK(&host->dma_done_work, vpl_sdhc_dma_cb);
#endif
#endif
	setup_timer(&host->cmd_abort_timer, vpl_sdhc_cmd_timer, (unsigned long) host);
//	setup_timer(&host->dma_abort_timer, vpl_sdhc_dma_timer, (unsigned long) host);

	vpl_init_controller(host);
	host->present = 1;
	host->dma_sts = 1;
	host->host_sts = 0;

	platform_set_drvdata(pdev, mmc);
//	printk("before mmc_remove_host\n");
	retval = mmc_add_host(mmc);
//	printk("after mmc_remove_host result = %x\n", retval);
	if (retval < 0)
	{
		goto err_remove_host;
	}
	ret = request_irq(host->irq, vpl_sdhc_irq, 0, DRIVER_NAME, host);
	if (ret)
		goto out;


	return 0;
out:
	if (host) {
		if (host->clk) {
//			clk_disable(host->clk);
//			clk_put(host->clk);
		}
	}
err_remove_host:
	if (mmc)
	{
		mmc_remove_host(mmc);
	}
err_ioremap:
	iounmap(host->virt_base);
out_dma:
	if (use_dma)
	{
#if NON_CACHE
			dma_free_consistent(NULL,(NR_BUF) * sizeof(mshc_idmac_info), host->pThis, &host->kmalloced_dma);
#else
			kfree(host->pThis);
#endif
	}
out1:
	release_mem_region(r->start, SZ_4K);
	mmc_free_host(mmc);
	return ret;
}

static int vpl_sdhc_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	struct vpl_sdhc_host *host = NULL;

	if (mmc) {
		host = mmc_priv(mmc);
		mmc_remove_host(mmc);
		printk("remove mmc\n");
		free_irq(host->irq, host);
		if ( host->use_dma)
		{
#if NON_CACHE
			dma_free_consistent(NULL,(NR_BUF) * sizeof(mshc_idmac_info), host->pThis, &host->kmalloced_dma);
#else
			kfree(host->pThis);
#endif
		}
		iounmap(host->virt_base);

		release_mem_region(host->res->start, SZ_4K);
		mmc_free_host(mmc);
		platform_set_drvdata(pdev, NULL);

	}
	return 0;
}

#ifdef CONFIG_PM
static int vpl_sdhc_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	int ret = 0;
	POSDIODEBUG("vpl_sdhc_suspend\n");
	if (mmc)
		ret = mmc_suspend_host(mmc, state);//not yet implement 20090424 poplar

	return ret;
}

static int vpl_sdhc_resume(struct platform_device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct vpl_sdhc_host *host;
	int ret = 0;
	POSDIODEBUG("vpl_sdhc_resume\n");
	if (mmc)
		ret = mmc_resume_host(mmc);//not yet implement 20090424 poplar

	return ret;
}
#else
#define vpl_sdhc_suspend  NULL
#define vpl_sdhc_resume   NULL
#endif /* CONFIG_PM */

static struct platform_driver vpl_sdhc_driver = {
	.probe		= vpl_sdhc_probe,
	.remove		= vpl_sdhc_remove,
	.suspend	= vpl_sdhc_suspend,
	.resume		= vpl_sdhc_resume,
	.driver		= {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	}
};

static int __init vpl_sdhc_init(void)
{
	return platform_driver_register(&vpl_sdhc_driver);
}

static void __exit vpl_sdhc_exit(void)
{
	platform_driver_unregister(&vpl_sdhc_driver);
}

module_init(vpl_sdhc_init);
module_exit(vpl_sdhc_exit);

MODULE_DESCRIPTION("Multimedia Card Interface Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:vpl-sdhc");
module_param_named(write_protect, write_protect, int, 0444);
module_param_named(clk_div, clk_div, int, 0444);
module_param_named(pull_high_cmd, pull_high_cmd, int, 0444);
module_param_named(pull_high_data, pull_high_data, int, 0444);
module_param_named(pull_high_wpcd, pull_high_wpcd, int, 0444);
module_param_named(cd_share_pin, cd_share_pin, int, 0444);
