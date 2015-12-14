/*
 * $Header: /rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC/vpl_edmc.h 59    14/03/21 3:05p Morgan.chang $
 *
 * Driver for EDMC.
 *
 * Copyright 2006 ______ , Inc. 
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
 * $History: vpl_edmc.h $
 * 
 * *****************  Version 59  *****************
 * User: Morgan.chang Date: 14/03/21   Time: 3:05p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * footprint / cache status changed
 *     
 * 1. set chachable for any memory allocation.
 * 2. /sys/class/vpl_edmc/footprint shows allocations of each DRAM
 * separately.
 * 
 * *****************  Version 58  *****************
 * User: Morgan.chang Date: 14/02/12   Time: 3:01p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Print more than 1 page data of mappings in /proc/vpl_edmc_maps
 * 
 * *****************  Version 57  *****************
 * User: Morgan.chang Date: 13/10/04   Time: 4:29p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Backtrack version number to 6.0.0.8
 *     
 * In order to be compatible with old memory management library.
 * 
 * *****************  Version 56  *****************
 * User: Morgan.chang Date: 13/10/03   Time: 4:32p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Re-version 6.0.0.8 to 6.0.1.0
 * 
 * *****************  Version 55  *****************
 * User: Morgan.chang Date: 13/10/03   Time: 4:21p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Allocate all DRAM memory out of control of kernel to EDMC driver.
 * 
 * Since all memory out of kernel's control should be devoted to media
 * usage, edmc driver parses the kernel command line 'mem=' and gets
 * all the left memory.
 * So the module parameter dwMediaMemSize is abandoned !
 * 
 * *****************  Version 54  *****************
 * User: Morgan.chang Date: 13/04/11   Time: 7:14p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 1. FIX: Fix dram number sequencing.
 *    Dram number depends on the order of memory base addresses, 
 *    instead of the values of base addresses.
 * 
 * *****************  Version 53  *****************
 * User: Jon.lin      Date: 13/03/22   Time: 15:45
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 52  *****************
 * User: Morgan.chang Date: 12/03/30   Time: 4:42p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 1. REFINEMENT: EDMC bus number is decided by base address set in dram
 * controller which should be identical with slave base address in AHB
 * controller. Should be with Mozart v2 loader with version 12033001 or
 * newer.
 * 
 * *****************  Version 51  *****************
 * User: Jon.lin      Date: 12/01/18   Time: 15:39
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [6.0.0.4] bug fixed
 * 
 * *****************  Version 50  *****************
 * User: Poplar       Date: 11/12/30   Time: 11:17a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version 6.0.0.3
 * 
 * *****************  Version 49  *****************
 * User: Jon.lin      Date: 11/12/09   Time: 17:44
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * add GPL header
 * 
 * *****************  Version 48  *****************
 * User: Acer.tsai    Date: 11/08/02   Time: 11:53a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [REFINEMENT] : rename DRAMInfo to MediaSize
 * 
 * *****************  Version 47  *****************
 * User: Acer.tsai    Date: 11/08/01   Time: 4:06p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [commit] FAATURE: Add sysfs to export DRAM size on MOZART platform.
 * 
 * *****************  Version 47  *****************
 * User: Acer.Tsai      Date: 11/08/01   Time: 3:30p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version 6.0.0.1
 * FAATURE: Add sysfs to export DRAM size on MOZART platform
 * *****************  Version 46  *****************
 * User: Jon.lin      Date: 10/12/08   Time: 9:43a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version 6.0.0.0
 * 
 * *****************  Version 45  *****************
 * User: Poplar       Date: 10/03/08   Time: 7:23p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Add ioctl command, VPL_EDMC_IOC_CHECK_MAPPED_REGION to map
 * host side memory.
 * 
 * *****************  Version 44  *****************
 * User: Tassader.chen Date: 10/02/03   Time: 10:56a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 43  *****************
 * User: Poplar       Date: 10/01/30   Time: 6:32p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 1. FEATURE: Modify the way to caluate the size of DRAM.
 * 
 * *****************  Version 42  *****************
 * User: Poplar       Date: 10/01/29   Time: 5:27p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version: 5.0.0.12
 * 
 * *****************  Version 41  *****************
 * User: Cchuang      Date: 10/01/27   Time: 18:11
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 5.0.0.10 to 5.0.0.11 - DONE.
 * 
 * *****************  Version 38  *****************
 * User: Jon.lin      Date: 09/05/20   Time: 3:22p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * bug fixed
 * 
 * *****************  Version 37  *****************
 * User: Jon.lin      Date: 09/03/19   Time: 2:50p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 36  *****************
 * User: Jon.lin      Date: 09/03/19   Time: 11:50a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * for Mozart release
 * 
 * *****************  Version 35  *****************
 * User: Jon.lin      Date: 08/12/31   Time: 8:56p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Vision 5.0.0.6
 * 
 * *****************  Version 34  *****************
 * User: Ycchang      Date: 08/09/15   Time: 9:12a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 33  *****************
 * User: Sor          Date: 08/01/16   Time: 11:39a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 5.0.0.3 to 5.0.0.4 - DONE.
 * 
 * *****************  Version 32  *****************
 * User: Jon.lin      Date: 08/01/14   Time: 8:33p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *  fix cache operation in Linux 2.6.20.4
 * 
 * *****************  Version 31  *****************
 * User: Sor          Date: 07/12/25   Time: 9:20p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 5.0.0.1 to 5.0.0.2 - DONE.
 * 
 * *****************  Version 30  *****************
 * User: Alan         Date: 07/12/25   Time: 9:14p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 29  *****************
 * User: Ycchang      Date: 07/09/04   Time: 2:35p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 28  *****************
 * User: Ycchang      Date: 07/08/16   Time: 12:49p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 27  *****************
 * User: Vincent      Date: 07/01/26   Time: 8:18p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 4.1.0.1 to 4.1.0.2- DONE.
 * 
 * *****************  Version 26  *****************
 * User: Sor          Date: 07/01/02   Time: 7:41p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 4.1.0.0 to 4.1.0.1- DONE.
 * 
 * *****************  Version 25  *****************
 * User: Sor          Date: 06/12/11   Time: 6:19p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 4.0.0.2 to 4.1.0.0 - DONE.
 * 
 * *****************  Version 24  *****************
 * User: Sor          Date: 06/11/01   Time: 4:28p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 4.0.0.1 to 4.0.0.2 - DONE.
 * 
 * *****************  Version 23  *****************
 * User: Sor          Date: 06/10/16   Time: 6:57p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 4.0.0.0 to 4.0.0.1 - DONE.
 * 
 * *****************  Version 22  *****************
 * User: Sor          Date: 06/07/10   Time: 5:31p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 3.0.0.0 to 4.0.0.0 - DONE.
 * 
 * *****************  Version 21  *****************
 * User: Sor          Date: 06/07/10   Time: 5:31p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Remove VPL_EDMC_IOC_SET_CACHABLE support, all memory region in
 * system bus will be always cacheable and all memory region in other
 * buses will be always noncacheable - DONE.
 * 
 * *****************  Version 20  *****************
 * User: Sor          Date: 06/06/27   Time: 8:06p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 2.2.0.0 to 3.0.0.0 - DONE.
 * 
 * *****************  Version 19  *****************
 * User: Sor          Date: 06/06/21   Time: 9:46p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 2.1.0.0 to 2.2.0.0 - DONE.
 * 
 * *****************  Version 18  *****************
 * User: Sor          Date: 06/06/02   Time: 11:08a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 2.0.0.2 to 2.1.0.0 - DONE.
 * 
 * *****************  Version 17  *****************
 * User: Sor          Date: 06/05/25   Time: 1:41p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 2.0.0.1 to 2.0.0.2 - DONE.
 * 
 * *****************  Version 16  *****************
 * User: Sor          Date: 06/05/15   Time: 1:35p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 2.0.0.0 to 2.0.0.1 - DONE.
 * 
 * *****************  Version 15  *****************
 * User: Sor          Date: 06/05/11   Time: 5:49p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Change VPL_EDMC_IOC_USR_TO_PHY name to
 * VPL_EDMC_IOC_VIRT_TO_PHYS - DONE.
 * FEATURE: Change VPL_EDMC_IOC_PHY_TO_USR name to
 * VPL_EDMC_IOC_PHYS_TO_VIRT - DONE.
 * FEATURE: Add VPL_EDMC_IOC_SET_SHARED_SIZE ioctl option - DONE.
 * FEATURE: Add VPL_EDMC_IOC_CHECK_SHARED_REGION ioctl option - DONE.
 * 
 * *****************  Version 14  *****************
 * User: Sor          Date: 06/04/26   Time: 8:56p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 1.2.0.1 to 1.2.0.2 - DONE.
 * 
 * *****************  Version 13  *****************
 * User: Sor          Date: 06/04/26   Time: 5:31p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 1.2.0.0 to 1.2.0.1 - DONE.
 * 
 * *****************  Version 12  *****************
 * User: Sor          Date: 06/04/26   Time: 5:31p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Incorrect empty memory appending procedure - FIXED.
 * REFINEMENT: Remove useless adwCurrAddr arrag - DONE.
 * BUG: Fragmental memory space search should be moved to
 * VPL_EDMC_IOC_SET_SIZE ioctl option - FIXED.
 * 
 * *****************  Version 11  *****************
 * User: Sor          Date: 06/04/26   Time: 2:50p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 1.1.0.0 to 1.2.0.0 - DONE.
 * 
 * *****************  Version 10  *****************
 * User: Sor          Date: 06/04/25   Time: 8:19p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Add VPL_EDMC_IOC_FREE option - DONE.
 * 
 * *****************  Version 9  *****************
 * User: Sor          Date: 06/04/25   Time: 7:15p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * MODIFICATION: Add VPL_EDMC_IOC_PHY_TO_USR optoin - DONE.
 * MODIFICATION: Sort all allocated memory section by physical address in
 * Mmap() - DONE.
 * 
 * *****************  Version 8  *****************
 * User: Sor          Date: 06/04/24   Time: 11:36a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 1.0.0.2 to 1.1.0.0 - DONE.
 * 
 * *****************  Version 7  *****************
 * User: Sor          Date: 06/04/24   Time: 11:35a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Add VPL_EDMC_IOC_GET_ALIGNMENT option - DONE.
 * 
 * *****************  Version 6  *****************
 * User: Sor          Date: 06/04/21   Time: 9:36a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 1.0.0.1 to 1.0.0.2 - DONE.
 * 
 * *****************  Version 5  *****************
 * User: Sor          Date: 06/04/20   Time: 12:17p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * UPDATE: Update version number from 1.0.0.0 to 1.0.0.1 - DONE.
 * 
 * *****************  Version 4  *****************
 * User: Sor          Date: 06/04/19   Time: 4:45p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 3  *****************
 * User: Sor          Date: 06/04/19   Time: 4:15p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 2  *****************
 * User: Sor          Date: 06/04/19   Time: 4:01p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 1  *****************
 * User: Sor          Date: 06/04/19   Time: 3:39p
 * Created in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 */

/* ============================================================================================== */
#ifndef __VPL_EDMC_H__
#define __VPL_EDMC_H__

/* ============================================================================================== */
#include <linux/ioctl.h>

#include "typedef.h"
#include "global_codec.h"

/* ============================================================================================== */
#define VPL_EDMC_VERSION MAKEFOURCC(6, 0, 0, 10)
#define VPL_EDMC_ID_VERSION "6.0.0.10"

/* ============================================================================================== */
#define VPL_EDMC_IOC_MAGIC 0xCF

#define VPL_EDMC_IOC_ENTER_CRITICAL_SECTION		_IO		(VPL_EDMC_IOC_MAGIC, 0)
#define VPL_EDMC_IOC_LEAVE_CRITICAL_SECTION		_IO		(VPL_EDMC_IOC_MAGIC, 1)
#define VPL_EDMC_IOC_SET_BUS					_IO		(VPL_EDMC_IOC_MAGIC, 2)
#define VPL_EDMC_IOC_SET_ALIGNMENT				_IO		(VPL_EDMC_IOC_MAGIC, 3)
#define VPL_EDMC_IOC_SET_SIZE					_IO		(VPL_EDMC_IOC_MAGIC, 4)
/* Version 4.0.0.0 modification, 2006.07.10 */
#define VPL_EDMC_IOC_SET_ADDRESS				_IO		(VPL_EDMC_IOC_MAGIC, 5)
#define VPL_EDMC_IOC_SET_CACHE_SIZE				_IO		(VPL_EDMC_IOC_MAGIC, 6)
/* Version 3.0.0.0 modification, 2006.06.27 */
#define VPL_EDMC_IOC_CACHE_INVALIDATE			_IOW	(VPL_EDMC_IOC_MAGIC, 7, DWORD)
#define VPL_EDMC_IOC_CACHE_COPYBACK				_IOW	(VPL_EDMC_IOC_MAGIC, 8, DWORD)
#define VPL_EDMC_IOC_CACHE_FLUSH				_IOW	(VPL_EDMC_IOC_MAGIC, 9, DWORD)
#define VPL_EDMC_IOC_VIRT_TO_PHYS				_IOW	(VPL_EDMC_IOC_MAGIC, 10, DWORD)
#define VPL_EDMC_IOC_PHYS_TO_VIRT				_IOW	(VPL_EDMC_IOC_MAGIC, 11, DWORD)
/* ======================================== */
#define VPL_EDMC_IOC_FREE						_IO		(VPL_EDMC_IOC_MAGIC, 12)
#define VPL_EDMC_IOC_GET_ALIGNMENT				_IOR	(VPL_EDMC_IOC_MAGIC, 13, DWORD)
#define VPL_EDMC_IOC_SET_SHARED_SIZE			_IO		(VPL_EDMC_IOC_MAGIC, 14)
#define VPL_EDMC_IOC_CHECK_SHARED_REGION		_IOR	(VPL_EDMC_IOC_MAGIC, 15, DWORD)
/* Version 2.1.0.0 modification, 2006.06.02 */
#define VPL_EDMC_IOC_SHARED_MAPPING_TABLE		_IO		(VPL_EDMC_IOC_MAGIC, 16)
#define VPL_EDMC_IOC_GET_VERSION_NUMBER			_IOR	(VPL_EDMC_IOC_MAGIC, 17, DWORD)
#define VPL_EDMC_IOC_QUERY_CHIP_SUBTYPE			_IOR	(VPL_EDMC_IOC_MAGIC, 18, DWORD)
/* Version 5.0.0.14 modification, 2010.02.03 */
#define VPL_EDMC_IOC_GET_CHIP_ID				_IOR	(VPL_EDMC_IOC_MAGIC, 19, DWORD)
/* ======================================== */
/* Version 5.0.0.15 modification, 2010.03.08 */
#define VPL_EDMC_IOC_CHECK_MAPPED_REGION		_IOR    (VPL_EDMC_IOC_MAGIC, 20, DWORD)
/* ======================================== */

#define VPL_EDMC_IOC_MAX_NUMBER	20
/* ======================================== */

typedef struct vpl_mem_blk
{
	DWORD addr;
	DWORD offset;
	DWORD size;
} MEM_BLK;
/* ============================================================================================== */
#endif //__VPL_EDMC_H__
