#ifndef __GAMC_BEETHOVEN_H__
#define __GAMC_BEETHOVEN_H__

#include <phy.h>

extern volatile unsigned long DMA_BUFFER_BASE;
#define DMA_BUFFER_SIZE 1536
#define TX_DESC_COUNT 256
#define RX_DESC_COUNT 256
#define DESC_COUNT (TX_DESC_COUNT+RX_DESC_COUNT)

typedef int bool;

typedef struct    /* The structure is common for both receive and transmit descriptors */
{                               /* put it here to allow the caller to know its size */
	unsigned long   status;         /* Status */
	unsigned long   length;         /* Buffer length */
	unsigned long   buffer1;        /* Buffer 1 pointer */
	unsigned long   buffer2;        /* Buffer 2 pointer or next descriptor pointer in chain structure */
	unsigned long	desc4;          /* reserved in Tx descriptors, extend status in Rx descriptors */
	unsigned long	desc5;		/* reserved */
	unsigned long	desc6;		/* reserved */
	unsigned long	desc7;		/* reserved */
} TDmaDesc;

typedef struct        /* Tipster device data */
{
	unsigned long	configBase;	/* base address of Config registers */
	unsigned long	macBase;	/* base address of MAC registers */
	unsigned long	dmaBase;	/* base address of DMA registers */
	unsigned long	phyAddr;	/* PHY device address on MII interface */
	TDmaDesc 	*tx;		/* start of TX descriptors ring */
	TDmaDesc 	*rx;		/* start of RX descriptors ring */
	unsigned long  	rxCount;	/* number of rx descriptors */
	unsigned long  	txCount;    	/* number of tx descriptors */
	unsigned long  	txBusy;		/* first descriptor owned by DMA engine, moved by DmaTxGet */
	unsigned long  	txNext;      	/* next available tx descriptor, moved by DmaTxSet */
	unsigned long  	rxBusy;      	/* first descripror owned by DMA engine, moved by DmaRxGet */
	unsigned long  	rxNext;      	/* next available rx descriptor, moved by DmaRxSet */ //CPU available!

	phy_interface_t	interface;
	struct mii_dev	*mdio_bus;
	struct phy_device	*phydev;
} TGMAC;

typedef struct
{
	unsigned char	buf[DMA_BUFFER_SIZE];
} TDmaBuff;

typedef struct
{
	TGMAC gmac;
	TDmaDesc *desc;
	unsigned long tx_desc_base;
	unsigned long rx_desc_base;
	TDmaBuff *tx_buff_list;
	TDmaBuff *rx_buff_list;
} TPrivate;

enum GmacRegisters              /* GMAC registers, base address is BAR+GmacRegistersBase */
{
	GmacConfig         = 0x00,    /* config */
	GmacFrameFilter    = 0x04,    /* frame filter */
	GmacHashHigh       = 0x08,    /* multi-cast hash table high */
	GmacHashLow        = 0x0C,    /* multi-cast hash table low */
	GmacGmiiAddr       = 0x10,    /* GMII address */
	GmacGmiiData       = 0x14,    /* GMII data */
	GmacFlowControl    = 0x18,    /* Flow control */
	GmacVlan           = 0x1C,    /* VLAN tag */
	GmacAddr0High      = 0x40,    /* address0 high */
	GmacAddr0Low       = 0x44,    /* address0 low */
	GmacAddr1High      = 0x48,    /* address1 high */
	GmacAddr1Low       = 0x4C,    /* address1 low */
	GmacAddr2High      = 0x50,    /* address2 high */
	GmacAddr2Low       = 0x54,    /* address2 low */
	GmacAddr3High      = 0x58,    /* address3 high */
	GmacAddr3Low       = 0x5C,    /* address3 low */
	GmacAddr4High      = 0x60,    /* address4 high */
	GmacAddr4Low       = 0x64,    /* address4 low */
	GmacAddr5High      = 0x68,    /* address5 high */
	GmacAddr5Low       = 0x6C,    /* address5 low */
	GmacAddr6High      = 0x70,    /* address6 high */
	GmacAddr6Low       = 0x74,    /* address6 low */
	GmacAddr7High      = 0x78,    /* address7 high */
	GmacAddr7Low       = 0x7C,    /* address7 low */
	GmacAddr8High      = 0x80,    /* address8 high */
	GmacAddr8Low       = 0x84,    /* address8 low */
	GmacAddr9High      = 0x88,    /* address9 high */
	GmacAddr9Low       = 0x8C,    /* address9 low */
	GmacAddr10High     = 0x90,    /* address10 high */
	GmacAddr10Low      = 0x94,    /* address10 low */
	GmacAddr11High     = 0x98,    /* address11 high */
	GmacAddr11Low      = 0x9C,    /* address11 low */
	GmacAddr12High     = 0xA0,    /* address12 high */
	GmacAddr12Low      = 0xA4,    /* address12 low */
	GmacAddr13High     = 0xA8,    /* address13 high */
	GmacAddr13Low      = 0xAC,    /* address13 low */
	GmacAddr14High     = 0xB0,    /* address14 high */
	GmacAddr14Low      = 0xB4,    /* address14 low */
	GmacAddr15High     = 0xB8,    /* address15 high */
	GmacAddr15Low      = 0xBC,    /* address15 low */
	GmacSRGmiiStatus   = 0xD8,    /* SGmii/RGmii status */
};

