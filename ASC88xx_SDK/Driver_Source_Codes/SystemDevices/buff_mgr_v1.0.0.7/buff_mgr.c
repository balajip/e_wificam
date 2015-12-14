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

#ifndef __KERNEL__
    #define __KERNEL__
#endif //!__KERNEL__

#ifndef MODULE
    #define MODULE
#endif //!MODULE

/* ============================================================================================== */
#include "buff_mgr_locals.h"

/* ============================================================================================== */
static const CHAR BUFF_MGR_ID[] = "$Version: "BUFF_MGR_ID_VERSION"  (BUFF_MGR) $";
static TBUFFMGRDevInfo *gptDevInfo = NULL;
static SDWORD gsdwMajor = 0;
static SDWORD gsdwMaxReaderNum = 4;
static SDWORD gsdwNumNodes = 4;
static struct class *ptModuleClass;

/* ============================================================================================== */
MODULE_AUTHOR ("VN Inc.");
module_param (gsdwMajor, int, 0644);
module_param(gsdwMaxReaderNum, int, 0644);
module_param(gsdwNumNodes, int, 0644);
MODULE_PARM_DESC (gsdwMajor, "Major number for BUFF_MGR module");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
void BUFF_MGR_Print_List_Entry(DWORD dwMinorNum)
{
	TBUFFMGRUsedIDList *ptTempListEntry;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

	list_for_each_entry(ptTempListEntry, &(ptBuffSetInfo->tUsedIDList.list), list);
}

/* ============================================================================================== */
SCODE BUFF_MGR_Register_To_Driver(TBUFFMGRReaderInit *ptInit, DWORD dwMinorNum)
{
    // only reader will call this option.
	SDWORD dwIndex;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	// get one non-used ID 
	// ReaderCount ++
	// return this ID to user space
	for (dwIndex=0; dwIndex<gsdwMaxReaderNum; dwIndex++)
	{
		if (ptBuffSetInfo->ReaderEntryInfo[dwIndex].bHaveReader == FALSE)
		{
			ptBuffSetInfo->ReaderEntryInfo[dwIndex].bHaveReader = TRUE;
            ptInit->sdwReaderID = dwIndex;
            if (ptInit->eRole == ebmrSlowReader) {
                ptBuffSetInfo->ReaderEntryInfo[dwIndex].bSlowRole = TRUE;
            } else {
                ptBuffSetInfo->ReaderEntryInfo[dwIndex].bSlowRole = FALSE;
            }
			ptBuffSetInfo->dwReaderCount++;
			break;
		}
	}
	BUFF_MGR_Print_List_Entry(dwMinorNum);
	// gsdwMaxReaderNum
	if (dwIndex == gsdwMaxReaderNum)
	{
		ptInit->sdwReaderID = -1;
		return S_FAIL;
	}
	return S_OK;
}

SCODE BUFF_MGR_READER_STOP(SDWORD *psdwReaderID, DWORD dwMinorNum)
{
	struct list_head *pos, *q;
	TBUFFMGRUsedIDList *ptTempListEntry;
	TBUFFMGRBufInfo *ptBufInfo;
	DWORD dwIndex;
	SDWORD sdwReaderIDEntry = *psdwReaderID;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

	// reset Ready Status or Access Status to NoUsed
    for (dwIndex=0; dwIndex<ptBuffSetInfo->dwBufNum; dwIndex++)
	{
		ptBufInfo = ptBuffSetInfo->ptBufPtr + dwIndex;
		if (ptBufInfo->ebmReaderStatus[sdwReaderIDEntry] == ebmrsReady)
		{
			ptBufInfo->ebmReaderStatus[sdwReaderIDEntry] = ebmrsAcceed;
			ptBufInfo->dwRestReaderNum--;
	    	if (ptBufInfo->dwRestReaderNum == 0) {
				ptBufInfo->ebmBufStatus = ebmrsAcceed;
    		}
		}
	}
	// delete from list
	list_for_each_safe(pos, q, &(ptBuffSetInfo->tUsedIDList.list))
	{
		ptTempListEntry = list_entry(pos, TBUFFMGRUsedIDList, list);
		if (ptTempListEntry->sdwID == sdwReaderIDEntry)
		{
			PDEBUG("freeing item ID = %d... \n", ptTempListEntry->sdwID);
			list_del(pos);
			kfree(ptTempListEntry);
			break;
		}
	}
	// set entry = -1, and not in List
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].sdwReaderEntry = -1;
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].bInList = FALSE;
	
	return S_OK;
}
/* ============================================================================================== */
SCODE BUFF_MGR_UnRegister_To_Driver(SDWORD *psdwReaderID, DWORD dwMinorNum)
{
	struct list_head *pos, *q;
	TBUFFMGRUsedIDList *ptTempListEntry;
	TBUFFMGRBufInfo *ptBufInfo;
	DWORD dwIndex;
	SDWORD sdwReaderIDEntry = *psdwReaderID;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
		
	// it needn't unregister because it don't register successfully
	if (sdwReaderIDEntry == -1)
	{
		return S_OK;
	}
	if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].bHaveReader == FALSE)
	{
		// already unregistration by Ctrl+C
		return S_OK;
	}
	// set haveReader = False
	// clean the bufferInfo: if entry is Ready: counter-- , change to NoUsed , if Acceed: change to NoUsed.
	// delete this ID from list
	// ReaderCount --
	
	// clear the bufferInfo (set the bufferInfo entry to NoUsed and count-- if needed)
    for (dwIndex=0; dwIndex<ptBuffSetInfo->dwBufNum; dwIndex++)
	{
		ptBufInfo = ptBuffSetInfo->ptBufPtr + dwIndex;
		if (ptBufInfo->ebmReaderStatus[sdwReaderIDEntry] == ebmrsReady)
		{
			ptBufInfo->ebmReaderStatus[sdwReaderIDEntry] = ebmrsNoUsed;
			ptBufInfo->dwRestReaderNum--;
		}
		else if (ptBufInfo->ebmReaderStatus[sdwReaderIDEntry] == ebmrsAcceed)
		{
			ptBufInfo->ebmReaderStatus[sdwReaderIDEntry] = ebmrsNoUsed;
		}
		
		if (ptBufInfo->dwRestReaderNum == 0) {
            ptBufInfo->ebmBufStatus = ebmrsNoUsed;
		}
	}
	
	list_for_each_safe(pos, q, &(ptBuffSetInfo->tUsedIDList.list))
	{
		ptTempListEntry = list_entry(pos, TBUFFMGRUsedIDList, list);
		if (ptTempListEntry->sdwID == sdwReaderIDEntry)
		{
			list_del(pos);
			kfree(ptTempListEntry);
			break;
		}
	}
	BUFF_MGR_Print_List_Entry(dwMinorNum);
	
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].bHaveReader = FALSE;
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].sdwReaderEntry = -1;
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].bInList = FALSE;
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderIDEntry].bNeedMapping = FALSE;
	
	ptBuffSetInfo->dwReaderCount--;
	return S_OK;
}

