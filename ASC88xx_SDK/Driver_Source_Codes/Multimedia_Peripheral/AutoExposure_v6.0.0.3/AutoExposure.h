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

#ifndef __AUTOEXPOSURE_H__
#define __AUTOEXPOSURE_H__

#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif


/* ============================================================================================== */
#define AUTOEXPOSURE_VERSION MAKEFOURCC(6, 0, 0, 3)
#define AUTOEXPOSURE_ID_VERSION "6.0.0.3"

/* ============================================================================================== */
/* AE initial parameters */
typedef struct auto_exposure_initial_param
{
	/* AE Numbers of AE operating every N frames (SWAEMode) */
	DWORD dwAEopframes;	
	/* AE Target luminace (SWAEMode) */
	DWORD dwTargetLuminance;
	/* AE Luminace offset (SWAEMode) */
	DWORD dwTargetOffset; // should be greater than FINE_TUNE_RANGE (5)
	/* AE Shutter default width */
	DWORD dwDefaultShutter;
	/* AE Gain default value (SWAEMode: related to DefaultGain) */
	DWORD dwDefaultGain;
	/* AE Shutter width lower bound (SWAEMode: related to MinShutter), 0 stands for "lower bound don't care" */
	DWORD dwMinShutter;
	/* AE Shutter width upper bound (SWAEMode: related to MaxShutter), 1/2/4/8/15/30/60/120 */
	DWORD dwMaxShutter;
	/* AE Analog gain lower bound (SWAEMode: MinGain), 1-64 */
	DWORD dwMinGain;
	/* AE Analog gain upper bound (SWAEMode: MaxGain), 1-64 */
	DWORD dwMaxGain;
	/* AE cooperation scheme
			shutter & gain | DC iris
		0:  on			   | on 
		1:  on			   | off
		2:  off			   | on
		3:  off            | off
	*/
	DWORD dwAECoopMode;
	/* Identify sensor IIC setting timming must Sync Vsync Falling */
	BOOL bIICSyncVsyncFallingEn;
	/* Control iris device type */
	DWORD dwIrisCtlType;
	/* P iris total step */
	DWORD dwPIrisTotalStep;
	/* P iris current step */
	DWORD dwPIrisCurStep;
}TAutoExposureInitialParam;

/* AE status structure */
typedef struct auto_exposure_state
{
	QWORD aqwHWAELuminance[10]; // (i)
	DWORD adwWindowPixels[10]; // (i)

	DWORD dwLumaHorSubWindowNum; // (i)
	DWORD dwLumaVerSubWindowNum; // (i)
	DWORD dwLumaWinSize; // (i)
	DWORD *pdwStatisticY; // (i)

	DWORD dwWBHorSubWindowNum; // (i)
	DWORD dwWBVerSubWindowNum; // (i)
	DWORD dwWBWinSize; // (i)
	DWORD *pdwStatisticR; // (i)
	DWORD *pdwStatisticG; // (i)
	DWORD *pdwStatisticB; // (i)

	DWORD dwHistogramBinNum; // (i)
	DWORD *pdwHistogram; // (i)

	DWORD dwISPGain; // (i)

	DWORD dwCurrShutter; // (o)
	DWORD dwCurrGain; // (o)
	BOOL bShutterValid; // (o)
	BOOL bGainValid; // (o)

	TIrisCtrlInfo tIrisCtrlInfo; // (o) Control iris motor info.
	BOOL bIrisCtrlValid; // (o)
	ELensControlFlags eIrisCtrlCmd;// (o)

	BOOL bAEStable; // (o)
	DWORD dwCurrentEV; // (o)
} TAutoExposureState;

/* AE call back function */
typedef struct auto_exposure_module 
{
	int (*open)(TAutoExposureInitialParam* ptParam);
	int (*release)(void);
	int (*set_options)(TVideoSignalOptions *ptArg);
	void (*one_frame)(TAutoExposureState *ptState);
	void (*set_bayer_bitwidth)(DWORD dwParam);
} TAutoExposureModule;
/* ============================================================================================== */
#endif // __AUTOEXPOSURE_H__
