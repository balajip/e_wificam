/*
 * $Header: /rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib/VideoCap.h 14    13/12/24 6:24p Joe.tu $
 *
 * Copyright 2008 ______, Inc.  All rights reserved.
 *
 * Description:
 *
 * $History: VideoCap.h $
 * 
 * *****************  Version 14  *****************
 * User: Joe.tu       Date: 13/12/24   Time: 6:24p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 13  *****************
 * User: Joe.tu       Date: 13/12/09   Time: 6:47p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 12  *****************
 * User: Joe.tu       Date: 13/08/23   Time: 9:47a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 11  *****************
 * User: Joe.tu       Date: 13/04/19   Time: 5:18p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 10  *****************
 * User: Joe.tu       Date: 13/03/06   Time: 6:25p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 9  *****************
 * User: Ycchang      Date: 13/02/02   Time: 10:11a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 8  *****************
 * User: Joe.tu       Date: 13/01/23   Time: 2:48p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 7  *****************
 * User: Joe.tu       Date: 13/01/16   Time: 4:42p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 6  *****************
 * User: Joe.tu       Date: 12/11/22   Time: 6:55p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 5  *****************
 * User: Joe.tu       Date: 12/11/02   Time: 4:40p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 4  *****************
 * User: Joe.tu       Date: 12/10/15   Time: 3:51p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 3  *****************
 * User: Joe.tu       Date: 12/08/07   Time: 11:39a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 2  *****************
 * User: Joe.tu       Date: 12/06/28   Time: 5:44p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 1  *****************
 * User: Ycchang      Date: 12/06/26   Time: 2:02p
 * Created in $/rd_2/project/Mozart/Linux_Libraries/VideoCap_3.0.0.0/VideoCap_Lib
 * 
 * *****************  Version 14  *****************
 * User: Joe.tu       Date: 11/11/03   Time: 9:56a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 13  *****************
 * User: Evelyn       Date: 11/10/25   Time: 5:55p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 12  *****************
 * User: Evelyn       Date: 11/10/21   Time: 4:20p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 11  *****************
 * User: Joe.tu       Date: 11/09/21   Time: 7:50p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 10  *****************
 * User: Joe.tu       Date: 11/08/29   Time: 7:05p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 9  *****************
 * User: Evelyn       Date: 11/08/11   Time: 4:53p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 8  *****************
 * User: Joe.tu       Date: 11/07/15   Time: 6:50p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 7  *****************
 * User: Ycchang      Date: 11/07/04   Time: 9:38a
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 6  *****************
 * User: Ycchang      Date: 11/06/22   Time: 6:50p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 5  *****************
 * User: Ycchang      Date: 11/06/21   Time: 7:06p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 4  *****************
 * User: Tassader.chen Date: 11/02/08   Time: 1:35p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 3  *****************
 * User: Evelyn       Date: 11/01/13   Time: 6:09p
 * Updated in $/rd_2/project/Mozart/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 117  *****************
 * User: Evelyn       Date: 11/01/12   Time: 3:49p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 116  *****************
 * User: Ycchang      Date: 10/12/16   Time: 3:53p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 115  *****************
 * User: Evelyn       Date: 10/12/15   Time: 12:11p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 114  *****************
 * User: Evelyn       Date: 10/12/07   Time: 3:40p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 113  *****************
 * User: Evelyn       Date: 10/11/17   Time: 4:23p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 112  *****************
 * User: Evelyn       Date: 10/11/02   Time: 3:24p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 111  *****************
 * User: Evelyn       Date: 10/09/09   Time: 9:09p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. FEATURE: Synchronize with VPL_VIC device driver version 7.0.0.4
 * 2. FEATURE: Synchronize with VPL_EDMC device driver version 5.0.0.14
 * 3. FEATURE: Synchronize with MemMgr_Lib version 4.0.0.6
 * 4. BUG: When setting contrast, the saturation should be zero if bMono
 * is TRUE - FIXED.
 * 
 * *****************  Version 110  *****************
 * User: Ycchang      Date: 10/09/09   Time: 6:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 109  *****************
 * User: Evelyn       Date: 10/08/18   Time: 12:40p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 108  *****************
 * User: Evelyn       Date: 10/08/13   Time: 3:09p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * Release at 2010.08.13
 * 1. FEATURE: Support auto-detect NTSC/PAL standard - DONE.
 * 
 * *****************  Version 107  *****************
 * User: Evelyn       Date: 10/06/04   Time: 2:42p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * Release at 2010.06.04
 * 1. MODIFICATION: Refine VideoCap_App.c - DONE.
 * 2. MODIFICATION: forget to add __ASM_ARCH_PLATFORM_MOZART_H__ - DONE.
 * 
 * *****************  Version 106  *****************
 * User: Ycchang      Date: 10/05/17   Time: 4:54p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 105  *****************
 * User: Ycchang      Date: 10/05/13   Time: 5:07p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 104  *****************
 * User: Evelyn       Date: 10/04/30   Time: 10:46p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 103  *****************
 * User: Alan         Date: 10/03/05   Time: 4:00p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update the version number from 7.3.0.3 to 7.3.0.4 - DONE.
 * 
 * *****************  Version 102  *****************
 * User: Ycchang      Date: 10/01/12   Time: 6:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 101  *****************
 * User: Ycchang      Date: 10/01/10   Time: 12:41a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 
 * *****************  Version 100  *****************
 * User: Alan         Date: 09/12/10   Time: 11:20a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update the version number from 7.3.0.0 to 7.3.0.1 - DONE.
 *
 * *****************  Version 99  *****************
 * User: Ycchang      Date: 09/11/24   Time: 4:23p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 98  *****************
 * User: Alan         Date: 09/11/23   Time: 1:26p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update the version number from 7.2.0.3 to 7.2.0.4 - DONE.
 *
 * *****************  Version 97  *****************
 * User: Alan         Date: 09/11/10   Time: 10:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update the version number from 7.2.0.2 to 7.2.0.3 - DONE.
 *
 * *****************  Version 96  *****************
 * User: Rikchang     Date: 09/11/09   Time: 5:11p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 95  *****************
 * User: Alan         Date: 09/11/09   Time: 2:11a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update the version number from 7.2.0.0 to 7.2.0.1 - DONE.
 *
 * *****************  Version 94  *****************
 * User: Aniki        Date: 09/11/04   Time: 10:38p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * MODIFICATION: Add VIDEO_SIGNAL_OPTION_SET_PHOTO_LDC_EN option - DONE.
 *
 * *****************  Version 93  *****************
 * User: Aniki        Date: 09/10/16   Time: 10:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 92  *****************
 * User: Ronald       Date: 09/10/16   Time: 4:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 91  *****************
 * User: Ycchang      Date: 09/10/14   Time: 10:38a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 90  *****************
 * User: Alan         Date: 09/10/08   Time: 4:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 89  *****************
 * User: Alan         Date: 09/10/08   Time: 3:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * MODIFICATION: Change dwAEWin[0-7]Luminance to qwAEWin[0-7]Luminance and
 * remove dwAEWinsMsbLuminance, dwAEStatus and dwAEPixel in MOZART SoC -
 * DONE.
 *
 * *****************  Version 88  *****************
 * User: Rikchang     Date: 09/10/07   Time: 4:58p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 87  *****************
 * User: Ycchang      Date: 09/10/07   Time: 10:33a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 86  *****************
 * User: Ronald       Date: 09/10/06   Time: 11:09a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * Change the start position of AE window's coordinate from 1 to 0
 *
 * *****************  Version 85  *****************
 * User: Aniki        Date: 09/10/05   Time: 10:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 84  *****************
 * User: Rikchang     Date: 09/09/29   Time: 4:47p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 83  *****************
 * User: Ronald       Date: 09/09/28   Time: 7:50p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 82  *****************
 * User: Alan         Date: 09/08/21   Time: 3:40p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 81  *****************
 * User: Ycchang      Date: 09/08/13   Time: 6:58p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 80  *****************
 * User: Albert.shen  Date: 09/07/28   Time: 3:30p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 79  *****************
 * User: Albert.shen  Date: 09/07/28   Time: 3:22p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 78  *****************
 * User: Albert.shen  Date: 09/06/23   Time: 2:40p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 77  *****************
 * User: Albert.shen  Date: 09/06/19   Time: 3:58p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 76  *****************
 * User: Albert.shen  Date: 09/06/09   Time: 9:23a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 75  *****************
 * User: Albert.shen  Date: 09/06/05   Time: 8:06p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 74  *****************
 * User: Ycchang      Date: 09/06/03   Time: 3:33p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 73  *****************
 * User: Rikchang     Date: 09/05/26   Time: 5:58p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 72  *****************
 * User: Ycchang      Date: 09/05/23   Time: 4:21p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 71  *****************
 * User: Rikchang     Date: 09/05/18   Time: 10:58a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 70  *****************
 * User: Ycchang      Date: 09/05/09   Time: 2:08p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 69  *****************
 * User: Albert.shen  Date: 09/05/05   Time: 9:12p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 68  *****************
 * User: Ycchang      Date: 09/05/05   Time: 4:15p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 67  *****************
 * User: Albert.shen  Date: 09/04/23   Time: 3:56p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 66  *****************
 * User: Albert.shen  Date: 09/04/21   Time: 4:33p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 65  *****************
 * User: Albert.shen  Date: 09/04/21   Time: 1:14p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 64  *****************
 * User: Ronald       Date: 09/04/15   Time: 10:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 63  *****************
 * User: Ronald       Date: 09/04/13   Time: 3:58p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 62  *****************
 * User: Albert.shen  Date: 09/03/17   Time: 9:28a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 61  *****************
 * User: Ronald       Date: 09/03/16   Time: 10:37a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 60  *****************
 * User: Ronald       Date: 09/03/11   Time: 11:47a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 59  *****************
 * User: Albert.shen  Date: 09/02/25   Time: 10:04a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 58  *****************
 * User: Albert.shen  Date: 09/02/23   Time: 6:02p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. Update the version from 5.6.0.2 to 5.7.0.0 - DONE.
 * 2. FEATURE: Add the option VIDEO_SIGNAL_OPTION_SET_GAMMA_TABLE, which
 * is allowed VIC to use a user-defined gamma curve - DONE.
 *
 * *****************  Version 57  *****************
 * User: Albert.shen  Date: 09/02/20   Time: 6:55p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. MODIFICATION: Modify MCI Step MMR - DONE.
 *
 * 2. MODIFICATION: Add profiles of VIC - DONE.
 *
 * *****************  Version 56  *****************
 * User: Albert.shen  Date: 09/02/13   Time: 2:56p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 55  *****************
 * User: Aniki        Date: 09/02/03   Time: 9:43p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 54  *****************
 * User: Ycchang      Date: 09/01/09   Time: 10:44a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 53  *****************
 * User: Ycchang      Date: 09/01/09   Time: 10:27a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 52  *****************
 * User: Albert.shen  Date: 08/12/29   Time: 2:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 51  *****************
 * User: Albert.shen  Date: 08/12/23   Time: 1:22p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 50  *****************
 * User: Albert.shen  Date: 08/12/05   Time: 9:25p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 49  *****************
 * User: Albert.shen  Date: 08/12/01   Time: 9:27p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. UPDATE: Update the version number from 5.3.0.0 to 5.3.0.1 - DONE
 *
 * *****************  Version 48  *****************
 * User: Albert.shen  Date: 08/11/27   Time: 10:38p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: update the version number from 5.2.0.5 to 5.3.0.0 - DONE
 *
 * *****************  Version 47  *****************
 * User: Albert.shen  Date: 08/11/27   Time: 9:39p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 46  *****************
 * User: Albert.shen  Date: 08/11/27   Time: 4:50p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. UPDATE: Update version number from 5.2.0.3 to 5.2.0.4
 * 2. FEATURE: Add VIDEO_SIGNAL_OPTION_SET_START_PIXEL option in
 * VideoCap_Initial() to support setting options with the external devices
 * - DONE.
 *
 * 3. MODIFICATION: Disable IICtrl using by VIC before setting options
 * with the external devices - DONE.
 *
 * 4. MODIFICATION: Enable IICtrl using by VIC after setting options with
 * the external devices - DONE.
 *
 * 5. MODIFICATION: Add video_cap_info member bEnSSP to check if the
 * external device has the option VIDEO_SIGNAL_OPTION_SET_START_PIXEL -
 * DONE.
 *
 * *****************  Version 45  *****************
 * User: Albert.shen  Date: 08/11/18   Time: 11:25a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. UPDATE: Update the version number from 5.2.0.2 to 5.2.0.3 - DONE
 * 2. MODIFICATION: Add microsecond field in TVideoCapState - DONE.
 *
 * *****************  Version 44  *****************
 * User: Aniki        Date: 08/11/18   Time: 10:59a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 43  *****************
 * User: Albert.shen  Date: 08/11/10   Time: 3:44p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * 1. MODIFICATION: Using system time with calibration as time stamp of
 * buffers. -DONE
 * 2. MODIFICATION: Add state member bTimeStampCal to see if driver is
 * calibrating for time stamp - DONE
 * 3. UPDATE: Update the version number from 5.2.0.0 to 5.2.0.1
 *
 * *****************  Version 42  *****************
 * User: Aniki        Date: 08/11/06   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 41  *****************
 * User: Albert.shen  Date: 08/10/23   Time: 6:01p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * MODIFICATION: Refine the gamma table to enhance the contrast. - DONE
 *
 * *****************  Version 40  *****************
 * User: Aniki        Date: 08/10/23   Time: 5:54p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 39  *****************
 * User: Ycchang      Date: 08/10/16   Time: 2:13p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 38  *****************
 * User: Rikchang     Date: 08/10/07   Time: 11:51a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 37  *****************
 * User: Ycchang      Date: 08/10/03   Time: 11:06a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 36  *****************
 * User: Ycchang      Date: 08/09/30   Time: 5:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 35  *****************
 * User: Ycchang      Date: 08/09/24   Time: 10:42a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 34  *****************
 * User: Ycchang      Date: 08/09/23   Time: 11:06a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 33  *****************
 * User: Aniki        Date: 08/09/19   Time: 10:16p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 32  *****************
 * User: Jordan.cherng Date: 08/06/05   Time: 10:10a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 31  *****************
 * User: Jordan.cherng Date: 08/04/30   Time: 5:30p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 29  *****************
 * User: Albert.shen  Date: 08/03/14   Time: 1:36p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 28  *****************
 * User: Alan         Date: 08/03/04   Time: 9:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 27  *****************
 * User: Alan         Date: 08/02/26   Time: 9:29p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 26  *****************
 * User: Alan         Date: 08/02/22   Time: 11:10p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 25  *****************
 * User: Alan         Date: 08/02/21   Time: 6:49p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 24  *****************
 * User: Alan         Date: 08/02/04   Time: 3:51p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 23  *****************
 * User: Alan         Date: 08/01/07   Time: 10:45p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 22  *****************
 * User: Ycchang      Date: 08/01/07   Time: 6:44p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 21  *****************
 * User: Ycchang      Date: 07/12/21   Time: 9:49a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 20  *****************
 * User: Alan         Date: 07/12/18   Time: 7:19p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 19  *****************
 * User: Ycchang      Date: 07/12/18   Time: 2:31p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 18  *****************
 * User: Ycchang      Date: 07/12/13   Time: 3:48p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 17  *****************
 * User: Ycchang      Date: 07/12/06   Time: 8:57p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 16  *****************
 * User: Ycchang      Date: 07/11/25   Time: 3:10p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 15  *****************
 * User: Alan         Date: 07/11/23   Time: 3:57a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 14  *****************
 * User: Ycchang      Date: 07/11/19   Time: 1:20p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 13  *****************
 * User: Ycchang      Date: 07/10/25   Time: 5:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 12  *****************
 * User: Ycchang      Date: 07/10/25   Time: 3:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 11  *****************
 * User: Ycchang      Date: 07/10/18   Time: 6:55p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 10  *****************
 * User: Ycchang      Date: 07/10/16   Time: 5:51p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 9  *****************
 * User: Ycchang      Date: 07/10/12   Time: 6:35p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 8  *****************
 * User: Ycchang      Date: 07/10/09   Time: 9:21a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 7  *****************
 * User: Ycchang      Date: 07/10/03   Time: 10:55a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 6  *****************
 * User: Ycchang      Date: 07/09/17   Time: 9:23a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 5  *****************
 * User: Ycchang      Date: 07/09/07   Time: 6:46p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 * *****************  Version 4  *****************
 * User: Alan         Date: 06/08/24   Time: 4:52p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update version number from 2.0.0.1 to 2.0.0.2 - DONE.
 *
 * *****************  Version 3  *****************
 * User: Alan         Date: 06/07/11   Time: 2:03p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * UPDATE: Update version number from 2.0.0.0 to 2.0.0.1 - DONE.
 *
 * *****************  Version 2  *****************
 * User: Alan         Date: 06/06/22   Time: 12:30a
 * Updated in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 * FEATURE: Synchronize with VPL_VIC device driver version 2.0.0.1 - DONE.
 * FEATURE: Synchronize with MemMgr_Lib version 3.3.0.0 - DONE.
 *
 * *****************  Version 1  *****************
 * User: Alan         Date: 05/11/29   Time: 5:40p
 * Created in $/rd_2/project/SoC/Linux_Libraries/VideoCap/VideoCap_Lib
 *
 */