/* ============================================================================================== */
SCODE BUFF_MGR_Get_Mapping(TBUFFMGRMappingStatus *ptStatus, DWORD dwMinorNum)
{
	SDWORD sdwReaderID = ptStatus->sdwReaderID;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

	ptStatus->bNeedMapping = ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bNeedMapping;
	return S_OK;
}

/* ============================================================================================== */
SCODE BUFF_MGR_Set_Mapping(TBUFFMGRMappingStatus *ptStatus, DWORD dwMinorNum)
{
	SDWORD sdwReaderID = ptStatus->sdwReaderID;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

	ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bNeedMapping = ptStatus->bNeedMapping;
	return S_OK;
}

/* ============================================================================================== */
static int Close(struct inode *pinode, struct file *pfile)
{
    TBUFFMGRObjInfo *ptObjInfo = (TBUFFMGRObjInfo *)pfile->private_data;
    DWORD dwIndex;
	TBUFFMGRBufInfo *ptBufInfo;
	DWORD dwMinorNum = MINOR(pinode->i_rdev);
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
		
    if (ptObjInfo != NULL)
    {
        if (ptObjInfo->bLock == TRUE)
        {
            up(&ptBuffSetInfo->sem);
            ptObjInfo->bLock = FALSE;
        }
        // if I am a reader , it must call unregister
        if (ptObjInfo->bIsReader == TRUE)
        {
			BUFF_MGR_UnRegister_To_Driver(&(ptObjInfo->sdwID), dwMinorNum);
		}
		else // FALSE, I am a writer
		{
			if (ptObjInfo->bFirst == TRUE)
			{
		        ptBuffSetInfo->bInitialized = FALSE;
                ptBuffSetInfo->sdwSlowIndex = -1;
                ptBuffSetInfo->sdwWriterPrevIndex = -1;
				// insure that reader can get the final data
//				ptBuffSetInfo->dwWriterIndex++;
//			    if (ptBuffSetInfo->dwWriterIndex == ptBuffSetInfo->dwBufNum)
//			    {
//			      	ptBuffSetInfo->dwWriterIndex = 0; 
//			    }
			}
		}
        kfree(ptObjInfo);
    }
    module_put(THIS_MODULE); // module count--
    if (module_refcount(THIS_MODULE) == 0) // no reader & writer, we can free memory we allocate
    {
    	for (dwIndex=0; dwIndex<ptBuffSetInfo->dwBufNum; dwIndex++)
    	{
			ptBufInfo = ptBuffSetInfo->ptBufPtr + dwIndex;
    		if (ptBufInfo->ebmReaderStatus != NULL)
    		{
		    	kfree(ptBufInfo->ebmReaderStatus);		
    		}
    	}
    	if (ptBuffSetInfo->ptBufPtr != NULL)
    	{
	    	kfree(ptBuffSetInfo->ptBufPtr);
    	}
    }
    return 0;
}

