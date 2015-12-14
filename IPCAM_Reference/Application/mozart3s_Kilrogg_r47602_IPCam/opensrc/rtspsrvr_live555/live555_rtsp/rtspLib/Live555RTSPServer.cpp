/****************************************************************************
 *    COMPONENT :    
 *            Live 555 RTSP Server Stack
 *
 *    FILE:    
 *            Live555OnDemandServer.cpp
 *
 *    DATE:    
 *            Tue Jan 29 17:25:41 IST 2013
 *
 *    Description: 
 *            Implementation of Live555 OnDemand Server stack for Live555
 *            IP Camera platform
 *
****************************************************************************/

/****************************************************************************
 *  Include Files
****************************************************************************/
// Standard Headers
#include <pthread.h>

// Live555 Headers 
#include "RtspSrvr.h"
#include "ubuffer.h"
#include "LiveMediaSrc.h"
#include "LiveMediaSrc_protect.h"

// Live555 Headers
#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"
#include "RTSPServer.hh"
#include "JPEGVideoFileServerMediaSubsession.hh"
#include "FSH264VideoServerMediaSubsession.hh"


/****************************************************************************
 *    Macro definitions
****************************************************************************/
#define SERVER_DESCRIPTION  "Session streamed by \"Live555 OnDemandRTSPServer\""

#define Live555_CONFBUF_H264_SPS_SIZE_OFFSET  (sizeof(TUBufferConfH264))
#define Live555_CONFBUF_MP4V_PROFILE_OFFSET   (52)
#define Live555_CONFBUF_MP4V_CONFIG_OFFSET    (55)
#define Live555_CONFBUF_AAC_CHANNELNUM_OFFSET (44)
#define Live555_CONFBUF_AAC_SAMPLERATE_OFFSET (Live555_CONFBUF_AAC_CHANNELNUM_OFFSET+4)
#define Live555_CONFBUF_AAC_PROFILELVL_OFFSET (Live555_CONFBUF_AAC_SAMPLERATE_OFFSET+4)
#define Live555_CONFBUF_AAC_CONFIGSTR_OFFSET  (Live555_CONFBUF_AAC_PROFILELVL_OFFSET+4)
#define Live555_CONFBUF_CODEC_NAME_OFFSET     (4)

#define MAX_CHANNELS          4
#define MAX_CONFIG_SIZE      48
#define MAX_CODEC_STRING_LEN 12

/****************************************************************************
 *    Typedefs / Enums                
****************************************************************************/
typedef struct RtspSrvr{
    // Access File names
    char *pcAccessName[MAX_CHANNELS];

    // Server Media sessions for inidiviual Access Files
    ServerMediaSession *pSMS[MAX_CHANNELS];

    // Server Media sessions for inidiviual Access Files
    char cReset[MAX_CHANNELS];

} RtspSrvr;

/********TrackInfo*********/
//Holds info of Each session
typedef struct TrackInfo {

    char   acVCodecName[MAX_CODEC_STRING_LEN];
    char   acACodecName[MAX_CODEC_STRING_LEN];
    /*H264 */
    char pcSPS[MAX_CONFIG_SIZE];
    char pcPPS[MAX_CONFIG_SIZE];
    int  uiSPSsize;
    int  uiPPSsize;
    /*MPE4*/ 
    unsigned int ucProfile ;
    unsigned int uiConfigSize;
    char pcMP4VConfig[MAX_CONFIG_SIZE];
 
}TrackInfo;

/****************************************************************************
 *    Local Classes
****************************************************************************/

/****************************************************************************
 *    Global variables  
****************************************************************************/
UsageEnvironment*  env            = NULL ;
UsageEnvironment*  env1           = NULL ;
UsageEnvironment*  env2           = NULL ;
UsageEnvironment*  env3           = NULL ;
UsageEnvironment*  env4           = NULL ;

RTSPServer*        rtspServer     = NULL ;
TRtspSrvrInitOpts* srvrInitOpts   = NULL ;

TaskScheduler*     scheduler      = NULL ;
TaskScheduler*     scheduler1     = NULL ;
TaskScheduler*     scheduler2     = NULL ;
TaskScheduler*     scheduler3     = NULL ;
TaskScheduler*     scheduler4     = NULL ;

static char        RtspServer_Stop   = 0;
Boolean            reuseFirstSource  = True;

//Flag for Stopping Interrupted Server
bool               ServerStop        = false;  

pthread_t          threadid;
RtspSrvr           Live555RtspSrvr;
TrackInfo          SessionTrackInfo[MAX_CHANNELS];

/****************************************************************************
 *    extern variables
****************************************************************************/
/****************************************************************************
 *    Function Prototypes 
****************************************************************************/
static void* Live555_RtspSrvrStart(void*);

// To Add MPEG4 video sub session
static SCODE Live555_Add_MPEG4MediaSubSession(ServerMediaSession *pSMS, 
                                              HANDLE hLMVSrc, 
                                              TLMSrcUBufInfo ptUBuffInfo,
                                               int iTrackId );
// To Add H264 video sub session
static SCODE Live555_Add_H264MediaSubSession(ServerMediaSession *pSMS, 
                                              HANDLE hLMVSrc,
                                              TLMSrcUBufInfo ptUBuffInfo,
                                               int iTrackId);
// To Add MJPG video sub session
static SCODE Live555_Add_MJPGMediaSubSession(ServerMediaSession *pSMS, 
                                              HANDLE hLMVSrc,
                                              TLMSrcUBufInfo ptUBuffInfo,
                                              int iTrackId );
// To Add AAC Audio sub session
static SCODE Live555_Add_AACMediaSubSession(ServerMediaSession *pSMS, 
                                              HANDLE hLMVSrc,
                                              TLMSrcUBufInfo ptUBuffInfo,
                                               int iTrackId );
