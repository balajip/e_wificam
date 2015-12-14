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
 */

#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>
#include <linux/fs.h>       /* everything... */
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <mach/io.h>
#include <mach/irq.h>
#include <mach/mozart.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <mach/gpio.h>
#include <mach/agpo.h>
#include <linux/device.h>
#include <linux/cdev.h>

int agpo_set_direction(unsigned agpo, int value);
static DEFINE_SPINLOCK(agpo_lock);
static int gsdwMajor = 0;
static AGPO_Dev *g_AGPO_devices;
static struct class *ptModuleClass;
static struct cdev *agpo_cdev;

static DECLARE_BITMAP(agpo_in_use, MOZART_N_AGPO);
void __error_inval_agpio(void)
{
	printk("the number of agpo pins is invalid value\n");
}

/*-------------------------------------------------------*/
int agpo_request(unsigned agpo, const char *tag)
{
	int ret;
    if (agpo >= MOZART_N_AGPO)
        return -EINVAL;
	ret = gpio_request(agpo, tag);
	if (ret)
	{
		printk("fail\n");
		return ret;
	}
    if (test_and_set_bit(agpo, agpo_in_use))
        return -EBUSY;
	agpo_set_direction(agpo, 1);// The result is gpio direction pin logic AND agpo output enable pin.

	return 0;
}
EXPORT_SYMBOL(agpo_request);

/*-------------------------------------------------------*/
void agpo_free(unsigned agpo)
{
    if (agpo >= MOZART_N_AGPO)
        return;
	agpo_set_direction(agpo, 0);
    clear_bit(agpo, agpo_in_use);
	gpio_free(agpo);
}
EXPORT_SYMBOL(agpo_free);

/*-------------------------------------------------------*/
static struct agpo_controller *__iomem agpo2controller(unsigned agpo)
{
    return __agpo_to_controller(agpo);
}


/*-------------------------------------------------------*/
/*
 set the pin is output(1) mode  or input(0) mode.
*/
int agpo_set_direction(unsigned agpo, int value)
{
	struct agpo_controller *__iomem g = agpo2controller(agpo);
    u32 temp;
    if (!g)
        return -EINVAL;
    spin_lock(&agpo_lock);
	gpio_set_direction(agpo, value);
    temp = readl(&g->data_oe_n);
	if (value)//out mode enable
	{
		writel(temp&~(1<<agpo), &g->data_oe_n);
		gpio_set_value(agpo, 0);
	}
	else
	{
	    writel(temp|(1<<agpo), &g->data_oe_n);
	}
    spin_unlock(&agpo_lock);
    return 0;
}
EXPORT_SYMBOL(agpo_set_direction);

/*-------------------------------------------------------*/
int agpo_setup(unsigned agpo, AGPO_Info * AGPO_info)
{
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	int offset = agpo*7;
	int err = 0;

    if (!g)
        return -EINVAL;
	if (down_interruptible(&(g_AGPO_devices->mutex)))
	{
		return -EBUSY;
	}
	if (g_AGPO_devices->agpo_status[agpo] & AGPO_STATUS_ENABLE)
	{
		err = -EBUSY;
		goto agpo_setup_fail;
	}

    spin_lock(&agpo_lock);
	writel(AGPO_info->dwPattern_0, &g->pattern_0+offset);
	writel(AGPO_info->dwPattern_1, &g->pattern_1+offset);
	writel(AGPO_info->dwPattern_2, &g->pattern_2+offset);
	writel(AGPO_info->dwPattern_3, &g->pattern_3+offset);
	writel(AGPO_info->dwPeriod, &g->period+offset);
	writel(AGPO_info->dwDataLength, &g->data_length+offset);
	writel(AGPO_info->dwIntvRepeat, &g->interval_repeat+offset);
    spin_unlock(&agpo_lock);
agpo_setup_fail:
	up(&(g_AGPO_devices->mutex));
    return err;
}
EXPORT_SYMBOL(agpo_setup);

