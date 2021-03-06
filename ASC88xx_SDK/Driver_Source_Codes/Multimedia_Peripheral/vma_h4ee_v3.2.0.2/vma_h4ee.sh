################################################
# Generated by Code Generator Version 4.4.0.8 #
################################################
MODNAME="vma_h4ee"

rmmod vma_h4ee

rm /dev/$MODNAME

insmod vma_h4ee.ko

MAJOR=`cat /proc/devices | grep $MODNAME | sed -e 's/vma_h4ee//' | sed -e 's/ //'`

if test -z $MAJOR; then
	echo "The device specified is not found !"
else
	echo "Let's make a node here for $MODNAME with major number $MAJOR"
	mknod /dev/$MODNAME c $MAJOR 0
fi
