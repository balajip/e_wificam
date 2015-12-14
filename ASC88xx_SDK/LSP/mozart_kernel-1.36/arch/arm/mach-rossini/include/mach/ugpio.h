/*
 * Copyright (C) 2013  VN Inc.
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

#ifndef    __ROSSINI_UGPIO_H
#define    __ROSSINI_UGPIO_H

#include <mach/platform.h>

extern spinlock_t rossini_ugpio_lock;
extern u32 rossini_ugpio_requested;

enum UGPIO_PIN {
    PIN_UGPIO0      = 0,
    PIN_UGPIO_nCTS  = 0,
    PIN_UGPIO1      = 1,
    PIN_UGPIO_nDSR  = 1,
    PIN_UGPIO2      = 2,
    PIN_UGPIO_nRI   = 2,
    PIN_UGPIO3      = 3,
    PIN_UGPIO_nDCD  = 3,
    PIN_UGPIO4      = 4,
    PIN_UGPIO_nDTR  = 4,
    PIN_UGPIO5      = 5,
    PIN_UGPIO_nRTS  = 5,
};

enum UGPIO_REQ {
	REQ_UGPIO0		= 1<<PIN_UGPIO0,
	REQ_UGPIO_nCTS	= 1<<PIN_UGPIO0,
	REQ_UGPIO1		= 1<<PIN_UGPIO1,
	REQ_UGPIO_nDSR	= 1<<PIN_UGPIO1,
	REQ_UGPIO2		= 1<<PIN_UGPIO2,
	REQ_UGPIO_nRI	= 1<<PIN_UGPIO2,
	REQ_UGPIO3		= 1<<PIN_UGPIO3,
	REQ_UGPIO_nDCD	= 1<<PIN_UGPIO3,
	REQ_UGPIO4		= 1<<PIN_UGPIO4,
	ERQ_UGPIO_nDTR	= 1<<PIN_UGPIO4,
	REQ_UGPIO5		= 1<<PIN_UGPIO5,
	REQ_UGPIO_nRTS	= 1<<PIN_UGPIO5,
};

#define ugpio_pin_to_bit(n)		(1<<n)
#define ugpio_input_bit(n)		(1<<(4+n))
#define ugpio_output_bit(n)		(1<<(n-4))

static inline int request_ugpio_pin(u32 pin)
{
    if ((rossini_ugpio_requested & ugpio_pin_to_bit(pin)) != 0) {
        return -EBUSY;
    } else {
        spin_lock(&rossini_ugpio_lock);
        rossini_ugpio_requested |= ugpio_pin_to_bit(pin);
        spin_unlock(&rossini_ugpio_lock);
    }

    return 0;
}

static inline void free_ugpio_pin(u32 pin)
{
    spin_lock(&rossini_ugpio_lock);
    rossini_ugpio_requested &= ~ugpio_pin_to_bit(pin);
    spin_unlock(&rossini_ugpio_lock);
}

static inline int read_ugpio_pin(u32 pin)
{
	u32 val;

	spin_lock(&rossini_ugpio_lock);
	val = inl(IO_ADDRESS(VPL_UARTC_0_MMR_BASE) + SERIAL_MSR);
	spin_unlock(&rossini_ugpio_lock);
	return ((val&ugpio_input_bit(pin)) == 0);
}

static inline int set_ugpio_pin(u32 pin)
{
	u32 reg;

	spin_lock(&rossini_ugpio_lock);
	reg = inl(IO_ADDRESS(VPL_UARTC_0_MMR_BASE) + SERIAL_MCR);
	reg &= ~ugpio_output_bit(pin);
	outl(reg, IO_ADDRESS(VPL_UARTC_0_MMR_BASE) + SERIAL_MCR);
	spin_unlock(&rossini_ugpio_lock);

	return 0;
}

static inline int clear_ugpio_pin(u32 pin)
{
	u32 reg;

	spin_lock(&rossini_ugpio_lock);
	reg = inl(IO_ADDRESS(VPL_UARTC_0_MMR_BASE) + SERIAL_MCR);
	reg |= ugpio_output_bit(pin);
	outl(reg, IO_ADDRESS(VPL_UARTC_0_MMR_BASE) + SERIAL_MCR);
	spin_unlock(&rossini_ugpio_lock);

	return 0;
}

#endif
/* __ROSSINI_UGPIO_H */
