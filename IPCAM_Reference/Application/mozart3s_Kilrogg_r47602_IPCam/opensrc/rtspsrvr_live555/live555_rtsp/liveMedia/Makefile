INCLUDES = -Iinclude -I../UsageEnvironment/include -I../groupsock/include -I../include 
##### Change the following for your environment: 
CROSS_COMPILE?=		arm-unknown-linux-uclibcgnueabi-
COMPILE_OPTS =		$(INCLUDES) -I. -O2 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DLOCALE_NOT_USED -D ALLOW_RTSP_SERVER_PORT_REUSE=1
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
LINK =			$(CROSS_COMPILE)g++ -o
LINK_OPTS =		
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =	$(LINK_OPTS)
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

LIVEMEDIA_LIB = libliveMedia.$(LIB_SUFFIX)
ALL = $(LIVEMEDIA_LIB)
all:	$(ALL)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c -g $(CPLUSPLUS_FLAGS) $<

#MP3_SOURCE_OBJS = MP3FileSource.$(OBJ) MP3HTTPSource.$(OBJ) MP3Transcoder.$(OBJ) MP3ADU.$(OBJ) MP3ADUdescriptor.$(OBJ) MP3ADUinterleaving.$(OBJ) MP3ADUTranscoder.$(OBJ) MP3StreamState.$(OBJ) MP3Internals.$(OBJ) MP3InternalsHuffman.$(OBJ) MP3InternalsHuffmanTable.$(OBJ) MP3ADURTPSource.$(OBJ)
#MPEG_SOURCE_OBJS =  MPEGVideoStreamFramer.$(OBJ)  H264VideoStreamFramer.$(OBJ) H264VideoStreamDiscreteFramer.$(OBJ) MPEGVideoStreamParser.$(OBJ) ADTSAudioFileSource.$(OBJ)
MPEG_SOURCE_OBJS = MPEGVideoStreamFramer.$(OBJ) MPEG4VideoStreamFramer.$(OBJ) MPEG4VideoStreamDiscreteFramer.$(OBJ) H264VideoStreamFramer.$(OBJ)  MPEGVideoStreamParser.$(OBJ) MPEG4LATMAudioRTPSource.$(OBJ) MPEG4ESVideoRTPSource.$(OBJ) MPEG4GenericRTPSource.$(OBJ) ADTSAudioFileSource.$(OBJ)
#H263_SOURCE_OBJS = H263plusVideoRTPSource.$(OBJ) H263plusVideoStreamFramer.$(OBJ) H263plusVideoStreamParser.$(OBJ)
#AC3_SOURCE_OBJS = AC3AudioStreamFramer.$(OBJ) AC3AudioRTPSource.$(OBJ)
#DV_SOURCE_OBJS = DVVideoStreamFramer.$(OBJ) DVVideoRTPSource.$(OBJ)
#MP3_SINK_OBJS = MP3ADURTPSink.$(OBJ)
#MPEG_SINK_OBJS = MPEG1or2AudioRTPSink.$(OBJ) $(MP3_SINK_OBJS) MPEG1or2VideoRTPSink.$(OBJ) MPEG4LATMAudioRTPSink.$(OBJ) MPEG4GenericRTPSink.$(OBJ) MPEG4ESVideoRTPSink.$(OBJ)
#MPEG_SINK_OBJS = MPEG4GenericRTPSink.$(OBJ) 
MPEG_SINK_OBJS =  MPEG4LATMAudioRTPSink.$(OBJ) MPEG4GenericRTPSink.$(OBJ) MPEG4ESVideoRTPSink.$(OBJ)
#H263_SINK_OBJS = H263plusVideoRTPSink.$(OBJ)
#H263_SINK_OBJS = H263plusVideoRTPSink.$(OBJ)
H264_SINK_OBJS = H264VideoRTPSink.$(OBJ)
#DV_SINK_OBJS = DVVideoRTPSink.$(OBJ)
#AC3_SINK_OBJS = AC3AudioRTPSink.$(OBJ)

