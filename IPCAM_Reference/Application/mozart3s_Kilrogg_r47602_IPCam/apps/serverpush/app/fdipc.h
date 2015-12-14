/* Transfer of open file descriptors by AF_UNIX datagram sockets.

   Copyright (C) 2004  Axis Communications AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _FDIPC_H
#define _FDIPC_H

#include <sys/uio.h>

/* Return an open socket file descriptor for outgoing data identified by path,
   or -1 on error. */

extern int fdipc_client_socket(const char *path);

/* Send the file descriptor fd and n_iov buffers stored in iov[] on the local
   connected socket lfd. Return the number of byte sent or -1 on error. */

extern int fdipc_send(const int lfd, struct iovec iov[], const int n_iov,
	       const int fd);

/* Return an open socket file descriptor identified by path with permissions
   mode for incoming data, or -1 on error. */

extern int fdipc_server_socket(const char *path, const mode_t mode);

/* Receive the file descriptor fd and at most n_buf byte stored in buf
   on the local connected socket lfd. Return the number of byte received
   or -1 on error. */

extern int fdipc_recv(const int lfd, unsigned char *buf, const int n_buf,
		      int *fd);

#endif /* _FDIPC_H */
