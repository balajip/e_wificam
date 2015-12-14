/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2007-2010 VN Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 *
 *******************************************************************************
 */

/*!
 *******************************************************************************
 * Copyright 2007-2010 VN, Inc. All rights reserved.
 *
 * \file
 * SharedBuffMgr_App.c
 *
 * \brief
 * A sample code to get sharedbuffer from venc encoder.
 *
 * \date
 * 2010/04/13
 *
 * \author
 * Vincent Ho
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>                 
#include <unistd.h>                
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "SharedBuffMgr.h"
#include "ubuffer.h"
#include "xmlwrapper.h"
#define VERSION "1.0.0.0"

static char const rcsid[] = "$Id$";

/* =========================================================================================== */
typedef enum command_type
{
	letNeedIntra,
	letNeedConf,
	letBitstrmStart,
	letBitstrmStop,
	letGetSnapShot
} ECommandType;

/* =========================================================================================== */
SCODE CreateFifo(char *pData, int *piFIFO, int iFlag);
SCODE SendCmdMsg(int iChNo, int iCmdFiFo, ECommandType etype);
SCODE GetTotalStreamNum(char *szVencCfg, DWORD *pdwTotalNum);
BOOL  IsSupportSnapShot(void);

int main(int argc, char **argv)
{
	// For SharedBufferMgr
	TSharedBuffMgrInitOptions tSharedBuffMgrInitOpts;
	TSharedBuffMgrState tSharedBuffMgrState;
	HANDLE hSharedBuffMgrObj = NULL;
	int fd = -1;
	DWORD	 dwStreamTotalNum = -1;
	
	// For select()
	int Maxfd = -1;
	fd_set readset;
	struct timeval       timeout;
	int                  iRes = 0;

	// For Command Fifo
	int     iFIFO_fd = -1;
	CHAR	*szFIFOPathName = "/tmp/venc/c0/command.fifo";

	// For parsing UBuffer
	TUBuffer *ptUBuffer = NULL;	
	DWORD dwUBuffPayloadLen;
	BYTE  *pbyUBuffPayload;

	// For dump files
	FILE    *pfH264Output = NULL;
	FILE    *pfMPEG4Output = NULL;
	FILE    *pfJPEGOutput = NULL;
	FILE	*pfSnapShotOutput = NULL;
	CHAR    aszJPEGFilePath[30];
	CHAR    aszDumpPath[2048];
	// General use
	int     istream_no = 0;
	int     iframeno = 0;
	DWORD   dwCodecType = 0;
	BOOL    bNeedIntra = TRUE;
	BOOL    bSendIntraMsg = FALSE;
	int     i = 0;
//	DWORD dwCounter = 0;

	BOOL bDumpBitstream=FALSE; // dump bistream
	
	BOOL bGetSnapShot=FALSE; //dump snapshot

	// Parsing arguments
	if (argc != 5)
	{
		printf("Usage: %s istream_no(0/1/2/3/4) frame_num path bDump(0/1/2)\n", argv[0]);
		exit(1);
	}
	istream_no = atoi(argv[1]);
	iframeno = atoi(argv[2]);

	memset(aszDumpPath,0x0,sizeof(aszDumpPath));
	if(argv[3]!=NULL)
	{
		strncpy(aszDumpPath,argv[3],strlen(argv[3]));
	}
	else
	{
		printf("Dump path is NULL!!\n");
		exit(0);
	}
	
	if (GetTotalStreamNum("/etc/conf.d/c0/venc_conf.xml", &dwStreamTotalNum) != S_OK)
	{
		printf("Parse /etc/conf.d/c0/venc_conf.xml to get total stream number failed!!\n");
		exit(0);
	}
	else
	{
		printf("Total Stream number is %d\n",dwStreamTotalNum);
	}
	
	if (istream_no >= dwStreamTotalNum)
	{
		printf("The istream_no %d is larger than total stream number %d\n", istream_no ,dwStreamTotalNum);
		exit(0);
	}
	
	if (atoi(argv[4])==1)
	{
		bDumpBitstream=TRUE;
	}
	else if(atoi(argv[4])==2)
	{
		bGetSnapShot=TRUE;
	}
	
	printf("venc_receiver version %s %s\n", rcsid,VERSION);
	if (bDumpBitstream)
	{
		if ((pfH264Output=fopen(aszDumpPath, "wb")) == NULL)
		{
			printf("Open output file %s fail !!\n",aszDumpPath);
			perror("error");
			goto exit;
		}
		if ((pfMPEG4Output=fopen(aszDumpPath, "wb")) == NULL)
		{
			printf("Open output file %s fail !!\n",aszDumpPath);
			perror("error");
			goto exit;
		}
	}
	
	if (bGetSnapShot)
	{
		iframeno  = 1;
		if (IsSupportSnapShot() == FALSE)
		{
			printf("The snapshot function does not supported by venc!\n");
			goto exit;
		}
		
		if ((pfSnapShotOutput=fopen(aszDumpPath, "wb")) == NULL)
		{
			printf("Open output file %s fail !!\n",aszDumpPath);
			perror("error");
			goto exit;
		}
		
	}
	// Initialize command FIFO between venc
	if (CreateFifo(szFIFOPathName, &iFIFO_fd, O_WRONLY) != S_OK)
	{
		printf("Create Fifo fail...\n");
	}
	printf("Create Fifo success...\n");

	// Initialize SharedBuffMgr instance
	tSharedBuffMgrInitOpts.dwVersion = SHAREDBUFFMGR_VERSION;
	tSharedBuffMgrInitOpts.pObjectMem = NULL;
	
	if (bGetSnapShot == FALSE)
	{
		tSharedBuffMgrInitOpts.dwMinorNum = istream_no + 1;
	}
	else
	{
		tSharedBuffMgrInitOpts.dwMinorNum = istream_no + 1 + dwStreamTotalNum;	
	}
	
	tSharedBuffMgrInitOpts.eProcessRole = esbmprReader;

	if (SharedBuffMgr_Initial(&hSharedBuffMgrObj, &tSharedBuffMgrInitOpts) != S_OK)
	{
		printf("Initialize shared buffer manager object fail !!\n");
		exit(1);
	}
	printf("Initialize shared buffer manager object sucessful !!\n");

	// Retrieve fd of /dev/buff_mgr0 for using select()
	SharedBuffMgr_GetFileDescriptor(hSharedBuffMgrObj, &fd);	
	Maxfd = fd;
	FD_ZERO(&readset);

	if (bGetSnapShot == FALSE)
	{
		// Generate start command msg and send through fifo
		SendCmdMsg(istream_no, iFIFO_fd, letBitstrmStart);
	}
	else
	{
		SendCmdMsg(istream_no, iFIFO_fd, letGetSnapShot);
	}
	// Looping to get bitstream from ShardBuffMgr
	while (i < iframeno)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 500;

		FD_SET(fd, &readset);

		iRes = select(Maxfd+1, &readset, NULL, NULL, &timeout);
		if (iRes <= 0) 
		{
			continue;
		}			
		if (FD_ISSET(fd, &readset))
		{
			if (SharedBuffMgr_GetBuffer(hSharedBuffMgrObj, &tSharedBuffMgrState) == S_OK)
			{
				if (bGetSnapShot == FALSE)
				{
					ptUBuffer = (TUBuffer *)tSharedBuffMgrState.pbyHdrAddr;

					if (ptUBuffer->dwDataType == FOURCC_CONF)
					{
						// get a conf ubuffer, 
						// venc will send conf ubuffer when
						// 1. venc receive a forceCI message
						// 2. when the stream be changed codec type
						if (ptUBuffer->dwFollowingDataType == FOURCC_H264)
						{
							printf("stream codec is changed to H.264\n");
						}
						else if (ptUBuffer->dwFollowingDataType == FOURCC_MP4V)
						{
							printf("stream codec is changed to Mpeg4\n");
						}
						else if (ptUBuffer->dwFollowingDataType == FOURCC_JPEG)
						{
							printf("stream codec is changed to JPEG\n");
						}
						dwUBuffPayloadLen = 0;
						pbyUBuffPayload = NULL;
						dwCodecType = ptUBuffer->dwFollowingDataType;
						bNeedIntra = TRUE;
					}
					else
					{
						if (dwCodecType == 0)
						{
							//Unknown codec type, send FourceCI command to request a conf ubuffer
							SendCmdMsg(istream_no, iFIFO_fd, letNeedConf);
							printf("send needconf\n");
						}
						else
						{   
							if (bNeedIntra == TRUE)
							{
								if (!ptUBuffer->bIsSync) // check if a Intra frame
								{
									if (bSendIntraMsg == FALSE)
									{
										// If you need to start recording or streaming the bitstream from a I frame
										// You can send FourceIntra command to venc, and venc will encode the next
										// frame by I when venc received the command.
										// Otherwise you have to wait I frame according to venc's current I interval settings.
										SendCmdMsg(istream_no, iFIFO_fd, letNeedIntra);
										bSendIntraMsg = TRUE;
									}
									// Skip this frame since it's not a Intra frame
									SharedBuffMgr_ReleaseBuffer(hSharedBuffMgrObj, &tSharedBuffMgrState);
									continue;
								}
								bNeedIntra = FALSE;
								printf("get intra\n");
							}

							dwUBuffPayloadLen = ptUBuffer->dwSize - sizeof(TUBuffer) - ptUBuffer->dwUserDataSize;
							pbyUBuffPayload = tSharedBuffMgrState.pbyDataAddr;
							printf("(new)stream%d frame(%d):Size = %d\n", istream_no, i, dwUBuffPayloadLen);

							if (ptUBuffer->dwDataType == FOURCC_H264)
							{
								if (pfH264Output)
								{

									BYTE    *pbyUBufPld =   pbyUBuffPayload;
									DWORD   dwFrameByte = dwUBuffPayloadLen;
									DWORD dwByteStreamHeader = 0x01000000;
									DWORD   dwNALSize, dwTtlSize;
									dwTtlSize = 0;
									for (; dwFrameByte > sizeof(DWORD);) {
										if(bDumpBitstream)
										{
											fwrite(&dwByteStreamHeader, sizeof(BYTE), sizeof(DWORD), pfH264Output);
										}
										memcpy(&dwNALSize, pbyUBufPld, sizeof(DWORD));
										pbyUBufPld += sizeof(DWORD);
										if(bDumpBitstream)
										{
											fwrite(pbyUBufPld, sizeof(BYTE), dwNALSize, pfH264Output);
										}
										dwFrameByte -= sizeof(DWORD);
										pbyUBufPld += dwNALSize;
										dwFrameByte -= dwNALSize;
									}


									//fwrite(pbyUBuffPayload, dwUBuffPayloadLen, sizeof(BYTE), pfH264Output);
								}
								i++;
							}
							else if (ptUBuffer->dwDataType == FOURCC_MP4V)
							{
								DWORD   	dwBsBuffOffset = 0;
								WORD		wHdrTag = 0;
								WORD		wHdrLen = 0;
								BYTE		*pbyUBuffHdr = (BYTE *)tSharedBuffMgrState.pbyHdrAddr;
								if (ptUBuffer->dwUserDataSize > 0)
								{
									wHdrTag = *((WORD *)(pbyUBuffHdr+sizeof(TUBuffer)));
									if (wHdrTag == TAG_MP4V_EXTENINFO)
									{
										wHdrLen = *((WORD *)(pbyUBuffHdr+sizeof(TUBuffer)+sizeof(WORD)));
										dwBsBuffOffset = *((DWORD *)(pbyUBuffHdr+sizeof(TUBuffer)+wHdrLen));	
									}
								}
								if (pfMPEG4Output)
								{
									if( bDumpBitstream)
									{
										fwrite(pbyUBuffPayload+dwBsBuffOffset, dwUBuffPayloadLen, sizeof(BYTE), pfMPEG4Output);

									}
								}
								i++;
							}
							else if (ptUBuffer->dwDataType == FOURCC_JPEG)
							{
								if(bDumpBitstream)
								{
									sprintf(aszJPEGFilePath, "%s%i",aszDumpPath,i);
									if ((pfJPEGOutput = fopen(aszJPEGFilePath, "wb")) == NULL)
									{
										printf("Can't Open Output file %s\n", aszJPEGFilePath);
										perror("error");
									}
									else
									{
										fwrite(pbyUBuffPayload, dwUBuffPayloadLen, sizeof(BYTE), pfJPEGOutput);
										fclose(pfJPEGOutput);
									}
								}
								i++;
							}
						}
					} 
				}// if bGetSnapShot == FALSE
				else
				{
					DWORD	dwSnapShotFrameBytes = *((DWORD *)tSharedBuffMgrState.pbyHdrAddr);
					if (tSharedBuffMgrState.pbyDataAddr != NULL)
					{	
						printf("SnapShot size is %d\n",dwSnapShotFrameBytes);
						fwrite(tSharedBuffMgrState.pbyDataAddr, sizeof(BYTE), dwSnapShotFrameBytes, pfSnapShotOutput);
						fclose(pfSnapShotOutput);
					}
					break;
				}
				SharedBuffMgr_ReleaseBuffer(hSharedBuffMgrObj, &tSharedBuffMgrState);
				//dwCounter++;
				//if ( dwCounter%300 == 0)
					//printf("SharedBuffMgr_GetBuffer success %d times ...\n", dwCounter);
			}
		} // if FD_ISSET
	}

exit:

	if (bGetSnapShot == FALSE)
	{
		if (iFIFO_fd != -1) 
		{
			// Generate stop command msg and send through fifo
			SendCmdMsg(istream_no, iFIFO_fd, letBitstrmStop);
		}
	}
	if (hSharedBuffMgrObj != NULL)
	{
		// Release SharedBuffMgr instance
		if (SharedBuffMgr_Release(&hSharedBuffMgrObj) != S_OK)
		{
			printf("Release shared buffer manager object fail !!\n");
			exit(1);
		}
		printf("SharedBuffMgr_Release success ....\n");
	}
	if (iFIFO_fd != -1)
	{
		close(iFIFO_fd);
	}
	if (pfMPEG4Output != NULL)
	{
		fclose(pfMPEG4Output);
	}
	
	if (pfH264Output != NULL)
	{
		fclose(pfH264Output);
	}
	return 0;
}

/* =========================================================================================== */
SCODE CreateFifo(char *pData, int *piFIFO, int iFlag)
{
	int iFIFO,res;

	printf("FIFO path = %s\r\n",(char*)pData);

	if(access((char*)pData, F_OK) == -1)
	{
		res = mkfifo((char*)pData, 0777);
		if (res != 0)
		{
			printf("Could not create fifo %s\n",(char*)pData);
			return S_FAIL;
		}
	}

	//use blocking mode to open write or it will return -1 
	//if open read is not ready yet 

	if( iFlag == O_RDONLY )
		iFIFO = open( pData, O_RDONLY | O_NONBLOCK);
	else    	
		iFIFO = open( pData, O_WRONLY);

	if(  iFIFO  < 0  )
	{
		printf("open fifo failed %d\r\n",errno);
		return S_FAIL;    
	}
	else 
	{
		printf("open FIFO: %d success\r\n",iFIFO);
		*(int*)piFIFO = iFIFO;
		return S_OK;    
	}
}

static void GetTotalNum(void *userData, const char *s, int len)
{
	DWORD *pdwTotalNum = (DWORD *)userData;
	CHAR szTemp[len+1];
	DWORD dwVal = 0;

	memset(szTemp, 0x0, sizeof(szTemp));
	snprintf(szTemp, sizeof(szTemp), "%s", s);
	dwVal = strtoul(szTemp, (char**)NULL, 10);

	if (pdwTotalNum != NULL)
	{
		*pdwTotalNum = dwVal;
	}
	return;
}
SCODE GetTotalStreamNum(char *szVencCfg, DWORD *pdwTotalNum)
{
	int sRet;
	HANDLE hXmlWrapObj;
	
	TXmlWrapperTreeMap ptParsingMap[] = 
	{
		{"root/venc_encoder/total_num", NULL, &GetTotalNum, NULL},
		{NULL, NULL, NULL}
	};
	TXmlWrapperInitOptions tInitOptions;
	memset(&tInitOptions, 0x0, sizeof(TXmlWrapperInitOptions));

	
	tInitOptions.dwVersion = XMLWRAPPER_VERSION;
	if (XmlWrapper_Initial(&hXmlWrapObj, &tInitOptions) != S_OK)
	{
		return S_FAIL;
	}
	if (XmlWrapper_SetHandler(hXmlWrapObj, ptParsingMap, NULL) != S_OK)
	{
		printf("%s %d : XmlWrapper_SetHandler Fail! \n", __FILE__, __LINE__);
		return S_FAIL;
	}
	
		
	if ((sRet = XmlWrapper_ReadFile_UsrDefFunc(szVencCfg, hXmlWrapObj, pdwTotalNum)) != S_OK)
	{
		printf("%s %d : XmlWrapper_ReadFile_UsrDefFunc Fail! %x (%s)\n", __FILE__, __LINE__, sRet, szVencCfg);
		return S_FAIL;
	}
	if (XmlWrapper_Release(&hXmlWrapObj) != S_OK)
	{
		printf("Release XmlWrapper Fail! \n");
		return S_FAIL;
	}
	return sRet;
}

#define BUILD_CONFIG "/usr/local/firmware/.config"
static int foundKeywordOccurrence(char *fname, char *szkeyowrd) {
	
	FILE *fp;
	int find_result = 0;
	char temp[512];
	
	
	if((fp=fopen(fname, "r")) == NULL) {
		printf("Can not open %s\n", fname);
		goto exit;
	}

	while(fgets(temp, 512, fp) != NULL) {
		if((strstr(temp, szkeyowrd)) != NULL) {
			find_result++;
		}
		memset(temp, 0x0, sizeof(temp));
	}
	if(fp) {
		fclose(fp);
	}
exit:	
   	return find_result;
}

BOOL IsSupportSnapShot(void)
{
	BOOL bRet = FALSE;
	if (foundKeywordOccurrence(BUILD_CONFIG, "CONFIG_JPEG_SNAPSHOT=y") > 0)
	{
		bRet = TRUE;
	}
	return bRet;
}
/* =========================================================================================== */
/* Fifo message is defined in XML formate
 * <control>
 *    <request>
 *        <host> %s </host>
 *        <content> %s </content>
 *    </request>
 * </control>
 *
 * The elements of <host> are encoder0, encoder1 and encoder2 to indicate stream1, stream2 or stream3
 * The elements of <content> are start, stop, fourceCI and fourceIntra.
 *      start: request venc to start encodeing for the <host> stream.
 *      stop:  request venc to stop encodeing for the <host> stream.
 *      fourceCI: request venc to send a CONF type Ubuffer
 *      fourceIntra: request venc to encode a IntraFrame(I Frame).
 * */

#define CONTROL_MSG_START           "<content>start</content>"
#define CONTROL_MSG_STOP            "<content>stop</content>"
#define CONTROL_MSG_FORCECI         "<content>forceCI</content>"
#define CONTROL_MSG_FORCEINTRA      "<content>forceIntra</content>"
#define CONTROL_MSG_GETSNAPSHOT      "<content>GetSnapshot</content>"
#define CONTROL_MSG_PREFIX			"<control><request>"
#define CONTROL_MSG_POSTFIX			"</request></control>"

SCODE SendCmdMsg(int iChNo, int iCmdFiFo, ECommandType eType)
{
	CHAR    acTemp[512];
	int     iLen;

	switch(eType)
	{
		case letGetSnapShot:
			printf("Get snapshot\n");
			if (iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
						CONTROL_MSG_PREFIX, 
						iChNo, 
						CONTROL_MSG_GETSNAPSHOT,
						CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			
			break;
		case letNeedIntra:
			printf("Need Intra\n");
			if (iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
						CONTROL_MSG_PREFIX, 
						iChNo, 
						CONTROL_MSG_FORCEINTRA,
						CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letNeedConf:
			printf("Need Conf\n");
			if (iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
						CONTROL_MSG_PREFIX, 
						iChNo,
						CONTROL_MSG_FORCECI,
						CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letBitstrmStart:
			printf("Bitstrm start\n");
			if (iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
						CONTROL_MSG_PREFIX, 
						iChNo, 
						CONTROL_MSG_START,
						CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		case letBitstrmStop:
			printf("Bitstrm stop\n");
			if (iCmdFiFo >= 0)
			{
				acTemp[0] = 1;
				// force it using a long mode
				acTemp[1] = 0x84;

				sprintf(&(acTemp[6]), "%s<host>encoder%d</host>%s%s", 
						CONTROL_MSG_PREFIX, 
						iChNo, 
						CONTROL_MSG_STOP,
						CONTROL_MSG_POSTFIX);
				iLen = strlen(&(acTemp[6]));
				acTemp[5] =  iLen >> 24;
				acTemp[4] = (iLen & 0x00FF0000) >> 16;
				acTemp[3] = (iLen & 0x0000FF00) >> 8;
				acTemp[2] = (iLen & 0x000000FF);

				write(iCmdFiFo, acTemp, iLen+6);
				printf("write command %d bytes: %s\r\n", iLen+6, acTemp+6);
			}
			break;
		default:
			return S_FAIL;
	}
	return S_OK;
}
