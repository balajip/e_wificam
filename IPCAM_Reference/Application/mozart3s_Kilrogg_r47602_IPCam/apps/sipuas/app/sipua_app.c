#include "sipua_app.h"

int iSipUaAppKeepGoing=1;
HANDLE hSipUaAppUaObject=NULL;

int main(int argc, char *argv[])
{
	TUaInitOpts tUaInitOpts;
	
	SCODE sRet;

	char *szLocalUri, *szContactUri, *szAudioCodec;
	char *szTxUnixSock;
	long lTimeout;
	
/****************************************************************************/
	if (argc < 2)
	{
FAIL:
		fprintf(stderr, "Usage : %s -h\n", argv[0]);
		exit(1);
	}
	else
	{
		int ch;
		while ((ch = getopt(argc, argv, "hc:")) != -1)
		{
			switch (ch)
			{
				case 'h':
		            fprintf(stderr, "Usage : %s -c [CONFIG_FILE]\n", argv[0]);
					exit(1);
					break;
				case 'c':
					if (argc < 3)
					{
						goto FAIL;
					}
					break;
				default:
					goto FAIL;
			}
		}
	}

	while (iSipUaAppKeepGoing)
	{
/****************************************************************************/
		// read config values from config file
		sRet = SipUaApp_Get_configs(argv[2], &szLocalUri, &szContactUri, &szAudioCodec,
												&szTxUnixSock, &lTimeout);
		if (sRet != S_OK)
		{
			exit(1);
		}
		
/****************************************************************************/
		// Initial Uas
		tUaInitOpts.dwVersion = SIPUA_VERSION;
		tUaInitOpts.szLocalUri = szLocalUri;
		tUaInitOpts.szDisplayName = NULL;
		tUaInitOpts.szContactUri = szContactUri;
		// The following 4 options is for sipua client
		tUaInitOpts.iCommandFifoFd = -1;
		tUaInitOpts.pfOnRcvCommand = NULL;
		tUaInitOpts.pfOnStatusChange = NULL;
		tUaInitOpts.szRxUnixSockPath = NULL;
		// for send ubuffer to adec
		tUaInitOpts.szTxUnixSockPath = szTxUnixSock;
		tUaInitOpts.szAudioCodec = szAudioCodec;
        // For client timeout
        tUaInitOpts.lTimeout = lTimeout;     

		if (Ua_Init(&hSipUaAppUaObject, &tUaInitOpts) != S_OK)
		{
			fprintf(stderr, "[SipUaApp] Ua_Init failed!\n");
			return -1;
		}
	
		//handling the ctrl-c
		signal(SIGINT, SipUaApp_SignalHandler);
		//handling the reload config file
		signal(SIGHUP, SipUaApp_SignalHandler);
		//handling the SIGTERM
		signal(SIGTERM, SipUaApp_SignalHandler);

		if (Ua_DoMainLoop(hSipUaAppUaObject) != S_OK)
		{
			fprintf(stderr, "[SipUaApp] Ua_DoMainLoop failed!\n");
			return -1;
		}

		if (Ua_Release(&hSipUaAppUaObject) != S_OK)
		{
			fprintf(stderr, "[SipUaApp] Ua_Release failed!\n");
			return -1;
		}
	}
	
	return 0;
}

