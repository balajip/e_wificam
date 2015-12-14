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

#include "psiad_local.h"
extern const TPsiaStatusCodeTable g_psia_statuscodeInfo[];

static void create_path_dir(char *szDirPath);
static int SendOutMsg(int ifdReq, char *szHTTPHeader, BYTE  *pbyReturnResult, DWORD dwReturnResultLen);

/* =========================================================================================== */
SCODE Psiad_InitHTTPSock(int *piSck, CHAR *szPath)
{
    create_path_dir(szPath);

    unlink(szPath);

    // ====== create communication socket for fdipc from Boa======
    *piSck = fdipc_server_socket(szPath,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (*piSck == -1) {    	
    
        return S_FAIL;
    }
    return S_OK;
}

/* =========================================================================================== */
static void create_path_dir(char *szDirPath)
{
    char    *pcChr;
    int	dwSuffixLength = 0;
    int	dwPathLength = 0;
    char     szTmpDir[256];

    /*****************************************************************
     * Create directory
     *****************************************************************/
    pcChr = strchr(szDirPath, '/');
    while (pcChr)
    {
        dwSuffixLength = strlen(pcChr);
        if (strlen(szDirPath) >= dwSuffixLength)
        {
            dwPathLength = strlen(szDirPath) - dwSuffixLength;
            memcpy(szTmpDir, szDirPath, dwPathLength);
            szTmpDir[dwPathLength] = '\0';
#ifdef _DEBUG
            fprintf(stderr, "create tmp directory : %s\n", szTmpDir);
#endif
            mkdir(szTmpDir, 0755);
        }
        pcChr = strchr(++pcChr, '/');
    }

    return;
}
static int SendOutMsg(int ifdReq, char *szHTTPHeader,BYTE    *pbyReturnResult,DWORD dwReturnResultLen)
{
	struct iovec iov[2];
	struct msghdr MsgHeader;
	int		iRet;

	char	*pchHeaderPtr = szHTTPHeader;
	BYTE    *byResult = pbyReturnResult;
	
	MsgHeader.msg_name = NULL;
	MsgHeader.msg_namelen = 0;
	MsgHeader.msg_iov = iov;
	MsgHeader.msg_iovlen = 2;
	MsgHeader.msg_control = NULL;
	MsgHeader.msg_controllen = 0;
	MsgHeader.msg_flags = 0;
					
	iov[0].iov_base = pchHeaderPtr;
	iov[0].iov_len  = strlen(pchHeaderPtr);
	iov[1].iov_base = byResult;
	iov[1].iov_len  = dwReturnResultLen;
	
	if ((iRet = sendmsg(ifdReq, &MsgHeader, 0)) != strlen(pchHeaderPtr)+dwReturnResultLen) {
		printf(__FILE__":%s:%d sendmsg error, iRet = %d (%s)\n", __func__, __LINE__, iRet,strerror(errno));
		return -1;
	}
	return 0;
}
SCODE Psiad_Loop(TPsiadInfo *ptPsiadInfo)
{
  
  	fd_set	fdsRead;
	struct timeval	timeout;
	int		fdIPC=ptPsiadInfo->iHttpFdipcSck;
	int		fdReq;
	int		iRet;
	int		iReadSz;
	char *szMsg= (char  *)malloc(MAX_BUFFER);
	TPsiaMsgInfo *ptMsgInfo=(TPsiaMsgInfo *)malloc(sizeof(TPsiaMsgInfo));
	TPsiaInputMsgInfo *ptInputMsgInfo=(TPsiaInputMsgInfo *)malloc(sizeof(TPsiaInputMsgInfo));
	TPsiaOutputMsgInfo *ptOutputMsgInfo=(TPsiaOutputMsgInfo *)malloc(sizeof(TPsiaOutputMsgInfo));
	memset(szMsg,0x0,MAX_BUFFER);
	memset(ptMsgInfo,0x0,sizeof(TPsiaMsgInfo));
	memset(ptInputMsgInfo,0x0,sizeof(TPsiaInputMsgInfo));
	memset(ptOutputMsgInfo,0x0,sizeof(TPsiaOutputMsgInfo));
	
	
	
	ptOutputMsgInfo->pbyReturnResult=(BYTE *)malloc(MAX_BUFFER+1);
	memset(ptOutputMsgInfo->pbyReturnResult,0x0,MAX_BUFFER+1);	
	ptOutputMsgInfo->eProcResult = eomsgprocres_error;
	ptOutputMsgInfo->eStatusCodeIndex = est_no_error;	
	memset(ptOutputMsgInfo->szHTTPHeader,0x0,sizeof(ptOutputMsgInfo->szHTTPHeader));
	
	ptPsiadInfo->eCurrentProcMsg=emsg_unknown;
	
	while(1) 
	{
		if (ptPsiadInfo->bTerminate)
		{
			break;
		}
		
		
		FD_ZERO(&fdsRead);
		FD_SET(fdIPC, &fdsRead);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		
		// select the socket (the incoming message from fdipc)
		iRet = select(fdIPC + 1, &fdsRead, NULL, NULL, &timeout);
		if (iRet < 0) {	//if error occurs
			printf(__FILE__":%s:%d select error\n", __func__, __LINE__);
			continue;
		} else if (iRet == 0) { //if select timeout
			//debug_printf(__FILE__":%s:%d [Onvifd] select timeout\n", __func__, __LINE__);
			continue;
		}
		if (FD_ISSET(fdIPC, &fdsRead)) { 
		  // receive buffer
		  //TODO:The conformance testing tool (version 2.0.0.2) always set "Content-Type" as "application/xml" and does not set "Accept" header.
		  //So the conformance testing tool can not upload the binary file (ex:*.deb,*.tar.gz,...) to the server successfully.
		  //Two interfaces: /PSIA/System/updateFirmware and /PSIA/System/configurationData can not be implemented correctly for the conformance testing.
		  iReadSz = fdipc_recv(fdIPC, (char *)szMsg, MAX_BUFFER, &fdReq);
		  Psiad_ParsingMsg(ptPsiadInfo, szMsg, ptMsgInfo,ptInputMsgInfo);
		  Psiad_Dispatch2Service(ptPsiadInfo, ptMsgInfo->eMsgStatus, ptInputMsgInfo,ptOutputMsgInfo);

		  if(ptOutputMsgInfo->eProcResult==eomsgprocres_success)
		  {
		
		   if(ptOutputMsgInfo->pbyReturnResult!=NULL)
		   {
		      SendOutMsg(fdReq, ptOutputMsgInfo->szHTTPHeader,ptOutputMsgInfo->pbyReturnResult,ptOutputMsgInfo->dwReturnResultLen);
		   }
		   
		  }
		  else
		  {
		    printf("[psiad](%s:%d)There are errors when proceessing service (%d)!!\n",__FILE__,__LINE__,ptMsgInfo->eMsgStatus);
		  }
		 memset(ptOutputMsgInfo->pbyReturnResult,0x0,MAX_BUFFER+1);	
		 ptOutputMsgInfo->eProcResult = eomsgprocres_error;
		  ptOutputMsgInfo->eStatusCodeIndex = est_no_error;	
		  memset(ptOutputMsgInfo->szHTTPHeader,0x0,sizeof(ptOutputMsgInfo->szHTTPHeader));
		  close(fdReq);
		}
		
	}

	if (szMsg != NULL)
	{
	  free(szMsg);	
	}
	if(ptMsgInfo!=NULL)
	{
	  free(ptMsgInfo);
	}
	
	if(ptInputMsgInfo->pbyInboundData != NULL)
	{
	  free(ptInputMsgInfo->pbyInboundData);
	}
	if(ptInputMsgInfo!=NULL)
	{
	  free(ptInputMsgInfo);
	
	}
	if(ptOutputMsgInfo->pbyReturnResult != NULL)
	{
	  free(ptOutputMsgInfo->pbyReturnResult);
	}
	if(ptOutputMsgInfo !=NULL)
	{
	  free(ptOutputMsgInfo);
	}  
	 
      return S_OK;

}
SCODE Psiad_ParsingMsg(TPsiadInfo *ptPsiadInfo, CHAR *szMsg, TPsiaMsgInfo *ptMsgInfo,TPsiaInputMsgInfo *ptInputMsgInfo)
{ 

    char *szURL=NULL;
    char *szTemp=NULL;
    //char *szTemp1=NULL;
    
    char *szMethod=NULL;
 
    char *szContentType=NULL;
    char *szQueryStr=NULL;
    
    DWORD dwInboundDataLen=0;
    char *szContent=NULL;
    
    
    BOOL bPUT=FALSE;
    BOOL bGET=FALSE;
    BOOL bPOST=FALSE;
    BOOL bDELETE=FALSE;
    
     //printf("[psiad_loacl]%d szMsg=%s\n",__LINE__,szMsg);
    szURL=strtok(szMsg,"\0");
    // printf("[psiad_loacl]%d szURL=%s\n",__LINE__, szURL);
   szContent=szMsg+strlen(szURL)+1;
   // printf("[psiad_loacl](%d) ----------Begin of msg--------\n",__LINE__);
    //printf("[psiad_loacl](%d) %s\n",__LINE__,szContent);
    //printf("[psiad_loacl](%d)  ----------End of msg----------\n",__LINE__);
  
   //Only the /PSIA/System/factoryReset has query string.
   if(strncmp(szURL,FACTORYRESET_URI,strlen(szURL))==0){
    szTemp=strstr(szContent,"query_string=");
    szQueryStr=szTemp+strlen("query_string=");
    //printf("[psiad_loacl]%d szQueryStr=%s\n",__LINE__,szQueryStr);
    szTemp=strstr(szContent,"method=");
    szMethod=strtok(szTemp+strlen("method="),"&");
    //printf("[psiad_loacl]%d szMethod=%s\n",__LINE__,szMethod);
   }else{ 
    szTemp=strstr(szContent,"method=");
    //printf("[psiad_loacl]%d szTemp=%s\n",__LINE__,szTemp);
    szMethod=szTemp+strlen("method=");
     //printf("[psiad_loacl]%d szMethod=%s\n",__LINE__,szMethod);
   }
    szTemp=strstr(szContent,"content_type=");
     //printf("[psiad_loacl]%d szTemp=%s\n",__LINE__,szTemp);
    szContentType=strtok(szTemp+strlen("content_type="),";");
     //printf("[psiad_loacl]%d szContentType=%s\n",__LINE__,  szContentType);
   
 
    bGET=(strncmp(szMethod,"GET",strlen(szMethod))==0);
    bPUT=(strncmp(szMethod,"PUT",strlen(szMethod))==0);
    bPOST=(strncmp(szMethod,"POST",strlen(szMethod))==0);
    bDELETE=(strncmp(szMethod,"DELETE",strlen(szMethod))==0);
    
   printf("[psiad_loacl]%d bGET=%d bPUT=%d bPOST=%d bDELETE=%d\n",__LINE__,bGET,bPUT,bPOST,bDELETE);
   
  //printf("[psiad_loacl](%d)\n",__LINE__);
    if(bPUT || bPOST) 
    {
      while(1)
      {
      
	if(*szContent == '&' && *(szContent+1) == '&')
	{
	  break;
	}
	szContent++;
	dwInboundDataLen++;
	
      }
    }
    szContent=szMsg+strlen(szURL)+1;
  //  printf("[psiad_loacl](%d)dwInboundDataLen=%d\n",__LINE__,dwInboundDataLen);

   ptInputMsgInfo->dwInboundDataLen=dwInboundDataLen;
   
    ptInputMsgInfo->pbyInboundData=(BYTE *)malloc(dwInboundDataLen+1);
    memset(ptInputMsgInfo->pbyInboundData,0x0,dwInboundDataLen+1);
   
   memset(ptInputMsgInfo->szQueryString,0x0,sizeof(ptInputMsgInfo->szQueryString));
   memset(ptInputMsgInfo->szURI,0x0,sizeof(ptInputMsgInfo->szURI));
   ptMsgInfo->eMsgStatus=emsg_unknown;
   
   if((ptInputMsgInfo->szURI!=NULL) && (szURL!=NULL))
   {
      strncpy(ptInputMsgInfo->szURI,szURL,strlen(szURL));
   }
  // printf("[psiad_loacl](%d)\n",__LINE__);

   if((ptInputMsgInfo->pbyInboundData!=NULL) && (szContent!=NULL))
   {
	 strncpy(ptInputMsgInfo->pbyInboundData,szContent,dwInboundDataLen);
	//  printf("ptInputMsgInfo->pbyInboundData=%s\n",ptInputMsgInfo->pbyInboundData);
   }
    //printf("[psiad_loacl](%d)\n",__LINE__);
   if((ptInputMsgInfo->szQueryString!=NULL) && (szQueryStr!=NULL) )
   {
      strncpy(ptInputMsgInfo->szQueryString,szQueryStr,strlen(szQueryStr));
   }
    //printf("[psiad_loacl](%d)\n",__LINE__);
    if(strncmp(szURL,REBOOT_URI,strlen(szURL))==0)
    {
      
         if(bPUT)
	 {
	  ptMsgInfo->eMsgStatus=emsg_reboot;
	 }
	
    
    }
    else if(strncmp(szURL,UPDATEFIRMWARE_URI,strlen(szURL))==0)
    {
	 if(bPUT)
	 {
	  ptMsgInfo->eMsgStatus=emsg_updateFirmware;
	 }
	
    }
    else if(strncmp(szURL,CONFIGURATIONDATA_URI,strlen(szURL))==0)
    {
    
       if(bPUT)
       {	
	ptMsgInfo->eMsgStatus=emsg_putConfigurateData;
       }
       else if(bGET)
       {
	ptMsgInfo->eMsgStatus=emsg_getConfigurateData;
       }
    }
    else if(strncmp(szURL,FACTORYRESET_URI,strlen(szURL))==0)
    {
      if(bPUT)
      {
	  ptMsgInfo->eMsgStatus=emsg_factoryReset;
      }
	
    
    }
    else if(strncmp(szURL,DEVICEINFO_URI,strlen(szURL))==0)
    {
    
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putDeviceInfo;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getDeviceInfo;
       }
    }
    else if(strncmp(szURL,SUPPORTREPORT_URI,strlen(szURL))==0)
    {
	 if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getSupportReport;
	}
    
    }
    else if(strncmp(szURL,STATUS_URI,strlen(szURL))==0)
    {
       if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getDeviceStatus;
       }
    }
    else if(strncmp(szURL,TIME_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putTime;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getTime;
       }
    
    }
    else if(strncmp(szURL,LOCALTIME_URI,strlen(szURL))==0)
    {
	if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putLocalTime;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getLocalTime;
       }
    }
    else if(strncmp(szURL,TIMEZONE_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putTimeZone;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getTimeZone;
       }
    
    }
    else if(strncmp(szURL,NTPSERVERS_URI,strlen(szURL))==0)
    {
        if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putNTPServers;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getNTPServers;
       }
       else if(bPOST)
       {
	 ptMsgInfo->eMsgStatus=emsg_postNTPServers;
       }
      
    }
    else if(strncmp(szURL,NTPSERVERS_0_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putNTPServer;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getNTPServer;
       }
       else if(bDELETE)
       {
	  ptMsgInfo->eMsgStatus=emsg_deleteNTPServer;
       }
    
    }
    else if(strncmp(szURL,LOGGING_URI,strlen(szURL))==0)
    {
        if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putLogging;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getLogging;
       }
    
    }
    else if(strncmp(szURL,LOGGING_MESSAGES_URI,strlen(szURL))==0)
    {
       if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getLoggingMessages;
       }
    
    }
    else if(strncmp(szURL,VOLUMES_URI,strlen(szURL))==0)
    {
       if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getStorageVolumes;
       }
    
    }
    else if(strncmp(szURL,VOLUMES_0_URI,strlen(szURL))==0)
    {
       if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getStorageVolume;
       }
    }
    else if(strncmp(szURL,VOLUMES_0_STATUS_URI,strlen(szURL))==0)
    {	
	if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getStorageVolumeStatus;
       }
    
    }
    else if(strncmp(szURL,FORMAT_URI,strlen(szURL))==0)
    {
      if(bPUT)
       {
	  ptMsgInfo->eMsgStatus=emsg_fomatStorageVolume;
       }
    }
    else if(strncmp(szURL,FILES_URI,strlen(szURL))==0)
    {	
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getStorageVolumeFiles;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteStorageVolumeFiles;
	}
    }
    else if(strncmp(szURL,FILES_0_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getStorageVolumeFile;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteStorageVolumeFile;
	}
    
    }
    else if(strncmp(szURL,FILES_0_DATA_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getStorageVolumeData;
	}
    
    }
    else if(strncmp(szURL,INTERFACES_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getNetworkInterfaces;
	}
    
    }
    else if(strncmp(szURL,INTERFACES_0_URI,strlen(szURL))==0)
    {
        if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getNetworkInterface;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putNetworkInterface;
	}
    
    }
    else if(strncmp(szURL,INTERFACES_0_IPADDRESS_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getIPAddress;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putIPAddress;
	}
      
    }
    else if(strncmp(szURL,INTERFACES_0_WIRELESS_URI,strlen(szURL))==0)
    {
        if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getWireless;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putWireless;
	}
    
    }
      else if(strncmp(szURL,INTERFACES_0_IEEE802_1X_URI,strlen(szURL))==0)
    {
        if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getIEEE8021x;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putIEEE8021x;
	}
    }
      else if(strncmp(szURL,INTERFACES_0_IPFILTER_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getIPFilter;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putIPFilter;
	}
    }
      else if(strncmp(szURL,INTERFACES_0_IPFILTER_FILTERADDRESSES_URI,strlen(szURL))==0)
    {
	if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putFilterAddresses;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getFilterAddresses;
       }
       else if(bPOST)
       {
	  ptMsgInfo->eMsgStatus=emsg_postFilterAddresses;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteFilterAddresses;
	}
    
    }
     else if(strncmp(szURL,INTERFACES_0_IPFILTER_FILTERADDRESSES_0_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putFilterAddress;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getFilterAddress;
       }
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteFilterAddress;
	}
    }
    else if(strncmp(szURL,INTERFACES_0_SNMP,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmp;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmp;
       }
      
      
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_V2C_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpv2c;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpv2c;
       }
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_V2C_TRAPRECEIVERS_URI,strlen(szURL))==0)
    {
	if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpv2cTrapReceivers;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpv2cTrapReceivers;
       }
       else if(bPOST)
       {
	  ptMsgInfo->eMsgStatus=emsg_postSnmpv2cTrapReceivers;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpv2cTrapReceivers;
	}
      
      
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_V2C_TRAPRECEIVERS_0_URI,strlen(szURL))==0)
    {
	if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpv2cTrapReceiver;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpv2cTrapReceiver;
       }
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpv2cTrapReceiver;
	}
      
    }
    else if(strncmp(szURL,INTERFACES_0_SNMP_ADVANCED_URI,strlen(szURL))==0)
    {
	if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvanced;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvanced;
       }
    }
    else if(strncmp(szURL,INTERFACES_0_SNMP_ADVANCED_USERS_URI,strlen(szURL))==0)
    {
      if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedUsers;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvancedUsers;
       }
       else if(bPOST)
       {
	  ptMsgInfo->eMsgStatus=emsg_postSnmpAdvancedUsers;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpAdvancedUsers;
	}
    
      
    }
     else if(strncmp(szURL, INTERFACES_0_SNMP_ADVANCED_USERS_0_URI,strlen(szURL))==0)
    {
      
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedUser;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvancedUser;
       }
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpAdvancedUser;
	}
      
      
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONFILTERS_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedNotificationFilters;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvancedNotificationFilters;
       }
       else if(bPOST)
       {
	  ptMsgInfo->eMsgStatus=emsg_postSnmpAdvancedNotificationFilters;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpAdvancedNotificationFilters;
	}
    }
     else if(strncmp(szURL, INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONFILTERS_0_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {
	 ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedNotificationFilter;
       }
       else if(bGET)
       {
	ptMsgInfo->eMsgStatus= emsg_getSnmpAdvancedNotificationFilter;
       }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteSnmpAdvancedNotificationFilter;
      }
  
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONRECEIVERS_URI,strlen(szURL))==0)
    {
      if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedNotificationReceivers;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvancedNotificationReceivers;
       }
       else if(bPOST)
       {
	  ptMsgInfo->eMsgStatus=emsg_postSnmpAdvancedNotificationReceivers;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpAdvancedNotificationReceivers;
	}
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONRECEIVERS_0_URI,strlen(szURL))==0)
    {	
	 if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedNotificationReceiver;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvancedNotificationReceiver;
       }
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSnmpAdvancedNotificationReceiver;
	}
      
      
    }
     else if(strncmp(szURL,INTERFACES_0_SNMP_V3_URI,strlen(szURL))==0)
    {
      if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSnmpAdvancedv3;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSnmpAdvancedv3;
       }
    }
     else if(strncmp(szURL,INTERFACES_0_QOS_URI,strlen(szURL))==0)
    {
      
      if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putQos;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getQos;
       }
      
    }
     else if(strncmp(szURL, INTERFACES_0_QOS_COS_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putCoses;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getCoses;
       }
       else if(bPOST)
       {
	ptMsgInfo->eMsgStatus=emsg_postCoses;
       }
       else if(bDELETE)
       {
	ptMsgInfo->eMsgStatus=emsg_deleteCoses;
       }
      
    }
    else if(strncmp(szURL,INTERFACES_0_QOS_COS_0_URI,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putCos;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getCos;
       }
       else if(bDELETE)
       {
	ptMsgInfo->eMsgStatus=emsg_deleteCos;
       }
    
    }
    else if(strncmp(szURL,INTERFACES_0_QOS_DSCP_URI,strlen(szURL))==0)
    {
	if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putDscps;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getDscps;
       }
       else if(bPOST)
       {
	ptMsgInfo->eMsgStatus=emsg_postDscps;
       }
       else if(bDELETE)
       {
	ptMsgInfo->eMsgStatus=emsg_deleteDscps;
       }
    
    }
     else if(strncmp(szURL,INTERFACES_0_QOS_DSCP_0_URI ,strlen(szURL))==0)
    {
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putDscp;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getDscp;
       }
       else if(bDELETE)
       {
	ptMsgInfo->eMsgStatus=emsg_deleteDscp;
       }
    }
     else if(strncmp(szURL,INTERFACES_0_DISCOVERY_URI,strlen(szURL))==0)
    {
      if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putDiscovery;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getDiscovery;
       }
    }
     else if(strncmp(szURL,INTERFACES_0_SYSLOG_URI,strlen(szURL))==0)
    {
      if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSyslog;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getSyslog;
       }
    }
    else if(strncmp(szURL,INTERFACES_0_SYSLOG_SERVERS_URI,strlen(szURL))==0)
    {
      
      
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSyslogServers;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus= emsg_getSyslogServers;
       }
       else if(bPOST)
       {
	ptMsgInfo->eMsgStatus= emsg_postSyslogServers;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSyslogServers;
	}
 
    }
    else if(strncmp(szURL,INTERFACES_0_SYSLOG_SERVERS_0_URI,strlen(szURL))==0)
    {
      
       if(bPUT)
       {	
	  ptMsgInfo->eMsgStatus=emsg_putSyslogServer;
       }
       else if(bGET)
       {
	  ptMsgInfo->eMsgStatus= emsg_getSyslogServer;
       }
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteSyslogServer;
	}
    }
     else if(strncmp(szURL,IO_URI,strlen(szURL))==0)
    {
       if(bGET)
       {
	  ptMsgInfo->eMsgStatus=emsg_getIOPorts;
       }
    }
    else if(strncmp(szURL,IO_STATUS_URI,strlen(szURL))==0)
    { 	
	if(bGET)
        {
	  ptMsgInfo->eMsgStatus=emsg_getIOPortStatuses;
	}
    }
     else if(strncmp(szURL,IO_INPUTS_URI,strlen(szURL))==0)
    {
      	if(bGET)
        {
	  ptMsgInfo->eMsgStatus=emsg_getInputPorts;
	}
    }
     else if(strncmp(szURL, IO_INPUTS_0_URI,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	    ptMsgInfo->eMsgStatus=emsg_getInputPort;
	  }
	  else if(bPUT)
	  {
	    ptMsgInfo->eMsgStatus=emsg_putInputPort;
	  }
      
    }
     else if(strncmp(szURL,IO_INPUTS_0_STATUS_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getIOPortStatus;
	}
    }
     else if(strncmp(szURL,IO_OUTPUTS_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getOutputPorts;
	}
    }
     else if(strncmp(szURL,IO_OUTPUTS_0_URI,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	    ptMsgInfo->eMsgStatus=emsg_getOutputPort;
	  }
	  else if(bPUT)
	  {
	    ptMsgInfo->eMsgStatus=emsg_putOutputPort;
	  }
    }
    else if(strncmp(szURL, IO_OUTPUTS_0_TRIGGER_URI,strlen(szURL))==0)
    {
       if(bPUT)
	{
	    ptMsgInfo->eMsgStatus=emsg_setIOportTrigger;
	}
      
    }
    else if(strncmp(szURL, IO_OUTPUTS_0_STATUS_URI,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getIOPortStatus;
	}
    }
    else if(strncmp(szURL,AUDIO_CHANNELS_URI,strlen(szURL))==0)
    {	
      if(bGET)
      {
	  ptMsgInfo->eMsgStatus=emsg_getAudioChannels;
      }
    }
    else if(strncmp(szURL,AUDIO_CHANNELS_0_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getAudioChannel;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putAudioChannel;
      }
    }
    else if(strncmp(szURL,VIDEO_OVERLAYIMAGES_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoOverlayImages;
      }
      else if(bPOST)
      {
	ptMsgInfo->eMsgStatus=emsg_postVideoOverlayImages;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoOverlayImages;
      
      }
      
    }
    else if(strncmp(szURL,VIDEO_OVERLAYIMAGES_0_URI,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoOverlayImage;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoOverlayImage;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoOverlayImage;
      
      }
      
    }
    else if(strncmp(szURL,VIDEO_INPUTS_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoInput;
      }
    
    }
    else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoInputChannels;
      }
    }
    else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoInputChannel;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoInputChannel;
      }
      
    }
    else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_FOCUS_URI,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setVideoFocus;
      }
    }
    else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_IRIS_URI,strlen(szURL))==0)
    {
       if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setVideoIRIS;
      }
    }
    else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_LENS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
      ptMsgInfo->eMsgStatus=emsg_getVideoLens;
      }
    } 
    else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_OVERLAYS_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoOverlay;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoOverlay;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoOverlay;
      }
    }
    else if(strncmp(szURL, VIDEO_INPUTS_CHANNELS_0_OVERLAYS_TEXT_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoTextOverlays;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoTextOverlays;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postVideoTextOverlays;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoTextOverlays;
      }
    }
     else if(strncmp(szURL,  VIDEO_INPUTS_CHANNELS_0_OVERLAYS_TEXT_0_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoTextOverlay;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoTextOverlay;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoTextOverlay;
      }
      
    }
     else if(strncmp(szURL, VIDEO_INPUTS_CHANNELS_0_OVERLAYS_IMAGE_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoImageOverlays;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoImageOverlays;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postVideoImageOverlays;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoImageOverlays;
      }
      
    }
     else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_OVERLAYS_IMAGE_0_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoImageOverlay;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoImageOverlay;
      }
     else if(bDELETE)
     {
       ptMsgInfo->eMsgStatus=emsg_deleteVideoImageOverlay;
     }

      
    }
     else if(strncmp(szURL, VIDEO_INPUTS_CHANNELS_0_PRIVACYMASK_URI  ,strlen(szURL))==0)
    {
      
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoPrivacyMask;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoPrivacyMask;
      }
    }
     else if(strncmp(szURL,VIDEO_INPUTS_CHANNELS_0_PRIVACYMASK_REGIONS_URI  ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoPrivacyMaskRegions;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoPrivacyMaskRegions;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postVideoPrivacyMaskRegions;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoPrivacyMaskRegions;
      }
    }
     else if(strncmp(szURL,  VIDEO_INPUTS_CHANNELS_0_PRIVACYMASK_REGIONS_0_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getVideoPrivacyMaskRegion;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putVideoPrivacyMaskRegion;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteVideoPrivacyMaskRegion;
      }
    }
     else if(strncmp(szURL,  SERIAL_PORTS_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getSerialPorts;
      }
    }
     else if(strncmp(szURL, SERIAL_PORTS_0_URI  ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getSerialPort;
      }
      else if(bPUT)
      {	
	ptMsgInfo->eMsgStatus=emsg_putSerialPort;
      
      }
    }
     else if(strncmp(szURL, SERIAL_PORTS_0_COMMAND_URI ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_sendCommandToSerailPort;
      }
    }
     else if(strncmp(szURL, DIAGNOSTICS_COMMANDS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getDiagnosticsCommands;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postDiagnosticsCommands;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteDiagnosticsCommands;
      }
      
    }
     else if(strncmp(szURL, DIAGNOSTICS_COMMANDS_0_URI  ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getDiagnosticsCommand;
      }
       else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteDiagnosticsCommand;
      }
      
    }
    else if(strncmp(szURL,SECURITY_SRTPMASTERKEY_URI ,strlen(szURL))==0)
    {
      
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getSRTPMasterKey;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putSRTPMasterKey;
      }
    }
    else if(strncmp(szURL,SECURITY_DEVICECERTIFICATE_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getDeviceCert;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putDeviceCert;
      }
    }
    else if(strncmp(szURL,SECURITY_AAA_USERS_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getUsers;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putUsers;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postUsers;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteUsers;
      }
    }
    else if(strncmp(szURL,SECURITY_AAA_USERS_0_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getUser;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putUser;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteUser;
      }
    }
    else if(strncmp(szURL,SECURITY_AAA_CERTIFICATE_URI  ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getAAACert;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putAAACert;
      }
       
    }
    else if(strncmp(szURL,SECURITY_AAA_ADMINACCESSES_URI  ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getAdminAccesses;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putAdminAccesses;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postAdminAccesses;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteAdminAccesses;
      }
      
    }
    else if(strncmp(szURL, SECURITY_AAA_ADMINACCESSES_0_URI ,strlen(szURL))==0)
    {
      
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getAdminAccess;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putAdminAccess;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteAdminAccess;
      }
    }
    else if(strncmp(szURL,STREAMING_STATUS_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getStreamingStatus;
      }
    }
    else if(strncmp(szURL,STREAMING_CHANNELS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getStreamingChannels;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putStreamingChannels;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postStreamingChannels;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteStreamingChannels;
      }
      
      
    }
    else if(strncmp(szURL, STREAMING_CHANNELS_0_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getStreamingChannel;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putStreamingChannel;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteStreamingChannel;
      }
      
    }
    else if(strncmp(szURL, STREAMING_CHANNELS_0_STATUS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getStreamingSessionStatus;
      }
    }
    else if(strncmp(szURL,STREAMING_CHANNELS_0_HTTP_URI ,strlen(szURL))==0)
    {
      //This is the uri for requesting stream through HTTP. Does not be implemneted in the psiad.
      if(bPOST)
      {
	ptMsgInfo->eMsgStatus=emsg_postStreaming;
      }
      else if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getStreaming;
      }
	
    }
    else if(strncmp(szURL,STREAMING_CHANNELS_0_PICTURE_URI ,strlen(szURL))==0)
    {
      //This is the uri for requesting snapshots through HTTP. Does not be implemneted in the psiad.
      if(bPOST)	
      {
	ptMsgInfo->eMsgStatus=emsg_postSnapshot;
      }
      else if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getSnapshot;
      }
    }
    else if(strncmp(szURL,STREAMING_CHANNELS_0_REQUESTKEYFRAME_URI ,strlen(szURL))==0)
    {
      //This is the uri for requesting keyframes through HTTP. Does not be implemneted in the psiad.
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_requstKeyFrame;
      }
    }
    else if(strncmp(szURL,PTZ_CHANNELS_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZhannels;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZChannels;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postPTZChannels;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deletePTZChannels;
      }
      
    }
    else if(strncmp(szURL,PTZ_CHANNELS_0_URI ,strlen(szURL))==0)
    {
    
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZhannel;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZChannel;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deletePTZChannel;
      }
    }
    else if(strncmp(szURL,PTZ_CHANNELS_0_HOMEPOSITION_URI  ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setPTZHomePosition;
      }
    }
    else if(strncmp(szURL,  PTZ_CHANNELS_0_CONTINUOUS_URI,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setPTZContinuous;
      }
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_MOMENTARY_URI ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setPTZMomentary;
      }
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_RELATIVE_URI ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setPTZRelative;
      }
    }
    else if(strncmp(szURL,  PTZ_CHANNELS_0_ABSOLUTE_URI  ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setPTZAbsolute;
      }
    
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_DIGITAL_URI  ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_setPTZDigital;
      }
    
    }
    else if(strncmp(szURL,  PTZ_CHANNELS_0_STATUS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZStatus;
	
      }
    
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_PRESETS_URI  ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPresets;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZPresets;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postPTZPresets;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deletePTZPresets;
      }
      
    
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_PRESETS_0_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPreset;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZPreset;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deletePTZPreset;
      }
    
    }
    else if(strncmp(szURL,  PTZ_CHANNELS_0_PRESETS_0_GOTO_URI  ,strlen(szURL))==0)
    {
	if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_gotoPresetPosition;
	}
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_PATROLS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPatrols;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZPatrols;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postPTZPatrols;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deletePTZPatrols;
      }
    
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_PATROLS_STATUS_URI ,strlen(szURL))==0)
    {
    
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPatrolStatuses;
      }
    }
    else if(strncmp(szURL,  PTZ_CHANNELS_0_PATROLS_0_URI ,strlen(szURL))==0)
    {
    
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPatrol;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZPatrol;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deletePTZPatrol;
      }
    }
    else if(strncmp(szURL,PTZ_CHANNELS_0_PATROLS_0_START_URI  ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_startPTZPatrol;
      }
    }
    else if(strncmp(szURL, PTZ_CHANNELS_0_PATROLS_0_STOP_URI ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_stopPTZPatrol;
      }
      
    }
    else if(strncmp(szURL,PTZ_CHANNELS_0_PATROLS_0_PAUSE_URI ,strlen(szURL))==0)
    {
      if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_pausePTZPatrol;
      }
       
    }
    else if(strncmp(szURL,PTZ_CHANNELS_0_PATROLS_0_STATUS_URI  ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPatrolStatus;
      }
    }
    else if(strncmp(szURL,  PTZ_CHANNELS_0_PATROLS_0_SCHEDULE_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getPTZPatrolSchedule;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putPTZPatrolSchedule;
      }

    
    }
    else if(strncmp(szURL, CUSTOM_MOTIONDETECTION_URI  ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getMotionDetctions;
      }
    }
    else if(strncmp(szURL,  CUSTOM_MOTIONDETECTION_0_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getMotionDetction;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putMotionDetction;
      }
	
    }
     else if(strncmp(szURL,CUSTOM_MOTIONDETECTION_0_REGIONS_URI ,strlen(szURL))==0)
    {		
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getMotionDetectionRegions;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putMotionDetectionRegions;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postMotionDetectionRegions;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteMotionDetectionRegions;
      }
    
    }
     else if(strncmp(szURL,  CUSTOM_MOTIONDETECTION_0_REGIONS_0_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getMotionDetectionRegion;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putMotionDetectionRegion;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteMotionDetectionRegion;
      }
    }
     else if(strncmp(szURL, CUSTOM_EVENT_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventNotification;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventNotification;
      }
    }
     else if(strncmp(szURL,CUSTOM_EVENT_TRIGGERS_URI  ,strlen(szURL))==0)
    {
    
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventTriggers;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventTriggers;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postEventTriggers;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteEventTriggers;
      }
      
    }
     else if(strncmp(szURL,CUSTOM_EVENT_TRIGGERS_0_URI ,strlen(szURL))==0)
    {
        if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventTrigger;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventTrigger;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteEventTrigger;
      }
    
    }
    else if(strncmp(szURL,CUSTOM_EVENT_TRIGGERS_0_NOTIFICATIONS_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventTriggerNontifications;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventTriggerNontifications;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postEventTriggerNontifications;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteEventTriggerNontifications;
      }
      
    }
     else if(strncmp(szURL,  CUSTOM_EVENT_TRIGGERS_0_NOTIFICATIONS_0_URI,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventTriggerNontification;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventTriggerNontification;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteEventTriggerNontification;
      }
    
    }
     else if(strncmp(szURL,CUSTOM_EVENT_SCHEDULE_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventSchedule;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventSchedule;
      }
    
    }
     else if(strncmp(szURL, CUSTOM_EVENT_NOTIFICATION_URI ,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getEventNotificationMethods;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putEventNotificationMethods;
      }
    
    }
     else if(strncmp(szURL,  CUSTOM_EVENT_NOTIFICATION_MAILING_URI,strlen(szURL))==0)
    {
      if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getMailingNontifications;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putMailingNontifications;
      }
      else if(bPOST)
      {
      	ptMsgInfo->eMsgStatus=emsg_postMailingNontifications;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteMailingNontifications;
      }
    
    }
     else if(strncmp(szURL,  CUSTOM_EVENT_NOTIFICATION_MAILING_0_URI,strlen(szURL))==0)
    {
       if(bGET)
      {
	ptMsgInfo->eMsgStatus=emsg_getMailingNontification;
      }
      else if(bPUT)
      {
	ptMsgInfo->eMsgStatus=emsg_putMailingNontification;
      }
      else if(bDELETE)
      {
	ptMsgInfo->eMsgStatus=emsg_deleteMailingNontification;
      }
    
    }
     else if(strncmp(szURL, CUSTOM_EVENT_NOTIFICATION_FTP_URI ,strlen(szURL))==0)
    {	 
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getFTPNontifications;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putFTPNontifications;
	}
	else if(bPOST)
	{
	  ptMsgInfo->eMsgStatus=emsg_postFTPNontifications;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteFTPNontifications;
	}
    
    }
    else if(strncmp(szURL,  CUSTOM_EVENT_NOTIFICATION_FTP_0_URI ,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getFTPNontification;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putFTPNontification;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteFTPNontification;
	}
    }
    else if(strncmp(szURL,  CUSTOM_EVENT_NOTIFICATION_HTTPHOST_URI ,strlen(szURL))==0)
    {	
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getHTTPHostNontifications;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putHTTPHostNontifications;
	}
	else if(bPOST)
	{
	  ptMsgInfo->eMsgStatus=emsg_postHTTPHostNontifications;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteHTTPHostNontifications;
	}
    
    }
    else if(strncmp(szURL,  CUSTOM_EVENT_NOTIFICATION_HTTPHOST_0_URI ,strlen(szURL))==0)
    {
	if(bGET)
	{
	  ptMsgInfo->eMsgStatus=emsg_getHTTPHostNontification;
	}
	else if(bPUT)
	{
	  ptMsgInfo->eMsgStatus=emsg_putHTTPHostNontification;
	}
	else if(bDELETE)
	{
	  ptMsgInfo->eMsgStatus=emsg_deleteHTTPHostNontification;
	}
    }
    else if(strncmp(szURL,  CUSTOM_EVENT_NOTIFICATION_ALERTSTREAM_URI ,strlen(szURL))==0)
    {
      if(bGET)
      {
	  ptMsgInfo->eMsgStatus=emsg_getEventNotificationAlert;
      }
    }
    else if(strncmp(szURL,  ROOTSERVICE_INDEX  ,strlen(szURL))==0)
    {
      if(bGET)
      {
	  ptMsgInfo->eMsgStatus=emsg_getRootServiceIndex;
      }
    }
    else if(strncmp(szURL,  ROOTSERVICE_INDEXR  ,strlen(szURL))==0)
    {
      if(bGET)
      {
	  ptMsgInfo->eMsgStatus=emsg_getRootServiceIndexr;
      }
    }
    else if(strncmp(szURL,  ROOTSERVICE_DESCRIPTION   ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getRootServiceDescription;
	  }
    }
    else if(strncmp(szURL, SYSTEM_INDEX ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemIndex;
	  }
    }
    else if(strncmp(szURL,  SYSTEM_DESCRIPTION   ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemDescription;
	  }
    }
    else if(strncmp(szURL, SYSTEM_NETWORK_INDEX    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemNetworkIndex;
	  }
    }
    else if(strncmp(szURL,  SYSTEM_NETWORK_DESCRIPTION  ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemNetworkDescription;
	  }
    }
    else if(strncmp(szURL,  SYSTEM_IO_INDEX    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemIOIndex;
	  }
    }
    else if(strncmp(szURL, SYSTEM_IO_DESCRIPTION   ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemIODescription;
	  }
    }
    else if(strncmp(szURL,  SYSTEM_AUDIO_INDEX   ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemAudioIndex;
	  }
    }
    else if(strncmp(szURL,  SYSTEM_AUDIO_DESCRIPTION    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemAudioDescription;
	  }
    }
    else if(strncmp(szURL,  SYSTEM_VIDEO_INDEX    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemVideoIndex;
	  }
    }
    else if(strncmp(szURL, SYSTEM_VIDEO_DESCRIPTION    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSystemVideoDescription;
	  }
    }
    else if(strncmp(szURL,  STREAMING_INDEX  ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getStreamingIndex;
	  }
    }
    else if(strncmp(szURL,  STREAMING_DESCRIPTION   ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getStreamingDescription;
	  }
    }
    else if(strncmp(szURL,  PTZ_INDEX    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getPTZIndex;
	  }
    }
    else if(strncmp(szURL, PTZ_DESCRIPTION    ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getPTZDescription;
	  }
    }
    else if(strncmp(szURL, CUSTOM_MD_INDEX   ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getCustomMotionDetectionIndex;
	  }
    }
    else if(strncmp(szURL,CUSTOM_MD_DESCRIPTION  ,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getCustomMotionDetectionDescription;
	  }
    }
    else if(strncmp(szURL,SECUTITY_INDEX,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSecurityIndex;
	  }
    }
    else if(strncmp(szURL,SECUTITY_DESCRIPTION,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSecurityDescription;
	  }
    }
    else if(strncmp(szURL,SECUTITY_AAA_INDEX,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSecurityAAAIndex;
	  }
    }
    else if(strncmp(szURL,SECUTITY_AAA_DESCRIPTION,strlen(szURL))==0)
    {
	  if(bGET)
	  {
	      ptMsgInfo->eMsgStatus=emsg_getSecurityAAADescription;
	  }
    }
    else
    {
	ptMsgInfo->eMsgStatus=emsg_unknown;
    }
  //printf("[psiad_loacl] %d ptMsgInfo->eMsgStatus=%d\n",__LINE__,ptMsgInfo->eMsgStatus);
  return S_OK;

}




SCODE Psiad_Dispatch2Service(TPsiadInfo *ptPsiadInfo, EPsiaMsgStatus eMsgStatus, TPsiaInputMsgInfo *ptInputMsgInfo,  TPsiaOutputMsgInfo *ptOutputMsgInfo)
{


    
     ptPsiadInfo->eCurrentProcMsg=eMsgStatus;
   
    switch (eMsgStatus) {
      case emsg_reboot:
	  Psiad_Reboot(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
      case emsg_updateFirmware:
	Psiad_UpdateFirmware(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	 break;
      case emsg_getConfigurateData:
	Psiad_GetConfigurationData(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putConfigurateData:
	Psiad_PutConfigurationData(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_factoryReset:
	FactoryReset(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getDeviceInfo:
//	printf("[psiad_local](%d)\n",__LINE__);
	Psiad_GetDeviceInfo(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;

      case emsg_putDeviceInfo:
//	printf("[psiad_local](%d)\n",__LINE__);
	Psiad_PutDeviceInfo(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getSupportReport:
	Psiad_SupportReport(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getDeviceStatus:
	Psiad_GetDeviceStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getTime:
	Psiad_GetTime(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putTime:
	Psiad_PutTime(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getLocalTime:
	Psiad_GetLocalTime(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putLocalTime:
	Psiad_PutLocalTime(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getTimeZone:
	 Psiad_GetTimeZone(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putTimeZone:
	Psiad_PutTimeZone(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getNTPServers:
	Psiad_GetNTPServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putNTPServers:
	Psiad_PutNTPServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_postNTPServers:
	Psiad_PostNTPServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_deleteNTPServers:
	Psiad_DeleteNTPServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getNTPServer:
	Psiad_GetNTPServer(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putNTPServer:
	 Psiad_PutNTPServer(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_deleteNTPServer:
	 Psiad_DeleteNTPServer(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getLogging:
	Psiad_GetLogging(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_putLogging:
	Psiad_PutLogging(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getLoggingMessages:
	Psiad_GetLoggingMessages(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getStorageVolumes:
	Psiad_GetStorageVolumes(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getStorageVolume:
	Psiad_GetStorageVolume(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getStorageVolumeStatus:
	Psiad_GetStorageVolumeStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_fomatStorageVolume:
	Psiad_FomatStorageVolume(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_getStorageVolumeFiles:
	Psiad_GetStorageVolumeFiles(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
      case emsg_deleteStorageVolumeFiles:
	Psiad_DeleteStorageVolumeFiles(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStorageVolumeFile:
	  Psiad_GetStorageVolumeFile(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteStorageVolumeFile:
	  Psiad_DeleteStorageVolumeFile(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStorageVolumeData:
	  Psiad_GetStorageVolumeData(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getNetworkInterfaces:
	  Psiad_GetNetworkInterfaces(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getNetworkInterface:
	   Psiad_GetNetworkInterface(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putNetworkInterface:
	   Psiad_PutNetworkInterface(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getIPAddress:
	  Psiad_GetIPAddress(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putIPAddress:
	  Psiad_PutIPAddress(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getWireless:
	  Psiad_GetWireless(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putWireless:
	  Psiad_PutWireless(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getIEEE8021x:
	  Psiad_GetIEEE8021x(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putIEEE8021x:
	  Psiad_PutIEEE8021x(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getIPFilter:
	  Psiad_GetIPFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putIPFilter:
	  Psiad_PutIPFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getFilterAddresses:
	  Psiad_GetIPFilterAddresses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putFilterAddresses:
	  Psiad_PutIPFilterAddresses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postFilterAddresses:
	  Psiad_PostIPFilterAddresses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteFilterAddresses:
	  Psiad_DeleteIPFilterAddresses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getFilterAddress:
	  Psiad_GetIPFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);  
	break;
	case emsg_putFilterAddress:
	  Psiad_PutIPFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo); 
	break;
	case emsg_deleteFilterAddress:
	  Psiad_DeleteIPFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmp:
	  Psiad_GetSNMP(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmp:
	  Psiad_PutSNMP(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpv2c:
	  Psiad_GetSNMPV2c(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpv2c:
	  Psiad_PutSNMPV2c(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpv2cTrapReceivers:
	  Psiad_GetSNMPTrapReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_putSnmpv2cTrapReceivers:
	  Psiad_PutSNMPTrapReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postSnmpv2cTrapReceivers:
	   Psiad_PostSNMPTrapReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSnmpv2cTrapReceivers:
	  Psiad_DeleteSNMPTrapReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpv2cTrapReceiver:
	  Psiad_GetSNMPTrapReceiver(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpv2cTrapReceiver:
	  Psiad_PutSNMPTrapReceiver(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSnmpv2cTrapReceiver:
	  Psiad_DeleteSNMPTrapReceiver(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpAdvanced:
	   Psiad_GetSNMPAdvanced(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvanced:
	   Psiad_PutSNMPAdvanced(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpAdvancedUsers:
	  Psiad_GetSNMPAdvancedUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvancedUsers:
	  Psiad_PutSNMPAdvancedUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postSnmpAdvancedUsers:
	  Psiad_PostSNMPAdvancedUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSnmpAdvancedUsers:
	  Psiad_DeleteSNMPAdvancedUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpAdvancedUser:
	  Psiad_GetSNMPAdvancedUser(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvancedUser:
	  Psiad_PutSNMPAdvancedUser(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSnmpAdvancedUser:
	  Psiad_DeleteSNMPAdvancedUser(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpAdvancedNotificationFilters:
	  Psiad_GetSNMPAdvancedNotificationFilters(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvancedNotificationFilters:
	  Psiad_PutSNMPAdvancedNotificationFilters(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postSnmpAdvancedNotificationFilters:
	  Psiad_PostSNMPAdvancedNotificationFilters(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSnmpAdvancedNotificationFilters:
	  Psiad_DeleteSNMPAdvancedNotificationFilters(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpAdvancedNotificationFilter:
	  Psiad_GetSNMPAdvancedNotificationFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvancedNotificationFilter:
	  Psiad_PutSNMPAdvancedNotificationFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSnmpAdvancedNotificationFilter:
	  Psiad_DeleteSNMPAdvancedNotificationFilter(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSnmpAdvancedNotificationReceivers:
	  Psiad_GetSNMPAdvancedNotificationReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvancedNotificationReceivers:
	  Psiad_PutSNMPAdvancedNotificationReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_postSnmpAdvancedNotificationReceivers:
	Psiad_PostSNMPAdvancedNotificationReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_deleteSnmpAdvancedNotificationReceivers:
	Psiad_DeleteSNMPAdvancedNotificationReceivers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getSnmpAdvancedNotificationReceiver:
	Psiad_GetSNMPAdvancedNotificationReceiver(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_putSnmpAdvancedNotificationReceiver:
	Psiad_PutSNMPAdvancedNotificationReceiver(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_deleteSnmpAdvancedNotificationReceiver:
	Psiad_DeleteSNMPAdvancedNotificationReceiver(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getSnmpAdvancedv3:
	  Psiad_GetSNMPAdvancedV3(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSnmpAdvancedv3:
	  Psiad_PutSNMPAdvancedV3(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getQos:
	  Psiad_GetQoS(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putQos:
	  Psiad_PutQoS(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getCoses:
	  Psiad_GetCoSes(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putCoses:
	  Psiad_PutCoSes(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postCoses:
	  Psiad_PostCoSes(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteCoses:
	  Psiad_DeleteCoSes(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getCos:
	  Psiad_GetCoS(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putCos:
	  Psiad_PutCoS(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteCos:
	  Psiad_DeleteCoS(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getDscps:
	  Psiad_GetDscps(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putDscps:
	  Psiad_PutDscps(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postDscps:
	  Psiad_PostDscps(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteDscps:
	  Psiad_DeleteDscps(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getDscp:
	  Psiad_GetDscp(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putDscp:
	  Psiad_PutDscp(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteDscp:
	  Psiad_DeleteDscp(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getDiscovery:
	  Psiad_GetDiscovery(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putDiscovery:
	  Psiad_PutDiscovery(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSyslog:
	  Psiad_GetSyslog(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSyslog:
	  Psiad_PutSyslog(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSyslogServers:
	  Psiad_GetSyslogServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSyslogServers:
	  Psiad_PutSyslogServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postSyslogServers:
	  Psiad_PostSyslogServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSyslogServers:
	  Psiad_DeleteSyslogServers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSyslogServer:
	   Psiad_GetSyslogServer(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSyslogServer:
	  Psiad_PutSyslogServer(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteSyslogServer:
	  Psiad_DeleteSyslogServer(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getIOPorts:
	  Psiad_GetIOPorts(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getIOPortStatuses:
	  Psiad_GetIOPortStatuses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getInputPorts:
	  Psiad_GetInputPorts(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getInputPort:
	   Psiad_GetInputPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putInputPort:
	  Psiad_PutInputPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getIOPortStatus:
	  Psiad_GetIOPortStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getOutputPorts:
	  Psiad_GetOutputPorts(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getOutputPort:
	  Psiad_GetOutputPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putOutputPort:
	  Psiad_PutOutputPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setIOportTrigger:
	  Psiad_SetIOportTrigger(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getAudioChannels:
	  Psiad_GetAudioChannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getAudioChannel:
	  Psiad_GetAudioChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putAudioChannel:
	   Psiad_PutAudioChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoOverlayImages:
	  Psiad_GetVideoOverlayImages(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postVideoOverlayImages:
	  Psiad_PostVideoOverlayImages(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoOverlayImages:
	 Psiad_DeleteVideoOverlayImages(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo); 
	break;
	case emsg_getVideoOverlayImage:
	  Psiad_GetVideoOverlayImage(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoOverlayImage:
	  Psiad_PutVideoOverlayImage(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoOverlayImage:
	  Psiad_DeleteVideoOverlayImage(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoInput:
	  Psiad_GetVideoInput(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoInputChannels:
	Psiad_GetVideoInputChannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getVideoInputChannel:
	Psiad_GetVideoInputChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_putVideoInputChannel:
	Psiad_PutVideoInputChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_setVideoFocus:
	Psiad_SetVideoFocus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_setVideoIRIS:
	  Psiad_SetVideoIRIS(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoLens:
	  Psiad_GetVideoLens(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoOverlay:
	  Psiad_GetVideoOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoOverlay:
	  Psiad_PutVideoOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoOverlay:
	  Psiad_DeleteVideoOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoTextOverlays:
	  Psiad_GetVideoTextOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoTextOverlays:
	  Psiad_PutVideoTextOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postVideoTextOverlays:
	   Psiad_PostVideoTextOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoTextOverlays:
	  Psiad_DeleteVideoTextOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoTextOverlay:
	  Psiad_GetVideoTextOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoTextOverlay:
	  Psiad_PutVideoTextOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoTextOverlay:
	  Psiad_DeleteVideoTextOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoImageOverlays:
	  Psiad_GetVideoImageOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoImageOverlays:
	  Psiad_PutVideoImageOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postVideoImageOverlays:
	  Psiad_PostVideoImageOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoImageOverlays:
	  Psiad_DeleteVideoImageOverlays(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoImageOverlay:
	  Psiad_GetVideoImageOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoImageOverlay:
	Psiad_PutVideoImageOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_deleteVideoImageOverlay:
	Psiad_DeleteVideoImageOverlay(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getVideoPrivacyMask:
	Psiad_GetPrivacyMask(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_putVideoPrivacyMask:
	  Psiad_PutPrivacyMask(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoPrivacyMaskRegions:
	   Psiad_GetPrivacyMaskRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoPrivacyMaskRegions:
	  Psiad_PutPrivacyMaskRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postVideoPrivacyMaskRegions:
	  Psiad_PostPrivacyMaskRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoPrivacyMaskRegions:
	  Psiad_DeletePrivacyMaskRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getVideoPrivacyMaskRegion:
	  Psiad_GetPrivacyMaskRegion(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putVideoPrivacyMaskRegion:
	  Psiad_PutPrivacyMaskRegion(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteVideoPrivacyMaskRegion:
	  Psiad_DeletePrivacyMaskRegion(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSerialPorts:
	  Psiad_GetSerialPorts(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSerialPort:
	  Psiad_GetSerialPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSerialPort:
	  Psiad_PutSerialPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_sendCommandToSerailPort:
	  Psiad_SendCommandToSerailPort(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getDiagnosticsCommands:
	  Psiad_GetDiagnosticsCommands(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postDiagnosticsCommands:
	  Psiad_PostDiagnosticsCommands(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteDiagnosticsCommands:
	  Psiad_DeleteDiagnosticsCommands(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getDiagnosticsCommand:
	  Psiad_GetDiagnosticsCommand(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteDiagnosticsCommand:
	  Psiad_DeleteDiagnosticsCommand(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getSRTPMasterKey:
	  Psiad_GetSRTPMasterKey(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putSRTPMasterKey:
	  Psiad_PutSRTPMasterKey(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getDeviceCert:
	  Psiad_GetDeviceCert(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putDeviceCert:
	  Psiad_PutDeviceCert(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getUsers:
	  Psiad_GetUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putUsers:
	  Psiad_PutUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postUsers:
	  Psiad_PostUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteUsers:
	  Psiad_DeleteUsers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getUser:
	  Psiad_GetUser(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putUser:
	  Psiad_PutUser(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteUser:
	  Psiad_DeleteUser(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getAAACert:
	  Psiad_GetAAACert(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putAAACert:
	  Psiad_PutAAACert(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getAdminAccesses:
	  Psiad_GetAdminAccesses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putAdminAccesses:
	  Psiad_PutAdminAccesses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postAdminAccesses:
	  Psiad_PostAdminAccesses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteAdminAccesses:
	  Psiad_DeleteAdminAccesses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getAdminAccess:
	  Psiad_GetAdminAccess(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putAdminAccess:
	  Psiad_PutAdminAccess(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteAdminAccess:
	  Psiad_DeleteAdminAccess(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStreamingStatus:
	  Psiad_GetStreamingStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStreamingChannels:
	   Psiad_GetStreamingChannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putStreamingChannels:
	  Psiad_PutStreamingChannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postStreamingChannels:
	  Psiad_PostStreamingChannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteStreamingChannels:
	   Psiad_DeleteStreamingChannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStreamingChannel:
	  Psiad_GetStreamingChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putStreamingChannel:
	   Psiad_PutStreamingChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteStreamingChannel:
	  Psiad_DeleteStreamingChannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStreamingSessionStatus:
	  Psiad_GetStreamingSessionStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getStreaming:
	   printf("[psiad](%s:%d)Geting streaming function is not implemented!\n",__FILE__,__LINE__);
	break;
	case emsg_postStreaming:
	   printf("[psiad](%s:%d)Posting streaming function is not implemented!\n",__FILE__,__LINE__);
	break;
	case emsg_getSnapshot:
	  printf("[psiad](%s:%d)Geting snapshot function is not implemented!\n",__FILE__,__LINE__);
	break;
	case emsg_postSnapshot:
	  printf("[psiad](%s:%d)Posting snapshot function is not implemented!\n",__FILE__,__LINE__);
	break;
	case emsg_requstKeyFrame:
	  printf("[psiad](%s:%d)Requesting keyframe function is not implemented!\n",__FILE__,__LINE__);
	break;
	case emsg_getPTZhannels:
	  Psiad_GetPTZhannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZChannels:
	   Psiad_PutPTZhannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postPTZChannels:
	  Psiad_PostPTZhannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deletePTZChannels:
	  Psiad_DeletePTZhannels(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZhannel:
	   Psiad_GetPTZhannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZChannel:
	   Psiad_PutPTZhannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deletePTZChannel:
	   Psiad_DeletePTZhannel(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setPTZHomePosition:
	  Psiad_SetPTZHomePosition(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setPTZContinuous:
	  Psiad_SetPTZContinuous(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setPTZMomentary:
	  Psiad_SetPTZMomentary(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setPTZRelative:
	  Psiad_SetPTZRelative(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setPTZAbsolute:
	  Psiad_SetPTZAbsolute(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_setPTZDigital:
	  Psiad_SetPTZDigital(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZStatus:
	  Psiad_GetPTZStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPresets:
	  Psiad_GetPTZPresets(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZPresets:
	  Psiad_PutPTZPresets(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postPTZPresets:
	  Psiad_PostPTZPresets(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deletePTZPresets:
	  Psiad_DeletePTZPresets(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPreset:
	  Psiad_GetPTZPreset(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZPreset:
	  Psiad_PutPTZPreset(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deletePTZPreset:
	   Psiad_DeletePTZPreset(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_gotoPresetPosition:
	   Psiad_GotoPresetPosition(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPatrols:
	  Psiad_GetPTZPatrols(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZPatrols:
	  Psiad_PutPTZPatrols(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postPTZPatrols:
	  Psiad_PostPTZPatrols(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deletePTZPatrols:
	  Psiad_DeletePTZPatrols(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPatrolStatuses:
	  Psiad_GetPTZPatrolStatuses(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPatrol:
	  Psiad_GetPTZPatrol(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZPatrol:
	  Psiad_PutPTZPatrol(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deletePTZPatrol:
	  Psiad_DeletePTZPatrol(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_startPTZPatrol:
	   Psiad_StartPTZPatrol(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_stopPTZPatrol:
	   Psiad_StopPTZPatrol(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_pausePTZPatrol:
	  Psiad_PausePTZPatrol(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPatrolStatus:
	  Psiad_GetPTZPatrolStatus(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getPTZPatrolSchedule:
	  Psiad_GetPTZPatrolSchedule(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putPTZPatrolSchedule:
	  Psiad_PutPTZPatrolSchedule(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getMotionDetctions:
	  Psiad_GetMotionDetctions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getMotionDetction:
	  Psiad_GetMotionDetction(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putMotionDetction:
	  Psiad_PutMotionDetction(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getMotionDetectionRegions:
	  Psiad_GetMotionDetectionRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putMotionDetectionRegions:
	  Psiad_PutMotionDetectionRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postMotionDetectionRegions:
	  Psiad_PostMotionDetectionRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteMotionDetectionRegions:
	  Psiad_DeleteMotionDetectionRegions(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getMotionDetectionRegion:
	  Psiad_GetMotionDetectionRegion(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putMotionDetectionRegion:
	  Psiad_PutMotionDetectionRegion(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteMotionDetectionRegion:
	  Psiad_DeleteMotionDetectionRegion(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventNotification:
	  Psiad_GetEventNotification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventNotification:
	  Psiad_PutEventNotification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventTriggers:
	  Psiad_GetEventTriggers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventTriggers:
	  Psiad_PutEventTriggers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postEventTriggers:
	  Psiad_PostEventTriggers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteEventTriggers:
	  Psiad_DeleteEventTriggers(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventTrigger:
	  Psiad_GetEventTrigger(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventTrigger:
	  Psiad_PutEventTrigger(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteEventTrigger:
	  Psiad_DeleteEventTrigger(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventTriggerNontifications:
	  Psiad_GetEventTriggerNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventTriggerNontifications:
	  Psiad_PutEventTriggerNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postEventTriggerNontifications:
	  Psiad_PostEventTriggerNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteEventTriggerNontifications:
	  Psiad_DeleteEventTriggerNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventTriggerNontification:
	  Psiad_GetEventTriggerNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventTriggerNontification:
	  Psiad_PutEventTriggerNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteEventTriggerNontification:
	  Psiad_DeleteEventTriggerNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventSchedule:
	  Psiad_GetEventSchedule(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventSchedule:
	  Psiad_PutEventSchedule(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventNotificationMethods:
	  Psiad_GetEventNotificationMethods(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putEventNotificationMethods:
	  Psiad_PutEventNotificationMethods(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getMailingNontifications:
	  Psiad_GetMailingNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putMailingNontifications:
	  Psiad_PutMailingNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postMailingNontifications:
	  Psiad_PostMailingNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteMailingNontifications:
	   Psiad_DeleteMailingNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getMailingNontification:
	  Psiad_GetMailingNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putMailingNontification:
	  Psiad_PutMailingNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteMailingNontification:
	   Psiad_DeleteMailingNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getFTPNontifications:
	  Psiad_GetFTPNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putFTPNontifications:
	  Psiad_PutFTPNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postFTPNontifications:
	  Psiad_PostFTPNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteFTPNontifications:
	  Psiad_DeleteFTPNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getFTPNontification:
	  Psiad_GetFTPNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putFTPNontification:
	  Psiad_PutFTPNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteFTPNontification:
	  Psiad_DeleteFTPNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getHTTPHostNontifications:
	  Psiad_GetHTTPHostNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putHTTPHostNontifications:
	  Psiad_PutHTTPHostNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_postHTTPHostNontifications:
	  Psiad_PostHTTPHostNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteHTTPHostNontifications:
	   Psiad_DeleteHTTHostPNontifications(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getHTTPHostNontification:
	  Psiad_GetHTTPHostNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_putHTTPHostNontification:
	  Psiad_PutHTTPHostNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_deleteHTTPHostNontification:
	  Psiad_DeleteHTTHostPNontification(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	break;
	case emsg_getEventNotificationAlert:
	  Psiad_GetEventNotificationAlert(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getRootServiceIndex:
	 Psiad_GetRootServiceIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	 break;
	case emsg_getRootServiceIndexr:
	  Psiad_GetRootServiceIndexr(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_getRootServiceDescription:
	 Psiad_GetRootServiceDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break; 
	case emsg_getSystemIndex:
	 Psiad_GetSystemIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSystemDescription:
	 Psiad_GetSystemDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSystemNetworkIndex:
	 Psiad_GetSystemNetworkIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
 
	  break;
	case emsg_getSystemNetworkDescription:
	Psiad_GetSystemNetworkDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSystemIOIndex:
	 Psiad_GetSystemIOIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
 
	  break;
	case emsg_getSystemIODescription:
	Psiad_GetSystemIODescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
 
	  break;
	case emsg_getSystemAudioIndex:
	Psiad_GetSystemAudioIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSystemAudioDescription:
	 Psiad_GetSystemAudioDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSystemVideoIndex:
	 Psiad_GetSystemVideoIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSystemVideoDescription:
	 Psiad_GetSystemVideoDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
 
	  break;
	case emsg_getStreamingIndex:
	Psiad_GetStreamingIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getStreamingDescription:
	 Psiad_GetStreamingDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getPTZIndex:
	 Psiad_GetPTZIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getPTZDescription:
	 Psiad_GetPTZDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getCustomMotionDetectionIndex:
	 Psiad_GetCustomMotionDetectionIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getCustomMotionDetectionDescription:
	 Psiad_GetCustomMotionDetectionDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSecurityIndex:
	 Psiad_GetSecurityIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSecurityDescription:
	 Psiad_GetSecurityDescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);

	  break;
	case emsg_getSecurityAAAIndex:
	 Psiad_GetSecurityAAAIndex(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
 
	  break;
	case emsg_getSecurityAAADescription:
	Psiad_GetSecurityAAADescription(ptPsiadInfo,ptInputMsgInfo,ptOutputMsgInfo);
	  break;
	case emsg_unknown:
	 printf("[psiad](%s:%d)The interface (%s) is not implemented!\n",__FILE__,__LINE__,ptInputMsgInfo->szURI);
	 break;
	default:	  
	 break;
    }
	
    return S_OK;
}

SCODE Psiad_Reboot(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   
    TPsiaResponseStatus  tResponseStatus;
    if(ptPsiadInfo->pfnRebootFunc==NULL)
    {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnRebootFunc==NULL\n",__LINE__);
      goto exit;
    }
    memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
    //memset(ptOutputMsgInfo->szXMLOutMsg,0x0,MAX_BUFFER);
    memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
    ptOutputMsgInfo->eProcResult=eomsgprocres_success;
    
  
    
    if(ptPsiadInfo-> pfnRebootFunc(ptPsiadInfo->hAppObject,&tResponseStatus)==S_FAIL)
    {
	ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
    }
    ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;

  
  snprintf( ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
  
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
						      
exit:
    
    return S_OK;
}

SCODE Psiad_UpdateFirmware(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
    TPsiaResponseStatus  tResponseStatus;
    if(ptPsiadInfo->pfnUpdateFirmwareFunc==NULL)
    {
      printf("[psiad_loacl](%d)ptPsiadInfo-> pfnUpdateFirmwareFunc==NULL\n",__LINE__);
      goto exit;
    }
    memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   // memset(ptOutputMsgInfo->szXMLOutMsg,0x0,MAX_BUFFER);
    memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
    ptOutputMsgInfo->eProcResult=eomsgprocres_success;
    if(ptPsiadInfo-> pfnUpdateFirmwareFunc(ptPsiadInfo->hAppObject,&tResponseStatus)==S_FAIL)
    {
	ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
    }
    

    
    ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
    

  
  snprintf( ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
  
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
    
exit:
    
    return S_OK;

}
SCODE Psiad_GetConfigurationData(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{

  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetConfigurationDataFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetConfigurationDataFunc==NULL\n",__LINE__);
      goto exit;
  }
  if(ptPsiadInfo->pfnGetConfigurationDataFunc(ptPsiadInfo->hAppObject,ptOutputMsgInfo->pbyReturnResult,&ptOutputMsgInfo->dwReturnResultLen)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
#if 0
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     //"Content-Type: application/xml; charset=utf-8\r\n"
							     "Accept: */*\r\n"
							     "Content-Type: multipart/form-data\r\n"
							     "Accept-Encoding: gzip,  deflate\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
#endif    
exit:

  return  S_OK;

}



SCODE Psiad_PutConfigurationData(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResponseStatus  tResponseStatus;
  if(ptPsiadInfo->pfnPutConfigurationDataFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutConfigurationDataFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));

  memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnPutConfigurationDataFunc(ptPsiadInfo->hAppObject, (CHAR *)ptInputMsgInfo->pbyInboundData,&tResponseStatus)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  

  
  snprintf( ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
  
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
						      
exit:
    
  //Maybe reboot the system after applying the configuration data. (p.19 of PSIA-IPMD-v1 1r1.pdf)
  
  return  S_OK;

}
SCODE FactoryReset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  //The "full" mode  resets all device parameters and settings to their factory values.
  //The "basic" mode resets all device parameters and settings except the values in "/PSIA/System/Network and /PSIA/Securirty."
  //The default mode is "full".
  //The device may be rebooted atfer it is reset.
  CHAR *szMode=NULL;
  
  
  TPsiaResponseStatus  tResponseStatus;
  #if 0
  if(strlen(ptInputMsgInfo->szQueryString)!=0)
  {
    printf("[psiad_loacl](%d)ptInputMsgInfo->szQueryString is \"%s\"\n",__LINE__,ptInputMsgInfo->szQueryString);
  }
  #endif
  if(ptPsiadInfo->pfnFactoryResetFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnactoryResetFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  //memset(ptOutputMsgInfo->szXMLOutMsg,0x0,MAX_BUFFER);
  memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
  szMode=strtok(ptInputMsgInfo->szQueryString,"mode=");
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
 
  if(ptPsiadInfo->pfnFactoryResetFunc(ptPsiadInfo->hAppObject,szMode,&tResponseStatus)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
  }
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  

  
  snprintf( ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
  
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
exit: 
  return  S_OK;
}

SCODE Psiad_GetDeviceInfo(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
//typedef SCODE (* FGetDeviceInfo)(HANDLE dwIntance, TPsiaDeviceInfo *ptDeviceInfo);
  TPsiaDeviceInfo tDeviceInfo;
  if(ptPsiadInfo->pfnGetDeviceInfoFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetDeviceInfoFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  
  memset(&tDeviceInfo,0x0,sizeof(TPsiaDeviceInfo));
 // memset(ptOutputMsgInfo->szXMLOutMsg,0x0,MAX_BUFFER);
  memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  //printf("[psiad_loacl](%d)Enter Psiad_GetDeviceInfo()\n",__LINE__);

  if(ptPsiadInfo->pfnGetDeviceInfoFunc(ptPsiadInfo->hAppObject, &tDeviceInfo)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
  }
  
 if(ptOutputMsgInfo->pbyReturnResult!=NULL)
 {
  snprintf( ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						    "<DeviceInfo version=\"1.0\" xmlns=\"%s\">"
						    "<deviceName>%s</deviceName>"
						    "<deviceID>%s</deviceID>"
						    "<deviceDescription>%s</deviceDescription>"
						    "<deviceLocation>%s</deviceLocation>"
						    "<systemContact>%s</systemContact>"
						    "<model>%s</model>"
						    "<serialNumber>%s</serialNumber>"
						    "<macAddress>%s</macAddress>"
						    "<firmwareVersion>%s</firmwareVersion>"
						    "<firmwareReleasedDate>%s</firmwareReleasedDate>"
						    "<logicVersion>%s</logicVersion>"
						    "<logicReleasedDate>%s</logicReleasedDate>"
						    "<bootVersion>%s</bootVersion>"
						    "<bootReleasedDate>%s</bootReleasedDate>"
						    "<rescueVersion>%s</rescueVersion>"
						    "<rescueReleasedDate>%s</rescueReleasedDate>"
						    "<hardwareVersion>%s</hardwareVersion>"
						    "<systemObjectID>%s</systemObjectID>"
						    "</DeviceInfo>"
						   ,MSG_XML_DECLARATION
						   ,PSIA_NAMESPACE_STR
						   ,tDeviceInfo.szDeviceName
						   ,tDeviceInfo.szDeviceUUID
						   ,tDeviceInfo.szDeviceDescription
						   ,tDeviceInfo.szDeviceLocation
						   ,tDeviceInfo.szSystemContact
						   ,tDeviceInfo.szModel
						   ,tDeviceInfo.szSerialNumber
						   ,tDeviceInfo.szMacAddress
						   ,tDeviceInfo.szFirmwareVersion
						   ,tDeviceInfo.szFirmwareReleasedDate
						   ,tDeviceInfo.szLogicVersion
						   ,tDeviceInfo.szLogicReleasedDate
						   ,tDeviceInfo.szBootVersion
						   ,tDeviceInfo.szBootReleasedDate
						   ,tDeviceInfo.szRescueVersion
						   ,tDeviceInfo.szRescueReleasedDate
						   ,tDeviceInfo.szHardwareVersion
						   ,tDeviceInfo.szSystemObjectID);
						 
 ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						   
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);


 }
 else
 {
    printf("[psiad_loacl](%d)ptOutputMsgInfo->pbyReturnResult==NULL\n",__LINE__);
 
 }
exit:
  return  S_OK;
}


static void DeviceInfoSetDeviceName(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  printf("[psiad_loacl](%d)str=%s strlen(str)=%d\n",__LINE__,str,strlen(str));
  strncpy(ptDeivceInfo->szDeviceName,str,strlen(str));
  return;
}
static void DeviceInfoSetDeviceID(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szDeviceUUID,str,strlen(str));
  return;
}
static void DeviceInfoSetDeviceDescription(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szDeviceDescription,str,strlen(str));
  return;
}
static void DeviceInfoSetDeviceLocation(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szDeviceLocation,str,strlen(str));
  return;
}
static void DeviceInfoSetSystemContact(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szSystemContact,str,strlen(str));
  return;
}
static void DeviceInfoSetModel(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szModel,str,strlen(str));
  return;
}
static void DeviceInfoSetSerialNumber(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szSerialNumber,str,strlen(str));
  return;
}
static void DeviceInfoSetMacAddress(void *userData, const char *s, int len)
{
   TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szMacAddress,str,strlen(str));
  return;
}
static void DeviceInfoSetFirmwareVersion(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szFirmwareVersion,str,strlen(str));
  return;
}
static void DeviceInfoSetFirmwareReleasedDate(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szFirmwareReleasedDate,str,strlen(str));
  return;
}
static void DeviceInfoSetLogicVersion(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szLogicVersion,str,strlen(str));
  return;
}
static void DeviceInfoSetLogicReleasedDate(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szLogicReleasedDate,str,strlen(str));
  return;
}
static void DeviceInfoSetBootVersion(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szBootVersion,str,strlen(str));
  return;
}
static void DeviceInfoSetBootReleasedDate(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szBootReleasedDate,str,strlen(str));
  return;
}
static void DeviceInfoSetRescueVersion(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szRescueVersion,str,strlen(str));
  return;
}
static void DeviceInfoSetRescueReleasedDate(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szRescueReleasedDate,str,strlen(str));
  return;
}
static void DeviceInfoSetHardwareVersion(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szHardwareVersion,str,strlen(str));
  return;
}
static void DeviceInfoSetSystemObjectID(void *userData, const char *s, int len)
{
  TPsiaDeviceInfo *ptDeivceInfo = (TPsiaDeviceInfo *)userData;
  char str[len+1];
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDeivceInfo->szSystemObjectID,str,strlen(str));
  return;
}

static SCODE parse_deviceInfo_msg(TPsiaDeviceInfo *ptDeivceInfo,CHAR *szMsg)
{


	int sRet;
	HANDLE hXmlWrapObj;	
	TXmlWrapperTreeMap ptParsingMap[] = 
	{
		{"ns:DeviceInfo/deviceName",NULL,&DeviceInfoSetDeviceName,NULL},
		{"ns:DeviceInfo/deviceID", NULL, &DeviceInfoSetDeviceID, NULL},
		{"ns:DeviceInfo/deviceDescription", NULL, &DeviceInfoSetDeviceDescription, NULL},
		{"ns:DeviceInfo/deviceLocation", NULL,&DeviceInfoSetDeviceLocation, NULL},
		{"ns:DeviceInfo/systemContact",NULL,&DeviceInfoSetSystemContact,NULL},
		{"ns:DeviceInfo/model",NULL,&DeviceInfoSetModel,NULL},
		{"ns:DeviceInfo/serialNumber", NULL, &DeviceInfoSetSerialNumber, NULL},
		{"ns:DeviceInfo/macAddress",NULL,&DeviceInfoSetMacAddress,NULL},
		{"ns:DeviceInfo/firmwareVersion",NULL,&DeviceInfoSetFirmwareVersion,NULL},
		{"ns:DeviceInfo/firmwareReleasedDate",NULL,&DeviceInfoSetFirmwareReleasedDate,NULL},
		{"ns:DeviceInfo/logicVersion",NULL,&DeviceInfoSetLogicVersion,NULL},
		{"ns:DeviceInfo/logicReleasedDate",NULL,&DeviceInfoSetLogicReleasedDate,NULL},
		{"ns:DeviceInfo/bootVersion",NULL,&DeviceInfoSetBootVersion,NULL},
		{"ns:DeviceInfo/bootReleasedDate",NULL,&DeviceInfoSetBootReleasedDate,NULL},
		{"ns:DeviceInfo/rescueVersion",NULL,&DeviceInfoSetRescueVersion,NULL},
		{"ns:DeviceInfo/rescueReleasedDate",NULL,&DeviceInfoSetRescueReleasedDate,NULL},
		{"ns:DeviceInfo/hardwareVersion",NULL,&DeviceInfoSetHardwareVersion,NULL},
		{"ns:DeviceInfo/systemObjectID",NULL,&DeviceInfoSetSystemObjectID,NULL},
		{NULL, NULL, NULL}
	};
	
	TXmlWrapperNamespaceMap ptNSMap[] =
	{
		      {"ns", PSIA_NAMESPACE_STR},
		      {NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;
	//printf("[psiad_local](%d)szMsg:\n",__LINE__);
	//printf("%s\n",szMsg);
	//printf("[psiad_local](%d)End\n",__LINE__);
	if (ptDeivceInfo==NULL)
	{
	    printf("%s %d : ptDeivceInfo is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj, ptDeivceInfo)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;

}

SCODE Psiad_PutDeviceInfo(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   TPsiaResponseStatus  tResponseStatus;
   TPsiaDeviceInfo tDeviceInfo;
  if(ptPsiadInfo->pfnPutDeviceInfoFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutDeviceInfoFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tDeviceInfo,0x0,sizeof(TPsiaDeviceInfo));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_deviceInfo_msg(&tDeviceInfo,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutDeviceInfoFunc(ptPsiadInfo->hAppObject, &tDeviceInfo,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;
}
SCODE Psiad_SupportReport(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   
 
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnSupportReportFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnSupportReportFunc==NULL\n",__LINE__);
      goto exit;
  }
   //Get the reporting data from the system.
   if(ptPsiadInfo->pfnSupportReportFunc(ptPsiadInfo->hAppObject,ptOutputMsgInfo->pbyReturnResult,&ptOutputMsgInfo->dwReturnResultLen)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/x-tar; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  
exit:  
  return  S_OK;
}
SCODE Psiad_GetDeviceStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
 
  TPsiaDeviceStatus  tDeviceStatus;
  int iRet=0;
  int offset=0;
  int i=0;
  
  TPsiaTemperature *ptTemperature=NULL;
  TPsiaFan *ptFan=NULL;
  TPsiaPressure *ptPressure=NULL;
  TPsiaTamper *ptTamper=NULL;
  TPsiaCPU *ptCPU=NULL;
  TPsiaMemory *ptMemory=NULL;
   if(ptPsiadInfo->pfnGetDeviceStatusFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetDeviceStatusFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  memset(&tDeviceStatus,0x0,sizeof(TPsiaDeviceStatus));
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  
   
  
  tDeviceStatus.tTemperatureList.ptTemperature=(TPsiaTemperature *)malloc(sizeof(TPsiaTemperature)*MAX_LIST_NUM);
  tDeviceStatus.tTemperatureList.dwTemperatureListNum=0;
  memset(tDeviceStatus.tTemperatureList.ptTemperature,0x0,sizeof(TPsiaTemperature)*MAX_LIST_NUM);
  
  tDeviceStatus.tFanList.ptFan=(TPsiaFan *)malloc(sizeof(TPsiaFan)*MAX_LIST_NUM);
  tDeviceStatus.tFanList.dwFanListNum=0;
  memset(tDeviceStatus.tFanList.ptFan,0x0,sizeof(TPsiaFan)*MAX_LIST_NUM);
  
  tDeviceStatus.tPressureList.ptPressure=(TPsiaPressure *)malloc(sizeof(TPsiaPressure)*MAX_LIST_NUM);
  tDeviceStatus.tPressureList.dwPressureListNum=0;
  memset(tDeviceStatus.tPressureList.ptPressure,0x0,sizeof(TPsiaPressure)*MAX_LIST_NUM);
  
  tDeviceStatus.tTamperList.ptTamper=(TPsiaTamper *)malloc(sizeof(TPsiaTamper)*MAX_LIST_NUM);
  tDeviceStatus.tTamperList.dwTamperListNum=0;
  memset(tDeviceStatus.tTamperList.ptTamper,0x0,sizeof(TPsiaTamper)*MAX_LIST_NUM);
  
  
  tDeviceStatus.tCPUList.ptCPU=(TPsiaCPU *)malloc(sizeof(TPsiaCPU)*MAX_LIST_NUM);
  tDeviceStatus.tCPUList.dwCPUListNum=0;
  memset(tDeviceStatus.tCPUList.ptCPU,0x0,sizeof(TPsiaCPU)*MAX_LIST_NUM);
  
  tDeviceStatus.tMemoryList.ptMemory=(TPsiaMemory *)malloc(sizeof(TPsiaMemory)*MAX_LIST_NUM);
  tDeviceStatus.tMemoryList.dwMemoryListNum=0;
  memset(tDeviceStatus.tMemoryList.ptMemory,0x0,sizeof(TPsiaMemory)*MAX_LIST_NUM);

  
  if(ptPsiadInfo->pfnGetDeviceStatusFunc(ptPsiadInfo->hAppObject,&tDeviceStatus)==S_FAIL)
  {
    ptOutputMsgInfo->eProcResult=eomsgprocres_error;
    goto exit;
  }
  


  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<DeviceStatus version=\"1.0\" xmlns=\"%s\">"
						    "<currentDeviceTime>%s</currentDeviceTime>"
						    "<deviceUpTime>%d</deviceUpTime>"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    ,tDeviceStatus.szCurrentDeviceTime
						    ,tDeviceStatus.dwDeviceUpTime
						    );
						    
   
						    
					
   ptTemperature=tDeviceStatus.tTemperatureList.ptTemperature;
   ptFan=tDeviceStatus.tFanList.ptFan;
   ptPressure=tDeviceStatus.tPressureList.ptPressure;
   ptTamper=tDeviceStatus.tTamperList.ptTamper;
   ptCPU=tDeviceStatus.tCPUList.ptCPU;
   ptMemory=tDeviceStatus.tMemoryList.ptMemory;
      
   offset += iRet;
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<TemperatureList>");
   offset += iRet;
   
   for(i=0;i<tDeviceStatus.tTemperatureList.dwTemperatureListNum;i++)
   {
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<Temperature>"
							  "<tempSensorDescription>%s</tempSensorDescription>"
							  "<temperature>%f</temperature>"
							  "</Temperature>"
							  ,ptTemperature->szTempSensorDescription
							  ,ptTemperature->fTemperature
							  );
	ptTemperature++;						  
	offset += iRet;
      
   }
						   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</TemperatureList>");
   offset += iRet;
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<FanList>");
   offset += iRet;
  
   for(i=0;i<tDeviceStatus.tFanList.dwFanListNum;i++)
   {
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<Fan>"
							  "<fanDescription>%s</fanDescription>"
							  "<speed>%d</speed>"
							  "</Fan>"
							  ,ptFan->szFanDescription
							  ,ptFan->dwFanSpeed
							  );
	ptFan++;						  
	offset += iRet;
      
   }
						   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</FanList>");
   offset += iRet;
   
   
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<PressureList>");
   offset += iRet;
  
   for(i=0;i<tDeviceStatus.tPressureList.dwPressureListNum;i++)
   {
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<Pressure>"
							  "<pressureSensorDescription>%s</pressureSensorDescription>"
							  "<pressure>%d</pressure>"
							  "</Pressure>"
							  ,ptPressure->szPressureSensorDescription
							  ,ptPressure->dwPressure
							  );
	ptPressure++;						  
	offset += iRet;
      
   }
						   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</PressureList>");
   offset += iRet;
   
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<TamperList>");
   offset += iRet;
  
   for(i=0;i<tDeviceStatus.tTamperList.dwTamperListNum;i++)
   {
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<Tamper>"
							  "<tamperSensorDescription>%s</tamperSensorDescription>"
							  "<tamper>%d</tamper>"
							  "</Tamper>"
							  ,ptTamper->szTamperSensorDescription
							  ,ptTamper->bTamper
							  );
	ptTamper++;						  
	offset += iRet;
      
   }
						   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</TamperList>");
   offset += iRet;
   
   
   
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<CPUList>");
   offset += iRet;
  
   for(i=0;i<tDeviceStatus.tCPUList.dwCPUListNum;i++)
   {
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<CPU>"
							  "<cpuDescription>%s</cpuDescription>"
							  "<cpuUtilization>%d</cpuUtilization>"
							  "</CPU>"
							  ,ptCPU->szCpuDescription
							  ,ptCPU->dwCPUUtilization
							  );
	ptCPU++;						  
	offset += iRet;
      
   }
						   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</CPUList>");
   offset += iRet;
  
  
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<MemoryList>");
   offset += iRet;
  
   for(i=0;i<tDeviceStatus.tMemoryList.dwMemoryListNum;i++)
   {
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<Memory>"
							  "<memoryDescription>%s</memoryDescription>"
							  "<memoryUsage>%f</memoryUsage>"
							  "<memoryAvailable>%f</memoryAvailable>"
							  "</Memory>"
							  ,ptMemory->szMemoryDescription
							  ,ptMemory->fMemoryUsage
							  ,ptMemory->fMemoryAvailable
							  );
	ptMemory++;						  
	offset += iRet;
      
   }
						   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</MemoryList>");
   offset += iRet;
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<openFileHandles>%d</openFileHandles>",tDeviceStatus.dwOpenFileHandles);
   offset += iRet;
   
   
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</DeviceStatus>");


   ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
   
  // printf("[psiad_local](%d)ptOutputMsgInfo->pbyReturnResult:\n",__LINE__);
  // printf("%s\n",ptOutputMsgInfo->pbyReturnResult);
  // printf("[psiad_local](%d)End of ptOutputMsgInfo->pbyReturnResult\n",__LINE__);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);

exit:  

  if(tDeviceStatus.tTemperatureList.ptTemperature!=NULL)
  {
    free(tDeviceStatus.tTemperatureList.ptTemperature);
    tDeviceStatus.tTemperatureList.ptTemperature=NULL;
  }
  if(tDeviceStatus.tFanList.ptFan!=NULL)
  {
    free(tDeviceStatus.tFanList.ptFan);
    tDeviceStatus.tFanList.ptFan=NULL;
  }
  if(tDeviceStatus.tPressureList.ptPressure!=NULL)
  {
    free(tDeviceStatus.tPressureList.ptPressure);
    tDeviceStatus.tPressureList.ptPressure=NULL;
  }
  if(tDeviceStatus.tTamperList.ptTamper!=NULL)
  {
    free(tDeviceStatus.tTamperList.ptTamper);
    tDeviceStatus.tTamperList.ptTamper=NULL;
  }
  if(tDeviceStatus.tCPUList.ptCPU!=NULL)
  {
    free(tDeviceStatus.tCPUList.ptCPU);
    tDeviceStatus.tCPUList.ptCPU=NULL;
  }
  if(tDeviceStatus.tMemoryList.ptMemory!=NULL)
  {
    free(tDeviceStatus.tMemoryList.ptMemory);
    tDeviceStatus.tMemoryList.ptMemory=NULL;
  }
  return  S_OK;
}
SCODE Psiad_GetTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  TPsiaTime tTime;
  
  memset(&tTime,0x0,sizeof(TPsiaTime));
  
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetTimeFunc(ptPsiadInfo->hAppObject,&tTime)==S_FAIL)
  {
    ptOutputMsgInfo->eProcResult=eomsgprocres_error;
    goto exit;
  }
 
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<Time version=\"1.0\" xmlns=\"%s\">"
						    "<timeMode>%s</timeMode>"
						    "<localTime>%s</localTime>"
						    "<timeZone>%s</timeZone>"
						    "</Time>"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    ,tTime.szTimeMode
						    ,tTime.szLocalTime
						    ,tTime.szTimeZone
						    );


  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:
  
  return  S_OK;
}

static void TimeSetTimeMode(void *userData, const char *s, int len)
{
    TPsiaTime  *ptTime = (TPsiaTime *)userData;
    char str[len+1];
    memcpy(str, s, len);
    str[len]='\0';
    strncpy(ptTime->szTimeMode,str,strlen(str));
    return;
}

static void TimeSetLocalTime(void *userData, const char *s, int len)
{
    TPsiaTime  *ptTime = (TPsiaTime *)userData;
    char str[len+1];
    memcpy(str, s, len);
    str[len]='\0';
    strncpy(ptTime->szLocalTime,str,strlen(str));
    return;
}
static void TimeSetTimeZone(void *userData, const char *s, int len)
{
    TPsiaTime  *ptTime = (TPsiaTime *)userData;
    char str[len+1];
    memcpy(str, s, len);
    str[len]='\0';
    strncpy(ptTime->szTimeZone,str,strlen(str));
    return;
}
static SCODE parse_time_msg(TPsiaTime *ptTime,CHAR *szMsg)
{


	int sRet;
	HANDLE hXmlWrapObj;	
	TXmlWrapperTreeMap ptParsingMap[] = 
	{
		{"ns:Time/timeMode",NULL,&TimeSetTimeMode,NULL},
		{"ns:Time/localTime", NULL, &TimeSetLocalTime, NULL},
		{"ns:Time/timeZone", NULL, &TimeSetTimeZone, NULL},
		
		{NULL, NULL, NULL}
	};
	TXmlWrapperNamespaceMap ptNSMap[] =
	{
		      {"ns", PSIA_NAMESPACE_STR},
		      {NULL, NULL}
	};
	
	TXmlWrapperInitOptions tInitOptions;
	
	if (ptTime==NULL)
	{
	    printf("%s %d : ptTime is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj,ptTime)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;

}
SCODE Psiad_PutTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
   TPsiaResponseStatus  tResponseStatus;
   TPsiaTime tTime;
  
  if(ptPsiadInfo->pfnPutTimeFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutTimeFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tTime,0x0,sizeof(TPsiaTime));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_time_msg(&tTime,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutTimeFunc(ptPsiadInfo->hAppObject, &tTime,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);

exit:

  return  S_OK;

}


SCODE Psiad_GetLocalTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  
  if(ptPsiadInfo->pfnGetLocalTimeFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetLocalTimeFunc==NULL\n",__LINE__);
      goto exit;
  }
  if(ptPsiadInfo->pfnGetLocalTimeFunc(ptPsiadInfo->hAppObject,(CHAR *)ptOutputMsgInfo->pbyReturnResult,&ptOutputMsgInfo->dwReturnResultLen)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
  }
     
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: text/plain; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
  
exit: 
  return  S_OK;

}
SCODE Psiad_PutLocalTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResponseStatus  tResponseStatus;
  if(ptPsiadInfo->pfnPutLocalTimeFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutLocalTimeFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));

  memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnPutLocalTimeFunc(ptPsiadInfo->hAppObject, (CHAR *)ptInputMsgInfo->pbyInboundData,&tResponseStatus)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;

  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
 ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
 snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
						      
exit:
    

  return  S_OK;
}
SCODE Psiad_GetTimeZone(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  

  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  
  if(ptPsiadInfo->pfnGetTimeZoneFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetTimeZoneFunc==NULL\n",__LINE__);
      goto exit;
  }
  memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
  if(ptPsiadInfo->pfnGetTimeZoneFunc(ptPsiadInfo->hAppObject, (CHAR *)ptOutputMsgInfo->pbyReturnResult,&ptOutputMsgInfo->dwReturnResultLen)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
  
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: text/plain; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
  
      
exit:
  return  S_OK;
}
SCODE Psiad_PutTimeZone(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResponseStatus  tResponseStatus;
  
  if(ptPsiadInfo->pfnPutTimeZoneFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutTimeZoneFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  //memset(ptOutputMsgInfo->szXMLOutMsg,0x0,MAX_BUFFER);
  memset(ptOutputMsgInfo->szHTTPHeader,0x0,HTTP_HEADER_SIZE);
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnPutTimeZoneFunc(ptPsiadInfo->hAppObject,(CHAR *) ptInputMsgInfo->pbyInboundData,&tResponseStatus)==S_FAIL)
  {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;

  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
 snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,ptOutputMsgInfo->dwReturnResultLen);
						        
exit:
  return  S_OK;

}
SCODE Psiad_GetNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  TPsiaNTPServerList tNTPServerList;
  int  offset=0;
  int  iRet=0;
  int i=0;
  
  TPsiaNTPServer *ptNTPServer=NULL;
 
  if(ptPsiadInfo->pfnGetNTPServersFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetNTPServersFunc==NULL\n",__LINE__);
      goto exit;
  }
  memset(&tNTPServerList,0x0,sizeof(TPsiaNTPServerList));
  
  tNTPServerList.ptNTPServer=(TPsiaNTPServer *)malloc(sizeof(TPsiaNTPServer)*MAX_LIST_NUM);
  memset(tNTPServerList.ptNTPServer,0x0,sizeof(TPsiaNTPServer)*MAX_LIST_NUM);
  tNTPServerList.dwListNum=0;
  
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetNTPServersFunc(ptPsiadInfo->hAppObject,&tNTPServerList)==S_FAIL)
  {
    ptOutputMsgInfo->eProcResult=eomsgprocres_error;
    goto exit;
  }
  ptNTPServer=tNTPServerList.ptNTPServer;
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<NTPServerList version=\"1.0\" xmlns=\"%s\">"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );

  
  for(i=0;i<tNTPServerList.dwListNum;i++)
  {
  
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<NTPServer>"
							  "<id>%s</id>"
							  "<addressingFormatType>%s</addressingFormatType>"
							  "<hostName>%s</hostName>"
							  "<ipAddress>%s</ipAddress>"
							  "<ipv6Address>%s</ipv6Address>"
							  "<portNo>%ul</portNo>"
							  "</NTPServer>"
							  ,ptNTPServer->szID
							  ,ptNTPServer->szAddressingFormatType
							  ,ptNTPServer->szHostName
							  ,ptNTPServer->szIPAddress
							  ,ptNTPServer->szIPV6Address
							  ,ptNTPServer->dwPortNum
							  );
    
    
    ptNTPServer++;
    offset += iRet;
  }
  
  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</NTPServerList>");
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:
  if(tNTPServerList.ptNTPServer!=NULL)
  {
    free(tNTPServerList.ptNTPServer);
    tNTPServerList.ptNTPServer=NULL;
  }
  return  S_OK;

}
SCODE Psiad_PutNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetNTPServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutNTPServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteNTPServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetLogging(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutLogging(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetLoggingMessages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStorageVolumes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStorageVolume(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStorageVolumeStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_FomatStorageVolume(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStorageVolumeFiles(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteStorageVolumeFiles(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStorageVolumeFile(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteStorageVolumeFile(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStorageVolumeData(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetNetworkInterfaces(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaNetworkInterfaceList tNetworkInterfaceList;
  int  offset=0;
  int  iRet=0;
  int i=0;
  int j=0;
  int k=0;
  
  
  TPsiaNetworkInterface *ptNetworkInterface=NULL;
  TPsiaEncryptionKey * ptEncryptionKey=NULL;
  TPsiaIPFilterAddress *ptIPFilterAddress=NULL;
  TPsiaSNMPTrapReceiver *ptSNMPTrapReceiver=NULL;
  TPsiaSNMPUser *ptSNMPUser=NULL;
  TPsiaSNMPNotificationFilter *ptSNMPNotificationFilter=NULL;
  TPsiaSNMPNotificationReceiver *ptNotificationReceiver=NULL;
  
  if(ptPsiadInfo->pfnGetNetworkInterfacesFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetNetworkInterfacesFunc==NULL\n",__LINE__);
      goto exit;
  }
  memset(&tNetworkInterfaceList,0x0,sizeof(TPsiaNetworkInterfaceList));
  
  
  tNetworkInterfaceList.ptNetworkInterface=(TPsiaNetworkInterface *)malloc(sizeof(TPsiaNetworkInterface)*MAX_LIST_NUM);
  memset(tNetworkInterfaceList.ptNetworkInterface,0x0,sizeof(TPsiaNetworkInterface)*MAX_LIST_NUM);
  tNetworkInterfaceList.dwListNum=0;
  
  tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey=(TPsiaEncryptionKey *)malloc(sizeof( TPsiaEncryptionKey)*MAX_LIST_NUM);
  memset(tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey,0x0,sizeof( TPsiaEncryptionKey)*MAX_LIST_NUM);
  tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.dwListNum=0;
  
  tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress=(TPsiaIPFilterAddress *)calloc(MAX_LIST_NUM,sizeof( TPsiaIPFilterAddress));
  tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.dwListNum=0;
  
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver=(TPsiaSNMPTrapReceiver  *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPTrapReceiver));
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.dwListNum=0;	
  
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser=(TPsiaSNMPUser  *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPUser));
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.dwListNum=0;
  
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter=(TPsiaSNMPNotificationFilter *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPNotificationFilter));
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.dwListNum=0;
 
  
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver=(TPsiaSNMPNotificationReceiver *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPNotificationReceiver));
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.dwListNum=0;
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetNetworkInterfacesFunc(ptPsiadInfo->hAppObject,&tNetworkInterfaceList)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
 
  ptNetworkInterface=tNetworkInterfaceList.ptNetworkInterface;
  ptEncryptionKey=tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey;
  ptIPFilterAddress=tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress;
  ptSNMPTrapReceiver=tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver;
  ptSNMPUser=tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser;
  ptSNMPNotificationFilter=tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter;
  ptNotificationReceiver= tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver;
  
  
  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<NetworkInterfaceList  version=\"1.0\"  xmlns=\"%s\">"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
						    
   offset += iRet;
 
   for(i=0;i<tNetworkInterfaceList.dwListNum;i++)
   {
     
 

	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<NetworkInterface version=\"1.0\" >"
							  "<id>%s</id>"
							  "<IPAddress version=\"1.0\" >"
							    "<ipVersion>%s</ipVersion>"
							    "<addressingType>%s</addressingType>"
							    "<ipAddress>%s</ipAddress>"
							    "<subnetMask>%s</subnetMask>"
							    "<ipv6Address>%s</ipv6Address>"
							    "<bitMask>%s</bitMask>"
							    "<DefaultGateway>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</DefaultGateway>"
							    "<PrimaryDNS>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</PrimaryDNS>"
							    "<SecondaryDNS>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</SecondaryDNS>"
							  "</IPAddress>"
							  "<Wireless version=\"1.0\" >"
							    "<enabled>%d</enabled>"
							    "<wirelessNetworkMode>%s</wirelessNetworkMode>"
							    "<channel>%s</channel>"
							    "<ssid>%s</ssid>"
							    "<wmmEnabled>%d</wmmEnabled>"
							    "<WirelessSecurity>"
							      "<securityMode>%s</securityMode>"
								  ,ptNetworkInterface->szID
								  ,ptNetworkInterface->tIPAddress.szIPVersion
								  ,ptNetworkInterface->tIPAddress.szAddressingType
								  ,ptNetworkInterface->tIPAddress.szIPAddress
								  ,ptNetworkInterface->tIPAddress.szSubnetMask
								  ,ptNetworkInterface->tIPAddress.szIPv6Address
								  ,ptNetworkInterface->tIPAddress.szBitMask
								  ,ptNetworkInterface->tIPAddress.tDefaultGateway.szIPAddress
								  ,ptNetworkInterface->tIPAddress.tDefaultGateway.szIPV6Address
								  ,ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPAddress
								  ,ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPV6Address
								  ,ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPAddress
								  ,ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPV6Address
								  ,ptNetworkInterface->tWireless.bEnabled
								  ,ptNetworkInterface->tWireless.szWirelessNetworkMode
								  ,ptNetworkInterface->tWireless.szChannel
								  ,ptNetworkInterface->tWireless.szSSID
								  ,ptNetworkInterface->tWireless.bWmmEnabled
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.szSecurityMode
								  );
		offset += iRet;						  
						     	
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,				     
								 "<WEP>"
								  "<authenticationType>%s</authenticationType>"
								  "<defaultTransmitKeyIndex>%u</defaultTransmitKeyIndex>"
								  "<wepKeyLength>%u</wepKeyLength>"
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.szAuthenticationType
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwDefaultTransmitKeyIndex
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwWepKeyLength
								  );
		offset += iRet;	
		
			
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<EncryptionKeyList>");
		offset += iRet;	
		
		for(j=0;j< tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.dwListNum;j++)
		{
		  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<encryptionKey>"
											   "%s" 
											  "</encryptionKey>"
											  ,ptEncryptionKey->szEncryptionKey
				);
		  
		  
		  
		  ptEncryptionKey++;
		  offset += iRet;	
		
		}
								  
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</EncryptionKeyList>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</WEP>");
		offset += iRet;
							    
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<WPA>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<algorithmType>%s</algorithmType>",tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szAlgorithmType);
		offset += iRet;
		    
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<sharedKey>%s</sharedKey>",tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szSharedKey);
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</WPA>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</WirelessSecurity>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</Wireless>");
		offset += iRet;
		
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IEEE802_1x version=\"1.0\" >");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<enabled>%d</enabled>"
											"<authenticationProtocolType>%s</authenticationProtocolType>"
											"<innerTTLSAuthenticationMethod>%s</innerTTLSAuthenticationMethod>"
											"<innerEAPProtocolType>%s</innerEAPProtocolType>"
											"<validateServerEnabled>%d</validateServerEnabled>"
											"<userName>%s</userName>"
											"<password>%s</password>"
											"<anonymousID>%s</anonymousID>"
											"<autoPACProvisioningEnabled>%d</autoPACProvisioningEnabled>"
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.bEnabled
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.szAuthenticationProtocolType
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.szInnerTTLSAuthenticationMethod
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.szInnerEAPProtocolType	
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.bValidateServerEnabled
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.szUserName
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.szPassword
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.szAnonymousID
											,tNetworkInterfaceList.ptNetworkInterface->tIEEE8021X.bAutoPACProvisioningEnabled
											);
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IEEE802_1x>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IPFilter version=\"1.0\">");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<enabled>%d</enabled>"
											"<permissionType>%s</permissionType>"
											,tNetworkInterfaceList.ptNetworkInterface->tIPFilter.bEnabled
											,tNetworkInterfaceList.ptNetworkInterface->tIPFilter.szPermissionType
											 );
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IPFilterAddressList version=\"1.0\" >");
		offset += iRet;
		
		for(k=0;k<tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.dwListNum;k++)
		{
		   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IPFilterAddress version=\"1.0\" ><id>%s</id>"
											   "<permissionType>%s</permissionType>" 
											   "<addressFilterType>%s</addressFilterType>"
											   "<AddressRange>"
											      "<startIPAddress>%s</startIPAddress>"
											      "<endIPAddress>%s</endIPAddress>"
											      "<startIPv6Address>%s</startIPv6Address>"
											      "<endIPv6Address>%s</endIPv6Address>"
											   "</AddressRange>"
											   "<AddressMask>"
											      "<ipAddress>%s</ipAddress>"
											      "<ipv6Address>%s</ipv6Address>"
											      "<bitMask>%s</bitMask>"
											   "</AddressMask></IPFilterAddress>"
											  ,ptIPFilterAddress->szID
											  ,ptIPFilterAddress->szPermissionType
											  ,ptIPFilterAddress->szAddressFilterType
											  ,ptIPFilterAddress->tAddressRange.szStartIPAddress
											  ,ptIPFilterAddress->tAddressRange.szEndIPAddress
											  ,ptIPFilterAddress->tAddressRange.szStartIPv6Address
											  ,ptIPFilterAddress->tAddressRange.szEndIPv6Address
											  ,ptIPFilterAddress->tAddressMask.szIPAddress
											  ,ptIPFilterAddress->tAddressMask.szIPV6Address
											  ,ptIPFilterAddress->tAddressMask.szBitMask
											  );
		  
		  
		  ptIPFilterAddress++; 
		  offset += iRet;
		}
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IPFilterAddressList>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IPFilter>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMP version=\"1.0\" >");
		offset += iRet;
		
		if(tNetworkInterfaceList.ptNetworkInterface->tSNMP.eSNMPChoice==esnmpv2c)
		{
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPv2c version=\"1.0\" >");
		    offset += iRet;
		    
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<notificationEnabled>"
											    "%d"
											    "</notificationEnabled>"
											    ,tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.bNotificationEnabled);
		    offset += iRet;
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPTrapReceiverList version=\"1.0\" >");
		    offset += iRet;
		    
		    
		    for(k=0;k<tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.dwListNum;i++)
		    {
			
		      iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPTrapReceiver>"
											      "<id>%s</id>"
											      "<ReceiverAddress>"
											      "<addressingFormatType>%s</addressingFormatType>"
											      "<hostName>%s</hostName>"
											      "<ipAddress>%s</ipAddress>"
											      "<ipv6Address>%s</ipv6Address>"
											      "<portNo>%d</portNo>"
											      "</ReceiverAddress>"
											      "<notificationType>%s</notificationType>"
											      "<communityString>%s</communityString>"
											      "</SNMPTrapReceiver>"
											      ,ptSNMPTrapReceiver->szID
											      ,ptSNMPTrapReceiver->tReceiverAddress.szAddressingFormatType
											      ,ptSNMPTrapReceiver->tReceiverAddress.szHostName
											      ,ptSNMPTrapReceiver->tReceiverAddress.szIPAddress
											      ,ptSNMPTrapReceiver->tReceiverAddress.szIPV6Address
											      ,ptSNMPTrapReceiver->tReceiverAddress.dwPortNo
											      ,ptSNMPTrapReceiver->szNotificationType
											      ,ptSNMPTrapReceiver->szCommunityString
											      );
											      
		      ptSNMPTrapReceiver++;
		      offset += iRet;
		      
		    
		    }
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMPTrapReceiverList>");
		    offset += iRet;
		    
		    
		    
		    
		    
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMPv2c>");
		    offset += iRet;
		    
		
		}
		else //SNMP Advanced
		{
		
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPAdvanced>");
		    offset += iRet;
		   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<localEngineID>%s</localEngineID>"
					"<authenticationNotificationEnabled>%d</authenticationNotificationEnabled>"
					"<notificationEnabled>%d</notificationEnabled>"
					 ,tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.szLocalEngineID
					 ,tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.bAuthenticationNotificationEnabled
					 ,tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced. bNotificationEnabled
					 );
		    offset += iRet;
		    
		    for(k=0;k<tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.dwListNum;k++)
		    {
		      iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPUser>"
											      "<id>%s</id>"
											      "<userName>%s</userName>"
											      "<remoteEngineID>%s</remoteEngineID>"
											      "<snmpAuthenticationMethod>%s</snmpAuthenticationMethod>"
											      "<snmpAuthenticationKey>%s</snmpAuthenticationKey>"
											      "<snmpAuthenticationPassword>%s</snmpAuthenticationPassword>"
											      "<snmpPrivacyMethod>%s</snmpPrivacyMethod>"
											      "<snmpPrivacyKey>%s</snmpPrivacyKey>"
											      "<snmpPrivacyPassword>%s</snmpPrivacyPassword>"
											      "</SNMPUser>"
											      ,ptSNMPUser->szID
											      ,ptSNMPUser->szUserName 
											      ,ptSNMPUser->szRemoteEngineID 
											      ,ptSNMPUser->szSnmpAuthenticationMethod 
											      ,ptSNMPUser->szSnmpAuthenticationKey 
											      ,ptSNMPUser->szSnmpAuthenticationPassword 
											      ,ptSNMPUser->szSnmpPrivacyMethod 
											      ,ptSNMPUser->szSnmpPrivacyKey 
											      ,ptSNMPUser->szSnmpPrivacyPassword  
											      );
			ptSNMPUser++;								      
		       offset += iRet;
		    }
		    for(k=0;k<tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.dwListNum;k++)
		    {
		      //iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,""
			//								      ,
			//								      );
		      
		      
		      ptSNMPNotificationFilter++;
		      offset += iRet;
		    }
		    for(k=0;k<tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.dwListNum;k++)
		    {
		       
		      ptNotificationReceiver++;
		       offset += iRet;
		    }
		    
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMPAdvanced>");
		    offset += iRet;
		
		}
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMP>");
		offset += iRet;
		
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</NetworkInterface>");
		offset += iRet;
	
						
		
      ptNetworkInterface++;						  
	
      
   }
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</NetworkInterfaceList>");
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  


if(tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey!=NULL)
{
  free(tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey);
  tNetworkInterfaceList.ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey=NULL;
}


if(tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress!=NULL)
{
  free(tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress);
  tNetworkInterfaceList.ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress=NULL;
}


if(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver!=NULL)
{
    free(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver);
    tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver=NULL;
}  

if(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser!=NULL)
{
  free(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser);
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser=NULL;

}

if(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter!=NULL)
{
  free(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter);
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter=NULL;
}

if(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver!=NULL)
{
  free(tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver);
  tNetworkInterfaceList.ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver=NULL;
}

if(tNetworkInterfaceList.ptNetworkInterface!=NULL)
{
  free(tNetworkInterfaceList.ptNetworkInterface);
  tNetworkInterfaceList.ptNetworkInterface=NULL;

}
  return  S_OK;
}
SCODE Psiad_GetNetworkInterface(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  int  offset=0;
  int  iRet=0;
  int i=0;
  int j=0;
  int k=0;
  

  TPsiaNetworkInterface *ptNetworkInterface=NULL;
  TPsiaEncryptionKey * ptEncryptionKey=NULL;
  TPsiaIPFilterAddress *ptIPFilterAddress=NULL;
  TPsiaSNMPTrapReceiver *ptSNMPTrapReceiver=NULL;
  TPsiaSNMPUser *ptSNMPUser=NULL;
  TPsiaSNMPNotificationFilter *ptSNMPNotificationFilter=NULL;
  TPsiaSNMPNotificationReceiver *ptNotificationReceiver=NULL;
  
  if(ptPsiadInfo->pfnGetNetworkInterfaceFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetNetworkInterfaceFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  ptNetworkInterface=(TPsiaNetworkInterface *)malloc(sizeof(TPsiaNetworkInterface));
  memset(ptNetworkInterface,0x0,sizeof(TPsiaNetworkInterface));
  
  ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey=(TPsiaEncryptionKey *)malloc(sizeof( TPsiaEncryptionKey)*MAX_LIST_NUM);
  memset(ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey,0x0,sizeof( TPsiaEncryptionKey)*MAX_LIST_NUM);
  ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.dwListNum=0;
  
  ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress=(TPsiaIPFilterAddress *)calloc(MAX_LIST_NUM,sizeof( TPsiaIPFilterAddress));
  ptNetworkInterface->tIPFilter.tIPFilterAddressList.dwListNum=0;
  
  ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver=(TPsiaSNMPTrapReceiver  *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPTrapReceiver));
  ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.dwListNum=0;	
  
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser=(TPsiaSNMPUser  *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPUser));
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.dwListNum=0;
  
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter=(TPsiaSNMPNotificationFilter *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPNotificationFilter));
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.dwListNum=0;
 
  
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver=(TPsiaSNMPNotificationReceiver *)calloc(MAX_LIST_NUM,sizeof(TPsiaSNMPNotificationReceiver));
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.dwListNum=0;
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetNetworkInterfaceFunc(ptPsiadInfo->hAppObject,ptNetworkInterface)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
 
  
  ptEncryptionKey=ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey;
  ptIPFilterAddress=ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress;
  ptSNMPTrapReceiver=ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver;
  ptSNMPUser=ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser;
  ptSNMPNotificationFilter=ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter;
  ptNotificationReceiver= ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver;
  
  
  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<NetworkInterface  version=\"1.0\"  xmlns=\"%s\" >"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
						    
   offset += iRet;
 
  
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
							  "<id>%s</id>"
							  "<IPAddress version=\"1.0\" >"
							    "<ipVersion>%s</ipVersion>"
							    "<addressingType>%s</addressingType>"
							    "<ipAddress>%s</ipAddress>"
							    "<subnetMask>%s</subnetMask>"
							    "<ipv6Address>%s</ipv6Address>"
							    "<bitMask>%s</bitMask>"
							    "<DefaultGateway>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</DefaultGateway>"
							    "<PrimaryDNS>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</PrimaryDNS>"
							    "<SecondaryDNS>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</SecondaryDNS>"
							  "</IPAddress>"
							  "<Wireless version=\"1.0\" >"
							    "<enabled>%d</enabled>"
							    "<wirelessNetworkMode>%s</wirelessNetworkMode>"
							    "<channel>%s</channel>"
							    "<ssid>%s</ssid>"
							    "<wmmEnabled>%d</wmmEnabled>"
							    "<WirelessSecurity>"
							      "<securityMode>%s</securityMode>"
								  ,ptNetworkInterface->szID
								  ,ptNetworkInterface->tIPAddress.szIPVersion
								  ,ptNetworkInterface->tIPAddress.szAddressingType
								  ,ptNetworkInterface->tIPAddress.szIPAddress
								  ,ptNetworkInterface->tIPAddress.szSubnetMask
								  ,ptNetworkInterface->tIPAddress.szIPv6Address
								  ,ptNetworkInterface->tIPAddress.szBitMask
								  ,ptNetworkInterface->tIPAddress.tDefaultGateway.szIPAddress
								  ,ptNetworkInterface->tIPAddress.tDefaultGateway.szIPV6Address
								  ,ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPAddress
								  ,ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPV6Address
								  ,ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPAddress
								  ,ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPV6Address
								  ,ptNetworkInterface->tWireless.bEnabled
								  ,ptNetworkInterface->tWireless.szWirelessNetworkMode
								  ,ptNetworkInterface->tWireless.szChannel
								  ,ptNetworkInterface->tWireless.szSSID
								  ,ptNetworkInterface->tWireless.bWmmEnabled
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.szSecurityMode
								  );
	offset += iRet;						  
						     	
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,				     
								 "<WEP>"
								  "<authenticationType>%s</authenticationType>"
								  "<defaultTransmitKeyIndex>%u</defaultTransmitKeyIndex>"
								  "<wepKeyLength>%u</wepKeyLength>"
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.szAuthenticationType
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwDefaultTransmitKeyIndex
								  ,ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwWepKeyLength
								  );
	offset += iRet;	
		
			
	iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<EncryptionKeyList>");
	offset += iRet;	
		
	for(j=0;j< ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.dwListNum;j++)
	{
		  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<encryptionKey>"
											   "%s" 
											  "</encryptionKey>"
											  ,ptEncryptionKey->szEncryptionKey
				);
		  
		  
		  
		  ptEncryptionKey++;
		  offset += iRet;	
		
		}
								  
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</EncryptionKeyList>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</WEP>");
		offset += iRet;
							    
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<WPA>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<algorithmType>%s</algorithmType>",ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szAlgorithmType);
		offset += iRet;
		    
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<sharedKey>%s</sharedKey>",ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szSharedKey);
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</WPA>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</WirelessSecurity>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</Wireless>");
		offset += iRet;
		
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IEEE802_1x version=\"1.0\" >");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<enabled>%d</enabled>"
											"<authenticationProtocolType>%s</authenticationProtocolType>"
											"<innerTTLSAuthenticationMethod>%s</innerTTLSAuthenticationMethod>"
											"<innerEAPProtocolType>%s</innerEAPProtocolType>"
											"<validateServerEnabled>%d</validateServerEnabled>"
											"<userName>%s</userName>"
											"<password>%s</password>"
											"<anonymousID>%s</anonymousID>"
											"<autoPACProvisioningEnabled>%d</autoPACProvisioningEnabled>"
											,ptNetworkInterface->tIEEE8021X.bEnabled
											,ptNetworkInterface->tIEEE8021X.szAuthenticationProtocolType
											,ptNetworkInterface->tIEEE8021X.szInnerTTLSAuthenticationMethod
											,ptNetworkInterface->tIEEE8021X.szInnerEAPProtocolType	
											,ptNetworkInterface->tIEEE8021X.bValidateServerEnabled
											,ptNetworkInterface->tIEEE8021X.szUserName
											,ptNetworkInterface->tIEEE8021X.szPassword
											,ptNetworkInterface->tIEEE8021X.szAnonymousID
											,ptNetworkInterface->tIEEE8021X.bAutoPACProvisioningEnabled
											);
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IEEE802_1x>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IPFilter version=\"1.0\">");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<enabled>%d</enabled>"
											"<permissionType>%s</permissionType>"
											,ptNetworkInterface->tIPFilter.bEnabled
											,ptNetworkInterface->tIPFilter.szPermissionType
											 );
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IPFilterAddressList version=\"1.0\" >");
		offset += iRet;
		
		for(k=0;k<ptNetworkInterface->tIPFilter.tIPFilterAddressList.dwListNum;k++)
		{
		   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<IPFilterAddress version=\"1.0\" ><id>%s</id>"
											   "<permissionType>%s</permissionType>" 
											   "<addressFilterType>%s</addressFilterType>"
											   "<AddressRange>"
											      "<startIPAddress>%s</startIPAddress>"
											      "<endIPAddress>%s</endIPAddress>"
											      "<startIPv6Address>%s</startIPv6Address>"
											      "<endIPv6Address>%s</endIPv6Address>"
											   "</AddressRange>"
											   "<AddressMask>"
											      "<ipAddress>%s</ipAddress>"
											      "<ipv6Address>%s</ipv6Address>"
											      "<bitMask>%s</bitMask>"
											   "</AddressMask></IPFilterAddress>"
											  ,ptIPFilterAddress->szID
											  ,ptIPFilterAddress->szPermissionType
											  ,ptIPFilterAddress->szAddressFilterType
											  ,ptIPFilterAddress->tAddressRange.szStartIPAddress
											  ,ptIPFilterAddress->tAddressRange.szEndIPAddress
											  ,ptIPFilterAddress->tAddressRange.szStartIPv6Address
											  ,ptIPFilterAddress->tAddressRange.szEndIPv6Address
											  ,ptIPFilterAddress->tAddressMask.szIPAddress
											  ,ptIPFilterAddress->tAddressMask.szIPV6Address
											  ,ptIPFilterAddress->tAddressMask.szBitMask
											  );
		  
		  
		  ptIPFilterAddress++; 
		  offset += iRet;
		}
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IPFilterAddressList>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IPFilter>");
		offset += iRet;
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMP version=\"1.0\" >");
		offset += iRet;
		
		if(ptNetworkInterface->tSNMP.eSNMPChoice==esnmpv2c)
		{
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPv2c version=\"1.0\" >");
		    offset += iRet;
		    
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<notificationEnabled>"
											    "%d"
											    "</notificationEnabled>"
											    ,ptNetworkInterface->tSNMP.tSNMPV2c.bNotificationEnabled);
		    offset += iRet;
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPTrapReceiverList version=\"1.0\" >");
		    offset += iRet;
		    
		    
		    for(k=0;k<ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.dwListNum;i++)
		    {
			
		      iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPTrapReceiver>"
											      "<id>%s</id>"
											      "<ReceiverAddress>"
											      "<addressingFormatType>%s</addressingFormatType>"
											      "<hostName>%s</hostName>"
											      "<ipAddress>%s</ipAddress>"
											      "<ipv6Address>%s</ipv6Address>"
											      "<portNo>%d</portNo>"
											      "</ReceiverAddress>"
											      "<notificationType>%s</notificationType>"
											      "<communityString>%s</communityString>"
											      "</SNMPTrapReceiver>"
											      ,ptSNMPTrapReceiver->szID
											      ,ptSNMPTrapReceiver->tReceiverAddress.szAddressingFormatType
											      ,ptSNMPTrapReceiver->tReceiverAddress.szHostName
											      ,ptSNMPTrapReceiver->tReceiverAddress.szIPAddress
											      ,ptSNMPTrapReceiver->tReceiverAddress.szIPV6Address
											      ,ptSNMPTrapReceiver->tReceiverAddress.dwPortNo
											      ,ptSNMPTrapReceiver->szNotificationType
											      ,ptSNMPTrapReceiver->szCommunityString
											      );
											      
		      ptSNMPTrapReceiver++;
		      offset += iRet;
		      
		    
		    }
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMPTrapReceiverList>");
		    offset += iRet;
		    
		    
		    
		    
		    
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMPv2c>");
		    offset += iRet;
		    
		
		}
		else //SNMP Advanced
		{
		
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPAdvanced>");
		    offset += iRet;
		   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<localEngineID>%s</localEngineID>"
					"<authenticationNotificationEnabled>%d</authenticationNotificationEnabled>"
					"<notificationEnabled>%d</notificationEnabled>"
					 ,ptNetworkInterface->tSNMP.tSNMPAdvanced.szLocalEngineID
					 ,ptNetworkInterface->tSNMP.tSNMPAdvanced.bAuthenticationNotificationEnabled
					 ,ptNetworkInterface->tSNMP.tSNMPAdvanced. bNotificationEnabled
					 );
		    offset += iRet;
		    
		    for(k=0;k<ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.dwListNum;k++)
		    {
		      iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<SNMPUser>"
											      "<id>%s</id>"
											      "<userName>%s</userName>"
											      "<remoteEngineID>%s</remoteEngineID>"
											      "<snmpAuthenticationMethod>%s</snmpAuthenticationMethod>"
											      "<snmpAuthenticationKey>%s</snmpAuthenticationKey>"
											      "<snmpAuthenticationPassword>%s</snmpAuthenticationPassword>"
											      "<snmpPrivacyMethod>%s</snmpPrivacyMethod>"
											      "<snmpPrivacyKey>%s</snmpPrivacyKey>"
											      "<snmpPrivacyPassword>%s</snmpPrivacyPassword>"
											      "</SNMPUser>"
											      ,ptSNMPUser->szID
											      ,ptSNMPUser->szUserName 
											      ,ptSNMPUser->szRemoteEngineID 
											      ,ptSNMPUser->szSnmpAuthenticationMethod 
											      ,ptSNMPUser->szSnmpAuthenticationKey 
											      ,ptSNMPUser->szSnmpAuthenticationPassword 
											      ,ptSNMPUser->szSnmpPrivacyMethod 
											      ,ptSNMPUser->szSnmpPrivacyKey 
											      ,ptSNMPUser->szSnmpPrivacyPassword  
											      );
			ptSNMPUser++;								      
		       offset += iRet;
		    }
		    for(k=0;k<ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.dwListNum;k++)
		    {
		      //iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,""
			//								      ,
			//								      );
		      
		      
		      ptSNMPNotificationFilter++;
		      offset += iRet;
		    }
		    for(k=0;k<ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.dwListNum;k++)
		    {
		       
		      ptNotificationReceiver++;
		       offset += iRet;
		    }
		    
		    
		    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMPAdvanced>");
		    offset += iRet;
		
		}
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</SNMP>");
		offset += iRet;
		
		
		iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</NetworkInterface>");
	
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  


if(ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey!=NULL)
{
  free(ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey);
  ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey=NULL;
}


if(ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress!=NULL)
{
  free(ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress);
  ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress=NULL;
}


if(ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver!=NULL)
{
    free(ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver);
    ptNetworkInterface->tSNMP.tSNMPV2c.tSNMPTrapReceiverList.ptSNMPTrapReceiver=NULL;
}  

if(ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser!=NULL)
{
  free(ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser);
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPUserList.ptSNMPUser=NULL;

}

if(ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter!=NULL)
{
  free(ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter);
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationFilterList.ptSNMPNotificationFilter=NULL;
}

if(ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver!=NULL)
{
  free(ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver);
  ptNetworkInterface->tSNMP.tSNMPAdvanced.tSNMPNotificationReceiverList.ptNotificationReceiver=NULL;
}

if(ptNetworkInterface!=NULL)
{
  free(ptNetworkInterface);
  ptNetworkInterface=NULL;

}
  
  
  return  S_OK;
}


static void NetworkInterfaceGetID(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->szID,str,strlen(str));
  return;
}
static void NetworkInterfaceGetIPVersion(void *userData, const char *s, int len)
{
   TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.szIPVersion,str,strlen(str));
  return;

}
static void NetworkInterfaceGetIPType(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.szAddressingType,str,strlen(str));
  return;
}


static void NetworkInterfaceGetIPAddress(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.szIPAddress,str,strlen(str));
  return;
}


static void NetworkInterfaceGetSubnetMask(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.szSubnetMask,str,strlen(str));
  return;
}


static void NetworkInterfaceGetDefaultGateway(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.tDefaultGateway.szIPAddress,str,strlen(str));
  return;
}

static void NetworkInterfaceGetPrimaryDNS(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPAddress,str,strlen(str));
  return;
}


static void NetworkInterfaceGetSecondaryDNS(void *userData, const char *s, int len)
{
  TPsiaNetworkInterface *ptNetworkInterface = (TPsiaNetworkInterface *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPAddress,str,strlen(str));
  return;
}

static SCODE parse_netwrok_interface_msg(TPsiaNetworkInterface *ptNetworkInterface,CHAR *szMsg)
{


	int sRet;
	HANDLE hXmlWrapObj;
	//TODO:Now just parse the <IPAddress> section. In the future we will parse other sections.
	TXmlWrapperTreeMap ptParsingMap[] = {
	  {"ns:NetworkInterface/id",NULL,&NetworkInterfaceGetID,NULL},
	  {"ns:NetworkInterface/IPAddress/ipVersion",NULL,&NetworkInterfaceGetIPVersion,NULL},
	  {"ns:NetworkInterface/IPAddress/addressingType",NULL,&NetworkInterfaceGetIPType,NULL},
	  {"ns:NetworkInterface/IPAddress/ipAddress",NULL,&NetworkInterfaceGetIPAddress,NULL},
	  {"ns:NetworkInterface/IPAddress/subnetMask",NULL,&NetworkInterfaceGetSubnetMask,NULL},
	  {"ns:NetworkInterface/IPAddress/DefaultGateway/ipAddress",NULL,&NetworkInterfaceGetDefaultGateway,NULL},
	  {"ns:NetworkInterface/IPAddress/PrimaryDNS/ipAddress",NULL,&NetworkInterfaceGetPrimaryDNS,NULL},
	  {"ns:NetworkInterface/IPAddress/SecondaryDNS/ipAddress",NULL,&NetworkInterfaceGetSecondaryDNS,NULL},
	  {NULL, NULL, NULL}
	};
	TXmlWrapperNamespaceMap ptNSMap[] =
	{
		      {"ns", PSIA_NAMESPACE_STR},
		      {NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;
	//printf("[psiad_local](%d)szMsg:\n",__LINE__);
	//printf("%s\n",szMsg);
	//printf("[psiad_local](%d)End\n",__LINE__);
	if (ptNetworkInterface==NULL)
	{
	    printf("%s %d : ptNetworkInterface is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj, ptNetworkInterface)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;

}


SCODE Psiad_PutNetworkInterface(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResponseStatus  tResponseStatus;
  TPsiaNetworkInterface tNetworkInterface;
  if(ptPsiadInfo->pfnPutNetworkInterfaceFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutNetworkInterfaceFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tNetworkInterface,0x0,sizeof(TPsiaNetworkInterface));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_netwrok_interface_msg(&tNetworkInterface,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutNetworkInterfaceFunc(ptPsiadInfo->hAppObject, &tNetworkInterface,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;
  


}
SCODE Psiad_GetIPAddress(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  int  offset=0;
  int  iRet=0;
  TPsiaIPAddress *ptIPAddress=NULL;
  
  if(ptPsiadInfo->pfnGetIPAddressFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetIPAddressFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  ptIPAddress=(TPsiaIPAddress *)malloc(sizeof(TPsiaIPAddress));
  memset(ptIPAddress,0x0,sizeof(TPsiaIPAddress));

  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetIPAddressFunc(ptPsiadInfo->hAppObject,ptIPAddress)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
 
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<IPAddress  version=\"1.0\"  xmlns=\"%s\" >"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
  offset += iRet;
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
						    "<ipVersion>%s</ipVersion>"
						    "<addressingType>%s</addressingType>"
						     "<ipAddress>%s</ipAddress>"
							    "<subnetMask>%s</subnetMask>"
							    "<ipv6Address>%s</ipv6Address>"
							    "<bitMask>%s</bitMask>"
							    "<DefaultGateway>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</DefaultGateway>"
							    "<PrimaryDNS>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</PrimaryDNS>"
							    "<SecondaryDNS>"
							      "<ipAddress>%s</ipAddress>"
							      "<ipv6Address>%s</ipv6Address>"
							    "</SecondaryDNS>"
							      ,ptIPAddress->szIPVersion
							      ,ptIPAddress->szAddressingType
							      ,ptIPAddress->szIPAddress
							      ,ptIPAddress->szSubnetMask
							      ,ptIPAddress->szIPv6Address
							      ,ptIPAddress->szBitMask
							      ,ptIPAddress->tDefaultGateway.szIPAddress
							      ,ptIPAddress->tDefaultGateway.szIPV6Address
							      ,ptIPAddress->tPrimaryDNS.szIPAddress
							       ,ptIPAddress->tPrimaryDNS.szIPV6Address
							       ,ptIPAddress->tSecondaryDNS.szIPAddress
							       ,ptIPAddress->tSecondaryDNS.szIPV6Address
								);
	offset += iRet;						  
						     	
	
	
		
		
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</IPAddress>");
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  



if( ptIPAddress!=NULL)
{
  free( ptIPAddress);
   ptIPAddress=NULL;

}
  
  

  
  return  S_OK;
}


static void IPAddressGetIPVersion(void *userData, const char *s, int len)
{
  TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->szIPVersion,str,strlen(str));
  return;

}
static void IPAddressGetIPType(void *userData, const char *s, int len)
{
   TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->szAddressingType,str,strlen(str));
  return;
}


static void IPAddressGetIPAddress(void *userData, const char *s, int len)
{
  TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->szIPAddress,str,strlen(str));
  return;
}


static void IPAddressGetSubnetMask(void *userData, const char *s, int len)
{
  TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->szSubnetMask,str,strlen(str));
  return;
}


static void IPAddressGetDefaultGateway(void *userData, const char *s, int len)
{
   TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->tDefaultGateway.szIPAddress,str,strlen(str));
  return;
}

static void IPAddressGetPrimaryDNS(void *userData, const char *s, int len)
{
   TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->tPrimaryDNS.szIPAddress,str,strlen(str));
  return;
}


static void IPAddressGetSecondaryDNS(void *userData, const char *s, int len)
{
  TPsiaIPAddress *ptIPAddress = (TPsiaIPAddress *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptIPAddress->tSecondaryDNS.szIPAddress,str,strlen(str));
  return;
}

static SCODE parse_ipaddress_msg(TPsiaIPAddress *ptIPAddress,CHAR *szMsg)
{


	int sRet;
	HANDLE hXmlWrapObj;
	
	TXmlWrapperTreeMap ptParsingMap[] = {
	  
	  {"ns:IPAddress/ipVersion",NULL,&IPAddressGetIPVersion,NULL},
	  {"ns:IPAddress/addressingType",NULL,&IPAddressGetIPType,NULL},
	  {"ns:IPAddress/ipAddress",NULL,&IPAddressGetIPAddress,NULL},
	  {"ns:IPAddress/subnetMask",NULL,&IPAddressGetSubnetMask,NULL},
	  {"ns:IPAddress/DefaultGateway/ipAddress",NULL,&IPAddressGetDefaultGateway,NULL},
	  {"ns:IPAddress/PrimaryDNS/ipAddress",NULL,&IPAddressGetPrimaryDNS,NULL},
	  {"ns:IPAddress/SecondaryDNS/ipAddress",NULL,&IPAddressGetSecondaryDNS,NULL},
	  {NULL, NULL, NULL}
	};
	TXmlWrapperNamespaceMap ptNSMap[] =
	{
		      {"ns", PSIA_NAMESPACE_STR},
		      {NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;
	//printf("[psiad_local](%d)szMsg:\n",__LINE__);
	//printf("%s\n",szMsg);
	//printf("[psiad_local](%d)End\n",__LINE__);
	if (ptIPAddress==NULL)
	{
	    printf("%s %d : ptIPAddress is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj, ptIPAddress)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;

}
SCODE Psiad_PutIPAddress(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){

  TPsiaResponseStatus  tResponseStatus;
  TPsiaIPAddress tIPAddress;

   if(ptPsiadInfo->pfnPutIPAddressFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutIPAddressFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tIPAddress,0x0,sizeof( TPsiaIPAddress));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_ipaddress_msg(&tIPAddress,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutIPAddressFunc(ptPsiadInfo->hAppObject, &tIPAddress,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;
 
}
SCODE Psiad_GetWireless(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutWireless(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetIEEE8021x(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutIEEE8021x(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetIPFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutIPFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteIPFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMP(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMP(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPV2c(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPV2c(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPTrapReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPTrapReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPTrapReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvanced(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvanced(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPAdvancedUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedNotificationFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedNotificationFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPAdvancedNotificationFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedNotificationReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedNotificationReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSNMPAdvancedNotificationReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSNMPAdvancedV3(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSNMPAdvancedV3(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetQoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutQoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetCoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutCoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteCoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetDscp(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutDscp(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteDscp(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetDiscovery(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   int  offset=0;
  int  iRet=0;
  TPsiaDiscovery *ptDiscovery=NULL;
  
  if(ptPsiadInfo->pfnGetDiscoveryFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetDiscoveryFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  ptDiscovery=(TPsiaDiscovery *)malloc(sizeof(TPsiaDiscovery));
  memset(ptDiscovery,0x0,sizeof(TPsiaDiscovery));

  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetDiscoveryFunc(ptPsiadInfo->hAppObject, ptDiscovery)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
	goto exit;
  }
 
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<Discovery  version=\"1.0\"  xmlns=\"%s\" >"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
  offset += iRet;
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
						   "<UPnP><enabled>%d</enabled></UPnP>"
						   "<Zeroconf><enabled>%d</enabled></Zeroconf>"
						   "<MulticastDiscovery>"
						   "<enabled>%d</enabled>"
						   "<ipAddress>%s</ipAddress>"
						   "<ipv6Address>%s</ipv6Address>"
						   "<portNo>%d</portNo>"
						    "<ttl>%d</ttl>"
						   "</MulticastDiscovery>"
						    ,ptDiscovery->tUPnP.bEnabled
						    ,ptDiscovery->tZeroConf.bEnabled
						    ,ptDiscovery->tMulticastDiscovery.bEnabled
						    ,ptDiscovery->tMulticastDiscovery.szIPAddress
						    ,ptDiscovery->tMulticastDiscovery.szIPV6Address
						    ,ptDiscovery->tMulticastDiscovery.dwPortNo
						    ,ptDiscovery->tMulticastDiscovery.dwTTL
						    );
	offset += iRet;						  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</Discovery>");
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  

if(ptDiscovery!=NULL)
{
  free( ptDiscovery);
   ptDiscovery=NULL;

}
  
  return  S_OK;
 
}
static void DiscoveryGetUPnPEnabled(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  if(strncmp(str,"0",strlen(str))!=0)
  {  
    ptDiscovery->tUPnP.bEnabled=TRUE;
  }
  else
  {
    ptDiscovery->tUPnP.bEnabled=FALSE;
  }
  return;
}

static void DiscoveryGetZeroconfEnabled(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  if(strncmp(str,"0",strlen(str))!=0)
  {  
    ptDiscovery->tZeroConf.bEnabled=TRUE;
  }
  else
  {
   ptDiscovery->tZeroConf.bEnabled=FALSE;
  }
  return;
}
static void DiscoveryGetMulticastDiscoveryEnabled(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  if(strncmp(str,"0",strlen(str))!=0)
  {  
    ptDiscovery->tMulticastDiscovery.bEnabled=TRUE;
  }
  else
  {
   ptDiscovery->tMulticastDiscovery.bEnabled=FALSE;
  }
  return;

}
static void DiscoveryGetMulticastDiscoveryIPAddress(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDiscovery->tMulticastDiscovery.szIPAddress,str,strlen(str));
  return;
}
static void DiscoveryGetMulticastDiscoveryIPV6Address(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  strncpy(ptDiscovery->tMulticastDiscovery.szIPV6Address,str,strlen(str));
  return;
}
static void DiscoveryGetMulticastDiscoveryPortNo(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  ptDiscovery->tMulticastDiscovery.dwPortNo=strtoul(str,NULL,10);

}
static void DiscoveryGetMulticastDiscoveryTTL(void *userData, const char *s, int len)
{
  TPsiaDiscovery *ptDiscovery= (TPsiaDiscovery *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  ptDiscovery->tMulticastDiscovery.dwTTL=strtoul(str,NULL,10);

}
static SCODE parse_discovry_msg(TPsiaDiscovery *ptDiscovery,CHAR *szMsg)
{

  
	int sRet;
	HANDLE hXmlWrapObj;
	
	TXmlWrapperTreeMap ptParsingMap[] = {
	  
	  {"ns:Discovery/UPnP/enabled",NULL,&DiscoveryGetUPnPEnabled,NULL},
	  {"ns:Discovery/Zeroconf/enabled",NULL,&DiscoveryGetZeroconfEnabled,NULL},
	  {"ns:Discovery/MulticastDiscovery/enabled",NULL,&DiscoveryGetMulticastDiscoveryEnabled,NULL},
	  {"ns:Discovery/MulticastDiscovery/ipAddress",NULL,&DiscoveryGetMulticastDiscoveryIPAddress,NULL},
	  {"ns:Discovery/MulticastDiscovery/ipv6Address",NULL,&DiscoveryGetMulticastDiscoveryIPV6Address,NULL},
	  {"ns:Discovery/MulticastDiscovery/portNo",NULL,&DiscoveryGetMulticastDiscoveryPortNo,NULL},
	  {"ns:Discovery/MulticastDiscovery/ttl",NULL,&DiscoveryGetMulticastDiscoveryTTL,NULL},
	  {NULL, NULL, NULL}
	};
	TXmlWrapperNamespaceMap ptNSMap[] =
	{
		      {"ns", PSIA_NAMESPACE_STR},
		      {NULL, NULL}
	};
	
	TXmlWrapperInitOptions tInitOptions;
	//printf("[psiad_local](%d)szMsg:\n",__LINE__);
	//printf("%s\n",szMsg);
	//printf("[psiad_local](%d)End\n",__LINE__);
	if (ptDiscovery==NULL)
	{
	    printf("%s %d : ptDiscovery is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj, ptDiscovery)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;

}
SCODE Psiad_PutDiscovery(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResponseStatus  tResponseStatus;
  TPsiaDiscovery tDiscovery;

   if(ptPsiadInfo->pfnPutDiscoveryFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutDiscoveryFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tDiscovery,0x0,sizeof(  TPsiaDiscovery));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_discovry_msg(&tDiscovery,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutDiscoveryFunc(ptPsiadInfo->hAppObject, &tDiscovery,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;

}
SCODE Psiad_GetSyslog(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSyslog(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSyslogServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSyslogServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteSyslogServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetIOPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetIOPortStatuses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetInputPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetInputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutInputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetIOPortStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetOutputPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetOutputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutOutputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetIOportTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetAudioChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetAudioChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutAudioChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoOverlayImages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostVideoOverlayImages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoOverlayImages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoOverlayImage(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoOverlayImage(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoOverlayImage(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoInput(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoInputChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoInputChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoInputChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetVideoFocus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetVideoIRIS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoLens(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoTextOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoTextOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoTextOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetVideoImageOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutVideoImageOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteVideoImageOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPrivacyMask(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPrivacyMask(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostPrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPrivacyMaskRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPrivacyMaskRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePrivacyMaskRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSerialPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSerialPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSerialPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SendCommandToSerailPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetDiagnosticsCommands(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostDiagnosticsCommands(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteDiagnosticsCommands(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetDiagnosticsCommand(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteDiagnosticsCommand(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetSRTPMasterKey(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutSRTPMasterKey(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetDeviceCert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutDeviceCert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  int  offset=0;
  int  iRet=0;
  int  i=0;
  TPsiaUserList  tUserList;
  TPsiaUser *ptUser=NULL;
  
  
  if(ptPsiadInfo->pfnGetUsersFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetUsersFunc==NULL\n",__LINE__);
      goto exit;
  }
  
  memset(&tUserList,0x0,sizeof(TPsiaUserList));
  tUserList.ptUser=(TPsiaUser *)malloc(sizeof(TPsiaUser)*MAX_LIST_NUM);
  memset(tUserList.ptUser,0x0,sizeof(TPsiaUser)*MAX_LIST_NUM);
  tUserList.dwListNum=0;
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetUsersFunc(ptPsiadInfo->hAppObject, &tUserList)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
  }
 
  ptUser=tUserList.ptUser;
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<UserList  version=\"1.0\"  xmlns=\"%s\" >"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
  offset += iRet;
  for(i=0;i<tUserList.dwListNum;i++)
  {
    //TODO:Upload the  <ProtocolList> to the client.
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
						    "<User version=\"1.0\" >"
						    "<id>%s</id>"
						    "<userName>%s</userName>"
						    "<password>%s</password>"
						    "</User>"
						      ,ptUser->szID
						      ,ptUser->szUserName
						      ,ptUser->szPassword
						      );
    offset += iRet;
    ptUser++;
  
  }

  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</UserList>");
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  

  if(tUserList.ptUser!=NULL)
  {
    free(tUserList.ptUser);
    tUserList.ptUser=NULL;
  }
  
  return  S_OK;
}



static DWORD gdwCurrentUserID=0;

static void UsersGetPassword(void *userData, const char *s, int len)
{
  TPsiaUserList  *ptUserList= (TPsiaUserList  *)userData;
  TPsiaUser *ptUser=NULL;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  ptUser=(TPsiaUser *)(ptUserList->ptUser+gdwCurrentUserID);
  if(ptUser!=NULL)
  {
    strncpy(ptUser->szPassword,str,strlen(str));
  }
}


static void UsersGetName(void *userData, const char *s, int len)
{
  TPsiaUserList  *ptUserList= (TPsiaUserList  *)userData;
  TPsiaUser *ptUser=NULL;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  ptUser=(TPsiaUser *)(ptUserList->ptUser+gdwCurrentUserID);
  if(ptUser!=NULL)
  {
    strncpy(ptUser->szUserName,str,strlen(str));
  }
}

static void UsersGetID(void *userData, const char *s, int len)
{
  TPsiaUserList  *ptUserList= (TPsiaUserList  *)userData;
  TPsiaUser *ptUser=NULL;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
  gdwCurrentUserID=strtoul(str,NULL,10);
  //printf("[psiad_local](%d)gdwCurrentUserID=%u\n",__LINE__,gdwCurrentUserID);
  if(gdwCurrentUserID >= MAX_LIST_NUM)
  {
    gdwCurrentUserID=0;
  }
 
  ptUser=(TPsiaUser *)(ptUserList->ptUser+gdwCurrentUserID);
  
  if(ptUser!=NULL)
  {
    strncpy(ptUser->szID,str,strlen(str));
  }
}

static SCODE  parse_users_msg(TPsiaUserList  *ptUserList,CHAR * szMsg)
{
  //  printf("[psiad_loacl](%d)szMsg:%s\n",__LINE__,szMsg);
  int sRet;
  HANDLE hXmlWrapObj;
  
  //TODO:handle the   <ProtocolList> from the client.
  TXmlWrapperTreeMap ptParsingMap[] = {
	  
	  {"ns:UserList/User/id",NULL,&UsersGetID,NULL},
	  {"ns:UserList/User/userName",NULL,&UsersGetName,NULL},
	  {"ns:UserList/User/password",NULL,&UsersGetPassword,NULL},
	  {NULL, NULL, NULL}
  };
	TXmlWrapperNamespaceMap ptNSMap[] =
	{
		      {"ns", PSIA_NAMESPACE_STR},
		      {NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;
	//printf("[psiad_local](%d)szMsg:\n",__LINE__);
	//printf("%s\n",szMsg);
	//printf("[psiad_local](%d)End\n",__LINE__);
	if (ptUserList==NULL)
	{
	    printf("%s %d : ptUserList is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj, ptUserList)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	ptUserList->dwListNum=gdwCurrentUserID+1;
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;



}
SCODE Psiad_PutUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{

  TPsiaResponseStatus  tResponseStatus;
 
  TPsiaUserList tUserList;

   if(ptPsiadInfo->pfnPutUsersFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutUsersFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tUserList,0x0,sizeof(TPsiaUserList));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  
   
   tUserList.ptUser=(TPsiaUser *)malloc(sizeof(TPsiaUser)* MAX_LIST_NUM);
   memset(tUserList.ptUser,0x0,sizeof(TPsiaUser)* tUserList.dwListNum);
   
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_users_msg(&tUserList,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutUsersFunc(ptPsiadInfo->hAppObject, &tUserList,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:  

    if(tUserList.ptUser!=NULL)
    {
    
      free(tUserList.ptUser);
      tUserList.ptUser=NULL;
    }
     return  S_OK;
  
  
  

}


static void UserGetPassword(void *userData, const char *s, int len)
{
  TPsiaUser *ptUser= (TPsiaUser *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
 strncpy(ptUser->szPassword,str,strlen(str));

}


static void UserGetName(void *userData, const char *s, int len)
{
  TPsiaUser *ptUser= (TPsiaUser *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
 strncpy(ptUser->szUserName,str,strlen(str));

}

static void UserGetID(void *userData, const char *s, int len)
{
  TPsiaUser *ptUser= (TPsiaUser *)userData;
  char str[len+1];
  memset(str,0x0,sizeof(str));
  memcpy(str, s, len);
  str[len]='\0';
 strncpy(ptUser->szID,str,strlen(str));

}
static SCODE parse_user_msg(TPsiaUser *ptUser,CHAR *szMsg)
{
//  printf("[psiad_loacl](%d)szMsg:%s\n",__LINE__,szMsg);
  int sRet;
  HANDLE hXmlWrapObj;
  
  //TODO:handle the   <ProtocolList> from the client.
  TXmlWrapperTreeMap ptParsingMap[] = {
	  
	  {"ns:User/id",NULL,&UserGetID,NULL},
	  {"ns:User/userName",NULL,&UserGetName,NULL},
	  {"ns:User/password",NULL,&UserGetPassword,NULL},
	  {NULL, NULL, NULL}
  };
	TXmlWrapperNamespaceMap ptNSMap[] =
		{
		      {"ns", "urn:psialliance-org"},
		      {NULL, NULL}
		};
	TXmlWrapperInitOptions tInitOptions;
	//printf("[psiad_local](%d)szMsg:\n",__LINE__);
	//printf("%s\n",szMsg);
	//printf("[psiad_local](%d)End\n",__LINE__);
	if (ptUser==NULL)
	{
	    printf("%s %d : ptUser is NULL! \n", __FILE__, __LINE__);
	    return S_FAIL;
	}
	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	#if 0
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	#endif
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap,ptNSMap) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	if ((sRet=XmlWrapper_ReadBuf_UsrDefFunc(szMsg, hXmlWrapObj, ptUser)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x\n", __FILE__, __LINE__, sRet);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	
    return S_OK;

}

SCODE Psiad_PostUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{ 
  TPsiaResponseStatus  tResponseStatus;
 
  TPsiaUser tUser;

   if(ptPsiadInfo->pfnPostUsersFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPostUsersFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tUser,0x0,sizeof(TPsiaUser));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_user_msg(&tUser,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPostUsersFunc(ptPsiadInfo->hAppObject, &tUser,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;

}
SCODE Psiad_DeleteUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  TPsiaResponseStatus  tResponseStatus;


   if(ptPsiadInfo->pfnDeleteUsersFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnDeleteUsersFunc==NULL\n",__LINE__);
      goto exit;
  }

   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
 
   if(ptPsiadInfo->pfnDeleteUsersFunc(ptPsiadInfo->hAppObject,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;
}
SCODE Psiad_GetUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  int  offset=0;
  int  iRet=0;
 
  TPsiaUser *ptUser=NULL;
  
  
  if(ptPsiadInfo->pfnGetUserFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnGetUserFunc==NULL\n",__LINE__);
      goto exit;
  }
  

  ptUser=(TPsiaUser *)malloc(sizeof(TPsiaUser));

  memset(ptUser,0x0,sizeof(TPsiaUser));
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
  if(ptPsiadInfo->pfnGetUserFunc(ptPsiadInfo->hAppObject, ptUser)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
  }
 
 
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<User version=\"1.0\"  xmlns=\"%s\" >"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
  offset += iRet;
  
    //TODO:Upload the  <ProtocolList> to the client.
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
						    "<id>%s</id>"
						    "<userName>%s</userName>"
						    "<password>%s</password>"
						      ,ptUser->szID
						      ,ptUser->szUserName
						      ,ptUser->szPassword
						      );
    offset += iRet;
 
  
  

  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</User>");
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  

  if(ptUser!=NULL)
  {
    free(ptUser);
    ptUser=NULL;
  }
  
  return  S_OK;
}
SCODE Psiad_PutUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResponseStatus  tResponseStatus;
 
  TPsiaUser tUser;

   if(ptPsiadInfo->pfnPutUserFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnPutUserFunc==NULL\n",__LINE__);
      goto exit;
  }
   memset(&tUser,0x0,sizeof(TPsiaUser));
   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
   parse_user_msg(&tUser,(CHAR *)ptInputMsgInfo->pbyInboundData);
   if(ptPsiadInfo->pfnPutUserFunc(ptPsiadInfo->hAppObject, &tUser,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;

}
SCODE Psiad_DeleteUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  TPsiaResponseStatus  tResponseStatus;


   if(ptPsiadInfo->pfnDeleteUserFunc==NULL)
  {
      printf("[psiad_loacl](%d)ptPsiadInfo->pfnDeleteUserFunc==NULL\n",__LINE__);
      goto exit;
  }

   memset(&tResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   ptOutputMsgInfo->eProcResult=eomsgprocres_success;
 
   if(ptPsiadInfo->pfnDeleteUserFunc(ptPsiadInfo->hAppObject,&tResponseStatus)==S_FAIL)
   {
      ptOutputMsgInfo->eProcResult=eomsgprocres_error;
      goto exit;
   }
  
  ptOutputMsgInfo->eStatusCodeIndex=tResponseStatus.eStatusCodeIndex;
  
  
  snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER ,"%s"
						      "<ResponseStatus version=\"1.0\" xmlns=\"%s\">"
						      "<requestURL>%s</requestURL>"
						      "<statusCode>%d</statusCode>"
						      "<statusString>%s</statusString>"
						      "<id>%s</id>"
						      "</ResponseStatus>"
						      ,MSG_XML_DECLARATION
						      ,PSIA_NAMESPACE_STR
						      ,tResponseStatus.szRequestURL
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].dwStatusCode
						      ,g_psia_statuscodeInfo[tResponseStatus.eStatusCodeIndex].szStatusString
						      ,tResponseStatus.szID);
    ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);						      
   snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
							     "Content-Type: application/xml; charset=utf-8\r\n"
							     "Content-Length: %d\r\n"
							     "Connection: close\r\n"
							      "\r\n"
							      ,  ptOutputMsgInfo->dwReturnResultLen);  

							      
exit:      
     return  S_OK;  
  

}
SCODE Psiad_GetAAACert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutAAACert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetAdminAccess(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutAdminAccess(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteAdminAccess(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStreamingStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStreamingChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutStreamingChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteStreamingChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetStreamingSessionStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostPTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZhannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZhannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePTZhannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetPTZHomePosition(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetPTZContinuous(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetPTZMomentary(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetPTZRelative(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetPTZAbsolute(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_SetPTZDigital(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostPTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPreset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZPreset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePTZPreset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GotoPresetPosition(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostPTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPatrolStatuses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeletePTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_StartPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_StopPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PausePTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPatrolStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetPTZPatrolSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutPTZPatrolSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetMotionDetctions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetMotionDetction(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutMotionDetction(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetMotionDetectionRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutMotionDetectionRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteMotionDetectionRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventNotification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventNotification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteEventTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventTriggerNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventTriggerNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteEventTriggerNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventNotificationMethods(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutEventNotificationMethods(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetMailingNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutMailingNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteMailingNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetFTPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutFTPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteFTPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetHTTPHostNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutHTTPHostNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostHTTPHostNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteHTTHostPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetHTTPHostNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PutHTTPHostNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_DeleteHTTHostPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_GetEventNotificationAlert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}
SCODE Psiad_PostEventNotificationAlert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo){return  S_OK;}


static SCODE Psiad_GetServiceDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   FGetServiceDescription pfGetServiceDescription=NULL;
   TPsiaResourceDescription tPsiaResourceDescription;
 
   int iRet=0;
   int offset=0;
   int i=0;
   TPsiaQueryStrParameter *ptQueryStrParameter=NULL;
   
   
   switch(ptPsiadInfo->eCurrentProcMsg)
   {
    case emsg_getRootServiceDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetRootServiceDescriptionFunc;
      break; 
    case emsg_getSystemDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSystemDescriptionFunc;
      break;
    case emsg_getSystemNetworkDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSystemNetworkDescriptionFunc;
      break;	  
    case emsg_getSystemIODescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSystemIODescriptionFunc;
      break;	  
    case emsg_getSystemAudioDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSystemAudioDescriptionFunc;
      break;
    case emsg_getSystemVideoDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSystemVideoDescriptionFunc;
      break;
    case emsg_getStreamingDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetStreamingDescriptionFunc;
      break;	  
    case emsg_getPTZDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetPTZDescriptionFunc;
      break;
    case  emsg_getCustomMotionDetectionDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetCustomMotionDetectionDescriptionFunc;
      break;
    case  emsg_getSecurityDescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSecurityDescriptionFunc;
      break;	  
    case  emsg_getSecurityAAADescription:
      pfGetServiceDescription=ptPsiadInfo->pfnGetSecurityAAADescriptionFunc;
      break;
    case  emsg_unknown:
    default: 
    break; 
  }

  if( pfGetServiceDescription == NULL)
  {
    printf("[psiad_loacl](%d)Psiad_GetServiceDescription()pfGetServiceDescription==NULL\n",__LINE__);
    goto exit;
  }

  memset(&tPsiaResourceDescription,0x0,sizeof(TPsiaResourceDescription));
  
  tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter=(TPsiaQueryStrParameter *)malloc(sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  memset( tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter,0x0,sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.dwQueryStrParameterListNum=0;
  
  tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter=(TPsiaQueryStrParameter *)malloc(sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  memset( tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter,0x0,sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.dwQueryStrParameterListNum=0;
  
  
  tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter=(TPsiaQueryStrParameter *)malloc(sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  memset( tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter,0x0,sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.dwQueryStrParameterListNum=0;
  
  
  
  tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter=(TPsiaQueryStrParameter *)malloc(sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  memset( tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter,0x0,sizeof(TPsiaQueryStrParameter)*MAX_LIST_NUM);
  tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.dwQueryStrParameterListNum=0;
  
  
  
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;

  if(pfGetServiceDescription(ptPsiadInfo->hAppObject,&tPsiaResourceDescription)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
       goto exit;
  }
  
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<ResourceDescription  version=\"1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"%s\">"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
  offset += iRet;
  

 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<name>%s</name>"
									 "<version>%s</version>"
									 "<type>%s</type>"
									 "<description>%s</description>"
									 "<notes>%s</notes>"
									,tPsiaResourceDescription.szName
									,tPsiaResourceDescription.szVersion
									,tPsiaResourceDescription.szType
									,tPsiaResourceDescription.szDescription
									,tPsiaResourceDescription.szNotes);
									
 offset += iRet;
 
 /*===================================================================*/
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<get>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<QueryStringParameterList>");
 offset += iRet;
 
 
 ptQueryStrParameter=tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter;
 for(i=0;i<tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.dwQueryStrParameterListNum;i++)
 {
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<QueryStringParameter>"
									 "<name>%s</name>"
									 "<type>%s</type>"
									 "<description>%s</description>"
									 "</QueryStringParameter>"
									,ptQueryStrParameter->szName
									,ptQueryStrParameter->szType
									,ptQueryStrParameter->szDescription
									);
		
 
  ptQueryStrParameter++;
  offset += iRet;
 }
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</QueryStringParameterList>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<inboundData>%s</inboundData>"
									  "<returnResult>%s</returnResult>"
									  "<function>%s</function>"
									  "<notes>%s</notes>"
									  ,tPsiaResourceDescription.tURLParametersGet.szInBoundData
									  ,tPsiaResourceDescription.tURLParametersGet.szReturnResult
									  ,tPsiaResourceDescription.tURLParametersGet.szFunction
									  ,tPsiaResourceDescription.tURLParametersGet.szNotes
									  );
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</get>");
 offset += iRet;
  /*===================================================================*/
  
   iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<put>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<QueryStringParameterList>");
 offset += iRet;
 
 
 ptQueryStrParameter=tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter;
 for(i=0;i<tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.dwQueryStrParameterListNum;i++)
 {
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<QueryStringParameter>"
									 "<name>%s</name>"
									 "<type>%s</type>"
									 "<description>%s</description>"
									 "</QueryStringParameter>"
									,ptQueryStrParameter->szName
									,ptQueryStrParameter->szType
									,ptQueryStrParameter->szDescription
									);
		
 
  ptQueryStrParameter++;
  offset += iRet;
 }
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</QueryStringParameterList>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<inboundData>%s</inboundData>"
									  "<returnResult>%s</returnResult>"
									  "<function>%s</function>"
									  "<notes>%s</notes>"
									  ,tPsiaResourceDescription.tURLParametersPut.szInBoundData
									  ,tPsiaResourceDescription.tURLParametersPut.szReturnResult
									  ,tPsiaResourceDescription.tURLParametersPut.szFunction
									  ,tPsiaResourceDescription.tURLParametersPut.szNotes
									  );
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</put>");
 offset += iRet;
 /*===================================================================*/
  
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<post>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<QueryStringParameterList>");
 offset += iRet;
 
 
 ptQueryStrParameter=tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter;
 for(i=0;i<tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.dwQueryStrParameterListNum;i++)
 {
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<QueryStringParameter>"
									 "<name>%s</name>"
									 "<type>%s</type>"
									 "<description>%s</description>"
									 "</QueryStringParameter>"
									,ptQueryStrParameter->szName
									,ptQueryStrParameter->szType
									,ptQueryStrParameter->szDescription
									);
		
 
  ptQueryStrParameter++;
  offset += iRet;
 }
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</QueryStringParameterList>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<inboundData>%s</inboundData>"
									  "<returnResult>%s</returnResult>"
									  "<function>%s</function>"
									  "<notes>%s</notes>"
									  ,tPsiaResourceDescription.tURLParametersPost.szInBoundData
									  ,tPsiaResourceDescription.tURLParametersPost.szReturnResult
									  ,tPsiaResourceDescription.tURLParametersPost.szFunction
									  ,tPsiaResourceDescription.tURLParametersPost.szNotes
									  );
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</post>");
 offset += iRet;
 /*===================================================================*/
  
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<delete>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<QueryStringParameterList>");
 offset += iRet;
 
 
 ptQueryStrParameter=tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter;
 for(i=0;i<tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.dwQueryStrParameterListNum;i++)
 {
    iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"<QueryStringParameter>"
									 "<name>%s</name>"
									 "<type>%s</type>"
									 "<description>%s</description>"
									 "</QueryStringParameter>"
									,ptQueryStrParameter->szName
									,ptQueryStrParameter->szType
									,ptQueryStrParameter->szDescription
									);
		
 
  ptQueryStrParameter++;
  offset += iRet;
 }
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</QueryStringParameterList>");
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "<inboundData>%s</inboundData>"
									  "<returnResult>%s</returnResult>"
									  "<function>%s</function>"
									  "<notes>%s</notes>"
									  ,tPsiaResourceDescription.tURLParametersDelete.szInBoundData
									  ,tPsiaResourceDescription.tURLParametersDelete.szReturnResult
									  ,tPsiaResourceDescription.tURLParametersDelete.szFunction
									  ,tPsiaResourceDescription.tURLParametersDelete.szNotes
									  );
 offset += iRet;
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset, "</delete>");
 offset += iRet;
 /*===================================================================*/
 
 
 
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</ResourceDescription>");
 offset += iRet;
  
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
 
  
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:


  if(tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter!=NULL)
  {
    free(tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter);
     tPsiaResourceDescription.tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter=NULL; 
  }
  
  
  if(tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter!=NULL)
  {
    free(tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter);
     tPsiaResourceDescription.tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter=NULL; 
  }
  
  if(tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter!=NULL)
  {
    free(tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter);
     tPsiaResourceDescription.tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter=NULL; 
  }
  
  if(tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter!=NULL)
  {
    free(tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter);
    tPsiaResourceDescription.tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter=NULL; 
  }
  
  return S_OK;

}

static SCODE Psiad_GetServiceIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  TPsiaResource tResource;
  TPsiaResource *ptResource=NULL;
  int iRet=0;
  int offset=0;
  int i=0;
  FGetServiceIndex pfGetServiceIndex=NULL;

  
  
  switch(ptPsiadInfo->eCurrentProcMsg)
  {
    case emsg_getRootServiceIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetRootServiceIndexFunc;
      break;
    case emsg_getRootServiceIndexr:
      pfGetServiceIndex=ptPsiadInfo->pfnGetRootServiceIndexrFunc;
      break;  
    case emsg_getSystemIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSystemIndexFunc;
      break;
    case emsg_getSystemNetworkIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSystemNetworkIndexFunc;
      break;	  
    case emsg_getSystemIOIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSystemIOIndexFunc;
      break;	  
    case emsg_getSystemAudioIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSystemAudioIndexFunc;
      break;
    case emsg_getSystemVideoIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSystemVideoIndexFunc;
      break;
    case emsg_getStreamingIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetStreamingIndexFunc;
      break;	  
    case emsg_getPTZIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetPTZIndexFunc;
      break;
    case  emsg_getCustomMotionDetectionIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetCustomMotionDetectionIndexFunc;
      break;
    case  emsg_getSecurityIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSecurityIndexFunc;
      break;	  
    case  emsg_getSecurityAAAIndex:
      pfGetServiceIndex=ptPsiadInfo->pfnGetSecurityAAAIndexFunc;
      break;
    case  emsg_unknown:
    default: 
    break; 
  }  
 // printf("[psiad_loacl](%d)Psiad_GetServiceIndex()\n",__LINE__);
  if( pfGetServiceIndex==NULL)
  {
    printf("[psiad_loacl](%d)Psiad_GetServiceIndex()pfGetServiceIndex==NULL\n",__LINE__);
    goto exit;
  }
  
  memset(&tResource,0x0,sizeof(TPsiaResource));
 
  
  
  tResource.tResourceList.ptResource=(TPsiaResource *)malloc(sizeof(TPsiaResource)*MAX_LIST_NUM);
  memset( tResource.tResourceList.ptResource,0x0,sizeof(TPsiaResource)*MAX_LIST_NUM);
  tResource.tResourceList.dwResourceListNum=0;
  
  
  ptOutputMsgInfo->eProcResult=eomsgprocres_success;
 
  if(pfGetServiceIndex(ptPsiadInfo->hAppObject,&tResource)==S_FAIL)
  {
       ptOutputMsgInfo->eProcResult=eomsgprocres_error;
       goto exit;
  }
  
 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult,MAX_BUFFER,"%s"
						    "<ResourceList  version=\"1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"%s\">"
						    ,MSG_XML_DECLARATION
						    ,PSIA_NAMESPACE_STR
						    );
 offset += iRet;

 iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
						    "<Resource version=\"1.0\">"
						    "<name>%s</name>"
						    "<version>%s</version>"
						    "<type>%s</type>"
						    "</Resource>"
						    ,tResource.szName
						    ,tResource.szVersion
						    ,tResource.szType  );
 offset += iRet;
						  
//TODO:Recursively parsing the resource data is  not implemented yet!
  ptResource=tResource.tResourceList.ptResource;
  for(i=0;i< tResource.tResourceList.dwResourceListNum;i++)
  {
  
     iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,
						    "<Resource version=\"1.0\">"
						    "<name>%s</name>"
						    "<version>%s</version>"
						    "<type>%s</type>"
						    "</Resource>"
						    ,ptResource->szName
						    ,ptResource->szVersion
						    ,ptResource->szType  );
  
    
    ptResource++;
    offset += iRet;
  }
					    
 // offset += iRet;						    					    
  iRet=snprintf(ptOutputMsgInfo->pbyReturnResult+offset,MAX_BUFFER-offset,"</ResourceList>");
   offset += iRet;
  ptOutputMsgInfo->dwReturnResultLen=strlen(ptOutputMsgInfo->pbyReturnResult);
 // printf("ptOutputMsgInfo->pbyReturnResult=>\n");
   // printf("%s\n",ptOutputMsgInfo->pbyReturnResult);
  
  snprintf(ptOutputMsgInfo->szHTTPHeader,HTTP_HEADER_SIZE,"HTTP/1.1 200 OK \r\n"
					"Content-Type: application/xml; charset=utf-8\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"\r\n"
					,ptOutputMsgInfo->dwReturnResultLen);
exit:  
  if(tResource.tResourceList.ptResource!=NULL)
  {
    free(tResource.tResourceList.ptResource);
    tResource.tResourceList.ptResource=NULL;
  }
  return  S_OK;
}

SCODE Psiad_GetRootServiceIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  //printf("[psiad_loacl](%d)Psiad_GetRootServiceIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;
}

SCODE Psiad_GetRootServiceIndexr(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  //printf("[psiad_loacl](%d)Psiad_GetRootServiceIndexr()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetRootServiceDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSystemIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetSystemIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSystemDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
  
}
SCODE Psiad_GetSystemNetworkIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  //printf("[psiad_loacl](%d)Psiad_GetSystemNetworkIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSystemNetworkDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSystemIOIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetSystemIOIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSystemIODescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetSystemAudioIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetSystemAudioIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetSystemAudioDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSystemVideoIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetSystemVideoIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetSystemVideoDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetStreamingIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetStreamingIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetStreamingDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetPTZIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetPTZIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetPTZDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetCustomMotionDetectionIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetCustomMotionDetectionIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetCustomMotionDetectionDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
 Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSecurityIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetSecurityIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetSecurityDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
  Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
SCODE Psiad_GetSecurityAAAIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
   //printf("[psiad_loacl](%d)Psiad_GetSecurityAAAIndex()\n",__LINE__);
  Psiad_GetServiceIndex(ptPsiadInfo, ptInputMsgInfo,ptOutputMsgInfo);
  return  S_OK;

}
SCODE Psiad_GetSecurityAAADescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptInputMsgInfo, TPsiaOutputMsgInfo *ptOutputMsgInfo)
{
Psiad_GetServiceDescription(ptPsiadInfo,ptInputMsgInfo, ptOutputMsgInfo);
  return  S_OK;
}
