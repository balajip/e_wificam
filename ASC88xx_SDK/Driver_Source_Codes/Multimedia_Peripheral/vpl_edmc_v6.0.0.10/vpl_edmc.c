/*
 * $Header: /rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC/vpl_edmc.c 71    14/03/21 3:05p Morgan.chang $
 *
 * Driver for EDMC.
 *
 * Copyright 2010 ______, Inc. 
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
 * $History: vpl_edmc.c $
 * 
 * *****************  Version 71  *****************
 * User: Morgan.chang Date: 14/03/21   Time: 3:05p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * footprint / cache status changed
 *     
 * 1. set chachable for any memory allocation.
 * 2. /sys/class/vpl_edmc/footprint shows allocations of each DRAM
 * separately.
 * 
 * *****************  Version 70  *****************
 * User: Morgan.chang Date: 14/02/12   Time: 3:01p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Print more than 1 page data of mappings in /proc/vpl_edmc_maps
 * 
 * *****************  Version 69  *****************
 * User: Morgan.chang Date: 13/10/04   Time: 4:29p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Backtrack version number to 6.0.0.8
 *     
 * In order to be compatible with old memory management library.
 * 
 * *****************  Version 68  *****************
 * User: Morgan.chang Date: 13/10/03   Time: 4:21p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Allocate all DRAM memory out of control of kernel to EDMC driver.
 * 
 * Since all memory out of kernel's control should be devoted to media
 * usage, edmc driver parses the kernel command line 'mem=' and gets
 * all the left memory.
 * So the module parameter dwMediaMemSize is abandoned !
 * 
 * *****************  Version 67  *****************
 * User: Morgan.chang Date: 13/09/26   Time: 11:20a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Support Rosinni.
 * 
 * *****************  Version 66  *****************
 * User: Morgan.chang Date: 13/04/11   Time: 7:14p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 1. FIX: Fix dram number sequencing.
 *    Dram number depends on the order of memory base addresses, 
 *    instead of the values of base addresses.
 * 
 * *****************  Version 65  *****************
 * User: Jon.lin      Date: 13/03/22   Time: 15:46
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 64  *****************
 * User: Morgan.chang Date: 12/03/30   Time: 4:42p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 1. REFINEMENT: EDMC bus number is decided by base address set in dram
 * controller which should be identical with slave base address in AHB
 * controller. Should be with Mozart v2 loader with version 12033001 or
 * newer.
 * 
 * *****************  Version 63  *****************
 * User: Jon.lin      Date: 12/01/18   Time: 16:55
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [6.0.0.4]release
 * 
 * *****************  Version 62  *****************
 * User: Jon.lin      Date: 12/01/18   Time: 15:39
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [6.0.0.4] bug fixed
 * 
 * *****************  Version 61  *****************
 * User: Poplar       Date: 11/12/30   Time: 11:17a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version 6.0.0.3
 * 
 * *****************  Version 60  *****************
 * User: Jon.lin      Date: 11/12/09   Time: 17:44
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * add GPL header
 * 
 * *****************  Version 59  *****************
 * User: Acer.tsai    Date: 11/08/02   Time: 11:53a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [REFINEMENT] : rename DRAMInfo to MediaSize
 * 
 * *****************  Version 58  *****************
 * User: Acer.tsai    Date: 11/08/01   Time: 4:06p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * [commit] FAATURE: Add sysfs to export DRAM size on MOZART platform.
 * 
 * *****************  Version 58  *****************
 * User: Acer.Tsai      Date: 11/08/01   Time: 3:30p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version 6.0.0.1
 * 
 * *****************  Version 57  *****************
 * User: Jon.lin      Date: 10/12/08   Time: 9:43a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version 6.0.0.0
 * 
 * *****************  Version 56  *****************
 * User: Poplar       Date: 10/03/08   Time: 7:23p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Add ioctl command, VPL_EDMC_IOC_CHECK_MAPPED_REGION to map
 * host side memory.
 * 
 * *****************  Version 55  *****************
 * User: Tassader.chen Date: 10/02/03   Time: 10:56a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 54  *****************
 * User: Poplar       Date: 10/01/30   Time: 6:32p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 1. FEATURE: Modify the way to caluate the size of DRAM.
 * 
 * *****************  Version 53  *****************
 * User: Poplar       Date: 10/01/29   Time: 5:27p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Version: 5.0.0.12
 * 
 * *****************  Version 52  *****************
 * User: Cchuang      Date: 10/01/28   Time: 10:54
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: VPL_EDMC_IOC_FREE doesn't work - FIXED
 * 
 * *****************  Version 48  *****************
 * User: Jon.lin      Date: 09/05/20   Time: 5:37p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * bug fix
 * 
 * *****************  Version 47  *****************
 * User: Jon.lin      Date: 09/05/20   Time: 3:34p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 46  *****************
 * User: Jon.lin      Date: 09/05/20   Time: 3:20p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * bug fixed
 * 
 * *****************  Version 45  *****************
 * User: Jon.lin      Date: 09/03/19   Time: 11:56a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * Fix company name
 * 
 * *****************  Version 44  *****************
 * User: Jon.lin      Date: 09/03/19   Time: 11:50a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * for Moxart release
 * 
 * *****************  Version 43  *****************
 * User: Jon.lin      Date: 08/12/31   Time: 8:56p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * jon: to fix non-default dwMediaMemSize setting bug
 * 
 * *****************  Version 42  *****************
 * User: Ycchang      Date: 08/09/15   Time: 9:15a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 41  *****************
 * User: Ycchang      Date: 08/09/15   Time: 9:12a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 40  *****************
 * User: Sor          Date: 08/01/16   Time: 11:38a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * REFINEMENT: Change all unnecessary printk() to PDEBUG() - DONE.
 * 
 * *****************  Version 39  *****************
 * User: Jon.lin      Date: 08/01/09   Time: 4:58p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * fix cache operation in Linux 2.6.20.4
 * 
 * *****************  Version 38  *****************
 * User: Sor          Date: 07/12/25   Time: 9:19p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Close() procedure should be protected by critical section - FIXED.
 * 
 * *****************  Version 37  *****************
 * User: Alan         Date: 07/12/25   Time: 9:14p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * 
 * *****************  Version 36  *****************
 * User: Ycchang      Date: 07/09/04   Time: 2:35p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 35  *****************
 * User: Ycchang      Date: 07/08/16   Time: 2:03p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 34  *****************
 * User: Jon.lin      Date: 07/08/16   Time: 1:33p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * for 2.6.20.4 kernel porting
 *
 * *****************  Version 33  *****************
 * User: Ycchang      Date: 07/08/16   Time: 12:49p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 32  *****************
 * User: Vincent      Date: 07/01/26   Time: 8:16p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: For multi-instace which use the same shared memory in a processes
 * should cron the table and assign its private data - FIXED.
 * BUG: While free the shared memory table must check the private data -
 * FIXED.
 *
 * *****************  Version 31  *****************
 * User: Sor          Date: 07/01/02   Time: 7:41p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: There is no mechanism to delete element of shared memory table
 * unless closing the EDMC device - FIXED.
 *
 * *****************  Version 30  *****************
 * User: Sor          Date: 06/12/11   Time: 6:18p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Support dynamic memory size configuration during module
 * installation - DONE.
 *
 * *****************  Version 29  *****************
 * User: Sor          Date: 06/11/01   Time: 4:27p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Incorrect boundary check - FIXED.
 * BUG: The bank enable bit should be checked first under Vivaldi
 * configuration before reading the size information - FIXED.
 *
 * *****************  Version 28  *****************
 * User: Sor          Date: 06/10/16   Time: 6:56p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: The size of adwSize and adwBaseAddr should be one instead of three
 * under Vivaldi configurations - FIXED.
 *
 * *****************  Version 27  *****************
 * User: Sor          Date: 06/07/10   Time: 6:42p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 26  *****************
 * User: Sor          Date: 06/07/10   Time: 5:31p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Remove VPL_EDMC_IOC_SET_CACHABLE support, all memory region in
 * system bus will be always cacheable and all memory region in other
 * buses will be always noncacheable - DONE.
 *
 * *****************  Version 25  *****************
 * User: Sor          Date: 06/06/27   Time: 8:05p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Use copy_from_user() in VPL_EDMC_IOC_CACHE_INVALIDATE,
 * VPL_EDMC_IOC_CACHE_COPYBACK, and VPL_EDMC_IOC_CACHE_FLUSH IO control
 * options to improve its performance - DONE.
 *
 * *****************  Version 24  *****************
 * User: Sor          Date: 06/06/21   Time: 9:46p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Support Vivaldi platform - DONE.
 *
 * *****************  Version 23  *****************
 * User: Sor          Date: 06/06/02   Time: 11:50a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 22  *****************
 * User: Sor          Date: 06/06/02   Time: 11:01a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Add VPL_EDMC_IOC_SHARED_MAPPING_TABLE and
 * VPL_EDMC_IOC_GET_VERSION_NUMBER ioctl options - DONE.
 *
 * *****************  Version 21  *****************
 * User: Sor          Date: 06/05/25   Time: 1:40p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: dwAlignPadding should be calculated immediately after dwStartAddr
 * - FIXED.
 *
 * *****************  Version 20  *****************
 * User: Sor          Date: 06/05/15   Time: 1:34p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: ptPrevBlkInfo should not be assigned if ptBlkInfo is not NULL in
 * Mmap() - DONE.
 *
 * *****************  Version 19  *****************
 * User: Sor          Date: 06/05/11   Time: 6:03p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 18  *****************
 * User: Sor          Date: 06/05/11   Time: 5:49p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Change VPL_EDMC_IOC_USR_TO_PHY name to
 * VPL_EDMC_IOC_VIRT_TO_PHYS - DONE.
 * FEATURE: Change VPL_EDMC_IOC_PHY_TO_USR name to
 * VPL_EDMC_IOC_PHYS_TO_VIRT - DONE.
 * FEATURE: Add VPL_EDMC_IOC_SET_SHARED_SIZE ioctl option - DONE.
 * FEATURE: Add VPL_EDMC_IOC_CHECK_SHARED_REGION ioctl option - DONE.
 *
 * *****************  Version 17  *****************
 * User: Sor          Date: 06/04/26   Time: 8:55p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Virtual memory space of different processes may have overlap, we
 * have to use the process ID to distinguish the virtual address from
 * different processes - FIXED.
 *
 * *****************  Version 16  *****************
 * User: Sor          Date: 06/04/26   Time: 5:31p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Incorrect empty memory appending procedure - FIXED.
 * REFINEMENT: Remove useless adwCurrAddr arrag - DONE.
 * BUG: Fragmental memory space search should be moved to
 * VPL_EDMC_IOC_SET_SIZE ioctl option - FIXED.
 *
 * *****************  Version 15  *****************
 * User: Sor          Date: 06/04/26   Time: 5:00p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Incorrect empty memory appending procedure - FIXED.
 *
 * *****************  Version 14  *****************
 * User: Sor          Date: 06/04/26   Time: 2:49p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * REFINEMENT: Support dynamic memory allocation - DONE.
 *
 * *****************  Version 13  *****************
 * User: Sor          Date: 06/04/25   Time: 8:19p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * FEATURE: Add VPL_EDMC_IOC_FREE option - DONE.
 *
 * *****************  Version 12  *****************
 * User: Sor          Date: 06/04/25   Time: 7:15p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * MODIFICATION: Add VPL_EDMC_IOC_PHY_TO_USR optoin - DONE.
 * MODIFICATION: Sort all allocated memory section by physical address in
 * Mmap() - DONE.
 *
 * *****************  Version 11  *****************
 * User: Sor          Date: 06/04/25   Time: 5:41p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * MODIFICATION: Use dynamic allocation method for TVPLEDMCBlkInfo buffer
 * to remove the limitation of block information number - DONE.
 *
 * *****************  Version 10  *****************
 * User: Sor          Date: 06/04/25   Time: 4:32p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * MODIFICATION: Add TVPLEDMCObjInfo structure to store private data for
 * each device object - DONE.
 * BUG: Semaphore may be locked if process quit abnormally - FIXED.
 * MODIFICATION: Move table clear-up from Mmap() to Close() - DONE.
 *
 * *****************  Version 9  *****************
 * User: Sor          Date: 06/04/24   Time: 11:35a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: The page unit of virtual memory should be 4096 aligned - FIXED.
 * FEATURE: Add VPL_EDMC_IOC_GET_ALIGNMENT option - DONE.
 *
 * *****************  Version 8  *****************
 * User: Sor          Date: 06/04/21   Time: 9:36a
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * MODIFICATION: Use system memory instead of EDMC memory for virtual to
 * physical look up table - DONE.
 *
 * *****************  Version 7  *****************
 * User: Sor          Date: 06/04/20   Time: 12:17p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 * BUG: Cannot return physical address directly in VPL_EDMC_IOC_USR_TO_PHY
 * case of Ioctl() - FIXED.
 *
 * *****************  Version 6  *****************
 * User: Sor          Date: 06/04/19   Time: 4:49p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 5  *****************
 * User: Sor          Date: 06/04/19   Time: 4:45p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 4  *****************
 * User: Sor          Date: 06/04/19   Time: 4:15p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 3  *****************
 * User: Sor          Date: 06/04/19   Time: 4:01p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 2  *****************
 * User: Sor          Date: 06/04/19   Time: 3:52p
 * Updated in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 * *****************  Version 1  *****************
 * User: Sor          Date: 06/04/19   Time: 3:39p
 * Created in $/rd_2/project/SoC/Components/EDMC/Device_Driver/EDMC
 *
 */

