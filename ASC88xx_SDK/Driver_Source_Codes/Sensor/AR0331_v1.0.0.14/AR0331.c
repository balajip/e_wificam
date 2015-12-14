/*
 * AR0331
 * Driver for AR0331 sensor.
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

#include "AR0331.h"
#include "AR0331_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR AR0331_ID[] = "$Version: "AR0331_ID_VERSION"  (AR0331 DRIVER) $";

static TAR0331Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore AR0331_mutex;

static DWORD dwAR0331Hsync = AR0331_HSYNC_1920x1080_60HZ;
static DWORD dwAR0331Vsync = AR0331_VSYNC_1920x1080_60HZ;

static SOCKET video_sensor_width = 1920;
static SOCKET video_sensor_height = 1080;
/* Version 1.0.0.1 modification, 2012.04.02 */
static SOCKET sckEnHDR = 1;
/* ======================================== */
/* Version 1.0.0.8 modification, 2013.05.24 */
static SOCKET sckEn2DMC = 0;
/* ======================================== */
/* Version 1.0.0.9 modification, 2013.07.01 */
static SOCKET sckEnSerialIF = 1;
static SOCKET sckHasOTPMCode = 1;
/* ======================================== */
/* Version 1.0.0.11 modification, 2013.09.04 */
static SOCKET sckGainR = 1024;
static SOCKET sckGainB = 1024;
/* ======================================== */

/* Version 1.0.0.13 modification, 2013.12.24 */
static SOCKET sckEnDPC = 0;
/* ======================================== */

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "AR0331 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "AR0331 capture height");
/* Version 1.0.0.1 modification, 2012.04.02 */
module_param(sckEnHDR, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckEnHDR, "Enable AR0331 HDR");
/* ======================================== */
/* Version 1.0.0.8 modification, 2013.05.24 */
module_param(sckEn2DMC, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckEn2DMC, "Enable 2D motion correction or DLO");
/* ======================================== */
/* Version 1.0.0.9 modification, 2013.07.01 */
module_param(sckEnSerialIF, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckEnSerialIF, "AR0331 Serial Mode On/Off");
module_param(sckHasOTPMCode, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckHasOTPMCode, "AR0331 has sequence setting in OTPM or not");
/* ======================================== */
/* Version 1.0.0.11 modification, 2013.09.04 */
module_param(sckGainR, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckGainR, "Apply fixed WB R gain");
module_param(sckGainB, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckGainB, "Apply fixed WB B gain");
/* ======================================== */

/* Version 1.0.0.13 modification, 2013.12.24 */
module_param(sckEnDPC, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckEnDPC, "Enable DPC on FPGA");
/* ======================================== */

/*==================================================================*/
MODULE_AUTHOR("______ Inc.");
MODULE_DESCRIPTION("AR0331 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TAR0331RegAddrData atAR0331HDRSequenceTbl[AR0331_HDR_SEQUENCE_TBL_SIZE] = 
{
	// [AR0331 Rev2 HDR sequencer load - 3.0]
	{0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x6134}, 	// SEQ_DATA_PORT
	{0x3086, 0x4A31}, 	// SEQ_DATA_PORT
	{0x3086, 0x4342}, 	// SEQ_DATA_PORT
	{0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{0x3086, 0x2714}, 	// SEQ_DATA_PORT
	{0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{0x3086, 0x3DEA}, 	// SEQ_DATA_PORT
	{0x3086, 0x2704}, 	// SEQ_DATA_PORT
	{0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{0x3086, 0x2705}, 	// SEQ_DATA_PORT
	{0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{0x3086, 0x2715}, 	// SEQ_DATA_PORT
	{0x3086, 0x3527}, 	// SEQ_DATA_PORT
	{0x3086, 0x053D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1045}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0427}, 	// SEQ_DATA_PORT
	{0x3086, 0x143D}, 	// SEQ_DATA_PORT
	{0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{0x3086, 0xEA62}, 	// SEQ_DATA_PORT
	{0x3086, 0x2728}, 	// SEQ_DATA_PORT
	{0x3086, 0x3627}, 	// SEQ_DATA_PORT
	{0x3086, 0x083D}, 	// SEQ_DATA_PORT
	{0x3086, 0x6444}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4B00}, 	// SEQ_DATA_PORT
	{0x3086, 0x432D}, 	// SEQ_DATA_PORT
	{0x3086, 0x6343}, 	// SEQ_DATA_PORT
	{0x3086, 0x1664}, 	// SEQ_DATA_PORT
	{0x3086, 0x435F}, 	// SEQ_DATA_PORT
	{0x3086, 0x4F50}, 	// SEQ_DATA_PORT
	{0x3086, 0x2604}, 	// SEQ_DATA_PORT
	{0x3086, 0x2694}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FC}, 	// SEQ_DATA_PORT
	{0x3086, 0x530D}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C0D}, 	// SEQ_DATA_PORT
	{0x3086, 0x5754}, 	// SEQ_DATA_PORT
	{0x3086, 0x1709}, 	// SEQ_DATA_PORT
	{0x3086, 0x5556}, 	// SEQ_DATA_PORT
	{0x3086, 0x4953}, 	// SEQ_DATA_PORT
	{0x3086, 0x0753}, 	// SEQ_DATA_PORT
	{0x3086, 0x034D}, 	// SEQ_DATA_PORT
	{0x3086, 0x286C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4C09}, 	// SEQ_DATA_PORT
	{0x3086, 0x282C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2828}, 	// SEQ_DATA_PORT
	{0x3086, 0x261C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4E5C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0960}, 	// SEQ_DATA_PORT
	{0x3086, 0x4500}, 	// SEQ_DATA_PORT
	{0x3086, 0x4580}, 	// SEQ_DATA_PORT
	{0x3086, 0x26BE}, 	// SEQ_DATA_PORT
	{0x3086, 0x27F8}, 	// SEQ_DATA_PORT
	{0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C0B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1712}, 	// SEQ_DATA_PORT
	{0x3086, 0x26BA}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C03}, 	// SEQ_DATA_PORT
	{0x3086, 0x1713}, 	// SEQ_DATA_PORT
	{0x3086, 0x27F2}, 	// SEQ_DATA_PORT
	{0x3086, 0x171C}, 	// SEQ_DATA_PORT
	{0x3086, 0x5F28}, 	// SEQ_DATA_PORT
	{0x3086, 0x0817}, 	// SEQ_DATA_PORT
	{0x3086, 0x0360}, 	// SEQ_DATA_PORT
	{0x3086, 0x173C}, 	// SEQ_DATA_PORT
	{0x3086, 0x26B2}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x5F4D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1926}, 	// SEQ_DATA_PORT
	{0x3086, 0x9316}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0xFA45}, 	// SEQ_DATA_PORT
	{0x3086, 0xA017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0527}, 	// SEQ_DATA_PORT
	{0x3086, 0xFB17}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F45}, 	// SEQ_DATA_PORT
	{0x3086, 0x801F}, 	// SEQ_DATA_PORT
	{0x3086, 0x1705}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{0x3086, 0x171E}, 	// SEQ_DATA_PORT
	{0x3086, 0x5D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0C26}, 	// SEQ_DATA_PORT
	{0x3086, 0x9248}, 	// SEQ_DATA_PORT
	{0x3086, 0x4D4E}, 	// SEQ_DATA_PORT
	{0x3086, 0x269A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2808}, 	// SEQ_DATA_PORT
	{0x3086, 0x4C0B}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0327}, 	// SEQ_DATA_PORT
	{0x3086, 0xF217}, 	// SEQ_DATA_PORT
	{0x3086, 0x2626}, 	// SEQ_DATA_PORT
	{0x3086, 0x9216}, 	// SEQ_DATA_PORT
	{0x3086, 0x165F}, 	// SEQ_DATA_PORT
	{0x3086, 0x4D19}, 	// SEQ_DATA_PORT
	{0x3086, 0x2693}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{0x3086, 0x2643}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C01}, 	// SEQ_DATA_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{0x3086, 0x1720}, 	// SEQ_DATA_PORT
	{0x3086, 0x4A65}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x6643}, 	// SEQ_DATA_PORT
	{0x3086, 0x5A43}, 	// SEQ_DATA_PORT
	{0x3086, 0x165B}, 	// SEQ_DATA_PORT
	{0x3086, 0x4327}, 	// SEQ_DATA_PORT
	{0x3086, 0x9C45}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0627}, 	// SEQ_DATA_PORT
	{0x3086, 0x9D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x1C45}, 	// SEQ_DATA_PORT
	{0x3086, 0x4023}, 	// SEQ_DATA_PORT
	{0x3086, 0x1705}, 	// SEQ_DATA_PORT
	{0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{0x3086, 0x5D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x4417}, 	// SEQ_DATA_PORT
	{0x3086, 0x0E28}, 	// SEQ_DATA_PORT
	{0x3086, 0x0053}, 	// SEQ_DATA_PORT
	{0x3086, 0x014B}, 	// SEQ_DATA_PORT
	{0x3086, 0x5251}, 	// SEQ_DATA_PORT
	{0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{0x3086, 0x4B01}, 	// SEQ_DATA_PORT
	{0x3086, 0x432D}, 	// SEQ_DATA_PORT
	{0x3086, 0x4643}, 	// SEQ_DATA_PORT
	{0x3086, 0x1647}, 	// SEQ_DATA_PORT
	{0x3086, 0x434F}, 	// SEQ_DATA_PORT
	{0x3086, 0x5026}, 	// SEQ_DATA_PORT
	{0x3086, 0x0426}, 	// SEQ_DATA_PORT
	{0x3086, 0x8427}, 	// SEQ_DATA_PORT
	{0x3086, 0xFC53}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D5C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D57}, 	// SEQ_DATA_PORT
	{0x3086, 0x5417}, 	// SEQ_DATA_PORT
	{0x3086, 0x0955}, 	// SEQ_DATA_PORT
	{0x3086, 0x5649}, 	// SEQ_DATA_PORT
	{0x3086, 0x5307}, 	// SEQ_DATA_PORT
	{0x3086, 0x5303}, 	// SEQ_DATA_PORT
	{0x3086, 0x4D28}, 	// SEQ_DATA_PORT
	{0x3086, 0x6C4C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0928}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C28}, 	// SEQ_DATA_PORT
	{0x3086, 0x2826}, 	// SEQ_DATA_PORT
	{0x3086, 0x0C4E}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C09}, 	// SEQ_DATA_PORT
	{0x3086, 0x6045}, 	// SEQ_DATA_PORT
	{0x3086, 0x0045}, 	// SEQ_DATA_PORT
	{0x3086, 0x8026}, 	// SEQ_DATA_PORT
	{0x3086, 0xAE27}, 	// SEQ_DATA_PORT
	{0x3086, 0xF817}, 	// SEQ_DATA_PORT
	{0x3086, 0x0227}, 	// SEQ_DATA_PORT
	{0x3086, 0xFA5C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0B17}, 	// SEQ_DATA_PORT
	{0x3086, 0x1226}, 	// SEQ_DATA_PORT
	{0x3086, 0xAA5C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0317}, 	// SEQ_DATA_PORT
	{0x3086, 0x0B27}, 	// SEQ_DATA_PORT
	{0x3086, 0xF217}, 	// SEQ_DATA_PORT
	{0x3086, 0x265F}, 	// SEQ_DATA_PORT
	{0x3086, 0x2808}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0326}, 	// SEQ_DATA_PORT
	{0x3086, 0xA216}, 	// SEQ_DATA_PORT
	{0x3086, 0x165F}, 	// SEQ_DATA_PORT
	{0x3086, 0x4D1A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2683}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{0x3086, 0x45A0}, 	// SEQ_DATA_PORT
	{0x3086, 0x1705}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FB}, 	// SEQ_DATA_PORT
	{0x3086, 0x171F}, 	// SEQ_DATA_PORT
	{0x3086, 0x4580}, 	// SEQ_DATA_PORT
	{0x3086, 0x2017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0527}, 	// SEQ_DATA_PORT
	{0x3086, 0xFA17}, 	// SEQ_DATA_PORT
	{0x3086, 0x1E5D}, 	// SEQ_DATA_PORT
	{0x3086, 0x170C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2682}, 	// SEQ_DATA_PORT
	{0x3086, 0x484D}, 	// SEQ_DATA_PORT
	{0x3086, 0x4E26}, 	// SEQ_DATA_PORT
	{0x3086, 0x8A28}, 	// SEQ_DATA_PORT
	{0x3086, 0x084C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0B60}, 	// SEQ_DATA_PORT
	{0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{0x3086, 0x27F2}, 	// SEQ_DATA_PORT
	{0x3086, 0x1738}, 	// SEQ_DATA_PORT
	{0x3086, 0x2682}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x5F4D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{0x3086, 0x8316}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0xFA26}, 	// SEQ_DATA_PORT
	{0x3086, 0x435C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0145}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x9817}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F4A}, 	// SEQ_DATA_PORT
	{0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{0x3086, 0x0343}, 	// SEQ_DATA_PORT
	{0x3086, 0x1604}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x5843}, 	// SEQ_DATA_PORT
	{0x3086, 0x1659}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{0x3086, 0x1705}, 	// SEQ_DATA_PORT
	{0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{0x3086, 0x171D}, 	// SEQ_DATA_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x2217}, 	// SEQ_DATA_PORT
	{0x3086, 0x0527}, 	// SEQ_DATA_PORT
	{0x3086, 0x985D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2645}, 	// SEQ_DATA_PORT
	{0x3086, 0x170E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2800}, 	// SEQ_DATA_PORT
	{0x3086, 0x5301}, 	// SEQ_DATA_PORT
	{0x3086, 0x4B52}, 	// SEQ_DATA_PORT
	{0x3086, 0x5112}, 	// SEQ_DATA_PORT
	{0x3086, 0x4460}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
};

/* Version 1.0.0.1 modification, 2012.04.02 */
const TAR0331RegAddrData atAR0331LinearSequenceTbl[AR0331_LINEAR_SEQUENCE_TBL_SIZE] = 
{
	// [AR0331 Linear sequencer load - 1.0]
	{0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{0x3086, 0x4A03}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x0443}, 	// SEQ_DATA_PORT
	{0x3086, 0x1645}, 	// SEQ_DATA_PORT
	{0x3086, 0x4045}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0x5045}, 	// SEQ_DATA_PORT
	{0x3086, 0x404B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{0x3086, 0x6134}, 	// SEQ_DATA_PORT
	{0x3086, 0x4A31}, 	// SEQ_DATA_PORT
	{0x3086, 0x4342}, 	// SEQ_DATA_PORT
	{0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{0x3086, 0x2714}, 	// SEQ_DATA_PORT
	{0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{0x3086, 0x3DEA}, 	// SEQ_DATA_PORT
	{0x3086, 0x2704}, 	// SEQ_DATA_PORT
	{0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{0x3086, 0x2705}, 	// SEQ_DATA_PORT
	{0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{0x3086, 0x2715}, 	// SEQ_DATA_PORT
	{0x3086, 0x3527}, 	// SEQ_DATA_PORT
	{0x3086, 0x053D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1045}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0427}, 	// SEQ_DATA_PORT
	{0x3086, 0x143D}, 	// SEQ_DATA_PORT
	{0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{0x3086, 0xEA62}, 	// SEQ_DATA_PORT
	{0x3086, 0x2728}, 	// SEQ_DATA_PORT
	{0x3086, 0x3627}, 	// SEQ_DATA_PORT
	{0x3086, 0x083D}, 	// SEQ_DATA_PORT
	{0x3086, 0x6444}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4B01}, 	// SEQ_DATA_PORT
	{0x3086, 0x432D}, 	// SEQ_DATA_PORT
	{0x3086, 0x4643}, 	// SEQ_DATA_PORT
	{0x3086, 0x1647}, 	// SEQ_DATA_PORT
	{0x3086, 0x435F}, 	// SEQ_DATA_PORT
	{0x3086, 0x4F50}, 	// SEQ_DATA_PORT
	{0x3086, 0x2604}, 	// SEQ_DATA_PORT
	{0x3086, 0x2694}, 	// SEQ_DATA_PORT
	{0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{0x3086, 0xFC53}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D5C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D57}, 	// SEQ_DATA_PORT
	{0x3086, 0x5417}, 	// SEQ_DATA_PORT
	{0x3086, 0x0955}, 	// SEQ_DATA_PORT
	{0x3086, 0x5649}, 	// SEQ_DATA_PORT
	{0x3086, 0x5307}, 	// SEQ_DATA_PORT
	{0x3086, 0x5303}, 	// SEQ_DATA_PORT
	{0x3086, 0x4D28}, 	// SEQ_DATA_PORT
	{0x3086, 0x6C4C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0928}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C28}, 	// SEQ_DATA_PORT
	{0x3086, 0x294E}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C09}, 	// SEQ_DATA_PORT
	{0x3086, 0x4500}, 	// SEQ_DATA_PORT
	{0x3086, 0x4580}, 	// SEQ_DATA_PORT
	{0x3086, 0x26B6}, 	// SEQ_DATA_PORT
	{0x3086, 0x27F8}, 	// SEQ_DATA_PORT
	{0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C0B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1718}, 	// SEQ_DATA_PORT
	{0x3086, 0x26B2}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C03}, 	// SEQ_DATA_PORT
	{0x3086, 0x1744}, 	// SEQ_DATA_PORT
	{0x3086, 0x27F2}, 	// SEQ_DATA_PORT
	{0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{0x3086, 0x2809}, 	// SEQ_DATA_PORT
	{0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{0x3086, 0x1628}, 	// SEQ_DATA_PORT
	{0x3086, 0x084D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{0x3086, 0x9316}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0xFA45}, 	// SEQ_DATA_PORT
	{0x3086, 0xA017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0727}, 	// SEQ_DATA_PORT
	{0x3086, 0xFB17}, 	// SEQ_DATA_PORT
	{0x3086, 0x2945}, 	// SEQ_DATA_PORT
	{0x3086, 0x8017}, 	// SEQ_DATA_PORT
	{0x3086, 0x0827}, 	// SEQ_DATA_PORT
	{0x3086, 0xFA17}, 	// SEQ_DATA_PORT
	{0x3086, 0x285D}, 	// SEQ_DATA_PORT
	{0x3086, 0x170E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2691}, 	// SEQ_DATA_PORT
	{0x3086, 0x5301}, 	// SEQ_DATA_PORT
	{0x3086, 0x1740}, 	// SEQ_DATA_PORT
	{0x3086, 0x5302}, 	// SEQ_DATA_PORT
	{0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{0x3086, 0x2693}, 	// SEQ_DATA_PORT
	{0x3086, 0x2692}, 	// SEQ_DATA_PORT
	{0x3086, 0x484D}, 	// SEQ_DATA_PORT
	{0x3086, 0x4E28}, 	// SEQ_DATA_PORT
	{0x3086, 0x094C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0B17}, 	// SEQ_DATA_PORT
	{0x3086, 0x5F27}, 	// SEQ_DATA_PORT
	{0x3086, 0xF217}, 	// SEQ_DATA_PORT
	{0x3086, 0x1428}, 	// SEQ_DATA_PORT
	{0x3086, 0x0816}, 	// SEQ_DATA_PORT
	{0x3086, 0x4D1A}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{0x3086, 0x2603}, 	// SEQ_DATA_PORT
	{0x3086, 0x5C01}, 	// SEQ_DATA_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{0x3086, 0x172A}, 	// SEQ_DATA_PORT
	{0x3086, 0x4A0A}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x0B43}, 	// SEQ_DATA_PORT
	{0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1725}, 	// SEQ_DATA_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x1708}, 	// SEQ_DATA_PORT
	{0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{0x3086, 0x5D53}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x455C}, 	// SEQ_DATA_PORT
	{0x3086, 0x014B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{0x3086, 0x5251}, 	// SEQ_DATA_PORT
	{0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{0x3086, 0x6018}, 	// SEQ_DATA_PORT
	{0x3086, 0x4A03}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x0443}, 	// SEQ_DATA_PORT
	{0x3086, 0x1658}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x5943}, 	// SEQ_DATA_PORT
	{0x3086, 0x165A}, 	// SEQ_DATA_PORT
	{0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{0x3086, 0x5B43}, 	// SEQ_DATA_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1725}, 	// SEQ_DATA_PORT
	{0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{0x3086, 0x1720}, 	// SEQ_DATA_PORT
	{0x3086, 0x224B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
};
/* ======================================== */

const TAR0331RegAddrData atAR0331HiSpiHDRTbl[AR0331_HISPI_HDR_TBL_SIZE] =
{
	{0x30B0, 0x0000}, 	// DIGITAL_TEST
	{0x30BA, 0x06EC}, 	// DIGITAL_CTRL
	{0x31AC, 0x100C}, 	// DATA_FORMAT_BITS
	{0x302A, 0x0006}, 	// VT_PIX_CLK_DIV
	{0x302C, 0x0001}, 	// VT_SYS_CLK_DIV
	{0x302E, 0x0004}, 	// PRE_PLL_CLK_DIV
	{0x3030, 0x0042}, 	// PLL_MULTIPLIER
	{0x3036, 0x000C}, 	// OP_PIX_CLK_DIV
	{0x3038, 0x0001}, 	// OP_SYS_CLK_DIV
/* Version 1.0.0.1 modification, 2012.04.02 */
/* ======================================== */
	{0x300A, 0x0465}, 	// FRAME_LENGTH_LINES
	{0x300C, 0x044C}, 	// LINE_LENGTH_PCK

	{0x3012, 0x0409}, 	// COARSE_INTEGRATION_TIME
	{0x30A2, 0x0001}, 	// X_ODD_INC
	{0x30A6, 0x0001}, 	// Y_ODD_INC
	{0x3040, 0x0000}, 	// READ_MODE
	{0x3082, 0x0008}, 	// OPERATION_MODE_CTRL
	{0x305E, 0x0080}, 	// GLOBAL_GAIN
	//DELAY=60
	{0x3082, 0x0008}, 	// OPERATION_MODE_CTRL

/* Version 1.0.0.3 modification, 2012.09.26 */
/* ======================================== */
	{0x2410, 0x0010}, 	// ALTM_POWER_GAIN
	{0x2412, 0x0010}, 	// ALTM_POWER_OFFSET
	{0x2442, 0x0080}, 	// ALTM_CONTROL_KEY_K0
	{0x2444, 0x0000}, 	// ALTM_CONTROL_KEY_K01_LO
	{0x2446, 0x0004}, 	// ALTM_CONTROL_KEY_K01_HI
	{0x2440, 0x0002}, 	// ALTM_CONTROL_DAMPER
	{0x2450, 0x0000}, 	// ALTM_OUT_PEDESTAL
	{0x2438, 0x0010}, 	// ALTM_CONTROL_MIN_FACTOR
	{0x243A, 0x0020}, 	// ALTM_CONTROL_MAX_FACTOR
	{0x243C, 0x0000}, 	// ALTM_CONTROL_DARK_FLOOR
	{0x243E, 0x0200}, 	// ALTM_CONTROL_BRIGHT_FLOOR
	{0x31D0, 0x0000}, 	// COMPANDING
	{0x301E, 0x0010}, 	// DATA_PEDESTAL
	{0x2400, 0x0002}, 	// ALTM_CONTROL
	{0x3202, 0x00CF}, 	// ADACD_NOISE_MODEL1
	{0x3206, 0x0A06}, 	// RESERVED_MFR_3206
	{0x3208, 0x1A12}, 	// RESERVED_MFR_3208
	{0x3200, 0x0002}, 	// ADACD_CONTROL
	{0x31E0, 0x0200}, 	// PIX_DEF_ID
	{0x3060, 0x0006}, 	// ANALOG_GAIN
	{0x318A, 0x0E10}, 	// HDR_MC_CTRL1
	{0x3064, 0x1982}, 	// SMIA_TEST
	{0x3064, 0x1982}, 	// SMIA_TEST
	{0x3202, 0x00CF}, 	// ADACD_NOISE_MODEL1
	{0x3206, 0x0A06}, 	// RESERVED_MFR_3206
	{0x3208, 0x1A12}, 	// RESERVED_MFR_3208
	{0x3200, 0x0002}, 	// ADACD_CONTROL
	{0x31AE, 0x0304}, 	// SERIAL_FORMAT
	{0x306E, 0x9010}, 	// DATAPATH_SELECT
	{0x3180, 0x8089}, 	// DELTA_DK_CONTROL
	{0x30F4, 0x4000}, 	// RESERVED_MFR_30F4

/* Version 1.0.0.3 modification, 2012.09.26 */
	/* enable Anti-blomming */ 
	{0x3ED4, 0x8F6C}, 	// RESERVED_MFR_3ED4
	{0x3ED6, 0x66CC}, 	// RESERVED_MFR_3ED6
	{0x3ED8, 0x8C42}, 	// RESERVED_MFR_3ED8
	/*****************/
/* ======================================== */

	{0x3EDA, 0x8899}, 	// RESERVED_MFR_3EDA
	{0x3EE6, 0x00F0}, 	// RESERVED_MFR_3EE6
	{0x3ED2, 0x1F46}, 	// DAC_LD_6_7
};

/* Version 1.0.0.1 modification, 2012.04.02 */
const TAR0331RegAddrData atAR0331HiSpiLinearTbl[AR0331_HISPI_LINEAR_TBL_SIZE] =
{ 
	{0x30B0, 0x0000}, 	// DIGITAL_TEST
	{0x30BA, 0x06EC}, 	// DIGITAL_CTRL
	{0x31AC, 0x100C}, 	// DATA_FORMAT_BITS
	{0x302A, 0x0006}, 	// VT_PIX_CLK_DIV
	{0x302C, 0x0001}, 	// VT_SYS_CLK_DIV
	{0x302E, 0x0004}, 	// PRE_PLL_CLK_DIV
	{0x3030, 0x0042}, 	// PLL_MULTIPLIER
	{0x3036, 0x000C}, 	// OP_PIX_CLK_DIV
	{0x3038, 0x0001}, 	// OP_SYS_CLK_DIV

	{0x300A, 0x0465}, 	// FRAME_LENGTH_LINES
	{0x300C, 0x044C}, 	// LINE_LENGTH_PCK
	{0x3012, 0x0409}, 	// COARSE_INTEGRATION_TIME

	{0x30A2, 0x0001}, 	// X_ODD_INC
	{0x30A6, 0x0001}, 	// Y_ODD_INC

	{0x3040, 0x0000}, 	// READ_MODE
	{0x3082, 0x0009}, 	// OPERATION_MODE_CTRL
	{0x305E, 0x0080}, 	// GLOBAL_GAIN
	//DELAY=60
	{0x3082, 0x0008}, 	// OPERATION_MODE_CTRL
	{0x318C, 0xC001}, 	// HDR_MC_CTRL2
	{0x3198, 0x061E}, 	// HDR_MC_CTRL8
	{0x301E, 0x0000}, 	// DATA_PEDESTAL
	{0x30FE, 0x0000}, 	// RESERVED_MFR_30FE
	{0x320A, 0x0000}, 	// ADACD_PEDESTAL
	{0x2410, 0x0010}, 	// RESERVED_MFR_2410
	{0x2412, 0x0010}, 	// RESERVED_MFR_2412
	{0x2442, 0x0080}, 	// ALTM_CONTROL_KEY_K0
	{0x2444, 0x0000}, 	// ALTM_CONTROL_KEY_K01_LO
	{0x2446, 0x0004}, 	// ALTM_CONTROL_KEY_K01_HI
	{0x2440, 0x0002}, 	// ALTM_CONTROL_DAMPER
	{0x2450, 0x0000}, 	// ALTM_OUT_PEDESTAL
	{0x2438, 0x0010}, 	// ALTM_CONTROL_MIN_FACTOR
	{0x243A, 0x0020}, 	// ALTM_CONTROL_MAX_FACTOR
	{0x243C, 0x0000}, 	// ALTM_CONTROL_DARK_FLOOR
	{0x243E, 0x0200}, 	// ALTM_CONTROL_BRIGHT_FLOOR
	{0x31D0, 0x0000}, 	// COMPANDING
	{0x301E, 0x0000}, 	// DATA_PEDESTAL
	{0x2400, 0x0002}, 	// ALTM_CONTROL
	{0x3202, 0x00A0}, 	// ADACD_NOISE_MODEL1
	{0x3206, 0x1810}, 	// RESERVED_MFR_3206
	{0x3208, 0x2820}, 	// RESERVED_MFR_3208
	{0x3200, 0x0002}, 	// ADACD_CONTROL
	{0x31E0, 0x0200}, 	// RESERVED_MFR_31E0
	{0x3060, 0x0006}, 	// ANALOG_GAIN
	{0x318A, 0x0E10}, 	// HDR_MC_CTRL1
	{0x3064, 0x1982}, 	// SMIA_TEST
	{0x3064, 0x1982}, 	// SMIA_TEST
	{0x3202, 0x00A0}, 	// ADACD_NOISE_MODEL1
	{0x3206, 0x1810}, 	// RESERVED_MFR_3206
	{0x3208, 0x2820}, 	// RESERVED_MFR_3208
	{0x3200, 0x0002}, 	// ADACD_CONTROL
	{0x31AE, 0x0304}, 	// SERIAL_FORMAT
	{0x306E, 0x9010}, 	// DATAPATH_SELECT
	{0x3180, 0x8089}, 	// DELTA_DK_CONTROL
	{0x30F4, 0x4000}, 	// RESERVED_MFR_30F4
	{0x3ED4, 0x8F6C}, 	// RESERVED_MFR_3ED4
	{0x3ED6, 0x6666}, 	// RESERVED_MFR_3ED6
	{0x3EDA, 0x8899}, 	// RESERVED_MFR_3EDA
	{0x3EE6, 0x00F0}, 	// RESERVED_MFR_3EE6
	{0x3ED2, 0x9F46}, 	// DAC_LD_6_7
	//DELAY=33
	{0x31C6, 0x0400}, 	// HISPI_CONTROL_STATUS
	{0x3012, 0x0230}, 	// COARSE_INTEGRATION_TIME
	{0x318C, 0x0000}, 	// HDR_MC_CTRL2
	{0x3190, 0x0000}, 	// HDR_MC_CTRL4
	{0x301E, 0x00A8}, 	// DATA_PEDESTAL
	{0x30FE, 0x0080}, 	// RESERVED_MFR_30FE
	{0x320A, 0x0080}, 	// ADACD_PEDESTAL
};
/* ======================================== */
/* Version 1.0.0.9 modification, 2013.07.01 */
const TAR0331RegAddrData atAR0331ParallelHDRTbl[AR0331_PARALLEL_HDR_TBL_SIZE] =
{
	{0x30B0, 0x0000}, 	// DIGITAL_TEST 			
	{0x30BA, 0x06EC}, 	// DIGITAL_CTRL		
	{0x31AC, 0x100C}, 	// DATA_FORMAT_BITS

	// [PLL_settings - Parallel]
	{0x302A, 0x0008}, 	// VT_PIX_CLK_DIV 		
	{0x302C, 0x0001}, 	// VT_SYS_CLK_DIV 		
	{0x302E, 0x0002}, 	// PRE_PLL_CLK_DIV 		
	{0x3030, 0x002C}, 	// PLL_MULTIPLIER 		
	{0x3036, 0x000C}, 	// OP_PIX_CLK_DIV 		
	{0x3038, 0x0001}, 	// OP_SYS_CLK_DIV

	// [Parallel HDR 1080p30]			
	{0x3002, 0x00E4}, 	// Y_ADDR_START 			
	{0x3004, 0x0042}, 	// X_ADDR_START 			
	{0x3006, 0x0523}, 	// Y_ADDR_END 			
	{0x3008, 0x07C9}, 	// X_ADDR_END 		
	{0x300A, 0x0465}, 	// FRAME_LENGTH_LINES 		
	{0x300C, 0x044C}, 	// LINE_LENGTH_PCK	
	{0x3012, 0x0232}, 	// COARSE_INTEGRATION_TIME 			
	{0x30A2, 0x0001}, 	// X_ODD_INC 			
	{0x30A6, 0x0001}, 	// Y_ODD_INC 			
	{0x3040, 0x0000}, 	// READ_MODE 		
	{0x31AE, 0x0301}, 	// SERIAL_FORMAT

	// [HDR Mode 16x]	
	{0x3082, 0x0008}, 	// OPERATION_MODE_CTRL			
	{0x305E, 0x0080}, 	// GLOBAL_GAIN

	//DELAY=60
	// [HDR Mode Setup]
	{0x3082, 0x0008}, 	// OPERATION_MODE_CTRL

	// [DLO enabled]
	// [2D motion compensation OFF]			
	{0x318C, 0x0000}, 	// HDR_MC_CTRL2			
	{0x3190, 0x0000}, 	// HDR_MC_CTRL4 
	{0x30FE, 0x0080}, 	// RESERVED_MFR_30FE		
	{0x320A, 0x0080}, 	// ADACD_PEDESTAL
	// [DLO enabled]			
	{0x3190, 0xE000}, 	// HDR_MC_CTRL4

	// [ALTM Enabled]		
	{0x2410, 0x0010}, 	// RESERVED_MFR_2410 
	{0x2412, 0x0010}, 	// RESERVED_MFR_2412		
	{0x2442, 0x0080}, 	// ALTM_CONTROL_KEY_K0	
	{0x2444, 0x0000}, 	// ALTM_CONTROL_KEY_K01_LO	
	{0x2446, 0x0004}, 	// ALTM_CONTROL_KEY_K01_HI		
	{0x2440, 0x0002}, 	// ALTM_CONTROL_DAMPER		
	{0x2450, 0x0000}, 	// ALTM_OUT_PEDESTAL	
	{0x2438, 0x0010}, 	// ALTM_CONTROL_MIN_FACTOR	
	{0x243A, 0x0020}, 	// ALTM_CONTROL_MAX_FACTOR	
	{0x243C, 0x0000}, 	// ALTM_CONTROL_DARK_FLOOR	
	{0x243E, 0x0200}, 	// ALTM_CONTROL_BRIGHT_FLOOR 			
	{0x31D0, 0x0000}, 	// COMPANDING			
	{0x301E, 0x0010}, 	// DATA_PEDESTAL
	{0x2400, 0x0002}, 	// ALTM_CONTROL			

	// [ADACD Enabled]			
	{0x3202, 0x00CF}, 	// ADACD_NOISE_MODEL1				
	{0x3206, 0x0A06}, 	// RESERVED_MFR_3206				
	{0x3208, 0x1A12}, 	// RESERVED_MFR_3208 		
	{0x3200, 0x0002}, 	// ADACD_CONTROL

	// [HDR Mode Setup]
	{0x31E0, 0x0200}, 	// RESERVED_MFR_31E0			
	{0x3060, 0x0006}, 	// ANALOG_GAIN			
	{0x318A, 0x0E10}, 	// HDR_MC_CTRL1

	// [Enable Embedded Data and Stats]
	{0x3064, 0x1982}, 	// SMIA_TEST	

	// [Analog Settings]	
	{0x3180, 0x8089}, 	// DELTA_DK_CONTROL
	{0x30F4, 0x4000}, 	// RESERVED_MFR_30F4
	{0x3ED4, 0x8F6C}, 	// RESERVED_MFR_3ED4
	{0x3ED6, 0x66CC}, 	// RESERVED_MFR_3ED6
	{0x3ED8, 0x8C42}, 	// RESERVED_MFR_3ED8
	{0x3EDA, 0x8899}, 	// RESERVED_MFR_3EDA
	{0x3EE6, 0x00F0}, 	// RESERVED_MFR_3EE6

	{0x3ED2, 0x1F46} 	// DAC_LD_6_7 		
};

const TAR0331RegAddrData atAR0331ParallelLinearTbl[AR0331_PARALLEL_LINEAR_TBL_SIZE] =
{ 
	//{0x301A, 0x10D8}, 	// RESET_REGISTER 			
	{0x30B0, 0x0000}, 	// DIGITAL_TEST 			
	{0x30BA, 0x06EC}, 	// DIGITAL_CTRL		
	{0x31AC, 0x0C0C}, 	// DATA_FORMAT_BITS

	// [PLL_settings - Parallel]		
	{0x302A, 0x0008}, 	// VT_PIX_CLK_DIV 		
	{0x302C, 0x0001}, 	// VT_SYS_CLK_DIV 		
	{0x302E, 0x0002}, 	// PRE_PLL_CLK_DIV 		
	{0x3030, 0x002C}, 	// PLL_MULTIPLIER 		
	{0x3036, 0x000C}, 	// OP_PIX_CLK_DIV 		
	{0x3038, 0x0001}, 	// OP_SYS_CLK_DIV
		
	{0x3002, 0x00E4}, 	// Y_ADDR_START 			
	{0x3004, 0x0042}, 	// X_ADDR_START 			
	{0x3006, 0x0523}, 	// Y_ADDR_END 			
	{0x3008, 0x07C9}, 	// X_ADDR_END 		
	{0x300A, 0x0465}, 	// FRAME_LENGTH_LINES 		
	{0x300C, 0x044C}, 	// LINE_LENGTH_PCK 	
	{0x3012, 0x0416}, 	// COARSE_INTEGRATION_TIME 			
	{0x30A2, 0x0001}, 	// X_ODD_INC 			
	{0x30A6, 0x0001}, 	// Y_ODD_INC 			
	{0x3040, 0x0000}, 	// READ_MODE 		
	{0x31AE, 0x0301}, 	// SERIAL_FORMAT

	// [Linear Mode]
	{0x3082, 0x0009}, 	// OPERATION_MODE_CTRL

	// [2D motion compensation OFF]			
	{0x318C, 0x0000}, 	// HDR_MC_CTRL2			
	{0x3190, 0x0000}, 	// HDR_MC_CTRL4 
	{0x30FE, 0x0080}, 	// RESERVED_MFR_30FE		
	{0x320A, 0x0080}, 	// ADACD_PEDESTAL

	// [ALTM Bypassed]	
	{0x2400, 0x0003}, 	// ALTM_CONTROL		
	{0x2450, 0x0000}, 	// ALTM_OUT_PEDESTAL			
	{0x301E, 0x00A8}, 	// DATA_PEDESTAL

	// [ADACD Disabled]
	{0x3200, 0x0000}, 	// ADACD_CONTROL

	// [Companding Disabled]		
	{0x31D0, 0x0000}, 	// COMPANDING 
	{0x31E0, 0x0200}, 	// RESERVED_MFR_31E0			
	{0x3060, 0x0006}, 	// ANALOG_GAIN

	// [Enable Embedded Data and Stats]
	{0x3064, 0x1982}, 	// SMIA_TEST	

	// [Analog Settings]		
	{0x3180, 0x8089}, 	// DELTA_DK_CONTROL
	{0x30F4, 0x4000}, 	// RESERVED_MFR_30F4
	{0x3ED4, 0x8F6C}, 	// RESERVED_MFR_3ED4
	{0x3ED6, 0x66CC}, 	// RESERVED_MFR_3ED6
	{0x3ED8, 0x8C42}, 	// RESERVED_MFR_3ED8
	{0x3EDA, 0x8899}, 	// RESERVED_MFR_3EDA
	{0x3EE6, 0x00F0}, 	// RESERVED_MFR_3EE6			
	{0x3ED2, 0x1F46}, 	// DAC_LD_6_7
};
/* ======================================== */

/* Version 1.0.0.5 modification, 2012.12.03 */
/* Version 1.0.0.8 modification, 2013.05.24 */
const DWORD adwAR0331AnalogGainTbl[23][2] =
{
	{1230,0x0606},
	{1280,0x0707},
	{1340,0x0808},
	{1390,0x0909},
	{1450,0x0A0A},
	{1520,0x0B0B},
	{1600,0x0C0C},
	{1690,0x0D0D},
	{1780,0x0E0E},
	{1880,0x0F0F},
	{2000,0x1010},
	{2140,0x1212},
	{2280,0x1414},
	{2470,0x1616},
	{2670,0x1818},
	{2910,0x1A1A},
	{3200,0x1C1C},
	{3560,0x1E1E},
	{4000,0x2020},
	{4560,0x2424},
	{5340,0x2828},
	{6410,0x2C2C},
	{8000,0x3030},
/* ======================================== */
};
/* ======================================== */

/*==================================================================*/
 static SOCKET AR0331_WriteBuf(TAR0331Info *ptInfo, EAR0331Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BYTE bWaitCmpt, EAR0331DEVICEID eDeviceID)
{
	BYTE abyData[3];

	if (eDeviceID == AR0331_Device_AR0331)
	{
		ptInfo->dwDeviceAddr = AR0331_DEAFULT_DEVICE_ADDR;
	}
	else
	{
		ptInfo->dwDeviceAddr = FPGA_DEAFULT_DEVICE_ADDR;
	}

	abyData[0] = (BYTE)(eRegAddr&0x00FF);
	abyData[1] = (BYTE)((dwData&0xFF00)>>8);
	abyData[2] = (BYTE)(dwData&0x00FF);
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), 3, abyData);
}
#if 0
 static SOCKET AR0331_WriteBuf(TAR0331Info *ptInfo, EAR0331Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen, BYTE bWaitCmpt, EAR0331DEVICEID eDeviceID)
{
	BYTE abyData[20];
	DWORD dwIndex;

	if (eDeviceID == AR0331_Device_AR0331)
	{
		ptInfo->dwDeviceAddr = AR0331_DEAFULT_DEVICE_ADDR;
	}
	else
	{
		ptInfo->dwDeviceAddr = FPGA_DEAFULT_DEVICE_ADDR;
	}

	abyData[0] = (BYTE)(eRegAddr&0x00FF);

	for (dwIndex = 0; dwIndex < dwDataLen; dwIndex++)
	{
		abyData[(2*dwIndex)+1] = (BYTE)((pdwData[dwIndex]&0xFF00)>>8);
		abyData[(2*dwIndex)+2] = (BYTE)(pdwData[dwIndex]&0x00FF);
	}
	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), (2*dwDataLen+1), abyData);
}
#endif
/*-------------------------------------------------------------------------------------*/ 
static SOCKET AR0331_ReadBuf(TAR0331Info *ptInfo, EAR0331Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen, EAR0331DEVICEID eDeviceID)
{
	BYTE abyData[30];
	DWORD dwIndex;

	if (eDeviceID == AR0331_Device_AR0331)
	{
		ptInfo->dwDeviceAddr = AR0331_DEAFULT_DEVICE_ADDR;
	}
	else
	{
		ptInfo->dwDeviceAddr = FPGA_DEAFULT_DEVICE_ADDR;
	}

	abyData[0] = (BYTE)(eRegAddr&0x00FF);
	abyData[1] = 0;
	if (ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, ((eRegAddr&0xFF00)>>8), (2*dwDataLen), abyData) < 0)
	{
		return S_FAIL;
	}
	else
	{
		for (dwIndex = 0; dwIndex < dwDataLen; dwIndex++)
		{
			pdwData[dwIndex] = (abyData[2*dwIndex+1]&0xFF) + ((abyData[2*dwIndex]<<8)&0xFF00);
		}		
		return S_OK;
	}
}

/*-------------------------------------------------------------------------------------*/
static SOCKET AR0331_CompBuf(TAR0331Info *ptInfo, EAR0331Regs eRegAddr, DWORD dwMask, DWORD dwData, EAR0331DEVICEID eDeviceID)
{
	DWORD dwReadData;

	AR0331_ReadBuf(ptInfo, eRegAddr, &dwReadData, 1, eDeviceID);
	return ((dwReadData&dwMask) == dwData);
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.1 modification, 2012.04.02 */
SOCKET AR0331_SetSize(DWORD dwWidth, DWORD dwHeight)
{
/* Version 1.0.0.7 modification, 2013.02.05 */
	if ((dwWidth==1920) && (dwHeight==1080))
/* ======================================== */
	{
/* Version 1.0.0.2 modification, 2012.08.06 */
		AR0331_WriteBuf(ptInfo, _3002_YAddrStart, 0x00EC, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
		AR0331_WriteBuf(ptInfo, _3004_XAddrStart, 0x0042, 1, 0, AR0331_Device_AR0331);
/* Version 1.0.0.2 modification, 2012.08.06 */
		AR0331_WriteBuf(ptInfo, _3006_YAddrEnd, 0x052B, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
		AR0331_WriteBuf(ptInfo, _3008_XAddrEnd, 0x07C9, 1, 0, AR0331_Device_AR0331);

		// Set statistic window
		AR0331_WriteBuf(ptInfo, _3140_AERoiXStartOffset, 0x0042, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3142_AERoiYStartOffset, 0x00E4, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3144_AERoiXSize, 0x07C8, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3146_AERoiYSize, 0x0522, 1, 0, AR0331_Device_AR0331);
/* Version 1.0.0.7 modification, 2013.02.05 */
		ptInfo->dwWidth = dwWidth;
		ptInfo->dwHeight = dwHeight;
/* ======================================== */
	}
/* Version 1.0.0.7 modification, 2013.02.05 */
	else if ((dwWidth==2048) && (dwHeight==1536))
/* ======================================== */
	{
/* Version 1.0.0.2 modification, 2012.08.06 */
		AR0331_WriteBuf(ptInfo, _3002_YAddrStart, 0x0000, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
		AR0331_WriteBuf(ptInfo, _3004_XAddrStart, 0x0006, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3006_YAddrEnd, 0x0603, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3008_XAddrEnd, 0x0805, 1, 0, AR0331_Device_AR0331);

		// Set statistic window
		AR0331_WriteBuf(ptInfo, _3140_AERoiXStartOffset, 0x0006, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3142_AERoiYStartOffset, 0x0004, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3144_AERoiXSize, 0x0805, 1, 0, AR0331_Device_AR0331);
		AR0331_WriteBuf(ptInfo, _3146_AERoiYSize, 0x0603, 1, 0, AR0331_Device_AR0331);
/* Version 1.0.0.7 modification, 2013.02.05 */
		ptInfo->dwWidth = dwWidth;
		ptInfo->dwHeight = dwHeight;
/* ======================================== */
	}
	else
	{
		printk("[AR0331] : Size is not supported (width=%d, height=%d).\n", (int)ptInfo->dwWidth, (int)ptInfo->dwHeight);
	}
	return 0;
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.9 modification, 2013.07.01 */
static void AR0331_UpdateIntTimeLimit(TAR0331Info *ptInfo)
{
	if (ptInfo->dwWDRRatio > 1)
	{
		ptInfo->dwMaxIntTime = ((70*ptInfo->dwWDRRatio)<=(dwAR0331Vsync-71))
								? (70*ptInfo->dwWDRRatio)
								: (dwAR0331Vsync-71);

		ptInfo->dwMinIntTime = ptInfo->dwWDRRatio>>1;
	}
	else
	{
		ptInfo->dwMaxIntTime = 0xFFFF;
		ptInfo->dwMinIntTime = 10;
	}
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static DWORD AR0331_Interpolate(SDWORD sdwValueA,
										SDWORD sdwValueB, 
										SDWORD sdwValueC, 
										SDWORD sdwValueD, 
										SDWORD sdwValueE)
{
	if (((sdwValueC>=sdwValueD)&&(sdwValueD>=sdwValueA)) ||
		((sdwValueC<=sdwValueD)&&(sdwValueD<=sdwValueA)))
	{
		return ((DWORD)sdwValueE);
	}
	else if (((sdwValueC>=sdwValueA)&&(sdwValueA>=sdwValueD)) ||
		((sdwValueC<=sdwValueA)&&(sdwValueA<=sdwValueD)))
	{
		return ((DWORD)sdwValueB);
	}
	else
	{
		return ((DWORD)((((sdwValueC-sdwValueA)*(sdwValueE-sdwValueB))/(sdwValueD-sdwValueA))+sdwValueB));
	}
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0331_SetFrequence(void)
{
	if ((ptInfo->dwWidth==1920) && (ptInfo->dwHeight==1080))
	{
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0331Hsync = AR0331_HSYNC_1920x1080_60HZ;
			dwAR0331Vsync = AR0331_VSYNC_1920x1080_60HZ;
		}
		else
		{
			dwAR0331Hsync = AR0331_HSYNC_1920x1080_50HZ;
			dwAR0331Vsync = AR0331_VSYNC_1920x1080_50HZ;
		}

	}
	else if ((ptInfo->dwWidth==2048) && (ptInfo->dwHeight==1536))
	{
		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwAR0331Hsync = AR0331_HSYNC_2048x1536_60HZ;
			dwAR0331Vsync = AR0331_VSYNC_2048x1536_60HZ;
		}
		else
		{
			dwAR0331Hsync = AR0331_HSYNC_2048x1536_50HZ;
			dwAR0331Vsync = AR0331_VSYNC_2048x1536_50HZ;
		}
	}

	AR0331_WriteBuf(ptInfo, _300C_LineLengthPck, (dwAR0331Hsync/2), 1, 0, AR0331_Device_AR0331); // LINE_LENGTH_PCK
	AR0331_WriteBuf(ptInfo, _300A_FrameLengthLines, dwAR0331Vsync, 1, 0, AR0331_Device_AR0331); // FRAME_LENGTH_LINES

/* Version 1.0.0.9 modification, 2013.07.01 */
	ptInfo->dwMulHSync = (ptInfo->bEnSerialIF) ? (dwAR0331Hsync*10) : (dwAR0331Hsync*100);
	AR0331_UpdateIntTimeLimit(ptInfo);
/* ======================================== */
	return 0;
}
/* ======================================== */

/* Version 1.0.0.12 modification, 2013.10.09 */
/*-------------------------------------------------------------------------------------*/
static void AR0331_UpdateVtxlo(TAR0331Info *ptInfo)
{
	DWORD dwVtxlo0, dwVtxlo1, dwVtxlo2;

	if (ptInfo->dwPreAGain < 2000)
	{
		dwVtxlo0 = 0x8F2C;
		dwVtxlo1 = 0x2222;
		dwVtxlo2 = 0x8242;
	}
	else if (ptInfo->dwPreAGain < 3500)
	{
		dwVtxlo0 = 0x8FAC;
		dwVtxlo1 = 0xAAAA;
		dwVtxlo2 = 0x8A42;
	}
	else
	{
		dwVtxlo0 = 0x8FCC;
		dwVtxlo1 = 0xCCCC;
		dwVtxlo2 = 0x8C42;
	}

	if (ptInfo->dwVtxlo0 != dwVtxlo0)
	{
		AR0331_WriteBuf(ptInfo, 0x3ED4, dwVtxlo0, 1, 0, AR0331_Device_AR0331); 
		ptInfo->dwVtxlo0 = dwVtxlo0;
	}

	if (ptInfo->dwVtxlo1 != dwVtxlo1)
	{
		AR0331_WriteBuf(ptInfo, 0x3ED6, dwVtxlo1, 1, 0, AR0331_Device_AR0331); 
		ptInfo->dwVtxlo1 = dwVtxlo1;		
	}

	if (ptInfo->dwVtxlo2 != dwVtxlo2)
	{
		AR0331_WriteBuf(ptInfo, 0x3ED8, dwVtxlo2, 1, 0, AR0331_Device_AR0331); 
		ptInfo->dwVtxlo2 = dwVtxlo2;
	}
}
/* ======================================== */

/* Version 1.0.0.9 modification, 2013.07.01 */
/*-------------------------------------------------------------------------------------*/
static void AR0331_DLOEnable(TAR0331Info *ptInfo)
{
	//HDR_MC_CTRL2 0x318C[15] : smoothing_filter On/OFF. 0x318C[14] : motion_correct On/OFF.0x318C[0] : blue_halo On/OFF.
	AR0331_WriteBuf(ptInfo, 0x318C, 0x0000, 1, 0, AR0331_Device_AR0331); 
	//HDR_MC_CTRL4
	AR0331_WriteBuf(ptInfo, 0x3190, 0x0000, 1, 0, AR0331_Device_AR0331);
	//NOISE_PEDESTAL
	AR0331_WriteBuf(ptInfo, 0x30FE, 0x0080, 1, 0, AR0331_Device_AR0331);
	//ADACD_PEDESTAL
	AR0331_WriteBuf(ptInfo, 0x320A, 0x0080, 1, 0, AR0331_Device_AR0331);
	//HDR_MC_CTRL4
	AR0331_WriteBuf(ptInfo, 0x3190, 0xE000, 1, 0, AR0331_Device_AR0331);

	AR0331_WriteBuf(ptInfo, _319E_HdrMcCtrl9, 0x5040, 1, 0, AR0331_Device_AR0331);
	AR0331_WriteBuf(ptInfo, _31A2_HdrMcCtrl11, 0x0BB8, 1, 0, AR0331_Device_AR0331);
}

/* Version 1.0.0.11 modification, 2013.09.04 */
/*-------------------------------------------------------------------------------------*/
void AR0331_AdjustGainStage(TAR0331Info *ptInfo)
{
/* Version 1.0.0.12 modification, 2013.10.09 */
	DWORD dwMaxDGain, dwIndex, dwDGain;

	dwMaxDGain = (sckGainR>=sckGainB) ? sckGainR : sckGainB;
	dwMaxDGain = (15992*1024) / dwMaxDGain;

	for (dwIndex=0; dwIndex<23; dwIndex++)
	{
/* Version 1.0.0.13 modification, 2013.12.24 */
		dwDGain = ((adwAR0331AnalogGainTbl[dwIndex+1][0]*6)>>1);
/* ======================================== */
		if (dwDGain >= dwMaxDGain)
		{
			ptInfo->adwLowGainStage[dwIndex][0] = dwMaxDGain*adwAR0331AnalogGainTbl[dwIndex+1][0]/1000;
			ptInfo->adwLowGainStage[dwIndex][1] = adwAR0331AnalogGainTbl[dwIndex][0];
			ptInfo->adwLowGainStage[dwIndex][2] = adwAR0331AnalogGainTbl[dwIndex][1];
			break;
		}

		ptInfo->adwLowGainStage[dwIndex][0] = dwDGain*adwAR0331AnalogGainTbl[dwIndex+1][0]/1000;
		ptInfo->adwLowGainStage[dwIndex][1] = adwAR0331AnalogGainTbl[dwIndex][0];
		ptInfo->adwLowGainStage[dwIndex][2] = adwAR0331AnalogGainTbl[dwIndex][1];
	}
/* ======================================== */
	ptInfo->dwMaxGain = (dwMaxDGain*8000)/1235;
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static void AR0331_2DMCEnable(TAR0331Info *ptInfo)
{
	//HDR_MC_CTRL4
	AR0331_WriteBuf(ptInfo, 0x3190, 0x0000, 1, 0, AR0331_Device_AR0331);
	//HDR_MC_CTRL2 0x318C[15] : smoothing_filter On/OFF. 0x318C[14] : motion_correct On/OFF.0x318C[0] : blue_halo On/OFF.
	AR0331_WriteBuf(ptInfo, 0x318C, 0xC001, 1, 0, AR0331_Device_AR0331); 
	//HDR_MC_CTRL8
/* Version 1.0.0.13 modification, 2013.12.24 */
	AR0331_WriteBuf(ptInfo, 0x3198, 0x5548, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
	//NOISE_PEDESTAL
	AR0331_WriteBuf(ptInfo, 0x30FE, 0x0000, 1, 0, AR0331_Device_AR0331);
	//ADACD_PEDESTAL
	AR0331_WriteBuf(ptInfo, 0x320A, 0x0000, 1, 0, AR0331_Device_AR0331);
}

/*-------------------------------------------------------------------------------------*/
static void AR0331_SetWDRRatio(TAR0331Info *ptInfo, DWORD dwWDRRatio)
{
	if (dwWDRRatio == 1)
	{
		// Enable Linear Mode
		AR0331_WriteBuf(ptInfo, 0x3082, 0x0001, 1, 0, AR0331_Device_AR0331); 
/* Version 1.0.0.12 modification, 2013.10.09 */
/* ======================================== */
		// [DLO disabled]
		AR0331_WriteBuf(ptInfo, 0x3190, 0x0000, 1, 0, AR0331_Device_AR0331); 
		// [2D motion compensation OFF]
		AR0331_WriteBuf(ptInfo, 0x318C, 0x0000, 1, 0, AR0331_Device_AR0331); 
		AR0331_WriteBuf(ptInfo, 0x3190, 0x0000, 1, 0, AR0331_Device_AR0331); 
		AR0331_WriteBuf(ptInfo, 0x30FE, 0x0080, 1, 0, AR0331_Device_AR0331); 
		AR0331_WriteBuf(ptInfo, 0x320A, 0x0080, 1, 0, AR0331_Device_AR0331); 

		// [ALTM Bypassed]
		AR0331_WriteBuf(ptInfo, 0x2400, 0x0003, 1, 0, AR0331_Device_AR0331); 
		AR0331_WriteBuf(ptInfo, 0x2450, 0x0000, 1, 0, AR0331_Device_AR0331); 
		AR0331_WriteBuf(ptInfo, 0x301E, 0x0028, 1, 0, AR0331_Device_AR0331);  // 0x00A8

		// [ADACD Disabled]
		AR0331_WriteBuf(ptInfo, 0x3200, 0x0000, 1, 0, AR0331_Device_AR0331);

		// data_format_bits  0x31AC[4:0] : data_format_out. 0x31AC[12:8] : data_format_in.
		AR0331_WriteBuf(ptInfo, 0x31AC, 0x0C0C, 1, 0, AR0331_Device_AR0331);

		AR0331_WriteBuf(ptInfo, 0x314C, 0xFFFF, 1, 0, AR0331_Device_AR0331);
	}
	else
	{
		// Enable HDR Mode Mode & set ratio
		switch (dwWDRRatio)
		{
			case 4:
			AR0331_WriteBuf(ptInfo, 0x3082, 0x0000, 1, 0, AR0331_Device_AR0331);
			break;
			case 8:
			AR0331_WriteBuf(ptInfo, 0x3082, 0x0004, 1, 0, AR0331_Device_AR0331);
			break;
			case 16:
			AR0331_WriteBuf(ptInfo, 0x3082, 0x0008, 1, 0, AR0331_Device_AR0331);
			break;
			case 32:
			AR0331_WriteBuf(ptInfo, 0x3082, 0x000C, 1, 0, AR0331_Device_AR0331);
			break;
			default:
			AR0331_WriteBuf(ptInfo, 0x3082, 0x0008, 1, 0, AR0331_Device_AR0331);
			break;
		}
/* Version 1.0.0.12 modification, 2013.10.09 */
/* ======================================== */

		//only one can be enabled
		if (ptInfo->bEn2DMC)
		{
			AR0331_2DMCEnable(ptInfo);
		}
		else
		{
			AR0331_DLOEnable(ptInfo);
		}

		// DATA_Pedestal
		AR0331_WriteBuf(ptInfo, 0x301E, 0x0010, 1, 0, AR0331_Device_AR0331);
		// ALTM_Pedestal
		AR0331_WriteBuf(ptInfo, 0x2450, 0x0000, 1, 0, AR0331_Device_AR0331);

		// [ALTM Enabled]
		AR0331_WriteBuf(ptInfo, 0x2400, 0x0002, 1, 0, AR0331_Device_AR0331);

		// [ADACD Enabled]
		AR0331_WriteBuf(ptInfo, 0x3200, 0x0002, 1, 0, AR0331_Device_AR0331);

		// data_format_bits  0x31AC[4:0] : data_format_out. 0x31AC[12:8] : data_format_in.
		AR0331_WriteBuf(ptInfo, 0x31AC, 0x100C, 1, 0, AR0331_Device_AR0331);

/* Version 1.0.0.11 modification, 2013.09.04 */
		AR0331_WriteBuf(ptInfo, 0x314C, 3800, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
	}

	ptInfo->dwWDRRatio = dwWDRRatio;
	AR0331_UpdateIntTimeLimit(ptInfo);
}
	
/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.11 modification, 2013.09.04 */
static void AR0331_UpdateALTM(TAR0331Info *ptInfo, DWORD dwT1Mean, DWORD dwT1Permill, DWORD dwT2Mean, 
									DWORD dwT2Permill, DWORD dwHistLowPercentValue, DWORD dwHistHighPercentValue)
{
	DWORD dwValue, dwUpperLimit, dwLowerLimit, dwRatio;

	dwRatio = ((dwT1Permill==0)||(dwT2Permill==0)||(dwT2Mean<dwT1Mean))
				? 100
				: (dwT2Mean*100/dwT1Mean);

	dwT1Mean = dwT1Mean>>4;

	if (dwT1Permill <= 50)
	{
		dwValue = AR0331_K01_MAX;
	}
/* Version 1.0.0.13 modification, 2013.12.24 */
	else if ((dwT1Permill>=900) || (ptInfo->adwStatisticY<40))
	{
		dwValue = AR0331_Interpolate(AR0331_K01_BRIGHT_START, 
									AR0331_K01_MIN,
									(SDWORD)dwT1Mean,
									AR0331_K01_BRIGHT_END,
									AR0331_K01_MAX);		
/* ======================================== */
	}
	else 
	{
		dwValue = AR0331_Interpolate(AR0331_K01_RATIO_START, 
										AR0331_K01_MAX,
										(SDWORD)dwRatio,
										AR0331_K01_RATIO_END,
										AR0331_K01_MIN);
	}

	dwUpperLimit = ptInfo->dwPreK01 + AR0331_K01_CONVERGE_STEP;
	dwLowerLimit = (ptInfo->dwPreK01<=AR0331_K01_CONVERGE_STEP) ? 0 : (ptInfo->dwPreK01-AR0331_K01_CONVERGE_STEP);
	dwValue = (dwValue>=dwUpperLimit) ? dwUpperLimit : (dwValue<=dwLowerLimit) ? dwLowerLimit : dwValue;
	dwValue = (dwValue<=AR0331_K01_MIN) ? AR0331_K01_MIN : (dwValue>=AR0331_K01_MAX) ? AR0331_K01_MAX : dwValue;

	if (ptInfo->dwPreK01 != dwValue)
	{
		// ALTM_control_key_k01_hi
		AR0331_WriteBuf(ptInfo, 0x2446, dwValue, 1, 0, AR0331_Device_AR0331);
		ptInfo->dwPreK01 = dwValue;
	}
/* Version 1.0.0.12 modification, 2013.10.09 */
/* ======================================== */
}
/* ======================================== */
/* ======================================== */

/* Version 1.0.0.12 modification, 2013.10.09 */
/*-------------------------------------------------------------------------------------*/
static void AR0331_StreamEnable(TAR0331Info *ptInfo, BOOL bEnable)
{
	if (bEnable)
	{
		if (ptInfo->bEnSerialIF)
		{
			AR0331_WriteBuf(ptInfo, 0x301A, 0x005C, 1, 0, AR0331_Device_AR0331);

			AR0331_WriteBuf(ptInfo, 0x1001, 0x3100, 1, 0, AR0331_Device_FPGA);
			mdelay(30);
			AR0331_WriteBuf(ptInfo, 0x1001, 0x1100, 1, 0, AR0331_Device_FPGA);
			mdelay(2);
/* Version 1.0.0.13 modification, 2013.12.24 */
			if (sckEnDPC)
				AR0331_WriteBuf(ptInfo, 0x1001, 0x4900, 1, 0, AR0331_Device_FPGA);
			else
				AR0331_WriteBuf(ptInfo, 0x1001, 0x0900, 1, 0, AR0331_Device_FPGA);
/* ======================================== */
		}
		else
			AR0331_WriteBuf(ptInfo, 0x301A, 0x10DC, 1, 0, AR0331_Device_AR0331);
	}
	else
	{
		if (ptInfo->bEnSerialIF)
			AR0331_WriteBuf(ptInfo, 0x301A, 0x0058, 1, 0, AR0331_Device_AR0331);
		else
			AR0331_WriteBuf(ptInfo, 0x301A, 0x10D8, 1, 0, AR0331_Device_AR0331);
	}
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static void AR0331_ApplyFixedWB(TAR0331Info *ptInfo, DWORD dwDGain)
{
/* Version 1.0.0.12 modification, 2013.10.09 */
	ptInfo->dwGainR = (dwDGain*sckGainR)>>10;
	ptInfo->dwGainG = dwDGain;
	ptInfo->dwGainB = (dwDGain*sckGainB)>>10;
/* ======================================== */
}
/* ======================================== */

/*-------------------------------------------------------------------------------------*/
static void AR0331_Reset(TAR0331Info *ptInfo)
{
	DWORD dwIndex;

/* Version 1.0.0.9 modification, 2013.07.01 */
/* Version 1.0.0.13 modification, 2013.12.24 */
	AR0331_StreamEnable(ptInfo, FALSE);
/* ======================================== */
	mdelay(30);

	// RESET_REGISTER
	AR0331_WriteBuf(ptInfo, 0x301A, 0x0001, 1, 0, AR0331_Device_AR0331);
	mdelay(200);

/* Version 1.0.0.13 modification, 2013.12.24 */
	AR0331_StreamEnable(ptInfo, FALSE);
/* ======================================== */
	mdelay(30);

/* Version 1.0.0.1 modification, 2012.04.02 */
	if (ptInfo->bHDREnable)
	{
		if (!ptInfo->bHasOTPMCode)
		{
			for (dwIndex=0; dwIndex<AR0331_HDR_SEQUENCE_TBL_SIZE; dwIndex++)
			{
				AR0331_WriteBuf(ptInfo, atAR0331HDRSequenceTbl[dwIndex].eRegAddr, atAR0331HDRSequenceTbl[dwIndex].dwData, 1, 0, AR0331_Device_AR0331);
			}
			mdelay(30);
		}

		if (ptInfo->bEnSerialIF)
		{
			for (dwIndex=0; dwIndex<AR0331_HISPI_HDR_TBL_SIZE; dwIndex++)
			{
				AR0331_WriteBuf(ptInfo, atAR0331HiSpiHDRTbl[dwIndex].eRegAddr, atAR0331HiSpiHDRTbl[dwIndex].dwData, 1, 0, AR0331_Device_AR0331);
				if (atAR0331HiSpiHDRTbl[dwIndex].eRegAddr == 0x305E)
				{
					mdelay(60);		
				}
			}
/* ======================================== */
/* Version 1.0.0.3 modification, 2012.09.26 */
			// datapath_select (0x306E[9] : vcm mode(1.8V))
			AR0331_WriteBuf(ptInfo, 0x306E, 0x9210, 1, 0, AR0331_Device_AR0331);
			// hispi_control_status (0x31C6[1] : output_msb_first)
			AR0331_WriteBuf(ptInfo, 0x31C6, 0x8402, 1, 0, AR0331_Device_AR0331);
		}
		else
		{
			for (dwIndex=0; dwIndex<AR0331_PARALLEL_HDR_TBL_SIZE; dwIndex++)
			{
				AR0331_WriteBuf(ptInfo, atAR0331ParallelHDRTbl[dwIndex].eRegAddr, atAR0331ParallelHDRTbl[dwIndex].dwData, 1, 0, AR0331_Device_AR0331);
				if (atAR0331ParallelHDRTbl[dwIndex].eRegAddr == 0x305E)
				{
					mdelay(60);		
				}
			}
		}

		// digital_ctrl (0x30BA[8] :combi_mode. 0x30BA[5] :dither_enable)
		if (!ptInfo->bHasOTPMCode)
			AR0331_WriteBuf(ptInfo, _30BA_DigitalCtrl, 0x07EC, 1, 0, AR0331_Device_AR0331); // using combi linear mode
		else
			AR0331_WriteBuf(ptInfo, _30BA_DigitalCtrl, 0x06EC, 1, 0, AR0331_Device_AR0331); // using pure linear mode
			
		// SMIA_TEST  (0x3064[8] : embedded_data ON/OFF. 0x3064[7] : embedded_stats ON/OFF)
		AR0331_WriteBuf(ptInfo, 0x3064, 0x1882, 1, 0, AR0331_Device_AR0331);

		// hdr_mc_ctrl  0x318E[15:14] : bypass_pix_comb_cb. 0x318E[13:12] : bypass_pix_comb	
		AR0331_WriteBuf(ptInfo, 0x318E, 0x0000, 1, 0, AR0331_Device_AR0331);
		// delta_dk_control  0x3180[7:4] : delta_dk_rows
		AR0331_WriteBuf(ptInfo, 0x3180, 0x8089, 1, 0, AR0331_Device_AR0331);

		// companding  0x31D0[0] : compand on/off.
		AR0331_WriteBuf(ptInfo, 0x31D0, 0x0000, 1, 0, AR0331_Device_AR0331);
/* Version 1.0.0.5 modification, 2012.12.03 */
/* ======================================== */
/* Version 1.0.0.1 modification, 2012.04.02 */
		// ALTM_CONTROL_MIN_FACTOR Shows more detail in dark image
		AR0331_WriteBuf(ptInfo, 0x2438, 0x000A, 1, 0, AR0331_Device_AR0331); //0x0010 0x0000
/* ======================================== */
		// ALTM_CONTROL_MAX_FACTOR  Shows more detail in bright image
		AR0331_WriteBuf(ptInfo, 0x243A, 0x0020, 1, 0, AR0331_Device_AR0331); // 0x0020 0x00FF
		// ALTM damping factor for Lmin and Lmax value changes	
		AR0331_WriteBuf(ptInfo, 0x2440, 0x0002, 1, 0, AR0331_Device_AR0331);
		// ALTM_control_key_k0
		AR0331_WriteBuf(ptInfo, 0x2442, 0x0080, 1, 0, AR0331_Device_AR0331);
		// ALTM_control_key_k01_lo
		AR0331_WriteBuf(ptInfo, 0x2444, 0x0000, 1, 0, AR0331_Device_AR0331);

		// ALTM_control_key_k01_hi Overall brightness
		AR0331_WriteBuf(ptInfo, 0x2446, 0x0040, 1, 0, AR0331_Device_AR0331);
		ptInfo->dwPreK01 = 0x0040;
		// altm_fsharp_v sharpness strength 
/* Version 1.0.0.12 modification, 2013.10.09 */
		AR0331_WriteBuf(ptInfo, 0x2420, 0x0000, 1, 0, AR0331_Device_AR0331);
		ptInfo->dwPreFSharp = 0x0000;
/* ======================================== */
		// Low Gamma
/* Version 1.0.0.11 modification, 2013.09.04 */
/* Version 1.0.0.13 modification, 2013.12.24 */
		ptInfo->dwPreLGamma = 16;
/* ======================================== */
		AR0331_WriteBuf(ptInfo, 0x2412, ptInfo->dwPreLGamma, 1, 0, AR0331_Device_AR0331);
		// High Gamma
		AR0331_WriteBuf(ptInfo, 0x2410, (32-ptInfo->dwPreLGamma), 1, 0, AR0331_Device_AR0331);
/* ======================================== */

/* Version 1.0.0.5 modification, 2012.12.03 */
/* ======================================== */
		// ADACD NR strength
/* Version 1.0.0.1 modification, 2012.04.02 */
		AR0331_WriteBuf(ptInfo, 0x3202, 0x03FF, 1, 0, AR0331_Device_AR0331); // 0x00CF
/* ======================================== */
		mdelay(60);

/* Version 1.0.0.5 modification, 2012.12.03 */
/* Version 1.0.0.8 modification, 2013.05.24 */
/* ======================================== */
/* ======================================== */
		AR0331_SetWDRRatio(ptInfo, ptInfo->dwUsrWDRRatio);

		// GLOBAL_GAIN
		AR0331_WriteBuf(ptInfo, 0x305E, 0x0080, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
/* Version 1.0.0.1 modification, 2012.04.02 */
	}
	else
	{
		if (!ptInfo->bHasOTPMCode)
		{
			for (dwIndex=0; dwIndex<AR0331_LINEAR_SEQUENCE_TBL_SIZE; dwIndex++)
			{
				AR0331_WriteBuf(ptInfo, atAR0331LinearSequenceTbl[dwIndex].eRegAddr, atAR0331LinearSequenceTbl[dwIndex].dwData, 1, 0, AR0331_Device_AR0331);
			}
			mdelay(30);
		}

		if (ptInfo->bEnSerialIF)
		{
			for (dwIndex=0; dwIndex<AR0331_HISPI_LINEAR_TBL_SIZE; dwIndex++)
			{
				AR0331_WriteBuf(ptInfo, atAR0331HiSpiLinearTbl[dwIndex].eRegAddr, atAR0331HiSpiLinearTbl[dwIndex].dwData, 1, 0, AR0331_Device_AR0331);
				if (atAR0331HiSpiLinearTbl[dwIndex].eRegAddr == 0x305E)
				{
					mdelay(60);		
				}
			}
			// datapath_select (0x306E[9] : vcm mode(1.8V))
			AR0331_WriteBuf(ptInfo, 0x306E, 0x9210, 1, 0, AR0331_Device_AR0331);
			// hispi_control_status (0x31C6[1] : output_msb_first)
			AR0331_WriteBuf(ptInfo, 0x31C6, 0x8402, 1, 0, AR0331_Device_AR0331);
		}
		else
		{
			for (dwIndex=0; dwIndex<AR0331_PARALLEL_LINEAR_TBL_SIZE; dwIndex++)
			{
				AR0331_WriteBuf(ptInfo, atAR0331ParallelLinearTbl[dwIndex].eRegAddr, atAR0331ParallelLinearTbl[dwIndex].dwData, 1, 0, AR0331_Device_AR0331);
				if (atAR0331ParallelLinearTbl[dwIndex].eRegAddr == 0x305E)
				{
					mdelay(60);		
				}
			}
		}
/* Version 1.0.0.2 modification, 2012.08.06 */
		// OPERATION_MODE_CTRL
		AR0331_WriteBuf(ptInfo, 0x3082, 0x0001, 1, 0, AR0331_Device_AR0331);

		// ALTM_CONTROL	0x2400[1] : altm on/off.  0x2400[1] : altm_bypass on/off.
		AR0331_WriteBuf(ptInfo, 0x2400, 0x0001, 1, 0, AR0331_Device_AR0331);

		// SMIA_TEST  (0x3064[8] : embedded_data ON/OFF. 0x3064[7] : embedded_stats ON/OFF)
		AR0331_WriteBuf(ptInfo, 0x3064, 0x1882, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
		// data_format_bits  0x31AC[4:0] : data_format_out. 0x31AC[12:8] : data_format_in.
		AR0331_WriteBuf(ptInfo, 0x31AC, 0x0C0C, 1, 0, AR0331_Device_AR0331);

		// digital_ctrl (0x30BA[8] :combi_mode. 0x30BA[5] :dither_enable)
		AR0331_WriteBuf(ptInfo, _30BA_DigitalCtrl, 0x06EC, 1, 0, AR0331_Device_AR0331);
	}
/* ======================================== */
/* ======================================== */

/* Version 1.0.0.11 modification, 2013.09.04 */
	AR0331_WriteBuf(ptInfo, _3148_AEHistBeginPerc, AR0331_HISTO_BEGION_PERCENT, 1, 0, AR0331_Device_AR0331);
	AR0331_WriteBuf(ptInfo, _314A_AEHistEndPerc, AR0331_HISTO_END_PERCENT, 1, 0, AR0331_Device_AR0331);

	AR0331_AdjustGainStage(ptInfo);
/* ======================================== */

/* Version 1.0.0.12 modification, 2013.10.09 */
	ptInfo->dwVtxlo0 = 0x8F6C;
	ptInfo->dwVtxlo1 = 0x66CC;
	ptInfo->dwVtxlo2 = 0x8C42;

	AR0331_WriteBuf(ptInfo, 0x3ED2, 0x1FC6, 1, 0, AR0331_Device_AR0331);	
	AR0331_WriteBuf(ptInfo, 0x3ED4, ptInfo->dwVtxlo0, 1, 0, AR0331_Device_AR0331);
	AR0331_WriteBuf(ptInfo, 0x3ED6, ptInfo->dwVtxlo1, 1, 0, AR0331_Device_AR0331);
	AR0331_WriteBuf(ptInfo, 0x3ED8, ptInfo->dwVtxlo2, 1, 0, AR0331_Device_AR0331);
/* ======================================== */
	printk("AR0331_Reset Done \n");
}

/*-------------------------------------------------------------------------------------*/
void AR0331_SetShutter(TAR0331Info *ptInfo, DWORD dwShutter)
{
	DWORD dwValue;

/* Version 1.0.0.9 modification, 2013.07.01 */
	dwValue = (dwShutter*ptInfo->dwPixelClock) / ptInfo->dwMulHSync;

	dwValue = (dwValue>ptInfo->dwMaxIntTime)
				? ptInfo->dwMaxIntTime
				: (dwValue<ptInfo->dwMinIntTime)
				? ptInfo->dwMinIntTime
				: dwValue;
/* ======================================== */

	if (ptInfo->dwPreExpLine != dwValue)
	{
		AR0331_WriteBuf(ptInfo, 0x3012, dwValue, 1, 0, AR0331_Device_AR0331);
		ptInfo->dwPreExpLine = dwValue;
	}

	ptInfo->dwCurShutter = dwShutter;
}

/*-------------------------------------------------------------------------------------*/
void AR0331_SetGain(TAR0331Info *ptInfo, DWORD dwGain)
{
	DWORD dwAGainMMRValue, dwAGain, dwDGain;
	DWORD dwIndex, dwTblIdx;

/* Version 1.0.0.2 modification, 2012.08.06 */
	dwGain = (dwGain>ptInfo->dwMaxGain)
				? ptInfo->dwMaxGain :
/* Version 1.0.0.8 modification, 2013.05.24 */
				(dwGain<1000)
				? 1000
/* ======================================== */
				: dwGain;
/* ======================================== */

	dwGain = (dwGain*AR0331_MIN_GAIN_X128)>>7;

/* Version 1.0.0.5 modification, 2012.12.03 */
	if (ptInfo->dwUsrWDRRatio == 1)
	{
/* Version 1.0.0.8 modification, 2013.05.24 */
		if (dwGain < 2000)
			dwTblIdx = 0;
		else if (dwGain < 4000)
			dwTblIdx = 10;
		else
			dwTblIdx = 18;

		for (dwIndex=dwTblIdx; dwIndex<23; dwIndex++)
		{
			if (dwGain <= adwAR0331AnalogGainTbl[dwIndex][0])
				break;
		}

		dwIndex = (dwIndex==0) ? 0 : (dwIndex-1);

/* Version 1.0.0.12 modification, 2013.10.09 */
		dwAGainMMRValue = adwAR0331AnalogGainTbl[dwIndex][1];
		dwAGain = adwAR0331AnalogGainTbl[dwIndex][0];
		dwDGain = (dwGain<<10) / adwAR0331AnalogGainTbl[dwIndex][0];
/* ======================================== */
/* ======================================== */
	}
	else
	{
/* ======================================== */
/* Version 1.0.0.1 modification, 2012.04.02 */
/* ======================================== */
/* Version 1.0.0.12 modification, 2013.10.09 */
		if (dwGain < 10000)
		{
			for (dwIndex=0; dwIndex<23; dwIndex++)
			{
				if (dwGain <= ptInfo->adwLowGainStage[dwIndex][0])
					break;
			}

			dwIndex = (dwIndex>=23) ? 23 : dwIndex;

			dwAGainMMRValue = ptInfo->adwLowGainStage[dwIndex][2];
			dwAGain = ptInfo->adwLowGainStage[dwIndex][1];
			dwDGain = (dwGain<<10) / ptInfo->adwLowGainStage[dwIndex][1];
		}
		else
		{
			dwAGainMMRValue = 0x3030;   // 8X
			dwAGain = 8000;
			dwDGain = (dwGain<<10) / 8000;
		}
/* Version 1.0.0.11 modification, 2013.09.04 */
	}

	if (ptInfo->dwPreDGain != dwDGain)
	{
		AR0331_ApplyFixedWB(ptInfo, dwDGain);
		AR0331_WriteBuf(ptInfo, 0x3056, ((ptInfo->dwGainG>>3)&0x07FF), 1, 0, AR0331_Device_AR0331); //D green1 Gain
		AR0331_WriteBuf(ptInfo, 0x3058, ((ptInfo->dwGainB>>3)&0x07FF), 1, 0, AR0331_Device_AR0331); //D blue Gain
		AR0331_WriteBuf(ptInfo, 0x305A, ((ptInfo->dwGainR>>3)&0x07FF), 1, 0, AR0331_Device_AR0331); //D red Gain
		AR0331_WriteBuf(ptInfo, 0x305C, ((ptInfo->dwGainG>>3)&0x07FF), 1, 0, AR0331_Device_AR0331); //D green2 Gain
/* ======================================== */
		ptInfo->dwPreDGain = dwDGain;
	}
	if (ptInfo->dwPreAGain != dwAGain)
	{
		AR0331_WriteBuf(ptInfo, 0x3060, (dwAGainMMRValue&0x3F3F), 1, 0, AR0331_Device_AR0331); //A Gain
		ptInfo->dwPreAGain = dwAGain;
	}
/* ======================================== */
}

/*==================================================================*/
SOCKET  AR0331_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET SCKRet = 0;
/* Version 1.0.0.2 modification, 2012.08.06 */
	DWORD dwIdx, dwRegNum, adwAEMeasureWinTbl[2];
/* ======================================== */

	if (down_interruptible(&AR0331_mutex))
	{
		return -ERESTARTSYS;
	}

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		AR0331_Reset(ptInfo);
/* Version 1.0.0.1 modification, 2012.04.02 */
		// Reset captured size
		if (AR0331_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
		{
			printk("[AR0331] : Reset width/height error !!\n");			
			SCKRet =  -EPERM;
		}
		else
		{
			AR0331_SetFrequence();
		}

/* Version 1.0.0.13 modification, 2013.12.24 */
		AR0331_StreamEnable(ptInfo, TRUE);
/* ======================================== */
		break;
		case VIDEO_SIGNAL_OPTION_SET_SIZE:
		if (AR0331_SetSize(ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]) < 0)
		{
			printk("[AR0331] : Size is not supported (width=%ld, height=%ld).\n", ptIoctlArg->adwUserData[0], ptIoctlArg->adwUserData[1]);			
			SCKRet = -EPERM;
		}
		else
		{
			AR0331_SetFrequence();
		}
/* Version 1.0.0.13 modification, 2013.12.24 */
		AR0331_StreamEnable(ptInfo, TRUE);
/* ======================================== */
		break;
/* ======================================== */
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		AR0331_SetShutter(ptInfo, ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
		AR0331_SetGain(ptInfo, ptIoctlArg->adwUserData[0]);
/* Version 1.0.0.12 modification, 2013.10.09 */
		AR0331_UpdateVtxlo(ptInfo);
/* ======================================== */
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[AR0331] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			SCKRet = -EPERM;
			break;
		}
/* Version 1.0.0.14 modification, 2014.02.17 */
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];
/* ======================================== */
/* Version 1.0.0.1 modification, 2012.04.02 */
		AR0331_SetFrequence();
/* ======================================== */
/* Version 1.0.0.14 modification, 2014.02.17 */
/* ======================================== */
		break;
		case VIDEO_SIGNAL_OPTION_SET_WDR:
/* Version 1.0.0.9 modification, 2013.07.01 */
		if (ptInfo->bHDREnable)
		{
/* Version 1.0.0.5 modification, 2012.12.03 */
			switch (ptIoctlArg->adwUserData[0])
			{
				case 0:
				ptInfo->dwUsrWDRRatio = 1;
				break;
				case 2:
				ptInfo->dwUsrWDRRatio = 4;
				break;
				case 3:
				ptInfo->dwUsrWDRRatio = 8;
				break;
				case 4:
				ptInfo->dwUsrWDRRatio = 16;
				break;
				case 5:
				ptInfo->dwUsrWDRRatio = 32;
				break;
				default:
				printk("[AR0331] : WDR ratio : %dX not supported.\n", (1<<ptIoctlArg->adwUserData[0]));
				break;
			}
/* Version 1.0.0.8 modification, 2013.05.24 */
/* ======================================== */
/* ======================================== */
/* Version 1.0.0.12 modification, 2013.10.09 */
			AR0331_StreamEnable(ptInfo, FALSE);

			mdelay(30);

			AR0331_SetWDRRatio(ptInfo, ptInfo->dwUsrWDRRatio);

			AR0331_StreamEnable(ptInfo, TRUE);
/* ======================================== */
		}
/* ======================================== */
		break;
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS:
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MODE:
/* Version 1.0.0.2 modification, 2012.08.06 */
		if (sensor_dev_ops.get_exp_statistic)
		{
/* ======================================== */
			memcpy(adwAEMeasureWinTbl, (DWORD *)ptIoctlArg->adwUserData[0], sizeof(DWORD)*2);
			AR0331_WriteBuf(ptInfo, _3140_AERoiXStartOffset, (adwAEMeasureWinTbl[0]&0x1FFF), 1, 0, AR0331_Device_AR0331);	
			AR0331_WriteBuf(ptInfo, _3142_AERoiYStartOffset, ((adwAEMeasureWinTbl[0]>>16)&0x1FFF), 1, 0, AR0331_Device_AR0331);	
			AR0331_WriteBuf(ptInfo, _3144_AERoiXSize, (adwAEMeasureWinTbl[1]&0x1FFF), 1, 0, AR0331_Device_AR0331);	
			AR0331_WriteBuf(ptInfo, _3146_AERoiYSize, ((adwAEMeasureWinTbl[1]>>16)&0x1FFF), 1, 0, AR0331_Device_AR0331);	
/* Version 1.0.0.2 modification, 2012.08.06 */
		}
/* Version 1.0.0.12 modification, 2013.10.09 */
		SCKRet = -1;
/* ======================================== */
/* ======================================== */
		break;
/* Version 1.0.0.13 modification, 2013.12.24 */
		case VIDEO_SIGNAL_OPTION_SENSOR_DIRECT_ACCESS:
		dwRegNum = (ptIoctlArg->adwUserData[0]>20) ? 20 : ptIoctlArg->adwUserData[0];
		copy_from_user(&(ptInfo->adwRegAddr[0]), (DWORD *)ptIoctlArg->adwUserData[1], dwRegNum*sizeof(DWORD));
		copy_from_user(&(ptInfo->adwRegData[0]), (DWORD *)ptIoctlArg->adwUserData[2], dwRegNum*sizeof(DWORD));
		for (dwIdx=0; dwIdx<dwRegNum; dwIdx++)
			AR0331_WriteBuf(ptInfo, (ptInfo->adwRegAddr[dwIdx]&0xFFFF), (ptInfo->adwRegData[dwIdx]&0xFFFF), 1, 0, AR0331_Device_AR0331);			
		break;
/* ======================================== */
		default:
		SCKRet = -EPERM;
	}
	up(&AR0331_mutex);

	return SCKRet;
}

/*-------------------------------------------------------------------------------------*/
void AR0331_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0331_Open(TVideoSensorInitialParam* ptAR0331InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptAR0331InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(AR0331_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptAR0331InitialParam->dwVideoSensorVersion&0x0000FF00)>(AR0331_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid AR0331 device driver version %d.%d.%d.%d !!\n", 
				(int)(AR0331_INTERFACE_VERSION&0xFF), 
				(int)((AR0331_INTERFACE_VERSION>>8)&0xFF), 
				(int)((AR0331_INTERFACE_VERSION>>16)&0xFF), 
				(int)(AR0331_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TAR0331Info *)kmalloc(sizeof(TAR0331Info), GFP_KERNEL)))
	{
		printk("[AR0331] : Allocate %d bytes memory fail\n", sizeof(TAR0331Info));
		return -ENOMEM;
	}

/* Version 1.0.0.12 modification, 2013.10.09 */
	memset(ptInfo, 0, sizeof(TAR0331Info));
/* ======================================== */

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 30;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[AR0331] : Initial IICCtrl object fail !!\n");
		AR0331_Release(dwDevNum);			
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = AR0331_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = (DWORD)ptAR0331InitialParam->dwVideoSensorFrequency;
	ptInfo->dwCurShutter = 33333;
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;
/* Version 1.0.0.1 modification, 2012.04.02 */
	ptInfo->bHDREnable = sckEnHDR;
	ptInfo->dwUsrWDRRatio = 1;
/* ======================================== */
/* Version 1.0.0.8 modification, 2013.05.24 */
	ptInfo->bEn2DMC = sckEn2DMC;
/* ======================================== */

/* Version 1.0.0.9 modification, 2013.07.01 */
	ptInfo->bEnSerialIF = sckEnSerialIF;
	ptInfo->dwMulHSync = (ptInfo->bEnSerialIF) ? (dwAR0331Hsync*10) : (dwAR0331Hsync*100);

	if (ptInfo->bEnSerialIF)
	{
		
		ptInfo->dwPixelClock = AR0331_PIXEL_CLOCK_60FPS_M_X10;
		// Detect FPGA device
/* Version 1.0.0.2 modification, 2012.08.06 */
#if 0	
		if (!AR0331_CompBuf(ptInfo, 0x1000, 0xFFFF, FPGA_CHIP_VERSION, AR0331_Device_FPGA))
		{
			printk("\n[AR0331] : FPGA device doesn't exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
			AR0331_Release(dwDevNum);				
			return -ENODEV;
		}
		else
		{
			printk("\n[AR0331] : FPGA device exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
		}
#endif
/* ======================================== */

		// release sensor reset, RSTN
		AR0331_WriteBuf(ptInfo, 0x1001, 0x3100, 1, 0, AR0331_Device_FPGA);
		mdelay(30);
	}
	else
	{
		ptInfo->dwPixelClock = AR0331_PIXEL_CLOCK_30FPS_M_X100;
	}

	// Detect sensor device
/* Version 1.0.0.10 modification, 2013.07.02 */
/* ======================================== */
	// Check has OTPM code 
	if ((AR0331_CompBuf(ptInfo, 0x309E, 0xFFFF, 0x01D0, AR0331_Device_AR0331)) && sckHasOTPMCode)
		ptInfo->bHasOTPMCode = TRUE;
	else
		ptInfo->bHasOTPMCode = FALSE;
/* ======================================== */

	// Reset device
	AR0331_Reset(ptInfo);

	// init captured size
/* Version 1.0.0.1 modification, 2012.04.02 */
	if (AR0331_SetSize(ptInfo->dwWidth, ptInfo->dwHeight) < 0)
	{
		printk("[AR0331] : initial width/height error !!\n");			
		AR0331_Release(dwDevNum);				
		return -EPERM;
	}
	AR0331_SetFrequence();

/* Version 1.0.0.13 modification, 2013.12.24 */
	AR0331_StreamEnable(ptInfo, TRUE);
/* ======================================== */

	return 0;
}

/*-------------------------------------------------------------------------------------*/
SOCKET AR0331_GetExpStatistic(TVideoSensorStatistic* ptAR0331Statistic, DWORD dwDevNum)
{
/* Version 1.0.0.11 modification, 2013.09.04 */
	DWORD dwReadData[9]={0};
	DWORD dwT1Permill, dwT2Permill, dwT1Mean, dwT2Mean, dwGlobalMean;
	DWORD dwHistLowPercentValue, dwHistHighPercentValue;

	if (down_interruptible(&AR0331_mutex))
	{
		return -ERESTARTSYS;
	}	

/* Version 1.0.0.1 modification, 2012.04.02 */
/* Version 1.0.0.12 modification, 2013.10.09 */
	AR0331_ReadBuf(ptInfo, _3150_AEMeanH, &dwReadData[0], 6, AR0331_Device_AR0331);
	dwGlobalMean = ((dwReadData[0]&0x000F)<<16)|(dwReadData[1]&0xFFFF);
	dwGlobalMean = (dwGlobalMean*ptInfo->dwWDRRatio)>>4;

	dwHistLowPercentValue = ((dwReadData[2]&0x000F)<<16)|(dwReadData[3]&0xFFFF);
	dwHistHighPercentValue = ((dwReadData[4]&0x000F)<<16)|(dwReadData[5]&0xFFFF);

	AR0331_ReadBuf(ptInfo, _315C_AEHistEndMeanH, &dwReadData[0], 2, AR0331_Device_AR0331);
	dwT1Mean = ((dwReadData[0]&0x000F)<<16)|(dwReadData[1]&0xFFFF);
	dwT1Mean = (dwT1Mean*ptInfo->dwWDRRatio)>>4;

	AR0331_ReadBuf(ptInfo, _3160_AEPercLowEnd, &dwReadData[0], 1, AR0331_Device_AR0331);
	dwT1Permill = ((dwReadData[0]+1)*1000)>>16;
	dwT2Permill = 1000 - dwT1Permill;

	dwT2Mean = (dwT2Permill<=1)
				? 0
				: ((dwGlobalMean*1000-(dwT1Mean*dwT1Permill))/dwT2Permill);

	ptInfo->adwStatisticY = (ptInfo->dwWDRRatio==1)
							? (dwGlobalMean>>4)
							: ((((dwT1Mean>>4)*dwT1Permill)+((dwT2Mean>>7)*dwT2Permill))/1000); // (dwT2Mean>>6)*dwT2Permill
/* ======================================== */
/* ======================================== */

	if (ptInfo->dwUsrWDRRatio > 1)
	{
		AR0331_UpdateALTM(ptInfo, dwT1Mean, dwT1Permill, dwT2Mean, dwT2Permill, dwHistLowPercentValue, dwHistHighPercentValue);
	}
	up(&AR0331_mutex);

/* ======================================== */

	ptAR0331Statistic->dwLumaHorSubWindowNum = 1;
	ptAR0331Statistic->dwLumaVerSubWindowNum = 1;
/* ======================================== */

	ptAR0331Statistic->pdwStatisticY = &ptInfo->adwStatisticY;

	ptAR0331Statistic->dwHistogramBinNum = 0;
	ptAR0331Statistic->pdwHistogram = NULL;

	ptAR0331Statistic->dwWBHorSubWindowNum = 0;
	ptAR0331Statistic->dwWBVerSubWindowNum = 0;
	ptAR0331Statistic->pdwStatisticR = NULL;
	ptAR0331Statistic->pdwStatisticG = NULL;
	ptAR0331Statistic->pdwStatisticB = NULL;

	return 0;
}

/* Version 1.0.0.2 modification, 2012.08.06 */
/*-------------------------------------------------------------------------------------*/
DWORD AR0331_GetMaxGain(void)
{
	return (ptInfo->dwMaxGain);
}
/* ======================================== */
/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	AR0331_Open,
	.release = 	AR0331_Release,
	.ioctl = 	AR0331_Ioctl,
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,	
	.get_exp_statistic = AR0331_GetExpStatistic,
	.get_wb_statistic = NULL,
/* Version 1.0.0.2 modification, 2012.08.06 */
	.get_max_gain = AR0331_GetMaxGain,
/* ======================================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct AR0331_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t AR0331_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct AR0331_attribute *AR0331_attr = container_of(attr, struct AR0331_attribute, attr);
	int result = 0;
	
	if (AR0331_attr->show)
		result = AR0331_attr->show(kobj, buf);	
	return result;
}

ssize_t AR0331_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "AR0331");
	return retval;
}

ssize_t AR0331_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t AR0331_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "2048");
	return retval;
}

ssize_t AR0331_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1536");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject AR0331_kobj;

struct AR0331_attribute AR0331_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0331_SensorName_attr_show,
};

struct AR0331_attribute AR0331_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0331_dwBufNum_attr_show,
};

struct AR0331_attribute AR0331_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0331_MaxFrameWidth_attr_show,
};

struct AR0331_attribute AR0331_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = AR0331_MaxFrameHeight_attr_show,
};

struct sysfs_ops AR0331_sysfs_ops = {
	.show	= AR0331_default_attr_show,
};

struct kobj_type AR0331_ktype = {
	.sysfs_ops	= &AR0331_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET AR0331_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[AR0331] : kobj set name fail!\n");
		return -1;
	}
	
	printk(KERN_INFO "AR0331() sensor_width=%d sensor_height=%d\n", video_sensor_width, video_sensor_height);
	AR0331_kobj.ktype = &AR0331_ktype;	
	sckResult = kobject_init_and_add(&AR0331_kobj, &AR0331_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[AR0331] : Cannot register kobject [AR0331_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0331_kobj, &AR0331_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0331] : Cannot create AR0331_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&AR0331_kobj, &AR0331_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0331] : Cannot create AR0331_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0331_kobj, &AR0331_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0331] : Cannot create AR0331_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&AR0331_kobj, &AR0331_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[AR0331] : Cannot create AR0331_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&AR0331_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void AR0331_Exit(void)
{
	sysfs_remove_file(&AR0331_kobj, &(AR0331_SensorName_attr.attr));
	sysfs_remove_file(&AR0331_kobj, &(AR0331_dwBufNum_attr.attr));	
	sysfs_remove_file(&AR0331_kobj, &(AR0331_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&AR0331_kobj, &(AR0331_MaxFrameHeight_attr.attr));	
	kobject_put(&AR0331_kobj);	
	printk(KERN_DEBUG "[AR0331] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(AR0331_Init);
module_exit(AR0331_Exit);
