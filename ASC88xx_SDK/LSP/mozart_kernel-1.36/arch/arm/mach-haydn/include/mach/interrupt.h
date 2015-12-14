#ifndef __ASM_ARCH_HAYDN_INTERRUPT_H__
#define __ASM_ARCH_HAYDN_INTERRUPT_H__

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

#define FIQ_SOURCE_REG                      0x20
#define FIQ_MASK_REG                        0x24
#define FIQ_CLEAR_REG                       0x28
#define FIQ_MODE_REG                        0x2c
#define FIQ_LEVEL_REG                       0x30
#define FIQ_STATUS_REG                      0x34


/*  IRQ numbers definition  */

#define APBC_IRQ_NUM        0
//#define PCI_IRQ_NUM         1
//#define RTCC_IRQ_NUM        2
#define UARTC0_IRQ_NUM      44
#define UARTC1_IRQ_NUM      45
#define UARTC2_IRQ_NUM      46
#define UARTC3_IRQ_NUM      47
#define I2SC0_IRQ_NUM       33
#define I2SC1_IRQ_NUM       6 
#define TMRC_TM0_IRQ_NUM    36
#define TMRC_TM1_IRQ_NUM    37
#define TMRC_TM2_IRQ_NUM    38
#define WDTC_IRQ_NUM        34
#define GPIOC_IRQ_NUM       35
#define I2CC_IRQ_NUM        12
#define	PS2IC_IRQ_NUM       13
#define USBC_IRQ_NUM        1
#define MAC_IRQ_NUM         2
/* Add by Sor */
#define F2BE_IRQ_NUM        15
#define B2FE_IRQ_NUM        18
/* ========== */                
#define IDEC_IRQ_NUM        19
#define DMAC_IRQ_NUM        9
#define DIE_IRQ_NUM         10
#define MEAE_IRQ_NUM        4
#define JEBE_IRQ_NUM        5
#define MEBE_IRQ_NUM        7
#define IBPE_IRQ_NUM        11
#define MDE_IRQ_NUM         12
#define MME_IRQ_NUM         13
#define IRE_IRQ_NUM			16
#define DCE_IRQ_NUM			17
#define VIC_IRQ_NUM         14
#define VOC_IRQ_NUM         26
#define IPE_IRQ_NUM         27
#define MDBE_IRQ_NUM        28
#define AHBC0_IRQ_NUM       29
#define AHBC1_IRQ_NUM       30
#define AHBC2_IRQ_NUM       31
#define SSI_IRQ_NUM			32

/* Add by Sor */
#define VMA_IRE_IRQ_NUM			IRE_IRQ_NUM	
#define VMA_JEBE_IRQ_NUM 		JEBE_IRQ_NUM 
#define VMA_IBPE_IRQ_NUM 		IBPE_IRQ_NUM        
#define VMA_MDE_IRQ_NUM         MDE_IRQ_NUM 
#define VPL_APBC_IRQ_NUM        APBC_IRQ_NUM    
#define VPL_RTCC_IRQ_NUM        RTCC_IRQ_NUM    
#define VPL_UARTC_0_IRQ_NUM     UARTC0_IRQ_NUM  
#define VPL_UARTC_1_IRQ_NUM     UARTC1_IRQ_NUM  
#define VPL_TMRC_TM0_IRQ_NUM    TMRC_TM0_IRQ_NUM
#define VPL_TMRC_TM1_IRQ_NUM    TMRC_TM1_IRQ_NUM
#define VPL_TMRC_TM2_IRQ_NUM    TMRC_TM2_IRQ_NUM
#define VPL_GPIOC_IRQ_NUM       GPIOC_IRQ_NUM   
#define	VPL_PS2IC_IRQ_NUM       PS2IC_IRQ_NUM   
#define VMA_B2FE_IRQ_NUM        B2FE_IRQ_NUM    
#define VMA_F2BE_IRQ_NUM        F2BE_IRQ_NUM    
#define VPL_IDEC_IRQ_NUM        IDEC_IRQ_NUM    
#define VPL_DMAC_IRQ_NUM        DMAC_IRQ_NUM    
#define VMA_DIE_IRQ_NUM         DIE_IRQ_NUM     
#define VMA_MEAE_IRQ_NUM        MEAE_IRQ_NUM    
#define VMA_MEBE_IRQ_NUM        MEBE_IRQ_NUM    
#define VMA_MME_IRQ_NUM         MME_IRQ_NUM     
#define VPL_VIC_IRQ_NUM         VIC_IRQ_NUM     
#define VPL_VOC_IRQ_NUM         VOC_IRQ_NUM     
#define VMA_IPE_IRQ_NUM         IPE_IRQ_NUM     
#define VMA_MDBE_IRQ_NUM        MDBE_IRQ_NUM    
#define VPL_AHBC_0_IRQ_NUM      AHBC0_IRQ_NUM   
#define VPL_AHBC_1_IRQ_NUM      AHBC1_IRQ_NUM   
#define VPL_AHBC_2_IRQ_NUM      AHBC2_IRQ_NUM
#define VMA_DCE_IRQ_NUM			DCE_IRQ_NUM
/* ========== */

/*  Interrupt bit positions  */
#define MAXIRQNUM                       63 
#define MAXFIQNUM                       31  
#define HAYDN_NR_IRQS                    (MAXIRQNUM + 1)
#define HAYDN_NR_FIQS                    (MAXFIQNUM + 1)

#define LEVEL                           0
#define EDGE                            1
                         
#define H_ACTIVE                        0
#define L_ACTIVE                        1


// --------------------------------------------------------------------
//		PCI 的 interrupt
// --------------------------------------------------------------------
//  還不知值

//#define IRQ_PCIINT0						0
//#define IRQ_PCIINT1						0
//#define IRQ_PCIINT2						0
//#define IRQ_PCIINT3						0

#endif	// __ASM_ARCH_HANDEL_INTERRUPT_H__
