#include <common.h>
#include <malloc.h>
#include <net.h>
#include <netdev.h>

#include <asm/arch/platform.h>
#include "gmac.h"

#if !defined(CONFIG_MII)
# error "MOZART_GMAC requires MII -- missing CONFIG_MII"
#endif

#if !defined(CONFIG_PHYLIB)
# error "MOZART_GMAC requires PHYLIB -- missing CONFIG_PHYLIB"
#endif

//-1 : no quick-test
// 0 : force to use 10/100Mbps
// 1 : force to use 1Gbps
int gmac_quick_testspeed = -1;

void gmac_set_ethaddr(TGMAC *dev, u8 *Addr);

static TPrivate gmac_priv;
u8 MAC_Addr[6] = {0x00, 0xab, 0xcd, 0xab, 0xcd, 0xef};

static void __inline__ GMACWriteMacReg( TGMAC *Dev, u32 Reg, u32 Data )
{
    v_outl(Dev->macBase+Reg, Data);
}

static u32 __inline__ GMACReadMacReg( TGMAC *Dev, u32 Reg )
{
    u32 data = v_inl(Dev->macBase+Reg);
    return data;
}

static void __inline__ GMACSetMacReg( TGMAC *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->macBase+Reg;
  u32 data = v_inl(addr);
  data |= Data;
  v_outl(addr, data);
}

static void __inline__ GMACClearMacReg( TGMAC *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->macBase+Reg;
  u32 data = v_inl(addr);
  data &= ~Data;
  v_outl(addr, data);
}

static u32 __inline__ GMACReadDmaReg( TGMAC *Dev, u32 Reg )
{
  u32 data = v_inl(Dev->dmaBase+Reg);
  return data;
}

static void __inline__ GMACWriteDmaReg( TGMAC *Dev, u32 Reg, u32 Data )
{
  v_outl(Dev->dmaBase+Reg, Data);
}

static void __inline__ GMACSetDmaReg( TGMAC *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->dmaBase+Reg;
  u32 data = v_inl(addr);
  data |= Data;
  v_outl(addr, data);
}

static void __inline__ GMACClearDmaReg( TGMAC *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->dmaBase+Reg;
  u32 data = v_inl(addr);
  data &= ~Data;
  v_outl(addr, data);
}

static void DescInit( TDmaDesc *Desc, bool EndOfRing )
{
  Desc->status = 0;
  Desc->length = EndOfRing ? DescEndOfRing : 0;
  Desc->buffer1 = 0;
  Desc->buffer2 = 0;
}

static bool DescLast( TDmaDesc *Desc )
{
  return (Desc->length & DescEndOfRing) != 0;
}

static bool DescEmpty( TDmaDesc *Desc )
{
  return (Desc->length & ~ DescEndOfRing) == 0;
}

static bool DescDma( TDmaDesc *Desc )
{
  return (Desc->status & DescOwnByDma) != 0;   /* Owned by DMA */
}

static void DescTake( TDmaDesc *Desc )   /* Take ownership */
{
  if( DescDma(Desc) )               /* if descriptor is owned by DMA engine */
  {
    Desc->status &= ~DescOwnByDma;    /* clear DMA own bit */
    Desc->status |= DescError;        /* Set error bit to mark this descriptor bad */
  }
}

int GMACMacInit( TGMAC *dev, u8 Addr[6], int quiet )
{
	struct phy_device *phydev = dev->phydev;

	if(!quiet) printf("  MAC Address  : %02x %02x %02x %02x %02x %02x\n", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
	gmac_set_ethaddr(dev, Addr) ;

	if (phydev->duplex) {
		/* Added by Lokesh */
		if (phydev->speed == SPEED_1000) {
			if (dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, 0x01200D0C ); /* set init values of config registers */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx1000 ); /* set init values of config registers */
		} else if (phydev->speed == SPEED_100) {
			if (dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, 0x0120CD0C ); /* set init values of config registers */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx100 ); /* set init values of config registers with MII port */
		} else if (phydev->speed == SPEED_10) {
			if (dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, 0x01208D0C ); /* set init values of config registers with MII port */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx10 ); /* set init values of config registers with MII port */
		}

		GMACWriteMacReg( dev, GmacFrameFilter, GmacFrameFilterInitFdx );
		GMACWriteMacReg( dev, GmacFlowControl, GmacFlowControlInitFdx );
	}
	else
	{
		/* Modified by Lokesh */
		if(phydev->speed == SPEED_1000)
			GMACWriteMacReg( dev, GmacConfig, GmacConfigInitHdx1000 ); /* set init values of config registers */
		else
			GMACWriteMacReg( dev, GmacConfig, GmacConfigInitHdx110 ); /* set init values of config registers with MII port */

		GMACWriteMacReg( dev, GmacFrameFilter, GmacFrameFilterInitHdx );
		GMACWriteMacReg( dev, GmacFlowControl, GmacFlowControlInitHdx );
		/* To set PHY register to enable CRS on Transmit */
		/* OOPS: THis is phy specified ...*/
	}

	return 0;
}

