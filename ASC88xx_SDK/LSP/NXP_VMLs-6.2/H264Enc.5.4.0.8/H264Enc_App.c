/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App/H264Enc_App.c 40    14/10/21 7:07p Jaja $
 *
 * Copyright 2008 ______ Inc. All rights reserved.
 *
 * Description:
 *
 * $History: H264Enc_App.c $
 * 
 * *****************  Version 40  *****************
 * User: Jaja         Date: 14/10/21   Time: 7:07p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 * 
 * *****************  Version 39  *****************
 * User: Jaja         Date: 14/04/30   Time: 2:36p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 * 
 * *****************  Version 38  *****************
 * User: Jaja         Date: 14/04/30   Time: 1:42p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 37  *****************
 * User: Jaja         Date: 13/12/12   Time: 1:16p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 35  *****************
 * User: Jaja         Date: 13/09/14   Time: 10:53a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 34  *****************
 * User: Jaja         Date: 13/09/13   Time: 11:54a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 33  *****************
 * User: Jaja         Date: 13/08/23   Time: 9:37a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 32  *****************
 * User: Jaja         Date: 13/08/02   Time: 9:39a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 31  *****************
 * User: Jaja         Date: 13/03/29   Time: 12:23a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 30  *****************
 * User: Jaja         Date: 13/03/28   Time: 11:25a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 29  *****************
 * User: Jaja         Date: 12/12/26   Time: 9:14a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 28  *****************
 * User: Jaja         Date: 12/09/24   Time: 10:44a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 27  *****************
 * User: Jaja         Date: 12/08/08   Time: 10:51a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 26  *****************
 * User: Jaja         Date: 12/06/27   Time: 1:39p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 25  *****************
 * User: Jaja         Date: 12/04/17   Time: 11:10p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 16  *****************
 * User: Jaja         Date: 12/04/17   Time: 11:08p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 24  *****************
 * User: Jaja         Date: 12/01/11   Time: 10:20p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 23  *****************
 * User: Jaja         Date: 11/09/20   Time: 2:52p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 22  *****************
 * User: Jaja         Date: 11/08/24   Time: 4:06p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 21  *****************
 * User: Jaja         Date: 11/07/19   Time: 5:58p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 20  *****************
 * User: Jaja         Date: 11/07/12   Time: 10:57p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 19  *****************
 * User: Jaja         Date: 11/07/06   Time: 9:04a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 18  *****************
 * User: Jaja         Date: 11/06/19   Time: 10:21a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 17  *****************
 * User: Jaja         Date: 11/06/17   Time: 12:05a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 16  *****************
 * User: Jaja         Date: 11/03/02   Time: 6:11p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 15  *****************
 * User: Jaja         Date: 11/03/02   Time: 5:17p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 14  *****************
 * User: Jaja         Date: 10/08/11   Time: 9:29p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 13  *****************
 * User: Jaja         Date: 10/08/11   Time: 11:10a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 12  *****************
 * User: Jaja         Date: 10/08/10   Time: 5:46p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 10  *****************
 * User: Jaja         Date: 10/02/01   Time: 6:41p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 9  *****************
 * User: Jaja         Date: 10/01/05   Time: 9:59p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 8  *****************
 * User: Jaja         Date: 09/11/30   Time: 7:14p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 7  *****************
 * User: Jaja         Date: 09/11/25   Time: 2:23p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 6  *****************
 * User: Jaja         Date: 09/10/01   Time: 1:05p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 5  *****************
 * User: Jaja         Date: 09/09/24   Time: 6:03p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 4  *****************
 * User: Jaja         Date: 09/09/16   Time: 4:41p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 3  *****************
 * User: Jaja         Date: 09/06/04   Time: 6:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 2  *****************
 * User: Jaja         Date: 09/03/31   Time: 9:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 1  *****************
 * User: Jaja         Date: 09/03/30   Time: 10:57p
 * Created in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc_App
 *
 * *****************  Version 1  *****************
 * User: Jaja         Date: 09/03/30   Time: 10:14p
 * Created in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc/H264Enc_App
 *
 * *****************  Version 1  *****************
 * User: Jaja         Date: 09/03/30   Time: 10:05p
 * Created in $/rd_2/project/SoC/Linux_Libraries/H264Enc/H264Enc.root/H264Enc/H264Enc_App
 *
 *
 */

/* =========================================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


/* =========================================================================================== */
#define PB_BUFFER_SIZE		1024*1024*4
#define PACKET_SIZE			1460
#define MAX_WIDTH			2560
#define MAX_HEIGHT			1920
#define USER_DATA_LENGTH	1
#define CALIBRATE_TIME		1
//#define __SHA1__
//#define __USE_DCE_SHA1__
//#define __USE_SW_SHA1__
//
/* =========================================================================================== */
#define H264ENC_STREAM_BUS_NUM	0

#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
#define H264ENC_CURR_BUS_NUM	1
#define H264ENC_FRMAE_BUS_NUM	2
#define H264ENC_OBJECT_BUS_NUM	3
#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
#define H264ENC_CURR_BUS_NUM	1
#define H264ENC_FRMAE_BUS_NUM	1
#define H264ENC_OBJECT_BUS_NUM	1
#endif //__ASM_ARCH_PLATFORM_MOZART_H__

#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
#define H264ENC_CURR_BUS_NUM	0
#define H264ENC_FRMAE_BUS_NUM	0
#define H264ENC_OBJECT_BUS_NUM	0
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__

//#define H264ENC_LOOP

#ifdef __SHA1__
//#include "DataCrypto.h"
#define RotateLeft(dwWord, dwBits) ((dwWord<<dwBits) | (dwWord>>(32-dwBits)))

#define MAX_NAME_LEN 300
#define MAX_STAT_LEN 20
#define MAX_FILE_SIZE 1024*1024*4

#ifndef MAX_NAME_LEN
	#define MAX_NAME_LEN 300
#endif //!MAX_NAME_LEN
#endif //__SHA1__

/* =========================================================================================== */
#include "H264Enc.h"
#include "MemMgr.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* =========================================================================================== */
//extern FLOAT  fBW0, fBW1;
//extern FLOAT  fOp;

//#define __BASE_MV_OUTPUT__
/* =========================================================================================== */

