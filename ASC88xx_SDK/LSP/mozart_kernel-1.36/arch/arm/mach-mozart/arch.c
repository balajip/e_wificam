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
#include <linux/platform_device.h>
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
#include <asm/mach/time.h>
#include <mach/lm.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <mach/spi-gpio.h>

#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach/mmc.h>
#include <mach/gmac.h>

extern void SetIRQmode(unsigned int IRQ, unsigned int edge);
extern void debug_puts(const char *s);

spinlock_t mozart_ugpio_lock = SPIN_LOCK_UNLOCKED;
u32 mozart_ugpio_requested = 0;
EXPORT_SYMBOL(mozart_ugpio_lock);
EXPORT_SYMBOL(mozart_ugpio_requested);

//spinlock_t AV_SYNC_LOCK = SPIN_LOCK_UNLOCKED;
//EXPORT_SYMBOL(AV_SYNC_LOCK ) ;
struct mutex AV_SYNC_LOCK;
EXPORT_SYMBOL(AV_SYNC_LOCK) ;

static struct map_desc mozart_io_desc[] __initdata = {
	/* ---- AHB ---- */
	{
		.virtual	= IO_ADDRESS(MOZART_AHBC_0_BASE),
		.pfn		= __phys_to_pfn(MOZART_AHBC_0_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_AHBC_1_BASE),
		.pfn		= __phys_to_pfn(MOZART_AHBC_1_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_AHBC_2_BASE),
		.pfn		= __phys_to_pfn(MOZART_AHBC_2_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_DDRSDMC_0_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_DDRSDMC_0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_DDRSDMC_1_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_DDRSDMC_1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_APBC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_APBC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_USB_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_USB_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_GMAC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_GMAC_MMR_BASE),
		.length		= SZ_8K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_DMAC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_DMAC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_VOC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_VOC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_VIC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_VIC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_NFC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_NFC_MMR_BASE),
		.length		= SZ_8K,
		.type		= MT_DEVICE
	},
	/* ---- AHB VMA ---- */
	{
		.virtual	= IO_ADDRESS(MOZART_DIE_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_DIE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_MEAE_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_MEAE_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},

	/* ---- APB ---- */
	{
		.virtual	= IO_ADDRESS(MOZART_INTC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_INTC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_WDTC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_WDTC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_TMRC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_TMRC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_GPIOC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_GPIOC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_AGPOC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_AGPOC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_UARTC0_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_UARTC0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_UARTC1_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_UARTC1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_UARTC2_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_UARTC2_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_UARTC3_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_UARTC3_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_I2S_0_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_I2S_0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_I2S_1_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_I2S_1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_I2S_2_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_I2S_2_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_SYSC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_SYSC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_SSI_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_SSI_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_IRDAC_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_IRDAC_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_PCIEC_SSC_BASE),
		.pfn		= __phys_to_pfn(MOZART_PCIEC_SSC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_PCIEC_DBI_BASE),
		.pfn		= __phys_to_pfn(MOZART_PCIEC_DBI_BASE),
		.length		= SZ_512K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_PCIEC_MEM_BASE),
		.pfn		= __phys_to_pfn(MOZART_PCIEC_MEM_BASE),
		.length		= SZ_16M,
		.type		= MT_DEVICE
	},

	/* ---- others ---- */
    //---------------------------------------------------------------
	{
		.virtual	= IO_ADDRESS(MOZART_SRAM_CTRL_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_SRAM_CTRL_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_SRAM_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_SRAM_MMR_BASE),
		.length		= SZ_8K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_MSHC0_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_MSHC0_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= IO_ADDRESS(MOZART_MSHC1_MMR_BASE),
		.pfn		= __phys_to_pfn(MOZART_MSHC1_MMR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
    //---------------------------------------------------------------
};
static struct platform_device *mozart_devices[ ] __initdata = {
#ifdef CONFIG_MMC_VPL_MSHC_0
&mozart_device_mmc0,
#endif
#ifdef CONFIG_MMC_VPL_MSHC_1
&mozart_device_mmc1,
#endif
&mozart_device_nand,
&mozart_device_i2c_bus_0,
&mozart_device_i2c_bus_1,
&mozart_device_spi,
&mozart_dw_spi_dev,
#if defined(CONFIG_INPUT_ADXL34X_I2C) || defined(CONFIG_INPUT_ADXL34X_I2C_MODULE) || defined (CONFIG_SENSORS_STK8312) || defined(CONFIG_SENSORS_STK8312_MODULE)
&mozart_gs,
#endif
&mozart_device_dwmac,
};

/* -------------------------------------------------------------------- */
static struct irq_chip mozart_intc = {
        .name = "MOZART_INTC",
	.ack = mozart_mask_ack_irq,
        .mask = mozart_mask_irq,
        .unmask = mozart_unmask_irq,
	.enable = mozart_unmask_irq,
	.disable = mozart_mask_irq,
};

static void mozart_init_irq(void)
{
	unsigned long flags;
	int irq;

	local_irq_save(flags);
	// interrupt controller initialization
	mozart_int_init();
	local_irq_restore(flags);
	for (irq = 0; irq < NR_IRQS; irq++)
	{
                set_irq_chip(irq, &mozart_intc);
                set_irq_handler(irq, handle_level_irq);
                set_irq_flags(irq, IRQF_VALID|IRQF_PROBE);
	}
}

/* -------------------------------------------------------------------- */
void __init mozart_map_io(void)
{
	iotable_init(mozart_io_desc, ARRAY_SIZE(mozart_io_desc));
}

static void __init mozart_fixup(struct machine_desc *desc,
			     struct tag *unused,
			     char **cmdline, struct meminfo *mi)
{
	mi->nr_banks = 1;
	mi->bank[0].start = DRAM_BASE;
	mi->bank[0].size = DRAM_SIZE;
	mi->bank[0].node = 0;
}

#ifdef CONFIG_RTC_DRV_S35390A
static struct i2c_board_info __initdata mozart_i2c_rtc = {
    I2C_BOARD_INFO("s35390a", 0x30),
    .irq    = 0,
};
#endif
#ifdef CONFIG_RTC_DRV_PCF8563
static struct i2c_board_info __initdata mozart_i2c_rtc = {
   I2C_BOARD_INFO("pcf8563", 0x51),
   .irq    = 0,
};
#endif
#ifdef CONFIG_AT24
static struct i2c_board_info __initdata mozart_i2c_at24 = {
	I2C_BOARD_INFO("24c16", 0x50),
};
#endif
/*
static struct i2c_board_info __initdata mozart_i2c_mt9p031 = {
    I2C_BOARD_INFO("mt9p031", 0x5d),
    .irq    = 0,
};
*/
	


#if defined(CONFIG_INPUT_ADXL34X) || defined(CONFIG_INPUT_ADXL34X_MODULE)
#include <linux/input/adxl34x.h>
	static const struct adxl34x_platform_data adxl34x_info = {
		.x_axis_offset = 0,
		.y_axis_offset = 0,
		.z_axis_offset = 0,
		.tap_threshold = 0x14,
		.tap_duration = 0x10,
		.tap_latency = 0x60,
		.tap_window = 0xF0,
		.tap_axis_control = ADXL_TAP_X_EN | ADXL_TAP_Y_EN | ADXL_TAP_Z_EN,
		.act_axis_control = 0xFF,
		.activity_threshold = 5,
		.inactivity_threshold = 3,
		.inactivity_time = 4,
		.free_fall_threshold = 0x7,
		.free_fall_time = 0x20,
		.data_rate = 0x4,
		.data_range = ADXL_FULL_RES,
	 
		.ev_type = EV_ABS,
		.ev_code_x = ABS_X, 	/* EV_REL */
		.ev_code_y = ABS_Y, 	/* EV_REL */
		.ev_code_z = ABS_Z, 	/* EV_REL */
	 
		.ev_code_tap = {BTN_TOUCH, BTN_TOUCH, BTN_TOUCH}, /* EV_KEY x,y,z */
	 
	/*	.ev_code_ff = KEY_F,*/		/* EV_KEY */
	/*	.ev_code_act_inactivity = KEY_A,*/	/* EV_KEY */
		.power_mode = ADXL_AUTO_SLEEP | ADXL_LINK,
		.fifo_mode = ADXL_FIFO_STREAM,
		.orientation_enable = ADXL_EN_ORIENTATION_3D,
		.deadzone_angle = ADXL_DEADZONE_ANGLE_10p8,
		.divisor_length = ADXL_LP_FILTER_DIVISOR_16,
		/* EV_KEY {+Z, +Y, +X, -X, -Y, -Z} */
		.ev_codes_orient_3d = {BTN_Z, BTN_Y, BTN_X, BTN_A, BTN_B, BTN_C},
	};
#endif


static struct i2c_board_info __initdata mozart_i2c_gs_info[] = {
#if defined(CONFIG_INPUT_ADXL34X_I2C) || defined(CONFIG_INPUT_ADXL34X_I2C_MODULE)
		{
			I2C_BOARD_INFO("adxl34x", 0x53),
			.irq = GPIOC_IRQ_NUM ,
			.platform_data = (void *)&adxl34x_info,
		},
#endif
#if defined(CONFIG_SENSORS_STK8312) || defined(CONFIG_SENSORS_STK8312_MODULE)
		{
			I2C_BOARD_INFO("stk831x", 0x3D),
			.irq = GPIOC_IRQ_NUM ,
		},
#endif

};

static struct spi_board_info __initdata mozart_spi_devs[] = {
	[0] = {
		.modalias   = "imx035",
		.bus_num    = 0x0,
		.chip_select    = 0,
		.mode       = SPI_MODE_3,	/* CPOL=1, CPHA=1 */
		.max_speed_hz   = 100000,
	},

};

static void __init mozart_init(void)
{
#if CONFIG_USB
	struct lm_device *lmdev;
	lmdev = kmalloc(sizeof(struct lm_device), GFP_KERNEL);
	if (!lmdev)
	{
		printk("Fail\n");
	}
	memset(lmdev, 0, sizeof(struct lm_device));

	lmdev->resource.start = VPL_USBC_MMR_BASE;
	lmdev->resource.end = lmdev->resource.start + SZ_256K - 1;
	lmdev->resource.flags = IORESOURCE_MEM;
	lmdev->irq = USBC_IRQ_NUM;
	lmdev->id = 0;

	lm_device_register(lmdev);
	printk("[jon]lm device registered!\n");
#endif
	mozart_gmac_init();
	platform_add_devices( mozart_devices, ARRAY_SIZE( mozart_devices ) );
#if defined(CONFIG_RTC_DRV_PCF8563) || defined(CONFIG_RTC_DRV_S35390A)
	i2c_register_board_info(1, &mozart_i2c_rtc, 1);//busnum, info, number of descriptor
#endif
//	i2c_register_board_info(0, &mozart_i2c_mt9p031, 1);
#ifdef CONFIG_AT24
	i2c_register_board_info(0, &mozart_i2c_at24, 1);//busnum, info, number of descriptor
#endif

#if defined(CONFIG_INPUT_ADXL34X_I2C) || \
	defined(CONFIG_INPUT_ADXL34X_I2C_MODULE) || \
	defined (CONFIG_SENSORS_STK8312) || \
	defined(CONFIG_SENSORS_STK8312_MODULE)
	
	i2c_register_board_info(1, mozart_i2c_gs_info,
				ARRAY_SIZE(mozart_i2c_gs_info));
#endif
	spi_register_board_info(mozart_spi_devs, ARRAY_SIZE(mozart_spi_devs));

	//initialize
	mutex_init(&AV_SYNC_LOCK);

	/* UART clock source */
	if (readl(IO_ADDRESS(VPL_SYSC_MMR_BASE) + 0x14) == 0x03000000) // Mozart v3
	{
#define I2SSC_PLL_REF_SEL_SHIFT   (25)
#define I2SSC_PLL_REF_SEL(x)      ((x) << I2SSC_PLL_REF_SEL_SHIFT)
		int val = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE) + 0x48);
		if (CONFIG_UART_CLK == 18432000)
			writel(val & ~I2SSC_PLL_REF_SEL(1), IO_ADDRESS(VPL_SYSC_MMR_BASE) + 0x48);
		else if (CONFIG_UART_CLK == 24000000)
			writel(val | I2SSC_PLL_REF_SEL(1), IO_ADDRESS(VPL_SYSC_MMR_BASE) + 0x48);
	}
}
/* -------------------------------------------------------------------- */

