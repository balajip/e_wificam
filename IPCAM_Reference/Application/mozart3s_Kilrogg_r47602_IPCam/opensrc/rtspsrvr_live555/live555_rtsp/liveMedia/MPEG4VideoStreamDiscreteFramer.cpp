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
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2013 Live Networks, Inc.  All rights reserved.
// A simplified version of "MPEG4VideoStreamFramer" that takes only complete,
// discrete frames (rather than an arbitrary byte stream) as input.
// This avoids the parsing and data copying overhead of the full
// "MPEG4VideoStreamFramer".
// Implementation

/*************************
 HEADER FILES
*************************/
#include "MPEG4VideoStreamDiscreteFramer.hh"
#include "global_codec.h"
#include "ubuffer.h"

#include "LiveMediaSrc.h"
#include "LiveMediaSrc_protect.h"

/*************************
 Global Variables
*************************/
int MP4BuffCnt;
extern  int Stop[4];
extern  UsageEnvironment*  env1    ;
extern  UsageEnvironment*  env2    ;
extern  UsageEnvironment*  env3    ;
extern  UsageEnvironment*  env4    ;
extern bool ServerStop ;  //Flag for Stopping Server by Interrupt
extern bool StopAudio;

MPEG4VideoStreamDiscreteFramer*
MPEG4VideoStreamDiscreteFramer::createNew(UsageEnvironment& env,
				  FramedSource* inputSource, Boolean leavePresentationTimesUnmodified,HANDLE hndlr,int TrackID) {
  // Need to add source type checking here???  #####
  return new MPEG4VideoStreamDiscreteFramer(env, inputSource, leavePresentationTimesUnmodified,hndlr,TrackID);
}

MPEG4VideoStreamDiscreteFramer
::MPEG4VideoStreamDiscreteFramer(UsageEnvironment& env,
				 FramedSource* inputSource, Boolean leavePresentationTimesUnmodified,HANDLE hndlr,int TrackID)
  : MPEG4VideoStreamFramer(env, inputSource, False/*don't create a parser*/),
    fLeavePresentationTimesUnmodified(leavePresentationTimesUnmodified), vop_time_increment_resolution(0), fNumVTIRBits(0),
    fLastNonBFrameVop_time_increment(0) ,hLMSrc(hndlr),iTrackId(TrackID) {

  fLastNonBFramePresentationTime.tv_sec = 0;
  fLastNonBFramePresentationTime.tv_usec = 0;
  bFirstframe = true;
  NonConfBufNum = 0;
  MP4BuffCnt = 0;
  Stop[iTrackId] = 0;
  FramedSource_TrackId = iTrackId;
  //Destructor = False; 
  MP4BitStreamStart = False;
}

MPEG4VideoStreamDiscreteFramer::~MPEG4VideoStreamDiscreteFramer() {

  if( MP4BitStreamStart == True )
  {  
      LiveMediaSrc_MPEG4_EventHandler( hLMSrc, letBitstrmStop);
      Stop[iTrackId] = 1;
  }
}

SCODE MPEG4VideoStreamDiscreteFramer::LiveMediaSrc_MPEG4_GetUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

     MP4BuffCnt++;
    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfGetUBuf )
    {
        // Call to media source to get the data/conf buffer
        iRet = (phLiveVSrc->ptCBMap->pfGetUBuf)(hLMSrc, ptUBuffInfo); 
    }

    return  iRet ;
}

SCODE MPEG4VideoStreamDiscreteFramer::LiveMediaSrc_MPEG4_ReleaseUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

    MP4BuffCnt--;
    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfReleaseUBuf)
    {
        // Call to media source to get the data/conf buffer
        iRet = (phLiveVSrc->ptCBMap->pfReleaseUBuf)(hLMSrc, ptUBuffInfo);
    }

    return  iRet ;
}

SCODE MPEG4VideoStreamDiscreteFramer::LiveMediaSrc_MPEG4_EventHandler(HANDLE hLMSrc, ELMSrcEventType eType)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfEvntHandler)
    {
        // Call to media source to handle the events
        iRet = ( phLiveVSrc->ptCBMap->pfEvntHandler)(hLMSrc, eType); 
    }

    return  iRet ;
}

#define MAX_MP4_RETRY  10 

void MPEG4VideoStreamDiscreteFramer::doGetNextFrame() 
{  
  int iPayloadLen = 0 ;
  fFrameSize =0;
  TUBuffer        *ptUBuffer ;

   //Server Stopping by Interrupt
   if(ServerStop == true || Stop[iTrackId] == 1)
   {
         handleClosure(this);
         printf("Stoping MPEG4 Media Session.....\n" ) ;
         return ;
   }

   if (bFirstframe == true)
   {
        MP4BitStreamStart = True;
        LiveMediaSrc_MPEG4_EventHandler( hLMSrc, letBitstrmStart);
        LiveMediaSrc_MPEG4_EventHandler( hLMSrc,letNeedIntra );
    }  

GetBuf : 
    if( LiveMediaSrc_MPEG4_GetUBuffer( hLMSrc, &ptUBuffInfo))
    {
       printf("MPEG4 no data......\n" ) ;
       usleep(10000);
       goto GetBuf ;
       //return;
    }
       
    ptUBuffer  = (TUBuffer *) ptUBuffInfo.pbyUBuffHdr ;
    if( (ptUBuffer->dwDataType == FOURCC_CONF) )
    {
         bFirstframe = false;
         NonConfBufNum = 0;
         printf("MP4V:%s\n",(char*)ptUBuffInfo.pbyUBuffHdr+4); 
         if(!(strncmp("CONFMP4V",(char*)ptUBuffInfo.pbyUBuffHdr+4,8)))
         {
            //bFirstframe = True;
	    LiveMediaSrc_MPEG4_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
            usleep(30000);
            goto GetBuf;
         }
         else
         {
	    LiveMediaSrc_MPEG4_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
	    printf("MP4V CONFIG Changed for Session:%d\n",iTrackId ) ;
            handleClosure(this);
	    return ;
         }
    }
  
   if ( ptUBuffInfo.pbyUBuffPayload != NULL )
   {  
      iPayloadLen = ptUBuffInfo.iUBuffPayloadLen;
      if( iPayloadLen > 0 )
      {
          fFrameSize = iPayloadLen ;
          if (fFrameSize < fMaxSize) 
          memcpy( fTo,ptUBuffInfo.pbyUBuffPayload,ptUBuffInfo.iUBuffPayloadLen );
          else
          memcpy( fTo,ptUBuffInfo.pbyUBuffPayload,fMaxSize );
      }
      else
      {
          LiveMediaSrc_MPEG4_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
          usleep(10000);
          if(Stop[iTrackId] == 1)
          {
              //printf("TEst Return Doget Next Frame MPEG4...\n");
              return ; 
          }
 
          goto GetBuf ;
      }
       // Calculate the timestamp and duration
       fPresentationTime.tv_sec = ptUBuffer->dwSec ; 
       fPresentationTime.tv_usec =  ptUBuffer->dwUSec ; 
       fDurationInMicroseconds =  33000 ;
       bFirstframe = false;
       NonConfBufNum++;
       LiveMediaSrc_MPEG4_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
   }
   else
   {
       LiveMediaSrc_MPEG4_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
       goto GetBuf;
   }
   if (fFrameSize < fMaxSize)
   afterGettingFrame(this,fFrameSize,0,fPresentationTime,fDurationInMicroseconds);
   else
   {
       fNumTruncatedBytes = fFrameSize - fMaxSize;
       fFrameSize = fMaxSize;      
       afterGettingFrame(this,fMaxSize,fNumTruncatedBytes,fPresentationTime,fDurationInMicroseconds);
   }
}

void MPEG4VideoStreamDiscreteFramer
::afterGettingFrame(void* clientData, unsigned frameSize,
                    unsigned numTruncatedBytes,
                    struct timeval presentationTime,
                    unsigned durationInMicroseconds) {
  MPEG4VideoStreamDiscreteFramer* source = (MPEG4VideoStreamDiscreteFramer*)clientData;
  source->afterGettingFrame1(frameSize, numTruncatedBytes,
                             presentationTime, durationInMicroseconds);
}

