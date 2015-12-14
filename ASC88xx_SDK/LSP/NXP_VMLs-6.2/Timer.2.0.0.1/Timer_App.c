/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/Timer/Timer_App/Timer_App.c 7     09/04/02 2:37p Aniki $
 *
 * Copyright 2008 ______ Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: Timer_App.c $
 * 
 * *****************  Version 7  *****************
 * User: Aniki        Date: 09/04/02   Time: 2:37p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * 
 * *****************  Version 6  *****************
 * User: Aniki        Date: 08/01/21   Time: 6:17p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * 
 * *****************  Version 5  *****************
 * User: Aniki        Date: 08/01/21   Time: 5:28p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * 
 * *****************  Version 4  *****************
 * User: Aniki        Date: 06/09/14   Time: 5:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * 
 * *****************  Version 3  *****************
 * User: Aniki        Date: 06/09/14   Time: 5:07p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * 
 * *****************  Version 2  *****************
 * User: Aniki        Date: 05/05/20   Time: 3:04p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * MODIFICATION: Print "dwTick" instead of "dwTimeOut" in application -
 * DONE.
 * 
 * *****************  Version 1  *****************
 * User: Aniki        Date: 05/05/12   Time: 6:03p
 * Created in $/rd_2/project/SoC/Linux_Libraries/Timer/Timer_App
 * 
 */

/* ============================================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Timer.h"

/* ============================================================================================= */
/* Version 1.2.0.4 modification, 2008.01.21 */
#ifdef __ASM_ARCH_PLATFORM_BACH_H__
#define SYS_CLK				(100000000)
#endif // __ASM_ARCH_PLATFORM_BACH_H__

#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
#define SYS_CLK				(150000000)
#endif // __ASM_ARCH_PLATFORM_HAYDN_H__
/* ======================================== */
/* Version 1.2.0.5 modification, 2008.03.31 */
#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define SYS_CLK				(20000000)
#endif // __ASM_ARCH_PLATFORM_MOZART_H__
/* ======================================== */
#define APB_CLK				(SYS_CLK/2)

#define	MAX_COUNT			50

/* ============================================================================================= */
int main(int argc, char **argv)
{
	TTimerInitOptions tTimerInitOptions;

	HANDLE hTimerObject0, hTimerObject1;
	DWORD dwInterval;
	DWORD dwAccessSize, dwSleepInterval, dwLoopCount, dwIntrInterval;
	DWORD dwReadData, dwWriteData;
	DWORD dwCurrentTick, dwTick;
	DWORD dwTimeOut;
	DWORD i;

	DWORD *pdwBuf0, *pdwBuf1;

	if (argc != 5)
	{
		printf("Usage: timer access_size(K Bytes) sleep_second loop_count(K times) interrupt_interval(msec)\n\n");
		exit(1);
	}

	dwAccessSize = atoi(argv[1]) * 1024;
	dwSleepInterval = atoi(argv[2]);
	dwLoopCount = atoi(argv[3]) * 1024;
	dwIntrInterval = atoi(argv[4]);

	printf("Loop count = %ld times\n", dwLoopCount);
	printf("Access size = %ld bytes\n", dwAccessSize);
	printf("Interrupt interval = %ld msec\n", dwIntrInterval);

	dwIntrInterval *= (APB_CLK / 1000);	

	if ((pdwBuf0=(DWORD *)malloc(dwAccessSize)) == NULL)
	{
		printf("Allocate buffer 0 fail !!\n");
		exit(1);
	}

	if ((pdwBuf1=(DWORD *)malloc(dwAccessSize)) == NULL)
	{
		printf("Allocate buffer 0 fail !!\n");
		exit(1);
	}

/* Version 1.2.0.2 modification, 2006.07.31 */
	printf("Start TMRC profile type test...\n");
/* ======================================== */
	tTimerInitOptions.dwVersion = TIMER_VERSION;
	tTimerInitOptions.eTimerType = TIMER_TYPE_PROFILE;
    tTimerInitOptions.pObjectMem = NULL;

	if (Timer_Initial(&hTimerObject0, &tTimerInitOptions) != S_OK)
	{
		printf("Initial Timer0 object fail !!\n");
		exit(1);
	}

	dwReadData = 0;
	dwWriteData = 0;

	Timer_ProfileStart(hTimerObject0);
	for (i=0; i<dwLoopCount; i++)
	{
		dwReadData = dwReadData + i;
	}
	Timer_GetInterval(hTimerObject0, &dwInterval);
	printf("Loop tick count = %d\n", (int)dwInterval);

	Timer_ProfileStart(hTimerObject0);
	for (i=0; i<(dwAccessSize>>2); i++)
	{
		dwReadData = dwReadData + pdwBuf0[i];
	}
	Timer_GetInterval(hTimerObject0, &dwInterval);
	printf("Read tick count = %d\n", (int)dwInterval);

	dwWriteData = 0;
	Timer_ProfileStart(hTimerObject0);
	for (i=0; i<(dwAccessSize>>2); i++)
	{
		pdwBuf1[i] = dwWriteData + i;
	}
	Timer_GetInterval(hTimerObject0, &dwInterval);
	printf("Write tick count = %d\n", (int)dwInterval);

	Timer_ProfileStart(hTimerObject0);
	for (i=0; i<(dwAccessSize>>2); i++)
	{
		pdwBuf1[i] = pdwBuf0[i];
	}
	Timer_GetInterval(hTimerObject0, &dwInterval);
	printf("Read/write tick count = %d\n", (int)dwInterval);

	Timer_ProfileStart(hTimerObject0);
	sleep(dwSleepInterval);
	Timer_GetInterval(hTimerObject0, &dwInterval);
	printf("Sleep tick count = %d\n", (int)dwInterval);

/* Version 1.2.0.2 modification, 2006.07.31 */
	printf("Press 'enter' to start 10 seconds profile test !!\n");
	printf("And then press 'enter' again after 10 seconds...\n");
	getchar();
	Timer_ProfileStart(hTimerObject0);
	getchar();
	Timer_GetInterval(hTimerObject0, &dwInterval);
	printf("Tick count of 10 seconds interval = %d\n\n\n", (int)dwInterval);
/* ======================================== */

	if (Timer_Release(&hTimerObject0) != S_OK)
	{
		printf("Release Timer0 object fail !!\n");
		exit(1);
	}

	free(pdwBuf0);
	free(pdwBuf1);

/* Version 1.2.0.2 modification, 2006.07.31 */
	printf("Start TMRC interrupt type test...\n");
/* ======================================== */

	tTimerInitOptions.eTimerType = TIMER_TYPE_INTERRUPT;
	tTimerInitOptions.dwInterval = dwIntrInterval;

	if (Timer_Initial(&hTimerObject1, &tTimerInitOptions) != S_OK)
	{
		printf("Initial Timer1 object fail !!\n");
		exit(1);
	}

	dwCurrentTick = 0;
	dwTimeOut = 0;

	do
	{
		Timer_GetTick(hTimerObject1, &dwTick);
		if (dwTick > dwCurrentTick)
		{
			dwTimeOut++;
/* Version 1.1.0.0 modification, 2005.05.20 */
			printf("Interrupt %ld !!\n", dwTick);
/* ======================================== */
		}
		dwCurrentTick = dwTick;
	} while (dwTimeOut < MAX_COUNT);

	if (Timer_Release(&hTimerObject1) != S_OK)
	{
		printf("Release Timer1 object fail !!\n");
		exit(1);
	}

	exit(0);
}

/* ============================================================================================= */
