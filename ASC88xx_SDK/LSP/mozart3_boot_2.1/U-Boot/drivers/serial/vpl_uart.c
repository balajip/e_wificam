#include <common.h>
#include <watchdog.h>

#include <asm/io.h>
#include <asm/arch/clk.h>

#include <asm/arch/platform.h>

#if (CONFIG_UART_PORTNUM == 0)
    #define USART_ID	0
    #define USART_BASE	USART0_BASE
    #define UART_PORT   UART_0
#elif (CONFIG_UART_PORTNUM == 1)
    #define USART_ID	1
    #define USART_BASE	USART1_BASE
    #define UART_PORT   UART_1    
#elif (CONFIG_UART_PORTNUM == 2)
    #define USART_ID	2
    #define USART_BASE	USART2_BASE
    #define UART_PORT   UART_2    
#elif (CONFIG_UART_PORTNUM == 3)
    #define USART_ID	3
    #define USART_BASE	USART3_BASE
    #define UART_PORT   UART_3    
#endif

DECLARE_GLOBAL_DATA_PTR;


void fLib_SerialInit(unsigned long port, unsigned long baudrate, unsigned long parity,unsigned long num,unsigned long len)
{
	unsigned long lcr;

	lcr = v_inl(port + SERIAL_LCR) & ~SERIAL_LCR_DLAB;
	/* Set DLAB=1 */
	v_outl(port + SERIAL_LCR,SERIAL_LCR_DLAB);
	/* Set baud rate */
	v_outl(port + SERIAL_DLM, ((baudrate & 0xf00) >> 8));
	v_outl(port + SERIAL_DLL, (baudrate & 0xff));

	//clear orignal parity setting
	lcr &= 0xc0;

	switch (parity)
	{
		case PARITY_NONE:	
			//do nothing
			break;
		case PARITY_ODD:
			lcr|=SERIAL_LCR_ODD;
			break;
		case PARITY_EVEN:
			lcr|=SERIAL_LCR_EVEN;
			break;
		case PARITY_MARK:
			lcr|=(SERIAL_LCR_STICKPARITY|SERIAL_LCR_ODD);
			break;
		case PARITY_SPACE:
			lcr|=(SERIAL_LCR_STICKPARITY|SERIAL_LCR_EVEN);
			break;

		default:
			break;
	}

	if(num==2)
		lcr|=SERIAL_LCR_STOP;

	len-=5;

	lcr|=len;	

	v_outl(port+SERIAL_LCR,lcr);    
}

void fLib_SetSerialFifoCtrl(unsigned long port, unsigned long level, unsigned long resettx, unsigned long resetrx)
{
	unsigned char fcr = 0;

	if ((resettx == ENABLE) && (resetrx == ENABLE))
		fcr |= SERIAL_FCR_FE;

	switch(level)
	{
		case 4:
			fcr|=0x40;
			break;
		case 8:
			fcr|=0x80;
			break;
		case 14:
			fcr|=0xc0;
			break;
		default:
			break;
	}

	if(resettx == ENABLE)
		fcr|=SERIAL_FCR_TXFR;

	if(resetrx == ENABLE)
		fcr|=SERIAL_FCR_RXFR; 	
	v_outl(port+SERIAL_FCR,fcr);
	//	v_outl(port+SERIAL_FCR, 0);
}

void fLib_DisableSerialFifo(unsigned long port)
{
	v_outl(port+SERIAL_FCR,0);
}


char fLib_GetSerialChar(unsigned long port)
{   
	char Ch;    
	unsigned long status;
	do
	{
		status=v_inl(port+SERIAL_LSR);
	}
	while (!((status & SERIAL_LSR_DR)==SERIAL_LSR_DR));	// wait until Rx ready

	Ch = v_inl(port + SERIAL_RBR);    
	return (Ch);
}				

void fLib_PutSerialChar(unsigned long port, char Ch)
{
	unsigned long status;

	do
	{
		status=v_inl(port+SERIAL_LSR);
	}while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE));	// wait until Tx ready	   
	v_outl(port + SERIAL_THR,Ch);
}



static unsigned long baudrate = 0 ;
    
void serial_setbrg(void)
{
	unsigned int reg = 0;
    baudrate = CONFIG_BAUDRATE ;
    
	if (baudrate == 9600)
	{
		reg = EVM_BAUD_9600;
	}
	else if (baudrate == 19200)
	{
		reg = EVM_BAUD_19200;
	}
	else if (baudrate == 38400)
	{
		reg = EVM_BAUD_38400;
	}
	else if (baudrate == 57600)
	{
		reg = EVM_BAUD_57600;
	}
	else if (baudrate == 115200)
	{
		reg = EVM_BAUD_115200;
	}
	else
	{
		hang();
	}

    //set sysc to init all UARTs
#ifdef CONFIG_SOC_HAYDN
    v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC0_PARAM, 0x3) ;
    v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC1_PARAM, 0x3) ;
    v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC2_PARAM, 0x3) ;
    v_outl( EVM_SYSC_BASE + EVM_SYSC_UARTC3_PARAM, 0x3) ;
#elif defined(CONFIG_SOC_MOZART)
    unsigned long tmp = v_inl( EVM_SYSC_BASE + EVM_SYSC_PAD_EN_CTRL ) ;
    tmp |= (0xf << 14) ;
    v_outl(EVM_SYSC_BASE + EVM_SYSC_PAD_EN_CTRL, tmp) ;
#elif defined(CONFIG_SOC_BEETHOVEN)
	/* PAD mux should be decided in Loader */
#else 
//#error You must init uart in drivers/serial/vpl_uart.c
#endif

	fLib_SerialInit( UART_PORT, reg, PARITY_NONE, 0, 8 );
    fLib_DisableSerialFifo( UART_PORT ) ;
}

int serial_init(void)
{
    baudrate = gd->baudrate;

#ifdef CONFIG_SOC_BEETHOVEN
	v_outl(EVM_SYSC_BASE + EVM_SYSC_UARTC_CLK_CFG_CTRL, (EVM_UART_CLOCK == 18432000) ? 1 : 0);
#elif defined(CONFIG_ASC88XX_A)
	int val = inl(EVM_SYSC_BASE + EVM_SYSC_JTAG_IF_SEL);
	if (EVM_UART_CLOCK == 18432000)
		v_outl(EVM_SYSC_BASE + EVM_SYSC_JTAG_IF_SEL, val & ~I2SSC_PLL_REF_SEL(1));
	else if (EVM_UART_CLOCK == 24000000)
		v_outl(EVM_SYSC_BASE + EVM_SYSC_JTAG_IF_SEL, val | I2SSC_PLL_REF_SEL(1));
#endif
    
    serial_setbrg() ;

	return 0;
}

void serial_putc(char c)
{
	fLib_PutSerialChar( UART_PORT, c );
	
    /* If \n, also do \r */
    if(c  == '\n') {
       	serial_putc('\r');
    }
}

void serial_puts(const char *Str)
{
  	const char *cp;
   
 	for(cp = Str; *cp != 0 ; cp++)       
   		serial_putc(*cp);	
}

int serial_getc(void)
{
    return fLib_GetSerialChar(UART_PORT) ;
}

int serial_tstc(void)
{
	return ( v_inl( UART_PORT + SERIAL_LSR ) & SERIAL_LSR_DR ) == SERIAL_LSR_DR;
}