/* ============================================================================================== */
#ifndef __KERNEL__
	#define __KERNEL__
#endif //!__KERNEL__

#ifndef MODULE
	#define MODULE
#endif //!MODULE

/* ============================================================================================== */
#include "vpl_edmc_locals.h"

/* ============================================================================================== */
/* Version 5.0.0.5 modification, 2008.09.15 */
const CHAR VPL_EDMC_ID[] = "$Version: "VPL_EDMC_ID_VERSION"  (VPL_EDMC) $";
/* ======================================== */
static TVPLEDMCDevInfo *gptDevInfo = NULL;
static SDWORD gsdwMajor = 0;
//#ifdef MEDIA_MEM_SIZE
static DWORD dwMediaMemSize = 0;
//#endif
static int	flexible_mode = 0;
#ifdef EXPORT_STAT_BY_SYSFS
static struct class *ptModuleClass;
MODULE_LICENSE("GPL");
#endif //EXPORT_STAT_BY_SYSFS

/* ============================================================================================== */
MODULE_AUTHOR ("______ Inc.");
/* Version 4.2.0.0 modification, 2007.08.16 */
module_param (gsdwMajor, int, 0644);
MODULE_PARM_DESC(gsdwMajor, "Major number for VPL_EDMC module");
//#ifdef MEDIA_MEM_SIZE
module_param(dwMediaMemSize, long, 0644);
MODULE_PARM_DESC(dwMediaMemSize, "Obsolete! Will be removed in the future.");
//#endif
module_param_named(flexible, flexible_mode, int, 0644);
MODULE_PARM_DESC(flexible, "not zero: May NOT allocate memory at the requested memory bank if it has been full or is unavailable.");

/* ============================================================================================== */
#define CAL_SIZE(data)	(1 << (((data)&0x0000000F)+(((data)&0x000000F0)>>4) + (((data)&0x00000300)>>8) + 2))

#if defined(__ASM_ARCH_PLATFORM_MOZART_H__) | \
    defined(__ASM_ARCH_PLATFORM_ANDES_H__) | defined(__ASM_ARCH_PLATFORM_BEETHOVEN_H__) | \
    defined(__ASM_ARCH_PLATFORM_ROSSINI_H__)
const DWORD adwEDMCBaseAddr[] =
{
	VPL_EMDC_BUSES
};
#endif

/* Version 4.2.0.0 modification, 2007.08.16 */
/* Version 5.0.0.0 modification, 2007.08.23 */
#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
const DWORD adwEDMCSizeTbl[8] =
{
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000,
	0x10000000,
	0x20000000,
	0x40000000,
	0x80000000
/* ======================================== */
};
#endif //__ASM_ARCH_PLATFORM_HAYDN_H__
/* ======================================== */
#ifdef __ASM_ARCH_PLATFORM_BACH_H__
const DWORD adwEDMCSizeTbl[4] =
{
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000
};
#endif //__ASM_ARCH_PLATFORM_BACH_H__
#ifdef __ASM_ARCH_PLATFORM_HANDEL_H__
const DWORD adwEDMCSizeTbl[4] =
{
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000
};
#endif //__ASM_ARCH_PLATFORM_HANDEL_H__
#ifdef __ASM_ARCH_PLATFORM_VIVALDI_H__
const DWORD adwEDMCSizeTbl[8] =
{
	0x00100000,
	0x00200000,
	0x00400000,
	0x00800000,
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000
};
#endif //__ASM_ARCH_PLATFORM_VIVALDI_H__

/* ============================================================================================== */
#ifdef _DEBUG
static void ShowLinkList(struct list_head *the_head)
{
	TVPLEDMCBlkInfo *ptBlkInfo;

	PDEBUG("Regular block table :\n");
	list_for_each_entry(ptBlkInfo, &(gptDevInfo->lRegularBlk), node) {
		PDEBUG("\tP[0x%08lX:0x%08lX], V[0x%08lX:0x%08lX], mm %p, filp %p\n",
				ptBlkInfo->dwPhysLowerAddr, ptBlkInfo->dwPhysUpperAddr,
				ptBlkInfo->dwVirtLowerAddr, ptBlkInfo->dwVirtUpperAddr,
				ptBlkInfo->mm, ptBlkInfo->pfile);
	}

	PDEBUG("Shared block table :\n");
	list_for_each_entry(ptBlkInfo, &(gptDevInfo->lSharedBlk), node) {
		PDEBUG("\tP[0x%08lX:0x%08lX], V[0x%08lX:0x%08lX], mm %p, filp %p\n",
				ptBlkInfo->dwPhysLowerAddr, ptBlkInfo->dwPhysUpperAddr,
				ptBlkInfo->dwVirtLowerAddr, ptBlkInfo->dwVirtUpperAddr,
				ptBlkInfo->mm, ptBlkInfo->pfile);
	}
	return;
}
#else
#define ShowLinkList(the_head)
#endif //_DEBUG

#ifdef EXPORT_STAT_BY_SYSFS
static ssize_t show_edmc_maps_per_list(struct list_head *the_list, char *buf, ssize_t size)
{
	TVPLEDMCBlkInfo *ptBlkInfo;
	TVPLEDMCObjInfo *ptObjInfo;
	pid_t	last_pid = 0;
	ssize_t n = size;
	ssize_t i;

	list_for_each_entry(ptBlkInfo, the_list, node) {
		ptObjInfo = ptBlkInfo->pfile->private_data;
		i = scnprintf(buf, n, "\tP 0x%08lX V 0x%08lX S(h) %10lu (0x%08lX)",
				ptBlkInfo->dwPhysLowerAddr, ptBlkInfo->dwVirtLowerAddr,
				ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr,
				ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr);
		n -= i; buf += i;
		if (ptObjInfo->tgid != last_pid) {
			i = scnprintf(buf, n, " pid %d", ptObjInfo->tgid);
			last_pid = ptObjInfo->tgid;
			n -= i; buf += i;
		}
		if (n > 1) {
			*buf++ = '\n'; n--;
		}
		if (n <= 1)	break;
	}
	return (size - n);
}

/*
 * show_edmc_footprint()
 * 
 * The footprint supposes the blocks are sorted by physical address.
 */