/* ============================================================================================== */
static int Open(struct inode *pinode, struct file *pfile)
{
    TBUFFMGRObjInfo *ptObjInfo;
	DWORD dwMinorNum = MINOR(pinode->i_rdev);
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

    if (pfile->private_data == NULL)
    {
        if ((pfile->private_data = (TBUFFMGRObjInfo *)kmalloc(sizeof(TBUFFMGRObjInfo), GFP_KERNEL)) == NULL)
        {
            PDEBUG("Insufficient kernel memory space !!\n");
            return -EFAULT;
        }

        ptObjInfo = (TBUFFMGRObjInfo *)pfile->private_data;
        ptObjInfo->bLock = FALSE;
        ptObjInfo->bIsReader = FALSE;
        ptObjInfo->bFirst = FALSE;
        ptObjInfo->dwMinorNum = dwMinorNum;
        ptObjInfo->sdwID = -1;
    }
    else
    {
        return -EBUSY;
    }

	// first time to call Open
    if (module_refcount(THIS_MODULE) == 0)
    {
        ptBuffSetInfo->bInitialized = FALSE;
    }    
	// try_module_get: module count++ , if return 0: failure
    if (try_module_get(THIS_MODULE) == 0) 
    {
        return -EBUSY;
    }

    return 0;
}

/* ============================================================================================== */
static SCODE BUFF_MGR_Initial(TBUFFMGRInitOptions* ptInitOptions, DWORD dwMinorNum)
{
    TBUFFMGRBufInfo *ptBufInfo;
    DWORD dwPhyBaseAddr;
    DWORD dwBufOffset;
    DWORD dwIndexI, dwIndexJ;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

    dwBufOffset = (ptInitOptions->dwHdrSize+ptInitOptions->dwDataSize);

    ptBuffSetInfo->dwPhyBaseAddr = ptInitOptions->dwPhyBaseAddr;
    ptBuffSetInfo->dwBufNum = ptInitOptions->dwBufNum;
    ptBuffSetInfo->dwBufSize = dwBufOffset*ptBuffSetInfo->dwBufNum;

    dwPhyBaseAddr = ptBuffSetInfo->dwPhyBaseAddr;

    if ((ptBuffSetInfo->ptBufPtr = (TBUFFMGRBufInfo *)kmalloc(sizeof(TBUFFMGRBufInfo)*ptBuffSetInfo->dwBufNum, GFP_KERNEL)) == NULL)
    {
        PDEBUG("Insufficient kernel memory space !!\n");
        return -ENOMEM;
    }

    for (dwIndexI=0; dwIndexI<ptBuffSetInfo->dwBufNum; dwIndexI++, dwPhyBaseAddr+=dwBufOffset)
    {
        ptBufInfo = ptBuffSetInfo->ptBufPtr + dwIndexI;
        ptBufInfo->dwIndex = dwIndexI;
        ptBufInfo->dwHeaderAddr = dwPhyBaseAddr;
        ptBufInfo->ebmBufStatus = ebmrsNoUsed;
        ptBufInfo->dwDataAddr = dwPhyBaseAddr+ptInitOptions->dwHdrSize;
        ptBufInfo->dwRestReaderNum = 0; // initial value is zero
        // dynamic allocate ebmReaderStatus memory
	    if ((ptBufInfo->ebmReaderStatus = (EBUFFMGRReaderStatus *)kmalloc(sizeof(EBUFFMGRReaderStatus)*gsdwMaxReaderNum, GFP_KERNEL)) == NULL)
    	{
        	PDEBUG("Insufficient kernel memory space !!\n");
        	return -ENOMEM;
    	}
        
        for (dwIndexJ=0; dwIndexJ<gsdwMaxReaderNum; dwIndexJ++)
        {
	        ptBufInfo->ebmReaderStatus[dwIndexJ] = ebmrsNoUsed;    	
        }
//        PDEBUG("Buffer number %lu, HdrAddr: %08lx, DataAddr: %08lx !!\n", dwIndexI, ptBufInfo->dwHeaderAddr, ptBufInfo->dwDataAddr);
    }

    ptBuffSetInfo->dwWriterIndex = 0;
    ptBuffSetInfo->sdwWriterPrevIndex = -1;
    ptBuffSetInfo->sdwSlowIndex = -1;
	for (dwIndexI=0; dwIndexI<gsdwMaxReaderNum; dwIndexI++)
	{
		if (ptBuffSetInfo->ReaderEntryInfo[dwIndexI].bHaveReader == TRUE)
		{
		    ptBuffSetInfo->ReaderEntryInfo[dwIndexI].bNeedMapping = TRUE;
		}
	    ptBuffSetInfo->ReaderEntryInfo[dwIndexI].sdwReaderEntry = -1;
   	}    
//    ptBuffSetInfo->dwReaderCount = 0; // Initial_Module: 0 , but maybe writer will leave and in, so the ReaderCount should not be changed.
    return S_OK;
}

