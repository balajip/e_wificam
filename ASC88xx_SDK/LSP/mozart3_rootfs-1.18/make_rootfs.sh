#!/bin/bash
#
# make_rootfs.sh
#
# Constructs the base root file system for Mozart3 Chip EVM.
# NOTE: Must have root privileges.
#

VERSION=1
PATCHVERSION=18
EXTRAVERSION=

# Configuration variables.
#  - TARGET = temporary mount point (this is added to the current path)
#  - FSSIZE = this is the ramdisk size in 1k blocks (uncompressed)
#  - MKFS = is the make filesystem command to be used.
# If not defined in environment, define them.
[ ! $TARGET ] &&  TARGET=arm_ramdisk
[ ! $FSSIZE ] && FSSIZE=4096
[ ! $MKFS ] && MKFS='/sbin/mke2fs -F -v -m0'
[ ! $UB_DESC ] && UB_DESC='Mozart3 Ramdisk'

# The current path.
BASE_DIR=`pwd`
CROSS_COMPILE=arm-linux-
# splash 
echo
echo You have executed the 'Mozart3 Ramdisk creation' script.
echo This script creates and populates the ramdisk that is 
echo This script may be useful as a model in creating a 
echo custom ramdisk.
echo The ramdisk file size is: "$FSSIZE"k
echo The ramdisk file will be created  in: $BASE_DIR
echo The ramdisk file will be mounted at: $BASE_DIR/$TARGET
echo
echo Press [ ENTER ] to continue, or CTRL-C to abort.
read

# Verify user id matches super user
if [ $UID -ne 0 ]; then
	echo You must be root for this script to complete successfully.
	exit
fi

# Verify the environment is as expected
if [ ! $CROSS_COMPILE ]; then
	echo To complete this root filesystem build, the Mozart3
	echo EVM tools need to be properly configured.  
	echo The CROSS_COMPILE environment variable does not 
	echo appear to be set.
	exit
fi

# Check for ARCH variable
#if [ ! $ARCH ]; then
#	echo One or more of the packages built by this script
#	echo needs the ARCH variable defined. For an AMCC 
#	echo file system, this should be ARCH=ppc.
#	exit
#fi

# Test that the compiler will execute
"$CROSS_COMPILE"gcc > /dev/null 2>&1
if [ ! $? = 1 ]; then
	echo The "$CROSS_COMPILE"gcc compiler is apparently not 
	echo in the path. The Mozart3 EVM toolchain needs to be 
	echo properly configured before this script is executed.
	exit
fi

# Verify we do not already have the
# ramdisk mounted. If so, notify the user
# we are unmounting and deleting it.
#
# NOTE: efficency has been removed for
# readability.
echo Checking for mounted $TARGET
mount | grep -q $TARGET
if [ $? -eq 0 ]; then
	echo $TARGET is currently mounted.
	echo Unmounting $TARGET.
	umount $BASE_DIR/$TARGET
	if [ -d $BASE_DIR/$TARGET ]; then
		rm -rf $BASE_DIR/$TARGET
	fi
fi
echo Checking for ramdisk and ramdisk.gz files
if [ -f ramdisk ]; then
	echo Deleting ramdisk file
	rm -f ramdisk
fi
if [ -f ramdisk.image.gz ]; then
	echo Deleting ramdisk.image.gz file
 	rm -f ramdisk.image.gz
fi
if [ -f ramdisk.img ]; then
	echo Deleting U-Boot ramdisk.img file
	rm -f ramdisk.img 
fi
if [ -f rootfs.sqfs ]; then
	echo Deleting rootfs.sqfs file
	rm -f rootfs.sqfs 
fi


# Create the ramdisk file. This file is initially  
# zero-filled for compression benefits.
echo Creating ramdisk file
dd if=/dev/zero of=ramdisk.image bs=1k count=$FSSIZE > /dev/null 2>&1
$MKFS ramdisk.image > /dev/null 2>&1

# Create a temporary mount point and mount
# the ramdisk as a loop device.
echo Creating temporary mount point: $BASE_DIR/$TARGET
mkdir $BASE_DIR/$TARGET
echo Mounting ramdisk to $BASE_DIR/$TARGET
mount -o loop ramdisk.image $BASE_DIR/$TARGET

# Populate the ramdisk.
echo Populating ramdisk...
# First, make the base directory structure.
mkdir $BASE_DIR/$TARGET/bin
#mkdir -p $BASE_DIR/$TARGET/dev/pts
mkdir $BASE_DIR/$TARGET/dev
mkdir $BASE_DIR/$TARGET/etc
mkdir $BASE_DIR/$TARGET/home
mkdir $BASE_DIR/$TARGET/lib
mkdir $BASE_DIR/$TARGET/proc
mkdir $BASE_DIR/$TARGET/root
mkdir $BASE_DIR/$TARGET/sbin
mkdir $BASE_DIR/$TARGET/tmp
mkdir $BASE_DIR/$TARGET/var
mkdir $BASE_DIR/$TARGET/sys
mkdir $BASE_DIR/$TARGET/share
mkdir -p $BASE_DIR/$TARGET/usr/share
mkdir -p $BASE_DIR/$TARGET/usr/sbin
mkdir -p $BASE_DIR/$TARGET/usr/bin
mkdir -p $BASE_DIR/$TARGET/usr/lib
mkdir -p $BASE_DIR/$TARGET/mnt/flash
mkdir -p $BASE_DIR/$TARGET/mnt/ramdisk
mkdir -p $BASE_DIR/$TARGET/mnt/samba
mkdir -p $BASE_DIR/$TARGET/mnt/nand

# Add our basic dev nodes.
# If the kernel is configured to use devfs,
# these nodes will be ignored or removed.
echo Calling script to make /dev nodes
./scripts/make_devs.sh $BASE_DIR/$TARGET/dev

# Make certain we are in our base directory.
cd $BASE_DIR

# Copy our static files into the ramdisk.
cp -a ./static/etc $BASE_DIR/$TARGET
cp -a ./static/lib $BASE_DIR/$TARGET
cp -a ./static/usr $BASE_DIR/$TARGET
cp -a ./static/sbin/* $BASE_DIR/$TARGET/sbin/.

# Generate version file
echo ${VERSION}.${PATCHVERSION}${EXTRAVERSION} > $BASE_DIR/$TARGET/etc/.version_of_filesystem

# Create the links we need.
cd $BASE_DIR/$TARGET/var
ln -s /tmp lock
ln -s /tmp log
ln -s /tmp run
ln -s /tmp tmp
cd $BASE_DIR/$TARGET/usr/share
ln -s /tmp empty
cd $BASE_DIR/$TARGET
ln -s ./bin/busybox init
cd $BASE_DIR

# Build and install the packages.
echo Calling script to build and install packages
./scripts/build_packages.sh $BASE_DIR/$TARGET

# Do cleanup: unmount the ramdisk; remove
# the temporary mount point; compress the
# the ramdisk.
./scripts/gen_initramfs_list.sh -o rootfs.cpio.gz $TARGET
./mksquashfs $BASE_DIR/$TARGET rootfs.sqfs > /dev/null 2>&1 || exit 1
chmod +r rootfs.sqfs
gunzip rootfs.cpio.gz -f
echo "Cleaning up"
umount $TARGET
rm -rf $TARGET
gzip -9 ramdisk.image
chown $USER ramdisk.image.gz

echo Done.

