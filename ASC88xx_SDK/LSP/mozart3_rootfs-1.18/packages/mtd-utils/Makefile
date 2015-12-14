
# -*- sh -*-
CROSS=arm-linux-

CPPFLAGS += -I./include $(ZLIBCPPFLAGS) $(LZOCPPFLAGS)
UBIDIR=ubi-utils

ifeq ($(WITHOUT_XATTR), 1)
  CPPFLAGS += -DWITHOUT_XATTR
endif

SUBDIRS = ubi-utils

TARGETS = flash_eraseall nanddump mtd_debug nandwrite 

SYMLINKS =

include common.mk

clean::
	-rm -f $(SYMLINKS)
ifneq ($(BUILDDIR)/.git,)
ifneq ($(BUILDDIR),.)
ifneq ($(BUILDDIR),$(PWD))
	rm -rf $(BUILDDIR)
endif
endif
endif

$(BUILDDIR)/flash_eraseall: $(BUILDDIR)/crc32.o $(BUILDDIR)/flash_eraseall.o

install:: ${TARGETS}
	mkdir -p ${DESTDIR}/${SBINDIR}
	install -m 0755 ${TARGETS} ${DESTDIR}/${SBINDIR}/
	mkdir -p ${DESTDIR}/${MANDIR}/man1
