/*
 * Mozart_Audio_IT6604
 * Driver for Mozart IT6604 Audio Codec Driver
 *
 * Copyright (C) 2010  VN Inc.
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

#include "IT6604_Audio.h"

static struct i2c_client *IT6604_audio_i2c_client;
static struct i2c_client IT6604_audio_i2c_client_template;

//#define DEBUG_DEBUG
#ifdef DEBUG_DEBUG
#define DEBUG(args...) printk(KERN_DEBUG args)
#else
#define DEBUG(args...)
#endif

//"normal_i2c" is the standard name for kernel i2c usage.
//Please do not try to modify it.
static const unsigned short normal_i2c[] = { IT6604_ADDR >> 1, I2C_CLIENT_END };
I2C_CLIENT_INSMOD;

/* IDG for this card */
static char *IT6604_audio_card_id = "Mozart_IT6604";

/* device structure */
static struct platform_device *IT6604_audio_platform_device;

/* audio stream */
struct audio_stream {
	int stream_id;		/* numeric identification */	
	unsigned int active:1;	/* we are using this stream for transfer now */
	int dma_pos;			/* dma processed position in frames */
	struct dma_regs_t * desc_addr;
	struct snd_pcm_substream *stream;
};

/* chip */
struct IT6604_audio_chip {
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

/* runtime private data for device info */
typedef struct {
	int dev_num;
	int dma_chn;
}snd_pcm_runtime_privete_data;

/* The actual rates supported by the card */
static unsigned int IT6604_audio_samplerates_array[] = {
	32000, 44100, 48000, 88200, 96000, 176400, 192000
};

static struct snd_pcm_hw_constraint_list constraints_rates = {
	.count = ARRAY_SIZE(IT6604_audio_samplerates_array), 
	.list = IT6604_audio_samplerates_array,
	.mask = 0,
};

/* MOZART VPL SSP virtual base address */
static unsigned int ssp_base[MOZART_I2S_NUM] = {
	SSP2_BASE
};

/* MOZART VPL SYSC base address */
static void *sysc_base;

/* MOZART VPL APBC REQ_SEL for SSP0 */
static int dma_req_sel[MOZART_I2S_NUM] = {0x5};

/* DMA ISR prototype */
static irqreturn_t IT6604_audio_dma_isr(int irq, void *dev_id);
const char vaudio_id[] = "$Vaudio: "VAUDIO_DRIVER_VERSION" "VAUDIO_PLATFORM" "__DATE__" $";


extern struct mutex AV_SYNC_LOCK;
int IT6604_audio_codec_write_reg(struct i2c_client *i2cdev, int reg, int val)
{
	unsigned char buf[2] ;
	int ret = 0 ;

	buf[0] = (unsigned char)reg ;
	buf[1] = (unsigned char)val ;

	DEBUG("Write reg 0x%x = 0x%x\n", reg, val) ;

	mutex_lock(&AV_SYNC_LOCK) ;

	if(i2c_master_send(i2cdev, buf, 2) != 2) {
		ret = -1 ;
	}

	mutex_unlock(&AV_SYNC_LOCK) ;

	return ret ;
}

int IT6604_audio_codec_read_reg(struct i2c_client *i2cdev, unsigned char reg)
{
	unsigned char data = 0 ;

	mutex_lock(&AV_SYNC_LOCK) ;

	i2c_master_send(i2cdev, &reg, 1);
	i2c_master_recv(i2cdev, &data, 1);

	mutex_unlock(&AV_SYNC_LOCK) ;

	return data;
}

static int IT6604_audio_volume_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	return 0;
}

static int IT6604_audio_volume_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	return 0 ;
}

static int IT6604_audio_volume_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int new_volume = value->value.integer.value[0] ;
	unsigned char reg_val = 0 ;

	DEBUG("Capture Volume(put)\n") ;
	DEBUG(" - put value = 0x%x\n", new_volume) ;

	switch(new_volume) {
		case 0 ://Mute
			//reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_HWMUTE_CTRL) ;
			//reg_val |= (1 << 3) | (1 << 6) ;
			//IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_HWMUTE_CTRL, reg_val) ;
			reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL) ;
			reg_val |= B_FORCE_AVMUTE ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL, reg_val) ;
			break ;
		case 1 :// 1x enlarge audio
			reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL) ;
			reg_val &= ~B_FORCE_AVMUTE ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL, reg_val) ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_HWAMP_CTRL, 0) ;
			break ;
		case 2 :// 2x enlarge audio
			reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL) ;
			reg_val &= ~B_FORCE_AVMUTE ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL, reg_val) ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_HWAMP_CTRL, 1) ;
			break ;
		case 3 :// 4x enlarge audio
			reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL) ;
			reg_val &= ~B_FORCE_AVMUTE ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_AUDIO_CTRL, reg_val) ;
			IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_HWAMP_CTRL, 2) ;
			break ;
		default:
			break ;
	};
	
    return 0;
}

static int IT6604_audio_samplerate_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *info)
{
	DEBUG("The sample rate of IT6604 codec cannot be configured manually.\n") ;
	DEBUG("It will be regenerated according to the N/CTS/TMDS-Clock sent by HDMI source device.\n") ;
	DEBUG("Please read the HDMI spec to get the information in advance.\n") ;
	
	return 0;
}

static int IT6604_audio_samplerate_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *value)
{
	int reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_FS) & 0xF;
	int sample_rate = 0 ;

	switch(reg_val) {
		case B_Fs_44p1KHz :
			sample_rate = 44100 ;
			break ;
		case B_Fs_48KHz :
			sample_rate = 48000 ;
			break ;
		case B_Fs_32KHz :
			sample_rate = 32000 ;
			break ;
		case B_Fs_88p2KHz :
			sample_rate = 88200 ;
			break ;
		case B_Fs_96KHz :
			sample_rate = 96000 ;
			break ;
		case B_Fs_176p4KHz :
			sample_rate = 176400 ;
			break ;
		case B_Fs_192KHz :
			sample_rate = 192000 ;
			break ;
	} ;
		
	value->value.integer.value[0] = sample_rate ;	

	return 0 ;
}

static struct snd_kcontrol_new IT6604_audio_mixer_array[] = {
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Capture Volume",
		.info = IT6604_audio_volume_info,
		.get = IT6604_audio_volume_get,
		.put = IT6604_audio_volume_put,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Sample Rate",
		.info = IT6604_audio_samplerate_info,
		.get = IT6604_audio_samplerate_get,
	}
} ;

static void IT6604_audio_set_samplerate(struct IT6604_audio_chip *chip, long rate, int device)
{
	int reg_val = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client, REG_FS) & 0xF;
	int sample_rate = 0 ;

	switch(reg_val) {
		case B_Fs_44p1KHz :
			sample_rate = 44100 ;
			break ;
		case B_Fs_48KHz :
			sample_rate = 48000 ;
			break ;
		case B_Fs_32KHz :
			sample_rate = 32000 ;
			break ;
		case B_Fs_88p2KHz :
			sample_rate = 88200 ;
			break ;
		case B_Fs_96KHz :
			sample_rate = 96000 ;
			break ;
		case B_Fs_176p4KHz :
			sample_rate = 176400 ;
			break ;
		case B_Fs_192KHz :
			sample_rate = 192000 ;
			break ;
	} ;
	
	chip->samplerate[device] = sample_rate;

	return ;
}

void IT6604_audio_dma_set_settings(struct dma_regs_t* dma_mmr, int chn)
{
	MOZART_APBC_WRITEL(APBC_DMA_SRC_ADDR(chn), dma_mmr->dwSrc_Addr);
	MOZART_APBC_WRITEL(APBC_DMA_DES_ADDR(chn), dma_mmr->dwDes_Addr);	
	MOZART_APBC_WRITEL(APBC_DMA_LLP(chn), dma_mmr->dwLLP_Addr);		
	MOZART_APBC_WRITEL(APBC_DMA_CTRL(chn), dma_mmr->dwCtrl);
}

void IT6604_audio_dma_get_settings(struct dma_regs_t* dma_mmr, int chn)
{
	dma_mmr->dwSrc_Addr = MOZART_APBC_READL(APBC_DMA_SRC_ADDR(chn));
	dma_mmr->dwDes_Addr = MOZART_APBC_READL(APBC_DMA_DES_ADDR(chn));	
	dma_mmr->dwLLP_Addr = MOZART_APBC_READL(APBC_DMA_LLP(chn));		
	dma_mmr->dwCtrl = MOZART_APBC_READL(APBC_DMA_CTRL(chn));
}