enum DmaRegisters             /* DMA engine registers, base address is BAR+DmaRegistersBase */
{
	DmaBusMode        = 0x00,    /* CSR0 - Bus Mode */
	DmaTxPollDemand   = 0x04,    /* CSR1 - Transmit Poll Demand */
	DmaRxPollDemand   = 0x08,    /* CSR2 - Receive Poll Demand */
	DmaRxBaseAddr     = 0x0C,    /* CSR3 - Receive list base address */
	DmaTxBaseAddr     = 0x10,    /* CSR4 - Transmit list base address */
	DmaStatus         = 0x14,    /* CSR5 - Dma status */
	DmaControl        = 0x18,    /* CSR6 - Dma control */
	DmaInterrupt      = 0x1C,    /* CSR7 - Interrupt enable */
	DmaMissedFr       = 0x20,    /* CSR8 - Missed Frame Counter */
	DmaTxCurrAddr     = 0x50,    /* CSR20 - Current host transmit buffer address */
	DmaRxCurrAddr     = 0x54,    /* CSR21 - Current host receive buffer address */
};

enum GmacConfigReg      /* GMAC Config register layout */
{                                            /* Bit description                      R/W   Reset value */
	GmacWatchdogDisable      = 0x00800000,     /* Disable watchdog timer               RW                */
	GmacWatchdogEnable       = 0,              /* Enable watchdog timer                          0       */

	GmacJabberDisable        = 0x00400000,     /* Disable jabber timer                 RW                */
	GmacJabberEnable         = 0,              /* Enable jabber timer                            0       */

	GmacFrameBurstEnable     = 0x00200000,     /* Enable frame bursting                RW                */
	GmacFrameBurstDisable    = 0,              /* Disable frame bursting                         0       */

	GmacJumboFrameEnable     = 0x00100000,     /* Enable jumbo frame                   RW                */
	GmacJumboFrameDisable    = 0,              /* Disable jumbo frame                            0       */

	GmacInterFrameGap7       = 0x000E0000,     /* IFG Config7 - 40 bit times           RW        000     */
	GmacInterFrameGap6       = 0x000C0000,     /* IFG Config6 - 48 bit times                             */
	GmacInterFrameGap5       = 0x000A0000,     /* IFG Config5 - 56 bit times                             */
	GmacInterFrameGap4       = 0x00080000,     /* IFG Config4 - 64 bit times                             */
	GmacInterFrameGap3       = 0x00040000,     /* IFG Config3 - 72 bit times                             */
	GmacInterFrameGap2       = 0x00020000,     /* IFG Config2 - 80 bit times                             */
	GmacInterFrameGap1       = 0x00010000,     /* IFG Config1 - 88 bit times                             */
	GmacInterFrameGap0       = 000,            /* IFG Config0 - 96 bit times                             */

	GmacSelectMii            = 0x00008000,     /* Select MII mode                      RW                */
	GmacSelectGmii           = 0,              /* Select GMII mode                               0       */

	GmacFastEthernetSpeed100 = 0x00004000,
	GmacFastEthernetSpeed10  = 0,

	GmacDisableRxOwn         = 0x00002000,     /* Disable receive own packets          RW                */
	GmacEnableRxOwn          = 0,              /* Enable receive own packets                     0       */

	GmacLoopbackOn           = 0x00001000,     /* Loopback mode                        RW                */
	GmacLoopbackOff          = 0,              /* Normal mode                                    0       */

	GmacFullDuplex           = 0x00000800,     /* Full duplex mode                     RW                */
	GmacHalfDuplex           = 0,              /* Half duplex mode                               0       */

	GmacCsumOffloadOn  		 = 0x00000400,
	GmacCsumOffloadOff 		 = 0,

	GmacRetryDisable         = 0x00000200,     /* Disable retransmission               RW                */
	GmacRetryEnable          = 0,              /* Enable retransmission                          0       */

	GmacPadCrcStripEnable    = 0x00000080,     /* Pad / Crc stripping enable           RW                */
	GmacPadCrcStripDisable   = 0,              /* Pad / Crc stripping disable                    0       */

	GmacBackoffLimit3        = 0x00000060,     /* Back-off limit                       RW                */
	GmacBackoffLimit2        = 0x00000040,     /*                                                        */
	GmacBackoffLimit1        = 0x00000020,     /*                                                        */
	GmacBackoffLimit0        = 00,             /*                                                00      */

	GmacDeferralCheckEnable  = 0x00000010,     /* Deferral check enable                RW                */
	GmacDeferralCheckDisable = 0,              /* Deferral check disable                         0       */

	GmacTxEnable             = 0x00000008,     /* Transmitter enable                   RW                */
	GmacTxDisable            = 0,              /* Transmitter disable                            0       */

	GmacRxEnable             = 0x00000004,     /* Receiver enable                      RW                */
	GmacRxDisable            = 0,              /* Receiver disable                               0       */
};

enum GmacFrameFilterReg /* GMAC frame filter register layout */
{
	GmacFilterOff            = 0x80000000,     /* Receive all incoming packets         RW                */
	GmacFilterOn             = 0,              /* Receive filtered packets only                  0       */

	GmacSrcAddrFilterEnable  = 0x00000200,     /* Source Address Filter enable         RW                */
	GmacSrcAddrFilterDisable = 0,              /*                                                0       */

	GmacSrcInvAddrFilterEn   = 0x00000100,     /* Inverse Source Address Filter enable RW                */
	GmacSrcInvAddrFilterDis  = 0,              /*                                                0       */

	GmacPassControl3         = 0x000000C0,     /* Forwards control frames that pass AF RW                */
	GmacPassControl2         = 0x00000080,     /* Forwards all control frames                            */
	GmacPassControl1         = 0x00000040,     /* Does not pass control frames                           */
	GmacPassControl0         = 00,             /* Does not pass control frames                   00      */

	GmacBroadcastDisable     = 0x00000020,     /* Disable reception of broadcast frames RW               */
	GmacBroadcastEnable      = 0,              /* Enable broadcast frames                        0       */

	GmacMulticastFilterOff   = 0x00000010,     /* Pass all multicast packets           RW                */
	GmacMulticastFilterOn    = 0,              /* Pass filtered multicast packets                0       */

	GmacDestAddrFilterInv    = 0x00000008,     /* Inverse filtering for DA             RW                */
	GmacDestAddrFilterNor    = 0,              /* Normal filtering for DA                        0       */

	GmacMcastHashFilterOn    = 0x00000004,     /* perfom multicast hash filtering      RW                */
	GmacMcastHashFilterOff   = 0,              /* perfect filtering only                         0       */

	GmacUcastHashFilterOn    = 0x00000002,     /* Unicast Hash filtering only          RW                */
	GmacUcastHashFilterOff   = 0,              /* perfect filtering only                         0       */

	GmacPromiscuousModeOn    = 0x00000001,     /* Receive all valid packets            RW                */
	GmacPromiscuousModeOff   = 0,              /* Receive filtered packets only                  0       */
};

enum GmacGmiiAddrReg      /* GMII address register layout */
{
	GmiiDevMask    = 0x0000F800,     /* GMII device address */
	GmiiDevShift   = 11,

	GmiiRegMask    = 0x000007C0,     /* GMII register */
	GmiiRegShift   = 6,

	GmiiAppClk5    = 0x00000014,     /* Application Clock Range 250-300 MHz */
	GmiiAppClk4    = 0x00000010,     /*                         150-250 MHz */
	GmiiAppClk3    = 0x0000000C,     /*                         35-60 MHz */
	GmiiAppClk2    = 0x00000008,     /*                         20-35 MHz */
	GmiiAppClk1    = 0x00000004,     /*                         100-150 MHz */
	GmiiAppClk0    = 00,             /*                         60-100 MHz */

	GmiiWrite      = 0x00000002,     /* Write to register */
	GmiiRead       = 0,              /* Read from register */

	GmiiBusy       = 0x00000001,     /* GMII interface is busy */
};

enum GmacGmiiDataReg      /* GMII address register layout */
{
	GmiiDataMask   = 0x0000FFFF,     /* GMII Data */
};

enum GmacFlowControlReg  /* GMAC flow control register layout */
{                                          /* Bit description                        R/W   Reset value */
	GmacPauseTimeMask        = 0xFFFF0000,   /* PAUSE TIME field in the control frame  RW      0000      */
	GmacPauseTimeShift       = 16,

	GmacPauseLowThresh3      = 0x00000030,   /* threshold for pause tmr 256 slot time  RW        00      */
	GmacPauseLowThresh2      = 0x00000020,   /*                         144 slot time                    */
	GmacPauseLowThresh1      = 0x00000010,   /*                         28  slot time                    */
	GmacPauseLowThresh0      = 00,           /*                         4   slot time                    */

	GmacUnicastPauseFrameOn  = 0x00000008,   /* Detect pause frame with unicast addr.  RW                */
	GmacUnicastPauseFrameOff = 0,            /* Detect only pause frame with multicast addr.     0       */

	GmacRxFlowControlEnable  = 0x00000004,   /* Enable Rx flow control                 RW                */
	GmacRxFlowControlDisable = 0,            /* Disable Rx flow control                          0       */

	GmacTxFlowControlEnable  = 0x00000002,   /* Enable Tx flow control                 RW                */
	GmacTxFlowControlDisable = 0,            /* Disable flow control                             0       */

	GmacSendPauseFrame       = 0x00000001,   /* send pause frame                       RW        0       */
};

#define GmacSRGmiiLinkSpeedMask 0x6
enum GmacSRGmiiStatusReg
{
	GmacSRGmiiLinkUp       = 0x00000008,
	GmacSRGmiiLinkDown     = 0x00000000,

	GmacSRGmiiLinkSpeed0   = 0x00000000, //2.5MHz
	GmacSRGmiiLinkSpeed1   = 0x00000002, //25MHz
	GmacSRGmiiLinkSpeed2   = 0x00000004, //125MHz

	GmacSRGmiiLinkHalfDux  = 0x00000000,
	GmacSRGmiiLinkFullDux  = 0x00000001,
};

enum DmaBusModeReg         /* DMA bus mode register */
{						/* Bit description                        R/W   Reset value */
	DmaFixedBurstEnable	= 0x00010000,	/* Fixed Burst SINGLE, INCR4, INCR8 or INCR16               */
	DmaFixedBurstDisable	= 0x00000000,	/*             SINGLE, INCR                         0       */

	Dma8xPBLEnable		= 0x01000000,
	Dma8xPBLDisable		= 0x00000000,

	DmaBurstLength32	= 0x00002000,   /* Dma burst length = 32                  RW                */
	DmaBurstLength16	= 0x00001000,   /* Dma burst length = 16                                    */
	DmaBurstLength8		= 0x00000800,   /* Dma burst length = 8                                     */
	DmaBurstLength4		= 0x00000400,   /* Dma burst length = 4                                     */
	DmaBurstLength2		= 0x00000200,   /* Dma burst length = 2                                     */
	DmaBurstLength1		= 0x00000100,   /* Dma burst length = 1                                     */
	DmaBurstLength0		= 0x00000000,   /* Dma burst length = 0                             0       */

	DmaDescSize32		= 0x00000080,
	DmaDescSize16		= 0x00000000,

	DmaDescriptorSkip16	= 0x00000040,   /* number of dwords to skip               RW                */
	DmaDescriptorSkip8	= 0x00000020,   /* between two unchained descriptors                        */
	DmaDescriptorSkip4	= 0x00000010,   /*                                                          */
	DmaDescriptorSkip2	= 0x00000008,   /*                                                          */
	DmaDescriptorSkip1	= 0x00000004,   /*                                                          */
	DmaDescriptorSkip0	= 0x00000000,	/*                                                  0       */

	DmaResetOn		= 0x00000001,   /* Reset DMA engine                       RW                */
	DmaResetOff		= 0x00000000,	/*                                                  0       */
};

enum DmaStatusReg         /* DMA Status register */
{                                         /* Bit description                        R/W   Reset value */
	DmaLineIntfIntr         = 0x04000000,   /* Line interface interrupt               R         0       */
	DmaErrorBit2            = 0x02000000,   /* err. 0-data buffer, 1-desc. access     R         0       */
	DmaErrorBit1            = 0x01000000,   /* err. 0-write trnsf, 1-read transfr     R         0       */
	DmaErrorBit0            = 0x00800000,   /* err. 0-Rx DMA, 1-Tx DMA                R         0       */

	DmaTxState              = 0x00700000,   /* Transmit process state                 R         000     */
	DmaTxStopped            = 0x00000000,   /* Stopped                                                  */
	DmaTxFetching           = 0x00100000,   /* Running - fetching the descriptor                        */
	DmaTxWaiting            = 0x00200000,   /* Running - waiting for end of transmission                */
	DmaTxReading            = 0x00300000,   /* Running - reading the data from memory                   */
	DmaTxSuspended          = 0x00600000,   /* Suspended                                                */
	DmaTxClosing            = 0x00700000,   /* Running - closing descriptor                             */

	DmaRxState              = 0x000E0000,   /* Receive process state                  R         000     */
	DmaRxStopped            = 0x00000000,   /* Stopped                                                  */
	DmaRxFetching           = 0x00020000,   /* Running - fetching the descriptor                        */
	DmaRxWaiting            = 0x00060000,   /* Running - waiting for packet                             */
	DmaRxSuspended          = 0x00080000,   /* Suspended                                                */
	DmaRxClosing            = 0x000A0000,   /* Running - closing descriptor                             */
	DmaRxQueuing            = 0x000E0000,   /* Running - queuing the recieve frame into host memory     */

	DmaIntNormal            = 0x00010000,   /* Normal interrupt summary               RW        0       */
	DmaIntAbnormal          = 0x00008000,   /* Abnormal interrupt summary             RW        0       */

	DmaIntEarlyRx           = 0x00004000,   /* Early receive interrupt (Normal)       RW        0       */
	DmaIntBusError          = 0x00002000,   /* Fatal bus error (Abnormal)             RW        0       */
	DmaIntEarlyTx           = 0x00000400,   /* Early transmit interrupt (Abnormal)    RW        0       */
	DmaIntRxWdogTO          = 0x00000200,   /* Receive Watchdog Timeout (Abnormal)    RW        0       */
	DmaIntRxStopped         = 0x00000100,   /* Receive process stopped (Abnormal)     RW        0       */
	DmaIntRxNoBuffer        = 0x00000080,   /* Receive buffer unavailable (Abnormal)  RW        0       */
	DmaIntRxCompleted       = 0x00000040,   /* Completion of frame reception (Normal) RW        0       */
	DmaIntTxUnderflow       = 0x00000020,   /* Transmit underflow (Abnormal)          RW        0       */
	DmaIntRcvOverflow       = 0x00000010,   /* Receive Buffer overflow interrupt      RW        0       */
	DmaIntTxJabberTO        = 0x00000008,   /* Transmit Jabber Timeout (Abnormal)     RW        0       */
	DmaIntTxNoBuffer        = 0x00000004,   /* Transmit buffer unavailable (Normal)   RW        0       */
	DmaIntTxStopped         = 0x00000002,   /* Transmit process stopped (Abnormal)    RW        0       */
 	DmaIntTxCompleted       = 0x00000001,   /* Transmit completed (Normal)            RW        0       */
};

