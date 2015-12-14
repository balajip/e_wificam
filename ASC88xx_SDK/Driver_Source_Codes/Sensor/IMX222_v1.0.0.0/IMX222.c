/*
 * IMX222
 * Driver for IMX222 sensor.
 *
 * Copyright (C) 2010  ______ Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include "IMX222.h"
#include "IMX222_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR IMX222_ID[] = "$Version: "IMX222_ID_VERSION"  (IMX222 DRIVER) $";
/*==================================================================*/
static TIMX222Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore IMX222_mutex;

MODULE_AUTHOR("______ Inc.");
MODULE_DESCRIPTION("IMX222 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const DWORD adwIMX222GainTbl[141][2] =
{
	{  1000,   0},//	 0.0 	dB
	{  1035,   1},//	 0.3	dB	           
	{  1072,   2},//	 0.6	dB	           
	{  1110,   3},//	 0.9	dB	           
	{  1149,   4},//	 1.2	dB	           
	{  1189,   5},//	 1.5	dB	           
	{  1231,   6},//	 1.81	dB           
	{  1274,   7},//	 2.11	dB
	{  1319,   8},//	 2.41	dB
	{  1366,   9},//	 2.71	dB
	{  1414,  10},//	 3.01	dB
	{  1464,  11},//	 3.31	dB
	{  1516,  12},//	 3.61	dB
	{  1569,  13},//	 3.91	dB
	{  1624,  14},//	 4.21	dB
	{  1682,  15},//	 4.51	dB
	{  1741,  16},//	 4.82	dB
	{  1802,  17},//	 5.12	dB
	{  1866,  18},//	 5.42	dB
	{  1932,  19},//	 5.72	dB
	{  2000,  20},//	 6.02	dB
	{  2070,  21},//	 6.32	dB
	{  2143,  22},//	 6.62	dB
	{  2219,  23},//	 6.92	dB
	{  2297,  24},//	 7.22	dB
	{  2378,  25},//	 7.52	dB
	{  2462,  26},//	 7.82	dB
	{  2548,  27},//	 8.13	dB
	{  2638,  28},//	 8.43	dB
	{  2731,  29},//	 8.73	dB
	{  2828,  30},//	 9.03	dB
	{  2927,  31},//	 9.33	dB
	{  3031,  32},//	 9.63	dB
	{  3137,  33},//	 9.93	dB
	{  3248,  34},//	10.23	dB
	{  3362,  35},//	10.53	dB
	{  3481,  36},//	10.83	dB
	{  3604,  37},//	11.14	dB
	{  3731,  38},//	11.44	dB
	{  3862,  39},//	11.74	dB
	{  4000,  40},//	12.04	dB
	{  4139,  41},//	12.34	dB
	{  4285,  42},//	12.64	dB
	{  4436,  43},//	12.94	dB
	{  4593,  44},//	13.24	dB
	{  4755,  45},//	13.54	dB
	{  4922,  46},//	13.84	dB
	{  5096,  47},//	14.14	dB
	{  5276,  48},//	14.45	dB
	{  5462,  49},//	14.75	dB
	{  5654,  50},//	15.05	dB
	{  5853,  51},//	15.35	dB
	{  6060,  52},//	15.65	dB
	{  6274,  53},//	15.95	dB
	{  6495,  54},//	16.25	dB
	{  6724,  55},//	16.55	dB
	{  6961,  56},//	16.85	dB
	{  7206,  57},//	17.15	dB
	{  7460,  58},//	17.45	dB
	{  7723,  59},//	17.76	dB
	{  8000,  60},//	18.06	dB
	{  8277,  61},//	18.36	dB
	{  8569,  62},//	18.66	dB
	{  8871,  63},//	18.96	dB
	{  9184,  64},//	19.26	dB
	{  9508,  65},//	19.56	dB
	{  9843,  66},//	19.86	dB
	{ 10190,  67},//	20.16	dB
	{ 10549,  68},//	20.46	dB
	{ 10921,  69},//	20.77	dB
	{ 11306,  70},//	21.07	dB
	{ 11705,  71},//	21.37	dB
	{ 12117,  72},//	21.67	dB
	{ 12545,  73},//	21.97	dB
	{ 12987,  74},//	22.27	dB
	{ 13445,  75},//	22.57	dB
	{ 13919,  76},//	22.87	dB
	{ 14409,  77},//	23.17	dB
	{ 14917,  78},//	23.47	dB
	{ 15443,  79},//	23.77	dB
	{ 16000,  80},//	24.08	dB
	{ 16551,  81},//	24.38	dB
	{ 17135,  82},//	24.68	dB
	{ 17739,  83},//	24.98	dB
	{ 18364,  84},//	25.28	dB
	{ 19012,  85},//	25.58	dB
	{ 19682,  86},//	25.88	dB
	{ 20376,  87},//	26.18	dB
	{ 21094,  88},//	26.48	dB
	{ 21838,  89},//	26.78	dB
	{ 22608,  90},//	27.09	dB
	{ 23405,  91},//	27.39	dB
	{ 24230,  92},//	27.69	dB
	{ 25084,  93},//	27.99	dB
	{ 25969,  94},//	28.29	dB
	{ 26884,  95},//	28.59	dB
	{ 27832,  96},//	28.89	dB
	{ 28813,  97},//	29.19	dB
	{ 29829,  98},//	29.49	dB
	{ 30881,  99},//	29.79	dB
	{ 32000, 100},//	30.09	dB
	{ 33096, 101},//	30.4	dB	
	{ 34263, 102},//	30.7	dB	
	{ 35471, 103},//	31.0	dB	
	{ 36722, 104},//	31.3	dB	
	{ 38016, 105},//	31.6	dB	
	{ 39357, 106},//	31.9	dB	
	{ 40744, 107},//	32.2	dB	
	{ 42181, 108},//	32.5	dB	
	{ 43668, 109},//	32.8	dB	
	{ 45207, 110},//	33.1	dB	
	{ 46801, 111},//	33.41	dB
	{ 48451, 112},//	33.71	dB
	{ 50159, 113},//	34.01	dB
	{ 51927, 114},//	34.31	dB
	{ 53758, 115},//	34.61	dB
	{ 55653, 116},//	34.91	dB
	{ 57616, 117},//	35.21	dB
	{ 59647, 118},//	35.51	dB
	{ 61750, 119},//	35.81	dB
	{ 64000, 120},//	36.11	dB
	{ 66257, 121},//	36.3	dB
	{ 68594, 122},//	36.6	dB
	{ 71013, 123},//	36.9	dB
	{ 73517, 124},//	37.2	dB
	{ 76110, 125},//	37.5	dB
	{ 78794, 126},//	37.8	dB
	{ 81573, 127},//	38.1	dB
	{ 84450, 128},//	38.4	dB
	{ 87428, 129},//	38.7	dB
	{ 90511, 130},//	39.0	dB
	{ 93703, 131},//	39.3	dB
	{ 97007, 132},//	39.6	dB
	{100428, 133},//	39.9	dB
	{103970, 134},//	40.2	dB
	{107636, 135},//	40.5	dB
	{111432, 136},//	40.8	dB
	{115362, 137},//	41.1	dB
	{119430, 138},//	41.4	dB
	{123642, 139},//	41.7	dB
	{128000, 140},//	42.0	dB
};

/*==================================================================*/
static BYTE IMX222_BitReverse(BYTE byData)
{
	BYTE byOut;

	byOut = ((byData<<7)&0x80) | ((byData<<5)&0x40) | ((byData<<3)&0x20) | ((byData<<1)&0x10) |
			((byData>>1)&0x08) | ((byData>>3)&0x04) | ((byData>>5)&0x02) | ((byData>>7)&0x01);

	return byOut;
}

/*-------------------------------------------------------------------------------------*/
static int IMX222_WriteReg(TIMX222Info *ptInfo, EIMX222Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BOOL bWaitCmpt)
{
	BYTE abyData[6];
	BYTE byOutRegAddr;
	DWORD dwLength = (dwDataLen+1);

	byOutRegAddr = IMX222_BitReverse(ptInfo->byChipID);
	abyData[0] = IMX222_BitReverse((BYTE)eRegAddr);
	abyData[1] = IMX222_BitReverse(dwData&0xFF);
	abyData[2] = IMX222_BitReverse((dwData>>8)&0xFF);
	abyData[3] = IMX222_BitReverse((dwData>>16)&0xFF);
	abyData[4] = IMX222_BitReverse((dwData>>24)&0xFF);
	abyData[dwLength] = (BYTE)bWaitCmpt;

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData,  ptInfo->dwDeviceAddr, (DWORD)byOutRegAddr, (dwLength+1), abyData);
}

