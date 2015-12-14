/*
 * $Header: /rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C/gpio_i2c_locals.h 24    12/09/25 3:13p Vincent $
 *
 * Copyright (C) 2010  VN Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * $History: gpio_i2c_locals.h $
 * 
 * *****************  Version 24  *****************
 * User: Vincent      Date: 12/09/25   Time: 3:13p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 23  *****************
 * User: Evelyn       Date: 12/01/05   Time: 3:04p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 20  *****************
 * User: Alan         Date: 09/12/09   Time: 4:23p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Modify GPIOI2C_SCL_0 and GPIOI2C_SDA for Mozart RDK and
 * EVB ver. b - DONE.
 *
 * *****************  Version 19  *****************
 * User: Alan         Date: 09/12/04   Time: 2:15a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Replace __GPIO_I2C_IN_ISR__ with GPIOI2C_INTR. If
 * GPIOI2C_INTR is defined, SPI will be done in ISR - DONE.
 *
 * *****************  Version 18  *****************
 * User: Alan         Date: 09/11/22   Time: 2:08p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 17  *****************
 * User: Alan         Date: 09/11/21   Time: 8:02p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Modify GPIOI2C_SDA to GPIOI2C_SDA_0 and add GPIOI2C_SCL_1
 * and GPIOI2C_SDA_1 for other I2C devices - DONE.
 *
 * *****************  Version 16  *****************
 * User: Alan         Date: 09/11/15   Time: 12:10p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 15  *****************
 * User: Alan         Date: 09/11/14   Time: 11:44p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 14  *****************
 * User: Alan         Date: 09/09/25   Time: 7:08p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 13  *****************
 * User: Alan         Date: 09/08/19   Time: 10:15p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * OPTIMIZATION: To further shorten the config time for the fast (SPI)
 * mode, move all GPIO MMR progrmming into ISR - DONE.
 *
 * *****************  Version 12  *****************
 * User: Alan         Date: 09/08/18   Time: 11:57a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Change GPIOI2C_SEN and GPIOI2C_INTR to GPIO 14 and GPIO 5
 * for Haydn RDK - DONE.
 *
 * *****************  Version 11  *****************
 * User: Ycchang      Date: 09/05/11   Time: 5:14p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 10  *****************
 * User: Albert.shen  Date: 09/05/05   Time: 10:17p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 9  *****************
 * User: Albert.shen  Date: 09/04/13   Time: 8:55p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 8  *****************
 * User: Aniki        Date: 09/04/01   Time: 1:54p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 7  *****************
 * User: Evelyn       Date: 09/03/13   Time: 4:37p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 5  *****************
 * User: Ycchang      Date: 08/03/20   Time: 10:55a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 4  *****************
 * User: Alan         Date: 07/12/31   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 3  *****************
 * User: Ycchang      Date: 07/11/23   Time: 2:33p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 2  *****************
 * User: Alan         Date: 06/05/15   Time: 9:42a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * FEATURE: Support 4 I2C buses with shared data bit - DONE.
 * FEATURE: Add an object information structure to handle multiple open -
 * DONE.
 *
 * *****************  Version 1  *****************
 * User: Chikuang     Date: 06/01/23   Time: 11:37a
 * Created in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 5  *****************
 * User: Aren         Date: 04/09/09   Time: 1:58p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 * MODIFICATION: modified for vivaldi with kernel 2.4.26
 *
 * *****************  Version 4  *****************
 * User: Steve        Date: 04/07/11   Time: 9:18p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 3  *****************
 * User: Vicky        Date: 04/07/06   Time: 10:01a
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 2  *****************
 * User: Sor          Date: 04/06/24   Time: 12:33p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 1  *****************
 * User: Vicky        Date: 04/06/12   Time: 10:45a
 * Created in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 */

/* ============================================================================================= */
#ifndef __GPIOI2C_LOCALS_H__
#define __GPIOI2C_LOCALS_H__

/* ============================================================================================= */
/* Version 3.0.0.0 modification, 2007.09.05 */
#include <linux/interrupt.h>
#include <linux/mm.h>
/* ======================================== */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
/* Version 3.1.0.0 modification, 2009.11.20 */
#include <linux/fs.h>
/* ======================================== */
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
/* Version 3.0.0.0 modification, 2007.09.05 */
#include <asm/cache.h>
/* ======================================== */
#include <asm/arch/platform.h>
#include <asm/arch/irq.h>
#include <asm/arch/gpio.h>

#include "gpio_i2c.h"

/* ============================================================================================= */
#ifdef _DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

/* ============================================================================================= */
/* Version 3.0.0.0 modification, 2007.11.23 */
/* Version 3.0.0.1 modification, 2007.12.28 */
#ifdef __ASM_ARCH_PLATFORM_BACH_H__
	#define GPIOI2C_SCL_0	0x00001000	// GPIO 12
	#define GPIOI2C_SCL_1	0x00002000	// GPIO 13
	#define GPIOI2C_SCL_2	0x00004000	// GPIO 14
	#define GPIOI2C_SCL_3	0x00008000	// GPIO 15
	#define GPIOI2C_SDA		0x00010000	// GPIO 16
/* Version 3.0.0.3 modification, 2008.03.20 */
#elif defined(__ASM_ARCH_PLATFORM_HAYDN_H__)
/* ======================================== */
	#define GPIOI2C_SCL_0	0x00001000	// GPIO 12
