#ifndef _UA_H_
#define _UA_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "typedef.h"
#include "errordef.h"
#include "dbgdefs.h"
#include "ubuffer.h"
#include "global_codec.h"

#define SIP_DEFAULT_PORT	5060

#define SIPUA_VERSION MAKEFOURCC(1, 0, 0, 0)

typedef struct ua_status_recorder TUaStatusRecorder;

typedef SCODE (*PFUaOnRcvCommand) (HANDLE hObject, char *abyBuffer, int iBuffLen);
typedef SCODE (*PFUaOnStatusChange) (HANDLE hObject, char *szStatus, char *szMsg);

typedef struct ua_init_options
{
	DWORD				dwVersion;
	const char			*szLocalUri; 		//required
	
	// Options
	const char			*szDisplayName;		//if no use, set to NULL
	const char			*szContactUri;		//if no use, set to NULL
	
	int					iCommandFifoFd;		//if no use, set to -1
	PFUaOnRcvCommand	pfOnRcvCommand;		//if no use, set to NULL

	PFUaOnStatusChange	pfOnStatusChange;	//if no use, set to NULL

	const char			*szRxUnixSockPath;	//if no use, set to NULL
	const char			*szTxUnixSockPath;	//if no use, set to NULL

	const char			*szAudioCodec;		// PCMU (G.711 u-law), PCMA (G.711 a-law), 
											// AMR, AMR-WB, AAC4, default is PCMU 
	long            	lTimeout;           // Client timeout                     
} TUaInitOpts;

typedef struct ua_start_options
{
	const char *szRemoteUri;
} TUaStartOpts;

SCODE Ua_Init(HANDLE *phObject, TUaInitOpts *ptInitOpts);
SCODE Ua_Release(HANDLE *phObject);
SCODE Ua_DoMainLoop(HANDLE hObject);
SCODE Ua_StopMainLoop(HANDLE hObject);
SCODE Uac_Start_A_Session(HANDLE hObject, TUaStartOpts *ptStartOpts);
SCODE Uac_Stop_A_Session(HANDLE hObject);

#endif // _UA_H_
