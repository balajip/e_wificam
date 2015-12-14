#include "evm_sd_util.h"

//-----------------------------------
// 2011.08.18
// Obsolete code.
// We use mmc subsystem now.
//-----------------------------------

#if 0

#define v_outl(addr,value)	(*((volatile unsigned long *)(addr)) = value)
#define v_outw(addr,value)	(*((volatile unsigned short *)(addr)) = value)
#define v_outb(addr,value)	(*((volatile unsigned char *)(addr)) = value)
#define v_inl(addr)		(*((volatile unsigned long *)(addr)))
#define v_inw(addr)		(*((volatile unsigned short *)(addr)))
#define v_inb(addr)		(*((volatile unsigned char *)(addr)))

#define SDHC_CHECK_MASK 0x40000000
int isSDHC = 0 ; //1: SDHC, 0 : SDSC or others

//-------------------------------------------------------
//		Initilization
//-------------------------------------------------------
int SD_Controller_Init()
{
	ulong reg ;
	int i ;    
    unsigned long status ;
	volatile int timeout = 0 ;

    status = v_inl(SD_MMR_STATUS) & 0x6FF ;    
    if(status != 0x6) { //reset card
        if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_STOP_ABORT_CMD) != 0)
			goto sd_ctrl_init_fail ;

        v_outl(SD_MMR_CTRL, SD_CTRL_FIFO_RESET) ;        

		timeout = SD_TIMEOUT_CHECK_VAL;
        while((v_inl(SD_MMR_CTRL) & SD_CTRL_FIFO_RESET) != 0) {
			timeout--;
			if(timeout < 0)
				goto sd_ctrl_init_fail ;
        }
        
        if(SD_Check_CMDDone() != 0)
			goto sd_ctrl_init_fail ;
    }

	//Reset Controller
	v_outl( SD_MMR_CTRL, SD_CTRL_CONTROLLER_RESET ) ;   
	timeout = SD_TIMEOUT_CHECK_VAL;
    while((v_inl(SD_MMR_CTRL) & SD_CTRL_CONTROLLER_RESET) != 0) {
		timeout--;
		if(timeout < 0)
			goto sd_ctrl_init_fail ;
    }

	//Enable Power to Cards
	reg = 0 ;
	for ( i = 0 ; i < SD_NUMBER ; i++ ) {
		reg = reg << 1 ;
		reg = reg | 1 ;
	}
	v_outl( SD_MMR_PWREN, reg ) ;

    //Disable clock
    v_outl( SD_MMR_CLKENA, 0x0 ) ;   

    //Goto Idle state
    if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_UPDATE_CLKREGS_ONLY | SD_CMD_SEND_INITIALIZATION) != 0)
		goto sd_ctrl_init_fail ;

    //DIV=20, 400KHz
    v_outl( SD_MMR_CLKDIV, 0x20 ) ;  

    //Clock source
    v_outl( SD_MMR_CLKSRC, 0x0  ) ;  

    //Goto Idle state
    if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_UPDATE_CLKREGS_ONLY | SD_CMD_SEND_INITIALIZATION) != 0)
		goto sd_ctrl_init_fail ;

    //enable clock
  	v_outl( SD_MMR_CLKENA, 0x01 ) ;  

    //Goto Idle state
    if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_UPDATE_CLKREGS_ONLY | SD_CMD_SEND_INITIALIZATION) != 0)
		goto sd_ctrl_init_fail ;

    //set timeout
	v_outl( SD_MMR_TMOUT, 0xffffffff ) ;

  	//Set CardType
	v_outl( SD_MMR_CTYPE, 0 ) ; //1-bit

    //clear interrupt status
    v_outl( SD_MMR_RINSTS , 0xffffffff ) ; 

    //Goto Idle state
    if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_UPDATE_CLKREGS_ONLY | SD_CMD_SEND_INITIALIZATION) != 0)
		goto sd_ctrl_init_fail ;
	
    if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_SEND_INITIALIZATION) != 0)
		goto sd_ctrl_init_fail ;

	if(SD_Check_CMDDone() != 0)
		goto sd_ctrl_init_fail ;

	return 0 ;

sd_ctrl_init_fail:
	return -1 ;	
}

