#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/cacheflush.h>
#include <linux/interrupt.h>
#include "../ubi/mozart_ubi_vmalloc.h"

/*------------------------------------------------------
 *                    Global Variables Part
 *------------------------------------------------------
 * We put global variables, macros, and structures in this part.
 */
//related mmr definitions are in include/asm/arch/mozart/mozart.h
#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))
#define v_inb(addr)		(*((volatile unsigned char *)(addr)))

#define J_DBG
#ifdef J_DBG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

#ifdef CONFIG_MTD_CMDLINE_PARTS
static const char *part_probes[] = { "cmdlinepart", NULL };
#endif

// Enable this for SW ECC; Disable for HW ECC (Also in nand_base.c)
//#define MOZART_NFC_DISABLE_HW_ECC 1


#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition Mozart_partitions_128M[] =
{
	{
		.name		= "Mozart bootloader",
		.offset		= 0,
		.size		= 64*2048,	/* 1st block = 64 pages = 64 * 2048B */
		.mask_flags	= MTD_WRITEABLE,	/* read-only */
	},
	{
		.name		= "Mozart others",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,	/* rest = 9 sectors */
		.mask_flags = MTD_WRITEABLE,
	}
};

static struct mtd_partition Mozart_partitions_512M[] =
{
#if 0
	{
		.name		= "Mozart bootloader",
		.offset		= 0,
		.size		= 64*2048*10,	/* 1st block = 64 pages = 64 * 2048B */
		.mask_flags	= MTD_WRITEABLE,	/* read-only */
	},
	/*{
		.name		= "Mozart UBI test",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 64*2048*100,
	},*/
#endif
	{
		.name		= "Mozart NAND Flash UBIFS Part",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
	}
};

#define BOOTLOADER_INDEX_PARTITION 0
#define ENVS_INDEX_PARTITION 1
#define KERNEL_IMAGE_MAXSIZE (3*1024*1024)
#define ROOTFS_IMAGE_MAXSIZE (3*1024*1024)
static struct mtd_partition board_nand_default_partition[] =
{
	{
		.name		= CONFIG_MTD_NAND_PARTITION_0_NAME,
		.offset		= CONFIG_MTD_NAND_PARTITION_0_OFFSET,
		.size		= CONFIG_MTD_NAND_PARTITION_0_SIZE,
	},
	{
		.name		= CONFIG_MTD_NAND_PARTITION_1_NAME,
		.offset		= CONFIG_MTD_NAND_PARTITION_1_OFFSET,
		.size		= CONFIG_MTD_NAND_PARTITION_1_SIZE,
	},
#if defined(CONFIG_MTD_NAND_3_PARTITION) || defined(CONFIG_MTD_NAND_4_PARTITION) || defined(CONFIG_MTD_NAND_5_PARTITION)
	{
		.name		= CONFIG_MTD_NAND_PARTITION_2_NAME,
		.offset		= CONFIG_MTD_NAND_PARTITION_2_OFFSET,
		.size		= CONFIG_MTD_NAND_PARTITION_2_SIZE,
	},
#endif
#if defined(CONFIG_MTD_NAND_4_PARTITION) || defined(CONFIG_MTD_NAND_5_PARTITION)
	{
		.name		= CONFIG_MTD_NAND_PARTITION_3_NAME,
		.offset		= CONFIG_MTD_NAND_PARTITION_3_OFFSET,
		.size		= CONFIG_MTD_NAND_PARTITION_3_SIZE,
	},
#endif
#if defined(CONFIG_MTD_NAND_5_PARTITION)
	{
		.name		= CONFIG_MTD_NAND_PARTITION_4_NAME,
		.offset		= CONFIG_MTD_NAND_PARTITION_4_OFFSET,
		.size		= CONFIG_MTD_NAND_PARTITION_4_SIZE,
	}
#endif
};
#endif

//[jam trace] Remember, we use all oob to store HW ECC.
#if 0
struct nand_ecclayout Mozart_nand_oob_64 = {
	.eccbytes = 64,
	.eccpos = {
		0,  1,  2,  3,  4,  5,  6,  7,
		8,  9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63}
};

struct nand_ecclayout Mozart_nand_oob_128 = {
	.eccbytes = 128,
	.eccpos = {
		0,  1,  2,  3,  4,  5,  6,  7,
		8,  9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99,100,101,102,103,
		104,105,106,107,108,109,110,111,
		112,113,114,115,116,117,118,119,
		120,121,122,123,124,125,126,127}
};
#endif

DECLARE_COMPLETION( NFC_TRANSFER_COMPLETE ) ;

/*------------------------------------------------------
 *                    redundant part
 *------------------------------------------------------
 * We declare these redundant functions because when HW ECC mode is enabled,
 * kernel would check if these functions are already implemented.
 * But in our hw ecc mode, we do not use these functions.
 */
int	Mozart_nand_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
	printk( "[jam ERR] Our NFC does not need this, you should not call Mozart_nand_calculate()\n" ) ;
	return -1 ;
}

int Mozart_nand_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
	printk( "[jam ERR] Our NFC does not need this, you should not call Mozart_nand_correct()\n" ) ;
	return -1 ;
}

