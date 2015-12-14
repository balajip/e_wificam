#ifndef __MOZARTEVM_H__
#define __MOZARTEVM_H__

#include <configs/mozart_model.h>
#define CONFIG_SOC_MOZART			1

/*
 * . Layout of this configuration file :
 * |-- #1. UBoot HW/Peripheral Settings
 * |	|-- #1-1. Basic HW SPEC.
 * |	|-- #1-2. Advanced HW SPEC.
 * |    `-- #1-3. Quick Test : This will be effective only when turn on PLATFORM_QUICKTEST_ENABLE.
 * |-- #2. UBoot SW Settings
 * |	|-- #2-1. Command List.
 * |	|-- #2-2. Environment Configurations.
 * |	`-- #2-3. Booting Kernel Information.
 * `-- #3. Misc
 */

/*=========================================================*
 *===				#1-1. Basic HW SPEC.				===*
 *=========================================================*/
/* Please set the correct peripheral settings of your PCB.
 * In fact, this file covers most configurations of U-Boot.
 * You do not need to modify the source code in genearl case, unless you hope to customize it.
 */
#define CONFIG_ARM926EJS	 				1//CPU Type.

#if defined(CONFIG_ASC8848_A) || defined(CONFIG_ASC8848_A_133MHZ) || \
    defined(CONFIG_ASC8848_M2) || defined(CONFIG_ASC8848_M2_133MHZ) || \
    defined(CONFIG_ASC8848_M1)
#  define PLATFORM_CONSOLE                  1 // UART port as console
#  define PLATFORM_RS485_PORTNUM            0 // UART port of RS485, -1 means no rs485
#  define PLATFORM_DRAM_NUM                 1
#else

#  if defined(CONFIG_ASC88XX_A)
#    define PLATFORM_CONSOLE                1
#    define PLATFORM_RS485_PORTNUM          0 // UART port of RS485, -1 means no rs485


#  else
#    define PLATFORM_CONSOLE                1
#    define PLATFORM_RS485_PORTNUM          1 // UART port of RS485, -1 means no rs485
#  endif

#define PLATFORM_DRAM_NUM					2
#endif
#define PLATFORM_SPIFLASH_NUM               1 // Max number of SPI Flashes
#define PLATFORM_NANDFLASH_NUM              0
#define PLATFORM_SDCARD_NUM                 1 //At most 2, but "sdread" only reads SD card in the "first" slot.	
#define PLATFORM_USB_NUM                    0
#define PLATFORM_PCIE_NUM                   0
#define PLATFORM_DIDO_NUM                   0

#define PLATFORM_ETHERNET                   1
#define PLATFORM_ETHERNET_PHYADDR           1
//In MozartV1, Uboot GMAC driver can know whether EVM is GMii or RGMii mode by reading the specific register.
//But that register cannot give the same information in MozartV2.
//We should set the mode of Uboot manually.
#define PLATFORM_ETHERNET_DEFAULT_PHY_MODE  rgmii
#define PLATFORM_ETHERNET_SPEED_MODE        0 // 0: Auto negotiation 1: Fixed to 100Mbps Full Duplex.

/* audio codec */
#define PLATFORM_AUDIO_CODEC_SSM2603		0
#define PLATFORM_AUDIO_CODEC_TW2866x1		1//TW2866x1 : one TW2866 codec chip, like VS8401
#define PLATFORM_AUDIO_CODEC_TW2866x2		2//TW2866x2 : two TW2866 codec chips, like VS8801
#define PLATFORM_AUDIO_CODEC_TYPE			PLATFORM_AUDIO_CODEC_SSM2603

#define PLATFORM_VIDEO_CODEC_MT9P031			0
#define PLATFORM_VIDEO_CODEC_TW2866x1		1
#define PLATFORM_VIDEO_CODEC_TW2866x2		2
#define PLATFORM_VIDEO_CODEC_OV9715			3
#define PLATFORM_VIDEO_CODEC_TYPE			PLATFORM_VIDEO_CODEC_OV9715


