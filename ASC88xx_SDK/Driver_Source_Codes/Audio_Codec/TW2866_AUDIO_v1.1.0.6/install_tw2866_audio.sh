#!bin/sh

#DRIVER_PATH=/home/james.lin/PROJ/Mozart/asoc/Mozart_VideoServer
DRIVER_PATH=`pwd`
LIB_PATH=/home/evelyn/Bach_ALSA/ALSA_app/ALSA_Lib/shared_lib/lib
CONF_PATH=/home/evelyn/Bach_ALSA/ALSA_app/ALSA_Lib/shared_lib/share/alsa
AUDIOCTRL_PATH=/home/evelyn/AudioCtrl2/AudioCtrl_App
KERNEL_I2C_DRIVER_PATH=/home/evelyn/Mozart_Kernel_2.6.28.9/linux-2.6.28.9/drivers/i2c

mount_as_tmpfs /lib
mount_as_tmpfs /etc
cp $LIB_PATH/libasound.so.2 /lib/
mkdir /etc/ALSA
cp $CONF_PATH/alsa.conf /etc/ALSA

#cd /home/Mozart/drivers
#sh bgh26.sh
#./Godshand.bin -w -a 0x4f800078 -v 0x802

cd $KERNEL_I2C_DRIVER_PATH
insmod algos/i2c-algo-bit.ko
insmod busses/i2c-gpio.ko bus_num=2 scl0=6 sda0=7 scl1=12 sda1=13

cd $DRIVER_PATH
#sh gpio_i2c.sh
sh snddevices
insmod TW2866_AUDIO.ko CodecNum=2

cd $AUDIOCTRL_PATH

#echo "config bus0 0x34"
#./audioctrl_0x34 -s
#./audioctrl_0x34 -e 1 -t 0 -m 1

#echo "config bus1 0x36"
#./audioctrl_0x36 -d /dev/gpio_i2c1 -s
#./audioctrl_0x36 -d /dev/gpio_i2c1 -e 1 -m 1 -t 0

#echo "config bus1 0x34"
#./audioctrl_0x34 -d /dev/gpio_i2c1 -s
#./audioctrl_0x34 -d /dev/gpio_i2c1 -e 1 -m 1 -t 0

#echo "config bus2 0x36"
#./audioctrl_0x36 -d /dev/gpio_i2c2 -s
#./audioctrl_0x36 -d /dev/gpio_i2c2 -e 1 -m 1 -t 0

#echo "config bus2 0x34"
#./audioctrl_0x34 -d /dev/gpio_i2c2 -s
#./audioctrl_0x34 -d /dev/gpio_i2c2 -e 1 -m 1 -t 0