enum DmaControlReg        /* DMA control register */
{                                         /* Bit description                        R/W   Reset value */
	DmaStoreAndForward      = 0x00200000,   /* Store and forward                      RW        0       */
	DmaTxStart              = 0x00002000,   /* Start/Stop transmission                RW        0       */
 	DmaFwdErrorFrames       = 0x00000080,   /* Forward error frames                   RW        0       */
	DmaFwdUnderSzFrames     = 0x00000040,   /* Forward undersize frames               RW        0       */
	DmaTxSecondFrame        = 0x00000004,   /* Operate on second frame                RW        0       */
	DmaRxStart              = 0x00000002,   /* Start/Stop reception                   RW        0       */
};

enum  DmaInterruptReg     /* DMA interrupt enable register */
{                                         /* Bit description                        R/W   Reset value */
	DmaIeNormal            = DmaIntNormal     ,   /* Normal interrupt enable                 RW        0       */
 	DmaIeAbnormal          = DmaIntAbnormal   ,   /* Abnormal interrupt enable               RW        0       */

	DmaIeEarlyRx           = DmaIntEarlyRx    ,   /* Early receive interrupt enable          RW        0       */
	DmaIeBusError          = DmaIntBusError   ,   /* Fatal bus error enable                  RW        0       */
	DmaIeEarlyTx           = DmaIntEarlyTx    ,   /* Early transmit interrupt enable         RW        0       */
	DmaIeRxWdogTO          = DmaIntRxWdogTO   ,   /* Receive Watchdog Timeout enable         RW        0       */
	DmaIeRxStopped         = DmaIntRxStopped  ,   /* Receive process stopped enable          RW        0       */
	DmaIeRxNoBuffer        = DmaIntRxNoBuffer ,   /* Receive buffer unavailable enable       RW        0       */
	DmaIeRxCompleted       = DmaIntRxCompleted,   /* Completion of frame reception enable    RW        0       */
	DmaIeTxUnderflow       = DmaIntTxUnderflow,   /* Transmit underflow enable               RW        0       */
	DmaIeRxOverflow        = DmaIntRcvOverflow,   /* Receive Buffer overflow interrupt       RW        0       */
	DmaIeTxJabberTO        = DmaIntTxJabberTO ,   /* Transmit Jabber Timeout enable          RW        0       */
	DmaIeTxNoBuffer        = DmaIntTxNoBuffer ,   /* Transmit buffer unavailable enable      RW        0       */
	DmaIeTxStopped         = DmaIntTxStopped  ,   /* Transmit process stopped enable         RW        0       */
	DmaIeTxCompleted       = DmaIntTxCompleted,   /* Transmit completed enable               RW        0       */
};

enum DmaDescriptorStatus    /* status word of DMA descriptor */
{
	DescOwnByDma		= 0x80000000,   /* Descriptor is owned by DMA engine  */
	DescError		= 0x00008000,