int GMACDmaRxSet( TGMAC *Dev, u32 Buffer1, u32 Length1)
{
  int desc = Dev->rxNext;
  TDmaDesc *rx = Dev->rx + desc;
  if( !DescEmpty(rx) ) return -1;       /* descriptor is not empty - cannot set */

  rx->length &= DescEndOfRing;      /* clear everything except the EndOfRing bit-field*/
  rx->length |= ((Length1 << DescSize1Shift) & DescSize1Mask);
  rx->buffer1 = Buffer1;
  rx->status  = DescOwnByDma;//Ready for SNPS-GMAC DMA to use
  Dev->rxNext = DescLast(rx) ? 0 : desc + 1;

  return desc;
}

int GMACDmaInit( TGMAC *Dev, void *Buffer, u32 Size )
{
  int n = Size/sizeof(TDmaDesc);     /* number of descriptors that can fit into the buffer passed */
  int i;

  Dev->txCount = n/2;               /* use 1/2 for Tx descriptors and 1/2 for Rx descriptors */
  Dev->rxCount = n - Dev->txCount;

  Dev->tx = Buffer;
  Dev->rx = Dev->tx + Dev->txCount;

  for( i=0; i<Dev->txCount; i++ ) DescInit( Dev->tx + i, i==Dev->txCount-1 );
  for( i=0; i<Dev->rxCount; i++ ) DescInit( Dev->rx + i, i==Dev->rxCount-1 );

  Dev->txNext = 0;
  Dev->txBusy = 0;
  Dev->rxNext = 0;
  Dev->rxBusy = 0;

  GMACWriteDmaReg( Dev, DmaBusMode, DmaBusModeInit );   /* Set init register values */
  GMACWriteDmaReg( Dev, DmaRxBaseAddr, (u32)Dev->rx );
  GMACWriteDmaReg( Dev, DmaTxBaseAddr, (u32)Dev->tx );
  if( Dev->phydev->speed == SPEED_1000 ) GMACWriteDmaReg( Dev, DmaControl, DmaControlInit1000);
  else if( Dev->phydev->speed == SPEED_100 ) GMACWriteDmaReg( Dev, DmaControl, DmaControlInit100 );
  else GMACWriteDmaReg( Dev, DmaControl, DmaControlInit10 );
  GMACWriteDmaReg( Dev, DmaInterrupt, DmaIntDisable );

  return 0;
}

int GMACDmaRxGet( TGMAC *Dev, u32 *Status, u32 *Buffer1, u32 *Length1)
{
  int desc = Dev->rxBusy;
  TDmaDesc *rx = Dev->rx + desc;
  if( DescDma(rx) ) { return -1; }  /* descriptor is owned by DMA - cannot get */
  if( DescEmpty(rx) ) { return -1; }        /* descriptor is empty - cannot get */

  //if Status/Length1/Buffer1 are not NULL poiners, the update them.
  if( Status != 0 ) *Status = rx->status;
  if( Length1 != 0 ) *Length1 = (rx->length & DescSize1Mask) >> DescSize1Shift;
  if( Buffer1 != 0 ) *Buffer1 = rx->buffer1;

  DescInit( rx, DescLast(rx) );
  Dev->rxBusy = DescLast(rx) ? 0 : desc + 1;

  return desc;
}

int GMACDmaTxSet( TGMAC *Dev, u32 Buffer1, u32 Length1)
{
    int desc = Dev->txNext;
    TDmaDesc *tx = Dev->tx + desc;

    tx->length &= DescEndOfRing;      /* clear everything */
    tx->length |= ((Length1 << DescSize1Shift) & DescSize1Mask) | DescTxFirst | DescTxLast | DescTxIntEnable;

    tx->buffer1 = Buffer1;
    tx->status  = DescOwnByDma;
    Dev->txNext = DescLast(tx) ? 0 : desc + 1;

    return desc;
}

