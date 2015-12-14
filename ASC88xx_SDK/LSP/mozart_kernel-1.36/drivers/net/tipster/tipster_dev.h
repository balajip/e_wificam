/* Tipster PCI/Ethernet test chip driver for OS Linux
 *
 * Device-dependent (mostly system-independent) functions
 *
 * 1 Jun 1999 - G.Kazakov.
 */

#ifndef TIPSTER_INCLUDED
#define TIPSTER_INCLUDED

#include <linux/mii.h>

#include "tipster_sys.h" /* System-dependent definitions */

#define NET_TX_DESC_NUM 256
#define NET_RX_DESC_NUM 256


typedef struct DmaDescStruct    /* The structure is common for both receive and transmit descriptors */
{                               /* put it here to allow the caller to know its size */
  u32   status;         /* Status */
  u32   length;         /* Buffer length */
  u32   buffer1;        /* Buffer 1 pointer */
  u32   buffer2;        /* Buffer 2 pointer or next descriptor pointer in chain structure */
  u32   data1;          /* driver data, are not used by DMA engine,                       */
  u32   data2;          /*   set DmaDescriptorSkip2 in DmaBusModeInit to skip these words */
} DmaDesc;

typedef struct TipsterStruct       /* Tipster device data */
{
  u32 configBase;   /* base address of Config registers */
  u32 macBase;      /* base address of MAC registers */
  u32 dmaBase;      /* base address of DMA registers */
  u32 phyAddr;      /* PHY device address on MII interface */
  DmaDesc *tx;      /* start of TX descriptors ring */
  DmaDesc *rx;      /* start of RX descriptors ring */
  u32  rxCount;     /* number of rx descriptors */
  u32  txCount;     /* number of tx descriptors */
  u32  txBusy;      /* first descriptor owned by DMA engine, moved by DmaTxGet */
  u32  txNext;      /* next available tx descriptor, moved by DmaTxSet */
  u32  rxBusy;      /* first descripror owned by DMA engine, moved by DmaRxGet */
  u32  rxNext;      /* next available rx descriptor, moved by DmaRxSet */
  int fdx;          /* interface is in full-duplex mode */
  int speed_10;    /* interface operates on high speed (10Mb/s) */
  int speed_100;    /* interface operates on high speed (100Mb/s) */
  int speed_1000;   /* interface operates on high speed (1000Mb/s) */
  u32 linkup;
  u32 linkchanged;
  struct work_struct link_changed_work; /* work queue to handle link status changing */
  spinlock_t lock;  //jon
  struct net_device_stats stats;
  int rgmii_interface ;
} Tipster;

typedef struct PrivateStruct  /* Driver private data */
{
  Tipster tipster;            /* tipster device internal data */
  DmaDesc * rxDesc;
  DmaDesc * txDesc;
  u32 rxDmaDesc;
  u32 txDmaDesc;
  struct mii_if_info mii;
} Private;

#include "tipster_reg.h" /* System-dependent register access functions */

/**********************************************************
 * Common functions
 **********************************************************/

int TipsterInit        /* Initialise device data */
(
  Tipster *Dev,          /* Device structure, must be allocated by caller */
  u32 ConfigBase,         /* Base address of Configuration registers */
  u32 MacBase,            /* Base address of MAC registers */
  u32 DmaBase,            /* Base address of DMA registers */
  u32 PhyAddr             /* PHY device address */
);

/**********************************************************
 * MAC module functions
 **********************************************************/

int TipsterMacInit   /* Initialize MAC module  - set MAC and broadcast adresses, set filtering mode */
(
  Tipster *Dev,        /* Device */
  u8 Addr[6],          /* MAC address */
  u8 Broadcast[6]      /* Broadcast address */
);

u16 TipsterMiiRead   /* Read MII register */
(
  Tipster *Dev,        /* Device */
  u8 Reg                /* MII register */
);

void TipsterMiiWrite /* Write MII register */
(
  Tipster *Dev,        /* Device */
  u8 Reg,               /* MII register */
  u16 Data              /* Data to write */
);

/**********************************************************
 * DMA engine functions
 **********************************************************/

int TipsterDmaInit   /* Initialize DMA engine  - setup descriptor rings and control registers */
(
  Tipster *Dev        /* Device */
);

int TipsterDmaRxSet  /* Set rx descriptor Desc and pass ownership to DMA engine */
(                     /* returns descriptor number if ok, -1 if next descriptor is busy (owned by DMA) */
  Tipster *Dev,        /* Device */
  u32   Buffer1,        /* First buffer address to set to descriptor */
  u32   Length1,        /* First buffer length */
  u32   Data1           /* Driver's data to store into descriptor */
);

