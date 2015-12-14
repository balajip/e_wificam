#ifndef __HAYDN_H__
#define __HAYDN_H__

/********************************************* 
 *           Include device headers 
 *********************************************
 */
#include "Haydn_uart.h"
#include "Haydn_timer.h"
#include "Haydn_gmac.h"
#include "Haydn_serialflash.h"
#include "Haydn_sysctrl.h"
#include "Haydn_quicktest.h"
#include "Haydn_interrupt.h"
#include "Haydn_wdt.h"

/************************************************************************************ 
 *  This is a number of machine type for kernel, but we have not register yet.
 ************************************************************************************
 */ 
#define MACH_TYPE_EVM				500	
#define BOOT_PARAMS_ADDR			0x100	
#define SYSTEM_CLOCK                150000000
#define SYS_CLK                     SYSTEM_CLOCK
#define AHB_CLK                     SYS_CLK
#define APB_CLK                     (SYS_CLK / 2)

/********************************************* 
 *           Device MMR-Base address
 *********************************************
 */


/********************************************************* 
 *           Useful macros and global variables 
 ********************************************************* 
 */
typedef unsigned long UINT32 ;

#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
#define v_outw(addr,value)	(*((volatile unsigned short *)(addr)) = value)
#define v_outb(addr,value)	(*((volatile unsigned char *)(addr)) = value)
#define v_inl(addr)		(*((volatile unsigned long *)(addr)))
#define v_inw(addr)		(*((volatile unsigned short *)(addr)))
#define v_inb(addr)		(*((volatile unsigned char *)(addr)))

extern unsigned long _bss_end ;
extern unsigned long _armboot_real_end ;


#endif
