/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
 *
 */

#include <common.h>
#include <command.h>

#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

#define GMACTXSWITCH  0x1
#define LEDSWITCH     0x2

int do_gpio (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{    
    unsigned long reg ;
    
    if (argc != 3) {
        goto gpio_usage ;
    }   

    reg = v_inl(UART_0 + SERIAL_MCR) ;

    if(strcmp(argv[1], "switch") == 0) {        
        if(strcmp(argv[2], "on") == 0) {
            v_outl(UART_0 + SERIAL_MCR, reg | GMACTXSWITCH) ;            
        }
        else if(strcmp(argv[2], "off") == 0) {
            v_outl(UART_0 + SERIAL_MCR, reg & (~(GMACTXSWITCH))) ;
        }
        else
            goto gpio_usage ;
    }
    else if(strcmp(argv[1], "led") == 0) {
        if(strcmp(argv[2], "on") == 0) {
            v_outl(UART_0 + SERIAL_MCR, reg | LEDSWITCH) ;
        }
        else if(strcmp(argv[2], "off") == 0) {
            v_outl(UART_0 + SERIAL_MCR, reg & (~(LEDSWITCH))) ;
        }
        else
            goto gpio_usage ;        
    }
    else 
        goto gpio_usage ;
    

    return 0;
    
gpio_usage :
    printf ("Usage:\n%s\n", cmdtp->usage);
    return 1;
}

U_BOOT_CMD(
	gpio, 3,	0,	do_gpio,
	"Set gpio pins",
	"gpio switch [on/off] - Switch gmac tx direction or not.\n"
	"gpio led [on/off]    - Turn-on LED or turn off.\n"
);

