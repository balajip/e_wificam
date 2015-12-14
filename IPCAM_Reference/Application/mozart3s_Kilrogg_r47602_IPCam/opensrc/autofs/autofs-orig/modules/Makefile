# $Id: Makefile,v 1.14 2000/06/19 23:55:57 hpa Exp $
#
# Makefile for autofs
#

-include ../Makefile.conf
include ../Makefile.rules

SRCS :=	lookup_file.c  \
	parse_sun.c    \
	mount_generic.c  mount_nfs.c  mount_autofs.c \
	mount_bind.c

MODS :=	lookup_file.so \
	parse_sun.so \
	mount_generic.so mount_nfs.so mount_autofs.so \
	mount_bind.so

ifeq ($(EXT2FS), 1)
  SRCS += mount_ext2.c
  MODS += mount_ext2.so
endif

ifeq ($(HESIOD), 1)
  SRCS += lookup_hesiod.c  parse_hesiod.c
  MODS += lookup_hesiod.so parse_hesiod.so
endif

ifeq ($(NISPLUS), 1)
  SRCS += lookup_nisplus.c
  MODS += lookup_nisplus.so
endif

ifeq ($(LDAP), 1)
  SRCS += lookup_ldap.c
  MODS += lookup_ldap.so
endif

CFLAGS += -I../include -fpic -DAUTOFS_LIB_DIR=\"$(autofslibdir)\" -DPATH_AUTOMOUNT=\"$(sbindir)/automount\"

all: $(MODS)

clean:
	rm -f *.o *.s *.so

# mount_smbfs.so is an obsolete module which must be removed
install: all
	install -d -m 755 $(INSTALLROOT)$(autofslibdir)
	install -c $(MODS) -m 644 $(INSTALLROOT)$(autofslibdir)
	-rm -f $(INSTALLROOT)$(autofslibdir)/mount_smbfs.so

#
# Ad hoc compilation rules for modules which need auxilliary libraries
#
lookup_yp.so: lookup_yp.c
	$(CC) $(SOLDFLAGS) $(CFLAGS) -o lookup_yp.so lookup_yp.c $(LIBNSL)
	$(STRIP) lookup_yp.so

lookup_nisplus.so: lookup_nisplus.c
	$(CC) $(SOLDFLAGS) $(CFLAGS) -o lookup_nisplus.so lookup_nisplus.c \
		$(LIBNSL)
	$(STRIP) lookup_nisplus.so

lookup_hesiod.so: lookup_hesiod.c
	$(CC) $(SOLDFLAGS) $(CFLAGS) $(HESIOD_FLAGS) -o lookup_hesiod.so \
		lookup_hesiod.c $(LIBHESIOD) $(LIBRESOLV)
	$(STRIP) lookup_hesiod.so

lookup_ldap.so: lookup_ldap.c
	$(CC) $(SOLDFLAGS) $(CFLAGS) $(LDAP_FLAGS) -o lookup_ldap.so \
		lookup_ldap.c $(LIBLDAP)
	$(STRIP) lookup_ldap.so
