########################################################
#
#   Makefile for Tamper Detection
#   Copyright (c) 2009 by VN.
#
########################################################

# You only have to revise this section.
TARGET			= tamper_detect_$(SENSOR_NAME)

LIB_NAME		= videocap
LIB_DIR			= ../../libs/VideoCap
INCL                     =../../include
OBJECTS  		= TamperDetect.o
VERSION		= $(shell sed -n \
			  '/VIDEOCAP_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/./g;p}' \
			  VideoCap.h)
MAJOR_VERSION           = $(shell sed -n \
                          '/VIDEOCAP_VERSION/{s/.*(\(.*\)).*/\1/;s/\(.\),.*/\1/;p}' \
                          VideoCap.h)
                          

SENSOR_TYPE	= NullSensor

 
SENSOR_NAME		= $(shell echo $(SENSOR_TYPE) | tr '[A-Z' '[a-z]')
MEMMGR_DIR	    = ../../libs/MemMgr
ARCH            = __ASM_ARCH_PLATFORM_$(shell echo $(PRODUCTNAME) | tr [a-z] [A-Z])_H__

CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -I. -I$(MEMMGR_DIR)/src -I$(LIB_DIR)/src -I$(INCL) 
CFLAGS			= -O3 -Wall -DNO_DP -DNDEBUG -D__$(SENSOR_TYPE)__ -D$(ARCH)
LINCS			= -L. -L$(MEMMGR_DIR)/lib -L$(LIB_DIR)/lib 
LDFLAGS			=  -l$(LIB_NAME) -lmemmgr

 
MAKE_OPTIONS	=

$(TARGET): $(OBJECTS) 
	@ echo "Generating $(TARGET)"
	@ $(CC) -o $(TARGET) $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)

.c.o:
	@ echo "Compiling $*.c"
	@ $(CC) $(CFLAGS) $(CINCS) -c $*.c -o $(OBJECTS)



clean_obj:
	$(RM) $(OBJECTS)
clean:
	$(RM) $(TARGET)* $(OBJECTS)

