/*
 * $Header: 
 *
 * Copyright 2008 VN, Inc.  All rights reserved.
 *
 * Description: ASC8852A Tamper Detection Example Application.
 *
 * 
 */

/* ============================================================================================== */


//Define Debug definitions here before header file. move this to makefile later in final version

#ifndef _DEBUG_L1
#define _DEBUG_L1
#endif
#ifndef _DEBUG_L2
//#define _DEBUG_L2
#endif
#ifndef _DEBUG_L3
//#define _DEBUG_L3
#endif
#ifndef _DEBUG_L4
//#define _DEBUG_L4
#endif

/*==============================     Include Files  ================================*/


#include "TamperDetect.h"


/*==============================     Definitions  =================================*/


/*===================    Modify the Previous Focus Threshold here  ==========================*/

#define FOCUS_DIFF_THRES_HIGH			(0x0)
#define FOCUS_DIFF_THRES_LOW			(0x1000000)


/*===================    Modify the Focus Average Thresholds here  ==========================*/

#define FOCUS_DIFF_THRES_AVG_HIGH	(0x0)
#define FOCUS_DIFF_THRES_AVG_LOW		(0x40000000)

/*=================   No of Previous frames to save focus average and RGB Sum ==================*/
#define NO_OF_FRAMES					(10U)


/*===================     Modify the Frame Skip Period here ===============================*/
#define FRAME_SKIP_PERIOD 				(10U)

/*===================    Modify the R/G/B Sum Thresholds here  ============================*/

#define R_SUM_THRESHOLD                         (0x1000000)
#define G_SUM_THRESHOLD                         (0x1000000)
#define B_SUM_THRESHOLD                         (0x1000000)

#define MSB_32_SHIFT                                 (32U)
#define LSB_32_MASK                                  (0xffffffff)

/*===============================================  Macro Definitions  ==================================*/

#define QWORD_2_DWORD(qwValue, dwHighValue, dwLowValue)                                 \
                {                                                                                                                      \
                    dwHighValue = (DWORD ) ((qwValue ) >>MSB_32_SHIFT);                           \
                    dwLowValue  = (DWORD ) ((qwValue) & LSB_32_MASK);                               \
                }


#define DWORD_2_QWORD(qwValue, dwHighValue, dwLowValue)                                                                 \
                {                                                                                                                                                      \
                    qwValue =  ((((QWORD)(dwHighValue)) << MSB_32_SHIFT) |(((QWORD)(dwLowValue))  & LSB_32_MASK ));    \
                }


/* ============================================================================================== */
int main(int argc, char* argv[])
{
	TVideoCapInitOptions tVideoCapInitOptions;
	TVideoCapState tVideoCapState;
	TMemMgrInitOptions tMemMgrInitOptions;
	HANDLE hMemObject=NULL;
	HANDLE hVideoCapObject=NULL;
	DWORD dwCount1=0;
	DWORD dwDevNum;
	DWORD dwFrameSize, dwYFrameSize, dwCFrameSize;
	DWORD MaxNoOfFrames;
	DWORD dwDummy;

	FILE *pfInput=NULL;
	CHAR acTmpText[200];


	BOOL bIsTamperDetect =FALSE;
	DWORD   dwInitalFrameCount=0;

	

	if ((pfInput=fopen(argv[1], "r")) == NULL) {
		DBPRINT_L1_1("Open input configuration file %s fail !!\n", argv[1]);
		exit(1);
	}
	dwDevNum = GetParam(pfInput);
	tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
	tMemMgrInitOptions.pObjectMem = (void *)NULL;
	if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK) {
		DBPRINT_L1_0("Initial memory manager object fail !!\n");
		exit(1);
	}
	else {
		DBPRINT_L1_0("MemMgr_Lib is initialized !!\n");
	}
	tVideoCapInitOptions.dwDeviceNum = dwDevNum;
	fscanf(pfInput, "%s", acTmpText);

	fgets(acTmpText, 200, pfInput);
	fscanf(pfInput, "%s", acTmpText);
	fgets(acTmpText, 200, pfInput);
	MaxNoOfFrames = GetParam(pfInput);
	tVideoCapInitOptions.dwVersion			= VIDEOCAP_VERSION;
	tVideoCapInitOptions.pObjectMem		= NULL;
	tVideoCapInitOptions.dwMaxFrameWidth	= GetParam(pfInput);	//used for capturing stride
	tVideoCapInitOptions.dwMaxFrameHeight	= GetParam(pfInput);
	tVideoCapInitOptions.dwCapWidth		= GetParam(pfInput);	//must be multiple of 16
	tVideoCapInitOptions.dwCapHeight		= GetParam(pfInput);	//must be multiple of 16
	tVideoCapInitOptions.dwStartPixel		= GetParam(pfInput);
	tVideoCapInitOptions.dwStartLine			= GetParam(pfInput);
	tVideoCapInitOptions.eOutPixelFormat 		= GetParam(pfInput);
	tVideoCapInitOptions.eFormat			= GetParam(pfInput);
	tVideoCapInitOptions.dwInWidth			= GetParam(pfInput);
	tVideoCapInitOptions.dwInHeight			= GetParam(pfInput);
	tVideoCapInitOptions.bFlip				= GetParam(pfInput);
	tVideoCapInitOptions.bMirror				= GetParam(pfInput);
	tVideoCapInitOptions.bFieldMode			= GetParam(pfInput);
	tVideoCapInitOptions.dwBrightness		= (DWORD)(GetParam(pfInput)&0x000000FF);
	tVideoCapInitOptions.dwContrast			= (DWORD)(GetParam(pfInput)&0x000000FF);
	tVideoCapInitOptions.dwSaturation		= GetParam(pfInput);
	tVideoCapInitOptions.bFieldInvert			= GetParam(pfInput);
	for (dwCount1=0; dwCount1<140; dwCount1++) {
		tVideoCapInitOptions.adwConfig[dwCount1] = GetParam(pfInput);
	}
	tVideoCapInitOptions.dwFrameRate		= GetParam(pfInput);
	tVideoCapInitOptions.dwBufNum			= 4;
	dwDummy                            = GetParam(pfInput);
	dwDummy							= GetParam(pfInput);

	fclose(pfInput);
	
	tVideoCapInitOptions.byChNum0 = 1;
	tVideoCapInitOptions.byChNum1 = 1;
	tVideoCapInitOptions.bChClkEdge0 = 0;
	tVideoCapInitOptions.bChClkEdge1 = 0;
	tVideoCapInitOptions.eChIDType0 = NO_CH_ID;
	tVideoCapInitOptions.eChIDType1 = NO_CH_ID;

	dwYFrameSize = tVideoCapInitOptions.dwMaxFrameWidth * tVideoCapInitOptions.dwMaxFrameHeight;
	dwCFrameSize = (tVideoCapInitOptions.eOutPixelFormat==YUV420) ? dwYFrameSize/4 : dwYFrameSize/2;
	dwFrameSize = dwYFrameSize + (2*dwCFrameSize);

	tVideoCapInitOptions.pbyCapBufUsrBaseAddr = NULL;
	tVideoCapInitOptions.pbyStatAEWBBufUsrBaseAddr = NULL;
	tVideoCapInitOptions.pbyStatHistoBufUsrBaseAddr = NULL;
	tVideoCapInitOptions.pbyStatFocusBufUsrBaseAddr = NULL;

	DBPRINT_L3_0("VideoCap is going to be initialized !!\n");
	if (VideoCap_Initial(&hVideoCapObject, &tVideoCapInitOptions) != S_OK) {
		DBPRINT_L1_0("VideoCap Object Initialization failed !!\n");
		exit(1);
	}
	DBPRINT_L3_2("CapWidth CapHeight %d %d\n", (int)tVideoCapInitOptions.dwCapWidth, (int)tVideoCapInitOptions.dwCapHeight);                                             
	DBPRINT_L3_3("InWidth InHeight Format %d %d %d\n", (int)tVideoCapInitOptions.dwInWidth, (int)tVideoCapInitOptions.dwInHeight, (int)tVideoCapInitOptions.eFormat);    
	DBPRINT_L3_2("VShift HShift %d %d\n", (int)tVideoCapInitOptions.adwConfig[10], (int)tVideoCapInitOptions.adwConfig[11] );                                            


	tVideoCapState.dwFrameCount =0;
	dwInitalFrameCount = 0;
	while ( tVideoCapState.dwFrameCount   <= (MaxNoOfFrames + dwInitalFrameCount)  )
	{
		if (VideoCap_TamperDetect(hVideoCapObject, &tVideoCapState, &bIsTamperDetect, (PDWORD)tVideoCapInitOptions.pbyStatFocusBufUsrBaseAddr, (PBYTE)tVideoCapInitOptions.pbyStatHistoBufUsrBaseAddr) != S_OK)
		{
			DBPRINT_L1_0("VideoCap_TamperDetect:  --------------- Error getting video capture buffer--------------\n\n");
		}
		else if (bIsTamperDetect == TRUE)
		{
			DBPRINT_L1_0("\nVideoCap_TamperDetect:  ---------------    Camera Tampering DETECTED             !!!!!!!!!!!!!!!!!!!!!!!!!!!    -------\n\n");
		}
		else
		{
			DBPRINT_L3_0("VideoCap_TamperDetect:  ------------       Camera Tampering      N O T   DETECTED               ----------\n\n");
		}
		if (dwInitalFrameCount == 0)
		{
				dwInitalFrameCount = tVideoCapState.dwFrameCount;
		}
	}
	DBPRINT_L1_3("tVideoCapState.dwFrameCount  =%u   MaxNoOfFrames =%u, dwInitalFrameCount =%u\n",tVideoCapState.dwFrameCount,MaxNoOfFrames,dwInitalFrameCount);

	if (VideoCap_Release(&hVideoCapObject) != S_OK) 
       {
		DBPRINT_L1_0("Release VideoCap object fail !!\n");
	}

	return S_OK;
}


/* ============================================================================================== */
SDWORD GetParam(FILE *pfInput)
{
	CHAR acTmpText[200];
	SDWORD sdwRet;

	fscanf(pfInput, "%s", acTmpText);
	sdwRet = atoi(acTmpText);
	fgets(acTmpText, 200, pfInput);

	return sdwRet;
}


SCODE 
VideoCap_TamperDetect(
							HANDLE			hObject, 
							TVideoCapState *	ptState,
							BOOL *			pbIsTamperDetect,/* Output Variable */
							PDWORD 		pdwStatFocusBuf,
							PBYTE			pbyStatHistoBuf
							)
{
		SCODE scStatus = S_OK;

		static DWORD dwOOF_PrevFramesAvg_High =0;
		static DWORD dwOOF_PrevFramesAvg_Low =0;
		static DWORD dwOOF_PrevFrames_High[NO_OF_FRAMES]	=	{0};  // circular array; static definition will anyway intialise. However {0} adds to more readable code
		static DWORD dwOOF_PrevFrames_Low[NO_OF_FRAMES]	=	{0};		


		static QWORD qwTD_R_Avg =0;
		static QWORD qwTD_G_Avg =0;
		static QWORD qwTD_B_Avg =0;

		static QWORD qwTD_R_Sum[NO_OF_FRAMES]	=	{0};  // circular array; static definition will anyway intialise. However {0} adds to more readable code
		static QWORD qwTD_G_Sum[NO_OF_FRAMES]	=	{0};		
		static QWORD qwTD_B_Sum[NO_OF_FRAMES]	=	{0};		

		static DWORD dwArrayIndex							=	0;
		static DWORD dwFrameNum							=     0;
		static DWORD dwPrevFrameCount  						=	0;


		DBPRINT_L4_0("\n\n\n-------------------VideoCap_TamperDetect(): Entered... -----------------------------\n");
		*pbIsTamperDetect =FALSE; //Initialise

		dwPrevFrameCount = 0xFFFFFFFF;

		VideoCap_Sleep(hObject); // Sleep before the check so that we dont keep printing /wasting CPU while we need to skip frames.

		if ( (dwFrameNum % FRAME_SKIP_PERIOD) == 0)
		{
/*-------------- Get the current frame ----------*/
        		scStatus = VideoCap_GetBuf(hObject, ptState);
                	{
        			//Check for errrors
        			if ((scStatus!=S_OK) || (ptState->dwFrameCount==dwPrevFrameCount))
        			{
        				if (ptState->bFifoFull == TRUE)
        				{
        					if (scStatus == S_OK)
        					{
        						DBPRINT_L1_1("Fifo Full (error frame) with S_OK, dwInWidth: %d !!\n",  (int)ptState->dwInWidth);
        					}
        					else
        					{
        						DBPRINT_L1_1(" Fifo Full (error frame) with S_FAIL, dwInWidth: %d !!\n",  (int)ptState->dwInWidth);
        					}
        				}
        				if (ptState->bNoSignal == TRUE)
        				{
        					DBPRINT_L1_0("No signal 1!!\n");
        				}
        				while ((scStatus!=S_OK) || (ptState->dwFrameCount==dwPrevFrameCount))
        				{
        					if (scStatus != S_OK)
        					{
        						DBPRINT_L2_0("Failed to get correct buffer after sleep!\n");
        					}
        					else  // (scStatus==S_OK)				
        					{
        						if (VideoCap_ReleaseBuf(hObject, ptState) != S_OK)
        						{
        							DBPRINT_L1_0("video_cap_release_buf err!\n");
        						}
        					}
        					DBPRINT_L4_0("before VideoCap_Sleep\n");
        					VideoCap_Sleep(hObject);
        					scStatus = VideoCap_GetBuf(hObject, ptState);
        					if (ptState->bFifoFull == TRUE)
        					{
        						if (scStatus == S_OK)
        						{
        							DBPRINT_L1_1(" Fifo Full (error frame) with S_OK, dwInWidth: %d !!\n", (int)ptState->dwInWidth);
        						}
        						else
        						{
        							DBPRINT_L1_1(" Fifo Full (error frame) with S_FAIL, dwInWidth: %d !!\n",  (int)ptState->dwInWidth);
        						}
        					}
        					if (ptState->bNoSignal == TRUE)
        					{
        						DBPRINT_L1_0("No signal 2 !!\n");
        					}
        				}
        			}
        		}


// To reach here, scStatus must be S_OK and a new frame is captured.
			DBPRINT_L3_0("VideoCap_TamperDetect():  Received Video Capture buffer successfully...................................................................................  \n");

/*-----------------------------  Update the Average   ------------------------*/
			{
				static BOOL dwArrayFull = FALSE;


				if(dwArrayFull)
				{
					dwOOF_PrevFramesAvg_High 	+= (ptState->dwAFFocusValueHigh/NO_OF_FRAMES )   -  (dwOOF_PrevFrames_High[dwArrayIndex] /NO_OF_FRAMES) ;
					dwOOF_PrevFramesAvg_Low 	+= (ptState->dwAFFocusValueLow /NO_OF_FRAMES )   - (dwOOF_PrevFrames_Low[dwArrayIndex] /NO_OF_FRAMES );

					qwTD_R_Avg	+=	 ( ptState->qwAWBRedSum      /  NO_OF_FRAMES )       - (qwTD_R_Sum[dwArrayIndex]	/	NO_OF_FRAMES);
					qwTD_G_Avg	+=	( ptState->qwAWBGreenSum  /   NO_OF_FRAMES )      -  (qwTD_G_Sum[dwArrayIndex]	/	NO_OF_FRAMES);
					qwTD_B_Avg	+=	( ptState->qwAWBBlueSum     /   NO_OF_FRAMES )      -  (qwTD_B_Sum[dwArrayIndex]	/	NO_OF_FRAMES);
				}
				else
				{
					DWORD dwLoopCount =0;

					dwOOF_PrevFramesAvg_High =dwOOF_PrevFramesAvg_Low =0;
					qwTD_R_Avg = qwTD_G_Avg = qwTD_B_Avg =0;

					dwOOF_PrevFrames_High[dwArrayIndex ]	=	ptState->dwAFFocusValueHigh;
					dwOOF_PrevFrames_Low[dwArrayIndex ]	=	ptState->dwAFFocusValueLow;

					qwTD_R_Sum[dwArrayIndex] = ptState->qwAWBRedSum;
					qwTD_G_Sum[dwArrayIndex] = ptState->qwAWBGreenSum;
					qwTD_B_Sum[dwArrayIndex] = ptState->qwAWBBlueSum;

					for(dwLoopCount =0;   dwLoopCount  < (dwArrayIndex+1); dwLoopCount++)
					{
						dwOOF_PrevFramesAvg_High	+=   (dwOOF_PrevFrames_High[dwLoopCount ]  /  (dwArrayIndex+1));
						dwOOF_PrevFramesAvg_Low		+=   (dwOOF_PrevFrames_Low[dwLoopCount ]   /   (dwArrayIndex+1));

						qwTD_R_Avg 				+=  (qwTD_R_Sum[dwLoopCount ]  /  (dwArrayIndex+1));
						qwTD_G_Avg 				+=  (qwTD_G_Sum[dwLoopCount ]  /  (dwArrayIndex+1));
						qwTD_B_Avg 				+=  (qwTD_B_Sum[dwLoopCount ]  /  (dwArrayIndex+1));
					}
				}

				if (dwArrayIndex == (NO_OF_FRAMES-1) )  // dwArrayIndex  points to the current index
				{
					dwArrayFull =TRUE;
				}
				DBPRINT_L3_6("VideoCap_TamperDetect(): FrameCount = %d,  ArrayIndex = %d   Avg:  0X%x  0X%x  Focus : 0X%x  0X%x\n", dwFrameNum, dwArrayIndex, dwOOF_PrevFramesAvg_High, dwOOF_PrevFramesAvg_Low, ptState->dwAFFocusValueHigh, ptState->dwAFFocusValueLow );
			}

                    DBPRINT_L4_2("VideoCap_TamperDetect(): Video Capstate: index=%d, framecount =%d,\n", ptState->dwIndex, ptState->dwFrameCount);
                    DBPRINT_L4_8("VideoCap_TamperDetect():   sec =0X%x, millisec =0X%x, microSec =0X%x bTimeStampCal =%d inwid=%d, outwid=%d, outht =%d, out stride =%d \n", ptState->dwSecond,ptState->dwMilliSecond,ptState->dwMicroSecond, ptState->bTimeStampCal,ptState->dwInWidth,ptState->dwOutWidth,ptState->dwOutHeight, ptState->dwOutStride);
                    DBPRINT_L4_7("VideoCap_TamperDetect(): Video Capstate: fifofull = %d, NoSignal %d, CCIRErr = %d, device Num = %d, Ybuf = 0X%x Cbbuf = 0X%x Crbuf = 0X%x\n",ptState->bFifoFull, ptState->bNoSignal, ptState->bCCIRErr, ptState->dwDeviceNum, ptState->pbyYFrame, ptState->pbyCbFrame, ptState->pbyCrFrame);
                    DBPRINT_L4_8("VideoCap_TamperDetect(): Video Capstate: FocusHigh = 0X%x, focusLow = 0X%x AFCurrentStatus =0X%x\n RGain =0X%x, BGain =0X%x, RSum =0X%x, GSum =0X%x, BSum =0X%x\n",ptState->dwAFFocusValueHigh, ptState->dwAFFocusValueLow, ptState->dwAFCurrentStatus, ptState->dwAWBRedGain,ptState->dwAWBBlueGain, (DWORD)(ptState->qwAWBRedSum & LSB_32_MASK), (DWORD)(ptState->qwAWBGreenSum & LSB_32_MASK), (DWORD)(ptState->qwAWBBlueSum & LSB_32_MASK));
                    DBPRINT_L4_6("VideoCap_TamperDetect(): Video Capstate: AEShutter = %d, AEGain = %d, ZoomRatio = %d, AEStable = %d, CapH = %d  CapV = %d\n",ptState->dwAEShutter, ptState->dwAEGain, ptState->dwZoomRatio, ptState->bAEStable,ptState->dwCapH,ptState->dwCapV);
                    DBPRINT_L4_0("VideoCap_TamperDetect(): --------------- Printing Focus Buffer Value -------------- \n\t");
                    {
                        DWORD dwIndex=0;
                        for(dwIndex =0; dwIndex<(49*2); dwIndex=dwIndex+2)
                        {
                        		DBPRINT_L4_3("[%d] = 0X%x   0X%x\n\t",(dwIndex/2), pdwStatFocusBuf[dwIndex+1], pdwStatFocusBuf[dwIndex]);
                        }
                        DBPRINT_L4_0("VideoCap_TamperDetect(): --------------- Focus Buffer Value Printed-------------- \n\t");
                    }
			/* check if current Focus Value is reduced beyond threshold wrt Prev stored focus value */
			if( IsFocusValueReduced(ptState, dwOOF_PrevFrames_High[dwArrayIndex], dwOOF_PrevFrames_Low[dwArrayIndex], FOCUS_DIFF_THRES_HIGH, FOCUS_DIFF_THRES_LOW))
			{
				DBPRINT_L2_0("\n-----    Focus Value reduced beyond threshold:               Current   Vs    PREVIOUS      ---------\n\n");

				/* check if current Focus Value is reduced beyond threshold wrt Prev stored Average focus value */
				if( IsFocusValueReduced(ptState, dwOOF_PrevFramesAvg_High, dwOOF_PrevFramesAvg_Low, FOCUS_DIFF_THRES_AVG_HIGH, FOCUS_DIFF_THRES_AVG_LOW ))
				{
					DBPRINT_L1_0("\n-----    Focus Value reduced beyond threshold:               Current   Vs   AVERAGE      ---------\n\n");
                                   /*Check if scene has changed based on information from R/G/B Sum */
					if( IsSceneChanged(ptState,qwTD_R_Avg, R_SUM_THRESHOLD, qwTD_G_Avg, G_SUM_THRESHOLD, qwTD_B_Avg, B_SUM_THRESHOLD ) == TRUE)
					{
                                        *pbIsTamperDetect =TRUE;
                                        DBPRINT_L4_0("-------------------    SCENE  Change   D E T E C T E D  !!! -------------------\n");
					}
					else
					{
                                        *pbIsTamperDetect =FALSE;					
                                        DBPRINT_L2_0("-------------------    SCENE  Change   N O T  Detected!!! -------------------\n");
					}			
				}
			}

			/*---------------------- Updates for next iterations		----------------------*/

			/* Update Current Focus Value to Array for next iterations of average calculations *//* This will be done twice when array is not completely full*/
			dwOOF_PrevFrames_High[dwArrayIndex] 	= ptState->dwAFFocusValueHigh;
			dwOOF_PrevFrames_Low[dwArrayIndex] 	= ptState->dwAFFocusValueLow;

			qwTD_R_Sum[dwArrayIndex] = ptState->qwAWBRedSum;
			qwTD_G_Sum[dwArrayIndex] = ptState->qwAWBGreenSum;
			qwTD_B_Sum[dwArrayIndex] = ptState->qwAWBBlueSum;

			/* Increment Array Index for next iteration*/
			dwArrayIndex 			= (dwArrayIndex+1) % NO_OF_FRAMES; // Since its a circular array index, indexed from 0

			/* Increment FrameCount */
			dwFrameNum++;

			/*------------------------Release Video Cap Buf ----------------------------*/
			if (VideoCap_ReleaseBuf(hObject, ptState) != S_OK) 
			{
				DBPRINT_L1_0("video_cap_release_buf err!\n");
			}
			else
			{
				DBPRINT_L3_0("VideoCap_TamperDetect(): ----------------------------   Released Video capture Buffer successfully ....-----------------------------------\n\n");
			}
		}
		else
		{
			//Skip Tamper Detection calculation for this frame
			dwFrameNum++;
		}

		DBPRINT_L4_0("VideoCap_TamperDetect(): ----------------------------   Exiting....-----------------------------------\n\n");

		return scStatus;
}



