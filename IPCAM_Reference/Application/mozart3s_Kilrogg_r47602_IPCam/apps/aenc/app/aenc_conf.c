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
 * aenc_conf.c
 *
 * \brief
 * Audio encoder configuration 
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
#include <string.h>
#include <assert.h>
#include "xmlwrapper.h"
#include "aenc.h"
#include "dbgdefs.h"
//=====================================================================================
#if 1 //Not support configfile reloading by Jeff Liao 
void CheckSetOptions(TAencXMLTTh *ptaxHandle)
{
	TDevOpt  *ptDevOpt = ptaxHandle->ptAencInfo->ptdoHead;
	TAFConf *ptacConf;

	while(ptDevOpt)
	{
		ptacConf = &(ptDevOpt->tacConf);
		if (ptacConf->bCodec || ptacConf->bBitRate || ptacConf->bSampRate || ptacConf->bEncMode || ptDevOpt->bVolumeChanged || ptDevOpt->bMuteChanged || ptDevOpt->bInputSelChanged)
		{
			ptacConf->bSetOptions = TRUE;
			//printf("(bCodec, bBitRate, bSampRate) = (%d, %d, %d)\n", ptacConf->bCodec, ptacConf->bBitRate, ptacConf->bSampRate);
		}
#ifdef _CHECK_RTSPS_STATUS_	
		ptacConf->bCallReload = TRUE;
#endif
		ptDevOpt = ptDevOpt->ptdoNext;
	}
}
#endif
SCODE readConf(const char *filename, TAencXMLTTh *ptaxHandle)
{				
	if (ptaxHandle->bFirstLoad == TRUE)
	{
		printf("[AENC] FIRST read config %s! \n", filename);
		if (XmlWrapper_ReadFile_UsrDefFunc(filename, ptaxHandle->hXmlWrapperObject, ptaxHandle) != S_OK)
		{		
			printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! \n", __FILE__, __LINE__);
			return S_FAIL;
		}						
		ptaxHandle->bFirstLoad = FALSE;		
	}
#if 1 //Not support configfile reloading by Jeff Liao 
	else
	{
		//printf("[AENC] Reload config %s! \n", filename);
		if ( XmlWrapper_ReadFile_UsrDefFunc_SameXmlHdl(NULL, ptaxHandle->hXmlWrapperObject, ptaxHandle) != S_OK)
		{		
			printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc_SameXmlHdl! \n", __FILE__, __LINE__);
			return S_FAIL;
		}		
		/* Set Options */
		CheckSetOptions(ptaxHandle);		
	}
#endif
	
	return S_OK;
}

