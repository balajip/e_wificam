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
 * linked_list.h
 *
 * \brief
 * double linked list, items in the list are unique
 *
 * \date
 * 2006/09/27
 *
 * \author
 * Jessie Lin
 *
 **********************************************************************
 */

#include <stdlib.h>

/* =========================================================================================== */
struct socket_node
{
    int iNum;
    struct socket_node *ptPre;
    struct socket_node *ptNext;
};
typedef struct socket_node TSckNode;

#if 0
/* ===========================================================================================
 * Function List
 * =========================================================================================== */
TSckNode *list_Add(TSckNode *ptHead, int iNum);
TSckNode *list_Delete(TSckNode *ptNode);
TSckNode *list_DeleteAll(TSckNode *ptHead);
#endif


/*!
 *********************************************************************
 * \brief
 * Add a new node with iNum in list and update the node amount
 * recorded in ptHead. If iNum already exists, return the node directly
 *
 * \param ptHead
 * a (o) pointer of TSckNode, List head, record amount of nodes.
 *  
 * \param iNum
 * a (i) int to fill in;
 *
 * \retval
 * pointer of TSckNode with iNum in the list headed by ptHead
 *
 ******************************************************************** */
TSckNode *list_Add(TSckNode *ptHead, int iNum);

/*!
 *********************************************************************
 * \brief
 * Delete ptNode and update the node number recorded in ptHead.
 *
 * \param ptHead
 * a (o) pointer of TSckNode, List head, record amount of nodes.
 *
 * \param ptNode
 * a (o) pointer of TSckNode to delete
 *
 * \retval 
 * the previous node of ptNode
 *
 ******************************************************************** */
TSckNode *list_Delete(TSckNode *ptHead, TSckNode *ptNode);

/*!
 *********************************************************************
 * \brief
 * Delete all ptNode in list after ptHead
 *
 * \param ptHead
 * a (o) pointer of TSckNode, List head, record amount of nodes.
 *
 * \retval
 * ptHead
 *
 ******************************************************************** */
TSckNode *list_DeleteAll(TSckNode *ptHead);

