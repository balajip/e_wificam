/*
 *
 * iicctrl
 * Driver for IICCtrl.
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>

/* ============================================================================================== */
#include "IICCtrl.h"

/* ============================================================================================== */
const char IICCtrl_ID[] = "$Version: "IICCtrl_ID_VERSION"  (IICCtrl DRIVER) $";

/* ============================================================================================== */
static struct file_operations *g_GPIOI2C_fops = NULL;
static struct file g_GPIOI2C_fp;
extern struct file_operations *GPIOI2C_get_fops(void);

MODULE_AUTHOR ("VN Inc.");
MODULE_LICENSE("GPL");

/* =========================================================================================== */
unsigned long  IICCtrl_QueryMemSize(TIICCtrlInitOptions *ptInitOptions)
{
	return (sizeof(TIICCtrlInfo) + (ptInitOptions->dwMaxDataLength+3)*sizeof(unsigned char));
}

/* ============================================================================================== */
long IICCtrl_ReadReg(void* hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char *pbyRegData)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(hObject);
	struct inode g_GPIOI2C_inode;			
	int ret = 0;

	g_GPIOI2C_inode.i_rdev = MKDEV(243, ptInfo->dwBusNum);
	g_GPIOI2C_fops->open(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);		

	if ((ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA )|(ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA_NOSTOP))
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byRegAddr;
		if (ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA )
		{
			ptInfo->pbyData[2] = IICCTRL_STOP_BIT_ATTACHED;
		}
		else
		{
			ptInfo->pbyData[2] = IICCTRL_NO_STOP_BIT_ATTACHED;
		}

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ptInfo->pbyData[2] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Read GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		*pbyRegData = ptInfo->pbyData[0];
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_SEQUENTIAL_DATA)
	{
		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ptInfo->pbyData[2] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Read GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		*pbyRegData = ptInfo->pbyData[0];
	}
	else
	{
		printk("Unsupport transfer type !!\n");
		goto out;
	}
out:	
	g_GPIOI2C_fops->release(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);	

	return ret;
}
EXPORT_SYMBOL(IICCtrl_ReadReg);

/* ============================================================================================== */
long IICCtrl_WriteReg(void* hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char byRegData)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(hObject);
	struct inode g_GPIOI2C_inode;			
	int ret = 0;

	g_GPIOI2C_inode.i_rdev = MKDEV(243, ptInfo->dwBusNum);
	g_GPIOI2C_fops->open(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);		
	

	if ((ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA ) ||
		(ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA_NOSTOP) ||
		(ptInfo->dwTransType == IICCTRL_TYPE_SEQUENTIAL_DATA))
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byRegAddr;
		ptInfo->pbyData[2] = byRegData;
		ptInfo->pbyData[3] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 2, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;
		}
	}
	else
	{
		printk("Unsupport transfer type !!\n");
		goto out;
	}
out:	
	g_GPIOI2C_fops->release(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);	

	return ret;
}
EXPORT_SYMBOL(IICCtrl_WriteReg);

/* ============================================================================================== */
long IICCtrl_ReadWordReg(void* hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char bySpecRegAddr, unsigned short *pwRegData)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(hObject);
	struct inode g_GPIOI2C_inode;			
	int ret = 0;

	g_GPIOI2C_inode.i_rdev = MKDEV(243, ptInfo->dwBusNum);
	g_GPIOI2C_fops->open(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);		


	if (ptInfo->dwTransType == IICCTRL_TYPE_MICRON_16_BITS)
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byRegAddr;
		ptInfo->pbyData[2] = IICCTRL_NO_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ptInfo->pbyData[2] = IICCTRL_NO_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Read GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		*pwRegData = ptInfo->pbyData[0] << 8;

		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = bySpecRegAddr;
		ptInfo->pbyData[2] = IICCTRL_NO_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ptInfo->pbyData[2] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Read GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		*pwRegData = (*pwRegData) | ptInfo->pbyData[0];
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_MICRON_16_BITS_TYPE_0)
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byRegAddr;
		ptInfo->pbyData[2] = IICCTRL_NO_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail 5!! ret = %d\n", ret);
			goto out;
		}

		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ptInfo->pbyData[2] = IICCTRL_STOP_BIT_ATTACHED;
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2010.06.01 */
		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
/* ========================================= */					
		if (ret < 0)
		{
			printk("Read GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		*pwRegData = (ptInfo->pbyData[0]<<8) | ptInfo->pbyData[1];

	}
	else
	{
		printk("Unsupport transfer type !!\n");
		goto out;
	}

out:	
	g_GPIOI2C_fops->release(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);	

	return ret;
}
EXPORT_SYMBOL(IICCtrl_ReadWordReg);

