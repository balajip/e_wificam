/*
 * Mozart_Audio_SSM2603
 * Driver for Mozart Audio Codec SSM2603.
 *
 * Copyright (C) 2010  ______ Inc.
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

#include "vaudio_locals.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/i2c.h> // kernel i2c interface 2009.09.29

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/control.h>

#include <asm/cacheflush.h>

#include "SSM2603.h"

/* kernel i2c interface 2009.09.29 */
static struct i2c_client *ssm2603_i2c;
static struct i2c_client client_template;
static const unsigned short normal_i2c[] = { 0x1a, I2C_CLIENT_END };
/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/*==========================================================*/
MODULE_DESCRIPTION("SSM2603 Audio Driver");
MODULE_LICENSE("GPL");

/*==========================================================*/
/* IDG for this card */
static char *id = "Mozart_SSM2603";

/* device structure */
static struct platform_device *device;

static int i2sClkSrc = 0;
module_param(i2sClkSrc, int, 0644);
MODULE_PARM_DESC (i2sClkSrc, "I2S clock source 0: 18.432, 1: 24MHz");
//.........................................................................................................................
/* audio stream */
struct audio_stream {
	int stream_id;		/* numeric identification */
	unsigned int active:1;	/* we are using this stream for transfer now */
	int dma_pos;			/* dma processed position in frames */
	struct dma_regs_t * desc_addr;
	struct snd_pcm_substream *stream;
};

//.........................................................................................................................
/* chip */
struct snd_i2s {
	struct snd_card *card;
	struct snd_pcm *pcm[MOZART_I2S_NUM];
	long samplerate[MOZART_I2S_NUM];
	long vvdc_param[MOZART_I2S_NUM];
	int dma_irq;
	int dma_irq_acks;
	struct mutex dma_irq_mutex;
	unsigned int dma_requested;
	struct audio_stream s[MOZART_I2S_NUM][2];	/* I2S0 playback/capture */
};

//.........................................................................................................................
/* runtime private data for device info */
typedef struct {
	int dev_num;
	int dma_chn;
}snd_i2s_dev_private;

//.........................................................................................................................
/* The actual rates supported by the card */
static unsigned int samplerates[5] = {
	8000, 16000, 32000, 44100, 48000
};
static struct snd_pcm_hw_constraint_list constraints_rates = {
	.count = ARRAY_SIZE(samplerates),
	.list = samplerates,
	.mask = 0,
};

//.........................................................................................................................
/* MOZART VPL SSP virtual base address */
static unsigned int ssp_base[MOZART_I2S_NUM] = {
	SSP0_BASE
};

//.........................................................................................................................
/* MOZART VPL SYSC base address */
static void *sysc_base;
//.........................................................................................................................
/* MOZART VPL PLLC base address */
static void *pllc_base;
//.........................................................................................................................
/* MOZART VPL APBC REQ_SEL for SSP0 */
static int dma_req_sel[MOZART_I2S_NUM] = {0x3};

//.........................................................................................................................
/* DMA ISR prototype */
static irqreturn_t audio_dma_isr(int irq, void *dev_id);

//.........................................................................................................................
const char vaudio_id[] = "$Vaudio: "VAUDIO_DRIVER_VERSION" "VAUDIO_PLATFORM" "__DATE__" "VAUDIO_COMPANY" $";

int vpl_audio_write_i2cdev_reg(struct i2c_client *i2cdev, int reg, int val)
{
	unsigned char buf[2] ;
	unsigned char recv[2] = {0} ;
	int ret = 0 ;

	buf[0] = (unsigned char)((reg << 1) | ((val >> 8) & 0x01));
	buf[1] = (unsigned char)(val & 0xff) ;

	if(i2c_master_send(i2cdev, buf, 2) != 2) {
		ret = -1 ;
	}
	ssm2603_reg_cache[reg] = val ;

	recv[0] = (unsigned char)(reg << 1) ;
	recv[1] = 0 ;
	i2c_master_recv(i2cdev, recv, 2) ;

	return ret ;
}

int vpl_audio_read_i2cdev_reg(struct i2c_client *i2cdev, int reg)
{
	int data = ssm2603_reg_cache[reg] ;

	return data;
}

/*==========================================================*/
/*
 * read DMA MMR
 */
void mozart_read_dma(struct dma_regs_t* dma_mmr, int chn)
{
	dma_mmr->dwSrc_Addr = MOZART_APBC_READL(APBC_DMA_SRC_ADDR(chn));
	dma_mmr->dwDes_Addr = MOZART_APBC_READL(APBC_DMA_DES_ADDR(chn));
	dma_mmr->dwLLP_Addr = MOZART_APBC_READL(APBC_DMA_LLP(chn));
	dma_mmr->dwCtrl = MOZART_APBC_READL(APBC_DMA_CTRL(chn));
}

/*==========================================================*/
/*
 * write DMA MMR
 */
void mozart_write_dma(struct dma_regs_t* dma_mmr, int chn)
{
	MOZART_APBC_WRITEL(APBC_DMA_SRC_ADDR(chn), dma_mmr->dwSrc_Addr);
	MOZART_APBC_WRITEL(APBC_DMA_DES_ADDR(chn), dma_mmr->dwDes_Addr);
	MOZART_APBC_WRITEL(APBC_DMA_LLP(chn), dma_mmr->dwLLP_Addr);
	MOZART_APBC_WRITEL(APBC_DMA_CTRL(chn), dma_mmr->dwCtrl);
}

/*==========================================================*/
/*
 * disable DMA
 */