/*-------------------------------------------------------------------------------------*/
static void IMX222_Reset(TIMX222Info *ptInfo)
{
	DWORD dwFlags;

	ptInfo->byChipID = 0x02;

	local_irq_save(dwFlags);

	IMX222_WriteReg(ptInfo, _00_Stby, 0x0031, 1, FALSE);//Register TestEN and standby
	IMX222_WriteReg(ptInfo, _11_FRSEL, 0x0080, 1, FALSE);//Frame and output mode select 30frame/s Parallel cmos SDR output
	IMX222_WriteReg(ptInfo, _2D_DCKDLY, 0x0041, 1, FALSE);//Dclk output edge
	IMX222_WriteReg(ptInfo, _02_Mode, 0x000F, 1, FALSE);// HD 1080P mode
	IMX222_WriteReg(ptInfo, _14_WINPH, 0x0000, 2, FALSE);// Cropping start address Horizontal
	IMX222_WriteReg(ptInfo, _16_WINPV, 0x003D, 2, FALSE);// Cropping start address Vertical 
	IMX222_WriteReg(ptInfo, _18_WINWH, 0x07C0, 2, FALSE);// Window crop-out size(Width)
	IMX222_WriteReg(ptInfo, _1A_WINWV, 0x04C9, 2, FALSE);// Window crop-out size(Hight)

	IMX222_WriteReg(ptInfo, _12_SSBRK, 0x0082, 1, FALSE);// Output Resolution Select 10bit/12bit 
	IMX222_WriteReg(ptInfo, _9A_12B1080P, 0x0226, 2, FALSE);// 12bit 1080p 30frame/s
	IMX222_WriteReg(ptInfo, _CE_PRES, 0x0016, 1, FALSE);// 12bit 1080p 30frame/s
	IMX222_WriteReg(ptInfo, _CF_DRES, 0x0082, 2, FALSE);// 12bit 1080p 30frame/s
	IMX222_WriteReg(ptInfo, _1E_GAIN, 0x0000, 1, FALSE); // Gain 0 db
	IMX222_WriteReg(ptInfo, _0D_SPL, 0x0000, 2, FALSE); // SPL
	IMX222_WriteReg(ptInfo, _0F_SVS, 0x0000, 2, FALSE); // SVS
	IMX222_WriteReg(ptInfo, _08_SHS1, 0x0000, 2, FALSE); // SHS1
	IMX222_WriteReg(ptInfo, _20_BLKLEVEL, 0x003C, 1, FALSE); // Black level
	IMX222_WriteReg(ptInfo, _21_XHSLNG, 0x0020, 1, FALSE); // xhslng
	IMX222_WriteReg(ptInfo, _22_XVSLNG, 0x0043, 1, FALSE); // xvslng

	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
	{
		ptInfo->dwHsync = IMX222_HSYNC_1920x1080;
		ptInfo->dwVsync = IMX222_VSYNC_1920x1080;	
	}
	else
	{
		ptInfo->dwHsync = IMX222_HSYNC_1920x1080_50;
		ptInfo->dwVsync = IMX222_VSYNC_1920x1080_50;
	} 
 	IMX222_WriteReg(ptInfo, _03_HMAX, (ptInfo->dwHsync>>1), 2, FALSE);
 	IMX222_WriteReg(ptInfo, _05_VMAX, ptInfo->dwVsync, 2, FALSE);
	IMX222_WriteReg(ptInfo, _2C_XMSTA, 0x0000, 1, FALSE); // Master mode operation start
	IMX222_WriteReg(ptInfo, _00_Stby, 0x0030, 1, FALSE); //out Standby

	mdelay(100);

	local_irq_restore(dwFlags);
}

