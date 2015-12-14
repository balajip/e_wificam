/*
 * Driver for VPL IrDA Controller.
 * Copyright (c) 2007 by VN Inc.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>

#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/smp_lock.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/device.h>

#define DEBUG 1
#define _IRRC_REAPET_COUNT	3
#define INVERSE	0
//#define PIO_MODE_TEST

#include "irrc.h"

static int protocol = RC5;
module_param(protocol, int, S_IRUGO|S_IWUSR);

static struct class *ptModuleClass;

const char irrc_id[] = "$IrDA: "IRRC_DRIVER_VERSION" "IRRC_PLATFORM" "__DATE__" "IRRC_MAINTAINER" "IRRC_COMPANY" $";

/*==================================================================*/
static IrrcSettings IRRC_SETTINGS_NEC =
{
	31,					//IRDN = 32-1
	IRRC_25_PERCENT,	//IRDR = 2'b01
	IRRC_PULSE_DIST,	//IRMT = 2'b01
	INVERSE,				//IRISE = 1'b1
	IRRC_LSB,			//IRENDIAN = 1'b0
	1,					//IRBME = 1'b1
	1,					//IRFIE = 1'b1

	0x38,				//IRRML = 0x38
	0x38,				//IRRZL = 0x38
	0xA9,				//IRROL = 0xA9

	0x0F,				//IRRDL = 0x0F
	0xE1,				//IRRRL = 0xE1
	0xE1,				//IRRSL = 0xE1
	0xE1,				//IRRBL = 0xE1

	IR_FREQ_DIV-1,		//IRFD = 0x9C-1
	0x0000,				//IRRI = 0x0000
};

static IrrcSettings IRRC_SETTINGS_RC5 =
{
	13,					//IRDN = 14-1
	IRRC_25_PERCENT,	//IRDR = 2'b01
	IRRC_BI_PHASE,		//IRMT = 2'b00
	INVERSE,				//IRISE = 1'b1
	IRRC_MSB,			//IRENDIAN = 1'b1
	0,					//IRBME = 1'b0
	1,					//IRFIE = 1'b1

	0x5A,				//IRRML = 0x5A
	0x5A,				//IRRZL = 0x5A
	0x5A,				//IRROL = 0x5A
	
	0x0F,				//IRRDL = 0x0F
	0x00,				//IRRRL = 0x00
	0x00,				//IRRSL = 0x00
	0x00,				//IRRBL = 0x00
	
	IR_FREQ_DIV-1,		//IRFD = 0x9C-1
	0x0000,				//IRRI = 0x0000
};

static IrrcSettings IRRC_SETTINGS_Sharp =
{
	14,					//IRDN = 15-1
	IRRC_25_PERCENT,	//IRDR = 2'b01
	IRRC_PULSE_DIST,	//IRMT = 2'b10
	INVERSE,				//IRISE = 1'b1
	IRRC_LSB,			//IRENDIAN = 1'b0
	0,					//IRBME = 1'b0
	1,					//IRFIE = 1'b1

	0x20,				//IRRML = 0x20
	0x44,				//IRRZL = 0x44
	0xa8,				//IRROL = 0xa8
	
	0x0F,				//IRRDL = 0x0F
	0x00,				//IRRRL = 0x00
	0x00,				//IRRSL = 0x00
	0x00,				//IRRBL = 0x00
	
	IR_FREQ_DIV-1,		//IRFD = 0x9C-1
	0x0000,				//IRRI = 0x0000
};

static IrrcSettings IRRC_SETTINGS_SonySIRC =
{
	14,					//IRDN = 15-1
	IRRC_25_PERCENT,	//IRDR = 2'b01
	IRRC_PULSE_WIDTH,	//IRMT = 2'b10
	INVERSE,				//IRISE = 1'b1
	IRRC_LSB,			//IRENDIAN = 1'b0
	1,					//IRBME = 1'b1
	1,					//IRFIE = 1'b1

	0x3C,				//IRRML = 0x3C
	0x3C,				//IRRZL = 0x3C
	0x78,				//IRROL = 0x78
	
	0x0F,				//IRRDL = 0x0F
	0x00,				//IRRRL = 0x00
	0x1E,				//IRRSL = 0x1E
	0x3C,				//IRRBL = 0x3C
	
	IR_FREQ_DIV-1,		//IRFD = 0x9C-1
	0x01C2,				//IRRI = 0x01C2
};

