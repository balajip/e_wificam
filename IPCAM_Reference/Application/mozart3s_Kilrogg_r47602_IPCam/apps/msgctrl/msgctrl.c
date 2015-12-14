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
 * msgctrl.c
 *
 * \brief
 * A simple utility to send msg to venc
 *
 * \date
 * 2009/08/14
 *
 * \author
 * Acer Tsai
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static const char id[] = "$Id$";

// ./msgctrl -f /tmp/venc/c0/s0/command.fifo -h encoder0 -c start
void print_usage(void) 
{
	fprintf(stderr, "Msg control utility\n"
			"Usage: \n"
			" msgctrl -f <command_fifo> -h <host> -c <content> \n"
            "  .-------------------------------------. \n"
            "  |     host      |      content        | \n"
			"  |===============|=====================| \n"
			"  |    master     |      WDR_ON         | \n"
			"  |               |---------------------| \n"
			"  |               |      WDR_OFF        | \n"
            "  |               |---------------------| \n"
            "  |               |      AF_ON          | \n"
            "  |               |---------------------| \n"
            "  |               |      AF_OFF         | \n"
            "  |               |---------------------| \n"
			"  |               |      AF_ZoomIn      | \n"
			"  |               |---------------------| \n" 
			"  |               |      AF_ZoomOut     | \n"
			"  |               |---------------------| \n"
			"  |               |      AF_FocusNear   | \n"
            "  |               |---------------------| \n"
            "  |               |      AF_FocusFar    | \n"
            "  |               |---------------------| \n"
			"  |               | AutoSceneIsNotRun   | \n"
			"  |               |---------------------| \n"   	
			"  |               |  AutoSceneIsRun     | \n"
			"  |---------------|---------------------| \n"
			"  |    videoin    |   dumpVideoCapBuf   | \n"
			"  |               |---------------------| \n"
			"  |               |   dumpSharedBuf     | \n"
			"  |               |---------------------| \n"
			"  |               | RetVideoCapStatus   | \n"
			"  |               |---------------------| \n"
			"  |               |   photomtcdeftbl    | \n"
			"  |               |---------------------| \n"	
			"  |               |   keepCurrentWB     | \n"
			"  |               |---------------------| \n"		
			"  |               |keepCurrentWBFromFile| \n"
			"  |               |---------------------| \n"	
			"  |               |   FRCalculate_ON    | \n"
			"  |               |---------------------| \n"	
			"  |               |   FRCalculate_OFF   | \n"
			"  |               |---------------------| \n"
			"  |               |   dumpAFStatus      | \n"
			"  |               |---------------------| \n"	
			"  |               |     ISPEnable       | \n"
			"  |               |---------------------| \n"
			"  |               |     ISPDisable      | \n"
			"  |               |---------------------| \n"
			"  |               |    ISPUpdateState   | \n"
			"  |---------------|---------------------| \n"
			"  |    display    |   dumpVideoCapBuf   | \n"
			"  |               |---------------------| \n"
			"  |               | dumpVideoDispBuf    | \n"
			"  |               |---------------------| \n"
			"  |               |   EnableDisplay     | \n"
			"  |               |---------------------| \n"
			"  |               |  DisableDisplay     | \n"
			"  |---------------|---------------------| \n"
			"  |   encoder<id> |      start          | \n"
			"  |id:[0:stream-1]|---------------------| \n"
			"  |               |      stop           | \n"
			"  |               |---------------------| \n"
			"  |               |     forceCI         | \n"
			"  |               |---------------------| \n"
			"  |               |    forceIntra       | \n"
            "  |               |---------------------| \n"
            "  |               |    GetSnapshot      | \n"
			"  .---------------|---------------------. \n"
			);
	exit(1);
}

int main(int argc, char *argv[])
{
	int	ch, iLen;
	char	*szCmd, *szH, *szC;
	char	szMsg[1024];
	FILE	*fpCmdFifo;

	szCmd = NULL;
	/* I know it's weird, 
	   but our libc doesn't support floating point right now. */
	szH = "encoder0";
	szC = "start";

    while ((ch = getopt(argc, argv, "f:h:c:h")) != -1) {
        switch(ch) {
			case 'f':
				szCmd = optarg;
				break;
			case 'h':
				szH = optarg;
				break;
			case 'c':
				szC = optarg;
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
	sprintf(&(szMsg[6]), "<control><request><host>%s</host><content>%s</content></request></control>",
			szH, szC);

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

