#ifndef __MOZART_SYSCTRL_H__
#define __MOZART_SYSCTRL_H__

extern int (*env_init)(void) ;
extern int (*saveenv)(void) ;
extern unsigned char (*env_get_char_spec)(int index) ;
extern void (*env_relocate_spec)(void) ;

#define EVM_SYSC_BASE			VPL_SYSC_MMR_BASE
#define SYSCTRL_SYSC_INFO		(EVM_SYSC_BASE + 0x34)
#define SYSCTRL_PROT_INFO		(EVM_SYSC_BASE + 0x3C)
#define SYSCTRL_PROT_INFO_1		(EVM_SYSC_BASE + 0x40)
#define SYSCTRL_DATA_IN_SERIALFLASH	0x0
#define SYSCTRL_DATA_IN_SD		0x1
#define SYSCTRL_DATA_IN_NANDFLASH	0x2
#define SYSCTRL_MASK_SHIFT		0
#define SYSCTRL_MASK			0x00000002

#define SYSC_MMR_CLKEN_CTRL0		0x24
#define SYSC_MMR_CLKEN_CTRL1		0x28
#define SYSC_MMR_CLK_GEN		0X2C
#define SYSC_MMR_CHIP_VER		0x14
#define SYSC_MMR_MON_CLK_SEL            0x30
#define SYSC_MMR_SYS_INFO		0x34

#define EVM_SYSC_PAD_EN_CTRL		0x44
#define EVM_SYSC_JTAG_IF_SEL		0x48
#define SYSC_MMR_IF_CTRL 		EVM_SYSC_JTAG_IF_SEL
#if defined(CONFIG_ASC88XX_A)
#define     I2SSC_PLL_REF_SEL_SHIFT     (25)
#define     I2SSC_PLL_REF_SEL(x)        ((x) << I2SSC_PLL_REF_SEL_SHIFT)
#endif
#define EVM_SYSC_PULL_CTRL   		0x50
#define SYSC_MMR_DRV_STRENGTH_CTRL_2   0x5c

#define SYSC_CHIP_VERSION		(EVM_SYSC_BASE + SYSC_MMR_CHIP_VER)
#define ASC88XX_M1_VERSION 		0x01000000
#define ASC88XX_M2_VERSION 		0x02000000
#define ASC88XX_A_VERSION 		0x03000000

#endif
