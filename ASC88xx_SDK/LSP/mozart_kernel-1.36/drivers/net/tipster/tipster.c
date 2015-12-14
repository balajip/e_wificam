/* Tipster PCI/Ethernet test chip driver for OS Linux
 * Linux netcard driver
 *
 *
 * 3 Jun 1999 - G.Kazakov.
 */
#ifdef MODULE
#ifdef MODVERSION
#include <linux/modversion.h>
#endif
#include <linux/module.h>
#else
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif

#include <linux/version.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/timer.h>
#include <linux/crc32.h>
#include <linux/ethtool.h>
#include <asm/io.h>
#include <asm/cacheflush.h>
#include <mach/ugpio.h>
#include <linux/proc_fs.h>
#include "tipster_dev.h"

#define BUFFER_SIZE (1536)

#define dma_alloc_consistent(d,s,p) pci_alloc_consistent(d,s,p)

#define ENABLE_LOOPBACK 0

#define MOZART_V1_VERSION 0x01000000
static int is_mozart_v1 = 0 ;

#define TRACE
#ifdef TRACE
  static int debug = 0;
  void tipster_trace(char *format, ...)
  {
    if(debug) __builtin_return( __builtin_apply( printk, __builtin_apply_args(), 20 ) );
  }
#else
  void tipster_trace(char *format, ...) {}
#endif

enum MiiRegisters
{
  GEN_ctl   = 0x00,
  GEN_sts   = 0x01,
  GEN_id_hi = 0x02,
  GEN_id_lo = 0x03,
  AN_adv    = 0x04,
  AN_lpa    = 0x05,
  AN_exp    = 0x06,
  AN_np     = 0x07,
  PHY_ctl   = 0x10,
};

#include "tipster_dev.h"                   /* Tipster device definitions and functions */

#define DEFAULT_PHY_ADDRESS 0x1
#define DEFAULT_MAC_ADDRESS { 0x02, 0x00, 0x01, 0x01, 0x01, 0x12 }

#define ETHERNET_PACKET_EXTRA 18    /* preallocated length for rx packets is MTU + ETHERNET_PACKET_EXTRA */
#define ETHERNET_PACKET_COPY  250   /* max length when received data is copied to new skb */

#define TIMEOUT  (10*HZ)

/* L&T */
#if LINUX_VERSION_CODE == 131589
    #define net_device device
    #define early_stop_netif_stop_queue(dev) test_and_set_bit(0, &dev->tbusy)
#else
    #define early_stop_netif_stop_queue(dev) 0
#endif


volatile Private pr;
static struct net_device *ndev;

volatile int trans_busy=0;
#define DMAMMR_INTERRUPT 0x1c
#define DMA_RxAbnormal_Bit (1<<7)
u32 addr;
#ifdef MODULE


/* L&T */

static char devicename[16] ={0, };


static struct net_device this_device = {
	"", /* will be inserted by linux/drivers/net/net_init.c */
	0, 0, 0, 0,
	0, 0,  /* I/O address, IRQ */
	0, 0, 0, NULL, tipster_probe };

static int io = 0x300;
static int irq = 0x20;
static int dma = 0;
static int mem = 0;
u32 addr;

int init_module(void)
{
	int result;

	if (io == 0)
		printk(KERN_WARNING "%s: You shouldn't use auto-probing with insmod!\n", cardname);

	memcpy(this_device.name, devicename, 16);
	/* Copy the parameters from insmod into the device structure. */

	this_device.base_addr = io;
	this_device.irq       = irq;
	this_device.dma       = dma;
	this_device.mem_start = mem;


	if ((result = register_netdev(&this_device)) != 0)
		return result;

	return 0;
}

void
cleanup_module(void)
{
	/* No need to check MOD_IN_USE, as sys_delete_module() checks. */
	unregister_netdev(&this_device);
	/*
	 * If we don't do this, we can't re-insmod it later.
	 * Release irq/dma here, when you have jumpered versions and
	 * allocate them in net_probe1().
	 */
	/*
	   free_irq(this_device.irq, dev);
	   free_dma(this_device.dma);
	*/
	/*release_region(this_device.base_addr, NETCARD_IO_EXTENT);*/

	if (this_device.priv)
		kfree(this_device.priv);

        /* unmap the mapped memory */
        if(addr)
            iounmap((u32 *)addr);
}

#endif /* MODULE */
//jon
u32 rxBufHandle = 0;
char * rxBuffer;
static struct net_device *dev_for_receive = NULL;

//Because a lot of CORE-DUMP message will stall kernel performance significantly,
//we use a flag "__GFP_NOWARN" to turn off message.
static inline struct sk_buff *tipster_dev_alloc_skb(unsigned int length)
{
    return __dev_alloc_skb(length, GFP_ATOMIC | __GFP_NOWARN);
}

static void receive( unsigned long unused )       /* handle received packets */
{
  Private *pr = NULL;
  Tipster *tc = NULL;
  int r = 0;
  int curr = 0;

  if(dev_for_receive == NULL){
  	 printk("[jon]dev=NULL!\n");
     return;
  }


  pr = (Private *)dev_for_receive->priv;
  if(pr == NULL){
  		printk("[jon]pr=NULL!\n");
        return;
  }

  tc = &pr->tipster;
  if(tc == NULL){
	  printk("[jon]tc=NULL!\n");
	  return;
  }

  do                          /* handle recieve descriptors */
  {
    u32 data1;
    u32 status;
    struct sk_buff *skb = NULL;
	curr = tc->rxBusy;
    r = TipsterDmaRxGet( tc, &status, NULL, NULL, &data1 );   /* get rx descriptor content */

	if( r >= 0)
    {
      if( TipsterDmaRxValid(status) ) /* process good packets only */
      {
      	int len;
        len = TipsterDmaRxLength( status ) - 4; /* ignore Ethernet CRC bytes */

		skb = tipster_dev_alloc_skb( len+2 );
		if( skb == NULL )
        {
            TR( KERN_ERR "Tipster::receive(%s) - no memory for skb\n", dev_for_receive->name );

			tc->stats.rx_dropped++;
            TipsterDmaRxSet( tc, rxBufHandle + (curr*(BUFFER_SIZE)), (BUFFER_SIZE), NULL );

            goto exit_receive ;
        }
		skb_reserve( skb, 2 );//16 bit align

	  	memcpy( skb_put(skb,len), rxBuffer + (curr*(BUFFER_SIZE)), len );   /* copy data to the skb */

		if (TipsterDmaRxCRC(status)) {
			skb->ip_summed = CHECKSUM_UNNECESSARY; /* cause there's no csum information from hw */
		}
		else {
            skb->ip_summed = CHECKSUM_NONE;
		}

		skb->dev = dev_for_receive;
        skb->protocol = eth_type_trans(skb, dev_for_receive);    /* set packet type */
        netif_rx(skb);        /* send the packet up protocol stack */
        dev_for_receive->last_rx = jiffies;                         /* set the time of the last receive */
        tc->stats.rx_packets++;
        tc->stats.rx_bytes += len;
	  }
      else//not valid pkts
      {
        tc->stats.rx_errors++;
        tc->stats.collisions += TipsterDmaRxCollisions(status);
        tc->stats.rx_crc_errors += TipsterDmaRxCrc(status);
		tc->stats.rx_frame_errors += TipsterDmaRxFrame(status);
		tc->stats.rx_length_errors += TipsterDmaRxLengthError(status);
      }

      r = TipsterDmaRxSet( tc, rxBufHandle + (curr*(BUFFER_SIZE)), (BUFFER_SIZE), NULL );  /* put the skb to the descriptor */
	  if( r < 0 )
      {
         TR( KERN_DEBUG "Tipster::receive(%s) - cannot set rx descriptor for skb %p\n", dev_for_receive->name, skb );
		 printk("Tipster::receive(%s) - cannot set rx descriptor for skb %p\n", dev_for_receive->name, skb );

      }
    }//get some CPU-OWNed descriptors

  } while( r >= 0 );

exit_receive:
	if(trans_busy) {
		int tmpbsy = virt_to_bus(TipsterReadDmaReg(tc, 0x4c)) - virt_to_bus(TipsterReadDmaReg(tc, 0x0c)) ;
		tmpbsy /= 24 ;
		tc->rxBusy = tmpbsy % NET_RX_DESC_NUM ;
		tc->rxNext = tmpbsy % NET_RX_DESC_NUM ;
	}
  	//trans_busy = 0 ;
    //TipsterDmaIntEnable(tc);
  	//TipsterDmaRxStart( tc ) ;

}


static void dma_free_tx_skbs(Tipster *tc)
{
	int i;
	DmaDesc *tx_list = tc->tx;

	for (i = 0; i < tc->txCount; ++i) {
		DmaDesc *tx_desc = tx_list + i;
		if (tx_desc->data1 != NULL)
      		dev_kfree_skb_irq((struct sk_buff *)(tx_desc->data1));
	}
}

static int RESTART_GMAC_SEC = 0 ;
static struct timer_list handle_abnormal_timer;
static int abnormal_tasklet_counter = 0 ;
static void icp_set_speed(Tipster *tc, u32 speed);
static void rtl_set_speed(Tipster *tc, u32 speed);

void abnormal_timer_func_DescInit( DmaDesc *Desc, bool EndOfRing )
{
  Desc->status = 0;
  Desc->length = EndOfRing ? 0x02000000 : 0;
  Desc->buffer1 = 0;
  Desc->buffer2 = 0;
  Desc->data1 = 0;
  Desc->data2 = 0;
}


