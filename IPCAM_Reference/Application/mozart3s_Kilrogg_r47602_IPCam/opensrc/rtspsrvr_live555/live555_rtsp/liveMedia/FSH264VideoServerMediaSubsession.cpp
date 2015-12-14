/*
 * Copyright (C) 2005-2006 WIS Technologies International Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and the associated README documentation file (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "FSH264VideoServerMediaSubsession.hh"
#include "H264VideoRTPSink.hh"
#include "FSH264VideoFramerSource.hh"

FSH264VideoServerMediaSubsession* FSH264VideoServerMediaSubsession 
::createNew(UsageEnvironment& env, int TrackID, Boolean  reuseFirstSource, 
            u_int8_t const* Sps,int Sps_Size, 
            u_int8_t const * Pps,int Pps_Size,HANDLE hlmsrc) {

  if(Sps != NULL && Pps != NULL)
  {
      return new FSH264VideoServerMediaSubsession(env, TrackID, reuseFirstSource,Sps,Sps_Size,Pps,Pps_Size,hlmsrc);
  }
  else
  {
    return NULL;
  }

}

FSH264VideoServerMediaSubsession
::FSH264VideoServerMediaSubsession(UsageEnvironment& env, int TrackID,
				     Boolean reuseFirstSource,u_int8_t const * Sps,int Sps_Size, u_int8_t const * Pps,int Pps_Size,HANDLE hlmsrc)
  : FileServerMediaSubsession(env,NULL,reuseFirstSource), SrcHndlr(hlmsrc),iTrackId(TrackID)
{
    sps = new u_int8_t[Sps_Size];
    pps = new u_int8_t[Pps_Size];
    memmove(sps,Sps,sps_size =Sps_Size);
    memmove(pps,Pps, pps_size = Pps_Size);
}


FramedSource* FSH264VideoServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) 
{
    estBitrate = 500; // kbps, estimate
    // Create the video source:
    return  FSH264VideoFramerSource::createNew(envir() , iTrackId,SrcHndlr );
}

RTPSink* FSH264VideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char /*rtpPayloadTypeIfDynamic*/,
		   FramedSource* /*inputSource*/) {
  
  return H264VideoRTPSink::createNew(envir(), rtpGroupsock, 96,sps,sps_size,pps,pps_size);
}



FSH264VideoServerMediaSubsession:: ~FSH264VideoServerMediaSubsession() {

}
