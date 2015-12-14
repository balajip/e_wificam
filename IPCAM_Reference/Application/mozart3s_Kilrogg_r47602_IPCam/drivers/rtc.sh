MODNAME="rtc"


MAJOR=`cat /proc/devices | grep $MODNAME | sed -e 's/rtc//' | sed -e 's/ //'`

if test -z $MAJOR; then
	echo "The device specified is not found !"
else
	echo "Let's make a node here for $MODNAME with major number $MAJOR"
	mkdir /dev/misc
	mknod /dev/misc/$MODNAME c $MAJOR 0
fi