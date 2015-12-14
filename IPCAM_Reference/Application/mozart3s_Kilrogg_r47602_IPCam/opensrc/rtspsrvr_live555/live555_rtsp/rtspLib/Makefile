##### INCLUDE PATHS = NXP HEADERS + LIVE555 HEADERS
INCLUDES = -I../UsageEnvironment/include -I../groupsock/include -I../liveMedia/include -I../BasicUsageEnvironment/include -I../include 

##### Change the following for your environment: 
CROSS_COMPILE?=		arm-unknown-linux-uclibcgnueabi-
COMPILE_OPTS =		$(INCLUDES) -I. -O2 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DLOCALE_NOT_USED -D ALLOW_RTSP_SERVER_PORT_REUSE=1
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
LINK =			$(CROSS_COMPILE)g++ -o
LINK_OPTS =		
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =	$(LINK_OPTS)
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

#LIB = librtspsrvr_live555.$(LIB_SUFFIX)
#ALL = $(LIB)
#all:	$(ALL)

OBJS = Live555RTSPServer.$(OBJ)  \

ALL = $(OBJS)
all:	$(ALL)

#librtspsrvr_live555.$(LIB_SUFFIX): $(OBJS)
#	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) \
#		$(OBJS)

.$(C).$(OBJ):
	$(C_COMPILER) -c  $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) Live555RTSPServer.cpp



clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

##### Any additional, platform-specific rules come here:
