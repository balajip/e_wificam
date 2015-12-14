########################################################
# 
#   Makefile for MPEG-4 AAC Encoder Library
#   Copyright (c) 2006 by ______.
#
########################################################
# You only have to revise this section. 
# If you have more than one device driver to include, please use DRV_0_DIR, DRV_1_DIR, ....
LIB_NAME		= aac4enc
TARGET			= aac4enc
TARGET_PROFILE		= aac4enc_profile
TARGET_FRAME_PROFILE	= aac4enc_frame_profile
TARGET_DEBUG		= aac4enc_debug
VERSION		= $(shell sed -n \
			  '/AAC4ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  AAC4Enc.h)
MAJOR_VERSION		= $(shell sed -n \
			  '/AAC4ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
			  AAC4Enc.h)

LIB_DIR		= .
OBJECTS  	= AAC4Enc_App.o 

MEMMGR_DIR	    	= ../MemMgr_Lib
MASSMEMACCESS_DIR	= ../MassMemAccess_Lib
PBENGINE_DIR		= ../PBEngine_Lib
TIMER_DIR		= ../Timer_Lib

# You don't have to change the following sections
STATIC_TARGET   = lib$(LIB_NAME).a
DYN_LINK_NAME   = lib$(LIB_NAME).so
DYNAMIC_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')
DYNAMIC_PROFILE_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').profile
DYNAMIC_DEBUG_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.').debug

CROSS_COMPILE	= arm-linux-
CC				= $(CROSS_COMPILE)gcc
AR				= $(CROSS_COMPILE)ar
CINCS			= -I$(LIB_DIR) -I$(TIMER_DIR) -I$(MEMMGR_DIR) -I$(PBENGINE_DIR)
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG
LINCS			= -L$(LIB_DIR) -L$(MEMMGR_DIR) -L$(MASSMEMACCESS_DIR) -L$(PBENGINE_DIR) -L$(TIMER_DIR)
LDFLAGS			= -l$(LIB_NAME) -lpbengine -lmemmgr -lmassmemaccess -ltimer

# What this makefile does
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
	$(RM) $(TARGET)* $(OBJECTS)

