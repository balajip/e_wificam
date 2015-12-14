#
# Makefile for I2S Controller driver under slave mode 
#
ifneq ($(KERNELRELEASE),)
CPPFLAGS	+= -DDEBUG 
CPPFLAGS	+= -Dlinux
obj-m	:= SSM2603.o

else

KDIR		:= $(KERNELSRC)
#KDIR		:=  /home/evelyn/Mozart_Kernel/test/linux-2.6.20.4-mozart
#KDIR		:= /home/evelyn/Mozart_Kernel_2.6.28.9/linux-2.6.28.9
KDIRINC = $(KDIR)/include
INC_FLAG = -I$(KDIRINC)
PWD		:= $(shell pwd)
CROSS_COMPILE 	:= arm-linux-
ARCH 		:= arm
CPPFLAGS	+= -v
CPPFLAGS	+= $(INC_FLAG)


default:
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules


tags:	$(wildcard *.[hc])
	$(CTAGS) -e $(wildcard *.[hc]) $(wildcard linux/*.[hc]) $(wildcard $(KDIR)/include/linux/usb*.h)

endif

clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions modules.order Module.symvers