void Mozart_nand_hwctl(struct mtd_info *mtd, int mode)
{
	printk( "[jam ERR] Our NFC does not need this, you should not call Mozart_nand_hwctl()\n" ) ;
	return ;
}

static void Mozart_nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	printk( "[jam ERR] Our NFC does not need this, you should not call Mozart_nand_hwcontrol()\n" ) ;
	return ;
}

/*------------------------------------------------------
 *                    Misc Part
 *------------------------------------------------------
 * This is a important part. We collect set functions, command function, and status functions here.
 */
#define NF_FLCTRL_BLOCK_64PAGES  (0 << 12)
#define NF_FLCTRL_BLOCK_128PAGES (1 << 12)
#define NF_FLCTRL_PAGESIZE_4KB   (0 << 11)
#define NF_FLCTRL_PAGESIZE_2KB   (1 << 11)
#define NF_FLCTRL_ECC_DISABLE    (0 <<  9)
#define NF_FLCTRL_ECC_ENABLE     (1 <<  9)
#define NF_FLCTRL_GLOBAL_INT_DISABLE (0 << 8)
#define NF_FLCTRL_GLOBAL_INT_ENABLE  (1 << 8)
#define NF_FLCTRL_DMA_TRIGGER_DISABLE (0 << 3)
#define NF_FLCTRL_DMA_TRIGGER_ENABLE  (1 << 3)
#define NF_FLCTRL_TRANSFER_COMPLETE_INT_DISABLE (0 << 2)
#define NF_FLCTRL_TRANSFER_COMPLETE_INT_ENABLE  (1 << 2)
#define NF_FLCTRL_ADDRCYCLE_4    (0 <<  0)
#define NF_FLCTRL_ADDRCYCLE_5    (1 <<  0)
void NFC_setDmaTriggerMode(struct mtd_info* mtd)
{
    struct nand_chip *chip = mtd->priv;
	struct Mozart_chip_private *chip_priv = chip->priv ;
    unsigned long flctrl ;

    if(!(chip_priv->dma_mode)) {
        printk( "[ERR] It means that you cannot call this function when not PAGEPROG or PAGEREAD in dma mode!!\n" ) ;
        return ;
    }

    flctrl = v_inl(MOZART_NFC_MMR_CTRL) ;
    flctrl |= NF_FLCTRL_GLOBAL_INT_ENABLE ;
    flctrl |= NF_FLCTRL_TRANSFER_COMPLETE_INT_ENABLE ;
    flctrl |= NF_FLCTRL_DMA_TRIGGER_ENABLE ;
    v_outl( MOZART_NFC_MMR_CTRL, flctrl) ;
}

void Mozart_nand_select_chip(struct mtd_info *mtd, int chipnumber)
{
    /* We not only select the chip, but also config it. */

	struct nand_chip *chip = mtd->priv;
	struct Mozart_chip_private *chip_priv = chip->priv ;
	unsigned long setting = 0x0 ;

	//In usual case, we only use at most 2 nand flash chips.
	//So we assign 3 to chip-select field when dis-select.
	if ( chipnumber < 0 ) {
		chipnumber = 0x3 ;
		setting = (chipnumber << 16) ;
		v_outl( MOZART_NFC_MMR_CTRL, setting ) ;
		return ;
	}

	//step1. set timing
	v_outl( MOZART_NFC_MMR_CONF, chip_priv->nf_chip_timing ) ;

	//setp2. set ctrl settings
#ifdef MOZART_NFC_DISABLE_HW_ECC
	setting |= (chipnumber << 16) | (1 << 1);
#else
	setting |= (chipnumber << 16) | NF_FLCTRL_ECC_ENABLE | (1 << 1);    
#endif
	if ( mtd->writesize == 2048 ) {
		setting |= NF_FLCTRL_PAGESIZE_2KB ;
	}
	else if ( mtd->writesize == 4096 ) {
		setting |= NF_FLCTRL_PAGESIZE_4KB ;
	}

	//step3. set cycle#
	if (chip->chipsize <= (128 << 20)) {
		setting |= NF_FLCTRL_ADDRCYCLE_4 ;
	}
	else {
		setting |= NF_FLCTRL_ADDRCYCLE_5 ;
	}

	//step4. write setting
	v_outl( MOZART_NFC_MMR_CTRL, setting ) ;

	return ;
}

//Mozart NFC status bit field
#define MOZART_NFC_RnB_READY 0x2
#define MOZART_NFC_RnB_BUSY_MASK  ( 0x60 | 0X2 )
#define MOZART_NFC_DMA_BUSY_MASK  0x80
static int Mozart_nand_dev_ready( struct mtd_info *mtd )
{
	register struct nand_chip *chip = mtd->priv;
    struct Mozart_chip_private *chip_priv = chip->priv ;

    //Because TRANSFER-COMPLETE-INTERRUPT is only triggered when :
    //  1. READ/WRITE to nand flash chip(the operation must operate the nand flash chip directly)
    //  2. the command must be PageRead or PageProgram
    //So we should add "else condition" to handle other cases.
	if ((chip_priv->buf_mode_rw_op == 1) && (chip_priv->dma_mode)) {
		wait_for_completion( &NFC_TRANSFER_COMPLETE ) ;
		chip_priv->buf_mode_rw_op = 0 ;
		return 1 ;
	}
	else {
		volatile unsigned long status = v_inl(MOZART_NFC_MMR_STATE) ;
		return ( (status & MOZART_NFC_RnB_BUSY_MASK) == MOZART_NFC_RnB_READY ) ;
	}
}

static int Mozart_nand_program_erase_waitfunc( struct mtd_info *mtd, struct nand_chip *chip )
{
    while ( 1 ) {
        if ( chip->dev_ready(mtd) )
            break ;
    }

    chip->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
    return ((int)chip->read_byte(mtd));
}

static void Mozart_nand_command_waitfunc( struct mtd_info *mtd )
{
    nand_wait_ready( mtd ) ;
}

static void Mozart_nand_dma_ready( struct mtd_info *mtd )
{
	unsigned long status ;
    do {
        status = v_inl(MOZART_NFC_MMR_STATE) ;
    } while( (status & MOZART_NFC_DMA_BUSY_MASK) == MOZART_NFC_DMA_BUSY_MASK ) ;
}

static long NFC_set_status(int set, unsigned long val)
{
    static unsigned long status = 0 ;
    if(set) {
        status = val ;
    }

    return status ;
}

//In our NFC, we should clear status register by ourselves.
static void Mozart_nand_clear_STATE(void)
{
	v_outl( MOZART_NFC_MMR_STATE, 0x0) ;
	return ;
}

static void Mozart_nand_internalbuf_CPUMode( void )
{
    v_outl( MOZART_BRC_CTRL, 0 ) ;
	return ;
}

static void Mozart_nand_internalbuf_NFCMode( void )
{
  	v_outl( MOZART_BRC_CTRL, 1 ) ;
	return ;
}

static void Mozart_nand_command(struct mtd_info *mtd, unsigned int command, int column, int page_num)
{
	register struct nand_chip *chip = mtd->priv;
	unsigned long low_addr = 0x0, high_addr = 0x0 ;

    struct Mozart_chip_private *chip_priv = chip->priv ;
    chip_priv->buf_mode_rw_op = 0 ;

	// Step0. Clear status
	if (command == NAND_CMD_ERASE2)
        return ;

   	Mozart_nand_clear_STATE() ;

	/* Emulate NAND_CMD_READOOB */
	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	// Step1. Write Address
	if (column != -1 || page_num != -1) {
		/* Serially input address */
		if (column != -1) {
			low_addr = column ;
		}
		if (page_num != -1) {
			// We should add 1 to chip->page_shift
			low_addr = low_addr | (page_num << (chip->page_shift + 1)) ;

			// One more address cycle for devices > 128MiB
			if (chip->chipsize > (128 << 20)) {
				high_addr = high_addr | ( page_num >> 16 ) ;
			}
		}

		// READ and PROGRAM uses different sets of address registers
		if ( (command == NAND_CMD_READ0) || (command == NAND_CMD_PAGEREAD_BUFMODE) ) {
			//printk(KERN_DEBUG "rlo = 0x%lx, rhi = 0x%lx\n", low_addr, high_addr) ;
			v_outl( MOZART_NFC_MMR_ADDR0L, low_addr) ;
			v_outl( MOZART_NFC_MMR_ADDR0H, high_addr) ;
		}
		else if ( (command == NAND_CMD_PAGEPROG1) || (command == NAND_CMD_ERASE1) || (command == NAND_CMD_PAGEPROG_BUFMODE)) {
			//printk(KERN_DEBUG "wlo = 0x%lx, whi = 0x%lx\n", low_addr, high_addr) ;
			v_outl( MOZART_NFC_MMR_ADDR1L, low_addr) ;
			v_outl( MOZART_NFC_MMR_ADDR1H, high_addr) ;
		}
	}

	//Step2. Write command
	if ( command == NAND_CMD_READ0 ) {
		command = NAND_CMD_PAGEREAD1 ;//[jam] we use page-read#1 here
	}

	if ( (command == NAND_CMD_PAGEREAD_BUFMODE) || (command == NAND_CMD_PAGEPROG_BUFMODE) ) {
		Mozart_nand_internalbuf_NFCMode() ;
        chip_priv->buf_mode_rw_op = 1 ;
	}
	if ( command == NAND_CMD_PAGEPROG_BUFMODE ) {
		command = NAND_CMD_SEQIN ;
	}
	v_outl( MOZART_NFC_MMR_COMM, command) ;

	//Step3. Make sure that the action is completed.
	switch (command) {
		case NAND_CMD_CACHEDPROG: //[jam trace] it is cancelled now, see comments in nand_write_page()
		case NAND_CMD_RNDIN:
		case NAND_CMD_DEPLETE1:
		case NAND_CMD_ERASE2:
        case NAND_CMD_SEQIN:
			return ;

		case NAND_CMD_PAGEPROG:
		case NAND_CMD_PAGEPROG1:
		case NAND_CMD_STATUS:
		case NAND_CMD_ERASE1:
			udelay(chip->chip_delay);
			return;

		/*
		 * read error status commands require only a short delay
		 */
		case NAND_CMD_STATUS_ERROR:
		case NAND_CMD_STATUS_ERROR0:
		case NAND_CMD_STATUS_ERROR1:
		case NAND_CMD_STATUS_ERROR2:
		case NAND_CMD_STATUS_ERROR3:
			udelay(chip->chip_delay);
			return;

		case NAND_CMD_RESET:
			if (chip->dev_ready)
				break;
			udelay(chip->chip_delay);
			Mozart_nand_command_waitfunc( mtd ) ;//defined in nand_base.c
			Mozart_nand_clear_STATE() ;//in our NFC, we should clean this bit in state mmr
			return;

		case NAND_CMD_READ0:
		case NAND_CMD_PAGEREAD1:
		case NAND_CMD_PAGEREAD_BUFMODE:
#if ((AHB_CLK == 266666666) || (AHB_CLK == 200000000))
                       udelay(50);
#else
                       udelay(100);
#endif
			Mozart_nand_command_waitfunc( mtd ) ;//defined in nand_base.c
			Mozart_nand_clear_STATE() ;//in our NFC, we should clean this bit in state mmr
			Mozart_nand_internalbuf_CPUMode() ;
			return ;

			/* This applies to read commands */
		default:
			/*
			 * If we don't have access to the busy pin, we apply the given
			 * command delay
			 */
			if (!chip->dev_ready) {
				udelay(chip->chip_delay);
				return;
			}
	}

	/* Apply this short delay always to ensure that we do wait tWB in
	 * any case on any machine. */
	ndelay(100);

}

int Mozart_nand_get_desc_page_number(void* tmp, struct mtd_info* mtd)
{
	struct nand_bbt_descr* desc = tmp ;
	struct nand_chip* chip = mtd->priv ;
	int bits = desc->options & NAND_BBT_NRBITS_MSK;
	int bbt_length = ((chip->chipsize / mtd->erasesize) * bits) >> 3;

	bbt_length = (bbt_length + (mtd->writesize - 1)) & ~(mtd->writesize - 1);//make bbt_length page-aligned

	return (bbt_length / mtd->writesize) + 1 ;
}

/*------------------------------------------------------
 *                    Internal-DMA part
 *------------------------------------------------------
 */
static void Mozart_nand_dma_reset_ctrl( void )
{
    Mozart_nand_internalbuf_NFCMode() ;
    v_outl( MOZART_NFC_MMR_DMACTRL, 0 ) ;
    return ;
}

static void Mozart_nand_dma_set_memory_addr( unsigned long memory_addr )
{
	v_outl( MOZART_NFC_MMR_DMAADDR, memory_addr ) ;
	return ;
}

// We read/write data from/to internal buffer at address 0
static void Mozart_nand_dma_set_counter( unsigned long counter )
{
    unsigned long reg = v_inl( MOZART_NFC_MMR_DMACNTR ) ;
    counter = counter & 0xffff ;
    reg = (reg & 0x0000ffff) | (counter << 16) ;
	v_outl( MOZART_NFC_MMR_DMACNTR, reg) ;
	return ;
}

static void Mozart_nand_dma_set_srambase( unsigned long addr )
{
    unsigned long reg = v_inl( MOZART_NFC_MMR_DMACNTR ) ;
    addr = addr & 0xffff ;
    reg = (reg & 0xffff0000) | addr ;
    v_outl( MOZART_NFC_MMR_DMACNTR, reg) ;
	return ;
}

#define MOZART_NFC_DMA_BURSTTYPE    ( 2 << 11 )
#define MOZART_NFC_DMA_TRANSFERSIZE ( 0 << 14 ) //8-bits transfer size
#define MOZART_NFC_DMA_TRANSFER_AHB2SRAM ( 0 << 16 )
#define MOZART_NFC_DMA_TRANSFER_SRAM2AHB ( 1 << 16 )
#define MOZART_NFC_DMA_DMA_ENABLE   ( 1 << 24 )
void Mozart_nand_dma_start_transfer( unsigned long dir )
{
	unsigned long val = MOZART_NFC_DMA_BURSTTYPE | MOZART_NFC_DMA_TRANSFERSIZE | dir | MOZART_NFC_DMA_DMA_ENABLE ;
	v_outl( MOZART_NFC_MMR_DMACTRL, val ) ;
	return ;
}

void Mozart_nand_dma_end_transfer(void)
{
	v_outl( MOZART_NFC_MMR_DMACTRL, 0 ) ;
    Mozart_nand_internalbuf_CPUMode() ;
	return ;
}

int Mozart_nand_dma_error( void )
{
    unsigned long reg = v_inl( MOZART_NFC_MMR_DMACTRL ) ;

    return ((reg & 0x2) == 0x2) ? 1 : 0 ;//0x2 means dma error
}

int Mozart_nand_dma_transfer(struct mtd_info *mtd, uint8_t *buf, unsigned long sram_addr, int len, unsigned long dir, int oobmode)
{
	// In our NFC, we have read a page to internal buffer by Page-read command.
	// So the task of this function is "using dma"
	// to transfer data from "internal buffer" to "external memory".
    unsigned long busaddr = 0 ;
    busaddr = Mozart_get_handle( (unsigned long)(buf)) ;
    if (busaddr == 0) {
        busaddr = virt_to_bus((unsigned long)(buf)) ;
    }

    //flush memory
    flush_cache_all() ;

    //reset
    Mozart_nand_dma_reset_ctrl() ;

	//wait for dma ready
	Mozart_nand_dma_ready(mtd) ;

	//start dma transferring...
	Mozart_nand_dma_set_memory_addr( busaddr ) ;
	Mozart_nand_dma_set_counter( (unsigned long)(len) ) ;
    Mozart_nand_dma_set_srambase( sram_addr ) ;

    //Because we will move "data-part" between buffer and nand flash chip,
    //but we will read "oob-part" to the buffer of struct nand_chip.
    //And because the data of oob-part is few, we can just use dma-polling-mode to handle this.
    if (!oobmode) {
        struct nand_chip *chip = mtd->priv ;
	    struct Mozart_chip_private *chip_priv = chip->priv ;

        NFC_setDmaTriggerMode(mtd) ;
   	    unsigned long val = MOZART_NFC_DMA_BURSTTYPE | MOZART_NFC_DMA_TRANSFERSIZE | dir ;
    	v_outl( MOZART_NFC_MMR_DMACTRL, val ) ;

        if (dir == MOZART_NFC_DMA_TRANSFER_SRAM2AHB) {
            Mozart_nand_command(mtd, NAND_CMD_PAGEREAD_BUFMODE, 0x00, chip_priv->pageaddr );//give addr and command
        } else if (dir == MOZART_NFC_DMA_TRANSFER_AHB2SRAM) {
            Mozart_nand_command(mtd, NAND_CMD_PAGEPROG_BUFMODE, 0x00, chip_priv->pageaddr );//give addr and command
            unsigned long status = Mozart_nand_program_erase_waitfunc(mtd, chip) ;
            NFC_set_status( 1, status) ;
        } else {
            printk( "[ERROR] Wrong direction of dma transfer!\n" ) ;
        }
    }
    else {
	    Mozart_nand_dma_start_transfer( dir ) ;

    	//wait for dma ready
	    Mozart_nand_dma_ready(mtd) ;

        if ( Mozart_nand_dma_error() ) {
            if ( dir == MOZART_NFC_DMA_TRANSFER_AHB2SRAM) {
                printk( "[ERR] NFC-DMA Transfer from AHB to SRAM failed!!\n" ) ;
            }
            else {
                printk( "[ERR] NFC-DMA Transfer from SRAM to AHB failed!!\n" ) ;
            }
            return -1 ;
        }
    }

	//clear status and disable dma
	Mozart_nand_dma_end_transfer() ;
	Mozart_nand_clear_STATE() ;

  	//invalidate memory
  	if ( dir == MOZART_NFC_DMA_TRANSFER_SRAM2AHB ) {
       dmac_inv_range( (unsigned long)buf, ((unsigned long)buf + len)) ;
  	}
	else
		flush_cache_all() ;

	return 0 ;
}

/*------------------------------------------------------
 *                    read part
 *------------------------------------------------------
 */
/*
 * This function is only for READ-OPERATION.
 */
static int Mozart_nand_read_eccstatus( struct mtd_info *mtd )
{
	struct nand_chip *chip = mtd->priv ;
	int i ;
	unsigned long mask, eccstatus, error_flag, error_correct ;

	mask = 1 << (mtd->writesize / 512) ;
	mask -= 1 ;

	eccstatus = v_inl( MOZART_NFC_MMR_ECCState ) ;
	error_flag = (eccstatus >> 16) & mask ;
	error_correct = eccstatus & mask ;
	error_correct &= error_flag; // the only valid bits are those for which corresponding ecc error bits are set.

	if (likely(error_flag == 0)) {	// No Error Occurs
		return 0;
	}
	else {	// Error Occurs
		/* Error Corrected Successfully */
		if (error_flag == error_correct) {
			int n = 0;
			while(error_correct != 0) {
				n += (error_correct & 1);
				error_correct >>= 1 ;
			}
			printk("correct %d errors\n", n) ;
			return n;
		}
		else {
			/* check if it is a freshly-erased block
			 * (oob filled with 0xff), in this case the ECC will not come out right.
			 * We'll suppress the error and tell the caller everything's
			 * OK. Because it is. */
			int emptymatch = 1 ;
			for ( i = 0 ; i < mtd->oobsize ; i++ ) {
				if(chip->oob_poi[i] == 0xff)
					continue ;
				emptymatch = 0 ;
				break ;
			}
			/* if emptymatch == 1, the block has just been erased, return OK */
			return ((emptymatch) ? 0 : -1);
		}
	}
}

static int Mozart_nand_ecc_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf)
{
    struct Mozart_chip_private *chip_priv = chip->priv ;
   	int stat;
    int i, j ;
    unsigned long tmp ;

    if ( !(chip_priv->dma_mode) ) {
        Mozart_nand_internalbuf_CPUMode() ;

        i = 0 ;
	    while( i < mtd->writesize ) {
	    	tmp = v_inl( MOZART_BRC_SRAM_BASE + i ) ;
            for ( j = 0 ; j < 4 ; j++ ) {
	       		unsigned long mask = 0xff ;
		        int shift = j * 8 ;
		        buf[i] = (tmp & (mask << shift)) >> shift;
		      	i++ ;
		    }
        }

        i = 0 ;
	    while( i < mtd->oobsize ) {
		    tmp = v_inl( MOZART_BRC_SRAM_BASE + mtd->writesize + i ) ;
		    for ( j = 0 ; j < 4 ; j++ ) {
			    unsigned long mask = 0xff ;
			    int shift = j * 8 ;
			    chip->oob_poi[i] = (tmp & (mask << shift)) >> shift;
			    i++ ;
		    }
	    }
    }
    else {
        Mozart_nand_dma_transfer( mtd, buf, 0, mtd->writesize, MOZART_NFC_DMA_TRANSFER_SRAM2AHB, 0 ) ;
        Mozart_nand_dma_transfer( mtd, chip->oob_poi, mtd->writesize, mtd->oobsize, MOZART_NFC_DMA_TRANSFER_SRAM2AHB, 1 ) ;
    }

	stat = Mozart_nand_read_eccstatus(mtd);
	if (stat == -1)
		mtd->ecc_stats.failed++;
	else
		mtd->ecc_stats.corrected += stat;

	return 0;
}


static int Mozart_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf)
{
    struct Mozart_chip_private *chip_priv = chip->priv ;
   	int stat;
    int i, j ;
    unsigned long tmp ;

    if ( !(chip_priv->dma_mode) ) {
        Mozart_nand_internalbuf_CPUMode() ;

        i = 0 ;
	    while( i < mtd->writesize ) {
	    	tmp = v_inl( MOZART_BRC_SRAM_BASE + i ) ;
            for ( j = 0 ; j < 4 ; j++ ) {
	       		unsigned long mask = 0xff ;
		        int shift = j * 8 ;
		        buf[i] = (tmp & (mask << shift)) >> shift;
		      	i++ ;
		    }
        }

        i = 0 ;
	    while( i < mtd->oobsize ) {
		    tmp = v_inl( MOZART_BRC_SRAM_BASE + mtd->writesize + i ) ;

		    for ( j = 0 ; j < 4 ; j++ ) {
			    unsigned long mask = 0xff ;
			    int shift = j * 8 ;
			    chip->oob_poi[i] = (tmp & (mask << shift)) >> shift;
			    i++ ;
		    }
	    }
    }
    else {
		printk ("Mozart_NANDFlas.c. Mozart_nand_read_page_raw. DMA mode not enabled\n");
    }

	return 0;
}



/*------------------------------------------------------
 *                    write part
 *------------------------------------------------------
 */

static void Mozart_nand_ecc_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
    struct Mozart_chip_private *chip_priv = chip->priv ;

   if ( !(chip_priv->dma_mode) ) {
		int i = 0, j = 0 ;
		unsigned long tmp ;
		uint32_t *buf32 = (uint32_t *)buf ;

		Mozart_nand_internalbuf_CPUMode() ;

		while( i < mtd->writesize ) {
			tmp = buf32[j] ;
			v_outl( MOZART_BRC_SRAM_BASE + i, tmp ) ;
			i+=4 ;
			j++ ;
		}
    }
    else{
        Mozart_nand_dma_transfer( mtd, buf, 0, mtd->writesize, MOZART_NFC_DMA_TRANSFER_AHB2SRAM, 0) ;
    }
}
static void Mozart_nand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
    struct Mozart_chip_private *chip_priv = chip->priv ;

   if ( !(chip_priv->dma_mode) ) {
		int i = 0, j = 0 ;
		unsigned long tmp ;
		uint32_t *buf32 = (uint32_t *)buf ;

		Mozart_nand_internalbuf_CPUMode() ;

		while( i < mtd->writesize ) {
			tmp = buf32[j];
			v_outl( 0xf3001000 + i, tmp ) ;
			i+=4 ;
			j++ ;
		}

        i = 0;
        j = 0;
        buf32 = (uint32_t *) (chip->oob_poi);
	    while( i < mtd->oobsize ) {
			tmp = buf32[j];
			v_outl (MOZART_BRC_SRAM_BASE + mtd->writesize + i, tmp);
			i+=4;
			j++;
	    }
    }
    else{
		printk ("Mozart_NANDFlas.c. Mozart_nand_write_page_raw. DMA mode not enabled\n");
    }
}

static int Mozart_nand_write_page(struct mtd_info *mtd, struct nand_chip *chip,
		const uint8_t *buf, int page, int cached, int raw)
{
	int status;
    struct Mozart_chip_private *chip_priv = chip->priv ;

#if 0
	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG1, 0x00, page);//[jam]write page prog1 command

	if (unlikely(raw))
		chip->ecc.write_page_raw(mtd, chip, buf);  //[jam]write data
	else
		chip->ecc.write_page(mtd, chip, buf);

	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);//[jam]write page command
