#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include "SockSrc.h"
#include "UnixSockUtil.h"
#include "ubuffer.h"
#include "ubuffer_limit.h"


int main(int argc, char** argv)
{
    char *szRecvSockPath = NULL;
    char *szCmdFiFoPath = NULL;
    TSockSrcInitOpts tSockSrcInitOpts;
    HANDLE hSockSrc;
   
    TSrcUBufInfo tLMSrcUBufInfo;
    TUBuffer	*ptUBuffHdr = NULL;
    TUBufferConfAAC4 *ptAac4Conf = NULL;
    TUBufferConfGAMR *ptGamrConf = NULL;
    FILE *pfile = NULL;
    DWORD dwFrameCount = 0;
    DWORD dwFrames=0;

    BYTE *pbySpecConf;
    int iSpecConfLen;
    
    if(argc!=4)
    {
      printf("/usr/sbin/aenc_receiver [sock_path] [cmd_fifo_path] [frame_count]\n");
      return 0;
    }
    szRecvSockPath=argv[1];
    szCmdFiFoPath=argv[2];
    dwFrames=strtol(argv[3],NULL,10);
    memset(&tSockSrcInitOpts,0x0,sizeof(TSockSrcInitOpts));
    tSockSrcInitOpts.szRecvSockPath=szRecvSockPath;
    tSockSrcInitOpts.szCmdFiFoPath=szCmdFiFoPath;
    if(SockSrc_Initial(&hSockSrc,&tSockSrcInitOpts)==S_FAIL)
    {
	  printf("[%s:%s:%d] SockSrc_Initial error!", __func__, __FILE__, __LINE__);
	  return -1;
	
    }
    
    //Send command to aenc to get the CI information, ie. FOURCC_CONF ubuffer.
    SockSrc_EventHandler(hSockSrc, letNeedIntra);
    //Send command to aenc to start encoding
    SockSrc_EventHandler(hSockSrc, letBitstrmStart);
    printf("Get bitstream....\n");
    int i=0;
    while(dwFrameCount <  dwFrames)
    {
	printf("dwFrameCount=%lu\n",dwFrameCount);
        if (SockSrc_GetUBuffer(hSockSrc, &tLMSrcUBufInfo) != S_OK)
        {
            printf("get audio bitstream fail\n");
        }
        else
        {
            ptUBuffHdr=(TUBuffer *)tLMSrcUBufInfo.pbyUBuffHdr;

            if (ptUBuffHdr->dwDataType == FOURCC_CONF)  //receive conf ubuff
            {
                if (ptUBuffHdr->dwFollowingDataType == FOURCC_AAC4)
                {
                    if ((pfile = fopen("/tmp/test.aac", "wb")) == NULL)
                    {
                        printf("Open dump file fail\n");
                        exit(1);
                    }
                    ptAac4Conf = (TUBufferConfAAC4 *)ptUBuffHdr;
                  	pbySpecConf = ((BYTE *)ptAac4Conf)+sizeof(TUBufferConfAAC4);
                    iSpecConfLen = ((int)ptAac4Conf->dwSize)-sizeof(TUBufferConfAAC4);

                    printf("AAC4 with Channel number %lu\n", ptAac4Conf->dwChannelNumber);
                    printf("          SampleRate %lu\n", ptAac4Conf->dwSampleRate);
                    printf("          profile level %lu\n", ptAac4Conf->dwProfileLevel);
                    printf("sepc len %d\n", iSpecConfLen);
                    //fwrite(pbySpecConf, iSpecConfLen, 1, pfile);
                }
                else if (ptUBuffHdr->dwFollowingDataType == FOURCC_GAMR)
                {
                    if ((pfile = fopen("/tmp/test.amr", "wb")) == NULL)
                    {
                        printf("Open dump file fail\n");
                        exit(1);
                    }
                    ptGamrConf = (TUBufferConfGAMR *)ptUBuffHdr;
                    printf("GAMR with sample rate %lu\n", ptGamrConf->dwSampleRate);
                }
            }
            else
            {
                if (ptUBuffHdr->dwDataType == FOURCC_AAC4)
                {
                    if (i>1)
                    {
                    printf("frame %ld :sec %ld, usec %ld\n", dwFrameCount, ptUBuffHdr->dwSec, ptUBuffHdr->dwUSec);
                    printf("         :size %d\n", tLMSrcUBufInfo.iUBuffPayloadLen);
                    fwrite(tLMSrcUBufInfo.pbyUBuffPayload, tLMSrcUBufInfo.iUBuffPayloadLen, 1, pfile);
                    }
                    i++;
                }
                else if (ptUBuffHdr->dwDataType == FOURCC_GAMR)
                {
                    printf("frame %ld :sec %ld, usec %ld\n", dwFrameCount, ptUBuffHdr->dwSec, ptUBuffHdr->dwUSec);
                    printf("         :size %d\n", tLMSrcUBufInfo.iUBuffPayloadLen);
                    fwrite(tLMSrcUBufInfo.pbyUBuffPayload, tLMSrcUBufInfo.iUBuffPayloadLen, 1, pfile);
                }
            }
            dwFrameCount++;
        }
    }

    //Send command to aenc to stop encoding.
    SockSrc_EventHandler(hSockSrc, letBitstrmStop);
SockSrc_EventHandler(hSockSrc, letBitstrmStop);
    SockSrc_Release(&hSockSrc);

    fclose(pfile);

    return 0;
}
