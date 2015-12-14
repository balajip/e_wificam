#include <sd_card.h>

#include <asm/arch/sysctrl.h>
#include "evm_sd_util.h"

//-----------------------------------
// 2011.08.18
// Obsolete code.
// We use mmc subsystem now.
//-----------------------------------

#if 0
#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)

int SD_Card_Detect(int print_msg)
{
    unsigned long reg = v_inl(SD_MMR_CDETECT) ;
    if ( reg == 0 ) {
        if(print_msg) 
            printf("  Card Status  : SD card exists!\n") ;                
    }
    else {
        if(print_msg)
            printf("  Card Status  : SD card does not exist!\n" ) ;
        return 0 ;
    }

    return 1 ;
}

int SD_Init(void) 
{
	int ret ;
		
	ret = SD_Controller_Init() ;
	if(ret != 0)
		return ret ;

	return SD_Card_IdentificationMode() ; 
}

int SD_Read(unsigned long sd_addr, unsigned long cnt, unsigned long memory_addr)
{
    int totalIterations, iterations ;
    int tenPercentIterations[10] ;
    int i ;
	volatile int timeout = SD_TIMEOUT_CHECK_VAL ;
	int err = 0 ;

    totalIterations = cnt / SD_BLOCK_SIZE ;
    if((cnt % SD_BLOCK_SIZE) != 0) {
        totalIterations++ ;
    }
    
    for(i = 0 ; i < 10 ; i++){
        tenPercentIterations[i] = totalIterations / 10 ;
    }

    if((totalIterations % 10) != 0) {
        tenPercentIterations[9] += totalIterations % 10 ; 
    }    
    if ( ( sd_addr % SD_BLOCK_SIZE ) != 0 ) {
		printf("\n---Error! SD address should be block(512) aligned!!---\n" ) ;
		err = -1 ;
		goto sdread_exit ;
	}

    if((cnt % SD_BLOCK_SIZE) != 0) {
        cnt = cnt / SD_BLOCK_SIZE ;
        cnt++ ;
        cnt = cnt * SD_BLOCK_SIZE ;
    }

    if(isSDHC) {
        sd_addr = sd_addr / SD_BLOCK_SIZE ;
    }

    i = 0 ;
    iterations = 0 ;

    //set bytcnt
    v_outl(SD_MMR_BYTCNT, cnt) ;
    
    //Clear Interrupt Status
    v_outl(SD_MMR_RINSTS, 0xFFFFFFFF) ;

    //Set Read
    v_outl( SD_MMR_CMDARG, sd_addr ) ;
	v_outl( SD_MMR_CMD, SD_MULTIPLEBLK_READ ) ;
    if(SD_Check_CMDDone() != 0) {
		err = -1 ;
		goto sdread_exit ;
    }

	timeout = SD_TIMEOUT_CHECK_VAL;
	timeout *= 5 ;
	while ( 1 ) {        
        if((v_inl(SD_MMR_RINSTS) & SD_INTMASK_DTO) != SD_INTMASK_DTO) {
            if((v_inl(SD_MMR_RINSTS) & SD_INTMASK_RXDR) == SD_INTMASK_RXDR)  {
                for(i = 0 ; i < 8 ; i++) {
                    unsigned long sddata = v_inl(SD_MMR_DATA) ;
                    v_outl(memory_addr, sddata) ;                    
                    memory_addr += 4 ;                    
                }
                v_outl(SD_MMR_RINSTS, SD_INTMASK_RXDR) ;
				timeout = SD_TIMEOUT_CHECK_VAL;
				timeout *= 5 ;
            }
        }
        else {
            unsigned long fifo_rest_counter  = v_inl(SD_MMR_STATUS) & 0x3FFE0000 ;
            fifo_rest_counter= fifo_rest_counter >> 17 ;
            while(fifo_rest_counter != 0) {
                unsigned long sddata = v_inl(SD_MMR_DATA) ;
                v_outl(memory_addr, sddata) ;
                memory_addr += 4 ;
                fifo_rest_counter-- ;                
            }
            break ;
        }

		timeout-- ;
		if(timeout < 0) {
			err = -1 ;
			goto sdread_exit ;
		}
	}

sdread_exit:
    v_outl(SD_MMR_RINSTS, SD_INTMASK_DTO | SD_INTMASK_RXDR) ;

	SD_Set_CMD(0x0, SD_CMD0) ;

	return err ;

}

int SD_Write(unsigned long memory_addr, unsigned long sd_addr, unsigned long cnt ) 
{
	return 0 ;
}

int SD_Erase(unsigned long start_sector, unsigned long end_sector ) 
{
	return 0 ;
}
#endif

