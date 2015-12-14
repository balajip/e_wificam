/*

/* ============================================================================================== */
#include "VideoCap_Locals.h"
/* Version 5.5.0.1 modification, 2009.01.09 */
const CHAR VIDEOCAP_ID[] = "$Version: "VIDEOCAP_ID_VERSION"  (VIDEOCAP) $";
/* ======================================== */

/* ============================================================================================== */
SCODE VideoCap_Initial(HANDLE *phObject, TVideoCapInitOptions *ptInitOptions)
{
	TVideoCapInfo *ptInfo;
	TVideoSignalOptions tOptions;
/* Version 2.0.0.0 modification, 2006.06.21 */
	TMemMgrInitOptions tMemMgrInitOptions;
	DWORD dwPhyBaseAddr, dwMemSize;
/* ======================================== */
/* Version 3.0.0.0 modification, 2007.09.03 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
	TMemMgrState tMemMgrState;
	int i, j, k;
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* Version 6.3.0.0 modification, 2009.10.05 */
/* ======================================== */
/* ======================================== */
/* Version 8.0.0.2 modification, 2010.05.17 */
	HANDLE hEDMCDev;
	DWORD adwChipID[3];
/* Version 10.0.0.4 modification, 2011.10.21 */
	FILE *fpCPUInfo;
	CHAR  acTmp[200];
/* ======================================== */
	DWORD dwCTTbl[10];
/* ======================================== */
/* Version 12.0.0.3 modification, 2013.01.16 */
	SCODE scError;
	SDWORD sdwResult;
/* ======================================== */

	if (((ptInitOptions->dwVersion&0x00FF00FF)!=(VIDEOCAP_VERSION&0x00FF00FF)) |
		((ptInitOptions->dwVersion&0x0000FF00)>(VIDEOCAP_VERSION&0x0000FF00)))
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
/* Version 2.0.0.0 modification, 2006.06.21 */
		ERRPRINT("Invalid library version %ld.%ld.%ld.%ld !!", (int)(ptInitOptions->dwVersion&0xFF), (int)((ptInitOptions->dwVersion>>8)&0xFF),
															(int)((ptInitOptions->dwVersion>>16)&0xFF), (int)((ptInitOptions->dwVersion>>24)&0xFF));
		ERRPRINT("Please use version %ld.%ld.%ld.%ld or compatible versions !!", (int)(VIDEOCAP_VERSION&0xFF), (int)((VIDEOCAP_VERSION>>8)&0xFF),
																			(int)((VIDEOCAP_VERSION>>16)&0xFF), (int)((VIDEOCAP_VERSION>>24)&0xFF));
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
/* ======================================== */
		return _______VIDEOCAP_ERR_INVALID_VERSION;
	}
	else
	{
		printf("Start to use VideoCap library version %ld.%ld.%ld.%ld !!\n", ptInitOptions->dwVersion&0xFF, (ptInitOptions->dwVersion>>8)&0xFF, (ptInitOptions->dwVersion>>16)&0xFF, (ptInitOptions->dwVersion>>24)&0xFF);
	}

	if ((ptInitOptions->dwCapWidth+ptInitOptions->dwStartPixel) > ptInitOptions->dwInWidth)
	{
		ERRPRINT("The sum of the capture width and the start pixel must be less or equal to input width.\n");
		return _______VIDEOCAP_ERR_INVALID_ARG;
	}

	if ((ptInitOptions->dwCapHeight+ptInitOptions->dwStartLine) > ptInitOptions->dwInHeight)
	{
		ERRPRINT("The sum of the capture height and the start line must be less or equal to input height.\n");
		return _______VIDEOCAP_ERR_INVALID_ARG ;
	}

	if (ptInitOptions->dwCapWidth > ptInitOptions->dwMaxFrameWidth)
	{
		ERRPRINT("The max frame width must be larger or equal to capture width.\n");
		return _______VIDEOCAP_ERR_INVALID_ARG ;
	}

	if (ptInitOptions->dwCapHeight > ptInitOptions->dwMaxFrameHeight)
	{
		ERRPRINT("The max frame height must be larger or equal to capture height.\n");
		return _______VIDEOCAP_ERR_INVALID_ARG ;
	}
/* ======================================== */

	if (ptInitOptions->pObjectMem == NULL)
	{
		if ((ptInfo=(TVideoCapInfo *)calloc(sizeof(BYTE), VideoCap_QueryMemSize(ptInitOptions))) == NULL)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
/* Version 2.0.0.0 modification, 2006.06.21 */
			ERRPRINT("Fail to allocate object memory !!\n");
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
/* ======================================== */
			return _______VIDEOCAP_ERR_OUT_OF_SYSTEM_MEMORY;
/* ======================================== */
		}
		ptInfo->bAllocateExternal = FALSE;
	}
	else
	{
		memset(ptInitOptions->pObjectMem, 0, VideoCap_QueryMemSize(ptInitOptions));
		ptInfo = (TVideoCapInfo *)ptInitOptions->pObjectMem;
		ptInfo->bAllocateExternal = TRUE;
	}

	*phObject = (HANDLE)ptInfo;

