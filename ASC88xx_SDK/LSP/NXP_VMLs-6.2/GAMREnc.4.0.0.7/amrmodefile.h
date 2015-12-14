/*
 * $Header: /rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRTool_Lib/amrmodefile.h 3     09/02/27 11:58a Angel $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * AMRModeFile.h : 
 *
 *  header file of AMRModeFile, included externally
 *
 * $History: amrmodefile.h $
 * 
 * *****************  Version 3  *****************
 * User: Angel        Date: 09/02/27   Time: 11:58a
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRTool_Lib
 * 
 * *****************  Version 2  *****************
 * User: Angel        Date: 09/01/09   Time: 10:40a
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
 * User: Aren         Date: 02/05/07   Time: 2:11p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/enc_lib
 *
 *
 */


#ifndef __AMRMODEFILE_H__
#define __AMRMODEFILE_H__
/* ----------------------------------------------------------------------- */
/* Version 4.0.0.2 modification,2008.01.08, Angel Hu */
#include "GAMREnc.h"
/* ================================================= */

/* ----------------------------------------------------------------------- */
#define S_EOF   (-1)

/* ----------------------------------------------------------------------- */
// declare function prototypes which you want them as public member functions
SCODE DLLAPI GAMRModeFile_Init   (HANDLE *phObject, char *pszModeFileName);
SCODE DLLAPI GAMRModeFile_Release(HANDLE *phObject);
SCODE DLLAPI GAMRModeFile_ReadMode(HANDLE hObject, DWORD *pdwMode);

/* ----------------------------------------------------------------------- */
#endif	// __AMRMODEFILE_H__
