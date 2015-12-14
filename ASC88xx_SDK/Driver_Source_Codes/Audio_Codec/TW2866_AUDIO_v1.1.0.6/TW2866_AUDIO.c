/*
 * Copyright (C) 2007  ______ Inc.
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
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/control.h>

#include <asm/cacheflush.h>

#include "TW2866.h"

//#define DEBUG_PRINTK
#ifdef DEBUG_PRINTK
#define DEBUG(args...) printk(KERN_DEBUG args)
#else
#define DEBUG(args...)
#endif

static int CodecNum = 2 ;

const char vaudio_id[] = "$Vaudio: "TW2866_AUDIO_DRIVER_VERSION" "TW2866_AUDIO_PLATFORM" "__DATE__" "TW2866_AUDIO_COMPANY" $";

/* kernel i2c interface 2009.09.29 */
static struct i2c_client *i2c;
static struct i2c_client *vpl_codec_client;

#define TW2866_1ST_ADDR 0x50
#define TW2866_2ND_ADDR 0x52
#define MUX_1ST_ADDR 0x6A
#define MUX_2ND_ADDR 0x6E

static const unsigned short normal_i2c_2codec[] = { 
		TW2866_1ST_ADDR	>> 1, 
		TW2866_2ND_ADDR >> 1, 
		MUX_1ST_ADDR >> 1, 
		MUX_2ND_ADDR >> 1,
		I2C_CLIENT_END 
};

static const unsigned short normal_i2c_1codec[] = { 
		TW2866_2ND_ADDR	>> 1, 
		MUX_1ST_ADDR >> 1, 
		I2C_CLIENT_END 
};

static unsigned short normal_i2c[] = { 
		I2C_CLIENT_END , 
		I2C_CLIENT_END , 
		I2C_CLIENT_END , 
		I2C_CLIENT_END ,
		I2C_CLIENT_END 
};

/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/*==========================================================*/

/* device structure */
static struct platform_device *vpl_audio_device;

static int vpl_audio_codec_active[2] = {0} ;
static int vpl_audio_codec_capture_ch_num[2] = {0} ;
static int TW2866_audio_capture_8bits[2] = {0} ; // 1 : yes, 0 : no
static int TW2866_audio_playback_8bits = 0 ;// 1 : yes, 0 :no

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
	SSP1_BASE,
	SSP2_BASE,
	SSP0_BASE,
};

//.........................................................................................................................
/* MOZART VPL APBC REQ_SEL for SSP0 */
static int dma_req_sel[MOZART_I2S_NUM] = {0x4, 0x5, 0x3};

//.........................................................................................................................
/* DMA ISR prototype */
static irqreturn_t audio_dma_isr(int irq, void *dev_id);

//.........................................................................................................................

//extern spinlock_t AV_SYNC_LOCK;
extern struct mutex AV_SYNC_LOCK;
void vpl_audio_write_i2cdev_reg(struct i2c_client *i2cdev, int reg, int val)
{
	unsigned char buf[2] ;

	mutex_lock(&AV_SYNC_LOCK) ;

	DEBUG( "vpl_audio_write_i2cdev_reg(i2cdev->addr = 0x%lx)\n", i2cdev->addr) ;
	
	buf[0] = (unsigned char)reg ;
	buf[1] = (unsigned char)val ;

	i2c_master_send(i2cdev, &buf, 2) ;
	switch(i2cdev->addr) {
		case (TW2866_1ST_ADDR >> 1) :
			tw2866_1st_reg_cache[reg] = val ;
			break ;
		case (TW2866_2ND_ADDR >> 1) :
			tw2866_2nd_reg_cache[reg] = val ;
			break ;		
	} ;

	mutex_unlock(&AV_SYNC_LOCK) ;

	return ;
}

int vpl_audio_read_i2cdev_reg(struct i2c_client *i2cdev, int reg)
{
	u8 data = 0 ;
	
	switch(i2cdev->addr) {
		case (TW2866_1ST_ADDR >> 1) :
			data = tw2866_1st_reg_cache[reg] ;
			break ;
		case (TW2866_2ND_ADDR >> 1) :
			data = tw2866_2nd_reg_cache[reg] ;
			break ;		
	} ;

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
 * set sample rate and format
 */
static void snd_i2s_set_samplerate(struct snd_i2s *chip, long rate, int device)
{

	if (rate > 0) {
		//mozart_vvdc_config(rate, device);
		udelay(1000);
		chip->samplerate[device] = rate;
	}
}

/*==========================================================*/
/*
 * enable/disable audio device
 */
static int audio_device_is_enalbed(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	
	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);
	
	return (int)(MOZART_VPL_SSP_READL(virt_dev_base, I2S_IER));
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
 * enable/disable audio clock
 */
 static void audio_enable_clk(struct audio_stream *s, int enable)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	
	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_CER, (enable) ? 0x1 : 0x0);
}

/*==========================================================*/
/*
 * clear TX/RX FIFO
 */
static void audio_clear_FIFO(struct audio_stream *s, int stream_id)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = runtime->private_data;
	
	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TFF0, 0x1);
	}
	else {
		MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFF0, 0x1);	
	}
}

/*==========================================================*/
/*
 * setup LLP and drive DMA to process audio data
 */
