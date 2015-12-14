/*
 *******************************************************************************
 * $Header: /rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRComm_Lib/vivo_codec.h 2     05/07/14 9:07p Yancheng $
 *
 * $History: vivo_codec.h $
 * 
 * *****************  Version 2  *****************
 * User: Yancheng     Date: 05/07/14   Time: 9:07p
 * Updated in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRComm_Lib
 * Convert calling convention from __stdcall to __cdecl .
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/03/17   Time: 4:19p
 * Created in $/rd_2/speech/GAMR/ARM/GAMR_ARM9/GAMRCOMM_LIB
 * Optimized on ARM9
 * 
 * *****************  Version 1  *****************
 * User: Aren         Date: 03/02/19   Time: 8:43p
 * Created in $/rd_2/speech/GAMR/C/GAMR_PC/GAMRCOMM_LIB
 * New Version
 * 
 * *****************  Version 10  *****************
 * User: Sor          Date: 03/02/14   Time: 3:39p
 * Updated in $/rd_2/image/JPEG/C/Encoder/JPEGEnc/JPEGEnc_Lib
 * MODIFICATION: Modify DLLAPI definition - DONE.
 * 
 * *****************  Version 9  *****************
 * User: Sor          Date: 03/02/14   Time: 3:25p
 * Updated in $/rd_2/image/JPEG/C/Encoder/JPEGEnc/JPEGEnc_Lib
 * MODIFICATION: Add DLLAPI definition - DONE.
 * 
 * *****************  Version 8  *****************
 * User: Sor          Date: 03/02/14   Time: 9:55a
 * Updated in $/rd_2/image/JPEG/C/Encoder/JPEGEnc/JPEGEnc_Lib
 * FEATURE: Add new video option flag - DONE.
 * 
 * *****************  Version 7  *****************
 * User: Sor          Date: 03/01/22   Time: 4:50p
 * Updated in $/rd_2/video/MPEG4/C/Decoder/MP4VDEC/Mp4VDec_Lib
 * MODIFICATION: Add VIDEO_CHANGE_PIXEL_FORMAT and VIDEO_SET_DEBLOCKING in
 * TVideoOptionFlags - DONE.
 * 
 * *****************  Version 6  *****************
 * User: Sor          Date: 03/01/20   Time: 3:58p
 * Updated in $/rd_2/video/MPEG4/TM1300/Encoder/Mp4VEnc/Mp4VEnc_Lib
 * REFINEMENT: Change TMp4VEncOptionFlags to TVideoOptionFlags and move it
 * to vivo_codec.h - DONE.
 * REFINEMENT: Remove TMp4VEncProfileLevel field in TMp4VEncInitOptions -
 * DONE.
 * 
 * *****************  Version 5  *****************
 * User: Sor          Date: 03/01/15   Time: 1:56p
 * Updated in $/rd_2/video/MPEG4/C/Decoder/MP4VDEC/Mp4VDec_Lib
 * Change to fit the VNDP requirement.
 * 
 * *****************  Version 4  *****************
 * User: Sor          Date: 02/12/02   Time: 11:08a
 * Updated in $/rd_2/video/MPEG4/C/Decoder/MP4VDEC/Mp4VDec_Lib
 * Added output pixel format enumerator for video decoders.
 * 
 * *****************  Version 3  *****************
 * User: Sor          Date: 02/11/29   Time: 8:23a
 * Updated in $/rd_2/misc/CODINGRULE/codec_proto/comm_lib
 * Add GB and PB engine buffer type enumeration.
 * 
 * *****************  Version 2  *****************
 * User: Vicky        Date: 02/10/28   Time: 12:01p
 * Updated in $/rd_2/misc/CODINGRULE/codec_proto/comm_lib
 * remove "\" for complier error on linux OS
 * 
 * *****************  Version 1  *****************
 * User: Sor          Date: 02/07/23   Time: 1:46p
 * Created in $/rd_2/misc/CODINGRULE/codec_proto/comm_lib
 * 
 * *****************  Version 3  *****************
 * User: Joe          Date: 02/04/26   Time: 4:16p
 * Updated in $/rd_2/misc/CODINGRULE/codec_proto/comm_lib
 * update 80 word wrapping
 *
 * *****************  Version 2  *****************
 * User: Joe          Date: 02/04/25   Time: 4:58p
 * Updated in $/rd_2/misc/CODINGRULE/codec_proto/comm_lib
 * Update callback function definitions
 *
 * *****************  Version 1  *****************
 * User: Joe          Date: 02/03/06   Time: 10:25a
 * Created in $/rd_2/misc/CODINGRULE/CODEC_PROTO/comm_lib
 * codec prototype
 *
 */

