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
 * firmware_update.c
 *
 * \brief
 * upgrade whole firmware cgi implement
 *
 * \date
 * 2010/01/15
 *
 
 * \author
 * Anil Kulkarni
 *
 *******************************************************************************
 */

#include "firmware_update.h"
//#define DUMP_GCI_DATA 1

/* ============================================================================================= */
int main ( int argc, char *argv[] )
{
	DWORD 	dwCntLength;
	CHAR 	*szInputStr;	// get from network.
	BYTE	*pbyTemp;
	FILE 	*fpOutput;
	DWORD	dwBoundaryCount = 0;
	DWORD	dwHeaderCount = 0;
	DWORD	dwLineIndex;
	int		iRet;
	pid_t 	pid;
	int		status;
	struct rusage rusage;
	int fd;
#ifdef DUMP_GCI_DATA
	CHAR *szDumpFilePath="/tmp/firmware_update.dat";
	FILE	*fpDumpFile=NULL;
	int  dwCGICntLength=0;
#endif
	
	iRet = system("/usr/sbin/stopallproc");
#ifdef DUMP_GCI_DATA
	system("/bin/mount -t tmpfs -o remount,size=36m /tmp");
#else
	system("/bin/mount -t tmpfs -o remount,size=24m /tmp");
#endif	
	printf("Content-Type:text/html\r\n");
	printf("Cache-Control: no-cache\r\n\n");
	
	/* METHOD="POST", CGI specifications say that the data is passed to the script or program 
	 * in the standard input stream (stdin), and the length (in bytes, i.e. characters) of the 
	 * data is passed in an environment variable called CONTENT_LENGTH. */
	dwCntLength = atoi(getenv("CONTENT_LENGTH"));
#ifdef DUMP_GCI_DATA
	dwCGICntLength=dwCntLength;
#endif
	if (dwCntLength <= 0) {
		printf("Error: String Length <= 0 \n");
		return 0;
	}

	szInputStr = (CHAR *)malloc(dwCntLength+1);
    if (szInputStr == NULL) {
        return ERR_OUT_OF_MEMORY;
    }
	memset(szInputStr, 0, dwCntLength);
	szInputStr[dwCntLength] = '\0';
	pbyTemp = szInputStr;

	
	// read data from stdin, that means from client upload
	fread(szInputStr, dwCntLength, 1, stdin);
#ifdef DUMP_GCI_DATA
	fpDumpFile=fopen(szDumpFilePath, "wb");
	fwrite(szInputStr, dwCGICntLength, 1, fpDumpFile);
	fclose(fpDumpFile);
#endif
	
	/* parse ----------------------7383914384, that's boundary
	 * 7383914384 is a random number from network packet 
	 */
	while (*(szInputStr++) != '\n') {
		dwBoundaryCount++;
		dwHeaderCount++;
	}
	dwBoundaryCount--; 	// discount \r, so it must -1.
	dwHeaderCount++;	// compensate \n, so it must +1
//	printf("dwBoundaryCount = %d \n", dwBoundaryCount);
	
	for (dwLineIndex=0; dwLineIndex < 3;) {
		while (*(szInputStr++) != '\n') {
			dwHeaderCount++;
		}	
		dwHeaderCount++;	// compensate \n, so it must +1
		dwLineIndex++;		// means next line start
	}
//	printf("dwHeaderCount = %d \n", dwHeaderCount);

	/* Because after the real data, \r\n------------------7383914384--\r\n comes
	 *   7383914384 is a random number, the same as header 
	 */
	dwCntLength = dwCntLength - dwHeaderCount - 2 - dwBoundaryCount - 4; // 2 means: \r\n, 4 means --\r\n
	if((fpOutput=fopen(EXTRACT_PATH, "wb"))==NULL) {
		printf("error open file\n");
		return 0;
	}

	fwrite(szInputStr, dwCntLength, 1, fpOutput);
	fclose(fpOutput);
	free(pbyTemp);

	if ((pid = fork()) == 0) {
		// child process
		if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {  
			dup2(fd, STDIN_FILENO);  
			dup2(fd, STDOUT_FILENO);  
			dup2(fd, STDERR_FILENO);  
			if (fd > 2) {
				close(fd);
			}
		} 
		system("/usr/sbin/firmware_write"); 
	} else { // parent
		wait4(pid, &status, 0, &rusage);
		/*
		 *	WIFEXITED(status)
         *  	returns true if the child terminated normally, that is, by calling
         *  	exit(3) or _exit(2), or by returning from main().
		*/

		if (WIFEXITED(status)) { // non-zero value
#ifdef DUMP_GCI_DATA
			printf("<html><body>");
			printf("<h1>done. Please reboot. GOT_CGI_DATALEN=%d</h1>",dwCGICntLength);
			printf("</body></html>");
#else
			printf("<html><body>");
			printf("<h1>done. Please reboot the target.</h1>");
			printf("</body></html>");
			
			
#endif
						
		} else {	// zero
#ifdef DUMP_GCI_DATA
			printf("<html><body>");
			printf("<h1>fail. Please reboot, it will restore to the default. GOT_CGI_DATALEN=%d</h1>",dwCGICntLength);
			printf("</body></html>");

#else
			printf("<html><body>");
			printf("<h1>fail. Please reboot, it will restore to the default.</h1>");
			printf("</body></html>");		
#endif	
		}
	}

	return 0;
}
