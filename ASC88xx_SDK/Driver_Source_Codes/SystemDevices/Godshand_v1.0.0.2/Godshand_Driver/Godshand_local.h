/*
 * $Header: $
 *
 * Copyright 2000-2006 VN , Inc.  All rights reserved.
 *
 *         
 * $History: $
 * 
 */
/* ======================================================================== */
#ifndef __GODSHAND_LOCAL_H__
#define __GODSHAND_LOCAL_H__

#ifndef __KERNEL__
#	define __KERNEL__
#endif //__KERNEL__

#ifndef MODULE
#	define MODULE
#endif //MODULE

#define _DEBUG
/* ======================================================================== */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>
#include <linux/fs.h>		/* everything... */
#include <linux/ioctl.h>	/* needed for the _IOW etc stuff used later */
#include <linux/ioport.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>			/* ioremap */
#ifdef _MOZART
#include <asm/irq.h>	/* IRQ_EXPINT0 */
#else
#include <asm/irq.h>	/* IRQ_EXPINT0 */
//#include <asm/arch/irqs.h>	/* IRQ_EXPINT0 */
#endif
#include <mach/platform.h>

#include "Godshand.h"
#include <linux/moduleparam.h>
#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define PLATFORM	"Mozart"
#endif
#ifdef __ASM_ARCH_PLATFORM_BACH_H__
#define PLATFORM	"Bach"
#endif
#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
#define PLATFORM	"Haydn"
#endif

#ifndef PLATFORM
#define PLATFORM "Un-know"
#endif
#define DRIVER_VERSION "1.0.0.2"
#define COMPANY "VN Inc."
/* ======================================================================== */

typedef struct 
{
	int number;
	struct semaphore sem;
}GODSHAND_Dev;

#define GODSHANDCD_MAJOR_DEFAULT 0
#define GODSHAND_START 0x99c00000
#define GODSHAND_SIZE 0x04
/* ======================================================================== */

#endif //__GODSHAND_LOCAL_H__
