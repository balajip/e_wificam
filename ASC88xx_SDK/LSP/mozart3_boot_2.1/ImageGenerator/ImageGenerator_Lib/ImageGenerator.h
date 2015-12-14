/*
 * Copyright 2001-2004 VN, Inc.  All rights reserved.
 *
 * Description:
 *
 */

/* ============================================================================================== */
#ifndef __IMAGEGENERATOR_H__
#define __IMAGEGENERATOR_H__

/* ============================================================================================== */
#include "typedef.h"
#include "errordef.h"
#include "vivo_codec.h"

#include "stdio.h"
//#include "sys/stat.h"
//
#define IMAGEGENERATOR_VERSION "2.0.0.2"

#define SD_DEFAULT_CONFIG_FILE "SD/Mozart_SD.config"
#define SF_DEFAULT_CONFIG_FILE "SF/Mozart_SF.config"
#define NAND2KB_DEFAULT_CONFIG_FILE "NF2K/Mozart_NF2K.config"
#define NAND4KB_DEFAULT_CONFIG_FILE "NF4K/Mozart_NF4K.config"

#define SD_DEFAULT_OUTPUTFILE "SD/SD_Output.bin"
#define SF_DEFAULT_OUTPUTFILE "SF/SF_Output.bin"
#define NAND2KB_DEFAULT_OUTPUTFILE "NF2K/NF2K_Output.bin"
#define NAND4KB_DEFAULT_OUTPUTFILE "NF4K/NF4K_Output.bin"

#define SD_DEFAULT_LOADER "SD/SD_Loader.bin"
#define SF_DEFAULT_LOADER "SF/SF_Loader.bin"
#define NAND2KB_DEFAULT_LOADER "NF2K/NF2K_Loader.bin"
#define NAND4KB_DEFAULT_LOADER "NF4K/NF4K_Loader.bin"

#define SD_DEFAULT_UBOOT "SD/SD_Uboot.bin"
#define SF_DEFAULT_UBOOT "SF/SF_Uboot.bin"
#define NAND2KB_DEFAULT_UBOOT "NF2K/NF2K_Uboot.bin"
#define NAND4KB_DEFAULT_UBOOT "NF4K/NF4K_Uboot.bin"

//NF Preloader
#define NAND2KB_PRELOADER_DEFAULT_BASE 0xA0
#define NAND2KB_PERLOADER_FILENAME "NF2K/NF2K_Preloader.bin"

#define STORAGE_SD 0
#define STORAGE_SF 1
#define STORAGE_NAND2KB 2
#define STORAGE_NAND4KB 3

#define SD_LOADER_DEFAULT_BASE 0x200
#define SF_LOADER_DEFAULT_BASE 0x200
#define NAND2KB_LOADER_DEFAULT_BASE 0x200
#define NAND4KB_LOADER_DEFAULT_BASE 0x200

#define SD_UBOOT_DEFAULT_BASE 0x1000
#define SF_UBOOT_DEFAULT_BASE 0x1000
#define NAND2KB_UBOOT_DEFAULT_BASE 0x1000
#define NAND4KB_UBOOT_DEFAULT_BASE 0x1000

#define IMAGE_SIZE_64K (64*1024)
#define IMAGE_SIZE_128K (128*1024)
#define IMAGE_TOTAL_SIZE IMAGE_SIZE_128K
#define SD_SECTOR_SIZE  512
#define NF2KB_BLOCKSIZE (64*2048)
#define SF_BLOCK_SIZE   0x10000

#define DEFAULT_L1BOOT_ADDRESS 0x300
#define DEFAULT_UBOOT_ADDRESS 0x1000

#define SECTOR_SIZE IMAGE_SIZE_64K

#define UNKNOWN_VALUE 0xffffffff

#define CONFIG_KEYWORD "LOADERBASE"

extern int storage_choice;

/* ------------------------------------------------------------------------ */
/*!
 *  A data structure of \b TConfigData object.
 */
