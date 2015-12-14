#ifndef __MOZART_MAPS_LEGACY_H__
#define __MOZART_MAPS_LEGACY_H__


/* AHB slaves */
#define VPL_BRC_ROM_BASE              0x00000000
#define VPL_BRC_RAM_BASE              (VPL_BRC_ROM_BASE + 0x00001000)
#define VPL_BRC_MMR_BASE              (VPL_BRC_ROM_BASE + 0x00004000)
#define VPL_BRC_ROM_BASE_REMAPPED     0x30000000
#define VPL_BRC_RAM_BASE_REMAPPED     (VPL_BRC_ROM_BASE_REMAPPED + 0x00001000)
#define VPL_BRC_MMR_BASE_REMAPPED     (VPL_BRC_ROM_BASE_REMAPPED + 0x00004000)
#define VPL_DDRSDMC_1_MEM_BASE        0x10000000
                                   /* 0x20000000 */
#define VPL_DDRSDMC_0_MEM_BASE        0x30000000
#define VPL_APBC_DEV_MMR_BASE         0x40000000
#define VPL_APB3C_DEV_MMR_BASE        0x50000000
#define VPL_PCIEC_MEM_BASE            0x60000000
                                   /* 0x70000000 */
#define VPL_USBC_MMR_BASE             0x80000000
#define VPL_USBC_RAM_BASE             0x80020000
#define VPL_PCIEC_MMR_BASE            0x84000000
#define VPL_MSHC_0_MMR_BASE           0x90000000
#define VPL_MSHC_1_MMR_BASE           0x94000000
#define VPL_GMAC_MMR_BASE             0x98000000
#define VPL_NFC_MMR_BASE              0xA0000000
                                   /* 0xB0000000 */
#define VPL_AHBC_0_MMR_BASE           0xC0000000
#define VPL_AHBC_1_MMR_BASE           0xC1000000
#define VPL_AHBC_2_MMR_BASE           0xC2000000
#define VPL_DDRSDMC_0_MMR_BASE        0xC4000000
#define VPL_DDRSDMC_1_MMR_BASE        0xC5000000
#define VPL_INTC_MMR_BASE             0xC7000000
#define VPL_APBC_MMR_BASE             0xC8000000
#define VPL_DMAC_MMR_BASE             0xC9000000
#define VPL_VIC_MMR_BASE              0xCA000000
#define VPL_VOC_MMR_BASE              0xCC000000
#define VMA_DCE_MMR_BASE              0xD0000000
#define VMA_DIE_MMR_BASE              0xD1000000
#define VMA_IBPE_MMR_BASE             0xD2000000
#define VMA_IRE_MMR_BASE              0xD3000000
#define VMA_MEAE_MMR_BASE             0xD7000000
#define VMA_JEBE_MMR_BASE             0xD8000000
#define VMA_H4EE_MMR_BASE             0xDC000000
                                   /* 0xE0000000 */
#define VPL_DUMMY_BASE                0xF0000000

/* APB slaves */
                                   /* 0x40000000 */
#define VPL_SSI_MMR_BASE              0x40800000
                                   /* 0x41000000 */
#define VPL_I2S_0_MMR_BASE            0x41800000
#define VPL_I2S_1_MMR_BASE            0x42000000
#define VPL_I2S_2_MMR_BASE            0x42800000
#define VPL_I2S_3_MMR_BASE            0x43000000
#define VPL_I2S_4_MMR_BASE            0x43800000
                                   /* 0x44000000 */
                                   /* 0x45000000 */
                                   /* 0x46000000 */
#define VPL_USB_PHY_BASE              0x47000000
#define VPL_PCIEC_SSC_BASE            0x47800000
#define VPL_TMRC_MMR_BASE             0x48000000
#define VPL_WDTC_MMR_BASE             0x48800000
#define VPL_GPIOC_MMR_BASE            0x49000000
#define VPL_AGPOC_MMR_BASE            0x49800000
#define VPL_UARTC0_MMR_BASE           0x4A000000
#define VPL_UARTC1_MMR_BASE           0x4A800000
#define VPL_UARTC2_MMR_BASE           0x4B000000
#define VPL_UARTC3_MMR_BASE           0x4B800000
#define VPL_IRDAC_MMR_BASE            0x4C000000
                                   /* 0x4D000000 */
                                   /* 0x4E000000 */
#define VPL_PLLC_MMR_BASE             0x4F000000
#define VPL_SYSC_MMR_BASE             0x4F800000

/* APB 3 slaves */
#define VPL_DDR32PHY_PUB_BASE         0x50000000


#endif
