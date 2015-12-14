#include <common.h>

#include <malloc.h>
#include <watchdog.h>
#include <linux/err.h>
#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif

#include <asm/io.h>
#include <asm/errno.h>

#ifdef CONFIG_JFFS2_NAND
#include <jffs2/jffs2.h>
#endif

#include <asm/arch/platform.h>
#include <asm/sizes.h>
#include <nand.h>
#include <linux/math64.h>

/*------------------------------------------------------
 *                    Global Variables Part
 *------------------------------------------------------
 * We put global variables, macros, and structures in this part.
 */
//related mmr definitions are in include/asm/arch/mozart/mozart.h
#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))
#define v_inb(addr)		(*((volatile unsigned char *)(addr)))

#ifdef JAMES_DBG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif

// Enable this for SW ECC; Disable for HW ECC (Also in nand_base.c)
//#define MOZART_NFC_DISABLE_HW_ECC 1

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition partitions_128M[] =
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

static struct mtd_partition partitions_512M[] =
{
	{
		.name		= "Mozart bootloader",
		.offset		= 0,
		.size		= 64*2048*10,	/* 1st block = 64 pages = 64 * 2048B */
		.mask_flags	= MTD_WRITEABLE,	/* read-only */
	},
	{
		.name		= "Mozart others",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,	/* rest = 9 sectors */
	}
};
#endif

//[jam trace] Remember, we use all oob to store HW ECC.
#ifdef MOZART_NFC_DISABLE_HW_ECC
// For SW ECC
struct nand_ecclayout Mozart_nand_oob_64 = {
	.eccbytes = 24,
	.eccpos = {
		   40, 41, 42, 43, 44, 45, 46, 47,
		   48, 49, 50, 51, 52, 53, 54, 55,
		   56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = {
		{.offset = 2,
		 .length = 38}}
};
#else
struct nand_ecclayout nand_oob_64 = {
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
#endif

struct nand_ecclayout nand_oob_128 = {
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


/*------------------------------------------------------
 *                    redundant part
 *------------------------------------------------------
 * We declare these redundant functions because when HW ECC mode is enabled, 
 * kernel would check if these functions are already implemented.
 * But in our hw ecc mode, we do not use these functions.
 */
int	nand_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
	printf( "[jam ERR] Our NFC does not need this, you should not call nand_calculate()\n" ) ;
	return -1 ;
}

int nand_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
	printf( "[jam ERR] Our NFC does not need this, you should not call nand_correct()\n" ) ;
	return -1 ;
}

void nand_hwctl(struct mtd_info *mtd, int mode)
{
	printf( "[jam ERR] Our NFC does not need this, you should not call nand_hwctl()\n" ) ;
	return ;
}

static void nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	printf( "[jam ERR] Our NFC does not need this, you should not call nand_hwcontrol()\n" ) ;
	return ;
}

/*------------------------------------------------------
 *                    Misc Part
 *------------------------------------------------------
 * This is a important part. We collect set functions, command function, and status functions here.
 */
#define NF_FLCTRL_BLOCK_64PAGES        (0 << 12)
#define NF_FLCTRL_BLOCK_128PAGES       (1 << 12)
#define NF_FLCTRL_PAGESIZE_4KB         (0 << 11)
#define NF_FLCTRL_PAGESIZE_2KB         (1 << 11)
#define NF_FLCTRL_ECC_DISABLE          (0 <<  9)
#define NF_FLCTRL_ECC_ENABLE           (1 <<  9)
#define NF_FLCTRL_DMATRIGGER_DISABLE   (0 <<  3)
#define NF_FLCTRL_DMATRIGGER_ENABLE    (1 <<  3)
#define NF_FLCTRL_SEPERATE_RNB         (0 <<  1)
#define NF_FLCTRL_SHARE_RNB            (1 <<  1)
#define NF_FLCTRL_ADDRCYCLE_4          (0 <<  0)
#define NF_FLCTRL_ADDRCYCLE_5          (1 <<  0)
void nand_select_chip(struct mtd_info *mtd, int chipnumber)
{
    /* We not only select the chip, but also config it. */
	
	struct nand_chip *chip = mtd->priv;
	struct vpl_nand_chip_private *chip_priv = chip->priv ;
	unsigned long setting = 0x0 ;

	// Since we only use #0 ~ (#CONFIG_SYS_MAX_NAND_DEVICE-1),
	// #CONFIG_SYS_MAX_NAND_DEVICE will be safe to dis-select other all signals.
	if ( chipnumber < 0 ) {		
		chipnumber = CONFIG_SYS_MAX_NAND_DEVICE ;
		setting = (chipnumber << 16) ;
		v_outl( VPL_NFC_MMR_CTRL, setting ) ;
		return ;
	}

	//step1. set timing
	//static int assign_timing = 0 ;
    //if ( assign_timing == 0 ) {
    //    assign_timing = 1 ;
    	//printf("chip_priv->nf_chip_timing = %x\n", chip_priv->nf_chip_timing) ;
        v_outl( VPL_NFC_MMR_CONF, chip_priv->nf_chip_timing ) ;
    //}

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

	if(chip_priv->dma_mode)
		setting |= NF_FLCTRL_DMATRIGGER_ENABLE ;
	else
		setting |= NF_FLCTRL_DMATRIGGER_DISABLE ;

	//step4. write setting
    v_outl( VPL_NFC_MMR_CTRL, setting ) ;

	return ;
}

//Mozart NFC status bit field
#define VPL_NFC_RnB_READY 0x2
#define VPL_NFC_RnB_BUSY_MASK  ( 0x60 | 0X2 )
#define VPL_NFC_DMA_BUSY_MASK  0x80
static int nand_dev_ready( struct mtd_info *mtd ) 
{
    unsigned long status = v_inl(VPL_NFC_MMR_STATE) ;

	return ( (status & VPL_NFC_RnB_BUSY_MASK) == VPL_NFC_RnB_READY ) ;   
}

static void nand_command_waitfunc( struct mtd_info *mtd )
{  
    nand_wait_ready( mtd ) ;
}

static void nand_wait_reset( struct mtd_info *mtd)
{
    struct nand_chip *chip = mtd->priv;
    volatile int timeout = 1000 ;
    while(timeout >= 0) {
        timeout-- ;
    }

    timeout = 1000 ;
    chip->cmdfunc(mtd, NAND_CMD_STATUS, 0x00, -1);
    while (timeout >= 0){
        if(chip->read_byte(mtd) & NAND_STATUS_READY) {
            //printf( "In Mozart_nand_wait_reset(), It's ready!!") ;
            return ;
        }
        timeout-- ;
    }      
    
}

static int nand_program_erase_waitfunc( struct mtd_info *mtd, struct nand_chip *chip )
{
	while ( 1 ) {
		if ( chip->dev_ready(mtd) )
			break ;
    }
	
    chip->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
    return ((int)chip->read_byte(mtd));
}

static void nand_dma_ready( struct mtd_info *mtd ) 
{
	unsigned long status ;
	do {
          status = v_inl(VPL_NFC_MMR_STATE) ; 	
    } while((status & 0x60) != 0) ;
}

//In our NFC, we should clear status register by ourselves.
static void nand_clear_STATE(void)
{
    v_outl( VPL_NFC_MMR_STATE, 0x0 ) ;
	return ;
}

static void nand_internalbuf_CPUMode( void ) 
{
    v_outl( VPL_BRC_CTRL, 0x0 ) ;

	return ;
}

static void nand_internalbuf_NFCMode( void ) 
{
  	v_outl( VPL_BRC_CTRL, 0x1 ) ;
	return ;
}