MISC_SOURCE_OBJS = MediaSource.$(OBJ) FramedSource.$(OBJ) FramedFileSource.$(OBJ) FramedFilter.$(OBJ) BasicUDPSource.$(OBJ)  $(MPEG_SOURCE_OBJS) StreamReplicator.$(OBJ) JPEGVideoSource.$(OBJ) FSJPEGVideoFramerSource.$(OBJ) FSH264VideoFramerSource.$(OBJ) H264VideoSource.$(OBJ)

MISC_SINK_OBJS = MediaSink.$(OBJ) FileSink.$(OBJ) BasicUDPSink.$(OBJ)  $(H264_SINK_OBJS) SimpleRTPSink.$(OBJ) TCPStreamSink.$(OBJ) JPEGVideoRTPSink.$(OBJ) 

#MISC_FILTER_OBJS = uLawAudioFilter.$(OBJ)
#TRANSPORT_STREAM_TRICK_PLAY_OBJS = MPEG2IndexFromTransportStream.$(OBJ) MPEG2TransportStreamIndexFile.$(OBJ) MPEG2TransportStreamTrickModeFilter.$(OBJ)

RTP_SOURCE_OBJS = RTPSource.$(OBJ) MultiFramedRTPSource.$(OBJ) SimpleRTPSource.$(OBJ) H264VideoRTPSource.$(OBJ) JPEGVideoRTPSource.$(OBJ)
 
RTP_SINK_OBJS = RTPSink.$(OBJ) MultiFramedRTPSink.$(OBJ)  VideoRTPSink.$(OBJ) 
RTP_INTERFACE_OBJS = RTPInterface.$(OBJ)
RTP_OBJS = $(RTP_SOURCE_OBJS) $(RTP_SINK_OBJS) $(RTP_INTERFACE_OBJS)

RTCP_OBJS = RTCP.$(OBJ) rtcp_from_spec.$(OBJ)
RTSP_OBJS = RTSPServer.$(OBJ) RTSPClient.$(OBJ) RTSPCommon.$(OBJ) 
#SIP_OBJS = SIPClient.$(OBJ)

SESSION_OBJS = MediaSession.$(OBJ) ServerMediaSession.$(OBJ)  OnDemandServerMediaSubsession.$(OBJ) FileServerMediaSubsession.$(OBJ)  JPEGVideoFileServerMediaSubsession.$(OBJ) FSH264VideoServerMediaSubsession.$(OBJ) ADTSAudioFileServerMediaSubsession.$(OBJ) MPEG4VideoFileServerMediaSubsession.$(OBJ)
#QUICKTIME_OBJS = QuickTimeFileSink.$(OBJ) QuickTimeGenericRTPSource.$(OBJ)
#AVI_OBJS = AVIFileSink.$(OBJ)

#MATROSKA_FILE_OBJS = MatroskaFile.$(OBJ) MatroskaFileParser.$(OBJ) EBMLNumber.$(OBJ) MatroskaDemuxedTrack.$(OBJ)
#MATROSKA_SERVER_MEDIA_SUBSESSION_VIDEO_OBJS = H264VideoMatroskaFileServerMediaSubsession.$(OBJ) VP8VideoMatroskaFileServerMediaSubsession.$(OBJ)
#MATROSKA_SERVER_MEDIA_SUBSESSION_AUDIO_OBJS = AACAudioMatroskaFileServerMediaSubsession.$(OBJ) AC3AudioMatroskaFileServerMediaSubsession.$(OBJ) MP3AudioMatroskaFileServerMediaSubsession.$(OBJ) VorbisAudioMatroskaFileServerMediaSubsession.$(OBJ)
#MATROSKA_SERVER_MEDIA_SUBSESSION_TEXT_OBJS = T140TextMatroskaFileServerMediaSubsession.$(OBJ)
#MATROSKA_SERVER_MEDIA_SUBSESSION_OBJS = $(MATROSKA_SERVER_MEDIA_SUBSESSION_VIDEO_OBJS) $(MATROSKA_SERVER_MEDIA_SUBSESSION_AUDIO_OBJS) $(MATROSKA_SERVER_MEDIA_SUBSESSION_TEXT_OBJS)
#MATROSKA_RTSP_SERVER_OBJS = MatroskaFileServerDemux.$(OBJ) $(MATROSKA_SERVER_MEDIA_SUBSESSION_OBJS)
#MATROSKA_OBJS = $(MATROSKA_FILE_OBJS) $(MATROSKA_RTSP_SERVER_OBJS)

MISC_OBJS = BitVector.$(OBJ) StreamParser.$(OBJ) DigestAuthentication.$(OBJ) our_md5.$(OBJ) our_md5hl.$(OBJ) Base64.$(OBJ) Locale.$(OBJ)

LIVEMEDIA_LIB_OBJS = Media.$(OBJ) $(MISC_SOURCE_OBJS) $(MISC_SINK_OBJS) $(MISC_FILTER_OBJS) $(RTP_OBJS) $(RTCP_OBJS) $(RTSP_OBJS) $(SIP_OBJS) $(SESSION_OBJS) $(QUICKTIME_OBJS) $(AVI_OBJS) $(TRANSPORT_STREAM_TRICK_PLAY_OBJS) $(MATROSKA_OBJS) $(MISC_OBJS) $(MPEG_SINK_OBJS)

$(LIVEMEDIA_LIB): $(LIVEMEDIA_LIB_OBJS) \
    $(PLATFORM_SPECIFIC_LIB_OBJS)
	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) \
		$(LIVEMEDIA_LIB_OBJS)

Media.$(CPP):		include/Media.hh
include/Media.hh:	include/liveMedia_version.hh
MediaSource.$(CPP):	include/MediaSource.hh
include/MediaSource.hh:		include/Media.hh
FramedSource.$(CPP):	include/FramedSource.hh
include/FramedSource.hh:	include/MediaSource.hh
FramedFileSource.$(CPP): include/FramedFileSource.hh
include/FramedFileSource.hh:	include/FramedSource.hh
FramedFilter.$(CPP):	include/FramedFilter.hh
include/FramedFilter.hh:	include/FramedSource.hh
RTPSource.$(CPP):	include/RTPSource.hh
include/RTPSource.hh:		include/FramedSource.hh include/RTPInterface.hh
include/RTPInterface.hh:	include/Media.hh
MultiFramedRTPSource.$(CPP):	include/MultiFramedRTPSource.hh
include/MultiFramedRTPSource.hh:	include/RTPSource.hh
SimpleRTPSource.$(CPP):	include/SimpleRTPSource.hh
include/SimpleRTPSource.hh:	include/MultiFramedRTPSource.hh
#H261VideoRTPSource.$(CPP):	include/H261VideoRTPSource.hh
#include/H261VideoRTPSource.hh:	include/MultiFramedRTPSource.hh
JPEGVideoRTPSource.$(CPP):      include/JPEGVideoRTPSource.hh
include/JPEGVideoRTPSource.hh:  include/MultiFramedRTPSource.hh
JPEGVideoSource.$(CPP):         include/JPEGVideoSource.hh
H264VideoSource.$(CPP):          H264VideoSource.hh
FSJPEGVideoFramerSource.$(CPP):          FSJPEGVideoFramerSource.hh
include/JPEGVideoSource.hh:     include/FramedSource.hh
JPEGVideoRTPSink.$(CPP):        include/JPEGVideoRTPSink.hh include/JPEGVideoSource.hh
include/JPEGVideoRTPSink.hh:    include/VideoRTPSink.hh
H264VideoRTPSource.$(CPP):      include/H264VideoRTPSource.hh include/Base64.hh
include/H264VideoRTPSource.hh:  include/MultiFramedRTPSource.hh
#QCELPAudioRTPSource.$(CPP):	include/QCELPAudioRTPSource.hh include/MultiFramedRTPSource.hh include/FramedFilter.hh
#include/QCELPAudioRTPSource.hh:		include/RTPSource.hh
#AMRAudioRTPSource.$(CPP):	include/AMRAudioRTPSource.hh include/MultiFramedRTPSource.hh
#include/AMRAudioRTPSource.hh:		include/RTPSource.hh include/AMRAudioSource.hh
#JPEGVideoRTPSource.$(CPP):	include/JPEGVideoRTPSource.hh
#include/JPEGVideoRTPSource.hh:	include/MultiFramedRTPSource.hh
#VorbisAudioRTPSource.$(CPP):	include/VorbisAudioRTPSource.hh
#include/VorbisAudioRTPSource.hh:	include/MultiFramedRTPSource.hh
#VP8VideoRTPSource.$(CPP):	include/VP8VideoRTPSource.hh
#include/VP8VideoRTPSource.hh:	include/MultiFramedRTPSource.hh
BasicUDPSource.$(CPP):		include/BasicUDPSource.hh
include/BasicUDPSource.hh:	include/FramedSource.hh
#ProxyServerMediaSession.$(CPP):         include/liveMedia.hh
#DeviceSource.$(CPP):	include/DeviceSource.hh
#include/DeviceSource.hh:	include/FramedSource.hh
#AudioInputDevice.$(CPP):	include/AudioInputDevice.hh
#include/AudioInputDevice.hh:	include/FramedSource.hh
#WAVAudioFileSource.$(CPP):	include/WAVAudioFileSource.hh include/InputFile.hh
#include/WAVAudioFileSource.hh:	include/AudioInputDevice.hh
#MPEG1or2Demux.$(CPP):	include/MPEG1or2Demux.hh include/MPEG1or2DemuxedElementaryStream.hh StreamParser.hh
#include/MPEG1or2Demux.hh:		include/FramedSource.hh
#include/MPEG1or2DemuxedElementaryStream.hh:	include/MPEG1or2Demux.hh
StreamParser.hh:	include/FramedSource.hh
#MPEG1or2DemuxedElementaryStream.$(CPP):	include/MPEG1or2DemuxedElementaryStream.hh
#MPEGVideoStreamFramer.$(CPP):	MPEGVideoStreamParser.hh
MPEGVideoStreamParser.hh:	StreamParser.hh include/MPEGVideoStreamFramer.hh
include/MPEGVideoStreamFramer.hh:	include/FramedFilter.hh
#MPEG1or2VideoStreamFramer.$(CPP):	include/MPEG1or2VideoStreamFramer.hh MPEGVideoStreamParser.hh
#include/MPEG1or2VideoStreamFramer.hh:	include/MPEGVideoStreamFramer.hh
#MPEG1or2VideoStreamDiscreteFramer.$(CPP):	include/MPEG1or2VideoStreamDiscreteFramer.hh
#include/MPEG1or2VideoStreamDiscreteFramer.hh:	include/MPEG1or2VideoStreamFramer.hh
MPEG4VideoStreamFramer.$(CPP):	include/MPEG4VideoStreamFramer.hh MPEGVideoStreamParser.hh include/MPEG4LATMAudioRTPSource.hh
include/MPEG4VideoStreamFramer.hh:	include/MPEGVideoStreamFramer.hh
MPEG4VideoStreamDiscreteFramer.$(CPP):	include/MPEG4VideoStreamDiscreteFramer.hh
include/MPEG4VideoStreamDiscreteFramer.hh:	include/MPEG4VideoStreamFramer.hh
H264VideoStreamFramer.$(CPP):	include/H264VideoStreamFramer.hh MPEGVideoStreamParser.hh BitVector.hh include/H264VideoRTPSource.hh
include/H264VideoStreamFramer.hh:	include/MPEGVideoStreamFramer.hh
H264VideoStreamDiscreteFramer.$(CPP):	include/H264VideoStreamDiscreteFramer.hh
include/H264VideoStreamDiscreteFramer.hh:	include/H264VideoStreamFramer.hh
MPEGVideoStreamParser.$(CPP):	MPEGVideoStreamParser.hh
FSH264VideoServersession.$(CPP): FSH264VideoServersession.hh
#MPEG1or2AudioStreamFramer.$(CPP):	include/MPEG1or2AudioStreamFramer.hh StreamParser.hh MP3Internals.hh
#include/MPEG1or2AudioStreamFramer.hh:	include/FramedFilter.hh
#MPEG1or2AudioRTPSource.$(CPP):	include/MPEG1or2AudioRTPSource.hh
#include/MPEG1or2AudioRTPSource.hh:	include/MultiFramedRTPSource.hh
MPEG4LATMAudioRTPSource.$(CPP):	include/MPEG4LATMAudioRTPSource.hh
include/MPEG4LATMAudioRTPSource.hh:	include/MultiFramedRTPSource.hh
MPEG4ESVideoRTPSource.$(CPP):	include/MPEG4ESVideoRTPSource.hh
include/MPEG4ESVideoRTPSource.hh:	include/MultiFramedRTPSource.hh
MPEG4GenericRTPSource.$(CPP):	include/MPEG4GenericRTPSource.hh BitVector.hh  include/MPEG4LATMAudioRTPSource.hh
include/MPEG4GenericRTPSource.hh:	include/MultiFramedRTPSource.hh
#MP3FileSource.$(CPP):	include/MP3FileSource.hh MP3StreamState.hh include/InputFile.hh
#include/MP3FileSource.hh:	include/FramedFileSource.hh
#MP3StreamState.hh:	MP3Internals.hh
#MP3Internals.hh:	BitVector.hh
#MP3HTTPSource.$(CPP):	include/MP3HTTPSource.hh MP3StreamState.hh
#include/MP3HTTPSource.hh:	include/MP3FileSource.hh
#MP3Transcoder.$(CPP):	include/MP3ADU.hh include/MP3Transcoder.hh
#include/MP3ADU.hh:		include/FramedFilter.hh
#include/MP3Transcoder.hh:	include/MP3ADU.hh include/MP3ADUTranscoder.hh
#include/MP3ADUTranscoder.hh:	include/FramedFilter.hh
#MP3ADU.$(CPP):		include/MP3ADU.hh MP3ADUdescriptor.hh MP3Internals.hh
#MP3ADUdescriptor.$(CPP):	MP3ADUdescriptor.hh
#MP3ADUinterleaving.$(CPP):	include/MP3ADUinterleaving.hh MP3ADUdescriptor.hh
#include/MP3ADUinterleaving.hh:	include/FramedFilter.hh
#MP3ADUTranscoder.$(CPP):	include/MP3ADUTranscoder.hh MP3Internals.hh
#MP3StreamState.$(CPP):	MP3StreamState.hh include/InputFile.hh
#MP3Internals.$(CPP):	MP3InternalsHuffman.hh
#MP3InternalsHuffman.hh:	MP3Internals.hh
#MP3InternalsHuffman.$(CPP):	MP3InternalsHuffman.hh
#MP3InternalsHuffmanTable.$(CPP):	MP3InternalsHuffman.hh
#MP3ADURTPSource.$(CPP):	include/MP3ADURTPSource.hh MP3ADUdescriptor.hh
#include/MP3ADURTPSource.hh:	include/MultiFramedRTPSource.hh
#MPEG1or2VideoRTPSource.$(CPP):	include/MPEG1or2VideoRTPSource.hh
#include/MPEG1or2VideoRTPSource.hh:	include/MultiFramedRTPSource.hh
#MPEG2TransportStreamMultiplexor.$(CPP):	include/MPEG2TransportStreamMultiplexor.hh
#include/MPEG2TransportStreamMultiplexor.hh:	include/FramedSource.hh include/MPEG1or2Demux.hh
#MPEG2TransportStreamFromPESSource.$(CPP):	include/MPEG2TransportStreamFromPESSource.hh
#include/MPEG2TransportStreamFromPESSource.hh:	include/MPEG2TransportStreamMultiplexor.hh include/MPEG1or2DemuxedElementaryStream.hh

ADTSAudioFileSource.$(CPP):	include/ADTSAudioFileSource.hh 
include/ADTSAudioFileSource.hh:	include/FramedFileSource.hh
#if 0
MPEG2TransportStreamFromESSource.$(CPP):	include/MPEG2TransportStreamFromESSource.hh
include/MPEG2TransportStreamFromESSource.hh:	include/MPEG2TransportStreamMultiplexor.hh
MPEG2TransportStreamFramer.$(CPP):	include/MPEG2TransportStreamFramer.hh
include/MPEG2TransportStreamFramer.hh:	include/FramedFilter.hh include/MPEG2TransportStreamIndexFile.hh
ADTSAudioFileSource.$(CPP):	include/ADTSAudioFileSource.hh
include/ADTSAudioFileSource.hh:	include/FramedFileSource.hh
H263plusVideoRTPSource.$(CPP):	include/H263plusVideoRTPSource.hh
include/H263plusVideoRTPSource.hh:	include/MultiFramedRTPSource.hh
H263plusVideoStreamFramer.$(CPP):	include/H263plusVideoStreamFramer.hh H263plusVideoStreamParser.hh
include/H263plusVideoStreamFramer.hh:	include/FramedFilter.hh
H263plusVideoStreamParser.hh:	StreamParser.hh
H263plusVideoStreamParser.$(CPP):	H263plusVideoStreamParser.hh include/H263plusVideoStreamFramer.hh
AC3AudioStreamFramer.$(CPP):	include/AC3AudioStreamFramer.hh StreamParser.hh
include/AC3AudioStreamFramer.hh:	include/FramedFilter.hh
AC3AudioRTPSource.$(CPP):	include/AC3AudioRTPSource.hh
include/AC3AudioRTPSource.hh:	include/MultiFramedRTPSource.hh
DVVideoRTPSource.$(CPP):	include/DVVideoRTPSource.hh
include/DVVideoRTPSource.hh:	include/MultiFramedRTPSource.hh
JPEGVideoSource.$(CPP):		include/JPEGVideoSource.hh
include/JPEGVideoSource.hh:	include/FramedSource.hh
#AMRAudioSource.$(CPP):	include/AMRAudioSource.hh
#include/AMRAudioSource.hh:	include/FramedSource.hh
#AMRAudioFileSource.$(CPP):	include/AMRAudioFileSource.hh include/InputFile.hh
include/AMRAudioFileSource.hh:	include/AMRAudioSource.hh
#endif
#InputFile.$(CPP):		include/InputFile.hh
StreamReplicator.$(CPP):	include/StreamReplicator.hh
include/StreamReplicator.hh:	include/FramedSource.hh
MediaSink.$(CPP):	include/MediaSink.hh
include/MediaSink.hh:		include/FramedSource.hh
#FileSink.$(CPP):	include/FileSink.hh
include/FileSink.hh:		include/MediaSink.hh
BasicUDPSink.$(CPP):	include/BasicUDPSink.hh
include/BasicUDPSink.hh:	include/MediaSink.hh
#H264VideoFileSink.$(CPP):       include/H264VideoFileSink.hh include/OutputFile.hh include/H264VideoRTPSource.hh
#include/H264VideoFileSink.hh:   include/FileSink.hh
RTPSink.$(CPP):		include/RTPSink.hh
include/RTPSink.hh:		include/MediaSink.hh include/RTPInterface.hh
MultiFramedRTPSink.$(CPP):	include/MultiFramedRTPSink.hh
include/MultiFramedRTPSink.hh:		include/RTPSink.hh

AudioRTPSink.$(CPP):		include/AudioRTPSink.hh
include/AudioRTPSink.hh:	include/MultiFramedRTPSink.hh
VideoRTPSink.$(CPP):		include/VideoRTPSink.hh
include/VideoRTPSink.hh:	include/MultiFramedRTPSink.hh
#TextRTPSink.$(CPP):		include/TextRTPSink.hh
#include/TextRTPSink.hh:		include/MultiFramedRTPSink.hh
RTPInterface.$(CPP):		include/RTPInterface.hh
#MPEG1or2AudioRTPSink.$(CPP):	include/MPEG1or2AudioRTPSink.hh
#include/MPEG1or2AudioRTPSink.hh:	include/AudioRTPSink.hh
#MP3ADURTPSink.$(CPP):	include/MP3ADURTPSink.hh
#include/MP3ADURTPSink.hh:	include/AudioRTPSink.hh
#MPEG1or2VideoRTPSink.$(CPP):	include/MPEG1or2VideoRTPSink.hh include/MPEG1or2VideoStreamFramer.hh
#include/MPEG1or2VideoRTPSink.hh:	include/VideoRTPSink.hh
MPEG4LATMAudioRTPSink.$(CPP):	include/MPEG4LATMAudioRTPSink.hh
include/MPEG4LATMAudioRTPSink.hh:	include/AudioRTPSink.hh
MPEG4GenericRTPSink.$(CPP):	include/MPEG4GenericRTPSink.hh include/Locale.hh
include/MPEG4GenericRTPSink.hh:	include/MultiFramedRTPSink.hh
MPEG4ESVideoRTPSink.$(CPP):	include/MPEG4ESVideoRTPSink.hh include/MPEG4VideoStreamFramer.hh include/MPEG4LATMAudioRTPSource.hh
include/MPEG4ESVideoRTPSink.hh: include/VideoRTPSink.hh
#H263plusVideoRTPSink.$(CPP):	include/H263plusVideoRTPSink.hh
#include/H263plusVideoRTPSink.hh:	include/VideoRTPSink.hh
H264VideoRTPSink.$(CPP):	include/H264VideoRTPSink.hh include/H264VideoStreamFramer.hh include/Base64.hh include/H264VideoRTPSource.hh
include/H264VideoRTPSink.hh:	include/VideoRTPSink.hh include/FramedFilter.hh
#DVVideoRTPSink.$(CPP):	include/DVVideoRTPSink.hh
#include/DVVideoRTPSink.hh:	include/VideoRTPSink.hh include/DVVideoStreamFramer.hh
#include/DVVideoStreamFramer.hh:	include/FramedFilter.hh
#AC3AudioRTPSink.$(CPP):		include/AC3AudioRTPSink.hh
#include/AC3AudioRTPSink.hh:	include/AudioRTPSink.hh
#VorbisAudioRTPSink.$(CPP):	include/VorbisAudioRTPSink.hh include/Base64.hh
#include/VorbisAudioRTPSink.hh:	include/AudioRTPSink.hh
#VP8VideoRTPSink.$(CPP):		include/VP8VideoRTPSink.hh
#include/VP8VideoRTPSink.hh:	include/VideoRTPSink.hh
#GSMAudioRTPSink.$(CPP):		include/GSMAudioRTPSink.hh
#include/GSMAudioRTPSink.hh:	include/AudioRTPSink.hh
#JPEGVideoRTPSink.$(CPP):	include/JPEGVideoRTPSink.hh include/JPEGVideoSource.hh
#include/JPEGVideoRTPSink.hh:	include/VideoRTPSink.hh
SimpleRTPSink.$(CPP):		include/SimpleRTPSink.hh
include/SimpleRTPSink.hh:	include/MultiFramedRTPSink.hh
#AMRAudioRTPSink.$(CPP):		include/AMRAudioRTPSink.hh include/AMRAudioSource.hh
#include/AMRAudioRTPSink.hh:	include/AudioRTPSink.hh
#T140TextRTPSink.$(CPP):		include/T140TextRTPSink.hh
#include/T140TextRTPSink.hh:	include/TextRTPSink.hh include/FramedFilter.hh
TCPStreamSink.$(CPP):		include/TCPStreamSink.hh
include/TCPStreamSink.hh:	include/MediaSink.hh
RTCP.$(CPP):		include/RTCP.hh rtcp_from_spec.h
include/RTCP.hh:		include/RTPSink.hh include/RTPSource.hh
rtcp_from_spec.$(C):	rtcp_from_spec.h
RTSPServer.$(CPP):	include/RTSPServer.hh include/RTSPCommon.hh include/Base64.hh
include/RTSPServer.hh:		include/ServerMediaSession.hh include/DigestAuthentication.hh include/RTSPCommon.hh
include/ServerMediaSession.hh:	include/Media.hh include/FramedSource.hh include/RTPInterface.hh
RTSPClient.$(CPP):	include/RTSPClient.hh  include/RTSPCommon.hh include/Base64.hh include/Locale.hh our_md5.h
include/RTSPClient.hh:		include/MediaSession.hh include/DigestAuthentication.hh
RTSPCommon.$(CPP):	include/RTSPCommon.hh include/Locale.hh
#RTSPServerSupportingHTTPStreaming.$(CPP):	include/RTSPServerSupportingHTTPStreaming.hh include/RTSPCommon.hh
#include/RTSPServerSupportingHTTPStreaming.hh:	include/RTSPServer.hh include/ByteStreamMemoryBufferSource.hh include/TCPStreamSink.hh
MediaSession.$(CPP):	include/liveMedia.hh include/Locale.hh
include/MediaSession.hh:	include/RTCP.hh include/FramedFilter.hh
ServerMediaSession.$(CPP):	include/ServerMediaSession.hh
OnDemandServerMediaSubsession.$(CPP):	include/OnDemandServerMediaSubsession.hh
include/OnDemandServerMediaSubsession.hh:	include/ServerMediaSession.hh include/RTPSink.hh include/BasicUDPSink.hh include/RTCP.hh
FileServerMediaSubsession.$(CPP):	include/FileServerMediaSubsession.hh
include/FileServerMediaSubsession.hh:	include/OnDemandServerMediaSubsession.hh
#H264VideoFileServerMediaSubsession.$(CPP):	include/H264VideoFileServerMediaSubsession.hh include/H264VideoRTPSink.hh include/H264VideoStreamFramer.hh include/JPEGVideoFileServerMediaSubsession.hh
#include/H264VideoFileServerMediaSubsession.hh:	include/FileServerMediaSubsession.hh
ADTSAudioFileServerMediaSubsession.$(CPP):      include/ADTSAudioFileServerMediaSubsession.hh include/ADTSAudioFileSource.hh include/MPEG4GenericRTPSink.hh
include/ADTSAudioFileServerMediaSubsession.hh:  include/FileServerMediaSubsession.hh
JPEGVideoFileServerMediaSubsession.$(CPP):	include/H264VideoFileServerMediaSubsession.hh include/JPEGVideoRTPSink.hh include/JPEGVideoFileServerMediaSubsession.hh
include/H264VideoFileServerMediaSubsession.hh:	include/FileServerMediaSubsession.hh
include/JPEGVideoFileServerMediaSubsession.hh:	include/FileServerMediaSubsession.hh
EBMLNumber.$(CPP): EBMLNumber.hh
BitVector.$(CPP):	BitVector.hh
StreamParser.$(CPP):	StreamParser.hh
DigestAuthentication.$(CPP):	include/DigestAuthentication.hh our_md5.h
our_md5.$(C):		our_md5.h
our_md5hl.$(C):		our_md5.h
Base64.$(CPP):	include/Base64.hh
Locale.$(CPP):	include/Locale.hh
FSH264VideoFramerSource.$(CPP): FSH264VideoFramerSource.hh

include/liveMedia.hh::  include/BasicUDPSink.hh  include/H264VideoRTPSink.hh include/H264VideoStreamDiscreteFramer.hh include/SimpleRTPSink.hh  include/BasicUDPSource.hh include/SimpleRTPSource.hh include/JPEGVideoRTPSink.hh include/JPEGVideoRTPSource.hh include/JPEGVideoSource.hh include/MPEG4ESVideoRTPSink.hh include/MPEG4ESVideoRTPSource.hh include/MPEG4LATMAudioRTPSource.hh include/MPEG4LATMAudioRTPSink.hh include/MPEG4ESVideoRTPSource.hh 

include/liveMedia.hh:: include/H264VideoRTPSource.hh include/StreamReplicator.hh  include/ADTSAudioFileSource.hh include/MPEG4GenericRTPSink.hh include/MPEG4VideoStreamDiscreteFramer.hh

include/liveMedia.hh:: include/RTSPClient.hh include/H264VideoFileServerMediaSubsession.hh include/TCPStreamSink.hh  include/JPEGVideoFileServerMediaSubsession.hh include/ADTSAudioFileServerMediaSubsession.hh include/MPEG4VideoFileServerMediaSubsession.hh

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

##### Any additional, platform-specific rules come here:
