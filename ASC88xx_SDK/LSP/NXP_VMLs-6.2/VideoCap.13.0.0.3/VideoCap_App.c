/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_App/VideoCap_App.c 4     13/12/09 6:44p Joe.tu $
 *
 * Copyright 2008 ______, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: VideoCap_App.c $
 * 
 * *****************  Version 4  *****************
 * User: Joe.tu       Date: 13/12/09   Time: 6:44p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_App
 * 
 * *****************  Version 3  *****************
 * User: Joe.tu       Date: 12/09/03   Time: 5:03p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_App
 * 
 * *****************  Version 2  *****************
 * User: Joe.tu       Date: 12/08/07   Time: 11:37a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_App
 * 
 * *****************  Version 1  *****************
 * User: Ycchang      Date: 12/06/26   Time: 2:01p
 * Created in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_App
 * 
 * *****************  Version 6  *****************
 * User: Joe.tu       Date: 11/07/18   Time: 3:34p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 5  *****************
 * User: Ycchang      Date: 11/06/21   Time: 7:06p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 4  *****************
 * User: Evelyn       Date: 11/01/13   Time: 6:09p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 93  *****************
 * User: Ycchang      Date: 10/09/09   Time: 6:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 92  *****************
 * User: Evelyn       Date: 10/08/13   Time: 3:09p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * Release at 2010.08.13
 * 1. FEATURE: Support auto-detect NTSC/PAL standard - DONE.
 * 
 * *****************  Version 91  *****************
 * User: Evelyn       Date: 10/06/04   Time: 2:42p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * Release at 2010.06.04
 * 1. MODIFICATION: Refine VideoCap_App.c - DONE.
 * 2. MODIFICATION: forget to add __ASM_ARCH_PLATFORM_MOZART_H__ - DONE.
 * 
 * *****************  Version 89  *****************
 * User: Alan         Date: 10/03/31   Time: 7:42p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * MODIFICATION: Add __USE_AF_LIB__ so as not to include related codes for
 * AutoFocus library - DONE.
 * 
 * *****************  Version 88  *****************
 * User: Ycchang      Date: 10/03/05   Time: 5:20p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 87  *****************
 * User: Ycchang      Date: 10/01/12   Time: 6:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 86  *****************
 * User: Ycchang      Date: 10/01/10   Time: 12:41a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * 
 * *****************  Version 84  *****************
 * User: Alan         Date: 09/12/04   Time: 9:11p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * MODIFICATION: Add OV2710 support - DONE.
 * MODIFICATION: Use the relative path for the shutter and gain tables -
 * DONE.
 *
 * *****************  Version 83  *****************
 * User: Ycchang      Date: 09/11/24   Time: 4:23p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 82  *****************
 * User: Rikchang     Date: 09/11/09   Time: 5:11p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 81  *****************
 * User: Alan         Date: 09/11/09   Time: 1:59a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 80  *****************
 * User: Aniki        Date: 09/11/05   Time: 11:20a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 79  *****************
 * User: Aniki        Date: 09/10/20   Time: 11:21a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * Add HAYDN and MOZART platform macro definition to
 * AutoExposure_Release() and AutoFocus_Release() - DONE.
 *
 * *****************  Version 78  *****************
 * User: Aniki        Date: 09/10/20   Time: 11:10a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 77  *****************
 * User: Aniki        Date: 09/10/16   Time: 10:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 76  *****************
 * User: Ronald       Date: 09/10/16   Time: 4:17p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 75  *****************
 * User: Alan         Date: 09/10/11   Time: 9:18p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 74  *****************
 * User: Alan         Date: 09/10/08   Time: 3:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * MODIFICATION: The HW luminance is changed from DWORD to QWORD and the
 * App does not need to combine high 2 bits and low 32 bits of the HW
 * luminance - DONE.
 * MODIFICATION: In MOZART, the average lumiance is sent to AE library and
 * the total pixel is set to 1 - DONE.
 *
 * *****************  Version 73  *****************
 * User: Alan         Date: 09/10/08   Time: 10:49a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * MODIFICATION: Add MN34030 support - DONE.
 *
 * *****************  Version 72  *****************
 * User: Ycchang      Date: 09/10/07   Time: 1:55p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 71  *****************
 * User: Aniki        Date: 09/10/05   Time: 10:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 70  *****************
 * User: Ycchang      Date: 09/10/02   Time: 11:09a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 69  *****************
 * User: Ronald       Date: 09/09/28   Time: 7:50p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 68  *****************
 * User: Ronald       Date: 09/09/28   Time: 4:17p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 67  *****************
 * User: Ycchang      Date: 09/06/09   Time: 8:41a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 66  *****************
 * User: Ycchang      Date: 09/05/23   Time: 4:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 65  *****************
 * User: Aniki        Date: 09/05/22   Time: 6:08p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 64  *****************
 * User: Aniki        Date: 09/05/22   Time: 6:02p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 63  *****************
 * User: Ycchang      Date: 09/05/05   Time: 4:15p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 62  *****************
 * User: Ronald       Date: 09/04/21   Time: 3:42p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 61  *****************
 * User: Ycchang      Date: 09/04/15   Time: 4:41p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 60  *****************
 * User: Albert.shen  Date: 09/04/14   Time: 8:44p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 59  *****************
 * User: Albert.shen  Date: 09/04/13   Time: 8:47p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 58  *****************
 * User: Albert.shen  Date: 09/04/13   Time: 5:15p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 57  *****************
 * User: Ronald       Date: 09/04/13   Time: 3:40p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 56  *****************
 * User: Aniki        Date: 09/04/06   Time: 2:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 55  *****************
 * User: Aniki        Date: 09/03/16   Time: 12:03p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 54  *****************
 * User: Aniki        Date: 09/03/16   Time: 12:00p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 53  *****************
 * User: Ronald       Date: 09/03/11   Time: 5:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 52  *****************
 * User: Albert.shen  Date: 09/02/25   Time: 10:06a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 51  *****************
 * User: Albert.shen  Date: 09/02/23   Time: 6:17p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 50  *****************
 * User: Aniki        Date: 09/02/10   Time: 6:15p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 49  *****************
 * User: Aniki        Date: 09/02/03   Time: 9:49p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 48  *****************
 * User: Albert.shen  Date: 09/01/20   Time: 6:12p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 47  *****************
 * User: Albert.shen  Date: 09/01/16   Time: 6:58p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 46  *****************
 * User: Aniki        Date: 08/11/21   Time: 6:59p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 45  *****************
 * User: Aniki        Date: 08/11/18   Time: 11:02a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 44  *****************
 * User: Aniki        Date: 08/11/06   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 43  *****************
 * User: Aniki        Date: 08/10/23   Time: 5:54p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 42  *****************
 * User: Ycchang      Date: 08/10/16   Time: 2:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 41  *****************
 * User: Rikchang     Date: 08/10/06   Time: 2:04p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 40  *****************
 * User: Ycchang      Date: 08/10/06   Time: 8:10a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 39  *****************
 * User: Ycchang      Date: 08/10/03   Time: 11:06a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 38  *****************
 * User: Ycchang      Date: 08/09/30   Time: 5:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 37  *****************
 * User: Albert.shen  Date: 08/09/25   Time: 5:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 36  *****************
 * User: Ycchang      Date: 08/09/23   Time: 11:06a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 35  *****************
 * User: Aniki        Date: 08/09/19   Time: 10:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 34  *****************
 * User: Ycchang      Date: 08/07/15   Time: 1:23p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 33  *****************
 * User: Jordan.cherng Date: 08/06/05   Time: 10:10a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 32  *****************
 * User: Jordan.cherng Date: 08/04/30   Time: 5:30p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 30  *****************
 * User: Alan         Date: 08/03/04   Time: 9:20p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 29  *****************
 * User: Alan         Date: 08/02/22   Time: 11:02p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 28  *****************
 * User: Alan         Date: 08/02/04   Time: 3:59p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 27  *****************
 * User: Alan         Date: 08/01/31   Time: 9:57p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 26  *****************
 * User: Alan         Date: 07/12/18   Time: 7:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 25  *****************
 * User: Ycchang      Date: 07/12/18   Time: 5:34p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 24  *****************
 * User: Ycchang      Date: 07/12/13   Time: 3:51p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 23  *****************
 * User: Alan         Date: 07/12/06   Time: 11:41a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 22  *****************
 * User: Ycchang      Date: 07/11/29   Time: 9:07a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 21  *****************
 * User: Ycchang      Date: 07/11/25   Time: 3:10p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 20  *****************
 * User: Alan         Date: 07/11/23   Time: 2:09a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 19  *****************
 * User: Ycchang      Date: 07/11/19   Time: 1:20p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 18  *****************
 * User: Ycchang      Date: 07/10/25   Time: 5:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 17  *****************
 * User: Ycchang      Date: 07/10/25   Time: 3:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 16  *****************
 * User: Ycchang      Date: 07/10/18   Time: 6:55p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 15  *****************
 * User: Ycchang      Date: 07/10/16   Time: 5:51p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 14  *****************
 * User: Ycchang      Date: 07/10/12   Time: 6:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 13  *****************
 * User: Ycchang      Date: 07/10/09   Time: 9:21a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 12  *****************
 * User: Ycchang      Date: 07/10/03   Time: 10:55a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 11  *****************
 * User: Ronald       Date: 07/10/03   Time: 10:30a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 9  *****************
 * User: Ycchang      Date: 07/09/19   Time: 10:12a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 8  *****************
 * User: Ycchang      Date: 07/09/17   Time: 9:23a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 7  *****************
 * User: Ycchang      Date: 07/09/07   Time: 6:46p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 6  *****************
 * User: Alan         Date: 06/08/16   Time: 7:01p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 5  *****************
 * User: Alan         Date: 06/07/11   Time: 2:03p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 * FEATURE: Synchronize with MemMgr_Lib version 4.0.0.0 - DONE.
 *
 * *****************  Version 4  *****************
 * User: Alan         Date: 06/07/03   Time: 1:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 3  *****************
 * User: Alan         Date: 06/07/02   Time: 5:54p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 2  *****************
 * User: Alan         Date: 06/06/22   Time: 12:18a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 * *****************  Version 1  *****************
 * User: Alan         Date: 05/11/29   Time: 5:40p
 * Created in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_App
 *
 */

