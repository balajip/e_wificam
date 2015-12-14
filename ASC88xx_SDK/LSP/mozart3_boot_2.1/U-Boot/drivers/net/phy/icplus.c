/*
 * ICPlus PHY drivers
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 * author Andy Fleming
 *
 */
#include <config.h>
#include <common.h>
#include <phy.h>

#define PHY_AUTONEGOTIATE_TIMEOUT 5000

/* IP101A/G - IP1001 */
#define IP10XX_SPEC_CTRL_STATUS         16      /* Spec. Control Register */
#define IP1001_RX_PHASE_SEL_MASK        0x1      /* IP1001 RXPHASE_SEL */
#define IP1001_TX_PHASE_SEL_MASK        0x2      /* IP1001 TXPHASE_SEL */
#define IP101A_G_IRQ_CONF_STATUS        0x11    /* Conf Info IRQ & Status Reg */
#define IP101A_G_IRQ_PIN_USED           (1<<15) /* INTR pin used */
#define IP101A_G_IRQ_DEFAULT            IP101A_G_IRQ_PIN_USED

/* IP1001 PHY Status Register */
#define MIIM_IP1001_PHY_STATUS       0x11
#define MIIM_IP1001_PHYSTAT_SPEED    0x6000
#define MIIM_IP1001_PHYSTAT_GBIT     0x4000
#define MIIM_IP1001_PHYSTAT_100      0x2000
#define MIIM_IP1001_PHYSTAT_DUPLEX   0x1000
#define MIIM_IP1001_PHYSTAT_LINK     0x8000
/*---------------------------------------------------------------------*/
#undef DEBUG_PHY

#ifdef DEBUG_PHY
#define DEBUGF(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGF(fmt,args...)
#endif
/*---------------------------------------------------------------------*/
/* ICPlus 1001 */
static int ip1001_config(struct phy_device *phydev)
{
	int c;
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, BMCR_RESET);

	/*
	 * [Vincent]: In Mozart, Tx clock phase is done by internal PLL through SYSC setting.
	 * 	      Rx clock phase is done by PHY's Hardware pin configuration
	 *            We still set Rx clock phase here to capture hardware bug if lost.
	 * NOTE: This register is not SW resetable, the value will keep even BMCR_RESET is applied.
	 */
	if (phydev->interface == PHY_INTERFACE_MODE_RGMII) {
		c = phy_read(phydev, MDIO_DEVAD_NONE, IP10XX_SPEC_CTRL_STATUS);
		if (c < 0)
		        return c;
		c &=~IP1001_TX_PHASE_SEL_MASK;
		c |= IP1001_RX_PHASE_SEL_MASK;
		c = phy_write(phydev, MDIO_DEVAD_NONE, IP10XX_SPEC_CTRL_STATUS, c);
		if (c < 0)
		        return c;
	}

	genphy_config_aneg(phydev);

	return 0;
}

static int ip1001_parse_status(struct phy_device *phydev)
{
	unsigned int speed;
	unsigned int mii_reg;

	mii_reg = phy_read(phydev, MDIO_DEVAD_NONE, MIIM_IP1001_PHY_STATUS);

	if (!(mii_reg & MIIM_IP1001_PHYSTAT_LINK)) {
		int i = 0;

		/* in case of timeout ->link is cleared */
		phydev->link = 1;
		DEBUGF("  Link Status  : Waiting for PHY realtime link");
		while (!(mii_reg & MIIM_IP1001_PHYSTAT_LINK)) {
			/* Timeout reached ? */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				DEBUGF(" TIMEOUT !\n");
				phydev->link = 0;
				break;
			}

			if ((i++ % 1000) == 0)
				DEBUGF(".");
			udelay(1000);	/* 1 ms */
			mii_reg = phy_read(phydev, MDIO_DEVAD_NONE,
					MIIM_IP1001_PHY_STATUS);
		}
		DEBUGF(" done\n");
		udelay(500000);	/* another 500 ms (results in faster booting) */
	} else {
		phydev->link = 1;
	}

	if (mii_reg & MIIM_IP1001_PHYSTAT_DUPLEX)
		phydev->duplex = DUPLEX_FULL;
	else
		phydev->duplex = DUPLEX_HALF;

	speed = (mii_reg & MIIM_IP1001_PHYSTAT_SPEED);

	switch (speed) {
		case MIIM_IP1001_PHYSTAT_GBIT:
			phydev->speed = SPEED_1000;
			break;
		case MIIM_IP1001_PHYSTAT_100:
			phydev->speed = SPEED_100;
			break;
		default:
			phydev->speed = SPEED_10;
	}

	return 0;
}

static int ip1001_startup(struct phy_device *phydev)
{
	/* Read the Status (2x to make sure link is right) */
	genphy_update_link(phydev);
	ip1001_parse_status(phydev);

	return 0;
}

static struct phy_driver IP1001_driver = {
	.name = "ICPlus IP1001",
	.uid = 0x02430d90,
	.mask = 0x0ffffff0,
	.features = PHY_GBIT_FEATURES,
	.config = &ip1001_config,
	.startup = &ip1001_startup,
	.shutdown = &genphy_shutdown,
};

static struct phy_driver IP101_driver = {
	.name = "ICPlus IP101A/G",
	.uid = 0x02430c54,
	.mask = 0x0ffffff0,
	.features = PHY_BASIC_FEATURES,
	.config = &genphy_config,
	.startup = &genphy_startup,
	.shutdown = &genphy_shutdown,
};

int phy_icplus_init(void)
{
	phy_register(&IP1001_driver);
	phy_register(&IP101_driver);
	return 0;
}
