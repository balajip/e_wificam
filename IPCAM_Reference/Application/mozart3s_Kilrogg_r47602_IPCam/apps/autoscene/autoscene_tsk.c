#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <syslog.h>
#include <semaphore.h>
#include <fcntl.h>		/* for nonblocking */
#include <errno.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/stat.h> 
#include <sys/time.h>
#if defined(_AUTOSCENE_)  
#include "AutoScene.h"
#endif

#include "Resize.h"
#include "VideoCap.h"
#include "MemMgr.h"
#include "xmlwrapper.h"
#include "msgreader.h"

#define MAX_BUF_SIZE  1024
#define RESIZED_WIDTH 48
#define RESIZED_HEIGHT 32
#define START 	"start"
#define STOP 	"stop"
#define INIT 	"initial"
#define INIT_AND_START "initial_and_start"
#define MAX_VIDEOCAP_RESTART_COUNT 100
#define VENC_CONF_PATH "/etc/conf.d/c0/venc_conf.xml"
#define DIGITAL_SIZE 10
#define AUTOSCENE "autoscene"

BOOLEAN g_bIsDaemon = FALSE;

volatile sig_atomic_t got_interrupt = 0;

void sig_alarm(int signo) 
{
	got_interrupt = 1;
}

typedef enum ctrl_command_type
{
	rscShutDown,
	rscReloadConfigFile,
	rscNoCommand
} ECtrlCmd;

static ECtrlCmd  g_iCommand=rscNoCommand;

typedef enum scene_mode
{
	OUTDOOR = 0,
	INDOOR =1,
	WDR =2,
	UNKOWNMODE=3,
} ESceneMode;

typedef struct msg
{
	CHAR szHost[64];
	CHAR szContent[64];
} TMsg;

typedef struct task_info
{
	DWORD	dwResizedWidth;
	DWORD 	dwResizedHeight;
	DWORD 	dwResizedStride;
	DWORD 	dwCapWidth;
	DWORD 	dwCapHeight;
	DWORD 	dwInWidth;
	DWORD 	dwInHeight;
	DWORD 	dwMaxFrameWidth;
	DWORD 	dwMaxFrameHeight;
	DWORD 	dwDevNum;
	BYTE 	*pbyResizedOutputBuf;
	HANDLE 	hMemObject;
	HANDLE 	hResizeObj;
	HANDLE 	hVideoCapObject;
	HANDLE 	hMsgReaderObj;
	HANDLE 	hAutoSceneObject;
	ESceneMode 	eSceneMode;
	sem_t  	smOptions;
	CHAR 	*szConfigFile;
	DWORD 	dwTargetOffset;
	DWORD 	dwTargetLuma;
	CHAR 	*szCmdFifo;
	DWORD 	dwTriggerTime;
	CHAR 	*szRefCfgFile; 
	TMsg 	*ptMsg;
	DWORD 	*pdwUsrCELut;
	BYTE 	*pbyRGBGammaLut;
	BOOL 	bInitial;
	BYTE 	*pbyStatAEWBBuf;
	BYTE 	*pbyStatHistoBuf;
	BYTE 	*pbyStatFocusBuf;
	DWORD 	dwLSCTblSize;
	WORD 	*pwLSCTbl;
	//The followings are parameters for autoscene setoptions. 
	DWORD 	dwPlineFreq;
	BOOL 	bPlineFreqChanged;
	DWORD 	dwExposureLevel;
	BOOL 	bExposureLevelChanged;
	DWORD 	dwExposureMinShutter;
	BOOL	bExposureMinShutterChanged;
	DWORD 	dwExposureMaxShutter;
	BOOL	bExposureMaxShutterChanged;
	DWORD 	dwExposureMinGain;
	BOOL	bExposureMinGainChanged;
	DWORD 	dwExposureMaxGain;
	BOOL	bExposureMaxGainChanged;
	DWORD 	dwBrightLevel;
	BOOL	bBrightLevelChanged;
	DWORD 	dwContrastLevel;
	BOOL	bContrastLevelChanged;
	DWORD 	dwSaturation;
	BOOL 	bSaturationChanged;
	DWORD	dwMono;
	BOOL	bMonoChanged;
	DWORD	dwNRMode;
	BOOL	bNRModeChanged;
#if 0
	DWORD	dwNRLevel;
	BOOL	bNRLevelChanged;
#endif	
	DWORD	dwSharpnessLevel;
	BOOL	bSharpnessLevelChanged;
	DWORD 	dwAEMode;
	BOOL 	bAEModeChanged;
	DWORD	dwIrisMode;
	BOOL	bIrisModeChanged;
	DWORD 	dwAutoIrisActiveTime;
	BOOL	bAutoIrisActiveTimeChanged;
	DWORD	dwIrcutMode;
	BOOL	bIrcutModeChanged;
	DWORD	dwGammaLevel;
	BOOL	bGammaLevelChanged;
	DWORD	dwCELevel;
	BOOL	bCELevelChanged;
	DWORD	dw2DNRLevel;
	BOOL	b2DNRLevelChanged;
	DWORD	dw3DNRLevel;
	BOOL	b3DNRLevelChanged;
	DWORD 	dwAELock;
	BOOL	bAELockChanged;
	DWORD	dwAWBLock;
	BOOL	bAWBLockChanged;
	DWORD	dwAWBMode;
	DWORD 	dwAWBGainR;
	DWORD 	dwAWBGainB;
	BOOL	bAWBModeChanged;
	DWORD	dwWDRRatio;
	BOOL	bWDRRatioChanged;
} TTaskInfo;

#define SHARED_MEM_PATH_NAME "autoscene_shm"
#define SHARED_MEM_LEN 8192 /*The SHARED_MEM_LEN in the venc_videoin_process.c must be modified if this is modified.*/
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
static SCODE create_shm(void)
{
	int	fd, flags;
	char	*ptr;
	off_t	length;

	flags = O_RDWR | O_CREAT;
	length = SHARED_MEM_LEN ;
	
	fd = shm_open(SHARED_MEM_PATH_NAME, flags, FILE_MODE);
	if (fd == -1)
	{
		printf("[autoscene](%d)%s\n", __LINE__, strerror(errno));
		return S_FAIL;
	}
	ftruncate(fd, length);

	ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	munmap(ptr, length);

	close(fd);
	return S_OK;
}

static SCODE unlink_shm(void)
{
	if (shm_unlink(SHARED_MEM_PATH_NAME) == -1) 
	{
		printf("[autoscene](%d)%s\n", __LINE__, strerror(errno));
		return S_FAIL;
	}
	return S_OK;
}

static SCODE write_shm(const char *szContent)
{
	int		i, fd;
	struct stat	stat;
	char	*ptr;

	fd = shm_open(SHARED_MEM_PATH_NAME, O_RDWR, FILE_MODE);

	if (fd == -1)
	{
		printf("[autoscene](%d)%s\n",__LINE__,strerror(errno));
		return S_FAIL;
	}
	fstat(fd, &stat);
	ptr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, 0);
	close(fd);

	for (i = 0; i < stat.st_size; i++)
		ptr[i] = szContent[i];

	munmap(ptr, stat.st_size);

	return S_OK;
}

static SCODE read_shm(char *szResult, int iResultLen)
{
	int		i, fd;
	struct stat	stat;
	char	*ptr;

	fd = shm_open(SHARED_MEM_PATH_NAME, O_RDONLY, FILE_MODE);
	if (fd == -1)
	{
		printf("[autoscene](%d)%s\n",__LINE__,strerror(errno));
		return S_FAIL;
	}
	fstat(fd, &stat);
	ptr = mmap(NULL, stat.st_size, PROT_READ,
			MAP_SHARED, fd, 0);
	close(fd);
	if(iResultLen < stat.st_size)
	{
		printf("[autoscene](%d)Can not read the shm from %s. Because the len of reading buf  %d is smaller than shm size %d\n", __LINE__, SHARED_MEM_PATH_NAME, iResultLen, (int)stat.st_size);  
		return S_FAIL;
	}
	memset(szResult, 0x0, iResultLen);
	for (i = 0; i < stat.st_size; i++)
		szResult[i] = ptr[i] ;

	munmap(ptr, stat.st_size);

	return S_OK;
}

void autoscene_sighandler(int signo)
{
	if (signo == SIGINT) //SIGINT	2
	{	
		printf("Got the SIGINT(%d)\n", SIGINT);
		g_iCommand = rscShutDown;
	}
	else if (signo == SIGHUP) //SIGHUP==1
	{		
		printf("Got the SIGHUP(%d)\n", SIGHUP);
		g_iCommand = rscReloadConfigFile;
	}
}

void autoscene_signal(int signum, void (*handler)(int))
{
	struct sigaction sigaInst;

	sigaInst.sa_flags = 0;
	sigfillset(&sigaInst.sa_mask);
	sigaInst.sa_handler = handler;

	sigaction(signum, &sigaInst, NULL);
}

static void SetResizedStride(void *userData, const char *s, int len)
{  
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if(ptTaskInfo != NULL)
	{
		ptTaskInfo->dwResizedStride=dwVal;
	}
	return;   
}

static void SetResizedwidth(void *userData, const char *s, int len)
{  
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwResizedWidth = dwVal;
	}
	return;   
}

static void SetResizedheight(void *userData, const char *s, int len)
{  
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwResizedHeight=dwVal;
	}
	return;   
}

static void SetMode(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	char str[len+1];

	memset(str,0x0,sizeof(str));
	memcpy(str, s, len);
	str[len] = '\0';
	if (ptTaskInfo != NULL)
	{
		if (strncmp (str,"outdoor", strlen(str)) == 0)
		{   
			ptTaskInfo->eSceneMode = OUTDOOR;
		}
		else if (strncmp (str,"indoor", strlen(str)) == 0)
		{
			ptTaskInfo->eSceneMode = INDOOR;
		}
		else if (strncmp (str,"wdr", strlen(str)) == 0)
		{
			ptTaskInfo->eSceneMode = WDR;
		}
		else
		{
			ptTaskInfo->eSceneMode = UNKOWNMODE;
		}
	}
	return;
}

static void SetTargetLuma(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);
	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwTargetLuma = dwVal;
	}
	return;
}

static void SetTargetOffset(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwTargetOffset = dwVal;
	}
	return;
}

static void SetCmdFifo(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->szCmdFifo = strdup(szTemp);
	}
	return;
}

static void SetTriggerTime(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwTriggerTime = dwVal;
	}
	return;
}

static void SetPlineFreq(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwPlineFreq = dwVal;
		ptTaskInfo->bPlineFreqChanged = TRUE;
	}
	return;
}

static void SetExposureLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwExposureLevel = dwVal;
		ptTaskInfo->bExposureLevelChanged = TRUE;
	}
	return;
}

static void SetExposureMinShutter(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwExposureMinShutter = dwVal;
		ptTaskInfo->bExposureMinShutterChanged = TRUE;
	}
	return;
}

static void SetExposureMaxShutter(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwExposureMaxShutter = dwVal;
		ptTaskInfo->bExposureMaxShutterChanged = TRUE;
	}
	return;
}

static void SetExposureMinGain(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwExposureMinGain = dwVal;
		ptTaskInfo->bExposureMinGainChanged = TRUE;
	}
	return;
}

static void SetExposureMaxGain(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwExposureMaxGain = dwVal;
		ptTaskInfo->bExposureMaxGainChanged = TRUE;
	}
	return;	
}

static void SetBrightLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwBrightLevel = dwVal;
		ptTaskInfo->bBrightLevelChanged = TRUE;
	}
	return;
}

static void SetContrastLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwContrastLevel = dwVal;
		ptTaskInfo->bContrastLevelChanged = TRUE;
	}
	return;	
}

static void SetSaturation(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwSaturation = dwVal;
		ptTaskInfo->bSaturationChanged = TRUE;
	}
	return;
}

static void SetMono(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwMono = dwVal;
		ptTaskInfo->bMonoChanged = TRUE;
	}
	return;
}

static void SetNRMode(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwNRMode = dwVal;
		ptTaskInfo->bNRModeChanged = TRUE;
	}
	return;
}
#if 0
static void SetNRLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwNRLevel = dwVal;
		ptTaskInfo->bNRLevelChanged = TRUE;
	}
	return;
}
#endif

static void SetSharpnessLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwSharpnessLevel = dwVal;
		ptTaskInfo->bSharpnessLevelChanged = TRUE;
	}
	return;
}



static void SetAEMode(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAEMode = dwVal;
		ptTaskInfo->bAEModeChanged = TRUE;
	}
	return;
}

static void SetIrisMode(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwIrisMode = dwVal;
		ptTaskInfo->bIrisModeChanged = TRUE;
	}
	return;
}

static void SetAutoIrisActiveTime(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAutoIrisActiveTime = dwVal;
		ptTaskInfo->bAutoIrisActiveTimeChanged = TRUE;
	}
	return;
}

static void SetIrcutMode(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwIrcutMode = dwVal;
		ptTaskInfo->bIrcutModeChanged = TRUE;
	}
	return;
}

static void SetGammaLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwGammaLevel = dwVal;
		ptTaskInfo->bGammaLevelChanged = TRUE;
	}
	return;
}

static void SetCELevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwCELevel = dwVal;
		ptTaskInfo->bCELevelChanged = TRUE;
	}
	return;
}

static void Set2DNRLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dw2DNRLevel = dwVal;
		ptTaskInfo->b2DNRLevelChanged = TRUE;
	}
	return;
}

static void Set3DNRLevel(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dw3DNRLevel = dwVal;
		ptTaskInfo->b3DNRLevelChanged = TRUE;
	}
	return;
}

static void SetAELock(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAELock = dwVal;
		ptTaskInfo->bAELockChanged = TRUE;
	}
	return;
}


static void SetAWBLock(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAWBLock = dwVal;
		ptTaskInfo->bAWBLockChanged = TRUE;
	}
	return;
}

static void SetAWBMode(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAWBMode = dwVal;
		ptTaskInfo->bAWBModeChanged = TRUE;
	}
	return;
}

static void SetAWBGainR(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAWBGainR = dwVal;
	}
	return;
}

static void SetAWBGainB(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwAWBGainB = dwVal;
	}
	return;
}


static void SetWDRRatio(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo = (TTaskInfo *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->dwWDRRatio = dwVal;
		ptTaskInfo->bWDRRatioChanged = TRUE;
	}
	return;
}
static void SetReferenceConfig(void *userData, const char *s, int len)
{
	TTaskInfo *ptTaskInfo= (TTaskInfo *)userData;
	CHAR szTemp[len+1];

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);

	if (ptTaskInfo != NULL)
	{
		ptTaskInfo->szRefCfgFile = strdup(szTemp);
	}
	return;
}

SCODE LoadConfig(TTaskInfo *ptTaskInfo)
{
	int sRet;
	HANDLE hXmlWrapObj;
	TXmlWrapperTreeMap ptParsingMap[] = 
	{
		{"root/resizedstride", NULL, &SetResizedStride, NULL},
		{"root/resizedwidth", NULL, &SetResizedwidth, NULL},
		{"root/resizedheight", NULL, &SetResizedheight, NULL},
		{"root/mode",NULL, &SetMode, NULL},
		{"root/targetLuma", NULL, &SetTargetLuma, NULL},
		{"root/targetOffset", NULL, &SetTargetOffset, NULL},
		{"root/cmdfifo", NULL, &SetCmdFifo, NULL},
		{"root/triggertime", NULL, &SetTriggerTime, NULL},
		{"root/pline_freq", NULL, &SetPlineFreq, NULL},
		{"root/exposure_level", NULL, &SetExposureLevel, NULL},
		{"root/exposure_min_shutter", NULL, &SetExposureMinShutter, NULL},
		{"root/exposure_max_shutter", NULL, &SetExposureMaxShutter, NULL},
		{"root/exposure_min_gain", NULL, &SetExposureMinGain, NULL},
		{"root/exposure_max_gain", NULL, &SetExposureMaxGain, NULL},
		{"root/bright_level", NULL, &SetBrightLevel, NULL},
		{"root/contrast_level", NULL, &SetContrastLevel, NULL},
		{"root/saturation", NULL, &SetSaturation, NULL},
		{"root/mono", NULL, &SetMono, NULL},
		{"root/nr_mode", NULL, &SetNRMode, NULL},
		//{"root/nr_level", NULL, &SetNRLevel, NULL},
		{"root/exposure_mode", NULL, &SetAEMode, NULL},
		{"root/exposure_lock", NULL, &SetAELock, NULL},
		{"root/iris_mode", NULL, &SetIrisMode, NULL},
		{"root/iris_active_time", NULL, &SetAutoIrisActiveTime, NULL},
		{"root/ircut_mode", NULL, &SetIrcutMode, NULL},
		{"root/gamma_curve", NULL, &SetGammaLevel, NULL},
		{"root/contrast_enhance_level", NULL, &SetCELevel, NULL},
		{"root/two_d_nr_level", NULL, &Set2DNRLevel, NULL},
		{"root/three_d_nr_level", NULL, &Set3DNRLevel, NULL},
		{"root/sharpness_level", NULL, &SetSharpnessLevel, NULL},	
		{"root/awb_mode", NULL, &SetAWBMode, NULL},
		{"root/awb_gainR", NULL, &SetAWBGainR, NULL},
		{"root/awb_gainB", NULL, &SetAWBGainB, NULL},
		{"root/awb_lock", NULL, &SetAWBLock, NULL},
		{"root/wdr_ratio", NULL, &SetWDRRatio, NULL},
		{"root/referenceConfig", NULL, &SetReferenceConfig, NULL},
		{NULL, NULL, NULL}
	};

	TXmlWrapperInitOptions tInitOptions;
	memset(&tInitOptions, 0x0, sizeof(TXmlWrapperInitOptions));

	if (ptTaskInfo == NULL)
	{
		printf("%s %d : ptTaskInfo is NULL! \n", __FILE__, __LINE__);
		return S_FAIL;
	}

	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap, NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet = XmlWrapper_ReadFile_UsrDefFunc(ptTaskInfo->szConfigFile, hXmlWrapObj,ptTaskInfo)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
	printf("[autoscene][%d](resizedstride,resizedwidth,resizedheight,scenemode,targetLuma,targetOffset,triggertime,cmdfifo,referenceConfig)=(%d,%d,%d,%d,%d,%d,%d,%s,%s)\n",__LINE__,ptTaskInfo->dwResizedStride,ptTaskInfo->dwResizedWidth,ptTaskInfo->dwResizedHeight,ptTaskInfo->eSceneMode,ptTaskInfo->dwTargetLuma,ptTaskInfo->dwTargetOffset,ptTaskInfo->dwTriggerTime,ptTaskInfo->szCmdFifo,ptTaskInfo->szRefCfgFile);
	
	return S_OK;
}

static SDWORD GetParam(FILE *pfInput)
{
	CHAR acTmpText[200];
	SDWORD sdwRet;

	fscanf(pfInput, "%s", acTmpText);
	sdwRet = atoi(acTmpText);
	fgets(acTmpText, 200, pfInput);

	return sdwRet;
}

static SCODE create_tmpfile(char *szTmpfileName)
{
	int fd;

	if (szTmpfileName == NULL)
	{
		printf("(%d)szTmpfileName is null!\n",__LINE__);
		return S_FAIL;
	}
	/* maybe get the tmpdir env variable instead of using /tmp */
	strcpy(szTmpfileName, "/tmp/autoscene-XXXXXX");
	umask(0077); // ensure exclusive access on buggy implementations of mkstemp
	fd = mkstemp(szTmpfileName);
	close(fd);

	return S_OK;
}

void GetHost(HANDLE hObject, const CHAR *szVisitor, SDWORD len)
{
	CHAR szTemp[len+1];
	TMsg *ptMsg = (TMsg *)hObject;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", szVisitor);

	memset(ptMsg->szHost, 0x0, sizeof(ptMsg->szHost));
	strncpy(ptMsg->szHost, szTemp, strlen(szTemp));
	return;
}

void GetContent(HANDLE hObject, const CHAR *szVisitor, SDWORD len)
{
	CHAR szTemp[len+1];
	TMsg *ptMsg = (TMsg *)hObject;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", szVisitor);

	memset(ptMsg->szContent, 0x0, sizeof(ptMsg->szContent));
	strncpy(ptMsg->szContent, szTemp, strlen(szTemp));
	return;
}

static SCODE InitialMsgReader(HANDLE *phMsgReaderObj,TMsg *ptMsg ,CHAR* szMsgFifo)
{
	TMsgReaderInitialOpts tMsgReaderInitOpts;
	TXmlWrapperTreeMap MsgHdlTreeMap[] = 
	{
		{"control/request/host", NULL, &GetHost, NULL},
		{"control/request/content", NULL, &GetContent, NULL},
		{NULL, NULL, NULL}
	};

	if (ptMsg == NULL)
	{
		fprintf(stderr, "(%d)ptMsg is NULL!!\n", __LINE__);
		return S_FAIL;
	}
	tMsgReaderInitOpts.dwVersion = MSGREADER_VERSION;
	tMsgReaderInitOpts.pcMsgPath = szMsgFifo;
	tMsgReaderInitOpts.ptCallBackHandler = MsgHdlTreeMap;
	tMsgReaderInitOpts.ptv_time = NULL;
	tMsgReaderInitOpts.hUsrObj = (HANDLE)ptMsg;

	if (MsgReader_Initial(phMsgReaderObj, &tMsgReaderInitOpts) != S_OK)
	{
		fprintf(stderr, "(%d)InitialMsgReader Fail!!\n", __LINE__);
		return S_FAIL;
	}
	return S_OK;
}

SCODE init_task_info(TTaskInfo *ptTaskInfo)
{
	DWORD dwOuptFormat = -1;
	DWORD dwInputFormat = -1;
	
	if (ptTaskInfo == NULL)
	{
		printf("[autoscene][%d]ptTaskInfo==NULL!!\n", __LINE__);
		return S_FAIL;
	}

	if (sem_init(&(ptTaskInfo->smOptions), 0, 1) != 0)
	{
		printf("[autoscene][%d]initial ptTaskInfo->smOptions failed!!\n", __LINE__);
		return S_FAIL;
	}

	{
		ptTaskInfo->dwResizedWidth = 0;
		ptTaskInfo->dwResizedHeight = 0;
		ptTaskInfo->dwResizedStride = 0;
		ptTaskInfo->eSceneMode = UNKOWNMODE;
		ptTaskInfo->dwTargetOffset = -1;
		ptTaskInfo->dwTargetLuma = -1;

		if (LoadConfig(ptTaskInfo) != S_OK)
		{
			printf("[autoscene][%d] call LoadConfig() failed!!\n", __LINE__);
			return S_FAIL;
		}

		if ((ptTaskInfo->dwResizedWidth == 0) || 
				(ptTaskInfo->dwResizedHeight == 0) ||
				(ptTaskInfo->dwResizedStride == 0) || 
				(ptTaskInfo->eSceneMode == UNKOWNMODE))
		{
			printf("[autoscene][%d]initial Resized Size and mode failed!!\n", __LINE__);
			return S_FAIL;
		}
	}

	//Initial maxwidth,maxheight,capwidth,capheight,inwidth,inheight
	{
		char  szCommand[1024];
		char  szTmpFileName[1024];
		FILE *fp = NULL;
		char  szSensorConfPath[1024];
		FILE *fpSensorConf = NULL;
		DWORD dwIndex = 0;
		
		
		memset(szCommand, 0x0, sizeof(szCommand));
		memset(szTmpFileName, 0x0, sizeof(szTmpFileName));
		memset(szSensorConfPath, 0x0, sizeof(szSensorConfPath));

		create_tmpfile(szTmpFileName);
		snprintf(szCommand, sizeof(szCommand), "cat %s  |  grep sensor_config_path | awk -F'<sensor_config_path>' '{ print $2}' | awk -F'</sensor_config_path>' '{ print $1}' > %s", VENC_CONF_PATH, szTmpFileName);
		system(szCommand);

		fp = fopen(szTmpFileName, "r");
		if (fp != NULL)
		{
			fscanf(fp, "%s", szSensorConfPath);
			fclose(fp);
			fp = NULL;
			unlink(szTmpFileName);
		}
		else
		{
			printf("[autoscene][%d]cannot open file %s !!\n", __LINE__, szTmpFileName);
			return S_FAIL;
		}

		fpSensorConf = fopen(szSensorConfPath, "r");
		if (fpSensorConf == NULL)
		{
			printf("[autoscene][%d]cannot open file %s !!\n", __LINE__, szSensorConfPath);
			return S_FAIL;
		}

		ptTaskInfo->dwDevNum = GetParam(fpSensorConf);

		// ignore 3 fields
		for (dwIndex = 0; dwIndex < 3; dwIndex++) 
		{
			GetParam(fpSensorConf);
		}

		ptTaskInfo->dwMaxFrameWidth = GetParam(fpSensorConf);
		ptTaskInfo->dwMaxFrameHeight = GetParam(fpSensorConf);
		ptTaskInfo->dwCapWidth = GetParam(fpSensorConf);
		ptTaskInfo->dwCapHeight = GetParam(fpSensorConf);

		// ignore 2 fields
		for (dwIndex = 0; dwIndex < 2; dwIndex++)
		{
			GetParam(fpSensorConf);
		}
		dwOuptFormat = GetParam(fpSensorConf);
		dwInputFormat = GetParam(fpSensorConf);
		
		
		ptTaskInfo->dwInWidth = GetParam(fpSensorConf);
		ptTaskInfo->dwInHeight = GetParam(fpSensorConf);
		
		printf("(maxwidth,maxheight,capwidth,capheight,inwidth,inheight,dwOuptFormat,dwInputFormat)=(%d,%d,%d,%d,%d,%d,%d,%d)\n", ptTaskInfo->dwMaxFrameWidth, ptTaskInfo->dwMaxFrameHeight, ptTaskInfo->dwCapWidth, ptTaskInfo->dwCapHeight, ptTaskInfo->dwInWidth, ptTaskInfo->dwInHeight,dwOuptFormat,dwInputFormat);

		if (fpSensorConf != NULL)
		{
			fclose(fpSensorConf);
			fpSensorConf = NULL;
		}
	}

	//Initial VdieoCap Handle
	{
		TVideoCapInitOptions tVideoCapInitOptions;
		DWORD dwVideoSize = 0;
		memset(&tVideoCapInitOptions, 0, sizeof(TVideoCapInitOptions));
		tVideoCapInitOptions.dwVersion             = VIDEOCAP_VERSION;
		tVideoCapInitOptions.dwMaxFrameWidth       = ptTaskInfo->dwMaxFrameWidth;
		tVideoCapInitOptions.dwMaxFrameHeight      = ptTaskInfo->dwMaxFrameHeight;
		tVideoCapInitOptions.dwCapWidth            = ptTaskInfo->dwCapWidth;
		tVideoCapInitOptions.dwCapHeight           = ptTaskInfo->dwCapHeight;
		tVideoCapInitOptions.dwInWidth             = ptTaskInfo->dwInWidth;
		tVideoCapInitOptions.dwInHeight            =  ptTaskInfo->dwInHeight;
		tVideoCapInitOptions.dwFrameRate           = 0;
		tVideoCapInitOptions.dwDeviceNum           = ptTaskInfo->dwDevNum;
		tVideoCapInitOptions.pObjectMem            = NULL;
		tVideoCapInitOptions.eOutPixelFormat = (EPixelFormatFlags)dwOuptFormat;
		tVideoCapInitOptions.eFormat = (EPixelFormatFlags)dwInputFormat;
		ptTaskInfo->pbyStatAEWBBuf = NULL;
		ptTaskInfo->pbyStatHistoBuf = NULL;
		ptTaskInfo->pbyStatFocusBuf = NULL;

		if (VideoCap_Initial(&ptTaskInfo->hVideoCapObject, &tVideoCapInitOptions) != S_OK) 
		{
			printf("[autoscene][%d]Initial VideoCapObject failed!!\n",__LINE__);
			return S_FAIL;
		}

		ptTaskInfo->pbyStatAEWBBuf = tVideoCapInitOptions.pbyStatAEWBBufUsrBaseAddr;
		ptTaskInfo->pbyStatHistoBuf = tVideoCapInitOptions.pbyStatHistoBufUsrBaseAddr;
		ptTaskInfo->pbyStatFocusBuf = tVideoCapInitOptions.pbyStatFocusBufUsrBaseAddr;
		printf("[autoscene](pbyStatAEWBBuf,pbyStatHistoBuf,pbyStatFocusBuf)=(%p,%p,%p)\n",ptTaskInfo->pbyStatAEWBBuf,ptTaskInfo->pbyStatHistoBuf,ptTaskInfo->pbyStatFocusBuf);
		dwVideoSize = tVideoCapInitOptions.dwInWidth*tVideoCapInitOptions.dwInHeight;
		if (tVideoCapInitOptions.dwInWidth <= 512)
		{
			ptTaskInfo->dwLSCTblSize = dwVideoSize;
		}
		else if (tVideoCapInitOptions.dwInWidth <= 1024)
		{
			ptTaskInfo->dwLSCTblSize = dwVideoSize >> 1;
		}
		else if (tVideoCapInitOptions.dwInWidth <= 2048)
		{
			ptTaskInfo->dwLSCTblSize = dwVideoSize >> 2;
		}
		else
		{
			ptTaskInfo->dwLSCTblSize = dwVideoSize >> 3;
		}

	}

	//Initial Resize Handle
	{
		TResizeInitOptions tResizeInitOptions;
		memset(&(tResizeInitOptions), 0, sizeof(TResizeInitOptions));
		tResizeInitOptions.dwVersion = RESIZE_VERSION;
		tResizeInitOptions.pObjectMem = NULL;
		if (Resize_Initial(&ptTaskInfo->hResizeObj, &tResizeInitOptions) != S_OK) 
		{
			printf("[autoscene][%d]Initial ResieObject failed!!\n", __LINE__);
			return S_FAIL;
		}
	}

	//Initial MemMgr Handle
	{
		TMemMgrInitOptions tMemMgrInitOptions;
		TMemMgrState tMemMgrState; 

		memset(&tMemMgrInitOptions, 0x0, sizeof(TMemMgrInitOptions));
		tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
		tMemMgrInitOptions.pObjectMem = (void *)NULL;
		if (MemMgr_Initial(&ptTaskInfo->hMemObject, &tMemMgrInitOptions) != S_OK) 
		{
			printf("[autoscene][%d]Initial memory manager object fail !!\n", __LINE__);
			return S_FAIL;
		}

#ifdef _1DRAM_VERSION
		tMemMgrState.dwBusNum = 0;
#else
		tMemMgrState.dwBusNum = 1;
#endif
		tMemMgrState.dwSize = (ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight * 3) / 2;
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
		if ((ptTaskInfo->pbyResizedOutputBuf = (BYTE *)MemMgr_GetMemory(ptTaskInfo->hMemObject, &tMemMgrState)) == NULL) 
		{
			printf("[autoscene][%d] Allocate Resize output buffer %d fail !!\n",  __LINE__, tMemMgrState.dwSize);
			return S_FAIL;
		}

		memset(&tMemMgrState, 0x0, sizeof(TMemMgrState));
#ifdef _1DRAM_VERSION
		tMemMgrState.dwBusNum = 0;
#else
		tMemMgrState.dwBusNum = 1;
#endif

		tMemMgrState.dwSize = 4096*3;
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;

		if ((ptTaskInfo->pbyRGBGammaLut = (BYTE*)MemMgr_GetMemory(ptTaskInfo->hMemObject, &tMemMgrState)) == NULL)
		{
			printf("[autoscene][%d] Allocate RGB gamma table (%d bytes) buffer fail !!\n", __LINE__, tMemMgrState.dwSize);
			return S_FAIL;
		}

		memset(&tMemMgrState, 0x0, sizeof(TMemMgrState));
#ifdef _1DRAM_VERSION
		tMemMgrState.dwBusNum = 0;
#else
		tMemMgrState.dwBusNum = 1;
#endif
		tMemMgrState.dwSize = 256 * sizeof(DWORD);
		tMemMgrState.eAlignType = ALIGN_TYPE_256_BYTE;

		if ((ptTaskInfo->pdwUsrCELut = (DWORD*)MemMgr_GetMemory(ptTaskInfo->hMemObject, &tMemMgrState)) == NULL)
		{
			printf("Allocate CE table buffer (%d bytes) fail !!\n", tMemMgrState.dwSize);
			return S_FAIL;
		}
		
		tMemMgrState.dwBusNum = 0;
		tMemMgrState.dwSize = ptTaskInfo->dwLSCTblSize*sizeof(WORD);
		tMemMgrState.eAlignType = ALIGN_TYPE_1024_BYTE;
		if ((ptTaskInfo->pwLSCTbl=(WORD *)MemMgr_GetMemory(ptTaskInfo->hMemObject, &tMemMgrState)) == NULL) {
			printf("Allocate LDC buffer %d fail !!\n", (int)tMemMgrState.dwSize);
			return S_FAIL;
		} else {
			printf("Request %d bytes non-cached memory succeed !!\n", (int)tMemMgrState.dwSize);
		}    
		memset(ptTaskInfo->pwLSCTbl, 0, ptTaskInfo->dwLSCTblSize*sizeof(WORD));
		
	}

	//Initial MsgReader Hanlde
	{
		ptTaskInfo->ptMsg = malloc(sizeof(TMsg));
		if (ptTaskInfo->ptMsg == NULL)
		{
			return S_FAIL;
		}
		if (ptTaskInfo->szCmdFifo == NULL)
		{
			return S_FAIL;
		}
		// Tony 2012.06.07
		if (ptTaskInfo->szRefCfgFile == NULL)
		{
			return S_FAIL;
		}	
		if (InitialMsgReader(&ptTaskInfo->hMsgReaderObj, ptTaskInfo->ptMsg ,ptTaskInfo->szCmdFifo) != S_OK)
		{
			return S_FAIL;
		}
	}

#if defined(_AUTOSCENE_)
	{
		TAutoSceneInitOptions tAutoSceneInitOptions;
		memset(&tAutoSceneInitOptions, 0x0, sizeof(TAutoSceneInitOptions));
		tAutoSceneInitOptions.dwVersion = AUTOSCENE_VERSION;
		tAutoSceneInitOptions.pObjectMem = NULL;
		tAutoSceneInitOptions.pRefCfgFile = ptTaskInfo->szRefCfgFile;
#ifdef _PRE_NR_		
		tAutoSceneInitOptions.bEnPreYUVISP = TRUE;	// define PreNR & PreEE enable
#else
		tAutoSceneInitOptions.bEnPreYUVISP = FALSE;	// define PreNR & PreEE enable
#endif
		if (AutoScene_Initial(&ptTaskInfo->hAutoSceneObject, &tAutoSceneInitOptions) != S_OK) 
		{
			printf("[autoscene][%d] AutoScene initial err  !!\n", __LINE__);
			return S_FAIL;
		}
	}
#endif

	return S_OK;
}

SCODE rls_task_info(TTaskInfo *ptTaskInfo)
{
	if (ptTaskInfo == NULL)
	{
		return S_FAIL;
	}

	if (ptTaskInfo->hVideoCapObject != NULL)
	{
		if (VideoCap_Release(&ptTaskInfo->hVideoCapObject) != S_OK) 
		{
			fprintf(stderr, "[autoscene][%d] Release VideoCap object fail !!\n", __LINE__);
		}
	}
	if (ptTaskInfo->pbyResizedOutputBuf != NULL)
	{
		MemMgr_FreeMemory(ptTaskInfo->hMemObject, (DWORD)ptTaskInfo->pbyResizedOutputBuf);
	}
	if (ptTaskInfo->pbyRGBGammaLut != NULL)
	{
		MemMgr_FreeMemory(ptTaskInfo->hMemObject, (DWORD)ptTaskInfo->pbyRGBGammaLut);
	}
	if (ptTaskInfo->pdwUsrCELut != NULL)
	{
		MemMgr_FreeMemory(ptTaskInfo->hMemObject, (DWORD)ptTaskInfo->pdwUsrCELut);
	}
	if (ptTaskInfo->pwLSCTbl != NULL)
	{
		MemMgr_FreeMemory(ptTaskInfo->hMemObject, (DWORD)ptTaskInfo->pwLSCTbl);
	}
	
	if (ptTaskInfo->hMemObject != NULL)
	{
		MemMgr_Release(&ptTaskInfo->hMemObject);
	}

	if (ptTaskInfo->hResizeObj != NULL)
	{
		if (Resize_Release(&ptTaskInfo->hResizeObj) != S_OK)
		{
			fprintf(stderr, "[autoscene][%d] Release Resize handle fail !!\n", __LINE__);
		}
	}
	if (ptTaskInfo->hAutoSceneObject != NULL)
	{
		if (AutoScene_Release(ptTaskInfo->hAutoSceneObject) != S_OK)
		{
			fprintf(stderr, "[autoscene][%d] Release AutoScene handle fail !!\n", __LINE__);
		}
	}
	if (ptTaskInfo->szCmdFifo != NULL)
	{
		free(ptTaskInfo->szCmdFifo);
		ptTaskInfo->szCmdFifo = NULL;
	}
	
	if (ptTaskInfo->szRefCfgFile != NULL)
	{
		free(ptTaskInfo->szRefCfgFile);
		ptTaskInfo->szRefCfgFile = NULL;
	}
	if (ptTaskInfo->ptMsg != NULL)
	{
		free(ptTaskInfo->ptMsg);
		ptTaskInfo->ptMsg = NULL;
	}
	if (ptTaskInfo->hMsgReaderObj != NULL)
	{
		MsgReader_Release(&ptTaskInfo->hMsgReaderObj);
		ptTaskInfo->hMsgReaderObj = NULL;
	}

	sem_destroy(&(ptTaskInfo->smOptions));
	memset(ptTaskInfo, 0x0, sizeof(TTaskInfo));
	return S_OK;
}

