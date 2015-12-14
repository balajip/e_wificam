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


SCODE Psiad_Reload(HANDLE hPsiadObj)
{
	TPsiadInfo *ptPsiadInfo=(TPsiadInfo *)(hPsiadObj);

	ptPsiadInfo->bReload = TRUE;
	return S_OK;
}


SCODE Psiad_Start(HANDLE hPsiadObj)
{
  TPsiadInfo *ptPsiadInfo=(TPsiadInfo *)(hPsiadObj);
  Psiad_Loop(ptPsiadInfo);
  return S_OK;

}

SCODE Psiad_Stop(HANDLE hPsiadObj)
{
	 TPsiadInfo *ptPsiadInfo=(TPsiadInfo *)(hPsiadObj);

	ptPsiadInfo->bTerminate = TRUE;
	return S_OK;
}

SCODE Psiad_Release(HANDLE *phPsiadObj){

  TPsiadInfo *ptPsiadInfo=NULL;
  
  if(phPsiadObj!=NULL)
  {
      ptPsiadInfo=(TPsiadInfo *)(*phPsiadObj);
      if(ptPsiadInfo!=NULL)
      {
	  close(ptPsiadInfo->iHttpFdipcSck);
	free(ptPsiadInfo);
	ptPsiadInfo=NULL;
      }
  }
  return S_OK;
}
SCODE Psiad_Initial(HANDLE *phPsiadObj, TPsiadInitOpt  tPsiadInitOpt)
{
  TPsiadInfo *ptPsiadInfo=(TPsiadInfo *)malloc(sizeof(TPsiadInfo));
  
  if(ptPsiadInfo==NULL)
  {
    fprintf(stderr, __FILE__":%s:%d [psiad] Initializing TPsiadInfo object is failed !!\n", __func__, __LINE__);
    return S_FAIL;
  }
  
  memset(ptPsiadInfo, 0, sizeof(TPsiadInfo));
  *phPsiadObj = (HANDLE)(ptPsiadInfo);
  

  if(tPsiadInitOpt.szSckPath!=NULL)
  {
    if(Psiad_InitHTTPSock(&ptPsiadInfo->iHttpFdipcSck, tPsiadInitOpt.szSckPath)!=S_OK)
    {
	printf(__FILE__":%s:%d [psiad] fdipc_server_socket failed.\n", __func__, __LINE__);
	return S_FAIL;
    }
  }
  else
  {	
	printf(__FILE__":%s:%d [psiad] fdipc_server_socket failed. Because socket path is NULL\n", __func__, __LINE__);
	return S_FAIL;
  
  
  }
  // record application handle
   ptPsiadInfo->hAppObject = tPsiadInitOpt.hAppObject;
   ptPsiadInfo->bTerminate = FALSE;
   ptPsiadInfo->bReload = FALSE;
   
   
   
  ptPsiadInfo->pfnReloadFunc=tPsiadInitOpt.pfnReloadFunc;
  ptPsiadInfo->pfnRebootFunc=tPsiadInitOpt.pfnRebootFunc;
  ptPsiadInfo->pfnUpdateFirmwareFunc=tPsiadInitOpt.pfnUpdateFirmwareFunc;
  ptPsiadInfo->pfnGetConfigurationDataFunc=tPsiadInitOpt.pfnGetConfigurationDataFunc;
  ptPsiadInfo->pfnPutConfigurationDataFunc=tPsiadInitOpt.pfnPutConfigurationDataFunc;
  ptPsiadInfo->pfnFactoryResetFunc=tPsiadInitOpt.pfnFactoryResetFunc;
  ptPsiadInfo->pfnGetDeviceInfoFunc=tPsiadInitOpt.pfnGetDeviceInfoFunc;
  ptPsiadInfo->pfnPutDeviceInfoFunc=tPsiadInitOpt.pfnPutDeviceInfoFunc;
  ptPsiadInfo->pfnSupportReportFunc=tPsiadInitOpt.pfnSupportReportFunc;
  ptPsiadInfo->pfnGetDeviceStatusFunc=tPsiadInitOpt.pfnGetDeviceStatusFunc;
  ptPsiadInfo->pfnGetTimeFunc=tPsiadInitOpt.pfnGetTimeFunc;
  ptPsiadInfo->pfnPutTimeFunc=tPsiadInitOpt.pfnPutTimeFunc;
  ptPsiadInfo->pfnGetLocalTimeFunc=tPsiadInitOpt.pfnGetLocalTimeFunc;
  ptPsiadInfo->pfnPutLocalTimeFunc=tPsiadInitOpt.pfnPutLocalTimeFunc;
  ptPsiadInfo->pfnGetTimeZoneFunc=tPsiadInitOpt.pfnGetTimeZoneFunc;
  ptPsiadInfo->pfnPutTimeZoneFunc=tPsiadInitOpt.pfnPutTimeZoneFunc;
  ptPsiadInfo->pfnGetNTPServersFunc=tPsiadInitOpt.pfnGetNTPServersFunc;
  ptPsiadInfo->pfnPutNTPServersFunc=tPsiadInitOpt.pfnPutNTPServersFunc;
  ptPsiadInfo->pfnPostNTPServersFunc=tPsiadInitOpt.pfnPostNTPServersFunc;
  ptPsiadInfo->pfnDeleteNTPServersFunc=tPsiadInitOpt.pfnDeleteNTPServersFunc;
  ptPsiadInfo->pfnGetNTPServerFunc=tPsiadInitOpt.pfnGetNTPServerFunc;
  ptPsiadInfo->pfnPutNTPServerFunc=tPsiadInitOpt.pfnPutNTPServerFunc;
  ptPsiadInfo->pfnDeleteNTPServerFunc=tPsiadInitOpt.pfnDeleteNTPServerFunc;
  ptPsiadInfo->pfnGetLoggingFunc=tPsiadInitOpt.pfnGetLoggingFunc;
  ptPsiadInfo->pfnPutLoggingFunc=tPsiadInitOpt.pfnPutLoggingFunc;
  ptPsiadInfo->pfnGetLoggingMessagesFunc=tPsiadInitOpt.pfnGetLoggingMessagesFunc;
  ptPsiadInfo->pfnGetStorageVolumesFunc=tPsiadInitOpt.pfnGetStorageVolumesFunc;
  ptPsiadInfo->pfnGetStorageVolumeFunc=tPsiadInitOpt.pfnGetStorageVolumeFunc;
  ptPsiadInfo->pfnGetStorageVolumeStatusFunc=tPsiadInitOpt.pfnGetStorageVolumeStatusFunc;
  ptPsiadInfo->pfnFomatStorageVolumeFunc=tPsiadInitOpt.pfnFomatStorageVolumeFunc;
  ptPsiadInfo->pfnGetStorageVolumeFilesFunc=tPsiadInitOpt.pfnGetStorageVolumeFilesFunc;
  ptPsiadInfo->pfnDeleteStorageVolumeFilesFunc=tPsiadInitOpt.pfnDeleteStorageVolumeFilesFunc;
  ptPsiadInfo->pfnGetStorageVolumeFileFunc=tPsiadInitOpt.pfnGetStorageVolumeFileFunc;
  ptPsiadInfo->pfnDeleteStorageVolumeFileFunc=tPsiadInitOpt.pfnDeleteStorageVolumeFileFunc;
  ptPsiadInfo->pfnGetStorageVolumeDataFunc=tPsiadInitOpt.pfnGetStorageVolumeDataFunc;
  ptPsiadInfo->pfnGetNetworkInterfacesFunc=tPsiadInitOpt.pfnGetNetworkInterfacesFunc;
  ptPsiadInfo->pfnGetNetworkInterfaceFunc=tPsiadInitOpt.pfnGetNetworkInterfaceFunc;
  ptPsiadInfo->pfnPutNetworkInterfaceFunc=tPsiadInitOpt.pfnPutNetworkInterfaceFunc;
  ptPsiadInfo->pfnGetIPAddressFunc=tPsiadInitOpt.pfnGetIPAddressFunc;
  ptPsiadInfo->pfnPutIPAddressFunc=tPsiadInitOpt.pfnPutIPAddressFunc;
  ptPsiadInfo->pfnGetWirelessFunc=tPsiadInitOpt.pfnGetWirelessFunc;
  ptPsiadInfo->pfnPutWirelessFunc=tPsiadInitOpt.pfnPutWirelessFunc;
  ptPsiadInfo->pfnGetIEEE8021xFunc=tPsiadInitOpt.pfnGetIEEE8021xFunc;
  ptPsiadInfo->pfnPutIEEE8021xFunc=tPsiadInitOpt.pfnPutIEEE8021xFunc;
  ptPsiadInfo->pfnGetIPFilterFunc=tPsiadInitOpt.pfnGetIPFilterFunc;
  ptPsiadInfo->pfnPutIPFilterFunc=tPsiadInitOpt.pfnPutIPFilterFunc;
  ptPsiadInfo->pfnGetIPFilterAddressesFunc=tPsiadInitOpt.pfnGetIPFilterAddressesFunc;
  ptPsiadInfo->pfnPutIPFilterAddressesFunc=tPsiadInitOpt.pfnPutIPFilterAddressesFunc;
  ptPsiadInfo->pfnPostIPFilterAddressesFunc=tPsiadInitOpt.pfnPostIPFilterAddressesFunc;
  ptPsiadInfo->pfnDeleteIPFilterAddressesFunc=tPsiadInitOpt.pfnDeleteIPFilterAddressesFunc;
  ptPsiadInfo->pfnDeleteIPFilterFunc=tPsiadInitOpt.pfnDeleteIPFilterFunc;
  ptPsiadInfo->pfnGetSNMPFunc=tPsiadInitOpt.pfnGetSNMPFunc;
  ptPsiadInfo->pfnPutSNMPFunc=tPsiadInitOpt.pfnPutSNMPFunc;
  ptPsiadInfo->pfnGetSNMPV2cFunc=tPsiadInitOpt.pfnGetSNMPV2cFunc;
  ptPsiadInfo->pfnPutSNMPV2cFunc=tPsiadInitOpt.pfnPutSNMPV2cFunc;
  ptPsiadInfo->pfnGetSNMPTrapReceiversFunc=tPsiadInitOpt.pfnGetSNMPTrapReceiversFunc;
  ptPsiadInfo->pfnPutSNMPTrapReceiversFunc=tPsiadInitOpt.pfnPutSNMPTrapReceiversFunc;
  ptPsiadInfo->pfnPostSNMPTrapReceiversFunc=tPsiadInitOpt.pfnPostSNMPTrapReceiversFunc;
  ptPsiadInfo->pfnDeleteSNMPTrapReceiversFunc=tPsiadInitOpt.pfnDeleteSNMPTrapReceiversFunc;
  ptPsiadInfo->pfnGetSNMPTrapReceiverFunc=tPsiadInitOpt.pfnGetSNMPTrapReceiverFunc;
  ptPsiadInfo->pfnPutSNMPTrapReceiverFunc=tPsiadInitOpt.pfnPutSNMPTrapReceiverFunc;
  ptPsiadInfo->pfnDeleteSNMPTrapReceiverFunc=tPsiadInitOpt.pfnDeleteSNMPTrapReceiverFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedFunc=tPsiadInitOpt.pfnGetSNMPAdvancedFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedFunc=tPsiadInitOpt.pfnPutSNMPAdvancedFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedUsersFunc=tPsiadInitOpt.pfnGetSNMPAdvancedUsersFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedUsersFunc=tPsiadInitOpt.pfnPutSNMPAdvancedUsersFunc;
  ptPsiadInfo->pfnPostSNMPAdvancedUsersFunc=tPsiadInitOpt.pfnPostSNMPAdvancedUsersFunc;
  ptPsiadInfo->pfnDeleteSNMPAdvancedUsersFunc=tPsiadInitOpt.pfnDeleteSNMPAdvancedUsersFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedUserFunc=tPsiadInitOpt.pfnGetSNMPAdvancedUserFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedUserFunc=tPsiadInitOpt.pfnPutSNMPAdvancedUserFunc;
  ptPsiadInfo->pfnDeleteSNMPAdvancedUserFunc=tPsiadInitOpt.pfnDeleteSNMPAdvancedUserFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedNotificationFiltersFunc=tPsiadInitOpt.pfnGetSNMPAdvancedNotificationFiltersFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedNotificationFiltersFunc=tPsiadInitOpt.pfnPutSNMPAdvancedNotificationFiltersFunc;
  ptPsiadInfo->pfnPostSNMPAdvancedNotificationFiltersFunc=tPsiadInitOpt.pfnPostSNMPAdvancedNotificationFiltersFunc;
  ptPsiadInfo->pfnDeleteSNMPAdvancedNotificationFiltersFunc=tPsiadInitOpt.pfnDeleteSNMPAdvancedNotificationFiltersFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedNotificationFilterFunc=tPsiadInitOpt.pfnGetSNMPAdvancedNotificationFilterFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedNotificationFilterFunc=tPsiadInitOpt.pfnPutSNMPAdvancedNotificationFilterFunc;
  ptPsiadInfo->pfnDeleteSNMPAdvancedNotificationFilterFunc=tPsiadInitOpt.pfnDeleteSNMPAdvancedNotificationFilterFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedNotificationReceiversFunc=tPsiadInitOpt.pfnGetSNMPAdvancedNotificationReceiversFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedNotificationReceiversFunc=tPsiadInitOpt.pfnPutSNMPAdvancedNotificationReceiversFunc;
  ptPsiadInfo->pfnPostSNMPAdvancedNotificationReceiversFunc=tPsiadInitOpt.pfnPostSNMPAdvancedNotificationReceiversFunc;
  ptPsiadInfo->pfnDeleteSNMPAdvancedNotificationReceiversFunc=tPsiadInitOpt.pfnDeleteSNMPAdvancedNotificationReceiversFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedNotificationReceiverFunc=tPsiadInitOpt.pfnGetSNMPAdvancedNotificationReceiverFunc;
  ptPsiadInfo->pfnPutSNMPAdvancedNotificationReceiverFunc=tPsiadInitOpt.pfnPutSNMPAdvancedNotificationReceiverFunc;
  ptPsiadInfo->pfnDeleteSNMPAdvancedNotificationReceiverFunc=tPsiadInitOpt.pfnDeleteSNMPAdvancedNotificationReceiverFunc;
  ptPsiadInfo->pfnGetSNMPAdvancedV3Func=tPsiadInitOpt.pfnGetSNMPAdvancedV3Func;
  ptPsiadInfo->pfnPutSNMPAdvancedV3Func=tPsiadInitOpt.pfnPutSNMPAdvancedV3Func;
  ptPsiadInfo->pfnGetQoSFunc=tPsiadInitOpt.pfnGetQoSFunc;
  ptPsiadInfo->pfnPutQoSFunc=tPsiadInitOpt.pfnPutQoSFunc;
  ptPsiadInfo->pfnGetCoSesFunc=tPsiadInitOpt.pfnGetCoSesFunc;
  ptPsiadInfo->pfnPutCoSesFunc=tPsiadInitOpt.pfnPutCoSesFunc;
  ptPsiadInfo->pfnPostCoSesFunc=tPsiadInitOpt.pfnPostCoSesFunc;
  ptPsiadInfo->pfnDeleteCoSesFunc=tPsiadInitOpt.pfnDeleteCoSesFunc;
  ptPsiadInfo->pfnGetCoSFunc=tPsiadInitOpt.pfnGetCoSFunc;
  ptPsiadInfo->pfnPutCoSFunc=tPsiadInitOpt.pfnPutCoSFunc;
  ptPsiadInfo->pfnDeleteCoSFunc=tPsiadInitOpt.pfnDeleteCoSFunc;
  ptPsiadInfo->pfnGetDscpsFunc=tPsiadInitOpt.pfnGetDscpsFunc;
  ptPsiadInfo->pfnPutDscpsFunc=tPsiadInitOpt.pfnPutDscpsFunc;
  ptPsiadInfo->pfnPostDscpsFunc=tPsiadInitOpt.pfnPostDscpsFunc;
  ptPsiadInfo->pfnDeleteDscpsFunc=tPsiadInitOpt.pfnDeleteDscpsFunc;
  ptPsiadInfo->pfnGetDscpFunc=tPsiadInitOpt.pfnGetDscpFunc;
  ptPsiadInfo->pfnPutDscpFunc=tPsiadInitOpt.pfnPutDscpFunc;
  ptPsiadInfo->pfnDeleteDscpFunc=tPsiadInitOpt.pfnDeleteDscpFunc;
  ptPsiadInfo->pfnGetDiscoveryFunc=tPsiadInitOpt.pfnGetDiscoveryFunc;
  ptPsiadInfo->pfnPutDiscoveryFunc=tPsiadInitOpt.pfnPutDiscoveryFunc;
  ptPsiadInfo->pfnGetSyslogFunc=tPsiadInitOpt.pfnGetSyslogFunc;
  ptPsiadInfo->pfnPutSyslogFunc=tPsiadInitOpt.pfnPutSyslogFunc;
  ptPsiadInfo->pfnGetSyslogServersFunc=tPsiadInitOpt.pfnGetSyslogServersFunc;
  ptPsiadInfo->pfnPutSyslogServersFunc=tPsiadInitOpt.pfnPutSyslogServersFunc;
  ptPsiadInfo->pfnPostSyslogServersFunc=tPsiadInitOpt.pfnPostSyslogServersFunc;
  ptPsiadInfo->pfnDeleteSyslogServersFunc=tPsiadInitOpt.pfnDeleteSyslogServersFunc;
  ptPsiadInfo->pfnGetSyslogServerFunc=tPsiadInitOpt.pfnGetSyslogServerFunc;
  ptPsiadInfo->pfnPutSyslogServerFunc=tPsiadInitOpt.pfnPutSyslogServerFunc;
  ptPsiadInfo->pfnDeleteSyslogServerFunc=tPsiadInitOpt.pfnDeleteSyslogServerFunc;
  ptPsiadInfo->pfnGetIOPortsFunc=tPsiadInitOpt.pfnGetIOPortsFunc;
  ptPsiadInfo->pfnGetIOPortStatusesFunc=tPsiadInitOpt.pfnGetIOPortStatusesFunc;
  ptPsiadInfo->pfnGetInputPortsFunc=tPsiadInitOpt.pfnGetInputPortsFunc;
  ptPsiadInfo->pfnGetInputPortFunc=tPsiadInitOpt.pfnGetInputPortFunc;
  ptPsiadInfo->pfnPutInputPortFunc=tPsiadInitOpt.pfnPutInputPortFunc;
  ptPsiadInfo->pfnGetIOPortStatusFunc=tPsiadInitOpt.pfnGetIOPortStatusFunc;
  ptPsiadInfo->pfnGetOutputPortsFunc=tPsiadInitOpt.pfnGetOutputPortsFunc;
  ptPsiadInfo->pfnGetOutputPortFunc=tPsiadInitOpt.pfnGetOutputPortFunc;
  ptPsiadInfo->pfnPutOutputPortFunc=tPsiadInitOpt.pfnPutOutputPortFunc;
  ptPsiadInfo->pfnSetIOportTriggerFunc=tPsiadInitOpt.pfnSetIOportTriggerFunc;
  ptPsiadInfo->pfnGetAudioChannelsFunc=tPsiadInitOpt.pfnGetAudioChannelsFunc;
  ptPsiadInfo->pfnGetAudioChannelFunc=tPsiadInitOpt.pfnGetAudioChannelFunc;
  ptPsiadInfo->pfnPutAudioChannelFunc=tPsiadInitOpt.pfnPutAudioChannelFunc;
  ptPsiadInfo->pfnGetVideoOverlayImagesFunc=tPsiadInitOpt.pfnGetVideoOverlayImagesFunc;
  ptPsiadInfo->pfnPostVideoOverlayImagesFunc=tPsiadInitOpt.pfnPostVideoOverlayImagesFunc;
  ptPsiadInfo->pfnDeleteVideoOverlayImagesFunc=tPsiadInitOpt.pfnDeleteVideoOverlayImagesFunc;
  ptPsiadInfo->pfnGetVideoOverlayImageFunc=tPsiadInitOpt.pfnGetVideoOverlayImageFunc;
  ptPsiadInfo->pfnPutVideoOverlayImageFunc=tPsiadInitOpt.pfnPutVideoOverlayImageFunc;
  ptPsiadInfo->pfnDeleteVideoOverlayImageFunc=tPsiadInitOpt.pfnDeleteVideoOverlayImageFunc;
  ptPsiadInfo->pfnGetVideoInputFunc=tPsiadInitOpt.pfnGetVideoInputFunc;
  ptPsiadInfo->pfnGetVideoInputChannelsFunc=tPsiadInitOpt.pfnGetVideoInputChannelsFunc;
  ptPsiadInfo->pfnGetVideoInputChannelFunc=tPsiadInitOpt.pfnGetVideoInputChannelFunc;
  ptPsiadInfo->pfnPutVideoInputChannelFunc=tPsiadInitOpt.pfnPutVideoInputChannelFunc;
  ptPsiadInfo->pfnSetVideoFocusFunc=tPsiadInitOpt.pfnSetVideoFocusFunc;
  ptPsiadInfo->pfnSetVideoIRISFunc=tPsiadInitOpt.pfnSetVideoIRISFunc;
  ptPsiadInfo->pfnGetVideoLensFunc=tPsiadInitOpt.pfnGetVideoLensFunc;
  ptPsiadInfo->pfnGetVideoOverlayFunc=tPsiadInitOpt.pfnGetVideoOverlayFunc;
  ptPsiadInfo->pfnPutVideoOverlayFunc=tPsiadInitOpt.pfnPutVideoOverlayFunc;
  ptPsiadInfo->pfnDeleteVideoOverlayFunc=tPsiadInitOpt.pfnDeleteVideoOverlayFunc;
  ptPsiadInfo->pfnGetVideoTextOverlaysFunc=tPsiadInitOpt.pfnGetVideoTextOverlaysFunc;
  ptPsiadInfo->pfnPutVideoTextOverlaysFunc=tPsiadInitOpt.pfnPutVideoTextOverlaysFunc;
  ptPsiadInfo->pfnPostVideoTextOverlaysFunc=tPsiadInitOpt.pfnPostVideoTextOverlaysFunc;
  ptPsiadInfo->pfnDeleteVideoTextOverlaysFunc=tPsiadInitOpt.pfnDeleteVideoTextOverlaysFunc;
  ptPsiadInfo->pfnGetVideoTextOverlayFunc=tPsiadInitOpt.pfnGetVideoTextOverlayFunc;
  ptPsiadInfo->pfnPutVideoTextOverlayFunc=tPsiadInitOpt.pfnPutVideoTextOverlayFunc;
  ptPsiadInfo->pfnDeleteVideoTextOverlayFunc=tPsiadInitOpt.pfnDeleteVideoTextOverlayFunc;
  ptPsiadInfo->pfnGetVideoImageOverlaysFunc=tPsiadInitOpt.pfnGetVideoImageOverlaysFunc;
  ptPsiadInfo->pfnPutVideoImageOverlaysFunc=tPsiadInitOpt.pfnPutVideoImageOverlaysFunc;
  ptPsiadInfo->pfnPostVideoImageOverlaysFunc=tPsiadInitOpt.pfnPostVideoImageOverlaysFunc;
  ptPsiadInfo->pfnDeleteVideoImageOverlaysFunc=tPsiadInitOpt.pfnDeleteVideoImageOverlaysFunc;
  ptPsiadInfo->pfnGetVideoImageOverlayFunc=tPsiadInitOpt.pfnGetVideoImageOverlayFunc;
  ptPsiadInfo->pfnPutVideoImageOverlayFunc=tPsiadInitOpt.pfnPutVideoImageOverlayFunc;
  ptPsiadInfo->pfnDeleteVideoImageOverlayFunc=tPsiadInitOpt.pfnDeleteVideoImageOverlayFunc;
  ptPsiadInfo->pfnGetPrivacyMaskFunc=tPsiadInitOpt.pfnGetPrivacyMaskFunc;
  ptPsiadInfo->pfnPutPrivacyMaskFunc=tPsiadInitOpt.pfnPutPrivacyMaskFunc;
  ptPsiadInfo->pfnGetPrivacyMaskRegionsFunc=tPsiadInitOpt.pfnGetPrivacyMaskRegionsFunc;
  ptPsiadInfo->pfnPutPrivacyMaskRegionsFunc=tPsiadInitOpt.pfnPutPrivacyMaskRegionsFunc;
  ptPsiadInfo->pfnPostPrivacyMaskRegionsFunc=tPsiadInitOpt.pfnPostPrivacyMaskRegionsFunc;
  ptPsiadInfo->pfnDeletePrivacyMaskRegionsFunc=tPsiadInitOpt.pfnDeletePrivacyMaskRegionsFunc;
  ptPsiadInfo->pfnGetPrivacyMaskRegionFunc=tPsiadInitOpt.pfnGetPrivacyMaskRegionFunc;
  ptPsiadInfo->pfnPutPrivacyMaskRegionFunc=tPsiadInitOpt.pfnPutPrivacyMaskRegionFunc;
  ptPsiadInfo->pfnDeletePrivacyMaskRegionFunc=tPsiadInitOpt.pfnDeletePrivacyMaskRegionFunc;
  ptPsiadInfo->pfnGetSerialPortsFunc=tPsiadInitOpt.pfnGetSerialPortsFunc;
  ptPsiadInfo->pfnGetSerialPortFunc=tPsiadInitOpt.pfnGetSerialPortFunc;
  ptPsiadInfo->pfnPutSerialPortFunc=tPsiadInitOpt.pfnPutSerialPortFunc;
  ptPsiadInfo->pfnSendCommandToSerailPortFunc=tPsiadInitOpt.pfnSendCommandToSerailPortFunc;
  ptPsiadInfo->pfnGetDiagnosticsCommandsFunc=tPsiadInitOpt.pfnGetDiagnosticsCommandsFunc;
  ptPsiadInfo->pfnPostDiagnosticsCommandsFunc=tPsiadInitOpt.pfnPostDiagnosticsCommandsFunc;
  ptPsiadInfo->pfnDeleteDiagnosticsCommandsFunc=tPsiadInitOpt.pfnDeleteDiagnosticsCommandsFunc;
  ptPsiadInfo->pfnGetDiagnosticsCommandFunc=tPsiadInitOpt.pfnGetDiagnosticsCommandFunc;
  ptPsiadInfo->pfnDeleteDiagnosticsCommandFunc=tPsiadInitOpt.pfnDeleteDiagnosticsCommandFunc;
  ptPsiadInfo->pfnGetSRTPMasterKeyFunc=tPsiadInitOpt.pfnGetSRTPMasterKeyFunc;
  ptPsiadInfo->pfnPutSRTPMasterKeyFunc=tPsiadInitOpt.pfnPutSRTPMasterKeyFunc;
  ptPsiadInfo->pfnGetDeviceCertFunc=tPsiadInitOpt.pfnGetDeviceCertFunc;
  ptPsiadInfo->pfnPutDeviceCertFunc=tPsiadInitOpt.pfnPutDeviceCertFunc;
  ptPsiadInfo->pfnGetUsersFunc=tPsiadInitOpt.pfnGetUsersFunc;
  ptPsiadInfo->pfnPutUsersFunc=tPsiadInitOpt.pfnPutUsersFunc;
  ptPsiadInfo->pfnPostUsersFunc=tPsiadInitOpt.pfnPostUsersFunc;
  ptPsiadInfo->pfnDeleteUsersFunc=tPsiadInitOpt.pfnDeleteUsersFunc;
  ptPsiadInfo->pfnGetUserFunc=tPsiadInitOpt.pfnGetUserFunc;
  ptPsiadInfo->pfnPutUserFunc=tPsiadInitOpt.pfnPutUserFunc;
  ptPsiadInfo->pfnDeleteUserFunc=tPsiadInitOpt.pfnDeleteUserFunc;
  ptPsiadInfo->pfnGetAAACertFunc=tPsiadInitOpt.pfnGetAAACertFunc;
  ptPsiadInfo->pfnPutAAACertFunc=tPsiadInitOpt.pfnPutAAACertFunc;
  ptPsiadInfo->pfnGetAdminAccessesFunc=tPsiadInitOpt.pfnGetAdminAccessesFunc;
  ptPsiadInfo->pfnPutAdminAccessesFunc=tPsiadInitOpt.pfnPutAdminAccessesFunc;
  ptPsiadInfo->pfnPostAdminAccessesFunc=tPsiadInitOpt.pfnPostAdminAccessesFunc;
  ptPsiadInfo->pfnDeleteAdminAccessesFunc=tPsiadInitOpt.pfnDeleteAdminAccessesFunc;
  ptPsiadInfo->pfnGetAdminAccessFunc=tPsiadInitOpt.pfnGetAdminAccessFunc;
  ptPsiadInfo->pfnPutAdminAccessfunc=tPsiadInitOpt.pfnPutAdminAccessfunc;
  ptPsiadInfo->pfnDeleteAdminAccessFunc=tPsiadInitOpt.pfnDeleteAdminAccessFunc;
  ptPsiadInfo->pfnGetStreamingStatusFunc=tPsiadInitOpt.pfnGetStreamingStatusFunc;
  ptPsiadInfo->pfnGetStreamingChannelsFunc=tPsiadInitOpt.pfnGetStreamingChannelsFunc;
  ptPsiadInfo->pfnPutStreamingChannelsFunc=tPsiadInitOpt.pfnPutStreamingChannelsFunc;
  ptPsiadInfo->pfnPostStreamingChannelsFunc=tPsiadInitOpt.pfnPostStreamingChannelsFunc;
  ptPsiadInfo->pfnDeleteStreamingChannelsFunc=tPsiadInitOpt.pfnDeleteStreamingChannelsFunc;
  ptPsiadInfo->pfnGetStreamingChannelFunc=tPsiadInitOpt.pfnGetStreamingChannelFunc;
  ptPsiadInfo->pfnPutStreamingChannelFunc=tPsiadInitOpt.pfnPutStreamingChannelFunc;
  ptPsiadInfo->pfnDeleteStreamingChannelFunc=tPsiadInitOpt.pfnDeleteStreamingChannelFunc;
  ptPsiadInfo->pfnGetStreamingSessionStatusFunc=tPsiadInitOpt.pfnGetStreamingSessionStatusFunc;
  ptPsiadInfo->pfnGetPTZhannelsFunc=tPsiadInitOpt.pfnGetPTZhannelsFunc;
  ptPsiadInfo->pfnPutPTZhannelsFunc=tPsiadInitOpt.pfnPutPTZhannelsFunc;
  ptPsiadInfo->pfnPostPTZhannelsFunc=tPsiadInitOpt.pfnPostPTZhannelsFunc;
  ptPsiadInfo->pfnDeletePTZhannelsFunc=tPsiadInitOpt.pfnDeletePTZhannelsFunc;
  ptPsiadInfo->pfnGetPTZhannelFunc=tPsiadInitOpt.pfnGetPTZhannelFunc;
  ptPsiadInfo->pfnPutPTZhannelFunc=tPsiadInitOpt.pfnPutPTZhannelFunc;
  ptPsiadInfo->pfnDeletePTZhannelFunc=tPsiadInitOpt.pfnDeletePTZhannelFunc;
  ptPsiadInfo->pfnSetPTZHomePositionFunc=tPsiadInitOpt.pfnSetPTZHomePositionFunc;
  ptPsiadInfo->pfnSetPTZContinuousFunc=tPsiadInitOpt.pfnSetPTZContinuousFunc;
  ptPsiadInfo->pfnSetPTZMomentaryFunc=tPsiadInitOpt.pfnSetPTZMomentaryFunc;
  ptPsiadInfo->pfnSetPTZRelativeFunc=tPsiadInitOpt.pfnSetPTZRelativeFunc;
  ptPsiadInfo->pfnSetPTZAbsoluteFunc=tPsiadInitOpt.pfnSetPTZAbsoluteFunc;
  ptPsiadInfo->pfnSetPTZDigitalFunc=tPsiadInitOpt.pfnSetPTZDigitalFunc;
  ptPsiadInfo->pfnGetPTZStatusFunc=tPsiadInitOpt.pfnGetPTZStatusFunc;
  ptPsiadInfo->pfnGetPTZPresetsFunc=tPsiadInitOpt.pfnGetPTZPresetsFunc;
  ptPsiadInfo->pfnPutPTZPresetsFunc=tPsiadInitOpt.pfnPutPTZPresetsFunc;
  ptPsiadInfo->pfnPostPTZPresetsFunc=tPsiadInitOpt.pfnPostPTZPresetsFunc;
  ptPsiadInfo->pfnDeletePTZPresetsFunc=tPsiadInitOpt.pfnDeletePTZPresetsFunc;
  ptPsiadInfo->pfnGetPTZPresetFunc=tPsiadInitOpt.pfnGetPTZPresetFunc;
  ptPsiadInfo->pfnPutPTZPresetFunc=tPsiadInitOpt.pfnPutPTZPresetFunc;
  ptPsiadInfo->pfnDeletePTZPresetFunc=tPsiadInitOpt.pfnDeletePTZPresetFunc;
  ptPsiadInfo->pfnGotoPresetPositionFunc=tPsiadInitOpt.pfnGotoPresetPositionFunc;
  ptPsiadInfo->pfnGetPTZPatrolsFunc=tPsiadInitOpt.pfnGetPTZPatrolsFunc;
  ptPsiadInfo->pfnPutPTZPatrolsFunc=tPsiadInitOpt.pfnPutPTZPatrolsFunc;
  ptPsiadInfo->pfnPostPTZPatrolsFunc=tPsiadInitOpt.pfnPostPTZPatrolsFunc;
  ptPsiadInfo->pfnDeletePTZPatrolsFunc=tPsiadInitOpt.pfnDeletePTZPatrolsFunc;
  ptPsiadInfo->pfnGetPTZPatrolStatusesFunc=tPsiadInitOpt.pfnGetPTZPatrolStatusesFunc;
  ptPsiadInfo->pfnGetPTZPatrolFunc=tPsiadInitOpt.pfnGetPTZPatrolFunc;
  ptPsiadInfo->pfnPutPTZPatrolFunc=tPsiadInitOpt.pfnPutPTZPatrolFunc;
  ptPsiadInfo->pfnDeletePTZPatrolFunc=tPsiadInitOpt.pfnDeletePTZPatrolFunc;
  ptPsiadInfo->pfnStartPTZPatrolFunc=tPsiadInitOpt.pfnStartPTZPatrolFunc;
  ptPsiadInfo->pfnStopPTZPatrolFunc=tPsiadInitOpt.pfnStopPTZPatrolFunc;
  ptPsiadInfo->pfnPausePTZPatrolFunc=tPsiadInitOpt.pfnPausePTZPatrolFunc;
  ptPsiadInfo->pfnGetPTZPatrolStatusFunc=tPsiadInitOpt.pfnGetPTZPatrolStatusFunc;
  ptPsiadInfo->pfnGetPTZPatrolScheduleFunc=tPsiadInitOpt.pfnGetPTZPatrolScheduleFunc;
  ptPsiadInfo->pfnPutPTZPatrolScheduleFunc=tPsiadInitOpt.pfnPutPTZPatrolScheduleFunc;
  ptPsiadInfo->pfnGetMotionDetctionsFunc=tPsiadInitOpt.pfnGetMotionDetctionsFunc;
  ptPsiadInfo->pfnGetMotionDetctionFunc=tPsiadInitOpt.pfnGetMotionDetctionFunc;
  ptPsiadInfo->pfnPutMotionDetctionFunc=tPsiadInitOpt.pfnPutMotionDetctionFunc;
  ptPsiadInfo->pfnGetMotionDetectionRegionsFunc=tPsiadInitOpt.pfnGetMotionDetectionRegionsFunc;
  ptPsiadInfo->pfnPutMotionDetectionRegionsFunc=tPsiadInitOpt.pfnPutMotionDetectionRegionsFunc;
  ptPsiadInfo->pfnPostMotionDetectionRegionsFunc=tPsiadInitOpt.pfnPostMotionDetectionRegionsFunc;
  ptPsiadInfo->pfnDeleteMotionDetectionRegionsFunc=tPsiadInitOpt.pfnDeleteMotionDetectionRegionsFunc;
  ptPsiadInfo->pfnGetMotionDetectionRegionFunc=tPsiadInitOpt.pfnGetMotionDetectionRegionFunc;
  ptPsiadInfo->pfnPutMotionDetectionRegionFunc=tPsiadInitOpt.pfnPutMotionDetectionRegionFunc;
  ptPsiadInfo->pfnDeleteMotionDetectionRegionFunc=tPsiadInitOpt.pfnDeleteMotionDetectionRegionFunc;
  ptPsiadInfo->pfnGetEventNotificationFunc=tPsiadInitOpt.pfnGetEventNotificationFunc;
  ptPsiadInfo->pfnPutEventNotificationFunc=tPsiadInitOpt.pfnPutEventNotificationFunc;
  ptPsiadInfo->pfnGetEventTriggersFunc=tPsiadInitOpt.pfnGetEventTriggersFunc;
  ptPsiadInfo->pfnPutEventTriggersFunc=tPsiadInitOpt.pfnPutEventTriggersFunc;
  ptPsiadInfo->pfnPostEventTriggersFunc=tPsiadInitOpt.pfnPostEventTriggersFunc;
  ptPsiadInfo->pfnDeleteEventTriggersFunc=tPsiadInitOpt.pfnDeleteEventTriggersFunc;
  ptPsiadInfo->pfnGetEventTriggerFunc=tPsiadInitOpt.pfnGetEventTriggerFunc;
  ptPsiadInfo->pfnPutEventTriggerFunc=tPsiadInitOpt.pfnPutEventTriggerFunc;
  ptPsiadInfo->pfnDeleteEventTriggerFunc=tPsiadInitOpt.pfnDeleteEventTriggerFunc;
  ptPsiadInfo->pfnGetEventTriggerNontificationsFunc=tPsiadInitOpt.pfnGetEventTriggerNontificationsFunc;
  ptPsiadInfo->pfnPutEventTriggerNontificationsFunc=tPsiadInitOpt.pfnPutEventTriggerNontificationsFunc;
  ptPsiadInfo->pfnPostEventTriggerNontificationsFunc=tPsiadInitOpt.pfnPostEventTriggerNontificationsFunc;
  ptPsiadInfo->pfnDeleteEventTriggerNontificationsFunc=tPsiadInitOpt.pfnDeleteEventTriggerNontificationsFunc;
  ptPsiadInfo->pfnGetEventTriggerNontificationFunc=tPsiadInitOpt.pfnGetEventTriggerNontificationFunc;
  ptPsiadInfo->pfnPutEventTriggerNontificationFunc=tPsiadInitOpt.pfnPutEventTriggerNontificationFunc;
  ptPsiadInfo->pfnDeleteEventTriggerNontificationFunc=tPsiadInitOpt.pfnDeleteEventTriggerNontificationFunc;
  ptPsiadInfo->pfnGetEventScheduleFunc=tPsiadInitOpt.pfnGetEventScheduleFunc;
  ptPsiadInfo->pfnPutEventScheduleFunc=tPsiadInitOpt.pfnPutEventScheduleFunc;
  ptPsiadInfo->pfnGetEventNotificationMethodsFunc=tPsiadInitOpt.pfnGetEventNotificationMethodsFunc;
  ptPsiadInfo->pfnPutEventNotificationMethodsFunc=tPsiadInitOpt.pfnPutEventNotificationMethodsFunc;
  ptPsiadInfo->pfnGetMailingNontificationsFunc=tPsiadInitOpt.pfnGetMailingNontificationsFunc;
  ptPsiadInfo->pfnPutMailingNontificationsFunc=tPsiadInitOpt.pfnPutMailingNontificationsFunc;
  ptPsiadInfo->pfnPostMailingNontificationsFunc=tPsiadInitOpt.pfnPostMailingNontificationsFunc;
  ptPsiadInfo->pfnDeleteMailingNontificationsFunc=tPsiadInitOpt.pfnDeleteMailingNontificationsFunc;
  ptPsiadInfo->pfnGetMailingNontificationFunc=tPsiadInitOpt.pfnGetMailingNontificationFunc;
  ptPsiadInfo->pfnPutMailingNontificationFunc=tPsiadInitOpt.pfnPutMailingNontificationFunc;
  ptPsiadInfo->pfnDeleteMailingNontificationFunc=tPsiadInitOpt.pfnDeleteMailingNontificationFunc;
  ptPsiadInfo->pfnGetFTPNontificationsFunc=tPsiadInitOpt.pfnGetFTPNontificationsFunc;
  ptPsiadInfo->pfnPutFTPNontificationsFunc=tPsiadInitOpt.pfnPutFTPNontificationsFunc;
  ptPsiadInfo->pfnPostFTPNontificationsFunc=tPsiadInitOpt.pfnPostFTPNontificationsFunc;
  ptPsiadInfo->pfnDeleteFTPNontificationsFunc=tPsiadInitOpt.pfnDeleteFTPNontificationsFunc;
  ptPsiadInfo->pfnGetFTPNontificationFunc=tPsiadInitOpt.pfnGetFTPNontificationFunc;
  ptPsiadInfo->pfnPutFTPNontificationFunc=tPsiadInitOpt.pfnPutFTPNontificationFunc;
  ptPsiadInfo->pfnDeleteFTPNontificationFunc=tPsiadInitOpt.pfnDeleteFTPNontificationFunc;
  ptPsiadInfo->pfnGetHTTPHostNontificationsFunc=tPsiadInitOpt.pfnGetHTTPHostNontificationsFunc;
  ptPsiadInfo->pfnPutHTTPHostNontificationsFunc=tPsiadInitOpt.pfnPutHTTPHostNontificationsFunc;
  ptPsiadInfo->pfnPostHTTPHostNontificationsFunc=tPsiadInitOpt.pfnPostHTTPHostNontificationsFunc;
  ptPsiadInfo->pfnDeleteHTTHostPNontificationsFunc=tPsiadInitOpt.pfnDeleteHTTHostPNontificationsFunc;
  ptPsiadInfo->pfnGetHTTPHostNontificationFunc=tPsiadInitOpt.pfnGetHTTPHostNontificationFunc;
  ptPsiadInfo->pfnPutHTTPHostNontificationFunc=tPsiadInitOpt.pfnPutHTTPHostNontificationFunc;
  ptPsiadInfo->pfnDeleteHTTHostPNontificationFunc=tPsiadInitOpt.pfnDeleteHTTHostPNontificationFunc;
  ptPsiadInfo->pfnGetEventNotificationAlertFunc=tPsiadInitOpt.pfnGetEventNotificationAlertFunc;
  ptPsiadInfo->pfnPostEventNotificationAlertFunc=tPsiadInitOpt.pfnPostEventNotificationAlertFunc;


  ptPsiadInfo->pfnGetRootServiceIndexFunc=tPsiadInitOpt.pfnGetRootServiceIndexFunc;
  ptPsiadInfo->pfnGetRootServiceIndexrFunc=tPsiadInitOpt.pfnGetRootServiceIndexrFunc;
  ptPsiadInfo->pfnGetRootServiceDescriptionFunc=tPsiadInitOpt.pfnGetRootServiceDescriptionFunc;
  ptPsiadInfo->pfnGetSystemIndexFunc=tPsiadInitOpt.pfnGetSystemIndexFunc;
  ptPsiadInfo->pfnGetSystemDescriptionFunc=tPsiadInitOpt.pfnGetSystemDescriptionFunc;
  ptPsiadInfo->pfnGetSystemNetworkIndexFunc=tPsiadInitOpt.pfnGetSystemNetworkIndexFunc;
  ptPsiadInfo->pfnGetSystemNetworkDescriptionFunc=tPsiadInitOpt.pfnGetSystemNetworkDescriptionFunc;
  ptPsiadInfo->pfnGetSystemIOIndexFunc=tPsiadInitOpt.pfnGetSystemIOIndexFunc;
 ptPsiadInfo->pfnGetSystemIODescriptionFunc=tPsiadInitOpt.pfnGetSystemIODescriptionFunc;


  ptPsiadInfo->pfnGetSystemAudioIndexFunc=tPsiadInitOpt.pfnGetSystemAudioIndexFunc;
  ptPsiadInfo->pfnGetSystemAudioDescriptionFunc=tPsiadInitOpt.pfnGetSystemAudioDescriptionFunc;

  ptPsiadInfo->pfnGetSystemVideoIndexFunc=tPsiadInitOpt.pfnGetSystemVideoIndexFunc;
   ptPsiadInfo->pfnGetSystemVideoDescriptionFunc=tPsiadInitOpt.pfnGetSystemVideoDescriptionFunc;
 ptPsiadInfo->pfnGetStreamingIndexFunc=tPsiadInitOpt.pfnGetStreamingIndexFunc;
 ptPsiadInfo->pfnGetStreamingDescriptionFunc=tPsiadInitOpt.pfnGetStreamingDescriptionFunc;
  ptPsiadInfo->pfnGetPTZIndexFunc=tPsiadInitOpt.pfnGetPTZIndexFunc;
  ptPsiadInfo->pfnGetPTZDescriptionFunc=tPsiadInitOpt.pfnGetPTZDescriptionFunc;
  ptPsiadInfo->pfnGetCustomMotionDetectionIndexFunc=tPsiadInitOpt.pfnGetCustomMotionDetectionIndexFunc;
  ptPsiadInfo->pfnGetCustomMotionDetectionDescriptionFunc=tPsiadInitOpt.pfnGetCustomMotionDetectionDescriptionFunc;
 ptPsiadInfo->pfnGetSecurityIndexFunc=tPsiadInitOpt.pfnGetSecurityIndexFunc;
 ptPsiadInfo->pfnGetSecurityDescriptionFunc=tPsiadInitOpt.pfnGetSecurityDescriptionFunc;
  ptPsiadInfo->pfnGetSecurityAAAIndexFunc=tPsiadInitOpt.pfnGetSecurityAAAIndexFunc;
  ptPsiadInfo->pfnGetSecurityAAADescriptionFunc=tPsiadInitOpt.pfnGetSecurityAAADescriptionFunc;
  
  return S_OK;
}