#endif
#if 1
    //printk("write page..\n") ;
    if( chip_priv->dma_mode ) {
        if (unlikely(raw)) {
	    	chip->ecc.write_page_raw(mtd, chip, buf);  //[jam]write data
	    	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG_BUFMODE, 0x00, page);//[jam]write page command
	    } else {
	        chip_priv->pageaddr = page ;
	    	chip->ecc.write_page(mtd, chip, buf);
	    }

        status = NFC_set_status( 0, 0) ;
        //return 0;
        //status = chip->waitfunc(mtd, chip);
    }
    else {
	    if (unlikely(raw)) {
	    	chip->ecc.write_page_raw(mtd, chip, buf);  //[jam]write data
	    } else {
	    	chip->ecc.write_page(mtd, chip, buf);
	    }

    /*printk( "after dma moving...\n" ) ;
    printk( "brc ver %08x\n", v_inl( MOZART_BRC_VERSION ) );
    printk( "brc ctrl %08x\n", v_inl( MOZART_BRC_CTRL ) ) ;
    int i = 0 ;
    while( i < 2048 ) {
        unsigned long reg = v_inl( 0xf3001000 + i ) ;
        printk( "%08x\n", reg ) ;

        i+=4;
    }*/

	    chip->cmdfunc(mtd, NAND_CMD_PAGEPROG_BUFMODE, 0x00, page);//[jam]write page command

    /*volatile unsigned long timedelay = 0xffff ;
    while( timedelay != 0 )
        timedelay-- ;*/
        status = chip->waitfunc(mtd, chip);
    }
#endif


/*    printk( "after writing...\n" ) ;
//    int i ;
    for ( i = 0 ; i < mtd->writesize ; i++ ) {
        if ( (i%4) == 0) {
            printk( "\n" ) ;
        }
        printk( "%02x, ", buf[i] ) ;
    }
    printk( "\n" ) ;
    printk( "====================\n" ) ;
  */
	/*
	 * See if operation failed and additional status checks are
	 * available
	 */
	if ((status & NAND_STATUS_FAIL) && (chip->errstat)) {
		status = chip->errstat(mtd, chip, FL_WRITING, status, page);
	}

	if (status & NAND_STATUS_FAIL) {
		return -EIO;
	}

	return 0;
}

/*------------------------------------------------------
 *                    remove part
 *------------------------------------------------------
 */
static int Mozart_nand_remove(struct platform_device *pdev)
{
	int err = 0 ;

	DBG( "Mozart_nand_remove() is called, it terminates Mozart NAND-Flash now.\n" ) ;

	return err ;
}

/*------------------------------------------------------
 *                    ISR Part
 *------------------------------------------------------
 */
