########################################################
#
#   Makefile for Video Capture Application
#   Copyright (c) 2009 by ______.
#
########################################################

# You only have to revis this section.
TARGET			= videocap_$(SENSOR_NAME)
TARGET_PROFILE		= videocap_profile
TARGET_FRAME_PROFILE	= videocap_frame_profile
TARGET_DEBUG		= videocap_debug

LIB_NAME		= videocap
LIB_DIR			= ../VideoCap_Lib
OBJECTS  		= VideoCap_App.o

VERSION		= $(shell sed -n \
			  '/VIDEOCAP_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  VideoCap.h)
MAJOR_VERSION           = $(shell sed -n \
                          '/VIDEOCAP_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
                          VideoCap.h)
                          

SENSOR_TYPE	= NullSensor
#PRODUCT_TYPE	= VideoServer
PRODUCT_TYPE	= IPCam

LENS_TYPE = NONE
#LENS_TYPE	= ALG3967
#LENS_TYPE	= NE041B0


# Usually, you don't have to change the following sections
ifeq ($(CCD_SENSOR_TYPE),ICX098)
	SENSOR_TYPE = AD9921
	CFLAGS += -D__ICX098__
	TARGET	= videocap_icx098
endif
ifeq ($(CCD_SENSOR_TYPE),ICX445)
	SENSOR_TYPE = AD9921
	CFLAGS += -D__ICX445__
	TARGET	= videocap_icx445
endif
SENSOR_NAME		= $(shell echo $(SENSOR_TYPE) | tr '[A-Z' '[a-z]')
SENSOR_DIR		= ../$(SENSOR_TYPE)_Lib
MOTOR_DIR		= ../Motor_Lib
AUTOFOCUS_DIR		= ../AutoFocus_Lib
AUTOEXPOSURE_DIR	= ../AutoExposure_Lib
IICCTRL_DIR		= ../IICCtrl_Lib
TIMER_DIR		= ../Timer_Lib
MEMMGR_DIR	    = ../MemMgr_Lib
ARCH            = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__
LIBS			= lib$(LIB_NAME).a libmemmgr.a libiicctrl.a lib$(SENSOR_NAME).a

CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -I. -I$(MEMMGR_DIR) -I$(IICCTRL_DIR) -I$(SENSOR_DIR) -I$(LIB_DIR) -I$(MOTOR_DIR) -I$(AUTOFOCUS_DIR) -I$(AUTOEXPOSURE_DIR)
CFLAGS			= -O3 -Wall -DNO_DP -DNDEBUG -D__$(SENSOR_TYPE)__ -D$(ARCH)
LINCS			= -L. -L$(MEMMGR_DIR) -L$(IICCTRL_DIR) -L$(SENSOR_DIR) -L$(LIB_DIR) -L$(MOTOR_DIR) -L$(AUTOFOCUS_DIR) -L$(AUTOEXPOSURE_DIR) -L$(TIMER_DIR)
LDFLAGS			= -l$(LIB_NAME) -lmemmgr -ltimer

ifneq ($(ARCH),__ASM_ARCH_PLATFORM_BACH_H__)

ifeq ($(LENS_TYPE),ALG3967)
	LENS_NAME	= motor
	CFLAGS		+= -D__ALG3967__ -D__USE_AF_LIB__
	LENS_DIR	= ../../Motor/Motor_Lib
	CINCS		+= -I$(MOTOR_DRV_DIR) -I$(MOTOR_DIR)
	LIBS		+= libmotor.a libautofocus.a
	LINCS		+= -L$(MOTOR_DIR)
	LDFLAGS		+= -lmotor -lautofocus
endif
ifeq ($(LENS_TYPE),NE041B0)
	LENS_NAME	= $(shell echo $(LENS_TYPE) | tr '[A-Z' '[a-z]')
	CFLAGS		+= -D__$(LENS_TYPE)__ -D__USE_AF_LIB__
	LENS_DIR	= ../../$(LENS_TYPE)/$(LENS_TYPE)_Lib
	CINCS		+= -I$(LENS_DIR)
	LIBS		+= lib$(LENS_NAME).a libautofocus.a
	LINCS		+= -L$(LENS_DIR)
	LDFLAGS		+= -l$(LENS_NAME) -lautofocus
endif
endif
ifeq ($(CCD_SENSOR_TYPE),ICX098)
	CFLAGS += -D__ICX098__
endif
ifeq ($(CCD_SENSOR_TYPE),ICX445)
	CFLAGS += -D__ICX445__
endif

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

frame_profile: CFLAGS += -D__USE_PROFIL
frame_profile: MAKE_OPTIONS += frame_profile
frame_profile: $(TARGET_FRAME_PROFILE)

debug: CFLAGS += -D_DEBUG
debug: MAKE_OPTIONS += debug
debug: $(TARGET_DEBUG)

clean_obj:
	$(RM) $(OBJECTS)
clean:
	$(RM) $(TARGET)* $(OBJECTS)