static u_int IT6604_audio_dma_get_ops(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	unsigned int offset;
	dma_addr_t addr;

	addr = MOZART_APBC_READL(APBC_DMA_DES_ADDR(dev_private->dma_chn)) ;	
	offset = addr - runtime->dma_addr;
	
	offset = bytes_to_frames(runtime, offset);
	if (offset >= runtime->buffer_size) {
		offset = 0;
	}
	return offset;
}

static void IT6604_audio_dma_start_op(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	int dev_num = dev_private->dev_num;
	unsigned int dma_size;
	unsigned int index;
	unsigned int offset;
	struct dma_regs_t *desc_addr = s->desc_addr;
	struct dma_regs_t *dma_mmr_init = NULL;

	dma_size = frames_to_bytes(runtime, runtime->period_size); // transfer one period size data each dma move
	dma_mmr_init = kmalloc(sizeof(*dma_mmr_init), GFP_ATOMIC);
	if (dma_mmr_init == NULL) {
		DEBUG("No memory.\n");
	}

	DEBUG("%s dma size = %d, dev_private->dma_chn = %d\n", __func__, dma_size, dev_private->dma_chn) ;

	/* prepare LLP */
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
	
	dmac_flush_range((void *)desc_addr, (void *)(desc_addr + 16 * (runtime->periods) -1));
	IT6604_audio_dma_set_settings(dma_mmr_init, dev_private->dma_chn);
	kfree(dma_mmr_init);
}

inline void IT6604_audio_dma_diable(int dma_chn)
{
	DWORD reg;

	reg = MOZART_APBC_READL(APBC_DMA_CTRL(dma_chn));
	reg &= ~ 0x3;
	MOZART_APBC_WRITEL(APBC_DMA_CTRL(dma_chn), reg);
	reg = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);	
	reg &= ~(1<<dma_chn);
	MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, reg);	
	
}

static void IT6604_audio_dma_stop_op(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	int dma_chn = dev_private->dma_chn;

#if 0	//IT6604: "Audio have to set as AudioOff state when the video mode is not a HDMI VideoOn mode" 
	struct dma_regs_t *llp_addr, *n_llp_addr;

	llp_addr = (struct dma_regs_t*)(bus_to_virt(MOZART_APBC_READL(APBC_DMA_LLP(dma_chn))));
	n_llp_addr = (struct dma_regs_t*)(bus_to_virt(llp_addr->dwLLP_Addr));

	// 1. Disable LLP
	llp_addr->dwLLP_Addr = 0x0;
	n_llp_addr->dwLLP_Addr = 0x0;
	flush_cache_all();

	// 2. Wait for DMA Complete
	while (MOZART_APBC_READL(APBC_DMA_LLP(dma_chn)) != 0);
	while (MOZART_APBC_READL(APBC_DMA_CTRL(dma_chn))>>20 != 0);
#endif
	// 3. Disable DMA
	IT6604_audio_dma_diable(dma_chn);
	
	// 4. Free LLP
	if (s->desc_addr) {
		kfree(s->desc_addr);
	}
}

static irqreturn_t IT6604_audio_dma_isr(int irq, void *dev_id)
{
	struct IT6604_audio_chip *chip = dev_id;
	struct audio_stream *s;
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	snd_pcm_runtime_privete_data *dev_private;
	unsigned long chn_monitor;

	//DEBUG("%s\n", __func__) ;
	
	chn_monitor = MOZART_APBC_READL(APBC_DMA_CHN_MONITOR);
	s = &chip->s[MOZART_I2S2][SNDRV_PCM_STREAM_CAPTURE];	
	if (s->active && s->stream) {
		substream = s->stream;
		runtime = substream->runtime;
		dev_private = runtime->private_data;

		if ((1<<(dev_private->dma_chn)) & chn_monitor) {
			MOZART_APBC_WRITEL(APBC_DMA_CHN_MONITOR, ~(1<<(dev_private->dma_chn)));
			snd_pcm_period_elapsed(s->stream);
			return IRQ_HANDLED;
		}
	}
	
	return IRQ_NONE;	
}

static int IT6604_audio_dma_request_irq(struct IT6604_audio_chip *chip)
{
	mutex_lock(&chip->dma_irq_mutex);
	if (chip->dma_irq < 0) {
		chip->dma_irq = APBC_IRQ_NUM;
		if(request_irq (chip->dma_irq, IT6604_audio_dma_isr, IRQF_SHARED, "APBC DMA", chip)) 	{
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

static void IT6604_audio_dma_free_irq(struct IT6604_audio_chip *chip)
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

static void IT6604_audio_enable_i2s_device(struct audio_stream *s, int enable)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	
	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IER, (enable) ? 0x1 : 0x0);
}

 static void IT6604_audio_enable_i2s_rxblock(struct audio_stream *s, int enable, int stream_id)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	
	unsigned int dev_base = ssp_base[dev_private->dev_num];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	//enable Rx Block
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IRER, (enable) ? 0x1 : 0x0);		
}

static int IT6604_audio_pcm_ops_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct IT6604_audio_chip *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	struct audio_stream *s ;
	int err = 0;

	s = &chip->s[dev_private->dev_num][substream->pstr->stream];	
	switch (cmd) {
		
	case SNDRV_PCM_TRIGGER_START:
		if ((runtime->dma_addr % 8) != 0) {
			printk(KERN_EMERG "runtime->dma_addr not aligned with APBC DMA data size!\n");
			return -EPERM;
		}
		s->active = 1;
		IT6604_audio_dma_start_op(s);
		IT6604_audio_enable_i2s_device(s, 1);
		IT6604_audio_enable_i2s_rxblock(s, 1, substream->pstr->stream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
		s->active = 0;
		IT6604_audio_dma_stop_op(s);
		IT6604_audio_enable_i2s_rxblock(s, 0, substream->pstr->stream);		
		IT6604_audio_enable_i2s_device(s, 0);
		break;
		
	default:
		err = -EINVAL;
		break;
	}
	return err;
}

static int IT6604_audio_pcm_ops_prepare(struct snd_pcm_substream *substream)
{
	struct IT6604_audio_chip *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	struct audio_stream *s = &chip->s[dev_private->dev_num][substream->pstr->stream];
	struct audio_stream *s1 = &chip->s[dev_private->dev_num][substream->pstr->stream ^ 1];

	if (s1->active != 1) {
		IT6604_audio_set_samplerate(chip, runtime->rate, dev_private->dev_num);
	}	
	else if (chip->samplerate[dev_private->dev_num] != runtime->rate) {
		printk(KERN_EMERG "[I2S driver] WARNING : The samplerate %d is not set successfully into I2S%d, because the other stream is running under %ld samplerate.\n", runtime->rate, dev_private->dev_num, chip->samplerate[dev_private->dev_num]);
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

static snd_pcm_uframes_t IT6604_audio_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct IT6604_audio_chip *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;
	struct audio_stream *s = &chip->s[dev_private->dev_num][substream->pstr->stream];
	snd_pcm_uframes_t ret;

	ret = IT6604_audio_dma_get_ops(s);
	return ret;
}

static int IT6604_audio_pcm_ops_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params)
{
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

static int IT6604_audio_pcm_ops_hw_free(struct snd_pcm_substream *substream)
{
	return snd_pcm_lib_free_pages(substream);
}

static struct snd_pcm_hardware snd_i2s_capture =
{
	.info =		 (SNDRV_PCM_INFO_INTERLEAVED |
				   SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID),
	.formats =	SNDRV_PCM_FMTBIT_S16_LE,
	.rates =		 (SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |\
				      SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |\
				      SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |\
				      SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_KNOT),
	.rate_min =		32000,
	.rate_max =		192000,
	.channels_min =		2,
	.channels_max =		2,
	.periods_min =		2,
	.periods_max =		8192,
	.buffer_bytes_max =	1024 * 1024,
	.period_bytes_min =	128,
	.period_bytes_max =	PERIOD_BYTES_MAX,
};

static void IT6604_audio_runtime_private_free(struct snd_pcm_runtime *runtime)
{
	if (runtime->private_data) 	{
		kfree(runtime->private_data);
	}
}

static int IT6604_audio_pcm_ops_open(struct snd_pcm_substream *substream)
{
	struct IT6604_audio_chip *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = NULL;
	int stream_id = substream->pstr->stream;
	int err;

	if ((err = IT6604_audio_dma_request_irq(chip)) < 0) {
		return err;
	}
	chip->s[MOZART_I2S2][stream_id].stream = substream;
	dev_private = kmalloc(sizeof(*dev_private), GFP_ATOMIC);		
	dev_private->dev_num = MOZART_I2S2;
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		DEBUG("[ERR] IT6604 only supports CAPTURE(no PLAYBACK)!\n") ;
		return -1 ;
	}
	else {
		dev_private->dma_chn = DMA_CHN_10;
		runtime->hw = snd_i2s_capture;
	}
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0)
		return err;
	if ((err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, &constraints_rates)) < 0)
		return err;

	runtime->private_data = dev_private;
	runtime->private_free = IT6604_audio_runtime_private_free;
	return 0;	
}

