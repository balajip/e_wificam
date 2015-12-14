# $Id: Makefile,v 1.5 1999/03/07 22:59:55 hpa Exp $

-include ../Makefile.conf
include ../Makefile.rules

all: rc.autofs

clean:
	rm -f rc.autofs

install: all
	-mv -f $(INSTALLROOT)/etc/auto.master $(INSTALLROOT)/etc/auto.master.old
	install -c auto.master -m 644 $(INSTALLROOT)/etc
	-mv -f $(INSTALLROOT)/etc/auto.misc $(INSTALLROOT)/etc/auto.misc.old
	install -c auto.misc   -m 644 $(INSTALLROOT)/etc
	install -c rc.autofs   -m 755 $(INSTALLROOT)$(initdir)/autofs

rc.autofs: rc.autofs.in
	sed -e "s|@@sbindir@@|$(sbindir)|g" \
	    -e "s|@@initdir@@|$(initdir)|g" < rc.autofs.in > rc.autofs
