#ifndef __MOZART_NAND_H__
#define __MOZART_NAND_H__

/*
 ********************************************************
 *                 Mozart NFC MMRs
 ********************************************************
 */
#define MOZART_NFC_MMR_BASE_CPUMODE IO_ADDRESS(VPL_NFC_MMR_BASE)
#define MOZART_NFC_MMR_STARTADDR   ( MOZART_NFC_MMR_BASE_CPUMODE + 0x1300 )
#define MOZART_NFC_MMR_CONF        ( 0x00 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_CTRL        ( 0x04 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_COMM        ( 0x08 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_ADDR0L      ( 0x0c + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_ADDR1L      ( 0x10 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_DATA        ( 0x14 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_PB0         ( 0x18 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_STATE       ( 0x1c + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp0    ( 0x20 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp1    ( 0x24 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp2    ( 0x28 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp3    ( 0x2c + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp4    ( 0x30 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp5    ( 0x34 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp6    ( 0x38 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_FLookUp7    ( 0x3c + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_ECCState    ( 0x40 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_ADDR0H      ( 0x44 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_ADDR1H      ( 0x48 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_LookUpEN    ( 0x4c + MOZART_NFC_MMR_STARTADDR )

//--NANDDMA MMR--
#define MOZART_NFC_MMR_DMAADDR    ( 0x80 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_DMACTRL    ( 0x84 + MOZART_NFC_MMR_STARTADDR )
#define MOZART_NFC_MMR_DMACNTR    ( 0x88 + MOZART_NFC_MMR_STARTADDR )

//--NAND DMA SETTING
#define MOZART_NFC_ADDR_MASK        0x0fffffff
#define MOZART_NFC_DMA_ENABLE       0x01000000
#define MOZART_NFC_DMA_DIR_BUF2AHB  0x10000
#define MOZART_NFC_DMA_DIR_AHB2BUF  0x0
#define MOZART_NFC_DMA_SIZE         ( 0 << 14 )
#define MOZART_NFC_DMA_BURST        ( 2 << 11 )

//--NAND Useful Flag
#define MOZART_NF_TRANSFER_READ  0x01
#define MOZART_NF_TRANSFER_WRITE 0x00

//--NAND Errors
#define MOZART_NF_ERR_NO_ERRORS                0x00u
#define MOZART_NF_ERR_INVALID_PARAMETER        0x01u /** error - invalid parameter */
#define MOZART_NF_ERR_INVALID_ADDRESS          0x03u /** error - invalid address */
#define MOZART_NF_ERR_DMA_TRANSFER             0x04u /** DMA transmission error */
#define MOZART_NF_ERR_MEMORY_BUSY              0x07u /** error - memory doesn't response on request */
#define MOZART_NF_ERR_PROTECTED                0x08u /** error - try Write or Erase protected area */
#define MOZART_NF_ERR_ECC_CODE_WRONG           0x0Au /** error - read data are corrupted */
#define MOZART_NF_ERR_CURR_PAGE_OP_ERR         0x0Eu
#define MOZART_NF_ERR_PRIOR_CURR_PAGE_OP_FAIL  0x10u /** erase/program operation of current and prior page/block failed */

#define MOZART_BRC_SRAM_BASE  IO_ADDRESS(MOZART_SRAM_MMR_BASE)
#define MOZART_BRC_CTRL       IO_ADDRESS(MOZART_SRAM_CTRL_MMR_BASE)

//[jam patch]
//#define MOZART_KMALLOC_MAX_SIZE KMALLOC_MAX_SIZE
#define MOZART_KMALLOC_MAX_SIZE 131072

#endif
