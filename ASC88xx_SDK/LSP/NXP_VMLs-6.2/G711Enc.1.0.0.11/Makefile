########################################################
# 
#   Makefile for Image Backend Process Application
#   Copyright (c) 2007 by ______.
#
########################################################

# You only have to revis this section. 
TARGET			= g711enc
TARGET_PROFILE		= g711enc_profile
TARGET_DEBUG		= g711enc_debug
LIB_NAME		= g711enc
LIB_DIR			= ../G711Enc_Lib
OBJECTS  		= G711Enc_App.o
VERSION		= $(shell sed -n \
			  '/G711ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  G711Enc.h)
MAJOR_VERSION           = $(shell sed -n \
                          '/G711ENC_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
                          G711Enc.h)
# Usually, you don't have to change the following sections
TIMER_DIR		= ../Timer_Lib
ARCH            = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__

#LIBS			= lib$(LIB_NAME).a libtimer.a

CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -I./ -I$(LIB_DIR) -I$(TIMER_DIR)
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG -D$(ARCH)
LINCS			= -L. -L$(LIB_DIR) -L$(TIMER_DIR)
LDFLAGS			= -l$(LIB_NAME) -ltimer

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

