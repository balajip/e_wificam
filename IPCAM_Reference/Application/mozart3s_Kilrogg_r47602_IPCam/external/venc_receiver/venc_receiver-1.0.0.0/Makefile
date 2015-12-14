########################################################
# 
#   Makefile for venc receiver sample application
#   Copyright (c) 2008-2010 by VN Inc.
#
########################################################

TARGET			= venc_receiver
OBJECTS			= venc_receiver.o
RM = rm
CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -Iinclude
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG
LINCS			= -Llib
LDFLAGS			= -lsharedbuffmgr -lmemmgr -lexpat -lxmlwrapper

vpath %.c app
vpath lib%.so	lib
vpath lib%.a	lib

all: $(TARGET)

$(TARGET): $(OBJECTS)  
	@ echo "Generating $(TARGET)"
	@ $(CC) -o $@ $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)
		
.PHONY: all clean

%.o:%.c
	@ echo "Compiling $*.c"
	@ $(CC) $(CFLAGS) $(CINCS) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

