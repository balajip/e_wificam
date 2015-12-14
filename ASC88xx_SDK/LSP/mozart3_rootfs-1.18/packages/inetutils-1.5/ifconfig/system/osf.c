/* system/osf.c -- OSF specific code for ifconfig

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

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if STDC_HEADERS
# include <stdlib.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
#endif

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>

#include "../ifconfig.h"


/* Output format stuff.  */

const char *system_default_format "osf"


/* Argument parsing stuff.  */

const char *system_help = "\
  NAME [AF]
or
  NAME AF [ADDR [DSTADDR]] [broadcast BRDADDR]\n\
  [netmask MASK] [metric N]";

const char *system_help_options;

int
system_parse_opt(struct ifconfig **ifp, char option, char *optarg)
{
  return 0;
}

int
system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[])
{
  int i = -1;
  enum
  {
    EXPECT_NOTHING,
    EXPECT_AF,
    EXPECT_NAME,
    EXPECT_BROADCAST,
    EXPECT_NETMASK,
    EXPECT_METRIC,
  } expect = EXPECT_NAME;

  while (++i < argc)
    {
      switch (expect)
	{
	case EXPECT_NAME:
	  *ifp = parse_opt_new_ifs (argv[i]);
	  break;

	case EXPECT_AF:
	  parse_opt_set_af (*ifp, argv[i]);
	  break;

	case EXPECT_BROADCAST:
	  parse_opt_set_brdaddr (*ifp, argv[i]);
	  break;

	case EXPECT_NETMASK:
	  parse_opt_set_netmask (*ifp, argv[i]);
	  break;

	case EXPECT_METRIC:
	  parse_opt_set_metric (*ifp, argv[i]);
	  break;

	case EXPECT_NOTHING:
	  break;
	}

      if (expect != EXPECT_NOTHING)
	expect = EXPECT_NOTHING;
      else if (! strcmp (argv[i], "broadcast"))
	expect = EXPECT_BROADCAST;
      else if (! strcmp (argv[i], "netmask"))
	expect = EXPECT_NETMASK;
      else if (! strcmp (argv[i], "metric"))
	expect = EXPECT_METRIC;
      else
	{
	  /* Recognize up/down.  */
	  /* Also debug, -debug, trailers, -trailers,
	     ipdst.  */
	  if (! (*ifp->valid & IF_VALID_ADDR))
	    {
	      parse_opt_set_address (*ifp, argv[i]);
	      expect = EXPECT_AF;
	    }
	  else if (! (*ifp->valid & IF_VALID_DSTADDR))
	    parse_opt_set_dstaddr (*ifp, argv[i]);
	}
    }

  switch (expect)
    {
    case EXPECT_BROADCAST:
      fprintf (stderr, "%s: option `broadcast' requires an argument\n",
	       program_name);
      break;

    case EXPECT_NETMASK:
      fprintf (stderr, "%s: option `netmask' requires an argument\n",
	       program_name);
      break;

    case EXPECT_METRIC:
      fprintf (stderr, "%s: option `metric' requires an argument\n",
	       program_name);
      break;

    case EXPECT_NAME:
    case EXPECT_AF:
    case EXPECT_NOTHING:
      expect = EXPECT_NOTHING;
     break;
    }
  if (expect != EXPECT_NOTHING)
    usage (EXIT_FAILURE);

  return 1;
}

int
system_configure (int sfd, struct ifreq *ifr, struct system_ifconfig *ifs)
{
  return 0;
}