	DescTxIntEnable		= 0x40000000,
	DescTxLast		= 0x20000000,
	DescTxFirst		= 0x10000000,
	DescTxCRCDisable	= 0x08000000,
	DescTxPadDisable	= 0x04000000,
	DescTXIPCsumEnable	= 0x00400000,
	DescTxPayloadCsumEn	= 0x00800000,
	DescTxEndOfRing		= 0x00200000,
	DescTxChain		= 0x00100000,
	DescTxIPHdrErr		= 0x00010000,
	DescTxError		= 0x00008000,
	DescTxTimeout		= 0x00004000,   /* Tx - Transmit jabber timeout                      E  */
	DescTxFrameFlushed	= 0x00002000,   /* Tx - DMA/MTL flushed the frame due to SW flush    I  */
	DescTxPayloadErr	= 0x00001000,
	DescTxLostCarrier	= 0x00000800,   /* Tx - carrier lost during tramsmission             E  */
	DescTxNoCarrier		= 0x00000400,   /* Tx - no carrier signal from the tranceiver        E  */
	DescTxLateCollision	= 0x00000200,   /* Tx - transmission aborted due to collision        E  */
	DescTxExcCollisions	= 0x00000100,   /* Tx - transmission aborted after 16 collisions     E  */
	DescTxVLANFrame		= 0x00000080,   /* Tx - VLAN-type frame                                 */
	DescTxCollMask		= 0x00000078,   /* Tx - Collision count                                 */
	DescTxCollShift		= 3,
	DescTxExcDeferral	= 0x00000004,   /* Tx - excessive deferral                           E  */
	DescTxUnderflow		= 0x00000002,   /* Tx - late data arrival from the memory            E  */
	DescTxDeferred		= 0x00000001,   /* Tx - frame transmision deferred                      */


	DescDAFilterFail	= 0x40000000,   /* Rx - DA Filter Fail for the received frame        E  */
	DescFrameLengthMask	= 0x3FFF0000,   /* Receive descriptor frame length */
	DescFrameLengthShift	= 16,
	DescRxError 		= 0x00008000,   /* Error summary bit  - OR of the following bits:    v  */
	DescRxTruncated		= 0x00004000,   /* Rx - no more descriptors for receive frame        E  */
	DescSAFilterFail 	= 0x00002000,   /* Rx - SA Filter Fail for the received frame        E  */
	DescRxLengthError	= 0x00001000,   /* Rx - frame size not matching with length field    E  */
	DescRxDamaged		= 0x00000800,   /* Rx - frame was damaged due to buffer overflow     E  */
	DescRxVLANTag		= 0x00000400,   /* Rx - received frame is a VLAN frame               I  */
	DescRxFirst		= 0x00000200,   /* Rx - first descriptor of the frame                I  */
	DescRxLast		= 0x00000100,   /* Rx - last descriptor of the frame                 I  */
	DescRxLongFrame		= 0x00000080,   /* Rx - frame is longer than 1518 bytes              E  */
	DescRxCollision		= 0x00000040,   /* Rx - late collision occurred during reception     E  */
	DescRxFrameEther	= 0x00000020,   /* Rx - Frame type - Ethernet, otherwise 802.3          */
	DescRxWatchdog		= 0x00000010,   /* Rx - watchdog timer expired during reception      E  */
	DescRxMiiError		= 0x00000008,   /* Rx - error reported by MII interface              E  */
	DescRxDribbling		= 0x00000004,   /* Rx - frame contains noninteger multiple of 8 bits    */
	DescRxCrc		= 0x00000002,   /* Rx - CRC error                                    E  */
	DescRxExtSts		= 0x00000001,
};

enum DmaDescriptorLength    /* length word of DMA descriptor */
{
	DescTxSize1Mask		= 0x00001fff,
	DescTxSize1Shift	= 0,
	DescTxSize2Mask		= 0x1fff0000,
	DescTxSize2Shift	= 16,

	DescRxIntDisable	= 0x80000000,
	DescRxSize2Mask		= 0x1fff0000,
	DescRxSize2Shift	= 16,
	DescRxEndOfRing		= 0x00008000,
	DescRxChain		= 0x00004000,
	DescRxSize1Mask		= 0x00001fff,
	DescRxSize1Shift	= 0,
};

/**********************************************************
 * Initial register values
 **********************************************************/
enum InitialRegisters
{
	/* Full-duplex mode with perfect filter on */
	GmacConfigInitFdx1000 = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstDisable | GmacJumboFrameDisable
			      | GmacSelectGmii     | GmacEnableRxOwn          | GmacLoopbackOff
			      | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
			      | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable
			      | GmacCsumOffloadOn,

	/* Full-duplex mode with perfect filter on */
	GmacConfigInitFdx100 = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstDisable  | GmacJumboFrameDisable
			     | GmacSelectMii      | GmacEnableRxOwn          | GmacLoopbackOff
			     | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
			     | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable | GmacFastEthernetSpeed100
			     | GmacCsumOffloadOn,

