/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2005 VN Inc. All rights reserved.
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
 * Copyright 2000-2005 VN, Inc. All rights reserved.
 *
 * \file
 * serverpushapp_init.c
 *
 * \brief
 * Initial each required component
 *
 * \date
 * 2007/1/3
 *
 * \author
 * Jessie Lin
 *
 *
 *******************************************************************************
 */
#include "serverpush_local.h"

#include <stdio.h>
#include <string.h>
#include <libgen.h>     // dirname
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>  // chmod
#include <sys/stat.h>   // chmod
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "xmlwrapper.h"
#include "fdipc.h"
#include "dbgdefs.h"


extern TXmlWrapperTreeMap ptTreeMap[];


static void create_path_dir(char *szDirPath);
//static int create_unix_socket(const char *path);
//static void open_unix_socket(char *szConfValue, void *pObj);
/* =========================================================================================== */
SCODE XmlWrapper_Setting(TServerPushAppInfo *pThis)
{
	TXmlWrapperInitOptions tInitOptions;
		
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if ( XmlWrapper_Initial(&(pThis->hXMLWrapperObject), &tInitOptions) != S_OK)
	{
		printf("Initial XmlWrapper object fail! \n");
		return S_FAIL;
	}
	if ( XmlWrapper_SetHandler(pThis->hXMLWrapperObject, ptTreeMap,NULL) != S_OK)
	{
		printf("Set XMLWrapper handler fail! \n");
		return S_FAIL;
	}
	return S_OK;
}

/* =========================================================================================== */
SCODE ServerPush_SetHTTPSck(TServerPushAppInfo *pThis)
{
    create_path_dir(pThis->szHttpFdipcPath);

    unlink(pThis->szHttpFdipcPath);
    
    // ====== create communication socket for fdipc from Boa======    
    pThis->iHttpFdipcSck = fdipc_server_socket(pThis->szHttpFdipcPath, 
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (pThis->iHttpFdipcSck == -1)
    {
        printf("[SERVER_PUSH] %screate fdipc server fail\n", get_commonlog_time());
        return S_FAIL;
    }
    return S_OK;
}

/* =========================================================================================== */
/*SCODE ServerPush_SetMPEGEncSck(TServerPushAppInfo *pThis, DWORD dwTrackNo)
{
    //int                 iFlgs = 0;
    
    // ====== Create and set socket fd ====== 
    open_unix_socket(pThis->tTrackInfo[dwTrackNo].szSckPath, 
                     (void *)&(pThis->tTrackInfo[dwTrackNo].fdEncSck));

        
    // ======Set Nonblocking ======
    //iFlgs = fcntl(pThis->tTrackInfo[dwTrackNo].fdEncSck, F_GETFL, 0);
    //fcntl(pThis->tTrackInfo[dwTrackNo].fdEncSck, F_SETFL, iFlgs | O_NONBLOCK);
    
    if (pThis->tTrackInfo[dwTrackNo].fdEncSck == -1)
    {
        printf("[SERVER_PUSH] %sTrack %02d: create MJPEG ecnoder socket server fail\n", 
               get_commonlog_time(), dwTrackNo);
        return S_FAIL;
    }
    return S_OK;
}
*/
/* =========================================================================================== */
/*static int create_unix_socket(const char *path)
{
    struct sockaddr_un sunx;
    int fd;

    if (path[0] == '\0')
        return -1;

    (void) unlink(path);

    memset(&sunx, 0, sizeof(sunx));
    sunx.sun_family = AF_UNIX;
    (void) strncpy(sunx.sun_path, path, sizeof(sunx.sun_path));
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd < 0 || bind(fd, (struct sockaddr *) &sunx, sizeof(sunx.sun_family)+strlen(sunx.sun_path)) < 0 || chmod(path, 0666) < 0)
    {
        DBPRINT3("cannot create %s (%d).%s\n", path, errno, strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}
*/
/* =========================================================================================== */
/*static void open_unix_socket(char *szConfValue, void *pObj)
{
    if ((*(int *) pObj = create_unix_socket(szConfValue)) == -1)
    {
        DBPRINT1("Create UNIX socket %s failed!\n", szConfValue);
    }
    return;
}*/

/* =========================================================================================== */
static void create_path_dir(char *szDirPath)
{
    char    *pcChr;
    DWORD    dwSuffixLength = 0;
    DWORD    dwPathLength = 0;
    char     szTmpDir[256];

    /*****************************************************************
     * Create directory
     *****************************************************************/
    pcChr = strchr(szDirPath, '/');
    while (pcChr)
    {
        dwSuffixLength = strlen(pcChr);
        if (strlen(szDirPath) >= dwSuffixLength)
        {
            dwPathLength = strlen(szDirPath) - dwSuffixLength;
            memcpy(szTmpDir, szDirPath, dwPathLength);
            szTmpDir[dwPathLength] = '\0';
#ifdef _DEBUG_CONFIG
            fprintf(stderr, "create tmp directory : %s\n", szTmpDir);
#endif
            mkdir(szTmpDir, 0755);
        }
        pcChr = strchr(++pcChr, '/');
    }

    return;
}

/* =========================================================================================== */

SCODE ServerPush_InitSharedBuffMgr(TServerPushAppInfo *pThis, DWORD dwTrackNo)
{
	CHAR* byTemp;
    TSharedBuffMgrInitOptions tInitOptions;

    byTemp = strrchr(pThis->tTrackInfo[dwTrackNo].szSckPath, 'r');
    byTemp++;

  	tInitOptions.dwVersion = SHAREDBUFFMGR_VERSION;
	tInitOptions.pObjectMem = NULL;
	tInitOptions.dwMinorNum = (DWORD) strtol(byTemp,NULL,10);
	tInitOptions.eProcessRole = esbmprReader;
	
	if (SharedBuffMgr_Initial(&(pThis->tTrackInfo[dwTrackNo].hJPEGEncObj), &tInitOptions) != S_OK)
	{
		printf("[SERV_PUSH] Initialize shared buffer manager object fail !!\n");
		return S_FAIL;
	}    
    return S_OK;
}
/* ========================================================================= */

SCODE ServerPush_CreateFifo(char *pData, int *piFIFO, int iFlag)
{
    int iFIFO,res;
    
    printf("FIFO path = %s\r\n",(char*)pData);
    
    if(access((char*)pData, F_OK) == -1)
    {
        res = mkfifo((char*)pData, 0777);
        if (res != 0)
        {
            printf("[SERV_PUSH] Could not create fifo %s\n",(char*)pData);
            return S_FAIL;
        }
    }
    
    //use blocking mode to open write or it will return -1 
    //if open read is not ready yet 
    
    if( iFlag == O_RDONLY )
    	iFIFO = open( pData, O_RDONLY | O_NONBLOCK);
	else    	
		iFIFO = open( pData, O_WRONLY);
		
    if(  iFIFO  < 0  )
    {
        printf("open fifo failed %d\r\n",errno);
        return S_FAIL;    
    }
    else 
    {
        printf("[SERV_PUSH] open FIFO: %d success\r\n",iFIFO);
        *(int*)piFIFO = iFIFO;
        return S_OK;    
    }
}
