/*
 * Copyright (C) 2007  ______ Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#if defined(_WIN32)
	#include <limits.h>
#else //_WIN32
	#include <linux/version.h>

	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0))
	#include <limits.h>
	#endif
#endif //_WIN32

// to avoid confliction if including "windows.h"
#ifndef _WINDOWS_
// the following types are already define in windows.h

#ifndef __TYPES_H__		// for psos
// the following types are already define in psos.h

#ifndef UCHAR	// uc
typedef unsigned char   UCHAR;
#endif

#ifndef ULONG	// ul
typedef unsigned long   ULONG;
#endif

#ifndef USHORT	// us
typedef unsigned short  USHORT;
#endif

#ifndef UINT	// ui
typedef unsigned int    UINT;
#endif

#endif	// end of __TYPES_H__	// for psos

//  1 byte
#ifndef char	// c
typedef char            CHAR;
#endif

#ifndef PCHAR	// pc
typedef char            *PCHAR;
#endif

#ifndef PUCHAR	// puc
typedef unsigned char   *PUCHAR;
#endif

#ifndef BYTE	// by
typedef unsigned char   BYTE;
#endif

#ifndef PBYTE	// pby
typedef BYTE*           PBYTE;
#endif

//  2 bytes
#ifndef short	// s
typedef short           SHORT;
#endif

#ifndef PSHORT	// ps
typedef short           *PSHORT;
#endif

#ifndef PUSHORT	// pus
typedef unsigned short  *PUSHORT;
#endif

#ifndef WORD	// w
typedef unsigned short  WORD;
#endif

#ifndef PWORD	// pw
typedef WORD*           PWORD;
#endif

//  4 bytes
#ifndef DWORD	// dw
	#if defined(_WIN32)
		#if defined(__arm)	// for ADS compiler -- armcc
			typedef unsigned long   DWORD;
		#elif UINT_MAX == 4294967295
			typedef unsigned int	DWORD;
		#elif ULONG_MAX == 4294967295
			typedef unsigned long   DWORD;
		#endif // ULONG_MAX == 4294967295
	#else
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 4, 0))
			typedef unsigned long   DWORD;
		#elif defined(__arm)	// for ADS compiler -- armcc
			typedef unsigned long   DWORD;
		#elif UINT_MAX == 4294967295
			typedef unsigned int	DWORD;
		#elif ULONG_MAX == 4294967295
			typedef unsigned long   DWORD;
		#endif // ULONG_MAX == 4294967295
	#endif //_WIN32
#endif

#ifndef PDWORD	// pdw
typedef DWORD*          PDWORD;
#endif

#ifndef PUINT	// pui
typedef UINT*           PUINT;
#endif

#ifndef long	// l
typedef long            LONG;
#endif

#ifndef PLONG	// pl
typedef long            *PLONG;
#endif

#ifndef PULONG	// plu
typedef unsigned long   *PULONG;
#endif


#ifndef BOOLEAN	// b
typedef unsigned int    BOOLEAN;
#endif

#ifndef BOOL	// b
typedef BOOLEAN         BOOL;
#endif

// bool is keyword in C++
#if defined(_WIN32)
	#if !defined(bool) && !defined(__cplusplus)
	typedef BOOLEAN         bool;
	#endif
#else
	#if !defined(bool) && !defined(__cplusplus) && (LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0))
	typedef BOOLEAN         bool;
	#endif
#endif //_WIN32

#ifndef PVOID	// pv
typedef void *          PVOID;
#endif

#ifndef HANDLE	// h
typedef void *          HANDLE;
#endif

/* the new SCODE is defined in errordef.h
#ifndef SCODE	// sc
typedef signed int	SCODE;
#endif
*/
// --------- for floating point -------------
#ifndef FLOAT	// fl
#ifdef _DOUBLE_PRECISION
	typedef double FLOAT;
#else
	typedef float  FLOAT;
#endif

#endif

#endif

// non-window conflict types

#ifndef SCHAR   //c
typedef signed char     SCHAR;
#endif

#ifndef SWORD	// sw
typedef signed short    SWORD;
#endif

#ifndef SDWORD	// sdw
	#if defined(_WIN32)
		#if INT_MAX == 2147483647
			typedef signed int		SDWORD;
		#elif LONG_MAX == 2147483647
			typedef signed long     SDWORD;
		#endif // LONG_MAX == 2147483647
	#else
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 4, 0))
			typedef signed int		SDWORD;
		#elif INT_MAX == 2147483647
			typedef signed int		SDWORD;
		#elif LONG_MAX == 2147483647
			typedef signed long     SDWORD;
		#endif // LONG_MAX == 2147483647
	#endif
#endif

#ifndef TASK
typedef void            TASK;
#endif

#if defined(_WIN32)
	typedef unsigned __int64 	QWORD;	// qw
	typedef __int64				SQWORD;	// sqw
#elif defined(_EQUATOR_X_) || defined(__arm)
	typedef unsigned long long	QWORD;	// qw
    typedef long long           SQWORD;	// sqw
#elif defined(__GNUC__)
	typedef unsigned long long int	QWORD;	// qw
    typedef long long int           SQWORD;	// sqw
#endif


// --------- for fix point -------------
typedef signed short     FIX16;
typedef unsigned short   UFIX16;
typedef signed long      FIX;
typedef unsigned long    UFIX;


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define ON  1
#define OFF 0

#ifndef S_OK
#define S_OK 0
#endif

#ifndef S_FAIL
#define S_FAIL (SCODE)(-1)
#endif

#ifndef S_INVALID_VERSION
#define S_INVALID_VERSION (SCODE)(-2)
#endif

#endif // _TYPEDEF_H_
