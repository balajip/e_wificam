/*
 *
 * Copyright 2008 VN Inc.  All rights reserved.
 *
 * Description:
 *
 */

/* ======================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "ImageGenerator.h"
#include "ImgGen_Cmdline.h"

/* ======================================================================== */

char acL1FileName[256] = {0} ;
char acUBootFileName[256] = {0} ;
FILE *fOutput, *fL1Image, *fUBootImage ;

#define   ESC           '\033'
#define   WHITE       37
#define   RED           31
#define   YELLOW     33
#define   GREEN       32
#define   BLUE         34
#define   BROWN       35
#define   MEGEN       36
#define   GRAY         38
void ImageGenerator_SETCOLOR(int color)
{
        printf("%c[%dm",ESC,color);
}

void   ImageGenerator_CANCELCOLOR()
{
        printf("%c[m",ESC);
}

void ImageGenerator_PrintString( UINT uiColor, PCHAR pcString)
{
	ImageGenerator_SETCOLOR( uiColor ) ;
	printf( "%s", pcString ) ;
	ImageGenerator_CANCELCOLOR();
}

void ImageGenerator_PrintOption( UINT uiIndex, PCHAR pcName, DWORD dwAddress, DWORD dwContent )
{
	UINT uiSpaceNum = 25 ;

	//print index
	ImageGenerator_SETCOLOR( MEGEN ) ;
	printf( "<%3d> ", uiIndex ) ;
	ImageGenerator_CANCELCOLOR();

	//print name
	ImageGenerator_SETCOLOR( WHITE ) ;
	printf( "%s ", pcName ) ;
	ImageGenerator_CANCELCOLOR();

	//print space
	uiSpaceNum = uiSpaceNum - strlen( pcName ) ;
	while( uiSpaceNum > 0 )
	{
		printf( " " ) ;
		uiSpaceNum-- ;
	}

	//print address
	ImageGenerator_PrintString( WHITE, "( " ) ;
	ImageGenerator_PrintString( YELLOW, "addr " ) ;
	ImageGenerator_PrintString( WHITE, ": " ) ;
	ImageGenerator_SETCOLOR( YELLOW ) ;
	if ( dwAddress != UNKNOWN_VALUE )
	{
		printf( "0x%08x ", dwAddress ) ;
	}
	else
	{
		printf( "It's Fixed " ) ;
	}
	ImageGenerator_CANCELCOLOR();

	//print content
	ImageGenerator_PrintString( WHITE, "| " ) ;
	ImageGenerator_PrintString( BROWN, "content " ) ;
	ImageGenerator_PrintString( WHITE, ": " ) ;
	ImageGenerator_SETCOLOR( BROWN ) ;
	printf( "0x%08x ", dwContent ) ;
	ImageGenerator_CANCELCOLOR();
	ImageGenerator_PrintString( WHITE, ")\n" ) ;
}

SCODE ImageGenerator_DataSubMenu(TConfigDataList *ptList, UINT uiChoice)
{
	uiChoice-- ;
	TConfigData *ptTarget ;
	ImageGenerator_ListSearch(ptList, uiChoice, &ptTarget) ;
	UINT uiOption = 0 ;
	CHAR acNewName[30] ;
	DWORD dwAddr, dwContent ;


	while ( uiOption != 4 )
	{
		ImageGenerator_PrintString( RED, "======== Image Generator =========\n" ) ;
		ImageGenerator_PrintString( GREEN, "\n######## Data Part ########\n" ) ;

		system( "clear" ) ;

		ImageGenerator_PrintOption( 0, ptTarget->acName, ptTarget->dwAddress, ptTarget->dwContent ) ;
		printf( "# (1). Modify Name.\n" ) ;
		printf( "# (2). Modify Address.\n" ) ;
		printf( "# (3). Modify Content.\n" ) ;
		printf( "# (4). Back To Top.\n" ) ;
		printf( "# Enter Your Choice [1~4] > " ) ;
		scanf( "%d", &uiOption ) ;

		switch ( uiOption ) {
			case 1 : //modify name
			   printf ( "## The New Name = " ) ;
			   scanf( "%s", acNewName ) ;
			   ImageGenerator_ModifyDataName( ptTarget, acNewName) ;
			   break ;
			case 2 : //modify address
			   printf ( "## The New Addr = " ) ;
			   scanf( "%x", &dwAddr ) ;
			   ImageGenerator_ModifyDataAddress( ptTarget, dwAddr);
			   break ;
			case 3 : //modify content
			   printf ( "## The New Content = " ) ;
			   scanf( "%x", &dwContent ) ;
	        	   ImageGenerator_ModifyDataContent( ptTarget, dwContent);
			   break ;
		} ;
	}

	return S_OK ;
}

