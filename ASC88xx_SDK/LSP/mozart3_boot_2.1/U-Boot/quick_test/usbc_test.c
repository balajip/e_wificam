#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>


#define USBC        VPL_USBC_MMR_BASE
#define USBC_PHY    VPL_USB_PHY_BASE
ulong USBC_BASE[1] = { USBC} ;
ulong USBC_PHY_BASE[1] = { USBC_PHY} ;

#define TIMEOUT   3*10000   // 3 seconds for udelay(100)


unsigned int usbc_write_reg(unsigned int base, unsigned int reg, unsigned int val)
{
	v_outl((unsigned int *)(base+reg), val);
//	printf("addr, val = %x, %x\n", base+reg, val);
	return 0;
}
unsigned int usbc_read_reg(unsigned int base, unsigned int reg)
{
	unsigned int reg_val;
	reg_val = v_inl((unsigned int *) (base + reg));
//	printf("addr, val = %x, %x\n", base+reg, reg_val);
	return reg_val ;
}

int
usbc_phy_init(void)
{
	unsigned int  count, buffer_reg, phy_ctrl, usbc_phy_base;


	usbc_phy_base = USBC_PHY_BASE[0];
	phy_ctrl = usbc_read_reg(usbc_phy_base, USBC_CTRL);

	if ((phy_ctrl & (PHY_RST_N_MASK|CORE_RST_N_MASK|CORE_RST_RELEASE_MASK))
		== (PHY_RST_N(1)|CORE_RST_N(1)|CORE_RST_RELEASE(1)))
	{
		/* reset PHY, non-OTG */
		phy_ctrl &= ~(OTG_MODE(1) | PHY_RST_RELEASE(1));
		usbc_write_reg(usbc_phy_base, USBC_CTRL, phy_ctrl);
		phy_ctrl |= PHY_RST_RELEASE(1);
		usbc_write_reg(usbc_phy_base, USBC_CTRL, phy_ctrl);
		udelay(60);
		phy_ctrl = usbc_read_reg(usbc_phy_base, USBC_CTRL);
		
	}
	else
	{
#if CONFIG_ASC88XX_M2 == 1
		buffer_reg = 0xD000;
		usbc_write_reg(usbc_phy_base, USBC_PWR_ON_CYCLE, buffer_reg);
		phy_ctrl &= 0x000FFFFF;
		phy_ctrl |= PHY_RST_RELEASE(1);
		usbc_write_reg(usbc_phy_base, USBC_CTRL, phy_ctrl);
#elif CONFIG_ASC88XX_A == 1
		buffer_reg = 0xFF00;
		usbc_write_reg(usbc_phy_base, USBC_PWR_ON_CYCLE, buffer_reg);
		/*
		if ((phy_ctrl & 0x020FFFFF)  != 0x0202871C) 
		{
			printf("USB PHY CTRL value: 0x%08x\n", phy_ctrl);
			return;
		}
		*/
		phy_ctrl &= 0x000FFFFF;
		phy_ctrl |= REFCLK_SEL(0x2) | REFCLK_DIV(1) | PHY_RST_RELEASE(1);
		usbc_write_reg(usbc_phy_base, USBC_CTRL, phy_ctrl);
#endif
	}

	count = TIMEOUT;
	do
	{
		udelay(100);
		phy_ctrl = usbc_read_reg(usbc_phy_base, USBC_CTRL);
	} while ((phy_ctrl & (PHY_RST_N_MASK|CORE_RST_N_MASK|CORE_RST_RELEASE_MASK))
	             != (PHY_RST_N(1)|CORE_RST_N(1)|CORE_RST_RELEASE(1))
	      && count-- > 0);

	if (count == 0)
		return -1;
	else
		return 0;
}


void print_reg(unsigned int usbc_base, unsigned int reg)
{
	unsigned int buffer_reg;
	buffer_reg = usbc_read_reg(usbc_base, reg);
	printf("%x = %x\n",reg, buffer_reg);
}

