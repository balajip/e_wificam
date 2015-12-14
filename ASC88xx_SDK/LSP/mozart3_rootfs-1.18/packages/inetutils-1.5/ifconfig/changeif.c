/* changeif.c -- change the configuration of a network interface

   Copyright (C) 2001, 2002, 2007 Free Software Foundation, Inc.

   Written by Marcus Brinkmann.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 3
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "ifconfig.h"

#define SIOCSIF(type, addr)						\
  int err = 0;								\
  struct sockaddr_in *sin = (struct sockaddr_in *) &ifr->ifr_addr;	\
									\
  sin->sin_family = AF_INET;						\
  err = inet_aton (addr, &sin->sin_addr);				\
  if (!err)								\
    {									\
      fprintf (stderr, "%s: `%s' is not a valid address\n",		\
	       program_name, addr);					\
      return -1;							\
    }									\
  err = ioctl (sfd, SIOCSIF##type, ifr);				\
  if (err < 0)								\
    {									\
      fprintf (stderr, "%s: %s failed: %s\n", program_name,		\
               "SIOCSIF" #type, strerror (errno));			\
      return -1;							\
    }

extern void herror (const char *pfx);

/* Set address of interface in IFR. Destroys union in IFR, but leaves
   ifr_name intact.  ADDRESS may be an IP number or a hostname that
   can be resolved.  */
int
set_address (int sfd, struct ifreq *ifr, char *address)
{
#ifndef SIOCSIFADDR
  fprintf (stderr, "%s: Don't know how to set an interface address on this system.\n",
	  program_name);
  return -1;
#else
  char *addr;
  struct hostent *host = gethostbyname (address);

  if (! host)
    {
      fprintf (stderr, "%s: can not resolve `%s': ", program_name,
               address);
      herror (NULL);
      return -1;
    }
  if (host->h_addrtype != AF_INET)
    {
      fprintf (stderr, "%s: `%s' refers to an unknown address type", program_name,
	       address);
      return -1;
    }

  addr = inet_ntoa (* ((struct in_addr *) host->h_addr));

  {
    SIOCSIF(ADDR, addr)
      if (verbose)
	printf ("Set interface address of `%s' to %s.\n",
		ifr->ifr_name, inet_ntoa (sin->sin_addr));
  }
  return 0;
#endif
}

int
set_netmask (int sfd, struct ifreq *ifr, char *netmask)
{
#ifndef SIOCSIFNETMASK
  printf ("%s: Don't know how to set an interface netmask on this system.\n",
	  program_name);
  return -1;
#else

  SIOCSIF(NETMASK, netmask)
  if (verbose)
    printf ("Set interface netmask of `%s' to %s.\n",
	    ifr->ifr_name, inet_ntoa (sin->sin_addr));
  return 0;
#endif
}

int
set_dstaddr (int sfd, struct ifreq *ifr, char *dstaddr)
{
#ifndef SIOCSIFDSTADDR
  printf ("%s: Don't know how to set an interface peer address on this system.\n",
	  program_name);
  return -1;
#else
  SIOCSIF(DSTADDR, dstaddr)
  if (verbose)
    printf ("Set interface peer address of `%s' to %s.\n",
	    ifr->ifr_name, inet_ntoa (sin->sin_addr));
  return 0;
#endif
}

int
set_brdaddr (int sfd, struct ifreq *ifr, char *brdaddr)
{
#ifndef SIOCSIFBRDADDR
  printf ("%s: Don't know how to set an interface broadcast address on this system.\n",
	  program_name);
  return -1;
#else
  SIOCSIF(BRDADDR, brdaddr)
  if (verbose)
    printf ("Set interface broadcast address of `%s' to %s.\n",
	    ifr->ifr_name, inet_ntoa (sin->sin_addr));
  return 0;
#endif
}

int
set_mtu (int sfd, struct ifreq *ifr, int mtu)
{
#ifndef SIOCSIFMTU
  printf ("%s: Don't know how to set the interface mtu on this system.\n",
	  program_name);
  return -1;
#else
  int err = 0;

  ifr->ifr_mtu = mtu;
  err = ioctl (sfd, SIOCSIFMTU, ifr);
  if (err < 0)
    {
      fprintf (stderr, "%s: SIOCSIFMTU failed: %s\n",
	       program_name, strerror (errno));
      return -1;
    }
  if (verbose)
    printf ("Set mtu value of `%s' to `%i'.\n",
	    ifr->ifr_name, ifr->ifr_mtu);
  return 0;
#endif
}

int
set_metric (int sfd, struct ifreq *ifr, int metric)
{
#ifndef SIOCSIFMETRIC
  printf ("%s: Don't know how to set the interface metric on this system.\n",
	  program_name);
  return -1;
#else
  int err = 0;

  ifr->ifr_metric = metric;
  err = ioctl (sfd, SIOCSIFMETRIC, ifr);
  if (err < 0)
    {
      fprintf (stderr, "%s: SIOCSIFMETRIC failed: %s\n",
	       program_name, strerror (errno));
      return -1;
    }
  if (verbose)
    printf ("Set metric value of `%s' to `%i'.\n",
	    ifr->ifr_name, ifr->ifr_metric);
  return 0;
#endif
}

int
configure_if (int sfd, struct ifconfig *ifp)
{
  int err = 0;
  struct ifreq ifr;

  memset (&ifr, 0, sizeof (ifr));
  strncpy (ifr.ifr_name, ifp->name, IFNAMSIZ);
  ifr.ifr_name[IFNAMSIZ - 1] = '\0';

  if (ifp->valid & IF_VALID_ADDR)
    err = set_address (sfd, &ifr, ifp->address);
  if (!err && ifp->valid & IF_VALID_NETMASK)
    err = set_netmask (sfd, &ifr, ifp->netmask);
  if (!err && ifp->valid & IF_VALID_DSTADDR)
    err = set_dstaddr (sfd, &ifr, ifp->dstaddr);
  if (!err && ifp->valid & IF_VALID_BRDADDR)
    err = set_brdaddr (sfd, &ifr, ifp->brdaddr);
  if (!err && ifp->valid & IF_VALID_MTU)
    err = set_mtu (sfd, &ifr, ifp->mtu);
  if (!err && ifp->valid & IF_VALID_METRIC)
    err = set_metric (sfd, &ifr, ifp->metric);
  if (!err && ifp->valid & IF_VALID_SYSTEM)
    err = system_configure (sfd, &ifr, ifp->system);
  if (!err && ifp->valid & IF_VALID_FORMAT)
    print_interface (sfd, ifp->name, &ifr, ifp->format);
  return err;
}
