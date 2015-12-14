/*
 * $Header: /rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRCOMM_LIB/profile.h 1     03/03/17 4:19p Aren $
 *
 * Copyright 2000-2002 ______ , Inc. All rights reserved.
 *
 * profile.h :
 *
 *  Head file for profile
 *
 * $History: profile.h $
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/03/17   Time: 4:19p
 * Created in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRCOMM_LIB
 * Optimized on ARM9
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/02/19   Time: 8:42p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/GAMRCOMM_LIB
 * New Version
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 02/05/07   Time: 2:09p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/comm_lib
 *
 *
 */


#ifndef __PROFILE_H__
#define __PROFILE_H__

/* ----------------------------------------------------------------------- */
#include <stdio.h>
#include <time.h>
#include "typedef.h"

/* ----------------------------------------------------------------------- */
//#define _PROFILE_

#ifdef _PROFILE_

#ifdef _WIN32
	#define CLK_IN_MHZ	757
	#define GetCycle()	((CLK_IN_MHZ) * clock())
#endif

extern FILE *_prof_fptr;
extern DWORD dwCycleStart;
extern DWORD dwCycleStop;
extern DWORD dwCycleCount;

extern DWORD dwMEMORY;

#define OPENPROFILE(S)	{if(S[0] == '\0'){_prof_fptr = stdout;}			\
						 else if((_prof_fptr = fopen((S), "wb")) == NULL) \
						{_prof_fptr = stdout;}} 

#define CLOSEPROFILE()	{if(_prof_fptr) fclose(_prof_fptr);}

#define PROFILE_START()	{dwCycleStart = (DWORD)(GetCycle());}
#define PROFILE_STOP()	{dwCycleStop  = (DWORD)(GetCycle());}
#define PROFILE_COUNT()	{dwCycleCount = (dwCycleStop-dwCycleStart);}
//#define PROFILE_MIPS	((double)CLK_IN_MHZ * (double)50. * (double)dwCycleCount/(double)CLK_TCK)
#define PROFILE_MIPS	((double)50. * (double)(dwCycleCount)/(double)CLK_TCK)

#define PROFILE_MEM_SUM(dwMemSize)	{(dwMEMORY) += (dwMemSize);}


#define PROSTR0(S)	fprintf(_prof_fptr, S);
#define PROSTR1(S, S1)	fprintf(_prof_fptr, S, S1);
#define PROSTR2(S, S1, S2)	fprintf(_prof_fptr, S, S1, S2);
#define PROSTR3(S, S1, S2, S3)	fprintf(_prof_fptr, S, S1, S2, S3);
#define PROSTR4(S, S1, S2, S3, S4)	fprintf(_prof_fptr, S, S1, S2, S3, S4);
#define PROSTR5(S, S1, S2, S3, S4, S5)	fprintf(_prof_fptr, S, S1, S2, S3, S4, S5);
#define	PROSTAT(S)	S
#define PROFILE_OUT()	{PROFILE_STOP(); PROFILE_COUNT(); PROSTR1("%8.3f MIPS    ",PROFILE_MIPS);}
//#define PROFILE_OUT()	{PROFILE_STOP(); PROFILE_COUNT(); PROSTR2("%8.3f MIPS,  %4d Ticks,  ",PROFILE_MIPS,dwCycleCount);}

#else
#define OPENPROFILE(S)
#define CLOSEPROFILE()
#define PROFILE_START()
#define PROFILE_STOP()
#define PROFILE_COUNT()
#define PROFILE_MIPS
#define PROFILE_MEM_SUM(dwMemSize)
#define PROSTR0(S)
#define PROSTR1(S, S1)
#define PROTR2(S, S1, S2)
#define PROSTR3(S, S1, S2, S3)
#define PROSTR4(S, S1, S2, S3, S4)
#define PROSTR5(S, S1, S2, S3, S4, S5)
#define	PROSTAT(S)
#define PROFILE_OUT()

#endif

/* ----------------------------------------------------------------------- */
#endif	// __PROFILE_H__
