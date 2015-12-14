/*
 * spi-gpio.h: Mozart SPI-GPIO device header
 *
 * Copyright (C) 2012  VN Inc.
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

#ifndef __ASM_ARCH_SPIGPIO_H
#define __ASM_ARCH_SPIGPIO_H __FILE__

struct mozart_spigpio_info {
	unsigned long		 pin_clk;
	unsigned long		 pin_mosi;
	unsigned long		 pin_miso;
	unsigned long		 pin_chip_sel;

	int			 num_chipselect;
	int			 bus_num;
	int			 write_only;
	void (*chip_select)(struct mozart_spigpio_info *spi, int cs);
};


#endif /* __ASM_ARCH_SPIGPIO_H */
