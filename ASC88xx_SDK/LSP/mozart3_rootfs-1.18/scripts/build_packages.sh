#!/bin/sh
#
# Build the source packages for the Haydn Chip EVM.
#
# Most of the commands executed are set to pipe their std/err outputs 
# to /dev/null (> /dev/null 2>&1). Make command are also 
# flagged for 'silent'.
#

#If no parameter provided, notify user of usage.
if [ -z "$1" ]; then
	echo  - build_packages.sh
	echo  - "usage: `basename $0` path/to/rootfs"
	exit 1
fi

if [ ! -e "$1" ]; then
	echo  - build_packages.sh
	echo  - ERROR: Directory \("$1"\) does not exist.
	echo  - Aborting operation.
	exit 1
fi


echo " Building and installing busybox (~4mins)"
cd packages/busybox-1.10.4
cp ../configs/busybox.config .config
make -s > /dev/null 2>&1 || exit 1
make -s CONFIG_PREFIX=$1 install > /dev/null 2>&1 || exit 1
make -s distclean 

INETUTILS='inetd ftpd telnetd rcp'
echo " Building and installing inetutils (~1min)"
cd ../inetutils-1.5
CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip ./configure --host=arm-linux --disable-ftp --disable-rshd --disable-rlogind --disable-syslogd --disable-uucpd > /dev/null 2>&1 || exit 1
make -s > /dev/null 2>&1 || exit 1
# manually install
for tool in ${INETUTILS}
do
	cp -f ${tool}/${tool} $1/usr/sbin/
	arm-linux-strip $1/usr/sbin/${tool}
done
make -s distclean > /dev/null 2>&1

