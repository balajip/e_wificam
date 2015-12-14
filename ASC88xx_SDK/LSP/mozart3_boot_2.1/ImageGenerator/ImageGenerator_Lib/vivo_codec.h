/*
 * $Header: /rd_2/Peripherals/MT9V111/MT9V111_Lib/vivo_codec.h 89    06/11/07 2:43p Ycchang $
 *
 * $History: vivo_codec.h $
 * 
 * *****************  Version 89  *****************
 * User: Ycchang      Date: 06/11/07   Time: 2:43p
 * Updated in $/rd_2/Peripherals/MT9V111/MT9V111_Lib
 * MODIFICATION: Add VIDEO_SIGNAL_OPTION_SET_AUTO_TRACK_WHITE option to
 * EVideoSignalOptionFlags - DONE.
 * 
 * *****************  Version 88  *****************
 * User: Ycchang      Date: 06/11/02   Time: 3:43p
 * Updated in $/rd_2/Peripherals/PV480/PV480_Lib
 * MODIFICATION: Add VIDEO_SIGNAL_OPTION_SET_ZOOM and
 * VIDEO_SIGNAL_OPTION_SET_PRIVACY_MASK option to
 *  * EVideoSignalOptionFlags - DONE.
 * 
 * *****************  Version 87  *****************
 * User: Sor          Date: 06/10/12   Time: 10:16a
 * Updated in $/rd_2/image/ImgProcess/MMX/ImgProcess/ImgProcess_Lib
 * 
 * *****************  Version 86  *****************
 * User: Sor          Date: 06/10/11   Time: 6:25p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/GBEngine_Lib
 * 
 * *****************  Version 85  *****************
 * User: Sor          Date: 06/10/11   Time: 5:49p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/GBEngine_Lib
 * FEATURE: Use PVOID data type for all cross platform variables - DONE.
 * 
 * *****************  Version 84  *****************
 * User: Sor          Date: 06/10/05   Time: 9:11p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/GBEngine_Lib
 * MODIFICATION: Change FOnEnterCriticalSec, FOnLeaveCriticalSec, and
 * FOnSendData function type definition - DONE.
 * 
 * *****************  Version 83  *****************
 * User: Sor          Date: 06/08/15   Time: 7:53p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Block2Frame/Block2Frame_Lib
 * 
 * *****************  Version 82  *****************
 * User: Sor          Date: 06/08/15   Time: 7:36p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Mp4VDec/Mp4VDec_Lib
 * 
 * *****************  Version 81  *****************
 * User: Ycchang      Date: 06/08/09   Time: 4:29p
 * Updated in $/rd_2/Peripherals/SAA7113H/SAA7113H_Lib
 * UPDATE: Add VIDEO_SIGNAL_OPTION_SET_OUTPUT_TYPE option to
 * EVideoSignalOptionFlags - DONE.
 *  
 * 
 * *****************  Version 80  *****************
 * User: Vincent      Date: 06/07/26   Time: 5:42p
 * Updated in $/rd_common/common_header
 * MODIFICATION: Add FOURCC for JPEG and HINT
 * 
 * *****************  Version 79  *****************
 * User: Sor          Date: 06/07/05   Time: 10:45a
 * Updated in $/rd_2/project/Vivaldi/Linux_Libraries/Mp4VEnc/Mp4VEnc_Lib
 * MODIFICATION: Add ERateCtrlFlags enumerator - DONE.
 * 
 * *****************  Version 78  *****************
 * User: Bernard      Date: 06/05/15   Time: 7:46p
 * Updated in $/rd_2/misc/PBENGINE/C/PBEngine/PBEngine_Lib
 * MODIFICATION: Add VIDEO_CHANGE_QUANT_TABLE - DONE.
 * 
 * *****************  Version 77  *****************
 * User: Sor          Date: 06/04/21   Time: 11:56a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/Block2Frame/Block2Frame_Lib
 * 
 * *****************  Version 76  *****************
 * User: Sean         Date: 06/03/10   Time: 12:57p
 * Updated in $/rd_2/Training/Library_Development/Sean/MtnDtct/MtnDtct_Lib
 * MODIFICATION: Add VIDEO_ADD_WINDOW and
 * VIDEO_DELETE_WINDOW options - DONE.
 * 
 * *****************  Version 75  *****************
 * User: Sean         Date: 06/03/07   Time: 4:23p
 * Updated in $/rd_2/Training/Library_Development/Sean/MtnDtct/MtnDtct_Lib
 * 
 * *****************  Version 74  *****************
 * User: Sor          Date: 06/03/01   Time: 1:13p
 * Updated in $/rd_2/project/Vivaldi/Libraries/Mp4VEnc/Mp4VEnc_Lib
 * UPDATE: Add VIDEO_CHANGE_IN_MODE option to EVideoOPtionFlags - DONE.
 * 
 * *****************  Version 73  *****************
 * User: Sor          Date: 06/02/21   Time: 6:14p
 * Updated in $/rd_2/Training/Library_Development/Sean/MtnDtct/MtnDtct_Lib
 * MODIFICATION: Add VIDEO_CHANGE_WINDOW_POSITION and
 * VIDEO_CHANGE_WINDOW_SIZE options - DONE.
 * 
 * *****************  Version 72  *****************
 * User: Sor          Date: 06/01/13   Time: 4:29p
 * Updated in $/rd_2/project/SoC/Simulation_Models/Block2Frame_C_Simulation/Block2Frame_Lib
 * 
 * *****************  Version 71  *****************
 * User: Alan         Date: 05/12/13   Time: 5:46p
 * Updated in $/rd_2/project/SoC/Simulation_Models/VideoCap_C_Simulation/VideoCap_Lib
 * UPDATE: Add VIDEO_SIGNAL_FORMAT_BAYER_PATTERN.
 * 
 * *****************  Version 70  *****************
 * User: Cchuang      Date: 05/11/29   Time: 11:28
 * Updated in $/rd_common/common_header
 * 1. MODIFICATION: Add FOURCC for VIVO, TEXT and CONF
 * 
 * *****************  Version 69  *****************
 * User: Sor          Date: 05/10/31   Time: 10:57a
 * Updated in $/rd_2/image/ImgProcess/C/ImgProcess/ImgProcess_Lib
 * MODIFICATION: Add BGR16555, BGR16565, BGR24, and BGR32 to
 * EPixelFormatFlags enumerator - DONE.
 * 
 * *****************  Version 68  *****************
 * User: Sor          Date: 05/08/20   Time: 4:41p
 * Updated in $/rd_2/project/SoC/Simulation_Models/Sample_C_Simulation/MemMgr_Lib
 * 
 * *****************  Version 67  *****************
 * User: Sor          Date: 05/08/20   Time: 4:38p
 * Updated in $/rd_2/project/SoC/Simulation_Models/Sample_C_Simulation/MemMgr_Lib
 * 
 * *****************  Version 66  *****************
 * User: Sor          Date: 05/08/20   Time: 4:27p
 * Updated in $/rd_2/project/SoC/Simulation_Models/Mp4VEnc_C_Simulation/Mp4VEnc_Lib
 * 
 * *****************  Version 65  *****************
 * User: Sor          Date: 05/08/10   Time: 11:09a
 * Updated in $/rd_2/project/Vivaldi/Libraries/IICCtrl/IICCtrl_Lib
 * MODIFICATION: Modify FOnReadReg and FOnWriteReg definition - DONE.
 * 
 * *****************  Version 64  *****************
 * User: Sor          Date: 05/04/21   Time: 4:50p
 * Updated in $/rd_2/image/ImgProcess/C/ImgProcess/ImgProcess_Lib
 * 
 * *****************  Version 63  *****************
 * User: Sor          Date: 05/04/20   Time: 2:29p
 * Updated in $/rd_2/video/H263/C/Decoder/H263Dec/ImgProcess_Lib
 * 
 * *****************  Version 62  *****************
 * User: Sor          Date: 05/04/20   Time: 2:29p
 * Updated in $/rd_2/video/H263/C/Decoder/H263Dec/ImgProcess_Lib
 * 
 * *****************  Version 61  *****************
 * User: Sor          Date: 05/04/20   Time: 1:40p
 * Updated in $/rd_2/video/H263/C/Decoder/H263Dec/H263Dec_Lib
 * 
 * *****************  Version 60  *****************
 * User: Chikuang     Date: 05/03/03   Time: 7:29p
 * Updated in $/rd_2/project/Vivaldi/Libraries/Mp4VEnc/Mp4VEnc_Lib
 * 
 * *****************  Version 59  *****************
 * User: Sor          Date: 05/02/17   Time: 3:35p
 * Updated in $/rd_2/misc/PBENGINE/C/PBEngine/PBEngine_Lib
 * 
 * *****************  Version 58  *****************
 * User: Sor          Date: 05/01/10   Time: 11:45a
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 57  *****************
 * User: Sor          Date: 04/11/19   Time: 10:56a
 * Updated in $/rd_2/project/Vivaldi/Libraries/Mp4VEnc/PBEngine_Lib
 * 
 * *****************  Version 56  *****************
 * User: Chikuang     Date: 04/11/18   Time: 10:28a
 * Updated in $/rd_2/project/Handel/Simulation_Models/ImgProcess_C_Simulation/ImgProcess_Lib
 * 
 * *****************  Version 55  *****************
 * User: Chikuang     Date: 04/11/18   Time: 10:00a
 * Updated in $/rd_2/project/Handel/Simulation_Models/ImgProcess_C_Simulation/VMA_IPE_Lib
 * 
 * *****************  Version 54  *****************
 * User: Sor          Date: 04/11/17   Time: 12:05p
 * Updated in $/rd_2/misc/PBENGINE/C/PBEngine/PBEngine_Lib
 * 
 * *****************  Version 53  *****************
 * User: Sor          Date: 04/11/17   Time: 10:55a
 * Updated in $/rd_2/Peripherals/OV7620/OV7620_Lib
 * 
 * *****************  Version 52  *****************
 * User: Sor          Date: 04/09/17   Time: 1:36p
 * Updated in $/rd_2/Peripherals/OV7648/OV7648_Lib
 * 
 * *****************  Version 51  *****************
 * User: Sor          Date: 04/09/01   Time: 9:23p
 * Updated in $/rd_2/video/MPEG4/C/Decoder/MP4VDEC/Mp4VDec_Lib
 * 
 * *****************  Version 50  *****************
 * User: Sor          Date: 04/09/01   Time: 5:12p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 48  *****************
 * User: Sor          Date: 04/09/01   Time: 4:55p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 47  *****************
 * User: Sor          Date: 04/09/01   Time: 4:53p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 46  *****************
 * User: Sor          Date: 04/09/01   Time: 3:00p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 45  *****************
 * User: Sor          Date: 04/09/01   Time: 1:17p
 * Updated in $/rd_2/video/MPEG4/C/Decoder/MP4VDEC/Mp4VDec_Lib
 * 
 * *****************  Version 44  *****************
 * User: Sor          Date: 04/08/31   Time: 10:32p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 43  *****************
 * User: Sor          Date: 04/08/31   Time: 10:29p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 42  *****************
 * User: Sor          Date: 04/08/31   Time: 10:22p
 * Updated in $/rd_2/image/JPEG/C/Decoder/JPEGDec/JPEGDec_Lib
 * 
 * *****************  Version 41  *****************
 * User: Sor          Date: 04/08/31   Time: 9:23p
 * Updated in $/rd_2/video/H263/C/Decoder/H263Dec/H263Dec_Lib
 * 
 * *****************  Version 40  *****************
 * User: Sor          Date: 04/08/31   Time: 2:23p
 * Updated in $/rd_2/image/ImgProcess/C/ImgProcess/ImgProcess_Lib
 * 
 * *****************  Version 39  *****************
 * User: Sor          Date: 04/08/31   Time: 1:40p
 * Updated in $/rd_2/image/ImgProcess/C/ImgProcess/ImgProcess_Lib
 * 
 * *****************  Version 37  *****************
 * User: Sor          Date: 04/08/20   Time: 7:51p
 * Updated in $/rd_2/Peripherals/OV7648/OV7648_Lib
 * MODIFICATION: Change VIDEO_SIGNAL_OPTION_SET_COLOR_TEMPERATURE option -
 * DONE.
 * 
 * *****************  Version 36  *****************
 * User: Chikuang     Date: 04/07/22   Time: 4:29p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/SAA7113H_Lib
 * 
 * *****************  Version 35  *****************
 * User: Sor          Date: 04/06/24   Time: 7:46p
 * Updated in $/rd_2/project/Vivaldi/Libraries/RealTimeClk/RealTimeClk_Lib
 * UPDATE: Update version number from 1.0.0.0 to 2.0.0.0 - DONE.
 * 
 * *****************  Version 34  *****************
 * User: Sor          Date: 04/06/24   Time: 4:21p
 * Updated in $/rd_2/Peripherals/OV7648/OV7648_Lib
 * 
 * *****************  Version 33  *****************
 * User: Vicky        Date: 04/06/10   Time: 4:24p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/OV7648_Lib
 * 
 * *****************  Version 32  *****************
 * User: Chikuang     Date: 04/06/09   Time: 11:27a
 * Updated in $/rd_2/image/JPEG/C/Encoder/JPEGEnc/JPEGEnc_Lib
 * MODIFICATION: Rename YUV to YUV420 and add YUV422 in TPixelFormatFlags
 * 
 * *****************  Version 31  *****************
 * User: Sor          Date: 04/05/07   Time: 3:02p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * 
 * *****************  Version 30  *****************
 * User: Sor          Date: 04/05/07   Time: 2:31p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * 
 * *****************  Version 29  *****************
 * User: Sor          Date: 04/05/07   Time: 1:48p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * 
 * *****************  Version 28  *****************
 * User: Sor          Date: 04/05/07   Time: 11:51a
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * 
 * *****************  Version 27  *****************
 * User: Sor          Date: 04/05/06   Time: 8:07p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * MODIFICATION: Add alias to some definition with old naming rule - DONE.
 * 
 * *****************  Version 26  *****************
 * User: Sor          Date: 04/05/06   Time: 7:57p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * 
 * *****************  Version 25  *****************
 * User: Chikuang     Date: 04/05/05   Time: 3:40p
 * Updated in $/rd_2/project/Vivaldi/Libraries/Mp4VEnc/Mp4VEnc_Lib
 * 
 * *****************  Version 24  *****************
 * User: Sor          Date: 04/05/05   Time: 9:32a
 * Updated in $/rd_2/Peripherals/OV7648/OV7648_Lib
 * 
 * *****************  Version 23  *****************
 * User: Sor          Date: 04/05/04   Time: 11:32p
 * Updated in $/rd_2/Peripherals/OV7648/OV7648_Lib
 * 
 * *****************  Version 22  *****************
 * User: Sor          Date: 04/05/04   Time: 10:53p
 * Updated in $/rd_2/project/Vivaldi/Libraries/MemMgr/MemMgr_Lib
 * MODIFICATION: Add video signal enumerations - DONE.
 * 
 * *****************  Version 21  *****************
 * User: Sor          Date: 04/04/30   Time: 5:23p
 * Updated in $/rd_2/project/Vivaldi/Libraries/VideoCap/CPE/VideoCap_Lib
 * MODIFICATION: Add request memory callback function definition - DONE.
 * 
 * *****************  Version 20  *****************
 * User: Sor          Date: 04/04/30   Time: 1:42p
 * Updated in $/rd_2/project/Vivaldi/Libraries/Mp4VEnc/Mp4VEnc_Lib
 * MODIFICATION: Add version number structure - DONE.
 * 
 * *****************  Version 19  *****************
 * User: Sor          Date: 03/12/22   Time: 4:49p
 * Updated in $/rd_2/project/SOC/Mp4VEnc_C_Simulation/Mp4VEnc_Lib
 * 
 * *****************  Version 18  *****************
 * User: Sor          Date: 03/11/20   Time: 4:29p
 * Updated in $/rd_2/video/MPEG4/TM1300/Encoder/Mp4VEnc/Mp4VEnc_Lib
 * FEATURE: Add Mp4VEnc_Config.c to library - DONE.
 * 
 * *****************  Version 17  *****************
 * User: Aren         Date: 03/07/09   Time: 5:18p
 * Updated in $/rd_2/speech/G723_1/ARM9/comm_lib
 * MODIFICATION: Remove superfluous ',' in TPixelFormatFlags - DONE.
 * 
 * *****************  Version 16  *****************
 * User: Sor          Date: 03/07/02   Time: 9:32a
 * Updated in $/rd_2/video/H263/C/Decoder/H263Dec/H263Dec_Lib
 * FEATURE: Add RGB16555, RGB16565, and FLIP_RGB24 to TPixelFormatFlags -
 * DONE.
 * 
 * *****************  Version 15  *****************
 * User: Sor          Date: 03/05/30   Time: 4:24p
 * Updated in $/rd_2/project/SOC/Mp4VEnc_C_Simulation/Mp4VEnc_Lib
 * FEATURE: Add TColorFlags - DONE.
 * 
 * *****************  Version 14  *****************
 * User: Sor          Date: 03/05/06   Time: 4:12p
 * Updated in $/rd_2/video/MPEG4/C/Encoder/MP4VENC/Mp4VEnc_Lib
 * MODIFICATION: Add VIDEO_CHANGE_FRAME_SIZE to TVideoOptionFlags
 * structure - DONE.
 * 
 * *****************  Version 13  *****************
 * User: Sor          Date: 03/05/06   Time: 1:57p
 * Updated in $/rd_2/video/MPEG4/C/Encoder/MP4VENC/Mp4VEnc_Lib
 * 
 * *****************  Version 12  *****************
 * User: Sor          Date: 03/05/06   Time: 1:55p
 * Updated in $/rd_2/video/MPEG4/C/Encoder/MP4VENC/Mp4VEnc_Lib
 * MODIFICATION: Original TVideoOptionFlags can't be modified - RECOVERED.
 * 
 * *****************  Version 11  *****************
 * User: Sor          Date: 03/05/06   Time: 1:37p
 * Updated in $/rd_2/video/MPEG4/C/Encoder/MP4VENC/Mp4VEnc_Lib
 * MODIFICATION: Modify TVideoOptionFlags structure - DONE.
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
 * Copyright 2000-2002 VN, Inc. All rights reserved.
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
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
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
typedef void (* FOnEnterCriticalSec)(PVOID pvSemaphore);
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
typedef void (* FOnLeaveCriticalSec)(PVOID pvSemaphore);
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
typedef SCODE (* FOnSendData)(PVOID pvUserData);
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
typedef SCODE (* FOnRequestData)(PVOID pvUserData);
#define TOnRequestData FOnRequestData

/* ====================================================================================== */
#define ALIGN_ADDRESS(a) ((BYTE *)a+ALIGN_PADDING-(((DWORD)(QWORD)a+ALIGN_PADDING)&(~ALIGN_MASK)));

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
/*! FOURCC codec for H263 codec codec */
#define FOURCC_H263 (MAKEFOURCC('H','2','6','3'))
/*! FOURCC codec for MPEG-4 video codec */
#define FOURCC_MP4V (MAKEFOURCC('M','P','4','V'))
/*! FOURCC for MPEG-2 video codec */
#define FOURCC_MP2V (MAKEFOURCC('M','P','2','V'))
/*! FOURCC for JPEG image codec */
#define FOURCC_JPEG (MAKEFOURCC('J','P','E','G'))

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