static ssize_t show_edmc_footprint(struct class *cls, char *buf)
{
	ssize_t n = PAGE_SIZE ;
	ssize_t i, idx;
	ssize_t total_size[VPL_EDMC_BUS_NUM] = {[0 ... VPL_EDMC_BUS_NUM-1] = 0};
	TVPLEDMCBlkInfo *ptBlkInfo;
	struct list_head	*pos, *next;

	idx = 1;
	list_for_each_safe (pos, next, &(gptDevInfo->lRegularBlk))
	{
		ptBlkInfo = list_entry(pos, TVPLEDMCBlkInfo, node);
		for (; idx < VPL_EDMC_BUS_NUM; idx++)
			if (ptBlkInfo->dwPhysLowerAddr < gptDevInfo->adwBaseAddr[idx])
				break;
		total_size[idx - 1] += (ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr);
	}

	for (idx = 0; idx < VPL_EDMC_BUS_NUM; idx++)
	{
		i = scnprintf(buf, n, "DRAM %d Allocated Size: %d bytes\n", idx, total_size[idx]);
		n -= i; buf += i;
	}

	return (PAGE_SIZE - n);
}

static ssize_t show_edmc_maps(struct class *cls, char *buf)
{
	ssize_t n = PAGE_SIZE ;
	ssize_t i;

	i = scnprintf(buf, n, "Regular:\n");
	n -= i; buf += i;
	i = show_edmc_maps_per_list(&(gptDevInfo->lRegularBlk), buf, n);
	n -= i; buf += i;
	if (n <= 1)	goto SHOW_MAP_OUT;

	i = scnprintf(buf, n, "Shared:\n");
	n -= i; buf += i;
	if (n <= 1)	goto SHOW_MAP_OUT;
	i = show_edmc_maps_per_list(&(gptDevInfo->lSharedBlk), buf, n);
	n -= i; buf += i;
	if (n <= 1)	goto SHOW_MAP_OUT;

SHOW_MAP_OUT:

	/* the buffer is not enough. i gave it a nice ending. */
	if (n == 1) {
		memcpy(buf-4, "...\n", 4);
	}
	return (PAGE_SIZE - n);
}

const static CLASS_ATTR (maps, S_IRUGO, show_edmc_maps, NULL);
const static CLASS_ATTR (footprint, S_IRUGO, show_edmc_footprint, NULL);
#endif //EXPORT_STAT_BY_SYSFS

#if 0
static void sort_block_list (struct list_head *the_list)
{
	TVPLEDMCBlkInfo *ptNew, *ptPrev;

	ptNew = list_entry(the_list->prev, TVPLEDMCBlkInfo, node);
	ptPrev = list_entry(ptNew->node.prev, TVPLEDMCBlkInfo, node);

	while ((the_list != &(ptPrev->node)) && (ptNew->dwPhysLowerAddr < ptPrev->dwPhysLowerAddr)) {
		PDEBUG("\tsort: 0x%08lX <> %08lX\n", ptNew->dwPhysLowerAddr, ptPrev->dwPhysLowerAddr);
		ptPrev = list_entry(ptPrev->node.prev, TVPLEDMCBlkInfo, node);
	}

	if (ptPrev->node.next != &(ptNew->node)) {
		list_move(&(ptNew->node), &(ptPrev->node));
	}
}
#endif

/* ============================================================================================== */

static int search_for_free_block(DWORD dwBus, DWORD dwReqSize)
{
	TVPLEDMCBlkInfo *ptBlkInfo;
	DWORD dwLowerBoundAddr, dwUpperBoundAddr, dwStartAddr, dwAlignPadding;

	dwLowerBoundAddr = gptDevInfo->adwBaseAddr[dwBus];
	dwUpperBoundAddr = gptDevInfo->adwBaseAddr[dwBus] + gptDevInfo->adwSize[dwBus];
	dwStartAddr = gptDevInfo->adwBaseAddr[dwBus];
	dwAlignPadding = ((gptDevInfo->dwAlignMask+1)-(dwStartAddr&gptDevInfo->dwAlignMask)) & gptDevInfo->dwAlignMask;

	list_for_each_entry(ptBlkInfo, &(gptDevInfo->lRegularBlk), node) {
		if ((ptBlkInfo->dwPhysLowerAddr >= dwLowerBoundAddr) &&
				(ptBlkInfo->dwPhysUpperAddr <= dwUpperBoundAddr)) {
			if ((dwStartAddr + dwAlignPadding + dwReqSize) <= ptBlkInfo->dwPhysLowerAddr) {
				break;
			} else {
				dwStartAddr = ptBlkInfo->dwPhysUpperAddr;
				dwAlignPadding = ((gptDevInfo->dwAlignMask+1)-(dwStartAddr&gptDevInfo->dwAlignMask)) & gptDevInfo->dwAlignMask;
			}
		} else if (ptBlkInfo->dwPhysLowerAddr >= dwUpperBoundAddr) {
			/* we will never enter the desired bank. */
			break;
		}
	}

	if ((dwStartAddr + dwAlignPadding + dwReqSize) > dwUpperBoundAddr) {
		return -ENOMEM;
	}

	PDEBUG("Start address = 0x%08lX, align mask = 0x%08lX, align padding = %lu\n", dwStartAddr, gptDevInfo->dwAlignMask, dwAlignPadding);
	gptDevInfo->plReqNext = &(ptBlkInfo->node);
	gptDevInfo->dwReqAddr = dwStartAddr + dwAlignPadding;

	gptDevInfo->dwReqSize = dwReqSize;

	PDEBUG("Size = %lu\n", gptDevInfo->dwReqSize);
	return 0;
}

/* ============================================================================================== */
#if 0
static int VPLEDMC_QueryVirtualAddr(void __user *uarg)
{
}
#endif

/* ============================================================================================== */
static void DeleteBlkInfoNode(struct file *pfile)
{
	TVPLEDMCBlkInfo *ptBlkInfo;
	struct list_head	*pos, *n;

	list_for_each_safe (pos, n, &(gptDevInfo->lRegularBlk)) {
		ptBlkInfo = list_entry(pos, TVPLEDMCBlkInfo, node);
		if ((ptBlkInfo->pfile==pfile) || (pfile == NULL)) {
			PDEBUG("Release     P[0x%08lX:0x%08lX], V[0x%08lX:0x%08lX], mm %p FILE %p\n",
					ptBlkInfo->dwPhysLowerAddr, ptBlkInfo->dwPhysUpperAddr,
					ptBlkInfo->dwVirtLowerAddr, ptBlkInfo->dwVirtUpperAddr,
					ptBlkInfo->mm, ptBlkInfo->pfile);
			list_del(pos);
			kfree(ptBlkInfo);
		}
	}
}

/* ============================================================================================== */
static void DeleteSharedBlkInfoNode(struct file *pfile)
{
	TVPLEDMCBlkInfo *ptBlkInfo;
	struct list_head	*pos, *n;

	list_for_each_safe (pos, n, &(gptDevInfo->lSharedBlk)) {
		ptBlkInfo = list_entry(pos, TVPLEDMCBlkInfo, node);
		if ((ptBlkInfo->pfile==pfile) || (pfile == NULL)) {
			PDEBUG("Release [S] P[0x%08lX:0x%08lX], V[0x%08lX:0x%08lX], mm %p FILE %p\n",
					ptBlkInfo->dwPhysLowerAddr, ptBlkInfo->dwPhysUpperAddr,
					ptBlkInfo->dwVirtLowerAddr, ptBlkInfo->dwVirtUpperAddr,
					ptBlkInfo->mm, ptBlkInfo->pfile);
			list_del(pos);
			kfree(ptBlkInfo);
		}
	}
}

/* ============================================================================================== */
/* This function is NOT necessary, but it can make the map more neat and easy to read. */
#ifdef HINT_VIRTUAL_ADDR
static unsigned long edmc_get_unmapped_area (struct file *filp, unsigned long addr,
		unsigned long len, unsigned long pgoff, unsigned long flags)
{
	int	i;
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;

	/*
	 * We enforce the MAP_FIXED case.
	 */
	if (flags & MAP_FIXED) {
		return addr;
	}

	if (len > TASK_SIZE)
		return -ENOMEM;

	if ((!addr) && (!list_empty(gptDevInfo->plReqNext))){
		TVPLEDMCBlkInfo *ptBlkInfo;
		ptBlkInfo = list_entry(gptDevInfo->plReqNext->prev, TVPLEDMCBlkInfo, node);
		if (ptBlkInfo->mm == current->mm) {
			addr = ptBlkInfo->dwVirtUpperAddr;
		}
	}

	if (addr) {
		addr = PAGE_ALIGN(addr);

		vma = find_vma(mm, addr);
		if (TASK_SIZE - len >= addr &&
			(!vma || addr + len <= vma->vm_start)) {
			return addr;
		}
	}

	/* because shared won't record the bus number, thus i allocate it by dwReqAddr. */
	for (i = VPL_EDMC_BUS_NUM - 1; i >= 0; i--){
		if(gptDevInfo->dwReqAddr >= gptDevInfo->adwBaseAddr[i]) {
			break;
		}
	}

	switch (i) {
		case 0:
			addr = TASK_UNMAPPED_BASE + (TASK_UNMAPPED_BASE >> 2);
			break;
		case 1:
			addr = TASK_UNMAPPED_BASE + (TASK_UNMAPPED_BASE >> 1);
			break;
		default:
			addr = (TASK_UNMAPPED_BASE << 1);
	}

	addr = PAGE_ALIGN(addr);

	for (vma = find_vma(mm, addr); ; vma = vma->vm_next) {
		/* At this point:  (!vma || addr < vma->vm_end). */
		if (TASK_SIZE - len < addr) {
			return -ENOMEM;
		}
		if (!vma || addr + len <= vma->vm_start) {
			return addr;
		}
		addr = vma->vm_end;
	}
}
#endif //HINT_VIRTUAL_ADDR

/* ============================================================================================== */