int GMACDmaTxGet( TGMAC *Dev, u32 *Status, u32 *Buffer1, u32 *Length1)
{
  int desc = Dev->txBusy;
  TDmaDesc *tx = Dev->tx + desc;
  if( DescDma(tx) ) return -1;          /* descriptor is owned by DMA - cannot get */
  if( DescEmpty(tx) ) return -1;        /* descriptor is empty - cannot get */

  if( Status != 0 ) *Status = tx->status;
  if( Buffer1 != 0 ) *Buffer1 = tx->buffer1;
  if( Length1 != 0 ) *Length1 = (tx->length & DescSize1Mask) >> DescSize1Shift;

  DescInit( tx, DescLast(tx) );//re-init the descriptor
  Dev->txBusy = DescLast(tx) ? 0 : desc + 1;

  return desc;
}


void GMACDmaRxStart( TGMAC *Dev )
{
  GMACSetDmaReg( Dev, DmaControl, DmaRxStart );
}

void GMACDmaRxStop( TGMAC *Dev )
{
  int i;
  GMACClearDmaReg( Dev, DmaControl, DmaRxStart );

  /* Receive the remaing package in rx-buf before REAL stop..*/
  for( i=0; i<Dev->rxCount; i++ ) DescTake( Dev->rx + i );
}

void GMACDmaTxStart( TGMAC *Dev )
{
  GMACSetDmaReg( Dev, DmaControl, DmaTxStart );
}

void GMACDmaTxStop( TGMAC *Dev )
{
  int i;
  GMACClearDmaReg( Dev, DmaControl, DmaTxStart );
  for( i=0; i<Dev->txCount; i++ ) DescTake( Dev->tx + i );
}

void GMACDmaTxResume( TGMAC *Dev )
{
  GMACWriteDmaReg( Dev, DmaTxPollDemand, 0 );
}

int GMACDmaRxValid( u32 Status )
{
  return ( (Status & DescError) == 0 )      /* no errors, whole frame is in the buffer */
      && ( (Status & DescRxFirst) != 0 )
      && ( (Status & DescRxLast) != 0 );
}

u32 GMACDmaRxLength( u32 Status )
{
  return (Status & DescFrameLengthMask) >> DescFrameLengthShift;
}

/*
 * Assign Interface. Overwrite by steps
 * 1. enviroment variable rgmii=0/1 or phy_mode=gmii, rgmii, mii (NEW)
 * 2. HW setting (V1 only)
 * 3. Chip limitation (M325, M330, M330s are support MII only)
 */
static phy_interface_t gmac_get_phy_mode(void)
{
	u32 chip_version = v_inl(SYSC_CHIP_VERSION);
	/* Default GMII */
	phy_interface_t	interface = PHY_INTERFACE_MODE_GMII;
	phy_interface_t mode;
	char *phy_mode_env;

	/* Enviroment variable: backward compatible for rgmii=0/1 */
	phy_mode_env = getenv("rgmii");
	if(phy_mode_env != NULL)
		if (simple_strtoul (phy_mode_env, NULL, 10))
			interface = PHY_INTERFACE_MODE_RGMII;

	/* Enviroment variable: New environment variable "phy_mode" */
	phy_mode_env = getenv("phy_mode");
	for (mode=PHY_INTERFACE_MODE_MII; mode< PHY_INTERFACE_MODE_NONE; mode++) {
		if (strcmp(phy_mode_env, phy_interface_strings[mode]) == 0) {
			interface = mode;
			break;
		}
	}

	/*
	 * HW setting: Mozart V1 can get HW 2.5V or 3.3V from sysc
	 * for other modle this bit is result of SW control at 0x48
	 */
	if (chip_version == ASC88XX_M1_VERSION) {
		if (v_inl(EVM_SYSC_BASE + SYSC_MMR_SYS_INFO) & 0x4) {
			if (interface != PHY_INTERFACE_MODE_RGMII)
			{
				interface = PHY_INTERFACE_MODE_RGMII;
				setenv("phy_mode", (char *)phy_interface_strings[PHY_INTERFACE_MODE_RGMII]);
			}
		}
	}

	/* Chip limitation: For M325, M330 and M330s, only support MII interface */
	if((v_inl(SYSCTRL_PROT_INFO) & 0x7) == ASC8848M2_NUM) {
		if (interface != PHY_INTERFACE_MODE_MII)
		{
			interface = PHY_INTERFACE_MODE_MII;
			setenv("phy_mode", (char *)phy_interface_strings[PHY_INTERFACE_MODE_MII]);
		}
	}
	return interface;
}

