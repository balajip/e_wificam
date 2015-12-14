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
 * sysmgr_system.c
 *
 * \brief
 * Cerberus system manager : modify system time and led switch
 *
 * \date
 * 2006/10/03
 *
 * \author
 * Rey Cheng
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "errordef.h"
#include "xmlmgr.h"

#define SYS_DATE_CMD			"/bin/date"
#define SYS_SET_HWCLOCK_CMD		"/sbin/hwclock -w -u"
#define HEART_BEAT_CMD			"/etc/init.d/heartbeat"
#define SYS_NTP_CRON_CMD		"/usr/sbin/setntpcron"
#define SYS_CROND_CMD			"/etc/init.d/bbcrond"
#define SYSTEM_XML_FILE			"/etc/conf.d/system.xml"
#define SYS_NTP_CRON			"/var/spool/cron/root-crontabs/msntp.crontab"
#define TZ_FILE					"/etc/TZ"

SCODE sysmgr_set_system(char *szParam)
{
	char *tok;
	char szCmd[256];
	char *timezone;
	HANDLE hXMLObject;
	TXmlMgrInitOptions tXmlMgrInitOpt;
	/* check ending '$', szBuf[ret-1]='\n', szBuf[ret]='\0' */
	if (szParam[strlen(szParam)-2] != '$')
	{
		return S_FAIL;
	}
	// CMD_SET_SYSTEM : '4'
	tok = (char *) strtok(szParam, " ");
	// temporal XML configuration file
	tok = (char *) strtok(NULL, " ");
	// use xmlmgr to parse the file
	// initial
	tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
	if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK)
	{
		fprintf(stderr, "Initial fail\n");
		exit(1);
	}
	// read file
	if (XmlMgr_ReadFile(hXMLObject, tok) != S_OK)
	{
		fprintf(stderr, "Open XML file %s fail\n", tok);
		exit(1);
	}
	// write to SYSTEM_XML_FILE
	if (XmlMgr_WriteFile(hXMLObject, SYSTEM_XML_FILE) != S_OK)
	{
		fprintf(stderr, "Write to %s fail\n", SYSTEM_XML_FILE);
		return S_FAIL;
	}
	// TODO : use exec to replace system

	timezone = XmlMgr_GetConfValue(hXMLObject, "/root/system/timezone");
	sprintf(szCmd, "echo %s > %s", timezone, TZ_FILE);
	system(szCmd);

	// set time first
	if (!strncmp(XmlMgr_GetConfValue(hXMLObject, "/root/system/time_mode"), "manual", 6))
	{
		FILE *fpNTPCron;
		sprintf(szCmd, "%s %s > /dev/null", SYS_DATE_CMD, XmlMgr_GetConfValue(hXMLObject, "/root/system/set_time"));
fprintf(stderr, "%s %d : %s\n", __FILE__, __LINE__, szCmd);
		system(szCmd);
		system(SYS_SET_HWCLOCK_CMD);
		// clean SYS_NTP_CRON
		if ((fpNTPCron = fopen(SYS_NTP_CRON, "w")) == NULL)
		{
			return S_FAIL;
		}
		fclose(fpNTPCron);
	}
	else if (!strncmp(XmlMgr_GetConfValue(hXMLObject, "/root/system/time_mode"), "ntp", 3))
	{
		system(SYS_NTP_CRON_CMD);
		system(SYS_SET_HWCLOCK_CMD);
	}
	// restart crond
	sprintf(szCmd, "%s restart > /dev/null", SYS_CROND_CMD);
fprintf(stderr, "%s %d : %s\n", __FILE__, __LINE__, szCmd);
	system(szCmd);
	/* set led switch */
/*	if (!strncmp(XmlMgr_GetConfValue(hXMLObject, "/root/system/led"), "on", 2))
	{
		sprintf(szCmd, "%s start > /dev/null", HEART_BEAT_CMD);
	}
	else
	{
		sprintf(szCmd, "%s stop > /dev/null", HEART_BEAT_CMD);
	}
	system(szCmd);
*/
	XmlMgr_Release(&hXMLObject);
	unlink(tok);
	return S_OK;
}