static IrrcSettings IRRC_SETTINGS_NEC_64 =
{
	47,					//IRDN = 48-1
	IRRC_25_PERCENT,	//IRDR = 2'b01
	IRRC_PULSE_DIST,	//IRMT = 2'b10
	INVERSE,				//IRISE = 1'b1
	IRRC_LSB,			//IRENDIAN = 1'b0
	1,					//IRBME = 1'b1
	1,					//IRFIE = 1'b1

	0x26,				//IRRML = 0x26
	0x2f,				//IRRZL = 0x2f
	0x87,				//IRROL = 0x87
	
	0x0F,				//IRRDL = 0x0F
	0x00,				//IRRRL = 0x00
	0x5a,				//IRRSL = 0x5a
	0x53,				//IRRBL = 0x53
	
	IR_FREQ_DIV-1,		//IRFD = 0x9C-1
	0x1B58,				//IRRI = 0x1B58
};

struct irrc_queue {
	unsigned long head;
	unsigned long tail;
	wait_queue_head_t proc_list;
	unsigned long buf[IRRC_BUF_SIZE];
};

typedef struct IrrcDevice_t {
	IrrcProtocols Protocol;
}IrrcDevice;

/*-------------------------------------------------------------------------------------*/
static struct irrc_queue *queue;	/* Infra Red remote Controller data buffer. */
static unsigned long IRRC_Repeat_Count = 0;
static unsigned long toggle_p = 2;

/*-------------------------------------------------------------------------------------*/
ssize_t  mozart_irrc_read(struct file *filep, unsigned long *buffer, size_t count, loff_t *offp);
int mozart_irrc_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
int mozart_irrc_open(struct inode *inode, struct file *filep);
int mozart_irrc_release(struct inode *inode, struct file *filep);
#ifndef PIO_MODE_TEST
int mozart_irrc_interrupt(int irq, void *dev_id);
#endif

static struct file_operations mozart_irrc_fops = {
	read:		mozart_irrc_read,
	ioctl:		mozart_irrc_ioctl,
	open:		mozart_irrc_open,
	release:		mozart_irrc_release,
};

/*==================================================================*/
static int irrc_major = MOZART_IRRC_MAJOR;

MODULE_DESCRIPTION("IrDA driver");
MODULE_LICENSE("GPL");
#if 0
MODULE_PARM(irrc_major, "i");
MODULE_PARM_DESC(irrc_major, "Major number for irrc");
#endif
/*==================================================================*/
static inline int queue_empty(void)
{
	return queue->head == queue->tail;
}

/*-------------------------------------------------------------------------------------*/
static unsigned long get_from_queue(void)
{
	unsigned long result = 0;

	result = queue->buf[queue->tail];
	queue->tail = (queue->tail + 1) & (IRRC_BUF_SIZE-1);
	return result;
}

/*==================================================================*/
/* This function configures the specified settings of the IrDA remote controller module. */
void  Irrc_SetParameters (IrrcSettings *IrdaSettings)
{
	unsigned long CTRLSetting;
	unsigned long TIMINGSetting;
	unsigned long TIMINGEXTSetting;
	unsigned long InternalFreqSetting;

	CTRLSetting = MOZART_IRRC_READL(IRRC_CTRL);
	CTRLSetting &= ~(DATA_NUM_MASK
				| DRIFT_MASK
				| MODU_TYPE_MASK 
				| (1<<INV_SDA_EN_BIT)
				| (1<<IRRX_ENDIAN_BIT)
				| (1<<BURST_MODE_EN_BIT) 
				| (1<<FIFO_EN_BIT));
	CTRLSetting |= (IrdaSettings->IRDN<<DATA_NUM_BIT) 
				| (IrdaSettings->IRDR<<DRIFT_BIT)
				| (IrdaSettings->IRMT<<MODU_TYPE_BIT)
				| (IrdaSettings->IRISE<<INV_SDA_EN_BIT)
				| (IrdaSettings->IRENDIAN<<IRRX_ENDIAN_BIT)
				| (IrdaSettings->IRBME<<BURST_MODE_EN_BIT) 
				| (IrdaSettings->IRFIE<<FIFO_EN_BIT);

	TIMINGSetting = (IrdaSettings->IRRML<<MODU_LEN_BIT)
				| (IrdaSettings->IRRDZL<<ZERO_LEN_BIT) 
				| (IrdaSettings->IRRDOL<<ONE_LEN_BIT);

	TIMINGEXTSetting = (IrdaSettings->IRRDL<<DEBOUNCE_BIT)
					| (IrdaSettings->IRRRL<<REPEAT_LEN_BIT) 
					| (IrdaSettings->IRRSL<<SILEN_LEN_BIT) 
					| (IrdaSettings->IRRBL<<BURST_LEN_BIT);

	InternalFreqSetting = (IrdaSettings->IRFD<<FREQ_DIV_BIT)
					| (IrdaSettings->IRRI<<REPEAT_INTERVAL_BIT);

	MOZART_IRRC_WRITEL(IRRC_CTRL, CTRLSetting);
	MOZART_IRRC_WRITEL(IRRC_TIMING, TIMINGSetting);
	MOZART_IRRC_WRITEL(IRRC_TIMING_EXT, TIMINGEXTSetting);
	MOZART_IRRC_WRITEL(IRRC_INTERVAL_FREQ_DIV, InternalFreqSetting);
}

/*-------------------------------------------------------------------------------------*/
/* This function gets the configuration settings of the IrDA remote controller module. */
void  Irrc_GetParameters (IrrcSettings *IrdaSettings)
{
	unsigned long CTRLSetting;
	unsigned long TIMINGSetting;
	unsigned long TIMINGEXTSetting;
	unsigned long INTERVALSetting;

	CTRLSetting = MOZART_IRRC_READL(IRRC_CTRL);
	TIMINGSetting = MOZART_IRRC_READL(IRRC_TIMING);
	TIMINGEXTSetting = MOZART_IRRC_READL(IRRC_TIMING_EXT);
	INTERVALSetting = MOZART_IRRC_READL(IRRC_INTERVAL_FREQ_DIV);

	//IRDAC_CTRL
	IrdaSettings->IRDN = (CTRLSetting & DATA_NUM_MASK) >> DATA_NUM_BIT;
	IrdaSettings->IRDR = (CTRLSetting & DRIFT_MASK) >> DRIFT_BIT;
	IrdaSettings->IRMT = (CTRLSetting & MODU_TYPE_MASK) >> MODU_TYPE_BIT;
	IrdaSettings->IRISE = (CTRLSetting & (1<<INV_SDA_EN_BIT)) >> INV_SDA_EN_BIT; 
	IrdaSettings->IRENDIAN = (CTRLSetting & (1<<IRRX_ENDIAN_BIT)) >> IRRX_ENDIAN_BIT;
	IrdaSettings->IRBME = (CTRLSetting & (1<<BURST_MODE_EN_BIT)) >> BURST_MODE_EN_BIT;
	IrdaSettings->IRFIE = (CTRLSetting & (1<<FIFO_EN_BIT)) >> FIFO_EN_BIT;

	//IRDAC_TIMING
	IrdaSettings->IRRML = (TIMINGSetting & MODU_LEN_MASK) >> MODU_LEN_BIT;
	IrdaSettings->IRRDZL = (TIMINGSetting & ZERO_LEN_MASK) >> ZERO_LEN_BIT;
	IrdaSettings->IRRDOL = (TIMINGSetting & ONE_LEN_MASK) >> ONE_LEN_BIT;

	//IRDAC_TIMING_EXT
	IrdaSettings->IRRDL = (TIMINGEXTSetting & DEBOUNCE_MASK) >> DEBOUNCE_BIT;
	IrdaSettings->IRRRL = (TIMINGEXTSetting & REPEAT_LEN_MASK) >> REPEAT_LEN_BIT;
	IrdaSettings->IRRSL = (TIMINGEXTSetting & SILEN_LEN_MASK) >> SILEN_LEN_BIT;
	IrdaSettings->IRRBL = (TIMINGEXTSetting & BURST_LEN_MASK) >> BURST_LEN_BIT;
	
	//IRDAC_INTERVAL
	IrdaSettings->IRFD = (INTERVALSetting & FREQ_DIV_MASK) >> FREQ_DIV_BIT;	
	IrdaSettings->IRRI = (INTERVALSetting & REPEAT_INTERVAL_MASK) >> REPEAT_INTERVAL_BIT;
}