static void gmac_phy_init(TGMAC *gc)
{
	struct eth_device *fake_dev;
	struct phy_device *phydev;
	phy_interface_t interface = gc->interface;

	fake_dev = (struct eth_device *)malloc(sizeof(struct eth_device)); /*TODO: free*/
	sprintf(fake_dev->name, "GMAC");

	/* phy_connect implicate phy_reset */
	phydev = phy_connect(gc->mdio_bus, gc->phyAddr, fake_dev, gc->interface);

	if (phydev) {
		/*
		 * By default GMII is used, but also work for MII PHY,
		 * auto fix environment variable if PHY driver told it doesn't support 1000baseT.
		 */
		if (interface == PHY_INTERFACE_MODE_GMII) {
			if (!((phydev->drv->features & SUPPORTED_1000baseT_Half) ||
			      (phydev->drv->features & SUPPORTED_1000baseT_Full))) {
				interface = gc->interface = phydev->interface = PHY_INTERFACE_MODE_MII;
				setenv("phy_mode", (char *)phy_interface_strings[PHY_INTERFACE_MODE_MII]);
				saveenv();
			}
		}
		gc->phydev = phydev;
		/*
		 * Stop Advertising 1000BASE Capability if interface is not GMII
		 * Useful in case PHY supports GMII but used in MII only,
		 * and Hardware miss to limit poweron advertising capability.
		 * Can fix it by setting phy_mode = "mii"
		 */
		if ((interface == PHY_INTERFACE_MODE_MII) ||
				(interface == PHY_INTERFACE_MODE_RMII))
			phydev->advertising &= ~(SUPPORTED_1000baseT_Half |
					SUPPORTED_1000baseT_Full);

		/* phy_config calls driver config, generally include change advert and auto-neg */
		phy_config(phydev);
	}
}

static inline void set_uart_dtr(void)
{
	/* 1'b0: DTR to high */
	u32 reg;
	reg = v_inl(UART_0 + SERIAL_MCR);
	reg &=~(SERIAL_MCR_DTR);
	v_outl(UART_0 + SERIAL_MCR, reg);
}

static inline void clear_uart_dtr(void)
{
	/* 1'b1: DTR to low */
	u32 reg;
	reg = v_inl(UART_0 + SERIAL_MCR);
	reg |= SERIAL_MCR_DTR;
	v_outl(UART_0 + SERIAL_MCR, reg);
}

static void gmac_mac_mode_init_v1(TGMAC *gc)
{
	/*
	 * To set GMAC work at correct interface mode.
	 * The following settings must be confirmed.
	 *
	 * 1. Set UART0 SERIAL_MCR_DTR
	 *    In Mozart V1, UART0 DTR ping is used to control Tx Clock direction
	 *    clear_uart_dtr(): Internal Tx Clk MAC->PHY
	 *    set_uart_dtr(): External Tx Clk MAC<-PHY
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
	unsigned long sysc_reg_clk_gen;
	unsigned long monitor_clk;
	switch (gc->interface) {
		case PHY_INTERFACE_MODE_MII:
		case PHY_INTERFACE_MODE_GMII:
			{
				/* Internal Tx clock */
				clear_uart_dtr();

				/* 125MHz GMII */
				sysc_reg_clk_gen =  v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
				sysc_reg_clk_gen &= ~(0x3 << 30);
				sysc_reg_clk_gen |= (0x1 << 30);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, sysc_reg_clk_gen);
			}
			break;
		case PHY_INTERFACE_MODE_RGMII:
			{
				/* Internal Tx clock */
				clear_uart_dtr();

				/* 125MHz RGMII */
				sysc_reg_clk_gen =  v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
				sysc_reg_clk_gen &= ~(0x3 << 30);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, sysc_reg_clk_gen);
				/* Workaround Tx Clock out */
				monitor_clk = v_inl(EVM_SYSC_BASE + SYSC_MMR_MON_CLK_SEL);

				monitor_clk &=~0x3F00;
				monitor_clk |= 0x0200;
				v_outl(EVM_SYSC_BASE + SYSC_MMR_MON_CLK_SEL, monitor_clk);
			}
			break;
		default:
			break;
	}
}

