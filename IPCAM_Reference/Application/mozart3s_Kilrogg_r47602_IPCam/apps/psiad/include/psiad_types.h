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

#ifndef _PSIAD_TYPES_H_
#define _PSIAD_TYPES_H_

#include "typedef.h"


#define LONG_STR_LEN 128
#define SHORT_STR_LEN 64
#define MAX_OCCURS_NUM 1024 
#define ERROR_CODE_SIZE      	128
//! enumeration for status code
typedef enum psia_status_code_index{
	est_no_error=0,
	est_device_busy = 1,
	est_device_error= 2,
	est_invalid_operation=3,
	est_invalid_xml_format=4,
	est_invalid_xml_content=5,
	est_reboot_required=6,
	est_num
} EPsiaStatusCodeIndex;

typedef struct psia_StatusCodeTable{
  DWORD dwStatusCode;
  CHAR szStatusString[ERROR_CODE_SIZE];

} TPsiaStatusCodeTable;




typedef enum psia_messgae_process_result {
	eomsgprocres_success = 0,
	eomsgprocres_error,
} EPsiaMsgProcessResult;

typedef struct psia_response_status
{
  CHAR szRequestURL[LONG_STR_LEN];
  EPsiaStatusCodeIndex eStatusCodeIndex;
  CHAR szID[SHORT_STR_LEN];

}TPsiaResponseStatus;



typedef struct psia_resource
{
  CHAR szName[SHORT_STR_LEN];
  CHAR szVersion[SHORT_STR_LEN];
  CHAR szType[SHORT_STR_LEN];
  CHAR szDescription[SHORT_STR_LEN];
  struct psia_resource_list{
    struct psia_resource *ptResource;
    DWORD dwResourceListNum;
  } tResourceList;

}TPsiaResource;

typedef struct psia_query_str_parameter
{
  CHAR szName[SHORT_STR_LEN];
  CHAR szType[SHORT_STR_LEN];
  CHAR szDescription[SHORT_STR_LEN];

}TPsiaQueryStrParameter;

typedef struct psia_query_str_parameter_list
{
  TPsiaQueryStrParameter *ptQueryStrParameter;
  DWORD dwQueryStrParameterListNum;

}TPsiaQueryStrParameterList;

typedef struct psia_url_parameters
{
  TPsiaQueryStrParameterList tQueryStrParameterList;
  CHAR szInBoundData[LONG_STR_LEN];
  CHAR szReturnResult[SHORT_STR_LEN];
  CHAR szFunction[SHORT_STR_LEN]; 
  CHAR szNotes[SHORT_STR_LEN];  
}TPisaURLParameters;


typedef struct psia_resource_description
{  
  CHAR szName[SHORT_STR_LEN];
  CHAR szVersion[SHORT_STR_LEN];
  CHAR szType[SHORT_STR_LEN];
  CHAR szDescription[SHORT_STR_LEN];
  CHAR szNotes[SHORT_STR_LEN];
  TPisaURLParameters tURLParametersGet;
  TPisaURLParameters tURLParametersPut;
  TPisaURLParameters tURLParametersPost;
  TPisaURLParameters tURLParametersDelete;

}TPsiaResourceDescription;


typedef struct psia_device_info
{
  CHAR szDeviceName[SHORT_STR_LEN];
  CHAR szDeviceUUID[LONG_STR_LEN];
  CHAR szDeviceDescription[LONG_STR_LEN];
  CHAR szDeviceLocation[SHORT_STR_LEN];
  CHAR szSystemContact[SHORT_STR_LEN];
  CHAR szModel[SHORT_STR_LEN];
  CHAR szSerialNumber[SHORT_STR_LEN];
  CHAR szMacAddress[SHORT_STR_LEN];
  CHAR szFirmwareVersion[SHORT_STR_LEN];
  CHAR szFirmwareReleasedDate[SHORT_STR_LEN];
  CHAR szLogicVersion[SHORT_STR_LEN];
  CHAR szLogicReleasedDate[SHORT_STR_LEN];
  CHAR szBootVersion[SHORT_STR_LEN];
  CHAR szBootReleasedDate[SHORT_STR_LEN];
  CHAR szRescueVersion[SHORT_STR_LEN];
  CHAR szRescueReleasedDate[SHORT_STR_LEN];
  CHAR szHardwareVersion[SHORT_STR_LEN];
  CHAR szSystemObjectID[SHORT_STR_LEN];
} TPsiaDeviceInfo;

typedef struct psia_temperature
{
    CHAR szTempSensorDescription[LONG_STR_LEN];
    FLOAT fTemperature;
} TPsiaTemperature;

typedef struct psia_fan
{
    CHAR szFanDescription[LONG_STR_LEN];
    DWORD dwFanSpeed;
  
} TPsiaFan;

typedef struct psia_pressure
{	
   CHAR szPressureSensorDescription[LONG_STR_LEN];
   DWORD dwPressure;
} TPsiaPressure;

typedef struct psia_tamper
{
  CHAR szTamperSensorDescription[LONG_STR_LEN];
  BOOL bTamper;  
} TPsiaTamper;

typedef struct psia_cpu
{
  CHAR szCpuDescription[LONG_STR_LEN];
  DWORD dwCPUUtilization;
} TPsiaCPU;

typedef struct psia_memory
{
  CHAR szMemoryDescription[LONG_STR_LEN];
  FLOAT fMemoryUsage;
  FLOAT fMemoryAvailable;
} TPsiaMemory;

typedef struct psia_temperature_list
{
  TPsiaTemperature *ptTemperature;
  DWORD dwTemperatureListNum;
}TPsiaTemperatureList;

typedef struct psia_fan_list
{
  TPsiaFan *ptFan;
  DWORD dwFanListNum;

}TPsiaFanList;

typedef struct psia_pressure_list
{
  TPsiaPressure *ptPressure;
  DWORD dwPressureListNum;
}TPsiaPressureList;

