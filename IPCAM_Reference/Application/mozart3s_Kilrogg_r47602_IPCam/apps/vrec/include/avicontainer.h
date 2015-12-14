/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2007-2010 VN Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 *
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2007-2010 VN, Inc. All rights reserved.
 *
 * \file
 * avicontainer.h
 *
 * \brief
 * AVI media file format recording library
 *
 * \date
 * 2010/10/21
 *
 * \author
 * Acer Tsai
 *
 *
 *******************************************************************************
 */ 
 
#ifndef _AVICONTAINER_H_
#define _AVICONTAINER_H_

#include <string.h>

#include "global_codec.h"
#include "ubuffer.h"
#include "typedef.h"

typedef DWORD FOURCC;

#if 0
// --------------------- function brief ----------------------------------------
SCODE AVIC_Initial(HANDLE *phObject, TAVICInitOptions *pInitOpts);
SCODE AVIC_Release(HANDLE *phObject);
SCODE AVIC_CreateFile(HANDLE hObject, TAVICCreateOptions *ptOpts);
SCODE AVIC_AddSample(HANDLE hObject, TSrcUBufInfo *ptSrcUBInfo);
SCODE AVIC_CloseFile(HANDLE hObject);
SCODE AVIC_HasAudioSample(HANDLE hObject, BOOL *bHasSample);
SCODE AVIC_HasVideoSample(HANDLE hObject, BOOL *bHasSample);
#endif

/*! FOUR_CC Version code of your \b AVIC instance. */
#define AVIC_VERSION   MAKEFOURCC(1, 1, 0, 0)

/*! zero track in the movie */
#define ERR_ZERO_TRACK				0x80060001
/*! Track type is not defined */
#define ERR_EATT_TYPE				0x80060002
/*! the file size exceed the maximum file size 
  (avic_initial_options::dwMaxSize).  this sample can't be add. */
#define	ERR_EXCEED_FILE_SIZE		0x80060003
/*! open exist file in AVIC_CreateFile() */
#define ERR_OPEN_EXIST_FILE			0x80060004
/*! open file fail in AVIC_CreateFile() */
#define ERR_OPEN_FILE_FAIL			0x80060007
/*! The total number of VideoTrack and AudioTrack exceed MAX supported track number in AVIC_CreateFile() */
#define	ERR_EXCEED_MAX_TRACK_NUM	0x80060008
/*! no match track found in AVIC_AddSample() */
#define ERR_NO_MATCH_TRACK			0x80060009
/*! sample with zero size in AVIC_AddSample() */
#define ERR_INVALID_SAMPLE			0x8006000A
/*! file already closed when AVIC_CloseFile() want to write it*/
#define ERR_FILE_CLOSED				0x8006000B
/*! no file name */
#define ERR_NO_FILENAME				0x8006000D
/*! write to file fail */
#define ERR_WRITE_TO_FILE_FAIL		0x80060005
/*! zero sample in track */
#define ERR_ZERO_SAMPLE				0x80060006
/*! write header failed */
#define ERR_WRITE_HRADER			0x8006000C

/*! AVIC initial options */
typedef struct avic_initial_options
{
	/*! This is a version control parameter. 
	    Set this value as \b AVIC_VERSION.*/
	DWORD			dwVersion;
} TAVICInitOptions;

typedef struct src_ubuf_info
{
	BYTE	*pbyUBuffHdr;
	int		iUBuffHdrLen;
	BYTE	*pbyUBuffPayload;
	int		iUBuffPayloadLen;
} TSrcUBufInfo;

typedef struct avic_create_options
{
	BOOL			bCheckMaxFileSize;
	/*! The maximum size of the media file in byte */
	DWORD			dwMaxFileSize;
	/*!	file name */
	char            *szAVIFile;
	/*! The total video number of track */
	DWORD 			dwVideoTrackNum;
	/*! The video source ubuffer info array */
	TSrcUBufInfo	*ptVideoTrackUBufInfo;
	/*! The total audio number of track */
	DWORD 			dwAudioTrackNum;
	/*! The audio source ubuffer info array */
	TSrcUBufInfo	*ptAudioTrackUBufInfo;
} TAVICCreateOptions;

#ifdef __cplusplus
extern "C" {
#endif

/*!
******************************************************************************
* \brief
* Create handle of AVIC object
*
* \param phObject
* \a (o) pointer to receive the handle of the AVIC object
*
* \param pInitOpts
* \some initial option must be set by user when initialization.
*
* \retval S_OK
* Create object ok
*
* \retval S_FAIL
* Create object failed
*
* \retval ERR_INVALID_VERSION
* Invalid version number
*
* \retval ERR_OUT_OF_MEMORY
* Memory allocate fail
*
* \remark
* A AVIC object shall be initialized before using it.
*
* \see AVIC_Release
*
******************************************************************************
*/
SCODE AVIC_Initial(HANDLE *phObject, TAVICInitOptions *pInitOpts);

/*!
******************************************************************************
* \brief
* Release an AVIC object
*
* \param phObject
* \a (i/o) pointer to the handle of the AVIC object
*
* \retval S_OK
* Release object ok
*
* \remark
* After release the oject, the *phObject will be set to NULL
*
* \see AVIC_Initial
*
******************************************************************************
*/
SCODE AVIC_Release(HANDLE *phObject);

/*!
******************************************************************************
* \brief
* Create a avi file
*
* \param hObject
* Handle of the AVIC object
*
* \param ptOpts
* Pointer of TAVICCreateOptions structure
*
* \retval S_OK
* Create new track ok
*
* \retval S_FAIL
* dwDataType is not FOURCC_CONF or TrackNum is over MAX_TRACK_NUM
*
* \retval ERR_OUT_OF_MEMORY
* Memory allocate fail
*
* \see AVIC_AddSample
*
******************************************************************************
*/

SCODE AVIC_CreateFile(HANDLE hObject, TAVICCreateOptions *ptOpts);

/*!
******************************************************************************
* \brief
* Add a new media sample
*
* \param hObject
* Handle of the AVIC object
*
* \param ptSrcUBInfo
* Pointer of TSrcUBufInfo structure, it should be with media data, not CONF.
*
* \retval S_OK
* add sample successfully
*
* \retval ERR_EXCEED_FILE_SIZE
* the file size exceed the maximum file size (AVIC_initial_options::dwMaxSize). 
* this sample can't be add.
*
* \retval ERR_OUT_OF_MEMORY
* Memory allocate fail
*
* \retval ERR_INVALID_SAMPLE
* sample with zero size
*
* \retval ERR_NO_MATCH_TRACK
* no match track found in AVIC_AddSample()
*
* \remark
* Be sure to call AVIC_NewTrack() before calling this, or you'll lose the data.
*
* \see AVIC_NewTrack
*
******************************************************************************
*/

SCODE AVIC_AddSample(HANDLE hObject, TSrcUBufInfo *ptSrcUBInfo);

/*!
******************************************************************************
* \brief
* Write the media file and close it
*
* \param hObject
* Handle of the AVIC object
*
* \retval S_OK
* Write to file successfully
*
* \retval S_FAIL
* Write to file failed
*
* \retval ERR_ZERO_TRACK
* no track in the movie
* 
* \retval ERR_ZERO_SAMPLE
* no sample in the movie 
*
* \retval ERR_FILE_CLOSED
* file already closed 
*
* \retval ERR_WRITE_TO_FILE_FAIL
* write to file fail
*
* \retval ERR_EATT_TYPE
* Track type is not defined 
*
******************************************************************************
*/

SCODE AVIC_CloseFile(HANDLE hObject, unsigned int video_duration_ms);

/*!
******************************************************************************
* \brief
* Query this file has audio sample or not
*
* \param hObject
* Handle of the AVIC object
*
* \param bHasSample
* TRUE: has audio sample
* FALSE: no audio sample
*
* \retval S_OK
* It always return S_OK
*
******************************************************************************
*/
SCODE AVIC_HasAudioSample(HANDLE hObject, BOOL *bHasSample);

/*!
******************************************************************************
* \brief
* Query this file has video sample or not
*
* \param hObject
* Handle of the AVIC object
*
* \param bHasSample
* TRUE: has video sample
* FALSE: no video sample
*
* \retval S_OK
* It always return S_OK
*
******************************************************************************
*/
SCODE AVIC_HasVideoSample(HANDLE hObject, BOOL *bHasSample);

SCODE AVIC_FlushCache(HANDLE hObject);
SCODE AVIC_CommitData(HANDLE hObject);

#ifdef __cplusplus
}
#endif

#endif // _AVICONTAINER_H_
