#ifndef __BEETHOVEN_SYSCTRL_H__
#define __BEETHOVEN_SYSCTRL_H__

extern int (*env_init)(void) ;
extern int (*saveenv)(void) ;
extern unsigned char (*env_get_char_spec)(int index) ;
extern void (*env_relocate_spec)(void) ;

#define EVM_SYSC_BASE 0x7f800000

#define SYSCTRL_DATA_IN_SERIALFLASH   0x0
#define SYSCTRL_DATA_IN_SD            0x1
#define SYSCTRL_DATA_IN_NANDFLASH     0x2
#define SYSCTRL_MASK_SHIFT            21
#define SYSCTRL_MASK                  (1 << SYSCTRL_MASK_SHIFT)

#define SYSCTRL_SYSC_INFO (EVM_SYSC_BASE + 0xC4)

#define SYSC_MMR_GMAC_CLK_CFG_CTRL    0x30
#define SYSC_MMR_PAD_VOLTAGE_LEVEL    0x70

#define EVM_SYSC_UARTC_CLK_CFG_CTRL   0x3C
#define EVM_SYSC_PULL_CTRL            0x78


#endif
