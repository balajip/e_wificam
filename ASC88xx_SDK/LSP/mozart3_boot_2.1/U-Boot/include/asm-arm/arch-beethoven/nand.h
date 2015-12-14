#ifndef __BEETHOVEN_NAND_H__
#define __BEETHOVEN_NAND_H__

/*
 ********************************************************
 *                 Mozart NFC MMRs
 ********************************************************
 */
#define VPL_NFC_MMR_BASE 0xB0000000
#define VPL_NFC_MMR_STARTADDR   ( VPL_NFC_MMR_BASE + 0x1300 )
#define VPL_NFC_MMR_CONF        ( 0x00 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_CTRL        ( 0x04 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_COMM        ( 0x08 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_ADDR0L      ( 0x0c + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_ADDR1L      ( 0x10 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_DATA        ( 0x14 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_PB0         ( 0x18 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_STATE       ( 0x1c + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp0    ( 0x20 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp1    ( 0x24 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp2    ( 0x28 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp3    ( 0x2c + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp4    ( 0x30 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp5    ( 0x34 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp6    ( 0x38 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_FLookUp7    ( 0x3c + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_ECCState    ( 0x40 + VPL_NFC_MMR_STARTADDR ) 
#define VPL_NFC_MMR_ADDR0H      ( 0x44 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_ADDR1H      ( 0x48 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_LookUpEN    ( 0x4c + VPL_NFC_MMR_STARTADDR )
//--DMA MMR--
#define VPL_NFC_MMR_DMAADDR    ( 0x80 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_DMACTRL    ( 0x84 + VPL_NFC_MMR_STARTADDR )
#define VPL_NFC_MMR_DMACNTR    ( 0x88 + VPL_NFC_MMR_STARTADDR )

//--DMA SETTING
#define VPL_NFC_ADDR_MASK        0x0fffffff
#define VPL_NFC_DMA_ENABLE       0x01000000
#define VPL_NFC_DMA_DIR_BUF2AHB  0x10000
#define VPL_NFC_DMA_DIR_AHB2BUF  0x0
#define VPL_NFC_DMA_SIZE         ( 0 << 14 )
#define VPL_NFC_DMA_BURST        ( 2 << 11 )

//--Useful Flag
#define VPL_NF_TRANSFER_READ  0x01
#define VPL_NF_TRANSFER_WRITE 0x00

//--Errors--
#define VPL_NF_ERR_NO_ERRORS                0x00u
#define VPL_NF_ERR_INVALID_PARAMETER        0x01u /** error - invalid parameter */
#define VPL_NF_ERR_INVALID_ADDRESS          0x03u /** error - invalid address */
#define VPL_NF_ERR_DMA_TRANSFER             0x04u /** DMA transmission error */
#define VPL_NF_ERR_MEMORY_BUSY              0x07u /** error - memory doesn't response on request */
#define VPL_NF_ERR_PROTECTED                0x08u /** error - try Write or Erase protected area */
#define VPL_NF_ERR_ECC_CODE_WRONG           0x0Au /** error - read data are corrupted */
#define VPL_NF_ERR_CURR_PAGE_OP_ERR         0x0Eu
#define VPL_NF_ERR_PRIOR_CURR_PAGE_OP_FAIL  0x10u /** erase/program operation of current and prior page/block failed */

#define VPL_BRC_BASE       0x60000000
#define VPL_BRC_SRAM_BASE  (VPL_BRC_BASE + 0x01000000)
#define VPL_BRC_VERSION    (VPL_BRC_BASE + 0x01800000)
#define VPL_BRC_CTRL       (VPL_BRC_BASE + 0x01800008)

extern unsigned long nand_erase_opts_max_block_num ;//this is the final block that u-boot can erase

#endif
