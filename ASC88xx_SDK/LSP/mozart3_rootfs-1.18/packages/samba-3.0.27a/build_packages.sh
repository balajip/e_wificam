#!/bin/sh
#
# Build the source packages for the AMCC ship image.
# Packages are then installed using the provided directory
# as a base.
#
# This script should be executed from the ../software/filesystem
# directory.
#
# Most of the commands executed are set to pipe their std/err outputs 
# to /dev/null (> /dev/null 2>&1). Make command are also 
# flagged for 'silent'.
#

#If no parameter provided, notify user of usage.
SMBCLIENT='smbmount smbmnt smbumount'
echo " Building and installing samba client... (~8min)"
cd source
CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip CONFIG_SITE=arm-linux.site ./configure --build=i686 --host=arm-linux --with-smbmount --prefix=/etc/samba --disable-cups --with-configdir=/etc/samba --without-ldap --without-ads --without-krb5 --without-sendfile-support --without-winbind || exit 1
#CONFIG_SITE=arm-linux.site ./configure --build=i686-pc-linux --host=i686-pc-linux  --with-smbmount --prefix=/etc/samba --disable-cups --with-configdir=/etc/samba --without-ldap --without-ads --without-krb5 --without-sendfile-support --without-winbind || exit 1
#CC=ppc-linux-gcc AR=ppc-linux-ar RANLIB=ppc-linux-ranlib STRIP=ppc-linux-strip CONFIG_SITE=arm-linux.site ./configure --build=i686 --host=ppc-linux --with-smbmount --prefix=/etc/samba --disable-cups --with-configdir=/etc/samba --without-ldap --without-ads --without-krb5 --without-sendfile-support --without-winbind || exit 1
make || exit 1
