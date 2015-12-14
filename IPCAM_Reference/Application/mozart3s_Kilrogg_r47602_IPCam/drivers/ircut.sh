#bin/sh
#rmmod ircut.ko
mknod /dev/ircut c 100 0
insmod ircut.ko controlmode=2  
