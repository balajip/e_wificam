#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

#define RS485_PORT PLATFORM_RS485_PORTNUM
static ulong UART_BASE[4] = { UART_0, UART_1 , UART_2, UART_3 } ;

void RS485_putc(char ch) {
	volatile int delay = 0 ;
	
	//set RTS 0 to send data
	v_outl( UART_BASE[RS485_PORT] + 0x14, 0x0 ) ;
	delay = 0 ;
	while( delay < 10000 ) {
		delay++ ;
	}
	//send data
	v_outl( UART_BASE[RS485_PORT] + 0x04, ch );
	delay = 0 ;
	while( delay < 10000 ) {
		delay++ ;
	}
}

/*
 * Put string on the screen.
 */
void RS485_puts( char *str ) {
	int len = strlen( str ) ;
	int i ;

	for( i = 0 ; i < len ; i++ ) {
		RS485_putc( str[i] ) ;
		
		if( str[i] == '\n') {
			RS485_putc('\r');
		}
	}
}

char RS485_getc(void) {
	volatile int delay = 0 ;
	
	//set RTS 1 to receive data
	v_outl( UART_BASE[RS485_PORT] + SERIAL_MCR, 0x2 ) ;
	delay = 0 ;
	while( delay < 1000 ) {
		delay++ ;
	}
	
	//get char
	return fLib_GetSerialChar( UART_BASE[RS485_PORT]) ;
}

/*
 * If there is any data in receive buffer, then return 1.
 */
int RS485_tstc(void) {
	volatile int delay = 0 ;	
	//set RTS 1 to receive data
	v_outl( UART_BASE[RS485_PORT] + SERIAL_MCR, 0x2 ) ;
	delay = 0 ;
	while( delay < 1000 ) {
		delay++ ;
	}
	return ( v_inl(UART_BASE[RS485_PORT] + SERIAL_LSR) & 0x1 ) == 0x1 ; 
}

int RS485_test_func(void) 
{
	volatile char transmit ;
	volatile char receive ;
	int timeout = 10 ;
	int i = 0 ;
	int result = 1 ;// 1 : equal, -1 : not equal, -2 : timeout

	printf("RS485_test_func : 0x%08lx\n", v_inl(VPL_SYSC_MMR_BASE+0x44)) ;
	//init sysc
	//v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC0_PARAM + RS485_PORT*4, 0x3 ) ;	
	//set baud rate - 38400
	v_outl( UART_BASE[RS485_PORT] + SERIAL_LCR, 0x80 ) ;
	v_outl( UART_BASE[RS485_PORT] + SERIAL_DLM, (EVM_BAUD_38400 & 0xff00) >> 8);
	v_outl( UART_BASE[RS485_PORT] + SERIAL_DLL, EVM_BAUD_38400 & 0xff);
	v_outl( UART_BASE[RS485_PORT] + SERIAL_LCR, 0x03 ) ;//word length = 8-bits
	
	//waiting for user to switch equipment
	printf( "RS485_test_func!!! (baud rate 38400)\n" ) ;
	printf( "# 1. Please connect RS232 line to RS485/232 Transfer Device.\n" ) ;
	printf( "# 2. After connecting, please enter anykey ASAP!\n" ) ;
	timeout = 50;
	while( 1 ) {
		if ( RS485_tstc() ) {
			break ;
		}
		
		if( timeout < 0 ) {
			result = -2 ;
			goto CHECK_TEST_RESULT ;
		}		
		timeout-- ;
		udelay(100000);
	}
	
	//RS485 half-duplex test
	RS485_getc() ;//consume dummy data
	RS485_puts( "\n\n# 3. Connecting successfully !!!!\n" ) ;
	RS485_puts( "# 4. Please enter 5 characters...\n" ) ;
	RS485_puts( "#    You should check \"the char you type\" eqauls to \"the char on screen\"\n" ) ;
	for( i = 0 ; i < 5 ; i++ ) {
		RS485_puts( "#    *Your enter key is => " ) ;
		receive = RS485_getc() ;
		RS485_putc( receive ) ;
		RS485_putc( '\n' ) ;
		RS485_putc( '\r' ) ;
	}

	RS485_puts( "# Test finish. \n" ) ;
	RS485_puts( "# Please reconnect the equipment and enter any key if ok. \n\n" ) ;
	while (!tstc()) ;
	
CHECK_TEST_RESULT:
	if ( result == 1 ) {
		printf("[ RS485 ] Test End normally. \n" ) ;
	}
	else if ( result == -1 ) {
		printf("[ RS485 ] ...................................... Fail\n") ;
		printf("          -- Transmit = %02x\n", transmit ) ;
		printf("          -- Receive  = %02x\n", receive ) ; 
	}
	else {
		printf("[ RS485 ] ...................................... Fail\n") ;
		printf("          -- Timeout!!!\n" ) ;
	}

	return result ;
}

