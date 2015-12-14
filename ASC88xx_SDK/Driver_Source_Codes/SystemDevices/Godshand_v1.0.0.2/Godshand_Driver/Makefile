#
# Makefile for DWC_otg Highspeed USB controller driver
#
ifneq ($(KERNELRELEASE),)
CPPFLAGS	+= -DDEBUG 
BACH		:=bach
HAYDN		:=haydn
MOZART		:=mozart
ifeq ($(BOARDTYPE), $(BACH))
	EXTRA_CFLAGS	+= -D_BACH
endif
ifeq ($(BOARDTYPE), $(HAYDN))
	EXTRA_CFLAGS	+= -D_HAYDN
endif
ifeq ($(BOARDTYPE), $(MOZART))
	EXTRA_CFLAGS	+= -D_MOZART
endif


CPPFLAGS	+= -Dlinux
obj-m	:= Godshand.o

else

KDIR		:= $(KERNELSRC)
KDIRINC = $(KDIR)/include
INC_FLAG = -I$(KDIRINC)
PWD		:= $(shell pwd)
CROSS_COMPILE 	:= arm-linux-
ARCH 		:= arm
CPPFLAGS	+= -v -Wall
CPPFLAGS	+= $(INC_FLAG)

# Command paths
ifeq (${LINUX_DEBUG},true)
	DEBFLAGS = -O -g -D__DEBUG -DLMEX2_DEBUG
	HEADER_OPS = 
	LDFLAGS = -lefence
else
	DEBFLAGS = -O2 -DLMEX2_DEBUG
	HEADER_OPS = 
	LDFLAGS =
endif

CPPFLAGS += $(DEBFLAGS)

default:
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
	
endif


clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers


