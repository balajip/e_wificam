/*
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

#ifndef    __MOZART_GPIO_EXPORT_H
#define    __MOZART_GPIO_EXPORT_H

#define GPIO_MAGIC          0xd0
#define GPIO_PINDIR_OUT     _IO(GPIO_MAGIC, 0)
#define GPIO_PINDIR_IN      _IO(GPIO_MAGIC, 1)
#define GPIO_READ_DATAIN    _IOR(GPIO_MAGIC, 2, int)
#define GPIO_DATA_SET       _IO(GPIO_MAGIC, 3)
#define GPIO_DATA_CLEAR     _IO(GPIO_MAGIC, 4)
#define GPIO_INTR_ENABLE    _IO(GPIO_MAGIC, 5)
#define GPIO_INTR_DISABLE   _IO(GPIO_MAGIC, 6)
#define GPIO_INTR_MASK      _IO(GPIO_MAGIC, 7)
#define GPIO_INTR_UNMASK    _IO(GPIO_MAGIC, 8)
#define GPIO_WAIT_INTR      _IO(GPIO_MAGIC, 9)
#define GPIO_PINDIR_LOOSE   _IO(GPIO_MAGIC, 10)
#define GPIO_PINDIR_HOLD    _IO(GPIO_MAGIC, 11)
#define GPIO_INTR_EDGE      _IO(GPIO_MAGIC, 12)
#define GPIO_INTR_LEVEL     _IO(GPIO_MAGIC, 13)
#define GPIO_INTR_HACTIVE   _IO(GPIO_MAGIC, 14)
#define GPIO_INTR_LACTIVE   _IO(GPIO_MAGIC, 15)
#define GPIO_INTR_BOTHEDGE  _IO(GPIO_MAGIC, 16)
#define GPIO_INTR_SINGLEEDGE  _IO(GPIO_MAGIC, 17)
#define GPIO_MAXNR 18


#endif
/* __MOZART_GPIO_EXPORT_H */
