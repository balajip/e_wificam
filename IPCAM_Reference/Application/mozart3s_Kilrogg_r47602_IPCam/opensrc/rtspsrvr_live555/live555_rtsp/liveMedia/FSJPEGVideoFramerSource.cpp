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

/****************************************
      HEADER 
******************************************/
#include "FSJPEGVideoFramerSource.hh"
#include "liveMedia.hh"
#include "RTSPServer.hh"

#include "global_codec.h"
#include "ubuffer.h"
#include "LiveMediaSrc.h"
#include "LiveMediaSrc_protect.h"

/****************************************
      Global Variables
******************************************/
int JPEGBuffcnt;
extern int Stop[4];
extern bool StopAudio;
extern  UsageEnvironment*  env1    ;
extern  UsageEnvironment*  env2    ;
extern  UsageEnvironment*  env3    ;
extern  UsageEnvironment*  env4    ;
extern bool ServerStop ;  //Flag for stopping Server by Inturrpt


FSJPEGVideoFramerSource*
FSJPEGVideoFramerSource::createNew(UsageEnvironment& env,
				   int TrackID ,HANDLE hLmsrc) {

  return new FSJPEGVideoFramerSource(env,TrackID,hLmsrc);
}

FSJPEGVideoFramerSource
::FSJPEGVideoFramerSource(UsageEnvironment& env,int TrackID ,HANDLE hLmsrc)
  : JPEGVideoSource(env),iTrackId(TrackID),hLMSrc( hLmsrc) {

  bFirstframe = true;
  NonConfBufNum =  0;
  FramedSource_TrackId = iTrackId;
  Stop[iTrackId] = 0; 
  setCodecType(CODEC_TYPE_MJPG) ; //Added to set codecType 
  JPEGBitStreamStart = False; 
}

FSJPEGVideoFramerSource::~FSJPEGVideoFramerSource() {

  if( JPEGBitStreamStart == True )
  {
      LiveMediaSrc_JPEG_EventHandler( hLMSrc, letBitstrmStop);
      Stop[iTrackId] = 1; 
  }
}

SCODE FSJPEGVideoFramerSource::LiveMediaSrc_JPEG_GetUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;
    JPEGBuffcnt++;
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

SCODE FSJPEGVideoFramerSource::LiveMediaSrc_JPEG_ReleaseUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;
    JPEGBuffcnt--;

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

SCODE FSJPEGVideoFramerSource::LiveMediaSrc_JPEG_EventHandler(HANDLE hLMSrc, ELMSrcEventType eType)
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


void FSJPEGVideoFramerSource::doGetNextFrame() 
{
    int iPayloadLen = 0 ;
    fFrameSize =0;
    TUBuffer        *ptUBuffer ;

    //Server Stopping By Intrrupt Singnal
    if(ServerStop == true || Stop[iTrackId] == 1)
    {
           handleClosure(this);
           printf("Stopping JPEG MediaSession......\n" ) ;
           return ;
    }
    if (bFirstframe == true)
    {
        NeedIntra:
            JPEGBitStreamStart = True; //Set Flag True
            LiveMediaSrc_JPEG_EventHandler( hLMSrc, letBitstrmStart);
            LiveMediaSrc_JPEG_EventHandler( hLMSrc,letNeedIntra );
                  
            // Tryout for 10 times to get the media data
            int iRetry = 0;
            while ( iRetry++ < 10 )
            {
                if( LiveMediaSrc_JPEG_GetUBuffer( hLMSrc, &ptUBuffInfo   ))
                {
	            LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                    continue ;
                }
                if ( ptUBuffInfo.pbyUBuffPayload != NULL )
                {
	            LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                    break ;
                }
	        LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
            }  
    }
  
    GetBuf : 
        if( LiveMediaSrc_JPEG_GetUBuffer( hLMSrc, &ptUBuffInfo))
        {
           printf("JPEG no data......\n" ) ;
           usleep(10000);
           goto GetBuf ;
           //return;
        }
 
        // Check for any configuration changes on this Track 
        ptUBuffer  = (TUBuffer *) ptUBuffInfo.pbyUBuffHdr ;
        if( (ptUBuffer->dwDataType == FOURCC_CONF) )
        {
            bFirstframe = false;
            StopAudio = True;
            NonConfBufNum = 0; 
            //If any Resloution Height and width
            if(!(strncmp("CONFJPEG",(char*)ptUBuffInfo.pbyUBuffHdr+4,8)))
            {
	        LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                usleep(20000);
                goto GetBuf;
            }
            else
            {
                Reconfig:
	            LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                    handleClosure(this);
                    printf("JPEG CONFIG Changed......%d\n",NonConfBufNum ) ;
                    return ;
            }
        }
        if ( ptUBuffInfo.pbyUBuffPayload != NULL )
        {
            iPayloadLen = ptUBuffInfo.iUBuffPayloadLen ;
            if( iPayloadLen < 0 )
            {
	        LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
                usleep(10000);
                if(Stop[iTrackId] == 1)
                {
                    printf("Not Getting JPEG Data , Stop \n");
                    return ;
                }
                goto GetBuf ;
            }
 
            // Calculate the time stamp ( microseconds part ) 
            if ( ptUBuffInfo.pbyUBuffHdr[36] != 1 ) 
            {
                printf("JPEG -->Not end ofFrame..\n " ) ;
            }
 
            uint32_t JpegHeaderSize = 
                  (ptUBuffInfo.pbyUBuffHdr[55] << 24 | ptUBuffInfo.pbyUBuffHdr[54] << 16 | ptUBuffInfo.pbyUBuffHdr[53] << 8| ptUBuffInfo.pbyUBuffHdr[52]);

            //To Cofirm that we received MJPEG payload  
            if( JpegHeaderSize !=  JPEG_HEADER_SIZE  )
            {
	         if(!(strncmp("JPEG",(char*)ptUBuffInfo.pbyUBuffHdr+4,4)))
	         {
		     LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
		     usleep(10000);
	      	     goto GetBuf;
	         }
                 else
                    goto Reconfig;
            } 
            memset(fBuffer,0x0,JpegHeaderSize);
            memcpy(fBuffer,ptUBuffInfo.pbyUBuffPayload,JpegHeaderSize );
            setParamsFromHeader();
       
            // Copy the JPEG payload 
            memcpy( fTo,ptUBuffInfo.pbyUBuffPayload + JpegHeaderSize,ptUBuffInfo.iUBuffPayloadLen -JpegHeaderSize);
            fFrameSize = ptUBuffInfo.iUBuffPayloadLen - JpegHeaderSize;

            fPresentationTime.tv_sec = ptUBuffer->dwSec ; 
            fPresentationTime.tv_usec =  ptUBuffer->dwUSec ; 
            fDurationInMicroseconds =  0 ;
            //fDurationInMicroseconds =  40000 ;
                  
            LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);

            bFirstframe = false;
            NonConfBufNum++;
        }
        else
        {
            LiveMediaSrc_JPEG_ReleaseUBuffer(hLMSrc, &ptUBuffInfo);
            goto NeedIntra;
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
            Default:
                nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
        }
}


u_int8_t FSJPEGVideoFramerSource::type() {
  return 1;
}
u_int8_t FSJPEGVideoFramerSource::qFactor() {
   fLastQFactor = 255;
  return   fLastQFactor;  //quantization tables are dynamic
}
u_int8_t FSJPEGVideoFramerSource::width() {
  return fLastWidth;
}
u_int8_t FSJPEGVideoFramerSource::height() {
  return fLastHeight;
}

void FSJPEGVideoFramerSource::setParamsFromHeader() 
{
    Boolean foundSOF0 = False;
    fLastQuantizationTableSize = 0;
            
    for (unsigned i = 0; i < JPEG_HEADER_SIZE-8; ++i) 
    {
        if (fBuffer[i] == 0xFF) 
        {
            if (fBuffer[i+1] == 0xDB) 
            { // DQT
                u_int16_t length = (fBuffer[i+2]<<8) | fBuffer[i+3];
                if (i+2 + length < JPEG_HEADER_SIZE) 
                { 
                    u_int16_t tableSize = length - 3;
                         
                    if (fLastQuantizationTableSize + tableSize > 128) 
                    {
                        tableSize = 128 - fLastQuantizationTableSize;
                    }
                    // combine Two Quantization Tables by  eliminate
                    //byte (0x01) which seperates two tables
                    memmove(&fLastQuantizationTable[fLastQuantizationTableSize],
                          &fBuffer[i+5], 64);
                    memmove(&fLastQuantizationTable[fLastQuantizationTableSize+64],
                          &fBuffer[i+5+65], 64);

                    fLastQuantizationTableSize += tableSize;
	            if (fLastQuantizationTableSize == 128 && foundSOF0) break;
                          i += length; // skip over table
                }
            } 
            else if (fBuffer[i+1] == 0xC0) 
            { 
                fLastHeight = (fBuffer[i+5]<<5)|(fBuffer[i+6]>>3);
                fLastWidth = (fBuffer[i+7]<<5)|(fBuffer[i+8]>>3);
                foundSOF0 = True;
                if (fLastQuantizationTableSize == 128) break;
                    // we've found everything that we want
                    i += 8;
            }
        }
    }

}

u_int8_t const* FSJPEGVideoFramerSource::quantizationTables(u_int8_t& precision,
                                                    u_int16_t& length) {
  // Default implementation
  precision = 0;
  length = 128;
  return fLastQuantizationTable;
}

