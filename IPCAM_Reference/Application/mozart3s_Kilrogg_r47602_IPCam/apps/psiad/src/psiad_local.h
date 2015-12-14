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

#ifndef _PSIAD_LOCAL_H_
#define _PSIAD_LOCAL_H_

/* ============================================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "xmlwrapper.h"

#include "fdipc.h"
#include "psiad.h"
#include "psiad_types.h"

#include "dbgdefs.h"

#define MAX_BUFFER      1024*1024*1
#define MAX_LIST_NUM    16
#define MAX_URI_LEN	256
#define HTTP_HEADER_SIZE      	256
#define DEFAULT_SOCKET_PATH		"/var/run/psiad.sck"
#define ISO_8601_DATE_TIME_STR_LEN 64

#define MSG_XML_DECLARATION 	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define PSIA_NAMESPACE_STR   "urn:psialliance-org"
//! output message structure
typedef struct psia_out_message_info {
	EPsiaMsgProcessResult	eProcResult;
	EPsiaStatusCodeIndex eStatusCodeIndex;
	CHAR	szHTTPHeader[HTTP_HEADER_SIZE];
	BYTE    *pbyReturnResult;
	DWORD	 dwReturnResultLen;	
	//CHAR	szXMLOutMsg[MAX_BUFFER];//used for saving the XML data
	//BYTE	*byUserData;//used for saving the data except XML
	//DWORD	dwUserDataLen;
} TPsiaOutputMsgInfo;


typedef struct psia_input_message_info {
	CHAR 	szURI[MAX_URI_LEN];
	CHAR    szQueryString[MAX_BUFFER];
	BYTE  	*pbyInboundData;
	DWORD 	dwInboundDataLen;
	
	//CHAR	szXMLInMsg[MAX_BUFFER];//used for saving the XML data
	//BYTE	*byUserData;//used for saving the binary data
	//DWORD   dwUserDataLen;
} TPsiaInputMsgInfo;

//! enumeration for message
typedef enum psia_message_status {
	emsg_reboot = 0,
	emsg_updateFirmware=1,
	emsg_getConfigurateData=2,
	emsg_putConfigurateData=3,
	emsg_factoryReset=4,
	emsg_getDeviceInfo=5,
	emsg_putDeviceInfo,
	emsg_getSupportReport,
	emsg_getDeviceStatus,
	emsg_getTime,
	emsg_putTime,
	emsg_getLocalTime,
	emsg_putLocalTime,
	emsg_getTimeZone,
	emsg_putTimeZone,
	emsg_getNTPServers,
	emsg_putNTPServers,
	emsg_postNTPServers,
	emsg_deleteNTPServers,
	emsg_getNTPServer,
	emsg_putNTPServer,
	emsg_deleteNTPServer,
	emsg_getLogging,
	emsg_putLogging,
	emsg_getLoggingMessages,
	emsg_getStorageVolumes,
	emsg_getStorageVolume,
	emsg_getStorageVolumeStatus,
	emsg_fomatStorageVolume,
	emsg_getStorageVolumeFiles,
	emsg_deleteStorageVolumeFiles,
	emsg_getStorageVolumeFile,
	emsg_deleteStorageVolumeFile,
	emsg_getStorageVolumeData,
	emsg_getNetworkInterfaces,
	emsg_getNetworkInterface,
	emsg_putNetworkInterface,
	emsg_getIPAddress,
	emsg_putIPAddress,
	emsg_getWireless,
	emsg_putWireless,
	emsg_getIEEE8021x,
	emsg_putIEEE8021x,
	emsg_getIPFilter,
	emsg_putIPFilter,
	emsg_getFilterAddresses,
	emsg_putFilterAddresses,
	emsg_postFilterAddresses,
	emsg_deleteFilterAddresses,
	emsg_getFilterAddress,
	emsg_putFilterAddress,
	emsg_deleteFilterAddress,
	emsg_getSnmp,
	emsg_putSnmp,
	emsg_getSnmpv2c,
	emsg_putSnmpv2c,
	emsg_getSnmpv2cTrapReceivers,
	emsg_putSnmpv2cTrapReceivers,
	emsg_postSnmpv2cTrapReceivers,
	emsg_deleteSnmpv2cTrapReceivers,
	emsg_getSnmpv2cTrapReceiver,
	emsg_putSnmpv2cTrapReceiver,
	emsg_deleteSnmpv2cTrapReceiver,
	emsg_getSnmpAdvanced,
	emsg_putSnmpAdvanced,
	emsg_getSnmpAdvancedUsers,
	emsg_putSnmpAdvancedUsers,
	emsg_postSnmpAdvancedUsers,
	emsg_deleteSnmpAdvancedUsers,
	emsg_getSnmpAdvancedUser,
	emsg_putSnmpAdvancedUser,
	emsg_deleteSnmpAdvancedUser,
	emsg_getSnmpAdvancedNotificationFilters,
	emsg_putSnmpAdvancedNotificationFilters,
	emsg_postSnmpAdvancedNotificationFilters,
	emsg_deleteSnmpAdvancedNotificationFilters,
	emsg_getSnmpAdvancedNotificationFilter,
	emsg_putSnmpAdvancedNotificationFilter,
	emsg_deleteSnmpAdvancedNotificationFilter,
	emsg_getSnmpAdvancedNotificationReceivers,
	emsg_putSnmpAdvancedNotificationReceivers,
	emsg_postSnmpAdvancedNotificationReceivers,
	emsg_deleteSnmpAdvancedNotificationReceivers,
	emsg_getSnmpAdvancedNotificationReceiver,
	emsg_putSnmpAdvancedNotificationReceiver,
	emsg_deleteSnmpAdvancedNotificationReceiver,
	emsg_getSnmpAdvancedv3,
	emsg_putSnmpAdvancedv3,
	emsg_getQos,
	emsg_putQos,
	emsg_getCoses,
	emsg_putCoses,
	emsg_postCoses,
	emsg_deleteCoses,
	emsg_getCos,
	emsg_putCos,
	emsg_deleteCos,
	emsg_getDscps,
	emsg_putDscps,
	emsg_postDscps,
	emsg_deleteDscps,
	emsg_getDscp,
	emsg_putDscp,
	emsg_deleteDscp,
	emsg_getDiscovery,
	emsg_putDiscovery,
	emsg_getSyslog,
	emsg_putSyslog,
	emsg_getSyslogServers,
	emsg_putSyslogServers,
	emsg_postSyslogServers,
	emsg_deleteSyslogServers,
	emsg_getSyslogServer,
	emsg_putSyslogServer,
	emsg_deleteSyslogServer,
	emsg_getIOPorts,
	emsg_getIOPortStatuses,
	emsg_getInputPorts,
	emsg_getInputPort,
	emsg_putInputPort,
	emsg_getIOPortStatus,
	emsg_getOutputPorts,
	emsg_getOutputPort,
	emsg_putOutputPort,
	emsg_setIOportTrigger,
	emsg_getAudioChannels,
	emsg_getAudioChannel,
	emsg_putAudioChannel,
	emsg_getVideoOverlayImages,
	emsg_postVideoOverlayImages,
	emsg_deleteVideoOverlayImages,
	emsg_getVideoOverlayImage,
	emsg_putVideoOverlayImage,
	emsg_deleteVideoOverlayImage,
	emsg_getVideoInput,
	emsg_getVideoInputChannels,
	emsg_getVideoInputChannel,
	emsg_putVideoInputChannel,
	emsg_setVideoFocus,
	emsg_setVideoIRIS,
	emsg_getVideoLens,
	emsg_getVideoOverlay,
	emsg_putVideoOverlay,
	emsg_deleteVideoOverlay,
	emsg_getVideoTextOverlays,
	emsg_putVideoTextOverlays,
	emsg_postVideoTextOverlays,
	emsg_deleteVideoTextOverlays,
	emsg_getVideoTextOverlay,
	emsg_putVideoTextOverlay,
	emsg_deleteVideoTextOverlay,
	emsg_getVideoImageOverlays,
	emsg_putVideoImageOverlays,
	emsg_postVideoImageOverlays,
	emsg_deleteVideoImageOverlays,
	emsg_getVideoImageOverlay,
	emsg_putVideoImageOverlay,
	emsg_deleteVideoImageOverlay,
	emsg_getVideoPrivacyMask,
	emsg_putVideoPrivacyMask,
	emsg_getVideoPrivacyMaskRegions,
	emsg_putVideoPrivacyMaskRegions,
	emsg_postVideoPrivacyMaskRegions,
	emsg_deleteVideoPrivacyMaskRegions,
	emsg_getVideoPrivacyMaskRegion,
	emsg_putVideoPrivacyMaskRegion,
	emsg_deleteVideoPrivacyMaskRegion,
	emsg_getSerialPorts,
	emsg_getSerialPort,
	emsg_putSerialPort,
	emsg_sendCommandToSerailPort,
	emsg_getDiagnosticsCommands,
	emsg_postDiagnosticsCommands,
	emsg_deleteDiagnosticsCommands,
	emsg_getDiagnosticsCommand,
	emsg_deleteDiagnosticsCommand,
	emsg_getSRTPMasterKey,
	emsg_putSRTPMasterKey,
	emsg_getDeviceCert,
	emsg_putDeviceCert,
	emsg_getUsers,
	emsg_putUsers,
	emsg_postUsers,
	emsg_deleteUsers,
	emsg_getUser,
	emsg_putUser,
	emsg_deleteUser,
	emsg_getAAACert,
	emsg_putAAACert,
	emsg_getAdminAccesses,
	emsg_putAdminAccesses,
	emsg_postAdminAccesses,
	emsg_deleteAdminAccesses,
	emsg_getAdminAccess,
	emsg_putAdminAccess,
	emsg_deleteAdminAccess,
	emsg_getStreamingStatus,
	emsg_getStreamingChannels,
	emsg_putStreamingChannels,
	emsg_postStreamingChannels,
	emsg_deleteStreamingChannels,
	emsg_getStreamingChannel,
	emsg_putStreamingChannel,
	emsg_deleteStreamingChannel,
	emsg_getStreamingSessionStatus,
	emsg_getStreaming,
	emsg_postStreaming,
	emsg_getSnapshot,
	emsg_postSnapshot,
	emsg_requstKeyFrame,
	emsg_getPTZhannels,
	emsg_putPTZChannels,
	emsg_postPTZChannels,
	emsg_deletePTZChannels,
	emsg_getPTZhannel,
	emsg_putPTZChannel,
	emsg_deletePTZChannel,
	emsg_setPTZHomePosition,
	emsg_setPTZContinuous,
	emsg_setPTZMomentary,
	emsg_setPTZRelative,
	emsg_setPTZAbsolute,
	emsg_setPTZDigital,
	emsg_getPTZStatus,
	emsg_getPTZPresets,
	emsg_putPTZPresets,
	emsg_postPTZPresets,
	emsg_deletePTZPresets,
	emsg_getPTZPreset,
	emsg_putPTZPreset,
	emsg_deletePTZPreset,
	emsg_gotoPresetPosition,
	emsg_getPTZPatrols,
	emsg_putPTZPatrols,
	emsg_postPTZPatrols,
	emsg_deletePTZPatrols,
	emsg_getPTZPatrolStatuses,
	emsg_getPTZPatrol,
	emsg_putPTZPatrol,
	emsg_deletePTZPatrol,
	emsg_startPTZPatrol,
	emsg_stopPTZPatrol,
	emsg_pausePTZPatrol,
	emsg_getPTZPatrolStatus,
	emsg_getPTZPatrolSchedule,
	emsg_putPTZPatrolSchedule,
	emsg_getMotionDetctions,
	emsg_getMotionDetction,
	emsg_putMotionDetction,
	emsg_getMotionDetectionRegions,
	emsg_putMotionDetectionRegions,
	emsg_postMotionDetectionRegions,
	emsg_deleteMotionDetectionRegions,
	emsg_getMotionDetectionRegion,
	emsg_putMotionDetectionRegion,
	emsg_deleteMotionDetectionRegion,
	emsg_getEventNotification,
	emsg_putEventNotification,
	emsg_getEventTriggers,
	emsg_putEventTriggers,
	emsg_postEventTriggers,
	emsg_deleteEventTriggers,
	emsg_getEventTrigger,
	emsg_putEventTrigger,
	emsg_deleteEventTrigger,
	emsg_getEventTriggerNontifications,
	emsg_putEventTriggerNontifications,
	emsg_postEventTriggerNontifications,
	emsg_deleteEventTriggerNontifications,
	emsg_getEventTriggerNontification,
	emsg_putEventTriggerNontification,
	emsg_deleteEventTriggerNontification,
	emsg_getEventSchedule,
	emsg_putEventSchedule,
	emsg_getEventNotificationMethods,
	emsg_putEventNotificationMethods,
	emsg_getMailingNontifications,
	emsg_putMailingNontifications,
	emsg_postMailingNontifications,
	emsg_deleteMailingNontifications,
	emsg_getMailingNontification,
	emsg_putMailingNontification,
	emsg_deleteMailingNontification,
	emsg_getFTPNontifications,
	emsg_putFTPNontifications,
	emsg_postFTPNontifications,
	emsg_deleteFTPNontifications,
	emsg_getFTPNontification,
	emsg_putFTPNontification,
	emsg_deleteFTPNontification,
	emsg_getHTTPHostNontifications,
	emsg_putHTTPHostNontifications,
	emsg_postHTTPHostNontifications,
	emsg_deleteHTTPHostNontifications,
	emsg_getHTTPHostNontification,
	emsg_putHTTPHostNontification,
	emsg_deleteHTTPHostNontification,
	emsg_getEventNotificationAlert,
	emsg_getRootServiceIndex,
	emsg_getRootServiceIndexr,
	emsg_getRootServiceDescription,
	emsg_getSystemIndex,
	emsg_getSystemDescription,
	emsg_getSystemNetworkIndex,
	emsg_getSystemNetworkDescription,
	emsg_getSystemIOIndex,
	emsg_getSystemIODescription,
	emsg_getSystemAudioIndex,
	emsg_getSystemAudioDescription,
	emsg_getSystemVideoIndex,
	emsg_getSystemVideoDescription,
	emsg_getStreamingIndex,
	emsg_getStreamingDescription,
	emsg_getPTZIndex,
	emsg_getPTZDescription,
	emsg_getCustomMotionDetectionIndex,
	emsg_getCustomMotionDetectionDescription,
	emsg_getSecurityIndex,
	emsg_getSecurityDescription,
	emsg_getSecurityAAAIndex,
	emsg_getSecurityAAADescription,
	emsg_unknown
} EPsiaMsgStatus;

typedef struct psia_message_info {
	EPsiaMsgStatus	eMsgStatus;
} TPsiaMsgInfo;




typedef struct psiad_info
{
    //! socket to receive socket from Boa by fdipc
    int		iHttpFdipcSck;
    //! socket path
    CHAR	*szSckPath;
    //! handle of xmlwrapper, to check incoming message
    HANDLE	hMsgXMLWrap;

  //! control while(1) to stop
    BOOL	bTerminate;
    //! control while(1) to reload
    BOOL	bReload;

    //! record the application instance
    HANDLE	hAppObject;
    
   EPsiaMsgStatus eCurrentProcMsg;
   FReload pfnReloadFunc;
   FReboot pfnRebootFunc;
  FUpdateFirmware pfnUpdateFirmwareFunc;
  FGetConfigurationData pfnGetConfigurationDataFunc;
  FPutConfigurationData pfnPutConfigurationDataFunc;
  FFactoryReset pfnFactoryResetFunc;
  FGetDeviceInfo pfnGetDeviceInfoFunc;
  FPutDeviceInfo pfnPutDeviceInfoFunc;
  FSupportReport pfnSupportReportFunc;
  FGetDeviceStatus pfnGetDeviceStatusFunc;
  FGetTime pfnGetTimeFunc;
  FPutTime pfnPutTimeFunc;
  FGetLocalTime pfnGetLocalTimeFunc;
  FPutLocalTime pfnPutLocalTimeFunc;
  FGetTimeZone pfnGetTimeZoneFunc;
  FPutTimeZone pfnPutTimeZoneFunc;
  FGetNTPServers pfnGetNTPServersFunc;
  FPutNTPServers pfnPutNTPServersFunc;
  FPostNTPServers pfnPostNTPServersFunc;
  FDeleteNTPServers pfnDeleteNTPServersFunc;
  FGetNTPServer pfnGetNTPServerFunc;
  FPutNTPServer pfnPutNTPServerFunc;
  FDeleteNTPServer pfnDeleteNTPServerFunc;
  FGetLogging pfnGetLoggingFunc;
  FPutLogging pfnPutLoggingFunc;
  FGetLoggingMessages pfnGetLoggingMessagesFunc;
  FGetStorageVolumes pfnGetStorageVolumesFunc;
  FGetStorageVolume pfnGetStorageVolumeFunc;
  FGetStorageVolumeStatus pfnGetStorageVolumeStatusFunc;
  FFomatStorageVolume pfnFomatStorageVolumeFunc;
  FGetStorageVolumeFiles pfnGetStorageVolumeFilesFunc;
  FDeleteStorageVolumeFiles pfnDeleteStorageVolumeFilesFunc;
  FGetStorageVolumeFile pfnGetStorageVolumeFileFunc;
  FDeleteStorageVolumeFile pfnDeleteStorageVolumeFileFunc;
  FGetStorageVolumeData pfnGetStorageVolumeDataFunc;
  FGetNetworkInterfaces pfnGetNetworkInterfacesFunc;
  FGetNetworkInterface pfnGetNetworkInterfaceFunc;
  FPutNetworkInterface pfnPutNetworkInterfaceFunc;
  FGetIPAddress pfnGetIPAddressFunc;
  FPutIPAddress pfnPutIPAddressFunc;
  FGetWireless pfnGetWirelessFunc;
  FPutWireless pfnPutWirelessFunc;
  FGetIEEE8021x pfnGetIEEE8021xFunc;
  FPutIEEE8021x pfnPutIEEE8021xFunc;
  FGetIPFilter pfnGetIPFilterFunc;
  FPutIPFilter pfnPutIPFilterFunc;
  FGetIPFilterAddresses pfnGetIPFilterAddressesFunc;
  FPutIPFilterAddresses pfnPutIPFilterAddressesFunc;
  FPostIPFilterAddresses pfnPostIPFilterAddressesFunc;
  FDeleteIPFilterAddresses pfnDeleteIPFilterAddressesFunc;
  FDeleteIPFilter pfnDeleteIPFilterFunc;
  FGetSNMP pfnGetSNMPFunc;
  FPutSNMP pfnPutSNMPFunc;
  FGetSNMPV2c pfnGetSNMPV2cFunc;
  FPutSNMPV2c pfnPutSNMPV2cFunc;
  FGetSNMPTrapReceivers pfnGetSNMPTrapReceiversFunc;
  FPutSNMPTrapReceivers pfnPutSNMPTrapReceiversFunc;
  FPostSNMPTrapReceivers pfnPostSNMPTrapReceiversFunc;
  FDeleteSNMPTrapReceivers pfnDeleteSNMPTrapReceiversFunc;
  FGetSNMPTrapReceiver pfnGetSNMPTrapReceiverFunc;
  FPutSNMPTrapReceiver pfnPutSNMPTrapReceiverFunc;
  FDeleteSNMPTrapReceiver pfnDeleteSNMPTrapReceiverFunc;
  FGetSNMPAdvanced pfnGetSNMPAdvancedFunc;
  FPutSNMPAdvanced pfnPutSNMPAdvancedFunc;
  FGetSNMPAdvancedUsers pfnGetSNMPAdvancedUsersFunc;
  FPutSNMPAdvancedUsers pfnPutSNMPAdvancedUsersFunc;
  FPostSNMPAdvancedUsers pfnPostSNMPAdvancedUsersFunc;
  FDeleteSNMPAdvancedUsers pfnDeleteSNMPAdvancedUsersFunc;
  FGetSNMPAdvancedUser pfnGetSNMPAdvancedUserFunc;
  FPutSNMPAdvancedUser pfnPutSNMPAdvancedUserFunc;
  FDeleteSNMPAdvancedUser pfnDeleteSNMPAdvancedUserFunc;
  FGetSNMPAdvancedNotificationFilters pfnGetSNMPAdvancedNotificationFiltersFunc;
  FPutSNMPAdvancedNotificationFilters pfnPutSNMPAdvancedNotificationFiltersFunc;
  FPostSNMPAdvancedNotificationFilters pfnPostSNMPAdvancedNotificationFiltersFunc;
  FDeleteSNMPAdvancedNotificationFilters pfnDeleteSNMPAdvancedNotificationFiltersFunc;
  FGetSNMPAdvancedNotificationFilter pfnGetSNMPAdvancedNotificationFilterFunc;
  FPutSNMPAdvancedNotificationFilter pfnPutSNMPAdvancedNotificationFilterFunc;
  FDeleteSNMPAdvancedNotificationFilter pfnDeleteSNMPAdvancedNotificationFilterFunc;
  FGetSNMPAdvancedNotificationReceivers pfnGetSNMPAdvancedNotificationReceiversFunc;
  FPutSNMPAdvancedNotificationReceivers pfnPutSNMPAdvancedNotificationReceiversFunc;
  FPostSNMPAdvancedNotificationReceivers pfnPostSNMPAdvancedNotificationReceiversFunc;
  FDeleteSNMPAdvancedNotificationReceivers pfnDeleteSNMPAdvancedNotificationReceiversFunc;
  FGetSNMPAdvancedNotificationReceiver pfnGetSNMPAdvancedNotificationReceiverFunc;
  FPutSNMPAdvancedNotificationReceiver pfnPutSNMPAdvancedNotificationReceiverFunc;
  FDeleteSNMPAdvancedNotificationReceiver pfnDeleteSNMPAdvancedNotificationReceiverFunc;
  FGetSNMPAdvancedV3 pfnGetSNMPAdvancedV3Func;
  FPutSNMPAdvancedV3 pfnPutSNMPAdvancedV3Func;
  FGetQoS pfnGetQoSFunc;
  FPutQoS pfnPutQoSFunc;
  FGetCoSes pfnGetCoSesFunc;
  FPutCoSes pfnPutCoSesFunc;
  FPostCoSes pfnPostCoSesFunc;
  FDeleteCoSes pfnDeleteCoSesFunc;
  FGetCoS pfnGetCoSFunc;
  FPutCoS pfnPutCoSFunc;
  FDeleteCoS pfnDeleteCoSFunc;
  FGetDscps pfnGetDscpsFunc;
  FPutDscps pfnPutDscpsFunc;
  FPostDscps pfnPostDscpsFunc;
  FDeleteDscps pfnDeleteDscpsFunc;
  FGetDscp pfnGetDscpFunc;
  FPutDscp pfnPutDscpFunc;
  FDeleteDscp pfnDeleteDscpFunc;
  FGetDiscovery pfnGetDiscoveryFunc;
  FPutDiscovery pfnPutDiscoveryFunc;
  FGetSyslog pfnGetSyslogFunc;
  FPutSyslog pfnPutSyslogFunc;
  FGetSyslogServers pfnGetSyslogServersFunc;
  FPutSyslogServers pfnPutSyslogServersFunc;
  FPostSyslogServers pfnPostSyslogServersFunc;
  FDeleteSyslogServers pfnDeleteSyslogServersFunc;
  FGetSyslogServer pfnGetSyslogServerFunc;
  FPutSyslogServer pfnPutSyslogServerFunc;
  FDeleteSyslogServer pfnDeleteSyslogServerFunc;
  FGetIOPorts pfnGetIOPortsFunc;
  FGetIOPortStatuses pfnGetIOPortStatusesFunc;
  FGetInputPorts pfnGetInputPortsFunc;
  FGetInputPort pfnGetInputPortFunc;
  FPutInputPort pfnPutInputPortFunc;
  FGetIOPortStatus pfnGetIOPortStatusFunc;
  FGetOutputPorts pfnGetOutputPortsFunc;
  FGetOutputPort pfnGetOutputPortFunc;
  FPutOutputPort pfnPutOutputPortFunc;
  FSetIOportTrigger pfnSetIOportTriggerFunc;
  FGetAudioChannels pfnGetAudioChannelsFunc;
  FGetAudioChannel pfnGetAudioChannelFunc;
  FPutAudioChannel pfnPutAudioChannelFunc;
  FGetVideoOverlayImages pfnGetVideoOverlayImagesFunc;
  FPostVideoOverlayImages pfnPostVideoOverlayImagesFunc;
  FDeleteVideoOverlayImages pfnDeleteVideoOverlayImagesFunc;
  FGetVideoOverlayImage pfnGetVideoOverlayImageFunc;
  FPutVideoOverlayImage pfnPutVideoOverlayImageFunc;
  FDeleteVideoOverlayImage pfnDeleteVideoOverlayImageFunc;
  FGetVideoInput pfnGetVideoInputFunc;
  FGetVideoInputChannels pfnGetVideoInputChannelsFunc;
  FGetVideoInputChannel pfnGetVideoInputChannelFunc;
  FPutVideoInputChannel pfnPutVideoInputChannelFunc;
  FSetVideoFocus pfnSetVideoFocusFunc;
  FSetVideoIRIS pfnSetVideoIRISFunc;
  FGetVideoLens pfnGetVideoLensFunc;
  FGetVideoOverlay pfnGetVideoOverlayFunc;
  FPutVideoOverlay pfnPutVideoOverlayFunc;
  FDeleteVideoOverlay pfnDeleteVideoOverlayFunc;
  FGetVideoTextOverlays pfnGetVideoTextOverlaysFunc;
  FPutVideoTextOverlays pfnPutVideoTextOverlaysFunc;
  FPostVideoTextOverlays pfnPostVideoTextOverlaysFunc;
  FDeleteVideoTextOverlays pfnDeleteVideoTextOverlaysFunc;
  FGetVideoTextOverlay pfnGetVideoTextOverlayFunc;
  FPutVideoTextOverlay pfnPutVideoTextOverlayFunc;
  FDeleteVideoTextOverlay pfnDeleteVideoTextOverlayFunc;
  FGetVideoImageOverlays pfnGetVideoImageOverlaysFunc;
  FPutVideoImageOverlays pfnPutVideoImageOverlaysFunc;
  FPostVideoImageOverlays pfnPostVideoImageOverlaysFunc;
  FDeleteVideoImageOverlays pfnDeleteVideoImageOverlaysFunc;
  FGetVideoImageOverlay pfnGetVideoImageOverlayFunc;
  FPutVideoImageOverlay pfnPutVideoImageOverlayFunc;
  FDeleteVideoImageOverlay pfnDeleteVideoImageOverlayFunc;
  FGetPrivacyMask pfnGetPrivacyMaskFunc;
  FPutPrivacyMask pfnPutPrivacyMaskFunc;
  FGetPrivacyMaskRegions pfnGetPrivacyMaskRegionsFunc;
  FPutPrivacyMaskRegions pfnPutPrivacyMaskRegionsFunc;
  FPostPrivacyMaskRegions pfnPostPrivacyMaskRegionsFunc;
  FDeletePrivacyMaskRegions pfnDeletePrivacyMaskRegionsFunc;
  FGetPrivacyMaskRegion pfnGetPrivacyMaskRegionFunc;
  FPutPrivacyMaskRegion pfnPutPrivacyMaskRegionFunc;
  FDeletePrivacyMaskRegion pfnDeletePrivacyMaskRegionFunc;
  FGetSerialPorts pfnGetSerialPortsFunc;
  FGetSerialPort pfnGetSerialPortFunc;
  FPutSerialPort pfnPutSerialPortFunc;
  FSendCommandToSerailPort pfnSendCommandToSerailPortFunc;
  FGetDiagnosticsCommands pfnGetDiagnosticsCommandsFunc;
  FPostDiagnosticsCommands pfnPostDiagnosticsCommandsFunc;
  FDeleteDiagnosticsCommands pfnDeleteDiagnosticsCommandsFunc;
  FGetDiagnosticsCommand pfnGetDiagnosticsCommandFunc;
  FDeleteDiagnosticsCommand pfnDeleteDiagnosticsCommandFunc;
  FGetSRTPMasterKey pfnGetSRTPMasterKeyFunc;
  FPutSRTPMasterKey pfnPutSRTPMasterKeyFunc;
  FGetDeviceCert pfnGetDeviceCertFunc;
  FPutDeviceCert pfnPutDeviceCertFunc;
  FGetUsers pfnGetUsersFunc;
  FPutUsers pfnPutUsersFunc;
  FPostUsers pfnPostUsersFunc;
  FDeleteUsers pfnDeleteUsersFunc;
  FGetUser pfnGetUserFunc;
  FPutUser pfnPutUserFunc;
  FDeleteUser pfnDeleteUserFunc;
  FGetAAACert pfnGetAAACertFunc;
  FPutAAACert pfnPutAAACertFunc;
  FGetAdminAccesses pfnGetAdminAccessesFunc;
  FPutAdminAccesses pfnPutAdminAccessesFunc;
  FPostAdminAccesses pfnPostAdminAccessesFunc;
  FDeleteAdminAccesses pfnDeleteAdminAccessesFunc;
  FGetAdminAccess pfnGetAdminAccessFunc;
  FPutAdminAccess pfnPutAdminAccessfunc;
  FDeleteAdminAccess pfnDeleteAdminAccessFunc;
  FGetStreamingStatus pfnGetStreamingStatusFunc;
  FGetStreamingChannels pfnGetStreamingChannelsFunc;
  FPutStreamingChannels pfnPutStreamingChannelsFunc;
  FPostStreamingChannels pfnPostStreamingChannelsFunc;
  FDeleteStreamingChannels pfnDeleteStreamingChannelsFunc;
  FGetStreamingChannel pfnGetStreamingChannelFunc;
  FPutStreamingChannel pfnPutStreamingChannelFunc;
  FDeleteStreamingChannel pfnDeleteStreamingChannelFunc;
  FGetStreamingSessionStatus pfnGetStreamingSessionStatusFunc;
  FGetPTZhannels pfnGetPTZhannelsFunc;
  FPutPTZhannels pfnPutPTZhannelsFunc;
  FPostPTZhannels pfnPostPTZhannelsFunc;
  FDeletePTZhannels pfnDeletePTZhannelsFunc;
  FGetPTZhannel pfnGetPTZhannelFunc;
  FPutPTZhannel pfnPutPTZhannelFunc;
  FDeletePTZhannel pfnDeletePTZhannelFunc;
  FSetPTZHomePosition pfnSetPTZHomePositionFunc;
  FSetPTZContinuous pfnSetPTZContinuousFunc;
  FSetPTZMomentary pfnSetPTZMomentaryFunc;
  FSetPTZRelative pfnSetPTZRelativeFunc;
  FSetPTZAbsolute pfnSetPTZAbsoluteFunc;
  FSetPTZDigital pfnSetPTZDigitalFunc;
  FGetPTZStatus pfnGetPTZStatusFunc;
  FGetPTZPresets pfnGetPTZPresetsFunc;
  FPutPTZPresets pfnPutPTZPresetsFunc;
  FPostPTZPresets pfnPostPTZPresetsFunc;
  FDeletePTZPresets pfnDeletePTZPresetsFunc;
  FGetPTZPreset pfnGetPTZPresetFunc;
  FPutPTZPreset pfnPutPTZPresetFunc;
  FDeletePTZPreset pfnDeletePTZPresetFunc;
  FGotoPresetPosition pfnGotoPresetPositionFunc;
  FGetPTZPatrols pfnGetPTZPatrolsFunc;
  FPutPTZPatrols pfnPutPTZPatrolsFunc;
  FPostPTZPatrols pfnPostPTZPatrolsFunc;
  FDeletePTZPatrols pfnDeletePTZPatrolsFunc;
  FGetPTZPatrolStatuses pfnGetPTZPatrolStatusesFunc;
  FGetPTZPatrol pfnGetPTZPatrolFunc;
  FPutPTZPatrol pfnPutPTZPatrolFunc;
  FDeletePTZPatrol pfnDeletePTZPatrolFunc;
  FStartPTZPatrol pfnStartPTZPatrolFunc;
  FStopPTZPatrol pfnStopPTZPatrolFunc;
  FPausePTZPatrol pfnPausePTZPatrolFunc;
  FGetPTZPatrolStatus pfnGetPTZPatrolStatusFunc;
  FGetPTZPatrolSchedule pfnGetPTZPatrolScheduleFunc;
  FPutPTZPatrolSchedule pfnPutPTZPatrolScheduleFunc;
  FGetMotionDetctions pfnGetMotionDetctionsFunc;
  FGetMotionDetction pfnGetMotionDetctionFunc;
  FPutMotionDetction pfnPutMotionDetctionFunc;
  FGetMotionDetectionRegions pfnGetMotionDetectionRegionsFunc;
  FPutMotionDetectionRegions pfnPutMotionDetectionRegionsFunc;
  FPostMotionDetectionRegions pfnPostMotionDetectionRegionsFunc;
  FDeleteMotionDetectionRegions pfnDeleteMotionDetectionRegionsFunc;
  FGetMotionDetectionRegion pfnGetMotionDetectionRegionFunc;
  FPutMotionDetectionRegion pfnPutMotionDetectionRegionFunc;
  FDeleteMotionDetectionRegion pfnDeleteMotionDetectionRegionFunc;
  FGetEventNotification pfnGetEventNotificationFunc;
  FPutEventNotification pfnPutEventNotificationFunc;
  FGetEventTriggers pfnGetEventTriggersFunc;
  FPutEventTriggers pfnPutEventTriggersFunc;
  FPostEventTriggers pfnPostEventTriggersFunc;
  FDeleteEventTriggers pfnDeleteEventTriggersFunc;
  FGetEventTrigger pfnGetEventTriggerFunc;
  FPutEventTrigger pfnPutEventTriggerFunc;
  FDeleteEventTrigger pfnDeleteEventTriggerFunc;
  FGetEventTriggerNontifications pfnGetEventTriggerNontificationsFunc;
  FPutEventTriggerNontifications pfnPutEventTriggerNontificationsFunc;
  FPostEventTriggerNontifications pfnPostEventTriggerNontificationsFunc;
  FDeleteEventTriggerNontifications pfnDeleteEventTriggerNontificationsFunc;
  FGetEventTriggerNontification pfnGetEventTriggerNontificationFunc;
  FPutEventTriggerNontification pfnPutEventTriggerNontificationFunc;
  FDeleteEventTriggerNontification pfnDeleteEventTriggerNontificationFunc;
  FGetEventSchedule pfnGetEventScheduleFunc;
  FPutEventSchedule pfnPutEventScheduleFunc;
  FGetEventNotificationMethods pfnGetEventNotificationMethodsFunc;
  FPutEventNotificationMethods pfnPutEventNotificationMethodsFunc;
  FGetMailingNontifications pfnGetMailingNontificationsFunc;
  FPutMailingNontifications pfnPutMailingNontificationsFunc;
  FPostMailingNontifications pfnPostMailingNontificationsFunc;
  FDeleteMailingNontifications pfnDeleteMailingNontificationsFunc;
  FGetMailingNontification pfnGetMailingNontificationFunc;
  FPutMailingNontification pfnPutMailingNontificationFunc;
  FDeleteMailingNontification pfnDeleteMailingNontificationFunc;
  FGetFTPNontifications pfnGetFTPNontificationsFunc;
  FPutFTPNontifications pfnPutFTPNontificationsFunc;
  FPostFTPNontifications pfnPostFTPNontificationsFunc;
  FDeleteFTPNontifications pfnDeleteFTPNontificationsFunc;
  FGetFTPNontification pfnGetFTPNontificationFunc;
  FPutFTPNontification pfnPutFTPNontificationFunc;
  FDeleteFTPNontification pfnDeleteFTPNontificationFunc;
  FGetHTTPHostNontifications pfnGetHTTPHostNontificationsFunc;
  FPutHTTPHostNontifications pfnPutHTTPHostNontificationsFunc;
  FPostHTTPHostNontifications pfnPostHTTPHostNontificationsFunc;
  FDeleteHTTHostPNontifications pfnDeleteHTTHostPNontificationsFunc;
  FGetHTTPHostNontification pfnGetHTTPHostNontificationFunc;
  FPutHTTPHostNontification pfnPutHTTPHostNontificationFunc;
  FDeleteHTTHostPNontification pfnDeleteHTTHostPNontificationFunc;
  FGetEventNotificationAlert pfnGetEventNotificationAlertFunc;
  FPostEventNotificationAlert pfnPostEventNotificationAlertFunc;
  
    FGetRootServiceIndex pfnGetRootServiceIndexFunc;
  FGetRootServiceIndexr pfnGetRootServiceIndexrFunc;
  FGetRootServiceDescription pfnGetRootServiceDescriptionFunc;
  FGetSystemIndex pfnGetSystemIndexFunc;
  FGetSystemDescription pfnGetSystemDescriptionFunc;
  FGetSystemNetworkIndex pfnGetSystemNetworkIndexFunc;
  FGetSystemNetworkDescription pfnGetSystemNetworkDescriptionFunc;
  FGetSystemIOIndex pfnGetSystemIOIndexFunc;
  FGetSystemIODescription pfnGetSystemIODescriptionFunc;


  FGetSystemAudioIndex pfnGetSystemAudioIndexFunc;
  FGetSystemAudioDescription pfnGetSystemAudioDescriptionFunc;

  FGetSystemVideoIndex pfnGetSystemVideoIndexFunc;
  FGetSystemVideoDescription pfnGetSystemVideoDescriptionFunc;
  FGetStreamingIndex pfnGetStreamingIndexFunc;
  FGetStreamingDescription pfnGetStreamingDescriptionFunc;
  FGetPTZIndex pfnGetPTZIndexFunc;
  FGetPTZDescription pfnGetPTZDescriptionFunc;
  FGetCustomMotionDetectionIndex pfnGetCustomMotionDetectionIndexFunc;
  FGetCustomMotionDetectionDescription pfnGetCustomMotionDetectionDescriptionFunc;
  FGetSecurityIndex pfnGetSecurityIndexFunc;
  FGetSecurityDescription pfnGetSecurityDescriptionFunc;
  FGetSecurityAAAIndex pfnGetSecurityAAAIndexFunc;
  FGetSecurityAAADescription pfnGetSecurityAAADescriptionFunc;
    
} TPsiadInfo;


//---private function---
SCODE Psiad_InitHTTPSock(int *piSck, CHAR *szPath);
SCODE Psiad_InitXmlWrapper(HANDLE *phXMLWrapperObj, TXmlWrapperTreeMap *ptTreeMap);
SCODE Psiad_Loop(TPsiadInfo *ptPsiadInfo);
SCODE Psiad_ParsingMsg(TPsiadInfo *ptPsiadInfo, CHAR *szMsg, TPsiaMsgInfo *ptMsgInfo,TPsiaInputMsgInfo *ptPsiaInputMsgInfo);
SCODE Psiad_Dispatch2Service(TPsiadInfo *ptPsiadInfo, EPsiaMsgStatus eMsgStatus, TPsiaInputMsgInfo *ptPsiaInputMsgInfo,  TPsiaOutputMsgInfo *ptOutMsgInfo);


SCODE Psiad_GetRootServiceIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetRootServiceIndexr(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetRootServiceDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemNetworkIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemNetworkDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemIOIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemIODescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemAudioIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemAudioDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemVideoIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSystemVideoDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStreamingIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStreamingDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetCustomMotionDetectionIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetCustomMotionDetectionDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSecurityIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSecurityDescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSecurityAAAIndex(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSecurityAAADescription(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);





SCODE Psiad_Reboot(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_UpdateFirmware(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetConfigurationData(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutConfigurationData(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE FactoryReset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDeviceInfo(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutDeviceInfo(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SupportReport(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDeviceStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetLocalTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutLocalTime(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetTimeZone(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutTimeZone(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteNTPServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetNTPServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutNTPServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteNTPServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetLogging(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutLogging(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetLoggingMessages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStorageVolumes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStorageVolume(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStorageVolumeStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_FomatStorageVolume(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStorageVolumeFiles(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteStorageVolumeFiles(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStorageVolumeFile(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteStorageVolumeFile(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStorageVolumeData(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetNetworkInterfaces(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetNetworkInterface(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutNetworkInterface(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIPAddress(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutIPAddress(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetWireless(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutWireless(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIEEE8021x(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutIEEE8021x(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIPFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutIPFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteIPFilterAddresses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteIPFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMP(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMP(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPV2c(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPV2c(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPTrapReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPTrapReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPTrapReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPTrapReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvanced(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvanced(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPAdvancedUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPAdvancedUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPAdvancedNotificationFilters(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedNotificationFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedNotificationFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPAdvancedNotificationFilter(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPAdvancedNotificationReceivers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedNotificationReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedNotificationReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSNMPAdvancedNotificationReceiver(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSNMPAdvancedV3(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSNMPAdvancedV3(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetQoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutQoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteCoSes(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetCoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutCoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteCoS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteDscps(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDscp(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutDscp(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteDscp(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDiscovery(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutDiscovery(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSyslog(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSyslog(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSyslogServers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSyslogServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSyslogServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteSyslogServer(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIOPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIOPortStatuses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetInputPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetInputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutInputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetIOPortStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetOutputPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetOutputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutOutputPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetIOportTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetAudioChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetAudioChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutAudioChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoOverlayImages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostVideoOverlayImages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoOverlayImages(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoOverlayImage(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoOverlayImage(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoOverlayImage(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoInput(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoInputChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoInputChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoInputChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetVideoFocus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetVideoIRIS(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoLens(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoTextOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoTextOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoTextOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoTextOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoImageOverlays(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetVideoImageOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutVideoImageOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteVideoImageOverlay(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPrivacyMask(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPrivacyMask(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostPrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePrivacyMaskRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPrivacyMaskRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPrivacyMaskRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePrivacyMaskRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSerialPorts(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSerialPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSerialPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SendCommandToSerailPort(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDiagnosticsCommands(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostDiagnosticsCommands(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteDiagnosticsCommands(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDiagnosticsCommand(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteDiagnosticsCommand(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetSRTPMasterKey(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutSRTPMasterKey(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetDeviceCert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutDeviceCert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteUsers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteUser(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetAAACert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutAAACert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteAdminAccesses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetAdminAccess(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutAdminAccess(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteAdminAccess(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStreamingStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteStreamingChannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStreamingChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutStreamingChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteStreamingChannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetStreamingSessionStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostPTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePTZhannels(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZhannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZhannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePTZhannel(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetPTZHomePosition(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetPTZContinuous(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetPTZMomentary(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetPTZRelative(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetPTZAbsolute(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_SetPTZDigital(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostPTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePTZPresets(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPreset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZPreset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePTZPreset(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GotoPresetPosition(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostPTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePTZPatrols(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPatrolStatuses(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeletePTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_StartPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_StopPTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PausePTZPatrol(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPatrolStatus(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetPTZPatrolSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutPTZPatrolSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetMotionDetctions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetMotionDetction(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutMotionDetction(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteMotionDetectionRegions(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetMotionDetectionRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutMotionDetectionRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteMotionDetectionRegion(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventNotification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventNotification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteEventTriggers(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteEventTrigger(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteEventTriggerNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventTriggerNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventTriggerNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteEventTriggerNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventSchedule(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventNotificationMethods(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutEventNotificationMethods(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteMailingNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetMailingNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutMailingNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteMailingNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteFTPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetFTPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutFTPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteFTPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetHTTPHostNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutHTTPHostNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostHTTPHostNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteHTTHostPNontifications(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetHTTPHostNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PutHTTPHostNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_DeleteHTTHostPNontification(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_GetEventNotificationAlert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);
SCODE Psiad_PostEventNotificationAlert(TPsiadInfo *ptPsiadInfo, TPsiaInputMsgInfo *ptPsiaInputMsgInfo, TPsiaOutputMsgInfo *ptOutMsgInfo);

//We suppose the ID is 0. Just one channel is on the system.
#define ROOTSERVICE_INDEX 	"/PSIA/index"
#define ROOTSERVICE_INDEXR 	"/PSIA/indexr"
#define ROOTSERVICE_DESCRIPTION 	"/PSIA/description"
#define SYSTEM_INDEX 	"/PSIA/System/index"
#define SYSTEM_DESCRIPTION 	"/PSIA/System/description"
#define SYSTEM_NETWORK_INDEX  	"/PSIA/System/Network/index"
#define SYSTEM_NETWORK_DESCRIPTION  	"/PSIA/System/Network/description"
#define SYSTEM_IO_INDEX  	"/PSIA/System/IO/index"
#define SYSTEM_IO_DESCRIPTION 	"/PSIA/System/IO/description"
#define SYSTEM_AUDIO_INDEX  "/PSIA/System/Audio/index"
#define SYSTEM_AUDIO_DESCRIPTION 	"/PSIA/System/Audio/description"
#define SYSTEM_VIDEO_INDEX  "/PSIA/System/Video/index"
#define SYSTEM_VIDEO_DESCRIPTION 	"/PSIA/System/Video/description"
#define STREAMING_INDEX  "/PSIA/Streaming/index"
#define STREAMING_DESCRIPTION "/PSIA/Streaming/description"
#define PTZ_INDEX  "/PSIA/PTZ/index"
#define PTZ_DESCRIPTION "/PSIA/PTZ/description"
#define CUSTOM_MD_INDEX  "/PSIA/Custom/MotionDetection/index"
#define CUSTOM_MD_DESCRIPTION "/PSIA/Custom/MotionDetection/description"
#define SECUTITY_INDEX  "/PSIA/Security/index"
#define SECUTITY_DESCRIPTION "/PSIA/Security/description"
#define SECUTITY_AAA_INDEX  "/PSIA/Security/AAA/index"
#define SECUTITY_AAA_DESCRIPTION "/PSIA/Security/AAA/description"

#define REBOOT_URI "/PSIA/System/reboot"
#define UPDATEFIRMWARE_URI "/PSIA/System/updateFirmware"
#define CONFIGURATIONDATA_URI "/PSIA/System/configurationData"
			  
#define FACTORYRESET_URI "/PSIA/System/factoryReset"
#define DEVICEINFO_URI "/PSIA/System/deviceInfo"
#define SUPPORTREPORT_URI "/PSIA/System/supportReport"
#define STATUS_URI "/PSIA/System/status"
#define TIME_URI "/PSIA/System/time"
#define LOCALTIME_URI "/PSIA/System/time/localTime"
#define TIMEZONE_URI "/PSIA/System/time/timeZone"
#define NTPSERVERS_URI "/PSIA/System/time/ntpServers"
#define NTPSERVERS_0_URI "/PSIA/System/time/ntpServers/0"
#define LOGGING_URI "/PSIA/System/logging"
#define LOGGING_MESSAGES_URI "/PSIA/System/logging/messages"
#define VOLUMES_URI "/PSIA/System/Storage/volumes"
#define VOLUMES_0_URI "/PSIA/System/Storage/volumes/0"
#define VOLUMES_0_STATUS_URI "/PSIA/System/Storage/volumes/0/status"
#define FORMAT_URI "/PSIA/System/Storage/volumes/0/format"
#define FILES_URI "/PSIA/System/Storage/volumes/0/files"
#define FILES_0_URI "/PSIA/System/Storage/volumes/0/files/0"
#define FILES_0_DATA_URI "/PSIA/System/Storage/volumes/0/files/0/data"
#define INTERFACES_URI "/PSIA/System/Network/interfaces"
#define INTERFACES_0_URI "/PSIA/System/Network/interfaces/0"
#define INTERFACES_0_IPADDRESS_URI "/PSIA/System/Network/interfaces/0/ipAddress"
#define INTERFACES_0_WIRELESS_URI "/PSIA/System/Network/interfaces/0/wireless"
#define INTERFACES_0_IEEE802_1X_URI "/PSIA/System/Network/interfaces/0/ieee802.1x"
#define INTERFACES_0_IPFILTER_URI "/PSIA/System/Network/interfaces/0/ipFilter"
#define INTERFACES_0_IPFILTER_FILTERADDRESSES_URI "/PSIA/System/Network/interfaces/0/ipFilter/filterAddresses"
#define INTERFACES_0_IPFILTER_FILTERADDRESSES_0_URI  "/PSIA/System/Network/interfaces/0/ipFilter/filterAddresses/0"
#define INTERFACES_0_SNMP "/PSIA/System/Network/interfaces/0/snmp"
#define INTERFACES_0_SNMP_V2C_URI "/PSIA/System/Network/interfaces/0/snmp/v2c"
#define INTERFACES_0_SNMP_V2C_TRAPRECEIVERS_URI "/PSIA/System/Network/interfaces/0/snmp/v2c/trapReceivers"
#define INTERFACES_0_SNMP_V2C_TRAPRECEIVERS_0_URI "/PSIA/System/Network/interfaces/0/snmp/v2c/trapReceivers/0"
#define INTERFACES_0_SNMP_ADVANCED_URI "/PSIA/System/Network/interfaces/0/snmp/advanced"
#define INTERFACES_0_SNMP_ADVANCED_USERS_URI "/PSIA/System/Network/interfaces/0/snmp/advanced/users"
#define INTERFACES_0_SNMP_ADVANCED_USERS_0_URI "/PSIA/System/Network/interfaces/0/snmp/advanced/users/0"
#define INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONFILTERS_URI "/PSIA/System/Network/interfaces/0/snmp/advanced/notificationFilters"
#define INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONFILTERS_0_URI "/PSIA/System/Network/interfaces/0/snmp/advanced/notificationFilters/0"
#define INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONRECEIVERS_URI "/PSIA/System/Network/interfaces/0/snmp/advanced/notificationReceivers"
#define INTERFACES_0_SNMP_ADVANCED_NOTIFICATIONRECEIVERS_0_URI "/PSIA/System/Network/interfaces/0/snmp/advanced/notificationReceivers/0"
#define INTERFACES_0_SNMP_V3_URI "/PSIA/System/Network/interfaces/0/snmp/v3"
#define INTERFACES_0_QOS_URI "/PSIA/System/Network/interfaces/0/qos"
#define INTERFACES_0_QOS_COS_URI "/PSIA/System/Network/interfaces/0/qos/cos"
#define INTERFACES_0_QOS_COS_0_URI "/PSIA/System/Network/interfaces/0/qos/cos/0"
#define INTERFACES_0_QOS_DSCP_URI "/PSIA/System/Network/interfaces/0/qos/dscp"
#define INTERFACES_0_QOS_DSCP_0_URI "/PSIA/System/Network/interfaces/0/qos/dscp/0"
#define INTERFACES_0_DISCOVERY_URI "/PSIA/System/Network/interfaces/0/discovery"
#define INTERFACES_0_SYSLOG_URI "/PSIA/System/Network/interfaces/0/syslog"
#define INTERFACES_0_SYSLOG_SERVERS_URI "/PSIA/System/Network/interfaces/0/syslog/servers"
#define INTERFACES_0_SYSLOG_SERVERS_0_URI "/PSIA/System/Network/interfaces/0/syslog/servers/0"
#define IO_URI "/PSIA/System/IO"
#define IO_STATUS_URI "/PSIA/System/IO/status"
#define IO_INPUTS_URI "/PSIA/System/IO/inputs"
#define IO_INPUTS_0_URI "/PSIA/System/IO/inputs/0"
#define IO_INPUTS_0_STATUS_URI "/PSIA/System/IO/inputs/0/status"
#define IO_OUTPUTS_URI "/PSIA/System/IO/outputs"
#define IO_OUTPUTS_0_URI "/PSIA/System/IO/outputs/0"
#define IO_OUTPUTS_0_TRIGGER_URI "/PSIA/System/IO/outputs/0/trigger"
#define IO_OUTPUTS_0_STATUS_URI "/PSIA/System/IO/outputs/0/status"
#define AUDIO_CHANNELS_URI "/PSIA/System/Audio/channels"
#define AUDIO_CHANNELS_0_URI "/PSIA/System/Audio/channels/0"
#define VIDEO_OVERLAYIMAGES_URI "/PSIA/System/Video/overlayImages"
#define VIDEO_OVERLAYIMAGES_0_URI "/PSIA/System/Video/overlayImages/0"
#define VIDEO_INPUTS_URI "/PSIA/System/Video/inputs"
#define VIDEO_INPUTS_CHANNELS_URI "/PSIA/System/Video/inputs/channels"
#define VIDEO_INPUTS_CHANNELS_0_URI "/PSIA/System/Video/inputs/channels/0"
#define VIDEO_INPUTS_CHANNELS_0_FOCUS_URI "/PSIA/System/Video/inputs/channels/0/focus"
#define VIDEO_INPUTS_CHANNELS_0_IRIS_URI "/PSIA/System/Video/inputs/channels/0/iris"
#define VIDEO_INPUTS_CHANNELS_0_LENS_URI "/PSIA/System/Video/inputs/channels/0/lens"
#define VIDEO_INPUTS_CHANNELS_0_OVERLAYS_URI "/PSIA/System/Video/inputs/channels/0/overlays"
#define VIDEO_INPUTS_CHANNELS_0_OVERLAYS_TEXT_URI "/PSIA/System/Video/inputs/channels/0/overlays/text"
#define VIDEO_INPUTS_CHANNELS_0_OVERLAYS_TEXT_0_URI "/PSIA/System/Video/inputs/channels/0/overlays/text/0"
#define VIDEO_INPUTS_CHANNELS_0_OVERLAYS_IMAGE_URI "/PSIA/System/Video/inputs/channels/0/overlays/image"
#define  VIDEO_INPUTS_CHANNELS_0_OVERLAYS_IMAGE_0_URI "/PSIA/System/Video/inputs/channels/0/overlays/image/0"
#define VIDEO_INPUTS_CHANNELS_0_PRIVACYMASK_URI "/PSIA/System/Video/inputs/channels/0/privacyMask"
#define VIDEO_INPUTS_CHANNELS_0_PRIVACYMASK_REGIONS_URI "/PSIA/System/Video/inputs/channels/0/privacyMask/regions"
#define VIDEO_INPUTS_CHANNELS_0_PRIVACYMASK_REGIONS_0_URI "/PSIA/System/Video/inputs/channels/0/privacyMask/regions/0"
#define SERIAL_PORTS_URI "/PSIA/System/Serial/ports"
#define SERIAL_PORTS_0_URI "/PSIA/System/Serial/ports/0"
#define SERIAL_PORTS_0_COMMAND_URI "/PSIA/System/Serial/ports/0/command"
#define DIAGNOSTICS_COMMANDS_URI "/PSIA/Diagnostics/commands"
#define DIAGNOSTICS_COMMANDS_0_URI "/PSIA/Diagnostics/commands/0"
#define SECURITY_SRTPMASTERKEY_URI "/PSIA/Security/srtpMasterKey"
#define SECURITY_DEVICECERTIFICATE_URI "/PSIA/Security/deviceCertificate"
#define SECURITY_AAA_USERS_URI "/PSIA/Security/AAA/users"
#define SECURITY_AAA_USERS_0_URI "/PSIA/Security/AAA/users/0"
#define SECURITY_AAA_CERTIFICATE_URI "/PSIA/Security/AAA/certificate"
#define SECURITY_AAA_ADMINACCESSES_URI "/PSIA/Security/AAA/adminAccesses"
#define SECURITY_AAA_ADMINACCESSES_0_URI "/PSIA/Security/AAA/adminAccesses/0"
#define STREAMING_STATUS_URI "/PSIA/Streaming/status"
#define STREAMING_CHANNELS_URI "/PSIA/Streaming/channels"
#define STREAMING_CHANNELS_0_URI "/PSIA/Streaming/channels/0"
#define STREAMING_CHANNELS_0_STATUS_URI "/PSIA/Streaming/channels/0/status"
#define STREAMING_CHANNELS_0_HTTP_URI "/PSIA/Streaming/channels/0/http"
#define STREAMING_CHANNELS_0_PICTURE_URI "/PSIA/Streaming/channels/0/picture"
#define STREAMING_CHANNELS_0_REQUESTKEYFRAME_URI "/PSIA/Streaming/channels/0/requestKeyFrame"
#define PTZ_CHANNELS_URI "/PSIA/PTZ/channels"
#define PTZ_CHANNELS_0_URI "/PSIA/PTZ/channels/0"
#define PTZ_CHANNELS_0_HOMEPOSITION_URI "/PSIA/PTZ/channels/<ID>/homePosition"
#define PTZ_CHANNELS_0_CONTINUOUS_URI "/PSIA/PTZ/channels/<ID>/continuous"
#define PTZ_CHANNELS_0_MOMENTARY_URI "/PSIA/PTZ/channels/<ID>/momentary"
#define PTZ_CHANNELS_0_RELATIVE_URI "/PSIA/PTZ/channels/<ID>/relative"
#define PTZ_CHANNELS_0_ABSOLUTE_URI "/PSIA/PTZ/channels/<ID>/absolute"
#define PTZ_CHANNELS_0_DIGITAL_URI "/PSIA/PTZ/channels/<ID>/digital"
#define PTZ_CHANNELS_0_STATUS_URI "/PSIA/PTZ/channels/<ID>/status"
#define PTZ_CHANNELS_0_PRESETS_URI "/PSIA/PTZ/channels/<ID>/presets"
#define PTZ_CHANNELS_0_PRESETS_0_URI "/PSIA/PTZ/channels/<ID>/presets/<ID>"
#define PTZ_CHANNELS_0_PRESETS_0_GOTO_URI "/PSIA/PTZ/channels/<ID>/presets/<ID>/goto"
#define PTZ_CHANNELS_0_PATROLS_URI "/PSIA/PTZ/channels/<ID>/patrols"
#define PTZ_CHANNELS_0_PATROLS_STATUS_URI "/PSIA/PTZ/channels/<ID>/patrols/status"
#define PTZ_CHANNELS_0_PATROLS_0_URI "/PSIA/PTZ/channels/<ID>/patrols/<ID>"
#define PTZ_CHANNELS_0_PATROLS_0_START_URI "/PSIA/PTZ/channels/<ID>/patrols/<ID>/start"
#define PTZ_CHANNELS_0_PATROLS_0_STOP_URI "/PSIA/PTZ/channels/<ID>/patrols/<ID>/stop"
#define PTZ_CHANNELS_0_PATROLS_0_PAUSE_URI "/PSIA/PTZ/channels/<ID>/patrols/<ID>/pause"
#define PTZ_CHANNELS_0_PATROLS_0_STATUS_URI "/PSIA/PTZ/channels/<ID>/patrols/<ID>/status"
#define PTZ_CHANNELS_0_PATROLS_0_SCHEDULE_URI "/PSIA/PTZ/channels/<ID>/patrols/<ID>/schedule"
#define CUSTOM_MOTIONDETECTION_URI "/PSIA/Custom/MotionDetection"
#define CUSTOM_MOTIONDETECTION_0_URI "/PSIA/Custom/MotionDetection/<ID>"
#define CUSTOM_MOTIONDETECTION_0_REGIONS_URI "/PSIA/Custom/MotionDetection/<ID>/regions"
#define CUSTOM_MOTIONDETECTION_0_REGIONS_0_URI "/PSIA/Custom/MotionDetection/<ID>/regions/<ID>"
#define CUSTOM_EVENT_URI "/PSIA/Custom/Event"
#define CUSTOM_EVENT_TRIGGERS_URI "/PSIA/Custom/Event/triggers"
#define CUSTOM_EVENT_TRIGGERS_0_URI "/PSIA/Custom/Event/triggers/<ID>"
#define CUSTOM_EVENT_TRIGGERS_0_NOTIFICATIONS_URI "/PSIA/Custom/Event/triggers/<ID>/notifications"
#define CUSTOM_EVENT_TRIGGERS_0_NOTIFICATIONS_0_URI "/PSIA/Custom/Event/triggers/<ID>/notifications/<ID>"
#define CUSTOM_EVENT_SCHEDULE_URI "/PSIA/Custom/Event/schedule"
#define CUSTOM_EVENT_NOTIFICATION_URI "/PSIA/Custom/Event/notification"
#define CUSTOM_EVENT_NOTIFICATION_MAILING_URI "/PSIA/Custom/Event/notification/mailing"
#define CUSTOM_EVENT_NOTIFICATION_MAILING_0_URI "/PSIA/Custom/Event/notification/mailing/<ID>"
#define CUSTOM_EVENT_NOTIFICATION_FTP_URI "/PSIA/Custom/Event/notification/ftp"
#define CUSTOM_EVENT_NOTIFICATION_FTP_0_URI "/PSIA/Custom/Event/notification/ftp/<ID>"
#define CUSTOM_EVENT_NOTIFICATION_HTTPHOST_URI "/PSIA/Custom/Event/notification/httpHost"
#define CUSTOM_EVENT_NOTIFICATION_HTTPHOST_0_URI "/PSIA/Custom/Event/notification/httpHost/<ID>"
#define CUSTOM_EVENT_NOTIFICATION_ALERTSTREAM_URI "/PSIA/Custom/Event/notification/alertStream"
   

#endif