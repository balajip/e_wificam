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
 * sysmgr_http.c
 *
 * \brief
 * Cerberus system manager : modify Boa and serverpush config file.
 *
 * \date
 * 2009/01/19
 *
 * \author
 * Acer Tsai
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "xmlmgr.h"

#define SERVERPUSH_XML_CONFIG			"/etc/conf.d/serverpush_conf.xml"
// warning : the config file and the temp file should be in the same partition
#ifdef _HTTP_SERVER_LIGHTHTTP_
  #define WEB_SERVER_CONF                 "/etc/conf.d/lighttpd/lighttpd.conf"
  #define TMP_WEB_SERVER_CONF             "/etc/conf.d/lighttpd/lighttpd.conf.tmp"
#else
  #define WEB_SERVER_CONF                 "/etc/conf.d/boa/boa.conf"
  #define TMP_WEB_SERVER_CONF             "/etc/conf.d/boa/boa.conf.tmp"
#endif
#define SYS_TOUCH_CMD					"/bin/touch"

SCODE set_accessname(HANDLE hXMLSVRPUSHObject, DWORD dwIndex, CHAR *szName)
{
	CHAR acBuffer[256];
	sprintf(acBuffer, "/root/server_push/video[@TrackNo='%u']/accessname", dwIndex);
	XmlMgr_SetConfValue(hXMLSVRPUSHObject, acBuffer, szName);
	return S_OK;
}

SCODE set_httpdport(char *szPort)
{
	FILE *fpConf;
	FILE *fpConfTmp;
	fpConf = fopen(WEB_SERVER_CONF, "r");
	fpConfTmp = fopen(TMP_WEB_SERVER_CONF, "w");
	if (fpConf != NULL && fpConfTmp != NULL)
	{
		char szBuf[256];
		while (fgets(szBuf, 256, fpConf) != NULL)
		{
#ifdef _HTTP_SERVER_LIGHTHTTP_
			if (!strncmp(szBuf, "server.port", 11))
			{
				fprintf(fpConfTmp, "server.port = %s\n", szPort);
			}
#else
			if (!strncmp(szBuf, "Port", 4))
			{
				fprintf(fpConfTmp, "Port %s\n", szPort);
			}
#endif
			else
			{
				fprintf(fpConfTmp, "%s", szBuf);
			}
		}
		fclose(fpConf);
		fclose(fpConfTmp);
		if (rename(TMP_WEB_SERVER_CONF, WEB_SERVER_CONF) != 0)
		{
			return S_FAIL;
		}
//fprintf(stderr, "%s %d : http port %s\n", __FILE__, __LINE__, szPort);
		return S_OK;
	}
	else
	{
		return S_FAIL;
	}
}

SCODE set_httpdtrackname(CHAR *szName1, CHAR *szName2)
{
	FILE *fpConf;
	FILE *fpConfTmp;
	DWORD dwIndex = 0;
	CHAR acBuffer[3][64];
	CHAR szBuffer[256];

	fpConf = fopen(WEB_SERVER_CONF, "r");
	fpConfTmp = fopen(TMP_WEB_SERVER_CONF, "w");
	if (fpConf != NULL && fpConfTmp != NULL)
	{
		char szBuf[256];
		while (fgets(szBuf, 256, fpConf) != NULL)
		{
			if (!strncmp(szBuf, "Transfer", 8))
			{
				sscanf(szBuf, "%s %s %s", acBuffer[0], acBuffer[1], acBuffer[2]);
				if (strstr(acBuffer[2], "/tmp/serverpush/http_spush.sck") != NULL)
				{
					if (dwIndex == 0)
						sprintf(szBuffer, "%s /%s %s", acBuffer[0], szName1, acBuffer[2]);
					else					
						sprintf(szBuffer, "%s /%s %s", acBuffer[0], szName2, acBuffer[2]);
					dwIndex++;
					fprintf(fpConfTmp, "%s\n", szBuffer);
				}
				else
				{
					fprintf(fpConfTmp, "%s", szBuf);
				}
			}
			else
			{
				fprintf(fpConfTmp, "%s", szBuf);
			}
		}
		fclose(fpConf);
		fclose(fpConfTmp);
		if (rename(TMP_WEB_SERVER_CONF, WEB_SERVER_CONF) != 0)
		{
			return S_FAIL;
		}
		return S_OK;
	}
	else
	{
		return S_FAIL;
	}	
}

