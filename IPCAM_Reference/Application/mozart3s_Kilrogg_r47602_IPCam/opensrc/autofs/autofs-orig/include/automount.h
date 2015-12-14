#ident "$Id: automount.h,v 1.7 1998/04/01 11:15:47 hpa Exp $"
/*
 * automount.h
 *
 * Header file for automounter modules
 *
 */

#ifndef AUTOMOUNT_H
#define AUTOMOUNT_H

#include <paths.h>
#include "config.h"

/* We MUST have the paths to mount(8) and umount(8) */
#ifndef HAVE_MOUNT
#error Failed to locate mount(8)!
#endif

#ifndef HAVE_UMOUNT
#error Failed to locate umount(8)!
#endif

/* The -s (sloppy) option to mount is good, if we have it... */

#ifdef HAVE_SLOPPY_MOUNT
#define SLOPPYOPT "-s",		/* For use in spawnl() lists */
#define SLOPPY    "-s "		/* For use in strings */
#else
#define SLOPPYOPT
#define SLOPPY
#endif

/* Standard function used by daemon or modules */

int spawnl(int logpri, const char *prog, ...);
int spawnv(int logpri, const char *prog, const char * const *argv);
void reset_signals(void);
int do_mount(const char *root, const char *name, int name_len,
	     const char *what, const char *fstype, const char *options);

/* Prototype for module functions */

/* lookup module */

#define AUTOFS_LOOKUP_VERSION 4

#ifdef MODULE_LOOKUP
int lookup_init(const char *mapfmt, int argc, const char * const *argv,
		void **context);
int lookup_mount(const char *, const char *, int, void *);
int lookup_done(void *);
#endif
typedef int (*lookup_init_t)(const char *, int, const char * const *, void **);
typedef int (*lookup_mount_t)(const char *, const char *, int, void *);
typedef int (*lookup_done_t)(void *);
struct lookup_mod {
  lookup_init_t  lookup_init;
  lookup_mount_t lookup_mount;
  lookup_done_t  lookup_done;
  void *dlhandle;
  void *context;
};
struct lookup_mod *open_lookup(const char *name, const char *err_prefix,
			       const char *mapfmt,
			       int argc, const char * const *argv);
int close_lookup(struct lookup_mod *);

/* parse module */

#define AUTOFS_PARSE_VERSION 3

#ifdef MODULE_PARSE
int parse_init(int argc, const char * const *argv, void **context);
int parse_mount(const char *root, const char *name,
		int name_len, const char *mapent, void *context);
int parse_done(void *);
#endif
typedef int (*parse_init_t)(int, const char * const *,void **);
typedef int (*parse_mount_t)(const char *,const char *,int, const char *,void *);
typedef int (*parse_done_t)(void *);
struct parse_mod {
  parse_init_t  parse_init;
  parse_mount_t parse_mount;
  parse_done_t  parse_done;
  void *dlhandle;
  void *context;
};
struct parse_mod *open_parse(const char *name, const char *err_prefix,
			      int argc, const char * const *argv);
int close_parse(struct parse_mod *);

/* mount module */

#define AUTOFS_MOUNT_VERSION 4

#ifdef MODULE_MOUNT
int mount_init(void **context);
int mount_mount(const char *root, const char *name, int name_len,
		const char *what, const char *fstype, const char *options,
		void *context);
int mount_done(void *context);
#endif
typedef int (*mount_init_t)(void **);
typedef int (*mount_mount_t)(const char *, const char *, int, const char *, const char *, const char *, void *);
typedef int (*mount_done_t)(void *);
struct mount_mod {
  mount_init_t  mount_init;
  mount_mount_t mount_mount;
  mount_done_t  mount_done;
  void *dlhandle;
  void *context;
};
struct mount_mod *open_mount(const char *name, const char *err_prefix);
int close_mount(struct mount_mod *);


#endif /* AUTOMOUNT_H */