#define ENABLE_DMA_CHANNEL             (1 << 0)
#define DISABLE_DMA_CHANNEL            (0 << 0)
#define ENABLE_DMA_COMPLETE_INT_FLAG   (1 << 2)
#define DISABLE_DMA_COMPLETE_INT_FLAG  (0 << 2)
#define ENABLE_DMA_LL_UPDATE_INT       (1 << 3)
#define DISABLE_DMA_LL_UPDATE_INT      (0 << 3)
#define BURST_LENGTH_1                 (0 << 4)
#define BURST_LENGTH_4                 (1 << 4)
#define BURST_LENGTH_8                 (2 << 4)
#define BURST_LENGTH_16                (3 << 4)
#define TRANSFER_TYPE_AHB2AHB          (0 << 6)
#define TRANSFER_TYPE_AHB2APB          (1 << 6)
#define TRANSFER_TYPE_APB2AHB          (2 << 6)
#define TRANSFER_TYPE_APB2APB          (3 << 6)
#define SRC_ADDR_NO_INC                (0 << 8)
#define SRC_ADDR_POSITIVE_INC          (1 << 8)
#define SRC_ADDR_NEGATIVE_INC          (2 << 8)
#define DES_ADDR_NO_INC                (0 << 10)
#define DES_ADDR_POSITIVE_INC          (1 << 10)
#define DES_ADDR_NEGATIVE_INC          (2 << 10)
#define REQ_TYPE                       (1 << 16)
#define BYTE_SWAP_DISABLE              (0 << 17)
#define BYTE_SWAP_ENABLE               (1 << 17)
#define DATA_WIDTH_8BITS               (0 << 18)
#define DATA_WIDTH_16BITS              (1 << 18)
#define DATA_WIDTH_32BITS              (2 << 18)
#define DATA_WIDTH_64BITS              (3 << 18) 
// Here I use a func for user to set the ctrl of APBC-DMA
// This is because there are many bits in ctrl, 
// to avoid missing some setting bits, 
// we enforce the user to pass the necessary info to a func.
static unsigned long apbc_dma_ctrl_set(int enable, 
                                       int int_type, 
                                       int burst_length, 
                                       int transfer_type, 
                                       int src_addr_inc, 
                                       int des_addr_inc, 
                                       int req_sel, 
                                       int swap_enable, 
                                       int data_width, 
                                       int cycle_num)
{
    unsigned long dma_ctrl = 0 ;

    dma_ctrl |= REQ_TYPE ;//We must set this bit.
    dma_ctrl |= enable |
                int_type |
                burst_length |
                transfer_type |
                src_addr_inc |
                des_addr_inc |
                req_sel |
                swap_enable |
                data_width |
                cycle_num ;
    
    return dma_ctrl ;
}

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

	int capture_channel_num = runtime->channels ;
	//if (s->stream_id == SNDRV_PCM_STREAM_CAPTURE) {
	//	printk("dev(%d), cap num = %d\n", substream->pcm->device, capture_channel_num) ;
	//}

	dma_size = frames_to_bytes(runtime, runtime->period_size); // transfer one period size data each dma move
	//printk("[jam]dma_size = %d\n", dma_size) ;
	dma_mmr_init = kmalloc(sizeof(*dma_mmr_init), GFP_ATOMIC);
	if (dma_mmr_init == NULL) {
		printk("No memory.\n");
	}
	
	/* prepare LLP */
	if (s->stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		dma_mmr_init->dwSrc_Addr = runtime->dma_addr;
		dma_mmr_init->dwDes_Addr = ssp_base[dev_num] + I2S_TXDMA;
		dma_mmr_init->dwLLP_Addr = virt_to_bus(desc_addr);
		dma_mmr_init->dwCtrl = apbc_dma_ctrl_set(ENABLE_DMA_CHANNEL,
                                                ENABLE_DMA_LL_UPDATE_INT,
                                                BURST_LENGTH_4,
                                                TRANSFER_TYPE_AHB2APB,
                                                SRC_ADDR_POSITIVE_INC,
                                                DES_ADDR_NO_INC,
                                                (dma_req_sel[dev_num] << 12),
                                                BYTE_SWAP_DISABLE,
                                                (TW2866_audio_playback_8bits) ? DATA_WIDTH_8BITS : DATA_WIDTH_16BITS,
                      						    (TW2866_audio_playback_8bits) ? ((dma_size / 4) << 20) : ((dma_size / 8) << 20)
                      						   );
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
		if(TW2866_audio_capture_8bits[dev_num]) {
			dma_mmr_init->dwCtrl = apbc_dma_ctrl_set(ENABLE_DMA_CHANNEL,
    	            							    ENABLE_DMA_LL_UPDATE_INT,
        	        							    BURST_LENGTH_4,
            	    							    TRANSFER_TYPE_APB2AHB,
                								    SRC_ADDR_NO_INC,
                								    DES_ADDR_POSITIVE_INC,
                								    (dma_req_sel[dev_num] << 12),
                								    BYTE_SWAP_DISABLE,
                                	                DATA_WIDTH_16BITS,
                                    	            ((dma_size / 8) << 20)
                                        	       );
		}
		else {
			dma_mmr_init->dwCtrl = apbc_dma_ctrl_set(ENABLE_DMA_CHANNEL,
    	            							    ENABLE_DMA_LL_UPDATE_INT,
        	        							    BURST_LENGTH_4,
            	    							    TRANSFER_TYPE_APB2AHB,
                								    SRC_ADDR_NO_INC,
                								    DES_ADDR_POSITIVE_INC,
                								    (dma_req_sel[dev_num] << 12),
                								    BYTE_SWAP_DISABLE,
                                	                (capture_channel_num == 4)? DATA_WIDTH_32BITS : DATA_WIDTH_16BITS,
                                    	            (capture_channel_num == 4)? ((dma_size / 16) << 20) : ((dma_size / 8) << 20)
                                        	       );
		}
		for (index = 0; index < (runtime->periods); index++) {
			offset = dma_size * ((index + 1) % runtime->periods);
			desc_addr[index].dwSrc_Addr = dma_mmr_init->dwSrc_Addr;			
			desc_addr[index].dwDes_Addr = dma_mmr_init->dwDes_Addr + offset;
			desc_addr[index].dwLLP_Addr = dma_mmr_init->dwLLP_Addr + sizeof(struct dma_regs_t) * ((index + 1) % runtime->periods);
			desc_addr[index].dwCtrl = dma_mmr_init->dwCtrl;
		}
	}
	dmac_flush_range((unsigned long)desc_addr, (unsigned long)(desc_addr + 16 * (runtime->periods) -1));
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
	for (index = 0; index < MOZART_I2S_NUM; index++) {
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

	DEBUG("snd_i2s_audio_init\n") ;

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
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x5);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_TCR, 0x0);	

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

	//printk("snd_i2s_trigger()\n") ;

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

	//printk("snd_i2s_pcm_prepare()\n") ;

	if (s1->active != 1) {
		snd_i2s_set_samplerate(chip, runtime->rate, dev_private->dev_num);
	}	
	else if (chip->samplerate[dev_private->dev_num] != runtime->rate) {
		printk(KERN_ERR "[I2S driver] WARNING : The samplerate %d can not set into I2S%d successfully,\
			because the other stream is running under %ld samplerate.\n", 
			runtime->rate, dev_private->dev_num, chip->samplerate[dev_private->dev_num]);
		return -EINVAL;
	}
 	s->dma_pos = bytes_to_frames(runtime, runtime->dma_addr);
	
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
int vpl_audio_codec_get_cur_samplerate(struct i2c_client *client)
{
	int val_f0 = vpl_audio_read_i2cdev_reg(client, 0xF0) ;
	int val_f1 = vpl_audio_read_i2cdev_reg(client, 0xF1) ;
	int val_f2 = vpl_audio_read_i2cdev_reg(client, 0xF2) ;
	int val_f3 = vpl_audio_read_i2cdev_reg(client, 0xF3) ;
	int val_f4 = vpl_audio_read_i2cdev_reg(client, 0xF4) ;
	int val_f5 = vpl_audio_read_i2cdev_reg(client, 0xF5) ;

	int cur_samplerate = -1 ;
	int index ;

	for(index = 0 ; index < 5 ; index++) {
		if((tw2866_sample_rate_table_50MHz[index][0] == val_f0) &&
		   (tw2866_sample_rate_table_50MHz[index][1] == val_f1) &&
		   (tw2866_sample_rate_table_50MHz[index][2] == val_f2) &&
		   (tw2866_sample_rate_table_50MHz[index][3] == val_f3) &&
		   (tw2866_sample_rate_table_50MHz[index][4] == val_f4) &&
		   (tw2866_sample_rate_table_50MHz[index][5] == val_f5)
		  ) {
		  break ;
		}
		if((tw2866_sample_rate_table_60MHz[index][0] == val_f0) &&
		   (tw2866_sample_rate_table_60MHz[index][1] == val_f1) &&
		   (tw2866_sample_rate_table_60MHz[index][2] == val_f2) &&
		   (tw2866_sample_rate_table_60MHz[index][3] == val_f3) &&
		   (tw2866_sample_rate_table_60MHz[index][4] == val_f4) &&
		   (tw2866_sample_rate_table_60MHz[index][5] == val_f5)
		  ) {
		  break ;
		}
	}

	switch(index) {
		case SAMPLE_RATE_TABLE_48KHZ :
			cur_samplerate = 48000 ;
			break ;
		case SAMPLE_RATE_TABLE_44KHZ :
			cur_samplerate = 44100 ;
			break ;
		case SAMPLE_RATE_TABLE_32KHZ :
			cur_samplerate = 32000 ;
			break ;
		case SAMPLE_RATE_TABLE_16KHZ :
			cur_samplerate = 16000 ;
			break ;
		case SAMPLE_RATE_TABLE_08KHZ :
			cur_samplerate = 8000 ;
			break ;
	}

	return cur_samplerate ;	
}

#define VIDEO_STD_MASK (0x7 << 4)
#define VIDEO_STD_NTSC (0x0 << 4)
#define VIDEO_STD_PAL  (0x1 << 4)
static int snd_i2s_pcm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params)
{
	int i, video_index, samplerate_index = -1 ;
	int val ;
	int tmp ;

	unsigned int format ;
	unsigned int channels ;
	unsigned int samplerate ;
	
	unsigned int device_num = substream->pcm->device ;
	struct i2c_client *hw_params_client = &vpl_codec_client[device_num] ;
	int active_number = vpl_audio_codec_active[substream->pcm->device] ;
	int cur_sample_rate ;

	unsigned int dev_base, virt_dev_base ;

	struct snd_pcm_runtime *rt = substream->runtime ;

	DEBUG("snd_i2s_pcm_hw_params\n") ;

	/* ===== set format ===== */
	format = params_format(hw_params) ;
	DEBUG("format = %d\n", format) ;
	switch(format) {
		case SNDRV_PCM_FORMAT_S16_LE :	
			DEBUG("[TW2866]SNDRV_PCM_FORMAT_S16_LE\n") ;
			if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
				//format
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_AINCTL) ;
				tmp &= ~0x3 ;
				tmp |= 0x0 ;
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_AINCTL, tmp) ;
				//bit width
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_ACLKPOL) ;
				tmp &= ~(1 << 6) ;
				tmp |= (0x0 << 6) ;
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_ACLKPOL, tmp) ;

				TW2866_audio_playback_8bits = 0 ;
			}
			else {
				//format
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_MIX_MUTE) ;
				tmp &= ~(0x3 << 6) ;
				tmp |= (0x0 << 6) ;
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_MIX_MUTE, tmp) ;
				//bit width
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_AMASTER) ;
				tmp &= ~(0x1 << 2) ;
				tmp |= (0x0 << 2);
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_AMASTER, tmp) ;
				//set bit width for DMA reference
				TW2866_audio_capture_8bits[device_num] = 0 ;
			}			
			break ;
			
		case SNDRV_PCM_FORMAT_MU_LAW :
			DEBUG("[TW2866]SNDRV_PCM_FORMAT_MU_LAW\n") ;
			if(substream->stream == SNDRV_PCM_STREAM_CAPTURE) {				
				//format
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_MIX_MUTE) ;
				tmp &= ~(0x3 << 6) ;
				tmp |= (0x2 << 6) ;
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_MIX_MUTE, tmp) ;
				//bit width
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_AMASTER) ;
				tmp &= ~(0x1 << 2) ;
				tmp |= (0x1 << 2);
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_AMASTER, tmp) ;
				//set bit width for DMA reference
				TW2866_audio_capture_8bits[device_num] = 1 ;
			}
			else {
				printk(KERN_DEBUG "[ERR] Our PLAYBACK only supports \"Signed 16 Bits Little Endian\"!!\n") ;
				printk(KERN_DEBUG "      (This format val = %d)\n", format) ;		
				return -1 ;
			}			
			break ;
			
		case SNDRV_PCM_FORMAT_A_LAW :
			DEBUG("[TW2866]SNDRV_PCM_FORMAT_A_LAW\n") ;
			if(substream->stream == SNDRV_PCM_STREAM_CAPTURE) {				
				//format
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_MIX_MUTE) ;
				tmp &= ~(0x3 << 6) ;
				tmp |= (0x3 << 6) ;
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_MIX_MUTE, tmp) ;				
				//bit width
				tmp = vpl_audio_read_i2cdev_reg(hw_params_client, TW2866_REG_AMASTER) ;
				tmp &= ~(0x1 << 2) ;
				tmp |= (0x1 << 2);
				vpl_audio_write_i2cdev_reg(hw_params_client, TW2866_REG_AMASTER, tmp) ;				
				//set bit width for DMA reference
				TW2866_audio_capture_8bits[device_num] = 1 ;
			}
			else {
				printk(KERN_DEBUG "[ERR] Our PLAYBACK only supports \"Signed 16 Bits Little Endian\"!!\n") ;
				printk(KERN_DEBUG "      (This format val = %d)\n", format) ;		
				return -1 ;
			}			
			break ;
			
		default :
			if(substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
				printk(KERN_DEBUG "[ERR] Our CAPTURE only supports \"Signed 16 Bits Little Endian\",\n") ;
				printk(KERN_DEBUG "      \"u-Law\" ,and \"a-Law\"!! (This format val = %d)\n", format) ;		
			}
			else {
				printk(KERN_DEBUG "[ERR] Our PLAYBACK only supports \"Signed 16 Bits Little Endian\"!!\n") ;
				printk(KERN_DEBUG "      (This format val = %d)\n", format) ;		
			}
			return -1 ;			
	}

	/* ===== set channel ===== */
	channels = params_channels(hw_params) ;
	if((channels != 2) && (channels != 4)) {
		printk(KERN_DEBUG "[ERR] Our codec only supports 2 or 4 channels!!\n") ;		
		return -1 ;
	}
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		if(channels != 2) {
			printk(KERN_DEBUG "[ERR] Our platform does not support this channel number for PLAYBACK!!\n") ;		 
			return -1 ;
		}
		else
			rt->channels = channels ;
	}
	else {
		rt->channels = channels ;
		if(TW2866_audio_capture_8bits[device_num]) {
			DEBUG("[jam] We only support 4 channels for 8bits format!\n") ;
			rt->channels = channels = 4 ;
		}
				
		val = vpl_audio_read_i2cdev_reg(hw_params_client, 0xD2) ;
		val &= ~(0x3) ;
		switch(channels) {
			case 2:
				val |= 0 ;
				break ;
			case 4:
				val |= 1 ;
				break ;
		}
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xD2, val);

		dev_base = ssp_base[device_num];
		virt_dev_base = IO_ADDRESS(dev_base);
		
		if(channels == 2) {		
			//Because the capture hw line connection will be different between 1 chips and 2 chips, 
			//and the order of channels will be different.
			//SW should also do some modifications.
			if(CodecNum == 2) {
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD3, 0xF0);
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD7, 0xF1);
			}
			else {
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD3, 0xF1);
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD7, 0xF0);
			}
			
			MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x2);
			vpl_audio_codec_capture_ch_num[device_num] = channels ;
		}
		else if (channels == 4) {
			//Because the capture hw line connection will be different between 1 chips and 2 chips, 
			//and the order of channels will be different.
			//SW should also do some modifications.
			if(CodecNum == 2) {
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD3, 0x01);
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD7, 0x23);
			}
			else {
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD3, 0x23);
				vpl_audio_write_i2cdev_reg(hw_params_client, 0xD7, 0x01);
			}

			if(TW2866_audio_capture_8bits[device_num]) {
				MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x2);
			}
			else {
				MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x5);
			}
			vpl_audio_codec_capture_ch_num[device_num] = channels ;			
		}
	}

	/* ===== set sample rate ===== */	
	samplerate = params_rate(hw_params) ;
	cur_sample_rate = vpl_audio_codec_get_cur_samplerate(hw_params_client) ;
	if((cur_sample_rate != samplerate) && (active_number != 1)) {
		
		printk( KERN_DEBUG "[ERR] Sorry, there is already one stream running on this codec!!!\n") ;
		printk( KERN_DEBUG "      - Solution (1). You can follow that stream's sample rate(%d) to set up this stream\n", cur_sample_rate) ;
		printk( KERN_DEBUG "      - Solution (2). You can stop other stream or wait until it ends, then you can set any sample rate you want.\n") ;		
		return -1 ;
	}
	// - Because TW2866-video would support auto-detect NTSC/PAL, 
	// - we should set our audio sample rate based on the video settings.
	// - (You can reference TW2866 document).
	//step 1. Get the first index of table by the sample rate	
	switch(samplerate) {
		case 48000 :
			samplerate_index = SAMPLE_RATE_TABLE_48KHZ ;
			break ;
		case 44100 :
			samplerate_index = SAMPLE_RATE_TABLE_44KHZ ;
			break ;
		case 32000 :
			samplerate_index = SAMPLE_RATE_TABLE_32KHZ ;
			break ;
		case 16000 :
			samplerate_index = SAMPLE_RATE_TABLE_16KHZ ;
			break ;
		case 8000 :
			samplerate_index = SAMPLE_RATE_TABLE_08KHZ ;
			break ;		
		default:
			DEBUG("[ERR] We do not support this sample rate!\n") ;
			DEBUG("      Select \"44.1KHz\" as the default sample rate.\n") ;
			samplerate_index = SAMPLE_RATE_TABLE_44KHZ ;
			break ;
	}
	
	//step 2. VideoDetect ?
	video_index = -1 ;
	for(i = 0 ; i < 4 ; i++) { 
		int reg = 0x10 * i;
		int vdloss = vpl_audio_read_i2cdev_reg(hw_params_client, reg) ;

		//printk("reg[0x%02lx] = 0x%02lx\n", reg, vdloss) ;
		vdloss &= 0x80 ;
				
		if(!vdloss) {
			video_index = i ;//one video input has signal
			//printk("video present!\n") ;
			break ;
		}
	}

	//Step 3. if video detect, set the rate according to the video standard
	if(video_index != -1) {
		//get the video format from that video register
		int reg = 0x0E + (0x10*video_index) ;
		int video_std = vpl_audio_read_i2cdev_reg(hw_params_client, reg) & VIDEO_STD_MASK ;
		//printk("video_std = 0x%x\n", video_std) ;

        //set the 
		if(video_std == VIDEO_STD_PAL) {//50MHz		
			//printk("Video Detects PAL!\n") ;
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF0, tw2866_sample_rate_table_50MHz[samplerate_index][0]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF1, tw2866_sample_rate_table_50MHz[samplerate_index][1]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF2, tw2866_sample_rate_table_50MHz[samplerate_index][2]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF3, tw2866_sample_rate_table_50MHz[samplerate_index][3]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF4, tw2866_sample_rate_table_50MHz[samplerate_index][4]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF5, tw2866_sample_rate_table_50MHz[samplerate_index][5]);
		}
		else if(video_std == VIDEO_STD_NTSC) {//60MHz
			//printk("Video Detects NTSC!\n") ;
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF0, tw2866_sample_rate_table_60MHz[samplerate_index][0]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF1, tw2866_sample_rate_table_60MHz[samplerate_index][1]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF2, tw2866_sample_rate_table_60MHz[samplerate_index][2]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF3, tw2866_sample_rate_table_60MHz[samplerate_index][3]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF4, tw2866_sample_rate_table_60MHz[samplerate_index][4]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF5, tw2866_sample_rate_table_60MHz[samplerate_index][5]);			
		}
		else {//default use 60MHz
			DEBUG("[ERR] We detect some video input having signal, but not PAL or NTSC!\n") ;
			DEBUG("      Select NTSC(60MHz) settings!\n") ;
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF0, tw2866_sample_rate_table_60MHz[samplerate_index][0]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF1, tw2866_sample_rate_table_60MHz[samplerate_index][1]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF2, tw2866_sample_rate_table_60MHz[samplerate_index][2]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF3, tw2866_sample_rate_table_60MHz[samplerate_index][3]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF4, tw2866_sample_rate_table_60MHz[samplerate_index][4]);
			vpl_audio_write_i2cdev_reg(hw_params_client, 0xF5, tw2866_sample_rate_table_60MHz[samplerate_index][5]);			
		}
	}
	else {
		DEBUG("[WARN] We cannot detect any video input having signal.\n") ;
		DEBUG("       Select NTSC(60MHz) settings!\n") ;
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xF0, tw2866_sample_rate_table_60MHz[samplerate_index][0]);
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xF1, tw2866_sample_rate_table_60MHz[samplerate_index][1]);
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xF2, tw2866_sample_rate_table_60MHz[samplerate_index][2]);
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xF3, tw2866_sample_rate_table_60MHz[samplerate_index][3]);
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xF4, tw2866_sample_rate_table_60MHz[samplerate_index][4]);
		vpl_audio_write_i2cdev_reg(hw_params_client, 0xF5, tw2866_sample_rate_table_60MHz[samplerate_index][5]);
	}
	
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
	.info =		 (SNDRV_PCM_INFO_INTERLEAVED),
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
	.formats =	SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_MU_LAW | SNDRV_PCM_FMTBIT_A_LAW,
	.rates =		 (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |\
				   SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |\
				   SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_KNOT),
	.rate_min =		8000,
	.rate_max =		48000,
	.channels_min =		2,
	.channels_max =		4,
	.periods_min =		4,
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

	DEBUG("snd_i2s0_open(%d)\n", substream->number) ;
	/*if(vpl_audio_codec_active[substream->pcm->device] != 0) {
	}*/

	if ((err = audio_dma_irq_request(chip)) < 0) {
		return err;
	}
	chip->s[MOZART_I2S1][stream_id].stream = substream;
	dev_private = kmalloc(sizeof(*dev_private), GFP_ATOMIC);		
	dev_private->dev_num = MOZART_I2S1;
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		dev_private->dma_chn = DMA_CHN_7;
		runtime->hw = snd_i2s_playback;		
	}
	else {
		dev_private->dma_chn = DMA_CHN_8;
		runtime->hw = snd_i2s_capture;
	}
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0)
		return err;
	if ((err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, &constraints_rates)) < 0)
		return err;

	runtime->private_data = dev_private;
	runtime->private_free = snd_i2s_free_private_data;

	vpl_audio_codec_active[substream->pcm->device] += 1 ;
		
	return 0;	
}

static int snd_i2s1_open(struct snd_pcm_substream *substream)
{
	struct snd_i2s *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_i2s_dev_private *dev_private = NULL;
	int stream_id = substream->pstr->stream;
	int err;

	DEBUG("snd_i2s1_open(%d)\n", substream->number) ;

	if ((err = audio_dma_irq_request(chip)) < 0) {
		return err;
	}

	dev_private = kmalloc(sizeof(*dev_private), GFP_ATOMIC);			
	
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		chip->s[MOZART_I2S0][stream_id].stream = substream;
		dev_private->dev_num = MOZART_I2S0 ;
		dev_private->dma_chn = DMA_CHN_7;
		runtime->hw = snd_i2s_playback;		

		/*
		//james test
		struct i2c_client *client = &vpl_codec_client[substream->pcm->device] ;
		printk("EVERY TIME..\n") ;
		for(i = 0 ; i < TW2866_CACHEREGNUM ; i++) {
			int val = vpl_audio_read_i2cdev_reg(client, i) ;
			printk("[0x%02lx] = 0x%02lx", i, val) ;		
			if(val != tw2866_firsttime_regs[i] ) {
				printk(" ==/== ori(0x%02lx)", tw2866_firsttime_regs[i]) ;
			}
			printk("\n") ;
		}*/
	}
	else {
		chip->s[MOZART_I2S2][stream_id].stream = substream;
		dev_private->dev_num = MOZART_I2S2;
		dev_private->dma_chn = DMA_CHN_10;
		runtime->hw = snd_i2s_capture;
	}
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0)
		return err;
	if ((err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, &constraints_rates)) < 0)
		return err;

	runtime->private_data = dev_private;
	runtime->private_free = snd_i2s_free_private_data;

	vpl_audio_codec_active[substream->pcm->device] += 1 ;
	
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

	//printk("snd_i2s_close()\n") ;

	audio_dma_irq_free(chip);
	chip->s[dev_private->dev_num][substream->pstr->stream].stream = NULL;

	vpl_audio_codec_active[substream->pcm->device] -= 1 ;
	
	return 0;	
}

//[JAM] We do the "de-interleave" in this func.
static int vpl_audio_capture_copy(struct snd_pcm_substream *ss, int channel,
			     snd_pcm_uframes_t src, void __user *dst,
			     snd_pcm_uframes_t count)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	int dma_buf_index, user_buf_index ;
	int totalchannelnum = runtime->channels ;
	int device_num = ss->pcm->device ;
	int byte_num_of_sample = (TW2866_audio_capture_8bits[device_num]) ? 1 : 2 ;

	//printk("channel = %d\n", totalchannelnum) ;

	char *hwbuf = runtime->dma_area + frames_to_bytes(runtime, src);
	
	user_buf_index = 0 ;
	dma_buf_index = channel * byte_num_of_sample ;

	while(dma_buf_index < frames_to_bytes(runtime, count)) {
		//printk("u[0x%lx] <- h[0x%lx]\n", (unsigned long)(dst + user_buf_index), (unsigned long)(hwbuf + dma_buf_index)) ;
		if (copy_to_user(dst + user_buf_index , hwbuf + dma_buf_index, byte_num_of_sample))
			return -EFAULT;
	
		user_buf_index += byte_num_of_sample ;
		dma_buf_index += totalchannelnum * byte_num_of_sample ;
	}

	return 0;
}

/*==========================================================*/
/*
 *  pcm pos
 */
 /* for I2S0 */
