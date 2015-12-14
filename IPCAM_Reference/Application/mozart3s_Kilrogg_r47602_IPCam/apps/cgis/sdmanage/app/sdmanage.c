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
#include <sys/stat.h> 
#include "sdmanage.h"
#include "xmlmgr.h"


/*

SD Card management

(1)Foramt: 

Function: Format
CGI method: Get
Request:


command=format&filesystem=vfat


Respond:

<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>
      <filesystem>vfat</filesystem>
      <total_size></total_size>
      <used_size></used_size>
      <free_size></free_size>
      <used_percentage></used_percentage>
      <status></status> <!--(1)Error and (2)OK-->
      <message></message>
</root>

(2)status: 

Function: status
CGI method: Get
Request:


command=status&filesystem=vfat 


Respond:

<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>
      <filesystem>vfat</filesystem>
      <total_size></total_size>
      <used_size></used_size>
      <free_size></free_size>
      <used_percentage></used_percentage>
      <status></status> <!--(1)Error and (2)OK-->
      <message></message>
</root>



(4)enable cyclic storage:

Function: enable cyclic storage 
CGI method: Get
Request:


command=enable_cyclic_storage&filesystem=vfat 


Respond:

<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>
      <filesystem>vfat</filesystem>
      <total_size></total_size>
      <used_size></used_size>
      <free_size></free_size>
      <used_percentage></used_percentage>
      <status></status> <!--(1)Error and (2)OK-->
      <message></message>
</root>

(5)disable cyclic storage:

Function: disable cyclic storage 
CGI method: Get
Request:


command=disable_cyclic_storage&filesystem=vfat 


Respond:

<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>
      <filesystem>vfat</filesystem>
      <total_size></total_size>
      <used_size></used_size>
      <free_size></free_size>
      <used_percentage></used_percentage>
      <status></status> <!--(1)Error and (2)OK-->
      <message></message>
</root>

*/
#define VREC_SIGNATURE ".VREC_SIGNATURE"
#define VREC_CONF "/etc/conf.d/vrec_conf.xml"
#define FORMAT_CMD "format"
#define STATUS_CMD "status"
#define FSCK_CMD "fsck"
#define ENABLE_CYCLIC_STORAGE_CMD "enable_cyclic_storage"
#define DISABLE_CYCLIC_STORAGE_CMD "disable_cyclic_storage"
#define MOUNT_CMD "mount"
#define UMOUNT_CMD "umount"

//#define SD_BLK_DEV_NAME "mmcblk0"

#define SD_MOUNT_POINT "/usr/share/www/html/vrecord/videoclips"
#define AUTOFS_SD_MOUNT_POINT     "/tmp/vrecord/videoclips"
// /usr/share/www/html/videoclips
#define SD_ERR_NOERR 0x0
#define SD_ERR_DETATCHED 0x1
#define SD_ERR_FULL 0x2
#define SD_ERR_WP 0x3
#define SD_ERR_FS_UNDEFINED 0x4
#define SD_ERR_FS_MOUNT_POINT 0x5
#define SD_ERR_FS_MOUNT 0x6
#define SD_ERR_FS_UMOUNT 0x7
#define SD_ERR_QUERY_STRING_NULL 0x8
#define SD_ERR_FORMAT 0x9
#define SD_ERR_OTHER 0xff

#define UNDEFINEDFS 0xff
#define VFAFS 0x0

//#define TMPFILE "/tmp/tmpfile.sdmanage"

#define touch(x) close(open((x), O_CREAT|O_WRONLY|O_TRUNC, 0644))
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
  strcpy(szTmpfileName,"/tmp/sdmanage-XXXXXX"); 
 // printf("[sdmgr](%d)\n",__LINE__);
  umask(0077); // ensure exclusive access on buggy implementations of mkstemp
  fd = mkstemp(szTmpfileName);
  //printf("[sdmgr](%d)szTmpfileName=%s\n",__LINE__,szTmpfileName);
  close(fd);
  
  return S_OK;
 
}

static DWORD get_reserved_sectors(const char *szDevName,DWORD dwBlkEraseSize)
{
  DWORD	dwReservedSectors=0;
  char szCommand[128];
  char szTmpFileName[64];
  DWORD	 dwFatSize=0;
  DWORD	 dwOneFatTableSize=0;
  FILE * pFile=NULL;
  div_t  tdiv;
  DWORD	 dwCount=0;
  
  memset(szTmpFileName,0x0,sizeof(szTmpFileName));
  memset(szCommand,0x0,sizeof(szCommand));
  
  create_tmpfile(szTmpFileName); 
  //printf("[sdmanage](%d)\n",__LINE__);
  snprintf(szCommand,sizeof(szCommand),"/usr/sbin/mkfs.vfat    -F 32  -s 32 -v /dev/%sp1  |  grep \"FAT size\" | awk -F' ' '{ print $4 }'  >  %s  2>&1",szDevName,szTmpFileName);
   
  system(szCommand);
  //printf("[sdmanage](%d)\n",__LINE__);
  pFile=fopen(szTmpFileName,"r");
  
  if(pFile==NULL)
  {
    return 0;
  
  }
  fscanf (pFile, "%d",&dwFatSize);
  if(pFile!=NULL)
  {
    fclose(pFile);
   
	    
  }
  unlink(szTmpFileName);
	    
  dwOneFatTableSize=2*dwFatSize*512;
  tdiv=div(dwOneFatTableSize,dwBlkEraseSize);
  
  if(tdiv.rem == 0)
  {
    dwCount=tdiv.quot;
  }
  else if(tdiv.rem > 0)
  {
     dwCount=tdiv.quot+1;
  }
  else if(tdiv.rem < 0)
  {
     dwCount=tdiv.quot-1;
  }
  
  if( (dwCount*dwBlkEraseSize) == dwOneFatTableSize)
  {
    dwReservedSectors=0;
  }
  else if( (dwCount*dwBlkEraseSize) > dwOneFatTableSize)
  {
    dwReservedSectors=((dwCount*dwBlkEraseSize) - dwOneFatTableSize)/512;
  }
  else if( (dwCount*dwBlkEraseSize) < dwOneFatTableSize)
  {
     dwReservedSectors=(dwOneFatTableSize-(dwCount*dwBlkEraseSize))/512;
  }
  
  /*
  ~ # mkfs.vfat    -F 32  -s 32 -v /dev/mmcblk0p1
  mkfs.vfat 2.11 (12 Mar 2005)
  /dev/mmcblk0p1 has 4 heads and 16 sectors per track,
  logical sector size is 512,
  using 0xf8 media descriptor, with 7896960 sectors;
  file system has 2 32-bit FATs and 32 sectors per cluster.
  FAT size is 1928 sectors, and provides 246658 clusters.
  Volume ID is 4d365581, no volume label.
  ~ #
  */
  

  return dwReservedSectors;
}

//About CSD (Card specific Data) register fields, refer to P.128 of Part_1_Physical_Layer_Specification_Ver3.00_Final_090416 
static DWORD get_sd_erase_blk_size(const char *szDevName)
{
  char szCSDPath[128];
  char szSDSYSFSPath[128];
  char szCSD[128];
  char szCommand[128];
  char szTmpFileName[64];
  
  char szEraseSectorSize[4];
  FILE * pFile=NULL;
 
  DWORD dwBlkEraseSize=0;
  
  //The szDevName must be mmcblkX X=0,1, And then udevadm info -q  env -n /dev/mmcblk0
  //DEVPATH=/block/mmcblk0
  //MAJOR=179
  //MINOR=0
  //DEVTYPE=disk
  //PHYSDEVPATH=/class/mmc_host/mmc0/mmc0:0007
  //PHYSDEVBUS=mmc
  //PHYSDEVDRIVER=mmcblk
  //DEVNAME=/dev/mmcblk0
  //DEVLINKS=/dev/block/179:0

   
  
  
  memset(szTmpFileName,0x0,sizeof(szTmpFileName));
  memset(szCommand,0x0,sizeof(szCommand));
  memset(szSDSYSFSPath,0x0,sizeof( szSDSYSFSPath));
  memset(szEraseSectorSize,0x0,sizeof(szEraseSectorSize));
  memset(szCSD,0x0,sizeof(szCSD));
  memset(szCSDPath,0x0,sizeof(szCSDPath));
  
  
  
  
  
  create_tmpfile(szTmpFileName); 
  snprintf(szCommand,sizeof(szCommand),"/sbin/udevadm info -q  env -n /dev/%s |  grep \"PHYSDEVPATH=\" |  awk -F'=' '{ print $2 }'  >  %s  2>&1",szDevName,szTmpFileName);
  system(szCommand);
  pFile=fopen(szTmpFileName,"r");
  
  if(pFile==NULL)
  {
    return 0;
  
  }
  fscanf (pFile, "%s", szSDSYSFSPath);
  if(pFile!=NULL)
  {
    fclose(pFile);
  }
   unlink(szTmpFileName);
  snprintf(szCSDPath,sizeof(szCSDPath),"/sys/%s/csd",szSDSYSFSPath);
  pFile=fopen(szCSDPath,"r");
  if(pFile==NULL)
  {
    return 0;
  
  }
  fscanf (pFile, "%s", szCSD);
  if(pFile!=NULL)
  {
    fclose(pFile);
  }
  szEraseSectorSize[0]=szCSD[20];
  szEraseSectorSize[1]=szCSD[21];
  szEraseSectorSize[2]=szCSD[22];
  
 // printf("szCSD=%s szEraseSectorSize=%s szCSD[0]=%c szCSD len=%d\n",szCSD,szEraseSectorSize,szCSD[0],strlen(szCSD));
  
   dwBlkEraseSize = (((strtoul ( szEraseSectorSize,NULL,16)& 0x3f8) >> 3)+1)*512;
   
   //printf("dwBlkEraseSize=%u\n",dwBlkEraseSize);
   return dwBlkEraseSize;
}
static void  u_sleep(int usec)
{
  struct timeval tv;
  tv.tv_sec=0;
  tv.tv_usec=usec;
  
  select(0,NULL,NULL,NULL,&tv);

  return;
}

static BOOLEAN is_cyclic_storage(void)
{
  BOOLEAN bRet=FALSE;
  char	*szFileName=VREC_CONF;
  HANDLE hXmlConf;
  TXmlMgrInitOptions	tInitOpt;
  char szCyclicStorage[32];
  TXmlMgrParsingMap atParsingMap[] = {	
	{"/root/cyclic_storage", xmlmgr_set_string, szCyclicStorage},
	{NULL, 0, NULL}
  };
  
  
  memset(szCyclicStorage,'\0',32);
    
  /* initial */
  tInitOpt.dwVersion = XMLMGR_VERSION;
  if (XmlMgr_Initial(&hXmlConf, &tInitOpt) != S_OK)
  {
	fprintf(stderr, "Initial fail\n");
		
	goto exit;
  }
  /* read file */
  if (XmlMgr_ReadFile(hXmlConf, szFileName) != S_OK)
  {
	fprintf(stderr, "Open XML file %s fail\n", szFileName);
	
	goto exit;
  }


 
  XmlMgr_GetMultiConfValue(hXmlConf, atParsingMap);
  
  if(strncmp(szCyclicStorage,"1",strlen("1"))==0)
  {
    bRet=TRUE;
  }
  else  if(strncmp(szCyclicStorage,"0",strlen("0"))==0)
  {
    bRet=FALSE;
  }
  
exit:
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

static SCODE cyclic_storage(BOOLEAN enable)
{

  SCODE sRet=S_OK;
  //BOOLEAN bRecording=FALSE;
  char	*szFileName=VREC_CONF;
  HANDLE hXmlConf;
  TXmlMgrInitOptions	tInitOpt;
  char szSwitchValue[32];

  memset(szSwitchValue,'\0',sizeof(szSwitchValue));
  
  /* initial */
  tInitOpt.dwVersion = XMLMGR_VERSION;
  if (XmlMgr_Initial(&hXmlConf, &tInitOpt) != S_OK)
  {
		fprintf(stderr, "Initial fail\n");
		sRet=S_FAIL;
	goto exit;
  }
  /* read file */
  if (XmlMgr_ReadFile(hXmlConf, szFileName) != S_OK)
  {
	fprintf(stderr, "Open XML file %s fail\n", szFileName);
	sRet=S_FAIL;
	goto exit;
  }


   //3. modify  <cyclic_storage>
  if(enable==TRUE)
  {
     XmlMgr_SetConfValue(hXmlConf, "/root/cyclic_storage", "1");
  
  }
  else
  {
    XmlMgr_SetConfValue(hXmlConf, "/root/cyclic_storage", "0");
  
  }
    
   /* write back */
  if (XmlMgr_WriteFile(hXmlConf,szFileName) != S_OK)
  {
	  fprintf(stderr, "Write XML file %s fail\n",szFileName);
	  sRet=S_FAIL;
	   goto exit;			
  }

exit:
  
  if(hXmlConf!=NULL)
  {
    XmlMgr_Release(&hXmlConf);
  }
  return sRet;


}

//The idea of "vrec_storage_is_full" is copied from "busybox-1.10.4/coreutils/df.c".
static BOOLEAN storage_is_full(const char *szRecordPath)
{
  
  BOOLEAN bRet=FALSE;
  unsigned long blocks_used;
  unsigned blocks_percent_used;
  struct statfs s;
  if (statfs(szRecordPath, &s) != 0) {
      printf("Call statfs for checking the capability of  %s is failed \n",szRecordPath);
      bRet=TRUE;
      goto exit;
  }
  
  blocks_used = s.f_blocks - s.f_bfree;
  blocks_percent_used = 0;
  if (blocks_used + s.f_bavail) {
		blocks_percent_used = (blocks_used * 100ULL
						+ (blocks_used + s.f_bavail)/2
						) / (blocks_used + s.f_bavail);
  }
 // printf("Call statfs for checking the capability of  %s. Its used percentage is %3u%% \n",__LINE__,szRecordPath,blocks_percent_used);
  if( blocks_percent_used == 100)
  {
     bRet=TRUE;  
  }

exit:
  return bRet;

}
/*
printf(" %9lu %9lu %9lu %3u%% %s\n",
					kscale(s.f_blocks, s.f_bsize),
					kscale(s.f_blocks-s.f_bfree, s.f_bsize),
					kscale(s.f_bavail, s.f_bsize),
					blocks_percent_used, mount_point);

*/
static unsigned long kscale(unsigned long b, unsigned long bs)
{
	return (b * (unsigned long long) bs + 1024/2) / 1024;
}
static SCODE get_sd_capability_status(const char *szRecordPath,
				      DWORD *pdwTotalSize,
				      DWORD *pdwAvailSize, 
				      DWORD *pdwUsedSize,
				      DWORD *pdwUsedPercent)
{
    SCODE sRet=S_OK;
    struct statfs s;
    DWORD dwUsedBlocks=0;
    if (statfs(szRecordPath, &s) != 0) {
      //printf("Call statfs for checking the capability of  %s is failed \n",szRecordPath);
      sRet=S_FAIL;
     
    }
    *pdwTotalSize= kscale(s.f_blocks, s.f_bsize);
    *pdwAvailSize=kscale(s.f_bavail, s.f_bsize);
    *pdwUsedSize=kscale(s.f_blocks-s.f_bfree, s.f_bsize);
     dwUsedBlocks = s.f_blocks - s.f_bfree;
    *pdwUsedPercent = 0;
    if (dwUsedBlocks + s.f_bavail) {
		*pdwUsedPercent = (dwUsedBlocks * 100ULL
						+ (dwUsedBlocks+ s.f_bavail)/2
						) / (dwUsedBlocks + s.f_bavail);
    }
    
    
    return sRet;
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
SCODE UmountFS(const char *szRecordPath)
{
    system("sync;sync;sync");
  if(umount(szRecordPath)!=0)
  {
      
      //printf("[sdmanage.cgi](%d)Calling umoumt() to umount szMountPoint=%s  is failed!(%s)\n",__LINE__,szRecordPath,strerror(errno));
      
    
      return S_FAIL;
  }
  
 
  return S_OK;
}
SCODE MountFS(const char *szStorage,const char *szRecordPath,int iFS)
{

    
    
   char  szCommand[128];
   char szTmpFileName[64];
   struct stat stFile;
   
   memset(szTmpFileName,'\0',64);
   memset(szCommand,'\0',128);
   
   if(iFS==VFAFS)
   {
    create_tmpfile(szTmpFileName); 
    snprintf(szCommand,128,"/bin/mount -t vfat  %s  %s  >  %s  2>&1",szStorage,szRecordPath,szTmpFileName);
    system(szCommand);
    stat(szTmpFileName,&stFile);
    unlink(szTmpFileName);
    if (stFile.st_size !=0)
    {
	return S_FAIL;
    }
   }
   else
   {
   
      return S_FAIL;
   }  
 
     
  
   
    return S_OK;
 

 
}
int main ( int argc, char *argv[] )
{

  char szQueryStr[128];
  char *szCmd=NULL;
  char *szPara=NULL;
  char *szDelimiter1="&";
  char *szDelimiter2="=";
  int   status=SD_ERR_NOERR;
  char  *szTemp1=NULL;
  char  *szTemp2=NULL;
  char  szCommand[128];
  int 	iFS= UNDEFINEDFS;
  char 	szFilePath[128];
  char  szSDDevName[16];
  char 	szSDPartName[24];
  char  szSDCylinders[16];
  DWORD	dwEraseBlkSize=0;
  BOOLEAN bEnableCylicStorage=FALSE;
 // BOOLEAN bMounted=FALSE;
 // BOOLEAN bIsFORMATED=FALSE;
  DWORD dwSDSize=0;
  DWORD dwSDUsedSize=0;
  DWORD dwSDAvailSize=0;
  DWORD dwUsedPercentage=0;
  char  szTemp3[24];
  //char  szTemp4[24];
  //char  szTemp5[24];
 // struct stat stFile;
 char  szTmpFileName[64];
  // FLOAT  fSDUsedPercentage=0.0;
  FILE *fp=NULL;
//  DIR *pDir=NULL;
//  BOOLEAN bNewMount=FALSE;
 


  printf("Content-type: text/xml\r\n");
  printf("Cache-Control: no-cache\r\n\n"); 
  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
  printf("<root>\n");
  
 
  
  memset(szQueryStr,'\0',128);
 
  if (getenv("QUERY_STRING")==NULL)
  {
	status=SD_ERR_QUERY_STRING_NULL;
	goto respond;

  }
  strncpy(szQueryStr,getenv("QUERY_STRING"),strlen(getenv("QUERY_STRING")));
  szTemp1= strtok (szQueryStr,szDelimiter1);
  szTemp2 = strtok (NULL, szDelimiter1);
  strtok(szTemp1,szDelimiter2);
  szCmd=strtok(NULL,szDelimiter2);
  strtok(szTemp2,szDelimiter2);
  szPara=strtok(NULL,szDelimiter2);
  
  

  memset(szSDDevName,'\0',16);
  memset(szTemp3,'\0',24);
  if(Mounted(AUTOFS_SD_MOUNT_POINT)==FALSE)
  {
    u_sleep(10);
  }
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
      unlink(szTmpFileName);
    }
    else
    {
	  unlink(szTmpFileName);
	  status=SD_ERR_DETATCHED;
	  goto respond;
    
    }  
    
    strncpy(szSDDevName,szTemp3,strlen("mmcblk")+1);
    
    memset(szSDPartName,'\0',24);
    snprintf(szSDPartName,24,"%sp1",szSDDevName);
    
    if (szSDDevName[0] == '\0')
    {
	   status=SD_ERR_DETATCHED;
	   goto respond;
    
    }

    
    // bMounted=Mounted(AUTOFS_SD_MOUNT_POINT);
     

    /*2. mkfs.vfat -I /dev/mmcblk0 */
   
    //printf("%dszPara=%s\n",__LINE__,szPara);
    if (strncmp(szPara,"vfat",strlen(szPara))==0)
    {
      //printf("%d\n",__LINE__);
      iFS=VFAFS;
      if (strncmp(szCmd,FORMAT_CMD,strlen(szCmd))==0 || strncmp(szCmd,FSCK_CMD,strlen(szCmd))==0 )
      {
	// //0. must stop recoder thread.
	if(Mounted(AUTOFS_SD_MOUNT_POINT)==TRUE)
	{
	    //UmountFS(const char *szRecordPath)
	     if(UmountFS(AUTOFS_SD_MOUNT_POINT)!= S_OK)
	     {
	 	u_sleep(10);    
	 	if(UmountFS(AUTOFS_SD_MOUNT_POINT)!=S_OK)
		{	// printf("(%d)\n",__LINE__);
			status=SD_ERR_FS_UMOUNT;
			goto respond;
	     
		}
	     }
	}
	
	if(strncmp(szCmd,FORMAT_CMD,strlen(szCmd))==0)
	{ 
	  //printf("[sdmanage](%d)\n",__LINE__);
	  dwEraseBlkSize=get_sd_erase_blk_size(szSDDevName);
	   //printf("[sdmanage](%d)\n",__LINE__);
	  if(dwEraseBlkSize==0)
	  {
	    status=SD_ERR_FORMAT;
	    goto respond;
	  }
	  //printf("[sdmanage](%d)\n",__LINE__);
	  u_sleep(10);
	  //printf("[sdmanage](%d)\n",__LINE__);
	 // memset(szCommand,'\0',128);
	  //snprintf(szCommand,128,"/sbin/mkfs.ext2 /dev/%s > /dev/null 2>&1",szSDDevName);
	  //system(szCommand);
	  // printf("[sdmanage](%d)\n",__LINE__);
	  memset(szCommand,'\0',128);
	  snprintf(szCommand,128,"/bin/dd if=/dev/zero of=/dev/%s bs=1024 count=1024 > /dev/null 2>&1",szSDDevName);
	  system(szCommand);
	
	  memset(szSDCylinders,'\0',16);
	  memset(szCommand,'\0',128);
	  snprintf(szCommand,128,"/sbin/fdisk  -ul /dev/%s |  grep cylinders  |  awk -F',' '{ print $3}' |  awk -F' ' '{ print $1 }'  > %s",szSDDevName,szTmpFileName);
	  system(szCommand);
	
	  
	  fp=fopen(szTmpFileName,"r");
	  if(fp != NULL)
	  {
	    
	    fscanf(fp,"%s",szSDCylinders);
	   // printf("[sdmnage](%d)szSDCylinders=%s\n",__LINE__,szSDCylinders);
	    fclose(fp);
     
	  }
	  else
	  {
	    status=SD_ERR_FORMAT;
	    goto respond;
    
	  }  
	  //  printf("[sdmanage](%d)\n",__LINE__);
	  
	  if (szSDCylinders[0] == '\0')
	  {
	    status=SD_ERR_FORMAT;
	    goto respond;
    
	  }
	  unlink(szTmpFileName);
	    
	    
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"/bin/touch %s > /dev/null  2>&1",szTmpFileName);
	   system(szCommand);
	   
	   //fdisk  /dev/mmcblk0 //d->c->u->n->p->1->(64k/512=128)->(default value)->t->c
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"c\"   > %s",szTmpFileName);
	   system(szCommand);
	  
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"u\"   > %s",szTmpFileName);
	   system(szCommand);
	   
	   //memset(szCommand,'\0',128);
	   //snprintf(szCommand,128,"echo \"d\"   > %s",szTmpFileName);
	   //system(szCommand);
	   
	  memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"n\"   > %s",szTmpFileName);
	   system(szCommand);
	 
	    memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"p\"   >> %s",szTmpFileName);
	   system(szCommand);
	   
	 
	    memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"1\"   >> %s",szTmpFileName);
	   system(szCommand);
	     
	   //(64k/512=128)
	    memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo  %u   >> %s",(dwEraseBlkSize/512),szTmpFileName);
	   system(szCommand);
	   
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"%s\"   >> %s",szSDCylinders,szTmpFileName);
	   system(szCommand);
	
	   
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"t\"   >> %s",szTmpFileName);
	   system(szCommand);
	   
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"c\"   >> %s",szTmpFileName);
	   system(szCommand);
	   
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"w\"   >> %s",szTmpFileName);
	   system(szCommand);
	   
	   
	   
	  system("echo \"0 0 0\" > /proc/sys/kernel/printk");
	  //printf("[sdmanage](%d)\n",__LINE__);
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"/sbin/fdisk  /dev/%s  <  %s > /dev/null 2>&1",szSDDevName,szTmpFileName);
	   system(szCommand);
	 //  printf("[sdmanage](%d)\n",__LINE__);
	    system("echo \"7 7 7\" > /proc/sys/kernel/printk");
	    
	    u_sleep(10);
	    memset(szCommand,'\0',128);
	    snprintf(szCommand,128,"/usr/sbin/mkfs.vfat -F 32 -s 32 /dev/%sp1 > /dev/null 2>&1" ,szSDDevName);
	    system(szCommand);
	   //printf("[sdmanage](%d)\n",__LINE__);
	    
	   u_sleep(10); 
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"/usr/sbin/fsck.vat -a  /dev/%sp1  > /dev/null  2>&1",szSDDevName);
	   system(szCommand);
	   
	   memset(szFilePath,'\0',128);
	   snprintf(szFilePath,128,"%s/%s",AUTOFS_SD_MOUNT_POINT,VREC_SIGNATURE);
	   if(touch(szFilePath)!= 0)
	   {

	     
	      status=SD_ERR_WP;
	      goto respond;
	    }
	   system("sync;sync;sync");
	   //  printf("[sdmanage](%d)\n",__LINE__);
	   
	    
	    unlink(szTmpFileName);
	}
	
	if(strncmp(szCmd,FSCK_CMD,strlen(szCmd))==0)
	{
	   #if 0
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"/bin/touch /tmp/fsck.dat > /dev/null  2>&1");
	   system(szCommand);
	   
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"echo \"1\" > /tmp/fsck.dat");
	   system(szCommand);
	   #endif
	   memset(szCommand,'\0',128);
	   snprintf(szCommand,128,"/usr/sbin/fsck.vat -a  /dev/%sp1  > /dev/null  2>&1",szSDDevName);
	   system(szCommand);
	   
	   //unlink("/tmp/fsck.dat");
	
	}
	//bMounted=FALSE;
	//bIsFORMATED=TRUE;
      }
     // printf("%d\n",__LINE__);
    }
    else
    {
	status=SD_ERR_FS_UNDEFINED;
	goto respond;
	  
    }
  
    
      	
     if(strncmp(szCmd,ENABLE_CYCLIC_STORAGE_CMD,strlen(szCmd))==0)
     {
       cyclic_storage(TRUE);
    
     
     }  
     else if (strncmp(szCmd,DISABLE_CYCLIC_STORAGE_CMD,strlen(szCmd))==0)	
     {
       cyclic_storage(FALSE);
   
     }

  


    if(is_cyclic_storage()==TRUE)
    {
      bEnableCylicStorage=TRUE;
    
    }
    else
    {
       bEnableCylicStorage=FALSE;
    }
      
      
	
	
	
      
      
      
	memset(szFilePath,'\0',128);
	snprintf(szFilePath,128,"%s/%s",AUTOFS_SD_MOUNT_POINT,VREC_SIGNATURE);
	if(touch(szFilePath)!= 0)
	{

	  if(Mounted(AUTOFS_SD_MOUNT_POINT)==FALSE)
	  {
	     status =SD_ERR_FS_MOUNT;
	     goto respond; 
	  
	  }
	  status=SD_ERR_WP;
	  goto respond;
	
	
	
	}


	if(get_sd_capability_status(AUTOFS_SD_MOUNT_POINT,
				 &dwSDSize,
				 &dwSDAvailSize, 
				 &dwSDUsedSize,
				 &dwUsedPercentage)==S_FAIL)
	{			 
				 
	  dwSDSize=0;
	  dwSDUsedSize=0;
	  dwSDAvailSize=0;
	  dwUsedPercentage=0;
	}
	if(storage_is_full(AUTOFS_SD_MOUNT_POINT)==TRUE)
	{
	      status=SD_ERR_FULL;
	      goto respond;
	}
 
respond:


      printf("<filesystem>%d</filesystem>\n",iFS);
      printf("<total_size>%u</total_size>\n",dwSDSize);
      printf("<used_size>%u</used_size>\n",dwSDUsedSize);
      printf("<free_size>%u</free_size>\n",dwSDAvailSize);
      printf("<used_percentage>%u</used_percentage>\n", dwUsedPercentage);
      printf("<enable_cyclic_storage>%d</enable_cyclic_storage>\n",bEnableCylicStorage);
    
      if (status ==SD_ERR_DETATCHED)
      {
	printf("<status>Error</status>\n");
	printf("<message>SD_DETATCHED</message>\n");
      
      }
      else if (status == SD_ERR_FS_UNDEFINED)
      {
	  printf("<status>Error</status>\n");
	  printf("<message>FS_UNDEFINED</message>\n");
      
      }
      else if(status ==SD_ERR_FS_MOUNT)
      {
	  printf("<status>Error</status>\n");
	  printf("<message>FS_MOUNT_ERROR</message>\n");
      
      }
      else if (status ==SD_ERR_FS_UMOUNT)
      {
	    printf("<status>Error</status>\n");
	    printf("<message>FS_UMOUNT_ERROR</message>\n");
      
      }
      else if (status == SD_ERR_WP )
      {
	    printf("<status>Error</status>\n");
	    printf("<message>FS_WRITE_PROTECT</message>\n");
      
      }
      else if (status == SD_ERR_FULL)
      {	
	    printf("<status>Error</status>\n");
	    printf("<message>SD_FULL</message>\n");
      
      
      }
      else if (status == SD_ERR_FS_MOUNT_POINT)
      {
	    printf("<status>Error</status>\n");
	    printf("<message>FS_MOUNT_POINT_ERROR</message>\n");

      }
      else if (status == SD_ERR_QUERY_STRING_NULL)
      {
		
	   printf("<status>Error</status>\n");
	    printf("<message>CGI_QUERY_STRING_IS_NULL</message>\n");

      }
      else if (status == SD_ERR_FORMAT)
      {
	   printf("<status>Error</status>\n");
	   printf("<message>FORMAT_SD_ERROR</message>\n");
      
      }
      else //if(status == SD_ERR_NOERR)
      {
	printf("<status>OK</status>\n");
	printf("<message>NO_ERROR</message>\n");
	
      }
    // printf("<command>%s</command>\n",szCmd);
    //printf("<parameter>%s</parameter>\n",szPara);
     printf("</root>\n");
     return 0;

}


