#ifndef __MOZART_H__
#define __MOZART_H__


#include "maps.h"


/********************************************* 
 *           Include device headers 
 *********************************************
 */
#include "uart.h"
#include "timer.h"
#include "nand.h"
#include "gmac.h"
#include "serialflash.h"
#include "sysctrl.h"
#include "wdt.h"
#include "mshc.h"
#include "interrupt.h"
#include "i2s.h"
#include "dram.h"
#include "quicktest.h"
#include "autoburn.h"
#include "usb.h"
#include "gpio.h"
#include "apbcdma.h"
#include "dce.h"

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

//#define CONFIG_SYS_USE_NANDFLASH 1

#endif
