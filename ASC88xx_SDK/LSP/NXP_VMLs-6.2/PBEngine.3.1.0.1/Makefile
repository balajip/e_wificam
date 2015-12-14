########################################################
# 
#   Makefile for Put Bits Engine Library
#   Copyright (c) 2005 by ______.
#
########################################################

# You only have to revise this section. 
# If you have more than one device driver to include, please use DRV_0_DIR, DRV_1_DIR, ....
LIB_NAME	= pbengine
VERSION		= $(shell sed -n \
			  '/PBENGINE_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/ /g;p}' \
			  PBEngine.h)
LIB_0_DIR	= .
LIB_1_DIR	= ../../MassMemAccess/MassMemAccess_Lib
LIB_2_DIR	= ../../Sample/MemMgr_Lib
OBJECTS		= PBEngine.o

# You don't have to change the following sections
ARCH		    = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__
STATIC_TARGET   = lib$(LIB_NAME).a
DYN_LINK_NAME   = lib$(LIB_NAME).so
DYNAMIC_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')

CROSS_COMPILE	= arm-linux-
CC				= $(CROSS_COMPILE)gcc
AR				= $(CROSS_COMPILE)ar
CINCS			= -I$(LIB_0_DIR) -I$(LIB_1_DIR) -I$(LIB_2_DIR)
CFLAGS			= -O3 -DNO_DP -DNDEBUG -D$(ARCH)
LINCS			= 
LDFLAGS			= 

# What this makefile does

all: static 

static: $(STATIC_TARGET)

dynamic: CFLAGS += -fpic
dynamic: $(DYNAMIC_TARGET)

profile: CFLAGS += -D__USE_PROFILE__
profile: static

$(STATIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(AR) r $@ $^

$(DYNAMIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

.SUFFIXES: .c

.c.o:
	@ echo "Compiling $<"
	@ $(CC) -c $(CINCS) $(CFLAGS) $(LINCS) $(LDFLAGS) $<

clean:
	$(RM) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(DYN_LINK_NAME).* $(OBJECTS) *.o

.PHONY: all clean static dynamic profile
