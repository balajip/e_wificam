/*

/* ============================================================================================== */
#include "VideoCap_Locals.h"

/* ============================================================================================== */
SCODE VideoCap_Start(HANDLE hObject)
{
    TVideoCapInfo *ptInfo = (TVideoCapInfo *)hObject;
/* Version 3.2.0.0 modification, 2008.04.30 */
/* Version 3.3.0.1 modification, 2008.09.23 */
/* ======================================== */
/* ======================================== */
/* Version 12.0.0.3 modification, 2013.01.16 */
    VideoCap_StartVIC(ptInfo);
/* ======================================== */
/* Version 3.2.0.0 modification, 2008.04.30 */
/* Version 3.3.0.1 modification, 2008.09.23 */
/* ======================================== */
/* ======================================== */
    return S_OK;
}

/* ============================================================================================== */
SCODE VideoCap_GetBuf(HANDLE hObject, TVideoCapState *ptState)
{
	TVideoCapInfo *ptInfo = (TVideoCapInfo *)hObject;
	DWORD dwDiffTicks;
	DWORD dwInterval;
/* Version 6.0.0.1 modification, 2009.06.19 */
	DWORD dwIncr;
/* ======================================== */
/* Version 3.1.0.0 modification, 2008.02.02 */
	DWORD dwCurrTicks;
/* ======================================== */
/* Version 7.0.0.0 modification, 2009.10.08 */
/* Version 7.2.0.3 modification, 2009.11.10 */
	DWORD dwCurrLuma, dwCurrACSFactor, dwDownACSFactor, dwUpACSFactor;
/* ======================================== */
	TVideoSignalOptions tOption;
/* ======================================== */

	if (VideoCap_GetBufVIC(ptInfo, ptState) != S_OK)
	{
/* Version 2.0.0.0 modification, 2006.06.21 */
		if (ptState->bFifoFull == TRUE)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			ERRPRINT("FIFO full (error frame) !!\n");
/* Version 12.0.0.4 modification, 2013.01.23 */
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
/* ======================================== */
/* ======================================== */
		}

		if (ptState->bNoSignal == TRUE)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			ERRPRINT("No signal !!\n");
/* Version 12.0.0.4 modification, 2013.01.23 */
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
/* ======================================== */
/* ======================================== */
		}
/* ======================================== */
/* Version 12.0.0.3 modification, 2013.01.16 */
/* Version 12.0.0.4 modification, 2013.01.23 */
		DBPRINT0("[VIDEOCAP][ERROR]: Fail to get video buffer !!\n");
/* ======================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
		VideoCap_ReleaseBuf(ptInfo, ptState);
/* ======================================== */
		return _______VIDEOCAP_ERR_GET_FRAME_BUFFER;
/* ======================================== */
	}


/* Version 3.1.0.0 modification, 2008.02.02 */
/* Version 5.4.0.0 modification, 2008.12.17 */
	dwCurrTicks = ptState->dwSecond*1000 + ptState->dwMilliSecond;
/* Version 6.0.0.1 modification, 2009.06.19 */
	ptInfo->dwIntvResAccu = ptInfo->dwIntvResAccu + ptInfo->dwIntervalResidue;
	dwIncr = (ptInfo->dwIntvResAccu>=PRECISION) ? 1 : 0;
	ptInfo->dwIntvResAccu = (dwIncr != 0)
							? (ptInfo->dwIntvResAccu - PRECISION)
							: ptInfo->dwIntvResAccu;
	dwInterval = ptInfo->dwInterval	+ dwIncr;
/* ======================================== */
/* Version 5.2.0.5 modification, 2008.11.27 */
/* Version 5.3.0.1 modification, 2008.12.01 */
/* ======================================== */
	if (ptInfo->bInitPrevTicks == TRUE)
	{
/* Version 5.4.0.0 modification, 2008.12.17 */
		ptInfo->dwPrevTicks = dwCurrTicks - dwInterval;
/* ======================================== */
		ptInfo->bInitPrevTicks = FALSE;
	}
	else if (ptState->bTimeStampCal == TRUE)
	{
/* Version 5.3.0.1 modification, 2008.12.01 */
		ptInfo->dwPrevTicks = dwCurrTicks;
/* ======================================== */
		ptInfo->dwDiffResidue = 0;
		ptInfo->dwFrameCount = 0;
	}
/* ======================================== */

	if (dwCurrTicks >= ptInfo->dwPrevTicks)
	{
		dwDiffTicks = dwCurrTicks - ptInfo->dwPrevTicks;
	}
	else
	{
/* Version 6.0.0.1 modification, 2009.06.19 */
		dwDiffTicks = (DWORD)((DWORD)0xFFFFFFFF-ptInfo->dwPrevTicks) + dwCurrTicks + 1;
/* ======================================== */
	}
/* ======================================== */

/* Version 3.2.0.0 modification, 2008.04.30 */
    ptState->dwCapH = ptInfo->ptVICInfo->atChnInfo[0].dwCapH;
    ptState->dwCapV = ptInfo->ptVICInfo->atChnInfo[0].dwCapV;
/* ======================================== */

/* Version 3.1.0.0 modification, 2008.02.02 */
	if ((ptInfo->dwFrameRate==0) || ((dwDiffTicks+ptInfo->dwDiffResidue)>=dwInterval) || (ptInfo->bIsMaster==TRUE))
/* ======================================== */
	{
		// take this picture and APP should release this buffer
		ptInfo->dwDiffResidue = ptInfo->dwDiffResidue + dwDiffTicks - dwInterval;
		ptInfo->dwFrameCount = ((ptInfo->dwFrameCount+1)<ptInfo->dwFrameRate) ? (ptInfo->dwFrameCount+1) : 0;
/* Version 5.2.0.5 modification, 2008.11.27 */
/* Version 5.4.0.0 modification, 2008.12.17 */
/* Version 6.0.0.1 modification, 2009.06.19 */
/* Version 6.0.0.2 modification, 2009.06.23 */
		if (dwDiffTicks >= (dwInterval << 2))
		{
			ptInfo->dwDiffResidue = (dwInterval >> 1);
		}
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.0 modification, 2008.02.02 */
		ptInfo->dwPrevTicks = dwCurrTicks;
/* ======================================== */
		return S_OK;
	}
	else
	{
		// drop this frame and release this buffer
/* Version 12.0.0.3 modification, 2013.01.16 */
		VideoCap_ReleaseBufVIC(ptInfo, ptState);
/* Version 12.0.0.4 modification, 2013.01.23 */
		DBPRINT0("[VIDEOCAP][ERROR]: Fail to get video buffer !!\n");
		DBPRINT0("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_GET_FRAME_BUFFER;
/* ======================================== */
/* ======================================== */
	}
}

/* ============================================================================================== */
SCODE VideoCap_ReleaseBuf(HANDLE hObject, TVideoCapState *ptState)
{
	TVideoCapInfo *ptInfo = (TVideoCapInfo *)hObject;

/* Version 12.0.0.3 modification, 2013.01.16 */
	VideoCap_ReleaseBufVIC(ptInfo, ptState);
/* ======================================== */

	return S_OK;
}

/* ============================================================================================== */
SCODE VideoCap_Sleep(HANDLE hObject)
{
	TVideoCapInfo *ptInfo = (TVideoCapInfo *)hObject;

/* Version 12.0.0.3 modification, 2013.01.16 */
	VideoCap_WaitVIC(ptInfo);
/* ======================================== */

	return S_OK;
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_Stop(HANDLE hObject)
{
	TVideoCapInfo *ptInfo = (TVideoCapInfo *)hObject;

	/*if (VideoCap_WaitVIC(ptInfo) != S_OK)
	{
		DBPRINT0("VPL_VIC sleep fail !!\n");
		return S_FAIL;
	}*/

/* Version 3.2.0.0 modification, 2008.04.30 */
/* Version 3.3.0.1 modification, 2008.09.23 */
//    if (VideoCap_StopVICIs(ptInfo) != S_OK)
//    {
//        DBPRINT0("Stop IS of VPL_VIC fail!!\n");
//        return S_FAIL;
//    }
/* ======================================== */
/* ======================================== */
/* Version 12.0.0.3 modification, 2013.01.16 */
	VideoCap_StopVIC(ptInfo);
/* ======================================== */

	return S_OK;
}
/* ======================================== */

/* ============================================================================================== */