SCODE ImageGenerator_ListSubMenu(TConfigDataList *ptList)
{
	UINT uiListOption = 0 ;
	UINT uiChoice ;
	UINT uiIndex = 0 ;
	TConfigData *ptDataCurr ;
	TConfigData *ptNewData ;

	while( uiListOption != 4 )
	{
		system( "clear" ) ;
		uiIndex = 0 ;

		ImageGenerator_PrintString( RED, "======== Image Generator =========\n" ) ;
		ImageGenerator_PrintString( GREEN, "\n######## ConfigData List Part ########\n" ) ;

		//Print the Config Data List
		ptDataCurr = ptList->ptListHead ;
		while( ptDataCurr != NULL )
		{
			uiIndex++ ;
			ImageGenerator_PrintOption( uiIndex, ptDataCurr->acName, ptDataCurr->dwAddress, ptDataCurr->dwContent ) ;
			ptDataCurr = ptDataCurr->ptNextData ;
		}

		printf( "# (1). Modify Data.\n" ) ;
		printf( "# (2). Add Data.\n" ) ;
		printf( "# (3). Remove Data.\n" ) ;
		printf( "# (4). Back To Top.\n" ) ;
		printf( "# Enter Your Choice [1~4] > " ) ;
		scanf( "%d", &uiListOption ) ;

		switch( uiListOption )
		{
			case 1 : //modify ConfigData
				printf( "## Enter the data number : " ) ;
				scanf( "%d", &uiChoice ) ;
				ImageGenerator_DataSubMenu( ptList, uiChoice) ;
				break ;
			case 2 : //add data
	        		ptNewData = (TConfigData *)malloc(sizeof(TConfigData)) ;
        			strcpy (ptNewData->acName ,"New") ;
				ptNewData->dwAddress = 0 ;
				ptNewData->dwContent = 0 ;
		        	ImageGenerator_ListInsert(ptList, ptNewData, ptList->iListSize ) ;
				break ;
			case 3 : //remove data
				printf( "!!!Please Enter The Choice => " ) ;
				scanf( "%d", &uiChoice ) ;
				uiChoice-- ;
				ImageGenerator_ListRemove( ptList, uiChoice ) ;
				break ;
			case 4 :
				return S_OK ;

		};
	}//end while uiListOption != 2
	return S_FAIL ;
}