	/* Full-duplex mode with perfect filter on */
	GmacConfigInitFdx10 = GmacWatchdogEnable | GmacJabberEnable 	    | GmacJumboFrameDisable
			    | GmacSelectMii      | GmacEnableRxOwn          | GmacLoopbackOff
			    | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
			    | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable | GmacFastEthernetSpeed10
			    | GmacCsumOffloadOn,

	/* Full-duplex mode */
	// CHANGED: Pass control config, dest addr filter normal, added source address filter, multicast & unicast
	GmacFrameFilterInitFdx = GmacFilterOn           | GmacPassControl0      | GmacBroadcastDisable |  GmacSrcAddrFilterDisable /* disable broadcast frame to avoid TFTP command error */
			       | GmacMulticastFilterOn  | GmacDestAddrFilterNor | GmacMcastHashFilterOff
			       | GmacPromiscuousModeOff | GmacUcastHashFilterOff,
	/* Full-duplex mode */
	GmacFlowControlInitFdx = GmacUnicastPauseFrameOff | GmacRxFlowControlEnable | GmacTxFlowControlEnable,

	/* Full-duplex mode */
	GmacGmiiAddrInitFdx = GmiiAppClk2,

	/* Half-duplex mode with perfect filter on */
	// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,
	GmacConfigInitHdx1000 = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
			      | GmacSelectGmii     | GmacDisableRxOwn         | GmacLoopbackOff
			      | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
			      | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable,

	/* Half-duplex mode with perfect filter on */
	GmacConfigInitHdx110 = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
			     | GmacSelectMii      | GmacDisableRxOwn         | GmacLoopbackOff
			     | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
			     | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable,

	/* Half-duplex mode */
	GmacFrameFilterInitHdx
		= GmacFilterOn          | GmacPassControl0        | GmacBroadcastEnable | GmacSrcAddrFilterDisable
		| GmacMulticastFilterOn | GmacDestAddrFilterNor   | GmacMcastHashFilterOff
		| GmacUcastHashFilterOff| GmacPromiscuousModeOff,

	/* Half-duplex mode */
	GmacFlowControlInitHdx = GmacUnicastPauseFrameOff | GmacRxFlowControlDisable | GmacTxFlowControlDisable,

	/* Half-duplex mode */
	GmacGmiiAddrInitHdx = GmiiAppClk2,

	/* Little-endian mode */
	// CHANGED: Removed Endian configuration
	DmaBusModeInit = DmaDescSize32 | DmaBurstLength16 | DmaDescriptorSkip0 | DmaResetOff,

	/* 1000 Mb/s mode */
	DmaControlInit1000 = DmaStoreAndForward       | DmaTxSecondFrame |0x00000040,

	/* 100 Mb/s mode */
	DmaControlInit100 = DmaStoreAndForward |0x00000040,

	/* 10 Mb/s mode */
	DmaControlInit10 = DmaStoreAndForward |0x00000040,

	/* Interrupt groups */
	DmaIntErrorMask         = DmaIntBusError,           /* Error */
	DmaIntRxAbnMask         = DmaIntRxNoBuffer,         /* receiver abnormal interrupt */
	DmaIntRxNormMask        = DmaIntRxCompleted,        /* receiver normal interrupt   */
	DmaIntRxStoppedMask     = DmaIntRxStopped,          /* receiver stopped */
	DmaIntTxAbnMask         = DmaIntTxUnderflow,        /* transmitter abnormal interrupt */
	DmaIntTxNormMask        = DmaIntTxCompleted,        /* transmitter normal interrupt */
	DmaIntTxStoppedMask     = DmaIntTxStopped,          /* receiver stopped */

	DmaIntEnable            = DmaIeNormal     | DmaIeAbnormal    | DmaIntErrorMask
				| DmaIntRxAbnMask | DmaIntRxNormMask | DmaIntRxStoppedMask
				| DmaIntTxAbnMask | DmaIntTxNormMask | DmaIntTxStoppedMask,
	DmaIntDisable           = 0,
};

extern int gmac_eth_initialize(int quiet) ;
int gmac_mdio_init(TGMAC *gc);

#endif