/* ============================================================================================== */
static SCODE BUFF_MGR_Get_InBuff(TBUFFMGRState *ptState, DWORD dwMinorNum)
{
	//writer get Buffer
	// if can get the entry, get it, if can't return S_FAIL	
    TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
 
    ptBufInfo = ptBuffSetInfo->ptBufPtr + ptBuffSetInfo->dwWriterIndex;

    if (ptBufInfo->ebmBufStatus != ebmrsReady) {
        ptState->dwIndex =  ptBufInfo->dwIndex;
        ptState->dwHdrAddr = ptBufInfo->dwHeaderAddr;
        ptState->dwDataAddr = ptBufInfo->dwDataAddr;
    } else {
	    return S_FAIL;
    }
    return S_OK;	
}

/* ============================================================================================== */
static void BUFF_MGR_Release_InBuff(DWORD dwIndex, DWORD dwMinorNum)
{
	// writer release buffer
	struct list_head *pos, *q;
	TBUFFMGRUsedIDList *ptTempListEntry;
	TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	ptBufInfo = ptBuffSetInfo->ptBufPtr + ptBuffSetInfo->dwWriterIndex;
	
//	ptBufInfo->ebmBufStatus = ebmrsReady;
	list_for_each_safe(pos, q, &(ptBuffSetInfo->tUsedIDList.list))
	{
		ptTempListEntry = list_entry(pos, TBUFFMGRUsedIDList, list);
		if (ptBuffSetInfo->ReaderEntryInfo[ptTempListEntry->sdwID].bHaveReader == TRUE) {
            if (ptBuffSetInfo->ReaderEntryInfo[ptTempListEntry->sdwID].bSlowRole != TRUE) {
    			ptBufInfo->ebmReaderStatus[ptTempListEntry->sdwID] = ebmrsReady;
    			ptBufInfo->dwRestReaderNum++;
		    }
			// set ReaderIndex to WriterIndex if in list but get successfully at once.
			if (ptBuffSetInfo->ReaderEntryInfo[ptTempListEntry->sdwID].sdwReaderEntry == -1) {
				ptBuffSetInfo->ReaderEntryInfo[ptTempListEntry->sdwID].sdwReaderEntry = ptBuffSetInfo->dwWriterIndex;
			}
		}
	}
    if (ptBufInfo->dwRestReaderNum > 0) {
   	    ptBufInfo->ebmBufStatus = ebmrsReady;   // we set the buffer status to Ready when there is just one reader.
    }

	ptBuffSetInfo->sdwWriterPrevIndex = ptBuffSetInfo->dwWriterIndex;
	ptBuffSetInfo->dwWriterIndex = (ptBuffSetInfo->dwWriterIndex+1 == ptBuffSetInfo->dwBufNum)? 0: ptBuffSetInfo->dwWriterIndex+1;
	wake_up_interruptible(&ptBuffSetInfo->wReaderWaitQueue);
}

/* ============================================================================================== */
static SCODE BUFF_MGR_Get_OutBuff(TBUFFMGRState *ptState, DWORD dwMinorNum)
{
	TBUFFMGRUsedIDList *ptTempListEntry;
	SDWORD sdwReaderID = ptState->sdwReaderID;
	TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	// reader get buffer
	// 1. first time( Index == -1 )? ( linked list & set Index to writer-1)
	if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry == -1) // reader first time call GetBuffer
	{
		if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bInList == FALSE)
		{
			if ((ptTempListEntry = (TBUFFMGRUsedIDList *)kmalloc(sizeof(TBUFFMGRUsedIDList), GFP_KERNEL)) == NULL)
			{
			        PDEBUG("Insufficient kernel memory space !!\n");
			        return -ENOMEM;
			}
			ptTempListEntry->sdwID = sdwReaderID;
			list_add_tail(&(ptTempListEntry->list), &(ptBuffSetInfo->tUsedIDList.list));
			ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bInList = TRUE;
			//BUFF_MGR_Print_List_Entry(dwMinorNum);
		}		
		return S_FAIL;
	} else {
	    ptBufInfo = ptBuffSetInfo->ptBufPtr + ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry;
		if (ptBufInfo->ebmReaderStatus[sdwReaderID] == ebmrsReady) {
	        ptState->dwIndex =  ptBufInfo->dwIndex;
    	    ptState->dwHdrAddr = ptBufInfo->dwHeaderAddr;
        	ptState->dwDataAddr = ptBufInfo->dwDataAddr;
        } else {
            return S_FAIL;
        }
	}
	return S_OK;
}

/* ============================================================================================== */
static void BUFF_MGR_Release_OutBuff(SDWORD sdwReaderID, DWORD dwMinorNum)
{
	// reader release buffer
	TBUFFMGRBufInfo *ptBufInfo;
	SDWORD sdwReaderEntry;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	sdwReaderEntry = ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry; // current buffer index
	ptBufInfo = ptBuffSetInfo->ptBufPtr + sdwReaderEntry; // point to current buffer
	
	ptBufInfo->ebmReaderStatus[sdwReaderID] = ebmrsAcceed;
	ptBufInfo->dwRestReaderNum--;
	if (ptBufInfo->dwRestReaderNum == 0) {
	    ptBufInfo->ebmBufStatus = ebmrsAcceed;
	}
	ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry = (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry+1 == ptBuffSetInfo->dwBufNum)? 0: ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry+1;
	wake_up_interruptible(&ptBuffSetInfo->wWriterWaitQueue);
}

