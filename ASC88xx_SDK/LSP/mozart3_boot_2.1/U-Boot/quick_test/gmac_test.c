#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>
#include <malloc.h>
#include <linux/mdio.h>

extern int get_phy_id(struct mii_dev *bus, int addr, int devad, u32 *phy_id);
extern struct phy_driver *get_phy_driver(struct phy_device *phydev,
				phy_interface_t interface);

extern TPrivate* gmac_returnTPrivate(void) ;
extern int GMACDmaRxGet( TGMAC *Dev, u32 *Status, u32 *Buffer1, u32 *Length1);
extern int GMACDmaRxValid( u32 Status );
extern u32 GMACDmaRxLength( u32 Status );
extern int GMACDmaRxSet( TGMAC *Dev, u32 Buffer1, u32 Length1);
extern void eth_halt(void);
extern void GMACInit(int quiet);
extern u16 GMACMiiRead( TGMAC *Dev, u8 Reg );
extern void GMACMiiWrite( TGMAC *Dev, u8 Reg, u16 Data );
extern int eth_init(bd_t *bis);
extern int eth_send(volatile void *packet, int length);
extern int reset_net_phy(void) ;
extern int gmac_quick_testspeed ;
extern int gmac_quick_testbmcr;

int eth_rx_QuickTest(volatile char *pkt ,int txPatternSize, int* errSummary)
{
    TPrivate *pr;
    int r = 0;
    int i = 0;
    int errNum = -1;
    int recieve = 0;
    int timeout;

    pr = gmac_returnTPrivate();
    TGMAC *gc = &(pr->gmac);

    udelay(10000);
    timeout = 10000;

    while( (timeout >= 0) && (recieve!=1) ) {
        u32 buffer1 = 0;
        u32 status = 0;

        /* get rx descriptor content */
        r = GMACDmaRxGet( gc, &status, &buffer1, NULL);

        if (r >= 0 && buffer1 != 0) //receive something!
        {
            if (GMACDmaRxValid(status)) //no error!! and "whole" frame in the buf(because buf size = 0x600)
            {
                int len;
                //get recieved packet length
                len = GMACDmaRxLength( status ) - 4;

                recieve = 1;
                errNum = 0;

                //compare Tx-Packet and Rx-Packet
                uchar *tmp = (uchar*)buffer1;

                for(i=0; i<len; i++)
                {
                    if ( *(tmp+i) != *(pkt+i))
                    {
                        errNum++;
                    }
                }
                timeout = 10000;
            }

            GMACDmaRxSet(gc, (u32)buffer1, 1600);//set the default RxDesc
            if ( errNum > 0 ) {
                return errNum;
            }
        }
        else {
            timeout--;
        }
    }//end while timeout

    if ( timeout < 0 ) {
        printf("timeout\n");

        return -5 ;//timeout
    }

    if ( recieve != 1 )
    {
        return -4;
    }

    return errNum;
}

void GMAC_RandPattern( volatile char* pkt, int patternSize, int counter )
{
    int length, seed = 0 ;
    uchar pattern[10] = { 0x5a, 0xff, 0x00, 0x69, 0x96, 0x34, 0x87, 0xdc, 0xa5, 0x43 } ;

    //for broadcast
    for ( length = 0 ; length < 6 ; length++ )
    {
        *(pkt+length) = 0xff ;
    }

    for ( length = 6 ; length < patternSize ; length++ )
    {
        seed = ((counter % 5) + (length % 7)%10) ;
        *(pkt+length) =  pattern[seed];
    }

}

int GMAC_do_quick_test(int option)
{
	TPrivate *pr ;
	TGMAC *gc;
	bd_t *bdTmp = NULL ;
	char *pkt ;
	volatile u16 phyData ;
	int link_timeout = 0;
	int counter = 50;
	int patternSize = 1460 ;
	int *errSummary ;
	int errReturn = 0 ;
	struct phy_device *phydev;
	pr = gmac_returnTPrivate() ;
	gc = &(pr->gmac);

	phydev = gc->phydev;

	/*
	 * 1. Since MAC or PHY loopback test still needs TX/RX Clock, must plug cable to RJ45 to active PHY.
	 * 2. Some PHY can work without cable by enable PHY loopback
	 *     ex. IP1001. whether verify MAC loopback or PHY loopback, Just enable PHY Loopback, and Check link up.
	 *         Then start tx/rx test.
	 *     But not all PHY can do this........so....just write down a memo.
	 * 3. Realtek PHY is different to others
	 *    By experiment when test MAC 1000 loopback, it need to set BMCR_ANENABLE.
	 *    But When test PHY loopback, it need to disable BMCR_ANENABLE.....SIGH...XD
	 * 4.Also even you check (BMCR_LOOPBACK == 0), it migh still need more delay(x) before start tx/rx test.
	 *    ............SIGH...
	 */

	/* Force SPEED_1000/ SPEED_100 for quick test */
	phydev->autoneg = AUTONEG_DISABLE;
	phydev->speed = (gmac_quick_testspeed == 1)? SPEED_1000 : SPEED_100;
	phydev->duplex = DUPLEX_FULL;
	phy_config(phydev);
	phydev->link = 1;

	if (option == 1) {
		if (((phydev->drv->uid & 0xfffff0) == 0x1cc910) && (gmac_quick_testspeed == 1)) {
			phyData = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
			phyData |= BMCR_ANENABLE;
			phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, phyData);
		}
	}

	if (option == 2) /* Loopback */
	{
		phyData = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
		phyData |= BMCR_LOOPBACK;
		phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, phyData);
	}

	/* Do a fake read, or fix speed config will not be applied */
	phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);

	/*
	 * Since need cable plug to active PHY,
	 * Check link done might help to check PHY is ready
	 * but not all PHY can do this in loopback mode
	 * ex. Realtek PHY never link up when in loopback mode....leave by timeout.
	 */
	while (!(phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR) & BMSR_LSTATUS)) {
		if ((link_timeout++) > 4000)
			break;
		udelay(1000);
	}

	//Step1. Stop Transmit/Receive actions and re-init descriptors and registers.
	eth_halt() ;//clear

	GMACInit(1);

	v_outl( gc->macBase+GmacFrameFilter, GmacFilterOn | GmacPassControl0 | GmacBroadcastEnable | GmacSrcAddrFilterDisable);

	//Step2. After re-initing regs, now we can set MAC/PHY loopback to specified register.
	if ( option == 1 ) { //set MAC loop-back
		u32 MacAddr = gc->macBase+GmacConfig;
		u32 MacData = v_inl(MacAddr);
		MacData |= GmacLoopbackOn;
		v_outl(MacAddr, MacData);

		while((v_inl(MacAddr) & GmacLoopbackOn) != GmacLoopbackOn) udelay(40);
	}
	else if ( option == 2 ) { //Check PHY loop-back Enable
		while((phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR) & BMCR_LOOPBACK) == 0) udelay(40);
	}

	udelay(5000);

	//Step3. re-start Transmit/Receive actions
	eth_init(bdTmp);

	udelay(5000);
	if (((phydev->drv->uid & 0xfffff0) == 0x1cc910)) {
		udelay(1000);
	}

	//Step4. Loopback test
	pkt = (char*)malloc(sizeof(char)*patternSize);
	errSummary = (int*)malloc(patternSize*sizeof(int));
	while(counter > 0 ) {
		//generate the random pattern packet
		GMAC_RandPattern(pkt, patternSize, counter);

		//send the packet
		eth_send(pkt, patternSize);
		//receive the packet
		errReturn = eth_rx_QuickTest(pkt, patternSize, errSummary);

		//if there is error, end the test
		if (errReturn != 0)
		{
			break;
		}

		counter--;
	}

	//Step5. Stop transmitting and receiving
	eth_halt() ;//clear

	//Step6. Close loopback before re-initing.
	if ( option == 1 ) { //close MAC loopback
		u32 MacAddr = gc->macBase+GmacConfig;
		u32 MacData = v_inl(MacAddr);
		MacData &= (~GmacLoopbackOn);
		v_outl(MacAddr, MacData);
		while((v_inl(MacAddr) & GmacLoopbackOn) != 0) udelay(40);
	}
	else if ( option == 2 ) { //close PHY loop-back
		phyData = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
		phyData &= ~BMCR_LOOPBACK;
		phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, phyData);
		//printf("wait #2\n") ;
		while((phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR) & BMCR_LOOPBACK) != 0) udelay(40);
	}

	//Step6. Before leaving, we re-init descriptors and registers
	GMACInit(1);

	if ( errReturn != 0 )
	{
		printf( "errReturn = %d counter=%d\n", errReturn, counter );
		free(errSummary);
		free(pkt);

		return errReturn;
	}

	free(errSummary);
	free(pkt);

	return 0;
}

