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
 * Copyright 2007-2010 VN, Inc. All rights reserved.
 *
 * \file
 * adec_app.h
 *
 * \brief
 * Audio decoder application interface header file.
 *
 * \date
 * 2008/3/19
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#ifndef _ADEC_APP_H_
#define _ADEC_APP_H_

#include <signal.h>
#include <semaphore.h>
#include "typedef.h"
#include "errordef.h"

typedef struct adec_initopt
{
    //! Configuration file path
    CHAR* szConfigFile;
} TAdecInitOpt;

#if 0
// --------------------- function brief ----------------------------------------
SCODE AdecApp_Initial(HANDLE *phObject, TVencSlaveAppInitOpt tArgInfo);
SCODE AdecApp_Release(HANDLE *phObject);
#endif

/*!
******************************************************************************
* \brief
* Initial a adec process handle.
*
* \retval S_OK
* Initial object ok.
*
* \retval S_FAIL
* Initial object failed.
*
* \retval ERR_OUT_OF_MEMORY
* System out of memory.
*
* \remark
* A adec object shall be initialized before using it.
*
******************************************************************************
*/
SCODE AdecApp_Initial(HANDLE *phObject, TAdecInitOpt tAdecInitOpt);

/*!
******************************************************************************
* \brief
* Release all resource used by adec.
*
* \retval S_OK
* Release the object's resource ok.
*
* \retval S_FAIL
* Release the object's resource failed.
*
* \remark
* Shoule call Video_Stop before calling this.
*
* \see AdecApp_Stop
*
******************************************************************************
*/
SCODE AdecApp_Release(HANDLE *phObject);

SCODE AdecApp_Process_Loop(HANDLE hObject);
SCODE Adec_LoadConfig(HANDLE hObject);
SCODE Adec_ResetConfigFlag(HANDLE hObject);
SCODE Adec_ResetTerminalFlag(HANDLE hObject);

#endif // define _ADEC_APP_H_
