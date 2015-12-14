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
//#include <asm/arch/haydn/haydn.h>
//#include <asm/arch/hardware.h>

//#define DEBUG_SERIAL_BASE	IO_ADDRESS(HAYDN_UARTC0_MMR_BASE)

//void debug_puts(const char *s, int bIsVirtual)
void debug_puts(const char *s)
{
	//unsigned int SerialBase = bIsVirtual ? IO_ADDRESS(HAYDN_UARTC0_MMR_BASE) : HAYDN_UARTC0_MMR_BASE;
	unsigned int SerialBase = IO_ADDRESS(HAYDN_UARTC1_MMR_BASE);

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