/* =========================================================================================== */
#ifndef __VIDEOCAP_H__
#define __VIDEOCAP_H__

/* =========================================================================================== */
#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "errordef.h"
#include "global_codec.h"
#include "MemMgr.h"

/* =========================================================================================== */
#define VIDEOCAP_VERSION MAKEFOURCC(13, 0, 0, 3)
#define VIDEOCAP_ID_VERSION "13.0.0.3"
//#define __USE_PROFILE__

/* =========================================================================================== */
#define VIDEOCAP_IN_BUS_NUM	    0
#define VIDEOCAP_OUT_BUS_NUM	0

/* =========================================================================================== */
typedef SCODE (* FOnSetVideoSignalOptions)(HANDLE hObject, TVideoSignalOptions *ptOptions);

/* ============================================================================================== */
typedef enum video_cap_rgb2ycbcr_flags
{
	BT601_0_255 = 1,
	BT601_16_235 = 2,
	BT709_0_255 = 3,
	BT709_16_235 = 4
} EVideoCapRGB2YCbCrFlags;

/* ============================================================================================== */
typedef enum video_cap_ch_id_type
{
	NO_CH_ID = 0,
	SYNC_ID = 1,
	BLANKING_ID = 2,
	BOTH_ID = 3
} EVideoCapChIDType;

/* =========================================================================================== */
/*! A data structure used in function
 * \b VideoCap_Initial to set the initial options. */
typedef struct video_cap_init_options
{
	/*! Handle of video device. */
	HANDLE hVideoSignalObject;

	/*! Callback function for setting video device options. */
	FOnSetVideoSignalOptions pfnSetOptions;

	/*! Video capture library version number */
	DWORD dwVersion;

	/*! Maximum frame buffer width, must be multiple of 16. */
	DWORD dwMaxFrameWidth;

	/*! Maximum frame buffer height, must be multiple of 16. */
	DWORD dwMaxFrameHeight;

	/*! Capture width after subsample, must be multiple of 16.*/
	DWORD dwCapWidth;

	/*! Capture height after subsample, must be multiple of 16..*/
	DWORD dwCapHeight;

	/*! Start pixel in horizontal axis.*/
	DWORD dwStartPixel;

	/*! Start line in vertical axis.*/
	DWORD dwStartLine;

	/*! Output pixel format. */
	EPixelFormatFlags eOutPixelFormat;

	/*! Write field data to memory.
	* - 0 => Write two single fields in SDRAM.
	* - 1 => Write one interleaved field in SDRAM.
	*/
	BOOL bFieldMode;

	/*! Invert the field number
	* - 0 => Disable field invert
	* - 1 => Enable field invert
	*/
	BOOL bFieldInvert;

	/*! Specify the video input data type.
	* - VIDEO_CAP_FORMAT_PROGRESSIVE_8BITS_RAW => For 8-bit CCIR601 raw data.
	* - VIDEO_CAP_FORMAT_PROGRESSIVE_16BITS_RAW => For 16-bit CCIR601 raw data.
	* - VIDEO_CAP_FORMAT_INTERLACE_CCIR656 => For interlaced two-field CCIR656 data.
	* - VIDEO_CAP_FORMAT_PROGRESSIVE_CCIR656 => For progressive one-field CCIR656 data.
	* - VIDEO_CAP_FORMAT_BAYER_PATTERN => For Bayer pattern input data.
	*/
	EVideoSignalFormat eFormat;

	/*! The input video frame width. */
	DWORD dwInWidth;

	/*! The input video frame height. */
	DWORD dwInHeight;

	/*! Internal frame buffer number, not implemented yet, should set to 4. */
	DWORD dwBufNum;

	/*! The capture image frames in one second.
	* - 0: Disable frame rate control.
	* - Other value: Frame rate.
	*/
	DWORD dwFrameRate;

	/*! Which device to be opended. */
	DWORD dwDeviceNum;

	/*! Flip the video */
	BOOL bFlip;

	/*! Mirror the video */
	BOOL bMirror;

	/*! Adjust the brightness */
	DWORD dwBrightness;

	/*! Adjust the contrast */
	DWORD dwContrast;

	/*! Adjust the saturation */
	DWORD dwSaturation;

	/*! Video signal frequency enumeration. Please refer to global_codec.h */
	EVideoSignalFrequency eFrequency;

	DWORD adwConfig[200];

	/*! Start addrress of input video buffer allocated outside.
	*	(The start address must be 128 bytes align.)
	*/
	BYTE *pbyCapBufUsrBaseAddr;
	BYTE *pbyStatAEWBBufUsrBaseAddr;
	BYTE *pbyStatHistoBufUsrBaseAddr;
	BYTE *pbyStatFocusBufUsrBaseAddr;

	BYTE byChNum0, byChNum1;
	BOOL bChClkEdge0, bChClkEdge1;
	EVideoCapChIDType eChIDType0, eChIDType1;

	/*! VideoCap object memory address. Set to NULL to allocate object memory internally. */
	void *pObjectMem;

} TVideoCapInitOptions;

/* =========================================================================================== */
/*! A data structure of \b VideoCap object used in function \b VideoCap_Getbuf.
 * Fields in this structure will show the VideoCap states. */
typedef struct video_cap_state
{
	/*! The variable indicates index(0-3) of current video frame captured.*/
	DWORD dwIndex;

	/*! The variable indicates frame count (from VideoCap_Initial() was called) of current video frame captured.*/
	DWORD dwFrameCount;

	/*! Current captured frame second information. */
	DWORD dwSecond;

	/*! Current captured frame millisecond information. */
	DWORD dwMilliSecond;

	DWORD dwMicroSecond;
	BOOL bTimeStampCal;
	DWORD dwInWidth;

	/*! Current captured frame width information. */
	DWORD dwOutWidth;

	/*! Current captured frame height information. */
	DWORD dwOutHeight;

	/*! Current captured frame stride information. */
	DWORD dwOutStride;

	/* Video FIFO full occurred during capturing this frame. */
	BOOL bFifoFull;

	/*! No video input signal was detected. */
	BOOL bNoSignal;

	/*! CCIR protocol error or width check fail*/
	BOOL bCCIRErr;

	/*! Which device to be opended. */
	DWORD dwDeviceNum;

	/*! Current captured frame Y component base address */
	BYTE *pbyYFrame;

	/*! Current captured frame Cb component base address */
	BYTE *pbyCbFrame;

	/*! Current captured frame Cr component base address */
	BYTE *pbyCrFrame;

	DWORD dwAFFocusValueHigh;
	DWORD dwAFFocusValueLow;
	DWORD dwAFCurrentStatus;

/* Version 12.0.0.0 modification, 2012.06.28 */
/* ======================================== */

	DWORD dwAWBRedGain;
	DWORD dwAWBBlueGain;

/* Version 12.0.0.0 modification, 2012.06.28 */
	QWORD qwAWBRedSum;
	QWORD qwAWBGreenSum;
	QWORD qwAWBBlueSum;
/* ======================================== */

	DWORD dwAEShutter;
	DWORD dwAEGain;

	DWORD dwZoomRatio;

	BOOL bAEStable;

	DWORD dwCapH;
	DWORD dwCapV;

} TVideoCapState;