static void gmac_fix_mac_speed_v1(struct phy_device *phydev)
{
	u32 reg;
	if (phydev->interface == PHY_INTERFACE_MODE_RGMII) {
		clear_uart_dtr();
		reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
		reg &= ~(0x3 << 30);
		switch (phydev->speed) {
			case SPEED_1000:
				/* Provide 125MHz TX clock */
				reg |= (0x0 << 30);
				break;
			case SPEED_100:
				/* Provide 25MHz TX clock */
				reg |= (0x2 << 30);
				break;
			case SPEED_10:
				/* Provide 2.5MHz TX clock */
				reg |= (0x3 << 30);
				break;
		}
		v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, reg);
	} else {
		switch (phydev->speed) {
			case SPEED_1000:
				/* Provide 125MHz TX clock */
				clear_uart_dtr();
				reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
				reg &= ~(0x3 << 30);
				reg |= (0x1 << 30);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, reg);
				break;
			case SPEED_100:
				/* TX CLK is external clock source, don't care speed */
				set_uart_dtr();
				break;
			case SPEED_10:
				/* TX CLK is external clock source, don't care speed */
				set_uart_dtr();
				break;
		}
	}
}

static void gmac_mac_mode_init(TGMAC *gc)
{
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
	unsigned long sysc_reg_jtag_if_sel, sysc_reg_clk_gen;

	switch (gc->interface) {
		case PHY_INTERFACE_MODE_MII:
		case PHY_INTERFACE_MODE_GMII:
			{
				/* 3.3V & Internal Tx clock*/
				sysc_reg_jtag_if_sel = v_inl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL);
				sysc_reg_jtag_if_sel &= ~(0x1 << 2);
				sysc_reg_jtag_if_sel &= ~(0x1 << 1);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL, sysc_reg_jtag_if_sel);

				/* 125MHz, NOT Reduce Mode */
				sysc_reg_clk_gen =  v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
				sysc_reg_clk_gen &= ~(0x3 << 30);
				sysc_reg_clk_gen &= ~(0x1 << 23);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, sysc_reg_clk_gen);
			}
			break;
		case PHY_INTERFACE_MODE_RGMII:
			{
				/* 2.5V, Internal Tx clock*/
				sysc_reg_jtag_if_sel = v_inl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL);
				sysc_reg_jtag_if_sel &= ~(0x1 << 1);
				sysc_reg_jtag_if_sel |= (0x1 << 2);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL, sysc_reg_jtag_if_sel);

				/* 125MHz, Reduce Mode */
				sysc_reg_clk_gen =  v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
				sysc_reg_clk_gen &= ~(0x3 << 30);
				sysc_reg_clk_gen |= (0x1 << 23);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, sysc_reg_clk_gen);
			}
			break;
		default:
			break;
	}
}

static void gmac_fix_mac_speed(struct phy_device *phydev)
{
	u32 reg;
	if (phydev->interface == PHY_INTERFACE_MODE_RGMII) {
		reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
		reg &= ~(0x3 << 30);
		switch (phydev->speed) {
			case SPEED_1000:
				/* Provide 125MHz TX clock */
				reg |= (0x0 << 30);
				break;
			case SPEED_100:
				/* Provide 25MHz TX clock */
				reg |= (0x1 << 30);
				break;
			case SPEED_10:
				/* Provide 2.5MHz TX clock */
				reg |= (0x2 << 30);
				break;
		}
		v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, reg);
	} else {
		switch (phydev->speed) {
			case SPEED_1000:
				/* Provide 125MHz TX clock */
				reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL);
				reg &= ~(0x1<< 1);
				reg |= (0x0<< 1);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL, reg);

				reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN);
				reg &= ~(0x3 << 30);
				reg |= (0x0 << 30);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, reg);
				break;
			case SPEED_100:
				/* TX CLK is external clock source, don't care speed */
				reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL);
				reg &= ~(0x1<< 1);
				reg |= (0x1<< 1);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL, reg);
				break;
			case SPEED_10:
				/* TX CLK is external clock source, don't care speed */
				reg = v_inl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL);
				reg &= ~(0x1<< 1);
				reg |= (0x1<< 1);
				v_outl(EVM_SYSC_BASE + SYSC_MMR_IF_CTRL, reg);
				break;
		}
	}
}

