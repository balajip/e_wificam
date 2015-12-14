#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
#define v_inl(addr)		(*((volatile unsigned long *)(addr)))

#define NANDFLASH_LOWADDR 0x40000
#define NANDFLASH_HIGHADDR 0x0
#define DRAM_WRITEADDR 0xa00000
#define DRAM_READADDR 0xb00000

//#define NAND_TEST_DEBUG
#ifdef NAND_TEST_DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x)
#endif

void nand_fsm_ready(void)
{
    DBG("  wait fsm ready...\n") ;
    while((v_inl(VPL_NFC_MMR_STATE) & 0x60) != 0x0) ;
}

void nand_rnb_ready(void)
{
    DBG("  wait rnb...\n") ;
    while((v_inl(VPL_NFC_MMR_STATE) & 0x2) != 0x2) ;
}

void nand_test_init(void)
{
    //settings
    v_outl(VPL_NFC_MMR_CONF, 0xbf7df7de) ;
    v_outl(VPL_NFC_MMR_CTRL, 0x00000a00) ;
}

//write one page from dram to nf
void nand_pageprog_bufmode(unsigned long dram_addr, unsigned long nf_addr)
{        
    int i, j ;

    DBG("=== NF PAGE-PROG ===\n") ;

    //============= PART1 =============
    DBG(" 1. DRAM->SRAM\n") ;

    //change sram to cpu mode
    DBG("  change sram to cpu mode...\n") ;
    v_outl(VPL_BRC_MMR_BASE_REMAPPED+0x4, 0x0) ;    
    
    //copy data from sram to dram
    for(i = 0; i < 512 ; i++ ) {            
        j = i * 4 ;
        v_outl(VPL_BRC_RAM_BASE_REMAPPED + j, v_inl(dram_addr + j)) ;
    }    
    
    //============= PART1 =============        
    DBG(" 2. SRAM->NF\n") ;
        
    //change sram to nfc mode
    DBG("  change sram to nfc mode...\n") ;
    v_outl(VPL_BRC_MMR_BASE_REMAPPED+0x4, 0x1) ;

    nand_fsm_ready() ;

    //clean status reg
    DBG("  clean status reg...\n") ;
    v_outl(VPL_NFC_MMR_STATE, 0x0) ;

    //write addr
    DBG("  write addr...\n") ;
    v_outl(VPL_NFC_MMR_ADDR1L, nf_addr) ;
    v_outl(VPL_NFC_MMR_ADDR1H, NANDFLASH_HIGHADDR) ;

    //write page read command
    DBG("  write page-prog cmd...\n") ;
    v_outl(VPL_NFC_MMR_COMM, 0x80) ;

    //wait dev & ctrl ready
    nand_fsm_ready() ;
    nand_rnb_ready() ;

    //clean status reg
    DBG("  clean stauts reg...\n") ;
    v_outl(VPL_NFC_MMR_STATE, 0x0) ;

    //change sram to cpu mode
    DBG("  change sram to cpu mode...\n") ;
    v_outl(VPL_BRC_MMR_BASE_REMAPPED+0x4, 0x0) ;    
}

//read one page from nf to dram
void nand_pageread_bufmode(unsigned long dram_addr, unsigned long nf_addr)
{        
    int i, j ;

    DBG("=== NF PAGE-READ ===\n") ;
    
    //============= PART1 =============        
    DBG(" 1. NF->SRAM\n") ;
        
    //change sram to nfc mode
    DBG("  change sram to nfc mode...\n") ;
    v_outl(VPL_BRC_MMR_BASE_REMAPPED+0x4, 0x1) ;

    nand_fsm_ready() ;

    //clean status reg
    DBG("  clean status reg...\n") ;
    v_outl(VPL_NFC_MMR_STATE, 0x0) ;

    //write addr
    DBG("  write addr...\n") ;
    v_outl(VPL_NFC_MMR_ADDR0L, nf_addr) ;
    v_outl(VPL_NFC_MMR_ADDR0H, NANDFLASH_HIGHADDR) ;

    //write page read command
    DBG("  write page-read cmd...\n") ;
    v_outl(VPL_NFC_MMR_COMM, 0x30) ;

    //wait dev & ctrl ready
    nand_rnb_ready() ;
    nand_fsm_ready() ;

    //clean status reg
    DBG("  clean stauts reg...\n") ;
    v_outl(VPL_NFC_MMR_STATE, 0x0) ;

    //change sram to cpu mode
    DBG("  change sram to cpu mode...\n") ;
    v_outl(VPL_BRC_MMR_BASE_REMAPPED+0x4, 0x0) ;

    //============= PART2 =============
    DBG(" 2. SRAM->DRAM\n") ;
    //copy data from sram to dram
    for(i = 0; i < 512 ; i++ ) {            
        j = i * 4 ;
        v_outl(dram_addr + j, v_inl(VPL_BRC_RAM_BASE_REMAPPED + j)) ;
    }    
}

void nand_test_erase(unsigned long block_addr)
{
    DBG("=== NF Erase ===\n") ;

    nand_fsm_ready() ;

    //clean status reg
    DBG("  clean status reg...\n") ;
    v_outl(VPL_NFC_MMR_STATE, 0x0) ;

    //write addr
    DBG("  write addr...\n") ;
    v_outl(VPL_NFC_MMR_ADDR1L, block_addr) ;
    v_outl(VPL_NFC_MMR_ADDR1H, NANDFLASH_HIGHADDR) ;

    //write page read command
    DBG("  write blk-erase cmd...\n") ;
    v_outl(VPL_NFC_MMR_COMM, 0x60) ;

    //wait dev & ctrl ready
    nand_fsm_ready() ;    
    nand_rnb_ready() ;

    //clean status reg
    DBG("  clean stauts reg...\n") ;
    v_outl(VPL_NFC_MMR_STATE, 0x0) ;
}

