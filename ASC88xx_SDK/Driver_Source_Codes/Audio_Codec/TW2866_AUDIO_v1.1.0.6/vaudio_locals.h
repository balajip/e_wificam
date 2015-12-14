/*
 * Copyright (C) 2007  ______ Inc.
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
 */

/* ======================================================================== */

#ifndef __VAUDIO_LOCALS_H__
#define __VAUDIO_LOCALS_H__

/* ============================================================================
*/
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif
#define NEW_KERN

/* ============================================================================
   System-related header files.
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#ifdef NEW_KERN
#include <mach/irq.h>
#include <mach/platform.h>
#else
#include <asm/arch/irq.h>
#include <asm/arch/platform.h>
#endif

/* ============================================================================
   User-releated files.
*/
//#define VAUDIO_DEBUG

#include "vaudio.h"
#include "typedef.h"

/* ============================================================================
*/
#include <asm/io.h>

#define PERIOD_BYTES_MAX	16380

// SSP
#define SSP0_BASE			MOZART_I2S_0_MMR_BASE
#define SSP1_BASE			MOZART_I2S_1_MMR_BASE
#define SSP2_BASE			MOZART_I2S_2_MMR_BASE
#define SSP3_BASE			MOZART_I2S_3_MMR_BASE
#define SSP4_BASE			MOZART_I2S_4_MMR_BASE

#define I2S_IER				0x00000000
#define I2S_IRER				0x00000004
#define I2S_ITER				0x00000008
#define I2S_CER				0x0000000c
#define I2S_CCR				0x00000010
#define I2S_RXFFR			0x00000014
#define I2S_TXFFR			0x00000018
#define I2S_RER				0x00000028
#define I2S_TER				0x0000002C
#define I2S_RCR				0x00000030
#define I2S_TCR				0x00000034
#define I2S_ISR				0x00000038
#define I2S_IMR				0x0000003C
#define I2S_ROR				0x00000040
#define I2S_TOR				0x00000044
#define I2S_RFCR				0x00000048
#define I2S_TFCR				0x0000004C
#define I2S_RFF0				0x00000050
#define I2S_TFF0				0x00000054
#define I2S_RXDMA			0x000001C0
#define I2S_RRXDMA			0x000001C4
#define I2S_TXDMA			0x000001C8
#define I2S_RTXDMA			0x000001CC
#define I2S_COMP_PARAM_2	0x000001F0
#define I2S_COMP_PARAM_1	0x000001F4
#define I2S_COMP_VERSION	0x000001F8
#define I2S_COMP_TYPE		0x000001FC


// APBC DMA
#define APBC_BASE			MOZART_APBC_MMR_BASE
#define V_APBC_BASE     		IO_ADDRESS(APBC_BASE)
#define APBC_VERSION		(APBC_BASE+0x80)
#define APBC_DMA_BASE		(APBC_BASE+0x88)

enum {
	DMA_CHN_0 = 0,
	DMA_CHN_1,
	DMA_CHN_2,
	DMA_CHN_3,
	DMA_CHN_4,
	DMA_CHN_5,
	DMA_CHN_6,
	DMA_CHN_7,
	DMA_CHN_8,
	DMA_CHN_9,
	DMA_CHN_10,
	DMA_CHN_11,
	DMA_CHN_12,
	DMA_CHN_13,
	DMA_CHN_14,
	DMA_CHN_15,
};

#define APBC_DMA_SRC_ADDR(chn)	(0x00000090+chn*0x10)
#define APBC_DMA_DES_ADDR(chn)	(0x00000094+chn*0x10)
#define APBC_DMA_LLP(chn)			(0x00000098+chn*0x10)
#define APBC_DMA_CTRL(chn)			(0x0000009C+chn*0x10)

#define APBC_DMA_CHN_MONITOR		0x8c

#define APBC_IRQ              0

// SYSC
#define SYSC_BASE               			MOZART_SYSC_MMR_BASE
#define SYSC_ID_VERSION       			0x00000000
#if 1 //new layout
#define SYSC_I2S_CLK_PARAM 		0x0000002C
#define SYSC_I2S_CLK_ENABLE		0x00000028
#define SYSC_I2S_ENABLE				0x00000800
#else
#define SYSC_I2S_CLK_PARAM 		0x00000028
#define SYSC_I2S_CLK_ENABLE		0x00000034
#define SYSC_I2S_ENABLE				0x00000001
#endif

#define SYSC_I2S_CLK_PARAM_DIVIDER_MASK	0x00FF0000

/* ======================================================================== */
#define MOZART_VPL_SSP_READL(base, reg)		__raw_readl((base) + (reg))
#define MOZART_VPL_SSP_WRITEL(base, reg, val)	__raw_writel((val), (base) + (reg))

#define MOZART_APBC_READL(reg)				__raw_readl(V_APBC_BASE + (reg))
#define MOZART_APBC_WRITEL(reg,val)			__raw_writel((val), V_APBC_BASE + (reg))

/* ======================================================================== */

struct dma_regs_t{
	DWORD dwSrc_Addr;
	DWORD dwDes_Addr;
	DWORD dwLLP_Addr;
	DWORD dwCtrl;
};

enum {
	MOZART_I2S1 = 0,
	MOZART_I2S2 = 1,
	MOZART_I2S0 = 2,
#if 0 // modify for single chan	
	MOZART_I2S2 = 2,
	MOZART_I2S3 = 3,
	MOZART_I2S4 = 4,
#endif
	MOZART_I2S_NUM,
};

/* ======================================================================== */
#endif //__VAUDIO_LOCALS_H__