/* ============================================================================================== */
static SCODE BUFF_MGR_Get_OutBuff_By_Slow(TBUFFMGRState *ptState, DWORD dwMinorNum)
{
	TBUFFMGRUsedIDList *ptTempListEntry;
	SDWORD sdwReaderID = ptState->sdwReaderID;
	TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

	// slow-reader get buffer
	// 1. first time( Index == -1 )? ( linked list & set Index to writer-1)
	if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry == -1) // reader first time call GetBuffer
	{
		if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bInList == FALSE)
		{
			if ((ptTempListEntry = (TBUFFMGRUsedIDList *)kmalloc(sizeof(TBUFFMGRUsedIDList), GFP_KERNEL)) == NULL)
			{
			        PDEBUG("Insufficient kernel memory space !!\n");
			        return -ENOMEM;
			}
			ptTempListEntry->sdwID = sdwReaderID;
			list_add_tail(&(ptTempListEntry->list), &(ptBuffSetInfo->tUsedIDList.list));
			ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bInList = TRUE;
			//BUFF_MGR_Print_List_Entry(dwMinorNum);
		}		
		return S_FAIL;
	} else {
        if (ptBuffSetInfo->sdwWriterPrevIndex == -1) {
            return S_FAIL;
        } else {
    	    ptBufInfo = ptBuffSetInfo->ptBufPtr + ptBuffSetInfo->sdwWriterPrevIndex;
    	    ptBuffSetInfo->sdwSlowIndex = ptBuffSetInfo->sdwWriterPrevIndex;
	        ptState->dwIndex =  ptBufInfo->dwIndex;
    	    ptState->dwHdrAddr = ptBufInfo->dwHeaderAddr;
        	ptState->dwDataAddr = ptBufInfo->dwDataAddr;
        }
	}
	return S_OK;
}

/* ============================================================================================== */
static void BUFF_MGR_Release_OutBuff_By_Slow(SDWORD sdwReaderID, DWORD dwMinorNum)
{
	// slow-reader release buffer
    // it can just return ...
}

/* ============================================================================================== */
SCODE Try_Get_OutBuff(SDWORD sdwReaderID, DWORD dwMinorNum)
{
	TBUFFMGRUsedIDList *ptTempListEntry;
	TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	if (sdwReaderID == -1) {
		return S_FAIL;
	}
	else {
	    if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bSlowRole == TRUE) {
	        // slow-reader get buffer
	        if (ptBuffSetInfo->sdwSlowIndex == ptBuffSetInfo->sdwWriterPrevIndex)
	            return S_FAIL;
	        else
	            return S_OK;
	    } else {
            // reader get buffer
    		// 1. first time( Index == -1 )? ( linked list & set Index to writer-1)
    		if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry == -1) { // reader first time call GetBuffer
    			if (ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bInList == FALSE) {
    				if ((ptTempListEntry = (TBUFFMGRUsedIDList *)kmalloc(sizeof(TBUFFMGRUsedIDList), GFP_KERNEL)) == NULL) {
    				        PDEBUG("Insufficient kernel memory space !!\n");
    				        return -ENOMEM;
    				}
    				ptTempListEntry->sdwID = sdwReaderID;
    				list_add_tail(&(ptTempListEntry->list), &(ptBuffSetInfo->tUsedIDList.list));
    				ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].bInList = TRUE;
    				//BUFF_MGR_Print_List_Entry(dwMinorNum);
    			}
    			return S_FAIL;
    		} else {
        	    ptBufInfo = ptBuffSetInfo->ptBufPtr + ptBuffSetInfo->ReaderEntryInfo[sdwReaderID].sdwReaderEntry;
                if (ptBufInfo->ebmReaderStatus[sdwReaderID] == ebmrsReady) {
                    return S_OK;
                } else {
                    return S_FAIL;
                }
    		}
	    }	    
	}
}

/* ============================================================================================== */
SCODE Try_Get_InBuff(SDWORD sdwReaderID, DWORD dwMinorNum)
{
	//writer get Buffer
	// if can get, get it, if can't return S_FAIL
    TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	if (sdwReaderID != -1) {
		return S_FAIL;
	} else {
        ptBufInfo = ptBuffSetInfo->ptBufPtr + ptBuffSetInfo->dwWriterIndex;
        if (ptBufInfo->ebmBufStatus != ebmrsReady) {
            return S_OK;
        } else {
            return S_FAIL;
        }
	}
    return S_OK;
}
/* ============================================================================================== */
SCODE BUFF_MGR_GET_TABLE(TBUFFMGRTableInfo *ptBuffMgrTableInfo, DWORD dwMinorNum)
{
	TBUFFMGRBufInfo *ptBufInfo;
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	DWORD dwIndex = ptBuffMgrTableInfo->dwIndex;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	if ((dwIndex < 0) || (dwIndex >= ptBuffSetInfo->dwBufNum))
	{
		PDEBUG("[SharedBuffMgr] Get Table index error ....\n");
		return S_FAIL;
	}
	
	ptBufInfo = ptBuffSetInfo->ptBufPtr + dwIndex;
	ptBuffMgrTableInfo->dwHdrAddr = ptBufInfo->dwHeaderAddr;
	ptBuffMgrTableInfo->dwDataAddr = ptBufInfo->dwDataAddr;

	return S_OK;
}

