#ident "$Id: parse_sun.c,v 1.2 1997/10/06 21:52:03 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *  parse_sun.c - module for Linux automountd to parse a Sun-format
 *                automounter map
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
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <syslog.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <alloca.h>


#define MODULE_PARSE
#include "automount.h"

#define MODPREFIX "parse(sun): "
int parse_version = AUTOFS_PARSE_VERSION; /* Required by protocol */

static struct mount_mod *mount_nfs = NULL;
static int init_ctr = 0;

struct substvar {
  char *def;			/* Define variable */
  char *val;			/* Value to replace with */
  struct substvar *next;
};

struct parse_context {
  char *optstr;			/* Mount options */
  struct substvar *subst;	/* $-substitutions */
  int slashify_colons;		/* Change colons to slashes? */
};

struct utsname un;
char processor[65];		/* Not defined on Linux, so we make our own */

static struct substvar		/* Predefined variables: tail of link chain */
  sv_arch   = { "ARCH",   un.machine,  NULL },
  sv_cpu    = { "CPU",    processor,   &sv_arch },
  sv_host   = { "HOST",   un.nodename, &sv_cpu },
  sv_osname = { "OSNAME", un.sysname,  &sv_host },
  sv_osrel  = { "OSREL",  un.release,  &sv_osname },
  sv_osvers = { "OSVERS", un.version,  &sv_osrel };

/* Default context pattern */

static struct parse_context default_context = {
  NULL,				/* No mount options */
  &sv_osvers,			/* The substvar predefined variables */
  1				/* Do slashify_colons */
};

/* Free all storage associated with this context */
static void kill_context(struct parse_context *ctxt)
{
  struct substvar *sv, *nsv;

  sv = ctxt->subst;
  while ( sv != &sv_osvers ) {
    nsv = sv->next;
    free(sv);
    sv = nsv;
  }
  
  if ( ctxt->optstr )
    free(ctxt->optstr);

  free(ctxt);
}

/* Find the $-variable matching a certain string fragment */
static const struct substvar *findvar(const struct substvar *sv,
				      const char *str, int len)
{
  while ( sv ) {
    if ( !strncmp(str,sv->def,len) && sv->def[len] == '\0' )
      return sv;
    sv = sv->next;
  }
  return NULL;
}

/* $- and &-expand a Sun-style map entry and return the length of the entry.
   If "dst" is NULL, just count the length. */
static int expandsunent(const char *src, char *dst, const char *key,
			const struct substvar *svc, int slashify_colons)
{
  const struct substvar *sv;
  int len, l, seen_colons;
  const char *p;
  char ch;

  len = 0;
  seen_colons = 0;
  
  while ( (ch = *src++) ) {
    switch ( ch ) {
    case '&':
      l = strlen(key);
      if ( dst ) {
	strcpy(dst,key);
	dst += l;
      }
      len += l;
      break;

    case '$':
      if ( *src == '{' ) {
	p = strchr(++src,'}');
	if ( !p ) {
	  /* Ignore rest of string */
	  if ( dst ) *dst = '\0';
	  return len;
	}
	sv = findvar(svc, src, p-src);
	if ( sv ) {
	  l = strlen(sv->val);
	  if ( dst ) {
	    strcpy(dst,sv->val);
	    dst += l;
	  }
	  len += l;
	}
	src = p+1;
      } else {
	p = src;
	while ( isalnum(*p) || *p == '_' ) p++;
	sv = findvar(svc, src, p-src);
	if ( sv ) {
	  l = strlen(sv->val);
	  if ( dst ) {
	    strcpy(dst,sv->val);
	    dst += l;
	  }
	  len += l;
	}
	src = p;
      }
      break;

    case '\\':
      ch = *src;
      if ( ch ) {
	src++;
	if ( dst ) *(dst++) = ch;
	len++;
      } else {
	; /* Terminal backslash, just eat it */
      }
      break;

    case ':':
      if ( dst ) *(dst++) = (seen_colons && slashify_colons) ? '/' : ':';
      len++;
      seen_colons = 1;
      break;

    default:
      if ( dst ) *(dst++) = ch;
      len++;
      break;
    }
  }
  if ( dst ) *dst = '\0';
  return len;
}

/* Skip whitespace in a string; if we hit a #, consider the rest of the
   entry a comment */