void GMACInit(int quiet)
{
	TPrivate *pr = &gmac_priv;
	TGMAC *gc;
	u32 chip_version;
	struct phy_device *phydev;
	int rs_timeout = 0;
	gc = &(pr->gmac);
	phydev = gc->phydev;
	chip_version = v_inl(SYSC_CHIP_VERSION);

	/* Print status */
	if(!quiet) printf("  PHY TYPE     : %s(%s)\n", phydev->drv->name, phy_interface_strings[gc->interface]);

	if (gmac_quick_testspeed == -1) { /* Normal auto-nego */
		if (NET_FIX_SPEED == 0) { /* audo nego */
			phydev->autoneg = AUTONEG_ENABLE;
			phydev->speed = 0;
			phydev->duplex = -1;
		} else { /* fixed speed */
			phydev->autoneg = AUTONEG_DISABLE;
			phydev->speed = SPEED_100;
			phydev->duplex = DUPLEX_FULL;
		}
		phydev->link = 1;
		phy_config(phydev);

		/* Get link status */
		if (!quiet) printf("  Link Detect  : ");
		if (NET_FIX_SPEED == 0) { /* audo nego */
			phy_startup(phydev);
		} else {
			/* Do a fake read, or fix speed config will not be applied */
			phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
			printf("%s Fixed speed mode\n",
				phydev->dev->name);
		}
	}

	if(!quiet) printf("  Link Status  : %s\n", (phydev->link)?"Link Up":"Link Down");

	if(phydev->link == 0) {
		if(!quiet) printf("                 - Cannot detect link connected signal in the limited time...\n");
		if(!quiet) printf("                 - Assume it works in 100Mbps + Full-duplex.\n");
		if (gmac_quick_testspeed == -1) {
			phydev->autoneg = AUTONEG_DISABLE;
			phydev->speed = SPEED_100;
			phydev->duplex = DUPLEX_FULL;
			phy_config(phydev);
			/* Do a fake read, or fix speed config will not be applied */
			phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
		}
	} else {
		if(!quiet) printf("%s", (phydev->duplex == DUPLEX_FULL) ? "               : Full Duplex\n" : "               : Half-Duplex\n");
		switch(phydev->speed) {
			case SPEED_1000:
				if(!quiet) printf("  Link Speed   : 1000Mbps\n");
				break;
			case SPEED_100:
				if(!quiet) printf("  Link Speed   : 100Mbps\n");
				break;
			case SPEED_10:
				if(!quiet) printf("  Link Speed   : 10Mbps\n");
				break;
			default:
				if(!quiet) printf("  Link Speed   : Unknown\n");
				break;
		}
	}
	//-------------------------------
	// setup_net_DMA
	//-------------------------------
	/* Setup GMAC to work at PHY interface mode */
	if (chip_version == ASC88XX_M1_VERSION)
		gmac_mac_mode_init_v1(gc);
	else
		gmac_mac_mode_init(gc);

	/* Before reset, must do gmac_mac_mode_init. */
	GMACWriteDmaReg( gc, DmaBusMode, DmaResetOn);
	while(GMACReadDmaReg( gc, DmaBusMode) & 0x1) {
		if (rs_timeout > 10000) {
			printf("[GMAC] Fatal Error: Reset GMAC timeout, not complete over 1sec\n");
			break;
		}
		rs_timeout++;
		udelay(100);
	};

	if( GMACDmaInit( gc, pr->desc, sizeof(TDmaDesc)*DESC_COUNT ) != 0 )        /* Init Tipster DMA Engine */
	{
		printf( "GMAC - cannot init DMA\n" );
		return;
	}

	/* Set Tx Clock direction, speed accroding to PHY link */
	if (chip_version == ASC88XX_M1_VERSION)
		gmac_fix_mac_speed_v1(gc->phydev);
	else
		gmac_fix_mac_speed(gc->phydev);

	//-------------------------------
	// setup_net_GMAC
	//-------------------------------
	if( GMACMacInit ( gc, MAC_Addr, quiet) != 0 )
	{
		printf( "GMAC - cannot init MAC\n" );
		return;
	}
}

int eth_init(bd_t* bd)
{
	TGMAC *gc = &(gmac_priv.gmac);
	int r;

	eth_getenv_enetaddr("ethaddr", MAC_Addr);
	gmac_set_ethaddr(gc, MAC_Addr) ;

	//Because rx func of GMAC is PASSIVE, as long as we start the "RECEIVER", it can receive the packet any time.
	//So we should prepare for receiving!!
	do
	{
		r = GMACDmaRxSet(gc, (u32)&(gmac_priv.rx_buff_list[gc->rxNext]), DMA_BUFFER_SIZE);
	} while (r >= 0);

	//Start DMA
	GMACDmaRxStart(gc);     /* start receiver */
	GMACDmaTxStart(gc);     /* start transmitter, it must go to suspend immediately */

	return 0;
}

