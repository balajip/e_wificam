#!/bin/bash
MODNAME="Godshand"

rmmod $MODNAME

insmod $MODNAME.ko

MAJOR=`cat /proc/devices | grep $MODNAME | sed -e 's/'$MODNAME'//' | sed -e 's/ //'`

if test -z $MAJOR; then
        echo "The device specified is not found !"
fi