#define PLATFORM_QUICKTEST_ENABLE           0

/*=========================================================*
 *===				#1-2. Advanced HW SPEC.				===*
 *=========================================================*/
/* ARM asynchronous clock */
#define AHB_CLOCK            PLATFORM_AHB_CLOCK
#define APB_CLOCK            (AHB_CLOCK / 2)
#define CONFIG_SYS_HZ		 1000

/********* SF Flash *********/
#if (PLATFORM_SPIFLASH_NUM != 0)
#define CONFIG_DW_SPI                       1
#define CONFIG_DEFAULT_SPI_MODE             (SPI_MODE_3)
#define CONFIG_SPI_FLASH                    1
#define CONFIG_SF_DEFAULT_SPEED             min(25000000, APB_CLOCK) /* 25 MHz */
#define CONFIG_SPI_FLASH_BUS                0
#define SPI_FLASH_NUMBER					PLATFORM_SPIFLASH_NUM//The number of spi flash
#define SSI_NUM_SLAVES                      (SPI_FLASH_NUMBER) /* Number of SSI Slaves */

#define CONFIG_SPI_FLASH_VPL                1 //This is VPL spi-flash interface
#define CONFIG_SPI_FLASH_MACRONIX						1
#define CONFIG_SPI_FLASH_SPANSION						1
#define CONFIG_SPI_FLASH_WINBOND						1
#define CONFIG_SPI_FLASH_GIGADEVICE					1
#endif//(SPI_FLASH_NUMBER != 0)

/********* SD *********/
#if (PLATFORM_SDCARD_NUM != 0)
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC   /* generic MMC framework */
#define CONFIG_DW_MMC
#define CONFIG_DOS_PARTITION
#define MMC_REFERENCE_CLOCK                25000000 /* 25 MHZ */
#define SD_BLOCK_SIZE 512

/* Helpful settings when booting from SD-card. */
//#define SDAUTOBURN_FLOW_FROMSD
//#define SDAUTOBURN_FLOW_FROMTFTP
//#define SD_AUTORUN /* script autorun */
#define CONFIG_IPADDR_SDBOOT				172.17.201.30
#define CONFIG_ETHADDR_SDBOOT				02:00:01:00:03:00
#endif

/// For Autoupdate the firmware from SD card to SPI flash
//#define SDAUTOSPI_UPDATE 1

/********* DRAM *********/
#if (PLATFORM_DRAM_NUM > 2)
#define CONFIG_DRAM_NUMBER					2//the max DRAM number Our UBoot can see
#else
#define CONFIG_DRAM_NUMBER					PLATFORM_DRAM_NUM
#endif
#define 	CONFIG_DCE

/********* NAND *********/
#if (PLATFORM_NANDFLASH_NUM != 0)
#define CONFIG_SYS_MAX_NAND_DEVICE			PLATFORM_NANDFLASH_NUM
#define CONFIG_SYS_NAND_BASE				0x0 //[trace] In fact this is useless in our case, but we still keep this macro and assign it a dummy value.
#endif

/********* DIDO *********/
#if (PLATFORM_DIDO_NUM != 0)
#define PLATFORM_DI_PIN                     14 /* GPIO14 */
#define PLATFORM_DO_PIN                     15 /* GPIO15 */
#endif

/********* UART *********/
#define CONFIG_UART_PORTNUM					PLATFORM_CONSOLE /* 0 ~ 3 */

/********* Ethernet *********/
#define NET_PHY_ADDRESS                         PLATFORM_ETHERNET_PHYADDR
#define NET_FIX_SPEED                           PLATFORM_ETHERNET_SPEED_MODE
#define CONFIG_MII                              1
#define CONFIG_PHYLIB                           1
#define CONFIG_PHY_BROADCOM                     1
#define CONFIG_PHY_ICPLUS                       1
#define CONFIG_PHY_MICREL                       1
#define CONFIG_PHY_REALTEK                      1
#define CONFIG_NET_RETRY_COUNT                  20
#define CONFIG_SYS_LOAD_ADDR                    0x22000000	/* load address */

/*=========================================================*
 *===				#1-3. Quick Test					===*
 *=========================================================*/
/*
 * When enable the macro CONFIG_QUICK_TEST, u-boot will support quick-test functions.
 * By default it will have a TEST-MENU in command interface(type the command "test"),
 * it also supports automatically quick test by set environment 'auto_quicktest' to 1.
 */
#if (PLATFORM_QUICKTEST_ENABLE)
#define CONFIG_QUICK_TEST
#endif

#ifdef CONFIG_QUICK_TEST
#define CONFIG_STACKSIZE_IRQ				(4*1024)/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ				(4*1024)/* FIQ stack */
#define CONFIG_USE_IRQ

#if (PLATFORM_SPIFLASH_NUM != 0)
#define CONFIG_QUICK_TEST_SPI
#endif

#if (PLATFORM_PCIE_NUM != 0)
#define CONFIG_QUICK_TEST_PCIE
#endif

#if (PLATFORM_DIDO_NUM != 0)
#define CONFIG_QUICK_TEST_DIDO
#endif

#if (PLATFORM_SDCARD_NUM != 0)
#define CONFIG_QUICK_TEST_MSHC
#endif

#if (PLATFORM_NANDFLASH_NUM != 0)
#define CONFIG_QUICK_TEST_NAND
#define NAND_QUICK_TEST_BLOCK_INDEX			35//You can erase the block#35.
                                              //Please choose the empty block for testing based on your EVM SW partitions on Nand flash.
                                              //The nand block number starts from "0"(0, 1, .., N).
#endif

#if (PLATFORM_USB_NUM != 0)
#define CONFIG_QUICK_TEST_USBC
#endif

#if (PLATFORM_RS485_PORTNUM != -1)
#define CONFIG_QUICK_TEST_RS485
#endif

#define CONFIG_QUICK_TEST_WDT
#define CONFIG_QUICK_TEST_DRAM
#define CONFIG_QUICK_TEST_RTC
#if (PLATFORM_ETHERNET)
#define CONFIG_QUICK_TEST_GMAC
#define CONFIG_QUICK_TEST_GMAC_WARNING
#endif
#define CONFIG_QUICK_TEST_AUDIO

#else
#undef CONFIG_USE_IRQ
#endif//CONFIG_QUICK_TEST

/********* Advanced test configurations *********/
#if defined(CONFIG_QUICK_TEST_RTC) || defined(CONFIG_QUICK_TEST_AUDIO) || defined(CONFIG_QUICK_TEST_VIDEO)
#undef CONFIG_HARD_I2C
#define CONFIG_SOFT_I2C						1

#define CONFIG_SYS_I2C_SPEED				20 /* i2c frequency */
#define CONFIG_SYS_I2C_SLAVE				0xFE /* slave addr mask ? */

#define MOZART_GPIOC_DATA_OUT				0x4
#define MOZART_GPIOC_DATA_IN				0x8
#define MOZART_GPIOC_PIN_DIR				0xc
#define MOZART_GPIOC_DATA_SET				0x14
#define MOZART_GPIOC_DATA_CLR				0x18

#define I2C_INIT
#define CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW	0x7
#endif //defined(CONFIG_QUICK_TEST_RTC) || defined(CONFIG_QUICK_TEST_AUDIO) || defined(CONFIG_QUICK_TEST_VIDEO)

#ifdef CONFIG_QUICK_TEST_RTC
#define CONFIG_CMD_DATE
#define MOZART_RTC_SCL                      12 /* GPIO12 */
#define MOZART_RTC_SDA                      13 /* GPIO13 */

/*
#define CONFIG_RTC_S35390A                  1
#define CONFIG_SYS_I2C_RTC_ADDR             0x30
*/
#define CONFIG_RTC_PCF8563                  1
#define CONFIG_SYS_I2C_RTC_ADDR             0x51
#endif