void eth_halt(void)
{
	int r;
	int desc_index ;
	TGMAC *gc = &(gmac_priv.gmac);

	GMACDmaRxStop(gc);
	GMACDmaTxStop(gc);

	do /* free recieve descriptors */
	{
		r = GMACDmaRxGet( gc, NULL, NULL, NULL);   /* clear rx descriptor content */
	} while ( r >= 0 );

	do /* free transmit descriptors */
	{
		r = GMACDmaTxGet( gc, NULL, NULL, NULL);   /* clear tx descriptor content */
	} while ( r >= 0 );

	//wait until dmac REALLY stop.
	while((v_inl(GMAC_MMR_BASE + 0x1014) & (0x7 << 17)) != 0x0) ;

	//we do not have to receive the packets after stopping,
	//but we should rewind the rxBusy/rxNext
	TDmaDesc *rx = (TDmaDesc *)v_inl( GMAC_MMR_BASE + 0x104c ) ;
	if(rx != NULL)
		desc_index = ((unsigned long)rx - (unsigned long)gc->rx) / sizeof(TDmaDesc) ;
	else
		desc_index = 0 ;

	gc->rxBusy = desc_index ;
	gc->rxNext = desc_index ;
}

//In eth_rx(), we will receive more and more packets as possible.
int eth_rx(void)
{
    int r = 0;
    TGMAC *gc = &(gmac_priv.gmac);

    do
    {
        u32 buffer1 = 0;
        u32 status = 0;

        //1. Get "status" & "buffer1" fields updated by receiver.
        r = GMACDmaRxGet( gc, &status, &buffer1, NULL);   /* get rx descriptor content */

        //2. if we really receive something..
        if (r >= 0 && buffer1 != 0)
        {
			/*int rx_index ;
			for(rx_index = 0 ; rx_index < 64 ; rx_index++) {
				if(rx_index % 4 == 0)
					printf("\n") ;
				printf("%02x ", *(((u32 *)buffer1) + rx_index)) ;
			}*/
            //3. and if the packet is ok...
            if (GMACDmaRxValid(status))
            {
                int len;
                len = GMACDmaRxLength( status ) - 4;
                NetReceive((u8 *)buffer1, len);//send the recieved packet to the upper layer.
            }

            //4. Because GMACDmaRxGet() will call DescInit() to reset all fields of descriptor,
            //we should call GMACDmaRxSet() to set descriptor's fields again.
            GMACDmaRxSet(gc, (u32)buffer1, DMA_BUFFER_SIZE);
        }

    } while (r >= 0);

    return 0;
}

//We send one packet every eth_send()
int eth_send(volatile void *packet, int length)
{
    TGMAC *gc = &(gmac_priv.gmac);
    TDmaDesc pdesc;
    int des;

    //1. Get available descriptor index
    des = gc->txNext;
    //2. Get available descriptor
    pdesc = gmac_priv.desc[gc->txNext];
    //3. Copy from packet to tx_buff_list
    memcpy((void *)&gmac_priv.tx_buff_list[gc->txNext], (const void *)packet, length);
    //4. Setup tx descriptors
    GMACDmaTxSet( gc, (u32)&(gmac_priv.tx_buff_list[gc->txNext]), length);
    //5. Start the TxEngine of GMAC
    GMACDmaTxResume(gc);
    //6. Wait for finishing the transmission
    while(1)
    {
        if (pdesc.status != DescOwnByDma)
        {
            break;
        }
    }
    return length;
}

TPrivate* gmac_returnTPrivate(void)
{
	return &gmac_priv ;
}

struct eth_device Mozart_netdev ;

void gmac_set_ethaddr(TGMAC *dev, u8 *Addr)
{
	unsigned long data ;

	data = (Addr[5]<<8) | Addr[4];                            /* set our MAC address */
	GMACWriteMacReg( dev, GmacAddr0High, data );
	data = (Addr[3]<<24) | (Addr[2]<<16) | (Addr[1]<<8) | Addr[0];
	GMACWriteMacReg( dev, GmacAddr0Low, data );
}

