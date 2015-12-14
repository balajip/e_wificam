/*
 *
 * Copyright 2007-2008 VN, Inc.  All rights reserved.
 *
 */

#ifndef __WDT_H__
#define __WDT_H__

#define WDT_DRIVER_VERSION "1.0.0.0"

#define WATCHDOG_IOCTL_BASE 'W'

#define WDIOC_KEEPALIVE     _IOR(WATCHDOG_IOCTL_BASE, 0, int)
#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 2, int)
#define WDIOC_ENABLE            _IOR(WATCHDOG_IOCTL_BASE, 3, int)
#define WDIOC_DISABLE           _IOR(WATCHDOG_IOCTL_BASE, 4, int)

#endif /* __WDT_H__ */