typedef struct TConfigData
{
	/*! Data Name */
	CHAR acName[25] ;
	/*! Data Address */
	DWORD dwAddress ;
	/*! Data Content */
	DWORD dwContent ;
	/*! Next Data */
	struct TConfigData *ptNextData ;
} TConfigData ;

/* ------------------------------------------------------------------------ */
/*!
 *  A data structure of \b TConfigDataList object.
 */
typedef struct
{
	/*! Configured Data List Size */
	UINT iListSize ;
	/*! Config Data List Head */
	TConfigData *ptListHead ;
	/*! Config Data List End */
	TConfigData *ptListEnd ;

} TConfigDataList ;

/* ------------------------------------------------------------------------ */
/*!
 *  A data structure of \b TImageHeader object.
 */
typedef struct TImageHeader
{
	/*! Header Name */
	CHAR acName[25] ;
	/*! Header Value */
	DWORD dwValue ;
	/*! Header Data List */
	TConfigDataList	*ptList ;
	/*! Next Header */
	struct TImageHeader* ptNextHeader ;
} TImageHeader ;

/* ------------------------------------------------------------------------ */
/*!
 *  A data structure of \b TImageGeneratorInfo object.
 */
 typedef struct
{
	/*! Header Head */
	TImageHeader *ptHeadListStart ;
	/*! Header End */
	TImageHeader *ptHeadListEnd ;
	/*! Header Number */
	UINT uiHeadNum ;
	/*! Input Config */
	FILE *fInputFile ;
	/*! Output Image */
	FILE *fOutputFile ;
	/*! L1-Image */
	FILE *fL1Image ;
	/*! ARMBoot-Image */
	FILE *fUBootImage ;
	/*! 64K image */
	BYTE *bImage;
	//BYTE bImage[IMAGE_TOTAL_SIZE] ;
	/*! addr of L1Boot */
	DWORD dwL1BootAddr ;
	/*! addr of ARMBoot */
	DWORD dwUBootAddr ;
	/*! sector number */
	UINT uiSector ;
} TImageGeneratorInfo;

/* ------------------------------------------------------------------------ */
/*!
 *  A data structure of \b TImageGeneratorInitOptions object. Used in function \b AudioCtrl_Initial to
 *  set initial options.
 */
typedef struct
{
	/*! Input File */
	PCHAR pcInputName ;
	FILE *pfInputFile ;
	/*! OutputImage */
	PCHAR pcOutputName ;
	FILE *pfOutputFile ;

} TImageGeneratorInitOptions;

/* ============================================================================================== */
/*!
 *******************************************************************************
 * \brief
 * Create a \b TImageGeneratorInfo instance and initialize it.
 *
 * \param phObject
 * \a (o) the pointer to receive the handle of this \b TImageGeneratorInfo instance.
 *
 * \param pInitOptions
 * \a (i/o) the pointer of data structure \b TImageGeneratorInitOptions for setting the
 * initialization parameters of the \b TImageGeneratorInfo instance. If you did not set some
 * parameters which are needed, the default value will be set and write back to
 * the corresponding fields.
 *
 * \retval S_OK
 * Create and initialize this instance successfully.
 * \retval S_FAIL
 * Create or initialize this instance failed.
 *
 * \note
 * Make sure the initialization options in the \b TImageGeneratorInitOptions structure
 * are set properly. This function must be called before any other functions of
 * the \b TImageGeneratorInfo.
 *
 * \see TAudioCtrlInitOptions
 * \see TImageGeneratorInfo
 *
 **************************************************************************** */