//-------------------------------------------------------
//		Identification
//-------------------------------------------------------
int SD_Card_IdentificationMode(void)
{
    unsigned long response ;
    int sd_v1 = 0 ;//is sd 1.0?
    unsigned long cmdargs ;
    int timeout = SD_TIMEOUT_CHECK_VAL ;
    unsigned long RCA ;
    
	if(SD_Set_CMD(0x1AA, SD_CMD8) != 0)
		goto sd_idmode_fail ;
    if(SD_Check_CMDDone() != 0)
		goto sd_idmode_fail ;

    response = v_inl(SD_MMR_RESP0) & 0xAA ;

    if(response == 0xAA) {//SD 2.0
        sd_v1 = 0 ;
        printf("  Card Version : SD2.0\n") ;
    }
    else {//SD 1.0
        if(SD_Set_CMD(0x0, SD_CMD_START_CMD | SD_CMD_SEND_INITIALIZATION) != 0)
			goto sd_idmode_fail ;
		
        if(SD_Check_CMDDone() != 0)
			goto sd_idmode_fail ;
		
        v_outl(SD_MMR_RINSTS, SD_INTMASK_RTO) ;
        sd_v1 = 1 ;
        printf("  Card Version : SD1.0\n") ;
    }

    //POWER ON CHECK
    while(timeout >= 0) {
        cmdargs = (sd_v1) ? 0x00FF8000 : 0x40FF8000 ;
        if(SD_Set_ACMD(cmdargs) != 0)
			goto sd_idmode_fail ;
		
        if(SD_Check_CMDDone() != 0)
			goto sd_idmode_fail ;

        response = v_inl(SD_MMR_RESP0) & 0x80000000 ;
        if(response == 0x80000000) {
            break ;
        }

        timeout-- ;
    }

    if(timeout < 0) {
        return -1 ;
    }

    if((v_inl(SD_MMR_RESP0) & SDHC_CHECK_MASK) == SDHC_CHECK_MASK) {
        printf("  Card Type    : SDHC\n") ;
        isSDHC = 1 ;
    }
    else {
        printf("  Card Type    : SDSC\n") ;
        isSDHC = 0 ;
    }

    //Goto Ident State
    if(SD_Set_CMD(0x0 , SD_CMD2) != 0)
		goto sd_idmode_fail ;
    if(SD_Check_CMDDone() != 0)
		goto sd_idmode_fail ;

    //Get RCA
    if(SD_Set_CMD(0x0 , SD_CMD3) != 0)
		goto sd_idmode_fail ;
	if(SD_Check_CMDDone() != 0)
		goto sd_idmode_fail ;
    RCA = v_inl(SD_MMR_RESP0) ;

    //Set CLKDIV
    v_outl(SD_MMR_CLKDIV, 0x0) ;

    //Goto idle state
    if(SD_Set_CMD(RCA, SD_CMD_START_CMD | SD_CMD_UPDATE_CLKREGS_ONLY | SD_CMD_SEND_INITIALIZATION) != 0)
		goto sd_idmode_fail ;

    //Wait for clk stable
    v_outl(0x4f800018, 0x80000000) ;
	timeout = SD_TIMEOUT_CHECK_VAL ;
    while(v_inl(0x4f80001c) < 0x200) {
		timeout-- ;
		if(timeout < 0)
			goto sd_idmode_fail;
    }

    //Goto TransferState
    if(SD_Set_CMD(RCA, SD_CMD7) != 0)
		goto sd_idmode_fail;
    if(SD_Check_CMDDone() != 0)
		goto sd_idmode_fail;

    //Set BLKSIZE
    v_outl(SD_MMR_BLKSIZ, SD_BLOCK_SIZE) ;
    if(SD_Set_CMD(SD_BLOCK_SIZE, SD_CMD16) != 0)
		goto sd_idmode_fail;
    if(SD_Check_CMDDone() != 0)
		goto sd_idmode_fail;

    //Set FIFO
    v_outl(SD_MMR_FIFOTH, 0x00070008) ;

    return 0 ;
	
sd_idmode_fail:
	return -1; 
}

//------------------------------
//          Misc
//------------------------------
int SD_Check_CMDDone(void) 
{
	ulong reg ;
	volatile int timeout = SD_TIMEOUT_CHECK_VAL ;

	//wait Command done
	do {
		reg = v_inl( SD_MMR_RINSTS ) ;

		timeout-- ;
		if(timeout < 0)
			return -1 ;
		
	} while ( ( reg & SD_INTMASK_CMDDONE ) != SD_INTMASK_CMDDONE ) ;

	//clear interrupt status
	v_outl( SD_MMR_RINSTS, SD_INTMASK_CMDDONE ) ;

	return 0 ;
}

int SD_Set_CMD( ulong cmd_arg, ulong cmd )
{
	volatile int timeout = SD_TIMEOUT_CHECK_VAL ;

	v_outl( SD_MMR_CMDARG, cmd_arg ) ;
	v_outl( SD_MMR_CMD, cmd ) ;

    while((v_inl(SD_MMR_CMD) & 0x80000000) == 0x80000000) {
		timeout--;
		if(timeout < 0)
			return -1 ;
    }

	return 0 ;
}

int SD_Set_ACMD( ulong cmd_arg )
{
	//printf( "@SD_GOTO_ACMD\n" ) ;

	v_outl( SD_MMR_CMDARG, 0x0 ) ;
	v_outl( SD_MMR_CMD, SD_CMD55 ) ;
    if(SD_Check_CMDDone() != 0)
		return -1 ;

    if(SD_Set_CMD(cmd_arg, SD_ACMD41) != 0)
		return -1 ;
    v_outl(SD_MMR_RINSTS, 0x80000000) ;

	return 0 ;
}

void SD_Check_ERR( ulong interrupt ) 
{
	int i = 0 ;

	printf( "INT...." ) ;
	if ( ( interrupt & SD_INTMASK_RE ) == SD_INTMASK_RE ) {
		printf( "SD_INTMASK_RE\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_RCRC ) == SD_INTMASK_RCRC ) {
		printf( "SD_INTMASK_RCRC\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_DCRC ) == SD_INTMASK_DCRC ) {
		printf( "SD_INTMASK_DCRC\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_RTO ) == SD_INTMASK_RTO ) {
		printf( "SD_INTMASK_RTO\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_DRTO ) == SD_INTMASK_DRTO ) {
		printf( "SD_INTMASK_DRTO\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_HTO ) == SD_INTMASK_HTO ) {
		printf( "SD_INTMASK_HTO\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_FRUN ) == SD_INTMASK_FRUN ) {
		printf( "SD_INTMASK_FRUN\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_HLE ) == SD_INTMASK_HLE ) {
		printf( "SD_INTMASK_HLE\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_SBE ) == SD_INTMASK_SBE ) {
		printf( "SD_INTMASK_SBE\n" ) ;
		i = 1 ;
	}
	if ( ( interrupt & SD_INTMASK_EBE ) == SD_INTMASK_EBE ) {
		printf( "SD_INTMASK_EBE\n" ) ;
		i = 1 ;
	}

	if ( i == 0 )
		printf( "NO Error!\n" ) ;

	return ;
}

#endif

