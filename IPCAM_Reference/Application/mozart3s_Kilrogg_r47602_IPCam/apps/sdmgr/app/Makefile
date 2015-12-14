CC = arm-linux-gcc



LDFLAGS	= $(LIBS) -Wl,--start-group $(LDLIBS) -Wl,--end-group


APPS = sdmgr

.SILENT:

all: $(APPS)

sdmgr: sdmgr.o
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

.c.o:
	echo "  CC    $<"
	#echo "CFLAGS=" $(CFLAGS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(APPS) *.o
