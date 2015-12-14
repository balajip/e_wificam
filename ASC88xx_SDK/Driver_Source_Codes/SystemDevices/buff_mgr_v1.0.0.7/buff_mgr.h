/*
 * Copyright 2011 VN, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ============================================================================================== */
#ifndef __BUFF_MGR_H__
#define __BUFF_MGR_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "vivo_codec.h"

/* ============================================================================================== */
#define BUFF_MGR_VERSION MAKEFOURCC(1, 0, 0, 7)
#define BUFF_MGR_ID_VERSION "1.0.0.7"
#define DEVICE_NAME 		"buff_mgr"
//#define MAX_READER_NUM 4
//#define BUFF_MGR_BUFFSET_NUM 4

/* ============================================================================================== */
#define BUFF_MGR_IOC_MAGIC 0xB0

#define BUFF_MGR_IOC_ENTER_CRITICAL_SECTION         _IO     (BUFF_MGR_IOC_MAGIC, 0)
#define BUFF_MGR_IOC_LEAVE_CRITICAL_SECTION         _IO     (BUFF_MGR_IOC_MAGIC, 1)
#define BUFF_MGR_IOC_GET_VERSION_NUMBER             _IOR    (BUFF_MGR_IOC_MAGIC, 2, DWORD)
#define BUFF_MGR_IOC_CHECK_INIT                     _IOR    (BUFF_MGR_IOC_MAGIC, 3, BOOL)
#define BUFF_MGR_IOC_INITIAL                        _IOW    (BUFF_MGR_IOC_MAGIC, 4, TBUFFMGRInitOptions)
#define BUFF_MGR_IOC_GET_ADDR                       _IOR    (BUFF_MGR_IOC_MAGIC, 5, DWORD)
#define BUFF_MGR_IOC_GET_SIZE                       _IOR    (BUFF_MGR_IOC_MAGIC, 6, DWORD)
#define BUFF_MGR_IOC_GET_INBUFF                     _IOR    (BUFF_MGR_IOC_MAGIC, 7, TBUFFMGRState)
#define BUFF_MGR_IOC_RELEASE_INBUFF                 _IOW    (BUFF_MGR_IOC_MAGIC, 8, DWORD)
#define BUFF_MGR_IOC_GET_OUTBUFF                    _IOWR   (BUFF_MGR_IOC_MAGIC, 9, TBUFFMGRState)
#define BUFF_MGR_IOC_RELEASE_OUTBUFF                _IOW    (BUFF_MGR_IOC_MAGIC, 10, SDWORD)
                                                                                                                
//#define BUFF_MGR_IOC_REGISTER_TO_DRIVER			    _IOR	(BUFF_MGR_IOC_MAGIC, 11, SDWORD)
#define BUFF_MGR_IOC_REGISTER_TO_DRIVER             _IOWR   (BUFF_MGR_IOC_MAGIC, 11, TBUFFMGRReaderInit)

#define BUFF_MGR_IOC_UNREGISTER_TO_DRIVER		    _IOW	(BUFF_MGR_IOC_MAGIC, 12, SDWORD)
#define BUFF_MGR_IOC_GET_NEEDMAPPING			    _IOWR 	(BUFF_MGR_IOC_MAGIC, 13, TBUFFMGRMappingStatus)
#define BUFF_MGR_IOC_SET_NEEDMAPPING			    _IOW 	(BUFF_MGR_IOC_MAGIC, 14, TBUFFMGRMappingStatus)
#define BUFF_MGR_IOC_WRITER_SET_FIRST			    _IOW	(BUFF_MGR_IOC_MAGIC, 15, BOOL)
#define BUFF_MGR_IOC_GET_BUFNUM					    _IOR	(BUFF_MGR_IOC_MAGIC, 16, DWORD)
#define BUFF_MGR_IOC_GET_TABLE_INFO				    _IOWR	(BUFF_MGR_IOC_MAGIC, 17, TBUFFMGRTableInfo)
#define BUFF_MGR_IOC_READER_STOP				    _IOW	(BUFF_MGR_IOC_MAGIC, 18, SDWORD)

#define BUFF_MGR_IOC_GET_OUTBUFF_BY_SLOW            _IOWR   (BUFF_MGR_IOC_MAGIC, 19, TBUFFMGRState)
#define BUFF_MGR_IOC_RELEASE_OUTBUFF_BY_SLOW        _IOW    (BUFF_MGR_IOC_MAGIC, 20, SDWORD)

//#define BUFF_MGR_IOC_GET_TABLE_INFO				_IOR    (BUFF_MGR_IOC_MAGIC, 7, TBUFFMGRTableInfo)
//#define BUFF_MGR_IOC_READER_COUNT_ADD_ONE		_IOW	(BUFF_MGR_IOC_MAGIC, 12, DWORD)
//#define BUFF_MGR_IOC_REG_WRITER                  _IOR    (BUFF_MGR_IOC_MAGIC, 12, DWORD)
//#define BUFF_MGR_IOC_DEL_WRITER                  _IOR    (BUFF_MGR_IOC_MAGIC, 13, DWORD)
//#define BUFF_MGR_IOC_REG_READER                  _IOR    (BUFF_MGR_IOC_MAGIC, 14, DWORD)
//#define BUFF_MGR_IOC_DEL_READER                  _IOR    (BUFF_MGR_IOC_MAGIC, 15, DWORD)

#define BUFF_MGR_IOC_MAX_NUMBER  21

/* ============================================================================================== */

typedef struct buff_mgr_init_options
{
    DWORD dwBufNum;
    DWORD dwHdrSize;
    DWORD dwDataSize;
    DWORD dwPhyBaseAddr;
} TBUFFMGRInitOptions;

typedef struct buff_mgr_registration_options
{
	BOOL bReader;	// Reader/Writer
	DWORD dwCounter;
} TBUFFMGRRegistrationOptions;


typedef struct buff_mgr_state
{
	SDWORD sdwReaderID; // ignore by writer
    DWORD dwIndex;
    DWORD dwHdrAddr;
    DWORD dwDataAddr;
} TBUFFMGRState;

typedef struct buff_mgr_mapping_status
{
	SDWORD sdwReaderID;
	BOOL bNeedMapping;
} TBUFFMGRMappingStatus;

typedef struct buff_mgr_table_info
{
    DWORD dwIndex;
    DWORD dwHdrAddr;
    DWORD dwDataAddr;
} TBUFFMGRTableInfo;

typedef enum buff_mgr_reader_role
{
	ebmrReader,
	ebmrSlowReader,
} EBUFFMGRReaderRole;

typedef struct buff_mgr_reader_init
{
	SDWORD                      sdwReaderID;
    EBUFFMGRReaderRole          eRole;
} TBUFFMGRReaderInit;

/* ============================================================================================== */
#endif //__BUFF_MGR_H__

/* ============================================================================================== */
