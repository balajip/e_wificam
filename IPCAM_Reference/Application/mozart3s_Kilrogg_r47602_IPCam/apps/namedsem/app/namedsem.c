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

#include "my_semaphore.h"
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define NAMED_SEM_NAME "namedsem"
#define NAMED_SEM_PATH "/tmp/namedsem"
#define NAMED_SEM_MAP_DIR "/dev/shm/sem.tmp"
void namedsem_create(void);
void namedsem_destroy(void);
void namedsem_wait(void);
void namedsem_post(void);
void namedsem_getvalue(void);

int main(int argc, char **argv)
{
  char *szCmd=argv[1];
  
  //printf("szCmd=%s argv[1]=%s\n",szCmd,argv[1]);
  if (argc != 2)
  {
    printf("usage: namedsem [create|destroy|wait|post|getvalue]\n");
    goto exit;
  }
  
  if(szCmd==NULL)
  {
    printf("argv[1] is NULL!\n");
    
    goto exit;
  }
  
  if(strncmp(szCmd,"create",strlen(szCmd))==0)
  {
    namedsem_create();
  }
  else if (strncmp(szCmd,"destroy",strlen(szCmd))==0)
  {
    namedsem_destroy();
  }
  else if (strncmp(szCmd,"wait",strlen(szCmd))==0)
  {
    namedsem_wait();
  }
  else if (strncmp(szCmd,"post",strlen(szCmd))==0)
  {
    namedsem_post();
  }
  else if (strncmp(szCmd,"getvalue",strlen(szCmd))==0)
  {
    namedsem_getvalue();
  }
  else
  {
      printf("Unsupported command: %s\n",szCmd);
  }
  
exit:
  return 0;
}
void namedsem_create(void)
{
   DIR *pDir=NULL;
   FILE *fp=NULL; 
   char szPath[128];
   int isMapDirExist=1;
   mysem_t	*sem;

   
   if((pDir=opendir(NAMED_SEM_MAP_DIR)) == NULL)
   {
	isMapDirExist=0;
	
   }
   if(pDir!=NULL)
   {
    closedir(pDir);
   }
   if(isMapDirExist==1)
   {
      memset(szPath,'\0',128);
      snprintf(szPath,128,"%s/%s",NAMED_SEM_MAP_DIR,NAMED_SEM_NAME);
      if((fp=fopen(szPath,"r"))!=NULL)
      {
	  printf("(%d) Named semaphore %s has already existed!\n",__LINE__,NAMED_SEM_PATH);
	  goto exit;
      }
   }
   

   sem = mysem_open(NAMED_SEM_PATH,O_RDWR | O_CREAT, FILE_MODE, 1);
   if(sem==SEM_FAILED)
   {
     printf("(%d) Failed to call sem_open(): %s\n",__LINE__,strerror(errno));
   }
   else
   {
    mysem_close(sem);
   }
exit:   
   if(fp!=NULL)
   {
    fclose(fp);
   }
  return;
}
void namedsem_destroy(void)
{
  int ret=0;
  ret=mysem_unlink(NAMED_SEM_PATH);
  if(ret == -1)
  {
    printf("(%d) Failed to call sem_unlink(): %s\n",__LINE__,strerror(errno));
  }
  return;
}
void namedsem_wait(void)
{
    mysem_t	*sem;
    int ret=0;
    sem = mysem_open(NAMED_SEM_PATH, 0);
    if(sem==SEM_FAILED)
    {
      printf("(%d) Failed to call sem_open(): %s\n",__LINE__,strerror(errno));
      goto exit;
    }
    ret=mysem_wait(sem);
    if(ret==-1)
    {
      printf("(%d) Failed to call sem_wait(): %s\n",__LINE__,strerror(errno));
    }
    else
    {
      if(mysem_close(sem)==-1)
      {
	printf("(%d) Failed to call sem_close(): %s\n",__LINE__,strerror(errno));
	 goto exit;
      }
    }
exit:	
  return;
}
void namedsem_post(void)
{
    mysem_t	*sem;
    int ret=0;
    sem = mysem_open(NAMED_SEM_PATH, 0);
    if(sem==SEM_FAILED)
    {
      printf("(%d) Failed to call sem_open(): %s\n",__LINE__,strerror(errno));
      goto exit;
    }
    ret=mysem_post(sem);
    if(ret==-1)
    {
      printf("(%d) Failed to call sem_post(): %s\n",__LINE__,strerror(errno));
    }
    else
    {
      if(mysem_close(sem)==-1)
      {
	printf("(%d) Failed to call sem_close(): %s\n",__LINE__,strerror(errno));
	 goto exit;
      }
    }
exit:	
  return;
}
void namedsem_getvalue(void)
{
  int		val;
    mysem_t	*sem;
    int ret=0;
    sem = mysem_open(NAMED_SEM_PATH, 0);
    if(sem==SEM_FAILED)
    {
      printf("(%d) Failed to call sem_open(): %s\n",__LINE__,strerror(errno));
      goto exit;
    }
    ret=mysem_getvalue(sem, &val);
    if(ret==-1)
    {
      printf("(%d) Failed to call sem_getvalue(): %s\n",__LINE__,strerror(errno));
      goto exit;
    }
    else
    {
      if(mysem_close(sem)==-1)
      {
	printf("(%d) Failed to call sem_close(): %s\n",__LINE__,strerror(errno));
	 goto exit;
      }
    }
    printf("The semaphore value is %d \n",val);
exit:	
  return;
}