/*! FOURCC for VIVO codec */
#define FOURCC_VIVO (MAKEFOURCC('V','I','V','O'))
/*! FOURCC for TEXT codec */
#define FOURCC_TEXT (MAKEFOURCC('T','E','X','T'))
/*! FOURCC for HINT codec */
#define FOURCC_HINT (MAKEFOURCC('H','I','N','T'))

/*! FOURCC for configuration */
#define FOURCC_CONF	(MAKEFOURCC('C','O','N','F'))

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
	H26LCIRCULAR = 6
} EBufTypeFlags;
#define TBufTypeFlags EBufTypeFlags

/* ====================================================================================== */
/*! Video signal format enumeration */
typedef enum video_signal_format
{
	VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW = 1,
	VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656 = 2,
	VIDEO_SIGNAL_FORMAT_PROGRESSIVE_CCIR656 = 3,
	VIDEO_SIGNAL_FORMAT_BAYER_PATTERN = 4
} EVideoSignalFormat;

typedef enum video_signal_size
{
	VIDEO_SIGNAL_SIZE_160x120 = 1,
	VIDEO_SIGNAL_SIZE_176x120 = 2,
	VIDEO_SIGNAL_SIZE_176x144 = 3,
	VIDEO_SIGNAL_SIZE_320x240 = 4,
	VIDEO_SIGNAL_SIZE_352x240 = 5,
	VIDEO_SIGNAL_SIZE_352x288 = 6,
	VIDEO_SIGNAL_SIZE_640x240 = 7,
	VIDEO_SIGNAL_SIZE_704x240 = 8,
	VIDEO_SIGNAL_SIZE_704x288 = 9,
    VIDEO_SIGNAL_SIZE_720x240 = 10,
    VIDEO_SIGNAL_SIZE_720x288 = 11,
	VIDEO_SIGNAL_SIZE_640x480 = 12,
	VIDEO_SIGNAL_SIZE_704x480 = 13,
	VIDEO_SIGNAL_SIZE_704x576 = 14,
    VIDEO_SIGNAL_SIZE_720x480 = 15,
    VIDEO_SIGNAL_SIZE_720x576 = 16
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
	VIDEO_SIGNAL_FREQUENCY_60HZ = 2
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
	VIDEO_SIGNAL_OPTION_SET_AUTO_TRACK_WHITE = 24
} EVideoSignalOptionFlags;

