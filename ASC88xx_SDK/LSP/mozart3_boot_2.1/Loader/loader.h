/*
 * loader.h
 *
 * Copyright 2011 VN Inc. All rights reserved.
 *
 */

#ifndef _LOADER_H_
#define _LOADER_H_

#define LOADER_VERSION                  0x14031801

#define MAX_DRAM_NUMBERS   (2)


#define LOADER_BASE    0x200
#define LOADER_SIZE    0xE00
/* ARMBOOT should be allocated at any location (may not be address zero) */
#define ARMBOOT_DESTADDR                (0x2000)
#define ARMBOOT_OFFSET                  (0x1000) /* offset in a block */
#define ARMBOOT_SIZE_OFFSET             (0x80)

#define BOOT_SF       (0)
#define BOOT_SD       (1)
#define BOOT_NF2K     (2)
#define BOOT_NF4K     (2)
#ifndef WHERE_IS_BOOTLOADER
#define WHERE_IS_BOOTLOADER             (BOOT_SF)
#endif

#define DDR2          (0)
#define DDR3          (1)

#define THROUGH_PUB   (0)
#define BYPASS_PUB    (1)

#define BANKS_4       (1)
#define BANKS_8       (2)

#define DUALBOOT_MARKER   "DUAL"

/* handy sizes */
#define SZ_16         (0x00000010)
#define SZ_256        (0x00000100)
#define SZ_512        (0x00000200)
#define SZ_1K         (0x00000400)
#define SZ_2K         (0x00000800)
#define SZ_4K         (0x00001000)
#define SZ_8K         (0x00002000)
#define SZ_16K        (0x00004000)
#define SZ_32K        (0x00008000)
#define SZ_64K        (0x00010000)
#define SZ_128K       (0x00020000)
#define SZ_256K       (0x00040000)
#define SZ_512K       (0x00080000)
#define SZ_1M         (0x00100000)
#define SZ_2M         (0x00200000)
#define SZ_4M         (0x00400000)
#define SZ_8M         (0x00800000)
#define SZ_16M        (0x01000000)
#define SZ_32M        (0x02000000)
#define SZ_64M        (0x04000000)
#define SZ_128M       (0x08000000)
#define SZ_256M       (0x10000000)
#define SZ_512M       (0x20000000)
#define SZ_1G         (0x40000000)
#define SZ_2G         (0x80000000)

/* concatenatation */
#define cat(x,y)    x ## y
#define xcat(x,y)   cat(x,y)


#include "config.h"
#include "default-config.h"

#include "mozart.h"


/*-----------------------------------------------------------------------------
 * Exception
 *----------------------------------------------------------------------------*/
#define F_BIT                           0x40
#define I_BIT                           0x80
#define SUPERVISOR_MODE                 0x13
#define CPU_INIT_DATA                   (SUPERVISOR_MODE+F_BIT+I_BIT)

/*-----------------------------------------------------------------------------
 * magic number
 *----------------------------------------------------------------------------*/
#define MG_0                            0x82451282
#define MG_1                            0x28791166

/*-----------------------------------------------------------------------------
 * Header Informations
 *----------------------------------------------------------------------------*/
#define HEADER_LOADER_BASE              0x08   /* for serial flash and nand flash */
#define HEADER_GENERAL_CONFIG           0x0c   /* for serial flash and nand flash */
#define HEADER_LOADER_SIZE              0x10   /* for serial flash */

/*-----------------------------------------------------------------------------
 * Memory Map
 *----------------------------------------------------------------------------*/
#define LOADER_VERSION_ADDR             (VPL_BRC_RAM_BASE + 0x1000)
#define TRAINING_STATUS_ADDR            (LOADER_VERSION_ADDR + 0x10)

/* SYSTEM_DDR_MEM: system dram before remap */
#if  CONFIG_DDR_PORT_0 == 1 \
  && (  (CONFIG_DDR_PORT_1 == 1 && CONFIG_SYSTEM_DDR == 0) \
     || CONFIG_DDR_PORT_1 == 0)
  #define SYSTEM_DDR_MEM              VPL_DDRSDMC_0_MEM_BASE
#elif CONFIG_DDR_PORT_1 == 1 \
   && (  (CONFIG_DDR_PORT_0 == 1 && CONFIG_SYSTEM_DDR == 1) \
      || CONFIG_DDR_PORT_0 == 0)
  #define SYSTEM_DDR_MEM              VPL_DDRSDMC_1_MEM_BASE
#else
#error Invaliad system memory address!
#endif


/*-----------------------------------------------------------------------------
 * SSI definitions
 *----------------------------------------------------------------------------*/

#define CTRLR0_CFS                      0x7000          // control frame size 8-bit
#define CTRLR0_TMOD                     0x300           // transfer mod is EEPROM Read
#define CTRLR0_SCPOL                    0x80            // inactive state of serial clock is high
#define CTRLR0_SCPH                     0x40            // seriral clock toggles at start of first data bit
#define CTRLR0_FRF                      0x00            // frame format is SPI
#define CTRLR0_DFS                      0x07            // data frame size 8-bit
#define SSI_CTRLR0_DATA                 (CTRLR0_CFS + CTRLR0_TMOD + CTRLR0_SCPOL + CTRLR0_SCPH + CTRLR0_FRF + CTRLR0_DFS)

#define SR_TFE                          0x4             // Transmit FIFO Empty
#define SR_BUSY                         0x1

#define SSI_BAUDR_DATA                  0x0000000F      // 5 MHz
#define SSI_DMACR_DATA                  0x01            // enable Rx DMA
#define SSI_DMARDLR_DATA                0x03            // 4 data entries, burst mode

/*-----------------------------------------------------------------------------
 * Serial Flash definitions
 *----------------------------------------------------------------------------*/
#define SFLASH_SECTOR_SIZE              0x8000      // 32K
// command code
#define SFLASH_READ                     0x03
#define SFLASH_FAST_READ                0x0B


/*-----------------------------------------------------------------------------
 * APBC definitions
 *----------------------------------------------------------------------------*/
#define APBC_DMA_CHN_SSIC_RX            0x2

#define DMA_LLP_DATA                    0x0

#define CTRL_DATA_SZ                    0x00000000      // 8-bit
#define CTRL_BURST_MODE                 0x00000010      // with burst length 4
#define CTRL_REQ_TYPE                   0x00010000      // cease DMA transfer and assert DMA GRANT signal after the CYC is reduced by one
#define CTRL_REQ_SEL                    0x00001000
#define CTRL_DES_ADDR_INC               0x00000400      // destination address positive increment
#define CTRL_SRC_ADDR_INC               0x00000000      // source address no increment
#define CTRL_TRANS_TYPE                 0x00000080      // transfer type : APB to AHB
#define CTRL_INTR_CMPT_EN               0x00000004      // enalbe completion interrupt flag
#define DMA_CTRL_DATA                   (CTRL_DATA_SZ + CTRL_BURST_MODE + CTRL_REQ_TYPE + CTRL_REQ_SEL + CTRL_DES_ADDR_INC + CTRL_SRC_ADDR_INC + CTRL_TRANS_TYPE)

#define DMA_CTRL_EN                     0x1
#define DMA_CTRL_CMPT_OCCUR             0x2
#define DMA_CTRL_CYC_OFFSET             20


/*-----------------------------------------------------------------------------
 * SD card information
 *----------------------------------------------------------------------------*/
#define SDCARD_BLKSZ                    0x200

#define DESC0_DATA                      0x8000002c
#define DESC1_DATA                      0x00001000      // buf size : 4K (SRAM size)
#define DESC2_DATA                      0x00001000      // buf address : 4K (SRAM base)
#define DESC3_DATA                      0x00002000

#define SD_DESC_OFFSET                  4112            // 4K + 16

/*-----------------------------------------------------------------------------
 * SD card commands
 *----------------------------------------------------------------------------*/
#define SD_CMD0                         0x80208000
#define SD_CMD2                         0x800001c2
#define SD_CMD3                         0x80000143
#define SD_CMD7                         0x80000147
#define SD_CMD8                         0x80000148
#define SD_CMD16                        0x80000550
#define SD_CMD55                        0x80000177
#define SD_ACMD41                       0x80000069
#define SD_READ_MULTI_BLK_CMD           0x80003352


#endif /* !_LOADER_H_ */
