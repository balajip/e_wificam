/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib/Timer.h 9     10/02/08 11:08a Bernard $
 *
 * Copyright 2008 ______ Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: Timer.h $
 * 
 * *****************  Version 9  *****************
 * User: Bernard      Date: 10/02/08   Time: 11:08a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * UPDATE: Update version number from 2.0.0.0 to 2.0.0.1 - DONE.
 * 
 * *****************  Version 8  *****************
 * User: Alan         Date: 09/11/26   Time: 7:53p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * UPDATE: Update version number from 1.2.0.5 to 2.0.0.0 - DONE.
 * 
 * *****************  Version 7  *****************
 * User: Aniki        Date: 09/01/09   Time: 10:50a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * 
 * *****************  Version 6  *****************
 * User: Aniki        Date: 08/01/21   Time: 6:17p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * 
 * *****************  Version 5  *****************
 * User: Aniki        Date: 07/09/03   Time: 11:19a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * 
 * *****************  Version 4  *****************
 * User: Aniki        Date: 06/09/14   Time: 5:18p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * 
 * *****************  Version 3  *****************
 * User: Jaja         Date: 05/10/20   Time: 8:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * UPDATE: Update version number from 1.2.0.0 to 1.2.0.1 - DONE.
 * 
 * *****************  Version 2  *****************
 * User: Aniki        Date: 05/05/20   Time: 3:02p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * 
 * *****************  Version 1  *****************
 * User: Aniki        Date: 05/05/12   Time: 6:04p
 * Created in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_Lib
 * 
 */

/* ============================================================================================= */
#ifndef __TIMER_H__
#define __TIMER_H__

/* ============================================================================================= */
#include "typedef.h"
#include "errordef.h"
#include "global_codec.h"

/* ============================================================================================= */
#define TIMER_VERSION MAKEFOURCC(2, 0, 0, 1)
/* Version 1.2.0.5 modification, 2009.01.09 */
#define TIMER_ID_VERSION "2.0.0.1"
/* ======================================== */

/* ============================================================================================= */
typedef enum timerinfo
{
	TIMER_TYPE_PROFILE = 1,
	TIMER_TYPE_INTERRUPT = 2
} ETimerInfo;

/* ============================================================================================= */
typedef struct timer_init_options
{
	DWORD dwVersion;

	ETimerInfo eTimerType;

	DWORD dwInterval;

	void *pObjectMem;
} TTimerInitOptions;

/* ============================================================================================= */
SCODE Timer_Initial(HANDLE *phObject, TTimerInitOptions *ptInitOptions);
SCODE Timer_Release(HANDLE *phObject);

DWORD Timer_QueryMemSize(TTimerInitOptions *ptInitOptions);
SCODE Timer_GetVersionInfo(TVersionNum *ptVersion);

SCODE Timer_ProfileStart(HANDLE hObject);
SCODE Timer_GetInterval(HANDLE hObject, DWORD *pdwInterval);
SCODE Timer_GetTick(HANDLE hObject, DWORD *pdwTick);

/* ============================================================================================= */
#endif //__TIMER_H__

/* ============================================================================================= */
