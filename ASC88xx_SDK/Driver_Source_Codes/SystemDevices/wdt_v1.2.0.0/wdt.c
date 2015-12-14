/*
 * wdt
 * Driver for watchdog timer.
 *
 * Copyright (C) 2007-2011  VN Inc.
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
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <mach/irq.h>

#include "wdt_local.h"

#define IRQ_WATCHDOG WDTC_IRQ_NUM

#define WDT_MARGIN       30
#define WDT_MAJOR_DEFAULT 130

#ifndef WDT_MAJOR_DEFAULT
#define WDT_MAJOR_DEFAULT 0
#endif

#define WDT_ACTION_RESET
//#define WDT_ACTION_INTERRUPT

static int g_WDT_major = WDT_MAJOR_DEFAULT;
static unsigned int WDT_margin = WDT_MARGIN;
static int wdt_opened = 0;
static int wdt_nowayout = 0;
static int refclk = WDT_CLKFREQ;
static struct class *ptModuleClass;
static unsigned long wdt_match = 0;

static struct cdev *wdt_cdev;

const char id[] = "$Version: VPL WDT driver v"WDT_DRIVER_VERSION" $";

static void WDT_ping(void)
{
	PDEBUG("WDT_ping at %lu, margin=%d\n", jiffies, WDT_margin);
	PDEBUG("MATCH_VALUE is %u\n", readl(WDT_MMR_MATCH_VALUE));
	outl((unsigned int) WDT_margin * refclk, WDT_MMR_RELOAD_VALUE);
	outl((unsigned int) WDT_RELOAD_PASSWD, WDT_MMR_RELOAD_CTRL);
}

#ifdef WDT_ACTION_INTERRUPT
irqreturn_t WDT_interrupt(int irq, void *dev_id)
{
	printk("WDT interrupt!!\n");
	PDEBUG("WDT_Interrupt\n");
	WDT_ping();
	return IRQ_HANDLED;
}
#endif

static void WDT_enable(void)
{
	unsigned int val;
	val = readl(WDT_MMR_CTRL);
	writel(val | WDT_CTRL_OP_EN, WDT_MMR_CTRL);
}

#ifdef WDT_ACTION_RESET
static void WDT_enable_rst(void)
{
	unsigned int val = WDT_RESET_LEN;
	writel(val, WDT_MMR_RST_LEN);
}
#endif

#ifdef WDT_ACTION_INTERRUPT
static void WDT_enable_intr(void)
{
	unsigned int val;
	val = readl(WDT_MMR_CTRL);
	writel(val | WDT_CTRL_ACK_EN, WDT_MMR_CTRL);
}
#endif

static void WDT_disable(void)
{
	unsigned int val;
	val = readl(WDT_MMR_CTRL);
	writel(val & (~WDT_CTRL_OP_EN), WDT_MMR_CTRL);
}

static void WDT_disable_rst(void)
{
	writel(0, WDT_MMR_RST_LEN);
}

static void WDT_disable_intr(void)
{
	unsigned int val;
	val = readl(WDT_MMR_CTRL);
	writel(val & (~WDT_CTRL_ACK_EN), WDT_MMR_CTRL);
}

static int WDT_ioctl(struct inode *inode, struct file *filp,
                     unsigned int cmd, unsigned long arg)
{
	unsigned long new_margin = 0;

	switch (cmd) {
		case WDIOC_SETMATCH:
			if (get_user(wdt_match, (unsigned long *)arg))
				return -EFAULT;
			if (unlikely(wdt_match > ULONG_MAX/refclk))
				return -EINVAL;
			writel(wdt_match * refclk, WDT_MMR_MATCH_VALUE);
			wdt_match = readl(WDT_MMR_MATCH_VALUE);
			PDEBUG("set match to %d\n", wdt_match);
			break;

		case WDIOC_KEEPALIVE:
			WDT_ping();
			break;
		case WDIOC_SETTIMEOUT:
			if (get_user(new_margin, (unsigned long *) arg))
				return -EFAULT;
			if (unlikely(new_margin > ULONG_MAX/refclk))
				return -EINVAL;
			WDT_margin = new_margin;
			WDT_ping();
			break;
		case WDIOC_GETTIMEOUT:
			return put_user(WDT_margin, (unsigned long *) arg);
		case WDIOC_ENABLE:
			WDT_enable();
			WDT_ping();
			break;
		case WDIOC_DISABLE:
			WDT_disable();
			break;
		default:
			return -ENOTTY;
	}
	return 0;
}

static int WDT_open(struct inode *inode, struct file *filp)
{
	if (wdt_opened != 0){
		return -EBUSY;
	}
	wdt_opened++;
	WDT_disable();
	WDT_disable_rst();
	WDT_disable_intr();
#ifdef WDT_ACTION_RESET
	WDT_enable_rst();
#endif
#ifdef WDT_ACTION_INTERRUPT
	fLib_SetIntTrig(IRQ_WATCHDOG, 1, 0);
	WDT_enable_intr();
	if (request_irq(IRQ_WATCHDOG, WDT_interrupt, IRQF_DISABLED, "wdt", NULL)) {
		PDEBUG(KERN_ERR "Unable to allocate WDT IRQ=0x%X\n", IRQ_WATCHDOG);
		WDT_disable_intr();
		return -EBUSY;
	}
#endif
	WDT_enable();
	WDT_margin = WDT_MARGIN;
	WDT_ping();
	PDEBUG("WDT module open success\n");
	return 0;
}

static int WDT_release(struct inode *inode, struct file *file)
{
	wdt_opened = 0;

	if (wdt_nowayout == 0) {
		WDT_ping();
		WDT_disable();
	}
#ifdef WDT_ACTION_INTERRUPT
	WDT_disable_intr();
	free_irq(IRQ_WATCHDOG, NULL);
#endif
#ifdef WDT_ACTION_RESET
	WDT_disable_rst();
#endif
	PDEBUG("WDT module close success\n");
	return 0;
}

void WDT_cleanup(void)
{
	if (g_WDT_major != 0) {
		dev_t dev;

		dev = MKDEV(g_WDT_major, 0);

		WDT_disable();
#ifdef WDT_ACTION_RESET
		WDT_disable_rst();
#endif
#ifdef WDT_ACTION_INTERRUPT
		WDT_disable_intr();
#endif
		unregister_chrdev_region(dev, 1);
	}

	device_destroy(ptModuleClass, MKDEV(g_WDT_major, 0));
	class_destroy(ptModuleClass);

	cdev_del(wdt_cdev);
}

static struct file_operations WDT_fops = {
	owner:THIS_MODULE,
	ioctl:WDT_ioctl,
	open:WDT_open,
	release:WDT_release,
};

int WDT_init(void)
{
	int ret = 0;
	dev_t dev;

	printk(KERN_INFO "%s\n", id);

	wdt_cdev = cdev_alloc();
	if (wdt_cdev == NULL)
		goto fail_init;
	wdt_cdev->ops = &WDT_fops;

	if (g_WDT_major != 0) {
		dev = MKDEV(g_WDT_major, 0);
		ret = register_chrdev_region(dev, 1, "wdt");
		if (ret < 0) {
			PDEBUG(" can't get major %d\n", g_WDT_major);
			goto fail_init;
		}
	}
	else {
		ret = alloc_chrdev_region(&dev, 0, 1, "wdt");
		if (ret < 0) {
			PDEBUG(" can't alloc major\n");
			goto fail_init;
		}
		g_WDT_major = MAJOR(dev);
	}
	PDEBUG("WDT major number = %d\n", g_WDT_major);
	PDEBUG("WDT_CLKFREQ = %d\n", WDT_CLKFREQ);
	PDEBUG("refclk = %d\n", refclk);

	ret = cdev_add(wdt_cdev, dev, 1);
	if (ret < 0) {
		PDEBUG("cant't add cdev\n");
		goto fail_init;
	}

		/* Create a struct class structure */
	ptModuleClass = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(ptModuleClass)) {
		goto fail_init;
	}
	device_create(ptModuleClass, NULL, MKDEV(g_WDT_major, 0), NULL, DEVICE_NAME);


	return 0;
fail_init:
	WDT_cleanup();
	return ret;
}

module_init(WDT_init);
module_exit(WDT_cleanup);

module_param(refclk, int, 0644);
MODULE_PARM_DESC(refclk, "The reference clock referred by watchdog.");
module_param(wdt_nowayout, int, 0644);
MODULE_PARM_DESC(wdt_nowayout, "The watchdog cannot be stopped once it has been started.");

MODULE_AUTHOR("tassader");
MODULE_DESCRIPTION("VPL Watchdog Timer driver");
MODULE_LICENSE("GPL");
