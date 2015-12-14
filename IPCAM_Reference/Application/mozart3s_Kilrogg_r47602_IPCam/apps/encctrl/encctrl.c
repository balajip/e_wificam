/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2011-2015 VN Inc. All rights reserved.
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
 * Copyright 2011-2015 VN, Inc. All rights reserved.
 *
 * \file
 * encctrl.c
 *
 * \brief
 * A simple utility to control the quality  of encoder 
 *
 * \date
 * 2012/09/11
 *
 * \author
 * Jeff Liao
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static const char id[] = "$Id$";
#define QUALITY_PARAMETER_DUMP_PATH "/tmp/.qualty_parameter.dump"

void print_usage(void) 
{
	fprintf(stderr, "encoder (H264/SVC/MPEG4/MJPEG) quality control utility\n"
			"%s \n"
			"Usage: \n"
			"	encctrl -f <command_fifo> -t [0:H264,1:SVC,2:MPEG4,3:MJPEG] -m [0:NONE,1:VQCB,2:CQCB,3:STRICT_VQCB,4:CVBR] -q [quant:0~51 for H264 and SVC, 15,10,7,5,4 for MPEG4,  150,100,50,30,10 for MJPEG]  -b [20000~12000000]  -h <host>\n"
			"	encctrl -f <command_fifo> -h <host> -c GetQualityParameter\n", id);
	exit(1);
}

int main(int argc, char *argv[])
{
	int	ch, iLen;
	char	*szCmd, *szMode, *szBitRate, *szQuant, *szHost,*szCodecType,*szReqContent;
	char	szMsg[512];
	FILE	*fpCmdFifo;

	szCmd = NULL;
	szMode = "0";
	/* I know it's weird, 
	   but our libc doesn't support floating point right now. */
	szQuant = "25";
	szBitRate = "384000";
	szHost="encoder0";
	szCodecType="0";//H264
	szReqContent="UN_SUPPORTED_REQ_CONTENT";
	
    while ((ch = getopt(argc, argv, "f:t:m:q:b:h:c:w")) != -1) {
        switch(ch) {
			case 'f':
				szCmd = optarg;
				break;
			case 't':
				szCodecType = optarg;
				break;
			case 'm':
				szMode = optarg;
				break;
			case 'q':
				szQuant  = optarg;
				break;
			case 'b':
				szBitRate = optarg;
				break;
			case 'h':
				szHost=optarg;
				 break;
			case 'c':
				szReqContent=optarg;
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
	if (strncmp ( szReqContent,"GetQualityParameter", strlen("GetQualityParameter")) == 0)
	{
	  sprintf(&(szMsg[6]), "<control><request><host>%s</host><content>%s</content></request></control>",
			szHost, "GetQualityParameter");
	}
	else
	{
	  sprintf(&(szMsg[6]), "<control><encode><host>%s</host><codec>%s</codec><mode>%s</mode><quant>%s</quant><bitrate>%s</bitrate></encode></control>", 
			szHost,szCodecType,szMode, szQuant, szBitRate);
	}
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
	
	if (strncmp ( szReqContent,"GetQualityParameter", strlen("GetQualityParameter")) == 0)
	{
	  FILE *fp=NULL;
	  unsigned long dwSize=0;
	  char *szDumpContent=NULL;
	  
	  system("sync;sync;sync");
	  usleep(10000);
	  
	  fp=fopen(QUALITY_PARAMETER_DUMP_PATH,"rb");
	  
	  if (fp != NULL)
	  {
	    fseek (fp , 0 , SEEK_END);
	    dwSize = ftell (fp);
	    rewind (fp);
	    szDumpContent = (char*) malloc (sizeof(char)*dwSize);
	    if (szDumpContent == NULL)
	    {
	      goto exit;
	    }
	    memset(szDumpContent,0x0,sizeof(char)*dwSize);
	    if (fread (szDumpContent,1,dwSize,fp) != dwSize)
	    {
	      goto exit;
	    }
	    puts (szDumpContent);
	    
	    if (szDumpContent != NULL)
	    {
	      free(szDumpContent);
	      szDumpContent=NULL;
	    }
	    
	    fclose(fp);
	    fp=NULL;
	    
	  }
	  
	}
exit:
	return 0;
}

