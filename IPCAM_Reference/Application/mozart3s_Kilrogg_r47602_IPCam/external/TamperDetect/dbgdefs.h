/*
 *
 * Copyright (C) 2010  VN Inc.
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
 * 
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

#ifdef _DEBUG_L1
#define DBPRINT_L1_0(S)	printf(S);
#define DBPRINT_L1_1(S, S1)	printf(S, S1);
#define DBPRINT_L1_2(S, S1, S2)	printf(S, S1, S2);
#define DBPRINT_L1_3(S, S1, S2, S3)	printf(S, S1, S2, S3);
#define DBPRINT_L1_4(S, S1, S2, S3, S4)	printf(S, S1, S2, S3, S4);
#define DBPRINT_L1_18(S, S1, S2, S3, S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18)  printf(S, S1, S2, S3, S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18);
#else
#define DBPRINT_L1_0(S)
#define DBPRINT_L1_1(S, S1)
#define DBPRINT_L1_2(S, S1, S2)
#define DBPRINT_L1_3(S, S1, S2, S3)
#define DBPRINT_L1_4(S, S1, S2, S3, S4)
#define DBPRINT_L1_18(S, S1, S2, S3, S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18) 
#endif

#ifdef _DEBUG_L2
#define DBPRINT_L2_0(S)	printf(S);
#define DBPRINT_L2_1(S, S1)	printf(S, S1);
#define DBPRINT_L2_2(S, S1, S2)	printf(S, S1, S2);
#define DBPRINT_L2_3(S, S1, S2, S3)	printf(S, S1, S2, S3);
#define DBPRINT_L2_4(S, S1, S2, S3, S4)	printf(S, S1, S2, S3, S4);
#define DBPRINT_L2_18(S, S1, S2, S3, S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18)  printf(S, S1, S2, S3, S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18);
#else
#define DBPRINT_L2_0(S)
#define DBPRINT_L2_1(S, S1)
#define DBPRINT_L2_2(S, S1, S2)
#define DBPRINT_L2_3(S, S1, S2, S3)
#define DBPRINT_L2_4(S, S1, S2, S3, S4)
#define DBPRINT_L2_18(S, S1, S2, S3, S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18) 
#endif

#ifdef _DEBUG_L3
#define DBPRINT_L3_0(S)	printf(S);
#define DBPRINT_L3_1(S, S1)	printf(S, S1);
#define DBPRINT_L3_2(S, S1, S2)	printf(S, S1, S2);
#define DBPRINT_L3_3(S, S1, S2, S3)	printf(S, S1, S2, S3);
#define DBPRINT_L3_4(S, S1, S2, S3, S4)	printf(S, S1, S2, S3, S4);
#define DBPRINT_L3_6(S, S1, S2, S3, S4,S5,S6)   printf(S, S1, S2, S3, S4,S5,S6);

#else
#define DBPRINT_L3_0(S)
#define DBPRINT_L3_1(S, S1)
#define DBPRINT_L3_2(S, S1, S2)
#define DBPRINT_L3_3(S, S1, S2, S3)
#define DBPRINT_L3_4(S, S1, S2, S3, S4)
#define DBPRINT_L3_6(S, S1, S2, S3, S4,S5,S6) 
#endif

#ifdef _DEBUG_L4
#define DBPRINT_L4_0(S)	printf(S);
#define DBPRINT_L4_1(S, S1)	printf(S, S1);
#define DBPRINT_L4_2(S, S1, S2)	printf(S, S1, S2);
#define DBPRINT_L4_3(S, S1, S2, S3)	printf(S, S1, S2, S3);
#define DBPRINT_L4_4(S, S1, S2, S3, S4)	printf(S, S1, S2, S3, S4);
#define DBPRINT_L4_5(S, S1, S2, S3, S4,S5)  printf(S, S1, S2, S3, S4,S5);
#define DBPRINT_L4_6(S, S1, S2, S3, S4,S5,S6)  printf(S, S1, S2, S3, S4,S5,S6);
#define DBPRINT_L4_7(S, S1, S2, S3, S4,S5,S6,S7)  printf(S, S1, S2, S3, S4,S5,S6,S7);
#define DBPRINT_L4_8(S, S1, S2, S3, S4,S5,S6,S7,S8)  printf(S, S1, S2, S3, S4,S5,S6,S7,S8);

#else
#define DBPRINT_L4_0(S)
#define DBPRINT_L4_1(S, S1)
#define DBPRINT_L4_2(S, S1, S2)
#define DBPRINT_L4_3(S, S1, S2, S3)
#define DBPRINT_L4_4(S, S1, S2, S3, S4)
#define DBPRINT_L4_5(S, S1, S2, S3, S4,S5) 
#define DBPRINT_L4_6(S, S1, S2, S3, S4,S5,S6) 
#define DBPRINT_L4_7(S, S1, S2, S3, S4,S5,S6,S7) 
#define DBPRINT_L4_8(S, S1, S2, S3, S4,S5,S6,S7,S8) 
#endif



/* ============================================================================================== */
#endif //__DBGDEFS_H__

/* ============================================================================================== */
