#ident "$Id: mount_bind.c,v 1.3 2000/11/04 07:02:22 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  mount_bind.c      - module to mount a local filesystem if possible;
 *			otherwise create a symlink.
 *
 *   Copyright 2000 Transmeta Corporation - All Rights Reserved
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
#include <limits.h>
#include <time.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>
#define MODULE_MOUNT
#include "automount.h"

#define MODPREFIX "mount(bind): "
int mount_version = AUTOFS_MOUNT_VERSION; /* Required by protocol */

static int have_mount_bind = -1; /* -1 means: not tested yet */

/* This gives a conservative estimate for the number of hex digits it
   may require to printf() type T. */
#define HEXDIGITS(T) ((sizeof(T)*CHAR_BIT+3)/4)

/* Test to see if "mount --bind" works properly or not */
static int mount_bind_works_p(void)
{
  char testdir[32+HEXDIGITS(time_t)+HEXDIGITS(pid_t)];
  char dir1[40+HEXDIGITS(time_t)+HEXDIGITS(pid_t)];
  char dir2[40+HEXDIGITS(time_t)+HEXDIGITS(pid_t)];
  struct stat st1, st2;
  pid_t mypid = getpid();
  int rv;
  int bind_works = 0;		/* Guilty until proven innocent */
  
  syslog(LOG_DEBUG, MODPREFIX "Testing if \"mount --bind\" works correctly...");

  /* Make a safe temporary directory */
  do {
    sprintf(testdir, "/tmp/autofs-bind-%lx-%lx",
	    (unsigned long)time(NULL), (unsigned long)mypid);
    rv = mkdir(testdir, 0700);
    if ( rv && errno != EEXIST ) {
      syslog(LOG_ERR, MODPREFIX "Cannot create temporary directory: %m");
      goto f0;
    }
  } while ( rv );

  sprintf(dir1, "%s/dir1", testdir);
  sprintf(dir2, "%s/dir2", testdir);

  if ( mkdir(dir1, 0700) )
    goto f1;
  if ( mkdir(dir2, 0700) )
    goto f2;

  syslog(LOG_DEBUG, MODPREFIX "calling mount --bind %s %s",
	 dir1, dir2);
  if ( spawnl(LOG_DEBUG, PATH_MOUNT, PATH_MOUNT, "--bind",
	      dir1, dir2, NULL) ) {
    syslog(LOG_DEBUG, MODPREFIX "mount --bind failed");
    goto f3;
  }

  /* mount --bind returned OK, now verify it did the right thing */
  if ( stat(dir1, &st1) || stat(dir2, &st2) ) {
    syslog(LOG_DEBUG, MODPREFIX "Cannot stat temporary directories: %m");
    goto f4;
  }
  
  /* If dir1 and dir2 are now the same directory, mount --bind worked */
  if ( st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino )
    bind_works = 1;

  /* Disassemble the test setup */
 f4:
  /* umount the bind */
  spawnl(LOG_DEBUG, PATH_UMOUNT, PATH_UMOUNT, dir2, NULL);
 f3:
  rmdir(dir2);
 f2:
  rmdir(dir1);
 f1:
  rmdir(testdir);
 f0:
  return bind_works;
}

int mount_init(void **context)
{
  if ( have_mount_bind == -1 ) {
    /* Need to figure out if mount --bind works or not */
    have_mount_bind = mount_bind_works_p();
  }
  return 0;
}

int mount_mount(const char *root, const char *name, int name_len,
		const char *what, const char *fstype, const char *options,
		void *context)
{
  char *fullpath;
  int err = 0;

  fullpath = alloca(strlen(root)+name_len+2);
  if ( !fullpath ) {
    syslog(LOG_ERR, MODPREFIX "alloca: %m");
    return 1;
  }
  sprintf(fullpath, "%s/%s", root, name);

  if ( have_mount_bind ) {
    syslog(LOG_DEBUG, MODPREFIX "calling mkdir %s", fullpath);
    if ( mkdir(fullpath, 0555) && errno != EEXIST ) {
      syslog(LOG_NOTICE, MODPREFIX "mkdir %s failed: %m", name);
      return 1;
    }
    
    syslog(LOG_DEBUG, MODPREFIX "calling mount --bind %s %s",
	   what, fullpath);
    err = spawnl(LOG_NOTICE, PATH_MOUNT, PATH_MOUNT, "--bind",
		 what, fullpath, NULL);

    if ( err ) {
      if ( rmdir(fullpath) && errno == EBUSY ) {
	return 0;
      } else {
	/* This shouldn't happen, but try anyway... */
	syslog(LOG_NOTICE, MODPREFIX "failed to mount --bind %s on %s, trying symlink",
	       what, fullpath);
      }
    }
  }

  if ( err || !have_mount_bind ) {
    if ( symlink(what, fullpath) && errno != EEXIST ) {
      syslog(LOG_NOTICE, MODPREFIX "failed to create symlink %s -> %s", fullpath, what);
      return 1;
    } else {
      syslog(LOG_DEBUG, MODPREFIX "symlinked %s -> %s", fullpath, what);
      return 0;
    }
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
