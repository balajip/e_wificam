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
#include <sys/types.h>
#include <dirent.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/vfs.h>
#include <time.h>
#include <mntent.h>
#include "dbgdefs.h"
#include "errordef.h"
#include "typedef.h"
#include "xmlwrapper.h"
/*

Function: Format
CGI method: Get
Request:


start=[YYYY][mm][dd][hh][mm][ss]&end=[YYYY][mm][dd][hh][mm][ss]

*/
/*
http://172.17.255.110/vrecord/videoclips/Record_20101116154642_G726_H264.avi
(1) If there is no error:
<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>
<videoclip>
      <video_codec></video_codec>
      <audio_codec></audio_codec>
      <trigger_time></trigger_time>
      <file_name></file_name>    
</videoclip>
<dir_url></dir_url>
<total_num></total_num>
<status>OK</status>
<message>NO_ERROR</message>
</root>

(2)If there is an error:
<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>
<status>ERROR</status>
<message>FS_MOUNT_ERROR</message>
</root>
*/
//%Y(4)%m(2)%d(2)%H(2)%M(2)%S(2) 4+2+2+2+2+2+1=15
#define RECORD_TIMESTAMP_LEN 15
#define RECORD_FILENAME_LEN 64
#define RECORD_CODEC_STR_LEN 8
#define FILEPATH_LEN 1024
#define SD_MOUNT_POINT "/usr/share/www/html/vrecord/videoclips"
#define AUTOFS_SD_MOUNT_POINT     "/tmp/vrecord/videoclips"
#define RECORD_WEB_DIR "vrecord/videoclips"
#define VREC_SIGNATURE ".VREC_SIGNATURE"
#define RECORD_FILE_PRIFIX "Record"
//#define TMPFILE "/tmp/tmpfile.vrecsearch"
//#define RECORD_LEN 60

typedef struct videoClip
{
  char szTriggerTime[RECORD_TIMESTAMP_LEN];
  char szAudioCodec[RECORD_CODEC_STR_LEN];
  char szVideoCodec[RECORD_CODEC_STR_LEN];
  char szFileName[RECORD_FILENAME_LEN];
  
}TVideoClip;
static TVideoClip **ppTVideoClip;
static void  u_sleep(int usec)
{
  struct timeval tv;
  tv.tv_sec=0;
  tv.tv_usec=usec;
  
  select(0,NULL,NULL,NULL,&tv);

  return;
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
  strcpy(szTmpfileName,"/tmp/vrecsearch-XXXXXX"); 
 // printf("[sdmgr](%d)\n",__LINE__);
  umask(0077); // ensure exclusive access on buggy implementations of mkstemp
  fd = mkstemp(szTmpfileName);
  //printf("[sdmgr](%d)szTmpfileName=%s\n",__LINE__,szTmpfileName);
  close(fd);
  
  return S_OK;
 
}

