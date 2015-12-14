/*

/*!
 *******************************************************************************
 *
 * Copyright 2008 VN Inc. All rights reserved.
 *
 * \file
 * vivo_codec.h
 *
 * \brief
 * VN's Audio/Video codec common callback function definitions
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
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)  ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

/*! Use for generate dynamic link library for Windows */
#ifndef DLLAPI
#ifdef _WIN32
  #ifdef _USRDLL
    #define DLLAPI __declspec(dllexport) __stdcall
  #else
    #define DLLAPI __stdcall
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
#if defined(__TCS__)
typedef void (* FOnEnterCriticalSec)(DWORD dwSemaphore);
#else
typedef void (* FOnEnterCriticalSec)(PVOID pvSemaphore);
#endif
#define TOnEnterCriticalSec FOnEnterCriticalSec

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
#if defined(__TCS__)
typedef void (* FOnLeaveCriticalSec)(DWORD dwSemaphore);
#else
typedef void (* FOnLeaveCriticalSec)(PVOID pvSemaphore);
#endif
#define TOnLeaveCriticalSec FOnLeaveCriticalSec

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
#if defined(__TCS__)
typedef SCODE (* FOnSendData)(DWORD dwUserData);
#else
typedef SCODE (* FOnSendData)(PVOID pvUserData);
#endif
#define TOnSendData FOnSendData

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
#if defined(__TCS__)
typedef SCODE (* FOnRequestData)(DWORD dwUserData);
#else
typedef SCODE (* FOnRequestData)(PVOID pvUserData);
#endif
#define TOnRequestData FOnRequestData

/* ====================================================================================== */
#if defined(__x86_64__) || defined(__IA64__) || defined(_M_IA64) || defined (_M_X64) || defined(_WIN64)
#define ALIGN_ADDRESS(a) ((BYTE *)a+ALIGN_PADDING-(((DWORD)(QWORD)a+ALIGN_PADDING)&(~ALIGN_MASK)));
#else //TriMedia or 32-bit PC
#define ALIGN_ADDRESS(a) ((BYTE *)a+ALIGN_PADDING-(((DWORD)a+ALIGN_PADDING)&(~ALIGN_MASK)));
#endif
/* ====================================================================================== */
typedef enum mem_type
{
    MEM_TYPE_CACHED = 0,
    MEM_TYPE_NONCACHED = 1
} EMemType;

typedef enum align_type
{
    ALIGN_TYPE_1_BYTE = 0,
    ALIGN_TYPE_2_BYTE = 1,
    ALIGN_TYPE_4_BYTE = 2,
    ALIGN_TYPE_8_BYTE = 3,
    ALIGN_TYPE_16_BYTE = 4,
    ALIGN_TYPE_32_BYTE = 5,
    ALIGN_TYPE_64_BYTE = 6,
    ALIGN_TYPE_128_BYTE = 7,
    ALIGN_TYPE_256_BYTE = 8,
    ALIGN_TYPE_512_BYTE = 9,
    ALIGN_TYPE_1024_BYTE = 10,
    ALIGN_TYPE_2048_BYTE = 11,
    ALIGN_TYPE_4096_BYTE = 12,
    ALIGN_TYPE_8192_BYTE = 13,
    ALIGN_TYPE_16384_BYTE = 14,
    ALIGN_TYPE_32768_BYTE = 15
} EAlignType;

typedef SCODE (* FOnRequestMem)(DWORD dwUserData,
                                DWORD dwSize,
                                EMemType eMemType,
                                EAlignType eAlignType,
                                DWORD *pdwUserBaseAddr,
                                DWORD *pdwPhyBaseAddr);
typedef SCODE (* FOnRequestNonCachedMem)(DWORD dwUserData,
                                         DWORD dwSize,
                                         DWORD *pdwUserBaseAddr,
                                         DWORD *pdwPhyBaseAddr);
typedef SCODE (* FOnRequestCachedMem)(DWORD dwUserData,
                                      DWORD dwSize,
                                      DWORD *pdwUserBaseAddr,
                                      DWORD *pdwPhyBaseAddr);

/* ====================================================================================== */
typedef SCODE (* FOnReadReg)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwSpecRegAddr, DWORD *pdwData);
typedef SCODE (* FOnWriteReg)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwRegAddr, DWORD dwSpecRegAddr, DWORD dwData);
typedef SCODE (* FOnReadBuf)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwCmdRegAddr, DWORD dwLength, BYTE *pbyData);
typedef SCODE (* FOnWriteBuf)(DWORD dwUserData, DWORD dwDeviceAddr, DWORD dwCmdRegAddr, DWORD dwLength, BYTE *pbyData);

