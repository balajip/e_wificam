#ifndef _DW_MMC_H_
#define _DW_MMC_H_

#include <asm/arch/hardware.h>

#define FIFO_DEPTH          (16)
#define FIFO_RX_THRESHOLD   (FIFO_DEPTH / 2 - 1)
#define FIFO_TX_THRESHOLD   (FIFO_DEPTH / 2)


typedef struct _dw_mmc_host {
	unsigned int	ctrl;       /* 0x00 */
	unsigned int	pwren;      /* 0x04 */
	unsigned int	clkdiv;     /* 0x08 */
	unsigned int	clksrc;     /* 0x0c */
	unsigned int	clkena;     /* 0x10 */
	unsigned int	tmout;      /* 0x14 */
	unsigned int	ctype;      /* 0x18 */
	unsigned int	blksiz;     /* 0x1c */
	unsigned int	bytcnt;     /* 0x20 */
	unsigned int	intmask;    /* 0x24 */
	unsigned int	cmdarg;     /* 0x28 */
	unsigned int	cmd;        /* 0x2c */
	unsigned int	resp0;      /* 0x30 */
	unsigned int	resp1;      /* 0x34 */
	unsigned int	resp2;      /* 0x38 */
	unsigned int	resp3;      /* 0x3c */
	unsigned int	mintsts;    /* 0x40 */
	unsigned int	rintsts;    /* 0x44 */
	unsigned int	status;     /* 0x48 */
	unsigned int	fifoth;     /* 0x4c */
	unsigned int	cdetect;    /* 0x50 */
	unsigned int	wrtprt;     /* 0x54 */
	unsigned int	gpio;       /* 0x58 */
	unsigned int	tcbcnt;     /* 0x5c */
	unsigned int	tbbcnt;     /* 0x60 */
	unsigned int	debnce;     /* 0x64 */
	unsigned int	usrid;      /* 0x68 */
	unsigned int	verid;      /* 0x6c */
	unsigned int	hcon;       /* 0x70 */
	unsigned int	rsv[3];     /* 0x74~0x7f */
	unsigned int	bmod;       /* 0x80 */
	unsigned int	pldmnd;     /* 0x84 */
	unsigned int	dbaddr;     /* 0x88 */
	unsigned int	idsts;      /* 0x8c */
	unsigned int	idinten;    /* 0x90 */
	unsigned int	dscaddr;    /* 0x94 */
	unsigned int	bufaddr;    /* 0x98 */
	unsigned int	rsv1[25];   /* 0x9c~0xff */
	unsigned int	data;       /* 0x100 */
} dw_mmc_host_t;


typedef struct _dw_mmc {
	dw_mmc_host_t *   host;
	int               cardnum; /* card number within the host */
} dw_mmc_t;


/* CTRL */
#define CTRL_USE_INTERNAL_DMAC    BIT_MASK(25)
#define CTRL_DMA_ENABLE           BIT_MASK(5)
#define CTRL_INT_ENABLE           BIT_MASK(4)
#define CTRL_DMA_RESET            BIT_MASK(2)
#define CTRL_FIFO_RESET           BIT_MASK(1)
#define CTRL_CONTROLLER_RESET     BIT_MASK(0)

/* CLKDIV */
#define CLKDIV_DIV_MAX            (0xff)

/* CLKENA */
#define CLKENA_CCLK_LOW_POWER(x)  (0x10000UL << (x))
#define CLKENA_CCLK_ENABLE(x)     (1UL << (x))

/* CTYPE */
#define CTYPE_WIDTH_MASK          (0x00010001)
#define CTYPE_8_BIT               (0x00010000)
#define CTYPE_4_BIT               (0x00000001)

/* CMD */
#define CMD_START_CMD                     BIT_MASK(31)
#define CMD_UPDATE_CLOCK_REGISTERS_ONLY   BIT_MASK(21)
#define CMD_CARD_NUMBER(x)                ((x) <<  16)
#define CMD_SEND_INITIALIZATION           BIT_MASK(15)
#define CMD_TRANSFER_MODE                 BIT_MASK(11)
#define     CMD_BLOCK_XFER                (0 << 11)
#define     CMD_STREAM_XFER               (1 << 11)
#define CMD_READ_WRITE                    BIT_MASK(10)
#define     CMD_READ_CARD                 (0 << 10)
#define     CMD_WRITE_CARD                (1 << 10)
#define CMD_DATA_EXPECTED                 BIT_MASK( 9)
#define CMD_CHECK_RESPONSE_CRC            BIT_MASK( 8)
#define CMD_RESPONSE_LENGTH               BIT_MASK( 7)
#define CMD_RESPONSE_EXPECT               BIT_MASK( 6)
#define CMD_INDEX(c)                      ((c) <<   0)

/* INTMASK / MINTSTS / RINTSTS */
#define INT_EBE                   BIT_MASK(15)
#define INT_ACD                   BIT_MASK(14)
#define INT_SBE                   BIT_MASK(13)
#define INT_HLE                   BIT_MASK(12)
#define INT_FRUN                  BIT_MASK(11)
#define INT_HTO                   BIT_MASK(10)
#define INT_DRTO                  BIT_MASK( 9)
#define INT_RTO                   BIT_MASK( 8)
#define INT_DCRC                  BIT_MASK( 7)
#define INT_RCRC                  BIT_MASK( 6)
#define INT_RXDR                  BIT_MASK( 5)
#define INT_TXDR                  BIT_MASK( 4)
#define INT_DTO                   BIT_MASK( 3)
#define INT_CD                    BIT_MASK( 2)
#define INT_RE                    BIT_MASK( 1)
#define INT_CDT                   BIT_MASK( 0)

/* STATUS */
#define STATUS_FIFO_COUNT(x)      (((x) >> 17) & 0x1fff)

/* FIFOTH */
#define FIFOTH_RX_WMARK(x)        ((x) << 16)
#define FIFOTH_TX_WMARK(x)        ((x) <<  0)

#define BITS_PER_LONG 32
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))

#endif /* !_DW_MMC_H_ */