typedef struct psia_tamper_list
{
  TPsiaTamper *ptTamper;
  DWORD dwTamperListNum;
}TPsiaTamperList;

typedef struct psia_cpu_list
{
  TPsiaCPU *ptCPU;
  DWORD dwCPUListNum;
}TPsiaCPUList;

typedef struct psia_memory_list
{
  TPsiaMemory *ptMemory;
  DWORD dwMemoryListNum;
}TPsiaMemoryList;

typedef struct psia_device_status
{
  CHAR szCurrentDeviceTime[SHORT_STR_LEN];
  DWORD dwDeviceUpTime;
  TPsiaTemperatureList tTemperatureList;
  TPsiaFanList tFanList;
  TPsiaPressureList tPressureList;
  TPsiaTamperList tTamperList;
  TPsiaCPUList tCPUList;
  TPsiaMemoryList tMemoryList;
  
  DWORD dwOpenFileHandles;
  
} TPsiaDeviceStatus;


typedef struct psia_time
{
  CHAR szTimeMode[SHORT_STR_LEN];
  CHAR szLocalTime[SHORT_STR_LEN];
  CHAR szTimeZone[SHORT_STR_LEN];
} TPsiaTime;


typedef struct psia_ntpserver
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szAddressingFormatType[SHORT_STR_LEN];
  CHAR szHostName[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNum;  
} TPsiaNTPServer;

typedef struct psia_ntpserver_list
{
  TPsiaNTPServer *ptNTPServer;
  DWORD dwListNum;  

} TPsiaNTPServerList;
typedef struct psia_logtrigger
{
  CHAR szSeverity[SHORT_STR_LEN];
} TPsiaLogTrigger;

typedef struct psia_locallog
{

  DWORD dwMaxEntries;
} TPsiaLocalLog;

typedef struct psia_logging
{
    TPsiaLogTrigger tPsiaLogTrigger;
    TPsiaLocalLog   tPsiaLocalLog;

} TPsiaLogging;

typedef struct psia_log_message
{
  DWORD dwLogNum;
  CHAR 	szDateTime[SHORT_STR_LEN];
  DWORD dwSeverity;
  CHAR  szEventID[SHORT_STR_LEN];
  CHAR  szMessage[LONG_STR_LEN];
  

} TPsiaLogMessage;

typedef struct psia_log_message_list
{
  TPsiaLogMessage *ptLogMessage;
  DWORD dwListNum; 

} TPsiaLogMessageList;


typedef struct psia_storage_volume
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szVolumeName[SHORT_STR_LEN];
  CHAR szVolumePath[SHORT_STR_LEN];
  CHAR szVolumeDescription[LONG_STR_LEN];
  CHAR szVolumeType[SHORT_STR_LEN];
  CHAR szStorgaeDescription[LONG_STR_LEN];
  CHAR szStorageLocation[SHORT_STR_LEN];
  CHAR szStorgaeType[SHORT_STR_LEN];
  FLOAT fCapacity;

} TPsiaStorageVolume;


typedef struct psia_storage_volume_list
{
  TPsiaStorageVolume *ptStorageVolume;
  DWORD dwListNum; 

} TPsiaStorageVolumeList;

typedef struct psia_storage_vloume_status
{
  FLOAT fFreeSpace;

} TPsiaStorageVolumeStatus;


typedef struct psia_storage_file
{
    CHAR szID[SHORT_STR_LEN];
    CHAR szFileName[SHORT_STR_LEN];
    CHAR szFileTimeStamp[SHORT_STR_LEN];
    FLOAT fFileSize;
} TPsiaStorageFile;

typedef struct psia_storage_file_list
{
  TPsiaStorageFile *ptStorageFile;
  DWORD dwListNum;
  
} TPsiaStorageFileList;

typedef struct psia_default_gateway
{
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];  
} TPsiaDefaultGateway;

typedef struct psia_primary_dns
{
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];  
} TPsiaPrimaryDNS;

typedef struct psia_secondary_dns
{
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];  
} TPsiaSecondaryDNS;

typedef struct psia_ip_Address
{
  CHAR szIPVersion[SHORT_STR_LEN];
  CHAR szAddressingType[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szSubnetMask[SHORT_STR_LEN];
  CHAR szIPv6Address[SHORT_STR_LEN];
  CHAR szBitMask[SHORT_STR_LEN];
  TPsiaDefaultGateway tDefaultGateway;
  TPsiaPrimaryDNS tPrimaryDNS;
  TPsiaSecondaryDNS tSecondaryDNS;
  
} TPsiaIPAddress;

typedef struct psia_encryption_key
{
  CHAR szEncryptionKey[LONG_STR_LEN];
  
} TPsiaEncryptionKey;

typedef struct psia_encryption_key_list
{
  TPsiaEncryptionKey *ptEncryptionKey;
  DWORD dwListNum;

} TPsiaEncryptionKeyList;


typedef struct psia_wep
{
  CHAR szAuthenticationType[SHORT_STR_LEN];
  DWORD dwDefaultTransmitKeyIndex;
  DWORD dwWepKeyLength;
  TPsiaEncryptionKeyList tEncryptionKeyList;
} TPsiaWEP;

typedef struct psia_wpa
{
  CHAR szAlgorithmType[SHORT_STR_LEN];
  CHAR szSharedKey[SHORT_STR_LEN];
  
} TPsiaWPA;


typedef  struct psia_wireless_security
{
  CHAR szSecurityMode[SHORT_STR_LEN];
  TPsiaWEP tWEP;
  TPsiaWPA tWPA;  

} TPsiaWirelessSecurity;

typedef struct psia_wireless
{
  BOOL bEnabled;
  CHAR szWirelessNetworkMode[SHORT_STR_LEN];
  CHAR szChannel[SHORT_STR_LEN];
  CHAR szSSID[LONG_STR_LEN];
  BOOL bWmmEnabled;
  TPsiaWirelessSecurity tWirelessSecurity;

  
  
} TPsiaWireless;


typedef struct psia_ieee_802_1x
{
  BOOL bEnabled;
  CHAR szAuthenticationProtocolType[SHORT_STR_LEN];
  CHAR szInnerTTLSAuthenticationMethod[SHORT_STR_LEN];
  CHAR szInnerEAPProtocolType[SHORT_STR_LEN];
  BOOL bValidateServerEnabled;
  CHAR szUserName[SHORT_STR_LEN];
  CHAR szPassword[SHORT_STR_LEN];
  CHAR szAnonymousID[SHORT_STR_LEN];
  BOOL bAutoPACProvisioningEnabled;
  
}TPsiaIEEE8021X;


typedef struct psia_address_range
{

 CHAR szStartIPAddress[SHORT_STR_LEN];  
 CHAR szEndIPAddress[SHORT_STR_LEN];
 CHAR szStartIPv6Address[SHORT_STR_LEN]; 
 CHAR szEndIPv6Address[SHORT_STR_LEN]; 

} TPsiaAddressRange;

typedef struct psia_address_mask
{
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  CHAR szBitMask[SHORT_STR_LEN];
  
}TPsiaAddressMask;

typedef struct psia_ip_filter_address
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szPermissionType[SHORT_STR_LEN];
  CHAR szAddressFilterType[SHORT_STR_LEN];
  TPsiaAddressRange tAddressRange; 
  TPsiaAddressMask  tAddressMask;

} TPsiaIPFilterAddress;

