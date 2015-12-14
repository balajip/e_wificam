/* Tipster PCI/Ethernet test chip driver for OS Linux
 *
 * Device-dependent (mostly system-independent) functions
 *
 * 1 Jun 1999 - G.Kazakov.
 */


#include "tipster_dev.h"

extern volatile Private pr;

//Use the slowest speed as default value.
static int MDC_CLK_DIV = 0x14 ;
static int mdc_clk_init_counter = 0 ;

static inline void *
pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
		     dma_addr_t *dma_handle)
{
	return dma_alloc_coherent(NULL, size, dma_handle, GFP_ATOMIC);
}


/**********************************************************
 * PCI registers - BASE ADDRESS 0 offsets
 **********************************************************/
enum ConfigRegisters          /* Non-standart configuration registers, base address is BAR+ConfigRegistersBase */
{
  PciToSipBase      = 0x00,
  SipToPciMemBase   = 0x04,
  SipToPciIoBase    = 0x08,
  PciConfigComplete = 0x0C,
  PciSipControl     = 0x10,
  PciSipIntEnable   = 0x14,
  PciSipIntStatus   = 0x18,
  PciSipReadCount   = 0x24,
  Bar0Mask          = 0x2C,
  Bar1Mask          = 0x34,
};

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

/**********************************************************
 * GMAC Network interface registers
 **********************************************************/

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

// CHANGE: Added on 07/28 SNPS  
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

//  CHANGE: Commented as Endian mode is not register configurable   
//  GmacBigEndian            = 0x00004000,     /* Big endian mode                    RW                */
//  GmacLittleEndian         = 0,              /* Little endian                                0       */
  GmacSpeed100Mbps		   = 0x00004000,
  GmacSpeed10Mbps		   = 0,

  GmacDisableRxOwn         = 0x00002000,     /* Disable receive own packets          RW                */
  GmacEnableRxOwn          = 0,              /* Enable receive own packets                     0       */

  GmacLoopbackOn           = 0x00001000,     /* Loopback mode                        RW                */
  GmacLoopbackOff          = 0,              /* Normal mode                                    0       */

  GmacFullDuplex           = 0x00000800,     /* Full duplex mode                     RW                */
  GmacHalfDuplex           = 0,              /* Half duplex mode                               0       */

  /* tassader: IPC bit */
  GmacChecksumOffloadOn    = 0x00000400,
  GmacChecksumOffloadOff   = 0,

  GmacRetryDisable         = 0x00000200,     /* Disable retransmission               RW                */
  GmacRetryEnable          = 0,              /* Enable retransmission                          0       */

//  CHANGE: Commented as Pad / CRC strip is one single bit  
//  GmacPadStripEnable       = 0x00000100,     /* Pad stripping enable               RW                */
//  GmacPadStripDisable      = 0,              /* Pad stripping disable                        0       */

// CHANGE: 07/28 renamed GmacCrcStrip* GmacPadCrcStrip* 
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

// CHANGE: Added on 07/28 SNPS 
  GmacSrcAddrFilterEnable  = 0x00000200,     /* Source Address Filter enable         RW                */
  GmacSrcAddrFilterDisable = 0,              /*                                                0       */

// CHANGE: Added on 07/28 SNPS  
  GmacSrcInvAddrFilterEn   = 0x00000100,     /* Inverse Source Address Filter enable RW                */
  GmacSrcInvAddrFilterDis  = 0,              /*                                                0       */

// CHANGE: Changed the control frame config (07/28)  
  GmacPassControl3         = 0x000000C0,     /* Forwards control frames that pass AF RW                */
  GmacPassControl2         = 0x00000080,     /* Forwards all control frames                            */
  GmacPassControl1         = 0x00000040,     /* Does not pass control frames                           */
  GmacPassControl0         = 00,             /* Does not pass control frames                   00      */

  GmacBroadcastDisable     = 0x00000020,     /* Disable reception of broadcast frames RW               */
  GmacBroadcastEnable      = 0,              /* Enable broadcast frames                        0       */

  GmacMulticastFilterOff   = 0x00000010,     /* Pass all multicast packets           RW                */
  GmacMulticastFilterOn    = 0,              /* Pass filtered multicast packets                0       */

// CHANGE: Changed to Dest Addr Filter Inverse (07/28)  
  GmacDestAddrFilterInv    = 0x00000008,     /* Inverse filtering for DA             RW                */
  GmacDestAddrFilterNor    = 0,              /* Normal filtering for DA                        0       */

// CHANGE: Changed to Multicast Hash filter (07/28)  
  GmacMcastHashFilterOn    = 0x00000004,     /* perfom multicast hash filtering      RW                */
  GmacMcastHashFilterOff   = 0,              /* perfect filtering only                         0       */

// CHANGE: Changed to Unicast Hash filter (07/28)
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

// CHANGED: 3-bit config instead of older 2-bit (07/28)  
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

// CHANGED: Added on (07/28)
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

/**********************************************************
 * DMA Engine registers
 **********************************************************/

enum DmaBusModeReg         /* DMA bus mode register */
{                                         /* Bit description                        R/W   Reset value */
// CHANGED: Commented as not applicable (07/28)
//  DmaBigEndianDesc        = 0x00100000,   /* Big endian data buffer descriptors   RW                */
//  DmaLittleEndianDesc     = 0,            /* Little endian data descriptors                 0       */

// CHANGED: Added on 07/28
  DmaFixedBurstEnable     = 0x00010000,   /* Fixed Burst SINGLE, INCR4, INCR8 or INCR16               */
  DmaFixedBurstDisable    = 0,            /*             SINGLE, INCR                         0       */

  DmaBurstLength32        = 0x00002000,   /* Dma burst length = 32                  RW                */
  DmaBurstLength16        = 0x00001000,   /* Dma burst length = 16                                    */
  DmaBurstLength8         = 0x00000800,   /* Dma burst length = 8                                     */
  DmaBurstLength4         = 0x00000400,   /* Dma burst length = 4                                     */
  DmaBurstLength2         = 0x00000200,   /* Dma burst length = 2                                     */
  DmaBurstLength1         = 0x00000100,   /* Dma burst length = 1                                     */
  DmaBurstLength0         = 0x00000000,   /* Dma burst length = 0                             0       */

// CHANGED: Commented as not applicable (07/28)
//  DmaBigEndianData        = 0x00000080,   /* Big endian data buffers              RW                */
//  DmaLittleEndianData     = 0,            /* Little endian data buffers                     0       */

  DmaDescriptorSkip16     = 0x00000040,   /* number of dwords to skip               RW                */
  DmaDescriptorSkip8      = 0x00000020,   /* between two unchained descriptors                        */
  DmaDescriptorSkip4      = 0x00000010,   /*                                                          */
  DmaDescriptorSkip2      = 0x00000008,   /*                                                          */
  DmaDescriptorSkip1      = 0x00000004,   /*                                                          */
  DmaDescriptorSkip0      = 0,            /*                                                  0       */

  DmaResetOn              = 0x00000001,   /* Reset DMA engine                       RW                */
  DmaResetOff             = 0,            /*                                                  0       */
};

enum DmaStatusReg         /* DMA Status register */
{                                         /* Bit description                        R/W   Reset value */
// CHANGED: Added on 07/28
  DmaLineIntfIntr         = 0x04000000,   /* Line interface interrupt               R         0       */
// CHANGED: Added on 07/28
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
// CHANGED: Commented as not applicable (07/28)
//  DmaRxChecking           = 0x00040000,   /* Running - checking for end of packet                   */
  DmaRxWaiting            = 0x00060000,   /* Running - waiting for packet                             */
  DmaRxSuspended          = 0x00080000,   /* Suspended                                                */
  DmaRxClosing            = 0x000A0000,   /* Running - closing descriptor                             */
// CHANGED: Commented as not applicable (07/28)
//  DmaRxFlushing           = 0x000C0000,   /* Running - flushing the current frame                   */
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
// CHANGED: Added on 07/28
  DmaIntRcvOverflow       = 0x00000010,   /* Receive Buffer overflow interrupt      RW        0       */
  DmaIntTxJabberTO        = 0x00000008,   /* Transmit Jabber Timeout (Abnormal)     RW        0       */
  DmaIntTxNoBuffer        = 0x00000004,   /* Transmit buffer unavailable (Normal)   RW        0       */
  DmaIntTxStopped         = 0x00000002,   /* Transmit process stopped (Abnormal)    RW        0       */
  DmaIntTxCompleted       = 0x00000001,   /* Transmit completed (Normal)            RW        0       */
};

enum DmaControlReg        /* DMA control register */
{  
                                       /* Bit description                        R/W   Reset value */
  DmaDisableFlushFrames   = 0x01000000,   /* Disable Flushing of Received Freams    RW        0       */
  DmaStoreAndForward      = 0x00200000,   /* Store and forward                      RW        0       */
  DmaTxStart              = 0x00002000,   /* Start/Stop transmission                RW        0       */
// CHANGED: Added on 07/28
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
// CHANGED: Added on 07/28
  DmaIeRxOverflow        = DmaIntRcvOverflow,   /* Receive Buffer overflow interrupt       RW        0       */
  DmaIeTxJabberTO        = DmaIntTxJabberTO ,   /* Transmit Jabber Timeout enable          RW        0       */
  DmaIeTxNoBuffer        = DmaIntTxNoBuffer ,   /* Transmit buffer unavailable enable      RW        0       */
  DmaIeTxStopped         = DmaIntTxStopped  ,   /* Transmit process stopped enable         RW        0       */
  DmaIeTxCompleted       = DmaIntTxCompleted,   /* Transmit completed enable               RW        0       */
};

/**********************************************************
 * DMA Engine descriptors
 **********************************************************/

enum DmaDescriptorStatus    /* status word of DMA descriptor */
{
  DescOwnByDma          = 0x80000000,   /* Descriptor is owned by DMA engine  */
// CHANGED: Added on 07/29
  DescDAFilterFail      = 0x40000000,   /* Rx - DA Filter Fail for the received frame        E  */
  DescFrameLengthMask   = 0x3FFF0000,   /* Receive descriptor frame length */
  DescFrameLengthShift  = 16,

  DescError             = 0x00008000,   /* Error summary bit  - OR of the following bits:    v  */

  DescRxTruncated       = 0x00004000,   /* Rx - no more descriptors for receive frame        E  */
// CHANGED: Added on 07/29
  DescSAFilterFail      = 0x00002000,   /* Rx - SA Filter Fail for the received frame        E  */
/* added by reyaz */
  DescRxLengthError	= 0x00001000,   /* Rx - frame size not matching with length field    E  */
  DescRxDamaged         = 0x00000800,   /* Rx - frame was damaged due to buffer overflow     E  */
// CHANGED: Added on 07/29
  DescRxVLANTag         = 0x00000400,   /* Rx - received frame is a VLAN frame               I  */
  DescRxFirst           = 0x00000200,   /* Rx - first descriptor of the frame                I  */
  DescRxLast            = 0x00000100,   /* Rx - last descriptor of the frame                 I  */
  DescRxLongFrame       = 0x00000080,   /* Rx - frame is longer than 1518 bytes              E  */
  DescRxIPCsumErr		= 0x00000080,   /* Rx - IPC Checksum Error */
  DescRxCollision       = 0x00000040,   /* Rx - late collision occurred during reception     E  */
  DescRxFrameEther      = 0x00000020,   /* Rx - Frame type - Ethernet, otherwise 802.3          */
  DescRxWatchdog        = 0x00000010,   /* Rx - watchdog timer expired during reception      E  */
  DescRxMiiError        = 0x00000008,   /* Rx - error reported by MII interface              E  */
  DescRxDribbling       = 0x00000004,   /* Rx - frame contains noninteger multiple of 8 bits    */
  DescRxCrc             = 0x00000002,   /* Rx - CRC error                                    E  */
  DescRxPLCsumErr		= 0x00000001,	/* Rx - Payload checksum Error */

  DescTxIPHeaderErr     = 0x00010000,   /* Tx - IP header detected by IP checksum offload engine */
  DescTxTimeout         = 0x00004000,   /* Tx - Transmit jabber timeout                      E  */
// CHANGED: Added on 07/29
  DescTxFrameFlushed    = 0x00002000,   /* Tx - DMA/MTL flushed the frame due to SW flush    I  */
  DescTxPLCsumErr       = 0x00001000,   /* Tx - payload checksum error */
  DescTxLostCarrier     = 0x00000800,   /* Tx - carrier lost during tramsmission             E  */
  DescTxNoCarrier       = 0x00000400,   /* Tx - no carrier signal from the tranceiver        E  */
  DescTxLateCollision   = 0x00000200,   /* Tx - transmission aborted due to collision        E  */
  DescTxExcCollisions   = 0x00000100,   /* Tx - transmission aborted after 16 collisions     E  */
  DescTxVLANFrame       = 0x00000080,   /* Tx - VLAN-type frame                                 */
  DescTxCollMask        = 0x00000078,   /* Tx - Collision count                                 */
  DescTxCollShift       = 3,
  DescTxExcDeferral     = 0x00000004,   /* Tx - excessive deferral                           E  */
  DescTxUnderflow       = 0x00000002,   /* Tx - late data arrival from the memory            E  */
  DescTxDeferred        = 0x00000001,   /* Tx - frame transmision deferred                      */
};

enum DmaDescriptorLength    /* length word of DMA descriptor */
{
  DescTxIntEnable       = 0x80000000,   /* Tx - interrupt on completion                         */
  DescTxLast            = 0x40000000,   /* Tx - Last segment of the frame                       */
  DescTxFirst           = 0x20000000,   /* Tx - First segment of the frame                      */
  DescTxCsumInsert      = 0x18000000,   /* Tx - Insert TCP/UDP/ICMP checksum and IPv4 checksum */
  DescTxDisableCrc      = 0x04000000,   /* Tx - Add CRC disabled (first segment only)           */

  DescEndOfRing         = 0x02000000,   /* End of descriptors ring                              */
  DescChain             = 0x01000000,   /* Second buffer address is chain address               */
  DescTxDisablePadd	= 0x00800000,   /* disable padding, added by - reyaz */

  DescSize2Mask         = 0x003FF800,   /* Buffer 2 size                                        */
  DescSize2Shift        = 11,
  DescSize1Mask         = 0x000007FF,   /* Buffer 1 size                                        */
  DescSize1Shift        = 0,
};

/**********************************************************
 * Initial register values
 **********************************************************/

enum InitialRegisters
{
  GmacConfigInitFdx1000       /* Full-duplex mode with perfect filter on */
                          = GmacWatchdogEnable | GmacJabberEnable         /*| GmacFrameBurstEnable*/ | GmacJumboFrameDisable 
// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,   			  
                          | GmacSelectGmii     | GmacEnableRxOwn          | GmacLoopbackOff
                          | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable
                          | GmacChecksumOffloadOn,


  GmacConfigInitFdx100       /* Full-duplex mode with perfect filter on */
                          = GmacWatchdogEnable | GmacJabberEnable         /*| GmacFrameBurstEnable*/  | GmacJumboFrameDisable
// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,   			  
                          | GmacSelectMii      | GmacEnableRxOwn          | GmacLoopbackOff
                          | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable | GmacSpeed100Mbps
                          | GmacChecksumOffloadOn,


  GmacConfigInitFdx10       /* Full-duplex mode with perfect filter on */
                          = GmacWatchdogEnable | GmacJabberEnable         /*| GmacFrameBurstEnable*/  | GmacJumboFrameDisable
// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,   			  
                          | GmacSelectMii      | GmacEnableRxOwn          | GmacLoopbackOff
                          | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable | GmacSpeed10Mbps
                          | GmacChecksumOffloadOn,