BOOL
IsFocusValueReduced(
							TVideoCapState *ptState, 
							DWORD dwOOF_Prev_High,
							DWORD dwOOF_Prev_Low, 
							DWORD dwFocus_Diff_Thres_High,
							DWORD dwFocus_Diff_Thres_Low
							)
{
        BOOL 	bIsFocusValueReduced =FALSE;
        QWORD qwOOF_Prev, qwFocus_Diff_Thres, qwAFFocusValue;

        DBPRINT_L4_0("IsFocusValueReduced(): Entered\n");

        DWORD_2_QWORD(qwOOF_Prev,              dwOOF_Prev_High,                  dwOOF_Prev_Low)
        DWORD_2_QWORD(qwFocus_Diff_Thres,    dwFocus_Diff_Thres_High,     dwFocus_Diff_Thres_Low)
        DWORD_2_QWORD(qwAFFocusValue,         ptState->dwAFFocusValueHigh,  ptState->dwAFFocusValueLow)

        if( qwOOF_Prev > qwFocus_Diff_Thres) // make sure below subtraction does not give negative result
        {
            if(qwAFFocusValue < (qwOOF_Prev -qwFocus_Diff_Thres) ) // subtracting here to avoid an MSB overflow during addition
            {
                bIsFocusValueReduced = TRUE;
                DBPRINT_L2_3("IsFocusValueReduced: dwOOF_Prev_Low =0X%x     ptState->dwAFFocusValueLow = 0X%x    dwFocus_Diff_Thres_Low = 0X%x    ", dwOOF_Prev_Low, ptState->dwAFFocusValueLow, dwFocus_Diff_Thres_Low);
                DBPRINT_L2_1("Low has reduced beyond threshold    bIsFocusValueReduced =%d\n",bIsFocusValueReduced);
            }
            else //Focus Value has not reduced beyond LOW threshold
            {
                bIsFocusValueReduced  = FALSE;
                DBPRINT_L3_1("Not reduced beyond threshold    bIsFocusValueReduced =%d\n",bIsFocusValueReduced);
        	  DBPRINT_L3_6("IsFocusValueReduced(): dwOOF_Prev_High Low=0X%x  0X%x,   dwFocus_Diff_Thres_High Low = 0X%x  0X%x   ptState->dwAFFocusValueHigh Low = 0X%x  0X%x\n",dwOOF_Prev_High, dwOOF_Prev_Low, dwFocus_Diff_Thres_High, dwFocus_Diff_Thres_Low,ptState->dwAFFocusValueHigh, ptState->dwAFFocusValueLow);
            }
        }
        else
        {
            // Since qwOOF_Prev is smalleer than threshold, the focus value has obviously not reduced beyond the threshold values.
            bIsFocusValueReduced  = FALSE;
            DBPRINT_L2_3("qwOOF_Prev is smaller than threshold,  dwOOF_Prev_Low = 0X%x dwFocus_Diff_Thres_Low =  0X%x  bIsFocusValueReduced =%d\n",dwOOF_Prev_Low , dwFocus_Diff_Thres_Low, bIsFocusValueReduced);
        }

	DBPRINT_L4_0("IsFocusValueReduced(): Exiting\n");

	return bIsFocusValueReduced ;
}