/*-------------------------------------------------------------------------------------*/
 /* This function shall enable the IrDA remote controller module. */
void  Irrc_Enable (void)
{
	unsigned long CRSetting;

	CRSetting = MOZART_IRRC_READL(IRRC_CTRL);
	MOZART_IRRC_WRITEL(IRRC_CTRL, CRSetting | (1<<OP_START_BIT)); //IRRC Enable
}

/*-------------------------------------------------------------------------------------*/
/* This function shall disable the IrDA remote controller module. */
void  Irrc_Disable (void)
{
	unsigned long CRSetting;

	CRSetting = MOZART_IRRC_READL(IRRC_CTRL);
	MOZART_IRRC_WRITEL(IRRC_CTRL, CRSetting & ~(1<<OP_START_BIT)); //IRRC Disable
}

/*-------------------------------------------------------------------------------------*/
/* This function resets all register of all control and status registers of the IrDA remote controller module. */
void  Irrc_Reset (void)
{
	MOZART_IRRC_WRITEL(IRRC_CTRL, 1<<RST_EN_BIT); //IRRC reset
 	udelay(10);			//wait for reset complete
}

/*-------------------------------------------------------------------------------------*/
/* This function shall initialize the IrDA remote controller module. */
int  Irrc_Open (IrrcProtocols IrdaProtocol)
{
	IrrcSettings * ProtoSettings;

	Irrc_Reset();

	switch(IrdaProtocol)
	{
	case NEC:
		ProtoSettings = &IRRC_SETTINGS_NEC;
		break;
	case RC5:
		ProtoSettings = &IRRC_SETTINGS_RC5;
		break;
	case Sharp:
		ProtoSettings = &IRRC_SETTINGS_Sharp;
		break;
	case SonySIRC:
		ProtoSettings = &IRRC_SETTINGS_SonySIRC;
		break;
	case NEC64:
		ProtoSettings = &IRRC_SETTINGS_NEC_64;
		break;
	default:
		printk("Selected Protocol Invalid!\n");
		return -EINVAL;
	}

	Irrc_SetParameters(ProtoSettings);
	Irrc_Enable(); //Enable IRRC

	return 0;
}

/*-------------------------------------------------------------------------------------*/
/* This function shall close the IrDA remote controller module. */
int  Irrc_Close (void)
{
	Irrc_Disable();
	Irrc_Reset();
	return 0;
}