typedef struct psia_ip_filter_address_list
{
  TPsiaIPFilterAddress *ptIPFilterAddress;
  DWORD dwListNum;
  
} TPsiaIPFilterAddressList;

typedef struct psia_ip_filter
{
  BOOL bEnabled;
  CHAR szPermissionType[SHORT_STR_LEN];
  TPsiaIPFilterAddressList tIPFilterAddressList;
  
} TPsiaIPFilter; 



typedef struct psia_receiver_address
{
  
  CHAR szAddressingFormatType[SHORT_STR_LEN];
  CHAR szHostName[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  

}TPsiaReceiverAddress;

typedef struct psia_snmp_trap_receiver
{
  CHAR szID[SHORT_STR_LEN];
  TPsiaReceiverAddress tReceiverAddress;
  CHAR szNotificationType[SHORT_STR_LEN];
  CHAR szCommunityString[SHORT_STR_LEN];
  

}TPsiaSNMPTrapReceiver;

typedef struct psia_snmp_trapReceiverList
{
 TPsiaSNMPTrapReceiver *ptSNMPTrapReceiver;
 DWORD dwListNum;
} TPsiaSNMPTrapReceiverList;

typedef struct psia_snmp_v2c
{
  BOOL bNotificationEnabled;
  TPsiaSNMPTrapReceiverList tSNMPTrapReceiverList;

}TPsiaSNMPV2c;

typedef struct psia_snmp_user
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szUserName[SHORT_STR_LEN];
  CHAR szRemoteEngineID[LONG_STR_LEN];
  CHAR szSnmpAuthenticationMethod[SHORT_STR_LEN];
  CHAR szSnmpAuthenticationKey[LONG_STR_LEN];
  CHAR szSnmpAuthenticationPassword[LONG_STR_LEN];
  CHAR szSnmpPrivacyMethod[SHORT_STR_LEN];
  CHAR szSnmpPrivacyKey[LONG_STR_LEN];
  CHAR szSnmpPrivacyPassword[LONG_STR_LEN];
  

} TPsiaSNMPUser;

typedef struct psia_snmp_user_list
{
  TPsiaSNMPUser *ptSNMPUser;
  DWORD dwListNum;

} TPsiaSNMPUserList;


typedef struct psia_oid
{
  CHAR szOIDSubtree[LONG_STR_LEN];
  CHAR szFilterAction[SHORT_STR_LEN];

} TPsiaOID;


typedef struct psia_oid_subtree_list
{
  
  TPsiaOID *ptOID;
  DWORD dwListNum;
  

}TPsiaOIDSubtreeList;

typedef struct psia_snmp_notification_filter 
{
  
  CHAR szID[SHORT_STR_LEN];
  CHAR szFilterName[SHORT_STR_LEN];
  TPsiaOIDSubtreeList tOIDSubtreeList;

} TPsiaSNMPNotificationFilter; 



typedef struct psia_snmp_notification_filter_list
{
  TPsiaSNMPNotificationFilter *ptSNMPNotificationFilter;
  DWORD dwListNum;
  
} TPsiaSNMPNotificationFilterList;

 


typedef struct psia_snmp_notification_receiver
{
    TPsiaReceiverAddress tReceiverAddress;
    CHAR szNotificationType[SHORT_STR_LEN];
    CHAR szUserID[SHORT_STR_LEN];
    CHAR szSecurityType[SHORT_STR_LEN];
    BOOL bFilterEnabled;
    DWORD dwFilterName;
    DWORD dwTimeout;
    DWORD dwRetries;


} TPsiaSNMPNotificationReceiver;

typedef struct psia_snmp_notification_receiver_list
{
  TPsiaSNMPNotificationReceiver *ptNotificationReceiver;
  
  DWORD dwListNum;

}TPsiaSNMPNotificationReceiverList;

typedef struct psia_snmp_advanced
{
  CHAR szLocalEngineID[LONG_STR_LEN];
  BOOL bAuthenticationNotificationEnabled;
  BOOL bNotificationEnabled;
  TPsiaSNMPUserList tSNMPUserList;
  TPsiaSNMPNotificationFilterList tSNMPNotificationFilterList;
  TPsiaSNMPNotificationReceiverList tSNMPNotificationReceiverList;
 
}TPsiaSNMPAdvanced;

typedef enum psia_snmp_choice
{
    esnmpv2c=0,
    esnmpadvanced=1,
    esnmpunkown
} EPsiaSNMPChoice;

typedef struct psia_snmp
{
  EPsiaSNMPChoice eSNMPChoice;
  TPsiaSNMPV2c tSNMPV2c;
  TPsiaSNMPAdvanced tSNMPAdvanced;
  
}TPsiaSNMP; 

