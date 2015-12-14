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
#include <mntent.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h> 
#include <ctype.h>
#include "dbgdefs.h"
#include "errordef.h"
#include "typedef.h"

#include "xmlmgr.h"
#include "xmlwrapper.h"

#define SD_MOUNT_POINT "/usr/share/www/html/vrecord/videoclips"
#define AUTOFS_SD_MOUNT_POINT     "/tmp/vrecord/videoclips"
//#define TMPFILE "/tmp/tmpfile.vrecord"
BOOLEAN Mounted(const char *szMountPoint);
BOOLEAN CheckVrecIsRecording(const char *szMountPoint);
BOOLEAN SDCardHasError(void);
SCODE UmountFS(const char *szRecordPath);

typedef struct vrecord_videoclips
{
  DWORD	 dwVideoClipTotalNum;
  DWORD	 dwCurId;
  CHAR **pszVideoClipNameList;

}TVrecClips;


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
  strcpy(szTmpfileName,"/tmp/vrecord-XXXXXX"); 
 // printf("[sdmgr](%d)\n",__LINE__);
  umask(0077); // ensure exclusive access on buggy implementations of mkstemp
  fd = mkstemp(szTmpfileName);
  //printf("[sdmgr](%d)szTmpfileName=%s\n",__LINE__,szTmpfileName);
  close(fd);
  
  return S_OK;
 
}

void Vrecord_SetVideoClipsTotalNum(HANDLE hObject, const CHAR *szTotalNum, SDWORD len)
{
  TVrecClips *pTVrecClips = (TVrecClips *) hObject;
  DWORD   dwVideoClipsTotalNum=0;
  DWORD	dwCurId=0;
	
  dwVideoClipsTotalNum = atoi(szTotalNum);
  if (dwVideoClipsTotalNum< 0) {
    pTVrecClips->dwVideoClipTotalNum = 0;
  } else {
    pTVrecClips->dwVideoClipTotalNum = dwVideoClipsTotalNum;
  }
  
   pTVrecClips->pszVideoClipNameList=(CHAR **)malloc(sizeof(CHAR*)*pTVrecClips->dwVideoClipTotalNum);
   for (dwCurId = 0; dwCurId < pTVrecClips->dwVideoClipTotalNum; dwCurId++) 
   {
		 pTVrecClips->pszVideoClipNameList[dwCurId]=NULL;
   }
  
  return;

}
SCODE Vrecord_SetVideoClipIndex(HANDLE hObject, const CHAR *szIDTag, const CHAR** atts)
{
    TVrecClips *pTVrecClips = (TVrecClips *) hObject;
    DWORD  dwCurId=0;

    if (atts) {
        if (strcmp(atts[0], "id") != 0) {
            
            return IGNORE_CHILD_CONFIG;            
        }
        dwCurId = strtoul(atts[1], (char**)NULL, 10);
        if (dwCurId < 0 || dwCurId >= pTVrecClips->dwVideoClipTotalNum)
        	return IGNORE_CHILD_CONFIG;
        pTVrecClips->dwCurId = dwCurId;
        return S_OK;
    }
  
    return IGNORE_CHILD_CONFIG;
}


void Vrecord_SetVideoClipFileName(HANDLE hObject, const CHAR *szFileName, SDWORD len)
{
    TVrecClips *pTVrecClips = (TVrecClips *) hObject;
    CHAR szName[len+1];
    DWORD dwCurId = pTVrecClips->dwCurId;

    snprintf(szName, len+1, "%s", szFileName);
    pTVrecClips->pszVideoClipNameList[dwCurId] = strdup(szName);
	
}


static int fs_proc_check(const char *fs_name)
{
	FILE	*f;
	char	buf[80], *cp, *t;

	f = fopen("/proc/filesystems", "r");
	if (!f)
		return (0);
	while (!feof(f)) {
		if (!fgets(buf, sizeof(buf), f))
			break;
		cp = buf;
		if (!isspace(*cp)) {
			while (*cp && !isspace(*cp))
				cp++;
		}
		while (*cp && isspace(*cp))
			cp++;
		if ((t = strchr(cp, '\n')) != NULL)
			*t = 0;
		if ((t = strchr(cp, '\t')) != NULL)
			*t = 0;
		if ((t = strchr(cp, ' ')) != NULL)
			*t = 0;
		if (!strcmp(fs_name, cp)) {
			fclose(f);
			return (1);
		}
	}
	fclose(f);
	return (0);
}
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=vrec_status
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=on
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=off
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=sd_status
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=format
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=enable_cyclic_storage
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=disable_cyclic_storage
//http://172.17.255.109/cgi-bin/admin/vrecord.cgi?command=search&start=20101117130000&end=20111230180000
int main ( int argc, char *argv[] )
{
  
  char szQueryStr[128];
  char *szCmd=NULL;
  char *szStart=NULL;
  char *szEnd=NULL;
  char *szDelimiter1="&";
  char *szDelimiter2="=";
  char  *szTemp1=NULL;
  char  *szTemp2=NULL;
  char  *szTemp3=NULL;
//  char  szCommand[128];
  char  szSearchStr[128];
  BOOLEAN bISRecording=FALSE;
   
 // printf("[vrecord.cgi](%d)\n",__LINE__);
  if (getenv("QUERY_STRING")==NULL)
  { 
    printf("Content-type: text/xml\r\n");
    printf("Cache-Control: no-cache\r\n\n"); 
    printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
    printf("<root>\n");
    printf("<status>Error</status>\n");
    printf("<message>CGI_QUERY_STRING_IS_NULL</message>\n");
    printf("</root>\n");
  }
  else
  {	
     memset(szQueryStr,'\0',128);
     strncpy(szQueryStr,getenv("QUERY_STRING"),strlen(getenv("QUERY_STRING")));
    // printf("szQueryStr=%s\n",szQueryStr);
   
     if(strncmp(szQueryStr,"command=search",strlen("command=search"))==0)
     {
          szTemp1= strtok (szQueryStr,szDelimiter1);
	  szTemp2 = strtok (NULL, szDelimiter1);
	  szTemp3 = strtok (NULL, szDelimiter1);
	  
	  strtok(szTemp1,szDelimiter2);
	  szCmd=strtok(NULL,szDelimiter2);
	  
	  strtok(szTemp2,szDelimiter2);
	  szStart=strtok(NULL,szDelimiter2);
	  
	   strtok(szTemp3,szDelimiter2);
	  szEnd=strtok(NULL,szDelimiter2);
	  
	//printf("szCmd=%s szStart=%s szEnd=%s\n",szCmd,szStart,szEnd);
     
     }
     else
     {
      szTemp1= strtok (szQueryStr,szDelimiter1);
      //szTemp2 = strtok (NULL, szDelimiter1);
      strtok(szTemp1,szDelimiter2);
      szCmd=strtok(NULL,szDelimiter2);
	//strtok(szTemp2,szDelimiter2);
	//szPara=strtok(NULL,szDelimiter2);
	//printf("szCmd=%s\n",szCmd);
     }
     if(fs_proc_check("autofs")!=1)
     {
          printf("Content-type: text/xml\r\n");
	  printf("Cache-Control: no-cache\r\n\n"); 
	  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	  printf("<root>\n");
	  printf("<status>Error</status>\n");
	  printf("<message>Can_Not_Record_Because_Of_autofs_Not_Supported_In_System</message>\n");
	  printf("</root>\n");
	  goto exit;
     }
     
     if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE && CheckVrecIsRecording(AUTOFS_SD_MOUNT_POINT)==TRUE)
     {
	  bISRecording=TRUE;
	
     }
     else if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE && CheckVrecIsRecording(AUTOFS_SD_MOUNT_POINT)==FALSE)
     {
	  usleep(10);
	  if(CheckVrecIsRecording(SD_MOUNT_POINT)==TRUE)
	  {
	    bISRecording=TRUE;
	  }
     
     }
     //printf("[vrecord.cgi](%d)\n",__LINE__);
    // printf("bISRecording=%d\n",bISRecording);
     //printf("Mounted(AUTOFS_SD_MOUNT_POINT)=%d\n",Mounted(AUTOFS_SD_MOUNT_POINT));
     if(strncmp(szCmd,"vrec_status",strlen(szCmd))==0)
     {
       // system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	//if(SDCardHasError()==TRUE)
	//{
	  //  goto check_sd_status;
	//}
       
	if(bISRecording==TRUE)
	{
		printf("Content-type: text/xml\r\n");
		printf("Cache-Control: no-cache\r\n\n"); 
		printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
		printf("<root>\n");
		printf("<status>OK</status>\n");
		printf("<message>VREC_RECORDING</message>\n");
		printf("</root>\n");
	}
	else
	{	printf("Content-type: text/xml\r\n");
		printf("Cache-Control: no-cache\r\n\n"); 
		printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
		printf("<root>\n");
		printf("<status>OK</status>\n");
		printf("<message>VREC_NOT_RECORDING</message>\n");
		printf("</root>\n");
	
	
	}
	//system("/usr/sbin/namedsem post >  /dev/null 2>&1");
     }
     else if(strncmp(szCmd,"mount",strlen(szCmd))==0)
     {	
        if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE)
	{
	  printf("Content-type: text/xml\r\n");
	  printf("Cache-Control: no-cache\r\n\n"); 
	  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	  printf("<root>\n");
	  printf("</root>\n");
	}
	else
	{
	    system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","command=mount&filesystem=vfat",1);
	    system("/usr/share/www/cgi-bin/admin/sdmanage.cgi > /dev/null 2>&1");
	  
	 system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	}
	 
     }
     else  if(strncmp(szCmd,"umount",strlen(szCmd))==0)
     {	
        if(Mounted(AUTOFS_SD_MOUNT_POINT)==FALSE)
	{
	  printf("Content-type: text/xml\r\n");
	  printf("Cache-Control: no-cache\r\n\n"); 
	  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	  printf("<root>\n");
	  printf("</root>\n");
	}
	else
	{
	    system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","command=umount&filesystem=vfat",1);
	    system("/usr/share/www/cgi-bin/admin/sdmanage.cgi > /dev/null 2>&1");
	    
	system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	}
	
     }
     else  if(strncmp(szCmd,"fsck",strlen(szCmd))==0)
     {	
	  
       system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
       if(bISRecording==TRUE)
       {
	   
	    
	   setenv("REQUEST_METHOD","GET",1);
	   setenv("QUERY_STRING","off",1);
	   system("/usr/share/www/cgi-bin/admin/control_record.lua >  /dev/null 2>&1");
	 
       }
       setenv("REQUEST_METHOD","GET",1);
       setenv("QUERY_STRING","command=fsck&filesystem=vfat",1);
       system("/usr/share/www/cgi-bin/admin/sdmanage.cgi");
       system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	
	
     }
     else if(strncmp(szCmd,"on",strlen(szCmd))==0)
     {
        //1. check sd card status
	//2. Firstly check  SD_MOUNT_POINT is mounted or not because there will multiple clients executing the cgi. 
    
  
	if(SDCardHasError()==TRUE)
	{
	  goto check_sd_status;
	}
    
    
    
	if(bISRecording==TRUE)
	{
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	      printf("</root>\n");
	
	
	}
	else
	{
	   
	    
	   // if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE)
	    //{
		//system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
		//UmountFS(AUTOFS_SD_MOUNT_POINT);
	      //system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	     
	    //}
	      system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","on",1);
	    //printf("QUERY_STRING=%s",getenv("QUERY_STRING"));
	    system("/usr/share/www/cgi-bin/admin/control_record.lua");
	    system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	  
	}  
	
	
    }
    else if(strncmp(szCmd,"off",strlen(szCmd))==0)
    {
	 // if(SDCardHasError()==TRUE)
	  //{
	    //goto check_sd_status;
	  //}
	 
	  if(bISRecording==TRUE)
	  {
	  
	    //system("export QUERY_STRING=\"off\"");
	    //system("export REQUEST_METHOD=\"GET\"");
	    system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","off",1);
	    //printf("QUERY_STRING=%s",getenv("QUERY_STRING"));
	    system("/usr/share/www/cgi-bin/admin/control_record.lua");
	    //printf("[vrecord.cgi](%d)\n",__LINE__);
	    if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE)
	    {
	      //printf("[vrecord.cgi](%d)\n",__LINE__);
	      UmountFS(AUTOFS_SD_MOUNT_POINT);
	      //printf("[vrecord.cgi](%d)\n",__LINE__);
	    }
	    //printf("[vrecord.cgi](%d)\n",__LINE__);
	    system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	  
	  }
	  else
	  { 
		printf("Content-type: text/xml\r\n");
		printf("Cache-Control: no-cache\r\n\n"); 
		printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
		printf("<root>\n");
		printf("</root>\n");
	  
	  }
	    
	  
     }
     else if(strncmp(szCmd,"search",strlen(szCmd))==0)
     {
	//system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	if(SDCardHasError()==TRUE)
	{
	    goto check_sd_status;
	}

	
	setenv("REQUEST_METHOD","GET",1);
	memset(szSearchStr,'\0',128);
	snprintf(szSearchStr,128,"start=%s&end=%s",szStart,szEnd);
	setenv("QUERY_STRING",szSearchStr,1);
        system("/usr/share/www/cgi-bin/admin/vrecsearch.cgi");
	
 
	//system("/usr/sbin/namedsem post >  /dev/null 2>&1");
     }
     else if(strncmp(szCmd,"format",strlen(szCmd))==0)
     {
        system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	 if(bISRecording==TRUE)
	 {
	   
	    //system("export QUERY_STRING=\"off\"");
	    //system("export REQUEST_METHOD=\"GET\"");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","off",1);
	    //printf("QUERY_STRING=%s",getenv("QUERY_STRING"));
	    system("/usr/share/www/cgi-bin/admin/control_record.lua >  /dev/null 2>&1");
	  //  system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	 }
       
	//system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	setenv("REQUEST_METHOD","GET",1);
	setenv("QUERY_STRING","command=format&filesystem=vfat",1);
        system("/usr/share/www/cgi-bin/admin/sdmanage.cgi");
	if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE)
	{
	  UmountFS(AUTOFS_SD_MOUNT_POINT);
	}

	system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	
	
	
     }
     else if(strncmp(szCmd,"enable_cyclic_storage",strlen(szCmd))==0||strncmp(szCmd,"disable_cyclic_storage",strlen(szCmd))==0)
     {
	 system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	 if(bISRecording==TRUE)
	 {
	   
	    //system("export QUERY_STRING=\"off\"");
	    //system("export REQUEST_METHOD=\"GET\"");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","off",1);
	    //printf("QUERY_STRING=%s",getenv("QUERY_STRING"));
	    system("/usr/share/www/cgi-bin/admin/control_record.lua > /dev/null 2>&1");
	   // system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	 }
	 
	//system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	setenv("REQUEST_METHOD","GET",1);
	if(strncmp(szCmd,"enable_cyclic_storage",strlen(szCmd))==0)
	{
	  setenv("QUERY_STRING","command=enable_cyclic_storage&filesystem=vfat",1);
	}
	else
	{
	  setenv("QUERY_STRING","command=disable_cyclic_storage&filesystem=vfat",1);
	}
	system("/usr/share/www/cgi-bin/admin/sdmanage.cgi");
	//system("/usr/sbin/namedsem post >  /dev/null 2>&1");
     
	
	
	 if(bISRecording==TRUE)
	 {
	  //  if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE)
	    //{
	      // system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
		//UmountFS(SD_MOUNT_POINT);
	      //system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	    //}
	   // system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	    //system("export QUERY_STRING=\"off\"");
	    //system("export REQUEST_METHOD=\"GET\"");
	    setenv("REQUEST_METHOD","GET",1);
	    setenv("QUERY_STRING","on",1);
	    //printf("QUERY_STRING=%s",getenv("QUERY_STRING"));
	    system("/usr/share/www/cgi-bin/admin/control_record.lua > /dev/null 2>&1");
	    
	    
	 
	 }
	 
	 system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	
     }
     else if(strncmp(szCmd,"remove",strlen(szCmd))==0)
     {
       
        /*
	<?xml version="1.0" standalone="yes"?>
	<root>
	  <total_num>4</total_num>
	  <videoclip id="0">
		<file_name>Record_20101229144301_AAC4_MJPEG.avi</file_name>
	  </videoclip>
	  <videoclip id="1">
		<file_name>Record_20101229144000_AAC4_MJPEG.avi</file_name>
	  </videoclip>
	    <videoclip id="2">
		<file_name>Record_20101229144101_AAC4_MJPEG.avi</file_name>
	  </videoclip>
	  <videoclip id="3">
		<file_name>Record_20101229144201_AAC4_MJPEG.avi</file_name>
	  </videoclip>
	</root>
	
	typedef struct vrecord_videoclips
	{
	  DWORD	 dwVideoClipTotalNum;
	  DWORD	 dwCurId;
	  CHAR **pszVideoClipNameList;

	}TVrecClips;
	
	How to test remove function:
	1.Create testremove.xml containing the removing list of videoclips.
	2.export CONTENT_LENGTH=[filesize_of_testremove.xml]
	3.export QUERY_STRING="command=remove"
	4.cat testremove.xml  | /usr/share/www/cgi-bin/admin/vrecord.cgi
	*/
	BOOLEAN bHasErr=FALSE;
	DWORD dwCntLength = 0;
	char *szRemoveListStr=NULL;
	HANDLE hXmlWrapperObj;
	TXmlWrapperInitOptions tXmlWrapperInitOptions;
	TVrecClips *pTVrecClips=NULL;
	TXmlWrapperTreeMap ptCfgTreeMap[] = 
	{
	  {"root/total_num", NULL, &Vrecord_SetVideoClipsTotalNum, NULL}, 
	  {"root/videoclip", &Vrecord_SetVideoClipIndex, NULL, NULL},
	  {"root/videoclip/file_name", NULL, &Vrecord_SetVideoClipFileName, NULL},
	  {NULL, NULL, NULL}
	};
	DWORD dwIndex=0;
	CHAR szFilePath[4096];
	//#if 0
	FILE *fpForDumpMsg=NULL;
	
	CHAR szDumpFileName[1024];
	memset(szDumpFileName,0x0,sizeof(szDumpFileName));
	
	create_tmpfile(szDumpFileName);
	fpForDumpMsg=fopen(szDumpFileName,"wb");
	//#endif
	
	//printf("[vrecord.cgi](%d)\n",__LINE__);
	memset(szFilePath,'\0',sizeof(szFilePath));
	memset(&tXmlWrapperInitOptions,0x0,sizeof(TXmlWrapperInitOptions));
	pTVrecClips=(TVrecClips *)malloc(sizeof(TVrecClips));
	//printf("[vrecord.cgi](%d)\n",__LINE__);
	if(pTVrecClips==NULL)
	{
	    printf("Content-type: text/xml\r\n");
	    printf("Cache-Control: no-cache\r\n\n"); 
	    printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	    printf("<root>\n");
	    printf("<status>Error</status>\n");
	    printf("<message>REMOVE_SD_FILES_ERROR_0</message>\n");
	    printf("</root>\n");
	     bHasErr=TRUE;
	    goto exit;
	
	}
	//printf("[vrecord.cgi](%d)\n",__LINE__);
	if(SDCardHasError()==TRUE)
	{
	  goto check_sd_status;
	}
	//printf("[vrecord.cgi](%d)\n",__LINE__);
	dwCntLength = atoi(getenv("CONTENT_LENGTH"));
	szRemoveListStr=(char *)malloc(dwCntLength);
	if(szRemoveListStr==NULL)
	{
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	       printf("<status>Error</status>\n");
	      printf("<message>REMOVE_SD_FILES_ERROR_1</message>\n");
	      printf("</root>\n");
	       bHasErr=TRUE;
	      goto exit;
	}
	//printf("[vrecord.cgi](%d)dwCntLength=%d\n",__LINE__,dwCntLength);
	memset(szRemoveListStr,'\0',dwCntLength);
	  
	
	fread(szRemoveListStr, dwCntLength, 1, stdin);
	//usleep(100);
	//#if 0
	//This is very strange. Must write the  POST msg to the file here.
	//After calling the cgi from the web, could remove the video clips successfully.  
	if(fpForDumpMsg!=NULL)
	{
	  fwrite(szRemoveListStr, dwCntLength, 1, fpForDumpMsg);
	
	}
	//#endif
	//printf("[vrecord.cgi](%d)\n",__LINE__);
	//printf("szRemoveListStr=%s\n",szRemoveListStr);
	//printf("[vrecord.cgi](%d)\n",__LINE__);
	
	tXmlWrapperInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapperObj, &tXmlWrapperInitOptions) != S_OK) {
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	      printf("<status>Error</status>\n");
	      printf("<message>REMOVE_SD_FILES_ERROR_2</message>\n");
	      printf("</root>\n");
	       bHasErr=TRUE;
	      goto exit_1;
	}
	
	if (XmlWrapper_SetHandler(hXmlWrapperObj, ptCfgTreeMap,NULL) != S_OK) {
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	      printf("<status>Error</status>\n");
	      printf("<message>REMOVE_SD_FILES_ERROR_3</message>\n");
	      printf("</root>\n");
	       bHasErr=TRUE;
	      goto exit_1;
	}
	
	if (XmlWrapper_ReadBuf_UsrDefFunc((BYTE *)szRemoveListStr, hXmlWrapperObj, pTVrecClips) != S_OK) {
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	      printf("<status>Error</status>\n");
	      printf("<message>REMOVE_SD_FILES_ERROR_4</message>\n");
	      printf("</root>\n");
	       bHasErr=TRUE;
	      goto exit_1;
	}
	
	if (XmlWrapper_Release(&hXmlWrapperObj) != S_OK) {
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	      printf("<status>Error</status>\n");
	      printf("<message>REMOVE_SD_FILES_ERROR_5</message>\n");
	      printf("</root>\n");
	       bHasErr=TRUE;
	      goto exit_1;
	}
	system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	for(dwIndex=0;dwIndex< pTVrecClips->dwVideoClipTotalNum;dwIndex++)
	{
	
	    memset(szFilePath,'\0',sizeof(szFilePath));
	    snprintf(szFilePath,sizeof(szFilePath),"%s/%s",AUTOFS_SD_MOUNT_POINT,pTVrecClips->pszVideoClipNameList[dwIndex]);
	    
	    system("sync");
	    if( unlink(szFilePath)!=0)
	    {
	     //printf("[vrecord.cgi](%d)Calling unlink() to remove %s  is failed!(%s)\n",__LINE__,szFilePath,strerror(errno));
	      printf("Content-type: text/xml\r\n");
	      printf("Cache-Control: no-cache\r\n\n"); 
	      printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	      printf("<root>\n");
	      printf("<status>Error</status>\n");
	      printf("<message>Can_Not_Remove_%s</message>\n",pTVrecClips->pszVideoClipNameList[dwIndex]);
	      printf("</root>\n");
	      bHasErr=TRUE;
	      break;
	    }
	     system("sync");
	
	
	}
	system("/usr/sbin/namedsem post >  /dev/null 2>&1");
	 
	
exit_1:	
//#if 0
	if(fpForDumpMsg!=NULL)
	{
	  fclose(fpForDumpMsg);
	  fpForDumpMsg=NULL;
	}  
	unlink(szDumpFileName);
//#endif	
	
	if(szRemoveListStr!=NULL)
	{
	  free(szRemoveListStr);
	  szRemoveListStr=NULL;
	} 
	if(pTVrecClips != NULL)
	{
	     for(dwIndex=0;dwIndex<pTVrecClips->dwVideoClipTotalNum;dwIndex++)
	     {
		if(pTVrecClips->pszVideoClipNameList[dwIndex]!=NULL)
		{
		    free(pTVrecClips->pszVideoClipNameList[dwIndex]);
		    pTVrecClips->pszVideoClipNameList[dwIndex]=NULL;
		} 
		  
	     }  
	    free(pTVrecClips);
	    pTVrecClips=NULL;
	}  
	
	if(bHasErr==FALSE)
	{  
	  printf("Content-type: text/xml\r\n");
	  printf("Cache-Control: no-cache\r\n\n"); 
	  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
	  printf("<root>\n");
	  printf("<status>OK</status>\n");
	  printf("<message>NO_ERROR</message>\n");
	  printf("</root>\n");
	}
     }  
     else if(strncmp(szCmd,"sd_status",strlen(szCmd))==0)
     {
check_sd_status:

	//system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
	setenv("REQUEST_METHOD","GET",1);
	setenv("QUERY_STRING","command=status&filesystem=vfat",1);
        system("/usr/share/www/cgi-bin/admin/sdmanage.cgi");
	system("sync");
	//system("/usr/sbin/namedsem post >  /dev/null 2>&1");
     
     
     }	    
     
     
  }
exit:
 
  return 0;
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


}
BOOLEAN SDCardHasError(void)
{
  BOOLEAN bHasError=FALSE;
  char  szCommand[128];
  struct stat stFile;
  char  szTmpFileName1[64];
  char  szTmpFileName2[64];
  
  memset(szTmpFileName1,'\0',64);
  memset(szTmpFileName2,'\0',64);
  memset(szCommand,'\0',128);
  
  //system("/usr/sbin/namedsem wait >  /dev/null 2>&1");
  
  setenv("REQUEST_METHOD","GET",1);
  setenv("QUERY_STRING","command=status&filesystem=vfat",1);
  create_tmpfile(szTmpFileName1);
  snprintf(szCommand,128,"/usr/share/www/cgi-bin/admin/sdmanage.cgi > %s",szTmpFileName1);
  system(szCommand);
   
   
  memset(szCommand,'\0',128);
  create_tmpfile(szTmpFileName2);
  snprintf(szCommand,128," grep -r \"<status>Error</status>\" %s > %s",szTmpFileName1,szTmpFileName2);
  system(szCommand);
  stat(szTmpFileName2,&stFile);
  if (stFile.st_size !=0)
  {
    bHasError=TRUE;
  }
 unlink(szTmpFileName1);
 unlink(szTmpFileName2);
 
 //system("/usr/sbin/namedsem post >  /dev/null 2>&1");
  
  
  
  return bHasError;

}


SCODE UmountFS(const char *szRecordPath)
{
  
  system("sync;sync;sync");
  if(umount(szRecordPath)!=0)
  {
      
      //printf("[vrecod.cgi](%d)Calling umoumt() to umount szMountPoint=%s  is failed!(%s)\n",__LINE__,szRecordPath,strerror(errno));
      
    
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
#endif
  return S_OK;
}
