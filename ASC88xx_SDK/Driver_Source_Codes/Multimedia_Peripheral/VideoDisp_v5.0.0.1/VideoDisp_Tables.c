/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib/VideoDisp_Tables.c 8     11/08/31 2:58p Vincent $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: VideoDisp_Tables.c $
 * 
 * *****************  Version 8  *****************
 * User: Vincent      Date: 11/08/31   Time: 2:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 1. FEATURE: Synchronize with VPL_VOC device driver version 4.3.0.0.
 * 2. MODIFICATION: Support 16-bit YUV with embedded SYNC - DONE.
 * 
 * *****************  Version 7  *****************
 * User: Alan         Date: 11/06/28   Time: 12:42a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Add adwPAL50Hz table - DONE.
 * 
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 11/01/12   Time: 5:39p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * 
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 10/04/26   Time: 11:35a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 10/03/10   Time: 3:53p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 *
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 10/02/26   Time: 2:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoDisp/VideoDisp_Lib
 * MODIFICATION: Add support VIDEO_SIGNAL_SIZE_720x480, VIDEO_SIGNAL_SIZE_1280x720 ,
 * VIDEO_SIGNAL_SIZE_1920x1080 - DONE.
 *
 */
/* ============================================================================================== */
#include "VideoDisp_Locals.h"

/* =========================================================================================== */

const DWORD dwYCbCr2RGBBT601[10]={
0x0000012A,
0x00000199,
0x0000012A,
0x00000064,
0x000000D0,
0x0000012A,
0x00000204,
0x00000010,
0x00000080,
0x00000080
};
/* Version 3.4.0.1 modification, 2011.08.31 */
/* Version 1.1.0.0 modification, 2010.02.26 */
const DWORD adwQQHDTV60Hz[8] = //480x272
{
	533,	//dwOutWidth;
	288,	//dwOutHeight;
	  4,	//dwVsyncFrontPorch;
	  2,	//dwVsyncBackPorch;
	  8,	//dwHsyncFrontPorch;
	  4,	//dwHsyncBackPorch;
	 27,    //ClkFreq
   0xFF     //VICID defiend in CEA-861, 0xFF for undefined format
};

const DWORD adwVGA60Hz[8] = //640x480
{
	800,	//dwOutWidth;
	525,	//dwOutHeight;
	 10,	//dwVsyncFrontPorch;
	 33,	//dwVsyncBackPorch;
	 16,	//dwHsyncFrontPorch;
	 48,	//dwHsyncBackPorch;
	 27,    //ClkFreq
      1     //VICID
};

const DWORD adwNTSC60Hz[8] = //704x480, 720x480
{
	858,	//dwOutWidth;
	525,	//dwOutHeight;
	  9,	//dwVsyncFrontPorch;
	 30,	//dwVsyncBackPorch;
	 16,	//dwHsyncFrontPorch;
	 60,	//dwHsyncBackPorch;
	 27,    //ClkFreq
      2     //VICID
};

/* Version 3.2.0.0 modification, 2011.06.27 */
const DWORD adwPAL50Hz[8] = //704x576, 720x576
{
	864,	//dwOutWidth;
	625,	//dwOutHeight;
	  5,	//dwVsyncFrontPorch;
	 39,	//dwVsyncBackPorch;
	 12,	//dwHsyncFrontPorch;
	 68,	//dwHsyncBackPorch;
	 27,    //ClkFreq
     17     //VICID
};
/* ======================================== */

const DWORD adwPAL60Hz[8] = //768x576
{
	976,	//dwOutWidth;
	597,	//dwOutHeight;
	  1,	//dwVsyncFrontPorch;
	 17,	//dwVsyncBackPorch;
	 24,	//dwHsyncFrontPorch;
	104,	//dwHsyncBackPorch;
	 27,    //ClkFreq
   0xFF     //VICID
};

const DWORD adwWVGA60Hz[8] = //800x480
{
	1056,	//dwOutWidth;
	 525,	//dwOutHeight;
	  13,	//dwVsyncFrontPorch;
	  29,	//dwVsyncBackPorch;
	  40,	//dwHsyncFrontPorch;
	  60,	//dwHsyncBackPorch;
	  27,   //ClkFreq
    0xFF    //VICID
};

const DWORD adwSVGA60Hz[8] = //800x600
{
	1056,	//dwOutWidth;
	 628,	//dwOutHeight;
	   1,	//dwVsyncFrontPorch;
	  23,	//dwVsyncBackPorch;
	  40,	//dwHsyncFrontPorch;
	  88,	//dwHsyncBackPorch;
	  27,   //ClkFreq
    0xFF    //VICID
};

const DWORD adwXGA60Hz[8] = //1024x768
{
	1344,	//dwOutWidth;
	 806,	//dwOutHeight;
	   3,	//dwVsyncFrontPorch;
	  29,	//dwVsyncBackPorch;
	  24,	//dwHsyncFrontPorch;
	 160,	//dwHsyncBackPorch;
	  27,   //ClkFreq
    0xFF    //VICID
};

const DWORD adwSXGA60Hz[8] = //1280x1024
{
	1688,	//dwOutWidth;
	1066,	//dwOutHeight;
	   1,	//dwVsyncFrontPorch;
	  38,	//dwVsyncBackPorch;
	  48,	//dwHsyncFrontPorch;
	 248,	//dwHsyncBackPorch;
	 108,   //ClkFreq
    0xFF    //VICID
};

const DWORD adwWXGA60Hz[8] = //1280x768
{
	1680,	//dwOutWidth;
	 828,	//dwOutHeight;
	   1,	//dwVsyncFrontPorch;
	  24,	//dwVsyncBackPorch;
	  64,	//dwHsyncFrontPorch;
	 200,	//dwHsyncBackPorch;
	  27,   //ClkFreq
    0xFF    //VICID
};

const DWORD adw720p60Hz[8] = //1280x720
{
	1650,	//dwOutWidth;
	 750,	//dwOutHeight;
	   5,	//dwVsyncFrontPorch;
	  20,	//dwVsyncBackPorch;
	 110,	//dwHsyncFrontPorch;
	 220,	//dwHsyncBackPorch;
	  74,   //ClkFreq
       4    //VICID
};

const DWORD adwSXGA_960_60Hz[8] = //1280x960
{
	1712,	//dwOutWidth;
	 994,	//dwOutHeight;
	   1,	//dwVsyncFrontPorch;
	  30,	//dwVsyncBackPorch;
	  80,	//dwHsyncFrontPorch;
	 216,	//dwHsyncBackPorch;
	 108,   //ClkFreq
    0xFF    //VICID
};

const DWORD adwSXGAPLUS60Hz[8] = //1400x1050
{
	1863,	//dwOutWidth;
	1087,	//dwOutHeight;
	   1,	//dwVsyncFrontPorch;
	  33,	//dwVsyncBackPorch;
	  88,	//dwHsyncFrontPorch;
	 240,	//dwHsyncBackPorch;
	 121,   //ClkFreq
    0xFF,   //VICID
};

const DWORD adw1080p60Hz[8] = //1920x1080
{
	2200,	//dwOutWidth;
	1125,	//dwOutHeight;
	   4,	//dwVsyncFrontPorch;
	  36,	//dwVsyncBackPorch;
	  88,	//dwHsyncFrontPorch;
	 148,	//dwHsyncBackPorch;
	 148,   //ClkFreq
      16    //VICID
};
/* ======================================== */

/* Version 2.0.0.2 modification, 2010.04.26 */
const DWORD adw1080p24Hz[8] = //1920x1080@24Hz
{
	2750,	//dwOutWidth;
	1125,	//dwOutHeight;
	   4,	//dwVsyncFrontPorch;
	  36,	//dwVsyncBackPorch;
	 638,	//dwHsyncFrontPorch;
	 148,	//dwHsyncBackPorch;
	  74,   //ClkFreq
      32    //VICID
};
/* ======================================== */

/* Version 3.1.0.0 modification, 2011.01.12 */
const DWORD adw1080p30Hz[8] = //1920x1080
{
	2200,	//dwOutWidth;
	1125,	//dwOutHeight;
	   4,	//dwVsyncFrontPorch;
	  36,	//dwVsyncBackPorch;
	  88,	//dwHsyncFrontPorch;
	 148,	//dwHsyncBackPorch;
	  74,   //ClkFreq
      34    //VICID
};
/* ======================================== */
/* ======================================== */
/* =========================================================================================== */
