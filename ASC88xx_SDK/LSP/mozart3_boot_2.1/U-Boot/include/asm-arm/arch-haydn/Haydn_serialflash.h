#ifndef __HAYDN_SERIALFLASH_H__
#define __HAYDN_SERIALFLASH_H__

#include <spi_flash.h>

#define SPANSION_MANUFACT 0x010000
#define SPANSION_ID_S25FL064A  0x0216

#define SPANSION_S25FL064A_CHIPSIZE 0x800000
#define EVM_FLASH_BANK0_SIZE SPANSION_S25FL064A_CHIPSIZE

#define SPANSION_S25FL064A_REGION_NUM 1
#define SPANSION_S25FL064A_REGION0_SECTOR_NUM 128
#define SPANSION_S25FL064A_REGION1_SECTOR_NUM   0

#define FLASH_REGION_NUM SPANSION_S25FL064A_REGION_NUM
#define FLASH_REGION0_SECTOR_NUM SPANSION_S25FL064A_REGION0_SECTOR_NUM
#define FLASH_REGION1_SECTOR_NUM SPANSION_S25FL064A_REGION1_SECTOR_NUM

#define EVM_FLASH_REGION_NUM FLASH_REGION_NUM
#define EVM_FLASH_REGION0_SECTOR_NUM FLASH_REGION0_SECTOR_NUM
#define EVM_FLASH_REGION1_SECTOR_NUM FLASH_REGION1_SECTOR_NUM
#define EVM_FLASH_SECTOR_NUM (EVM_FLASH_REGION0_SECTOR_NUM+EVM_FLASH_REGION1_SECTOR_NUM)

#define K (1024)

#define BAUDRATE_DIVIDER 0x2

//static const ulong flash_sectors[EVM_FLASH_REGION_NUM] = {EVM_FLASH_REGION0_SECTOR_NUM};
//static const ulong flash_sector_size[EVM_FLASH_REGION_NUM] = {64*K};

#define SSI_MMR_BASE 0x40800000
#define SSI_MMR_CTRLR0           (0x0  + SSI_MMR_BASE)
#define SSI_MMR_CTRLR1           (0x4  + SSI_MMR_BASE)
#define SSI_MMR_SSIENR           (0x08 + SSI_MMR_BASE)
#define SSI_MMR_SER              (0x10 + SSI_MMR_BASE)
#define SSI_MMR_BAUDR            (0x14 + SSI_MMR_BASE)
#define SSI_MMR_TXFTLR           (0x18 + SSI_MMR_BASE)
#define SSI_MMR_RXFTLR           (0x1C + SSI_MMR_BASE)
#define SSI_MMR_TXFLR            (0x20 + SSI_MMR_BASE)
#define SSI_MMR_RXFLR            (0x24 + SSI_MMR_BASE)
#define SSI_MMR_SR               (0x28 + SSI_MMR_BASE)
#define SSI_MMR_IMR              (0x2C + SSI_MMR_BASE)
#define SSI_MMR_DMACR            (0x4C + SSI_MMR_BASE)
#define SSI_MMR_DMATDLR          (0x50 + SSI_MMR_BASE)
#define SSI_MMR_DMARDLR          (0x54 + SSI_MMR_BASE)
#define SSI_MMR_DR               (0x60 + SSI_MMR_BASE)

#define APB_MMR_BASE 0x99500000
#define APB_MMR_DMA0_SRC_ADDR (0x90 + APB_MMR_BASE)
#define APB_MMR_DMA0_DES_ADDR (0x94 + APB_MMR_BASE)
#define APB_MMR_DMA0_LLP      (0x98 + APB_MMR_BASE)
#define APB_MMR_DMA0_CTRL     (0x9c + APB_MMR_BASE)
#define APB_MMR_DMA1_SRC_ADDR (0xa0 + APB_MMR_BASE)
#define APB_MMR_DMA1_DES_ADDR (0xa4 + APB_MMR_BASE)
#define APB_MMR_DMA1_LLP      (0xa8 + APB_MMR_BASE)
#define APB_MMR_DMA1_CTRL     (0xac + APB_MMR_BASE)

/**************************
 *  APBC_DMA_CTRL_FIELDS  *
 **************************/
//write part
#define APBC_DMA_CTRL_OP_EN (1 << 0)
#define APBC_DMA_CTRL_INTR_CMPT_EN (1 << 2)
#define APBC_DMA_CTRL_INTR_LL_EN   (1 << 3) //Enable link-list update interrupt flag. 
                                            //This interrupt can be used to notify individual continuous memory DMA is completed.
#define APBC_DMA_CTRL_INTR_ERR_EN (1 << 5)                                            
#define APBC_DMA_CTRL_TRANS_TYPE_AHB2AHB (0 << 6)
#define APBC_DMA_CTRL_TRANS_TYPE_AHB2APB (1 << 6)
#define APBC_DMA_CTRL_TRANS_TYPE_APB2AHB (2 << 6)
#define APBC_DMA_CTRL_TRANS_TYPE_APB2APB (3 << 6)
#define APBC_DMA_CTRL_SRC_ADDR_NO_INC (0 << 8)
#define APBC_DMA_CTRL_SRC_ADDR_POS_INC (1 << 8)
#define APBC_DMA_CTRL_SRC_ADDR_NEG_INC (2 << 8)
#define APBC_DMA_CTRL_DES_ADDR_NO_INC (0 << 10)
#define APBC_DMA_CTRL_DES_ADDR_POS_INC (1 << 10)
#define APBC_DMA_CTRL_DES_ADDR_NEG_INC (2 << 10)
#define APBC_DMA_CTRL_REQ_TYPE (1 << 16)
#define APBC_DMA_CTRL_BURST_MODE (1 << 17)
#define APBC_DMA_CTRL_DATASZ_BYTE (0 << 18)
#define APBC_DMA_CTRL_DATASZ_HALF (1 << 18)
#define APBC_DMA_CTRL_DATASZ_WORD (2 << 18)
#define APBC_DMA_CTRL_DATASZ_DOUBLE (3 << 18)
//read part
#define APBC_DMA_CTRL_INTR_CMPT_OCCURS_MASK (1 << 1)
#define APBC_DMA_CTRL_INTR_ERR_OCCURS_MASK (1 << 4)
//macro
#define APBC_DMA_CTRL_REQ_SEL(val) ((0xff & val) << 12)


#define DMA_DESC_BASE 0x00700000

#define SRWD_MASK 0x80
#define BP_MASK   0x1c
#define WEL_MASK  0x02
#define WIP_MASK  0x01

#define SERIAL_FLASH_LOGICAL_BASE				0x10000000	//Serial Flash Logical Address

#define FLASH_UNKNOWN	0xFFFF		/* unknown flash type			*/

#define SPI_PAGEPROGRAM_MAXSIZE 256

#define CMD_READ_ID			0x9f

#endif