static int Mmap(struct file *pfile, struct vm_area_struct *vma)
{
	DWORD dwSize;
	DWORD dwAlignment;
	DWORD dwVirtLowerAddr;
	DWORD dwVirtUpperAddr;
	DWORD dwPhysLowerAddr;

	TVPLEDMCBlkInfo *ptBlkInfo;

	PDEBUG("Enter Mmap function...\n");

	dwSize = vma->vm_end - vma->vm_start;

	PDEBUG("Start physical address = 0x%08lX\n", gptDevInfo->dwReqAddr);

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
/* ======================================== */

	vma->vm_flags |= VM_RESERVED;

	dwPhysLowerAddr = gptDevInfo->dwReqAddr;

	/* vm_pgoff will be shown in the 3rd column of /dev/vpl_edmc in /proc/<pid>/maps.  */
	vma->vm_pgoff = (dwPhysLowerAddr & PAGE_MASK) >> PAGE_SHIFT;

/* Map physical address to virtual address */
/* Version 4.2.0.0 modification, 2007.08.16 */
	PDEBUG("Before remap_pfn_range(): vm_start: 0x%08lX, vm_end: 0x%08lX\n", vma->vm_start, vma->vm_end);
	if (remap_pfn_range(vma,
	                    vma->vm_start,
	                    vma->vm_pgoff,
	                    dwSize,
	                    vma->vm_page_prot))
	{
		return -EFAULT;
	}
	PDEBUG("After remap_pfn_range(): vm_start: 0x%08lX, vm_end: 0x%08lX\n", vma->vm_start, vma->vm_end);
/* ======================================== */

	dwAlignment = dwPhysLowerAddr & (PAGE_SIZE -1);
	dwVirtLowerAddr = vma->vm_start + dwAlignment;
	dwVirtUpperAddr = vma->vm_end;


/* Allocate a new block information buffer */
	PDEBUG("Try to allocate next block information buffer ...\n");
	if ((ptBlkInfo=(TVPLEDMCBlkInfo *)kmalloc(sizeof(TVPLEDMCBlkInfo), GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate next block information buffer fail !!\n");
		return -EFAULT;
	}

	ptBlkInfo->dwVirtLowerAddr = dwVirtLowerAddr;
	ptBlkInfo->dwVirtUpperAddr = dwVirtUpperAddr;
	ptBlkInfo->dwPhysLowerAddr = dwPhysLowerAddr;
	ptBlkInfo->dwPhysUpperAddr = dwPhysLowerAddr + gptDevInfo->dwReqSize;
	ptBlkInfo->dwOffset = dwPhysLowerAddr - dwVirtLowerAddr;
	ptBlkInfo->pfile = pfile;
	ptBlkInfo->mm = current->mm;


	gptDevInfo->bShared = FALSE;
	list_add_tail(&(ptBlkInfo->node), gptDevInfo->plReqNext);

	PDEBUG("Exit Mmap function !!\n");
	return 0;
}

/* ============================================================================================== */
static int Close(struct inode *pinode, struct file *pfile)
{
	TVPLEDMCObjInfo *ptObjInfo = (TVPLEDMCObjInfo *)pfile->private_data;

	PDEBUG("Enter Close function...\n");

	if (pfile->private_data != NULL)
	{
		if (ptObjInfo->bLock == TRUE)
		{
			up(&gptDevInfo->sem);
			ptObjInfo->bLock = FALSE;
		}

/*[bug fix]:if one of the instances hold semaphore then another instance calls close syscall */
/* or forced to call close syscall by Ctrl-C, the close syscall would hang in down semaphore */
		if (down_trylock(&gptDevInfo->sem) == 0){
			DeleteBlkInfoNode(pfile);
			DeleteSharedBlkInfoNode(pfile);
			up(&gptDevInfo->sem);
		}else{
			if((current->flags & PF_SIGNALED) == 0){
				down(&gptDevInfo->sem);
				DeleteBlkInfoNode(pfile);
				DeleteSharedBlkInfoNode(pfile);
				up(&gptDevInfo->sem);
			}else{
				DeleteBlkInfoNode(pfile);
				DeleteSharedBlkInfoNode(pfile);
			}

		}


/* ======================================== */

		kfree(pfile->private_data);


	}

/* Version 4.2.0.0 modification, 2007.08.16 */
	module_put(THIS_MODULE);
/* ======================================== */

	PDEBUG("Exit Close function !!\n");

	return 0;
}

/* ============================================================================================== */
static int Open(struct inode *pinode, struct file *pfile)
{
	TVPLEDMCObjInfo *ptObjInfo;

	PDEBUG("Enter Open function...\n");

#ifdef VPL_EDMC_QUERY_EDMCMAPPED_ONLY
	if (!gptDevInfo->dev_inode) {
		gptDevInfo->dev_inode = pinode;
	} else if (gptDevInfo->dev_inode != pinode) {
		printk("Open this device from another inode!\n");
		return -EFAULT;
	}
#endif //VPL_EDMC_QUERY_EDMCMAPPED_ONLY

	if (pfile->private_data == NULL)
	{
		if ((pfile->private_data=(TVPLEDMCObjInfo *)kmalloc(sizeof(TVPLEDMCObjInfo), GFP_KERNEL)) == NULL)
		{
			PDEBUG("Insufficient kernel memory space !!\n");
			return -EFAULT;
		}

		ptObjInfo = (TVPLEDMCObjInfo *)pfile->private_data;
		ptObjInfo->bLock = FALSE;
		ptObjInfo->tgid = current->tgid;
	}
	else
	{
		return -EBUSY;
	}

/* Version 4.2.0.0 modification, 2007.08.16 */
	if (try_module_get(THIS_MODULE) == 0)
	{
		return -EBUSY;
	}
/* ======================================== */

	PDEBUG("Exit Open function !!\n");

	return 0;
}

/* ============================================================================================== */
static int Ioctl(struct inode *pinode, struct file *pfile, unsigned int dwCmd, unsigned long dwArg)
{
	int scError, i;
	DWORD dwVirtAddr;
	DWORD dwPhysAddr;
	DWORD dwReqSize;
	DWORD dwAlignment;
	DWORD dwVersionNum;
	DWORD adwArgBuff[2];
	DWORD dwMapSize;
	DWORD dwOffset;

	TVPLEDMCObjInfo *ptObjInfo = (TVPLEDMCObjInfo *)pfile->private_data;
	TVPLEDMCBlkInfo *ptBlkInfo;
	struct list_head	*pos, *n;
	MEM_BLK *memBlk;

	PDEBUG("Enter Ioctl function...\n");

	if (pfile->private_data == NULL)
	{
		PDEBUG("Device error !!\n");
		return -ENODEV;
	}

	if ((_IOC_TYPE(dwCmd)!=VPL_EDMC_IOC_MAGIC) || (_IOC_NR(dwCmd)>VPL_EDMC_IOC_MAX_NUMBER))
	{
		PDEBUG("Incorrect ioctl command !!\n");

		return -ENOTTY;
	}

	if (_IOC_DIR(dwCmd) & _IOC_READ)
	{
		scError = !access_ok(VERIFY_WRITE, (void *)dwArg, _IOC_SIZE(dwCmd));
	}
	else if (_IOC_DIR(dwCmd) & _IOC_WRITE)
	{
		scError = !access_ok(VERIFY_READ, (void *)dwArg, _IOC_SIZE(dwCmd));
	}
	else
	{
		scError = 0;
	}

	if (scError != 0)
	{
		PDEBUG("Unsupport ioctl command %d !!\n", dwCmd);

		return -ENOTTY;
	}

	switch (dwCmd)
	{
		case VPL_EDMC_IOC_ENTER_CRITICAL_SECTION:
			PDEBUG("Enter VPL_EDMC_IOC_ENTER_CRITICAL_SECTION option\n");
			down(&gptDevInfo->sem);
			ptObjInfo->bLock = TRUE;
			PDEBUG("Leave VPL_EDMC_IOC_ENTER_CRITICAL_SECTION option\n");
		break;
		case VPL_EDMC_IOC_LEAVE_CRITICAL_SECTION:
			PDEBUG("Enter VPL_EDMC_IOC_LEAVE_CRITICAL_SECTION option\n");
			up(&gptDevInfo->sem);
			ptObjInfo->bLock = FALSE;
			PDEBUG("Leave VPL_EDMC_IOC_LEAVE_CRITICAL_SECTION option\n");
		break;
		case VPL_EDMC_IOC_SET_BUS:
			PDEBUG("Enter VPL_EDMC_IOC_SET_BUS option\n");
			if (dwArg >= VPL_EDMC_BUS_NUM) {
				return -EINVAL;
			}
			gptDevInfo->dwReqBusNum = dwArg;
			PDEBUG("Bus number = %lu\n", gptDevInfo->dwReqBusNum);
			PDEBUG("Leave VPL_EDMC_IOC_SET_BUS option\n");
		break;
		case VPL_EDMC_IOC_SET_ALIGNMENT:
			PDEBUG("Enter VPL_EDMC_IOC_SET_ALIGNMENT option\n");
			gptDevInfo->dwAlignMask = ~(0xFFFFFFFF<<dwArg);
			PDEBUG("Aligned mask = 0x%08lX\n", gptDevInfo->dwAlignMask);
			PDEBUG("Leave VPL_EDMC_IOC_SET_ALIGNMENT option\n");
		break;
		case VPL_EDMC_IOC_SET_SIZE:
			PDEBUG("Enter VPL_EDMC_IOC_SET_SIZE option\n");
			if (!flexible_mode) {
				return search_for_free_block(gptDevInfo->dwReqBusNum, dwArg);
			}

			i = gptDevInfo->dwReqBusNum;
			do {
				scError = search_for_free_block(i, dwArg);
				if (!scError)	break;
				i ++;
				i = (i < VPL_EDMC_BUS_NUM) ? i : 0;
			} while(i != gptDevInfo->dwReqBusNum);
			if (scError)	return scError;
			PDEBUG("Leave VPL_EDMC_IOC_SET_SIZE option (succeed)\n");
		break;
		case VPL_EDMC_IOC_SET_ADDRESS:
			PDEBUG("Enter VPL_EDMC_IOC_SET_ADDRESS option\n");
			gptDevInfo->dwReqAddr = dwArg;
			PDEBUG("Leave VPL_EDMC_IOC_SET_ADDRESS option\n");
		break;
		case VPL_EDMC_IOC_SET_CACHE_SIZE:
		case VPL_EDMC_IOC_SET_SHARED_SIZE:
			PDEBUG("Enter VPL_EDMC_IOC_SET_CACHE_SIZE or VPL_EDMC_IOC_SET_SHARED_SIZE option\n");
			gptDevInfo->dwReqSize = dwArg;
			PDEBUG("Leave VPL_EDMC_IOC_SET_CACHE_SIZE or VPL_EDMC_IOC_SET_SHARED_SIZE option\n");
		break;
/* Version 3.0.0.0 modification, 2006.06.27 */
		case VPL_EDMC_IOC_CACHE_INVALIDATE:
			PDEBUG("Enter VPL_EDMC_IOC_CACHE_INVALIDATE option\n");
			copy_from_user(adwArgBuff, (DWORD *)dwArg, sizeof(DWORD)*2);
/* Version 4.2.0.0 modification, 2007.08.16 */
			dmac_inv_range((vma_ptr_t)adwArgBuff[0], (vma_ptr_t)(adwArgBuff[0]+adwArgBuff[1]));
/* ======================================== */
			PDEBUG("Leave VPL_EDMC_IOC_CACHE_INVALIDATE option\n");
		break;
		case VPL_EDMC_IOC_CACHE_COPYBACK:
			PDEBUG("Enter VPL_EDMC_IOC_CACHE_COPYBACK option\n");
			copy_from_user(adwArgBuff, (DWORD *)dwArg, sizeof(DWORD)*2);
/* Version 4.2.0.0 modification, 2007.08.16 */
			dmac_clean_range((vma_ptr_t)adwArgBuff[0], (vma_ptr_t)(adwArgBuff[0]+adwArgBuff[1]));
/* ======================================== */
			PDEBUG("Leave VPL_EDMC_IOC_CACHE_COPYBACK option\n");
		break;
		case VPL_EDMC_IOC_CACHE_FLUSH:
			PDEBUG("Enter VPL_EDMC_IOC_CACHE_FLUSH option\n");
			copy_from_user(adwArgBuff, (DWORD *)dwArg, sizeof(DWORD)*2);
/* Version 4.2.0.0 modification, 2007.08.16 */
			dmac_flush_range((vma_ptr_t)adwArgBuff[0], (vma_ptr_t)(adwArgBuff[0]+adwArgBuff[1]));
/* ======================================== */
			PDEBUG("Leave VPL_EDMC_IOC_CACHE_FLUSH option\n");
		break;
/* ======================================== */
		case VPL_EDMC_IOC_VIRT_TO_PHYS:
			PDEBUG("Enter VPL_EDMC_IOC_VIRT_TO_PHYS option\n");
			memBlk = (MEM_BLK *)dwArg;
			get_user(dwVirtAddr, (unsigned long __user *)&memBlk->addr);
			list_for_each_entry(ptBlkInfo, &(gptDevInfo->lRegularBlk), node) {
				if ((dwVirtAddr >= ptBlkInfo->dwVirtLowerAddr) &&
					(dwVirtAddr <  ptBlkInfo->dwVirtUpperAddr) &&
					(ptBlkInfo->mm == current->mm))
				{
					//dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					dwPhysAddr =  ptBlkInfo->dwVirtLowerAddr + ptBlkInfo->dwOffset;
					PDEBUG("Physical Address = 0x%08lX\n", dwPhysAddr);
					dwMapSize = ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr;
					dwOffset = dwVirtAddr - ptBlkInfo->dwVirtLowerAddr;
					copy_to_user((DWORD *)&memBlk->addr, &dwPhysAddr, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->offset, &dwOffset, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->size, &dwMapSize, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_VIRT_TO_PHYS option (successful) \n");
					return 0;
				}
			}

			list_for_each_entry(ptBlkInfo, &(gptDevInfo->lSharedBlk), node) {
				if ((dwVirtAddr>=ptBlkInfo->dwVirtLowerAddr) &&
					(dwVirtAddr<ptBlkInfo->dwVirtUpperAddr) &&
					(ptBlkInfo->mm == current->mm))
				{
					//dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					dwPhysAddr =  ptBlkInfo->dwVirtLowerAddr + ptBlkInfo->dwOffset;
					PDEBUG("Physical Address = 0x%08lX\n", dwPhysAddr);
					dwMapSize = ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr;
					dwOffset = dwVirtAddr - ptBlkInfo->dwVirtLowerAddr;
					copy_to_user((DWORD *)&memBlk->addr, &dwPhysAddr, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->offset, &dwOffset, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->size, &dwMapSize, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_VIRT_TO_PHYS option (successful) \n");
					return 0;
				}
			}

			PDEBUG("Leave VPL_EDMC_IOC_VIRT_TO_PHYS option (fail) \n");
			return -EFAULT;
		break;
		case VPL_EDMC_IOC_PHYS_TO_VIRT:
			PDEBUG("Enter VPL_EDMC_IOC_PHYS_TO_VIRT option\n");

			memBlk = (MEM_BLK *)dwArg;
			copy_from_user(&dwPhysAddr, (DWORD *)&memBlk->addr, sizeof(DWORD));

			PDEBUG("Search for P[0x%08lX] (%p)\n", dwPhysAddr, current->mm);

			list_for_each_entry(ptBlkInfo, &(gptDevInfo->lRegularBlk), node) {
				if ((dwPhysAddr >= ptBlkInfo->dwPhysLowerAddr) &&
					(dwPhysAddr <  ptBlkInfo->dwPhysUpperAddr) &&
					(ptBlkInfo->mm == current->mm))
				{
					//dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					dwVirtAddr =  ptBlkInfo->dwPhysLowerAddr - ptBlkInfo->dwOffset;
					PDEBUG("Virtual Address = 0x%08lX\n", dwVirtAddr);
					dwMapSize = ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr;
					dwOffset = dwPhysAddr - ptBlkInfo->dwPhysLowerAddr;
					copy_to_user((DWORD *)&memBlk->addr, &dwVirtAddr, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->offset, &dwOffset, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->size, &dwMapSize, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_PHYS_TO_VIRT option (successful) \n");
					return 0;
				}
			}

			list_for_each_entry(ptBlkInfo, &(gptDevInfo->lSharedBlk), node) {
				if ((dwPhysAddr>=ptBlkInfo->dwPhysLowerAddr) &&
					(dwPhysAddr<ptBlkInfo->dwPhysUpperAddr) &&
					(ptBlkInfo->mm == current->mm))
				{
					//dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					dwVirtAddr =  ptBlkInfo->dwPhysLowerAddr - ptBlkInfo->dwOffset;
					PDEBUG("Virtual Address = 0x%08lX\n", dwVirtAddr);
					dwMapSize = ptBlkInfo->dwPhysUpperAddr - ptBlkInfo->dwPhysLowerAddr;
					dwOffset = dwPhysAddr - ptBlkInfo->dwPhysLowerAddr;
					copy_to_user((DWORD *)&memBlk->addr, &dwVirtAddr, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->offset, &dwOffset, sizeof(DWORD));
					copy_to_user((DWORD *)&memBlk->size, &dwMapSize, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_PHYS_TO_VIRT option (successful) \n");
					return 0;
				}
			}

			PDEBUG("Leave VPL_EDMC_IOC_PHYS_TO_VIRT option (fail) \n");
			return -EFAULT;
		break;
		case VPL_EDMC_IOC_FREE:
			PDEBUG("Enter VPL_EDMC_IOC_FREE option\n");

			dwVirtAddr = dwArg;

			list_for_each_safe (pos, n, &(gptDevInfo->lRegularBlk)) {
				ptBlkInfo = list_entry(pos, TVPLEDMCBlkInfo, node);
				if ((dwVirtAddr == ptBlkInfo->dwVirtLowerAddr) &&
					(ptBlkInfo->mm == current->mm)) {
					list_del(pos);
					kfree(ptBlkInfo);
					PDEBUG("Leave VPL_EDMC_IOC_FREE option (successful) \n");
					down_write(&current->mm->mmap_sem);
					dwAlignment = ptBlkInfo->dwPhysLowerAddr & 0x00000FFF;
					do_munmap(current->mm, (unsigned long)(ptBlkInfo->dwVirtLowerAddr-dwAlignment), (size_t)(ptBlkInfo->dwVirtUpperAddr-ptBlkInfo->dwVirtLowerAddr+dwAlignment));
					up_write(&current->mm->mmap_sem);
					return 0;

				}
			}


/* Version 4.1.0.1 modification, 2007.01.02 */

			list_for_each_safe (pos, n, &(gptDevInfo->lSharedBlk)) {
				ptBlkInfo = list_entry(pos, TVPLEDMCBlkInfo, node);
				if ((dwVirtAddr == ptBlkInfo->dwVirtLowerAddr) &&
					(ptBlkInfo->mm == current->mm)) {
					list_del(pos);
					kfree(ptBlkInfo);
					PDEBUG("Leave VPL_EDMC_IOC_FREE option (successful) \n");
					down_write(&current->mm->mmap_sem);
					dwAlignment = ptBlkInfo->dwPhysLowerAddr & 0x00000FFF;
					do_munmap(current->mm, (unsigned long)(ptBlkInfo->dwVirtLowerAddr-dwAlignment), (size_t)(ptBlkInfo->dwVirtUpperAddr-ptBlkInfo->dwVirtLowerAddr+dwAlignment));
					up_write(&current->mm->mmap_sem);
					return 0;
				}
			}

/* ======================================== */

			PDEBUG("Leave VPL_EDMC_IOC_FREE option (fail) \n");
			return -EFAULT;
		break;
		case VPL_EDMC_IOC_GET_ALIGNMENT:
			PDEBUG("Enter VPL_EDMC_IOC_GET_ALIGNMENT option\n");
			dwAlignment = gptDevInfo->dwReqAddr & 0x00000FFF;
			copy_to_user((DWORD *)dwArg, &dwAlignment, sizeof(DWORD));
			PDEBUG("Leave VPL_EDMC_IOC_GET_ALIGNMENT option\n");
		break;
		case VPL_EDMC_IOC_CHECK_MAPPED_REGION:
			PDEBUG("Enter VPL_EDMC_IOC_CHECK_MAPPED_REGION option\n");

			dwPhysAddr = gptDevInfo->dwReqAddr;
			dwReqSize = gptDevInfo->dwReqSize;
			gptDevInfo->bShared = FALSE;
			{
				BOOL	bFound = FALSE;
				bFound = FALSE;
				list_for_each_entry(ptBlkInfo, &(gptDevInfo->lSharedBlk), node) {
					if ((ptBlkInfo->mm == current->mm) &&
							(dwPhysAddr>=ptBlkInfo->dwPhysLowerAddr) &&
							((dwPhysAddr+dwReqSize)<=ptBlkInfo->dwPhysUpperAddr)) {
						bFound = TRUE;
						break;
					}

					/* Since it is sorted by ptBlkInfo->dwPhysLowerAddr, we are sure that
					 * (dwPhysAddr>=ptBlkInfo->dwPhysLowerAddr) will never be true.
					 */
					if (dwPhysAddr < ptBlkInfo->dwPhysLowerAddr) {
						break;
					}
				}

				if (bFound) {
					dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					copy_to_user((DWORD *)dwArg, &dwVirtAddr, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_CHECK_MAPPED_REGION option (successful)\n");
					return 0;
				} else {
					dwVirtAddr = (DWORD)NULL;
					copy_to_user((DWORD *)dwArg, &dwVirtAddr, sizeof(DWORD));
					gptDevInfo->plReqNext = &(ptBlkInfo->node);
					gptDevInfo->bShared = TRUE;
					PDEBUG("Leave VPL_EDMC_IOC_CHECK_MAPPED_REGION option (successful)\n");
					return 0;
				}
			}
		break;
		case VPL_EDMC_IOC_CHECK_SHARED_REGION:
			PDEBUG("Enter VPL_EDMC_IOC_CHECK_SHARED_REGION option\n");

			dwPhysAddr = gptDevInfo->dwReqAddr;
			dwReqSize = gptDevInfo->dwReqSize;
			gptDevInfo->bShared = FALSE;

			{
				BOOL	bFound = FALSE;
				list_for_each_entry(ptBlkInfo, &(gptDevInfo->lRegularBlk), node) {
					if ((dwPhysAddr>=ptBlkInfo->dwPhysLowerAddr) &&
						((dwPhysAddr+dwReqSize)<=ptBlkInfo->dwPhysUpperAddr)) {
						bFound = TRUE;
						break;
					}
				}

				if (!bFound) {
					PDEBUG("Leave VPL_EDMC_IOC_CHECK_SHARED_REGION option (fail)\n");
					return -EFAULT;
				}

				/* if they have the same memory maps */
				if (ptBlkInfo->mm == current->mm) {
					dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					copy_to_user((DWORD *)dwArg, &dwVirtAddr, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_CHECK_SHARED_REGION option (successful)\n");
					return 0;
				}

				/* they have different memory maps, i.e., they are different processes. */
				/* The requested physical address is used by the other process (and that is what I want!).
				 * But I have to make sure that if my process has mapped this address.
				 * So I will search in the lSharedBlk.
				 */
				bFound = FALSE;
				list_for_each_entry(ptBlkInfo, &(gptDevInfo->lSharedBlk), node) {
					if ((ptBlkInfo->mm == current->mm) &&
							(dwPhysAddr>=ptBlkInfo->dwPhysLowerAddr) &&
							((dwPhysAddr+dwReqSize)<=ptBlkInfo->dwPhysUpperAddr)) {
						bFound = TRUE;
						break;
					}

					/* Since it is sorted by ptBlkInfo->dwPhysLowerAddr, we are sure that
					 * (dwPhysAddr>=ptBlkInfo->dwPhysLowerAddr) will never be true.
					 */
					if (dwPhysAddr < ptBlkInfo->dwPhysLowerAddr) {
						break;
					}
				}

				if (bFound) {
					dwVirtAddr = dwPhysAddr - ptBlkInfo->dwOffset;
					copy_to_user((DWORD *)dwArg, &dwVirtAddr, sizeof(DWORD));
					PDEBUG("Leave VPL_EDMC_IOC_CHECK_SHARED_REGION option (successful)\n");
					return 0;
				} else {
					dwVirtAddr = (DWORD)NULL;
					copy_to_user((DWORD *)dwArg, &dwVirtAddr, sizeof(DWORD));
					gptDevInfo->plReqNext = &(ptBlkInfo->node);
					gptDevInfo->bShared = TRUE;
					PDEBUG("Leave VPL_EDMC_IOC_CHECK_SHARED_REGION option (successful)\n");
					return 0;
				}
			}

		break;
/* Version 2.1.0.0 modification, 2006.06.02 */
		case VPL_EDMC_IOC_SHARED_MAPPING_TABLE:
			/* obsolete */
			PDEBUG("Enter VPL_EDMC_IOC_SHARED_MAPPING_TABLE option\n");
			PDEBUG("Leave VPL_EDMC_IOC_SHARED_MAPPING_TABLE option\n");
		break;
		case VPL_EDMC_IOC_GET_VERSION_NUMBER:
			PDEBUG("Enter VPL_EDMC_IOC_GET_VERSION_NUMBER option\n");
			dwVersionNum = VPL_EDMC_VERSION;
			copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
			PDEBUG("Leave VPL_EDMC_IOC_GET_VERSION_NUMBER option\n");
		break;
#ifdef VPL_SYSC_CHIP_SUBTYPE
		case VPL_EDMC_IOC_QUERY_CHIP_SUBTYPE:
			put_user(gptDevInfo->dwChipSubType, (unsigned long __user *)dwArg);
		break;
#endif //VPL_SYSC_CHIP_SUBTYPE
/* Version 5.0.0.14 modification, 2010.02.03 */
		case VPL_EDMC_IOC_GET_CHIP_ID:
			{
				DWORD dwID[3];
				dwID[0] = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE)+VPL_SYSC_CHIP_ID0);
				dwID[1] = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE)+VPL_SYSC_CHIP_ID1);
				dwID[2] = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE)+VPL_SYSC_CHIP_ID2);
				copy_to_user((unsigned long __user *)dwArg, dwID, sizeof(dwID));
			}
		break;
/* ======================================== */
		default:
			return -ENOTTY;
	}

	PDEBUG("Exit Ioctl function !!\n");

	return 0;
}

/* ============================================================================================== */
static struct file_operations vpl_edmc_fops =
{
	.owner 				= THIS_MODULE,
	.ioctl 				= Ioctl,
	.mmap 				= Mmap,
	.open 				= Open,
	.release 			= Close,
#ifdef HINT_VIRTUAL_ADDR
	.get_unmapped_area	= edmc_get_unmapped_area
#endif
};

struct EDMC_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */


/* Version 6.0.0.1 modification, 2011.08.01 */
#if defined(__ASM_ARCH_PLATFORM_MOZART_H__) | defined(__ASM_ARCH_PLATFORM_ROSSINI_H__)
ssize_t EDMC_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct EDMC_attribute *EDMC_attr = container_of(attr, struct EDMC_attribute, attr);
	int result = 0;

	if (EDMC_attr->show)
		result = EDMC_attr->show(kobj, buf);
	return result;
}

ssize_t EDMC_DRAM0_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%ld\n", gptDevInfo->adwSize[0]);
	return retval;
}