static void voc_pclk_25Mhz(void)
{
#define PLLC_CTRL_3		0x1c
#define PLLC_RATIO_3		0x20
#define SYSC_CLK_GEN_CFG	0x2c
#define SYSC_CLK_EN_CTRL_1	0x28
#define SYSC_PAD_EN_CTRL 	0x44
#define VOC_CTRL		0x4
#define VOC_OUT_SIZE            0x28
	u32 ratio = 0x02030031;
	u32 dwDivider = 0x6;
	u32 pllc_base = VPL_PLLC_MMR_BASE;
	u32 sysc_base = VPL_SYSC_MMR_BASE;
	u32 voc_base = VPL_VOC_MMR_BASE;
	u32 reg;
	/* Set VOC divider by SYSC */
	reg = v_inl(sysc_base+SYSC_CLK_GEN_CFG);
	reg &=~(0x3F << 16);
	reg |= (dwDivider << 16);

	reg &=~(0x1 << 22);
	//reg |= (0x1 << 22);
	v_outl(sysc_base+SYSC_CLK_GEN_CFG, reg);

	/* Set PLL3 to target ratio */
	v_outl(pllc_base + PLLC_CTRL_3, 0x00000009);   /* Enable Adjustment, Disable clock out */
	v_outl(pllc_base + PLLC_RATIO_3, ratio);
	v_outl(pllc_base + PLLC_CTRL_3, 0x00000001); /* Disable Adjustment */

	/*Wait for PLL lock*/
	while ((v_inl(pllc_base + PLLC_CTRL_3) & 0x4) != 0x4)
	{
		//printf("Waiting PLL lock....\n");
		udelay(10);
	}

	/*Start to output the clock of PLL*/
	v_outl(pllc_base + PLLC_CTRL_3, 0x00000003); /* Enable clock output */

	/* Enable VOC CLK */
	reg = v_inl(sysc_base + SYSC_CLK_EN_CTRL_1);
	reg |= (0x1 << 9);
	v_outl(sysc_base + SYSC_CLK_EN_CTRL_1, reg);

	/* Enable VOC PAD */
	reg = v_inl(sysc_base + SYSC_PAD_EN_CTRL);
	reg |= (0x1 << 24);
	v_outl(sysc_base + SYSC_PAD_EN_CTRL, reg);

	/* Let VOC idle, minimize bus access time */
	v_outl(voc_base + VOC_OUT_SIZE, 0xffff0000);

	/* Enable VOC */
	reg = v_inl(voc_base + VOC_CTRL);
	reg |= (0x1<<2);
	v_outl(voc_base + VOC_CTRL, reg);
}

static void monclk1(void)
{
#define SYSC_CLK_EN_CTRL_0	0x24
#define SYSC_MON_CLK_SEL	0x30
	/* Use mon1 on SDHC1 as PHY 25MHz reference clock */
	u32 sysc_base = VPL_SYSC_MMR_BASE;
	volatile u32 reg;
	reg = v_inl(sysc_base + SYSC_MON_CLK_SEL);
	reg &= ~0x3f00;
	reg |= (0x15<<8);
	v_outl(sysc_base + SYSC_MON_CLK_SEL, reg);

	reg = v_inl(sysc_base + SYSC_CLK_EN_CTRL_0);
	reg |= (0x1<<27);
	v_outl(sysc_base + SYSC_CLK_EN_CTRL_0, reg);
}

int gmac_eth_initialize(int quiet)
{
	TPrivate *pr = &gmac_priv;
	TGMAC *gc;
	char *phyclk_src;

	gc = &(pr->gmac);

	eth_getenv_enetaddr("ethaddr", MAC_Addr);

	/*Oops...transform board....*/
	phyclk_src = getenv("phy_clksrc");

	if (strcmp(phyclk_src, "voc") == 0) {
		voc_pclk_25Mhz();
	}

	if (strcmp(phyclk_src, "mon1") == 0) {
		monclk1();
	}

	/* register phy libs */
	phy_init();

	/* Get GMAC and PHY interface mode */
	gc->interface = gmac_get_phy_mode();

	/* DMA Descriptor allocate */
	pr->desc = (TDmaDesc *)DMA_BUFFER_BASE;
	pr->tx_desc_base = DMA_BUFFER_BASE;
	pr->rx_desc_base = DMA_BUFFER_BASE + (sizeof(TDmaDesc)*TX_DESC_COUNT);
	pr->tx_buff_list = (TDmaBuff *)(DMA_BUFFER_BASE + (sizeof(TDmaDesc)*DESC_COUNT));
	pr->rx_buff_list = (TDmaBuff *)(DMA_BUFFER_BASE + (sizeof(TDmaDesc)*DESC_COUNT) + (DMA_BUFFER_SIZE*TX_DESC_COUNT));

	gc->macBase = GMAC_MMR_BASE;
	gc->dmaBase = GMAC_MMR_BASE + 0x1000;
	gc->phyAddr = NET_PHY_ADDRESS;

	/* Initialize the MDIO bus */
	gmac_mdio_init(gc);

	/* Connect PHY and config auto-nego */
	gmac_phy_init(gc);

	GMACInit(quiet);

	return 0;
}
