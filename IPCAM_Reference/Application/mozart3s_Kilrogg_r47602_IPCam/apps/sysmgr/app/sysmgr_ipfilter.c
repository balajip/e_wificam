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
 * sysmgr_ipfilter.c
 *
 * \brief
 * Cerberus system manager : modify iptable setting at /usr/local/virus/iptables/iptables.deny
 *
 * \date
 * 2009/02/06
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

#define IPFILTER_DENYRULE_FILE			"/etc/iptables/iptables.deny"

SCODE sysmgr_set_iptable(char *szParam)
{
	char *tok;
	char *szConf;
	char *szStartIP;
	char *szEndIP;
	HANDLE hXMLObject;
	TXmlMgrInitOptions tXmlMgrInitOpt;
	SDWORD sdwTotalTrackNo;
	DWORD dwIndex;
	CHAR acBuffer[256];
	FILE *fpIPTableDeny;
	
	// DO: . verify the command line string
	//     . rewrite the iptables.deny file to the new rule.
	
	/* check ending '$', szBuf[ret-1]='\n', szBuf[ret]='\0' */
	if (szParam[strlen(szParam)-2] != '$')
	{
		return S_FAIL;
	}
	// CMD_IPFILTER_CONF
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
	// read config file 
	if (XmlMgr_ReadFile(hXMLObject, tok) != S_OK)
	{
		fprintf(stderr, "Open XML file %s fail\n", tok);
		return S_FAIL;
	}

	szConf = XmlMgr_GetConfValue(hXMLObject, "/root/access_list/deny/total_num");
	sdwTotalTrackNo = strtoul(szConf, (char**)NULL, 10);
	
	// open the iptable.deny file
	if ((fpIPTableDeny = fopen(IPFILTER_DENYRULE_FILE, "w")) == NULL)
	{
		return S_FAIL;
	}
	// write the header 
	fprintf(fpIPTableDeny, "#!/bin/sh\n#\n# This script will deny some IPs that set from /usr/share/www/cgi-bin/admin/ipfilter.lua\n\n");

	for (dwIndex = 0; dwIndex < sdwTotalTrackNo; dwIndex++)
	{
		// get start IP
		sprintf(acBuffer, "/root/access_list/deny/rule[@id='%u']/start", dwIndex);		
		szStartIP = XmlMgr_GetConfValue(hXMLObject, acBuffer);
		
		// get end IP
		sprintf(acBuffer, "/root/access_list/deny/rule[@id='%u']/end", dwIndex);
		szEndIP = XmlMgr_GetConfValue(hXMLObject, acBuffer);
		
		fprintf(fpIPTableDeny, "/usr/sbin/iptables -A INPUT -p all -m iprange --src-range %s-%s -j DROP\n", szStartIP, szEndIP);
	}
	
	fclose(fpIPTableDeny);
	XmlMgr_Release(&hXMLObject);
	unlink(tok);
		
	return S_OK;	
}