int TipsterDmaTxSet  /* Set tx descriptor Desc and pass ownership to DMA engine */
(                     /* returns descriptor number if ok, -1 if next descriptor is busy (owned by DMA) */
  Tipster *Dev,        /* Device */
  u32   Buffer1,        /* First buffer address to set to descriptor */
  u32   Length1,        /* First buffer length */
  u32   Data1,           /* Driver's data to store into descriptor */
  u32   ip_summed
);

int TipsterDmaRxGet /* take ownership of the rx descriptor Desc and clear it, returns old data */
(                    /* returns descriptor number if ok, -1 if next descriptor is busy (owned by DMA) */
  Tipster *Dev,        /* Device */
  u32  *Status,         /* Descriptor status */
  u32  *Buffer1,        /* First buffer address to set to descriptor */
  u32  *Length1,        /* First buffer length */
  u32  *Data1           /* Driver's data to store into descriptor */
);

int TipsterDmaTxGet  /* take ownership of the rx descriptor Desc and clear it, returns old data */
(                     /* returns descriptor number if ok, -1 if next descriptor is busy (owned by DMA) */
  Tipster *Dev,        /* Device */
  u32  *Status,         /* Descriptor status */
  u32  *Buffer1,        /* First buffer address to set to descriptor */
  u32  *Length1,        /* First buffer length */
  u32  *Data1           /* Driver's data to store into descriptor */
);

u32 TipsterDmaRxLength   /* extracts length from the Dma descriptor status word */
(
  u32 Status
);

int TipsterDmaRxValid   /* Test the status word if the descriptor is valid */
(
  u32 Status
);

int TipsterDmaRxCRC   /* Test the IP checksum offload status */
(
  u32 Status
);

int TipsterDmaRxCollisions
(
  u32 Status
);

int TipsterDmaRxCrc
(
  u32 Status
);

int TipsterDmaTxValid   /* Test the status word if the descriptor is valid */
(
  u32 Status
);

int TipsterDmaTxCollisions
(
  u32 Status
);

int TipsterDmaTxAborted
(
  u32 Status
);

int TipsterDmaTxCarrier
(
  u32 Status
);

void TipsterDmaRxStart      /* Start DMA receiver */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaRxStop      /* Stop DMA receiver */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaRxResume    /* Resume DMA receiver after suspend */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaTxStart      /* Start DMA transmitter */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaTxStop      /* Stop DMA transmitter */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaTxResume    /* Resume DMA transmitter after suspend */
(
  Tipster *Dev         /* Device */
);

/* L&T */

int TipsterDmaRxFrame    
(
  u32 Status
);

int TipsterDmaRxLengthError    
(
  u32 Status
);

/**********************************************************
 * DMA engine interrupt handling functions
 **********************************************************/

enum TipsterDmaIntEnum  /* Intrerrupt types */
{
  TipsterDmaRxNormal   = 0x01,   /* normal receiver interrupt */
  TipsterDmaRxAbnormal = 0x02,   /* abnormal receiver interrupt */
  TipsterDmaRxStopped  = 0x04,   /* receiver stopped */
  TipsterDmaTxNormal   = 0x08,   /* normal transmitter interrupt */
  TipsterDmaTxAbnormal = 0x10,   /* abnormal transmitter interrupt */
  TipsterDmaTxStopped  = 0x20,   /* transmitter stopped */
  TipsterDmaError      = 0x80,   /* Dma engine error */
};

void TipsterDmaIntEnable /* Enable DMA engine intrerrupts */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaIntDisable /* Disable DMA engine intrerrupts */
(
  Tipster *Dev         /* Device */
);

void TipsterDmaIntClear /* Clear DMA engine interrupt requests */
(
  Tipster *Dev         /* Device */
);

u32 TipsterDmaIntType
(
  Tipster *Dev         /* Device */
);

void GMACMacHashMulticast(Tipster *Dev);

void GMACMacPassAllMulticast(Tipster *Dev);

void GMACMacPromiscuous(Tipster *Dev);

void GMACMacAddHash(Tipster *Dev, u32 entry);

void GMACMacClearHash(Tipster *Dev);

//PHY ID
#define ICPLUS_IP1001_ID_1  0x0243
#define ICPLUS_IP1001_ID_2  0x0d90
#define REALTEK_RTL8112C_ID_1 0x001c
#define REALTEK_RTL8112C_ID_2 0xc910
#define ICPLUS_IP101_ID_1 0x0243
#define ICPLUS_IP101_ID_2 0X0C54
#define BROADCOM_BCM54616_ID_1	0x0362
#define BROADCOM_BCM54616_ID_2	0x5D12
#define MICREL_KSZ8051_ID_1	0x0022
#define MICREL_KSZ8051_ID_2	0x1550

#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))

#endif /* TIPSTER_INCLUDED */

