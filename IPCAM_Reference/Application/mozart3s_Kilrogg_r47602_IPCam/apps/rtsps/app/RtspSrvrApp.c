/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2006-2010 VN Inc. All rights reserved.
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
 * Copyright 2006-2010 VN, Inc. All rights reserved.
 *
 * \file
 * RtspSrvrApp.c
 *
 * \brief
 * Rtsp Server Application implementation
 *
 * \date
 * 2008/10/08
 *
 * \author
 * DJhow.Tu
 *
 *
 *******************************************************************************
 */

#include "RtspSrvrApp_local.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

SCODE RtspSrvrApp_ReleaseConfData(TRtspSrvrApp *ptRtspSrvrApp)
{
	int i;

	if (ptRtspSrvrApp->szHttpFdipcPath != NULL)
	{
		free(ptRtspSrvrApp->szHttpFdipcPath);
		ptRtspSrvrApp->szHttpFdipcPath = NULL;
	}

	if (ptRtspSrvrApp->szAuthMode != NULL)
	{
		free(ptRtspSrvrApp->szAuthMode);
		ptRtspSrvrApp->szAuthMode = NULL;
	}
	
	if (ptRtspSrvrApp->atVideoSettings != NULL)
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalVTrackNum; ++i)
		{
			if (ptRtspSrvrApp->atVideoSettings[i].szMechanism != NULL)
			{
				free(ptRtspSrvrApp->atVideoSettings[i].szMechanism);
			}
			if (ptRtspSrvrApp->atVideoSettings[i].szPath != NULL)
			{
				free(ptRtspSrvrApp->atVideoSettings[i].szPath);
			}
			if (ptRtspSrvrApp->atVideoSettings[i].szCmdFiFoPath != NULL)
			{
				free(ptRtspSrvrApp->atVideoSettings[i].szCmdFiFoPath);
			}
		}
		free(ptRtspSrvrApp->atVideoSettings);
		ptRtspSrvrApp->atVideoSettings = NULL;
	}
	
	if (ptRtspSrvrApp->atAudioSettings != NULL)
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalATrackNum; ++i)
		{
			if (ptRtspSrvrApp->atAudioSettings[i].szMechanism != NULL)
			{
				free(ptRtspSrvrApp->atAudioSettings[i].szMechanism);
			}
			if (ptRtspSrvrApp->atAudioSettings[i].szPath != NULL)
			{
				free(ptRtspSrvrApp->atAudioSettings[i].szPath);
			}
			if (ptRtspSrvrApp->atAudioSettings[i].szCmdFiFoPath != NULL)
			{
				free(ptRtspSrvrApp->atAudioSettings[i].szCmdFiFoPath);
			}
		}
		free(ptRtspSrvrApp->atAudioSettings);
		ptRtspSrvrApp->atAudioSettings = NULL;
	}

	if (ptRtspSrvrApp->atMDataSettings != NULL)
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalDTrackNum; ++i)
		{
			if (ptRtspSrvrApp->atMDataSettings[i].szMechanism != NULL)
			{
				free(ptRtspSrvrApp->atMDataSettings[i].szMechanism);
			}
			if (ptRtspSrvrApp->atMDataSettings[i].szPath != NULL)
			{
				free(ptRtspSrvrApp->atMDataSettings[i].szPath);
			}
			if (ptRtspSrvrApp->atMDataSettings[i].szCmdFiFoPath != NULL)
			{
				free(ptRtspSrvrApp->atMDataSettings[i].szCmdFiFoPath);
			}
		}
		free(ptRtspSrvrApp->atMDataSettings);
		ptRtspSrvrApp->atMDataSettings = NULL;
	}

	if (ptRtspSrvrApp->atAcsFileInfo != NULL) 
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalAcsFileNum; ++i)
		{
			if (ptRtspSrvrApp->atAcsFileInfo[i].szAccessName != NULL)
			{
				free(ptRtspSrvrApp->atAcsFileInfo[i].szAccessName);
			}
			if (ptRtspSrvrApp->atAcsFileInfo[i].szMltcstIP != NULL)
			{
				free(ptRtspSrvrApp->atAcsFileInfo[i].szMltcstIP);
			}
		}
		free(ptRtspSrvrApp->atAcsFileInfo);
		ptRtspSrvrApp->atAcsFileInfo = NULL;
	}

#ifdef __HISTORY_STREAM_SUPPORT__
	if (ptRtspSrvrApp->atUPlayerChInfo != NULL)
	{
		if (ptRtspSrvrApp->szHistAccessName != NULL)
		{
			free(ptRtspSrvrApp->szHistAccessName);
			ptRtspSrvrApp->szHistAccessName = NULL;
		}

		if (ptRtspSrvrApp->szUPlayerCommandPath != NULL)
		{
			free(ptRtspSrvrApp->szUPlayerCommandPath);
			ptRtspSrvrApp->szUPlayerCommandPath = NULL;
		}

		for (i=0; i < ptRtspSrvrApp->dwTotalUPlayerChNum; ++i)
		{
			if (ptRtspSrvrApp->atUPlayerChInfo[i].szRecvVideoUBufPath != NULL)
			{
				free(ptRtspSrvrApp->atUPlayerChInfo[i].szRecvVideoUBufPath);
			}

			if (ptRtspSrvrApp->atUPlayerChInfo[i].szRecvAudioUBufPath != NULL)
			{
				free(ptRtspSrvrApp->atUPlayerChInfo[i].szRecvAudioUBufPath);
			}
		}
		free(ptRtspSrvrApp->atUPlayerChInfo);
		ptRtspSrvrApp->atUPlayerChInfo = NULL;
	}
#endif // __HISTORY_STREAM_SUPPORT__

	return S_OK;
}

SCODE RtspSrvrApp_Destruct(TRtspSrvrApp *ptRtspSrvrApp)
{
	int i;

	if (!ptRtspSrvrApp)	return S_FAIL;
	
	if (ptRtspSrvrApp->hRtspSrvrObj != NULL)
	{
		RtspSrvr_Release(&ptRtspSrvrApp->hRtspSrvrObj);
	}

	RtspSrvrApp_ReleaseConfData(ptRtspSrvrApp);

	if (ptRtspSrvrApp->ahVideoSrcs != NULL)
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalVTrackNum; ++i)
		{
			if (ptRtspSrvrApp->ahVideoSrcs[i] != NULL)
			{
				LiveMediaSrc_Release(&(ptRtspSrvrApp->ahVideoSrcs[i]));
			}
		}
		free(ptRtspSrvrApp->ahVideoSrcs);
	}

	if (ptRtspSrvrApp->ahAudioSrcs != NULL)
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalATrackNum; ++i)
		{
			if (ptRtspSrvrApp->ahAudioSrcs[i] != NULL)
			{
				LiveMediaSrc_Release(&(ptRtspSrvrApp->ahAudioSrcs[i]));
			}
		}
		free(ptRtspSrvrApp->ahAudioSrcs);
	}
	
	if (ptRtspSrvrApp->ahMDataSrcs != NULL)
	{
		for (i=0; i < ptRtspSrvrApp->dwTotalDTrackNum; ++i)
		{
			if (ptRtspSrvrApp->ahMDataSrcs[i] != NULL)
			{
				LiveMediaSrc_Release(&(ptRtspSrvrApp->ahMDataSrcs[i]));
			}
		}
		free(ptRtspSrvrApp->ahMDataSrcs);
	}
	
	memset(ptRtspSrvrApp, 0, sizeof(TRtspSrvrApp));

	return S_OK;
}

#ifdef _QOS_
static const char * rateCtrlFlagStr(ERateCtrlFlags eRateCtrlFlag)
{
  switch(eRateCtrlFlag){
		case RATE_CTRL_VQCB:
			return "VQCB";
		case RATE_CTRL_CQCB:
			return "CQCB";
		case RATE_CTRL_CVBR:
			return "CVBR";
		case RATE_CTRL_STRICT_VQCB:
			return "VQCB";
		case RATE_CTRL_NONE:
			return "NONE";
		case RATE_CTRL_ADPT:
			return "ADPT";
  }
  return "bad rate control flag";
}
#endif

SCODE RtspSrvrApp_InitialRtspSrvrObj(TRtspSrvrApp *ptRtspSrvrApp)
{
	TRtspSrvrInitOpts	tInitOpts;
	TSockSrcInitOpts	tSockSrcInitOpts;
	TFileSrcInitOpts	tFileSrcInitOpts;
#ifdef _ENABLE_SHAREDBUFF_
	TShrdBufSrcInitOpts	tShrdBufSrcInitOpts;
#endif // _ENABLE_SHAREDBUFF_
#ifdef _ENABLE_REGSRC_
	TRegSrcInitOpts		tRegSrcInitOpts;
	CHAR				*pcRegTrackNo;
#endif // _ENABLE_REGSRC_
	PTAcsFileInfo		*aptAcsFileInfo=NULL;
	SCODE				sRet;
	int					i;
	
	if (!ptRtspSrvrApp)	return S_FAIL;

#ifdef __HISTORY_STREAM_SUPPORT__
	if ((ptRtspSrvrApp->atUPlayerChInfo != NULL) && (ptRtspSrvrApp->dwTotalUPlayerChNum > 0))
	{
		THistUPlayerInfo	*ptHistUPlayerInfo=NULL;

		ptHistUPlayerInfo = (THistUPlayerInfo *)malloc(sizeof(THistUPlayerInfo));
		if (ptHistUPlayerInfo == NULL)
		{
			printf("[%s:%s:%d] malloc fail!\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}

		ptHistUPlayerInfo->szHistAccessName = ptRtspSrvrApp->szHistAccessName;
		ptHistUPlayerInfo->szUPlayerCommandPath = ptRtspSrvrApp->szUPlayerCommandPath;
		
		ptHistUPlayerInfo->aptUPlayerChInfo = (TUPlayerChInfo **)calloc(ptRtspSrvrApp->dwTotalUPlayerChNum, sizeof(TUPlayerChInfo *));
		if (ptHistUPlayerInfo->aptUPlayerChInfo == NULL)
		{
			printf("[%s:%s:%d] calloc fail!\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}

		for (i=0; i < ptRtspSrvrApp->dwTotalUPlayerChNum; ++i)
		{
			ptHistUPlayerInfo->aptUPlayerChInfo[i] = &(ptRtspSrvrApp->atUPlayerChInfo[i]);
		}
		
		ptHistUPlayerInfo->iUPlayerChInfoNum = ptRtspSrvrApp->dwTotalUPlayerChNum;
	}
#endif // __HISTORY_STREAM_SUPPORT__

	if (ptRtspSrvrApp->dwTotalAcsFileNum > 0)
	{
		aptAcsFileInfo = (PTAcsFileInfo *)calloc(ptRtspSrvrApp->dwTotalAcsFileNum, sizeof(PTAcsFileInfo));
		if (aptAcsFileInfo == NULL)
		{
			printf("[%s:%s:%d] calloc fail!\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}

		for (i=0;i < ptRtspSrvrApp->dwTotalAcsFileNum; ++i)
		{
			aptAcsFileInfo[i] = &(ptRtspSrvrApp->atAcsFileInfo[i]);
		}
	}
	else
	{
		printf("[%s:%s:%d] No any access file availabe!\n", __func__, __FILE__, __LINE__);
		goto error_handle;
	}
	
	for (i=0;i < ptRtspSrvrApp->dwTotalVTrackNum; ++i)
	{
#ifdef _ENABLE_SHAREDBUFF_
		if (strcasecmp(ptRtspSrvrApp->atVideoSettings[i].szMechanism, "sharedbuffer") == 0)
		{
			tShrdBufSrcInitOpts.szSharedBuffer = ptRtspSrvrApp->atVideoSettings[i].szPath;
			tShrdBufSrcInitOpts.szCmdFiFoPath = ptRtspSrvrApp->atVideoSettings[i].szCmdFiFoPath;

			if (ShrdBufSrc_Initial(&(ptRtspSrvrApp->ahVideoSrcs[i]), &tShrdBufSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] ShrdBufSrc_Initial error! vidoe track %d", __func__, __FILE__, __LINE__, i); 
			}
		}
		else 
#endif // _ENABLE_SHAREDBUFF_
#ifdef _ENABLE_REGSRC_
		if (strcasecmp(ptRtspSrvrApp->atVideoSettings[i].szMechanism, "register") == 0)
		{
			pcRegTrackNo = strstr(ptRtspSrvrApp->atVideoSettings[i].szPath, "@");
			
			if (pcRegTrackNo == NULL)
			{
				printf("[%s:%s:%d] RegSrc_Initial error! vidoe track %d", __func__, __FILE__, __LINE__, i); 
			}
			else
			{
				*pcRegTrackNo = '\0';
				++pcRegTrackNo;

				tRegSrcInitOpts.szRegPath	= ptRtspSrvrApp->atVideoSettings[i].szPath;
				tRegSrcInitOpts.dwTrackNo	= (DWORD)atoi(pcRegTrackNo);
				tRegSrcInitOpts.dwMediaType	= 0;

				if (RegSrc_Initial(&(ptRtspSrvrApp->ahVideoSrcs[i]), &tRegSrcInitOpts) != S_OK)
				{
					printf("[%s:%s:%d] RegSrc_Initial error! vidoe track %d", __func__, __FILE__, __LINE__, i); 
				}
			}
		}
		else 
#endif // _ENABLE_REGSRC_
		if (strcasecmp(ptRtspSrvrApp->atVideoSettings[i].szMechanism, "socket") == 0)
		{
			tSockSrcInitOpts.szRecvSockPath = ptRtspSrvrApp->atVideoSettings[i].szPath;
			tSockSrcInitOpts.szCmdFiFoPath = ptRtspSrvrApp->atVideoSettings[i].szCmdFiFoPath;
			
			if (SockSrc_Initial(&(ptRtspSrvrApp->ahVideoSrcs[i]), &tSockSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] SockSrc_Initial error! vidoe track %d", __func__, __FILE__, __LINE__, i); 
			}
		}
		else if (strcasecmp(ptRtspSrvrApp->atVideoSettings[i].szMechanism, "file") == 0)
		{
			tFileSrcInitOpts.szFilePath = ptRtspSrvrApp->atVideoSettings[i].szPath;

			if (FileSrc_Initial(&(ptRtspSrvrApp->ahVideoSrcs[i]), &tFileSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] FileSrc_Initial error! vidoe track %d", __func__, __FILE__, __LINE__, i);
			}
		}
		else
		{
			printf("[%s:%s:%d] Warning! Inactive Video Src %d\n", __func__, __FILE__, __LINE__, i);
		}
	}

	for (i=0;i < ptRtspSrvrApp->dwTotalATrackNum; ++i)
	{
#ifdef _ENABLE_SHAREDBUFF_
		if (strcasecmp(ptRtspSrvrApp->atAudioSettings[i].szMechanism, "sharedbuffer") == 0)
		{
			tShrdBufSrcInitOpts.szSharedBuffer = ptRtspSrvrApp->atAudioSettings[i].szPath;
			tShrdBufSrcInitOpts.szCmdFiFoPath = ptRtspSrvrApp->atAudioSettings[i].szCmdFiFoPath;

			if (ShrdBufSrc_Initial(&(ptRtspSrvrApp->ahAudioSrcs[i]), &tShrdBufSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] ShrdBufSrc_Initial error! audio track %d", __func__, __FILE__, __LINE__, i); 
			}
		}
		else 
#endif // _ENABLE_SHAREDBUFF_
		if (strcasecmp(ptRtspSrvrApp->atAudioSettings[i].szMechanism, "socket") == 0)
		{
			tSockSrcInitOpts.szRecvSockPath = ptRtspSrvrApp->atAudioSettings[i].szPath;
			tSockSrcInitOpts.szCmdFiFoPath = ptRtspSrvrApp->atAudioSettings[i].szCmdFiFoPath;
			
			if (SockSrc_Initial(&(ptRtspSrvrApp->ahAudioSrcs[i]), &tSockSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] SockSrc_Initial error! audio track %d", __func__, __FILE__, __LINE__, i); 
			}
		}
		else if (strcasecmp(ptRtspSrvrApp->atAudioSettings[i].szMechanism, "file") == 0)
		{
			tFileSrcInitOpts.szFilePath = ptRtspSrvrApp->atAudioSettings[i].szPath;

			if (FileSrc_Initial(&(ptRtspSrvrApp->ahAudioSrcs[i]), &tFileSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] FileSrc_Initial error! audio track %d", __func__, __FILE__, __LINE__, i);
			}
		}
		else
		{
			printf("[%s:%s:%d] Warning! Inactive Audio Src %d\n", __func__, __FILE__, __LINE__, i);
		}
	}

	for (i=0;i < ptRtspSrvrApp->dwTotalDTrackNum; ++i)
	{
#ifdef _ENABLE_SHAREDBUFF_
		if (strcasecmp(ptRtspSrvrApp->atMDataSettings[i].szMechanism, "sharedbuffer") == 0)
		{
			tShrdBufSrcInitOpts.szSharedBuffer = ptRtspSrvrApp->atMDataSettings[i].szPath;
			tShrdBufSrcInitOpts.szCmdFiFoPath = ptRtspSrvrApp->atMDataSettings[i].szCmdFiFoPath;

			if (ShrdBufSrc_Initial(&(ptRtspSrvrApp->ahMDataSrcs[i]), &tShrdBufSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] ShrdBufSrc_Initial error! mdata track %d", __func__, __FILE__, __LINE__, i); 
			}
		}
		else 
#endif // _ENABLE_SHAREDBUFF_
		if (strcasecmp(ptRtspSrvrApp->atMDataSettings[i].szMechanism, "socket") == 0)
		{
			tSockSrcInitOpts.szRecvSockPath = ptRtspSrvrApp->atMDataSettings[i].szPath;
			tSockSrcInitOpts.szCmdFiFoPath = ptRtspSrvrApp->atMDataSettings[i].szCmdFiFoPath;
			
			if (SockSrc_Initial(&(ptRtspSrvrApp->ahMDataSrcs[i]), &tSockSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] SockSrc_Initial error! mdata track %d", __func__, __FILE__, __LINE__, i); 
			}
		}
		else if (strcasecmp(ptRtspSrvrApp->atMDataSettings[i].szMechanism, "file") == 0)
		{
			tFileSrcInitOpts.szFilePath = ptRtspSrvrApp->atMDataSettings[i].szPath;

			if (FileSrc_Initial(&(ptRtspSrvrApp->ahMDataSrcs[i]), &tFileSrcInitOpts) != S_OK)
			{
				printf("[%s:%s:%d] FileSrc_Initial error! mdata track %d", __func__, __FILE__, __LINE__, i);
			}
		}
		else
		{
			printf("[%s:%s:%d] Warning! Inactive MData Src %d\n", __func__, __FILE__, __LINE__, i);
		}
	}

	memset(&tInitOpts, 0, sizeof(TRtspSrvrInitOpts));
	tInitOpts.szSrvIPAddr				= NULL;
	tInitOpts.usSrvListenPort			= ptRtspSrvrApp->usRtspServerPort;
	tInitOpts.usBlockSize				= ptRtspSrvrApp->usBlockSize;
	tInitOpts.httpserver_type			= ptRtspSrvrApp->httpserver_type;
	tInitOpts.usRtpStartPort			= 8888;
	tInitOpts.ahLiveVSrc				= ptRtspSrvrApp->ahVideoSrcs;
	tInitOpts.iLiveVSrcNum				= ptRtspSrvrApp->dwTotalVTrackNum;
	tInitOpts.ahLiveASrc				= ptRtspSrvrApp->ahAudioSrcs;
	tInitOpts.iLiveASrcNum				= ptRtspSrvrApp->dwTotalATrackNum;
	tInitOpts.ahLiveDSrc				= ptRtspSrvrApp->ahMDataSrcs;
	tInitOpts.iLiveDSrcNum				= ptRtspSrvrApp->dwTotalDTrackNum;
	tInitOpts.aptAcsFileInfo			= aptAcsFileInfo;
	tInitOpts.iAcsFileInfoNum			= ptRtspSrvrApp->dwTotalAcsFileNum;
	tInitOpts.szAuthMode				= ptRtspSrvrApp->szAuthMode;
	tInitOpts.szRtspOverHttpFdipcPath	= ptRtspSrvrApp->szHttpFdipcPath;
#ifdef __HISTORY_STREAM_SUPPORT__
	tInitOpts.ptHistUPlayerInfo			= ptHistUPlayerInfo;
#endif // __HISTORY_STREAM_SUPPORT__
#ifdef _QOS_	
	tInitOpts.eRateCtrlFlag				= ptRtspSrvrApp->eRateCtrlFlag;
	printf("[RtspServer] ratecontrol mode:%s\n",rateCtrlFlagStr(tInitOpts.eRateCtrlFlag));
#endif
	printf("[RtspServer] listen port is %hu\n", tInitOpts.usSrvListenPort);
	printf("[RtspServer] rtp start port is %hu\n", tInitOpts.usRtpStartPort);
	printf("[RtspServer] fdipc path is %s\n", ptRtspSrvrApp->szHttpFdipcPath);
	printf("[RtspServer] authenticate mode is %s\n", ptRtspSrvrApp->szAuthMode);

	sRet = RtspSrvr_Initial(&ptRtspSrvrApp->hRtspSrvrObj, &tInitOpts);
	if (sRet != S_OK)
	{
		printf("RtspSrvr_Initial fail!\n");
		goto error_handle;
	}

#ifdef __HISTORY_STREAM_SUPPORT__
	if (ptHistUPlayerInfo != NULL)
	{
		if (ptHistUPlayerInfo->aptUPlayerChInfo != NULL)
		{
			free(ptHistUPlayerInfo->aptUPlayerChInfo);
		}
		free(ptHistUPlayerInfo);
	}
#endif // __HISTORY_STREAM_SUPPORT__
	free(aptAcsFileInfo);
	return S_OK;

error_handle:
#ifdef __HISTORY_STREAM_SUPPORT__
	if (ptHistUPlayerInfo != NULL)
	{
		if (ptHistUPlayerInfo->aptUPlayerChInfo != NULL)
		{
			free(ptHistUPlayerInfo->aptUPlayerChInfo);
		}
		free(ptHistUPlayerInfo);
	}
#endif // __HISTORY_STREAM_SUPPORT__
	if (aptAcsFileInfo != NULL)
	{
		free(aptAcsFileInfo);
	}
	return S_FAIL;
}

SCODE RtspSrvrApp_Construct(TRtspSrvrApp *ptRtspSrvrApp, const char *szConfFileName)
{
	if ((!ptRtspSrvrApp) || (!szConfFileName))	return S_FAIL;

	memset(ptRtspSrvrApp, 0, sizeof(TRtspSrvrApp));

	ptRtspSrvrApp->usRtspServerPort = RTSPSERVER_DEFAULT_PORT;

	if (RtspSrvrApp_ParseXmlFile(ptRtspSrvrApp, szConfFileName) != S_OK)
	{
		goto error_handle;
	}

	if (RtspSrvrApp_InitialRtspSrvrObj(ptRtspSrvrApp) != S_OK)
	{
		goto error_handle;
	}

	RtspSrvrApp_ReleaseConfData(ptRtspSrvrApp);

	return S_OK;

error_handle:

	RtspSrvrApp_Destruct(ptRtspSrvrApp);
	return S_FAIL;
}

SCODE RtspSrvrApp_Initial(HANDLE *phRtspSrvrApp, const char *szConfFileName)
{
	TRtspSrvrApp *ptRtspSrvrApp;

	if ((!phRtspSrvrApp) || (!szConfFileName))	return S_FAIL;

	if ((ptRtspSrvrApp=(TRtspSrvrApp *)malloc(sizeof(TRtspSrvrApp))) == NULL)
	{
		debug_printf("[%s:%s:%d] malloc error!\n", __func__, __FILE__, __LINE__);
		return S_FAIL;
	}
	*phRtspSrvrApp = ptRtspSrvrApp;

	if (RtspSrvrApp_Construct(ptRtspSrvrApp, szConfFileName) != S_OK)
	{
		free(ptRtspSrvrApp);
		*phRtspSrvrApp = NULL;
		return S_FAIL;
	}

	return S_OK;
}

SCODE RtspSrvrApp_Release(HANDLE *phRtspSrvrApp)
{
	TRtspSrvrApp *ptRtspSrvrApp;

	if ((!phRtspSrvrApp) || (!(ptRtspSrvrApp=(TRtspSrvrApp *)*phRtspSrvrApp)))	return S_FAIL;

	RtspSrvrApp_Destruct(ptRtspSrvrApp);
	free(ptRtspSrvrApp);
	*phRtspSrvrApp = NULL;

	return S_OK;
}

SCODE RtspSrvrApp_Start(HANDLE hRtspSrvrApp)
{
	TRtspSrvrApp *ptRtspSrvrApp;

	if (!(ptRtspSrvrApp=(TRtspSrvrApp *)hRtspSrvrApp))	return S_FAIL;

	if (RtspSrvr_Start(ptRtspSrvrApp->hRtspSrvrObj) != S_OK)
	{
		printf("RtspSrvr_Start fail!\n");
		return S_FAIL;
	}	

	return S_OK;
}

SCODE RtspSrvrApp_Stop(HANDLE hRtspSrvrApp)
{
	TRtspSrvrApp *ptRtspSrvrApp;

	if (!(ptRtspSrvrApp=(TRtspSrvrApp *)hRtspSrvrApp))	return S_FAIL;

	if (RtspSrvr_Stop(ptRtspSrvrApp->hRtspSrvrObj) != S_OK)
	{
		printf("RtspSrvr_Stop fail!\n");
		return S_FAIL;
	}

	printf("[%s:%s:%d]After RtspSrvr_Stop\n", __FILE__, __func__, __LINE__);

	return S_OK;
}

SCODE RtspSrvrApp_Reconfig(HANDLE hRtspSrvrApp, CHAR *szConfigFile)
{
	if (RtspSrvrApp_Stop(hRtspSrvrApp) != S_OK)
	{
		printf("RtspSrvr_Stop fail!\n");
		return S_FAIL;
	}

	if (RtspSrvrApp_Destruct((TRtspSrvrApp *)hRtspSrvrApp) != S_OK)
	{
		printf("RtspSrvr_Destruct fail!\n");
		return S_FAIL;
	}
	
	if (RtspSrvrApp_Construct((TRtspSrvrApp *)hRtspSrvrApp, szConfigFile) != S_OK)
	{
		printf("RtspSrvr_Construct fail!\n");
		return S_FAIL;
	}
	
	if (RtspSrvrApp_Start(hRtspSrvrApp) != S_OK)
	{
		printf("RtspSrvr_Start fail!\n");
		return S_FAIL;
	}

	return S_OK;
}
