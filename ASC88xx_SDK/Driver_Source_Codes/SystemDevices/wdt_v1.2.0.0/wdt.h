/*
 * wdt
 * Driver for watchdog timer.
 *
 * Copyright (C) 2007-2011  VN Inc.
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

#ifndef __WDT_H__
#define __WDT_H__

#define WDT_DRIVER_VERSION "1.2.0.0"

#define WATCHDOG_IOCTL_BASE 'W'

#define WDIOC_KEEPALIVE         _IOR(WATCHDOG_IOCTL_BASE, 0, int)
#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 2, int)
#define WDIOC_ENABLE            _IOR(WATCHDOG_IOCTL_BASE, 3, int)
#define WDIOC_DISABLE           _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_SETMATCH          _IOR(WATCHDOG_IOCTL_BASE, 5, int)

#endif /* __WDT_H__ */
