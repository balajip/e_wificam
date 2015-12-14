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
 */

/*!
 *******************************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * aenc_task.h
 *
 * \brief
 * local data structure of theDevTask() 
 *
 * \date
 * 2006/12/01
 *
 * \author
 * Ming Jing Tsai
 *
 *
 *******************************************************************************
 */

#ifndef _AENC_TASK_H_
#define _AENC_TASK_H_

typedef struct aenc_ubuffer_conf
{
    DWORD dwPeriodFrames;
    DWORD dwInSize;
	DWORD dwOutSize;
	HANDLE hOut;		
	TDevOpt *ptDevOpt;	
}TAUC;

#endif


