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


/***********************************
   Headers
***********************************/
#include "LiveMediaSrc.h"
#include "global_codec.h"
#include "ubuffer.h"
#include "LiveMediaSrc_protect.h"
#include "FSH264VideoFramerSource.hh"

/***********************************
   Global variables
***********************************/
int H264Buffcnt;
extern bool StopAudio;
extern int Stop[4];
extern  UsageEnvironment*  env1    ;
extern  UsageEnvironment*  env2    ;
extern  UsageEnvironment*  env3    ;
extern  UsageEnvironment*  env4    ;
extern bool ServerStop;  //Flag for stopping server by Interrupt singnal

FSH264VideoFramerSource*
FSH264VideoFramerSource::createNew(UsageEnvironment& env,
				   int TrackID,HANDLE hndlrsrc) {
  return new FSH264VideoFramerSource(env,TrackID,hndlrsrc);
}

FSH264VideoFramerSource::FSH264VideoFramerSource(UsageEnvironment& env, int TrackID, 
                                            HANDLE hndlsrc): H264VideoSource(env),hLMSrc(hndlsrc)
{
  
  bFirstframe = true;
  fMaxOutputPacketSize = 1436;
  fNumValidDataBytes=1;
  fLastFragmentCompletedNALUnit = True; // by default
  fCurDataOffset=1;
  fInputBuffer = NULL;
  iTrackId = TrackID;
  NonConfBufNum =0;
  IFrame = False;
  H264Buffcnt = 0;
  FramedSource_TrackId = iTrackId;
  H264BitStreamStart  = False;  
}

FSH264VideoFramerSource::~FSH264VideoFramerSource() {

  if( fInputBuffer != NULL )
  {
      free(fInputBuffer);
      fInputBuffer = NULL ;
  }
  if( H264BitStreamStart == True)
  {
      LiveMediaSrc_H264_EventHandler( hLMSrc, letBitstrmStop);
      Stop[iTrackId] = 1;
  }

}

SCODE FSH264VideoFramerSource::LiveMediaSrc_H264_GetUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;
    H264Buffcnt++;
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

SCODE FSH264VideoFramerSource::LiveMediaSrc_H264_ReleaseUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

    H264Buffcnt--;
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

SCODE FSH264VideoFramerSource::LiveMediaSrc_H264_EventHandler(HANDLE hLMSrc, ELMSrcEventType eType)
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

#define MAX_H264_RETRY  10 

void FSH264VideoFramerSource::doGetNextFrame() {
    
    int iPayloadLen = 0 ;
    TUBuffer        *ptUBuffer ;
    fFrameSize = 0;
    
    // Server Stopping by Interrupt Signals
    if(ServerStop == true || Stop[iTrackId] == 1)
    {
        handleClosure(this);
        printf("Stopping H264 MediaSession......\n" ) ;
        return ;
    }
    //If IFrame we need to send sps&pps in First Packet
    //Payload Data in next packet.
    if (IFrame == True )
    {
        goto SendIFrame;
    }

    if(fNumValidDataBytes == 1)
    {
        //FirstPacket = True;
        if (bFirstframe == true)
        {
           H264BitStreamStart = True;
	   LiveMediaSrc_H264_EventHandler( hLMSrc, letBitstrmStart);
           //LiveMediaSrc_H264_EventHandler( hLMSrc,letNeedIntra );
           int iRetry = 0;
           while ( iRetry++ < MAX_H264_RETRY )
           {
               if( LiveMediaSrc_H264_GetUBuffer( hLMSrc, &ptUBuffInfo   ))
               {
	           LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                   continue ;
                   //return;
               }
               if ( ptUBuffInfo.pbyUBuffPayload != NULL )
               {
	           LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                   break ;
               }
	       LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
           }  
	   LiveMediaSrc_H264_EventHandler( hLMSrc,letNeedIntra );
        }  
        GetBuf : 
            if( LiveMediaSrc_H264_GetUBuffer( hLMSrc, &ptUBuffInfo))
            {
                printf("H264 no data......\n" ) ;
                usleep(10000);
                goto GetBuf ;
                //return;
            }
        ptUBuffer  = (TUBuffer *) ptUBuffInfo.pbyUBuffHdr ;
        if(ptUBuffer == NULL)
        {
           LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
           printf("H264 Conf Buffer is Null\n") ;
           handleClosure(this);
           return ;
        }

        if( (ptUBuffer->dwDataType == FOURCC_CONF) )
        {
 	        bFirstframe = false;
                NonConfBufNum = 0;     
                if(!(strncmp("CONFH264",(char*)ptUBuffInfo.pbyUBuffHdr+4,8)))
                {
	            bFirstframe = True;
	            LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                    usleep(10000);
                    goto GetBuf;
                }
                else
                {
	            LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                    printf("H264 CONFIG Changed for Session :%d\n",iTrackId ) ;
	            handleClosure(this);
	            return ;
                }
        }
        if( ptUBuffInfo.pbyUBuffPayload != NULL )
        {
            iPayloadLen = ptUBuffInfo.iUBuffPayloadLen ;
            if( iPayloadLen < 0 )
            {
	        LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                usleep(10000);
                if(Stop[iTrackId] == 1)
                {
                    //printf("TEst Return Doget Next Frame H264...\n");
                    return ;
                }
                goto GetBuf ;
            }
            fMaxOutputPacketSize = 1436;
	    fInputBuffer = (PBYTE) malloc( ptUBuffInfo.iUBuffPayloadLen);
            if(fInputBuffer == NULL)
	    {
                printf("Failed to allocate memory for H264 input buffer\n" ) ;
	        return ; 
	    }
            //Send IFrame as first Frame
            if( (bFirstframe == True) &&  (ptUBuffInfo.pbyUBuffHdr[28] != 1)  )
            {
	        LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
	        LiveMediaSrc_H264_EventHandler( hLMSrc,letNeedIntra );
                usleep(30000);
                goto GetBuf;
            }
                      
            //TODO: Get SPS and pps from i -frame.Read the  isync byte from buffer header.
            //ISSync bit set 
            if ( ptUBuffInfo.pbyUBuffHdr[28] == 1 )
            {
                // Send sps and pps in single packet
                IFrame = True;
                // Calculate the SPS size and copy the SPS
                ispsSize = ptUBuffInfo.pbyUBuffPayload[3]<<24 | ptUBuffInfo.pbyUBuffPayload[2]<<16 | 
                                                                ptUBuffInfo.pbyUBuffPayload[1]<<8 | ptUBuffInfo.pbyUBuffPayload[0];

                fTo[0] = 0x38; // To understand  client Its parameter packet 
                fTo[1] = 0x0;
                fTo[2] = ispsSize;

	        memcpy( fTo+3,ptUBuffInfo.pbyUBuffPayload+4,ispsSize );

                ippsSize = ptUBuffInfo.pbyUBuffPayload[3+ispsSize+4]<<24 | ptUBuffInfo.pbyUBuffPayload[2+ispsSize+4]<<16
                                           | ptUBuffInfo.pbyUBuffPayload[1+ispsSize+4]<<8 | ptUBuffInfo.pbyUBuffPayload[0+ispsSize+4];

                fTo[ispsSize+3] = 0x0;
                fTo[ispsSize+4] = ippsSize;

	        memcpy( fTo+ispsSize+5,ptUBuffInfo.pbyUBuffPayload+ispsSize+8,ippsSize );
	        fFrameSize = ispsSize +ippsSize + 5;
	        memcpy( fInputBuffer,ptUBuffInfo.pbyUBuffPayload+(ispsSize+ippsSize+12-1),ptUBuffInfo.iUBuffPayloadLen-(ispsSize+ippsSize+12-1) );
                fNumValidDataBytes = ptUBuffInfo.iUBuffPayloadLen - (ispsSize+ippsSize+12-1) ;
	        fPresentationTime.tv_sec = ptUBuffer->dwSec ; 
                fPresentationTime.tv_usec =  ptUBuffer->dwUSec; 
                goto Send;

                SendIFrame:
                    IFrame = False;
	            fDurationInMicroseconds =  40000 ;
	            goto SendFrame;
            }
            else
            {
	        memcpy( fInputBuffer,ptUBuffInfo.pbyUBuffPayload+3,ptUBuffInfo.iUBuffPayloadLen-3 );
                fNumValidDataBytes = ptUBuffInfo.iUBuffPayloadLen-3;
            }
            // Calculate the timestamp and duration
            fPresentationTime.tv_sec = ptUBuffer->dwSec ; 
            fPresentationTime.tv_usec =  ptUBuffer->dwUSec; 
	    fDurationInMicroseconds =  40000 ;

            uCurFrameSTimeStamp = ptUBuffer->dwSec ; 
            uCurFrameTimeStamp = ptUBuffer->dwUSec ;
            // Calculat the presentation time and frame duration
            if(bFirstframe == true)
            {
                //For First frame
                fDurationInMicroseconds = 40000;
                gettimeofday(&fPresentationTime, NULL);
                uLastFrameSTimeStamp = uCurFrameSTimeStamp;
                uLastFrameTimeStamp = uCurFrameTimeStamp;
            }
            if((uCurFrameSTimeStamp == uLastFrameSTimeStamp))
            {
                fDurationInMicroseconds = uCurFrameTimeStamp - uLastFrameTimeStamp;
                fPresentationTime.tv_sec = ptUBuffer->dwSec ; 
                fPresentationTime.tv_usec =  ptUBuffer->dwUSec ; 

                uLastFrameTimeStamp = uCurFrameTimeStamp;
                uLastFrameSTimeStamp = uCurFrameSTimeStamp;
            }
            else if( uCurFrameSTimeStamp == (uLastFrameSTimeStamp+1) && ( uCurFrameTimeStamp < uLastFrameTimeStamp) )
            {
                    fDurationInMicroseconds = 1000000 + uCurFrameTimeStamp - uLastFrameTimeStamp;

                    if( fDurationInMicroseconds > 200000 )
                    {
                        fDurationInMicroseconds =  200000;
                        gettimeofday(&fPresentationTime, NULL);
                        //uLastFrameSTimeStamp = fPresentationTime.tv_sec ;
                        //uLastFrameTimeStamp = fPresentationTime.tv_usec ;
                    }
                    else
                    {
                        fPresentationTime.tv_sec = ptUBuffer->dwSec ; 
                        fPresentationTime.tv_usec =  ptUBuffer->dwUSec ; 
                    }
            }
            else
            {
                 fDurationInMicroseconds =  33000;
                 gettimeofday(&fPresentationTime, NULL);
                 uLastFrameSTimeStamp = fPresentationTime.tv_sec ;
                 uLastFrameTimeStamp = fPresentationTime.tv_usec ;
            }
            uLastFrameTimeStamp = uCurFrameTimeStamp;
            uLastFrameSTimeStamp = uCurFrameSTimeStamp;

            SendFrame:
	        LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                bFirstframe = false;
                NonConfBufNum++;

        }
        else
        {
	    LiveMediaSrc_H264_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
            goto GetBuf;
        } 
    }
    if( fInputBuffer != NULL )
    {    
      fMaxSize = fMaxOutputPacketSize;
      fLastFragmentCompletedNALUnit = True; // by default
      if (fCurDataOffset == 1) 
      {
        // case 1 or 2
        if (fNumValidDataBytes - 1 <= fMaxSize ) 
        { 
            // case 1
            memmove(fTo, &fInputBuffer[1], fNumValidDataBytes - 1);
            fFrameSize = fNumValidDataBytes - 1;
            fCurDataOffset = fNumValidDataBytes;
        } 
        else 
        { 
	    fInputBuffer[0] = (fInputBuffer[1] & 0xE0) | 28; // FU indicator
	    fInputBuffer[1] = 0x80 | (fInputBuffer[1] & 0x1F); // FU header (with S bit)
	    memmove(fTo, fInputBuffer, fMaxSize);
            fFrameSize = fMaxSize;
            fCurDataOffset += fMaxSize - 1;
	    fLastFragmentCompletedNALUnit = False;
        }
      }  
      else 
      { 
          fInputBuffer[fCurDataOffset-2] = fInputBuffer[0]; 
          fInputBuffer[fCurDataOffset-1] = fInputBuffer[1]&~0x80; 
          unsigned numBytesToSend = 2 + fNumValidDataBytes - fCurDataOffset;
          if (numBytesToSend > fMaxSize) 
          {
	      //We can't send all of the remaining data this time:
	      numBytesToSend = fMaxSize;
	      fLastFragmentCompletedNALUnit = False;
          } 
          else 
          {
	      // This is the last fragment:
	      fInputBuffer[fCurDataOffset-1] |= 0x40; // set the E bit in the FU header
	      //fNumTruncatedBytes = fSaveNumTruncatedBytes;
       
          }
          memmove(fTo, &fInputBuffer[fCurDataOffset-2], numBytesToSend);
          fFrameSize = numBytesToSend;
          fCurDataOffset += numBytesToSend - 2;
      }

      if (fCurDataOffset >= fNumValidDataBytes) {
          // We're done with this data.  Reset the pointers for receiving new data:
          fNumValidDataBytes = fCurDataOffset = 1;
          free(fInputBuffer);
          fInputBuffer = NULL ;
      }
   }
    if (iTrackId == 0)
    {
            if(Stop[iTrackId] == 0)
            nextTask() = env1->taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
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

Send:
Default:	   
            // Switch to another task, and inform the reader that he has data:
	    nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                                (TaskFunc*)FramedSource::afterGetting, this);
   }
 
}