// To Add new Server media session session
static SCODE Live555_Add_ServerMediaSession( HANDLE hLMVSrc,
                                   TLMSrcUBufInfo ptUBuffInfo,
                                   char *pcAccessName, char *pcCodecName,
                                   int iTrackId);

// To Add audio sub session to all the video Tracks
static void* Live555_Add_AudioSubSession(void*);

static void* CreateScheduler1(void*);
static void* CreateScheduler2(void*);
static void* CreateScheduler3(void*);
static void* CreateScheduler4(void*);

// To reconfigure a Server media session session
SCODE Live555_Reconfig_ServerMediaSession(int iTrackId,
                                          TLMSrcUBufInfo ptUBuffInfo);

//Check the any chnages in the Media formats & configures
bool  IsServerMediaSessionGetChange(ServerMediaSession *);
SCODE CmpVidCodecNames ( char * ExistingCodec,char * NewCodec , int Len);
/****************************************************************************
 *
 * NAME          :  Live555_Add_MPEG4MediaSubSession
 *
 * PARAMETERS    :  pSMS : Server media session object
 *                  hLMVSrc : Live media source object 
 *                  ptUBuffInfo : Configuration Buffer 
 *
 * RETURN VALUES :  Status of adding new media sub session 
 *
 * DESCRIPTION   :  Construct a new  MPEG4 media sub session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
SCODE Live555_Add_MPEG4MediaSubSession(ServerMediaSession *pSMS, 
                                       HANDLE hLMVSrc, 
                                       TLMSrcUBufInfo ptUBuffInfo,
                                        int iTrackId)
{
    SCODE iRet = S_OK; 

    // Parse the MPEG4V profile ID
    SessionTrackInfo[iTrackId].ucProfile = 
                   ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_MP4V_PROFILE_OFFSET] ;

    // Parse the MPEG4V Configration size
    SessionTrackInfo[iTrackId].uiConfigSize  = 
                   ptUBuffInfo.iUBuffHdrLen - Live555_CONFBUF_MP4V_CONFIG_OFFSET ;

   memset(&SessionTrackInfo[iTrackId].pcMP4VConfig ,0,SessionTrackInfo[iTrackId].uiConfigSize);

    // copy the MP4V configuration data
    memcpy(SessionTrackInfo[iTrackId].pcMP4VConfig,  
           ptUBuffInfo.pbyUBuffHdr+(Live555_CONFBUF_MP4V_CONFIG_OFFSET+1),
                                SessionTrackInfo[iTrackId].uiConfigSize );
 
     SessionTrackInfo[iTrackId].pcMP4VConfig[SessionTrackInfo[iTrackId].uiConfigSize] = '\0';

    // Add MPEG4 media sub session to the ServerMedia session
    if( pSMS->addSubsession( MPEG4VideoFileServerMediaSubsession::createNew(*env,
                         iTrackId,
                         reuseFirstSource ,
                         SessionTrackInfo[iTrackId].ucProfile,
                         (char const*)SessionTrackInfo[iTrackId].pcMP4VConfig,
                         SessionTrackInfo[iTrackId].uiConfigSize,
                         hLMVSrc)) == False )
    {
        iRet = S_FAIL ;
    }
 
    return iRet;
}

/****************************************************************************
 *
 * NAME          :  Live555_Add_H264MediaSubSession
 *
 * PARAMETERS    :  pSMS : Server media session object
 *                  hLMVSrc : Live media source object 
 *                  ptUBuffInfo : Configuration Buffer 
 *
 * RETURN VALUES :  Status of adding new media sub session 
 *
 * DESCRIPTION   :  Construct a new  H264 media sub session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
SCODE Live555_Add_H264MediaSubSession( ServerMediaSession *pSMS, 
                                       HANDLE hLMVSrc, 
                                       TLMSrcUBufInfo ptUBuffInfo,
                                       int iTrackId )
{
    SCODE iRet = S_OK; 

    // Parse the SPS Size 
    SessionTrackInfo[iTrackId].uiSPSsize = (ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_H264_SPS_SIZE_OFFSET]) |  
                (ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_H264_SPS_SIZE_OFFSET+1]) << 8 ;

    // copy the SPS data
    memcpy(SessionTrackInfo[iTrackId].pcSPS,  ptUBuffInfo.pbyUBuffHdr+(Live555_CONFBUF_H264_SPS_SIZE_OFFSET+2), 
                                   SessionTrackInfo[iTrackId].uiSPSsize );
   
    // Parse the PPS Size 
    SessionTrackInfo[iTrackId].uiPPSsize = (ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_H264_SPS_SIZE_OFFSET+
                                          SessionTrackInfo[iTrackId].uiSPSsize+2]) |  
       (ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_H264_SPS_SIZE_OFFSET+SessionTrackInfo[iTrackId].uiSPSsize+3]) << 8 ;

    // copy the PPS data
    memcpy(SessionTrackInfo[iTrackId].pcPPS,  
            ptUBuffInfo.pbyUBuffHdr+(Live555_CONFBUF_H264_SPS_SIZE_OFFSET+ SessionTrackInfo[iTrackId].uiSPSsize+4) ,
             SessionTrackInfo[iTrackId].uiPPSsize );

    // Add H264 media sub session to the ServerMedia session
    if( pSMS->addSubsession( FSH264VideoServerMediaSubsession::createNew(*env,iTrackId,reuseFirstSource,
                         (u_int8_t*)SessionTrackInfo[iTrackId].pcSPS,SessionTrackInfo[iTrackId].uiSPSsize,
                         (u_int8_t*)SessionTrackInfo[iTrackId].pcPPS,SessionTrackInfo[iTrackId].uiPPSsize,
                         hLMVSrc)) == False )
    {
        iRet = S_FAIL ;
    }
    return iRet ;
}

/****************************************************************************
 *
 * NAME          :  Live555_Add_MJPGMediaSubSession
 *
 * PARAMETERS    :  pSMS : Server media session object
 *                  hLMVSrc : Live media source object 
 *                  ptUBuffInfo : Configuration Buffer 
 *
 * RETURN VALUES :  Status of adding new media sub session 
 *
 * DESCRIPTION   :  Construct a new MJPG media sub session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
SCODE Live555_Add_MJPGMediaSubSession(ServerMediaSession *pSMS, 
                                       HANDLE hLMVSrc, 
                                       TLMSrcUBufInfo ptUBuffInfo,
                                        int iTrackId)
 
{
    SCODE iRet = S_OK; 
    // Add MJPEG media sub session to the ServerMedia session
    if( pSMS->addSubsession( JPEGVideoFileServerMediaSubsession::createNew(*env,
                          iTrackId,
                         reuseFirstSource ,
                         hLMVSrc)) == False )
    {
        iRet = S_FAIL ;
    }
    return iRet ;
}


/****************************************************************************
 *
 * NAME          :  Live555_Add_AACMediaSubSession
 *
 * PARAMETERS    :  pSMS : Server media session object
 *                  hLMASrc : Live media source object 
 *                  ptUBuffInfo : Configuration Buffer 
 *
 * RETURN VALUES :  Status of adding new media sub session 
 *
 * DESCRIPTION   :  Construct a new  AAC media sub session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
SCODE Live555_Add_AACMediaSubSession(ServerMediaSession *pSMS, 
                                       HANDLE hLMASrc, 
                                       TLMSrcUBufInfo ptUBuffInfo,
                                        int iTrackId )
{
    SCODE iRet = S_OK; 
    int  iSampleRate   ;
    int  iProfileLevel ;
    int  iChannelNumber ;
    char acConfigStr[5];  

    // Parse the AAC channelnumber 
    iChannelNumber = ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_CHANNELNUM_OFFSET]   |
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_CHANNELNUM_OFFSET+1] << 8  |
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_CHANNELNUM_OFFSET+2] << 16 |  
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_CHANNELNUM_OFFSET+3] << 24 ;  

    // Parse the Samplerate 
    iSampleRate    = ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_SAMPLERATE_OFFSET]   |
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_SAMPLERATE_OFFSET+1] << 8  |
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_SAMPLERATE_OFFSET+2] << 16 |  
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_SAMPLERATE_OFFSET+3] << 24 ;  

    // Parse profile level
    iProfileLevel  = ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_PROFILELVL_OFFSET]   |
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_PROFILELVL_OFFSET+1] << 8  |
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_PROFILELVL_OFFSET+2] << 16 |  
                     ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_PROFILELVL_OFFSET+3] << 24 ;  

    // Parse the AAC Config string
    sprintf(acConfigStr,"%x%x", ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_CONFIGSTR_OFFSET],
                                ptUBuffInfo.pbyUBuffHdr[Live555_CONFBUF_AAC_CONFIGSTR_OFFSET+1]);
    acConfigStr[4]='\0';
                    
    // Add AAC media sub session to the ServerMedia session
    if( pSMS->addSubsession( ADTSAudioFileServerMediaSubsession::createNew(*env,NULL,
                         reuseFirstSource ,
                         hLMASrc,  
                         iSampleRate, 
                         iChannelNumber,
                         iProfileLevel, acConfigStr,iTrackId)) == False )
    {
        iRet = S_FAIL ;
    }
    return iRet ;
}


/****************************************************************************
 *
 * NAME          :  Live555_Add_ServerMediaSession 
 *
 * PARAMETERS    :  pServer : RTSP server sessions
 *                  hLMVSrc : Live media source object 
 *
 * RETURN VALUES :  Status of adding new media session 
 *
 * DESCRIPTION   :  Construct a new media session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
SCODE Live555_Add_ServerMediaSession( HANDLE hLMVSrc,
                                      TLMSrcUBufInfo ptUBuffInfo,
                                      char *pcAccessName, char *pcCodecName,
                                      int iTrackId  )
{
    SCODE iRet = S_OK;
    ServerMediaSession *pSMS ;
    const char *pcDescription = SERVER_DESCRIPTION ;

    // Create a server media session
    if( Live555RtspSrvr.pSMS[iTrackId] == NULL )
    {
        Live555RtspSrvr.pSMS[iTrackId] = ServerMediaSession::createNew(*env, pcAccessName, 
                                           pcAccessName,pcDescription,False,NULL,iTrackId);
    }
    else
    {
          printf("Failed to Create Server Media Session\n"); 
          return S_FAIL;
    }

    pSMS  = Live555RtspSrvr.pSMS[iTrackId] ;
    if( pSMS )
    { 
        if( !strncmp(pcCodecName,"CONFH264",8) )
        {
            // Create H264 media subsession
            iRet = Live555_Add_H264MediaSubSession( pSMS, hLMVSrc,ptUBuffInfo,iTrackId);
        }
        else if( !strncmp(pcCodecName,"CONFJPEG",8) )
        {
            // Create MJPG media subsession
            iRet = Live555_Add_MJPGMediaSubSession( pSMS, hLMVSrc,ptUBuffInfo,iTrackId);
        }
        else if( !strncmp(pcCodecName,"CONFMP4V",8) )
        {
            // Create MP4V media subsession
            iRet = Live555_Add_MPEG4MediaSubSession( pSMS, hLMVSrc,ptUBuffInfo,iTrackId);
        }
        else if( !strncmp(pcCodecName,"CONFAAC4",8) )
        {
            // Create AAC  media subsession
            iRet = Live555_Add_AACMediaSubSession( pSMS, hLMVSrc,ptUBuffInfo,iTrackId);
        }
        else
        {
            // Invalid CODEC 
            printf("Invalid codec....\n");
            return S_FAIL ;
        }
        // Add server media session to the RTSP Server
        rtspServer->addServerMediaSession( pSMS );
    } 
    else
    { 
        iRet = S_FAIL ;
    }  
    return iRet ; 
}

/****************************************************************************
 *
 * NAME          :  Live555_RtspSrvrStart
 *
 * PARAMETERS    :  pvData : NONE
 *
 * RETURN VALUES :  Status of adding new media session 
 *
 * DESCRIPTION   :  Construct a new media session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
void *Live555_RtspSrvrStart ( void *pvData )
{
    void *pvRet = NULL ;
    SCODE iRet = S_OK; 
    pthread_t AudioThread;
    pthread_t SchThread1,SchThread2,SchThread3,SchThread4;

    TLMSrcUBufInfo ptUBuffInfo[MAX_CHANNELS] ;
    HANDLE hLMVSrc[MAX_CHANNELS] ;
    int iTrackId = -1;

    // Add media sessions for all access files
    for( iTrackId=0;  iTrackId<srvrInitOpts->iAcsFileInfoNum; iTrackId++ )
    {
        if( iTrackId< srvrInitOpts->iLiveVSrcNum ) 
        {
            // Get the live media source object
            hLMVSrc[iTrackId] = ( TLiveMediaSrc*)(srvrInitOpts->ahLiveVSrc[iTrackId]);
            if(hLMVSrc[iTrackId] == NULL)
            {
               printf("Failed to get Media Server Source Object\n ");
               return NULL;
            }
            //Send the command to get codec configuration
            LiveMediaSrc_EventHandler( hLMVSrc[iTrackId],letNeedConf );

            NeedVideoCONF: 
                //Get the codec configuration
                LiveMediaSrc_GetUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]) ;
                         
                if( ptUBuffInfo[iTrackId].iUBuffHdrLen <= 0 )
                {
                    LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);
                    printf("ERROR : Invalid Video Live Media source header length\n");
                    return NULL ;
                } 
                 
                if (ptUBuffInfo[iTrackId].pbyUBuffPayload == NULL)
                {
                    // Copy the CODEC name
                    strncpy (SessionTrackInfo[iTrackId].acVCodecName,
                                             (const char*)(ptUBuffInfo[iTrackId].pbyUBuffHdr+
                                             Live555_CONFBUF_CODEC_NAME_OFFSET), 8);
                                       
                    SessionTrackInfo[iTrackId].acVCodecName[8]='\0';
                                    
                    // Add server media session to the RTSP server
                    iRet = Live555_Add_ServerMediaSession( hLMVSrc[iTrackId], 
                                       ptUBuffInfo[iTrackId],
                                       Live555RtspSrvr.pcAccessName[iTrackId], 
                                       SessionTrackInfo[iTrackId].acVCodecName, 
                                       iTrackId);
                    usleep(20000);
                    LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);
                }
                else
                {
                    usleep(20000);
                    LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);
                    printf("NO CONF DATA FOUND..\n");
                    goto NeedVideoCONF;
                }
                //Release the conf buffer
                usleep(200000);
                LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);
        }
        // Add Audio sub sessions to the server media session
        // For all the 
        if( iTrackId < srvrInitOpts->iLiveASrcNum ) 
        {
            // Start the Audio thread to add the sub session to the media server session
            pthread_create(&AudioThread,NULL,Live555_Add_AudioSubSession,(void*)&iTrackId);
        }
    }
    pthread_create(&SchThread1,NULL,CreateScheduler1,(void*)&iTrackId);
    pthread_create(&SchThread2,NULL,CreateScheduler2,(void*)&iTrackId);
    pthread_create(&SchThread3,NULL,CreateScheduler3,(void*)&iTrackId);
    pthread_create(&SchThread4,NULL,CreateScheduler4,(void*)&iTrackId);
   
    printf("\nRTSP Server START Successfully\n");
    // Start the RTSP Server 
    env->taskScheduler().doEventLoop( &RtspServer_Stop );
    printf("Stopping live555 server thread.....\n");

    return pvRet ;
}
/**************************************************************************



*************************************************************************/
void* CreateScheduler1( void * pvTrackerId  )
{


      scheduler1 = BasicTaskScheduler::createNew();
      env1 = BasicUsageEnvironment::createNew(*scheduler1);
    // TODELETE 
      env1->taskScheduler().doEventLoop( &RtspServer_Stop );
      return NULL;
}
/**************************************************************************



*************************************************************************/
void* CreateScheduler2( void * pvTrackerId  )
{


      scheduler2 = BasicTaskScheduler::createNew();
      env2 = BasicUsageEnvironment::createNew(*scheduler2);
    // TODELETE 
    env2->taskScheduler().doEventLoop( &RtspServer_Stop );
      return NULL;
}
/**************************************************************************



*************************************************************************/
void* CreateScheduler3( void * pvTrackerId  )
{


      scheduler3 = BasicTaskScheduler::createNew();
      env3 = BasicUsageEnvironment::createNew(*scheduler3);
    // TODELETE 
    env3->taskScheduler().doEventLoop( &RtspServer_Stop );
      return NULL;
}
/**************************************************************************



*************************************************************************/
void* CreateScheduler4( void * pvTrackerId  )
{


      scheduler4 = BasicTaskScheduler::createNew();
      env4 = BasicUsageEnvironment::createNew(*scheduler4);
    // TODELETE 
      env4->taskScheduler().doEventLoop( &RtspServer_Stop );
      return NULL;
}
/****************************************************************************
 *
 * NAME          :  Live555_Add_AudioSubSession
 *
 * PARAMETERS    :  pvTrackid
 *
 * RETURN VALUES :  Status of adding new media session 
 *
 * DESCRIPTION   :  Construct a new media session and add it to 
 *                  the existing Server Media Session
 *
****************************************************************************/
void* Live555_Add_AudioSubSession( void * pvTrackerId  )
{
  
    int iTrackId =  ((int) *((int*)(pvTrackerId)));
    TLMSrcUBufInfo ptUBuffInfo ;
    HANDLE hLMASrc ; 
    char   aAVCodecName[MAX_CODEC_STRING_LEN];
    int AudioTrack =0 ;
    SCODE iRet = S_OK ;  

    // Audio Trackid
    AudioTrack = ((int) *((int*)(pvTrackerId))) ;
    AudioTrack = 0 ;

    // Get the live media source object
    hLMASrc  = ( TLiveMediaSrc*)(srvrInitOpts->ahLiveASrc[AudioTrack]);

    NeedAudioCONF: 
        //Send the command to get codec configuration
        LiveMediaSrc_EventHandler( hLMASrc,letNeedConf );

        //Get the codec configuration
        LiveMediaSrc_GetUBuffer( hLMASrc, &ptUBuffInfo) ;
        if( ptUBuffInfo.iUBuffHdrLen <= 0 )
        {
            LiveMediaSrc_ReleaseUBuffer( hLMASrc, &ptUBuffInfo);
            //printf("ERROR : Invalid AAC Audio header length\n");
            return NULL ;
        } 

        if (ptUBuffInfo.pbyUBuffPayload == NULL)
        {
            // Copy the CODEC name
            strncpy (aAVCodecName,(const char*)(ptUBuffInfo.pbyUBuffHdr+
                                               Live555_CONFBUF_CODEC_NAME_OFFSET), 8);
            aAVCodecName[8]='\0';
                 
            // Add the AAC sub session to all the Media sessions
            if( !strncmp(aAVCodecName,"CONFAAC4",8) )
            {
                if( Live555RtspSrvr.pSMS[iTrackId] != NULL)
                {
                    // Create AAC media subsession
                    iRet = Live555_Add_AACMediaSubSession( Live555RtspSrvr.pSMS[iTrackId], 
                                                     hLMASrc,ptUBuffInfo,iTrackId);
                    if( iRet != S_OK )
                    {
                        printf("Failed to Add the AAC Audio sub session to the Track = %d\n", iTrackId );
                    }
                }
            }
        } 
        else
        {
            LiveMediaSrc_ReleaseUBuffer( hLMASrc, &ptUBuffInfo);
            //printf("NO CONF DATA : Check again..... %s()\n", __FUNCTION__ );
            goto NeedAudioCONF;
        }
        // Release the conf buffer
        LiveMediaSrc_ReleaseUBuffer( hLMASrc, &ptUBuffInfo);
        //printf("Adding AudioSubSession.....\n");
        return NULL;
}
/****************************************************************************
 *
 * NAME          :  Live555_Reconfig_ServerMediaSession
 *
 * PARAMETERS    :  iTrackId : int
 *
 * RETURN VALUES :  S_OK
 *
 * DESCRIPTION   :  Reconfig  Media Session
 *
****************************************************************************/
SCODE Live555_Reconfig_ServerMediaSession(int iTrackId,TLMSrcUBufInfo ptUBuffInfo)
{
    SCODE iRet = S_OK; 

    HANDLE hLMVSrc;
    HANDLE hLMASrc ; 
    char   aAVCodecName[MAX_CODEC_STRING_LEN];
    int AudioTrack = 0;  

    // Get the live media source object
    hLMVSrc = ( TLiveMediaSrc*)(srvrInitOpts->ahLiveVSrc[iTrackId]);

    if (ptUBuffInfo.pbyUBuffPayload == NULL)
    {
	 // Copy the CODEC name
        strncpy (aAVCodecName,
              (const char*)(ptUBuffInfo.pbyUBuffHdr+Live555_CONFBUF_CODEC_NAME_OFFSET), 
                     8);
	aAVCodecName[8]='\0';

	// Add server media session to the RTSP server
        iRet = Live555_Add_ServerMediaSession( hLMVSrc, 
			                      ptUBuffInfo,
			                      Live555RtspSrvr.pcAccessName[iTrackId], 
			                      aAVCodecName, iTrackId);
    }     
    if( iTrackId  == 0)
    {
        // Get the live media source object
        hLMASrc  = ( TLiveMediaSrc*)(srvrInitOpts->ahLiveASrc[AudioTrack]);
        //Send the command to get codec configuration
        LiveMediaSrc_EventHandler( hLMASrc,letNeedConf );
                   
        NeedAudioCONF: 
            //Get the codec configuration
            LiveMediaSrc_GetUBuffer( hLMASrc, &ptUBuffInfo) ;
            if( ptUBuffInfo.iUBuffHdrLen <= 0 )
            {
                LiveMediaSrc_ReleaseUBuffer( hLMASrc, &ptUBuffInfo);
                printf("ERROR : Invalid AAC Audio header length\n");
                return NULL ;
            } 
            if (ptUBuffInfo.pbyUBuffPayload == NULL)
            {
                // Copy the CODEC name
                strncpy (aAVCodecName,(const char*)(ptUBuffInfo.pbyUBuffHdr+Live555_CONFBUF_CODEC_NAME_OFFSET), 8);
                aAVCodecName[8]='\0';

                // Add the AAC sub session to all the Media sessions
                if( !strncmp(aAVCodecName,"CONFAAC4",8) )
                {
                    if( Live555RtspSrvr.pSMS[iTrackId] != NULL)
                    {
                        // Create AAC media subsession
                        iRet = Live555_Add_AACMediaSubSession( Live555RtspSrvr.pSMS[iTrackId], 
                                                        hLMASrc,
                                                        ptUBuffInfo,iTrackId);
                       if( iRet != S_OK )
                       {
                           printf("Failed to Add the AAC Audio sub session to the Track = %d\n", iTrackId );
                       }
                    }
                    LiveMediaSrc_ReleaseUBuffer( hLMASrc, &ptUBuffInfo);
                }
            }
            else
            {
                LiveMediaSrc_ReleaseUBuffer( hLMASrc, &ptUBuffInfo);
                printf("NO CONF DATA : Check again..... %s()\n", __FUNCTION__ );
                usleep(20000);
                goto NeedAudioCONF;
            }
    }
    return iRet ;
}
/****************************************************************************
 *
 * NAME          :  CmpVidCodecNames 
 *
 * PARAMETERS    :  
 *
 * RETURN VALUES :  
 *
 * DESCRIPTION   :  
 *
****************************************************************************/
SCODE CmpVidCodecNames ( char * ExistingCodec,char * NewCodec , int Len)
{
    int Index = 0;

    if ( ( ExistingCodec != NULL ) && ( NewCodec != NULL )  )
    {
        for( Index = 4;Index < Len;Index++)
        {
            if( ExistingCodec[Index] == NewCodec[Index] )
                continue;
            else
              break;
        } 
    }
    else
    { 
       printf("DATA is not Proper NULL pointers\n");
       return S_FAIL;
    }
   
    if( Index == Len)
       return S_OK;
    else
       return Index;
}
/****************************************************************************
 *
 * NAME          :  IsServerMediaSessionGetChange 
 *
 * PARAMETERS    :  ServerMediaSession
 *
 * RETURN VALUES :  Status of SubSession Changed
 *
 * DESCRIPTION   :  COnfiguration of Subsession will verify 
 *
****************************************************************************/
bool IsServerMediaSessionGetChange(ServerMediaSession * SMS )
{
         
    HANDLE hLMVSrc[MAX_CHANNELS] ; 
    TLMSrcUBufInfo ptUBuffInfo; 
    int iTrackId = -1;
   
    if(SMS != NULL)
    { 
       iTrackId = SMS->SessionTrackID();
    }
    else
    {
        printf("Server Media Session Not Exits\n");
        return False;
    }
                                          
    // Get info of Servermedia Session
    if( iTrackId < srvrInitOpts->iLiveVSrcNum ) 
    {
        //Get the live media source object
        hLMVSrc[iTrackId] = ( TLiveMediaSrc*)(srvrInitOpts->ahLiveVSrc[iTrackId]);

	//Send the command to get codec configuration
	LiveMediaSrc_EventHandler( hLMVSrc[iTrackId],letNeedConf );
          
       NeedVideoCONF: 
	    //Get the codec configuration
	    LiveMediaSrc_GetUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo) ;
         
	    if( ptUBuffInfo.iUBuffHdrLen < 0 )
	    {
	        LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo);
	        printf("ERROR : Invalid Video Live Media source header length\n");
                return False;
	    }
	    if (ptUBuffInfo.pbyUBuffPayload == NULL)
	    {
	         // Copy the CODEC name
	         if( CmpVidCodecNames ( SessionTrackInfo[iTrackId] .acVCodecName,
                      ( char*)(ptUBuffInfo.pbyUBuffHdr+Live555_CONFBUF_CODEC_NAME_OFFSET), 
                       8) )
                 {
                      rtspServer->closeAllClientSessionsForServerMediaSession(SMS);
                      SMS->deleteAllSubsessions();
                         
                      rtspServer->deleteServerMediaSession(Live555RtspSrvr.pSMS[iTrackId]);
                              
                      Live555RtspSrvr.pSMS[iTrackId] = NULL;
                             
                      //Copying Current CodecName 
                      strncpy(SessionTrackInfo[iTrackId].acVCodecName,
                           (const char*)(ptUBuffInfo.pbyUBuffHdr+Live555_CONFBUF_CODEC_NAME_OFFSET)
                                ,8);                   
                      SessionTrackInfo[iTrackId].acVCodecName[8]='\0';
                                 
	              if (Live555_Reconfig_ServerMediaSession( iTrackId , ptUBuffInfo ))
                         return False;
                                    
                      usleep(20000);
		      LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo);
                 }
                 else
                 {
		      LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo);
                 }
	    }
	    else
	    { 
                LiveMediaSrc_ReleaseUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo);
                //printf("NO CONF DATA : Check again..... %s()\n", __FUNCTION__ );
                usleep(5000);
                goto NeedVideoCONF;
            }
    }
    else
    {
         printf("Invalid Track ID\n");
         return False;
    }
    return True;
}


