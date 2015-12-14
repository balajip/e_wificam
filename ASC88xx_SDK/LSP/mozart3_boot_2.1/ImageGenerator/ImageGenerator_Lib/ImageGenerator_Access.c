#include "ImageGenerator_Locals.h"

#define MAGICNUMBER_1 0x82451282
#define MAGICNUMBER_2 0x28791166

SCODE ImageGenerator_ListInsert( TConfigDataList * ptList, TConfigData *ptNewData, UINT uiTarget)
{
	TConfigData *ptCurr, *ptPrev ;
	UINT uiIndex = 0 ;
//	UINT uiIsGeneral = 0 ; //0 : no, it's not general config, so AddressPart is default 0xffffffff
	                       //1 : yes, so AddressPart is default 0x0

//	if ( ptList->ptListHead == NULL )
//	{
//		ptList->ptListHead = ptNewData ;
//		ptPrev->ptNextData = ptNewData ;
//		ptList->iDataSize++ ;
//		return S_OK ;
//	}

	//Start to find the position "uiTarget"
	ptCurr = ptList->ptListHead ;
	ptPrev = ptCurr ;
	//If there is no data in the original list, and we will insert the first data
	if ((ptList->ptListHead == NULL) && (ptList->ptListEnd == NULL))
	{
        ptList->ptListHead = ptNewData ;
        ptList->ptListEnd = ptNewData ;
        ptList->iListSize++ ;
        ptNewData->ptNextData = NULL ;
        return S_OK ;
    }
    //If we want to append the data to the end of list.
	else if (uiTarget == ptList->iListSize )
	{
		if ( ptList->ptListEnd->dwAddress == UNKNOWN_VALUE )
		{
			ptNewData->dwAddress = UNKNOWN_VALUE ;
		}
		ptNewData->ptNextData = NULL ;
		ptList->ptListEnd->ptNextData = ptNewData ;
		ptList->ptListEnd = ptNewData ;
		ptList->iListSize++ ;
		return S_OK ;
	}
	//If we want to insert the data to the position 0 -- the head of list.
	else if (uiTarget == 0)
	{
		if ( ptList->ptListHead->dwAddress == UNKNOWN_VALUE  )
		{
			ptNewData->dwAddress = UNKNOWN_VALUE ;
		}
		ptNewData->ptNextData = ptCurr ;
		ptList->ptListHead = ptNewData ;
		ptList->iListSize++ ;
		return S_OK ;
	}
	//other case
	else
	{
        //Find the uiTarget and insert the data
    	while (ptCurr != NULL)
	    {
		if (uiIndex == uiTarget)
    		{
			if ( ptList->ptListHead->dwAddress == UNKNOWN_VALUE )
			{
				ptNewData->dwAddress = UNKNOWN_VALUE ;
			}
	    		ptNewData->ptNextData = ptCurr ;
		    	ptPrev->ptNextData = ptNewData ;
			    ptList->iListSize++ ;
    			return S_OK ;
	    	}
		ptPrev = ptCurr ;
    		ptCurr = (TConfigData*)ptCurr->ptNextData ;
	    	uiIndex++ ;
	    }
     }

	return S_FAIL ;
}

SCODE ImageGenerator_ListSearch( TConfigDataList * ptList, UINT uiTarget, TConfigData **ptAnswer)
{
	TConfigData* ptCurr ;
	UINT uiIndex = 0 ;

	//find the TConfigData at the target positon
	ptCurr = ptList->ptListHead ;
	while (ptCurr != NULL )
	{
		if (uiIndex == uiTarget)
		{
			*ptAnswer = ptCurr ;
			return S_OK ;
		}
		ptCurr = (TConfigData*)(ptCurr->ptNextData) ;
		uiIndex++ ;
	}

	return S_OK ;
}

SCODE ImageGenerator_ListRemove( TConfigDataList * ptList, UINT uiTarget)
{
	TConfigData *ptCurr, *ptPrev ;
	UINT uiIndex = 0 ;

	//if the list is NULL, do nothing and return
	if ( ptList == NULL )
	{
		return S_FAIL ;
	}

	ptCurr = ptList->ptListHead ;
	//If only one data in the list
	if (ptList->iListSize == 1)
	{
        ptList->ptListHead = NULL ;
        ptList->ptListEnd = NULL ;
        ptList->iListSize-- ;
        free(ptCurr) ;
		return S_OK ;
    }
    //if the target is the head of list, we should update the ptList->ptListHead
	else if (uiTarget == 0)
	{
		ptList->ptListHead = (TConfigData*)(ptCurr->ptNextData) ;
		ptList->iListSize-- ;
		free(ptCurr) ;
		return S_OK ;
	}
	else
	{

        //find the target, remove the target and update ptList->ptListEnd
    	while (ptCurr != NULL)
	    {
		    if (uiIndex == uiTarget)
    		{
                //remove the final data of the list
	    		if ( uiTarget == ptList->iListSize - 1 )
		    	{
			    	ptList->ptListEnd = ptPrev ;
				    ptPrev->ptNextData = NULL ;
    				free(ptCurr) ;
	    			ptList->iListSize-- ;
		    		return S_OK ;
			    }
			    else
    			{
	    			ptPrev->ptNextData = ptCurr->ptNextData ;
		    		ptCurr->ptNextData = NULL ;
			    	free(ptCurr) ;
				    ptList->iListSize-- ;
    				return S_OK ;
	    		}
		    }
    		ptPrev = ptCurr ;
	    	ptCurr = (TConfigData*)ptCurr->ptNextData ;
    		uiIndex++ ;
	    }//end while
     }//end else
}

SCODE ImageGenerator_ListPrint(HANDLE hObject)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	TImageHeader *ptHeadCurr = ptInfo->ptHeadListStart ;
	TConfigDataList * ptDataList ;
	TConfigData* ptDataCurr ;

	while( ptHeadCurr != NULL )
	{
		//print the information of Head
		printf( "Head Name = %s\n", ptHeadCurr->acName ) ;
		printf( "Head Value= %x\n", ptHeadCurr->dwValue ) ;

		//print the information of Configured Data
		ptDataList = ptHeadCurr->ptList ;
		if ( ptDataList != NULL )
		{
			printf( "List size =%d :\n", ptDataList->iListSize ) ;
			ptDataCurr = ptDataList->ptListHead ;
			while (ptDataCurr != NULL )
			{
				printf( "[%s] 0x%x 0x%x\n", ptDataCurr->acName, ptDataCurr->dwAddress, ptDataCurr->dwContent ) ;
				ptDataCurr = (TConfigData*)ptDataCurr->ptNextData ;
			}
			printf( "\n" ) ;
		}

		ptHeadCurr = ptHeadCurr->ptNextHeader ;
	}
}

SCODE ImageGenerator_HeadInsert(HANDLE hObject, TImageHeader *ptNewHead, UINT uiTarget)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	TImageHeader *ptHeadList = ptInfo->ptHeadListStart ;
	TImageHeader *ptHeadCurr, *ptHeadPrev ;
	UINT uiIndex = 0 ;

	//if list of head is NULL, init it
	if ( ptInfo->uiHeadNum == 0 )
	{
         ptNewHead->ptList = (TConfigDataList*)malloc(sizeof(TConfigDataList));
         ptNewHead->ptList->iListSize = 0 ;
         ptNewHead->ptList->ptListEnd = NULL ;
         ptNewHead->ptList->ptListHead = NULL ;
    }

	ptHeadCurr = ptInfo->ptHeadListStart ;
	ptHeadPrev = ptHeadCurr ;

    if ((ptInfo->ptHeadListStart == NULL) && (ptInfo->ptHeadListEnd == NULL))
	{

        ptInfo->ptHeadListStart = ptNewHead ;
        ptInfo->ptHeadListEnd = ptNewHead ;
        ptInfo->uiHeadNum++ ;
        ptNewHead->ptNextHeader = NULL ;
        return S_OK ;
    }
	//insert the data at the head of list, and we should update ptInfo->ptHeadListStart
	if (uiTarget == 0)
	{
		ptNewHead->ptNextHeader = ptHeadCurr ;
		ptInfo->ptHeadListStart = ptNewHead ;
		ptInfo->uiHeadNum++ ;
		return S_OK ;
	}

	//insert the data at the end of list, and we should update ptInfo->ptHeadListEnd
	if (uiTarget == ptInfo->uiHeadNum )
	{
		ptNewHead->ptNextHeader = NULL ;
		ptInfo->ptHeadListEnd->ptNextHeader = ptNewHead ;
		ptInfo->ptHeadListEnd = ptNewHead ;
		ptInfo->uiHeadNum++ ;
		return S_OK ;
	}

	//insert the data at the target position
	while (ptHeadCurr != NULL)
	{
		if (uiIndex == uiTarget)
		{
			ptNewHead->ptNextHeader = ptHeadCurr ;
			ptHeadPrev->ptNextHeader = ptNewHead ;
			ptInfo->uiHeadNum++ ;
			return S_OK ;
		}
		ptHeadPrev = ptHeadCurr ;
		ptHeadCurr = (TImageHeader*)ptHeadCurr->ptNextHeader ;
		uiIndex++ ;
	}
}

SCODE ImageGenerator_HeadSearch(HANDLE hObject, UINT uiTarget, TImageHeader **ptHeadAns)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	TImageHeader *ptHeadCurr ;
	UINT uiIndex = 0 ;

	//find the target
	ptHeadCurr = ptInfo->ptHeadListStart ;
	while (ptHeadCurr != NULL )
	{
		if (uiIndex == uiTarget)
		{
			*ptHeadAns = ptHeadCurr ;
			return S_OK ;
		}
		ptHeadCurr = (TImageHeader*)(ptHeadCurr->ptNextHeader) ;
		uiIndex++ ;
	}

	return S_OK ;
}

SCODE ImageGenerator_HeadRemove(HANDLE hObject, UINT uiTarget)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	TImageHeader *ptHeadCurr, *ptHeadPrev ;
	UINT uiIndex = 0 ;

	ptHeadCurr = ptInfo->ptHeadListStart ;
	//If only one data in the list
	if (ptInfo->uiHeadNum == 1)
	{
        ptInfo->ptHeadListStart = NULL ;
        ptInfo->ptHeadListEnd = NULL ;
        ptInfo->uiHeadNum-- ;
        free(ptHeadCurr) ;
		return S_OK ;
    }
	//Remove the data at the beginning of list and update ptInfo->ptHeadListStart
	else if (uiTarget == 0)
	{
		ptInfo->ptHeadListStart = ptHeadCurr->ptNextHeader ;
		ptInfo->uiHeadNum-- ;
		free(ptHeadCurr) ;
		return S_OK ;
	}

	//remove the data at target position and update the related settings
	while (ptHeadCurr != NULL)
	{
	//	printf( "index=%d, target=%d\n", uiIndex, uiTarget ) ;
		if (uiIndex == uiTarget)
		{
			if ( uiTarget == ptInfo->uiHeadNum - 1 )
			{
				ptInfo->ptHeadListEnd = ptHeadPrev ;
				ptHeadPrev->ptNextHeader = NULL ;
				free(ptHeadCurr) ;
				ptInfo->uiHeadNum-- ;
				return S_OK ;
			}
			else
			{
				ptHeadPrev->ptNextHeader = ptHeadCurr->ptNextHeader ;
				ptHeadCurr->ptNextHeader = NULL ;
				free(ptHeadCurr) ;
				ptInfo->uiHeadNum-- ;
				return S_OK ;
			}
		}
		ptHeadPrev = ptHeadCurr ;
		ptHeadCurr = ptHeadCurr->ptNextHeader ;
		uiIndex++ ;
	}
}

SCODE ImageGenerator_ModifyDataName( TConfigData *ptData, CHAR pcNewName[20])
{
	if ( strcmp( ptData->acName, CONFIG_KEYWORD) != 0 )
	{
		strcpy( ptData->acName, pcNewName ) ;
	}
	return S_OK ;
}

SCODE ImageGenerator_ModifyDataAddress( TConfigData *ptData, DWORD dwAddress)
{
    if ( ptData->dwAddress != UNKNOWN_VALUE )
    {
	    //if ( dwAddress <= IMAGE_TOTAL_SIZE )
	    //{
		    ptData->dwAddress = dwAddress ;
	    //}
    }

	return S_OK ;
}

SCODE ImageGenerator_ModifyDataContent( TConfigData *ptData, DWORD dwContent)
{
	ptData->dwContent = dwContent ;
	return S_OK ;
}

SCODE ImageGenerator_ModifyHeadName( TImageHeader *ptHead, CHAR pcNewName[20])
{
	strcpy( ptHead->acName, pcNewName ) ;
	return S_OK ;
}

SCODE ImageGenerator_ModifyHeadContent( TImageHeader *ptHead, DWORD dwContent)
{
	if ( dwContent <= IMAGE_TOTAL_SIZE )
	{
		ptHead->dwValue = dwContent ;
	}
	return S_OK ;
}


SCODE ImageGenerator_ModifyInputFile(HANDLE hObject, CHAR *pcFileName)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	FILE *pfTmp ;

	if ( ptInfo->fInputFile != NULL )
	{
		fclose(ptInfo->fInputFile) ;
	}

	pfTmp = fopen( pcFileName, "r" ) ;
	if ( pfTmp == NULL )
	{
		return S_FAIL ;
	}
	ptInfo->fInputFile = pfTmp ;

	return S_OK ;
}

SCODE ImageGenerator_ModifyOutputFile(HANDLE hObject, CHAR *pcFileName)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	FILE *pfTmp ;

	if ( ptInfo->fOutputFile != NULL )
	{
		fclose(ptInfo->fOutputFile) ;
	}

	pfTmp = fopen( pcFileName, "wb" ) ;
	if ( pfTmp == NULL )
	{
		return S_FAIL ;
	}
	ptInfo->fOutputFile = pfTmp ;

	return S_OK ;
}

SCODE ImageGenerator_ModifyInputFilePtr(HANDLE hObject, FILE *pfFilePtr)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;

	printf("sss\n");
	if ( pfFilePtr == NULL ) {
		printf("ppp\n");
		return S_FAIL ;
	}

	printf("ppp\n");
	if ( ptInfo->fInputFile != NULL )
	{
		fclose(ptInfo->fInputFile) ;
	}

	ptInfo->fInputFile = pfFilePtr ;
	printf("sss\n");

	return S_OK ;
}

SCODE ImageGenerator_ModifyOutputFilePtr(HANDLE hObject, FILE *pfFilePtr)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;

	if ( pfFilePtr == NULL ) {
		return S_FAIL ;
	}

	if ( ptInfo->fOutputFile != NULL )
	{
		fclose(ptInfo->fOutputFile) ;
	}

	ptInfo->fOutputFile = pfFilePtr ;

	return S_OK ;
}

SCODE ImageGenerator_ModifyL1Image(HANDLE hObject, CHAR *pcFileName)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	FILE *pfTmp ;

	if ( ptInfo->fL1Image != NULL )
	{
		fclose(ptInfo->fL1Image) ;
	}

	pfTmp = fopen( pcFileName, "r" ) ;
	if ( pfTmp == NULL )
	{
		return S_FAIL ;
	}
	ptInfo->fL1Image = pfTmp ;

	return S_OK ;
}

SCODE ImageGenerator_ModifyUBootImage(HANDLE hObject, CHAR *pcFileName)
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	FILE *pfTmp ;

	if ( ptInfo->fUBootImage != NULL )
	{
		fclose(ptInfo->fUBootImage) ;
	}

	pfTmp = fopen( pcFileName, "r" ) ;
	if ( pfTmp == NULL )
	{
		return S_FAIL ;
	}
	ptInfo->fUBootImage = pfTmp ;

	return S_OK ;
}


