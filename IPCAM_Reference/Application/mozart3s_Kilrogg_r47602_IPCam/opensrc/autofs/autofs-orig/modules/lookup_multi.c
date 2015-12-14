#ident "$Id: lookup_multi.c,v 1.2 2000/09/28 21:51:32 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  lookup_multi.c - module for Linux automount to seek multiple lookup
 *                   methods in succession
 *
 *   Copyright 1999-2000 Transmeta Corporation - All Rights Reserved
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

#define MODULE_LOOKUP
#include "automount.h"

#define MODPREFIX "lookup(multi): "

#define MAPENT_MAX_LEN 4095

struct module_info {
  int argc;
  const char * const *argv;
  struct lookup_mod *mod;
};

struct lookup_context {
  int n;
  const char **argl;
  struct module_info *m;
};

int lookup_version = AUTOFS_LOOKUP_VERSION; /* Required by protocol */

int lookup_init(const char *my_mapfmt, int argc, const char * const *argv,
		void **context)
{
  struct lookup_context *ctxt;
  char *map, *mapfmt;
  int i, j, an;

  if ( !(*context = ctxt = malloc(sizeof(struct lookup_context))) )
    goto nomem;

  if ( argc < 1 ) {
    syslog(LOG_CRIT, MODPREFIX "No map list");
    return 1;
  }
  ctxt->n = 1;			/* Always at least one map */
  for ( i = 0 ; i < argc ; i++ ) {
    if ( !strcmp(argv[i], "--") ) /* -- separates maps */
      ctxt->n++;
  }

  if ( !(ctxt->m = malloc(ctxt->n*sizeof(struct module_info))) ||
       !(ctxt->argl = malloc((argc+1)*sizeof(const char **))) )
    goto nomem;

  memcpy(ctxt->argl, argv, (argc+1)*sizeof(const char **));

  for ( i = j = an = 0 ; argv[an] ; an++ ) {
    if ( j == 0 )
      ctxt->m[i].argv = &ctxt->argl[an];
    if ( !strcmp(ctxt->argl[an], "--") ) {
      ctxt->argl[an] = NULL;
      i++; j = 0;
    } else {
      ctxt->m[i].argc++;
      j++;
    }
  }

  for ( i = 0 ; i < ctxt->n ; i++ ) {
    if ( !ctxt->m[i].argv[0] ) {
      syslog(LOG_CRIT, MODPREFIX "missing module name");
      return 1;
    }
    map = strdup(ctxt->m[i].argv[0]);
    if ( !map )
      goto nomem;

    if ( (mapfmt = strchr(map, ',')) )
      *(mapfmt++) = '\0';
    
    if ( !(ctxt->m[i].mod = open_lookup(map, MODPREFIX,
					mapfmt ? mapfmt : my_mapfmt,
					ctxt->m[i].argc, ctxt->m[i].argv)) )
      return 1;
  }

  *context = ctxt;
  return 0;

 nomem:
  syslog(LOG_CRIT, MODPREFIX "malloc: %m");
  return 1;
}

int lookup_mount(const char *root, const char *name, int name_len,
		 void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int i;

  for ( i = 0 ; i < ctxt->n ; i++ ) {
    if ( ctxt->m[i].mod->lookup_mount(root,name,name_len,
				      ctxt->m[i].mod->context) == 0 )
      return 0;
  }
  return 1;			/* No module succeeded */
}

int lookup_done(void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int i, rv = 0;

  for ( i = 0 ; i < ctxt->n ; i++ ) {
    rv = rv || close_lookup(ctxt->m[i].mod);
  }

  free(ctxt->argl);
  free(ctxt->m);
  free(ctxt);

  return rv;
}
