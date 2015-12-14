/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>

#include <asm/arch/platform.h>
#include <sd_card.h>
#include <spi_flash.h>
#include <nand.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_AMIGAONEG3SE
	extern void enable_nvram(void);
	extern void disable_nvram(void);
#endif

//#define DEBUG_ENV
#ifdef DEBUG_ENV
#define DEBUGF(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGF(fmt,args...)
#endif

extern env_t *env_ptr;

/*
 * [jam] Because sometimes we will load u-boot on serial flash or nand flash on Mozart Soc,
 *       we should use function pointers to decide what storage we want to use.
 */
#ifdef CONFIG_SOC_HAYDN
extern int serialflash_saveenv(void) ;
extern int serialflash_env_init(void) ;
extern unsigned char serialflash_env_get_char_spec(int index) ;
extern void serialflash_env_relocate_spec(void) ;
#elif (defined(CONFIG_SOC_MOZART) || (defined(CONFIG_SOC_BEETHOVEN)))
#ifdef CONFIG_CMD_MMC
extern inline int sd_env_init(void);
extern int sd_saveenv(void);
extern unsigned char sd_env_get_char_spec(int index) ;
extern void sd_env_relocate_spec(void);
void update_default_envs_ifsdboot(void) ;
#else
inline int sd_env_init(void) {};
int sd_saveenv(void) {};
unsigned char sd_env_get_char_spec(int index) {};
void sd_env_relocate_spec(void) {};
#endif

#ifdef CONFIG_CMD_SF
extern int serialflash_saveenv(void) ;
extern int serialflash_env_init(void) ;
extern unsigned char serialflash_env_get_char_spec(int index) ;
extern void serialflash_env_relocate_spec(void) ;
#else
int serialflash_saveenv(void) {return 0;};
int serialflash_env_init(void) {return 0;};
unsigned char serialflash_env_get_char_spec(int index) {return 0;};
void serialflash_env_relocate_spec(void) {};
#endif

#ifdef CONFIG_CMD_NAND
extern int nand_env_init(void) ;
extern int nand_saveenv(void) ;
extern unsigned char nand_env_get_char_spec(int index) ;
extern void nand_env_relocate_spec(void) ;

void update_default_envs_ifnfboot(void) ;
#endif
#else
#error You should define CONFIG_SOC_HAYDN || CONFIG_SOC_MOZART.
#endif

int (*env_init)(void) ;
int (*saveenv)(void) ;
unsigned char (*env_get_char_spec)(int index) ;
void (*env_relocate_spec)(void) ;
char *env_name_spec ;
env_t *env_ptr = NULL;

//extern struct vpl_spi_flash ;
extern inline struct vpl_spi_flash *to_vpl_spi_flash(struct spi_flash *flash) ;

#ifdef SDAUTOBURN_FLOW_FROMSD
static struct spi_flash *autoburn_sf;
#endif

