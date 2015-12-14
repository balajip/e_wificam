/*
 * $Header: /rd_2/project/Mozart/Components/VOC/Device_Driver/VOC/vpl_voc.c 18    13/05/14 3:25p Vincent $
 *
 * vpl_voc
 * Driver for VPL VOC.
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
 * $History: vpl_voc.c $
 * 
 * *****************  Version 18  *****************
 * User: Vincent      Date: 13/05/14   Time: 3:25p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION: pdwSlewCtrl assigned by global macro defined in Kernel
 * instead of mapped from directly MMR address value. - DONE
 *
 * *****************  Version 17  *****************
 * User: Vincent      Date: 13/03/22   Time: 11:48a
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION: Replace some directly MMR address value by global
 * macro defined in Kernel. - DONE
 *
 * *****************  Version 16  *****************
 * User: Vincent      Date: 12/10/15   Time: 2:44p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. FEATURE: Implement buffers by linked list instead of array.
 * 2. FEATURE: Add VPL_VOC_IOC_QUEUE_BUF and VPL_VOC_IOC_DEQUEUE_BUF ioctl
 * for
 * dynamically feed display buffer address.
 *
 * *****************  Version 15  *****************
 * User: Vincent      Date: 12/07/10   Time: 8:15p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. BUG: According to driver flow, there is case gptDevInfo->bIntFlag ==
 * TRUE due to previous one interrupt ISR.
 *    and wait_event_interruptible return immediately, move
 * gptDevInfo->bIntFlag = FALSE before call wait_event_interruptible -
 * FIXED.
 *
 * *****************  Version 14  *****************
 * User: Alan         Date: 12/06/26   Time: 12:12p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Change version 5.1.0.0 to 5.0.0.2 - DONE.
 *
 * *****************  Version 13  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:48p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. FEATURE: Add profile option. - DONE
 * 2. MODIFICATION: Rename dwISRCnt to dwFrameCnt. - DONE.
 *
 * *****************  Version 12  *****************
 * User: Evelyn       Date: 12/01/12   Time: 7:58p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 11  *****************
 * User: Shihchun.fanchiang Date: 11/11/04   Time: 1:03p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 11  *****************
 * User: SC. FanChiang Date: 11/11/04   Time:12:57p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 10  *****************
 * User: Evelyn       Date: 11/09/14   Time: 5:07p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 9  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:35p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION: Modify interface with HDMI Tx device
 *
 * *****************  Version 8  *****************
 * User: Vincent      Date: 11/08/05   Time: 5:41p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * FEATURE: Add module parameter gsdwClkSrc for PLL refence clock source
 * selection.
 *
 * *****************  Version 7  *****************
 * User: Alan         Date: 11/07/14   Time: 3:17p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * FEATURE: Add 16-bit YUV with separate or embedded SYNC support for HDMI
 * Tx device - DONE.
 *
 * *****************  Version 6  *****************
 * User: Alan         Date: 11/07/14   Time: 2:04p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * FEATURE: Add BT.1120 support for HDMI Tx device - DONE.
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 10/12/31   Time: 11:16a
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Do not wait complete when VPL_VOC_IOC_GET_BUF is called. - DONE
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 10/09/17   Time: 3:58p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 10/06/09   Time: 1:28p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 10/05/13   Time: 1:40p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 17  *****************
 * User: Albert.shen  Date: 10/04/28   Time: 7:50p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 16  *****************
 * User: Albert.shen  Date: 10/04/26   Time: 11:07a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 15  *****************
 * User: Albert.shen  Date: 10/03/11   Time: 12:04p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 14  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 1:49p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * 1. FEATURE: Add function VPL_VOC_Start and VPL_VOC_Stop - DONE.
 * 2. FEATURE: Dynamically change the frequency of VOC output clock in VPL_VOC_Start - DONE.
 *
 * *****************  Version 13  *****************
 * User: Albert.shen  Date: 10/01/22   Time: 6:50p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 12  *****************
 * User: Albert.shen  Date: 10/01/06   Time: 11:51a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: To dynamically disable the engine clock when the engine is idle - DONE.
 *
 * *****************  Version 11  *****************
 * User: Albert.shen  Date: 09/11/27   Time: 10:41p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Modify macro VPL_VOC_BUS_NUM and VPL_VOC_MASTER_NUM for Mozart SoC - DONE
 *
 * *****************  Version 10  *****************
 * User: Albert.shen  Date: 09/11/25   Time: 2:13p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 9  *****************
 * User: Albert.shen  Date: 09/06/22   Time: 11:45a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 09/06/15   Time: 11:22a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 09/06/15   Time: 11:19a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 11:19a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 08/12/05   Time: 9:28p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 08/11/13   Time: 5:00p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 08/11/06   Time: 10:03a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/11/06   Time: 9:52a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION: Update buffer pointer only when top field - DONE.
 * 2. MODIFICATION: Use status register to decide that which buffer
 * pointer should be updated. -DONE.
 *
 * *****************  Version 1  *****************
 * User: Albert.shen  Date: 08/10/17   Time: 5:32p
 * Created in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 *
 */
/* ============================================================================================== */

 /* ======================================================================== */
#ifndef __KERNEL__
    #define __KERNEL__
#endif
#ifndef MODULE
    #define MODULE
#endif

/* ======================================================================== */
#include "vpl_voc_locals.h"
#include "tv_encoder.h"
#include "hdmi_tx.h"
#include <linux/delay.h>
/* ======================================================================== */

/* Version 1.1.0.0 modification, 2008.11.06 */
const CHAR VPL_VOC_ID[] = "$Version: "VPL_VOC_ID_VERSION"  (VPL_VOC) $";
/* ======================================== */
static TVPLVOCDevInfo *gptDevInfo = NULL;
static SDWORD gsdwMajor = 0;
/* Version 4.0.0.2 modification, 2010.04.26 */
static SDWORD gsdwClkFreq = 24;
/* ======================================== */
/* Version 4.2.0.1 modification, 2011.08.05 */
static SDWORD gsdwClkSrc = 0;
/* ======================================== */
/* Version 2.0.0.1 modification, 2008.12.10 */
/* ======================================== */
/* Version 4.0.0.3 modification, 2010.04.28 */
extern struct tv_encoder_device tv_encoder_ops;
extern struct hdmi_tx_device hdmi_tx_ops;
static struct tv_encoder_device *TVEncoder = &tv_encoder_ops;
static struct hdmi_tx_device *HDMITX = &hdmi_tx_ops;
/* ======================================== */

/* ============================================================================================== */
MODULE_AUTHOR ("VN Inc.");
MODULE_LICENSE("GPL");

module_param (gsdwMajor, int, 0644);
MODULE_PARM_DESC (gsdwMajor, "Major number for VPL_VOC module");
/* Version 4.0.0.2 modification, 2010.04.26 */
module_param (gsdwClkFreq, int, 0644);
MODULE_PARM_DESC (gsdwClkFreq, "Oscillator clock frequency in MHz");
/* ======================================== */
/* Version 4.2.0.1 modification, 2011.08.05 */
module_param (gsdwClkSrc, int, 0644);
MODULE_PARM_DESC (gsdwClkSrc, "PLL Reference Clock Source 0:XTAL((SYS_I_OSC_2_CLK) 1:OSC(VOC_I_CLK)");
/* ======================================== */
/* ======================================================================== */
static void IntrDisable(void)
{
    writel(readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0xFFFFFFFD, &gptDevInfo->ptMMRInfo->dwCtrl);

    PDEBUG("Interrupt disabled !!\n");
}

/* ======================================================================== */
static void IntrEnable(void)
{
    writel(readl(&gptDevInfo->ptMMRInfo->dwCtrl)|0x00000002, &gptDevInfo->ptMMRInfo->dwCtrl);

    PDEBUG("Interrupt enabled !!\n");
}

/* ======================================================================== */
static void IntrClear(void)
{
    writel(readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0xFFFFFFFE, &gptDevInfo->ptMMRInfo->dwCtrl);

    PDEBUG("Interrupt cleared !!\n");
}

/* ============================================================================================== */
static void VPL_VOC_Reset(void)
{
    int i;
    PDEBUG("Enter Reset function...\n");

    writel(0x00000008, &gptDevInfo->ptMMRInfo->dwCtrl);
    //while ((readl(&gptDevInfo->ptMMRInfo->dwCtrl)&0x00000008) != 0);

/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */
    gptDevInfo->dwFrameCnt = 0;
/* Version 6.0.0.0 modification, 2012.10.15 */
    for (i = 0; i < VIDEO_MAX_FRAME; i++)
        gptDevInfo->atVOCBufInfo[i].state = BUF_STATE_DEQUEUED;

    INIT_LIST_HEAD(&gptDevInfo->queued_list);
    INIT_LIST_HEAD(&gptDevInfo->done_list);
    gptDevInfo->ptCur_Frm = gptDevInfo->ptNext_Frm = NULL;
/* ======================================== */
/* Version 2.0.0.1 modification, 2008.12.10 */
/* Version 4.0.0.4 modification, 2010.06.09 */
    gptDevInfo->dwProcessIndex = 0; //avoid process index go over display index
/* ======================================== */
/* ======================================== */

    PDEBUG("Exit Reset function !!\n");
}
/* Version 4.0.0.3 modification, 2010.04.28 */
static void VPL_VOC_PLL_CONFIG(DWORD dwRatio, DWORD dwDivider)
{
    DWORD *pdwPLLRatio;
    DWORD *pdwPLLCtrl;
/* Version 6.0.0.1 modification, 2013.03.22 */
/* ======================================== */
    DWORD dwSYSCClkGenCtrl;

/* Version 6.0.0.1 modification, 2013.03.22 */
    pdwPLLCtrl = (DWORD *)ioremap((int)(VPL_PLLC_MMR_BASE+0x1C), sizeof(DWORD));
    pdwPLLRatio = (DWORD *)ioremap((int)(VPL_PLLC_MMR_BASE+0x20), sizeof(DWORD));

    dwSYSCClkGenCtrl = readl(SYSC_CLK_GEN_MMR);
/* ======================================== */
/* Version 4.2.0.1 modification, 2011.08.05 */
    dwSYSCClkGenCtrl = ((dwSYSCClkGenCtrl & 0xFFC0FFFF) | (dwDivider<<16));
/* ======================================== */
    /*Adjust PLL control*/
    writel(0x00000009, pdwPLLCtrl);
    writel(dwRatio, pdwPLLRatio);
/* Version 6.0.0.1 modification, 2013.03.22 */
    writel(dwSYSCClkGenCtrl, SYSC_CLK_GEN_MMR);
/* ======================================== */
    writel(0x00000001, pdwPLLCtrl);

    /*Wait for PLL lock*/
    while ((readl(pdwPLLCtrl) & 0x4) != 0x4)
    {
        udelay(1);
    }

    /*Start to output the clock of PLL*/
    writel(0x00000003, pdwPLLCtrl);
/* Version 5.0.0.1 modification, 2013.03.22 */
/* ======================================== */
    iounmap(pdwPLLCtrl);
    iounmap(pdwPLLRatio);
}

/* ======================================== */
/* ======================================================================== */
/* Version 4.0.0.0 modification, 2010.02.26 */
static void VPL_VOC_Start(int dwClkFreq)
{
    DWORD dwPinNum, dwRatio;
/* Version 4.0.0.3 modification, 2010.04.28 */
    DWORD dwDivider = 2;

/* Version 4.0.0.2 modification, 2010.04.26 */
    if (gsdwClkFreq == 27)
    {
        switch (dwClkFreq)
        {
            case 74: /*74.25MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 10); //with 27MHz osc
                dwDivider = 4;
                break;
            case 54: /*54 MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 3);
                break;
            case 108: /*108 MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 7); //with 27MHz osc
                break;
            case 148: /*148.5MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 10); //with 27MHz osc
                break;
            case 121: /*121.5 MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 8);
                break;
            default: /* 27 MHz */
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 1);
                break;
        }
    }
    else // (gsdwClkFreq == 24)
    {
        switch (dwClkFreq)
        {
            case 74: /*74.25MHz*/
                dwRatio = ((0x1<<24) | (0x3<<16) | (0x1<<8) | 98);
                break;
            case 54: /*54 MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 0x8);
                dwDivider = 4;
                break;
            case 108: /*108 MHz*/
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 0x8);
                break;
            case 148: /*148.5MHz*/
                dwRatio = ((0x1<<24) | (0x2<<16) | (0x1<<8) | 98);
                break;
            default: /* 27 MHz */
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 0x8);
                dwDivider = 8;
                break;
        }
    }

    VPL_VOC_PLL_CONFIG(dwRatio, dwDivider);
/* ======================================== */
/* Version 4.3.0.0 modification, 2011.08.31 */
    dwPinNum = (gptDevInfo->tVideoOutDevInfo.eOutFormat!=VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656) ? 0x7 : 0x1;
/* ======================================== */
    /*Enable voc output pins*/
    writel(readl(SYSC_PAD_EN_MMR)|(dwPinNum<<24), SYSC_PAD_EN_MMR);
    /*Enalbe VOC module*/
    writel((readl(&gptDevInfo->ptMMRInfo->dwCtrl)| 0x00000004), &gptDevInfo->ptMMRInfo->dwCtrl);

}

/* Version 4.0.0.3 modification, 2010.04.28 */
static void VPL_VOC_Start_Device(void)
{
/* Version 4.2.0.0 modification, 2011.07.13 */
/* Version 4.3.0.0 modification, 2011.08.31 */
/* ======================================== */
/* ======================================== */
/* Version 4.3.0.0 modification, 2011.08.31 */
    if (gptDevInfo->tVideoOutDevInfo.eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)
/* ======================================== */
    {
/* Version 4.0.0.5 modification, 2010.09.17 */
        // Use TV encoder
        if ((readl(&gptDevInfo->ptMMRInfo->dwOutSize)&0xFFFF)== 0x360)
        {
            // PAL mode
            TVEncoder->open(1);
        }
        else
        {
            // NTSC mode
            TVEncoder->open(0);
        }
/* ======================================== */
    }
    else//VIDEO_SIGNAL_FORMAT_RGB24 || VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS || VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120
    {
/* Version 4.2.0.0 modification, 2011.07.13 */
/* ======================================== */
/* Version 4.3.0.0 modification, 2011.08.31 */
        if (gptDevInfo->tVideoOutDevInfo.eOutFormat == VIDEO_SIGNAL_FORMAT_RGB24)
        {
            HDMITX->open(gptDevInfo->tVideoOutDevInfo.dwVICID, gptDevInfo->tVideoOutDevInfo.dwClkFreq, FORMAT_RGB444, INSIG_SYNCSEP, FORMAT_RGB444);
        }
        else if (gptDevInfo->tVideoOutDevInfo.eOutFormat == VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS)
        {
            HDMITX->open(gptDevInfo->tVideoOutDevInfo.dwVICID, gptDevInfo->tVideoOutDevInfo.dwClkFreq, FORMAT_YUV422, INSIG_SYNCSEP, FORMAT_RGB444);
        }
        else //VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120
        {
            HDMITX->open(gptDevInfo->tVideoOutDevInfo.dwVICID, gptDevInfo->tVideoOutDevInfo.dwClkFreq, FORMAT_YUV422, INSIG_SYNCEMB, FORMAT_RGB444);
        }
/* ======================================== */
    }
}
/* ======================================== */

/* ======================================================================== */
static void VPL_VOC_Stop(void)
{
    /*Disable voc output pins*/
    writel(readl(SYSC_PAD_EN_MMR)&(~(0x7<<24)), SYSC_PAD_EN_MMR);
    /*Disable VOC module*/
    writel((readl(&gptDevInfo->ptMMRInfo->dwCtrl)& 0xFFFFFFFB), &gptDevInfo->ptMMRInfo->dwCtrl);
}

/* ======================================== */
/* ======================================================================== */
static void WaitComplete(void)
{
    int retval;
    PDEBUG("Enter WaitComplete function...\n");
#ifdef __VOC_USE_IRQ__
/* Version 6.0.0.0 modification, 2012.10.15 */
check:
    retval = wait_event_interruptible(gptDevInfo->wq, !list_empty(&gptDevInfo->done_list));
    if (retval == -ERESTARTSYS)
        goto check;
/* ======================================== */
#else //__VOC_USE_IRQ__
    // Polling
/* Version 2.0.0.1 modification, 2008.12.10 */
    while ((readl(VPL_VOC_STAT_OFFSET+(DWORD)(gptDevInfo->ptMMRInfo))&0x01)==0)
/* ======================================== */
    {
        mdelay(1);
    }
#endif //__VOC_USE_IRQ__
    PDEBUG("Exit WaitComplete function !!\n");
}

/* ======================================================================== */
#ifdef __VOC_USE_IRQ__
static irqreturn_t ISR(int dwIrq, void *dev_id)
{

/* Version 5.0.0.2 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    DWORD dwReqTimes;
    DWORD dwRGInterval;
    DWORD dwBandwidth;
#endif //__USE_PROFILE__
    DWORD dwCtrl, dwStat;

    PDEBUG("Enter ISR function !!\n");
    dwCtrl = readl(VPL_VOC_CTRL_OFFSET+(DWORD)(gptDevInfo->ptMMRInfo));
    dwStat = readl(VPL_VOC_STAT_OFFSET+(DWORD)(gptDevInfo->ptMMRInfo));
/* ======================================== */

/* Version 5.0.0.2 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
    dwReqTimes = readl(gptDevInfo->pdwReqTimes);
    if (dwReqTimes >= gptDevInfo->dwPrevRqTimes)
    {
        gptDevInfo->dwTotalRqTimes += (dwReqTimes-gptDevInfo->dwPrevRqTimes);
    }
    else
    {
        gptDevInfo->dwTotalRqTimes += (65536-gptDevInfo->dwPrevRqTimes+dwReqTimes);
    }
    gptDevInfo->dwPrevRqTimes = dwReqTimes;

    dwRGInterval = readl(gptDevInfo->pdwRGInterval);
    if (dwRGInterval >= gptDevInfo->dwPrevRqGntInterval)
    {
        gptDevInfo->dwTotalRqGntInterval += (dwRGInterval-gptDevInfo->dwPrevRqGntInterval);
    }
    else
    {
        gptDevInfo->dwTotalRqGntInterval += (65536-gptDevInfo->dwPrevRqGntInterval+dwRGInterval);
    }
    gptDevInfo->dwPrevRqGntInterval = dwRGInterval;

    dwBandwidth = readl(gptDevInfo->pdwBandwidth);
    if (dwBandwidth >= gptDevInfo->dwPrevBandwidth)
    {
        gptDevInfo->dwTotalBandwidthLow += (dwBandwidth-gptDevInfo->dwPrevBandwidth);
    }
    else
    {
        gptDevInfo->dwTotalBandwidthLow = ((~gptDevInfo->dwPrevBandwidth)+dwBandwidth+1); //(4294967295-gptDevInfo->dwPrevBandwidth)+dwBandwidth+1);
        gptDevInfo->dwTotalBandwidthHigh += 1;
    }
    gptDevInfo->dwPrevBandwidth = dwBandwidth;
#endif //__USE_PROFILE__
/* ======================================== */

/* Version 3.0.0.0 modification, 2009.06.15 */
/* Version 3.0.0.1 modification, 2009.06.22 */
/* ======================================== */
/* ======================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
    if (!gptDevInfo->bfirst_int && (gptDevInfo->ptCur_Frm != gptDevInfo->ptNext_Frm))
    {
    gptDevInfo->ptCur_Frm->state = BUF_STATE_DONE;
        list_add_tail(&gptDevInfo->ptCur_Frm->queue, &gptDevInfo->done_list); //move to done_list
    gptDevInfo->ptCur_Frm = gptDevInfo->ptNext_Frm;
    }

    gptDevInfo->bfirst_int = 0;
    if (!list_empty(&gptDevInfo->queued_list))
    {
        gptDevInfo->ptNext_Frm = list_first_entry(&gptDevInfo->queued_list, TVPLVOCBufInfo, queue);

        list_del_init(&gptDevInfo->ptNext_Frm->queue);
        gptDevInfo->ptNext_Frm->state = BUF_STATE_ACTIVE;

        writel((DWORD)( gptDevInfo->ptNext_Frm->pbyYFrame),
                      (VPL_VOC_Y_BUFF1_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
        writel((DWORD)(gptDevInfo->ptNext_Frm->pbyCbFrame),
                  (VPL_VOC_CB_BUFF1_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
        writel((DWORD)( gptDevInfo->ptNext_Frm->pbyCrFrame),
                      (VPL_VOC_CR_BUFF1_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));

        gptDevInfo->ptNext_Frm->dwFrameCount = gptDevInfo->dwFrameCnt;
    }
/* ======================================== */

    IntrClear();
/* ======================================== */

    gptDevInfo->dwFrameCnt++;

/* Version 1.1.0.0 modification, 2008.11.06 */
/* ======================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */
    wake_up_interruptible(&gptDevInfo->wq);
    return IRQ_HANDLED;
}
#endif //__VOC_USE_IRQ__

/* ======================================================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
static int VPL_VOC_QBUF(TVideoBuffer *ptVideoBuf)
{
    int ret = 0;
    TVPLVOCBufInfo *ptVOCBufInfo;
    unsigned long flags = 0;

    spin_lock_irqsave(&gptDevInfo->lock, flags);

    if (ptVideoBuf->dwIndex > VIDEO_MAX_FRAME)
    {
        PDEBUG("VPL_VOC_QBUF: Index out of range\n");
        goto done;
    }

    ptVOCBufInfo = &(gptDevInfo->atVOCBufInfo[ptVideoBuf->dwIndex]);

    switch(ptVOCBufInfo->state)
    {
        case BUF_STATE_DEQUEUED:
            ptVOCBufInfo->dwIndex = ptVideoBuf->dwIndex;
            ptVOCBufInfo->pbyYFrame = ptVideoBuf->pbyYFrame;
            ptVOCBufInfo->pbyCbFrame = ptVideoBuf->pbyCbFrame;
            ptVOCBufInfo->pbyCrFrame = ptVideoBuf->pbyCrFrame;
            ptVOCBufInfo->state = BUF_STATE_PREPARED;
        case BUF_STATE_PREPARED:
            break;
        default:
            printk("VPL_VOC_QBUF: buffer is already queued or active.\n");
            ret = -EINVAL;
            goto done;
    }

    list_add_tail(&ptVOCBufInfo->queue, &gptDevInfo->queued_list);
    ptVOCBufInfo->state = BUF_STATE_QUEUED;

done:
    spin_unlock_irqrestore(&gptDevInfo->lock, flags);
    return ret;
}
/* ======================================================================== */
static int VPL_VOC_DQBUF(TVideoBuffer *ptVideoBuf)
{
    int retval;
    TVPLVOCBufInfo *ptVOCBufInfo;
    unsigned long flags = 0;

    spin_lock_irqsave(&gptDevInfo->lock, flags);

check:
    if (list_empty(&gptDevInfo->done_list)) {
        spin_unlock_irqrestore(&gptDevInfo->lock, flags);
        retval = wait_event_interruptible(gptDevInfo->wq, !list_empty(&gptDevInfo->done_list));
        spin_lock_irqsave(&gptDevInfo->lock, flags);

        if (retval == -ERESTARTSYS)
            goto check;
    }

    ptVOCBufInfo = list_first_entry(&gptDevInfo->done_list, TVPLVOCBufInfo, queue);

    ptVideoBuf->dwIndex = ptVOCBufInfo->dwIndex;
    ptVideoBuf->dwFrameCount = ptVOCBufInfo->dwFrameCount;
    ptVideoBuf->pbyYFrame = ptVOCBufInfo->pbyYFrame;
    ptVideoBuf->pbyCbFrame = ptVOCBufInfo->pbyCbFrame;
    ptVideoBuf->pbyCrFrame = ptVOCBufInfo->pbyCrFrame;

    list_del(&ptVOCBufInfo->queue);

    ptVOCBufInfo->state = BUF_STATE_DEQUEUED;

    retval = 0;

    spin_unlock_irqrestore(&gptDevInfo->lock, flags);
    return retval;
}

/* ======================================================================== */
/* ======================================== */
static int MMap(struct file *file, struct vm_area_struct *vma)
{
    DWORD dwSize;
    dwSize = vma->vm_end - vma->vm_start;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_flags |= VM_RESERVED;

    if (remap_pfn_range(vma, vma->vm_start, (VPL_VOC_MMR_BASE>>PAGE_SHIFT), dwSize, vma->vm_page_prot))
    {
        return -EAGAIN;
    }

    PDEBUG("Start address = 0x%08lX, end address = 0x%08lX\n", vma->vm_start, vma->vm_end);
    return 0;
}

