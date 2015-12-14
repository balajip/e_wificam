#!bin/sh

DRIVER_PATH=/home/james.lin/PROJ/Mozart/Audio/Mozart_Audio_SSM2603
LIB_PATH=/home/james.lin/PROJ/Mozart/Audio/Alsa_Test_App/ALSA_Lib/homed_lib/lib
CONF_PATH=/home/james.lin/PROJ/Mozart/Audio/Alsa_Test_App
KERNEL_I2C_DRIVER_PATH=/home/james.lin/PROJ/Mozart/Kernel/clean_kernel/mozart_380/drivers/i2c

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
sh gpio_i2c.sh
sh snddevices
insmod SSM2603.ko


