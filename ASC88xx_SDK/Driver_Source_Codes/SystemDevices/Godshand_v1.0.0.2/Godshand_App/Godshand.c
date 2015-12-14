/*
 * $Header: $
 *
 * Copyright 2006 VN , Inc. All rights reserved.
 *
 * Description:
 *  Baha Godshand application
 *
 * $History: $
 *
*/
/* ======================================================================== */
#include "Godshand.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
//#define DEBUG_MODE
//extern int      optind, opterr;
//extern char     *optarg;
/* ======================================================================== */
static void display_help(char *command)
{
	printf("Usage: %s [[-r|-w]|[-a address|-v value]|[-b|-s|-h]]\n", command);
	printf("\t-r			read data\n");
	printf("\t-w			write data\n");
	printf("\t-s			the size of buffer\n");
	printf("\t-b			append byte data show\n");
	printf("\t-a			the address in memory\n");
	printf("\t-v			the data in the address \n");
	printf("\t-h			help message\n");
	return;
}

/* ------------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
	int testdev, option;
	char ch;
	GODSHAND_Info tGODSHAND_info;
	DWORD dwTemp, dwTemp1;
	int tmp, offset;

	memset(&tGODSHAND_info, 0, sizeof(tGODSHAND_info));
	opterr = 0;

	testdev = open("/dev/Godshand", O_RDWR);
	if (testdev == -1 )
	{
		printf("Cann't open file\n");
		exit(0);
	}

	while ((option=getopt(argc, argv, "a:v:s:brwlh"))!=-1) {
		switch (option) {
			case 'a':
				tGODSHAND_info.dwAddress = strtoul(optarg, NULL, 16);
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_ADDRESS;
				break;
			case 'v':
				tGODSHAND_info.dwData = strtoul(optarg, NULL, 16);
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_DATA;
				break;
			case 's':
				tGODSHAND_info.dwSize = strtoul(optarg, NULL, 16);
				tGODSHAND_info.pDataBuffer = malloc(tGODSHAND_info.dwSize);
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_AREA;
				break;
			case 'b':
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_BYTE;
				break;
			case 'r':
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_READ;
				break;
			case 'w':
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_WRITE;
				break;
			case 'l':
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_LOOP;
				break;
			case 'h':
			default:
				display_help(argv[0]);
				exit(ERR_RETURN_VALUE);
		}
	}
	do
	{
	if (!((tGODSHAND_info.dwFlag & GODSHAND_FLAG_READ)||(tGODSHAND_info.dwFlag & GODSHAND_FLAG_WRITE)))
	{
		printf("What do you want to do? read(1) or write(2)\n");
		scanf(" %c", &ch);
		if (ch == '1')
			tGODSHAND_info.dwFlag |= GODSHAND_FLAG_READ;
		else if (ch == '2')
			tGODSHAND_info.dwFlag |= GODSHAND_FLAG_WRITE;
		else
			exit(ERR_RETURN_VALUE);
	}
	if (!(tGODSHAND_info.dwFlag & GODSHAND_FLAG_ADDRESS))
	{
		printf("Please type in what address you want to watch.\n");
		scanf("%lx", &tGODSHAND_info.dwAddress);
		tGODSHAND_info.dwFlag |= GODSHAND_FLAG_ADDRESS;
	}
	if (tGODSHAND_info.dwFlag & GODSHAND_FLAG_AREA)
	{
		ioctl(testdev, GODSHAND_IOC_READ_AREA, &tGODSHAND_info);
		for(tmp=0; tmp<tGODSHAND_info.dwSize; tmp+=0x10)
		{
			printf("\n0x%08lx\t", tGODSHAND_info.dwAddress+tmp);
			for(offset=0; offset<0x10; offset+=4)
			{
				printf("%08lx  ", *(DWORD *)(((BYTE *)tGODSHAND_info.pDataBuffer)+offset+tmp) );
			}
			if (tGODSHAND_info.dwFlag & GODSHAND_FLAG_BYTE)
			{
				for(offset=0; offset<0x10; offset+=1)
				{
					if( *(((BYTE *)tGODSHAND_info.pDataBuffer)+offset+tmp) < 126 &&*(((BYTE *)tGODSHAND_info.pDataBuffer)+offset+tmp) > 32 )
						printf("%c", *(((BYTE *)tGODSHAND_info.pDataBuffer)+offset+tmp) );
					else
						printf(".");
				}
			}
		}
		printf("\n");
	}
	else
	{
		PDEBUG("The Flag is %lx\n", tGODSHAND_info.dwFlag);
		dwTemp = tGODSHAND_info.dwData;
		ioctl(testdev, GODSHAND_IOC_READ, &tGODSHAND_info);
		if ((tGODSHAND_info.dwFlag & GODSHAND_FLAG_WRITE))
		{
			dwTemp1 = tGODSHAND_info.dwData;
			tGODSHAND_info.dwData = dwTemp;
			if (!(tGODSHAND_info.dwFlag & GODSHAND_FLAG_DATA))
			{
				printf("Please type in the data you want to change.\n");
				scanf("%lx", &tGODSHAND_info.dwData);
				tGODSHAND_info.dwFlag |= GODSHAND_FLAG_DATA;
			}
			PDEBUG("GODSHAND_FLAG_WRITE %lx\n", tGODSHAND_info.dwFlag);
			ioctl(testdev, GODSHAND_IOC_WRITE, &tGODSHAND_info);
			printf("The write data to %08lx from %08lx ", tGODSHAND_info.dwAddress, dwTemp1);
			ioctl(testdev, GODSHAND_IOC_READ, &tGODSHAND_info);
			printf("to %08lx\n", tGODSHAND_info.dwData);
		}
		else
		{
			printf("The read data from %08lx is %08lx\n", tGODSHAND_info.dwAddress, tGODSHAND_info.dwData);
		}
	}
	}while((tGODSHAND_info.dwFlag & GODSHAND_FLAG_LOOP) == GODSHAND_FLAG_LOOP);

	close(testdev);
	return 0;
}

/* ======================================================================== */
