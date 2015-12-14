#ifndef __HAYDN_SYSCTRL_H__
#define __HAYDN_SYSCTRL_H__

extern int (*env_init)(void) ;
extern int (*saveenv)(void) ;
extern unsigned char (*env_get_char_spec)(int index) ;
extern void (*env_relocate_spec)(void) ;

#define EVM_SYSC_BASE 0x4f800000

#define EVM_SYSC_UARTC0_PARAM 0x40
#define EVM_SYSC_UARTC1_PARAM 0x44
#define EVM_SYSC_UARTC2_PARAM 0x48
#define EVM_SYSC_UARTC3_PARAM 0x4c

#endif