/* ============================================================================================== */
SCODE BUFF_MGR_EnterCriticalSection(DWORD dwMinorNum)
{
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
    if (down_interruptible(&ptBuffSetInfo->sem) != 0)
    {
        PDEBUG("Cannot get semaphore !!\n");
        return S_FAIL;
    }

    return S_OK;
}

/* ============================================================================================== */
SCODE BUFF_MGR_LeaveCriticalSection(DWORD dwMinorNum)
{
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

    up(&ptBuffSetInfo->sem);

    return S_OK;
}

/* ============================================================================================== */
static unsigned int Poll(struct file *pfile, poll_table *pt)
{
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	TBUFFMGRObjInfo *ptObjInfo = (TBUFFMGRObjInfo *)pfile->private_data;
	unsigned int mask = 0;
	DWORD dwMinorNum = MINOR(pfile->f_dentry->d_inode->i_rdev);
//	DWORD dwMinorNum = ptObjInfo->dwMinorNum;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];
	
	poll_wait(pfile, &ptBuffSetInfo->wWriterWaitQueue, pt);		
	poll_wait(pfile, &ptBuffSetInfo->wReaderWaitQueue, pt);	
    
//    if (BUFF_MGR_EnterCriticalSection(dwMinorNum) != S_OK)
//    {
//    	return -ERESTARTSYS;
//    }
//    ptObjInfo->bLock = TRUE;

	if (Try_Get_InBuff(ptObjInfo->sdwID, dwMinorNum) == S_OK) // writable
		mask |= POLLOUT | POLLWRNORM;	
	if (Try_Get_OutBuff(ptObjInfo->sdwID, dwMinorNum) == S_OK)  // readible
		mask |= POLLIN | POLLRDNORM;

//    BUFF_MGR_LeaveCriticalSection(dwMinorNum);
//	ptObjInfo->bLock = FALSE;	
	return mask;
}