static int IT6604_audio_pcm_ops_close(struct snd_pcm_substream *substream)
{
	struct IT6604_audio_chip *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_runtime_privete_data *dev_private = runtime->private_data;

	IT6604_audio_dma_free_irq(chip);
	chip->s[dev_private->dev_num][substream->pstr->stream].stream = NULL;
	return 0;	
}

static struct snd_pcm_ops IT6604_audio_i2s_capture_ops = {
	.open = IT6604_audio_pcm_ops_open,
	.close = IT6604_audio_pcm_ops_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = IT6604_audio_pcm_ops_hw_params,
	.hw_free = IT6604_audio_pcm_ops_hw_free,
	.prepare = IT6604_audio_pcm_ops_prepare,
	.trigger = IT6604_audio_pcm_ops_trigger,
	.pointer = IT6604_audio_pcm_pointer,
};

static void IT6604_audio_i2s_hwinit(struct IT6604_audio_chip *chip, int device)
{
	unsigned int dev_base = ssp_base[device];
	unsigned int virt_dev_base = IO_ADDRESS(dev_base);

	DEBUG("%s\n", __func__) ;

	/* Setup DMA stuff */
	chip->s[device][SNDRV_PCM_STREAM_CAPTURE].stream_id = SNDRV_PCM_STREAM_CAPTURE;

	/* Initialize I2S */
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IER, 0x0);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IRER, 0x0);

	//This is the necessary step.
	//We have to unmask the interrupt(Tx Empty/Rx Ready) and let I2S signifies DMA to send/receive data to/from I2S.
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_IMR, 0x0);

	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFCR, 0x3);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RFF0, 0x1);
	MOZART_VPL_SSP_WRITEL(virt_dev_base, I2S_RCR, 0x2);

	//set up i2s mclk
	IT6604_audio_set_samplerate(chip, chip->samplerate[device], device);
} 
/*
 * create pcm instances
 */
static int IT6604_audio_create_pcm_instance(struct IT6604_audio_chip *chip, int dev)
{
	struct snd_pcm *pcm;
	int err;
	int playback_substream_count = 0 ;
	int capture_substream_count = 1 ;

	if((err = snd_pcm_new(chip->card, "IT6604 PCM", dev, playback_substream_count, capture_substream_count, &pcm))<0) {
		return err;
	}
	err = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV, NULL, 64*1024, 64*1024);
	if (err < 0) {
		printk(KERN_EMERG "Preallocate memory fail.\n");
	}

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &IT6604_audio_i2s_capture_ops);

	pcm->private_data = chip;
	pcm->info_flags = 0;
	strcpy(pcm->name, chip->card->shortname);

	chip->pcm[dev] = pcm;
	chip->samplerate[dev] = 48000; // DEFAULT_SAMPLERATE

	/* init audio dev */
	IT6604_audio_i2s_hwinit(chip, dev);

	return 0;
}

static inline int IT6604_audio_codec_swreset(void)
{
    IT6604_audio_codec_write_reg(IT6604_audio_i2c_client,REG_RST_CTRL, B_AUDRST) ;
    udelay(1000) ;
    IT6604_audio_codec_write_reg(IT6604_audio_i2c_client,REG_RST_CTRL, 0) ;
    
	return 0 ;
}

#define FLAG_CLEAR_INT_MODE 1
#define FLAG_CLEAR_INT_PKG 2
#define FLAG_CLEAR_INT_HDCP 4
#define FLAG_CLEAR_INT_AUDIO 8    
#define FLAG_CLEAR_INT_ALL (FLAG_CLEAR_INT_MODE|FLAG_CLEAR_INT_PKG|FLAG_CLEAR_INT_HDCP|FLAG_CLEAR_INT_AUDIO)
#define FLAG_CLEAR_INT_MASK (~FLAG_CLEAR_INT_ALL)

