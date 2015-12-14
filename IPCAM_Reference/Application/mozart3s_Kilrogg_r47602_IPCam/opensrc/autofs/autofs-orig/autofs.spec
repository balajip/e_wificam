# $Id: autofs.spec,v 1.5 1999/03/07 23:04:23 hpa Exp $
Summary: autofs daemon
Name: autofs
%define version 3.1.4
Version: %{version}
Release: 1
Copyright: GPL
Group: Networking/Daemons
Source: ftp://ftp.kernel.org/pub/linux/daemons/autofs/autofs-%{version}.tar.gz
Buildroot: /var/tmp/autofs-tmp
Prereq: chkconfig
Requires: /bin/bash mktemp sed textutils sh-utils grep /bin/ps
Summary(de): autofs daemon 
Summary(fr): d�mon autofs
Summary(tr): autofs sunucu s�reci
Summary(sv): autofs-daemon

%description
autofs is a daemon which automatically mounts filesystems when you use
them, and unmounts them later when you are not using them.  This can
include network filesystems, CD-ROMs, floppies, and so forth.

%description -l de
autofs ist ein D�mon, der Dateisysteme automatisch montiert, wenn sie 
benutzt werden, und sie sp�ter bei Nichtbenutzung wieder demontiert. 
Dies kann Netz-Dateisysteme, CD-ROMs, Disketten und �hnliches einschlie�en. 

%description -l fr
autofs est un d�mon qui monte automatiquement les syst�mes de fichiers
lorsqu'on les utilise et les d�monte lorsqu'on ne les utilise plus. Cela
inclus les syst�mes de fichiers r�seau, les CD-ROMs, les disquettes, etc.

%description -l tr
autofs, kullan�lan dosya sistemlerini gerek olunca kendili�inden ba�lar
ve kullan�mlar� sona erince yine kendili�inden ��zer. Bu i�lem, a� dosya
sistemleri, CD-ROM'lar ve disketler �zerinde yap�labilir.

%description -l sv
autofs �r en daemon som mountar filsystem n�r de anv�nda, och senare
unmountar dem n�r de har varit oanv�nda en best�md tid.  Detta kan
inkludera n�tfilsystem, CD-ROM, floppydiskar, och s� vidare.

%prep
%setup -q

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=/usr
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
mkdir -p $RPM_BUILD_ROOT/usr/sbin
mkdir -p $RPM_BUILD_ROOT/usr/lib/autofs
mkdir -p $RPM_BUILD_ROOT/usr/man/man5
mkdir -p $RPM_BUILD_ROOT/usr/man/man8

make install INSTALLROOT=$RPM_BUILD_ROOT
make install_samples INSTALLROOT=$RPM_BUILD_ROOT
install -m 755 -d $RPM_BUILD_ROOT/misc

%clean
rm -rf $RPM_BUILD_ROOT

%post
chkconfig --add autofs

%postun
if [ "$1" = 0 ] ; then
  chkconfig --del autofs
fi

%files
%defattr(-,root,root)
%doc COPYRIGHT NEWS README TODO
%config /etc/rc.d/init.d/autofs
%config(missingok) /etc/auto.master
%config(missingok) /etc/auto.misc
/usr/sbin/automount
%dir /misc
/usr/lib/autofs
/usr/man/*/*

%changelog
$Log: autofs.spec,v $
Revision 1.5  1999/03/07 23:04:23  hpa
Remove silly commented-out patch line

Revision 1.4  1999/03/07 22:34:02  hpa
Added (missingok) to auto.master.

Revision 1.3  1998/11/05 04:06:00  hpa
Regularized the logs.

Revision 1.2  1998/11/05 04:00:50  hpa
Add RCS Id/Log tags.

Revision 1.1  1998/11/05 01:13:10  hpa
Imported spec file from RedHat Rawhide SRPM 3.1.1-8;
adjusted to be closer to 3.1.4 recommended use; not
done yet, however.

RedHat logs:

* Tue Oct  6 1998 Bill Nottingham <notting@redhat.com>
- fix bash2 breakage in init script

* Sun Aug 23 1998 Jeff Johnson <jbj@redhat.com>
- typo in man page.

* Mon Jul 20 1998 Jeff Johnson <jbj@redhat.com>
- added sparc to ExclusiveArch.

* Thu May 07 1998 Prospector System <bugs@redhat.com>
- translations modified for de, fr, tr

* Thu Apr 30 1998 Cristian Gafton <gafton@redhat.com>
- updated to 3.1.1

* Wed Apr 22 1998 Michael K. Johnson <johnsonm@redhat.com>
- enhanced initscripts

* Fri Dec 05 1997 Michael K. Johnson <johnsonm@redhat.com>
- Link with -lnsl for glibc compliance.

* Thu Oct 23 1997 Michael K. Johnson <johnsonm@redhat.com>
- exclusivearch for i386 for now, since our kernel packages on
  other platforms don't include autofs yet.
- improvements to initscripts.

* Thu Oct 16 1997 Michael K. Johnson <johnsonm@redhat.com>
- Built package from 0.3.14 for 5.0
