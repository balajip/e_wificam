/*
 * $Header: /rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRTool_Lib/amrctrlfile.h 2     09/02/27 11:58a Angel $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * AMRCtrlFile.h :
 *
 *  header file of AMRCtrlFile, included externally
 *
 * $History: amrctrlfile.h $
 * 
 * *****************  Version 2  *****************
 * User: Angel        Date: 09/02/27   Time: 11:58a
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRTool_Lib
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/03/17   Time: 4:27p
 * Created in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRTOOL_LIB
 * Optimized on ARM9
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/02/19   Time: 8:52p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/GAMRTool_Lib
 * New Version
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 02/05/07   Time: 2:08p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/comm_lib
 *
 *
 */


#ifndef __AMRCTRLFILE_H__
#define __AMRCTRLFILE_H__

#include "common.h"

typedef struct {
	// ---------------------- OUTPUT ------------------------------
	char pszInFileName[256];
    char pszOutFileName[256];
    int  bEnableDTX;
    char pszModeRate[10];
    char pszModeRateFile[255];
    int  iVadOption;
    int  iOutputFormat;
    int  bUseModeFile;

	char pszCompFileName[256];		// compliance file (for testing compliance)
    BOOL bUseCompFile;

} TAMRCtrlFilePublic;

/* ----------------------------------------------------------------------- */
// declare function prototypes which you want them as public member functions
SCODE DLLAPI AMRCtrlFile_Init   (HANDLE *phObject, char *pszFileName);
SCODE DLLAPI AMRCtrlFile_Release(HANDLE *phObject);

// declare public member variables accessing function
DWORD DLLAPI AMRCtrlFile_GetPubVar(HANDLE hObject);

/* ----------------------------------------------------------------------- */
#endif	// __AMRCTRLFILE_H__