/****************************************************************************
 *
 * NAME          :  LiveMediaSrc_GetUBuffer
 *
 * PARAMETERS    :  hLMSrc : Live media source File/Socket/ShrdBuf/UnxiSkt
 *                  ptUBuffInfo : Pointer to the buffer
 *
 * RETURN VALUES :  Status of Get buffer call
 *
 * DESCRIPTION   :  To get the data or configuration buffers
 *
****************************************************************************/
SCODE LiveMediaSrc_GetUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;
         
    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfGetUBuf )
    {
        // Call to media source to get the data/conf buffer
        iRet = (phLiveVSrc->ptCBMap->pfGetUBuf)(hLMSrc, ptUBuffInfo); 
    }
    else
    {
        printf("No Data/Conf Buffer Found In %s\n",__FUNCTION__);
    }
    return  iRet ;
}

/****************************************************************************
 *
 * NAME          :  LiveMediaSrc_ReleaseUBuffer
 *
 * PARAMETERS    :  hLMSrc : Live media source File/Socket/ShrdBuf/UnxiSkt
 *                  ptUBuffInfo : Pointer to the buffer
 *
 * RETURN VALUES :  Status of Release buffer call  
 *
 * DESCRIPTION   :  To release the data or configuration buffers
 *
****************************************************************************/
SCODE LiveMediaSrc_ReleaseUBuffer(HANDLE hLMSrc, TLMSrcUBufInfo *ptUBuffInfo)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfReleaseUBuf)
    {
        // Call to media source to get the data/conf buffer
        iRet = (phLiveVSrc->ptCBMap->pfReleaseUBuf)(hLMSrc, ptUBuffInfo);
    }
    else
    {
        printf("No Data/Conf Buffer Found In %s\n",__FUNCTION__);
    }
    
    return  iRet ;
}