void mozart_disable_dma(int dma_chn)
{
	DWORD reg;

	reg = MOZART_APBC_READL(APBC_DMA_CTRL(dma_chn));
	reg &= ~ 0x3;
	MOZART_APBC_WRITEL(APBC_DMA_CTRL(dma_chn), reg);
	reg = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);
	reg &= ~(1<<dma_chn);
	MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, reg);

}

/*==========================================================*/
/*
 * get DMA SRC_ADDR or DES_ADDR registers
 */
dma_addr_t mozart_get_dma_pos(int stream_id, int chn)
{
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		return (dma_addr_t)(MOZART_APBC_READL(APBC_DMA_SRC_ADDR(chn)));
	}
	else {
		return (dma_addr_t)(MOZART_APBC_READL(APBC_DMA_DES_ADDR(chn)));
	}
}

/*==========================================================*/
/*
 * set clock divider
 */
 void mozart_vvdc_config(DWORD dwSampleRate, int device)
{
	DWORD MDiv = 0;
	DWORD SR = 0;
   	DWORD reg;

	switch (dwSampleRate) {
	case 8000:
		MDiv = 22;	// MCLK = 12.288
		SR = 0xC;
		break;
	case 16000:
		MDiv = 22;	// MCLK = 12.288
		SR = 0x14;
		break;
	case 32000:
		MDiv = 22;	// MCLK = 12.288
		SR = 0x18;
		break;
	case 44100:
		MDiv = 24;	// MCLK = 11.2896
		SR = 0x20;
		break;
	case 48000:
		MDiv = 22;	// MCLK = 12.288
		SR = 0x0;
		break;
	default:
		MDiv = 22;
		SR = 0x18;
	}
	//buff[0] = 0x10;
	//buff[1] = SR;
	/* kernel i2c interface 2009.09.29 */
	/*if (ssm2603_i2c != NULL) {
		if (i2c_master_send(ssm2603_i2c, buff, 2) != 2) {
			printk(KERN_ERR "SSM2603: i2c write failed\n");
		}
	}*/
	if( vpl_audio_write_i2cdev_reg(ssm2603_i2c, SAMPLE_RATE_REG, SR) != 0) {
		printk("[ERR] Sample Rate Setting Failed!!\n") ;
		return ;
	}

	/* I2S0 TX/RX MDiv is set to SYSC_I2S_CLK_PARAM[7:0]
	    I2S1~I2S4 RX MDiv is set to SYSC_I2S_CLK_PARAM[15:8] (shared MDiv) */
	reg = readl((DWORD)sysc_base + SYSC_I2S_CLK_PARAM);
	reg &= ~((device == MOZART_I2S0) ? 0xFF : (0xFF<<8));
	reg &= ~(0xFF<<8);
	reg |= (device == MOZART_I2S0) ? MDiv : (MDiv<<8);
	reg |= MDiv<<8; //eve test
	writel(reg, (DWORD)sysc_base + SYSC_I2S_CLK_PARAM);
}

/*==========================================================*/
void mozart_pplc_config(u32 ratio)
{
	/* Set PLL2 to ratio 0x0203002F. i.e 144MHz */
	writel(0x00000009, pllc_base + PLLC_CTRL_2);   /* Enable Adjustment, Disable clock out */
	writel(ratio, pllc_base + PLLC_RATIO_2);
	writel(0x00000001, pllc_base + PLLC_CTRL_2); /* Disable Adjustment */

	/*Wait for PLL lock*/
	while ((readl(pllc_base + PLLC_CTRL_2) & 0x4) != 0x4)
	{
		printk("Unlock....\n");
	}

	/*Start to output the clock of PLL*/
	writel(0x00000003, pllc_base + PLLC_CTRL_2); /* Enable clock output */

}

/*==========================================================*/
/*
 * set clock divider
 */
 void mozart_vvdc_config_USB_Mode(DWORD dwSampleRate, int device)
{
	DWORD MDiv = 0;
	DWORD SR = 0;
   	DWORD reg;

	switch (dwSampleRate) {
	case 8000:
		MDiv = 4;	// MCLK = 24
		SR = 0xD;
		break;
	case 16000:
		MDiv = 4;	// MCLK = 24
		SR = 0x15;
		break;
	case 32000:
		MDiv = 4;	// MCLK = 24
		SR = 0x19;
		break;
	case 44100:
		MDiv = 4;	// MCLK = 24
		SR = 0x23;
		break;
	case 48000:
		MDiv = 4;	// MCLK = 24
		SR = 0x1;
		break;
	default:
		MDiv = 4;
		SR = 0x19;
	}

	mozart_pplc_config(0x0203002F); /* PLLOUT = 148MHz -> PLL_2_DIV_3 = 48MHz */

	/* I2S0 TX/RX MDiv is set to SYSC_I2S_CLK_PARAM[7:0]
	   I2S1~I2S4 RX MDiv is set to SYSC_I2S_CLK_PARAM[15:8] (shared MDiv) */

	/* 48MHz/4 = 12 MHz, SSM2603 support 12MHz USB Mode */
	reg = readl((DWORD)sysc_base + SYSC_I2S_CLK_PARAM);
	reg &= ~(0xFFFF);
	reg |= MDiv;
	reg |= MDiv<<8; //eve test
	writel(reg, (DWORD)sysc_base + SYSC_I2S_CLK_PARAM);

	/* In USB 12MHz Mode, Must Disable ACTIVE -> Change Sample Rate -> Enable ACTIVE.
	 * Otherwise, SSM2603 will not output correct WS and BCLK (by experiment)*/
	vpl_audio_write_i2cdev_reg(ssm2603_i2c, ACTIVE_REG, 0x00) ;

	if( vpl_audio_write_i2cdev_reg(ssm2603_i2c, SAMPLE_RATE_REG, SR) != 0) {
		printk("[ERR] Sample Rate Setting Failed!!\n") ;
		return ;
	}

	vpl_audio_write_i2cdev_reg(ssm2603_i2c, ACTIVE_REG, 0x01) ;

}

