#
# Haydn I2S driver  
#


obj-m   := IICCtrl.o

#GPIO := gpio_i2c.ko
#GPIO_I2C_DIR = /home/evelyn/GPIO_I2C/
#KDIR		:=  /home/evelyn/Haydn/Device_Drivers/IMX035/kernel/haydn_kernel
KDIR	:= $(KERNELSRC)

KDIRINC = $(KDIR)/include
INC_FLAG = -I$(KDIRINC)
PWD             := $(shell pwd)
ARCH            := arm
CROSS_COMPILE 	:= arm-linux-
ARCH 		:= arm
CPPFLAGS	+= -v
CPPFLAGS	+= $(INC_FLAG)

default: 
	$(MAKE) -C$(KDIR) M=$(PWD)  ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

#gpio_i2c.ko:
#	$(MAKE) -C$(KDIR) M=$(GPIO_I2C_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules


clean:
	@ rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers


