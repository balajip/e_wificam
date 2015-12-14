/*
 * Mozart_Audio_WAU8822
 * Driver for Mozart WAU8822 Audio Codec Driver
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
 
#include <sound/driver.h>
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

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>	// add for ASOC

#include <asm/cacheflush.h>
#include <linux/dma-mapping.h> // add for "dma_alloc_coherent"

#include "vaudio_locals.h"

#ifdef DEBUG_PRINTK
#define DEBUG(args...) printk(KERN_DEBUG args)
#else
#define DEBUG(args...)
#endif

#define VAUDIO_DRIVER_VERSION		"3.0.0.4"
#define VAUDIO_COMPANY				"______ Inc."
#define VAUDIO_PLATFORM				"EVM"

const char vaudio_id[] = "$Vaudio: "VAUDIO_DRIVER_VERSION" "VAUDIO_PLATFORM" "__DATE__" "VAUDIO_COMPANY" $";


static struct dma_regs_t *vpl_audio_apb_descbuf_playback ;
static struct dma_regs_t *vpl_audio_apb_descbuf_capture ;

static struct snd_pcm_hardware mozart_pcm_hardware =
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
	.buffer_bytes_max =	1024 * 1024,//evetest
	.period_bytes_min =	128,
	.period_bytes_max =	PERIOD_BYTES_MAX,
};

struct mozart_runtime_data {
	spinlock_t lock;
	int dev_num;		/* device num */		
	struct mozart_pcm_dma_params *dma_data;	/* DMA params */
};

static unsigned int ssp_base[MOZART_I2S_NUM] = {
	SSP0_BASE
};
static int dma_req_sel[MOZART_I2S_NUM] = {
	0x3
};
static irqreturn_t audio_dma_isr(int irq, void *dev_id);

static void mozart_i2s_enable_device(int dev_num, int enable)
{
	unsigned int dev_base = ssp_base[dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	DEBUG("[eve ASOC] mozart_i2s_enable_device.\n");
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IER, (enable) ? 0x1 : 0x0);
}

 static void mozart_i2s_enable_stream(int dev_num, int enable, int stream_id)
{
	unsigned int dev_base = ssp_base[dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	DEBUG("[eve ASOC] mozart_i2s_enable_stream.\n");
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_ITER, (enable) ? 0x1 : 0x0);
	}
	else {
		MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IRER, (enable) ? 0x1 : 0x0);		
	}	
}

static void mozart_i2s_init(int device)
{
	unsigned int dev_base = ssp_base[device];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	DEBUG("[eve ASOC] mozart_i2s_init\n");		
	/* Initialize I2S */
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IER, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IRER, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_ITER, 0x0);	
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IMR, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFCR, 0x3);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TFCR, 0x3);
	
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFF0, 0x1);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TFF0, 0x1);		
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x2);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TCR, 0x2);	

}

void mozart_read_dma(struct dma_regs_t* dma_mmr, int chn)
{
	dma_mmr->dwSrc_Addr = MOZART_APBC_READL(APBC_DMA_SRC_ADDR(chn));
	dma_mmr->dwDes_Addr = MOZART_APBC_READL(APBC_DMA_DES_ADDR(chn));	
	dma_mmr->dwLLP_Addr = MOZART_APBC_READL(APBC_DMA_LLP(chn));		
	dma_mmr->dwCtrl = MOZART_APBC_READL(APBC_DMA_CTRL(chn));
}

void mozart_write_dma(struct dma_regs_t* dma_mmr, int chn)
{
	MOZART_APBC_WRITEL(APBC_DMA_SRC_ADDR(chn), dma_mmr->dwSrc_Addr);
	MOZART_APBC_WRITEL(APBC_DMA_DES_ADDR(chn), dma_mmr->dwDes_Addr);	
	MOZART_APBC_WRITEL(APBC_DMA_LLP(chn), dma_mmr->dwLLP_Addr);		
	MOZART_APBC_WRITEL(APBC_DMA_CTRL(chn), dma_mmr->dwCtrl);
}

