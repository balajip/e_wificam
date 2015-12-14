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

#ifndef _PSIAD_H_
#define _PSIAD_H_
#include "typedef.h"
#include "errordef.h"
#include "psiad_types.h"
//7.1.1
typedef SCODE (* FReboot )(HANDLE dwIntance, TPsiaResponseStatus *ptResponseStatus);

//7.1.2
typedef SCODE (* FUpdateFirmware )(HANDLE dwIntance, TPsiaResponseStatus *ptResponseStatus);

//7.1.3
typedef SCODE (* FGetConfigurationData )(HANDLE dwIntance, BYTE *pbyOpaqueData,DWORD *pdwLen);
typedef SCODE (* FPutConfigurationData )(HANDLE dwIntance, BYTE *pbyOpaqueData,TPsiaResponseStatus *ptResponseStatus);


//7.1.4
typedef SCODE (* FFactoryReset)(HANDLE dwIntance,CHAR *szMode,TPsiaResponseStatus *ptResponseStatus);

//7.1.5
typedef SCODE (* FGetDeviceInfo)(HANDLE dwIntance, TPsiaDeviceInfo *ptDeviceInfo);
typedef SCODE (* FPutDeviceInfo)(HANDLE dwIntance, TPsiaDeviceInfo *ptDeviceInfo,TPsiaResponseStatus *ptResponseStatus);

//7.1.6
typedef SCODE (* FSupportReport)(HANDLE dwIntance, BYTE *pbySupportData,DWORD *pdwLen);

//7.1.7
typedef SCODE (* FGetDeviceStatus)(HANDLE dwIntance,TPsiaDeviceStatus *ptDeviceStatus);

//7.1.8
typedef SCODE (* FGetTime)(HANDLE dwIntance,TPsiaTime *ptTime);
typedef SCODE (* FPutTime)(HANDLE dwIntance,TPsiaTime *ptTime,TPsiaResponseStatus *ptResponseStatus);

//7.1.9
typedef SCODE (* FGetLocalTime)(HANDLE dwIntance,CHAR *szLocalTimeString,DWORD *pdwLen);
typedef SCODE (* FPutLocalTime)(HANDLE dwIntance,CHAR *szLocalTimeString,TPsiaResponseStatus *ptResponseStatus);

//7.1.10
typedef SCODE (* FGetTimeZone)(HANDLE dwIntance,CHAR *szTimeZoneString,DWORD *pdwLen);
typedef SCODE (* FPutTimeZone)(HANDLE dwIntance,CHAR *szTimeZoneString,TPsiaResponseStatus *ptResponseStatus);

//7.1.11
typedef SCODE (* FGetNTPServers)(HANDLE dwIntance,TPsiaNTPServerList *ptNTPServerList);
typedef SCODE (* FPutNTPServers)(HANDLE dwIntance,TPsiaNTPServerList *ptNTPServerList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostNTPServers)(HANDLE dwIntance,TPsiaNTPServer *ptNTPServer,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteNTPServers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.1.12
typedef SCODE (* FGetNTPServer)(HANDLE dwIntance,TPsiaNTPServer *ptNTPServer);
typedef SCODE (* FPutNTPServer)(HANDLE dwIntance,TPsiaNTPServer *ptNTPServer,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteNTPServer)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.1.13
typedef SCODE (* FGetLogging)(HANDLE dwIntance,TPsiaLogging *ptLogging);
typedef SCODE (* FPutLogging)(HANDLE dwIntance,TPsiaLogging *ptLogging,TPsiaResponseStatus *ptResponseStatus);

//7.1.14
typedef SCODE (* FGetLoggingMessages)(HANDLE dwIntance,TPsiaLogMessageList *ptLogMessageList);

//7.3
typedef SCODE (* FGetStorageVolumes)(HANDLE dwIntance,TPsiaStorageVolumeList *ptStorageVolumeList);

//7.3.1
typedef SCODE (* FGetStorageVolume)(HANDLE dwIntance,TPsiaStorageVolume *ptStorageVolume);

//7.3.2
typedef SCODE (* FGetStorageVolumeStatus)(HANDLE dwIntance,TPsiaStorageVolumeStatus *ptStorageVolumeStatus);

//7.3.3
typedef SCODE (* FFomatStorageVolume)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.3.4
typedef SCODE (* FGetStorageVolumeFiles)(HANDLE dwIntance,TPsiaStorageFileList *ptStorageFileList);
typedef SCODE (* FDeleteStorageVolumeFiles)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.3.5
typedef SCODE (* FGetStorageVolumeFile)(HANDLE dwIntance,TPsiaStorageFile *ptStorageFile);
typedef SCODE (* FDeleteStorageVolumeFile)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.3.6
typedef SCODE (* FGetStorageVolumeData)(HANDLE dwIntance,BYTE *pbyRawFileData);

//7.4.1
typedef SCODE (* FGetNetworkInterfaces)(HANDLE dwIntance,TPsiaNetworkInterfaceList *ptNetworkInterfaceList);

//7.4.2
typedef SCODE (* FGetNetworkInterface)(HANDLE dwIntance,TPsiaNetworkInterface *ptNetworkInterface);
typedef SCODE (* FPutNetworkInterface)(HANDLE dwIntance,TPsiaNetworkInterface *ptNetworkInterface,TPsiaResponseStatus *ptResponseStatus);

//7.4.3
typedef SCODE (* FGetIPAddress)(HANDLE dwIntance,TPsiaIPAddress *ptIPAddress);
typedef SCODE (* FPutIPAddress)(HANDLE dwIntance,TPsiaIPAddress *ptIPAddress,TPsiaResponseStatus *ptResponseStatus);

//7.4.4
typedef SCODE (* FGetWireless)(HANDLE dwIntance,TPsiaWireless *ptWireless);
typedef SCODE (* FPutWireless)(HANDLE dwIntance,TPsiaWireless *ptWireless,TPsiaResponseStatus *ptResponseStatus);

//7.4.5
typedef SCODE (* FGetIEEE8021x)(HANDLE dwIntance,TPsiaIEEE8021X *ptIEEE8021X);
typedef SCODE (* FPutIEEE8021x)(HANDLE dwIntance,TPsiaIEEE8021X *ptIEEE8021X,TPsiaResponseStatus *ptResponseStatus);


//7.4.6
typedef SCODE (* FGetIPFilter)(HANDLE dwIntance,TPsiaIPFilter *ptIPFilter);
typedef SCODE (* FPutIPFilter)(HANDLE dwIntance,TPsiaIPFilter *ptIPFilter,TPsiaResponseStatus *ptResponseStatus);


//7.4.7
typedef SCODE (* FGetIPFilterAddresses)(HANDLE dwIntance,TPsiaIPFilterAddressList *ptIPFilterAddressList);
typedef SCODE (* FPutIPFilterAddresses)(HANDLE dwIntance,TPsiaIPFilterAddressList *ptIPFilterAddressList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostIPFilterAddresses)(HANDLE dwIntance,TPsiaIPFilterAddress *ptIPFilterAddress,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteIPFilterAddresses)(HANDLE dwIntance, TPsiaResponseStatus *ptResponseStatus);

//7.4.8
typedef SCODE (* FDeleteIPFilter)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.9
typedef SCODE (* FGetSNMP)(HANDLE dwIntance, TPsiaSNMP *ptSNMP);
typedef SCODE (* FPutSNMP)(HANDLE dwIntance, TPsiaSNMP *ptSNMP,TPsiaResponseStatus *ptResponseStatus);

//7.4.10
typedef SCODE (* FGetSNMPV2c)(HANDLE dwIntance, TPsiaSNMPV2c *ptSNMPV2c);
typedef SCODE (* FPutSNMPV2c)(HANDLE dwIntance, TPsiaSNMPV2c *ptSNMPV2c,TPsiaResponseStatus *ptResponseStatus);

//7.4.11
typedef SCODE (* FGetSNMPTrapReceivers)(HANDLE dwIntance,  TPsiaSNMPTrapReceiverList *ptSNMPTrapReceiverList);
typedef SCODE (* FPutSNMPTrapReceivers)(HANDLE dwIntance, TPsiaSNMPTrapReceiverList *ptSNMPTrapReceiverList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostSNMPTrapReceivers)(HANDLE dwIntance, TPsiaSNMPTrapReceiver *ptSNMPTrapReceiver,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPTrapReceivers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.4.12
typedef SCODE (* FGetSNMPTrapReceiver)(HANDLE dwIntance,  TPsiaSNMPTrapReceiver *ptSNMPTrapReceiver);
typedef SCODE (* FPutSNMPTrapReceiver)(HANDLE dwIntance, TPsiaSNMPTrapReceiver *ptSNMPTrapReceiver,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPTrapReceiver)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.4.13
typedef SCODE (* FGetSNMPAdvanced)(HANDLE dwIntance,TPsiaSNMPAdvanced *ptSNMPAdvanced);
typedef SCODE (* FPutSNMPAdvanced)(HANDLE dwIntance,TPsiaSNMPAdvanced *ptSNMPAdvanced,TPsiaResponseStatus *ptResponseStatus);


//7.4.14
typedef SCODE (* FGetSNMPAdvancedUsers)(HANDLE dwIntance,  TPsiaSNMPUserList *ptSNMPUserList);
typedef SCODE (* FPutSNMPAdvancedUsers)(HANDLE dwIntance, TPsiaSNMPUserList *ptSNMPUserList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostSNMPAdvancedUsers)(HANDLE dwIntance, TPsiaSNMPUser *ptSNMPUser,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPAdvancedUsers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.15
typedef SCODE (* FGetSNMPAdvancedUser)(HANDLE dwIntance,  TPsiaSNMPUser *ptSNMPUser);
typedef SCODE (* FPutSNMPAdvancedUser)(HANDLE dwIntance, TPsiaSNMPUser *ptSNMPUser,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPAdvancedUser)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.16
typedef SCODE (* FGetSNMPAdvancedNotificationFilters)(HANDLE dwIntance,  TPsiaSNMPNotificationFilterList  *ptSNMPNotificationFilterList);
typedef SCODE (* FPutSNMPAdvancedNotificationFilters)(HANDLE dwIntance, TPsiaSNMPNotificationFilterList  *ptSNMPNotificationFilterList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostSNMPAdvancedNotificationFilters)(HANDLE dwIntance, TPsiaSNMPNotificationFilter  *ptSNMPNotificationFilter,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPAdvancedNotificationFilters)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.17
typedef SCODE (* FGetSNMPAdvancedNotificationFilter)(HANDLE dwIntance,  TPsiaSNMPNotificationFilter  *ptSNMPNotificationFilter);
typedef SCODE (* FPutSNMPAdvancedNotificationFilter)(HANDLE dwIntance, TPsiaSNMPNotificationFilter  *ptSNMPNotificationFilter,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPAdvancedNotificationFilter)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.18
typedef SCODE (* FGetSNMPAdvancedNotificationReceivers)(HANDLE dwIntance,  TPsiaSNMPNotificationReceiverList  *ptSNMPNotificationReceiverList);
typedef SCODE (* FPutSNMPAdvancedNotificationReceivers)(HANDLE dwIntance, TPsiaSNMPNotificationReceiverList  *ptSNMPNotificationReceiverList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostSNMPAdvancedNotificationReceivers)(HANDLE dwIntance, TPsiaSNMPNotificationReceiver  *ptSNMPNotificationReceiver,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPAdvancedNotificationReceivers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.19
typedef SCODE (* FGetSNMPAdvancedNotificationReceiver)(HANDLE dwIntance,  TPsiaSNMPNotificationReceiver  *ptSNMPNotificationReceiver);
typedef SCODE (* FPutSNMPAdvancedNotificationReceiver)(HANDLE dwIntance, TPsiaSNMPNotificationReceiver  *ptSNMPNotificationReceiver,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSNMPAdvancedNotificationReceiver)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.20
typedef SCODE (* FGetSNMPAdvancedV3)(HANDLE dwIntance,  TPsiaSNMPAdvanced  *ptSNMPAdvanced);
typedef SCODE (* FPutSNMPAdvancedV3)(HANDLE dwIntance, TPsiaSNMPAdvanced  *ptSNMPAdvanced,TPsiaResponseStatus *ptResponseStatus);

//7.4.21
typedef SCODE (* FGetQoS)(HANDLE dwIntance, TPsiaQoS  *ptQoS);
typedef SCODE (* FPutQoS)(HANDLE dwIntance, TPsiaQoS  *ptQoS,TPsiaResponseStatus *ptResponseStatus);

//7.4.22
typedef SCODE (* FGetCoSes)(HANDLE dwIntance,  TPsiaCoSList  *ptCoSList);
typedef SCODE (* FPutCoSes)(HANDLE dwIntance, TPsiaCoSList  *ptCoSList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostCoSes)(HANDLE dwIntance, TPsiaCoS  *ptCoS,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteCoSes)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.23
typedef SCODE (* FGetCoS)(HANDLE dwIntance,  TPsiaCoS  *ptCoS);
typedef SCODE (* FPutCoS)(HANDLE dwIntance, TPsiaCoS  *ptCoS,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteCoS)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.4.24
typedef SCODE (* FGetDscps)(HANDLE dwIntance, TPsiaDSCPList  *ptDSCPList);
typedef SCODE (* FPutDscps)(HANDLE dwIntance, TPsiaDSCPList  *ptDSCPList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostDscps)(HANDLE dwIntance, TPsiaDSCP  *ptDSCP,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteDscps)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.25
typedef SCODE (* FGetDscp)(HANDLE dwIntance, TPsiaDSCP  *ptDSCP);
typedef SCODE (* FPutDscp)(HANDLE dwIntance, TPsiaDSCP  *ptDSCP,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteDscp)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.4.26
typedef SCODE (* FGetDiscovery)(HANDLE dwIntance, TPsiaDiscovery  *ptDiscovery);
typedef SCODE (* FPutDiscovery)(HANDLE dwIntance, TPsiaDiscovery  *ptDiscovery,TPsiaResponseStatus *ptResponseStatus);

//7.4.27
typedef SCODE (* FGetSyslog)(HANDLE dwIntance, TPsiaSyslog  *ptSyslog);
typedef SCODE (* FPutSyslog)(HANDLE dwIntance, TPsiaSyslog  *ptSyslog,TPsiaResponseStatus *ptResponseStatus);

//7.4.28
typedef SCODE (* FGetSyslogServers)(HANDLE dwIntance, TPsiaSyslogServerList  *ptSyslogServerList);
typedef SCODE (* FPutSyslogServers)(HANDLE dwIntance, TPsiaSyslogServerList  *ptSyslogServerList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostSyslogServers)(HANDLE dwIntance, TPsiaSyslogServer  *ptSyslogServer,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSyslogServers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.4.29
typedef SCODE (* FGetSyslogServer)(HANDLE dwIntance, TPsiaSyslogServer  *ptSyslogServer);
typedef SCODE (* FPutSyslogServer)(HANDLE dwIntance, TPsiaSyslogServer  *ptSyslogServer,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteSyslogServer)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.5
typedef SCODE (* FGetIOPorts)(HANDLE dwIntance, TPsiaIOPortList  *ptIOPortList);

//7.5.1
typedef SCODE (* FGetIOPortStatuses)(HANDLE dwIntance, TPsiaIOPortStatusList  *ptIOPortStatusList);

//7.5.2
typedef SCODE (* FGetInputPorts)(HANDLE dwIntance,TPsiaIOInputPortList  *ptIOInputPortList);

//7.5.3
typedef SCODE (* FGetInputPort)(HANDLE dwIntance,TPsiaIOInputPort  *ptIOInputPort);
typedef SCODE (* FPutInputPort)(HANDLE dwIntance, TPsiaIOInputPort  *ptIOInputPort,TPsiaResponseStatus *ptResponseStatus);

//7.5.4
typedef SCODE (* FGetIOPortStatus)(HANDLE dwIntance, TPsiaIOPortStatus  *ptIOPortStatus);

//7.5.5
typedef SCODE (* FGetOutputPorts)(HANDLE dwIntance,TPsiaIOOutputPortList  *ptIOOutputPortList);

//7.5.6
typedef SCODE (* FGetOutputPort)(HANDLE dwIntance,TPsiaIOOutputPort  *ptIOOutputPort);
typedef SCODE (* FPutOutputPort)(HANDLE dwIntance,TPsiaIOOutputPort  *ptIOOutputPort,TPsiaResponseStatus *ptResponseStatus);

//7.5.7
typedef SCODE (* FSetIOportTrigger)(HANDLE dwIntance, TPsiaIOPortData *ptIOPortData,TPsiaResponseStatus *ptResponseStatus);

//7.5.8 is the same as 7.5.4

//7.6.1
typedef SCODE (* FGetAudioChannels)(HANDLE dwIntance,TPsiaAudioChannelList *ptAudioChannelList);

//7.6.2
typedef SCODE (* FGetAudioChannel)(HANDLE dwIntance,TPsiaAudioChannel  *ptAudioChannel);
typedef SCODE (* FPutAudioChannel)(HANDLE dwIntance,TPsiaAudioChannel  *ptAudioChannel,TPsiaResponseStatus *ptResponseStatus);

//7.7.1
typedef SCODE (* FGetVideoOverlayImages)(HANDLE dwIntance,TPsiaOverlayImageList *ptOverlayImageList);
typedef SCODE (* FPostVideoOverlayImages)(HANDLE dwIntance,BYTE *pbyRawImageData,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoOverlayImages)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.2
typedef SCODE (* FGetVideoOverlayImage)(HANDLE dwIntance,BYTE *pbyRawImageData);
typedef SCODE (* FPutVideoOverlayImage)(HANDLE dwIntance,BYTE *pbyRawImageData,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoOverlayImage)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.3
typedef SCODE (* FGetVideoInput)(HANDLE dwIntance,TPsiaVideoInput *ptVideoInput);

//7.7.4
typedef SCODE (* FGetVideoInputChannels)(HANDLE dwIntance,TPsiaVideoInputChannelList *ptVideoInputChannelList);

//7.7.5
typedef SCODE (* FGetVideoInputChannel)(HANDLE dwIntance,TPsiaVideoInputChannel  *ptVideoInputChannel);
typedef SCODE (* FPutVideoInputChannel)(HANDLE dwIntance,TPsiaVideoInputChannel  *ptVideoInputChannel,TPsiaResponseStatus *ptResponseStatus);


//7.7.6
typedef SCODE (* FSetVideoFocus)(HANDLE dwIntance,TPsiaFocusData *ptFocusData,TPsiaResponseStatus *ptResponseStatus);

//7.7.7
typedef SCODE (* FSetVideoIRIS)(HANDLE dwIntance,TPsiaIRISData *ptIRISData,TPsiaResponseStatus *ptResponseStatus);

//7.7.8
typedef SCODE (* FGetVideoLens)(HANDLE dwIntance,TPsiaLensStatus *ptLensStatus);

//7.7.9
typedef SCODE (* FGetVideoOverlay)(HANDLE dwIntance,TPsiaVideoOverlay *ptVideoOverlay);
typedef SCODE (* FPutVideoOverlay)(HANDLE dwIntance,TPsiaVideoOverlay *ptVideoOverlay,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoOverlay)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.7.10
typedef SCODE (* FGetVideoTextOverlays)(HANDLE dwIntance, TPsiaTextOverlayList  *ptTextOverlayList);
typedef SCODE (* FPutVideoTextOverlays)(HANDLE dwIntance, TPsiaTextOverlayList  *ptTextOverlayList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostVideoTextOverlays)(HANDLE dwIntance, TPsiaTextOverlay  *ptTextOverlay,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoTextOverlays)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.11
typedef SCODE (* FGetVideoTextOverlay)(HANDLE dwIntance, TPsiaTextOverlay  *ptTextOverlay);
typedef SCODE (* FPutVideoTextOverlay)(HANDLE dwIntance, TPsiaTextOverlay  *ptTextOverlay,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoTextOverlay)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.12
typedef SCODE (* FGetVideoImageOverlays)(HANDLE dwIntance, TPsiaImageOverlayList  *ptImageOverlayList);
typedef SCODE (* FPutVideoImageOverlays)(HANDLE dwIntance, TPsiaImageOverlayList  *ptImageOverlayList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostVideoImageOverlays)(HANDLE dwIntance, TPsiaImageOverlay  *ptImageOverlay,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoImageOverlays)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.13
typedef SCODE (* FGetVideoImageOverlay)(HANDLE dwIntance, TPsiaImageOverlay  *ptImageOverlay);
typedef SCODE (* FPutVideoImageOverlay)(HANDLE dwIntance, TPsiaImageOverlay  *ptImageOverlay,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteVideoImageOverlay)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.14
typedef SCODE (* FGetPrivacyMask)(HANDLE dwIntance, TPsiaPrivacyMask  *ptPrivacyMask);
typedef SCODE (* FPutPrivacyMask)(HANDLE dwIntance, TPsiaPrivacyMask  *ptPrivacyMask,TPsiaResponseStatus *ptResponseStatus);

//7.7.15
typedef SCODE (* FGetPrivacyMaskRegions)(HANDLE dwIntance, TPsiaPrivacyMaskRegionList  *ptPrivacyMaskRegionList);
typedef SCODE (* FPutPrivacyMaskRegions)(HANDLE dwIntance, TPsiaPrivacyMaskRegionList  *ptPrivacyMaskRegionList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostPrivacyMaskRegions)(HANDLE dwIntance, TPsiaPrivacyMaskRegion  *ptPrivacyMaskRegion,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePrivacyMaskRegions)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.7.16
typedef SCODE (* FGetPrivacyMaskRegion)(HANDLE dwIntance, TPsiaPrivacyMaskRegion  *ptPrivacyMaskRegion);
typedef SCODE (* FPutPrivacyMaskRegion)(HANDLE dwIntance, TPsiaPrivacyMaskRegion  *ptPrivacyMaskRegion,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePrivacyMaskRegion)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.8.1
typedef SCODE (* FGetSerialPorts)(HANDLE dwIntance, TPsiaSerialPortList  *ptSerialPortList);

//7.8.2
typedef SCODE (* FGetSerialPort)(HANDLE dwIntance, TPsiaSerialPort  *ptSerialPort);
typedef SCODE (* FPutSerialPort)(HANDLE dwIntance, TPsiaSerialPort  *ptSerialPort,TPsiaResponseStatus *ptResponseStatus);

//7.8.3
typedef SCODE (*FSendCommandToSerailPort)(HANDLE dwIntance,TPsiaSerialCommand *ptSerialCommand,TPsiaResponseStatus *ptResponseStatus);


//7.9.1
typedef SCODE (* FGetDiagnosticsCommands)(HANDLE dwIntance, TPsiaDiagnosticCommandList  *ptDiagnosticCommandList);
typedef SCODE (* FPostDiagnosticsCommands)(HANDLE dwIntance, TPsiaDiagnosticCommand  *ptDiagnosticCommand,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteDiagnosticsCommands)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.9.2
typedef SCODE (* FGetDiagnosticsCommand)(HANDLE dwIntance, TPsiaDiagnosticCommand  *ptDiagnosticCommand);
typedef SCODE (* FDeleteDiagnosticsCommand)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.10.1
typedef SCODE (*FGetSRTPMasterKey)(HANDLE dwIntance,BYTE *pbyData);
typedef SCODE (*FPutSRTPMasterKey)(HANDLE dwIntance,BYTE *pbyData,TPsiaResponseStatus *ptResponseStatus);

//7.10.2
typedef SCODE (*FGetDeviceCert)(HANDLE dwIntance,BYTE *pbyData);
typedef SCODE (*FPutDeviceCert)(HANDLE dwIntance,BYTE *pbyData,TPsiaResponseStatus *ptResponseStatus);

//7.11.1
typedef SCODE (* FGetUsers)(HANDLE dwIntance, TPsiaUserList  *ptUserList);
typedef SCODE (* FPutUsers)(HANDLE dwIntance, TPsiaUserList  *ptUserList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostUsers)(HANDLE dwIntance, TPsiaUser  *ptUser,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteUsers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.11.2
typedef SCODE (* FGetUser)(HANDLE dwIntance, TPsiaUser  *ptUser);
typedef SCODE (* FPutUser)(HANDLE dwIntance, TPsiaUser  *ptUser,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteUser)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.11.3
typedef SCODE (*FGetAAACert)(HANDLE dwIntance,BYTE *pbyData);
typedef SCODE (*FPutAAACert)(HANDLE dwIntance,BYTE *pbyData,TPsiaResponseStatus *ptResponseStatus);

//7.11.4
typedef SCODE (* FGetAdminAccesses)(HANDLE dwIntance, TPsiaAdminAccessProtocolList  *ptAdminAccessProtocolList);
typedef SCODE (* FPutAdminAccesses)(HANDLE dwIntance, TPsiaAdminAccessProtocolList  *ptAdminAccessProtocolList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostAdminAccesses)(HANDLE dwIntance, TPsiaAdminAccessProtocol  *ptAdminAccessProtocol,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteAdminAccesses)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.11.5
typedef SCODE (* FGetAdminAccess)(HANDLE dwIntance, TPsiaAdminAccessProtocol  *ptAdminAccessProtocol);
typedef SCODE (* FPutAdminAccess)(HANDLE dwIntance, TPsiaAdminAccessProtocol  *ptAdminAccessProtocol,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteAdminAccess)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.12.1
typedef SCODE (*FGetStreamingStatus)(HANDLE dwIntance,TPsiaStreamingStatus *ptStreamingStatus);

//7.12.2
typedef SCODE (* FGetStreamingChannels)(HANDLE dwIntance, TPsiaStreamingChannelList *ptStreamingChannelList);
typedef SCODE (* FPutStreamingChannels)(HANDLE dwIntance, TPsiaStreamingChannelList *ptStreamingChannelList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostStreamingChannels)(HANDLE dwIntance, TPsiaStreamingChannel  *ptStreamingChannel,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteStreamingChannels)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.12.3
typedef SCODE (* FGetStreamingChannel)(HANDLE dwIntance, TPsiaStreamingChannel *ptStreamingChannel);
typedef SCODE (* FPutStreamingChannel)(HANDLE dwIntance, TPsiaStreamingChannel *ptStreamingChannel,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteStreamingChannel)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.12.4
typedef SCODE (* FGetStreamingSessionStatus)(HANDLE dwIntance,TPsiaStreamingSessionStatus *ptStreamingSessionStatus);

//The interfaces of 7.12.5, 7.12.6, 7.12.7 are not be defined. 

//7.13.1
typedef SCODE (* FGetPTZhannels)(HANDLE dwIntance, TPsiaPTZChannelList *ptPTZChannelList);
typedef SCODE (* FPutPTZhannels)(HANDLE dwIntance, TPsiaPTZChannelList *ptPTZChannelList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostPTZhannels)(HANDLE dwIntance, TPsiaPTZChannel  *ptPTZChannel,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePTZhannels)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.2
typedef SCODE (* FGetPTZhannel)(HANDLE dwIntance, TPsiaPTZChannel *ptPTZChannel);
typedef SCODE (* FPutPTZhannel)(HANDLE dwIntance, TPsiaPTZChannel *ptPTZChannel,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePTZhannel)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.3
typedef SCODE (* FSetPTZHomePosition)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.4
typedef SCODE (* FSetPTZContinuous)(HANDLE dwIntance,TPsiaPTZData *ptPTZData,TPsiaResponseStatus *ptResponseStatus);

//7.13.5
typedef SCODE (* FSetPTZMomentary)(HANDLE dwIntance,TPsiaPTZData *ptPTZData,TPsiaResponseStatus *ptResponseStatus);

//7.13.6
typedef SCODE (* FSetPTZRelative)(HANDLE dwIntance,TPsiaPTZData *ptPTZData,TPsiaResponseStatus *ptResponseStatus);

//7.13.7
typedef SCODE (* FSetPTZAbsolute)(HANDLE dwIntance,TPsiaPTZData *ptPTZData,TPsiaResponseStatus *ptResponseStatus);

//7.13.8
typedef SCODE (* FSetPTZDigital)(HANDLE dwIntance,TPsiaPTZData *ptPTZData,TPsiaResponseStatus *ptResponseStatus);


//7.13.9
typedef SCODE (* FGetPTZStatus)(HANDLE dwIntance,TPsiaPTZStatus *ptPTZStatus);

//7.13.10
typedef SCODE (* FGetPTZPresets)(HANDLE dwIntance, TPsiaPTZPresetList *ptPTZPresetList);
typedef SCODE (* FPutPTZPresets)(HANDLE dwIntance, TPsiaPTZPresetList *ptPTZPresetList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostPTZPresets)(HANDLE dwIntance, TPsiaPTZPreset  *ptPTZPreset,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePTZPresets)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.11
typedef SCODE (* FGetPTZPreset)(HANDLE dwIntance, TPsiaPTZPreset *ptPTZPreset);
typedef SCODE (* FPutPTZPreset)(HANDLE dwIntance, TPsiaPTZPreset *ptPTZPreset,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePTZPreset)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.12 This interface may be rethinked?
typedef SCODE (* FGotoPresetPosition)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.13
typedef SCODE (* FGetPTZPatrols)(HANDLE dwIntance,TPsiaPTZPatrolList *ptPTZPatrolList);
typedef SCODE (* FPutPTZPatrols)(HANDLE dwIntance,TPsiaPTZPatrolList *ptPTZPatrolList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostPTZPatrols)(HANDLE dwIntance,TPsiaPTZPatrol *ptPTZPatrol,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePTZPatrols)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.14
typedef SCODE (* FGetPTZPatrolStatuses)(HANDLE dwIntance,TPsiaPTZPatrolStatusList ptPTZPatrolStatusList);

//7.13.15
typedef SCODE (* FGetPTZPatrol)(HANDLE dwIntance,TPsiaPTZPatrol *ptPTZPatrol);
typedef SCODE (* FPutPTZPatrol)(HANDLE dwIntance,TPsiaPTZPatrol *ptPTZPatrol,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeletePTZPatrol)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.16
typedef SCODE (*FStartPTZPatrol)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.17
typedef SCODE (*FStopPTZPatrol)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.18
typedef SCODE (*FPausePTZPatrol)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.13.19
typedef SCODE (* FGetPTZPatrolStatus)(HANDLE dwIntance,TPsiaPTZPatrolStatus ptPTZPatrolStatus);

//7.13.20
typedef SCODE (*FGetPTZPatrolSchedule)(HANDLE dwIntance,TPsiaTimeBlockList *ptTimeBlockList);	
typedef SCODE (*FPutPTZPatrolSchedule)(HANDLE dwIntance,TPsiaTimeBlockList *ptTimeBlockList,TPsiaResponseStatus *ptResponseStatus);

//7.14
typedef SCODE (*FGetMotionDetctions)(HANDLE dwIntance,TPsiaMotionDetectionList *ptMotionDetectionList);

//7.14.1
typedef SCODE (*FGetMotionDetction)(HANDLE dwIntance,TPsiaMotionDetection *ptMotionDetection);
typedef SCODE (*FPutMotionDetction)(HANDLE dwIntance,TPsiaMotionDetection *ptMotionDetection,TPsiaResponseStatus *ptResponseStatus);

//7.14.2
typedef SCODE (* FGetMotionDetectionRegions)(HANDLE dwIntance,TPsiaMotionDetectionRegionList *ptMotionDetectionRegionList);
typedef SCODE (* FPutMotionDetectionRegions)(HANDLE dwIntance,TPsiaMotionDetectionRegionList *ptMotionDetectionRegionList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostMotionDetectionRegions)(HANDLE dwIntance,TPsiaMotionDetectionRegion *ptMotionDetectionRegion,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteMotionDetectionRegions)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.14.3
typedef SCODE (* FGetMotionDetectionRegion)(HANDLE dwIntance,TPsiaMotionDetectionRegion *ptMotionDetectionRegion);
typedef SCODE (* FPutMotionDetectionRegion)(HANDLE dwIntance,TPsiaMotionDetectionRegion *ptMotionDetectionRegion,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteMotionDetectionRegion)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15
typedef SCODE (* FGetEventNotification)(HANDLE dwIntance,TPsiaEventNotification *ptEventNotification);
typedef SCODE (* FPutEventNotification)(HANDLE dwIntance,TPsiaEventNotification *ptEventNotification,TPsiaResponseStatus *ptResponseStatus);

//7.15.1
typedef SCODE (* FGetEventTriggers)(HANDLE dwIntance,TPsiaEventTriggerList *ptEventTriggerList);
typedef SCODE (* FPutEventTriggers)(HANDLE dwIntance,TPsiaEventTriggerList *ptEventTriggerList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostEventTriggers)(HANDLE dwIntance,TPsiaEventTrigger *ptEventTrigger,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteEventTriggers)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.2
typedef SCODE (* FGetEventTrigger)(HANDLE dwIntance,TPsiaEventTrigger *ptEventTrigger);
typedef SCODE (* FPutEventTrigger)(HANDLE dwIntance,TPsiaEventTrigger *ptEventTrigger,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteEventTrigger)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.3
typedef SCODE (* FGetEventTriggerNontifications)(HANDLE dwIntance,PsiaEventTriggerNotificationList *ptEventTriggerNotificationList);
typedef SCODE (* FPutEventTriggerNontifications)(HANDLE dwIntance,PsiaEventTriggerNotificationList *ptEventTriggerNotificationList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostEventTriggerNontifications)(HANDLE dwIntance,PsiaEventTriggerNotification *ptEventTriggerNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteEventTriggerNontifications)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.4
typedef SCODE (* FGetEventTriggerNontification)(HANDLE dwIntance,PsiaEventTriggerNotification *ptEventTriggerNotification);
typedef SCODE (* FPutEventTriggerNontification)(HANDLE dwIntance,PsiaEventTriggerNotification *ptEventTriggerNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteEventTriggerNontification)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.5
typedef SCODE (* FGetEventSchedule)(HANDLE dwIntance,TPsiaEventSchedule *ptEventSchedule);
typedef SCODE (* FPutEventSchedule)(HANDLE dwIntance,TPsiaEventSchedule *ptEventSchedule,TPsiaResponseStatus *ptResponseStatus);

//7.15.6
typedef SCODE (* FGetEventNotificationMethods)(HANDLE dwIntance,TPsiaEventNotificationMethods *ptEventNotificationMethods);
typedef SCODE (* FPutEventNotificationMethods)(HANDLE dwIntance,TPsiaEventNotificationMethods *ptEventNotificationMethods,TPsiaResponseStatus *ptResponseStatus);


//7.15.7
typedef SCODE (* FGetMailingNontifications)(HANDLE dwIntance,TPsiaMailingNotificationList ptMailingNotificationList);
typedef SCODE (* FPutMailingNontifications)(HANDLE dwIntance,TPsiaMailingNotificationList ptMailingNotificationList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostMailingNontifications)(HANDLE dwIntance,TPsiaMailingNotification ptMailingNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteMailingNontifications)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.8
typedef SCODE (* FGetMailingNontification)(HANDLE dwIntance,TPsiaMailingNotification ptMailingNotification);
typedef SCODE (* FPutMailingNontification)(HANDLE dwIntance,TPsiaMailingNotification ptMailingNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteMailingNontification)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);


//7.15.9
typedef SCODE (* FGetFTPNontifications)(HANDLE dwIntance,TPsiaFTPNotificationList *ptFTPNotificationList);
typedef SCODE (* FPutFTPNontifications)(HANDLE dwIntance,TPsiaFTPNotificationList *ptFTPNotificationList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostFTPNontifications)(HANDLE dwIntance,TPsiaFTPNotification *ptFTPNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteFTPNontifications)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.10
typedef SCODE (* FGetFTPNontification)(HANDLE dwIntance,TPsiaFTPNotification *ptFTPNotification);
typedef SCODE (* FPutFTPNontification)(HANDLE dwIntance,TPsiaFTPNotification *ptFTPNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteFTPNontification)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.11
typedef SCODE (* FGetHTTPHostNontifications)(HANDLE dwIntance,TPsiaHTTPHostNotificationList *ptHTTPHostNotificationList);
typedef SCODE (* FPutHTTPHostNontifications)(HANDLE dwIntance,TPsiaHTTPHostNotificationList *ptHTTPHostNotificationList,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FPostHTTPHostNontifications)(HANDLE dwIntance,TPsiaHTTPHostNotification *ptHTTPHostNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteHTTHostPNontifications)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.12
typedef SCODE (* FGetHTTPHostNontification)(HANDLE dwIntance,TPsiaHTTPHostNotification *ptHTTPHostNotification);
typedef SCODE (* FPutHTTPHostNontification)(HANDLE dwIntance,TPsiaHTTPHostNotification *ptHTTPHostNotification,TPsiaResponseStatus *ptResponseStatus);
typedef SCODE (* FDeleteHTTHostPNontification)(HANDLE dwIntance,TPsiaResponseStatus *ptResponseStatus);

//7.15.13
typedef SCODE (* FGetEventNotificationAlert)(HANDLE dwIntance,TPsiaEventNotificationAlert *ptEventNotificationAlert);

//7.15.14 ??
typedef SCODE (* FPostEventNotificationAlert)(HANDLE dwIntance,TPsiaEventNotificationAlert *ptEventNotificationAlert,TPsiaResponseStatus *ptResponseStatus);



typedef SCODE (* FGetRootServiceIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetRootServiceIndexr) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetRootServiceDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetSystemIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSystemDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetSystemNetworkIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSystemNetworkDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetSystemIOIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSystemIODescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);


typedef SCODE (* FGetSystemAudioIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSystemAudioDescription)  (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);

typedef SCODE (* FGetSystemVideoIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSystemVideoDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetStreamingIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetStreamingDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetPTZIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetPTZDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetCustomMotionDetectionIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetCustomMotionDetectionDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetSecurityIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSecurityDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);
typedef SCODE (* FGetSecurityAAAIndex) (HANDLE dwIntance,TPsiaResource *ptResource);
typedef SCODE (* FGetSecurityAAADescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);

typedef SCODE  (* FGetServiceIndex ) (HANDLE dwIntance,TPsiaResource *ptResource);

typedef SCODE (* FGetServiceDescription) (HANDLE dwIntance,TPsiaResourceDescription *ptResourceDescription);


// reload function
typedef SCODE (* FReload)(HANDLE dwIntance);
typedef struct psiad_initopt
{
  // reload function
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


  
  
  CHAR* szSckPath;
  HANDLE hAppObject;

}TPsiadInitOpt;

SCODE Psiad_Initial(HANDLE *phPsiadObj, TPsiadInitOpt  tPsiadInitOpt);
SCODE Psiad_Release(HANDLE *phPsiadObj);
SCODE Psiad_Start(HANDLE hPsiadObj);
SCODE Psiad_Stop(HANDLE hPsiadObj);
SCODE Psiad_Reload(HANDLE hPsiadObj);
#endif