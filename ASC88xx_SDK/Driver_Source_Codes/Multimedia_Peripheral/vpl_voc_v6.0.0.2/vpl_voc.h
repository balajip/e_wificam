/*
 * $Header: /rd_2/project/Mozart/Components/VOC/Device_Driver/VOC/vpl_voc.h 19    13/05/14 3:25p Vincent $
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
 * $History: vpl_voc.h $
 * 
 * *****************  Version 19  *****************
 * User: Vincent      Date: 13/05/14   Time: 3:25p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 18  *****************
 * User: Vincent      Date: 13/03/22   Time: 11:48a
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION: Replace some directly MMR address value by global
 * macro defined in Kernel. - DONE
 *
 * *****************  Version 17  *****************
 * User: Vincent      Date: 12/10/15   Time: 2:44p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. FEATURE: Implement buffers by linked list instead of array.
 * 2. FEATURE: Add VPL_VOC_IOC_QUEUE_BUF and VPL_VOC_IOC_DEQUEUE_BUF ioctl
 * for
 * dynamically feed display buffer address.
 *
 * *****************  Version 16  *****************
 * User: Vincent      Date: 12/07/10   Time: 8:15p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 15  *****************
 * User: Alan         Date: 12/06/26   Time: 12:12p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Change version 5.1.0.0 to 5.0.0.2 - DONE.
 *
 * *****************  Version 14  *****************
 * User: Bernard      Date: 12/02/07   Time: 7:49p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * Update version from 5.0.0.1 to 5.1.0.0 - Done.
 *
 * *****************  Version 13  *****************
 * User: Evelyn       Date: 12/01/12   Time: 7:58p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 12  *****************
 * User: Shihchun.fanchiang Date: 11/11/04   Time: 1:03p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 12  *****************
 * User: SC. FanChiang Date: 11/11/04   Time:12:57p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION : When output resolution > 720p, VOC slew rate set to high - DONE.
 *
 * *****************  Version 11  *****************
 * User: Evelyn       Date: 11/09/14   Time: 5:07p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 10  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:35p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 1. MODIFICATION: Modify interface with HDMI Tx device
 *
 * *****************  Version 9  *****************
 * User: Vincent      Date: 11/08/05   Time: 5:40p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 8  *****************
 * User: Alan         Date: 11/07/14   Time: 3:17p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * FEATURE: Add 16-bit YUV with separate or embedded SYNC support for HDMI
 * Tx device - DONE.
 *
 * *****************  Version 7  *****************
 * User: Alan         Date: 11/07/14   Time: 2:04p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * FEATURE: Add BT.1120 support for HDMI Tx device - DONE.
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 10/12/31   Time: 11:16a
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 10/09/21   Time: 10:59a
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
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
 * *****************  Version 15  *****************
 * User: Albert.shen  Date: 10/04/28   Time: 7:50p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 14  *****************
 * User: Albert.shen  Date: 10/04/26   Time: 11:07a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 13  *****************
 * User: Albert.shen  Date: 10/03/11   Time: 12:04p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 12  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 1:49p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Add the sturct TVideoOutDevInfo - DONE.
 *
 * *****************  Version 11  *****************
 * User: Albert.shen  Date: 10/01/22   Time: 6:50p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 10  *****************
 * User: Albert.shen  Date: 10/01/06   Time: 11:51a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: To dynamically disable the engine clock when the engine is idle - DONE.
 *
 * *****************  Version 9  *****************
 * User: Albert.shen  Date: 09/11/27   Time: 10:41p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * MODIFICATION: Modify macro VPL_VOC_BUS_NUM and VPL_VOC_MASTER_NUM for Mozart SoC - DONE
 *
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 09/11/25   Time: 2:13p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 09/06/22   Time: 11:45a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 09/06/15   Time: 11:22a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 09/06/15   Time: 11:19a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 11:19a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 08/11/13   Time: 5:00p
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/11/06   Time: 9:51a
 * Updated in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 * 1.MODIFICATION: Update version number from 1.0.0.0 to 1.1.0.0 -DONE.
 * 2. MODIFICATION: Update buffer pointer only when top field - DONE.
 * 3. MODIFICATION: Use status register to decide that which buffer
 * pointer should be updated. -DONE.
 *
 * *****************  Version 1  *****************
 * User: Albert.shen  Date: 08/10/17   Time: 5:32p
 * Created in $/rd_2/project/SoC/Components/VOC/Device_Driver/VOC
 *
 *
 */
/* ============================================================================================== */

/* ============================================================================================== */
#ifndef __VPL_VOC_H__
#define __VPL_VOC_H__

/* ============================================================================================== */
#include <linux/ioctl.h>
#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define VPL_VOC_VERSION MAKEFOURCC(6, 0, 0, 2)
#define VPL_VOC_ID_VERSION "6.0.0.2"

/* ============================================================================================== */
/* Version 3.0.0.3 modification, 2009.11.27 */
#define VPL_VOC_BUS_NUM 2

#define VPL_VOC_MASTER_NUM  0x01
/* ======================================== */

/* ============================================================================================== */
#define VPL_VOC_IOC_MAGIC 243

#define VPL_VOC_IOC_RESET               _IO     (VPL_VOC_IOC_MAGIC, 0)
#define VPL_VOC_IOC_WAIT_COMPLETE       _IO     (VPL_VOC_IOC_MAGIC, 1)
#define VPL_VOC_IOC_WRITE_BUF_INFO      _IOWR   (VPL_VOC_IOC_MAGIC, 2, DWORD)
#define VPL_VOC_IOC_GET_BANDWIDTH       _IOR    (VPL_VOC_IOC_MAGIC, 3, DWORD)
#define VPL_VOC_IOC_GET_RG_INTERVAL     _IOR    (VPL_VOC_IOC_MAGIC, 4, DWORD)
#define VPL_VOC_IOC_GET_REQ_TIMES       _IOR    (VPL_VOC_IOC_MAGIC, 5, DWORD)
#define VPL_VOC_IOC_CLEAR_PROFILE       _IO     (VPL_VOC_IOC_MAGIC, 6)
#define VPL_VOC_IOC_GET_BUF             _IOR    (VPL_VOC_IOC_MAGIC, 7, DWORD)
#define VPL_VOC_IOC_RELEASE_BUF         _IO     (VPL_VOC_IOC_MAGIC, 8)
#define VPL_VOC_IOC_GET_VERSION_NUMBER  _IOR    (VPL_VOC_IOC_MAGIC, 9, DWORD)
#define VPL_VOC_IOC_GET_FRAME_CNT       _IOR    (VPL_VOC_IOC_MAGIC, 10, DWORD)
/* Version 4.0.0.0 modification, 2010.02.25 */
#define VPL_VOC_IOC_START               _IOWR   (VPL_VOC_IOC_MAGIC, 11, DWORD)
#define VPL_VOC_IOC_STOP                _IO     (VPL_VOC_IOC_MAGIC, 12)
/* ======================================== */
/* Version 4.0.0.3 modification, 2010.04.28 */
#define VPL_VOC_IOC_PLL_CONFIG          _IOWR   (VPL_VOC_IOC_MAGIC, 13, DWORD)
/* ======================================== */
/* Version 4.0.0.5 modification, 2010.09.17 */
#define VPL_VOC_IOC_SET_PAL             _IOW    (VPL_VOC_IOC_MAGIC, 14, DWORD)
/* ======================================== */
/* Version 5.0.0.0 modification, 2011.09.14 */
#define VPL_VOC_IOC_CABLE_DETECT        _IOW    (VPL_VOC_IOC_MAGIC, 15, DWORD)
/* Version 6.0.0.0 modification, 2012.10.15 */
#define VPL_VOC_IOC_QUEUE_BUF           _IOW    (VPL_VOC_IOC_MAGIC, 16, DWORD)
#define VPL_VOC_IOC_DEQUEUE_BUF         _IOR    (VPL_VOC_IOC_MAGIC, 17, DWORD)

#define VPL_VOC_IOC_MAX_NUMBER  17
/* ======================================== */
/* ======================================== */
/* Version 6.0.0.0 modification, 2012.10.15 */
#define VIDEO_MAX_FRAME 		32
/* ======================================== */

/* ============================================================================================== */
#define VOC_OUTPUT_RGB24                    0
#define VOC_OUTPUT_INTERLACE_CCIR656        1
/* Version 4.2.0.0 modification, 2011.07.14 */
#define VOC_OUTPUT_PROGRESSIVE_YUV_16BITS   2
/* ======================================== */

#define VOC_INPUT_YUV420                    0
#define VOC_INPUT_YUV422                    1

/* ============================================================================================== */
/* Version 4.0.0.0 modification, 2010.02.26 */
#define VOC_DEVICE_NULL                     0
#define VOC_DEVICE_ADV7391                  1
#define VOC_DEVICE_CAT6612                  2

/* ======================================== */
/* ============================================================================================== */
#define VPL_VOC_VERSION_OFFSET              0x00
#define VPL_VOC_CTRL_OFFSET                 0x04
#define VPL_VOC_STAT_OFFSET                 0x08
/* Version 1.1.0.0 modification, 2008.11.06 */
#define VPL_VOC_Y_BUFF0_ADDR_OFFSET         0x0C
#define VPL_VOC_CB_BUFF0_ADDR_OFFSET        0x10
#define VPL_VOC_CR_BUFF0_ADDR_OFFSET        0x14
#define VPL_VOC_Y_BUFF1_ADDR_OFFSET         0x18
#define VPL_VOC_CB_BUFF1_ADDR_OFFSET        0x1C
#define VPL_VOC_CR_BUFF1_ADDR_OFFSET        0x20
#define VPL_VOC_INSIZE_OFFSET               0x24
#define VPL_VOC_OUTSIZE_OFFSET              0x28
#define VPL_VOC_OUTSIZE_CTRL_OFFSET         0x2C
#define VPL_VOC_HSYNC_CTRL_OFFSET           0x30
#define VPL_VOC_VSYNC_CTRL_OFFSET           0x34
/* Version 2.0.0.0 modification, 2008.11.11 */
#define VPL_VOC_SBC_CTRL_OFFSET             0x38
#define VPL_VOC_YCBCR2RGB_COEFF_0_OFFSET    0x3C
#define VPL_VOC_YCBCR2RGB_COEFF_1_OFFSET    0x40
#define VPL_VOC_YCBCR2RGB_COEFF_2_OFFSET    0x44
/* ======================================== */
/* ======================================== */

/* ============================================================================================== */
typedef struct video_buffer
{
/* Version 6.0.0.0 modification, 2012.10.15 */
    DWORD dwIndex;
/* ======================================== */
    DWORD dwFrameCount;
    BYTE *pbyYFrame;
    BYTE *pbyCbFrame;
    BYTE *pbyCrFrame;
} TVideoBuffer;

/* Version 4.0.0.0 modification, 2010.02.26 */
typedef struct video_output_device_info
{
    DWORD dwVideoOutDev;
    DWORD dwDispWidth;
    DWORD dwDispHeight;
/* Version 4.3.0.0 modification, 2011.08.31 */
    DWORD dwVICID;
/* ======================================== */
    DWORD dwClkFreq;
/* Version 4.3.0.0 modification, 2011.08.31 */
    EVideoSignalFormat eOutFormat;
/* ======================================== */
} TVideoOutDevInfo;
/* ======================================== */

typedef struct vpl_voc_info
{
    DWORD dwVersion;

    /* Bit 31-16: IN_STRIDE, Input video stride.
       Bit 09-09: CBCR_FORMAT, The chroma components format in YUV420 input image format - 0: One field YUV422
                                                                                           1: Two fields YUV420
       Bit 08-08: FIELD_MODE, 0: One interleaved field in SDRAM.
                              1: Two single fields in SDRAM.
       Bit 07-07: IN_FORMAT, input image format - 0: YUV420, 1: YUV422
       Bit 06-06: OUT_FORMAT, output data format - 0: raw RGB24 data with HSYNC/VSYNC/BLANK
                                                   1: interlace CCIR656
       Bit 05-05: BLANK_TRG, blank signal is active high (0) or active low (1).
       Bit 04-04: CLK_POL, output data changes at clock rising edge (0) or falling edge (1).
       Bit 03-03: RST_EN, enable bit for software reset.
       Bit 02-02: OP_EN, enable signal for VOC.
       Bit 01-01: OP_CMPT_ACK_EN, enable bit for VOC operation complete acknowledge.
       Bit 00-00: OP_CMPT_ACK, acknowledge bit when VOC operation completed.
     */
    DWORD dwCtrl;

    /* Bit 03: PRE_LOAD_CMPT, Set when VOC preload operation is complete. When this signal is set, the MMRs of address should be updated.
       Bit 02: FIELD, current output field for CCIR656, 0 for top field and 1 for bottom field.
       Bit 01: AMBA_ERR
       Bit 00: OP_CMPT
     */
    DWORD dwStat;

/* Version 1.1.0.0 modification, 2008.11.06 */
    DWORD dwYBuff0Addr;
    DWORD dwCbBuff0Addr;
    DWORD dwCrBuff0Addr;
    DWORD dwYBuff1Addr;
    DWORD dwCbBuff1Addr;
    DWORD dwCrBuff1Addr;
/* ======================================== */

    /* Bit 31-16: IN_HEIGHT, input image height.
       Bit 15-00: IN_WIDTH, input image width.
     */
    DWORD dwInSize;

    /* Bit 31-16: OUT_HEIGHT, output total height.
       Bit 15-00: OUT_WIDTH, output total width.
     */
    DWORD dwOutSize;

    /* Bit 31-24: OUT_SPH
       Bit 23-20: F1_OVERLAP
       Bit 19-16: F0_OVERLAP
       Bit 15-00: F0_HEIGHT
     */
    DWORD dwOutSizeCtrl;

    /* Bit 31-31: POLARITY, active high (0) or active low (1)
       Bit 23-12: DELAY_END, HSYNC end point delay cycle
       Bit 11-00: DELAY_START, HSYNC start point delay cycle
     */
    DWORD dwHSyncCtrl;

    /* Bit 31-31: POLARITY, active high (0) or active low (1)
       Bit 23-12: DELAY_END, VSYNC end point delay line number
       Bit 11-00: DELAY_START, VSYNC start point delay line number
     */
    DWORD dwVSyncCtrl;

/* Version 2.0.0.0 modification, 2008.11.11 */
    /* Bit 24-16: SATURATION
       Bit 15-08: BRIGHTNESS
       Bit 07-00: CONTRAST (-127~+128)
     */
    DWORD dwSBCCtrl;

    DWORD dwYCbCr2RGBCoeff0; //MMR 11
    DWORD dwYCbCr2RGBCoeff1; //MMR 12
    DWORD dwYCbCr2RGBCoeff2; //MMR 13
/* ======================================== */
} TVPLVOCInfo;

/* ============================================================================================== */
#endif //__VPL_VOC_H__