void mozart_disable_dma(int dma_chn)
{
	unsigned long reg;

	reg = MOZART_APBC_READL(APBC_DMA_CTRL(dma_chn));
	reg &= ~0x3;
	MOZART_APBC_WRITEL(APBC_DMA_CTRL(dma_chn), reg);
	reg = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);	
	reg &= ~(1<<dma_chn);
	MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, reg);	
	
}

static void mozart_dma_getposition(int chn, dma_addr_t* src, dma_addr_t* dst)
{
	*src = (dma_addr_t)(MOZART_APBC_READL(APBC_DMA_SRC_ADDR(chn)));
	*dst = (dma_addr_t)(MOZART_APBC_READL(APBC_DMA_DES_ADDR(chn)));
}


static void mozart_pcm_start_dma(struct snd_pcm_substream *substream)
{
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int dev_num = prtd->dev_num;
	unsigned int dma_size;
	unsigned int index;
	unsigned int offset;
	struct dma_regs_t *desc_addr = prtd->dma_data->desc_addr;
	struct dma_regs_t dma_mmr_init;

	DEBUG("[eve ASOC] mozart_pcm_start_dma\n");		
	dma_size = snd_pcm_lib_period_bytes(substream); // transfer one period size data each dma move
	memset(&dma_mmr_init, 0, sizeof(dma_mmr_init));
	
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		dma_mmr_init.dwSrc_Addr = runtime->dma_addr;
		dma_mmr_init.dwDes_Addr = ssp_base[dev_num] + I2S_TXDMA;
		dma_mmr_init.dwLLP_Addr = virt_to_bus(desc_addr);
		dma_mmr_init.dwCtrl = 0x50159 |
							(dma_req_sel[dev_num] << 12) |
							((dma_size / 8) << 20);
		for (index = 0; index < (runtime->periods); index++) {
			offset = dma_size * ((index + 1) % runtime->periods);
			desc_addr[index].dwSrc_Addr = dma_mmr_init.dwSrc_Addr + offset;
			desc_addr[index].dwDes_Addr = dma_mmr_init.dwDes_Addr;
			desc_addr[index].dwLLP_Addr = dma_mmr_init.dwLLP_Addr + sizeof(struct dma_regs_t) * ((index + 1) % runtime->periods);
			desc_addr[index].dwCtrl = dma_mmr_init.dwCtrl;
		}
	}
	else {
		dma_mmr_init.dwSrc_Addr = ssp_base[dev_num] + I2S_RXDMA;
		dma_mmr_init.dwDes_Addr = runtime->dma_addr;
		dma_mmr_init.dwLLP_Addr = virt_to_bus(desc_addr);
		dma_mmr_init.dwCtrl = 0x50499 |
							(dma_req_sel[dev_num] << 12) |
							((dma_size / 8) << 20);			
		for (index = 0; index < (runtime->periods); index++) {
			offset = dma_size * ((index + 1) % runtime->periods);
			desc_addr[index].dwSrc_Addr = dma_mmr_init.dwSrc_Addr;			
			desc_addr[index].dwDes_Addr = dma_mmr_init.dwDes_Addr + offset;
			desc_addr[index].dwLLP_Addr = dma_mmr_init.dwLLP_Addr + sizeof(struct dma_regs_t) * ((index + 1) % runtime->periods);
			desc_addr[index].dwCtrl = dma_mmr_init.dwCtrl;
		}
	}
	dmac_flush_range((unsigned long)desc_addr, (unsigned long)(desc_addr + 16 * (runtime->periods) -1));
	mozart_write_dma(&dma_mmr_init, prtd->dma_data->chn);
}

static void mozart_pcm_stop_dma(struct snd_pcm_substream *substream)
{
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	int dma_chn = prtd->dma_data->chn;

	DEBUG("[eve ASOC] mozart_pcm_stop_dma.\n");

	// 1. Disable DMA
	mozart_disable_dma(dma_chn);
	
	// 2. Free LLP
	//if (prtd->dma_data->desc_addr) {
	//	kfree(prtd->dma_data->desc_addr);
	//}
}

