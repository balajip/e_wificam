/*
**********************************************************************
* $Header$
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
* $History$
*
  ***********************************************************************
 */

/*!
 ***********************************************************************
 * Copyright 2000-2006 VN, Inc. All rights reserved.
 *
 * \file
 * linked_list.c
 *
 * \brief
 * Encode the video into compressed video stream.
 * Required libraries: Mp4VEnc, PBEngine
 *
 * \date
 * 2006/09/27
 *
 * \author
 * Jessie Lin
 *
 *
 **********************************************************************
 */
#include "linked_list.h" 
#include <stdio.h>
/* =========================================================================================== */
TSckNode *list_newnode(int iNum)
{
    TSckNode *ptNew = (TSckNode *)malloc(sizeof(TSckNode));
    ptNew->iNum = iNum;
    ptNew->ptPre = ptNew->ptNext = NULL;
    return ptNew;
}

/* =========================================================================================== */
TSckNode *list_Add(TSckNode *ptHead, int iNum)
{
    TSckNode *ptNew;
    TSckNode *ptNode = ptHead;

    // ====== check list for iNum ======
    while (ptNode->ptNext)
    {
        ptNode = ptNode->ptNext;
        if (ptNode->iNum == iNum)
        {
            return ptNode;
        }
    }
    
    ptNew = list_newnode(iNum);  
    
    // ====== redirect the links ======
    ptNew->ptNext = ptNode->ptNext;
    ptNew->ptPre = ptNode;
    
    if (ptNode->ptNext != NULL)
    {
        ptNode->ptNext->ptPre = ptNew;
    }
    ptNode->ptNext = ptNew;
    ptHead->iNum ++;
    return ptNew;
}

/* =========================================================================================== */
TSckNode *list_Delete(TSckNode *ptHead, TSckNode *ptNode)
{
    TSckNode *ptTmp = ptNode->ptPre;
    
    if (ptNode->ptNext != NULL)
    {
        ptNode->ptNext->ptPre = ptTmp;
    }
    
    if (ptTmp != NULL)
    {
        ptTmp->ptNext = ptNode->ptNext;
    }
    ptHead->iNum --;
    free(ptNode);    
    return ptTmp;
}

/* =========================================================================================== */
TSckNode *list_DeleteAll(TSckNode *ptHead)
{
    while (ptHead->ptNext != NULL)
    {
        list_Delete(ptHead, ptHead->ptNext);
    }
    return ptHead;
}

/* =========================================================================================== */

