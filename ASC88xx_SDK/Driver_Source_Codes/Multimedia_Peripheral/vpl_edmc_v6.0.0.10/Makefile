# Copyright (C) 2007-2014  ______ Inc.
# morgan.chang@______.com
#
# Template of Makefile for Linux module
#


### Path to the kernel directory
KDIR =
ifeq ("$(origin O)", "command line")
override kernelsrc := $(O)
else ifneq ($(KDIR),)
kernelsrc := $(KDIR)
else ifeq ($(KERNELSRC),)
kernelsrc := /usr/src/linux-headers-$(shell uname -r)
else
kernelsrc := $(KERNELSRC)
endif

### cross compile ?
ifneq ($(CROSS_COMPILE),)
cross_compile = CROSS_COMPILE=$(CROSS_COMPILE)
endif
ifneq ($(ARCH),)
arch = ARCH=$(ARCH)
endif

clean-files = *.i modules.order

####################### 2nd stage #######################
### included by kbuild
###
ifneq ($(KERNELRELEASE),)

### the name of module
MODULE_NAME = vpl_edmc

### the objects linked into the module
$(MODULE_NAME)-objs = # reset objs! set objs from the += assignment.
$(MODULE_NAME)-objs +=

obj-m += $(MODULE_NAME).o

### customized options
override EXTRA_CFLAGS += -O2

####################### 1st stage #######################
else

all := $(filter-out all Makefile,$(MAKECMDGOALS))

.PHONY: all module $(MAKECMDGOALS)

module:
	$(MAKE) $(cross_compile) $(arch) -C $(kernelsrc) M=$(PWD) modules
#	$(MAKE) $(cross_compile) $(arch) -C $(kernelsrc) SUBDIRS=$(PWD) modules

all:
#	$(MAKE) $(cross_compile) $(arch) -C $(kernelsrc) M=$(PWD) $(all)
	$(MAKE) $(cross_compile) $(arch) -C $(kernelsrc) SUBDIRS=$(PWD) $(all)

$(all): all
	@:

endif
#########################################################
