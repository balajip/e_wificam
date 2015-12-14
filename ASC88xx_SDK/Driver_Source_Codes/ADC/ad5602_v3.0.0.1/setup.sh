#bin/sh

rmmod IICCtrl.ko
insmod /home/evelyn/IICCtrl/IICCtrl_driver/IICCtrl.ko
rmmod ad5602.ko
insmod ad5602.ko
