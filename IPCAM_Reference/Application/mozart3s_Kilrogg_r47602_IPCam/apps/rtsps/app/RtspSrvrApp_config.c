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
 * RtspSrvrApp_config.c
 *
 * \brief
 * Rtsp Server Application Read XML config file implementation
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
#include "xmlmgr.h"

void RtspServer_xmlmgr_set_int(char *szConfValue, void *pObj)
{
	*((int *)pObj) = atoi(szConfValue);
}

void RtspServer_xmlmgr_set_string(char *szConfValue, void *pObj)
{
	*((CHAR **)pObj) = strdup(szConfValue);
}

SCODE RtspSrvrApp_ParseXmlFile(TRtspSrvrApp *ptRtspSrvrApp, const char* pzConfigFile)
{
	HANDLE				hXmlConf;
	TXmlMgrInitOptions	tInitOpt;
	CHAR				acBuffer[256];
	CHAR				*szConfValue;
	int					i;
	TXmlMgrParsingMap	tParsingMap[] = {
		{"/root/rtsp/port", xmlmgr_set_short, &ptRtspSrvrApp->usRtspServerPort},
		{"/root/rtsp/blocksize", xmlmgr_set_short, &ptRtspSrvrApp->usBlockSize},
		{"/root/rtsp/httpserver_type", xmlmgr_set_long, &ptRtspSrvrApp->httpserver_type},
		{"/root/rtsp/auth_mode", RtspServer_xmlmgr_set_string, &ptRtspSrvrApp->szAuthMode},
		{"/root/rtsp/http_fdipc_sock", RtspServer_xmlmgr_set_string, &ptRtspSrvrApp->szHttpFdipcPath},
		{"/root/rtsp/video_track/total_num", xmlmgr_set_long, &ptRtspSrvrApp->dwTotalVTrackNum},
		{"/root/rtsp/audio_track/total_num", xmlmgr_set_long, &ptRtspSrvrApp->dwTotalATrackNum},
		{"/root/rtsp/mdata_track/total_num", xmlmgr_set_long, &ptRtspSrvrApp->dwTotalDTrackNum},
		{"/root/rtsp/live_stream/total_num", xmlmgr_set_long, &ptRtspSrvrApp->dwTotalAcsFileNum},
		{NULL, 0, NULL}
	};
	
	tInitOpt.dwVersion = XMLMGR_VERSION;
	if (XmlMgr_Initial(&hXmlConf, &tInitOpt) != S_OK)
	{
		debug_printf("[%s:%s:%d] Initial xmlmgr fail\n", __func__, __FILE__, __LINE__);
		return S_FAIL;
	}

	if (XmlMgr_ReadFile(hXmlConf, pzConfigFile) != S_OK)
	{
		debug_printf("[%s:%s:%d] Open XML file %s fail\n", __func__, __FILE__, __LINE__, pzConfigFile);
		goto error_handle;
	}

	if (XmlMgr_GetMultiConfValue(hXmlConf, tParsingMap) != S_OK)
	{
		debug_printf("[%s:%s:%d] XmlMgr_GetMultiConfValue fail\n", __func__, __FILE__, __LINE__);
		goto error_handle;
	}

	if (ptRtspSrvrApp->dwTotalVTrackNum > 0)
	{
		ptRtspSrvrApp->atVideoSettings = (TLMSrcSettingInfo *)calloc(ptRtspSrvrApp->dwTotalVTrackNum, sizeof(TLMSrcSettingInfo));
		if (ptRtspSrvrApp->atVideoSettings == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
		ptRtspSrvrApp->ahVideoSrcs = (HANDLE *)calloc(ptRtspSrvrApp->dwTotalVTrackNum, sizeof(HANDLE));
		if (ptRtspSrvrApp->ahVideoSrcs == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
	}

	if (ptRtspSrvrApp->dwTotalATrackNum > 0)
	{
		ptRtspSrvrApp->atAudioSettings = (TLMSrcSettingInfo *)calloc(ptRtspSrvrApp->dwTotalATrackNum, sizeof(TLMSrcSettingInfo));
		if (ptRtspSrvrApp->atAudioSettings == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
		ptRtspSrvrApp->ahAudioSrcs = (HANDLE *)calloc(ptRtspSrvrApp->dwTotalATrackNum, sizeof(HANDLE));
		if (ptRtspSrvrApp->ahAudioSrcs == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
	}

	if (ptRtspSrvrApp->dwTotalDTrackNum > 0)
	{
		ptRtspSrvrApp->atMDataSettings = (TLMSrcSettingInfo *)calloc(ptRtspSrvrApp->dwTotalDTrackNum, sizeof(TLMSrcSettingInfo));
		if (ptRtspSrvrApp->atMDataSettings == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
		ptRtspSrvrApp->ahMDataSrcs = (HANDLE *)calloc(ptRtspSrvrApp->dwTotalDTrackNum, sizeof(HANDLE));
		if (ptRtspSrvrApp->ahMDataSrcs == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
	}

	if (ptRtspSrvrApp->dwTotalAcsFileNum > 0)
	{
		ptRtspSrvrApp->atAcsFileInfo = (TAcsFileInfo *)calloc(ptRtspSrvrApp->dwTotalAcsFileNum, sizeof(TAcsFileInfo));
		if (ptRtspSrvrApp->atAcsFileInfo == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}
	}

	for (i=0; i < ptRtspSrvrApp->dwTotalVTrackNum; ++i)
	{
		sprintf(acBuffer, "/root/rtsp/video_track/track[@no='%d']/mechanism", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if ((szConfValue != NULL) && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atVideoSettings[i].szMechanism = strdup(szConfValue);
		}
		printf("video track%d use mechanism: %s\n", i, ptRtspSrvrApp->atVideoSettings[i].szMechanism);

		sprintf(acBuffer, "/root/rtsp/video_track/track[@no='%d']/path", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if ((szConfValue != NULL) && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atVideoSettings[i].szPath = strdup(szConfValue);
		}
		printf("video track%d path: %s\n", i, ptRtspSrvrApp->atVideoSettings[i].szPath);

		sprintf(acBuffer, "/root/rtsp/video_track/track[@no='%d']/cmd_fifo", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		//printf("cmd_fifo is %s\n", szConfValue);
		if ((szConfValue != NULL)  && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atVideoSettings[i].szCmdFiFoPath = strdup(szConfValue);
		}
		printf("video track%d cmd fifo: %s\n", i, ptRtspSrvrApp->atVideoSettings[i].szCmdFiFoPath);
	}

	for (i=0; i < ptRtspSrvrApp->dwTotalATrackNum; ++i)
	{
		sprintf(acBuffer, "/root/rtsp/audio_track/track[@no='%d']/mechanism", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if ((szConfValue != NULL) && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atAudioSettings[i].szMechanism = strdup(szConfValue);
		}
		printf("audio track%d use mechanism: %s\n", i, ptRtspSrvrApp->atAudioSettings[i].szMechanism);

		sprintf(acBuffer, "/root/rtsp/audio_track/track[@no='%d']/path", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->atAudioSettings[i].szPath = strdup(szConfValue);
		}
		printf("audio track%d path: %s\n", i, ptRtspSrvrApp->atAudioSettings[i].szPath);

		sprintf(acBuffer, "/root/rtsp/audio_track/track[@no='%d']/cmd_fifo", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if ((szConfValue != NULL)  && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atAudioSettings[i].szCmdFiFoPath = strdup(szConfValue);
		}
		printf("audio track%d cmd fifo: %s\n", i, ptRtspSrvrApp->atAudioSettings[i].szCmdFiFoPath);
	}

	for (i=0; i < ptRtspSrvrApp->dwTotalDTrackNum; ++i)
	{
		sprintf(acBuffer, "/root/rtsp/mdata_track/track[@no='%d']/mechanism", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if ((szConfValue != NULL) && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atMDataSettings[i].szMechanism = strdup(szConfValue);
		}
		printf("mdata track%d use mechanism: %s\n", i, ptRtspSrvrApp->atMDataSettings[i].szMechanism);

		sprintf(acBuffer, "/root/rtsp/mdata_track/track[@no='%d']/path", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->atMDataSettings[i].szPath = strdup(szConfValue);
		}
		printf("mdata track%d path: %s\n", i, ptRtspSrvrApp->atMDataSettings[i].szPath);

		sprintf(acBuffer, "/root/rtsp/mdata_track/track[@no='%d']/cmd_fifo", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if ((szConfValue != NULL)  && (strcasecmp(szConfValue, "N.A.") != 0))
		{
			ptRtspSrvrApp->atMDataSettings[i].szCmdFiFoPath = strdup(szConfValue);
		}
		printf("mdata track%d cmd fifo: %s\n", i, ptRtspSrvrApp->atMDataSettings[i].szCmdFiFoPath);
	}

	for (i=0; i < ptRtspSrvrApp->dwTotalAcsFileNum; ++i)
	{
		sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/accessname", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->atAcsFileInfo[i].szAccessName = strdup(szConfValue);
		}

		sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/videotrack", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->atAcsFileInfo[i].iVTrackNo = atoi(szConfValue);
		}

		sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/audiotrack", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->atAcsFileInfo[i].iATrackNo = atoi(szConfValue);
		}

		sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/mdatatrack", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->atAcsFileInfo[i].iDTrackNo = atoi(szConfValue);
		}

		sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast", i);
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast/ipaddress", i);
			szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
			if (szConfValue != NULL)
			{
				ptRtspSrvrApp->atAcsFileInfo[i].szMltcstIP = strdup(szConfValue);

				sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast/enable", i);
				szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
				if (szConfValue != NULL)
				{				    
					ptRtspSrvrApp->atAcsFileInfo[i].bMltcstEnable = (strcasecmp(szConfValue, "true") == 0)? TRUE: FALSE;
				}

				sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast/ttl", i);
				szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
				if (szConfValue != NULL)
				{
					ptRtspSrvrApp->atAcsFileInfo[i].byMltcstTTL = (BYTE)atoi(szConfValue);
				}
				
				sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast/videoport", i);
				szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
				if (szConfValue != NULL)
				{
					ptRtspSrvrApp->atAcsFileInfo[i].usMltcstVideoPort = (USHORT)atoi(szConfValue);
				}
				sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast/audioport", i);
				szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
				if (szConfValue != NULL)
				{
					ptRtspSrvrApp->atAcsFileInfo[i].usMltcstAudioPort = (USHORT)atoi(szConfValue);
				}
				sprintf(acBuffer, "/root/rtsp/live_stream/stream[@no='%d']/multicast/mdataport", i);
				szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
				if (szConfValue != NULL)
				{
					ptRtspSrvrApp->atAcsFileInfo[i].usMltcstMDataPort = (USHORT)atoi(szConfValue);
				}
			}
		}
		
		printf("stream%d access file: %s, video track %d, audio track %d, mdata track %d\n", 
				i, ptRtspSrvrApp->atAcsFileInfo[i].szAccessName, 
				ptRtspSrvrApp->atAcsFileInfo[i].iVTrackNo, 
				ptRtspSrvrApp->atAcsFileInfo[i].iATrackNo,
				ptRtspSrvrApp->atAcsFileInfo[i].iDTrackNo);
		if (ptRtspSrvrApp->atAcsFileInfo[i].szMltcstIP != NULL)
		{
			printf("multicst ip %s, ttl %hhu, video port %hu, audio port %hu, mdata port %hu\n",
					ptRtspSrvrApp->atAcsFileInfo[i].szMltcstIP,
					ptRtspSrvrApp->atAcsFileInfo[i].byMltcstTTL,
					ptRtspSrvrApp->atAcsFileInfo[i].usMltcstVideoPort,
					ptRtspSrvrApp->atAcsFileInfo[i].usMltcstAudioPort,
					ptRtspSrvrApp->atAcsFileInfo[i].usMltcstMDataPort);
		}
	}
#ifdef __HISTORY_STREAM_SUPPORT__
	ptRtspSrvrApp->dwTotalUPlayerChNum = 0;
	sprintf(acBuffer, "/root/rtsp/history_stream/total_num");
	szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
	if (szConfValue != NULL)
	{
		ptRtspSrvrApp->dwTotalUPlayerChNum = (DWORD)atoi(szConfValue);
	}

	if (ptRtspSrvrApp->dwTotalUPlayerChNum > 0)
	{
		ptRtspSrvrApp->atUPlayerChInfo = (TUPlayerChInfo *)calloc(ptRtspSrvrApp->dwTotalUPlayerChNum, sizeof(TUPlayerChInfo));
		if (ptRtspSrvrApp->atUPlayerChInfo == NULL)
		{
			debug_printf("[%s:%s:%d] calloc fail\n", __func__, __FILE__, __LINE__);
			goto error_handle;
		}

		sprintf(acBuffer, "/root/rtsp/history_stream/accessname");
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->szHistAccessName = strdup(szConfValue);
		}

		sprintf(acBuffer, "/root/rtsp/history_stream/command_path");
		szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
		if (szConfValue != NULL)
		{
			ptRtspSrvrApp->szUPlayerCommandPath = strdup(szConfValue);
		}

		for (i=0; i < ptRtspSrvrApp->dwTotalUPlayerChNum; ++i)
		{
			sprintf(acBuffer, "/root/rtsp/history_stream/stream[@no='%d']/uplayer_id", i);
			szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
			if (szConfValue != NULL)
			{
				ptRtspSrvrApp->atUPlayerChInfo[i].dwUPlayerId = atoi(szConfValue);
			}

			sprintf(acBuffer, "/root/rtsp/history_stream/stream[@no='%d']/video_path", i);
			szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
			if ((szConfValue != NULL) && (strcasecmp(szConfValue, "N.A.") != 0))
			{
				ptRtspSrvrApp->atUPlayerChInfo[i].szRecvVideoUBufPath = strdup(szConfValue);
			}

			sprintf(acBuffer, "/root/rtsp/history_stream/stream[@no='%d']/audio_path", i);
			szConfValue = XmlMgr_GetConfValue(hXmlConf, acBuffer);
			if ((szConfValue != NULL) && (strcasecmp(szConfValue, "N.A.") != 0))
			{
				ptRtspSrvrApp->atUPlayerChInfo[i].szRecvAudioUBufPath = strdup(szConfValue);
			}
		}
	}
#endif // __HISTORY_STREAM_SUPPORT__
	XmlMgr_Release(&hXmlConf);
	printf("[%s:%s:%d] config file parse OK!!\r\n", __func__, __FILE__, __LINE__);
	return S_OK;
error_handle:
	XmlMgr_Release(&hXmlConf);
	return S_FAIL;
}