static void nand_command(struct mtd_info *mtd, unsigned int command, int column, int page_addr)
{
	register struct nand_chip *chip = mtd->priv;
	unsigned long low_addr = 0x0, high_addr = 0x0 ;

    if (command == NAND_CMD_ERASE2)
        return ;

	// Step0. Clear status
	nand_clear_STATE() ;

	/* Emulate NAND_CMD_READOOB */
	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	// Step1. Write Address
	if (column != -1 || page_addr != -1) {
		/* Serially input address */
		if (column != -1) {
			low_addr = column ;
		}
		if (page_addr != -1) {			
			// We should add 1 to chip->page_shift
			low_addr = low_addr | (page_addr << (chip->page_shift + 1)) ;

			// One more address cycle for devices > 128MiB
			if (chip->chipsize > (128 << 20)) {
				high_addr = high_addr | ( page_addr >> 16 ) ;
			}

		}

		// READ and PROGRAM uses different sets of address registers
		if ( (command == NAND_CMD_READ0) || (command == NAND_CMD_PAGEREAD_BUFMODE) ) {			
            v_outl( VPL_NFC_MMR_ADDR0L, low_addr ) ;
			v_outl( VPL_NFC_MMR_ADDR0H, high_addr ) ;
		}
		else if ( (command == NAND_CMD_PAGEPROG1) || (command == NAND_CMD_ERASE1) || (command == NAND_CMD_PAGEPROG_BUFMODE)) {
			v_outl( VPL_NFC_MMR_ADDR1L, low_addr ) ;
			v_outl( VPL_NFC_MMR_ADDR1H, high_addr ) ;
		}
	}

	//Step2. Write command
	if ( command == NAND_CMD_READ0 ) {
		command = NAND_CMD_PAGEREAD1 ;//[jam] we use page-read#1 here
	} 

	if ( (command == NAND_CMD_PAGEREAD_BUFMODE) || (command == NAND_CMD_PAGEPROG_BUFMODE) ) {
		nand_internalbuf_NFCMode() ;
	}
	if ( command == NAND_CMD_PAGEPROG_BUFMODE ) {
		command = NAND_CMD_SEQIN ;
	}

    v_outl( VPL_NFC_MMR_COMM, command ) ;

	//Step3. Make sure that the action is completed.
	switch (command) {		
		case NAND_CMD_CACHEDPROG: //[jam trace] it is cancelled now, see comments in nand_write_page()			
		case NAND_CMD_SEQIN:
		case NAND_CMD_RNDIN:
		case NAND_CMD_DEPLETE1:          
		case NAND_CMD_ERASE2:          
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
            nand_wait_reset(mtd) ;
			nand_clear_STATE() ;//in our NFC, we should clean this bit in state mmr
			return;

		case NAND_CMD_READ0:
		case NAND_CMD_PAGEREAD1:
		case NAND_CMD_PAGEREAD_BUFMODE:  
#if ((AHB_CLOCK == 266666666) || (AHB_CLOCK == 200000000)) 
			udelay(50);
#else
			udelay(100);
#endif
			nand_command_waitfunc( mtd ) ;//defined in nand_base.c
			nand_clear_STATE() ;//in our NFC, we should clean this bit in state mmr
			nand_internalbuf_CPUMode() ;
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

int nand_get_desc_page_number(void* tmp, struct mtd_info* mtd) 
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
static void nand_dma_reset_ctrl( void )
{
    nand_internalbuf_NFCMode() ;
    v_outl( VPL_NFC_MMR_DMACTRL, 0x0 ) ;//clear ctrl
    return ;
}

static void nand_dma_set_memory_addr( unsigned long memory_addr ) 
{
  	v_outl( VPL_NFC_MMR_DMAADDR, memory_addr ) ;
	return ;
}

// We read/write data from/to internal buffer at address 0
static void nand_dma_set_counter( unsigned long counter ) 
{
    unsigned long reg = v_inl( VPL_NFC_MMR_DMACNTR ) ;
    counter = counter & 0xffff ;
	counter /= 4;//transfer-size : 32bits
	reg = (reg & 0x0000ffff) | (counter << 16) ;
	v_outl( VPL_NFC_MMR_DMACNTR, reg ) ;    
	return ;
}

static void nand_dma_set_srambase( unsigned long addr )
{
    unsigned long reg = v_inl( VPL_NFC_MMR_DMACNTR ) ;
    addr = addr & 0xffff ;
    reg = (reg & 0xffff0000) | addr ;
    v_outl( VPL_NFC_MMR_DMACNTR, reg ) ;
	return ;    
}

#define VPL_NFC_DMA_BURSTTYPE    ( 6 << 11 ) // 16 beat burst address increment
#define VPL_NFC_DMA_TRANSFERSIZE ( 3 << 14 ) // 32-bits transfer size
#define VPL_NFC_DMA_TRANSFER_AHB2SRAM ( 0 << 16 )
#define VPL_NFC_DMA_TRANSFER_SRAM2AHB ( 1 << 16 )
#define VPL_NFC_DMA_DMA_ENABLE   ( 1 << 24 )
void nand_dma_start_transfer( unsigned long dir ) 
{
	unsigned long ctrl = VPL_NFC_DMA_BURSTTYPE | VPL_NFC_DMA_TRANSFERSIZE | dir;// | VPL_NFC_DMA_DMA_ENABLE ;
	v_outl( VPL_NFC_MMR_DMACTRL, ctrl ) ;
	return ;
}

void nand_dma_end_transfer(void) 
{
    v_outl( VPL_NFC_MMR_DMACTRL, 0 ) ;
    nand_internalbuf_CPUMode() ;
	return ;
}

int nand_dma_error( void ) 
{
    unsigned long reg = v_inl( VPL_NFC_MMR_DMACTRL ) ;

    return ((reg & 0x2) == 0x2) ? 1 : 0 ;//0x2 means dma error
}

unsigned long dma_page_addr = 0 ;
extern unsigned long sysc_64bits_counter1, sysc_64bits_counter2 ;
int Mozart_nand_dma_transfer_SRAM2AHB(struct mtd_info *mtd, uint8_t *buf, unsigned long sram_addr, int len)
{
	// In our NFC, we have read a page to internal buffer by Page-read command.
	// So the task of this function is "using dma" 
	// to transfer data from "internal buffer" to "external memory".
    unsigned long busaddr = (unsigned long)(buf) ;

    //reset    
    nand_dma_reset_ctrl() ;

	//wait for dma ready
	nand_dma_ready(mtd) ;
	
	//start dma transferring...
	nand_dma_set_memory_addr( busaddr ) ;
	nand_dma_set_counter( (unsigned long)(len) ) ;
    nand_dma_set_srambase( sram_addr ) ;

	nand_dma_start_transfer( VPL_NFC_DMA_TRANSFER_SRAM2AHB ) ;		

	v_outl(VPL_NFC_MMR_CTRL, v_inl(VPL_NFC_MMR_CTRL) | (1 << 3)) ;//enable DMA trigger mode	
	//READ
	v_outl(VPL_NFC_MMR_ADDR0L, dma_page_addr << 12) ;
	v_outl(VPL_NFC_MMR_ADDR0H, 0) ;
	v_outl(VPL_NFC_MMR_COMM, NAND_CMD_PAGEREAD_BUFMODE) ;
		
	//wait for dma ready
	nand_dma_ready(mtd) ;

    if ( nand_dma_error() ) {
        printf( "[ERR] NFC-DMA Transfer from SRAM to AHB failed!!\n" ) ;        
        return -1 ;        
    }    
    
	//clear status and disable dma
	nand_dma_end_transfer() ;
	nand_clear_STATE() ;

	return 0 ;
}

int Mozart_nand_dma_transfer_AHB2SRAM(struct mtd_info *mtd, const uint8_t *buf, unsigned long sram_addr, int len)
{
	// In our NFC, we have read a page to internal buffer by Page-read command.
	// So the task of this function is "using dma" 
	// to transfer data from "internal buffer" to "external memory".
    unsigned long busaddr = (unsigned long)(buf) ;

    //reset    
    nand_dma_reset_ctrl() ;

	//wait for dma ready
	nand_dma_ready(mtd) ;
	
	//start dma transferring...
	nand_dma_set_memory_addr( busaddr ) ;
	nand_dma_set_counter( (unsigned long)(len) ) ;
    nand_dma_set_srambase( sram_addr ) ;

	nand_dma_start_transfer( VPL_NFC_DMA_TRANSFER_AHB2SRAM ) ;		

	v_outl(VPL_NFC_MMR_CTRL, v_inl(VPL_NFC_MMR_CTRL) | (1 << 3)) ;//enable DMA trigger mode	
	v_outl(VPL_NFC_MMR_ADDR1L, dma_page_addr << 12) ;
	v_outl(VPL_NFC_MMR_ADDR1H, 0) ;
	v_outl(VPL_NFC_MMR_COMM, 0x80) ;
	
	//wait for dma ready
	nand_dma_ready(mtd) ;

    if ( nand_dma_error() ) {        
		printf( "[ERR] NFC-DMA Transfer from AHB to SRAM failed!!\n" ) ;
        return -1 ;        
    }    
    
	//clear status and disable dma
	nand_dma_end_transfer() ;
	nand_clear_STATE() ;

	return 0 ;
}

/*------------------------------------------------------
 *                    read part
 *------------------------------------------------------
 */
/*
 * This function is only for READ-OPERATION.
 */
static int nand_read_eccstatus( struct mtd_info *mtd ) 
{
	struct nand_chip *chip = mtd->priv ;
	int result ;
	int i ;    
	unsigned long mask, eccstatus, error_flag, error_correct ;     

	mask = 1 << (mtd->writesize / 512) ;
	mask -= 1 ;

  	eccstatus = v_inl( VPL_NFC_MMR_ECCState ) ;
	error_flag = (eccstatus >> 16) & mask ;
	error_correct = eccstatus & mask ;

	result = -1 ;
	/* No Error Occurs */
	if (error_flag == 0) {
		result = 0 ;
	}
	else {
		/* Error Corrects Successfully */
		if (error_flag == error_correct) {            
            result = 0 ;
            while(error_correct != 0) {
                error_correct >>= 1 ;
                result++;
            }
            if (result != 0) {
                printf("correct %d errors\n", result) ;
            }
            while(error_correct != 0) {
                result++ ;
                error_correct >>= 1 ;
            }
            printf("correct result = %d\n", result) ;
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
			result = (emptymatch)? 0 : -1 ;
		}
	}

	return result ;
}

static int nand_ecc_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf)
{
    struct vpl_nand_chip_private *chip_priv = chip->priv ;

    if ( !(chip_priv->dma_mode) ) {	
        nand_internalbuf_CPUMode() ;
        
	    int i = 0, j ;
	    unsigned long tmp ;

	    while( i < mtd->writesize ) {
	    	tmp = v_inl( VPL_BRC_SRAM_BASE + i ) ;
            for ( j = 0 ; j < 4 ; j++ ) {
	       		unsigned long mask = 0xff ;
		        int shift = j * 8 ;                
		        buf[i] = (tmp & (mask << shift)) >> shift;
		      	i++ ;
		    }
        }

	    i = 0 ;        
	    while( i < mtd->oobsize ) {
		    tmp = v_inl( VPL_BRC_SRAM_BASE + mtd->writesize + i ) ;
		    for ( j = 0 ; j < 4 ; j++ ) {
			    unsigned long mask = 0xff ;
			    int shift = j * 8 ;                
			    chip->oob_poi[i] = (tmp & (mask << shift)) >> shift;
			    i++ ;
		    }
	    }
    }
    else {   
		//printf("dma-read\n") ;
        //Read Data Part
        Mozart_nand_dma_transfer_SRAM2AHB( mtd, buf, 0, mtd->writesize) ;
        //Read OOB Part
        Mozart_nand_dma_transfer_SRAM2AHB( mtd, chip->oob_poi, mtd->writesize, mtd->oobsize) ;
    }

  	int stat;
	stat = nand_read_eccstatus(mtd);
	//stat = 0 ;
	if (stat == -1)
		mtd->ecc_stats.failed++;
	else
		mtd->ecc_stats.corrected += stat;	

	return 0;
}

static int nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf)
{
    struct vpl_nand_chip_private *chip_priv = chip->priv ;
   	int stat;
    int i, j ;
    unsigned long tmp ;

    if ( !(chip_priv->dma_mode) ) {
        nand_internalbuf_CPUMode() ;

        i = 0 ;
	    while( i < mtd->writesize ) {
	    	tmp = v_inl( VPL_BRC_SRAM_BASE + i ) ;
            for ( j = 0 ; j < 4 ; j++ ) {
	       		unsigned long mask = 0xff ;
		        int shift = j * 8 ;
		        buf[i] = (tmp & (mask << shift)) >> shift;
		      	i++ ;
		    }
        }

        i = 0 ;
	    while( i < mtd->oobsize ) {
		    tmp = v_inl( VPL_BRC_SRAM_BASE + mtd->writesize + i ) ;

		    for ( j = 0 ; j < 4 ; j++ ) {
			    unsigned long mask = 0xff ;
			    int shift = j * 8 ;
			    chip->oob_poi[i] = (tmp & (mask << shift)) >> shift;
			    i++ ;
		    }
	    }
    }
    else {
		printf ("mozart_nand.c. nand_read_page_raw. DMA mode not enabled\n");
    }

	return 0;
}

/*------------------------------------------------------
 *                    write part
 *------------------------------------------------------
 */

static void nand_ecc_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
    struct vpl_nand_chip_private *chip_priv = chip->priv ;

    if ( !(chip_priv->dma_mode) ) {        
		int i = 0, j = 0 ;
		unsigned long tmp ;
		uint32_t *buf32 = (uint32_t *)buf ;

		nand_internalbuf_CPUMode() ; 

		while( i < mtd->writesize ) {
			tmp = buf32[j] ;
			v_outl( VPL_BRC_SRAM_BASE + i, tmp ) ;
			i+=4 ;
			j++ ;
		}
    }
    else{        
        Mozart_nand_dma_transfer_AHB2SRAM( mtd, buf, 0, mtd->writesize) ;
    }

}

static void nand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
    struct vpl_nand_chip_private *chip_priv = chip->priv ;

   if ( !(chip_priv->dma_mode) ) {
		int i = 0, j = 0 ;
		unsigned long tmp ;
		uint32_t *buf32 = (uint32_t *)buf ;

		nand_internalbuf_CPUMode() ;

		while( i < mtd->writesize ) {
			tmp = buf32[j];
			v_outl( VPL_BRC_SRAM_BASE + i, tmp ) ;
			i+=4 ;
			j++ ;
		}

        i = 0;
        j = 0;
        buf32 = (uint32_t *) (chip->oob_poi);
	    while( i < mtd->oobsize ) {
			tmp = buf32[j];
			v_outl (VPL_BRC_SRAM_BASE + mtd->writesize + i, tmp);
			i+=4;
			j++;
	    }
    }
    else{
		printf ("mozart_nand.c. nand_write_page_raw. DMA mode not enabled\n");
    }
}
static int nand_write_page(struct mtd_info *mtd, struct nand_chip *chip,
		const uint8_t *buf, int page, int cached, int raw)
{
	int status;
	struct vpl_nand_chip_private *chip_priv = chip->priv ;

	if (unlikely(raw)) {  
		chip->ecc.write_page_raw(mtd, chip, buf);  //[jam]write data
	} else {
		chip->ecc.write_page(mtd, chip, buf);        
	}

	if ( !(chip_priv->dma_mode) ) {  
		chip->cmdfunc(mtd, NAND_CMD_PAGEPROG_BUFMODE, 0x00, page);//[jam]write page command		
		status = chip->waitfunc(mtd, chip); 		
	}
	else {
		chip->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
    	status = (int)chip->read_byte(mtd);
	}

	/*
	 * See if operation failed and additional status checks are
	 * available
	 */
	if ((status & NAND_STATUS_FAIL) && (chip->errstat))
		status = chip->errstat(mtd, chip, FL_WRITING, status, page);

	if (status & NAND_STATUS_FAIL)
		return -EIO;

	return 0;
}

