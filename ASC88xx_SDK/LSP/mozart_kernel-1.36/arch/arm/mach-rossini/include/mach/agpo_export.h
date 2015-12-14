/*
 * Copyright (C) 2013  VN Inc.
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

#ifndef    __ROSSINI_AGPO_EXPORT_H
#define    __ROSSINI_AGPO_EXPORT_H

#define AGPO_MAGIC		0xe0
#define AGPO_ENABLE		_IO(AGPO_MAGIC, 0)
#define AGPO_READ_STATUS	_IOR(AGPO_MAGIC, 1, int)
#define AGPO_INTR_MASK		_IO(AGPO_MAGIC, 2)
#define AGPO_INTR_UNMASK	_IO(AGPO_MAGIC, 3)
#define AGPO_OUTPUT_ENABLE	_IO(AGPO_MAGIC, 4)
#define AGPO_OUTPUT_DISABLE	_IO(AGPO_MAGIC, 5)
#define AGPO_DEFAULT_VAL	_IOW(AGPO_MAGIC, 6, int)
#define AGPO_MODE		_IOW(AGPO_MAGIC, 7, int)
#define AGPO_SETUP_PATTERN	_IOW(AGPO_MAGIC, 8, int)
#define AGPO_SETUP_SEND		_IOW(AGPO_MAGIC, 9, int)
#define AGPO_DISABLE		_IO(AGPO_MAGIC, 10)
#define	AGPO_MULTI_ENABLE	_IOW(AGPO_MAGIC, 11, int)
#define	AGPO_MULTI_DISABLE	_IOW(AGPO_MAGIC, 12, int)
#define AGPO_MAXNR		13
/* powerup default direction is IN */
#define AGPO_FLAG_I2C_MODE	0x00000001


typedef struct
{
	unsigned long	dwFlag;
	unsigned long	dwPattern_0;
	unsigned long	dwPattern_1;
	unsigned long	dwPattern_2;
	unsigned long	dwPattern_3;
	unsigned long	dwPeriod;
	unsigned long	dwDataLength;
	unsigned long	dwIntvRepeat;
}AGPO_Info;
#endif
/* __ROSSINI_AGPO_EXPORT_H */
