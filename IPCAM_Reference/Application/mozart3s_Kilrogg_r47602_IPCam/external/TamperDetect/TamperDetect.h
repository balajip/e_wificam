/*
 * $Header: 
 *
 * Copyright 2008 VN, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: TamperDetect.h $
 * 
*
*/
#ifndef __TAMPER_DETECT_H__
#define __TAMPER_DETECT_H__

#include <VideoCap.h>
#include "dbgdefs.h"
#include <typedef.h>

extern SDWORD GetParam(FILE *pfInput);

extern SCODE
VideoCap_TamperDetect(
							HANDLE			hObject, 
							TVideoCapState *	ptState,
							BOOL *			pbOutOfFocus,
							PDWORD 		pdwStatFocusBuf,
							PBYTE			pbyStatHistoBuf
							);

extern BOOL
IsFocusValueReduced(
						TVideoCapState *ptState, 
						DWORD dwOOF_Prev_High,
						DWORD dwOOF_Prev_Low, 
						DWORD dwFocus_Diff_Thres_High,
						DWORD dwFocus_Diff_Thres_Low
						);

extern BOOL 
IsSceneChanged(
        				TVideoCapState *ptState, 
        				QWORD qw_R_Avg ,
        				QWORD qw_R_Thres ,
        				QWORD qw_G_Avg ,
        				QWORD qw_G_Thres ,
        				QWORD qw_B_Avg ,
        				QWORD qw_B_Thres
        				);

#endif /*__TAMPER_DETECT_H__ */