/****************************************************************************
 *
 * NAME          :  LiveMediaSrc_EventHandler  
 *
 * PARAMETERS    :  hLMSrc : Live media source File/Socket/ShrdBuf/UnxiSkt
 *                  eType : Event type
 *
 * RETURN VALUES :  Status of Release buffer call  
 *
 * DESCRIPTION   :  To release the data or configuration buffers
 *
****************************************************************************/
SCODE LiveMediaSrc_EventHandler(HANDLE hLMSrc, ELMSrcEventType eType)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfReleaseUBuf)
    {
        // Call to media source to handle the events
        iRet = ( phLiveVSrc->ptCBMap->pfEvntHandler)(hLMSrc, eType); 
    }
    else
    {
        printf("No Event Found In %s\n",__FUNCTION__);
    }
    return  iRet ;
}

/****************************************************************************
 *
 * NAME          :  LiveMediaSrc_Release   
 *
 * PARAMETERS    :  hLMSrc : Live media source File/Socket/ShrdBuf/UnxiSkt
 *
 * RETURN VALUES :  Status of Get Release the source
 *
 * DESCRIPTION   :  To release the live media source
 *
****************************************************************************/
SCODE LiveMediaSrc_Release(HANDLE *hLMSrc)
{
    SCODE iRet = S_OK; 
    TLiveMediaSrc * phLiveVSrc;

    // Get the Live media video source ShrdBuf/File/Socket/UnixSocket
    phLiveVSrc = ( TLiveMediaSrc*)*(hLMSrc) ;

    // Check for the valid call back function to get Buffer
    if( phLiveVSrc && phLiveVSrc->ptCBMap  && phLiveVSrc->ptCBMap->pfRelease)
    {
        // Call to media source to release the data/conf buffer
        iRet = phLiveVSrc->ptCBMap->pfRelease(hLMSrc);
    }
    else
    {
        printf("No Buffer Found for Release In %s\n",__FUNCTION__);
    }

    return  iRet ;
}

/****************************************************************************
 *
 * NAME          :  RtspSrvr_Initial
 *
 * PARAMETERS    :  phRtspSrvrObj  : RTSP Server Object
 *                  ptOpts : Server options
 *
 * RETURN VALUES :  Status of RTSP server stack initialization
 *
 * DESCRIPTION   :  Allocate the resource and copy the server options 
 *
****************************************************************************/
extern "C" SCODE RtspSrvr_Initial(HANDLE *phRtspSrvrObj, TRtspSrvrInitOpts *ptOpts)
{
    SCODE iRet = S_OK;
 
    // Begin by setting up our usage environment:
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    srvrInitOpts = (TRtspSrvrInitOpts*)malloc(sizeof( TRtspSrvrInitOpts));
    if(srvrInitOpts == NULL)
    {
        printf("Mem not allocated ...\n");
        return S_FAIL ;
    }

    memset(srvrInitOpts, 0, sizeof(TRtspSrvrInitOpts));
    //Get Data from srvr Apps
    srvrInitOpts->usSrvListenPort          =  ptOpts->usSrvListenPort;        
    srvrInitOpts->usRtpStartPort           =  ptOpts->usRtpStartPort;
    srvrInitOpts->ahLiveVSrc               =  ptOpts->ahLiveVSrc ;             
    srvrInitOpts->iLiveVSrcNum             =  ptOpts->iLiveVSrcNum ;
    srvrInitOpts->ahLiveASrc               =  ptOpts->ahLiveASrc; 
    srvrInitOpts->iLiveASrcNum             =  ptOpts->iLiveASrcNum;
    srvrInitOpts->ahLiveDSrc               =  ptOpts->ahLiveDSrc;
    srvrInitOpts->iLiveDSrcNum             =  ptOpts->iLiveDSrcNum;
    srvrInitOpts->aptAcsFileInfo           =  ptOpts->aptAcsFileInfo;
    srvrInitOpts->szAuthMode               =  ptOpts->szAuthMode;
    srvrInitOpts->iAcsFileInfoNum          =  ptOpts->iAcsFileInfoNum;
                 
    // Create the RTSP server:
    rtspServer  = RTSPServer::createNew( *env, srvrInitOpts->usSrvListenPort, NULL);
    if (rtspServer == NULL) 
    {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        iRet = S_FAIL ;
    }
    
    for(int iIndex=0; iIndex < srvrInitOpts->iAcsFileInfoNum; iIndex++ )
    {
        Live555RtspSrvr.pcAccessName[iIndex] = (char*) malloc(strlen 
                                           (srvrInitOpts->aptAcsFileInfo[iIndex]->szAccessName) + 1 );
    
        if( Live555RtspSrvr.pcAccessName[iIndex] )
        {
            strcpy(Live555RtspSrvr.pcAccessName[iIndex],srvrInitOpts->
                                         aptAcsFileInfo[iIndex]->szAccessName);
        }
                      
        // Initialize the server media sessions
        Live555RtspSrvr.pSMS[iIndex]     = NULL ;

        // Initialize the reset flag
        Live555RtspSrvr.cReset[iIndex]   = 0;
    }
    for( int iChannelNum =0;iChannelNum < MAX_CHANNELS;iChannelNum++  )
        memset(&SessionTrackInfo[iChannelNum],0,sizeof(TrackInfo));

    printf("RTSP Server Initials Done....\n");
    return iRet ;                
}

