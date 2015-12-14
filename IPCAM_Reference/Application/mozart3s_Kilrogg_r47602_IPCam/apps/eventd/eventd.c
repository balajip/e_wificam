#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "dbgdefs.h"
#include "typedef.h"
#include "video_error.h"
#include <signal.h>
#include <syslog.h>
#include "xmlwrapper.h"
#include <time.h>
#include <assert.h>

#include "error_print.h"
extern int	daemon_proc;	/* defined in error_print.c */
#define MOTION_SCK "/var/run/event_motion.sck"
#define EMAIL "/usr/sbin/smtp_app.lua"
#define FTP "/usr/sbin/ftp_app.lua"
#define EVENTD_CONF "/etc/conf.d/event_action_conf.xml"
#define EVENT_ACTION_MAX_RULE_NUM 1

#define ACTIONS_SCRIPT_FILE "/tmp/event/actions.sh"
#define ACTIONS_SCRIPT "actions.sh"
volatile sig_atomic_t g_bReload = FALSE;

typedef struct t_event_detect_schedule
{
	BOOL abWeekday[7]; 
	DWORD dwStartMinute;
	DWORD dwEndMinute;
} TSchedule;

typedef struct t_action_for_event
{
	DWORD dwAlarmInterval;
	BOOL bEmail;
	BOOL bFTP;
	char* szEventSnapshotPath;
} TAction;