static struct snd_pcm_ops snd_i2s0_capture_ops = {
	.open = snd_i2s0_open,
	.close = snd_i2s_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_i2s_pcm_hw_params,
	.hw_free = snd_i2s_pcm_hw_free,
	.prepare = snd_i2s_pcm_prepare,
	.trigger = snd_i2s_trigger,
	.pointer = snd_i2s_pointer,
	.copy = vpl_audio_capture_copy,
};
static struct snd_pcm_ops snd_i2s1_capture_ops = {
	.open = snd_i2s1_open,
	.close = snd_i2s_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_i2s_pcm_hw_params,
	.hw_free = snd_i2s_pcm_hw_free,
	.prepare = snd_i2s_pcm_prepare,
	.trigger = snd_i2s_trigger,
	.pointer = snd_i2s_pointer,
	.copy = vpl_audio_capture_copy,
};

static struct snd_pcm_ops snd_i2s1_playback_ops = {
	.open = snd_i2s1_open,
	.close = snd_i2s_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_i2s_pcm_hw_params,
	.hw_free = snd_i2s_pcm_hw_free,
	.prepare = snd_i2s_pcm_prepare,
	.trigger = snd_i2s_trigger,
	.pointer = snd_i2s_pointer,
};

static int vpl_audio_capture_volume_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	return 0;
}

static int vpl_audio_capture_volume_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	struct i2c_client *capture_client ;
	int device_num = value->value.integer.value[0] ;
	int channel_num = value->value.integer.value[1] ; 
	int ori_volume = -1 ;

	DEBUG("Capture Volume(get)\n") ;
	DEBUG(" - dev = 0x%x\n", device_num) ;
	if((device_num < 0) || (device_num >= CodecNum)) {
		printk(KERN_DEBUG "[ERR] Only Codec Dev#0 ~ #%d!!\n", CodecNum-1) ;
		return -1 ;
	}
	
	if(vpl_audio_codec_capture_ch_num[device_num] == 0) {
		printk(KERN_DEBUG "[ERR] You have to setup the capture channel number of this codec at first!\n") ;
		return -1 ;
	}
	
	DEBUG(" - ch = 0x%x\n", channel_num) ;
	if((channel_num < 0) || (channel_num >= 4)) {
		printk(KERN_DEBUG "[ERR] Only Channel#0 ~ #3!!\n") ;
		return -1 ;
	}

	if(CodecNum == 1) {
		//Because HW connection is different again...
		switch(channel_num) {
			case 0 :
				channel_num = 2 ;
				break ;
			case 1 :
				channel_num = 3 ;				
				break ;
			case 2 :
				channel_num = 0 ;				
				break ;
			case 3 :
				channel_num = 1 ;				
				break ;
		}
	}

	//get volume
	capture_client = &vpl_codec_client[device_num] ;
	switch(channel_num) {
		case 0 :
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD0) ;
			ori_volume &= (0x0F) ;
			break ;
		case 1 :
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD0) ;
			ori_volume &= (0xF0) ;
			ori_volume = ori_volume >> 4 ;
			break ;
		case 2 :
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD1) ;
			ori_volume &= (0x0F) ;
			break ;
		case 3 :	
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD1) ;
			ori_volume &= (0xF0) ;
			ori_volume = ori_volume >> 4 ;
			break ;
	}

	value->value.integer.value[0] = ori_volume ;
	
	return 0 ;
}

static int vpl_audio_capture_volume_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	struct i2c_client *capture_client ;
	int device_num = value->value.integer.value[0] ;
	int channel_num = value->value.integer.value[1] ; 	
	int new_volume = value->value.integer.value[2] ; 
	int ori_volume ;	

	DEBUG("Capture Volume(put)\n") ;
		
	DEBUG(" - dev = 0x%x\n", device_num) ;
	if((device_num < 0) || (device_num >= CodecNum)) {
		printk(KERN_DEBUG "[ERR] Only Codec Dev#0 ~ #%d!!\n", CodecNum-1) ;
		return -1 ;
	}

	if(vpl_audio_codec_capture_ch_num[device_num] == 0) {
		printk(KERN_DEBUG "[ERR] You have to setup the capture channel number of this codec at first!\n") ;
		return -1 ;
	}
	
	DEBUG(" - ch = 0x%x\n", channel_num) ;
	if((channel_num < 0) || (channel_num >= vpl_audio_codec_capture_ch_num[device_num])) {
		printk(KERN_DEBUG "[ERR] Only Channel #0 ~ #%d!!\n", vpl_audio_codec_capture_ch_num[device_num]) ;
		return -1 ;
	}
	
	DEBUG(" - vol = 0x%x\n", new_volume) ;	
	if((new_volume < 0) || (new_volume >= 16)) {
		printk(KERN_DEBUG "[ERR] Only Volume Level #0 ~ #15!!\n") ;
		return -1 ;
	}

	if(CodecNum == 1) {
		//Because HW connection is different again...
		switch(channel_num) {
			case 0 :
				channel_num = 2 ;
				break ;
			case 1 :
				channel_num = 3 ;				
				break ;
			case 2 :
				channel_num = 0 ;				
				break ;
			case 3 :
				channel_num = 1 ;				
				break ;
		}
	}

	//set volume
	capture_client = &vpl_codec_client[device_num] ;
	switch(channel_num) {
		case 0 :
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD0) ;
			ori_volume &= ~(0x0F) ;
			new_volume = ori_volume | (new_volume & 0x0F) ;
			vpl_audio_write_i2cdev_reg(capture_client, 0xD0, new_volume);
			break ;
		case 1 :
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD0) ;
			ori_volume &= ~(0xF0) ;
			new_volume = ori_volume | ((new_volume << 4) & 0xF0) ;
			vpl_audio_write_i2cdev_reg(capture_client, 0xD0, new_volume);
			break ;
		case 2 :
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD1) ;
			ori_volume &= ~(0x0F) ;
			new_volume = ori_volume | (new_volume & 0x0F) ;
			vpl_audio_write_i2cdev_reg(capture_client, 0xD1, new_volume);
			break ;
		case 3 :	
			ori_volume = vpl_audio_read_i2cdev_reg(capture_client, 0xD1) ;
			ori_volume &= ~(0xF0) ;
			new_volume = ori_volume | ((new_volume << 4) & 0xF0) ;
			vpl_audio_write_i2cdev_reg(capture_client, 0xD1, new_volume);
			break ;
	}
	
    return 0;
}

static int vpl_audio_playback_volume_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	return 0;
}

static int vpl_audio_playback_volume_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	struct i2c_client *playback_client  ;

	DEBUG("Playback Volume(get)\n") ;

	if(CodecNum == 1) {
		playback_client = &vpl_codec_client[0] ;
	}
	else {
		playback_client = &vpl_codec_client[1] ;
	}

	value->value.integer.value[0] = vpl_audio_read_i2cdev_reg(playback_client, 0xDF) ; 
			
	return 0 ;
}

