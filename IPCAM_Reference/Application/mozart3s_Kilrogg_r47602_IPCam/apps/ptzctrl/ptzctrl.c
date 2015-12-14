/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2007-2010 VN Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 *
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2007-2010 VN, Inc. All rights reserved.
 *
 * \file
 * ptzctrl.c
 *
 * \brief
 * A simple utility to control PTZ
 *
 * \date
 * 2008/11/10
 *
 * \author
 * Cheng-Ching Huang
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static const char id[] = "$Id$";

void print_usage(void) 
{
	fprintf(stderr, "PTZ control utility\n"
			"%s \n"
			"Usage: \n"
			"	ptzctrl -f <command_fifo> -e <t/f> -p <p> -t <t> -z <z> -s <s> -h <host>\n", id);
	exit(1);
}

int main(int argc, char *argv[])
{
	int	ch, iLen;
	char	*szCmd, *szEnable, *szP, *szT, *szZ, *szS,*szHost;
	char	szMsg[512];
	FILE	*fpCmdFifo;

	szCmd = NULL;
	szEnable = "t";
	/* I know it's weird, 
	   but our libc doesn't support floating point right now. */
	szP = szT = "0";
	szZ = szS = "1";
	szHost="encoder0";
    while ((ch = getopt(argc, argv, "f:e:p:t:z:s:h:w")) != -1) {
        switch(ch) {
			case 'f':
				szCmd = optarg;
				break;
			case 'e':
				szEnable = optarg;
				break;
			case 'p':
				szP = optarg;
				break;
			case 't':
				szT = optarg;
				break;
			case 'z':
				szZ = optarg;
				break;
			case 's':
				szS = optarg;
				break;
			case 'h':
				szHost=optarg;
				 break;
			default:
				print_usage();
				exit(1);
        }
    }

	if (szCmd == NULL) {
		print_usage();
	}

	szMsg[0] = 1;
	/* force it using a long mode */
	szMsg[1] = 0x84;
	sprintf(&(szMsg[6]), "<control><ptz><virtual><host>%s</host><enable>%s</enable><pan>%s</pan><tilt>%s</tilt><zoom>%s</zoom><speed>%s</speed></virtual></ptz></control>", 
			szHost,szEnable, szP, szT, szZ, szS);
	//printf("[ptzctl]host=%s\n",szHost);
	iLen = strlen(&(szMsg[6]));
	/*
	if (iLen > 127) {
		fprintf(stderr, "The composed message is too long!\n %s\n", szMsg + 2);
		exit(1);
	}
	*/
	//szMsg[1] = iLen;
	szMsg[5] =  iLen >> 24;
	szMsg[4] = (iLen & 0x00FF0000) >> 16;
	szMsg[3] = (iLen & 0x0000FF00) >> 8;
	szMsg[2] = (iLen & 0x000000FF);

	fpCmdFifo = fopen(szCmd, "w");
	if (fpCmdFifo == NULL) {
		fprintf(stderr, "Cannot open %s\n", szCmd);
	}

	fwrite(szMsg, 1, iLen + 6, fpCmdFifo);
	fclose(fpCmdFifo);

	return 0;
}