echo " Building and installing dosfstools (~5sec)"
cd ../dosfstools-2.11
make -s all SBINDIR=$1/usr/sbin MANDIR=$1/tmp > /dev/null 2>&1 || exit 1
make -s SBINDIR=$1/usr/sbin MANDIR=$1/tmp install > /dev/null 2>&1 || exit 1
make -s distclean > /dev/null 2>&1
arm-linux-strip $1/usr/sbin/mkdosfs
rm -f $1/tmp/*.vfat.8
rm -f $1/tmp/*.msdos.8

echo " Building and installing pciutils... (~15secs)"
echo "          Building and installing zlib first (~10secs)"
cd ../zlib-1.2.3
CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip ./configure --shared > /dev/null 2>&1 || exit 1
make -s > /dev/null 2>&1 || exit 1
make -s prefix=../../static/tmp install
cp -d ../../static/tmp/lib/* $1/usr/lib/
make -s distclean

echo "          Building and installing pciutils (~5secs)"
cd ../pciutils-2.2.9
CC=arm-linux-gcc CFLAGS="-I$1/../static/tmp/include -O2" LDFLAGS="-L$1/../static/tmp/lib" make > /dev/null 2>&1
arm-linux-strip ./lspci
arm-linux-strip ./setpci
cp lspci $1/usr/bin || exit 1
cp setpci $1/usr/bin || exit 1
make -s distclean

#echo " Building and installing mtd-utils..."
#echo "           Building and installing lzo lib first (~5mins)"
#cd ../lzo-2.03
#CC=arm-linux-gcc ./configure --host=arm-linux > /dev/null 2>&1 || exit 1
#make -s > /dev/null 2>&1
#make -s prefix=$1/../static/tmp install > /dev/null 2>&1 
#make -s distclean > /dev/null 2>&1


#echo "           Building and installing mtd-utils (~10secs)"
#cd  ../mtd-utils-1.1.0
#CC=arm-linux-gcc CFLAGS="-I$1/../static/tmp/include -O2" LDFLAGS="-L$1/../static/tmp/lib" make WITHOUT_XATTR=1 > /dev/null 2>&1
#arm-linux-strip ./arm-linux/mtd_debug
#cp ./arm-linux/mtd_debug $1/usr/bin || exit 1
#make -s clean


rm -rf ../../static/tmp/*

echo " Building and installing strace (~30secs)"
cd ../strace-4.5.20
CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip ./configure --host=arm-linux > /dev/null 2>&1 || exit 1
make -s > /dev/null 2>&1
arm-linux-strip ./strace
cp ./strace $1/usr/bin || exit 1
make -s distclean


echo " Building and installing udev-136 (~30secs)"
cd ../udev-136
CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip ./configure --host=arm-linux > /dev/null 2>&1 || exit 1
make -s > /dev/null 2>&1
arm-linux-strip udev/udevd
arm-linux-strip udev/udevadm
mkdir -p $1/lib/udev/rules.d
cp vtcs/rules.d/* $1/lib/udev/rules.d
cp udev.conf $1/etc/
cp vtcs/vtcs-mod-udev $1/sbin
cp vtcs/start-udev $1/sbin
cp udev/udevd $1/sbin
cp udev/udevadm $1/sbin
make -s distclean > /dev/null 2>&1


echo " Building and installing uboot-envtools (~5secs) "
cd ../uboot-envtools
make -s CROSS_COMPILE=arm-linux- > /dev/null 2>&1
arm-linux-strip ./fw_printenv
cp ./fw_printenv $1/usr/bin
ln -s /usr/bin/fw_printenv $1/usr/bin/fw_setenv
cp ./fw_env.config $1/etc
make -s clean


echo " Building and installing iptables... (~40secs)"
cd ../iptables-1.4.2
CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip ./configure --host=arm-linux --enable-static=y --enable-shared=n > /dev/null 2>&1 || exit 1
make -s CC=arm-linux-gcc > /dev/null 2>&1 || exit 1
arm-linux-strip ./iptables-static
cp ./iptables-static $1/usr/sbin/iptables
make -s distclean


echo " Building and installing wireless_tools"
cd ../wireless_tools.29
make -s CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib > /dev/null 2>&1 || exit 1
cp ./iwconfig $1/usr/bin
cp ./iwlist $1/usr/bin
make -s realclean


echo " Building and installing mtd-utils"
cd ../mtd-utils
make -s > /dev/null 2>&1 || exit 1
arm-linux-strip arm-linux/nandwrite
arm-linux-strip arm-linux/nanddump
arm-linux-strip arm-linux/flash_eraseall
arm-linux-strip arm-linux/mtd_debug
arm-linux-strip arm-linux/ubi-utils/ubiattach
arm-linux-strip arm-linux/ubi-utils/ubimkvol
cp arm-linux/nandwrite $1/usr/bin
cp arm-linux/nanddump $1/usr/bin
cp arm-linux/flash_eraseall $1/usr/bin
cp arm-linux/mtd_debug $1/usr/bin
cp arm-linux/ubi-utils/ubiattach $1/usr/bin
cp arm-linux/ubi-utils/ubimkvol $1/usr/bin
make -s clean


#echo " Building gdb-6.7... (~5min)"
#cd ../gdb-6.7.1
#./configure --target=arm-linux > /dev/null 2>&1 || exit
#make -s > /dev/null 2>&1 || exit 1


#echo " Building and installing gdbserver-6.7... (~10secs)"
#cd ../gdb-6.7.1/gdb/gdbserver
#chmod +x ./configure
#CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip ./configure --target=arm-linux --host=arm-linux > /dev/null 2>&1 || exit 1
#make -s > /dev/null 2>&1 || exit 1
#arm-linux-strip ./gdbserver
#cp ./gdbserver $1/usr/bin
#make -s distclean

#SMBCLIENT='smbmount smbmnt smbumount'
#echo " Building and installing samba client... (~8min)"
#cd ../../../samba-3.0.27a/source
#CC=arm-linux-gcc AR=arm-linux-ar RANLIB=arm-linux-ranlib STRIP=arm-linux-strip CONFIG_SITE=arm-linux.site ./configure --build=i686 --host=arm-linux --with-smbmount --prefix=/etc/samba --disable-cups --with-configdir=/etc/samba --without-ldap --without-ads --without-krb5 --without-sendfile-support --without-winbind > /dev/null 2>&1 || exit 1
#make -s > /dev/null 2>&1 || exit 1

#for tool in $SMBCLIENT
#do
#	rm -f $1/usr/sbin/${tool}
#	arm-linux-strip bin/${tool} 
#	cp bin/${tool} $1/usr/sbin/ || exit 1
#done
#make -s distclean
#mkdir -p $1/etc/samba
#cp -f $1/../packages/configs/smb.conf $1/etc/samba/smb.conf