/* ======================================================================== */
static int Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long dwArg)
{
    DWORD dwVersionNum;
    DWORD adwData[2];
    DWORD dwRatio, dwDivider;
/* Version 6.0.0.2 modification, 2013.05.14 */
/* ======================================== */
    int err = 0;
/* Version 5.0.0.0 modification, 2011.09.14 */
    DWORD dwCableDetect;
/* ======================================== */

/* Version 6.0.0.0 modification, 2012.10.15 */
   TVideoBuffer tVideoBuf_User;
   TVideoBuffer *ptVideoBuf;
/* ======================================== */
    int i;
    if (filp->private_data == NULL)
    {
        PDEBUG("device error\n");
        return -ENODEV;
    }

    if (_IOC_TYPE(cmd) != VPL_VOC_IOC_MAGIC)
    {
        PDEBUG("command IOC MAGIC is not compatible with VPL_VOC_IOC_MAGIC\n");
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > VPL_VOC_IOC_MAX_NUMBER)
    {
        PDEBUG("command IOC MAGIC exceeds VPL_VOC_IOC_MAXNR\n");
        return -ENOTTY;
    }

/* Version 4.0.0.5 modification, 2010.09.17 */
    if (cmd != VPL_VOC_IOC_SET_PAL)
/* ======================================== */
    {
        if (_IOC_DIR(cmd) & _IOC_READ)
        {
            err = !access_ok(VERIFY_WRITE, (void *)dwArg, _IOC_SIZE(cmd));
        }
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
        {
            err =  !access_ok(VERIFY_READ, (void *)dwArg, _IOC_SIZE(cmd));
        }
/* Version 4.0.0.5 modification, 2010.09.17 */
    }
/* ======================================== */

    if (err)
    {
        return -EFAULT;
    }

    switch (cmd)
    {
        case VPL_VOC_IOC_RESET:
            VPL_VOC_Reset();
            break;
        case VPL_VOC_IOC_WAIT_COMPLETE:
            WaitComplete();
            break;
        case VPL_VOC_IOC_WRITE_BUF_INFO:
/* Version 6.0.0.0 modification, 2012.10.15 */
            ptVideoBuf = (TVideoBuffer *)dwArg;
            for (i=0; i<4; i++)
            {
                ptVideoBuf = (TVideoBuffer *)dwArg + i;
                gptDevInfo->atVOCBufInfo[i].dwIndex = ptVideoBuf->dwIndex;
                gptDevInfo->atVOCBufInfo[i].pbyYFrame = ptVideoBuf->pbyYFrame;
                gptDevInfo->atVOCBufInfo[i].pbyCbFrame = ptVideoBuf->pbyCbFrame;
                gptDevInfo->atVOCBufInfo[i].pbyCrFrame = ptVideoBuf->pbyCrFrame;
            }
/* ======================================== */
        break;
        case VPL_VOC_IOC_GET_BANDWIDTH:
            copy_to_user((DWORD *)dwArg, gptDevInfo->pdwBandwidth, sizeof(DWORD));
            break;
        case VPL_VOC_IOC_GET_REQ_TIMES:
            copy_to_user((DWORD *)dwArg, gptDevInfo->pdwReqTimes, sizeof(DWORD));
            break;
        case VPL_VOC_IOC_GET_RG_INTERVAL:
            copy_to_user((DWORD *)dwArg, gptDevInfo->pdwRGInterval, sizeof(DWORD));
            break;
        case VPL_VOC_IOC_CLEAR_PROFILE:
            writel((0x1<<VPL_VOC_MASTER_NUM), gptDevInfo->pdwProfileClr);
            break;
        case VPL_VOC_IOC_GET_BUF:
/* Version 6.0.0.0 modification, 2012.10.15 */
            if (list_empty(&gptDevInfo->done_list))
            {
                return -EFAULT;
            }

            VPL_VOC_DQBUF(&tVideoBuf_User);
            gptDevInfo->dwProcessIndex = tVideoBuf_User.dwIndex;
/* ======================================== */
            copy_to_user((DWORD *)dwArg, (DWORD *)&(gptDevInfo->dwProcessIndex), sizeof(DWORD));
            break;
        case VPL_VOC_IOC_RELEASE_BUF:
/* Version 6.0.0.0 modification, 2012.10.15 */
            tVideoBuf_User.dwIndex = gptDevInfo->atVOCBufInfo[gptDevInfo->dwProcessIndex].dwIndex;
            tVideoBuf_User.pbyYFrame = gptDevInfo->atVOCBufInfo[gptDevInfo->dwProcessIndex].pbyYFrame;
            tVideoBuf_User.pbyCbFrame = gptDevInfo->atVOCBufInfo[gptDevInfo->dwProcessIndex].pbyCbFrame;
            tVideoBuf_User.pbyCrFrame = gptDevInfo->atVOCBufInfo[gptDevInfo->dwProcessIndex].pbyCrFrame;
            VPL_VOC_QBUF(&tVideoBuf_User);
/* ======================================== */
            break;
        case VPL_VOC_IOC_GET_VERSION_NUMBER:
            dwVersionNum = VPL_VOC_VERSION;
            copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
            break;
        case VPL_VOC_IOC_GET_FRAME_CNT:
            copy_to_user((DWORD *)dwArg, (DWORD *)&(gptDevInfo->dwFrameCnt), sizeof(DWORD));
            break;
/* Version 4.0.0.0 modification, 2010.02.26 */
        case VPL_VOC_IOC_START:
/* Version 6.0.0.0 modification, 2012.10.15 */
            if (list_empty(&gptDevInfo->queued_list) && gptDevInfo->ptCur_Frm == NULL) {
                printk("VPL_VOC_IOC_START: At least ADD one buffer before start\n");
                return -EINVAL;
            }
            if (gptDevInfo->ptCur_Frm == NULL)
            {
                gptDevInfo->ptCur_Frm = gptDevInfo->ptNext_Frm = list_first_entry(&gptDevInfo->queued_list, TVPLVOCBufInfo, queue);
                list_del_init(&gptDevInfo->ptCur_Frm->queue);
                gptDevInfo->ptCur_Frm->state = BUF_STATE_ACTIVE;

                writel((DWORD)(gptDevInfo->ptCur_Frm->pbyYFrame),
                        (VPL_VOC_Y_BUFF0_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
                writel((DWORD)(gptDevInfo->ptCur_Frm->pbyCbFrame),
                        (VPL_VOC_CB_BUFF0_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
                writel((DWORD)( gptDevInfo->ptCur_Frm->pbyCrFrame),
                        (VPL_VOC_CR_BUFF0_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));

                writel((DWORD)(gptDevInfo->ptCur_Frm->pbyYFrame),
                        (VPL_VOC_Y_BUFF1_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
                writel((DWORD)(gptDevInfo->ptCur_Frm->pbyCbFrame),
                        (VPL_VOC_CB_BUFF1_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
                writel((DWORD)( gptDevInfo->ptCur_Frm->pbyCrFrame),
                        (VPL_VOC_CR_BUFF1_ADDR_OFFSET+(DWORD)gptDevInfo->ptMMRInfo));
                gptDevInfo->bfirst_int = 1;
            }
/* ======================================== */
            copy_from_user(&(gptDevInfo->tVideoOutDevInfo), (void *)dwArg, sizeof(TVideoOutDevInfo));
            VPL_VOC_Start(gptDevInfo->tVideoOutDevInfo.dwClkFreq);

/* Version 5.0.0.1 modification, 2011.11.04 */
/* Version 6.0.0.2 modification, 2013.05.14 */
            if ((gptDevInfo->tVideoOutDevInfo.dwDispWidth) == 1920 || (gptDevInfo->tVideoOutDevInfo.dwDispWidth) == 1280)
            {
                writel(((readl(SYSC_SLEW_CTRL_MMR)&0xFFFFFFFC)|0x2), SYSC_SLEW_CTRL_MMR);
            }
            else
            {
                writel(((readl(SYSC_SLEW_CTRL_MMR)&0xFFFFFFFC)), SYSC_SLEW_CTRL_MMR);
            }
/* ======================================== */
/* ======================================== */
/* Version 4.0.0.3 modification, 2010.04.28 */
            VPL_VOC_Start_Device();
/* ======================================== */
            break;
        case VPL_VOC_IOC_STOP:
            VPL_VOC_Stop();
            break;
/* ======================================== */
/* Version 4.0.0.3 modification, 2010.04.28 */
        case VPL_VOC_IOC_PLL_CONFIG:
            copy_from_user(adwData, (void *)dwArg, 2*sizeof(DWORD));
            VPL_VOC_PLL_CONFIG(adwData[0], adwData[1]);
            break;
/* ======================================== */
/* Version 4.0.0.5 modification, 2010.09.17 */
        case VPL_VOC_IOC_SET_PAL:
            if (gsdwClkFreq == 27)
            {
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 1);
                dwDivider = 2;
            }
            else
            {
                dwRatio = ((0x3<<24) | (0x0<<16) | (0x0<<8) | 0x8);
                dwDivider = 8;
            }
            VPL_VOC_PLL_CONFIG(dwRatio, dwDivider);
            TVEncoder->open(dwArg);
            break;
/* ======================================== */
/* Version 5.0.0.0 modification, 2011.09.14 */
        case VPL_VOC_IOC_CABLE_DETECT:
            if (gptDevInfo->tVideoOutDevInfo.eOutFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)
            {
                dwCableDetect = (DWORD)(TVEncoder->cable_detect());
                copy_to_user((DWORD *)dwArg, &dwCableDetect, sizeof(DWORD));
            }
            else
            {
                return -EPERM;
            }
            break;
/* ======================================== */

/* Version 6.0.0.0 modification, 2012.10.15 */
        case VPL_VOC_IOC_QUEUE_BUF:
            copy_from_user(&tVideoBuf_User, (void *)dwArg, sizeof(TVideoBuffer));
            VPL_VOC_QBUF(&tVideoBuf_User);
            break;
        case VPL_VOC_IOC_DEQUEUE_BUF:
            VPL_VOC_DQBUF(&tVideoBuf_User);
            copy_to_user((DWORD *)dwArg, &tVideoBuf_User, sizeof(TVideoBuffer));
            break;
/* ======================================== */
        default:
            return -ENOTTY;
    }

    return 0;
}
/* Version 6.0.0.0 modification, 2012.10.15 */
static void QueueClear(void)
{
    int i;
    unsigned long flags = 0;

    spin_lock_irqsave(&gptDevInfo->lock, flags);
    for (i = 0; i < VIDEO_MAX_FRAME; i++)
    {
        if (/*(gptDevInfo->atVOCBufInfo[i].state == BUF_STATE_ACTIVE) ||*/
            (gptDevInfo->atVOCBufInfo[i].state == BUF_STATE_QUEUED))
        {
            gptDevInfo->atVOCBufInfo[i].state = BUF_STATE_DONE;
            {
                list_move_tail(&(gptDevInfo->atVOCBufInfo[i].queue), &gptDevInfo->done_list);
            }
        }
    }

    spin_unlock_irqrestore(&gptDevInfo->lock, flags);
    wake_up_all(&gptDevInfo->wq);
}
/* ======================================== */
/* ======================================================================== */
static int Close(struct inode *inode, struct file *pfile)
{

    PDEBUG("Enter Close function...\n");
    module_put(THIS_MODULE);

    if (module_refcount(THIS_MODULE)==0)
    {

/* Version 5.0.0.2 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
        printk("Frame Count: %lu\n", gptDevInfo->dwFrameCnt);
        printk("Bus 2 Request times = %08lu\n", gptDevInfo->dwTotalRqTimes);
        printk("Bus 2 Req-Grant interval = %08lu\n", gptDevInfo->dwTotalRqGntInterval);
        printk("Bus 2 Bandwidth = %08lu%08lu\n", gptDevInfo->dwTotalBandwidthHigh, gptDevInfo->dwTotalBandwidthLow);
#endif //__USE_PROFILE__
/* ======================================== */

        pfile->private_data = NULL;

        IntrDisable();
        IntrClear();
/* Version 6.0.0.0 modification, 2012.10.15 */
        QueueClear();
/* ======================================== */
/* Version 4.0.0.0 modification, 2010.02.26 */
        writel(readl(SYSC_PAD_EN_MMR)&(~(0x7<<24)), SYSC_PAD_EN_MMR);
/* ======================================== */
        if (gptDevInfo->dwIrq != (DWORD)NULL)
        {
            free_irq(VPL_VOC_IRQ_NUM, gptDevInfo);
        }

        gptDevInfo->dwIrq = 0;

    }
    PDEBUG("Exit Close function !!\n");
    return 0;
}

/* ======================================================================== */
static int Open(struct inode *inode, struct file *filp)
{
 #ifdef __VOC_USE_IRQ__
    int scResult;
 #endif

    PDEBUG("Enter Open function...\n");

    if (filp->private_data == NULL)
    {
        filp->private_data = gptDevInfo; /* for other methods */
    }

  if (module_refcount(THIS_MODULE)==0)
    {
/* Version 3.0.0.4 modification, 2010.01.06 */
        writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VPL_VOC_CLK_EN_NUM), SYSC_CLK_EN_MMR);
/* ======================================== */
#ifdef __VOC_USE_IRQ__
        if (gptDevInfo->dwIrq == (DWORD)NULL)
        {
            IntrDisable();
/* Version 3.0.0.0 modification, 2009.06.15 */
            fLib_SetIntTrig(VPL_VOC_IRQ_NUM, LEVEL, H_ACTIVE);
/* ======================================== */
/* Version 3.0.0.2 modification, 2009.11.25 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
            scResult = request_irq(VPL_VOC_IRQ_NUM, &ISR, SA_INTERRUPT, "vpl_voc", gptDevInfo);
#else
            scResult = request_irq(VPL_VOC_IRQ_NUM, &ISR, IRQF_DISABLED, "vpl_voc", gptDevInfo);
#endif
/* ======================================== */
            PDEBUG("Interrupt requested\n");
            if (scResult < 0)
            {
                PDEBUG("Cannot get irq %d !!\n", VPL_VOC_IRQ_NUM);
                if (try_module_get(THIS_MODULE) == 0)
                {
                    return -EBUSY;
                }

                Close(inode, filp);
                return scResult;
            }

            disable_irq(VPL_VOC_IRQ_NUM);
            enable_irq(VPL_VOC_IRQ_NUM);
            gptDevInfo->dwIrq = VPL_VOC_IRQ_NUM;
/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */
            IntrClear();
            IntrEnable();
        }
#endif //__VOC_USE_IRQ__
        VPL_VOC_Reset();

/* Version 5.0.0.2 modification, 2012.06.26 */
#ifdef __USE_PROFILE__
        writel((0x1<<VPL_VOC_MASTER_NUM), gptDevInfo->pdwProfileClr);
        gptDevInfo->dwTotalRqTimes = 0;
        gptDevInfo->dwPrevRqTimes = 0;
        gptDevInfo->dwTotalRqGntInterval = 0;
        gptDevInfo->dwPrevRqGntInterval = 0;
        gptDevInfo->dwTotalBandwidthLow = 0;
        gptDevInfo->dwTotalBandwidthHigh = 0;
        gptDevInfo->dwPrevBandwidth = 0;
        //gptDevInfo->dwMaxISRInterval = 0;
        //gptDevInfo->dwMinISRInterval = 0;
#endif //__USE_PROFILE__
/* ======================================== */

/* Version 4.0.0.0 modification, 2010.02.26 */
/* ======================================== */
    }
    if (try_module_get(THIS_MODULE) == 0)
    {
        return -EBUSY;
    }
    return S_OK;          /* success */
}

/* ======================================================================== */
struct file_operations vpl_voc_fops =
{
    ioctl:      Ioctl,
    mmap:       MMap,
    open:       Open,
    release:    Close,
};

/* ======================================================================== */
static void Cleanup_Module(void)
{
/* Version 3.0.0.2 modification, 2009.11.25 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
    int scResult;
#endif
/* ======================================== */

    PDEBUG("Enter Cleanup_Module function...\n");

    if (gptDevInfo != NULL)
    {
        if (gsdwMajor != 0)
        {
/* Version 3.0.0.2 modification, 2009.11.25 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
            scResult = unregister_chrdev(gsdwMajor, "vpl_voc");

            if (scResult < 0)
            {
                PDEBUG("Cannot release major number %d !!\n", (int)gsdwMajor);
                return;
            }
#else
            unregister_chrdev(gsdwMajor, "vpl_voc");
#endif
/* ======================================== */
        }

        if (gptDevInfo->pdwProfileClr != NULL)
        {
            iounmap(gptDevInfo->pdwProfileClr);
/* Version 3.0.0.3 modification, 2009.11.27 */
            release_mem_region(VPL_AHBC_2_MMR_BASE+0x108, sizeof(DWORD));
/* ======================================== */
        }

        if (gptDevInfo->pdwBandwidth != NULL)
        {
            iounmap(gptDevInfo->pdwBandwidth);
/* Version 3.0.0.3 modification, 2009.11.27 */
            release_mem_region(VPL_AHBC_2_MMR_BASE+0x10C+((VPL_VOC_MASTER_NUM-1)<<2), sizeof(DWORD));
/* ======================================== */
        }

        if (gptDevInfo->pdwRGInterval != NULL)
        {
            iounmap(gptDevInfo->pdwRGInterval);
/* Version 3.0.0.3 modification, 2009.11.27 */
            release_mem_region(VPL_AHBC_2_MMR_BASE+0x148+((VPL_VOC_MASTER_NUM-1)<<2), sizeof(DWORD));
/* ======================================== */
        }

        if (gptDevInfo->pdwReqTimes != NULL)
        {
            iounmap(gptDevInfo->pdwReqTimes);
/* Version 3.0.0.3 modification, 2009.11.27 */
            release_mem_region(VPL_AHBC_2_MMR_BASE+0x184+((VPL_VOC_MASTER_NUM-1)<<2), sizeof(DWORD));
/* ======================================== */
        }

        if (gptDevInfo->ptMMRInfo != NULL)
        {
            iounmap(gptDevInfo->ptMMRInfo);
            release_mem_region(VPL_VOC_MMR_BASE, sizeof(TVPLVOCInfo));
        }

        kfree(gptDevInfo);
    }

    PDEBUG("Exit Cleanup_Module function !!\n");

    return;
}

/* ======================================================================== */
static int Initial_Module(void)
{
    int scResult;
    DWORD dwVersion;

    PDEBUG("Enter Initial_Module function...\n");

    if ((gptDevInfo=kmalloc(sizeof(TVPLVOCDevInfo), GFP_KERNEL)) == NULL)
    {
        PDEBUG("Allocate device info buffer fail !!\n");
        scResult = -ENOMEM;
        goto FAIL;
    }
    memset(gptDevInfo, 0, sizeof(TVPLVOCDevInfo));

/* Version 3.0.0.4 modification, 2010.01.06 */
    writel(readl(SYSC_CLK_EN_MMR)|(0x1<<VPL_VOC_CLK_EN_NUM), SYSC_CLK_EN_MMR);
/* ======================================== */
    request_mem_region(VPL_VOC_MMR_BASE, sizeof(TVPLVOCInfo), "VPL_VOC");
    gptDevInfo->ptMMRInfo = (TVPLVOCInfo *)ioremap((int)VPL_VOC_MMR_BASE, sizeof(TVPLVOCInfo));
/* Version 3.0.0.4 modification, 2010.01.06 */
    PDEBUG("Remapped base address = 0x%08X\n", (int)gptDevInfo->ptMMRInfo);
/* ======================================== */

    if ((readl(&gptDevInfo->ptMMRInfo->dwVersion)&0xFF000000) == 0)
    {
        printk("Device does not exist !!\n");
        scResult = -ENODEV;
        goto FAIL;
    }

/* Version 3.0.0.3 modification, 2009.11.27 */
    request_mem_region(VPL_AHBC_2_MMR_BASE+0x108, sizeof(DWORD), "VPL_VOC Profile Clear");
    gptDevInfo->pdwProfileClr = (DWORD *)ioremap((int)(VPL_AHBC_2_MMR_BASE+0x108), sizeof(DWORD));

    request_mem_region(VPL_AHBC_2_MMR_BASE+0x10C+((VPL_VOC_MASTER_NUM-1)<<2), sizeof(DWORD), "VPL_VOC Bandwidth");
    gptDevInfo->pdwBandwidth = (DWORD *)ioremap((int)(VPL_AHBC_2_MMR_BASE+0x10C+((VPL_VOC_MASTER_NUM-1)<<2)), sizeof(DWORD));

    request_mem_region(VPL_AHBC_2_MMR_BASE+0x148+((VPL_VOC_MASTER_NUM-1)<<2), sizeof(DWORD), "VPL_VOC Request Grant Interval");
    gptDevInfo->pdwRGInterval = (DWORD *)ioremap((int)(VPL_AHBC_2_MMR_BASE+0x148+((VPL_VOC_MASTER_NUM-1)<<2)), sizeof(DWORD));

    request_mem_region(VPL_AHBC_2_MMR_BASE+0x184+((VPL_VOC_MASTER_NUM-1)<<2), sizeof(DWORD), "VPL_VOC Request Times");
    gptDevInfo->pdwReqTimes = (DWORD *)ioremap((int)(VPL_AHBC_2_MMR_BASE+0x184+((VPL_VOC_MASTER_NUM-1)<<2)), sizeof(DWORD));

    request_mem_region(VPL_AHBC_2_MMR_BASE+0x04, sizeof(DWORD), "VPL AHBC 1 CTRL");
    gptDevInfo->pdwAHBC2Ctrl = (DWORD *)ioremap((int)(VPL_AHBC_2_MMR_BASE+0x04), sizeof(DWORD));
    if (((readl(gptDevInfo->pdwAHBC2Ctrl)>>(16+VPL_VOC_MASTER_NUM))&0x01) != 0x01)
    {
        writel(readl(gptDevInfo->pdwAHBC2Ctrl)|(0x1<<(16+VPL_VOC_MASTER_NUM)), gptDevInfo->pdwAHBC2Ctrl);
    }
    PDEBUG("Priority: 0x%08x !!\n", readl(gptDevInfo->pdwAHBC2Ctrl));
/* ======================================== */


    scResult = register_chrdev(gsdwMajor, "vpl_voc", &vpl_voc_fops);

    if (scResult < 0)
    {
        PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
        goto FAIL;
    }

    if (gsdwMajor == 0)
    {
        gsdwMajor = scResult;
    }

/* Version 6.0.0.0 modification, 2012.10.15 */
/* ======================================== */
    init_waitqueue_head(&gptDevInfo->wq);

    dwVersion = readl(&gptDevInfo->ptMMRInfo->dwVersion);

    printk("Install VPL_VOC device driver version %ld.%ld.%ld.%ld on VPL_VOC hardware version %ld.%ld.%ld.%ld complete !!\n",
           VPL_VOC_VERSION&0xFF,
           (VPL_VOC_VERSION>>8)&0xFF,
           (VPL_VOC_VERSION>>16)&0xFF,
           (VPL_VOC_VERSION>>24)&0xFF,
           (dwVersion>>24)&0xFF,
           (dwVersion>>16)&0xFF,
           (dwVersion>>8)&0xFF,
           dwVersion&0xFF);
/* Version 4.2.0.1 modification, 2011.08.05 */
    if (gsdwClkSrc)
    {
        writel(readl(SYSC_CLK_GEN_MMR)|(0x1<<22), SYSC_CLK_GEN_MMR);
    }
    else
    {
        writel(readl(SYSC_CLK_GEN_MMR)&(~(0x1<<22)), SYSC_CLK_GEN_MMR);
    }
/* ======================================== */

/* Version 3.0.0.4 modification, 2010.01.06 */
    writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VPL_VOC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
/* ======================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
    INIT_LIST_HEAD(&gptDevInfo->queued_list);
    INIT_LIST_HEAD(&gptDevInfo->done_list);
    spin_lock_init(&gptDevInfo->lock);

    gptDevInfo->ptCur_Frm = gptDevInfo->ptNext_Frm = NULL;
/* ======================================== */
    PDEBUG("Exit Initial_Module function !!\n");

    return 0;

FAIL:
/* Version 3.0.0.4 modification, 2010.01.06 */
    writel(readl(SYSC_CLK_EN_MMR)&(~(0x1<<VPL_VOC_CLK_EN_NUM)), SYSC_CLK_EN_MMR);
/* ======================================== */
    Cleanup_Module();
    return scResult;
}

/* ======================================================================== */
module_init(Initial_Module);
module_exit(Cleanup_Module);

/* ======================================================================== */
