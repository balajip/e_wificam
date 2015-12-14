#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include "../wdt.h"
int main(int argc, char **argv)
{
	int fd;
	unsigned long timeout;
	unsigned long keepalive;

	if (argc != 3) {
		printf("Usage: watchdog timeout keepalive\n");
		return -1;
	}

	if (   (fd = open("/dev/wdt", O_RDONLY)) < 0
	    && (fd = open("/dev/watchdog", O_RDONLY)) < 0)
	{
		perror("watchdog");
		exit(1);
	}
	timeout = strtoul(argv[1], NULL, 0);
	keepalive = strtoul(argv[2], NULL, 0);
	if (ioctl(fd, WDIOC_SETTIMEOUT, &timeout))
	{
		perror("Cannot set timeout value: ");
		close(fd);
		return errno;
	}
	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	/*
	ioctl(fd, WDIOC_SETMATCH, &keepalive);
	*/
	printf
	    ("Watchdog daemon enabled: timeout=%lu, keepalive period=%lu\n",
	     timeout, keepalive);
	while (1) {
		ioctl(fd, WDIOC_KEEPALIVE);
		sleep(keepalive);
	}
	return 0;
}
