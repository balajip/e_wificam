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
#include <pwd.h>
#include <crypt.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
//gcc -o setpasswd setpasswd.c  -lcrypt

//#define SYSMGR_PASSWD_PATH			"passwd"
#define TMP_SYSMGR_PASSWD_PATH		"/tmp/passwd.new"


void sysmgr_set_passwd(const char *szUser, const char *szPass,const char *szPasswdPath);

void print_usage(void)
{
    fprintf(stderr, "Set password tool\n"
           "Usage:\n"
           "    setpasswd  [-u username] [-p password]\n"
           "Options:\n"
           "    -u username     	username\n"
           "    -p password		new password\n"
	   "	-f passwd_path		passwd file path\n"
           "    -h              	this help\n");
    exit(1);
}

int main(int argc, char *argv[])
{
  int ch;
  char *szUsername=NULL;
  char *szPasswd=NULL;
  char *szPasswdPath=NULL;
  
  while ((ch = getopt(argc, argv, "u:p:f:h")) != -1) 
  {
        switch(ch) {
            case 'u':
		szUsername = strdup(optarg);
                break;
            case 'p':
                szPasswd = strdup(optarg);
                break;
	    case 'f':
		szPasswdPath = strdup(optarg);
		break;
            default:
                print_usage();
                exit(1);
        }
  }
  if(szUsername == NULL || szPasswd == NULL)
  {
      if(szPasswdPath== NULL)
      {
	print_usage();
	exit(1);
      }
    
  }
  sysmgr_set_passwd(szUsername,szPasswd,szPasswdPath);
  if (szUsername!=NULL)
  {
      free(szUsername);
      szUsername=NULL;
  }
  
  if (szPasswd!=NULL)
  {
      free(szPasswd);
      szPasswd=NULL;
  }
  if(szPasswdPath!=NULL)
  {
      free(szPasswdPath);
      szPasswdPath=NULL;
  }
  return 0;
}

/* copied from passwd.c in busybox */
static int i64c(int i)
{
	if (i <= 0)
		return ('.');
	if (i == 1)
		return ('/');
	if (i >= 2 && i < 12)
		return ('0' - 2 + i);
	if (i >= 12 && i < 38)
		return ('A' - 12 + i);
	if (i >= 38 && i < 63)
		return ('a' - 38 + i);
	return ('z');
}

static char *crypt_make_salt(void)
{
	time_t now;
	static unsigned long x;
	static char result[7];

	time(&now);
	x += now + getpid() + clock();
	strcpy(&(result[0]), "$1$");
	result[3] = i64c(((x >> 18) ^ (x >> 6)) & 077);
	result[4] = i64c(((x >> 12) ^ x) & 077);
	result[5] = '$';
	result[6] = '\0';
	return result;
}

void sysmgr_set_passwd(const char *szUser, const char *szPass,const char *szPasswdPath)
{
	FILE *fpPasswd;
	FILE *fpPasswdBack;
	char szCmd[128];
	struct passwd *entry;
	
	if ((fpPasswd = fopen(szPasswdPath , "r")) == NULL)
	{
		printf("setpasswd : open file %s fail\n", szPasswdPath);
		return;
	}
	if ((fpPasswdBack = fopen(TMP_SYSMGR_PASSWD_PATH, "w")) == NULL)
	{
		printf("setpasswd : open file %s fail\n", TMP_SYSMGR_PASSWD_PATH);
		return;
	}

	while ((entry = (struct passwd *)fgetpwent(fpPasswd)) != NULL)
	{
		if (strcmp(entry->pw_name, szUser))
		{
			putpwent(entry, fpPasswdBack);
		}
		else
		{
			char *szCryptOut;
			// fill the backup ectry (with new password)
			if (strlen(szPass) != 0)
			{
				szCryptOut = (char *)crypt(szPass, crypt_make_salt());
				fprintf(fpPasswdBack, "%s:%s:%d:%d:%s:%s:%s\n", entry->pw_name, szCryptOut, entry->pw_uid, entry->pw_gid, entry->pw_gecos, entry->pw_dir, entry->pw_shell);
				
			}
			else
			{
				szCryptOut = NULL;
				fprintf(fpPasswdBack, "%s::%d:%d:%s:%s:%s\n", entry->pw_name, entry->pw_uid, entry->pw_gid, entry->pw_gecos, entry->pw_dir, entry->pw_shell);
				
			}
		}
	}
	
	fclose(fpPasswdBack);
	fclose(fpPasswd);
	snprintf(szCmd,128,"mv %s %s",TMP_SYSMGR_PASSWD_PATH, szPasswdPath);
	system(szCmd);

	return;
}