/*-------------------------------------------------------*/
int agpo_read_status(unsigned agpo)
{
	unsigned ret;
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	ret = ((readl(&g->status)>>agpo)&1);
	return ret;
}
EXPORT_SYMBOL(agpo_read_status);

/*-------------------------------------------------------*/
int agpo_intr_mask(unsigned agpo, int value)
{
	int intr;
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	intr = readl(&g->intr_mask);
	if (value)
	{
		writel(intr|(1<<agpo), &g->intr_mask);
	}
	else
	{
		writel(intr&~(1<<agpo), &g->intr_mask);
	}
	return 0;
}
EXPORT_SYMBOL(agpo_intr_mask);

/*-------------------------------------------------------*/
int agpo_set_default_val(unsigned agpo, int value)
{
	int def_data_val;
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	def_data_val = readl(&g->def_data_val);
	if (value)
	{
		writel(def_data_val|(1<<agpo), &g->def_data_val);
	}
	else
	{
		writel(def_data_val&(~(1<<agpo)), &g->def_data_val);
	}
	return 0;
}
EXPORT_SYMBOL(agpo_set_default_val);

/*-------------------------------------------------------*/
void agpo_set_mode(unsigned agpo,  AGPO_Info * AGPO_info)
{
	int ctrl_port_sel;
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	int value;
	if (AGPO_info->dwFlag & AGPO_FLAG_I2C_MODE)
	{
			value = 1;
//				agpo_set_default_val(num, 0);
	}
	else
	{
			value = 0;
	}
	ctrl_port_sel = readl(&g->ctrl_port_sel);
	if (value)
	{
		writel(ctrl_port_sel|(1<<agpo), &g->ctrl_port_sel);
	}
	else
	{
		writel(ctrl_port_sel&~(1<<agpo), &g->ctrl_port_sel);
	}
}
EXPORT_SYMBOL(agpo_set_mode);

/*-------------------------------------------------------*/
int agpo_multi_enable(unsigned agpo, unsigned pins, int enable)
{
	int ctrl, err=0, num=agpo, i ;
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	if (down_interruptible(&(g_AGPO_devices->mutex)))
	{
		return 0;
	}
	ctrl = readl(&g->ctrl);
	if (enable)
	{
		for(i=0; i<MOZART_N_AGPO; i++)
		{
			num = (pins & (1<<i));
			if (num)
			{
				if (g_AGPO_devices->agpo_status[i] & AGPO_STATUS_ENABLE)
				{
					err = -EBUSY;
					goto agpo_multi_enable_fail;
				}
				else
				{
					g_AGPO_devices->agpo_status[i] |= AGPO_STATUS_ENABLE;
				}
			}
		}
		writel(ctrl|pins, &g->ctrl);
	}
	else
	{
		for(i=0; i<MOZART_N_AGPO; i++)
		{
			num = (pins & (1<<i));
			if (num)
			{
				if ((g_AGPO_devices->agpo_status[i] & AGPO_STATUS_ENABLE)==AGPO_STATUS_ENABLE)
				{
					g_AGPO_devices->agpo_status[i] &= ~AGPO_STATUS_ENABLE;
				}
			}
		}
		writel(ctrl&~(pins), &g->ctrl);
	}
agpo_multi_enable_fail:
	up(&(g_AGPO_devices->mutex));
	return err;

}
EXPORT_SYMBOL(agpo_multi_enable);