/* Version 8.0.0.2 modification, 2010.05.17 */
#if 0
/* Version 10.0.0.4 modification, 2011.10.21 */
	ptInfo->dwChipVersion = 0;
	if ((fpCPUInfo=fopen("/proc/cpuinfo", "r")) == NULL)
	{
		printf("[VideoDisp]Error: /proc/cpuinfo does not exist !!\n");
		VideoCap_Release(phObject);
		return S_FAIL;
	}
	while (!feof(fpCPUInfo))
	{
		fscanf(fpCPUInfo, "%s", acTmp);
		if (strcmp(acTmp, "Revision") == 0)
		{
			fscanf(fpCPUInfo, "%s", acTmp);	// ":"
			fscanf(fpCPUInfo, "%s", acTmp);	// SYSC version
			if (strcmp(acTmp, "8010c02") == 0)
			{
				DBPRINT0("[VideoCap]: Mozart v2.\n");
				ptInfo->dwChipVersion = 2;
				break;
			}
			else if (strcmp(acTmp, "8010002") == 0)
			{
				DBPRINT0("[VideoCap]: Mozart v1.\n");
				ptInfo->dwChipVersion = 1;
				break;
			}
			else
			{
				printf("[VideoCap]Error: Unknown device!!\n");
				VideoCap_Release(phObject);
				fclose(fpCPUInfo);
				return S_FAIL;
			}
		}
	}
	fclose(fpCPUInfo);
	if (ptInfo->dwChipVersion == 0)
	{
		printf("[VideoCap]Error: Unknown device !!\n");
		VideoCap_Release(phObject);
		return S_FAIL;
	}
#endif	
/* ======================================== */
	if ((hEDMCDev=(HANDLE)open("/dev/vpl_edmc", O_RDWR)) < 0 )
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to open vpl_edmc device driver !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		VideoCap_Release(phObject);
		return _______VIDEOCAP_ERR_OPEN_DEVICE_DRIVER;
/* ======================================== */
	}
	ioctl((int)hEDMCDev, VPL_EDMC_IOC_GET_CHIP_ID, &adwChipID[0]);
	ioctl((int)hEDMCDev, VPL_EDMC_IOC_QUERY_CHIP_SUBTYPE, &ptInfo->dwChipSubType);
	if (close((int)hEDMCDev)!=0)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to close vpl_edmc device driver !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		VideoCap_Release(phObject);
		return _______VIDEOCAP_ERR_CLOSE_DEVICE_DRIVER;
/* ======================================== */
	}
/* ======================================== */

/* Version 2.0.0.0 modification, 2006.06.21 */
	tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
	tMemMgrInitOptions.pObjectMem = (void *)NULL;

/* Version 12.0.0.3 modification, 2013.01.16 */
	if ((scError=MemMgr_Initial(&ptInfo->hEDMCDev, &tMemMgrInitOptions)) != S_OK)
	{
		ERRPRINT("Fail to initialize MemMgr object !!\n");
		VideoCap_Release(phObject);
		return scError;
	}
/* ======================================== */

/* Version 5.6.0.2 modification, 2009.02.20 */
#ifdef __USE_PROFILE__
	ptInfo->dwBusCycles = 0;
	ptInfo->dwBusLatency = 0;
	ptInfo->dwBusReqTimes = 0;
#endif //__USE_PROFILE__
/* ======================================== */

/* Version 5.1.0.0 modification, 2008.10.20 */
	ptInfo->dwDeviceNum = ptInitOptions->dwDeviceNum;
/* Version 12.0.0.3 modification, 2013.01.16 */
	if ((scError=VideoCap_OpenVIC(ptInfo)) != S_OK)
	{
		VideoCap_Release(phObject);
		return scError;
	}

	if ((scError=VideoCap_CheckVIC(ptInfo)) != S_OK)
	{
		VideoCap_Release(phObject);
		return scError;
	}
/* ======================================== */

	ptInfo->dwFrameRate = ptInitOptions->dwFrameRate;
	ptInfo->dwFrameCount = 0;
/* Version 2.0.0.2 modification, 2006.08.24 */
/* Version 5.2.0.5 modification, 2008.11.27 */
	ptInfo->dwPrevTicks = 0;
	ptInfo->bInitPrevTicks = TRUE;
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.0 modification, 2008.02.02 */
	ptInfo->bHalfSizedOutput = FALSE;
/* ======================================== */

	if (ptInitOptions->dwFrameRate != 0)
	{
		ptInfo->dwDiffResidue = 0;
/* Version 5.4.0.0 modification, 2008.12.19 */
		ptInfo->dwInterval = 1000 / ptInfo->dwFrameRate;
		ptInfo->dwIntervalResidue = 1000 - ((1000/ptInfo->dwFrameRate)*ptInfo->dwFrameRate);
/* ======================================== */
	}

	if (ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_CHECK_INIT) == TRUE)
	{
		printf("vpl_vic device have Initialized !!\n");
		// if device is already initialized, map the physical address of the frame buffer for current process
/* Version 12.0.0.3 modification, 2013.01.16 */
		sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_FB_BASEADDR, &dwPhyBaseAddr);
		if (sdwResult != 0)
		{
			ERRPRINT("Fail to do vpl_vic device driver ioctl (IO Number %d) !!\n", VPL_VIC_IOC_GET_FB_BASEADDR);
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
			return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
		}
		sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_FB_SIZE, &dwMemSize);
		if (sdwResult != 0)
		{
			ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_FB_SIZE);
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
			return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
		}
