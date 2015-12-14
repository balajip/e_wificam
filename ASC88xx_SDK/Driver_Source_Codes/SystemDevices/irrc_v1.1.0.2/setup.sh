#bin/sh

rmmod irrc.ko
rm /dev/irrc	
insmod irrc.ko protocol=0
MAJOR=`cat /proc/devices | grep irrc | sed -e 's/irrc//' | sed -e 's/ //'`
mknod /dev/irrc c $MAJOR 0
