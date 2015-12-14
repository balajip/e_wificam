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
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/vfs.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <sys/select.h>
#include <mntent.h>
#include <semaphore.h>
#include <MemMgr.h>
#include "avicontainer.h"
#include "xmlwrapper.h"
#include "ShrdBufferSrc.h"
#include "SockSrc.h"
#include "global_codec.h"
#include "mp4.h"
//#include "SrcCommon.h"

#define DEFAULT_CONFIG_FILE		"vrec_conf.xml"
#define RECORD_FILE_PREFIX "Record"
/*Record length is 60 sec*/
#define RECORD_LEN 60
//%Y(4)%m(2)%d(2)%H(2)%M(2)%S(2) 4+2+2+2+2+2+1=15
#define RECORD_TIMESTAMP_LEN 15
#define RECORD_FILENAME_LEN 64
#define RECORD_FILEPATH_LEN 1024
//#define RECORD_FILE_LEN (1024*1024*1024)
#define CHECK_MAX_FILESIZE 0
#define VREC_SIGNATURE ".VREC_SIGNATURE"

#define MAX_UBUFFPAYLOAD_SIZE (1*1024*1024)
#define MAX_UBUFFHDR_SIZE (1*1024)

#define MAX_ARRAY_SIZE 600

typedef enum vrec_command_type
{
	rscRecoderShutDown,
	rscReloadConfigFile,
	rscNoCommand

} EVideoRecordCmd;

typedef enum vrec_format
{
	AVI_FORMAT=0,
	TREE_GP_FORMAT=1,
	MP4_FORMAT = 2,
	UNKOWN_FORMAT=255

}EVideoRecordFormat;

typedef enum vrec_bistream_mechanism
{
	MSHRDBUFFER=0,
	MSOCKET=1,
	UNKOWN_MECHANISM=255

}EBistreamMechanism;

#define REC_PATH_LEN 128

typedef struct bitstream_src
{
	char *szSrcPath;
	char *szCmdFifo;
	EBistreamMechanism eBistreamMechanism;
	HANDLE hBistreamSrc;
	BOOLEAN bEnable;
	DWORD dwIOID;

}TBitstreamSrc;

typedef struct vrec_info
{

	BOOLEAN bEnableRec;
	char *szRecordPath;
	char *szConfigPath;
	// char *szStorage;
	EVideoRecordFormat eRecForamt;
	BOOLEAN bEnableCyclicStorage;
	TBitstreamSrc tVideoBitstreamSrc;
	TBitstreamSrc tAudioBitstreamSrc;    
	//! options semaphore to protect option datas
	sem_t              	smOptions;
	//! Reset config option used, it will change to TRUE
	BOOL      	bReConf_flag;
	DWORD		dwRecLen;
	DWORD		dwRecSize;
	DWORD  dwSizeLimit;//the unit is KBytes

	TSrcUBufInfo UBufArray[MAX_ARRAY_SIZE*2];
	TSrcUBufInfo* ReceivedArray;
	TSrcUBufInfo* WrittingArray;

	int ReceivedUsedSize;
	int WrittingUsedSize;
	pthread_mutex_t VrecMutex;
	pthread_mutex_t CheckerMutex;
	pthread_cond_t VrecCond;
	pthread_cond_t CheckerCond;
	HANDLE hContainer;
	HANDLE MemMgrHandle;
	 //! MP4
	DWORD         dwMp4AudioTrackID;
	DWORD         dwMp4VideoTrackID;
}TVRecInfo;

FILE *OpenRecordFile(const char *szRecordFilePath,EVideoRecordFormat eVideoRecordFormat);

static EVideoRecordCmd g_iCommand=rscNoCommand;

void vrec_sighandler(int signo)
{
	if (signo == SIGINT)//SIGINT	2
	{	printf("Got the SIGINT(%d)\n",SIGINT);
		g_iCommand = rscRecoderShutDown;
	}
	else if (signo == SIGHUP)//SIGHUP==1
	{		
		printf("Got the SIGHUP(%d)\n",SIGHUP);
		g_iCommand = rscReloadConfigFile;
	}
}

void vrec_signal(int signum, void (*handler)(int))
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
			"    vrec [-D] [-c config_file][-h] [-p pidfile]\n"
			"Options:\n"
			"    -D                 Run as Daemon\n"
			"    -c config_file     Configuration file of Video Recoder\n"
			"    -p pid_file        Write PID to this file\n"
			"    -h                 This help\n");
}

static char* g_szPidFile = NULL;
static BOOLEAN bIsDaemon = FALSE;
static BOOLEAN bWriterRunning = TRUE;
static BOOLEAN bReceivedReconfig = FALSE;
static BOOLEAN bReconfigWriter = TRUE;
static BOOLEAN bInWrittingStage = FALSE;

//idx: 0-99 for buffer1, 100-199 for buffer2 
#define MEM_CHUNK_ARRAY_SIZE 100
#define MEM_CHUNK_SEG1_IDX 0
#define MEM_CHUNK_SEG2_IDX 50
#define MEM_CHUNK_MAX_SIZE (1024*1024)
unsigned char* MEMORY_CHUNKS_ARRAY[MEM_CHUNK_ARRAY_SIZE] = { NULL };
unsigned char* MEMORY_FREE_CHUNKS_ARRAY[MEM_CHUNK_ARRAY_SIZE] = { NULL };
int CUR_MEM_FREE_CHUNK_USED_SIZE = 0;
int CUR_MEM_CHUNK_IDX = 0;
int CUR_MEM_CHUNK_USED_SIZE = 0;
int CUR_WRITTING_SEG_IDX = 0;
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

static void VRecSetEnable(void *userData, const char *s, int len)
{
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	//printf("(%d)s=%s\n",__LINE__,s);
	if (!memcmp(s, "1", 1))
	{
		ptVRecInfo->bEnableRec = TRUE;
	}
	else
	{
		ptVRecInfo->bEnableRec = FALSE;
	}
	//  printf("(%d)ptVRecInfo->bEnableRec=%d\n",__LINE__,ptVRecInfo->bEnableRec);
	return;
}

static void VRecSetPath(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{
		ptVRecInfo->szRecordPath=strdup(str);
	}
	return;
}

static void VRecSetFormat(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{
		if(strncmp (str,"avi", strlen(str))==0)
		{
			ptVRecInfo->eRecForamt= AVI_FORMAT;
		}
		else if(strncmp (str,"3gp", strlen(str))==0)
		{
			ptVRecInfo->eRecForamt= TREE_GP_FORMAT;
		}
		else if(strncmp (str,"mp4", strlen(str))==0)
		{
			 ptVRecInfo->eRecForamt= MP4_FORMAT;
		}
		else
		{
			ptVRecInfo->eRecForamt= UNKOWN_FORMAT;
		}

	}
	return;
}

static void VRecSetCyclicStorage(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	if (!memcmp(s, "1", 1))
	{
		ptVRecInfo->bEnableCyclicStorage = TRUE;
	}
	else
	{
		ptVRecInfo->bEnableCyclicStorage = FALSE;
	}	
	return;
}

static void VRecSetVideoMechanism(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{
		if(strncmp (str,"sharedbuffer", strlen(str))==0)
		{   
			ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism= MSHRDBUFFER;
		}
		else if(strncmp (str,"socket", strlen(str))==0)
		{
			ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism= MSOCKET;
		}
		else
		{
			ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism= UNKOWN_MECHANISM;

		}

	}
	return;
}

static void VRecSetVideoBitstreamSrc(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->tVideoBitstreamSrc.szSrcPath=strdup(str);


	}
	return;
}


static void VRecSetVideoBitstreamCmdFifo(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->tVideoBitstreamSrc.szCmdFifo=strdup(str);


	}
	return;
}

static void VRecSetVideoBitstreamEnable(void *userData, const char *s, int len)
{
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	if (!memcmp(s, "1", 1))
	{
		ptVRecInfo->tVideoBitstreamSrc.bEnable = TRUE;
	}
	else
	{
		ptVRecInfo->tVideoBitstreamSrc.bEnable= FALSE;
	}	
	return;


}
static void VRecSetAudioMechanism(void *userData, const char *s, int len)
{
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{
		if(strncmp (str,"sharedbuffer", strlen(str))==0)
		{   
			ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism= MSHRDBUFFER;
		}
		else if(strncmp (str,"socket", strlen(str))==0)
		{
			ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism= MSOCKET;
		}
		else
		{
			ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism= UNKOWN_MECHANISM;

		}

	}

	return;
}

static void VRecSetAudioBitstreamSrc(void *userData, const char *s, int len)
{

	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->tAudioBitstreamSrc.szSrcPath=strdup(str);


	}
	return;
}


static void VRecSetAudioBitstreamCmdFifo(void *userData, const char *s, int len)
{


	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char str[len+1];
	memcpy(str, s, len);
	str[len]='\0';
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->tAudioBitstreamSrc.szCmdFifo=strdup(str);


	}
	return;
}

static void VRecSetAudioBitstreamEnable(void *userData, const char *s, int len)
{
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	if (!memcmp(s, "1", 1))
	{
		ptVRecInfo->tAudioBitstreamSrc.bEnable = TRUE;
	}
	else
	{
		ptVRecInfo->tAudioBitstreamSrc.bEnable= FALSE;
	}	
	return;


}

static void VRecSetAudioBitstreamIOID(void *userData, const char *s, int len)
{
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	char szTemp[len+1];
	DWORD dwIOID;
	memset(szTemp,0x0,sizeof(szTemp));
	strncpy(szTemp,s,len);

	dwIOID = strtoul(szTemp, (char**)NULL, 10);
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->tAudioBitstreamSrc.dwIOID=dwIOID;
	}
	return; 

}
static void VRecSetSwitch(void *userData, const char *s, int len)
{
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	static DWORD dwCurrSwitch =-1;
	DWORD dwSwitch = strtoul(s, (char**)NULL, 10);
	if (dwCurrSwitch != dwSwitch) {

		dwCurrSwitch = dwSwitch;
	}

	sem_wait(&(ptVRecInfo->smOptions));
	ptVRecInfo->bReConf_flag = TRUE;
	sem_post(&(ptVRecInfo->smOptions));

	return;
}

static void VRecSetLen(void *userData, const char *s, int len)
{  
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	DWORD dwRecLen = strtoul(s, (char**)NULL, 10);
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->dwRecLen=dwRecLen;
	}
	return;   
}
static void VRecSetSizeLimit(void *userData, const char *s, int len)
{  
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	DWORD dwSizeLimit = strtoul(s, (char**)NULL, 10);
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->dwSizeLimit=dwSizeLimit;
	}
	return;   
}
static void VRecSetSize(void *userData, const char *s, int len)
{  
	TVRecInfo* ptVRecInfo = (TVRecInfo*)userData;
	DWORD dwRecSize = strtoul(s, (char**)NULL, 10);
	if(ptVRecInfo!=NULL)
	{

		ptVRecInfo->dwRecSize=dwRecSize;
	}
	return;   
}
SCODE LoadConfig(TVRecInfo *ptVRecInfo)
{
	int sRet;
	HANDLE hXmlWrapObj;	
	TXmlWrapperTreeMap ptParsingMap[] = 
	{
		{"root/switch",NULL,&VRecSetSwitch,NULL},
		{"root/record", NULL, &VRecSetEnable, NULL},
		{"root/path", NULL, &VRecSetPath, NULL},
		{"root/format", NULL,&VRecSetFormat, NULL},
		{"root/length",NULL,&VRecSetLen,NULL},
		{"root/size",NULL,&VRecSetSize,NULL},
		{"root/reversedSize",NULL,&VRecSetSizeLimit,NULL},
		{"root/cyclic_storage", NULL, &VRecSetCyclicStorage, NULL},
		//{"root/storage", NULL, &VRecSetStorage, NULL},
		//{"root/filesystem", NULL, &VRecSetFilesystem, NULL},
		{"root/video_bitstream/mechanism",NULL,&VRecSetVideoMechanism,NULL},
		{"root/video_bitstream/path",NULL,&VRecSetVideoBitstreamSrc,NULL},
		{"root/video_bitstream/cmd_fifo",NULL,&VRecSetVideoBitstreamCmdFifo,NULL},
		{"root/video_bitstream/enable",NULL,&VRecSetVideoBitstreamEnable,NULL},
		{"root/audio_bitstream/mechanism",NULL,&VRecSetAudioMechanism,NULL},
		{"root/audio_bitstream/path",NULL,&VRecSetAudioBitstreamSrc,NULL},
		{"root/audio_bitstream/cmd_fifo",NULL,&VRecSetAudioBitstreamCmdFifo,NULL},
		{"root/audio_bitstream/enable",NULL,&VRecSetAudioBitstreamEnable,NULL},
		{"root/audio_bitstream/ioid",NULL,&VRecSetAudioBitstreamIOID,NULL},
		{NULL, NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;

	if (ptVRecInfo==NULL)
	{
		printf("%s %d : ptVRecInfo is NULL! \n", __FILE__, __LINE__);
		return S_FAIL;
	}

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
	if ((sRet=XmlWrapper_ReadFile_UsrDefFunc(ptVRecInfo->szConfigPath, hXmlWrapObj, ptVRecInfo)) != S_OK)
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


SCODE vrec_initial(TVRecInfo** pptVRecInfo, const char *szConfigFile)
{
	TShrdBufSrcInitOpts tShrdBufSrcInitOpts;
	TSockSrcInitOpts tSockSrcInitOpts;

	TVRecInfo *ptVRecInfo = (TVRecInfo *)malloc(sizeof(TVRecInfo));

	if(ptVRecInfo==NULL)
	{
		return S_FAIL;

	}

	*pptVRecInfo = ptVRecInfo;
	memset(ptVRecInfo, 0, sizeof(TVRecInfo));
	ptVRecInfo->szConfigPath=strdup(szConfigFile);
	if (sem_init(&(ptVRecInfo->smOptions), 0, 1) != 0) {
		return S_FAIL;
	}

	if (LoadConfig(ptVRecInfo)==S_FAIL)
	{
		return S_FAIL;
	}  
	if (ptVRecInfo->tVideoBitstreamSrc.bEnable == TRUE)  
	{
		if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism==MSHRDBUFFER)
		{
			memset(&tShrdBufSrcInitOpts,0x0,sizeof(TShrdBufSrcInitOpts));
			tShrdBufSrcInitOpts.szSharedBuffer=ptVRecInfo->tVideoBitstreamSrc.szSrcPath;
			tShrdBufSrcInitOpts.szCmdFiFoPath=ptVRecInfo->tVideoBitstreamSrc.szCmdFifo;
			if(ShrdBufferSrc_Initial(&ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc, &tShrdBufSrcInitOpts)==S_FAIL)
			{	 
				printf("[vrec](%d)ShrdBufferSrc_Initial() failed!!\n",__LINE__);
				return S_FAIL;
			}  


		}
		else  if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism==MSOCKET)
		{
			//Initialize socket for getting the video bistream.
			memset(&tSockSrcInitOpts,0x0,sizeof(TSockSrcInitOpts));
			tSockSrcInitOpts.szRecvSockPath=ptVRecInfo->tVideoBitstreamSrc.szSrcPath;
			tSockSrcInitOpts.szCmdFiFoPath=ptVRecInfo->tVideoBitstreamSrc.szCmdFifo;
			if(SockSrc_Initial(&ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,&tSockSrcInitOpts)==S_FAIL)
			{
				printf("[vrec](%d)SockSrc_Initial() failed!!\n",__LINE__);
				return S_FAIL;

			}


		}
		else
		{
			printf("(%d)The getting mechanism of video bistream is not supported!!\n",__LINE__);
			return S_FAIL;

		}  
	}
	if (ptVRecInfo->tAudioBitstreamSrc.bEnable == TRUE)  
	{
		if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSHRDBUFFER)
		{
			memset(&tShrdBufSrcInitOpts,0x0,sizeof(TShrdBufSrcInitOpts));
			tShrdBufSrcInitOpts.szSharedBuffer=ptVRecInfo->tAudioBitstreamSrc.szSrcPath;
			tShrdBufSrcInitOpts.szCmdFiFoPath=ptVRecInfo->tAudioBitstreamSrc.szCmdFifo;
			if(ShrdBufferSrc_Initial(&ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc, &tShrdBufSrcInitOpts)==S_FAIL)
			{
				return S_FAIL;
			}
		}
		else  if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSOCKET)
		{
			//Initialize socket for getting the audio bistream.

			memset(&tSockSrcInitOpts,0x0,sizeof(TSockSrcInitOpts));
			tSockSrcInitOpts.szRecvSockPath=ptVRecInfo->tAudioBitstreamSrc.szSrcPath;
			tSockSrcInitOpts.szCmdFiFoPath=ptVRecInfo->tAudioBitstreamSrc.szCmdFifo;
			tSockSrcInitOpts.dwIOID=ptVRecInfo->tAudioBitstreamSrc.dwIOID;
			if(SockSrc_Initial(&ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,&tSockSrcInitOpts)==S_FAIL)
			{
				printf("[vrec](%d)SockSrc_Initial() failed!!\n",__LINE__);
				return S_FAIL;

			}
		}
		else
		{
			printf("(%d)The getting mechanism of audio bistream is not supported!!\n",__LINE__);
			return S_FAIL;

		}  
	}

	pthread_mutex_init(&ptVRecInfo->VrecMutex, NULL);
	pthread_mutex_init(&ptVRecInfo->CheckerMutex, NULL);
	pthread_cond_init(&ptVRecInfo->VrecCond, NULL);
	pthread_cond_init(&ptVRecInfo->CheckerCond, NULL);
	ptVRecInfo->ReceivedUsedSize = 0;
	ptVRecInfo->WrittingUsedSize = 0;
	ptVRecInfo->ReceivedArray = ptVRecInfo->UBufArray;
	ptVRecInfo->WrittingArray = ptVRecInfo->UBufArray + MAX_ARRAY_SIZE;
	ptVRecInfo->hContainer = NULL;
#ifdef USE_EDMC_MEMORY
	{
		TMemMgrInitOptions option;
		option.dwVersion = MEMMGR_VERSION;
		option.pObjectMem = NULL;
		if (MemMgr_Initial(&ptVRecInfo->MemMgrHandle, &option) != S_OK)
		{
			printf("Initial memory manager failed\n");
			return S_FAIL;
		}
	}
#endif
	return S_OK;
}

SCODE vrec_release(TVRecInfo* ptVRecInfo)
{
	int idx;
	if(ptVRecInfo != NULL)
	{
		if(ptVRecInfo->szRecordPath != NULL)
		{
			free(ptVRecInfo->szRecordPath );
			ptVRecInfo->szRecordPath=NULL;
		}

		if (ptVRecInfo->szConfigPath != NULL)
		{
			free(ptVRecInfo->szConfigPath);
			ptVRecInfo->szConfigPath=NULL;
		}

		if ( ptVRecInfo->tAudioBitstreamSrc.szSrcPath != NULL)
		{

			free( ptVRecInfo->tAudioBitstreamSrc.szSrcPath);
			ptVRecInfo->tAudioBitstreamSrc.szSrcPath=NULL;
		}

		if (ptVRecInfo->tAudioBitstreamSrc.szCmdFifo != NULL)
		{
			free(ptVRecInfo->tAudioBitstreamSrc.szCmdFifo);
			ptVRecInfo->tAudioBitstreamSrc.szCmdFifo=NULL;

		}
		if ( ptVRecInfo->tVideoBitstreamSrc.szSrcPath != NULL)
		{

			free( ptVRecInfo->tVideoBitstreamSrc.szSrcPath);
			ptVRecInfo->tVideoBitstreamSrc.szSrcPath=NULL;
		}

		if (ptVRecInfo->tVideoBitstreamSrc.szCmdFifo != NULL)
		{
			free(ptVRecInfo->tVideoBitstreamSrc.szCmdFifo);
			ptVRecInfo->tVideoBitstreamSrc.szCmdFifo=NULL;

		}
		if (ptVRecInfo->tVideoBitstreamSrc.bEnable == TRUE)  
		{
			if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism==MSHRDBUFFER)
			{

				if(ShrdBufferSrc_Release(&ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc)==S_FAIL)
				{
					printf("[vrec](%d)ShrdBufferSrc_Release() failed!!\n",__LINE__);
					return S_FAIL;
				}  
			}
			else  if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism==MSOCKET)
			{
				//Release socket for getting the video bistream.
				if(SockSrc_Release(&ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc)==S_FAIL)
				{
					printf("[vrec](%d)SockSrc_Release() failed!!\n",__LINE__);
					return S_FAIL;
				}
			}
			else
			{
				printf("(%d)The getting mechanism of video bistream is not supported!!\n",__LINE__);
				return S_FAIL;
			}  
		}
		if (ptVRecInfo->tAudioBitstreamSrc.bEnable == TRUE)  
		{
			if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSHRDBUFFER)
			{
				if(ShrdBufferSrc_Release(&ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc)==S_FAIL)
				{
					return S_FAIL;
				}
			}
			else  if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSOCKET)
			{
				//Release socket for getting the audio bistream.
				if(SockSrc_Release(&ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc)==S_FAIL)
				{
					printf("[vrec](%d)SockSrc_Release() failed!!\n",__LINE__);
					return S_FAIL;
				}
			}
			else
			{
				printf("(%d)The getting mechanism of audio bistream is not supported!!\n",__LINE__);
				return S_FAIL;

			} 
		}  
		sem_destroy(&(ptVRecInfo->smOptions));

		for (idx = 0; idx < MEM_CHUNK_ARRAY_SIZE; ++idx)
		{
			if (MEMORY_CHUNKS_ARRAY[idx])
			{
#ifndef USE_EDMC_MEMORY
				free(MEMORY_CHUNKS_ARRAY[idx]);
#else
				MemMgr_FreeMemory(ptVRecInfo->MemMgrHandle, (DWORD)MEMORY_CHUNKS_ARRAY[idx]);
#endif
			}
		}

		for (idx = 0; idx < CUR_MEM_FREE_CHUNK_USED_SIZE; ++idx)
		{
			if (MEMORY_FREE_CHUNKS_ARRAY[idx])
			{
#ifndef USE_EDMC_MEMORY
				free(MEMORY_FREE_CHUNKS_ARRAY[idx]);
#else
				MemBroker_FreeMemory(MEMORY_FREE_CHUNKS_ARRAY[idx]);
#endif
			}
		}

		pthread_mutex_destroy(&ptVRecInfo->VrecMutex);
		pthread_mutex_destroy(&ptVRecInfo->CheckerMutex);
		pthread_cond_destroy(&ptVRecInfo->VrecCond);
		pthread_cond_destroy(&ptVRecInfo->CheckerCond);

#ifdef USE_EDMC_MEMORY
		if (ptVRecInfo->MemMgrHandle)
		{
			if (MemMgr_Release(&ptVRecInfo->MemMgrHandle) != S_OK)
				printf("Release MemMgr object fail !!\n");

			ptVRecInfo->MemMgrHandle = NULL;
		}