void set_env_funcptrs(void)
{
#ifdef CONFIG_SOC_HAYDN
    env_init = serialflash_env_init ;
    saveenv = serialflash_saveenv ;
    env_get_char_spec = serialflash_env_get_char_spec ;
    env_relocate_spec = serialflash_env_relocate_spec ;
#elif (defined(CONFIG_SOC_MOZART) || defined(CONFIG_SOC_BEETHOVEN))
    unsigned long val = inl(BOOTING_DEVICE_INFO);
    int sdbootSucess = 0 ;


#ifdef CONFIG_CMD_MMC
    if(val == SYSCTRL_DATA_IN_SD && SD_Card_Detect(0)) {
        printf( "  Boot Storage : SD Card\n" ) ;
        gd->env_valid = 0 ;//We always use default envs when booting from SD.
        env_init = sd_env_init ;
        saveenv = sd_saveenv ;
        env_get_char_spec = sd_env_get_char_spec ;
        env_relocate_spec = sd_env_relocate_spec ;

        //check if real sd boot
        if(SD_Read(MAGIC_SD_ADDR, MAGIC_DATA_SIZE, MAGIC_DRAM_ADDR) != 0) {
			printf("[ERR] SD-Read fails!\n") ;
			sdbootSucess = 0 ;
			goto SDBOOT_FAIL;
        }

        if((v_inl(MAGIC_DRAM_ADDR) != MAGIC_NUM0) || (v_inl(MAGIC_DRAM_ADDR+4) != MAGIC_NUM1)) {
            printf("   !! MAGIC# of SD Card is wrong\n") ;
            printf("   !! Find other Boot Storage..\n\n") ;
            sdbootSucess = 0 ;
            goto SDBOOT_FAIL ;
        }
        printf("   SD Card has correct Magic#.\n") ;
        printf("   SD Boot Sucessfully.\n\n") ;

        update_default_envs_ifsdboot() ;

        sdbootSucess = 1 ;

#ifdef SDAUTOBURN_FLOW_FROMSD
        printf("  ****** Auto Burn Flow ******\n") ;
        printf("  Step 1. Copy data(size 0x%08x) from 0x%08x of SD to 0x%08x of DRAM\n", AUTOBURN_DATASIZE, AUTOBURN_SDADDR, AUTOBURN_DRAMADDR) ;
        SD_Read(AUTOBURN_SDADDR, AUTOBURN_DATASIZE, AUTOBURN_DRAMADDR) ;

        if(AUTOBURN_FLASHTYPE == AUTOBURN_SPIFLASH) {
            printf("  Step 2. Write data from 0x%08x of DRAM to 0x%08x of SPI FLASH\n", AUTOBURN_DRAMADDR, AUTOBURN_FLASHADDR) ;
            autoburn_sf = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED, CONFIG_DEFAULT_SPI_MODE); 
            spi_flash_erase(autoburn_sf, AUTOBURN_FLASHADDR, AUTOBURN_DATASIZE, 0) ;
            spi_flash_write(autoburn_sf, AUTOBURN_FLASHADDR, AUTOBURN_DATASIZE, AUTOBURN_DRAMADDR, 0) ;
            printf("  Done\n") ;
						outl(0xFFF, 0x49000004);
        }
        else if(AUTOBURN_FLASHTYPE == AUTOBURN_NANDFLASH)
        {
            printf("  Step 3. Write data from 0x%08x of DRAM to 0x%08x of NAND FLASH\n", AUTOBURN_DRAMADDR, AUTOBURN_FLASHADDR) ;
            nand_info_t *mtd ;
            mtd = &nand_info[0] ;
            nand_erase_options_t opts;

            memset(&opts, 0, sizeof(opts));
    		opts.offset = AUTOBURN_FLASHADDR;
	    	opts.length = AUTOBURN_DATASIZE;
		    //opts.jffs2  = clean;
            opts.jffs2  = 0;//[patch] we do not allow jffs2 in our u-boot
	    	opts.quiet  = 0;
            opts.all = 1 ;

            unsigned long datasize = AUTOBURN_DATASIZE ;
            nand_erase_opts(mtd, &opts) ;
            //printf("chipsize=%d, blocksize=%d, block#=%d\n", mtd->totalsize, mtd->erasesize, (mtd->totalsize / mtd->erasesize)) ;
            nand_write_skip_bad(mtd, AUTOBURN_FLASHADDR, &datasize, AUTOBURN_DRAMADDR) ;//100 temporary
        }
#endif //SDAUTOBURN_FLOW_FROMSD
    }

    if(sdbootSucess)
        return ;

#endif //CONFIG_CMD_SD
SDBOOT_FAIL:
    if ( val == SYSCTRL_DATA_IN_SERIALFLASH) {
        printf( "  Boot Storage : Serial Flash\n" ) ;
        env_init = serialflash_env_init ;
        saveenv = serialflash_saveenv ;
        env_get_char_spec = serialflash_env_get_char_spec ;
        env_relocate_spec = serialflash_env_relocate_spec ;
    }
