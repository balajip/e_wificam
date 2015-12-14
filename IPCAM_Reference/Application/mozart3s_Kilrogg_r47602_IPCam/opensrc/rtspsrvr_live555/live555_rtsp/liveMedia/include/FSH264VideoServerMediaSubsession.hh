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
// Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
// A 'ServerMediaSubsession' object that creates new, unicast, "RTPSink"s

// C++ header

#ifndef _FSH264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _FSH264_VIDEO_SERVER_MEDIA_SUBSESSION_HH

#ifndef _FILE_SERVER_MEDIA_SUBSESSION_HH
#include "FileServerMediaSubsession.hh"
#endif
/*********************************************************
                   HEADER
***********************************************************/
#include "LiveMediaSrc.h"

class FSH264VideoServerMediaSubsession: public FileServerMediaSubsession{
public:
  static FSH264VideoServerMediaSubsession* createNew(UsageEnvironment& env, 
                                                     int Trackid, 
                                                     Boolean reuseFirstSource,
                                                     u_int8_t const* sps,int sps_size,
                                                     u_int8_t const*pps,
                                                     int pps_size,HANDLE);
  
 //static FSH264VideoServerMediaSubsession* createNew(UsageEnvironment& env,int Track, 
 //                                                       Boolean reuseFirstSource);
protected:
  FSH264VideoServerMediaSubsession(UsageEnvironment& env,
			            int Track, Boolean reuseFirstSource,
                                   u_int8_t const* sps=NULL,int sps_size=0,
                                   u_int8_t const*pps = NULL,int pps_size=0,
                                   HANDLE  SrcHndlr = NULL);
      // called only by createNew();
  virtual ~FSH264VideoServerMediaSubsession();

private: // redefined virtual functions
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate );
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				                    FramedSource* inputSource);
private: 
    u_int8_t *sps,*pps;
    int  sps_size,pps_size;
    HANDLE SrcHndlr;
    int iTrackId;
};

#endif
