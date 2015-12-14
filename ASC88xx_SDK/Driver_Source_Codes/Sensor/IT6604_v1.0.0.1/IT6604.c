/*
 * IT6604
 * Driver for IT6604.
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/workqueue.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "IT6604.h"
#include "IT6604_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR IT6604_ID[] = "$Version: "IT6604_ID_VERSION"  (IT6604 DRIVER) $";
/*==================================================================*/
static TIT6604Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore IT6604_mutex;

MODULE_AUTHOR("______ Inc.");
MODULE_DESCRIPTION("IT6604 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TIT6604RegAddrData atIT6604regTbl[IT6604_TBL_SIZE] =
{
    {_1A_MiscCtrl                   , 0x14},
    {_1B_VideoMap                   , 0x20},
    {_1C_VideoCtrl1                 , 0x40},
    {_1D_VclkCtrl                   , 0x30},
    {_08_VIOCtrl                    , 0xAB},
    {_20_CSCCtrl                    , 0x00},
    {_3C_PGCtrl1                    , 0x00},
    {_3D_PGCtrl2                    , 0x40},
};

const TIT6604RegAddrData atCSCOffset_16_235[] =
{
    {0x21                           ,0x00},
    {0x22                           ,0x80},
    {0x23                           ,0x00},
};

const TIT6604RegAddrData atCSC_RGB2YUV_ITU601_16_235[] =
{
    {0x24                           ,0xB2},
    {0x25                           ,0x04},
    {0x26                           ,0x64},
    {0x27                           ,0x02},
    {0x28                           ,0xE9},
    {0x29                           ,0x00},
    {0x2a                           ,0x93},
    {0x2b                           ,0x3C},
    {0x2c                           ,0x18},
    {0x2d                           ,0x04},
    {0x2e                           ,0x56},
    {0x2f                           ,0x3F},
    {0x30                           ,0x49},
    {0x31                           ,0x3D},
    {0x32                           ,0x9F},
    {0x33                           ,0x3E},
    {0x34                           ,0x18},
    {0x35                           ,0x04},
};

const TIT6604RegAddrData atCSC_RGB2YUV_ITU709_16_235[] =
{
    {0x24                           ,0xB8},
    {0x25                           ,0x05},
    {0x26                           ,0xB4},
    {0x27                           ,0x01},
    {0x28                           ,0x93},
    {0x29                           ,0x00},
    {0x2a                           ,0x49},
    {0x2b                           ,0x3C},
    {0x2c                           ,0x18},
    {0x2d                           ,0x04},
    {0x2e                           ,0x9F},
    {0x2f                           ,0x3F},
    {0x30                           ,0xD9},
    {0x31                           ,0x3C},
    {0x32                           ,0x10},
    {0x33                           ,0x3F},
    {0x34                           ,0x18},
    {0x35                           ,0x04},
};
/*==================================================================*/
static int IT6604_WriteReg(TIT6604Info *ptInfo, EIT6604Regs eRegAddr, DWORD dwData)
{
    return ptInfo->pfnWriteReg(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, eRegAddr, dwData&0xFF);
}

/*-------------------------------------------------------------------------------------*/
static DWORD IT6604_ReadReg(TIT6604Info *ptInfo, EIT6604Regs eRegAddr)
{
    DWORD dwData;
    ptInfo->pfnReadReg(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, eRegAddr, &dwData);
    return (dwData & 0xFF);
}

/*-------------------------------------------------------------------------------------*/
static void IT6604_Reset(TIT6604Info *ptInfo)
{
    DWORD i;

    IT6604_WriteReg(ptInfo, _05_RstCtrl, 0x02);
    IT6604_WriteReg(ptInfo, _05_RstCtrl, 0x00);

    IT6604_WriteReg(ptInfo, _16_Int0Mask, 0x00);
    IT6604_WriteReg(ptInfo, _17_Int1Mask, 0x00);
    IT6604_WriteReg(ptInfo, _18_Int2Mask, 0x00);

    for (i=0; i<IT6604_TBL_SIZE; i++)
    {
        IT6604_WriteReg(ptInfo, atIT6604regTbl[i].eRegAddr, atIT6604regTbl[i].dwData);
    }

    IT6604_WriteReg(ptInfo, _07_PwdCtrl1, 0x0C);
    ptInfo->eVideoState = VSTATE_SWReset;
    ptInfo->dwAVI_DB1 = 0;
    ptInfo->dwAVI_DB2 = 0;
}

static void IT6604_clear_int(TIT6604Info *ptInfo, DWORD flag)
{
    DWORD dwData;
    dwData = IT6604_ReadReg(ptInfo, _19_IntClr);
    dwData |= flag;
    IT6604_WriteReg(ptInfo, _19_IntClr, dwData);
    msleep(1);
    dwData &= (~flag);
    IT6604_WriteReg(ptInfo, _19_IntClr, dwData);
    msleep(1);
}

/*-------------------------------------------------------------------------------------*/
static void IT6604_SetNewAVI(TIT6604Info *ptInfo, DWORD dwAVI_DB1, DWORD dwAVI_DB2)
{
    EPixelFormatFlags eInputVideoMode;
    EColorimetry eInputColorimetry;
    DWORD dwCSC;
    DWORD dwFilter = VIDEO_CTRL_SYNCEMBED;
    int i;
    switch ((dwAVI_DB1>>5)&0x3)
    {
        case 0:
            eInputVideoMode = RGB24;
            break;
        case 1:
            eInputVideoMode = YUV422;
            break;
        case 2:
            eInputVideoMode = YUV444;
            break;
        default:
            eInputVideoMode = RGB24;
    }

    switch ((dwAVI_DB2 >> 6) & 0x3)
    {
        case 0:
            eInputColorimetry = ITU601;
            break;
        case 1:
            eInputColorimetry = ITU709;
            break;
    }

    switch(eInputVideoMode)
    {
        case YUV444:
            dwFilter |= VIDEO_CTRL_UDFILTER;
            dwCSC = CSC_CTRL_BYPASS;
            break;
        case YUV422:
            dwFilter |= VIDEO_CTRL_UDFILTER | VIDEO_CTRL_DNFREEGO;
            dwCSC = CSC_CTRL_BYPASS;
            break;
        case RGB24:
    default:
            dwFilter |= VIDEO_CTRL_UDFILTER;
            dwCSC = CSC_CTRL_RGB2YUV;
            break;
    }

    if (dwCSC == CSC_CTRL_RGB2YUV)
    {
        for (i=0; i<ARRAY_SIZE(atCSCOffset_16_235); i++)
        {
            IT6604_WriteReg(ptInfo, atCSCOffset_16_235[i].eRegAddr, atCSCOffset_16_235[i].dwData);
        }

        if (eInputColorimetry == ITU601)
        {
            for (i=0; i<ARRAY_SIZE(atCSC_RGB2YUV_ITU601_16_235); i++)
            {
                IT6604_WriteReg(ptInfo, atCSC_RGB2YUV_ITU601_16_235[i].eRegAddr, atCSC_RGB2YUV_ITU601_16_235[i].dwData);
            }
        }
        else
        {
            for (i=0; i<ARRAY_SIZE(atCSC_RGB2YUV_ITU709_16_235); i++)
            {
                IT6604_WriteReg(ptInfo, atCSC_RGB2YUV_ITU709_16_235[i].eRegAddr, atCSC_RGB2YUV_ITU709_16_235[i].dwData);
            }
        }

    }

    IT6604_WriteReg(ptInfo, _20_CSCCtrl, dwCSC);
    IT6604_WriteReg(ptInfo, _1C_VideoCtrl1, dwFilter);

}

/*-------------------------------------------------------------------------------------*/
static void IT6604_detect_change(struct work_struct *work)
{
    struct delayed_work *dwork = container_of(work, struct delayed_work, work);
    TIT6604Info *ptInfo = container_of(dwork, TIT6604Info, state_polling);
    DWORD dwSysState;
    DWORD dwInt0;
    DWORD dwInt1;
    DWORD dwAVI_DB1, dwAVI_DB2;
    dwSysState = IT6604_ReadReg(ptInfo, _10_SysState);
    dwInt0 = IT6604_ReadReg(ptInfo, _13_Int0);
    dwInt1 = IT6604_ReadReg(ptInfo, _14_Int1);

    /*
     * There is chance that multiple interrupt indicated in the states.
     * ex, when fast plug/unplug HDMI cable, both PWR5VOFF and PWR5VON are set.
     * Only handle one status and exit, then use state machine to follw up signal check.
     */
    if (dwInt0)
    {
        IT6604_clear_int(ptInfo, INT_CLR_MODE);

        if (dwInt0 & INT0_PWR5V0FF)
        {
            IT6604_Reset(ptInfo);
            goto queue_out;

        }

        if ((dwInt0 & INT0_PWR5VON) & (dwSysState & SYS_STATE_PWR5V_DET))
        {
            if (ptInfo->eVideoState == VSTATE_PwrOff)
            {
                ptInfo->eVideoState = VSTATE_SyncWait;
            }
        }

        if (ptInfo->eVideoState != VSTATE_PwrOff)
        {
            if (dwInt0 & INT0_SCDTOFF)
            {
                ptInfo->eVideoState = VSTATE_SyncWait;
            }

           if (dwInt0 & INT0_SCDTON)
            {
                ptInfo->eVideoState = VSTATE_SyncChecking;
            }
        }

        if (dwInt0 & INT0_VIDMODE_CHG)
        {
            if (ptInfo->eVideoState == VSTATE_VideoOn)
            {
                ptInfo->eVideoState = VSTATE_SyncChecking;
                ptInfo->bForceNewAVIInfo = TRUE;
            }
        }

    }

    if (dwInt1)
    {
        IT6604_clear_int(ptInfo, INT_CLR_PKT | INT_CLR_LEFTMUTE | INT_CLR_SETMUTE);
        if(dwInt1 & INT1_NEWAVIPKT_DET)
        {
            ptInfo->bForceNewAVIInfo = TRUE;
        }
    }

    switch(ptInfo->eVideoState)
    {
        case VSTATE_SWReset:
            IT6604_WriteReg(ptInfo, _07_PwdCtrl1, 0x00);
            ptInfo->eVideoState = VSTATE_PwrOff;
            break;
        case VSTATE_PwrOff:
            if (dwSysState & SYS_STATE_PWR5V_DET)
            {
                ptInfo->eVideoState = VSTATE_SyncWait;
            }
        break;
        case VSTATE_SyncWait:
        case VSTATE_SyncChecking:
            if (dwSysState & SYS_STATE_SYNC_DONE)
            {
                ptInfo->eVideoState = VSTATE_VideoOn;
            }
            break;
        case VSTATE_VideoOn:
            dwAVI_DB1 = IT6604_ReadReg(ptInfo, _AE_AVIDB1);
            dwAVI_DB2 = IT6604_ReadReg(ptInfo, _AF_AVIDB2);
            if ((ptInfo->dwAVI_DB1 != dwAVI_DB1) || (ptInfo->dwAVI_DB2 != dwAVI_DB2) || ptInfo->bForceNewAVIInfo)
            {
                IT6604_SetNewAVI(ptInfo, dwAVI_DB1, dwAVI_DB2);
                ptInfo->dwAVI_DB1 = dwAVI_DB1;
                ptInfo->dwAVI_DB2 = dwAVI_DB2;
                ptInfo->bForceNewAVIInfo = FALSE;
            }
            break;
        default:
            break;
    }

queue_out:
    queue_delayed_work(ptInfo->work_queues, &ptInfo->state_polling, HZ);
}

/*==================================================================*/
int  IT6604_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
    int ret = 0;

    if (down_interruptible(&IT6604_mutex))
    {
        return -ERESTARTSYS;
    }
    switch (ptIoctlArg->eOptionFlags)
    {
        case VIDEO_SIGNAL_OPTION_RESET:
        IT6604_Reset(ptInfo);
        break;
        default:
        ret = -EPERM;
    }
    up(&IT6604_mutex);

    return ret;
}

