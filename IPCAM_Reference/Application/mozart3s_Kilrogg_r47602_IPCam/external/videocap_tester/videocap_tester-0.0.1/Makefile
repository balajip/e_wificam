CC = arm-linux-gcc
LDLIBS = -lmemmgr -lvideocap 
INCLUDES = -I./include
LIBS	= -L./lib
PRODUCT_TYPE = __IPCam__ # __IPCam__ for IPCam, __VideoServer__ for VideoServer
ifdef TYPE
	PRODUCT_TYPE = $(TYPE)	
	
endif

CFLAGS += -fno-strict-aliasing -D__ASM_ARCH_PLATFORM_MOZART_H__  -D$(PRODUCT_TYPE) $(INCLUDES) -Wall -O2 -g
LDFLAGS	= $(LIBS) -Wl,--start-group $(LDLIBS) -Wl,--end-group
APPS = videocap_tester

.SILENT:

all: $(APPS)

videocap_tester: videocap_tester.o
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

.c.o:
	echo "PRODUCT_TYPE: "$(PRODUCT_TYPE)
	echo "  CC    $<"
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(APPS) *.o
