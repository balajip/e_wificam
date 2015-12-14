/* include/config.h.  Generated automatically by configure.  */
#ident "$Id: config.h.in,v 1.4 1998/04/02 05:35:53 hpa Exp $"
/* -*- c -*-
 *
 * config.h.in: Pattern file for autofs to be filled in by configure
 *
 */

/* Program paths */
#define HAVE_MOUNT 1
#define PATH_MOUNT "/bin/mount"

#define HAVE_UMOUNT 1
#define PATH_UMOUNT "/bin/umount"

/* #undef HAVE_SMBMOUNT */
/* #undef PATH_SMBMOUNT */

#define HAVE_E2FSCK 1
#define PATH_E2FSCK "/sbin/fsck.ext2"

/* Define this option if mount(8) supports the -s (sloppy) option */
#define HAVE_SLOPPY_MOUNT 1
