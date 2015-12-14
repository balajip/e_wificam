/*
 * ad5602
 * Driver for AD5602.
 *
 * Copyright (C) 2010  VN Inc.
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
 */
 
/* ============================================================================================== */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/version.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "ad5602.h"
#include "ad5602_locals.h"
/* Version 2.0.0.0 modification, 2011.06.20 */
#include <linux/i2c.h> // kernel i2c interface
/* ======================================== */

const CHAR AD5602_ID[] = "$Version: "AD5602_ID_VERSION"  (AD5602 DRIVER) $";
/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static struct i2c_client *i2c[2];
static struct i2c_client client_template;
static const unsigned short normal_i2c[] = {AD5602_DEAFULT_DEVICE0_ADDR>>1, AD5602_DEAFULT_DEVICE1_ADDR>>1, I2C_CLIENT_END};
/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;
/* ======================================== */

MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("AD5602 driver");
MODULE_LICENSE("GPL");

/* ============================================================================================== */
// Para update by Yan-Chen Lu @ 2010-July-21
// Apply variable motor step size via speed control I/F.
// This is achieved by locking with a fixed waiting interval for different
// operating speeds. One may wonder what if we would like to have different
// speed to achieve the same degree of aperture open (or close). The answer
// is U cannot. Since u can stably run in a faster pace, why would u like to
// slow it down.
/* Version 2.0.0.1 modification, 2011.11.17 */
DWORD adwDCIrisSpeedVoltage[2][31] =
/* ======================================== */
{
	//0, /* voltage difference */ 64, 96, 128, 235, 255
/* Version 1.0.0.2 modification, 2010.09.28 */
	//{0, /* voltage difference */ 50, 50, 100, 200, 255} // 2010-08-10
/* Version 1.0.0.3 modification, 2011.01.21 */
	//{127, /* voltage difference */ 100, 80, 60, 40, 0},
/* Version 2.0.0.1 modification, 2011.11.17 */	
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 127},
/* ======================================== */
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 127},
/* ======================================== */
/* ======================================== */
};