void 
SetIntMask3(unsigned char AndMask,unsigned char OrMask)
{
    unsigned char uc = 0 ;
    if( AndMask != 0 )
    {
        uc = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client,0x18) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    IT6604_audio_codec_write_reg(IT6604_audio_i2c_client,0x18, uc) ;
}

void
SetHWMuteCTRL(BYTE AndMask, BYTE OrMask)
{
    BYTE uc = 0 ;
    
    if( AndMask )
    {
        uc = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client,REG_HWMUTE_CTRL) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_HWMUTE_CTRL,uc) ;
    
}

void
ClearIntFlags(BYTE flag) 
{
    BYTE uc = 0 ;
    uc = IT6604_audio_codec_read_reg(IT6604_audio_i2c_client,REG_INTERRUPT_CTRL) ;
    uc &= FLAG_CLEAR_INT_MASK ;
    uc |= flag ;
    IT6604_audio_codec_write_reg(IT6604_audio_i2c_client,REG_INTERRUPT_CTRL,uc) ;
    mdelay(1);
    uc &= FLAG_CLEAR_INT_MASK ;
    IT6604_audio_codec_write_reg(IT6604_audio_i2c_client,REG_INTERRUPT_CTRL,uc) ;  // write 1, then write 0, the corresponded clear action is activated.
    mdelay(1);
}

int IT6604_audio_codec_init(void)
 {
	DEBUG("%s\n", __func__) ;

	SetHWMuteCTRL((~B_HW_AUDMUTE_CLR_MODE),(B_HW_AUDMUTE_CLR_MODE)) ;
    ClearIntFlags(B_CLR_AUDIO_INT) ;
    SetIntMask3(~(B_AUTOAUDMUTE|B_AUDFIFOERR),(B_AUTOAUDMUTE|B_AUDFIFOERR)) ;

	IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_HWMUTE_CTRL, 0x00) ;
	IT6604_audio_codec_write_reg(IT6604_audio_i2c_client, REG_I2S_CTRL, 0xa0) ;

	return 0 ;
}

static int IT6604_audio_codec_probe(struct i2c_adapter *adap, int addr, int kind)
{
	int ret;

	IT6604_audio_i2c_client_template.adapter = adap;
	IT6604_audio_i2c_client_template.addr = addr;
 	IT6604_audio_i2c_client = kmemdup(&IT6604_audio_i2c_client_template, sizeof(IT6604_audio_i2c_client_template), GFP_KERNEL);
	if (IT6604_audio_i2c_client == NULL) {
		printk(KERN_ERR "[ERR]Fail to create I2C client!\n");
		return -ENOMEM;
	}
	
	ret = i2c_attach_client(IT6604_audio_i2c_client);
	if(ret < 0) {
		DEBUG("[ERR]Fail to attach IT6604 at addr %x\n", addr);
		goto err;
	}

	ret = IT6604_audio_codec_init() ;
	if(ret < 0) {
		DEBUG("[ERR]Fail to init IT6604!\n");
		goto err;
	}
	
	return ret;

err:
	kfree(IT6604_audio_i2c_client);
	return ret;

}

static int IT6604_audio_i2c_attach(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, IT6604_audio_codec_probe);
}

static int IT6604_audio_i2c_detach(struct i2c_client *client)
{
	i2c_detach_client(client);
	kfree(client);
	return 0;
}

#define I2C_DRIVERID_IT6604 0xfefe /* need a proper id */
/* corgi i2c codec control layer */
static struct i2c_driver IT6604_audio_i2c_driver = {
	.driver = {
		.name = "IT6604-Audio-I2C-Driver",
		.owner = THIS_MODULE,
	},
	.id =             I2C_DRIVERID_IT6604,
	.attach_adapter = IT6604_audio_i2c_attach,
	.detach_client =  IT6604_audio_i2c_detach,
	.command =        NULL,
};

static struct i2c_client IT6604_audio_i2c_client_template = {
	.name =   "IT6604-I2C-Device",//Max length is 20
	.driver = &IT6604_audio_i2c_driver,
};

