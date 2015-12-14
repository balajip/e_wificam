#ifndef EVM_SD_H
#define EVM_SD_H

//#include <configs/mozartevm.h>
#include <config.h>
#include <common.h>

//--------------------------------------------------
//                 Definitions
//--------------------------------------------------
//SD MMR
#define SD_MMR_BASE    0x90000000
#define SD_MMR_CTRL    ( SD_MMR_BASE + 0x00 )
#define SD_MMR_PWREN   ( SD_MMR_BASE + 0x04 )
#define SD_MMR_CLKDIV  ( SD_MMR_BASE + 0x08 )
#define SD_MMR_CLKSRC  ( SD_MMR_BASE + 0x0C )
#define SD_MMR_CLKENA  ( SD_MMR_BASE + 0x10 )
#define SD_MMR_TMOUT   ( SD_MMR_BASE + 0x14 )
#define SD_MMR_CTYPE   ( SD_MMR_BASE + 0x18 )
#define SD_MMR_BLKSIZ  ( SD_MMR_BASE + 0x1C )
#define SD_MMR_BYTCNT  ( SD_MMR_BASE + 0x20 )
#define SD_MMR_INTMASK ( SD_MMR_BASE + 0x24 )
#define SD_MMR_CMDARG  ( SD_MMR_BASE + 0x28 )
#define SD_MMR_CMD     ( SD_MMR_BASE + 0x2C )
#define SD_MMR_RESP0   ( SD_MMR_BASE + 0x30 )
#define SD_MMR_RESP1   ( SD_MMR_BASE + 0x34 )
#define SD_MMR_RESP2   ( SD_MMR_BASE + 0x38 )
#define SD_MMR_RESP3   ( SD_MMR_BASE + 0x3C )
#define SD_MMR_MINSTS  ( SD_MMR_BASE + 0x40 )
#define SD_MMR_RINSTS  ( SD_MMR_BASE + 0x44 )
#define SD_MMR_STATUS  ( SD_MMR_BASE + 0x48 )
#define SD_MMR_FIFOTH  ( SD_MMR_BASE + 0x4C )
#define SD_MMR_CDETECT ( SD_MMR_BASE + 0x50 )
#define SD_MMR_WRTPRT  ( SD_MMR_BASE + 0x54 )
#define SD_MMR_GPIO    ( SD_MMR_BASE + 0x58 )
#define SD_MMR_TCBCNT  ( SD_MMR_BASE + 0x5C )
#define SD_MMR_TBBCNT  ( SD_MMR_BASE + 0x60 )
#define SD_MMR_DEBNCE  ( SD_MMR_BASE + 0x64 )
#define SD_MMR_USRID   ( SD_MMR_BASE + 0x68 )
#define SD_MMR_VERID   ( SD_MMR_BASE + 0x6C )
#define SD_MMR_HCON    ( SD_MMR_BASE + 0x70 )
#define SD_MMR_BMOD    ( SD_MMR_BASE + 0x80 )
#define SD_MMR_PLDMND  ( SD_MMR_BASE + 0x84 )
#define SD_MMR_DBADDR  ( SD_MMR_BASE + 0x88 )
#define SD_MMR_IDSTS   ( SD_MMR_BASE + 0x8C )
#define SD_MMR_IDINTEN ( SD_MMR_BASE + 0x90 )
#define SD_MMR_DSCADDR ( SD_MMR_BASE + 0x94 )
#define SD_MMR_BUFADDR ( SD_MMR_BASE + 0x98 )
#define SD_MMR_DATA    ( SD_MMR_BASE + 0x100 )

//SD_MMR_CTRL
#define SD_CTRL_CONTROLLER_RESET     1 //reset BIU/CIU interfaces, CIU and state machines, start_cmd bit of Cmd register
#define SD_CTRL_FIFO_RESET           (1<<1)
#define SD_CTRL_DMA_RESET            (1<<2)
#define SD_CTRL_INT_ENABLE           (1<<4)
#define SD_CTRL_DMA_ENABLE           (1<<5)
#define SD_CTRL_INTERNAL_DMAC_ENABLE (1<<25)         

//SD_MMR_CMD
#define SD_CMD_STARTCMD   (1<<31)

//SD_MMR_INTMASK
#define SD_INTMASK_CD   (0x1)
#define SD_INTMASK_RE   (1<<1)
#define SD_INTMASK_CMDDONE (1<<2)
#define SD_INTMASK_DTO  (1<<3)
#define SD_INTMASK_RXDR (1<<5)
#define SD_INTMASK_RCRC (1<<6)
#define SD_INTMASK_DCRC (1<<7)
#define SD_INTMASK_RTO  (1<<8)
#define SD_INTMASK_DRTO (1<<9)
#define SD_INTMASK_HTO  (1<<10)
#define SD_INTMASK_FRUN (1<<11)
#define SD_INTMASK_HLE  (1<<12)
#define SD_INTMASK_SBE  (1<<13)
#define SD_INTMASK_ACD  (1<<14)
#define SD_INTMASK_EBE  (1<<15)

//SD misc
#define SD_NUMBER  1
#define SD_DESCRIPTOR_BASE    0xc00000
#define SD_BLOCK_SIZE   512

//SD status
#define SD_STATE_IDLE  0
#define SD_STATE_READY 1
#define SD_STATE_IDENT 2
#define SD_STATE_STBY  3
#define SD_STATE_TRAN  4
#define SD_STATE_DATA  5
#define SD_STATE_RCV   6
#define SD_STATE_PRG   7
#define SD_STATE_DIS   8

//--------------------------------------------------
//                     Commands
//--------------------------------------------------
//CMD REG FIELDS
#define SD_CMD_START_CMD              (1<<31)
#define SD_CMD_BOOT_MODE              (1<<27)
#define SD_CMD_DISABLE_BOOT           (1<<26)
#define SD_CMD_EXPECT_BOOT_ACK        (1<<25)
#define SD_CMD_ENABLE_BOOT            (1<<24)
#define SD_CMD_CCS_EXPECTED           (1<<23)
#define SD_CMD_READ_CEATA_DEV         (1<<22)
#define SD_CMD_UPDATE_CLKREGS_ONLY    (1<<21)
#define SD_CMD_SEND_INITIALIZATION    (1<<15)
#define SD_CMD_STOP_ABORT_CMD         (1<<14)
#define SD_CMD_WAIT_PRVDATA_COMPLETE  (1<<13)
#define SD_CMD_SEND_AUTO_STOP         (1<<12)
#define SD_CMD_TRANSFER_MODE          (1<<11)
#define SD_CMD_READ_FROM_CARD         (0<<10)
#define SD_CMD_WRITE_TO_CARD          (1<<10)
#define SD_CMD_DATA_EXPECTED          (1<<9)
#define SD_CMD_CHECK_RESP_CRC         (1<<8)
#define SD_CMD_RESP_LENGTH            (1<<7)
#define SD_CMD_RESP_EXPECT            (1<<6)

#define SD_CMD_CMDINDEX(index) (index & 0x3F)

//cmd
#define SD_CMD0           0x80000000
#define SD_CMD2           0x800001c2
#define SD_CMD3           0x80000143
#define SD_CMD5           0x80000005
#define SD_CMD7           0x80000147
#define SD_CMD8           0x80000148
#define SD_CMD12          0x8000004c
#define SD_CMD13          0x8000014d
#define SD_CMD16          0x80000550
#define SD_CMD17          0x80001351
#define SD_CMD18          0x80003352
#define SD_CMD25          0x80001759
#define SD_CMD55          0x80000177
//acmd
#define SD_ACMD41         0x80000069

#define SD_SINGLEBLK_READ     SD_CMD17
#define SD_MULTIPLEBLK_READ   SD_CMD18
#define SD_MULTIPLEBLK_WRITE  SD_CMD25
//--------------------------------------------------
//                     Functions
//--------------------------------------------------

//===== Initialization =====
/* 
 * @ Func_Name  : SD_Controller_Init
 * @ Func_Brief : Initialize the CTRL of SD Controller, it includes the following steps,
 *                -0. Reset the related devices
 *                -1. Configure Controller Register
 *                -2. Enable Power to Cards
 *                -3. Set Interrupt Masks
 *                -4. Enumerate Card Stack
 *                -5. Change Clock Source Assignment
 */
extern int SD_Controller_Init(void);

//===== Identification mode =====
/* 
 * @ Func_Name  : SD_Card_IdentificationMode
 * @ Func_Brief : Finish the Identification Mode, and go to the STANDY-BY Mode.
 */
extern int SD_Card_IdentificationMode(void) ;

//===== MISC =====
extern int SD_Check_CMDDone(void) ;
extern int SD_Set_CMD( unsigned long cmd_arg, unsigned long cmd ) ;
extern int SD_Set_ACMD( unsigned long cmd_arg ) ;
extern void SD_Check_ERR( unsigned long interrupt ) ;

#define v_inl(addr)		(*((volatile unsigned long *)(addr)))

extern int isSDHC ;//1:SDHC ; 0 :SDSC/others

#define SD_TIMEOUT_CHECK_VAL 1000

#endif

