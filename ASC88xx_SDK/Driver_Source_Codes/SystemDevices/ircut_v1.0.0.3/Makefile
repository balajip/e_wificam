
obj-m	:= ircut.o

PWD := $(shell pwd)
KDIR	:= $(KERNELSRC) 
CROSS_COMPILE := arm-linux-
ARCH := arm

default:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	@ rm -f *.o *.ko .ircut* modules.order Module.symvers ircut.mod.c
