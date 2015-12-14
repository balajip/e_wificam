/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib/VideoDisp_Mgr.c 5     12/10/15 4:18p Vincent $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: VideoDisp_Mgr.c $
 * 
 * *****************  Version 5  *****************
 * User: Vincent      Date: 12/10/15   Time: 4:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 6.0.0.0 
 * 2. FEATURE: Add VideoDisp_QueueBuf() and VideoDisp_DeQueueBuf() to
 * support dynamically feed display buffer address
 *
 * *****************  Version 4  *****************
 * User: Alan         Date: 11/07/06   Time: 11:59a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * FEATURE: Add VideoDisp_WaitComplete function - DONE.
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/12/05   Time: 9:28p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 *
 *
 */
/* ============================================================================================== */
#include "VideoDisp_Locals.h"

/* =========================================================================================== */
SCODE VideoDisp_Get_Buf(HANDLE hObject, TVideoDispState *ptState)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    return VideoDisp_GetBufVOC(ptInfo, ptState);
}

/* =========================================================================================== */
SCODE VideoDisp_Release_Buf(HANDLE hObject, TVideoDispState *ptState)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    return VideoDisp_ReleaseBufVOC(ptInfo, ptState);
}

/* =========================================================================================== */
void VideoDisp_Start(HANDLE hObject)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    VideoDisp_StartVOC(ptInfo);
}

/* ============================================================================================== */
/* Version 3.3.0.0 modification, 2011.07.05 */
void VideoDisp_WaitComplete(HANDLE hObject)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    VideoDisp_WaitVOC(ptInfo);
}
/* ======================================== */

/* =========================================================================================== */
void VideoDisp_Stop(HANDLE hObject)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    VideoDisp_StopVOC(ptInfo);
}

/* =========================================================================================== */
/* Version 5.0.0.0 modification, 2012.10.15 */
SCODE VideoDisp_QueueBuf(HANDLE hObject, TVideoDispState *ptState)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    return VideoDisp_QueueBufVOC(ptInfo, ptState);
}

/* =========================================================================================== */
SCODE VideoDisp_DeQueueBuf(HANDLE hObject, TVideoDispState *ptState)
{
    TVideoDispInfo *ptInfo = (TVideoDispInfo *)(hObject);

    return VideoDisp_DeQueueBufVOC(ptInfo, ptState);
}
/* ======================================== */