void MPEG4VideoStreamDiscreteFramer
::afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
                     struct timeval presentationTime,
                     unsigned durationInMicroseconds) {
  // Check that the first 4 bytes are a system code:
  if (frameSize >= 4 && fTo[0] == 0 && fTo[1] == 0 && fTo[2] == 1) {
    fPictureEndMarker = True; // Assume that we have a complete 'picture' here
    unsigned i = 3;
    if (fTo[i] == 0xB0) { // VISUAL_OBJECT_SEQUENCE_START_CODE
      // The next byte is the "profile_and_level_indication":
      if (frameSize >= 5) fProfileAndLevelIndication = fTo[4];

      // The start of this frame - up to the first GROUP_VOP_START_CODE
      // or VOP_START_CODE - is stream configuration information.  Save this:
      for (i = 7; i < frameSize; ++i) {
	if ((fTo[i] == 0xB3 /*GROUP_VOP_START_CODE*/ ||
	     fTo[i] == 0xB6 /*VOP_START_CODE*/)
	    && fTo[i-1] == 1 && fTo[i-2] == 0 && fTo[i-3] == 0) {
	  break; // The configuration information ends here
	}
      }
      fNumConfigBytes = i < frameSize ? i-3 : frameSize;
      delete[] fConfigBytes; fConfigBytes = new unsigned char[fNumConfigBytes];
      for (unsigned j = 0; j < fNumConfigBytes; ++j) fConfigBytes[j] = fTo[j];

      // This information (should) also contain a VOL header, which we need
      // to analyze, to get "vop_time_increment_resolution" (which we need
      // - along with "vop_time_increment" - in order to generate accurate
      // presentation times for "B" frames).
      analyzeVOLHeader();
    }

    if (i < frameSize) {
      u_int8_t nextCode = fTo[i];

      if (nextCode == 0xB3 /*GROUP_VOP_START_CODE*/) {
	// Skip to the following VOP_START_CODE (if any):
	for (i += 4; i < frameSize; ++i) {
	  if (fTo[i] == 0xB6 /*VOP_START_CODE*/
	      && fTo[i-1] == 1 && fTo[i-2] == 0 && fTo[i-3] == 0) {
	    nextCode = fTo[i];
	    break;
	  }
	}
      }

      if (nextCode == 0xB6 /*VOP_START_CODE*/ && i+5 < frameSize) {
	++i;

	// Get the "vop_coding_type" from the next byte:
	u_int8_t nextByte = fTo[i++];
	u_int8_t vop_coding_type = nextByte>>6;

	// Next, get the "modulo_time_base" by counting the '1' bits that
	// follow.  We look at the next 32-bits only.
	// This should be enough in most cases.
	u_int32_t next4Bytes
	  = (fTo[i]<<24)|(fTo[i+1]<<16)|(fTo[i+2]<<8)|fTo[i+3];
	i += 4;
	u_int32_t timeInfo = (nextByte<<(32-6))|(next4Bytes>>6);
	unsigned modulo_time_base = 0;
	u_int32_t mask = 0x80000000;
	while ((timeInfo&mask) != 0) {
	  ++modulo_time_base;
	  mask >>= 1;
	}
	mask >>= 2;

	// Then, get the "vop_time_increment".
	unsigned vop_time_increment = 0;
	// First, make sure we have enough bits left for this:
	if ((mask>>(fNumVTIRBits-1)) != 0) {
	  for (unsigned i = 0; i < fNumVTIRBits; ++i) {
	    vop_time_increment |= timeInfo&mask;
	    mask >>= 1;
	  }
	  while (mask != 0) {
	    vop_time_increment >>= 1;
	    mask >>= 1;
	  }
	}

	// If this is a "B" frame, then we have to tweak "presentationTime":
	if (!fLeavePresentationTimesUnmodified && vop_coding_type == 2/*B*/
	    && (fLastNonBFramePresentationTime.tv_usec > 0 ||
		fLastNonBFramePresentationTime.tv_sec > 0)) {
	  int timeIncrement
	    = fLastNonBFrameVop_time_increment - vop_time_increment;
	  if (timeIncrement<0) timeIncrement += vop_time_increment_resolution;
	  unsigned const MILLION = 1000000;
	  double usIncrement = vop_time_increment_resolution == 0 ? 0.0
	    : ((double)timeIncrement*MILLION)/vop_time_increment_resolution;
	  unsigned secondsToSubtract = (unsigned)(usIncrement/MILLION);
	  unsigned uSecondsToSubtract = ((unsigned)usIncrement)%MILLION;

	  presentationTime = fLastNonBFramePresentationTime;
	  if ((unsigned)presentationTime.tv_usec < uSecondsToSubtract) {
	    presentationTime.tv_usec += MILLION;
	    if (presentationTime.tv_sec > 0) --presentationTime.tv_sec;
	  }
	  presentationTime.tv_usec -= uSecondsToSubtract;
	  if ((unsigned)presentationTime.tv_sec > secondsToSubtract) {
	    presentationTime.tv_sec -= secondsToSubtract;
	  } else {
	    presentationTime.tv_sec = presentationTime.tv_usec = 0;
	  }
	} else {
	  fLastNonBFramePresentationTime = presentationTime;
	  fLastNonBFrameVop_time_increment = vop_time_increment;
	}
      }
    }
  }

  // Complete delivery to the client:
  fFrameSize = frameSize;
  if (fFrameSize > fMaxSize)
  {
      fNumTruncatedBytes = fFrameSize - fMaxSize;
      fFrameSize = fMaxSize; 
  } 
  fNumTruncatedBytes = numTruncatedBytes;
  fPresentationTime = presentationTime;
  fDurationInMicroseconds = durationInMicroseconds;

    if (iTrackId == 0)
    {
            if(Stop[iTrackId] == 0)
            nextTask() = env1->taskScheduler().scheduleDelayedTask(0, (TaskFunc*)afterGetting, this);
            else
              goto Default;
    }
    else if(iTrackId == 1)
    {
            if(Stop[iTrackId ] == 0)
            nextTask() = env2->taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else if(iTrackId == 2)
    {
            if(Stop[iTrackId ] == 0)
            nextTask() = env3->taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else if(iTrackId == 3)
    {
            if(Stop[iTrackId] == 0)
            nextTask() = env4->taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else
    {
Default:	   
      afterGetting(this);
   }
}

Boolean MPEG4VideoStreamDiscreteFramer::getNextFrameBit(u_int8_t& result) {
  if (fNumBitsSeenSoFar/8 >= fNumConfigBytes) return False;

  u_int8_t nextByte = fConfigBytes[fNumBitsSeenSoFar/8];
  result = (nextByte>>(7-fNumBitsSeenSoFar%8))&1;
  ++fNumBitsSeenSoFar;
  return True;
}

Boolean MPEG4VideoStreamDiscreteFramer::getNextFrameBits(unsigned numBits,
							 u_int32_t& result) {
  result = 0;
  for (unsigned i = 0; i < numBits; ++i) {
    u_int8_t nextBit;
    if (!getNextFrameBit(nextBit)) return False;
    result = (result<<1)|nextBit;
  }
  return True;
}

void MPEG4VideoStreamDiscreteFramer::analyzeVOLHeader() {
  // Begin by moving to the VOL header:
  unsigned i;
  for (i = 3; i < fNumConfigBytes; ++i) {
    if (fConfigBytes[i] >= 0x20 && fConfigBytes[i] <= 0x2F
	&& fConfigBytes[i-1] == 1
	&& fConfigBytes[i-2] == 0 && fConfigBytes[i-3] == 0) {
      ++i;
      break;
    }
  }

  fNumBitsSeenSoFar = 8*i + 9;
  do {
    u_int8_t is_object_layer_identifier;
    if (!getNextFrameBit(is_object_layer_identifier)) break;
    if (is_object_layer_identifier) fNumBitsSeenSoFar += 7;

    u_int32_t aspect_ratio_info;
    if (!getNextFrameBits(4, aspect_ratio_info)) break;
    if (aspect_ratio_info == 15 /*extended_PAR*/) fNumBitsSeenSoFar += 16;

    u_int8_t vol_control_parameters;
    if (!getNextFrameBit(vol_control_parameters)) break;
    if (vol_control_parameters) {
      fNumBitsSeenSoFar += 3; // chroma_format; low_delay
      u_int8_t vbw_parameters;
      if (!getNextFrameBit(vbw_parameters)) break;
      if (vbw_parameters) fNumBitsSeenSoFar += 79;
    }

    fNumBitsSeenSoFar += 2; // video_object_layer_shape
    u_int8_t marker_bit;
    if (!getNextFrameBit(marker_bit)) break;
    if (marker_bit != 1) break; // sanity check

    if (!getNextFrameBits(16, vop_time_increment_resolution)) break;
    if (vop_time_increment_resolution == 0) break; // shouldn't happen

    // Compute how many bits are necessary to represent this:
    fNumVTIRBits = 0;
    for (unsigned test = vop_time_increment_resolution; test>0; test /= 2) {
      ++fNumVTIRBits;
    }
  } while (0);
}
