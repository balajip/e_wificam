# Copyright (C) 2007-2013  VN Inc.
# morgan.chang@vn.com
#
# Template of Makefile for Linux module
#


### Path to the kernel directory
KDIR =
ifeq ("$(origin O)", "command line")
KERNELSRC := $(O)
else ifneq ($(KDIR),)
KERNELSRC := $(KDIR)
endif

CROSS_COMPILE ?= arm-linux-
ARCH ?= arm

clean-files = *.i modules.order

####################### 2nd stage #######################
### included by kbuild
###
ifneq ($(KERNELRELEASE),)

### the name of module
MODULE_NAME = vpl_wdt

### the objects linked into the module
$(MODULE_NAME)-objs += wdt.o

obj-m += $(MODULE_NAME).o

### customized options
override EXTRA_CFLAGS +=

####################### 1st stage #######################
else

all := $(filter-out all Makefile,$(MAKECMDGOALS))

.PHONY: all module $(MAKECMDGOALS)

module:
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PWD) modules
#	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) SUBDIRS=$(PWD) modules
	mv vpl_wdt.ko wdt.ko

all:
#	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PWD) $(all)
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) SUBDIRS=$(PWD) $(all)

$(all): all
	@:

endif
#########################################################