typedef struct  psia_cos
{
  
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  DWORD dwPriority;
  CHAR szVlanID[SHORT_STR_LEN];
  CHAR szTrafficType[SHORT_STR_LEN];

}TPsiaCoS;



typedef struct psia_cos_list 
{
  TPsiaCoS tCos;
  DWORD dwListNum;
  
}TPsiaCoSList;


typedef struct psia_dscp
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  DWORD dwPriorityValue;
  CHAR szTrafficType[SHORT_STR_LEN];
  
}TPsiaDSCP;

typedef struct psia_dscp_list 
{
  TPsiaDSCP tDscp;
  DWORD dwListNum;
  
} TPsiaDSCPList;

typedef struct psia_qos
{
   TPsiaCoSList tCoSList;
   TPsiaDSCPList tDSCPList;
 
}TPsiaQoS; 


typedef struct psia_upnp
{
  BOOL bEnabled;

}TPsiaUPnP;

typedef struct psia_zero_conf
{
  BOOL bEnabled;

}TPsiaZeroConf;

typedef struct psia_multicast_discovery
{
  BOOL bEnabled;
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  DWORD dwTTL;
  
}TPsiaMulticastDiscovery;

typedef struct psia_discovery
{
  TPsiaUPnP tUPnP;
  TPsiaZeroConf tZeroConf;
  TPsiaMulticastDiscovery tMulticastDiscovery;
}TPsiaDiscovery;

typedef struct psia_syslog_server
{
  
  CHAR szID[SHORT_STR_LEN];
  CHAR szAddressingFormatType[SHORT_STR_LEN];
  CHAR szHostName[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  CHAR szFacilityType[SHORT_STR_LEN];
  CHAR szSeverity[SHORT_STR_LEN];
 
  
}TPsiaSyslogServer;
typedef struct psia_syslog_server_list
{
  TPsiaSyslogServer *ptSyslogServer;
  DWORD dwListNum;

}TPsiaSyslogServerList;

typedef struct psia_syslog 
{
  BOOL bEnabled;
  TPsiaSyslogServerList tSyslogServerList;
  

}TPsiaSyslog;


typedef struct psia_network_interface
{
  CHAR szID[SHORT_STR_LEN];
  TPsiaIPAddress tIPAddress;
  TPsiaWireless tWireless;
  TPsiaIEEE8021X tIEEE8021X;
  TPsiaIPFilter  tIPFilter;
  TPsiaSNMP tSNMP;
  TPsiaQoS  tQoS;
  TPsiaDiscovery tDiscovery;
  TPsiaSyslog	tSyslog;  
} TPsiaNetworkInterface;

typedef struct psia_network_interface_list
{
   TPsiaNetworkInterface *ptNetworkInterface;
   DWORD dwListNum;
  
}TPsiaNetworkInterfaceList;

typedef struct psia_io_input_port
{
   CHAR szID[SHORT_STR_LEN];
   CHAR szTriggerType[SHORT_STR_LEN];


}TPsiaIOInputPort;
typedef struct psia_io_input_port_list
{
  TPsiaIOInputPort *ptIOInputPort;
  DWORD dwListNum;

}TPsiaIOInputPortList;


typedef struct psia_power_on_state
{
  CHAR szDefaultState[SHORT_STR_LEN];
  CHAR szOutputState[SHORT_STR_LEN];
  DWORD dwPulseDuration;

}TPsiaPowerOnState;

typedef struct psia_manual_control
{
  CHAR szActionMapping[SHORT_STR_LEN];

}TPsiaManualControl;

typedef struct psia_io_output_port
{
  DWORD szID[SHORT_STR_LEN];
  TPsiaPowerOnState tPowerOnState;
  TPsiaManualControl tManualControl;

}TPsiaIOOutputPort;

typedef struct  psia_io_output_port_list
{
  TPsiaIOOutputPort *ptIOOutputPort;
  DWORD dwListNum;
  

}TPsiaIOOutputPortList;


typedef struct psia_io_port_list
{
  TPsiaIOInputPortList tIOInputPortList;
  TPsiaIOOutputPortList tIOOutputPortList;
  

}TPsiaIOPortList;


typedef struct psia_io_port_status
{
    CHAR szIOPortID[SHORT_STR_LEN];
    CHAR szIOPortType[SHORT_STR_LEN];
    CHAR szIOState[SHORT_STR_LEN];
}TPsiaIOPortStatus;

typedef struct psia_io_port_status_list
{
  TPsiaIOPortStatus *ptIOPortStatus;
  DWORD dwListNum;
}TPsiaIOPortStatusList;

typedef struct psia_io_port_data
{
    CHAR szOutputState[SHORT_STR_LEN];
  DWORD dwPulseDuration;

}TPsiaIOPortData;


typedef struct psia_audio_channel
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  CHAR szAudioMode[SHORT_STR_LEN];
  BOOL bMicrophoneEnabled;
  CHAR szMicrophoneSource[SHORT_STR_LEN];
  DWORD dwMicrophoneVolume;
  BOOL bSpeakerEnabled;
  DWORD dwSpeakerVolume;
  
}TPsiaAudioChannel;

typedef struct psia_audio_channel_list
{
  TPsiaAudioChannel *ptAudioChannel;
  DWORD dwListNum;
  
}TPsiaAudioChannelList;


typedef struct psia_overlay_image
{
    CHAR szID[SHORT_STR_LEN];
    CHAR szImageType[SHORT_STR_LEN];
    DWORD dwImageWidth;
    DWORD dwImageHeight;

}TPsiaOverlayImage;


typedef struct psia_overlay_image_list
{
  TPsiaOverlayImage *ptOverlayImage;
  DWORD dwListNum;

}TPsiaOverlayImageList;

typedef struct psia_exposure
{
  
  DWORD dwExposureTarget;
  DWORD dwExposureAutoMin;
  DWORD dwExposureAutoMax;
  

}TPsiaExposure;


typedef struct psia_region_coordinates
{
  DWORD dwPositionX;
  DWORD dwPositionY;

}TPsiaRegionCoordinates;


typedef struct psia_region_coordinates_list
{
  TPsiaRegionCoordinates *ptRegionCoordinates;
  DWORD dwListNum;

}TPsiaRegionCoordinatesList;



typedef struct psia_gain_window
{
  TPsiaRegionCoordinatesList tRegionCoordinatesList;
}TPsiaGainWindow;

typedef struct psia_lens
{
    CHAR szLensModuleName[SHORT_STR_LEN];
    CHAR szIRISMode[SHORT_STR_LEN];
    CHAR szFocusMode[SHORT_STR_LEN];


}TPsiaLens;

typedef struct psia_lens_list
{
  TPsiaLens tLens;
  DWORD dwListNum;

}TPsiaLensList;

typedef struct psia_day_night_filter
{
  CHAR szDayNightFilterType[SHORT_STR_LEN];
  BOOL bSwitchScheduleEnabled;
  CHAR szBeginTime[SHORT_STR_LEN];
  CHAR szEndTime[SHORT_STR_LEN];
  
}TPsiaDayNightFilter;
typedef struct psia_video_input_channel
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szInputPort[SHORT_STR_LEN];
  CHAR szWhiteBalanceMode[SHORT_STR_LEN];
  DWORD dwWhiteBalanceLevel;
  CHAR szExposureMode[SHORT_STR_LEN];
  TPsiaExposure tExposure;
  TPsiaGainWindow tGainWindow;
  DWORD  dwGainLevel;
  DWORD  dwBrightnessLevel;
  DWORD  dwContrastLevel;
  DWORD  dwSharpnessLevel;
  DWORD  dwSaturationLevel;
  DWORD dwHueLevel;
  BOOL bGammaCorrectionEnabled;
  DWORD dwGammaCorrectionLevel;
  BOOL bWDREnabled;
  DWORD dwWDRLevel;
  TPsiaLensList tLensList;
  TPsiaDayNightFilter tDayNightFilter;
  
}TPsiaVideoInputChannel;

typedef struct psia_video_input_channel_list
{
  TPsiaVideoInputChannel *ptVideoInputChannel;
  DWORD dwListNum;

}TPsiaVideoInputChannelList;


typedef struct psia_video_input
{
  TPsiaVideoInputChannelList tVideoInputChannelList;

}TPsiaVideoInput;


typedef struct psia_focus_data
{
   DWORD dwFocus;

}TPsiaFocusData;

typedef struct psia_iris_data
{
  DWORD dwIRIS;

}TPsiaIRISData;


typedef struct psia_len_absolute
{
  DWORD dwAbsoluteFocus;
  DWORD dwAbsoluteIRIS;
}TPsiaLenAbsolute;

typedef struct psia_lens_status
{
  TPsiaLenAbsolute tAbsolute;
  
}TPsiaLensStatus;

typedef struct psia_text_overlay
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  BOOL bTimeStampEnabled;
  CHAR szDateTimeFormat[SHORT_STR_LEN];
  CHAR szBackgroundColor[LONG_STR_LEN];
  CHAR szFontColor[LONG_STR_LEN];
  DWORD dwFontSize;
  CHAR szDisplayText[LONG_STR_LEN];
  CHAR szHorizontalAlignType[SHORT_STR_LEN];
  CHAR szVerticalAlignType[SHORT_STR_LEN];
 

}TPsiaTextOverlay;

typedef struct psia_text_overlay_list
{
  TPsiaTextOverlay *ptTextOverlay;
  DWORD dwListNum;

}TPsiaTextOverlayList ;

typedef struct psia_image_overlay
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  CHAR szOverlayImageID[SHORT_STR_LEN];
  DWORD dwPositionX;
  DWORD dwPositionY;
  BOOL bTransparentColorEnabled;
  CHAR szTransparentColor[LONG_STR_LEN];

}TPsiaImageOverlay;


typedef struct psia_image_overlay_list
{
  
  TPsiaImageOverlay *ptImageOverlay;
  DWORD dwListNum;

}TPsiaImageOverlayList; 


typedef struct psia_video_overlay
{
  TPsiaTextOverlayList  tTextOverlayList;
  TPsiaImageOverlayList tImageOverlayList;

}TPsiaVideoOverlay;

typedef struct psia_privacy_mask_region
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  TPsiaRegionCoordinatesList tRegionCoordinatesList;
}TPsiaPrivacyMaskRegion;


typedef struct psia_privacy_mask_region_list
{
  TPsiaPrivacyMaskRegion *ptPrivacyMaskRegion;
  DWORD dwListNum;

}TPsiaPrivacyMaskRegionList;

typedef struct psia_privacy_mask
{
  BOOL bEnabled;
  TPsiaPrivacyMaskRegionList tPrivacyMaskRegionList;

}TPsiaPrivacyMask;

typedef struct psia_serial_port
{
   CHAR szID[SHORT_STR_LEN];
   BOOL bEnabled;
   CHAR szSerialPortType[SHORT_STR_LEN];
   CHAR szDuplexMode[SHORT_STR_LEN];
   CHAR szDirection[SHORT_STR_LEN];
   DWORD dwBaudRate;
   DWORD dwDataBits;
   CHAR szParityType[SHORT_STR_LEN];
   CHAR szStopBits[SHORT_STR_LEN];
  
}TPsiaSerialPort;

typedef struct psia_serial_port_list
{
  
  TPsiaSerialPort *ptSerialPort;
  DWORD dwListNum;

}TPsiaSerialPortList;

typedef struct psia_serial_command
{
  CHAR szChainNo[SHORT_STR_LEN];
  CHAR szCommand[LONG_STR_LEN];

}TPsiaSerialCommand;


