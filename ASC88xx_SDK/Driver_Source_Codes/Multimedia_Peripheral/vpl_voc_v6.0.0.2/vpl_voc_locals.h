/*
 * $Header: /rd_2/project/Mozart/Components/VOC/Device_Driver/VOC/vpl_voc_locals.h 8     13/05/14 3:25p Vincent $
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
 * Description:
 *
 * $History: vpl_voc_locals.h $
 * 
 * *****************  Version 8  *****************
 * User: Vincent      Date: 13/05/14   Time: 3:25p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 7  *****************
 * User: Vincent      Date: 12/10/15   Time: 2:44p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. FEATURE: Implement buffers by linked list instead of array.
 * 2. FEATURE: Add VPL_VOC_IOC_QUEUE_BUF and VPL_VOC_IOC_DEQUEUE_BUF ioctl
 * for
 * dynamically feed display buffer address.
 *
 * *****************  Version 6  *****************
 * User: Alan         Date: 12/06/26   Time: 12:12p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Change version 5.1.0.0 to 5.0.0.2 - DONE.
 *
 * *****************  Version 5  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:48p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. FEATURE: Add profile option. - DONE
 * 2. MODIFICATION: Rename dwISRCnt to dwFrameCnt. - DONE.
 *
 * *****************  Version 4  *****************
 * User: Evelyn       Date: 12/01/12   Time: 7:58p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 3  *****************
 * User: Vincent      Date: 11/08/05   Time: 5:40p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 10/05/13   Time: 1:40p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 1:49p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Add the sturct TVideoOutDevInfo  in vpl_voc_dev_info
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 10/01/22   Time: 6:50p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 10/01/06   Time: 11:51a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: To dynamically disable the engine clock when the engine is idle - DONE.
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 09/11/27   Time: 10:41p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Modify macro VPL_VOC_BUS_NUM and VPL_VOC_MASTER_NUM for Mozart SoC - DONE
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 09/11/25   Time: 2:13p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 11:19a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 1  *****************
 * User: Albert.shen  Date: 08/10/17   Time: 5:32p
 * Created in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 *
 */

/* ============================================================================================== */
#ifndef __VPL_VOC_LOCALS_H__
#define __VPL_VOC_LOCALS_H__

/* ============================================================================================== */
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/cache.h>
#include <asm/arch/platform.h>
#include <asm/arch/irq.h>

#include "typedef.h"
#include "vpl_voc.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
    #ifdef __KERNEL__
        #define LOGLEVEL KERN_DEBUG
        #define PDEBUG(fmt, args...) printk("VPL_VOC: " fmt, ## args)
    #else
        #define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
    #endif
#else
    #define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
/* Version 4.2.0.1 modification, 2011.08.05 */
#define SYSC_CLK_GEN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x2C)
/* ======================================== */
/* Version 3.0.0.4 modification, 2010.01.06 */
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x28)
/* Version 3.0.0.5 modification, 2010.01.22 */
#define SYSC_PAD_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x44)
/* ======================================== */
/* Version 6.0.0.2 modification, 2013.05.14 */
#define SYSC_SLEW_CTRL_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x4C)
 /* ======================================== */
#define VPL_VOC_CLK_EN_NUM  0x09
/* ======================================== */

/* ============================================================================================== */
extern struct file_operations vpl_voc_fops;

/* Version 6.0.0.0 modification, 2012.10.15 */
enum videobuf_state {
    BUF_STATE_DEQUEUED = 0,
    BUF_STATE_PREPARED,
    BUF_STATE_QUEUED,
    BUF_STATE_ACTIVE,
    BUF_STATE_DONE,
    BUF_STATE_ERROR,
};

typedef struct vpl_voc_bufinfo
{
    DWORD                   dwIndex;
    DWORD		    dwFrameCount;
    struct list_head        queue;
    enum videobuf_state     state;
    BYTE                    *pbyYFrame;
    BYTE                    *pbyCbFrame;
    BYTE                    *pbyCrFrame;
} TVPLVOCBufInfo;
/* ======================================== */

/* ============================================================================================== */
typedef struct vpl_voc_dev_info
{
    volatile TVPLVOCInfo *ptMMRInfo;
    DWORD *pdwBandwidth;
    DWORD *pdwRGInterval;
    DWORD *pdwReqTimes;
    DWORD *pdwProfileClr;

/* Version 3.0.0.3 modification, 2009.11.27 */
    DWORD *pdwAHBC2Ctrl;
/* ======================================== */
/* Version 5.0.0.2 modification, 2012.06.26 */
    DWORD dwFrameCnt;
/* ======================================== */

/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */

    wait_queue_head_t wq;
    DWORD dwIrq;
/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */

/* Version 4.0.0.0 modification, 2010.02.26 */
    TVideoOutDevInfo tVideoOutDevInfo;
/* ======================================== */
    TVideoBuffer atVideoBuffer[4];
/* Version 2.0.0.1 modification, 2008.12.10 */
/* ======================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */
    DWORD dwProcessIndex;

/* Version 5.0.0.2 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    DWORD dwTotalRqTimes;
    DWORD dwPrevRqTimes;
    DWORD dwTotalRqGntInterval;
    DWORD dwPrevRqGntInterval;
    DWORD dwTotalBandwidthLow;
    DWORD dwTotalBandwidthHigh;
    DWORD dwPrevBandwidth;

    DWORD dwMaxISRInterval;
    DWORD dwMinISRInterval;
#endif //__USE_PROFILE__
/* ======================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
    spinlock_t lock;
    TVPLVOCBufInfo atVOCBufInfo[VIDEO_MAX_FRAME];
    struct list_head queued_list;
    struct list_head done_list;
    TVPLVOCBufInfo *ptCur_Frm;
    TVPLVOCBufInfo *ptNext_Frm;
    BOOL bfirst_int;
/* ======================================== */
} TVPLVOCDevInfo;

/* ============================================================================================== */
#endif //__VPL_VOC_LOCALS_H__

/* ============================================================================================== */
