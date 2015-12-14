#ifndef __ASM_ARCH_UNCOMPRESS_H__
#define __ASM_ARCH_UNCOMPRESS_H__

#include <mach/platform.h>
//#include <asm/arch/haydn/haydn.h>
//#include <asm/arch/haydn/chipset.h>

/*
static delayputs()
{
	volatile unsigned int i=100;
	while(i--)
	   ;
}
*/
/*
 * If we need to do some setup prior to decompression (like initializing the
 * UART if we want to use puts() above) then we define it here. Punt.
 */
//#ifdef  CONFIG_UART0
	#define PUTS_UART_BASE HAYDN_UARTC1_MMR_BASE
//#elif defined(CONFIG_UART1)
//	#define PUTS_UART_BASE HAYDN_UARTC1_MMR_BASE
//#else
//	#error "NO UART"
//#endif

//static void puts(const char *s)
static void putstr(const char *s)
{
	while (*s) 
	{
	    volatile unsigned int status=0;
	    do
	    {
	        status = *(unsigned char *)(PUTS_UART_BASE+SERIAL_LSR);
	    }
		while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );

		*(unsigned char *)(PUTS_UART_BASE+SERIAL_THR) = *s;

		if (*s == '\n') 
		{

    	    do
    	    {
    	        status = *(unsigned char *)(PUTS_UART_BASE+SERIAL_LSR);
    	    }
    		while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );

			*(unsigned char *)(PUTS_UART_BASE+SERIAL_THR) = '\r';
		}
		s++;
	}
}

#define arch_decomp_setup()

#define arch_decomp_wdog()

#endif  //__ASM_ARCH_UNCOMPRESS_H__
