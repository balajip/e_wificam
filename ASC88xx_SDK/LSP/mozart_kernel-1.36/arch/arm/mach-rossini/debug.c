/*
 *  linux/include/asm-arm/arch-integrator/debug-cpe.c
 *
 *  Copyright (C) 1999 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <mach/platform.h>
#include <mach/rossini.h>
#include <mach/hardware.h>

#if defined(CONFIG_ROSSINI_UART_PORT_NUM_0)
#define DEBUG_PORT_BASE ROSSINI_UARTC0_MMR_BASE
#elif defined(CONFIG_ROSSINI_UART_PORT_NUM_1)
#define DEBUG_PORT_BASE ROSSINI_UARTC1_MMR_BASE
#elif defined(CONFIG_ROSSINI_UART_PORT_NUM_2)
#define DEBUG_PORT_BASE ROSSINI_UARTC2_MMR_BASE
#elif defined(CONFIG_ROSSINI_UART_PORT_NUM_3)
#define DEBUG_PORT_BASE ROSSINI_UARTC3_MMR_BASE
#else
#error "You have to select correct UART-PORT-NUMBER(0~3)!!"
#endif

void debug_puts(const char *s)
{
    unsigned int SerialBase = IO_ADDRESS(DEBUG_PORT_BASE);
	//unsigned int SerialBase = IO_ADDRESS(ROSSINI_UARTC1_MMR_BASE);//james_test

	while (*s) {
		volatile unsigned int status = 0;
		do {
			status = *(unsigned char *)(SerialBase + SERIAL_LSR);
		}
		while (!((status & SERIAL_LSR_THRE) == SERIAL_LSR_THRE));

		*(unsigned char *) (SerialBase + SERIAL_THR) = *s;

		if (*s == '\n') {

			do {
				status = *(unsigned char *) (SerialBase + SERIAL_LSR);
			}
			while (!((status & SERIAL_LSR_THRE) == SERIAL_LSR_THRE));

			*(unsigned char *) (SerialBase + SERIAL_THR) = '\r';
		}
		s++;
	}
}