/* Choose the Audio-Codec you want to test */
#ifdef CONFIG_QUICK_TEST_AUDIO
#if (PLATFORM_AUDIO_CODEC_TYPE == PLATFORM_AUDIO_CODEC_SSM2603)
#define CONFIG_QUICK_TEST_AUDIO_SSM2603
#define MOZART_AUDIO_SCL					0x00001000
#define MOZART_AUDIO_SDA					0x00002000
#define AUDIO_TEST_LINE_IN
#define AUDIO_TEST_MIC_IN
#elif ((PLATFORM_AUDIO_CODEC_TYPE == PLATFORM_AUDIO_CODEC_TW2866x1) || (PLATFORM_AUDIO_CODEC_TYPE == PLATFORM_AUDIO_CODEC_TW2866x2))
#define CONFIG_QUICK_TEST_AUDIO_TW2866
#define MOZART_AUDIO_SCL					(1 << 6)
#define MOZART_AUDIO_SDA					(1 << 7)
#else
#error "-- Please choose a correct audio codec! --"
#endif//check PLATFORM_AUDIO_CODEC_TYPE..
#endif//CONFIG_QUICK_TEST_AUDIO

/* Choose the Sensor you want to test */
#ifdef CONFIG_QUICK_TEST_VIDEO
#if (PLATFORM_VIDEO_CODEC_TYPE == PLATFORM_VIDEO_CODEC_MT9P031)
#define CONFIG_QUICK_TEST_VIDEO_MT9P031
#define MOZART_VIDEO_SCL					0x00000040
#define MOZART_VIDEO_SDA					0x00000080
#elif ((PLATFORM_VIDEO_CODEC_TYPE == PLATFORM_VIDEO_CODEC_TW2866x1) || (PLATFORM_VIDEO_CODEC_TYPE == PLATFORM_VIDEO_CODEC_TW2866x2))
#define CONFIG_QUICK_TEST_VIDEO_TW2866
#define MOZART_VIDEO_SCL					(1 << 6)
#define MOZART_VIDEO_SDA					(1 << 7)
#elif (PLATFORM_VIDEO_CODEC_TYPE == PLATFORM_VIDEO_CODEC_OV9715)
#define CONFIG_QUICK_TEST_VIDEO_OV9715
#define MOZART_VIDEO_SCL					(1 << 6)
#define MOZART_VIDEO_SDA					(1 << 7)
#else
#error "-- Please choose a correct video codec! --"
#endif
#endif//CONFIG_QUICK_TEST_VIDEO

/*==========================================================*
 *===                #2-1. Command List.                 ===*
 *==========================================================*/
/*
 * These are UBoot command options.
 * "config_cmd_default.h" provides the basic UBoot commands,
 * if you do not need some default commands, please de-select it.
 * But if you need more, just add it.
 */
#include <config_cmd_default.h>
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
#undef CONFIG_CMD_XIMG
#undef CONFIG_CMD_PING
#if (PLATFORM_ETHERNET == 0)
#undef CONFIG_CMD_NET
#endif
#if (PLATFORM_NANDFLASH_NUM != 0)
#define CONFIG_CMD_NAND
#define CONFIG_CMD_MTDPARTS
/*
 * !! NOTICE !!
 * If you hope to write UBI image(created by mkfs.ubifs),
 * you should enable the following three macros and use the command "ubi" to write it to flash.
 *
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
*/
#endif
#if (PLATFORM_SPIFLASH_NUM != 0)
#define CONFIG_CMD_SF
#endif
#if (PLATFORM_SDCARD_NUM != 0)
#define CONFIG_CMD_MMC
#endif

/*=========================================================*
 *===           #2-2. Environment Configurations.		===*
 *=========================================================*/
/* In start.S */
#define CONFIG_SKIP_RELOCATE_UBOOT

/* Use optimized memcpy and memset from linux */
#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

