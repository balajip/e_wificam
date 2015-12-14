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
 * sysmgr.h
 *
 * \brief
 * Cerberus system manager header file
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

#ifndef _SYSMGR_H_
#define _SYSMGR_H_
#include "errordef.h"
/*! Input command FIFO path */
#define SYSMGR_FIFO_PATH			"/tmp/sys_mgr.fifo"

/*! One byte 0x24 indocates END_OF_COMMAND */
#define END_OF_COMMAND			0x24	// '$'

/*! CMD_SYS_SECURITY "username" "password" END_OF_COMMAND */
#define CMD_SYS_SECURITY		48		// '0'
/*! CMD_NETWORK_CONF temporal_network_xml_file END_OF_COMMAND */
#define CMD_NETWORK_CONF		49		// '1'
/*! CMD_SYSLOG_CONF temporal_syslog_xml_file END_OF_COMMAND */
#define CMD_SYSLOG_CONF			50		// '2'
/*! CMD_SET_SYSTEM temporal_system_xml_file END_OF_COMMAND */
#define CMD_SET_SYSTEM			52		// '4', time and led control
/*! CMD_SYS_REBOOT END_OF_COMMAND */
#define CMD_SYS_REBOOT			51		// '3'
/*! CMD_HTTP_CONF END_OF_COMMAND */
#define CMD_HTTP_CONF		53		// '5'
/*! CMD_IPFILTER_CONF temporal_ipfilter_xml_file END_OF_COMMAND */
#define CMD_IPFILTER_CONF		54		// '6'
/*! CMD_SYS_RESTORE END_OF_COMMAND */
#define CMD_SYS_RESTORE			55		// '7'
/*! CMD_SET_DDNS ddns_xml_file END_OF_COMMAND */
#define CMD_SET_DDNS			56		// '8', set ddns parameter

void sysmgr_set_passwd(const char *szUser, const char *szPass);
SCODE sysmgr_set_network(char *szParam);
SCODE sysmgr_set_syslog(char *szParam);
SCODE sysmgr_set_system(char *szParam);
SCODE sysmgr_set_http(char *szParam);
SCODE sysmgr_set_iptable(char *szParam);
SCODE sysmgr_set_ddns(char *szParam);
#endif //_SYSMGR_H_
