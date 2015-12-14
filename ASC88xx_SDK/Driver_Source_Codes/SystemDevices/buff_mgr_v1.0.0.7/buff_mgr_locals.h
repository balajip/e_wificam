/*
 * Copyright 2011 VN, Inc.
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

/* ============================================================================================== */
#ifndef __BUFF_MGR_LOCALS_H__
#define __BUFF_MGR_LOCALS_H__

/* ============================================================================================== */
//#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/cache.h>
#include <asm/arch/platform.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/device.h>
//#include <asm/arch/irq.h>

#include "typedef.h"
#include "buff_mgr.h"

/* ============================================================================================== */
#undef PDEBUG
#ifdef _DEBUG
	#ifdef __KERNEL__
		#define LOGLEVEL KERN_DEBUG
		#define PDEBUG(fmt, args...) printk(LOGLEVEL "BUFF_MGR: " fmt, ## args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif

/* ============================================================================================== */
extern struct file_operations buff_mgr_fops;

/* ============================================================================================== */
typedef struct buff_mgr_obj_info
{
	volatile BOOL bLock;
	SDWORD sdwID;
	BOOL bIsReader;
	BOOL bFirst; // first writer: TRUE, other writer: FALSE
	
	//-- Acer Insert 2008/05/13--
	DWORD dwMinorNum; // used in poll function
	//-- Acer Insert End       --
} TBUFFMGRObjInfo;

typedef enum buff_mgr_reader_status
{
	ebmrsNoUsed,
	ebmrsReady,
	ebmrsAcceed
} EBUFFMGRReaderStatus;

typedef struct buff_mgr_buf_info
{
	//! buffer index, the value is between (0 ~ dwBufNum-1)
    DWORD dwIndex;
    //! address of data entry
    DWORD dwDataAddr;
    //! address of header entry
    DWORD dwHeaderAddr;
    //! buffer status
    EBUFFMGRReaderStatus ebmBufStatus;
    //! number of access, the initial value is dwReaderCount, if 0: all reader take it out
    DWORD dwRestReaderNum;
    //! ebmrsNoUsed: no reader use this entry, ebmrsReady: should be taken, ebmrsAcceed: took it out
    EBUFFMGRReaderStatus *ebmReaderStatus;
} TBUFFMGRBufInfo;

typedef struct used_id_list{
	SDWORD sdwID;
	struct list_head list;
} TBUFFMGRUsedIDList;

typedef struct reader_entry_info{
	BOOL bHaveReader;
	BOOL bInList;
	SDWORD sdwReaderEntry;
	BOOL bNeedMapping;
    BOOL bSlowRole;
} TBUFFMGRReaderEntryInfo;

typedef struct buff_mgr_buffer_set_info
{
	//! protect critical section
	struct semaphore sem;
	//! initial value = FALSE, first writer will change to TRUE, use to protect the R/W initial  
    BOOL bInitialized;
    //! the physical address allocated by EDMC
    DWORD dwPhyBaseAddr;
    //! the total number of buffer writer create
    DWORD dwBufNum;
    //! each buffer size ( HdrSize + DataSize )
    DWORD dwBufSize;
	//! the address of buffer information    
    TBUFFMGRBufInfo *ptBufPtr;
    //! writer index to buffer , the value is between (0 ~ dwBufNum-1)
    DWORD dwWriterIndex;
    //! writer previous index, keep it for slow reader used.
    SDWORD sdwWriterPrevIndex;
    //! slow-reader index.
    SDWORD sdwSlowIndex;
    //! number of Reader, when first GetBuf call ReaderCount++
    DWORD dwReaderCount;
    //! index is ReaderID , value is which buf it read, initial value should be -1, -1 means first time to GetBuf, 0~(dwBuf-1) is correct entry number
    TBUFFMGRReaderEntryInfo *ReaderEntryInfo;
	//! Used Reader ID List
	TBUFFMGRUsedIDList tUsedIDList;

	//-- Acer Insert 2008/05/13--
	wait_queue_head_t wWriterWaitQueue;  // poll function use
	wait_queue_head_t wReaderWaitQueue;  // poll function use
	//-- Acer Insert End       --

    //! writer ID, needed? , guarantee this one
    //int iWriterID;
} TBUFFMGRBufferSetInfo;

typedef struct buff_mgr_dev_info
{
	TBUFFMGRBufferSetInfo *tBuffSetInfo;
} TBUFFMGRDevInfo;
/* ============================================================================================== */
#endif //__BUFF_MGR_LOCALS_H__

/* ============================================================================================== */