static irqreturn_t audio_dma_isr(int irq, void *dev_id)
{
	struct snd_pcm_substream *substream = dev_id;
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	unsigned long chn_monitor;
	int index;

	chn_monitor = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);
	for (index = MOZART_I2S0; index < MOZART_I2S_NUM; index++) {
		if ((1<<(prtd->dma_data->chn)) & chn_monitor) {
			MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, ~(1<<(prtd->dma_data->chn)));
			snd_pcm_period_elapsed(substream);
			return IRQ_HANDLED;	
		}
		if ((1<<(prtd->dma_data->chn)) & chn_monitor) {
			MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, ~(1<<(prtd->dma_data->chn)));	
			snd_pcm_period_elapsed(substream);
			return IRQ_HANDLED;	
		}
	}
	return IRQ_NONE;
}

static int mozart_dma_irqrequest(struct snd_pcm_substream *substream)
{
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	struct mozart_pcm_dma_params *dma_data = prtd->dma_data;

	DEBUG("[eve ASOC] mozart_dma_irqrequest\n");		
	spin_lock(&prtd->lock);	
	if (dma_data->irq_acks == 0) {
		dma_data->irq = APBC_IRQ_NUM;
		if(request_irq (dma_data->irq, audio_dma_isr, IRQF_SHARED, "APBC DMA", substream)) 	{
			DEBUG( "irq request fail\n");
			spin_unlock(&prtd->lock);	
			return -EBUSY;
		}
		// set Level Trigger for LLP interrupts
		fLib_SetIntTrig(dma_data->irq, LEVEL, H_ACTIVE);
	}
	dma_data->irq_acks++;
	spin_unlock(&prtd->lock);	
	return 0;	
}

static void mozart_dma_irqfree(struct snd_pcm_substream *substream)
{
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mozart_pcm_dma_params *dma_data = prtd->dma_data;

	DEBUG("[eve ASOC] mozart_dma_irqfree\n");
	spin_lock(&prtd->lock);	
	if (dma_data->irq_acks > 1) {
		dma_data->irq_acks--;
	}
	else {
		free_irq(dma_data->irq, substream);
	}
	spin_unlock(&prtd->lock);	
}

static int mozart_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;

	DEBUG("[eve ASOC] pcm_ops: mozart_pcm_trigger\n");		
	spin_lock(&prtd->lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		mozart_pcm_start_dma(substream);
		mozart_i2s_enable_device(prtd->dev_num, 1);
		mozart_i2s_enable_stream(prtd->dev_num, 1, substream->stream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
		mozart_pcm_stop_dma(substream);
		mozart_i2s_enable_stream(prtd->dev_num, 0, substream->pstr->stream);		
		break;
		
	default:
		ret = -EINVAL;
		break;
	}
	spin_unlock(&prtd->lock);	
	return ret;
}

static int mozart_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct mozart_runtime_data *prtd = substream->runtime->private_data;
	struct mozart_pcm_dma_params *dma_data = prtd->dma_data;	

	DEBUG("[eve ASOC] pcm_ops: mozart_pcm_prepare\n");	
	/*dma_data->desc_addr = vpl_audio_apb_descbuf;
	if (dma_data->desc_addr == NULL) {
		DEBUG("No memory.\n");
		return -ENOMEM;
	}*/	
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {	
		dma_data->desc_addr = vpl_audio_apb_descbuf_playback;
		prtd->dma_data->chn = DMA_CHN_7;
	}
	else {
		dma_data->desc_addr = vpl_audio_apb_descbuf_capture;
		prtd->dma_data->chn = DMA_CHN_6;
	}
	return 0;
}

static snd_pcm_uframes_t mozart_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct mozart_runtime_data *prtd = runtime->private_data;
	unsigned int offset;
	dma_addr_t count;
	dma_addr_t src, dst;

	spin_lock(&prtd->lock);
	mozart_dma_getposition(prtd->dma_data->chn, &src, &dst);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		count = src - runtime->dma_addr;
	else
		count = dst - runtime->dma_addr;;

	spin_unlock(&prtd->lock);

	offset = bytes_to_frames(runtime, count);
	if (offset >= runtime->buffer_size)
		offset = 0;

	return offset;
}

