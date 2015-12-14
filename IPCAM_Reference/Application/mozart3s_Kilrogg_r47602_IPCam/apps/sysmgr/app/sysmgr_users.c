#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <crypt.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "sysmgr.h"
#include "dbgdefs.h"

#define SYSMGR_PASSWD_PATH			"/etc/passwd"
#define TMP_SYSMGR_PASSWD_PATH		"/etc/passwd.new"


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

void sysmgr_set_passwd(const char *szUser, const char *szPass)
{
	FILE *fpPasswd;
	FILE *fpPasswdBack;
	char szBuffer[256];

	struct passwd *entry;

	if ((fpPasswd = fopen(SYSMGR_PASSWD_PATH, "r")) == NULL)
	{
		DBPRINT1("sysmgr : open file %s fail\n", SYSMGR_PASSWD_PATH);
		return;
	}
	if ((fpPasswdBack = fopen(TMP_SYSMGR_PASSWD_PATH, "w")) == NULL)
	{
		DBPRINT1("sysmgr : open file %s fail\n", TMP_SYSMGR_PASSWD_PATH);
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
#ifdef _HTTP_SERVER_LIGHTHTTP_
				system("/bin/sed -i \'s/#include \"conf.d\\/auth.conf\"/include \"conf.d\\/auth.conf\"\'/ /etc/conf.d/lighttpd/modules.conf");
				snprintf(szBuffer, 256, "/bin/sed -i \'s/^root.*$/root:%s/g\' /etc/conf.d/lighttpd/.lighttpd.pw", szPass);
				system(szBuffer);
#else				
				system("/bin/sed -i \"s/ProtRealm root_realm anonymous/#ProtRealm root_realm anonymous/\" /etc/conf.d/boa/boa.conf");
#endif				
			}
			else
			{
				szCryptOut = NULL;
				fprintf(fpPasswdBack, "%s::%d:%d:%s:%s:%s\n", entry->pw_name, entry->pw_uid, entry->pw_gid, entry->pw_gecos, entry->pw_dir, entry->pw_shell);
#ifdef _HTTP_SERVER_LIGHTHTTP_
				system("/bin/sed -i \'s/include \"conf.d\\/auth.conf\"/#include \"conf.d\\/auth.conf\"\'/ /etc/conf.d/lighttpd/modules.conf");
				snprintf(szBuffer, 256, "/bin/sed -i \'s/^root.*$/root:/g\' /etc/conf.d/lighttpd/.lighttpd.pw");
				system(szBuffer);
#else				
				system("/bin/sed -i \"s/#ProtRealm root_realm anonymous/ProtRealm root_realm anonymous/\" /etc/conf.d/boa/boa.conf");
#endif				
			}
		}
	}
	
	fclose(fpPasswdBack);
	fclose(fpPasswd);
	if (rename(TMP_SYSMGR_PASSWD_PATH, SYSMGR_PASSWD_PATH) < 0)
	{
		DBPRINT0("sysmgr : write password file fail\n");
	}
    //reload boa and rtsps for authentication
#ifdef _HTTP_SERVER_LIGHTHTTP_
    system("/etc/init.d/http stop");
	sleep(1);
    system("/etc/init.d/http start");
#else
    system("/etc/init.d/http reload");
#endif // _HTTP_SERVER_LIGHTHTTP_
	return;
}
