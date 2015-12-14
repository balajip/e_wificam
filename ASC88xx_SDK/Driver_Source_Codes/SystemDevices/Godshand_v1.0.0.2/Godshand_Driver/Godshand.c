/*
 * Godshand
 * Driver for Godshand.
 *
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
/* ======================================================================== */
#include "Godshand_local.h"
#include <linux/device.h>
/* ======================================================================== */
static void *Godshand_base = NULL;
static int Godshand_major = GODSHANDCD_MAJOR_DEFAULT;
static GODSHAND_Info * Godshand_info = NULL;
static GODSHAND_Dev * Godshand_device = NULL;
static void * Godshand_memdisk = NULL;
static struct class *ptModuleClass;

const char id[] = "$Godshand: "DRIVER_VERSION" "PLATFORM" "__DATE__" "COMPANY" $";
MODULE_LICENSE("GPL");
/*
#ifdef LINUX26
//module_param(Godshand_major, int, 0644);
#else
MODULE_PARM(Godshand_major,"i");
#endif
//MODULE_PARM_DESC(Godshand_major, "Major number for Godshanddc");
*/
/* ======================================================================== */
static int Godshand_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int ret = 0;
	void * Godshand_location = NULL;
	DWORD tmp;
	GODSHAND_Dev *dev = (GODSHAND_Dev *)filp->private_data;
	DWORD GODSHAND_LOCATION;
	DWORD GODSHAND_AREA_SIZE;
	Godshand_info = (GODSHAND_Info *)arg;

	GODSHAND_LOCATION = Godshand_info->dwAddress;
	GODSHAND_AREA_SIZE = GODSHAND_SIZE; 
	PDEBUG("the address is %x\n",Godshand_info->dwAddress);
	if (Godshand_info->dwFlag & GODSHAND_FLAG_AREA)
	{
		GODSHAND_AREA_SIZE = Godshand_info->dwSize;
	}

	if (check_mem_region(GODSHAND_START, GODSHAND_SIZE) < 0) {
		PDEBUG("ERROR: allocated fail!\n\r");
		ret = -EBUSY;
		goto fail;
	}
	request_mem_region(GODSHAND_START, GODSHAND_SIZE, "GODSHAND module .");

	Godshand_base = (unsigned long *)ioremap((int)GODSHAND_START, (int)GODSHAND_SIZE);
	Godshand_location = (unsigned long *)ioremap((int)GODSHAND_LOCATION, (int)GODSHAND_AREA_SIZE);
	PDEBUG("%x: Allocated\n\r", (int)Godshand_base);

	if (!dev) {
		return -ENODEV;
	}
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != GODSHAND_IOC_MAGIC) {
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) > GODSHAND_IOC_MAXNUM) {
		return -ENOTTY;
	}
	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	  * "write" is reversed
	*/
	if (_IOC_DIR(cmd) & _IOC_READ) {
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	} else if (_IOC_DIR(cmd) & _IOC_WRITE){
		err = !access_ok(VERIFY_READ,  (void *)arg, _IOC_SIZE(cmd));
	}
	
	if (err) {
		return -EFAULT;
	}

	switch (cmd)
	{
		case GODSHAND_IOC_RESET :
			break;
		case GODSHAND_IOC_CLEAR :
			break;
		case GODSHAND_IOC_READ :
			if (down_interruptible(&dev->sem)) {
				return -ERESTARTSYS;
			}
			ret = __put_user(readl((u32*)(Godshand_location)), (u32*)(&Godshand_info->dwData));
			up(&dev->sem);
			break;
		case GODSHAND_IOC_READ_AREA :
			if (down_interruptible(&dev->sem)) {
				return -ERESTARTSYS;
			}
			ret = copy_to_user(Godshand_info->pDataBuffer, ((u32*)Godshand_location), GODSHAND_AREA_SIZE);
			up(&dev->sem);
			break;
		case GODSHAND_IOC_WRITE :
			ret = __get_user(tmp, (u32*)(&Godshand_info->dwData));
			if (down_interruptible(&dev->sem)) {
				return -ERESTARTSYS;
			}
			writel(tmp,(u32*)(Godshand_location));
			up(&dev->sem);
			break;
		case GODSHAND_IOC_WRITE_AREA :
			ret = __get_user(tmp, (u32*)(&Godshand_info->dwData));
			if (down_interruptible(&dev->sem)) {
				return -ERESTARTSYS;
			}
			writel(tmp,(u32*)(Godshand_location));
			up(&dev->sem);
			break;
		default:  /* redundant, as cmd was checked against MAXNR */
			return -ENOTTY;
	}
