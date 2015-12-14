/*
 *******************************************************************************
 * $Header: /rd_2/project/SoC/Components/I2SC/Device_Driver/Vaudio/BACH/typedef.h 1     07/06/08 11:25a Aren $
 *
 *  Copyright (c) 2000-2002 VN  Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN  INC.                                                    |
 *  +-----------------------------------------------------------------+
 *
 * $History: typedef.h $
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 07/06/08   Time: 11:25a
 * Created in $/rd_2/project/SoC/Components/I2SC/Device_Driver/Vaudio/BACH
 * 
 * *****************  Version 11  *****************
 * User: Cchuang      Date: 05/09/29   Time: 17:06
 * Updated in $/rd_common/common_header
 * Add #include <limits.h>
 * Change the typedef of SDWORD to int or long by considering INT_MAX and
 * LONG_MAX to fix the bug that long is 64-64 on a 64-bit linux machine.
 * Change the typedef of DWORD in the same reason.
 * 
 * *****************  Version 10  *****************
 * User: Sor          Date: 04/09/01   Time: 5:10p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 8  *****************
 * User: Cchuang      Date: 04/03/10   Time: 23:05
 * Updated in $/rd_2/video/MPEG4/ARM/Decoder/Mp4VDec/GBEngine_Lib
 * Add "typedef signed char SCHAR", some compiler will treat char as
 * unsigned.
 * 
 * *****************  Version 7  *****************
 * User: Cchuang      Date: 03/09/02   Time: 1:13p
 * Updated in $/rd_2/misc/PBENGINE/C/PBEngine/PBEngine_Lib
 * Add typedef of QWORD and SQWORD when defined __GNUC__
 * 
 * *****************  Version 6  *****************
 * User: Cchuang      Date: 03/06/27   Time: 4:02p
 * Updated in $/rd_2/audio/AAC/C/Encoder/AAC4Enc/PBEngine_Lib
 * Add typedef of QWORD and SQWORD when defined __arm
 * 
 * *****************  Version 5  *****************
 * User: Joe          Date: 03/03/12   Time: 1:48p
 * Updated in $/rd_common/common_header
 * Undefine _DOUBLE_PRECISION
 *
 * *****************  Version 4  *****************
 * User: Sor          Date: 03/01/15   Time: 1:55p
 * Updated in $/rd_2/video/MPEG4/C/Decoder/MP4VDEC/Mp4VDec_Lib
 * Change to fit the VNDP requirement.
 *
 * *****************  Version 1  *****************
 * User: Joe          Date: 03/01/13   Time: 3:56p
 * Created in $/rd_common/common_header
 * first add typedef.h to common_header folder
 *
 * *****************  Version 3  *****************
 * User: Joe          Date: 02/12/25   Time: 4:29p
 * Updated in $/rd_1/System/FIRMWARE/common/src
 * Update some naming rules
 *
 * *****************  Version 2  *****************
 * User: Joe          Date: 02/12/19   Time: 2:09p
 * Updated in $/rd_1/System/FIRMWARE/common/src
 * change SCODE definition
 *
 * *****************  Version 1  *****************
 * User: Joe          Date: 02/11/06   Time: 2:54p
 * Created in $/rd_1/System/FIRMWARE/common/src
 *
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2000-2002 VN , Inc. All rights reserved.
 *
 * \file
 * typedef.h
 *
 * \brief
 * Define some useful types, this file should include after OS related include
 * files, such as windows.h, psos.h. currently support windows & psos only
 *
 * \date
 * 2002/07/03
 *
 * \author
 * Joe Wu
 *
 *
 *******************************************************************************
 */

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_
//#include <limits.h>

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
	//#if UINT_MAX == 4294967295
	//	typedef unsigned int	DWORD;
	//#elif ULONG_MAX == 4294967295
		typedef unsigned long   DWORD;
	//#endif // ULONG_MAX == 4294967295
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

#ifdef LINUX26
// bool is keyword in C++
#if !defined(bool) && !defined(__cplusplus)
typedef BOOLEAN         bool;
#endif
#endif

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
	//#if INT_MAX == 2147483647
	//	typedef signed int		SDWORD;
	//#elif LONG_MAX == 2147483647
		typedef signed long     SDWORD;
	//#endif // LONG_MAX == 2147483647
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