#if defined(CONFIG_CMD_NAND)
    else if ( val == SYSCTRL_DATA_IN_NANDFLASH) {
		update_default_envs_ifnfboot() ;
        printf( "  Boot Storage : Nand Flash\n" ) ;
        env_init = nand_env_init ;
        saveenv = nand_saveenv ;
        env_get_char_spec = nand_env_get_char_spec ;
        env_relocate_spec = nand_env_relocate_spec ;
    }
#endif

#endif //CONFIG_SOC_MOZART
}

static uchar env_get_char_init (int index);

/************************************************************************
 * Default settings to be used when no valid environment is found
 */
#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

uchar default_environment[CONFIG_DEFAULT_ENV_ARRAY_SIZE] = {
#ifdef	CONFIG_BOOTARGS
	"bootargs="	CONFIG_BOOTARGS			"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	MK_STR(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	MK_STR(CONFIG_BAUDRATE)		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	MK_STR(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	MK_STR(CONFIG_ETHADDR)		"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	MK_STR(CONFIG_ETH1ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	MK_STR(CONFIG_ETH2ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	MK_STR(CONFIG_ETH4ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	MK_STR(CONFIG_ETH5ADDR)		"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	MK_STR(CONFIG_IPADDR)		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	MK_STR(CONFIG_SERVERIP)		"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD			"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	MK_STR(CONFIG_ROOTPATH)		"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	MK_STR(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	MK_STR(CONFIG_NETMASK)		"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	MK_STR(CONFIG_HOSTNAME)		"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	MK_STR(CONFIG_BOOTFILE)		"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	MK_STR(CONFIG_LOADADDR)		"\0"
#endif
#ifdef  CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	MK_STR(CONFIG_PCI_BOOTDELAY)	"\0"
#endif
#ifdef  CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT
    "autoburn_bootsect=" CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEKERNEL
    "autoburn_kernel=" CONFIG_AUTOBURN_TFTP_UPDATEKERNEL "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEROOTFS
    "autoburn_rootfs=" CONFIG_AUTOBURN_TFTP_UPDATEROOTFS "\0"
#endif
#ifdef CONFIG_MODELNAME
    "modelname=" CONFIG_MODELNAME "\0"
#endif
#ifdef CONFIG_DEFAULT_PHY_MODE
	"phy_mode="	MK_STR(CONFIG_DEFAULT_PHY_MODE)		"\0"
#endif
#ifdef CONFIG_SETBOOTARGS
	"setargs=" CONFIG_SETBOOTARGS		"\0"
#endif
#ifdef CONFIG_QUICK_TEST
	"auto_quicktest=0" "\0"
#endif
#ifdef CONFIG_QUICK_TEST_DIDO
	"di_pin=" MK_STR(PLATFORM_DI_PIN) "\0"
	"do_pin=" MK_STR(PLATFORM_DO_PIN) "\0"
#endif
    "\0"
};

#ifdef CONFIG_CMD_MMC
uchar default_environment_SDBOOT[CONFIG_DEFAULT_ENV_ARRAY_SIZE] = {
#ifdef	CONFIG_BOOTARGS_RAMDISK
	"bootargs="	CONFIG_BOOTARGS_RAMDISK		"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND_MMC		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	MK_STR(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	MK_STR(CONFIG_BAUDRATE)		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	MK_STR(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	MK_STR(CONFIG_ETHADDR_SDBOOT)		"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	MK_STR(CONFIG_ETH1ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	MK_STR(CONFIG_ETH2ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	MK_STR(CONFIG_ETH4ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	MK_STR(CONFIG_ETH5ADDR)		"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	MK_STR(CONFIG_IPADDR_SDBOOT)		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	MK_STR(CONFIG_SERVERIP)		"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD			"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	MK_STR(CONFIG_ROOTPATH)		"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	MK_STR(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	MK_STR(CONFIG_NETMASK)		"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	MK_STR(CONFIG_HOSTNAME)		"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	MK_STR(CONFIG_BOOTFILE)		"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	MK_STR(CONFIG_LOADADDR)		"\0"
#endif
#ifdef  CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	MK_STR(CONFIG_PCI_BOOTDELAY)	"\0"
#endif
#ifdef  CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT
    "autoburn_bootsect=" CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEKERNEL
    "autoburn_kernel=" CONFIG_AUTOBURN_TFTP_UPDATEKERNEL "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEROOTFS
    "autoburn_rootfs=" CONFIG_AUTOBURN_TFTP_UPDATEROOTFS "\0"
#endif
#ifdef CONFIG_MODELNAME
    "modelname=" CONFIG_MODELNAME "\0"
#endif
#ifdef CONFIG_DEFAULT_PHY_MODE
	"phy_mode="	MK_STR(CONFIG_DEFAULT_PHY_MODE)		"\0"
#endif
#ifdef CONFIG_SETBOOTARGS
	"setargs=" CONFIG_SETBOOTARGS		"\0"
#endif
#ifdef CONFIG_QUICK_TEST
	"auto_quicktest=0" "\0"
#endif
#ifdef CONFIG_QUICK_TEST_DIDO
	"di_pin=" MK_STR(PLATFORM_DI_PIN) "\0"
	"do_pin=" MK_STR(PLATFORM_DO_PIN) "\0"
#endif
    "\0"
};

void update_default_envs_ifsdboot(void)
{
    //printf("update env if sdboot\n") ;
    memcpy(default_environment, default_environment_SDBOOT, sizeof(default_environment_SDBOOT)) ;
}
#endif

#if defined(CONFIG_ENV_IS_IN_NAND)		/* Environment is in Nand Flash */ \
	|| defined(CONFIG_ENV_IS_IN_SPI_FLASH)
int default_environment_size = sizeof(default_environment);
#endif

#if defined(CONFIG_CMD_NAND)
uchar default_environment_NFBOOT[CONFIG_DEFAULT_ENV_ARRAY_SIZE] = {
#ifdef	CONFIG_BOOTARGS_RAMDISK
	"bootargs="	CONFIG_BOOTARGS_RAMDISK		"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND_NF		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	MK_STR(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	MK_STR(CONFIG_BAUDRATE)		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	MK_STR(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	MK_STR(CONFIG_ETHADDR)		"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	MK_STR(CONFIG_ETH1ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	MK_STR(CONFIG_ETH2ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	MK_STR(CONFIG_ETH4ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	MK_STR(CONFIG_ETH5ADDR)		"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	MK_STR(CONFIG_IPADDR)		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	MK_STR(CONFIG_SERVERIP)		"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD			"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	MK_STR(CONFIG_ROOTPATH)		"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	MK_STR(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	MK_STR(CONFIG_NETMASK)		"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	MK_STR(CONFIG_HOSTNAME)		"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	MK_STR(CONFIG_BOOTFILE)		"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	MK_STR(CONFIG_LOADADDR)		"\0"
#endif
#ifdef  CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	MK_STR(CONFIG_PCI_BOOTDELAY)	"\0"
#endif
#ifdef  CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT
    "autoburn_bootsect=" CONFIG_AUTOBURN_TFTP_UPDATEBOOTSECT "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEKERNEL
    "autoburn_kernel=" CONFIG_AUTOBURN_TFTP_UPDATEKERNEL "\0"
#endif
#ifdef CONFIG_AUTOBURN_TFTP_UPDATEROOTFS
    "autoburn_rootfs=" CONFIG_AUTOBURN_TFTP_UPDATEROOTFS "\0"
#endif
#ifdef CONFIG_MODELNAME
    "modelname=" CONFIG_MODELNAME "\0"
#endif
#ifdef MTDPARTS_DEFAULT
    "mtdparts=" MTDPARTS_DEFAULT "\0"
#endif
#ifdef CONFIG_DEFAULT_PHY_MODE
	"phy_mode="	MK_STR(CONFIG_DEFAULT_PHY_MODE)		"\0"
#endif
#ifdef CONFIG_SETBOOTARGS
	"setargs=" CONFIG_SETBOOTARGS		"\0"
#endif
#ifdef CONFIG_QUICK_TEST
	"auto_quicktest=0" "\0"
#endif
#ifdef CONFIG_QUICK_TEST_DIDO
	"di_pin=" MK_STR(PLATFORM_DI_PIN) "\0"
	"do_pin=" MK_STR(PLATFORM_DO_PIN) "\0"
#endif
    "\0"
};

void update_default_envs_ifnfboot(void)
{
    //printf("update env if sdboot\n") ;
    memcpy(default_environment, default_environment_NFBOOT, sizeof(default_environment_NFBOOT)) ;
}

#endif//endif CONFIG_CMD_NAND

void env_crc_update (void)
{
	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
}

static uchar env_get_char_init (int index)
{
	uchar c;

	/* if crc was bad, use the default environment */
	if (gd->env_valid)
	{
		c = (*env_get_char_spec)(index);
	} else {
		c = default_environment[index];
	}

	return (c);
}

#ifdef CONFIG_AMIGAONEG3SE
uchar env_get_char_memory (int index)
{
	uchar retval;
	enable_nvram();
	if (gd->env_valid) {
		retval = ( *((uchar *)(gd->env_addr + index)) );
	} else {
		retval = ( default_environment[index] );
	}
	disable_nvram();
	return retval;
}
#else
uchar env_get_char_memory (int index)
{
	if (gd->env_valid) {
		return ( *((uchar *)(gd->env_addr + index)) );
	} else {
		return ( default_environment[index] );
	}
}
#endif

uchar env_get_char (int index)
{
	uchar c;

	/* if relocated to RAM */
	if (gd->flags & GD_FLG_RELOC)
		c = env_get_char_memory(index);
	else
		c = env_get_char_init(index);

	return (c);
}

uchar *env_get_addr (int index)
{
	if (gd->env_valid) {
		return ( ((uchar *)(gd->env_addr + index)) );
	} else {
		return (&default_environment[index]);
	}
}

void set_default_env(void)
{
	if (sizeof(default_environment) > ENV_SIZE) {
		puts ("*** Error - default environment is too large\n\n");
		return;
	}

	memset(env_ptr, 0, sizeof(env_t));
	memcpy(env_ptr->data, default_environment,
	       sizeof(default_environment));
#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
	env_ptr->flags = 0xFF;
#endif
	env_crc_update ();
	gd->env_valid = 1;
}

void env_relocate (void)
{
	DEBUGF ("%s[%d] offset = 0x%lx\n", __FUNCTION__,__LINE__, gd->reloc_off);

	env_ptr = (env_t *)mALLOc (CONFIG_ENV_SIZE);
	DEBUGF ("%s[%d] malloced ENV at %p\n", __FUNCTION__,__LINE__,env_ptr);

    set_env_funcptrs() ;
    (*env_init)() ;

	if (gd->env_valid == 0) {
#if defined(CONFIG_GTH)	|| defined(CONFIG_ENV_IS_NOWHERE)	/* Environment not changable */
		puts ("Using default environment\n\n");
#else
		//puts ("*** Warning - bad CRC, using default environment\n\n");
		puts ("*** We do not have environment varialbes stored in the booting storgage, so we using default environment\n\n");
		//show_boot_progress (-60);
#endif
		set_default_env();
	}
	else {
        printf("  Finding the environment variables in boot storage...\n") ;
		(*env_relocate_spec)();
	}
	gd->env_addr = (ulong)&(env_ptr->data);
}

#ifdef CONFIG_MODELNAME
void verify_the_modelname(void)
{
	unsigned long new_prot_info_val ;
	int ver, new_prot_info_val0, new_prot_info_val1;
	char *new_prot_info_name ;
	char *original_prot_info_name;

	puts ("\n=== Model Name ===\n");

	ver = inl(SYSC_CHIP_VERSION);
	switch (ver)
	{
		case ASC88XX_M1_VERSION :
			new_prot_info_val = v_inl(SYSCTRL_PROT_INFO) & 0x7 ;
			switch (new_prot_info_val)
			{
				case ASC8850M1_NUM :
					new_prot_info_name = ASC8850M1_NAME ;
					break ;
				case ASC8849M1_NUM :
					new_prot_info_name = ASC8849M1_NAME ;
					break ;
				case ASC8848M1_NUM :
					new_prot_info_name = ASC8848M1_NAME ;
					break ;
				default :
					new_prot_info_name = ASC_DEFAULT_NAME ;
			} ;
			break;

		case ASC88XX_M2_VERSION :
			new_prot_info_val0 = v_inl(SYSCTRL_PROT_INFO) & 0x7 ;
			new_prot_info_val1 = v_inl(SYSCTRL_PROT_INFO_1) & 0x7 ;

			switch (new_prot_info_val0)
			{
				case ASC8852M2_NUM  :
					new_prot_info_name = ASC8852M2_NAME;
					break;
				case ASC8851M2_NUM :
					if(new_prot_info_val1 == 0x4)
						new_prot_info_name = ASC8851M2_NAME ;
					else
						new_prot_info_name = ASC8850M2_NAME ;
					break ;
				case ASC8849M2_NUM :
					new_prot_info_name = ASC8849M2_NAME ;
					break ;
				case ASC8848M2_NUM :
					new_prot_info_name = ASC8848M2_NAME ;
					break ;
				default :
					new_prot_info_name = ASC_DEFAULT_NAME ;
			} ;
			break;

		case ASC88XX_A_VERSION:
			new_prot_info_val0 = v_inl(SYSCTRL_PROT_INFO);

			switch (new_prot_info_val0 & 0x7)
			{
				case ASC8852A_8851A_8850A_NUM :
					switch ((new_prot_info_val0 & 0x03ff0000)>>16)
					{
						case 0x0  : new_prot_info_name = ASC8852A_NAME; break;
						case 0x180: new_prot_info_name = ASC8851A_NAME; break;
						case 0x240: new_prot_info_name = ASC8850A_NAME; break;
						default   : new_prot_info_name = ASC_DEFAULT_NAME;
					}
					break ;
				case ASC8849A_NUM :
					new_prot_info_name = ASC8849A_NAME ;
					break ;
				case ASC8848A_NUM :
					new_prot_info_name = ASC8848A_NAME ;
					break ;
				default :
					new_prot_info_name = ASC_DEFAULT_NAME ;
			} ;
			break;

		default :
			new_prot_info_name = "unknow";
	}

	original_prot_info_name = getenv("modelname") ;
	if(strcmp(original_prot_info_name, new_prot_info_name) != 0) {
		setenv("modelname", new_prot_info_name) ;
		(*saveenv)() ;
	}

	printf("  Model Name : %s\n", new_prot_info_name) ;
}
#endif

#ifdef CONFIG_AUTO_COMPLETE
int env_complete(char *var, int maxv, char *cmdv[], int bufsz, char *buf)
{
	int i, nxt, len, vallen, found;
	const char *lval, *rval;

	found = 0;
	cmdv[0] = NULL;

	len = strlen(var);
	/* now iterate over the variables and select those that match */
	for (i=0; env_get_char(i) != '\0'; i=nxt+1) {

		for (nxt=i; env_get_char(nxt) != '\0'; ++nxt)
			;

		lval = (char *)env_get_addr(i);
		rval = strchr(lval, '=');
		if (rval != NULL) {
			vallen = rval - lval;
			rval++;
		} else
			vallen = strlen(lval);

		if (len > 0 && (vallen < len || memcmp(lval, var, len) != 0))
			continue;

		if (found >= maxv - 2 || bufsz < vallen + 1) {
			cmdv[found++] = "...";
			break;
		}
		cmdv[found++] = buf;
		memcpy(buf, lval, vallen); buf += vallen; bufsz -= vallen;
		*buf++ = '\0'; bufsz--;
	}

	cmdv[found] = NULL;
	return found;
}
#endif