fail :
	PDEBUG("%s iounmap\n",__func__);
	iounmap(Godshand_location);
	iounmap(Godshand_base);
	Godshand_base = NULL;
	PDEBUG("%s release_mem_region\n",__func__);
	release_mem_region(GODSHAND_START,GODSHAND_SIZE);
	return ret;

}

/* ------------------------------------------------------------------------ */
static ssize_t Godshand_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	ssize_t ret = 0;

//	if (verify_area(VERIFY_WRITE,buf,count) == -EFAULT )
	if (access_ok(VERIFY_WRITE,buf,count) == -EFAULT )
		return -EFAULT;
	copy_to_user(buf, ((char*)Godshand_memdisk), count);
	ret = count;
	PDEBUG("%s...\n", __func__);
	return ret;
}

/* ------------------------------------------------------------------------ */
static ssize_t Godshand_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	ssize_t ret = 1;
	copy_from_user(((char*)Godshand_memdisk), buf, count);
	PDEBUG("%s...\n", __func__);
	ret = count;
	return ret;
}

/* ------------------------------------------------------------------------ */
static loff_t Godshand_llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos = -1;
	return newpos;
}

/* ------------------------------------------------------------------------ */
static int Godshand_release(struct inode *inode, struct file *filp)
{
	if (Godshand_memdisk)
	{
		PDEBUG("%s kfree\n",__func__);
		kfree(Godshand_memdisk);
		Godshand_memdisk = NULL;
	}
	PDEBUG("%s Bye Bye...\n",__func__);
	return 0;
}

/* ------------------------------------------------------------------------ */
static int Godshand_open(struct inode *inode, struct file *filp)
{
	GODSHAND_Dev *dev;
	int tmp;

	dev = (GODSHAND_Dev *)filp->private_data;
	if (!dev) 
	{
		dev = Godshand_device;
		filp->private_data = dev;
	}
	Godshand_memdisk = kmalloc (sizeof(char)*30, GFP_KERNEL);
	if (!Godshand_memdisk)
	{
		printk("open memdisk fail!!!\n");
	}
	for (tmp=0; tmp<30; tmp++)
	{
		*((char *)Godshand_memdisk + (sizeof(char)*tmp)) = 'A' + tmp;
	}
	PDEBUG("Hello Godshand~\n");
	return 0;
}

/* ------------------------------------------------------------------------ */
struct file_operations Godshand_fops = {
	llseek:		Godshand_llseek,
	read:		Godshand_read,
	write:		Godshand_write,
	ioctl:		Godshand_ioctl,
	open:		Godshand_open,
	release:	Godshand_release,
};

/* ------------------------------------------------------------------------ */
static void Godshand_cleanup_module(void)
{
	if (Godshand_device) {
		PDEBUG("%s kfree\n\r",__func__);
		kfree(Godshand_device);
		Godshand_device = NULL;
	}
	device_destroy(ptModuleClass, MKDEV(Godshand_major, 0));
	if (Godshand_major != 0)
	{
		PDEBUG("%s unregister_chrdev\n",__func__);
		unregister_chrdev(Godshand_major, "Godshand");
		Godshand_major = 0;
	}
	class_destroy(ptModuleClass);
}

/* ------------------------------------------------------------------------ */
static int Godshand_init_module(void)
{
	int result;

	result = register_chrdev(Godshand_major, "Godshand", &Godshand_fops);
	if (result < 0) {
		PDEBUG("ERROR: can't get major %d\n\r",Godshand_major);
		goto fail;
	}
	Godshand_major = result;
	PDEBUG("The major number is %d\n\r",Godshand_major);
	
	Godshand_device = kmalloc(sizeof(GODSHAND_Dev), GFP_KERNEL);
	if (!Godshand_device)
	{
		result = -ENOMEM;
		goto fail;
	}
	memset(Godshand_device, 0, sizeof(GODSHAND_Dev));
	sema_init(&Godshand_device->sem, 1);
	ptModuleClass = class_create(THIS_MODULE, "Godshand");
	if (IS_ERR(ptModuleClass)) {
		goto fail;
	}
	device_create(ptModuleClass, NULL, MKDEV(Godshand_major, 0), NULL, "Godshand");
	return 0;
fail :
	Godshand_cleanup_module();
	return result;
}

/* ------------------------------------------------------------------------ */
module_init(Godshand_init_module);
module_exit(Godshand_cleanup_module);

/* ======================================================================== */
