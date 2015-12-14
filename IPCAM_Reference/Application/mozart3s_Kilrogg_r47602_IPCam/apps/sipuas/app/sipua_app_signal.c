#include "sipua_app.h"

extern HANDLE hSipUaAppUaObject;
extern int iSipUaAppKeepGoing;

void SipUaApp_SignalHandler(int signo)
{
	if (signo == SIGHUP)
	{
		if (hSipUaAppUaObject != NULL)
		{
			if (Ua_StopMainLoop(hSipUaAppUaObject) != S_OK)
        	{
            	fprintf(stderr, "[SipUaApp] Ua_StopMaiLoop failed!\n");
        	}
		}
	}
	else if (signo == SIGINT || signo==SIGTERM)
	{
		if (hSipUaAppUaObject != NULL)
		{
			if (Ua_StopMainLoop(hSipUaAppUaObject) != S_OK)
        	{
            	fprintf(stderr, "[SipUaApp] Ua_StopMaiLoop failed!\n");
        	}
		}
		iSipUaAppKeepGoing = 0;
	}
	
	return;
}