// Para update by Yan-Chen Lu @ 2010-July-21.
DWORD adwDCIrisSpeedWaitInterval[2][DCIRIS_SPEED_NUM] =
{	// unit:ms
#if 1 // variable stepping size
	// aperture open
/* Version 1.0.0.2 modification, 2010.09.28 */
	//{25, 40, 20, 20, 50, 80},
/* Version 1.0.0.3 modification, 2011.01.21 */
	//{20, 40, 20, 20, 50, 120},
	{25, 40, 40, 40, 60, 120},
/* ======================================== */
	// aperture close
	//{25, 60, 40, 40, 60, 120},
	{25, 40, 40, 40, 60, 120},
/* ======================================== */
#else // different speed for the same amount of aperture change
	// aperture open
	{550, // zero speed
	 2500, // slowest speed
	 1500, // slow speed
	 550, // normal speed
	 350, // fast speed
	 50}, // fastest speed (200)
	// aperture close
	{550, // zero speed
	 3000, // slowest speed
	 1800, // slow speed
	 750, // normal speed
	 500, // fast speed
	 100}, // fastest speed (250)
#endif	 
};
/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static void AD5602_WriteReg(DWORD dwDevNum, DWORD dwData)
/* ======================================== */
{
	BYTE pbyData[2];
	
	pbyData[0] = ((dwData>>4) & 0xFF);
	pbyData[1] = (dwData & 0x0F);
/* Version 2.0.0.0 modification, 2011.06.20 */
	if (i2c[dwDevNum] != NULL) 
	{
		if (i2c_master_send(i2c[dwDevNum], pbyData, 2) != 2) 
		{
			printk("AD5602: i2c write failed\n");
		}
	}
/* ======================================== */
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static void AD5602_WriteDataToDevice(DWORD dwData0, DWORD dwData1, DWORD dwDeviceNum)
/* ======================================== */
{	
	switch (dwDeviceNum)
	{
		case 0:
		AD5602_WriteReg(0, dwData0);
		break;
		case 1:
		AD5602_WriteReg(1, dwData1);
		break;
		case 2:
		default:
		AD5602_WriteReg(0, dwData0);
		AD5602_WriteReg(1, dwData1);
		break;
	}
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static void AD5602_reset(void)
/* ======================================== */
{	
	AD5602_WriteReg(0, 0);
	AD5602_WriteReg(1, 0);
	printk("AD5602_reset done.\n");
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
/* Version 2.0.0.1 modification, 2011.11.17 */
static void DCIris_Open(DWORD eSpeed)
/* ======================================== */
/* ======================================== */
{		
/* Version 1.0.0.2 modification, 2010.09.28 */
	//DWORD dwulRef = 255;
	//DWORD dwulDrive = dwulRef - dwDCIris_speed_voltage[speed];
/* Version 2.0.0.1 modification, 2011.11.17 */
	DWORD dwulRef = 64;
/* ======================================== */
/* Version 1.0.0.3 modification, 2011.01.21 */
	//DWORD dwulDrive = dwDCIris_speed_voltage[0][speed];
/* ======================================== */
	//AD5602_WriteDataToDevice(ptInfo, ulDrive, ulRef, 2);
/* Version 2.0.0.1 modification, 2011.11.17 */
	DWORD dwulDrive = dwulRef + ((adwDCIrisSpeedVoltage[0][eSpeed]*140)/100); // 140
	AD5602_WriteDataToDevice(dwulRef , dwulDrive, 2);
/* ======================================== */
/* ======================================== */
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
/* Version 2.0.0.1 modification, 2011.11.17 */
static void DCIris_Close(DWORD eSpeed)
/* ======================================== */
/* ======================================== */
{
/* Version 1.0.0.2 modification, 2010.09.28 */
	//DWORD dwulRef = 0;
	//DWORD dwulDrive = dwulRef + dwDCIris_speed_voltage[speed];
	DWORD dwulRef = 128;
/* Version 2.0.0.1 modification, 2011.11.17 */
	DWORD dwulDrive = dwulRef + ((adwDCIrisSpeedVoltage[1][eSpeed]*80)/100); //80
/* ======================================== */
/* Version 1.0.0.3 modification, 2011.01.21 */
/* ======================================== */
	AD5602_WriteDataToDevice(dwulDrive, dwulRef, 2);
/* ======================================== */
}

/* ============================================================================================== */
static void DCIris_ResetWait(SDWORD sdwInterval)
{
	while (sdwInterval >= 0) 
	{
		msleep(20);
		sdwInterval -= 20;
	}
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static void DCIris_Stop(void)
/* ======================================== */
{	
	AD5602_WriteDataToDevice(128, 128, 2);
	
	// 2010-08-10, a immediate stop cannnot ganrantee, a breaking period is issued.
	//msleep(5);
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static void DCIris_Wait(EDCIrisSpeed eSpeed, DWORD dwOpen)
/* ======================================== */
{
#if 0	//FIXME
	// Qx.6 -> Qx.0
	dwInterval >>= 6;
	dwInterval <<= 16;
	
	Timer_ProfileStart(ptDCIrisInfo->hTimerObject);
	printf("DCIris_wait...START!\n") ;
	do
	{	
		Timer_GetInterval(ptDCIrisInfo->hTimerObject, &dwTicks);
		if (dwTicks >= dwInterval)
		{
			//printf("dwTicks / dwInterval = %d / %d (%.2f).\n", dwTicks, dwInterval, (double)dwTicks/APB_CLK);
			break;
		}
			
		usleep(100);
		
	} while (1);
	printf("DCIris_wait...END!\n") ;
#else	
	SDWORD sdwInterval;

	if (dwOpen)
	{
		sdwInterval = (SDWORD)adwDCIrisSpeedWaitInterval[0][eSpeed];
	}
	else
	{
		sdwInterval = (SDWORD)adwDCIrisSpeedWaitInterval[1][eSpeed];
	}
	while (sdwInterval >= 0)
	{
		msleep(20);
		sdwInterval -= 20;
	}
#endif
}

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
static void DCIris_Reset(EDCIrisApertureSz eApertureSz)
/* ======================================== */
{
	DWORD dwSpeed = DCIRIS_FASTEST_SPEED;

	AD5602_reset();
	if (eApertureSz == DCIRIS_APERTURE_OPEN)
	{	// all the way up
		DCIris_Open(dwSpeed);
		DCIris_Wait(DCIRIS_NORMAL_SPEED, 1);
		DCIris_Stop();
	}
	else if (eApertureSz == DCIRIS_APERTURE_CLOSE)
	{	// all the way down
		DCIris_Close(dwSpeed);
		DCIris_Wait(DCIRIS_NORMAL_SPEED, 0);
		DCIris_Stop();
	}
	else // all the way down first, then up to the half
	{
/* Version 1.0.0.4 modification, 2011.01.26 */
		AD5602_WriteDataToDevice(255 , 0, 2);
/* ======================================== */
		//DCIris_Wait(ptInfo, DCIris_Normal_Speed, 1);
/* Version 1.0.0.4 modification, 2011.01.26 */
		DCIris_ResetWait(100);
		AD5602_WriteDataToDevice(0 , 255, 2);
		//DCIris_Wait(ptInfo, DCIris_Normal_Speed, 0);
		DCIris_ResetWait(200);
/* ======================================== */
		DCIris_Stop();
	}
	
}

/* ============================================================================================== */
/* Version 3.0.0.0 modification, 2012.09.20 */
SOCKET  AD5602_DCIrisControl(DWORD dwCmd,  DWORD dwArg)
{
	SOCKET sckRet = 0;
	TIrisCtrlInfo* ptAD5602Info;

	switch (dwCmd)
	{
		case LENS_CTRL_IRIS_OPEN:
		ptAD5602Info = (TIrisCtrlInfo*)dwArg;
/* Version 2.0.0.1 modification, 2011.11.17 */
		DCIris_Open(ptAD5602Info->dwSpeed);
/* ======================================== */
		break;
		case LENS_CTRL_IRIS_CLOSE:
/* Version 2.0.0.1 modification, 2011.11.17 */
		ptAD5602Info = (TIrisCtrlInfo*)dwArg;
		DCIris_Close(ptAD5602Info->dwSpeed);
/* ======================================== */
		break;
/* Version 1.0.0.3 modification, 2011.01.21 */
		case LENS_CTRL_IRIS_OPEN_LARGEST:
		ptAD5602Info = (TIrisCtrlInfo*)dwArg;
		DCIris_Open(ptAD5602Info->dwSpeed);
		DCIris_ResetWait(2000);
		DCIris_Stop();
		break;
		case LENS_CTRL_IRIS_CLOSE_SMALLEST:
		case LENS_CTRL_IRIS_STOP:
/* ======================================== */
		default:
/* ======================================== */
		sckRet = -EPERM;		
	}
	return sckRet;		
}

/* Version 2.0.0.0 modification, 2011.06.20 */
/*======================= I2C related ===========================*/
static int ad5602_codec_probe(struct i2c_adapter *adap, int addr, int kind)
{
	int ret;
	int dev_num;

	client_template.adapter = adap;
	client_template.addr = addr;
	dev_num = (addr==(AD5602_DEAFULT_DEVICE0_ADDR>>1)) ? 0 : 1;
 	i2c[dev_num] = kmemdup(&client_template, sizeof(client_template), GFP_KERNEL);
	if (i2c[dev_num] == NULL) 
	{
		printk(KERN_ERR "new i2c fail..\n");
		return -ENOMEM;
	}
	ret = i2c_attach_client(i2c[dev_num]);
	if(ret < 0) 
	{
		printk("ad5602 failed to attach at addr %x\n", addr);
		goto err;
	}
	return ret;

err:
	kfree(i2c[dev_num]);
	return ret;

}

static int ad5602_i2c_detach(struct i2c_client *client)
{
	i2c_detach_client(client);
	kfree(client);
	return 0;
}

static int ad5602_i2c_attach(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, ad5602_codec_probe);
}

#define I2C_DRIVERID_AD5602 0xfefe /* need a proper id */
/* corgi i2c codec control layer */
static struct i2c_driver ad5602_i2c_driver = {
	.driver = {
		.name = "AD5602 I2C",
		.owner = THIS_MODULE,
	},
	.id =             I2C_DRIVERID_AD5602,
	.attach_adapter = ad5602_i2c_attach,
	.detach_client =  ad5602_i2c_detach,
	.command =        NULL,
};

static struct i2c_client client_template = {
	.name =   "AD5602",
	.driver = &ad5602_i2c_driver,
};
/* ======================================== */

/* ============================================================================================== */
void AD5602_DCIrisRelease(void)
{
/* Version 2.0.0.0 modification, 2011.06.20 */
	i2c_del_driver(&ad5602_i2c_driver);	
/* ======================================== */
/* Version 3.0.0.0 modification, 2012.09.20 */
	module_put(THIS_MODULE);
/* ======================================== */
}

/* ============================================================================================== */
SOCKET AD5602_DCIrisOpen(void)
{
/* Version 2.0.0.0 modification, 2011.06.20 */
	if (i2c_add_driver(&ad5602_i2c_driver) != 0)
	{
		printk("can't add i2c driver");
	}
/* ======================================== */

	// reset device
	DCIris_Reset(DCIRIS_APERTURE_MID);

/* Version 3.0.0.0 modification, 2012.09.20 */
	if (try_module_get(THIS_MODULE) == 0)
	{
		return -EBUSY;
	}
/* ======================================== */
	return 0;
}

/* ============================================================================================== */
/* Version 3.0.0.0 modification, 2012.09.20 */
static TIrisMotorDevice DCIris_dev_ops =
/* ======================================== */
{
	.open = AD5602_DCIrisOpen,
	.release = AD5602_DCIrisRelease,
	.control = AD5602_DCIrisControl,
/* Version 3.0.0.0 modification, 2012.09.20 */
	.set_options = NULL,
/* ======================================== */
};
/* Version 3.0.0.0 modification, 2012.09.20 */
extern TIrisMotorDevice *iris_motor_dev;

/* ============================================================================================== */
static SOCKET AD5602_Init(void)
{
	iris_motor_dev = &DCIris_dev_ops;

	printk("Install ad5602 driver version %d.%d.%d.%d complete !!\n",
		(int)(AD5602_VERSION&0xFF),
		(int)((AD5602_VERSION>>8)&0xFF),
		(int)((AD5602_VERSION>>16)&0xFF),
		(int)((AD5602_VERSION>>24)&0xFF));

	return 0;
}

/* ============================================================================================== */
static void AD5602_Exit(void)
{
	iris_motor_dev = NULL;
	printk(KERN_DEBUG "[AD5602] : bye\n");
	return;
}

module_init(AD5602_Init);
module_exit(AD5602_Exit);
/* ======================================== */
/* ============================================================================================== */
