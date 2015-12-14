/* Copyright (C) 1998, 2001, 2005, 2007 Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
/*#include <netinet/ip_icmp.h> -- deliberately not including this */
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <icmp.h>
#include "ping.h"

static void _ping_freebuf (PING *p);
static int _ping_setbuf (PING *p);
static size_t _ping_packetsize (PING *p);

size_t
_ping_packetsize (PING *p)
{
  if (p->ping_type == ICMP_TIMESTAMP
      || p->ping_type == ICMP_TIMESTAMPREPLY)
    return 20;
  return 8 + p->ping_datalen;
}

PING *
ping_init (int type, int ident)
{
  int fd;
  struct protoent *proto;
  PING *p;

  /* Initialize raw ICMP socket */
  proto = getprotobyname ("icmp");
  if (!proto)
    {
      fprintf (stderr, "ping: unknown protocol icmp.\n");
      return NULL;
    }

  fd = socket (AF_INET, SOCK_RAW, proto->p_proto);
  if (fd < 0)
    {
      if (errno == EPERM)
	fprintf (stderr, "ping: ping must run as root\n");
      return NULL;
    }

  /* Allocate PING structure and initialize it to default values */
  p = malloc (sizeof (*p));
  if (!p)
    {
      close (fd);
      return p;
    }

  memset (p, 0, sizeof (*p));

  p->ping_fd = fd;
  p->ping_type = type;
  p->ping_count = 0;
  p->ping_interval = PING_DEFAULT_INTERVAL;
  p->ping_datalen  = sizeof (icmphdr_t);
  /* Make sure we use only 16 bits in this field, id for icmp is a u_short.  */
  p->ping_ident = ident & 0xFFFF;
  p->ping_cktab_size = PING_CKTABSIZE;
  return p;
}

void
ping_set_type (PING *p, int type)
{
  p->ping_type = type;
}

void
ping_set_datalen (PING *p, size_t len)
{
  _ping_freebuf (p);
  p->ping_datalen = len;
}

void
_ping_freebuf (PING *p)
{
  if (p->ping_buffer)
    {
      free (p->ping_buffer);
      p->ping_buffer = NULL;
    }
}

int
_ping_setbuf (PING *p)
{
  if (!p->ping_buffer)
    {
      p->ping_buffer = malloc (_PING_BUFLEN (p));
      if (!p->ping_buffer)
	return -1;
    }
  if (!p->ping_cktab)
    {
      p->ping_cktab = malloc (p->ping_cktab_size);
      if (!p->ping_cktab)
        return -1;
      memset (p->ping_cktab, 0, p->ping_cktab_size);
    }
  return 0;
}

int
ping_set_data (PING *p, void *data, size_t off, size_t len)
{
  icmphdr_t *icmp;

  if (_ping_setbuf (p))
    return -1;
  if (p->ping_datalen < off + len)
    return -1;
  icmp = (icmphdr_t *)p->ping_buffer;
  memcpy (icmp->icmp_data + off, data, len);
  return 0;
}

int
ping_xmit (PING *p)
{
  int i, buflen;

  if (_ping_setbuf (p))
    return -1;

  buflen = _ping_packetsize (p);

  /* Mark sequence number as sent */
  _PING_CLR (p, p->ping_num_xmit % p->ping_cktab_size);

  /* Encode ICMP header */
  switch (p->ping_type)
    {
    case ICMP_ECHO:
      icmp_echo_encode (p->ping_buffer, buflen, p->ping_ident,
			p->ping_num_xmit);
      break;
      
    case ICMP_TIMESTAMP:
      icmp_timestamp_encode (p->ping_buffer, buflen, p->ping_ident,
			     p->ping_num_xmit);
      break;
      
    case ICMP_ADDRESS:
      icmp_address_encode (p->ping_buffer, buflen, p->ping_ident,
			   p->ping_num_xmit);
      break;
      
    default:
      icmp_generic_encode (p->ping_buffer, buflen, p->ping_type, p->ping_ident,
			   p->ping_num_xmit);
      break;
    }
  
  i = sendto (p->ping_fd, (char *)p->ping_buffer, buflen, 0,
	      (struct sockaddr*) &p->ping_dest,
	      sizeof (struct sockaddr_in));
  if (i < 0)
    perror ("ping: sendto");
  else
    {
      p->ping_num_xmit++;
      if (i != buflen)
	printf ("ping: wrote %s %d chars, ret=%d\n",
		p->ping_hostname, buflen, i);
    }
  return 0;
}

