#ifndef _SIP_UA_APP_H_
#define _SIP_UA_APP_H_
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "xmlmgr.h"
#include "Ua.h"

// config
SCODE SipUaApp_Get_configs(const char *szConfFileName, char **pszLocalUri, 
							char **pszContactUri, char **pszAudioCodec,
							char **pszTxUnixSock, long *plTimeout);
// signal handler
void SipUaApp_SignalHandler(int signo);

#endif //  _SIP_UA_APP_H_