#ifdef __SHA1__
void ProcChunk(BYTE *pbyIn, DWORD *pdwH0, DWORD *pdwH1, DWORD *pdwH2, DWORD *pdwH3, DWORD *pdwH4)
{
    BYTE *pbyEnidan;

    DWORD i;
    DWORD dwTemp;
    DWORD dwA, dwB, dwC, dwD, dwE, dwF, dwK;
	DWORD adwW[80] = {0};
    for (i=0, pbyEnidan=pbyIn; i<16; i++, pbyEnidan+=4)
    {
        adwW[i] = (pbyEnidan[0]<<24) | (pbyEnidan[1]<<16) | (pbyEnidan[2]<<8) | pbyEnidan[3];
    }

    for (i=16; i<80; i++)
    {
        adwW[i] = RotateLeft((adwW[i-3]^adwW[i-8]^adwW[i-14]^adwW[i-16]), 1);
    }

    dwA = *pdwH0;
    dwB = *pdwH1;
    dwC = *pdwH2;
    dwD = *pdwH3;
    dwE = *pdwH4;

    dwK = 0x5A827999;
    for (i=0; i<20; i++)
    {
        dwF = (dwB&dwC) | ((~dwB)&dwD);

        dwTemp = RotateLeft(dwA, 5) + dwF + dwE + dwK + adwW[i];
        dwE = dwD;
        dwD = dwC;
        dwC = RotateLeft(dwB, 30);
        dwB = dwA;
        dwA = dwTemp;
    }

    dwK = 0x6ED9EBA1;
    for (i=20; i<40; i++)
    {
        dwF = dwB ^ dwC ^ dwD;

        dwTemp = RotateLeft(dwA, 5) + dwF + dwE + dwK + adwW[i];
        dwE = dwD;
        dwD = dwC;
        dwC = RotateLeft(dwB, 30);
        dwB = dwA;
        dwA = dwTemp;
    }

    dwK = 0x8F1BBCDC;
    for (i=40; i<60; i++)
    {
        dwF = (dwB&dwC) | (dwB&dwD) | (dwC&dwD);

        dwTemp = RotateLeft(dwA, 5) + dwF + dwE + dwK + adwW[i];
        dwE = dwD;
        dwD = dwC;
        dwC = RotateLeft(dwB, 30);
        dwB = dwA;
        dwA = dwTemp;
    }

    dwK = 0xCA62C1D6;
    for (i=60; i<80; i++)
    {
        dwF = dwB ^ dwC ^ dwD;

        dwTemp = RotateLeft(dwA, 5) + dwF + dwE + dwK + adwW[i];
        dwE = dwD;
        dwD = dwC;
        dwC = RotateLeft(dwB, 30);
        dwB = dwA;
        dwA = dwTemp;
    }

    *pdwH0 += dwA;
    *pdwH1 += dwB;
    *pdwH2 += dwC;
    *pdwH3 += dwD;
    *pdwH4 += dwE;
}

/* =========================================================================================== */
void Sha1(BYTE *pbyChunk, DWORD dwWidth, DWORD dwHeight, DWORD *pdwH0, DWORD *pdwH1, DWORD *pdwH2, DWORD *pdwH3, DWORD *pdwH4)
{
    BYTE *pbyShaIn;
    BOOL bSha1Extra;
    DWORD i;

    DWORD dwVaildBytes;
    DWORD dwVaildBits;
    DWORD dwSha1Size, dwSha1Mod, dwSha1Num;
    DWORD dwBase;
    DWORD dwH0Init = 0x67452301;
    DWORD dwH1Init = 0xEFCDAB89;
    DWORD dwH2Init = 0x98BADCFE;
    DWORD dwH3Init = 0x10325476;
    DWORD dwH4Init = 0xC3D2E1F0;

    dwVaildBytes = (dwWidth*dwHeight*3) / 2;
    dwVaildBits = dwVaildBytes << 3;
    dwSha1Size = dwVaildBits + 1;
    dwSha1Mod = dwSha1Size & 0x1ff;
    bSha1Extra = (dwSha1Mod<=448) ? FALSE : TRUE;
    dwSha1Num = (dwSha1Size>>9) + ((bSha1Extra==FALSE)?1:2);

    /* Pre-Porcessing */
    //Padding 1'b1
    pbyChunk[dwVaildBytes] = 0x80;

    //Apeend 64bits message
    dwBase = ((dwSha1Num*512)>>3) - 8;
	//Apeend 64bits message
	memset(pbyChunk+dwVaildBytes+1, 0x0, dwBase-dwVaildBytes-1);

    //pbyChunk[dwBase+0] = 0;
    //pbyChunk[dwBase+1] = 0;
    //pbyChunk[dwBase+2] = 0;
    //pbyChunk[dwBase+3] = 0;
    pbyChunk[dwBase+4] = (dwVaildBits>>24) & 0xff;
    pbyChunk[dwBase+5] = (dwVaildBits>>16) & 0xff;
    pbyChunk[dwBase+6] = (dwVaildBits>>8) & 0xff;
    pbyChunk[dwBase+7] = dwVaildBits & 0xff;

    /* Process Chunk */
    *pdwH0 = dwH0Init;
    *pdwH1 = dwH1Init;
    *pdwH2 = dwH2Init;
    *pdwH3 = dwH3Init;
    *pdwH4 = dwH4Init;

    for (i=0, pbyShaIn=pbyChunk; i<dwSha1Num; i++, pbyShaIn+=(512>>3))
    {
        ProcChunk(pbyShaIn, pdwH0, pdwH1, pdwH2, pdwH3, pdwH4);
    }

    //Digest
    //printf("Digest: %08X %08X %08X %08X %08X\n", *pdwH0, pdwH1, pdwH2, pdwH3, pdwH4);
}
#endif //__SHA1__