ssize_t EDMC_DRAM1_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%ld\n", gptDevInfo->adwSize[1]);
	return retval;
}

struct kobject EDMC_kobj;

struct EDMC_attribute EDMC_DRAM0_attr = {
	.attr = {
		.name = "DRAM0Size",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = EDMC_DRAM0_attr_show,
};

struct EDMC_attribute EDMC_DRAM1_attr = {
	.attr = {
		.name = "DRAM1Size",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = EDMC_DRAM1_attr_show,
};

struct sysfs_ops EDMC_sysfs_ops = {
	.show  = EDMC_default_attr_show,
};

struct kobj_type EDMC_ktype = {
	.sysfs_ops  = &EDMC_sysfs_ops,
};
#endif //__ASM_ARCH_PLATFORM_MOZART_H__ | __ASM_ARCH_PLATFORM_ROSSINI_H__
/* ======================================== */


#ifdef EXPORT_STAT_BY_SEQ_FILE
static int seq_printf_blkinfo(struct seq_file *m, struct list_head *h)
{
	TVPLEDMCBlkInfo   *blkinfo;
	TVPLEDMCObjInfo   *objinfo;
	pid_t   last_pid = 0;

	list_for_each_entry(blkinfo, h, node)
	{
		objinfo = blkinfo->pfile->private_data;
		if (seq_printf(m, "\tP 0x%08lX V 0x%08lX S(h) %10lu (0x%08lX)",
				blkinfo->dwPhysLowerAddr, blkinfo->dwVirtLowerAddr,
				blkinfo->dwPhysUpperAddr - blkinfo->dwPhysLowerAddr,
				blkinfo->dwPhysUpperAddr - blkinfo->dwPhysLowerAddr))
			return -1;
		if (objinfo->tgid != last_pid) {
			if (seq_printf(m, " pid %d", objinfo->tgid))
				return -1;
			last_pid = objinfo->tgid;
		}
		if (seq_printf(m, "\n"))
			return -1;
	}

	return 0;
}

static int edmc_show_maps(struct seq_file *m, void *v)
{
	if (seq_printf(m, "Regular:\n"))
		return 0;
	if (seq_printf_blkinfo(m, &gptDevInfo->lRegularBlk) < 0)
		return 0;

	if (seq_printf(m, "Shared:\n"))
		return 0;
	if (seq_printf_blkinfo(m, &gptDevInfo->lSharedBlk) < 0)
		return 0;

	return 0;
};

static int edmc_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, edmc_show_maps, NULL);
};

