/*
 * Copyright (C) 2013  VN Inc.
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
#include <linux/ioport.h>
#include <linux/stmmac.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <mach/ugpio.h>

/* -------------------------------------------------- */
static const char *phy_modes[] = {
	/* [PHY_INTERFACE_MODE_NA]         = "", */
	[PHY_INTERFACE_MODE_MII]        = "mii",
	[PHY_INTERFACE_MODE_GMII]       = "gmii",
	[PHY_INTERFACE_MODE_SGMII]      = "sgmii",
	[PHY_INTERFACE_MODE_TBI]        = "tbi",
	[PHY_INTERFACE_MODE_RMII]       = "rmii",
	[PHY_INTERFACE_MODE_RGMII]      = "rgmii",
	[PHY_INTERFACE_MODE_RGMII_ID]   = "rgmii-id",
	[PHY_INTERFACE_MODE_RGMII_RXID] = "rgmii-rxid",
	[PHY_INTERFACE_MODE_RGMII_TXID] = "rgmii-txid",
	[PHY_INTERFACE_MODE_RTBI]       = "rtbi",
	/* [PHY_INTERFACE_MODE_SMII]       = "smii", */
};

static inline char *cmdline_find_option(char *str)
{
	extern char *saved_command_line;
	return strstr(saved_command_line, str);
}

static phy_interface_t parse_cmdlin_phymod(void)
{
	phy_interface_t phy_mode = PHY_INTERFACE_MODE_GMII;
	char *cp;
	int i, rgmii = 0;

	/* Parse rgmii=0/1 or phy_mode = rgmii/gmii/mii/rmii */
	if ((cp = cmdline_find_option("rgmii="))) {
		cp+=strlen("rgmii=");
		if (get_option(&cp, &rgmii) && rgmii)
			phy_mode = PHY_INTERFACE_MODE_RGMII;
	}

	if ((cp = cmdline_find_option("phy_mode="))) {
		cp+=strlen("phy_mode=");
		for (i = 0; i < ARRAY_SIZE(phy_modes); i++)
			if (!strncasecmp(phy_modes[i], cp, strlen(phy_modes[i]))) {
				phy_mode = i;
				break;
			}
	}

	switch (phy_mode) {
		case PHY_INTERFACE_MODE_MII:
		case PHY_INTERFACE_MODE_GMII:
		case PHY_INTERFACE_MODE_RGMII:
			break;
		default:
			printk("PHY mode %s is Unsupported, force to GMII\n", cp);
			phy_mode = PHY_INTERFACE_MODE_GMII;
	}
	pr_debug("PHY Mode: %s\n", phy_modes[phy_mode]);

	return phy_mode;
}

static struct stmmac_mdio_bus_data rossini_dwmac_mdio_data = {
	.phy_mask = 0x1, /* mask addr 0, which is valid for most phy brodcast */
	/*
	   .phy_reset = NULL
	   .irqs = NULL,
	   .probed_phy_irq = 0,
	 */
};

static void rossini_dwmac_fix_mac_speed_v1(void *bsp_priv, unsigned int speed)
{
	struct plat_stmmacenet_data *plat_dat = (struct plat_stmmacenet_data *) bsp_priv;
	void __iomem *sysc_base = ioremap(VPL_SYSC_MMR_BASE, SZ_4K);

	/*
	 * In Rossini V1, UART DTR ping is used to control Tx Clock direction
	 * clear_ugpio_pin(): Internal Tx Clk MAC->PHY
	 * set_ugpio_pin()  : External Tx Clk MAC<-PHY
	 */

	u32 reg;
	if (plat_dat->interface == PHY_INTERFACE_MODE_RGMII) {
		clear_ugpio_pin(PIN_UGPIO4);
		reg = readl(sysc_base + 0x2C);
		reg &= ~(0x3 << 30);
		switch (speed) {
			case 1000:
				/* Provide 125MHz TX clock */
				reg |= (0x0 << 30);
				break;
			case 100:
				/* Provide 25MHz TX clock */
				reg |= (0x2 << 30);
				break;
			case 10:
				/* Provide 2.5MHz TX clock */
				reg |= (0x3 << 30);
				break;
		}
		writel(reg, sysc_base + 0x2C);
	} else {
		switch (speed) {
			case 1000:
				/* Provide 125MHz TX clock */
				clear_ugpio_pin(PIN_UGPIO4);
				reg = readl(sysc_base + 0x2C);
				reg &= ~(0x3 << 30);
				reg |= (0x1 << 30);
				writel(reg, sysc_base + 0x2C);
				break;
			case 100:
				/* TX CLK is external clock source, don't care speed */
				set_ugpio_pin(PIN_UGPIO4);
				break;
			case 10:
				/* TX CLK is external clock source, don't care speed */
				set_ugpio_pin(PIN_UGPIO4);
				break;
		}
	}

	iounmap(sysc_base);
}