/*==========================================================*/
/*
 * set sample rate and format
 */
static void snd_i2s_set_samplerate(struct snd_i2s *chip, long rate, int device)
{

	if (rate > 0) {
		if(i2sClkSrc)  //USB Mode
			mozart_vvdc_config_USB_Mode(rate, device);
		else //Normal Mode
			mozart_vvdc_config(rate, device);
		udelay(1000);
		chip->samplerate[device] = rate;
	}
}

/*==========================================================*/
/*
 * enable/disable audio device
 */
static void audio_enable_device(struct audio_stream *s, int enable)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;

	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IER, (enable) ? 0x1 : 0x0);
}

/*==========================================================*/
/*
 * enable/disable audio stream
 */
 static void audio_enable_stream(struct audio_stream *s, int enable, int stream_id)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;

	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_ITER, (enable) ? 0x1 : 0x0);
	}
	else {
		MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IRER, (enable) ? 0x1 : 0x0);
	}
}

/*==========================================================*/
/*
 * setup LLP and drive DMA to process audio data
 */
static void audio_start_dma(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	int dev_num = dev_private->dev_num;
	unsigned int dma_size;
	unsigned int index;
	unsigned int offset;
	struct dma_regs_t *desc_addr = s->desc_addr;
	struct dma_regs_t *dma_mmr_init = NULL;

	dma_size = frames_to_bytes(runtime, runtime->period_size); // transfer one period size data each dma move
	dma_mmr_init = kmalloc(sizeof(*dma_mmr_init), GFP_ATOMIC);
	if (dma_mmr_init == NULL) {
		printk("No memory.\n");
	}

	/* prepare LLP */
	if (s->stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		dma_mmr_init->dwSrc_Addr = runtime->dma_addr;
		dma_mmr_init->dwDes_Addr = ssp_base[dev_num] + I2S_TXDMA;
		dma_mmr_init->dwLLP_Addr = virt_to_bus(desc_addr);
		dma_mmr_init->dwCtrl = 0x50159 |
							(dma_req_sel[dev_num] << 12) |
							((dma_size / 8) << 20);
		for (index = 0; index < (runtime->periods); index++) {
			offset = dma_size * ((index + 1) % runtime->periods);
			desc_addr[index].dwSrc_Addr = dma_mmr_init->dwSrc_Addr + offset;
			desc_addr[index].dwDes_Addr = dma_mmr_init->dwDes_Addr;
			desc_addr[index].dwLLP_Addr = dma_mmr_init->dwLLP_Addr + sizeof(struct dma_regs_t) * ((index + 1) % runtime->periods);
			desc_addr[index].dwCtrl = dma_mmr_init->dwCtrl;
		}
	}
	else {
		dma_mmr_init->dwSrc_Addr = ssp_base[dev_num] + I2S_RXDMA;
		dma_mmr_init->dwDes_Addr = runtime->dma_addr;
		dma_mmr_init->dwLLP_Addr = virt_to_bus(desc_addr);
		dma_mmr_init->dwCtrl = 0x50499 |
							(dma_req_sel[dev_num] << 12) |
							((dma_size / 8) << 20);
		for (index = 0; index < (runtime->periods); index++) {
			offset = dma_size * ((index + 1) % runtime->periods);
			desc_addr[index].dwSrc_Addr = dma_mmr_init->dwSrc_Addr;
			desc_addr[index].dwDes_Addr = dma_mmr_init->dwDes_Addr + offset;
			desc_addr[index].dwLLP_Addr = dma_mmr_init->dwLLP_Addr + sizeof(struct dma_regs_t) * ((index + 1) % runtime->periods);
			desc_addr[index].dwCtrl = dma_mmr_init->dwCtrl;
		}
	}
	dmac_flush_range((void *)(unsigned long)desc_addr, (void *)(unsigned long)(desc_addr + 16 * (runtime->periods) -1));
	mozart_write_dma(dma_mmr_init, dev_private->dma_chn);
	kfree(dma_mmr_init);
}

/*==========================================================*/
/*
 * this stops dma
 */
static void audio_stop_dma(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	int dma_chn = dev_private->dma_chn;

	// 1. Disable DMA
	mozart_disable_dma(dma_chn);

	// 2. Free LLP
	if (s->desc_addr) {
		kfree(s->desc_addr);
	}
}


/*==========================================================*/
/*
 * DMA ISR
 */
static irqreturn_t audio_dma_isr(int irq, void *dev_id)
{
	struct snd_i2s *chip = dev_id;
	struct audio_stream *s, *s1;
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	snd_i2s_dev_private *dev_private;
	unsigned long chn_monitor;
	int index;

	chn_monitor = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);
	for (index = MOZART_I2S0; index < MOZART_I2S_NUM; index++) {
		s = &chip->s[index][SNDRV_PCM_STREAM_PLAYBACK];
		s1 = &chip->s[index][SNDRV_PCM_STREAM_CAPTURE];
		if (s1) {
			if (s1->active && s1->stream) {
				substream = s1->stream;
				runtime = substream->runtime;
				dev_private = runtime->private_data;

				if ((1<<(dev_private->dma_chn)) & chn_monitor) {
					MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, ~(1<<(dev_private->dma_chn)));
					snd_pcm_period_elapsed(s1->stream);
					return IRQ_HANDLED;
				}
			}
		}
		if (s) {
			if (s->active && s->stream) {

				substream = s->stream;
				runtime = substream->runtime;
				dev_private = runtime->private_data;
				chn_monitor = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);
				if ((1<<(dev_private->dma_chn)) & chn_monitor) {
					MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, ~(1<<(dev_private->dma_chn)));
					snd_pcm_period_elapsed(s->stream);
					return IRQ_HANDLED;
				}
			}
		}
	}
	return IRQ_NONE;
}

