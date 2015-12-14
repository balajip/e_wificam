#ident "$Id: mount_autofs.c,v 1.6 1999/03/07 22:34:53 hpa Exp $"
/*
 * mount_autofs.c
 *
 * Module for recursive autofs mounts.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <alloca.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MODULE_MOUNT
#include "automount.h"

#define MODPREFIX "mount(autofs): "
int mount_version = AUTOFS_MOUNT_VERSION; /* Required by protocol */

int mount_init(void **context)
{
  return 0;
}

int mount_mount(const char *root, const char *name, int name_len,
		const char *what, const char *fstype, const char *c_options,
		void *context)
{
  char *fullpath, **argv;
  int argc, status;
  char *options, *p;
  pid_t slave, wp;

  fullpath = alloca(strlen(root)+name_len+2);
  if ( !fullpath ) {
    syslog(LOG_ERR, MODPREFIX "alloca: %m");
    return 1;
  }
  sprintf(fullpath, "%s/%s", root, name);

  if ( c_options ) {
    options = alloca(strlen(c_options)+1);
    if ( !options ) {
      syslog(LOG_ERR, MODPREFIX "alloca: %m");
      return 1;
    }
    strcpy(options, c_options);
  } else {
    options = NULL;
  }

  syslog(LOG_DEBUG, MODPREFIX "calling mkdir %s", fullpath);
  if ( mkdir(fullpath, 0555) && errno != EEXIST ) {
    syslog(LOG_NOTICE, MODPREFIX "mkdir %s failed: %m", name);
    return 1;
  }
    
  syslog(LOG_DEBUG, MODPREFIX "fullpath=%s what=%s options=%s", 
	 fullpath, what, options);

  /* Build our argument vector.  */

  argc = 5;
  if ( options ) {
    char *p = options;
    do {
      argc++;
      if ( *p == ',' ) p++;
    } while ((p = strchr(p,',')) != NULL);
  }
  argv = (char **) alloca((argc+1) * sizeof(char *));

  argc = 0;
  argv[argc++] = PATH_AUTOMOUNT;
  argv[argc++] = "--submount";
  argv[argc++] = fullpath;
  argv[argc++] = strcpy(alloca(strlen(what)+1), what);
  
  if ( (p = strchr(argv[argc-1], ':')) == NULL ) {
    syslog(LOG_NOTICE, MODPREFIX "%s missing script type on %s", name, what);
    goto error;
  }

  *p++ = '\0';
  argv[argc++] = p;

  if ( options ) {
    /* Rainer Clasen reported funniness using strtok() here. */

    p = options;
    do {
      if ( *p == ',' ) {
	*p = '\0';
	p++;
      }
      argv[argc++] = p;
    } while ( (p = strchr(p,',')) != NULL );
  }
  argv[argc] = NULL;

  /* Spawn a new daemon.  If initialization is successful, the daemon will send
     itself SIGSTOP, which we detect and let it go on its merry way. */

  slave = fork();
  if ( slave < 0 ) { 
    syslog(LOG_ERR, MODPREFIX "fork: %m");
    goto error;
  } else if ( slave == 0 ) {
    /* Slave process */
    execv(PATH_AUTOMOUNT, argv);
    _exit(255);
  }

  while ( (wp = waitpid(slave, &status, WUNTRACED)) == -1 && errno == EINTR );
  if ( wp != slave ) {
    syslog(LOG_NOTICE, MODPREFIX "waitpid: %m");
    goto error;
  }

  if ( !WIFSTOPPED(status) || WSTOPSIG(status) != SIGSTOP ) {
    syslog(LOG_NOTICE, MODPREFIX "sub automount returned status 0x%x", status);
    goto error;
  }

  kill(slave, SIGCONT);		/* Carry on, private */

  syslog(LOG_DEBUG, MODPREFIX "mounted %s on %s", what, fullpath);
  return 0;

error:
  rmdir(fullpath);
  syslog(LOG_NOTICE, MODPREFIX "failed to mount %s on %s", what, fullpath);
  return 1;
}

int mount_done(void *context)
{
  return 0;
}