typedef struct t_eventd_info
{
	BOOL bEnable;
	BOOL bSchedule;
	TSchedule atSchedule[EVENT_ACTION_MAX_RULE_NUM];
	TAction	tAction;
	DWORD dwEAIndex;
} TEventdInfo;
TEventdInfo tEventdInfo;
//TakeActions() will fork processes to take actions. The actions maybe  decribed in one  shell script.
//The forked child processes will remove the *.jpg. 	
static void TakeActions(TAction* ptAction)
{
	//int iRet = 0;
	int pid ;//, died, status;

	FILE *fp=NULL;
	char action_script_path[1024];
	char action_script_name[1024];


	if(ptAction->bEmail == FALSE && ptAction->bFTP == FALSE)
	{
		err_msg("Email:disable, FTP:disable, remove %s \n",ptAction->szEventSnapshotPath);
		while (unlink(ptAction->szEventSnapshotPath) < 0)			 
		{
			
			usleep(5000);
		}

	
		return;
	}
	
	if (ptAction->bEmail == TRUE && ptAction->bFTP == TRUE)
	{	
		pid=fork();
		if (pid==0)
		{
			snprintf(action_script_path,sizeof(action_script_path),"%s_%d",ACTIONS_SCRIPT_FILE,getpid());			
			snprintf(action_script_name,sizeof(action_script_name),"%s_%d",ACTIONS_SCRIPT,getpid());
			fp=fopen(action_script_path,"w");
			if(fp == NULL)
			{
				exit(0);
			}
			fprintf(fp,"%s\n","#!/bin/sh");	
			fprintf(fp,"%s %s\n",EMAIL,ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n",FTP,ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n","rm -f",ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n","rm -f",action_script_path);
			fprintf(fp,"%s\n","exit 0");			
			if(fp != NULL)
			{
		  		fclose(fp);
			}
			chmod(action_script_path, 0777);
			execl(action_script_path,action_script_name,(char *)NULL);			
			
		}	
		

		return;
	}

	if (ptAction->bEmail == TRUE && ptAction->bFTP == FALSE)
	{
		pid=fork();
		if (pid==0)
		{
			snprintf(action_script_path,sizeof(action_script_path),"%s_%d",ACTIONS_SCRIPT_FILE,getpid());			
			snprintf(action_script_name,sizeof(action_script_name),"%s_%d",ACTIONS_SCRIPT,getpid());
			fp=fopen(action_script_path,"w");
			if(fp == NULL)
			{
				exit(0);
			}
			fprintf(fp,"%s\n","#!/bin/sh");	
			fprintf(fp,"%s %s\n",EMAIL,ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n","rm -f",ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n","rm -f",action_script_path);
			fprintf(fp,"%s\n","exit 0");
			if(fp != NULL)
			{
		  		fclose(fp);
			}
			chmod(action_script_path, 0777);
			execl(action_script_path,action_script_name,(char *)NULL);			
			
		}	
		

		return;
	}


	if (ptAction->bEmail == FALSE && ptAction->bFTP == TRUE)
	{	pid=fork();
		if (pid==0)
		{
			snprintf(action_script_path,sizeof(action_script_path),"%s_%d",ACTIONS_SCRIPT_FILE,getpid());			
			snprintf(action_script_name,sizeof(action_script_name),"%s_%d",ACTIONS_SCRIPT,getpid());
			fp=fopen(action_script_path,"w");
			if(fp == NULL)
			{
				exit(0);
			}
			fprintf(fp,"%s\n","#!/bin/sh");	
			fprintf(fp,"%s %s\n",FTP,ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n","rm -f",ptAction->szEventSnapshotPath);
			fprintf(fp,"%s %s\n","rm -f",action_script_path);
			fprintf(fp,"%s\n","exit 0");
			if(fp != NULL)
			{
		  		fclose(fp);
			}
			chmod(action_script_path, 0777);
			execl(action_script_path,action_script_name,(char *)NULL);			
			
		}	
		


		return;
	}

	

#if 0
	if (ptAction->bEmail == TRUE)
	{
		debug_printf("motion! Email...\n");
		if ((pid = fork())<0)
	    {
	        printf("fork fail\n");
	    }
	    else if (pid == 0)
	    {
	        iRet = execl(EMAIL, EMAIL, ptAction->szEventSnapshotPath, (char *) NULL);
	        printf("iRet = %d, errno = %d   \n", iRet, errno);
	        exit(0);
	    }
	    else
	    {
	    }
	    if ((died = waitpid(-1, &status, 0))<0)
	    {
	        printf("wait fail\n");
	    }
	    else
	    {
	    }
	}
	debug_printf("%x err = %d\n", iRet, errno);
	if (ptAction->bFTP == TRUE)
	{
		debug_printf("motion! FTP...\n");
		if ((pid = fork())<0)
	    {
	        printf("fork fail\n");
	    }
	    else if (pid == 0)
	    {
	        iRet = execl(FTP, FTP, ptAction->szEventSnapshotPath, (char *) NULL);
	        printf("iRet = %d, errno = %d   \n", iRet, errno);
	        exit(0);
	    }
	    else
	    {
	    }
	    if ((died = waitpid(-1, &status, 0))<0)
	    {
	        printf("wait fail\n");
	    }
	    else
	    {
	    }
	}
#endif	
	return;
}


static void Eventd_SetEnable(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	if (! memcmp(s, "true", 4))
	{
		ptEventdInfo->bEnable = TRUE;
	}
	else
	{
		ptEventdInfo->bEnable = FALSE;
	}
	return;
}

static void Eventd_SetSchedule(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	if (! memcmp(s, "true", 4))
	{
		ptEventdInfo->bSchedule = TRUE;
	}
	else
	{
		ptEventdInfo->bSchedule = FALSE;
	}

	return;
}

static SCODE StringToMinute(char* str, DWORD *pdwMinute)
{
	int i = 0;
	DWORD dwHour = 0;
	char * pch;
	pch = strtok (str,":");	

	while (pch != NULL)
	{
		if (i == 0)
		{
			dwHour = atoi(pch);
			debug_printf("hour% lu\n", dwHour);
			if(dwHour > 24)
			{
				fprintf(stderr, "hour exceeds 24!!!\n");
				return S_FAIL;
			}
			*pdwMinute = dwHour*60;
		}
		else if (i == 1)
		{
			debug_printf("min %d\n", atoi(pch));
			if (atoi(pch) > 60)
			{
				fprintf(stderr, "minute %d exceeds 60!!!\n", atoi(pch));
				return S_FAIL;
			}
			else if ((dwHour == 24)&&(atoi(pch)>0))
			{
				fprintf(stderr, "exceeds 24:00!!!\n");
				return S_FAIL;
			}
			else
			{
				*pdwMinute += atoi(pch);
			}
		}
		else
		{
			fprintf(stderr, "wrong format of start time!!!\n");
		}
		i++;
		
		pch = strtok (NULL, ":");
	}
	return S_OK;
}
	
static void  Eventd_SetEndTime(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	DWORD dwMinute = 0;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if (StringToMinute(str, &dwMinute) != S_OK)
	{
		ptEventdInfo->bEnable = FALSE;
	}
	else
	{
		ptEventdInfo->atSchedule[ptEventdInfo->dwEAIndex].dwEndMinute = dwMinute;
	}
	return;
}
static void Eventd_SetStartTime(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	DWORD dwMinute = 0;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if (StringToMinute(str, &dwMinute) != S_OK)
	{
		ptEventdInfo->bEnable = FALSE;
	}
	else
	{
		ptEventdInfo->atSchedule[ptEventdInfo->dwEAIndex].dwStartMinute = dwMinute;
	}
	return;
}

static void Eventd_SetDay(void *userData, const char *s, int len)
{
	int i;
	int iWeekDay;
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	TSchedule *ptSchedule = &ptEventdInfo->atSchedule[ptEventdInfo->dwEAIndex];
	char * pch;
	char str[len];
	memcpy(str, s, len);
	pch = strtok (str," ,");
	for (i=0; i<7; i++)
	{
		ptSchedule->abWeekday[i] = FALSE; 
	}
	while (pch != NULL)
	{
		switch (iWeekDay = atoi(pch))
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				ptSchedule->abWeekday[iWeekDay] = TRUE;
				break;
			default:
				break;
		}
		pch = strtok (NULL, " ,");
	}
	
	return;
}

static SCODE Eventd_SetRule(void *userData, const char *s, const char **atts)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;

	if (atts[0] && atts[1])
	{
		if (! memcmp(atts[0], "id", 2))
		{
			int iRuleID = atoi(atts[1]);
			if (iRuleID >= EVENT_ACTION_MAX_RULE_NUM)
			{
				return ERR_INVALID_ARG;
			}
			else
			{
				ptEventdInfo->dwEAIndex = (DWORD)iRuleID;
			}
		}
	}
	else
	{
		return ERR_INVALID_ARG;		
	}
	return S_OK;
}
static void Eventd_SetInterval(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	ptEventdInfo->tAction.dwAlarmInterval = atoi(s);
	return;
}