static void abnormal_timer_func(unsigned long data)
{
	Tipster *tc = NULL;
	Private *pri = NULL;
	struct net_device *dev;
	int i ;

	pri = (Private *)data;
	tc = &(pri->tipster) ;
	dev = ndev;

    if ((TipsterMiiRead(tc, 0x2) == ICPLUS_IP1001_ID_1) && ((TipsterMiiRead(tc, 0x3) & 0xFFFFFFF0) == (ICPLUS_IP1001_ID_2 & 0xFFFFFFF0))) {
		u16 phy_sts;

        pri->mii.supports_gmii = 1;
        phy_sts = TipsterMiiRead(tc, 0x11);
        tc->linkup = (phy_sts&0x8000) != 0;
        tc->linkchanged = 0;

		if(is_mozart_v1) {
			if ((phy_sts&0x6000) == 0x4000) {
				tc->speed_1000 = 1;
	            tc->speed_100 = 0;
	            tc->speed_10 = 0;
	            icp_set_speed(tc, 2);
		    } else if ((phy_sts&0x6000) == 0x2000) {
	            tc->speed_1000 = 0;
	            tc->speed_100 = 1;
	            tc->speed_10 = 0;
	            icp_set_speed(tc, 1);
		    } else if ((phy_sts&0x6000) == 0) {
	            tc->speed_1000 = 0;
	            tc->speed_100 = 0;
	            tc->speed_10 = 1;
	            icp_set_speed(tc, 0);
	        }
	        tc->fdx = (phy_sts&0x1000) != 0;
	        request_ugpio_pin(PIN_UGPIO4);
		}
		else {
			//Mozart V2
			if ((phy_sts&0x6000) == 0x4000) {
				tc->speed_1000 = 1;
	            tc->speed_100 = 0;
	            tc->speed_10 = 0;
	            icp_set_speed(tc, 2);
		    } else if ((phy_sts&0x6000) == 0x2000) {
	            tc->speed_1000 = 0;
	            tc->speed_100 = 1;
	            tc->speed_10 = 0;
	            icp_set_speed(tc, 1);
		    }
			tc->fdx = (phy_sts&0x1000) != 0;
		}
	}
    else if (((TipsterMiiRead(tc, 0x2) == ICPLUS_IP101_ID_1) && (TipsterMiiRead(tc, 0x3) == ICPLUS_IP101_ID_2)) ||
             ((TipsterMiiRead(tc, 0x2) == MICREL_KSZ8051_ID_1) && ((TipsterMiiRead(tc, 0x3) & 0xFFF0) == MICREL_KSZ8051_ID_2))) {
		tc->linkup = 1;
        tc->linkchanged = 0;
        tc->speed_10 = 0;
		tc->speed_100 = 1;
		tc->speed_1000 = 0;
		tc->fdx = 1;
    }
	else if ((TipsterMiiRead(tc, 0x2) == REALTEK_RTL8112C_ID_1) && (TipsterMiiRead(tc, 0x3) == REALTEK_RTL8112C_ID_2)) {
		u16 phy_sts;

        phy_sts = TipsterMiiRead(tc, 0x11);
		tc->linkup = (phy_sts&0x0400) != 0;
		tc->linkchanged = 0;

		if ((phy_sts&0xC000) == 0x8000) {
			//In RTL8112b, we have to setup gmac properly before reset dma.(hw limit of gmac)
			v_outl(0xf9800000, 0x01200d0c) ;
			tc->speed_1000 = 1;
			tc->speed_100 = 0;
			tc->speed_10 = 0;
			rtl_set_speed(tc, 2);
			if (tc->linkup)
				printk("1000BASE-T ");
		} else if ((phy_sts&0xC000) == 0x4000) {
			//In RTL8112b, we have to setup gmac properly before reset dma.(hw limit of gmac)
			v_outl(0xf9800000, 0x0120cd0c) ;
			tc->speed_1000 = 0;
			tc->speed_100 = 1;
			tc->speed_10 = 0;
			rtl_set_speed(tc, 1);
		} else if ((phy_sts&0xC000) == 0) {
			//In RTL8112b, we have to setup gmac properly before reset dma.(hw limit of gmac)
			v_outl(0xf9800000, 0x01208d0c) ;
			tc->speed_1000 = 0;
			tc->speed_100 = 0;
			tc->speed_10 = 1;
			rtl_set_speed(tc, 0);
		}

		tc->fdx = (phy_sts&0x2000) != 0;
		tc->rgmii_interface = 1;

		request_ugpio_pin(PIN_UGPIO4);
    }
    else if ((TipsterMiiRead(tc, 0x2) == BROADCOM_BCM54616_ID_1) && (TipsterMiiRead(tc, 0x3) == BROADCOM_BCM54616_ID_2)) {
		if((readl(0xf4f80034) & (1 << 5)) != 0) { //external clock -- 10/100Mbps
				tc->linkup = 1;
		        tc->linkchanged = 0;
        		tc->speed_10 = 0;
		        tc->speed_100 = 1;
		        tc->speed_1000 = 0;
		        tc->fdx = 1;
		        icp_set_speed(tc, 1);
		 }
         else {//internal clock -- 1000Mbps
		        tc->linkup = 1;
		        tc->linkchanged = 0;
		        tc->speed_10 = 0;
		        tc->speed_100 = 0;
		        tc->speed_1000 = 1;
		        tc->fdx = 1;
		        TipsterMiiWrite(tc, 0x09, 0x300);
		        TipsterMiiWrite(tc, 0x00, 0x1200);
         }
	}
	else {
		/* give it a default value, but may not work */
        tc->linkup = 1;
        tc->linkchanged = 0;
        tc->speed_10 = 0;
        tc->speed_100 = 1;
        tc->speed_1000 = 0;
        tc->fdx = 1;
	}

    trans_busy = 0 ;


	TipsterDmaInit( tc ) ;
	TipsterMacInit(tc, dev_for_receive->dev_addr, dev_for_receive->broadcast);
	for(i = 0; i < NET_RX_DESC_NUM; i++){
		TipsterDmaRxSet( tc, rxBufHandle + (i *(BUFFER_SIZE)), BUFFER_SIZE, NULL);
  	}
	TipsterDmaIntClear(tc);
	TipsterDmaIntEnable(tc);
	TipsterDmaRxStart(tc);
	TipsterDmaTxStart(tc);
	netif_carrier_on(dev_for_receive);
	netif_start_queue(dev_for_receive);
	enable_irq(dev_for_receive->irq);
	//printk("enable_irq(%d)\n", dev_for_receive->irq) ;

	abnormal_tasklet_counter = 0 ;
}

static void abnormal_tasklet_func( unsigned long unused )
{
	Private *pr = NULL;
	Tipster *tc = NULL;
	struct net_device *dev;
  	int r = 0;
  	int curr = 0;

	int RESTART_GMAC_JIFFIES = (100*RESTART_GMAC_SEC) ;

    if(abnormal_tasklet_counter != 0)
		return ;
	printk(KERN_DEBUG "[jam]abnormal_tasklet_func() = %d\n", abnormal_tasklet_counter) ;
	abnormal_tasklet_counter++ ;

  	if(dev_for_receive == NULL){
  		printk("[jon]dev=NULL!\n");
     	return;
  	}

    pr = (Private *)dev_for_receive->priv;
  	if(pr == NULL){
  		printk("[jon]pr=NULL!\n");
        return;
  	}

  	tc = &pr->tipster;
  	if(tc == NULL){
	  	printk("[jon]tc=NULL!\n");
	  	return;
  	}

	dev = ndev;

	// 1. stop the GMAC
	netif_stop_queue(dev_for_receive);
	netif_carrier_off(dev_for_receive);
	TipsterDmaIntDisable(tc);
	TipsterDmaRxStop(tc);
	TipsterDmaTxStop(tc);
	dma_free_tx_skbs(tc);
	disable_irq(dev_for_receive->irq);
	//receive(0);

	// 2. restart the gmac after N secs..
	handle_abnormal_timer.expires = jiffies + RESTART_GMAC_JIFFIES;
	handle_abnormal_timer.data = pr;
	handle_abnormal_timer.function = abnormal_timer_func;
	add_timer(&handle_abnormal_timer);

}

static int tipster_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	//printk("\n[NET]We will restart ethernet \"%d\" sec(s) after abnormal rx..\n\n", RESTART_GMAC_SEC) ;
	printk("%d\n", RESTART_GMAC_SEC) ;
	return 0 ;
}

static int tipster_write_proc(struct file *file, const char __user *buf,
                       unsigned long count, void *data)
{
    char num[10], *p;
    int nr, len;

    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(num) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(num, buf, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }

    /* atoi() */
    p = num;
    len = count;
	nr = 0 ;
	do {
        unsigned int c = *p - '0';
        if (*p == '\n') {
            break;
        }
        if (c > 9) {
            printk("%c is not digital\n", *p);
            return -EINVAL;
        }
        nr = nr * 10 + c;
        p++;
    } while (--len);

	if(nr == 0) {
		printk("[ERR] Sorry, you cannot set it to 0!!\n") ;
		//RESTART_GMAC_SEC = 1 ;
	}
	else {
		RESTART_GMAC_SEC = nr ;
	}

    return count;
}


static int finish_xmit( struct net_device *dev )   /* finish packet transmision started by start_xmit */
{
  Private *pr;
  Tipster *tc;
  int r;

  if(dev == NULL)
     return -1;

  TR( KERN_INFO "Tipster::finish_xmit(%s)\n", dev->name );

  pr = (Private *)dev->priv;
  if(pr == NULL)
    return -1;

  tc = &pr->tipster;
  if(tc == NULL)
    return -1;

  do                          /* handle transmit descriptors */
  {
    u32 data1;
    u32 status;
    u32 len;

    r = TipsterDmaTxGet( tc, &status, NULL, &len, &data1 );   /* get tx descriptor content */
    if(  r >= 0 && data1 != 0 )
    {
      TR( KERN_DEBUG "Tipster::finish_xmit(%s) - get tx descriptor %d for skb %08x, status = %08x\n",
                          dev->name, r, data1, status );
      dev_kfree_skb_irq((struct sk_buff *)data1);

      if( TipsterDmaTxValid(status) )
      {
        tc->stats.tx_bytes += len;
        tc->stats.tx_packets++;
      }
      else
      {
		printk("** tx sts err = 0x%x\n", status);
        tc->stats.tx_errors++;
        tc->stats.tx_aborted_errors += TipsterDmaTxAborted(status);
        tc->stats.tx_carrier_errors += TipsterDmaTxCarrier(status);
      }
      tc->stats.collisions += TipsterDmaTxCollisions(status);
    }

  } while( r >= 0 );     /* while we have transmitted descriptors */

  return 0;
}

static DECLARE_TASKLET(tipster, receive, 0);
static DECLARE_TASKLET(tipster_abnormal, abnormal_tasklet_func, 0);

static irqreturn_t int_handler( int int_num, void *dev_id)  /* device interrupt handler */
{
  struct net_device *dev = (struct net_device *)dev_id;
  Private *pr = NULL;
  Tipster *tc = NULL;
  u32 irq = 0;

  static u32 Interrupt_Without_RxAbnormal;

  if( dev == NULL )
  {
    printk( KERN_ERR "Tipster::int_handler - unknown device\n" );
    return IRQ_NONE;
  }

  TR( KERN_DEBUG "Tipster::int_handler(%s)\n", dev->name );
  //printk("status = 0x%0lx\n", v_inl(0xf9801014));
  //printk("rgmii = 0x%0lx\n", v_inl(0xf98000d8));
  v_inl(0xf98000d8);

  pr = (Private *)dev->priv;
  if(pr == NULL)
       return IRQ_NONE;

  tc = &pr->tipster;
  if(tc == NULL)
     return IRQ_NONE;

  irq = TipsterDmaIntType(tc);    /* get mask of current interrupt requests */
  while( irq != 0 )
  {
    TR( KERN_DEBUG "Tipster::int_handler(%s) - irq = %08x\n", dev->name, irq );

    if( irq & TipsterDmaError )      /* fatal error condition (bus error) */
    {
      printk("<1> \n Fatal Bus Error Interrupt Seen\n");
      TR( KERN_DEBUG "Tipster::int_handler(%s) - fatal error\n", dev->name );
    }

    if( irq & TipsterDmaRxNormal )   /* Normal receiver interrupt */
    {
      TR( KERN_DEBUG "Tipster::int_handler(%s) - rx normal\n", dev->name );
	  //printk("Tipster::int_handler(%s) - rx normal\n", dev->name );
      dev_for_receive = dev;
      tasklet_schedule(&tipster);
    }

    if( irq & TipsterDmaRxAbnormal ) /* Abnormal receiver interrupt */
    {
	  u32 interrupt_value;

      TR( KERN_DEBUG "Tipster::int_handler(%s) - rx abnormal\n", dev->name );
      /* no room for received packet - ignore */
      printk(KERN_DEBUG "Abnormal Rx Interrupt Seen\n");
      tc->stats.rx_over_errors++;
	  TipsterDmaRxStop(tc);
	  interrupt_value = readl( tc->dmaBase + DMAMMR_INTERRUPT ) ;
      Interrupt_Without_RxAbnormal = interrupt_value & ~(DMA_RxAbnormal_Bit) ;
      writel( Interrupt_Without_RxAbnormal, tc->dmaBase + DMAMMR_INTERRUPT ) ;
      trans_busy = 1 ;
      dev_for_receive = dev;
        //put the task into the schedule
      tasklet_schedule(&tipster_abnormal);

    }

    if( irq & TipsterDmaRxStopped )  /* Receiver went to stopped state */
    {
		u32 interrupt_value;

        printk(KERN_DEBUG "Rx Stopped Interrupt Seen\n");
        TipsterDmaRxStop( tc ) ;
        //turn off interrupt
        interrupt_value = readl( tc->dmaBase + DMAMMR_INTERRUPT ) ;
        Interrupt_Without_RxAbnormal = interrupt_value & ~(1<<8) ;
        writel( Interrupt_Without_RxAbnormal, tc->dmaBase + DMAMMR_INTERRUPT ) ;
        trans_busy = 1 ;
        dev_for_receive = dev;
        //put the task into the schedule
        tasklet_schedule(&tipster_abnormal);
    }

    if( irq & TipsterDmaTxNormal )   /* Normal transmitter interrupt */
    {
      TR( KERN_DEBUG "Tipster::int_handler(%s) - tx normal\n", dev->name );
      finish_xmit(dev); /* finish packet transmision started by start_xmit */
    }

    if( irq & TipsterDmaTxAbnormal ) /* Abnormal transmitter interrupt */
    {
      printk("<1> \n Abnormal Tx Interrupt Seen\n");
      TR( KERN_DEBUG "Tipster::int_handler(%s) - tx abnormal\n", dev->name );
      finish_xmit(dev);
    }

    if( irq & TipsterDmaTxStopped )  /* Transmitter went to stopped state */
    {
      printk("<1> \n Tx Stopped Interrupt Seen\n");
      TR( KERN_DEBUG "Tipster::int_handler(%s) - tx stopped\n", dev->name );
    }

    irq = TipsterDmaIntType(tc);    /* read interrupt requests came during interrupt handling */
  }

  if ( trans_busy == 1 ) {
      TipsterWriteDmaReg( tc, DMAMMR_INTERRUPT, Interrupt_Without_RxAbnormal ); /* reenable interrupts without RxAbnormal */
  }
  else {
      TipsterDmaIntEnable(tc);         /* reenable interrupts */
  }

  TR( KERN_DEBUG "Tipster::int_handler(%s) - return\n", dev->name );
  return IRQ_HANDLED;
}

static void inline phy_reset(Tipster *tc)
{
	if ((TipsterMiiRead(tc, 0x2) == 0x001c) && ((TipsterMiiRead(tc, 0x3) & 0xFFF0) == 0xc912)) {
		TipsterMiiWrite(tc, GEN_ctl, 0x8000);//RTL8112b
	}
	else {
		TipsterMiiWrite(tc, GEN_ctl, 0x8000);
	}

	//wait reset done.
	while((TipsterMiiRead(tc, GEN_ctl) & 0x8000) != 0) ;
}

static void icp_set_speed(Tipster *tc, u32 speed) /* speed 0: 10Mbps, 1: 100Mbs, 2: 1000Mbps */
{
	u32 reg;
	u16 phy_reg;
	unsigned long tmp ;
	u32 sysc_prot_info;

	phy_reg = TipsterMiiRead(tc, GEN_ctl);
	phy_reg &= ~0x2040;

	if ((TipsterMiiRead(tc, 0x2) == 0x0362) && (TipsterMiiRead(tc, 0x3) == 0x5D12)) {
		//turn off AUTO-NEGO
		phy_reg &= ~(1 << 12) ;
	}

	//If Mozart325/330, we should trun off AN in case the PHY changes to 1Gbps when detecting the cable speed is 1Gbps.
	sysc_prot_info = readl(0xf4f8003c);
	if ((sysc_prot_info & 0x7) ==  0x5) {
		//turn off AUTO-NEGO
		phy_reg &= ~(1 << 12) ;
	}

	if(is_mozart_v1 && tc->rgmii_interface) {
		//monitor clk
		tmp = v_inl(0xf4f80030) ;
		tmp &= 0x00ff ;
		tmp |= 0x0200 ;
		v_outl(0xf4f80030, tmp) ;
	}

	if (speed == 2) {
		if(is_mozart_v1) {
			TipsterMiiWrite(tc, GEN_ctl, phy_reg|0x0140);
			(void)TipsterMiiRead(tc, 0x0);

			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
			reg &= ~0xc0000000;
			if(tc->rgmii_interface) {
				reg |= 0x00000000 ;
			}
			else {
				reg |= 0x40000000;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c); /* set GMAC Tx clock speed to 125MHz */

			clear_ugpio_pin(PIN_UGPIO4);
		}
		else {
			//Mozart V2
			TipsterMiiWrite(tc, GEN_ctl, phy_reg|0x0140);
			(void)TipsterMiiRead(tc, 0x0);

			/* bit[1] 0: internal clock source as tx_clk
			  		  1: external clock source */
		 	/* bit[2] 0: 3.3V IO 1: 2.5V IO*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
			reg &= ~(1 << 1) ;
			reg &= ~(1 << 2) ;
			if(tc->rgmii_interface) {
				reg |= (0 << 1) ;
				reg |= (1 << 2) ;
			}
			else {
				reg |= (0 << 1) ;
				reg |= (0 << 2) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);

			/* bit[31:30] 2'b00 : 125MHz
			              2'b01 : 25MHz
			              2'b10 : 2.5MHz
			              2'b11 : reserved
			   bit[23] 1'b0: Shift 180 degrees.
			   		   1'b1: Shift 90 degrees.
			*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
			reg &= ~(0x3 << 30);
			reg &= ~(1 << 23) ;
			if(tc->rgmii_interface) {
				reg |= (0 << 30) ;
				reg |= (1 << 23) ;
			}
			else {
				reg |= (0 << 30) ;
				reg |= (0 << 23) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
		}
	} else if (speed == 1) {
		if(is_mozart_v1) {
			TipsterMiiWrite(tc, GEN_ctl, phy_reg|0x2100);
			(void)TipsterMiiRead(tc, 0x0);

			if(tc->rgmii_interface)
				clear_ugpio_pin(PIN_UGPIO4);
			else
				set_ugpio_pin(PIN_UGPIO4);
		}
		else {
			//Mozart V2
			TipsterMiiWrite(tc, GEN_ctl, phy_reg|0x2100);
			(void)TipsterMiiRead(tc, 0x0);

			/* bit[1] 0: internal clock source as tx_clk
			  		  1: external clock source */
		 	/* bit[2] 0: 3.3V IO 1: 2.5V IO*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
			reg &= ~(1 << 1) ;
			reg &= ~(1 << 2) ;
			if(tc->rgmii_interface) {
				reg |= (0 << 1) ;
				reg |= (1 << 2) ;
			}
			else {
				reg |= (1 << 1) ;
				reg |= (0 << 2) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);

			/* bit[31:30] 2'b00 : 125MHz
			              2'b01 : 25MHz
			              2'b10 : 2.5MHz
			              2'b11 : reserved
			   bit[23] 1'b0: Shift 180 degrees.
			   		   1'b1: Shift 90 degrees.
			*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
			reg &= ~(0x3 << 30);
			reg &= ~(1 << 23) ;
			if(tc->rgmii_interface) {
				reg |= (1 << 30) ;
				reg |= (1 << 23) ;
			}
			else {
				reg |= (1 << 30) ;
				reg |= (0 << 23) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
		}
	} else if (speed == 0) {
		TipsterMiiWrite(tc, GEN_ctl, phy_reg|0x0100);
		(void)TipsterMiiRead(tc, 0x0);

		if(tc->rgmii_interface)
			clear_ugpio_pin(PIN_UGPIO4);
		else
			set_ugpio_pin(PIN_UGPIO4);
	}
}

static void rtl_set_speed(Tipster *tc, u32 speed) /* speed 0: 10Mbps, 1: 100Mbs, 2: 1000Mbps */
{
	u32 reg;
	u16 phy_reg;
	unsigned long tmp ;

	if(is_mozart_v1) {
		//monitor clk
		tmp = v_inl(0xf4f80030) ;
		tmp &= 0x00ff ;
		tmp |= 0x0200 ;
		v_outl(0xf4f80030, tmp) ;

		//set tx clk dir
		//set_ugpio_pin(PIN_UGPIO4);
		//printk("uartpin = 0x%08lx\n", v_inl(0xf4a00014)) ;
		clear_ugpio_pin(PIN_UGPIO4);
		//printk("uartpin = 0x%08lx\n", v_inl(0xf4a00014)) ;

		if (speed == 2) {//1000Mbps
			//tx clk speed
			tmp = v_inl(0xf4f8002c) ;
			tmp &= ~(0xC0000000) ;
			tmp |= 0x00000000 ;//rgmii
			v_outl(0xf4f8002c, tmp) ;
			//phy
			TipsterMiiWrite(tc, 0x00, 0x1140);
			tmp = TipsterMiiRead(tc, 0x00);
		} else if (speed == 1) {//100Mbps
			//tx clk speed
			tmp = v_inl(0xf4f8002c) ;
			tmp &= ~(0xC0000000) ;
			tmp |= 0x80000000 ;
			v_outl(0xf4f8002c, tmp) ;
			//phy
			TipsterMiiWrite(tc, 0x00, 0x2100);
			tmp = TipsterMiiRead(tc, 0x00);
		} else if (speed == 0) {//10Mbps
			//tx clk speed
			tmp = v_inl(0xf4f8002c) ;
			tmp &= ~(0xC0000000) ;
			tmp |= 0xC0000000 ;
			v_outl(0xf4f8002c, tmp) ;
			//phy
			TipsterMiiWrite(tc, 0x00, 0x0100);
			tmp = TipsterMiiRead(tc, 0x00);
		}
	}
	else {
		//Mozart v2
		phy_reg = TipsterMiiRead(tc, GEN_ctl);
		phy_reg &= ~0x2040;

		unsigned long sysc_prot_info = readl(0xf4f8003c);
		//if m330, turn off Auto-Negotiation.
		if((sysc_prot_info & 0x7) == 0x5) {
			phy_reg &= ~(1 << 12) ;
		}

		if(speed == 2) {
			if(tc->rgmii_interface)
				TipsterMiiWrite(tc, GEN_ctl, phy_reg | 0x1140);
			else
				TipsterMiiWrite(tc, GEN_ctl, phy_reg | 0x0140);
			(void)TipsterMiiRead(tc, 0x0);

			/* bit[1] 0: internal clock source as tx_clk
			  		  1: external clock source */
		 	/* bit[2] 0: 3.3V IO 1: 2.5V IO*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
			reg &= ~(1 << 1) ;
			reg &= ~(1 << 2) ;
			if(tc->rgmii_interface) {
				reg |= (0 << 1) ;
				reg |= (1 << 2) ;
			}
			else {
				reg |= (0 << 1) ;
				reg |= (0 << 2) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);

			/* bit[31:30] 2'b00 : 125MHz
			              2'b01 : 25MHz
			              2'b10 : 2.5MHz
			              2'b11 : reserved
			   bit[23] 1'b0: Shift 180 degrees.
			   		   1'b1: Shift 90 degrees.
			*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
			reg &= ~(0x3 << 30);
			reg &= ~(1 << 23) ;
			if(tc->rgmii_interface) {
				reg |= (0 << 30) ;
				reg |= (1 << 23) ;
			}
			else {
				reg |= (0 << 30) ;
				reg |= (0 << 23) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
		}
		else if(speed == 1) {
			//Mozart V2
			TipsterMiiWrite(tc, GEN_ctl, phy_reg | 0x2100);
			(void)TipsterMiiRead(tc, 0x0);

			/* bit[1] 0: internal clock source as tx_clk
			  		  1: external clock source */
		 	/* bit[2] 0: 3.3V IO 1: 2.5V IO*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
			reg &= ~(1 << 1) ;
			reg &= ~(1 << 2) ;
			if(tc->rgmii_interface) {
				reg |= (0 << 1) ;
				reg |= (1 << 2) ;
			}
			else {
				reg |= (1 << 1) ;
				reg |= (0 << 2) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);

			/* bit[31:30] 2'b00 : 125MHz
			              2'b01 : 25MHz
			              2'b10 : 2.5MHz
			              2'b11 : reserved
			   bit[23] 1'b0: Shift 180 degrees.
			   		   1'b1: Shift 90 degrees.
			*/
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
			reg &= ~(0x3 << 30);
			reg &= ~(1 << 23) ;
			if(tc->rgmii_interface) {
				reg |= (1 << 30) ;
				reg |= (1 << 23) ;
			}
			else {
				reg |= (1 << 30) ;
				reg |= (0 << 23) ;
			}
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x2c);
		}
	}
}

static struct timer_list phy_sts_timer;

static void icp1001_link_changed_wq(struct work_struct *work)
{
	Tipster *tc = NULL;
	Private *pri = NULL;
	struct net_device *dev;

	tc = container_of(work, Tipster, link_changed_work);
	pri = container_of(tc, Private, tipster);
	dev = ndev;

	if (tc->linkup == 1) {
		u16 reg;
		int i;

		reg = TipsterMiiRead(tc, 0x11);

		tc->linkup = (reg&0x8000) != 0;
		printk("GMAC Link Up ");
		if ((reg&0x6000) == 0x4000) {
			if(tc->rgmii_interface) {
				//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
				v_outl(0xf9800000, 0x01200d0c) ;
			}
			else
				v_outl(0xf9800000, 0x00000c0c) ;
			printk("1000BASE-T ");
			tc->speed_1000 = 1;
			tc->speed_100 = 0;
			tc->speed_10 = 0;
			icp_set_speed(tc, 2);
		} else if ((reg&0x6000) == 0x2000) {
			if(tc->rgmii_interface) {
				//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
				v_outl(0xf9800000, 0x0120cd0c) ;
			}
			else
				v_outl(0xf9800000, 0x0000cc0c) ;
			printk("100BASE-T ");
			tc->speed_1000 = 0;
			tc->speed_100 = 1;
			tc->speed_10 = 0;
			icp_set_speed(tc, 1);
		} else if ((reg&0x6000) == 0) {
			printk("10BASE-T ");
			tc->speed_1000 = 0;
			tc->speed_100 = 0;
			tc->speed_10 = 1;
			icp_set_speed(tc, 0);
		} else {
			printk("Unknown Speed ");
		}
		tc->fdx = (reg&0x1000) != 0;
		printk("%s\n", tc->fdx? "FullDuplex" : "HalfDuplex");

		TipsterDmaInit( tc ) ;
		TipsterMacInit(tc, dev->dev_addr, dev->broadcast);       /* Init Tipster MAC module */
		for(i = 0; i < NET_RX_DESC_NUM; i++){
			TipsterDmaRxSet( tc, rxBufHandle + (i *(BUFFER_SIZE)), BUFFER_SIZE, NULL);
  		}
		TipsterDmaIntClear(tc);    /* clear interrupt requests */
		TipsterDmaIntEnable(tc);   /* enable all interrupts */
		TipsterDmaRxStart(tc);     /* start receiver */
		TipsterDmaTxStart(tc);     /* start transmitter, it must go to suspend immediately */
		netif_carrier_on(dev);
		netif_start_queue(dev);
		enable_irq(dev->irq);
	}
	else {
		printk("GMAC Link Down\n");

		netif_stop_queue(dev);
		netif_carrier_off(dev);
		TipsterDmaIntDisable(tc);  /* disable all interrupts */
		TipsterDmaRxStop(tc);      /* stop receiver, take ownership of all rx descriptors */
		TipsterDmaTxStop(tc);      /* stop transmitter, take ownership of all tx descriptors */
		dma_free_tx_skbs(tc);
		disable_irq(dev->irq);
		receive(0);
	}

	tc->linkchanged = 0;
	phy_sts_timer.expires = jiffies + 20;
	phy_sts_timer.data = dev;
	add_timer(&phy_sts_timer);
}


static void icp101_link_changed_wq(struct work_struct *work)
{
	Tipster *tc = NULL;
	Private *pri = NULL;
	struct net_device *dev;

	tc = container_of(work, Tipster, link_changed_work);
	pri = container_of(tc, Private, tipster);
	dev = ndev;

	if (tc->linkup == 1) {
		u16 reg;
		int i;

		//SW-Reset + Restart-AN
		TipsterMiiWrite(tc, GEN_ctl, 0x9200);
		//Check SW-Reset finished.
		while((TipsterMiiRead(tc, GEN_ctl) & 0x8000) != 0) ;
		//Check Restart-AN finished.
		volatile int auto_nego_timeout = 5000 ;
		while(!(TipsterMiiRead(tc, 0x1) & (1 << 5))) {
			mdelay(1) ;
			auto_nego_timeout-- ;
			//If AN cannot finish its work before timeout,
			//we just leave the loop.
			if(auto_nego_timeout == 0) {
				break ;
			}
		}

		reg = TipsterMiiRead(tc, 0x01);
		tc->linkup = (reg & (1 << 2)) != 0;
		printk("GMAC Link Up ");
		printk("100BASE-T ");
		tc->speed_1000 = 0;
		tc->speed_100 = 1;
		tc->speed_10 = 0;

		tc->fdx = (TipsterMiiRead(tc, 0x00)&(1 << 8)) != 0;
		printk("%s\n", tc->fdx? "FullDuplex" : "HalfDuplex");

		TipsterDmaInit( tc ) ;
		TipsterMacInit(tc, dev->dev_addr, dev->broadcast);       /* Init Tipster MAC module */
		for(i = 0; i < NET_RX_DESC_NUM; i++){
			TipsterDmaRxSet( tc, rxBufHandle + (i *(BUFFER_SIZE)), BUFFER_SIZE, NULL);
  		}
		TipsterDmaIntClear(tc);    /* clear interrupt requests */
		TipsterDmaIntEnable(tc);   /* enable all interrupts */
		TipsterDmaRxStart(tc);     /* start receiver */
		TipsterDmaTxStart(tc);     /* start transmitter, it must go to suspend immediately */
		netif_carrier_on(dev);
		netif_start_queue(dev);
		enable_irq(dev->irq);
	}
	else {
		printk("GMAC Link Down\n");

		netif_stop_queue(dev);
		netif_carrier_off(dev);
		TipsterDmaIntDisable(tc);  /* disable all interrupts */
		TipsterDmaRxStop(tc);      /* stop receiver, take ownership of all rx descriptors */
		TipsterDmaTxStop(tc);      /* stop transmitter, take ownership of all tx descriptors */
		dma_free_tx_skbs(tc);
		disable_irq(dev->irq);
		receive(0);
	}

	tc->linkchanged = 0;
	phy_sts_timer.expires = jiffies + 20;
	phy_sts_timer.data = dev;
	add_timer(&phy_sts_timer);
}


static void rtl_link_changed_wq(struct work_struct *work)
{
	Tipster *tc = NULL;
	Private *pri = NULL;
	struct net_device *dev;

	tc = container_of(work, Tipster, link_changed_work);
	pri = container_of(tc, Private, tipster);
	dev = ndev;

	if (tc->linkup == 1) {
		u16 reg;
		int i;

		reg = TipsterMiiRead(tc, 0x11);

		tc->linkup = (reg&0x0400) != 0;
		printk("GMAC Link Up ");
		if ((reg&0xC000) == 0x8000) {
			if(tc->rgmii_interface) {
				//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
				v_outl(0xf9800000, 0x01200d0c) ;
			}
			else
				v_outl(0xf9800000, 0x00000c0c) ;

			printk("1000BASE-T ");
			tc->speed_1000 = 1;
			tc->speed_100 = 0;
			tc->speed_10 = 0;
			rtl_set_speed(tc, 2);
		} else if ((reg&0xC000) == 0x4000) {
			if(tc->rgmii_interface) {
				//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
				v_outl(0xf9800000, 0x0120cd0c) ;
			}
			else
				v_outl(0xf9800000, 0x0000cc0c) ;

			printk("100BASE-T ");
			tc->speed_1000 = 0;
			tc->speed_100 = 1;
			tc->speed_10 = 0;
			rtl_set_speed(tc, 1);
		} else if ((reg&0xC000) == 0) {
			printk("10BASE-T ");
			tc->speed_1000 = 0;
			tc->speed_100 = 0;
			tc->speed_10 = 1;
			rtl_set_speed(tc, 0);
		} else {
			printk("Unknown Speed ");
		}
		tc->fdx = (reg&0x2000) != 0;
		printk("%s\n", tc->fdx? "FullDuplex" : "HalfDuplex");

		TipsterDmaInit( tc ) ;
		TipsterMacInit(tc, dev->dev_addr, dev->broadcast);       /* Init Tipster MAC module */
		for(i = 0; i < NET_RX_DESC_NUM; i++){
			TipsterDmaRxSet( tc, rxBufHandle + (i *(BUFFER_SIZE)), BUFFER_SIZE, NULL);
  		}
		TipsterDmaIntClear(tc);    /* clear interrupt requests */
		TipsterDmaIntEnable(tc);   /* enable all interrupts */
		TipsterDmaRxStart(tc);     /* start receiver */
		TipsterDmaTxStart(tc);     /* start transmitter, it must go to suspend immediately */
		netif_carrier_on(dev);
		netif_start_queue(dev);
		enable_irq(dev->irq);
	}
	else {
		printk("GMAC Link Down\n");

		netif_stop_queue(dev);
		netif_carrier_off(dev);
		TipsterDmaIntDisable(tc);  /* disable all interrupts */
		TipsterDmaRxStop(tc);      /* stop receiver, take ownership of all rx descriptors */
		TipsterDmaTxStop(tc);      /* stop transmitter, take ownership of all tx descriptors */
		dma_free_tx_skbs(tc);
		disable_irq(dev->irq);
		receive(0);
	}

	tc->linkchanged = 0;
	phy_sts_timer.expires = jiffies + 20;
	phy_sts_timer.data = dev;
	add_timer(&phy_sts_timer);
}


//Only for IC+1001
static void icp1001_phy_fn(unsigned long data) /* IC+ phy status polling function */
{
	u16 phy_sts, link;
	Tipster *tc = &pr.tipster;

	phy_sts = TipsterMiiRead(tc, 0x11);

	link = (phy_sts&0x8000) != 0;
	if (link != tc->linkup) {
		tc->linkchanged = 1;
		tc->linkup = link;
		schedule_work(&tc->link_changed_work);
	}

	if (tc->linkchanged == 0) {
		phy_sts_timer.expires = jiffies + 20;
		phy_sts_timer.data = data;
		add_timer(&phy_sts_timer);
	}
}

//Only for IC+101
static void icp101_phy_fn(unsigned long data) /* IC+ phy status polling function */
{
	u16 phy_sts, link;
	Tipster *tc = &pr.tipster;

	phy_sts = TipsterMiiRead(tc, 0x01) ;

	link = ((phy_sts & (1 << 2)) != 0);
	if (link != tc->linkup) {
		tc->linkchanged = 1;
		tc->linkup = link;
		schedule_work(&tc->link_changed_work);
	}

	if (tc->linkchanged == 0) {
		phy_sts_timer.expires = jiffies + 20;
		phy_sts_timer.data = data;
		add_timer(&phy_sts_timer);
	}
}


//Only for RTL8112b
static void rtl_phy_fn(unsigned long data)
{
	u16 phy_sts, link;
	Tipster *tc = &pr.tipster;

	phy_sts = TipsterMiiRead(tc, 0x11);

	link = (phy_sts&0x0400) != 0;
	if (link != tc->linkup) {
		tc->linkchanged = 1;
		tc->linkup = link;
		schedule_work(&tc->link_changed_work);
	}

	if (tc->linkchanged == 0) {
		phy_sts_timer.expires = jiffies + 20;
		phy_sts_timer.data = data;
		add_timer(&phy_sts_timer);
	}
}

int GMAC_mdio_read(struct net_device *dev, int phy_id, int location)
{
  Private *pr ;
  Tipster *tc ;
  pr = (Private *)dev->priv;
  tc = &pr->tipster;

  return TipsterMiiRead(tc, location);
}

void GMAC_mdio_write(struct net_device *dev, int phy_id, int location, int val)
{
  Private *pr ;
  Tipster *tc ;
  pr = (Private *)dev->priv;
  tc = &pr->tipster;

  TipsterMiiWrite(tc, location, val);
}

static int check_rgmii_in_v2(void)
{
	char *cp, *end;
	unsigned int is_rgmii = 0 ;

	for (cp = boot_command_line; *cp; ) {
		if (memcmp(cp, "rgmii=", 6) == 0) {
			cp += 6;
			is_rgmii = memparse(cp, &end);
			if (end != cp)
				break;
			cp = end;
		} else {
			while (*cp != ' ' && *cp)
				++cp;
			while (*cp == ' ')
				++cp;
		}
	}

	return is_rgmii ;
}

static int open( struct net_device *dev )
{
  Private *pr = NULL;
  Tipster *tc = NULL;
  int r = 0;
  int i = 0;
  struct proc_dir_entry *ent;

  is_mozart_v1 = (v_inl(0xf4f80014) == MOZART_V1_VERSION) ;

  if(dev == NULL)
     return -1;

  TR( KERN_NOTICE "Tipster::open(%s)\n", dev->name );

  MOD_INC_USE_COUNT;
  pr = (Private *)dev->priv;
  if(pr == NULL)
     return -1;

  tc = &pr->tipster;
  if(tc == NULL)
    return -1;


  pr->mii.dev = dev;
  pr->mii.mdio_read = GMAC_mdio_read;
  pr->mii.mdio_write = GMAC_mdio_write;
  pr->mii.phy_id_mask = 0x1f;
  pr->mii.reg_num_mask = 0x1f;
  pr->mii.phy_id = 1;

  phy_reset(tc);
  msleep(3000); /* till phy reset done */

  init_timer(&phy_sts_timer);
  phy_sts_timer.data = dev;
  phy_sts_timer.expires = jiffies + 20;

  //[jam] initialize handle_abnormal_timer
  RESTART_GMAC_SEC = 1 ;
  init_timer(&handle_abnormal_timer);
  ent = create_proc_entry("tipster_restart_time", S_IFREG | S_IRWXU, NULL);
  if (!ent) {
  	printk("create proc failed\n");
  } else {
    //ent->write_proc = tipster_write_proc;
    ent->read_proc = tipster_read_proc;
	ent->write_proc = tipster_write_proc;
  }

  if ((TipsterMiiRead(tc, 0x2) == ICPLUS_IP1001_ID_1) && ((TipsterMiiRead(tc, 0x3) & 0xFFFFFFF0) == (ICPLUS_IP1001_ID_2 & 0xFFFFFFF0))) {
  		if(is_mozart_v1) {
			u16 phy_sts;
			u32 sysc_prot_info;

			sysc_prot_info = readl(0xf4f8003c);

			if ((sysc_prot_info & 0x7) !=  0x5) { /* NOT Mozart325 */
		        phy_sts_timer.function = icp1001_phy_fn;
				pr->mii.supports_gmii = 1;

				tc->rgmii_interface = (v_inl(0xf4f80034) & 0x4) != 0;
				if(tc->rgmii_interface) {
					printk("Ethernet PHY: IC+ IP1001(RGMii)\n");
				}
				else {
					printk("Ethernet PHY: IC+ IP1001\n");
				}

				phy_sts = TipsterMiiRead(tc, 0x11);
				tc->linkup = (phy_sts&0x8000) != 0;
				tc->linkchanged = 0;
				if (tc->linkup)
					printk("GMAC Link Up ");
				else
					printk("GMAC Link Down\n");

				if ((phy_sts&0x6000) == 0x4000) {
					if(tc->rgmii_interface) {
						//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
						v_outl(0xf9800000, 0x01200d0c) ;
					}
					tc->speed_1000 = 1;
					tc->speed_100 = 0;
					tc->speed_10 = 0;
					icp_set_speed(tc, 2);
					if (tc->linkup)
						printk("1000BASE-T ");
				} else if ((phy_sts&0x6000) == 0x2000) {
					if(tc->rgmii_interface) {
						//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
						v_outl(0xf9800000, 0x0120cd0c) ;
					}
					tc->speed_1000 = 0;
					tc->speed_100 = 1;
					tc->speed_10 = 0;
					icp_set_speed(tc, 1);
					if (tc->linkup)
						printk("100BASE-T ");
				} else if ((phy_sts&0x6000) == 0) {
					if(tc->rgmii_interface) {
						//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
						v_outl(0xf9800000, 0x01208d0c) ;
					}
					tc->speed_1000 = 0;
					tc->speed_100 = 0;
					tc->speed_10 = 1;
					icp_set_speed(tc, 0);
					if (tc->linkup)
						printk("10BASE-T");
				}

				tc->fdx = (phy_sts&0x1000) != 0;
				if (tc->linkup)
					printk("%s\n", (tc->fdx)? "FullDuplex" : "HalfDuplex");
				INIT_WORK(&tc->link_changed_work, icp1001_link_changed_wq);

				r = request_ugpio_pin(PIN_UGPIO4);
				if (r < 0)
					printk("GMAC clock direction switch pin may be already captured!\n");
			} else { /* fix IP1001 link at 100Mbps on Mozart325 */
		  		printk("Ethernet PHY: IC+ IP1001 (Fixed at 100Mbps)\n");
				tc->linkup = 1;
				tc->linkchanged = 0;
				tc->speed_10 = 0;
				tc->speed_100 = 1;
				tc->speed_1000 = 0;
				tc->fdx = 1;

				r = request_ugpio_pin(PIN_UGPIO4);
				if (r < 0)
					printk("GMAC clock direction switch pin may be already captured!\n");

				TipsterMiiWrite(tc, GEN_ctl, 0x2100);
				(void)TipsterMiiRead(tc, 0x0);
				set_ugpio_pin(PIN_UGPIO4);
			}
  		}
		else {
			//Mozart v2
			u16 phy_sts;
			u32 sysc_prot_info;

			sysc_prot_info = readl(0xf4f8003c);
			if ((sysc_prot_info & 0x7) !=  0x5) { //if Mozart325/330, force 100Mbps.
	            phy_sts_timer.function = icp1001_phy_fn;
				pr->mii.supports_gmii = 1;

				tc->rgmii_interface = check_rgmii_in_v2() ;
				if(tc->rgmii_interface) {
					printk("Ethernet PHY: IC+ IP1001(RGMii)\n");
				}
				else {
					printk("Ethernet PHY: IC+ IP1001\n");
				}

				phy_sts = TipsterMiiRead(tc, 0x11);
				tc->linkup = (phy_sts&0x8000) != 0;
				tc->linkchanged = 0;
				if (tc->linkup)
					printk("GMAC Link Up ");
				else
					printk("GMAC Link Down\n");

				if ((phy_sts&0x6000) == 0x4000) {
					if(tc->rgmii_interface) {
						//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
						v_outl(0xf9800000, 0x01200d0c) ;
					}
					else
						v_outl(0xf9800000, 0x00000c0c) ;
					tc->speed_1000 = 1;
					tc->speed_100 = 0;
					tc->speed_10 = 0;
					icp_set_speed(tc, 2);
					if (tc->linkup)
						printk("1000BASE-T ");
				} else if ((phy_sts&0x6000) == 0x2000) {
					if(tc->rgmii_interface) {
						//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
						v_outl(0xf9800000, 0x0120cd0c) ;
					}
					else
						v_outl(0xf9800000, 0x0000cc0c) ;
					tc->speed_1000 = 0;
					tc->speed_100 = 1;
					tc->speed_10 = 0;
					icp_set_speed(tc, 1);
					if (tc->linkup)
						printk("100BASE-T ");
				}

				tc->fdx = (phy_sts&0x1000) != 0;
				if (tc->linkup)
					printk("%s\n", (tc->fdx)? "FullDuplex" : "HalfDuplex");
				INIT_WORK(&tc->link_changed_work, icp1001_link_changed_wq);
			}
			else {
				//force as 100Mbps.
				tc->rgmii_interface = 0 ;
				tc->linkup = 1;
				tc->linkchanged = 0;
				tc->speed_10 = 0;
				tc->speed_100 = 1;
				tc->speed_1000 = 0;
				tc->fdx = 1;
				printk("Ethernet PHY: IC+ IP1001\n");
				printk("100BASE-T ");
				v_outl(0xf9800000, 0x0000cc0c) ;
				icp_set_speed(tc, 1);
			}
		}
  }
  else if ((TipsterMiiRead(tc, 0x2) == REALTEK_RTL8112C_ID_1) && ((TipsterMiiRead(tc, 0x3) & 0xFFF0) == REALTEK_RTL8112C_ID_2)) {
  	if(is_mozart_v1) {
        printk("Ethernet PHY: Realtek RTL8211(RGMii)\n");
		u16 phy_sts;

        phy_sts_timer.function = rtl_phy_fn;
		pr->mii.supports_gmii = 1;

		phy_sts = TipsterMiiRead(tc, 0x11);
		tc->linkup = (phy_sts&0x0400) != 0;
		tc->linkchanged = 0;
		if (tc->linkup)
			printk("GMAC Link Up ");
		else
			printk("GMAC Link Down\n");

		if ((phy_sts&0xC000) == 0x8000) {
			//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
			v_outl(0xf9800000, 0x01200d0c) ;
			tc->speed_1000 = 1;
			tc->speed_100 = 0;
			tc->speed_10 = 0;
			rtl_set_speed(tc, 2);
			if (tc->linkup)
				printk("1000BASE-T ");
		} else if ((phy_sts&0xC000) == 0x4000) {
			//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
			v_outl(0xf9800000, 0x0120cd0c) ;
			tc->speed_1000 = 0;
			tc->speed_100 = 1;
			tc->speed_10 = 0;
			rtl_set_speed(tc, 1);
			if (tc->linkup)
				printk("100BASE-T ");
		} else if ((phy_sts&0xC000) == 0) {
			//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
			v_outl(0xf9800000, 0x01208d0c) ;
			tc->speed_1000 = 0;
			tc->speed_100 = 0;
			tc->speed_10 = 1;
			rtl_set_speed(tc, 0);
			if (tc->linkup)
				printk("10BASE-T");
		}

		tc->fdx = (phy_sts&0x2000) != 0;
		if (tc->linkup)
			printk("%s\n", (tc->fdx)? "FullDuplex" : "HalfDuplex");
		INIT_WORK(&tc->link_changed_work, rtl_link_changed_wq);
		tc->rgmii_interface = 1;

		r = request_ugpio_pin(PIN_UGPIO4);
		if (r < 0)
			printk("GMAC clock direction switch pin may be already captured!\n");
	}
	else {
		//Mozart v2
		u16 phy_sts;
		u32 sysc_prot_info;

		sysc_prot_info = readl(0xf4f8003c);
		if ((sysc_prot_info & 0x7) !=  0x5) { //if Mozart325/330, force 100Mbps.
            phy_sts_timer.function = rtl_phy_fn;
			pr->mii.supports_gmii = 1;

			tc->rgmii_interface = check_rgmii_in_v2() ;
			if(tc->rgmii_interface) {
				printk("Ethernet PHY: Realtek8211(RGMii)\n");
			}
			else {
				printk("Ethernet PHY: Realtek8211\n");
			}

			mdelay(1000) ;
			phy_sts = TipsterMiiRead(tc, 0x11);
			tc->linkup = (phy_sts&0x0400) != 0;
			tc->linkchanged = 0;
			if (tc->linkup)
				printk("GMAC Link Up ");
			else
				printk("GMAC Link Down\n");

			if ((phy_sts&0xC000) == 0x8000) {
				if(tc->rgmii_interface) {
					//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
					v_outl(0xf9800000, 0x01200d0c) ;
				}
				else
					v_outl(0xf9800000, 0x00000c0c) ;
				tc->speed_1000 = 1;
				tc->speed_100 = 0;
				tc->speed_10 = 0;
				rtl_set_speed(tc, 2);
				if (tc->linkup)
					printk("1000BASE-T ");
			} else if ((phy_sts&0xC000) == 0x4000) {
				if(tc->rgmii_interface) {
					//In rgmii mode, we have to setup gmac properly before reset dma.(hw limit of gmac)
					v_outl(0xf9800000, 0x0120cd0c) ;
				}
				else
					v_outl(0xf9800000, 0x0000cc0c) ;
				tc->speed_1000 = 0;
				tc->speed_100 = 1;
				tc->speed_10 = 0;
				rtl_set_speed(tc, 1);
				if (tc->linkup)
					printk("100BASE-T ");
			}

			tc->fdx = (phy_sts&0x2000) != 0;
			if (tc->linkup)
				printk("%s\n", (tc->fdx)? "FullDuplex" : "HalfDuplex");
			INIT_WORK(&tc->link_changed_work, rtl_link_changed_wq);
		}
		else {
force_rtl_100mbps:
			//force as 100Mbps.
			phy_sts_timer.function = NULL ;
			tc->rgmii_interface = 0 ;
			tc->linkup = 1;
			tc->linkchanged = 0;
			tc->speed_10 = 0;
			tc->speed_100 = 1;
			tc->speed_1000 = 0;
			tc->fdx = 1;
			printk("Ethernet PHY: Realtek8211\n");
			printk("100BASE-T ");
			v_outl(0xf9800000, 0x0000cc0c) ;
			rtl_set_speed(tc, 1);
		}
	}
  }
  else if (((TipsterMiiRead(tc, 0x2) == ICPLUS_IP101_ID_1) && (TipsterMiiRead(tc, 0x3) == ICPLUS_IP101_ID_2)) ||
           ((TipsterMiiRead(tc, 0x2) == MICREL_KSZ8051_ID_1) && ((TipsterMiiRead(tc, 0x3) & 0xFFF0) == MICREL_KSZ8051_ID_2))) {
  		printk("Ethernet PHY: %s\n", (TipsterMiiRead(tc, 0x2) == ICPLUS_IP101_ID_1)?"IC+ IP101":"MICREL KSZ8051");

		//SW-Reset + Restart-AN
		TipsterMiiWrite(tc, GEN_ctl, 0x9200);
		//Check SW-Reset finished.
		while((TipsterMiiRead(tc, GEN_ctl) & 0x8000) != 0) ;
		//Check Restart-AN finished.
		volatile int auto_nego_timeout = 5000 ;
		while(!(TipsterMiiRead(tc, 0x1) & (1 << 5))) {
			mdelay(1) ;
			auto_nego_timeout-- ;
			//If AN cannot finish its work before timeout,
			//we just leave the loop.
			if(auto_nego_timeout == 0) {
				break ;
			}
		}

		if(is_mozart_v1) {
			tc->linkup = 1;
			tc->linkchanged = 0;
			tc->speed_10 = 0;
			tc->speed_100 = 1;
			tc->speed_1000 = 0;
			tc->fdx = 1;
		}
		else {
			phy_sts_timer.function = icp101_phy_fn;
			pr->mii.supports_gmii = 1;

			tc->linkchanged = 0;
			tc->speed_10 = 0;
			tc->speed_100 = 1;
			tc->speed_1000 = 0;

			if(TipsterMiiRead(tc, 0x01) & (1 << 2)) {
				printk("[Ethernet] Link Up!\n") ;
				tc->linkup = 1 ;
			}
			else {
				printk("[Ethernet] Link Down!\n") ;
				tc->linkup = 0 ;
			}

			if(tc->linkup) {
				if(TipsterMiiRead(tc, GEN_ctl) & (1 << 8)) {
					printk("[Ethernet] Full Duplex!\n") ;
					tc->fdx = 1;
				}
				else {
					printk("[Ethernet] Half Duplex!\n") ;
					tc->fdx = 0;
				}
			}
			else {
				printk("[Ethernet] Set Ethernet MAC as Full-Duplex mode by default.\n") ;
				tc->fdx = 1;
			}

			unsigned long reg ;
			reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
			reg |= 0x2; /* bit[1] 0: internal clock source as tx_clk
					  		  	  1: external clock source */
			reg &= ~0x4; /* bit[2] 0: 3.3V IO 1: 2.5V IO*/
			outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);

			INIT_WORK(&tc->link_changed_work, icp101_link_changed_wq);
		}
  }
  else if ((TipsterMiiRead(tc, 0x2) == BROADCOM_BCM54616_ID_1) && (TipsterMiiRead(tc, 0x3) == BROADCOM_BCM54616_ID_2)) {

  		printk("Ethernet PHY: BROADCOM BCM54616\n");
		if((readl(0xf4f80034) & (1 << 5)) != 0) { //external clock -- 10/100Mbps
			printk("              (We force it to 100Mbps mode)\n") ;
			tc->linkup = 1;
			tc->linkchanged = 0;
			tc->speed_10 = 0;
			tc->speed_100 = 1;
			tc->speed_1000 = 0;
			tc->fdx = 1;
			icp_set_speed(tc, 1);
			msleep(5000); //delay for new setting ready.
		}
		else {//internal clock -- 1000Mbps
			printk("              (We force it to 1000Mbps mode)\n") ;
			tc->linkup = 1;
			tc->linkchanged = 0;
			tc->speed_10 = 0;
			tc->speed_100 = 0;
			tc->speed_1000 = 1;
			tc->fdx = 1;
			//icp_set_speed(tc, 2);
			TipsterMiiWrite(tc, 0x09, 0x300);
			TipsterMiiWrite(tc, 0x00, 0x1200);
			msleep(5000); //delay for new setting ready.
		}
  }
  else {
		printk("Unknown PHY transceiver\n");
		/* give it a default value, but may not work */
		tc->linkup = 1;
		tc->linkchanged = 0;
		tc->speed_10 = 0;
		tc->speed_100 = 1;
		tc->speed_1000 = 0;
		tc->fdx = 1;
  }

  TR( KERN_DEBUG "Tipster::open(%s) - request interrupt %d\n", dev->name, dev->irq );
  TR( KERN_DEBUG "Tipster::open(%s) - init DMA\n", dev->name );
  if( TipsterDmaInit( tc ) != 0 )
  {
    printk( KERN_ERR "Tipster::open(%s) - cannot init DMA\n", dev->name );
    MOD_DEC_USE_COUNT ;
    return -ENODEV;
  }

  TR( KERN_DEBUG "Tipster::open(%s) - init MAC\n", dev->name );
  if( TipsterMacInit( tc, dev->dev_addr, dev->broadcast ) != 0 )       /* Init Tipster MAC module */
  {
    printk( KERN_ERR "Tipster::open(%s) - cannot init MAC\n", dev->name );
    return -ENODEV;
  }

  dev->features = NETIF_F_IP_CSUM; /* hw can only support TCP, UDP, ICMP checksum offload */

  for(i = 0; i < NET_RX_DESC_NUM; i++){
	r = TipsterDmaRxSet( tc, rxBufHandle + (i *(BUFFER_SIZE)), BUFFER_SIZE, NULL);
  }
  TipsterDmaIntClear(tc);    /* clear interrupt requests */
  TipsterDmaIntEnable(tc);   /* enable all interrupts */

  if( request_irq(dev->irq, &int_handler, IRQF_DISABLED, dev->name, dev) != 0 )     /* request non-shared interrupt */
  {
    printk( KERN_ERR "Tipster::open(%s) - interrupt %d request fail\n", dev->name, dev->irq );
    MOD_DEC_USE_COUNT ;
    return -ENODEV;
  }

  TipsterDmaRxStart(tc);     /* start receiver */
  TipsterDmaTxStart(tc);     /* start transmitter, it must go to suspend immediately */

  netif_start_queue(dev);

  if (tc->linkup == 0) {
	disable_irq(dev->irq);
	phy_reset(tc);
	netif_stop_queue(dev);
	netif_carrier_off(dev);
  }

  if (phy_sts_timer.function != NULL)
	 add_timer(&phy_sts_timer);

  return 0;
}

static int stop( struct net_device *dev )
{
  Private *pr = NULL;
  Tipster *tc = NULL;

  trans_busy = 0 ;

  if(dev == NULL)
       return -1;

  TR( KERN_INFO "Tipster::stop(%s)\n", dev->name );
  MOD_DEC_USE_COUNT;
  pr = (Private *)dev->priv;
  if(pr == NULL)
        return -1;

  tc = &pr->tipster;
  if(tc == NULL)
     return -1;

  if(phy_sts_timer.function != NULL)
      del_timer_sync(&phy_sts_timer);
  free_ugpio_pin(PIN_UGPIO4);

  netif_stop_queue(dev);

  TipsterDmaIntDisable(tc);  /* disable all interrupts */
  TipsterDmaRxStop(tc);      /* stop receiver, take ownership of all rx descriptors */
  TipsterDmaTxStop(tc);      /* stop transmitter, take ownership of all tx descriptors */
  dma_free_tx_skbs(tc);

  free_irq(dev->irq, dev);    /* disconnect from irq */

  remove_proc_entry("tipster_restart_time", NULL);

  return 0;
}

int tx_crc = 0;
int tx_runt = 0;
int tx_both = 0;
int tx_numerr = 0;
int tx_len = 0;

static int hard_start_xmit( struct sk_buff *skb, struct net_device *dev )
{
  Private *pr = NULL;
  Tipster *tc = NULL;
  int r = 0;
  unsigned long flags;

  if(dev == NULL){
  	printk("Null dev!\n");
      return -EBUSY;
  }
  if(skb == NULL){
  	printk("Null skb!\n");
     return 1;
  }

  TR( KERN_INFO "Tipster::hard_start_xmit(%s)\n", dev->name );

  pr = (Private *)dev->priv;
  if(pr == NULL){
  	printk("Null pri!\n");
     return -EBUSY;
  }

  tc = &pr->tipster;
  if(tc == NULL){
  	printk("Null tip!\n");
     return -EBUSY;
  }

  spin_lock_irqsave(&tc->lock, flags);
  if(0)
  {
    int n;
    for( n=0; n<0x60; n+=4 )
      TR( KERN_DEBUG "CSR%d = %08x\n", n/4, readl(tc->dmaBase+n) );
  }

  if (early_stop_netif_stop_queue(dev) != 0)
  {
    if( jiffies - dev->trans_start < TIMEOUT ) return -EBUSY;

    printk( KERN_ERR "Tipster::hard_start_xmit(%s) - tx timeout expired, restart\n", dev->name );
    stop(dev);
    open(dev);
    dev->trans_start = jiffies;
  }

  dmac_flush_range(skb->data, skb->data + skb->len);

  r = TipsterDmaTxSet( tc, ( virt_to_bus( skb->data ) ), skb->len, (u32)skb, skb->ip_summed );
  if( r < 0 )
  {
    printk(KERN_DEBUG "Tipster::hard_start_xmit(%s) - no more free tx descriptors\n", dev->name );
    spin_unlock_irqrestore(&tc->lock, flags);
	return NETDEV_TX_BUSY;
  }

  TipsterDmaTxResume(tc);      /* resume transmitter */
  dev->trans_start = jiffies;

  spin_unlock_irqrestore(&tc->lock, flags);
  return 0;
}

static int do_ioctl( struct net_device *dev, struct ifreq *ifr, int cmd )
{
  Private *pr = NULL;
  Tipster *tc = NULL;
  struct mii_ioctl_data *req;

  if(ifr == NULL)
      return -1;

  req = (struct mii_ioctl_data *)&ifr->ifr_data;
  if(dev == NULL)
    return -1;

  TR( KERN_INFO "Tipster::do_ioctl(%s, cmd=%08x)\n", dev->name, cmd );

  pr = (Private *)dev->priv;

  if(pr == NULL)
    return -1;

  tc = &pr->tipster;

  if(tc == NULL)
     return -1;

  return generic_mii_ioctl(&pr->mii, req, cmd, NULL);
}

static struct net_device_stats *get_stats(struct net_device *dev)
{
  Private *pr = NULL;
  Tipster *tc = NULL;

  if(dev == NULL)
     return NULL;

  TR( KERN_INFO "Tipster::get_stats(%s)\n", dev->name );

  pr = (Private *)dev->priv;
  if( pr == NULL)
     return NULL;

  tc = &pr->tipster;
  if(tc == NULL)
     return NULL;

  return &tc->stats;
}

static int multicast_filter_limit = 32;

static void GMAC_set_multicast_list(struct net_device *dev)
{
  Private *pr ;
  Tipster *tc ;
  pr = (Private *)dev->priv;
  tc = &pr->tipster;

  if (dev->flags & IFF_PROMISC) { /* receive all incoming frames */
    GMACMacClearHash(tc);
    GMACMacPromiscuous(tc);
  }
  /* when hash table is not able to filter multicast frames or the IFF_ALLMULTI flag set, receive all multicast frames */
  else if ((dev->mc_count > multicast_filter_limit)
      || (dev->flags & IFF_ALLMULTI)) {
    GMACMacClearHash(tc);
    GMACMacPassAllMulticast(tc);
  } else {
    int i;
    struct dev_mc_list *mclist;

    for (i = 0, mclist = dev->mc_list; mclist && i < dev->mc_count; i++, mclist = mclist->next) {
      int bit_nr = (ether_crc(6, mclist->dmi_addr)^0xffffffff)>>26;
      GMACMacAddHash(tc, bit_nr);
    }
    GMACMacHashMulticast(tc);
  }

}

#define DRV_MODULE_NAME		"DW GMAC"
#define DRV_MODULE_VERSION	"1.0"
static void gmac_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strcpy(info->driver, DRV_MODULE_NAME);
	strcpy(info->version, DRV_MODULE_VERSION);
}