SCODE ImageGenerator_DispatchChoice( HANDLE hObject, UINT uiChoice )
{
	TImageGeneratorInfo *ptInfo = ( TImageGeneratorInfo *)hObject ;
	UINT uiOption ;
	DWORD dwAddr, dwContent ;
	CHAR acNewName[30] ;
	CHAR acTmpName[50] ;
	UINT uiSector ;
    CHAR cTemp ;

	//file part
	if ( uiChoice == 100 )
	{
		printf( "!!!Please Enter new L1 Image File Name : " );
		scanf( "%s", acTmpName ) ;
		if(ImageGenerator_ModifyL1Image(hObject, acTmpName) == S_OK)
		{
			strcpy( acL1FileName, acTmpName ) ;
		}
	}
	else if ( uiChoice == 101 )
	{
		printf( "!!!Please Enter new UBoot Image File Name : " );
		scanf( "%s", acTmpName ) ;
		if(ImageGenerator_ModifyUBootImage(hObject, acTmpName) == S_OK)
		{
			strcpy( acUBootFileName, acTmpName ) ;
		}
	}
	//misc part
	else if ( uiChoice == 200 )
	{
	    if((storage_choice == STORAGE_SD) || (storage_choice == STORAGE_NAND2KB)) {
            printf("Sorry..You cannot modify this when using this storage!(ENTER ANY KEY to continue..)\n") ;
            cTemp = getchar() ;
            cTemp = getchar() ;
            return S_OK ;
	    }
		TImageHeader *ptNew ;
        	ptNew = (TImageHeader *)malloc(sizeof(TImageHeader)) ;
        	strcpy (ptNew->acName ,"New") ;
		ImageGenerator_HeadInsert(ptInfo, ptNew, ptInfo->uiHeadNum);
	}
	else if ( uiChoice == 201 )
	{
	    if((storage_choice == STORAGE_SD) || (storage_choice == STORAGE_NAND2KB)) {
            printf("Sorry..You cannot modify this when using this storage!(ENTER ANY KEY to continue..)\n") ;
            cTemp = getchar() ;
            cTemp = getchar() ;
            return S_OK ;
	    }
		printf( "!!!Please Enter The Choice => " ) ;
		scanf( "%d", &uiOption ) ;
		ImageGenerator_HeadRemove( ptInfo, uiOption-1 ) ;
	}
	else if ( uiChoice == 202 )
	{
	    if(storage_choice == STORAGE_SD) {
            printf("Sorry..You cannot modify this when using this storage!(ENTER ANY KEY to continue..)\n") ;
            cTemp = getchar() ;
            cTemp = getchar() ;
            return S_OK ;
	    }
		printf( "!!!Please Enter The Address => " ) ;
		scanf( "%x", &dwAddr ) ;
		ImageGenerator_ModifyUBootAddr( hObject, dwAddr ) ;
	}
	else if ( uiChoice == 203 )
	{
	    if(storage_choice == STORAGE_NAND2KB) {
            printf("Sorry..You cannot modify this when using this storage!(ENTER ANY KEY to continue..)\n") ;
            cTemp = getchar() ;
            cTemp = getchar() ;
            return S_OK ;
	    }
		printf( "!!!Please Enter The Address => " ) ;
		scanf( "%x", &dwAddr ) ;
		ImageGenerator_ModifyL1BootAddr( hObject, dwAddr ) ;
	}
	else if ( uiChoice == 204 )
	{
	    if(storage_choice != STORAGE_SD) {
    		printf( "!!!Please Enter The Sector Number => " ) ;
	    	scanf( "%d", &uiSector ) ;
		    ImageGenerator_ModifySector(hObject, uiSector) ;
	    }
	}
	//HEAD part
	else  if ( uiChoice <= ptInfo->uiHeadNum )
	{
	    if(storage_choice == STORAGE_SD) {
            if(uiChoice > 2) {
                printf("Sorry..You cannot modify this when using this storage!(ENTER ANY KEY to continue..)\n") ;
                cTemp = getchar() ;
                cTemp = getchar() ;
                return S_OK ;
            }
	    }
		uiChoice-- ;
		TImageHeader *ptHeadTarget ;
		ImageGenerator_HeadSearch(ptInfo, uiChoice, &ptHeadTarget) ;
		uiOption = 0 ;
		while( uiOption != 4 )
		{
			system( "clear" ) ;

			ImageGenerator_PrintString( RED, "======== Image Generator =========\n" ) ;
			ImageGenerator_PrintString( GREEN, "\n######## Head Part ########\n" ) ;
			ImageGenerator_PrintOption( 0, ptHeadTarget->acName, UNKNOWN_VALUE, ptHeadTarget->dwValue ) ;
			printf( "# (1). Modify Name.\n" ) ;
			printf( "# (2). Modify Content.\n" ) ;
			printf( "# (3). Modify ConfigData.\n" ) ;
			printf( "# (4). Back To Top.\n" ) ;
			printf( "# Enter Your Choice [1~4] > " ) ;
			scanf( "%d", &uiOption ) ;

			switch ( uiOption ) {
				case 1 :
				   printf ( "## The New Name = " ) ;
				   scanf( "%s", acNewName ) ;
				   ImageGenerator_ModifyHeadName( ptHeadTarget, acNewName) ;
				   break ;
				case 2 :
				   printf ( "## The New Content = " ) ;
				   scanf( "%x", &dwContent ) ;
			           ImageGenerator_ModifyHeadContent( ptHeadTarget, dwContent);
				   break ;
				case 3 :
				   ImageGenerator_ListSubMenu(ptHeadTarget->ptList) ;
				   break ;
			} ;
		}
	}

	return S_OK ;
}

