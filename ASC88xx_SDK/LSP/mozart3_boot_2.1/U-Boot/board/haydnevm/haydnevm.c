/*
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian.pop@leadtechdesign.com>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
/*#include <asm/arch/at91sam9260.h>
#include <asm/arch/at91sam9260_matrix.h>
#include <asm/arch/at91sam9_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_rstc.h>*/
//#include <asm/arch/gpio.h>
//#include <asm/arch/io.h>
//#include <asm/arch/hardware.h>
#if defined(CONFIG_RESET_PHY_R) && defined(CONFIG_MACB)
#include <net.h>
#endif
#include <netdev.h>

#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

/* ------------------------------------------------------------------------- */
/*
 * Miscelaneous platform dependent initialisations
 */
int board_init(void)
{
	/* Enable Ctrlc */
	console_init_f();

	/* adress of boot parameters */
    gd->bd->bi_arch_number = MACH_TYPE_EVM ;
	gd->bd->bi_boot_params = BOOT_PARAMS_ADDR ;

	return 0;
}

#define EVM_SDRAMC_BASE				0x98100000
#define EVM_SDRAMC_SIZE_CFG		0xc
#define EVM_SDRAM_BANK_ADDR_MASK	0xFFF00000
int dram_init(void)
{
    unsigned long config_reg_val;
	
    config_reg_val = v_inl(EVM_SDRAMC_BASE + EVM_SDRAMC_SIZE_CFG);
    gd->bd->bi_dram[0].start = config_reg_val & EVM_SDRAM_BANK_ADDR_MASK;

    gd->bd->bi_dram[0].size = (1 << (config_reg_val & 0x0000000f)) *
                          (1 << ((config_reg_val & 0x000000f0) >> 4)) *
                          (4 *  ((config_reg_val & 0x00000300) >> 8)) * 2;

    return 0;    
}

#ifdef CONFIG_RESET_PHY_R
void reset_phy(void)
{
#ifdef CONFIG_MACB
	/*
	 * Initialize ethernet HW addr prior to starting Linux,
	 * needed for nfsroot
	 */
	eth_init(gd->bd);
#endif
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;
/*
#ifdef CONFIG_MACB
	rc = macb_eth_initialize(0, (void *)AT91SAM9260_BASE_EMAC, 0x00);
#endif
*/
    //gmac_eth_initialize(0, (void *)AT91SAM9260_BASE_EMAC, 0x00);
	return rc;
}
