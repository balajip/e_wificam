#!bin/sh

WORKING_DIR=`pwd`
CODEC_DRIVER=WM8978.ko
CODEC_DRIVER_PATH=$WORKING_DIR/codec
PLATFORM_DRIVER=vaudio.ko
PLATFORM_DRIVER_PATH=$WORKING_DIR/platform
I2S_DRIVER=vaudio-i2s.ko
I2S_DRIVER_PATH=$WORKING_DIR/platform/Mozart_I2S
MACHINE_DRIVER=vaudio-evm.ko
MACHINE_DRIVER_PATH=$WORKING_DIR/machine
#KERNEL_I2C_DRIVER_PATH=/home/evelyn/Mozart_Kernel_2.6.28.9/linux-2.6.28.9/drivers/i2c
KERNEL_I2C_DRIVER_PATH=/home/evelyn/tmp/kernel_i2c

LIB_PATH=/home/james.lin/PROJ/Mozart/Audio/Alsa_Test_App/ALSA_Lib/shared_lib/lib
CONF_PATH=/home/james.lin/PROJ/Mozart/Audio/Alsa_Test_App

mount_as_tmpfs /lib/
cp $LIB_PATH/libasound.so.2 /lib/
mount_as_tmpfs /etc/
mkdir /etc/ALSA
cp $CONF_PATH/alsa.conf /etc/ALSA

cd $KERNEL_I2C_DRIVER_PATH
#insmod algos/i2c-algo-bit.ko
#insmod busses/i2c-gpio.ko
insmod i2c-algo-bit.ko
insmod i2c-gpio.ko bus_num=2 scl0=255 sda0=255 scl1=12 sda1=13
#insmod i2c-gpio.ko

cd $WORKING_DIR
sh snddevices

cd $CODEC_DRIVER_PATH
insmod $CODEC_DRIVER
cd $I2S_DRIVER_PATH
insmod $I2S_DRIVER
cd $PLATFORM_DRIVER_PATH
insmod $PLATFORM_DRIVER
cd $MACHINE_DRIVER_PATH
insmod $MACHINE_DRIVER