/*!
 *******************************************************************************
 *
 * Copyright 2000-2002 VN , Inc. All rights reserved.
 *
 * \file
 * vivo_codec.h
 *
 * \brief
 * VN 's Audio/Video codec common callback function definitions
 *
 * \date
 * 2002/04/25
 *
 * \author
 * Joe Wu
 *
 **************************************************************************** */

#ifndef _VIVO_CODEC_H_
#define _VIVO_CODEC_H_

#include "typedef.h"
#include "errordef.h"

/*! Make from four character codes to one 32-bits DWORD */
#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

/*! Use for generate dynamic link library for Windows */
#ifndef DLLAPI
#ifdef _WIN32
  #ifdef _USRDLL
    #define DLLAPI __declspec(dllexport) __stdcall
  #else
    #define DLLAPI __cdecl
  #endif //defined(_USRDLL)
#else
  #define DLLAPI
#endif //defined(_WIN32)
#endif //defined(DLLAPI)

/*!
 *******************************************************************************
 * \brief
 * This callback function provides the codec to enter an OS dependent
 * critical section.
 *
 * \param dwSemaphore
 * The custom data for this callback function. It may be the address of the
 * semaphore or any other data you want to passed to the callback function
 * when enter the critical section.
 *
 * \return
 * NONE
 *
 * \note
 * This callback function will be called whenever the codec need to enter
 * critical section.
 *
 **************************************************************************** */
typedef void (* TOnEnterCriticalSec)(DWORD dwSemaphore);

/*!
 *******************************************************************************
 * \brief
 * This callback function provides the codec to leave an OS dependent
 * critical section.
 *
 * \param dwSemaphore
 * The custom data for this callback function. It may be the address of the
 * semaphore or any other data you want to passed to the callback function when
 * enter the critical section.
 *
 * \return
 * NONE
 *
 * \note
 * This callback function will be called whenever the codec need to leave
 * critical section. The semaphore are the same as that in TOnEnterCriticalSec.
 *
 **************************************************************************** */
typedef void (* TOnLeaveCriticalSec)(DWORD dwSemaphore);

/*!
 *******************************************************************************
 * \brief
 * This callback function is called when the encoder sends data from the
 * internal bitstream buffer and the bitstream buffer is full.
 *
 * \param dwUserData
 * The custom data for this callback function. It may be the address of a
 * structure or the delay time the encoder sends data from the internal buffer
 * again.
 *
 * \retval S_OK
 * The data is sent ok
 * \retval S_FAIL
 * The data is unable to process, and the encoder's task will return immediately
 * with return value - \a S_FAIL.
 *
 * \note
 * This callback function has two different usages. In single thread mode, if
 * you set this callback function, it will be called when the internal bitstream
 * buffer is full. You can call the \b AVEnc_TakeData function to retrieve the
 * bitstream data from internal buffer. In multithread mode, you can just call
 * the \b Sleep function, or any system call to let OS switch to another thread.
 *
 **************************************************************************** */
typedef SCODE (* TOnSendData)(DWORD dwUserData);

