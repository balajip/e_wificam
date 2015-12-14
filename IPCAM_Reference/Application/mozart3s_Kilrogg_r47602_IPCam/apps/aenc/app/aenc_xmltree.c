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
 * aenc_xmltree.c
 *
 * \brief
 * Audio encoder xmlwrapper treemap call back function
 *
 * \date
 * 2006/06/19
 *
 * \author
 * Cheng-Ching Huang
 *
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
//sample format
#include <linux/soundcard.h>
#include "typedef.h"
#include "global_codec.h"
#include "aenc.h"
//ignore sib/child data
//#include "aenc_conftree.h"
#include "xmlwrapper.h"
#include "dbgdefs.h"
//=================================================================================================
/* XML elements -- declaration */

/* XML_StartElementHandler */
/*
SCODE AxeXmlTree_Device(void *userData, const char *name, const char **atts);
SCODE AxeXmlTree_IO(void *userData, const char *name, const char **atts);
*/
/* XML_CharacterDataHandler */
/*
void AxeXmlTree_Node(void *userData, const char *name, int len);
void AxeXmlTree_ChNum(void *userData, const char *name, int len);
void AxeXmlTree_SampFmt(void *userData, const char *name, int len);

void AxeXmlTree_Codec(void *userData, const char *name, int len);
void AxeXmlTree_Bitrate(void *userData, const char *name, int len);
void AxeXmlTree_SampRate(void *userData, const char *name, int len);

void AxeXmlTree_IO_Input(void *userData, const char *name, int len);
void AxeXmlTree_IO_Output(void *userData, const char *name, int len);
*/
static DWORD gdwIOID = 0;
static DWORD dwIOCount = 0;
//=================================================================================================
SCODE AxeXmlTree_Device(void *userData, const char *name, const char **atts)
{	
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *) userData;
	DWORD				dwLen;
	
	/* attribute id and node */
	DBPRINT2("%s:%d\n", __func__, __LINE__)
	if (!(atts[0]) || !atts[2]) 
	{
		DBPRINT0("ignore\n")
		return IGNORE_CHILD_CONFIG;
	}//if atts	
	
	{
		if (strcmp(atts[0], "id") == 0 && strcmp(atts[2], "node") == 0) 
		{
			//fprintf(stderr, "find attribute id = %s\n", atts[1]);				
			TDevOpt				**pptdoSrc;
			TDevOpt				*ptDevOpt;
			pptdoSrc = &(ptaxHandle->ptAencInfo->ptdoHead);
			ptDevOpt = *pptdoSrc;
			while (ptDevOpt) {
				/* reloading */
				if (strcmp(atts[1], ptDevOpt->szID) == 0) {
					//fprintf(stderr, "%s: Device Id %s\n", __func__, ptDevOpt->szID);				
					ptaxHandle->hTmp = ptDevOpt;
					ptaxHandle->ptConfTmp = &(ptDevOpt->tacConf);
					return S_OK;
				}
				pptdoSrc = &(ptDevOpt->ptdoNext);
				ptDevOpt = *pptdoSrc;
			}
			ptDevOpt = malloc(sizeof(TDevOpt));
			memset (ptDevOpt, 0, sizeof(TDevOpt));
			strncpy(ptDevOpt->szID, atts[1], ID_LENGTH);				
			dwLen = strlen(atts[3]);
			dwLen = (dwLen < (NODE_LENGTH - 1)) ? dwLen : (NODE_LENGTH - 1);				
			memcpy(ptDevOpt->szNode, atts[3], dwLen);
			ptDevOpt->szNode[dwLen] = '\0';
			*pptdoSrc = ptDevOpt;				
			ptaxHandle->hTmp = ptDevOpt;
			ptaxHandle->ptConfTmp = &(ptDevOpt->tacConf);
			return S_OK;
		}//if id & node
	}
	/* id & node attribute is necessary. 
	   ignore this element and its children if there is no id */
	DBPRINT1("%s IGNORE_CHILD_CONFIG", __func__)
	return IGNORE_CHILD_CONFIG;
}//device

void AxeXmlTree_ChNum(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;				
	
	if (ptaxHandle->bFirstLoad)
	{
		TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
		char				szChNum[8];
		DWORD				dwChNum;
		DWORD				dwLen = (len < (sizeof(szChNum) - 1)) 
									? len : (sizeof(szChNum) - 1);

		DBPRINT1("%s ",__func__)
		memcpy(szChNum, name, dwLen);
		szChNum[dwLen] = '\0';
		dwChNum = strtol(szChNum, NULL, 0);
		assert(errno != ERANGE);
		ptDevOpt->dwChanNum = dwChNum;
		DBPRINT1("%d\n", (int)ptDevOpt->dwChanNum)
	}
}
void AxeXmlTree_ReadType(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;				
	
	if (ptaxHandle->bFirstLoad)
	{
		TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
		char				szReadType[8];
		DWORD				dwReadType;
		DWORD				dwLen = (len < (sizeof(szReadType) - 1)) 
									? len : (sizeof(szReadType) - 1);

		//printf("%s ",__func__);
		memcpy(szReadType, name, dwLen);
		szReadType[dwLen] = '\0';
		dwReadType = strtol(szReadType, NULL, 0);
		assert(errno != ERANGE);
		if (dwReadType == 0)
			ptDevOpt->eAudioFrameReadType = eNonInterleavedRead ;
		else
			ptDevOpt->eAudioFrameReadType = eInterleavedRead ;
		//printf("ReadType %d\n", (int)ptDevOpt->eAudioFrameReadType);
	}
	
}

void AxeXmlTree_SampFmt(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;				

	//char				szChNum[8];
	/* TODO: Modify this to support other formats */
	/* Currently only support Little endian signed 16 */
	
	if (ptaxHandle->bFirstLoad)
	{
		TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
		DBPRINT1("%s ",__func__)
		ptDevOpt->dwSampSize = 2;
		ptDevOpt->dwSampFormat = AFMT_S16_LE;
		DBPRINT1("%d\n", (int)ptDevOpt->dwSampFormat)
	}
}

void AxeXmlTree_InputSelection(void *userData, const char *name, int len)
{
	
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;				
	
	//if (ptaxHandle->bFirstLoad)
	{
		TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
		char				szInputSel[8];
		DWORD				dwInputSel;
		DWORD				dwLen = (len < (sizeof(szInputSel) - 1)) 
									? len : (sizeof(szInputSel) - 1);
		memcpy(szInputSel, name, dwLen);
		
		
		szInputSel[dwLen] = '\0';
		dwInputSel = strtol(szInputSel, NULL, 0);
		//printf("(%s:%d)szInputSel=%s dwInputSel=%d\n",__func__,__LINE__,szInputSel,(int)dwInputSel);
		assert(errno != ERANGE);
		if (ptDevOpt->dwInputSel != dwInputSel)
		{
			ptDevOpt->dwInputSel = dwInputSel;
			ptDevOpt->bInputSelChanged = TRUE;
		}
		else
		{
			ptDevOpt->bInputSelChanged = FALSE;
		}
		//printf("(%s:%d) ptDevOpt->dwInputSel=%d ptDevOpt->bInputSelChanged=%d\n",__func__,__LINE__, (int)ptDevOpt->dwInputSel,ptDevOpt->bInputSelChanged);
	}
}

void AxeXmlTree_Mute(void *userData, const char *name, int len)
{

	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;				
	
	//if (ptaxHandle->bFirstLoad)
	{
		TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
		char				szMute[8];
		DWORD				dwMute = 0;
		BOOL 				bMute = FALSE;
		DWORD				dwLen = (len < (sizeof(szMute) - 1)) ? len : (sizeof(szMute) - 1);
		
		memcpy(szMute, name, dwLen);
		szMute[dwLen] = '\0';
		dwMute = strtol(szMute, NULL, 0);
		//printf("(%s:%d)szMute=%s dwMute=%d\n",__func__,__LINE__,szMute,(int)dwMute);
		assert(errno != ERANGE);
		if(dwMute ==  0 )
		{	
			bMute =FALSE;
		}
		else if (dwMute > 0)
		{
			bMute =TRUE;
		}
		
		if (ptDevOpt->bMute != bMute)
		{	
			ptDevOpt->bMute = bMute;
			ptDevOpt->bMuteChanged = TRUE;
		}
		else
		{
			ptDevOpt->bMuteChanged = FALSE;
		}
		//printf("(%s:%d) ptDevOpt->bMute=%d ptDevOpt->bMuteChanged=%d\n",__func__,__LINE__, (int)ptDevOpt->bMute,ptDevOpt->bMuteChanged);
	}
}

void AxeXmlTree_Volume(void *userData, const char *name, int len)
{

	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;				
	
	//if (ptaxHandle->bFirstLoad)
	{
		TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
		char				szVolume[8];
		DWORD				dwVolume;
		DWORD				dwLen = (len < (sizeof(szVolume) - 1)) 
									? len : (sizeof(szVolume) - 1);
		memcpy(szVolume, name, dwLen);
		
		
		szVolume[dwLen] = '\0';
		dwVolume = strtol(szVolume, NULL, 0);
		//printf("(%s:%d)szVolume=%s dwVolume=%d\n",__func__,__LINE__,szVolume,(int)dwVolume);
		assert(errno != ERANGE);
		if(ptDevOpt->dwVolume != dwVolume)
		{
			ptDevOpt->dwVolume = dwVolume;
			ptDevOpt->bVolumeChanged = TRUE;
		}
		else
		{
			ptDevOpt->bVolumeChanged = FALSE;
		}
		//printf("(%s:%d) ptDevOpt->dwVolume=%d ptDevOpt->bVolumeChanged=%d\n",__func__,__LINE__, (int)ptDevOpt->dwVolume,ptDevOpt->bVolumeChanged);
	}
}

//==record IO Device===============================================================================
SCODE AxeXmlTree_IO(void *userData, const char *name, const char **atts)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;	
	
	DBPRINT1("%s\n",__func__)
	if (ptaxHandle->bFirstLoad)
	{		
		while (*atts) 
		{
			TDevOpt				*ptDevOpt;
			if (strcmp(atts[0], "device") != 0) 
			{				
				return IGNORE_SIBLING_CONFIG;
			}
			ptDevOpt = ptaxHandle->ptAencInfo->ptdoHead;
			while (ptDevOpt) 
			{
				if (strcmp(atts[1], ptDevOpt->szID) == 0) 
				{						
					ptaxHandle->hTmp = ptDevOpt;
					
					ptDevOpt->tCodecOpt.dwCodecHanldeNum = dwIOCount;
					ptDevOpt->dwSavedCodecHandleNum = ptDevOpt->tCodecOpt.dwCodecHanldeNum;
					printf("[aenc_xmltree](%d)ptDevOpt->tCodecOpt.dwCodecHanldeNum=%d\n",__LINE__,ptDevOpt->tCodecOpt.dwCodecHanldeNum);
					DBPRINT1("dev ID %s \n", atts[1])
					return S_OK;
				}
				ptDevOpt = ptDevOpt->ptdoNext;
			}
			DBPRINT1("NO Device has id %s\n", atts[1])
			return IGNORE_SIBLING_CONFIG;			
			atts += 2;
		}
	}
	/* device attribute is necessary. 
	   ignore this element and its parent if there is no id */
	DBPRINT2("%s:%d IGNORE SIB\n",__func__, __LINE__)
	return IGNORE_SIBLING_CONFIG;
}

//==record IO channel==============================================================================
void AxeXmlTree_IO_Input(void *userData, const char *name, int len)
{	
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;
	//DWORD				dwChMask = 0;	
	char				szInput[64];
	char				*szEnd, *szHead;
	BYTE				*pabyChannel = ptaxHandle->abyChanTmp;
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szInput) - 1)) 
								? len : (sizeof(szInput) - 1);
	//int i=0;
	DBPRINT1("%s\n",__func__)	
	memset(pabyChannel, 0, CHANNEL_LENGTH);
	memcpy(szInput, name, dwLen);	
	szInput[dwLen] = '\0';
	szHead = &(szInput[0]);
	if (ptaxHandle->bFirstLoad)
	{
		do {
			/* ignore numbers and EOL */
			while (!(((*szHead >= '0') && (*szHead <= '9')) || (*szHead == '\0'))) {			
				szHead ++;
			}
			dwTmp = strtol(szHead, &szEnd, 0);
			//DBPRINT1("dwTmp %d\n", (int)dwTmp)
			//printf("[aenc_xmltree.c](%d) dwTmp=%d  pabyChannel=%p \n",__LINE__,(int)dwTmp,pabyChannel);
			assert(errno != ERANGE);
			/* no digits at all */
			if (szHead == szEnd){			
				break;
			}
			/* max 8 channel in io */
			if (dwTmp < 8) {
				//ptaxHandle->szioInput[dwTmp] = 1;
				pabyChannel[dwTmp] = 1;
				//dwChMask |= (1 << dwTmp);
			}
			szHead = szEnd;
		} while (*szEnd != '\0');
	}
#if 0
	{
		int i = 0;
		for(i=0;i<CHANNEL_LENGTH;i++)
		{
			printf("[aenc_xmltree.c](%d)pabyChannel[%d]=%d \n",__LINE__,i,pabyChannel[i]);
		}
	}
#endif	
	//ptIO->dwChannelMask = dwChMask;
	//fprintf(stderr, "%d\n", dwChMask);
}


SCODE AxeXmlTree_IO_ID(void *userData, const char *name, const char **atts)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;
	TCodecOpt *pTCodecOpt = (TCodecOpt *)ptaxHandle->hTmp; 
	
	if (ptaxHandle->bFirstLoad)
	{
		if((*atts)!=NULL)
		{
		if (strcmp(atts[0], "id") != 0) 
		{				
			return IGNORE_SIBLING_CONFIG;
		}
		if(atts[1]==NULL)
		{
			return IGNORE_SIBLING_CONFIG;
		}
		
			gdwIOID=strtol(atts[1], (char **)NULL,10);
			printf("[aenc_xmltree](%d)name=%s  atts[1]=%s\n",__LINE__,name,atts[1]);
			dwIOCount ++;
			return S_OK;
		}
		else
		{
			return IGNORE_SIBLING_CONFIG;
		}
	
	}
	return S_OK;
}

#if 0
static DWORD dwOutID;

SCODE AxeXmlTree_IO_Output_ID(void *userData, const char *name, const char **atts)
{

	
	if((*atts)!=NULL)
	{
	  if (strcmp(atts[0], "id") != 0) 
	  {				
	    return IGNORE_SIBLING_CONFIG;
	  }
	  if(atts[1]==NULL)
	  {
	      return IGNORE_SIBLING_CONFIG;
	  }
	  
	    dwOutID=strtol(atts[1], (char **)NULL,10);
	    printf("[aenc_xmltree](%d)name=%s  atts[1]=%s\n",__LINE__,name,atts[1]);
	    return S_OK;
	}
	else
	{
	     return IGNORE_SIBLING_CONFIG;
	}
}
#endif

//==record IO socket path and construct IO to Device===============================================
void AxeXmlTree_IO_Output(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;		
	TDevOpt			*ptDevOpt = ptaxHandle->hTmp;	
	CHAR *szOut, szTmp[OUT_LENGTH<<2];
	//CHAR *szTmpOut;
	DWORD				dwLen;
	TIO		**pptSrc;
	TIO		*ptIO;
	int i=0;

	/* generate a new TIO at the tail of the TIO list */	
	DBPRINT2("%s:%d\n",__func__, __LINE__)
	pptSrc = &(ptDevOpt->ptioHead);	
	ptIO = *pptSrc;
	while (ptIO) {
		pptSrc = &(ptIO->ptioNext);
		ptIO = *pptSrc;
	}
	/* io list */
	memset(szTmp, 0, len+1);
	strncpy(szTmp, name, len);
	szOut = strtok(szTmp, ", ");
	if (ptaxHandle->bFirstLoad)
	{
		while(szOut)
		{
			ptIO = malloc(sizeof(TIO));
			memset (ptIO, 0, sizeof(TIO)); 				
			memcpy(ptIO->abyChannel, ptaxHandle->abyChanTmp, CHANNEL_LENGTH);		
			dwLen = strlen(szOut);
			dwLen = (dwLen < (OUT_LENGTH - 3)) ? dwLen : (OUT_LENGTH - 3);
			memcpy(ptIO->szOut, szOut, dwLen);
			snprintf(ptIO->szOut,dwLen+2,"%s%c",szOut,'\0');
			//printf("[aenc_xmltree](%d)ptIO->szOut=%s\n",__LINE__,ptIO->szOut);
			#if 0
			ptIO->dwOutId=dwOutID;
			#endif
			ptIO->dwIOId=gdwIOID;
			//printf("[aenc_xmltree](%d)ptIO->dwIOId=%d\n",__LINE__,ptIO->dwIOId);
			ptIO->bDoEncode=FALSE;
			*pptSrc = ptIO;		
			szOut = strtok(NULL, ", ");
			pptSrc = &(ptIO->ptioNext);
			
			//printf("[aenc_xmltree](%d)ptIO->szOut=%s ptIO->dwOutId=%d\n",__LINE__ ,ptIO->szOut,ptIO->dwOutId);
			i++;
		}
	}
#if 0
	pptSrc = &(ptDevOpt->ptioHead);	
	ptIO = *pptSrc;
	while (ptIO) {
	  
		printf("[aenc_xmltree](%d)ptIO->szOut=%s\n",__LINE__,ptIO->szOut);
		szTmpOut=strdup(ptIO->szOut);
		strncpy(ptIO->szOut,strtok(szTmpOut,"@"),strlen(ptIO->szOut));
		printf("[aenc_xmltree](%d)szTmpOut=%s\n",__LINE__,szTmpOut);
		printf("[aenc_xmltree](%d)ptIO->szOut=%s\n",__LINE__,ptIO->szOut);
		ptIO->dwOutId=strtol(strtok(NULL,"@"), (char **)NULL,10);
		printf("[aenc_xmltree](%d)ptIO->dwOutId=%d\n",__LINE__,ptIO->dwOutId);
		pptSrc = &(ptIO->ptioNext);
		ptIO = *pptSrc;
	}
     
	if(szTmpOut!=NULL)
	{
	    free(szTmpOut);
	    szTmpOut=NULL;
	}
#endif	
	
}

//==reloading======================================================================================
void AxeXmlTree_Codec(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;		
	TDevOpt			*ptDevOpt = ptaxHandle->hTmp;
	TAFConf *ptacConf = ptaxHandle->ptConfTmp;
	TCodecOpt		*ptcOpt;	
	char	szCodec[len+1];
	DWORD	dwFourCC;
	int	i;	

	DBPRINT2("%s:%d\n",__func__, __LINE__)

    strncpy(szCodec, name, len);
    szCodec[len+1]='\0';

    dwFourCC = 
		MAKEFOURCC(szCodec[0], szCodec[1], szCodec[2], szCodec[3]);

	/* set codec */
	ptcOpt = &(ptaxHandle->ptAencInfo->atcoSaved[0]);
	for (i = 0; i < MAX_CODEC_NUM; i ++) {
		if (ptcOpt->dwFourCC == dwFourCC) {
            break;
		}
		ptcOpt ++;
	}
	printf("[aenc_xmltree](%d)ptDevOpt->tCodecOpt.dwCodecHanldeNum=%d\n",__LINE__,ptDevOpt->tCodecOpt.dwCodecHanldeNum);

    if (i != MAX_CODEC_NUM) 
	{										
		if (ptaxHandle->bFirstLoad)
		{				
			memcpy(&(ptDevOpt->tCodecOpt), ptcOpt, sizeof(TCodecOpt));
			//===save codec
			ptaxHandle->hTmp = &(ptDevOpt->tCodecOpt);
		}
		//reloading reset codec
		else		
		{												
			/* save new codec pfn */
			if (ptDevOpt->tCodecOpt.dwFourCC != dwFourCC)
			{					
				memcpy(&(ptDevOpt->tNewOpt), ptcOpt, sizeof(TCodecOpt));
				ptacConf->bCodec = TRUE;										
//				printf("aenc_xmltree(%d)Set Codec\n",__LINE__);
			}
			else
			{
	//			printf("aenc_xmltree(%d)Set Codec\n",__LINE__);
				/*save old conf*/
				memcpy(&(ptDevOpt->tNewOpt), &(ptDevOpt->tCodecOpt), sizeof(TCodecOpt));
			}
			//===save codec
			ptaxHandle->hTmp = &(ptDevOpt->tNewOpt);
		}										
	} 
	else 
	{
		printf("[aenc_xmltree](%d) Not support the Codec type : %u!! (%s)\n", __LINE__,dwFourCC,name);	
		memset(&(ptDevOpt->tCodecOpt), 0, sizeof(TCodecOpt));
		//assert(i != MAX_CODEC_NUM);
	}
	printf("[aenc_xmltree](%d)ptDevOpt->tCodecOpt.dwCodecHanldeNum=%d\n",__LINE__,ptDevOpt->tCodecOpt.dwCodecHanldeNum);

}
void AxeXmlTree_ChannelNum(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;		
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;		
	//Not support configfile reloading by Jeff Liao 
	//TAFConf *ptacConf = ptaxHandle->ptConfTmp;
	
	char				szTmp[16];
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szTmp) - 1)) 
								? len : (sizeof(szTmp) - 1);

	if (ptOpt == NULL) {
		return;
	}
	memcpy(szTmp, name, dwLen);
	szTmp[dwLen] = '\0';
	dwTmp = strtol(szTmp, NULL, 0);
	assert(errno != ERANGE);
	ptOpt->dwChanNum = dwTmp;
	//printf("ptOpt->dwChanNum = %d\n",ptOpt->dwChanNum);
	
}
void AxeXmlTree_Bitrate(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;		
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;		
	//Not support configfile reloading by Jeff Liao 
	TAFConf *ptacConf = ptaxHandle->ptConfTmp;
	
	char				szTmp[16];
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szTmp) - 1)) 
								? len : (sizeof(szTmp) - 1);

	if (ptOpt == NULL) {
		return;
	}
	memcpy(szTmp, name, dwLen);
	szTmp[dwLen] = '\0';
	dwTmp = strtol(szTmp, NULL, 0);
	assert(errno != ERANGE);
#if 1		
	if (ptOpt->dwBitRate != dwTmp)
	{
		ptOpt->dwBitRate = dwTmp;		
		DBPRINT2("%s BitRate %ld\n", __FILE__, ptOpt->dwBitRate);
		if (!ptaxHandle->bFirstLoad && !ptacConf->bCodec)
		{
			//Change BitRate		
			//printf(" SetBitRate\n");
			ptacConf->bBitRate = TRUE;
		}												
	}
#endif
}
//The following hdmi_get_control() is for HDMI audio. The hw interface of alsa is hw:0,0.
#ifdef _CAT6011_HDMI_AUDIO_

static int hdmi_get_control(const char* item)
{
	snd_hctl_t *hctl;
	snd_ctl_elem_id_t *id;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_value_t *control;

	int result = -1 ;
	
	snd_hctl_open(&hctl, "hw:0", 0);
	snd_hctl_load(hctl);	

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

	elem = snd_hctl_find_elem(hctl, id);

	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_value_set_id(control, id);    

	if(snd_hctl_elem_read(elem, control) < 0)
		return -1 ;
	result = snd_ctl_elem_value_get_integer(control,0) ;

	snd_hctl_close(hctl);

	return result ;
}

#elif _IT6604_HDMI_AUDIO_

static int hdmi_get_control(const char* item)
{
        snd_hctl_t *hctl;
        snd_ctl_elem_id_t *id;
        snd_hctl_elem_t *elem;
        snd_ctl_elem_value_t *control;

        int result = -1 ;

        snd_hctl_open(&hctl, "hw:0", 0);
        snd_hctl_load(hctl);

        snd_ctl_elem_id_alloca(&id);
        snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
        snd_ctl_elem_id_set_name(id, item);//or snd_ctl_elem_id_set_id(id, numeric_id)

        elem = snd_hctl_find_elem(hctl, id);

        snd_ctl_elem_value_alloca(&control);
        snd_ctl_elem_value_set_id(control, id);

        if(snd_hctl_elem_read(elem, control) < 0)
                return -1 ;
        result = snd_ctl_elem_value_get_integer(control,0) ;

        snd_hctl_close(hctl);

        return result ;
}
#endif

void AxeXmlTree_SampRate(void *userData, const char *name, int len)
{
	TAencXMLTTh *ptaxHandle = (TAencXMLTTh *)userData;		
	TCodecOpt			*ptOpt = ptaxHandle->hTmp;		
	//Not support configfile reloading by Jeff Liao 	
	TAFConf *ptacConf = ptaxHandle->ptConfTmp;
	
	char				szTmp[16];
	DWORD				dwTmp;
	DWORD				dwLen = (len < (sizeof(szTmp) - 1)) 
								? len : (sizeof(szTmp) - 1);

	if (ptOpt == NULL) {
		return;
	}

	memcpy(szTmp, name, dwLen);
	szTmp[dwLen] = '\0';
	dwTmp = strtol(szTmp, NULL, 0);
	assert(errno != ERANGE);

#ifdef _CAT6011_HDMI_AUDIO_	
	ptOpt->dwSampRate= hdmi_get_control("Sample Rate");
#elif _IT6604_HDMI_AUDIO_
	ptOpt->dwSampRate= hdmi_get_control("Sample Rate");
#else
	ptOpt->dwSampRate = dwTmp;
#endif
	//printf("[aenc_xmltree](%d)SampleRate: %u\n",__LINE__ ,ptOpt->dwSampRate);
	
#if 1	
	if (ptOpt->dwSampRate != dwTmp)
	{			
		ptOpt->dwSampRate = dwTmp;
		DBPRINT2("%s SampRate %ld\n", __FILE__, ptOpt->dwSampRate)
		if (!ptaxHandle->bFirstLoad && !ptacConf->bCodec)
		{
			DBPRINT0("SetSampRate\n")
			ptacConf->bSampRate = TRUE;
		}									
	}
#endif		
	//fprintf(stderr, "%d\n", dwTmp);
}
