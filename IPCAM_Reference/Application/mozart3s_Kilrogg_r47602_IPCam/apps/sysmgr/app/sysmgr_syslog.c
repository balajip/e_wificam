/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2006 VN Inc. All rights reserved.
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
 *  | VN INC.                                                    |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 * 
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * sysmgr_syslog.c
 *
 * \brief
 * Cerberus system manager : modify syslogd settings
 *
 * \date
 * 2006/09/21
 *
 * \author
 * Rey Cheng
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "errordef.h"
#include "xmlmgr.h"

#define SYSLOG_CONF_FILE		"/etc/syslog.conf"
// warning : the config file and the temp file should be in the same partition
#define	SERVICES_FILE			"/etc/services"
#define	TMP_SERVICES_FILE		"/etc/services.tmp"
#define REMOTE_LOG_XML_CONFIG	"/etc/conf.d/remotesyslog_conf.xml"

SCODE sysmgr_set_syslog(char *szParam)
{
	char *tok;
	char *szEnableRemoteLog;
	char szBuf[256];
	FILE *fpSyslogConf;
	FILE *fpServices;
	FILE *fpServicesTmp;
	HANDLE hXMLObject;
	TXmlMgrInitOptions tXmlMgrInitOpt;
	/* check ending '$', szBuf[ret-1]='\n', szBuf[ret]='\0' */
	if (szParam[strlen(szParam)-2] != '$')
	{
		return S_FAIL;
	}
	// CMD_SYSLOG_CONF
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
	// write to REMOTE_LOG_XML_CONFIG
	if (XmlMgr_WriteFile(hXMLObject, REMOTE_LOG_XML_CONFIG) != S_OK)
	{
		fprintf(stderr, "Write to %s fail\n", REMOTE_LOG_XML_CONFIG);
		return S_FAIL;
	}
	szEnableRemoteLog = XmlMgr_GetConfValue(hXMLObject, "/root/remotesyslog/enable_remote_syslog");
	// set syslog.conf
	if ((fpSyslogConf = fopen(SYSLOG_CONF_FILE, "w")) == NULL)
	{
		XmlMgr_Release(&hXMLObject);
		return S_FAIL;
	}
	fprintf(fpSyslogConf, "*.*    /var/log/messages\n");
	// Remote log is enabled
	if (strncmp(szEnableRemoteLog, "0", 1))
	{
		fprintf(fpSyslogConf, "*.*    @%s\n", XmlMgr_GetConfValue(hXMLObject, "/root/remotesyslog/server_ip"));
		// set /etc/services
		fpServices = fopen(SERVICES_FILE, "r");
		fpServicesTmp = fopen(TMP_SERVICES_FILE, "w");
		if (fpServices == NULL || fpServicesTmp == NULL)
		{
			XmlMgr_Release(&hXMLObject);
			return S_FAIL;
		}
		while (fgets(szBuf, 256, fpServices) != NULL)
		{
			if (!strncmp(szBuf, "syslog", 6))
			{
				fprintf(fpServicesTmp, "syslog\t\t%s/udp\n", XmlMgr_GetConfValue(hXMLObject, "/root/remotesyslog/server_port"));
			}
			else
			{
				fprintf(fpServicesTmp, "%s", szBuf);
			}
		}
		fclose(fpServices);
		fclose(fpServicesTmp);
		if (rename(TMP_SERVICES_FILE, SERVICES_FILE) != 0)
		{
			return S_FAIL;
		}
	}
	fclose(fpSyslogConf);
	XmlMgr_Release(&hXMLObject);
	unlink(tok);
	return S_OK;
}
