########################################################
# 
#   Makefile for Mass Memory Access Application
#   Copyright (c) 2005 by ______.
#
########################################################

# You only have to revis this section. 
TARGET			= massmemaccess
TARGET_PROFILE		= massmemaccess_profile
TARGET_FRAME_PROFILE	= massmemaccess_frame_profile
TARGET_DEBUG		= massmemaccess_debug
LIB_NAME		= massmemaccess
LIB_DIR			= ../MassMemAccess_Lib
OBJECTS  		= MassMemAccess_App.o
VERSION		= $(shell sed -n \
			  '/MASSMEMACCESS_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  MassMemAccess.h)
MAJOR_VERSION           = $(shell sed -n \
                          '/MASSMEMACCESS_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
                          MassMemAccess.h)
# Usually, you don't have to change the following sections
MEMMGR_DIR	    = ../MemMgr_Lib
TIMER_DIR		= ../Timer_Lib
ARCH		    = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__

LIBS			= lib$(LIB_NAME).a libmemmgr.a libtimer.a

CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -I./ -I$(LIB_DIR) -I$(MEMMGR_DIR) -I$(TIMER_DIR)
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG -D$(ARCH)
LINCS			= -L. -L$(LIB_DIR) -L$(MEMMGR_DIR) -L$(TIMER_DIR)
LDFLAGS			= -l$(LIB_NAME) -lmemmgr -ltimer

MAKE_OPTIONS	=

$(TARGET): $(OBJECTS) 
	$(RM) ./lib$(LIB_NAME).so 
	$(RM) ./lib$(LIB_NAME).so.$(MAJOR_VERSION) 
	ln -s ./lib$(LIB_NAME).so.$(VERSION) ./lib$(LIB_NAME).so
	ln -s ./lib$(LIB_NAME).so.$(VERSION) ./lib$(LIB_NAME).so.$(MAJOR_VERSION)
	@ echo "Generating $(TARGET)"
	@ $(CC) -o $(TARGET) $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)
$(TARGET_PROFILE): $(OBJECTS) 
	$(RM) ./lib$(LIB_NAME).so 
	$(RM) ./lib$(LIB_NAME).so.$(MAJOR_VERSION)
	ln -s ./lib$(LIB_NAME).so.$(VERSION).profile ./lib$(LIB_NAME).so
	ln -s ./lib$(LIB_NAME).so.$(VERSION).profile ./lib$(LIB_NAME).so.$(MAJOR_VERSION)
	@ echo "Generating $(TARGET_PROFILE)"
	@ $(CC) -o $(TARGET_PROFILE) $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)
$(TARGET_FRAME_PROFILE): $(OBJECTS) 
	$(RM) ./lib$(LIB_NAME).so 
	$(RM) ./lib$(LIB_NAME).so.$(MAJOR_VERSION) 
	ln -s ./lib$(LIB_NAME).so.$(VERSION).profile ./lib$(LIB_NAME).so
	ln -s ./lib$(LIB_NAME).so.$(VERSION).profile ./lib$(LIB_NAME).so.$(MAJOR_VERSION)
	@ echo "Generating $(TARGET_FRAME_PROFILE)"
	@ $(CC) -o $(TARGET_FRAME_PROFILE) $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)
$(TARGET_DEBUG): $(OBJECTS) 
	$(RM) ./lib$(LIB_NAME).so
	$(RM) ./lib$(LIB_NAME).so.$(MAJOR_VERSION)
	ln -s ./lib$(LIB_NAME).so.$(VERSION).debug ./lib$(LIB_NAME).so
	ln -s ./lib$(LIB_NAME).so.$(VERSION).debug ./lib$(LIB_NAME).so.$(MAJOR_VERSION)
	@ echo "Generating $(TARGET_DEBUG)"
	@ $(CC) -o $(TARGET_DEBUG) $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)

.SUFFIXES: .c 

.c.o:
	@ echo "Compiling $*.c"
	@ $(CC) $(CFLAGS) $(CINCS) -c $*.c -o $(OBJECTS)

profile: CFLAGS += -D__USE_PROFILE__ 
profile: MAKE_OPTIONS += profile
profile: $(TARGET_PROFILE)

frame_profile: CFLAGS += -D__USE_PROFILE__ 
frame_profile: MAKE_OPTIONS += frame_profile
frame_profile: $(TARGET_FRAME_PROFILE)

debug: CFLAGS += -D_DEBUG
debug: MAKE_OPTIONS += debug
debug: $(TARGET_DEBUG)

clean:
	$(RM) $(TARGET)* $(OBJECTS)

