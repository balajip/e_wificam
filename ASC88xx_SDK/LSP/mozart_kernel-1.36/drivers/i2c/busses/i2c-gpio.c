/*
 * Bitbanging I2C bus driver using the GPIO API
 *
 * Copyright (C) 2007 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/i2c-gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <asm/gpio.h>

#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define PLATFORM    "Mozart"
#endif
#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
#define PLATFORM    "Rossini"
#endif
#ifdef MODULE
#define DRIVER_VERSION "1.0.0.2"
#define MAINTAINER "Poplar, Yang"
const char id[] = "$iicgpio: "DRIVER_VERSION" "PLATFORM" "__DATE__" "MAINTAINER" $";
#endif
struct i2c_gpio_driver_data i2c_pin_data;
static int bus_num = 0;//default bus_num
static int sda0 = 0;
static int sda1 = 0;
static int scl0 = 0;
static int scl1 = 0;

/* Toggle SDA by changing the direction of the pin */
static void i2c_gpio_setsda_dir(void *data, int state)
{
	struct i2c_gpio_platform_data *pdata = data;

	if (state)
		gpio_direction_input(pdata->sda_pin);
	else
		gpio_direction_output(pdata->sda_pin, 0);
}

static void i2c_gpio_setsda_direction(void *data, int state)
{
	struct i2c_gpio_platform_data *pdata = data;

	if (state)
		gpio_set_direction(pdata->sda_pin, 0);
	else
		gpio_set_direction(pdata->sda_pin, 1);
}

/*
 * Toggle SDA by changing the output value of the pin. This is only
 * valid for pins configured as open drain (i.e. setting the value
 * high effectively turns off the output driver.)
 */
static void i2c_gpio_setsda_val(void *data, int state)
{
	struct i2c_gpio_platform_data *pdata = data;

	gpio_set_value(pdata->sda_pin, state);
}

/* Toggle SCL by changing the direction of the pin. */
static void i2c_gpio_setscl_dir(void *data, int state)
{
	struct i2c_gpio_platform_data *pdata = data;

	if (state)
		gpio_direction_input(pdata->scl_pin);
	else
		gpio_direction_output(pdata->scl_pin, 0);
}

/*
 * Toggle SCL by changing the output value of the pin. This is used
 * for pins that are configured as open drain and for output-only
 * pins. The latter case will break the i2c protocol, but it will
 * often work in practice.
 */
static void i2c_gpio_setscl_val(void *data, int state)
{
	struct i2c_gpio_platform_data *pdata = data;

	gpio_set_value(pdata->scl_pin, state);
}

static int i2c_gpio_getsda(void *data)
{
	struct i2c_gpio_platform_data *pdata = data;
	gpio_set_direction(pdata->sda_pin, 0);
	return gpio_get_value(pdata->sda_pin);
}

static int i2c_gpio_getscl(void *data)
{
	struct i2c_gpio_platform_data *pdata = data;

	return gpio_get_value(pdata->scl_pin);
}