/*==========================================================*/
/*
 * get DMA current position
 */
static u_int audio_get_dma_pos(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	unsigned int offset;
	dma_addr_t addr;

	addr = mozart_get_dma_pos(s->stream_id, dev_private->dma_chn);
	offset = addr - runtime->dma_addr;

	offset = bytes_to_frames(runtime, offset);
	if (offset >= runtime->buffer_size) {
		offset = 0;
	}
	return offset;
}

/*==========================================================*/
/*
 * request irq
 */
static int audio_dma_irq_request(struct snd_i2s *chip)
{
	mutex_lock(&chip->dma_irq_mutex);
	if (chip->dma_irq < 0) {
		chip->dma_irq = APBC_IRQ_NUM;
		if(request_irq (chip->dma_irq, audio_dma_isr, IRQF_SHARED, "APBC DMA", chip)) 	{
			printk(KERN_EMERG "irq request fail\n");
			mutex_unlock(&chip->dma_irq_mutex);
			return -EBUSY;
		}
		// set Level Trigger for LLP interrupts
		fLib_SetIntTrig(chip->dma_irq, LEVEL, H_ACTIVE);
	}
	chip->dma_irq_acks++;
	mutex_unlock(&chip->dma_irq_mutex);
	return 0;
}

/*==========================================================*/
/*
 * free irq
 */
static void audio_dma_irq_free(struct snd_i2s *chip)
{
	mutex_lock(&chip->dma_irq_mutex);
	if (chip->dma_irq_acks > 0) {
		chip->dma_irq_acks--;
	}
	if (chip->dma_irq_acks == 0 && chip->dma_irq >= 0) {
		free_irq(chip->dma_irq, chip);
		chip->dma_irq = -1;
	}
	mutex_unlock(&chip->dma_irq_mutex);
}


/*==========================================================*/
/*
 * init audio device
 */
static void snd_i2s_audio_init(struct snd_i2s *chip, int device)
{
	unsigned int dev_base = ssp_base[device];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	/* Setup DMA stuff */
	chip->s[device][SNDRV_PCM_STREAM_PLAYBACK].stream_id = SNDRV_PCM_STREAM_PLAYBACK;
	chip->s[device][SNDRV_PCM_STREAM_CAPTURE].stream_id = SNDRV_PCM_STREAM_CAPTURE;


	/* Initialize I2S */
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IER, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IRER, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_ITER, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IMR, 0x0);
// remove for slave mode
//	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_CCR, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFCR, 0x3);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TFCR, 0x3);

	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFF0, 0x1);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TFF0, 0x1);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x2);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TCR, 0x2);


	snd_i2s_set_samplerate(chip, chip->samplerate[device], device);
}

/*==========================================================*/
/*
 * snd_pcm_ops : trigger
 */
static int snd_i2s_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct snd_i2s *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	struct audio_stream *s, *s1;
	int err = 0;

	s = &chip->s[dev_private->dev_num][substream->pstr->stream];
	s1 = &chip->s[dev_private->dev_num][substream->pstr->stream ^ 1];
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		if ((runtime->dma_addr % 8) != 0) {
			printk(KERN_EMERG "runtime->dma_addr not aligned with APBC DMA data size!\n");
			return -EPERM;
		}
		s->active = 1;
		audio_start_dma(s);
		audio_enable_device(s, 1);
		audio_enable_stream(s, 1, substream->pstr->stream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
		s->active = 0;
		audio_stop_dma(s);
		audio_enable_stream(s, 0, substream->pstr->stream);
		if (s1->active == 0) {
			audio_enable_device(s, 0);
		}
		break;

	default:
		err = -EINVAL;
		break;
	}
	return err;
}

/*==========================================================*/
/*
 * snd_pcm_ops : prepare
 */
static int snd_i2s_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_i2s *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	struct audio_stream *s = &chip->s[dev_private->dev_num][substream->pstr->stream];
	struct audio_stream *s1 = &chip->s[dev_private->dev_num][substream->pstr->stream ^ 1];

	if (s1->active != 1) {
		snd_i2s_set_samplerate(chip, runtime->rate, dev_private->dev_num);
	}
	else if (chip->samplerate[dev_private->dev_num] != runtime->rate) {
		printk(KERN_EMERG "[I2S driver] WARNING : The samplerate %d is not set successfully into I2S%d, because the other stream is running under %ld samplerate.\n", runtime->rate, dev_private->dev_num, chip->samplerate[dev_private->dev_num]);
		return -EINVAL;
	}
 	s->dma_pos = bytes_to_frames(runtime, runtime->dma_addr);

	if(vpl_audio_write_i2cdev_reg(ssm2603_i2c, POWER_MANAGE_REG, 0x00) != 0) {
		printk("[ERR] Power on SSM2603 failed!!\n") ;
		return -1 ;
	}

	s->desc_addr = kmalloc((runtime->periods)*sizeof(struct dma_regs_t), GFP_ATOMIC);
	if (s->desc_addr == NULL) {
		printk("No memory.\n");
		return -ENOMEM;
	}
	return 0;
}


/*==========================================================*/
/*
 * snd_pcm_ops : pointer
 */
static snd_pcm_uframes_t snd_i2s_pointer(struct snd_pcm_substream *substream)
{
	struct snd_i2s *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	struct audio_stream *s = &chip->s[dev_private->dev_num][substream->pstr->stream];
	snd_pcm_uframes_t ret;

	ret = audio_get_dma_pos(s);
	return ret;
}

/*==========================================================*/
/*
 * snd_pcm_ops : hw_params
 */