int
usbc_initial(unsigned int usbc_base)
{
	unsigned int buffer_reg;
	int count;


	buffer_reg = 0x0;
	usbc_write_reg(usbc_base, GAHBCFG, buffer_reg);
	usbc_write_reg(usbc_base, GUSBCFG, USBTrdTim(5)|HNPCap(0)|SRPCap(0)|ULPI_UTMI_Sel(0)|PHYIf(1));
	usbc_write_reg(usbc_base, GRSTCTL, CSftRst(1));
	count = TIMEOUT;
	while ((buffer_reg = usbc_read_reg(usbc_base, GRSTCTL)) & CSftRst_MASK)
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};

	udelay(100);
	usbc_write_reg(usbc_base, GUSBCFG, USBTrdTim(5)|HNPCap(0)|SRPCap(0)|ULPI_UTMI_Sel(1)|PHYIf(0));
	usbc_write_reg(usbc_base, GRSTCTL, CSftRst(1));
	count = TIMEOUT;
	while ((buffer_reg = usbc_read_reg(usbc_base, GRSTCTL)) & CSftRst_MASK)
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};
	udelay(100);
	usbc_write_reg(usbc_base, GUSBCFG, USBTrdTim(5)|HNPCap(0)|SRPCap(0)|ULPI_UTMI_Sel(0)|PHYIf(1));
	usbc_write_reg(usbc_base, GAHBCFG, DMAEn(1));
	usbc_write_reg(usbc_base, GUSBCFG, USBTrdTim(5)|HNPCap(0)|SRPCap(0)|ULPI_UTMI_Sel(0)|PHYIf(1)|TOutCal(5));
	usbc_write_reg(usbc_base, GOTGINT, 0xffffffff);
	usbc_write_reg(usbc_base, GINTSTS, 0xffffffff);
	usbc_write_reg(usbc_base, GINTMSK, WkUpInt(1)|SessReqInt(1)|DisconnInt(1)|ConIDStsChng(1)|
	                                   USBSusp(1)|OTGInt(1)|ModeMis(1));
	usbc_write_reg(usbc_base, HCINTMSKn(0), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(0), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(1), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(1), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(2), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(2), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(3), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(3), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(4), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(4), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(5), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(5), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(6), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(6), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(7), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(7), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(8), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(8), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(9), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(9), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(10), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(10), 0xffffffff);
	usbc_write_reg(usbc_base, HCINTMSKn(11), 0x00000000);
	usbc_write_reg(usbc_base, HCINTn(11), 0xffffffff);
	usbc_write_reg(usbc_base, PCGCR, 0x00000000);
	usbc_write_reg(usbc_base, HCFG, 0x00000000);
//	usbc_write_reg(usbc_base, GRXFSIZ, 0x0000010e);
//	usbc_write_reg(usbc_base, GNPTXFSIZ, 0x0100010e);
//	usbc_write_reg(usbc_base, HPTXFSIZ, 0x0100020e); /* FIXME: cannot be update ? */
//	usbc_write_reg(usbc_base, GOTGCTL, 0x00140000);  /* FIXME: */
	usbc_write_reg(usbc_base, GRSTCTL, TxFNum(0x10) | TxFFlsh(1));
	udelay(100);
	count = TIMEOUT;
	while ((buffer_reg = usbc_read_reg(usbc_base, GRSTCTL)) & TxFFlsh_MASK)
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};

	usbc_write_reg(usbc_base, GRSTCTL, RxFFlsh(1));
	count = TIMEOUT;
	while ((buffer_reg = usbc_read_reg(usbc_base, GRSTCTL)) & RxFFlsh_MASK)
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};

	usbc_write_reg(usbc_base, HCCHARn( 0), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 1), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 2), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 3), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 4), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 5), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 6), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 7), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 8), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 9), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn(10), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn(11), ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 0), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 1), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 2), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 3), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 4), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 5), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 6), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 7), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 8), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn( 9), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn(10), ChEna(1) | ChDis(1));
	usbc_write_reg(usbc_base, HCCHARn(11), ChEna(1) | ChDis(1));
	for(count=0; count<10; count++)
	{
		int cnt = TIMEOUT;
		while (buffer_reg = usbc_read_reg(usbc_base, HCCHARn(count)),
		       (buffer_reg & (ChEna_MASK|ChDis_MASK)) == (ChEna(1)|ChDis(1)));
		{
			if (cnt-- == 0)
			{
				printf("timeout [%s(), #%d]\n", __func__, __LINE__);
				return -1;
			}
			udelay(100);
		};
	}
	count = TIMEOUT;
	while ((buffer_reg = usbc_read_reg(usbc_base, HCCHARn(11))) & (ChEna_MASK|ChDis_MASK))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};

	usbc_write_reg(usbc_base, HPRT, PrtPwr(1));
	usbc_write_reg(usbc_base, GINTMSK, 0x00000000);
	usbc_write_reg(usbc_base, GINTSTS, 0xffffffff);
	usbc_write_reg(usbc_base, GOTGINT, 0xffffffff);
//	usbc_write_reg(usbc_base, GINTMSK, 0xf0000806);
	usbc_write_reg(usbc_base, GINTMSK, WkUpInt(1)|SessReqInt(1)|DisconnInt(1)|ConIDStsChng(1)|
	                                   USBSusp(1)|OTGInt(1)|ModeMis(1));
	usbc_write_reg(usbc_base, GAHBCFG, DMAEn(1)|GlbIntrMsk(1));

	return 0;
}

void usbc_send_cmd(void)
{
}

void usbc_check_cmd_done(void)
{
}

int
usb_send_pipe(unsigned int usbc_base, int ch)
{
	unsigned char buf[256];
	int i, count;
	unsigned int buffer_reg;

	/* Setup stage */
	for (i=0; i<256;i++)
		buf[i] = 0x0;
	buf[0] = 0x80;
	buf[1] = 0x06;
	buf[2] = 0x00;
	buf[3] = 0x01;
	buf[4] = 0x00;
	buf[5] = 0x00;
	buf[6] = 0x40;
	buf[7] = 0x00;

	usbc_write_reg(usbc_base, HCINTn(ch), 0x7ff);
	usbc_write_reg(usbc_base, HCINTMSKn(ch), AHBErr(1)|ChHltd(1));
	usbc_write_reg(usbc_base, HAINTMSK, 1 << ch);
	usbc_write_reg(usbc_base, GINTMSK, 0xf3000806);
	usbc_write_reg(usbc_base, HCSPLTn(ch), 0x0);
	usbc_write_reg(usbc_base, HCTSIZn(ch), PID(3)|PktCnt(1)|XferSize(8));
	usbc_write_reg(usbc_base, HCDMAn(ch), (unsigned int)&buf);
	usbc_write_reg(usbc_base, HCCHARn(ch), ChEna(1)|McEc(1)|EPDir(0)|MPS(64));
	count = TIMEOUT;
	while (!((buffer_reg = usbc_read_reg(usbc_base, GINTSTS)) & HChInt_MASK))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d] GINTSTS: 0x%x\n", __func__, __LINE__, buffer_reg);
			return -1;
		}
		udelay(100);
	};

	count = TIMEOUT;
	while (buffer_reg = usbc_read_reg(usbc_base, HCINTn(ch)),
	       (buffer_reg & (ACK_MASK|ChHltd_MASK|XferCompl_MASK))
	           != (ACK(1)|ChHltd(1)|XferCompl(1)))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d], HCINT%d: 0x%x\n", __func__, __LINE__, ch, buffer_reg);
			return -1;
		}
		udelay(100);
	};
	usbc_write_reg(usbc_base, HCINTMSKn(ch), 0x0);
	usbc_write_reg(usbc_base, HCINTn(ch), 0xffffffff);

	/* Data stage */
	usbc_write_reg(usbc_base, HCINTn(ch+1), 0x7ff);
	usbc_write_reg(usbc_base, HCINTMSKn(ch+1), AHBErr(1)|ChHltd(1));
	usbc_write_reg(usbc_base, HAINTMSK, 1 << (ch+1));
	usbc_write_reg(usbc_base, GINTMSK, 0xf3000806);
	usbc_write_reg(usbc_base, HCSPLTn(ch+1), 0x0);
	usbc_write_reg(usbc_base, HCTSIZn(ch+1), PID(2)|PktCnt(1)|XferSize(64));
	usbc_write_reg(usbc_base, HCDMAn(ch+1), (unsigned int)&buf);
	usbc_write_reg(usbc_base, HCCHARn(ch+1), ChEna(1)|McEc(1)|EPDir(1)|MPS(64));
	count = TIMEOUT;
	while (!((buffer_reg = usbc_read_reg(usbc_base, GINTSTS)) & HChInt_MASK))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d], GINTSTS: 0x%x\n", __func__, __LINE__, buffer_reg);
			return -1;
		}
		udelay(100);
	};

	count = TIMEOUT;
	while (buffer_reg = usbc_read_reg(usbc_base, HCINTn(ch+1)),
	       (buffer_reg & (ACK_MASK|ChHltd_MASK|XferCompl_MASK))
	           != (ACK(1)|ChHltd(1)|XferCompl(1)))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d], HCINT%d: 0x%x\n", __func__, __LINE__, ch+1, buffer_reg);
			return -1;
		}
		udelay(100);
	};
	usbc_write_reg(usbc_base, HCINTMSKn(ch+1), 0x0);
	usbc_write_reg(usbc_base, HCINTn(ch+1), 0xffffffff);

	/* Status stage */
	usbc_write_reg(usbc_base, HCINTn(ch+2), 0x7ff);
	usbc_write_reg(usbc_base, HCINTMSKn(ch+2), AHBErr(1)|ChHltd(1));
	usbc_write_reg(usbc_base, HAINTMSK, 1 << (ch+2));
	usbc_write_reg(usbc_base, GINTMSK, 0xf3000806);
	usbc_write_reg(usbc_base, HCSPLTn(ch+2), 0x0);
	usbc_write_reg(usbc_base, HCTSIZn(ch+2), 0x40080000);
	usbc_write_reg(usbc_base, HCDMAn(ch+2), (unsigned int)&buf);
	usbc_write_reg(usbc_base, HCCHARn(ch+2), ChEna(1)|McEc(1)|EPDir(0)|MPS(64));
	count = TIMEOUT;
	while (!((buffer_reg = usbc_read_reg(usbc_base, GINTSTS)) & HChInt_MASK))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d], GINTSTS: 0x%x\n", __func__, __LINE__, buffer_reg);
			return -1;
		}
		udelay(100);
	};

	count = TIMEOUT;
	while (buffer_reg = usbc_read_reg(usbc_base, HCINTn(ch+2)),
	       (buffer_reg & (ACK_MASK|ChHltd_MASK|XferCompl_MASK))
	           != (ACK(1)|ChHltd(1)|XferCompl(1)))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d], HCINT%d: 0x%x\n", __func__, __LINE__, ch+2, buffer_reg);
			return -1;
		}
		udelay(100);
	};
	usbc_write_reg(usbc_base, HCINTMSKn(ch+2), 0x0);
	usbc_write_reg(usbc_base, HCINTn(ch+2), 0xffffffff);

	printf("Vendor ID is %02x%02x\n",  buf[9], buf[8]);
	printf("The Product ID is %02x%02x\n", buf[11], buf[10]);
	return 0;
}


