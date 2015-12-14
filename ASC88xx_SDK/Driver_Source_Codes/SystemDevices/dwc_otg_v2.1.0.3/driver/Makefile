#
# Makefile for DWC_otg Highspeed USB controller driver
#


### Path to the kernel directory
KDIR =
ifneq ($(KDIR),)
override KERNELSRC := $(KDIR)
endif

ifeq ($(CPU),andes)
override EXTRA_CFLAGS += -Wa,-mno-small-text
CROSS_COMPILE = nds32le-linux-
ARCH := nds32
else
CROSS_COMPILE = arm-linux-
ARCH := arm
endif

clean-files = *.i modules.order

common_lib = ../dwc_common_port/


####################### 2nd stage #######################
### included by kbuild
###
ifneq ($(KERNELRELEASE),)

# Use the BUS_INTERFACE variable to compile the software for either
# PCI(PCI_INTERFACE) or LM(LM_INTERFACE) bus.
ifeq ($(BUS_INTERFACE),)
#BUS_INTERFACE = -DPCI_INTERFACE
BUS_INTERFACE = -DLM_INTERFACE
endif

### customized options
override EXTRA_CFLAGS += -DDUMMY
override EXTRA_CFLAGS += -DVPL_PATCH
override EXTRA_CFLAGS += -DDDR3_SINGLE_TRANSFER
#override EXTRA_CFLAGS += -DDEBUG
#override EXTRA_CFLAGS += -DFPGA

# Use one of the following flags to compile the software in host-only or
# device-only mode.
#override EXTRA_CFLAGS += -DDWC_HOST_ONLY
#override EXTRA_CFLAGS += -DDWC_DEVICE_ONLY

override EXTRA_CFLAGS += -Dlinux
override EXTRA_CFLAGS += -DDWC_HS_ELECT_TST
#override EXTRA_CFLAGS += -DDWC_EN_ISOC
override EXTRA_CFLAGS += -I$(PORTLIB)
override EXTRA_CFLAGS += -DDWC_LINUX
override EXTRA_CFLAGS += $(CFI)
override EXTRA_CFLAGS += $(BUS_INTERFACE)
#override EXTRA_CFLAGS += -DDWC_DEV_SRPCAP
#override EXTRA_CFLAGS += -Wa,-mno-small-text


### the name of module
MODULE_NAME = dwc_otg

### the objects linked into the module
$(MODULE_NAME)-objs := dwc_otg_driver.o \
                       dwc_otg_attr.o \
                       dwc_otg_cil.o \
                       dwc_otg_cil_intr.o \
                       dwc_otg_pcd_linux.o \
                       dwc_otg_pcd.o \
                       dwc_otg_pcd_intr.o \
                       dwc_otg_hcd.o \
                       dwc_otg_hcd_linux.o \
                       dwc_otg_hcd_intr.o \
                       dwc_otg_hcd_queue.o \
                       dwc_otg_hcd_ddma.o \
                       dwc_otg_adp.o
ifneq ($(COMMLIB),1)
$(MODULE_NAME)-objs += $(common_lib)dwc_cc.o \
                       $(common_lib)dwc_modpow.o \
                       $(common_lib)dwc_dh.o \
                       $(common_lib)dwc_crypto.o \
                       $(common_lib)dwc_notifier.o \
                       $(common_lib)dwc_common_linux.o \
                       $(common_lib)dwc_mem.o
endif
ifneq ($(CFI),)
$(MODULE_NAME)-objs += dwc_otg_cfi.o
endif

obj-m += $(MODULE_NAME).o


####################### 1st stage #######################
else

PWD := $(shell pwd)
export PORTLIB := $(PWD)/$(common_lib)
# Command paths
CTAGS := $(CTAGS)
DOXYGEN := $(DOXYGEN)

all := $(filter-out all cleanall Makefile,$(MAKECMDGOALS))

.PHONY: all module $(MAKECMDGOALS)

ifeq ($(COMMLIB),1)
module: portlib
else
module:
endif
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PWD) modules

all:
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PWD) $(all)

$(all): all
	@:

ifeq ($(COMMLIB),1)
portlib:
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PORTLIB) modules
	cp $(PORTLIB)/Module.symvers $(PWD)/
endif


install: default
	$(MAKE) -C$(KDIR) M=$(PORTLIB) modules_install
	$(MAKE) -C$(KDIR) M=$(PWD) modules_install


docs: $(wildcard *.[hc]) doc/doxygen.cfg
	$(DOXYGEN) doc/doxygen.cfg

tags: $(wildcard *.[hc])
	$(CTAGS) -e $(wildcard *.[hc]) $(wildcard linux/*.[hc]) $(wildcard $(KDIR)/include/linux/usb*.h)

endif
#########################################################

cleanall:
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PWD) clean
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KERNELSRC) M=$(PORTLIB) clean
#clean:
#	rm -rf   *.o *.ko .*cmd *.mod.c *.i .tmp_versions Module.symvers modules.order