/*!
 *******************************************************************************
 * \brief
 * This callback function is called when the decoder acquires the data from the
 * internal bitstream buffer and the bitstream buffer is empty.
 *
 * \param dwUserData
 * The custom data for this callback function. It may be the address of a
 * structure or the delay time the decoder acquires the data from the internal
 * buffer again.
 *
 * \retval S_OK
 * The data is sent ok
 * \retval S_FAIL
 * The data is unable to process, and the decoder's task will return immediately
 * with return value - \a S_FAIL.
 *
 * \note
 * This callback function has two different usages. In single thread mode, if
 * you set this callback function, it will be called when the internal bitstream
 * buffer is empty. You can call the \b AVDec_FeedData function to put the
 * bitstream data to the internal buffer. In multithread mode, you can just call
 * the \b Sleep function, or any system call to let OS switch to another thread.
 *
 **************************************************************************** */
typedef SCODE (* TOnRequestData)(DWORD dwUserData);

/*! FOURCC codec for H263 codec codec */
#define FOURCC_H263 (MAKEFOURCC('H','2','6','3'))
/*! FOURCC codec for MPEG-4 video codec */
#define FOURCC_MP4V (MAKEFOURCC('M','P','4','V'))
/*! FOURCC for MPEG-2 video codec */
#define FOURCC_MP2V (MAKEFOURCC('M','P','2','V'))

/*! FOURCC for MPEG-1 layer 3 audio codec */
#define FOURCC_MP3A (MAKEFOURCC('M','P','3','A'))
/*! FOURCC for MPEG-2 AAC audio codec */
#define FOURCC_AAC2 (MAKEFOURCC('A','A','C','2'))
/*! FOURCC for MPEG-4 AAC audio codec */
#define FOURCC_AAC4 (MAKEFOURCC('A','A','C','4'))

/*! FOURCC for GSM-AMR speech codec */
#define FOURCC_GAMR (MAKEFOURCC('G','A','M','R'))
/*! FOURCC for G.711 codec */
#define FOURCC_G711 (MAKEFOURCC('G','7','1','1'))
/*! FOURCC for G.722 speech codec */
#define FOURCC_G722 (MAKEFOURCC('G','7','2','2'))
/*! FOURCC for G.722.1 speech codec */
#define FOURCC_G721 (MAKEFOURCC('G','7','2','1'))
/*! FOURCC for G.723.1 speech codec */
#define FOURCC_G723 (MAKEFOURCC('G','7','2','3'))
/*! FOURCC for G.728 speech codec */
#define FOURCC_G728 (MAKEFOURCC('G','7','2','8'))
/*! FOURCC for G.729 speech codec */
#define FOURCC_G729 (MAKEFOURCC('G','7','2','9'))

/*! GB and PB engine buffer type enumeration */
typedef enum buffer_type_flags
{
	JPEGNONCIRCULAR = 1,
	JPEGCIRCULAR = 2,
	MPEGNONCIRCULAR = 3,
	MPEGCIRCULAR = 4,
	H26LNONCIRCULAR = 5,
	H26LCIRCULAR  =6
} TBufTypeFlags;

/* ! Video output pixel format enumeration */
typedef enum pixel_format_flags
{
	YUV = 1,
	YUY2 = 2,
	RGB16 = 3,
	RGB24 = 4,
	RGB32 = 5,
	NONE = 6
} TPixelFormatFlags;

/* ! Video and image codec option enumeration */
typedef enum video_option_flags
{ 
	VIDEO_RESTART = 1, 
	VIDEO_CHANGE_FRAME_BUFFER = 2, 
	VIDEO_CHANGE_BITSTREAM_BUFFER = 3,
	VIDEO_FLUSH_BITSTREAM_BUFFER = 4,
	VIDEO_CHANGE_USERDATA_BUFFER = 5,
	VIDEO_FORCE_INTRA = 6,
	VIDEO_CHANGE_QUANT = 7,
	VIDEO_CHANGE_PIXEL_FORMAT = 8,
	VIDEO_SET_DEBLOCKING = 9,
	VIDEO_CHANGE_COLOR = 10
} TVideoOptionFlags;

#endif // codec
