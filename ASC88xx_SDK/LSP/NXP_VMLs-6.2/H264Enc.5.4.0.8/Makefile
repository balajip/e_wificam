########################################################
#
#   Makefile for MPEG-4/AVC Video Encoder Library
#   Copyright (c) 2008 by ______.
#
########################################################

# You only have to revise this section. 
# If you have more than one library to include, please use LIB_0_DIR, LIB_1_DIR, ...., 
# and LIB_0_NAME, LIB_1_NAME, ....
TARGET			= h264enc
LIB_0_NAME		= h264enc
LIB_NAME		= h264enc
TARGET_FRAME_PROFILE	= h264enc_frame_profile
TARGET_DEBUG		= h264enc_debug
TARGET_PROFILE		= h264enc_profile
LIB_1_NAME		= massmemaccess
LIB_0_DIR		= ../H264Enc_Lib
LIB_1_DIR		= ../MassMemAccess_Lib
OBJECTS  		= H264Enc_App.o
VERSION		= $(shell sed -n \
			  '/H264ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  H264Enc.h)
MAJOR_VERSION           = $(shell sed -n \
                          '/H264ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
                          H264Enc.h)
# Usually, you don't have to change the following sections
# But if you don't want to use MemMgr, PBEngine, GBEngine, or Timer library, please remove them in 
# the following sections
MEMMGR_DIR	    = ../MemMgr_Lib
PBENGINE_DIR	= ../PBEngine_Lib
TIMER_DIR		= ../Timer_Lib
ARCH		    = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__

LIBS			= lib$(LIB_0_NAME).a lib$(LIB_1_NAME).a libmemmgr.a libpbengine.a libtimer.a

CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -I./ -I$(LIB_0_DIR) -I$(LIB_1_DIR) -I$(MEMMGR_DIR) -I$(PBENGINE_DIR) -I$(TIMER_DIR)
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG -D$(ARCH)
LINCS			= -L. -L$(LIB_0_DIR) -L$(LIB_1_DIR) -L$(MEMMGR_DIR) -L$(PBENGINE_DIR) -L$(TIMER_DIR)
LDFLAGS			= -l$(LIB_0_NAME) -lmemmgr -lpbengine -ltimer -l$(LIB_1_NAME)

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

clean_obj:
	$(RM) $(OBJECTS)
clean:
	$(RM) $(TARGET)* $(OBJECTS)

