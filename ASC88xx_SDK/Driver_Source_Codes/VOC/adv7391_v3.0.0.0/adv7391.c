/*
 * adv7391
 * Driver for ADV7391 TV encoder
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

#include <linux/i2c.h>
#include "tv_encoder.h"
#define ADV7391_ID_VERSION "3.0.0.0"

const char ADV7391_ID[] = "$Version: "ADV7391_ID_VERSION"  (ADV7391 DRIVER) $";

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("ADV7391 TV encoder driver");
MODULE_LICENSE("GPL");

struct adv7391 {
	struct i2c_client *client;
};
static const unsigned short normal_i2c[] = { 0x2a, I2C_CLIENT_END };//addr = 54>>1
/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;
static struct i2c_client client_template;
struct adv7391 *g_adv7391 = NULL;

static int reg_read(struct adv7391 *adv7391, const u8 reg)
{
	struct i2c_client *client = adv7391->client;
	unsigned char data = i2c_smbus_read_byte_data(client, reg);
	return data ;
}

static int reg_write(struct adv7391 *adv7391, const u8 reg,
		     const u8 data)
{
	return i2c_smbus_write_byte_data(adv7391->client, reg, data);
}

static int reg_set(struct adv7391 *adv7391, const u8 reg,
		   const u8 data)
{
	int ret;

	ret = reg_read(adv7391, reg);
	if (ret < 0)
		return ret;
	return reg_write(adv7391, reg, ret | data);
}

static int reg_clear(struct adv7391 *adv7391, const u8 reg,
		     const u8 data)
{
	int ret;

	ret = reg_read(adv7391, reg);
	if (ret < 0)
		return ret;
	return reg_write(adv7391, reg, ret & ~data);
}

/* Version 3.0.0.0 modification, 2011.09.14 */
static int adv7391_cable_detect(void)
{
	return !(reg_read(g_adv7391, 0x10)&0x01);
}
/* ======================================== */

static int adv7391_open(int pal_mode_en)
{
	if(g_adv7391 == NULL)
		return -1;
/* Version 3.0.0.0 modification, 2011.09.14 */
	reg_write(g_adv7391, 0x17, 0x02);
	reg_write(g_adv7391, 0x00, 0x12);
	reg_write(g_adv7391, 0x01, 0x00);
	reg_write(g_adv7391, 0x10, 0x10); // enable auto power down
	reg_write(g_adv7391, 0x87, 0x20); // pal/ntsc autodetection enabled
/* ======================================== */
	if (pal_mode_en==0)
	{
		//NTSC
		reg_write(g_adv7391, 0x80, 0x10);
		reg_write(g_adv7391, 0x82, 0xCB);
/* Version 2.0.0.1 modification, 2011.06.22 */
		reg_write(g_adv7391, 0x8C, 0x1F);
		reg_write(g_adv7391, 0x8D, 0x70);
		reg_write(g_adv7391, 0x8E, 0xF0);
		reg_write(g_adv7391, 0x8F, 0x21);
/* ======================================== */
	}
	else
	{
		//PAL
		reg_write(g_adv7391, 0x80, 0x11);
		reg_write(g_adv7391, 0x82, 0xC3);
		reg_write(g_adv7391, 0x8C, 0xCB);
		reg_write(g_adv7391, 0x8D, 0x8A);
		reg_write(g_adv7391, 0x8E, 0x09);
		reg_write(g_adv7391, 0x8F, 0x2A);
	}

	return 0;
}

static int adv7391_codec_probe(struct i2c_adapter *adap, int addr, int kind)
{
	struct adv7391 *adv7391;
	int ret;
	printk("ADV7391 probe\n");
	adv7391 = kzalloc(sizeof(struct adv7391), GFP_KERNEL);
	if (!adv7391)
		return -ENOMEM;

	client_template.adapter = adap;
	client_template.addr = addr;

	adv7391->client = kmemdup(&client_template, sizeof(client_template), GFP_KERNEL);
	if (adv7391->client == NULL)
		return -ENOMEM;

	ret = i2c_attach_client(adv7391->client);
	if (ret < 0) {
		printk(KERN_ERR "ADV7391 failed to attach at addr %x\n", addr);
		goto exit_err;
	}
	i2c_set_clientdata(adv7391->client, adv7391);
	g_adv7391 = adv7391;
	return 0;
exit_err:
	kfree(adv7391);
	return ret;
}


static int adv7391_probe(struct i2c_adapter *adap)
{
	printk("%s\n",__func__);
	return i2c_probe(adap, &addr_data, adv7391_codec_probe);
}

static int adv7391_remove(struct i2c_client *client)
{
	struct adv7391 *adv7391 = i2c_get_clientdata(client);

	kfree(adv7391);
	i2c_detach_client(client);
	kfree(client);

	return 0;
}

/* Version 3.0.0.0 modification, 2011.09.14 */
/* ======================================== */

struct tv_encoder_device tv_encoder_ops = {
	.open = adv7391_open,
	.release = NULL,
/* Version 3.0.0.0 modification, 2011.09.14 */
	.cable_detect = adv7391_cable_detect, 
/* ======================================== */
};
EXPORT_SYMBOL(tv_encoder_ops);

static const struct i2c_device_id adv7391_id[] = {
	{ "adv7391", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, adv7391_id);

static struct i2c_driver adv7391_i2c_driver = {
	.driver = {
		.name = "adv7391",
		.owner = THIS_MODULE,
	},
	.attach_adapter	= adv7391_probe,
	.detach_client = adv7391_remove,
	.id_table = adv7391_id,
};
static struct i2c_client client_template = {
	.name =   "adv7391",
	.driver = &adv7391_i2c_driver,
};
static int __init adv7391_mod_init(void)
{
	return i2c_add_driver(&adv7391_i2c_driver);
}

static void __exit adv7391_mod_exit(void)
{
	i2c_del_driver(&adv7391_i2c_driver);
}

module_init(adv7391_mod_init);
module_exit(adv7391_mod_exit);