typedef struct psia_diagnostic_command
{
  CHAR szCommand[SHORT_STR_LEN];
  CHAR szStatus[SHORT_STR_LEN];
  CHAR szResultMessage[SHORT_STR_LEN];

}TPsiaDiagnosticCommand;
typedef struct psia_diagnostic_command_list
{
  TPsiaDiagnosticCommand *ptDiagnosticCommand;
  DWORD dwListNum;

}TPsiaDiagnosticCommandList;


typedef struct psia_protocol
{
  CHAR  szProtocolID[SHORT_STR_LEN];

}PsiaProtocol;

typedef struct psia_protocol_list
{
  PsiaProtocol *ptProtocol;
  DWORD dwListNum;

}PsiaProtocolList;
typedef struct psia_user
{
    CHAR szID[SHORT_STR_LEN];
    CHAR szUserName[SHORT_STR_LEN];
    CHAR szPassword[SHORT_STR_LEN];
    PsiaProtocolList tProtocolList;
  

}TPsiaUser;

typedef struct psia_user_list
{
  TPsiaUser *ptUser;
  DWORD dwListNum;

}TPsiaUserList;

typedef struct psia_admin_access_protocol
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  CHAR szProtocol[SHORT_STR_LEN];
  DWORD dwPortNo;

}TPsiaAdminAccessProtocol;

typedef struct psia_admin_access_protocol_list
{
  TPsiaAdminAccessProtocol *ptAdminAccessProtocol;
  DWORD dwListNum;

}TPsiaAdminAccessProtocolList;


typedef struct psia_streaming_session_status
{
  CHAR szClientAddress[SHORT_STR_LEN];
  CHAR szClientUserName[SHORT_STR_LEN];
  CHAR szStartDateTime[SHORT_STR_LEN];
  DWORD dwElapsedTime;
  DWORD dwBandwidth;
  
}TPsiaStreamingSessionStatus;

typedef struct psia_streaming_session_status_list
{
    TPsiaStreamingSessionStatus *ptStreamingSessionStatus;
    DWORD dwListNum;
  
}TPsiaStreamingSessionStatusList;

typedef struct psia_streaming_status
{
  DWORD dwTotalStreamingSessions;
  TPsiaStreamingSessionStatusList tStreamingSessionStatusList;
}TPsiaStreamingStatus;

typedef struct psia_control_protocol
{
  CHAR szStreamingTransport[SHORT_STR_LEN];

}TPsiaControlProtocol;

typedef struct psia_control_protocol_list 
{
  TPsiaControlProtocol *ptControlProtocol;
  
  DWORD dwListNum;


}TPsiaControlProtocolList; 

typedef struct psia_unicast
{
  BOOL bEnabled;
  CHAR szInterfaceID[SHORT_STR_LEN];
  CHAR szRTPTransportType[SHORT_STR_LEN];
  
}TPsiaUnicast;

typedef struct psia_multicast
{
  BOOL bEnabled;
  DWORD dwUserTriggerThreshold;
  CHAR szDestIPAddress[SHORT_STR_LEN];
  DWORD dwVideoDestPortNo;
  DWORD dwAudioDestPortNo;
  CHAR szDestIPv6Address[SHORT_STR_LEN];
  DWORD dwTTL;

}TPsiaMulticast;

typedef struct psia_security
{
  BOOL bEnabled;

}TPsiaSecurity;


typedef struct  psia_transport
{
  DWORD dwRTSPPortNo;
  DWORD dwMaxPacketSize;
  DWORD dwAudioPacketLength;
  DWORD dwaudioInboundPacketLength;
  DWORD dwAudioInboundPortNo;
  DWORD dwVideoSourcePortNo;
  DWORD dwAudioSourcePortNo;
  TPsiaControlProtocolList tControlProtocolList;
  TPsiaUnicast tUnicast;
  TPsiaMulticast tMulticast;
  TPsiaSecurity tSecurity;

}TPsiaTransport;

typedef struct psia_video
{
  BOOL bEnabled;
  CHAR szVideoInputChannelID[SHORT_STR_LEN];
  CHAR szVideoCodecType[SHORT_STR_LEN];
  CHAR szVideoScanType[SHORT_STR_LEN];
  DWORD dwVideoResolutionWidth;
  DWORD dwVideoResolutionHeight;
  DWORD dwVideoPositionX;
  DWORD dwVideoPositionY;
  CHAR szVideoQualityControlType[SHORT_STR_LEN];	
  DWORD dwConstantBitRate;
  DWORD dwFixedQuality;
  DWORD dwVbrUpperCap;
  DWORD dwVbrLowerCap;
  DWORD dwMaxFrameRate;
  DWORD dwKeyFrameInterval;
  DWORD dwRotationDegree;
  BOOL bMirrorEnabled;
  CHAR szSnapShotImageType[SHORT_STR_LEN];

}TPsiaVideo;


typedef struct psia_audio
{
  BOOL bEnabled;
  CHAR szAudioInputChannelID[SHORT_STR_LEN];
  CHAR szAudioCompressionType[SHORT_STR_LEN];
  CHAR szAudioInboundCompressionType[SHORT_STR_LEN];
  DWORD dwAudioBitRate;
  DWORD dwAudioSamplingRate;
  DWORD dwAudioResolution;
  
}TPsiaAudio;

typedef struct psia_streaming_channel
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szChannelName[SHORT_STR_LEN];
  BOOL bEnabled;
  TPsiaTransport tTransport;
  TPsiaVideo tVideo;
  TPsiaAudio tAudio;
 

}TPsiaStreamingChannel;

typedef struct psia_streaming_channel_list
{
  TPsiaStreamingChannel *ptStreamingChannel;
  DWORD dwListNum;

}TPsiaStreamingChannelList;


typedef struct psia_ptz_channel
{
  CHAR szID[SHORT_STR_LEN];
  BOOL bEnabled;
  CHAR szVideoInputID[SHORT_STR_LEN];
  DWORD dwPanMaxSpeed;
  DWORD dwTiltMaxSpeed;
  DWORD dwAutoPatrolSpeed;
  CHAR szControlProtocol[SHORT_STR_LEN];
  CHAR szDefaultPresetID[SHORT_STR_LEN];

  
}TPsiaPTZChannel;

