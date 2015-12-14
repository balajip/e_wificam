/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/DataCrypto/DataCrypto_App/DataCrypto_App.c 11    13/01/16 5:13p Yiming.liu $
 *
 * Copyright 2008 ______ Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: DataCrypto_App.c $
 * 
 * *****************  Version 11  *****************
 * User: Yiming.liu   Date: 13/01/16   Time: 5:13p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 9  *****************
 * User: Albert.shen  Date: 09/01/08   Time: 11:38a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 08/04/18   Time: 9:45a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 08/02/01   Time: 1:02p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 6  *****************
 * User: Albert.shen  Date: 08/01/23   Time: 10:53a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 *
 * 
 * *****************  Version 5  *****************
 * User: Albert.shen  Date: 08/01/11   Time: 2:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 4  *****************
 * User: Albert.shen  Date: 08/01/10   Time: 7:11p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 3  *****************
 * User: Albert.shen  Date: 08/01/09   Time: 4:32p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 08/01/09   Time: 3:15p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 1  *****************
 * User: Albert.shen  Date: 07/12/19   Time: 10:30a
 * Created in $/rd_2/project/SoC/Linux_Libraries/DataCrypto/DataCrypto_App
 * 
 * *****************  Version 2  *****************
 * User: Albert.shen  Date: 07/09/21   Time: 9:43p
 * Updated in $/rd_2/project/SoC/Simulation_Models/DataCrypto_C_Simulation/DataCrypto_App
 * 
 * *****************  Version 1  *****************
 * User: Albert.shen  Date: 07/09/21   Time: 9:27p
 * Created in $/rd_2/project/SoC/Simulation_Models/DataCrypto_C_Simulation/DataCrypto_App
 * 
 * 
 */

/* =========================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>

#include "DataCrypto.h"
#include "MemMgr.h"
#ifdef __USE_PROFILE__
#include "Timer.h"
#endif //__USE_PROFILE__

/* =========================================================================================== */
#define MAX_SIZE			65536
#define USER_DATA_LENGTH	0
#define CALIBRATE_TIME		2

