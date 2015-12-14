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
#include <asm/arch/gpio.h>
#include <asm/arch/io.h>
#include <asm/arch/hardware.h>
#if defined(CONFIG_RESET_PHY_R) && defined(CONFIG_MACB)
#include <net.h>
#endif
#include <netdev.h>

#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	/* Enable Ctrlc */
	console_init_f();

	/* adress of boot parameters */
    gd->bd->bi_arch_number = MACH_TYPE_EVM ;
	gd->bd->bi_boot_params = BOOT_PARAMS_ADDR ;

	return 0;
}

#define EVM_SDRAMC_SIZE_CFG		    0xC
#define EVM_SDRAM_BANK_ADDR_MASK	0xFFF00000
int dram_init(void)
{
#if 1 /* the base address should be the same as that in AHB salve base */
	int i, j, ddrctrl;

	for (i=j=0, ddrctrl=DDR2SDMC0_MMR_BASE; i < MAX_DDRSDMC_NUMBERS; i++, ddrctrl += 0x01000000)
	{
		if ((readl(ddrctrl + DDR_CTRL) & EN(1)) && j < CONFIG_DRAM_NUMBER)
		{
			int v = readl(ddrctrl + DDR_SIZE);
			gd->bd->bi_dram[j].start = v & BASE_ADDR(0xff000000);
			gd->bd->bi_dram[j++].size = CALC_DDR_SIZE(v);
		}
	}

	/* bubble sorting by start address */
	for (i = CONFIG_DRAM_NUMBER; i != 0;)
	{
		int n;

		for (n = 0, j = 0; j < i-1; j++)
		{
			if (gd->bd->bi_dram[j].start > gd->bd->bi_dram[j+1].start)
			{
				ulong start = gd->bd->bi_dram[j].start;
				ulong size  = gd->bd->bi_dram[j].size;
				gd->bd->bi_dram[j].start = gd->bd->bi_dram[j+1].start;
				gd->bd->bi_dram[j].size  = gd->bd->bi_dram[j+1].size;
				gd->bd->bi_dram[j+1].start = start;
				gd->bd->bi_dram[j+1].size  = size;
				n = j + 1;
			}
		}
		i = n;
	}
#else
    unsigned long config_reg_val;

    config_reg_val = v_inl(AHBC1_MMR_BASE + AHBC_MMR_SLAVE_BASESIZE(DDR1_SLAVENUM)) ;
    gd->bd->bi_dram[0].start = config_reg_val & EVM_SDRAM_BANK_ADDR_MASK;
	
    config_reg_val = v_inl(DDR2SDMC1_MMR_BASE + EVM_SDRAMC_SIZE_CFG);
    gd->bd->bi_dram[0].size = (1 << (config_reg_val & 0x0000000f)) *
                          (1 << ((config_reg_val & 0x000000f0) >> 4)) *
                          (4 *  ((config_reg_val & 0x00000300) >> 8)) * 2;

#if (CONFIG_DRAM_NUMBER == 2)    
    config_reg_val = v_inl(AHBC1_MMR_BASE + AHBC_MMR_SLAVE_BASESIZE(DDR0_SLAVENUM)) ;
    gd->bd->bi_dram[1].start = config_reg_val & EVM_SDRAM_BANK_ADDR_MASK;
    
    config_reg_val = v_inl(DDR2SDMC0_MMR_BASE + EVM_SDRAMC_SIZE_CFG);
    gd->bd->bi_dram[1].size = (1 << (config_reg_val & 0x0000000f)) *
                          (1 << ((config_reg_val & 0x000000f0) >> 4)) *
                          (4 *  ((config_reg_val & 0x00000300) >> 8)) * 2;
#endif

/*    config_reg_val = v_inl(EVM_DDR2SDMC1_BASE + EVM_SDRAMC_SIZE_CFG);
    gd->bd->bi_dram[1].start = config_reg_val & EVM_SDRAM_BANK_ADDR_MASK;
    gd->bd->bi_dram[1].size = (1 << (config_reg_val & 0x0000000f)) *
                          (1 << ((config_reg_val & 0x000000f0) >> 4)) *
                          (4 *  ((config_reg_val & 0x00000300) >> 8)) * 2;
*/
#endif

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

	return rc;
}

#ifdef CONFIG_GENERIC_MMC

#include <mmc.h>
#include "../drivers/mmc/dw_mmc.h"

extern void reset_mmc_host(dw_mmc_host_t *host);
extern int dw_mmc_init(struct mmc *mmc);

static dw_mmc_t   evm_mmc_devs[PLATFORM_SDCARD_NUM] =
	{
		{
			.host = (void *)VPL_MSHC_0,
			.cardnum = 0,
		},
#if PLATFORM_SDCARD_NUM > 1
		{
			.host = (void *)VPL_MSHC_1,
			.cardnum = 0,
		}
#endif
#if PLATFORM_SDCARD_NUM > 2
#error PLATFORM_SDCARD_NUM exceeds 2!
#endif
	};


static struct mmc   mmc_devs[PLATFORM_SDCARD_NUM] =
	{
		{.priv = &evm_mmc_devs[0]},
#if PLATFORM_SDCARD_NUM > 1
		{.priv = &evm_mmc_devs[1]}
#endif
	};

int board_mmc_init(void)
{
	int i;

	/* reset host controller */
	reset_mmc_host((void *)VPL_MSHC_0);
#if PLATFORM_SDCARD_NUM > 1
	reset_mmc_host((void *)VPL_MSHC_1);
#endif

	/* init mmc objects */
	for (i = 0 ; i < PLATFORM_SDCARD_NUM; i++)
		dw_mmc_init(&mmc_devs[i]);

	/* TODO: power down all SD cards */
	/* TODO: any else ??? */

	return 0;
}
#endif