/* =========================================================================================== */
#ifdef __SHA1__		
void GetRecSHA1(TH264EncInitOptions tEncInitOptions, 
                //TDataCryptoState tDataCryptoState, 
                //HANDLE hDataCryptoObject, 
                BYTE *pbyDigest, 
                BYTE *pbySHA1, 
                FILE *pfOutput)
{
	BYTE *pbyChunk = NULL;
	//DWORD dwGoldenH0, dwGoldenH1, dwGoldenH2, dwGoldenH3, dwGoldenH4;
#ifdef __USE_DCE_SHA1__
	DWORD dwDceH0, dwDceH1, dwDceH2, dwDceH3, dwDceH4;
	DWORD aDecH[5];
#endif //__USE_DCE_SHA1__
#ifdef __USE_SW_SHA1__
	DWORD dwH0, dwH1, dwH2, dwH3, dwH4;
#endif //__USE_SW_SHA1__
  
#ifdef __USE_SW_SHA1__
			Sha1(pbySHA1, tEncInitOptions.dwEncWidth, tEncInitOptions.dwEncHeight, &dwH0, &dwH1, &dwH2, &dwH3, &dwH4);
#endif //__USE_SW_SHA1__
			//fscanf(pfSHA1, "%08lX %08lX %08lX %08lX %08lX\n", &dwGoldenH0, &dwGoldenH1, &dwGoldenH2, &dwGoldenH3, &dwGoldenH4);

/* Version 4.0.0.0 modification, 2012.02.09 */

			//printf("Golden \tSHA %08lX %08lX %08lX %08lX %08lX\n", dwGoldenH0, dwGoldenH1, dwGoldenH2, dwGoldenH3,dwGoldenH4);
#ifdef __USE_SW_SHA1__
			printf("sw \tSHA %08lX %08lX %08lX %08lX %08lX\n", dwH0, dwH1, dwH2, dwH3,dwH4);
#endif //__USE_SW_SHA1__
  
  
#ifdef __USE_DCE_SHA1__
            tDataCryptoState.dwHashSize = 0;
			tDataCryptoState.eHashStat = CRYPTO_HASH_STAT_BEGIN;
			tDataCryptoState.pbyInput = pbySHA1;
			tDataCryptoState.dwTextSize = tEncInitOptions.dwEncWidth*tEncInitOptions.dwEncHeight*3/2;
			if (DataCrypto_ProcessOneFrame(hDataCryptoObject, &tDataCryptoState) != S_OK)
			{
				printf("DEC Error occurs!!\n");
			}

			tDataCryptoState.eHashStat = CRYPTO_HASH_STAT_END;
			tDataCryptoState.pbyInput = pbySHA1;
			tDataCryptoState.dwTextSize = 0;
			if (DataCrypto_ProcessOneFrame(hDataCryptoObject, &tDataCryptoState) != S_OK)
			{
				printf("DEC Error occurs!!\n");
			}

			aDecH[0] = dwDceH0 = (pbyDigest[0]<<24)|(pbyDigest[1]<<16)|(pbyDigest[2]<<8)|(pbyDigest[3]);
			aDecH[1] = dwDceH1 = (pbyDigest[4]<<24)|(pbyDigest[5]<<16)|(pbyDigest[6]<<8)|(pbyDigest[7]);
			aDecH[2] = dwDceH2 = (pbyDigest[8]<<24)|(pbyDigest[9]<<16)|(pbyDigest[10]<<8)|(pbyDigest[11]);
			aDecH[3] = dwDceH3 = (pbyDigest[12]<<24)|(pbyDigest[13]<<16)|(pbyDigest[14]<<8)|(pbyDigest[15]);
			aDecH[4] = dwDceH4 = (pbyDigest[16]<<24)|(pbyDigest[17]<<16)|(pbyDigest[18]<<8)|(pbyDigest[19]);
                
			printf("hw \tSHA %08lX %08lX %08lX %08lX %08lX\n",dwDceH0, dwDceH1, dwDceH2, dwDceH3, dwDceH4);
			
			//fwrite(pbySHA1, sizeof(BYTE), (tEncInitOptions.dwEncWidth*tEncInitOptions.dwEncHeight*3/2), pfOutput);
			fprintf(pfOutput,"%08X %08X %08X %08X %08X\n", aDecH[0], aDecH[1], aDecH[2], aDecH[3], aDecH[4]);
#endif //__USE_DCE_SHA1__
}
#endif //__SHA1__

