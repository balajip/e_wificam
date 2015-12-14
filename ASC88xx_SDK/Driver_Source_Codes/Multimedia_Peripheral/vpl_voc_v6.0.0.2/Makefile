#==============================================================================
#
#   Makefile for VOC driver module
#   Copyright (c) 2007-2008 by VN.
#
#   Revision History:
#==============================================================================

#==============================================================================
DEBUG = n
PROFILE = n

MODULE		:= vpl_voc.ko
HEADERFILE	:= vpl_voc.h

# You normally should not need to change the following
ifeq ($(DEBUG),y)
	EXTRA_CFLAGS += -O -g -D_DEBUG -D__VOC_USE_IRQ__
else
	EXTRA_CFLAGS += -O2 -DNO_DP -DNDEBUG -D__VOC_USE_IRQ__
endif

ifeq ($(PROFILE),y)
	EXTRA_CFLAGS += -O -D__USE_PROFILE__
else
	EXTRA_CFLAGS += -O2 -DNO_DP -DNDEBUG
endif

ifneq ($(KERNELRELEASE),)

	obj-m := vpl_voc.o

else

	PWD := $(shell pwd)
	KERNELDIR := $(KERNELSRC)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	@ rm -rf *.o *.ko .*.cmd *.mod.c .tmp_versions *.order *symvers

endif