static void Eventd_SetAction_FTP(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	if (!memcmp(s, "on", 2))
	{
		ptEventdInfo->tAction.bFTP = TRUE;
	}
	else
	{
		ptEventdInfo->tAction.bFTP = FALSE;
	}	
	return;
}

static void Eventd_SetAction_EMAIL(void *userData, const char *s, int len)
{
	TEventdInfo* ptEventdInfo = (TEventdInfo*)userData;
	if (!memcmp(s, "on", 2))
	{
		ptEventdInfo->tAction.bEmail = TRUE;
	}
	else
	{
		ptEventdInfo->tAction.bEmail = FALSE;
	}	
	return;
}

SCODE LoadConfig(TEventdInfo* ptEventdInfo)
{
	int sRet;
	HANDLE hXmlWrapObj;	
	TXmlWrapperTreeMap ptParsingMap[] = 
	{
		{"root/event_detect/enabled", NULL, &Eventd_SetEnable, NULL},
		{"root/event_detect/schedule_enabled", NULL, &Eventd_SetSchedule, NULL},
		{"root/event_detect/schedule", &Eventd_SetRule, NULL, NULL},
		{"root/event_detect/schedule/day", NULL, &Eventd_SetDay, NULL},
		{"root/event_detect/schedule/start_time", NULL, &Eventd_SetStartTime, NULL},
		{"root/event_detect/schedule/end_time", NULL, &Eventd_SetEndTime, NULL},
		{"root/event_detect/action/alarm_interval", NULL, &Eventd_SetInterval, NULL},
		{"root/event_detect/action/Email", NULL, &Eventd_SetAction_EMAIL, NULL},
		{"root/event_detect/action/FTP", NULL, &Eventd_SetAction_FTP, NULL},
		{NULL, NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;
	memset(ptEventdInfo, 0, sizeof(TEventdInfo));
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadFile_UsrDefFunc(EVENTD_CONF, hXmlWrapObj, ptEventdInfo)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;
}

void print_usage(void)
{
	fprintf(stderr,	"Usage:\n"
					"    Eventd [-D] [-h] [-p pidfile]\n"
					"Options:\n"
					"    -D                 Run as Daemon\n"
		           	"    -p pid_file        Write PID to this file\n"
					"    -h                 This help\n");
}

static char* g_szPidFile = NULL;

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

int MsgUtil_CreateSocket(struct sockaddr_un* sunx)
{
	int fd = -1;
	char szBindPath[128];
	memset(szBindPath, 0, 128);
	memcpy(szBindPath, MOTION_SCK, strlen(MOTION_SCK));
	debug_printf("%s bind path [%s]", __FILE__, szBindPath);	
	
	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{	/* open UNIX UDP socket */
		debug_printf("[%s:%s:%d) open socket error (%s)\n", 
						__func__, __FILE__, __LINE__, 
						strerror(errno));
		return -1;
	}
                                                                  
	if (szBindPath != NULL) 
	{
		if (strlen(szBindPath) > 128)
		{
			debug_printf("[%s:%s:%d] input length error %d, \"%s\"\n", 
							__func__, __FILE__, __LINE__, 
							strlen(szBindPath), szBindPath);
			goto error_exit;
		}
		unlink(szBindPath);
		memset(sunx, 0, sizeof(*sunx));
		sunx->sun_family = AF_UNIX;
		strncpy(sunx->sun_path, szBindPath, sizeof(sunx->sun_path));
                                                      
		if ((bind(fd, (struct sockaddr *)sunx, sizeof(*sunx)) < 0) || 
			(chmod(szBindPath, 0666) < 0)) 
		{
			debug_printf("[%s:%s:%d] bind to \"%s\" error (%s)\n", 
							__func__, __FILE__, __LINE__, 
							szBindPath, strerror(errno));
			goto error_exit;
		}
	}

	return fd;

error_exit:
	close(fd);
	
	return -1;
}
//int ConnectSocket(const CHAR *szSockPath)
/*
int ConnectSocket(void)
{
	int fd = -1;
	struct sockaddr_un	sunx;
	char szSockPath[128];
	memset(szSockPath, 0, 128);
	memcpy(szSockPath, MOTION_SCK, strlen(MOTION_SCK));
	debug_printf("[%s]: socket path [%s]", __FILE__, szSockPath);
	
	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		debug_printf("[%s:%s:%d) open socket error (%s)\n", 
						__func__, __FILE__, __LINE__, 
						strerror(errno));
		return -1;
	}
	
    memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;     
	strncpy(sunx.sun_path, szSockPath, strlen(szSockPath));
                                                         
	while (connect(fd, (struct sockaddr *)&sunx, sizeof(sunx.sun_family) + strlen(sunx.sun_path)) != 0)
	{
	    debug_printf("%s %d connect[%s]..\n", __FILE__, __LINE__, szSockPath);
	    sleep(1);
	}
	
	return fd;

error_exit:
	close(fd);
	
	return -1;
}
*/
void sig_handle(int iSig)
{	
	pid_t	pid;
	int stat;
	if (iSig == SIGHUP)
	{
		g_bReload = TRUE;
	}
	else if (iSig == SIGCHLD)
	{
		while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
			;

	}
	return;
}
#if 0
void sig_chld(int signo)
{
	pid_t	pid;
	int stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		;	
	//printf("child %d terminated\n", pid);
	return;
}
#endif
//Register function for signl hanlder to catch sinal 
typedef	void	Sigfunc(int);	/* for signal handlers */
void register_signal_handler(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;
	
	act.sa_handler = func;
	sigfillset (&act.sa_mask);
	
	act.sa_flags = 0;
	if (signo != SIGALRM) {

#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;

#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
		debug_printf("signal hanlder register error\n");
	return;
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



int main(int argc, char* argv[])
{
    BOOL bIsDaemon = FALSE;
	int		i;
	//int iRet;
	int nfds;
	struct timeval timeout;	
	fd_set readfds;
	DWORD dwTimeStampBuf =0;
	struct msghdr MsgHeader;
    struct iovec iov[2];
    DWORD dwTimeStamp;
    int iSnapshotPathLen = 128;
	char szEventSnapshotPath[iSnapshotPathLen];
	struct sockaddr_un	sunx;
	int fd;
	 sigset_t block_mask;
	//syslog(LOG_INFO, "Start eventd process (Jeff Liao 15) with Pid : %d\n", getpid());
	
	while ((i=getopt(argc, argv, "Dp:h")) != -1)
	{
		switch(i)
		{
            case 'D':
                bIsDaemon = TRUE;
                break;
            case 'p':
                g_szPidFile = strdup(optarg);
                break;
			case 'h':
			default:
				print_usage();
				exit(1);
		}
	}

    /* run as Daemon*/
    if (bIsDaemon)
    {
	daemon_proc=1;  
	daemonize();
	//daemon(0, 0);
    }
    err_msg("Start eventd process (Jeff Liao 17) with Pid : %d\n", getpid());
    /* If the g_szPidFile is assigned write pid to g_szPidFile */
    if (g_szPidFile != NULL)
    {
        if (genPIDfile(g_szPidFile) != 1)
        {
            fprintf(stderr, "Generate pid file fail !!\n");
            exit(1);
        }
    }
   	
	//signal(SIGCHLD, sig_handle);
	//signal(SIGHUP, sig_handle);
	//Register signal hanlders
	register_signal_handler(SIGCHLD, sig_handle);
	register_signal_handler(SIGHUP, sig_handle);
	//int fd = ConnectSocket();
	     sigemptyset(&block_mask);
     //Block SIGQUIT (Terminal quit (POSIX)). The user can not use "Ctrl+\" to terminate the procgram.
     sigaddset(&block_mask,SIGQUIT);
     //Block SIGINT (Terminal interrupt (ANSI)). The user can not use "Ctrl+C" to terminate the procgram.
      sigaddset(&block_mask, SIGINT);
     sigprocmask (SIG_BLOCK, &block_mask, NULL);





	fd = MsgUtil_CreateSocket(&sunx);
	if (fd >0)
	{
		debug_printf("connect! fd = %d\n", fd);
	}
	else
	{
		fprintf(stderr, "connect fail!\n");
		exit(1);
	}
	
    
	//DWORD dwAlarmInterval = 3;
	if (LoadConfig(&tEventdInfo) != S_OK)
	{
		fprintf(stderr, "[eventd] load config error!!!\n");
		exit(1);
	}
	while (1)
	{
		if (g_bReload)
		{
			
			if (LoadConfig(&tEventdInfo) != S_OK)
			{
				err_msg("evntd reload config error!!!\n");
			}
			err_msg("evntd has been reconfigured...\n");
			g_bReload = FALSE;
		}
		debug_printf("enable:%d, schedule:%d, (%d,%d,%d,%d,%d,%d,%d), (%d,%d), email:%d, ftp:%d, interval:%d\n", 	
	tEventdInfo.bEnable, tEventdInfo.bSchedule, 
	tEventdInfo.atSchedule[0].abWeekday[1] , tEventdInfo.atSchedule[0].abWeekday[2],
	tEventdInfo.atSchedule[0].abWeekday[3] , tEventdInfo.atSchedule[0].abWeekday[4],
	tEventdInfo.atSchedule[0].abWeekday[5] , tEventdInfo.atSchedule[0].abWeekday[6],
	tEventdInfo.atSchedule[0].abWeekday[0] , tEventdInfo.atSchedule[0].dwStartMinute,
	tEventdInfo.atSchedule[0].dwEndMinute,
	tEventdInfo.tAction.bEmail, tEventdInfo.tAction.bFTP, tEventdInfo.tAction.dwAlarmInterval);
	debug_printf("==================\n");
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		nfds = select(fd+1, &readfds, NULL, NULL, &timeout);
		if (nfds > 0)
		{
			int iRecvlen;
			memset(&MsgHeader, 0, sizeof(struct msghdr));
			MsgHeader.msg_name = &sunx;
			MsgHeader.msg_namelen = sizeof(sunx);
			iov[0].iov_base = &dwTimeStamp;
			iov[0].iov_len  = sizeof(DWORD);
			iov[1].iov_base = szEventSnapshotPath;
			iov[1].iov_len  = iSnapshotPathLen;
			MsgHeader.msg_iov = iov;
			MsgHeader.msg_iovlen = 2;
			MsgHeader.msg_control = NULL;
			MsgHeader.msg_controllen = 0;
			MsgHeader.msg_flags = 0;
			memset(szEventSnapshotPath, 0, iSnapshotPathLen);
			
			iRecvlen = recvmsg(fd, &MsgHeader, 0);
			if (iRecvlen <= 0)
			{
				fprintf(stderr, "error! iRecvlen = %d, errno =%d\n", iRecvlen, errno);
				continue;
			}
			else
			{
				tEventdInfo.tAction.szEventSnapshotPath = szEventSnapshotPath;
			}
			if ((dwTimeStamp - dwTimeStampBuf ) < tEventdInfo.tAction.dwAlarmInterval)
			{
				printf("omit the same motion event\n");
				//Because venc_motion always create *.jpg, we must remove it.
				while (unlink(szEventSnapshotPath) < 0)			 
				{
					usleep(5000);
				}
				
			}
			else
			{
				if(!tEventdInfo.bEnable)
				{

					//Because venc_motion always create *.jpg, we must remove it.
					while (unlink(szEventSnapshotPath) < 0)			 
					{
						usleep(5000);
					}

	
				}
				else
				{
					if (!tEventdInfo.bSchedule)
					{
						TakeActions(&tEventdInfo.tAction);
					}
					else
					{
						TSchedule* ptSchedule;
						// get day of week
						struct tm *t;
						time_t current_time;
						(void) time(&current_time);
						t = localtime(&current_time);			
						// check every rule
						for (i=0; i<EVENT_ACTION_MAX_RULE_NUM; i++)
						{
							ptSchedule = &tEventdInfo.atSchedule[i];
							if (ptSchedule->abWeekday[t->tm_wday])
							{
								int iCurMin = t->tm_hour *60 + t->tm_min;
								if ((ptSchedule->dwStartMinute<iCurMin) && (iCurMin<ptSchedule->dwEndMinute))
								{
									TakeActions(&tEventdInfo.tAction);
								}else{
						//Because venc_motion always create *.jpg, we must remove it.
									while (unlink(szEventSnapshotPath) < 0)			 
									{
				
									usleep(5000);
									}

								}
							}else{
								while (unlink(szEventSnapshotPath) < 0)			 
								{
				
									usleep(5000);
								}

							}
						}
					}
				}
				dwTimeStampBuf = dwTimeStamp;
			}
//Now do removing jpeg file in TakeActions()			
#if 0					
			while ((iRet = unlink(szEventSnapshotPath)) < 0)			 
			{
				debug_printf("unlink %s iRet = %d, errno = %d\n", szEventSnapshotPath, iRet, errno);
				usleep(5000);
			}
#endif	
		}
		else if (nfds <0)
		{
			if (errno == EINTR)
			{
				continue;			
			}
			else			
			{
				err_sys("select error");
			}		
						
			//printf("select <0\n");

		}
		else
		{
			printf("select =0\n");
		}
	}
    if (g_szPidFile != NULL)
    {
        delPIDfile(g_szPidFile);
    }
	syslog(LOG_INFO, "Stop event process !!\n");

	return 0;
}

