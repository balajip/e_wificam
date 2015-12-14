########################################################
# 
#   Makefile for audioctrl device driver Library
#   Copyright (c) 2004 by VN.
#
########################################################

# You only have to revis this section. 
LIB_NAME	= imagegenerator
VERSION		= $(shell sed -n \
			  '/IMAGEGENERATOR_VERSION/{s/.*(\(.*\)).*/\1/;s/ //g;s/,/ /g;p}' \
			  ImageGenerator.h)
IMAGEGENERATOR_LIBDIR		= .
#IICCTRL_LIBDIR          = ../IICCtrl_Lib

OBJECTS  	= ImageGenerator_Setup.o \
			  ImageGenerator_Access.o

# You don't have to change the following sections
STATIC_TARGET   = lib$(LIB_NAME).a
DYN_LINK_NAME   = lib$(LIB_NAME).so
DYNAMIC_TARGET  = $(DYN_LINK_NAME).$(shell echo $(VERSION) | tr ' ' '.')

#CROSS_COMPILE	= arm-linux-
CC				= gcc
AR				= ar
CINCS			= -I$(IMAGEGENERATOR_LIBDIR)# -I$(IICCTRL_LIBDIR)
CFLAGS			= -O3 -DNO_DP -DNDEBUG -fPIC
#LINCS			= 
#LDFLAGS			= 

# What this makefile does
all: static 

static: $(STATIC_TARGET)

dynamic: CFLAGS += -fpic
dynamic: $(DYNAMIC_TARGET)

profile: CFLAGS += -D__USE_PROFILE__
profile: static

$(STATIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(AR) r $@ $^

$(DYNAMIC_TARGET): $(OBJECTS)
	@ echo "Creating $@"
	$(CC) -shared -Wl,-soname,$(DYN_LINK_NAME).$(word 1, $(VERSION)) -o $@ $^

.SUFFIXES: .c

.c.o:
	@ echo "Compiling $<"
	@ $(CC) -m32 -c $(CINCS) $(CFLAGS) $<

clean:
	$(RM) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(OBJECTS) *.o

.PHONY: all clean static dynamic profile