int gmac_MDC_MDIO_test(void)
{
	int result = 0;
	TPrivate *pr;
	TGMAC *gc;
	int i = 0;
	volatile int delay = 10000;
	struct mii_dev *bus;
	u32 phy_id;
	int r;
	struct phy_device *phydev;

	pr = (TPrivate*)gmac_returnTPrivate();
	gc = &(pr->gmac);
	bus = gc->mdio_bus;

	for ( i = 0 ; i < PHY_MAX_ADDR ; i++ ) { //phy_mask skip 0, since also for boradcast PHY address
		if ((bus->phy_mask & (1 << i)) == 0) {
			r = get_phy_id(bus, i, MDIO_DEVAD_NONE, &phy_id);
			if (r) {
				printf("[MDIO Error!!!] Read ID fail at PHY address %02d, un-recognize return value -- mighe be MDIO error\n", i);
				result = 2;
				break;
			}

			if ((i != gc->phyAddr) && ((phy_id & 0x1fffffff) != 0x1fffffff)) { /* Not expected PHY Address have non-f ID */
				if (((phy_id & 0x1fffffff) == 0x0000ffff) || ((phy_id & 0x1fffffff) == 0xffff0000) || ((phy_id & 0x1fffffff) == 0x00000000))
				{
#ifndef CONFIG_QUICK_TEST_GMAC_WARNING
					continue;

#endif
				}

				printf("  [MDIO Error!!!] PHY address %02d should not has ID value %08x\n", i, phy_id);
				printf("  			-- Should be 0x1fffffff.\n");
				printf("			-- If 0x0000ffff, 0xffff0000 or 0x00000000, please check MDIO line pull resister\n");
				printf("			-- If valid ID, please check PHY Address hardware configuration, should be %02lu\n", gc->phyAddr);

				result = 1;
			} else if (i == gc->phyAddr) { /* Specified PHY Address */
				if ((phy_id & 0x1fffffff) != 0x1fffffff) {
					phydev = malloc(sizeof(*phydev));
					phydev->phy_id = phy_id;
					phydev->drv = get_phy_driver(phydev, gc->interface); /* try to get the registered PHY drivers */
					printf("[ MDC/MDIO ] PHY: %s\n", phydev->drv->name);
					free(phydev);
				} else {
					printf( "  -------------------------------------------\n" ) ;
					printf( "  [MDIO Error!!!] Default PhyAddr %02lu has ID %08x\n", gc->phyAddr, phy_id);
					printf( "  -------------------------------------------\n" ) ;
					result = 2;
				}
			}
		}
	}

	while(delay >= 0) {
		delay--;
	}

	return result;
}

int gmac_test_func(void)
{
	int result ;
	int answer = 0 ;
	TPrivate *pr ;
	TGMAC *gc;
	pr = (TPrivate*)gmac_returnTPrivate() ;
	gc = &(pr->gmac);

	/* 1. MDC MDIO Test */
	/* Move to first step,
	 * Reason: Need to test once only, should not in gmac_quick_testspeed while loop
	 *         Should test first, Loopback test also need MDC/MDIO to configure PHY.
	 */
	result = gmac_MDC_MDIO_test() ;
	if ( result == 2 ) {
		printf("[ MDC/MDIO ] ..................................... Fail\n" ) ;
		answer |= -1 ;
		goto exit_gmac_test;
	}
	else {
		printf("[ MDC/MDIO ] ..................................... Pass\n" ) ;
	}

	/* RGMII can't support loopback */
	if ((gc->interface == PHY_INTERFACE_MODE_GMII) ||
	    (gc->interface == PHY_INTERFACE_MODE_RGMII))
		gmac_quick_testspeed = 1;
	else
		gmac_quick_testspeed = 0;

	/* Fix when PHY doesn't support GIGA but at default phy_mode "gmii" */
	if (!((gc->phydev->drv->features & SUPPORTED_1000baseT_Half) ||
              (gc->phydev->drv->features & SUPPORTED_1000baseT_Full)))
		gmac_quick_testspeed = 0;


	while(gmac_quick_testspeed != -1) {
		switch(gmac_quick_testspeed) {
			case 1 :
				printf("\n>> Force to 1Gbps mode for testing.....\n") ;
				break ;
			case 0 :
				printf("\n>> Force to 100Mbps mode for testing.....\n") ;
				break ;
		}

		//2. MAC Test
		result = GMAC_do_quick_test(1);
		if ( result != 0 ) {
			printf("[ MAC ] ......................................... Fail\n" ) ;
			if ( result > 0 ) {
				printf("        -- The received packet is different from\n" ) ;
				printf("        -- the packet we sent.\n" ) ;
			}
			else {
				printf("        -- We do not receive any packet in a limited time.\n" ) ;
			}
			answer |= -1 ;

			//goto exit_gmac_test ;
		}
		else {
			printf("[ MAC ] ......................................... Pass\n" ) ;
		}

		//3. PHY Test
		result = GMAC_do_quick_test(2) ;
		if ( result != 0 ) {
			printf("[ PHY ] ......................................... Fail\n" ) ;
			if ( result > 0 ) {
				printf("        -- The received packet is different from\n" ) ;
				printf("        -- the packet we sent.\n" ) ;
			}
			else {
				printf("        -- We do not receive any packet in a limited time.\n" ) ;
			}
			answer |= -1 ;

			//goto exit_gmac_test ;
		}
		else {
			printf("[ PHY ] ......................................... Pass\n" ) ;
		}

		gmac_quick_testspeed-- ;//switch to next speed.
	}

exit_gmac_test :
	printf("\n>> Finish the test. Now reset the related IPs...\n") ;

	gmac_quick_testspeed = -1 ;//reset gmac_quick_testspeed
	GMACInit(1);
	return answer;
}
