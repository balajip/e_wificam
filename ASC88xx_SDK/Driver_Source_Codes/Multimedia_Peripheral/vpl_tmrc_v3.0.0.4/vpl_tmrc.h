/*
 * $Header: /rd_2/project/Mozart/Components/TMRC/Device_Driver/TMRC/vpl_tmrc.h 1     13/02/04 3:39p Yiming.liu $
 *
 * Copyright 2012 VN, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: vpl_tmrc.h $
 * 
 * *****************  Version 1  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 3:39p
 * Created in $/rd_2/project/Mozart/Components/TMRC/Device_Driver/TMRC
 * 
 * *****************  Version 1  *****************
 * User: Yiming.liu   Date: 13/02/04   Time: 3:38p
 * Created in $/rd_2/project/Mozart/Components/TMRC/TMRC
 * 
 * *****************  Version 10  *****************
 * User: Alan         Date: 09/11/26   Time: 7:49p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Support up to 8 timers and only TM1 will request_irq
 * while others are used for profiling - DONE.
 * MODIFICATION: Move MAX_TMRC_DEV_NUM to vpl_tmrc.h - DONE.
 *
 * *****************  Version 9  *****************
 * User: Alan         Date: 09/11/21   Time: 6:11p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * UPDATE: Update version number from 3.0.0.2 to 3.0.0.3 - DONE.
 *
 * *****************  Version 8  *****************
 * User: Alan         Date: 09/11/17   Time: 5:59p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * UPDATE: Update version number from 3.0.0.1 to 3.0.0.2 - DONE.
 *
 * *****************  Version 7  *****************
 * User: Aniki        Date: 08/09/11   Time: 3:03p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 6  *****************
 * User: Aniki        Date: 07/08/31   Time: 4:17p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 5  *****************
 * User: Aniki        Date: 06/08/02   Time: 2:57p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * FEATURE: Update TMRC driver as the format B2FE driver - DONE.
 * FEATURE: Support multiple open for Timer 0 - DONE.
 *
 * *****************  Version 4  *****************
 * User: Aniki        Date: 05/11/18   Time: 2:30p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 * MODIFICATION: Update the definition of TMRC major number - DONE.
 *
 * *****************  Version 3  *****************
 * User: Aniki        Date: 05/06/06   Time: 9:30p
 * Updated in $/rd_2/project/SoC/Components/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 2  *****************
 * User: Aniki        Date: 05/05/20   Time: 2:51p
 * Updated in $/rd_2/Training/SoC_Development/Reggie/TMRC/Device_Driver/TMRC
 *
 * *****************  Version 1  *****************
 * User: Aniki        Date: 05/05/11   Time: 9:36p
 * Created in $/rd_2/Training/SoC_Development/Reggie/TMRC/Device_Driver/TMRC
 *
 */

/* ============================================================================================= */
#ifndef __VPL_TMRC_H__
#define __VPL_TMRC_H__

/* ============================================================================================= */
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include "typedef.h"
#include "errordef.h"
#include "vivo_codec.h"

/* Version 2.0.0.0 modification, 2006.07.28 */
/* Version 3.0.0.0 modification, 2007.08.31 */
/* Version 3.0.0.1 modification, 2008.09.11 */
#define VPL_TMRC_VERSION MAKEFOURCC(3, 0, 0, 4)
#define VPL_TMRC_ID_VERSION "3.0.0.4"
/* ======================================== */
/* ======================================== */
/* ======================================== */

/* ============================================================================================= */
/* Version 1.1.0.1 modification, 2005.11.18 */
#define TMRC_MAJOR_DEFAULT	0
/* ======================================== */

/* Version 3.0.0.4 modification, 2009.11.26 */
#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
#define MAX_TMRC_DEV_NUM		3
#else //!__ASM_ARCH_PLATFORM_HAYDN_H__
#define MAX_TMRC_DEV_NUM		4
#endif //__ASM_ARCH_PLATFORM_HAYDN_H__
/* ======================================== */

/* ============================================================================================= */
#define TMRC_MAGIC			0xd0

#define TMRC_ENABLE				_IO		(TMRC_MAGIC, 0)
#define TMRC_DISABLE			_IO		(TMRC_MAGIC, 1)
#define TMRC_OVERFLOW_ENABLE	_IO		(TMRC_MAGIC, 2)
#define TMRC_OVERFLOW_DISABLE	_IO		(TMRC_MAGIC, 3)
#define TMRC_MATCH_ENABLE		_IO		(TMRC_MAGIC, 4)
#define TMRC_MATCH_DISABLE		_IO		(TMRC_MAGIC, 5)
#define TMRC_SET_COUNTER		_IOW	(TMRC_MAGIC, 6, DWORD)
#define TMRC_GET_COUNTER		_IO		(TMRC_MAGIC, 7)
#define TMRC_SET_LOAD			_IOW	(TMRC_MAGIC, 8, DWORD)
#define TMRC_SET_MATCH			_IOW	(TMRC_MAGIC, 9, DWORD)
#define TMRC_SET_INCR_COUNT		_IO		(TMRC_MAGIC, 10)
#define TMRC_SET_DECR_COUNT		_IO		(TMRC_MAGIC, 11)
#define TMRC_GET_TICK			_IO		(TMRC_MAGIC, 12)

/* Version 2.0.0.0 modification, 2006.07.28 */
#define VPL_TMRC_IOC_MAX_NUMBER	12
/* ======================================== */

/* ============================================================================================= */
/* Version 1.1.0.0 modification, 2005.06.03 */
typedef struct vpl_tmrc_info
{
	DWORD dwVersion;
	DWORD dwCtrl;
	DWORD dwStat;

	DWORD dwTM0Cnt;
	DWORD dwTM0Load;
	DWORD dwTM0Match;

	DWORD dwTM1Cnt;
	DWORD dwTM1Load;
	DWORD dwTM1Match;

/* Version 3.0.0.4 modification, 2009.11.26 */
	DWORD dwTM2Cnt;
	DWORD dwTM2Load;
	DWORD dwTM2Match;

#if (MAX_TMRC_DEV_NUM > 3)
	DWORD dwTM3Cnt;
	DWORD dwTM3Load;
	DWORD dwTM3Match;
#endif
#if (MAX_TMRC_DEV_NUM > 4)
	DWORD dwTM4Cnt;
	DWORD dwTM4Load;
	DWORD dwTM4Match;
#endif
#if (MAX_TMRC_DEV_NUM > 5)
	DWORD dwTM5Cnt;
	DWORD dwTM5Load;
	DWORD dwTM5Match;
#endif
#if (MAX_TMRC_DEV_NUM > 6)
	DWORD dwTM6Cnt;
	DWORD dwTM6Load;
	DWORD dwTM6Match;
#endif
#if (MAX_TMRC_DEV_NUM > 7)
	DWORD dwTM7Cnt;
	DWORD dwTM7Load;
	DWORD dwTM7Match;
#endif
} TVPLTMRCInfo;
/* ======================================== */
/* ======================================== */

/* ============================================================================================= */
#endif //__VPL_TMRC_H__