/* =========================================================================================== */
/*! A data structure of \b VideoCap object used in function \b VideoCap_SetOption. */
typedef struct video_cap_options
{
    /*! Option flag, which will decide the content of adwUserData */
	EVideoSignalOptionFlags eOptionFlags;
 	DWORD adwUserData[3];
} TVideoCapOptions;

/* =========================================================================================== */
/*! A data structure of \b VideoCap object used by EVideoSignalOptionFlags of "VIDEO_SIGNAL_OPTION_SET_AUTO_SCENE" in function \b VideoCap_SetOption. */
typedef struct video_cap_isp_param
{
	/*! Adjust the gamma table  */
	BYTE *pbyGammaLut;

	/*! Adjust the contrast enhancement */
	DWORD *pdwCEParam;

	/*! Adjust the tome mapping */
	DWORD *pdwTMParam;

	/*! Adjust the RGB2RGB color matrix */
	SDWORD *psdwColorParam;

	/*! Adjust the saturation & brightness & contrast */
	DWORD *pdwSBCParam;

	/*! Adjust the CFA antialiasing */
	DWORD *pdwAntialiasingParam;

	/*! Adjust the auto exposure */
	DWORD *pdwAEParam;
/* Version 12.0.0.1 modification, 2012.11.02 */
	/*! Adjust the CFA deimpulse */
	DWORD *pdwDeimpulseParam;

	/*! Adjust the black clamp */
	SDWORD *psdwBlackClampParam;

	/*! Adjust the CFA mode */
	DWORD *pdwCFAMode;
/* ======================================== */

/* Version 13.0.0.0 modification, 2013.04.19 */
	/*! Adjust the auto white balance */
	DWORD *pdwAWBParam;

	/*! Adjust the lens shading correction */
	DWORD *pdwLSCParam;
/* ======================================== */
} TVideoCapIspParam;