SCODE ImageGenerator_Initial(HANDLE *phObject, TImageGeneratorInitOptions *ptInitOptions);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Delete an instance of the \b TImageGeneratorInfo object.
 *
 * \param phObject
 * \a (i/o) the pointer to the handle of the \b TImageGeneratorInfo instance.
 *
 * \retval S_OK
 * Release the instance successfully.
 * \retval S_FAIL
 * Release the instance failed.
 *
 * \note
 * After the instance is released, the handle of this instance will be set to
 * zero.
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_Release(HANDLE *phObject);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Insert a \b TConfigData into the \b TConfigDataList.
 *
 * \param ptList
 * \a (i/o) the pointer to the \b TConfigDataList instance.
 *
 * \param ptNewData
 * \a (i) the pointer to the \b TConfigData instance.
 *
 * \param iIndex
 * \a (i) the index of list that we hope to place the new data.
 *
 * \retval S_OK
 * Insert the instance successfully.
 * \retval S_FAIL
 * Insert the instance failed.
 *
 * \note
 * the index should be in the reasonable range.
 *
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ListInsert( TConfigDataList *ptList, TConfigData *ptNewData, UINT iIndex);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * find the \b TConfigData from the \b TConfigDataList.
 *
 * \param ptList
 * \a (i) the pointer to the \b TConfigDataList instance.
 *
 * \param iTarget
 * \a (i) the index of instance \b TConfigData that we want to get.
 *
 * \param ptAnswer
 * \a (o) the pointer to the pointer of target \b TConfigData instance.
 *
 * \retval S_OK
 * Find the instance successfully.
 * \retval S_FAIL
 * Find the instance failed.
 *
 * \note
 * The index should be in the reasonable range. Notice that usage of the parameter should be correct type.
 *
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ListSearch( TConfigDataList *ptList, UINT iTarget, TConfigData **ptAnswer);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Remove the \b TConfigData from the \b TConfigDataList.
 *
 * \param ptList
 * \a (i) the pointer to the \b TConfigDataList instance.
 *
 * \param iTarget
 * \a (i) the index of data that we want to remove.
 *
 * \retval S_OK
 * Remove the instance successfully.
 * \retval S_FAIL
 * Remove the instance failed.
 *
 * \note
 * The index should be in the reasonable range.
 *
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ListRemove( TConfigDataList *ptList, UINT iTarget);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Print all \b TConfigData informations of all \b TImageHeader instances.
 *
 * \param HANDLE
 * \a (i) the handle to the \b TImageGeneratorInfo instance.
 *
 * \retval S_OK
 * Print the instances successfully.
 * \retval S_FAIL
 * Print the instances failed.
 *
 * \note
 * We should use the HANDLE as parameter.
 *
 * \see TImageHeader
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ListPrint(HANDLE hObject) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Insert a \b TImageHeader into the list of \b TImageGeneratorInfo .
 *
 * \param hObject
 * \a (i/o) the handle to the \b TImageGeneratorInfo instance.
 *
 * \param ptHeader
 * \a (i) the pointer to the \b TImageHeader instance.
 *
 * \param iIndex
 * \a (i) the index of list that we hope to place the new header.
 *
 * \retval S_OK
 * Insert the instance successfully.
 * \retval S_FAIL
 * Insert the instance failed.
 *
 * \note
 * the index should be in the reasonable range.
 *
 * \see TImageHeader
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_HeadInsert(HANDLE hObject, TImageHeader *ptHeader, UINT iIndex);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * find the \b TImageHeader from the list of \b TImageGeneratorInfo.
 *
 * \param ptList
 * \a (i) the handle to the \b TImageGeneratorInfo. instance.
 *
 * \param iTarget
 * \a (i) the index of instance \b TImageHeader that we want to get.
 *
 * \param ptHeader
 * \a (o) the pointer to the pointer of target \b TImageHeader instance.
 *
 * \retval S_OK
 * Find the instance successfully.
 * \retval S_FAIL
 * Find the instance failed.
 *
 * \note
 * The index should be in the reasonable range. Notice that usage of the parameter should be correct type.
 *
 * \see TImageGeneratorInfo
 * \see TImageHeader
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_HeadSearch(HANDLE hObject, UINT iTarget, TImageHeader **ptHeader) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Remove the \b TImageHeader from the \b hObject.
 *
 * \param hObject
 * \a (i) the HANDLE to the \b TImageGeneratorInfo instance.
 *
 * \param iTarget
 * \a (i) the index of data that we want to remove.
 *
 * \retval S_OK
 * Remove the instance successfully.
 * \retval S_FAIL
 * Remove the instance failed.
 *
 * \note
 * The index should be in the reasonable range.
 *
 * \see TImageGeneratorInfo
 * \see TConfigDataList
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_HeadRemove(HANDLE hObject, UINT iTarget) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the name of \b TConfigData instance
 *
 * \param ptData
 * \a (i) the pointer to \b TConfigData instance.
 *
 * \param acNewName[20]
 * \a (i) the new name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyDataName( TConfigData *ptData, CHAR acNewName[20]) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the address of \b TConfigData instance
 *
 * \param ptData
 * \a (i) the pointer to \b TConfigData instance.
 *
 * \param dwAddress
 * \a (i) the new addressthat we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyDataAddress( TConfigData *ptData, DWORD dwAddress);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TConfigData instance
 *
 * \param ptData
 * \a (i) the pointer to \b TConfigData instance.
 *
 * \param dwContent
 * \a (i) the new content that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TConfigData
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyDataContent( TConfigData *ptData, DWORD dwContent);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the name of \b TImageHeader instance
 *
 * \param ptData
 * \a (i) the pointer to \b TImageHeader instance.
 *
 * \param acNewName[20]
 * \a (i) the new name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageHeader
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyHeadName( TImageHeader *ptHead, CHAR acNewName[20]) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageHeader instance
 *
 * \param ptData
 * \a (i) the pointer to \b TImageHeader instance.
 *
 * \param dwContent
 * \a (i) the new content that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageHeader
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyHeadContent( TImageHeader *ptHead, DWORD dwContent);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param pcFileName
 * \a (i) the new file name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyInputFile(HANDLE hObject, PCHAR pcFileName);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param pcFileName
 * \a (i) the new file name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyOutputFile(HANDLE hObject, PCHAR pcFileName);

