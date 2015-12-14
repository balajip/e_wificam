#include <common.h>
#include <malloc.h>
#include <linux/err.h>
#include <asm/arch/platform.h>
#include <phy.h>
#include "gmac.h"

/* MII address register definitions */
#define MII_BUSY		(1 << 0)
#define MII_WRITE		(1 << 1)
#define MII_CLKRANGE_60_100M	(0)
#define MII_CLKRANGE_100_150M	(0x4)
#define MII_CLKRANGE_20_35M	(0x8)
#define MII_CLKRANGE_35_60M	(0xC)
#define MII_CLKRANGE_150_250M	(0x10)
#define MII_CLKRANGE_250_300M	(0x14)

#define MIIADDRSHIFT		(11)
#define MIIREGSHIFT		(6)
#define MII_REGMSK		(0x1F << 6)
#define MII_ADDRMSK		(0x1F << 11)

static int gmac_mdio_read(struct mii_dev *bus, int addr, int devad,
				int reg)
{
	TGMAC *priv = bus->priv;
	int data;

	u16 regValue = ((addr << MIIADDRSHIFT) & MII_ADDRMSK) |
		       ((reg << MIIREGSHIFT) & MII_REGMSK);
	regValue |= MII_CLKRANGE_150_250M | MII_BUSY;

	do {} while (((v_inl(priv->macBase + GmacGmiiAddr)) & MII_BUSY) == 1);
	v_outl(priv->macBase + GmacGmiiAddr, regValue);
	do {} while (((v_inl(priv->macBase + GmacGmiiAddr)) & MII_BUSY) == 1);

	/* Read the data from the MII data register */
	data = (int)v_inl(priv->macBase + GmacGmiiData);

	return data;
}

static int gmac_mdio_write(struct mii_dev *bus, int addr, int devad,
				int reg, u16 val)
{
	TGMAC *priv = bus->priv;

	u16 regValue = ((addr << MIIADDRSHIFT) & MII_ADDRMSK) |
		       ((reg << MIIREGSHIFT) & MII_REGMSK) |
		       MII_WRITE;

	regValue |=  MII_CLKRANGE_150_250M | MII_BUSY;

	/* Wait until any existing MII operation is complete */
	do {} while (((v_inl(priv->macBase + GmacGmiiAddr)) & MII_BUSY) == 1);

	/* Set the MII address register to write */
	v_outl(priv->macBase + GmacGmiiData, val);
	v_outl(priv->macBase + GmacGmiiAddr, regValue);

	/* Wait until any existing MII operation is complete */
	do {} while (((v_inl(priv->macBase + GmacGmiiAddr)) & MII_BUSY) == 1);

	return 0;
}

static struct mii_dev *mdio_alloc(void)
{
	struct mii_dev *bus;

	bus = malloc(sizeof(*bus));
	if (!bus)
		return bus;

	memset(bus, 0, sizeof(*bus));

	/* initalize mii_dev struct fields */
	INIT_LIST_HEAD(&bus->link);

	return bus;
}

int gmac_mdio_init(TGMAC *gc)
{
	struct mii_dev *bus = mdio_alloc();

	if (!bus) {
		printf("Failed to allocate GMAC MDIO bus\n");
		return -ENOMEM;
	}

	bus->read = gmac_mdio_read;
	bus->write = gmac_mdio_write;
	bus->priv = gc;
	strncpy(bus->name, "gmac_mdio", MDIO_NAME_LEN);
	bus->name[MDIO_NAME_LEN - 1] = 0;
	/* skip phyaddr 0, since also for boradcast PHY address */
	bus->phy_mask = 0x1;
	gc->mdio_bus = bus;

	return 0;
}