/* ============================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "VideoCap.h"
#include "MemMgr.h"


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

/* ============================================================================================== */
int main(int argc, char* argv[])
{
	TVideoCapInitOptions tVideoCapInitOptions;
	TVideoCapState tVideoCapState;
	TVideoSignalOptions tVideoSignalOptions;
	TMemMgrInitOptions tMemMgrInitOptions;
	HANDLE hMemObject;
	TMemMgrState tMemMgrState;
	HANDLE hVideoCapObject;
	SCODE scStatus;
	BYTE *pbyCapBuf;

	BYTE *pbyStatAEWBBuf = NULL;
	BYTE *pbyStatHistoBuf = NULL;
	BYTE *pbyStatFocusBuf = NULL;

	DWORD dwDevNum, dwPrevFrameCount;
	BOOL bServer, bClient, bOutputEnable;
	DWORD dwFrameSize, dwYFrameSize, dwCFrameSize;
	DWORD dwFrameCount;
	DWORD i, j, k;
	BYTE *pbyBufferBase;
	DWORD dwDummy;

	FILE *pfInput;
	FILE *pfOutput;
	CHAR acTmpText[200];

	if (argc != 4)
	{
		printf("Usage: videocap config_file operation_mode output_enable\n");
		printf("operation_mode: 1 -> capture-only\n");
		printf("                2 -> retrieve-only\n");
		printf("                3 -> capture+retrieve\n");
		printf("output_enable: 0 -> disable output\n");
		printf("               1 -> enable output\n");
		exit(1);
	}

	bServer = FALSE;
	bClient = FALSE;
	if (atoi(argv[2]) == 1) {
		bServer = TRUE;
		printf("Capture-only mode\n");
	}
	else if (atoi(argv[2]) == 2) {
		bClient = TRUE;
		printf("Retrieve-only mode\n");
	}
	else {
		bServer = TRUE;
		bClient = TRUE;
		printf("Capture+Retrieve mode\n");
	}
	bOutputEnable = (atoi(argv[3])==1);

	if ((pfInput=fopen(argv[1], "r")) == NULL) {
		printf("Open input configuration file %s fail !!\n", argv[1]);
		exit(1);
	}
	dwDevNum = GetParam(pfInput);
	tMemMgrInitOptions.dwVersion = MEMMGR_VERSION;
	tMemMgrInitOptions.pObjectMem = (void *)NULL;
	if (MemMgr_Initial(&hMemObject, &tMemMgrInitOptions) != S_OK) {
		printf("Initial memory manager object fail !!\n");
		exit(1);
	}
	else {
		printf("MemMgr_Lib is initialized !!\n");
	}
	tVideoCapInitOptions.dwDeviceNum = dwDevNum;
	fscanf(pfInput, "%s", acTmpText);
	if (bOutputEnable==TRUE) {
		if ((pfOutput=fopen(acTmpText, "wb")) == NULL) {
			printf("Open video capture output data file %s fail !!\n", acTmpText);
			exit(1);
		}
	}
	fgets(acTmpText, 200, pfInput);
	fscanf(pfInput, "%s", acTmpText);
	fgets(acTmpText, 200, pfInput);
	dwFrameCount = GetParam(pfInput);
	tVideoCapInitOptions.dwVersion			= VIDEOCAP_VERSION;
	tVideoCapInitOptions.pObjectMem			= NULL;
	tVideoCapInitOptions.dwMaxFrameWidth	= GetParam(pfInput);	//used for capturing stride
	tVideoCapInitOptions.dwMaxFrameHeight	= GetParam(pfInput);
	tVideoCapInitOptions.dwCapWidth			= GetParam(pfInput);	//must be multiple of 16
	tVideoCapInitOptions.dwCapHeight		= GetParam(pfInput);	//must be multiple of 16
	tVideoCapInitOptions.dwStartPixel		= GetParam(pfInput);
	tVideoCapInitOptions.dwStartLine		= GetParam(pfInput);
	tVideoCapInitOptions.eOutPixelFormat 	= GetParam(pfInput);
	tVideoCapInitOptions.eFormat			= GetParam(pfInput);
	tVideoCapInitOptions.dwInWidth			= GetParam(pfInput);
	tVideoCapInitOptions.dwInHeight			= GetParam(pfInput);
	tVideoCapInitOptions.bFlip				= GetParam(pfInput);
	tVideoCapInitOptions.bMirror			= GetParam(pfInput);
	tVideoCapInitOptions.bFieldMode			= GetParam(pfInput);
	tVideoCapInitOptions.dwBrightness		= (DWORD)(GetParam(pfInput)&0x000000FF);
	tVideoCapInitOptions.dwContrast			= (DWORD)(GetParam(pfInput)&0x000000FF);
	tVideoCapInitOptions.dwSaturation		= GetParam(pfInput);
	tVideoCapInitOptions.bFieldInvert		= GetParam(pfInput);
	for (i=0; i<140; i++) {
		tVideoCapInitOptions.adwConfig[i] = GetParam(pfInput);
	}
	tVideoCapInitOptions.dwFrameRate		= GetParam(pfInput);
	tVideoCapInitOptions.dwBufNum			= 4;
	dwDummy                            = GetParam(pfInput);
	dwDummy							= GetParam(pfInput);

	fclose(pfInput);
	
	#ifdef __VideoServer__
		tVideoCapInitOptions.byChNum0 = 4;
		tVideoCapInitOptions.byChNum1 = 4;
		tVideoCapInitOptions.bChClkEdge0 = 0;
		tVideoCapInitOptions.bChClkEdge1 = 0;
		tVideoCapInitOptions.eChIDType0 = SYNC_ID;
		tVideoCapInitOptions.eChIDType1 = SYNC_ID;
	#else //__IPCam__
		tVideoCapInitOptions.byChNum0 = 1;
		tVideoCapInitOptions.byChNum1 = 1;
		tVideoCapInitOptions.bChClkEdge0 = 0;
		tVideoCapInitOptions.bChClkEdge1 = 0;
		tVideoCapInitOptions.eChIDType0 = NO_CH_ID;
		tVideoCapInitOptions.eChIDType1 = NO_CH_ID;
	#endif //#ifdef __VideoServer__

	dwYFrameSize = tVideoCapInitOptions.dwMaxFrameWidth * tVideoCapInitOptions.dwMaxFrameHeight;
	dwCFrameSize = (tVideoCapInitOptions.eOutPixelFormat==YUV420) ? dwYFrameSize/4 : dwYFrameSize/2;
	dwFrameSize = dwYFrameSize + (2*dwCFrameSize);

	if (bServer == TRUE) {
		printf("Output memory allocation\n");
		tMemMgrState.dwBusNum = VIDEOCAP_OUT_BUS_NUM;
		tMemMgrState.dwSize = dwFrameSize*4;
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
		if ((pbyCapBuf=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL) {
			printf("Allocate output frame buffer %d fail !!\n", (int)(dwFrameSize*4));
			exit(1);
		}
		else {
			printf("Request %d bytes non-cached memory succeed !!\n", (int)(dwFrameSize*4));
		}
		tVideoCapInitOptions.pbyCapBufUsrBaseAddr = pbyCapBuf;

		tMemMgrState.dwBusNum = VIDEOCAP_OUT_BUS_NUM;
		tMemMgrState.dwSize = 720*sizeof(DWORD);
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
		if ((pbyStatAEWBBuf=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL) {
			printf("Allocate AEWB statistic buffer %d fail !!\n", (int)tMemMgrState.dwSize);
			exit(1);
		}
		else {
			printf("Request %d bytes non-cached memory succeed !!\n", (int)tMemMgrState.dwSize);
		}
		tVideoCapInitOptions.pbyStatAEWBBufUsrBaseAddr = pbyStatAEWBBuf;

		memset(pbyStatAEWBBuf, 0, 720*sizeof(DWORD));

		tMemMgrState.dwBusNum = VIDEOCAP_OUT_BUS_NUM;
		tMemMgrState.dwSize = 192*sizeof(DWORD);
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
		if ((pbyStatHistoBuf=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL) {
			printf("Allocate histogram statistic buffer %d fail !!\n", (int)tMemMgrState.dwSize);
			exit(1);
		}
		else {
			printf("Request %d bytes non-cached memory succeed !!\n", (int)tMemMgrState.dwSize);
		}
		tVideoCapInitOptions.pbyStatHistoBufUsrBaseAddr = pbyStatHistoBuf;

		memset(pbyStatHistoBuf, 0, 192*sizeof(DWORD));

		tMemMgrState.dwBusNum = VIDEOCAP_OUT_BUS_NUM;
		tMemMgrState.dwSize = 98*sizeof(DWORD);
		tMemMgrState.eAlignType = ALIGN_TYPE_128_BYTE;
		if ((pbyStatFocusBuf=(BYTE *)MemMgr_GetMemory(hMemObject, &tMemMgrState)) == NULL) {
			printf("Allocate output frame buffer %d fail !!\n", (int)tMemMgrState.dwSize);
			exit(1);
		}
		else {
			printf("Request %d bytes non-cached memory succeed !!\n", (int)tMemMgrState.dwSize);
		}
		tVideoCapInitOptions.pbyStatFocusBufUsrBaseAddr = pbyStatFocusBuf;

		memset(pbyStatFocusBuf, 0, 98*sizeof(DWORD));

		for (i=0; i<4; i++) {
			// initialized to black background
			memset(pbyCapBuf, 0, dwYFrameSize);
			pbyCapBuf += dwYFrameSize;
			memset(pbyCapBuf, 0x80, dwCFrameSize);
			pbyCapBuf += dwCFrameSize;
			memset(pbyCapBuf, 0x80, dwCFrameSize);
			pbyCapBuf += dwCFrameSize;
		}
	}
	else {
		tVideoCapInitOptions.pbyCapBufUsrBaseAddr = NULL;
		tVideoCapInitOptions.pbyStatAEWBBufUsrBaseAddr = NULL;
		tVideoCapInitOptions.pbyStatHistoBufUsrBaseAddr = NULL;
		tVideoCapInitOptions.pbyStatFocusBufUsrBaseAddr = NULL;
	}
	printf("VideoCap is going to be initialized !!\n");
	if (VideoCap_Initial(&hVideoCapObject, &tVideoCapInitOptions) != S_OK) {
		printf("VideoCap Object Initialization failed !!\n");
		exit(1);
	}
	else {
		printf("VideoCap object initialization succeeded !!\n");
	}
	if (bServer == TRUE) {
		VideoCap_Start(hVideoCapObject);
		printf("Start VIC !!\n");
	}
	if (bServer == TRUE)
	{
		for (i=0; i<20; i++)
		{
			if(i == 0)
			{
				tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_TARGET_LUMINANCE;
				tVideoSignalOptions.adwUserData[0] = tVideoCapInitOptions.adwConfig[59];
				tVideoSignalOptions.adwUserData[1] = 10;
				if (VideoCap_SetOptions(hVideoCapObject, &tVideoSignalOptions) != S_OK)
				{
					printf("TARGET_LUMINANCE VideoCap_SetOptions error!\n");
					goto EXIT;
				}
				else
				{
					printf("VideoCap_SetOptions AETarget : %d AEOffset : %d !\n", (int)tVideoCapInitOptions.adwConfig[59], 10);
				}

				tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_SHUTTER;
				tVideoSignalOptions.adwUserData[0] = tVideoCapInitOptions.adwConfig[60];
				if (VideoCap_SetOptions(hVideoCapObject, &tVideoSignalOptions) != S_OK)
				{
					printf("MIN_SHUTTER VideoCap_SetOptions error!\n");
					goto EXIT;
				}
				else
				{
					printf("VideoCap_SetOptions dwMinShutter : %d !\n", (int)tVideoCapInitOptions.adwConfig[60]);
				}

				tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_SHUTTER;
				tVideoSignalOptions.adwUserData[0] = tVideoCapInitOptions.adwConfig[61];
				if (VideoCap_SetOptions(hVideoCapObject, &tVideoSignalOptions) != S_OK)
				{
					printf("MAX_SHUTTER VideoCap_SetOptions error!\n");
					goto EXIT;
				}
				else
				{
					printf("VideoCap_SetOptions dwMaxShutter : %d !\n", (int)tVideoCapInitOptions.adwConfig[61]);
				}

				tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MIN_GAIN;
				tVideoSignalOptions.adwUserData[0] = tVideoCapInitOptions.adwConfig[62]*1000;
				if (VideoCap_SetOptions(hVideoCapObject, &tVideoSignalOptions) != S_OK)
				{
					printf("MIN_GAIN VideoCap_SetOptions error!\n");
					goto EXIT;
				}
				else
				{
					printf("VideoCap_SetOptions dwMinGain : %d !\n", (int)tVideoCapInitOptions.adwConfig[62]);
				}

				tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MAX_GAIN;
				tVideoSignalOptions.adwUserData[0] = tVideoCapInitOptions.adwConfig[63]*1000;
				if (VideoCap_SetOptions(hVideoCapObject, &tVideoSignalOptions) != S_OK)
				{
					printf("MAX_GAIN VideoCap_SetOptions error!\n");
					goto EXIT;
				}
				else
				{
					printf("VideoCap_SetOptions dwMaxGain : %d !\n", (int)tVideoCapInitOptions.adwConfig[63]);
				}
			}
			VideoCap_Sleep(hVideoCapObject);
			VideoCap_GetBuf(hVideoCapObject, &tVideoCapState);

			if (VideoCap_ReleaseBuf(hVideoCapObject, &tVideoCapState) != S_OK) {
				printf("video_cap_release_buf err!\n");
			}
		}
	}
	
	dwPrevFrameCount = 0xFFFFFFFF;
	for (i=0; i<dwFrameCount; i++) {
//	for (i=0; ; i++) {
#if 0
		if (bServer == TRUE) {
			TAutoDetectStdInfo tStdInfo;

			// detect NTSC/PAL standard
			tVideoSignalOptions.eOptionFlags = VIDEO_SIGNAL_OPTION_AUTO_DETECT_STD;
			tVideoSignalOptions.adwUserData[0] = (DWORD)(&tStdInfo);
			if (VideoCap_SetOptions(hVideoCapObject, &tVideoSignalOptions) != S_OK)
			{
				printf("VideoCap_SetOptions error!\n");
				goto EXIT;
			}
			// setup standard
			if (tStdInfo.bDataLost == FALSE) {
				if (tStdInfo.eDetectStd == TVSTD_NTSC) {
					printf("detect NTSC\n");
				}
				else if (tStdInfo.eDetectStd == TVSTD_PAL) {
					printf("detect PAL\n");
				}
				else {
					printf("detect unknown standard\n");
				}				
			}
			else {	// (tStdInfo.bDataLost == TRUE) || (tStdInfo.eDetectStd != TVSTD_UNKNOWN)
				printf("data loss\n");
			}
		}
#endif
		VideoCap_Sleep(hVideoCapObject);
		scStatus = VideoCap_GetBuf(hVideoCapObject, &tVideoCapState);
		if ((scStatus!=S_OK) || (tVideoCapState.dwFrameCount==dwPrevFrameCount)) {
			if (tVideoCapState.bFifoFull == TRUE) {
				if (scStatus == S_OK) {
					printf("FrameCount = (%d) Fifo Full (error frame) with S_OK, dwInWidth: %d !!\n", (int)i, (int)tVideoCapState.dwInWidth);
				}
				else {
					printf("First FrameCount = (%d) Fifo Full (error frame) with S_FAIL, dwInWidth: %d !!\n", (int)i, (int)tVideoCapState.dwInWidth);
				}
			}
			if (tVideoCapState.bNoSignal == TRUE) {
				printf("No signal1 !!\n");
			}
			while ((scStatus!=S_OK) || (tVideoCapState.dwFrameCount==dwPrevFrameCount)) {
				if (scStatus != S_OK) {
					printf("Failed to get correct buffer after sleep!\n");
				}
				else { // (scStatus==S_OK)				
					if (VideoCap_ReleaseBuf(hVideoCapObject, &tVideoCapState) != S_OK) {
						printf("video_cap_release_buf err!\n");
					}
				}
				printf("before VideoCap_Sleep\n");
				VideoCap_Sleep(hVideoCapObject);
				scStatus = VideoCap_GetBuf(hVideoCapObject, &tVideoCapState);
				if (tVideoCapState.bFifoFull == TRUE) {
					if (scStatus == S_OK) {
						printf("FrameCount = (%d) Fifo Full (error frame) with S_OK, dwInWidth: %d !!\n", (int)i, (int)tVideoCapState.dwInWidth);
					}
					else {
						printf("FrameCount = (%d) Fifo Full (error frame) with S_FAIL, dwInWidth: %d !!\n", (int)i, (int)tVideoCapState.dwInWidth);
					}
				}
				if (tVideoCapState.bNoSignal == TRUE) {
					printf("No signal2 !!\n");
				}
			}
		}

		// To reach here, scStatus must be S_OK and a new frame is captured.
		if (bOutputEnable == TRUE) {

			MemMgr_CacheInvalidate(hMemObject, (DWORD)tVideoCapState.pbyYFrame, dwYFrameSize*sizeof(BYTE));
			MemMgr_CacheInvalidate(hMemObject, (DWORD)tVideoCapState.pbyCbFrame, dwCFrameSize*sizeof(BYTE));
			MemMgr_CacheInvalidate(hMemObject, (DWORD)tVideoCapState.pbyCrFrame, dwCFrameSize*sizeof(BYTE));
			printf("dwOutHeight:%d, dwOutStride:%d, dwOutWidth:%d\n", (int)tVideoCapState.dwOutHeight, (int)tVideoCapState.dwOutStride, (int)tVideoCapState.dwOutWidth);
			pbyBufferBase = tVideoCapState.pbyYFrame;
			for (j=0; j<tVideoCapInitOptions.dwMaxFrameHeight; j++) {
				for (k=0; k<tVideoCapState.dwOutStride; k++) {
					if (k < tVideoCapState.dwOutWidth) {
						fputc(*pbyBufferBase, pfOutput);
					}
					else {
						fputc(0x00, pfOutput);
					}
					pbyBufferBase++;
				}
			}
			pbyBufferBase = tVideoCapState.pbyCbFrame;
			for (j=0; j<(tVideoCapInitOptions.dwMaxFrameHeight>>(tVideoCapInitOptions.eOutPixelFormat==YUV420)); j++) {
				for (k=0; k<(tVideoCapState.dwOutStride>>1); k++) {
					if (k < (tVideoCapState.dwOutWidth>>1)) {
						fputc(*pbyBufferBase, pfOutput);
					}
					else {
						fputc(0x80, pfOutput);
					}
					pbyBufferBase++;
				}
			}
			pbyBufferBase = tVideoCapState.pbyCrFrame;
			for (j=0; j<(tVideoCapInitOptions.dwMaxFrameHeight>>(tVideoCapInitOptions.eOutPixelFormat==YUV420)); j++) {
				for (k=0; k<(tVideoCapState.dwOutStride>>1); k++) {
					if (k < (tVideoCapState.dwOutWidth>>1)) {
						fputc(*pbyBufferBase, pfOutput);
					}
					else {
						fputc(0x80, pfOutput);
					}
					pbyBufferBase++;
				}
			}
		}
		printf("FrameCount = (%d), %d, %d, Time = %d:%03d, size=%dx%d, dwInWidth: %d\n",
		(int)i,
		(int)tVideoCapState.dwFrameCount,
		(int)tVideoCapState.dwIndex,
		(int)tVideoCapState.dwSecond,
		(int)tVideoCapState.dwMilliSecond,
		(int)tVideoCapState.dwOutWidth,
		(int)tVideoCapState.dwOutHeight,
		(int)tVideoCapState.dwInWidth);

		dwPrevFrameCount = tVideoCapState.dwFrameCount;
		if (VideoCap_ReleaseBuf(hVideoCapObject, &tVideoCapState) != S_OK) {
			printf("video_cap_release_buf err!\n");
		}
	}
	
	if (bServer == TRUE) {
		VideoCap_Stop(hVideoCapObject);
		printf("Stop VIC !!\n");
	}
EXIT:
	if (VideoCap_Release(&hVideoCapObject) != S_OK) {
		printf("Release VideoCap object fail !!\n");
	}

	if (bOutputEnable==TRUE) {
		fclose(pfOutput);

		if (MemMgr_FreeMemory(hMemObject, (DWORD)(pbyCapBuf)) != S_OK) {
			printf("Release pbyCapBuf buffer fail !!\n");
		}
	}
	if (bServer == TRUE) {
		if (MemMgr_FreeMemory(hMemObject, (DWORD)(pbyStatAEWBBuf)) != S_OK) {
			printf("Release pbyStatAEWBBuf buffer fail !!\n");
		}
		if (MemMgr_FreeMemory(hMemObject, (DWORD)(pbyStatHistoBuf)) != S_OK) {
			printf("Release pbyStatHistoBuf buffer fail !!\n");
		}
		if (MemMgr_FreeMemory(hMemObject, (DWORD)(pbyStatFocusBuf)) != S_OK) {
			printf("Release pbyStatFocusBuf buffer fail !!\n");
		}

		if (MemMgr_Release(&hMemObject) != S_OK) {
			printf("Release MemMgr object fail !!\n");
			exit(1);
		}
	}
	return S_OK;
}
