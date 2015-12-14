#ifndef __ASM_ARCH_ROSSINI_INTERRUPT_H__
#define __ASM_ARCH_ROSSINI_INTERRUPT_H__

/* ----------------------------------------------------------------------------
 *  Interrupt Controllers
 * ----------------------------------------------------------------------------
 */
#define IRQ_SOURCE_LO_REG                   0x04
#define IRQ_SOURCE_HI_REG                   0x08
#define IRQ_STATUS_LO_REG                   0x0c
#define IRQ_STATUS_HI_REG                   0x10
#define IRQ_MASK_LO_REG                     0x14
#define IRQ_MASK_HI_REG                     0x18
#define IRQ_CLEAR_LO_REG                    0x1c
#define IRQ_CLEAR_HI_REG                    0x20
#define IRQ_MODE_LO_REG                     0x24
#define IRQ_MODE_HI_REG                     0x28
#define IRQ_SET_LO_REG                      0x2c    /* added in Rossini, for MSI */
#define IRQ_SET_HI_REG                      0x30    /* added in Rossini, for MSI */

#define FIQ_SOURCE_REG                      0x20
#define FIQ_MASK_REG                        0x24
#define FIQ_CLEAR_REG                       0x28
#define FIQ_MODE_REG                        0x2c
#define FIQ_LEVEL_REG                       0x30
#define FIQ_STATUS_REG                      0x34


/*  IRQ numbers definition  */

#define TMRC_TM0_IRQ_NUM    0
#define APBC_IRQ_NUM        1
#define VIC_IRQ_NUM         2
#define VOC_IRQ_NUM         3
#define DMAC_IRQ_NUM        4
#define GMAC_IRQ_NUM        5
#define MSHC0_IRQ_NUM       6
#define MSHC1_IRQ_NUM       7
#define USBC_IRQ_NUM        8
#define PCIEC_IRQ_NUM       9
#define NFC_IRQ_NUM         10
#define DCE_IRQ_NUM         11
#define H4EE_IRQ_NUM        12
#define JEBE_IRQ_NUM        13
#define IBPE_IRQ_NUM        14
#define DIE_IRQ_NUM         15
#define IRE_IRQ_NUM         16
#define MEAE_IRQ_NUM        17
#define SSI_IRQ_NUM         18
#define I2SC0_IRQ_NUM       19
#define I2SC1_IRQ_NUM       20
#define I2SC2_IRQ_NUM       21
#define I2SC3_IRQ_NUM       22
#define I2SC4_IRQ_NUM       23
#define WDTC_IRQ_NUM        24
#define UARTC0_IRQ_NUM      25
#define UARTC1_IRQ_NUM      26
#define UARTC2_IRQ_NUM      27
#define UARTC3_IRQ_NUM      28
#define AGPOC_IRQ_NUM       29
#define GPIOC_IRQ_NUM       30
#define IRDAC_IRQ_NUM       31
#define TMRC_TM1_IRQ_NUM    32
#define TMRC_TM2_IRQ_NUM    33
#define TMRC_TM3_IRQ_NUM    34
#define PCIEC_ERR_IRQ_NUM   35
#define VIC0_VSYNC_IRQ_NUM  36
#define VIC1_VSYNC_IRQ_NUM  37

/* Add by Sor */
#define VPL_AGPOC_IRQ_NUM       AGPOC_IRQ_NUM
#define VMA_IRE_IRQ_NUM         IRE_IRQ_NUM
#define VMA_JEBE_IRQ_NUM        JEBE_IRQ_NUM
#define VMA_IBPE_IRQ_NUM        IBPE_IRQ_NUM
#define VPL_APBC_IRQ_NUM        APBC_IRQ_NUM
#define VPL_UARTC_0_IRQ_NUM     UARTC0_IRQ_NUM
#define VPL_UARTC_1_IRQ_NUM     UARTC1_IRQ_NUM
#define VPL_UARTC_2_IRQ_NUM     UARTC2_IRQ_NUM
#define VPL_UARTC_3_IRQ_NUM     UARTC3_IRQ_NUM
#define VPL_TMRC_TM0_IRQ_NUM    TMRC_TM0_IRQ_NUM
#define VPL_TMRC_TM1_IRQ_NUM    TMRC_TM1_IRQ_NUM
#define VPL_TMRC_TM2_IRQ_NUM    TMRC_TM2_IRQ_NUM
#define VPL_TMRC_TM3_IRQ_NUM    TMRC_TM3_IRQ_NUM
#define VPL_GPIOC_IRQ_NUM       GPIOC_IRQ_NUM
#define VPL_DMAC_IRQ_NUM        DMAC_IRQ_NUM
#define VMA_DIE_IRQ_NUM         DIE_IRQ_NUM
#define VMA_MEAE_IRQ_NUM        MEAE_IRQ_NUM
#define VMA_H4EE_IRQ_NUM        H4EE_IRQ_NUM
#define VPL_VIC_IRQ_NUM         VIC_IRQ_NUM
#define VPL_VOC_IRQ_NUM         VOC_IRQ_NUM
#define VMA_DCE_IRQ_NUM         DCE_IRQ_NUM
#define VPL_IRDAC_IRQ_NUM       IRDAC_IRQ_NUM
/* ========== */
#define VPL_VIC0_VSYNC_IRQ_NUM  VIC0_VSYNC_IRQ_NUM
#define VPL_VIC1_VSYNC_IRQ_NUM  VIC1_VSYNC_IRQ_NUM

/*  Interrupt bit positions  */
#define MAXIRQNUM                       63
#define MAXFIQNUM                       31
#define ROSSINI_NR_IRQS                 (MAXIRQNUM + 1)
#define ROSSINI_NR_FIQS                 (MAXFIQNUM + 1)

#define LEVEL                           0
#define EDGE                            1

#define H_ACTIVE                        0
#define L_ACTIVE                        1


#endif  /* __ASM_ARCH_ROSSINI_INTERRUPT_H__ */
