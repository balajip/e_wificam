########################################################
# 
#   Makefile for Video Capture Library
#   Copyright (c) 2007 by ______.
#
########################################################

# You only have to revise this section. 
LIB_NAME	= videocap
VERSION		= $(shell sed -n \
			  '/VIDEOCAP_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/ /g;p}' \
			  VideoCap.h)
LIB_DIR		= .
DRV_DIR		= ../vpl_vic_v12.0.0.3
MEMMGR_DIR	= ../../../LSP/NXP_VMLs-6.2/MemMgr_Lib/
OBJECTS		= VideoCap_Setup.o \
			  VideoCap_Options.o \
			  VideoCap_Mgr.o \
			  VideoCap_Tables.o \
			  VPL_VIC_Tools.o

# You don't have to change the following sections
EDMC_DIR		= ../vpl_edmc_v6.0.0.8
ARCH            = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__
STATIC_TARGET   = lib$(LIB_NAME).a
DYN_LINK_NAME   = lib$(LIB_NAME).so
DYNAMIC_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')
DYNAMIC_PROFILE_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').profile
DYNAMIC_DEBUG_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').debug

CROSS_COMPILE	= arm-linux-
CC				= $(CROSS_COMPILE)gcc
AR				= $(CROSS_COMPILE)ar
CINCS			= -I$(LIB_DIR) -I$(DRV_DIR) -I$(MEMMGR_DIR) -I$(EDMC_DIR)
CFLAGS			= -O3 -DNO_DP -DNDEBUG -D$(ARCH)
LINCS			= 
LDFLAGS			= 

# What this makefile does
all: static 

static: $(STATIC_TARGET)

debug: CFLAGS += -D_DEBUG
debug: $(STATIC_TARGET)

dynamic: CFLAGS += -fpic
dynamic: $(DYNAMIC_TARGET)

profile: CFLAGS += -D__USE_PROFILE__
profile: static

dynamic_profile: CFLAGS += -fpic -D__USE_PROFILE__ -D__USE_SEQ_PROFILE__
dynamic_profile: $(DYNAMIC_PROFILE_TARGET)

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

clean_obj:
	$(RM) $(OBJECTS) *.o

clean:
	$(RM) $(STATIC_TARGET) $(DYN_LINK_NAME).* $(OBJECTS) *.o

.PHONY: all clean static dynamic profile
