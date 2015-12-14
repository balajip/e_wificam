/*
 * Copyright (C) 2012  VN Inc.
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
 * Description: IRCUT Driver for Mozart EVM and RDK
 *
 * $History: ircut.c $
 * 
 * *****************  Version 1.0.0.2  *****************
 * User: SC. FanChiang       Date: 12/10/03   Time: 18:15p
 * 1. CHANGE IRCUT GPIO to 10,11
 * 2. Rename GPIO_IRCUT, UART_IRCUT
 * 
 * *****************  Version 1.0.0.1  *****************
 * User: SC. FanChiang       Date: 12/08/07   Time: 11:38a
 * First Release.
 * 
 *
 */

#include "ircut_locals.h"

const CHAR IRCUT_ID[] = "$Version: "IRCUT_ID_VERSION"  (IRCUT DRIVER) $";

#define MAJOR_NUMBER 100
#define DEVICE_NAME "ircut"

static int IrcutMajor = MAJOR_NUMBER;
static int controlmode = 1;

//static struct cdev *ircut_cdev;
static struct workqueue_struct *queue;
//static struct work_struct *work;

static void IRCUT_Forward(struct work_struct *);
static void IRCUT_Reverse(struct work_struct *);
static void IRCUT_Standby(struct work_struct *);

static DECLARE_WORK(IRCUT_Forward_work,IRCUT_Forward);
static DECLARE_WORK(IRCUT_Reverse_work,IRCUT_Reverse);
static DECLARE_WORK(IRCUT_Standby_work,IRCUT_Standby);

MODULE_AUTHOR("                             >");
MODULE_DESCRIPTION("IRCUT driver");
MODULE_LICENSE("GPL");

static SOCKET GPIO_Open(SOCKET sckNum)
{
	struct gpio_controller *__iomem g = __gpio_to_controller(0);
	SOCKET	sckRet = gpio_request(sckNum, NULL);
	if (sckRet)
	{
		return sckRet;
	}
   
	sckRet = readl(&g->pin_dir);
	writel(sckRet&~(1<<sckNum), &g->pin_dir);
	sckRet = readl(&g->intr_enable);
	writel(sckRet&~(1<<sckNum), &g->intr_enable);
	sckRet = readl(&g->intr_mask);
	writel(sckRet&~(1<<sckNum), &g->intr_mask);
	writel((1<<sckNum), &g->intr_clear);
	
	return 0;
}

static SOCKET GPIO_Release(SOCKET sckNum)
{
	struct gpio_controller *__iomem g = __gpio_to_controller(0);
	SOCKET	sckRet;
       
	sckRet = readl(&g->intr_enable);
	writel(sckRet&~(1<<sckNum), &g->intr_enable);
	sckRet = readl(&g->intr_mask);
	writel(sckRet&~(1<<sckNum), &g->intr_mask);
	sckRet = readl(&g->pin_dir);
	writel(sckRet&~(1<<sckNum), &g->pin_dir);
	
	gpio_free(sckNum);

	return 0;
}

static void IRCUT_SetPin(int pin, int bit)
{
	gpio_set_value(pin, bit);
}

static void UART_SetPin(int pin, int bit)
{
	if (bit == 1)
	{
		set_ugpio_pin(pin);
	}
	else
	{
		clear_ugpio_pin(pin);
	}
}

static void IRCUT_Forward(struct work_struct *ws)
{
	IRCUT_SetPin(IRCUT_ENB,0);
	if (controlmode == TWO_WIRE)
	{
		IRCUT_SetPin(IRCUT_FBC,0);
	}
	mdelay(2000);	
}

static void IRCUT_Reverse(struct work_struct *ws)
{
	IRCUT_SetPin(IRCUT_ENB,0);
	if (controlmode == TWO_WIRE)
	{		
		IRCUT_SetPin(IRCUT_FBC,1);
	}	
	mdelay(2000);
}

static void IRCUT_Standby(struct work_struct *ws)
{
	if (controlmode == TWO_WIRE)
	{
		IRCUT_SetPin(IRCUT_ENB,1);
		//IRCUT_SetPin(IRCUT_FBC,0);
	}
}

static int ircut_dev_release(struct inode *pinode, struct file *pfile)
{
	//printk("%s.\n", __func__);
	GPIO_Release(IRCUT_ENB);
	if (controlmode == TWO_WIRE)
	{	
		GPIO_Release(IRCUT_FBC);
	}
	return 0;
}

static int ircut_dev_open(struct inode *pinode, struct file *pfile)
{	
	//printk("%s. \n", __func__);
	if (GPIO_Open(IRCUT_ENB) == -1 )
	{
		printk("Fail to open GPIO");
		return -ENODEV;
	}
	gpio_set_direction(IRCUT_ENB,1);
	//IRCUT_SetPin(IRCUT_ENB,0);
	
	if (controlmode == TWO_WIRE)
	{
		if (GPIO_Open(IRCUT_FBC) != 0)
		{
			printk("Fail to open GPIO");
			return -ENODEV;		
		}
	}
	
	return 0;
}

static int ircut_dev_ioctl(struct inode *pinode, struct file *pfile, unsigned int cmd, unsigned long dwArg)
{
	//printk("%s.\n", __func__);
	//printk("ioctl cmd = %d.\n", cmd);
	switch (cmd)
	{
	case IRCUT_OPEN:
		//printk("IRCUT_OPEN\n");
		queue_work(queue, &IRCUT_Forward_work);
		break;
	case IRCUT_CLOSE:
		//printk("IRCUT_CLOSE\n");
		queue_work(queue, &IRCUT_Reverse_work);
		break;
	case IRCUT_STANDBY: // Two-wire use.
		//printk("IRCUT_STANDBY\n");
		queue_work(queue, &IRCUT_Standby_work);
		break;
	default:
		break;
	}
	return 0;
}

struct file_operations gpio_ircut_fops = 
{
	open : ircut_dev_open,
	release : ircut_dev_release,
	ioctl : ircut_dev_ioctl,
};

static int ircut_init(void)
{
	int ret = 0;

	printk("IRCUT device driver version %s\n",IRCUT_ID_VERSION);
	printk("%s.\n", __func__);
	printk("Control Mode %d\n",controlmode);
	
	ret = register_chrdev(IrcutMajor, DEVICE_NAME, &gpio_ircut_fops);
	if (ret < 0)
	{
		printk(" can't get major %d\n", (int)IrcutMajor);
	}

	if (IrcutMajor == 0 && controlmode == TWO_WIRE) // Two-wire control. In RDK, it will use UART0-nDTR and GPIO 10
	{
		IrcutMajor = ret;
		printk("get major %d \n", (int)IrcutMajor);
	}
	
	//	Create work queue
	queue = create_singlethread_workqueue("IRCUTControl");
	if (!queue)
	{
		return -ENODEV;
	}	
	//INIT_WORK(IRCUT_Standby_work, IRCUT_Standby_work);
	//schedule_work(IRCUT_Standby_work);	
	
	
	return 0;
}

static void ircut_exit(void)
{
	printk("%s.\n", __func__);
	
	if (IrcutMajor != 0)
	{
		unregister_chrdev(IrcutMajor, DEVICE_NAME);
	}
	
	// Release work queue
	flush_workqueue(queue);
	destroy_workqueue(queue);
	return;
}

module_init(ircut_init);
module_exit(ircut_exit);
module_param(controlmode, int, 0644);