static int snd_i2s_pcm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params)
{
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

/*==========================================================*/
/*
 * snd_pcm_ops : hw_free
 */
static int snd_i2s_pcm_hw_free(struct snd_pcm_substream *substream)
{
	return snd_pcm_lib_free_pages(substream);
}

/*==========================================================*/
/*
 * playback hw info
 */
static struct snd_pcm_hardware snd_i2s_playback =
{
	.info =		 (SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_NONINTERLEAVED |
				   SNDRV_PCM_INFO_BLOCK_TRANSFER |
				   SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID),
	.formats =	SNDRV_PCM_FMTBIT_S16_LE,
	.rates =		(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |\
                                   SNDRV_PCM_RATE_32000 |SNDRV_PCM_RATE_44100 |\
                                   SNDRV_PCM_RATE_48000 |SNDRV_PCM_RATE_KNOT),
	.rate_min =		8000,
	.rate_max =		48000,
	.channels_min =		2,
	.channels_max =		2,
	.periods_min =		2,
	.periods_max =		8192,
	.buffer_bytes_max =	1024 * 1024,
	.period_bytes_min =	128,
	.period_bytes_max =	PERIOD_BYTES_MAX,
};

/*==========================================================*/
/*
 * capture hw info
 */
static struct snd_pcm_hardware snd_i2s_capture =
{
	.info =		 (SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_NONINTERLEAVED |
				   SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID),
	.formats =	SNDRV_PCM_FMTBIT_S16_LE,
	.rates =		 (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |\
				   SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |\
				   SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_KNOT),
	.rate_min =		8000,
	.rate_max =		48000,
	.channels_min =		2,
	.channels_max =		2,
	.periods_min =		2,
	.periods_max =		8192,
	.buffer_bytes_max =	1024 * 1024,
	.period_bytes_min =	128,
	.period_bytes_max =	PERIOD_BYTES_MAX,
};

/*==========================================================*/
/*
 * capture hw info
 */
static void snd_i2s_free_private_data(struct snd_pcm_runtime *runtime)
{
	if (runtime->private_data) 	{
		kfree(runtime->private_data);
	}
}

/*==========================================================*/
/*
 * snd_pcm_ops : open, for i2s0
 */
static int snd_i2s0_open(struct snd_pcm_substream *substream)
{
	struct snd_i2s *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = NULL;
	int stream_id = substream->pstr->stream;
	int err;

	if ((err = audio_dma_irq_request(chip)) < 0) {
		return err;
	}
	chip->s[MOZART_I2S0][stream_id].stream = substream;
	dev_private = kmalloc(sizeof(*dev_private), GFP_ATOMIC);
	dev_private->dev_num = MOZART_I2S0;
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		dev_private->dma_chn = DMA_CHN_7;
		runtime->hw = snd_i2s_playback;
	}
	else {
		dev_private->dma_chn = DMA_CHN_6;
		runtime->hw = snd_i2s_capture;
	}
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0)
		return err;
	if ((err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, &constraints_rates)) < 0)
		return err;

	runtime->private_data = dev_private;
	runtime->private_free = snd_i2s_free_private_data;
	return 0;
}

/*==========================================================*/
/*
 * snd_pcm_ops : close
 */
static int snd_i2s_close(struct snd_pcm_substream *substream)
{
	struct snd_i2s *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;

	audio_dma_irq_free(chip);
	chip->s[dev_private->dev_num][substream->pstr->stream].stream = NULL;
	return 0;
}

/*==========================================================*/
/*
 *  pcm pos
 */
 /* for I2S0 */
static struct snd_pcm_ops snd_i2s0_playback_ops = {
	.open = snd_i2s0_open,
	.close = snd_i2s_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_i2s_pcm_hw_params,
	.hw_free = snd_i2s_pcm_hw_free,
	.prepare = snd_i2s_pcm_prepare,
	.trigger = snd_i2s_trigger,
	.pointer = snd_i2s_pointer,
};
static struct snd_pcm_ops snd_i2s0_capture_ops = {
	.open = snd_i2s0_open,
	.close = snd_i2s_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_i2s_pcm_hw_params,
	.hw_free = snd_i2s_pcm_hw_free,
	.prepare = snd_i2s_pcm_prepare,
	.trigger = snd_i2s_trigger,
	.pointer = snd_i2s_pointer,
};

/*==========================================================*/
/*
 * create pcm instances
 */
static int snd_i2s_pcm(struct snd_i2s *chip, int dev)
{
	struct snd_pcm *pcm;
	int err;

	if((err = snd_pcm_new(chip->card, "I2S AIC23 PCM", dev, 1, 1, &pcm))<0) {
		return err;
	}
	err = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV, NULL, 64*1024, 64*1024);
	if (err < 0) {
		printk(KERN_EMERG "Preallocate memory fail.\n");
	}
	switch (dev) {
	case MOZART_I2S0:
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_i2s0_playback_ops);
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_i2s0_capture_ops);
		break;
 	default:
		break;
	}
	pcm->private_data = chip;
	pcm->info_flags = 0;
	strcpy(pcm->name, chip->card->shortname);

	chip->pcm[dev] = pcm;
	chip->samplerate[dev] = 8000; // DEFAULT_SAMPLERATE

	/* init audio dev */
	snd_i2s_audio_init(chip, dev);

	return 0;
}

/*======================= I2C related ===========================*/
static int vpl_audio_sidetone_gain_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	printk("[SSM2603] Sidetone Gain\n") ;
	printk("         - 0 : Disable!\n") ;
	printk("         - 1 ~ 4 : siedtone gain.\n") ;

	return 0;
}

