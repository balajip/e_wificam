DEBUG = n

MODULE		:= gpio_i2c.ko
HEADERFILE	:= gpio_i2c.h

# You normally should not need to change the following
ifeq ($(DEBUG),y)
	EXTRA_CFLAGS += -O -g -D_DEBUG
else
	EXTRA_CFLAGS += -O2 -DNO_DP -DNDEBUG
endif

ifneq ($(KERNELRELEASE),)

	obj-m := gpio_i2c.o

else

	PWD := $(shell pwd)
	KERNELDIR := $(KERNELSRC)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	@ rm -rf *.o *.ko .*.cmd *.mod.c .tmp_versions *.order *symvers

endif
