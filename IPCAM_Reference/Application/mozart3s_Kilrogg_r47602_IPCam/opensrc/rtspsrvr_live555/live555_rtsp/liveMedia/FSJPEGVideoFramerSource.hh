/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**********/

#ifndef _FS_JPEG_VIDEO_FRAMER_SOURCE_HH
#define _FS_JPEG_VIDEO_FRAMER_SOURCE_HH

#ifndef _JPEG_VIDEO_SOURCE_HH
#include "JPEGVideoSource.hh"
#include <stdint.h>
#include "JPEGVideoFileServerMediaSubsession.hh"
#endif
/*********************************************************
                    HEADER
***********************************************************/
#include "LiveMediaSrc.h"
#include "global_codec.h"
#include "ubuffer.h"

#define JPEG_HEADER_SIZE 589
class FSJPEGVideoFramerSource: public JPEGVideoSource {
public:
  static FSJPEGVideoFramerSource* createNew(UsageEnvironment& env,
					    int Track,HANDLE );
  // "timePerFrame" is in microseconds

protected:
  FSJPEGVideoFramerSource(UsageEnvironment& env,int Track, HANDLE);
  // called only by createNew()
  virtual ~FSJPEGVideoFramerSource();

  SCODE LiveMediaSrc_JPEG_GetUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo);
  SCODE LiveMediaSrc_JPEG_ReleaseUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo);
  SCODE LiveMediaSrc_JPEG_EventHandler(HANDLE hLMSrc, ELMSrcEventType eType);


private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual u_int8_t type();
  virtual u_int8_t qFactor();
  virtual u_int8_t width();
  virtual u_int8_t height();
 virtual u_int8_t const* quantizationTables(u_int8_t& precision,
                                             u_int16_t& length);
private:
  void setParamsFromHeader();
private:
  int iTrackId;
  int NonConfBufNum;
  struct timeval fLastCaptureTime;
  u_int8_t fLastQFactor, fLastWidth, fLastHeight;
  u_int8_t fLastQuantizationTable[128];
  u_int16_t fLastQuantizationTableSize;
  bool bFirstframe ;
  bool JPEGBitStreamStart ;
  HANDLE hLMSrc;
  //unsigned char fJPEGHeader[JPEG_HEADER_SIZE];
  unsigned char fBuffer[JPEG_HEADER_SIZE];
  TLMSrcUBufInfo ptUBuffInfo ;
  uint32_t uLastFrameTimeStamp,uCurFrameTimeStamp;
  uint32_t uLastFrameSTimeStamp,uCurFrameSTimeStamp;
};

#endif
