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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Return an open socket file descriptor for outgoing data identified by path,
   or -1 on error. */

int fdipc_client_socket(const char *path);

/* Send the file descriptor fd and n_iov buffers stored in iov[] on the local
   connected socket lfd. Return the number of byte sent or -1 on error. */

int fdipc_send(const int lfd, struct iovec iov[], const int n_iov, const int fd);

/* Return an open socket file descriptor identified by path with permissions
   mode for incoming data, or -1 on error. */

int fdipc_server_socket(const char *path, const mode_t mode);

/* Receive the file descriptor fd and at most n_buf byte stored in buf
   on the local connected socket lfd. Return the number of byte received
   or -1 on error. */

int fdipc_recv(const int lfd, unsigned char *buf, const int n_buf, int *fd);

/* Return 0 if a datagram AF_UNIX socket identified by path may be opened
   and return -1 on error, local contains the path specification and
   fd the file descriptor of the socket. */

static int new_socket(const char *path, struct sockaddr_un *local, int *fd);

/* Return -1 and the triggering errno, close the opened socket file
   descriptor. */

static int panic_socket(int fd);

/* Return an open socket file descriptor for outgoing data identified by path,
   or -1 on error. */
int
fdipc_client_socket(const char *path)
{
  struct sockaddr_un local;
  int fd;

  if (new_socket(path, &local, &fd) < 0)
    {
      return -1;
    }

  if (connect(fd, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
      return panic_socket(fd);
    }


  return fd;
}

/* Send the file descriptor fd and n_iov buffers stored in iov[] on the local
   connected socket lfd. Return the number of byte sent or -1 on error. */
int
fdipc_send(const int lfd, struct iovec iov[], const int n_iov, const int fd)
{
  struct msghdr msg;
  struct {
    struct cmsghdr cmsg;
    int data;
  } msg_data;
  
  msg.msg_iov = &iov[0];
  msg.msg_iovlen = n_iov;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;

  msg_data.cmsg.cmsg_level = SOL_SOCKET;
  msg_data.cmsg.cmsg_type = SCM_RIGHTS;
  msg_data.cmsg.cmsg_len = CMSG_LEN(sizeof(int));
  msg_data.data = fd;

  msg.msg_control = (caddr_t)&msg_data;
  msg.msg_controllen = sizeof(msg_data);

  return sendmsg(lfd, &msg, 0);
}

/* Return an open socket file descriptor identified by path with permissions
   mode for incoming data, or -1 on error. */
int
fdipc_server_socket(const char *path, const mode_t mode)
{
  struct sockaddr_un local;
  int fd;
unlink(path);
  if (new_socket(path, &local, &fd) < 0)
    {
      return -1;
    }

  if (bind(fd, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
      return panic_socket(fd);
    }

  if (chmod(path, mode) < 0)
    {
      return panic_socket(fd);
    }

  return fd;
}

/* Receive the file descriptor fd and at most n_buf byte stored in buf
   on the local connected socket lfd. Return the number of byte received
   or -1 on error. */
int
fdipc_recv(const int lfd, unsigned char *buf, const int n_buf, int *fd)
{
  struct iovec iov;
  struct msghdr msg;
  char cmsgbuf[CMSG_SPACE(sizeof(int))];
  struct cmsghdr *cmsgptr;
  int n_read;

  *fd = -1;

  iov.iov_base = buf;
  iov.iov_len = n_buf;

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = (caddr_t)cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);
  cmsgptr = CMSG_FIRSTHDR(&msg);

  cmsgptr->cmsg_level = SOL_SOCKET;
  cmsgptr->cmsg_type = SCM_RIGHTS;
  cmsgptr->cmsg_len = CMSG_LEN(sizeof(int));
  *(int *)CMSG_DATA(cmsgptr) = -1;

  n_read = recvmsg(lfd, &msg, 0);
  if (n_read <= 0)
    {
      return n_read;
    }

  cmsgptr = CMSG_FIRSTHDR(&msg);
  if (cmsgptr && 
      cmsgptr->cmsg_level == SOL_SOCKET && 
      cmsgptr->cmsg_type == SCM_RIGHTS)
    {
      *fd = *(int *)CMSG_DATA(cmsgptr);
    }

  return n_read;
}

/* Return 0 if a datagram AF_UNIX socket identified by path may be opened
   and return -1 on error, local contains the path specification and
   fd the file descriptor of the socket. */
static int
new_socket(const char *path, struct sockaddr_un *local, int *fd)
{
  *fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (*fd < 0)
    {
      return -1;
    }

  memset(local, 0, sizeof(struct sockaddr_un));
  local->sun_family = AF_UNIX;
  memcpy(local->sun_path, path, strlen(path));

  return 0;
}

/* Return -1 and the triggering errno, close the opened socket file
   descriptor. */
static int
panic_socket(int fd)
{
  int tmp_errno;

  tmp_errno = errno;
  (void)close(fd);
  errno = tmp_errno;

  return -1;
}
