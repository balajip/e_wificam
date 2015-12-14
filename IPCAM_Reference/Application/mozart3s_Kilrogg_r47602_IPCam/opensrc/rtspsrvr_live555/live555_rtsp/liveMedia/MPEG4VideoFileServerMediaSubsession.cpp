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
// A 'ServerMediaSubsession' object that creates new, unicast, "RTPSink"s
// on demand, from a MPEG-4 video file.
// Implementation

#include "MPEG4VideoFileServerMediaSubsession.hh"
#include "MPEG4ESVideoRTPSink.hh"
#include "MPEG4VideoStreamFramer.hh"
#include "MPEG4VideoStreamDiscreteFramer.hh"

MPEG4VideoFileServerMediaSubsession*
MPEG4VideoFileServerMediaSubsession::createNew(UsageEnvironment& env,
					       int TrackID,
					       Boolean reuseFirstSource,
                                               u_int8_t profileid,
                                               char const * configstr, 
                                               int configsize,
                                               HANDLE Srchndlr ) {
  return new MPEG4VideoFileServerMediaSubsession(env, TrackID, reuseFirstSource,profileid,configstr,configsize, Srchndlr);
}

MPEG4VideoFileServerMediaSubsession
::MPEG4VideoFileServerMediaSubsession(UsageEnvironment& env,
                                      int Trackid, Boolean reuseFirstSource,
                                       u_int8_t profileid,
                                       char const* configstr, 
                                       int configsize,HANDLE Srchndlr)
  : FileServerMediaSubsession(env, NULL, reuseFirstSource),iTrackId(Trackid),
    fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL),profileId(profileid),hLMVSrc (Srchndlr) {
    if(configstr != NULL)
   {
     Configstr = (char*)malloc(configsize);
     if(Configstr == NULL)
     {
         printf("Memmory not allocated..\n");
         return;
     }
     ConfigSize = configsize;
     memmove(Configstr,configstr,configsize);    
   }
}

MPEG4VideoFileServerMediaSubsession::~MPEG4VideoFileServerMediaSubsession() {
  delete[] fAuxSDPLine;
}

static void afterPlayingDummy(void* clientData) {
  MPEG4VideoFileServerMediaSubsession* subsess
    = (MPEG4VideoFileServerMediaSubsession*)clientData;
  subsess->afterPlayingDummy1();
}

void MPEG4VideoFileServerMediaSubsession::afterPlayingDummy1() {
  // Unschedule any pending 'checking' task:
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
  // Signal the event loop that we're done:
  setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData) {
  MPEG4VideoFileServerMediaSubsession* subsess
    = (MPEG4VideoFileServerMediaSubsession*)clientData;
  subsess->checkForAuxSDPLine1();
}

void MPEG4VideoFileServerMediaSubsession::checkForAuxSDPLine1() {
  char const* dasl;
 
  if (fAuxSDPLine != NULL) {
    // Signal the event loop that we're done:
    setDoneFlag();
  } else if (fDummyRTPSink != NULL && (dasl = fDummyRTPSink->auxSDPLine()) != NULL) {
    fAuxSDPLine= strDup(dasl);
    fDummyRTPSink = NULL;

    // Signal the event loop that we're done:
    setDoneFlag();
  } else {
    // try again after a brief delay:
    int uSecsToDelay = 100000; // 100 ms
    nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)checkForAuxSDPLine, this);
  }
}

char const* MPEG4VideoFileServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
  if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

  #if 1
  if (fDummyRTPSink == NULL) { // we're not already setting it up for another, concurrent stream
    // Note: For MPEG-4 video files, the 'config' information isn't known
    // until we start reading the file.  This means that "rtpSink"s
    // "auxSDPLine()" will be NULL initially, and we need to start reading data from our file until this changes.
    fDummyRTPSink = rtpSink;

    //fDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);
    // Check whether the sink's 'auxSDPLine()' is ready:
    checkForAuxSDPLine(this);
  }
 #endif
  envir().taskScheduler().doEventLoop(&fDoneFlag);
  return fAuxSDPLine;
}

FramedSource* MPEG4VideoFileServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 500; // kbps, estimate
  //estBitrate = 56; // kbps, estimate
  // Create a framer for the Video Elementary Stream:
  return MPEG4VideoStreamDiscreteFramer::createNew( envir(),NULL, true,hLMVSrc,iTrackId);
}

RTPSink* MPEG4VideoFileServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  return MPEG4ESVideoRTPSink::createNew(envir(), rtpGroupsock,
					rtpPayloadTypeIfDynamic,30000,profileId,ConfigSize ,Configstr);
}
