#ifndef __MOZART_MSHC_H__
#define __MOZART_MSHC_H__

#define VPL_MSHC_0			0xEB000000
#define VENDORID_V1			0x5342210a
#define VENDORID_V2			0x5342230a
#define POSDIODEBUG(...)
//#define POSDIODEBUG printk
#define PROCESSOR_CLK       50000     //In KH. 50MHz.
/* Error Codes */
#define	ERRCARDNOTCONN		1
#define ERRCMDNOTSUPP		2
#define ERRINVALIDCARDNUM	3
#define ERRRESPTIMEOUT		4
#define ERRCARDNOTFOUND		5
#define ERRCMDRETRIESOVER	6
#define	ERRCMDINPROGRESS	7
#define	ERRNOTSUPPORTED		8
#define ERRRESPRECEP		9
#define ERRENUMERATE		10
#define	ERRHARDWARE		11
#define ERRNOMEMORY		12
#define ERRFSMSTATE		14
#define ERRADDRESSRANGE		15
#define ERRADDRESSMISALIGN	16
#define ERRBLOCKLEN		17
#define ERRERASESEQERR		18
#define ERRERASEPARAM		19
#define ERRPROT			20
#define	ERRCARDLOCKED		21
#define ERRCRC			22
#define	ERRILLEGALCOMMAND	23
#define ERRECCFAILED		24
#define ERRCCERR		25
#define	ERRUNKNOWN		26
#define ERRUNDERRUN		27
#define ERROVERRUN		28
#define ERRCSDOVERWRITE		29
#define ERRERASERESET		30
#define ERRDATATIMEOUT		31
#define ERRUNDERWRITE		32
#define ERROVERREAD		33
#define ERRENDBITERR		34
#define ERRDCRC			35
#define ERRSTARTBIT		36
#define ERRTIMEROUT		37
#define ERRCARDNOTREADY		38
#define ERRBADFUNC		39
#define ERRPARAM		40
#define ERRNOTFOUND		41
#define ERRWRTPRT		42

/**
  * Controller Register definitions
  * This is the enumeration of the registers on the host controller. The
  * individual values for the members are the offsets of the individual
  * registers. The registers need to be updated according to IP release 2.10
  */
typedef enum synop_regs {
	CTRL     = 0x0,	/** Control */
	PWREN    = 0x4,	/** Power-enable */
	CLKDIV   = 0x8,	/** Clock divider */
	CLKSRC   = 0xC,	/** Clock source */
	CLKENA   = 0x10,/** Clock enable */
	TMOUT    = 0x14,/** Timeout */
	CTYPE    = 0x18,/** Card type */
	BLKSIZ   = 0x1C,/** Block Size */
	BYTCNT   = 0x20,/** Byte count */
	INTMSK   = 0x24,/** Interrupt Mask */
	CMDARG   = 0x28,/** Command Argument */
	CMD      = 0x2C,/** Command */
	RESP0    = 0x30,/** Response 0 */
	RESP1    = 0x34,/** Response 1 */
	RESP2    = 0x38,/** Response 2 */
	RESP3    = 0x3C,/** Response 3 */
	MINTSTS  = 0x40,/** Masked interrupt status */
	RINTSTS  = 0x44,/** Raw interrupt status */
	STATUS   = 0x48,/** Status */
	FIFOTH   = 0x4C,/** FIFO threshold */
	CDETECT  = 0x50,/** Card detect */
	WRTPRT   = 0x54,/** Write protect */
	GPIO     = 0x58,/** General Purpose IO */
	TCBCNT   = 0x5C,/** Transferred CIU byte count */
	TBBCNT   = 0x60,/** Transferred host/DMA to/from byte count */
	DEBNCE   = 0x64,/** Card detect debounce */
	USRID    = 0x68,/** User ID */
	VERID    = 0x6C,/** Version ID */
	HCON     = 0x70,/** Hardware Configuration */
	Reserved = 0x74,/** Reserved */
	BMOD     = 0x80,
	PLDMND   = 0x84,
	DBADDR   = 0x88,
	IDSTS    = 0x8C,
	IDINTEN  = 0x90,
	DSCADDR  = 0x94,
	BUFADDR  = 0x98,
	FIFODAT  = 0x100,/** FIFO data read write */
} Controller_Reg ;
#if 1
typedef struct
{
	volatile u32	des0;
	volatile u32	des1;
	volatile u32	des2;
	volatile u32	des3;
}mshc_idmac_info;

#endif
/* Misc Defines */
#define HCON_NUM_CARDS(x)       ((((x&0x3E)>>1))+1)
#define DEFAULT_DEBNCE_VAL         0x0FFFFF
#define GET_FIFO_DEPTH(x)       ((((x)&0x0FFF0000)>>16)+1)
#define FIFO_WIDTH 4		/* in bytes */
#define MAX_DIVIDER_VALUE	0xff
#define CLK_ONLY_CMD	0x80200000
#define SET_RCA(x,y)	((x)|=(y<<16))

typedef void (*synopmob_postproc_callback)(void *,u32 *) ;


/* Set the y bits in x */
#define SET_BITS(x,y)	((x)|=(y))

/* Unset the y bits in x */
#define UNSET_BITS(x,y)	((x)&=(~(y)))


/* Control register definitions */
#define CTRL_RESET	0x00000001
#define FIFO_RESET	0x00000002
#define DMA_RESET	0x00000004
#define INT_ENABLE	0x00000010
#define DMA_ENABLE	0x00000020
#define READ_WAIT	0x00000040
#define SEND_IRQ_RESP	0x00000080
#define ABRT_READ_DATA	0x00000100
#define SEND_CCSD	0x00000200
#define SEND_AS_CCSD	0x00000400
#define ENABLE_OD_PULLUP 0x01000000
#define IDMA_ENABLE			0x02000000

/* Interrupt mask defines */

#define INTMSK_CDETECT  0x00000001
#define INTMSK_RESP_ERR 0x00000002
#define INTMSK_CMD_DONE 0x00000004
#define INTMSK_DAT_OVER 0x00000008
#define INTMSK_TXDR     0x00000010
#define INTMSK_RXDR     0x00000020
#define INTMSK_RCRC     0x00000040
#define INTMSK_DCRC     0x00000080
#define INTMSK_RTO      0x00000100
#define INTMSK_DRTO     0x00000200
#define INTMSK_HTO      0x00000400
#define INTMSK_FRUN     0x00000800
#define INTMSK_HLE      0x00001000
#define INTMSK_SBE      0x00002000
#define INTMSK_ACD      0x00004000
#define INTMSK_EBE      0x00008000
/*SDIO interrupts are catered from bit 15 through 31*/
#define INTMSK_SDIO_INTR	0xffff0000
#define INTMSK_SDIO_CARD(x)     (1<<(16+x))
#define INTMSK_ALL_ENABLED      0xffffffff


/* CMD Register Defines */
#define CMD_RESP_EXP_BIT	0x00000040
#define CMD_RESP_LENGTH_BIT	0x00000080
#define CMD_CHECK_CRC_BIT	0x00000100
#define CMD_DATA_EXP_BIT	0x00000200
#define CMD_RW_BIT		0x00000400
#define	CMD_TRANSMODE_BIT	0x00000800
#define CMD_SENT_AUTO_STOP_BIT	0x00001000
#define CMD_WAIT_PRV_DAT_BIT	0x00002000
#define CMD_ABRT_CMD_BIT	0x00004000
#define CMD_SEND_INIT_BIT	0x00008000
#define CMD_SEND_CLK_ONLY	0x00200000
#define CMD_READ_CEATA		0x00400000
#define CMD_CCS_EXPECTED	0x00800000
#define CMD_DONE_BIT		0x80000000

#define SET_CARD_NUM(x,y) ((x)|=((y)<<16))
#define SET_CMD_INDEX(x,y) ((x)|=(y&0x3f))

/* Status register bits */
#define STATUS_DAT_BUSY_BIT	0x00000200

#define SDIO_RESET  100  //To differentiate CMD52 for IORESET and
                         //other rd/wrs.
#define SDIO_ABORT  101  //To differentiate CMD52 for IO ABORT and
                         //other rd/wrs.


#define UNADD_OFFSET	200
#define UNADD_CMD7	207
//#define	WCMC52		252
#define CMD532CMD52	252  /* SDIO_RW_DIRECT           ??                 R5  */
#define WCMD53		253
#define	WCMD60		260
#define WCMD61		261
#define ACMD6		206
#define SD_CMD8		208  /*This is added to support SD 2.0 (SDHC) cards*/

//#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define PLATFORM	"Mozart"
//#endif
#define DRIVER_VERSION "1.0.0.2"
#define MAINTAINER "Poplar, Yang"
#endif
