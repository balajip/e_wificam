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

#ifndef _FS_H264_VIDEO_FRAMER_SOURCE_HH
#define _FS_H264_VIDEO_FRAMER_SOURCE_HH

 #ifndef _H264_VIDEO_SOURCE_HH
#include "H264VideoSource.hh"
#include <stdint.h>
#include "FSH264VideoServerMediaSubsession.hh"
#include "H264VideoStreamFramer.hh"

#include "global_codec.h"
#include "ubuffer.h"
/*********************************************************
                    HEADER
***********************************************************/
#include "LiveMediaSrc.h"
#include "liveMedia.hh"
 /************************************************************
                MACROS
*************************************************************/
#define SPSSIZE 50
#define PPSSIZE  SPSSIZE

#endif

class FSH264VideoFramerSource : public H264VideoSource {
public:
  static FSH264VideoFramerSource* createNew(UsageEnvironment& env,
					    int,HANDLE);

protected:
  FSH264VideoFramerSource(UsageEnvironment& env,int,HANDLE );
  // called only by createNew()
  virtual ~FSH264VideoFramerSource();

  SCODE LiveMediaSrc_H264_GetUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo);
  SCODE LiveMediaSrc_H264_ReleaseUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo);
  SCODE LiveMediaSrc_H264_EventHandler(HANDLE hLMSrc, ELMSrcEventType eType);

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

private:
   int iTrackId;
   int NonConfBufNum;  
  struct timeval fLastCaptureTime;
  
  bool bFirstframe;
  bool fLastFragmentCompletedNALUnit ; // by default
  int fMaxOutputPacketSize;
  int fNumValidDataBytes,fCurDataOffset;
  PBYTE fInputBuffer ;
  
  bool H264BitStreamStart;

  uint32_t uLastFrameTimeStamp;
  uint32_t uLastFrameSTimeStamp;
  uint32_t uCurFrameTimeStamp;
  uint32_t uCurFrameSTimeStamp;

  bool IFrame;
  int ispsSize,ippsSize; 
  uint8_t sps[SPSSIZE],pps[PPSSIZE];
      
  HANDLE hShrdBufSrc,hLMSrc;     
  TLMSrcUBufInfo ptUBuffInfo ;  

};

#endif
