/*
 *
 *
 *
 */
/* ============================================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "VideoDisp.h"
#include "MemMgr.h"

/* =========================================================================================== */
#define MAX_WIDTH           1920
#define MAX_HEIGHT          1080
#define ALIGN_PADDING 7
#define ALIGN_MASK 0xFFFFFFF8


/* Repeat from first frame */
//#define REPEAT

/*
 * Only read 4 fram from input, and reused frame in memory.
 * For queue/deque frame rate test
 */
//#define READONCE_TEST

/*
 * define: Use VideoDisp_QueueBuf/VideoDisp_DeQueueBuf
 * not define: use VideoDisp_Get_Buf/VideoDisp_Release_Buf
 */
//#define QUEUE_METHOD
/* =========================================================================================== */
SDWORD GetParam(FILE *pfInput)
{
    CHAR acTmpText[200];
    SDWORD sdwRet;

    fscanf(pfInput, "%s", acTmpText);
    sdwRet = atoi(acTmpText);
    fgets(acTmpText, 200, pfInput);

    return sdwRet;
}
/* =========================================================================================== */
#ifndef QUEUE_METHOD
int main (int argc, char **argv)
{
    HANDLE hObject;
    HANDLE hMemObject;
    TVideoDispInitOptions tInitOptions;
    TVideoDispState tState;
    TMemMgrInitOptions tMemMgrInitOptions;
    TMemMgrState tMemMgrState;

    CHAR acTmpText[200];
    FILE *pfInput;
    FILE *pfConfig;

    BYTE *pbyBuffer;

    DWORD dwInFrameSize, dwDisplayMode, dwOutputSize, dwOutputFreq;
    DWORD dwFrameNum, dwInFormat, dwOutFormat, dwWidth, dwHeight, dwStride, dwSaturation;
    DWORD i, j;
    BOOL bHorzSub, bVertSub, bPALEn;
    SDWORD sdwBrightness, sdwContrast;

    if (argc!=2)
    {
        printf("Usage: videodisp config_file\n");
        exit(1);
    }

    if ((pfConfig=fopen(argv[1], "rb")) == NULL)
    {
        printf("Open input image file fail !!\n");
        exit(1);
    }

    fscanf(pfConfig, "%s", acTmpText);
    if ((pfInput=fopen(acTmpText, "rb")) == NULL)
    {
        printf("Open input image file %s fail !!\n", acTmpText);
        exit(1);
    }

    tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
    tMemMgrInitOptions.pObjectMem = (void *)NULL;
    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
    {
        printf("Initial memory manager object fail !!\n");
        exit(1);
    }

    dwDisplayMode = GetParam(pfConfig);
    dwWidth = GetParam(pfConfig);
    dwHeight = GetParam(pfConfig);
    dwStride = GetParam(pfConfig);
    dwInFormat = GetParam(pfConfig);
    dwOutFormat = GetParam(pfConfig);
    dwFrameNum = GetParam(pfConfig);
    bPALEn = GetParam(pfConfig);
    dwSaturation = GetParam(pfConfig);
    sdwBrightness = GetParam(pfConfig);
    sdwContrast = GetParam(pfConfig);
    dwOutputSize = GetParam(pfConfig);
    dwOutputFreq = GetParam(pfConfig);
    dwInFrameSize = dwWidth * dwHeight;

    tMemMgrState.dwBusNum = VIDEODISP_IN_BUS_NUM;
    tMemMgrState.dwSize = dwWidth*dwHeight*12;
    tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;
    if ((pbyBuffer=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate input frame buffer fail !!\n");
        exit(1);
    }
    else
    {
        printf("Request %ld bytes input frame buffer succeed !!\n", dwWidth*dwHeight*12);
    }
    memset(pbyBuffer, 128, dwWidth*dwHeight*12);

    tInitOptions.dwVersion = VIDEODISP_VERSION;
    tInitOptions.pObjectMem = NULL;
    tInitOptions.eOutFormat = (EVideoSignalFormat)dwOutFormat;
    tInitOptions.epixelInFormat = (EPixelFormatFlags)dwInFormat;
    tInitOptions.eOutputSize = (EVideoSignalSize)dwOutputSize;
    tInitOptions.eOutputRefreshRate = (EVideoSignalFrequency)dwOutputFreq;
    tInitOptions.dwMaxInWidth = dwWidth;
    tInitOptions.dwMaxInHeight = dwHeight;
    tInitOptions.dwInWidth = dwWidth;
    tInitOptions.dwInHeight = dwHeight;
    tInitOptions.bPALEn = bPALEn;
    tInitOptions.sdwBrightness = sdwBrightness;
    tInitOptions.sdwContrast = sdwContrast;
    tInitOptions.dwSaturation = dwSaturation;
    tInitOptions.pbyInBuffer = pbyBuffer;

    if (VideoDisp_Initial(&hObject, &tInitOptions) != S_OK)
    {
        printf("VideoDisp initial failed!\n");
        exit(1);
    }

    if (dwInFormat == (DWORD)YUV420)
    {
        bHorzSub = 1;
        bVertSub = 1;
        dwInFrameSize = dwInFrameSize*3 >> 1;
    }
    else if (dwInFormat == (DWORD)YUV422)
    {
        bHorzSub = 1;
        bVertSub = 0;
        dwInFrameSize = dwInFrameSize * 2;
    }
    else
    {
        printf("Unsupported input format!!");
        exit(1);
    }

    VideoDisp_Start(hObject);
#ifdef REPEAT
    while(1)
#endif
    {
        rewind(pfInput);
        for (i=0; i <dwFrameNum ; i++)
        {

            printf("Frame %d\n", (int)i);
            if (VideoDisp_Get_Buf(hObject, &tState) != S_OK)
            {
                VideoDisp_WaitComplete(hObject);
                if (VideoDisp_Get_Buf(hObject, &tState) != S_OK)
                {
                    printf("Get buffer failed !\n");
                    break;
                }
            }

            for (j=0; j<dwHeight; j++)
            {
                fread((void *)(tState.pbyYFrame+j*dwStride), dwWidth, 1, pfInput);
            }
            for (j=0; j<dwHeight>>bVertSub; j++)
            {
                fread((void *)(tState.pbyCbFrame+j*(dwStride>>bHorzSub)), dwWidth>>bHorzSub, 1, pfInput);
            }
            for (j=0; j<dwHeight>>bVertSub; j++)
            {
                fread((void *)(tState.pbyCrFrame+j*(dwStride>>bHorzSub)), dwWidth>>bHorzSub, 1, pfInput);
            }

            if (VideoDisp_Release_Buf(hObject, &tState) != S_OK)
            {
                printf("Release buffer failed !\n");
                break;
            }
        }
    }
    VideoDisp_Stop(hObject);

    if (VideoDisp_Release(&hObject) != S_OK)
    {
        printf("VideoDisp release failed!\n");
        exit(1);
    }
    fclose(pfInput);
    fclose(pfConfig);

    if (MemMgr_Release(&hMemObject) != S_OK)
    {
        printf("Release memory manager object fail !!\n");
        exit(1);
    }

    return 0;
}
#else /* defined QUEUE_METHOD */
int main (int argc, char **argv)
{
    HANDLE hObject;
    HANDLE hMemObject;
    TVideoDispInitOptions tInitOptions;
    TVideoDispState tState;
    TMemMgrInitOptions tMemMgrInitOptions;
    TMemMgrState tMemMgrState;

    CHAR acTmpText[200];
    FILE *pfInput;
    FILE *pfConfig;

    BYTE *pbyBuffer;

    DWORD dwDisplayMode, dwOutputSize, dwOutputFreq;
    DWORD dwFrameNum, dwInFormat, dwOutFormat, dwWidth, dwHeight, dwStride, dwSaturation;
    DWORD i, j, count=0;
#ifdef READONCE_TEST
    int k = 0;
#endif
    BOOL bPALEn;
    DWORD dwHorzSub, dwVertSub;
    SDWORD sdwBrightness, sdwContrast;
    DWORD dwYInFrameSize, dwCbInFrameSize, dwCrInFrameSize;


    if (argc!=2)
    {
        printf("Usage: videodisp config_file\n");
        exit(1);
    }

    if ((pfConfig=fopen(argv[1], "rb")) == NULL)
    {
        printf("Open input image file fail !!\n");
        exit(1);
    }

    fscanf(pfConfig, "%s", acTmpText);
    if ((pfInput=fopen(acTmpText, "rb")) == NULL)
    {
        printf("Open input image file %s fail !!\n", acTmpText);
        exit(1);
    }

    tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
    tMemMgrInitOptions.pObjectMem = (void *)NULL;
    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
    {
        printf("Initial memory manager object fail !!\n");
        exit(1);
    }

    dwDisplayMode = GetParam(pfConfig);
    dwWidth = GetParam(pfConfig);
    dwHeight = GetParam(pfConfig);
    dwStride = GetParam(pfConfig);
    dwInFormat = GetParam(pfConfig);
    dwOutFormat = GetParam(pfConfig);
    dwFrameNum = GetParam(pfConfig);
    bPALEn = GetParam(pfConfig);
    dwSaturation = GetParam(pfConfig);
    sdwBrightness = GetParam(pfConfig);
    sdwContrast = GetParam(pfConfig);
    dwOutputSize = GetParam(pfConfig);
    dwOutputFreq = GetParam(pfConfig);

    tInitOptions.dwVersion = VIDEODISP_VERSION;
    tInitOptions.pObjectMem = NULL;
    tInitOptions.eOutFormat = (EVideoSignalFormat)dwOutFormat;
    tInitOptions.epixelInFormat = (EPixelFormatFlags)dwInFormat;
    tInitOptions.eOutputSize = (EVideoSignalSize)dwOutputSize;
    tInitOptions.eOutputRefreshRate = (EVideoSignalFrequency)dwOutputFreq;
    tInitOptions.dwMaxInWidth = dwWidth;
    tInitOptions.dwMaxInHeight = dwHeight;
    tInitOptions.dwInWidth = dwWidth;
    tInitOptions.dwInHeight = dwHeight;
    tInitOptions.bPALEn = bPALEn;
    tInitOptions.sdwBrightness = sdwBrightness;
    tInitOptions.sdwContrast = sdwContrast;
    tInitOptions.dwSaturation = dwSaturation;
    tInitOptions.pbyInBuffer = NULL;

    if (VideoDisp_Initial(&hObject, &tInitOptions) != S_OK)
    {
        printf("VideoDisp initial failed!\n");
        exit(1);
    }

    if (dwInFormat == (DWORD)YUV420)
    {
        dwHorzSub = 1;
        dwVertSub = 1;
    }
    else if (dwInFormat == (DWORD)YUV422)
    {
        dwHorzSub = 1;
        dwVertSub = 0;
    }
    else
    {
        printf("Unsupported input format!!");
        exit(1);
    }

    dwYInFrameSize =  dwWidth * dwHeight;
    dwCbInFrameSize = dwCrInFrameSize = (dwWidth * dwHeight)>>(dwHorzSub + dwVertSub);

    /* Request Video Buffer */
    tMemMgrState.dwBusNum = VIDEODISP_IN_BUS_NUM;
    tMemMgrState.dwSize = (dwYInFrameSize + dwCbInFrameSize + dwCrInFrameSize) * 6;
    tMemMgrState.eAlignType = ALIGN_TYPE_8_BYTE;

    if ((pbyBuffer=(BYTE*)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate input frame buffer fail !!\n");
        exit(1);
    }
    else
    {
        printf("Request %ld bytes input frame buffer succeed !!\n", tMemMgrState.dwSize);
    }
    memset(pbyBuffer, 128, tMemMgrState.dwSize);
    printf("Request form V(%p) at P(%p)\n", pbyBuffer, (void *)MemMgr_GetPhysAddr(hMemObject, (DWORD)pbyBuffer));

    /* Add two buffer for initial status, in this case, the 4th and 5th buffer */
    tState.dwIndex = 4;
    tState.pbyYFrame = pbyBuffer + (dwYInFrameSize + dwCbInFrameSize + dwCrInFrameSize) * 4;
    tState.pbyCbFrame = tState.pbyYFrame + dwYInFrameSize;
    tState.pbyCrFrame = tState.pbyCbFrame + dwCbInFrameSize;

    if (VideoDisp_QueueBuf(hObject, &tState) != S_OK)
    {
        printf("Queue buffer failed !\n");
        exit(1);
    }
    memset(tState.pbyYFrame, 65, dwYInFrameSize);
    memset(tState.pbyCbFrame, 100, dwCbInFrameSize);
    memset(tState.pbyCrFrame, 212, dwCrInFrameSize);

    tState.dwIndex = 5;
    tState.pbyYFrame = pbyBuffer + (dwYInFrameSize + dwCbInFrameSize + dwCrInFrameSize) * 5;
    tState.pbyCbFrame = tState.pbyYFrame + dwYInFrameSize;
    tState.pbyCrFrame = tState.pbyCbFrame + dwCbInFrameSize;
    if (VideoDisp_QueueBuf(hObject, &tState) != S_OK)
    {
        printf("Queue buffer failed !\n");
        exit(1);
    }
    memset(tState.pbyYFrame, 35, dwYInFrameSize);
    memset(tState.pbyCbFrame, 212, dwCbInFrameSize);
    memset(tState.pbyCrFrame, 114, dwCrInFrameSize);

    VideoDisp_Start(hObject);

    /*
     * VideoDisp_QueueBuf / VideoDisp_DeQueueBuf is FIFO,
     * VideoDisp_DeQueueBuf will return deque index in tState.dwIndex for App
     * If the same dwIndex value is VideoDisp_QueueBuf but not been dequeue yet, the driver will alarm warning.
     * In this sample application, there is two pre queue buffer before start output and then loop queue/dequeue in four buffer
     * (q 0, dq 4) -> (q 1, dq 5) -> (q 2, dq 0) -> (q 3, dq 1) -> (q 0, dq 2) -> (q 1, dq 3)
     *                                     ^|__________________________________________|
     */
#ifdef REPEAT
    while(1)
#endif
    {
        rewind(pfInput);

        for (i=0; i <dwFrameNum ; i++)
        {
            printf("Frame %d\n", (int)i);

            tState.dwIndex = (count++%4);
            tState.pbyYFrame = pbyBuffer + (dwYInFrameSize + dwCbInFrameSize + dwCrInFrameSize) * (i%4);
            tState.pbyCbFrame = tState.pbyYFrame + dwYInFrameSize;
            tState.pbyCrFrame = tState.pbyCbFrame + dwCbInFrameSize;
#ifdef READONCE_TEST
            if (k<4)
            {
#endif
                for (j=0; j<dwHeight; j++)
                {
                    fread((void *)(tState.pbyYFrame+j*dwStride), dwWidth, 1, pfInput);
                }
                for (j=0; j<dwHeight>>dwVertSub; j++)
                {
                    fread((void *)(tState.pbyCbFrame+j*(dwStride>>dwHorzSub)), dwWidth>>dwHorzSub, 1, pfInput);
                }
                for (j=0; j<dwHeight>>dwVertSub; j++)
                {
                    fread((void *)(tState.pbyCrFrame+j*(dwStride>>dwHorzSub)), dwWidth>>dwHorzSub, 1, pfInput);
                }
#ifdef READONCE_TEST
                k++;
            }
#endif

            if (VideoDisp_QueueBuf(hObject, &tState) != S_OK)
            {
                printf("Queue buffer failed !\n");
                break;
            }

            if (VideoDisp_DeQueueBuf(hObject, &tState) != S_OK)
            {
                printf("Release buffer failed !\n");
                break;
            }
        }
    }

    VideoDisp_Stop(hObject);

    if (VideoDisp_Release(&hObject) != S_OK)
    {
        printf("VideoDisp release failed!\n");
        exit(1);
    }
    fclose(pfInput);
    fclose(pfConfig);

    if (MemMgr_Release(&hMemObject) != S_OK)
    {
        printf("Release memory manager object fail !!\n");
        exit(1);
    }

    return 0;
}
#endif /* Define QUEUE_METHOD */
/* =========================================================================================== */
