#############################################################################
#
# $Header: $
#
# Copyright (c) 2008 VN Inc. All rights reserved.
#
# $Log: $
#
#
#############################################################################

TARGET = imagegenerator
LIB_DIR = ../ImageGenerator_Lib
APP = ImageGenerator_App
OBJS = ImageGenerator_App.o
LIB = $(LIB_DIR)/lib$(TARGET).a
OUTPUT = output.bin

CC	= gcc

all: make_lib make_app

make_lib:
	@ make -C $(LIB_DIR)

make_app: $(LIB) $(APP).c ImgGen_Cmdline.h
	$(CC) $(APP).c $(LIB) -I$(LIB_DIR) -L$(LIB_DIR) -o $(TARGET) -m32

clean_app:
	@ rm -f $(OBJS) $(TARGET) $(OUTPUT)

clean_lib:
	@ make -C $(LIB_DIR) clean

clean:
	@ make clean_app
	@ make clean_lib

