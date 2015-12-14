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
 * getparam.c
 *
 * \brief
 * getparam cgi implement
 *
 * \date
 * 2009/11/09
 *
 * \author
 * Acer Tsai
 *
 *******************************************************************************
 */

#include "getparam.h"
//#define _HAVE_SERVERPUSH
#define SENDER_BUFFER_LENGTH 1024
#define RTSP_CONF "/etc/conf.d/stream_server_config.xml"
#define SERVERPUSH_CONF "/etc/conf.d/serverpush_conf.xml"
#define SIP_CONF "/etc/conf.d/sipuas_conf.xml"
//#define VENCENCODER_0_CONF "/etc/conf.d/c0/vencencoder_conf.0.xml"
//#define VENCENCODER_1_CONF "/etc/conf.d/c0/vencencoder_conf.1.xml"
//#define VENCENCODER_2_CONF "/etc/conf.d/c0/vencencoder_conf.2.xml"
#define SYSTEM_CONF		 "/etc/conf.d/system.xml"
#define VENC_CONF  "/etc/conf.d/c0/venc_conf.xml"
#define AENC_CONF  "/etc/conf.d/aenc_conf.xml"
//#define STREAM_NUM			3
#define VENCMASTER_CONF "/etc/conf.d/c0/vencmaster_conf.xml"
#define KERNEL_LOG  "/tmp/kernel_mem.log"
#define QUERY_KERNEL_SIZE "/usr/bin/fw_printenv bootargs | sed 's/^.*mem=//g' | sed 's/M.*$//g'"
#define AUDIO_DEVICE_LOG  "/tmp/audiodevnum.log"
#define AUDIO_DEVICE_NUM "/bin/cat /usr/local/firmware/.config  |  grep CONFIG_AUDIO_DEVICE_NUM | awk -F'=' '{ print $2}'"
static DWORD dwTrackNo;     // Record the currnet TrackNo
static DWORD dwStreamNum=0;
static CHAR szAudioDeviceID[128];
static char szSupportVideoCodec[128];
static char szSupportAudioCodec[128];

static DWORD dwCapWidth=0;
static DWORD dwCapHeight=0;

static DWORD dwCropWidth=0;
static DWORD dwCropHeight=0;
static DWORD dwCropTop=0;
static DWORD dwCropLeft=0;

/* ============================================================================================= */
char szOutput[SENDER_BUFFER_LENGTH];

/* =========================================================================================== */


void GetCapResolution(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    char    *szToken = NULL;
    memset(szTemp,0x0,sizeof(szTemp));
    strncpy(szTemp, szData, len);
    dwCapWidth = strtol(szTemp, &szToken, 10);
    dwCapHeight = strtol(szToken+1, (char**) NULL, 10);
    printf("videoin_c0_cap_width='%d'\x0a",dwCapWidth);
    printf("videoin_c0_cap_height='%d'\x0a",dwCapHeight);
}

void GetCropTop(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    DWORD sn= *(DWORD*)(hObject);
    memset(szTemp,0x0,sizeof(szTemp));
    strncpy(szTemp, szData, len);
    dwCropTop=strtoul(szTemp, NULL, 10);
    printf("videoin_c0_s%u_crop_top='%d'\x0a",sn,dwCropTop);
}
void GetCropLeft(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    DWORD sn= *(DWORD*)(hObject);
    memset(szTemp,0x0,sizeof(szTemp));
    strncpy(szTemp, szData, len);
    dwCropLeft=strtoul(szTemp, NULL, 10);
    printf("videoin_c0_s%u_crop_left='%d'\x0a",sn,dwCropLeft);
  
}
void GetCropWidth(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    DWORD sn= *(DWORD*)(hObject);
    memset(szTemp,0x0,sizeof(szTemp));
    strncpy(szTemp, szData, len);
    dwCropWidth=strtoul(szTemp, NULL, 10);
    printf("videoin_c0_s%u_crop_width='%d'\x0a",sn,dwCropWidth);
  
}
void GetCropHeight(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    DWORD sn= *(DWORD*)(hObject);
    memset(szTemp,0x0,sizeof(szTemp));
    strncpy(szTemp, szData, len);
    dwCropHeight=strtoul(szTemp, NULL, 10);
    printf("videoin_c0_s%u_crop_height='%d'\x0a",sn,dwCropHeight);
  
}



void GetSystemName(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    snprintf(szTemp, len+1, "%s", szData);
    printf("system_info_modelname='%s'\x0a", szTemp);
}


void GetStreamNum(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  dwStreamNum=strtol(szTemp,NULL,10);
  printf("videoin_c0_streamnum='%s'\x0a",szTemp);

}

void GetSupportVideoCodecType(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  strncpy(szSupportVideoCodec,szTemp,len+1);
  szSupportVideoCodec[len+1]='\0';
  printf("videoin_c0_support_codectype='%s'\x0a",szTemp);
}
SCODE GetAudioDeviceID(HANDLE hObject, const CHAR* szData, const CHAR** atts)
{
    memset(szAudioDeviceID,0x0,sizeof(szAudioDeviceID));
    if (!strcmp(atts[0], "id")) {
	
        strncpy(szAudioDeviceID,atts[1],sizeof(szAudioDeviceID));
	//printf("(%d)szAudioDeviceID=%s\n",__LINE__,szAudioDeviceID);
    } else {
        fprintf(stderr, "[Config] Error! NOT giving no attribute! \n");
        return IGNORE_CHILD_CONFIG;
    }
    return S_OK;
}

void GetSupportAudioCodecType(HANDLE hObject, const CHAR* szData, SDWORD len)
{
  char szTemp[len+1];
  snprintf(szTemp, len+1, "%s", szData);
  strncpy(szSupportAudioCodec,szTemp,len+1);
  szSupportAudioCodec[len+1]='\0';
  //printf("(%d)szAudioDeviceID=%s\n",__LINE__,szAudioDeviceID);
  printf("audioin_%s_support_codectype='%s'\x0a",szAudioDeviceID,szTemp);

}
/* ============================================================================================= */
void GetCodec(HANDLE hObject, const CHAR *szData, SDWORD len)
{
    char szTemp[len+1];
    DWORD sn= *(DWORD*)(hObject);
    snprintf(szTemp, len+1, "%s", szData);
    printf("videoin_c0_s%u_codectype='%s'\x0a", sn, szTemp);
}

/* ============================================================================================= */
void GetResolution(HANDLE hObject, const CHAR *szData, SDWORD len)
{
    char szTemp[len+1];
    DWORD sn= *(DWORD*)(hObject);
    snprintf(szTemp, len+1, "%s", szData);

    printf("videoin_c0_s%u_resolution='%s'\x0a", sn, szTemp);
}

/* =========================================================================================== */
SCODE GetServerPushTrackNo(HANDLE hObject, const CHAR* szData, const CHAR** atts)
{
    if (!strcmp(atts[0], "TrackNo")) {
        dwTrackNo = atoi(atts[1]);
	
        if (dwTrackNo >= dwStreamNum) {
            return IGNORE_CHILD_CONFIG;
        }	
    } else {
        fprintf(stderr, "[Config] Error! NOT giving TrackNo attribute! \n");
        return IGNORE_CHILD_CONFIG;
    }
    return S_OK;
}

/* =========================================================================================== */
void GetServerPushAccessName(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    snprintf(szTemp, len+1, "%s", szData);
    printf("network_http_s%u_accessname='%s'\x0a", dwTrackNo, szTemp);
}

void GetSipPort(HANDLE hObject, const CHAR *szData, SDWORD len)
{
	char szTemp[len+1];
	char * pch;
	snprintf(szTemp, len+1, "%s", szData);
	  
	pch = strtok (szTemp,"@");
	pch = strtok (NULL, ":");
	pch = strtok (NULL, ":");

	if (pch == NULL) {
		printf("network_sip_port='5060'\x0a");	
	} else {
		printf("network_sip_port='%s'\x0a", pch);
	}
}

/* =========================================================================================== */
SCODE GetRtspTrackNo(HANDLE hObject, const CHAR* szData, const CHAR** atts)
{
    if (!strcmp(atts[0], "no")) {
        dwTrackNo = atoi(atts[1]);
	
        if (dwTrackNo >= dwStreamNum) {
            return IGNORE_CHILD_CONFIG;
        }
	
    } else {
        fprintf(stderr, "[Config] Error! NOT giving no attribute! \n");
        return IGNORE_CHILD_CONFIG;
    }
    return S_OK;
}