static int vpl_audio_sidetone_gain_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int val = vpl_audio_read_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG) ;

	if((val & (1 << 5)) == 0) {
		//disable sidetone
		value->value.integer.value[0] = 0 ;
	}
	else {
		val = val >> 6 ;
		val &= 0x3 ;

		if(val == 0)
			value->value.integer.value[0] = 1 ;
		else if(val == 1)
			value->value.integer.value[0] = 2 ;
		else if(val == 2)
			value->value.integer.value[0] = 3 ;
		else if(val == 3)
			value->value.integer.value[0] = 4 ;
	}

	return 0 ;
}

static int vpl_audio_sidetone_gain_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int gain = value->value.integer.value[0] ;
	int tmp = vpl_audio_read_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG) ;

	if(gain == 0) {
		//disable sidetone
		tmp &= ~(1 << 5) ;
	}
	else if (gain == 1) {
		//enable sidetone
		tmp |= 1 << 5 ;
		//set gain
		tmp &= ~(0x3 << 6) ;
		tmp |= 0x0 << 6 ;
	}
	else if (gain == 2) {
		//enable sidetone
		tmp |= 1 << 5 ;
		//set gain
		tmp &= ~(0x3 << 6) ;
		tmp |= 0x1 << 6 ;
	}
	else if (gain == 3) {
		//enable sidetone
		tmp |= 1 << 5 ;
		//set gain
		tmp &= ~(0x3 << 6) ;
		tmp |= 0x2 << 6 ;
	}
	else if (gain == 4) {
		//enable sidetone
		tmp |= 1 << 5 ;
		//set gain
		tmp &= ~(0x3 << 6) ;
		tmp |= 0x3 << 6 ;
	}
	else {
		printk(KERN_DEBUG "[ERR] Wrong input..only #0 ~ #4!!\n") ;
		return -1 ;
	}

	vpl_audio_write_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG, tmp) ;

    return 0;
}

static int vpl_audio_mic_mute_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	printk("[SSM2603] Mic Mute\n") ;
	printk("         - 0 : No mute!\n") ;
	printk("         - 1 : Mute!\n") ;

	return 0;
}

static int vpl_audio_mic_mute_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int mute = vpl_audio_read_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG) & (1 << 1) ;

	value->value.integer.value[0] = (mute == 0)? 0 : 1 ;

	return 0 ;
}

static int vpl_audio_mic_mute_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int mute = value->value.integer.value[0] ;
	int tmp = vpl_audio_read_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG) ;

	if(mute) {
		tmp |= 1 << 1 ;
	}
	else {
		tmp &= ~(1 << 1) ;
	}

	vpl_audio_write_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG, tmp) ;

    return 0;
}

static int vpl_audio_input_select_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	printk("[SSM2603] Audio Input Select\n") ;
	printk("         - 0 : Bypass(Line-in => Line-out)\n") ;
	printk("         - 1 : Line-in as the input.\n") ;
	printk("         - 2 : Mic-in as the input.\n") ;
	return 0;
}

static int vpl_audio_input_select_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int tmp = vpl_audio_read_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG) ;

	if( (tmp & (1 << 3)) ) {//BYPASS?
		value->value.integer.value[0] = 0 ;
	}
	else if( !(tmp & (1 << 2)) ) {//Line-in
		value->value.integer.value[0] = 1 ;
	}
	else {//Mic-in
		value->value.integer.value[0] = 2 ;
	}

	return 0 ;
}

static int vpl_audio_input_select_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int selection = value->value.integer.value[0] ;
	int tmp = vpl_audio_read_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG) ;

	if(selection == 0) {//Bypass
		tmp |= (1 << 3) ;
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG, tmp);
	}
	else if(selection == 1) {//Line-in
		//disable BYPASS
		tmp &= ~(1 << 3);
		//select Line-in
		tmp &= ~(1 << 2);
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG, tmp);
	}
	else if(selection == 2) {
		//disable BYPASS
		tmp &= ~(1 << 3);
		//select Mic-in
		tmp |= 1 << 2 ;
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG, tmp);
	}
	else {
		printk(KERN_DEBUG "[ERR] Wrong Input Selection.\n") ;
		return -1 ;
	}

    return 0;
}

//capture volume : 6b'00,0000 ~ 7b'10,1101(0~45 == 1~46)
//others          : mute(0)
//i.e. send 0      ==> mute
//     send 1 ~ 46 ==> normal
//     send others ==> wrong!
static int vpl_audio_capture_volume_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	return 0;
}

static int vpl_audio_capture_volume_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int val = vpl_audio_read_i2cdev_reg(ssm2603_i2c, LEFT_ADC_VOLUME_REG) ;

	if(val & (1 << 7)) {
		//mute mode
		value->value.integer.value[0] = 0 ;
	}
	else {
		//normal mode, 1 ~ 46
		value->value.integer.value[0] = (val & 0x3F) + 1 ;
	}

	//printk("vpl_audio_capture_volume_get = %d\n", value->value.integer.value[0] ) ;

	return 0 ;
}