static time_t str_to_time(const char *szTime)
{

  char szYear[5],szMon[3],szMday[3],szHour[3],szMin[3],szSec[3];
  struct tm t1;

  memset(szYear,'\0',5);
  memset(szMon,'\0',3);
  memset(szMday,'\0',3);
  memset(szHour,'\0',3);
  memset(szMin,'\0',3);
  memset(szSec,'\0',3);


  strncpy(szYear,(char *)szTime,4);
  strncpy(szMon,(char *)szTime+4,2);
  strncpy(szMday,(char *)szTime+4+2,2);
  strncpy(szHour,(char *)szTime+4+2+2,2);
  strncpy(szMin,(char *)szTime+4+2+2+2,2);
  strncpy(szSec,(char *)szTime+4+2+2+2+2,2);
 
  t1.tm_year=atoi(szYear)-1900;
  t1.tm_mon=atoi(szMon);
  t1.tm_mday=atoi(szMday);
  t1.tm_hour=atoi(szHour);
  t1.tm_min=atoi(szMin);
  t1.tm_sec=atoi(szSec);
  t1.tm_isdst=0;

  return mktime(&t1);
  
  
}
static int compare(const void **a,const void **b)
{

 time_t t1_of_day=str_to_time(((TVideoClip *)*a)->szTriggerTime);
 

 time_t t2_of_day=str_to_time(((TVideoClip *)*b)->szTriggerTime);;

 if( t1_of_day== t2_of_day) return 0;
 if( t1_of_day> t2_of_day) return 1;
 if( t1_of_day< t2_of_day) return -1;

 return 2;
   
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
   
	if(strlen(RECORD_FILE_PRIFIX)==strlen(szRecordPrefix))
	{
	    if(strncmp(szRecordPrefix,RECORD_FILE_PRIFIX,strlen(RECORD_FILE_PRIFIX))==0)
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
static SCODE sort_recordfile(const char *szRecordPath,int *pIRecordNum)
{
   
    DIR *dir=NULL;
    int i=0;
    int filenum=0;
    struct  dirent *ptr=NULL;
    //char **pszFilename=NULL;
    const char *szRecDir=szRecordPath;
    SCODE sRet=S_OK;
    
    dir=opendir(szRecDir);
    while((ptr=readdir(dir))!=NULL)
    {
      if(strcmp(ptr->d_name,"..")==0 || strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,VREC_SIGNATURE)==0|| is_valid_record_filename(ptr->d_name)==FALSE)
      {
	continue;
      }
      //printf("d_name=%s\n",ptr->d_name);
      i++;
    }
   
    filenum=i;
    *pIRecordNum=filenum;
   // printf("[vrec](%d)There %d files in %s\n",__LINE__, filenum,szRecDir);
    if(filenum!=0)
    {
	
      
      
	//printf("[vrec](%d)\n",__LINE__);
	ppTVideoClip=(TVideoClip **)malloc(sizeof(TVideoClip *)*filenum);
	//printf("[vrec](%d)\n",__LINE__);
	for(i=0;i<filenum;i++)
	{
	  
	  ppTVideoClip[i]=(TVideoClip *)malloc(sizeof(TVideoClip));
	  memset( ppTVideoClip[i],0x0,sizeof(TVideoClip));
	} 
	// printf("[vrec](%d)\n",__LINE__);
	if(dir != NULL)
	{
	  closedir(dir);
	}
	//printf("[vrec](%d)\n",__LINE__);
	dir=opendir(szRecDir);
	i=0;
	//printf("[vrec](%d)\n",__LINE__);
	while((ptr=readdir(dir))!=NULL)
	{	
	  if(strcmp(ptr->d_name,"..")==0 || strcmp(ptr->d_name,".")==0 ||strcmp(ptr->d_name,VREC_SIGNATURE)==0||is_valid_record_filename(ptr->d_name)==FALSE)
	  {
	    continue;
	  }
      
	  strncpy(ppTVideoClip[i]->szFileName,ptr->d_name,RECORD_FILENAME_LEN);
	  //The pattern of recording file is Reocrd_%Y%m%d%H%M%S_[AudioCodec]_[VideoCodec].avi.
	  strtok(ptr->d_name,"_.");
	  
	  strncpy(ppTVideoClip[i]->szTriggerTime,strtok(NULL,"_."),RECORD_TIMESTAMP_LEN);
	  strncpy(ppTVideoClip[i]->szAudioCodec,strtok(NULL,"_."),RECORD_CODEC_STR_LEN);
	  strncpy(ppTVideoClip[i]->szVideoCodec,strtok(NULL,"_."),RECORD_CODEC_STR_LEN);
	   
	  //pszFilename[%d] is like %Y%m%d%H%M%S
	  //strncpy(pszFilename[i],strtok(NULL,"_."),RECORD_TIMESTAMP_LEN);
	  i++;
	} 
	// printf("[vrec](%d)\n",__LINE__);
	qsort(ppTVideoClip,filenum,sizeof(TVideoClip *),compare);
	// printf("[vrec](%d)\n",__LINE__);
	//for(i=0;i<filenum;i++)
	//{
	  //;
	 // printf("[vrec](%d)pszFilename[%d]=%s\n",__LINE__,i,pszFilename[i]);
	//}
	// printf("[vrec](%d)\n",__LINE__);
	//The oldest file could be *.3gp or *.avi
	//snprintf(szOldestRescordFilePath,RECORD_FILEPATH_LEN,"%s/%s_%s.%s",szRecordPath,RECORD_FILE_PRIFIX,pszFilename[0],"*");
    }  
    // printf("[vrec](%d)\n",__LINE__); 
    if(dir != NULL)
    {
      closedir(dir);
    }
    return sRet;


}

static BOOLEAN is_record_sd(const char *szRecordPath)
{
    BOOLEAN bRet=FALSE;
    char szFilePath[FILEPATH_LEN];
    FILE *fp=NULL;
    memset(szFilePath,'\0',FILEPATH_LEN);
    snprintf(szFilePath,FILEPATH_LEN,"%s/%s",szRecordPath,VREC_SIGNATURE);      
    if((fp=fopen(szFilePath,"r"))!=NULL)
    {
      bRet=TRUE;
    }
    if(fp!=NULL)
    {
      fclose(fp);
    }
    return bRet;
}
static BOOLEAN has_none_codec(const char *szFileName)
{
    BOOLEAN bRet=FALSE;
    char szName[RECORD_FILENAME_LEN];
    memset(szName,'\0',RECORD_FILENAME_LEN);
    strncpy(szName,szFileName,RECORD_FILENAME_LEN);
    strtok(szName,"_.");
    strtok(NULL,"_.");//trigger time
    if(strncmp(strtok(NULL,"_."),"NONE",RECORD_CODEC_STR_LEN)==0)//Audio codec
    { 
	 bRet=TRUE;
    }
    
    if(strncmp(strtok(NULL,"_."),"NONE",RECORD_CODEC_STR_LEN)==0)//Video codec
    { 
	 bRet=TRUE;
    }
    return bRet;


}

static struct mntent *find_mount_point(const char *name, const char *table)
{
	struct stat s;
	dev_t mountDevice;
	FILE *mountTable;
	struct mntent *mountEntry;

	if (stat(name, &s) != 0)
		return 0;

	if ((s.st_mode & S_IFMT) == S_IFBLK)
		mountDevice = s.st_rdev;
	else
		mountDevice = s.st_dev;


	mountTable = setmntent(table , "r");
	if (!mountTable)
		return 0;

	while ((mountEntry = getmntent(mountTable)) != 0) {
		if (strcmp(name, mountEntry->mnt_dir) == 0
		 || strcmp(name, mountEntry->mnt_fsname) == 0
		) { /* String match. */
			break;
		}
		if (stat(mountEntry->mnt_fsname, &s) == 0 && s.st_rdev == mountDevice)	/* Match the device. */
			break;
		if (stat(mountEntry->mnt_dir, &s) == 0 && s.st_dev == mountDevice)	/* Match the directory's mount point. */
			break;
	}
	endmntent(mountTable);
	return mountEntry;
}

BOOLEAN Mounted(const char *szMountPoint)
{
   
   
   struct mntent *mount_entry;
   BOOLEAN bMounted=FALSE;
   if((mount_entry=find_mount_point(szMountPoint, "/proc/mounts"))!=NULL)
   {
     if(strncmp(mount_entry->mnt_dir,szMountPoint,strlen(szMountPoint))==0)
     {
      bMounted=TRUE;
     }
    }
    return bMounted;
   #if 0
   char  szCommand[128];
   struct stat stFile;
   // printf("(%d)\n",__LINE__);
   memset(szCommand,'\0',128);
   // printf("(%d)\n",__LINE__);
   if (szMountPoint != NULL)
   {
     // printf("(%d)\n",__LINE__);
    snprintf(szCommand,128,"/bin/df  |  grep -r %s  > %s",szMountPoint,TMPFILE);
     //printf("(%d)\n",__LINE__);
    system(szCommand);
     //printf("(%d)\n",__LINE__);
    stat(TMPFILE,&stFile);
     //printf("(%d)\n",__LINE__);
    unlink(TMPFILE);
     //printf("(%d)\n",__LINE__);
    if (stFile.st_size !=0)
    {
	
	return TRUE;
    }
     //printf("(%d)\n",__LINE__);
  }
  #endif 
  return FALSE;

}
SCODE UmountFS(const char *szRecordPath)
{
   char  szCommand[128];
   struct stat stFile;
    char  szTmpFileName[64];
   // printf("(%d)\n",__LINE__);
   memset(szCommand,'\0',128);
memset(szTmpFileName,'\0',64);
   // printf("(%d)\n",__LINE__);
  
//  printf("(%d)umount %s\n",__LINE__,szRecordPath);
  system("sync;sync;sync");
  usleep(200000); 
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
  return S_OK;
}
//The CheckVrecIsRecording() uses /proc/[pid_of_vrec]/fd to check if vrec is  recording or not. 
BOOLEAN CheckVrecIsRecording(const char *szMountPoint)
{
  BOOLEAN bIsRecording=FALSE;
  DIR *dir=NULL;
  
  struct  dirent *ptr=NULL;
  char szFdDir[128];
  int pid=0;
  FILE *fp=NULL;
  char  szCommand[128];
  char szFdPath[128];
  struct stat stFile;
  char buf[1024];
  int len;
  char szTmpFileName[64];
  
  memset(szTmpFileName,'\0',64);
  memset(szFdPath,'\0',128);
  memset(szFdDir,'\0',128);
  memset(szCommand,'\0',128);
  memset(szCommand,'\0',128);
  memset(buf,'\0',1024);
 create_tmpfile(szTmpFileName);
  snprintf(szCommand,128,"/bin/pidof  %s  > %s","vrec",szTmpFileName);
  
  system(szCommand);
  stat(szTmpFileName,&stFile);
  
  if (stFile.st_size !=0)
  {
      fp=fopen(szTmpFileName,"r");
      if(fp != NULL)
      {
	  fscanf(fp,"%d",&pid);
	  //printf("(%d)pid=%d\n",__LINE__,pid);
	  snprintf(szFdDir,128,"/proc/%d/fd",pid);
	  //printf("szFdDir=%s\n",szFdDir);
	  dir=opendir(szFdDir);
	  while((ptr=readdir(dir))!=NULL)
	  {
	      if(strcmp(ptr->d_name,"..")==0 || strcmp(ptr->d_name,".")==0 )
	      {
		continue;
	      }
	      //printf("d_name=%s\n",ptr->d_name);
	      snprintf(szFdPath,128,"%s/%s",szFdDir,ptr->d_name);
	      if ((len = readlink(szFdPath, buf, sizeof(buf)-1)) != -1)
	      {
		buf[len] = '\0';
	      }
	      if(strncmp(buf,AUTOFS_SD_MOUNT_POINT,strlen(szMountPoint))==0)
	      {
		bIsRecording=TRUE;
	      }
	    //printf("buf=%s\n",buf);
	    //i++;
	  }
      }
    
   }
   
   if(fp!=NULL)
   {
     fclose(fp);
   }  
   unlink(szTmpFileName);
  return bIsRecording;
}
int main ( int argc, char *argv[] )
{
  char *szDelimiter1="&";
  char *szDelimiter2="=";
  char  *szTemp1=NULL;
  char  *szTemp2=NULL;
  char 	*szStart=NULL;
  char  *szEnd=NULL;
  time_t tm_start;
  time_t tm_end;
  //struct stat stFile;
  //BOOLEAN bMounted=FALSE;
  //BOOLEAN bNewMount=FALSE;
  char szQueryStr[128];
  char szCommand[128];
//  char szDevFilePath[128];
  char  szTemp3[24];
  char  szSDDevName[16];
  FILE *fp=NULL;
  //TVideoClip **ppTVideoClip=NULL;
  int iRecordNum=0;
  int i=0;
  double fDiffEnd=0.0;
  double fDiffStart=0.0;
  unsigned long dwDiffEndNum=0;
  unsigned long dwDiffStartNum=0;
  int iFoundStartIndex=0;
  int iFoundEndIndex=0;
  int iCount=0;
  BOOLEAN bSkipTheLastVideoClip=FALSE;
  
  DWORD dwRecordLen=1;
  
  char szTmpFileName[64];
  
 
  
  
  printf("Content-type: text/xml\r\n");
  printf("Cache-Control: no-cache\r\n\n"); 
  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
  printf("<root>\n");
  
  if (getenv("QUERY_STRING")==NULL)
  {
	printf("<status>ERROR</status>\n");
	printf("<message>QUERY_STRING_NULL</message>\n");
	goto exit;
	
  }
  
  memset(szQueryStr,'\0',128);
  strncpy(szQueryStr,getenv("QUERY_STRING"),strlen(getenv("QUERY_STRING")));
  szTemp1= strtok (szQueryStr,szDelimiter1);
  szTemp2 = strtok (NULL, szDelimiter1);
  //printf("szTemp1=%s szTemp2=%s\n",szTemp1,szTemp2 );
  
  strtok(szTemp1,szDelimiter2);
  szStart=strtok(NULL,szDelimiter2);
  strtok(szTemp2,szDelimiter2);
  szEnd=strtok(NULL,szDelimiter2);
  //printf("szStart=%s szEnd=%s\n",szStart,szEnd );
  tm_start=str_to_time(szStart);
  tm_end=str_to_time(szEnd);
  if(tm_end< tm_start)
  {
      printf("<status>ERROR</status>\n");
      printf("<message>END_EARLIER_START</message>\n");
      goto exit;
  }
  
  memset(szSDDevName,'\0',16);
  memset(szTemp3,'\0',24);

  u_sleep(10);
  memset(szCommand,'\0',128);
  memset(szTmpFileName,'\0',64);
   create_tmpfile(szTmpFileName);
  snprintf(szCommand,128,"cat /proc/partitions |  grep -r \"mmcblk\" | tail -n 1 |  awk -F' ' '{ print $4}' > %s",szTmpFileName);
  system(szCommand);
  fp=fopen(szTmpFileName,"r");
  if(fp != NULL)
  {	
      // /dev/mmcblk0           3940828         4   3940824   0% /mnt/ramdisk
      fscanf(fp,"%s",szTemp3);
      fclose(fp);
  }
  else
  {
      printf("<status>ERROR</status>\n");
      printf("<message>SD_DETATCHED</message>\n");
      goto exit;
    
  }  
    
  strncpy(szSDDevName,szTemp3,strlen("mmcblk")+1);
    
  if (szSDDevName[0] == '\0')
  {
	  printf("<status>ERROR</status>\n");
	    printf("<message>SD_DETATCHED</message>\n");
	    goto exit;
  }
  
  
  
  unlink(szTmpFileName);
  #if 0
  memset(szCommand,'\0',128);
 
  snprintf(szCommand,128,"/bin/df  |  grep -r %s  > %s",SD_MOUNT_POINT,TMPFILE);
  system(szCommand);
  stat(TMPFILE,&stFile);
 
  if (stFile.st_size !=0)
  {
	
	bMounted=TRUE;
  }
  #endif
 #if 0 
  bMounted=Mounted(SD_MOUNT_POINT);
  
  if(bMounted==FALSE)
  {
      memset(szDevFilePath,'\0',128);
      snprintf(szDevFilePath,128,"/dev/%s",szSDDevName);
      memset(szCommand,'\0',128);
      snprintf(szCommand,128,"/bin/mount -t vfat  %s %s > %s 2>&1",szDevFilePath,SD_MOUNT_POINT,TMPFILE);
      system(szCommand);
      stat(TMPFILE,&stFile);
      bNewMount=TRUE;
      if (stFile.st_size !=0)
      {	
	    printf("<status>ERROR</status>\n");
	    printf("<message>FS_MOUNT_ERROR</message>\n");
	    goto exit;
      }
      else
      {
	bMounted=TRUE;
      }

   }
   #endif

  if(is_record_sd(AUTOFS_SD_MOUNT_POINT)==FALSE)
  {
     printf("<status>ERROR</status>\n");
     printf("<message>IS_NOT_RECORD_SD</message>\n");
     goto exit;
      
  }
 // if(bMounted==TRUE)
  //{
	//printf("[vrec](%d)\n",__LINE__); 
	sort_recordfile(AUTOFS_SD_MOUNT_POINT,&iRecordNum);
	//printf("[vrec](%d)\n",__LINE__); 

  
	if(iRecordNum>0)
	{
	    //The following just is for searching the  video clip on the video clips with the continue trigger time.
	    //TODO: Handle the case  if the video clips is not on the  continue trigger time.
	    fDiffStart=difftime(tm_start,str_to_time(ppTVideoClip[0]->szTriggerTime));
	    fDiffEnd=difftime(tm_end,str_to_time(ppTVideoClip[iRecordNum-1]->szTriggerTime));
	    //printf("[vrecsearch][%d]fDiffStart=%f,fDiffEnd=%f\n",__LINE__,fDiffStart,fDiffEnd);
	    if(fDiffStart<0)
	    {
	      if(  ((0-fDiffStart) >= 60)  && ( (0-fDiffStart) < (60*60))) //between two videoclips' timestamp is >= 1 min and <= 1 hour
	      {
		   dwRecordLen=60;
	      }
	      else  if (( (0-fDiffStart) >=  (60*60) )&& ( (0-fDiffStart) < (60*60*24) ) ) //between two videoclips' timestamp is >= 1 hour and <= 1 day
	      {
		  dwRecordLen=60*60;
	      }
              else if (( (0-fDiffStart) >= (60*60*24) ) && ( (0-fDiffStart) < (60*60*24*365) ) ) //between two videoclips' timestamp is >= 1 day and <= 1 year
	      {
		   dwRecordLen=60*60*24;
	      }
	      else 
	      {
		  // dwRecordLen=60*60*24; 
                  dwRecordLen=60*60*24*365;
	      }
	      
	      dwDiffStartNum=(unsigned long)((double)(0-fDiffStart)/(double)dwRecordLen);
	    }
	    else
	    {
	      if((fDiffStart >= 60) && (fDiffStart < (60*60))) //between two videoclips' timestamp is >= 1 min and <= 1 hour
	      {
		   dwRecordLen=60;
	      }
	      else  if ((fDiffStart >=  (60*60) )&& ( fDiffStart < (60*60*24) ) ) //between two videoclips' timestamp is >= 1 hour and <= 1 day
	      {
		  dwRecordLen=60*60;
	      }
              else if (( (0-fDiffStart) >= (60*60*24) ) && ( (0-fDiffStart) < (60*60*24*365) ) ) //between two videoclips' timestamp is >= 1 day and <= 1 year
	      {
		   dwRecordLen=60*60*24;
	      }
	      else 
	      {
		  // dwRecordLen=60*60*24;
                  dwRecordLen=60*60*24*365;
	      }
	      
	    
	      dwDiffStartNum=(unsigned long)((double)(fDiffStart)/(double)dwRecordLen);
	    }
	    //printf("[%d]dwRecordLen=%d\n",__LINE__,dwRecordLen);
	    if(fDiffEnd<0)
	    {
	      if(  ((0-fDiffEnd) >= 60)  && ( (0-fDiffEnd) < (60*60))) //between two videoclips' timestamp is >= 1 min and <= 1 hour
	      {
		   dwRecordLen=60;
	      }
	      else  if (( (0-fDiffEnd) >=  (60*60) )&& ( (0-fDiffEnd) < (60*60*24) ) ) //between two videoclips' timestamp is >= 1 hour and <= 1 day
	      {
		  dwRecordLen=60*60;
	      }
              else if (( (0-fDiffEnd) >= (60*60*24) ) && ( (0-fDiffEnd) < (60*60*24*365) ) ) //between two videoclips' timestamp is >= 1 day and <= 1 year
	      {
		   dwRecordLen=60*60*24;
	      }
	      else 
	      {
		   //dwRecordLen=60*60*24;
                   dwRecordLen=60*60*24*365; 
	      }
	      dwDiffEndNum=(unsigned long)((double)(0-fDiffEnd)/(double)dwRecordLen);
	      
	    }
	    else
	    {
	      
	      if((fDiffEnd >= 60) && (fDiffEnd < (60*60))) //between two videoclips' timestamp is >= 1 min and <= 1 hour
	      {
		   dwRecordLen=60;
	      }
	      else  if ((fDiffEnd >=  (60*60) )&& ( fDiffEnd < (60*60*24) ) ) //between two videoclips' timestamp is >= 1 hour and <= 1 day
	      {
		  dwRecordLen=60*60;
	      }
              else if (( (0-fDiffEnd) >= (60*60*24) ) && ( (0-fDiffEnd) < (60*60*24*365) ) ) //between two videoclips' timestamp is >= 1 day and <= 1 year
	      {
		   dwRecordLen=60*60*24;
	      }
	      else 
	      {
		   //dwRecordLen=60*60*24;
                   dwRecordLen=60*60*24*365;
	      }
	      dwDiffEndNum=(unsigned long)((double)fDiffEnd/(double)dwRecordLen);
	    }
	  //printf("[%d]dwRecordLen=%d\n",__LINE__,dwRecordLen);
	 // printf("[vrecsearch][%d]fDiffStart=%f,fDiffEnd=%f ppTVideoClip[0]->szTriggerTime=%s szStart=%s\n",__LINE__,fDiffStart,fDiffEnd,ppTVideoClip[0]->szTriggerTime,szStart);
	   //printf(" dwDiffEndNum=%d,dwDiffStartNum=%d,iRecordNum=%d\n", dwDiffEndNum,dwDiffStartNum,iRecordNum);
	    
	    
	  
	  
	    if(fDiffStart<0)
	    {
		iFoundStartIndex=0;
		if(fDiffEnd <0)
		{
		  
		  iFoundEndIndex=iRecordNum-1-dwDiffEndNum;
		}
		else
		{
		  iFoundEndIndex=iRecordNum-1;
		}
		 
	    
	    }
	    else if(fDiffStart>0)
	    {
		iFoundStartIndex=0+dwDiffStartNum;
		
		if(fDiffEnd <0)
		{
		  
		  iFoundEndIndex=iRecordNum-1-dwDiffEndNum;
		}
		else
		{
		  iFoundEndIndex=iRecordNum-1;
		}

	    
	    }
	    else if(fDiffStart==0)
	    {
		iFoundStartIndex=0;
		if(fDiffEnd <0)
		{
		  
		  iFoundEndIndex=iRecordNum-1-dwDiffEndNum;
		}
		else
		{
		  iFoundEndIndex=iRecordNum-1;
		}

	    
	    }
	    
	    if(CheckVrecIsRecording(AUTOFS_SD_MOUNT_POINT)==TRUE)
	    {
	      bSkipTheLastVideoClip=TRUE;
		
	    }
	    else
	    {
	     usleep(10);
	      if(CheckVrecIsRecording(AUTOFS_SD_MOUNT_POINT)==TRUE)
	      {
		bSkipTheLastVideoClip=TRUE;
	      }
	    }
	//printf("jeff iFoundStartIndex=%d,iFoundEndIndex=%d\n", iFoundStartIndex,iFoundEndIndex);
	    if((iFoundEndIndex-iFoundStartIndex+1)>0)
	    {
	      for(i=iFoundStartIndex;i<=iFoundEndIndex;i++)
	      {
		
		   // printf("(%d)\n",__LINE__);
		    if(bSkipTheLastVideoClip==TRUE)
		    {
		      if(has_none_codec(ppTVideoClip[i]-> szFileName) || str_to_time(ppTVideoClip[i]->szTriggerTime)>tm_end || str_to_time(ppTVideoClip[i]->szTriggerTime) < tm_start||i==(iRecordNum-1))
		      {
			
			iCount+=1;
			continue;
		      }  
		    }
		    else
		    {
		      if(has_none_codec(ppTVideoClip[i]-> szFileName) || str_to_time(ppTVideoClip[i]->szTriggerTime)>tm_end || str_to_time(ppTVideoClip[i]->szTriggerTime) < tm_start)
		      {
			
			iCount+=1;
			continue;
		      }  
		    
		    }
		    printf("<videoclip>\n");
		    printf("<video_codec>%s</video_codec>\n",ppTVideoClip[i]->szVideoCodec);
		    printf("<audio_codec>%s</audio_codec>\n",ppTVideoClip[i]->szAudioCodec);
		    printf("<trigger_time>%s</trigger_time>\n",ppTVideoClip[i]->szTriggerTime);
		    printf("<file_name>%s</file_name>\n",ppTVideoClip[i]-> szFileName);    
		    printf("</videoclip>\n");
		    
	      }
	      
	      
		  
	    }
	    if(((iFoundEndIndex-iFoundStartIndex+1)-iCount)>0)
	    {
	      printf("<total_num>%d</total_num>\n",((iFoundEndIndex-iFoundStartIndex+1)-iCount));
	    }
	    else
	    {
	      printf("<total_num>0</total_num>\n");
	    }
	      printf("<dir_url>%s</dir_url>\n",RECORD_WEB_DIR);
	    
	 
	    
	}
	else
	{
	    printf("<total_num>0</total_num>\n");
	    printf("<dir_url>%s</dir_url>\n",RECORD_WEB_DIR);
	
	}
	if(ppTVideoClip!=NULL)
	{
	      for(i=0;i<iRecordNum;i++)
	      {
		  if(ppTVideoClip[i]!=NULL)
		  {
		      free(ppTVideoClip[i]);
		      ppTVideoClip[i]=NULL;
		  }
	      }
	      free(ppTVideoClip);
	      ppTVideoClip=NULL;
	}
   // }
    //if(bNewMount==TRUE && bMounted==TRUE)
    //{
      //if(UmountFS(SD_MOUNT_POINT)!=S_OK)
      //{
	//    printf("<status>ERROR</status>\n");
	  //  printf("<message>FS_UMOUNT_ERROR</message>\n");
	   // goto exit;
      //}

    //}
    
    printf("<status>OK</status>\n");
    printf("<message>NO_ERROR</message>\n");
  
  
    

exit:
  printf("</root>\n");
  return 0;
}
