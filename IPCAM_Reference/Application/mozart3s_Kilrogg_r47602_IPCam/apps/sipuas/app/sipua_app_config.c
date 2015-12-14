#include "sipua_app.h"

typedef struct sipclient_config_values
{
	char acLocalUri[100];
	char acContactUri[100];
	char acAudioCodec[8]; // PCMU, AMR
	char acTxUnixSock[100];
    long acTimeout; //timeout(seconds)
} TSipUaAppConfigValues;

SCODE SipUaApp_Get_configs(const char *szConfFileName, char **pszLocalUri, 
							char **pszContactUri, char **pszAudioCodec,
							char **pszTxUnixSock, long *plTimeout)
{
	HANDLE hXmlMgrObj;
	TXmlMgrInitOptions tXmlMgrInitOpt;
	
	static TSipUaAppConfigValues tConfigValues;
	TXmlMgrParsingMap tParsingMap[] = 
	{
		{"root/sipua/local_uri", xmlmgr_set_string, tConfigValues.acLocalUri},
		{"root/sipua/contact_uri", xmlmgr_set_string, tConfigValues.acContactUri},
		{"root/sipua/audio_codec", xmlmgr_set_string, tConfigValues.acAudioCodec},
		{"root/sipua/ua_server/tx_unix_sock", xmlmgr_set_string, tConfigValues.acTxUnixSock},
        {"root/sipua/client_timeout", xmlmgr_set_long, &(tConfigValues.acTimeout)},
		{NULL, 0, NULL}
    };


	if (szConfFileName == NULL)
	{
		return S_FAIL;
	}

	memset(&tConfigValues, 0, sizeof(TSipUaAppConfigValues));

	// xmlmgr initial
	tXmlMgrInitOpt.dwVersion = XMLMGR_VERSION;
    if (XmlMgr_Initial(&hXmlMgrObj, &tXmlMgrInitOpt) != S_OK)
    {
        fprintf(stderr, "[SipUaApp] XmlMgr_Initial fail\n");
        return S_FAIL;
    }

    if (XmlMgr_ReadFile(hXmlMgrObj, szConfFileName) != S_OK)
    {
        fprintf(stderr, "[SipUaApp] Open XML file %s fail\n", szConfFileName);
        return S_FAIL;
    }

	XmlMgr_GetMultiConfValue(hXmlMgrObj, tParsingMap);

	fprintf(stderr, "[SipUaApp]Local uri %s\n", tConfigValues.acLocalUri);
	fprintf(stderr, "[SipUaApp]Contact uri %s\n", tConfigValues.acContactUri);
	fprintf(stderr, "[SipUaApp]Audio codec %s\n", tConfigValues.acAudioCodec);
	fprintf(stderr, "[SipUaApp]Tx Unix sock path %s\n", tConfigValues.acTxUnixSock);
	fprintf(stderr, "[SipUaApp]Timeout %ld\n", tConfigValues.acTimeout);  

	if (strlen(tConfigValues.acLocalUri) > 0)
	{
		*pszLocalUri = tConfigValues.acLocalUri;
	}
	else
	{
		*pszLocalUri = NULL;
	}

	if (strlen(tConfigValues.acContactUri) > 0)
	{
		*pszContactUri = tConfigValues.acContactUri;
	}
	else
	{
		*pszContactUri = NULL;
	}

	if (strlen(tConfigValues.acAudioCodec) > 0)
	{
		*pszAudioCodec = tConfigValues.acAudioCodec;
	}
	else
	{
		*pszAudioCodec = NULL;
	}

	if (strlen(tConfigValues.acTxUnixSock) > 0)
	{
		*pszTxUnixSock = tConfigValues.acTxUnixSock;
	}
	else
	{
		*pszTxUnixSock = NULL;
	}
    if( tConfigValues.acTimeout > 0)
    {
        *plTimeout = tConfigValues.acTimeout;
    }
    else
    {
        *plTimeout = 30;
    }

	XmlMgr_Release(&hXmlMgrObj);
	
	return S_OK;
}
