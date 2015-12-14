########################################################
# 
#   Makefile for Memory Manager Library
#   Copyright (c) 2005 by ______.
#
########################################################

# You only have to revise this section. 
# If you have more than one device driver to include, please use DRV_0_DIR, DRV_1_DIR, ....
LIB_NAME	= memmgr
VERSION		= $(shell sed -n \
			  '/MEMMGR_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/ /g;p}' \
			  MemMgr.h)
LIB_DIR		= .
DRV_DIR		= ../../../Components/EDMC/Device_Driver/EDMC
OBJECTS		= MemMgr_Setup.o \
			  MemMgr_Options.o \
			  MemMgr_Mgr.o

# You don't have to change the following sections
STATIC_TARGET   = lib$(LIB_NAME).a
DYN_LINK_NAME   = lib$(LIB_NAME).so
DYNAMIC_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')
DYNAMIC_PROFILE_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').profile
DYNAMIC_DEBUG_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').debug

CROSS_COMPILE	= arm-linux-
CC				= $(CROSS_COMPILE)gcc
AR				= $(CROSS_COMPILE)ar
CINCS			= -I$(LIB_DIR) -I$(DRV_DIR)
CFLAGS			= -O3 -DNO_DP -DNDEBUG
LINCS			= 
LDFLAGS			= 

# What this makefile does
all: static 

static: $(STATIC_TARGET)

dynamic: CFLAGS += -fpic
dynamic: $(DYNAMIC_TARGET)

profile: CFLAGS += -D__USE_PROFILE__ -D__USE_SEQ_PROFILE__
profile: static

frame_profile: CFLAGS += -D__USE_PROFILE__
frame_profile: static

dynamic_profile: CFLAGS += -fpic  -D__USE_PROFILE__  -D__USE_SEQ_PROFILE__
dynamic_profile: $(DYNAMIC_PROFILE_TARGET)

debug: CFLAGS += -D_DEBUG
debug: static

dynamic_debug: CFLAGS += -fpic -D_DEBUG
dynamic_debug: $(DYNAMIC_DEBUG_TARGET)

$(STATIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(AR) r $@ $^

$(DYNAMIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

$(DYNAMIC_PROFILE_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

$(DYNAMIC_DEBUG_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

.SUFFIXES: .c

.c.o:
	@ echo "Compiling $<"
	@ $(CC) -c $(CINCS) $(CFLAGS) $(LINCS) $(LDFLAGS) $<

clean:
	$(RM) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(DYNAMIC_PROFILE_TARGET) $(DYNAMIC_DEBUG_TARGET) $(DYN_LINK_NAME).* $(OBJECTS) *.o

.PHONY: all clean static dynamic profile
