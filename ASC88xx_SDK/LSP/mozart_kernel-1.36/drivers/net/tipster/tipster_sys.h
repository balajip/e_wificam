/* Tipster PCI/Ethernet test chip driver for OS Linux
 *
 * System-dependent definitions
 *
 * 1 Jun 1999 - G.Kazakov.
 */

#ifndef TIPSTER_SYS_INCLUDED
#define TIPSTER_SYS_INCLUDED

#include <asm/types.h>
#include <asm/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#define TipsterSysDelay( usec ) udelay( usec )

#endif /* TIPSTER_SYS_INCLUDED */