static int vpl_audio_playback_volume_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int new_volume = value->value.integer.value[0] ;
	struct i2c_client *playback_client  ;
		
	DEBUG("Playback Volume(put)\n") ;
	DEBUG(" - put value = 0x%x\n", new_volume) ;
	if((new_volume < 0) || (new_volume >= 16)) {
		printk(KERN_DEBUG "[ERR] Only #0 ~ #15!!\n") ;
		return -1 ;
	}

	if(CodecNum == 1) {
		playback_client = &vpl_codec_client[0] ;
	}
	else {
		playback_client = &vpl_codec_client[1] ;
	}

	//set volume
	vpl_audio_write_i2cdev_reg(playback_client, 0xDF, new_volume);
	
    return 0;
}


#define MUX_DEVICE_NUM 2
#define MUX_OUTPUT_NUM 4
static int vpl_audio_playback_mux[MUX_DEVICE_NUM] = {0} ;

static int vpl_audio_playback_select_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	return 0;
}

static int vpl_audio_playback_select_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int mux_dev_index ;//each mux has most 4 outputs
	int mux_out_index ;
	int original_mux_val ;
	int max_playback_selection = CodecNum * 4 ;

	int playback_output = value->value.integer.value[0] ;

	DEBUG("Playback Mux Select\n") ;
	
	if((playback_output < 0) || (playback_output >= max_playback_selection)) {
		printk(KERN_DEBUG "[ERR] Only #0 ~ #%d!!\n", max_playback_selection -1) ;
		return -1 ;
	}
	
	mux_dev_index = playback_output / MUX_OUTPUT_NUM ;//each mux has most 4 outputs
	mux_out_index = playback_output % MUX_OUTPUT_NUM ;

	original_mux_val = vpl_audio_playback_mux[mux_dev_index] ;
	value->value.integer.value[0] = ((original_mux_val & (1 << mux_out_index)) != 0) ;
	
	return 0 ;
}

static int vpl_audio_playback_select_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int mux_dev_index ;
	int mux_out_index ;
	int original_mux_val ;
	int new_mux_val ;
	int max_playback_selection = CodecNum * 4 ;

	int playback_output = value->value.integer.value[0] ;
	int output_connect = value->value.integer.value[1] ;
	
	DEBUG( "Playback Mux Select\n") ;
	
	if((playback_output < 0) || (playback_output >= max_playback_selection)) {
		printk(KERN_DEBUG "[ERR] Only #0 ~ #%d!!\n", max_playback_selection) ;
		return -1 ;
	}
	
	mux_dev_index = playback_output / MUX_OUTPUT_NUM ;//each mux has most 4 outputs
	mux_out_index = playback_output % MUX_OUTPUT_NUM ;

	original_mux_val = vpl_audio_playback_mux[mux_dev_index] ;

	if(output_connect == 1) {
		new_mux_val = original_mux_val | (1 << mux_out_index) ;
	}
	else {
		new_mux_val = original_mux_val & ~(1 << mux_out_index) ;
	}
	
	vpl_audio_playback_mux[mux_dev_index] = new_mux_val ;

	vpl_audio_write_i2cdev_reg(&vpl_codec_client[mux_dev_index + CodecNum], new_mux_val, new_mux_val);
	
    return 0;
}

static struct snd_kcontrol_new vpl_audio_mixer_array[] = {
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
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Playback Mux Select",
		.info = vpl_audio_playback_select_info,
		.get = vpl_audio_playback_select_get,
		.put = vpl_audio_playback_select_put,
	}
} ;
 
/*==========================================================*/
/*
 * create pcm instances
 */
static int snd_i2s_pcm(struct snd_i2s *chip, int dev)
{
	struct snd_pcm *pcm;
	int err;

	if(dev == 0) {
		if(CodecNum == 1) {
			if((err = snd_pcm_new(chip->card, "tw2866#0", dev, 1, 1, &pcm))<0) {
				return err;
			}
		}
		else {
			if((err = snd_pcm_new(chip->card, "tw2866#0", dev, 0, 1, &pcm))<0) {
				return err;
			}
		}
	}
	else {
		if((err = snd_pcm_new(chip->card, "tw2866#1", dev, 1, 1, &pcm))<0) {
			return err;
		}
	}
	err = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV, NULL, 256*1024, 256*1024);
	if (err < 0) {
		printk(KERN_EMERG "Preallocate memory fail.\n");
	}

	if(CodecNum == 1) {
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_i2s1_playback_ops);
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_i2s1_capture_ops);
	}
	else {
		switch (dev) {
			case 0:	//0x50		
				snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_i2s0_capture_ops);			
				break;
			case 1://0x52
				snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_i2s1_playback_ops);
				snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_i2s1_capture_ops);
				break;
	 		default:
				break;
		}
	}
	
	pcm->private_data = chip;
	pcm->info_flags = 0;
	strcpy(pcm->name, chip->card->shortname);

	chip->pcm[dev] = pcm;
	chip->samplerate[dev] = 8000; // DEFAULT_SAMPLERATE

	/* init audio dev */
	if(CodecNum == 1) {
		snd_i2s_audio_init(chip, MOZART_I2S2);
		snd_i2s_audio_init(chip, MOZART_I2S0);
	}
	else {
		switch (dev) {
			case 0:	//0x50	
				snd_i2s_audio_init(chip, MOZART_I2S1);
				break ;
			case 1:
				snd_i2s_audio_init(chip, MOZART_I2S2);
				snd_i2s_audio_init(chip, MOZART_I2S0);
				break;
		}
	}

	return 0;
}