static int mozart_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct mozart_runtime_data *prtd;
	struct mozart_pcm_dma_params *dma_data;
	int ret = 0;

	DEBUG("[eve ASOC] mozart_pcm_open\n");
	snd_soc_set_runtime_hwparams(substream, &mozart_pcm_hardware);

	prtd = kzalloc(sizeof(*prtd), GFP_KERNEL);
	if (prtd == NULL)
		return -ENOMEM;
	dma_data = kzalloc(sizeof(*dma_data), GFP_KERNEL);
	if (dma_data == NULL)
		return -ENOMEM;

	spin_lock_init(&prtd->lock);
	prtd->dev_num = MOZART_I2S0;
	prtd->dma_data = dma_data;
	runtime->private_data = prtd;
	mozart_dma_irqrequest(substream);
	
	return ret;	
}

static int mozart_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct mozart_runtime_data *prtd = runtime->private_data;

	DEBUG("[eve ASOC] pcm_ops: mozart_pcm_close\n");
	mozart_dma_irqfree(substream);
	kfree(prtd);
	return 0;
}

static int mozart_pcm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params)
{
	DEBUG("[eve ASOC] pcm_ops: mozart_pcm_hw_params\n");
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

static int mozart_pcm_hw_free(struct snd_pcm_substream *substream)
{
	DEBUG("[eve ASOC] pcm_ops: mozart_pcm_hw_free\n");
	return snd_pcm_lib_free_pages(substream);
}

struct snd_pcm_ops mozart_pcm_ops = {
	.open = mozart_pcm_open,
	.close = mozart_pcm_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = mozart_pcm_hw_params,
	.hw_free = mozart_pcm_hw_free,
	.prepare = mozart_pcm_prepare,
	.trigger = mozart_pcm_trigger,
	.pointer = mozart_pcm_pointer,
};

static int mozart_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = mozart_pcm_hardware.buffer_bytes_max;

	DEBUG("[eve ASOC] mozart_pcm_preallocate_dma_buffer\n");
	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_coherent(pcm->card->dev, size, &buf->addr, GFP_KERNEL);
	if (!buf->area)
		return -ENOMEM;

	buf->bytes = size;
	return 0;
}

static void mozart_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	DEBUG("[eve ASOC] pcm_free: mozart_pcm_free_dma_buffers\n");
	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;
		dma_free_coherent(pcm->card->dev, buf->bytes, buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 mozart_pcm_dmamask = 0xffffffff;

static int mozart_pcm_new(struct snd_card *card, struct snd_soc_dai *dai, struct snd_pcm *pcm)
{
	int ret = 0;
	int i;
	DEBUG("[eve ASOC] pcm_new: mozart_pcm_new\n");

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &mozart_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = 0xffffffff;

	if (dai->playback.channels_min) {
		ret = mozart_pcm_preallocate_dma_buffer(pcm, SNDRV_PCM_STREAM_PLAYBACK);
		if (ret) {
			DEBUG("mozart_pcm_new:0\n");
			return ret;
		}
	}

	if (dai->capture.channels_min) {
		ret = mozart_pcm_preallocate_dma_buffer(pcm, SNDRV_PCM_STREAM_CAPTURE);
		if (ret) {
			DEBUG("mozart_pcm_new:1\n");
			return ret;
		}
	}

	// init i2ssc
	for (i = 0; i < MOZART_I2S_NUM; i++)
		mozart_i2s_init(i);

	vpl_audio_apb_descbuf_capture = \
		(struct dma_regs_t *)kmalloc((mozart_pcm_hardware.periods_max) * sizeof(struct dma_regs_t), GFP_KERNEL) ;
	vpl_audio_apb_descbuf_playback = \
		(struct dma_regs_t *)kmalloc((mozart_pcm_hardware.periods_max) * sizeof(struct dma_regs_t), GFP_KERNEL) ;	
	
	return 0;
}

struct snd_soc_platform mozart_soc_platform = {
	.name = "mozart-audio",
	.pcm_ops = &mozart_pcm_ops,
	.pcm_new = mozart_pcm_new,
	.pcm_free = mozart_pcm_free_dma_buffers,
};
EXPORT_SYMBOL_GPL(mozart_soc_platform);

MODULE_DESCRIPTION("______ Inc. Mozart PCM DMA module");
MODULE_LICENSE("GPL");
