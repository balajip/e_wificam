/*
 * (C) Copyright 2008
 * Stuart Wood, Lab X Technologies <stuart.wood@labxtechnologies.com>
 *
 * (C) Copyright 2004
 * Jian Zhang, Texas Instruments, jzhang@ti.com.

 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* #define DEBUG */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <nand.h>

#if defined(CONFIG_CMD_SAVEENV) && defined(CONFIG_CMD_NAND)
#define CMD_SAVEENV
#elif defined(CONFIG_ENV_OFFSET_REDUND)
#error Cannot use CONFIG_ENV_OFFSET_REDUND without CONFIG_CMD_SAVEENV & CONFIG_CMD_NAND
#endif

#if defined(CONFIG_ENV_SIZE_REDUND) && (CONFIG_ENV_SIZE_REDUND != CONFIG_ENV_SIZE)
#error CONFIG_ENV_SIZE_REDUND should be the same as CONFIG_ENV_SIZE
#endif

#ifdef CONFIG_INFERNO
#error CONFIG_INFERNO not supported yet
#endif

#ifndef CONFIG_ENV_RANGE
#define CONFIG_ENV_RANGE	CONFIG_ENV_SIZE
#endif

/* references to names in env_common.c */
extern uchar default_environment[];
extern int default_environment_size;

extern char *env_name_spec;
extern env_t *env_ptr ;

/* local functions */
#if !defined(ENV_IS_EMBEDDED)
static void use_default(void);
#endif

DECLARE_GLOBAL_DATA_PTR;

//[jam] We replace macros by global variables.
static unsigned long config_env_range_nand = CONFIG_ENV_RANGE_NAND;
static unsigned long config_env_offset_nand = CONFIG_ENV_OFFSET_NAND ;

uchar nand_env_get_char_spec (int index)
{
	return ( *((uchar *)(gd->env_addr + index)) );
}


/* this is called before nand_init()
 * so we can't read Nand to validate env data.
 * Mark it OK for now. env_relocate() in env_common.c
 * will call our relocate function which does the real
 * validation.
 *
 * When using a NAND boot image (like sequoia_nand), the environment
 * can be embedded or attached to the U-Boot image in NAND flash. This way
 * the SPL loads not only the U-Boot image from NAND but also the
 * environment.
 */
int nand_env_init(void)
{
	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 1;

	config_env_range_nand = config_env_range_nand * nand_info[0].erasesize ;
	config_env_offset_nand = config_env_offset_nand * nand_info[0].erasesize ;

	env_name_spec = "NAND";
	return (0);
}

#ifdef CMD_SAVEENV
/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
int writeenv(size_t offset, u_char *buf)
{
	size_t end = offset + config_env_range_nand;
	size_t amount_saved = 0;
	size_t blocksize, len;

	u_char *char_ptr;

	blocksize = nand_info[0].erasesize;
	len = min(blocksize, CONFIG_ENV_SIZE);

	while (amount_saved < CONFIG_ENV_SIZE && offset < end) {
		if (nand_block_isbad(&nand_info[0], offset)) {
			offset += blocksize;
		} else {
			char_ptr = &buf[amount_saved];
			if (nand_write(&nand_info[0], offset, &len,	char_ptr))
				return 1;
			offset += blocksize;
			amount_saved += len;
		}
	}
	if (amount_saved != CONFIG_ENV_SIZE)
		return 1;

	return 0;
}

int nand_saveenv(void)
{
	size_t total;
	int ret = 0;
	nand_erase_options_t nand_erase_options;

	nand_erase_options.length = config_env_range_nand;
	nand_erase_options.quiet = 0;
	nand_erase_options.jffs2 = 0;
	nand_erase_options.scrub = 0;
	nand_erase_options.offset = config_env_offset_nand;

	if (config_env_range_nand < CONFIG_ENV_SIZE)
		return 1;
	puts ("Erasing Nand...\n");
	if (nand_erase_opts(&nand_info[0], &nand_erase_options))
		return 1;

	puts ("Writing to Nand... ");
	total = CONFIG_ENV_SIZE;
	if (writeenv(config_env_offset_nand, (u_char *) env_ptr)) {
		puts("FAILED!\n");
		return 1;
	}

	puts ("done\n");
	return ret;
}
#endif /* CMD_SAVEENV */

int readenv (size_t offset, u_char * buf)
{
	size_t end = offset + config_env_range_nand;
	size_t amount_loaded = 0;
	size_t blocksize, len;

	u_char *char_ptr;

	blocksize = nand_info[0].erasesize;
	len = min(blocksize, CONFIG_ENV_SIZE);

    //we must write envs in the first valid block starting from offset
	while (amount_loaded < CONFIG_ENV_SIZE && offset < end) {
		if (nand_block_isbad(&nand_info[0], offset)) {
			offset += blocksize;
		} else {
			char_ptr = &buf[amount_loaded];
			if (nand_read(&nand_info[0], offset, &len, char_ptr))
				return 1;
			offset += blocksize;//[traca] go to next block
			amount_loaded += len;
		}
	}
	if (amount_loaded != CONFIG_ENV_SIZE)
		return 1;

	return 0;
}

/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
void nand_env_relocate_spec (void)
{
#if !defined(ENV_IS_EMBEDDED)
	int ret;

	ret = readenv(config_env_offset_nand, (u_char *) env_ptr);
	if (ret)
		return use_default();

	if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
		return use_default();

    printf("  The environment varialbes stored in Nand Flash are found.\n") ;
#endif /* ! ENV_IS_EMBEDDED */
}

#if !defined(ENV_IS_EMBEDDED)
static void use_default()
{
	puts ("*** Warning - bad CRC or NAND, using default environment\n\n");
	set_default_env();
}
#endif
