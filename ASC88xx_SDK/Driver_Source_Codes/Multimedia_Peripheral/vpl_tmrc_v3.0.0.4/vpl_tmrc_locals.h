/*
 * $Header: /rd_2/project/Mozart/Components/TMRC/Device_Driver/TMRC/vpl_tmrc_locals.h 1     13/02/04 3:39p Yiming.liu $
 *
 * Copyright 2012 VN, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: vpl_tmrc_locals.h $
 * 
 * *****************  Version 1  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 3:39p
 * Created in $/rd_2/project/Mozart/Components/TMRC/Device_Driver/TMRC
 * 
 * *****************  Version 1  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 3:38p
 * Created in $/rd_2/project/Mozart/Components/TMRC/TMRC
 * 
 * *****************  Version 6  *****************
 * User: Alan         Date: 09/11/26   Time: 7:49p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Move MAX_TMRC_DEV_NUM to vpl_tmrc.h - DONE.
 *
 * *****************  Version 5  *****************
 * User: Aniki        Date: 07/08/31   Time: 4:17p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 4  *****************
 * User: Aniki        Date: 06/08/02   Time: 2:57p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * FEATURE: Update TMRC driver as the format B2FE driver - DONE.
 * FEATURE: Support multiple open for Timer 0 - DONE.
 *
 * *****************  Version 3  *****************
 * User: Aniki        Date: 05/11/18   Time: 2:32p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Update the definition of TMRC MMR base address - DONE.
 * MODIFICATION: Update the definition of TMRC IRQ number - DONE.
 *
 * *****************  Version 2  *****************
 * User: Aniki        Date: 05/06/06   Time: 9:30p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 1  *****************
 * User: Aniki        Date: 05/05/11   Time: 9:36p
 * Created in $/rd_2/Training/SoC_Development/Reggie/TMRC/Device_Driver/TMRC
 *
 */

/* ============================================================================================= */
#ifndef __VPL_TMRC_LOCALS_H__
#define __VPL_TMRC_LOCALS_H__

/* ============================================================================================= */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/ioport.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/ioctl.h> /* _IOW etc stuff */
#include <linux/interrupt.h>
#include <linux/delay.h> // mdelay().
/* Version 3.0.0.0 modification, 2007.08.31 */
#include <linux/mm.h>
#include <asm/cache.h>
/* ======================================== */
#include <asm/io.h> /* ioremap */
#include <asm/irq.h>
#include <asm/uaccess.h>
/* Version 1.1.0.1 modification, 2005.11.18 */
#include <asm/arch/platform.h>
/* ======================================== */
#include <asm/arch/irq.h>

#include "typedef.h"
#include "vpl_tmrc.h"

/* ============================================================================================= */
/* Version 2.0.0.0 modification, 2006.07.28 */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "VPL_TMRC: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif
/* ======================================== */

/* ============================================================================================= */
#define READ_TMRC_MMR(a) readl((a)+(DWORD)pvTMRCBase)
#define WRITE_TMRC_MMR(a, b) writel(b, (a)+(DWORD)pvTMRCBase)

/* ============================================================================================= */
typedef struct
{
	volatile DWORD requested;
} TTMRCDevInfo;

/* ============================================================================================= */
#define TMRC_CONTROL_OFFSET	0x00000004
#define TMRC_COUNTER_OFFSET	0x0000000C
#define TMRC_LOAD_OFFSET	0x00000010
#define TMRC_MATCH_OFFSET	0x00000014

/* ============================================================================================= */
/* Version 3.0.0.4 modification, 2009.11.26 */
#define TMRC_COUNT_DIR			0x00000008	/* timer counting direction bit */
#define TMRC_MATCH_EN			0x00000004	/* timer match interrupt enable bit */
#define TMRC_OVERFLOW_EN		0x00000002	/* timer overflow interrupt enable bit */
#define TMRC_EN					0x00000001	/* timer enable bit */
/* ======================================== */
/* Version 1.1.0.1 modification, 2005.11.18 */
/* ======================================== */

/* ============================================================================================= */
#endif //__VPL_TMRC_LOCALS_H__

/* ============================================================================================= */
