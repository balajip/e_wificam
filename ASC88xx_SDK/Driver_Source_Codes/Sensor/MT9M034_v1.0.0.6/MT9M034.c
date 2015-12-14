/*
 * MT9M034
 * Driver for MT9M034 sensor.
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

#include "MT9M034.h"
#include "MT9M034_locals.h"
#include "IICCtrl.h"
#include <linux/semaphore.h> // sem_init(), up(), down_interruptibal()

const CHAR MT9M034_ID[] = "$Version: "MT9M034_ID_VERSION"  (MT9M034 DRIVER) $";

static TMT9M034Info *ptInfo;
static TVideoSensorDevice sensor_dev_ops;
static struct semaphore MT9M034_mutex;

static DWORD dwMT9M034Hsync = MT9M034_HSYNC_1280x720_60FPS;
static DWORD dwMT9M034Vsync = MT9M034_VSYNC_1280x720_60FPS;

static SOCKET video_sensor_width = 1280;
static SOCKET video_sensor_height = 720;

static SOCKET sckEnAE = 0;
static SOCKET sckEnHDR = 0;

module_param(video_sensor_width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_width, "MT9M034 capture width");
module_param(video_sensor_height, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (video_sensor_height, "MT9M034 capture height");

module_param(sckEnAE, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckEnAE, "Enable MT9M034 sensor AE");
module_param(sckEnHDR, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (sckEnHDR, "Enable MT9M034 HDR");

/*==================================================================*/
MODULE_AUTHOR("VN Inc.");
MODULE_DESCRIPTION("MT9M034 driver");
MODULE_LICENSE("GPL");

/*==================================================================*/
const TMT9M034RegAddrData atMT9M034HDRSequenceTbl[MT9M034_HDR_SEQUENCE_TBL_SIZE] = 
{
	// [A-1000 Hidy and linear sequencer load August 2 2011]
	{0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{0x3086, 0x0025}, 	// SEQ_DATA_PORT
	{0x3086, 0x5050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x0828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0926}, 	// SEQ_DATA_PORT
	{0x3086, 0x0028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0xA728}, 	// SEQ_DATA_PORT
	{0x3086, 0x0725}, 	// SEQ_DATA_PORT
	{0x3086, 0x8080}, 	// SEQ_DATA_PORT
	{0x3086, 0x2925}, 	// SEQ_DATA_PORT
	{0x3086, 0x0040}, 	// SEQ_DATA_PORT
	{0x3086, 0x2702}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F17}, 	// SEQ_DATA_PORT
	{0x3086, 0x3626}, 	// SEQ_DATA_PORT
	{0x3086, 0xA617}, 	// SEQ_DATA_PORT
	{0x3086, 0x0326}, 	// SEQ_DATA_PORT
	{0x3086, 0xA417}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F28}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0x2028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0425}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x171D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x2017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1219}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1728}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x171A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2660}, 	// SEQ_DATA_PORT
	{0x3086, 0x175A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2317}, 	// SEQ_DATA_PORT
	{0x3086, 0x1122}, 	// SEQ_DATA_PORT
	{0x3086, 0x1741}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x9027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0026}, 	// SEQ_DATA_PORT
	{0x3086, 0x1828}, 	// SEQ_DATA_PORT
	{0x3086, 0x002E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2A28}, 	// SEQ_DATA_PORT
	{0x3086, 0x081C}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7003}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7005}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7009}, 	// SEQ_DATA_PORT
	{0x3086, 0x170C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x0414}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0414}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x0514}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2405}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5001}, 	// SEQ_DATA_PORT
	{0x3086, 0x2550}, 	// SEQ_DATA_PORT
	{0x3086, 0x502D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2608}, 	// SEQ_DATA_PORT
	{0x3086, 0x280D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1709}, 	// SEQ_DATA_PORT
	{0x3086, 0x2600}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A7}, 	// SEQ_DATA_PORT
	{0x3086, 0x2807}, 	// SEQ_DATA_PORT
	{0x3086, 0x2580}, 	// SEQ_DATA_PORT
	{0x3086, 0x8029}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0216}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0x0620}, 	// SEQ_DATA_PORT
	{0x3086, 0x1736}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A6}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A4}, 	// SEQ_DATA_PORT
	{0x3086, 0x171F}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x2620}, 	// SEQ_DATA_PORT
	{0x3086, 0x2804}, 	// SEQ_DATA_PORT
	{0x3086, 0x2520}, 	// SEQ_DATA_PORT
	{0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1D25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1712}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0327}, 	// SEQ_DATA_PORT
	{0x3086, 0x0617}, 	// SEQ_DATA_PORT
	{0x3086, 0x2828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0517}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0xAE25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0090}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x2618}, 	// SEQ_DATA_PORT
	{0x3086, 0x2800}, 	// SEQ_DATA_PORT
	{0x3086, 0x2E2A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2808}, 	// SEQ_DATA_PORT
	{0x3086, 0x1D05}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7009}, 	// SEQ_DATA_PORT
	{0x3086, 0x1720}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2024}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5002}, 	// SEQ_DATA_PORT
	{0x3086, 0x2550}, 	// SEQ_DATA_PORT
	{0x3086, 0x502D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2608}, 	// SEQ_DATA_PORT
	{0x3086, 0x280D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1709}, 	// SEQ_DATA_PORT
	{0x3086, 0x2600}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A7}, 	// SEQ_DATA_PORT
	{0x3086, 0x2807}, 	// SEQ_DATA_PORT
	{0x3086, 0x2580}, 	// SEQ_DATA_PORT
	{0x3086, 0x8029}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0216}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0x0617}, 	// SEQ_DATA_PORT
	{0x3086, 0x3626}, 	// SEQ_DATA_PORT
	{0x3086, 0xA617}, 	// SEQ_DATA_PORT
	{0x3086, 0x0326}, 	// SEQ_DATA_PORT
	{0x3086, 0xA417}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F28}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0x2028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0425}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x171D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x2021}, 	// SEQ_DATA_PORT
	{0x3086, 0x1712}, 	// SEQ_DATA_PORT
	{0x3086, 0x1B17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0327}, 	// SEQ_DATA_PORT
	{0x3086, 0x0617}, 	// SEQ_DATA_PORT
	{0x3086, 0x2828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0517}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0xAE25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0090}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x2618}, 	// SEQ_DATA_PORT
	{0x3086, 0x2800}, 	// SEQ_DATA_PORT
	{0x3086, 0x2E2A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2808}, 	// SEQ_DATA_PORT
	{0x3086, 0x1E17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0A05}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7009}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1614}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2414}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2B2B}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C00}, 	// SEQ_DATA_PORT
	{0x3086, 0x0225}, 	// SEQ_DATA_PORT
	{0x3086, 0x5050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x0828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0926}, 	// SEQ_DATA_PORT
	{0x3086, 0x0028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0xA728}, 	// SEQ_DATA_PORT
	{0x3086, 0x0725}, 	// SEQ_DATA_PORT
	{0x3086, 0x8080}, 	// SEQ_DATA_PORT
	{0x3086, 0x2917}, 	// SEQ_DATA_PORT
	{0x3086, 0x0525}, 	// SEQ_DATA_PORT
	{0x3086, 0x0040}, 	// SEQ_DATA_PORT
	{0x3086, 0x2702}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1736}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A6}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A4}, 	// SEQ_DATA_PORT
	{0x3086, 0x171F}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x2620}, 	// SEQ_DATA_PORT
	{0x3086, 0x2804}, 	// SEQ_DATA_PORT
	{0x3086, 0x2520}, 	// SEQ_DATA_PORT
	{0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1E25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2117}, 	// SEQ_DATA_PORT
	{0x3086, 0x1028}, 	// SEQ_DATA_PORT
	{0x3086, 0x051B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x1747}, 	// SEQ_DATA_PORT
	{0x3086, 0x2660}, 	// SEQ_DATA_PORT
	{0x3086, 0x17AE}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x9027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0026}, 	// SEQ_DATA_PORT
	{0x3086, 0x1828}, 	// SEQ_DATA_PORT
	{0x3086, 0x002E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2A28}, 	// SEQ_DATA_PORT
	{0x3086, 0x081E}, 	// SEQ_DATA_PORT
	{0x3086, 0x0831}, 	// SEQ_DATA_PORT
	{0x3086, 0x1440}, 	// SEQ_DATA_PORT
	{0x3086, 0x4014}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1410}, 	// SEQ_DATA_PORT
	{0x3086, 0x1034}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x1014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x4013}, 	// SEQ_DATA_PORT
	{0x3086, 0x1802}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7003}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7017}, 	// SEQ_DATA_PORT
	{0x3086, 0x2002}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2002}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5022}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT	 
};

const TMT9M034RegAddrData atMT9M034LinearSequenceTbl[MT9M034_LINEAR_SEQUENCE_TBL_SIZE] = 
{
	{0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{0x3086, 0x0025}, 	// SEQ_DATA_PORT
	{0x3086, 0x5050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x0828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0926}, 	// SEQ_DATA_PORT
	{0x3086, 0x0028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0xA728}, 	// SEQ_DATA_PORT
	{0x3086, 0x0725}, 	// SEQ_DATA_PORT
	{0x3086, 0x8080}, 	// SEQ_DATA_PORT
	{0x3086, 0x2925}, 	// SEQ_DATA_PORT
	{0x3086, 0x0040}, 	// SEQ_DATA_PORT
	{0x3086, 0x2702}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F17}, 	// SEQ_DATA_PORT
	{0x3086, 0x3626}, 	// SEQ_DATA_PORT
	{0x3086, 0xA617}, 	// SEQ_DATA_PORT
	{0x3086, 0x0326}, 	// SEQ_DATA_PORT
	{0x3086, 0xA417}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F28}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0x2028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0425}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x171D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x2017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1219}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1728}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x171A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2660}, 	// SEQ_DATA_PORT
	{0x3086, 0x175A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2317}, 	// SEQ_DATA_PORT
	{0x3086, 0x1122}, 	// SEQ_DATA_PORT
	{0x3086, 0x1741}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x9027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0026}, 	// SEQ_DATA_PORT
	{0x3086, 0x1828}, 	// SEQ_DATA_PORT
	{0x3086, 0x002E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2A28}, 	// SEQ_DATA_PORT
	{0x3086, 0x081C}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7003}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7005}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7009}, 	// SEQ_DATA_PORT
	{0x3086, 0x170C}, 	// SEQ_DATA_PORT
	{0x3086, 0x0014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x0414}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0414}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x0514}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2405}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5001}, 	// SEQ_DATA_PORT
	{0x3086, 0x2550}, 	// SEQ_DATA_PORT
	{0x3086, 0x502D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2608}, 	// SEQ_DATA_PORT
	{0x3086, 0x280D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1709}, 	// SEQ_DATA_PORT
	{0x3086, 0x2600}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A7}, 	// SEQ_DATA_PORT
	{0x3086, 0x2807}, 	// SEQ_DATA_PORT
	{0x3086, 0x2580}, 	// SEQ_DATA_PORT
	{0x3086, 0x8029}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0216}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0x0620}, 	// SEQ_DATA_PORT
	{0x3086, 0x1736}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A6}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A4}, 	// SEQ_DATA_PORT
	{0x3086, 0x171F}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x2620}, 	// SEQ_DATA_PORT
	{0x3086, 0x2804}, 	// SEQ_DATA_PORT
	{0x3086, 0x2520}, 	// SEQ_DATA_PORT
	{0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1D25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1712}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0327}, 	// SEQ_DATA_PORT
	{0x3086, 0x0617}, 	// SEQ_DATA_PORT
	{0x3086, 0x2828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0517}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0xAE25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0090}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x2618}, 	// SEQ_DATA_PORT
	{0x3086, 0x2800}, 	// SEQ_DATA_PORT
	{0x3086, 0x2E2A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2808}, 	// SEQ_DATA_PORT
	{0x3086, 0x1D05}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7009}, 	// SEQ_DATA_PORT
	{0x3086, 0x1720}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2024}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5002}, 	// SEQ_DATA_PORT
	{0x3086, 0x2550}, 	// SEQ_DATA_PORT
	{0x3086, 0x502D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2608}, 	// SEQ_DATA_PORT
	{0x3086, 0x280D}, 	// SEQ_DATA_PORT
	{0x3086, 0x1709}, 	// SEQ_DATA_PORT
	{0x3086, 0x2600}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A7}, 	// SEQ_DATA_PORT
	{0x3086, 0x2807}, 	// SEQ_DATA_PORT
	{0x3086, 0x2580}, 	// SEQ_DATA_PORT
	{0x3086, 0x8029}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0216}, 	// SEQ_DATA_PORT
	{0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{0x3086, 0x0617}, 	// SEQ_DATA_PORT
	{0x3086, 0x3626}, 	// SEQ_DATA_PORT
	{0x3086, 0xA617}, 	// SEQ_DATA_PORT
	{0x3086, 0x0326}, 	// SEQ_DATA_PORT
	{0x3086, 0xA417}, 	// SEQ_DATA_PORT
	{0x3086, 0x1F28}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0x2028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0425}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x171D}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x2021}, 	// SEQ_DATA_PORT
	{0x3086, 0x1712}, 	// SEQ_DATA_PORT
	{0x3086, 0x1B17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0327}, 	// SEQ_DATA_PORT
	{0x3086, 0x0617}, 	// SEQ_DATA_PORT
	{0x3086, 0x2828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0517}, 	// SEQ_DATA_PORT
	{0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{0x3086, 0xAE25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0090}, 	// SEQ_DATA_PORT
	{0x3086, 0x2700}, 	// SEQ_DATA_PORT
	{0x3086, 0x2618}, 	// SEQ_DATA_PORT
	{0x3086, 0x2800}, 	// SEQ_DATA_PORT
	{0x3086, 0x2E2A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2808}, 	// SEQ_DATA_PORT
	{0x3086, 0x1E17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0A05}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7009}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2024}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x502B}, 	// SEQ_DATA_PORT
	{0x3086, 0x302C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C00}, 	// SEQ_DATA_PORT
	{0x3086, 0x0225}, 	// SEQ_DATA_PORT
	{0x3086, 0x5050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2D26}, 	// SEQ_DATA_PORT
	{0x3086, 0x0828}, 	// SEQ_DATA_PORT
	{0x3086, 0x0D17}, 	// SEQ_DATA_PORT
	{0x3086, 0x0926}, 	// SEQ_DATA_PORT
	{0x3086, 0x0028}, 	// SEQ_DATA_PORT
	{0x3086, 0x0526}, 	// SEQ_DATA_PORT
	{0x3086, 0xA728}, 	// SEQ_DATA_PORT
	{0x3086, 0x0725}, 	// SEQ_DATA_PORT
	{0x3086, 0x8080}, 	// SEQ_DATA_PORT
	{0x3086, 0x2917}, 	// SEQ_DATA_PORT
	{0x3086, 0x0525}, 	// SEQ_DATA_PORT
	{0x3086, 0x0040}, 	// SEQ_DATA_PORT
	{0x3086, 0x2702}, 	// SEQ_DATA_PORT
	{0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1736}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A6}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x26A4}, 	// SEQ_DATA_PORT
	{0x3086, 0x171F}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x2620}, 	// SEQ_DATA_PORT
	{0x3086, 0x2804}, 	// SEQ_DATA_PORT
	{0x3086, 0x2520}, 	// SEQ_DATA_PORT
	{0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0017}, 	// SEQ_DATA_PORT
	{0x3086, 0x1E25}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x2117}, 	// SEQ_DATA_PORT
	{0x3086, 0x121B}, 	// SEQ_DATA_PORT
	{0x3086, 0x1703}, 	// SEQ_DATA_PORT
	{0x3086, 0x2706}, 	// SEQ_DATA_PORT
	{0x3086, 0x1728}, 	// SEQ_DATA_PORT
	{0x3086, 0x2805}, 	// SEQ_DATA_PORT
	{0x3086, 0x171A}, 	// SEQ_DATA_PORT
	{0x3086, 0x2660}, 	// SEQ_DATA_PORT
	{0x3086, 0x17AE}, 	// SEQ_DATA_PORT
	{0x3086, 0x2500}, 	// SEQ_DATA_PORT
	{0x3086, 0x9027}, 	// SEQ_DATA_PORT
	{0x3086, 0x0026}, 	// SEQ_DATA_PORT
	{0x3086, 0x1828}, 	// SEQ_DATA_PORT
	{0x3086, 0x002E}, 	// SEQ_DATA_PORT
	{0x3086, 0x2A28}, 	// SEQ_DATA_PORT
	{0x3086, 0x081E}, 	// SEQ_DATA_PORT
	{0x3086, 0x0831}, 	// SEQ_DATA_PORT
	{0x3086, 0x1440}, 	// SEQ_DATA_PORT
	{0x3086, 0x4014}, 	// SEQ_DATA_PORT
	{0x3086, 0x2020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1410}, 	// SEQ_DATA_PORT
	{0x3086, 0x1034}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x1014}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x4013}, 	// SEQ_DATA_PORT
	{0x3086, 0x1802}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7003}, 	// SEQ_DATA_PORT
	{0x3086, 0x1470}, 	// SEQ_DATA_PORT
	{0x3086, 0x7017}, 	// SEQ_DATA_PORT
	{0x3086, 0x2002}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2002}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x2004}, 	// SEQ_DATA_PORT
	{0x3086, 0x1400}, 	// SEQ_DATA_PORT
	{0x3086, 0x5022}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0020}, 	// SEQ_DATA_PORT
	{0x3086, 0x0314}, 	// SEQ_DATA_PORT
	{0x3086, 0x0050}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
};

const TMT9M034RegAddrData atMT9M034HDRTbl[MT9M034_HDR_TBL_SIZE] = 
{
	{0x3082, 0x0028}, 	// OPERATION_MODE_CTRL

	{0x301E, 0x00C8}, 	// DATA_PEDESTAL 
	{0x3EDA, 0x0F03}, 	// RESERVED_MFR_3EDA
	{0x3EDE, 0xC005}, 	// RESERVED_MFR_3EDE 
	{0x3ED8, 0x09EF}, 	// RESERVED_MFR_3ED8
	{0x3EE2, 0xA46B}, 	// RESERVED_MFR_3EE2 
	{0x3EE0, 0x047D}, 	// RESERVED_MFR_3EE0  ==> Modify
	{0x3EDC, 0x0070}, 	// RESERVED_MFR_3EDC 
	{0x3044, 0x0404}, 	// DARK_CONTROL 
	{0x3EE6, 0x8303}, 	// RESERVED_MFR_3EE6 
	{0x3EE4, 0xD208}, 	// DAC_LD_24_25
	{0x3ED6, 0x00BD}, 	// RESERVED_MFR_3ED6
	{0x30E4, 0x6372}, 	// RESERVED_MFR_30E4
	{0x30E2, 0x7253}, 	// RESERVED_MFR_30E2
	{0x30E0, 0x5470}, 	// RESERVED_MFR_30E0
	{0x30E6, 0xC4CC}, 	// RESERVED_MFR_30E6
	{0x30E8, 0x8050}, 	// RESERVED_MFR_30E8

	{0x3012, 0x02A0}, 	// COARSE_INTEGRATION_TIME
	 
	{0x31D0, 0x0001}, 	// HDR_COMP

	{0x302C, 0x0004}, 	// VT_SYS_CLK_DIV
	{0x302A, 0x0004}, 	// VT_PIX_CLK_DIV
	{0x302E, 0x0002}, 	// PRE_PLL_CLK_DIV
	{0x3030, 0x002C}, 	// PLL_MULTIPLIER
	{0x30B0, 0x1300}, 	// DIGITAL_TEST

	{0x3064, 0x1982}, 	// EMBEDDED_DATA_CTRL
	{0x3064, 0x1982}, 	// EMBEDDED_DATA_CTRL

	{0x3100, 0x001B}, 	// AE_CTRL_REG
	{0x3112, 0x029F}, 	// AE_DCG_EXPOSURE_HIGH_REG
	{0x3114, 0x008C}, 	// AE_DCG_EXPOSURE_LOW_REG
	{0x3116, 0x02C0}, 	// AE_DCG_GAIN_FACTOR_REG
	{0x3118, 0x005B}, 	// AE_DCG_GAIN_FACTOR_INV_REG
	{0x3102, 0x0708}, 	// AE_LUMA_TARGET_REG
	{0x3104, 0x1000}, 	// AE_HIST_TARGET_REG
	{0x3126, 0x0064}, 	// AE_ALPHA_V1_REG
	{0x311C, 0x02A0}, 	// AE_MAX_EXPOSURE_REG
	{0x311E, 0x0080}, 	// AE_MIN_EXPOSURE_REG

	{0x301A, 0x10DC}, 	// RESET_REGISTER
};

const TMT9M034RegAddrData atMT9M034LinearTbl[MT9M034_LINEAR_TBL_SIZE] = 
{ 
	{0x309E, 0x018A}, 	// RESERVED_MFR_309E
	{0x301A, 0x10D8}, 	// RESET_REGISTER
	{0x3082, 0x0029}, 	// OPERATION_MODE_CTRL
	{0x301E, 0x00C8}, 	// DATA_PEDESTAL
	{0x3EDA, 0x0F03}, 	// RESERVED_MFR_3EDA
	{0x3EDE, 0xC005}, 	// RESERVED_MFR_3EDE
	{0x3ED8, 0x01EF}, 	// RESERVED_MFR_3ED8
	{0x3EE2, 0xA46B}, 	// RESERVED_MFR_3EE2
	{0x3EE0, 0x047D}, 	// RESERVED_MFR_3EE0
	{0x3EDC, 0x0070}, 	// RESERVED_MFR_3EDC
	{0x3044, 0x0404}, 	// DARK_CONTROL
	{0x3EE6, 0x4303}, 	// RESERVED_MFR_3EE6
	{0x3EE4, 0xD308}, 	// DAC_LD_24_25
	{0x3ED6, 0x00BD}, 	// RESERVED_MFR_3ED6
	{0x3EE6, 0x8303}, 	// RESERVED_MFR_3EE6
	{0x30E4, 0x6372}, 	// RESERVED_MFR_30E4
	{0x30E2, 0x7253}, 	// RESERVED_MFR_30E2
	{0x30E0, 0x5470}, 	// RESERVED_MFR_30E0
	{0x30E6, 0xC4CC}, 	// RESERVED_MFR_30E6
	{0x30E8, 0x8050}, 	// RESERVED_MFR_30E8
	{0x30B0, 0x1300}, 	// DIGITAL_TEST
	//DELAY=100

	{0x30D4, 0xE007}, 	// COLUMN_CORRECTION
	{0x30BA, 0x0008}, 	// DIGITAL_CTRL
	{0x3058, 0x003F}, 	// BLUE_GAIN
	{0x31D0, 0x0001}, 	// HDR_COMP

	{0x302C, 0x0002}, 	// VT_SYS_CLK_DIV
	{0x302A, 0x0004}, 	// VT_PIX_CLK_DIV
	{0x302E, 0x0002}, 	// PRE_PLL_CLK_DIV
	{0x3030, 0x002C}, 	// PLL_MULTIPLIER
	{0x30B0, 0x1300}, 	// DIGITAL_TEST

	{0x3064, 0x1982}, 	// EMBEDDED_DATA_CTRL
	{0x3058, 0x003F}, 	// BLUE_GAIN

	{0x3012, 0x01E6}, 	// COARSE_INTEGRATION_TIME

	{0x3100, 0x001B}, 	// AE_CTRL_REG
	{0x3112, 0x029F}, 	// AE_DCG_EXPOSURE_HIGH_REG
	{0x3114, 0x008C}, 	// AE_DCG_EXPOSURE_LOW_REG
	{0x3116, 0x02C0}, 	// AE_DCG_GAIN_FACTOR_REG
	{0x3118, 0x005B}, 	// AE_DCG_GAIN_FACTOR_INV_REG
	{0x3102, 0x0384}, 	// AE_LUMA_TARGET_REG
	{0x3104, 0x1000}, 	// AE_HIST_TARGET_REG
	{0x3126, 0x0080}, 	// AE_ALPHA_V1_REG
	{0x311C, 0x03DD}, 	// AE_MAX_EXPOSURE_REG
	{0x311E, 0x0003}, 	// AE_MIN_EXPOSURE_REG

	{0x301A, 0x10DC}, 	// RESET_REGISTER
};

/*==================================================================*/
 static SOCKET MT9M034_WriteBuf(TMT9M034Info *ptInfo, EMT9M034Regs eRegAddr, DWORD dwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[5];

	abyData[0] = (BYTE)(eRegAddr&0x00FF);

	if (dwDataLen == 1)
	{
		abyData[1] = (BYTE)(dwData&0x000000FF);
	}
	else if (dwDataLen == 2)
	{
		abyData[1] = (BYTE)((dwData&0xFF00)>>8);
		abyData[2] = (BYTE)(dwData&0x00FF);
	}
	else
	{
		abyData[1] = (BYTE)((dwData&0xFF000000)>>24);
		abyData[2] = (BYTE)((dwData&0x00FF0000)>>16);
		abyData[3] = (BYTE)((dwData&0x0000FF00)>>8);
		abyData[4] = (BYTE)(dwData&0x000000FF);
	}

	return ptInfo->pfnWriteBuf(ptInfo->dwFuncUserData, (ptInfo->dwDeviceAddr&0xFF), ((eRegAddr&0xFF00)>>8), (dwDataLen+1), abyData);
}
#if 0
 static SOCKET MT9M034_WriteBuf(TMT9M034Info *ptInfo, EMT9M034Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen, BYTE bWaitCmpt)
{
	BYTE abyData[20];
	DWORD dwIndex;

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
static SOCKET MT9M034_ReadBuf(TMT9M034Info *ptInfo, EMT9M034Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen)
{
	BYTE abyData[30];

	abyData[0] = (BYTE)(eRegAddr&0x00FF);
	abyData[1] = 0;
	if (ptInfo->pfnReadBuf(ptInfo->dwFuncUserData, ptInfo->dwDeviceAddr, ((eRegAddr&0xFF00)>>8), dwDataLen, abyData) < 0)
	{
		return S_FAIL;
	}
	else
	{
		if (dwDataLen == 1)
		{
			pdwData[0] = (abyData[0]&0xFF);
		}
		else if (dwDataLen == 2)
		{
			pdwData[0] = ((abyData[0]<<8)&0xFF00) + (abyData[1]&0xFF);
		}
		else
		{
			pdwData[0] = ((abyData[0]<<24)&0xFF000000) + ((abyData[1]<<16)&0x00FF0000) + ((abyData[2]<<8)&0x0000FF00) + (abyData[3]&0x000000FF);
		}
		return S_OK;
	}
}

/*-------------------------------------------------------------------------------------*/
static SOCKET MT9M034_CompBuf(TMT9M034Info *ptInfo, EMT9M034Regs eRegAddr, DWORD dwMask, DWORD dwData, DWORD dwDataLen)
{
	DWORD dwReadData;

	MT9M034_ReadBuf(ptInfo, eRegAddr, &dwReadData, dwDataLen);
	return ((dwReadData&dwMask) == dwData);
}

/*-------------------------------------------------------------------------------------*/
static SOCKET MT9M034_UpdateBuf(TMT9M034Info *ptInfo, EMT9M034Regs eRegAddr, DWORD dwMask, DWORD dwData, DWORD dwDataLen)
{
	DWORD dwReadData, dwWriteData;

	if (MT9M034_ReadBuf(ptInfo, eRegAddr, &dwReadData, dwDataLen) < 0)
	{
		return -1;
	}
	dwWriteData = ((dwReadData&dwMask) | dwData);
	if (MT9M034_WriteBuf(ptInfo, eRegAddr, dwWriteData, dwDataLen, 0) < 0)
	{
		return -1;
	}
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void MT9M034_Reset(TMT9M034Info *ptInfo)
{
	DWORD dwIndex;

	// RESET_REGISTER	
	MT9M034_WriteBuf(ptInfo, 0x301A, 0x0058, 2, 0);
	mdelay(200);
	MT9M034_WriteBuf(ptInfo, 0x301A, 0x0001, 2, 0);
	mdelay(200);
	MT9M034_WriteBuf(ptInfo, 0x301A, 0x10D8, 2, 0);
	mdelay(200);

	if (ptInfo->bHDREnable)
	{
		for (dwIndex=0; dwIndex<MT9M034_HDR_SEQUENCE_TBL_SIZE; dwIndex++)
		{
			MT9M034_WriteBuf(ptInfo, atMT9M034HDRSequenceTbl[dwIndex].eRegAddr, atMT9M034HDRSequenceTbl[dwIndex].dwData, 2, 0);
		}

		MT9M034_WriteBuf(ptInfo, 0x309E, 0x0186, 2, 0); // RESERVED_MFR_309E

		mdelay(200);

		for (dwIndex=0; dwIndex<MT9M034_HDR_TBL_SIZE; dwIndex++)
		{
			MT9M034_WriteBuf(ptInfo, atMT9M034HDRTbl[dwIndex].eRegAddr, atMT9M034HDRTbl[dwIndex].dwData, 2, 0);
			if (atMT9M034HDRTbl[dwIndex].eRegAddr == 0x30B0)
			{
				mdelay(100);		
			}
		}

		MT9M034_WriteBuf(ptInfo, 0x3082, 0x0028, 2, 0);	
		ptInfo->dwWDRRatio = 256;
	}
	else
	{
		for (dwIndex=0; dwIndex<MT9M034_LINEAR_SEQUENCE_TBL_SIZE; dwIndex++)
		{
			MT9M034_WriteBuf(ptInfo, atMT9M034LinearSequenceTbl[dwIndex].eRegAddr, atMT9M034LinearSequenceTbl[dwIndex].dwData, 2, 0);
		}

		mdelay(200);

		for (dwIndex=0; dwIndex<MT9M034_LINEAR_TBL_SIZE; dwIndex++)
		{
			MT9M034_WriteBuf(ptInfo, atMT9M034LinearTbl[dwIndex].eRegAddr, atMT9M034LinearTbl[dwIndex].dwData, 2, 0);
			if (atMT9M034LinearTbl[dwIndex].eRegAddr == 0x30BA)
			{
				mdelay(100);		
			}
		}
	}

	if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==960))
	{
		MT9M034_WriteBuf(ptInfo, 0x3032, 0x0000, 2, 0);// DIGITAL_BINNING
		MT9M034_WriteBuf(ptInfo, 0x3002, 0x0002, 2, 0);// Y_ADDR_START
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3004, 0x0002, 2, 0);// X_ADDR_START
/* Version 1.0.0.1 modification, 2012.08.06 */
		MT9M034_WriteBuf(ptInfo, 0x3006, 0x03C5, 2, 0);// Y_ADDR_END
		MT9M034_WriteBuf(ptInfo, 0x3008, 0x0501, 2, 0);// X_ADDR_END
/* ========================== */
/* ========================== */

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x960_40FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x960_40FPS;
		}
		else
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x960_45FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x960_45FPS;
		}

		MT9M034_WriteBuf(ptInfo, 0x300A, dwMT9M034Vsync, 2, 0);// FRAME_LENGTH_LINES
		MT9M034_WriteBuf(ptInfo, 0x300C, dwMT9M034Hsync, 2, 0);// LINE_LENGTH_PCK

		// ROI
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3140, 0x0002, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3142, 0x0002, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3144, 0x0501, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3146, 0x03C1, 2, 0);
/* ========================== */

		if (ptInfo->bAEEnable)
		{
			MT9M034_WriteBuf(ptInfo, 0x311C, dwMT9M034Vsync, 2, 0);// AE_MAX_EXPOSURE_REG
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001B, 2, 0);
		}
		else
		{
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001A, 2, 0);
		}
	}
	else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
	{
		MT9M034_WriteBuf(ptInfo, 0x3032, 0x0000, 2, 0);// DIGITAL_BINNING
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3002, 0x007C, 2, 0);// Y_ADDR_START
		MT9M034_WriteBuf(ptInfo, 0x3004, 0x0002, 2, 0);// X_ADDR_START
/* Version 1.0.0.1 modification, 2012.08.06 */
		MT9M034_WriteBuf(ptInfo, 0x3006, 0x034F, 2, 0);// Y_ADDR_END
		MT9M034_WriteBuf(ptInfo, 0x3008, 0x0501, 2, 0);// X_ADDR_END
/* ========================== */
/* ========================== */

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x720_60FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x720_60FPS;
		}
		else
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x720_50FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x720_50FPS;
		}

		MT9M034_WriteBuf(ptInfo, 0x300A, dwMT9M034Vsync, 2, 0);// FRAME_LENGTH_LINES
		MT9M034_WriteBuf(ptInfo, 0x300C, dwMT9M034Hsync, 2, 0);// LINE_LENGTH_PCK

		// ROI
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3140, 0x0002, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3142, 0x007C, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3144, 0x0501, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3146, 0x034B, 2, 0);
/* ========================== */

		if (ptInfo->bAEEnable)
		{
			MT9M034_WriteBuf(ptInfo, 0x311C, dwMT9M034Vsync, 2, 0);// AE_MAX_EXPOSURE_REG
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001B, 2, 0);
		}
		else
		{
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001A, 2, 0);
		}
	}
/* Version 1.0.0.1 modification, 2012.08.06 */
	else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==800))
	{
		MT9M034_WriteBuf(ptInfo, 0x3032, 0x0000, 2, 0);// DIGITAL_BINNING
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3002, 0x0054, 2, 0);// Y_ADDR_START
		MT9M034_WriteBuf(ptInfo, 0x3004, 0x0002, 2, 0);// X_ADDR_START
		MT9M034_WriteBuf(ptInfo, 0x3006, 0x0377, 2, 0);// Y_ADDR_END
		MT9M034_WriteBuf(ptInfo, 0x3008, 0x0501, 2, 0);// X_ADDR_END
/* ========================== */

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x800_60FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x800_60FPS;
		}
		else
		{
/* Version 1.0.0.5 modification, 2013.01.30 */
			dwMT9M034Hsync = MT9M034_HSYNC_1280x800_50FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x800_50FPS;
/* ========================== */
		}

		MT9M034_WriteBuf(ptInfo, 0x300A, dwMT9M034Vsync, 2, 0);// FRAME_LENGTH_LINES
		MT9M034_WriteBuf(ptInfo, 0x300C, dwMT9M034Hsync, 2, 0);// LINE_LENGTH_PCK

		// ROI
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3140, 0x0002, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3142, 0x0054, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3144, 0x0501, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3146, 0x0373, 2, 0);
/* ========================== */

		if (ptInfo->bAEEnable)
		{
			MT9M034_WriteBuf(ptInfo, 0x311C, dwMT9M034Vsync, 2, 0);// AE_MAX_EXPOSURE_REG
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001B, 2, 0);
		}
		else
		{
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001A, 2, 0);
		}
	}
/* ========================== */
/* Version 1.0.0.2 modification, 2012.11.02 */
	else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
	{
		MT9M034_WriteBuf(ptInfo, 0x3032, 0x0002, 2, 0);// DIGITAL_BINNING  Horizontal and vertical binning
		MT9M034_WriteBuf(ptInfo, 0x3002, 0x0002, 2, 0);// Y_ADDR_START
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3004, 0x0002, 2, 0);// X_ADDR_START
		MT9M034_WriteBuf(ptInfo, 0x3006, 0x03C9, 2, 0);// Y_ADDR_END
		MT9M034_WriteBuf(ptInfo, 0x3008, 0x0505, 2, 0);// X_ADDR_END
/* ========================== */

		if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x960_40FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x960_40FPS;
		}
		else
		{
			dwMT9M034Hsync = MT9M034_HSYNC_1280x960_45FPS;
			dwMT9M034Vsync = MT9M034_VSYNC_1280x960_45FPS;
		}

		MT9M034_WriteBuf(ptInfo, 0x300A, dwMT9M034Vsync, 2, 0);// FRAME_LENGTH_LINES
		MT9M034_WriteBuf(ptInfo, 0x300C, dwMT9M034Hsync, 2, 0);// LINE_LENGTH_PCK

		// ROI
/* Version 1.0.0.5 modification, 2013.01.30 */
		MT9M034_WriteBuf(ptInfo, 0x3140, 0x0002, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3142, 0x0002, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3144, 0x0501, 2, 0);
		MT9M034_WriteBuf(ptInfo, 0x3146, 0x03C1, 2, 0);
/* ========================== */

		if (ptInfo->bAEEnable)
		{
			MT9M034_WriteBuf(ptInfo, 0x311C, dwMT9M034Vsync, 2, 0);// AE_MAX_EXPOSURE_REG
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001B, 2, 0);
		}
		else
		{
			MT9M034_WriteBuf(ptInfo, 0x3100, 0x001A, 2, 0);
		}
	}