/* Size of malloc() pool */
#define CONFIG_DEFAULT_ENV_ARRAY_SIZE           1024
#define CONFIG_ENV_SIZE                         (64*1024) // 64KiB
#define CONFIG_SYS_MALLOC_LEN                   3*1024*1024
#define CONFIG_SYS_GBL_DATA_SIZE                128         /* 128 bytes for initial data */
#define CONFIG_STACKSIZE                        (300*1024)  /* regular stack */

#ifdef SDAUTOBURN_FLOW_FROMTFTP
#define CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT	"tftp 0xa00000 Mozart/autoburn/bootldr.bin;"\
											"sf erase 0x0 0x20000;"\
											"sf write 0xa00000 0x0 0x20000"
#if 0
#define CONFIG_AUTOBURN_TFTP_UPDATEKERNEL	"tftp 0xa00000 kernel_mozart_380;"\
											"sf write 0xa00000 0x00030000 0x1d0000"
#define CONFIG_AUTOBURN_TFTP_UPDATEROOTFS	"tftp 0xa00000 rootfs_mozart;"\
											"sf write 0xa00000 0x00200000 0x00200000"
#endif
#endif//end #ifdef SDAUTOBURN_FLOW_FROMTFTP

#define CONFIG_SETBOOTARGS					"setenv bootargs ${bootargs} ${mtdparts};"\
											"setenv bootargs ${bootargs} phy_mode=${phy_mode};"

#define CONFIG_BOOTCOMMAND_SF				"run setargs;"\
											"sf read 0xa00000 0x30000 0x1d0000;"\
											"jump 0xa00000"

#define	CONFIG_BOOTCOMMAND_NF				"run setargs;"\
											"nand read 0xa00000 nand-kernel 0x1d0000;"\
											"nand read 0x3000000 nand-rootfs 0x800000;"\
											"jump 0xa00000"
#ifdef SD_AUTORUN
#define CONFIG_BOOTCOMMAND_MMC				"setenv bootargs ${bootargs} ${mtdparts};"\
											"tftp 0xa00000 ${script}; source 0xa00000"
#else
#define	CONFIG_BOOTCOMMAND_MMC				"run setargs;"\
											"mmc read 0 0xa00000 0x1000 0xE80;"\
											"mmc read 0 0x3000000 0xB000 0x4000;"\
											"jump 0xa00000"
#endif
#define CONFIG_BOOTCOMMAND					CONFIG_BOOTCOMMAND_SF

#if (PLATFORM_CONSOLE == 0)
#define CONFIG_BOOTARGS                                         "root=/dev/mtdblock2 mem=64M console=0"
#define CONFIG_BOOTARGS_RAMDISK                                 "root=/dev/ram0 ro initrd=0x3000000,8M mem=64M console=0"
#elif (PLATFORM_CONSOLE == 1)
#define CONFIG_BOOTARGS                                         "root=/dev/mtdblock2 mem=64M console=1"
#define CONFIG_BOOTARGS_RAMDISK                                 "root=/dev/ram0 ro initrd=0x3000000,8M mem=64M console=1"
#elif (PLATFORM_CONSOLE == 2)
#define CONFIG_BOOTARGS                                         "root=/dev/mtdblock2 mem=64M console=2"
#define CONFIG_BOOTARGS_RAMDISK                                 "root=/dev/ram0 ro initrd=0x3000000,8M mem=64M console=2"
#elif (PLATFORM_CONSOLE == 3)
#define CONFIG_BOOTARGS                                         "root=/dev/mtdblock2 mem=64M console=3"
#define CONFIG_BOOTARGS_RAMDISK                                 "root=/dev/ram0 ro initrd=0x3000000,8M mem=64M console=3"
#endif

#define CONFIG_BAUDRATE		       			38400
#define CONFIG_SYS_BAUDRATE_TABLE  			{115200 , 19200, 38400, 57600, 9600 }
#define CONFIG_IPADDR             		 	0.0.0.0
#define CONFIG_ETHADDR             			00:00:00:00:00:00
#define CONFIG_SERVERIP            			172.17.0.6
#define CONFIG_NETMASK             			255.255.0.0
#define CONFIG_GATEWAYIP           			172.17.0.1
#define CONFIG_MODELNAME          			ASC_DEFAULT_NAME
#define	CONFIG_DEFAULT_PHY_MODE				PLATFORM_ETHERNET_DEFAULT_PHY_MODE

