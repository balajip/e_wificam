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

/*!
 *******************************************************************************
 * Copyright (c) 2007-2010 VN Inc. All rights reserved.
 *
 * \file
 * sysmgr.c
 *
 * \brief
 * Cerberus system manager : used to modify system configurations (receive commands from web CGI).
 * This program -sysmgr- should be run by super user.
 *
 * \date
 * 2006/09/14
 *
 * \author
 * Rey Cheng
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <linux/reboot.h>
//#include <sys/reboot.h>
#include "sysmgr.h"
#include "dbgdefs.h"

#define RECV_BUFFER_SIZE		256

int iRunning;

void sig_handle(int signo)
{
    if (signo == SIGTERM || signo == SIGINT)
    {
        iRunning = 0;
    }

    return;
}

int main(int argc, char *argv[])
{
	int ret;
	int fdFifo;
	char szBuf[RECV_BUFFER_SIZE];
	fd_set rfds; 

	unlink(SYSMGR_FIFO_PATH);
	ret = mkfifo(SYSMGR_FIFO_PATH, 0666);
	// create the named pipe
	if ((ret == -1) && (errno != EEXIST))
	{
		perror("Error creating the named pipe");
		exit(1);
	}
	ret = chmod(SYSMGR_FIFO_PATH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (ret < 0)
	{
		perror("Error changing the named pipe permission");
		exit(1);
	}
	// handle SIGTERM
    if (signal(SIGTERM, sig_handle) == SIG_ERR)
    {
		fprintf(stderr, "Can not catch SIGTERM\n");
    }
	/* Open the named pipe for reading */
	fdFifo = open (SYSMGR_FIFO_PATH, O_RDWR | O_NONBLOCK);
	/* loop */
	iRunning = 1;
	while (iRunning)
	{
		int	retSel, cnt = 0;
		FD_ZERO(&rfds);
		FD_SET(fdFifo, &rfds);
		retSel = select(fdFifo + 1, &rfds, NULL, NULL, NULL);
		// Read from the named pipe
		do {
			ret = read(fdFifo, &(szBuf[cnt]), 1);
			if (ret == 1) {
				cnt ++;
				if (szBuf[cnt-1] == '\n') {
					break;
				}
			} else {
				break;
			}
		} while (1);
		ret = cnt;
		/*
		ret = read (fdFifo, szBuf, RECV_BUFFER_SIZE);
		if (ret <= 0)
		{
			usleep(500000);
		}
		else
		*/
		if (ret > 0)
		{
			fprintf(stderr, "%s %d : %d Code:%d\n", __FILE__, __LINE__, ret, (int)szBuf[0]);
			szBuf[ret] = '\0';
			/* check CMD type */
			switch ((int)szBuf[0])
			{
			case CMD_SYS_SECURITY:
				{
					char szUser[64];
					char szPass[64];
					char *tok;
					tok = (char *)strtok(szBuf, " ");
					/* get username */
					if ((tok = (char *)strtok(NULL, " ")) == NULL)
					{
						DBPRINT0("sysmgr : strtok fail\n");
						break;
					}
					strcpy(szUser, tok);
					/* get password */
					if ((tok = (char *)strtok(NULL, " ")) == NULL)
					{
						DBPRINT0("sysmgr : strtok fail\n");
						break;
					}
					if (*tok == '$')
					{
						// no pass
						memset(szPass, 0, sizeof(szPass));
					}
					else
					{
						strcpy(szPass, tok);
						/* check ending '$' */
						if ((tok = (char *)strtok(NULL, " ")) == NULL)
						{
							DBPRINT0("sysmgr : strtok fail\n");
							break;
						}
					}
                    fprintf(stderr, "%s %d : %s %s %s\n", __FILE__, __LINE__, szUser, szPass, tok);
					sysmgr_set_passwd(szUser, szPass);
					break;
				}
			case CMD_NETWORK_CONF:
				{
					sysmgr_set_network(szBuf);
					//add by acer.stai 2011/04/06 
					//if network status modified, reboot system.
#if 0
					system("/etc/init.d/network restart");
					sync();
					system("/usr/sbin/reboot");
#endif
					break;
				}
			case CMD_SYSLOG_CONF:
				{
					sysmgr_set_syslog(szBuf);
					system("/etc/init.d/sysklogd restart");
					break;
				}
			case CMD_SYS_REBOOT:
				{
					sync();
					//reboot(LINUX_REBOOT_CMD_RESTART);
					system("/usr/sbin/reboot");
                    fprintf(stderr, "%s %d : reboot\n", __FILE__, __LINE__);
					break;
				}
			case CMD_SET_SYSTEM:
				{
					sysmgr_set_system(szBuf);
					break;
				}
			case CMD_HTTP_CONF:
				{
					sysmgr_set_http(szBuf);
					system("/etc/init.d/http restart");
					system("/etc/init.d/serverpush restart");
					system("/usr/sbin/setmdnsconf");
					system("/etc/init.d/mDNSResponderPosix reload");
					break;
				}
			case CMD_IPFILTER_CONF:
				{
					sysmgr_set_iptable(szBuf);
					system("/etc/init.d/iptable restart");
					break;
				}
			case CMD_SYS_RESTORE:
				{
					system("sh /usr/sbin/stopallproc");
					system("sh /usr/sbin/restore");
					sync();
					system("/usr/sbin/reboot");
					break;
				}
			case CMD_SET_DDNS:
			    {
                    sysmgr_set_ddns(szBuf);
                    system("/etc/init.d/ddns restart");
			    }
			default:
				{
					DBPRINT0("sysmgr : unknown command\n");
					break;
				}
			}
		}
	}
fprintf(stderr, "END\n");
	close(fdFifo);
	remove(SYSMGR_FIFO_PATH);
	return 0;
}