/* ============================================================================================== */
long IICCtrl_WriteWordReg(void* hObject, unsigned char byDevAddr, unsigned char byRegAddr, unsigned char bySpecRegAddr, unsigned short wRegData)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(hObject);
	struct inode g_GPIOI2C_inode;			
	int ret = 0;

	g_GPIOI2C_inode.i_rdev = MKDEV(243, ptInfo->dwBusNum);
	g_GPIOI2C_fops->open(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);		

	if (ptInfo->dwTransType == IICCTRL_TYPE_MICRON_16_BITS)
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byRegAddr;
		ptInfo->pbyData[2] = wRegData >> 8;
		ptInfo->pbyData[3] = IICCTRL_NO_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 2, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;;
		}

		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = bySpecRegAddr;
		ptInfo->pbyData[2] = wRegData & 0xFF;
		ptInfo->pbyData[3] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 2, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;;
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_MICRON_16_BITS_TYPE_0)
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byRegAddr;
		ptInfo->pbyData[2] = wRegData >> 8;
		ptInfo->pbyData[3] = wRegData & 0xFF;
		ptInfo->pbyData[4] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 3, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail2 !! ret = %d  \n", ret);
			goto out;;
		}
	}
	else
	{
		printk("Unsupport transfer type !!\n");
		goto out;;
	}

out:	
	g_GPIOI2C_fops->release(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);	

	return ret;
}
EXPORT_SYMBOL(IICCtrl_WriteWordReg);

/* ============================================================================================== */
long IICCtrl_ReadBuf(void* hObject, unsigned char byDevAddr, unsigned char byCmdRegAddr, unsigned long  dwLength, unsigned char *pbyData)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(hObject);
	unsigned long  i;
	struct inode g_GPIOI2C_inode;			
	int ret = 0;
    
	if (dwLength > ptInfo->dwMaxDataLength)
	{
		printk("Buffer overflow !!\n");
		return -1;
	}
	g_GPIOI2C_inode.i_rdev = MKDEV(243, ptInfo->dwBusNum);
	g_GPIOI2C_fops->open(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);		
	

	if ((ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA )|(ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA_NOSTOP))
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byCmdRegAddr;
		if (ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA )
		{
			ptInfo->pbyData[2] = IICCTRL_STOP_BIT_ATTACHED;
		}
		else
		{
			ptInfo->pbyData[2] = IICCTRL_NO_STOP_BIT_ATTACHED;
		}
		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 1, 0);
		if (ret < 0)
		{
			printk("Write register address fail !!\n");
			goto out;;
		}

		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Read register data fail !!\n");
			goto out;;
		}

		for (i=0; i<dwLength; i++)
		{
			pbyData[i] = ptInfo->pbyData[i];
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_ADDR_CMD_DATA)
	{
		ptInfo->pbyData[0] = byDevAddr | byCmdRegAddr | 0x01;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Read data fail !!\n");
			goto out;;
		}

		for (i=0; i<dwLength; i++)
		{
			pbyData[i] = ptInfo->pbyData[i];
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_SEQUENTIAL_DATA)
	{
		unsigned long  i;
		ptInfo->pbyData[0] = byDevAddr | 0x01;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Read data fail !!\n");
			goto out;;
		}

		for (i=0; i<dwLength; i++)
		{
			pbyData[i] = ptInfo->pbyData[i];
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_WRITEBUF_FAST_NOACK)
	{
		ptInfo->pbyData[0] = byCmdRegAddr;

		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+1] = pbyData[i];
		}
		ptInfo->pbyData[dwLength+1] = IICCTRL_FAST_NO_ACK;

		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Read data fail !!\n");
			goto out;
		}
		
		for (i=0; i<dwLength; i++)
		{
			pbyData[i] = ptInfo->pbyData[i+1];
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_ADDR_16_BITS_DATA)
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byCmdRegAddr;
		ptInfo->pbyData[2] = pbyData[0];
		ptInfo->pbyData[3] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, 2, 0);
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C address fail !!\n");
			goto out;
		}

		ptInfo->pbyData[0] = byDevAddr | 0x01;
		ptInfo->pbyData[dwLength+1] = IICCTRL_STOP_BIT_ATTACHED;
		ret = g_GPIOI2C_fops->read(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);		
		if (ret < 0)
		{
			printk("Read GPIO_I2C I2C data fail !!\n");
			goto out;
		}

		for (i=0; i<dwLength; i++)
		{
			pbyData[i] = ptInfo->pbyData[i];
		}
	}
	else
	{
		printk("Unsupport transfer type !!\n");
		goto out;;
	}
out:	
	g_GPIOI2C_fops->release(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);	

	return ret;
}
EXPORT_SYMBOL(IICCtrl_ReadBuf);

/* ============================================================================================== */
long IICCtrl_WriteBuf(void* hObject, unsigned char byDevAddr, unsigned char byCmdRegAddr, unsigned long  dwLength, const unsigned char *pbyData)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(hObject);
	unsigned long  i;
	struct inode g_GPIOI2C_inode;		
	int ret = 0;
	
	if (dwLength > ptInfo->dwMaxDataLength)
	{
		printk("Buffer overflow !!\n");
		return -1;
	}
	g_GPIOI2C_inode.i_rdev = MKDEV(243, ptInfo->dwBusNum);
	g_GPIOI2C_fops->open(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);		

	if ((ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA )|(ptInfo->dwTransType == IICCTRL_TYPE_ADDR_DATA_NOSTOP))
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byCmdRegAddr;

		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+2] = pbyData[i];
		}

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength+1, 0); 
		if (ret < 0)
		{
			printk("Write data fail !!\n");
			goto out;;
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_ADDR_CMD_DATA)
	{
		ptInfo->pbyData[0] = byDevAddr | byCmdRegAddr;

		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+1] = pbyData[i];
		}

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Write data fail !!\n");
			goto out;;
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_SEQUENTIAL_DATA)
	{
		unsigned long  i;

		ptInfo->pbyData[0] = byDevAddr;
		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+1] = pbyData[i];
		}

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Write data fail !!\n");
			goto out;;
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_WRITEBUF_NO_ACK)
	{
		unsigned long  i;

		ptInfo->pbyData[0] = byCmdRegAddr;

		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+1] = pbyData[i];
		}

		ptInfo->pbyData[dwLength+1] = IICCTRL_NO_ACK;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Write data fail !!\n");
			goto out;;
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_WRITEBUF_FAST_NOACK)
	{
		ptInfo->pbyData[0] = byCmdRegAddr;

		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+1] = pbyData[i];
		}
		
		ptInfo->pbyData[dwLength+1] = IICCTRL_FAST_NO_ACK;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength, 0);
		if (ret < 0)
		{
			printk("Write data fail !!\n");
			goto out;
		}
	}
	else if (ptInfo->dwTransType == IICCTRL_TYPE_ADDR_16_BITS_DATA)
	{
		ptInfo->pbyData[0] = byDevAddr;
		ptInfo->pbyData[1] = byCmdRegAddr;
		
		for (i=0; i<dwLength; i++)
		{
			ptInfo->pbyData[i+2] = pbyData[i];
		}
		ptInfo->pbyData[dwLength+2] = IICCTRL_STOP_BIT_ATTACHED;

		ret = g_GPIOI2C_fops->write(&g_GPIOI2C_fp, ptInfo->pbyData, dwLength+1, 0); 
		if (ret < 0)
		{
			printk("Write GPIO_I2C I2C data fail !!\n");
			goto out;;
		}
	}
	else
	{
		printk("Unsupport transfer type !!\n");
		goto out;;
	}
out:	
	g_GPIOI2C_fops->release(&(g_GPIOI2C_inode), &g_GPIOI2C_fp);	

	return ret;
}
EXPORT_SYMBOL(IICCtrl_WriteBuf);

/* ============================================================================================== */
long IICCtrl_Initial(void* *phObject, TIICCtrlInitOptions *ptInitOptions)
{
	TIICCtrlInfo *ptInfo;
	unsigned long  dwObjectMemSize;

	dwObjectMemSize = IICCtrl_QueryMemSize(ptInitOptions);

	if ((ptInfo=(TIICCtrlInfo *)kmalloc(dwObjectMemSize, GFP_ATOMIC)) == NULL){
		printk("Allocate IICCtrlInfo buffer fail !!\n");
		return -1;
	}		
	memset(ptInfo, 0, dwObjectMemSize);

	ptInfo->pbyData = (unsigned char *)((unsigned long )ptInfo+sizeof(TIICCtrlInfo));

	*phObject = (void*)ptInfo;

	g_GPIOI2C_fops = GPIOI2C_get_fops(); // for gpio_i2c control	
	
	ptInfo->dwTransType = ptInitOptions->dwTransType;
	ptInfo->dwMaxDataLength = ptInitOptions->dwMaxDataLength;
	ptInfo->dwBusNum = ptInitOptions->dwBusNum;

	return 0;
}
EXPORT_SYMBOL(IICCtrl_Initial);

/* ============================================================================================== */
long IICCtrl_Release(void* *phObject)
{
	TIICCtrlInfo *ptInfo = (TIICCtrlInfo *)(*phObject);

	if (ptInfo != NULL)
	{
		kfree(ptInfo);
	}

	*phObject = (void*)NULL;

	return 0;
}
EXPORT_SYMBOL(IICCtrl_Release);

