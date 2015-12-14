#include <stdlib.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include "../ircut.h"

#define FILE_NAME "/dev/ircut"

int main()
{
	int fd;
	int ret;
	int aaa = 0, i;

	
	fd = open("/dev/ircut", O_RDWR);
	if (fd == -1)
	{
		printf("cannot open\n");
		return 0;
	}

/* 	for (i=0; i<=10; i++)
	{
		if (aaa == 0)
		{
			printf("close\n");
			ret = ioctl(fd,IRCUT_CLOSE);
			aaa = 1;
		}
		else
		{
			printf("open\n");
			ret = ioctl(fd,IRCUT_OPEN);
			aaa = 0;
		}
	} */

	int end = 0;
	while (end == 0)
	{
		printf("enter value 0:Close,1:Forward,2:Reverse\n");
		int input;
		scanf("%d", &input);
		if (input == 1)
		{
			printf("open\n");
			ret = ioctl(fd,IRCUT_OPEN);
			ret = ioctl(fd,IRCUT_STANDBY);
		}
		else if (input == 2)
		{
			printf("close\n");
			ret = ioctl(fd,IRCUT_CLOSE);
			ret = ioctl(fd,IRCUT_STANDBY);
		}
		else
		{
			end = 1;
			printf("bye\n");
		}
	}
	
	
	close(fd);
	return 0;
}