/* =========================================================================================== */
int main (int argc, char **argv)
{
	TH264EncInitOptions tEncInitOptions;
	TH264EncOptions tEncOptions;
	TH264EncState tEncState;
	TH264EncROIWindowsInfos tEncROIWindowsOptions;
	TMemMgrInitOptions tMemMgrInitOptions;
	TMemMgrState tMemMgrState;

	HANDLE hMemObject;
	HANDLE hEncObject;

	DWORD dwFrameCount, dwFrameSize, dwFrameBytes;
	DWORD dwFrameNum;
	DWORD *pdwSrc;
	DWORD dwFrameSkip, dwSkipCount;
	DWORD dwPadWidth, dwPadHeight;
	DWORD dwInWidth, dwInHeight, dwEncWidth, dwEncHeight, dwQP, dwRefFrameNum;
    DWORD dwMaxFrameRate;
	DWORD dwTotalBytes;
	DWORD dwProfile, dwBitRate;
	DWORD dwIntraSliceInterval, dwIntraRefreshMBNum;
	DWORD dwSearchRangeX, dwSearchRangeY;
	DWORD dwConstrainedIntraPredEn, dwAdvancedSkipModeEn;
	DWORD dwIntra16x16PredEn, dwInterChmaSADEn, dwFrameperSecond;
	DWORD dwInOverlayWidth, dwInOverlayHeight, dwInOverlayFrameSize;
	BOOL bOsdEn;
	DWORD dwCmprInSize, dwCmprPadSize;
	DWORD dwOsdWidth, dwOsdHeight, dwOsdPadWidth, dwOsdCmprInSize, dwOsdCmprPadSize;
	DWORD dwOsdSrtHeight, dwOsdEndHeight;
	DWORD i, j;
	DWORD dwMediaMemSize;
	DWORD pbyOsdYFrame;
	DWORD pbyOsdCbFrame;
	DWORD pbyOsdCrFrame;
	DWORD dwCurCmprRatio, dwOsdCmprRatio;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

#ifdef __SHA1__
	FILE *pfSHA1 = NULL;
	FILE *pfBug = NULL;
	BYTE *pbySHA1 = NULL;
	CHAR *pcSHA1 = NULL;
    CHAR acSHA1[MAX_NAME_LEN] = "/share/test_bench/video/Bitstreams/H264/Golden/SHA1/";	
/* Version 4.0.0.0 modification, 2012.02.09 */
#ifdef __USE_DCE_SHA1__
	HANDLE hDataCryptoObject = NULL;
	TDataCryptoInitOptions tDataCryptoInitOptions;
	TDataCryptoState tDataCryptoState;
#endif //__USE_DCE_SHA1__
	BYTE *pbyDigest =  NULL;
/* ======================================== */
#endif //__SHA1__

	FILE *pfOutput;
	FILE *pfRecOutput;
	FILE *pfInput;
	FILE *pfOsdInput;
	BYTE *pbyNetbuf;
	BYTE *pbyFrame, *pbySrc;
	BYTE *pbyEncUserData;
	FILE *pfObjectInput;
	BYTE *pbyObjectInfo;
#ifdef __BASE_MV_OUTPUT__
	BYTE *pbyMBInfos;
	TH264EncMBInfos *ptMBInfos;
#endif //__BASE_MV_OUTPUT__

//#ifdef __USE_PROFILE__
//	if (argc != 27)
//#else //!__USE_PROFILE__
	if (argc != 37)
//#endif //!__USE_PROFILE__
	{
		printf("Usage:	h264enc\n");
		printf("	input_yuv_file\n");
		printf("	output_yuv_file\n");
		printf("	output_bitstream_file\n");
		printf("	encode_frame_num\n");
		printf("	input_frame_width\n");
		printf("	input_frame_height\n");
		printf("	encoded_frame_width\n");
		printf("	encoded_frame_height\n");
		printf("	frame_skip\n");
		printf("	reference_frame_number 1 or 2(default)\n");
		printf("	init_qp(0-51)\n");
		printf("	profile(0: baseline profile, 1: main profile, 2: high profile(default))\n");
		printf("	P_interval\n");
		printf("	search_range_x(1:16, 2:32, 4:64(default))\n");
		printf("	intra_16x16_Enable(0:Disable, 1:Enable(default))\n");
		printf("	refresh_MB_number (0:No Refresh MBs)\n");
		printf("	FME_skip_option_enable(0: Disable, 1:Enable(default))\n");
		printf("	constrainedIntraPred (0: Disable(default), 1:Enable)\n");
		printf("	chma_pel_satd (0: Disable(default), 1:Enable)\n");
		printf("	search_range_y(1:16, 2:32, 4:64(default))\n");
		printf("	bit_rate\n");
		printf("	frame_per_second(60000:30fps(default), 50000: 25fps\n");
		printf("	object_qp_delta(-26~+25, for Mozart V2/V3)\n");	// 23
		printf("    object_input(for Mozart V2/V3)\n");				// 24
		printf("	input_osd_file\n");								// 25
		printf("	osd_enable\n");									// 26
		printf("	osd_width \n");									// 27
		printf("	osd_height \n");								// 28
		printf("	osd_start_height \n");							// 29
		printf("	osd_end_height \n");							// 30
		printf("	memory compression source config sel(1:6/8   2:5/8   3:4/8   0: 8/8)\n");
		printf("	memory compression overlay source config sel(1:6/8   2:5/8   3:4/8   0: 8/8)\n");
		// 1:6/8(2.b3)   2:5/8(11.b4)   3:4/8(10.b4)   0: 8/8
		printf("	intra 4x4 (256~319)\n");							// 32
		printf("	intra 8x8 (256~319)\n");							// 33
		printf("	intra 16x16 (256~287)\n");							// 34
		printf("	FME (256~319)\n");							// 35

//#ifdef __USE_PROFILE__
//		printf("	frame rate\n");
//#endif //__USE_PROFILE__

		exit(1);
	}

	if ((pfInput=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open input YUV file %s fail !!\n", argv[1]);
		exit(1);
	}

#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
	if ((pfOsdInput=fopen(argv[25], "rb")) == NULL)
	{
		printf("Open input OSD YUV file %s fail !!\n", argv[25]);
		exit(1);
	}
	else
	{
		printf("Open input OSD YUV file %s success !!\n", argv[25]);
	}
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__
#ifndef H264ENC_LOOP
	if ((pfRecOutput=fopen(argv[2], "wb")) == NULL)
	{
		printf("Open output YUV file %s fail !!\n", argv[2]);
		goto RECFILE_FAIL;
	}

	if ((pfOutput=fopen(argv[3], "wb")) == NULL)
	{
		printf("Open output bitstream file %s fail !!\n", argv[3]);
		goto OUTFILE_FAIL;
	}
	//if ((pfObjectInput=fopen(argv[24], "rb")) == NULL)
	//{
	//	printf("Open object file %s fail !!\n", argv[24]);
	//	exit(1);
	//}
#endif //H264ENC_LOOP

	dwFrameNum = atoi(argv[4]);
	dwInWidth = atoi(argv[5]);
	dwInHeight = atoi(argv[6]);
	dwEncWidth = atoi(argv[7]);
	dwEncHeight = atoi(argv[8]);
	dwFrameSkip = atoi(argv[9]);
	dwRefFrameNum = atoi(argv[10]);
	dwQP = atoi(argv[11]);
	dwProfile = atoi(argv[12]);
	dwIntraSliceInterval = atoi(argv[13]);
	dwSearchRangeX = atoi(argv[14]);
	dwIntra16x16PredEn = atoi(argv[15]);
	dwIntraRefreshMBNum = atoi(argv[16]);
	dwAdvancedSkipModeEn = atoi(argv[17]);
	dwConstrainedIntraPredEn = atoi(argv[18]);
	dwInterChmaSADEn = atoi(argv[19]);
	dwSearchRangeY = atoi(argv[20]);
	dwBitRate = atoi(argv[21]);
	dwFrameperSecond = atoi(argv[22]);
	dwPadWidth = (((dwInWidth + 15)>>4)<<4);
	dwPadHeight = (((dwInHeight + 15)>>4)<<4);
	dwFrameSize = dwPadWidth*dwPadHeight;
	//dwFrameSize = dwPadWidth*dwPadHeight * 3/2;
	bOsdEn = atoi(argv[26]);
	dwOsdWidth = atoi(argv[27]);
	dwOsdPadWidth = (((dwOsdWidth + 15)>>4)<<4);
	dwOsdHeight = atoi(argv[28]);
	dwOsdSrtHeight = atoi(argv[29]);
	dwOsdEndHeight = atoi(argv[30]);

#ifdef __ROBUST_CONFIG_TEST__
	tEncInitOptions.dwIntra4x4Cfg = atoi(argv[33]);
	tEncInitOptions.dwIntra8x8Cfg = atoi(argv[34]);
	tEncInitOptions.dwIntra16x16Cfg = atoi(argv[35]);
	tEncInitOptions.dwIntraFMECfg = atoi(argv[36]);
#endif //__ROBUST_CONFIG_TEST__


	if ((pbyEncUserData=(BYTE *)calloc(USER_DATA_LENGTH, sizeof(BYTE))) == NULL)
	{
		printf("Allocate user data buffer fail !!\n");
		goto USRDATA_FAIL;
	}

#ifdef __USE_PROFILE__
    tTimerInitOptions.dwVersion = TIMER_VERSION;
	tTimerInitOptions.eTimerType = TIMER_TYPE_PROFILE;
    tTimerInitOptions.pObjectMem = NULL;

	if (Timer_Initial(&hTimerObject, &tTimerInitOptions) != S_OK)
	{
		printf("Initial timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

	tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
	tMemMgrInitOptions.pObjectMem = (void *)NULL;
    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
	{
		printf("Initial memory manager object fail !!\n");
		goto MEMMGR_INITIAL_FAIL;
	}

    tMemMgrState.dwBusNum = H264ENC_OBJECT_BUS_NUM;
	tMemMgrState.dwSize = ((MAX_WIDTH+31)>>4)*((MAX_HEIGHT+15)>>4)*4;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyObjectInfo = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate object buffer %d fail !!\n", tMemMgrState.dwSize);
		goto OBJECTBUF_FAIL;
	}
	else
	{
        printf("Allocate object buffer %d succeed !!\n", tMemMgrState.dwSize);
	}

	tMemMgrState.dwBusNum = H264ENC_CURR_BUS_NUM;
	tMemMgrState.dwSize = ((((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)*3/2);
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyFrame = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate input frame buffer %d fail !!\n", tMemMgrState.dwSize);
		goto FRAMEBUF_FAIL;
	}
	else
	{
        printf("Allocate input frame buffer %d succeed !!\n", tMemMgrState.dwSize);
	}
	memset(pbyFrame, 0, tMemMgrState.dwSize);

	tMemMgrState.dwBusNum = H264ENC_STREAM_BUS_NUM;
	tMemMgrState.dwSize = PB_BUFFER_SIZE;
	tMemMgrState.eAlignType = ALIGN_TYPE_32_BYTE;
	if ((pbyNetbuf=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate bit stream buffer fail !! Size = %d\n", tMemMgrState.dwSize);
		goto NETBUF_FAIL;
	}
	else
	{
		printf("Allocate bit stream buffer succeed !! Size = %d\n", tMemMgrState.dwSize);
	}

#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
	tMemMgrState.dwBusNum = H264ENC_CURR_BUS_NUM;
	tMemMgrState.dwSize = (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyOsdYFrame = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate input osd frame buffer %d fail !!\n", tMemMgrState.dwSize);
		goto FRAMEBUF_FAIL;
	}
	else
	{
        printf("Allocate input osd frame buffer %d succeed !!\n", tMemMgrState.dwSize);
	}
	tMemMgrState.dwBusNum = H264ENC_CURR_BUS_NUM;
	tMemMgrState.dwSize = (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)/4;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyOsdCbFrame = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate input osd frame buffer %d fail !!\n", tMemMgrState.dwSize);
		goto FRAMEBUF_FAIL;
	}
	else
	{
        printf("Allocate input osd frame buffer %d succeed !!\n", tMemMgrState.dwSize);
	}
	tMemMgrState.dwBusNum = H264ENC_CURR_BUS_NUM;
	tMemMgrState.dwSize = (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)/4;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyOsdCrFrame = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
        printf("Allocate input osd frame buffer %d fail !!\n", tMemMgrState.dwSize);
		goto FRAMEBUF_FAIL;
	}
	else
	{
        printf("Allocate input osd frame buffer %d succeed !!\n", tMemMgrState.dwSize);
	}
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__

#ifdef __BASE_MV_OUTPUT__
	// MOZART_V3_MBINFO
	tMemMgrState.dwBusNum = 1;
	tMemMgrState.dwSize = sizeof(TH264EncMBInfos)*((dwEncWidth+15)>>4)*((dwEncHeight+15)>>4);
	tMemMgrState.eAlignType = ALIGN_TYPE_16_BYTE;
	if ((pbyMBInfos=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	{
		printf("Allocate bit stream buffer fail !! Size = %d\n", tMemMgrState.dwSize);
		goto NETBUF_FAIL;
	}
	else
	{
		printf("Allocate bit stream buffer succeed !! Size = %d\n", tMemMgrState.dwSize);
	}
	// MOZART_V3_MBINFO Complete
#endif //__BASE_MV_OUTPUT__


#ifdef __SHA1__
	tMemMgrState.dwBusNum = H264ENC_CURR_BUS_NUM;
    tMemMgrState.dwSize = (MAX_WIDTH*MAX_HEIGHT*3/2) + 8 + 64;
    tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
    if ((pbySHA1 = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
        printf("Allocate SHA 1 buffer fail !!\n");
		goto ENCODE_END;
    }
	else
	{
		printf("Allocate SHA 1 buffer succeed !! Size = %ld\n", tMemMgrState.dwSize);
	}

	memset(pbySHA1, 0x00, (MAX_WIDTH*MAX_HEIGHT*3/2));
	memset((pbySHA1+(MAX_WIDTH*MAX_HEIGHT*3/2)), 0x00, 8+64);

#ifdef __USE_DCE_SHA1__
	tDataCryptoInitOptions.dwVersion = DATACRYPTO_VERSION;
	tDataCryptoInitOptions.pObjectMem = NULL;
	if (DataCrypto_Initial(&hDataCryptoObject, &tDataCryptoInitOptions) != S_OK)
	{
		printf("Initialize DataCrypto object fail !!\n");
		goto ENCODE_END;
	}

	tMemMgrState.dwBusNum = 0;
    tMemMgrState.dwSize = 20;
    tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
	if ((pbyDigest = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
		printf("Allocate SHA1 digest buffer fail !!\n" );
		goto ENCODE_END;
	}
	else
	{
		printf("Allocate SHA1 digest buffer succeed !! Size = %ld\n", tMemMgrState.dwSize);
	}
	memset(pbyDigest, 0x0, 20*sizeof(BYTE));
	
	tDataCryptoState.eOpMode = CRYPTO_OP_HASH;
	tDataCryptoState.eCryptoType = CRYPTO_ENCRYPT_TYPE_AES;
	tDataCryptoState.eCryptoMode = CRYPTO_ENCRYPT_MODE_EBC;
	tDataCryptoState.eHashType = CRYPTO_HASH_TYPE_SHA_1;
	tDataCryptoState.eHashMode = CRYPTO_HASH_MODE_HASHING_ONLY;
	tDataCryptoState.eHashStat = CRYPTO_HASH_STAT_BEGIN;
	tDataCryptoState.dwKeySize = 0;
	tDataCryptoState.pbyKey = NULL;
	tDataCryptoState.pbyInitVector = NULL;
	tDataCryptoState.pbyOutput = pbyDigest;
#endif //__USE_DCE_SHA1__
#endif //__SHA1__

	tEncInitOptions.dwVersion = H264ENC_VERSION;
	tEncInitOptions.dwMaxFrameWidth = MAX_WIDTH;
	tEncInitOptions.dwMaxFrameHeight = MAX_HEIGHT;
	tEncInitOptions.dwStreamBusNum = H264ENC_STREAM_BUS_NUM;
	tEncInitOptions.dwFrameBusNum = H264ENC_FRMAE_BUS_NUM;
	tEncInitOptions.dwInHorzOffset = 0;
	tEncInitOptions.dwInVertOffset = 0;
	tEncInitOptions.dwSearchRangeX = dwSearchRangeX;		// 1 => +-16, 2 => +-32, 4 => +-60
	tEncInitOptions.dwSearchRangeY = dwSearchRangeY;		// 1 => +-16, 2 => +-32, 4 => +-60
	tEncInitOptions.dwInWidth = dwInWidth;
	tEncInitOptions.dwInHeight = dwInHeight;
	tEncInitOptions.dwEncWidth = dwEncWidth;
	tEncInitOptions.dwEncHeight = dwEncHeight;
	tEncInitOptions.dwRefFrameNum = dwRefFrameNum;			// 1 = one reference, 2 = two reference
	//printf("stream bus/frame bus = %d, %d !!\n", tEncInitOptions.dwStreamBusNum, tEncInitOptions.dwFrameBusNum);
	tEncInitOptions.dwTicksPerSecond = dwFrameperSecond;	// 60000 = 30fps, 50000 = 25fps
	tEncInitOptions.dwTicksPerFrame = 1000;
	tEncInitOptions.dwQuant = dwQP;
	tEncInitOptions.dwIntraSliceInterval = dwIntraSliceInterval;
	tEncInitOptions.bIntraSliceRefreshEn = TRUE;
	tEncInitOptions.bAdvancedSkipModeEn = (dwAdvancedSkipModeEn==1) ? TRUE: FALSE;
	tEncInitOptions.bConstrainedIntraPredEn = (dwConstrainedIntraPredEn==1) ? TRUE: FALSE;
	tEncInitOptions.bIntra16x16PredEn = (dwIntra16x16PredEn==1) ? TRUE: FALSE;
	tEncInitOptions.bInterChmaSADEn = dwInterChmaSADEn;
	tEncInitOptions.dwProfile = dwProfile;
	tEncInitOptions.dwBitRate = dwBitRate;
	//tEncInitOptions.eRCType = (dwBitRate!=0) ? RATE_CTRL_CQCB : RATE_CTRL_NONE;
	//tEncInitOptions.eRCType = (dwBitRate!=0) ? RATE_CTRL_VQCB : RATE_CTRL_NONE;
	tEncInitOptions.eRCType = (dwBitRate!=0) ? RATE_CTRL_ADPT : RATE_CTRL_NONE;

	tEncInitOptions.dwIntraRefreshMBNum = dwIntraRefreshMBNum;

	tEncInitOptions.bEncByteStreamHdr = TRUE;

#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
	tEncInitOptions.sdwObjectQp = atoi(argv[23]);
	tEncInitOptions.pbyObjectMask = pbyObjectInfo;
	//tEncInitOptions.bFastModeEnable = 0;
	tEncInitOptions.dwSpeedNum = 0;
#else //!__ASM_ARCH_PLATFORM_MOZART_H__
	tEncInitOptions.sdwObjectQp = 0;
	tEncInitOptions.pbyObjectMask = pbyObjectInfo;
	//tEncInitOptions.bFastModeEnable = 0;
	tEncInitOptions.dwSpeedNum = 0;
#endif //__ASM_ARCH_PLATFORM_MOZART_H__

	tEncInitOptions.dwBSBufSize = 0;
	tEncInitOptions.dwTimeOut = 1;
	tEncInitOptions.dwSemaphore = (DWORD)NULL;
	tEncInitOptions.dwUserData = 0;
	tEncInitOptions.eBufTypeFlags = H264NONCIRCULAR;
	tEncInitOptions.pfnSend = NULL;
	tEncInitOptions.pfnEnter = NULL;
	tEncInitOptions.pfnLeave = NULL;

	tEncInitOptions.pbyEncUserData = pbyEncUserData;

//#ifdef __ASM_ARCH_PLATFORM_BEETHOVEN_H__
//	tEncInitOptions.pbyYFrame = pbyFrame;
//	tEncInitOptions.pbyCbFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
//	tEncInitOptions.pbyCrFrame = NULL;
//#else //!__ASM_ARCH_PLATFORM_BEETHOVEN_H__
	tEncInitOptions.pbyYFrame = pbyFrame;
	tEncInitOptions.pbyCbFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4);
	tEncInitOptions.pbyCrFrame = pbyFrame + (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4) + (((((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4))>>2);
//#endif //__ASM_ARCH_PLATFORM_BEETHOVEN_H__

	tEncInitOptions.pObjectMem = NULL;

	dwMediaMemSize = H264Enc_QueryMediaMemSize(&tEncInitOptions);

	tEncInitOptions.dwMediaBuffSize = dwMediaMemSize;
	//tMemMgrState.dwBusNum = H264ENC_FRMAE_BUS_NUM;
	//tMemMgrState.dwSize = dwMediaMemSize;
	//tMemMgrState.eAlignType = ALIGN_TYPE_1_BYTE;
	//if ((tEncInitOptions.pbyMediaBuff=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
	//{
	//	printf("Allocate media buffer for encoder fail !! Size = %d\n", tMemMgrState.dwSize);
	//	goto NETBUF_FAIL;
	//}
	//else
	//{
	//	printf("Allocate media buffer for encoder succeed !! Size = %d\n", tMemMgrState.dwSize);
	//}
	tEncInitOptions.pbyMediaBuff = NULL;

	if (H264Enc_Initial(&hEncObject, &tEncInitOptions) != S_OK)
	{
		printf("Initialize H.264 encoder object fail !!\n");
		goto H264ENC_INITIAL_FAIL;
	}
	else
	{
		printf("Initialize H.264 encoder object success !!\n");
	}

    dwMaxFrameRate = (tEncInitOptions.dwTicksPerSecond+(tEncInitOptions.dwTicksPerFrame>>1)) / tEncInitOptions.dwTicksPerFrame / 2;

	tEncState.dwSeconds = 0;
	tEncState.dwTicks = 0;
	tEncState.dwEncUserDataLength = USER_DATA_LENGTH;


#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__

#ifdef H264ENC_LOOP
	for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
	{
        pbySrc = tEncInitOptions.pbyYFrame;
		for (j = 0; j < dwInHeight; j++, pbySrc+=dwPadWidth)
		{
			if (fread(pbySrc, sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
			{
				printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
		}

		pbySrc = tEncInitOptions.pbyCbFrame;
		for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
		{
			if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
			{
				printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
		}

		pbySrc = tEncInitOptions.pbyCrFrame;
		for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwPadWidth>>1))
		{
			if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
			{
				printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
				goto ENCODE_END;
			}
		}
	}
#endif //H264ENC_LOOP


#ifdef H264ENC_LOOP
	for (dwFrameCount=0, dwTotalBytes=0; ; dwFrameCount++)
#else //!H264ENC_LOOP
	for (dwFrameCount=0, dwTotalBytes=0; dwFrameCount<dwFrameNum; dwFrameCount++)
#endif //H264ENC_LOOP
	{
#ifndef H264ENC_LOOP
		for (dwSkipCount=0; dwSkipCount<=dwFrameSkip; dwSkipCount++)
		{
			// current
			{
			    //if (dwFrameCount == 0)
			    {
    				pbySrc = tEncInitOptions.pbyYFrame;
    				for (j = 0; j < dwInHeight; j++, pbySrc+=dwInWidth)
    				{
    					if (fread(pbySrc, sizeof(BYTE), dwInWidth, pfInput) != dwInWidth)
    					{
    						printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
    						//Cal_PSNR(hEncObject, &tEncState, TRUE);
    						goto ENCODE_END;
    					}
    				}
    				pbySrc = tEncInitOptions.pbyCbFrame;
    				for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwInWidth>>1))
    				{
    					if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
    					{
    						printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
    						goto ENCODE_END;
    					}
    				}
                    
    				pbySrc = tEncInitOptions.pbyCrFrame;
    				for (j = 0; j < (dwInHeight>>1); j++, pbySrc+=(dwInWidth>>1))
    				{
    					if (fread(pbySrc, sizeof(BYTE), (dwInWidth>>1), pfInput) != (dwInWidth>>1))
    					{
    						printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
    						goto ENCODE_END;
    					}
    				}
    			}
			}

            //pbySrc = tEncInitOptions.pbyObjectMask;
            //if (fread(pbySrc, sizeof(BYTE), (dwPadWidth>>4)*(dwPadHeight>>4)*4, pfObjectInput) != ((dwPadWidth>>4)*(dwPadHeight>>4)*4))
            //{
            //    printf("There are only %d frames in the input YUV sequence !!\n", dwFrameCount);
            //    goto ENCODE_END;
            //}
    	}
#endif //H264ENC_LOOP

		tEncState.dwSeconds = tEncState.dwSeconds + tEncState.dwTicks / dwMaxFrameRate;
		tEncState.dwTicks = tEncState.dwTicks % dwMaxFrameRate;
		tEncState.bLast = (dwFrameCount>=(dwFrameNum-1));

		tEncOptions.eOptionFlags = VIDEO_CHANGE_BITSTREAM_BUFFER;
		tEncOptions.adwUserData[0] = (DWORD)pbyNetbuf;
		tEncOptions.adwUserData[1] = PB_BUFFER_SIZE;

		if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		{
			printf("Set options fail !!\n");
			goto ENCODE_END;
		}

		//if (dwFrameCount == 0)
		//{
		//	tEncOptions.eOptionFlags = VIDEO_SET_H264_PAYLOAD_TPYE_1_TIME_INFO;
		//	tEncOptions.adwUserData[0] = TRUE;
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
		//	tEncOptions.eOptionFlags = VIDEO_CHANGE_USERDATA_BUFFER;
		//	tEncOptions.adwUserData[0] = pbyNetbuf;
		//	tEncOptions.adwUserData[1] = 30;
		//	//tEncOptions.adwUserData[2] = 30;
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
		//	//tEncOptions.eOptionFlags = VIDEO_CHANGE_BITRATE;
		//	//tEncOptions.adwUserData[0] = 2048000;
		//	//if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	//{
		//	//	printf("Set options fail !!\n");
		//	//	goto ENCODE_END;
		//	//}
		//	//tEncOptions.eOptionFlags = VIDEO_CHANGE_FRAME_RATE;
		//	//tEncOptions.adwUserData[0] = 10;
		//	//tEncOptions.adwUserData[1] = 30;
		//	//if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	//{
		//	//	printf("Set options fail !!\n");
		//	//	goto ENCODE_END;
		//	//}
		//}


//#ifdef __BASE_MV_OUTPUT__
//		// Get information
//		tEncOptions.eOptionFlags = VIDEO_GET_MB_INFO_BUFF;
//		tEncOptions.adwUserData[0] = (DWORD )pbyMBInfos;
//		tEncOptions.adwUserData[1] = TRUE;
//		tEncOptions.adwUserData[2] = 1;
//		if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
//		{
//			printf("Set options fail !!\n");
//			goto ENCODE_END;
//		}
//#endif //__BASE_MV_OUTPUT__
/* ============ VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO ====================== */
		//// MOZART_V3_MBINFO
		//// Reference for SetOption VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO
		//if (dwFrameCount == 5)
		//{
        //
		//	tEncROIWindowsOptions.bROIEnable = TRUE;
		//	tEncROIWindowsOptions.dwROIWindowNum = 1;
		//	tEncROIWindowsOptions.dwStartMBXNum = 0;
		//	tEncROIWindowsOptions.dwStartMBYNum = 0;
		//	tEncROIWindowsOptions.dwEndMBXNum = 20;
		//	tEncROIWindowsOptions.dwEndMBYNum = 20;
		//	tEncROIWindowsOptions.sdwDeltaQp = -5;
		//	tEncROIWindowsOptions.dwEncodingInterval = 5;
        //
		//	tEncOptions.eOptionFlags = VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO;
		//	tEncOptions.adwUserData[0] = &tEncROIWindowsOptions;
		//	tEncOptions.adwUserData[1] = 0;
        //
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
        //
		//	tEncROIWindowsOptions.bROIEnable = TRUE;
		//	tEncROIWindowsOptions.dwROIWindowNum = 3;
		//	tEncROIWindowsOptions.dwStartMBXNum = 5;
		//	tEncROIWindowsOptions.dwStartMBYNum = 5;
		//	tEncROIWindowsOptions.dwEndMBXNum = 25;
		//	tEncROIWindowsOptions.dwEndMBYNum = 25;
		//	tEncROIWindowsOptions.sdwDeltaQp = -20;
		//	tEncROIWindowsOptions.dwEncodingInterval = 1;
        //
		//	tEncOptions.eOptionFlags = VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO;
		//	tEncOptions.adwUserData[0] = &tEncROIWindowsOptions;
		//	tEncOptions.adwUserData[1] = 0;
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
        //
		//	tEncROIWindowsOptions.bROIEnable = TRUE;
		//	tEncROIWindowsOptions.dwROIWindowNum = 8;   // back ground
		//	tEncROIWindowsOptions.dwStartMBXNum = 15;
		//	tEncROIWindowsOptions.dwStartMBYNum = 15;
		//	tEncROIWindowsOptions.dwEndMBXNum = 20;
		//	tEncROIWindowsOptions.dwEndMBYNum = 20;
		//	tEncROIWindowsOptions.sdwDeltaQp = -13;
		//	tEncROIWindowsOptions.dwEncodingInterval = 5;
        //
		//	tEncOptions.eOptionFlags = VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO;
		//	tEncOptions.adwUserData[0] = &tEncROIWindowsOptions;
		//	tEncOptions.adwUserData[1] = 0;
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
		//}
        //
		//if (dwFrameCount == 12)
		//{
		//	tEncOptions.eOptionFlags = VIDEO_CHANGE_REGION_OF_INTEREST_INFO;
		//	tEncOptions.adwUserData[0] = &tEncROIWindowsOptions;
		//	tEncOptions.adwUserData[1] = -2;
		//	tEncOptions.adwUserData[2] = FALSE;
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
		//}
		//// MOZART_V3_MBINFO Complete
/* ============ VIDEO_CHANGE_REGION_OF_INTEREST_WINDOWS_INFO ====================== */

/* ============ VIDEO_CHANGE_I_SLICE_QUALITY_STRATEGY ====================== */
		//if (dwFrameCount == 15)
		//{
		//	tEncOptions.eOptionFlags = VIDEO_CHANGE_I_SLICE_QUALITY_STRATEGY;
		//	tEncOptions.adwUserData[0] = TRUE;
		//	tEncOptions.adwUserData[1] = 3;
		//	if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//	{
		//		printf("Set options fail !!\n");
		//		goto ENCODE_END;
		//	}
		//}
/* ============ VIDEO_CHANGE_I_SLICE_QUALITY_STRATEGY ====================== */
		if (H264ENC_CURR_BUS_NUM == 0)
		{
			//MemMgr_CacheFlush(hMemObject, pbyFrame, (((MAX_WIDTH+15)>>4)<<4)*(((MAX_HEIGHT+15)>>4)<<4)*3/2);
		}

#ifdef __USE_PROFILE__
        Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__

		if (H264Enc_OneFrame(hEncObject, &tEncState) != S_OK)
    	{
    		printf("Encode %d frame fail !!\n", dwFrameCount);
    		break;
    	}

//		if (H264Enc_StartOneFrame(hEncObject, &tEncState) != S_OK)
//		{
//			printf("Encode %d frame fail !!\n", dwFrameCount);
//			//break;
//		}
//#ifdef __BASE_MV_OUTPUT__
//		// Get information
//		tEncOptions.eOptionFlags = VIDEO_GET_MB_INFO_BUFF;
//		tEncOptions.adwUserData[0] = (DWORD )pbyMBInfos;
//		tEncOptions.adwUserData[1] = TRUE;
//		tEncOptions.adwUserData[2] = 1;
//		if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
//		{
//			printf("Set options fail !!\n");
//			break;
//		}
//#endif //__BASE_MV_OUTPUT__
//
//		if (H264Enc_WaitOneFrame(hEncObject, &tEncState) != S_OK)
//		{
//			printf("Encode %d frame fail !!\n", dwFrameCount);
//			//break;
//		}

#ifdef __USE_PROFILE__
        Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__


#ifdef __BASE_MV_OUTPUT__
		// MOZART_V3_MBINFO
		//{
		//	DWORD i, j;
		//	DWORD dwMBXNum = (dwEncWidth>>4);
		//	DWORD dwMBYNum = (dwEncHeight>>4);
        //
		//	ptMBInfos = (TH264EncMBInfos *)pbyMBInfos;
        //
		//	for (j = 0; j < dwMBYNum; j++)
		//	{
		//		for (i = 0; i < dwMBXNum; i++)
		//		{
		//			printf("======== MBX/Y = [%3d][%3d]/[%3d]\n", dwFrameCount,i, j);
		//			printf("dwMBType = %2d\n", ptMBInfos[j*dwMBXNum + i].byMBTypeSkip&0x7F);
		//			printf("bMBSkip = %2d\n", (ptMBInfos[j*dwMBXNum + i].byMBTypeSkip>>7)&0x1);
		//			printf("scMVX = %2d\n", ptMBInfos[j*dwMBXNum + i].scMVX);
		//			printf("scMVY = %2d\n", ptMBInfos[j*dwMBXNum + i].scMVY);
		//			//printf("wHdrBits = %2d\n", ptMBInfos[j*dwMBXNum + i].byHdrBits);
		//			//printf("wRsdBits = %2d\n", ptMBInfos[j*dwMBXNum + i].byRsdBits);
		//		}
		//	}
		//}
		// MOZART_V3_MBINFO Complete
#endif //__BASE_MV_OUTPUT__
		/*
		{
			DWORD i;

			for (i=0; i<tEncState.dwNALCount; i++)
			{
				printf("NAL type = %d, NAL byte count = %d\n", tEncState.aeNALType[i], tEncState.adwNALBytes[i]);
			}
		}
		*/
        tEncState.dwTicks+=(dwFrameSkip+1);

		dwFrameBytes = tEncState.dwEncBytes;

#ifdef __SHA1__		
        Write_Frame_SHA1(hEncObject, pfRecOutput, pbySHA1);		
        GetRecSHA1(tEncInitOptions,
#ifdef __USE_DCE_SHA1__        
		            tDataCryptoState, 
		            hDataCryptoObject , 
#endif //__USE_DCE_SHA1__		            
		            pbyDigest, 
		            pbySHA1, 
		            pfRecOutput);
#else //
        //Write_Frame(hEncObject, pfRecOutput);		            
#endif //#ifdef __SHA1__		
		//Cal_PSNR(hEncObject, &tEncState, FALSE);
#ifndef H264ENC_LOOP
		fwrite(pbyNetbuf, sizeof(BYTE), dwFrameBytes, pfOutput);
#endif //H264ENC_LOOP
		//tEncOptions.eOptionFlags = VIDEO_GET_FRAME_QUANT;
		//if (H264Enc_SetOptions(hEncObject, &tEncOptions) != S_OK)
		//{
		//	printf("Set options fail !!\n");
		//	goto ENCODE_END;
		//}
		//printf("%d Frame: Encoded bytes/bits = %d, %d, Q = %d, initQ = %d\n", dwFrameCount, dwFrameBytes, dwFrameBytes<<3, tEncOptions.adwUserData[0], tEncOptions.adwUserData[1]);
		printf("%d Frame: Encoded bytes/bits = %d, %d \n", dwFrameCount, dwFrameBytes, dwFrameBytes<<3);
		dwTotalBytes = dwTotalBytes + dwFrameBytes;
	}


ENCODE_END:
	printf("Bit rate = %.2f\n", dwTotalBytes*8*30.0/dwFrameCount);
	if (H264Enc_Release(&hEncObject) != S_OK)
	{
		printf("Release H.264 encoder object fail !!\n");
		exit(1);
	}
	else
	{
		printf("Release H.264 encoder object success !!\n");
	}
#ifdef __USE_PROFILE__
	printf("Total cycles in AHB unit = %d\n", dwTotalTicks*2);
	//{
	//	DWORD dwTargetFrame = 30;
	//	printf("xx %3.2f  MHz (H4EE HW: %3.2f, SW:  %2.2f MHz, Bus bandwidth(0, 2): %1.2f, %2.2f MHz),  FullHD1080p@%dfps, 2 reference frames, quality factor 20, bitrate@%dfps %5.2f kbits \n",
	//		((FLOAT)dwTotalTicks*2*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		(fOp*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		((FLOAT)dwTotalTicks*2*dwTargetFrame/(FLOAT)dwFrameCount - fOp*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount)/1000000,
	//		(fBW0*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		(fBW1*(FLOAT)dwTargetFrame/(FLOAT)dwFrameCount/1000000),
	//		dwTargetFrame,
	//		dwTargetFrame,
	//		(FLOAT)dwTotalBytes*8*dwTargetFrame/(FLOAT)dwFrameCount/1000);
	//}
    
	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

H264ENC_INITIAL_FAIL:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyNetbuf) != S_OK)
	{
		printf("Release memory manager pbyNetbuf fail !!\n");
		exit(1);
	}

NETBUF_FAIL:
#ifndef __SEQ_10FRAME_PROFILE__
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyFrame) != S_OK)
	{
		exit(1);
	}
#endif

OBJECTBUF_FAIL:
	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyObjectInfo) != S_OK)
	{
		printf("Release memory manager object fail !!\n");
		exit(1);
	}

FRAMEBUF_FAIL:
	if (MemMgr_Release(&hMemObject) != S_OK)
	{
		printf("Release memory manager hMemObject fail !!\n");
		exit(1);
	}

MEMMGR_INITIAL_FAIL:
	free(pbyEncUserData);

USRDATA_FAIL:
	fclose(pfOutput);

OUTFILE_FAIL:
	fclose(pfRecOutput);

RECFILE_FAIL:
	fclose(pfInput);
#ifdef __ASM_ARCH_PLATFORM_ROSSINI_H__
	fclose(pfOsdInput);
#endif //__ASM_ARCH_PLATFORM_ROSSINI_H__
	return 0;
}
/* =========================================================================================== */