static BOOL isISPEqaul(TAutoSceneState *p1, TAutoSceneState *p2)
{
	BOOL bRet = FALSE;
	bRet = (p1->tYUVNRParam.bEnYUVNR == p2->tYUVNRParam.bEnYUVNR) && \
		   (p1->tYUVNRParam.dwNRMode == p2->tYUVNRParam.dwNRMode) && \
		   (p1->tYUVNRParam.dw2DNRStrength == p2->tYUVNRParam.dw2DNRStrength) && \
		   (p1->tYUVNRParam.dw3DNRRefStrength == p2->tYUVNRParam.dw3DNRRefStrength) && \
		   (p1->tYUVNRParam.dw3DNRCurrWeight == p2->tYUVNRParam.dw3DNRCurrWeight) && \
		   (p1->tYUVNRParam.bNRMotionAdaptiveEn == p2->tYUVNRParam.bNRMotionAdaptiveEn) && \
		   (p1->tYUVPreNRParam.bEnYUVNR == p2->tYUVPreNRParam.bEnYUVNR) && \
		   (p1->tYUVPreNRParam.dwNRMode == p2->tYUVPreNRParam.dwNRMode) && \
		   (p1->tYUVPreNRParam.dw2DNRStrength == p2->tYUVPreNRParam.dw2DNRStrength) && \
		   (p1->tYUVPreNRParam.dw3DNRRefStrength == p2->tYUVPreNRParam.dw3DNRRefStrength) && \
		   (p1->tYUVPreNRParam.dw3DNRCurrWeight == p2->tYUVPreNRParam.dw3DNRCurrWeight) && \
		   (p1->tYUVPreNRParam.bNRMotionAdaptiveEn == p2->tYUVPreNRParam.bNRMotionAdaptiveEn) && \
		   (p1->tEdgeEnhanceParam.bEnEdgeEnhance == p2->tEdgeEnhanceParam.bEnEdgeEnhance) && \
		   (p1->tEdgeEnhanceParam.dwEdgeSensitivity == p2->tEdgeEnhanceParam.dwEdgeSensitivity) && \
		   (p1->tEdgeEnhanceParam.dwNoiseSensitivity == p2->tEdgeEnhanceParam.dwNoiseSensitivity) && \
		   (p1->tEdgeEnhanceParam.dwEEStrength == p2->tEdgeEnhanceParam.dwEEStrength) && \
		   (p1->tEdgeEnhanceParam.dwEEClip == p2->tEdgeEnhanceParam.dwEEClip) && \
		   (p1->tPreEdgeEnhanceParam.bEnEdgeEnhance == p2->tPreEdgeEnhanceParam.bEnEdgeEnhance) && \
		   (p1->tPreEdgeEnhanceParam.dwEdgeSensitivity == p2->tPreEdgeEnhanceParam.dwEdgeSensitivity) && \
		   (p1->tPreEdgeEnhanceParam.dwNoiseSensitivity == p2->tPreEdgeEnhanceParam.dwNoiseSensitivity) && \
		   (p1->tPreEdgeEnhanceParam.dwEEStrength == p2->tPreEdgeEnhanceParam.dwEEStrength) && \
		   (p1->tPreEdgeEnhanceParam.dwEEClip == p2->tPreEdgeEnhanceParam.dwEEClip) && \
		   (p1->bEnIRCut == p2->bEnIRCut);
	return bRet;
}
SCODE  do_autoscene(TTaskInfo *ptTaskInfo)
{
	TVideoCapState tVideoCapState;
	SCODE scRet = S_FAIL;
	DWORD dwAEShutter = 0;
	DWORD dwAEGain = 0;
	DWORD dwCurrColorTemp = 0;
	BOOL bAEStable = FALSE;
	int restart_count = MAX_VIDEOCAP_RESTART_COUNT;
	
	static TAutoSceneState tPreAutoSceneState;
	static BOOL bFirstRun = TRUE;

	if (ptTaskInfo == NULL)
	{
		return S_FAIL;
	}

	if (bFirstRun == TRUE)
	{
				bFirstRun = FALSE;
				memset(&tPreAutoSceneState, 0x0, sizeof(TAutoSceneState));
	}
restart:	
	VideoCap_Sleep(ptTaskInfo->hVideoCapObject);
	memset(&tVideoCapState, 0x0, sizeof(TVideoCapState));
	scRet = VideoCap_GetBuf(ptTaskInfo->hVideoCapObject, &tVideoCapState);

	if (scRet != S_OK)
	{
		if (tVideoCapState.bNoSignal == TRUE)
		{
			printf("[autoscene][%d] No signal !!\n", __LINE__);
		}
		if (tVideoCapState.bFifoFull == TRUE) 
		{
			printf("[autoscene][%d] fifo full !!\n", __LINE__);
		}
		if (tVideoCapState.bCCIRErr == TRUE)
		{
			printf("[autoscene][%d] bCCIRErr !!\n", __LINE__);
		}
		VideoCap_Sleep(ptTaskInfo->hVideoCapObject);
		memset(&tVideoCapState, 0x0, sizeof(TVideoCapState));
		scRet = VideoCap_GetBuf(ptTaskInfo->hVideoCapObject, &tVideoCapState);
	} 

	if ((scRet != S_OK) && restart_count)
	{
		restart_count--;
		goto restart;
	} 

	if (scRet == S_OK) 
	{
#ifdef _AUTOSCENE__DEBUG_
		printf("[autoscene][%d] FrameCount = %u, %u, Time = %u:%u\n",__LINE__,
				tVideoCapState.dwFrameCount,
				tVideoCapState.dwIndex,
				tVideoCapState.dwSecond,
				tVideoCapState.dwMilliSecond);
#endif
		
		{
			TResizeState tState;

			memset(&tState, 0x0, sizeof(TResizeState));
			tState.dwInStride = ptTaskInfo->dwMaxFrameWidth;
			tState.dwOutStride = ptTaskInfo->dwResizedStride;
			tState.dwInWidth = ptTaskInfo->dwCapWidth;
			tState.dwInHeight = ptTaskInfo->dwCapHeight;
			tState.dwOutWidth = ptTaskInfo->dwResizedWidth;
			tState.dwOutHeight = ptTaskInfo->dwResizedHeight;
			tState.dwImageMode = 0;
			tState.ePixelFormat = YUV420; // input format;
			tState.dwCmptNum = 3;	  
			tState.pbyYInFrame = tVideoCapState.pbyYFrame;  
			tState.pbyCbInFrame = tVideoCapState.pbyCbFrame;
			tState.pbyCrInFrame = tVideoCapState.pbyCrFrame;

			memset(ptTaskInfo->pbyResizedOutputBuf, 0x0, (ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight * 3) /2 );
			tState.pbyYOutFrame = ptTaskInfo->pbyResizedOutputBuf;
			tState.pbyCbOutFrame = ptTaskInfo->pbyResizedOutputBuf + ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight;
			tState.pbyCrOutFrame = ptTaskInfo->pbyResizedOutputBuf + ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight + ((ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight) >> 2);

			if (Resize_CheckParam(ptTaskInfo->hResizeObj, &tState) != S_OK)
			{
				printf("[autoscene][%d] Call Resize_CheckParam() failed!!\n", __LINE__);
				return S_FAIL;
			}

			if (Resize_ProcessOneFrame(ptTaskInfo->hResizeObj, &tState) != S_OK)
			{
				printf("[autoscene][%d] Call Resize_ProcessOneFrame() failed!!\n", __LINE__);
				return S_FAIL;
			}
		}
		
		dwAEShutter = tVideoCapState.dwAEShutter;
		dwAEGain = tVideoCapState.dwAEGain;
		bAEStable = tVideoCapState.bAEStable;
		
		{
			TVideoSignalOptions tVideoSignalOptions;
			memset(&tVideoSignalOptions, 0x0, sizeof(TVideoSignalOptions));
			tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE;
			if (VideoCap_SetOptions(ptTaskInfo->hVideoCapObject, &tVideoSignalOptions) != S_OK)
			{
				fprintf(stderr, "[autoscene][%d] VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE err!!\n", __LINE__);
			}
			dwCurrColorTemp = tVideoSignalOptions.adwUserData[0];
		}

		if (VideoCap_ReleaseBuf(ptTaskInfo->hVideoCapObject, &tVideoCapState) != S_OK) 
		{
			fprintf(stderr, "[autoscene][%d] video_cap_releas_buf err!!\n", __LINE__);                
		}

#if defined(_AUTOSCENE_)
		{
			TAutoSceneState tAutoSceneState;
		
			BOOL bISPChanged = FALSE;
			CHAR szResult[SHARED_MEM_LEN];
			CHAR szReadBuf[SHARED_MEM_LEN];
			int iRet = 0;
			int offset = 0;
			BOOL bWriteResult = FALSE;
			TVideoCapIspParam tVideoCapIspTune;
			TVideoSignalOptions tVideoSignalOptions;
		
			BOOL  bVideoCapSatusUpdate = FALSE;

			DWORD adwCEParam[5];
			DWORD adwTMParam[2];
			SDWORD asdwCCParam[12];			
			DWORD adwSBCParam[3];			
			DWORD adwCFAAntiAliasingParam[10];
			DWORD adwAEParam[11];

			SDWORD asdwOBParam[4];			
			DWORD adwCFADeimpulseParam[7];
			DWORD dwCFAMode = 0;

			DWORD adwAWBParam[4];
			DWORD adwLSCParam[3];
			
			memset(&tVideoCapIspTune, 0x0, sizeof(TVideoCapIspParam));
			memset(&tVideoSignalOptions, 0x0, sizeof(TVideoSignalOptions));
			memset(szResult, 0x0, sizeof(szResult));
			memset(szReadBuf, 0x0, sizeof(szReadBuf));
			memset(&tAutoSceneState, 0x0, sizeof(TAutoSceneState));

			tAutoSceneState.dwCurrShutter = dwAEShutter;
			tAutoSceneState.dwCurrGain = dwAEGain;
#ifdef _AUTOSCENE__DEBUG_		
			printf("[autoscene][%d] ptTaskInfo->bInitial=%d ptTaskInfo->eSceneMode=%d\n", __LINE__, ptTaskInfo->bInitial, ptTaskInfo->eSceneMode); 
#endif
			if (ptTaskInfo->bInitial == TRUE)
			{
				tAutoSceneState.eAutosceneUpdateMode = AUTO_SCENE_INITIAL_SETTING;
				sem_wait(&(ptTaskInfo->smOptions));
				ptTaskInfo->bInitial = FALSE;
				sem_post(&(ptTaskInfo->smOptions));
			}
			else
			{	
				if (ptTaskInfo->eSceneMode == WDR)
				{
					tAutoSceneState.eAutosceneUpdateMode = AUTO_SCENE_WDR_SETTING;
				}
				else if (ptTaskInfo->eSceneMode == OUTDOOR)
				{
					tAutoSceneState.eAutosceneUpdateMode = AUTO_SCENE_OUTDOOR_SETTING;
				}
				else
				{
					tAutoSceneState.eAutosceneUpdateMode = AUTO_SCENE_INDOOR_SETTING;
				}
			}

			//Call setoptions
			{
				TAutoSceneOptions tAutoSceneOptions;
				
				//power line frequency setoption
				if (ptTaskInfo->bPlineFreqChanged == TRUE)
				{
					
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_FREQUENCY;
					tAutoSceneOptions.adwUserData[0] = (ptTaskInfo->dwPlineFreq == 50) ? VIDEO_SIGNAL_FREQUENCY_50HZ : VIDEO_SIGNAL_FREQUENCY_60HZ;
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (VIDEO_SIGNAL_OPTION_SET_FREQUENCY) (%d) \n", __LINE__, ptTaskInfo->dwPlineFreq);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (VIDEO_SIGNAL_OPTION_SET_FREQUENCY) (%d)\n", __LINE__, ptTaskInfo->dwPlineFreq);
					}
					ptTaskInfo->bPlineFreqChanged = FALSE;
				}
				
				// Exposure Level setoption
				if (ptTaskInfo->bExposureLevelChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_EXPOSURE_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwExposureLevel; // (50~200)default : 100
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_EXPOSURE_LEVEL) (%d) \n", __LINE__, ptTaskInfo->dwExposureLevel);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_EXPOSURE_LEVEL) (%d)\n", __LINE__, ptTaskInfo->dwExposureLevel);
					}
					ptTaskInfo->bExposureLevelChanged = FALSE;
				}
				
				// Min Shutter setoption
				if (ptTaskInfo->bExposureMinShutterChanged == TRUE)
				{ 
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwExposureMinShutter; // (1000000~30)default : 1000000
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER) (%d) \n", __LINE__, ptTaskInfo->dwExposureMinShutter);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER) (%d)\n", __LINE__, ptTaskInfo->dwExposureMinShutter);
					}
					ptTaskInfo->bExposureMinShutterChanged = FALSE;
				}
				
				// Max Shutter setoption
				if (ptTaskInfo->bExposureMaxShutterChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwExposureMaxShutter; // default : 30
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER) (%d) \n", __LINE__, ptTaskInfo->dwExposureMaxShutter);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER) (%d)\n", __LINE__,ptTaskInfo->dwExposureMaxShutter);
					}
					ptTaskInfo->bExposureMaxShutterChanged = FALSE;
				}
				
				// Min Gain setoption
				if (ptTaskInfo->bExposureMinGainChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwExposureMinGain; // default : 1
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN) (%d) \n", __LINE__, ptTaskInfo->dwExposureMinGain);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN) (%d)\n", __LINE__, ptTaskInfo->dwExposureMinGain);
					}
					ptTaskInfo->bExposureMinGainChanged = FALSE;
				}
				
				// Max Gain setoption
				if (ptTaskInfo->bExposureMaxGainChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwExposureMaxGain; // default : 32
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN) (%d) \n", __LINE__, ptTaskInfo->dwExposureMaxGain);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN) (%d)\n", __LINE__, ptTaskInfo->dwExposureMaxGain);
					}
					
					ptTaskInfo->bExposureMaxGainChanged = FALSE;
				}
				
				// Brightness setoption
				if (ptTaskInfo->bBrightLevelChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_BRIGHT_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwBrightLevel; // default : 100
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_BRIGHT_LEVEL) (%d) \n", __LINE__, ptTaskInfo->dwBrightLevel);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_BRIGHT_LEVEL) (%d)\n", __LINE__, ptTaskInfo->dwBrightLevel);
					}
					ptTaskInfo->bBrightLevelChanged = FALSE;
				}
				
				// Contrast setoption
				if (ptTaskInfo->bContrastLevelChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_CONTRAST_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwContrastLevel; // default : 100
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_CONTRAST_LEVEL) (%d) \n", __LINE__, ptTaskInfo->dwContrastLevel);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_CONTRAST_LEVEL) (%d)\n", __LINE__, ptTaskInfo->dwContrastLevel);
					}
					ptTaskInfo->bContrastLevelChanged = FALSE;
				}
				
				// Saturation setoption
				if (ptTaskInfo->bSaturationChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_SATURATION_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwSaturation; // default : 100
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_SATURATION_LEVEL) (%d) \n", __LINE__, ptTaskInfo->dwSaturation);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_SATURATION_LEVEL) (%d)\n", __LINE__, ptTaskInfo->dwSaturation);
					}
					ptTaskInfo->bSaturationChanged = FALSE;
				}

				// Color Mode setoption
				if (ptTaskInfo->bMonoChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_MONO;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwMono; // (0/1) default : 0
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_MONO) (%d) \n", __LINE__, ptTaskInfo->dwMono);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_MONO) (%d)\n", __LINE__, ptTaskInfo->dwMono);
					}
					ptTaskInfo->bMonoChanged = FALSE;
				}
				
				// NR Mode setoption
				if (ptTaskInfo->bNRModeChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_NOISE_REDUCTION_MODE;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwNRMode; // (3D NR On 1/Off 0) default : 1 
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_NOISE_REDUCTION_MODE) (%d) \n", __LINE__, ptTaskInfo->dwNRMode);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_NOISE_REDUCTION_MODE) (%d)\n", __LINE__, ptTaskInfo->dwNRMode);
					}
					ptTaskInfo->bNRModeChanged = FALSE;
				}
			
				// Sharpness Level setoption
				if (ptTaskInfo->bSharpnessLevelChanged == TRUE)
				{
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_SHARPNESS_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwSharpnessLevel; // default : 100
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_SHARPNESS_LEVEL) (%d) \n", __LINE__, ptTaskInfo->dwSharpnessLevel);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_SHARPNESS_LEVEL) (%d)\n", __LINE__, ptTaskInfo->dwSharpnessLevel);
					}
					ptTaskInfo->bSharpnessLevelChanged = FALSE;
				}
				
				if (ptTaskInfo->bAEModeChanged == TRUE)
				{
					// Exposure Mode 
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MODE;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwAEMode; // (auto:0, backlight:1, customer:2, default:0)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MODE) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_MODE) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bAEModeChanged = FALSE;	
				}		
				
				if (ptTaskInfo->bIrisModeChanged == TRUE)
				{
					// Iris mode
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_IRIS_MODE;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwIrisMode; // (Fixed to largest:0, Auto iris:1, Manual iris:2, default:2)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_IRIS_MODE) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_IRIS_MODE) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bIrisModeChanged = FALSE;
				}
				
				if (ptTaskInfo->bAutoIrisActiveTimeChanged == TRUE)
				{
					// Auto iris active time
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_IRIS_ACTIVE_TIME;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwAutoIrisActiveTime; // (1000000~30, default:120)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_IRIS_ACTIVE_TIME) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_IRIS_ACTIVE_TIME) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bAutoIrisActiveTimeChanged = FALSE;
				}
				
				if (ptTaskInfo->bIrcutModeChanged == TRUE)
				{
					// ircut mode
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_IRCUT_MODE;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwIrcutMode; // (auto:0, Cut:1, No_cut:2, default:1)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_IRCUT_MODE) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_IRCUT_MODE) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bIrcutModeChanged = FALSE;
				}
				
				if (ptTaskInfo->bGammaLevelChanged == TRUE)
				{
					// Gamma level
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_GAMMA_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwGammaLevel; // (0~15, default:0)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_GAMMA_LEVEL) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_GAMMA_LEVEL) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bGammaLevelChanged = FALSE;
				}
				
				if (ptTaskInfo->bCELevelChanged == TRUE)
				{
					// CE level
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_CE_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwCELevel; // (0~6, default:0)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_CE_LEVEL) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_CE_LEVEL) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bCELevelChanged = FALSE;
						
				}	
				
				if (ptTaskInfo->b2DNRLevelChanged == TRUE)
				{
					// 2D NR strength
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_2D_NOISE_REDUCTION_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dw2DNRLevel; // (50~200, default:100)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_2D_NOISE_REDUCTION_LEVEL) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_2D_NOISE_REDUCTION_LEVEL) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->b2DNRLevelChanged = FALSE;
					
				}
				
				if (ptTaskInfo->b3DNRLevelChanged == TRUE)
				{
					// 3D NR strength
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_3D_NOISE_REDUCTION_LEVEL;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dw3DNRLevel; // (50~200, default:100)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_3D_NOISE_REDUCTION_LEVEL) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_3D_NOISE_REDUCTION_LEVEL) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->b3DNRLevelChanged = FALSE;
				}	
					
				if (ptTaskInfo->bAELockChanged == TRUE)
				{
					// AE Lock
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_LOCK;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwAELock; // (disable:0, enable:1, default:0)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_LOCK) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_EXPOSURE_LOCK) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bAELockChanged = FALSE;
				}
				
				if (ptTaskInfo->bAWBLockChanged == TRUE)
				{
					// AWB Lock
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_WHITE_BALANCE_LOCK;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwAWBLock; // (disable:0, enable:1, default:0)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_WHITE_BALANCE_LOCK) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_WHITE_BALANCE_LOCK) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bAWBLockChanged = FALSE;
				}
				
				if (ptTaskInfo->bAWBModeChanged == TRUE)
				{
					// AWB Mode
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_AUTO_WHITE_BALANCE_MODE;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwAWBMode; // (auto:0, full:1, Usr:2,  default:0)
					tAutoSceneOptions.adwUserData[1] = ptTaskInfo->dwAWBGainR; // (1~8191,  default:1024)
					tAutoSceneOptions.adwUserData[2] = ptTaskInfo->dwAWBGainB; // (1~8191,  default:1024)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_AUTO_WHITE_BALANCE_MODE) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_AUTO_WHITE_BALANCE_MODE) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bAWBModeChanged = FALSE;
				}	
					
				if (ptTaskInfo->bWDRRatioChanged == TRUE)
				{
					// WDR Ratio
					tAutoSceneOptions.eOptionFlags = AUTO_SCENE_OPTION_SET_WDR_RATIO;
					tAutoSceneOptions.adwUserData[0] = ptTaskInfo->dwWDRRatio; // (1,2,4,8,16,32,64,128,256,  default:1)
					if (AutoScene_SetOptions(ptTaskInfo->hAutoSceneObject, &tAutoSceneOptions) != S_OK)
					{
						printf("[autoscene](%d)AutoScene_SetOptions error!! (AUTO_SCENE_OPTION_SET_WDR_RATIO) (%d) \n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					else
					{	
						printf("[autoscene](%d)AutoScene_SetOptions successfully!! (AUTO_SCENE_OPTION_SET_WDR_RATIO) (%d)\n", __LINE__, tAutoSceneOptions.adwUserData[0]);
					}
					ptTaskInfo->bWDRRatioChanged = FALSE;
				}		
				
				
			}//end call setoptions
			
			tAutoSceneState.pbyResizeYFrame = ptTaskInfo->pbyResizedOutputBuf;
			tAutoSceneState.pbyResizeCbFrame = ptTaskInfo->pbyResizedOutputBuf + ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight;
			tAutoSceneState.pbyResizeCrFrame = ptTaskInfo->pbyResizedOutputBuf + ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight + ((ptTaskInfo->dwResizedStride * ptTaskInfo->dwResizedHeight) >> 2);
			tAutoSceneState.bAEStable = bAEStable;
			tAutoSceneState.tAEParam.dwTargetLuma = ptTaskInfo->dwTargetLuma; // Come from UI
			tAutoSceneState.tAEParam.dwTargetOffset = ptTaskInfo->dwTargetOffset; // Come from UI
			tAutoSceneState.pbyStatAEWBBuf = ptTaskInfo->pbyStatAEWBBuf;
			tAutoSceneState.pbyStatHistoBuf = ptTaskInfo->pbyStatHistoBuf;
			tAutoSceneState.dwCurrColorTemp = dwCurrColorTemp;
			tAutoSceneState.dwParamUpdateSel = ISP_SET_NONE;
#ifdef _AUTOSCENE__DEBUG_			
			if (g_bIsDaemon)
			{
				syslog(LOG_INFO, "[autoscene][%d]Before calling AutoScene_ProcessOneFrame().\n", __LINE__);
			}
			else
			{
				printf("[autoscene][%d]Before calling AutoScene_ProcessOneFrame().\n",__LINE__);
			}
#endif
			
			AutoScene_ProcessOneFrame(ptTaskInfo->hAutoSceneObject, &tAutoSceneState);	 
			
			if (isISPEqaul(&tPreAutoSceneState, &tAutoSceneState) == FALSE)
			{
				bISPChanged = TRUE;
				memcpy(&tPreAutoSceneState, &tAutoSceneState, sizeof(TAutoSceneState));
			}
			else
			{
				bISPChanged = FALSE;
			}
			
#ifdef _AUTOSCENE__DEBUG_			
			if (g_bIsDaemon)
			{
				syslog(LOG_INFO, "[autoscene][%d]After calling  AutoScene_ProcessOneFrame().\n", __LINE__);
			}	
			else
			{
				printf("[autoscene][%d]After calling  AutoScene_ProcessOneFrame().\n", __LINE__);
			}
#endif
			iRet = snprintf(szResult, sizeof(szResult), "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?><root>");
			offset += iRet;
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_YUVNR)
			{
				if (bISPChanged == TRUE)
				{
					bWriteResult = TRUE;
					
					iRet = snprintf(szResult+offset, sizeof(szResult)-offset,
							"<denoise>"
							"<enable>%d</enable>"	
							"<mode>%d</mode>"
							"<two_d_mode_strength>%d</two_d_mode_strength>"
							"<three_d_mode_ref_strength>%d</three_d_mode_ref_strength>"
							"<three_d_mode_cur_weight>%d</three_d_mode_cur_weight>"
							"<motion_adaptive_enable>%d</motion_adaptive_enable>"
							"</denoise>",
							tAutoSceneState.tYUVNRParam.bEnYUVNR,
							tAutoSceneState.tYUVNRParam.dwNRMode,
							tAutoSceneState.tYUVNRParam.dw2DNRStrength,
							tAutoSceneState.tYUVNRParam.dw3DNRRefStrength,
							tAutoSceneState.tYUVNRParam.dw3DNRCurrWeight,
							tAutoSceneState.tYUVNRParam.bNRMotionAdaptiveEn
							);
					offset += iRet;
				}
			}	// end if & ISP_SET_YUVNR

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_EDGE)
			{
				if (bISPChanged == TRUE)
				{
					bWriteResult = TRUE;
					iRet = snprintf(szResult+offset, sizeof(szResult)-offset,
							"<edge_enhancement>"
							"<mode>%d</mode>"
							"<edge_sensitivity>%d</edge_sensitivity>"
							"<noise_sensitivity>%d</noise_sensitivity>"
							"<strength>%d</strength>"
							"<clip>%d</clip>"
							"</edge_enhancement>",
							tAutoSceneState.tEdgeEnhanceParam.bEnEdgeEnhance,
							tAutoSceneState.tEdgeEnhanceParam.dwEdgeSensitivity,
							tAutoSceneState.tEdgeEnhanceParam.dwNoiseSensitivity,
							tAutoSceneState.tEdgeEnhanceParam.dwEEStrength,
							tAutoSceneState.tEdgeEnhanceParam.dwEEClip
							);

					offset += iRet;
				}
			}	// end if & ISP_SET_EDGE
 
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_PRE_YUVNR)
			{
				if (bISPChanged == TRUE)
				{
					bWriteResult = TRUE;
					
					iRet = snprintf(szResult+offset, sizeof(szResult)-offset,
							"<pre_denoise>"
							"<enable>%d</enable>"	
							"<mode>%d</mode>"
							"<two_d_mode_strength>%d</two_d_mode_strength>"
							"<three_d_mode_ref_strength>%d</three_d_mode_ref_strength>"
							"<three_d_mode_cur_weight>%d</three_d_mode_cur_weight>"
							"<motion_adaptive_enable>%d</motion_adaptive_enable>"
							"</pre_denoise>",
							tAutoSceneState.tYUVPreNRParam.bEnYUVNR,
							tAutoSceneState.tYUVPreNRParam.dwNRMode,
							tAutoSceneState.tYUVPreNRParam.dw2DNRStrength,
							tAutoSceneState.tYUVPreNRParam.dw3DNRRefStrength,
							tAutoSceneState.tYUVPreNRParam.dw3DNRCurrWeight,
							tAutoSceneState.tYUVPreNRParam.bNRMotionAdaptiveEn
							);
					offset += iRet;
				}
				
			}

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_PRE_EDGE)
			{
				if (bISPChanged == TRUE)
				{
					bWriteResult = TRUE;
					iRet = snprintf(szResult+offset, sizeof(szResult)-offset,
							"<pre_edge_enhancement>"
							"<mode>%d</mode>"
							"<edge_sensitivity>%d</edge_sensitivity>"
							"<noise_sensitivity>%d</noise_sensitivity>"
							"<strength>%d</strength>"
							"<clip>%d</clip>"
							"</pre_edge_enhancement>",
							tAutoSceneState.tPreEdgeEnhanceParam.bEnEdgeEnhance,
							tAutoSceneState.tPreEdgeEnhanceParam.dwEdgeSensitivity,
							tAutoSceneState.tPreEdgeEnhanceParam.dwNoiseSensitivity,
							tAutoSceneState.tPreEdgeEnhanceParam.dwEEStrength,
							tAutoSceneState.tPreEdgeEnhanceParam.dwEEClip
							);

					offset += iRet;
				}
				
			}

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_IRCUT)
			{
				if (bISPChanged == TRUE)
				{
					bWriteResult = TRUE;
					iRet = snprintf(szResult+offset, sizeof(szResult)-offset,
							"<ircut>"
							"<enable>%d</enable>"
							"</ircut>",
							tAutoSceneState.bEnIRCut
							);
					offset += iRet;
				}
			}	// end if & ISP_SET_IRCUT

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_GAMMA)
			{
				//tVideoCapIspTune.pbyGammaLut = &(tAutoSceneState.abyGammaLut[0]);
				tVideoCapIspTune.pbyGammaLut = tAutoSceneState.pbyGammaLut;
				bVideoCapSatusUpdate = TRUE;
			
			}	// end if & ISP_SET_GAMMA

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_TM)
			{
				adwTMParam[0] = tAutoSceneState.tTMParam.bEnTM;
				adwTMParam[1] = tAutoSceneState.tTMParam.dwBlendLevel;
				tVideoCapIspTune.pdwTMParam = &(adwTMParam[0]);
				bVideoCapSatusUpdate = TRUE;
				
			}	// end if & ISP_SET_TM

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_CE)
			{
				
				adwCEParam[0] = tAutoSceneState.tCEParam.bEnCE;
				adwCEParam[1] = tAutoSceneState.tCEParam.dwCEBrightness;
				adwCEParam[2] = (DWORD)tAutoSceneState.tCEParam.pdwCEUsrLut;
				adwCEParam[3] = (DWORD)ptTaskInfo->pdwUsrCELut;/*(&(ptTaskInfo->pdwUsrCELut[0]));*/
				adwCEParam[4] = (DWORD)tAutoSceneState.tCEParam.bTblUpdate;
				tVideoCapIspTune.pdwCEParam = &(adwCEParam[0]);
				bVideoCapSatusUpdate = TRUE;
				
			}	// end if & ISP_SET_CE

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_COLOR)
			{
				
				{
					int k = 0;
					for (k = 0; k < 9; k++)
					{
						asdwCCParam[k] = tAutoSceneState.tColorCorrectionParam.asdwRGB2RGBMatrix[k];
					}
					asdwCCParam[k++] = (SDWORD)tAutoSceneState.tColorCorrectionParam.adwOffset[0];
					asdwCCParam[k++] = (SDWORD)tAutoSceneState.tColorCorrectionParam.adwOffset[1];
					asdwCCParam[k++] = (SDWORD)tAutoSceneState.tColorCorrectionParam.adwOffset[2];
					tVideoCapIspTune.psdwColorParam = &(asdwCCParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}	// end if & ISP_SET_COLOR

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_SBC)
			{
				
				{
					adwSBCParam[0] = (DWORD)tAutoSceneState.tSBCParam.sdwBright;
					adwSBCParam[1] = tAutoSceneState.tSBCParam.dwSaturation;
					adwSBCParam[2] = (DWORD)tAutoSceneState.tSBCParam.sdwContrast;
					tVideoCapIspTune.pdwSBCParam = &(adwSBCParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}	// end if & ISP_SET_SBC
			
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_ANTIALIASING)
			{
				
				{
					adwCFAAntiAliasingParam[0] = tAutoSceneState.tAntialiasingParam.dwCFAHVBlendEn;
					adwCFAAntiAliasingParam[1] = tAutoSceneState.tAntialiasingParam.dwCFAHVAvgThrd;
					adwCFAAntiAliasingParam[2] = tAutoSceneState.tAntialiasingParam.dwCFAHVBlendMinThrd;
					adwCFAAntiAliasingParam[3] = tAutoSceneState.tAntialiasingParam.dwCFAHVBlendMaxThrd;
					adwCFAAntiAliasingParam[4] = tAutoSceneState.tAntialiasingParam.dwCFAFCSEn;
					adwCFAAntiAliasingParam[5] = tAutoSceneState.tAntialiasingParam.dwCFAFCSEdgeThrd;
					adwCFAAntiAliasingParam[6] = tAutoSceneState.tAntialiasingParam.dwCFAFCSMinThrd;
					adwCFAAntiAliasingParam[7] = tAutoSceneState.tAntialiasingParam.dwCFAFCSMaxThrd;
					adwCFAAntiAliasingParam[8] = tAutoSceneState.tAntialiasingParam.dwCFAFCSExcludeEdge;
					adwCFAAntiAliasingParam[9] = tAutoSceneState.tAntialiasingParam.dwCFAFCSAlpha;
					tVideoCapIspTune.pdwAntialiasingParam = &(adwCFAAntiAliasingParam[0]);
					bVideoCapSatusUpdate= TRUE;
				}
			}	// end if & ISP_SET_ANTIALIASING
			
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_AE)
			{
				
				{
					adwAEParam[0] = tAutoSceneState.tAEParam.dwTargetLuma;
					adwAEParam[1] = tAutoSceneState.tAEParam.dwTargetOffset;
					adwAEParam[2] = tAutoSceneState.tAEParam.dwIrisActiveTime;
					adwAEParam[3] = tAutoSceneState.tAEParam.dwMinShutter;
					adwAEParam[4] = tAutoSceneState.tAEParam.dwMaxShutter;
					adwAEParam[5] = tAutoSceneState.tAEParam.dwMinGain;
					adwAEParam[6] = tAutoSceneState.tAEParam.dwMaxGain;
					adwAEParam[7] = tAutoSceneState.tAEParam.dwMode;
					adwAEParam[8] = (DWORD)tAutoSceneState.tAEParam.bLock;
					adwAEParam[9] = tAutoSceneState.tAEParam.dwSpeed;
					adwAEParam[10] = tAutoSceneState.tAEParam.dwIrisStatus;
					tVideoCapIspTune.pdwAEParam = &(adwAEParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}	// end if  & ISP_SET_AEPARAM
			
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_CFADEIMPULSE)
			{
				
				{
					adwCFADeimpulseParam[0] = tAutoSceneState.tCFADeimpulseParam.bDeImpulseEn;
					adwCFADeimpulseParam[1] = tAutoSceneState.tCFADeimpulseParam.dwDcTopThrd;
					adwCFADeimpulseParam[2] = tAutoSceneState.tCFADeimpulseParam.dwDcBotThrd;
					adwCFADeimpulseParam[3] = tAutoSceneState.tCFADeimpulseParam.dwMaxTopLevel;
					adwCFADeimpulseParam[4] = tAutoSceneState.tCFADeimpulseParam.dwMaxBotLevel;
					adwCFADeimpulseParam[5] = tAutoSceneState.tCFADeimpulseParam.dwMinTopLevel;
					adwCFADeimpulseParam[6] = tAutoSceneState.tCFADeimpulseParam.dwMinBotLevel;
					tVideoCapIspTune.pdwDeimpulseParam = &(adwCFADeimpulseParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}	// end if & ISP_SET_CFADEIMPULSE
			
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_BLACKCLAMP)
			{
				
				{
					asdwOBParam[0] = tAutoSceneState.tBlackClampParam.swBlackClampGr;
					asdwOBParam[1] = tAutoSceneState.tBlackClampParam.swBlackClampR;
					asdwOBParam[2] = tAutoSceneState.tBlackClampParam.swBlackClampGb;
					asdwOBParam[3] = tAutoSceneState.tBlackClampParam.swBlackClampB;
					tVideoCapIspTune.psdwBlackClampParam = &(asdwOBParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}	// end if & ISP_SET_BLACKCLAMP
			
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_CFAMODE)
			{
				
				{
					dwCFAMode = tAutoSceneState.dwCFAMode;
					tVideoCapIspTune.pdwCFAMode = &dwCFAMode;
					bVideoCapSatusUpdate = TRUE;
				}
			}	// end if & ISP_SET_CFAMODE
			
			
			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_AWB)
			{	
				
				{
					adwAWBParam[0] = (DWORD)tAutoSceneState.tAWBParam.bLock;
					adwAWBParam[1] = tAutoSceneState.tAWBParam.dwMode;
					adwAWBParam[2] = tAutoSceneState.tAWBParam.dwCustomGainR;
					adwAWBParam[3] = tAutoSceneState.tAWBParam.dwCustomGainB;
					tVideoCapIspTune.pdwAWBParam = &(adwAWBParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}

			if (tAutoSceneState.dwParamUpdateSel & ISP_SET_LSC)
			{
				
				{
					adwLSCParam[0] = (DWORD)tAutoSceneState.tLSCParam.bLSCEn;
					adwLSCParam[1] = (DWORD)tAutoSceneState.tLSCParam.bTblUpdate;
					if ((tAutoSceneState.tLSCParam.bTblUpdate) && (tAutoSceneState.tLSCParam.pwPhotoLSCTbl != NULL))
					{
						memcpy(ptTaskInfo->pwLSCTbl, tAutoSceneState.tLSCParam.pwPhotoLSCTbl, ptTaskInfo->dwLSCTblSize*sizeof(WORD));
						adwLSCParam[2] = (DWORD)ptTaskInfo->pwLSCTbl;
					}
					tVideoCapIspTune.pdwLSCParam = &(adwLSCParam[0]);
					bVideoCapSatusUpdate = TRUE;
				}
			}
			
			if (bVideoCapSatusUpdate)
			{
				bVideoCapSatusUpdate = FALSE;
				memset(ptTaskInfo->pbyRGBGammaLut, 0x0, 4096*3);
				//memset(ptTaskInfo->pdwUsrCELut, 0x0, 256*sizeof(DWORD));
				tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE;
				tVideoSignalOptions.adwUserData[0] = (DWORD)(&tVideoCapIspTune);
				tVideoSignalOptions.adwUserData[1] = (DWORD)(ptTaskInfo->pbyRGBGammaLut);
				//tVideoSignalOptions.adwUserData[2] = (DWORD)(ptTaskInfo->pdwUsrCELut);
				
				if (VideoCap_SetOptions(ptTaskInfo->hVideoCapObject, &tVideoSignalOptions) != S_OK)
				{
					printf("[autoscene][%d]Call VideoCap_SetOptions (VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE) error !!\n", __LINE__);
				}
				
			}
			else
			{
#ifdef _AUTOSCENE__DEBUG_
				printf("[autoscene][%d]No actions for VIC of autoscene must be done!! \n", __LINE__);
#endif
			}
			
			snprintf(szResult+offset, sizeof(szResult)-offset, "</root>");
			
			if (bWriteResult == TRUE)
			{
				if (write_shm(szResult) == S_FAIL)
				{
					printf("[autoscene][%d]Can not write data to %s\n", __LINE__, SHARED_MEM_PATH_NAME);
				}
				else
				{
					system("/usr/bin/msgctrl -f /tmp/venc/c0/command.fifo  -h videoin -c ISPUpdateState");
#ifdef _AUTOSCENE__DEBUG_
					read_shm(szReadBuf, sizeof(szReadBuf));
					printf("szReadBuf:\n");
					printf("%s\n", szReadBuf);
					printf("End of szReadBuf!\n");
#endif
				}
				bWriteResult = FALSE;
			}
			else
			{
#ifdef _AUTOSCENE__DEBUG_
				printf("[autoscene][%d]No actions for IBPE of autoscene must be done!! \n", __LINE__);
#endif
			}
		} // end ifdef _AUTOSCENE_
#endif      
		return S_OK;
	} 
	else
	{
		if (tVideoCapState.bNoSignal == TRUE) 
		{
			printf("[autoscene][%d] No signal !!\n", __LINE__);
		}
		if (tVideoCapState.bFifoFull == TRUE)
		{
			printf("[autoscene][%d] fifo full !!\n", __LINE__);
		}
	}
	return S_FAIL;
} 

#define	USPS	1000000		/* # of microseconds in a second */
unsigned int my_ualarm(unsigned int usecs, unsigned int interval)
{
  struct itimerval new, old;
	
  new.it_interval.tv_usec = interval % USPS;
  new.it_interval.tv_sec = interval / USPS;
  new.it_value.tv_usec = usecs % USPS;
  new.it_value.tv_sec = usecs / USPS;

  if (setitimer(ITIMER_REAL, &new, &old) == 0)
    return (old.it_value.tv_sec * USPS + old.it_value.tv_usec);

  return 0;
}

SCODE task_loop(TTaskInfo *ptTaskInfo)
{
	BYTE abyBuffer[512];
	fd_set rfds;
	int iFifoFD = -1;
	int iReadSz = -1;
	int ret = -1;
	
	iFifoFD = MsgReader_GetFifoFd(ptTaskInfo->hMsgReaderObj);
	autoscene_signal(SIGINT, autoscene_sighandler);
	autoscene_signal(SIGHUP, autoscene_sighandler);
	
	while (1) 
	{	
		if (g_iCommand == rscShutDown)
		{
			printf("[autoscene][%d]Exit task loop!!\n",__LINE__);
			g_iCommand=rscNoCommand;
			goto exit;
		}

		if(g_iCommand == rscReloadConfigFile)
		{
			sem_wait(&(ptTaskInfo->smOptions));
			if(LoadConfig(ptTaskInfo)==S_FAIL)
			{
				printf("[autoscene][%d] call LoadConfig() failed!!\n",__LINE__);
				goto exit;
			}
			sem_post(&(ptTaskInfo->smOptions));
			g_iCommand=rscNoCommand;
		}

		if (got_interrupt) 
		{
#ifdef _AUTOSCENE__DEBUG_
			printf("\n[autoscene][%d]got_interrupt=%d (%lu)\n", __LINE__, got_interrupt, (unsigned long)time(NULL));
#endif
			got_interrupt = 0;
			//alarm(0);
			my_ualarm(0,0);
			do_autoscene(ptTaskInfo);
			signal(SIGALRM, sig_alarm);
			//alarm(ptTaskInfo->dwTriggerTime);
			my_ualarm(ptTaskInfo->dwTriggerTime, ptTaskInfo->dwTriggerTime);
		}

		memset(abyBuffer, 0, 512);
		FD_ZERO(&rfds);
		FD_SET(iFifoFD, &rfds);

		ret = select(iFifoFD + 1, &rfds, NULL, NULL, NULL);
		if (ret == -1)
		{	
			if (errno == EINTR) 
			{
				continue;
			}
			goto exit;		
		}	

		if (FD_ISSET(iFifoFD, &rfds))
		{
			iReadSz = read(iFifoFD, abyBuffer, sizeof(abyBuffer));
			MsgReader_ParseMsg(ptTaskInfo->hMsgReaderObj, abyBuffer, iReadSz);
			if (strcasecmp(ptTaskInfo->ptMsg->szHost, AUTOSCENE) == 0)
			{
				if (strcasecmp(ptTaskInfo->ptMsg->szContent, START) == 0)
				{
					printf("(%d)start\n", __LINE__);
					signal(SIGALRM, sig_alarm);
					//alarm(ptTaskInfo->dwTriggerTime);
					my_ualarm(ptTaskInfo->dwTriggerTime, ptTaskInfo->dwTriggerTime);
				}
				else if(strcasecmp(ptTaskInfo->ptMsg->szContent, STOP) == 0)
				{  	
					printf("(%d)stop\n", __LINE__);
					//alarm(0);
					my_ualarm(0,0);
				}
				else if (strcasecmp(ptTaskInfo->ptMsg->szContent, INIT) == 0)
				{
					printf("(%d)initial\n", __LINE__);
					sem_wait(&(ptTaskInfo->smOptions));
					ptTaskInfo->bInitial = TRUE;
					sem_post(&(ptTaskInfo->smOptions));
				}
				else if (strcasecmp(ptTaskInfo->ptMsg->szContent, INIT_AND_START) == 0)
				{
					printf("(%d)initial_and_start\n", __LINE__);
					sem_wait(&(ptTaskInfo->smOptions));
					ptTaskInfo->bInitial = TRUE;
					sem_post(&(ptTaskInfo->smOptions));
					signal(SIGALRM, sig_alarm);
					my_ualarm(ptTaskInfo->dwTriggerTime, ptTaskInfo->dwTriggerTime);
				}
				
			}
		}
	}// end of while(1)
exit:
	return S_OK;
}

void print_usage(void)
{
	fprintf(stderr,	"Usage:\n"
			"    autoscene [-D] [-c config_file][-h] [-p pidfile][-s sockfile]\n"
			"Options:\n"
			"    -D                 Run as Daemon\n"
			"    -c config_file     Configuration file of Video Recoder\n"
			"    -p pid_file        Write PID to this file\n"
			"-s sock_file domain socket file\n"
			"    -h                 This help\n");
}

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
	
	if (getppid() == 1) return;
	
	pid = fork();
	if (pid < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0) 
	{
		exit(EXIT_SUCCESS);
	}
	
	umask(0);
	
	sid = setsid();
	
	if (sid < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	if ((chdir("/")) < 0) 
	{
		exit(EXIT_FAILURE);
	}
	
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);
}

int main(int argc, char *argv[])
{
	TTaskInfo *ptTaskInfo = NULL;
	int i = -1;
	BOOLEAN bIsDaemon = FALSE;
	CHAR *szPidFile = NULL;
	CHAR *szConfigFile = NULL;
	
	while ((i=getopt(argc, argv, "Dc:p:h")) != -1)
	{
		switch (i)
		{
			case 'D':
				bIsDaemon = TRUE;
				break;
			case 'p':
				szPidFile = strdup(optarg);
				break;
			case 'c':
				szConfigFile = strdup(optarg);
				break;
			case 'h':
			default:
				print_usage();
				goto exit_2;
		}
	}
	
	if ((szConfigFile == NULL) || (szPidFile == NULL))
	{
		print_usage();
		exit(1);
	}
	
	if (bIsDaemon)
	{
		syslog(LOG_INFO, "Start autoscene porcess with Pid : %d\n", getpid());
	}
	
	if (bIsDaemon){
		daemonize();
		g_bIsDaemon = TRUE;
	}
	
	if (szPidFile != NULL)
	{
		if (genPIDfile(szPidFile) != S_OK)
		{
			fprintf(stderr, "Generate pid file fail !!\n");
			exit(1);
		}
	}

	{
		FILE *fp = NULL;
		if ((fp = fopen(szConfigFile, "r")) == NULL)
		{
			fprintf(stderr, "Can not find the config file %s !!\n", szConfigFile);
			goto exit_2;
		}
		if (fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
	}

	ptTaskInfo = (TTaskInfo *)malloc(sizeof(TTaskInfo));

	if (ptTaskInfo == NULL)
	{
		return 0;
	}
	
	memset(ptTaskInfo, 0x0, sizeof(TTaskInfo));
	ptTaskInfo->szConfigFile = strdup(szConfigFile);

	if (init_task_info(ptTaskInfo) != S_OK)
	{
		goto exit_1;
	}

	if (create_shm() == S_OK)
	{
		task_loop(ptTaskInfo);
		unlink_shm();
	}
	
exit_1:  
	rls_task_info(ptTaskInfo);
	if (ptTaskInfo->szConfigFile != NULL)
	{
		free(ptTaskInfo->szConfigFile);
		ptTaskInfo->szConfigFile = NULL;
	}
	if (ptTaskInfo != NULL)
	{
		free(ptTaskInfo);
		ptTaskInfo = NULL;
	}
	
exit_2:
	if (szConfigFile != NULL)
	{
		free(szConfigFile);
		szConfigFile = NULL;
	}
	if (szPidFile != NULL)
	{
		delPIDfile(szPidFile);
	}
	if (bIsDaemon)
	{
		syslog(LOG_INFO, "Stop autoscene porcess !!\n");
	}
	if (szPidFile != NULL)
	{
		free(szPidFile);
		szPidFile = NULL;
	}
	
	printf("[autoscene][%d]Stop autoscene porcess!!\n", __LINE__);
	return 0;
}
