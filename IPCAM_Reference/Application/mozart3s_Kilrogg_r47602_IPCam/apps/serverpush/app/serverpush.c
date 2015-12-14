/*
**********************************************************************
* $Header$
*
*  Copyright (c) 2000-2006 VN Inc. All rights reserved.
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
*  | VN INC.                                                    |
*  +-----------------------------------------------------------------+
*
* $History$
*
  ***********************************************************************
 */

/*!
 ***********************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * serverpush.c
 *
 * \brief
 * Server Push for mjpeg
 *
 * \date
 * 2006/11/04
 *
 * \author
 * Jessie Lin
 *
 *
 **********************************************************************
 */

#include "serverpush_local.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>



HANDLE hServerPushObject = NULL;
//static BOOL bEND = FALSE;

/* =========================================================================================== */
static void sig_handle(int signum);
static void print_usage(void);
static SCODE genPIDfile(char *pcPidFile);
static void delPIDfile(char *pcPidFile);

/* =========================================================================================== */
int main(int argc, char *argv[])
{
    int             iOpt;
    TArgInfo        tArgInfo;
    static char    *szPidFile = NULL;
    
    memset(&tArgInfo, 0, sizeof(TArgInfo));

    while ((iOpt = getopt(argc, argv, "c:p:")) != -1)
    {
        switch(iOpt)
        {
            case 'c':
                tArgInfo.szConfigPath = strdup(optarg);
                break;
            case 'p':
                szPidFile = strdup(optarg);
                break;
            default:
                print_usage();
                exit(1);
        }
    }
    
	openlog("[SERVER PUSH]", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
	syslog(LOG_INFO, "Start server push process with Pid : %d\n", getpid());

    // ====== check Config File Path ======
    if (tArgInfo.szConfigPath == NULL)
    {
        printf("Please assigned configuration file\n\n");
        print_usage();
        exit(1);
    }
    if (szPidFile == NULL)
    {
        szPidFile = strdup(PID_FILE);
    }

    //======  register signals ======
    if (signal(SIGINT, sig_handle) == SIG_ERR)
    {
        fprintf(stderr, "[SERV_PUSH] Cannot catch SIGINT\n");
        exit(1);
    }
    if (signal(SIGTERM, sig_handle) == SIG_ERR)
    {
        fprintf(stderr, "[SERV_PUSH] Cannot catch SIGTERM\n");
        exit(1);
    }
    // Ignore broken pipes
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        fprintf(stderr, "[SERV_PUSH] Cannot catch SIGPIPE\n");
        exit(1);
    }
    
    // ====== initial server push object ======
    if (ServerPushApp_Initial(&hServerPushObject, tArgInfo) != S_OK)
    {
        fprintf(stderr, "[SERV_PUSH] Initial server push app error!!\n");
        exit(1);
    }
    
    // ====== start server push threads ======
    if (ServerPushApp_Start(hServerPushObject) != S_OK)
    {
        fprintf(stderr, "[SERV_PUSH] Start server push app error!!\n");
        exit(1);
    }   

    // ====== generate pid file ======
    if (genPIDfile(szPidFile) != S_OK)
    {
        fprintf(stderr, "[SERV_PUSH][%s] Generate pid file fail !!\n", __FUNCTION__);
        //exit(1);    
    }

    ServerPushApp_Release(&hServerPushObject);
    
    // ====== delete pid file ======
    if (szPidFile != NULL)
    {
        delPIDfile(szPidFile);
    }
    free(szPidFile);
    free(tArgInfo.szConfigPath);
	closelog();
	return 0;
}

/* =========================================================================================== */
static void sig_handle(int signum)
{
    switch(signum)
    {
        case SIGTERM:
            ServerPushApp_Stop(&hServerPushObject);
            break;
        case SIGINT:
            ServerPushApp_Stop(&hServerPushObject);
            break;
        default:
            break;
    }
    return;
}

/* =========================================================================================== */
static void print_usage(void)
{
	printf("Venc slave process\n"
		   "Usage:\n"
		   "    venc_slave -c <config_file> [-p <pid_file>]\n"
		   "Options:\n"
		   "    -c config_file     Configuration file of this stream\n"
		   "    -h                 This help\n"
           "    -p pid_file        Write pid to this file\n"
		   "                       Default: %s\n", 
		   PID_FILE 
		   );
}

/* =========================================================================================== */
static SCODE genPIDfile(char *pcPidFile)
{
    FILE *pid_fp = fopen (pcPidFile, "w");

    if (pid_fp == NULL)
    {
        syslog (LOG_ERR, "[VDEC_DISP][%s] Can't open %s\n", __FUNCTION__, pcPidFile);
        return S_FAIL;
    }
    else
    {
        fprintf (pid_fp, "%d\n", getpid());
        fchmod (fileno(pid_fp), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        fclose (pid_fp);
    }
    return S_OK;
}

/* =========================================================================================== */
static void delPIDfile(char *pcPidFile)
{           
    FILE *pid_fp = fopen(pcPidFile, "r");

    int iStoredPid;
    if (pid_fp == NULL)
    {
        syslog (LOG_ERR, "[VDEC_DISP][%s] Can't open %s", __FUNCTION__, pcPidFile);
    }
    else
    {
        fscanf (pid_fp, "%d\n", &iStoredPid);
        fclose(pid_fp);
        if (iStoredPid == getpid())
        {
            remove(pcPidFile);
        }
    }
}

/* =========================================================================================== */
/* =========================================================================================== */