typedef struct psia_ptz_channel_list
{
  TPsiaPTZChannel *ptPTZChannel;
  DWORD dwListNum;

}TPsiaPTZChannelList;

typedef struct psia_momentary
{
  DWORD dwDuration;
}TPsiaMomentary;

typedef struct psia_relative
{
  DWORD dwPositionX;
  DWORD dwPositionY;
  DWORD dwRelativeZoom;

}TPsiaRelative;

typedef struct psia_ptz_absolute
{
  DWORD dwElevation;
  DWORD dwAzimuth;
  DWORD dwAbsoluteZoom;

}TPsiaPTZAbsolute;

typedef struct psia_digital
{
  DWORD dwPositionX;
  DWORD dwPositionY;
  DWORD dwDigitalZoomLevel;

}TPsiaDigital;

typedef struct psia_ptz_data
{
  DWORD dwPan;
  DWORD dwTilt;
  DWORD dwZoom;
  TPsiaMomentary tMomentary;
  TPsiaRelative tRelative;
  TPsiaPTZAbsolute tPTZAbsolute;
  TPsiaDigital tDigital;
}TPsiaPTZData;

typedef struct psia_ptz_status
{
     TPsiaPTZAbsolute tPTZAbsolute;

}TPsiaPTZStatus;



typedef struct psia_ptz_preset
{
 CHAR szID[SHORT_STR_LEN];
 CHAR szPresetName[SHORT_STR_LEN];
 TPsiaTextOverlayList tTextOverlayList;
  
}TPsiaPTZPreset;

typedef struct psia_ptz_preset_list 
{
  
  TPsiaPTZPreset *ptPTZPreset;
  DWORD dwListNum;
}TPsiaPTZPresetList;

typedef struct psia_patrol_sequence
{
  CHAR szPresetID[SHORT_STR_LEN];
  DWORD dwDelay;
  
}TPsiaPatrolSequence;

typedef struct psia_patrol_sequence_list
{
  TPsiaPatrolSequence *ptPatrolSequence;
  DWORD dwListNum;
}TPsiaPatrolSequenceList;


typedef struct psia_ptz_patrol
{
 CHAR szID[SHORT_STR_LEN];
 CHAR szPresetName[SHORT_STR_LEN];
 
 TPsiaPatrolSequenceList tPatrolSequenceList;
  
}TPsiaPTZPatrol;

typedef struct psia_ptz_patrol_list
{
  TPsiaPTZPatrol *ptPTZPatrol;
  DWORD dwListNum;
}TPsiaPTZPatrolList;

typedef struct psia_ptz_patrol_status
{
  CHAR szPatrolID[SHORT_STR_LEN];
  CHAR szPatrolStatus[SHORT_STR_LEN];  

}TPsiaPTZPatrolStatus;

typedef struct psia_ptz_patrol_status_list
{
  TPsiaPTZPatrolStatus *ptPTZPatrolStatus;
  DWORD dwListNum;
  
}TPsiaPTZPatrolStatusList;

typedef struct psia_time_range
{
  CHAR szBeginTime[SHORT_STR_LEN];
  CHAR szEndTime[SHORT_STR_LEN];

}TPsiaTimeRange;

typedef struct psia_time_block 
{
  DWORD dwDayOfWeek;
  TPsiaTimeRange tTimeRange;
 

}TPsiaTimeBlock;

typedef struct psia_time_block_list
{
  TPsiaTimeBlock *ptTimeBlock;
  DWORD dwListNum;

}TPsiaTimeBlockList;

typedef struct psia_grid
{
  DWORD dwRowGranularity;
  DWORD dwColumnGranularity;
  
}TPsiaGrid;

typedef struct psia_roi
{
  DWORD dwMinHorizontalResolution;
  DWORD dwMinVerticalResolution;
}TPsiaROI;

typedef struct psia_motion_detection
{

   CHAR szID[SHORT_STR_LEN];
   BOOL bEnabled;
   DWORD dwSamplingInterval;
   DWORD dwStartTriggerTime;
   DWORD dwEndTriggerTime;
   CHAR szDirectionSensitivity[SHORT_STR_LEN];
   CHAR szRegionType[SHORT_STR_LEN];
   DWORD dwMinObjectSize;
   DWORD dwMaxObjectSize;
   TPsiaGrid tGrid;
   TPsiaROI tROI;
   
   
}TPsiaMotionDetection;

typedef struct psia_motion_detection_list
{
  TPsiaMotionDetection *ptMotionDetection;
   DWORD dwListNum;
  
}TPsiaMotionDetectionList;




typedef struct psia_motion_detection_region
{
   CHAR szID[SHORT_STR_LEN];
   BOOL bEnabled;
   BOOL bMaskEnabled;
   DWORD dwSensitivityLevel;
   DWORD dwDetectionThreshold;
   TPsiaRegionCoordinatesList tRegionCoordinatesList;
   
}TPsiaMotionDetectionRegion;

typedef struct psia_motion_detection_region_list
{
  TPsiaMotionDetectionRegion *ptMotionDetectionRegion;
  DWORD dwListNum;
}TPsiaMotionDetectionRegionList;

typedef struct psia_event_trigger_notification
{
   CHAR szID[SHORT_STR_LEN];
   CHAR szNotificationMethod[SHORT_STR_LEN];
   CHAR szNotificationRecurrence[SHORT_STR_LEN];
   DWORD dwNotificationInterval;
   CHAR szOutputIOPortID[SHORT_STR_LEN];
   

}PsiaEventTriggerNotification;

typedef struct psia_event_trigger_notification_list
{
  PsiaEventTriggerNotification *ptEventTriggerNotification;
  DWORD dwListNum;

}PsiaEventTriggerNotificationList;

