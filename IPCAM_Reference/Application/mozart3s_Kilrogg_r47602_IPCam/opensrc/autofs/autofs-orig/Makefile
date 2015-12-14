# $Id: Makefile,v 1.16 1999/03/07 22:56:33 hpa Exp $
#
# Main Makefile for the autofs user-space tools
#

-include Makefile.conf
include Makefile.rules

.PHONY: daemon all kernel clean samples install install_kernel install_samples
.PHONY: mrproper distclean backup

all:	daemon samples

daemon:
	for i in $(SUBDIRS); do $(MAKE) -C $$i all; done 

kernel:
	if [ -d kernel ]; then $(MAKE) -C kernel all; fi

samples:
	if [ -d samples ]; then $(MAKE) -C samples all; fi

clean:
	for i in $(SUBDIRS) samples kernel; do \
		if [ -d $$i ]; then $(MAKE) -C $$i clean; fi; done 	

install:
	for i in $(SUBDIRS); do $(MAKE) -C $$i install; done 	

install_kernel:
	if [ -d kernel ]; then $(MAKE) -C kernel install; fi

install_samples:
	if [ -d samples ]; then $(MAKE) -C samples install; fi

mrproper distclean: clean
	find . -noleaf \( -name '*~' -o -name '#*' -o -name '*.orig' -o -name '*.rej' -o -name '*.old' \) -print0 | xargs -0 rm -f
	-rm -f include/config.h Makefile.conf config.* .autofs-*
	echo x > .autofs-`cat .version`
	sed -e "s/(\.autofs-[0-9.]\+)/(.autofs-`cat .version`)/" < configure.in > configure.in.tmp
	mv -f configure.in.tmp configure.in
	rm -f configure
	$(MAKE) configure

TODAY := $(shell date +'%Y%m%d')

backup: mrproper
	cd .. ; tar cf - autofs | gzip -9 > autofs-bu-$(TODAY).tar.gz 

configure: configure.in aclocal.m4
	autoconf
	rm -f config.*

configure.in: .version
	-rm -f .autofs-*
	echo x > .autofs-`cat .version`
	sed -e "s/(\.autofs-[0-9.]\+)/(.autofs-`cat .version`)/" < configure.in > configure.in.tmp
	mv -f configure.in.tmp configure.in

-include Makefile.private


