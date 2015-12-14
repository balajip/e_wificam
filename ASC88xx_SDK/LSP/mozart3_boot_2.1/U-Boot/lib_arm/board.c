/*
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * To match the U-Boot user interface on ARM platforms to the U-Boot
 * standard (as on PPC platforms), some messages with debug character
 * are removed from the default U-Boot build.
 *
 * Define DEBUG here if you want additional info as shown below
 * printed upon startup:
 *
 * U-Boot code: 00F00000 -> 00F3C774  BSS: -> 00FC3274
 * IRQ Stack: 00ebff7c
 * FIQ Stack: 00ebef7c
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <timestamp.h>
#include <version.h>
#include <net.h>
#include <serial.h>
#include <nand.h>
#include <mmc.h>
#include <onenand_uboot.h>

#include "../drivers/net/gmac.h"
#include <asm/arch/serialflash.h>

#ifdef CONFIG_DRIVER_SMC91111
#include "../drivers/net/smc91111.h"
#endif
#ifdef CONFIG_DRIVER_LAN91C96
#include "../drivers/net/lan91c96.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

ulong monitor_flash_len;

#ifdef CONFIG_HAS_DATAFLASH
extern int  vpl_SerialFlashInit(void);
extern void dataflash_print_info(void);
#endif

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" U_BOOT_DATE " - " U_BOOT_TIME ")"CONFIG_IDENT_STRING;

unsigned long malloc_start ;
unsigned long malloc_end ;
unsigned long stack_start ;
unsigned long stack_end ;
volatile unsigned long DMA_BUFFER_BASE ;//GMAC
unsigned long gmac_dma_buf_end ;

#ifdef CONFIG_DRIVER_CS8900
extern void cs8900_get_enetaddr (void);
#endif

#ifdef CONFIG_DRIVER_RTL8019
extern void rtl8019_get_enetaddr (uchar * addr);
#endif

#if defined(CONFIG_HARD_I2C) || \
    defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#endif

#include <asm/arch/platform.h>

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static ulong mem_malloc_start = 0;
static ulong mem_malloc_end = 0;
static ulong mem_malloc_brk = 0;

static
void mem_malloc_init (ulong dest_addr)
{
	mem_malloc_start = dest_addr;
	mem_malloc_end = dest_addr + CONFIG_SYS_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset ((void *) mem_malloc_start, 0,
			mem_malloc_end - mem_malloc_start);

}

void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;

	return ((void *) old);
}


/************************************************************************
 * Coloured LED functionality
 ************************************************************************
 * May be supplied by boards if desired
 */
void inline __coloured_LED_init (void) {}
void coloured_LED_init (void) __attribute__((weak, alias("__coloured_LED_init")));
void inline __red_LED_on (void) {}
void red_LED_on (void) __attribute__((weak, alias("__red_LED_on")));
void inline __red_LED_off(void) {}
void red_LED_off(void)	     __attribute__((weak, alias("__red_LED_off")));
void inline __green_LED_on(void) {}
void green_LED_on(void) __attribute__((weak, alias("__green_LED_on")));
void inline __green_LED_off(void) {}
void green_LED_off(void)__attribute__((weak, alias("__green_LED_off")));
void inline __yellow_LED_on(void) {}
void yellow_LED_on(void)__attribute__((weak, alias("__yellow_LED_on")));
void inline __yellow_LED_off(void) {}
void yellow_LED_off(void)__attribute__((weak, alias("__yellow_LED_off")));

/************************************************************************
 * Init Utilities							*
 ************************************************************************
 * Some of this code should be moved into the core functions,
 * or dropped completely,
 * but let's get it working (again) first...
 */

#if defined(CONFIG_ARM_DCC) && !defined(CONFIG_BAUDRATE)
#define CONFIG_BAUDRATE 115200
#endif

static int init_baudrate (void)
{
	gd->bd->bi_baudrate = gd->baudrate = 38400 ;

	return (0);
}

static int display_banner (void)
{
	printf ("\n\n%s\n\n", version_string);
	//printf ("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",  _armboot_start, _bss_start, _bss_end);
#ifdef CONFIG_MODEM_SUPPORT
	debug ("Modem Support enabled\n");
#endif
/*#ifdef CONFIG_USE_IRQ
	debug ("IRQ Stack: %08lx\n", IRQ_STACK_START);
	debug ("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif*/

	return (0);
}

/*
 * WARNING: this code looks "cleaner" than the PowerPC version, but
 * has the disadvantage that you either get nothing, or everything.
 * On PowerPC, you might see "DRAM: " before the system hangs - which
 * gives a simple yet clear indication which part of the
 * initialization if failing.
 */
static int display_dram_config (void)
{
	int i;

	malloc_start = _bss_end ;
	malloc_end   = malloc_start + CONFIG_SYS_MALLOC_LEN;
	stack_start  = malloc_end + CONFIG_SYS_GBL_DATA_SIZE;
	stack_end    = stack_start + CONFIG_STACKSIZE + 12;
#ifdef CONFIG_USE_IRQ
	stack_end    += CONFIG_STACKSIZE_IRQ + CONFIG_STACKSIZE_FIQ;
#endif
	DMA_BUFFER_BASE = ((stack_end / 32) + 1) * 32 ;//3//32 bytes aligned
	gmac_dma_buf_end  = DMA_BUFFER_BASE + (sizeof(TDmaDesc)*DESC_COUNT) + (DMA_BUFFER_SIZE*DESC_COUNT);
	puts ("=== U-Boot DDR Reserved Area ===\n");
	printf("  !! Please do not write any data to these locations. !!\n") ;
	printf("  TEXT section  : 0x%08lx -> 0x%08lx\n", _armboot_start, _bss_start);
	printf("  BSS  section  : 0x%08lx -> 0x%08lx\n", _bss_start, _bss_end);
	printf("  MALLOC Area   : 0x%08lx -> 0x%08lx\n", malloc_start, malloc_end) ;
	printf("  STACK         : 0x%08lx -> 0x%08lx\n", stack_start, stack_end) ;
	printf("  GMAC Desc/Buf : 0x%08lx -> 0x%08lx\n", DMA_BUFFER_BASE, gmac_dma_buf_end) ;
	printf("\n") ;

	puts ("=== DRAM Configuration ===\n");
	for(i=0; i<CONFIG_DRAM_NUMBER; i++) {
		if (gd->bd->bi_dram[i].size)
		{
			printf ("  Bank #%d      : %#010lx ", i, gd->bd->bi_dram[i].start);
			print_size (gd->bd->bi_dram[i].size, "\n");
		}
	}

	return (0);
}

#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}
#endif /* CONFIG_SYS_NO_FLASH */
#if 0
static int init_func_i2c (void)
{
	puts ("I2C:   ");
	i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	puts ("ready\n");
	return (0);
}
#endif

#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
#include <pci.h>
static int arm_pci_init(void)
{
	pci_init();
	return 0;
}
#endif /* CONFIG_CMD_PCI || CONFIG_PCI */

static int mshc_pullup_config (void)
{
	unsigned long reg_val = 0 ;

	reg_val = v_inl(EVM_SYSC_BASE + EVM_SYSC_PULL_CTRL) ;
#ifdef CONFIG_SOC_MOZART
	reg_val |= (0x7E) ; //Enable bit#1~bit#6 of all MSHC related pull-up option.
#elif defined(CONFIG_SOC_BEETHOVEN)
	reg_val |= (0xE) ; //Enable bit#1~bit#3 of all MSHC related pull-up option.
#endif
	v_outl(EVM_SYSC_BASE + EVM_SYSC_PULL_CTRL, reg_val);

	return 0 ;
}

/*
 * Breathe some life into the board...
 *
 * Initialize a serial port as console, and carry out some hardware
 * tests.
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependent #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

int print_cpuinfo (void);

init_fnc_t *init_sequence[] = {
	cpu_init,		/* basic cpu dependent setup */
	board_init,		/* basic board dependent setup */
	interrupt_init,		/* set up exceptions */
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,		/* stage 1 init of console */
	display_banner,		/* say that we are here */
#if defined(CONFIG_DISPLAY_CPUINFO)
	//print_cpuinfo,		/* display cpu info (and speed) */
#endif
#if defined(CONFIG_DISPLAY_BOARDINFO)
	//checkboard,		/* display board info */
#endif
	dram_init,		/* configure available RAM banks */
	display_dram_config,
	mshc_pullup_config,
	NULL,
};

#ifdef CONFIG_QUICK_TEST
void autotest_main_func(void);
#endif
extern int init_spi_flash_array(void) ;
extern void verify_the_modelname(void);
void start_armboot (void)
{
	init_fnc_t **init_fnc_ptr;
	char *s;

	gd = (gd_t*)(_bss_end + CONFIG_SYS_MALLOC_LEN);
	__asm__ __volatile__("": : :"memory");
	memset ((void*)gd, 0, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd + sizeof(gd_t));
	memset (gd->bd, 0, sizeof (bd_t));

	monitor_flash_len = _bss_start - _armboot_start;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}

	/* armboot_start is defined in the board-specific linker script */
	mem_malloc_init (_bss_end);

	//Set the min drv trength of NFC(2mA).
	unsigned long drv_strength = v_inl(EVM_SYSC_BASE + 0x5C) ;
	drv_strength &= ~(0x3 << 8) ;
	v_outl(EVM_SYSC_BASE + 0x5C, drv_strength) ;

#if defined(CONFIG_CMD_NAND)
	puts ("\n=== NAND Configuration ===\n");
	nand_init();		// go init the NAND
#endif
	//printf("SOR TEST!!\n") ;
	//unsigned long paden = v_inl(0x4f800044) ;
	//paden |= 1 << 12 ;
	//v_outl(0x4f800044, paden) ;

#ifdef CONFIG_CMD_SF
	printf("\n=== SPI FLASH Configuration ===\n");
	init_spi_flash_array() ;
#endif

#ifdef CONFIG_GENERIC_MMC
/*
 * MMC initialization is called before relocating env.
 * Thus It is required that operations like pin multiplexer
 * be put in board_init.
 */
	//puts ("MMC:   ");
	mmc_initialize ();
#endif

	/* initialize environment */
	printf("\n=== Boot Configuration ===\n");
	env_relocate ();

	/* IP Address */
	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");

	devices_init ();	/* get the devices list going. */

	jumptable_init ();

#if defined(CONFIG_API)
	/* Initialize API */
	printf( "api_init?\n" ) ;
	api_init ();
#endif

	/* enable exceptions */
	enable_interrupts ();

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}

#if defined(CONFIG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif

#ifdef BOARD_LATE_INIT
	board_late_init ();
#endif

#if defined(CONFIG_CMD_NET)

	printf("\n=== Net Configuration  ===\n");
	eth_initialize(gd->bd);

#if defined(CONFIG_RESET_PHY_R)
	debug ("Reset Ethernet PHY\n");
	reset_phy();
#endif
#endif

#ifdef CONFIG_QUICK_TEST
	if ((s = getenv ("auto_quicktest")) != NULL && *s == '1' )
	{
		autotest_main_func() ;
	}
#endif

#ifdef CONFIG_MODELNAME
	verify_the_modelname() ;
#endif

	puts ("\n=== Loader Info ===\n");
	printf("  Loader Version# : 0x%08lx (This value is stored at 0x%08lx)\n", v_inl(LOADER_VERSION_OFFSET), (unsigned long)LOADER_VERSION_OFFSET) ;

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop ();
	}

	/* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}

#ifdef CONFIG_MODEM_SUPPORT
static inline void mdm_readline(char *buf, int bufsiz);

/* called from main loop (common/main.c) */
extern void  dbg(const char *fmt, ...);
int mdm_init (void)
{
	char env_str[16];
	char *init_str;
	int i;
	extern char console_buffer[];
	extern void enable_putc(void);
	extern int hwflow_onoff(int);

	enable_putc(); /* enable serial_putc() */

#ifdef CONFIG_HWFLOW
	init_str = getenv("mdm_flow_control");
	if (init_str && (strcmp(init_str, "rts/cts") == 0))
		hwflow_onoff (1);
	else
		hwflow_onoff(-1);
#endif

	for (i = 1;;i++) {
		sprintf(env_str, "mdm_init%d", i);
		if ((init_str = getenv(env_str)) != NULL) {
			serial_puts(init_str);
			serial_puts("\n");
			for(;;) {
				mdm_readline(console_buffer, CONFIG_SYS_CBSIZE);
				dbg("ini%d: [%s]", i, console_buffer);

				if ((strcmp(console_buffer, "OK") == 0) ||
					(strcmp(console_buffer, "ERROR") == 0)) {
					dbg("ini%d: cmd done", i);
					break;
				} else /* in case we are originating call ... */
					if (strncmp(console_buffer, "CONNECT", 7) == 0) {
						dbg("ini%d: connect", i);
						return 0;
					}
			}
		} else
			break; /* no init string - stop modem init */

		udelay(100000);
	}

	udelay(100000);

	/* final stage - wait for connect */
	for(;i > 1;) { /* if 'i' > 1 - wait for connection
				  message from modem */
		mdm_readline(console_buffer, CONFIG_SYS_CBSIZE);
		dbg("ini_f: [%s]", console_buffer);
		if (strncmp(console_buffer, "CONNECT", 7) == 0) {
			dbg("ini_f: connected");
			return 0;
		}
	}

	return 0;
}

/* 'inline' - We have to do it fast */
static inline void mdm_readline(char *buf, int bufsiz)
{
	char c;
	char *p;
	int n;

	n = 0;
	p = buf;
	for(;;) {
		c = serial_getc();

		/*		dbg("(%c)", c); */

		switch(c) {
		case '\r':
			break;
		case '\n':
			*p = '\0';
			return;

		default:
			if(n++ > bufsiz) {
				*p = '\0';
				return; /* sanity check */
			}
			*p = c;
			p++;
			break;
		}
	}
}
#endif	/* CONFIG_MODEM_SUPPORT */
