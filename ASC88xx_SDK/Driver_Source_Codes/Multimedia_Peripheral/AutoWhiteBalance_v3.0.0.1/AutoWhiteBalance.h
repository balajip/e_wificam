/*
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

#ifndef __AUTOWHITEBALANCE_H__
#define __AUTOWHITEBALANCE_H__

#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif


/* ============================================================================================== */
#define AUTOWHITEBALANCE_VERSION MAKEFOURCC(3, 0, 0, 1)
#define AUTOWHITEBALANCE_ID_VERSION "3.0.0.1"

/* ============================================================================================== */
/* AWB initial parameters */
typedef struct auto_white_Balance_initial_param
{
	/* AWB Numbers of AWB operating every N frames (SWAWBMode) */
	DWORD dwAWBOpFrames;	
	/* AWB Init Mode */
	DWORD dwAWBInitMode;	
} TAutoWhiteBalanceInitialParam;

/* AWB status structure */
typedef struct auto_white_balance_state
{
	DWORD dwWBHorSubWindowNum; // (i)
	DWORD dwWBVerSubWindowNum; // (i)
	DWORD dwWBWinSize; // (i)
	DWORD *pdwStatisticR; // (i)
	DWORD *pdwStatisticG; // (i)
	DWORD *pdwStatisticB; // (i)
	BOOL bAEStable; // (i)
	DWORD dwShutter; // (i)
	DWORD dwGain; // (i)
	DWORD dwCurrentEV; // (i)

	DWORD dwBlackClampR; // (i)
	DWORD dwBlackClampG; // (i)
	DWORD dwBlackClampB; // (i)

/* Version 3.0.0.0 modification, 2013.06.21 */
	DWORD dwISPGain; // (i)
/* ======================================== */

	BOOL bAWBStable; // (o)
	BOOL bAWBSetValid; // (o)
	DWORD dwCurrGainR; // (o)
	DWORD dwCurrGainB; // (o)
	DWORD dwColorTemp; // (o)
} TAutoWhiteBalanceState;

/* AWB call back function */
typedef struct auto_white_balance_module 
{
	int (*open)(TAutoWhiteBalanceInitialParam* ptParam);
	int (*release)(void);
	int (*set_options)(TVideoSignalOptions *ptArg);
	int (*one_frame)(TAutoWhiteBalanceState *ptState);
	void (*set_color_temperature)(DWORD *pdwParam);
/* Version 3.0.0.0 modification, 2013.06.21 */
	void (*set_bayer_bitwidth)(DWORD dwParam);
/* ======================================== */
} TAutoWhiteBalanceModule;
/* ============================================================================================== */
#endif // __AUTOWHITEBALANCE_H__
