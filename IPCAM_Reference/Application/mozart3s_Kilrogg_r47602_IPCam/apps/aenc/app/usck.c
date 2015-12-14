/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2000-2004 VN Inc. All rights reserved.
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
 * usck.c
 *
 * \brief
 * Usocket
 *
 * \date
 * 2006/06/07
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include "typedef.h"

int	initClientSocket(const char *szSckName)
{
	int	sck;
	struct	sockaddr_un	sunx;

	/* Create the unix socket */
	sck = socket(AF_UNIX, SOCK_DGRAM, 0);
	memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;
	(void) strncpy(sunx.sun_path, szSckName, strlen(szSckName));
	if (connect(sck, (struct sockaddr *)&sunx, 
				sizeof(sunx.sun_family) + strlen(sunx.sun_path)) < 0)
	{
		perror("UNIX socket connect failed!");
		fprintf(stderr, "%s UNIX socket connect failed!\n", szSckName);
		close(sck);
		sck = -1;
	}

	return sck;
}