int nand_compare_data(unsigned long src, unsigned long dest, unsigned long size)
{
    unsigned long i, j ;

    printf("**-- Compare...") ;
    for( i = 0 ; i < (size / 4) ; i++ ) {
        j = i * 4 ;
        if(v_inl(src + j) != v_inl(dest + j)) {
            printf(" DIFFERNENT!!!!! --**\n\n") ;
            return -1 ;
        }
    }

    printf(" SAME. --**\n\n") ;

	return 0 ;
}
 

int nand_test_buf(int total_rounds)
{
    int round, i ;
    int test_size = 128*1024 ;
    unsigned long nf_page_num = NAND_QUICK_TEST_BLOCK_INDEX * 64 ;
    unsigned long dram_addr ;
	int result = 0 ;

    //settings
    nand_test_init() ;

    round = 0 ;
    while(round < total_rounds) {
        //printf("round#%7d nand test(BUF MODE)....\n", round) ;    

        printf(">> Erase block...\n") ;
        nand_test_erase(nf_page_num << 12) ;

        printf(">> Write test pattern...\n") ;
        dram_addr = DRAM_WRITEADDR ;
        for( i = 0 ; i < (test_size / 2048) ; i++ ) {
            nand_pageprog_bufmode(dram_addr, (nf_page_num + i) << 12) ;
            dram_addr += 2048 ;
        }

        printf(">> Read test pattern...\n") ;
        dram_addr = DRAM_READADDR ;        
        for( i = 0 ; i < (test_size / 2048) ; i++ ) {
            nand_pageread_bufmode(dram_addr, (nf_page_num + i) << 12) ;
            dram_addr += 2048 ;
        }

        result = nand_compare_data(DRAM_WRITEADDR, DRAM_READADDR, test_size) ;
		if(result == -1)
			break ;

        round++ ;        
    }

	return result ;
}


int nand_test_reg(void)
{
#if 0
    int round, i, j ;

    //settings
    v_outl(VPL_NFC_MMR_CONF, 0xbf7df7de) ;
    v_outl(VPL_NFC_MMR_CTRL, 0x00000a00) ;

    round = 0 ;
    while(round < 100000) {
        printf("round#%d page read....\n", round) ;
        
        //============= PART1 =============        
        printf(" 1. NF Command\n") ;
        
        nand_fsm_ready() ;

        //clean status reg
        printf("  clean status reg...\n") ;
        v_outl(VPL_NFC_MMR_STATE, 0x0) ;

        //write addr
        printf("  write addr...\n") ;
        v_outl(VPL_NFC_MMR_ADDR0L, NANDFLASH_LOWADDR) ;
        v_outl(VPL_NFC_MMR_ADDR0H, NANDFLASH_HIGHADDR) ;

        //write page read command
        printf("  write page-read#1 cmd...\n") ;
        v_outl(VPL_NFC_MMR_COMM, 0x0130) ;

        //wait dev & ctrl ready
        nand_rnb_ready() ;
        nand_fsm_ready() ;

        //clean status reg
        //printf("  clean stauts reg...\n") ;
        //v_outl(VPL_NFC_MMR_STATE, 0x0) ;

        //============= PART2 =============
        printf(" 2. Read regs\n") ;
        //copy data from sram to dram
        for(i = 0; i < 512 ; i++ ) {            
            j = i * 4 ;
            v_outl(DRAM_ADDR + j, v_inl(VPL_NFC_MMR_DATA)) ;
        }

        round++ ;        
    }
#endif
	return 1 ;
}


int nand_test_func(void)
{
	int result = 0 ;

    //printf( "nand_test_func!!!\n" ) ;

    //printf("OPEN NAND MODULE!\n") ;
    v_outl(VPL_SYSC_MMR_BASE + SYSC_MMR_CLKEN_CTRL0,
    			v_inl(VPL_SYSC_MMR_BASE + SYSC_MMR_CLKEN_CTRL0) | (1 << 28));
    v_outl(VPL_SYSC_MMR_BASE + EVM_SYSC_PAD_EN_CTRL,
				v_inl(VPL_SYSC_MMR_BASE + EVM_SYSC_PAD_EN_CTRL) | (1 << 12));
    v_outl(VPL_SYSC_MMR_BASE + EVM_SYSC_PULL_CTRL,
				v_inl(VPL_SYSC_MMR_BASE + EVM_SYSC_PULL_CTRL) | (0x7 << 7));
    v_outl(VPL_SYSC_MMR_BASE + SYSC_MMR_DRV_STRENGTH_CTRL_2,
				v_inl(VPL_SYSC_MMR_BASE + SYSC_MMR_DRV_STRENGTH_CTRL_2) & ~(0x3 << 8));

    /*
	{
    char str[256] ;
    int choice ;
    char *endp ;
    int total_rounds = 0 ;

    readline_into_buffer("1.BUF MODE\n2.REG MODE\n=>", str) ;
    choice = simple_strtoul(str, &endp, 10);
    if(choice == 1) {
        readline_into_buffer("  Total Rounds=>", str) ;
        total_rounds = simple_strtoul(str, &endp, 10);        
        nand_test_buf(total_rounds) ;
    }
    else {
        nand_test_reg() ;
    }
    }
    */
    result = nand_test_buf(1) ;
    
    return result ;
}

