#!/bin/bash
MODNAME="Godshand"

#rmmod $MODNAME

insmod $MODNAME.ko

MAJOR=`cat /proc/devices | grep $MODNAME | sed -e 's/Godshand//' | sed -e 's/ //'`


if test -z $MAJOR; then
	echo "The device specified is not found !"
else
	echo "Let's make a node here for $MODNAME with major number $MAJOR"
	#mknod /dev/$MODNAME c $MAJOR 0
    mknod /dev/${MODNAME} c $MAJOR 0
fi