/*
 * Mozart_Audio_SSM2603
 * Driver for Mozart Audio Codec SSM2603.
 *
 * Copyright (C) 2010  ______ Inc.
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

#ifndef _VIVOERRORDEF_
#define _VIVOERRORDEF_

//
//  SCODEs are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-----------------------------+-------------------------------+
//  |S|        Facility             |               Code            |
//  +-+-----------------------------+-------------------------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//


//
// Define the facility codes
//
#define FACILITY_CODEC_VIDEO             1
#define FACILITY_CODEC_AUDIO             2
#define FACILITY_PROTOCOL                3
#define FACILITY_APP                     4
#define FACILITY_DATABASE                5


//
// return type
//
// to avoid conflicting with windows type
#if !defined(__wtypes_h__) && !defined(_SCODE_)
typedef unsigned int SCODE;
#define	_SCODE_
#endif

//
// Severity values
//
#ifndef SEVERITY_SUCCESS
#define SEVERITY_SUCCESS    0
#endif
#ifndef SEVERITY_ERROR
#define SEVERITY_ERROR      1
#endif

//
// Generic test for success on any status value.
//
#ifndef IS_SUCCESS
#define IS_SUCCESS(Status) ((SCODE)(Status) >> 31 == SEVERITY_SUCCESS)
#endif

//
// Generic test for error on any status value.
//
#ifndef IS_FAIL
#define IS_FAIL(Status) ((SCODE)(Status) >> 31 == SEVERITY_ERROR)
#endif

//
// Return the code
//
#ifndef SCODE_CODE
#define SCODE_CODE(sc)      ((sc) & 0xFFFF)
#endif

//
//  Return the facility
//
#ifndef SCODE_FACILITY
#define SCODE_FACILITY(sc)    (((sc) >> 16) & 0x1fff)
#endif

//
// Create an SCODE value from component pieces
//
#ifndef MAKE_SCODE
#define MAKE_SCODE(sev,fac,code) \
    ((SCODE) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#endif

//
// general return codes
//
#ifndef S_OK
#define S_OK                           ((SCODE)  0)
#endif
#ifndef S_FAIL
#define S_FAIL                         ((SCODE) -1)
#endif

//--------------------------------------------------------------------------------
// VNDP Genaral API Error Codes
//--------------------------------------------------------------------------------

//
// MessageId: ERR_INVALID_HANDLE
//
// MessageText:
//
//  Invalid object handle.
//
#define ERR_INVALID_HANDLE            0x80000001

//
// MessageId: ERR_OUT_OF_MEMORY
//
// MessageText:
//
//  Memory allocate fail.
//
#define ERR_OUT_OF_MEMORY             0x80000002

//
// MessageId: ERR_INVALID_ARG
//
// MessageText:
//
//  One or more arguments are invalid.
//
#define ERR_INVALID_ARG               0x80000003

//
// MessageId: ERR_NOT_IMPLEMENT
//
// MessageText:
//
//  Not implemented
//
#define ERR_NOT_IMPLEMENT             0x80000004

//
// MessageId: ERR_INVALID_VERSION
//
// MessageText:
//
//  Invalid version number
//
#define ERR_INVALID_VERSION           0x80000005

#endif //_VIVOERRORDEF_
