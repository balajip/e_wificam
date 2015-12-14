/*
 *
 * Copyright (C) 2010  ______ Inc.
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

#ifndef __IT6604_LOCALS_H__
#define __IT6604_LOCALS_H__
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
#include <linux/workqueue.h>
#include <asm/arch/irq.h>
#include <asm/arch/platform.h>
#include <linux/mutex.h> // spinlock
#include "global_codec.h"
#include "video_error.h"


/* ============================================================================
*/
#define IT6604_TBL_SIZE 8

typedef enum IT6604_registers
{
    _02_DevIDL                      = 0x02,
    _03_DevIDH                      = 0x03,
    _05_RstCtrl                     = 0x05,
    _07_PwdCtrl1                    = 0x07,
    _08_VIOCtrl                     = 0x08,
    _10_SysState                    = 0x10,
    _13_Int0                        = 0x13,
    _14_Int1                        = 0x14,
    _15_Int2                        = 0x15,
    _16_Int0Mask                    = 0x16,
    _17_Int1Mask                    = 0x17,
    _18_Int2Mask                    = 0x18,
    _19_IntClr                      = 0x19,
    _1A_MiscCtrl                    = 0x1A,
    _1B_VideoMap                    = 0x1B,
    _1C_VideoCtrl1                  = 0x1C,
    _1D_VclkCtrl                    = 0x1D,
    _20_CSCCtrl                     = 0x20,
    _3C_PGCtrl1                     = 0x3C,
    _3D_PGCtrl2                     = 0x3D,
    _3E_PGColR                      = 0x3E,
    _3F_PGColG                      = 0x3F,
    _40_PGColB                      = 0x40,
    _AE_AVIDB1                      = 0xAE,
    _AF_AVIDB2                      = 0xAF,
} EIT6604Regs;

typedef enum IT6604_Video_State
{
    VSTATE_Off=0,
    VSTATE_PwrOff,
    VSTATE_SyncWait ,
    VSTATE_SWReset,
    VSTATE_SyncChecking,
    VSTATE_HDCPSet,
    VSTATE_HDCP_Reset,
    VSTATE_ModeDetecting,
    VSTATE_VideoOn,
    VSTATE_ColorDetectReset,
    VSTATE_Reserved
} EVideoState;

#define INT_CLR_MODE            (1<<0)
#define INT_CLR_PKT             (1<<1)
#define INT_CLR_ECC             (1<<2)
#define INT_CLR_AUO             (1<<3)
#define INT_CLR_LEFTMUTE        (1<<6)
#define INT_CLR_SETMUTE         (1<<7)

#define SYS_STATE_RXPLL_LOCK    (1<<7)
#define SYS_STATE_RXCL_SPEED    (1<<6)
#define SYS_STATE_RXCL_VALID    (1<<5)
#define SYS_STATE_HDMI_MODE     (1<<4)
#define SYS_STATE_SCDT          (1<<2)
#define SYS_STATE_VCLK_DET      (1<<1)
#define SYS_STATE_PWR5V_DET     (1<<0)
#define SYS_STATE_SYNC_DONE     (SYS_STATE_RXPLL_LOCK | SYS_STATE_RXCL_VALID | SYS_STATE_SCDT | SYS_STATE_VCLK_DET)

#define INT0_VIDMODE_CHG        (1<<5)
#define INT0_HDMIMODE_CHG       (1<<4)
#define INT0_SCDTOFF            (1<<3)
#define INT0_SCDTON             (1<<2)
#define INT0_PWR5V0FF           (1<<1)
#define INT0_PWR5VON            (1<<0)

#define INT1_NEWAVIPKT_DET      (1<<2)

#define VIDEO_CTRL_DNFREEGO	(1<<7)
#define VIDEO_CTRL_SYNCEMBED	(1<<6)
#define VIDEO_CTRL_DITHER	(1<<5)
#define VIDEO_CTRL_UDFILTER	(1<<4)

#define CSC_CTRL_BYPASS		(0)
#define CSC_CTRL_RGB2YUV	(2<<0)
#define CSC_CTRL_YUV2RGB	(3<<0)

typedef SCODE (* FOnReadReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAdd, DWORD *pdwData);
typedef SCODE (* FOnWriteReg_t)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwData);

typedef enum colorimetry_flags
{
	ITU601 = 0,
	ITU709 = 1
} EColorimetry;

typedef struct IT6604_reg_addr_data
{
    EIT6604Regs eRegAddr;
    DWORD dwData;
} TIT6604RegAddrData;

typedef struct IT6604_info
{
    spinlock_t lock;

    BOOL bInit;

    DWORD dwDeviceAddr;
    DWORD dwFuncUserData;
    DWORD dwAVI_DB1;
    DWORD dwAVI_DB2;
    BOOL  bForceNewAVIInfo;
    EVideoState eVideoState;

    FOnWriteReg_t pfnWriteReg;
    FOnReadReg_t pfnReadReg;

    struct workqueue_struct *work_queues;
    struct delayed_work state_polling;

}TIT6604Info;