/* ========================== */
	else
	{
		printk("[MT9M034] : Size is not supported (width=%d, height=%d).\n", (int)ptInfo->dwWidth, (int)ptInfo->dwHeight);
		return;
	}

/* Version 1.0.0.1 modification, 2012.08.06 */
	MT9M034_WriteBuf(ptInfo, 0x3064, 0x1802, 2, 0); // EMBEDDED_DATA_CTRL
/* ========================== */

/* Version 1.0.0.2 modification, 2012.11.02 */
/* Version 1.0.0.5 modification, 2013.01.30 */
	MT9M034_WriteBuf(ptInfo, 0x301E, 0x0018, 2, 0); // DATA_PEDESTAL
/* ========================== */
	MT9M034_WriteBuf(ptInfo, 0x30EA, 0x8C00, 2, 0); // Manual black clamp
	MT9M034_WriteBuf(ptInfo, 0x3180, 0x0000, 2, 0); 
	MT9M034_WriteBuf(ptInfo, 0x30EC, 0x0000, 2, 0); // Manual offset comp0 
	MT9M034_WriteBuf(ptInfo, 0x30EE, 0x0000, 2, 0); // Manual offset comp1 
	MT9M034_WriteBuf(ptInfo, 0x30F8, 0x0000, 2, 0); // Manual offset comp2 
	MT9M034_WriteBuf(ptInfo, 0x30FA, 0x0000, 2, 0); // Manual offset comp3 
/* ========================== */

/* Version 1.0.0.5 modification, 2013.01.30 */
	// [Column Retriggering at start up]
	MT9M034_WriteBuf(ptInfo, 0x30B0, 0x1300, 2, 0);
	MT9M034_WriteBuf(ptInfo, 0x30D4, 0xE007, 2, 0);
	MT9M034_WriteBuf(ptInfo, 0x30BA, 0x0008, 2, 0);
	MT9M034_WriteBuf(ptInfo, 0x301A, 0x10DC, 2, 0);
	mdelay(200);
	MT9M034_WriteBuf(ptInfo, 0x301A, 0x10D8, 2, 0);
	mdelay(200);

	MT9M034_WriteBuf(ptInfo, 0x301A, 0x10DC, 2, 0);
/* ========================== */
/* Version 1.0.0.3 modification, 2012.11.09 */
	ptInfo->dwColumnGain = 0xFFFF;
	ptInfo->dwDACGain = 0xFFFF;
	ptInfo->dwDCGGain = 0xFFFF;
	ptInfo->dwGlobalGain = 0xFFFF;
/* ========================== */

	printk("MT9M034_Reset Done \n");
}

/*-------------------------------------------------------------------------------------*/
void MT9M034_SetShutter(DWORD dwShutter)
{
	DWORD dwValue;
	QWORD qwValue;
	
	if (ptInfo->bAEEnable) return;

	qwValue = dwShutter * MT9M034_PIXEL_CLOCK_M_100;
	do_div(qwValue, 100);
	do_div(qwValue, dwMT9M034Hsync);
	dwValue = (DWORD)qwValue;

	if (dwValue > 0x0000FFFF)
	{
		dwValue = 0x0000FFFF;
	}
	else if (dwValue < 10)
	{
		dwValue = 10;
	}

	MT9M034_WriteBuf(ptInfo, 0x3012, dwValue, 2, 0);
	ptInfo->dwCurShutter = dwShutter;
}

/*-------------------------------------------------------------------------------------*/
/* Version 1.0.0.3 modification, 2012.11.09 */
void MT9M034_SetGain(TMT9M034Info *ptInfo, DWORD dwGain)
/* ========================== */
{
/* Version 1.0.0.2 modification, 2012.11.02 */
	DWORD dwAnalogGain0 = 0;
	DWORD dwDigitalGain = 0, dwDCGGain = 0;
/* ========================== */

	if (ptInfo->bAEEnable) return;

/* Version 1.0.0.1 modification, 2012.08.06 */
	if (dwGain > MT9M034_MAX_GAIN)
	{
		dwGain = MT9M034_MAX_GAIN;
/* ========================== */
	}
	else if (dwGain < 1000)
	{
		dwGain = 1000;
	}

/* Version 1.0.0.2 modification, 2012.11.02 */
/* Version 1.0.0.3 modification, 2012.11.09 */
	if (ptInfo->bHDREnable)
	{
		if (dwGain < 4000)
		{
			dwAnalogGain0 = 0x1300;
			dwDigitalGain = dwGain*32/1000;
		}
		else if (dwGain < 16000)
		{
			dwAnalogGain0 = 0x1320;
			dwDigitalGain = dwGain*32/4000;
		}
		else
		{
			dwAnalogGain0 = 0x1330;
			dwDigitalGain = dwGain*32/8000;
		}
		dwDCGGain = 0x001A;
	}
	else
	{
		if (dwGain < 2000)
		{
			dwAnalogGain0 = 0x1300; // 1X
			dwDCGGain = 0x001A; // 1X
			dwDigitalGain = dwGain*32/1000;
		}
		else if (dwGain < 2880)
		{
			dwAnalogGain0 = 0x1310; // 2X
			dwDCGGain = 0x001A; // 1X
			dwDigitalGain = dwGain*32/2000;
		}
		else if (dwGain < 5760)
		{
			dwAnalogGain0 = 0x1300;  // 1X
			dwDCGGain = 0x001E; // 2.88X
			dwDigitalGain = dwGain*32/2880;
		}
		else if (dwGain < 11520)
		{
			dwAnalogGain0 = 0x1310;  // 2X
			dwDCGGain = 0x001E; // 2.88X
			dwDigitalGain = dwGain*32/5760;
		}
		else if (dwGain < 23040)
		{
			dwAnalogGain0 = 0x1320; // 4X
			dwDCGGain = 0x001E; // 2.88X
			dwDigitalGain = dwGain*32/11520;
		}
		else
		{
			dwAnalogGain0 = 0x1330; // 8X
			dwDCGGain = 0x001E; // 2.88X
			dwDigitalGain = dwGain*32/23040;
		}
/* ========================== */
/* ========================== */
	}

/* Version 1.0.0.2 modification, 2012.11.02 */
/* Version 1.0.0.3 modification, 2012.11.09 */
	if (ptInfo->dwGlobalGain != dwDigitalGain)
	{	
		MT9M034_WriteBuf(ptInfo, 0x305E, (dwDigitalGain&0xFF), 2, 0); //D Gain
		ptInfo->dwGlobalGain = dwDigitalGain;
	}
	if (ptInfo->dwDCGGain != dwDCGGain)
	{
		MT9M034_WriteBuf(ptInfo, 0x3100, (dwDCGGain&0xFFFF), 2, 0); //DGC Gain	
		ptInfo->dwDCGGain = dwDCGGain;
	}
	if (ptInfo->dwColumnGain != dwAnalogGain0)
	{
		MT9M034_WriteBuf(ptInfo, 0x30B0, (dwAnalogGain0&0xFFFF), 2, 0); //A0 Gain
		ptInfo->dwColumnGain = dwAnalogGain0;
	}
/* ========================== */
/* ========================== */
}