/* =========================================================================================== */
/*!
 *******************************************************************************
 * \brief
 * Create VideoCap instance and initialize it.
 * \param phObject
 * \a (o) pointer to receive the instance's handle of VideoCap instance.
 * \param ptInitOptions
 * \a (i) the pointer of data structure \b TVideoCapInitOptions for setting
 * the initialization parameters of the \b VideoCap instance.
 * \retval S_OK
 * Initialize \b VideoCap OK.
 * \retval S_FAIL
 * Initialize \b VideoCap failed.
 * \remark
 * This function should be called before using this instance.
 * \see VideoCap_Release
 *******************************************************************************/
SCODE VideoCap_Initial(HANDLE *phObject, TVideoCapInitOptions *ptInitOptions);

/*!
 *******************************************************************************
 * \brief
 * Delete an instance of the \b VideoCap object.
 * \param phObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 * \retval S_OK
 * Release VideoCap OK.
 * \retval S_FAIL
 * Release VideoCap failed.
 * \remark
 * After the instance is released, the handle of this instance will be set to
 * zero.
 * \see VideoCap_Initial
 *******************************************************************************/
SCODE VideoCap_Release(HANDLE *phObject);

/*!
 *******************************************************************************
 * \brief
 * Get video buffer for processing.
 * \param hObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 * \param ptState
 * \a (i) the pointer of data structure \b TVideoCapStatus to receiving VideoCap
 * status.
 * \retval S_OK
 * Get video buffer successfully.
 * \retval S_FAIL
 * Get video buffer failed.
 * \note
 * If VPE or CMOS hardware error occur, field \a dwErrACK in TVideoCapStatus
 * would be filled with error code.
 * \see VideoCap_Releasebuf
 *******************************************************************************/