static struct file_operations   edmc_seq_file_ops = {
	.owner   = THIS_MODULE,
	.open    = edmc_seq_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

#endif /* EXPORT_STAT_BY_SEQ_FILE */


/* ============================================================================================== */
static void __exit Cleanup_Module(void)
{
	PDEBUG("Enter Cleanup_Module function...\n");

/* Version 6.0.0.1 modification, 2011.08.01 */
#if defined(__ASM_ARCH_PLATFORM_MOZART_H__) | defined(__ASM_ARCH_PLATFORM_ROSSINI_H__)
	sysfs_remove_file(&EDMC_kobj, &(EDMC_DRAM0_attr.attr));
	sysfs_remove_file(&EDMC_kobj, &(EDMC_DRAM1_attr.attr));
	kobject_put(&EDMC_kobj);
#endif //__ASM_ARCH_PLATFORM_MOZART_H__ | __ASM_ARCH_PLATFORM_ROSSINI_H__
/* ======================================== */

	if (gptDevInfo != NULL) {
		DeleteBlkInfoNode(NULL);
		DeleteSharedBlkInfoNode(NULL);

		if (gsdwMajor != 0) {
			unregister_chrdev(gsdwMajor, "vpl_edmc");
		}

		kfree(gptDevInfo);

#ifdef EXPORT_STAT_BY_SYSFS
		if (!IS_ERR(ptModuleClass)) {
			device_destroy(ptModuleClass, MKDEV(gsdwMajor, 0));
			class_destroy(ptModuleClass);
		}
#endif //EXPORT_STAT_BY_SYSFS
#ifdef EXPORT_STAT_BY_SEQ_FILE
		remove_proc_entry(MAP_FILE_IN_PROC, NULL);
#endif
	}

#if defined (__ASM_ARCH_PLATFORM_ANDES_H__)| defined(__ASM_ARCH_PLATFORM_BEETHOVEN_H__)
#else
	system_rev = 0;
#endif
	PDEBUG("Exit Cleanup_Module function !!\n");

	return;
}


/* ============================================================================================== */
static int __init Initial_Module(void)
{
	int scResult;
	int i, j;

	DWORD *pdwVersion;
	DWORD *pdwCfg;
/* Version 4.1.0.0 modification, 2006.12.11 */
#ifdef __ASM_ARCH_PLATFORM_VIVALDI_H__
	DWORD dwTotalSize;
#endif //__ASM_ARCH_PLATFORM_VIVALDI_H__
/* ======================================== */

	PDEBUG("Enter Initial_Module function...\n");

/* Version 6.0.0.1 modification, 2011.08.01 */
#if defined(__ASM_ARCH_PLATFORM_MOZART_H__) | defined(__ASM_ARCH_PLATFORM_ROSSINI_H__)
	EDMC_kobj.ktype = &EDMC_ktype;
/* Version 6.0.0.2 modification, 2011.08.02 */
	scResult = kobject_init_and_add(&EDMC_kobj, &EDMC_ktype, NULL, "MediaSize");
/* ======================================== */
	if (scResult < 0) {
		printk("[VPL_EDMC] : Cannot register kobject [EDMC_kobj].\n");
		return -1;
	}

	scResult = sysfs_create_file(&EDMC_kobj, &EDMC_DRAM0_attr.attr);
	if (scResult < 0) {
		printk("[VPL_EDMC] : Cannot create EDMC_DRAM0_attr.attr.\n");
		return -1;
	}

	scResult = sysfs_create_file(&EDMC_kobj, &EDMC_DRAM1_attr.attr);
	if (scResult < 0) {
		printk("[VPL_EDMC] : Cannot create EDMC_DRAM1_attr.attr.\n");
		return -1;
	}
#endif //__ASM_ARCH_PLATFORM_MOZART_H__ | __ASM_ARCH_PLATFORM_ROSSINI_H__
/* ======================================== */

	if ((gptDevInfo=kmalloc(sizeof(TVPLEDMCDevInfo), GFP_KERNEL)) == NULL)
	{
		PDEBUG("Allocate device info buffer fail !!\n");
		scResult = -ENOMEM;
		goto FAIL;
	}
	memset(gptDevInfo, 0, sizeof(TVPLEDMCDevInfo));

	scResult = register_chrdev(gsdwMajor, "vpl_edmc", &vpl_edmc_fops);

	if (scResult < 0)
	{
		PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
		goto FAIL;
	}

	if (gsdwMajor == 0)
	{
		gsdwMajor = scResult;
	}

	sema_init(&gptDevInfo->sem, 1);

	request_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR, sizeof(DWORD), "VPL_EDMC");
	pdwVersion = (DWORD *)ioremap((int)VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR, sizeof(DWORD));

