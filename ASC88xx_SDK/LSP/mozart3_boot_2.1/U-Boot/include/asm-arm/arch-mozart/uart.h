#ifndef __MOZART_UART_H__
#define __MOZART_UART_H__

#include <config.h>

#define EVM_SYSC_UARTC0_PARAM 0x40
#define EVM_SYSC_UARTC1_PARAM 0x44
#define EVM_SYSC_UARTC2_PARAM 0x48
#define EVM_SYSC_UARTC3_PARAM 0x4c

#define UART_0                         VPL_UARTC0_MMR_BASE
#define UART_1                         VPL_UARTC1_MMR_BASE
#define UART_2                         VPL_UARTC2_MMR_BASE
#define UART_3                         VPL_UARTC3_MMR_BASE
//#define UART_PORT                      UART_0//ON FPGA
//#define UART_PORT                      UART_2//ON EVM
#define SERIAL_THR                     0x04	 		/* Transmitter Holding Register(Write).*/
#define SERIAL_RBR                     0x04	 		/* Receive Buffer register (Read).*/
#define SERIAL_IER                     0x08	 		/* Interrupt Enable register */
#define SERIAL_FCR                     0x0C
#define SERIAL_LCR                     0x10	 		/* Line Control register.*/
#define SERIAL_MCR                     0x14
#define SERIAL_LSR                     0x18	 		/* Line status register(Read) .*/
#define SERIAL_DLL                     0x04      		/* Divisor Register LSB */
#define SERIAL_DLM                     0x08      		/* Divisor Register MSB */
#define SERIAL_MCR_DTR                 0x1			/* Data Terminal ready */
#define SERIAL_LSR_DR                  0x1      		/* Data Ready */
#define SERIAL_LSR_THRE                0x20     		/* THR Empty */

#if defined(CONFIG_ASC88XX_A)
	#if defined (CONFIG_FPGA_20MHZ)
#define EVM_UART_CLOCK      18432000
	#else
#define EVM_UART_CLOCK      24000000
	#endif
#else
#define EVM_UART_CLOCK      18432000
#endif
#define EVM_BAUD_115200	    (EVM_UART_CLOCK/1843200)
#define EVM_BAUD_57600	    (EVM_UART_CLOCK/921600)
#define EVM_BAUD_38400	    (EVM_UART_CLOCK/614400)
#define EVM_BAUD_19200	    (EVM_UART_CLOCK/307200)
//#define EVM_BAUD_14400	    (EVM_UART_CLOCK/230400)
#define EVM_BAUD_9600	    (EVM_UART_CLOCK/153600)

#define PARITY_NONE		               0
#define PARITY_ODD		               1
#define PARITY_EVEN		               2
#define PARITY_MARK		               3
#define PARITY_SPACE	               4

#define ENABLE                         1
#define DISABLE                        0

/* MDR register */
#define SERIAL_MDR_MODE_SEL				0x03
#define SERIAL_MDR_UART					0x0
#define SERIAL_MDR_SIR					0x1
#define SERIAL_MDR_FIR					0x2

/* LCR Register */
#define SERIAL_LCR_LEN5                0x0
#define SERIAL_LCR_LEN6                0x1
#define SERIAL_LCR_LEN7                0x2
#define SERIAL_LCR_LEN8                0x3

#define SERIAL_LCR_STOP                0x4
#define SERIAL_LCR_EVEN                0x18 	 	/* Even Parity */
#define SERIAL_LCR_ODD                 0x8      	/* Odd Parity */
#define SERIAL_LCR_PE                  0x8			/* Parity Enable */
#define SERIAL_LCR_SETBREAK            0x40	 		/* Set Break condition */
#define SERIAL_LCR_STICKPARITY         0x20	 		/* Stick Parity Enable */
#define SERIAL_LCR_DLAB                0x80     	/* Divisor Latch Access Bit */

/* FCR Register */
#define SERIAL_FCR_FE                  0x1 	 		/* FIFO Enable */
#define SERIAL_FCR_RXFR                0x2 	 		/* Rx FIFO Reset */
#define SERIAL_FCR_TXFR                0x4 	 		/* Tx FIFO Reset */

extern char fLib_GetSerialChar(unsigned long port);

#endif
