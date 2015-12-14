#ifndef __H264ENC_EXPORT_H__
#define __H264ENC_EXPORT_H__

/* ============================================================================================== */
#include "typedef.h"
#include "errordef.h"
#include "global_codec.h"

/* =========================================================================================== */
typedef struct h264_enc_init_options
{
	DWORD dwVersion;				// encoder version checking
	DWORD dwMaxFrameWidth;			// Maximum process width, it should not exceed input width
	DWORD dwMaxFrameHeight;			// Maximum process height, it should not exceed input height
#ifndef __ASM_ARCH_PLATFORM_ROSSINI_H__
	DWORD dwEncWidth; 				// The encoding frame width
	DWORD dwEncHeight;				// The encoding frame height
	DWORD dwInWidth;				// The input frame width
	DWORD dwInHeight;				// The input frame height
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__

	DWORD dwInHorzOffset;			// Input frame horizontal offset, 16 alignment is better
	DWORD dwInVertOffset;			// Input frame vertical offset, 16 alignment is better

	DWORD dwRefFrameNum;			// H.264 reference frame number. Configure 1 or, 2. Default is 2.
	DWORD dwSearchRangeX;			// H.264 Search range. Configure 1, 2, or 4(1 => +-16, 2 => +-32, 4 => +-60). Default 4.
	DWORD dwSearchRangeY;			// H.264 Search range. Configure 1, 2, or 4(1 => +-16, 2 => +-32, 4 => +-60). Default 4.

#ifndef __ASM_ARCH_PLATFORM_ROSSINI_H__
	DWORD dwStreamBusNum;
	DWORD dwFrameBusNum;
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__
/* ======================================== */
	/* ===============================================================================
	  dwTicksPerSecond: total ticks in every second.
	  dwTicksPerFrame: ticks between two consequential frame.
	  frame per second = dwTicksPerSecond/(dwTicksPerFrame*2).
	  If 30 frame per second. Set dwTicksPerSecond=>60000 and dwTicksPerFrame=>1001.
	  If 15 frame per second. Set dwTicksPerSecond=>30000 and dwTicksPerFrame=>1001.
	  =============================================================================== */
	DWORD dwTicksPerSecond;
	DWORD dwTicksPerFrame;
	DWORD dwQuant;					// Quantization value (1~51).

	DWORD dwIntraSliceInterval;		// Intra-slice interval. there are interval number inter-slices between two Intra-slice

	DWORD dwProfile;				// H.264 profile. Configure 0, 1, 2 (0: baseline, 1: main profile, 2: high profile). Default is 2

	BYTE *pbyObjectMask;            // Mozart V2: the pointer to the object mask information buffer
	SDWORD sdwObjectQp;             // Mozart V2: Object mask object delta Qp
    //BOOL bFastModeEnable;       // Mozart V2: Intra Mode enable/disable in P-Slice
#ifndef __ASM_ARCH_PLATFORM_ROSSINI_H__
	DWORD dwSpeedNum;
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__
	BYTE *pbyMediaBuff;				// Set external allocated buffer start pointer
	DWORD dwMediaBuffSize;			// Set buffer size of external allocated buffer, used to check if external allocated buffer is enough or not.
#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
#ifdef __ROBUST_CONFIG_TEST__
	DWORD dwCurCmprRatio;
	DWORD dwOsdCmprRatio; //AHBC_CMPR_BYPASS;
#endif //__ROBUST_CONFIG_TEST__
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__

	BOOL bIntraSliceRefreshEn;		// Intra slice insert or not. If not, there is no Intra-slice exception for start frame. If yes, Intra-slice would be inserted according to dwIntraSliceInterval.
	BOOL bConstrainedIntraPredEn;	// Constrained intra prediction. H.264 coding performance issue. Default: FALSE
	BOOL bAdvancedSkipModeEn;		// Motion estimation configuration for skip mode. H.264 coding performance issue. Default: TRUE
	BOOL bIntra16x16PredEn;			// Intra 16x16 prediction. H.264 coding performance issue. Default: TRUE
	BOOL bInterChmaSADEn;			// Use chrominance motion estimation result as a mode decision factor. Default: FALSE

	DWORD dwIntraRefreshMBNum;		// Intra refresh MBs in Inter-slice.
	DWORD dwBitRate;				// Bit rate
	ERateCtrlFlags eRCType;			// Rate control flag. Currently support RATE_CTRL_VQCB and RATE_CTRL_STRICT_VQCB

	BOOL bEncByteStreamHdr;

	/* For PB engine initialization */
	DWORD dwBSBufSize;				/* PB engine buffer size */
	EBufTypeFlags eBufTypeFlags;

#ifdef __ROBUST_CONFIG_TEST__
	DWORD dwIntra4x4Cfg;
	DWORD dwIntra8x8Cfg;
	DWORD dwIntra16x16Cfg;
	DWORD dwIntraFMECfg;
#endif //__ROBUST_CONFIG_TEST__

	void *pObjectMem;
#ifndef __ASM_ARCH_PLATFORM_ROSSINI_H__
	BYTE *pbyYFrame; 				// the pointer to the input Y frame buffer
	BYTE *pbyCbFrame; 				// the pointer to the input U frame buffer
	BYTE *pbyCrFrame; 				// the pointer to the input V frame buffer
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__
	BYTE *pbyEncUserData;			// system must guarantee the size of the decoded user data buffer

} TH264EncInitOptions;

typedef struct h264_enc_state
{
	DWORD dwSeconds;
	DWORD dwTicks;
	DWORD dwEncBytes;

	DWORD dwEncUserDataLength;

	DWORD dwNALCount;
	DWORD adwNALBytes[16];
	ENALTypeFlags aeNALType[16];

/* Version 2.1.0.0 modification, 2009.12.04 */
	BOOL bIDR;
/* ======================================== */
	BOOL bLast;
} TH264EncState;

typedef struct h264_roi_windows_infos
{
	BOOL bROIEnable;
	DWORD dwROIWindowNum;	// 0 ~7
	DWORD dwStartMBXNum;
	DWORD dwStartMBYNum;
	DWORD dwEndMBXNum;
	DWORD dwEndMBYNum;
	SDWORD sdwDeltaQp;
	DWORD dwEncodingInterval;
} TH264EncROIWindowsInfos;

typedef struct h264_mb_infos
{
	BYTE byMBTypeSkip;
	SCHAR scMVX;
	SCHAR scMVY;
	BYTE byHdrBits;
	BYTE byRsdBits;
} TH264EncMBInfos;

typedef struct h264_enc_options
{
	EVideoOptionFlags eOptionFlags;
	DWORD adwUserData[3];
} TH264EncOptions;

#endif //__H264ENC_EXPORT_H__
