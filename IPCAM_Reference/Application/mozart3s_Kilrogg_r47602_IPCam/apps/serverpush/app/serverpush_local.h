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
 * mjpegserverpush_local.h
 *
 * \brief
 * MJpeg server push
 *
 * \date
 * 2006/11/14
 *
 * \author
 * Jessie Lin
 *
 *
 *******************************************************************************
 */

#ifndef _SERVER_PUSH_LOCAL_H_
#define _SERVER_PUSH_LOCAL_H_

#include <syslog.h>
#include "serverpushapp.h"

#include "typedef.h"
#include "linked_list.h"
#include "ubuffer.h"
#include "ubuffer_limit.h"
#include "SharedBuffMgr.h"


//#define PID_FILE               "/var/run/serverpush.pid"

#define MAX_BITSTREAM_SIZE  (1024 << 10)          // match encoder setting
#define VIDEO_UBUFFER_SIZE  (64 << 10)
#define MAX_BUFFER_SIZE     (MAX_BITSTREAM_SIZE + 256) // match encoder setting
#define MAX_PATH_LEN    128

#define MAX_CLIENT  3

typedef struct u_buffer_info
{
    TUBuffer            tuBuf;          //! ubuffer
    SWORD               seHdrTag;       //! tag
    SWORD               seHdrLen;       //! len
    DWORD               dwJPEGSize;     //! Jpeg Size
    BYTE                abyData[MAX_BITSTREAM_SIZE]; //! bitstream data
}TUBufferInfo;

typedef struct client_information
{
    int                 fdCliSck;       //! socket to client
    BOOL                bWriting;       //! flag indicate the server is writing a frame to this client
    BYTE               *pbyBase;        //! pointer to Frame data
    BYTE               *pbyWrite;       //! pointer to the start position to write
    DWORD               dwWriteSz;      //! write size
    DWORD               dwSeqNo;        //! Record write frame
	char				aszIPAddr[32];  //! client IP Address
}TClientInfo;

typedef struct track_information
{
    DWORD               dwTrackNo;
	DWORD				dwStreamNo;
	DWORD				dwCmdCounter;
    CHAR               *szURI;          //! access name form HTTP Server
    CHAR               *szSckPath;      //! path to MJPG Venc
    int                 fdEnc;        //! file descriptor to MJPG Venc, that is get from sharedbuffer
    DWORD               dwDataOffset;   //! MJPEG frame offset
    BOOL                bReading;       //! flag indicate the server is reading frame data
    TUBufferInfo        tuBufInfo;        //! pointer to frame data
    DWORD               dwCliNum;
    TClientInfo         tClients[MAX_CLIENT];
    //! handle of JPEG encoder
    HANDLE              hJPEGEncObj;
	// the  fifo to notify media track action
	int     iFdFIFO;
	CHAR	*szFIFOPathName;

}TTrackInfo;

typedef struct server_push_app_info
{
    int                 iHttpFdipcSck;  //! socket for fdipc form HTTP server
    CHAR               *szHttpFdipcPath;//! path for fdipc form HTTP server
    CHAR               *szBoundary;     //! boundary of a JPEG frame
    CHAR               *szConfigPath;   //! path of config file
    DWORD               dwTrackNum;     //! Max track number
    TTrackInfo         *tTrackInfo;     //! pointer to a dynamic array of TTrackInfo
    HANDLE              hXMLWrapperObject;//! handle of XMLWrapper
    BOOL                bTerminate;     //! loop control
    
    fd_set              fdsModelR;      // fd_set model: read fds
//    fd_set              fdsModelW;      // fd_set model: write fds
    int                 fdMaxSck;
    
} TServerPushAppInfo;

#if 0
/* ===========================================================================================
 * Function List
 * =========================================================================================== */

// ====== serverpushapp_init.c ======
SCODE XmlWrapper_Setting(TServerPushAppInfo *pThis);
//SCODE Sck_Setting(TServerPushAppInfo *pThis);

// ====== serverpushapp_process.c ======
char *get_commonlog_time(void);
#endif

/*!
 *********************************************************************
 * \brief
 * Initial tXMLWrapperObject and set the TreeMap
 *
 * \param phObject
 * a (o) pointer of TServerPushAppInfo Handle
 *
 * \retval S_OK
 * Initial and set tXMLWrapperObject ok
 *
 * \retval S_FAIL
 * Initial and set tXMLWrapperObject faild
 *
 * \Remark
 * See ServerPushApp_SetHttpFdipcSck, ServerPushApp_SetTrackNum, 
 * ServerPushApp_SetTrackInfo, ServerPushApp_SetAccessName, etc.
 * for setting TreeMap
 *
 ******************************************************************** */
SCODE XmlWrapper_Setting(TServerPushAppInfo *pThis);

/*!
 *********************************************************************
 * \brief
 * Create and bind sockets, 
 * one for receiving HTTP client fd and uri form HTTP server(boa), 
 * the others for receiving media data form video encoder
 *
 * \param phObject
 * a (o) pointer of TServerPushAppInfo Handle
 *
 * \retval S_OK
 * Create the socket communicate with HTTP server ok
 *
 * \retval S_FAIL
 * Create the socket communicate with HTTP server faild
 *
 * \Remark
 * If Creating or binding faild on the socket for venc,
 * the function doesn't return S_FAIL but set -1 in the proper variable.
 *
 ******************************************************************** */
//SCODE Sck_Setting(TServerPushAppInfo *pThis);


SCODE ServerPush_SetHTTPSck(TServerPushAppInfo *pThis);
SCODE ServerPush_InitSharedBuffMgr(TServerPushAppInfo *pThis, DWORD dwTrackNo);
SCODE ServerPush_CreateFifo(char *pData, int *piFIFO, int iFlag);
//SCODE ServerPush_SetMPEGEncSck(TServerPushAppInfo *pThis, DWORD dwTrackNo);
void *ServerPushApp_Process(HANDLE hObject);
char *get_commonlog_time(void);
SCODE ServerPush_SendStopCmdToEnc(TServerPushAppInfo* pThis, DWORD dwTrackNo);

#endif // _SERVER_PUSH_LOCAL_H_
