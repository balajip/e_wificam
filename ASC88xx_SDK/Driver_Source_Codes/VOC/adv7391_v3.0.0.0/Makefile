#
# Makefile for DWC_otg Highspeed USB controller driver
#

ifneq ($(KERNELRELEASE),)
#CPPFLAGS	+= -DDEBUG 

# Use one of the following flags to compile the software in host-only or
# device-only mode.
obj-m	:= adv7391.o

else

KDIR		:= $(KERNELSRC)
PWD			:= $(shell pwd)

CPPFLAGS	+= -v 


default:
	$(MAKE) -C$(KDIR) M=$(PWD) modules

endif

clean:
	@ rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers
