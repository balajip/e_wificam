#ifndef __CONFIG_H
#define __CONFIG_H

/* ARM asynchronous clock */
#define CONFIG_SYS_HZ		1000
#define CONFIG_ARM926EJS	1	/* This is an ARM926EJS Core	*/
#define CONFIG_SOC_HAYDN 1

/*==========================================================*
 *===                  Init Parts                        ===*
 *==========================================================*/
/* In start.S */
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SKIP_RELOCATE_UBOOT
#define ROUND(A, B)	(((A) + (B)) & ~((B) - 1))

/* malloc size, stack size */
#define CONFIG_SYS_MALLOC_LEN		ROUND(3 * CONFIG_ENV_SIZE + 128*1024, 0x1000)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* 128 bytes for initial data */
#define CONFIG_STACKSIZE	(200*1024)	/* regular stack */

/* IRQ */
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_QUICK_TEST
#ifdef CONFIG_QUICK_TEST
#define CONFIG_STACKSIZE_IRQ		(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ		(4*1024)	/* FIQ stack */
#define CONFIG_USE_IRQ

//Test items
#define CONFIG_QUICK_TEST_SPI
#define CONFIG_QUICK_TEST_UART
#define CONFIG_QUICK_TEST_DRAM
#define CONFIG_QUICK_TEST_TIMER
#define CONFIG_QUICK_TEST_WDT
#define CONFIG_QUICK_TEST_GMAC
#define CONFIG_QUICK_TEST_GPIO
#else
#undef CONFIG_USE_IRQ			
#endif

/*==========================================================*
 *===                    Commands                        ===*
 *==========================================================*/
#include <config_cmd_default.h>
//[trace] some useless macros are defined in config_cmd_default.h, 
//        we should undef them here.
#undef CONFIG_CMD_BDI  
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_ITEST
#undef CONFIG_CMD_LOADB 
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_SETGETDCR
#undef CONFIG_CMD_SOURCE  // should be check later, it runs script
#undef CONFIG_CMD_XIMG
#undef CONFIG_CMD_PING
#define CONFIG_CMD_SF 1

/*==========================================================*
 *===                  Hardware drivers                  ===*
 *==========================================================*/
/********* UART *********/
#undef CONFIG_USART0
#undef CONFIG_USART1
#undef CONFIG_USART2
#define CONFIG_USART3		1	/* USART 3 is DBGU */

/********* DRAM *********/
#define CONFIG_NR_DRAM_BANKS		 1
#define PHYS_SDRAM			  0xa00000
#ifdef CONFIG_CMD_MTEST
#define PHYS_SDRAM_SIZE		0x04000000	/* 64 megs */
#define CONFIG_SYS_ALT_MEMTEST //[jam trace] if we need more complete test, we should define CONFIG_SYS_ALT_MEMTEST.
#define CONFIG_SYS_MEMTEST_START		PHYS_SDRAM
#define CONFIG_SYS_MEMTEST_END			0xb00000
#endif

/********* SPI Flash-Related *********/
#define CONFIG_SYS_NO_FLASH	    1 //we do not have nor flash
#define CONFIG_DW_SPI       1 //This is SPI interface
#define CONFIG_SPI_FLASH        1 //This is SPI-FLASH interface
#define CONFIG_SPI_FLASH_VPL    1 //This is VPL spi-flash interface

/********* Ethernet *********/
#define CONFIG_RMII			     1
#define CONFIG_NET_RETRY_COUNT	20

/*==========================================================*
 *===          Board Environemt Settings                 ===*
 *==========================================================*/
/* bootstrap + u-boot + env + linux in nandflash */
#define CONFIG_ENV_IS_IN_SPI_FLASH  1
#define CONFIG_ENV_SIZE		        0x10000		/* 1 sector = 128 kB */
#define CONFIG_ENV_SECT_SIZE        0x10000
//#define CONFIG_ENV_OFFSET           0x20000
#define CONFIG_ENV_OFFSET_SPIFLASH  2 //[jam] we start from CONFIG_ENV_OFFSET th block of spi flash

#define CONFIG_SYS_LOAD_ADDR		0x22000000	/* boot from BOOT device */

#define CONFIG_BOOTCOMMAND	        "cp.b 0x10030000 0xa00000 0x150000;jump 0xa00000"
#define CONFIG_BOOTARGS				"root=/dev/mtdblock2 mem=64M console=1"
#define CONFIG_BAUDRATE	        	38400
#define CONFIG_SYS_BAUDRATE_TABLE	{115200 , 19200, 38400, 57600, 9600 }
#define CONFIG_IPADDR               172.17.4.43//[jam] this is james's ip
#define CONFIG_ETHADDR              00:ab:cd:ab:cd:ef
#define CONFIG_SERVERIP             172.17.0.6
#define CONFIG_NETMASK              255.255.0.0
#define CONFIG_GATEWAYIP            172.17.0.1
#define CONFIG_SYS_PROMPT		    "U-Boot> "
#define CONFIG_SYS_CBSIZE		    256 //[trace] max arguments allowed in all commands
#define CONFIG_SYS_MAXARGS		    16
#define CONFIG_SYS_PBSIZE		    (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16) //[trace] max length allowed when print a string
#define CONFIG_SYS_LONGHELP		    1

#define CONFIG_BOOTDELAY	3

/*==========================================================*
 *===                  Tag Options                       ===*
 *==========================================================*/
//These are related to the "tags" in bootm.c 
#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs	*/
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG	1

/*==========================================================*
 *===                  Misc Options                      ===*
 *==========================================================*/
/********* Command line extended options *********/
#define CONFIG_CMDLINE_EDITING	1 


#endif