/* ======================================== */

/* Version 6.0.0.4 modification, 2009.08.13 */
		if ((BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, dwPhyBaseAddr) == NULL)
		{
/* ======================================== */
			DBPRINT2("Map physical address 0x%08X with %d bytes !!\n", dwPhyBaseAddr, dwMemSize);
			if ((BYTE *)MemMgr_MapPhysAddr(ptInfo->hEDMCDev, dwPhyBaseAddr, dwMemSize) == NULL)
			{
/* Version 12.0.0.3 modification, 2013.01.16 */
				ERRPRINT("Fail to map the frame buffer of video capture !!\n");
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
/* Version 3.0.0.1 modification, 2007.12.18 */
				VideoCap_Release(phObject);
/* ======================================== */
				return _______VIDEOCAP_ERR_MAP_PHYSICAL_ADDRESS;
/* ======================================== */
			}
/* Version 3.1.0.0 modification, 2008.02.02 */
		}
/* Version 12.0.0.1 modification, 2012.11.02 */
		if (ptInitOptions->eFormat == VIDEO_SIGNAL_FORMAT_BAYER_PATTERN)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_STAT_GRID_BUFFER_BASEADDR, &dwPhyBaseAddr);
			if (sdwResult != 0)
			{
				ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_STAT_GRID_BUFFER_BASEADDR);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
			}
			sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_STAT_GRID_BUFFER_SIZE, &dwMemSize);
			if (sdwResult != 0)
			{
				ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_STAT_GRID_BUFFER_SIZE);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
			}
/* ======================================== */
			if ((ptInitOptions->pbyStatAEWBBufUsrBaseAddr=(BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, dwPhyBaseAddr)) == NULL)
			{
				if ((ptInitOptions->pbyStatAEWBBufUsrBaseAddr=(BYTE *)MemMgr_MapPhysAddr(ptInfo->hEDMCDev, dwPhyBaseAddr, dwMemSize)) == NULL)
				{
/* Version 12.0.0.3 modification, 2013.01.16 */
					ERRPRINT("Fail to map the grid statistic buffer !!\n");
					ERRPRINT("Please refer to the user guide for detailed actions.\n");
					VideoCap_Release(phObject);
					return _______VIDEOCAP_ERR_MAP_PHYSICAL_ADDRESS;
/* ======================================== */
				}		
			}

/* Version 12.0.0.3 modification, 2013.01.16 */
			sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_BASEADDR, &dwPhyBaseAddr);
			if (sdwResult != 0)
			{
				ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_BASEADDR);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
			}
			sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_SIZE, &dwMemSize);
			if (sdwResult != 0)
			{
				ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_STAT_HISTO_BUFFER_SIZE);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
			}
/* ======================================== */
			if ((ptInitOptions->pbyStatHistoBufUsrBaseAddr=(BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, dwPhyBaseAddr)) == NULL)
			{
				if ((ptInitOptions->pbyStatHistoBufUsrBaseAddr=(BYTE *)MemMgr_MapPhysAddr(ptInfo->hEDMCDev, dwPhyBaseAddr, dwMemSize)) == NULL)
				{
/* Version 12.0.0.3 modification, 2013.01.16 */
					ERRPRINT("Fail to map the histogram statistic buffer !!\n");
					ERRPRINT("Please refer to the user guide for detailed actions.\n");
					VideoCap_Release(phObject);
					return _______VIDEOCAP_ERR_MAP_PHYSICAL_ADDRESS;
/* ======================================== */
				}
			}

/* Version 12.0.0.3 modification, 2013.01.16 */
			sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_BASEADDR, &dwPhyBaseAddr);
			if (sdwResult != 0)
			{
				ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_BASEADDR);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
			}
			sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_SIZE, &dwMemSize);
			if (sdwResult != 0)
			{
				ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_GET_STAT_FOCUS_BUFFER_SIZE);
				ERRPRINT("Please refer to the user guide for detailed actions.\n");
				return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
			}
/* ======================================== */
			if ((ptInitOptions->pbyStatFocusBufUsrBaseAddr=(BYTE *)MemMgr_GetVirtAddr(ptInfo->hEDMCDev, dwPhyBaseAddr)) == NULL)
			{
				if ((ptInitOptions->pbyStatFocusBufUsrBaseAddr=(BYTE *)MemMgr_MapPhysAddr(ptInfo->hEDMCDev, dwPhyBaseAddr, dwMemSize)) == NULL)
				{
/* Version 12.0.0.3 modification, 2013.01.16 */
					ERRPRINT("Fail to map the focus statistic buffer !!\n");
					ERRPRINT("Please refer to the user guide for detailed actions.\n");
					VideoCap_Release(phObject);
					return _______VIDEOCAP_ERR_MAP_PHYSICAL_ADDRESS;
/* ======================================== */
				}
			}
		}		
/* ======================================== */
		ptInfo->bIsMaster = FALSE;
/* ======================================== */
		return S_OK;
	}
	else
	{
		printf("vpl_vic device have not initialized !!\n");
		if (ptInitOptions->pbyCapBufUsrBaseAddr == NULL)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			ERRPRINT("The capture buffers must be allocate before call VideoCap_Initial !!\n");
/* Version 3.0.0.1 modification, 2007.12.18 */
			VideoCap_Release(phObject);
/* ======================================== */
			return _______VIDEOCAP_ERR_INVALID_ARG;
/* ======================================== */
		}
/* Version 12.0.0.1 modification, 2012.11.02 */
		if (ptInitOptions->eFormat == VIDEO_SIGNAL_FORMAT_BAYER_PATTERN)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			if (ptInitOptions->pbyStatAEWBBufUsrBaseAddr == NULL)
			{
				ERRPRINT("The grid statistic buffer must be allocate before call VideoCap_Initial !!\n");
				VideoCap_Release(phObject);
				return _______VIDEOCAP_ERR_INVALID_ARG;
			}
			if (ptInitOptions->pbyStatHistoBufUsrBaseAddr == NULL)
			{
				ERRPRINT("The histogram statistic buffer must be allocate before call VideoCap_Initial !!\n");
				VideoCap_Release(phObject);
				return _______VIDEOCAP_ERR_INVALID_ARG;
			}
			if (ptInitOptions->pbyStatFocusBufUsrBaseAddr == NULL)
			{
				ERRPRINT("The focus statistic buffer must be allocate before call VideoCap_Initial !!\n");
				VideoCap_Release(phObject);
				return _______VIDEOCAP_ERR_INVALID_ARG;
			}
/* ======================================== */
		}		
/* ======================================== */
/* Version 3.1.0.0 modification, 2008.02.02 */
		ptInfo->bIsMaster = TRUE;
/* ======================================== */
	}
/* ======================================== */

