/*
 * $Header: /rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRComm_Lib/common.h 3     06/10/16 3:28p Aren $
 *
 * Copyright 2000-2002 ______ , Inc. All rights reserved.
 *
 * common.h : 
 *
 *  header file of common
 *
 * $History: common.h $
 * 
 * *****************  Version 3  *****************
 * User: Aren         Date: 06/10/16   Time: 3:28p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRComm_Lib
 * MODIFICATION: remove static from the macro _INLINE_
 * 
 * *****************  Version 2  *****************
 * User: Aren         Date: 05/02/18   Time: 1:47p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRCOMM_LIB
 * MODIFICATION: define inline for GNUC
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/03/17   Time: 4:18p
 * Created in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRCOMM_LIB
 * Optimized on ARM9
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/02/19   Time: 8:41p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/GAMRCOMM_LIB
 * New Version
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 02/05/07   Time: 2:09p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/comm_lib
 *
 *
 */

/* ------------------------------------------------------------------------ */
#ifndef __COMMON_H__
#define __COMMON_H__

/* ------------------------------------------------------------------------ */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "typedef.h"
#include "global_codec.h"
#include "profile.h"

/* ------------------------------------------------------------------------ */
#if defined(__GNUC__)
#define _INLINE_	__inline__
#else
#define _INLINE_	__inline
#endif

/* ------------------------------------------------------------------------ */
// dllapi declaration
#ifndef DLLAPI
/*
	#if defined(_IMPORTDLL_)
	#pragma message ("Import DLL")
	#define DLLAPI   __declspec(dllimport) __stdcall
	#elif defined(_EXPORTDLL_)
	#pragma message ("Export DLL")
	#define DLLAPI   __declspec(dllexport) __stdcall
	#else
	//#pragma message ("Not DLL")
	#define DLLAPI   __stdcall
	#endif
*/
#endif

/* ------------------------------------------------------------------------ */
extern FILE *_db_fptr;
extern FILE *_log_fptr;

#ifdef _GRAPH_ANAL
extern HANDLE g_hGraphAnal;
#endif

/* ------------------------------------------------------------------------ */
#ifdef _DEBUG
#define DBOPENFILE(S)	{if(S[0] == '\0'){_db_fptr = stdout;} \
						 else if((_db_fptr = fopen(S, "wb")) == NULL) \
						{_db_fptr = stdout;}} 

#define DBCLOSEFILE()	{if(_db_fptr) fclose(_db_fptr);}
#define DBSTR0(S)	fprintf(_db_fptr, S);
#define DBSTR1(S, S1)	fprintf(_db_fptr, S, S1);
#define DBSTR2(S, S1, S2)	fprintf(_db_fptr, S, S1, S2);
#define DBSTR3(S, S1, S2, S3)	fprintf(_db_fptr, S, S1, S2, S3);
#define DBSTR4(S, S1, S2, S3, S4)	fprintf(_db_fptr, S, S1, S2, S3, S4);
#define DBSTR5(S, S1, S2, S3, S4, S5)	fprintf(_db_fptr, S, S1, S2, S3, S4, S5);
#define	DBGSTAT(S)	S
#define MsgBox(S)   MessageBox(NULL, S, "Error Message", MB_OK)
#if defined(__WIN32__) || defined(_WIN32)
#define TRACE0(S)    OutputDebugString(S)
#else
#define TRACE0(S)
#endif
#define TRACE1(S, S1)   {char pchMsg[255]; sprintf(pchMsg, S, S1); TRACE0(pchMsg);}
#define TRACE2(S, S1, S2)   {char pchMsg[255]; sprintf(pchMsg, S, S1, S2); TRACE0(pchMsg);}
#define TRACE3(S, S1, S2, S3)   {char pchMsg[255]; sprintf(pchMsg, S, S1, S2, S3); TRACE0(pchMsg);}
#define TRACE4(S, S1, S2, S3, S4)   {char pchMsg[255]; sprintf(pchMsg, S, S1, S2, S3, S4); TRACE0(pchMsg);}
#define TRACE5(S, S1, S2, S3, S4, S5)   {char pchMsg[255]; sprintf(pchMsg, S, S1, S2, S3, S4, S5); TRACE0(pchMsg);}

#else
#define DBOPENFILE(S)
#define DBCLOSEFILE()
#define DBSTR0(S)
#define DBSTR1(S, S1)
#define DBSTR2(S, S1, S2)
#define DBSTR3(S, S1, S2, S3)
#define DBSTR4(S, S1, S2, S3, S4)
#define DBSTR5(S, S1, S2, S3, S4, S5)
#define	DBGSTAT(S)
#define MsgBox(S)
#define TRACE0(S)
#define TRACE1(S, S1)
#define TRACE2(S, S1, S2)
#define TRACE3(S, S1, S2, S3)
#define TRACE4(S, S1, S2, S3, S4)
#define TRACE5(S, S1, S2, S3, S4, S5)

#endif

// definitions for LOG

#define LOGOPENFILE(S)	{_log_fptr = fopen(S, "wb"); \
						 if(_log_fptr == NULL) _log_fptr = stdout;}

#define LOGCLOSEFILE()	{if(_log_fptr) fclose(_log_fptr);}
#define LOGTIME {time_t t; time(&t); fprintf(_log_fptr, "\t\t%s\r\n", ctime(&t));};
#define LOG0(S)	{fprintf(_log_fptr, S);LOGTIME;};
#define LOG1(S, S1)	{fprintf(_log_fptr, S, S1);LOGTIME;};
#define LOG2(S, S1, S2)	{fprintf(_log_fptr, S, S1, S2);LOGTIME;};
#define LOG3(S, S1, S2, S3)	{fprintf(_log_fptr, S, S1, S2, S3);LOGTIME;};
#define LOG4(S, S1, S2, S3, S4)	{fprintf(_log_fptr, S, S1, S2, S3, S4);LOGTIME;};
#define LOG5(S, S1, S2, S3, S4, S5)	{fprintf(_log_fptr, S, S1, S2, S3, S4, S5);LOGTIME;};


#define false 0
#define true  1

#ifndef S_OK
#define S_OK	0
#endif

#ifndef S_FAIL
#define S_FAIL	1
#endif

#define ABS(a)			(((a) >= 0) ? (a) : (-a))
#define MAX(a, b) 		(((a) > (b)) ? (a) : (b))
#define MIN(a, b) 		(((a) < (b)) ? (a) : (b))
#define SWAP(T, a, b)   {T _tmp; (_tmp) = (T)(a); (T)(a) = (T)(b); (T)(b) = _tmp;}

// 32-bits memory copy macro, nSize must be multiple of 4
#define MEMCPY32(pdwDest, pdwSrc, nSize)\
{\
	int i;\
	for(i = 0; i < ((nSize) / 4); i++)\
		*((long *)pdwDest + i) = *((long *)pdwSrc + i);\
}

// 32-bits zero memory macro, nSize4 must be multiple of 4
#define MEMZERO32(pdwDest, nSize)\
{\
	int i;\
	for(i = 0; i < ((nSize) / 4); i++)\
		*((long *)pdwDest + i) = 0;\
}


/* -------------------------------------------------------------------------- */
#endif	// __COMMON_H__