static irqreturn_t NFC_ISR( int int_num, void *dev_id)
{
    unsigned long status = v_inl(MOZART_NFC_MMR_STATE) ;

	//printk(KERN_DEBUG "%s\n", __func__) ;

    //if ((status & MOZART_NFC_DMA_BUSY_MASK) != MOZART_NFC_DMA_BUSY_MASK) {
	if (((status & MOZART_NFC_RnB_READY) == MOZART_NFC_RnB_READY) &&
		 ((status & MOZART_NFC_DMA_BUSY_MASK) != MOZART_NFC_DMA_BUSY_MASK)
		) {
          Mozart_nand_dma_reset_ctrl() ;
        Mozart_nand_clear_STATE() ;
        complete( &NFC_TRANSFER_COMPLETE );
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}


/*------------------------------------------------------
 *                    probe part
 *------------------------------------------------------
 */
static int Mozart_nand_scan(struct mtd_info *mtd, struct nand_chip *chip, struct Mozart_chip_private *chip_priv, int DeviceSize)
{
	//int err = 0 ;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *mtd_parts = 0;
	int mtd_parts_nr = 0;
#endif
    int ret ;

	//struct mtd_info init
	mtd->numeraseregions = 0 ;//fixed erase size

	//struct nand_chip init
	chip->options |= NAND_NO_SUBPAGE_WRITE | NAND_USE_FLASH_BBT;//[jam test]NAND_BBT_SCANEMPTY for testing...
	chip->IO_ADDR_W = MOZART_NFC_MMR_DATA ;
	chip->IO_ADDR_R = chip->IO_ADDR_W ;

	chip->select_chip = Mozart_nand_select_chip;
	chip->cmdfunc = Mozart_nand_command;
	chip->dev_ready = Mozart_nand_dev_ready;
	chip->cmd_ctrl = Mozart_nand_hwcontrol ;//This is used to issue command and address cycles to the chip (CLE/ALE)
	chip->write_page = Mozart_nand_write_page ;
    chip->waitfunc = Mozart_nand_program_erase_waitfunc ;

	chip->ecc.read_page_raw = Mozart_nand_read_page_raw ;
	chip->ecc.write_page_raw = Mozart_nand_write_page_raw ;
	chip->ecc.read_page = Mozart_nand_ecc_read_page_hwecc ;
	chip->ecc.write_page = Mozart_nand_ecc_write_page_hwecc ;
	//chip->ecc.layout = &Mozart_nand_oob_64 ;
    //chip->ecc.layout = &Mozart_nand_oob_128 ;
	chip->ecc.calculate = Mozart_nand_calculate ;
	chip->ecc.correct = Mozart_nand_correct ;
	chip->ecc.hwctl = Mozart_nand_hwctl ;
#ifdef MOZART_NFC_DISABLE_HW_ECC
	chip->ecc.mode = NAND_ECC_SOFT;
#else
	chip->ecc.mode = NAND_ECC_HW ;
#endif
	chip->ecc.size = 0;

	//struct Mozart_chip_private init
	chip_priv->nf_chip_timing = 0xffffffff ;//default timing
	chip_priv->get_desc_page_number = Mozart_nand_get_desc_page_number ;
	mutex_init(&chip_priv->nand_erasecheck_mutex);

	/*
	 * New patched in latest version of kernel
	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
	 * after power-up.(Before calling nand_get_flash_type() )
	 */
	//chip->cmdfunc(mtd, NAND_CMD_RESET, 0x00, -1);

    //[patch add]
    //Because in nand_scan_bbt(), it will use vmalloc() to alloc a buffer > 128KB.
    //DMA cannot handle its un-continutive space, so we use CPUMode.
    chip_priv->dma_mode = 0 ;
	ret = nand_scan( mtd, 1 ) ;
    if(ret)
        return ret ;
	chip_priv->nand_erasecheck_pagebuf = kmalloc(mtd->writesize, GFP_KERNEL);
#ifdef CONFIG_MTD_MOZART_NANDFLASH_DMAMODE || defined CONFIG_MTD_ROSSINI_NANDFLASH_DMAMODE
    chip_priv->dma_mode = 1 ;
#else
	chip_priv->dma_mode = 0 ;
#endif

#ifdef CONFIG_MTD_PARTITIONS
#ifdef CONFIG_MTD_CMDLINE_PARTS
	mtd_parts_nr = parse_mtd_partitions(mtd, part_probes, &mtd_parts, 0);
#endif //end CONFIG_MTD_CMDLINE_PARTS
	if (mtd_parts_nr <= 0) {
		printk("Using default nand partition..\n") ;
        mtd_parts = board_nand_default_partition;
    	mtd_parts_nr = ARRAY_SIZE(board_nand_default_partition);
	}

	if (mtd_parts_nr > 0) {
#ifdef DATAFLASH_ALWAYS_ADD_DEVICE
		add_mtd_device(mtd);
#endif //end DATAFLASH_ALWAYS_ADD_DEVICE
		return add_mtd_partitions(mtd, mtd_parts, mtd_parts_nr);
	}
#endif //CONFIG_MTD_PARTITIONS
	return add_mtd_device(mtd);		// add whole device

}

static int Mozart_nand_probe(struct platform_device *pdev)
{
	struct mtd_info *mtd = NULL ;
	struct nand_chip *chip = NULL ;
	struct Mozart_chip_private *chip_priv = NULL;

	int err = 0 ;

	mtd = kzalloc(sizeof(*mtd), GFP_KERNEL) ;
	if (mtd == NULL) {
		DBG( "[ERR]No memory for mtd_info of Mozart_nand\n" );
		err = -ENOMEM;
		goto probe_exit_error;
	}

	chip = kzalloc(sizeof(*chip), GFP_KERNEL) ;
	if (chip == NULL) {
		DBG( "[ERR]No memory for nand_chip of Mozart_nand\n" );
		err = -ENOMEM;
		goto probe_exit_error;
	}

	chip_priv = kzalloc(sizeof(*chip_priv), GFP_KERNEL) ;
	if (chip_priv == NULL) {
		DBG( "[ERR]No memory for chip_priv of Mozart_nand\n" );
		err = -ENOMEM;
		goto probe_exit_error;
	}

    //Set the min drv trength of NFC(2mA).
    unsigned long drv_strength = v_inl(IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x5C) ;
    drv_strength &= ~(0x3 << 8) ;
    v_outl(IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x5C, drv_strength) ;

	//main structure assignment
	chip->priv = chip_priv ;
	mtd->priv = chip ;
	mtd->owner = THIS_MODULE;

	//we init fields of struct nand_chip
	if( Mozart_nand_scan(mtd, chip, chip_priv, SZ_512M) != 0 ) {
		DBG( "[ERR]init nandchip failed!!\n" );
		err = -1;
		goto probe_exit_error;
	}

	//Only DMA mode needs ISR.
	if( request_irq( NFC_IRQ_NUM, &NFC_ISR, IRQF_DISABLED, pdev->name, pdev->id) != 0 )     /* request non-shared interrupt */
	{
		printk(  "NFC(%s) - interrupt %d request fail\n", pdev->name, pdev->id );
		return -ENODEV;
	}

	return 0 ;

probe_exit_error :

    if(mtd != NULL)
        kfree(mtd) ;

    if(chip != NULL)
        kfree(chip) ;

    if(chip_priv != NULL)
        kfree(chip_priv) ;

	Mozart_nand_remove(pdev);

	if (err == 0)
		err = -EINVAL;
	return err;
}


/*------------------------------------------------------
 *                    module part
 *------------------------------------------------------
 */
static struct platform_driver Mozart_nand_driver = {
	.probe		= Mozart_nand_probe,
	.remove		= Mozart_nand_remove,
	.driver		= {
		.name	= "Mozart-nand",
		.owner	= THIS_MODULE,
	},
};

static int __init Mozart_nand_module_init(void)
{
	DBG("Mozart NAND Driver\n");

	return platform_driver_register(&Mozart_nand_driver);
}

static void __exit Mozart_nand_module_exit(void)
{
	platform_driver_unregister(&Mozart_nand_driver);
}

module_init(Mozart_nand_module_init);
module_exit(Mozart_nand_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("James.Lin");
MODULE_DESCRIPTION("Mozart NAND driver");