static int vpl_audio_capture_volume_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int new_volume = value->value.integer.value[0] ;
	int ori_value ;

	printk(KERN_DEBUG "Capture Volume(put)\n") ;
	printk(KERN_DEBUG " - put value = 0x%x\n", new_volume) ;

    //mute case
	if(new_volume == 0) {
		//set volume
		ori_value = vpl_audio_read_i2cdev_reg(ssm2603_i2c, LEFT_ADC_VOLUME_REG) ;
		new_volume = ori_value | (1 << 7) ;
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, LEFT_ADC_VOLUME_REG, new_volume);

		ori_value = vpl_audio_read_i2cdev_reg(ssm2603_i2c, RIGHT_ADC_VOLUME_REG) ;
		new_volume = ori_value | (1 << 7) ;
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, RIGHT_ADC_VOLUME_REG, new_volume);
	}
	else if((new_volume >= 1) && (new_volume <= 46)) {
		new_volume -= 1 ;

		//set volume
		ori_value = vpl_audio_read_i2cdev_reg(ssm2603_i2c, LEFT_ADC_VOLUME_REG) ;
		ori_value &= ~(0x3F) ;
		ori_value &= ~(1 << 7) ;//no more mute
		new_volume = ori_value | new_volume ;
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, LEFT_ADC_VOLUME_REG, new_volume);

		ori_value = vpl_audio_read_i2cdev_reg(ssm2603_i2c, RIGHT_ADC_VOLUME_REG) ;
		ori_value &= ~(0x3F) ;
		ori_value &= ~(1 << 7) ;//no more mute
		new_volume = ori_value | new_volume ;
		vpl_audio_write_i2cdev_reg(ssm2603_i2c, RIGHT_ADC_VOLUME_REG, new_volume);
	}
	//wrong case
	else {
		printk(KERN_DEBUG "[ERR] Only #0 ~ #46!!\n") ;
		return -1 ;
	}

    return 0;
}

//playback volume : 7b'0110000 ~ 7b'1111111(48 ~ 127 == 1 ~ 80)
//others          : mute(0)
//i.e. send 0      ==> mute
//     send 1 ~ 80 ==> normal
//     send others ==> wrong!
static int vpl_audio_playback_volume_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	return 0;
}

static int vpl_audio_playback_volume_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int val = vpl_audio_read_i2cdev_reg(ssm2603_i2c, LEFT_DAC_VOLUME_REG) ;

	if((val >= 48) && (val <= 127)) {
		val -= 47 ;
	}
	else
		val = 0 ;

	value->value.integer.value[0] = val ;

	return 0 ;
}

static int vpl_audio_playback_volume_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int new_volume = value->value.integer.value[0] ;

	printk(KERN_DEBUG "Playback Volume(put)\n") ;
	printk(KERN_DEBUG " - put value = 0x%x\n", new_volume) ;

    //mute case
	if(new_volume == 0) {
		new_volume = 0 ;
	}
	else if((new_volume >= 1) && (new_volume <= 80)) {
		new_volume += 47 ;
	}
	//wrong case
	else {
		printk(KERN_DEBUG "[ERR] Only #0 ~ #80!!\n") ;
		return -1 ;
	}

	//set volume
	vpl_audio_write_i2cdev_reg(ssm2603_i2c, LEFT_DAC_VOLUME_REG, new_volume);
	vpl_audio_write_i2cdev_reg(ssm2603_i2c, RIGHT_DAC_VOLUME_REG, new_volume);

    return 0;
}

static struct snd_kcontrol_new vpl_audio_mixer_array[] = {
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Side-tone Ctrl",
		.info = vpl_audio_sidetone_gain_info,
		.get = vpl_audio_sidetone_gain_get,
		.put = vpl_audio_sidetone_gain_put,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Mic Mute",
		.info = vpl_audio_mic_mute_info,
		.get = vpl_audio_mic_mute_get,
		.put = vpl_audio_mic_mute_put,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Input Selection",
		.info = vpl_audio_input_select_info,
		.get = vpl_audio_input_select_get,
		.put = vpl_audio_input_select_put,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Capture Volume",
		.info = vpl_audio_capture_volume_info,
		.get = vpl_audio_capture_volume_get,
		.put = vpl_audio_capture_volume_put,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Playback Volume",
		.info = vpl_audio_playback_volume_info,
		.get = vpl_audio_playback_volume_get,
		.put = vpl_audio_playback_volume_put,
	}
} ;


static inline int ssm2603_reset(void)
{
	return vpl_audio_write_i2cdev_reg(ssm2603_i2c, SW_RESET_REG, 0x00) ;
}

int codec_init(void)
{
	int ret = 0 ;

	ret = ssm2603_reset() ;
	if(ret < 0) {
		printk("[ERR] Reset the codec failed!!\n") ;
		return -1 ;
	}

	//init every registers..
	ret = 0 ;
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, LEFT_ADC_VOLUME_REG, 0x17);	// LLINE_VOLCTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, RIGHT_ADC_VOLUME_REG, 0x17);	// RLINE_VOLCTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, LEFT_DAC_VOLUME_REG, 0x79);	// LHEADPHONE_VOLCTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, RIGHT_DAC_VOLUME_REG, 0x79);	// RHEADPHONE_VOLCTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, ANA_AUDIO_PATH_REG, 0x113);	// ANA_AUDIOPATH_CTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, DIG_AUDIO_PATH_REG, 0x00);	// DIGI_AUDIOPATH_CTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, POWER_MANAGE_REG, 0x60);		// POWERDOWN_CTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, DIGI_AUDIO_IF_REG, 0x42);		// DIGI_INTERFACE_FORMAT
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, SAMPLE_RATE_REG, 0x00) ;		// SAMPLERATE_CTRL
	ret |= vpl_audio_write_i2cdev_reg(ssm2603_i2c, ACTIVE_REG, 0x01) ;			// ACTIVE

	if(ret != 0) {
		printk("[ERR] Initialize SSM2603 failed!!\n") ;
		return -1 ;
	}

	return 0 ;
}