/* ============================================================================================== */
static int Ioctl(struct inode *pinode, struct file *pfile, unsigned int dwCmd, unsigned long dwArg)
{
    TBUFFMGRObjInfo *ptObjInfo = (TBUFFMGRObjInfo *)pfile->private_data;
    TBUFFMGRInitOptions tBUFFMGRInitOptions;
    TBUFFMGRState tState;
    TBUFFMGRMappingStatus tMappingStatus;
    TBUFFMGRReaderInit    tSlowReaderInit;
    TBUFFMGRTableInfo tBuffMgrTableInfo;
//    TBUFFMGRRegistrationOptions tBUFFMGRRegistrationOptions;
    DWORD dwVersionNum;
    SDWORD sdwReaderID;
    BOOL bFirst;
//    TBUFFMGRTableInfo tTableInfo;
    int scError;
    int scResult;
    DWORD dwMinorNum = MINOR(pinode->i_rdev);
	TBUFFMGRBufferSetInfo *ptBuffSetInfo;
	ptBuffSetInfo = &gptDevInfo->tBuffSetInfo[dwMinorNum];

    if (pfile->private_data == NULL)
    {
        PDEBUG("Device error !!\n");
        return -ENODEV;
    }

    if ((_IOC_TYPE(dwCmd)!=BUFF_MGR_IOC_MAGIC) || (_IOC_NR(dwCmd)>BUFF_MGR_IOC_MAX_NUMBER))
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
        return -EFAULT;
    }
    
    scResult = S_OK;
    switch (dwCmd)    
    {
        case BUFF_MGR_IOC_ENTER_CRITICAL_SECTION:
            if (BUFF_MGR_EnterCriticalSection(dwMinorNum) != S_OK)
            {
                return -ERESTARTSYS;
            }
            ptObjInfo->bLock = TRUE;
            break;
        case BUFF_MGR_IOC_LEAVE_CRITICAL_SECTION:
            BUFF_MGR_LeaveCriticalSection(dwMinorNum);
            ptObjInfo->bLock = FALSE;
            break;
        case BUFF_MGR_IOC_GET_VERSION_NUMBER:
            dwVersionNum = BUFF_MGR_VERSION;
            copy_to_user((DWORD *)dwArg, &dwVersionNum, sizeof(DWORD));
            break;
        case BUFF_MGR_IOC_CHECK_INIT:
            copy_to_user((BOOL *)dwArg, &ptBuffSetInfo->bInitialized, sizeof(BOOL));
            break;
        case BUFF_MGR_IOC_INITIAL:
            if (ptBuffSetInfo->bInitialized == FALSE)
            {
                copy_from_user(&tBUFFMGRInitOptions, (TBUFFMGRInitOptions *)dwArg, sizeof(tBUFFMGRInitOptions));
                scResult = BUFF_MGR_Initial(&tBUFFMGRInitOptions, dwMinorNum);
                ptBuffSetInfo->bInitialized = TRUE;
            }
            break;
        case BUFF_MGR_IOC_GET_ADDR:
            copy_to_user((DWORD *)dwArg, &ptBuffSetInfo->dwPhyBaseAddr, sizeof(DWORD));
            break;
        case BUFF_MGR_IOC_GET_SIZE:
            copy_to_user((DWORD *)dwArg, &ptBuffSetInfo->dwBufSize, sizeof(DWORD));
            break;
        case BUFF_MGR_IOC_GET_BUFNUM:
        	copy_to_user((DWORD *)dwArg, &ptBuffSetInfo->dwBufNum, sizeof(DWORD));
        	break;
        case BUFF_MGR_IOC_GET_INBUFF:
            scResult = BUFF_MGR_Get_InBuff(&tState, dwMinorNum);
            copy_to_user((TBUFFMGRState *)dwArg, &tState, sizeof(TBUFFMGRState));
            break;
        case BUFF_MGR_IOC_RELEASE_INBUFF:
            BUFF_MGR_Release_InBuff(dwArg, dwMinorNum);
            break;
        case BUFF_MGR_IOC_GET_OUTBUFF:
        	copy_from_user(&tState, (TBUFFMGRState *)dwArg, sizeof(tState)); 
           	scResult = BUFF_MGR_Get_OutBuff(&tState, dwMinorNum);
            copy_to_user((TBUFFMGRState *)dwArg, &tState, sizeof(TBUFFMGRState));
            break;
        case BUFF_MGR_IOC_RELEASE_OUTBUFF:
            BUFF_MGR_Release_OutBuff(dwArg, dwMinorNum);
            break;
        case BUFF_MGR_IOC_GET_OUTBUFF_BY_SLOW:
        	copy_from_user(&tState, (TBUFFMGRState *)dwArg, sizeof(tState)); 
           	scResult = BUFF_MGR_Get_OutBuff_By_Slow(&tState, dwMinorNum);
            copy_to_user((TBUFFMGRState *)dwArg, &tState, sizeof(TBUFFMGRState));
            break;
        case BUFF_MGR_IOC_RELEASE_OUTBUFF_BY_SLOW:
            BUFF_MGR_Release_OutBuff_By_Slow(dwArg, dwMinorNum);
            break;
        case BUFF_MGR_IOC_REGISTER_TO_DRIVER:
        	copy_from_user(&tSlowReaderInit, (TBUFFMGRReaderInit *)dwArg, sizeof(TBUFFMGRReaderInit));
        	scResult = BUFF_MGR_Register_To_Driver(&tSlowReaderInit, dwMinorNum);
        	ptObjInfo->sdwID = tSlowReaderInit.sdwReaderID;
        	ptObjInfo->bIsReader = TRUE;
        	copy_to_user((TBUFFMGRReaderInit *)dwArg, &tSlowReaderInit, sizeof(TBUFFMGRReaderInit));
        	break;
        case BUFF_MGR_IOC_UNREGISTER_TO_DRIVER:
        	copy_from_user(&sdwReaderID, (DWORD *)dwArg, sizeof(SDWORD));
        	scResult = BUFF_MGR_UnRegister_To_Driver(&sdwReaderID, dwMinorNum);
        	break;
        case BUFF_MGR_IOC_GET_NEEDMAPPING:
        	copy_from_user(&tMappingStatus, (TBUFFMGRMappingStatus *)dwArg, sizeof(tMappingStatus));
			scResult = BUFF_MGR_Get_Mapping(&tMappingStatus, dwMinorNum);
			copy_to_user((TBUFFMGRMappingStatus *)dwArg, &tMappingStatus, sizeof(TBUFFMGRMappingStatus));
        	break;
        case BUFF_MGR_IOC_SET_NEEDMAPPING:
        	copy_from_user(&tMappingStatus, (TBUFFMGRMappingStatus *)dwArg, sizeof(tMappingStatus));
			BUFF_MGR_Set_Mapping(&tMappingStatus, dwMinorNum);
        	break;
        case BUFF_MGR_IOC_WRITER_SET_FIRST:
        	copy_from_user(&bFirst, (BOOL *)dwArg, sizeof(BOOL));
        	ptObjInfo->bFirst = bFirst;
        	break;
        case BUFF_MGR_IOC_GET_TABLE_INFO:
        	copy_from_user(&tBuffMgrTableInfo, (TBUFFMGRTableInfo *)dwArg, sizeof(tBuffMgrTableInfo));
        	scResult = BUFF_MGR_GET_TABLE(&tBuffMgrTableInfo, dwMinorNum);
        	copy_to_user((TBUFFMGRTableInfo *)dwArg, &tBuffMgrTableInfo, sizeof(TBUFFMGRTableInfo));
        	break;
        case BUFF_MGR_IOC_READER_STOP:
        	copy_from_user(&sdwReaderID, (DWORD *)dwArg, sizeof(SDWORD));
        	scResult = BUFF_MGR_READER_STOP(&sdwReaderID, dwMinorNum);
        	break;
        default:
            return -ENOTTY;
    }
    return scResult;
}