/*-------------------------------------------------------*/
int agpo_enable(unsigned agpo, int value)
{
	int ctrl, err=0, num=agpo;
	struct agpo_controller *__iomem g = agpo2controller(agpo);
	if (down_interruptible(&(g_AGPO_devices->mutex)))
	{
		return 0;
	}
	ctrl = readl(&g->ctrl);
	if (value)
	{
		if (g_AGPO_devices->agpo_status[num] & AGPO_STATUS_ENABLE)
		{
			err = -EBUSY;
		}
		else
		{
			g_AGPO_devices->agpo_status[num] |= AGPO_STATUS_ENABLE;
		}
		if(err==0)
		writel(ctrl|(1<<agpo), &g->ctrl);
	}
	else
	{
		if ((g_AGPO_devices->agpo_status[num] & AGPO_STATUS_ENABLE)==AGPO_STATUS_ENABLE)
		{
			g_AGPO_devices->agpo_status[num] &= ~AGPO_STATUS_ENABLE;
			writel(ctrl&~(1<<agpo), &g->ctrl);
		}
	}
	up(&(g_AGPO_devices->mutex));
	return err;
}
EXPORT_SYMBOL(agpo_enable);

/*-------------------------------------------------------*/
irqreturn_t AGPO_isr(int _irq, void *_dev)
{
	int retval=0;
	return IRQ_RETVAL(retval);
}

/*-----------------------------------------------------------------------
 * Function Name: AGPO_open
 *
 * Input:   inode
 *      filp    file pointer
 * Value Returned:  0   Success
 *-----------------------------------------------------------------------*/
static int AGPO_open(struct inode * inode, struct file * filp)
{
    int num, major;
    int ret;

    num = MINOR(inode->i_rdev);
    major = MAJOR(inode->i_rdev);
	ret =agpo_request(num,NULL);
	if(ret)
	{
		return ret;
    }

    /* OK to open driver. */
	if(try_module_get(THIS_MODULE) == 0)
    {
        return -EBUSY;
    }
/* ======================================== */
    return 0;
}

/*-----------------------------------------------------------------------
 * Function Name: AGPO_release
 *
 * Input:   inode
 *          filp        file pointer
 * Value Returned:  0   Success
 *-----------------------------------------------------------------------*/
static int AGPO_release (struct inode *inode, struct file *filp)
{
    int num, major;
    num = MINOR(inode->i_rdev);
    major = MAJOR(inode->i_rdev);
    agpo_free(num);
/* ======================================== */
    return 0;
}

/*-------------------------------------------------------*/
static int AGPO_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int num, major, err, value;
	AGPO_Info  AGPO_info;

    num = MINOR(inode->i_rdev);
    major = MAJOR(inode->i_rdev);
	err = 0;
    if (num > MOZART_N_AGPO)
    {
        return -ENODEV;
    }

    if (_IOC_TYPE(cmd) != AGPO_MAGIC)
    {
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > AGPO_MAXNR)
    {
        return -ENOTTY;
    }

	if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    }

    if (err)
    {
        return -EFAULT;
    }

    switch(cmd)
    {
        case AGPO_ENABLE:
			value = (1<<num);
			err = agpo_multi_enable(num, value, 1);
            break;
		case AGPO_DISABLE:
			value = (1<<num);
			err = agpo_multi_enable(num, value, 0);
            break;
		case AGPO_MULTI_ENABLE:
			get_user(value, (unsigned long __user *)arg);
			err = agpo_multi_enable(num, value, 1);
			break;
		case AGPO_MULTI_DISABLE:
			get_user(value, (unsigned long __user *)arg);
			err = agpo_multi_enable(num, value, 0);
			break;
		case AGPO_READ_STATUS:
			value = agpo_read_status(num);
			put_user(value, (unsigned long __user *)arg);
			break;
		case AGPO_INTR_MASK:
			agpo_intr_mask(num, 1);
			break;
		case AGPO_INTR_UNMASK:
			agpo_intr_mask(num, 0);
			break;
		case AGPO_OUTPUT_ENABLE:
			agpo_set_direction(num, 1);//output mode
			break;
		case AGPO_OUTPUT_DISABLE:
			agpo_set_direction(num, 0);//input mode
			break;
		case AGPO_DEFAULT_VAL:
			get_user(value, (unsigned long __user *)arg);
			agpo_set_default_val(num, value);
			break;
		case AGPO_MODE:
			copy_from_user(&AGPO_info, (void*)arg, sizeof(AGPO_Info));
			agpo_set_mode(num, &AGPO_info);
			break;
		case AGPO_SETUP_PATTERN:
			copy_from_user(&AGPO_info, (void*)arg, sizeof(AGPO_Info));
			err = agpo_setup(num, &AGPO_info);
			break;
		case AGPO_SETUP_SEND:
			copy_from_user(&AGPO_info, (void*)arg, sizeof(AGPO_Info));
			err = agpo_enable(num, 0);
			if(err==0)
			{
				agpo_setup(num, &AGPO_info);
				err = agpo_enable(num, 1);
			}
		default:
			break;
	}
    return err;
}

/*-------------------------------------------------------*/
static struct file_operations AGPO_fops = {
    ioctl:      AGPO_ioctl,
    open:       AGPO_open,
    release:    AGPO_release,
};

/*-------------------------------------------------------*/
static void mozart_agpo_exit(void)
{

	dev_t dev;
	free_irq(VPL_AGPOC_IRQ_NUM, agpo_cdev);
	if (gsdwMajor!=0)
	{
		dev = MKDEV(gsdwMajor, 0);
        unregister_chrdev_region(dev, MOZART_N_AGPO);
	}
	device_destroy(ptModuleClass, gsdwMajor);
	class_destroy(ptModuleClass);
    cdev_del(agpo_cdev);

}

/*-------------------------------------------------------*/
static int  mozart_agpo_init(void)
{
	/* initial AGPO is disable */

	int ret, i;
    dev_t dev;
	agpo_cdev = cdev_alloc();
	if (agpo_cdev == NULL)
		goto fail_init;
	agpo_cdev->ops = &AGPO_fops;
	if (gsdwMajor != 0)
	{
		dev = MKDEV(gsdwMajor, 0);
		ret = register_chrdev_region(dev, MOZART_N_AGPO, "agpo");
		if (ret < 0)
	    {
    	    goto fail_init;
		}
	}
	else
	{
		ret = alloc_chrdev_region(&dev, 0, MOZART_N_AGPO, "agpo");
		if (ret < 0)
	    {
    	    goto fail_init;
		}
	    gsdwMajor = MAJOR(dev); /* dynamic */
	}
    ret = cdev_add(agpo_cdev, dev, MOZART_N_AGPO);

    ret = request_irq(VPL_AGPOC_IRQ_NUM, &AGPO_isr, IRQF_SHARED, "AGPO controller", agpo_cdev);
    if (ret)
    {
        printk("Unable to allocate AGPOC IRQ=0x%X\n", VPL_AGPOC_IRQ_NUM);
        return ret;
    }
	g_AGPO_devices = kmalloc(sizeof(AGPO_Dev), GFP_KERNEL);
	if (!g_AGPO_devices)
    {
        ret = -ENOMEM;
        goto fail_init;
    }
    memset(g_AGPO_devices, 0, sizeof(AGPO_Dev));
	sema_init(&g_AGPO_devices->mutex, 1);
	for (i=0; i<MOZART_N_AGPO ; i++)
	{
		g_AGPO_devices->agpo_status[i] = AGPO_STATUS_IDLE;
	}
	ptModuleClass = class_create(THIS_MODULE, "agpo");
	if (IS_ERR(ptModuleClass)) {
		goto fail_init;
	}
    for (i=0; i<MOZART_N_AGPO; i++)
	{
		device_create(ptModuleClass, NULL, MKDEV(gsdwMajor, i), NULL, "agpo""%d",i);
	}

	return 0;
fail_init:
	mozart_agpo_exit();
	return 0;

}

/*-------------------------------------------------------*/
module_init(mozart_agpo_init);
module_exit(mozart_agpo_exit);
MODULE_DESCRIPTION("Platform-dependent AGPO driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:agpo");