/****************************************************************************
 *
 * NAME          :  RtspSrvr_Start  
 *
 * PARAMETERS    :  phRtspSrvrObj  : RTSP Server Object
 *
 * RETURN VALUES :  status of RTSP server start function
 *
 * DESCRIPTION   :  Start the OnDemand RTSP server
 *
 *
****************************************************************************/
extern "C" SCODE RtspSrvr_Start(HANDLE phRtspSrvrObj)
{
    SCODE iRet = S_OK ;
    int iRetVal =  0;
                 
    // Start the Live555 RTSP server
    iRetVal = pthread_create(&threadid,NULL,Live555_RtspSrvrStart,NULL );
    if( iRetVal )
    {
        // Failed to start the thread
        printf("ERROR : Failed to start the RTSP Server thread\n");
        iRet = S_FAIL ;
    }
    else
    {
        printf("LIVE555 RTSP Server thread started..\n");
    }
    return iRet ;
}


/****************************************************************************
 *
 * NAME          :  RtspSrvr_Stop   
 *
 * PARAMETERS    :  phRtspSrvrObj  : RTSP Server Object
 *
 * RETURN VALUES :  status of RTSP server start function
 *
 * DESCRIPTION   :  Stop the OnDemand RTSP server
 *
****************************************************************************/
extern "C" SCODE RtspSrvr_Stop(HANDLE phRtspSrvrObj)
{
    SCODE iRet = S_OK ;
    TLMSrcUBufInfo ptUBuffInfo[MAX_CHANNELS] ;
    HANDLE hLMVSrc[MAX_CHANNELS] ;
    int iTrackId = -1 ;
                                      
    //Making Server Stop Flag True
    ServerStop  = true;
    printf("\nWait for RTSP Server Stopping.....\n");
    sleep(3);
                      
    for( iTrackId=0;  iTrackId < srvrInitOpts->iAcsFileInfoNum ; iTrackId++ )
    {
        if( iTrackId< srvrInitOpts->iLiveVSrcNum )
        {
            // Get the live media source object
            hLMVSrc[iTrackId] = ( TLiveMediaSrc*)(srvrInitOpts->ahLiveVSrc[iTrackId]);           
            if(hLMVSrc[iTrackId] == NULL)
            {
               printf("Failed to get Media Server Source Object\n ");
               return S_FAIL;
            }
            //Send the command to get codec configuration
            LiveMediaSrc_EventHandler( hLMVSrc[iTrackId],letNeedConf );
                      
            GETBUFF:
                //Get the codec configuration
                LiveMediaSrc_GetUBuffer( hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]) ;

                if (ptUBuffInfo[iTrackId].pbyUBuffPayload == NULL)
                {
                    if((ptUBuffInfo[iTrackId].pbyUBuffHdr[47] << 24 | 
                                                     ptUBuffInfo[iTrackId].pbyUBuffHdr[46] << 16 | 
                                                     ptUBuffInfo[iTrackId].pbyUBuffHdr[45] << 8| 
                                                     ptUBuffInfo[iTrackId].pbyUBuffHdr[44]) == 1920)
                    {
                        usleep(20000);
                        LiveMediaSrc_ReleaseUBuffer(hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);               
                    }
                    else
                    {
                        usleep(20000);
                        LiveMediaSrc_ReleaseUBuffer(hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);               
                                   
                        usleep(20000);
                        LiveMediaSrc_ReleaseUBuffer(hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);               
                    } 
                }
                else
                {
                    usleep(20000);
                    LiveMediaSrc_ReleaseUBuffer(hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);               
                    goto GETBUFF;
                }
        }
        usleep(200000);
        LiveMediaSrc_ReleaseUBuffer(hLMVSrc[iTrackId], &ptUBuffInfo[iTrackId]);               
    }
    // Stop the RTSP server thread
    RtspServer_Stop  =  1;

    // small delay to Live555 release the internal resources.
    usleep(100000);

    printf("LIVE555 RTSP Server thread closed\n" );
    printf("RTSP Server STOP Successfully \n" );
    return iRet ;
}

/****************************************************************************
 *
 * NAME          :  RtspSrvr_SetupDestination 
 *
 * PARAMETERS    :  None
 *
 * RETURN VALUES :  None
 *
 * DESCRIPTION   :  None
 *
****************************************************************************/
extern "C" SCODE RtspSrvr_SetupDestination( HANDLE phRtspSrvrObj, BOOL bEnDiffDestIP)
{
    SCODE iRet = S_OK;
    // TODO 
    printf("%s() Enter...\n", __FUNCTION__ ) ;
    return iRet ;
}

/****************************************************************************
 *
 * NAME          :  RtspSrvr_SetupDestination 
 *
 * PARAMETERS    :  phRtspSrvrObj  : RTSP Server Object
 *
 * RETURN VALUES :  Status of releasing server object
 *
 * DESCRIPTION   :  Release the Server object resources
 *
****************************************************************************/
extern "C" SCODE RtspSrvr_Release( HANDLE *phRtspSrvrObj)
{
    SCODE iRet = S_OK;
    int iIndex ;
    printf("%s() Enter...\n", __FUNCTION__ ) ;

    // Release the temprorary resource allocated in Init function
    for( iIndex = 0 ; iIndex < srvrInitOpts->iAcsFileInfoNum;iIndex++  )
    {
        // Remove the access file information
        if( Live555RtspSrvr.pcAccessName[iIndex] )
        { 
            free( Live555RtspSrvr.pcAccessName[iIndex] );
            Live555RtspSrvr.pcAccessName[iIndex] = NULL ;
        }
    }
    // Close the RTSP Server
    Medium::close(rtspServer);
    rtspServer = NULL ;
    // Release all the RTSP environment resources 
    env->reclaim();
    env = NULL ;

    env1->reclaim();
    env1 = NULL ;

    env2->reclaim();
    env2 = NULL ;

    env3->reclaim();
    env3 = NULL ;

    env4->reclaim();
    env4 = NULL ;
    // Release the scheduler object
    delete scheduler ;
    scheduler = NULL ; 

    // Release the scheduler object
    delete scheduler1 ;
    scheduler1 = NULL ; 

    // Release the scheduler object
    delete scheduler2 ;
    scheduler2 = NULL ; 

    // Release the scheduler object
    delete scheduler3 ;
    scheduler3 = NULL ; 

    // Release the scheduler object
    delete scheduler4 ;
    scheduler4 = NULL ; 

    printf(" %s() Release the ServerOptions\n", __FUNCTION__ );
    // Release the server Initialize resource
    if( srvrInitOpts )
    { 
        free(srvrInitOpts);
        srvrInitOpts = NULL ;
    } 
    printf("RTSP Server Resources Released Done..\n");
    return iRet ;
}