/*==================== Basic Flow of Drivers ========================*/
 void IT6604_audio_snd_card_private_free(struct snd_card *card)
{
	struct IT6604_audio_chip *chip = card->private_data;

	IT6604_audio_dma_free_irq(chip);
}

static int IT6604_audio_driver_probe(struct platform_device *devptr)
{
	int err;
	struct snd_card *card;
	struct IT6604_audio_chip *chip;
	unsigned long i, ret;
	struct snd_kcontrol_new *kctl;

	/* kernel i2c interface 2009.09.29 */
	ret = i2c_add_driver(&IT6604_audio_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "[ERR]Failed to add I2C!\n") ;
		return -EIO ;
	}	

	/* register the soundcard */
	card = snd_card_new(-1, IT6604_audio_card_id, THIS_MODULE, sizeof(struct IT6604_audio_chip));
	if (card == NULL) {
		printk(KERN_ERR "[ERR]Failed to allocate memory for card structure!\n") ;
		return -ENOMEM;
	}

	card->dev = &devptr->dev;
	strncpy(card->driver, devptr->dev.driver->name, sizeof(card->driver));
	
	chip = card->private_data;

 	card->private_free = IT6604_audio_snd_card_private_free;
	chip->card = card;

	chip->dma_irq = -1;
	mutex_init(&chip->dma_irq_mutex);
	
	strcpy(card->driver, "I2S");
	sprintf(card->shortname, "IT6604-Audio-I2S-Driver");
	sprintf(card->longname, "IT6604-Audio-I2S-Driver");

	snd_card_set_dev(card, &devptr->dev);
	
	// PCM
	if ((err = IT6604_audio_create_pcm_instance(chip, MOZART_I2S2)) < 0) {
		printk(KERN_ERR "[ERR]Failed to add I2S!\n") ;
		goto nodev;
	}

	/* Mixer controls */	
	strcpy(card->mixername, "IT6604-Audio-Mixer");
	for(i = 0 ; i < ARRAY_SIZE(IT6604_audio_mixer_array) ; i++) {
		kctl = &IT6604_audio_mixer_array[i] ;
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

static int __devexit IT6604_audio_driver_remove(struct platform_device *devptr)
{
	int ret;

	ret = snd_card_free(platform_get_drvdata(devptr));
	if (ret != 0) {
		printk(KERN_EMERG "snd_card_free returns %d\n", ret);
	}
	platform_set_drvdata(devptr, NULL);	
	i2c_del_driver(&IT6604_audio_i2c_driver);
	return 0;
}

#define IT6604_AUDIO_DRIVER_NAME	"IT6604-Audio-Driver"
static struct platform_driver IT6604_audio_platform_driver = {
	.probe		= IT6604_audio_driver_probe,
	.remove		= __devexit_p(IT6604_audio_driver_remove),
	.driver		= {
		.name	= IT6604_AUDIO_DRIVER_NAME,
	},
};

static int __init IT6604_audio_module_init(void)
{
	int err;

	printk(KERN_INFO "[%s]\n", __func__) ;

	request_mem_region(SYSC_BASE, 0x100, "SYSC");
	sysc_base = ioremap(SYSC_BASE, 0x100);

	if ((err = platform_driver_register(&IT6604_audio_platform_driver)) < 0) {
		return err;
	}
	IT6604_audio_platform_device = platform_device_register_simple(IT6604_AUDIO_DRIVER_NAME, -1, NULL, 0);
	if (!IS_ERR(IT6604_audio_platform_device)) {
		if (platform_get_drvdata(IT6604_audio_platform_device)) {
			return 0;
		}
		platform_device_unregister(IT6604_audio_platform_device);
		err = -ENODEV;
	}
	else {
		err = PTR_ERR(IT6604_audio_platform_device);
	}
	platform_driver_unregister(&IT6604_audio_platform_driver);
	return err;
}

static void IT6604_audio_module_exit(void)
{
	printk(KERN_INFO "[%s]\n", __func__) ;

	iounmap(sysc_base);
	release_mem_region(SYSC_BASE,0x100);

	platform_device_unregister(IT6604_audio_platform_device);
	platform_driver_unregister(&IT6604_audio_platform_driver);
}

MODULE_DESCRIPTION("IT6604 Audio Driver");
MODULE_LICENSE("GPL");

module_init(IT6604_audio_module_init)
module_exit(IT6604_audio_module_exit)