char *skipspace(char *whence)
{
  while(1) {
    switch(*whence) {
    case ' ':
    case '\b':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
      whence++;
      break;
    case '\0':
    case '#':
      return NULL;		/* End of string */
    default:
      return whence;
    }
  }
}

/* Get the length of a chunk delimitered by whitespace */
int chunklen(char *whence)
{
  int n = 0;
  while(1) {
    switch(*whence) {
    case ' ':
    case '\b':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
    case '#':
    case '\0':
      return n;
    default:
      break;
    }
    n++;
    whence++;
  }
}

/* Compare str with pat.  Return 0 if compare equal or str is an abbreviation of
   pat of no less than mchr characters. */
int strmcmp(const char *str, const char *pat, int mchr)
{
  int nchr = 0;

  while ( *str == *pat ) {
    if ( ! *str ) return 0;
    str++; pat++; nchr++;
  }

  if ( ! *str && nchr > mchr )
    return 0;

  return *pat - *str;
}

int parse_init(int argc, const char * const *argv, void **context)
{
  struct parse_context *ctxt;
  struct substvar *sv;
  char *noptstr;
  const char *xopt;
  int optlen, len;
  int i, bval;

  /* Get processor information for predefined escapes */

  if ( !init_ctr ) {
    uname(&un);
    /* uname -p is not defined on Linux.  Make it the same as uname -m,
       except make it return i386 on all x86 (x >= 3) */
    strcpy(processor, un.machine);
    if ( processor[0] == 'i' && processor[1] >= '3' &&
	 !strcmp(processor+2, "86") )
      processor[1] = '3';
  }

  /* Set up context and escape chain */
  
  if ( !(ctxt = (struct parse_context *) malloc(sizeof(struct parse_context))) ) {
    syslog(LOG_CRIT, MODPREFIX "malloc: %m");
    return 1;
  }
  *context = (void *) ctxt;
  
  *ctxt = default_context;
  optlen = 0;

  /* Look for options and capture, and create new defines if we need to */

  for ( i = 0 ; i < argc ; i++ ) {
    if ( argv[i][0] == '-' ) {
      switch(argv[i][1]) {
      case 'D':
	sv = malloc(sizeof(struct substvar));
	if ( !sv ) {
	  syslog(LOG_ERR, MODPREFIX "malloc: %m");
	  break;
	}
	if ( argv[i][2] )	  
	  sv->def = strdup(argv[i]+2);
	else if ( ++i < argc )
	  sv->def = strdup(argv[i]);
	else {
	  free(sv);
	  break;
	}

	if ( !sv->def ) {
	  syslog(LOG_ERR, MODPREFIX "strdup: %m");
	  free(sv);
	} else {
	  sv->val = strchr(sv->def, '=');
	  if ( sv->val )
	    *(sv->val++) = '\0';
	  else
	    sv->val = "";
	  
	  sv->next = ctxt->subst;
	  ctxt->subst = sv;
	}
	break;

      case '-':
	if ( !strncmp(argv[i]+2, "no-", 3) ) {
	  xopt = argv[i]+5;
	  bval = 0;
	} else {
	  xopt = argv[i]+2;
	  bval = 1;
	}

	if ( strmcmp(xopt, "slashify-colons", 1) )
	  ctxt->slashify_colons = bval;
	else
	  syslog(LOG_ERR, MODPREFIX "unknown option: %s", argv[i]);

	break;

      default:
	syslog(LOG_ERR, MODPREFIX "unknown option: %s", argv[i]);
	break;
      }
    } else {
      len = strlen(argv[i]);
      if ( ctxt->optstr ) {
	noptstr = (char *) realloc(ctxt->optstr, optlen+len+2);
	if ( !noptstr )
	  break;
	noptstr[optlen] = ',';
	strcpy(noptstr+optlen+1, argv[i]);
	optlen += len+1;
      } else {
	noptstr = (char *) malloc(len+1);
	strcpy(noptstr, argv[i]);
	optlen = len;
      }
      if ( !noptstr ) {
	kill_context(ctxt);
	syslog(LOG_CRIT, MODPREFIX "%m");
	return 1;
      }
      ctxt->optstr = noptstr;
      syslog(LOG_DEBUG, MODPREFIX "init gathered options: %s", ctxt->optstr);
    }
  }

  /* We only need this once.  NFS mounts are so common that we cache
     this module. */
  if ( !mount_nfs )
    if ( (mount_nfs = open_mount("nfs", MODPREFIX)) ) {
      init_ctr++;
      return 0;
    } else {
      kill_context(ctxt);
      return 1;
    }
  else {
    init_ctr++;
    return 0;
  }
}

int parse_mount(const char *root, const char *name,
		int name_len, const char *mapent, void *context)
{
  struct parse_context *ctxt = (struct parse_context *) context;
  char *pmapent, *options, *noptions, *ent, *p, *q, *fstype;
  int mapent_len, rv;
  int l, optlen;

  mapent_len = expandsunent(mapent,NULL,name,ctxt->subst,ctxt->slashify_colons);
  pmapent = alloca(mapent_len + 1);
  if (!pmapent) {
    syslog(LOG_ERR, MODPREFIX "alloca: %m");
    return 1;
  }
  expandsunent(mapent,pmapent,name,ctxt->subst,ctxt->slashify_colons);

  syslog(LOG_DEBUG, "expanded entry: %s", pmapent);

  options = strdup(ctxt->optstr ? ctxt->optstr : "");
  if ( !options ) {
    syslog(LOG_ERR, MODPREFIX "strdup: %m");
    return 1;
  }
  optlen = strlen(options);

  p = pmapent;
  ent = NULL;

  while ( (p = skipspace(p)) ) {
    if ( *p == '-' ) {
      l = chunklen(++p);
      if ( !l )
	continue;
      noptions = realloc(options, optlen+l+(optlen ? 2 : 1));
      if ( !noptions ) {
	free(options);
	syslog(LOG_ERR, MODPREFIX "realloc: %m");
	return 1;
      }
      if ( optlen ) noptions[optlen++] = ',';
      memcpy(noptions+optlen, p, l);
      noptions[optlen += l] = '\0';
      options = noptions;
      p += l;
      syslog(LOG_DEBUG, MODPREFIX "gathered options: %s", options);
    } else if ( *p == '/' ) {
      /* Uh-oh, a multientry; don't support now, but ****FIXME**** */
      /* We do tolerate "multientries" that only contain / though... */
      if ( chunklen(p) == 1 ) {
	p++;
      } else {
	free(options);
	syslog(LOG_NOTICE, MODPREFIX "entry %s is a multipath entry", name);
	return 1;
      }
    } else {
      p[chunklen(p)] = '\0';	/* We don't use anything after the entry */
      if ( *p == ':' ) p++;	/* Sun escape for entries starting with / */
      if ( ! *p ) {
	syslog(LOG_ERR, MODPREFIX "entry %s is empty!", name);
	free(options);
	return 1;
      }
      ent = p;
      syslog(LOG_DEBUG, MODPREFIX "core of entry: %s", ent);
      break;			/* We have the core entry, run w/it */
    }
  }

  fstype = "nfs";		/* Default filesystem type */

  /* Scan for -fstype= option, which isn't a real mount option */

  noptions = strdup(options);
  if ( !noptions ) {
    syslog(LOG_ERR, MODPREFIX "strdup: %m");
    free(options);
    return 1;
  }

  p = options;

  for ( q = strtok(noptions, ",") ; q ; q = strtok(NULL, ",") ) {
    if ( !strncmp(q, "fstype=", 7) ) {
      fstype = alloca(strlen(q+7)+1);
      if ( !fstype ) {
	syslog(LOG_ERR, MODPREFIX "alloca: %s",fstype);
	return 1;
      }
      strcpy(fstype, q+7);
    } else {
      l = strlen(q);
      memcpy(p, q, l);
      p += l;
      *p = ',';
      p++;
    }
  }

  if ( p > options ) {
    --p;			/* Delete last comma */
    *p = '\0';
  } else {
    free(options);		/* No options - mount modules want NULL here */
    options = NULL;
  }
  free(noptions);

  if ( !strcmp(fstype, "nfs") ) {
    rv = mount_nfs->mount_mount(root, name, name_len, ent, fstype, options,
				mount_nfs->context);
  } else {
    /* Generic mount routine */
    rv = do_mount(root, name, name_len, ent, fstype, options);
  }

  if (options) free(options);
  return rv;
}

int parse_done(void *context)
{
  int rv = 0;
  struct parse_context *ctxt = (struct parse_context *)context;

  if ( --init_ctr == 0 ) {
    rv = close_mount(mount_nfs);
    mount_nfs = NULL;
  }
  kill_context(ctxt);
  return rv;
}
