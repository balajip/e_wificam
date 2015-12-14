########################################################
#
#   Makefile for Sample Application
#   Copyright (c) 2008 by ______.
#
########################################################

# You only have to revise this section.
# If you have more than one library to include, please use LIB_0_DIR, LIB_1_DIR, ....,
# and LIB_0_NAME, LIB_1_NAME, ....
TARGET			= G726Enc

TARGET_PROFILE		= G726Enc_profile
TARGET_FRAME_PROFILE	= G726Enc_frame_profile
TARGET_DEBUG		= G726Enc_debug

LIB_NAME		= g726enc
LIB_DIR			= ../G726Enc_Lib
OBJECTS  		= G726Enc_App.o

MEMMGR_DIR	    = ../MemMgr_Lib
MASSMEMACCESS_DIR	    = ../MassMemAccess_Lib
PBENGINE_DIR	= ../PBEngine_Lib
TIMER_DIR		= ../Timer_Lib

VERSION		= $(shell sed -n \
			  '/G726ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  G726Enc.h)
MAJOR_VERSION           = $(shell sed -n \
                          '/G726ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
                          G726Enc.h)
                          
# Usually, you don't have to change the following sections
# But if you don't want to use PBEngine or GBEngine library, please remove them in
# the following sections

LIBS			= lib$(LIB_NAME).a libmemmgr.a libpbengine.a libtimer.a

CROSS_COMPILE	= arm-linux-
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CINCS			= -I. -I$(LIB_DIR) -I$(MEMMGR_DIR) -I$(MASSMEMACCESS_DIR) -I$(PBENGINE_DIR) -I$(TIMER_DIR)
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG
LINCS			= -L. -L$(LIB_DIR) -L$(MEMMGR_DIR) -L$(MASSMEMACCESS_DIR) -L$(PBENGINE_DIR) -L$(TIMER_DIR) 
LDFLAGS			= -l$(LIB_NAME) -lpbengine -lmassmemaccess -lmemmgr -ltimer

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
	$(RM) $(TARGET).o $(TARGET) $(OBJECTS) 

