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
 * uuidgen.c
 *
 * \brief
 * UUID generator
 *
 * \date
 * 2011/05/27
 *
 * \author
 * Acer Tsai
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "uuid.h"

static const char id[] = "$Id$";
#define UUID_LEN 36
#define UUID_CONF "/etc/uuid.conf"
#define UUID_FIXED_PREFIX	"56617469-6373-526f-636b-"
#define GET_HWADDR_CMD		"/sbin/ifconfig eth0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'"
#define HWADDR_RECORD		"/tmp/tmp4hwaddr"
#define BUF_SZ				128
#define MAC_BUF_SZ			13
#define HWADDR_LEN			17

/* ============================================================================================= */
int create_uuid(char *szUUID)
{
#if 0
	// size of szUUID should be UUID_LEN+1 (37)
	uuid_t uuid_tmp;
	
	uuid_generate_time(uuid_tmp);
	uuid_unparse(uuid_tmp, szUUID);
	szUUID[UUID_LEN] = '\0';
	uuid_clear(uuid_tmp);
#else
	char	acBuf[BUF_SZ];
	char	acMac[MAC_BUF_SZ];
	FILE	*fp;
	int		i, j;

	snprintf(acBuf, BUF_SZ, "%s > %s", GET_HWADDR_CMD, HWADDR_RECORD);
	unlink(HWADDR_RECORD);
	system(acBuf);

	fp = fopen(HWADDR_RECORD, "r");
	if (fp != NULL)
	{
		fread(acBuf, 1, BUF_SZ, fp);
		acBuf[HWADDR_LEN] = '\0';
		fclose(fp);
		unlink(HWADDR_RECORD);

		j = 0;
		for (i=0; i < HWADDR_LEN; ++i)
		{
			if (i%3 != 2)
			{
				if ((acBuf[i] >= 65) && (acBuf[i] <= 90))
				{	// upper case character to lower case character
					acMac[j++] = acBuf[i]+32;
				}
				else
				{
					acMac[j++] = acBuf[i];
				}
			}
		}
		acMac[MAC_BUF_SZ-1] = '\0';
		sprintf(szUUID, "%s%s", UUID_FIXED_PREFIX, acMac);
	}
	else
	{
		sprintf(szUUID, "%s%s", UUID_FIXED_PREFIX, "66616b656877");
	}
#endif
	return 1;
}

int main(int argc, char *argv[])
{
    char szUUID[UUID_LEN+1];
    FILE *pfUUID;
    
    if ((pfUUID = fopen(UUID_CONF, "wb")) == NULL) {
        printf("[uuidgen] open %s fail..... !!!\n", UUID_CONF);
        return 0;
    } else {
        memset(szUUID, 0, UUID_LEN+1);
        create_uuid(szUUID);
        fprintf(pfUUID, "%s", szUUID);
        fclose(pfUUID);
    }

    return 0;
}
