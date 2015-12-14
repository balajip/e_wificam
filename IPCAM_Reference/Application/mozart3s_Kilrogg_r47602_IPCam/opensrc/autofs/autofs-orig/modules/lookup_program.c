#ident "$Id: lookup_program.c,v 1.3 1999/12/17 19:02:47 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  lookup_program.c - module for Linux automount to access an
 *                     automount map via a query program 
 *
 *   Copyright 1997 Transmeta Corporation - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MODULE_LOOKUP
#include "automount.h"

#define MAPFMT_DEFAULT "sun"

#define MODPREFIX "lookup(program): "

#define MAPENT_MAX_LEN 4095

struct lookup_context {
  const char *mapname;
  struct parse_mod *parse;
};

int lookup_version = AUTOFS_LOOKUP_VERSION; /* Required by protocol */

int lookup_init(const char *mapfmt, int argc, const char * const *argv,
		void **context)
{
  struct lookup_context *ctxt;

  if ( !(*context = ctxt = malloc(sizeof(struct lookup_context))) ) {
    syslog(LOG_CRIT, MODPREFIX "malloc: %m");
    return 1;
  }
  if ( argc < 1 ) {
    syslog(LOG_CRIT, MODPREFIX "No map name");
    return 1;
  }
  ctxt->mapname = argv[0];

  if (ctxt->mapname[0] != '/') {
    syslog(LOG_CRIT, MODPREFIX "program map %s is not an absolute pathname",
	   ctxt->mapname);
    return 1;
  }

  if ( access(ctxt->mapname, X_OK) ) {
    syslog(LOG_WARNING, MODPREFIX "program map %s missing or not executable",
	   ctxt->mapname);
  }

  if ( !mapfmt )
    mapfmt = MAPFMT_DEFAULT;

  return !(ctxt->parse = open_parse(mapfmt,MODPREFIX,argc-1,argv+1));
}

int lookup_mount(const char *root, const char *name, int name_len,
		 void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  char mapent[MAPENT_MAX_LEN+1], *mapp;
  char errbuf[1024], *errp;
  char *p, ch;
  int pipefd[2], epipefd[2];
  pid_t f;
  int files_left;
  int status;
  fd_set readfds, ourfds;

  syslog(LOG_DEBUG, MODPREFIX "looking up %s", name);

  /* We don't use popen because we don't want to run /bin/sh plus we
     want to send stderr to the syslog, and we don't use spawnl()
     because we need the pipe hooks */

  if ( pipe(pipefd) ) {
    syslog(LOG_ERR, MODPREFIX "pipe: %m");
    return 1;
  }
  if ( pipe(epipefd) ) {
    close(pipefd[0]);
    close(pipefd[1]);
    return 1;
  }
  f = fork();
  if ( f < 0 ) {
    close(pipefd[0]);
    close(pipefd[1]);
    close(epipefd[0]);
    close(epipefd[1]);
    syslog(LOG_ERR, MODPREFIX "fork: %m");
    return 1;
  } else if ( f == 0 ) {
    close(pipefd[0]);
    close(epipefd[0]);
    dup2(pipefd[1],STDOUT_FILENO);
    dup2(epipefd[1],STDERR_FILENO);
    close(pipefd[1]);
    close(epipefd[1]);
    execl(ctxt->mapname, ctxt->mapname, name, NULL);
    _exit(255);			/* execl() failed */
  }
  close(pipefd[1]);
  close(epipefd[1]);

  mapp = mapent;
  errp = errbuf;

  FD_ZERO(&ourfds);
  FD_SET(pipefd[0],&ourfds);
  FD_SET(epipefd[0],&ourfds);

  files_left = 2;

  while (files_left) {
    readfds = ourfds;
    if ( select(OPEN_MAX, &readfds, NULL, NULL, NULL) < 0 && 
	 errno != EINTR )
      break;
    
    if ( FD_ISSET(pipefd[0],&readfds) ) {
      if ( read(pipefd[0], &ch, 1) < 1 ) {
	FD_CLR(pipefd[0], &ourfds);
	files_left--;
      } else if ( mapp ) {
	if ( ch == '\n' ) {
	  *mapp = '\0';
	  mapp = NULL;		/* End of line reached */
	} else if ( mapp-mapent < MAPENT_MAX_LEN )
	  *(mapp++) = ch;
      }
    }
    if ( FD_ISSET(epipefd[0],&readfds) ) {
      if ( read(epipefd[0], &ch, 1) < 1 ) {
	FD_CLR(epipefd[0], &ourfds);
	files_left--;
      } else if ( ch == '\n' ) {
	*errp = '\0';
	if ( errbuf[0] )
	  syslog(LOG_NOTICE, ">> %s", errbuf);
	errp = errbuf;
      } else {
	if ( errp >= &errbuf[1023] ) {
	  *errp = '\0';
	  syslog(LOG_NOTICE, ">> %s", errbuf);
	  errp = errbuf;
	}
	*(errp++) = ch;
      }
    }
  }

  if ( mapp )
    *mapp = '\0';
  if ( errp > errbuf ) {
    *errp = '\0';
    syslog(LOG_NOTICE, ">> %s", errbuf);
  }

  close(pipefd[0]);
  close(epipefd[0]);

  if ( waitpid(f,&status,0) != f ) {
    syslog(LOG_ERR, MODPREFIX "waitpid: %m");
    return 1;
  }
  if ( mapp == mapent || !WIFEXITED(status) || WEXITSTATUS(status) != 0 ) {
    syslog(LOG_NOTICE, MODPREFIX "lookup for %s failed", name);
    return 1;
  }
  if ( (p = strchr(mapent,'\n')) ) *p = '\0';

  syslog(LOG_DEBUG, MODPREFIX "%s -> %s", name, mapent);

  return ctxt->parse->parse_mount(root,name,name_len,mapent,
				  ctxt->parse->context); 
}

int lookup_done(void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int rv = close_parse(ctxt->parse);
  free(ctxt);
  return rv;
}