SCODE ImageGenerator_ModifyUBootAddr( HANDLE hObject, DWORD dwAddr )
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	//if ( dwAddr <= IMAGE_TOTAL_SIZE )
	//{
		ptInfo->dwUBootAddr = dwAddr ;
	//}
	return S_OK ;
}

SCODE ImageGenerator_ModifyL1BootAddr( HANDLE hObject, DWORD dwAddr )
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	//if ( dwAddr <= IMAGE_TOTAL_SIZE )
	//{
		ptInfo->dwL1BootAddr = dwAddr ;
	//}
	return S_OK ;
}

SCODE ImageGenerator_ModifySector( HANDLE hObject, UINT uiSector)
{
//	DWORD dwAddress ;

	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	ptInfo->uiSector = uiSector ;

//	dwAddress = ptInfo->dwARMBootAddr + (ptInfo->uiSector * SECTOR_SIZE) ;
//	ImageGenerator_ModifyUBootAddr( hObject, dwAddress ) ;
//	dwAddress = ptInfo->dwL1BootAddr + (ptInfo->uiSector * SECTOR_SIZE) ;
//	ImageGenerator_ModifyL1BootAddr( hObject,  dwAddress) ;

	return S_OK ;
}

BYTE static ImageGenerator_LittleEndian( DWORD dwValue, UINT uiShift)
{
	return (BYTE)( dwValue >> (uiShift*8) ) ;
}

