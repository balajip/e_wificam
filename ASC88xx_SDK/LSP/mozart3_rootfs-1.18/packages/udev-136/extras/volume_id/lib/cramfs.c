/*
 * volume_id - reads filesystem label and uuid
 *
 * Copyright (C) 2004 Kay Sievers <kay.sievers@vrfy.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "libvolume_id.h"
#include "libvolume_id-private.h"

struct cramfs_super {
	uint8_t		magic[4];
	uint32_t	size;
	uint32_t	flags;
	uint32_t	future;
	uint8_t		signature[16];
	struct cramfs_info {
		uint32_t	crc;
		uint32_t	edition;
		uint32_t	blocks;
		uint32_t	files;
	} PACKED info;
	uint8_t		name[16];
} PACKED;

int volume_id_probe_cramfs(struct volume_id *id, uint64_t off, uint64_t size)
{
	struct cramfs_super *cs;

	info("probing at offset 0x%" PRIx64 "\n", off);

	cs = (struct cramfs_super *) volume_id_get_buffer(id, off, 0x200);
	if (cs == NULL)
		return -1;

	if (memcmp(cs->magic, "\x45\x3d\xcd\x28", 4) == 0 || memcmp(cs->magic, "\x28\xcd\x3d\x45", 4) == 0) {
		volume_id_set_label_raw(id, cs->name, 16);
		volume_id_set_label_string(id, cs->name, 16);

		volume_id_set_usage(id, VOLUME_ID_FILESYSTEM);
		id->type = "cramfs";
		return 0;
	}

	return -1;
}