static void rossini_dwmac_fix_mac_speed(void *bsp_priv, unsigned int speed)
{
	struct plat_stmmacenet_data *plat_dat = (struct plat_stmmacenet_data *) bsp_priv;
	void __iomem *sysc_base = ioremap(VPL_SYSC_MMR_BASE, SZ_4K);
	/*
	 * SYSC_CLK_GEN_CFG 0x2C
	 * [31:30]: 2'b00: 125MHz; 2'b01: 25MHz; 2'b10: 2.5MHz; 2'b11: Reserved
	 * [23] GMAC_REDUCED: 1'b0: Shift 180 degrees ; 1'b1: Shift 90 degrees
	 *
	 * SYSC_IF_CTRL 0x48
	 * [1]: 1'b0: Internal Tx CLK; 1'b1: External Tx CLK;
	 * [2]: 1'b0: 3.3V; 1'b1: 2.5V
	 */
	u32 reg;
	if (plat_dat->interface == PHY_INTERFACE_MODE_RGMII) {
		reg = readl(sysc_base + 0x2C);
		reg &= ~(0x3 << 30);
		switch (speed) {
			case 1000:
				/* Provide 125MHz TX clock */
				reg |= (0x0 << 30);
				break;
			case 100:
				/* Provide 25MHz TX clock */
				reg |= (0x1 << 30);
				break;
			case 10:
				/* Provide 2.5MHz TX clock */
				reg |= (0x2 << 30);
				break;
		}
		writel(reg, sysc_base + 0x2C);
	} else {
		switch (speed) {
			case 1000:
				/* Provide 125MHz TX clock */
				reg = readl(sysc_base + 0x48);
				reg &= ~(0x1<< 1);
				reg |= (0x0<< 1);
				writel(reg, sysc_base + 0x48);

				reg = readl(sysc_base + 0x2C);
				reg &= ~(0x3 << 30);
				reg |= (0x0 << 30);
				writel(reg, sysc_base + 0x2C);
				break;
			case 100:
				/* TX CLK is external clock source, don't care speed */
				reg = readl(sysc_base + 0x48);
				reg &= ~(0x1<< 1);
				reg |= (0x1<< 1);
				writel(reg, sysc_base + 0x48);
				break;
			case 10:
				/* TX CLK is external clock source, don't care speed */
				reg = readl(sysc_base + 0x48);
				reg &= ~(0x1<< 1);
				reg |= (0x1<< 1);
				writel(reg, sysc_base + 0x48);
				break;
		}
	}

	iounmap(sysc_base);
}

static void rossini_dwmac_bus_setup(void __iomem *ioaddr)
{
	pr_debug("[GMAC]: %s\n", __FUNCTION__);
}

static int rossini_dwmac_init_v1(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat = pdev->dev.platform_data;
	void __iomem *sysc_base = ioremap(VPL_SYSC_MMR_BASE, SZ_4K);
	u32 value;

	/* ToDo: PAD/CLOCK enable/disable */
	pr_debug("[GMAC]: %s mac id %d\n", __FUNCTION__, pdev->id);

	/*
	 * Assign Interface. Overwrite by steps
	 * 1. command line
	 * 2. HW setting (V1 only)
	 * 3. Chip limitation.
	 */
	plat_dat->interface = parse_cmdlin_phymod();

	/*
	 * HW setting: Rossini V1 can get HW 2.5V or 3.3V from sysc
	 * for other modle this bit is result of SW control at 0x48
	 */
	if (readl(sysc_base + 0x34) & 0x4)
		plat_dat->interface = PHY_INTERFACE_MODE_RGMII;

	/* Chip limitation: For M325, M330 and M330s, only support MII interface */
	value = readl(sysc_base + 0x3C);
	if ((value & 0x7) == 0x5)
		plat_dat->interface = PHY_INTERFACE_MODE_MII;

	pr_info("GMAC: PHY mode is %s\n", phy_modes[plat_dat->interface]);

	/*
	 * To set GMAC work at correct interface mode.
	 * The following settings must be confirmed.
	 *
	 * 1. Set UART0 SERIAL_MCR_DTR
	 *    In Rossini V1, UART0 DTR ping is used to control Tx Clock direction
	 *    clear_ugpio_pin(PIN_UGPIO4): Internal Tx Clk MAC->PHY
	 *    set_ugpio_pin(PIN_UGPIO4): External Tx Clk MAC<-PHY
	 *
	 * 2. Set SYSC_CLK_GEN_CFG 0x2C
	 *      [30] GMAC_TX_CLK_CFG: GMAC TX clock speed selection
	 *           2'b00 125MHz(RGMII); 2'b01 125MHz(GMII);
	 *           2'b10 25MHz;         2'b11 2.5MHz
	 *
	 *          | PIN_UGPIO4 |   GMAC_TX_CLK_CFG
	 * ------------------------------------------------
	 * GMII/MII |   clear    |   2'b01 (125MHz GMII)
	 * RGMII    |   clear    |   2'b00 (125MHz RGMII)
	 *
	 * 3. Reset GMAC DMA in driver.
	 *
	 * After Reset DMA Engine, the Tx direction and Speed are free to change
	 * accroding to PHY's speed.
	 */

	if (request_ugpio_pin(PIN_UGPIO4) < 0) {
		pr_err("GMAC: TX_CLK direction switch pin was already occupied\n");
		return -EBUSY;
	}

	switch (plat_dat->interface) {
		case PHY_INTERFACE_MODE_MII:
		case PHY_INTERFACE_MODE_GMII:
			{
				/* Internal Tx clock */
				clear_ugpio_pin(PIN_UGPIO4);

				/* 125MHz GMII */
				value = readl(sysc_base + 0x2C);
				value &= ~(0x3 << 30);
				value |= (0x1 << 30);
				writel(value, sysc_base + 0x2C);
				pr_debug("set sysc_bas2c %x\n", value);
			}
			break;
		case PHY_INTERFACE_MODE_RGMII:
			{
				/* Internal Tx clock */
				clear_ugpio_pin(PIN_UGPIO4);

				/* 125MHz RGMII */
				value = readl(sysc_base + 0x2C);
				value &= ~(0x3 << 30);
				writel(value, sysc_base + 0x2C);
				pr_debug("set sysc_bas2c %x\n", value);

				/* Workaround Tx Clock out, MON_CLK_1_SEL monitor GMAC TX positive clock */
				value = readl(sysc_base + 0x30);
				value &=~0x3F00;
				value |= 0x0200;
				writel(value, sysc_base + 0x30);
			}
			break;
	}

	iounmap(sysc_base);

	plat_dat->bsp_priv = pdev->dev.platform_data;
	return 0;
};

static int rossini_dwmac_init(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat = pdev->dev.platform_data;
	void __iomem *sysc_base = ioremap(VPL_SYSC_MMR_BASE, SZ_4K);
	u32 value;

	/* ToDo: PAD/CLOCK enable/disable */
	pr_debug("[GMAC]: %s mac id %d\n", __FUNCTION__, pdev->id);

	/*
	 * Assign Interface. Overwrite by steps
	 * 1. command line
	 * 2. HW setting (V1 only)
	 * 3. Chip limitation.
	 */
	plat_dat->interface = parse_cmdlin_phymod();

	/* Chip limitation: For M325, M330 and M330s, only support MII interface */
	value = readl(sysc_base + 0x3C);
	if ((value & 0x7) == 0x5)
		plat_dat->interface = PHY_INTERFACE_MODE_MII;

	pr_info("GMAC: PHY mode is %s\n", phy_modes[plat_dat->interface]);

	/*
	 * To set GMAC work at correct interface mode.
	 * The following settings must be confirmed.
	 *
	 * 1. Set SYSC_IF_CTRL 0x48
	 *     [2] GMAC_IF_PWR_LEVEL: 1'b0: 3.3V, 1'b1:2.5V
	 *     [1] GMAC_TX_CLK_DIR:  GMAC Tx clock direction selection
	 *                           1'b0: Internal MAC->PHY, 1'b1: External MAC<-PHY
	 *
	 *          | GMAC_IF_PWR_LEVEL | GMAC_TX_CLK_DIR
	 * -----------------------------------------------------
	 * GMII/MII |     1'b0          |  1'b0
	 * RGMII    |     1'b1          |  1'b0
	 *
	 * 2. Set SYSC_CLK_GEN_CFG 0x2C
	 *    [31:30] GMAC_TX_CLK_CFG: 2'b00: GMAC TX clock speed selection
	 *                                    2'b00: 125MHz, 2'b01: 25MHz, 2'b10: 2.5MHz
	 *	 [23] GMAC_REDUCED_MODE_EN: GMAC reduced mode & Tx Phase
	 *	                            1'b0: 180 degree, 1'b1: 90 degree
	 *
	 *          |  GMAC_TX_CLK_CFG  | GMAC_REDUCED_MODE_EN
	 * -----------------------------------------------------
	 * GMII/MII |   2'b00 (125MHz)  |      1'b0
	 * RGMII    |   2'b00 (125MHz)  |      1'b1
	 *
	 * 3. Reset GMAC DMA in driver.
	 *
	 * After Reset DMA Engine, the Tx direction and Speed are free to change
	 * accroding to PHY's speed.
	 */

	switch (plat_dat->interface) {
		case PHY_INTERFACE_MODE_MII:
		case PHY_INTERFACE_MODE_GMII:
			{
				/* 3.3V & Internal Tx clock*/
				value = readl(sysc_base + 0x48);
				value &= ~(0x1 << 2);
				value &= ~(0x1 << 1);
				writel(value, sysc_base + 0x48);

				/* 125MHz, NOT Reduce Mode */
				value =  readl(sysc_base + 0x2C);
				value &= ~(0x3 << 30);
				value &= ~(0x1 << 23);
				writel(value, sysc_base + 0x2C);
			}
			break;
		case PHY_INTERFACE_MODE_RGMII:
			{
				/* 2.5V, Internal Tx clock*/
				value = readl(sysc_base + 0x48);
				value &= ~(0x1 << 1);
				value |= (0x1 << 2);
				writel(value, sysc_base + 0x48);
				pr_debug("set sysc_bas48 %x\n", value);

				/* 125MHz, Reduce Mode */
				value =  readl(sysc_base + 0x2C);
				value &= ~(0x3 << 30);
				value |= (0x1 << 23);
				writel(value, sysc_base + 0x2C);
				pr_debug("set sysc_bas2c %x\n", value);
			}
			break;
	}

	iounmap(sysc_base);

	plat_dat->bsp_priv = pdev->dev.platform_data;
	return 0;
};

static void rossini_dwmac_exit(struct platform_device *pdev)
{
	pr_debug("%s\n", __FUNCTION__);
};

/*
 * tx_coe:
 *          For Rossini, GMAC 3.42a without HW capability register.
 *          Accroding to databook, the IPC_FULL_OFFLOAD include Rx and Tx.
 *          Assigend manual tx_cot = 1
 *
 *          For Beethoven, GMAC 3.70 with HW capability register,
 *          The tx_coe can be check by driver.
 *
 *	    Note that, if tx_coe is avaliable in Hardware, MUST use Store-and-Forward Mode
 *
 * .bus_setup: call by driver when open device
 * .init:      call by driver probe
 * .exit:      call by driver exit
 * .pbl:       The driver can set x4(Rossini) or x8 (Beethoven)
 *             The PBL will effect available space in FIFO.
 *             Frame Size < FIFO Depth - PBL - 3 FIFO Locations
 *             ex. 2048 (FIFO depth) /8(data bus width in byte) - 64 - 3
 *                = 189 beats = 1512 bytes
 *             Generally, net mtu is 1500, so we can't set PBL too large,ex. 32x4,
 *	       this will result not enought 1000 bytes threshold.
 *	       Reference to section 4.7 in databook v3.70
 *	       In TCP case, the maximum frame is 1460+IPHeader(20)+TCP Header(20)+eth Header(14)=1514 bytes.
 *	       Use pbl=16*4=64 , 1512 bytes threshold is also not enought.
 *	       Set to pbl=8*4=32, 1768 bytes
 */
static struct stmmac_dma_cfg rossini_dwmac_dma_cfg = {
	.pbl = 8,
	.fixed_burst = 0,
	.mixed_burst = 0,
	.burst_len = 0,
};

static struct plat_stmmacenet_data rossini_dwmac_platform_data = {
	.phy_bus_name = "dwmac",
	.bus_id = 0,
	.phy_addr = -1, /*auto phy by pooling by mdio bus */
	/* .interface = given by .init callback */
	.mdio_bus_data = &rossini_dwmac_mdio_data,
	.dma_cfg = &rossini_dwmac_dma_cfg,
	/* .clk_csr = set dynamically according to AHB CLK in driver */
	.has_gmac = 1,
	.enh_desc = 0,
	.tx_coe =1,
	.rx_coe = STMMAC_RX_COE_TYPE2,
	.fix_mac_speed = rossini_dwmac_fix_mac_speed,
	.bus_setup = rossini_dwmac_bus_setup,
	.init = rossini_dwmac_init,
	.exit = rossini_dwmac_exit,
};

static struct resource rossini_dwmac_resource[] = {
	[0] = {
		.start  = VPL_GMAC_MMR_BASE,
		.end    = VPL_GMAC_MMR_BASE + SZ_8K -1,
		.flags  = IORESOURCE_MEM,
	},

	[1] = {
		.start = GMAC_IRQ_NUM,
		.flags = IORESOURCE_IRQ,
		.name = "dwmac_irq",
	},
};

static u64 dwmac_dma_mask = ~(u32) 0;

struct platform_device rossini_device_dwmac = {
	.name       = "dwmac",
	.id         = 0,
	.num_resources = ARRAY_SIZE(rossini_dwmac_resource),
	.resource = rossini_dwmac_resource,
	.dev = {
		.platform_data = &rossini_dwmac_platform_data,
		.dma_mask =  &dwmac_dma_mask,
		.coherent_dma_mask = ~0,
		/* .power = no pm, no wake up support in Beethoven */
	},
};
/* -------------------------------------------------- */

void __init rossini_gmac_init(void)
{
	void __iomem *sysc_base = ioremap(VPL_SYSC_MMR_BASE, SZ_4K);
	u32 chip_version;
	chip_version = readl(sysc_base + 0x14);
	if (chip_version == 0x01000000) {
		rossini_dwmac_platform_data.fix_mac_speed = rossini_dwmac_fix_mac_speed_v1;
		rossini_dwmac_platform_data.init = rossini_dwmac_init_v1;
	}
	iounmap(sysc_base);
}

EXPORT_SYMBOL(rossini_device_dwmac);
