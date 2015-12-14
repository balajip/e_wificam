#include <common.h>
#include <malloc.h>
#include <net.h>
#include <netdev.h>

#include <asm/arch/platform.h>
#include "gmac.h"

#if !defined(CONFIG_MII)
# error "GMAC requires MII -- missing CONFIG_MII"
#endif

#if !defined(CONFIG_PHYLIB)
# error "GMAC requires PHYLIB -- missing CONFIG_PHYLIB"
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

static inline void TxDescInit( TDmaDesc *Desc, bool EndOfRing )
{
	Desc->status = EndOfRing ? DescTxEndOfRing : 0;
	Desc->length = 0;
	Desc->buffer1 = 0;
	Desc->buffer2 = 0;
	Desc->desc4 = 0;
	Desc->desc5 = 0;
	Desc->desc6 = 0;
	Desc->desc7 = 0;
}

static inline void RxDescInit( TDmaDesc *Desc, bool EndOfRing )
{
	Desc->status = 0;
	Desc->length = EndOfRing ? DescRxEndOfRing : 0;
	Desc->buffer1 = 0;
	Desc->buffer2 = 0;
	Desc->desc4 = 0;
	Desc->desc5 = 0;
	Desc->desc6 = 0;
	Desc->desc7 = 0;
}

static inline bool TxDescLast( TDmaDesc *Desc )
{
	return (Desc->status & DescTxEndOfRing) != 0;
}

static inline bool RxDescLast( TDmaDesc *Desc )
{
	return (Desc->length & DescRxEndOfRing) != 0;
}

static inline bool DescEmpty( TDmaDesc *Desc )
{
	return (Desc->length & (DescTxSize1Mask | DescTxSize2Mask)) == 0;
}

static inline bool DescDma( TDmaDesc *Desc )
{
	return (Desc->status & DescOwnByDma) != 0;   /* Owned by DMA */
}

static inline void DescTake( TDmaDesc *Desc )   /* Take ownership */
{
	if( DescDma(Desc) )               /* if descriptor is owned by DMA engine */
	{
	Desc->status &= ~DescOwnByDma;    /* clear DMA own bit */
	Desc->status |= DescError;        /* Set error bit to mark this descriptor bad */
	}
}

int GMACMacInit( TGMAC *dev, u8 Addr[6], int quiet)
{
	struct phy_device *phydev =  dev->phydev;

	if(!quiet) printf("  MAC Address  : %02x %02x %02x %02x %02x %02x\n", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
	gmac_set_ethaddr(dev, Addr) ;

	if (phydev->duplex) {
		/* Added by Lokesh */
		if(phydev->speed == SPEED_1000) {
			if(dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx1000 | (0x1 << 24) | (0x1 << 8)); /* set init values of config registers */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx1000 ); /* set init values of config registers */
		} else if(phydev->speed == SPEED_100) {
			if(dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx100 | (0x1 << 24) | (0x1 << 8)); /* set init values of config registers */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx100 ); /* set init values of config registers with MII port */
		} else if(phydev->speed == SPEED_10) {
			if(dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx10 | (0x1 << 24) | (0x1 << 8)); /* set init values of config registers with MII port */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitFdx10 ); /* set init values of config registers with MII port */
		}

		GMACWriteMacReg( dev, GmacFrameFilter, GmacFrameFilterInitFdx );
		GMACWriteMacReg( dev, GmacFlowControl, GmacFlowControlInitFdx );
	} else {
		/* Modified by Lokesh */
		if(phydev->speed == SPEED_1000) {
			if(dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitHdx1000 | (0x1 << 24) | (0x1 << 8)); /* set init values of config registers */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitHdx1000 ); /* set init values of config registers */
		} else {
			if(dev->interface == PHY_INTERFACE_MODE_RGMII)
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitHdx110 | (0x1 << 24) | (0x1 << 8)); /* set init values of config registers */
			else
				GMACWriteMacReg( dev, GmacConfig, GmacConfigInitHdx110 ); /* set init values of config registers with MII port */
		}

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

	rx->length &= DescRxEndOfRing;      /* clear everything */
	rx->length |= ((Length1 << DescRxSize1Shift) & DescRxSize1Mask);
	rx->buffer1 = Buffer1;
	rx->status  = DescOwnByDma;//[trace]this is the most important part of GMACDmaRxSet()
	Dev->rxNext = RxDescLast(rx) ? 0 : desc + 1;

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

	for( i=0; i<Dev->txCount; i++ ) TxDescInit( Dev->tx + i, i==Dev->txCount-1 );
	for( i=0; i<Dev->rxCount; i++ ) RxDescInit( Dev->rx + i, i==Dev->rxCount-1 );

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
	if( DescDma(rx) ) {return -1;}          /* descriptor is owned by DMA - cannot get */
	if( DescEmpty(rx) ) {return -2;}        /* descriptor is empty - cannot get */

	if( Status != 0 ) *Status = rx->status;
	if( Length1 != 0 ) *Length1 = (rx->length & DescRxSize1Mask) >> DescRxSize1Shift;
	if( Buffer1 != 0 ) *Buffer1 = rx->buffer1;

	RxDescInit( rx, RxDescLast(rx) );
	Dev->rxBusy = RxDescLast(rx) ? 0 : desc + 1;

	return desc;
}

int GMACDmaTxSet( TGMAC *Dev, u32 Buffer1, u32 Length1)
{
	int desc = Dev->txNext;
	TDmaDesc *tx = Dev->tx + desc;

	tx->length |= ((Length1 << DescTxSize1Shift) & DescTxSize1Mask);

	tx->buffer1 = Buffer1;
	tx->status |= DescOwnByDma | DescTxFirst | DescTxLast;
	Dev->txNext = TxDescLast(tx) ? 0 : desc + 1;

	//debug("S[%x] st %08x, l %08x, b %08x\n", (u32)tx, (u32)tx->status, (u32)tx->length, (u32)tx->buffer1);

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
	if( Length1 != 0 ) *Length1 = (tx->length & DescTxSize1Mask) >> DescTxSize1Shift;

	TxDescInit( tx, TxDescLast(tx) );
	Dev->txBusy = TxDescLast(tx) ? 0 : desc + 1;

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

inline int GMACDmaRxValid( u32 Status )
{
	return ( (Status & DescError) == 0 )      /* no errors, whole frame is in the buffer */
		&& ( (Status & DescRxFirst) != 0 )
		&& ( (Status & DescRxLast) != 0 );
}

inline u32 GMACDmaRxLength(u32 Status)
{
	return (Status & DescFrameLengthMask) >> DescFrameLengthShift;
}

/*
 * Assign Interface. Overwrite by steps
 * 1. enviroment variable rgmii=0/1 or phy_mode=gmii, rgmii, mii, rmii
 * 2. HW setting (V1 only)
 * 3. Chip limitation (M325, M330, M330s are support MII only)
 */
static phy_interface_t gmac_get_phy_mode(void)
{
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

#define TXCLK_DIR_EXTERNAL 1
#define TXCLK_DIR_INTERNAL 0
#define GMAC_PLL_REF_25MHZ 1
#define GMAC_PLL_REF_24MHZ 0
void config_sysc_gmac_clk(int gmac_index, int reducemode_enable, int speed_sel, int txclk_dir, int pll_ref)
{
	unsigned long gmac_clk_ctrl = v_inl(EVM_SYSC_BASE + SYSC_MMR_GMAC_CLK_CFG_CTRL);

	//clear the related bits.
	gmac_clk_ctrl &= ~(0xFFFF << (16 * gmac_index));

	gmac_clk_ctrl |= (reducemode_enable << 12) | (speed_sel << 8) | (txclk_dir << 4) | (pll_ref << 0);
	v_outl(EVM_SYSC_BASE + SYSC_MMR_GMAC_CLK_CFG_CTRL, gmac_clk_ctrl);

	//printf("0x%x = 0x%x\n", EVM_SYSC_BASE + SYSCTRL_GMAC_CLK_CFG_CTRL, v_inl(EVM_SYSC_BASE + SYSCTRL_GMAC_CLK_CFG_CTRL)) ;
}

#define GMAC_PAD_PWR_LEVEL_2_5v	1
#define GMAC_PAD_PWR_LEVEL_3_3v	0
void config_sysc_gmac_voltage(int gmac_index, int voltage)
{
	unsigned long gmac_pwr_level = v_inl(EVM_SYSC_BASE + SYSC_MMR_PAD_VOLTAGE_LEVEL) ;

	//clear the related bits
	gmac_pwr_level &= ~(0x1 << gmac_index) ;

	gmac_pwr_level |= (voltage << gmac_index) ;
	v_outl(EVM_SYSC_BASE + SYSC_MMR_PAD_VOLTAGE_LEVEL, gmac_pwr_level) ;

	//printf("0x%x = 0x%x\n", EVM_SYSC_BASE + SYSCTRL_PAD_VOLTAGE_LEVEL, v_inl(EVM_SYSC_BASE + SYSCTRL_PAD_VOLTAGE_LEVEL)) ;
}

int sqrt(int value)
{
	int ans = 0 ;

	while(1) {
		if(value == 1)
			break ;
		value /= 2 ;
		ans++ ;
	}

	return ans ;
}

//F_pllout = F_ref * (M / N) * (1 / DIV)
void config_gmac_pll(int M, int N, int DIV)
{
	int DIV_FB = M - 1 ;
	int DIV_REF = N - 1 ;
	int DIV_OUT = sqrt(DIV) ;

	unsigned long pll1_ctrl ;
	unsigned long pll1_ratio = v_inl(PLL_RATIO_1) ;

	// 1. set PLL2 as 811.008MHz
	//disable PWR_ON and PLL_CLK_EN ; enable PLL_ADJUST_EN
	pll1_ctrl = 0x8 ;
	v_outl(PLL_CTRL_1, pll1_ctrl) ;
	//set DIV_FB
	pll1_ratio &= ~0xFF ;
	pll1_ratio |= DIV_FB & 0xFF ;
	//set DIV_REF
	pll1_ratio &= ~(0xFF << 8) ;
	pll1_ratio |= ((DIV_REF & 0xFF) << 8) ;
	//set DIV_OUT
	pll1_ratio &= ~(0x7 << 16) ;
	pll1_ratio |= (DIV_OUT << 16) ;
	v_outl(PLL_RATIO_1, pll1_ratio) ;
	//enable PWR_ON and wait until PLL_LOCK enable
	pll1_ctrl = v_inl(PLL_CTRL_1) ;
	pll1_ctrl |= 0x1 ;
	v_outl(PLL_CTRL_1, pll1_ctrl) ;
	while(1) {
		pll1_ctrl = v_inl(PLL_CTRL_1) ;
		if(pll1_ctrl & 0x4)
			break ;
	}
	//disable PLL_ADJUST_EN and enable PLL_CLK_EN
	pll1_ctrl = v_inl(PLL_CTRL_1) ;
	pll1_ctrl &= ~(1 << 3) ;
	pll1_ctrl |= (1 << 1) ;
	v_outl(PLL_CTRL_1, pll1_ctrl) ;

	//printf("PLL_CTRL_1 = 0x%x\n", v_inl(PLL_CTRL_1)) ;
	//printf("PLL_RATIO_1 = 0x%x\n", v_inl(PLL_RATIO_1)) ;
}

static void gmac_mac_mode_init(TGMAC *gc)
{
	volatile u32 reduce_mode, speed_sel, txclk_dir;
	unsigned long gmac_clk_ctrl = v_inl(EVM_SYSC_BASE + SYSC_MMR_GMAC_CLK_CFG_CTRL);

	switch (gc->interface) {
		case PHY_INTERFACE_MODE_MII:
		case PHY_INTERFACE_MODE_GMII:
		{
			reduce_mode = 0x0;
			speed_sel = 0x0;
			txclk_dir = 0x0;
			break;
		}
		case PHY_INTERFACE_MODE_RGMII:
		{
			reduce_mode = 0x1;
			speed_sel = 0x0;
			txclk_dir = 0x0;
			break;
		}
		case PHY_INTERFACE_MODE_RMII:
		{
			gmac_clk_ctrl |= 0x1 << 12;
			reduce_mode = 0x1;
			speed_sel = 0x1;
			txclk_dir = 0x0;
			break;
		}
		default:
			break;
	}
	//clear the related bits.
	gmac_clk_ctrl &= ~0xFFFF;
	gmac_clk_ctrl |= (reduce_mode << 12) | (speed_sel << 8) | (txclk_dir << 4);
	v_outl(EVM_SYSC_BASE + SYSC_MMR_GMAC_CLK_CFG_CTRL, gmac_clk_ctrl);

	//printf("0x%x = 0x%x\n", EVM_SYSC_BASE + SYSCTRL_GMAC_CLK_CFG_CTRL, v_inl(EVM_SYSC_BASE + SYSCTRL_GMAC_CLK_CFG_CTRL)) ;
}

static void gmac_fix_mac_speed(struct phy_device *phydev)
{
	volatile u32 reduce_mode, speed_sel, txclk_dir, pll_ref, pad_voltage, pad_en_ctrl;
	/*TODO: set pad_voltage according to PHY spec?
	 *      fix: set PLL source after config_gmac_pll is really tricky
	 * */
	if(phydev->interface == PHY_INTERFACE_MODE_RGMII) {
		pll_ref = GMAC_PLL_REF_25MHZ;
		txclk_dir = TXCLK_DIR_INTERNAL;
		reduce_mode = 1;
		speed_sel = 0;
		pad_voltage = GMAC_PAD_PWR_LEVEL_2_5v;

		if(phydev->speed == SPEED_1000)
			config_gmac_pll( 40, 1, 4);
		else if(phydev->speed == SPEED_100)
			config_gmac_pll( 2, 1, 1);
		else if(phydev->speed == SPEED_10)
			config_gmac_pll( 1, 5, 1);
	} else if (phydev->interface == PHY_INTERFACE_MODE_RMII) {
		pll_ref = GMAC_PLL_REF_24MHZ;
		txclk_dir = TXCLK_DIR_INTERNAL;
		reduce_mode = 1;
		speed_sel = 1;
		pad_voltage = GMAC_PAD_PWR_LEVEL_3_3v;
		v_outl(EVM_SYSC_BASE + 0xC0, 0x19); //MON_CLK = clk_gen_i_gmac_0_rmii_clk_p
		config_gmac_pll( 50, 1, 4);
	} else if (phydev->interface == PHY_INTERFACE_MODE_GMII) {
		/* Enable PAD*/
		pad_en_ctrl = v_inl(EVM_SYSC_BASE + 0x50);
		pad_en_ctrl &= (~0x7);
		pad_en_ctrl |= 0x3;
		v_outl(EVM_SYSC_BASE + 0x50, pad_en_ctrl);

		if (phydev->speed == SPEED_1000) {
			pll_ref = GMAC_PLL_REF_25MHZ;
			txclk_dir = TXCLK_DIR_INTERNAL;
			reduce_mode = 0;
			speed_sel = 0;
			config_gmac_pll( 40, 1, 4);
		} else {
			pll_ref = GMAC_PLL_REF_24MHZ;
			txclk_dir = TXCLK_DIR_EXTERNAL;
			reduce_mode = 0;

			config_gmac_pll( 50, 1, 4);
			if (phydev->speed == SPEED_100)
				speed_sel = 1;
			else
				speed_sel = 2;
		}
		pad_voltage = GMAC_PAD_PWR_LEVEL_3_3v;
	} else {
		pll_ref = GMAC_PLL_REF_24MHZ;
		txclk_dir = TXCLK_DIR_EXTERNAL;
		reduce_mode = 0;

		config_gmac_pll( 50, 1, 4);
		if (phydev->speed == SPEED_100)
			speed_sel = 1;
		else
			speed_sel = 2;
		pad_voltage = GMAC_PAD_PWR_LEVEL_3_3v;
	}

	//SYSC part.
	config_sysc_gmac_clk(0, reduce_mode, speed_sel, txclk_dir, pll_ref);
	config_sysc_gmac_voltage(0, pad_voltage);
}

void GMACInit(int quiet)
{
	TPrivate *pr = &gmac_priv;
	TGMAC *gc;
//	volatile u32 reduce_mode, speed_sel, txclk_dir, pll_ref, pad_voltage, pad_en_ctrl;
	struct phy_device *phydev;
	int rs_timeout = 0;
	gc = &(pr->gmac);
	phydev = gc->phydev;

	/* Print status */
	if(!quiet) printf("  PHY TYPE     : %s(%s)\n", phydev->drv->name, phy_interface_strings[gc->interface]);

	if (gmac_quick_testspeed == -1) { /* Normal auto-nego */
		/* audo nego */
		phydev->autoneg = AUTONEG_ENABLE;
		phydev->speed = 0;
		phydev->duplex = -1;
		phydev->link = 1;
		phy_config(phydev);

		/* Get link status */
		if (!quiet) printf("  Link Detect  : ");
		/* audo nego */
		phy_startup(phydev);
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

	gmac_mac_mode_init(gc);

	//To give GMAC clock, we should set gmac-reg#0 before reset.
	/*switch(phydev->speed) {
		case SPEED_1000:
			GMACWriteMacReg( gc, GmacConfig, 0x01200d0c);
			break;
		case SPEED_100:
			GMACWriteMacReg( gc, GmacConfig, 0x0120cd0c);
			break;
		case SPEED_10:
			GMACWriteMacReg( gc, GmacConfig, 0x01208d0c);
			break;
	} */

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
/*
	switch (GMACReadDmaReg( gc, 0x58)>>28) {
		case 0x0:
			printf("[GMAC] GMII interface mode\n");
			break;
		case 0x1:
			printf("[GMAC] RGMII interface mode\n");
			break;
		case 0x4:
			printf("[GMAC] RMII interface mode\n");
			break;
		default:
			printf("[GMAC] Fatal Error, unsupported interface\n");
			break;
	}
*/
	if( GMACDmaInit( gc, pr->desc, sizeof(TDmaDesc)*DESC_COUNT ) != 0 )        /* Init Tipster DMA Engine */
	{
		printf( "GMAC - cannot init DMA\n" );
		return;
	}

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
    static int counter = 0 ;
    TGMAC *gc = &(gmac_priv.gmac);

    GMACDmaRxStop(gc);
    GMACDmaTxStop(gc);

    TDmaDesc *rx = (TDmaDesc *)v_inl( GMAC_MMR_BASE + 0x104c ) ; ;
    int desc_index = ((unsigned long)rx - (unsigned long)gc->rx) / sizeof(TDmaDesc) ;

    do                          // free recieve descriptors
    {

        r = GMACDmaRxGet( gc, NULL, NULL, NULL);   /* clear rx descriptor content */
    } while ( r >= 0 );

    do                          /* free transmit descriptors */
    {

        r = GMACDmaTxGet( gc, NULL, NULL, NULL);   /* clear tx descriptor content */
    } while ( r >= 0 );

    if ( counter != 0 ) {//[jam] if this is first call, do not do this step.
        gc->rxBusy = desc_index ;
        gc->rxNext = desc_index ;
    }
    else {
        counter = 1 ;
    }
}

int eth_rx(void)
{
	int r = 0;
	TGMAC *gc = &(gmac_priv.gmac);

	do
	{
		u32 buffer1 = 0;
		u32 status = 0;

		r = GMACDmaRxGet( gc, &status, &buffer1, NULL);   /* get rx descriptor content */

		if (r >= 0 && buffer1 != 0)
		{
			if (GMACDmaRxValid(status))
			{
				int len;
				len = GMACDmaRxLength( status ) - 4;
				NetReceive((u8 *)buffer1, len);
			}
			else {
				printf("invalid frame 0x%08x\n", status);
			}
			GMACDmaRxSet(gc, (u32)buffer1, DMA_BUFFER_SIZE);
		}
	} while (r >= 0);

	return 0;
}

int eth_send(volatile void *packet, int length)
{
	TGMAC *gc = &(gmac_priv.gmac);
	int desc = gc->txNext;
	TDmaDesc *tx = gc->tx + desc;

	memcpy((void *)&gmac_priv.tx_buff_list[gc->txNext], (const void *)packet, length);
	GMACDmaTxSet( gc, (u32)&(gmac_priv.tx_buff_list[gc->txNext]), length);
	GMACDmaTxResume(gc);
	while(tx->status & DescOwnByDma)
	{
		udelay(5);
	}

	//printf("O[%x] st %08x, l %08x, b %08x\n", (u32)tx, (u32)tx->status, (u32)tx->length, (u32)tx->buffer1);

	return length;
}

TPrivate *gmac_returnTPrivate(void)
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

int gmac_eth_initialize(int quiet)
{
	TPrivate *pr = &gmac_priv;
	TGMAC *gc;
	gc = &(pr->gmac);

	eth_getenv_enetaddr("ethaddr", MAC_Addr);

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

	if (gc->interface == PHY_INTERFACE_MODE_RMII) {
		v_outl(EVM_SYSC_BASE + 0xC0, 0x19); //MON_CLK = clk_gen_i_gmac_0_rmii_clk_p
		config_gmac_pll( 50, 1, 4);
	}

	/* Initialize the MDIO bus */
	gmac_mdio_init(gc);

	/* Connect PHY and config auto-nego */
	gmac_phy_init(gc);

	GMACInit(quiet);

	return 0;
}
