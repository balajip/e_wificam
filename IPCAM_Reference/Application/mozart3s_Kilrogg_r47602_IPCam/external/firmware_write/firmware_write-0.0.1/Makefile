CC = arm-linux-gcc
#LDLIBS = -lmemmgr -lvideocap 
#INCLUDES = -I./include
#LIBS	= -L./lib
PRODUCT_TYPE = __SF__ # __SF__ for Kilrogg Application running on SF flash, __NF__ for Kilrogg Application running on NF flash
ifdef TYPE
	PRODUCT_TYPE = $(TYPE)	
	
endif

APPS = firmware_write

.SILENT:

all: $(APPS)

firmware_write: firmware_write.o
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

.c.o:
	echo "PRODUCT_TYPE: "$(PRODUCT_TYPE)
	echo "  CC    $<"
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(APPS) *.o
