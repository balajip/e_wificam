/*
 * drivers/net/phy/realtek.c
 *
 * Driver for Realtek PHYs
 *
 * Author: Johnson Leung <r58129@freescale.com>
 *
 * Copyright (c) 2004 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/phy.h>
#include <linux/delay.h>
#define RTL821x_PHYSR		0x11
#define RTL821x_PHYSR_DUPLEX	0x2000
#define RTL821x_PHYSR_SPEED	0xc000
#define RTL821x_INER		0x12
#define RTL821x_INER_INIT	0x6400
#define RTL821x_INSR		0x13

MODULE_DESCRIPTION("Realtek PHY driver");
MODULE_AUTHOR("Johnson Leung");
MODULE_LICENSE("GPL");

static int rtl821x_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = phy_read(phydev, RTL821x_INSR);

	return (err < 0) ? err : 0;
}

static int rtl821x_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, RTL821x_INER,
				RTL821x_INER_INIT);
	else
		err = phy_write(phydev, RTL821x_INER, 0);

	return err;
}

static int  rtl821x_config_init(struct phy_device *phydev)
{
	int phy_control = BMCR_RESET;
	int limit;

	/* First clear the PHY */
	phy_write(phydev, MII_BMCR, phy_control);
	limit = 5000;
	while (limit--) {
		phy_control = phy_read(phydev, MII_BMCR);
		if ((phy_control & BMCR_RESET) == 0) {
			udelay(40);
			break;
		}
		udelay(10);
	}

	if (limit < 0)
	{
		printk("oops not reset done\n");
		return -EBUSY;
	}
	return 0;
};


/* RTL8211B */
static struct phy_driver rtl821x_driver = {
	.phy_id		= 0x001cc910,
	.name		= "RTL8211EG Gigabit Ethernet",
	.phy_id_mask	= 0x001ffff0,
	.features	= PHY_GBIT_FEATURES,// | SUPPORTED_Pause,
	.flags		= PHY_POLL,
	.config_init    = &rtl821x_config_init,
	.config_aneg	= &genphy_config_aneg,
	.read_status	= &genphy_read_status,
	.ack_interrupt	= &rtl821x_ack_interrupt,
	.config_intr	= &rtl821x_config_intr,
	.driver		= { .owner = THIS_MODULE,},
};

static int __init realtek_init(void)
{
	return phy_driver_register(&rtl821x_driver);
}

static void __exit realtek_exit(void)
{
	phy_driver_unregister(&rtl821x_driver);
}

module_init(realtek_init);
module_exit(realtek_exit);
