###############################################
# Generated by Code Generator Version 3.6.0.9 #
###############################################
MODNAME="vpl_dmac"

rmmod vpl_dmac

rm /dev/$MODNAME

insmod vpl_dmac.ko

MAJOR=`cat /proc/devices | grep $MODNAME | sed -e 's/vpl_dmac//' | sed -e 's/ //'`

if test -z $MAJOR; then
	echo "The device specified is not found !"
else
	echo "Let's make a node here for $MODNAME with major number $MAJOR"
	mknod /dev/$MODNAME c $MAJOR 0
fi