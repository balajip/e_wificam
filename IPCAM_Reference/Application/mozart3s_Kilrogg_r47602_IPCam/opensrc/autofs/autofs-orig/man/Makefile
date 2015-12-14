# $Id: Makefile,v 1.6 1998/11/05 01:15:17 hpa Exp $

-include ../Makefile.conf
include ../Makefile.rules

GENFILES = $(patsubst %.in, %, $(wildcard *.[58].in))

.SUFFIXES: .in

.in:
	sed -e 's|@@INIT.D@@|$(initdir)|g' < $< > $@

all: $(GENFILES)

install: all
	install -c *.5 -m 644 $(INSTALLROOT)$(mandir)/man5
	install -c *.8 -m 644 $(INSTALLROOT)$(mandir)/man8

clean:
	rm -f $(GENFILES)