#define CONFIG_SYS_PROMPT		   			"U-Boot> "
#define CONFIG_SYS_CBSIZE		   			256
#define CONFIG_SYS_MAXARGS		   			16
#define CONFIG_SYS_PBSIZE		   			(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP		   			1
#define CONFIG_BOOTDELAY	       			3

/* Please pre-setup the following configurations if you want to put the environment variables in these storages. */
#if (PLATFORM_NANDFLASH_NUM != 0)
#define CONFIG_ENV_IS_IN_NAND				1
#define CONFIG_ENV_RANGE_NAND				5//UBoot will search envs during CONFIG_ENV_RANGE * (nand-block-size)
#define CONFIG_ENV_OFFSET_NAND				10//The offset of UBoot environment variables start from "CONFIG_ENV_OFFSET"th block of nand flash.
											//Index of nand block is from "zero".
#ifdef CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT						"nand0=Mozart-nand"//Please use Mozart-nand as the nand chips's name.
#define MTDPARTS_DEFAULT					"mtdparts=Mozart-nand:1280k(nand-bootldr),640k(nand-env),4m(nand-kernel),16m(nand-rootfs),-(nand-ubifs)"
#endif
#endif//end #if (PLATFORM_NANDFLASH_NUM != 0)

#if (PLATFORM_SPIFLASH_NUM != 0)
#define CONFIG_ENV_IS_IN_SPI_FLASH  		1
#define CONFIG_ENV_SECT_SIZE                (0x10000) // SZ_64K
#define CONFIG_ENV_OFFSET_SPIFLASH          (2 * CONFIG_ENV_SECT_SIZE)
#endif//end #if (PLATFORM_SPIFLASH_NUM != 0)

#if (PLATFORM_SDCARD_NUM != 0)
#define CONFIG_ENV_IS_IN_MMC	1
#define CONFIG_SYS_MMC_ENV_DEV	0
#define CONFIG_ENV_OFFSET_MMC	0x800 //unit : block, ie. the environment variables locate at mmc block#"CONFIG_ENV_OFFSET_MMC".

#endif

/*=========================================================*
 *===			#2-3. Booting Kernel Information.		===*
 *=========================================================*/
 /* We need these options when booting the kernel. */
#define CONFIG_CMDLINE_TAG					1	/* enable passing of ATAGs	*/
#define CONFIG_SETUP_MEMORY_TAGS			1
#define CONFIG_INITRD_TAG					1
#define MACH_TYPE_EVM						0x778
#define BOOT_PARAMS_ADDR					0x100

/*====================================================*
 *===					#3. Misc                   ===*
 *====================================================*/
#define CONFIG_CMDLINE_EDITING				1
#define LOADER_VERSION_OFFSET               (VPL_BRC_RAM_BASE_REMAPPED + 0x1000)
#define BOOTING_DEVICE_INFO                 (VPL_BRC_RAM_BASE_REMAPPED + 0x1004)
#define inl(addr)							(*(volatile unsigned long *)(addr))
#define outl(val, addr)						(*(volatile unsigned long *)(addr)) = val
#define ROUND(A, B)							(((A) + (B)) & ~((B) - 1))

#define CONFIG_CMD_MTEST
#ifdef CONFIG_CMD_MTEST
#define PHYS_SDRAM_SIZE						0x04000000	/* 64 megs */
#define CONFIG_SYS_MEMTEST_START			0xa00000
#define CONFIG_SYS_MEMTEST_END				0xb00000
#define CONFIG_SYS_ALT_MEMTEST				//[jam trace] if we need more complete test, we should define CONFIG_SYS_ALT_MEMTEST.
#endif

#define CONFIG_SYS_NO_FLASH					1//we do not have nor flash

#endif//__MOZARTEVM_H__
