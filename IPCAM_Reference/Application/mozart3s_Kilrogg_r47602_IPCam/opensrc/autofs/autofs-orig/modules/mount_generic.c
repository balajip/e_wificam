#ident "$Id: mount_generic.c,v 1.5 1999/12/17 19:02:47 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  mount_generic.c - module for Linux automountd to mount filesystems
 *                    for which no special magic is required
 *
 *   Copyright 1997-1999 Transmeta Corporation - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>
#define MODULE_MOUNT
#include "automount.h"

#define MODPREFIX "mount(generic): "
int mount_version = AUTOFS_MOUNT_VERSION; /* Required by protocol */

int mount_init(void **context)
{
  return 0;
}

int mount_mount(const char *root, const char *name, int name_len,
		const char *what, const char *fstype, const char *options,
		void *context)
{
  char *fullpath;
  int err;

  fullpath = alloca(strlen(root)+name_len+2);
  if ( !fullpath ) {
    syslog(LOG_ERR, MODPREFIX "alloca: %m");
    return 1;
  }
  sprintf(fullpath, "%s/%s", root, name);
    
  syslog(LOG_DEBUG, MODPREFIX "calling mkdir %s", fullpath);
  if ( mkdir(fullpath, 0555) && errno != EEXIST ) {
    syslog(LOG_NOTICE, MODPREFIX "mkdir %s failed: %m", name);
    return 1;
  }
    
  if ( options ) {
    syslog(LOG_DEBUG, MODPREFIX "calling mount -t %s " SLOPPY "-o %s %s %s",
	   fstype, options, what, fullpath);
    err = spawnl(LOG_NOTICE, PATH_MOUNT, PATH_MOUNT, "-t", fstype,
		 SLOPPYOPT "-o", options, what, fullpath, NULL);
  } else {
    syslog(LOG_DEBUG, MODPREFIX "calling mount -t %s %s %s",
	   fstype, what, fullpath);
      err = spawnl(LOG_NOTICE, PATH_MOUNT, PATH_MOUNT, "-t", fstype,
		   what, fullpath, NULL);
  }
  if ( err ) {
    rmdir(fullpath);
    syslog(LOG_NOTICE, MODPREFIX "failed to mount %s (type %s) on %s",
	   what, fstype, fullpath);
    return 1;
  } else {
    syslog(LOG_DEBUG, MODPREFIX "mounted %s type %s on %s",
	   what, fstype, fullpath);
    return 0;
  }
}

int mount_done(void *context)
{
  return 0;
}
