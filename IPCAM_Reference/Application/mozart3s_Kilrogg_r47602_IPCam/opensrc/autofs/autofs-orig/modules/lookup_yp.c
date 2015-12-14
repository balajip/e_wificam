#ident "$Id: lookup_yp.c,v 1.3 1999/03/07 22:34:52 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  lookup_yp.c - module for Linux automountd to access a YP (NIS)
 *                automount map
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

#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <rpcsvc/yp_prot.h>
#include <rpcsvc/ypclnt.h>

#define MODULE_LOOKUP
#include "automount.h"

#define MAPFMT_DEFAULT "sun"

#define MODPREFIX "lookup(yp): "

struct lookup_context {
  const char *domainname;
  const char *mapname;
  struct parse_mod *parse;
};

int lookup_version = AUTOFS_LOOKUP_VERSION; /* Required by protocol */

int lookup_init(const char *mapfmt, int argc, const char * const *argv,
		void **context)
{
  struct lookup_context *ctxt;
  int err;

  if ( !(*context = ctxt = malloc(sizeof(struct lookup_context))) ) {
    syslog(LOG_CRIT, MODPREFIX "%m");
    return 1;
  }
  
  if ( argc < 1 ) {
    syslog(LOG_CRIT, MODPREFIX "No map name");
    return 1;
  }
  ctxt->mapname = argv[0];

  /* This should, but doesn't, take a const char ** */
  err = yp_get_default_domain((char **) &ctxt->domainname);
  if ( err ) {
    syslog(LOG_CRIT, MODPREFIX "map %s: %s\n", ctxt->mapname, yperr_string(err));
    return 1;
  }

  if ( !mapfmt )
    mapfmt = MAPFMT_DEFAULT;

  return !(ctxt->parse = open_parse(mapfmt,MODPREFIX,argc-1,argv+1));
}

int lookup_mount(const char *root, const char *name,
		 int name_len, void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  char *mapent;
  int mapent_len;
  int err, rv;

  syslog(LOG_DEBUG, MODPREFIX "looking up %s", name);

  /* For reasons unknown, the standard YP definitions doesn't define input
     strings as const char *.  However, my understanding is that they will
     not be modified by the library. */
  err = yp_match((char *) ctxt->domainname, (char *) ctxt->mapname,
		 (char *) name, name_len, &mapent, &mapent_len);
  if ( err == YPERR_KEY ) {
    /* Try to get the "*" entry if there is one - note that we *don't*
       modify "name" so & -> the name we used, not "*" */
    err = yp_match((char *) ctxt->domainname, (char *) ctxt->mapname,
		   "*", 1, &mapent, &mapent_len);
  }
  if ( err ) {
    syslog(LOG_NOTICE, MODPREFIX "lookup for %s failed: %s", name, yperr_string(err));
    return 1;
  }

  mapent[mapent_len] = '\0';

  syslog(LOG_DEBUG, MODPREFIX "%s -> %s", name, mapent);

  rv = ctxt->parse->parse_mount(root,name,name_len,mapent,ctxt->parse->context); 
  free(mapent);
  return rv;
}

int lookup_done(void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int rv = close_parse(ctxt->parse);
  free(ctxt);
  return rv;
}
