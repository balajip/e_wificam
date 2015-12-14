#!bin/sh

DRIVER_PATH=/home/james.lin/PROJ/Mozart/Audio/Mozart_Audio_CAT6011
LIB_PATH=/home/james.lin/PROJ/Mozart/Audio/Alsa_Test_App/ALSA_Lib/homed_lib/lib
CONF_PATH=/home/james.lin/PROJ/Mozart/Audio/Alsa_Test_App
KERNEL_I2C_DRIVER_PATH=/home/james.lin/PROJ/Mozart/Kernel/clean_kernel/mozart_380/drivers/i2c

killall aenc adec
rmmod SSM2603 i2c_gpio i2c_algo_bit

mount_as_tmpfs /lib
mount_as_tmpfs /etc
cp $LIB_PATH/libasound.so.2 /lib/
mkdir /etc/ALSA
cp $CONF_PATH/alsa.conf /etc/ALSA

cd $KERNEL_I2C_DRIVER_PATH
insmod algos/i2c-algo-bit.ko
insmod busses/i2c-gpio.ko bus_num=2 scl0=6 sda0=7 scl1=255 sda1=255

cd $DRIVER_PATH
sh snddevices
insmod CAT6011_Audio.ko