BOOL IsSceneChanged(
						TVideoCapState *ptState, 
						QWORD qw_R_Avg ,
						QWORD qw_R_Thres ,
						QWORD qw_G_Avg ,
						QWORD qw_G_Thres ,
						QWORD qw_B_Avg ,
						QWORD qw_B_Thres
						)
{
	BOOL    bIsSceneChanged =   FALSE;
	BOOL    bIs_R_Changed =   FALSE;
       BOOL    bIs_G_Changed=   FALSE;
       BOOL    bIs_B_Changed =   FALSE;

#ifdef _DEBUG_L1
	DWORD    dw_R_Avg_H, dw_R_Avg_L, dw_R_Thres_H, dw_R_Thres_L;
       DWORD    dw_G_Avg_H, dw_G_Avg_L, dw_G_Thres_H, dw_G_Thres_L;
       DWORD    dw_B_Avg_H,dw_B_Avg_L,dw_B_Thres_H,dw_B_Thres_L;
       DWORD    dw_R_Sum_H,dw_R_Sum_L;
       DWORD    dw_G_Sum_H,dw_G_Sum_L;
       DWORD    dw_B_Sum_H,dw_B_Sum_L;

      QWORD_2_DWORD(qw_R_Avg, dw_R_Avg_H , dw_R_Avg_L);
      QWORD_2_DWORD(qw_G_Avg, dw_G_Avg_H , dw_G_Avg_L);
      QWORD_2_DWORD(qw_B_Avg, dw_B_Avg_H , dw_B_Avg_L);


      QWORD_2_DWORD(qw_R_Thres, dw_R_Thres_H , dw_R_Thres_L);
      QWORD_2_DWORD(qw_G_Thres, dw_G_Thres_H , dw_G_Thres_L);
      QWORD_2_DWORD(qw_B_Thres, dw_B_Thres_H , dw_B_Thres_L);
       
      QWORD_2_DWORD(ptState->qwAWBRedSum, dw_R_Sum_H , dw_R_Sum_L);
      QWORD_2_DWORD(ptState->qwAWBGreenSum, dw_G_Sum_H , dw_G_Sum_L);
      QWORD_2_DWORD(ptState->qwAWBBlueSum, dw_B_Sum_H , dw_B_Sum_L);

#endif

	DBPRINT_L4_0("IsSceneChanged(): Entered\n");

	if ( qw_R_Avg  > (ptState->qwAWBRedSum + qw_R_Thres )  )   /* Focus Value High has reduced beyond the High threshold. No need to check the Low value*/
	{
		bIs_R_Changed  = TRUE;
		DBPRINT_L2_0("Red Sum has reduced beyond threshold\n");
	}

	if ( qw_G_Avg  > (ptState->qwAWBGreenSum + qw_G_Thres )  )   /* Focus Value High has reduced beyond the High threshold. No need to check the Low value*/
	{
		bIs_G_Changed  = TRUE;
              DBPRINT_L2_0("Green Sum has reduced beyond threshold  \n");
	}

	if ( qw_B_Avg  > (ptState->qwAWBBlueSum + qw_B_Thres )  )   /* Focus Value High has reduced beyond the High threshold. No need to check the Low value*/
	{
		bIs_B_Changed  = TRUE;
              DBPRINT_L2_0("Blue Sum has reduced beyond threshold \n");
	}

        /* A sample and simple logic for scene changed is introduced below. Feel free to add your own logic here. */
        if (  (bIs_R_Changed ==TRUE) && (bIs_G_Changed ==TRUE) && (bIs_B_Changed ==TRUE) )
        {
            bIsSceneChanged = TRUE;
            DBPRINT_L2_18("IsSceneChanged(): R_Avg : 0X%x 0X%x  R_Thres : 0X%x 0X%x  G_Avg : 0X%x 0X%x  G_Thres : 0X%x 0X%x  B_Avg : 0X%x 0X%x  B_Thres : 0X%x 0X%x  R_Sum : 0X%x 0X%x  G_Sum : 0X%x 0X%x  B_Sum : 0X%x 0X%x  \n",dw_R_Avg_H,  dw_R_Avg_L,  dw_R_Thres_H,  dw_R_Thres_L,dw_G_Avg_H,  dw_G_Avg_L,  dw_G_Thres_H, dw_G_Thres_L,dw_B_Avg_H,  dw_B_Avg_L,  dw_B_Thres_H,  dw_B_Thres_L,dw_R_Sum_H, dw_R_Sum_L, dw_G_Sum_H,   dw_G_Sum_L, dw_B_Sum_H, dw_B_Sum_L );
            DBPRINT_L4_0("----------------  Scene has changed ------------------ \n");
        }

	DBPRINT_L4_0("IsSceneChanged(): Exiting\n");

	return bIsSceneChanged ;

}

