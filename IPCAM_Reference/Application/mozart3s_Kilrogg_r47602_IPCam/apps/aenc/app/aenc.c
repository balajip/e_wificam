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
 */

/*!
 *******************************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * aenc.c
 *
 * \brief
 * A audio encoder, output in U-buffer format in UNIX socket.
 *
 * \date
 * 2006/06/07
 *
 * \author
 * Cheng-Ching Huang
 *
 *******************************************************************************
 */

#include <sys/types.h>
#include <sys/syslog.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include "global_codec.h"
#include "ubuffer.h"
#include "usck.h"
#include "aenc.h"
#include "aenc_conftree.h"//ptTreeMap
#include "dbgdefs.h"

/* ========================================================================== */
#define DEFAULT_CONFIG_FILE    "/etc/conf.d/aenc_conf.xml"
#define DEFAULT_COMMAND_FILE   "/tmp/aenc/command.fifo"

/* ========================================================================== */
static CHAR* szConfigFile = NULL;

//signal
static TAencXMLTTh  taxHandle;
BOOL bStop;
DWORD dwLiveDev = 0;
BOOL g_bTerminate;

TAencInfo taencInfo;

/* ========================================================================== */
static SCODE XMLWRAPPER_Setting(TAencXMLTTh  *ptaxHandle)
{	 
	TXmlWrapperInitOptions tInitOptions;

	DBPRINT2("%s:%d\n",__func__, __LINE__)
	/* INITIAL */
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&(ptaxHandle->hXmlWrapperObject), &tInitOptions) != S_OK)
	{		
		printf("%s %d : Initial XmlWrapper Fail! \n", __func__, __LINE__);
		return S_FAIL;
	}

	/*construct XML HANDLER Tree*/
	if (XmlWrapper_SetHandler(ptaxHandle->hXmlWrapperObject, ptTreeMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __func__, __LINE__);
		return S_FAIL;
	}

	return S_OK;
}

/* ========================================================================== */
static SCODE aencStart(TAencInfo *ptaencInfo)
{	 		
	TDevOpt  *ptdoSrc = ptaencInfo->ptdoHead;
	SCODE iRet;

	iRet = S_FAIL;
	//*dwLiveDev = 0;	
	//printf("%s:%d:\n", __func__, __LINE__);
	while (ptdoSrc) {
		if (startDevTask(ptdoSrc) == S_OK)
		{
			iRet = S_OK;
			//=== count live dev
			//(*dwLiveDev)++;
			dwLiveDev++;
		}
		ptdoSrc = ptdoSrc->ptdoNext;
	}
	//printf("[aenc](%d)dwLiveDev=%d iRet=%d\n",__LINE__,dwLiveDev,iRet);
	return iRet;
}

/* ========================================================================== */
static SCODE aencStop(TAencInfo *ptaencInfo)
{
	TDevOpt  *ptdoSrc;
	ptdoSrc = ptaencInfo->ptdoHead;
	while (ptdoSrc) {
		stopDevTask(ptdoSrc);
		ptdoSrc = ptdoSrc->ptdoNext;
	}
	
	return S_OK;
}

/* ========================================================================== */
static SCODE aencRelease(TAencInfo *ptaencInfo)
{
	TDevOpt  *ptdoSrc, *ptDevPtr;
		
	ptdoSrc = ptaencInfo->ptdoHead;
	while (ptdoSrc) {
		releaseDevTask(ptdoSrc);
		ptDevPtr = ptdoSrc;
		ptdoSrc = ptdoSrc->ptdoNext;
		printf("[aenc]%d ptDevPtr=%p\n", __LINE__,ptDevPtr);
		free(ptDevPtr);
	}	
	return S_OK;
}
/* ========================================================================== */
void print_usage(char* appname)
{
    fprintf(stderr, "Aenc process\n"
           "Usage:\n"
           "     %s [-D] [-h] [-c config_file] [-f command_fifo] [-p pid_file]\n"
           "Options:\n"
           "    -D                 Run as Daemon\n"
           "    -c config_file     Configuration file path\n"
           "    -f command_fifo    Command fifo path\n"
           "    -p pid_file        Write pid to this file\n"
           "    -h                 This help\n", appname);
    exit(1);
}

/* ========================================================================== */
static SCODE genPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen (szPidFile, "w");

    if (pid_fp == NULL)
    {
        syslog (LOG_ERR, "can't open %s", szPidFile);
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

/* ========================================================================== */
static void delPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen(szPidFile, "r");

    int iStoredPid;
    if (pid_fp == NULL)
    {
        syslog (LOG_ERR, "can't open %s", szPidFile);
    }
    else
    {
        fscanf (pid_fp, "%d\n", &iStoredPid);
        fclose(pid_fp);
        if (iStoredPid == getpid())
        {
            remove(szPidFile);
        }
    }
}

/* ============================================================================================= */
void aenc_handle_signal(int signum)
{
    switch(signum) {
#if 1 //Not support configfile reloading by Jeff Liao  
        case SIGHUP:
                readConf(szConfigFile, &taxHandle);
                break;
#endif
        case SIGTERM:
	case SIGINT:        	
		g_bTerminate = TRUE;
                break;
        default:
                break;
    }
    return;
}

/* ========================================================================== */
void aenc_init_signals(void)
{
    struct sigaction sigaInst;
    sigaInst.sa_flags = 0;
    sigemptyset(&sigaInst.sa_mask);

    sigaddset(&sigaInst.sa_mask, SIGTERM);
    sigaddset(&sigaInst.sa_mask, SIGINT);
#if 1 //Not support configfile reloading by Jeff Liao 
    sigaddset(&sigaInst.sa_mask, SIGHUP);
#endif
    sigaInst.sa_handler = aenc_handle_signal;
    sigaction(SIGTERM, &sigaInst, NULL);
    sigaction(SIGINT, &sigaInst, NULL);
#if 1 //Not support configfile reloading by Jeff Liao  
   sigaction(SIGHUP, &sigaInst, NULL);
#endif
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
//======================================================================================
int main(int argc, char **argv)
{
    static CHAR* szCommandFIFO = NULL;
    static CHAR* szPidFile = NULL;
    BOOL bIsDaemon = FALSE;
    int ch;
#ifdef _ASYNC_IO_	
#if !defined(_RESTART_AENC_)	
	//#ifdef __GAMR_Enc__	
	/*
	 SIGIO is delivered to your process/thread just like
	 any other signal. it will mark your thread as ready-to-run if it was
	 not ready before. when its scheduled to run, it will execute its
	signal handler (or exit if it hadn't defined one). now, as to when it
	gets scheduled to run depends on the threads scheduling class and
	priority, amongst other things. if its class is SCHED_FIFO and the
	priority is right, it will run extremely soon after the SIGIO signal
	is raised.
	http://www.mail-archive.com/alsa-devel@lists.sourceforge.net/msg00568.html
	 */
	struct sched_param sp = { .sched_priority = 60 };
    int ret;

    ret = sched_setscheduler(0, SCHED_FIFO, &sp);
    if (ret == -1) {
        perror("sched_setscheduler");
        return 1;
    }
    printf("[aenc]Set aenc %d as SCHED_FIFO (%d)\n",getpid(), sp.sched_priority);
	//#endif
#endif
#endif	
    while ((ch = getopt(argc, argv, "c:f:p:D")) != -1)
    {
        switch(ch)
        {
            case 'D':
                bIsDaemon = TRUE;
                break;
            case 'c':
                szConfigFile = strdup(optarg);
                break;
            case 'f':
                szCommandFIFO = strdup(optarg);
                break;
            case 'p':
                szPidFile = strdup(optarg);
                break;
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }

    if (bIsDaemon)
    {
      daemonize();
     //   daemon(0, 0);
    }
    if (szPidFile != NULL)
    {
        if (genPIDfile(szPidFile) != S_OK)
        {
            fprintf(stderr, "Generate pid file fail !!\n");
            exit(1);
        }
    }

    if (!szConfigFile)
        szConfigFile = DEFAULT_CONFIG_FILE;
    if (!szCommandFIFO)
        szCommandFIFO = DEFAULT_COMMAND_FILE;

	memset (&taencInfo, 0, sizeof(TAencInfo));
	memset(&taxHandle, 0, sizeof(TAencXMLTTh));	

    /* codec setting */
	registerCodecs(&taencInfo);
	taxHandle.ptAencInfo = &taencInfo;
	taxHandle.bFirstLoad = TRUE;

	/* xmlwrapper */
	if (XMLWRAPPER_Setting(&taxHandle) != S_OK)
	{
		return 1;
	}		
	readConf(szConfigFile, &taxHandle);		
	/* set signal handler */
	aenc_init_signals();
//#if 0 //Not support configfile reloading by Jeff Liao 
    /* initial msgreader to handle command fifo */
    aenc_msg_initial(&taencInfo, szCommandFIFO);
    aenc_msg_start(&taencInfo);
//#endif
	/* start device thread */
	if (aencStart(&taencInfo) != S_OK)
	{
		return 1;
	}
	
	while (g_bTerminate == FALSE)
	{
		select(0,NULL,NULL,NULL,NULL);	
	}
	/* Release */
	aenc_msg_stop(&taencInfo);
	aencStop(&taencInfo);	
	aencRelease(&taencInfo);
	if (XmlWrapper_Release(&(taxHandle.hXmlWrapperObject)) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}

    if (szPidFile != NULL)
    {
        delPIDfile(szPidFile);
    }
    free(szPidFile);
    free(szCommandFIFO);
    free(szConfigFile);
	printf("%s:%d: End\n", __FILE__, __LINE__);
    return 0;
}

