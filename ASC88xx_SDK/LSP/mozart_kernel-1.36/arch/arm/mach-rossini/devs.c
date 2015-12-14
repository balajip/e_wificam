/* linux/arch/arm/plat-s3c24xx/devs.c
 *
 * Copyright (c) 2004 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * Base S3C24XX platform device definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <asm/irq.h>
#include <linux/i2c-gpio.h>
#include <mach/spi-gpio.h>
/* Rossini MSHC registrations */

static struct resource rossini_sdhc0_resource[] = {
	[0] = {
		.start = ROSSINI_MSHC0_MMR_BASE,
		.end   = ROSSINI_MSHC0_MMR_BASE + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = MSHC0_IRQ_NUM,
		.end   = MSHC0_IRQ_NUM,
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device rossini_device_mmc0 = {
	.name = "rossini-sdhc",
	.id		= 0,
	.num_resources = ARRAY_SIZE(rossini_sdhc0_resource),
	.resource = rossini_sdhc0_resource,
};

static struct resource rossini_sdhc1_resource[] = {
	[0] = {
		.start = ROSSINI_MSHC1_MMR_BASE,
		.end   = ROSSINI_MSHC1_MMR_BASE + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = MSHC1_IRQ_NUM,
		.end   = MSHC1_IRQ_NUM,
		.flags = IORESOURCE_IRQ,
	}
};



/* yart devices */
struct platform_device rossini_device_mmc1 = {
	.name = "rossini-sdhc",
	.id		= 1,
	.num_resources = ARRAY_SIZE(rossini_sdhc1_resource),
	.resource = rossini_sdhc1_resource,
};
//------------------------------------------------
//[jam patch add] Add Nand Flash Device Info
static struct resource Rossini_nand_resource[] = {
	[0] = {
		.start = ROSSINI_NFC_MMR_BASE,
		.end   = ROSSINI_NFC_MMR_BASE + 0x1000 - 1,
		.flags = IORESOURCE_MEM,
	}
};

struct platform_device rossini_device_nand = {
	.name		   = "Rossini-nand",
	.id		       = -1,
	.num_resources = ARRAY_SIZE(Rossini_nand_resource),
	.resource	   = Rossini_nand_resource,
};
//------------------------------------------------

struct i2c_gpio_platform_data i2c_bus_0_data = {
    .sda_pin = 0,
    .scl_pin = 0,
    .udelay  = 10,
    .timeout = 10,
    .sda_is_open_drain = 1,
    .scl_is_open_drain = 1,
    .scl_is_output_only = 0
};
struct platform_device rossini_device_i2c_bus_0 = {
    .name       = "i2c-gpio",
    .id     = 0,
    .dev = {
        .platform_data = &i2c_bus_0_data,
    }
};

struct i2c_gpio_platform_data i2c_bus_1_data = {
    .sda_pin = 0,
    .scl_pin = 0,
    .udelay  = 10,
    .timeout = 10,
    .sda_is_open_drain = 1,
    .scl_is_open_drain = 1,
    .scl_is_output_only = 0
};

struct platform_device rossini_device_i2c_bus_1 = {
    .name       = "i2c-gpio",
    .id     = 1,
    .dev = {
        .platform_data = &i2c_bus_1_data,
    }
};
#ifdef CONFIG_RTC_DRV_S35390A
struct platform_device rossini_rtc = {
    .name       = "rtc-s35390a",
};
#endif
#ifdef CONFIG_RTC_DRV_PCF8563
struct platform_device rossini_rtc = {
    .name       = "rtc-pcf8563",
};
#endif
struct rossini_spigpio_info spi_info;
struct platform_device rossini_device_spi = {
    .name         = "spi_rossini_gpio",
    .id       = 0,
	.dev ={
		.platform_data = &spi_info,
	},
};

struct resource rossini_dw_spi_plat_resource[] = {
    [0] = {
        .start =  ROSSINI_SSI_MMR_BASE,
        .end   =  ROSSINI_SSI_MMR_BASE+ SZ_4K - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = SSI_IRQ_NUM,
        .end   = SSI_IRQ_NUM,
        .flags = IORESOURCE_IRQ,
    }
};

struct platform_device rossini_dw_spi_dev = {
	.name		   = "rossini_dw_spi",
	.id		       = -1,
	.num_resources = ARRAY_SIZE(rossini_dw_spi_plat_resource),
	.resource	   = rossini_dw_spi_plat_resource,
};


EXPORT_SYMBOL(rossini_device_mmc0);
EXPORT_SYMBOL(rossini_device_mmc1);
EXPORT_SYMBOL(rossini_device_nand);
EXPORT_SYMBOL(rossini_device_i2c_bus_0);
EXPORT_SYMBOL(rossini_device_i2c_bus_1);
EXPORT_SYMBOL(rossini_rtc);
EXPORT_SYMBOL(rossini_device_spi);
EXPORT_SYMBOL(rossini_dw_spi_dev);
