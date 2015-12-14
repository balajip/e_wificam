/*
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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <asm/gpio.h>
#include <mach/spi-gpio.h>
#include <mach/hardware.h>

#define SPI_GPIO_OUTPUT 1
#define SPI_GPIO_INPUT	0
#ifdef DEBUG
#define SPIPRINTK	printk
#else
#define SPIPRINTK(x...)
#endif
static int spi_clk = 14;
static int spi_miso = 15;
static int spi_mosi = 17;
static int spi_chip_sel = 16;
static int write_only = 0;
/*
struct mozart_spigpio_info {
	unsigned long		 pin_clk;
	unsigned long		 pin_mosi;
	unsigned long		 pin_miso;
	unsigned long		 pin_chip_sel;

	int			 num_chipselect;
	int			 bus_num;

	void (*chip_select)(struct mozart_spigpio_info *spi, int cs);
};
*/
struct mozart_spi_info {
	unsigned long		 pin_cs;	/* simple gpio cs */
	unsigned int		 num_cs;	/* total chipselects */
	int			 bus_num;       /* bus number to use. */

	void (*gpio_setup)(struct mozart_spi_info *spi, int enable);
	void (*set_cs)(struct mozart_spi_info *spi, int cs, int pol);
};


struct mozart_spigpio {
	struct spi_bitbang		 bitbang;

	struct mozart_spigpio_info	*info;
	struct platform_device		*dev;
};

static inline struct mozart_spigpio *spidev_to_sg(struct spi_device *spi)
{
	SPIPRINTK("%s\n", __func__);
	return spi_master_get_devdata(spi->master);
}

static inline void setsck(struct spi_device *dev, int on)
{
	struct mozart_spigpio *sg = spidev_to_sg(dev);
	SPIPRINTK("%s pin = %d on = %d\n", __func__, sg->info->pin_clk, on);
	gpio_set_value(sg->info->pin_clk, on ? 1 : 0);
}

static inline void setmosi(struct spi_device *dev, int on)
{
	SPIPRINTK("%s\n", __func__);
	struct mozart_spigpio *sg = spidev_to_sg(dev);
	gpio_set_value(sg->info->pin_mosi, on ? 1 : 0);
}

static inline u32 getmiso(struct spi_device *dev)
{
	SPIPRINTK("%s\n", __func__);
	struct mozart_spigpio *sg = spidev_to_sg(dev);
	if (sg->info->write_only)
		return 0;
	else
		return gpio_get_value(sg->info->pin_miso) ? 1 : 0;
}

#define spidelay(x) ndelay(x)

#define	EXPAND_BITBANG_TXRX
#include <linux/spi/spi_bitbang.h>


static u32 mozart_spigpio_txrx_mode0(struct spi_device *spi,
				      unsigned nsecs, u32 word, u8 bits)
{
	SPIPRINTK("%s\n", __func__);
	return bitbang_txrx_be_cpha0(spi, nsecs, 0, word, bits);
}

static u32 mozart_spigpio_txrx_mode1(struct spi_device *spi,
				      unsigned nsecs, u32 word, u8 bits)
{
	SPIPRINTK("%s\n", __func__);
	return bitbang_txrx_be_cpha1(spi, nsecs, 0, word, bits);
}

static u32 mozart_spigpio_txrx_mode2(struct spi_device *spi,
				      unsigned nsecs, u32 word, u8 bits)
{
	SPIPRINTK("%s\n", __func__);
	return bitbang_txrx_be_cpha0(spi, nsecs, 1, word, bits);
}

static u32 mozart_spigpio_txrx_mode3(struct spi_device *spi,
				      unsigned nsecs, u32 word, u8 bits)
{
	SPIPRINTK("%s\n", __func__);
	return bitbang_txrx_be_cpha1(spi, nsecs, 1, word, bits);
}


