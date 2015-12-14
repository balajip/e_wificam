#!/bin/bash
MODNAME="Godshand"

rmmod $MODNAME

rm /dev/$MODNAME

insmod $MODNAME.ko

MAJOR=`cat /proc/devices | grep $MODNAME | sed -e 's/'$MODNAME'//' | sed -e 's/ //'`

if test -z $MAJOR; then
        echo "The device specified is not found !"
else
        echo "Let's make a node here for $MODNAME with major number $MAJOR"
        mknod /dev/$MODNAME c $MAJOR 0
fi