/* Version 3.0.0.6 modification, 2009.03.12 */
	#define GPIOI2C_SCL_1	0x00000100	// GPIO 8
/* ======================================== */
/* Version 3.0.0.7 modification, 2009.03.19 */
	#define GPIOI2C_SCL_2	0x00000040	// GPIO 6
/* ======================================== */
/* Version 3.0.0.5 modification, 2008.12.10 */
/* ======================================== */
/* Version 3.0.0.6 modification, 2009.03.12 */
	#define GPIOI2C_SDA_0	0x00002000	// GPIO 13
	#define GPIOI2C_SDA_1	0x00000800	// GPIO 11
/* ======================================== */
/* Version 3.0.0.9 modification, 2009.04.13 */
/* Version 3.0.0.7 modification, 2009.03.19 */
	//#define GPIOI2C_SDA_2	0x0000400	// GPIO 10
/* ======================================== */
/* Version 3.0.0.16 modification, 2009.08.19 */
	#define GPIOI2C_SEN		0x00000200	// GPIO 9
	#define GPIOI2C_SO		0x00000400	// GPIO 10, only for EVM
/* ========================================= */
/* ======================================== */
/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.0.0.16 modification, 2009.08.19 */
	#define GPIOI2C_INTR	0x00000020	// GPIO 5
/* ========================================= */
/* ========================================= */
/* Version 3.0.0.5 modification, 2008.12.04 */
#elif 	defined(__ASM_ARCH_PLATFORM_MOZART_H__)
/* Version 3.1.0.0 modification, 2009.11.20 */
/* Version 3.1.0.2 modification, 2009.12.09 */
	#define GPIOI2C_SCL_0	0x00000040	// GPIO 6
	#define GPIOI2C_SDA 	0x00000080	// GPIO 7, common data line for scl withou sda definition
/* ======================================== */
	#define GPIOI2C_SCL_1	0x00001000	// GPIO 12
	#define GPIOI2C_SDA_1	0x00002000	// GPIO 13
/* ======================================== */
/* Version 3.0.0.18 modification, 2009.11.14 */
	#define GPIOI2C_SEN		0x00010000	// GPIO 16
/* Version 3.1.0.0 modification, 2009.11.20 */
	#define GPIOI2C_SO		0x00008000	// SPI SDO pin
/* ======================================== */
/* ========================================= */
/* ======================================== */
/* Version 3.0.0.12 modification, 2009.05.11 */
/* Version 3.0.0.18 modification, 2009.11.14 */
	#define GPIOI2C_INTR	0x00020000	// GPIO 17
/* ========================================= */
/* ========================================= */
#else
	#define GPIOI2C_SCL_0	0x00001000	// GPIO 12
	#define GPIOI2C_SDA		0x00002000	// GPIO 13
#endif //__ASM_ARCH_PLATFORM_BACH_H__
/* ======================================== */
/* ======================================== */

#define DIR_IN	0
#define DIR_OUT 1

// The following timing information is from I2C spec. V2.1
#define T_HD_STA 5 // spec is 4.0 we use 5
#define T_LOW 6 // spec is 4.7 we use 6
#define T_HIGH 5 // spec is 4.0 we use 5
#define T_SU_STA 6 // spec is 4.7 we use 6
#define T_HD_DAT 6 // spec is 5 we use 6
#define T_SU_DAT 1 // spec is 0.25 we use 1
#define T_SU_STO 5 // spec is 4 we use 5
#define T_BUF 6 // spec is 4.7 we use 6

/* ============================================================================================= */
#define GPIOC_MMR_DATAOUT		0x04
#define GPIOC_MMR_DATAIN		0x08
#define GPIOC_MMR_PINDIR		0x0C
#define GPIOC_MMR_DATASET		0x14
#define GPIOC_MMR_DATACLR		0x18
#define GPIOC_MMR_INTREN		0x24
#define GPIOC_MMR_MASKSTATE	    0x2C
#define GPIOC_MMR_INTRMASK	    0x30
#define GPIOC_MMR_INTRCLR		0x34
#define GPIOC_MMR_INTRTRI     	0x38
#define GPIOC_MMR_INTRRISE     	0x40
#define GPIOC_MMR_BOTH     		0x3C

#define GPIOC_NR_DEVS        24
/* ============================================================================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
typedef struct gpioi2c_obj_info
{
	struct inode *pinode;
	volatile BOOL bLock;
} TGPIOI2CObjInfo;
/* ======================================== */

typedef struct
{
/* Version 3.0.0.11 modification, 2009.05.05 */
	wait_queue_head_t tWaitQueueHead;
	volatile int GPIO_intr_occurred;
/* ========================================= */
	struct semaphore sema;
/* Version 3.0.0.16 modification, 2009.08.19 */
	int iSCL;
	int iSDA;
	BOOL bFastMode;
/* Version 3.0.0.18 modification, 2009.11.14 */
	BOOL bIsWrite;
/* ========================================= */
	BYTE *pbyBuff;
	DWORD dwCount;
/* ========================================= */
/* Version 3.1.0.6 modification, 2011.12.01 */
	DWORD dwChipVersion;
/* ========================================= */
} GPIO_I2C_Dev;

/* ============================================================================================= */
#endif	//__GPIOI2C_LOCALS_H__

/* ============================================================================================= */