static void mozart_spigpio_chipselect(struct spi_device *dev, int value)
{
	SPIPRINTK("%s\n", __func__);
	struct mozart_spigpio *sg = spidev_to_sg(dev);

	if (sg->info && sg->info->chip_select)
		(sg->info->chip_select)(sg->info, value);
}
static void spi_gpio_cs(struct mozart_spigpio_info *spi, int cs)
{
	SPIPRINTK("%s\n", __func__);
    switch (cs) {
    case BITBANG_CS_ACTIVE:
        gpio_set_value(spi->pin_chip_sel, 0);// gpio set pin_cs low
        break;
    case BITBANG_CS_INACTIVE:
        gpio_set_value(spi->pin_chip_sel, 1);// gpio set pin_cs high
        break;
    }
}
//struct mozart_spigpio_info spi_info;
static int mozart_spigpio_probe(struct platform_device *dev)
{
	struct mozart_spigpio_info *info;
	struct spi_master	*master;
	struct mozart_spigpio  *sp;
	int ret;
	master = spi_alloc_master(&dev->dev, sizeof(struct mozart_spigpio));
	if (master == NULL) {
		dev_err(&dev->dev, "failed to allocate spi master\n");
		ret = -ENOMEM;
		goto err;
	}
//	dev_set_drvdata(&master->dev, &spi_info);
	sp = spi_master_get_devdata(master);
	platform_set_drvdata(dev, sp);
	sp->info = dev->dev.platform_data;
//	sp->info = &spi_info;
	sp->info->pin_clk = spi_clk;
	sp->info->pin_miso = spi_miso;
	sp->info->pin_mosi = spi_mosi;
	sp->info->pin_chip_sel = spi_chip_sel;
	sp->info->bus_num = 0x0;// depend on arch/arm/mach-mozart/arch.c
	sp->info->write_only = write_only;
	info = sp->info;
	ret = gpio_request(sp->info->pin_clk, "scl");
	if (ret)
		goto err_request_scl;
	if (write_only == 0)
	{
		ret = gpio_request(sp->info->pin_miso, "miso");
		if (ret)
			goto err_request_miso;
	}
	ret = gpio_request(sp->info->pin_mosi, "mosi");
	if (ret)
		goto err_request_mosi;
	ret = gpio_request(sp->info->pin_chip_sel, "cs");
	if (ret)
		goto err_request_cs;
	sp->info->num_chipselect = 1;
	sp->info->chip_select = &spi_gpio_cs;

	/* setup spi bitbang adaptor */
	sp->bitbang.master = spi_master_get(master);
	sp->bitbang.master->bus_num = info->bus_num;
	sp->bitbang.master->num_chipselect = info->num_chipselect;
	sp->bitbang.chipselect = mozart_spigpio_chipselect;

	sp->bitbang.txrx_word[SPI_MODE_0] = mozart_spigpio_txrx_mode0;
	sp->bitbang.txrx_word[SPI_MODE_1] = mozart_spigpio_txrx_mode1;
	sp->bitbang.txrx_word[SPI_MODE_2] = mozart_spigpio_txrx_mode2;
	sp->bitbang.txrx_word[SPI_MODE_3] = mozart_spigpio_txrx_mode3;

	/* set state of spi pins, always assume that the clock is
	 * available, but do check the MOSI and MISO. */
	gpio_set_direction(info->pin_clk, SPI_GPIO_OUTPUT);
	gpio_set_value(info->pin_clk, 1);

	gpio_set_direction(info->pin_mosi, SPI_GPIO_OUTPUT);
	gpio_set_value(info->pin_mosi, 1);

	if (write_only == 0)
		gpio_set_direction(info->pin_miso, SPI_GPIO_INPUT);

	ret = spi_bitbang_start(&sp->bitbang);
	if (ret)
		goto err_no_bitbang;

	return 0;

 err_no_bitbang:
	spi_master_put(sp->bitbang.master);
 	gpio_free(sp->info->pin_chip_sel);
 err_request_cs:
	gpio_free(sp->info->pin_mosi);
 err_request_mosi:
	if (write_only == 0)
		gpio_free(sp->info->pin_miso);
 err_request_miso:
	gpio_free(sp->info->pin_clk);
 err_request_scl:
	kfree(master);
 err:
	return ret;

}

static int mozart_spigpio_remove(struct platform_device *dev)
{
	struct mozart_spigpio *sp = platform_get_drvdata(dev);

	spi_bitbang_stop(&sp->bitbang);
	spi_master_put(sp->bitbang.master);
 	gpio_free(sp->info->pin_chip_sel);
	gpio_free(sp->info->pin_mosi);
	if (sp->info->write_only = 0)
		gpio_free(sp->info->pin_miso);
	gpio_free(sp->info->pin_clk);

	return 0;
}

/* all gpio should be held over suspend/resume, so we should
 * not need to deal with this
*/

#define mozart_spigpio_suspend NULL
#define mozart_spigpio_resume NULL

/* work with hotplug and coldplug */
MODULE_ALIAS("platform:spi_mozart_gpio");

static struct platform_driver mozart_spigpio_drv = {
	.probe		= mozart_spigpio_probe,
        .remove		= mozart_spigpio_remove,
        .suspend	= mozart_spigpio_suspend,
        .resume		= mozart_spigpio_resume,
        .driver		= {
		.name	= "spi_mozart_gpio",
		.owner	= THIS_MODULE,
        },
};

static int __init mozart_spigpio_init(void)
{
        return platform_driver_register(&mozart_spigpio_drv);
}

static void __exit mozart_spigpio_exit(void)
{
        platform_driver_unregister(&mozart_spigpio_drv);
}

module_init(mozart_spigpio_init);
module_exit(mozart_spigpio_exit);

MODULE_DESCRIPTION("Mozart SPI Driver");
MODULE_LICENSE("GPL");
module_param_named(spi_clk, spi_clk, int, 0444);
module_param_named(spi_miso, spi_miso, int, 0444);
module_param_named(spi_mosi, spi_mosi, int, 0444);
module_param_named(spi_chip_sel, spi_chip_sel, int, 0444);
module_param_named(write_only, write_only, int, 0444);
