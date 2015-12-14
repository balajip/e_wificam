#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "wdt.h"
#include <sys/ioctl.h>

static char* g_szPidFile = NULL;

/* ============================================================================================= */
void print_usage(void)
{
    fprintf(stderr, "Watchdog process\n"
           "Usage:\n"
           "    watchdog [-D] [-h] [-t timeout] [-k keepalive] [-p pidfile]\n"
           "Options:\n"
           "    -D                 Run as Daemon\n"
           "    -t timeout     	   timeout value\n"
           "    -k keepalive	   keepalive value\n"
           "    -p pid_file        Write PID to this file\n"
           "    -h                 This help\n");
    exit(1);
}

/* ============================================================================================= */
int genPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen(szPidFile, "w");

    if (pid_fp == NULL)
    {
        fprintf(stderr, "Can't open %s\n", szPidFile);
        return 0;
    }
    else
    {
        fprintf(pid_fp, "%d\n", getpid());
        fchmod(fileno(pid_fp), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        fclose(pid_fp);
    }
    return 1;
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


/* ============================================================================================= */
int main(int argc, char **argv)
{
	int fd;
    int iIsDaemon = 0;
	int timeout;
	int keepalive = 0;
	int c;
    syslog(LOG_INFO, "Start watch dog process with Pid : %d\n", getpid());

    while ((c = getopt(argc, argv, "t:Dk:p:h")) != -1)
    {
        switch(c)
        {
            case 't':
                timeout = atoi(optarg);
                break;
            case 'D':
                iIsDaemon = 1;
                break;
            case 'k':
                keepalive = atoi(optarg);
                break;
            case 'p':
                g_szPidFile = strdup(optarg);
                break;
            default:
                print_usage();
                exit(1);
        }
    }

    /* run as Daemon*/
    if (iIsDaemon)
    {
       daemonize();
     //   daemon(0, 0);
    }
    /* If the g_szPidFile is assigned write pid to g_szPidFile */
    if (g_szPidFile != NULL)
    {
        if (!genPIDfile(g_szPidFile))
        {
            fprintf(stderr, "Generate pid file fail !!\n");
            exit(1);
        }
    }

	fd = open("/dev/wdt", O_RDONLY);
	if (fd == -1) {
		perror("watchdog");
		exit(1);
	}

	ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	printf
	    ("Watchdog daemon enabled: timeout=%d, keepalive period=%d\n",
	     timeout, keepalive);
	while (1) {
		ioctl(fd, WDIOC_KEEPALIVE);
		sleep(keepalive);
	}

    if (g_szPidFile != NULL)
    {
        delPIDfile(g_szPidFile);
    }

    free(g_szPidFile);
    syslog(LOG_INFO, "Stop Watchdog process !!\n");

	return 0;
}
