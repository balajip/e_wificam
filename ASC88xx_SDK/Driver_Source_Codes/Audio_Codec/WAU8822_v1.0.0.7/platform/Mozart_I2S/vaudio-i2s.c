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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/clk.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>

#ifdef DEBUG_PRINTK
#define DEBUG(args...) printk(KERN_DEBUG args)
#else
#define DEBUG(args...)
#endif


#define VAUDIO_IIS_DRIVER_VERSION		"4.0.0.1"
#define VAUDIO_IIS_COMPANY				"______ Inc."
#define VAUDIO_IIS_PLATFORM			"EVM"

const char vaudioiis_id[] = "$VaudioIis: "VAUDIO_IIS_DRIVER_VERSION" "VAUDIO_IIS_PLATFORM" "__DATE__" "VAUDIO_IIS_COMPANY" $";


// SYSC
#define SYSC_BASE               			MOZART_SYSC_MMR_BASE
#define SYSC_ID_VERSION       			0x00000000
#define SYSC_I2S_CLK_PARAM 		0x0000002C
#define SYSC_I2S_CLK_ENABLE		0x00000028
#define SYSC_I2S_ENABLE				0x00000800

static void *sysc_base;

static int mozart_i2s_startup(struct snd_pcm_substream *substream)
{
	return 0;
}
static int mozart_i2s_set_dai_fmt(struct snd_soc_dai *cpu_dai, unsigned int fmt)
{
	return 0;
}
static int mozart_i2s_set_dai_clkdiv(struct snd_soc_dai *cpu_dai, int div_id, int div)
{
	int clk_param;

	DEBUG("[eve ASOC] i2s_ops: mozart_i2s_hw_params\n");
	clk_param = readl((unsigned long)sysc_base + SYSC_I2S_CLK_PARAM);
	clk_param &= ~0xFF;
	clk_param &= ~(0xFF<<8);		
	clk_param |= div;
	clk_param |= div<<8;	
	writel(clk_param, (unsigned long)sysc_base + SYSC_I2S_CLK_PARAM);

	return 0;
}

static int mozart_i2s_set_dai_pll(struct snd_soc_dai *codec_dai, int pll_id, unsigned int freq_in, unsigned int freq_out)
{
	volatile unsigned long reg;
	void *pllc_base;

	pllc_base = ioremap(MOZART_PLLC_MMR_BASE, 0x100);		
	reg =  readl((unsigned long)sysc_base + 0x48);
	reg |=  (0x1<<25); /* PLL2 Source 0:18.432 MHz / 1:24 MHz*/
	writel(reg, (unsigned long)sysc_base + 0x48);

	/* Set PLL2 to ratio 0x0203002F. i.e 144MHz */
	writel(0x00000009, pllc_base + 0x14);   /* Enable Adjustment, Disable clock out */
	writel(0x0203002F, pllc_base + 0x18);
	writel(0x00000001, pllc_base + 0x14); /* Disable Adjustment */
	/*Wait for PLL lock*/
	while ((readl(pllc_base + 0x14) & 0x4) != 0x4) {
		printk("Unlock....\n");
	}
	/*Start to output the clock of PLL*/
	writel(0x00000003, pllc_base + 0x14); /* Enable clock output */
	iounmap(pllc_base);	

	return 0;
}

static int mozart_i2s_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{

	return 0;
}

static int mozart_i2s_trigger(struct snd_pcm_substream *substream, int cmd)
{
	return 0;
}

static int mozart_i2s_probe(struct platform_device *pdev, struct snd_soc_dai *dai)
{	
	request_mem_region(SYSC_BASE, 0x100, "SYSC");
	sysc_base = ioremap(SYSC_BASE, 0x100);

	return 0;
}

static void mozart_i2s_remove(struct platform_device *pdev, struct snd_soc_dai *dai)
{
	iounmap(sysc_base);
	release_mem_region(SYSC_BASE,0x100);
}

#define MOZART_I2S_RATES	SNDRV_PCM_RATE_8000_48000

struct snd_soc_dai mozart_i2s_dai = {
	.name = "mozart-i2s",
	.id = 0,
	.type = SND_SOC_DAI_I2S,
	.probe = mozart_i2s_probe,
	.remove = mozart_i2s_remove,
	.playback = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = MOZART_I2S_RATES,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,},
	.capture = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = MOZART_I2S_RATES,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,},
	.ops = {
		.startup = mozart_i2s_startup,
		.trigger = mozart_i2s_trigger,
		.hw_params = mozart_i2s_hw_params,},
	.dai_ops = {
		.set_fmt = mozart_i2s_set_dai_fmt,
		.set_clkdiv = mozart_i2s_set_dai_clkdiv,
		.set_pll = mozart_i2s_set_dai_pll,
	},
};
EXPORT_SYMBOL_GPL(mozart_i2s_dai);

MODULE_DESCRIPTION("______ Inc. Mozart I2S SoC Interface");
MODULE_LICENSE("GPL");
