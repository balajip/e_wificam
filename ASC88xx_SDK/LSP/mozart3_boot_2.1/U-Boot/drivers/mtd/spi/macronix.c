/*
 * Copyright 2009(C) Marvell International Ltd. and its affiliates
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Based on drivers/mtd/spi/stmicro.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <asm/sizes.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

/* MX25xx-specific commands */
#define CMD_MX25XX_WREN		0x06	/* Write Enable */
#define CMD_MX25XX_WRDI		0x04	/* Write Disable */
#define CMD_MX25XX_RDSR		0x05	/* Read Status Register */
#define CMD_MX25XX_WRSR		0x01	/* Write Status Register */
#define CMD_MX25XX_READ		0x03	/* Read Data Bytes */
#define CMD_MX25XX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_MX25XX_PP		0x02	/* Page Program */
#define CMD_MX25XX_SE		0x20	/* Sector Erase */
#define CMD_MX25XX_BE		0xD8	/* Block Erase */
#define CMD_MX25XX_CE		0xc7	/* Chip Erase */
#define CMD_MX25XX_DP		0xb9	/* Deep Power-down */
#define CMD_MX25XX_RES		0xab	/* Release from DP, and Read Signature */
#define CMD_MX25XX_EN4B     0xB7    /* Enter 4-byte mode */
#define CMD_MX25XX_RDSCUR   0x2B    /* Read Security Register */
#define CMD_MX25XX_RDCR   0x15    /* Read Security Register */

#define MACRONIX_SR_WIP		(1 << 0)	/* Write-in-Progress */

struct macronix_spi_flash_params {
	u16 idcode;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_blocks;
	const char *name;
};

struct macronix_spi_flash {
	struct spi_flash flash;
	uint32_t flags;
	const struct macronix_spi_flash_params *params;
};

static inline struct macronix_spi_flash *to_macronix_spi_flash(struct spi_flash
							       *flash)
{
	return container_of(flash, struct macronix_spi_flash, flash);
}

static const struct macronix_spi_flash_params macronix_spi_flash_table[] = {
	{
		.idcode = 0x2015,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 32,
		.name = "MX25L1605D",
	},
	{
		.idcode = 0x2016,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 64,
		.name = "MX25L3205D",
	},
	{
		.idcode = 0x2017,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 128,
		.name = "MX25L6405D",
	},
	{
		.idcode = 0x2018,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25L12805D",
	},
	{
		.idcode = 0x2618,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25L12855E",
	},
	{
		.idcode = 0x2019,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 512,
		.name = "MX25L25635E/F",
	},
};

/*
 * enter_4_byte_mode()
 * Return: 0, 3 byte address
 *         1, 4 byte address
 *         else, error
 */
static int _enter_4_byte_mode(struct spi_flash *flash)
{
	int ret;
	u8 cmd;

	ret = spi_flash_cmd(flash->spi, CMD_MX25XX_EN4B, NULL, 0);
	if (ret) {
		debug("SF: Failed to send command %02x: %d\n", CMD_MX25XX_EN4B, ret);
		return -1;
	}

	cmd = CMD_MX25XX_RDSCUR;
	
	ret = spi_flash_cmd(flash->spi, cmd, &cmd, 1);
	if (ret) {
		debug("SF: Failed to send command %02x: %d\n", cmd, ret);
		return -1;
	}

	debug("status bit of 4-byte mode: %lu\n", cmd & BIT_MASK(2));

#if 1
	ret = (cmd & BIT_MASK(2)) != 0;
	if (ret) 
	{
		return ret;
	}
	else		//may be rev.F, check RDCR
	{
		cmd = CMD_MX25XX_RDCR;
	
		ret = spi_flash_cmd(flash->spi, cmd, &cmd, 1);
		if (ret) {
			debug("SF: Failed to send command %02x: %d\n", cmd, ret);
			return -1;
		}
		
		return (cmd & BIT_MASK(5)) != 0;
	}
#else
	return (cmd & BIT_MASK(2)) != 0;
#endif
}

static int enter_4_byte_mode(struct spi_flash *flash)
{
	struct macronix_spi_flash *mcx = to_macronix_spi_flash(flash);
	int ret = 0;

	if ((mcx->flags & (ADDR_4_BYTE_SUPPORT|ADDR_4_BYTE_ENTERED))
		== ADDR_4_BYTE_SUPPORT)
	{
		ret = _enter_4_byte_mode(flash);
		if (ret == 1)
			mcx->flags |= ADDR_4_BYTE_ENTERED;
	}

	debug("%s 4-byte address mode\n", (mcx->flags & ADDR_4_BYTE_ENTERED) ? "is" : "not");
	return (mcx->flags & ADDR_4_BYTE_ENTERED) == ADDR_4_BYTE_ENTERED;
}

static int macronix_wait_ready(struct spi_flash *flash, unsigned long timeout)
{
	struct spi_slave *spi = flash->spi;
	unsigned long timebase;
	int ret;
	u8 status;

	timebase = get_timer(0);
	do {
		ret = spi_flash_cmd(spi, CMD_MX25XX_RDSR, &status, sizeof(status));
		if (ret)
			return -1;

		if ((status & MACRONIX_SR_WIP) == 0)
			break;

	} while (get_timer(timebase) < timeout);

	if ((status & MACRONIX_SR_WIP) == 0)
		return 0;

	/* Timed out */
	return -1;
}

static int macronix_read_fast(struct spi_flash *flash,
			      u32 offset, size_t len, void *buf)
{
	u8 cmd[6], i;
	int addr_4byte;


	addr_4byte = enter_4_byte_mode(flash);
	if (addr_4byte < 0)
		return addr_4byte;

	cmd[0] = CMD_READ_ARRAY_FAST;
	i = 1;
	if (addr_4byte)
		cmd[i++] = offset >> 24;
	cmd[i++] = offset >> 16;
	cmd[i++] = offset >> 8;
	cmd[i++] = offset;
	cmd[i++] = 0x00; // dummy

	debug("READ(%02x) %02x%02x%02x%02x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
	if (addr_4byte) debug("%02x", cmd[5]);
	debug("\n");
	return spi_flash_read_common(flash, cmd, i, buf, len);
}

static int macronix_write(struct spi_flash *flash,
			  u32 offset, size_t len, const void *buf, int verbose)
{
	struct macronix_spi_flash *mcx = to_macronix_spi_flash(flash);
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret;
	u8 cmd[5], i;
	int addr_4byte;
	size_t actual_pre = 0, v = 0;


	addr_4byte = enter_4_byte_mode(flash);
	if (addr_4byte < 0)
		return addr_4byte;

	page_size = mcx->params->page_size;
	byte_addr = offset % page_size;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	if (verbose)
	{
		v = len / 10; // 10%
		actual_pre = 0;
	}

	ret = 0;
	for (actual = 0; actual < len; actual += chunk_len)
	{
		if (verbose && (actual == 0 || actual - actual_pre >= v))
		{
			print_progress(actual, len);
			actual_pre = actual;
		}

		chunk_len = min(len - actual, page_size - byte_addr);

		cmd[0] = CMD_MX25XX_PP;
		i = 1;
		if (addr_4byte)
			cmd[i++] = offset >> 24;
		cmd[i++] = offset >> 16;
		cmd[i++] = offset >> 8;
		cmd[i++] = offset;

		debug
		    ("PP: 0x%p, chunk_len = %d => cmd = { 0x%02x 0x%02x%02x%02x",
		     buf + actual, chunk_len, cmd[0], cmd[1], cmd[2], cmd[3]);
		if (addr_4byte) debug("%02x", cmd[4]);
		debug(" }\n");

		ret = spi_flash_cmd(flash->spi, CMD_MX25XX_WREN, NULL, 0);
		if (ret < 0) {
			debug("SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, i,
					  buf + actual, chunk_len);
		if (ret < 0) {
			debug("SF: Macronix Page Program failed\n");
			break;
		}

		ret = macronix_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret < 0) {
			debug("SF: Macronix page programming timed out\n");
			break;
		}

		byte_addr = 0;
		offset += chunk_len;
	}
	if (verbose)
	{
		print_progress(actual, len);
		printf("\n");
	}

	debug("SF: Macronix: Successfully programmed %u bytes @ 0x%x\n",
	      len, offset);

	spi_release_bus(flash->spi);
	return ret;
}

int macronix_erase(struct spi_flash *flash, u32 offset, size_t len, int verbose)
{
	struct macronix_spi_flash *mcx = to_macronix_spi_flash(flash);
	unsigned long sector_size;
	size_t actual;
	int ret;
	u8 cmd[4];
	int addr_4byte;
	size_t actual_pre = 0, v = 0;

	/*
	 * This function currently uses sector erase only.
	 * probably speed things up by using bulk erase
	 * when possible.
	 */
	sector_size = mcx->params->page_size * mcx->params->pages_per_sector
			* mcx->params->sectors_per_block;

	if (offset % sector_size || len % sector_size) {
		debug("SF: Erase offset/length not multiple of sector size\n");
		return -1;
	}

	addr_4byte = enter_4_byte_mode(flash);
	if (addr_4byte < 0)
		return addr_4byte;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	if (verbose)
	{
		v = len / 10; // 10%
		actual_pre = 0;
	}
	ret = 0;
	cmd[0] = CMD_MX25XX_BE;
	for (actual = 0; actual < len; actual += sector_size, offset += sector_size) {
		int i = 1;

		if (verbose && (actual == 0 || actual - actual_pre >= v))
		{
			print_progress(actual, len);
			actual_pre = actual;
		}

		if (addr_4byte)
			cmd[i++] = offset >> 24;
		cmd[i++] = offset >> 16;
		cmd[i++] = offset >> 8;
		cmd[i++] = offset;

		ret = spi_flash_cmd(flash->spi, CMD_MX25XX_WREN, NULL, 0);
		if (ret < 0) {
			debug("SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, i, NULL, 0);
		if (ret < 0) {
			debug("SF: Macronix page erase failed\n");
			break;
		}

		ret = macronix_wait_ready(flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
		if (ret < 0) {
			debug("SF: Macronix page erase timed out\n");
			break;
		}
	}
	if (verbose)
	{
		print_progress(actual, len);
		printf("\n");
	}

	debug("SF: Macronix: Successfully erased %u bytes @ 0x%x\n",
	      len, offset - len);

	spi_release_bus(flash->spi);
	return ret;
}

struct spi_flash *spi_flash_probe_macronix(struct spi_slave *spi, u8 *idcode)
{
	const struct macronix_spi_flash_params *params;
	struct macronix_spi_flash *mcx;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;

	for (i = 0; i < ARRAY_SIZE(macronix_spi_flash_table); i++) {
		params = &macronix_spi_flash_table[i];
		if (params->idcode == id)
			break;
	}

	if (i == ARRAY_SIZE(macronix_spi_flash_table)) {
		debug("SF: Unsupported Macronix ID %04x\n", id);
		return NULL;
	}

	mcx = malloc(sizeof(*mcx));
	if (!mcx) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	mcx->params = params;
	mcx->flash.spi = spi;
	mcx->flash.name = params->name;

	mcx->flash.write = macronix_write;
	mcx->flash.erase = macronix_erase;
	mcx->flash.read = macronix_read_fast;
	mcx->flash.size = params->page_size * params->pages_per_sector
	    * params->sectors_per_block * params->nr_blocks;
	mcx->flags = (mcx->flash.size > SZ_16M ? ADDR_4_BYTE_SUPPORT : 0);

	printf("SF: Detected %s with page size %u, total ",
	       params->name, params->page_size);
	print_size(mcx->flash.size, "\n");

	return &mcx->flash;
}
