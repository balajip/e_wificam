# Tool names
CROSS_COMPILE = arm-linux-
AS = $(CROSS_COMPILE)as
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
LD = $(CROSS_COMPILE)ld
NM = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB = $(CROSS_COMPILE)ranlib
SIZE = $(CROSS_COMPILE)size
STRINGS = $(CROSS_COMPILE)strings
STRIP = $(CROSS_COMPILE)strip
KDIR := $(KERNELSRC)
#KDIR := /home/evelyn/Mozart_Kernel_2.6.28.9/linux-2.6.28.9/
#KDIR := /home/evelyn/Mozart_Kernel/for_SSP/linux-2.6.20.4-mozart/
#KDIR := /home/james.lin/PROJ/Mozart/Kernel/test_kernel/NAND/linux-2.6.28.9-nand
KERNELINC		:=  $(KDIR)/include

obj-m   := irrc.o
LINUX_DEBUG = false
#LINUX_DEBUG = true

export AS AR CC CPP LD NM OBJCOPY OBJDUMP RANLIB SIZE STRINGS STRIP

INC_FLAG = -I$(KERNELINC)

EXTRA_CFLAGS = -Wall $(INC_FLAG)

ifeq ($(LINUX_DEBUG),true)
	DEBFLAGS = -O -g -D__DEBUG -DVAUDIO_DEBUG
	HEADER_OPS = 
	LDFLAGS = 
else
	DEBFLAGS = -O2
	HEADER_OPS = 
	LDFLAGS =
endif

EXTRA_CFLAGS += $(DEBFLAGS)
PWD             := $(shell pwd)
ARCH            := arm
INSTALL = install
INSTALL_DIR = ${PRJROOT}/rootfs/drivers
INSTALLHEADER_DIR = ${MODULEINC}


# Make rules


.c.o:
	$(CC) $(EXTRA_CFLAGS) $(HEADER_OPS) -c $<

default: 
	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

	

clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions *.order *.symvers
install: $(MODULE)
	$(INSTALL) -m 644 $(MODULE) $(INSTALL_DIR)
#	$(INSTALL) -m 644 $(HEADERFILE) $(INSTALLHEADER_DIR)
