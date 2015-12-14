#ifndef _MP4_H_
#define _MP4_H_

#include "ubuffer.h"
#include "typedef.h"
#include "global_codec.h"

#define MP4_VERSION			MAKEFOURCC(2, 0, 0, 3)

typedef struct mp4_src_ubuf_info
{
	BYTE	*pbyHdr;
	DWORD	dwHdrLen;
	BYTE	*pbyPld;
	DWORD	dwPldLen;
} TMP4SrcUBufInfo;

typedef struct mp4_initial_option
{
	DWORD	dwVersion;
} TMP4IintOpt;
#ifdef __cplusplus
extern "C" {
#endif
SCODE mp4_int(HANDLE *phObj, TMP4IintOpt *ptOpt);
SCODE mp4_rls(HANDLE *phObj);
SCODE mp4_create(HANDLE hObj, CHAR *szFileName, DWORD dwTimeScale);
DWORD mp4_add_track(HANDLE hObj, TMP4SrcUBufInfo *ptConfInfo, DWORD dwTimeScale); //return track ID. If track ID is 0, it means adding track fails.
//DWORD mp4_add_audio_track(HANDLE hObj, TSrcUBufInfo *ptConfInfo, DWORD dwTimeScale);
SCODE mp4_add_sample(HANDLE hObj, DWORD dwTrackID, TMP4SrcUBufInfo *ptUBufInfo);
SCODE mp4_flush_sample(HANDLE hObject);
SCODE mp4_commit_sample(HANDLE hObject);
void mp4_close(HANDLE hObj);
#ifdef __cplusplus
}
#endif

#endif //_MP4_H_
