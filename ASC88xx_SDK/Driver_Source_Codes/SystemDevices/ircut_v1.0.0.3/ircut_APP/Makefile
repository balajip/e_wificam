CROSS_COMPILE	= arm-linux-
CC		= $(CROSS_COMPILE)gcc

ircut_APP:ircut_APP.o
	$(CC) -o ircut_APP ircut_APP.o
ircut_APP.o:ircut_APP.c
	$(CC) -c ircut_APP.c	  
clean:
	rm -f *.o ircut_APP