/* =========================================================================================== */
void GetRtspAccessName(HANDLE hObject, const CHAR* szData, SDWORD len)
{
    char szTemp[len+1];
    snprintf(szTemp, len+1, "%s", szData);
    printf("network_rtsp_s%u_accessname='%s'\x0a", dwTrackNo, szTemp);
}

/* ============================================================================================= */
int main ( int argc, char *argv[] )
{
    HANDLE hXMLWrapperObj;
    TXmlWrapperInitOptions tInitOptions;
    DWORD sn;
    int i=0;
    char szEncoderConfPath[128];
    FILE *fp=NULL;
    DWORD dwAudioDevNum=0;
#ifdef _APPS_SYSMGR
    TXmlWrapperTreeMap ptSystemTreeMap[] =
    {
	    {"/root/system/host", NULL, &GetSystemName, NULL},
    	{NULL, NULL, NULL}
    };
#endif    
#ifdef _HAVE_SERVERPUSH
    TXmlWrapperTreeMap ptServerPushTreeMap[] =
    {
	    {"/root/server_push/video", &GetServerPushTrackNo, NULL, NULL},
	    {"/root/server_push/video/accessname", NULL, &GetServerPushAccessName, NULL},
    	{NULL, NULL, NULL}
    };
#endif
#ifdef _APPS_RTSPS
    TXmlWrapperTreeMap ptRtspTreeMap[] =
    {
        {"root/rtsp/live_stream/stream", &GetRtspTrackNo, NULL, NULL},
        {"root/rtsp/live_stream/stream/accessname", NULL, &GetRtspAccessName, NULL},
    	{NULL, NULL, NULL}
    };
#endif    
  
    TXmlWrapperTreeMap ptVideoTreeMap[] =
    {
      {"root/venc_encoder/total_num", NULL, &GetStreamNum, NULL},
      {"root/venc_encoder/support_codectype", NULL, &GetSupportVideoCodecType, NULL},
      {NULL,NULL,NULL}
    };
    
    TXmlWrapperTreeMap ptVideoMasterTreeMap[] =
    {
      {"root/venc_master/setting/resolution", NULL, &GetCapResolution, NULL},
      {NULL,NULL,NULL}
    };
    
     TXmlWrapperTreeMap ptAudioTreeMap[] =
     {
       {"root/audio_encoder/devices/device",&GetAudioDeviceID,NULL,NULL},
      {"root/audio_encoder/devices/device/codec/support_codectype", NULL, &GetSupportAudioCodecType, NULL},
      {NULL,NULL,NULL}
    };
    
    
    TXmlWrapperTreeMap ptEncoderTreeMap[] =
    {
        {"root/venc_encoder/codec", NULL, &GetCodec, NULL},
        {"root/venc_encoder/resolution", NULL, &GetResolution, NULL},
	{"root/venc_encoder/crop/top", NULL, &GetCropTop, NULL},
	{"root/venc_encoder/crop/left", NULL, &GetCropLeft, NULL},
	{"root/venc_encoder/crop/width", NULL, &GetCropWidth, NULL},
	{"root/venc_encoder/crop/height", NULL, &GetCropHeight, NULL},
    	{NULL, NULL, NULL}
    };
#ifdef _HAVE_SIPUAS
    TXmlWrapperTreeMap ptSipuasTreeMap[] =
    {
        {"root/sipua/local_uri", NULL, &GetSipPort, NULL},
    	{NULL, NULL, NULL}
    };
#endif
    tInitOptions.dwVersion = XMLWRAPPER_VERSION;

    if (XmlWrapper_Initial(&hXMLWrapperObj, &tInitOptions) != S_OK) {
        printf("Initial XmlWrapper object fail! \n");
        return S_FAIL;
    }

    printf("Content-type: text/plain\r\n\r\n");
    printf("system_hostname='Network Camera'\x0a");
    
 
    {
      FILE *fp=NULL;
      CHAR 	acBuffer[128];
      int iRet;
      memset(acBuffer, 0, sizeof(acBuffer));

      sprintf(acBuffer, "%s > %s", AUDIO_DEVICE_NUM, AUDIO_DEVICE_LOG );
      unlink(AUDIO_DEVICE_LOG);
      system(acBuffer);

      fp = fopen(AUDIO_DEVICE_LOG, "rb");
      if (fp == NULL) { // open file error
			dwAudioDevNum = 1;	// default value
			unlink(AUDIO_DEVICE_LOG );
      }
    
      iRet = fscanf(fp, "%d", &dwAudioDevNum);
     
      if (iRet <= 0) { // read from file error
			fclose(fp);
			unlink(AUDIO_DEVICE_LOG);
			dwAudioDevNum = 1;	// default value
      }
      fclose(fp);
      unlink(AUDIO_DEVICE_LOG);
    }
   
    
    printf("audio_device_num='%u'\x0a",dwAudioDevNum);
#ifdef _APPS_SYSMGR
    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptSystemTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
    fp=fopen(SYSTEM_CONF,"r");
    if(fp!=NULL)
    {
	fclose(fp);
	fp=NULL;
	if (XmlWrapper_ReadFile_UsrDefFunc(SYSTEM_CONF, hXMLWrapperObj, NULL) != S_OK) {
		DBPRINT1("XmlWrapper read %s config fail! \n", SYSTEM_CONF);
		return S_FAIL;
	}
    }
#endif

    


   /* Get video setting */    
    if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
    }


    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptVideoTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
    }
    fp=fopen(VENC_CONF,"r");
    if(fp!=NULL)
    {
	fclose(fp);
	fp=NULL;
	if (XmlWrapper_ReadFile_UsrDefFunc(VENC_CONF, hXMLWrapperObj, NULL) != S_OK) {
		printf("XmlWrapper read %s config fail! \n", VENC_CONF);
		return S_FAIL;
	}
    }

    /*Get PTZ info*/
    {
      
	/* Get video setting */    
	if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
	  printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
	}
	
	if (XmlWrapper_SetHandler(hXMLWrapperObj, ptVideoMasterTreeMap,NULL) != S_OK) {
		  printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		  return S_FAIL;
	}
	fp=fopen(VENCMASTER_CONF,"r");
	if(fp!=NULL)
	{
	  fclose(fp);
	  fp=NULL;
	  if (XmlWrapper_ReadFile_UsrDefFunc(VENCMASTER_CONF, hXMLWrapperObj, NULL) != S_OK) {
		  printf("XmlWrapper read %s config fail! \n", VENCMASTER_CONF);
		  return S_FAIL;
	  }
	}
      
      
    }

    
    
    

    /* Get audio setting */  
  if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
    }


    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptAudioTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
    }
    fp=fopen(AENC_CONF,"r");
    if(fp!=NULL)
    {
	fclose(fp);
	fp=NULL;
	if (XmlWrapper_ReadFile_UsrDefFunc(AENC_CONF, hXMLWrapperObj, NULL) != S_OK) {
		  printf("XmlWrapper read %s config fail! \n", AENC_CONF);
		  return S_FAIL;
	}
    }

#if 0
    printf("system_info_modelname='IP7138'\x0a");
#endif    
    printf("system_info_serialnumber='0002D1043FD9'\x0a");
#ifdef _HAVE_SERVERPUSH
    /* Get serverpush setting */    
    if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
    }


    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptServerPushTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
    }
    fp=fopen(SERVERPUSH_CONF,"r");
    if(fp!=NULL)
    {
      fclose(fp);
      fp=NULL;
    // printf("[getparam](%d)Enter parse %s\n",__LINE__,SERVERPUSH_CONF);
      if (XmlWrapper_ReadFile_UsrDefFunc(SERVERPUSH_CONF, hXMLWrapperObj, NULL) != S_OK) {
		  printf("XmlWrapper read %s config fail! \n", SERVERPUSH_CONF);
		  return S_FAIL;
      }
      
    }
    //printf("[getparam](%d)Leave parse %s\n",__LINE__,SERVERPUSH_CONF);
#endif
#if 0
	printf("network_http_s0_accessname='video1.mjpg'\x0a");
	printf("network_http_s1_accessname='video2.mjpg'\x0a");
#endif

#ifdef _APPS_RTSPS
	/* Get rtsps setting */
    if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
    }

    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptRtspTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
    fp=fopen(RTSP_CONF,"r");
    if(fp!=NULL)
    {
	fclose(fp);
	fp=NULL;
	if (XmlWrapper_ReadFile_UsrDefFunc(RTSP_CONF, hXMLWrapperObj, NULL) != S_OK) {
		printf("XmlWrapper read %s fail! \n",RTSP_CONF);
		return S_FAIL;
	}
    }
#endif	

#if 0
	printf("network_rtsp_s0_accessname='live1.sdp'\x0a");
	printf("network_rtsp_s1_accessname='live2.sdp'\x0a");
#endif

#ifdef _HAVE_SIPUAS
	/* Get sip server port setting */
    if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
    }

    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptSipuasTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
    fp=fopen(SIP_CONF,"r");
    if(fp!=NULL)
    {
	fclose(fp);
	fp=NULL;
	if (XmlWrapper_ReadFile_UsrDefFunc(SIP_CONF, hXMLWrapperObj, NULL) != S_OK) {
		printf("XmlWrapper read %s fail! \n",SIP_CONF);
		return S_FAIL;
	}
    }
#endif	
#if 0
    printf("network_sip_port='5060'\x0a");
#endif
    /* Get video encoder setting */


    
    for(i=0;i<dwStreamNum;i++)
    {

      sn=i;
      memset(szEncoderConfPath,'\0',128);
       if (XmlWrapper_Reset(hXMLWrapperObj) != S_OK) {
        printf("%s %d : XmlWrapper_Reset Fail! \n", __FILE__, __LINE__);
    }
    
    if (XmlWrapper_SetHandler(hXMLWrapperObj, ptEncoderTreeMap,NULL) != S_OK) {
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
    }
      
      snprintf(szEncoderConfPath, 128, "/etc/conf.d/c0/vencencoder_conf.%d.xml", i);
     // printf("szEncoderConfPath=%s\n",szEncoderConfPath);
      fp=fopen(szEncoderConfPath,"r");
      if(fp!=NULL)
      {
	fclose(fp);
	fp=NULL;
	if (XmlWrapper_ReadFile_UsrDefFunc(szEncoderConfPath, hXMLWrapperObj, &sn) != S_OK)
	{
		  printf("XmlWrapper read config fail! \n");
		  return S_FAIL;
	}
	printf("videoin_c0_s%d_support_codectype='%s'\x0a",i,szSupportVideoCodec);  
      }
    }
    XmlWrapper_Release(&hXMLWrapperObj);
#if 0
    printf("videoin_c0_s0_codectype='mpeg4'\x0a");
    printf("videoin_c0_s0_resolution='704x480'\x0a");
    printf("videoin_c0_s1_codectype='mjpeg'\x0a");
    printf("videoin_c0_s1_resolution='320x240'\x0a");
    printf("videoin_c0_s1_codectype='h264'\x0a");
    printf("videoin_c0_s1_resolution='320x240'\x0a");
#endif

    {
		FILE *fp;
		CHAR 	acBuffer[128];
		int iMem, iTotalKernelMem;
		int iDRAMSize;
		int iRet;
		memset(acBuffer, 0, sizeof(acBuffer));

        //query kernel size
		sprintf(acBuffer, "%s > %s", QUERY_KERNEL_SIZE, KERNEL_LOG);
		unlink(KERNEL_LOG);
		system(acBuffer);

		fp = fopen(KERNEL_LOG, "rb");
		if (fp == NULL) { // open file error
			iMem = 64;	// default value
			unlink(KERNEL_LOG);
		}
		iRet = fscanf(fp, "%d", &iMem);
		if (iRet <= 0) { // read from file error
			fclose(fp);
			unlink(KERNEL_LOG);
			iMem = 64;	// default value
		}
		fclose(fp);
		unlink(KERNEL_LOG);
        iTotalKernelMem = iMem * 1024 * 1024;
        printf("kernel_size='%d'\x0a", iTotalKernelMem);

        //query DRAM size
	    if ((fp=fopen("/sys/MediaSize/DRAM0Size", "r")) != NULL) {
    		fscanf(fp, "%d", &iDRAMSize);
    		fclose(fp);
    		printf("DRAM0_size='%d'\x0a", iDRAMSize);
	    }

	    if ((fp=fopen("/sys/MediaSize/DRAM1Size", "r")) != NULL) {
    		fscanf(fp, "%d", &iDRAMSize);
    		fclose(fp);
    		printf("DRAM1_size='%d'\x0a", iDRAMSize);
	    }        
    }
    
    return 0;
}