static int gmac_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	Private *pr = (Private *)dev->priv;
	return mii_ethtool_gset(&pr->mii, cmd);
}

static int gmac_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	Private *pr = (Private *)dev->priv;
	return mii_ethtool_sset(&pr->mii, cmd);
}

static u32 gmac_get_link(struct net_device *dev)
{
	Private *pr = (Private *)dev->priv;
	return mii_link_ok(&pr->mii);
}

static int gmac_nway_reset(struct net_device *dev)
{
	Private *pr = (Private *)dev->priv;
	return mii_nway_restart(&pr->mii);
}

static struct ethtool_ops gmac_ethtool_ops = {
	.get_drvinfo	= gmac_get_drvinfo,
	.get_settings	= gmac_get_settings,
	.set_settings	= gmac_set_settings,
	.nway_reset		= gmac_nway_reset,
	.get_link		= gmac_get_link,
};

static int probe( struct net_device *dev, u32 Addr )  /* probe device at specified location or find the device */
{
  u32 configBase = 0;
  u32 macBase    = 0;
  u32 dmaBase    = 0;
  u32 phyAddr    = DEFAULT_PHY_ADDRESS;
  u8  macAddr[6] = DEFAULT_MAC_ADDRESS;

  if(dev == NULL)
     return -1;

  ndev = dev;

  TR( KERN_DEBUG "Tipster::probe(%p, %08x)\n", dev, Addr );
  macBase = IO_ADDRESS(MOZART_GMAC_MMR_BASE);
  dmaBase = IO_ADDRESS(MOZART_GMAC_MMR_BASE) + 0x1000;
  dev->irq = GMAC_IRQ_NUM;
  dev->base_addr = 0x0;

  dev->priv = &pr;
  TipsterInit( &(pr.tipster), configBase, macBase, dmaBase, phyAddr ); /* init Tipster internal data */

  memcpy( dev->dev_addr, macAddr, dev->addr_len );       /* Set MAC address */

//jon alloc non-cacheable memory
  pr.rxDesc = dma_alloc_consistent( NULL, sizeof(DmaDesc)*NET_RX_DESC_NUM, &(pr.rxDmaDesc) );
  pr.txDesc = dma_alloc_consistent( NULL, sizeof(DmaDesc)*NET_TX_DESC_NUM, &(pr.txDmaDesc) );

  rxBuffer = dma_alloc_consistent( NULL, (BUFFER_SIZE)*NET_RX_DESC_NUM, &rxBufHandle );

  /* ethtool support */
  dev->ethtool_ops = &gmac_ethtool_ops;
  dev->open = &open;
  dev->hard_start_xmit = &hard_start_xmit;
  dev->stop = &stop;
  dev->do_ioctl = &do_ioctl;
  dev->get_stats = &get_stats;
  dev->set_multicast_list = &GMAC_set_multicast_list;
  if(debug) dev->flags |= IFF_DEBUG;

  return 0;
}



/* external entry point to the driver, called from Space.c to detect a card */
/* L&T*/
struct net_device * __init tipster_probe(int unit)
{
  struct net_device *dev; //jon
  static int probed;
  u32 addr = 0;
  u32 err = 0; //jon


  dev = alloc_etherdev(sizeof(struct PrivateStruct));
  if (!dev){
	printk("[jon]alloc ether dev fail!\n");
	return ERR_PTR(-ENOMEM);
  }
  if (unit >= 0) {
	sprintf(dev->name, "eth%d", unit);
	netdev_boot_setup_check(dev);
  }

  if( probed ) return ERR_PTR(-ENODEV);
  probed = 1;

  if( probe(dev, addr) == 0){
		err = register_netdev(dev);
		if (!err){
			return dev;
		}
		free_netdev(dev);
  }

  return ERR_PTR(-ENODEV);
}
