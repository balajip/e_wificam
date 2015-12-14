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
 * vencenv.c
 *
 * \brief
 * vencenv cgi implement
 *
 * \date
 * 2011/07/25
 *
 * \author
 * Acer Tsai
 *
 *******************************************************************************
 */

#include "vencenv.h"
#define VENC_CONF  "/etc/conf.d/c0/venc_conf.xml"
#define SENSOR_DEP_CONF "/etc/conf.d/c0/sensor_dependent.xml"
#define RTSP_CONF "/etc/conf.d/stream_server_config.xml"
#define SERVERPUSH_CONF "/etc/conf.d/serverpush_conf.xml"
#define VREC_CONF "/etc/conf.d/vrec_conf.xml"

/* =========================================================================================== */
void GetData(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  char *pch = (char *)hObject;
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  strncpy(pch, szTemp, len+1);
}

/* =========================================================================================== */
void GetStreamNum(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  int *piData = (int *)hObject;
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  *piData = strtoul(szTemp, NULL, 10);
}

/* =========================================================================================== */
int VencEnv_GETMethodProcess(void)
{
    HANDLE hXMLWrapperObj;
    TXmlWrapperInitOptions tInitOptions;
	char szSupportVideoCodec[128];
	char szSensorType[32];
	char szResolution[32];
	char szTempBuf[128];
	int  iStreamNum = 0;
    EERRCase eErr;
    
	TXmlWrapperTreeMap ptVideoTreeMap[] =
    {
      {"root/venc_encoder/support_codectype", NULL, &GetData, NULL},
      {NULL,NULL,NULL}
    };      

	TXmlWrapperTreeMap ptSensorTypeTreeMap[] = 
	{
		{"root/sensor_type", NULL, &GetData, NULL},
		{NULL,NULL,NULL}
	};
	memset(szSupportVideoCodec, 0, 128);
	memset(szTempBuf, 0, 128);
	memset(szSensorType, 0, 32);
	memset(szResolution, 0, 32);

    tInitOptions.dwVersion = XMLWRAPPER_VERSION;
    if (XmlWrapper_Initial(&hXMLWrapperObj, &tInitOptions) != S_OK) {
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

   /* Get video setting */    
    if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptVideoTreeMap,NULL) != S_OK) {
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

    if (XmlWrapper_ReadFile_UsrDefFunc(VENC_CONF, hXMLWrapperObj, szSupportVideoCodec) != S_OK) { 
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

   /* Get sensor type */
    if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptSensorTypeTreeMap, NULL) != S_OK) {
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

    if (XmlWrapper_ReadFile_UsrDefFunc(SENSOR_DEP_CONF, hXMLWrapperObj, szSensorType) != S_OK) { 
        eErr = eError_XmlWrapperFail;
        goto get_error_case;
    }

		/* Get Max resolution */
		sprintf(szTempBuf, "root/sensor_dependent_info/%s/max_resolution/support_resolution", szSensorType);
	{
		TXmlWrapperTreeMap ptResolutionTreeMap[] = 
		{
			{szTempBuf ,NULL, &GetData, NULL},
			{NULL,NULL,NULL}
		};
		if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto get_error_case;
    	}

    	if (XmlWrapper_SetHandler(hXMLWrapperObj, ptResolutionTreeMap, NULL) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto get_error_case;
    	}

        if (XmlWrapper_ReadFile_UsrDefFunc(SENSOR_DEP_CONF, hXMLWrapperObj, szResolution) != S_OK) { 
            eErr = eError_XmlWrapperFail;
            goto get_error_case;
    	}
	}

	{
		TXmlWrapperTreeMap ptStreamNumTreeMap[] = 
		{
      		{"root/venc_encoder/total_num", NULL, &GetStreamNum, NULL},
			{NULL,NULL,NULL}
		};
		if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto get_error_case;
    	}

    	if (XmlWrapper_SetHandler(hXMLWrapperObj, ptStreamNumTreeMap, NULL) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto get_error_case;
    	}

        if (XmlWrapper_ReadFile_UsrDefFunc(VENC_CONF, hXMLWrapperObj, &iStreamNum) != S_OK) { 
            eErr = eError_XmlWrapperFail;
            goto get_error_case;
    	}
	}

	XmlWrapper_Release(&hXMLWrapperObj);

	// generate the response
	printf("Content-type: text/xml\r\n");
    printf("Cache-Control: no-cache\r\n\n");
	printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	printf("<root>\n");
	printf("<total_num>%d</total_num>\n", iStreamNum);
	printf("<support_codectype>%s</support_codectype>\n", szSupportVideoCodec);
//	printf("<support_resolution>%s</support_resolution>\n", szResolution);
	printf("</root>");
	return 0;

get_error_case:
	printf("Content-type: text/xml\r\n");
    printf("Cache-Control: no-cache\r\n\n");
	printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	printf("<root>\n");
    printf("<result>1</result>\n");
    printf("<content>Fail...</content>\n");
	printf("</root>");
    return 0;
}


/* =========================================================================================== */
void GetPostCodec(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  struct post_data *pstrData = (struct post_data *)hObject;
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  strncpy(pstrData->szSupportCodec, szTemp, len+1);
}

/* =========================================================================================== */
/*
void GetPostRes(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  struct post_data *pstrData = (struct post_data *)hObject;
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  strncpy(pstrData->szResolution, szTemp, len+1);
}
*/
/* =========================================================================================== */
void GetPostTotalNum(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  struct post_data *pstrData = (struct post_data *)hObject;
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  strncpy(pstrData->szStreamNum, szTemp, len+1);
}

/* =========================================================================================== */
int VencEnv_POSTMethodProcess(void)
{
	HANDLE hXMLObject;

    DWORD dwCntLength = atoi(getenv("CONTENT_LENGTH"));
	CHAR 	*szInputStr;
	struct post_data str_postdata;
    EERRCase eErr;
	BYTE	*pbyTemp;
	
	if (dwCntLength <= 0) {
        eErr = eError_ContentLengthFail;
        goto post_error_case;
	}

	szInputStr = (CHAR *)malloc(dwCntLength+1);
    if (szInputStr == NULL) {
        eErr = eError_MemoryNotEnough;
        goto post_error_case;
    }
	memset(szInputStr, 0, dwCntLength);
	szInputStr[dwCntLength] = '\0';
    pbyTemp = szInputStr;
	
	// read data from stdin, that means from client upload
	fread(szInputStr, dwCntLength, 1, stdin);

#if 0
    {
        FILE *fpDumpFile = fopen("/tmp/postdata.log", "w");
    	fwrite(szInputStr, dwCntLength, 1, fpDumpFile);
    	fclose(fpDumpFile);
    }
#endif
	// get the post data
	{	
		HANDLE hXmlWrapperObj;
		TXmlWrapperInitOptions tXmlWrapperInitOptions;
		
	    TXmlWrapperTreeMap ptMap[] = 
		{	
			{"root/total_num", NULL, &GetPostTotalNum, NULL},
			{"root/support_codectype", NULL, &GetPostCodec, NULL},
//			{"root/support_resolution", NULL, &GetPostRes, NULL},
	    	{NULL, NULL, NULL}
		};
		
		tXmlWrapperInitOptions.dwVersion = XMLWRAPPER_VERSION;
		if (XmlWrapper_Initial(&hXmlWrapperObj, &tXmlWrapperInitOptions) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto post_error_case;
		}
		if (XmlWrapper_SetHandler(hXmlWrapperObj, ptMap, NULL) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto post_error_case;
		}
	
		if (XmlWrapper_ReadBuf_UsrDefFunc(szInputStr, hXmlWrapperObj, &str_postdata) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto post_error_case;
		}
	
		if (XmlWrapper_Release(&hXmlWrapperObj) != S_OK) {
            eErr = eError_XmlWrapperFail;
            goto post_error_case;
		}
	}

    // all data in str_postdata structure.
    // process stream total number
	{
    	char szTempBuf[128];
	    TXmlMgrInitOptions tXmlMgrInitOpt;
        memset(&tXmlMgrInitOpt, 0, sizeof(TXmlMgrInitOptions));
        
		tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
		if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
            eErr = eError_XmlMgrFail;
            goto post_error_case;
		}

	    // store stream num in venc_conf.xml file
		if (XmlMgr_ReadFile(hXMLObject, VENC_CONF) == S_OK) {
    		XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/total_num", str_postdata.szStreamNum);
    //		XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/support_codectype", str_postdata.szSupportCodec);
    		if (XmlMgr_WriteFile(hXMLObject, VENC_CONF) != S_OK) {
                eErr = eError_XmlMgrFail;
                goto post_error_case;
    		}
		}
        XmlMgr_Release(&hXMLObject);
        
	    // store stream num in rtsp config file
		tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
		if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
            eErr = eError_XmlMgrFail;
            goto post_error_case;
		}

		if (XmlMgr_ReadFile(hXMLObject, RTSP_CONF) == S_OK) {
    		XmlMgr_SetConfValue(hXMLObject, "/root/rtsp/video_track/total_num", str_postdata.szStreamNum);
    		XmlMgr_SetConfValue(hXMLObject, "/root/rtsp/live_stream/total_num", str_postdata.szStreamNum);
    
    		if (XmlMgr_WriteFile(hXMLObject, RTSP_CONF) != S_OK) {
                eErr = eError_XmlMgrFail;
                goto post_error_case;
    		}
		}
        XmlMgr_Release(&hXMLObject);
#if 1
	    // store stream num in serverpush config file
        memset(szTempBuf, 0, 128);
        sprintf(szTempBuf, "sed -i 's/^.*<tracknum>.*/\t<tracknum>%s<\\/tracknum>/g' /etc/conf.d/serverpush_conf.xml", str_postdata.szStreamNum);
        system(szTempBuf);
#else
        // xmlmgr method can't work now.
		tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
		if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
            eErr = eError_XmlMgrFail;
            goto post_error_case;
		}
		if (XmlMgr_ReadFile(hXMLObject, SERVERPUSH_CONF) == S_OK) {
			XmlMgr_SetConfValue(hXMLObject, "/root/server_push/tracknum", str_postdata.szStreamNum);

			if (XmlMgr_WriteFile(hXMLObject, SERVERPUSH_CONF) != S_OK) {
	            eErr = eError_XmlMgrFail;
	            goto post_error_case;
			}
		}
        XmlMgr_Release(&hXMLObject);
#endif
        
	    // store stream num in vrec config file
		tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
		if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
            eErr = eError_XmlMgrFail;
            goto post_error_case;
		}
		if (XmlMgr_ReadFile(hXMLObject, VREC_CONF) == S_OK) {
    	    memset(szTempBuf, 0, 128);
		    sprintf(szTempBuf, "/dev/buff_mgr%s", str_postdata.szStreamNum);
			XmlMgr_SetConfValue(hXMLObject, "/root/video_bitstream/path", szTempBuf);

    	    memset(szTempBuf, 0, 128);
		    sprintf(szTempBuf, "/tmp/venc/c0/command.fifo@%d", atoi(str_postdata.szStreamNum)-1);
			XmlMgr_SetConfValue(hXMLObject, "/root/video_bitstream/cmd_fifo", szTempBuf);

			if (XmlMgr_WriteFile(hXMLObject, VREC_CONF) != S_OK) {
	            eErr = eError_XmlMgrFail;
	            goto post_error_case;
			}		
		}
		XmlMgr_Release(&hXMLObject);
	}
    // process codec parts
	{
	    TXmlMgrInitOptions tXmlMgrInitOpt;
        memset(&tXmlMgrInitOpt, 0, sizeof(TXmlMgrInitOptions));
        
		tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
		if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
            eErr = eError_XmlMgrFail;
            goto post_error_case;
		}
	
	    // store codec in venc_conf.xml file
		if (XmlMgr_ReadFile(hXMLObject, VENC_CONF) == S_OK) {
    		XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/support_codectype", str_postdata.szSupportCodec);
    		if (XmlMgr_WriteFile(hXMLObject, VENC_CONF) != S_OK) {
                eErr = eError_XmlMgrFail;
                goto post_error_case;
    		}
		}
		XmlMgr_Release(&hXMLObject);

		// check if the encoder codec is in the codec set, if no, assign one into the codec.
        {
            CHAR *delim = ",";
            CHAR *p = NULL;
            char *codec; 
            DWORD dwVideoCodecInitMask = 0, dwEncoderCodec = 0;
            char szTempBuf[64];
            int i;
            
            p = strtok(str_postdata.szSupportCodec, delim);
            while (p != NULL) {
                if(strncmp(p, "mjpeg", strlen(p)) == 0) {
                    dwVideoCodecInitMask |= VIDEO_CODEC_JPEG_INIT_BIT;
                } else if(strncmp(p, "mpeg4", strlen(p)) == 0) {
                    dwVideoCodecInitMask |=  VIDEO_CODEC_MP4V_INIT_BIT;
                } else if(strncmp(p, "h264", strlen(p)) == 0) {
                    dwVideoCodecInitMask |=  VIDEO_CODEC_H264_INIT_BIT;
                } else if(strncmp(p, "svc", strlen(p))==0) {
                    dwVideoCodecInitMask |=  VIDEO_CODEC_SVC_INIT_BIT;
                }
                p = strtok(NULL, delim);
            }

            for (i=0; i<4; i++) {
                memset(szTempBuf, 0, 64);
		        sprintf(szTempBuf, "/etc/conf.d/c0/vencencoder_conf.%d.xml", i);

        		tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
        		if (XmlMgr_Initial(&hXMLObject, &tXmlMgrInitOpt) != S_OK) {
                    eErr = eError_XmlMgrFail;
                    goto post_error_case;
        		}

        		if (XmlMgr_ReadFile(hXMLObject, szTempBuf) == S_OK) {
    		        codec = XmlMgr_GetConfValue(hXMLObject, "/root/venc_encoder/codec");
                    if(strncmp(codec, "mjpeg", strlen(codec)) == 0) {
                        dwEncoderCodec = VIDEO_CODEC_JPEG_INIT_BIT;
                    } else if(strncmp(codec, "mpeg4", strlen(codec)) == 0) {
                        dwEncoderCodec = VIDEO_CODEC_MP4V_INIT_BIT;
                    } else if(strncmp(codec, "h264", strlen(codec)) == 0) {
                        dwEncoderCodec = VIDEO_CODEC_H264_INIT_BIT;
                    } else if(strncmp(codec, "svc", strlen(codec))==0) {
                        dwEncoderCodec = VIDEO_CODEC_SVC_INIT_BIT;
                    }
    
    		        if ((dwVideoCodecInitMask & dwEncoderCodec) == 0) {
    		            if ((dwVideoCodecInitMask & VIDEO_CODEC_H264_INIT_BIT) != 0) {
    		                XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/codec", "h264");
    		            } else if ((dwVideoCodecInitMask & VIDEO_CODEC_SVC_INIT_BIT) != 0) {
    		                XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/codec", "svc");
    		            } else if ((dwVideoCodecInitMask & VIDEO_CODEC_JPEG_INIT_BIT) != 0) {
    		                XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/codec", "mjpeg");
    		            } else if ((dwVideoCodecInitMask & VIDEO_CODEC_MP4V_INIT_BIT) != 0) {
    		                XmlMgr_SetConfValue(hXMLObject, "/root/venc_encoder/codec", "mpeg4");
                        }
                		if (XmlMgr_WriteFile(hXMLObject, szTempBuf) != S_OK) {
                            eErr = eError_XmlMgrFail;
                            goto post_error_case;
                		}
    		        } // if != 0, means the codec is in the codec set.
        		}
        		XmlMgr_Release(&hXMLObject);		
            }
        }
	}

	printf("Content-type: text/xml\r\n");
    printf("Cache-Control: no-cache\r\n\n");
	printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	printf("<root>\n");
    printf("<result>0</result>\n");
    printf("<content>Successful, it will reboot. please wait...</content>\n");
	printf("</root>");

    if (pbyTemp != NULL)
        free(pbyTemp);
    return 0;

post_error_case:
    	printf("Content-type: text/xml\r\n");
        printf("Cache-Control: no-cache\r\n\n");
    	printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    	printf("<root>\n");
        printf("<result>1</result>\n");

		if (eErr == eError_ContentLengthFail) {
            printf("<content>Fail, Content Length is 0. no effect.</content>\n");
		} else if (eErr == eError_MemoryNotEnough) {
            printf("<content>Fail, malloc fail. no effect.</content>\n");
		} else if (eErr == eError_XmlWrapperFail) {
            printf("<content>Fail, XmlWrapper library parse fail.</content>\n");
		} else if (eErr == eError_XmlMgrFail) {
            printf("<content>Fail, XmlMgr library parse fail.</content>\n");
		}
    	printf("</root>");
        return 0;
}
/* ============================================================================================= */
int main ( int argc, char *argv[] )
{
    char  szlength[8];
    memset(szlength, 0, 8);
    strcpy(szlength, getenv("REQUEST_METHOD"));
    
    if (strcmp(szlength, "GET") == 0) {
        VencEnv_GETMethodProcess();
    } else if (strcmp(szlength, "POST") == 0) {
        VencEnv_POSTMethodProcess();
        system("mv /etc/rcK.d/K73network /etc/rcK.d/K73network~");
        system("mv /etc/rcK.d/K50http /etc/rcK.d/K50http~");
        sleep(1);
        system("/usr/sbin/reboot");
    } else {
    	// generate the error response
    	printf("Content-type: text/xml\r\n");
        printf("Cache-Control: no-cache\r\n\n");
    	printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    	printf("<root>\n");
        printf("<result>1</result>\n");
        printf("<content>The request method doesn't supported. no effect.</content>\n");
    	printf("</root>");
    }
	return 0;
}


