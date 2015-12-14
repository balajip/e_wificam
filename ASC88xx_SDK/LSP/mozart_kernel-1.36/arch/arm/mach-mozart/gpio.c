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
#include <asm/irq.h>
#include <mach/io.h>
#include <mach/irq.h>
#include <mach/mozart.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/bitops.h>
#include <mach/gpio.h>
#include <mach/agpo.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/cdev.h>


static DEFINE_SPINLOCK(gpio_lock);
static int gsdwMajor = 0;

static struct class *ptModuleClass;
static struct cdev *gpio_cdev;
typedef struct
{
	unsigned long pindir_hold;
	unsigned long intr_requested;
} GPIO_Dev;
static GPIO_Dev *g_GPIO_devices;    /* This driver instance */
static volatile int g_GPIO_intr_occurred;
wait_queue_head_t gpio_wq[MOZART_N_GPIO];
static volatile unsigned long g_old_jiffies[MOZART_N_GPIO];
static unsigned long g_GPIO_debounce_time = 1;

static DECLARE_BITMAP(gpio_in_use, MOZART_N_GPIO);

/*--------------------------------------------------------------------------*/
void __error_inval_gpio(void)
{
	printk("the number of gpio pins is invalid value\n");
}

/*--------------------------------------------------------------------------*/
int gpio_request(unsigned gpio, const char *tag)
{
	if (gpio >= MOZART_N_GPIO)
		return -EINVAL;

	if (test_and_set_bit(gpio, gpio_in_use))
		return -EBUSY;

	if (gpio < MOZART_N_AGPO)
		agpo_set_direction(gpio, 0);

	return 0;
}
EXPORT_SYMBOL(gpio_request);

/*--------------------------------------------------------------------------*/
void gpio_free(unsigned gpio)
{
	if (gpio >= MOZART_N_GPIO)
		return;

	clear_bit(gpio, gpio_in_use);
}
EXPORT_SYMBOL(gpio_free);

/*--------------------------------------------------------------------------*/
static struct gpio_controller *__iomem gpio2controller(unsigned gpio)
{
	return __gpio_to_controller(gpio);
}

/*
 * Assuming the pin is muxed as a gpio output, set its output value.
 */
void __gpio_set(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g = gpio2controller(gpio);
	u32 mask;

	if (gpio >= MOZART_N_GPIO)
		__error_inval_gpio();

	spin_lock(&gpio_lock);
	mask = __gpio_mask(gpio);
	if (value)
	{
		writel((1<<gpio), &g->data_set);	// pin high
	}
	else
	{
		writel((1<<gpio), &g->data_clr);	// pin low
	}
	spin_unlock(&gpio_lock);
}
EXPORT_SYMBOL(__gpio_set);

/*
 * Read the pin's value (works even if it's set up as output);
 * returns zero/nonzero.
 *
 * Note that changes are synched to the GPIO clock, so reading values back
 * right after you've set them may give old values.
 */
int __gpio_get(unsigned gpio)
{
	struct gpio_controller *__iomem g = gpio2controller(gpio);

	int ret;

	if (gpio >= MOZART_N_GPIO)
		return -EINVAL;

	spin_lock(&gpio_lock);
	ret = readl(&g->pin_dir);
	if (ret & (1<<gpio))
	{
		ret = -EINVAL;
	}
	else
	{
		ret = ((readl(&g->data_in)>>gpio)&1);
	}
	spin_unlock(&gpio_lock);
	return ret;
}
EXPORT_SYMBOL(__gpio_get);

/*--------------------------------------------------------------------------*/
int gpio_direction_input(unsigned gpio)
{
	struct gpio_controller *__iomem g = gpio2controller(gpio);
	u32 temp;

	if (!g)
		return -EINVAL;

	spin_lock(&gpio_lock);
	temp = readl(&g->pin_dir);
	writel(temp&~(1<<gpio), &g->pin_dir);

	spin_unlock(&gpio_lock);
	return 0;
}
EXPORT_SYMBOL(gpio_direction_input);

/*--------------------------------------------------------------------------*/
int gpio_direction_output(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g = gpio2controller(gpio);
	u32 temp;

	if (!g)
		return -EINVAL;

	spin_lock(&gpio_lock);
	temp = readl(&g->pin_dir);
	if (value == 1)
	{
		writel((1<<gpio), &g->data_set);
	}
	else
	{
		writel((1<<gpio), &g->data_clr);
	}
	writel(temp|(1<<gpio), &g->pin_dir);

	spin_unlock(&gpio_lock);
	return 0;
}
EXPORT_SYMBOL(gpio_direction_output);

/*--------------------------------------------------------------------------*/
int gpio_set_direction(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g = gpio2controller(gpio);
	u32 temp;
	if (!g)
		return -EINVAL;
	spin_lock(&gpio_lock);
	temp = readl(&g->pin_dir);
	if (value)
	{
		writel(temp|(1<<gpio), &g->pin_dir);
	}
	else
	{
		writel(temp&~(1<<gpio), &g->pin_dir);
	}

	spin_unlock(&gpio_lock);
	return 0;
}
EXPORT_SYMBOL(gpio_set_direction);

/*--------------------------------------------------------------------------*/
void gpio_set_value(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g;
	u32 ret;
	u32 mask;

	if (gpio >= MOZART_N_GPIO)
		__error_inval_gpio();

	g = __gpio_to_controller(gpio);
	mask = __gpio_mask(gpio);
	spin_lock(&gpio_lock);
	ret = readl(&g->pin_dir);
	writel(ret|(1<<gpio), &g->pin_dir);
	if (value)
	{
		writel((1<<gpio), &g->data_set);
	}
	else
	{
		writel((1<<gpio), &g->data_clr);
	}
	spin_unlock(&gpio_lock);
	return;
}
EXPORT_SYMBOL(gpio_set_value);

/*--------------------------------------------------------------------------*/
void gpio_set_value_u(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g;
	u32 mask;

	if (gpio >= MOZART_N_GPIO)
		__error_inval_gpio();

	g = __gpio_to_controller(gpio);
	mask = __gpio_mask(gpio);
	spin_lock(&gpio_lock);
	if (value)
	{
		writel((1<<gpio), &g->data_set);
	}
	else
	{
		writel((1<<gpio), &g->data_clr);
	}
	spin_unlock(&gpio_lock);
	return;
}

/*--------------------------------------------------------------------------*/
int gpio_get_value(unsigned gpio)
{
	struct gpio_controller *__iomem g;
	u32 ret;

	if (gpio >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(gpio);
	spin_lock(&gpio_lock);
	ret = ((readl(&g->data_in)>>gpio)&1);
	spin_unlock(&gpio_lock);
	return ret;

}
EXPORT_SYMBOL(gpio_get_value);

/*-----------------------------------------------------------------------
 * Function Name: gpio_intr_enable
 */
static int gpio_intr_enable(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g;
	int ret, intr;

	if (gpio >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(gpio);
	ret = readl(&g->pin_dir);
	if (ret & (1<<gpio))
	{
		return -EINVAL;
	}
	else
	{
		intr = readl(&g->intr_enable);
		if (value)
		{
			writel(intr|(1<<gpio), &g->intr_enable);
		}
		else
		{
			writel(intr&~(1<<gpio), &g->intr_enable);
		}
	}

	return 0;

}
/*-----------------------------------------------------------------------
 * Function Name: gpio_intr_mask
 *-----------------------------------------------------------------------*/
static int gpio_intr_mask(unsigned gpio, int value)
{
	struct gpio_controller *__iomem g;
	int ret, intr;

	if (gpio >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(gpio);
	ret = readl(&g->pin_dir);
	if (ret & (1<<gpio))
	{
		return -EINVAL;
	}
	else
	{
		intr = readl(&g->intr_mask);
		if (value)
		{
			writel(intr|(1<<gpio), &g->intr_mask);
		}
		else
		{
			writel(intr&~(1<<gpio), &g->intr_mask);
		}
	}

	return 0;

}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_intr_wait
 *
 * Input:   None
 * Value Returned:  None
 *-----------------------------------------------------------------------*/
static int gpio_intr_wait(int pin)
{
	struct gpio_controller *__iomem g;
	int ret, pindir;

	if (pin >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(pin);
	ret = readl(&g->pin_dir);

	pindir = readl(&g->pin_dir);
	if (pindir & (1<<pin))
	{
		return -EINVAL;
	}
	else {
		/* Version 2.0.0.0 modification, 2007.11.23 */
		do
		{
			interruptible_sleep_on(&gpio_wq[pin]);
			if (signal_pending(current))
				return -ERESTARTSYS;
		} while ((g_GPIO_intr_occurred & (1<<pin)) == 0);
		/* ======================================== */
	}
	return 0;
}

/* Version 2.0.0.0 modification, 2007.11.23 */
/*-----------------------------------------------------------------------
 * Function Name: GPIO_intr_trigger
 *
 * Input:	None
 * Value Returned:	None
 *-----------------------------------------------------------------------*/
static int gpio_intr_trigger(int pin, int trigger)
{
	struct gpio_controller *__iomem g;
	int ret, pindir;

	if (pin >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(pin);
	pindir = readl(&g->pin_dir);
	if (pindir & (1<<pin))
	{
		return -EINVAL;
	}
	else
	{
		ret = readl(&g->intr_trigger_type);
		if (trigger)
		{
			writel(ret|(1<<pin), &g->intr_trigger_type);
		}
		else
		{
			writel(ret&(~(1<<pin)), &g->intr_trigger_type);
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_intr_riseneg
 *
 * Input:	None
 * Value Returned:	None
 *-----------------------------------------------------------------------*/
static int gpio_intr_riseneg(int pin, int riseneg)
{
	struct gpio_controller *__iomem g;
	int ret, pindir;

	if (pin >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(pin);
	pindir = readl(&g->pin_dir);

	if(pindir & (1<<pin))
	{
		return -EINVAL;
	}
	else
	{
		ret = readl(&g->intr_dir);
		if (riseneg)
		{
			writel(ret|(1<<pin), &g->intr_dir);
		}
		else
		{
			writel(ret&(~(1<<pin)), &g->intr_dir);
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_intr_bothedge
 *
 * Input:	None
 * Value Returned:	None
 *-----------------------------------------------------------------------*/
static int gpio_intr_bothedge(int pin, int both)
{
	struct gpio_controller *__iomem g;
	int ret, pindir;

	if (pin >= MOZART_N_GPIO)
		return -EINVAL;

	g = __gpio_to_controller(pin);

	pindir = readl(&g->pin_dir);
	if(pindir & (1<<pin))
	{
		return -EINVAL;
	}
	else
	{
		ret = readl(&g->intr_both);
		if (both)
		{
			writel(ret|(1<<pin), &g->intr_both);
		}
		else
		{
			writel(ret&(~(1<<pin)), &g->intr_both);
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_open
 *
 * Input:   inode
 *      filp    file pointer
 * Value Returned:  0   Success
 *-----------------------------------------------------------------------*/
static int GPIO_open(struct inode * inode, struct file * filp)
{
	int num, major;
	int ret;
	GPIO_Dev *device; /* device information */
	struct gpio_controller *__iomem g;
	g = __gpio_to_controller(0);

	num = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
	device = (GPIO_Dev *)filp->private_data;
	if (!device)
	{
		if (num > MOZART_N_GPIO)
		{
			return -ENODEV;
		}
		device = g_GPIO_devices;
		filp->private_data = device; /* for other methods */
	}
	ret =gpio_request(num,NULL);
	if(ret)
	{
		return ret;
	}

	/* OK to open driver. */
	if(try_module_get(THIS_MODULE) == 0)
	{
		gpio_free(num);
		return -EBUSY;
	}
	/* ======================================== */

	printk("GPIO module open success\n");
	return 0;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_release
 *
 * Input:   inode
 *          filp        file pointer
 * Value Returned:  0   Success
 *-----------------------------------------------------------------------*/
static int GPIO_release (struct inode *inode, struct file *filp)
{
	int num, major, ret;
	GPIO_Dev *device; /* device information */
	struct gpio_controller *__iomem g;
	g = __gpio_to_controller(0);

	num = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
	device = (GPIO_Dev *)filp->private_data;

	if (!(device->pindir_hold & (1<<num))) {
		ret = readl(&g->pin_dir);
		writel(ret&~(1<<num), &g->pin_dir);
		writel((1<<num), &g->data_clr);
	}
	if ((device->intr_requested & (1<<num)))
	{
		ret = readl(&g->intr_enable);
		writel(ret&~(1<<num), &g->intr_enable);
		ret = readl(&g->intr_mask);
		writel(ret&~(1<<num), &g->intr_mask);
		device->intr_requested &= ~(1<<num);
	}

	gpio_free(num);

	/* ======================================== */
	printk("GPIO module close success\n");

	return 0;
}

static int GPIO_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int num, major;
	int err = 0;
	GPIO_Dev *device = filp->private_data;
	if (!device)
	{
		return -ENODEV;
	}
	num = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
	if (num > MOZART_N_GPIO)
	{
		return -ENODEV;
	}

	if (_IOC_TYPE(cmd) != GPIO_MAGIC)
	{
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) > GPIO_MAXNR)
	{
		return -ENOTTY;
	}
	/*-------------------------------------------------------------------
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 *-------------------------------------------------------------------*/

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
		case GPIO_PINDIR_OUT:
			gpio_set_direction(num, 1);
			break;
		case GPIO_PINDIR_IN:
			gpio_set_direction(num, 0);
			break;
		case GPIO_READ_DATAIN:
			put_user(gpio_get_value(num), (unsigned long __user *)arg);
			break;
		case GPIO_DATA_SET:
			gpio_set_value_u(num, 1);
			break;
		case GPIO_DATA_CLEAR:
			gpio_set_value_u(num, 0);
			break;
		case GPIO_INTR_ENABLE:
			device->intr_requested |= 1<<num;
			return gpio_intr_enable(num, 1);
		case GPIO_INTR_DISABLE:
			device->intr_requested &= ~(1<<num);
			return gpio_intr_enable(num, 0);
		case GPIO_INTR_MASK:
			return gpio_intr_mask(num, 1);
		case GPIO_INTR_UNMASK:
			return gpio_intr_mask(num, 0);
		case GPIO_WAIT_INTR:
			return gpio_intr_wait(num);
			/* Version 2.0.0.0 modification, 2007.11.23 */
		case GPIO_PINDIR_LOOSE:
			device->pindir_hold &= ~(1<<num);
			break;
		case GPIO_PINDIR_HOLD:
			device->pindir_hold |= 1<<num;
			break;
		case GPIO_INTR_EDGE:
			return gpio_intr_trigger(num, 0);
		case GPIO_INTR_LEVEL:
			return gpio_intr_trigger(num, 1);
		case GPIO_INTR_HACTIVE:
			return gpio_intr_riseneg(num, 0);
		case GPIO_INTR_LACTIVE:
			return gpio_intr_riseneg(num, 1);
		case GPIO_INTR_SINGLEEDGE:
			return gpio_intr_bothedge(num, 0);
		case GPIO_INTR_BOTHEDGE:
			return gpio_intr_bothedge(num, 1);
			/* ======================================== */

		default:
			return -ENOTTY;
	}

	return err;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_isr
 *
 * Input:   None
 * Value Returned:  None
 *-----------------------------------------------------------------------*/
/* Version 2.0.0.0 modification, 2007.11.23 */
static irqreturn_t GPIO_isr(int irq, void *dev_id)
{
	volatile unsigned int status;
	volatile unsigned long new_jiffies = jiffies;
	/* ======================================== */
	int queue_num;
	struct gpio_controller *__iomem g;
	GPIO_Dev *device;
	device = (GPIO_Dev *) dev_id;
	g = __gpio_to_controller(0);
	status = (readl(&g->intr_mask_state) & device->intr_requested);
	g_GPIO_intr_occurred = status;
	writel(status, &g->intr_clear);
	for (queue_num=0; queue_num<MOZART_N_GPIO; queue_num++)
	{
		if ((g_GPIO_intr_occurred>>queue_num)&0x01)
		{
			/* Version 2.0.0.0 modification, 2007.11.23 */
			if( (new_jiffies - g_old_jiffies[queue_num]) >= g_GPIO_debounce_time)
			{
				wake_up_interruptible(&gpio_wq[queue_num]);
				g_old_jiffies[queue_num] = new_jiffies;
			}
			/* ======================================== */
		}
	}
	return IRQ_HANDLED;
}

/*-------------------------------------------------------*/
static struct file_operations GPIO_fops = {
ioctl:      GPIO_ioctl,
	    open:       GPIO_open,
	    release:    GPIO_release,
};

static void mozart_gpio_exit(void)
{
	dev_t dev;
	free_irq(VPL_GPIOC_IRQ_NUM, g_GPIO_devices);
	if (g_GPIO_devices)
	{
		kfree(g_GPIO_devices);
	}

	if (gsdwMajor!=0)
	{
		dev = MKDEV(gsdwMajor, 0);
		unregister_chrdev_region(dev, 1);
	}

	device_destroy(ptModuleClass, gsdwMajor);
	class_destroy(ptModuleClass);
	cdev_del(gpio_cdev);
}

/*-------------------------------------------------------*/
static int  mozart_gpio_init(void)
{
	/* initial AGPO is disable */
	int ret, i;
	dev_t dev;
	gpio_cdev = cdev_alloc();
	if (gpio_cdev == NULL)
		goto fail_init;
	gpio_cdev->ops = &GPIO_fops;
	if (gsdwMajor != 0)
	{
		dev = MKDEV(gsdwMajor, 0);
		ret = register_chrdev_region(dev, MOZART_N_GPIO, "gpio");
		if (ret < 0)
		{
			goto fail_init;
		}
	}
	else
	{
		ret = alloc_chrdev_region(&dev, 0, MOZART_N_GPIO, "gpio");
		if (ret < 0)
		{
			goto fail_init;
		}
		{
			gsdwMajor = MAJOR(dev); /* dynamic */
		}
	}
	ret = cdev_add(gpio_cdev, dev, MOZART_N_GPIO);
	if (ret < 0) {
		goto fail_init;
	}

	/*******************************************************
	 * Allocate space for each devices.
	 *******************************************************/
	g_GPIO_devices = kmalloc(sizeof(GPIO_Dev), GFP_KERNEL);
	if (!g_GPIO_devices)
	{
		ret = -ENOMEM;
		goto fail_init;
	}
	memset(g_GPIO_devices, 0, sizeof(GPIO_Dev));
	/* Version 2.0.0.0 modification, 2007.11.23 */
	for (i=0; i<MOZART_N_GPIO; i++)
	{
		g_old_jiffies[i] = 0;
	}
	g_GPIO_intr_occurred = 0;

	/* Version 2.0.0.1 modification, 2009.04.21 */
	fLib_SetIntTrig(VPL_GPIOC_IRQ_NUM, LEVEL, H_ACTIVE);
	/* ======================================== */

	ret = request_irq(VPL_GPIOC_IRQ_NUM, GPIO_isr, IRQF_SHARED, "GPIO controller", g_GPIO_devices);
	/* ======================================== */
	if (ret)
	{
		printk("Unable to allocate GPIOC IRQ=0x%X\n", VPL_GPIOC_IRQ_NUM);
		return ret;
	}

	for (i=0; i<MOZART_N_GPIO; i++)
	{
		init_waitqueue_head(&gpio_wq[i]);
	}
	ptModuleClass = class_create(THIS_MODULE, "gpio");
	if (IS_ERR(ptModuleClass)) {
		goto fail_init;
	}
	for (i=0; i<MOZART_N_GPIO; i++)
	{
		device_create(ptModuleClass, NULL, MKDEV(gsdwMajor, i), NULL, "gpio""%d",i);
	}
	return 0;
fail_init:
	mozart_gpio_exit();
	return 0;

}

module_init(mozart_gpio_init);
module_exit(mozart_gpio_exit);
MODULE_DESCRIPTION("Platform-independent bitbanging I2C driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:i2c-gpio");