  GmacFrameFilterInitFdx  /* Full-duplex mode */
// CHANGED: Pass control config, dest addr filter normal, added source address filter, multicast & unicast 
// Hash filter. 
                          = GmacFilterOn          | GmacPassControl0   | GmacBroadcastEnable |  GmacSrcAddrFilterDisable
/*                        = GmacFilterOff         | GmacPassControlOff | GmacBroadcastEnable */
                          | GmacMulticastFilterOn | GmacDestAddrFilterNor | GmacMcastHashFilterOff
                          | GmacPromiscuousModeOff | GmacUcastHashFilterOff,

  GmacFlowControlInitFdx  /* Full-duplex mode */
                          = GmacUnicastPauseFrameOff | GmacRxFlowControlEnable | GmacTxFlowControlEnable,

  GmacGmiiAddrInitFdx     /* Full-duplex mode */
                          = GmiiAppClk2,

  GmacConfigInitHdx1000       /* Half-duplex mode with perfect filter on */
                          = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,   			  
                        /*| GmacSelectMii      | GmacLittleEndian         | GmacDisableRxOwn      | GmacLoopbackOff*/
                          | GmacSelectGmii     | GmacDisableRxOwn         | GmacLoopbackOff
                          | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable   
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable
						  | GmacChecksumOffloadOn,

  GmacConfigInitHdx100       /* Half-duplex mode with perfect filter on */
                          = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,   			  
                          | GmacSelectMii      | GmacDisableRxOwn         | GmacLoopbackOff
                          | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable 
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable | GmacSpeed100Mbps
						  | GmacChecksumOffloadOn,

  GmacConfigInitHdx10       /* Half-duplex mode with perfect filter on */
                          = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
// CHANGED: Removed Endian configuration, added single bit config for PAD/CRC strip,   			  
                          | GmacSelectMii      | GmacDisableRxOwn         | GmacLoopbackOff
                          | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable 
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable | GmacSpeed10Mbps
						  | GmacChecksumOffloadOn,

  GmacFrameFilterInitHdx  /* Half-duplex mode */
                          = GmacFilterOn          | GmacPassControl0        | GmacBroadcastEnable | GmacSrcAddrFilterDisable
                          | GmacMulticastFilterOn | GmacDestAddrFilterNor   | GmacMcastHashFilterOff
                          | GmacUcastHashFilterOff| GmacPromiscuousModeOff,

  GmacFlowControlInitHdx  /* Half-duplex mode */
                          = GmacUnicastPauseFrameOff | GmacRxFlowControlDisable | GmacTxFlowControlDisable,

  GmacGmiiAddrInitHdx     /* Half-duplex mode */
                          = GmiiAppClk2,

  DmaBusModeInit          /* Little-endian mode */
// CHANGED: Removed Endian configuration   			  
                         = DmaBurstLength16   | DmaDescriptorSkip1       | DmaResetOff ,

  DmaControlInit1000      /* 1000 Mb/s mode */
                          = DmaStoreAndForward | DmaTxSecondFrame | DmaDisableFlushFrames,

  DmaControlInit100       /* 100 Mb/s mode */
                          = DmaStoreAndForward | DmaDisableFlushFrames ,

  DmaControlInit10        /* 10 Mb/s mode */
                          = DmaStoreAndForward | DmaDisableFlushFrames ,

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

/**********************************************************
 * Common functions
 **********************************************************/

int TipsterInit( Tipster *Dev, u32 ConfigBase, u32 MacBase, u32 DmaBase, u32 PhyAddr )
{
  memset( Dev, 0, sizeof(*Dev) );

  Dev->configBase = ConfigBase;
  Dev->macBase = MacBase;
  Dev->dmaBase = DmaBase;
  Dev->phyAddr = PhyAddr;

  return 0;
}

/**********************************************************
 * MAC module functions
 **********************************************************/

int TipsterMacInit( Tipster *Dev, u8 Addr[6], u8 Broadcast[6] )
{
  u32 data;
  u16 PHYreg;

  data = (Addr[5]<<8) | Addr[4];                            /* set our MAC address */
  TipsterWriteMacReg( Dev, GmacAddr0High, data );
  data = (Addr[3]<<24) | (Addr[2]<<16) | (Addr[1]<<8) | Addr[0];
  TipsterWriteMacReg( Dev, GmacAddr0Low, data );

  if( Dev->fdx )
  {
/* Added by Lokesh */
     if(Dev->speed_1000) {
	 	if(Dev->rgmii_interface) {
			TipsterWriteMacReg( Dev, GmacConfig, 0x01200D0C ); 
		} else {
			TipsterWriteMacReg( Dev, GmacConfig, GmacConfigInitFdx1000 ); /* set init values of config registers */
		}
     } else if(Dev->speed_100) {
     	if(Dev->rgmii_interface)
			TipsterWriteMacReg( Dev, GmacConfig, 0x0120CD0C ); /* set init values of config registers with MII port */
		else
			TipsterWriteMacReg( Dev, GmacConfig, GmacConfigInitFdx100 ); /* set init values of config registers with MII port */		
     } else if(Dev->speed_10) {
     	if(Dev->rgmii_interface)
			TipsterWriteMacReg( Dev, GmacConfig, 0x01208D0C ); /* set init values of config registers with MII port 10Mbps */
		else
			TipsterWriteMacReg( Dev, GmacConfig, GmacConfigInitFdx10 ); /* set init values of config registers with MII port 10Mbps */
     }

    TipsterWriteMacReg( Dev, GmacFrameFilter, GmacFrameFilterInitFdx );
    TipsterWriteMacReg( Dev, GmacFlowControl, GmacFlowControlInitFdx );
    TipsterWriteMacReg( Dev, GmacGmiiAddr, GmacGmiiAddrInitFdx );
  }
  else
  {
/* Modified by Lokesh */
    if(Dev->speed_1000) {
		TipsterWriteMacReg( Dev, GmacConfig, GmacConfigInitHdx1000 ); /* set init values of config registers */
    } else if(Dev->speed_100) {
    	TipsterWriteMacReg( Dev, GmacConfig, GmacConfigInitHdx100 ); /* set init values of config registers with MII port */
    } else if(Dev->speed_10) {    	
		TipsterWriteMacReg( Dev, GmacConfig, GmacConfigInitHdx10 ); /* set init values of config registers with MII port 10Mbps */
    }

    TipsterWriteMacReg( Dev, GmacFrameFilter, GmacFrameFilterInitHdx );
    TipsterWriteMacReg( Dev, GmacFlowControl, GmacFlowControlInitHdx );
    TipsterWriteMacReg( Dev, GmacGmiiAddr, GmacGmiiAddrInitHdx );
/* To set PHY register to enable CRS on Transmit */
    TipsterWriteMacReg( Dev, GmacGmiiAddr, GmiiBusy | 0x00000408);
    PHYreg = TipsterReadMacReg(Dev, GmacGmiiData);
    TipsterWriteMacReg( Dev, GmacGmiiData, PHYreg | 0x00000800);
    TipsterWriteMacReg( Dev, GmacGmiiAddr, GmiiBusy | 0x0000040a);
  }

  if ((TipsterMiiRead(Dev, 0x2) == REALTEK_RTL8112C_ID_1) && ((TipsterMiiRead(Dev, 0x3) & 0xFFF0) == REALTEK_RTL8112C_ID_2) &&
  	   (Dev->rgmii_interface) && (Dev->speed_1000) && !(v_inl(0xf4f80014) == 0x01000000)) {
  	   unsigned long reg = inl(IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
	   reg &= ~(1 << 2) ;	
	   reg |= (0 << 2) ; //switch to 3.3v
	   outl(reg, IO_ADDRESS(MOZART_SYSC_MMR_BASE)+0x48);
  }

  return 0;
}

u16 TipsterMiiRead( Tipster *Dev, u8 Reg )
{
  u32 addr;
  u16 data;

  if(mdc_clk_init_counter == 0 ) {
  	mdc_clk_init_counter++ ;

	if((AHB_CLK < 300000000) && (AHB_CLK >= 250000000)) {
		MDC_CLK_DIV = GmiiAppClk5 ;
	}
	else if((AHB_CLK < 250000000) && (AHB_CLK >= 150000000)) {
		MDC_CLK_DIV = GmiiAppClk4 ;
	}
	else if((AHB_CLK < 150000000) && (AHB_CLK >= 100000000)) {
		MDC_CLK_DIV = GmiiAppClk1 ;
	}
	else if((AHB_CLK < 100000000) && (AHB_CLK >= 60000000)) {
		MDC_CLK_DIV = GmiiAppClk0 ;
	}
	else if((AHB_CLK < 60000000) && (AHB_CLK >= 35000000)) {
		MDC_CLK_DIV = GmiiAppClk3 ;
	}
	else if((AHB_CLK < 35000000) && (AHB_CLK >= 20000000)) {
		MDC_CLK_DIV = GmiiAppClk2 ;
	}
	else {
		MDC_CLK_DIV = GmiiAppClk5 ;
	}
  }

  addr = ((Dev->phyAddr << GmiiDevShift) & GmiiDevMask) | ((Reg << GmiiRegShift) & GmiiRegMask);  
  TipsterWriteMacReg( Dev, GmacGmiiAddr, (addr | MDC_CLK_DIV | GmiiBusy));

  do{} while( (TipsterReadMacReg( Dev, GmacGmiiAddr ) & GmiiBusy) == GmiiBusy );

  data = TipsterReadMacReg( Dev, GmacGmiiData ) & 0xFFFF;
  return data;
}

void TipsterMiiWrite( Tipster *Dev, u8 Reg, u16 Data )
{
  u32 addr;

  if(mdc_clk_init_counter == 0 ) {
  	mdc_clk_init_counter++ ;

	if((AHB_CLK < 300000000) && (AHB_CLK >= 250000000)) {
		MDC_CLK_DIV = GmiiAppClk5 ;
	}
	else if((AHB_CLK < 250000000) && (AHB_CLK >= 150000000)) {
		MDC_CLK_DIV = GmiiAppClk4 ;
	}
	else if((AHB_CLK < 150000000) && (AHB_CLK >= 100000000)) {
		MDC_CLK_DIV = GmiiAppClk1 ;
	}
	else if((AHB_CLK < 100000000) && (AHB_CLK >= 60000000)) {
		MDC_CLK_DIV = GmiiAppClk0 ;
	}
	else if((AHB_CLK < 60000000) && (AHB_CLK >= 35000000)) {
		MDC_CLK_DIV = GmiiAppClk3 ;
	}
	else if((AHB_CLK < 35000000) && (AHB_CLK >= 20000000)) {
		MDC_CLK_DIV = GmiiAppClk2 ;
	}
	else {
		MDC_CLK_DIV = GmiiAppClk5 ;
	}
  }


  TipsterWriteMacReg( Dev, GmacGmiiData, Data );

  addr = ((Dev->phyAddr << GmiiDevShift) & GmiiDevMask) | ((Reg << GmiiRegShift) & GmiiRegMask) | GmiiWrite;
  TipsterWriteMacReg( Dev, GmacGmiiAddr, (addr | MDC_CLK_DIV | GmiiBusy));

  do{} while( (TipsterReadMacReg( Dev, GmacGmiiAddr ) & GmiiBusy) == GmiiBusy );
}

/**********************************************************
 * DMA engine functions
 **********************************************************/

static void DescInit( DmaDesc *Desc, bool EndOfRing )
{
  Desc->status = 0;
  Desc->length = EndOfRing ? DescEndOfRing : 0;
  Desc->buffer1 = 0;
  Desc->buffer2 = 0;
  Desc->data1 = 0;
  Desc->data2 = 0;
}

static bool DescLast( DmaDesc *Desc )
{
  return (Desc->length & DescEndOfRing) != 0;
}

static bool DescEmpty( DmaDesc *Desc )
{
  return (Desc->length & ~ DescEndOfRing) == 0;
}

static bool DescDma( DmaDesc *Desc )
{
  return (Desc->status & DescOwnByDma) != 0;   /* Owned by DMA */
}

static void DescTake( DmaDesc *Desc )   /* Take ownership */
{
  if( DescDma(Desc) )               /* if descriptor is owned by DMA engine */
  {
    Desc->status &= ~DescOwnByDma;    /* clear DMA own bit */
    Desc->status |= DescError;        /* Set error bit to mark this descriptor bad */
  }
}

#define dma_alloc_consistent(d,s,p) pci_alloc_consistent(d,s,p)
int TipsterDmaInit( Tipster *Dev )
{
  int i;

  Dev->txCount = NET_TX_DESC_NUM;       
  Dev->rxCount = NET_RX_DESC_NUM;       
  
  Dev->rx = pr.rxDesc;
  Dev->tx = pr.txDesc;

  for( i=0; i<Dev->txCount; i++ ) DescInit( Dev->tx + i, i==Dev->txCount-1 );
  for( i=0; i<Dev->rxCount; i++ ) DescInit( Dev->rx + i, i==Dev->rxCount-1 );

  Dev->txNext = 0;
  Dev->txBusy = 0;
  Dev->rxNext = 0;
  Dev->rxBusy = 0;

  TipsterWriteDmaReg( Dev, DmaBusMode, DmaResetOn );       /* Reset DMA engine */
  TipsterSysDelay( 2 );      /* Delay 2 usec (=50 PCI cycles on 25 Mhz) */
  while( (TipsterReadDmaReg( Dev, DmaBusMode) & DmaResetOn) != 0); //jon
  TipsterWriteDmaReg( Dev, DmaBusMode, DmaBusModeInit );   /* Set init register values */
  TipsterWriteDmaReg( Dev, DmaRxBaseAddr, pr.rxDmaDesc );
  TipsterWriteDmaReg( Dev, DmaTxBaseAddr, pr.txDmaDesc );
  if( Dev->speed_1000 ) 
	TipsterWriteDmaReg( Dev, DmaControl, DmaControlInit1000);
  else if( Dev->speed_100 ) 
	TipsterWriteDmaReg( Dev, DmaControl, DmaControlInit100 );
  else if ( Dev->speed_10 )
	TipsterWriteDmaReg( Dev, DmaControl, DmaControlInit10 );
  
  TipsterWriteDmaReg( Dev, DmaInterrupt, DmaIntDisable );

  return 0;
}

int TipsterDmaRxSet( Tipster *Dev, u32 Buffer1, u32 Length1, u32 Data1 )
{
  int desc = Dev->rxNext;
  DmaDesc *rx = Dev->rx + desc;
  if( !DescEmpty(rx) ) return -1;       /* descriptor is not empty - cannot set */


  rx->length &= DescEndOfRing;      /* clear everything */
  rx->length |= ((Length1 << DescSize1Shift) & DescSize1Mask);
  rx->buffer1 = Buffer1;
  rx->data1   = Data1;
  rx->status  = DescOwnByDma;
  
  Dev->rxNext = DescLast(rx) ? 0 : desc + 1;

  return desc;
}

int TipsterDmaRxGet( Tipster *Dev, u32 *Status, u32 *Buffer1, u32 *Length1, u32 *Data1 )
{
  int desc = Dev->rxBusy;
  DmaDesc *rx = Dev->rx + desc;

  if( DescDma(rx) ){ 
	 	return -1;
  }/* descriptor is owned by DMA - cannot get */
  if( DescEmpty(rx) ) {
	printk("[jon]get fail(Empty)!\n");
	return -1;        
  }/* descriptor is empty - cannot get */

  if( Status != 0 ) *Status = rx->status;
  if( Length1 != 0 ) *Length1 = (rx->length & DescSize1Mask) >> DescSize1Shift;
  if( Buffer1 != 0 ) *Buffer1 = rx->buffer1;
  if( Data1 != 0 ) *Data1 = rx->data1;

  DescInit( rx, DescLast(rx) );
  Dev->rxBusy = DescLast(rx) ? 0 : desc + 1;

  return desc;
}

extern int tx_crc;
extern int tx_runt;
extern int tx_both;
extern int tx_numerr;
extern int tx_len;
typedef struct 	eth_head {

	u8 dest_addr[6];
	u8 src_add[6];
	u16 type;
}ETH_HEADER;

int TipsterDmaTxSet( Tipster *Dev, u32 Buffer1, u32 Length1, u32 Data1, u32 ip_summed )
{
  int ln;
  int desc = Dev->txNext;
  DmaDesc *tx = Dev->tx + desc;
  if( !DescEmpty(tx) ) {
  	printk(KERN_DEBUG "descriptor not valid!\n");
  	return -1; /* descriptor is not empty - cannot set */
  }
#if 1 
if(tx_runt == 1 && tx_numerr != 0)	{
	ln = Length1;
	printk("<1> \n RUNT ln - %d, length1 - %d \n", ln, Length1);
}
if(tx_crc == 1 && tx_numerr != 0)	{
	u8 *sk = (struct sk_buff *)Data1;
	/* int i;
	ETH_HEADER *hd = (ETH_HEADER *)sk; */
	ln = Length1;
	if(Length1 < 60)	{	
		Length1 =  64; /* runt, less than 64 */
		memset((void *)(sk+ln), 0, Length1-ln ); 
	}
	printk("<1> \n CRC error pkt ln - %d,  length - %d \n", ln, Length1);
}

if(tx_len == 1 && tx_numerr != 0 )	{
	int i;
	u8 *sk = (struct sk_buff *)Data1;
	ETH_HEADER *hd = (ETH_HEADER *)sk;
	hd->type = 70; /*Length1-2;*/

	tx_numerr--;
	if(tx_numerr == 0)
		tx_len = 0;
	printk("<1> \n Frame length err - %d, length1 - %d \n", hd->type, Length1);
     for(i = 0; i < Length1; i++)
	printk(KERN_ERR "%1x", sk[i]);
	printk("\n");

}

#endif

  tx->length &= DescEndOfRing;      /* clear everything */
  tx->length |= ((Length1 << DescSize1Shift) & DescSize1Mask) | DescTxFirst | DescTxLast | DescTxIntEnable;

  if (ip_summed == CHECKSUM_PARTIAL) {
      tx->length |= DescTxCsumInsert; // CIC: Checksum Insertion Control
  }

#if 1 

if(tx_crc == 1 && tx_numerr != 0)	{
  
	tx->length |=  DescTxDisableCrc;
	tx_numerr--;
	if(tx_numerr == 0)
		tx_crc = 0;
	printk("<1> \n CRC \n");
}

if(tx_runt == 1 && tx_numerr != 0)	{
	tx->length |=  DescTxDisablePadd;
	tx_numerr--;
	if(tx_numerr == 0)
		tx_runt = 0;
	printk("<1> \n RUNT \n");
}

if(tx_both == 1 && tx_numerr != 0)	{
	tx->length |=  DescTxDisableCrc | DescTxDisablePadd;
	tx_numerr--;
	if(tx_numerr == 0)	{
		tx_runt = 0;
		tx_crc = 0;
	}
	printk("<1> \n CRC and RUNT \n");
}

#endif

  tx->buffer1 = Buffer1;
  tx->data1   = Data1;
  tx->status  = DescOwnByDma;

  Dev->txNext = DescLast(tx) ? 0 : desc + 1;

  return desc;
}

int TipsterDmaTxGet( Tipster *Dev, u32 *Status, u32 *Buffer1, u32 *Length1, u32 *Data1 )
{
  int desc = Dev->txBusy;
  DmaDesc *tx = Dev->tx + desc;

  if (tx == 0) {
    printk("tx NULL!!\n");
	printk("desc: %d\n", desc);
	printk("dev->tx: 0x%08x\n", (u32)Dev->tx);
	while(1);
  }
  if( DescDma(tx) ){
	return -1;
  }/* descriptor is owned by DMA - cannot get */
  if( DescEmpty(tx) ){
	return -1;        /* descriptor is empty - cannot get */
  }
  if( Status != 0 ) *Status = tx->status;
  if( Buffer1 != 0 ) *Buffer1 = tx->buffer1;
  if( Length1 != 0 ) *Length1 = (tx->length & DescSize1Mask) >> DescSize1Shift;
  if( Data1 != 0 ) *Data1 = tx->data1;
  DescInit( tx, DescLast(tx) );
  Dev->txBusy = DescLast(tx) ? 0 : desc + 1;

  return desc;
}

void TipsterDmaRxStart( Tipster *Dev )
{
  TipsterSetDmaReg( Dev, DmaControl, DmaRxStart );
}

void TipsterDmaRxStop( Tipster *Dev )
{
  int i;
  TipsterClearDmaReg( Dev, DmaControl, DmaRxStart );
  for( i=0; i<Dev->rxCount; i++ ) DescTake( Dev->rx + i );
}

void TipsterDmaTxStart( Tipster *Dev )
{
  TipsterSetDmaReg( Dev, DmaControl, DmaTxStart );
}

void TipsterDmaTxStop( Tipster *Dev )
{
  int i;
  TipsterClearDmaReg( Dev, DmaControl, DmaTxStart );
  for( i=0; i<Dev->txCount; i++ ) DescTake( Dev->tx + i );
}

void TipsterDmaRxResume( Tipster *Dev )
{
  TipsterWriteDmaReg( Dev, DmaRxPollDemand, 0 );
}

void TipsterDmaTxResume( Tipster *Dev )
{
  TipsterWriteDmaReg( Dev, DmaTxPollDemand, 0 );
}

int TipsterDmaRxValid( u32 Status )
{
  return ( (Status & DescError) == 0 )      /* no errors, whole frame is in the buffer */
      && ( (Status & DescRxFirst) != 0 )
      && ( (Status & DescRxLast) != 0 );
}

int TipsterDmaRxCRC( u32 Status )
{
  return (((Status & DescRxPLCsumErr) == 0) /* no error occurs on both IP checksum and payload checksum*/
  	  && ((Status & DescRxIPCsumErr) == 0));
}

// Returns 1 when the Dribbling status is set
int TipsterDmaRxStatusDriblling( u32 Status )
{
  return ( (Status & DescRxDribbling) != 0 );
}

u32 TipsterDmaRxLength( u32 Status )
{
  return (Status & DescFrameLengthMask) >> DescFrameLengthShift;
}

int TipsterDmaRxCollisions
(
  u32 Status
)
{
  if( Status & (DescRxDamaged | DescRxCollision) ) return 1;
  return 0;
}

int TipsterDmaRxCrc
(
  u32 Status
)
{
  if( Status & DescRxCrc ) return 1;
  return 0;
}
/* added by reyaz */
int TipsterDmaRxFrame
(
  u32 Status
)
{
  if( Status & DescRxDribbling ) return 1;
  return 0;
}

int TipsterDmaRxLongFrame
(
  u32 Status
)
{
  if( Status & DescRxLongFrame ) return 1;
  return 0;
}
int TipsterDmaRxLengthError
(
  u32 Status
)
{
  if( Status & DescRxLengthError ) return 1;
  return 0;
}
int TipsterDmaTxValid   /* Test the status word if the descriptor is valid */
(
  u32 Status
)
{
  return ( ((Status & DescError) == 0) &&
            ((Status & DescTxIPHeaderErr) == 0) &&
             ((Status & DescTxPLCsumErr) == 0));
}

int TipsterDmaTxCollisions
(
  u32 Status
)
{
  return (Status & DescTxCollMask) >> DescTxCollShift;
}

int TipsterDmaTxAborted
(
  u32 Status
)
{
  if( Status & (DescTxLateCollision | DescTxExcCollisions )) return 1;
  return 0;
}

int TipsterDmaTxCarrier
(
  u32 Status
)
{
  if( Status & (DescTxLostCarrier | DescTxNoCarrier )) return 1;
  return 0;
}


/**********************************************************
 * DMA engine interrupt handling functions
 **********************************************************/

void TipsterDmaIntEnable( Tipster *Dev )
{
  TipsterWriteDmaReg( Dev, DmaInterrupt, DmaIntEnable );    /* enable interrupts */
}

void TipsterDmaIntDisable( Tipster *Dev )
{
  TipsterWriteDmaReg( Dev, DmaInterrupt, DmaIntDisable );    /* disable interrupts */
}

void TipsterDmaIntClear( Tipster *Dev )
{
  TipsterWriteDmaReg( Dev, DmaStatus, TipsterReadDmaReg( Dev, DmaStatus) );    /* clear all interrupt requests */
}

u32 TipsterDmaIntType( Tipster *Dev )
{
  u32 status = TipsterReadDmaReg( Dev, DmaStatus );
  u32 type = 0;

  if( status & DmaIntErrorMask )      type |= TipsterDmaError;
  if( status & DmaIntRxNormMask )     type |= TipsterDmaRxNormal;
  if( status & DmaIntRxAbnMask )      type |= TipsterDmaRxAbnormal;
  if( status & DmaIntRxStoppedMask )  type |= TipsterDmaRxStopped;
  if( status & DmaIntTxNormMask )     type |= TipsterDmaTxNormal;
  if( status & DmaIntTxAbnMask )      type |= TipsterDmaTxAbnormal;
  if( status & DmaIntTxStoppedMask )  type |= TipsterDmaTxStopped;

  TipsterWriteDmaReg( Dev, DmaStatus, status );     /* clear all interrupt requests */

  return type;
}

void GMACMacHashMulticast(Tipster *Dev)
{
  TipsterWriteMacReg(Dev, GmacFrameFilter, GmacMcastHashFilterOn);
}

void GMACMacPassAllMulticast(Tipster *Dev)
{
  TipsterWriteMacReg(Dev, GmacFrameFilter, GmacMulticastFilterOff);
}

void GMACMacPromiscuous(Tipster *Dev)
{
  TipsterWriteMacReg(Dev, GmacFrameFilter, GmacPromiscuousModeOn);
}

void GMACMacAddHash(Tipster *Dev, u32 entry)
{

  if (entry < 64) {
    u32 reg;

    if (entry >= 32) {
      reg = TipsterReadMacReg(Dev, GmacHashHigh);
      TipsterWriteMacReg(Dev, GmacHashHigh, reg|(1<<(entry&~0x20)));
    }
    else {
      reg = TipsterReadMacReg(Dev, GmacHashLow);
      TipsterWriteMacReg(Dev, GmacHashLow, reg|(1<<entry));
    }
  }
}

void GMACMacClearHash(Tipster *Dev)
{
  TipsterWriteMacReg(Dev, GmacHashHigh, 0);
  TipsterWriteMacReg(Dev, GmacHashLow, 0);
}