SCODE ImageGenerator_MainMenu( HANDLE hImageGenerator )
{
	UINT uiChoice, uiIndex ;
	TImageGeneratorInfo *ptInfo = (TImageGeneratorInfo *)hImageGenerator ;
	TImageHeader *ptHeadCurr;

	//initialize the default file name
	switch(storage_choice) {
        case STORAGE_SD:
            strcpy( acL1FileName, SD_DEFAULT_LOADER ) ;
	        strcpy( acUBootFileName, SD_DEFAULT_UBOOT ) ;
        	ImageGenerator_ModifyL1BootAddr( ptInfo, SD_LOADER_DEFAULT_BASE ) ;
            ImageGenerator_ModifyUBootAddr( ptInfo, SD_UBOOT_DEFAULT_BASE ) ;
            break ;
        case STORAGE_SF:
            strcpy( acL1FileName, SF_DEFAULT_LOADER ) ;
        	strcpy( acUBootFileName, SF_DEFAULT_UBOOT ) ;
        	ImageGenerator_ModifyL1BootAddr( ptInfo, SF_LOADER_DEFAULT_BASE ) ;
            ImageGenerator_ModifyUBootAddr( ptInfo, SF_UBOOT_DEFAULT_BASE ) ;
            break ;
        case STORAGE_NAND2KB:
            strcpy( acL1FileName, NAND2KB_DEFAULT_LOADER ) ;
        	strcpy( acUBootFileName, NAND2KB_DEFAULT_UBOOT ) ;
        	ImageGenerator_ModifyL1BootAddr( ptInfo, NAND2KB_LOADER_DEFAULT_BASE) ;
            ImageGenerator_ModifyUBootAddr( ptInfo, NAND2KB_UBOOT_DEFAULT_BASE ) ;
            //ImageGenerator_ModifyOutputFile(ptInfo, NAND2KB_DEFAULT_OUTPUTFILE) ;
            break ;
        case STORAGE_NAND4KB:
            strcpy( acL1FileName, NAND4KB_DEFAULT_LOADER ) ;
        	strcpy( acUBootFileName, NAND4KB_DEFAULT_UBOOT ) ;
        	ImageGenerator_ModifyL1BootAddr( ptInfo, NAND4KB_LOADER_DEFAULT_BASE ) ;
            ImageGenerator_ModifyUBootAddr( ptInfo, NAND4KB_UBOOT_DEFAULT_BASE ) ;
            //ImageGenerator_ModifyOutputFile(ptInfo, NAND4KB_DEFAULT_OUTPUTFILE) ;
            break ;
        default :
            printf( "[WRONG!!]\n") ;
            return S_FAIL ;
    } ;

	ImageGenerator_ModifyL1Image(hImageGenerator, acL1FileName) ;
	ImageGenerator_ModifyUBootImage(hImageGenerator, acUBootFileName) ;

	while(1)
	{
		system( "clear" ) ;
		ImageGenerator_PrintString( RED, "========== Image Generator =========\n" ) ;
		ImageGenerator_PrintString( RED, "========== Main Menu =========\n" ) ;

		//print HEDAER part
		ptHeadCurr = ptInfo->ptHeadListStart ;
		uiIndex = 0 ;
		ImageGenerator_PrintString( GREEN, "\n######### Header Part #########\n" ) ;
		while ( (ptHeadCurr != NULL) )
		{
			uiIndex++ ;
			ImageGenerator_PrintOption( uiIndex, ptHeadCurr->acName, 0xffffffff, ptHeadCurr->dwValue ) ;

			ptHeadCurr = ptHeadCurr->ptNextHeader ;
        	}

		//print FILE part
		ImageGenerator_PrintString(GREEN, "\n######## File Part ########\n" ) ;
		ImageGenerator_PrintString( MEGEN, "<100> " );
        ImageGenerator_PrintString( WHITE, "Modify the path of L1 Image File : " ) ;
		printf( "%s\n", acL1FileName ) ;
		ImageGenerator_PrintString( MEGEN, "<101> " );
		ImageGenerator_PrintString( WHITE, "Modify the path of UBoot Image File : " ) ;
		printf( "%s\n", acUBootFileName) ;
		ImageGenerator_PrintString( MEGEN, "<102> " );
		ImageGenerator_PrintString( WHITE, "Generate the BOOT-IMAGE\n" ) ;

		//print MISC part
		ImageGenerator_PrintString(GREEN, "\n######## Misc Part ########\n" ) ;
		ImageGenerator_PrintString( MEGEN, "<200> " );
		ImageGenerator_PrintString( WHITE, "Add New Header\n" ) ;
		ImageGenerator_PrintString( MEGEN, "<201> " );
		ImageGenerator_PrintString( WHITE, "Remove Header\n" ) ;
		ImageGenerator_PrintString( MEGEN, "<202> " );
		ImageGenerator_PrintString( WHITE, "Set the RELATIVE offset of UBoot in the boot-image :" ) ;
		printf( "0x%x\n", ptInfo->dwUBootAddr ) ;
		ImageGenerator_PrintString( MEGEN, "<203> " );
		ImageGenerator_PrintString( WHITE, "Set the RELATIVE offset of LOADER in the boot-image :" ) ;
		printf( "0x%x\n", ptInfo->dwL1BootAddr ) ;
        if(storage_choice == STORAGE_SF) {
		    ImageGenerator_PrintString( MEGEN, "<204> " );
    		ImageGenerator_PrintString( WHITE, "Select the SF Block Number :" ) ;
	    	printf( "%d\n", ptInfo->uiSector ) ;
        }
        else if(storage_choice == STORAGE_NAND2KB) {
		    ImageGenerator_PrintString( MEGEN, "<204> " );
    		ImageGenerator_PrintString( WHITE, "Set Block Number :" ) ;
	    	printf( "%d\n", ptInfo->uiSector ) ;
        }


		ImageGenerator_PrintString( WHITE, "\n@ Enter Your Choice => " ) ;
		scanf( "%d", &uiChoice ) ;

		//Leave the menu
		if ( uiChoice == 102 )
		{
			return S_OK ;
		}

		//dispatch the choice
		ImageGenerator_DispatchChoice( hImageGenerator, uiChoice ) ;
		system( "clear" ) ;
	}
}

/* ======================================== */
#define ENTERKEY 10

SCODE ImageGenerator_ChooseInputFile( TImageGeneratorInitOptions *pInitOptions )
{
	CHAR acConfigFileName[256] ;
	CHAR *acDefaultConfigName = "Mozart120_Config" ;
	SCODE scResult = S_FAIL ;
	UINT uiIndex = 0 ;
	CHAR acTempChar ;
	FILE *pfInput ;

	system( "clear" ) ;
    printf( "%% ==================================== %%\n" ) ;
    printf( "%% ========== ImageGenerator ========== %%\n" ) ;
    printf( "%% ==================================== %%\n" ) ;
	printf( "  Welcome to use ImageGenerator\n\n" ) ;
    printf( " # Please choose one storage \n") ;
    printf( " # (1)SD Card\n" ) ;
    printf( " # (2)Serial Flash\n" );
    printf( " # (3)Nand Flash - 2KB page\n" ) ;
    //printf( " # (4)Nand Flash - 4KB page\n" ) ;
    printf( " # =>" ) ;
  	acTempChar = getchar() ;
    while(   ((acTempChar - '1') >= 3)
          || ((acTempChar - '1') < 0)
         ) {
        printf( "\r # =>" ) ;
        acTempChar = getchar() ;
    }
    storage_choice = acTempChar - '1' ;

    switch(storage_choice) {
        case STORAGE_SD:
            acDefaultConfigName = SD_DEFAULT_CONFIG_FILE ;
            pInitOptions->pcOutputName = SD_DEFAULT_OUTPUTFILE ;
            break ;
        case STORAGE_SF:
            acDefaultConfigName = SF_DEFAULT_CONFIG_FILE ;
            pInitOptions->pcOutputName = SF_DEFAULT_OUTPUTFILE ;
            break ;
        case STORAGE_NAND2KB:
            acDefaultConfigName = NAND2KB_DEFAULT_CONFIG_FILE ;
            pInitOptions->pcOutputName = NAND2KB_DEFAULT_OUTPUTFILE ;
            break ;
        //case STORAGE_NAND4KB:
        //    acDefaultConfigName = NAND4KB_DEFAULT_CONFIG_FILE ;
        //    break ;
        default :
            printf( "[WRONG!!]\n") ;
            return S_FAIL ;
    } ;

	printf( "  \nPlease enter the input configured file name(If using default %s, just hit Enter)\n", acDefaultConfigName ) ;
	printf( "  ==> " ) ;

    acTempChar = getchar() ;
	acTempChar = getchar() ;
	for( uiIndex = 0 ; uiIndex < 256 ; uiIndex++ ) {
		if ( acTempChar != ENTERKEY )
		{
			acConfigFileName[uiIndex] = acTempChar ;
		}
		else
		{
			if ( uiIndex == 0 )
			{
				acConfigFileName[0] = acTempChar ;
			}
			break ;
		}
		acTempChar = getchar() ;
	}

	if ( acConfigFileName[0] == ENTERKEY ) {
		printf( "\nEqual to enterkey!\n" ) ;
		printf( "Using Default %s\n", acDefaultConfigName) ;
		pfInput = fopen( acDefaultConfigName, "r" ) ;
		if ( pfInput == NULL ) {
			printf( "Failed to open %s\n", acDefaultConfigName ) ;
		}
		else
		{
			printf( "open ok!\n" ) ;
			pInitOptions->pfInputFile = pfInput ;
			scResult = S_OK ;
		}
	}
	else {
		printf( "\nNot equal!\n" ) ;
		printf( "Using new file %s\n", acConfigFileName ) ;
		pfInput = fopen( acConfigFileName, "r" ) ;
		if ( pfInput == NULL ) {
			printf( "Failed to open %s\n", acConfigFileName ) ;
		}
		else
		{
			printf( "open ok!\n" ) ;
			pInitOptions->pfInputFile = pfInput ;
			scResult = S_OK ;
		}

	}

	return scResult ;
}

