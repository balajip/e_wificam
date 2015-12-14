/*

/* ============================================================================================== */
#ifndef __VPL_VIC_LOCALS_H__
#define __VPL_VIC_LOCALS_H__

/* ============================================================================================== */
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/cache.h>
#include <asm/arch/platform.h>
#include <asm/arch/irq.h>

#include "typedef.h"
#include "vpl_vic.h"
#include "AutoExposure.h"
#include "autofocus.h"
/* Version 11.0.0.2 modification, 2012.10.24 */
#include "AutoWhiteBalance.h"
/* ========================================= */

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
    #ifdef __KERNEL__
        #define LOGLEVEL KERN_DEBUG
        #define PDEBUG(fmt, args...) printk(LOGLEVEL "VPL_VIC: " fmt, ## args)
    #else
        #define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
    #endif
#else
    #define PDEBUG(fmt, args...)
#endif


/* ============================================================================================== */
#define MAX_VIC_OPEN_NUM_DEFAULT 5
#define MAX_EnhancedBayer_WIDTH	2560
#define SYSC_CLK_EN_MMR (IO_ADDRESS(VPL_SYSC_MMR_BASE)+0x28)
#define INTC_VSYNC_STATUS_MMR (IO_ADDRESS(MOZART_INTC_MMR_BASE)+0x08)
#define INTC_VSYNC_CLEAR_MMR (IO_ADDRESS(MOZART_INTC_MMR_BASE)+0x30)
#define VPL_VIC_CLK_EN_NUM	0x7
extern struct file_operations vpl_vic_fops;

/* ============================================================================================== */
typedef struct TVideoCapBufInfo TVideoCapBufInfo;
struct TVideoCapBufInfo
{
    DWORD dwSecond;
    DWORD dwMicroSecond;
    BOOL bTimeStampCal;
    DWORD dwFrameCount;
    DWORD dwBufY;
    DWORD dwBufCb;
    DWORD dwBufCr;
    DWORD dwAFFocusValueHigh;
    DWORD dwAFFocusValueLow;
    DWORD dwAFCurrentStatus;
    QWORD qwAWBRedSum;
    QWORD qwAWBGreenSum;
    QWORD qwAWBBlueSum;
    DWORD dwAWBGain;
    DWORD dwInWidth;		// input width calculated by VIC
    DWORD dwOutWidth;
    DWORD dwOutHeight;
    DWORD dwOutStride;
    DWORD dwErrACK;     // [4]: FERR
                        // [3]: Field
                        // [2]: No signal error
                        // [1]: FIFO full error
                        // [0]: OP CMPT
    volatile BOOL bReadyForCapture;
    volatile BOOL bReadyForProcess;
    volatile BOOL bReadyForSlvProcess;
    DWORD dwInUseNum;
    DWORD dwIndex;
    TVideoCapBufInfo *ptNextBuf;
    TVideoCapBufInfo *ptPreBuf;
    BOOL bFlip;
} ;

typedef struct vpl_vic_obj_info
{
    BOOL bInitialized;
    DWORD dwProcIndex;
    DWORD dwPreFrameCount;
#ifndef __USE_LAST_FRAME__
    DWORD dwProcBufIndex;
#endif
    TVideoCapBufInfo **aptProcBuf;
    TVideoCapBufInfo *ptCurrGetBuf;     // pointer to the next buffer
    DWORD dwBufNum;                     // number of buffers owned by this process
    BOOL bIsMaster;
} TVPLVICObjInfo;

typedef struct vpl_vic_dev_chn_info
{
    wait_queue_head_t wq;

    BOOL bSetOption;
    BOOL bHalfSizedOutput;  // enable half sized frame output
    BOOL bHalfSize;         // true if current size if half the full size
    BOOL bStartOutput;      // true when starting output video frames
    DWORD dwLatestTS;       // the time-stamp for the latest frame input
    DWORD dwInputInterval;  // used for input frame interval
    DWORD dwFrameRate;      // used for frame rate control
    DWORD dwPrevTicks;      // used for frame rate control
    DWORD dwInterval;       // used for frame rate control
    DWORD dwIntervalResidue;// used for frame rate control
    DWORD dwDiffResidue;    // used for frame rate control
    DWORD dwFrameCount;     // used for frame rate control
    DWORD dwCapHeight;      // capture height for full size
    DWORD dwVertSS;         // vertical subsampling for full size
    DWORD dwInWidth;		// input width from the library
    BOOL bStopCapture;
    volatile BOOL abIntFlag[MAX_VIC_OPEN_NUM_DEFAULT];
    DWORD dwIntFlagUsed;
    TVideoCapBufInfo *ptProcessBuf;     // the latest available buffer
    TVideoCapBufInfo *ptCaptureBuf;     // buffer assigned in VIC module
    TVideoCapBufInfo *ptMasterBuf;
    TVPLVICOptions tOptions;

    TVideoCapBufInfo *ptBufInfo;
    DWORD dwBaseSecond;
    DWORD dwBaseMicroSecond;
    BOOL bInitialized;
    DWORD dwChnInUse;
    DWORD dwPhyBaseAddr;
    DWORD dwBufNum;
    DWORD dwMask;
    BOOL bOutFormat;
    DWORD dwMaxFrameWidth;
    DWORD dwMaxFrameHeight;
    DWORD dwFrameCountAll;
    DWORD dwErrACK;     // [5]: ccir656 protocol error or width check error
                        // [4]: FERR
                        // [3]: Field
                        // [2]: No signal error
                        // [1]: FIFO full error
                        // [0]: OP CMPT
    BOOL bFlip;
    DWORD dwFlipDelayCnt;
    DWORD dwFrameNum;
    DWORD dwFrameNumCnt;
    DWORD dwFrameNumDelay;
    DWORD dwRealFrameNum;
    BOOL  bStateUpdate;
    QWORD qwLatestAFFocusValue;
    DWORD adwAEMeasureWinTbl[2];
    ETVStd eStd;    	// TVSTD_NTSC or TVSTD_PAL
    BOOL bSetSize;
    DWORD dwAFState; //evetest : 0->idle, 1->wait, 2->pre-AF, 3->AF, 4-> manual focus, 5->zoom in/out
    DWORD dwManualDir;
    DWORD adwAFMeasureWinTbl[2];	
    DWORD dwShutter;
    DWORD dwGain;
    DWORD dwZoomRatio;
    DWORD dwCFAAntiAliasingTbl[10];
    DWORD dwDeImpulseTbl[12];
    BOOL bAEStable;
    DWORD dwCurrentEV;
    EVideoSignalFormat eFormat;
    TVPLVICISPParam tVPLVICISPParam;
} TVPLVICDevChnInfo;

typedef struct vpl_vic_dev_info
{
    volatile TVPLVICInfo *ptMMRInfo;

    DWORD *pdwBandwidth;
    DWORD *pdwRGInterval;
    DWORD *pdwReqTimes;
    DWORD *pdwProfileClr;
    DWORD *pdwAHBCCtrl;

    DWORD dwTotalRqTimes;
    DWORD dwPrevRqTimes;
    DWORD dwTotalRqGntInterval;
    DWORD dwPrevRqGntInterval;
    DWORD dwTotalBandwidth;
    DWORD dwPrevBandwidth;
#ifdef __USE_PROFILE__
    DWORD dwMaxISRInterval;
    DWORD dwMinISRInterval;
#endif //__USE_PROFILE__
    DWORD dwIrq;

    TVPLVICDevChnInfo atDevChnInfo[VPL_VIC_CHN_NUM];
    struct work_struct	aewb_oneframe_work;	//for AE&AWB : oneframe work queue structure
    DWORD bAEWBOneFrameOnGoing; // 0: AE&AWB one frame control stop, 1: AE&AWB one frame control's on going
    struct work_struct	af_oneframe_work;	//for AF : oneframe work queue structure
    DWORD bAFOneFrameOnGoing; // 0: AF one frame control stop, 1: AF one frame control's on going
    struct work_struct	auto_detect_work;	//for auto detec standard
    DWORD bAutoDetectOnGoing; // 0: control stop, 1: control's on going
    DWORD dwNum;
    DWORD *pdwStatBuff;
    DWORD *pdwStatAEWBBuf;
    DWORD *pdwStatHistoBuf;
    DWORD *pdwStatFocusBuf;
/* Version 11.0.0.3 modification, 2012.11.02 */
    DWORD dwStatAEWBPhyBaseAddr;
    DWORD dwStatHistoPhyBaseAddr;
    DWORD dwStatFocusPhyBaseAddr;
/* ========================================= */
} TVPLVICDevInfo;

/* ============================================================================================== */
#endif //__VPL_VIC_LOCALS_H__

/* ============================================================================================== */
