/* Copyright (C) 1997, 1998, 1999, 2000, 2001, 2007 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Solaris at least earlier 2.6 and before does not include
   the ioctl definitions if BSD_COMP is not set.  */
#if defined(__svr4__)
# define BSD_COMP 1
#endif

#include <sys/ioctl.h>
#include <net/if.h>

#include <ifconfig.h>

unsigned int
if_nametoindex (const char *ifname)
{
  int result = 0;
#ifdef SIOCGIFINDEX
  {
    int fd = socket (AF_INET, SOCK_DGRAM, 0);
    if (fd >= 0)
      {
	struct ifreq ifr;
	strncpy (ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	result = ioctl (fd, SIOCGIFINDEX, &ifr);
	close (fd);
	if (result == 0)
	  return ifr.ifr_index;
      }
  }
#endif
  {
    struct if_nameindex *idx;
    idx = if_nameindex ();
    if (idx != NULL)
      {
	struct if_nameindex *p;
	for (p = idx; p->if_index || p->if_name; ++p)
	  {
	    if (strncmp (p->if_name, ifname, IFNAMSIZ) == 0)
	      {
		result = p->if_index;
		break;
	      }
	  }
	if_freenameindex (idx);
      }
    return result;
  }
}

void
if_freenameindex (struct if_nameindex *ifn)
{
  struct if_nameindex *ptr = ifn;
  if (!ifn) return;
  while (ptr->if_name || ptr->if_index)
    {
      if (ptr->if_name)
	free (ptr->if_name);
      ++ptr;
    }
  free (ifn);
}

struct if_nameindex *
if_nameindex (void)
{
#if defined(SIOCGIFCONF)
  int fd = socket (AF_INET, SOCK_DGRAM, 0);
  struct ifconf ifc;
  unsigned int i = 0;
  int rq_len, last_len;
  struct if_nameindex *idx = NULL;
  struct ifreq *ifr, *end, *cur;

  if (fd < 0)
    return NULL;

  /* A first estimate.  */
  rq_len = 4 * sizeof (struct ifreq);

  ifc.ifc_buf = NULL;
  ifc.ifc_len = 0;
  /* Read all the interfaces out of the kernel.  */
  do
    {
      last_len = ifc.ifc_len;
      ifc.ifc_buf = alloca (ifc.ifc_len = rq_len);
      if (ifc.ifc_buf == NULL || ioctl (fd, SIOCGIFCONF, &ifc) < 0)
	{
	  close (fd);
	  return NULL;
	}
      rq_len *= 2;
    }
  while (ifc.ifc_len != last_len);

  i = 0;
  ifr = (struct ifreq *)ifc.ifc_req;
  end = (struct ifreq *)((caddr_t)ifr + ifc.ifc_len);
  while (ifr < end)
    {
      int len;
#ifdef HAVE_SOCKADDR_SA_LEN
#undef MAX
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
      len = MAX(sizeof (struct sockaddr), ifr->ifr_addr.sa_len);
#else
      len = sizeof (struct sockaddr);
#endif

      cur = ifr;

      /* Step along the array by the size of the current structure */
      ifr = (struct ifreq *)((caddr_t)ifr + len + IFNAMSIZ);

      /* We ignore the other families .. OK ?  */
      if (cur->ifr_addr.sa_family != AF_INET)
	continue;

      /* Make Room safely.  */
      {
	struct if_nameindex *tidx = NULL;
	tidx = realloc (idx, (i + 1) * sizeof(*idx));
	if (tidx == NULL)
	  {
	    if_freenameindex (idx);
	    close (fd);
	    errno = ENOBUFS;
	    return NULL;
	  }
	idx = tidx;
      }

      /* FIXME: We did not deal with duplicates or interface aliases.  */

      idx[i].if_name = strdup (cur->ifr_name);
      if (idx[i].if_name == NULL)
	{
	  if_freenameindex (idx);
	  close (fd);
	  errno = ENOBUFS;
	  return NULL;
	}

# if defined(SIOCGIFINDEX)
      if (ioctl (fd, SIOCGIFINDEX, cur) >= 0)
	idx[i].if_index = cur->ifr_index;
      else
# endif
	idx[i].if_index =  i + 1;
      i++;	
    }

  /* Terminate the array with an empty solt.  */
  {
    struct if_nameindex *tidx = NULL;
    tidx = realloc (idx, (i + 1) * sizeof(*idx));
    if (tidx == NULL)
      {
	if_freenameindex (idx);
	close (fd);
	errno = ENOBUFS;
	return NULL;
      }
    idx = tidx;
  }
  idx[i].if_index = 0;
  idx[i].if_name = NULL;

  close (fd);
  return idx;

#else
  errno = ENOSYS;
  return NULL;
#endif
}

char *
if_indextoname (unsigned int ifindex, char *ifname)
{
#if defined SIOCGIFNAME
  {
    int fd = socket (AF_INET, SOCK_DGRAM, 0);
    if (fd >= 0)
      {
	struct ifreq ifr;

	ifr.ifr_index = ifindex;
	if (ioctl (fd, SIOCGIFNAME, &ifr) == 0)
	  {
	    close (fd);
	    strncpy (ifname, ifr.ifr_name, IFNAMSIZ);
	    ifname[IFNAMSIZ - 1] = '\0';
	    return ifname;
	  }
	close (fd);
      }
  }
#else
  {
    struct if_nameindex *idx;
    char *result = NULL;

    idx = if_nameindex ();

    if (idx != NULL)
      {
	struct if_nameindex *p;
	for (p = idx; p->if_index || p->if_name; ++p)
	  {
	    if (p->if_index == ifindex)
	      {
		result = strncpy (ifname, p->if_name, IFNAMSIZ);
		result[IFNAMSIZ - 1] = '\0';
		break;
	      }
	  }
	if_freenameindex (idx);
      }
    return result;
  }
#endif
}