SCODE VideoCap_GetBuf(HANDLE hObject, TVideoCapState *ptState);

/*!
 *******************************************************************************
 * \brief
 * Release video buffer for next capture.
 * \param hObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 * \retval S_OK
 * Release video buffer successfully.
 * \retval S_FAIL
 * Release video buffer failed.
 * \see VideoCap_Getbuf
 *******************************************************************************/
SCODE VideoCap_ReleaseBuf(HANDLE hObject, TVideoCapState *ptState);

/*!
 *******************************************************************************
 * \brief
 * Get video buffer for processing.
 * \param hObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 * \param ptState
 * \a (i) the pointer of data structure \b TVideoCapOptions to provide capture
 * options.
 * \retval S_OK
 * Set option successfully.
 * \retval S_FAIL
 * Set option failed.
 * \note
 * This function can't be called when VideoCap is not stopped.
 * \see VideoCap_Stop
 *******************************************************************************/
SCODE VideoCap_SetOptions(HANDLE hObject, TVideoSignalOptions *ptOptions);

/*!
 *******************************************************************************
 * \brief
 * Wait for interrupt wake up in ISR of VPE device driver.
 * \param hObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 * \retval S_OK
 * VideoCap_Sleep OK.
 * \retval S_FAIL
 * VideoCap_Sleep failed.
 * \note
 * Current process would be sleep until new frame capture OK.
 *******************************************************************************/