BYTE testarray[2*IMAGE_TOTAL_SIZE] ;
BYTE *image_copy ;
SCODE ImageGenerator_Synthesized(HANDLE hObject)
{
	//printf( "syn...\n" ) ;
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	UINT uiIndex, uiShift, uiHeadIndex ;
	TImageHeader *ptHeadCurr ;
	TConfigData *ptDataCurr ;
	DWORD dwDataAddr, dwDataContent ;
	DWORD dwL1Base  = ptInfo->dwL1BootAddr ;

    DWORD dwPreloaderBaseAddr = 0 ;
    UINT uiPreloaderHeaderIndex = 1 ;

    int imagesize = 0 ;
    if (storage_choice == STORAGE_NAND2KB) {
	    int image_use_nf_blocknum ;
	    fseek(ptInfo->fUBootImage,0,SEEK_END);
	    imagesize = ftell(ptInfo->fUBootImage) ;
	    rewind( ptInfo->fUBootImage ) ;
	    imagesize += ptInfo->dwUBootAddr ;

	    image_use_nf_blocknum = (imagesize % NF2KB_BLOCKSIZE != 0)? (imagesize / NF2KB_BLOCKSIZE) + 1 : (imagesize / NF2KB_BLOCKSIZE) ;
	    imagesize += image_use_nf_blocknum * 2048 ;
	    if(imagesize % NF2KB_BLOCKSIZE != 0) {
		    imagesize /= NF2KB_BLOCKSIZE ;
		    imagesize++ ;
		    imagesize *= NF2KB_BLOCKSIZE ;
	    }

	    ptInfo->bImage = (BYTE*)malloc(imagesize) ;
	    image_copy = (BYTE*)malloc(imagesize) ;
    }
    else
	    ptInfo->bImage = testarray ;

	//Based on the sector number, we should add the sector size to all address items
	DWORD dwSectorShift ;
    if (storage_choice == STORAGE_NAND2KB) {
        //dwSectorShift = (DWORD)(ptInfo->uiSector * IMAGE_SIZE_128K) ;
        dwSectorShift = 0 ;
    }
    else {
        dwSectorShift = (DWORD)(ptInfo->uiSector * IMAGE_SIZE_64K) ;
    }

	//output the config data
	ptHeadCurr = ptInfo->ptHeadListStart ;
	uiHeadIndex = 0 ;
	while( ptHeadCurr != NULL )
	{
		//write the header
		uiIndex = uiHeadIndex ;
		for( uiShift = 0 ; uiShift < 4 ; uiShift++ )
		{
			ptInfo->bImage[uiIndex] = ImageGenerator_LittleEndian( ptHeadCurr->dwValue + dwSectorShift, uiShift) ;
            //printf("ptInfo->bImage[uiIndex] = %x\n", ptInfo->bImage[uiIndex]) ;
			uiIndex++ ;
		}
		uiHeadIndex+=4 ;

        //Because we should write preLoader's base to the addr which 3rd header points to.
        if(uiPreloaderHeaderIndex == 3) {
            dwPreloaderBaseAddr = ptHeadCurr->dwValue ;
        }
        uiPreloaderHeaderIndex++ ;

		//Base on the content of header, we can get a address which configured data starts.
		//Then we write the data to ptInfo->bImage starts at the address.
		uiIndex = ptHeadCurr->dwValue ;
		if ( ptHeadCurr->ptList != NULL )
		{
			//Go through every data of every header, and output the configured data
			ptDataCurr = ptHeadCurr->ptList->ptListHead ;
			while( ptDataCurr != NULL )
			{
//				printf( "name = %s\n", ptDataCurr->pcName ) ;
				//if the address equals to UNKNOWN_VALUE, then we will not output it
				dwDataAddr = ptDataCurr->dwAddress ;// + dwSectorShift;
				if ( dwDataAddr != UNKNOWN_VALUE )
				{
					for( uiShift = 0 ; uiShift < 4  ; uiShift++ )
					{
						ptInfo->bImage[uiIndex] = ImageGenerator_LittleEndian( dwDataAddr, uiShift) ;
						uiIndex++ ;
					}
				}

				//output the content
				dwDataContent = ptDataCurr->dwContent ;
				if ( strcmp(ptDataCurr->acName, CONFIG_KEYWORD ) == 0 )
				{
					dwDataContent = dwDataContent + dwSectorShift ;
				}
				for( uiShift = 0 ; uiShift < 4 ; uiShift++ )
				{
					ptInfo->bImage[uiIndex] = ImageGenerator_LittleEndian( dwDataContent, uiShift) ;
					uiIndex++ ;
				}
				ptDataCurr = ptDataCurr->ptNextData ;
			}
		}
		ptHeadCurr = ptHeadCurr->ptNextHeader ;
	}

	//2010.05.27
	//Add uboot size to the end of general-config-list
	//get uboot size
	fseek(ptInfo->fUBootImage,0,SEEK_END);
	int uboot_size = ftell(ptInfo->fUBootImage) ;
	rewind( ptInfo->fUBootImage ) ;
	//check storage
	//if sd -> adjust to multiple 0x200
	//if sf -> adjust to multiple 0xF00
	//if nf -> adjust to multiple 2KB
	if(storage_choice == STORAGE_SD) {
		if((uboot_size % 0x200) != 0) {
			uboot_size = uboot_size / 0x200 ;
			uboot_size++ ;
			uboot_size = uboot_size * 0x200 ;
		}
	} else if(storage_choice == STORAGE_SF) {
		if((uboot_size % 0xF00) != 0) {
			uboot_size = uboot_size / 0xF00 ;
			uboot_size++ ;
			uboot_size = uboot_size * 0xF00 ;
		}
	} else if(storage_choice == STORAGE_NAND2KB) {//nand loader uses PAGESIZE
		if((uboot_size % 2048) != 0) {
			uboot_size = uboot_size / 2048 ;
			uboot_size++ ;
			//uboot_size = uboot_size * 2048 ;
		}
		else {
			uboot_size = uboot_size / 2048 ;
		}
	}
	//we write the adjusted size at 0x80
	uiIndex = 0x80 ;
	for( uiShift = 0 ; uiShift < 4 ; uiShift++ )
	{
			ptInfo->bImage[uiIndex] = ImageGenerator_LittleEndian( uboot_size, uiShift) ;
			uiIndex++ ;
	}
	if(storage_choice == STORAGE_NAND2KB)
		uboot_size = uboot_size * 2048 ;
	int final_image_size = uboot_size + ptInfo->dwUBootAddr ;
	if(final_image_size < IMAGE_TOTAL_SIZE)
		final_image_size = IMAGE_TOTAL_SIZE ;

    //in sd image, 0x8 must place l1-loader-base, 0xc must place l1-loader-size
    if(storage_choice == STORAGE_SD) {
        DWORD dwSDL1base = ptInfo->dwL1BootAddr ;
        DWORD dwSDL1size = 0 ;
        UINT uiTempIndex = 0;

        if ( ptInfo->fL1Image != NULL )
	    {
		    fseek(ptInfo->fL1Image,0,SEEK_END);
            dwSDL1size = ftell(ptInfo->fL1Image) ;
            rewind( ptInfo->fL1Image ) ;
        }

        uiTempIndex = 8 ;
        for( uiShift = 0 ; uiShift < 4 ; uiShift++ )
	   	{
	    	ptInfo->bImage[uiTempIndex] = ImageGenerator_LittleEndian( dwSDL1base, uiShift) ;
		    uiTempIndex++ ;
    	}

        for( uiShift = 0 ; uiShift < 4 ; uiShift++ )
		{
			ptInfo->bImage[uiTempIndex] = ImageGenerator_LittleEndian( dwSDL1size, uiShift) ;
			uiTempIndex++ ;
		}
    }
    else if(storage_choice == STORAGE_NAND2KB) {
        //if storage is nand flash.
        //We should add PreLoader to final image
        DWORD dwSDPreloaderBase = NAND2KB_PRELOADER_DEFAULT_BASE + (DWORD)(ptInfo->uiSector * IMAGE_SIZE_128K * 2);
        UINT uiTempIndex = 0;
        FILE *fPreLoaderImage ;

        uiTempIndex = dwPreloaderBaseAddr ;
        for( uiShift = 0 ; uiShift < 4 ; uiShift++ )
		{
			ptInfo->bImage[uiTempIndex] = ImageGenerator_LittleEndian( dwSDPreloaderBase, uiShift) ;
			uiTempIndex++ ;
		}

        fPreLoaderImage = fopen( NAND2KB_PERLOADER_FILENAME, "r" ) ;
        if ( fPreLoaderImage == NULL )
    	{
	    	printf( "!!NO PreLoader-Image!!\n" ) ;
    	}
	    else
    	{
	    	CHAR c ;
		    DWORD dwLength ;
            DWORD dwPreLoaderLength ;
            unsigned long *loader_offset1 ;
            unsigned long *loader_offset2 ;

            //If we want to generate boot image of nand, we should read "PRE-Loader" to bImage
    		//write the data from the dwL1Base
	    	uiIndex = NAND2KB_PRELOADER_DEFAULT_BASE ;

	    	fseek(fPreLoaderImage,0,SEEK_END);
		    dwPreLoaderLength = ftell(fPreLoaderImage) ;
            dwPreLoaderLength += uiIndex ;
	    	rewind(fPreLoaderImage) ;
		    while ( uiIndex < dwPreLoaderLength )
    		{
	    		c = fgetc(fPreLoaderImage) ;
		    	ptInfo->bImage[uiIndex] = c ;
			    uiIndex++ ;
    		}

            //Update the offset of loader which locates in the end of preloader binary image.
            loader_offset1 = (unsigned long*)(&(ptInfo->bImage[dwPreLoaderLength - 8])) ;
            loader_offset2 = (unsigned long*)(&(ptInfo->bImage[dwPreLoaderLength - 4])) ;
            //printf("loader offset1 = 0x%08lx\n", *loader_offset1) ;
            //printf("loader offset2 = 0x%08lx\n", *loader_offset2) ;
            *loader_offset1 += ptInfo->uiSector * 64 ;
            *loader_offset2 += ptInfo->uiSector * 64 ;
	    }
    }

	//Write the content of L1BootImage to ptInfo->bImage
	if ( ptInfo->fL1Image == NULL )
	{
		printf( "!!NO L1-Image!!\n" ) ;
	}
	else
	{
		CHAR c ;
		DWORD dwLength ;

		//write the data from the dwL1Base
		uiIndex = dwL1Base ;
//		printf( "l1 addr = %x\n", dwL1Base ) ;

		fseek(ptInfo->fL1Image,0,SEEK_END);
		dwLength = ftell(ptInfo->fL1Image) ;
		dwLength += uiIndex ;
		rewind( ptInfo->fL1Image ) ;
		while ( uiIndex < dwLength )
		{
			c = fgetc(ptInfo->fL1Image) ;
			ptInfo->bImage[uiIndex] = c ;
			uiIndex++ ;
		}

        //Update the offset of uboot when SF Dual Boot
        if(storage_choice == STORAGE_SF) {
            unsigned long *ubootOffset = (unsigned long *)(&(ptInfo->bImage[uiIndex -4])) ;
            *ubootOffset = ptInfo->dwUBootAddr + dwSectorShift ;
        }
        else if(storage_choice == STORAGE_NAND2KB) {
            unsigned long *uboot_offset ;
            uboot_offset = (unsigned long*)(&(ptInfo->bImage[dwLength - 4])) ;
            //printf("uboot_offset = %d\n", *uboot_offset) ;
            *uboot_offset += ptInfo->uiSector * 64 ;
        }
	}

	//Write the content of UBootImage to ptInfo->bImage
	if ( ptInfo->fUBootImage == NULL )
	{
		printf( "!!NO UBoot img!!\n" ) ;
	}
	else
	{
		CHAR c ;
		DWORD dwLength ;

		//write the data from ptInfo->dwUBootAddr
		uiIndex = ptInfo->dwUBootAddr ;
//        printf("uiIndex = %d\n", uiIndex) ;

		fseek(ptInfo->fUBootImage,0,SEEK_END);
		dwLength = ftell(ptInfo->fUBootImage) ;
		dwLength += uiIndex ;
		rewind( ptInfo->fUBootImage ) ;
        //printf( "dwLength = %d\n", dwLength) ;
        int amount = 0 ;
		while ( uiIndex < dwLength )
		{
			c = fgetc(ptInfo->fUBootImage) ;
			ptInfo->bImage[uiIndex] = c ;
			uiIndex++ ;
            amount++ ;
          //  printf("amount = %d\n", amount) ;
		}
	}

    printf("generating...\n" ) ;

	if(storage_choice == STORAGE_NAND2KB) {
		//      We will add Magic Number to the last page(2048 Bytes) of each block(128K Bytes).
		int lastpage_startaddr = ((NF2KB_BLOCKSIZE / 2048) - 1) * 2048 ;
		int databuf_index = 0 ;
		int datacopy_index = 0 ;

		unsigned long* bootimage_bufptr ;

		int uboot_size_val ;
		fseek(ptInfo->fUBootImage,0,SEEK_END);
		uboot_size_val = ftell(ptInfo->fUBootImage) ;
		rewind( ptInfo->fUBootImage ) ;

		while(datacopy_index < imagesize) {
			if((datacopy_index % NF2KB_BLOCKSIZE) != lastpage_startaddr) {
				if(databuf_index < (ptInfo->dwUBootAddr + uboot_size_val)) {
					image_copy[datacopy_index] = ptInfo->bImage[databuf_index] ;
					databuf_index++ ;
				}
				datacopy_index++ ;
			}
			else {
				//write magic#
				bootimage_bufptr = (unsigned long*)&image_copy[datacopy_index] ;
				*bootimage_bufptr = MAGICNUMBER_1 ;
				bootimage_bufptr++ ;
				*bootimage_bufptr = MAGICNUMBER_2 ;
				datacopy_index += 2048 ;
			}
		}

		for ( uiIndex = 0 ; uiIndex < imagesize ; uiIndex++ )
		{
		    	//printf("index = %d\n", uiIndex) ;
			fputc( image_copy[uiIndex],  ptInfo->fOutputFile) ;
		}
	}
	else {
		//output the content of ptInfo->bImage to the output file
		for ( uiIndex = 0 ; uiIndex < final_image_size ; uiIndex++ )
		{
		    	//printf("index = %d\n", uiIndex) ;
			fputc( ptInfo->bImage[uiIndex],  ptInfo->fOutputFile) ;
		}
	}
}
