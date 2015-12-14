#ifndef __IMX104_LOCALS_H__
#define __IMX104_LOCALS_H__
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <asm/arch/irq.h>
#include <asm/arch/platform.h>
#include <linux/mutex.h> // spinlock
#include "vivo_codec.h"
#include "video_error.h"

/* ======================================== */
#define IMX104_DEFAULT_OUTPUT_WIDTH	1280
#define IMX104_DEFAULT_OUTPUT_HEIGHT	1024

/* Version 1.0.0.2 modification, 2013.05.20 */
#define IMX104_MAX_GAIN 256000
/* ========================== */

#define IMX104_PIXEL_CLOCK	74250000
#define IMX104_PIXEL_CLOCK_M_x100	(IMX104_PIXEL_CLOCK/10000)
#define IMX104_DELTA_1920x1080_12BIT	640

/* Version 1.0.0.1 modification, 2013.05.18 */
#define IMX104_HSYNC_1280x1024	2248
#define IMX104_VSYNC_1280x1024		1100
#define IMX104_HSYNC_1280x1024_50	2248
/* ========================== */
#define IMX104_VSYNC_1280x1024_50	1320

typedef enum IMX104_registers
{
	_00_Stby       = 0x00,
	_02_Mode       = 0x02,
	_03_HMAX       = 0x03,
	_05_VMAX       = 0x05,
	_08_SHS1       = 0x08,
	_0D_SPL        = 0x0D,  
	_0F_SVS        = 0x0F,  
	_11_FRSEL      = 0x11,
	_12_SSBRK      = 0x12,
	_14_WINPH      = 0x14,
	_16_WINPV      = 0x16,
	_18_WINWH      = 0x18,
	_1A_WINWV      = 0x1A,
	_1E_GAIN       = 0x1E,
	_20_BLKLEVEL   = 0x20,
	_21_XHSLNG     = 0x21,
	_22_XVSLNG     = 0x22,
	_2C_XMSTA      = 0x2C,
	_2D_DCKDLY     = 0x2D,
	_9A_12B1080P   = 0x9A,
	_CE_PRES       = 0xCE,
	_CF_DRES       = 0xCF,
} EIMX104Regs;

typedef struct IMX104_info
{
	spinlock_t lock;
	DWORD bInit;
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;
	FOnWriteBuf pfnWriteBuf;
	DWORD dwFreq;
	DWORD dwCurShutter;
/* Version 1.0.0.1 modification, 2013.05.18 */
	DWORD dwPreExpFrame;	
/* ========================== */
} TIMX104Info;