static int ssm2603_codec_probe(struct i2c_adapter *adap, int addr, int kind)
{
	int ret;
	u32 reg;

	client_template.adapter = adap;
	client_template.addr = addr;
 	ssm2603_i2c = kmemdup(&client_template, sizeof(client_template), GFP_KERNEL);
	if (ssm2603_i2c == NULL) {
		printk(KERN_ERR "new i2c fail..\n");
		return -ENOMEM;
	}
	ret = i2c_attach_client(ssm2603_i2c);
	if(ret < 0) {
		printk("SSM2603 failed to attach at addr %x\n", addr);
		goto err;
	}

	/* Setup PLL clock source */
	if(i2sClkSrc) {
		reg =  readl((DWORD)sysc_base + SYSC_IF_CTRL);
		reg |=  (0x1<<25); /* PLL2 Source 0:18.432 MHz / 1:24 MHz*/
		writel(reg, (DWORD)sysc_base + SYSC_IF_CTRL);
	}

	ret = codec_init() ;
	if(ret < 0) {
		printk("SSM2603 failed to init!!\n");
		goto err;
	}

	return ret;

err:
	kfree(ssm2603_i2c);
	return ret;

}

static int ssm2603_i2c_detach(struct i2c_client *client)
{
	i2c_detach_client(client);
	kfree(client);
	return 0;
}

static int ssm2603_i2c_attach(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, ssm2603_codec_probe);
}

#define I2C_DRIVERID_SSM2603 0xfefe /* need a proper id */
/* corgi i2c codec control layer */
static struct i2c_driver ssm2603_i2c_driver = {
	.driver = {
		.name = "SSM2603 I2C Codec",
		.owner = THIS_MODULE,
	},
	.id =             I2C_DRIVERID_SSM2603,
	.attach_adapter = ssm2603_i2c_attach,
	.detach_client =  ssm2603_i2c_detach,
	.command =        NULL,
};

static struct i2c_client client_template = {
	.name =   "SSM2603",
	.driver = &ssm2603_i2c_driver,
};

/*==================== Basic Flow of Drivers ========================*/
 void snd_i2s_free(struct snd_card *card)
{
	struct snd_i2s *chip = card->private_data;

	audio_dma_irq_free(chip);
}

//.........................................................................................................................
static int snd_i2s_probe(struct platform_device *devptr)
{
	int err;
	struct snd_card *card;
	struct snd_i2s *chip;
	unsigned long i, ret;
	struct snd_kcontrol_new *kctl;

	/* kernel i2c interface 2009.09.29 */
	ret = i2c_add_driver(&ssm2603_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "can't add i2c driver");
	}

	/* register the soundcard */
	card = snd_card_new(-1, id, THIS_MODULE, sizeof(struct snd_i2s));
	if (card == NULL)
		return -ENOMEM;

	card->dev = &devptr->dev;
	strncpy(card->driver, devptr->dev.driver->name, sizeof(card->driver));

	chip = card->private_data;

 	card->private_free = snd_i2s_free;
	chip->card = card;

	chip->dma_irq = -1;
	mutex_init(&chip->dma_irq_mutex);

	strcpy(card->driver, "I2S");
	sprintf(card->shortname, "I2S driver");
	sprintf(card->longname, "I2S driver");

	snd_card_set_dev(card, &devptr->dev);

	// PCM
	for (i = MOZART_I2S0; i < MOZART_I2S_NUM; i++) 	{
		if ((err = snd_i2s_pcm(chip, i)) < 0)
			goto nodev;
	}

	/* Mixer controls */
	strcpy(card->mixername, "Audio-Mixer");
	for(i = 0 ; i < ARRAY_SIZE(vpl_audio_mixer_array) ; i++) {
		kctl = &vpl_audio_mixer_array[i] ;
		kctl->count = 1;
		err = snd_ctl_add(card, snd_ctl_new1(kctl, chip));
		if (err < 0)
			return err;
	}

	if ((err = snd_card_register(card)) == 0) {
		platform_set_drvdata(devptr, card);
		return 0;
	}

 nodev:
	snd_card_free(card);
	return err;
}

//.........................................................................................................................
static int __devexit snd_i2s_remove(struct platform_device *devptr)
{
	int ret;

	ret = snd_card_free(platform_get_drvdata(devptr));
	if (ret != 0) {
		printk(KERN_EMERG "snd_card_free returns %d\n", ret);
	}
	platform_set_drvdata(devptr, NULL);
	/* kernel i2c interface 2009.09.29 */
	i2c_del_driver(&ssm2603_i2c_driver);
	return 0;
}

//.........................................................................................................................
#define SND_I2S_DRIVER	"Mozart_SSM2603"

static struct platform_driver snd_i2s_driver = {
	.probe		= snd_i2s_probe,
	.remove		= __devexit_p(snd_i2s_remove),
	.driver		= {
		.name	= SND_I2S_DRIVER,
	},
};

//.........................................................................................................................
static int __init alsa_card_i2s_init(void)
{
	int err;

	request_mem_region(SYSC_BASE, 0x100, "SYSC");
	sysc_base = ioremap(SYSC_BASE, 0x100);
	pllc_base = ioremap(MOZART_PLLC_MMR_BASE, 0x100);

	if ((err = platform_driver_register(&snd_i2s_driver)) < 0) {
		return err;
	}
	device = platform_device_register_simple(SND_I2S_DRIVER, -1, NULL, 0);
	if (!IS_ERR(device)) {
		if (platform_get_drvdata(device)) {
			return 0;
		}
		platform_device_unregister(device);
		err = -ENODEV;
	}
	else {
		err = PTR_ERR(device);
	}
	platform_driver_unregister(&snd_i2s_driver);
	return err;
}

//.........................................................................................................................
static void alsa_card_i2s_exit(void)
{
	iounmap(sysc_base);
	release_mem_region(SYSC_BASE,0x100);
	iounmap(pllc_base);
	platform_device_unregister(device);
	platform_driver_unregister(&snd_i2s_driver);
}

//.........................................................................................................................
module_init(alsa_card_i2s_init)
module_exit(alsa_card_i2s_exit)
