#ident "$Id: module.c,v 1.3 1999/12/17 19:02:47 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *
 *  module.c - common module-management functions
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
#include <syslog.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include "automount.h"

struct lookup_mod *open_lookup(const char *name, const char *err_prefix,
			       const char *mapfmt,
			       int argc, const char * const *argv)
{
  struct lookup_mod *mod;
  char *fnbuf;
  void *dh;
  int *ver;

  mod = malloc(sizeof(struct lookup_mod));
  if ( !mod ) {
    if ( err_prefix ) syslog(LOG_CRIT, "%s%m", err_prefix);
    return NULL;
  }
  fnbuf = alloca(strlen(name) + strlen(AUTOFS_LIB_DIR) + 13);
  if ( !fnbuf ) {
    free(mod);
    if ( err_prefix ) syslog(LOG_CRIT, "%s%m", err_prefix);
    return NULL;
  }
  sprintf(fnbuf, "%s//lookup_%s.so", AUTOFS_LIB_DIR, name);
  
  if ( !(dh = dlopen(fnbuf, RTLD_NOW)) ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%scannot open lookup module %s (%s)",
	     err_prefix, name, dlerror());
    free(mod);
    return NULL;
  }

  if ( !(ver = (int *) dlsym(dh, "lookup_version"))
       || *ver != AUTOFS_LOOKUP_VERSION ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%slookup module %s version mismatch", err_prefix, name);
    dlclose(dh);
    free(mod);
    return NULL;
  }

  if ( !(mod->lookup_init = (lookup_init_t) dlsym(dh, "lookup_init")) ||
       !(mod->lookup_mount = (lookup_mount_t) dlsym(dh, "lookup_mount")) ||
       !(mod->lookup_done = (lookup_done_t) dlsym(dh, "lookup_done")) ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%slookup module %s corrupt", err_prefix, name);
    dlclose(dh);
    free(mod);
    return NULL;
  }

  if ( mod->lookup_init(mapfmt, argc, argv, &mod->context) ) {
    dlclose(dh);
    free(mod);
    return NULL;
  }
  mod->dlhandle = dh;
  return mod;
}

int close_lookup(struct lookup_mod *mod)
{
  int rv = mod->lookup_done(mod->context);
  dlclose(mod->dlhandle);
  free(mod);
  return rv;
}

struct parse_mod *open_parse(const char *name, const char *err_prefix,
			      int argc, const char * const *argv)
{
  struct parse_mod *mod;
  char *fnbuf;
  void *dh;
  int *ver;

  mod = malloc(sizeof(struct parse_mod));
  if ( !mod ) {
    if ( err_prefix ) syslog(LOG_CRIT, "%s%m", err_prefix);
    return NULL;
  }
  fnbuf = alloca(strlen(name) + strlen(AUTOFS_LIB_DIR) + 12);
  if ( !fnbuf ) {
    free(mod);
    if ( err_prefix ) syslog(LOG_CRIT, "%s%m", err_prefix);
    return NULL;
  }
  sprintf(fnbuf, "%s//parse_%s.so", AUTOFS_LIB_DIR, name);
  
  if ( !(dh = dlopen(fnbuf, RTLD_NOW)) ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%scannot open parse module %s (%s)",
	     err_prefix, name, dlerror());
    free(mod);
    return NULL;
  }

  if ( !(ver = (int *) dlsym(dh, "parse_version"))
       || *ver != AUTOFS_PARSE_VERSION ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%sparse module %s version mismatch", err_prefix, name);
    dlclose(dh);
    free(mod);
    return NULL;
  }

  if ( !(mod->parse_init = (parse_init_t) dlsym(dh, "parse_init")) ||
       !(mod->parse_mount = (parse_mount_t) dlsym(dh, "parse_mount")) ||
       !(mod->parse_done = (parse_done_t) dlsym(dh, "parse_done")) ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%sparse module %s corrupt", err_prefix, name);
    dlclose(dh);
    free(mod);
    return NULL;
  }

  if ( mod->parse_init(argc,argv,&mod->context) ) {
    dlclose(dh);
    free(mod);
    return NULL;
  }
  mod->dlhandle = dh;
  return mod;
}

int close_parse(struct parse_mod *mod)
{
  int rv = mod->parse_done(mod->context);
  dlclose(mod->dlhandle);
  free(mod);
  return rv;
}

struct mount_mod *open_mount(const char *name, const char *err_prefix)
{
  struct mount_mod *mod;
  char *fnbuf;
  void *dh;
  int *ver;

  mod = malloc(sizeof(struct mount_mod));
  if ( !mod ) {
    if ( err_prefix ) syslog(LOG_CRIT, "%s%m", err_prefix);
    return NULL;
  }
  fnbuf = alloca(strlen(name) + strlen(AUTOFS_LIB_DIR) + 12);
  if ( !fnbuf ) {
    free(mod);
    if ( err_prefix )
      syslog(LOG_CRIT, "%s%m", err_prefix);
    return NULL;
  }
  sprintf(fnbuf, "%s//mount_%s.so", AUTOFS_LIB_DIR, name);
  
  if ( !(dh = dlopen(fnbuf, RTLD_NOW)) ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%scannot open mount module %s (%s)",
	     err_prefix, name, dlerror());
    free(mod);
    return NULL;
  }

  if ( !(ver = (int *) dlsym(dh, "mount_version"))
       || *ver != AUTOFS_MOUNT_VERSION ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%smount module %s version mismatch", err_prefix, name);
    dlclose(dh);
    free(mod);
    return NULL;
  }

  if ( !(mod->mount_init = (mount_init_t) dlsym(dh, "mount_init")) ||
       !(mod->mount_mount = (mount_mount_t) dlsym(dh, "mount_mount")) ||
       !(mod->mount_done = (mount_done_t) dlsym(dh, "mount_done")) ) {
    if ( err_prefix )
      syslog(LOG_CRIT, "%smount module %s corrupt", err_prefix, name);
    dlclose(dh);
    free(mod);
    return NULL;
  }

  if ( mod->mount_init(&mod->context) ) {
    dlclose(dh);
    free(mod);
    return NULL;
  }
  mod->dlhandle = dh;
  return mod;
}

int close_mount(struct mount_mod *mod)
{
  int rv = mod->mount_done(mod->context);
  dlclose(mod->dlhandle);
  free(mod);
  return rv;
}

