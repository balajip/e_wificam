#
# Makefile for I2S Controller driver under slave mode 
#
ifneq ($(KERNELRELEASE),)
CPPFLAGS	+= -DDEBUG 
CPPFLAGS	+= -Dlinux
obj-m	:= vaudio.o

else


KDIR		:=$(KERNELSRC)
KDIRINC = $(KDIR)/include
INC_FLAG = -I$(KDIRINC)
PWD		:= $(shell pwd)
CROSS_COMPILE 	:= arm-linux-
ARCH 		:= arm
CPPFLAGS	+= -v
CPPFLAGS	+= $(INC_FLAG)


default:
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules


endif

clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers
