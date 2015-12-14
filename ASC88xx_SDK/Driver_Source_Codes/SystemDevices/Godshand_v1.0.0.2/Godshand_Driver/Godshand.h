/*
 * Godshand
 * Driver for Godshand.
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
/* ======================================================================== */
#ifndef __GODSHAND_H__
#define __GODSHAND_H__

#ifdef _DEBUG__
#define PDEBUG printf
#else
#define PDEBUG(x...)
#endif //_DEBUG__

/* ======================================================================== */
#include "typedef.h"
#include "vivo_codec.h"
//#include <linux/unistd.h>

/* ======================================================================== */
#define GODSHAND_IOC_MAGIC		0xe0

#define GODSHAND_IOC_RESET		_IO (GODSHAND_IOC_MAGIC, 0)
#define GODSHAND_IOC_CLEAR		_IO (GODSHAND_IOC_MAGIC, 1)
#define GODSHAND_IOC_READ		_IOR (GODSHAND_IOC_MAGIC, 2, int)
#define GODSHAND_IOC_READ_AREA	_IOR (GODSHAND_IOC_MAGIC, 3, int)
#define GODSHAND_IOC_WRITE		_IOW (GODSHAND_IOC_MAGIC, 4, int)
#define GODSHAND_IOC_WRITE_AREA	_IOW (GODSHAND_IOC_MAGIC, 5, int)

#define GODSHAND_IOC_MAXNUM		5

/* ======================================================================== */
#define GODSHAND_FLAG_READ		0X01
#define GODSHAND_FLAG_WRITE		0x02
#define GODSHAND_FLAG_ADDRESS	0x04
#define GODSHAND_FLAG_DATA		0x08
#define GODSHAND_FLAG_AREA		0x10
#define GODSHAND_FLAG_BYTE		0x20
#define GODSHAND_FLAG_LOOP		0x40

/* ======================================================================== */
#define ERR_RETURN_VALUE	32

/* ======================================================================== */
typedef struct 
{
	DWORD	dwAddress;
	DWORD	dwData;
	DWORD	dwSize;
	DWORD	dwFlag;
	void *	pDataBuffer;
}GODSHAND_Info;

/* ======================================================================== */
#endif //__GODSHAND_H__
