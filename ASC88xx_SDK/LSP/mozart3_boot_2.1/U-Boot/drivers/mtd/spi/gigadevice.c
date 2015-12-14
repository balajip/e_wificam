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
#define CMD_GD5FXX_WREN		0x06	/* Write Enable */
#define CMD_GD5FXX_WRDI		0x04	/* Write Disable */
#define CMD_GD5FXX_RDSR		0x05	/* Read Status Register */
#define CMD_GD5FXX_WRSR		0x01	/* Write Status Register */
#define CMD_GD5FXX_READ		0x03	/* Read Data Bytes */
#define CMD_GD5FXX_PAGE_READ	0x13	/* Read Data Bytes */
#define CMD_GD5FXX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_GD5FXX_PP		0x02	/* Page Program */
#define CMD_GD5FXX_PE		0x10	/* Program execute */
#define CMD_GD5FXX_GF		0x0F	/* Get features */
#define CMD_GD5FXX_SF		0x1F	/* Get features */
#define CMD_GD5FXX_SE		0x20	/* Sector Erase */
#define CMD_GD5FXX_BE		0xD8	/* Block Erase */
#define CMD_GD5FXX_CE		0xc7	/* Chip Erase */
#define CMD_GD5FXX_DP		0xb9	/* Deep Power-down */
#define CMD_GD5FXX_RES		0xab	/* Release from DP, and Read Signature */
#define CMD_GD5FXX_EN4B     0xB7    /* Enter 4-byte mode */
#define CMD_GD5FXX_RDSCUR   0x2B    /* Read Security Register */
#define CMD_GD5FXX_RDCR   0x15    /* Read Security Register */

#define CMD_GD5FXX_PROTECTION 0xA0
#define CMD_GD5FXX_FEATURE		0xB0
#define CMD_GD5FXX_STATUS			0xC0
#define GIGADEVICE_SR_OIP		(1 << 0)	/* Write-in-Progress */


struct gigadevice_spi_flash_params {
	u16 idcode;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_blocks;
	const char *name;
};

struct gigadevice_spi_flash {
	struct spi_flash flash;
	uint32_t flags;
	const struct gigadevice_spi_flash_params *params;
};

static inline struct gigadevice_spi_flash *to_gigadevice_spi_flash(struct spi_flash
							       *flash)
{
	return container_of(flash, struct gigadevice_spi_flash, flash);
}

static const struct gigadevice_spi_flash_params gigadevice_spi_flash_table[] = {
	{
		.idcode = 0xc8f1,
		.page_size = 2048,
		.pages_per_sector = 64,
		.sectors_per_block = 1,
		.nr_blocks = 1024,
		.name = "GD5F1GQ4UAYIG",
	},
	{
		.idcode = 0x9b12,
		.page_size = 2048,
		.pages_per_sector = 64,
		.sectors_per_block = 1,
		.nr_blocks = 1024,
		.name = "ATO25T1GA",
	},
	{
		.idcode = 0xc820,
		.page_size = 2048,
		.pages_per_sector = 64,
		.sectors_per_block = 1,
		.nr_blocks = 1024,
		.name = "EN25LN512",
	},
};


static int gigadevice_wait_ready(struct spi_flash *flash, unsigned long timeout)
{
	struct spi_slave *spi = flash->spi;
	unsigned long timebase;
	int ret;
	u8 status;
	u8 cmd[2] = { CMD_GD5FXX_GF, CMD_GD5FXX_STATUS};

	timebase = get_timer(0);
	do {
		ret = spi_flash_cmd_read(spi, cmd, sizeof(cmd), &status, sizeof(status));
		if (ret)
			return -1;

		debug(" status %x \n", status);
		if ((status & GIGADEVICE_SR_OIP) == 0)
			break;

	} while (get_timer(timebase) < timeout);

	if ((status & GIGADEVICE_SR_OIP) == 0)
		return 0;

	/* Timed out */
	return -1;
}

static int gigadevice_read_fast(struct spi_flash *flash,
			      u32 offset, size_t len, void *buf)
{
	struct gigadevice_spi_flash *gd = to_gigadevice_spi_flash(flash);
	u8 cmd[6], i;
	int ret = 0;
	u32 pageaddr = 0;
	size_t chunk_len;
	size_t actual;
	u32 page_size = gd->params->page_size;
	u32 byte_addr = offset % page_size;
  
	for (actual = 0; actual < len; actual += chunk_len)
	{
		chunk_len = min(len - actual, page_size - byte_addr);


		cmd[0] = CMD_GD5FXX_PAGE_READ;
		i = 1;
		cmd[i++] = 0;	// dummy
		cmd[i++] = offset >> 19;
		cmd[i++] = offset >> 11;
	
		debug("READ(%02x) %02x%02x%02x%02x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
		debug("\n");
		spi_flash_cmd_write(flash->spi, cmd, i, NULL, 0);
	
		
		ret = gigadevice_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret < 0) {
				debug("SF: gigadevice page read timed out\n");
		}
			
		cmd[0] = CMD_GD5FXX_READ; //CMD_READ_ARRAY_FAST;
		i = 1;
		cmd[i++] = (offset >> 8) & 0x7;
		cmd[i++] = offset;
		cmd[i++] = 0x00; // dummy
	
		debug("READ(%02x) %02x%02x%02x%02x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
		debug("\n");
		ret = spi_flash_read_common(flash, cmd, i, buf + actual, chunk_len);
				
		byte_addr = 0;
		offset += chunk_len;
	}
	
	return ret;
}

static int gigadevice_write(struct spi_flash *flash,
			  u32 offset, size_t len, const void *buf, int verbose)
{
	struct gigadevice_spi_flash *gd = to_gigadevice_spi_flash(flash);
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret;
	u8 cmd[5], i;
	size_t actual_pre = 0, v = 0;

	page_size = gd->params->page_size;
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

		cmd[0] = CMD_GD5FXX_PP;
		i= 1;
		cmd[i++] = (offset >> 8) & 0x7;
		cmd[i++] = offset;

		debug
		    ("\nPL: 0x%p, chunk_len = %d => cmd = { 0x%02x 0x%02x%02x%02x, %02x}",
		     buf + actual, chunk_len, cmd[0], cmd[1], cmd[2], cmd[3], i);

		ret = spi_flash_cmd_write(flash->spi, cmd, i,
					  buf + actual, chunk_len);
		if (ret < 0) {
			debug("SF: gigadevice page Program load failed\n");
			break;
		}

		ret = spi_flash_cmd(flash->spi, CMD_GD5FXX_WREN, NULL, 0);
		if (ret < 0) {
			debug("SF: Enabling Write failed\n");
			break;
		}
		
		cmd[0] = CMD_GD5FXX_PE;
		i=1;
		cmd[i++] = 0;
		cmd[i++] = offset >> 19;
		cmd[i++] = offset >> 11;
		debug
		    ("\nPE: 0x%p, chunk_len = %d => cmd = { 0x%02x 0x%02x%02x%02x, %02x",
		     buf + actual, chunk_len, cmd[0], cmd[1], cmd[2], cmd[3], i);

		ret = spi_flash_cmd_write(flash->spi, cmd, i,
					  NULL, 0);
					  
	  if (ret < 0) {
			debug("SF: gigadevice page programming executed timed out\n");
			break;
		}
		
		ret = gigadevice_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret < 0) {
			debug("SF: gigadevice page programming timed out\n");
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

	debug("SF: gigadevice: Successfully programmed %u bytes @ 0x%x\n",
	      len, offset);

	spi_release_bus(flash->spi);
	return ret;
}

int gigadevice_erase(struct spi_flash *flash, u32 offset, size_t len, int verbose)
{
	struct gigadevice_spi_flash *gd = to_gigadevice_spi_flash(flash);
	unsigned long sector_size;
	size_t actual;
	int ret;
	u8 cmd[4];
	size_t actual_pre = 0, v = 0;

	/*
	 * This function currently uses sector erase only.
	 * probably speed things up by using bulk erase
	 * when possible.
	 */
	sector_size = gd->params->page_size * gd->params->pages_per_sector
			* gd->params->sectors_per_block;

	if (offset % sector_size || len % sector_size) {
		debug("SF: Erase offset/length not multiple of sector size\n");
		return -1;
	}

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
	
	cmd[0] = CMD_GD5FXX_SF;
	cmd[1] = CMD_GD5FXX_PROTECTION;
	ret = spi_flash_cmd_write(flash->spi, cmd, 3, NULL, 0);
	
	ret = 0;
	cmd[0] = CMD_GD5FXX_BE;
	for (actual = 0; actual < len; actual += sector_size, offset += sector_size) {
		int i = 1;

		if (verbose && (actual == 0 || actual - actual_pre >= v))
		{
			print_progress(actual, len);
			actual_pre = actual;
		}
		cmd[i++] = 0;
		cmd[i++] = offset >> 19;
		cmd[i++] = offset >> 11;

		ret = spi_flash_cmd(flash->spi, CMD_GD5FXX_WREN, NULL, 0);
		if (ret < 0) {
			debug("SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, i, NULL, 0);
		if (ret < 0) {
			debug("SF: gigadevice page erase failed\n");
			break;
		}

		ret = gigadevice_wait_ready(flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
		if (ret < 0) {
			debug("SF: gigadevice page erase timed out\n");
			break;
		}
	}
	if (verbose)
	{
		print_progress(actual, len);
		printf("\n");
	}

	debug("SF: gigadevice: Successfully erased %u bytes @ 0x%x\n",
	      len, offset - len);

	spi_release_bus(flash->spi);
	return ret;
}

struct spi_flash *spi_flash_probe_gigadevice(struct spi_slave *spi, u8 *idcode)
{
	const struct gigadevice_spi_flash_params *params;
	struct gigadevice_spi_flash *gd;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;
  
	for (i = 0; i < ARRAY_SIZE(gigadevice_spi_flash_table); i++) {
		params = &gigadevice_spi_flash_table[i];
		if (params->idcode == id)
			break;
	}
  
	if (i == ARRAY_SIZE(gigadevice_spi_flash_table)) {
		debug("SF: Unsupported gigadevice ID %04x\n", id);
		return NULL;
	}

	gd = malloc(sizeof(*gd));
	if (!gd) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	gd->params = params;
	gd->flash.spi = spi;
	gd->flash.name = params->name;

	gd->flash.write = gigadevice_write;
	gd->flash.erase = gigadevice_erase;
	gd->flash.read = gigadevice_read_fast;
	gd->flash.size = params->page_size * params->pages_per_sector
	    * params->sectors_per_block * params->nr_blocks;
	    
	printf("SF: Detected %s with page size %u, total ",
	       params->name, params->page_size);
	print_size(gd->flash.size, "\n");

	return &gd->flash;
}