typedef struct psia_event_trigger
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szEventType[SHORT_STR_LEN];
  CHAR szEventDescription[SHORT_STR_LEN];
  CHAR szInputIOPortID[SHORT_STR_LEN];
  DWORD dwIntervalBetweenEvents;
  PsiaEventTriggerNotificationList tEventTriggerNotificationList;
 
}TPsiaEventTrigger;

typedef struct psia_event_trigger_list
{
  TPsiaEventTrigger *ptEventTrigger;
  DWORD dwListNum;

}TPsiaEventTriggerList;

typedef struct psia_date_time_range
{
  CHAR szBeginDateTime[SHORT_STR_LEN];
  CHAR szEndDateTime[SHORT_STR_LEN];
  
}TPsiaDateTimeRange;

typedef struct psia_event_schedule
{
  TPsiaDateTimeRange tDateTimeRange;
  TPsiaTimeBlockList tTimeBlockList;

}TPsiaEventSchedule;


typedef struct psia_mailing_notification
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szAuthenticationMode[SHORT_STR_LEN];
  CHAR szAddressingFormatType[SHORT_STR_LEN];
  CHAR szHostName[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  CHAR szPOPAddressingFormatType[SHORT_STR_LEN];
  CHAR szPOPServerHostName[SHORT_STR_LEN];
  CHAR szPOPServerIPAddress[SHORT_STR_LEN];
  CHAR szPOPServerIPV6Address[SHORT_STR_LEN];
  CHAR szAccountName[SHORT_STR_LEN];
  CHAR szPassword[SHORT_STR_LEN];
  
  
}TPsiaMailingNotification;
typedef struct psia_mailing_notification_list
{
  TPsiaMailingNotification *ptMailingNotification;
  DWORD dwListNum;

} TPsiaMailingNotificationList;

typedef struct psia_ftp_notification
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szAddressingFormatType[SHORT_STR_LEN];
  CHAR szHostName[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  CHAR szUserName[SHORT_STR_LEN];
  CHAR szPassword[SHORT_STR_LEN];
  BOOL bPassiveModeEnabled;
  CHAR szUploadPath[LONG_STR_LEN];
  CHAR szBaseFileName[SHORT_STR_LEN];


}TPsiaFTPNotification;

typedef struct psia_ftp_notification_list
{
  TPsiaFTPNotification *ptFTPNotification;
   DWORD dwListNum;
  
}TPsiaFTPNotificationList;

typedef struct psia_http_host_notification
{
  CHAR szID[SHORT_STR_LEN];
  CHAR szURL[LONG_STR_LEN];
  CHAR szProtocolType[LONG_STR_LEN];
  CHAR szParameterFormatType[LONG_STR_LEN];
  CHAR szAddressingFormatType[SHORT_STR_LEN];
  CHAR szHostName[SHORT_STR_LEN];
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  CHAR szUserName[SHORT_STR_LEN];
  CHAR szPassword[SHORT_STR_LEN];
  CHAR szHTTPAuthenticationMethod[SHORT_STR_LEN];


}TPsiaHTTPHostNotification;


typedef struct psia_http_host_notification_list
{
  TPsiaHTTPHostNotification *ptHTTPHostNotification;
   DWORD dwListNum;

}TPsiaHTTPHostNotificationList;

typedef struct psia_ftp_format
{
  BOOL bUploadSnapShotEnabled;
  BOOL bUploadVideoClipEnabled;

}TPsiaFTPFormat;

typedef struct psia_body_setting
{
BOOL bAttachedVideoURLEnabled;
BOOL bAttachedSnapShotEnabled;
BOOL bAttachedVideoClipEnabled; 
}TPsiaBodySetting;



typedef struct psia_email_format
{
  CHAR szSenderEmailAddress[SHORT_STR_LEN];
  CHAR szReceiverEmailAddress[SHORT_STR_LEN];
  CHAR szSubject[SHORT_STR_LEN];
  TPsiaBodySetting tBodySetting;  
}TPsiaEmailFormat;

typedef struct  psia_media_format
{
  CHAR szSnapShotImageType[SHORT_STR_LEN];
  CHAR szVideoClipFormatType[SHORT_STR_LEN];
  DWORD dwPreCaptureLength;
  DWORD dwPostCaptureLength;
   

}TPsiaMediaFormat;


typedef struct psia_evnent_notification_methods
{

  TPsiaMailingNotificationList tMailingNotificationList;
  TPsiaFTPNotificationList tFTPNotificationList;
  TPsiaHTTPHostNotificationList tHTTPHostNotificationList;
  TPsiaFTPFormat  tFTPFormat;
  TPsiaEmailFormat tEmailFormat;
  TPsiaMediaFormat tMediaFormat;
  
}TPsiaEventNotificationMethods;


typedef struct psia_event_notification
{
  TPsiaEventTriggerList tEventTriggerList;
  TPsiaEventSchedule	tEventSchedule;
  TPsiaEventNotificationMethods tEventNotificationMethods;
  
}TPsiaEventNotification;


typedef struct psia_detection_region_entry
{
	CHAR szRegionID[SHORT_STR_LEN];
        DWORD dwSensitivityLevel;
        DWORD dwDetectionThreshold;

}TPsiaDetectionRegionEntry;

typedef struct psia_detection_region_list
{
  TPsiaDetectionRegionEntry *ptDetectionRegionEntry;
    DWORD dwListNum;
  
}TPsiaDetectionRegionList;

typedef struct psia_event_notification_alert
{
  CHAR szIPAddress[SHORT_STR_LEN];
  CHAR szIPV6Address[SHORT_STR_LEN];
  DWORD dwPortNo;
  CHAR szProtocol[SHORT_STR_LEN];
  CHAR szChannelID[SHORT_STR_LEN];
  CHAR szDateTime[SHORT_STR_LEN];
  DWORD dwActivePostCount;
  CHAR szEventType[SHORT_STR_LEN];
  CHAR szEventState[SHORT_STR_LEN];
  CHAR szEventDescription[LONG_STR_LEN];
  CHAR szInputIOPortID[SHORT_STR_LEN];
 
}TPsiaEventNotificationAlert;

#endif