/*==================================================================*/
SOCKET  MT9M034_Ioctl(TVideoSignalOptions* ptIoctlArg, DWORD dwDevNum)
{
	SOCKET SCKRet = 0;
	DWORD adwAEMeasureWinTbl[2];

	if (down_interruptible(&MT9M034_mutex))
	{
		return -ERESTARTSYS;	
	}

	switch (ptIoctlArg->eOptionFlags)
	{
		case VIDEO_SIGNAL_OPTION_RESET:
		MT9M034_Reset(ptInfo);
		break;
		case VIDEO_SIGNAL_OPTION_SET_SHUTTER_CTRL:
		MT9M034_SetShutter(ptIoctlArg->adwUserData[0]);
		break;
		case VIDEO_SIGNAL_OPTION_SET_GLOBALGAIN_CTRL:
/* Version 1.0.0.3 modification, 2012.11.09 */
		MT9M034_SetGain(ptInfo, ptIoctlArg->adwUserData[0]);
/* ========================== */
		break;
		case VIDEO_SIGNAL_OPTION_SET_FREQUENCY:
		if ((ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_50HZ) && (ptIoctlArg->adwUserData[0]!=VIDEO_SIGNAL_FREQUENCY_60HZ))
		{
			printk("[MT9M034] : Frequency is not supported (%ld Hz).\n", ptIoctlArg->adwUserData[0]);
			SCKRet = -EPERM;
			break;
		}
		ptInfo->dwFreq = ptIoctlArg->adwUserData[0];

		if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==960))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x960_40FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x960_40FPS;
			}
			else
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x960_45FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x960_45FPS;
			}
		}
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==720))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x720_60FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x720_60FPS;
			}
			else
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x720_50FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x720_50FPS;
			}
		}
/* Version 1.0.0.5 modification, 2013.01.30 */
		else if ((ptInfo->dwWidth==1280) && (ptInfo->dwHeight==800))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x800_60FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x800_60FPS;
			}
			else
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x800_50FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x800_50FPS;
			}
		}
		else if ((ptInfo->dwWidth==640) && (ptInfo->dwHeight==480))
		{
			if (ptInfo->dwFreq == VIDEO_SIGNAL_FREQUENCY_60HZ)
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x960_40FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x960_40FPS;
			}
			else
			{
				dwMT9M034Hsync = MT9M034_HSYNC_1280x960_45FPS;
				dwMT9M034Vsync = MT9M034_VSYNC_1280x960_45FPS;
			}
		}
/* ========================== */

		MT9M034_WriteBuf(ptInfo, 0x300A, dwMT9M034Vsync, 2, 0);// FRAME_LENGTH_LINES
		MT9M034_WriteBuf(ptInfo, 0x300C, dwMT9M034Hsync, 2, 0);// LINE_LENGTH_PCK

		break;
#if 0
		case VIDEO_SIGNAL_OPTION_SET_WDR:
		MT9M034_WriteBuf(ptInfo, 0x301A, 0x0058, 2, 0);
		mdelay(30);
		switch (ptIoctlArg->adwUserData[0])
		{
			case 0:
			MT9M034_WriteBuf(ptInfo, 0x3082, 0x0001, 2, 0);
			ptInfo->dwWDRRatio = 1;
			break;
			case 2:
			MT9M034_WriteBuf(ptInfo, 0x3082, 0x0004, 2, 0);		
			ptInfo->dwWDRRatio = 8;
			break;
			case 5:
			MT9M034_WriteBuf(ptInfo, 0x3082, 0x0008, 2, 0);
			ptInfo->dwWDRRatio = 16;
			break;
			case 8:
			MT9M034_WriteBuf(ptInfo, 0x3082, 0x000C, 2, 0);		
			ptInfo->dwWDRRatio = 32;
			break;
			default:
			printk("[MT9M034] : Not supported.\n");
			break;
		}
		break;
#endif
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_WINDOWS:
		case VIDEO_SIGNAL_OPTION_SET_AUTO_EXPOSURE_MODE:
		if (sensor_dev_ops.get_exp_statistic)
		{
			memcpy(adwAEMeasureWinTbl, (DWORD *)ptIoctlArg->adwUserData[0], sizeof(DWORD)*2);
			MT9M034_WriteBuf(ptInfo, _3140_AERoiXStartOffset, (adwAEMeasureWinTbl[0]&0x1FFF), 2, 0);	
			MT9M034_WriteBuf(ptInfo, _3142_AERoiYStartOffset, ((adwAEMeasureWinTbl[0]>>16)&0x1FFF), 2, 0);	
			MT9M034_WriteBuf(ptInfo, _3144_AERoiXSize, (adwAEMeasureWinTbl[1]&0x1FFF), 2, 0);	
			MT9M034_WriteBuf(ptInfo, _3146_AERoiYSize, ((adwAEMeasureWinTbl[1]>>16)&0x1FFF), 2, 0);	
		}
		else
		{
			SCKRet = -1;
		}
		break;
		default:
		SCKRet = -EPERM;
	}

	up(&MT9M034_mutex);

	return SCKRet;
}

/*-------------------------------------------------------------------------------------*/
void MT9M034_Release(DWORD dwDevNum)
{
	IICCtrl_Release((void*)(&(ptInfo->dwFuncUserData)));
	kfree(ptInfo);
}