/*======================= I2C related ===========================*/
static int i2c_counter = 0 ;
static int tw2866_codec_probe(struct i2c_adapter *adap, int addr, int kind)
{
	int ret;

	DEBUG("tw2866_codec_probe(%x)\n", addr) ;

    //If we want to use only one codec chip, we will use 0x50 as its I2C addresss.
    //Then we should skip I2C address 0x52.
	/*if((vpl_audio_codec_chip_num == 1) && (i2c_counter == 1))
	   return -1 ;*/		

	vpl_codec_client[i2c_counter].adapter = adap;
	vpl_codec_client[i2c_counter].addr = addr;
 	
	ret = i2c_attach_client(&vpl_codec_client[i2c_counter]);
	if(ret < 0) {
		printk(KERN_DEBUG "TW2866 failed to attach at addr %x\n", addr);
		i2c_counter++ ;
		goto err;
	}

	if(i2c_counter < CodecNum) {
		int i ;		
	
		//printk("write all audio regs..\n") ;
		for(i = 0 ; i < ARRAY_SIZE(tw2866_audio_regs_index) ; i++) {
			int reg = tw2866_audio_regs_index[i] ;
			int val = tw2866_all_regs[reg] ;
			vpl_audio_write_i2cdev_reg(&vpl_codec_client[i2c_counter], reg, val);
			//printk("write[0x%x] = 0x%02x\n", reg, val) ;
		}
		/*printk("read all audio regs..\n") ;
		for(i = 0 ; i < TW2866_CACHEREGNUM ; i++) {
			tw2866_firsttime_regs[i] = vpl_audio_read_i2cdev_reg(&vpl_codec_client[i2c_counter], i) ;
			//printk("read[0x%x] = 0x%02x\n", reg, val) ;
		}*/
	}
	/*else {
		printk("turn on the mux..\n") ;
		int reg = 0xff ;
		int val = 0xff ;
		vpl_audio_write_i2cdev_reg(&vpl_codec_client[i2c_counter], reg, val);
	}*/

	i2c_counter++ ;
	
	return ret;

err:
	kfree(i2c);
	return ret;

}

static int tw2866_i2c_detach(struct i2c_client *client)
{
	DEBUG("[i2c-detach] client->name = %s\n", client->name) ;
	i2c_detach_client(client);
	//kfree(client);
	return 0;
}

static int tw2866_i2c_attach(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, tw2866_codec_probe);
}

#define I2C_DRIVERID_TW2866 0xfefe /* need a proper id */
/* corgi i2c codec control layer */
static struct i2c_driver tw2866_i2c_driver = {
	.driver = {
		.name = "TW2866 I2C Codec",
		.owner = THIS_MODULE,
	},
	.id =             I2C_DRIVERID_TW2866,
	.attach_adapter = tw2866_i2c_attach,
	.detach_client =  tw2866_i2c_detach,
	.command =        NULL,
};

static struct i2c_client *vpl_codec_client ;
static struct i2c_client vpl_codec_client_2chips[] = {
		{
		    .name =   "TW2866#0X50",
    		.driver = &tw2866_i2c_driver,
		},
		{
		    .name =   "TW2866#0X52",
    		.driver = &tw2866_i2c_driver,
		},
		{
		    .name =   "MUX#0X6A",
    		.driver = &tw2866_i2c_driver,
		},
		{
		    .name =   "MUX#0X6E",
    		.driver = &tw2866_i2c_driver,
		}
};

static struct i2c_client vpl_codec_client_1chips[] = {
		{
		    .name =   "TW2866#0X50",
    		.driver = &tw2866_i2c_driver,
		},
		{
		    .name =   "MUX#0X6A",
    		.driver = &tw2866_i2c_driver,
		},
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
	//initialize normal_i2c
	if(CodecNum == 1) {
		for(i = 0; i < ARRAY_SIZE(normal_i2c_1codec) ; i++) {
			normal_i2c[i] = normal_i2c_1codec[i] ;
		}
		vpl_codec_client = vpl_codec_client_1chips ;
	}
	else {
		for(i = 0; i < ARRAY_SIZE(normal_i2c_2codec) ; i++) {
			normal_i2c[i] = normal_i2c_2codec[i] ;
		}
		vpl_codec_client = vpl_codec_client_2chips ;		
	}
	
	ret = i2c_add_driver(&tw2866_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "can't add i2c driver");
	}

	/* register the soundcard */
	card = snd_card_new(-1, "VPL_AUDIO", THIS_MODULE, sizeof(struct snd_i2s));//now the card->private_data = kmalloc(sizeof(struct snd_i2s)) ;
	if (card == NULL)
		return -ENOMEM;

	card->dev = &devptr->dev;
	strncpy(card->driver, devptr->dev.driver->name, sizeof(card->driver));
	
	chip = card->private_data;

 	card->private_free = snd_i2s_free;
	chip->card = card;

	chip->dma_irq = -1;
	mutex_init(&chip->dma_irq_mutex);
	
	strcpy(card->driver, "VPL AUDIO");
	sprintf(card->shortname, "VPL Audio TW2866 Driver");
	sprintf(card->longname, "VPL Audio Codec Driver, TW2866.");

	snd_card_set_dev(card, &devptr->dev);
	
	// PCM
	//for (i = MOZART_I2S1; i < MOZART_I2S_NUM; i++) 	{
	for (i = 0; i < CodecNum; i++) 	{
		if ((err = snd_i2s_pcm(chip, i)) < 0)
			goto nodev;
	}

	/* Mixer controls */	
	strcpy(card->mixername, "VAudio-CTRL");
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
	i2c_del_driver(&tw2866_i2c_driver);
	return 0;
}

//.........................................................................................................................
#define VPL_AUDIO_DRIVER	"VPL AUDIO"

static struct platform_driver vpl_audio_driver = {
	.probe		= snd_i2s_probe,
	.remove		= __devexit_p(snd_i2s_remove),
	.driver		= {
		.name	= VPL_AUDIO_DRIVER,
	},
};

//.........................................................................................................................
static int __init vpl_audio_module_init(void)
{
	int err;

	DEBUG("[VPL AUDIO]vpl_audio_module_init\n") ;
	DEBUG("       - CodecNum = %d\n", CodecNum) ;
	if((CodecNum != 1) && (CodecNum != 2)) {
		printk(KERN_DEBUG "[ERR] CodecNum should be 1 or 2!!!\n") ;
		return -1 ;
	}	

	if ((err = platform_driver_register(&vpl_audio_driver)) < 0) {
		return err;
	}
	
	vpl_audio_device = platform_device_register_simple(VPL_AUDIO_DRIVER, -1, NULL, 0);
	if (!IS_ERR(vpl_audio_device)) {
		if (platform_get_drvdata(vpl_audio_device)) {
			return 0;
		}
		platform_device_unregister(vpl_audio_device);
		err = -ENODEV;
	}
	else {
		err = PTR_ERR(vpl_audio_device);
	}
	
	platform_driver_unregister(&vpl_audio_driver);
	
	return err;
}
EXPORT_SYMBOL(vpl_codec_client);
//.........................................................................................................................
static void vpl_audio_module_exit(void)
{
	platform_device_unregister(vpl_audio_device);
	platform_driver_unregister(&vpl_audio_driver);
}

//.........................................................................................................................
module_init(vpl_audio_module_init)
module_exit(vpl_audio_module_exit)

module_param(CodecNum, int, 0);

MODULE_DESCRIPTION("______ Inc. TW2866 Audio Codec Driver");
MODULE_LICENSE("GPL");

