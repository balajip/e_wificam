#ifndef __ASM_ARCH_SERIAL_H__
#define __ASM_ARCH_SERIAL_H__


/*
 * include/asm-i386/serial.h
 */

//#include <linux/config.h>
#include <mach/rossini.h>

/*
 * This assumes you have a 1.8432 MHz clock for your UART.
 *
 * It'd be nice if someone built a serial card with a 24.576 MHz
 * clock, since the 16550A is capable of handling a top speed of 1.5
 * megabits/second; but this requires the faster clock.
 */
//#define BASE_BAUD ( BACH_UART_CLOCK / 16 )

/* Standard COM flags */
//#define STD_COM_FLAGS (ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST)
#define STD_COM_FLAGS (ASYNC_SKIP_TEST)

#define EXTRA_SERIAL_PORT_DEFNS


#define RS_TABLE_SIZE

#define MCA_COM_FLAGS	(STD_COM_FLAGS|ASYNC_BOOT_ONLYMCA)

/*
 * The following define the access methods for the HUB6 card. All
 * access is through two ports for all 24 possible chips. The card is
 * selected through the high 2 bits, the port on that card with the
 * "middle" 3 bits, and the register on that port with the bottom
 * 3 bits.
 *
 * While the access port and interrupt is configurable, the default
 * port locations are 0x302 for the port control register, and 0x303
 * for the data read/write register. Normally, the interrupt is at irq3
 * but can be anything from 3 to 7 inclusive. Note that using 3 will
 * require disabling com2.
 */

#define C_P(card,port) (((card)<<6|(port)<<3) + 1)

#include <mach/hardware.h>

/* macro to get at IO space when running virtually */
//#define IO_ADDRESS(x) (((x >> 4)&0xffff0000) + (x & 0xffff) + IO_BASE)
#define VPL_UART_SERIAL_PORT_DFNS		\
	/* UART CLK   PORT  IRQ  FLAGS    */ 		\
	{ 0, CONFIG_UART_CLK/16, 0x0, 25, UPF_BOOT_AUTOCONF | UPF_LOW_LATENCY, 0, SERIAL_IO_MEM, __io(IO_ADDRESS(ROSSINI_UARTC0_MMR_BASE)), 2 },   /* ttyS0 */ \
	{ 0, CONFIG_UART_CLK/16, 0x0, 26, UPF_BOOT_AUTOCONF | UPF_LOW_LATENCY, 0, SERIAL_IO_MEM, __io(IO_ADDRESS(ROSSINI_UARTC1_MMR_BASE)), 2 },   /* ttyS0 */ \
	{ 0, CONFIG_UART_CLK/16, 0x0, 27, UPF_BOOT_AUTOCONF | UPF_LOW_LATENCY, 0, SERIAL_IO_MEM, __io(IO_ADDRESS(ROSSINI_UARTC2_MMR_BASE)), 2 },   /* ttyS0 */ \
	{ 0, CONFIG_UART_CLK/16, 0x0, 28, UPF_BOOT_AUTOCONF | UPF_LOW_LATENCY, 0, SERIAL_IO_MEM, __io(IO_ADDRESS(ROSSINI_UARTC3_MMR_BASE)), 2 },   /* ttyS0 */
/*
#ifdef CONFIG_UART0
#define STD_SERIAL_PORT_DEFNS			\
{0,BASE_BAUD,IO_ADDRESS(BACH_UARTC0_MMR_BASE),UARTC0_IRQ_NUM,STD_COM_FLAGS},\
{0,BASE_BAUD,IO_ADDRESS(BACH_UARTC1_MMR_BASE),UARTC1_IRQ_NUM,STD_COM_FLAGS},
#elif CONFIG_UART1
#define STD_SERIAL_PORT_DEFNS			\
{0,BASE_BAUD,IO_ADDRESS(BACH_UARTC1_MMR_BASE),UARTC1_IRQ_NUM,STD_COM_FLAGS},\
{0,BASE_BAUD,IO_ADDRESS(BACH_UARTC0_MMR_BASE),UARTC0_IRQ_NUM,STD_COM_FLAGS},
#else
#error "NO UART"
#endif
*/
#endif  //__ASM_ARCH_SERIAL_H__
