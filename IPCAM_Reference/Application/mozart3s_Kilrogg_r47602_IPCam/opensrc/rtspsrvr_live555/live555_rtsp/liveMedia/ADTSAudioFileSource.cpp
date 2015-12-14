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
// A source object for AAC audio files in ADTS format
// Implementation

#include "ADTSAudioFileSource.hh"
#include <GroupsockHelper.hh>
#include "RTSPServer.hh"
#include "global_codec.h"
#include "ubuffer.h"
#include "LiveMediaSrc.h"
#include "LiveMediaSrc_protect.h"

int ADTSBuffCnt;
//extern int SessionReconfig[4];
////////// ADTSAudioFileSource //////////

extern int Stop[4];
extern  UsageEnvironment*  env1    ;
extern  UsageEnvironment*  env2    ;
extern  UsageEnvironment*  env3    ;
extern  UsageEnvironment*  env4    ;
bool StopAudio;
bool SessionReconfig;
extern bool ServerStop ;  //Flag for stopping Server by Inturrpt

ADTSAudioFileSource*
ADTSAudioFileSource::createNew(UsageEnvironment& env, char const* fileName,HANDLE hlmsrc, 
                               int samplerate,u_int8_t  numofchn,u_int8_t profile,
                               char *configstr,int iTrackid) {

    return new ADTSAudioFileSource(env, NULL,hlmsrc, samplerate,numofchn 
                                                     ,profile,configstr,iTrackid);
}

ADTSAudioFileSource
::ADTSAudioFileSource(UsageEnvironment& env, FILE* fid,HANDLE hlmsrc, u_int32_t samplerate,
		      u_int8_t numofchn, u_int8_t profile,char * configstr,int iTrackid)
  : FramedFileSource(env, fid),hLMSrc(hlmsrc){

   fSamplingFrequency = samplerate;
   fNumChannels = numofchn;

   strcpy(fConfigStr,configstr );
   fConfigStr[4] ='\0';  
   iTrackId = iTrackid;
   fuSecsPerFrame
   = (1024/*samples-per-frame*/*1000000) / fSamplingFrequency/*samples-per-second*/;
    NonConfBufNum = 0;
    bFirstframe = true ;
    ADTSBuffCnt = 0; 
    Stop[iTrackId] = 0; 
    FramedSource_TrackId = iTrackId;
    StopAudio = False;
    SessionReconfig = False;
    AACBitStreamStart = False;
        
}

ADTSAudioFileSource::~ADTSAudioFileSource() {

  if (AACBitStreamStart == True)
  {
    LiveMediaSrc_EventHandler( hLMSrc, letBitstrmStop);
    Stop[iTrackId] = 1; 
  }
  
}

void ADTSAudioFileSource::doGetNextFrame() {
   
 if (StopAudio != True  ) 
 {
    TUBuffer        *ptUBuffer ;

    int iPayloadLen = 0 ;
    fFrameSize =0;

    //Server Stopping By Intrrupt signal
    if(ServerStop == true || Stop[iTrackId] == 1)
    {
           handleClosure(this);
           printf("Stoping AAC Audio MediaSession......\n" ) ;
           return ;
    }

    if (bFirstframe == true)
    {
NeedIntra:
	AACBitStreamStart = True;
        LiveMediaSrc_EventHandler( hLMSrc, letBitstrmStart);
        LiveMediaSrc_EventHandler( hLMSrc,letNeedIntra );

        // Tryout for 10 times to get the media data
        int iRetry = 0;
        while ( iRetry++ < 10 )
        {  
               if( LiveMediaSrc_GetUBuffer( hLMSrc, &ptUBuffInfo   ))
               {
	           LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                   return;
               }
               if ( ptUBuffInfo.pbyUBuffPayload != NULL )
               {
                   NonConfBufNum++;
	           LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                   break ;
               }
	       LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
        }  
   }   

GetBuf : 
    if( LiveMediaSrc_GetUBuffer( hLMSrc, &ptUBuffInfo))
    {
       //return;
    }
    ptUBuffer  = (TUBuffer *) ptUBuffInfo.pbyUBuffHdr ;
#if 1
    if( (ptUBuffer->dwDataType == FOURCC_CONF ) && (NonConfBufNum > 3))
    {
	LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
        StopAudio = True;
        usleep(10000);
        goto out;
    }
#endif
    if ( ptUBuffInfo.pbyUBuffPayload != NULL )
    {

        iPayloadLen = ptUBuffInfo.iUBuffPayloadLen ;

        if( iPayloadLen < 0 )
        {
	    LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
            usleep(10000);
            if(Stop[iTrackId] == 1)
            {
               //printf("Stopping ADTS Audio doGet Next... \n"); 
               return ;
            }
            goto GetBuf ;
        }
         
        // Copy the data to the output buffer
        memcpy( fTo,ptUBuffInfo.pbyUBuffPayload, iPayloadLen);
        fFrameSize = iPayloadLen ;

        bFirstframe = false ;
        NonConfBufNum++;
        LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
        ADTSBuffCnt--;
   }
   else
   {
       LiveMediaSrc_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
       printf("No AAC data");
       usleep(10000);
       goto NeedIntra;
   }

   fPresentationTime.tv_sec = ptUBuffer->dwSec ;
   fPresentationTime.tv_usec =  ptUBuffer->dwUSec ;
   fDurationInMicroseconds =  fuSecsPerFrame ;

#if 1
    if (iTrackId == 0)
    {
            if(Stop[iTrackId] == 0)
            {
             nextTask() = env1->taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
            }
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
            if(Stop[iTrackId] == 0)
            nextTask() = env3->taskScheduler().scheduleDelayedTask(fDurationInMicroseconds, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else if(iTrackId == 3)
    {
            if(Stop[iTrackId] == 0)
            nextTask() = env2->taskScheduler().scheduleDelayedTask(fDurationInMicroseconds, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else if(iTrackId == 2)
    {
            if(Stop[iTrackId] == 0)
            nextTask() = env3->taskScheduler().scheduleDelayedTask(fDurationInMicroseconds, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else if(iTrackId == 3)
    {
            if(Stop[iTrackId] == 0)
            nextTask() = env4->taskScheduler().scheduleDelayedTask(fDurationInMicroseconds, (TaskFunc*)FramedSource::afterGetting, this);
            else
              goto Default;
    }
    else
#endif
    {
        Default:
            nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
    }
 }
 out:
   usleep(10000);
   
}