	printk("VPL_EDMC: initialized (driver %lu.%lu.%lu.%lu HW %u.%u.%u.%u) "__DATE__ " " __TIME__"\n",
	       VPL_EDMC_VERSION&0xFF,
	       (VPL_EDMC_VERSION>>8)&0xFF,
	       (VPL_EDMC_VERSION>>16)&0xFF,
	       (VPL_EDMC_VERSION>>24)&0xFF,
	       (readl(pdwVersion)>>24)&0xFF,
	       (readl(pdwVersion)>>16)&0xFF,
	       (readl(pdwVersion)>>8)&0xFF,
	       readl(pdwVersion)&0xFF);

	iounmap(pdwVersion);
	release_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR, sizeof(DWORD));

#if defined (__ASM_ARCH_PLATFORM_MOZART_H__) | \
    defined (__ASM_ARCH_PLATFORM_ANDES_H__) | defined(__ASM_ARCH_PLATFORM_BEETHOVEN_H__) | \
    defined (__ASM_ARCH_PLATFORM_ROSSINI_H__)

	for (i=0, j=0; i<VPL_EDMC_BUS_NUM; i++)
	{
		const int regs = ((VPL_EDMC_CFG_OFFSET - VPL_EDMC_CTRL_OFFSET)/sizeof(DWORD)) + 1;
		request_mem_region(adwEDMCBaseAddr[i]+VPL_EDMC_CTRL_OFFSET, sizeof(DWORD)*regs, "VPL_EDMC");
		pdwCfg = (DWORD *)ioremap((int)adwEDMCBaseAddr[i]+VPL_EDMC_CTRL_OFFSET, sizeof(DWORD)*regs);

		if ((*pdwCfg) & 0x1) /* ddr enabled */
		{
			DWORD v = *(pdwCfg + (regs - 1));
			gptDevInfo->adwBaseAddr[j] = v & 0xFF000000;
			gptDevInfo->adwSize[j++] = CAL_SIZE(v);
			PDEBUG("[ddrsdmc %d] base: 0x%08lx\n", i, gptDevInfo->adwBaseAddr[j-1]);
			PDEBUG("[ddrsdmc %d] size: 0x%08lx\n", i, gptDevInfo->adwSize[j-1]);
		}
		iounmap(pdwCfg);
		release_mem_region(adwEDMCBaseAddr[i]+VPL_EDMC_CTRL_OFFSET, sizeof(DWORD)*regs);
	}

	for (i = j; i != 0;) /* bubble sorting by base address */
	{
		int n, k;
		for (n = 0, k = 0; k < i-1; k++)
		{
			if (gptDevInfo->adwBaseAddr[k] > gptDevInfo->adwBaseAddr[k+1])
			{
				DWORD base = gptDevInfo->adwBaseAddr[k];
				DWORD size = gptDevInfo->adwSize[k];
				gptDevInfo->adwBaseAddr[k] = gptDevInfo->adwBaseAddr[k+1];
				gptDevInfo->adwSize[k] = gptDevInfo->adwSize[k+1];
				gptDevInfo->adwBaseAddr[k+1] = base;
				gptDevInfo->adwSize[k+1] = size;
				n = k + 1;
			}
		}
		i = n;
	}

#ifndef MEDIA_MEM_SIZE
	if (dwMediaMemSize != 0)
		printk(KERN_WARNING "WARN: dwMediaMemSize is obsolete and has no effection!\n");
	/* Allocate all DRAM memory out of control of kernel to EDMC driver */
	/* TODO: DON'T DO THAT(read/write files from kernel space)! */
	{
		struct file   *filep;
		char   cmdline[COMMAND_LINE_SIZE], *mem = cmdline;
		mm_segment_t   old_fs = get_fs();

		set_fs(KERNEL_DS);

		filep = filp_open("/proc/cmdline", O_RDONLY, 0);
		if (IS_ERR(filep))
			printk("/proc/cmdline/ open faile\n");
		else {
			int cnt;
			loff_t   pos = 0;
			cnt = vfs_read(filep, cmdline, COMMAND_LINE_SIZE, &pos);
			if (cnt < 0)
				printk("read error (%d)\n", cnt);
			else {
				PDEBUG("%s\n", cmdline);
				while ((mem = strstr(mem, "mem="))) {
					unsigned long kstart = PHYS_OFFSET;
					unsigned long ksize  = memparse(mem+4, &mem);
					if (*mem == '@')
						kstart = memparse(mem + 1, &mem);
					ksize -= kstart & ~PAGE_MASK;
					kstart = PAGE_ALIGN(kstart);
					PDEBUG("kstart 0x%08lx, ", kstart);
					PDEBUG("ksize 0x%08lx : ", ksize);
					//while (ksize) { /* TODO: fragements in one DRAM */
						for (i = 0; i < j; i++) {
							if ( kstart >= gptDevInfo->adwBaseAddr[i]
							  && kstart < gptDevInfo->adwBaseAddr[i] + gptDevInfo->adwSize[i]) {
								gptDevInfo->adwSize[i] -= ksize;
								gptDevInfo->adwBaseAddr[i] = kstart + ksize;
								PDEBUG("DRAM %d base address = 0x%08lX, size = 0x%08lX\n", i, gptDevInfo->adwBaseAddr[i], gptDevInfo->adwSize[i]);
								break;
							}
						}
					//}
				}
			}
			filp_close(filep, current->files);
		}

		set_fs(old_fs);
	}
	for (i = 0; i < j; i++)
		if (gptDevInfo->adwSize[i])
			printk("DRAM %d base address = 0x%08lX, size = 0x%08lX\n", i, gptDevInfo->adwBaseAddr[i], gptDevInfo->adwSize[i]);
