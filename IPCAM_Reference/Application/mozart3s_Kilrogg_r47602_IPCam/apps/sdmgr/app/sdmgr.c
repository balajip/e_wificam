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

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/socket.h> 
#include <asm/types.h>
#include <sys/mount.h>

#include <linux/netlink.h> 




#include "dbgdefs.h"
#include "errordef.h"
#include "typedef.h"

#define SD_REMOVE_SIGNATURE "remove@/block/mmcblk"
#define SD_ADD_SIGNATURE "add@/block/mmcblk"
#define SD_MOUNT_POINT "/tmp/vrecord/videoclips"
#define SD_DEV_NAME "/dev/mmcblk"

typedef enum sdmgr_command_type
{
  rscRecoderShutDown,
  rscReloadConfigFile,
  rscNoCommand
  
} ESDMgrCmd;

static ESDMgrCmd g_iCommand=rscNoCommand;
static BOOLEAN g_bIsDaemon=FALSE;

void sdmgr_sighandler(int signo)
{
	if (signo == SIGINT)//SIGINT	2
	{	//printf("Got the SIGINT(%d)\n",SIGINT);
		g_iCommand = rscRecoderShutDown;
	}
	else if (signo == SIGHUP)//SIGHUP==1
	{		
		//printf("Got the SIGHUP(%d)\n",SIGHUP);
		g_iCommand = rscReloadConfigFile;
	}
}
void sdmgr_signal(int signum, void (*handler)(int))
{
	struct sigaction sigaInst;
	
	sigaInst.sa_flags = 0;
	sigfillset(&sigaInst.sa_mask);
	sigaInst.sa_handler = handler;

	sigaction(signum, &sigaInst, NULL);
}
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
        //fchmod(fileno(pid_fp), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
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

void print_usage(void)
{
	fprintf(stderr,	"Usage:\n"
					"    sdmgr [-D] [-h] [-p pidfile]\n"
					"Options:\n"
					"    -D                 Run as Daemon\n"
		           	"    -p pid_file        Write PID to this file\n"
					"    -h                 This help\n");
}

void  sdmgr_loop(void);
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
int main(int argc, char *argv[])
{
    int		i;
    BOOLEAN	bIsDaemon = FALSE;
    char* 	szPidFile = NULL;

    
    while ((i=getopt(argc, argv, "Dp:h")) != -1)
    {
	switch(i)
	{
            case 'D':
                bIsDaemon = TRUE;
                break;
            case 'p':
                szPidFile = strdup(optarg);
                break;
	    case 'h':
	    default:
		print_usage();
		goto exit_program;
	}
    }
    if(szPidFile == NULL)
    {
     
	print_usage();
	exit(1);
     
    }
    if (bIsDaemon)
    {
      g_bIsDaemon=TRUE;
      syslog(LOG_INFO, "[sdmgr](%d) Start sdmgr process with Pid : %d\n",__LINE__, getpid());
    }
   /* run as Daemon*/
    if (bIsDaemon)
    {
      daemonize();
     //   daemon(0, 0);
    }
    sdmgr_signal(SIGINT, sdmgr_sighandler);
    sdmgr_signal(SIGHUP, sdmgr_sighandler); 

    if (szPidFile != NULL)
    {
        if (genPIDfile(szPidFile) != S_OK)
        {
            fprintf(stderr, "Generate pid file fail !!\n");
            exit(1);
        }
    }

    sdmgr_loop();
  
exit_program:
	if( szPidFile != NULL)
	{
	    free( szPidFile );
	     szPidFile =NULL;
	}
	if (szPidFile != NULL)
	{
	  delPIDfile(szPidFile);
	}
	
	if (bIsDaemon)
	{
	  syslog(LOG_INFO, "[sdmgr](%d) Stop sdmgr process !!\n",__LINE__);
	}

  return 0;
}

static int init_hotplug_sock(void)
{
    struct sockaddr_nl snl;
    const int buffersize= 16 * 1024 * 1024;
    int retval;
 
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
 
    int hotplug_sock= socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if(hotplug_sock== -1) {
        printf("error getting socket: %s", strerror(errno));
        return-1;
    }
 
    /* set receive buffersize */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
 
    retval= bind(hotplug_sock, (struct sockaddr*) &snl, sizeof(struct sockaddr_nl));
    if(retval< 0) {
        printf("bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock= -1;
        return-1;
    }
 
    return hotplug_sock;
}
static SCODE create_tmpfile(char *szTmpfileName)
{

  int fd;
  
  if(szTmpfileName==NULL)
  {
      printf("(%d)szTmpfileName is null!\n",__LINE__);
      return S_FAIL;
  }
  //printf("[sdmgr](%d)\n",__LINE__);
  /* maybe get the tmpdir env variable instead of using /tmp */
  strcpy(szTmpfileName,"/tmp/sdmgr-XXXXXX"); 
 // printf("[sdmgr](%d)\n",__LINE__);
  umask(0077); // ensure exclusive access on buggy implementations of mkstemp
  fd = mkstemp(szTmpfileName);
  //printf("[sdmgr](%d)szTmpfileName=%s\n",__LINE__,szTmpfileName);
  close(fd);
  
  return S_OK;
 
}
static void  u_sleep(int usec)
{
  struct timeval tv;
  tv.tv_sec=0;
  tv.tv_usec=usec;
  
  select(0,NULL,NULL,NULL,&tv);

  return;
}

static BOOLEAN check_is_sd_attached()
{
    char  szSDDevName[16];
    char  szTemp3[24];
    char  szCommand[128];
    char  szTmpFileName[64];
    FILE *fp=NULL;
    BOOLEAN bAttached=FALSE;
    
    memset(szSDDevName,'\0',16);
    memset(szTemp3,'\0',24);
    memset(szCommand,'\0',128);
    memset(szTmpFileName,'\0',64);
    
    create_tmpfile(szTmpFileName);
    
    
    u_sleep(10);
    
    snprintf(szCommand,128,"cat /proc/partitions |  grep -r \"mmcblk\" | tail -n 1 |  awk -F' ' '{ print $4}' > %s",szTmpFileName);
    
    system(szCommand);
     
    fp=fopen(szTmpFileName,"r");
    if(fp != NULL)
    {
      
      // /dev/mmcblk0           3940828         4   3940824   0% /mnt/ramdisk
      fscanf(fp,"%s",szTemp3);
   
      fclose(fp);
     
    }
    
     
    strncpy(szSDDevName,szTemp3,strlen("mmcblk")+1);
     
    if (szSDDevName[0] != '\0')
    {
	   bAttached=TRUE;
    
    }
 
    unlink(szTmpFileName);
 
    return bAttached;
}


static BOOLEAN sd_mount_check(const char *szMountPoint,const char *szSDDevName)
{
	FILE	*f;
	char	buf[4096], *cp, *t;
	
	char buf1[1024];
	char buf2[1024];
	BOOLEAN bIsSDMount=FALSE;
	
	memset(buf,0x0,sizeof(buf));
	memset(buf1,0x0,sizeof(buf));
	memset(buf2,0x0,sizeof(buf));
	
	f = fopen("/proc/mounts", "r");
	if (!f)
	{
	  goto exit;
	}
	  
	while (!feof(f)) {
		
		if (!fgets(buf, sizeof(buf), f))
			break;
		
		
	      strncpy(buf1,strtok(buf," "),sizeof(buf1));
	  
	      strncpy(buf2,strtok(NULL," "),sizeof(buf2));
	      
	
		
		//printf("buf1=%s buf2=%s\n",buf1,buf2);
		if(strncmp(buf1,szSDDevName,strlen(szSDDevName))==0 && strncmp(buf2,szMountPoint,strlen(buf2))==0)
		{
		    bIsSDMount=TRUE;
		
		}
		memset(buf,0x0,sizeof(buf));
		memset(buf1,0x0,sizeof(buf));
		memset(buf2,0x0,sizeof(buf));

	}
exit:
      if(!f)	
      {
	fclose(f);
      }
      return bIsSDMount;
}

static void do_mount(const char *szMountPoint)
{
  char szCommand[128];
  
  if(szMountPoint==NULL)
  {
    if(g_bIsDaemon==TRUE)
    {
	  syslog(LOG_INFO,"[sdmgr](%d)szMountPoint is NULL!\n",__LINE__);
    }
    else
    {
	  printf("[sdmgr](%d)szMountPoint is NULL!\n",__LINE__);
    }
    
    return;
  }
  memset(szCommand,'\0',128);
  snprintf(szCommand,128,"ls -al %s > /dev/null 2>&1",szMountPoint);
  system(szCommand);
  
  u_sleep(10);
  
  
  memset(szCommand,'\0',128);
  snprintf(szCommand,128,"ls -al %s > /dev/null 2>&1",szMountPoint);
  system(szCommand);
  
  return;

}
SCODE UmountFS(const char *szRecordPath)
{
  system("sync;sync;sync");
  if(umount(szRecordPath)!=0)
  {
      if(g_bIsDaemon==TRUE)
      {
	  syslog(LOG_INFO,"[sdmgr](%d)Calling umoumt() to umount szMountPoint=%s  is failed!(%s)\n",__LINE__,szRecordPath,strerror(errno));
      }
      else
      {
	  printf("[sdmgr](%d)Calling umoumt() to umount szMountPoint=%s  is failed!(%s)\n",__LINE__,szRecordPath,strerror(errno));
      }
    
      return S_FAIL;
  }
  
  
  #if 0
   char  szCommand[128];
   struct stat stFile;
    char  szTmpFileName[16];
   // printf("(%d)\n",__LINE__);
   memset(szCommand,'\0',128);
memset(szTmpFileName,'\0',16);
   // printf("(%d)\n",__LINE__);
  
//  printf("(%d)umount %s\n",__LINE__,szRecordPath);
  system("sync;sync;sync");
 // usleep(200000); 
  create_tmpfile(szTmpFileName);
  snprintf(szCommand,128,"/bin/umount  %s  > %s 2>&1",szRecordPath,szTmpFileName);
  
  system(szCommand);
     //printf("(%d)\n",__LINE__);
    stat(szTmpFileName,&stFile);
     //printf("(%d)\n",__LINE__);
    unlink(szTmpFileName);
     //printf("(%d)\n",__LINE__);
    if (stFile.st_size !=0)
    {
	
	return S_FAIL;
    }
#if 0    
  if(umount(szRecordPath)!=0)
  {
      return S_FAIL;
  } 
#endif  
#endif
  return S_OK;
}
static void do_umount()
{
    
    if(sd_mount_check(SD_MOUNT_POINT,SD_DEV_NAME)==TRUE)
    {
	UmountFS(SD_MOUNT_POINT);
      
    }
   
    //system("/etc/init.d/autofs umountall > /dev/null 2>&1");
    
    return;
}



#define UEVENT_BUFFER_SIZE      2048


void  sdmgr_loop(void)
{

   int iSockFD=-1; 
   int iRet=-1;
   fd_set fdsRead;
   fd_set fdsRec;
   char buf[UEVENT_BUFFER_SIZE*2];
    
   memset(buf,'\0',UEVENT_BUFFER_SIZE*2);
	

   
   FD_ZERO(&fdsRead);
   FD_ZERO(&fdsRec);
    
   iSockFD=init_hotplug_sock();
    
   FD_SET(iSockFD,&fdsRead);
   
   
    if(iSockFD ==-1)
    {
	if(g_bIsDaemon==TRUE)
	{
	  syslog(LOG_INFO,"[sdmgr](%d)iSockFD==-1\n",__LINE__);
	}
	else
	{
	  printf("[sdmgr](%d)iSockFD==-1\n",__LINE__);
	}
	return;
    }
  
    if(check_is_sd_attached()==TRUE)
    {
        if(g_bIsDaemon==TRUE)
	{
	  syslog(LOG_INFO,"[sdmgr](%d)SD card has been attached!\n",__LINE__);
	}
	else
	{
	    printf("[sdmgr](%d)SD card has been attached!\n",__LINE__);
	}
	do_mount(SD_MOUNT_POINT);
    }
 
    
    while(g_iCommand!=rscRecoderShutDown)
    {
        
	fdsRec=fdsRead;
	
	iRet = select(iSockFD+1, &fdsRec, NULL, NULL, NULL);
	 memset(buf,'\0',UEVENT_BUFFER_SIZE*2);
	if(iRet>0)
	{
	
	   // recv(iSockFD, &buf, sizeof(buf), 0); 
	    
	    if(FD_ISSET(iSockFD, &fdsRec))
	    {
		read(iSockFD, buf, sizeof(buf));
		
	      
		
		if(memcmp(buf,SD_ADD_SIGNATURE,strlen(SD_ADD_SIGNATURE))==0)
		{
		
		  if(g_bIsDaemon==TRUE)
		  {
		     syslog(LOG_INFO,"[sdmgr](%d)SD card is attached!\n",__LINE__);
		  }
		  else
		  {
		    printf("[sdmgr](%d)SD card is attached!\n",__LINE__);
		  }
		  do_mount(SD_MOUNT_POINT);
		}
		else if(memcmp(buf,SD_REMOVE_SIGNATURE,strlen(SD_REMOVE_SIGNATURE))==0)
		{
		 
		  printf("[sdmgr](%d)SD card is detached!\n",__LINE__);
		  do_umount();
		  
		}
	
	    
	    }
	    
	 }
	 
	 
	
    }


  return;
}