/*==================================================================*/
#ifndef PIO_MODE_TEST
ssize_t  mozart_irrc_read(struct file *filep, unsigned long *buffer, size_t count, loff_t *offp)
{
#ifdef DEBUG
	printk(KERN_EMERG "irrc.ko: we are about to read irrc...\n");
#endif
	DECLARE_WAITQUEUE(wait, current);
	ssize_t i = count;
	unsigned long c;

	if (queue_empty())
	{
#ifdef DEBUG
		printk(KERN_EMERG "queue empty\n");
#endif
		if (filep->f_flags & O_NONBLOCK)
			return -EAGAIN;
		add_wait_queue(&queue->proc_list, &wait);
repeat:
		current->state = TASK_INTERRUPTIBLE;
		if (queue_empty() && !signal_pending(current))
		{
#ifdef DEBUG
			printk(KERN_EMERG "ready to sleep\n");
#endif
			schedule();
			goto repeat;
		}
		current->state = TASK_RUNNING;
		remove_wait_queue(&queue->proc_list, &wait);
	}

	while (i > 0 && !queue_empty())
	{
#ifdef DEBUG
		printk(KERN_EMERG "put data %d to user space~\n", count - i);
#endif
		c = get_from_queue();
		put_user(c, buffer++);
		// data_len > 32
		if (((MOZART_IRRC_READL(IRRC_CTRL) & DATA_NUM_MASK) >> DATA_NUM_BIT) >=32)
		{
			c = get_from_queue();
			put_user(c, buffer++);		
		}
		i --;
	}

	if (count-i)
	{
		filep->f_dentry->d_inode->i_atime = CURRENT_TIME;
		return count-i;
	}
	if (signal_pending(current))
	{
		printk(KERN_EMERG "signal_pending??\n");
		return -ERESTARTSYS;
	}
	return 0;
}
#else
ssize_t  mozart_irrc_read(struct file *filep, unsigned long *buffer, size_t count, loff_t *offp)
{
	ssize_t i = count;
	unsigned long c;

	unsigned long head;
	unsigned long status;
	unsigned long inputKeyValue ;
	unsigned long toggle_c;
	IrrcDevice *irrcDev = (IrrcDevice*)filep->private_data;

	printk("irrc.ko: we are about to read irrc...\n");
	while (i > 0)
	{
		head = queue->head;
		do
		{
			status = MOZART_IRRC_READL(IRRC_STAT);
		}while(!(status & (1<<OP_CMPT_BIT)));
		inputKeyValue = MOZART_IRRC_READL(IRRC_PBR_RIGHT);
		switch(irrcDev->Protocol)
		{
		case NEC:
			printk("NEC count=%d\n", i);
			if(status & (1<<REPEAT_BIT))
			{
				IRRC_Repeat_Count++;
				printk("NEC--Repeat count=[%d]\n", IRRC_Repeat_Count);				
				if(IRRC_Repeat_Count >= _IRRC_REAPET_COUNT)
				{
					IRRC_Repeat_Count--;
					goto RECV;
				}
			}
			else
			{
				IRRC_Repeat_Count = 0;
				goto RECV;
			}
			break;
		case SonySIRC:
			printk("SonySIRC count=%d\n", i);
			if(status & (1<<REPEAT_BIT))
			{
				IRRC_Repeat_Count++;
				printk("SonySIRC--Repeat count=[%d]\n", IRRC_Repeat_Count);				
				if(IRRC_Repeat_Count >= _IRRC_REAPET_COUNT)
				{
					IRRC_Repeat_Count--;
					goto RECV;
				}
			}
			else
			{
				IRRC_Repeat_Count = 0;
				goto RECV;
			}
			break;
		case Sharp:
			printk("Sharp count=%d\n", i);			
			goto RECV;
		case RC5:
			printk("RC5 count=%d\n", i);
			toggle_c = (inputKeyValue & (0x1<<RC5_TOGGLE_BIT)) >> RC5_TOGGLE_BIT;
			if(toggle_p == toggle_c)
			{
				IRRC_Repeat_Count++;
				printk("RC5--Repeat count=[%d]\n", IRRC_Repeat_Count);
				if(IRRC_Repeat_Count >= _IRRC_REAPET_COUNT)
				{
					IRRC_Repeat_Count--;
					goto RECV;
				}
			}
			else
			{
				IRRC_Repeat_Count = 0;
				toggle_p = toggle_c;

RECV:
				printk("got: %x\n", inputKeyValue);
				queue->buf[head] = inputKeyValue;
				head = (head + 1) & (IRRC_BUF_SIZE - 1);
				if(head != queue->tail)
				{
					queue->head = head;
				}
				c = get_from_queue();
				put_user(c, buffer++);
				i --;
			}
			break;
		}
	}
	if (count-i)
	{
		filep->f_dentry->d_inode->i_atime = CURRENT_TIME;
		return count-i;
	}
	return 0;
}
#endif
/*-------------------------------------------------------------------------------------*/
int mozart_irrc_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	IrrcSettings *usr_settings;
	IrrcSettings settings;
	IrrcDevice *irrcDev = (IrrcDevice*)file->private_data;

#ifdef DEBUG	
	printk(KERN_EMERG "irrc.ko: ioctl..\n");
