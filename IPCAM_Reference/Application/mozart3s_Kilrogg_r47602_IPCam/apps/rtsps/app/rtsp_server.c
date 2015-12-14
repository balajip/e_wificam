#include "RtspSrvrApp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>

typedef enum rtsp_server_command_type
{
	rscServerShutDown,
	rscReloadConfigFile,
	rscNoCommand
} ERtspServerCmnd;

int g_iCommand=rscNoCommand;

void rtsp_server_sighandler(int signo)
{
	if (signo == SIGSEGV)
	{
		exit(1);
	}
	else if (signo == SIGHUP)
	{
		g_iCommand = rscReloadConfigFile;
	}
	else
	{
		g_iCommand = rscServerShutDown;
	}
}

void rtsp_server_signal(int signum, void (*handler)(int))
{
	struct sigaction sigaInst;
	
	sigaInst.sa_flags = 0;
	sigfillset(&sigaInst.sa_mask);
	sigaInst.sa_handler = handler;

	sigaction(signum, &sigaInst, NULL);
}

void print_usage(void)
{
	fprintf(stderr,	"Usage:\n"
					"    RtspSrvrApp [-D] [-c config_file][-h] [-p pidfile]\n"
					"Options:\n"
					"    -D                 Run as Daemon\n"
					"    -c config_file     Configuration file of Rtsp Server\n"
		           	"    -p pid_file        Write PID to this file\n"
					"    -h                 This help\n");
}

static char* g_szPidFile = NULL;

/* ============================================================================================= */
SCODE genPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen(szPidFile, "w");

    if (pid_fp == NULL)
    {
        fprintf(stderr, "Can't open %s\n", szPidFile);
        return S_FAIL;
    }
    else
    {
        fprintf(pid_fp, "%d\n", getpid());
        fchmod(fileno(pid_fp), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        fclose(pid_fp);
    }
    return S_OK;
}

/* ============================================================================================= */
void delPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen(szPidFile, "r");
    int   iStoredPid;
    if (pid_fp == NULL)
    {
        fprintf(stderr, "Can't open %s\n", szPidFile);
    }
    else
    {
        fscanf (pid_fp, "%d\n", &iStoredPid);
        fclose(pid_fp);
        if(iStoredPid == getpid())
        {
            remove(szPidFile);
        }
    }
}

int main(int argc, char *argv[])
{
	HANDLE	hRtspSrvrApp;
    BOOL	bIsDaemon = FALSE;
	CHAR	*szConfigFile=NULL;
	int		iUseDefaultConfigFile=0;
	int		i;
    syslog(LOG_INFO, "Start rtsp process with Pid : %d\n", getpid());
	
	while ((i=getopt(argc, argv, "Dc:p:h")) != -1)
	{
		switch(i)
		{
            case 'D':
                bIsDaemon = TRUE;
                break;
            case 'p':
                g_szPidFile = strdup(optarg);
                break;
			case 'c':
				szConfigFile = strdup(optarg);
				break;
			case 'h':
			default:
				print_usage();
				goto exit_program_1;
		}
	}

    /* run as Daemon*/
    if (bIsDaemon)
        daemon(0, 0);

    /* If the g_szPidFile is assigned write pid to g_szPidFile */
    if (g_szPidFile != NULL)
    {
        if (genPIDfile(g_szPidFile) != S_OK)
        {
            fprintf(stderr, "Generate pid file fail !!\n");
            exit(1);
        }
    }

	if (szConfigFile == NULL)
	{
		iUseDefaultConfigFile = 1;
		szConfigFile = RTSPSERVER_DEFAULT_CONFIG_FILE;
	}

	rtsp_server_signal(SIGINT, rtsp_server_sighandler);
	rtsp_server_signal(SIGHUP, rtsp_server_sighandler);
	rtsp_server_signal(SIGTERM, rtsp_server_sighandler);
	rtsp_server_signal(SIGSEGV, rtsp_server_sighandler);

	if (RtspSrvrApp_Initial(&hRtspSrvrApp, szConfigFile) != S_OK)
	{
		fprintf(stderr, "RtspSrvrApp_Initial error!\n");
		goto exit_program_1;
	}

	if (RtspSrvrApp_Start(hRtspSrvrApp) != S_OK)
	{
		fprintf(stderr, "RtspSrvrApp_Start error!\n");
		goto exit_program_2;
	}

	while (1)
	{
		if (g_iCommand == rscServerShutDown)
		{
			g_iCommand = rscNoCommand;
			break;
		}
		else if (g_iCommand == rscReloadConfigFile)
		{
			if (RtspSrvrApp_Reconfig(hRtspSrvrApp, szConfigFile) != S_OK)
			{
				fprintf(stderr, "RtspSrvrApp_Reconfig error!\n");
				goto exit_program_2;
			}
			g_iCommand = rscNoCommand;
		}
		pause();
	}

	if (RtspSrvrApp_Stop(hRtspSrvrApp) != S_OK)
	{
		fprintf(stderr, "RtspSrvrApp_Stop error!\n");
	}

exit_program_2:
	RtspSrvrApp_Release(&hRtspSrvrApp);
	
exit_program_1:
	if ((szConfigFile != NULL) && (!iUseDefaultConfigFile))
	{
		free(szConfigFile);
	}
    if (g_szPidFile != NULL)
    {
        delPIDfile(g_szPidFile);
    }
	syslog(LOG_INFO, "Stop rtsp process !!\n");
	
	return 0;
}