SCODE sysmgr_set_http(char *szParam)
{
	// DO: 1. modify the serverpush config file
	//     2. touch the new file at /usr/share/www/html/xxx.mjpg
	// 	   3. modify the BOA config Port value
	//     4. modify the BOA config Transfer xxx.mjpg
	//     5. restart boa
	//     6. restart serverpush
	
	char *tok;
	HANDLE hXMLObject;
	TXmlMgrInitOptions tXmlMgrInitOpt;
/*-- mark start by acer.tsai 2009/02/27, that is for accessname used
	char *szConf;
	HANDLE hXMLSVRPUSHObject;
	DWORD dwTotalTrackNo;
	DWORD dwIndex;
	CHAR acBuffer[256];
-- mark end by acer.tsai 2009/02/27, that is for accessname used */
	/* check ending '$', szBuf[ret-1]='\n', szBuf[ret]='\0' */
	if (szParam[strlen(szParam)-2] != '$')
	{
		return S_FAIL;
	}
	// CMD_HTTP_CONF
	tok = (char *) strtok(szParam, " ");
	// temporal XML configuration file
	tok = (char *) strtok(NULL, " ");
	// use xmlmgr to parse the file, initial
	tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
	if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK)
	{
		fprintf(stderr, "Initial fail\n");
		return S_FAIL;
	}
	// read file 
	if (XmlMgr_ReadFile(hXMLObject, tok) != S_OK)
	{
		fprintf(stderr, "Open XML file %s fail\n", tok);
		return S_FAIL;
	}

/*-- mark start by acer.tsai 2009/02/27, that is for accessname used
	if (XmlMgr_Initial(&hXMLSVRPUSHObject, &tXmlMgrInitOpt) != S_OK)
	{
		fprintf(stderr, "Initial fail\n");
		return S_FAIL;
	}
	// read file 
	if (XmlMgr_ReadFile(hXMLSVRPUSHObject, SERVERPUSH_XML_CONFIG) != S_OK)
	{
		fprintf(stderr, "Open XML file %s fail\n", SERVERPUSH_XML_CONFIG);
		return S_FAIL;
	}
	
	szConf = XmlMgr_GetConfValue(hXMLObject, "/root/total_num");
	dwTotalTrackNo = strtoul(szConf, (char**)NULL, 10);

	CHAR *TrackName[dwTotalTrackNo];
	
	for (dwIndex = 0; dwIndex < dwTotalTrackNo; dwIndex++)
	{
		// set access name
		sprintf(acBuffer, "/root/video[@TrackNo='%ld']/accessname", dwIndex);
		szConf = XmlMgr_GetConfValue(hXMLObject, acBuffer);
		set_accessname(hXMLSVRPUSHObject, dwIndex, szConf);
		TrackName[dwIndex] = szConf;
		// modify the name used for BOA
		sprintf(acBuffer, "%s /usr/share/www/html/%s", SYS_TOUCH_CMD, szConf); 
		system(acBuffer);
	}

	// write xml DOM tree to file
	if (XmlMgr_WriteFile(hXMLSVRPUSHObject, SERVERPUSH_XML_CONFIG) != S_OK)
	{
		fprintf(stderr, "Write to %s fail\n", SERVERPUSH_XML_CONFIG);
		return S_FAIL;
	}
-- mark end by acer.tsai 2009/02/27, that is for accessname used */

	// modify BOA config	
	if (set_httpdport(XmlMgr_GetConfValue(hXMLObject, "/root/HTTP_port")) != S_OK)
	{
		return S_FAIL;
	}

/*-- mark start by acer.tsai 2009/02/27, that is for accessname used
	if (set_httpdtrackname(TrackName[0],TrackName[1]) != S_OK)
	{
		return S_FAIL;
	}
-- mark end by acer.tsai 2009/02/27, that is for accessname used */

	XmlMgr_Release(&hXMLObject);
/*-- mark start by acer.tsai 2009/02/27, that is for accessname used
	XmlMgr_Release(&hXMLSVRPUSHObject);
-- mark end by acer.tsai 2009/02/27, that is for accessname used */
	unlink(tok);
		
	return S_OK;
}