/*-------------------------------------------------------------------------------------*/
static void IMX222_SetShutter(TIMX222Info *ptInfo, DWORD dwShutter)
{
	DWORD dwExpoLine, dwExpoLineSend, dwVMAX;

	if (dwShutter > 1000000)
		dwShutter = 1000000;

	dwExpoLine = ((dwShutter*IMX222_PIXEL_CLOCK_M)/10) / ptInfo->dwHsync;

	if (ptInfo->dwPreExpLine != dwExpoLine)
	{
		dwVMAX = (dwExpoLine>ptInfo->dwVsync) ? dwExpoLine : ptInfo->dwVsync;

		dwExpoLineSend = dwVMAX - dwExpoLine;

		if (dwExpoLineSend >= (dwVMAX-1))
			dwExpoLineSend = dwVMAX - 1;
		else if (dwExpoLineSend <= 3)
			dwExpoLineSend = 3;

		dwExpoLineSend = (dwExpoLineSend>=0xFFFF) ? 0xFFFF : dwExpoLineSend;
		dwVMAX = (dwVMAX>=0xFFFF) ? 0xFFFF : dwVMAX;

		if (ptInfo->dwPreExpFrame != dwVMAX)
		{
			IMX222_WriteReg(ptInfo, _05_VMAX, dwVMAX, 2, FALSE);
			IMX222_WriteReg(ptInfo, _08_SHS1,  dwExpoLineSend, 2, FALSE);
			ptInfo->dwPreExpFrame = dwVMAX;
		}
		else
		{
			IMX222_WriteReg(ptInfo, _08_SHS1,  dwExpoLineSend, 2, FALSE);
		}

		ptInfo->dwCurShutter = (dwExpoLine&0xFFFF);	// shutter frame num
		ptInfo->dwPreExpLine = dwExpoLine;
	}  
}

/*-------------------------------------------------------------------------------------*/
static void IMX222_SetGain(TIMX222Info *ptInfo, DWORD dwGain)
{
	WORD dwTblIdx, dwTempCnt;

	if (dwGain > IMX222_MAX_GAIN)
		dwGain = IMX222_MAX_GAIN;
	else if (dwGain < 1000)
		dwGain = 1000;

	if (dwGain <= 2000)      //6dB
		dwTblIdx = 0;
	else if (dwGain <= 4000) //12dB
		dwTblIdx = 20;
	else if (dwGain <= 8000) //18dB
		dwTblIdx = 40;
	else if (dwGain <= 16000)//24dB
		dwTblIdx = 60;
	else if (dwGain <= 32000)//30dB
		dwTblIdx = 80;
	else if (dwGain <= 64000)//36dB
		dwTblIdx =100;
	else
		dwTblIdx =120;
  
	for (dwTempCnt=0; dwTempCnt<20; dwTempCnt++,dwTblIdx++)
	{
		if (dwGain <= adwIMX222GainTbl[dwTblIdx][0])
			break;
	}

	if (ptInfo->dwPreGain != adwIMX222GainTbl[dwTblIdx][1])
	{
		IMX222_WriteReg(ptInfo, _1E_GAIN,  adwIMX222GainTbl[dwTblIdx][1], 1, FALSE);
		ptInfo->dwPreGain = adwIMX222GainTbl[dwTblIdx][1];
	}
}

/*==================================================================*/
SOCKET  IMX222_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET sckRet = 0;	

	if (down_interruptible(&IMX222_mutex))
		return -ERESTARTSYS;

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		IMX222_Reset(ptInfo);
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		IMX222_SetShutter(ptInfo, ptIoctlArg->adwUserData[0]);
		break;           
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		IMX222_SetGain(ptInfo, ptIoctlArg->adwUserData[0]);		
		break;    
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[IMX222.ko] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			sckRet = -EPERM;
			break;
		}

		if (ptIoctlArg->adwUserData[0] == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			ptInfo->dwHsync = IMX222_HSYNC_1920x1080;
			ptInfo->dwVsync = IMX222_VSYNC_1920x1080;
		}
		else
		{
			ptInfo->dwHsync = IMX222_HSYNC_1920x1080_50;
			ptInfo->dwVsync = IMX222_VSYNC_1920x1080_50;
		}                               

		if (ptInfo->dwCurShutter > (ptInfo->dwVsync-1))
		{
			ptInfo->dwCurShutter = ptInfo->dwVsync-1;
			IMX222_WriteReg(ptInfo, _08_SHS1, 1, 2, FALSE);
		}

		IMX222_WriteReg(ptInfo, _05_VMAX, ptInfo->dwVsync, 2, FALSE);
		mdelay(40);			
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];	
		break;
		default:
		sckRet = -EPERM;
	}

	up(&IMX222_mutex);	
	return sckRet;
}

/*-------------------------------------------------------------------------------------*/
DWORD IMX222_VICGetShutterValue(void)
{
	DWORD dwRet;

	dwRet = ptInfo->dwCurShutter;

	return dwRet;
}