/* ====================================================================================== */
/*! FOURCC for H263 video codec */
#define FOURCC_H263 (MAKEFOURCC('H','2','6','3'))
/*! FOURCC for MPEG-4 video codec */
#define FOURCC_MP4V (MAKEFOURCC('M','P','4','V'))
/*! FOURCC for MPEG-2 video codec */
#define FOURCC_MP2V (MAKEFOURCC('M','P','2','V'))
/*! FOURCC for JPEG image codec */
#define FOURCC_JPEG (MAKEFOURCC('J','P','E','G'))
/*! FOURCC for H264 video codec */
#define FOURCC_H264 (MAKEFOURCC('H','2','6','4'))

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
/*! FOURCC for G.726 codec */
#define FOURCC_G726 (MAKEFOURCC('G','7','2','6'))
/*! FOURCC for a-LAW codec */
#define FOURCC_ALAW (MAKEFOURCC('A','L','A','W'))
/*! FOURCC for u-LAW codec */
#define FOURCC_ULAW (MAKEFOURCC('U','L','A','W'))
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
/*! FOURCC for G.729AB speech codec */
#define FOURCC_729A (MAKEFOURCC('7','2','9','A'))
/*! FOURCC for G.729B speech codec */
#define FOURCC_729B (MAKEFOURCC('7','2','9','B'))

/*! FOURCC for VIVO codec */
#define FOURCC_VIVO (MAKEFOURCC('V','I','V','O'))
/*! FOURCC for TEXT codec */
#define FOURCC_TEXT (MAKEFOURCC('T','E','X','T'))
/*! FOURCC for HINT codec */
#define FOURCC_HINT (MAKEFOURCC('H','I','N','T'))
/*! FOURCC for EVNT */
#define FOURCC_EVNT (MAKEFOURCC('E','V','N','T'))
/*! FOURCC for Motion Event */
#define FOURCC_MOTN (MAKEFOURCC('M','O','T','N'))
/*! FOURCC for Digital Input Event */
#define FOURCC_DIGI (MAKEFOURCC('D','I','G','I'))
/*! FOURCC for Meta Data */
#define FOURCC_MDAT (MAKEFOURCC('M','D','A','T'))


/*! FOURCC for configuration */
#define FOURCC_CONF (MAKEFOURCC('C','O','N','F'))

/*! FOURCC for proprietary codec */
#define FOURCC_PROP (MAKEFOURCC('P','R','O','P'))

/*! GB and PB engine buffer type enumeration */
typedef enum buffer_type_flags
{
    JPEGNONCIRCULAR = 1,
    JPEGCIRCULAR = 2,
    MPEGNONCIRCULAR = 3,
    MPEGCIRCULAR = 4,
    H264NONCIRCULAR = 5,
	H26LNONCIRCULAR = 5,
    H264CIRCULAR = 6,
    H26LCIRCULAR = 6,
    H264IOVECTOR = 7,
    H26LIOVECTOR = 7
} EBufTypeFlags;
#define TBufTypeFlags EBufTypeFlags

/*! GB and PB engine io vector mode enumeration */
typedef enum io_vector_mode
{
    IO_VECTOR_BYTE = 0,
    IO_VECTOR_WORD = 1,
    IO_VECTOR_DWORD = 2,
	IO_VECTOR_NONE = 4
} EIOVectorMode;

/* ====================================================================================== */
/*! Video signal format enumeration */
typedef enum video_signal_format
{
    VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW = 1,
    VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656 = 2,
    VIDEO_SIGNAL_FORMAT_PROGRESSIVE_CCIR656 = 3,
    VIDEO_SIGNAL_FORMAT_BAYER_PATTERN = 4,
    VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS = 5,
    VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120 = 6,
    VIDEO_SIGNAL_FORMAT_INTERLACE_BT1120 = 7,
	VIDEO_SIGNAL_FORMAT_RGB24 = 8,
	VIDEO_SIGNAL_FORMAT_INTERLACE_RAW = 9,
	VIDEO_SIGNAL_FORMAT_INTERLACE_RAW_16BITS = 10,
	VIDEO_SIGNAL_FORMAT_RGB16 = 11,
	VIDEO_SIGNAL_FORMAT_INTERLEAVE_RGB16 = 12,
	VIDEO_SIGNAL_FORMAT_INTERLEAVE_RGB24 = 13
} EVideoSignalFormat;

typedef enum video_signal_size
{
    VIDEO_SIGNAL_SIZE_160x120   = 1,
    VIDEO_SIGNAL_SIZE_176x120   = 2,
    VIDEO_SIGNAL_SIZE_176x144   = 3,
    VIDEO_SIGNAL_SIZE_320x240   = 4,
    VIDEO_SIGNAL_SIZE_352x240   = 5,
    VIDEO_SIGNAL_SIZE_352x288   = 6,
    VIDEO_SIGNAL_SIZE_640x240   = 7,
    VIDEO_SIGNAL_SIZE_704x240   = 8,
    VIDEO_SIGNAL_SIZE_704x288   = 9,
    VIDEO_SIGNAL_SIZE_720x240   = 10,
    VIDEO_SIGNAL_SIZE_720x288   = 11,
    VIDEO_SIGNAL_SIZE_640x480   = 12,
    VIDEO_SIGNAL_SIZE_704x480   = 13,
    VIDEO_SIGNAL_SIZE_704x576   = 14,
    VIDEO_SIGNAL_SIZE_720x480   = 15,
    VIDEO_SIGNAL_SIZE_720x576   = 16,
    VIDEO_SIGNAL_SIZE_480x272   = 17,
    VIDEO_SIGNAL_SIZE_768x576   = 18,
    VIDEO_SIGNAL_SIZE_800x480   = 19,
    VIDEO_SIGNAL_SIZE_800x600   = 20,
    VIDEO_SIGNAL_SIZE_854x480   = 21,
    VIDEO_SIGNAL_SIZE_1024x600  = 22,
    VIDEO_SIGNAL_SIZE_1024x768  = 23,
    VIDEO_SIGNAL_SIZE_1152x768  = 24,
    VIDEO_SIGNAL_SIZE_1280x720  = 25,
    VIDEO_SIGNAL_SIZE_1280x768  = 26,
    VIDEO_SIGNAL_SIZE_1280x800  = 27,
    VIDEO_SIGNAL_SIZE_1280x854  = 28,
    VIDEO_SIGNAL_SIZE_1280x960  = 29,
    VIDEO_SIGNAL_SIZE_1280x1024 = 30,
    VIDEO_SIGNAL_SIZE_1368x768  = 31,
    VIDEO_SIGNAL_SIZE_1400x1050 = 32,
    VIDEO_SIGNAL_SIZE_1440x900  = 33,
    VIDEO_SIGNAL_SIZE_1440x960  = 34,
    VIDEO_SIGNAL_SIZE_1600x1200 = 35,
    VIDEO_SIGNAL_SIZE_1680x1050 = 36,
    VIDEO_SIGNAL_SIZE_1920x1080 = 37,
    VIDEO_SIGNAL_SIZE_1920x1200 = 38,
    VIDEO_SIGNAL_SIZE_2048x1080 = 39,
    VIDEO_SIGNAL_SIZE_2048x1536 = 40,
    VIDEO_SIGNAL_SIZE_2560x1600 = 41,
    VIDEO_SIGNAL_SIZE_2560x2048 = 42

} EVideoSignalSize;

/*! Video signal color temperature enumeration */
typedef enum video_signal_color_temperature
{
    VIDEO_SIGNAL_COLOR_TEMPERATURE_2856K = 1,
    VIDEO_SIGNAL_COLOR_TEMPERATURE_3200K = 1,
    VIDEO_SIGNAL_COLOR_TEMPERATURE_4150K = 2,
    VIDEO_SIGNAL_COLOR_TEMPERATURE_5500K = 2,
    VIDEO_SIGNAL_COLOR_TEMPERATURE_GT_5500K = 3,
    VIDEO_SIGNAL_COLOR_TEMPERATURE_6500K = 3
} EVideoSignalColorTemperature;

/*! Video signal frequency enumeration */
typedef enum video_signal_frequency
{
    VIDEO_SIGNAL_FREQUENCY_50HZ = 1,
    VIDEO_SIGNAL_FREQUENCY_60HZ = 2,
    VIDEO_SIGNAL_FREQUENCY_24HZ = 3,
    VIDEO_SIGNAL_FREQUENCY_30HZ = 4
} EVideoSignalFrequency;

/*! Video signal option flag enumeration */
typedef enum video_signal_option_flags
{
    VIDEO_SIGNAL_OPTION_RESET = 1,
    VIDEO_SIGNAL_OPTION_SET_SIZE = 2,
    VIDEO_SIGNAL_OPTION_SET_BRIGHTNESS = 3,
    VIDEO_SIGNAL_OPTION_SET_CONTRAST = 4,
    VIDEO_SIGNAL_OPTION_SET_HUE = 5,
    VIDEO_SIGNAL_OPTION_SET_SATURATION = 6,
    VIDEO_SIGNAL_OPTION_SET_COLOR_TEMPERATURE = 7,
    VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_CTRL = 8,
    VIDEO_SIGNAL_OPTION_SET_AUTO_GAIN_CTRL = 9,
    VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_CTRL = 10,
    VIDEO_SIGNAL_OPTION_SET_AUTO_BRIGHTNESS_CTRL = 11,
    VIDEO_SIGNAL_OPTION_SET_FREQUENCY = 12,
    VIDEO_SIGNAL_OPTION_SET_FLIP = 13,
    VIDEO_SIGNAL_OPTION_SET_MIRROR = 14,
    VIDEO_SIGNAL_OPTION_SET_MONO = 15,
    VIDEO_SIGNAL_OPTION_SET_LOW_PASS_FILTER = 16,
    VIDEO_SIGNAL_OPTION_SET_CAPTURE_AREA = 17,
    VIDEO_SIGNAL_OPTION_SET_START_PIXEL = 18,
    VIDEO_SIGNAL_OPTION_SET_NIGHT_MODE = 19,
    VIDEO_SIGNAL_OPTION_SET_FRAME_RATE = 20,
    VIDEO_SIGNAL_OPTION_SET_OUTPUT_TYPE = 21,
    VIDEO_SIGNAL_OPTION_SET_ZOOM = 22,
    VIDEO_SIGNAL_OPTION_SET_PRIVACY_MASK = 23,
    VIDEO_SIGNAL_OPTION_SET_AUTO_TRACK_WHITE = 24,
    VIDEO_SIGNAL_OPTION_SET_EXPOSURE_TIME = 25,
    VIDEO_SIGNAL_OPTION_SET_FIELD_INVERSE = 26,
    VIDEO_SIGNAL_OPTION_RESET_FRAME_RATE_CTRL = 27,
    VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL = 28,
    VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL = 29,
    VIDEO_SIGNAL_OPTION_SET_CONFIG = 30,
    VIDEO_SIGNAL_OPTION_SET_BACKLIGHT_COMPENSATION = 31,
    VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_CTRL = 32,
    VIDEO_SIGNAL_OPTION_SET_EXPOSURE_LEVEL = 33,
    VIDEO_SIGNAL_OPTION_SET_SHARPNESS = 34,
    VIDEO_SIGNAL_OPTION_SET_HALF_SIZED_OUTPUT = 35,
    VIDEO_SIGNAL_OPTION_SET_COLOR_CORRECTION = 36,
    VIDEO_SIGNAL_OPTION_SET_GAMMA_TABLE = 37,
	VIDEO_SIGNAL_OPTION_SET_TONE_MAPPING = 39,
	VIDEO_SIGNAL_OPTION_SET_CONTRAST_ENHANCEMENT = 40,
	VIDEO_SIGNAL_OPTION_PHOTO_LDC_CALIBRATE = 41,
	VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_TABLE = 42,
	VIDEO_SIGNAL_OPTION_SET_AUTO_COLOR_SUPPRESSION = 43,
	VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_EN = 44,
	VIDEO_SIGNAL_OPTION_SET_OBJECT_MASK = 45,
	VIDEO_SIGNAL_OPTION_AUTO_DETECT_STD = 52,
	VIDEO_SIGNAL_OPTION_SET_WDR = 53,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS = 38,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOW_PRIORITY = 46,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER = 47,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN = 48,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE = 49,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER = 50,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN = 51,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MODE = 54,
	VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_EN = 55,
	VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_WINDOW = 56,
	VIDEO_SIGNAL_OPTION_SET_FOCUS_POSITION = 57,
	VIDEO_SIGNAL_OPTION_SET_FOCUS_SPEED = 58,
	VIDEO_SIGNAL_OPTION_SET_ZOOM_POSITION = 59,
	VIDEO_SIGNAL_OPTION_SET_ZOOM_SPEED = 60,
	VIDEO_SIGNAL_OPTION_SET_FOCUS_NOISE_THRES = 61,
	VIDEO_SIGNAL_OPTION_SET_ZOOMTRACKING_FOCUS_EN = 62,
	VIDEO_SIGNAL_OPTION_GET_AUTO_FOCUS_TABLE_SIZE = 63,
	VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_CALIBRATE = 64,
	VIDEO_SIGNAL_OPTION_SET_AUTO_FOCUS_TABLE = 65,
	VIDEO_SIGNAL_OPTION_SET_ANTI_ALIASING = 66,
	VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_SPEED = 67,
	VIDEO_SIGNAL_OPTION_SET_AUTO_IRIS_ACTIVE_TIME = 68,
	VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE = 69,
	VIDEO_SIGNAL_OPTION_SET_BLACK_CLAMP = 70,
	VIDEO_SIGNAL_OPTION_SET_IMPULSE_NOISE_REMOVAL = 71,
	VIDEO_SIGNAL_OPTION_SET_AUTO_WHITE_BALANCE_WINDOW_PRIORITY = 72,
	VIDEO_SIGNAL_OPTION_SET_CACHE_COHERENCE = 73,
	VIDEO_SIGNAL_OPTION_GET_COLOR_TEMPERATURE = 74,
	VIDEO_SIGNAL_OPTION_SET_COLOR_TRANSFORM = 75,
	VIDEO_SIGNAL_OPTION_SENSOR_DIRECT_ACCESS = 76,
	VIDEO_SIGNAL_OPTION_SET_COMPRESS_FORMAT = 77,
	// [NOTE]!! MAXIMUM NUMBER is 77
} EVideoSignalOptionFlags;

/* ! Video display option flag enumeration */
typedef enum video_display_option_flags
{
    VIDEO_DISP_OPTION_SET_INPUT = 1,
    VIDEO_DISP_OPTION_SET_OVERLAY = 2,
    VIDEO_DISP_OPTION_SET_ALPHA_VALUE = 3,
    VIDEO_DISP_OPTION_SET_KEY = 4,
    VIDEO_DISP_OPTION_SET_CURSOR = 5,
    VIDEO_DISP_OPTION_SET_PIXEL_FORMAT = 6,
    VIDEO_DISP_OPTION_SET_PAL = 7,
    VIDEO_DISP_OPTION_SET_BRIGHTNESS = 8,
    VIDEO_DISP_OPTION_SET_CONTRAST = 9,
    VIDEO_DISP_OPTION_SET_SATURATION = 10,
    VIDEO_DISP_OPTION_SET_GAMMA = 11,
    VIDEO_DISP_OPTION_SET_OVERLAY_POSITION = 12,
    VIDEO_DISP_OPTION_SET_DISPLAY_BUFFER = 13,
    VIDEO_DISP_OPTION_SET_INPUT_OFFSET = 14,
    VIDEO_DISP_OPTION_SET_OUTPUT_CTRL = 15,
	VIDEO_DISP_OPTION_SET_VIEW = 16,
    VIDEO_DISP_OPTION_SET_OUTPUT_SIZE = 17,
    VIDEO_DISP_OPTION_CABLE_DETECT = 18,
} EVideoDispOptionFlags;

/* ====================================================================================== */
/* ! Encoding rate control type enumeration */
typedef enum rate_control_flags
{
    RATE_CTRL_NONE = 0,         // constant quality and variable bitrate
    RATE_CTRL_VQCB = 1,         // variable quality and constant bitrate
    RATE_CTRL_CQCB = 2,         // constant quality and constant bitrate
    RATE_CTRL_STRICT_VQCB = 3,  // strict variable quality and constant bitrate, never overflow
    RATE_CTRL_CVBR = 4,          // constant quality when bitrate meet the requirement.
	RATE_CTRL_ADPT =5           //Adaptive control for different conditions
} ERateCtrlFlags;

/* ! Encoding frame color components enumeration */
typedef enum color_flags
{
    COLOR = 1,
    MONO = 2
} EColorFlags;
#define TColorFlags EColorFlags

/* ! Video output pixel format enumeration */
typedef enum pixel_format_flags
{
    YUV420 = 1,
    YUV422 = 2,
    YUY2 = 3,
    UYVY = 4,
    RGB16555 = 5,
    RGB16565 = 6,
    RGB24 = 7,
    FLIP_RGB24 = 8,                 // obsolete
    RGB32 = 9,
    NONE = 10,
    YUV444 = 11,
    BGR16555 = 12,
    BGR16565 = 13,
    BGR24 = 14,
    BGR32 = 15,
    SP420 = 16,                     // semi-planar 420, aka NV12
    SP422 = 17,                     // semi-planar 422
    ARGB32 = 18,
    ARGB16 = 19
} EPixelFormatFlags;
#define TPixelFormatFlags EPixelFormatFlags

typedef enum nal_type_flags
{
	NAL_TYPE_NON_IDR_SLICE = 1,
	NAL_TYPE_DATA_PARTITION_A_SLICE = 2,
	NAL_TYPE_DATA_PARTITION_B_SLICE = 3,
	NAL_TYPE_DATA_PARTITION_C_SLICE = 4,
	NAL_TYPE_IDR_SLICE = 5,
	NAL_TYPE_SEI = 6,
	NAL_TYPE_SEQUENCE_PARAMETER_SET = 7,
	NAL_TYPE_PICTURE_PARAMETER_SET = 8,
	NAL_TYPE_ACCESS_UNIT_DELIMITER = 9,
	NAL_TYPE_END_OF_SEQUENCE = 10,
	NAL_TYPE_END_OF_STREAM = 11,
	NAL_TYPE_FILLER_DATA = 12,
	NAL_TYPE_SEQUENCE_PARAMETER_SET_EXTENSION = 13,
	NAL_TYPE_PREFIX_NAL_UNIT = 14
} ENALTypeFlags;

/* ! Video and image codec option enumeration */
typedef enum video_option_flags
{
    VIDEO_RESTART = 1,
    VIDEO_CHANGE_FRAME_BUFFER = 2,  // should be replaced with option 17 and 18
    VIDEO_CHANGE_BITSTREAM_BUFFER = 3,
    VIDEO_FLUSH_BITSTREAM_BUFFER = 4,
    VIDEO_CHANGE_USERDATA_BUFFER = 5,
    VIDEO_FORCE_INTRA = 6,
    VIDEO_CHANGE_QUANT = 7,
    VIDEO_CHANGE_PIXEL_FORMAT = 8,  // should be replaced with option 19 and 20
    VIDEO_SET_DEBLOCKING = 9,
    VIDEO_CHANGE_COLOR = 10,
    VIDEO_CHANGE_INTRA_INTERVAL = 11,
    VIDEO_CHANGE_BITRATE = 12,
    VIDEO_SET_RATE_CONTROL = 13,
    VIDEO_CHANGE_FRAME_SIZE = 14,   // should be replaced with option 21 and 22
    VIDEO_CHANGE_INTRA_MB_REFRESH_NUM = 15,
    VIDEO_CHANGE_MOTION_DETECTION_THRESHOLD = 16,
    VIDEO_CHANGE_IN_FRAME_BUFFER = 17,
    VIDEO_CHANGE_OUT_FRAME_BUFFER = 18,
    VIDEO_CHANGE_IN_PIXEL_FORMAT = 19,
    VIDEO_CHANGE_OUT_PIXEL_FORMAT = 20,
    VIDEO_CHANGE_IN_FRAME_SIZE = 21,
    VIDEO_CHANGE_OUT_FRAME_SIZE = 22,
    VIDEO_SET_OUTPUT = 23,
    VIDEO_SET_DERINGING = 24,
    VIDEO_SET_VERTICAL_DOUBLE = 25,
    VIDEO_CHANGE_DISPOSITION = 26,  // Frame-based or block-based
    VIDEO_CHANGE_OVERLAY_FRAME_BUFFER = 27,
    VIDEO_CHANGE_OVERLAY_FRAME_SIZE = 28,
    VIDEO_CHANGE_OVERLAY_CTRL = 29,
    VIDEO_CHANGE_IN_FRAME_OFFSET = 30,
    VIDEO_CHANGE_ENCODE_MODE = 31,
    VIDEO_CHANGE_ROTATE_DEGREE = 32,
    VIDEO_CHANGE_SUB_SAMPLE_RATIO = 33,
    VIDEO_CHANGE_WINDOW_POSITION = 34,
    VIDEO_CHANGE_WINDOW_SIZE = 35,
    VIDEO_CHANGE_IN_MODE = 36,
    VIDEO_ADD_WINDOW = 37,
	VIDEO_DELETE_WINDOW = 38,
    VIDEO_CHANGE_QUANT_TABLE = 39,
    VIDEO_DELETE_ALL_WINDOWS = 40,
    VIDEO_FD_SET_DB = 41,
    VIDEO_LPD_SET_DB = 42,
	VIDEO_OD_SET_FB_AREA = 43,
	VIDEO_CHANGE_OBJECT_INFO_BUFFER = 44,
	VIDEO_CHANGE_OBJECT_QP_DELTA = 45,
	VIDEO_OD_SET_ROI_MASK = 46,
	VIDEO_CHANGE_SEARCH_RANGE = 47,
	VIDEO_CHANGE_REFERENCE_FRAME_NUM = 48,
	VIDEO_OD_SET_FLAME_PARAM = 49,
	VIDEO_OD_SET_MB = 50,
	VIDEO_OD_SET_FG_BLK_TYPE = 51,
    VIDEO_CHANGE_BITSTREAM_IO_VECTOR_BUFFERS = 52,
    VIDEO_CHANGE_SCALABILITY_TEMPORAL_LAYER_NUM = 53,
    VIDEO_CHANGE_REGION_OF_INTEREST_INFO = 54,
    VIDEO_CHANGE_SCALABILITY_HEADER_EN = 55,
    VIDEO_CHANGE_ENCODE_MAXIMUM_FRAME_RATE = 56,
	VIDEO_SET_DECODE_FRAME_NUM = 57,
	VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO = 58,
	VIDEO_CHANGE_I_SLICE_QUALITY_STRATEGY = 59,
	VIDEO_GET_FRAME_QUANT = 60,
	VIDEO_CHANGE_FRAME_RATE = 61,
	VIDEO_CLEAN_INTERNAL_BUFFER = 62,
	VIDEO_GET_MB_INFO_BUFF = 63,
	VIDEO_CHANGE_FRAME_TIME_TICKS_INFO = 64,
	VIDEO_CHANGE_H264_ENCODE_DEBLOCKING_FLAG = 65,
	VIDEO_CHANGE_SVC_ENCODE_DEBLOCKING_FLAG = 66,
	VIDEO_CHANGE_COMPRESS_FORMAT_FLAG  =67
} EVideoOptionFlags;
#define TVideoOptionFlags EVideoOptionFlags

/* =========================================================================================== */
typedef enum sub_sample_ratio
{
    SSR_ORG = 0,
    SSR_HALF = 1,
    SSR_QUARTER = 2,
    SSR_OCT = 3
} ESubSampleRatio;

/* ====================================================================================== */
typedef struct video_signal_options
{
    EVideoSignalOptionFlags eOptionFlags;
    DWORD adwUserData[3];
} TVideoSignalOptions;

/* ! Version number structure */
typedef struct version_num
{
    BYTE byMajor;
    BYTE byMinor;
    BYTE byBuild;
    BYTE byRevision;
} TVersionNum;

typedef SCODE (* FOnVertDouble)(DWORD dwUserData, BYTE *pbyInYFrame, BYTE *pbyInCbFrame, BYTE *pbyInCrFrame, BYTE *pbyOutYFrame, BYTE *pbyOutCbFrame, BYTE *pbyOutCrFrame, EPixelFormatFlags ePixelFormat, DWORD dwStride, DWORD dwWidth, DWORD dwHeight);
typedef SCODE (* FOnDeblocking)(DWORD dwUserData, BYTE *pbyInYFrame, BYTE *pbyInCbFrame, BYTE *pbyInCrFrame, EPixelFormatFlags ePixelFormat, DWORD dwStride, DWORD dwWidth, DWORD dwHeight, DWORD dwQuant);
typedef SCODE (* FOnDeringing)(DWORD dwUserData, BYTE *pbyInYFrame, BYTE *pbyInCbFrame, BYTE *pbyInCrFrame, BYTE *pbyOutYFrame, BYTE *pbyOutCbFrame, BYTE *pbyOutCrFrame, EPixelFormatFlags ePixelFormat, DWORD dwStride, DWORD dwWidth, DWORD dwHeight, DWORD dwQuant);
typedef SCODE (* FOnMoving)(DWORD dwUserData, BYTE *pbyInYFrame, BYTE *pbyInCbFrame, BYTE *pbyInCrFrame, BYTE *pbyOutYFrame, BYTE *pbyOutCbFrame, BYTE *pbyOutCrFrame, EPixelFormatFlags ePixelFormat, DWORD dwClockwiseAngle, DWORD dwInStride, DWORD dwOutStride, DWORD dwWidth, DWORD dwHeight);
typedef SCODE (* FOnConverting)(DWORD dwUserData, BYTE *pbyInYFrame, BYTE *pbyInCbFrame, BYTE *pbyInCrFrame, BYTE *pbyOutYFrame, BYTE *pbyOutCbFrame, BYTE *pbyOutCrFrame, EPixelFormatFlags eInPixelFormat, EPixelFormatFlags eOutPixelFormat, DWORD dwClockwiseAngle, DWORD dwInStride, DWORD dwOutStride, DWORD dwWidth, DWORD dwHeight);
typedef SCODE (* FOnBlock2Frame)(DWORD , BYTE *, BYTE *, BYTE *, BYTE *, BYTE *, BYTE *, EPixelFormatFlags , DWORD , DWORD , DWORD , ESubSampleRatio, ESubSampleRatio);

typedef enum autoiris_en
{
		AUTOIRIS_DISABLE = 0,
		AUTOIRIS_ENABLE = 1,
}EAutoIrisEn;

typedef enum autoexposure_mode
{
		AEMODE_AUTO	= 0,
		AEMODE_BACKLIGHT	= 1,
		AEMODE_CUSTOMIZED = 2,
}EAEMode;

typedef enum tw2866_standard
{
		TVSTD_NTSC	= 0,
		TVSTD_PAL	= 1,
		TVSTD_UNKNOWN = 2,
}ETVStd;

typedef struct video_sensor_initial_param
{
	DWORD dwVideoSensorVersion;
	/* Sensor light frequency (50 Hz or 60 Hz) */
	DWORD dwVideoSensorFrequency;
} TVideoSensorInitialParam;

typedef struct auto_detect_std_info {
	BOOL bDataLost;
	ETVStd eDetectStd;
}TAutoDetectStdInfo;

typedef struct video_sensor_statistic
{
	DWORD dwLumaHorSubWindowNum;
	DWORD dwLumaVerSubWindowNum;
	DWORD *pdwStatisticY;

	DWORD dwWBHorSubWindowNum;
	DWORD dwWBVerSubWindowNum;
	DWORD *pdwStatisticR;
	DWORD *pdwStatisticG;
	DWORD *pdwStatisticB;

	DWORD dwHistogramBinNum;
	DWORD *pdwHistogram;
} TVideoSensorStatistic;

/* Window/crop information structrue */
typedef struct window_info
{
    DWORD dwHorzStart;
    DWORD dwVertStart;
    DWORD dwWidth;
    DWORD dwHeight;
} TWindowInfo;