int usbc_identification(unsigned int usbc_base)
{
	// CMD 0
	unsigned int buffer_reg, count;
	usbc_write_reg(usbc_base, HPRT, PrtSpd(1)|PrtPwr(1)|PrtLnSts(1)|
	                                PrtRst(0)|PrtConnDet(1)|PrtConnSts(1)/*0x00021403*/);
	usbc_write_reg(usbc_base, HPRT, PrtSpd(1)|PrtPwr(1)|PrtLnSts(1)|
	                                PrtRst(1)|PrtConnDet(0)|PrtConnSts(1)/*0x00021501*/);

	udelay(100000); // at least 50ms for HS; 10ms for FS/LS
	usbc_write_reg(usbc_base, HPRT, PrtSpd(1)|PrtPwr(1)|PrtLnSts(1)|
	                                PrtRst(0)|PrtConnDet(0)|PrtConnSts(1)/*0x00021401*/);
	count = TIMEOUT;
	while (!((buffer_reg = usbc_read_reg(usbc_base, GINTSTS)) & Sof_MASK))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};

	count = TIMEOUT;
	while (buffer_reg = usbc_read_reg(usbc_base, HPRT),
	       (buffer_reg & (PrtEnChng_MASK|PrtEna_MASK|PrtConnSts_MASK))
	           != (PrtEnChng(1)|PrtEna(1)|PrtConnSts(1)))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};
	udelay(10000);
	usbc_write_reg(usbc_base, HPRT, PrtPwr(1)|PrtEnChng(1)|PrtConnSts(1));
	count = TIMEOUT;
	while (((buffer_reg = usbc_read_reg(usbc_base, HPRT)) & PrtEnChng_MASK))
	{
		if (count-- == 0)
		{
			printf("timeout [%s(), #%d]\n", __func__, __LINE__);
			return -1;
		}
		udelay(100);
	};

	return usb_send_pipe(usbc_base, 0);
}


int USBC_do_test(void)
{
	// 1. read ip version register
	// 2. initial
	// 3. wait for usb dongle insert
	// 4. get descriptor
	
	unsigned int usbc_base;
	usbc_base = USBC_BASE[0];
	int i, tmp;
	// 1. read ip version register

	if (usbc_read_reg(usbc_base, GSNPSID) != SPNSID)
	{
		printf("[ USBC ] check version fail \n");
		return -1;
	}

	// 2. initial
	if (usbc_phy_init() != 0)
		return -2;
	if (usbc_initial(usbc_base) != 0)
		return -3;
	
	// 3. check dongle insert for ten times
	for (i=0; i<10000; i++)
	{
		if ((usbc_read_reg(usbc_base, GINTSTS) & PrtInt_MASK)
		 && (usbc_read_reg(usbc_base, HPRT) & PrtConnDet_MASK))
		{
			tmp = 1;
			break;
		}
		else if (i == 0)
			printf("Please insert a USB dongle...\n");
		udelay(1000);
		tmp = 0;
		if (i % 1000 == 0)
			printf(".");
	}
	putc('\n');
	if (tmp == 0)
	{
		return -1;
	}

	// 4. get descriptor
	// identification card
	return usbc_identification(usbc_base);
}

int usbc_test_func(void)
{
	int result = 0;
	printf( "usbc_test_func!!!\n" ) ;
	result = USBC_do_test();

	return result ;
}