/* ======================================== */

//int main(int argc, char* argv[])
int main_guimode(void)
{

	HANDLE hImageGenerator;  // handle
	TImageGeneratorInitOptions tImageGeneratorInitOptions;  // init options

	//initialize the object
	memset(&tImageGeneratorInitOptions, 0, sizeof(TImageGeneratorInitOptions));

	if (ImageGenerator_ChooseInputFile( &tImageGeneratorInitOptions ) != S_OK )
	{
		return -1 ;
	}

	if ( tImageGeneratorInitOptions.pfInputFile == NULL ) {
		printf( "fail\n" ) ;
		return 0 ;
	}
	printf( "ok\n" ) ;

	if (ImageGenerator_Initial(&hImageGenerator, &tImageGeneratorInitOptions) != S_OK)
	{
		printf("Initial VolCtrl object fail !!\n");
		return -1;
	}

	if (ImageGenerator_MainMenu( hImageGenerator ) == S_OK ) //user interface
	{
    		//Synthesize the final image
		ImageGenerator_Synthesized( hImageGenerator ) ;
	}

	//close the related files and deconstruct
	ImageGenerator_Release( &hImageGenerator ) ;

	return 0;
}

/* ======================================== */


int main(int argc, char** argv)
{
	int ret = 0 ;

	int bootstorage = 0 ;
	char *storage_type = NULL ;
	char *config_filepath = NULL ;
	char *loader_filepath = NULL ;
	char *preloader_filepath = NULL ;
	char *uboot_filepath = NULL ;
	char *output_filepath = NULL ;

	FILE *config_file = NULL;
	FILE *loader_file = NULL;
	FILE *preloader_file = NULL;
	FILE *uboot_file = NULL;
	FILE *output_file = NULL;

	char next_option ;
	while ((next_option = getopt(argc, argv, "s:iht:c:l:p:u:o:v")) != EOF) {
		switch(next_option) {
			case 'i' :
				printf("Interactive Text GUI menu...\n") ;
				ret = main_guimode() ;
				goto _main_exit ;
			case 'h' :
				print_usage() ;
				goto _main_exit ;
			case 't' :
				storage_type = optarg ;
				break ;
			case 'c' :
				config_filepath = optarg ;
				break ;
			case 'l' :
				loader_filepath = optarg ;
				break ;
			case 'p' :
				preloader_filepath = optarg ;
				break ;
			case 'u' :
				uboot_filepath = optarg ;
				break ;
			case 'o' :
				output_filepath = optarg ;
				break ;
			case 's' :
				sf_bootimage_start_block_number = atoi(optarg) ;
				break ;
			case 'v' :
				printf("%s\n", IMAGEGENERATOR_VERSION);
				ret = 0;
				goto _main_exit ;
			default :
				print_usage() ;
				ret = -1 ;
				goto _main_exit ;
		}
	}

	//Check the validity of option
	//1. boot storage
	//   This is the necessary option because our boot-image layout will be dependent on this.
	if(storage_type == NULL) {
		fprintf(stderr, "\n[ERR] You MUST assign the storage type!!! \n") ;
		print_usage() ;
		ret = -1 ;
		goto _main_exit ;
	}
	else if(!strcmp( storage_type, "sd")) {
		bootstorage = BOOTSTORAGE_SD ;

		if(sf_bootimage_start_block_number != 0) {
			printf("\n[WARNING] You use the option \"s\". This is only valid for SF boot.\n") ;
			printf("          You can only boot from SD block#0, not block#%d.\n", sf_bootimage_start_block_number) ;
			printf("          The boot image will still be generated, but remember to write it to block#0.\n\n") ;
		}
	}
	else if(!strcmp( storage_type, "sf")) {
		bootstorage = BOOTSTORAGE_SF ;
	}
	else if(!strcmp( storage_type, "nf2k")) {
		bootstorage = BOOTSTORAGE_NF2K ;

		if(sf_bootimage_start_block_number != 0) {
			printf("\n[WARNING] You use the option \"s\". This is only valid for SF boot.\n") ;
			printf("          You can only boot from NF block#0, not block#%d.\n", sf_bootimage_start_block_number) ;
			printf("          The boot image will still be generated, but remember to write it to block#0.\n\n") ;
		}
	}
	else {
		fprintf(stderr, "\n[ERR] Wrong boot storage type!\n") ;
		print_usage() ;
		ret = -1 ;
		goto _main_exit ;
	}

	//2. Config file
	if(config_filepath == NULL) {
		//Use default file.
		switch(bootstorage) {
			case BOOTSTORAGE_SD :
				config_filepath = DEFAULT_CONFIG_FILEPATH_SD ;
				break ;
			case BOOTSTORAGE_SF :
				config_filepath = DEFAULT_CONFIG_FILEPATH_SF ;
				break ;
			case BOOTSTORAGE_NF2K :
				config_filepath = DEFAULT_CONFIG_FILEPATH_NF2K ;
				break ;
		}
	}

	//2. loader file
	if(loader_filepath == NULL) {
		//Use default file.
		switch(bootstorage) {
			case BOOTSTORAGE_SD :
				loader_filepath = DEFAULT_LOADER_FILEPATH_SD ;
				break ;
			case BOOTSTORAGE_SF :
				loader_filepath = DEFAULT_LOADER_FILEPATH_SF ;
				break ;
			case BOOTSTORAGE_NF2K :
				loader_filepath = DEFAULT_LOADER_FILEPATH_NF2K ;
				break ;
		}
	}

	//2.1 preloader file for NAND
	if (bootstorage == BOOTSTORAGE_NF2K)
	{
		if (preloader_filepath == NULL)
			preloader_filepath = DEFAULT_PRELOADER_FILEPATH_NF2K ;
	}
	else
		preloader_filepath = NULL;

	//3. Uboot file
	if(uboot_filepath == NULL) {
		//Use default file.
		switch(bootstorage) {
			case BOOTSTORAGE_SD :
				uboot_filepath = DEFAULT_UBOOT_FILEPATH_SD ;
				break ;
			case BOOTSTORAGE_SF :
				uboot_filepath = DEFAULT_UBOOT_FILEPATH_SF ;
				break ;
			case BOOTSTORAGE_NF2K :
				uboot_filepath = DEFAULT_UBOOT_FILEPATH_NF2K ;
				break ;
		}
	}

	//4. Output file
	if(output_filepath == NULL) {
		//Use default file.
		switch(bootstorage) {
			case BOOTSTORAGE_SD :
				output_filepath = DEFAULT_OUTPUT_FILEPATH_SD ;
				break ;
			case BOOTSTORAGE_SF :
				output_filepath = DEFAULT_OUTPUT_FILEPATH_SF ;
				break ;
			case BOOTSTORAGE_NF2K :
				output_filepath = DEFAULT_OUTPUT_FILEPATH_NF2K ;
				break ;
		}
	}

	//Check the files.
	config_file = fopen( config_filepath, "r") ;
	if(config_file == NULL) {
		fprintf(stderr, "[ERR] Cannot open the config file : %s\n", config_filepath) ;
		ret = -1 ;
		goto _main_exit ;
	}

	loader_file = fopen( loader_filepath, "r") ;
	if(loader_file == NULL) {
		fprintf(stderr, "[ERR] Cannot the open loader file : %s\n", loader_filepath) ;
		ret = -1 ;
		goto _main_exit ;
	}
	if (bootstorage == BOOTSTORAGE_NF2K)
	{
		preloader_file = fopen(preloader_filepath, "r") ;
		if(preloader_file == NULL) {
			fprintf(stderr, "[ERR] Cannot the open preloader file : %s\n", preloader_filepath) ;
			ret = -1 ;
			goto _main_exit ;
		}
	}

	uboot_file = fopen( uboot_filepath, "r") ;
	if(uboot_file == NULL) {
		fprintf(stderr, "[ERR] Cannot open the uboot file : %s\n", uboot_filepath) ;
		ret = -1 ;
		goto _main_exit ;
	}

	output_file = fopen( output_filepath, "wb") ;
	if(output_file == NULL) {
		fprintf(stderr, "[ERR] Cannot open the output file : %s\n", output_filepath) ;
		ret = -1 ;
		goto _main_exit ;
	}

	//Generate the boot image file.
	switch(bootstorage) {
		case BOOTSTORAGE_SD :
				ret = generate_bootimg_sd(config_file, loader_file, uboot_file, output_file) ;
				break ;
		case BOOTSTORAGE_SF :
				ret = generate_bootimg_sf(config_file, loader_file, uboot_file, output_file) ;
				break ;
		case BOOTSTORAGE_NF2K :
				ret = generate_bootimg_nf2k(config_file, loader_file, preloader_file, uboot_file, output_file) ;
				break ;
	}

	if(ret == -1)
		printf("It is failed.\n") ;
	else
		printf("Done.\n\n") ;

_main_exit :
#define _FCLOSE(fp)   if (fp) fclose((fp));
	_FCLOSE(config_file) ;
	_FCLOSE(loader_file) ;
	_FCLOSE(uboot_file) ;
	_FCLOSE(output_file) ;

	return ret ;
}