#endif
	switch(cmd)
	{
		case IRRC_RESET:
			Irrc_Reset();
			break;

		case IRRC_SETPROTOCOL:
			switch((IrrcProtocols)arg)
			{
			case NEC:
				settings = IRRC_SETTINGS_NEC;
				break;
			case RC5:
				settings = IRRC_SETTINGS_RC5;
				break;
			case Sharp:
				settings = IRRC_SETTINGS_Sharp;
				break;
			case SonySIRC:
				settings = IRRC_SETTINGS_SonySIRC;
				break;
			default:
				printk(KERN_EMERG "Selected Protocol Invalid!\n");
				return -EINVAL;
			}
			Irrc_SetParameters(&settings);			
			break;

		case IRRC_SETPARAM:
			usr_settings = (IrrcSettings *)arg;
			get_user(settings.IRDN, &usr_settings->IRDN);
			get_user(settings.IRDR, &usr_settings->IRDR);
			get_user(settings.IRMT, &usr_settings->IRMT);
			get_user(settings.IRISE, &usr_settings->IRISE);			
			get_user(settings.IRENDIAN, &usr_settings->IRENDIAN);
			get_user(settings.IRBME, &usr_settings->IRBME);
			get_user(settings.IRFIE, &usr_settings->IRFIE);
			get_user(settings.IRRML, &usr_settings->IRRML);
			get_user(settings.IRRDZL, &usr_settings->IRRDZL);
			get_user(settings.IRRDOL, &usr_settings->IRRDOL);
			get_user(settings.IRRDL, &usr_settings->IRRDL);
			get_user(settings.IRRRL, &usr_settings->IRRRL);
			get_user(settings.IRRSL, &usr_settings->IRRSL);
			get_user(settings.IRRBL, &usr_settings->IRRBL);
			get_user(settings.IRFD, &usr_settings->IRFD);
			get_user(settings.IRRI, &usr_settings->IRRI);
			Irrc_SetParameters(&settings);
			break;

		case IRRC_GETPARAM:
			usr_settings = (IrrcSettings *)arg;
			Irrc_GetParameters(&settings);
			put_user(settings.IRDN, &usr_settings->IRDN);
			put_user(settings.IRDR, &usr_settings->IRDR);
			put_user(settings.IRMT, &usr_settings->IRMT);
			put_user(settings.IRISE, &usr_settings->IRISE);
			put_user(settings.IRENDIAN, &usr_settings->IRENDIAN);
			put_user(settings.IRBME, &usr_settings->IRBME);
			put_user(settings.IRFIE, &usr_settings->IRFIE);
			put_user(settings.IRRML, &usr_settings->IRRML);
			put_user(settings.IRRDZL, &usr_settings->IRRDZL);
			put_user(settings.IRRDOL, &usr_settings->IRRDOL);
			put_user(settings.IRRDL, &usr_settings->IRRDL);
			put_user(settings.IRRRL, &usr_settings->IRRRL);
			put_user(settings.IRRSL, &usr_settings->IRRSL);			
			put_user(settings.IRRBL, &usr_settings->IRRBL);
			put_user(settings.IRFD, &usr_settings->IRFD);			
			put_user(settings.IRRI, &usr_settings->IRRI);
			break;

		default:
			printk(KERN_EMERG "command not support!\n");
			break;
	}
	return 0;
}

/*-------------------------------------------------------------------------------------*/
int mozart_irrc_open(struct inode *inode, struct file *filep)
{
	unsigned int irrc_err;
	IrrcDevice *irrcDev;
	int rirq;

	if(!(irrcDev = (IrrcDevice *)kmalloc(sizeof(IrrcDevice),GFP_KERNEL)))
	{
		printk(KERN_EMERG "irrc.ko: Allocate %d bytes memory fail\n", sizeof(IrrcDevice));
		return -ENOMEM;
	}
	//select protocol
	irrcDev->Protocol = protocol;
	filep->private_data = irrcDev;
	// config IRRC to init IrDA
	irrc_err = Irrc_Open(irrcDev->Protocol);
	if(irrc_err != 0)
	{
		printk(KERN_EMERG "irrc.ko: Open Infra red remote controller device error-%d\n", irrc_err);
		return -ENODEV;
	}
#ifdef DEBUG
	switch (protocol)
	{
	case NEC:
		printk(KERN_EMERG "irrc.ko: open NEC protocol..\n");
		break;
	case RC5:
		printk(KERN_EMERG "irrc.ko: open RC5 protocol..\n");
		break;
	case Sharp:
		printk(KERN_EMERG "irrc.ko: open Sharp protocol..\n");		
		break;
	case SonySIRC:
		printk(KERN_EMERG "irrc.ko: open SonySIRC protocol..\n");		
		break;
	case NEC64:
		printk(KERN_EMERG "irrc.ko: open NEC64 protocol..\n");		
		break;
	}
#endif
#ifndef PIO_MODE_TEST
	//request irq...
#ifdef DEBUG	
	printk(KERN_EMERG "irrc.ko: enable interrupt number %d\n", IRDAC_IRQ_NUM);
#endif
	//enable IRRC interrupt
	MOZART_IRRC_WRITEL( IRRC_CTRL, MOZART_IRRC_READL(IRRC_CTRL) | (1<<OP_CMPT_ACK_EN_BIT));
	rirq = request_irq(IRDAC_IRQ_NUM, mozart_irrc_interrupt, IRQF_DISABLED, "irrc", irrcDev);
	if(rirq)
	{
		printk(KERN_EMERG "irrc.ko: can't assigned irq %d\n", IRDAC_IRQ_NUM);
	}
#endif

#ifdef DEBUG	
	printk(KERN_EMERG "V_IRRC_BASE=%lx, IRDA_FREQ=%lx\n", V_IRRC_BASE, IRDA_FREQ);
	printk(KERN_EMERG "IRRC_CTRL=%lx\n", MOZART_IRRC_READL(IRRC_CTRL));
	printk(KERN_EMERG "IRRC_TIMING=%lx\n", MOZART_IRRC_READL(IRRC_TIMING));
	printk(KERN_EMERG "IRRC_TIMING_EXT=%lx\n", MOZART_IRRC_READL(IRRC_TIMING_EXT));
	printk(KERN_EMERG "IRRC_INTERVAL_FREQ_DIV=%lx\n", MOZART_IRRC_READL(IRRC_INTERVAL_FREQ_DIV));
#endif	
	return 0;
}

/*-------------------------------------------------------------------------------------*/
int mozart_irrc_release(struct inode *inode, struct file *filep)
{
#ifdef DEBUG	
	printk(KERN_EMERG "irrc.ko: release..");
#endif
	IrrcDevice *irrcDev = filep->private_data;
	Irrc_Close();
	free_irq(IRDAC_IRQ_NUM, irrcDev);
	kfree(irrcDev);
	return 0;
}