/* =========================================================================================== */
int main (int argc, char **argv)
{
	TDataCryptoInitOptions tDCEInitOptions;
	TDataCryptoState	tDataCryptoState;

	HANDLE hDCEObject;
	DWORD dwInputSize;
	DWORD dwOutSize;
	DWORD dwKeySize;
	DWORD dwOpMode;
	DWORD dwEncryptType;
	DWORD dwEncryptMode;
	DWORD dwHashType;
	DWORD dwHashMode;
	DWORD dwIVSize;
	DWORD dwObjectMemSize;
	DWORD dwBlockSz = 0;

    HANDLE hMemObject;
    TMemMgrInitOptions tMemMgrInitOptions;
	TMemMgrState tMemMgrState;

#ifdef __USE_PROFILE__
	HANDLE hTimerObject;
	TTimerInitOptions tTimerInitOptions;
	DWORD dwTotalTicks, dwTicks;
#endif //__USE_PROFILE__

	void *pObjectMem;
	FILE *pfOutput;
	FILE *pfInput;
	FILE *pfInKey;
	FILE *pfInitVec;
	BYTE *pbyInput;
	BYTE *pbyKey;
	BYTE *pbyInitVec;
	BYTE *pbyOutput;
	BYTE *pbyUserData;
	DWORD iRead, iContinue=1;

	if (argc != 12)
	{
        printf("Usage: DataCrypto\n"); 
		printf("       input_bitstream_file\n"); 
		printf("       key_bitstream_file\n");
		printf("       initial_vector_bitstream_file\n");
		printf("       output_bitstream_file\n");
		printf("       input_size\n");
		printf("       key_size\n");
		printf("       operation_mode (0: encryption, 1: decrytion, 2: hash)\n");
		printf("       encrypt_type (0: AES, 1: TDES, 2: DES)\n");
		printf("       encrypt_mode (0: ECB, 1: CBC, 2: CFB, 3: OFB, 4: CTR)\n");
		printf("       hash_type (0: SHA-1, 2: SHA-256, 3: SHA-224, 4: SHA-512, 5: SHA-384)\n");
		printf("       hash_mode (0: hashing only, 1: HMAC)\n");
		printf("\n\n");
		exit(1);
	}

	printf("Single-thread process start...\n");

	if ((pfInput=fopen(argv[1], "rb")) == NULL)
	{
		printf("Open input bitstream file fail !!\n");
		exit(1);
	}

	if ((pfInKey=fopen(argv[2], "rb")) == NULL)
	{
		printf("Open key bitstream file fail !!\n");
		exit(1);
	}

	if ((pfInitVec=fopen(argv[3], "rb")) == NULL)
	{
		printf("Open initial vector bitstream file fail !!\n");
		exit(1);
	}

	if ((pfOutput=fopen(argv[4], "wb")) == NULL)
	{
		printf("Open output bitstream file fail !!\n");
		exit(1);
	}

	dwInputSize = atoi(argv[5]);
	dwKeySize = atoi(argv[6]);
	dwOpMode = atoi(argv[7]);
	dwEncryptType = atoi(argv[8]);
	dwEncryptMode = atoi(argv[9]);
	dwHashType = atoi(argv[10]);
	dwHashMode = atoi(argv[11]);
	dwIVSize = 16;
	dwOutSize = (dwOpMode == 2) ? 128 : dwInputSize;

/* Initial Timer object. Timer object is used to do profile and you have to make sure that the timer 
   library is ready before you want to use it to profile your library. */
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

/* Initial MemMgr object. MemMgr object is used to allocate memory that hardware module can access
   directly without MMU in SoC platform. */
	tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
	tMemMgrInitOptions.pObjectMem = (void *)NULL;

    if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK)
	{
		printf("Initial memory manager object fail !!\n");
		exit(1);
	}

	//Get input buffer
	tMemMgrState.dwBusNum = DATACRYPTO_IN_BUS_NUM;
	tMemMgrState.dwSize = (dwInputSize + 127) & 0xFFFFFF80;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;

	if ((pbyInput = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
		printf("Allocate input buffer fail !!\n" );
		exit(1);
	}
	memset(pbyInput, 0, dwInputSize);


	tMemMgrState.dwSize = 128;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;

	if ((pbyKey = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
		printf("Allocate key buffer fail !!\n" );
		exit(1);
	}
	memset(pbyKey, 0, dwKeySize);

	tMemMgrState.dwSize = dwIVSize;
	tMemMgrState.eAlignType = ALIGN_TYPE_16_BYTE;

	if ((pbyInitVec = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
		printf("Allocate initial vector buffer fail !!\n");
		exit(1);
	}
	memset(pbyInitVec, 0, dwIVSize);

	tMemMgrState.dwBusNum = DATACRYPTO_OUT_BUS_NUM;
	tMemMgrState.dwSize = (dwInputSize + 127) & 0xFFFFFF80;
	tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;

	if ((pbyOutput = (BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL)
    {
		printf("Allocate output buffer fail !!\n" );
		exit(1);
	}

	if ((pbyUserData=(BYTE *)calloc(USER_DATA_LENGTH, sizeof(BYTE))) == NULL)
	{
		printf("Allocate user data buffer fail !!\n");
		exit(1);
	}

	tDCEInitOptions.dwVersion = DATACRYPTO_VERSION;
	tDataCryptoState.eOpMode = dwOpMode;
	tDataCryptoState.eCryptoType = dwEncryptType;
	tDataCryptoState.eCryptoMode = dwEncryptMode;
	tDataCryptoState.eHashType = dwHashType;
	tDataCryptoState.eHashMode = dwHashMode;
	tDataCryptoState.eHashStat = CRYPTO_HASH_STAT_BEGIN;
	tDataCryptoState.dwKeySize = dwKeySize;
	tDataCryptoState.dwTextSize = dwInputSize;
	tDataCryptoState.dwHashSize = 0;
	tDataCryptoState.pbyInput = pbyInput;
	tDataCryptoState.pbyKey = pbyKey;
	tDataCryptoState.pbyInitVector = pbyInitVec;
	tDataCryptoState.pbyOutput = pbyOutput;

#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
	if ((tDataCryptoState.eCryptoType == CRYPTO_ENCRYPT_TYPE_AES) || (tDataCryptoState.eHashType != CRYPTO_HASH_TYPE_SHA_1))
	{
		printf("The encryption/decryption/hash is not support!!");
	}
#endif

	dwObjectMemSize = DataCrypto_QueryMemSize(&tDCEInitOptions);
	pObjectMem = (void *)calloc(sizeof(BYTE), dwObjectMemSize);
	tDCEInitOptions.pObjectMem = pObjectMem;

	// For preparation .

	if (DataCrypto_Initial(&hDCEObject, &tDCEInitOptions) != S_OK)
	{
		printf("Initialize DataCrypto object fail !!\n");
		exit(1);
	}
	
	fread(pbyKey, sizeof(BYTE), dwKeySize, pfInKey);
	fread(pbyInitVec, sizeof(BYTE), dwIVSize, pfInitVec);

#ifdef __USE_PROFILE__
	dwTotalTicks = 0;
#endif //__USE_PROFILE__	

	if ((dwOpMode == CRYPTO_OP_ENCRYPT) || (dwOpMode == CRYPTO_OP_DECRYPT))
	{
		if (dwEncryptType == CRYPTO_ENCRYPT_TYPE_AES)
		{
			dwBlockSz = 16;
		} else if ((dwEncryptType == CRYPTO_ENCRYPT_TYPE_TDES) || (dwEncryptType == CRYPTO_ENCRYPT_TYPE_DES))
		{
			dwBlockSz = 8;
		}

		if (dwOpMode == CRYPTO_OP_ENCRYPT)
		{
			if ((dwEncryptMode == CRYPTO_ENCRYPT_MODE_EBC) || (dwEncryptMode == CRYPTO_ENCRYPT_MODE_CBC))
			{
				do 
				{
					iRead = fread(pbyInput, sizeof(BYTE), dwInputSize, pfInput);
					//fprintf(stderr, "read %d bytes\n", iRead);
					if (iRead == 0)
					{
						break;
					}

					if (iRead < dwInputSize)
					{
						DWORD i, iPad;
	
						iContinue = 0;
						iPad = dwBlockSz - iRead%dwBlockSz;
						dwOutSize = iRead + iPad;
						for (i=iRead; i<dwOutSize; i++)
						{
							pbyInput[i] = iPad;
						}
						iPad = pbyInput[dwOutSize-1];
						fprintf(stderr, "iPad is %ld \n", iPad);

						tDataCryptoState.dwTextSize = dwOutSize;
					} 

#ifdef __USE_PROFILE__
					Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
					if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
					{
						printf("Error occurs!!\n");
					}
#ifdef __USE_PROFILE__
					Timer_GetInterval(hTimerObject, &dwTicks);
					dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__
					//fprintf(stderr, "write %d bytes\n", dwOutSize);
					fwrite(tDataCryptoState.pbyOutput , sizeof(BYTE), dwOutSize, pfOutput);
				} while (iContinue);
			}
			if ((dwEncryptMode == CRYPTO_ENCRYPT_MODE_CFB) || (dwEncryptMode == CRYPTO_ENCRYPT_MODE_OFB) || (dwEncryptMode == CRYPTO_ENCRYPT_MODE_CTR))
			{
				do 
				{
					iRead = fread(pbyInput, sizeof(BYTE), dwInputSize, pfInput);
					//fprintf(stderr, "read %d bytes\n", iRead);
					if (iRead == 0)
					{
						break;
					}
					if (iRead < dwInputSize)
					{
						DWORD iPad;
	
						iPad = dwBlockSz - iRead%dwBlockSz;
						iContinue = 0;

						tDataCryptoState.dwTextSize = iRead+iPad;
					} 
#ifdef __USE_PROFILE__
					Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
					if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
					{
						printf("Error occurs!!\n");
					}
#ifdef __USE_PROFILE__
					Timer_GetInterval(hTimerObject, &dwTicks);
					dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__
					//fprintf(stderr, "write %d bytes\n", iRead);
					fwrite(tDataCryptoState.pbyOutput , sizeof(BYTE), iRead, pfOutput);
				} while (iContinue);
			}
		} 
		else if (dwOpMode == CRYPTO_OP_DECRYPT)
		{
			if ((dwEncryptMode == CRYPTO_ENCRYPT_MODE_EBC) || (dwEncryptMode == CRYPTO_ENCRYPT_MODE_CBC))
			{
				DWORD iFileLength, iTimes, iRead;
				fseek(pfInput, 0, SEEK_END);
				iFileLength = ftell(pfInput);
				rewind(pfInput);

				fprintf(stderr, "file is %ld bytes\n", iFileLength);
				iTimes = iFileLength/dwInputSize;
				fprintf(stderr, "iTimes is %ld \n", iTimes);
				if ((iFileLength - iTimes*dwInputSize) > 0)
				{
					iTimes++;
				}
				fprintf(stderr, "iTimes is %ld \n", iTimes);

				while (iTimes > 0)
				{
					iRead = fread(pbyInput, sizeof(BYTE), dwInputSize, pfInput);
					//fprintf(stderr, "read %d bytes\n", iRead);
					if (iRead < dwInputSize)
					{
						tDataCryptoState.dwTextSize = iRead;
					} 
#ifdef __USE_PROFILE__
					Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
					if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
					{
						printf("Error occurs!!\n");
					}
#ifdef __USE_PROFILE__
					Timer_GetInterval(hTimerObject, &dwTicks);
					dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__				
					if (iTimes == 1)
					{
						DWORD iPad;
						iPad = tDataCryptoState.pbyOutput[iRead-1];
						fprintf(stderr, "iPad is %ld \n", iPad);
						fwrite(tDataCryptoState.pbyOutput , sizeof(BYTE), iRead-iPad, pfOutput);
					} 
					else
					{
						fwrite(tDataCryptoState.pbyOutput , sizeof(BYTE), dwOutSize, pfOutput);
					}
					iTimes--;
				}
			}
			if ((dwEncryptMode == CRYPTO_ENCRYPT_MODE_CFB) || (dwEncryptMode == CRYPTO_ENCRYPT_MODE_OFB) || (dwEncryptMode == CRYPTO_ENCRYPT_MODE_CTR))
			{
				do 
				{
					iRead = fread(pbyInput, sizeof(BYTE), dwInputSize, pfInput);
					//fprintf(stderr, "read %d bytes\n", iRead);
					if (iRead == 0)
					{
						break;
					}
					if (iRead < dwInputSize)
					{
						DWORD iPad;
	
						iPad = dwBlockSz - iRead%dwBlockSz;
	
						iContinue = 0;
						tDataCryptoState.dwTextSize = iRead+iPad;
					} 
#ifdef __USE_PROFILE__
					Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
					if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
					{
						printf("Error occurs!!\n");
					}
#ifdef __USE_PROFILE__
					Timer_GetInterval(hTimerObject, &dwTicks);
					dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

					//fprintf(stderr, "write %d bytes\n", iRead);
					fwrite(tDataCryptoState.pbyOutput , sizeof(BYTE), iRead, pfOutput);
				} while (iContinue);
			}
		}
	} // if ((dwOpMode == OP_ENCRYPTION) || (dwOpMode == OP_DECRYPTION))

	if (dwOpMode == CRYPTO_OP_HASH)
	{

		DWORD iFileLength, iTimes, iRead, iWrite=0;		
		// set block size
		if (dwHashType == CRYPTO_HASH_TYPE_SHA_1)
		{
			dwBlockSz = 64; // 512bits
			iWrite = 20;	//the length of sha1 digest 160 bits
		} 
		else if (dwHashType == CRYPTO_HASH_TYPE_SHA_256)
		{
			dwBlockSz = 64; // 512bits
			iWrite = 32;	//the length of sha256 digest 256 bits
		} 
		else if (dwHashType == CRYPTO_HASH_TYPE_SHA_224)
		{
			dwBlockSz = 64; // 512bits
			iWrite = 28;	//the length of sha224 digest 224 bits
		} 
		else if (dwHashType == CRYPTO_HASH_TYPE_SHA_512)
		{
			dwBlockSz = 128; // 1024bits
			iWrite = 64;	//the length of sha512 digest 512 bits
		} 
		else if (dwHashType == CRYPTO_HASH_TYPE_SHA_384)
		{
			dwBlockSz = 128; // 1024bits
			iWrite = 48;	//the length of sha384 digest 384 bits
		}
	

		// get iteration times
		fseek(pfInput, 0, SEEK_END);
		iFileLength = ftell(pfInput);
		rewind(pfInput);

		fprintf(stderr, "file is %ld bytes\n", iFileLength);
		iTimes = iFileLength/dwInputSize;
		fprintf(stderr, "maybe iTimes is %ld \n", iTimes);
		if ((iFileLength - iTimes*dwInputSize) > 0)
		{
			iTimes++;
		}
		fprintf(stderr, "actually iTimes is %ld \n", iTimes);

		iRead = fread(pbyInput, sizeof(BYTE), dwInputSize, pfInput);
		//fprintf(stderr, "read %d bytes\n", iRead);
		if (iRead < dwInputSize)
		{
			tDataCryptoState.dwTextSize = iRead;

		} 
#ifdef __USE_PROFILE__
		Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__
		if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
		{
			printf("Error occurs!!\n");
		}
#ifdef __USE_PROFILE__
		Timer_GetInterval(hTimerObject, &dwTicks);
		dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__
		iTimes--;		
	
		// change hash state to middle
		tDataCryptoState.eHashStat = CRYPTO_HASH_STAT_MID;

		while (iTimes > 0)
		{
			iRead = fread(pbyInput, sizeof(BYTE), dwInputSize, pfInput);
			fprintf(stderr, "read %ld bytes\n", iRead);
			if (iRead == 0)
			{
				break;
			}
			if (iRead < dwInputSize)
			{
				tDataCryptoState.dwTextSize = iRead;
			} 
#ifdef __USE_PROFILE__
			Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__		
			if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
			{
				printf("Error occurs!!\n");
			}
#ifdef __USE_PROFILE__
			Timer_GetInterval(hTimerObject, &dwTicks);
			dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__
			iTimes--;
		}

		// change hash state to end
		tDataCryptoState.eHashStat = CRYPTO_HASH_STAT_END;

#ifdef __USE_PROFILE__
			Timer_ProfileStart(hTimerObject);
#endif //__USE_PROFILE__		
		if (DataCrypto_ProcessOneFrame(hDCEObject, &tDataCryptoState) != S_OK)
		{
			printf("Error occurs!!\n");
		}
#ifdef __USE_PROFILE__
			Timer_GetInterval(hTimerObject, &dwTicks);
			dwTotalTicks = dwTotalTicks + dwTicks;
#endif //__USE_PROFILE__

			fwrite(tDataCryptoState.pbyOutput , sizeof(BYTE), iWrite, pfOutput);
	} // if (dwOpMode == OP_HASH)

	printf("=================================================================\n");

#ifdef __USE_PROFILE__
/* Version 2.0.0.8 modification, 2013.01.16 */
	printf("Total cycles in AHB unit = %d\n", dwTotalTicks*2);
/* ======================================== */

	if (Timer_Release(&hTimerObject) != S_OK)
	{
		printf("Release timer object fail !!\n");
		exit(1);
	}
#endif //__USE_PROFILE__

	if (DataCrypto_Release(&hDCEObject) != S_OK)
	{
		printf("Release DataCrypto object fail !!\n");
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyInput)!= S_OK)
	{
		printf("Free Memory space fail !!\n");
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyInitVec)!= S_OK)
	{
		printf("Free Memory space fail !!\n");
		exit(1);
	}

	if (MemMgr_FreeMemory(hMemObject, (DWORD)pbyOutput)!= S_OK)
	{
		printf("Free Memory space fail !!\n");
		exit(1);
	}

	if (MemMgr_Release(&hMemObject) != S_OK)
	{
		printf("Release MemMgr object fail !!\n");
		exit(1);
	}

	if (pObjectMem != NULL)
	{
		free(pObjectMem);
	}
	if (pbyUserData != NULL)
	{
		free(pbyUserData);	
	}
	fclose(pfInput);
	fclose(pfInKey);
	fclose(pfInitVec);
	fclose(pfOutput);

	exit(0);
}

/* =========================================================================================== */