/*-------------------------------------------------------------------------------------*/
DWORD IMX222_GetRemainingLineNum(void)
{
	if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		return (IMX222_VSYNC_1920x1080 - IMX222_DEFAULT_OUTPUT_HEIGHT - 16 - 8);
	else
		return (IMX222_VSYNC_1920x1080_50 - IMX222_DEFAULT_OUTPUT_HEIGHT - 16 - 8);
}
/*-------------------------------------------------------------------------------------*/
void IMX222_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET IMX222_Open(TVideoSensorInitialParam* ptIMX222InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptIMX222InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(IMX222_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptIMX222InitialParam->dwVideoSensorVersion&0x0000FF00)>(IMX222_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid IMX222 device driver version %d.%d.%d.%d !!\n", 
				(int)(IMX222_INTERFACE_VERSION&0xFF), 
				(int)((IMX222_INTERFACE_VERSION>>8)&0xFF), 
				(int)((IMX222_INTERFACE_VERSION>>16)&0xFF), 
				(int)(IMX222_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TIMX222Info *)kmalloc(sizeof(TIMX222Info), GFP_KERNEL)))
	{
		printk("[IMX222.ko] : Allocate %d bytes memory fail\n", sizeof(TIMX222Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_WRITEBUF_FAST_NOACK;
	tIICCtrlInitOptions.dwMaxDataLength = 8;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[IMX222.ko] : Initial IICCtrl object fail !!\n");
		IMX222_Release(dwDevNum);					
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = IMX222_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = ptIMX222InitialParam->dwVideoSensorFrequency;
	ptInfo->bInit = FALSE;

	// Reset device
	IMX222_Reset(ptInfo);

	ptInfo->bInit = TRUE;	
	return 0;
}

/*-------------------------------------------------------------------------------------*/
DWORD IMX222_GetMaxGain(void)
{
	return (IMX222_MAX_GAIN);
}

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open                  = IMX222_Open,
	.release               = IMX222_Release,
	.ioctl                 = IMX222_Ioctl,
	.get_shutter_value     = NULL,
	.get_remaining_line_num= NULL,
	.group_access          = NULL,	
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = IMX222_GetMaxGain,
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct IMX222_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t IMX222_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct IMX222_attribute *IMX222_attr = container_of(attr, struct IMX222_attribute, attr);
	int result = 0;
	
	if (IMX222_attr->show)
		result = IMX222_attr->show(kobj, buf);	
	return result;
}

ssize_t IMX222_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "IMX222");
	return retval;
}

ssize_t IMX222_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t IMX222_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1920");
	return retval;
}

ssize_t IMX222_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1125");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject IMX222_kobj;

struct IMX222_attribute IMX222_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX222_SensorName_attr_show,
};

struct IMX222_attribute IMX222_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX222_dwBufNum_attr_show,
};

struct IMX222_attribute IMX222_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX222_MaxFrameWidth_attr_show,
};

struct IMX222_attribute IMX222_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = IMX222_MaxFrameHeight_attr_show,
};

struct sysfs_ops IMX222_sysfs_ops = {
	.show	= IMX222_default_attr_show,
};

struct kobj_type IMX222_ktype = {
	.sysfs_ops	= &IMX222_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static int IMX222_Init(void)
{
	int result = 0;
	
	if (result < 0){
		printk("[IMX222] : kobj set name fail!\n");
		return -1;
	}

	IMX222_kobj.ktype = &IMX222_ktype;	
	result = kobject_init_and_add(&IMX222_kobj, &IMX222_ktype, NULL, "VideoSensorInfo");

	if (result < 0){
		printk("[IMX222] : Cannot register kobject [IMX222_kobj].\n");
		return -1;
	}

	result = sysfs_create_file(&IMX222_kobj, &IMX222_SensorName_attr.attr);		
	if (result < 0){
		printk("[IMX222] : Cannot create IMX222_SensorName_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX222_kobj, &IMX222_dwBufNum_attr.attr);		
	if (result < 0){
		printk("[IMX222] : Cannot create IMX222_dwBufNum_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX222_kobj, &IMX222_MaxFrameWidth_attr.attr);		
	if (result < 0){
		printk("[IMX222] : Cannot create IMX222_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	result = sysfs_create_file(&IMX222_kobj, &IMX222_MaxFrameHeight_attr.attr);		
	if (result < 0){
		printk("[IMX222] : Cannot create IMX222_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&IMX222_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void IMX222_Exit(void)
{
	sysfs_remove_file(&IMX222_kobj, &(IMX222_SensorName_attr.attr));
	sysfs_remove_file(&IMX222_kobj, &(IMX222_dwBufNum_attr.attr));	
	sysfs_remove_file(&IMX222_kobj, &(IMX222_MaxFrameWidth_attr.attr));	
	sysfs_remove_file(&IMX222_kobj, &(IMX222_MaxFrameHeight_attr.attr));		
	kobject_put(&IMX222_kobj);	
	printk(KERN_DEBUG "[IMX222] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(IMX222_Init);
module_exit(IMX222_Exit);

