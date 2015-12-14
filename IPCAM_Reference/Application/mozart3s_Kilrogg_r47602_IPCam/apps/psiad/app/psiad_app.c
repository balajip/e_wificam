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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>

#include "psiad.h"


HANDLE hPsiadApp;

/* ============================================================================================= */
void print_usage(void)
{
    fprintf(stderr, "Psiad process\n"
           "Usage:\n"
           "   psiad [-D] [-h]  [-p pidfile]\n"
           "Options:\n"
           "    -D                 Run as Daemon\n"
           "    -p pid_file        Write PID to this file\n"
           "    -h                 This help\n");
    exit(1);
}

/* ============================================================================================= */
SCODE genPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen(szPidFile, "w");

    if (pid_fp == NULL) {
        fprintf(stderr, "Can't open %s\n", szPidFile);
        return S_FAIL;
    } else {
        fprintf(pid_fp, "%d\n", getpid());
        fchmod(fileno(pid_fp), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        fclose(pid_fp);
    }
    return S_OK;
}

/* ============================================================================================= */
void delPIDfile(char *szPidFile)
{
    FILE *pid_fp = fopen(szPidFile, "r");
    int   iStoredPid;
    if (pid_fp == NULL) {
        fprintf(stderr, "Can't open %s\n", szPidFile);
    }
    else {
        fscanf (pid_fp, "%d\n", &iStoredPid);
        fclose(pid_fp);
        if(iStoredPid == getpid()) {
            remove(szPidFile);
        }
    }
}
SCODE PsiadApp_Reload(HANDLE hPsiadObj)
{
	 Psiad_Reload(hPsiadObj);

	return S_OK;
}

SCODE PsiadApp_Stop(HANDLE hPsiadObj)
{
	Psiad_Stop(hPsiadObj);
	return S_OK;
}
/* ============================================================================================= */
void PsiadApp_sig_handler(int signum)
{
    switch(signum) {
        case SIGHUP:
		syslog(LOG_INFO, "[psiad]Reload....\n");
		PsiadApp_Reload(hPsiadApp);
                break;
        case SIGTERM:
	case SIGINT: 
		syslog(LOG_INFO, "[psiad]Terminate....\n");
		PsiadApp_Stop(hPsiadApp);
                break;
        default:
                break;
    }
    return;
}

/* ============================================================================================= */
void PsiadApp_Init_Signals(void)
{
    struct sigaction sigaInst;
    sigaInst.sa_flags = 0;
    sigemptyset(&sigaInst.sa_mask);

    sigaddset(&sigaInst.sa_mask, SIGTERM);
    sigaddset(&sigaInst.sa_mask, SIGINT);
    sigaddset(&sigaInst.sa_mask, SIGHUP);

    sigaInst.sa_handler = PsiadApp_sig_handler;
    sigaction(SIGTERM, &sigaInst, NULL);
    sigaction(SIGINT, &sigaInst, NULL);
    sigaction(SIGHUP, &sigaInst, NULL);
}

//ptPsiadInfo->pfnGetDeviceInfoFunc(ptPsiadInfo->hAppObject, &tDeviceInfo)
SCODE PsiadApp_GetDeviceInfo(HANDLE dwIntance, TPsiaDeviceInfo *ptDeviceInfo)
{
  
  if(ptDeviceInfo!=NULL)
  {
    
    memset(ptDeviceInfo->szDeviceName,0x0,sizeof(ptDeviceInfo->szDeviceName));
    strncpy(ptDeviceInfo->szDeviceName,"Mozart380",strlen("Mozart380"));
 
    memset(ptDeviceInfo->szDeviceUUID,0x0,sizeof(ptDeviceInfo->szDeviceUUID));
    strncpy(ptDeviceInfo->szDeviceUUID,"989cbea8-dc48-11d3-bb52-1f6d05a4b48e",strlen("989cbea8-dc48-11d3-bb52-1f6d05a4b48e"));
   
    memset(ptDeviceInfo->szDeviceDescription,0x0,sizeof(ptDeviceInfo->szDeviceDescription));
    strncpy(ptDeviceInfo->szDeviceDescription,"123",strlen("123"));
    
    memset(ptDeviceInfo->szDeviceLocation,0x0,sizeof(  ptDeviceInfo->szDeviceLocation));
    strncpy(ptDeviceInfo->szDeviceLocation,"123",strlen("123"));
    
    memset( ptDeviceInfo->szSystemContact,0x0,sizeof(  ptDeviceInfo->szSystemContact));
    strncpy( ptDeviceInfo->szSystemContact,"123",strlen("123"));
   
      
    memset( ptDeviceInfo->szModel,0x0,sizeof(  ptDeviceInfo->szModel));
    strncpy( ptDeviceInfo->szModel,"123",strlen("123"));
    
     memset(ptDeviceInfo->szSerialNumber,0x0,sizeof(  ptDeviceInfo->szSerialNumber));
    strncpy(ptDeviceInfo->szSerialNumber,"123",strlen("123"));;
    
    memset( ptDeviceInfo->szMacAddress,0x0,sizeof(   ptDeviceInfo->szMacAddress));
     strncpy(ptDeviceInfo->szMacAddress,"02:00:02:01:02:1B",strlen("02:00:02:01:02:1B"));
    
    memset(ptDeviceInfo->szFirmwareVersion,0x0,sizeof( ptDeviceInfo->szFirmwareVersion));
     strncpy(ptDeviceInfo->szFirmwareVersion,"123",strlen("123"));
    
    memset( ptDeviceInfo->szFirmwareReleasedDate,0x0,sizeof(  ptDeviceInfo->szFirmwareReleasedDate));
    strncpy(ptDeviceInfo->szFirmwareReleasedDate,"123",strlen("123"));
    
    memset(ptDeviceInfo->szLogicVersion,0x0,sizeof( ptDeviceInfo->szLogicVersion));
    strncpy(ptDeviceInfo->szLogicVersion,"123",strlen("123"));
    
    memset(  ptDeviceInfo->szLogicReleasedDate,0x0,sizeof(    ptDeviceInfo->szLogicReleasedDate));
    strncpy(ptDeviceInfo->szLogicReleasedDate,"123",strlen("123"));
    
    memset(ptDeviceInfo->szBootVersion,0x0,sizeof( ptDeviceInfo->szBootVersion));
    strncpy(ptDeviceInfo->szBootVersion,"123",strlen("123"));
    
    memset(ptDeviceInfo->szBootReleasedDate,0x0,sizeof(  ptDeviceInfo->szBootReleasedDate));
    strncpy(ptDeviceInfo->szBootReleasedDate,"123",strlen("123"));
    
    memset(ptDeviceInfo->szRescueVersion,0x0,sizeof( ptDeviceInfo->szRescueVersion));
    strncpy(ptDeviceInfo->szRescueVersion,"123",strlen("123"));
    
    memset(ptDeviceInfo->szRescueReleasedDate,0x0,sizeof( ptDeviceInfo->szRescueReleasedDate));
    strncpy(ptDeviceInfo->szRescueReleasedDate,"123",strlen("123"));
    
    memset( ptDeviceInfo->szHardwareVersion,0x0,sizeof(  ptDeviceInfo->szHardwareVersion));
    strncpy(ptDeviceInfo->szHardwareVersion,"123",strlen("123"));
    
    memset(ptDeviceInfo->szSystemObjectID,0x0,sizeof( ptDeviceInfo->szSystemObjectID));
    strncpy(ptDeviceInfo->szSystemObjectID,"123",strlen("123"));
 

  }
  return S_OK;
}

//ptPsiadInfo->pfnPutDeviceInfoFunc(ptPsiadInfo->hAppObject, &tDeviceInfo,&tResponseStatus)
SCODE PsiadApp_PutDeviceInfo(HANDLE dwIntance, TPsiaDeviceInfo *ptDeviceInfo,TPsiaResponseStatus *ptResponseStatus)
{

  printf("[psiad_app](%d)Recive Device Info:\n",__LINE__);
  printf("[psiad_app](%d) %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",__LINE__
			      ,ptDeviceInfo->szDeviceName
			      ,ptDeviceInfo->szDeviceUUID
			      ,ptDeviceInfo->szDeviceDescription
			      ,ptDeviceInfo->szDeviceLocation
			      ,ptDeviceInfo->szSystemContact
			      ,ptDeviceInfo->szModel
			      ,ptDeviceInfo->szSerialNumber
			      ,ptDeviceInfo->szMacAddress
			      ,ptDeviceInfo->szFirmwareVersion
			      ,ptDeviceInfo->szFirmwareReleasedDate
			      ,ptDeviceInfo->szLogicVersion
			      ,ptDeviceInfo->szLogicReleasedDate
			      ,ptDeviceInfo->szBootVersion
			      ,ptDeviceInfo->szBootReleasedDate
			      ,ptDeviceInfo->szRescueVersion
			      ,ptDeviceInfo->szRescueReleasedDate
			      ,ptDeviceInfo->szHardwareVersion
			      ,ptDeviceInfo->szSystemObjectID);
  
  printf("[psiad_app](%d)End of Recive Device Info\n",__LINE__);
  
  
  memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
  ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));


  return S_OK;
}

SCODE PsiadApp_GetDeviceStatus(HANDLE dwIntance,TPsiaDeviceStatus *ptDeviceStatus)
{

  
  DWORD dwTemperatureListNum=0;
  DWORD dwFanListNum=0;
  DWORD dwPressureListNum=0;
  DWORD dwTamperListNum=0;
  DWORD dwCPUListNum=0;
  DWORD dwMemoryListNum=0;
  
  
  TPsiaTemperature *ptTemperature=NULL;
  TPsiaFan *ptFan=NULL;
  TPsiaPressure *ptPressure=NULL;
  TPsiaTamper *ptTamper=NULL;
  TPsiaCPU *ptCPU=NULL;
  TPsiaMemory *ptMemory=NULL;
  
  int i=0;
  
  if(ptDeviceStatus!=NULL)
  {
    memset(ptDeviceStatus->szCurrentDeviceTime,0x0,sizeof(ptDeviceStatus->szCurrentDeviceTime));
    strncpy(ptDeviceStatus->szCurrentDeviceTime,"2002-05-30T09:00:00",strlen("2002-05-30T09:00:00"));
    ptDeviceStatus->dwDeviceUpTime=15;
    
    
    ptTemperature=ptDeviceStatus->tTemperatureList.ptTemperature;
    ptFan=ptDeviceStatus->tFanList.ptFan;
    ptPressure=ptDeviceStatus->tPressureList.ptPressure;
    ptTamper=ptDeviceStatus->tTamperList.ptTamper;
    ptCPU=ptDeviceStatus->tCPUList.ptCPU;
    ptMemory=ptDeviceStatus->tMemoryList.ptMemory;
    
    dwTemperatureListNum=2;
    dwFanListNum=2;
    dwPressureListNum=2;
    dwTamperListNum=2;
    dwCPUListNum=2;
    dwMemoryListNum=2;
    ptDeviceStatus->tTemperatureList.dwTemperatureListNum=dwTemperatureListNum;
    ptDeviceStatus->tFanList.dwFanListNum=dwFanListNum;
    ptDeviceStatus->tPressureList.dwPressureListNum=dwPressureListNum;
    ptDeviceStatus->tTamperList.dwTamperListNum=dwTamperListNum;
    ptDeviceStatus->tCPUList.dwCPUListNum=dwCPUListNum;
    ptDeviceStatus->tMemoryList.dwMemoryListNum=dwMemoryListNum;
    for(i=0;i<dwTemperatureListNum;i++)
    {
      strncpy(ptTemperature->szTempSensorDescription,"123",strlen("123"));
      ptTemperature->fTemperature=(FLOAT)i;
      ptTemperature++;
    }
    
      
    for(i=0;i<dwFanListNum;i++)
    {
      strncpy( ptFan->szFanDescription,"123",strlen("123"));
       ptFan->dwFanSpeed=(DWORD)i;
       ptFan++;
    }
    
    for(i=0;i<dwPressureListNum;i++)
    {
      strncpy( ptPressure->szPressureSensorDescription,"123",strlen("123"));
      ptPressure->dwPressure=(DWORD)i;
      ptPressure++;
    }
    
    for(i=0;i<dwTamperListNum;i++)
    {
      strncpy( ptTamper->szTamperSensorDescription,"123",strlen("123"));
      ptTamper->bTamper=TRUE;
      ptTamper++;
    }
    
     for(i=0;i<dwCPUListNum;i++)
    {
      strncpy( ptCPU->szCpuDescription,"123",strlen("123"));
      ptCPU->dwCPUUtilization=(DWORD)i;
      ptCPU++;
    }
      
    for(i=0;i<dwMemoryListNum;i++)
    {
      strncpy( ptMemory->szMemoryDescription,"123",strlen("123"));
      ptMemory->fMemoryUsage=(FLOAT)i;
       ptMemory->fMemoryAvailable=(FLOAT)(100-i);
      ptMemory++;
    }
    
    
    
    
  }
  
  
  return S_OK;

}
SCODE PsiadApp_GetRootServiceIndex(HANDLE dwIntance,TPsiaResource *ptResource)
{
  
 TPsiaResource *ptChildResource=NULL;
 strncpy(ptResource->szName,"index",strlen("index"));
 strncpy(ptResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptResource->szType,"resource",strlen("resource"));
 
 ptResource->tResourceList.dwResourceListNum=6;
 ptChildResource=ptResource->tResourceList.ptResource;

 strncpy(ptChildResource->szName,"System",strlen("System"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"Diagnostics",strlen("Diagnostics"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"Security",strlen("Security"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"Streaming",strlen("Streaming"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"PTZ",strlen("index"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"Custom",strlen("Custom"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 

 
 return S_OK;

}
SCODE PsiadApp_GetRootServiceIndexr(HANDLE dwIntance,TPsiaResource *ptResource)
{
  //TODO:Just use  Psiad_GetRootServiceIndex() to pass the responding message. But this is not correct!
  PsiadApp_GetRootServiceIndex(dwIntance,ptResource);
  return S_OK;
}

SCODE PsiadApp_GetSystemIndex(HANDLE dwIntance,TPsiaResource *ptResource)
{

  TPsiaResource *ptChildResource=NULL;
 strncpy(ptResource->szName,"index",strlen("index"));
 strncpy(ptResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptResource->szType,"resource",strlen("resource"));
 
 ptResource->tResourceList.dwResourceListNum=9;
 ptChildResource=ptResource->tResourceList.ptResource;


 strncpy(ptChildResource->szName,"reboot",strlen("reboot"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"updateFirmware",strlen("updateFirmware"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"configurationData",strlen("configurationData"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"factoryReset",strlen("factoryReset"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 ptChildResource++;
 strncpy(ptChildResource->szName,"deviceInfo",strlen("deviceInfo"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 
  ptChildResource++;
 strncpy(ptChildResource->szName,"supportReport",strlen("supportReport"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 
  ptChildResource++;
 strncpy(ptChildResource->szName,"status",strlen("status"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"resource",strlen("resource"));
 
 
  ptChildResource++;
 strncpy(ptChildResource->szName,"time",strlen("Custom"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 
  ptChildResource++;
 strncpy(ptChildResource->szName,"logging",strlen("Custom"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
  return S_OK;
}
SCODE PsiadApp_GetSystemNetworkIndex(HANDLE dwIntance,TPsiaResource *ptResource)
{
 
 TPsiaResource *ptChildResource=NULL;
 strncpy(ptResource->szName,"index",strlen("index"));
 strncpy(ptResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptResource->szType,"resource",strlen("resource"));
 
 ptResource->tResourceList.dwResourceListNum=1;
 ptChildResource=ptResource->tResourceList.ptResource;


 strncpy(ptChildResource->szName,"interfaces",strlen("interfaces"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 
 return S_OK;
}
SCODE PsiadApp_GetSecurityIndex(HANDLE dwIntance,TPsiaResource *ptResource)
{
  
  strncpy(ptResource->szName,"index",strlen("index"));
  strncpy(ptResource->szVersion,"1.0",strlen("1.0"));
  strncpy(ptResource->szType,"resource",strlen("resource"));
  ptResource->tResourceList.dwResourceListNum=0;
  
  return S_OK;
}
SCODE PsiadApp_GetSecurityAAAIndex(HANDLE dwIntance,TPsiaResource *ptResource)
{

 TPsiaResource *ptChildResource=NULL;
 strncpy(ptResource->szName,"index",strlen("index"));
 strncpy(ptResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptResource->szType,"resource",strlen("resource"));
 
 ptResource->tResourceList.dwResourceListNum=1;
 ptChildResource=ptResource->tResourceList.ptResource;


 strncpy(ptChildResource->szName,"users",strlen("users"));
 strncpy(ptChildResource->szVersion,"1.0",strlen("1.0"));
 strncpy(ptChildResource->szType,"service",strlen("service"));
 return S_OK;
}
SCODE PsiadApp_GetRootServiceDescription(HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription)
{
  
  TPsiaQueryStrParameter*ptQueryStrParameter=NULL;
  DWORD dwListNum=0;
  int i=0;
  
  strncpy(ptResourceDescription->szName,"index",strlen("index"));
  strncpy(ptResourceDescription->szVersion,"1.0",strlen("1.0"));
  strncpy(ptResourceDescription->szType,"resource",strlen("resource"));
  strncpy(ptResourceDescription->szDescription,"RootService",strlen("RootService"));
  strncpy(ptResourceDescription->szNotes,"none",strlen("none"));
  
  strncpy(ptResourceDescription->tURLParametersGet.szInBoundData,"none",strlen("none"));
  strncpy(ptResourceDescription->tURLParametersGet.szReturnResult,"none",strlen("none"));
  strncpy(ptResourceDescription->tURLParametersGet.szFunction,"none",strlen("none"));
  strncpy(ptResourceDescription->tURLParametersGet.szNotes,"none",strlen("none"));
  
  dwListNum=1;
  ptResourceDescription->tURLParametersGet.tQueryStrParameterList.dwQueryStrParameterListNum=dwListNum;
  ptQueryStrParameter=ptResourceDescription->tURLParametersGet.tQueryStrParameterList.ptQueryStrParameter;
  for(i=0;i<dwListNum;i++)
  {
   strncpy(ptQueryStrParameter->szName,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szType,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szDescription,"none",strlen("none"));
   ptQueryStrParameter++; 
  }
  
  
    
  dwListNum=1;
  ptResourceDescription->tURLParametersPut.tQueryStrParameterList.dwQueryStrParameterListNum=dwListNum;
  ptQueryStrParameter=ptResourceDescription->tURLParametersPut.tQueryStrParameterList.ptQueryStrParameter;
  for(i=0;i<dwListNum;i++)
  {
   strncpy(ptQueryStrParameter->szName,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szType,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szDescription,"none",strlen("none"));
   ptQueryStrParameter++; 
  }
  
  
    
  dwListNum=1;
  ptResourceDescription->tURLParametersPost.tQueryStrParameterList.dwQueryStrParameterListNum=dwListNum;
  ptQueryStrParameter=ptResourceDescription->tURLParametersPost.tQueryStrParameterList.ptQueryStrParameter;
  for(i=0;i<dwListNum;i++)
  {
   strncpy(ptQueryStrParameter->szName,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szType,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szDescription,"none",strlen("none"));
   ptQueryStrParameter++; 
  }
  
  
    
  dwListNum=1;
  ptResourceDescription->tURLParametersDelete.tQueryStrParameterList.dwQueryStrParameterListNum=dwListNum;
  ptQueryStrParameter=ptResourceDescription->tURLParametersDelete.tQueryStrParameterList.ptQueryStrParameter;
  for(i=0;i<dwListNum;i++)
  {
   strncpy(ptQueryStrParameter->szName,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szType,"none",strlen("none"));
   strncpy(ptQueryStrParameter->szDescription,"none",strlen("none"));
   ptQueryStrParameter++; 
  }
  
 return S_OK;
}
SCODE PsiadApp_GetSystemDescription(HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription)
{
  PsiadApp_GetRootServiceDescription(dwIntance,ptResourceDescription);
  return S_OK;
}
SCODE PsiadApp_GetSystemNetworkDescription(HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription)
{
    PsiadApp_GetRootServiceDescription(dwIntance,ptResourceDescription);
  return S_OK;

}
SCODE PsiadApp_GetSecurityDescription(HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription)
{

    PsiadApp_GetRootServiceDescription(dwIntance,ptResourceDescription);
  return S_OK;
}
SCODE PsiadApp_GetSecurityAAADescription(HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription)
{
    PsiadApp_GetRootServiceDescription(dwIntance,ptResourceDescription);
  return S_OK;

}
SCODE PsiadApp_GetNetworkInterfaces(HANDLE dwIntance,TPsiaNetworkInterfaceList *ptNetworkInterfaceList)
{
  TPsiaNetworkInterface *ptNetworkInterface=NULL;
  int i=0;
  
 // printf("[psiad_app](%d)Enter PsiadApp_GetNetworkInterfaces()\n",__LINE__);
  
  ptNetworkInterfaceList->dwListNum=1;
  ptNetworkInterface=ptNetworkInterfaceList->ptNetworkInterface;
  

  for(i=0;i<ptNetworkInterfaceList->dwListNum;i++)
  {
    strncpy(ptNetworkInterface->szID,"0",strlen("0"));//this szID must be 0 for testing /PSIA/System/Network/interfaces/0
    
    //tIPAddress
    strncpy(ptNetworkInterface->tIPAddress.szIPVersion,"v4",strlen("v4"));//v4,v6,dual
    strncpy(ptNetworkInterface->tIPAddress.szAddressingType,"dynamic",strlen("dynamic"));//dynamic,static,apipa
    strncpy(ptNetworkInterface->tIPAddress.szIPAddress,"172.17.207.27",strlen("172.17.207.27"));
    strncpy(ptNetworkInterface->tIPAddress.szSubnetMask,"255.255.0.0",strlen("255.255.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.szIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.szBitMask,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.tDefaultGateway.szIPAddress,"172.17.0.1",strlen("172.17.0.1"));
    strncpy(ptNetworkInterface->tIPAddress.tDefaultGateway.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPAddress,"192.168.0.10",strlen("192.168.0.10"));
    strncpy(ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPAddress,"192.168.0.20",strlen("192.168.0.20"));
    strncpy(ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    
    //tWireless
    ptNetworkInterface->tWireless.bEnabled=FALSE;
    strncpy(ptNetworkInterface->tWireless.szWirelessNetworkMode,"infrastructure",strlen("infrastructure"));//infrastructure,adhoc
    strncpy(ptNetworkInterface->tWireless.szChannel,"auto",strlen("auto"));//1,2,3,4,5,6,7,8,9,10,11,12,13,14,auto
    strncpy(ptNetworkInterface->tWireless.szSSID,"VN",strlen("VN"));
    ptNetworkInterface->tWireless.bWmmEnabled=FALSE;
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.szSecurityMode,"disable",strlen("disable"));//disable,WEP,WPA-personal,WPA2-personal,WPA-RADIUS,WPA-enterprise,WPA2-enterprise
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.szAuthenticationType,"open",strlen("open") );//open,sharedkey,auto         
    ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwDefaultTransmitKeyIndex=0;
    ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwWepKeyLength=128;//64,128
    ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.dwListNum=1;
    strncpy((ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey)->szEncryptionKey,"0123456789",strlen("0123456789"));
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szAlgorithmType,"TKIP/AES",strlen("TKIP/AES")); //TKIP,AES,TKIP/AES
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szSharedKey,"0123456789",strlen("0123456789")); 
    
    
    //tIEEE8021X
    ptNetworkInterface->tIEEE8021X.bEnabled=FALSE;
    strncpy(ptNetworkInterface->tIEEE8021X.szAuthenticationProtocolType,"EAP-TLS",strlen("EAP-TLS"));//EAP-TLS,EAP-TTLS,EAP-PEAP,EAP-LEAP,EAP-FAST
    strncpy(ptNetworkInterface->tIEEE8021X.szInnerTTLSAuthenticationMethod,"EAP-MD5",strlen("EAP-MD5"));//MS-CHAP,MS-CHAPv2,PAP,EAP-MD5
    strncpy(ptNetworkInterface->tIEEE8021X.szInnerEAPProtocolType,"MS-CHAPv2",strlen("MS-CHAPv2"));//EAP-POTP,MS-CHAPv2
    ptNetworkInterface->tIEEE8021X.bValidateServerEnabled=FALSE;      
    strncpy(ptNetworkInterface->tIEEE8021X.szUserName,"jeff.liao",strlen("jeff.liao"));
    strncpy(ptNetworkInterface->tIEEE8021X.szPassword,"02468",strlen("02468"));
    strncpy(ptNetworkInterface->tIEEE8021X.szAnonymousID,"jeff.liao@localhost",strlen("jeff.liao@localhost"));
    ptNetworkInterface->tIEEE8021X.bAutoPACProvisioningEnabled=FALSE;
    
    //tIPFilter
    ptNetworkInterface->tIPFilter.bEnabled=FALSE;
    strncpy(ptNetworkInterface->tIPFilter.szPermissionType,"allow",strlen("allow"));//deny,allow
    ptNetworkInterface->tIPFilter.tIPFilterAddressList.dwListNum=1;
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->szID,"123",strlen("123"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->szPermissionType,"allow",strlen("allow")); //deny,allow
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->szAddressFilterType,"range",strlen("range"));//mask,range
    
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szStartIPAddress,"172.17.0.1",strlen("172.17.0.1"));
    strncpy( (ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szEndIPAddress,"172.17.255.255",strlen("172.17.255.255"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szStartIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szEndIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressMask.szIPAddress,"0.0.0.0",strlen("0.0.0.0"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressMask.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressMask.szBitMask,"0.0.0.0",strlen("0.0.0.0"));
    
    
    ptNetworkInterface++;
  }
  
 // printf("[psiad_app](%d)Leave PsiadApp_GetNetworkInterfaces()\n",__LINE__);
  return S_OK;
}
SCODE PsiadApp_GetNetworkInterface(HANDLE dwIntance,TPsiaNetworkInterface *ptNetworkInterface)
{

  

  
  // printf("[psiad_app](%d)Enter PsiadApp_GetNetworkInterface()\n",__LINE__);
  strncpy(ptNetworkInterface->szID,"0",strlen("0"));//this szID must be 0 for testing /PSIA/System/Network/interfaces/0
    
    //tIPAddress
    strncpy(ptNetworkInterface->tIPAddress.szIPVersion,"v4",strlen("v4"));//v4,v6,dual
    strncpy(ptNetworkInterface->tIPAddress.szAddressingType,"dynamic",strlen("dynamic"));//dynamic,static,apipa
    strncpy(ptNetworkInterface->tIPAddress.szIPAddress,"172.17.207.27",strlen("172.17.207.27"));
    strncpy(ptNetworkInterface->tIPAddress.szSubnetMask,"255.255.0.0",strlen("255.255.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.szIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.szBitMask,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.tDefaultGateway.szIPAddress,"172.17.0.1",strlen("172.17.0.1"));
    strncpy(ptNetworkInterface->tIPAddress.tDefaultGateway.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPAddress,"192.168.0.10",strlen("192.168.0.10"));
    strncpy(ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPAddress,"192.168.0.20",strlen("192.168.0.20"));
    strncpy(ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    
    //tWireless
    ptNetworkInterface->tWireless.bEnabled=FALSE;
    strncpy(ptNetworkInterface->tWireless.szWirelessNetworkMode,"infrastructure",strlen("infrastructure"));//infrastructure,adhoc
    strncpy(ptNetworkInterface->tWireless.szChannel,"auto",strlen("auto"));//1,2,3,4,5,6,7,8,9,10,11,12,13,14,auto
    strncpy(ptNetworkInterface->tWireless.szSSID,"VN",strlen("VN"));
    ptNetworkInterface->tWireless.bWmmEnabled=FALSE;
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.szSecurityMode,"disable",strlen("disable"));//disable,WEP,WPA-personal,WPA2-personal,WPA-RADIUS,WPA-enterprise,WPA2-enterprise
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.szAuthenticationType,"open",strlen("open") );//open,sharedkey,auto         
    ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwDefaultTransmitKeyIndex=0;
    ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.dwWepKeyLength=128;//64,128
    ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.dwListNum=1;
    strncpy((ptNetworkInterface->tWireless.tWirelessSecurity.tWEP.tEncryptionKeyList.ptEncryptionKey)->szEncryptionKey,"0123456789",strlen("0123456789"));
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szAlgorithmType,"TKIP/AES",strlen("TKIP/AES")); //TKIP,AES,TKIP/AES
    strncpy(ptNetworkInterface->tWireless.tWirelessSecurity.tWPA.szSharedKey,"0123456789",strlen("0123456789")); 
    
    
    //tIEEE8021X
    ptNetworkInterface->tIEEE8021X.bEnabled=FALSE;
    strncpy(ptNetworkInterface->tIEEE8021X.szAuthenticationProtocolType,"EAP-TLS",strlen("EAP-TLS"));//EAP-TLS,EAP-TTLS,EAP-PEAP,EAP-LEAP,EAP-FAST
    strncpy(ptNetworkInterface->tIEEE8021X.szInnerTTLSAuthenticationMethod,"EAP-MD5",strlen("EAP-MD5"));//MS-CHAP,MS-CHAPv2,PAP,EAP-MD5
    strncpy(ptNetworkInterface->tIEEE8021X.szInnerEAPProtocolType,"MS-CHAPv2",strlen("MS-CHAPv2"));//EAP-POTP,MS-CHAPv2
    ptNetworkInterface->tIEEE8021X.bValidateServerEnabled=FALSE;      
    strncpy(ptNetworkInterface->tIEEE8021X.szUserName,"jeff.liao",strlen("jeff.liao"));
    strncpy(ptNetworkInterface->tIEEE8021X.szPassword,"02468",strlen("02468"));
    strncpy(ptNetworkInterface->tIEEE8021X.szAnonymousID,"jeff.liao@localhost",strlen("jeff.liao@localhost"));
    ptNetworkInterface->tIEEE8021X.bAutoPACProvisioningEnabled=FALSE;
    
    //tIPFilter
    ptNetworkInterface->tIPFilter.bEnabled=FALSE;
    strncpy(ptNetworkInterface->tIPFilter.szPermissionType,"allow",strlen("allow"));//deny,allow
    ptNetworkInterface->tIPFilter.tIPFilterAddressList.dwListNum=1;
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->szID,"123",strlen("123"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->szPermissionType,"allow",strlen("allow")); //deny,allow
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->szAddressFilterType,"range",strlen("range"));//mask,range
    
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szStartIPAddress,"172.17.0.1",strlen("172.17.0.1"));
    strncpy( (ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szEndIPAddress,"172.17.255.255",strlen("172.17.255.255"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szStartIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressRange.szEndIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressMask.szIPAddress,"0.0.0.0",strlen("0.0.0.0"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressMask.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy((ptNetworkInterface->tIPFilter.tIPFilterAddressList.ptIPFilterAddress)->tAddressMask.szBitMask,"0.0.0.0",strlen("0.0.0.0"));
    

  
  
  //printf("[psiad_app](%d)Leave PsiadApp_GetNetworkInterface()\n",__LINE__);
  return S_OK;
}
SCODE PsiadApp_PutNetworkInterface(HANDLE dwIntance, TPsiaNetworkInterface *ptNetworkInterface,TPsiaResponseStatus *ptResponseStatus)
{

  printf("[psiad_app](%d)Recive Netwrok interface:\n",__LINE__);
  printf("[psiad_app](%d) %s %s %s %s %s %s %s %s\n",__LINE__
			      ,ptNetworkInterface->tIPAddress.tSecondaryDNS.szIPAddress
			      ,ptNetworkInterface->tIPAddress.tPrimaryDNS.szIPAddress
			      ,ptNetworkInterface->tIPAddress.tDefaultGateway.szIPAddress
			      ,ptNetworkInterface->tIPAddress.szSubnetMask
			      ,ptNetworkInterface->tIPAddress.szIPAddress
			      ,ptNetworkInterface->tIPAddress.szAddressingType
			      ,ptNetworkInterface->szID
			      ,ptNetworkInterface->tIPAddress.szIPVersion
			      );
  
  printf("[psiad_app](%d)End of Recive Netwrok interface\n",__LINE__);
  
  
  memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
  ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));


  return S_OK;
}
SCODE PsiadApp_GetIPAddress(HANDLE dwIntance,TPsiaIPAddress *ptIPAddress)
{
  if(ptIPAddress!=NULL)
  {
    strncpy(ptIPAddress->szIPVersion,"v4",strlen("v4"));//v4,v6,dual
    strncpy(ptIPAddress->szAddressingType,"dynamic",strlen("dynamic"));//dynamic,static,apipa
    strncpy(ptIPAddress->szIPAddress,"172.17.207.27",strlen("172.17.207.27"));
    strncpy(ptIPAddress->szSubnetMask,"255.255.0.0",strlen("255.255.0.0"));
    strncpy(ptIPAddress->szIPv6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptIPAddress->szBitMask,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptIPAddress->tDefaultGateway.szIPAddress,"172.17.0.1",strlen("172.17.0.1"));
    strncpy(ptIPAddress->tDefaultGateway.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptIPAddress->tPrimaryDNS.szIPAddress,"192.168.0.10",strlen("192.168.0.10"));
    strncpy(ptIPAddress->tPrimaryDNS.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    strncpy(ptIPAddress->tSecondaryDNS.szIPAddress,"192.168.0.20",strlen("192.168.0.20"));
    strncpy(ptIPAddress->tSecondaryDNS.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
  } 
  return S_OK;
}

SCODE PsiadApp_PutIPAddress(HANDLE dwIntance, TPsiaIPAddress *ptIPAddress,TPsiaResponseStatus *ptResponseStatus)
{

  printf("[psiad_app](%d)Recive IP Address:\n",__LINE__);
  printf("[psiad_app](%d) %s %s %s %s %s %s %s\n",__LINE__
			      ,ptIPAddress->tSecondaryDNS.szIPAddress
			      ,ptIPAddress->tPrimaryDNS.szIPAddress
			      ,ptIPAddress->tDefaultGateway.szIPAddress
			      ,ptIPAddress->szSubnetMask
			      ,ptIPAddress->szIPAddress
			      ,ptIPAddress->szAddressingType
			      ,ptIPAddress->szIPVersion
			      );
  
  printf("[psiad_app](%d)End of Recive IP Address\n",__LINE__);
  
  
  memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
  ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));


  return S_OK;
}

SCODE PsiadApp_GetDiscovery(HANDLE dwIntance,TPsiaDiscovery *ptDiscovery)
{
  if(ptDiscovery!=NULL)
  {
  
    ptDiscovery->tUPnP.bEnabled=FALSE;
    ptDiscovery->tZeroConf.bEnabled=TRUE;
    ptDiscovery->tMulticastDiscovery.bEnabled=FALSE;
    strncpy(ptDiscovery->tMulticastDiscovery.szIPAddress,"172.17.207.27",strlen("172.17.207.27"));
    strncpy(ptDiscovery->tMulticastDiscovery.szIPV6Address,"0.0.0.0",strlen("0.0.0.0"));
    ptDiscovery->tMulticastDiscovery.dwPortNo=8080;
    ptDiscovery->tMulticastDiscovery.dwTTL=0;
  }

   return S_OK;
}
SCODE PsiadApp_PutDiscovery(HANDLE dwIntance, TPsiaDiscovery *ptDiscovery,TPsiaResponseStatus *ptResponseStatus)
{

  printf("[psiad_app](%d)Recive Discovery:\n",__LINE__);
  printf("[psiad_app](%d) %d %d %d %s %s %d %d\n",__LINE__
			    ,ptDiscovery->tUPnP.bEnabled
			    ,ptDiscovery->tZeroConf.bEnabled
			    ,ptDiscovery->tMulticastDiscovery.bEnabled
			    ,ptDiscovery->tMulticastDiscovery.szIPAddress
			    ,ptDiscovery->tMulticastDiscovery.szIPV6Address
			    ,ptDiscovery->tMulticastDiscovery.dwPortNo
			    ,ptDiscovery->tMulticastDiscovery.dwTTL
			      );
  
  printf("[psiad_app](%d)End of Recive Discovery\n",__LINE__);
  
  
  memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
  strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
  ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));


  return S_OK;
}
SCODE PsiadApp_UpdateFirmware(HANDLE dwIntance, TPsiaResponseStatus *ptResponseStatus)
{
  //TODO: Handle the updating-firmware process.
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));

  return S_OK;
}
SCODE PsiadApp_Reboot(HANDLE dwIntance, TPsiaResponseStatus *ptResponseStatus)
{

  //TODO: Handle the rebooting process.
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));

  return S_OK;
}

SCODE PsiadApp_FactoryReset(HANDLE dwIntance,CHAR *szMode,TPsiaResponseStatus *ptResponseStatus)
{

  //TODO: Handle the factoryReset process.
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));
  printf("[psiad_app](%d)Factory Reset Mode is \"%s\"\n",__LINE__,szMode);
  return S_OK;
}

SCODE PsiadApp_GetConfigurationData(HANDLE dwIntance, BYTE *pbyOpaqueData,DWORD *pdwLen)
{
  //TODO: Handle the process of uplading configurationData to client! Must handle the binary data in the future!
  
  *pdwLen=strlen("<?xml version=\"1.0\" standalone=\"yes\"?><root><a>jeff</a></root>");
  strncpy(pbyOpaqueData,"<?xml version=\"1.0\" standalone=\"yes\"?><root><a>jeff</a></root>",*pdwLen);
  
#if 0
  FILE *pfAFInput=NULL;
  if ((pfAFInput=fopen("test.tar.gz", "rb")) != NULL)
  {
				
	fseek (pfAFInput , 0 , SEEK_END);
	*pdwLen=ftell(pfAFInput);
	rewind (pfAFInput);
	fread(pbyOpaqueData,1,*pdwLen,pfAFInput);
	 printf("[psiad_app](%d)configurationData file size:%d\n",__LINE__,*pdwLen);
	fclose(pfAFInput);
  }
#endif  
  
  
  return S_OK;
}
SCODE PsiadApp_PutConfigurationData(HANDLE dwIntance, BYTE *pbyOpaqueData,TPsiaResponseStatus *ptResponseStatus)
{
  //TODO: Handle the process of getting configurationData from client!
   printf("[psiad_app](%d)Recive ConfigurationData:\n",__LINE__);
   printf("[psiad_app](%d)%s\n",__LINE__,pbyOpaqueData);
   printf("[psiad_app](%d)Recive ConfigurationData\n",__LINE__);
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));
    return S_OK;
}
SCODE PsiadApp_SupportReport(HANDLE dwIntance, BYTE *pbySupportData,DWORD *pdwLen)
{
  //TODO: Handle the process of uplading support report to client!
  return S_OK;
}
SCODE PsiadApp_GetUsers(HANDLE dwIntance, TPsiaUserList  *ptUserList)
{
  ptUserList->dwListNum=2;
  
  if(ptUserList->ptUser != NULL)
  {
    strncpy(ptUserList->ptUser->szID,"0",strlen("0"));
    strncpy(ptUserList->ptUser->szUserName,"jeff_0",strlen("jeff_0"));
    strncpy(ptUserList->ptUser->szPassword,"1234",strlen("1234"));
    
    strncpy((ptUserList->ptUser+1)->szID,"1",strlen("1"));
    strncpy((ptUserList->ptUser+1)->szUserName,"jeff_1",strlen("jeff_1"));
    strncpy((ptUserList->ptUser+1)->szPassword,"12345",strlen("12345"));
  }
  
  return S_OK;
}

SCODE PsiadApp_PostUsers(HANDLE dwIntance, TPsiaUser  *ptUser,TPsiaResponseStatus *ptResponseStatus)
{
  printf("[psiad_app](%d)Recive User:\n",__LINE__);
  printf("[psiad_app](%d) %s %s %s\n",__LINE__
			    ,ptUser->szID
			    ,ptUser->szUserName
			    ,ptUser->szPassword
			     );
  
  printf("[psiad_app](%d)End of Recive User\n",__LINE__);
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));
  return S_OK;
}
SCODE PsiadApp_PutUsers(HANDLE dwIntance, TPsiaUserList  *ptUserList,TPsiaResponseStatus *ptResponseStatus)
{
  int i=0;
  printf("[psiad_app](%d)Recive Users:\n",__LINE__);
  for(i=0;i<ptUserList->dwListNum;i++)
  {
    printf("[psiad_app](%d) %s %s %s\n",__LINE__
			    ,(ptUserList->ptUser+i)->szID
			    ,(ptUserList->ptUser+i)->szUserName
			    ,(ptUserList->ptUser+i)->szPassword
			     );
  }
  printf("[psiad_app](%d)End of Recive Users\n",__LINE__);
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));

    return S_OK;
}

SCODE PsiadApp_DeleteUsers(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus)
{
   //TODO: Handle the process of deleting users!
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));

  return S_OK;
}
SCODE PsiadApp_GetUser(HANDLE dwIntance, TPsiaUser  *ptUser)
{

    strncpy(ptUser->szID,"0",strlen("0"));
  strncpy(ptUser->szUserName,"jeff",strlen("jeff"));
   strncpy(ptUser->szPassword,"1234",strlen("1234"));
  return S_OK;
}
SCODE PsiadApp_PutUser(HANDLE dwIntance, TPsiaUser  *ptUser,TPsiaResponseStatus *ptResponseStatus)
{
  printf("[psiad_app](%d)Recive User:\n",__LINE__);
  printf("[psiad_app](%d) %s %s %s\n",__LINE__
			    ,ptUser->szID
			    ,ptUser->szUserName
			    ,ptUser->szPassword
			     );
  
  printf("[psiad_app](%d)End of Recive User\n",__LINE__);
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));
   return S_OK;
}
SCODE PsiadApp_DeleteUser(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus)
{
 //TODO: Handle the process of deleting user corresponding to /PSIA/Security/AAA/users/id !
   memset( ptResponseStatus,0x0,sizeof(TPsiaResponseStatus));
   strncpy(ptResponseStatus->szRequestURL,"172.17.207.27",strlen("172.17.207.27"));
   ptResponseStatus->eStatusCodeIndex=est_no_error;
   strncpy(ptResponseStatus->szID,"0",strlen("0"));
  return S_OK;
}
static void daemonize(void)
{
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}
#define  SCK_PATH "/var/run/psiad.sck"

int main(int argc, char *argv[])
{
    static CHAR* szPidFile = NULL;
    
    BOOL bIsDaemon = FALSE;
    SCODE scRes;
   
    int ch;
    TPsiadInitOpt  tPsiadInitOpt;
    
    
    
    

    while ((ch = getopt(argc, argv, "D:p:h:")) != -1) {
        switch(ch) {
            case 'D':
                bIsDaemon = TRUE;
                break;
            case 'p':
                szPidFile = strdup(optarg);
                break;
            default:
                print_usage();
                exit(1);
        }
    }

    // run as Daemon
    if (bIsDaemon)
    {
        daemonize();
    }
    syslog(LOG_INFO, "[psiad] Start psiad process with Pid : %d\n", getpid());
    // If the szPidFile is assigned write pid to szPidFile 
    if (szPidFile != NULL) {
        if (genPIDfile(szPidFile) != S_OK) {
            fprintf(stderr, "[psiad] Generate pid file fail !!\n");
            exit(1);
        }
    }

	
	memset(&tPsiadInitOpt,0x0,sizeof(TPsiadInitOpt));
	tPsiadInitOpt.szSckPath=strdup(SCK_PATH);
	tPsiadInitOpt.pfnGetDeviceInfoFunc=&PsiadApp_GetDeviceInfo;
	tPsiadInitOpt.pfnPutDeviceInfoFunc=&PsiadApp_PutDeviceInfo;
	tPsiadInitOpt.pfnGetDeviceStatusFunc=&PsiadApp_GetDeviceStatus;
	
	tPsiadInitOpt.pfnGetRootServiceIndexFunc=&PsiadApp_GetRootServiceIndex;
	tPsiadInitOpt.pfnGetRootServiceIndexrFunc=&PsiadApp_GetRootServiceIndexr;
	tPsiadInitOpt.pfnGetSystemIndexFunc=&PsiadApp_GetSystemIndex;
	tPsiadInitOpt.pfnGetSystemNetworkIndexFunc=&PsiadApp_GetSystemNetworkIndex;
	tPsiadInitOpt.pfnGetSecurityIndexFunc=&PsiadApp_GetSecurityIndex;
	tPsiadInitOpt.pfnGetSecurityAAAIndexFunc=&PsiadApp_GetSecurityAAAIndex;
	
	tPsiadInitOpt.pfnGetRootServiceDescriptionFunc=&PsiadApp_GetRootServiceDescription;
	tPsiadInitOpt.pfnGetSystemDescriptionFunc=&PsiadApp_GetSystemDescription;
	tPsiadInitOpt.pfnGetSystemNetworkDescriptionFunc=&PsiadApp_GetSystemNetworkDescription;
	tPsiadInitOpt.pfnGetSecurityDescriptionFunc=&PsiadApp_GetSecurityDescription;
	tPsiadInitOpt.pfnGetSecurityAAADescriptionFunc=&PsiadApp_GetSecurityAAADescription;
	tPsiadInitOpt.pfnGetNetworkInterfacesFunc=&PsiadApp_GetNetworkInterfaces;
	tPsiadInitOpt.pfnGetNetworkInterfaceFunc=&PsiadApp_GetNetworkInterface;
	tPsiadInitOpt.pfnPutNetworkInterfaceFunc=&PsiadApp_PutNetworkInterface;
	tPsiadInitOpt.pfnGetIPAddressFunc=&PsiadApp_GetIPAddress;
	tPsiadInitOpt.pfnPutIPAddressFunc=&PsiadApp_PutIPAddress;
	tPsiadInitOpt.pfnGetDiscoveryFunc=&PsiadApp_GetDiscovery;
	tPsiadInitOpt.pfnPutDiscoveryFunc=&PsiadApp_PutDiscovery;
	tPsiadInitOpt.pfnUpdateFirmwareFunc=&PsiadApp_UpdateFirmware;
	tPsiadInitOpt.pfnRebootFunc=&PsiadApp_Reboot;
	tPsiadInitOpt.pfnFactoryResetFunc=&PsiadApp_FactoryReset;
	tPsiadInitOpt.pfnGetConfigurationDataFunc=&PsiadApp_GetConfigurationData;
	tPsiadInitOpt.pfnPutConfigurationDataFunc=&PsiadApp_PutConfigurationData;
	tPsiadInitOpt.pfnSupportReportFunc=&PsiadApp_SupportReport;
	tPsiadInitOpt.pfnGetUsersFunc=&PsiadApp_GetUsers;
	tPsiadInitOpt.pfnPostUsersFunc=&PsiadApp_PostUsers;
	tPsiadInitOpt.pfnPutUsersFunc=&PsiadApp_PutUsers;
	tPsiadInitOpt.pfnDeleteUsersFunc=&PsiadApp_DeleteUsers;
	tPsiadInitOpt.pfnGetUserFunc=&PsiadApp_GetUser;
	tPsiadInitOpt.pfnPutUserFunc=&PsiadApp_PutUser;
	tPsiadInitOpt.pfnDeleteUserFunc=&PsiadApp_DeleteUser;
//printf("[psiad_app]%d\n",__LINE__);	
	scRes = Psiad_Initial(&hPsiadApp, tPsiadInitOpt);

//printf("[psiad_app]%d\n",__LINE__);
    if (scRes != S_OK) {
        fprintf(stderr, "[psiad] Initial psiad process err !!\n");
        exit(1);
    }
//printf("[psiad_app]%d\n",__LINE__);
	PsiadApp_Init_Signals();
//printf("[psiad_app]%d\n",__LINE__);	
	Psiad_Start(hPsiadApp);
//printf("[psiad_app]%d\n",__LINE__);	
	// after catch SIGINT or SIGTERM
	Psiad_Release(&hPsiadApp);
//printf("[psiad_app]%d\n",__LINE__);
    if (szPidFile != NULL) {
        delPIDfile(szPidFile);
    }

    free(szPidFile);
    syslog(LOG_INFO, "[psiad] Stop psiad process !!\n");
    return 0;
}