SCODE VideoCap_Sleep(HANDLE hObject);

/*!
 ******************************************************************************
 * \brief
 * Start videoin device to capture image to buffer
 *
 * \param hObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 *
 * \retval S_OK
 * Start VideoCap library successful.
 *
 * \retval S_FAIL
 * Start VideoCap library failed.
 *
 * \see VideoCap_Release
 * \see VideoCap_Stop
 *
 ******************************************************************************
 */
SCODE VideoCap_Start(HANDLE hObject);

/*!
 ******************************************************************************
 * \brief
 * Stop the videoin device
 *
 * \param hObject
 * \a (i) the pointer to the handle of the \b VideoCap instance.
 *
 * \retval S_OK
 * Stop VideoCap library successful.
 *
 * \retval S_FAIL
 * Stop VideoCap library failed.
 *
 * \remark
 * Call this function to stop the VideoCap library operation.
 *
 * \see VideoCap_Release
 * \see VideoCap_Start
 *
 ******************************************************************************
*/
/* Version 2.0.0.0 modification, 2006.06.21 */
SCODE VideoCap_Stop(HANDLE hObject);
/* ======================================== */

/*!
 *******************************************************************************
 * \brief
 * Calculate the byte number required by VideoCap instance.
 * \param ptInitOptions
 * \a (i) the pointer of data structure \b TVideoCapInitOptions for setting
 * the initialization parameters of the \b VideoCap instance.
 * \retval
 * Memory size required by VideoCap instance in bytes.
 * \see VideoCap_Initial
 *******************************************************************************/
DWORD VideoCap_QueryMemSize(TVideoCapInitOptions *ptInitOptions);

/*!
 *******************************************************************************
 * \brief
 * Get version information of VideoCap library
 * \param pbyMajor
 * \a (o) the pointer to the major version number of the \b VideoCap library.
 * \param pbyMinor
 * \a (o) the pointer to the minor version number of the \b VideoCap library.
 * \param pbyBuild
 * \a (o) the pointer to the build version number of the \b VideoCap library.
 * \param pbyRevision
 * \a (o) the pointer to the revision number of the \b VideoCap library.
 * \retval S_OK
 * VideoCap_GetVersionInfo OK.
 * \retval S_FAIL
 * VideoCap_GetVersionInfo failed.
 * \note
 * Current process would be sleep until new frame capture OK.
 *******************************************************************************/
SCODE VideoCap_GetVersionInfo(TVersionNum *ptVersion);

/* =========================================================================================== */
/** \example VideoCap_App.c
 * This is an example of how to use the VideoCap Library.
 *
 */
/* =========================================================================================== */
#endif /*__VIDEOCAP_H__*/