#else /* MEDIA_MEM_SIZE */
	for (i = 0; i < j; i++)
	{
		DWORD size = gptDevInfo->adwSize[i];
		gptDevInfo->adwSize[i] = i == 0 ? dwMediaMemSize == 0 ? size >> 2
		                                                      : (dwMediaMemSize << 20)
		                                : size - 4 == 0 ? 0
		                                                : size;
		gptDevInfo->adwBaseAddr[i] = i == 0 ? size - gptDevInfo->adwSize[0]
		                                    : gptDevInfo->adwBaseAddr[i];
		printk("DRAM %d base address = 0x%08lX, size = 0x%08lX\n", i, gptDevInfo->adwBaseAddr[i], gptDevInfo->adwSize[i]);
	}
#endif
#endif // MOZART / ANDES / BEETHOVEN / ROSSINI

/* Version 4.2.0.0 modification, 2007.08.16 */
#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
	flexible_mode = 0;
	request_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	if (dwMediaMemSize == 0)
	{
/* Version 5.0.0.0 modification, 2007.08.23 */
		gptDevInfo->adwSize[0] = adwEDMCSizeTbl[(((*pdwCfg)&0x0000000F)-9)+((((*pdwCfg)&0x000000F0)>>4)-12)+(((*pdwCfg)&0x00000300)>>8)-1]>>2;
/* ======================================== */
	}
	else
	{
		gptDevInfo->adwSize[0] = (dwMediaMemSize<<20);
	}
/* Version 5.0.0.0 modification, 2007.08.23 */
	gptDevInfo->adwBaseAddr[0] = adwEDMCSizeTbl[(((*pdwCfg)&0x0000000F)-9)+((((*pdwCfg)&0x000000F0)>>4)-12)+(((*pdwCfg)&0x00000300)>>8)-1] - gptDevInfo->adwSize[0];
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 0 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[0], gptDevInfo->adwSize[0]);

	request_mem_region(VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

/* Version 5.0.0.0 modification, 2007.08.23 */
	gptDevInfo->adwSize[1] = adwEDMCSizeTbl[(((*pdwCfg)&0x0000000F)-9)+((((*pdwCfg)&0x000000F0)>>4)-12)+(((*pdwCfg)&0x00000300)>>8)-1];
/* ======================================== */
	gptDevInfo->adwBaseAddr[1] = (*pdwCfg) & 0xFF000000;

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 1 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[1], gptDevInfo->adwSize[1]);
#endif //__ASM_ARCH_PLATFORM_HAYDN_H__
/* ======================================== */
#ifdef __ASM_ARCH_PLATFORM_BACH_H__
	flexible_mode = 0;
	request_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

/* Version 4.1.0.0 modification, 2006.12.11 */
	if (dwMediaMemSize == 0)
	{
		gptDevInfo->adwSize[0] = (adwEDMCSizeTbl[(*pdwCfg)&0x00000003]>>2);
	}
	else
	{
		gptDevInfo->adwSize[0] = (dwMediaMemSize<<20);
	}
	gptDevInfo->adwBaseAddr[0] = adwEDMCSizeTbl[(*pdwCfg)&0x00000003] - gptDevInfo->adwSize[0];
/* ======================================== */
/* Version 5.0.0.1 modification, 2007.11.09 */
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 0 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[0], gptDevInfo->adwSize[0]);

	request_mem_region(VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	gptDevInfo->adwSize[1] = adwEDMCSizeTbl[(*pdwCfg)&0x00000003];
	gptDevInfo->adwBaseAddr[1] = (*pdwCfg) & 0xFF000000;
/* Version 5.0.0.1 modification, 2007.11.09 */
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 1 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[1], gptDevInfo->adwSize[1]);

	request_mem_region(VPL_EDMC_BUS_2_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_2_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	gptDevInfo->adwSize[2] = adwEDMCSizeTbl[(*pdwCfg)&0x00000003];
	gptDevInfo->adwBaseAddr[2] = (*pdwCfg) & 0xFF000000;
/* Version 5.0.0.1 modification, 2007.11.09 */
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_2_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 2 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[2], gptDevInfo->adwSize[2]);
#endif //__ASM_ARCH_PLATFORM_BACH_H__
#ifdef __ASM_ARCH_PLATFORM_HANDEL_H__
	flexible_mode = 0;
	request_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

/* Version 4.1.0.0 modification, 2006.12.11 */
	if (dwMediaMemSize == 0)
	{
		gptDevInfo->adwSize[0] = (adwEDMCSizeTbl[(*pdwCfg)&0x00000003]>>2);
	}
	else
	{
		gptDevInfo->adwSize[0] = (dwMediaMemSize<<20);
	}
	gptDevInfo->adwBaseAddr[0] = adwEDMCSizeTbl[(*pdwCfg)&0x00000003] - gptDevInfo->adwSize[0];
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 0 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[0], gptDevInfo->adwSize[0]);

	request_mem_region(VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	gptDevInfo->adwSize[1] = adwEDMCSizeTbl[(*pdwCfg)&0x00000003];
	gptDevInfo->adwBaseAddr[1] = (*pdwCfg) & 0xFF000000;

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_1_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 1 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[1], gptDevInfo->adwSize[1]);

	request_mem_region(VPL_EDMC_BUS_2_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_2_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	gptDevInfo->adwSize[2] = adwEDMCSizeTbl[(*pdwCfg)&0x00000003];
	gptDevInfo->adwBaseAddr[2] = (*pdwCfg) & 0xFF000000;

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_2_CTRL_MMR_BASEADDR+VPL_EDMC_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 2 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[2], gptDevInfo->adwSize[2]);
#endif //__ASM_ARCH_PLATFORM_HANDEL_H__
#ifdef __ASM_ARCH_PLATFORM_VIVALDI_H__
	flexible_mode = 0;
	request_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_BANK_0_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_BANK_0_CFG_OFFSET, sizeof(DWORD));

/* Version 4.0.0.1 modification, 2006.11.01 */
/* Version 4.1.0.0 modification, 2006.12.11 */
	dwTotalSize = 0;

	if (((*pdwCfg)&0x00080000) != 0)
	{
		dwTotalSize = dwTotalSize + adwEDMCSizeTbl[(*pdwCfg)&0x00000007];
	}
/* ======================================== */
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_BANK_0_CFG_OFFSET, sizeof(DWORD));

	request_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_BANK_1_CFG_OFFSET, sizeof(DWORD), "VPL_EDMC");
	pdwCfg = (DWORD *)ioremap((int)VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_BANK_1_CFG_OFFSET, sizeof(DWORD));

/* Version 4.1.0.0 modification, 2006.12.11 */
/* Version 4.0.0.1 modification, 2006.11.01 */
	if (((*pdwCfg)&0x00080000) != 0)
	{
		dwTotalSize = dwTotalSize + adwEDMCSizeTbl[(*pdwCfg)&0x00000007];
	}

	if (dwMediaMemSize == 0)
	{
		gptDevInfo->adwSize[0] = (dwTotalSize>>2);
	}
	else
	{
		gptDevInfo->adwSize[0] = (dwMediaMemSize<<20);
	}
/* ======================================== */
	gptDevInfo->adwBaseAddr[0] = dwTotalSize - gptDevInfo->adwSize[0];
/* ======================================== */

	iounmap(pdwCfg);
	release_mem_region(VPL_EDMC_BUS_0_CTRL_MMR_BASEADDR+VPL_EDMC_BANK_1_CFG_OFFSET, sizeof(DWORD));

	printk("Bus 0 base address = 0x%08lX, size = 0x%08lX\n", gptDevInfo->adwBaseAddr[0], gptDevInfo->adwSize[0]);
/* Version 4.0.0.1 modification, 2006.10.16 */
/* ======================================== */
#endif //__ASM_ARCH_PLATFORM_VIVALDI_H__

	//gptDevInfo->pBlkInfoNext = NULL;
	//gptDevInfo->pSharedBlkInfoNext = NULL;
	gptDevInfo->bShared = FALSE;

	INIT_LIST_HEAD(&(gptDevInfo->lRegularBlk));
	INIT_LIST_HEAD(&(gptDevInfo->lSharedBlk));

#ifdef EXPORT_STAT_BY_SYSFS
	/* Create a struct class structure */
	ptModuleClass = class_create(THIS_MODULE, "vpl_edmc");
	if (IS_ERR(ptModuleClass)) {
		scResult = -EFAULT;
		goto FAIL;
	}

	class_create_file(ptModuleClass, &class_attr_maps);
	class_create_file(ptModuleClass, &class_attr_footprint);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	device_create(ptModuleClass, NULL, MKDEV(gsdwMajor, 0), NULL, "vpl_edmc");
#else
	device_create(ptModuleClass, NULL, MKDEV(gsdwMajor, 0), "vpl_edmc");
#endif
#endif //EXPORT_STAT_BY_SYSFS

#ifdef EXPORT_STAT_BY_SEQ_FILE
	{
		struct proc_dir_entry *entry;

		entry = create_proc_entry(MAP_FILE_IN_PROC, 0, NULL);
		if (entry)
		{
			entry->proc_fops = &edmc_seq_file_ops;
		}
	}
#endif /* EXPORT_STAT_BY_SEQ_FILE */

#ifdef VPL_SYSC_MMR_BASE
	system_rev = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE));
#endif //VPL_SYSC_MMR_BASE

#ifdef VPL_SYSC_CHIP_SUBTYPE
	gptDevInfo->dwChipSubType = readl(IO_ADDRESS(VPL_SYSC_MMR_BASE) + VPL_SYSC_CHIP_SUBTYPE) & 0x7;
#endif //VPL_SYSC_CHIP_SUBTYPE

	PDEBUG("Exit Initial_Module function !!\n");

	return 0;

FAIL:
	Cleanup_Module();

	return scResult;
}

/* ============================================================================================== */
module_init(Initial_Module);
module_exit(Cleanup_Module);

/* ============================================================================================== */
