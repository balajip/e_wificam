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
 * adec_init.c
 *
 * \brief
 * Modules and libraries initialize functions implement.
 *
 * \date
 * 2008/3/20
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#include "adec_init.h"


/* ========================================================================== */
SCODE Adec_InitG711Dec(HANDLE *phG711DecObj, CHAR *szMode)
{
    TG711DecInitOptions tG711DecInitOptions;
    EG711DecMode eDecMode;
    memset(&tG711DecInitOptions, 0, sizeof(TG711DecInitOptions));
    if (strcmp(szMode, "A_LAW") == 0)
    {
        eDecMode = G711DEC_MODE_A_LAW;
    }
    else if (strcmp(szMode, "U_LAW") == 0)
    {
        eDecMode = G711DEC_MODE_U_LAW;
    }
    else
    {
        DBPRINT0("[ADEC] Unknown G711 DecMode !!\n");
        return S_FAIL;
    }

    tG711DecInitOptions.dwVersion = G711DEC_VERSION;
   	tG711DecInitOptions.dwInFrameSize = PCM_S8_MONO8K_SIZE;
	tG711DecInitOptions.eDecMode = eDecMode;
   	tG711DecInitOptions.pObjectMem = NULL;

    if (G711Dec_Initial(phG711DecObj, &tG711DecInitOptions) != S_OK)
    {
        DBPRINT1("[ADEC] Initial G711 %s object fail !!\n", szMode);
    }
    return S_OK;
}
