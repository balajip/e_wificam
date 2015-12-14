/*
 *  linux/arch/arm/mach-cpe/arch.c
 *
 *  Architecture specific fixups.  This is where any
 *  parameters in the params struct are fixed up, or
 *  any additional architecture specific information
 *  is pulled from the params struct.
 */
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/device.h>

#include <asm/elf.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/irq.h>
#include <mach/time.h>
#include <mach/platform.h>
#include <mach/hardware.h>
#include <asm/sizes.h>
#include <asm/mach/map.h>
#include <mach/time.h>

//#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
//#include <asm/mach/map.h>
#include <asm/mach/mmc.h>


//#include <asm/mach/lm.h>
extern void SetIRQmode(unsigned int IRQ, unsigned int edge);
extern void debug_puts(const char *s);
//include <asm/arch/haydn/pci.h>
extern void genarch_init_irq(void);

static struct map_desc haydn_io_desc[] __initdata = {

#if 1	/* ---- AHB ---- */

	{
		.virtual	= IO_ADDRESS(HAYDN_AHBC_0_BASE),
		.pfn		= __phys_to_pfn(HAYDN_AHBC_0_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_SMC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_SMC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_DDRSDMC_0_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_DDRSDMC_0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_APBC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_APBC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_USB_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_USB_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_MAC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_MAC_MMR_BASE),
		.length		= SZ_8K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_AHBC_1_BASE),
		.pfn		= __phys_to_pfn(HAYDN_AHBC_1_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_DDRSDMC_1_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_DDRSDMC_1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_DMAC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_DMAC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_VIC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_VIC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
	/* ---- AHB VMA ---- */
	{
		.virtual	= IO_ADDRESS(HAYDN_DIE_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_DIE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_MEAE_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_MEAE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_MEBE_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_MEBE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_MME_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_MME_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_IPE_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_IPE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_MDBE_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_MDBE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
	/* ---- APB ---- */
	{
		.virtual	= IO_ADDRESS(HAYDN_INTC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_INTC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_WDTC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_WDTC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_TMRC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_TMRC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_RTCC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_RTCC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_GPIOC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_GPIOC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_UARTC0_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_UARTC0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_UARTC1_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_UARTC1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_I2C_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_I2C_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_I2S_0_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_I2S_0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_I2S_1_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_I2S_1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_PS2IC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_PS2IC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_I2S_2_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_I2S_2_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(HAYDN_SYSC_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_SYSC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},{
		.virtual	= IO_ADDRESS(HAYDN_SSI_MMR_BASE),
		.pfn		= __phys_to_pfn(HAYDN_SSI_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
	/* ---- others ---- */
/*	{

		.virtual	= PCI_IO_VADDR,
		.pfn		= __phys_to_pfn(PHYS_PCI_IO_BASE),
		.length		= SZ_64K-SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= PCI_MEMORY_VADDR,
		.pfn		= __phys_to_pfn(PHYS_PCI_MEM_BASE),
		.length		= SZ_1M, // prefetchable + non-prefetchable size
		.type		= MT_DEVICE
	}, {

		.virtual	= IO_ADDRESS(HAYDN_FLASH_BASE),
		.pfn		= __phys_to_pfn(HAYDN_FLASH_BASE),
		.length		= HAYDN_FLASH_SIZE,
		.type		= MT_MEMORY
	}
*/
#endif
};

/* -------------------------------------------------------------------- */
static struct irq_chip haydn_intc = {
        .name = "HAYDN_INTC",
	.ack = haydn_mask_ack_irq,
        .mask = haydn_mask_irq,
        .unmask = haydn_unmask_irq,
//        .retrigger = NULL,
//        .type = NULL,
//        .wake = NULL,
//jon
		.enable = haydn_unmask_irq,
		.disable = haydn_mask_irq,
};

static void haydn_init_irq(void)
{
	unsigned long flags;
	int irq;

	//debug_puts("haydn_init_irq()\n");
	local_irq_save(flags);
	// interrupt controller initialization
	haydn_int_init();
	local_irq_restore(flags);
/*
	for (irq = 0; irq < NR_IRQS; irq++) 
	{
		irq_desc[irq].valid	= 1;
		irq_desc[irq].probe_ok	= 1;
		irq_desc[irq].mask_ack	= haydn_mask_ack_irq;
		irq_desc[irq].mask	= haydn_mask_irq;
		irq_desc[irq].unmask	= haydn_unmask_irq;
	}
*/
	for (irq = 0; irq < NR_IRQS; irq++) 
	{
                set_irq_chip(irq, &haydn_intc);
                //set_irq_handler(irq, do_edge_IRQ);
                set_irq_handler(irq, handle_level_irq);
                //set_irq_handler(irq, do_level_IRQ);
                //set_irq_flags(irq, IRQF_VALID|IRQF_PROBE|IRQF_NOAUTOEN);
                set_irq_flags(irq, IRQF_VALID|IRQF_PROBE);
	}
}

/* -------------------------------------------------------------------- */
static struct semaphore semaSharedBus;

void SharedBus_LooseBus(void)
{
	up(&semaSharedBus);
}

int SharedBus_HoldBus(void)
{
	if (down_interruptible(&semaSharedBus))
		return -ERESTARTSYS;

	return 0;
}

static void SharedBus_Init(void)
{
	/* Initiate a semaphore for shared bus */
	sema_init(&semaSharedBus, 1);
}

/* -------------------------------------------------------------------- */
void __init haydn_map_io(void)
{
	printk("[jon]map io:(0x%x -> 0x%x)\n", HAYDN_INTC_MMR_BASE, IO_ADDRESS(HAYDN_INTC_MMR_BASE));
	iotable_init(haydn_io_desc, ARRAY_SIZE(haydn_io_desc));
}

//static void __init fixup_haydn(struct machine_desc *desc,
//			     struct param_struct *unused,
//			     char **cmdline, struct meminfo *mi)
static void __init haydn_fixup(struct machine_desc *desc,
			     struct tag *unused,
			     char **cmdline, struct meminfo *mi)
{
	mi->nr_banks = 1;
	mi->bank[0].start = DRAM_BASE;
	mi->bank[0].size = DRAM_SIZE;
	mi->bank[0].node = 0;

	/* Set something for shared bus */
	SharedBus_Init();
}

static void haydn_init(void)
{
#if 0
	struct lm_device *lmdev;
	lmdev = kmalloc(sizeof(struct lm_device), GFP_KERNEL);
	if (!lmdev)
	{
		printk("Fail\n");
	}
	memset(lmdev, 0, sizeof(struct lm_device));

	lmdev->resource.start = 0x98400000;
	lmdev->resource.end = lmdev->resource.start + 0x40000 - 1 /*0x0fffffff*/;
	lmdev->resource.flags = IORESOURCE_MEM;
	lmdev->irq = 1;
	lmdev->id = 0;

	lm_device_register(lmdev);
	printk("[jon]lm device registered!\n");
#endif
	printk("[jon]machine init!\n");
}
/* -------------------------------------------------------------------- */
//extern unsigned long (*gettimeoffset)(void);
//unsigned long haydn_count = 0;
static irqreturn_t haydn_timer_interrupt(int irq, void *dev_id)
{
    u32 timer;
	//jon for measure ISR delay
	volatile u32 tmp = 0;

	//jon test
	//printk("pid:%x\n", current->pid);	
    //write_seqlock(&xtime_lock);
	//jon for measure ISR delay
	/*
	tmp = (u32)*((u32 *)0xf4800018);
	printk("[%x]\n", tmp);
	tmp = (u32)*((volatile u32 *)0xf4800004);
	tmp &= ~(0x00000010);
	(u32)*((volatile u32 *)0xf4800004) = tmp;
	(u32)*((volatile u32 *)0xf4800018) = 0x0;
	*/
/*jon
    switch (irq) {
	case TMRC_TM2_IRQ_NUM:
	    timer = 2;
	    break;
	case TMRC_TM1_IRQ_NUM:
	    timer = 1;
	    break;
	case TMRC_TM0_IRQ_NUM:
	    timer = 0;
	    break;
	default:
            write_sequnlock(&xtime_lock);
            return IRQ_HANDLED;
    }
*/
    // clear the interrupt
    //jon TMRC_InterruptDisable(timer);
   // *(unsigned int *)(0xF9840034) = 0;
	//haydn_count++;
	//if(haydn_count >= 1000){
		//debug_puts(".");
		//haydn_count = 0;
	//}
//debug_puts(".");
	/*
	 * the clock tick routines are only processed on the
	 * primary CPU
	 */
	timer_tick();



	//write_sequnlock(&xtime_lock);

	return IRQ_HANDLED;
}
/*
static irqreturn_t haydn_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
//static int haydn_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    do_timer(regs);
//#ifdef CONFIG_FARADAY_RTC
//    do_set_rtc();
//#endif
        return IRQ_HANDLED;
}
*/
static struct irqaction haydn_timer_irq = {
	.name		= "Haydn Timer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= haydn_timer_interrupt
};

extern unsigned long (*gettimeoffset)(void);
#if 1
static void Haydn_setup_timer(u32 timer)
{
	unsigned long irq;

	debug_puts("[arch]setup timer\n");
#ifdef TIMER_INC_MODE
    TMRC_SetAutoReload(timer, 0xffffffff - APB_CLK/HZ);
    TMRC_SetCounter(timer, 0xffffffff - APB_CLK/HZ);
#else
    TMRC_SetAutoReload(timer, APB_CLK/HZ);
    TMRC_SetCounter(timer, APB_CLK/HZ);
#endif

    /* aren 20040512 */

    if (TMRC_Ctrl_Config(timer,1)!=TRUE) {
        panic("Can not configure timer to down counter\n");
    }

    /* aren 20040512 */
    switch (timer) {
	case 0:
		irq = TMRC_TM0_IRQ_NUM;
		break;
	case 1:
		irq = TMRC_TM1_IRQ_NUM;
		break;
	case 2:
		irq = TMRC_TM2_IRQ_NUM;
		break;
	default:
		while(1);	// hang
    }

    printk("IRQ timer at interrupt number 0x%02lx clock %d\r\n",irq,APB_CLK);
    //fLib_SetIntTrig(irq, EDGE, L_ACTIVE);
	SetIRQmode(irq, 1); //set edge trigger
    debug_puts("setup_timer(): setup_irq()\n");
    setup_irq(irq, &haydn_timer_irq);
//    timer_irq.handler = haydn_timer_interrupt;
//    timer_irq.flags = SA_INTERRUPT;
//    gettimeoffset = VPL_TMRC_gettimeoffset;

    if (TMRC_Enable(timer)!=TRUE) {
    	    debug_puts("init timer fail!\n");
	    panic("Can not enable timer\n");
    }

}
static void haydn_timer_init(void)
{
	debug_puts("[arch]init timer 2\n");
	Haydn_setup_timer(2);
	debug_puts("[arch]done\n");
}
#endif

#if 0
static void haydn_timer_init(void)
{
	*((unsigned long *)0xF9840020) = (APB_CLK/HZ);
	*((unsigned long *)0xF9840024) = (APB_CLK/HZ);

	fLib_SetIntTrig(8, EDGE, L_ACTIVE);
	
    if(setup_irq(8, &haydn_timer_irq))
    {
		debug_puts("request irq8 fail!\n");
	}
	
	*((unsigned long *)0xF9840030) = 0x00000140;
}
#endif
static struct sys_timer haydn_timer = {
        .init           = haydn_timer_init,
        .offset         = VPL_TMRC_gettimeoffset,
};

MACHINE_START(MOZART, "VN Haydn")
	/* Maintainer: VN Inc. */
	.phys_io	= HAYDN_UARTC1_MMR_BASE,
	.io_pg_offst	= ((IO_ADDRESS(HAYDN_UARTC1_MMR_BASE)) >> 18) & 0xfffc,
	.boot_params	= 0x00000100,
	.fixup		= haydn_fixup,
	.map_io		= haydn_map_io,
	.init_irq	= haydn_init_irq,
	.timer		= &haydn_timer,
	.init_machine	= haydn_init,
MACHINE_END
/*
MACHINE_START(HAYDN, "VN HAYDN")
    MAINTAINER("VN")
    BOOT_MEM(0x00000000, HAYDN_UARTC0_MMR_BASE, IO_ADDRESS(HAYDN_UARTC0_MMR_BASE))
    BOOT_PARAMS(0x00000100)
    FIXUP(fixup_haydn)
    MAPIO(haydn_map_io)
    //INITIRQ(genarch_init_irq)
    INITIRQ(haydn_init_irq)
    //.timer = &haydn_timer,
    INITTIME(haydn_time_init)
    //.init_time	= haydn_time_init,
MACHINE_END
*/