/* ====================================================================================== */
/* ! Encoding rate control type enumeration */
typedef enum rate_control_flags
{
	RATE_CTRL_NONE = 0,		// constant quality and variable bitrate
	RATE_CTRL_VQCB = 1,		// variable quality and constant bitrate
	RATE_CTRL_CQCB = 2		// constant quality and constant bitrate
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
	FLIP_RGB24 = 8,					// obsolete
	RGB32 = 9,
	NONE = 10,
	YUV444 = 11,
	BGR16555 = 12,
	BGR16565 = 13,
	BGR24 = 14,
	BGR32 = 15
} EPixelFormatFlags;
#define TPixelFormatFlags EPixelFormatFlags

/* ! Video and image codec option enumeration */
typedef enum video_option_flags
{ 
	VIDEO_RESTART = 1, 
	VIDEO_CHANGE_FRAME_BUFFER = 2,	// should be replaced with option 17 and 18
	VIDEO_CHANGE_BITSTREAM_BUFFER = 3,
	VIDEO_FLUSH_BITSTREAM_BUFFER = 4,
	VIDEO_CHANGE_USERDATA_BUFFER = 5,
	VIDEO_FORCE_INTRA = 6,
	VIDEO_CHANGE_QUANT = 7,
	VIDEO_CHANGE_PIXEL_FORMAT = 8,	// should be replaced with option 19 and 20
	VIDEO_SET_DEBLOCKING = 9,
	VIDEO_CHANGE_COLOR = 10,
	VIDEO_CHANGE_INTRA_INTERVAL = 11,
	VIDEO_CHANGE_BITRATE = 12,
	VIDEO_SET_RATE_CONTROL = 13,
	VIDEO_CHANGE_FRAME_SIZE = 14,	// should be replaced with option 21 and 22
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
    VIDEO_CHANGE_QUANT_TABLE = 39
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
/* ====================================================================================== */
#endif // codec

/* ====================================================================================== */