#if 0
/*------------------------------------------------------
 *                    remove part
 *------------------------------------------------------
 */
static int nand_remove(void)
{
	int err = 0 ;

	DBG( "nand_remove() is called, it terminates Mozart NAND-Flash now.\n" ) ;

	return err ;
}
#endif

/*------------------------------------------------------
 *                    probe part
 *------------------------------------------------------
 */
static int mozart_nand_scan(struct nand_chip *chip, struct vpl_nand_chip_private *chip_priv)
{
	//int err = 0 ;    
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *mtd_parts = 0;
	int mtd_parts_nr = 0;
#endif

	//struct nand_chip init 
	chip->options |= NAND_NO_SUBPAGE_WRITE | NAND_USE_FLASH_BBT;//[jam test]NAND_BBT_SCANEMPTY for testing...
	chip->IO_ADDR_W = (void  __iomem *)VPL_NFC_MMR_DATA ;
	chip->IO_ADDR_R = chip->IO_ADDR_W ;

	chip->select_chip = nand_select_chip;
	chip->cmdfunc = nand_command;
	chip->dev_ready = nand_dev_ready;
	chip->cmd_ctrl = nand_hwcontrol ;//This is used to issue command and address cycles to the chip (CLE/ALE)
	chip->write_page = nand_write_page ;
    chip->waitfunc = nand_program_erase_waitfunc ;

	chip->ecc.read_page_raw = nand_read_page_raw ;
	chip->ecc.write_page_raw = nand_write_page_raw ;
	chip->ecc.read_page = nand_ecc_read_page_hwecc ;
	chip->ecc.write_page = nand_ecc_write_page_hwecc ;
	//chip->ecc.layout = &nand_oob_64 ;
	chip->ecc.calculate = nand_calculate ;
	chip->ecc.correct = nand_correct ;
	chip->ecc.hwctl = nand_hwctl ;
#ifdef MOZART_NFC_DISABLE_HW_ECC
	chip->ecc.mode = NAND_ECC_SOFT;
#else
	chip->ecc.mode = NAND_ECC_HW ;
#endif
	chip->ecc.size = 0;

	//struct vpl_nand_chip_private init
	chip_priv->nf_chip_timing = 0xffffffff ;//default timing
	chip_priv->get_desc_page_number = nand_get_desc_page_number ;    

	//Since DMA cannot work on some chips(SAMSUNG), we use CPU mode instead.
    //chip_priv->dma_mode = 1 ;
    chip_priv->dma_mode = 0 ;

    return 0 ;
}

int board_nand_init(struct nand_chip *chip)
{
	struct vpl_nand_chip_private *chip_priv ;
	int err = 0 ;

//    printf("[jam] test for rdi burn tool\n") ;    
//    v_outl(0x4f800044, v_inl(0x4f800044) | (1 << 12)) ;
//    v_outl(0x4f800024, v_inl(0x4f800024) | (1 << 28)) ;

	chip_priv = (struct vpl_nand_chip_private *)malloc(sizeof(struct vpl_nand_chip_private)) ;
	if (chip_priv == NULL) {
		printf( "[ERR]No memory for chip_priv of Mozart_nand\n" );
		err = -ENOMEM;
		goto probe_exit_error;
	}

	//main structure assignment
	chip->priv = chip_priv ;

	//we init fields of struct nand_chip
    if( mozart_nand_scan(chip, chip_priv) != 0 ) {
		printf( "[ERR]init nandchip failed!!\n" );
		err = -1;
		goto probe_exit_error;
	}

	return 0 ;

probe_exit_error :
	if (err == 0)
		err = -EINVAL;
	return err;
}

