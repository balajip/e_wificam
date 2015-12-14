#
# Makefile for I2S Controller driver under slave mode 
#
ifneq ($(KERNELRELEASE),)
CPPFLAGS	+= -DDEBUG 
CPPFLAGS	+= -Dlinux
obj-m	:= NVP1114A_AUDIO.o

else

#GPIO := gpio_i2c.ko
#GPIO_I2C_DIR = /home/evelyn/Mozart_GPIO_I2C/
KDIR		:= $(KERNELSRC)
#KDIR		:=  /home/evelyn/Mozart_Kernel/test/linux-2.6.20.4-mozart
#KDIR		:= /home/evelyn/Mozart_Kernel_2.6.28.9/linux-2.6.28.9
#KDIR	:= /home/james.lin/PROJ/Mozart/Kernel/clean_kernel/linux-28.9-newpath
KDIRINC = $(KDIR)/include
INC_FLAG = -I$(KDIRINC)
PWD		:= $(shell pwd)
CROSS_COMPILE 	:= arm-linux-
ARCH 		:= arm
CPPFLAGS	+= -v
CPPFLAGS	+= $(INC_FLAG)


default:
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

#gpio_i2c.ko:
#	$(MAKE) -C$(KDIR) M=$(GPIO_I2C_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules


tags:	$(wildcard *.[hc])
	$(CTAGS) -e $(wildcard *.[hc]) $(wildcard linux/*.[hc]) $(wildcard $(KDIR)/include/linux/usb*.h)

endif

clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers
