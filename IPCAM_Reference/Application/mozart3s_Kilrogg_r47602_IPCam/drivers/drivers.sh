#!/bin/sh
SENSORTYPE=#SENSORTYPE#
DRIVER_DIR=/drivers

#DISABLE_KERNEL_MSG#echo "0 0 0 0" > /proc/sys/kernel/printk


cd $DRIVER_DIR

sh $DRIVER_DIR/rtc.sh &

#mknod /dev/sda b 8 0
#mknod /dev/sda1 b 8 1
#mknod /dev/sdb b 8 16
#mknod /dev/sdb1 b 8 17

if [ -f $DRIVER_DIR/vpl_mmc.ko ];then
  insmod $DRIVER_DIR/vpl_mmc.ko &
#  mknod /dev/mmcblk0 b 179 0
#  mknod /dev/mmcblk0p1 b 179 1
#  mknod /dev/mmcblk1 b 179 8 
#  mknod /dev/mmcblk1p1 b 179 9
fi

#64 for all, 80 for 5M , 5M: cat /proc/cmdline from mem=64M to mem=48M
#insmod $DRIVER_DIR/vpl_edmc.ko dwMediaMemSize=#MediaMemSize#  & 
sh $DRIVER_DIR/vpl_edmc.sh &

#insmod $DRIVER_DIR/vpl_dmac.ko &
sh $DRIVER_DIR/vpl_dmac.sh &

if [ -f $DRIVER_DIR/i2c-algo-bit.ko ];then
  insmod $DRIVER_DIR/i2c-algo-bit.ko
fi

if [ -f $DRIVER_DIR/i2c-gpio.ko ];then
# Mozart controls the auido codec (SSM2603) through i2c bus1. The control pins of the bus  is gpio #12 and #13.
# According to  default setting of gpio on Mozart, gpio #6 and #7 is as the control pins of VIC for capturing vidoe data from sensor or video chip.
insmod $DRIVER_DIR/i2c-gpio.ko  #I2CGPIO_ARG# &
fi
 
if [ -f $DRIVER_DIR/gpio_i2c.sh ];then
	sh $DRIVER_DIR/gpio_i2c.sh 
fi

if [ -f $DRIVER_DIR/snddevices ];then
	sh $DRIVER_DIR/snddevices &
fi

if [  -f  $DRIVER_DIR/#AUDIO_DRIVER_NAME#  ];then 	 
	   insmod $DRIVER_DIR/#AUDIO_DRIVER# 	 &
fi

sh $DRIVER_DIR/vma_dce.sh &

if [ -f $DRIVER_DIR/Godshand.sh ];then
	sh $DRIVER_DIR/Godshand.sh &
fi

if [ -f $DRIVER_DIR/vma_ibpe.ko ];then
	#insmod $DRIVER_DIR/vma_ibpe.ko &
	sh $DRIVER_DIR/vma_ibpe.sh &
fi

#insmod $DRIVER_DIR/vma_ire.ko &
sh $DRIVER_DIR/vma_ire.sh &

if [ -f $DRIVER_DIR/vma_jebe.ko ];then
	#insmod $DRIVER_DIR/vma_jebe.ko &
	sh $DRIVER_DIR/vma_jebe.sh &
fi
if [ -f $DRIVER_DIR/vma_h4ee.ko ];then
	#insmod $DRIVER_DIR/vma_h4ee.ko &
	sh $DRIVER_DIR/vma_h4ee.sh &
fi

if [ -f $DRIVER_DIR/vma_meae.ko ];then
	#insmod $DRIVER_DIR/vma_meae.ko &
	sh $DRIVER_DIR/vma_meae.sh &
fi



#insmod $DRIVER_DIR/buff_mgr.ko #BUFMGR_ARG# &
sh $DRIVER_DIR/buff_mgr.sh &


insmod $DRIVER_DIR/IICCtrl.ko 

#===Installation of VIC and sensor related drivers=== 
#			 insmod sensor.ko
#            sh vpl_vic.sh
#            insmod ad5602.ko
#            insmod AutoExposure.ko
#            insmod autofocus.ko
#            insmod AutoWhiteBalance.ko
#            insmod nagano.ko

if [ -f  $DRIVER_DIR/$SENSORTYPE.ko  ];then
  insmod $DRIVER_DIR/$SENSORTYPE.ko #SENSOR_ARG# 
fi

if [ -f $DRIVER_DIR/vpl_vic.sh ];then
  sh $DRIVER_DIR/vpl_vic.sh
fi

if [ -f $DRIVER_DIR/ad5602.ko ];then
	insmod $DRIVER_DIR/ad5602.ko
elif [ -f $DRIVER_DIR/ap1513.ko ];then
	insmod $DRIVER_DIR/ap1513.ko dwAgpoVINNode=0 dwAgpoBRCNode=1
elif [ -f $DRIVER_DIR/pwmiriscontrol.ko ];then
	insmod $DRIVER_DIR/pwmiriscontrol.ko dwAgpoVINNode=0
fi

if [ -f  $DRIVER_DIR/AutoExposure.ko  ];then
	insmod $DRIVER_DIR/AutoExposure.ko &
fi

if [ -f $DRIVER_DIR/autofocus.ko ];then
  insmod $DRIVER_DIR/autofocus.ko 
fi

if [ -f $DRIVER_DIR/AutoWhiteBalance.ko ];then
	insmod $DRIVER_DIR/AutoWhiteBalance.ko &
fi

if [ -f $DRIVER_DIR/nagano.ko ];then
	insmod $DRIVER_DIR/nagano.ko dwFocusSTMA1=0 dwFocusSTMA0=1 dwFocusSTMB1=2 dwFocusSTMB0=3 dwZoomSTMA1=4 dwZoomSTMA0=-1 dwZoomSTMB1=5 dwZoomSTMB0=-1 dwFocusResetNode=8 dwZoomResetNode=9 &
elif [ -f $DRIVER_DIR/lv8044lp.ko ];then
	insmod $DRIVER_DIR/lv8044lp.ko dwZoomPWM=2 dwFocusPWM=3 dwSerialSCLK=4 dwSerialSData=5 dwSerialSTB=10 dwFocusRst=8 dwZoomRst=9 &
fi

#====================================================
if [ -f $DRIVER_DIR/irrc.ko ];then
  insmod $DRIVER_DIR/irrc.ko &
fi

if [ -f $DRIVER_DIR/vpl_tmrc.sh ];then
  sh $DRIVER_DIR/vpl_tmrc.sh &
fi

if [ -f $DRIVER_DIR/wdt.ko ];then
  insmod $DRIVER_DIR/wdt.ko &
fi

if [ -f $DRIVER_DIR/ircut.sh  ];then
	sh $DRIVER_DIR/ircut.sh  &
fi

if [ -f $DRIVER_DIR/vpl_voc.sh ];then
  sh $DRIVER_DIR/vpl_voc.sh &
fi

if [ -f $DRIVER_DIR/dwc_otg.ko ];then
  insmod $DRIVER_DIR/dwc_otg.ko &
fi
cd -

sh $DRIVER_DIR/wifi.sh &
