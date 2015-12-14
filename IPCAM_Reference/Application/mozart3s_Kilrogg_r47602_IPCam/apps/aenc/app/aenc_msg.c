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
 * aenc_msg.c
 *
 * \brief
 * aenc message handler
 *
 * \date
 * 2008/1/15
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#include "msgreader.h"
#include "aenc.h"
static DWORD gdwIOID=-1;
/* ========================================================================== */
SCODE aenc_msg_ForceCI(HANDLE hObject, const CHAR *el, const CHAR **atts)
{
    TAencInfo *ptaencInfo = (TAencInfo *)hObject;
    TDevOpt *ptDevOpt = ptaencInfo->ptdoHead;
    
    
   // if(gdwIOID!=0)
    //{  
      //  /*
	do
	{
	//   ptDevOpt->tacConf.bForceCI = TRUE;
	  // ptDevOpt->tacConf.bSetOptions = TRUE;
	    
	      ptDevOpt->tMsgOptRec.bSendConf=TRUE;
	      sem_wait(&( ptDevOpt->smMsgOptions));
	    ptDevOpt->bReMsg_flag = TRUE;
	    sem_post(&( ptDevOpt->smMsgOptions));
	  ptDevOpt=ptDevOpt->ptdoNext;
	}while(ptDevOpt);
      // */
   // }
    //printf("[aenc_msg]Jeff 111 (%d)ForceCI gdwIOID=%d\n",__LINE__,gdwIOID);
    return S_OK;
}
#if 0
SCODE aenc_msg_SetOuputSocketID(HANDLE hObject, const CHAR *el, const CHAR **atts)
{
    DWORD  dwCurId;

    if (atts) {
	printf("[anec_msg]atts[0]=%s\n",atts[0]);
        if (strcmp(atts[0], "id") != 0) {
            printf("[aenc_msg](%d) No id attribute \n",__LINE__);
            return IGNORE_CHILD_CONFIG;            
        }
	printf("[anec_msg]atts[1]=%s\n",atts[1]);
        dwCurId = strtoul(atts[1], (char**)NULL, 10);
        printf("[aenc_msg](%d)dwCurId=%d \n",__LINE__,dwCurId);
        return S_OK;
    }

  printf("[aenc_msg](%d) dd No attribute \n",__LINE__);
    return IGNORE_CHILD_CONFIG;

}
#endif

SCODE aenc_msg_SetIOId(HANDLE hObject, const CHAR *el, const CHAR **atts)
{
  
    DWORD  dwCurId;
  
     TAencInfo *ptaencInfo = (TAencInfo *)hObject;
     TDevOpt *ptDevOpt = ptaencInfo->ptdoHead;
    

    if (atts) {
	
	//printf("[anec_msg](%d) atts[0]=%s\n",__LINE__,atts[0]);
	if(atts[0]==NULL||atts[1]==NULL)
	{
	    printf("[aenc_msg](%d) No id attribute \n",__LINE__);
	    return IGNORE_CHILD_CONFIG; 
	} 
        if (strcmp(atts[0], "id") != 0) {
            printf("[aenc_msg](%d) No id attribute \n",__LINE__);
            return IGNORE_CHILD_CONFIG;            
        }
	//printf("[anec_msg]atts[1]=%s\n",atts[1]);
        dwCurId = strtoul(atts[1], (char**)NULL, 10);
	gdwIOID=dwCurId ;
//        printf("[aenc_msg](%d)dwCurId=%d \n",__LINE__,dwCurId);
	do
	{
	  ptDevOpt->tMsgOptRec.dwIOId=dwCurId;
	//  printf("[aenc_msg](%d)ptDevOpt->szID=%s \n",__LINE__,ptDevOpt->szID);
	  ptDevOpt=ptDevOpt->ptdoNext;
	   
	}while(ptDevOpt);
	
	
        return S_OK;
    }

    printf("[aenc_msg](%d) No attribute \n",__LINE__);
    return IGNORE_CHILD_CONFIG;



}

void aenc_msg_SetOuputSocketStatus(HANDLE hObject, const CHAR *s, SDWORD len)
{
  char str[len+1];
  TAencInfo *ptaencInfo = (TAencInfo *)hObject;
  TDevOpt *ptDevOpt = ptaencInfo->ptdoHead;
  
  memcpy(str, s, len);
  str[len]='\0';
  printf("[aenc_msg](%d)SocketStatus=%s dwIOId=%d\n",__LINE__,str,ptDevOpt->tMsgOptRec.dwIOId);
  do
  {
	  if(strcmp(str,"start")==0)
	  {
		  ++ptDevOpt->tMsgOptRec.adwVisitors[ptDevOpt->tMsgOptRec.dwIOId];
	  }
	  else if(strcmp(str,"stop")==0)
	  {
		  if (ptDevOpt->tMsgOptRec.adwVisitors[ptDevOpt->tMsgOptRec.dwIOId] > 0)
			  --ptDevOpt->tMsgOptRec.adwVisitors[ptDevOpt->tMsgOptRec.dwIOId];
	  }
	  else
	  {
		printf("[aenc_msg](%d)IGNORE_CHILD_CONFIG\n",__LINE__);
	  }
	  //printf("[aenc_msg](%d)&(ptDevOpt->smMsgOptions)=%p\n",__LINE__,&( ptDevOpt->smMsgOptions));
	  sem_wait(&( ptDevOpt->smMsgOptions));
	  ptDevOpt->bReMsg_flag = TRUE;
	  sem_post(&( ptDevOpt->smMsgOptions));
	  
	  
	  ptDevOpt=ptDevOpt->ptdoNext;
	  
    }while(ptDevOpt);
  
 // return S_OK;
}
/* ========================================================================== */
SCODE aenc_msg_initial(TAencInfo *ptaencInfo, CHAR* szMsgFIFO)
{
    TMsgReaderInitialOpts tMsgReaderInitOpts;
    TDevOpt *ptDevOpt = ptaencInfo->ptdoHead;
	
    TXmlWrapperTreeMap MsgHdlTreeMap[] = {	
		//{"control",&aenc_msg_SetOuputSocketID,NULL, NULL}, 
		{"control",&aenc_msg_SetIOId,NULL, NULL}, 
		{"control/output",NULL,&aenc_msg_SetOuputSocketStatus,NULL},//start, stop
        {"control/forceCI",  &aenc_msg_ForceCI, NULL, NULL},
        {NULL, NULL, NULL}
    };
    
	do {
		
		if (sem_init(&(ptDevOpt->smMsgOptions), 0, 1) != 0) {
			printf("[aenc_msg_initial](%d) Failed! Because sem_init() is failed.\n",__LINE__);
			return S_FAIL;
		}
		memset(&(ptDevOpt->tMsgOptRec), 0x0 ,sizeof(TAENCMsgOptRec));
		ptDevOpt=ptDevOpt->ptdoNext;
		
	} while(ptDevOpt);
	
    tMsgReaderInitOpts.dwVersion = MSGREADER_VERSION;
    tMsgReaderInitOpts.pcMsgPath = szMsgFIFO;
    tMsgReaderInitOpts.ptCallBackHandler = MsgHdlTreeMap;
    tMsgReaderInitOpts.ptv_time = NULL;
    tMsgReaderInitOpts.hUsrObj = (HANDLE)ptaencInfo;
//  printf("[aenc_msg_initial](%d)msgfifo=%s\n",__LINE__,szMsgFIFO);
    if (MsgReader_Initial(&ptaencInfo->hMsgRdrObj, &tMsgReaderInitOpts) != S_OK)
    {
        fprintf(stderr, "MsgReader_Initial Fail!!\n");
        return -1;
    }
    return S_OK;
}

/* ========================================================================== */

static void theMsgTask(HANDLE handle)
{
	BYTE abyBuffer[512];
	fd_set rfds;
	int iFifoFD;
	int iReadSz;
	TAencInfo *ptaencInfo = (TAencInfo *)handle;
	iFifoFD = MsgReader_GetFifoFd(ptaencInfo->hMsgRdrObj);
	
	ptaencInfo->edtsState = edtsRunning;
	while (ptaencInfo->edtsState == edtsRunning) { 
		memset(abyBuffer, 0, 512);
		FD_ZERO(&rfds); 
		FD_SET(iFifoFD, &rfds);
		if (select(iFifoFD + 1, &rfds, NULL, NULL, NULL) > 0) {
			iReadSz = read(iFifoFD, abyBuffer, 512);
			MsgReader_ParseMsg(ptaencInfo->hMsgRdrObj, abyBuffer, iReadSz);
		}
	}
}

SCODE aenc_msg_start(TAencInfo *ptaencInfo)
{
	
	int iRet;   

	iRet = pthread_create(&(ptaencInfo->thread_id), NULL, (HANDLE)&theMsgTask, (HANDLE)ptaencInfo);
	if (iRet != 0){
		fprintf(stderr, "pthread_create Fail : %s!\n", strerror(iRet));
		return S_FAIL;
	}   
	return S_OK;
#if 0
	BYTE abyBuffer[512];
	fd_set rfds;
	int iFifoFD;
	struct timeval tv;
	int iReadSz;
	
	iFifoFD = MsgReader_GetFifoFd(ptaencInfo->hMsgRdrObj);
	
   	while (g_bTerminate == FALSE) {
		//printf("[aenc_msg]%d\n",__LINE__);  
		memset(abyBuffer, 0, 512);
		//printf("[aenc_msg]%d\n",__LINE__);  
		FD_ZERO(&rfds);
		//printf("[aenc_msg]%d\n",__LINE__);  
		FD_SET(iFifoFD, &rfds);
		//printf("[aenc_msg]%d\n",__LINE__);  
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		//printf("[aenc_msg]%d\n",__LINE__);  
//		if (select(iFifoFD + 1, &rfds, NULL, NULL, &tv) > 0) {
		if (select(iFifoFD + 1, &rfds, NULL, NULL, NULL) > 0) {
			iReadSz = read(iFifoFD, abyBuffer, 512);
//			printf("[aenc_msg](%d)abyBuffer=%s\n",__LINE__,abyBuffer);
			MsgReader_ParseMsg(ptaencInfo->hMsgRdrObj, abyBuffer, iReadSz);
		}
		//printf("[aenc_msg]%d\n\n",__LINE__);  
	}

	return S_OK;
	#endif
}

/* ========================================================================== */
SCODE aenc_msg_stop(TAencInfo *ptaencInfo)
{ 
    int iRet;
	TDevOpt *ptDevOpt = ptaencInfo->ptdoHead;
 
	ptaencInfo->edtsState = edtsStopping; 
	iRet = pthread_join(ptaencInfo->thread_id, NULL);
	if (iRet != 0)  {
		fprintf(stderr, "pthread_join Fail, %s\n", strerror(iRet));
	}    
    if(&(ptDevOpt->smMsgOptions)!=NULL)
    { 
        do
		{ 
			sem_destroy(&(ptDevOpt->smMsgOptions));
			ptDevOpt=ptDevOpt->ptdoNext;
		}while(ptDevOpt); 
    }
	return S_OK;
   
  
}