/* sensor call back function */
typedef struct video_sensor_device
{
	/* open function */
	int (*open)(TVideoSensorInitialParam* ptParam, DWORD dwDevNum);
	/* release function */
	void (*release)(DWORD dwDevNum);
	/* ioctl function */
	int (*ioctl)( TVideoSignalOptions* ptArg, DWORD dwDevNum);
	/* getting current shutter value function */
	DWORD (*get_shutter_value)(void);
	/* getting sensor remaining line function */
	DWORD (*get_remaining_line_num)(void);
	/* grouping access function */
	void (*group_access)(int status);
	/* detect NTSC/PAL standard */
	void (*detect_standard)(DWORD dwDevNum, TAutoDetectStdInfo *ptInfo);
	/* setup NTSC/PAL standard */
	void (*setup_standard)(DWORD dwDevNum, ETVStd eStd);
	/* get ae statistic from sensor */
	int (*get_exp_statistic)(TVideoSensorStatistic *ptParam, DWORD dwDevNum);
	/* get wb statistic from sensor */
	int (*get_wb_statistic)(TVideoSensorStatistic *ptParam, DWORD dwDevNum);
	/* getting sensor max gain function */
	DWORD (*get_max_gain)(void);
} TVideoSensorDevice;

/* Iris motor call back function */
typedef struct iris_motor_device
{
	int (*open)(void);
	void (*release)(void);
	int  (*control)(DWORD dwCmd,  DWORD dwArg);
	int (*set_options)(TVideoSignalOptions *ptArg);
} TIrisMotorDevice;

/* Iris motor control info */
typedef struct iris_ctrl_info
{
	DWORD dwSpeed;
	DWORD dwStep;
	BOOL bWaitCmpt;
} TIrisCtrlInfo;
/* ====================================================================================== */
// With respective to CMOS sensor
typedef enum lens_move_direction
{
	LENS_NO_MOVE = 0,
	LENS_MOVE_NEAR = 1,
	LENS_MOVE_FAR = 2,
} ELensMoveDirection;

/* Lens motor control options */
typedef enum lens_control_flags
{
	LENS_CTRL_FOCUS_STOP = 1,
	LENS_CTRL_FOCUS_RESET = 2,
	LENS_CTRL_FOCUS_GET_POSITION = 3,
	LENS_CTRL_FOCUS_GET_RANGE = 4,
	LENS_CTRL_FOCUS_MOVE_TARGET = 5,
	LENS_CTRL_FOCUS_MOVE_NEAR_LIMIT = 6,
	LENS_CTRL_FOCUS_MOVE_FAR_LIMIT = 7,
	LENS_CTRL_ZOOM_STOP = 8,
	LENS_CTRL_ZOOM_RESET = 9,
	LENS_CTRL_ZOOM_GET_POSITION = 10,
	LENS_CTRL_ZOOM_GET_RANGE = 11,
	LENS_CTRL_ZOOM_MOVE_TARGET = 12,
	LENS_CTRL_ZOOM_MOVE_TELE = 13,
	LENS_CTRL_ZOOM_MOVE_WIDE = 14,
	LENS_CTRL_ZOOM_GET_MAXRATIO = 15,
	LENS_CTRL_FOCUS_ZOOM_STOP = 16,
	LENS_CTRL_FOCUS_ZOOM_MOVE_TARGET = 17,
	LENS_CTRL_IRCUT_ENABLE = 18,
	LENS_CTRL_IRCUT_DISABLE = 19,
	LENS_CTRL_IRIS_STOP = 20,
	LENS_CTRL_IRIS_OPEN_LARGEST = 21,
	LENS_CTRL_IRIS_OPEN = 22,
	LENS_CTRL_IRIS_CLOSE = 23,
	LENS_CTRL_IRIS_CLOSE_SMALLEST = 24,
	LENS_CTRL_IRIS_GET_POSITION = 25,
	LENS_CTRL_IRIS_GET_RANGE = 26,
} ELensControlFlags;

/* Auto-Focus motor call back function */
typedef struct autofocus_motor_device
{
	int (*open)(void);
	void (*release)(void);
	int (*control)(unsigned int dwCmd, DWORD dwArg);
} TAutoFocusMotorDevice;

/* Auto-Focus motor control info */
typedef struct autofocus_control_info
{
	DWORD dwFocusDirection;
	DWORD dwFocusSpeed;
	DWORD dwFocusStep;
	DWORD dwZoomDirection;
	DWORD dwZoomSpeed;
	DWORD dwZoomStep;
	BOOL bWaitCmpt;	// 1: TRUE, 0:FALSE
} TAutoFocusCtrlInfo;

#endif // codec

/* ====================================================================================== */

