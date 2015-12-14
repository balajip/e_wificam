#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

//#define EVM_UART_BASE 0x4a000000 //UART#1
//#define SYSC_BASE 0x4F800000 
#define UART_TESTPATTERN_SIZE 10
ulong UART_BASE[4] = { UART_0, UART_1 , UART_2, UART_3 } ;
uchar UART_TestPattern[UART_TESTPATTERN_SIZE] = { 0x5a, 0xff, 0x00, 0x69, 0x96, 0x34, 0x87, 0xdc, 0xa5, 0x43 } ;

typedef struct {
	int index ;
	char transmit ;
	char receive ;
} UART_ERR_DESC ;
UART_ERR_DESC UART_TestRecord[4] ;

void UARTC_DisableFIFO(UINT32 port)
{
        v_outl(port + SERIAL_FCR, 0);
}

void UARTC_SetLoopback(UINT32 port, UINT32 onoff)
{
        UINT32 temp;

        temp = v_inl(port + SERIAL_MCR);
        if(onoff==1)
                temp|=0x10;
        else
                temp&=~(0x10);

        v_outl(port + SERIAL_MCR, temp);
}

unsigned long UARTC_LoopbackTest(ulong uart_base)
{
	volatile char transmit ;
	volatile char receive ;
	int i ;
	int equal = 0 ;// 0 : not equal, 1 : equal
	int uart_index = 0 ;

	for( i = 0 ; i < 1000 ; i++ ) 
	{
		transmit = UART_TestPattern[ i % UART_TESTPATTERN_SIZE] ;
		fLib_PutSerialChar( uart_base, transmit );
		receive = fLib_GetSerialChar( uart_base) ;
		
		if ( receive == transmit ) {
			equal = 1 ;
		}
		else {
			//record the error
			for ( uart_index = 0 ; uart_index < 3 ; uart_index++ ) {
				if ( uart_base == UART_BASE[uart_index] ) {
					UART_TestRecord[uart_index].index = i ;
					UART_TestRecord[uart_index].transmit = transmit ;
					UART_TestRecord[uart_index].receive = receive ;
					break ;
				}
			}

			equal = 0 ;
			break ;
		}
	} 

	UARTC_SetLoopback(uart_base, 0) ;//close loopback mode
	
	if ( equal )
		return 1 ;
	else
		return 0 ;
}

int UARTC_do_quick_test(ulong uart_base) 
{
	unsigned long equ ;
	
	UARTC_DisableFIFO(uart_base) ;
	UARTC_SetLoopback(uart_base, 1) ;
        
	equ = UARTC_LoopbackTest(uart_base) ;
       
	return equ ;
}

int uart_test_func(void) 
{
	int equ ;
	int result = 0 ;
	int i ;
	ulong uart_base ;
	
	int NOFIFOMODE[4] = {0} ;
	
	//init uart by setting sys
	v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC0_PARAM, 0x03 ) ;
	v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC1_PARAM, 0x03 ) ;
	v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC2_PARAM, 0x03 ) ;
	v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC3_PARAM, 0x03 ) ;

	for ( i = 0 ; i < 4 ; i++ ) {
		uart_base = UART_BASE[i] ;
		v_outl( uart_base + SERIAL_LCR, 0x3 ) ;

		//set baud rate -- 38400
		v_outl( uart_base + SERIAL_LCR, 0x80 ) ;
		udelay(1000) ;
		v_outl( uart_base + SERIAL_DLL, 0x1e ) ;
		v_outl( uart_base + SERIAL_DLM, 0x0 ) ;
		v_outl( uart_base + SERIAL_LCR, 0x3 ) ;

	    equ = UARTC_do_quick_test(uart_base )  ;
		if ( equ == 1 ) 
		{
			NOFIFOMODE[i] = 1 ;
		}
		else
		{
			NOFIFOMODE[i] = -1 ;
			result = -1 ;
		}
	}//end for

	for ( i = 0 ; i < 4 ; i++ ) {
		if ( NOFIFOMODE[i] == 1) {
			printf( "[ UART #%d] ...................................... Pass\n", i ) ;
		}
		else {
			printf("[ UART #%d] ...................................... Fail\n", i ) ;
			printf("            -- Loopback Test failed in FIFO-Disable Mode.\n" ) ;	
			printf("            -- #%d Test\n", UART_TestRecord[i].index ) ;
			printf("            -- Transmit = %02x\n", UART_TestRecord[i].transmit ) ;
			printf("            -- Receive  = %02x\n", UART_TestRecord[i].receive ) ; 
		}
	}

	
	return result ;	
}


