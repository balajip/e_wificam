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
 * sysmgr_ddns.c
 *
 * \brief
 * Cerberus system manager : modify ddns setting at /etc/inadyn.conf
 *
 * \date
 * 2011/05/05
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

#define INADYN_CONF_FILE			"/etc/inadyn.conf"

SCODE sysmgr_set_ddns(char *szParam)
{

	char *tok;
	char *szUsername;
	char *szPassword;
	char *szPeriod;
	char *szHostname;
	
	HANDLE hXMLObject;
	TXmlMgrInitOptions tXmlMgrInitOpt;
	FILE *fpInadyn;
	
	// DO: . verify the command line string
	//     . rewrite the inadyn.conf file to the new rule.
	
	/* check ending '$', szBuf[ret-1]='\n', szBuf[ret]='\0' */
	if (szParam[strlen(szParam)-2] != '$') {
		return S_FAIL;
	}
	// CMD_DDNS_CONF
	tok = (char *) strtok(szParam, " ");
	// XML configuration file
	tok = (char *) strtok(NULL, " ");
	// use xmlmgr to parse the file, initial
	tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
	if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
		fprintf(stderr, "Initial fail\n");
		return S_FAIL;
	}
	// read config file 
	if (XmlMgr_ReadFile(hXMLObject, tok) != S_OK) {
		fprintf(stderr, "Open XML file %s fail\n", tok);
		return S_FAIL;
	}

	szUsername = XmlMgr_GetConfValue(hXMLObject, "/root/ddns/username");
	szPassword = XmlMgr_GetConfValue(hXMLObject, "/root/ddns/password");
	szPeriod = XmlMgr_GetConfValue(hXMLObject, "/root/ddns/update_period");
	szHostname = XmlMgr_GetConfValue(hXMLObject, "/root/ddns/hostname");
	
//	printf("%s %s %s %s\n", szUsername, szPassword, szPeriod, szHostname);

	// open the inadyn.conf file
	if ((fpInadyn = fopen(INADYN_CONF_FILE, "w")) == NULL) {
		return S_FAIL;
	}

	// write to file
	fprintf(fpInadyn, "# an example of /etc/inadyn.conf\n");
	fprintf(fpInadyn, "username %s\n", szUsername);
	fprintf(fpInadyn, "password %s\n", szPassword);
	fprintf(fpInadyn, "update_period %s\n", szPeriod);
	fprintf(fpInadyn, "alias %s\n", szHostname);
	fprintf(fpInadyn, "syslog");

	fclose(fpInadyn);
	XmlMgr_Release(&hXMLObject);

	return S_OK;	
}