/*-------------------------------------------------------------------------------------*/
SOCKET MT9M034_Open(TVideoSensorInitialParam* ptMT9M034InitialParam, DWORD dwDevNum)
{
	TIICCtrlInitOptions tIICCtrlInitOptions;
	void* hIICCtrlObject;

	if (((ptMT9M034InitialParam->dwVideoSensorVersion&0x00FF00FF)!=(MT9M034_INTERFACE_VERSION&0x00FF00FF)) ||
		((ptMT9M034InitialParam->dwVideoSensorVersion&0x0000FF00)>(MT9M034_INTERFACE_VERSION&0x0000FF00)))
	{
		printk("Invalid MT9M034 device driver version %d.%d.%d.%d !!\n", 
				(int)(MT9M034_INTERFACE_VERSION&0xFF), 
				(int)((MT9M034_INTERFACE_VERSION>>8)&0xFF), 
				(int)((MT9M034_INTERFACE_VERSION>>16)&0xFF), 
				(int)(MT9M034_INTERFACE_VERSION>>24)&0xFF);
		return S_INVALID_VERSION;
	}

	if (!(ptInfo=(TMT9M034Info *)kmalloc(sizeof(TMT9M034Info), GFP_KERNEL)))
	{
		printk("[MT9M034] : Allocate %d bytes memory fail\n", sizeof(TMT9M034Info));
		return -ENOMEM;
	}

	// init iic
	tIICCtrlInitOptions.dwTransType = IICCTRL_TYPE_ADDR_16_BITS_DATA;
	tIICCtrlInitOptions.dwMaxDataLength = 30;
	tIICCtrlInitOptions.dwBusNum = 0;
	if (IICCtrl_Initial(&hIICCtrlObject, &tIICCtrlInitOptions))
	{
		printk("[MT9M034] : Initial IICCtrl object fail !!\n");
		MT9M034_Release(dwDevNum);			
		return -ENODEV;
	}

	// init private data
	ptInfo->dwDeviceAddr = MT9M034_DEAFULT_DEVICE_ADDR;
	ptInfo->dwFuncUserData = (DWORD)hIICCtrlObject;
	ptInfo->pfnReadBuf = (FOnReadBuf)IICCtrl_ReadBuf;
	ptInfo->pfnWriteBuf = (FOnWriteBuf)IICCtrl_WriteBuf;
	ptInfo->dwFreq = (DWORD)ptMT9M034InitialParam->dwVideoSensorFrequency;
	ptInfo->dwCurShutter = 33333;
	ptInfo->dwWidth = (DWORD)video_sensor_width;
	ptInfo->dwHeight = (DWORD)video_sensor_height;
	ptInfo->bHDREnable = sckEnHDR;
	ptInfo->bAEEnable = sckEnAE;

	// Detect sensor device
	if (!MT9M034_CompBuf(ptInfo, _3000_ChipVersion, 0xFFFF, MT9M034_CHIP_VERSION, 2))
	{
		printk("\n[MT9M034] : Sensor device doesn't exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
		MT9M034_Release(dwDevNum);				
		return -ENODEV;
	}
	else
	{
		printk("\n[MT9M034] : Sensor device exist in device address 0x%02X\n", (int)ptInfo->dwDeviceAddr);
	}

	// Reset device
	MT9M034_Reset(ptInfo);

	return 0;
}

/*-------------------------------------------------------------------------------------*/
SOCKET MT9M034_GetExpStatistic(TVideoSensorStatistic* ptMT9M034Statistic, DWORD dwDevNum)
{
	DWORD dwReadData[64]={0}, dwTemp;

	if (down_interruptible(&MT9M034_mutex))
	{
		return -ERESTARTSYS;	
	}	

	MT9M034_ReadBuf(ptInfo, 0x3150, &dwReadData[0], 2);	

	ptMT9M034Statistic->dwLumaHorSubWindowNum = 1;
	ptMT9M034Statistic->dwLumaVerSubWindowNum = 1;

	dwTemp = ((dwReadData[0]&0x000F)<<16)|(dwReadData[1]&0xFFFF);
	ptInfo->adwStatisticY = (dwTemp*ptInfo->dwWDRRatio)>>8;

	up(&MT9M034_mutex);

	ptMT9M034Statistic->pdwStatisticY = &ptInfo->adwStatisticY;

	ptMT9M034Statistic->dwHistogramBinNum = 0;
	ptMT9M034Statistic->pdwHistogram = NULL;

	ptMT9M034Statistic->dwWBHorSubWindowNum = 0;
	ptMT9M034Statistic->dwWBVerSubWindowNum = 0;
	ptMT9M034Statistic->pdwStatisticR = NULL;
	ptMT9M034Statistic->pdwStatisticG = NULL;
	ptMT9M034Statistic->pdwStatisticB = NULL;

	return 0;
}

/* Version 1.0.0.1 modification, 2012.08.06 */
/*-------------------------------------------------------------------------------------*/
DWORD MT9M034_GetMaxGain(void)
{
	return (MT9M034_MAX_GAIN);
}
/* ========================== */

/*-------------------------------------------------------------------------------------*/
static TVideoSensorDevice sensor_dev_ops =
{
	.open = 	MT9M034_Open,
	.release = 	MT9M034_Release,
	.ioctl = 	MT9M034_Ioctl,
	.get_shutter_value = NULL,
	.get_remaining_line_num = NULL,
	.group_access = NULL,	
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
/* Version 1.0.0.1 modification, 2012.08.06 */
	.get_exp_statistic = NULL,
	.get_wb_statistic = NULL,
	.get_max_gain = MT9M034_GetMaxGain,
/* ========================== */
};
EXPORT_SYMBOL(sensor_dev_ops);

/*==================================================================*/
struct MT9M034_attribute {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj, char *buf);
};

/**
 * Sysfs subsystem: forward read calls
 *
 * Sysfs operation for forwarding read call to the show method of the
 * attribute owner
 */
ssize_t MT9M034_default_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct MT9M034_attribute *MT9M034_attr = container_of(attr, struct MT9M034_attribute, attr);
	int result = 0;
	
	if (MT9M034_attr->show)
		result = MT9M034_attr->show(kobj, buf);	
	return result;
}

ssize_t MT9M034_SensorName_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "MT9M034");
	return retval;
}

ssize_t MT9M034_dwBufNum_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "5");
	return retval;
}

ssize_t MT9M034_MaxFrameWidth_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "1280");
	return retval;
}

ssize_t MT9M034_MaxFrameHeight_attr_show(struct kobject *kobj, char *buf)
{
	int retval;
	retval = sprintf (buf, "%s\n", "960");
	return retval;
}
/*-------------------------------------------------------------------------------------*/
struct kobject MT9M034_kobj;

struct MT9M034_attribute MT9M034_SensorName_attr = {
	.attr = {
		.name = "SensorName",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9M034_SensorName_attr_show,
};

struct MT9M034_attribute MT9M034_dwBufNum_attr = {
	.attr = {
		.name = "dwBufNum",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9M034_dwBufNum_attr_show,
};

struct MT9M034_attribute MT9M034_MaxFrameWidth_attr = {
	.attr = {
		.name = "MaxFrameWidth",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9M034_MaxFrameWidth_attr_show,
};

struct MT9M034_attribute MT9M034_MaxFrameHeight_attr = {
	.attr = {
		.name = "MaxFrameHeight",
		.mode = S_IRUGO | S_IWUSR,
	},
	.show = MT9M034_MaxFrameHeight_attr_show,
};

struct sysfs_ops MT9M034_sysfs_ops = {
	.show	= MT9M034_default_attr_show,
};

struct kobj_type MT9M034_ktype = {
	.sysfs_ops	= &MT9M034_sysfs_ops,
};

/*-------------------------------------------------------------------------------------*/
static SOCKET MT9M034_Init(void)
{
	SOCKET sckResult = 0;
	
	if (sckResult < 0)
	{
		printk("[MT9M034] : kobj set name fail!\n");
		return -1;
	}
	
	printk(KERN_INFO "MT9M034() sensor_width=%d sensor_height=%d\n", video_sensor_width, video_sensor_height);
	MT9M034_kobj.ktype = &MT9M034_ktype;	
	sckResult = kobject_init_and_add(&MT9M034_kobj, &MT9M034_ktype, NULL, "VideoSensorInfo");

	if (sckResult < 0)
	{
		printk("[MT9M034] : Cannot register kobject [MT9M034_kobj].\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MT9M034_kobj, &MT9M034_SensorName_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9M034] : Cannot create MT9M034_SensorName_attr.attr.\n");
		return -1;
       }

	sckResult = sysfs_create_file(&MT9M034_kobj, &MT9M034_dwBufNum_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9M034] : Cannot create MT9M034_dwBufNum_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MT9M034_kobj, &MT9M034_MaxFrameWidth_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9M034] : Cannot create MT9M034_MaxFrameWidth_attr.attr.\n");
		return -1;
	}

	sckResult = sysfs_create_file(&MT9M034_kobj, &MT9M034_MaxFrameHeight_attr.attr);		
	if (sckResult < 0)
	{
		printk("[MT9M034] : Cannot create MT9M034_MaxFrameHeight_attr.attr.\n");
		return -1;
	}	   
	sema_init(&MT9M034_mutex, 1);
		
	return 0;
}

/*-------------------------------------------------------------------------------------*/
static void MT9M034_Exit(void)
{
	sysfs_remove_file(&MT9M034_kobj, &(MT9M034_SensorName_attr.attr));
	sysfs_remove_file(&MT9M034_kobj, &(MT9M034_dwBufNum_attr.attr));	
	sysfs_remove_file(&MT9M034_kobj, &(MT9M034_MaxFrameWidth_attr.attr));
	sysfs_remove_file(&MT9M034_kobj, &(MT9M034_MaxFrameHeight_attr.attr));	
	kobject_put(&MT9M034_kobj);	
	printk(KERN_DEBUG "[MT9M034] : bye\n");
	return;
}

/*-------------------------------------------------------------------------------------*/
module_init(MT9M034_Init);
module_exit(MT9M034_Exit);