static int
my_echo_reply (PING *p, icmphdr_t *icmp)
{
  struct ip *orig_ip = &icmp->icmp_ip;
  icmphdr_t *orig_icmp = (icmphdr_t *)(orig_ip + 1);
  
  return (orig_ip->ip_dst.s_addr == p->ping_dest.sin_addr.s_addr
	  && orig_ip->ip_p == IPPROTO_ICMP
	  && orig_icmp->icmp_type == ICMP_ECHO
	  && orig_icmp->icmp_id == p->ping_ident);
}

int
ping_recv (PING *p)
{
  int fromlen = sizeof (p->ping_from);
  int n, rc;
  icmphdr_t *icmp;
  struct ip *ip;
  int dupflag;

  n = recvfrom (p->ping_fd,
		(char *)p->ping_buffer, _PING_BUFLEN (p), 0,
		(struct sockaddr *)&p->ping_from, &fromlen);
  if (n < 0)
    return -1;
  
  rc = icmp_generic_decode (p->ping_buffer, n, &ip, &icmp);
  if (rc < 0)
    {
      /*FIXME: conditional*/
      fprintf (stderr, "packet too short (%d bytes) from %s\n", n,
	       inet_ntoa (p->ping_from.sin_addr));
      return -1;
    }
  
  switch (icmp->icmp_type)
    {
    case ICMP_ECHOREPLY:
    case ICMP_TIMESTAMPREPLY:
    case ICMP_ADDRESSREPLY:
      /*    case ICMP_ROUTERADV:*/
      
      if (icmp->icmp_id != p->ping_ident)
	return -1;
      
      if (rc)
	fprintf (stderr, "checksum mismatch from %s\n",
		 inet_ntoa (p->ping_from.sin_addr));
      
      p->ping_num_recv++;
      if (_PING_TST (p, icmp->icmp_seq % p->ping_cktab_size))
	{
	  p->ping_num_rept++;
	  p->ping_num_recv--;
	  dupflag = 1;
	}
      else
	{
	  _PING_SET (p, icmp->icmp_seq % p->ping_cktab_size);
	  dupflag = 0;
	}

      if (p->ping_event)
	(*p->ping_event)(dupflag ? PEV_DUPLICATE : PEV_RESPONSE,
			 p->ping_closure,
			 &p->ping_dest,
			 &p->ping_from,
			 ip, icmp, n);
      break;

    case ICMP_ECHO:
    case ICMP_TIMESTAMP:
    case ICMP_ADDRESS:
      return -1;

    default:
      if (!my_echo_reply(p, icmp))
	return -1;
      
      if (p->ping_event)
	(*p->ping_event)(PEV_NOECHO,
			 p->ping_closure,
			 &p->ping_dest,
			 &p->ping_from,
			 ip, icmp, n);
    }
  return 0;
}

void
ping_set_event_handler (PING *ping, ping_efp pf, void *closure)
{
  ping->ping_event = pf;
  ping->ping_closure = closure;
}

void
ping_set_count (PING *ping, size_t count)
{
  ping->ping_count = count;
}

void
ping_set_sockopt (PING *ping, int opt, void *val, int valsize)
{
  setsockopt (ping->ping_fd, SOL_SOCKET, opt, (char*)&val, valsize);
}

void
ping_set_interval (PING *ping, size_t interval)
{
  ping->ping_interval = interval;
}

void
ping_set_packetsize (PING *ping, size_t size)
{
  ping->ping_datalen = size;
}

int
ping_set_dest (PING *ping, char *host)
{
  struct sockaddr_in *s_in = &ping->ping_dest;
  s_in->sin_family = AF_INET;
  if (inet_aton (host, &s_in->sin_addr))
    ping->ping_hostname = strdup (host);
  else
    {
      struct hostent *hp = gethostbyname (host);
      if (!hp)
	return 1;

      s_in->sin_family = hp->h_addrtype;
      if (hp->h_length > (int)sizeof (s_in->sin_addr))
	hp->h_length = sizeof (s_in->sin_addr);

      memcpy (&s_in->sin_addr, hp->h_addr, hp->h_length);
      ping->ping_hostname = strdup (hp->h_name);
    }
  return 0;
}

 
