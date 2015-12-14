# Tool names
CROSS_COMPILE = arm-linux-
#CROSS_COMPILE = arm-uclibc-
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

LINUX_DEBUG = false

export AS AR CC CPP LD NM OBJCOPY OBJDUMP RANLIB SIZE STRINGS STRIP
#
# Makefile for DWC_otg Highspeed USB controller driver
#
CPPFLAGS	+= -DDEBUG

# Use one of the following flags to compile the software in host-only or
# device-only mode.
#CPPFLAGS        += -DDWC_HOST_ONLY
#CPPFLAGS        += -DDWC_DEVICE_ONLY

CPPFLAGS	+= -Dlinux

BACH		:=bach
HAYDN		:=haydn
MOZART		:=mozart
ifeq ($(BOARDTYPE), $(BACH))
	CFLAGS	+= -D_BACH
endif
ifeq ($(BOARDTYPE), $(HAYDN))
	CFLAGS	+= -D_HAYDN
endif
ifeq ($(BOARDTYPE), $(MOZART))
	CFLAGS	+= -D_MOZART
endif

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

GODSHAND_DRIVERDIR = ../Godshand_Driver
CINCLUDEDIR = ./ \
	$(GODSHAND_DRIVERDIR)
CINCS = -I. -I$(GODSHAND_DRIVERDIR)
CPPFLAGS = -Wall $(INC_FLAG) $(CINCS)

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



#default:
#	$(MAKE) -C$(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules


# Files needed for the build
GOBJ = Godshand.o
DRIVER = Godshand.ko

EXE = Godshand.bin
# Make rules

.c.o:
	@ $(CC) $(CFLAGS) $(CPPFLAGS) $(HEADER_OPS) -c $<

BIN = $(EXE)

ALL = $(DRIVER) $(BIN)

all: $(ALL)

Godshand.ko: $(GODSHAND_DRIVERDIR)/*.c $(GODSHAND_DRIVERDIR)/*.h
	@echo "Compiling "
	@ make -C $(GODSHAND_DRIVERDIR)

Godshand.bin: $(GOBJ)
	$(CC) -o Godshand.bin $(CPPFLAGS) $(GOBJ) $(LDFLAGS)
clean:
	@ rm -f $(MODULE) $(GOBJ) $(ALL)
cleanlib:
	@ make clean -C $(GODSHAND_DRIVERDIR)

cleanall:
	@ make clean
	@ make cleanlib


