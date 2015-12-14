/* system.h

   Copyright (C) 2001, 2007 Free Software Foundation, Inc.

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
   MA 02110-1301 USA. */

#ifndef IFCONFIG_SYSTEM_H
#define IFCONFIG_SYSTEM_H


/* Option parsing.  */

/* Define this if ifconfig supports parsing the remaining non-option
   arguments on the command line (see system_parse_opt_rest) to a string
   usable in the help info.  Like "  <addr> [ netmask <mask> ]" */
extern const char *system_help;

extern const char *system_help_options;

/* Additional short options.  */
#undef SYSTEM_SHORT_OPTIONS

/* Addition long options.  */
#undef SYSTEM_LONG_OPTIONS

/* Hooked into a struct ifconfig (setting the flag IF_VALID_SYSTEM),
   to store system specific configurations from the command line
   arguments.  */
/* struct system_ifconfig; */

/* Parse option OPTION, with argument OPTARG (may be NULL), and store
   it in IFP.  You may create a new struct ifconfig if appropriate,
   and store its pointer in IFP to make it the current one.
   Return 0 if option was not recognized, otherwise 1.  */
extern int system_parse_opt(struct ifconfig **ifp, char option, char *optarg);

/* Parse remaining ARGC arguments ARGV on the command line. IFP has
   the same meaning as in system_parse_opt.  (There is some
   post-processing, so you are not reliefed from setting IPF is
   appropriate.)
   Return 0 if all options were not recognized, otherwise 1.  */
extern int system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[]);


/* Output format support.  */

/* Define this if you want to set a system specific default output
   format.  Possible value is a string with the same meaning as an
   argument to the --format option, e.g. `"gnu"', or `"${name}:"'.  */
extern const char *system_default_format;

/* Define this to a list of struct format_handler items.  Add a
   trailing comma, too.  */
#undef SYSTEM_FORMAT_HANDLER


int system_configure (int sfd, struct ifreq *ifr, struct system_ifconfig *__ifs);



/* For systems which loose.  */

#ifndef HAVE_STRUCT_IFREQ_IFR_INDEX
# define ifr_index ifr_ifindex
#endif

#ifndef HAVE_STRUCT_IFREQ_IFR_NETMASK
# define ifr_netmask ifr_addr
#endif

#ifndef HAVE_STRUCT_IFREQ_IFR_BROADADDR
# define ifr_broadaddr ifr_addr
#endif



#if defined(__linux__)
# include "system/linux.h"
#elif defined(__sun__)
# include "system/solaris.h"
#elif defined(__hpux__)
# include "system/hpux.h"
#elif defined(__QNX__)
# include "system/qnx.h"
#else
#include "system/generic.h"
#endif

#endif