static int __devinit i2c_gpio_probe(struct platform_device *pdev)
{
	struct i2c_gpio_platform_data *pdata;
	struct i2c_gpio_driver_data *pindata;
	struct i2c_algo_bit_data *bit_data;
	struct i2c_adapter *adap;
	int ret;
	dev_set_drvdata(&pdev->dev, &i2c_pin_data);
	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -ENXIO;
	pindata = pdev->dev.driver_data;
	if (!pindata)
		return -ENXIO;
	if(pdev->id < i2c_pin_data.i2c_number)
	{
		switch(pdev->id)
		{
			case 0:
				pdata->sda_pin = pindata->sda_0_pin;
				pdata->scl_pin = pindata->scl_0_pin;
				break;
			case 1:
				pdata->sda_pin = pindata->sda_1_pin;
				pdata->scl_pin = pindata->scl_1_pin;
				break;
			default:
				pdata->sda_pin = pindata->sda_0_pin;
				pdata->scl_pin = pindata->scl_0_pin;
				break;
		}
	}
	else
	{
		return -ENXIO;
	}
	if((pdata->sda_pin == 255)|(pdata->scl_pin== 255))
	{
		return 0;
	}
	ret = -ENOMEM;
	adap = kzalloc(sizeof(struct i2c_adapter), GFP_KERNEL);
	if (!adap)
		goto err_alloc_adap;
	bit_data = kzalloc(sizeof(struct i2c_algo_bit_data), GFP_KERNEL);
	if (!bit_data)
		goto err_alloc_bit_data;

	ret = gpio_request(pdata->sda_pin, "sda");
	if (ret)
		goto err_request_sda;
	ret = gpio_request(pdata->scl_pin, "scl");
	if (ret)
		goto err_request_scl;

	if (pdata->sda_is_open_drain == 0) {
//		printk("[pop] sda_is_open_drain\n");
		gpio_direction_output(pdata->sda_pin, 1);
		bit_data->setsda = i2c_gpio_setsda_val;
		bit_data->setsdadir = i2c_gpio_setsda_direction;
	} else {
//		printk("[pop] sda_is not_open_drain\n");
		gpio_direction_input(pdata->sda_pin);
		bit_data->setsda = i2c_gpio_setsda_dir;
		bit_data->setsdadir = i2c_gpio_setsda_direction;
	}

	if ((pdata->scl_is_open_drain == 0 )|| pdata->scl_is_output_only) {
		gpio_direction_output(pdata->scl_pin, 1);
		bit_data->setscl = i2c_gpio_setscl_val;
	} else {
		gpio_direction_input(pdata->scl_pin);
		bit_data->setscl = i2c_gpio_setscl_dir;
	}

	if (!pdata->scl_is_output_only)
		bit_data->getscl = i2c_gpio_getscl;
	bit_data->getsda = i2c_gpio_getsda;

	if (pdata->udelay)
		bit_data->udelay = pdata->udelay;
	else if (pdata->scl_is_output_only)
		bit_data->udelay = 50;			/* 10 kHz */
	else
		bit_data->udelay = 5;			/* 100 kHz */

	if (pdata->timeout)
		bit_data->timeout = pdata->timeout;
	else
		bit_data->timeout = HZ / 10;		/* 100 ms */

	bit_data->data = pdata;

	adap->owner = THIS_MODULE;
	snprintf(adap->name, sizeof(adap->name), "i2c-gpio%d", pdev->id);
	adap->algo_data = bit_data;
	adap->class = I2C_CLASS_HWMON | I2C_CLASS_SPD;
	adap->dev.parent = &pdev->dev;

	/*
	 * If "dev->id" is negative we consider it as zero.
	 * The reason to do so is to avoid sysfs names that only make
	 * sense when there are multiple adapters.
	 */
	adap->nr = (pdev->id != -1) ? pdev->id : 0;
	ret = i2c_bit_add_numbered_bus(adap);
	if (ret)
		goto err_add_bus;

	platform_set_drvdata(pdev, adap);

	dev_info(&pdev->dev, "using pins %u (SDA) and %u (SCL%s)\n",
		 pdata->sda_pin, pdata->scl_pin,
		 pdata->scl_is_output_only
		 ? ", no clock stretching" : "");

	return 0;

err_add_bus:
	gpio_free(pdata->scl_pin);
err_request_scl:
	gpio_free(pdata->sda_pin);
err_request_sda:
	kfree(bit_data);
err_alloc_bit_data:
	kfree(adap);
err_alloc_adap:
	return ret;
}

static int __devexit i2c_gpio_remove(struct platform_device *pdev)
{
	struct i2c_gpio_platform_data *pdata;
	struct i2c_adapter *adap;

	adap = platform_get_drvdata(pdev);
	pdata = pdev->dev.platform_data;

	if((pdata->sda_pin != 255)&(pdata->scl_pin!= 255))
	{

	i2c_del_adapter(adap);
	gpio_free(pdata->scl_pin);
	gpio_free(pdata->sda_pin);
	kfree(adap->algo_data);
	kfree(adap);
	}

	return 0;
}

static struct platform_driver i2c_gpio_driver = {
	.driver		= {
		.name	= "i2c-gpio",
		.owner	= THIS_MODULE,
	},
	.probe		= i2c_gpio_probe,
	.remove		= __devexit_p(i2c_gpio_remove),
};

static int __init i2c_gpio_init(void)
{
	int ret;
	if(bus_num != 0)
	{
		i2c_pin_data.i2c_number = bus_num;
		i2c_pin_data.sda_0_pin = sda0;
		i2c_pin_data.scl_0_pin = scl0;
		i2c_pin_data.sda_1_pin = sda1;
		i2c_pin_data.scl_1_pin = scl1;

	}
	else
	{//default value
		i2c_pin_data.i2c_number = 2;
		i2c_pin_data.scl_0_pin = 14;
		i2c_pin_data.sda_0_pin = 15;
		i2c_pin_data.scl_1_pin = 12;
		i2c_pin_data.sda_1_pin = 13;
	}
	ret = platform_driver_register(&i2c_gpio_driver);
	if (ret)
		printk(KERN_ERR "i2c-gpio: probe failed: %d\n", ret);

	return ret;
}
module_init(i2c_gpio_init);

static void __exit i2c_gpio_exit(void)
{
	platform_driver_unregister(&i2c_gpio_driver);
}
module_exit(i2c_gpio_exit);

MODULE_AUTHOR("Haavard Skinnemoen <hskinnemoen@atmel.com>");
MODULE_DESCRIPTION("Platform-independent bitbanging I2C driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:i2c-gpio");
module_param_named(bus_num, bus_num, int, 0444);
module_param_named(sda0, sda0, int, 0444);
module_param_named(scl0, scl0, int, 0444);
module_param_named(sda1, sda1, int, 0444);
module_param_named(scl1, scl1, int, 0444);
