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
 * adec.c
 *
 * \brief
 * A audio decoder, output to audio device through ALSA.
 *
 * \date
 * 2008/3/19
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


#include <sys/types.h>
#include <sys/stat.h>

#include "adec.h"
#include "adec_app.h"

HANDLE hAdecObject;

/* ============================================================================================= */
void Adec_HANDLE_SIGNAL(int signum)
{
    switch(signum) {
        case SIGHUP:
                Adec_ResetConfigFlag(hAdecObject);
                break;
        case SIGTERM:
		case SIGINT:        	
				Adec_ResetTerminalFlag(hAdecObject);
                break;
        default:
                break;
    }
    return;
}

/* ============================================================================================= */
void Adec_Init_Signals(void)
{
    struct sigaction sigaInst;
    sigaInst.sa_flags = 0;
    sigemptyset(&sigaInst.sa_mask);

    sigaddset(&sigaInst.sa_mask, SIGTERM);
    sigaddset(&sigaInst.sa_mask, SIGINT);
    sigaddset(&sigaInst.sa_mask, SIGHUP);

    sigaInst.sa_handler = Adec_HANDLE_SIGNAL;
    sigaction(SIGTERM, &sigaInst, NULL);
    sigaction(SIGINT, &sigaInst, NULL);
    sigaction(SIGHUP, &sigaInst, NULL);
}
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static void daemonize(void)
{
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}


/* ========================================================================== */
int main(int argc, char **argv)
{
    TAdecInitOpt tAdecInitOpt;
    SCODE scRes;
    CHAR *szPidFile = NULL;
    CHAR *szConfigFile = NULL;
    BOOL bIsDaemon = FALSE;
    int optelemt;
    memset(&tAdecInitOpt, 0, sizeof(TAdecInitOpt));

    while ((optelemt = getopt(argc, argv, "c:Dh")) != -1)
    {
        switch(optelemt)
        {
            case 'c':
                tAdecInitOpt.szConfigFile = strdup(optarg);
                break;
            case 'D':
                bIsDaemon = TRUE;
                break;
            case 'h':
            default:
                print_usage(argv[0]);
        }
    }

    if (tAdecInitOpt.szConfigFile == NULL)
    {
        print_usage(argv[0]);
    }

    /* run as Daemon*/
    if (bIsDaemon){
      daemonize();
     //   daemon(0, 0);
    }
    if ((scRes = AdecApp_Initial(&hAdecObject, tAdecInitOpt)) != S_OK)
    {
        fprintf(stderr, "[ADEC] Initial Audio decoder application fail %x\n", scRes);
        exit(1);
    }
    
    Adec_Init_Signals();
    
    AdecApp_Process_Loop(hAdecObject);

	AdecApp_Release(&hAdecObject);
	
    free(szConfigFile);
    free(szPidFile);

    return 0;
}

/* ========================================================================== */
void print_usage(char* appname)
{
    fprintf(stderr, "Aenc process\n"
           "Usage:\n"
           "     %s <-c config_file> [-D] [-h] \n"
           "Options:\n"
           "    -c config_file     Configuration file path\n"
           "    -D                 Run as Daemon\n"
           "    -h                 This help\n", appname);
    exit(1);
}
