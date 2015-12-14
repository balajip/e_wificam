/*
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
 */

#ifndef __CAT6011_LOCALS_H__
#define __CAT6011_LOCALS_H__
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


/* ============================================================================
*/
#define CAT6011_TBL_SIZE 11

typedef enum CAT6011_registers
{
	_02_DevIDL						= 0x02,
	_03_DevIDH						= 0x03,
	_05_RstCtrl						= 0x05,
	_08_VIOCtrl						= 0x08,
	_1A_MiscCtrl					= 0x1A,
	_1B_VideoMap					= 0x1B,
	_1C_VideoCtrl1					= 0x1C,
	_1D_VclkCtrl						= 0x1D,
	_20_CSCCtrl						= 0x20,
	_3C_PGCtrl1						= 0x3C,
	_3D_PGCtrl2						= 0x3D,
	_3E_PGColR						= 0x3E,
	_3F_PGColG						= 0x3F,
	_40_PGColB						= 0x40
} ECAT6011Regs;


typedef SCODE (* FOnReadReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAdd, DWORD *pdwData);
typedef SCODE (* FOnWriteReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwData);


typedef struct CAT6011_reg_addr_data
{
    ECAT6011Regs eRegAddr;
    DWORD dwData;
} TCAT6011RegAddrData;

typedef struct CAT6011_info
{
	spinlock_t lock;

	BOOL bInit;
	
	DWORD dwDeviceAddr;
	DWORD dwFuncUserData;

	FOnWriteReg_t pfnWriteBuf;	
}TCAT6011Info;

