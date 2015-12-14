TARGET			= aenc_receiver
OBJECTS         = aenc_receiver.o SockSrc.o UnixSockUtil.o
RM = rm
CROSS_COMPILE	= arm-linux-
AS				= $(CROSS_COMPILE)as
AR				= $(CROSS_COMPILE)ar
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
LD				= $(CROSS_COMPILE)ld
NM				= $(CROSS_COMPILE)nm
CINCS			= -I./includes
CFLAGS			= -Wall -O3 -DNO_DP -DNDEBUG

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@ echo "Generating $(TARGET)"
	@ echo "$(OBJECTS)"
	@ $(CC) -o $(TARGET) $(OBJECTS) $(LINCS) $(LDFLAGS) $(CINCS) $(CFLAGS)

		
.SUFFIXES: .c

.c.o:
	@ echo "Compiling $<"
	@ $(CC) -c $(CFLAGS) $(CINCS) $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

cleanall:
	$(RM) $(TARGET) $(OBJECTS)
