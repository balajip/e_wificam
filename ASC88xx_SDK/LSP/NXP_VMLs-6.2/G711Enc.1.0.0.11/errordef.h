/*
 *******************************************************************************
 * $Header: /rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC/errordef.h 5     11/12/09 17:41 Jon.lin $
 *
 * Driver for EDMC.
 *
 * Copyright 2008 ______ Inc.
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
 *
 * $History: errordef.h $
 *
 * *****************  Version 5  *****************
 * User: Jon.lin      Date: 11/12/09   Time: 17:41
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * add GPL header
 *
 * *****************  Version 2  *****************
 * User: Alan         Date: 08/01/23   Time: 2:01p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 1  *****************
 * User: Joe          Date: 04/04/13   Time: 4:20p
 * Created in $/rd_common/common/SRC
 *
 * *****************  Version 2  *****************
 * User: Joe          Date: 03/07/15   Time: 4:42p
 * Updated in $/rd_1/Project/TM1300_PSOS/FarSeer/common/src
 * Update SCODE definition.
 *
 * *****************  Version 1  *****************
 * User: Joe          Date: 03/03/10   Time: 10:54a
 * Created in $/rd_1/Project/TM1300_PSOS/FarSeer/COMMON/src
 * common used source. Including debug macros, type definitions, error
 * code definitions.
 *
 * *****************  Version 3  *****************
 * User: Allatin      Date: 03/01/13   Time: 5:01p
 * Updated in $/rd_common/common_header
 *
 * *****************  Version 2  *****************
 * User: Joe          Date: 03/01/13   Time: 3:52p
 * Updated in $/rd_common/common_header
 * 1. Change the header
 * 2. Change the definition of SCODE to avoid confliction with Windows
 * 3. Add an Error code ERR_INVALID_VERSION
 *
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * \file
 * errordef.h
 *
 * \brief
 * common error code and macros definitions
 *
 * \date
 * 2003/01/13
 *
 * \author
 * Jason Yang
 *
 *
 *******************************************************************************
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

#define ERR_INVALIDE_CODEC_SYNTAX	  0x80000011
#define ERR_INVALID_PARAMETER		  0x80000012
#define ERR_UNSUPPORT_SOC_PLATFORM 	  0x80000013		//: Some features are only supported on specific SoC platforms.
#define ERR_UNSUPPORT_SOC_MODEL		  0x80000014		//: Some features are only supported on specific SoC models.
#define ERR_UNSUPPORT_OPTION_FLAGS	0x80000015		//: Used in SetOption.
#define ERR_OUT_OF_SYSTEM_MEMORY	0x80000016		//: Used when calloc or malloc failed.
#define ERR_OUT_OF_HARDWARE_MEMORY	0x80000017		//: Used when MemMgr_GetMemory failed.
#define ERR_RELEASE_HARDWARE_MEMORY	0x80000018		//: Used when MemMgr_FreeMemory failed.
#define ERR_OPEN_DEVICE_DRIVER		0x80000019		//: Used when opening device driver failed.
#define ERR_CLOSE_DEVICE_DRIVER		0x8000001A	//: Used when closing device driver failed.
#define ERR_OPEN_LIBRARY			0x8000001B				//: Used when opening another VML failed.
#define ERR_CLOSE_LIBRARY			0x8000001C				//: Used when closing another VML failed.
#define ERR_EXEC_LIBRARY			0x8000001D

#endif //_VIVOERRORDEF_