#endif

		free(ptVRecInfo);
		ptVRecInfo=NULL;
	}

	return S_OK;
}
static void vrec_show_compressionLaw(DWORD id)
{
	char szLaw[16];
	memset(szLaw,0x0,16);
	switch (id)
	{
		case FOURCC_ALAW:
			strncpy(szLaw,"alaw",strlen("alaw"));
			break;
		case FOURCC_ULAW:
			strncpy(szLaw,"ulaw",strlen("ulaw"));
			break;
		default:
			strncpy(szLaw,"UNKOWN",strlen("UNKOWN"));

	}
	printf("Compress law is %s\n",szLaw);
	return;
}
static void vrec_show_codec(DWORD id)
{
	char szCodec[16];
	memset(szCodec,0x0,16);
	switch (id)
	{
		case FOURCC_MP4V:
			strncpy(szCodec,"mp4v",strlen("mp4v"));
			break;
		case FOURCC_JPEG:
			strncpy(szCodec,"mjpeg",strlen("mjpeg"));
			break;
		case FOURCC_H264:
			strncpy(szCodec,"h264",strlen("h264"));
			break;
		case FOURCC_G711:
			strncpy(szCodec,"G711",strlen("G711"));
			break;
		case FOURCC_G726:
			strncpy(szCodec,"G726",strlen("G726"));
			break;
		case FOURCC_AAC4:
			strncpy(szCodec,"AAC4",strlen("AAC4"));
			break;
		case FOURCC_GAMR:
			strncpy(szCodec,"GAMR",strlen("GAMR"));
			break;
		default:
			strncpy(szCodec,"UNKOWN",strlen("UNKOWN"));
	}
	printf("Codec is %s\n",szCodec);
	return ;
}
static BOOLEAN vrec_check_codec(BYTE *pbyUBuffHdr,DWORD *pdwDataType,BOOLEAN bFromConf)
{
	BOOLEAN bRes=FALSE;
	DWORD dwCodecId=0;
	if(bFromConf==TRUE)
	{
		dwCodecId=((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType;

	}
	else
	{
		dwCodecId=((TUBuffer *)pbyUBuffHdr)->dwDataType;


	}
	if(dwCodecId != *pdwDataType)
	{
		*pdwDataType=dwCodecId;
		bRes=TRUE;
	}

	return bRes;
}

static BOOLEAN vrec_check_compression(BYTE *pbyUBuffHdr,DWORD *pdwCompressionLaw)
{
	BOOLEAN bRes=FALSE;


	if (FOURCC_G711 == ((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType) 
	{

		if(*pdwCompressionLaw != ((TUBufferConfG711 *)pbyUBuffHdr)->dwCompressionLaw)
		{
			//The compression law of audio bitstream has been changed.
			*pdwCompressionLaw=((TUBufferConfG711 *)pbyUBuffHdr)->dwCompressionLaw;
			bRes=TRUE;
		}
	}


	return bRes;
}
static BOOLEAN vrec_check_codewordBits(BYTE *pbyUBuffHdr,DWORD *pdwCodewordBits)
{
	BOOLEAN bRes=FALSE;

	if (FOURCC_G726== ((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType) 
	{
		if(*pdwCodewordBits != ((TUBufferConfG726 *)pbyUBuffHdr)->dwCodewordBits)
		{
			//The code word bits of audio bitstream has been changed.
			*pdwCodewordBits =((TUBufferConfG726 *)pbyUBuffHdr)->dwCodewordBits;
			bRes=TRUE;
		}

	}

	return bRes;

}

static BOOLEAN vrec_check_profileLevel(BYTE *pbyUBuffHdr,DWORD *pdwProfileLevel)
{
	BOOLEAN bRes=FALSE;

	if (FOURCC_AAC4== ((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType) 
	{
		if(*pdwProfileLevel != ((TUBufferConfAAC4 *)pbyUBuffHdr)->dwProfileLevel)
		{
			//The code word bits of audio bitstream has been changed.
			*pdwProfileLevel =((TUBufferConfAAC4  *)pbyUBuffHdr)->dwProfileLevel;
			bRes=TRUE;
		}

	}

	return bRes;

}
static BOOLEAN vrec_check_samplerate(BYTE *pbyUBuffHdr,DWORD *pdwSampleRate)
{
	BOOLEAN bRes=FALSE;

	if (FOURCC_GAMR== ((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType) 
	{
		if(*pdwSampleRate != ((TUBufferConfGAMR *)pbyUBuffHdr)->dwSampleRate)
		{
			//The sample rate of audio bitstream has been changed.
			*pdwSampleRate=((TUBufferConfGAMR *)pbyUBuffHdr)->dwSampleRate;
			bRes=TRUE;
		}

	}
	else if (FOURCC_AAC4 == ((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType) 
	{
		if(*pdwSampleRate != ((TUBufferConfAAC4 *)pbyUBuffHdr)->dwSampleRate)
		{
			//The sample rate of audio bitstream has been changed.
			*pdwSampleRate=((TUBufferConfAAC4  *)pbyUBuffHdr)->dwSampleRate;
			bRes=TRUE;
		}

	}



	return bRes;
}

static BOOLEAN vrec_check_resolution(BYTE *pbyUBuffHdr,DWORD *pdwWidth,DWORD *pdwHeight)
{
	DWORD dwCodecId=((TUBuffer *)pbyUBuffHdr)->dwFollowingDataType;
	BOOLEAN bRes=FALSE;
	switch(dwCodecId)
	{
		case FOURCC_MP4V:
			if( (((TUBufferConfMP4V *)pbyUBuffHdr)->dwWidth != *pdwWidth) || (((TUBufferConfMP4V *)pbyUBuffHdr)->dwHeight != *pdwHeight)) 
			{
				bRes=TRUE;
				*pdwWidth=((TUBufferConfMP4V *)pbyUBuffHdr)->dwWidth;
				*pdwHeight=((TUBufferConfMP4V *)pbyUBuffHdr)->dwHeight;
			}
			break;
		case FOURCC_H264:
			if( (((TUBufferConfH264 *)pbyUBuffHdr)->dwWidth != *pdwWidth) || (((TUBufferConfH264 *)pbyUBuffHdr)->dwHeight != *pdwHeight)) 
			{
				bRes=TRUE;
				*pdwWidth=((TUBufferConfH264 *)pbyUBuffHdr)->dwWidth;
				*pdwHeight=((TUBufferConfH264 *)pbyUBuffHdr)->dwHeight;
			}
			break;
		case FOURCC_JPEG:
			if( (((TUBufferConfJPEG *)pbyUBuffHdr)->dwWidth != *pdwWidth) || (((TUBufferConfJPEG *)pbyUBuffHdr)->dwHeight != *pdwHeight)) 
			{
				bRes=TRUE;
				*pdwWidth=((TUBufferConfJPEG *)pbyUBuffHdr)->dwWidth;
				*pdwHeight=((TUBufferConfJPEG *)pbyUBuffHdr)->dwHeight;
			}
			break;
		default:
			bRes=FALSE;

	}

	return bRes;
}

static SCODE vrec_init_container_handle(HANDLE *phContainer,EVideoRecordFormat eFormat)
{
	SCODE sRet=S_OK;
	SCODE sRetFromfunc=S_OK;
	
	if (eFormat == AVI_FORMAT)
	{	
		TAVICInitOptions tAVIInitOpt;
		memset(&tAVIInitOpt,0x0,sizeof( TAVICInitOptions));
		tAVIInitOpt.dwVersion = AVIC_VERSION;
//		tAVIInitOpt.bRecTimeStamp = FALSE;
		sRetFromfunc=AVIC_Initial(phContainer, &tAVIInitOpt) ;
		if (sRetFromfunc != S_OK) 
		{	
			sRet=S_FAIL;
			printf("[vrec](%d)AVIC_Initial() failed! (%x)\n", __LINE__,sRetFromfunc);
			goto exit;

		}

	}
	else if (eFormat == MP4_FORMAT)
	{
		TMP4IintOpt tOpt;
		tOpt.dwVersion = MP4_VERSION;
		sRetFromfunc = mp4_int(phContainer, &tOpt);
		if (S_OK != sRetFromfunc)
		{
			sRet = S_FAIL;
			printf("[vrec](%d)mp4_int() failed! (%x)\n", __LINE__,sRetFromfunc);
			goto exit;
		}
	}
	else if (eFormat == TREE_GP_FORMAT)
	{
		//TODO:We will support 3gp reocrd file format in the future.		
		printf("[vrec](%d)We will support 3gp reocrd file format in the future!\n",__LINE__);
	}
	else
	{
		printf("[vrec](%d)Unkown container format!\n",__LINE__);

	}
exit:
	return sRet;
}
static SCODE vrec_rls_container_handle(HANDLE *phContainer,EVideoRecordFormat eFormat)
{
	SCODE sRet=S_OK;
	SCODE sRetFromfunc=S_OK;
	if (eFormat == AVI_FORMAT)
	{
		sRetFromfunc=AVIC_Release(phContainer) ;
		if (sRetFromfunc != S_OK) 
		{	
			sRet=S_FAIL;
			printf("[vrec](%d)AVIC_Release() failed! (%x)\n", __LINE__,sRetFromfunc);
			goto exit;
		}
	}
	else if (eFormat == MP4_FORMAT)
	{
		sRetFromfunc = mp4_rls(phContainer);
		if (sRetFromfunc != S_OK) 
		{	
			sRet=S_FAIL;
			printf("[vrec](%d)mp4_rls() failed! (%x)\n", __LINE__,sRetFromfunc);
			goto exit;
		}
		
	}
	else if(eFormat== TREE_GP_FORMAT)
	{
		//TODO:We will support 3gp reocrd file format in the future.		
		printf("[vrec](%d)We will support 3gp reocrd file format in the future!\n",__LINE__);
	}
	else
	{
		printf("[vrec](%d)Unkown container format!\n",__LINE__);

	}

exit:

	return sRet;

}


typedef struct vrec_container_init_data
{
	DWORD dwVideoTrackNum;
	DWORD dwAudioTrackNum;
	DWORD dwMaxFileSize;
	BOOLEAN bCheckMaxFileSize;
	TSrcUBufInfo *ptVideoTrackUBufInfo;
	TSrcUBufInfo *ptAudioTrackUBufInfo;
	CHAR *szFilePath;
	EVideoRecordFormat eFormat;

} TContainerFileInitData;
static SCODE vrec_create_file(TVRecInfo* ptVRecInfo, HANDLE hContainer,TContainerFileInitData tInitData)
{
	SCODE sRet=S_OK;
	SCODE sRetFromfunc=S_OK;
	TAVICCreateOptions	tCreateOpt;

	if(tInitData.szFilePath==NULL)
	{
		printf("[vrec](%d)Given the record file path is NULL!\n",__LINE__);
		sRet=S_FAIL;
		goto exit;
	}

	if(tInitData.eFormat== AVI_FORMAT)
	{	
		memset(&tCreateOpt,0x0,sizeof(TAVICCreateOptions));
		tCreateOpt.bCheckMaxFileSize = tInitData.bCheckMaxFileSize;
		tCreateOpt.dwMaxFileSize = tInitData.dwMaxFileSize;
		tCreateOpt.szAVIFile=tInitData.szFilePath;
		printf("[vrec](%d) tCreateOpt.szAVIFile=%s\n",__LINE__, tCreateOpt.szAVIFile);
		printf("[vrec](%d)tInitData.dwVideoTrackNum=%d\n",__LINE__,tInitData.dwVideoTrackNum);
		printf("[vrec](%d)tInitData.dwAudioTrackNum=%d\n",__LINE__,tInitData.dwAudioTrackNum);
		tCreateOpt.dwVideoTrackNum = tInitData.dwVideoTrackNum;
		tCreateOpt.dwAudioTrackNum = tInitData.dwAudioTrackNum;
		printf("[vrec](%d)tInitData.ptVideoTrackUBufInfo=%p\n",__LINE__,tInitData.ptVideoTrackUBufInfo);
		printf("[vrec](%d)tInitData.ptAudioTrackUBufInfo=%p\n",__LINE__,tInitData.ptAudioTrackUBufInfo);
		tCreateOpt.ptVideoTrackUBufInfo = tInitData.ptVideoTrackUBufInfo;
		tCreateOpt.ptAudioTrackUBufInfo = tInitData.ptAudioTrackUBufInfo;

		sRetFromfunc=AVIC_CreateFile( hContainer, &tCreateOpt);

		//When vrec run firstly,  one fd (video or audio) will be selected firstly.
		//One file of the fd will be  created. Immediately the other fd  will be selected.The other file of the fd will be  created. 
		//Because we use "Reocrd_%Y%m%d%H%M%S.avi" as filename.The two filename will be the same. So we must deleted the first file.
		if(sRetFromfunc==ERR_OPEN_EXIST_FILE)
		{
			printf("[vrec](%d)Because %s exists! We remove it and create it.\n",__LINE__,tCreateOpt.szAVIFile);
			if(unlink( tCreateOpt.szAVIFile)!=0)
			{
				printf("[vrec](%d) Call unlink() to remove file %s is failed. (%s)\n",__LINE__,tCreateOpt.szAVIFile,strerror(errno));
				sRet=S_FAIL;
				goto exit;
			}
			sRetFromfunc=AVIC_CreateFile( hContainer, &tCreateOpt);
		}

		if( sRetFromfunc != S_OK)
		{
			printf("[vrec](%d)AVIC_CreateFile() is failed!(%x)\n",__LINE__,sRetFromfunc);
			sRet=S_FAIL;
		}


	}
	else if(tInitData.eFormat== TREE_GP_FORMAT)
	{    
		//TODO:We will support 3gp reocrd file format in the future.		
		printf("[vrec](%d)We will support 3gp reocrd file format in the future!\n",__LINE__);
		//sRet=S_FAIL;    
	}
	else if(tInitData.eFormat== MP4_FORMAT)
	{
		sRetFromfunc = mp4_create(hContainer, tInitData.szFilePath, 1000000);
		if (ERR_OPEN_EXIST_FILE == sRetFromfunc)
		{
			printf("[vrec](%d)Because %s exists! We remove it and create it.\n",__LINE__, tInitData.szFilePath);
			if (0 != unlink(tInitData.szFilePath))
			{
				printf("[vrec](%d) Call unlink() to remove file %s is failed. (%s)\n", __LINE__, tInitData.szFilePath, strerror(errno));
				sRet = S_FAIL;
				goto exit;
			}
			sRetFromfunc = mp4_create(hContainer, tInitData.szFilePath, 1000000);
		}
		
		if (S_OK == sRetFromfunc)
		{
			if (NULL != tInitData.ptVideoTrackUBufInfo)
				ptVRecInfo->dwMp4VideoTrackID = mp4_add_track(hContainer, (TMP4SrcUBufInfo *)tInitData.ptVideoTrackUBufInfo, 1000000);
			
			if (NULL != tInitData.ptAudioTrackUBufInfo)
			{
				DWORD dwFollowingDataType = ((TUBuffer *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))-> dwFollowingDataType;
				if (dwFollowingDataType == MAKEFOURCC('A','A','C','4'))
				{	
					ptVRecInfo->dwMp4AudioTrackID = mp4_add_track(hContainer, (TMP4SrcUBufInfo *)tInitData.ptAudioTrackUBufInfo, ((TUBufferConfAAC4 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwSampleRate);

					printf("[vrec](%d) audio trackid %d, chnum %d, sample rate %d\n",__LINE__, 
						((TUBufferConfAAC4 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwTrackID,
						((TUBufferConfAAC4 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwChannelNumber, 
						((TUBufferConfAAC4 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwSampleRate);
				}
				else if (dwFollowingDataType == MAKEFOURCC('G','7','1','1'))
				{
					ptVRecInfo->dwMp4AudioTrackID = mp4_add_track(hContainer, (TMP4SrcUBufInfo *)tInitData.ptAudioTrackUBufInfo, 8000);

					printf("[vrec]G711 (%d) audio trackid %u, chnum %u, sample rate %u\n",__LINE__, 
						((TUBufferConfG711 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwTrackID,
						((TUBufferConfG711 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwChannelNumber, 
						8000);
					
				}
				else if (dwFollowingDataType == MAKEFOURCC('G','A','M','R'))
				{
					ptVRecInfo->dwMp4AudioTrackID = mp4_add_track(hContainer, (TMP4SrcUBufInfo *)tInitData.ptAudioTrackUBufInfo, 8000);

					printf("[vrec]GAMR (%d) audio trackid %u, chnum %u, sample rate %u\n",__LINE__, 
						((TUBufferConfGAMR *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwTrackID,
						1, 
						8000);
					
				}
				else if (dwFollowingDataType == MAKEFOURCC('G','7','2','6'))
				{
					ptVRecInfo->dwMp4AudioTrackID = mp4_add_track(hContainer, (TMP4SrcUBufInfo *)tInitData.ptAudioTrackUBufInfo, 8000);


					printf("[vrec]G726 (%d) audio trackid %u, chnum %u, sample rate %u\n",__LINE__, 
						((TUBufferConfG726 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwTrackID,
						((TUBufferConfG726 *)(tInitData.ptAudioTrackUBufInfo->pbyUBuffHdr))->dwChannelNumber, 
						8000);
				}
				else
				{
					printf("[vrec](%d)unsupported audio codec type!!\n",__LINE__);
				}
				

			}
		}
	}
	else
	{
		printf("[vrec](%d)Unkown container format!\n",__LINE__);
		//sRet=S_FAIL;
	}


exit:

	return sRet;

}
static DWORD vrec_get_maxfilesize(EVideoRecordFormat eFormat)
{
	switch (eFormat)
	{
	case AVI_FORMAT:
		return ULONG_MAX;//In FAT32, the max file size of avi is 4G. 
		
	case TREE_GP_FORMAT:
		//TODO:We will support 3gp record file format in the future.
		printf("[vrec](%d)We will support 3gp record file format in the future!\n",__LINE__);
		return  (1*1024*1024*1024);//??1G
		
	case MP4_FORMAT:
		return ULONG_MAX;
		
	default:
		printf("[vrec](%d)Unkown container format!\n",__LINE__);
		return 0;
	}
	
	return 0;

}

static void vrec_get_codec_str(DWORD id,char *szCodec,int length)
{

	memset(szCodec,0x0,length);
	switch (id)
	{
		case FOURCC_MP4V:
			strncpy(szCodec,"MP4V",strlen("MP4V"));
			break;
		case FOURCC_JPEG:
			strncpy(szCodec,"MJPEG",strlen("MJPEG"));
			break;
		case FOURCC_H264:
			strncpy(szCodec,"H264",strlen("H264"));
			break;
		case FOURCC_G711:
			strncpy(szCodec,"G711",strlen("G711"));
			break;
		case FOURCC_G726:
			strncpy(szCodec,"G726",strlen("G726"));
			break;
		case FOURCC_AAC4:
			strncpy(szCodec,"AAC4",strlen("AAC4"));
			break;
		case FOURCC_GAMR:
			strncpy(szCodec,"GAMR",strlen("GAMR"));
			break;
		default:
			strncpy(szCodec,"NONE",strlen("NONE"));
	}
	// printf("Codec is %s\n",szCodec);
	return ;
}
static SCODE vrec_gen_filepath(time_t curtime,const char *szRecordPath, EVideoRecordFormat eFormat,char *szRecordFilePath,DWORD dwAudioDatatype,DWORD dwVideoDatatype)
{

	char szRecordFileName[ RECORD_FILENAME_LEN];
	char szAudioCodec[16];
	char szVideoCodec[16];
	char szRecordType[16];

	// static DWORD dwIdex=0;
	if(szRecordFilePath==NULL)
	{
		printf("[vrec](%d)Given the record file path is NULL!\n",__LINE__);
		return S_FAIL;
	}
	memset(szRecordFileName,'\0', RECORD_FILENAME_LEN);
	memset(szRecordFilePath,'\0',RECORD_FILEPATH_LEN);
	memset(szRecordType,'\0',16);
	memset(szAudioCodec,'\0',16);
	memset(szVideoCodec,'\0',16);

	vrec_get_codec_str(dwAudioDatatype,szAudioCodec,16);
	vrec_get_codec_str(dwVideoDatatype,szVideoCodec,16);
	if (eFormat == AVI_FORMAT)
	{
		//strftime (szRecordFileName,  RECORD_FILENAME_LEN, "%Y%m%d%H%M%S.avi", localtime(&curtime));
		strncpy(szRecordType,"avi",strlen("avi"));
	}
	else if (eFormat == TREE_GP_FORMAT)
	{
		//strftime (szRecordFileName,  RECORD_FILENAME_LEN, "%Y%m%d%H%M%S.3gp", localtime(&curtime));
		strncpy(szRecordType,"3gp",strlen("3gp"));
	}
	else if (eFormat == MP4_FORMAT)
	{
		strncpy(szRecordType,"mp4",strlen("mp4"));
	}
	else
	{
		printf("[vrec](%d)Unkown container format!\n",__LINE__);
		strncpy(szRecordType,"unkown",strlen("unkown"));
		//strftime (szRecordFileName,  RECORD_FILENAME_LEN, "%Y%m%d%H%M%S.unkown", localtime(&curtime));
	}

	strftime (szRecordFileName,  RECORD_FILENAME_LEN, "%Y%m%d%H%M%S", localtime(&curtime));
	//snprintf(szRecordFilePath,RECORD_FILEPATH_LEN,"%s/%u_%s",szRecordPath,dwIdex,szRecordFileName);
	//Record_%Y%m%d%H%M%S_[audio_codec]_[video_codec].[record_container]
	snprintf(szRecordFilePath,RECORD_FILEPATH_LEN,"%s/%s_%s_%s_%s.%s",szRecordPath,RECORD_FILE_PREFIX,szRecordFileName,szAudioCodec,szVideoCodec,szRecordType);      
	//  printf("[vrec](%d)szRecordFilePath=%s szRecordFileName=%s szRecordPath=%s\n",__LINE__,szRecordFilePath,szRecordFileName,szRecordPath);
	//dwIdex++;

	return S_OK;

}

static SCODE vrec_remove_recordfile(const char* szRecordPath, const char *szRecordFilePath)
{
	SCODE sRet=S_OK;

	//The oldest file could be *.3gp or *.avi
	//Remove *.3gp or *.avi
	char  szCommand[128];
	if (szRecordFilePath != NULL)
	{
		snprintf(szCommand,128,"/bin/rm -f %s/%s*",szRecordPath, szRecordFilePath);
		system(szCommand);
	}

	return sRet;
}

static BOOLEAN vrec_storage_is_full(const char *szRecordPath,DWORD dwReversedSize)
{
	BOOLEAN isFull=FALSE;
	unsigned long avalible_size=0;
	struct statfs s;
	if (statfs(szRecordPath, &s) != 0) {
		printf("[vrec](%d)Call statfs for checking the capability of  %s is failed \n",__LINE__,szRecordPath);
		return TRUE;
	}

	avalible_size = (s.f_bavail >> 10) * (s.f_bsize >> 10);
	isFull = (avalible_size <= dwReversedSize);
	printf("[vrec](%d) %s avalible size: %9lu MBytes (smaller than %u MBytes is %d)\n",__LINE__,szRecordPath,avalible_size,dwReversedSize,isFull);
	if(isFull )
	{
		printf("[vrec](%d)Because %9lu Kbytes (avalible size) smaller than %u Mbytes, %s is full\n",__LINE__,avalible_size,dwReversedSize,szRecordPath);
		return TRUE;
	}

	return FALSE;
}
//The valid filename looks like:  Reocrd_%Y%m%d%H%M%S_[AudioCodec]_[VideoCodec].avi.
static BOOLEAN is_valid_record_filename(const char *szName)
{
	BOOLEAN bRet=FALSE;
	BOOLEAN bIsValidPrefix=FALSE;
	BOOLEAN bIsValidTimeStamp=FALSE;
	int iCheckTriggerTimeCount=0;
	char szFileName[RECORD_FILENAME_LEN];
	char szTriggerTime[RECORD_TIMESTAMP_LEN];
	char szYear[5],szMon[3],szMday[3],szHour[3],szMin[3],szSec[3];
	char szRecordPrefix[16];
	char *szTmp;

	memset(szRecordPrefix,'\0',16);
	memset(szFileName,'\0',RECORD_FILENAME_LEN);
	memset(szTriggerTime,'\0',RECORD_TIMESTAMP_LEN);

	strncpy(szFileName,szName,strlen(szName));
	//printf("%d\n",__LINE__);
	szTmp=strtok(szFileName,"_.");
	if(szTmp!=NULL)
	{
		strncpy(szRecordPrefix, szTmp,16);
		//printf("%dszRecordPrefix=%s\n",__LINE__,szRecordPrefix);

		if(strlen(RECORD_FILE_PREFIX)==strlen(szRecordPrefix))
		{
			if(strncmp(szRecordPrefix,RECORD_FILE_PREFIX,strlen(RECORD_FILE_PREFIX))==0)
			{
				bIsValidPrefix=TRUE;
			}
		}
	}

	//printf("%d\n",__LINE__);
	szTmp=strtok(NULL,"_.");
	if(szTmp!=NULL)
	{
		strncpy(szTriggerTime,szTmp,RECORD_TIMESTAMP_LEN);
		//printf("%d szTriggerTime=%s\n",__LINE__,szTriggerTime);

		//%Y(4)%m(2,01~12)%d(2,01~31)%H(2,00~23)%M(2,00~59)%S(2,00~61)
		if(strlen(szTriggerTime)==(RECORD_TIMESTAMP_LEN-1)&&szTriggerTime!=NULL)
		{	
			iCheckTriggerTimeCount=0;
			strncpy(szYear,(char *)szTriggerTime,4);
			strncpy(szMon,(char *)szTriggerTime+4,2);
			strncpy(szMday,(char *)szTriggerTime+4+2,2);
			strncpy(szHour,(char *)szTriggerTime+4+2+2,2);
			strncpy(szMin,(char *)szTriggerTime+4+2+2+2,2);
			strncpy(szSec,(char *)szTriggerTime+4+2+2+2+2,2);
			if(strncmp(szYear,"0000",4)!=0)
			{
				if(atoi(szYear)!=0)
				{
					iCheckTriggerTimeCount++;//1
				}  

			}
			if(strncmp(szMon,"00",2)!=0)
			{
				if(atoi(szMon)!=0)
				{
					if(atoi(szMon)>=1 && atoi(szMon) <= 12)
					{
						iCheckTriggerTimeCount++;//2
					}	    
				}  

			}
			if(strncmp(szMday,"00",2)!=0)
			{
				if(atoi(szMday)!=0)
				{ 
					if(atoi(szMday)>=1 && atoi(szMday) <= 31)
					{
						iCheckTriggerTimeCount++;//3
					}	 
				}  

			}
			if(atoi(szHour)>=0 && atoi(szHour) <= 23)
			{
				iCheckTriggerTimeCount++;//4
			}	
			if(atoi(szMin)>=0 && atoi(szMin) <= 59)
			{
				iCheckTriggerTimeCount++;//5
			}
			if(atoi(szSec)>=0 && atoi(szSec) <= 61)
			{
				iCheckTriggerTimeCount++;//6
			}
			if(iCheckTriggerTimeCount==6)
			{
				bIsValidTimeStamp=TRUE;
			}

		}  
	}


	if(bIsValidPrefix==TRUE && bIsValidTimeStamp==TRUE)
	{
		bRet=TRUE;
	}

	return bRet;

}

static SCODE vrec_check_oldest_recordfile(const char *szRecordPath, DWORD dwReversedSize)
{
	SCODE ret = S_OK;
	char szOldestRescordFilePath[RECORD_FILEPATH_LEN];

	struct dirent **namelist;
	//n includes "." and ".."
	int n = scandir(szRecordPath, &namelist, 0, alphasort);
	printf("[vrec](%d)There are %d files in %s\n",__LINE__, n - 2, szRecordPath);

	if (n > 0)
	{
		//The oldest file could be *.3gp or *.avi
		int file_idx;
		for (file_idx = 0; file_idx < n; ++file_idx)
		{
			if (is_valid_record_filename(namelist[file_idx]->d_name)==FALSE)
				continue;
			//!!!! warning important!!!!!!!
			// the filename pattern is Record_20130802135608_xxxxx.avi, the length is 19...
			strncpy(szOldestRescordFilePath, namelist[file_idx]->d_name, 19);
			printf("[vrec](%d)szOldestRescordFilePath=%s\n",__LINE__,szOldestRescordFilePath);
			if (vrec_remove_recordfile(szRecordPath, szOldestRescordFilePath) == S_FAIL)
			{
				printf("[vrec](%d)Call vrec_remove_recordfile() to remove file %s is failed. And the storage %s is full. So can not save more recording files.\n",__LINE__,szOldestRescordFilePath, szRecordPath);
				ret = S_FAIL;
				break;
			}

			if (!vrec_storage_is_full(szRecordPath, dwReversedSize))
				break;
		}

		while (n--) {
			free(namelist[n]);
		}
		free(namelist);
	}

	return ret;

}


static SCODE vrec_create_recordfile(
		TVRecInfo* ptVRecInfo,
		HANDLE hContainer,
		time_t curtime_for_record,
		const char *szRecordPath,
		char *szRecordFilePath,
		EVideoRecordFormat eRecForamt,
		DWORD dwVideoTrackNum, 
		DWORD dwAudioTrackNum,
		TSrcUBufInfo *ptPreVideoConfUBufInfo,
		TSrcUBufInfo *ptPreAudioConfUBufInfo,
		DWORD dwAudioDataType,
		DWORD dwVideoDataType)
{
	SCODE sRet=S_OK;
	TContainerFileInitData tContainerFileInitData;

	if(vrec_gen_filepath(curtime_for_record,szRecordPath,eRecForamt,szRecordFilePath,dwAudioDataType,dwVideoDataType)!=S_OK)
	{
		sRet=S_FAIL;
		printf("[vrec](%d)Call vrec_gen_filepath() is failed!\n",__LINE__);
		goto exit;
	}
	// printf("[vrec](%d)\n",__LINE__);
	memset(&tContainerFileInitData,0x0,sizeof(TContainerFileInitData));
	tContainerFileInitData.dwVideoTrackNum=dwVideoTrackNum;
	tContainerFileInitData.dwAudioTrackNum=dwAudioTrackNum;
	// printf("[vrec](%d)dwVideoTrackNum=%d dwAudioTrackNum=%d\n",__LINE__,dwVideoTrackNum,dwAudioTrackNum);
	tContainerFileInitData.dwMaxFileSize= vrec_get_maxfilesize(eRecForamt);
	tContainerFileInitData.bCheckMaxFileSize=(BOOLEAN)CHECK_MAX_FILESIZE;
	if(tContainerFileInitData.dwVideoTrackNum==0)
	{
		tContainerFileInitData.ptVideoTrackUBufInfo=NULL;

	}
	else
	{
		tContainerFileInitData.ptVideoTrackUBufInfo=ptPreVideoConfUBufInfo;
	}
	if(tContainerFileInitData.dwAudioTrackNum==0)
	{
		tContainerFileInitData.ptAudioTrackUBufInfo=NULL;
	}
	else
	{  
		tContainerFileInitData.ptAudioTrackUBufInfo=ptPreAudioConfUBufInfo;
	}
	tContainerFileInitData.szFilePath=szRecordFilePath;
	tContainerFileInitData.eFormat=eRecForamt;
	//	 printf("[vrec](%d) tContainerFileInitData.szFilePath=%s\n",__LINE__, tContainerFileInitData.szFilePath);			  
	if(vrec_create_file(ptVRecInfo, hContainer,tContainerFileInitData)!=S_OK)
	{
		sRet=S_FAIL;
		printf("[vrec](%d)Call vrec_create_file() is failed!\n",__LINE__);
		goto exit;
	} 
	//      printf("[vrec](%d)\n",__LINE__);
exit:
	return sRet;

}

static BOOLEAN vrec_has_vsample(HANDLE hContainer,EVideoRecordFormat eFormat)
{
	BOOLEAN bHas=FALSE;
	SCODE sRetFromfunc=S_OK;
	
	switch (eFormat)
	{
	case AVI_FORMAT:
		sRetFromfunc=AVIC_HasVideoSample(hContainer, &bHas);
		if(sRetFromfunc!=S_OK)
		{
			printf("[vrec](%d)AVIC_HasVideoSample() is failed!(%x)\n",__LINE__,sRetFromfunc);
		}
		break;
		
	case TREE_GP_FORMAT:
		//TODO:We will support 3gp reocrd file format in the future.		
		printf("[vrec](%d)We will support 3gp reocrd file format in the future!\n",__LINE__);
		break;
		
	case MP4_FORMAT:
		bHas = TRUE;
		break;
		
	default:
		printf("[vrec](%d)Unkown container format!\n",__LINE__);
		break;
	}
	
	return bHas;
}

static BOOLEAN vrec_has_asample(HANDLE hContainer,EVideoRecordFormat eFormat)
{
	BOOLEAN bHas=FALSE;
	SCODE sRetFromfunc=S_OK;

	switch (eFormat)
	{
	case AVI_FORMAT:
		sRetFromfunc = AVIC_HasAudioSample(hContainer, &bHas);
		if (sRetFromfunc != S_OK)
		{
			printf("[vrec](%d)AVIC_HasVideoSample() is failed!(%x)\n",__LINE__,sRetFromfunc);
		}
		break;
		
	case TREE_GP_FORMAT:
		//TODO:We will support 3gp reocrd file format in the future.
		printf("[vrec](%d)We will support 3gp reocrd file format in the future!\n",__LINE__);
		break;
		
	case MP4_FORMAT:
		bHas = TRUE;
		break;
		
	default:
		printf("[vrec](%d)Unkown container format!\n",__LINE__);
		break;
	}

	return bHas;
}

static SCODE vrec_close_file(TVRecInfo* ptVRecInfo, EVideoRecordFormat eRecForamt, char* szRecordFilePath, int duration_ms)
{
	SCODE sRet = S_OK;
	BOOLEAN bNoSample = FALSE;
	if((vrec_has_vsample(ptVRecInfo->hContainer, eRecForamt)==FALSE) && (vrec_has_asample(ptVRecInfo->hContainer, eRecForamt)==FALSE))
	{
		//There are no  audio sample and video sample to remove the reording file.
		bNoSample=TRUE;
	}

	if (eRecForamt == AVI_FORMAT)
	{
		if (duration_ms < 0)
			duration_ms = 0;
		if (AVIC_CloseFile(ptVRecInfo->hContainer, duration_ms) != S_OK)
		{
			printf("[vrec](%d)AVIC_CloseFile() is failed!\n",__LINE__);
			sRet = S_FAIL;
		}
	}
	else if (eRecForamt == MP4_FORMAT)
	{
		mp4_close(ptVRecInfo->hContainer);
	}
	else
		printf("[vrec](%d)Unkown container format!\n",__LINE__);

	if (bNoSample)
	{
		printf("[vrec](%d)Because there is no sample in %s. So unlink it\n",__LINE__,szRecordFilePath);
		if(unlink(szRecordFilePath)!=0)
			printf("[vrec](%d)Call unlink() to remove file %s is failed. (%s)\n",__LINE__,szRecordFilePath,strerror(errno));
	}

	return sRet;
}

static pthread_mutex_t allocate_mutex = PTHREAD_MUTEX_INITIALIZER;

void* allocate_memory(TVRecInfo *ptVRecInfo, int size)
{
	void* ptr;
	if ((CUR_MEM_CHUNK_USED_SIZE + size) >= MEM_CHUNK_MAX_SIZE)
	{
		++CUR_MEM_CHUNK_IDX;
		CUR_MEM_CHUNK_USED_SIZE = 0;
	}

	if (MEMORY_CHUNKS_ARRAY[CUR_MEM_CHUNK_IDX] == NULL)
	{
		pthread_mutex_lock(&allocate_mutex);
		if (CUR_MEM_FREE_CHUNK_USED_SIZE > 0)
		{
			MEMORY_CHUNKS_ARRAY[CUR_MEM_CHUNK_IDX] = MEMORY_FREE_CHUNKS_ARRAY[--CUR_MEM_FREE_CHUNK_USED_SIZE];
			MEMORY_FREE_CHUNKS_ARRAY[CUR_MEM_FREE_CHUNK_USED_SIZE] = NULL;
		}
		else
		{
#ifndef USE_EDMC_MEMORY
			MEMORY_CHUNKS_ARRAY[CUR_MEM_CHUNK_IDX] = (unsigned char*)malloc(MEM_CHUNK_MAX_SIZE);
#else
			TMemMgrState state;
			state.dwBusNum = 0;
			state.dwSize = MEM_CHUNK_MAX_SIZE;
			state.eAlignType = ALIGN_TYPE_8_BYTE;
			MEMORY_CHUNKS_ARRAY[CUR_MEM_CHUNK_IDX] = (unsigned char*)MemMgr_GetMemory(ptVRecInfo->MemMgrHandle, &state);
#endif
		}
		pthread_mutex_unlock(&allocate_mutex);
	}

	ptr = MEMORY_CHUNKS_ARRAY[CUR_MEM_CHUNK_IDX] + CUR_MEM_CHUNK_USED_SIZE;
	//make sure 8-byte alignment
	CUR_MEM_CHUNK_USED_SIZE = (CUR_MEM_CHUNK_USED_SIZE + size + 7) & (~0x7);

	return ptr;
}

void free_memory_chunks(void)
{
	int chunk_array_id = CUR_WRITTING_SEG_IDX;
	pthread_mutex_lock(&allocate_mutex);
	do
	{
		MEMORY_FREE_CHUNKS_ARRAY[CUR_MEM_FREE_CHUNK_USED_SIZE++] = MEMORY_CHUNKS_ARRAY[chunk_array_id];
		MEMORY_CHUNKS_ARRAY[chunk_array_id] = NULL;
	} while (MEMORY_CHUNKS_ARRAY[++chunk_array_id]);
	pthread_mutex_unlock(&allocate_mutex);
}

static void vrec_add_sample(TVRecInfo *ptVRecInfo, TSrcUBufInfo *ptSrcUBufInfo, BOOLEAN IsVideo)
{
	//static DWORD LastIFrameSec = 0;
	static DWORD total_data_size = 0;
	TSrcUBufInfo* ptDstUBufInfo;

	if (ptSrcUBufInfo->pbyUBuffHdr == NULL && ptSrcUBufInfo->pbyUBuffPayload == NULL)
		return;

	//trigger writer to record data every three I-Frame Intervals.
    //Note: the arrary size should have enough space;
	if (IsVideo && ((TUBuffer *)ptSrcUBufInfo->pbyUBuffHdr)->bIsSync)
	{
		//To avoid non-continuous data
		/*if (LastIFrameSec && (((TUBuffer *)ptSrcUBufInfo->pbyUBuffHdr)->dwSec - LastIFrameSec) > 3)
		{
			IFrameCount = 0;
			ptVRecInfo->ReceivedUsedSize = 0;
		}
		LastIFrameSec = ((TUBuffer *)ptSrcUBufInfo->pbyUBuffHdr)->dwSec;*/

		//if video codec is JPEG, each frame is a key frame.
		//Thus we need to check total frame numbers (ReceivedUsedSize) there.
		if (((total_data_size >= (1024 * 1024 * 2)) || (ptVRecInfo->ReceivedUsedSize >= (MAX_ARRAY_SIZE - 100)))&& !bInWrittingStage)
		{
			pthread_mutex_lock(&ptVRecInfo->VrecMutex);
			bInWrittingStage = TRUE;

			ptDstUBufInfo = ptVRecInfo->ReceivedArray;
			ptVRecInfo->ReceivedArray = ptVRecInfo->WrittingArray;
			ptVRecInfo->WrittingArray = ptDstUBufInfo;
			if (bReceivedReconfig)
			{
				bReconfigWriter = TRUE;
				bReceivedReconfig = FALSE;
			}
			ptVRecInfo->WrittingUsedSize = ptVRecInfo->ReceivedUsedSize;
			ptVRecInfo->ReceivedUsedSize = 0;
			total_data_size = 0;

			//switch chunk-segment.
			CUR_WRITTING_SEG_IDX = CUR_MEM_CHUNK_IDX;
			CUR_MEM_CHUNK_IDX = (CUR_MEM_CHUNK_IDX < MEM_CHUNK_SEG2_IDX)? MEM_CHUNK_SEG2_IDX:MEM_CHUNK_SEG1_IDX;
			CUR_MEM_CHUNK_USED_SIZE = 0;
			pthread_mutex_unlock(&ptVRecInfo->VrecMutex);
			pthread_cond_signal(&ptVRecInfo->VrecCond);
		}
	}

	ptDstUBufInfo = (ptVRecInfo->ReceivedArray + ptVRecInfo->ReceivedUsedSize);
	ptDstUBufInfo->iUBuffHdrLen = ptSrcUBufInfo->iUBuffHdrLen;
	ptDstUBufInfo->pbyUBuffHdr = (BYTE*) allocate_memory(ptVRecInfo, ptDstUBufInfo->iUBuffHdrLen);//malloc(MAX_UBUFFHDR_SIZE);
	memcpy(ptDstUBufInfo->pbyUBuffHdr, ptSrcUBufInfo->pbyUBuffHdr, ptSrcUBufInfo->iUBuffHdrLen);
	ptDstUBufInfo->iUBuffPayloadLen = ptSrcUBufInfo->iUBuffPayloadLen;
	if (ptSrcUBufInfo->iUBuffPayloadLen > 0)
	{
		total_data_size += ptDstUBufInfo->iUBuffPayloadLen;
		ptDstUBufInfo->pbyUBuffPayload = (BYTE*) allocate_memory(ptVRecInfo, ptDstUBufInfo->iUBuffPayloadLen);//malloc(MAX_UBUFFPAYLOAD_SIZE);
		memcpy(ptDstUBufInfo->pbyUBuffPayload, ptSrcUBufInfo->pbyUBuffPayload, ptSrcUBufInfo->iUBuffPayloadLen);

		if ((((TUBuffer *)ptDstUBufInfo->pbyUBuffHdr)->dwDataType == FOURCC_H264) &&
			(MP4_FORMAT != ptVRecInfo->eRecForamt))
		{
			unsigned char* pbyUBufPld = ptDstUBufInfo->pbyUBuffPayload;
			unsigned int len = ptDstUBufInfo->iUBuffPayloadLen;
			unsigned int dwByteStreamHeader = 0x01000000;
			unsigned int nal_size;
			do {
				memcpy(&nal_size, pbyUBufPld, 4);
				nal_size += 4;
				memcpy(pbyUBufPld, &dwByteStreamHeader, 4);
				pbyUBufPld += nal_size;
				len -= nal_size;
			} while (len > 4);
		}
	}

	if (++ptVRecInfo->ReceivedUsedSize >= MAX_ARRAY_SIZE)
	{
		//it means venc_writer() is busy, and we need to drop data.
		printf("!!! I/O is busy, drop data !!!\n");
		ptVRecInfo->ReceivedUsedSize = 0;

		//reset current chunk-segment
		CUR_MEM_CHUNK_IDX = (CUR_MEM_CHUNK_IDX < MEM_CHUNK_SEG2_IDX)? MEM_CHUNK_SEG1_IDX:MEM_CHUNK_SEG2_IDX;
		CUR_MEM_CHUNK_USED_SIZE = 0;
	}
}

static SCODE vrec_copy_ubufInfo(TSrcUBufInfo *ptDestUBufInfo,TSrcUBufInfo *ptSrcUBufInfo)
{
	SCODE sRet=S_OK;
	if(ptDestUBufInfo==NULL || ptSrcUBufInfo==NULL)
	{
		printf("[vrec](%d)can not copy UBufInfo because ptDestUBufInfo=%p ptSrcUBufInfo=%p \n",__LINE__,ptDestUBufInfo,ptSrcUBufInfo);
		sRet=S_FAIL;
		goto exit;
	}


	if(ptSrcUBufInfo->pbyUBuffHdr!=NULL)
	{
		if(ptDestUBufInfo->pbyUBuffHdr!=NULL)
		{
			free(ptDestUBufInfo->pbyUBuffHdr);

		}
		ptDestUBufInfo->pbyUBuffHdr=(BYTE *)malloc(ptSrcUBufInfo->iUBuffHdrLen);
		memcpy(ptDestUBufInfo->pbyUBuffHdr,ptSrcUBufInfo->pbyUBuffHdr,ptSrcUBufInfo->iUBuffHdrLen);
		ptDestUBufInfo->iUBuffHdrLen=ptSrcUBufInfo->iUBuffHdrLen;
	}

	if(ptSrcUBufInfo->pbyUBuffPayload!=NULL)
	{
		if(ptDestUBufInfo->pbyUBuffPayload!=NULL)
		{
			free(ptDestUBufInfo->pbyUBuffPayload);

		}

		ptDestUBufInfo->pbyUBuffPayload=(BYTE *)malloc(ptSrcUBufInfo->iUBuffPayloadLen);

		memcpy(ptDestUBufInfo->pbyUBuffPayload,ptSrcUBufInfo->pbyUBuffPayload,ptSrcUBufInfo->iUBuffPayloadLen);
		ptDestUBufInfo->iUBuffPayloadLen=ptSrcUBufInfo->iUBuffPayloadLen;
	}



exit:
	return sRet;

}

void* vrec_checker(void* hVRec)
{
	TVRecInfo *ptVRecInfo =(TVRecInfo *)hVRec;
	pthread_mutex_lock(&ptVRecInfo->CheckerMutex);
	for(;;)
	{
		pthread_cond_wait(&ptVRecInfo->CheckerCond, &ptVRecInfo->CheckerMutex);
		if (!bWriterRunning)
			break;

		vrec_check_oldest_recordfile(ptVRecInfo->szRecordPath, ptVRecInfo->dwSizeLimit);
	}
	pthread_mutex_unlock(&ptVRecInfo->CheckerMutex);

	return NULL;
}

/*


   Calling sequence:

   vrec_init_container_handle(HANDLE *phContainer,EVideoRecordFormat eFormat)
   vrec_gen_filepath(time_t curtime,const char *szRecordPath, EVideoRecordFormat eFormat,char *szRecordFilePath)
   vrec_get_maxfilesize(EVideoRecordFormat eFormat)
   memset(&tContainerFileInitData,0x0,sizeof(TContainerFileInitData));
   vrec_create_file(HANDLE hContainer,TContainerFileInitData tInitData)
   vrec_add_sample(HANDLE hContainer, TSrcUBufInfo *ptSrcUBufInfo,EVideoRecordFormat eFormat)
   vrec_has_vsample(HANDLE hContainer,EVideoRecordFormat eFormat)
   vrec_has_asample(HANDLE hContainer,EVideoRecordFormat eFormat)
   vrec_close_file(HANDLE hContainer,EVideoRecordFormat eFormat)
   vrec_rls_container_handle(HANDLE *phContainer,EVideoRecordFormat eFormat)

 */
//Video_YYYYMMDD_HHMMSS_[video_foramt]_[audio_foramt]_[length].avi
//Video_YYYYMMDD_HHMMSS_H264_G721_1.avi
SCODE vrec_receiver(TVRecInfo *ptVRecInfo)
{
	BOOLEAN bEnableRec = FALSE;
	BOOLEAN bBeginToRevBitstream = FALSE;
	TSrcUBufInfo tSrcUBufInfo;

	fd_set fdsRead;
	fd_set fdsRec;
	int iFd=-1,iVFd=-1,iAFd=-1;
	int iRet=-1;
	int fake_fd = 0;
	char fake_file[256];

	FD_ZERO(&fdsRead);
    FD_ZERO(&fdsRec);

	if (ptVRecInfo->tVideoBitstreamSrc.bEnable == TRUE)
	{
		if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
			iVFd = ShrdBufferSrc_GetFd(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc);
		else if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSOCKET)
			iVFd = SockSrc_GetFd(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc) ;
		else
		{
			printf("(%d)The getting mechanism of video bistream is not supported!!\n",__LINE__);
			return S_FAIL;
		}

		FD_SET(iVFd,&fdsRead);
        iFd = iVFd;
	}

	if (ptVRecInfo->tAudioBitstreamSrc.bEnable == TRUE)
	{
		if(ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSHRDBUFFER)
			iAFd = ShrdBufferSrc_GetFd(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc);
		else if(ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSOCKET)
			iAFd = SockSrc_GetFd(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc) ;      
		else
		{
			printf("(%d)The getting mechanism of audio bistream is not supported!!\n",__LINE__);
			return S_FAIL;
		}

		FD_SET(iAFd,&fdsRead);
        if(iAFd > iVFd)
            iFd = iAFd;
	}

	printf("(%d)iFd=%d iVFd=%d iAFd=%d\n",__LINE__,iFd,iVFd,iAFd);

	memset(&tSrcUBufInfo,0x0,sizeof(TSrcUBufInfo));

	bEnableRec = ptVRecInfo->bEnableRec;
	for (;;)
	{
		if (g_iCommand == rscRecoderShutDown)
			break;

		if(g_iCommand == rscReloadConfigFile)
		{
			//status change
			g_iCommand = rscNoCommand;

			if (LoadConfig(ptVRecInfo) == S_FAIL)
				return S_FAIL;

			if (ptVRecInfo->bReConf_flag == TRUE)
			{
				sem_wait(&(ptVRecInfo->smOptions));
				bEnableRec = ptVRecInfo->bEnableRec;
				//web cgi uses such info in /proc/pid/fd to detect recording status
				if (bEnableRec)
				{
					sprintf(fake_file, "%s/fake.avi", ptVRecInfo->szRecordPath);	
					fake_fd = open(fake_file, O_WRONLY | O_CREAT);
				}
				else if (fake_fd)
				{
					unlink(fake_file);
					close(fake_fd);
					fake_fd = 0;
				}
				ptVRecInfo->bReConf_flag = FALSE;
				sem_post(&(ptVRecInfo->smOptions));
			}

			if (bEnableRec)
			{
				if (bIsDaemon)
					syslog(LOG_INFO,"[vrec](%d)Enable reocrding... \n",__LINE__);

				if (ptVRecInfo->tVideoBitstreamSrc.bEnable == TRUE )
				{
					//trigger venc_encoder to encode the video bitstream through the command fifo.
					if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
					{ 

						ShrdBufferSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letBitstrmStart);
						ShrdBufferSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letNeedConf);
						usleep(200000);
						ShrdBufferSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letNeedConf);
						ShrdBufferSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letNeedIntra);
					}
					else if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSOCKET)
					{
						//trigger venc_encoder to get video bitstream from socket.
						SockSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letNeedConf);
						SockSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letBitstrmStart);
					}
					else
					{
						printf("(%d)The getting mechanism of video bistream is not supported!!\n",__LINE__);
						return S_FAIL;
					} 
				}
				if (ptVRecInfo->tAudioBitstreamSrc.bEnable == TRUE )
				{
					//trigger audio_encoder to encode the audio bitstream through the command fifo.
					if(ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
					{ 

						ShrdBufferSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letBitstrmStart);
						ShrdBufferSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letNeedConf);
						usleep(200000);
						ShrdBufferSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letNeedConf);
					}
					else if(ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism == MSOCKET)
					{
						//trigger audio_encoder to encode audio bitstream from socket.
						SockSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letNeedConf);
						SockSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letBitstrmStart);

					}
					else
					{
						printf("(%d)The getting mechanism of audio bistream is not supported!!\n",__LINE__);
						return S_FAIL;
					} 
				}
				bBeginToRevBitstream = TRUE;

				//notify writer
				bReceivedReconfig = TRUE;
			}
			else
			{
				if (bIsDaemon)
					syslog(LOG_INFO,"[vrec](%d)Disable reocrding... \n",__LINE__);

				if (ptVRecInfo->tVideoBitstreamSrc.bEnable && bBeginToRevBitstream)
				{
					if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
						ShrdBufferSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letBitstrmStop);
					else if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSOCKET)
						SockSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letBitstrmStop);
					else
					{
						printf("(%d)The getting mechanism of video bistream is not supported!!\n",__LINE__);
						return S_FAIL;
					} 
				}

				if (ptVRecInfo->tAudioBitstreamSrc.bEnable && bBeginToRevBitstream)
				{
					if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
						ShrdBufferSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letBitstrmStop);
					else if(ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSOCKET)
						SockSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letBitstrmStop);
					else
					{
						printf("(%d)The getting mechanism of audio bistream is not supported!!\n",__LINE__);
						return S_FAIL;
					}
				}
				bBeginToRevBitstream = FALSE;

				//force to close recording file:
				pthread_mutex_lock(&ptVRecInfo->VrecMutex);
				bReconfigWriter = TRUE;
				pthread_mutex_unlock(&ptVRecInfo->VrecMutex);
				pthread_cond_signal(&ptVRecInfo->VrecCond);
			}
		}

		fdsRec = fdsRead;
		errno = 0;
		iRet = select(iFd+1, &fdsRec, NULL, NULL, NULL);

		if(iRet <= 0)
			continue;
		
		if (ptVRecInfo->tAudioBitstreamSrc.bEnable && FD_ISSET(iAFd, &fdsRec))
		{
			if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism == MSOCKET)
			{
				if(SockSrc_GetUBuffer(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc, &tSrcUBufInfo) == S_OK)
				{
					if (bEnableRec)
						vrec_add_sample(ptVRecInfo, &tSrcUBufInfo, FALSE);
					SockSrc_ReleaseUBuffer(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc, &tSrcUBufInfo);
				}
			}
			else if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
			{
				if (ShrdBufferSrc_GetUBuffer(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc, &tSrcUBufInfo) == S_OK)
				{
					if (bEnableRec)
						vrec_add_sample(ptVRecInfo, &tSrcUBufInfo, FALSE);
					ShrdBufferSrc_ReleaseUBuffer(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc, &tSrcUBufInfo);
				}
			}
		}

		if (ptVRecInfo->tVideoBitstreamSrc.bEnable && FD_ISSET(iVFd, &fdsRec))
		{
			if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
			{
				if (ShrdBufferSrc_GetUBuffer(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc, &tSrcUBufInfo) == S_OK)
				{
					if (bEnableRec)
						vrec_add_sample(ptVRecInfo, &tSrcUBufInfo, TRUE);
					ShrdBufferSrc_ReleaseUBuffer(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc, &tSrcUBufInfo);
				}
			}
			else if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSOCKET)
			{
				if (SockSrc_GetUBuffer(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc, &tSrcUBufInfo) == S_OK )
				{
					if (bEnableRec)
						vrec_add_sample(ptVRecInfo, &tSrcUBufInfo, TRUE);
					SockSrc_ReleaseUBuffer(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc, &tSrcUBufInfo);
				}
			}
		}
	}

	if (fake_fd)
	{
		unlink(fake_file);
		close(fake_fd);
	}

	if (ptVRecInfo->tVideoBitstreamSrc.bEnable && bBeginToRevBitstream)
	{
		if (ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
			ShrdBufferSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letBitstrmStop);
		else if(ptVRecInfo->tVideoBitstreamSrc.eBistreamMechanism == MSOCKET)
			SockSrc_EventHandler(ptVRecInfo->tVideoBitstreamSrc.hBistreamSrc,letBitstrmStop);
		else
		{
			printf("(%d)The getting mechanism of video bistream is not supported!!\n",__LINE__);
			return S_FAIL;
		} 
	}

	if (ptVRecInfo->tAudioBitstreamSrc.bEnable && bBeginToRevBitstream)
	{
		if (ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism == MSHRDBUFFER)
			ShrdBufferSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letBitstrmStop);
		else if(ptVRecInfo->tAudioBitstreamSrc.eBistreamMechanism==MSOCKET)
			SockSrc_EventHandler(ptVRecInfo->tAudioBitstreamSrc.hBistreamSrc,letBitstrmStop);
		else
		{
			printf("(%d)The getting mechanism of audio bistream is not supported!!\n",__LINE__);
			return S_FAIL;
		}
	}

	return S_OK;
}

void* vrec_writer(void* hVRec)
{
	int idx;
	SCODE sRes = S_OK;
	TVRecInfo *ptVRecInfo =(TVRecInfo *)hVRec;

	BOOLEAN bEnableRec = FALSE;
	char *szRecordPath = NULL;
	EVideoRecordFormat eRecForamt = UNKOWN_FORMAT;
	BOOLEAN bEnableCyclicStorage = FALSE;

	DWORD dwRecLen=0;
	DWORD dwRecSize=0;
	DWORD dwReversedSize=ptVRecInfo->dwSizeLimit;

	DWORD dwAudioDataType = 0;
	DWORD dwVideoDataType = 0;
	DWORD dwCompressionLaw = 0;

	DWORD dwSampleRate=-1; 
	DWORD dwCodewordBits=-1;
	DWORD dwProfileLevel=-1;

	DWORD dwVideoWidth=-1;
	DWORD dwVideoHeight=-1;

	TSrcUBufInfo tPreVideoConfUBufInfo;
	TSrcUBufInfo tPreAudioConfUBufInfo;
	TSrcUBufInfo* ptLocalUBufInfo = NULL;

	DWORD dwVideoTrackNum = 0;
	DWORD dwAudioTrackNum = 0;

	BOOLEAN bOpenFile = FALSE;
	BOOLEAN bWriteVideoClipsErr = FALSE; 

	time_t curtime_for_record = 0;
	time_t nexttime_for_record = 0;

	unsigned int clip_start_sec = 0;
	unsigned int clip_end_sec = 0;

	//Note: don't use unsinged int because of caculation.
	int clip_start_usec = 0;
	int clip_end_usec = 0;

	char szRecordFilePath[RECORD_FILEPATH_LEN] = { 0 };

	if(vrec_init_container_handle(&ptVRecInfo->hContainer, ptVRecInfo->eRecForamt) != S_OK)
		printf("[vrec](%d)call vrec_init_container_handle() failed! %x\n", __LINE__, sRes);

	memset(&tPreVideoConfUBufInfo,0x0,sizeof(TSrcUBufInfo));
	memset(&tPreAudioConfUBufInfo,0x0,sizeof(TSrcUBufInfo));

	pthread_mutex_lock(&ptVRecInfo->VrecMutex);
	for (;;)
	{
		if (bWriteVideoClipsErr)
		{
			printf("[vrec](%d)There is an error when writing %s. So Disable reocrding...\n",__LINE__,szRecordFilePath);
			if (bIsDaemon)
				syslog(LOG_INFO,"[vrec](%d)There is an error when writing %s. So Disable reocrding...\n",__LINE__,szRecordFilePath);
			bWriteVideoClipsErr = FALSE;

			dwVideoTrackNum=0;
			dwAudioTrackNum=0;

			dwAudioDataType = 0;
			dwVideoDataType = 0;
			dwCompressionLaw = 0;
			dwVideoWidth=0;
			dwVideoHeight=0;
			memset(&tPreVideoConfUBufInfo,0x0,sizeof(TSrcUBufInfo));
			memset(&tPreAudioConfUBufInfo,0x0,sizeof(TSrcUBufInfo));
		}

		/////////////////////////////////////////////////////////////
		bInWrittingStage = FALSE;
		pthread_cond_wait(&ptVRecInfo->VrecCond, &ptVRecInfo->VrecMutex);
		if (!bWriterRunning)
			break;

		if (bReconfigWriter)
		{
			if (bOpenFile)
			{
				int duration_ms = ((clip_end_sec - clip_start_sec) * 1000) + ((clip_end_usec - clip_start_usec) / 1000);
				vrec_close_file(ptVRecInfo, eRecForamt, szRecordFilePath, duration_ms);
				bOpenFile = FALSE;
			}

			bEnableRec = ptVRecInfo->bEnableRec;
			bEnableCyclicStorage = ptVRecInfo->bEnableCyclicStorage;
			szRecordPath = strdup(ptVRecInfo->szRecordPath);
			eRecForamt = ptVRecInfo->eRecForamt;
			dwRecLen = ptVRecInfo->dwRecLen;
			dwRecSize = ptVRecInfo->dwRecSize;

			bReconfigWriter = FALSE;
		}

		if (!bEnableRec)
			continue;
		
		if (AVI_FORMAT == ptVRecInfo->eRecForamt)
			AVIC_FlushCache(ptVRecInfo->hContainer);
		else if (MP4_FORMAT == ptVRecInfo->eRecForamt)
			mp4_flush_sample(ptVRecInfo->hContainer);
		
		for (idx = 0; idx < ptVRecInfo->WrittingUsedSize; ++idx)
		{
			ptLocalUBufInfo = (ptVRecInfo->WrittingArray + idx);
			if ((ptLocalUBufInfo->iUBuffPayloadLen > 0))
			{
				if (!bOpenFile && ((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwDataType == dwVideoDataType)
				{
					if ((((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->bIsSync == FALSE) ||
						(ptVRecInfo->tVideoBitstreamSrc.bEnable && (dwVideoDataType == 0)) ||
						(ptVRecInfo->tAudioBitstreamSrc.bEnable && (dwAudioDataType == 0)))
						continue;

					curtime_for_record = clip_start_sec = ((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwSec;
					clip_start_usec = ((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwUSec;

					if(vrec_create_recordfile(
								ptVRecInfo,
								ptVRecInfo->hContainer,
								curtime_for_record,
								szRecordPath,
								szRecordFilePath,
								eRecForamt,
								dwVideoTrackNum, 
								dwAudioTrackNum,
								&tPreVideoConfUBufInfo,
								&tPreAudioConfUBufInfo,
								dwAudioDataType,
								dwVideoDataType)!= S_OK)
					{
						bWriteVideoClipsErr = TRUE;
						break;
					}

					bOpenFile = TRUE;
				}

				if (bOpenFile)
				{
					if (AVI_FORMAT == ptVRecInfo->eRecForamt)
					{
						if (AVIC_AddSample(ptVRecInfo->hContainer, ptLocalUBufInfo) != S_OK)
						{
							int duration_ms = ((((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwSec - clip_start_sec) * 1000) + ((((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwUSec - clip_start_usec) / 1000);
							vrec_close_file(ptVRecInfo, eRecForamt, szRecordFilePath, duration_ms);
							bWriteVideoClipsErr = TRUE;
							bOpenFile = FALSE;
							syslog(LOG_INFO,"[vrec]write file error: %s. So Disable reocrding...\n", szRecordFilePath);
							break;
						}
					}
					else if (MP4_FORMAT == ptVRecInfo->eRecForamt)
					{
						DWORD dwMp4TrackID = 0;
						
						if (((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwTrackID == ((TUBuffer *)tPreVideoConfUBufInfo.pbyUBuffHdr)->dwTrackID)
							dwMp4TrackID = ptVRecInfo->dwMp4VideoTrackID;
						else if (((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwTrackID == ((TUBuffer *)tPreAudioConfUBufInfo.pbyUBuffHdr)->dwTrackID)
						{
							dwMp4TrackID = ptVRecInfo->dwMp4AudioTrackID;
/*
printf("[vrec](%d) audio trackid %d, chnum %d, sample rate %d\n",__LINE__, 
	((TUBufferConfAAC4 *)(tPreAudioConfUBufInfo.pbyUBuffHdr))->dwTrackID,
	((TUBufferConfAAC4 *)(tPreAudioConfUBufInfo.pbyUBuffHdr))->dwChannelNumber, 
	((TUBufferConfAAC4 *)(tPreAudioConfUBufInfo.pbyUBuffHdr))->dwSampleRate);
*/
						}
						
//printf("[vrec](%d) mp4add track %d\n",__LINE__, dwMp4TrackID);
						if ((0 != dwMp4TrackID) && 
						    (S_OK != mp4_add_sample(ptVRecInfo->hContainer, dwMp4TrackID, (TMP4SrcUBufInfo *)ptLocalUBufInfo)))
						{
							vrec_close_file(ptVRecInfo, eRecForamt, szRecordFilePath, 0);
							bWriteVideoClipsErr = TRUE;
							bOpenFile = FALSE;
							break;
						}
					}
				}
			}
			else
			{
				BOOLEAN refresh = FALSE;
				DWORD codectype = 0;

				if (vrec_check_codec(ptLocalUBufInfo->pbyUBuffHdr, &codectype,TRUE) == TRUE)
				{
					printf("[vrec](%d)(from conf)The codec of  bitstream has been changed.  dwVideoDataType=%d\n",__LINE__, codectype);
					vrec_show_codec(codectype);
				}

				if (codectype == FOURCC_H264 ||
					codectype == FOURCC_JPEG ||
					codectype == FOURCC_MP4V)
				{
					
					//1. Check that the codec of video bitstream has been changed or not.
					if (vrec_copy_ubufInfo(&tPreVideoConfUBufInfo, ptLocalUBufInfo) == S_FAIL)
					{
						sRes = S_FAIL;
						break;
					}

					if (dwVideoDataType != codectype)
					{
						dwVideoDataType = codectype;
						refresh = TRUE;
					}

					if(vrec_check_resolution(ptLocalUBufInfo->pbyUBuffHdr, &dwVideoWidth, &dwVideoHeight)==TRUE)
					{
						printf("[vrec](%d)(from conf)The resolution of video bitstream has been changed. dwVideoWidth=%d  dwVideoHeight=%d  \n",__LINE__,dwVideoWidth,dwVideoHeight);
						refresh = TRUE;
					}

					if(refresh)
						dwVideoTrackNum = 1;
				}
				else if (codectype > 0)//assume it is an audio packet
				{
					if (vrec_copy_ubufInfo(&tPreAudioConfUBufInfo, ptLocalUBufInfo) == S_FAIL)
					{
						sRes = S_FAIL;
						break;
					}

					if (vrec_check_compression(ptLocalUBufInfo->pbyUBuffHdr, &dwCompressionLaw) == TRUE)
					{
						printf("[vrec](%d)(from conf)The compression law of audio bitstream has been changed. dwCompressionLaw=%d \n",__LINE__,dwCompressionLaw);
						vrec_show_compressionLaw(dwCompressionLaw);
						refresh = TRUE;
					}

					if (vrec_check_samplerate(ptLocalUBufInfo->pbyUBuffHdr, &dwSampleRate) == TRUE)
					{
						printf("[vrec](%d)(from conf)The sample rate of audio bitstream has been changed. dwSampleRate=%d \n",__LINE__,dwSampleRate);
						refresh = TRUE;
					}

					if (vrec_check_codewordBits(ptLocalUBufInfo->pbyUBuffHdr, &dwCodewordBits) == TRUE)
					{
						printf("[vrec](%d)(from conf)The code word bits of audio bitstream has been changed. dwCodewordBits=%d \n",__LINE__,dwCodewordBits);
						refresh = TRUE;
					}

					if (vrec_check_profileLevel(ptLocalUBufInfo->pbyUBuffHdr,&dwProfileLevel) == TRUE)
					{
						printf("[vrec](%d)(from conf)The profile level of audio bitstream has been changed. dwProfileLevel=%d \n",__LINE__,dwProfileLevel);
						refresh = TRUE;
					}

					if (dwAudioDataType != codectype)
					{
						dwAudioDataType = codectype;
						refresh = TRUE;
					}

					if (refresh)
						dwAudioTrackNum = 1;
				}

				if (refresh && bOpenFile)
				{
					int duration_ms = ((((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwSec - clip_start_sec) * 1000) + ((((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwUSec - clip_start_usec) / 1000);
					vrec_close_file(ptVRecInfo, eRecForamt, szRecordFilePath, duration_ms);
					bOpenFile = FALSE;
				}
			}
		}

		free_memory_chunks();

		if (sRes != S_OK)
			break;
		//////////////////////////////////////////////////////////////////////////////
		nexttime_for_record = ((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwSec;
		clip_end_sec = nexttime_for_record = ((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwSec;
		clip_end_usec = ((TUBuffer *)ptLocalUBufInfo->pbyUBuffHdr)->dwUSec;
		if (!bOpenFile || difftime (nexttime_for_record,  curtime_for_record) < (dwRecLen - 1))
		{
			if (AVI_FORMAT == ptVRecInfo->eRecForamt) 
				AVIC_CommitData(ptVRecInfo->hContainer);
			else if (MP4_FORMAT == ptVRecInfo->eRecForamt) 
				mp4_commit_sample(ptVRecInfo->hContainer);
			continue;
		}

		curtime_for_record = nexttime_for_record;
		{
			int duration_ms = ((clip_end_sec - clip_start_sec) * 1000) + ((clip_end_usec - clip_start_usec) / 1000);
			vrec_close_file(ptVRecInfo, eRecForamt, szRecordFilePath, duration_ms);
		}
		bOpenFile = FALSE;

		if (bEnableCyclicStorage==TRUE && vrec_storage_is_full(szRecordPath, dwReversedSize))
			pthread_cond_signal(&ptVRecInfo->CheckerCond);
	}
	pthread_mutex_unlock(&ptVRecInfo->VrecMutex);

	///exit ////
	if (bOpenFile == TRUE)
	{
		int duration_ms = ((clip_end_sec - clip_start_sec) * 1000) + ((clip_end_usec - clip_start_usec) / 1000);
		vrec_close_file(ptVRecInfo, eRecForamt, szRecordFilePath, duration_ms);
	}

	if (szRecordPath != NULL)
		free(szRecordPath);

	if (tPreVideoConfUBufInfo.pbyUBuffHdr != NULL)
		free(tPreVideoConfUBufInfo.pbyUBuffHdr);

	if (tPreAudioConfUBufInfo.pbyUBuffHdr != NULL)
		free(tPreAudioConfUBufInfo.pbyUBuffHdr);

	if (tPreVideoConfUBufInfo.pbyUBuffPayload != NULL)
		free(tPreVideoConfUBufInfo.pbyUBuffPayload);

	if (tPreAudioConfUBufInfo.pbyUBuffPayload != NULL)
		free(tPreAudioConfUBufInfo.pbyUBuffPayload);

	if(ptVRecInfo->hContainer != NULL)
		vrec_rls_container_handle(&ptVRecInfo->hContainer, eRecForamt);

	return NULL;
}

int main(int argc, char *argv[])
{
	char* szConfigFile = NULL;
	int	iUseDefaultConfigFile = 0;
	int	i;
	FILE *fp = NULL;
	TVRecInfo* ptVRecInfo = NULL;
	pthread_t writer_thread;
	pthread_t checker_thread;

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
				goto exit_program_2;
		}
	}
	if(szConfigFile == NULL &&  g_szPidFile == NULL)
	{

		print_usage();
		exit(1);

	}
	if (bIsDaemon)
	{
		syslog(LOG_INFO, "Start vrec porcess with Pid : %d\n", getpid());
	}
	/* run as Daemon*/
	if (bIsDaemon){
		daemonize();
		//   daemon(0, 0);
	}
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
		szConfigFile = DEFAULT_CONFIG_FILE;
	}

	if((fp=fopen(szConfigFile,"r"))==NULL)
	{
		fprintf(stderr, "Can not find the config file %s !!\n",szConfigFile);
		goto exit_program_2;
	}
	if (fp != NULL)
	{
		fclose(fp);
		fp=NULL;
	}
	vrec_signal(SIGINT, vrec_sighandler);
	vrec_signal(SIGHUP, vrec_sighandler);
	signal(SIGPIPE, SIG_IGN);
	/*1. Initialize some hanles of video recording*/
	if (vrec_initial(&ptVRecInfo, szConfigFile)==S_FAIL)
	{
		printf("(%d)Can not run vrec_initial() successfully !!\n",__LINE__);
		goto exit_program_1;

	}

	pthread_create(&writer_thread, NULL, &vrec_writer, ptVRecInfo);
	pthread_create(&checker_thread, NULL, &vrec_checker, ptVRecInfo);
	vrec_receiver(ptVRecInfo);

	bWriterRunning = FALSE;
	pthread_mutex_lock(&ptVRecInfo->VrecMutex);
	pthread_cond_signal(&ptVRecInfo->VrecCond);
	pthread_mutex_unlock(&ptVRecInfo->VrecMutex);

	pthread_mutex_lock(&ptVRecInfo->CheckerMutex);
	pthread_cond_signal(&ptVRecInfo->CheckerCond);
	pthread_mutex_unlock(&ptVRecInfo->CheckerMutex);

	pthread_join(writer_thread, NULL);
	pthread_join(checker_thread, NULL);

exit_program_1:
	/*Release some hanles of video recording */
	vrec_release(ptVRecInfo);
exit_program_2:
	if ((szConfigFile != NULL) && (!iUseDefaultConfigFile))
	{
		free(szConfigFile);
		szConfigFile=NULL;
	}
	if (g_szPidFile != NULL)
	{
		delPIDfile(g_szPidFile);
	}
	if (fp != NULL)
	{
		fclose(fp);
		fp=NULL;
	}
	if (bIsDaemon)
	{
		syslog(LOG_INFO, "Stop vrec process !!\n");
	}
	if( g_szPidFile != NULL)
	{
		free( g_szPidFile );
		g_szPidFile =NULL;
	}
	//return 0;


	return 0;
}