/* ============================================================================================== */
struct file_operations buff_mgr_fops =
{
    ioctl:      Ioctl,
    open:       Open,
    release:    Close,
    poll:		Poll,
};

/* ============================================================================================== */
static void Cleanup_Module(void)
{
//    int scResult;
    int j;

    if (gptDevInfo != NULL)
    {
        if (gsdwMajor != 0)
        {
//            scResult = unregister_chrdev(gsdwMajor, "buff_mgr");
            unregister_chrdev(gsdwMajor, "buff_mgr");

//            if (scResult < 0)
//            {
//                PDEBUG("Cannot release major number %d !!\n", (int)gsdwMajor);
//                return;
//            }
        }    
		for (j=0; j<gsdwNumNodes; j++)
		{
			if (gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo != NULL)
			{
		    	kfree(gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo);
			}
		}
		if (gptDevInfo->tBuffSetInfo != NULL)
		{
			kfree(gptDevInfo->tBuffSetInfo);		
		}
        kfree(gptDevInfo);

		for (j = 0; j < gsdwNumNodes; j ++) {
			device_destroy(ptModuleClass, MKDEV(gsdwMajor, j));
		}
		class_destroy(ptModuleClass);
    }
    return;
}

/* ============================================================================================== */
static int Initial_Module(void)
{
    int scResult;
    int i, j;

    if ((gptDevInfo=kmalloc(sizeof(TBUFFMGRDevInfo), GFP_KERNEL)) == NULL)
    {
        PDEBUG("Allocate device info buffer fail !!\n");
        scResult = -ENOMEM;
        goto FAIL;
    }
//    memset(gptDevInfo, 0, sizeof(TBUFFMGRDevInfo));
	
    scResult = register_chrdev(gsdwMajor, "buff_mgr", &buff_mgr_fops);

    if (scResult < 0)
    {
        PDEBUG("Cannot get major number %d !!\n", (int)gsdwMajor);
        goto FAIL;
    }

    if (gsdwMajor == 0)
    {
        gsdwMajor = scResult;
    }
    
	if ((gptDevInfo->tBuffSetInfo = (TBUFFMGRBufferSetInfo *)kmalloc(sizeof(TBUFFMGRBufferSetInfo)*gsdwNumNodes, GFP_KERNEL)) == NULL)
	{
        PDEBUG("Insufficient kernel memory space !!\n");
        return -ENOMEM;
	}
	memset(gptDevInfo->tBuffSetInfo, 0, sizeof(TBUFFMGRBufferSetInfo)*gsdwNumNodes);

	// init ReaderEntry value to -1
	for (j=0; j<gsdwNumNodes; j++)
	{
		if ((gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo = (TBUFFMGRReaderEntryInfo *)kmalloc(sizeof(TBUFFMGRReaderEntryInfo)*gsdwMaxReaderNum, GFP_KERNEL)) == NULL)
		{
		        PDEBUG("Insufficient kernel memory space !!\n");
		        return -ENOMEM;
		}		
		for (i=0; i<gsdwMaxReaderNum; i++)
		{
			gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo[i].bHaveReader = FALSE;
			gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo[i].bInList = FALSE;
			gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo[i].sdwReaderEntry = -1;
			gptDevInfo->tBuffSetInfo[j].ReaderEntryInfo[i].bNeedMapping = FALSE;
		}	
		sema_init(&gptDevInfo->tBuffSetInfo[j].sem, 1);
		INIT_LIST_HEAD(&(gptDevInfo->tBuffSetInfo[j].tUsedIDList.list)); 
	    init_waitqueue_head(&gptDevInfo->tBuffSetInfo[j].wWriterWaitQueue);
	    init_waitqueue_head(&gptDevInfo->tBuffSetInfo[j].wReaderWaitQueue);
	}

	/* Create a struct class structure */
	ptModuleClass = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(ptModuleClass)) {
		return -EFAULT;
	}	

	for (i = 0; i < gsdwNumNodes; i ++) {
		device_create(ptModuleClass, NULL, MKDEV(gsdwMajor, i), NULL, DEVICE_NAME "%d", i);
	}

	// init ID list, it should be empty
    printk("Install BUFF_MGR device driver version %d.%d.%d.%d complete !!\n",
           (int)(BUFF_MGR_VERSION&0xFF),
           (int)((BUFF_MGR_VERSION>>8)&0xFF),
           (int)((BUFF_MGR_VERSION>>16)&0xFF),
           (int)((BUFF_MGR_VERSION>>24)&0xFF));

    return 0;

FAIL:
    Cleanup_Module();

    return scResult;
}

/* ============================================================================================== */
module_init(Initial_Module);
module_exit(Cleanup_Module);

/* ============================================================================================== */