/*-------------------------------------------------------------------------------------*/
void IT6604_Release(DWORD dwDevNum)
{
    IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
    cancel_delayed_work(&ptInfo->state_polling);
    destroy_workqueue(ptInfo->work_queues);
    kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
int IT6604_Open(TVideoSensorInitialParam* ptIT6604InitialParam, DWORD dwDevNum)
{
    TIICCtrlInitOptions tIICCtrlInitOptions;
    void* hIICCtrlObject;

    if (((ptIT6604InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(IT6604_INTERFACE_VERSION&0x00FF00FF)) ||
        ((ptIT6604InitialParam->dwVideoSensorVersion&0x0000FF00)>(IT6604_INTERFACE_VERSION&0x0000FF00)))
    {
        printk("Invalid IT6604 device driver version %d.%d.%d.%d !!\n",
                (int)(IT6604_INTERFACE_VERSION&0xFF),
                (int)((IT6604_INTERFACE_VERSION>>8)&0xFF),
                (int)((IT6604_INTERFACE_VERSION>>16)&0xFF),
                (int)(IT6604_INTERFACE_VERSION>>24)&0xFF);
        return S_INVALID_VERSION;
    }

    if (!(ptInfo = (TIT6604Info *)kmalloc(sizeof(TIT6604Info), GFP_KERNEL)))
    {
        printk("[IT6604.ko] : Allocate %d bytes memory fail\n", sizeof(TIT6604Info));
        return -ENOMEM;
    }

    // init iic
    tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_DATA;
    tIICCtrlInitOptions.dwMaxDataLength = 3;
    tIICCtrlInitOptions.dwBusNum = 0;
    if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
    {
        printk("[IT6604.ko] : Initial IICCtrl object fail !!\n");
        IT6604_Release(dwDevNum);
        return -ENODEV;
    }

    // init private data
    ptInfo->dwDeviceAddr = IT6604_DEAFULT_DEVICE_ADDR;
    ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
    ptInfo->pfnWriteReg = (FOnWriteReg_t)IICCtrl_WriteReg;
    ptInfo->pfnReadReg = (FOnReadReg_t)IICCtrl_ReadReg;

    // Reset device
    IT6604_Reset(ptInfo);

    // work queues
    ptInfo->work_queues = create_singlethread_workqueue("IT6604");
    if (!ptInfo->work_queues) {
        printk("[IT6604.ko] : Could not create work queue!!\n");
        IT6604_Release(dwDevNum);
        return -ENOMEM;
    }

    INIT_DELAYED_WORK(&ptInfo->state_polling, IT6604_detect_change);

    queue_delayed_work(ptInfo->work_queues, &ptInfo->state_polling, HZ);

    return 0;
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
    .open =     IT6604_Open,
    .release =  IT6604_Release,
    .ioctl =    IT6604_Ioctl,
    .get_shutter_value = NULL,
    .get_remaining_line_num = NULL,
    .group_access = NULL,
    .get_exp_statistic = NULL,
    .get_wb_statistic = NULL,
    .get_max_gain = NULL,
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct IT6604_attribute {
    struct attribute attr;
    ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t IT6604_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    struct IT6604_attribute *IT6604_attr = container_of(attr, struct IT6604_attribute, attr);
    int result = 0;

    if (IT6604_attr->show)
        result = IT6604_attr->show(kobj, buf);
    return result;
}

ssize_t IT6604_SensorName_attr_show(struct kobject *kobj, char *buf)
{
    int retval;
    retval = sprintf (buf, "%s\n", "IT6604");
    return retval;
}

ssize_t IT6604_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
    int retval;
    retval = sprintf (buf, "%s\n", "5");
    return retval;
}

ssize_t IT6604_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
    int retval;
    retval = sprintf (buf, "%s\n", "1920");
    return retval;
}

ssize_t IT6604_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
    int retval;
    retval = sprintf (buf, "%s\n", "1080");
    return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject IT6604_kobj;

struct IT6604_attribute IT6604_SensorName_attr = {
    .attr = {
        .name = "SensorName",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = IT6604_SensorName_attr_show,
};

struct IT6604_attribute IT6604_dwBufNum_attr = {
    .attr = {
        .name = "dwBufNum",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = IT6604_dwBufNum_attr_show,
};

struct IT6604_attribute IT6604_MaxFrameWidth_attr = {
    .attr = {
        .name = "MaxFrameWidth",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = IT6604_MaxFrameWidth_attr_show,
};

struct IT6604_attribute IT6604_MaxFrameHeight_attr = {
    .attr = {
        .name = "MaxFrameHeight",
        .mode = S_IRUGO | S_IWUSR,
    },
    .show = IT6604_MaxFrameHeight_attr_show,
};

struct sysfs_ops IT6604_sysfs_ops = {
    .show   = IT6604_default_attr_show,
};

struct kobj_type IT6604_ktype = {
    .sysfs_ops  = &IT6604_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int IT6604_Init(void)
{
    int result = 0;

    if (result < 0){
        printk("[IT6604] : kobj set name fail!\n");
        return -1;
    }

    IT6604_kobj.ktype = &IT6604_ktype;
    result = kobject_init_and_add(&IT6604_kobj, &IT6604_ktype, NULL, "VideoSensorInfo");

    if (result < 0) {
        printk("[IT6604] : Cannot register kobject [IT6604_kobj].\n");
        return -1;
    }

    result = sysfs_create_file(&IT6604_kobj, &IT6604_SensorName_attr.attr);
       if (result < 0) {
        printk("[IT6604] : Cannot create IT6604_SensorName_attr.attr.\n");
        return -1;
       }

    result = sysfs_create_file(&IT6604_kobj, &IT6604_dwBufNum_attr.attr);
       if (result < 0) {
        printk("[IT6604] : Cannot create IT6604_dwBufNum_attr.attr.\n");
        return -1;
    }

    result = sysfs_create_file(&IT6604_kobj, &IT6604_MaxFrameWidth_attr.attr);
       if (result < 0) {
        printk("[IT6604] : Cannot create IT6604_MaxFrameWidth_attr.attr.\n");
        return -1;
    }

    result = sysfs_create_file(&IT6604_kobj, &IT6604_MaxFrameHeight_attr.attr);
       if (result < 0) {
        printk("[IT6604] : Cannot create IT6604_MaxFrameHeight_attr.attr.\n");
        return -1;
    }
    sema_init(&IT6604_mutex, 1);

    return 0;
}

/*-------------------------------------------------------------------------------------*/
static void IT6604_Exit(void)
{
    sysfs_remove_file(&IT6604_kobj, &(IT6604_SensorName_attr.attr));
    sysfs_remove_file(&IT6604_kobj, &(IT6604_dwBufNum_attr.attr));
    sysfs_remove_file(&IT6604_kobj, &(IT6604_MaxFrameWidth_attr.attr));
    sysfs_remove_file(&IT6604_kobj, &(IT6604_MaxFrameHeight_attr.attr));
    kobject_put(&IT6604_kobj);
    printk(KERN_DEBUG "[IT6604] : bye\n");
    return;
}

/*-------------------------------------------------------------------------------------*/
module_init(IT6604_Init);
module_exit(IT6604_Exit);
