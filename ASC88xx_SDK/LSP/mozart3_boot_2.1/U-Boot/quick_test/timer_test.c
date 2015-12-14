#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

/* --INTC Quick Test----------------------------------------------------------------------------- */
static volatile int TMRC_LOOP = 0 ;
static volatile int intc_timer = 2 ; //default timer for INTC_quick_test/TMRC_quick_test
static volatile int INTC_LOOP  = 1 ;

unsigned long irq_high_part = 0 ;
unsigned long irq_low_part = 0 ;

int INTC_transfer_to_proper_irqpart( int irqnum )
{
    if ((irqnum < 0) || (irqnum > 63)) {
        printf( "[ERR] Wrong IRQ NUM\n" ) ;
        return -1 ;
    }

    if ( irqnum < 32 ) {
        irq_low_part = irq_low_part | (1 << irqnum) ;
    }
    else {
        irqnum -= 32 ;        
        irq_high_part = irq_high_part | (1 << irqnum) ;
    }

    return 0 ;
}

void INTC_reset_timer(void) {		
	v_outl(EVM_INTC_BASE + EVM_INTC_MASK_HI, v_inl(EVM_INTC_BASE + EVM_INTC_MASK_HI) & ~(IRQ_TMR)) ; //mask interrupt
	v_outl(EVM_INTC_BASE + EVM_INTC_CLEAR_HI, IRQ_TMR ) ; //clear high status register
	v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) & ~(1 << (intc_timer * 4))) ; //disable intc_timer
}

void IRQ_Handler(void) {
	ulong reg ;
	INTC_LOOP = 0 ;
	TMRC_LOOP = 1 ;

	INTC_reset_timer() ;

	return ;
}

/* --TMRC Quick Test----------------------------------------------------------------------------- */
void TMRC_print_settings( unsigned long status ) 
{
	printf("=TMRC print settings!\n" );
	printf("@Of   = %s\n", ( (status & 0x020) == 0x020 )? "Yes" : "No" ) ;
	printf("@Match= %s\n", ( (status & 0x040) == 0x040 )? "Yes" : "No" ) ;
	printf("@Dir  = %s\n", ( (status & 0x080) == 0x080 )? "Decrement" : "Increment" ) ;
}

/*
 . Function:  TMRC_set_settings( unsigned long base, int isMatch, int isOf, int isDec )
 . Purpose:
 .      This sets the MMR of TMRC, including set its match value, overflow value, and Timer counting direction
 . Comment:
 .      This is default for Timer2!!!!!! 	
 . Method:
 .      1.  set the counter
 .      2.  enable/disable match interrupt of Timer2
 .      3.  enable/disable overflow interrupt of Timer2
 .      4.  if the counting direction is incremental, we should set the match value larger than counter, and vice versa.
*/
void TMRC_set_settings( int timer, int isMatch, int isOf, int isDec ) 
{
	//printf("=TMRC set settings!\n" );
	v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) & ~(1 << (timer * 4)) ) ;//disable timer 

	v_outl(EVM_TIMER_BASE + EVM_TIMER_COUNTER_BASE + (0xC * timer), 0x30 ) ;    //set the counter
	v_outl(EVM_TIMER_BASE + EVM_TIMER_AUTO_RELOAD_VALUE_BASE + (0xC * timer), 0x30 ) ;    //set the reload value

	if ( isMatch == 1 ) {
		v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) | (0x4 << (timer * 4))) ;//enable match
	} else {
		v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) & ~(0x4 << (timer * 4))) ;//disable match
	}

	if ( isOf == 1 )
		v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) | (0x2 << (timer * 4))) ;//enable overflow 
	else
		v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) & ~(0x2 << (timer * 4))) ;//disable overflow 
	
	if ( isDec == 1 ) {
		v_outl(EVM_TIMER_BASE + EVM_TIMER_MATCH_VALUE_BASE + (0xC * timer), 0x0 ) 			;//set the match value 
		v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) | (0x8 << (timer * 4)))    ;//decrement
	} else {
		v_outl(EVM_TIMER_BASE + EVM_TIMER_MATCH_VALUE_BASE + (0xC * timer), 0x100 ) 			;//set the match value 
		v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) & ~(0x8 << (timer * 4))) ;//increment
	}
}

#define TMRC_COUNTER_VALUE  0xfff
#define TMRC_RELOAD_VALUE   0xfff
#define TMRC_MATCH_VALUE    0x2000
int timer_test_func(void)
{
	//match -- Increment
	int timeout = 1000000 ;
	ulong TMRC_original = v_inl( EVM_TIMER_BASE + EVM_TIMER_CTRL ) ;

    printf( "%% Write a value to timer\n" ) ;
    printf( "%% Enable intc and timer\n" ) ;
    printf( "%% INTC should receive the interrupt in the limited time\n" ) ;

	TMRC_LOOP = 0 ;
    TMRC_set_settings( intc_timer, 1, 1, 0 ) ;//timer#, isMatch, isOf, isDec
	v_outl(EVM_INTC_BASE + EVM_INTC_TRIGGER_MODE_HI, v_inl(EVM_INTC_BASE + EVM_INTC_TRIGGER_MODE_HI) | IRQ_TMR) ;//edge trigger
   	v_outl(EVM_INTC_BASE + EVM_INTC_MASK_HI, v_inl(EVM_INTC_BASE + EVM_INTC_MASK_HI) | IRQ_TMR) ;//irq masked
	v_outl(EVM_TIMER_BASE + EVM_TIMER_CTRL, v_inl(EVM_TIMER_BASE + EVM_TIMER_CTRL) | (1 << (intc_timer * 4))) ;//enable timer2

	while(!TMRC_LOOP) {
		timeout-- ;
		if ( timeout < 0 ) {
			printf("[ INTC ] ........................................ Fail\n" ) ;
			printf("         -- INTC do not receive interrupt request  \n" ) ;
			printf("         -- from TMRC in the limited time.\n" ) ;
			printf("         -- Please check TMRC.\n" ) ;
			printf("[ TMRC ] ........................................ Fail\n" ) ;
			printf("         -- The counter of TMRC can not match \n" ) ;
			printf("         -- the specific value in the limited time.\n" ) ;
			INTC_reset_timer() ; 
            break ;
		}
	}
    
	v_outl( EVM_TIMER_BASE + EVM_TIMER_CTRL, TMRC_original ) ;

    if( timeout < 0 ) 
        return -1 ;
            
	return 1 ;
}