/*==================================================================*/
#ifndef PIO_MODE_TEST
int mozart_irrc_interrupt(int irq, void *dev_id)
{
	unsigned long head;
	unsigned long status;
	unsigned long toggle_c;
	unsigned long inputKeyValue_l, inputKeyValue_r ;
	IrrcDevice *irrcDev = (IrrcDevice*)dev_id;

	status = MOZART_IRRC_READL(IRRC_STAT);
	head = queue->head;
	inputKeyValue_l = MOZART_IRRC_READL(IRRC_PBR_LEFT);
	inputKeyValue_r = MOZART_IRRC_READL(IRRC_PBR_RIGHT);	// data_len > 32
	switch(irrcDev->Protocol)
	{
	case NEC:
#ifdef DEBUG				
		printk(KERN_EMERG "NEC\n");
#endif
		if(status & (1<<REPEAT_BIT))
		{
			IRRC_Repeat_Count++;
#ifdef DEBUG					
			printk(KERN_EMERG "NEC--Repeat count=[%d]\n", IRRC_Repeat_Count);
#endif
			if(IRRC_Repeat_Count >= _IRRC_REAPET_COUNT)
			{
#ifdef DEBUG					
				printk(KERN_EMERG "NEC--Repeat\n");
#endif
				IRRC_Repeat_Count--;
				goto RECV;
			}
		}
		else
		{
			IRRC_Repeat_Count = 0;
			goto RECV;
		}
		break;
	case SonySIRC:		
#ifdef DEBUG				
		printk(KERN_EMERG "SonySIRC\n");
#endif
		if(status & (1<<REPEAT_BIT))
		{
			IRRC_Repeat_Count++;
#ifdef DEBUG					
			printk(KERN_EMERG "SonySIRC--Repeat count=[%d]\n", IRRC_Repeat_Count);
#endif
			if(IRRC_Repeat_Count >= _IRRC_REAPET_COUNT)
			{
#ifdef DEBUG					
				printk(KERN_EMERG "SonySIRC--Repeat\n");
#endif
				IRRC_Repeat_Count--;
				goto RECV;
			}
		}
		else
		{
			IRRC_Repeat_Count = 0;
			goto RECV;
		}
		break;
		
	case Sharp:
#ifdef DEBUG				
		printk(KERN_EMERG "Sharp\n");
#endif
		goto RECV;

	case NEC64:
#ifdef DEBUG				
		printk(KERN_EMERG "NEC64\n");
#endif
		goto RECV;

		case RC5:
#ifdef DEBUG				
		printk(KERN_EMERG "RC5\n");			
#endif
		toggle_c = (inputKeyValue_l & (0x1<<RC5_TOGGLE_BIT)) >> RC5_TOGGLE_BIT;
		if(toggle_p == toggle_c)
		{
			IRRC_Repeat_Count++;
#ifdef DEBUG				
			printk(KERN_EMERG "RC5--Repeat count=[%d]\n", IRRC_Repeat_Count);
#endif
			if(IRRC_Repeat_Count >= _IRRC_REAPET_COUNT)
			{
#ifdef DEBUG					
				printk(KERN_EMERG "RC5--Repeat\n");
#endif
				IRRC_Repeat_Count--;
				goto RECV;
			}
		}
		else
		{
			printk(KERN_EMERG "RC5--Not Repeat\n");
			IRRC_Repeat_Count = 0;
			toggle_p = toggle_c;
RECV:
#ifdef DEBUG		
			printk(KERN_EMERG "got(L): %x\n", inputKeyValue_l);
			printk(KERN_EMERG "got(R): %x\n", inputKeyValue_r);
#endif
			queue->buf[head] = inputKeyValue_l;
			head = (head + 1) & (IRRC_BUF_SIZE - 1);
			// data len > 32
			if (((MOZART_IRRC_READL(IRRC_CTRL) & DATA_NUM_MASK) >> DATA_NUM_BIT) >=32)
			{
				queue->buf[head] = inputKeyValue_r;
				head = (head + 1) & (IRRC_BUF_SIZE - 1);				
			}
			if(head != queue->tail)
			{
				queue->head = head;
				wake_up_interruptible(&queue->proc_list);
			}
			}
		break;
	default:
		printk(KERN_EMERG "unknown protocol!\n");
		break;
	}
	MOZART_IRRC_WRITEL(IRRC_CTRL, MOZART_IRRC_READL(IRRC_CTRL) &~(1<<OP_CMPT_ACK_BIT));
	return IRQ_HANDLED;
}
#endif
/*==================================================================*/
void __exit mozart_irrc_exit(void)
{
#ifdef DEBUG	
	printk(KERN_EMERG "irrc.ko: bye\n");
#endif
	device_destroy(ptModuleClass, MKDEV(irrc_major, 0));	
	class_destroy(ptModuleClass);
	
	if (irrc_major != 0)
	{
		unregister_chrdev(irrc_major, "irrc");
		irrc_major = 0;
	}
	kfree(queue);
}

/*-------------------------------------------------------------------------------------*/
int __init mozart_irrc_init(void)
{
	int result;
	int reg;	//evelyn patched for enabling IrDA clock

#ifdef DEBUG	
	printk(KERN_EMERG "irrc.ko: infra red remote controller entries driver module\n");
#endif
	result = register_chrdev(irrc_major,"irrc",&mozart_irrc_fops);
	if (result < 0)
	{
		goto fail;
	}
	irrc_major = result;
	//irrc queue init
	queue = (struct irrc_queue *) kmalloc(sizeof(*queue), GFP_KERNEL);
	memset(queue, 0, sizeof(*queue));
	queue->head = queue->tail = 0;
	init_waitqueue_head(&queue->proc_list);

	/* Create a struct class structure */	
	ptModuleClass = class_create(THIS_MODULE, DEVICE_NAME);	
	if (IS_ERR(ptModuleClass)) {		
		goto fail;	
	}	
	device_create(ptModuleClass, NULL, MKDEV(irrc_major, 0), NULL, DEVICE_NAME);
	//evelyn patched for enabling IrDA clock
	//enable IrDA clock : SYSC_CLK_EN_CTRL_0
	reg = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24);
	reg |= 0x400000;
	writel(reg, IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x24);

	return 0;
fail:
	mozart_irrc_exit();
}

/*-------------------------------------------------------------------------------------*/
module_init(mozart_irrc_init);
module_exit(mozart_irrc_exit);
