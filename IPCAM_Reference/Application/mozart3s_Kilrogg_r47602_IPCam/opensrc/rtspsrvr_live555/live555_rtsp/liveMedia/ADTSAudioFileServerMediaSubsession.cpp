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
// on demand, from an AAC audio file in ADTS format
// Implementation

#include "ADTSAudioFileServerMediaSubsession.hh"
#include "ADTSAudioFileSource.hh"
#include "MPEG4GenericRTPSink.hh"

ADTSAudioFileServerMediaSubsession*
ADTSAudioFileServerMediaSubsession::createNew(UsageEnvironment& env,
					     char const* fileName,
					     Boolean reuseFirstSource,HANDLE handlesrc ,int samplerate,
                                             int numofchn,int profile_level,
                                             char*Configstr,int iTrackid) {

  return new ADTSAudioFileServerMediaSubsession(env, fileName, reuseFirstSource,handlesrc ,samplerate,
                                                               numofchn,profile_level,Configstr,iTrackid);
}

ADTSAudioFileServerMediaSubsession
::ADTSAudioFileServerMediaSubsession(UsageEnvironment& env,
				    char const* fileName, Boolean reuseFirstSource,HANDLE handlesrc,
                                    int samplerate,int numofchn,int profile_level,
                                    char *Configstr,int iTrackid)
  : FileServerMediaSubsession(env, fileName, reuseFirstSource),hlmsrc(handlesrc) ,Samplerate(samplerate),
                                                  NumofChn(numofchn),Profile_level(profile_level) {

     strcpy(ConfigStr,Configstr );
     ConfigStr[4] ='\0';  
   
     iTrackId = iTrackid;
}

ADTSAudioFileServerMediaSubsession
::~ADTSAudioFileServerMediaSubsession() {
}

FramedSource* ADTSAudioFileServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 96; // kbps, estimate
   
 return ADTSAudioFileSource::createNew(envir(), fFileName,hlmsrc,Samplerate,
                                                NumofChn,Profile_level,ConfigStr,iTrackId);
}

RTPSink* ADTSAudioFileServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* inputSource) {

  
  
  ADTSAudioFileSource* adtsSource = (ADTSAudioFileSource*)inputSource;
  return MPEG4GenericRTPSink::createNew(envir(), rtpGroupsock,
					rtpPayloadTypeIfDynamic,
					adtsSource->samplingFrequency(),
					"audio", "AAC-hbr", adtsSource->configStr(),
					adtsSource->numChannels(),Profile_level );
}