/* Version 3.0.0.0 modification, 2007.09.03 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
/* Version 5.1.0.0 modification, 2008.10.20 */
	//if (ptInfo->bIsMaster == TRUE)
	{
		tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
		tMemMgrState.dwSize = 256*3*16;
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;

		if ((ptInfo->pbyGammaTbl=(BYTE*)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			ERRPRINT("Fail to allocate gamma table buffer !!\n");
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
			return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
		}
		else
		{
			DBPRINT1("Request %d bytes gamma table buffer succeed !!\n", 256*3*16);
		}
	}

	if (ptInfo->bIsMaster == TRUE)
	{
		tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
		tMemMgrState.dwSize = 1088*sizeof(DWORD);
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
		if ((ptInfo->pbyStatBufUsrBaseAddr=(BYTE *)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			ERRPRINT("Fail to allocate buffer for all statistic !!\n");
			ERRPRINT("Please refer to the user guide for detailed actions.\n");
			return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
		}
		else 
		{
			DBPRINT1("Request %d bytes StatBuf memory succeed !!\n", (int)tMemMgrState.dwSize);
		}
	}
/* ======================================== */

/* Version 5.0.0.0 modification, 2008.10.09 */
/* Version 5.8.0.0 modification, 2009.04.08 */
	ptInfo->dwBlackClamp			= ptInitOptions->adwConfig[0];
	ptInfo->dwHorBlanking			= ptInitOptions->adwConfig[1];
	ptInfo->byBayerBitwidth			= (BYTE)ptInitOptions->adwConfig[2];
	ptInfo->eCFAMode				= (EVideoCapCFAFlags)ptInitOptions->adwConfig[3];
	ptInfo->dwCFAEnhancedBayerHorPenThreshold = ptInitOptions->adwConfig[4];
	ptInfo->dwCFAEnhancedBayerVerPenThreshold = ptInitOptions->adwConfig[5];
	ptInfo->bAdditionalPad_en		= (BOOL)ptInitOptions->adwConfig[6];
	ptInfo->bDenoise_en				= (BOOL)ptInitOptions->adwConfig[7];
	ptInfo->dwThr0			= ptInitOptions->adwConfig[8];
	ptInfo->dwThr1				= ptInitOptions->adwConfig[9];
/* Version 7.2.0.4 modification, 2009.11.23 */
	ptInfo->byCFAPatternVShift		= (BYTE)ptInitOptions->adwConfig[10];
	ptInfo->byCFAPatternHShift		= (BYTE)ptInitOptions->adwConfig[11];
/* ======================================== */
	ptInfo->dwCMYGEdgeThr			= ptInitOptions->adwConfig[12];
	ptInfo->byCMYGAttenuateMethod	= (BYTE)ptInitOptions->adwConfig[13];
	ptInfo->dwCMYGEdgeDiffThr		= ptInitOptions->adwConfig[14];
	ptInfo->dwCMYGChromaDiffThr		= ptInitOptions->adwConfig[15];
	ptInfo->bGammaEn				= (BOOL)ptInitOptions->adwConfig[16];
	ptInfo->bManualWBEn				= (BOOL)ptInitOptions->adwConfig[17];
	ptInfo->byAWBMaxStep			= (BYTE)ptInitOptions->adwConfig[18];
	ptInfo->byAWBMinStep			= (BYTE)ptInitOptions->adwConfig[19];
//	ptInfo->bRESERVED0En				= (BOOL)ptInitOptions->adwConfig[20];
//	ptInfo->bRESERVED1En			= (BOOL)ptInitOptions->adwConfig[21];
	ptInfo->bAWBEn				= (BOOL)ptInitOptions->adwConfig[22];
	ptInfo->byAWBMax				= (BYTE)ptInitOptions->adwConfig[23];
	ptInfo->byAWBMin				= (BYTE)ptInitOptions->adwConfig[24];
	ptInfo->byAWBCbUpperBond		= (BYTE)ptInitOptions->adwConfig[25];
	ptInfo->byAWBCbLowerBond		= (BYTE)ptInitOptions->adwConfig[26];
	ptInfo->byAWBCrUpperBond		= (BYTE)ptInitOptions->adwConfig[27];
	ptInfo->byAWBCrLowerBond		= (BYTE)ptInitOptions->adwConfig[28];
	ptInfo->dwRESERVED0		= ptInitOptions->adwConfig[29];
	ptInfo->dwRESERVED1		= ptInitOptions->adwConfig[30];
	ptInfo->dwRESERVED2		= ptInitOptions->adwConfig[31];
	ptInfo->dwRESERVED3		= ptInitOptions->adwConfig[32];
/* Version 12.0.0.1 modification, 2012.10.15 */
	ptInfo->dwAWBGainUpperBond		= ptInitOptions->adwConfig[33];
	ptInfo->dwAWBGainLowerBond		= ptInitOptions->adwConfig[34];
	ptInfo->dwAWBRGain				= ptInitOptions->adwConfig[35];
	ptInfo->dwAWBBGain				= ptInitOptions->adwConfig[36];
/* ======================================== */
	ptInfo->dwCoeffRR				= ptInitOptions->adwConfig[37];
	ptInfo->dwCoeffGR				= ptInitOptions->adwConfig[38];
	ptInfo->dwCoeffBR				= ptInitOptions->adwConfig[39];
	ptInfo->dwCoeffRG				= ptInitOptions->adwConfig[40];
	ptInfo->dwCoeffGG				= ptInitOptions->adwConfig[41];
	ptInfo->dwCoeffBG				= ptInitOptions->adwConfig[42];
	ptInfo->dwCoeffRB				= ptInitOptions->adwConfig[43];
	ptInfo->dwCoeffGB				= ptInitOptions->adwConfig[44];
	ptInfo->dwCoeffBB				= ptInitOptions->adwConfig[45];

	ptInfo->dwDeNoiseThrMax1			= ((ptInitOptions->adwConfig[46]>>16)&0xFFFF);
	ptInfo->dwDeNoiseThrMax2			= (ptInitOptions->adwConfig[46]&0xFFFF);
	ptInfo->dwDeNoiseThrMin1			= ((ptInitOptions->adwConfig[47]>>16)&0xFFFF);
	ptInfo->dwDeNoiseThrMin2			= (ptInitOptions->adwConfig[47]&0xFFFF);
	ptInfo->dwAEWBWindowLocationX	= ptInitOptions->adwConfig[64];
	ptInfo->dwAEWBWindowLocationY	= ptInitOptions->adwConfig[65];
	ptInfo->dwAEWBWindowWidth		= ptInitOptions->adwConfig[66];
	ptInfo->dwAEWBWindowHeight		= ptInitOptions->adwConfig[67];

	ptInfo->bAFRefFrameSel			= (BOOL)ptInitOptions->adwConfig[100];
	ptInfo->bAFEn					= (BOOL)ptInitOptions->adwConfig[101];
	ptInfo->bAFRestart				= (BOOL)ptInitOptions->adwConfig[102];
	ptInfo->bAFZoomTrackAFStart     = (BOOL)ptInitOptions->adwConfig[103];
	ptInfo->dwAFFrameIntv			= ptInitOptions->adwConfig[104];
	ptInfo->dwAFMaxReturnNum		= ptInitOptions->adwConfig[105];
	ptInfo->dwAFFineTuneFinishThrsh = ptInitOptions->adwConfig[106];
	ptInfo->dwAFNoiseThresh			= ptInitOptions->adwConfig[107];
	ptInfo->dwAFRestartUpperDiff	= ptInitOptions->adwConfig[108];
	ptInfo->dwAFRestartLowerDiff	= ptInitOptions->adwConfig[109];
	ptInfo->dwAFInitStepSize		= ptInitOptions->adwConfig[110];
	ptInfo->dwAFTotalPlaneNum		= ptInitOptions->adwConfig[111];
	ptInfo->dwAFInitPlaneNum        = ptInitOptions->adwConfig[112];
	ptInfo->dwAFWindowStartPosX		= ptInitOptions->adwConfig[113];
	ptInfo->dwAFWindowStartPosY		= ptInitOptions->adwConfig[114];
	ptInfo->dwAFWindowWidth			= ptInitOptions->adwConfig[115];
	ptInfo->dwAFWindowHeight		= ptInitOptions->adwConfig[116];
	ptInfo->byClipYUpper			= (BYTE)ptInitOptions->adwConfig[117];
	ptInfo->byClipYLower			= (BYTE)ptInitOptions->adwConfig[118];
	ptInfo->byClipCbCrUpper			= (BYTE)ptInitOptions->adwConfig[119];
	ptInfo->byClipCbCrLower			= (BYTE)ptInitOptions->adwConfig[120];
	ptInfo->byYKiller				= (BYTE)ptInitOptions->adwConfig[121];
/* Version 13.0.0.2 modification, 2013.12.09 */
	ptInfo->bTMEn					= (BOOL)ptInitOptions->adwConfig[122];
/* ======================================== */
	ptInfo->bTMKeyTuningEn			= (BOOL)ptInitOptions->adwConfig[123];
	ptInfo->byTMKeyValue			= (BYTE)ptInitOptions->adwConfig[124];
	ptInfo->byTMExpValue			= (BYTE)ptInitOptions->adwConfig[125];
	ptInfo->dwTMLSBMask				= ptInitOptions->adwConfig[126];
	ptInfo->dwTMAvg					= ptInitOptions->adwConfig[127];
	ptInfo->bCEEn					= (BOOL)ptInitOptions->adwConfig[128];
	ptInfo->bCEAutoEn				= (BOOL)ptInitOptions->adwConfig[129];
	ptInfo->byCEDLMethod			= (BYTE)ptInitOptions->adwConfig[130];
	ptInfo->byCEMethod				= (BYTE)ptInitOptions->adwConfig[131];
	ptInfo->byCEBrightness			= (BYTE)ptInitOptions->adwConfig[132];
/* Version 13.0.0.2 modification, 2013.12.09 */
	ptInfo->bPhotoLDCEn             = (BOOL)ptInitOptions->adwConfig[133];
/* ======================================== */
	ptInfo->bAWBAfterCC				= (BOOL)ptInitOptions->adwConfig[134];
/* Version 8.1.0.0 modification, 2011.06.21 */
	ptInfo->dwCoeffROffset			= (BOOL)ptInitOptions->adwConfig[135];
	ptInfo->dwCoeffGOffset			= (BOOL)ptInitOptions->adwConfig[136];
	ptInfo->dwCoeffBOffset			= (BOOL)ptInitOptions->adwConfig[137];
	ptInfo->dwGammaOverflowSlope	= (BOOL)ptInitOptions->adwConfig[138];
/* ======================================== */
/* ======================================== */
/* ======================================== */

	for (j=0; j<3; j++)
	{
		for (i=0; i<256; i++)
		{
			for (k=0; k<16; k++)
			{
				ptInfo->pbyGammaTbl[(j<<8)*16 + (i*16)+k] = (ptInfo->bGammaEn==TRUE) ? abyGammaTbl[i] : i;
//				ptInfo->pbyGammaTbl[(j<<8)*16 + (i*16)+k] = 255;
			}
		}
	}

/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.1.0.6 modification, 2008.03.19 */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* Version 6.2.0.0 modification, 2009.10.02 */
/* Version 4.0.0.2 modification, 2008.10.06 */
	tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
	tMemMgrState.dwSize = 256*sizeof(DWORD);
	tMemMgrState.eAlignType = ALIGN_TYPE_256_BYTE;

	if ((ptInfo->pdwCETbl0=(DWORD*)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to allocate CE table 0 buffer !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
	}
	else
	{
		DBPRINT1("Request %d bytes CE table 0 buffer succeed !!\n", 256*sizeof(DWORD));
	}

	for (i=0; i<256; i++)
	{
		ptInfo->pdwCETbl0[i] = adwCETbl0[i];
	}

	tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
	tMemMgrState.dwSize = 256*sizeof(DWORD);
	tMemMgrState.eAlignType = ALIGN_TYPE_256_BYTE;

	if ((ptInfo->pdwCETbl1=(DWORD*)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to allocate CE table 1 buffer !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
	}
	else
	{
		DBPRINT1("Request %d bytes CE table 1 buffer succeed !!\n", 256*sizeof(DWORD));
	}

	for (i=0; i<256; i++)
	{
		ptInfo->pdwCETbl1[i] = adwCETbl1[i];
	}

	tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
	tMemMgrState.dwSize = 256*sizeof(DWORD);
	tMemMgrState.eAlignType = ALIGN_TYPE_256_BYTE;

	if ((ptInfo->pdwCETbl2=(DWORD*)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to allocate CE table 2 buffer !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
	}
	else
	{
		DBPRINT1("Request %d bytes CE table 2 buffer succeed !!\n", 256*sizeof(DWORD));
	}

	for (i=0; i<256; i++)
	{
		ptInfo->pdwCETbl2[i] = adwCETbl2[i];
	}

	tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
	tMemMgrState.dwSize = 256*sizeof(DWORD);
	tMemMgrState.eAlignType = ALIGN_TYPE_256_BYTE;

	if ((ptInfo->pdwCETbl3=(DWORD*)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to allocate CE table 3 buffer !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
	}
	else
	{
		DBPRINT1("Request %d bytes CE table 3 buffer succeed !!\n", 256*sizeof(DWORD));
	}

	for (i=0; i<256; i++)
	{
		ptInfo->pdwCETbl3[i] = adwCETbl3[i];
	}
/* ======================================== */
/* ======================================== */
/* Version 6.2.0.0 modification, 2009.10.02 */
	tMemMgrState.dwBusNum = VIDEOCAP_IN_BUS_NUM;
	tMemMgrState.dwSize = 256*sizeof(DWORD);
	tMemMgrState.eAlignType = ALIGN_TYPE_256_BYTE;

	if ((ptInfo->pdwCEUserTbl=(DWORD*)MemMgr_GetMemory(ptInfo->hEDMCDev, &tMemMgrState)) == NULL)
	{
/* Version 12.0.0.3 modification, 2013.01.16 */
		ERRPRINT("Fail to allocate CE user table buffer !!\n");
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_OUT_OF_HARDWARE_MEMORY;
/* ======================================== */
	}
	else
	{
		DBPRINT1("Request %d bytes CE user table buffer succeed !!\n", 256*sizeof(DWORD));
	}
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

/* Version 3.3.0.0 modification, 2008.09.19 */
/* Version 6.3.0.0 modification, 2009.10.05 */
/* ======================================== */
/* ======================================== */

/* Version 5.1.0.0 modification, 2008.10.20 */
/* ======================================== */

/* Version 8.0.0.0 modification, 2010.04.28 */
/* ======================================== */

	ptInfo->dwMaxFrameWidth = ptInitOptions->dwMaxFrameWidth;
	ptInfo->dwMaxFrameHeight = ptInitOptions->dwMaxFrameHeight;
/* Version 7.0.0.0 modification, 2009.10.11 */
	ptInfo->dwStartPixel = ptInitOptions->dwStartPixel;
	ptInfo->dwStartLine = ptInitOptions->dwStartLine;
/* ======================================== */
	ptInfo->dwCapWidth = ptInitOptions->dwCapWidth;
	ptInfo->dwCapHeight = ptInitOptions->dwCapHeight;

/* Version 8.0.0.0 modification, 2010.04.28 */
	ptInfo->dwVideoWidth = (ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_BAYER_PATTERN)
							? (ptInitOptions->dwStartPixel+ptInitOptions->dwCapWidth)
							: ptInitOptions->dwInWidth;

	ptInfo->dwVideoHeight = ptInitOptions->dwInHeight;
/* ======================================== */

	ptInfo->dwBufNum = ptInitOptions->dwBufNum;

/* Version 2.0.0.0 modification, 2006.06.21 */
/* ======================================== */

	ptInfo->bMono = FALSE;
	ptInfo->dwSaturation = ptInitOptions->dwSaturation;
/* Version 7.3.0.4 modification, 2010.03.05 */
	ptInfo->sdwBrightness = ((SDWORD)(ptInitOptions->dwBrightness<<24)) >> 24;
	ptInfo->sdwContrast = ((SDWORD)(ptInitOptions->dwContrast<<24)) >> 24;
/* ======================================== */
/* Version 7.0.0.0 modification, 2009.10.08 */
	ptInfo->bACSEn = FALSE;
/* Version 7.2.0.4 modification, 2009.11.23 */
	ptInfo->dwACSFactor = ACS_STEP;
/* ======================================== */
	ptInfo->dwACSLumaMax = 40;
	ptInfo->dwACSLumaMin = 20;
	ptInfo->dwACSLumaRange = ptInfo->dwACSLumaMax - ptInfo->dwACSLumaMin;
/* ======================================== */
/* Version 2.0.0.0 modification, 2006.06.21 */
/* ======================================== */
/* Version 4.0.0.0 modification, 2008.09.30 */
/* Version 5.10.0.0 modification, 2009.04.24 */
	if (ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW)
	{
		ptInfo->eFormat = 0;
	}
	else if (ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_RAW_16BITS)
	{
		ptInfo->eFormat = 1;
	}
	else if ((ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_CCIR656)||(ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_CCIR656))
	{
		ptInfo->eFormat = 2;
	}
	else if ((ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_INTERLACE_BT1120)||(ptInitOptions->eFormat==VIDEO_SIGNAL_FORMAT_PROGRESSIVE_BT1120))
	{
		ptInfo->eFormat = 3;
	}
	else
	{
		ptInfo->eFormat = ptInitOptions->eFormat;
	}
/* ======================================== */
/* ======================================== */

/* Version 12.0.0.3 modification, 2013.01.16 */
/* Version 2.0.0.0 modification, 2006.06.21 */
	if ((scError=VideoCap_InitVIC(ptInfo, ptInitOptions)) != S_OK)
	{
		VideoCap_Release(phObject);
		return scError;
/* ======================================== */
	}
/* ======================================== */
	dwCTTbl[0]  = ptInitOptions->adwConfig[49];
	dwCTTbl[1]  = ptInitOptions->adwConfig[51];
	dwCTTbl[2]  = ptInitOptions->adwConfig[53];
	dwCTTbl[3]  = ptInitOptions->adwConfig[55];
	dwCTTbl[4]  = ptInitOptions->adwConfig[57];
	dwCTTbl[5]  = ptInitOptions->adwConfig[50];
	dwCTTbl[6]  = ptInitOptions->adwConfig[52];
	dwCTTbl[7]  = ptInitOptions->adwConfig[54];
	dwCTTbl[8]  = ptInitOptions->adwConfig[56];
	dwCTTbl[9]  = ptInitOptions->adwConfig[58];
/* Version 12.0.0.3 modification, 2013.01.16 */
	sdwResult = ioctl((int)ptInfo->hVICDev, VPL_VIC_IOC_SET_COLOR_TEMPERATURE, (DWORD)&dwCTTbl);
	if (sdwResult != 0)
	{
		ERRPRINT("Fail to do vpl_vic device driver ioctl(IO Number %d) !!\n", VPL_VIC_IOC_SET_COLOR_TEMPERATURE);
		ERRPRINT("Please refer to the user guide for detailed actions.\n");
		return _______VIDEOCAP_ERR_DEVICE_IO_ERROR;
	}
/* ======================================== */

	return S_OK;
}

/* ============================================================================================== */
SCODE VideoCap_Release(HANDLE *phObject)
{
	TVideoCapInfo *ptInfo = (TVideoCapInfo *)(*phObject);
	SCODE scError;

/* Version 5.6.0.2 modification, 2009.02.20 */
#ifdef __USE_PROFILE__
	printf("Total VIC bandwidth consumption = %d\n", ptInfo->dwBusCycles);
	printf("Total VIC bus latency = %d\n", ptInfo->dwBusLatency);
	printf("Total VIC bus request time = %d\n", ptInfo->dwBusReqTimes);
#endif //__USE_PROFILE__
/* ======================================== */

	if (ptInfo != NULL)
	{
/* Version 2.0.0.0 modification, 2006.06.21 */
		if ((SDWORD)ptInfo->hVICDev >= 0)
		{
/* Version 12.0.0.3 modification, 2013.01.16 */
			if ((scError=VideoCap_CloseVIC(ptInfo)) != S_OK)
			{
				return scError;
/* ======================================== */
			}
			else
			{
				printf("Close VPL_VIC OK !!\n");
			}
		}

/* Version 3.0.0.0 modification, 2007.09.12 */
/* Version 7.3.0.2 modification, 2010.01.10 */
/* ======================================== */
/* Version 5.1.0.2 modification, 2008.10.27 */
		if (ptInfo->bIsMaster == TRUE)
		{
/* ======================================== */
/* Version 12.0.0.3 modification, 2013.01.16 */
			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyGammaTbl)) != S_OK)
			{
				ERRPRINT("Fail to release gamma table buffer !!\n");
				return scError;
/* ======================================== */
			}
			else
			{
				printf("Release gamma table buffer OK !!\n");
			}

/* Version 12.0.0.3 modification, 2013.01.16 */
			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pbyStatBufUsrBaseAddr)) != S_OK)
			{
				ERRPRINT("Fail to release buffer for all statistic !!\n");
				return scError;
/* ======================================== */
			}
			else
			{
				printf("Release buffer for all statistic OK !!\n");
			}

/* Version 13.0.0.1 modification, 2013.08.19 */
			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pdwCETbl0)) != S_OK)
			{
				ERRPRINT("Fail to release buffer for CE table 0 !!\n");
				return scError;
			}
			else
			{
				printf("Release buffer for CE table 0 OK !!\n");
			}

			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pdwCETbl1)) != S_OK)
			{
				ERRPRINT("Fail to release buffer for CE table 1 !!\n");
				return scError;
			}
			else
			{
				printf("Release buffer for CE table 1 OK !!\n");
			}

			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pdwCETbl2)) != S_OK)
			{
				ERRPRINT("Fail to release buffer for CE table 2 !!\n");
				return scError;
			}
			else
			{
				printf("Release buffer for CE table 2 OK !!\n");
			}

			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pdwCETbl3)) != S_OK)
			{
				ERRPRINT("Fail to release buffer for CE table 3 !!\n");
				return scError;
			}
			else
			{
				printf("Release buffer for CE table 3 OK !!\n");
			}

			if ((scError=MemMgr_FreeMemory(ptInfo->hEDMCDev, (DWORD)ptInfo->pdwCEUserTbl)) != S_OK)
			{
				ERRPRINT("Fail to release buffer for CE Usr table !!\n");
				return scError;
			}
			else
			{
				printf("Release buffer for CE Use table OK !!\n");
			}			
/* ======================================== */
/* Version 5.1.0.2 modification, 2008.10.27 */
		}
/* ======================================== */
/* Version 3.3.0.0 modification, 2008.09.19 */
/* ======================================== */
/* ======================================== */

/* Version 12.0.0.3 modification, 2013.01.16 */
		if ((scError=MemMgr_Release(&ptInfo->hEDMCDev)) != S_OK)
		{
			ERRPRINT("Fail to release memory management object !!\n");
			return scError;
/* ======================================== */
		}
		else
		{
			printf("Release memory management object OK !!\n");
		}
/* ======================================== */

		if (ptInfo->bAllocateExternal == FALSE)
		{
			free(ptInfo);
			ptInfo = NULL;
		}
	}

	*phObject = (HANDLE)NULL;

	return S_OK;
}

/* ============================================================================================== */