static irqreturn_t mozart_timer_interrupt(int irq, void *dev_id)
{
	timer_tick();

	return IRQ_HANDLED;
}

static struct irqaction mozart_timer_irq = {
	.name		= "Mozart Timer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= mozart_timer_interrupt
};

extern unsigned long (*gettimeoffset)(void);

static void Mozart_setup_timer(u32 timer)
{
	unsigned long irq;
    unsigned long div_cnt, flag_cnt ;

	TMRC_Disable(timer);
#ifdef TIMER_INC_MODE
    TMRC_SetAutoReload(timer, 0xffffffff - APB_CLK/HZ);
    TMRC_SetCounter(timer, 0xffffffff - APB_CLK/HZ);
#else
    TMRC_SetAutoReload(timer, (APB_CLK)/HZ);
    TMRC_SetCounter(timer, (APB_CLK)/HZ);
#endif

    if (TMRC_Ctrl_Config(timer,1)!=TRUE) {
        panic("Can not configure timer to down counter\n");
    }

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
	case 3:
		irq = TMRC_TM3_IRQ_NUM;
		break;
	default:
		while(1);	// hang
    }

    printk("IRQ timer at interrupt number 0x%02lx clock %d\r\n",irq,APB_CLK);
	SetIRQmode(irq, 1); //set edge trigger
    //debug_puts("setup_timer(): setup_irq()\n");
    setup_irq(irq, &mozart_timer_irq);

    if (TMRC_Enable(timer)!=TRUE) {
        panic("Can not enable timer\n");
    }

/*
 * AHBCLK = 200/150/133MHz : official clock value.
 * AHBCLK = 166/160MHz : experimental clock value.
 */
#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
// in James' design...
// 	if (APBCLK==100MHz)	
//		Tick of SYS_CNT (64 bit timer) is 1 usec
// 	else /* (APBCLK==166MHz) || (APBCLK==133MHz) || (APBCLK==160MHz) || (APBCLK==150MHz) ...
//		Tick of SYS_CNT (64 bit timer) is 1 APBCLK CYCLE
#if (AHB_CLK == 200000000)
	div_cnt = APB_CLK/1000000 ;//APB_CLK divide div_cnt to get 1usec
//#elif ((AHB_CLK == 166666666) || (AHB_CLK == 160000000) || (AHB_CLK == 150000000) || (AHB_CLK == 133333333))
#else
    div_cnt = 1 ;//jam test
#endif

	flag_cnt = (1<<31) | (div_cnt-1) ;//bit#31 = 1 can clear counter
    v_outl( IO_ADDRESS((VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_CTRL)), flag_cnt) ;
}
static void mozart_timer_init(void)
{
#if defined(CONFIG_MOZART_TMR0)
	Mozart_setup_timer(0);
#elif defined(CONFIG_MOZART_TMR1)
	Mozart_setup_timer(1);
#elif defined(CONFIG_MOZART_TMR2)
	Mozart_setup_timer(2);
#elif defined(CONFIG_MOZART_TMR3)
	Mozart_setup_timer(3);
#endif
}

static struct sys_timer mozart_timer = {
        .init           = mozart_timer_init,
        .offset         = VPL_TMRC_gettimeoffset,
};

MACHINE_START(MOZART, "Mozart")
	.phys_io	= MOZART_UARTC0_MMR_BASE,
	.io_pg_offst	= ((IO_ADDRESS(MOZART_UARTC0_MMR_BASE)) >> 18) & 0xfffc,
	.boot_params	= 0x00000100,
	.fixup		= mozart_fixup,
	.map_io		= mozart_map_io,
	.init_irq	= mozart_init_irq,
	.timer		= &mozart_timer,
	.init_machine	= mozart_init,
MACHINE_END
