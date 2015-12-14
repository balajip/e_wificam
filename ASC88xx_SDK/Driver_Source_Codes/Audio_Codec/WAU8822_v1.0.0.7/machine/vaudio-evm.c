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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/dma.h>
#include <mach/hardware.h>

#include "../platform/vaudio.h"
#include "../platform/Mozart_I2S/vaudio-i2s.h"
#include "../codec/CodecParameter.h"

#ifdef DEBUG_PRINTK
#define DEBUG(args...) printk(KERN_DEBUG args)
#else
#define DEBUG(args...)
#endif

#define VAUDIO_EVM_DRIVER_VERSION		"4.0.0.1"
#define VAUDIO_EVM_COMPANY				"______ Inc."
#define VAUDIO_EVM_PLATFORM				"EVM"

const char vaudioevm_id[] = "$VaudioEvm: "VAUDIO_EVM_DRIVER_VERSION" "VAUDIO_EVM_PLATFORM" "__DATE__" "VAUDIO_EVM_COMPANY" $";

static int i2sClkSrc = 0;
module_param(i2sClkSrc, int, 0644);
MODULE_PARM_DESC (i2sClkSrc, "I2S clock source 0: 18.432, 1: 24MHz");

static int evm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;	
	unsigned int bclk_div = 0, mclk_div = 0, sysclk_div = 0;
	int ret = 0;

	DEBUG("[eve ASOC] evm_ops: evm_hw_params.\n");

	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
					 SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		DEBUG("set fmt to codec dai failed.\n");
		return ret;
	}

	//MCLK source selection: internal PLL oscillator output (WAU8822 R6[8])
	ret = snd_soc_dai_set_clkdiv(codec_dai, vpl_codec_param_mclksel_reg, (i2sClkSrc) ? 0x100 : 0x0);		
	if (ret < 0) {
		DEBUG("set MCLKSEL to codec dai failed.\n");
		return ret;
	}

	switch (params_rate(params)) {
	case 48000:
		sysclk_div = (i2sClkSrc) ? 4 : 22;
		mclk_div = (i2sClkSrc) ? 0x40 : 0x00;
		break;
	case 44100:
		sysclk_div = (i2sClkSrc) ? 4 : 24;		
		mclk_div = (i2sClkSrc) ? 0x40 : 0x00;
		break;
	case 32000:
		sysclk_div = (i2sClkSrc) ? 4 : 22;		
		mclk_div = (i2sClkSrc) ? 0x60 : 0x20;
		break;
	case 16000:
		sysclk_div = (i2sClkSrc) ? 4 : 22;		
		mclk_div = (i2sClkSrc) ? 0xA0 : 0x60;
		break;
	case 8000:
		sysclk_div = (i2sClkSrc) ? 4 : 22;		
		mclk_div = (i2sClkSrc) ? 0xE0 : 0xA0;
		break;
	default:
		pr_warning("vaudio-evm: Unsupported sample rate %d\n", params_rate(params));
		return -EINVAL;
	}
	//MCLK divisor (WAU8822 R6[7,6,5])
	ret = snd_soc_dai_set_clkdiv(codec_dai, vpl_codec_param_mclkdiv_reg, mclk_div);
	if (ret < 0) {
		DEBUG("set MCLKDIV to codec dai failed.\n");
		return ret;
	}

	ret = snd_soc_dai_set_clkdiv(cpu_dai, 0, sysclk_div);
	if (ret < 0) {
		DEBUG("set MCLKDIV to codec dai failed.\n");
		return ret;
	}

	if (i2sClkSrc == 1) { // 24MHz
		//PLL setting : PLL2 outputs 144MHz, 144/3=48MHz
		ret = snd_soc_dai_set_pll(cpu_dai, 0, 0, 0);
		if (ret < 0) {
			DEBUG("set PLL to codec dai failed.\n");
		}
		//PLL setting (WAU8822 R36~R39): freq_in is 12MHz, expected out is 11.2896MHz for 44100 sampling-rate and 12.288MHz for others
		ret = snd_soc_dai_set_pll(codec_dai, 0, 12000000, (params_rate(params)==44100) ? 11289600 : 12288000);
		if (ret < 0) {
			DEBUG("set PLL to codec dai failed.\n");
		}
	}

	bclk_div = vpl_codec_param_bclkdiv_val;	
	ret = snd_soc_dai_set_clkdiv(codec_dai, vpl_codec_param_bclkdiv_reg, bclk_div);
	if (ret < 0) {
		DEBUG("set BCLKDIV to codec dai failed.\n");
		return ret;
	}


	return 0;
}

static struct snd_soc_ops evm_ops = {
	.hw_params = evm_hw_params,
};

static int evm_codec_init(struct snd_soc_codec *codec)
{
	//DEBUG("[eve ASOC] evm_wau8822_init.\n");
	return 0;
}

static struct snd_soc_dai_link evm_dai = {
	.name = vpl_codec_param_name,
	.stream_name = vpl_codec_param_name,
	.cpu_dai = &mozart_i2s_dai,
	.codec_dai = &vpl_codec_param_codec_dai,
	.init = evm_codec_init,
	.ops = &evm_ops,
};

static struct snd_soc_machine snd_soc_machine_evm = {
	.name = "VPL EVM",
	.dai_link = &evm_dai,
	.num_links = 1,
};

/* evm audio private data */
static struct vpl_codec_param_setup_data evm_codec_setup = {
	.i2c_address = 0x1a,
};

/* evm audio subsystem */
static struct snd_soc_device evm_snd_devdata = {
	.machine = &snd_soc_machine_evm,
	.platform = &mozart_soc_platform,
	.codec_dev = &vpl_codec_param_codec_dev,
	.codec_data = &evm_codec_setup,
};

static struct platform_device *evm_snd_device;

static int __init evm_init(void)
{
	int ret;

	DEBUG("[eve ASOC] evm_init.\n");
	evm_snd_device = platform_device_alloc("soc-audio", 0);
	if (!evm_snd_device)
		return -ENOMEM;

	platform_set_drvdata(evm_snd_device, &evm_snd_devdata);
	evm_snd_devdata.dev = &evm_snd_device->dev;
	//evm_snd_device->dev.platform_data = &evm_snd_data;

	ret = platform_device_add(evm_snd_device);
	if (ret)
		platform_device_put(evm_snd_device);

	return ret;
}

static void __exit evm_exit(void)
{
	DEBUG("[eve ASOC] evm_exit.\n");
	platform_device_unregister(evm_snd_device);
}

module_init(evm_init);
module_exit(evm_exit);

MODULE_DESCRIPTION("______ Inc. Mozart EVM ASoC driver");
MODULE_LICENSE("GPL");

