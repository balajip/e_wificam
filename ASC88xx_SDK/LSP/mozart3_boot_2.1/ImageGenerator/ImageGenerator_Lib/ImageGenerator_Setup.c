/*
 * Copyright 2008 VN Inc.  All rights reserved.
 *
 * Description:
 *
 */

/* ============================================================================================== */
#include "ImageGenerator_Locals.h"

/* ============================================================================================== */

int storage_choice = -1 ;//0:SD, 1:serial, 2:nand 2KB page, 3:nand 4KB page

void static ImageGenerator_ParseFile_ConstructData(TImageGeneratorInfo *ptInfo )
{
	ptInfo->uiHeadNum = 0 ;
	FILE *fInput = ptInfo->fInputFile ;

    TImageHeader *ptHeadCurr, *ptHeadPrev ;
	TConfigDataList* ptList ;
	TConfigData *ptPrev, *ptCurr ;

    //HEADER data
    CHAR acHeadName[30] ;
	DWORD dwHeadValue ;
	UINT uiConfigNum ;

	UINT uiIndex ;

    //CONFIGURED DATA data
	DWORD dwDataAddr ;
	DWORD dwDataContent ;
	CHAR acDataName[30] ;

	//parse the formatted file
    while ( fscanf( fInput, "%s 0x%x %d\n", acHeadName, &dwHeadValue, &uiConfigNum ) != EOF ) //parse the header info
	{
		ptInfo->uiHeadNum++ ;

		//init the header accoding the input Config file
		ptHeadCurr = (TImageHeader*)malloc(sizeof(TImageHeader)) ;
		strcpy( ptHeadCurr->acName, acHeadName ) ;
		ptHeadCurr->dwValue = dwHeadValue ;

		if ( uiConfigNum != 0 ) //this headr have data in the configured data list
		{
			//init the list data structure
			ptHeadCurr->ptList = (TConfigDataList*)malloc(sizeof(TConfigDataList)) ;
			ptList = ptHeadCurr->ptList ;
			ptList->iListSize = 0 ;
			ptList->ptListHead =  (TConfigData *)malloc(sizeof(TConfigData)) ;
			ptPrev = ptList->ptListHead ;

			//parse the file and build the first Config Data structure of the list
			fscanf( fInput, "%s 0x%x 0x%x\n", acDataName, &dwDataAddr, &dwDataContent ) ;
			strcpy( ptList->ptListHead->acName , acDataName );
   		    ptList->ptListHead->dwAddress = dwDataAddr ;
			ptList->ptListHead->dwContent = dwDataContent ;
			if ( strcmp(CONFIG_KEYWORD, ptList->ptListHead->acName) == 0 )
   		    {
		       printf("L1Base catch!!!\n" ) ;
		       ImageGenerator_ModifyL1BootAddr( ptInfo, ptList->ptListHead->dwContent ) ;
            }
			ptList->ptListEnd = ptList->ptListHead ;
			ptList->iListSize++ ;

			//build the other Config Data Structure of the list
			for ( uiIndex = 1 ; uiIndex < uiConfigNum ; uiIndex++ )
			{
				fscanf( fInput, "%s 0x%x 0x%x\n", acDataName, &dwDataAddr, &dwDataContent ) ;
				printf( "%s 0x%x 0x%x\n", acDataName, dwDataAddr, dwDataContent ) ;
				ptList->ptListEnd = (TConfigData *)malloc(sizeof(TConfigData)) ;
				strcpy( ptList->ptListEnd->acName , acDataName );
				ptList->ptListEnd->dwAddress = dwDataAddr ;
				ptList->ptListEnd->dwContent = dwDataContent ;
				if ( strcmp(CONFIG_KEYWORD, ptList->ptListEnd->acName) == 0 )
				{
           		       printf("L1Base catch!!!\n" ) ;
           		       ImageGenerator_ModifyL1BootAddr( ptInfo, ptList->ptListEnd->dwContent ) ;
                }
				ptPrev->ptNextData = ptList->ptListEnd ;
				ptPrev = ptList->ptListEnd ;
				ptList->iListSize++ ;
			}

			if ( ptList->ptListEnd != NULL )
			{
				ptList->ptListEnd->ptNextData = NULL ;
			}
		}
		else //the Header do not have any data in Config Data List (but in default, every header has the list)
		{
			ptHeadCurr->ptList = (TConfigDataList*)malloc(sizeof(TConfigDataList)) ;
			ptList = ptHeadCurr->ptList ;
			ptList->ptListHead = NULL ;
			ptList->ptListEnd = NULL ;
			ptList->iListSize = 0 ;
		}

		//if this Header is the first Header, then we should also update ptInfo->ptHeadListStart
		if (  ptInfo->uiHeadNum == 1 )
		{
			ptInfo->ptHeadListStart = ptHeadCurr ;
			ptHeadPrev = ptHeadCurr ;
		}
		else
		{
			ptHeadPrev->ptNextHeader = ptHeadCurr ;
			ptHeadPrev = ptHeadCurr ;
		}
	}
	ptInfo->ptHeadListEnd = ptHeadCurr ;
	ptHeadCurr->ptNextHeader = NULL ;
}

SCODE ImageGenerator_Initial(HANDLE *phObject, TImageGeneratorInitOptions *ptInitOptions)
{
	TImageGeneratorInfo *ptInfo;
	TConfigDataList *ptConfigDataList ;
	CHAR acContent[9];
	CHAR acAddr[9] ;

	// allocate memory to handler
	if ((ptInfo=(TImageGeneratorInfo *)malloc(sizeof(TImageGeneratorInfo))) == NULL)
	{
		DBPRINT0("Allocate TImageGeneratorInfo fail !!\n");
		return S_FAIL;
	}
	*phObject = (HANDLE)ptInfo;

    //parse the config input file
	//parse the Header Data
	//ImageGenerator_ModifyInputFile(ptInfo, ptInitOptions->pcInputName) ;
	if( ImageGenerator_ModifyInputFilePtr( ptInfo, ptInitOptions->pfInputFile ) == S_FAIL )
	{
		return S_FAIL ;
	}

    //set default address
	ImageGenerator_ModifyUBootAddr( ptInfo, DEFAULT_UBOOT_ADDRESS ) ;
	ImageGenerator_ModifyL1BootAddr( ptInfo, DEFAULT_L1BOOT_ADDRESS ) ;

	//build the header list
	ImageGenerator_ParseFile_ConstructData(ptInfo);//, fInput) ;

	//set the output files
	ImageGenerator_ModifyOutputFile(ptInfo, ptInitOptions->pcOutputName);

	return S_OK;
}

SCODE ImageGenerator_Release(HANDLE *phObject)
{
	TImageGeneratorInfo *ptInfo = (TImageGeneratorInfo *)(*phObject);
	TImageHeader *ptHeadCurr, *ptHeadNext ;
	TConfigData *ptDataCurr, *ptDataNext ;

	// release every Config Data and Header
	ptHeadCurr = ptInfo->ptHeadListStart ;
	ptHeadNext = ptHeadCurr->ptNextHeader ;
	while ( ptHeadCurr != NULL )
	{
		ptDataCurr = ptHeadCurr->ptList->ptListHead ;
		ptDataNext = ptDataCurr->ptNextData ;
		while( ptDataCurr != NULL )
		{
			free(ptDataCurr) ;
			ptDataCurr = ptDataNext ;
			if ( ptDataCurr != NULL )
			{
				ptDataNext = ptDataCurr->ptNextData ;
			}
		}

		free(ptHeadCurr->ptList) ;
		free(ptHeadCurr) ;
		ptHeadCurr = ptHeadNext ;
		if(ptHeadCurr != NULL )
		{
			ptHeadNext = ptHeadCurr->ptNextHeader ;
		}
	}

	//close all files
	if (ptInfo->fInputFile != NULL )
	{
		fclose(ptInfo->fInputFile) ;
	}
	if (ptInfo->fOutputFile != NULL )
	{
		fclose(ptInfo->fOutputFile) ;
	}
	if (ptInfo->fL1Image != NULL )
	{
		fclose(ptInfo->fL1Image) ;
	}
	if (ptInfo->fUBootImage != NULL )
	{
		fclose(ptInfo->fUBootImage) ;
	}

        //release the TImageGeneratorInfo
        if (ptInfo != NULL)
        {
                free(ptInfo);
        }
        *phObject = (HANDLE)NULL;

        return S_OK;
}

