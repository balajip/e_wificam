/*
 * Copyright (C) 2007-2013  VN Inc.
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

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>
#include <asm/sizes.h>

#include "spi_flash_internal.h"
#include <asm/arch/platform.h>


/*-----------------------------------
 *      VPL SPI Flash Zone
 *-----------------------------------
 */
struct vpl_spi_flash_params {
	u16 idcode1;
	u16 idcode2;
	u16 page_size;
	u16 pages_per_sector;
	u16 nr_sectors;
	const char *name;
	struct spi_flash * array[SPI_FLASH_NUMBER];
};


struct vpl_spi_flash {
	struct spi_flash flash;
	const struct vpl_spi_flash_params *params;
};

extern int vpl_spi_flash_erase(struct spi_flash *flash, u32 offset, size_t len, int quiet);
extern inline struct vpl_spi_flash *to_vpl_spi_flash(struct spi_flash *flash) ;
extern int vpl_spi_flash_writebuffer(struct spi_flash *flash, u32 offset,	size_t len, const void *buf, int quiet);
extern int vpl_spi_flash_read(struct spi_flash *flash, u32 offset, size_t len, void *buf);



inline struct vpl_spi_flash *to_vpl_spi_flash(struct spi_flash
								 *flash)
{
	return container_of(flash, struct vpl_spi_flash, flash);
}

int
vpl_spi_flash_writebuffer(struct spi_flash *flash, u32 offset, size_t len, const void *buf, int verbose)
{
	struct vpl_spi_flash *   vs = to_vpl_spi_flash(flash);
	int   i, ret = len;


	for (i = 0; i < SPI_FLASH_NUMBER && len && vs->params->array[i]; i++)
	{
		size_t wrlen = 0;

		if (offset < vs->params->array[i]->size)
		{
			wrlen = (wrlen = vs->params->array[i]->size - offset) <= len ? wrlen : len;
			ret =spi_flash_write(vs->params->array[i], offset, wrlen, buf, verbose);
			if (ret)
				return ret;
			buf += wrlen;
			len -= wrlen;
			offset = 0;
		}
		else
			offset -= vs->params->array[i]->size;
	}

	return ret ;
}

int
vpl_spi_flash_read(struct spi_flash *flash, u32 offset, size_t len, void *buf)
{
	struct vpl_spi_flash *   vs = to_vpl_spi_flash(flash);
	int   i, ret = len;


	for (i = 0; i < SPI_FLASH_NUMBER && len && vs->params->array[i]; i++)
	{
		size_t rdlen = 0;

		if (offset < vs->params->array[i]->size)
		{
			rdlen = (rdlen = vs->params->array[i]->size - offset) <= len ? rdlen : len;
			len -= rdlen;

#if 0
			ret =spi_flash_read(vs->params->array[i], offset, rdlen, buf);
#else
			while (rdlen) /* max. 0xffff SPI data frames */
			{
				size_t s = rdlen < SZ_64K ? rdlen : SZ_64K; /* for 8-bit width of dw_spi */
				ret =spi_flash_read(vs->params->array[i], offset, s, buf);
				if (ret)
					return ret;
				offset += s;
				buf += s;
				rdlen -= s;
			};
#endif

			offset = 0;
		}
		else
			offset -= vs->params->array[i]->size;
	}

	return ret ;
}

int
vpl_spi_flash_erase(struct spi_flash *flash, u32 offset, size_t len, int verbose)
{
	struct vpl_spi_flash *   vs = to_vpl_spi_flash(flash);
	int   i, ret = len;


	for (i = 0; i < SPI_FLASH_NUMBER && len && vs->params->array[i]; i++)
	{
		size_t erlen = 0;

		if (offset < vs->params->array[i]->size)
		{
			erlen = (erlen = vs->params->array[i]->size - offset) <= len ? erlen : len;
			ret =spi_flash_erase(vs->params->array[i], offset, erlen, verbose);
			if (ret)
				return ret;
			offset = 0;
			len -= erlen;
		}
		else
			offset -= vs->params->array[i]->size;
	}

	return ret ;
}

extern inline void assign_config_env_sector_size_spiflash( unsigned long val )  ;
struct spi_flash *spi_flash_probe_vpl(struct spi_slave *spi, u8 *idcode)
{
	struct vpl_spi_flash_params *params;
	struct vpl_spi_flash *vpl_flash;
	struct spi_flash *   new;
	int   cs, i, j;


	params = malloc(sizeof(struct vpl_spi_flash_params)) ;

	for (cs = i = 0, j = SPI_FLASH_NUMBER - 1; cs < SPI_FLASH_NUMBER; cs++)
	{
		new = spi_flash_probe(CONFIG_SPI_FLASH_BUS, cs, CONFIG_SF_DEFAULT_SPEED,
		                      CONFIG_DEFAULT_SPI_MODE);
		if (!new)
		{
			params->array[j--] = NULL;
			continue;
		}
		params->array[i++] = new;
	}

	if (i == 0)
		goto err_malloc;

	vpl_flash = malloc(sizeof(struct vpl_spi_flash));
	if (!vpl_flash)
	{
		printf("SF: Failed to allocate memory\n");
		goto err_malloc;
	}

	vpl_flash->params = params;
	vpl_flash->flash.spi = spi;
	vpl_flash->flash.name = params->name = "VPL Virtual Flash";

	vpl_flash->flash.write = vpl_spi_flash_writebuffer;
	vpl_flash->flash.erase = vpl_spi_flash_erase;
	vpl_flash->flash.read = vpl_spi_flash_read;

	vpl_flash->flash.size = 0;
	for (i = 0; i < SPI_FLASH_NUMBER && params->array[i]; i++)
		vpl_flash->flash.size += ((struct spi_flash *)params->array[i])->size;
	printf("  There are %d SPI FLASH, total ", i);
	print_size(vpl_flash->flash.size, "\n");

	//assign_config_env_sector_size_spiflash( params->page_size * params->pages_per_sector ) ;
#ifndef CONFIG_QUICK_TEST
	//printf("SPI FLASH : Detected %s with page size %u, total %u bytes\n", params->name, params->page_size, vpl_flash->flash.size);
#endif

	return &vpl_flash->flash;

err_malloc:
	/* ToDo: device-specific flash struct is not been freed !!! */
	while (i >= 0)
		spi_flash_free(params->array[i--]);
	free(params);
	return NULL;
}