/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param pcFileName
 * \a (i) the new file name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyInputFilePtr(HANDLE hObject, FILE *pfFilePtr);

/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param pcFileName
 * \a (i) the new file name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyOutputFilePtr(HANDLE hObject, FILE *pfFilePtr);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param pcFileName
 * \a (i) the new file name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyL1Image(HANDLE hObject, PCHAR pcFileName);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the content of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param pcFileName
 * \a (i) the new file name that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyARMBootImage(HANDLE hObject, PCHAR pcFileName);

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the ARMBoot Address of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param dwAddr
 * \a (i) the new address that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyUBootAddr( HANDLE hObject, DWORD dwAddr ) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the L1Boot Address of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param dwAddr
 * \a (i) the new address that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifyL1BootAddr( HANDLE hObject, DWORD dwAddr ) ;
/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Modify the sector number of \b TImageGeneratorInfo instance
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \param uiSector
 * \a (i) the new sector number that we want to assign.
 *
 * \retval S_OK
 * Modify the instance successfully.
 * \retval S_FAIL
 * Modify the instance failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_ModifySector( HANDLE hObject, UINT uiSector) ;

/* ------------------------------------------------------------------------ */
/*!
 *******************************************************************************
 * \brief
 * Generate the final image.
 *
 * \param hObject
 * \a (i) the handle to \b hObject instance.
 *
 * \retval S_OK
 * Generate the image successfully.
 * \retval S_FAIL
 * Generate the image failed.
 *
 * \note
 *
 * \see TImageGeneratorInfo
 *
 ****************************************************************************  */
SCODE ImageGenerator_Synthesized(HANDLE hObject);

/* ============================================================================================== */

#endif //__IMAGEGENERATOR_H__
/* ============================================================================================== */
