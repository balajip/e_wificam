/*
 * $Header: /rd_2/project/Mozart/Components/VOC/Device_Driver/VOC/dbgdefs.h 4     12/01/12 7:55p Evelyn $
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
 *
 * Description:
 *
 * $History: dbgdefs.h $
 * 
 * *****************  Version 4  *****************
 * User: Evelyn       Date: 12/01/12   Time: 7:55p
 * Updated in $/rd_2/project/Mozart/Components/VOC/Device_Driver/VOC
 * 
 * *****************  Version 3  *****************
 * User: Alan         Date: 08/01/21   Time: 9:12p
 * Updated in $/rd_2/Peripherals/TW2815/TW2815_Lib
 *
 */

/* ============================================================================================== */
#ifndef __DBGDEFS_H__
#define __DBGDEFS_H__

/* ============================================================================================== */
#ifdef _DEBUG
#define DBPRINT0(S)	printf(S);
#define DBPRINT1(S, S1)	printf(S, S1);
#define DBPRINT2(S, S1, S2)	printf(S, S1, S2);
#define DBPRINT3(S, S1, S2, S3)	printf(S, S1, S2, S3);
#define DBPRINT4(S, S1, S2, S3, S4)	printf(S, S1, S2, S3, S4);
#else
#define DBPRINT0(S)
#define DBPRINT1(S, S1)
#define DBPRINT2(S, S1, S2)
#define DBPRINT3(S, S1, S2, S3)
#define DBPRINT4(S, S1, S2, S3, S4)
#endif

/* ============================================================================================== */
#endif //__DBGDEFS_H__

/* ============================================================================================== */
