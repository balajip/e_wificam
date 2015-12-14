#ident "$Id: lookup_file.c,v 1.2 1997/10/06 21:52:02 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  lookup_file.c - module for Linux automount to query a flat file map
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
#include <ctype.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MODULE_LOOKUP
#include "automount.h"

#define MAPFMT_DEFAULT "sun"

#define MODPREFIX "lookup(file): "

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
    syslog(LOG_CRIT, MODPREFIX "file map %s is not an absolute pathname",
	   ctxt->mapname);
    return 1;
  }

  if ( access(ctxt->mapname, R_OK) ) {
    syslog(LOG_WARNING, MODPREFIX "file map %s missing or not readable",
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
  int ch, nch;
  char mapent[MAPENT_MAX_LEN+1], *p;
  const char *nptr;
  int mapent_len;
  FILE *f;
  enum {
    st_begin, st_compare, st_star, st_badent, st_entspc, st_getent
  } state;
  enum { got_nothing, got_star, got_real } getting, gotten;
  enum { esc_none, esc_char, esc_val } escape;

  syslog(LOG_DEBUG, MODPREFIX "looking up %s", name);

  chdir("/");			/* If this is not here the filesystem stays
				   busy, for some reason... */
  f = fopen(ctxt->mapname, "r");
  if ( !f ) {
    syslog(LOG_ERR, MODPREFIX "could not open map file %s", ctxt->mapname);
    return 1;
  }
  
  state = st_begin;
  gotten = got_nothing;

  /* Shut up gcc */
  nptr = p = NULL;
  mapent_len = 0;
  getting = got_nothing;
  escape = esc_none;

  /* This is ugly.  We can't just remove \ escape sequences in the value
     portion of an entry, because the parsing routine may need it. */

  while ( (ch = getc(f)) != EOF ) {
    switch ( escape ) {
    case esc_none:
      if ( ch == '\\' ) {
	/* Handle continuation lines */
	if ( (nch = getc(f)) == '\n' )
	  continue;
	  //goto next_char;
	ungetc(nch,f);
	escape = esc_char;
      }
      break;

    case esc_char:
      escape = esc_val;
      break;

    case esc_val:
      escape = esc_none;
      break;
    }     

    switch(state) {
    case st_begin:
      if ( isspace(ch) && !escape )
	;
      else if ( escape == esc_char )
	;
      else if ( ch == '#' && !escape )
	state = st_badent;
      else if ( (char)ch == name[0] ) {
	state = st_compare;
	nptr = name+1;
      }	else if ( ch == '*' && !escape )
	state = st_star;
      else
	state = st_badent;
      break;

    case st_compare:
      if ( ch == '\n' )
	state = st_begin;
      else if ( isspace(ch) && !*nptr && !escape ) {
	getting = got_real;
	state = st_entspc;
      } else if ( escape == esc_char )
	;
      else if ( (char)ch != *(nptr++) )
	state = st_badent;
      break;

    case st_star:
      if ( ch == '\n' )
	state = st_begin;
      else if ( isspace(ch) && gotten < got_star && !escape ) {
	getting = got_star;
	state = st_entspc;
      } else if ( escape != esc_char )
	state = st_badent;
      break;

    case st_badent:
      if ( ch == '\n' )
	state = st_begin;
      break;

    case st_entspc:
      if ( ch == '\n' )
	state = st_begin;
      else if ( !isspace(ch) || escape ) {
	state = st_getent;
	p = mapent;
	gotten = getting;
	*(p++) = ch;
	mapent_len = 1;
      }
      break;

    case st_getent:
      if ( ch == '\n' ) {
	state = st_begin;
	if ( gotten == got_real )
	  goto got_it;		/* No point in parsing the rest of the file */
      } else if ( mapent_len < MAPENT_MAX_LEN ) {
	mapent_len++;
	*(p++) = ch;
      }
      break;
    }
 // next_char:			/* End of loop, since we can't continue;
//				   inside a switch */
  }
  
got_it:
  fclose(f);

  if ( gotten == got_nothing ) {
    syslog(LOG_NOTICE, MODPREFIX "lookup for %s failed", name);
    return 1;			/* Didn't found anything */
  }
  *p = '\0';			/* Null-terminate */

  syslog(LOG_DEBUG, MODPREFIX "%s -> %s", name, mapent);

  return ctxt->parse->parse_mount(root,name,name_len,mapent,ctxt->parse->context); 
}

int lookup_done(void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int rv = close_parse(ctxt->parse);
  free(ctxt);
  return rv;
}
