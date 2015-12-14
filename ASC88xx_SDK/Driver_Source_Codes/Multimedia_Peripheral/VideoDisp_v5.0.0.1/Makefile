########################################################
#
#   Makefile for Video Display Library
#   Copyright (c) 2012 by ______
#
########################################################
LIB_NAME	= videodisp
VERSION		= $(shell sed -n \
			  '/VIDEODISP_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/ /g;p}' \
			  VideoDisp.h)
LIB_DIR		= .
DRV_DIR		= ../vpl_voc_v6.0.0.2
EDMC_DIR	= ../vpl_edmc_v6.0.0.8
MEMMGR_DIR	= ../../../LSP/NXP_VMLs-6.2/MemMgr_Lib/
OBJECTS		= VideoDisp_Mgr.o \
			  VideoDisp_Options.o \
			  VideoDisp_Setup.o \
			  VideoDisp_Tables.o \
			  VPL_VOC_Tools.o
SOURCES		= VideoDisp_Mgr.c \
			  VideoDisp_Options.c \
			  VideoDisp_Setup.c \
			  VideoDisp_Tables.c \
			  VPL_VOC_Tools.c

# You don't have to change the following sections
STATIC_TARGET   = ./ERROR_CODE/lib$(LIB_NAME).a
DYN_LINK_NAME   = lib$(LIB_NAME).so
DYNAMIC_TARGET  = ./ERROR_CODE/$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')
DYNAMIC_PROFILE_TARGET  = ./ERROR_CODE/$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').profile
DYNAMIC_DEBUG_TARGET  = ./ERROR_CODE/$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').debug

NO_ERROR_CODE_STATIC_TARGET   = ./NO_ERROR_CODE/lib$(LIB_NAME).a
NO_ERROR_CODE_DYN_LINK_NAME   = ./NO_ERROR_CODE/lib$(LIB_NAME).so
NO_ERROR_CODE_DYNAMIC_TARGET  = ./NO_ERROR_CODE/$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')
NO_ERROR_CODE_DYNAMIC_PROFILE_TARGET  = ./NO_ERROR_CODE/$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').profile
NO_ERROR_CODE_DYNAMIC_DEBUG_TARGET  = ./NO_ERROR_CODE/$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').debug

SYMBOLIC_STATIC_TARGET   = ./lib$(LIB_NAME).a
SYMBOLIC_DYNAMIC_TARGET  = ./$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')
SYMBOLIC_DYNAMIC_PROFILE_TARGET  = ./$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').profile
SYMBOLIC_DYNAMIC_DEBUG_TARGET  = ./$(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').debug

CROSS_COMPILE	= arm-linux-
CC				= $(CROSS_COMPILE)gcc
AR				= $(CROSS_COMPILE)ar
CINCS			= -I$(DRV_DIR) -I$(EDMC_DIR) -I$(MEMMGR_DIR) -I$(LIB_DIR)
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG
LINCS			=
LDFLAGS			=

# What this makefile does
all: static

static: $(STATIC_TARGET)
static: clean_obj
static: FORCE
static: $(NO_ERROR_CODE_STATIC_TARGET)

dynamic: CFLAGS += -fpic
dynamic: $(DYNAMIC_TARGET)
dynamic: clean_obj
dynamic: FORCE
dynamic: $(NO_ERROR_CODE_DYNAMIC_TARGET)

profile: CFLAGS += -D__USE_PROFILE__ -D__USE_SEQ_PROFILE__
profile: static

frame_profile: CFLAGS += -D__USE_PROFILE__
frame_profile: static

dynamic_profile: CFLAGS += -fpic -D__USE_PROFILE__ -D__USE_SEQ_PROFILE__
dynamic_profile: $(DYNAMIC_PROFILE_TARGET)
dynamic_profile: clean_obj
dynamic_profile: FORCE
dynamic_profile: $(NO_ERROR_CODE_DYNAMIC_PROFILE_TARGET)

debug: CFLAGS += -D_DEBUG
debug: static

dynamic_debug: CFLAGS += -fpic -D_DEBUG
dynamic_debug: $(DYNAMIC_DEBUG_TARGET)
dynamic_debug: clean_obj
dynamic_debug: FORCE
dynamic_debug: $(NO_ERROR_CODE_DYNAMIC_DEBUG_TARGET)

FORCE:CFLAGS += -D__NO_ERROR_CODE__
FORCE:$(SOURCES)
	@ echo "Compiling $^"
	@ $(CC) -c $(CINCS) $(CFLAGS) $(LINCS) $(LDFLAGS) $^

$(STATIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(AR) r $@ $^
	@ if [ ! -s $(SYMBOLIC_STATIC_TARGET) ]; then ln -s $(STATIC_TARGET) $(SYMBOLIC_STATIC_TARGET); fi

$(DYNAMIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^
	@ if [ ! -s $(SYMBOLIC_DYNAMIC_TARGET) ]; then ln -s $(DYNAMIC_TARGET) $(SYMBOLIC_DYNAMIC_TARGET);fi

$(DYNAMIC_PROFILE_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^
	@ if [ ! -s $(SYMBOLIC_DYNAMIC_PROFILE_TARGET) ]; then ln -s $(DYNAMIC_PROFILE_TARGET) $(SYMBOLIC_DYNAMIC_PROFILE_TARGET); fi

$(DYNAMIC_DEBUG_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^
	@ if [ ! -s $(SYMBOLIC_DYNAMIC_DEBUG_TARGET) ]; then ln -s $(DYNAMIC_DEBUG_TARGET) $(SYMBOLIC_DYNAMIC_DEBUG_TARGET); fi

$(NO_ERROR_CODE_STATIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(AR) r $@ $^

$(NO_ERROR_CODE_DYNAMIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

$(NO_ERROR_CODE_DYNAMIC_PROFILE_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

$(NO_ERROR_CODE_DYNAMIC_DEBUG_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

.SUFFIXES: .c

.c.o:
	@ if [ ! -d ./ERROR_CODE ]; then mkdir ./ERROR_CODE; fi
	@ if [ ! -d ./NO_ERROR_CODE ]; then mkdir ./NO_ERROR_CODE; fi
	@ echo "Compiling $<"
	@ $(CC) -c $(CINCS) $(CFLAGS) $(LINCS) $(LDFLAGS) $<

clean_obj:
	$(RM) $(OBJECTS) *.o

clean:
	$(RM) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(DYNAMIC_PROFILE_TARGET) $(DYNAMIC_DEBUG_TARGET) $(NO_ERROR_CODE_STATIC_TARGET) $(NO_ERROR_CODE_DYNAMIC_TARGET) $(NO_ERROR_CODE_DYNAMIC_PROFILE_TARGET) $(NO_ERROR_CODE_DYNAMIC_DEBUG_TARGET) $(DYN_LINK_NAME).* $(OBJECTS) *.o *.a
	$(RM) -r ./ERROR_CODE ./NO_ERROR_CODE

.PHONY: all clean clean_obj static dynamic profile FORCE
