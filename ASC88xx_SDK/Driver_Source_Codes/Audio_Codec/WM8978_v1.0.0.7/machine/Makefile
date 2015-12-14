#
# Makefile for I2S Controller driver under slave mode 
#
ifneq ($(KERNELRELEASE),)
CPPFLAGS	+= -DDEBUG 
CPPFLAGS	+= -Dlinux
obj-m	:= vaudio-evm.o

else

CODEC = wau8822.ko
CODEC_DIR = `pwd`/../codec
PLATFORM = vaudio.ko
PLATFORM_DIR = `pwd`/../platform
MOZART_I2S = vaudio-i2s.ko
#MOZART_I2S_DIR = /opt/misc_share/james.lin/PROJ/Mozart/Audio/Mozart_Audio_WAU8822/Mozart_I2S/
MOZART_I2S_DIR = `pwd`/../platform/Mozart_I2S

KDIR		:= $(KERNELSRC)
KDIRINC = $(KDIR)/include
INC_FLAG = -I$(KDIRINC)
PWD		:= $(shell pwd)
CROSS_COMPILE 	:= arm-linux-
ARCH 		:= arm
CPPFLAGS	+= -v
CPPFLAGS	+= $(INC_FLAG)


default: $(CODEC) $(PLATFORM) $(MOZART_I2S)
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

vaudio-i2s.ko:
	$(MAKE) -C$(KDIR) M=$(MOZART_I2S_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

vaudio.ko:
	$(MAKE) -C$(KDIR) M=$(PLATFORM_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

wau8822.ko:
	$(MAKE) -C$(KDIR) M=$(CODEC_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

endif

clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers
