/*
 * $Header: /rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C/gpio_i2c.c 32    12/09/25 3:13p Vincent $
 *
 * gpio_i2c
 * Driver for GPIO I2C.
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
 *
 * $History: gpio_i2c.c $
 * 
 * *****************  Version 32  *****************
 * User: Vincent      Date: 12/09/25   Time: 3:13p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Register GPIO pin to Linux Kernel by gpio_request,
 * gpio_request will help to disable muxed AGPO pin when request pin is
 * 0~7.
 *
 * *****************  Version 31  *****************
 * User: Evelyn       Date: 12/01/05   Time: 3:04p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 26  *****************
 * User: Evelyn       Date: 10/04/30   Time: 5:33p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 25  *****************
 * User: Alan         Date: 09/12/09   Time: 4:27p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * FEATURE: GPIO I2C bus0 clock and common data pins could be dynamically
 * assigned by module parameters "iSCL_bus0" and "iSDA_bus" when
 * installing module - DONE.
 *
 * *****************  Version 24  *****************
 * User: Alan         Date: 09/12/04   Time: 2:16a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Replace __GPIO_I2C_IN_ISR__ with GPIOI2C_INTR. If
 * GPIOI2C_INTR is defined, SPI will be done in ISR - DONE.
 * BUG: Only check GPIOI2C_INTR to decide if the shared interrupt is for
 * GPIO_I2C device driver - DONE.
 *
 * *****************  Version 23  *****************
 * User: Alan         Date: 09/11/22   Time: 10:14p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * BUG: The dev_id of free_irq should be consistent with request_irq -
 * FIXED.
 *
 * *****************  Version 22  *****************
 * User: Alan         Date: 09/11/22   Time: 2:08p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * BUG: GPIO_I2C_ReadByte in ISR should use call by pointer, not value -
 * FIXED.
 *
 * *****************  Version 21  *****************
 * User: Alan         Date: 09/11/21   Time: 8:04p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: GPIOI2C_NO_ACK is change to 0x0002 and add
 * GPIOI2C_IGNORE_ACK (0x0008) - DONE.
 * MODIFICATION: Only toggle GPIOI2C_SEN in the fast (SPI) mode and set to
 * DIR_IN when not used - DONE.
 * MODIFICATION: Use LINUX_VERSION_CODE to decide the related codes for
 * Linux 2.6.24 and above - DONE.
 *
 * *****************  Version 20  *****************
 * User: Alan         Date: 09/11/15   Time: 12:09p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 19  *****************
 * User: Alan         Date: 09/09/25   Time: 7:08p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * FEATURE: Add read operation to the fast (SPI) mode including the
 * interrupt mechanism in the write operation - DONE.
 *
 * *****************  Version 18  *****************
 * User: Alan         Date: 09/08/21   Time: 4:03p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: Configure the sensor without waiting ISR if the VSYNC is
 * already low - DONE.
 *
 * *****************  Version 17  *****************
 * User: Alan         Date: 09/08/19   Time: 10:15p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * OPTIMIZATION: To further shorten the config time for the fast (SPI)
 * mode, move all GPIO MMR progrmming into ISR - DONE.
 *
 * *****************  Version 16  *****************
 * User: Alan         Date: 09/08/18   Time: 11:57a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 15  *****************
 * User: Albert.shen  Date: 09/05/12   Time: 9:48p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 14  *****************
 * User: Albert.shen  Date: 09/05/12   Time: 6:23p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 13  *****************
 * User: Albert.shen  Date: 09/05/05   Time: 10:17p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 12  *****************
 * User: Albert.shen  Date: 09/04/23   Time: 3:32p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 11  *****************
 * User: Evelyn       Date: 09/04/02   Time: 11:40a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 10  *****************
 * User: Aniki        Date: 09/04/01   Time: 1:54p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 9  *****************
 * User: Evelyn       Date: 09/03/13   Time: 4:37p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 08/12/11   Time: 10:59a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 7  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 6  *****************
 * User: Ycchang      Date: 08/09/15   Time: 9:05a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 5  *****************
 * User: Poplar       Date: 08/01/08   Time: 3:43p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: kfree private_data and set filp->private_data = NULL
 *
 * *****************  Version 4  *****************
 * User: Alan         Date: 07/12/31   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 3  *****************
 * User: Ycchang      Date: 07/11/23   Time: 2:33p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 2  *****************
 * User: Alan         Date: 06/05/15   Time: 9:40a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * FEATURE: Add ID string in device driver - DONE.
 * FEATURE: Support 4 I2C buses with shared data bit - DONE.
 * FEATURE: Add an object information structure to handle multiple open -
 * DONE.
 * MODIFICATION: The private_data in file structure pointer stores object
 * information rather than the device structure - DONE.
 *
 * *****************  Version 1  *****************
 * User: Chikuang     Date: 06/01/23   Time: 11:36a
 * Created in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 14  *****************
 * User: Sor          Date: 04/12/29   Time: 4:41p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 * FEATURE: Support non-stop bit read/write - DONE.
 *
 * *****************  Version 13  *****************
 * User: Aren         Date: 04/09/09   Time: 1:58p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 * MODIFICATION: modified for vivaldi with kernel 2.4.26
 *
 * *****************  Version 12  *****************
 * User: Steve        Date: 04/07/11   Time: 9:18p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 10  *****************
 * User: Steve        Date: 04/07/06   Time: 10:34p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 9  *****************
 * User: Vicky        Date: 04/07/06   Time: 4:33p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 8  *****************
 * User: Vicky        Date: 04/07/06   Time: 10:01a
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 7  *****************
 * User: Vicky        Date: 04/07/02   Time: 5:33p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 6  *****************
 * User: Sor          Date: 04/06/24   Time: 12:35p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 5  *****************
 * User: Sor          Date: 04/06/24   Time: 12:33p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 4  *****************
 * User: Vicky        Date: 04/06/16   Time: 5:27p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 3  *****************
 * User: Vicky        Date: 04/06/16   Time: 4:41p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 2  *****************
 * User: Vicky        Date: 04/06/12   Time: 11:09a
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 1  *****************
 * User: Vicky        Date: 04/06/12   Time: 10:44a
 * Created in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 */

/* ============================================================================================= */
#ifndef __KERNEL__
    #define __KERNEL__
#endif
#ifndef MODULE
    #define MODULE
#endif

/* ============================================================================================= */
/* Version 3.0.0.8 modification, 2009.04.01 */
#include <linux/moduleparam.h>
/* ======================================== */
#include "gpio_i2c_locals.h"

/* ============================================================================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.4 modification, 2008.09.15 */
const CHAR GPIOI2C_ID[] = "$Version: "GPIOI2C_ID_VERSION"  (GPIOI2C) $";
/* ======================================== */
static const CHAR GPIOI2C_DATE[] = "$Date: 12/09/25 3:13p $";
static const CHAR GPIOI2C_TIME[] = "$Time: "__TIME__"    (GPIOI2C) $";
/* ======================================== */

void *GPIO_I2C_base;
void *GPIO_I2C_Reg;

/* Version 3.1.0.5 modification, 2011.11.10 */
void *SYSC_base;
/* ======================================== */
static GPIO_I2C_Dev *g_GPIO_I2C_devices;    /* This driver instance */
#ifdef GPIOI2C_MAJOR_DEFAULT
static int g_GPIO_I2C_major = GPIOI2C_MAJOR_DEFAULT;
#else
static int g_GPIO_I2C_major = 0;
#endif

/* Version 3.1.0.2 modification, 2009.12.09 */
static int iSCL_bus0 = 0;
static int iSDA_bus = 0;    // common data line for scl withou sda definition
/* ======================================== */
/* Version 3.0.0.8 modification, 2009.04.01 */
static int iSCL_bus1 = 0;
static int iSDA_bus1 = 0;
/* Version 3.0.0.16 modification, 2009.08.19 */
static int iBuffLen = 16;

module_param(iBuffLen, int, S_IRUGO|S_IWUSR);
/* ========================================= */

/* Version 3.1.0.2 modification, 2009.12.09 */
module_param(iSCL_bus0, int, S_IRUGO|S_IWUSR);
module_param(iSDA_bus, int, S_IRUGO|S_IWUSR);
/* ======================================== */
module_param(iSCL_bus1, int, S_IRUGO|S_IWUSR);
module_param(iSDA_bus1, int, S_IRUGO|S_IWUSR);
/* ======================================== */

static int iHD_STA  = T_HD_STA; // spec is 4.0 we use 5
static int iLOW  = T_LOW; // spec is 4.7 we use 6
static int iHIGH = T_HIGH; // spec is 4.0 we use 5
static int iSU_STA = T_SU_STA; // spec is 4.7 we use 6
static int iHD_DAT = T_HD_DAT; // spec is 5 we use 6
static int iSU_DAT = T_SU_DAT; // spec is 0.25 we use 1
static int iSU_STO = T_SU_STO; // spec is 4 we use 5
static int iBUF = T_BUF; // spec is 4.7 we use 6

/* ============================================================================================= */
/* Version 3.0.0.1 modification, 2007.12.31 */
/* ======================================== */

static void setdir(int pin, int dir)
{
    int ret;
    if (dir == DIR_IN) {
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_PINDIR);
        writel(ret & (~pin), GPIO_I2C_Reg + GPIOC_MMR_PINDIR);
    }
    else if (dir == DIR_OUT) {
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_PINDIR);
        writel(ret | pin, GPIO_I2C_Reg + GPIOC_MMR_PINDIR);
    }
    else {
        printk("gpio_i2c error: wrong direction\n");
    }
    return;
}

static int readpin(int pin)
{
    if (readl(GPIO_I2C_Reg + GPIOC_MMR_DATAIN) & pin) {
        return 1;
    }
    return 0;
}
static void setpinhigh(int pin)
{
    /* set direction to output */
    setdir(pin, DIR_OUT);
    /* set pin */
    writel(pin, GPIO_I2C_Reg + GPIOC_MMR_DATASET);
    return;
}

static void setpinlow(int pin)
{
    /* set direction to output */
    setdir(pin, DIR_OUT);
    /* clear pin */
    writel(pin, GPIO_I2C_Reg + GPIOC_MMR_DATACLR);
    return;
}

static void waitpinhigh(int pin)
{
    while (readpin(pin) == 0) {
/* Version 3.0.0.1 modification, 2007.12.31 */
        udelay(1);
/* ======================================== */
    }
    return;
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
static void setpin(int pin, int bit)
{
    if (bit)
    {
        writel(pin, GPIO_I2C_Reg + GPIOC_MMR_DATASET);
    }
    else
    {
        writel(pin, GPIO_I2C_Reg + GPIOC_MMR_DATACLR);
    }
}

static void write_bit(int bit, int iSCL, int iSDA, BOOL bSPI)
/* ========================================= */
/* ======================================== */
/* ======================================== */
{
/* Version 3.0.0.15 modification, 2009.08.14 */
    if (bSPI == TRUE)
    {
        setpin(iSCL, 0);
        setpin(iSDA, bit);
        if (iSU_DAT != 0)
        {
            udelay(iSU_DAT);
        }
        setpin(iSCL, 1);
        if (iHD_DAT != 0)
        {
            udelay(iHD_DAT);
        }
    }
    else
    {
/* ========================================= */
        if (bit) {
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.1.0.4 modification, 2011.06.09 */
            setdir(iSDA, DIR_IN);
/* ======================================== */
            waitpinhigh(iSDA);
/* ======================================== */
        }
        else {
/* Version 3.0.0.6 modification, 2009.03.12 */
            setpinlow(iSDA);
/* ======================================== */
        }
/* Version 3.0.0.10 modification, 2009.04.22 */
        udelay(iSU_DAT);
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
        setpinhigh(iSCL);
        waitpinhigh(iSCL);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
        udelay(iHIGH);
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
        setpinlow(iSCL);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
            udelay(iHD_DAT);
/* ========================================= */
/* Version 3.0.0.15 modification, 2009.08.14 */
    }
/* ========================================= */
    return;
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.17 modification, 2009.09.07 */
/* ========================================= */
static int read_bit(int iSCL, int iSDA, BOOL bSPI)
/* ======================================== */
/* ======================================== */
{
    int ret;
/* Version 3.0.0.17 modification, 2009.09.07 */
    if (bSPI == TRUE)
    {
        setpin(iSCL, 0);
        ret = readpin(iSDA);
        if (iSU_DAT != 0)
        {
            udelay(iSU_DAT);
        }
        setpin(iSCL, 1);
        ret = readpin(iSDA);
        if (iHD_DAT != 0)
        {
            udelay(iHD_DAT);
        }
    }
    else
    {
/* ========================================= */
/* Version 3.0.0.6 modification, 2009.03.12 */
        setdir(iSDA, DIR_IN);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
        udelay(iSU_DAT);
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
        setpinhigh(iSCL);
        waitpinhigh(iSCL);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
        udelay(iHIGH);
/* ========================================= */
/* Version 3.0.0.6 modification, 2009.03.12 */
        ret = readpin(iSDA);
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
        setpinlow(iSCL);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
        udelay(iHD_DAT);
/* ========================================= */
/* Version 3.0.0.17 modification, 2009.09.07 */
    }
/* ========================================= */
    return ret;
}
/* Version 3.1.0.7 modification, 2012.09.25 */
#ifdef __ASM_ARCH_PLATFORM_MOZART_H__
static void register_gpio(int pin)
{
    int i = 0;
    for (i=0; i<8; i++)
    {
        if ((1<<i) & pin)
        {
            gpio_request(i, NULL);
            break;
        }
    }
}
#else
static inline void register_gpio(int pin) {}
#endif /* __ASM_ARCH_PLATFORM_MOZART_H__ */
/* ======================================== */
/* I2C start signal (S): SDA change from high to low while SCL remain at high */
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
static void GPIO_I2C_Start(int iSCL, int iSDA)
/* ======================================== */
/* ======================================== */
{
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iBUF);
/* ========================================= */
/* Version 3.0.0.6 modification, 2009.03.12 */
    if (readpin(iSDA) == 0) {
        setpinhigh(iSDA);
        waitpinhigh(iSDA);
/* ======================================== */
    }
/* Version 2.0.0.1 modification, 2006.05.15 */
    if (readpin(iSCL) == 0) {
        setpinhigh(iSCL);
        waitpinhigh(iSCL);
/* ======================================== */
    }
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iSU_STA);
/* ========================================= */
/* Version 3.0.0.6 modification, 2009.03.12 */
    setpinlow(iSDA);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iHD_STA);
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
    setpinlow(iSCL);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iHD_DAT);
/* ========================================= */
    return;
}

/* I2C stop signal (P): SDA change from low to high while SCL remain at high */
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
static void GPIO_I2C_Stop(int iSCL, int iSDA)
/* ======================================== */
/* ======================================== */
{
/* Version 3.0.0.5 modification, 2008.12.10 */
/* Version 3.1.0.0 modification, 2009.11.20 */
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
    setpinlow(iSCL);
/* ======================================== */
/* Version 3.0.0.6 modification, 2009.03.12 */
    setpinlow(iSDA);
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iSU_DAT);
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
    if (readpin(iSCL) == 0) {
        setpinhigh(iSCL);
        waitpinhigh(iSCL);
/* ======================================== */
    }
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iSU_STO);
/* ========================================= */
/* Version 3.0.0.6 modification, 2009.03.12 */
    setpinhigh(iSDA);
    waitpinhigh(iSDA);
/* ======================================== */

/* Version 3.0.0.0 modification, 2007.11.23 */
/* Version 3.0.0.6 modification, 2009.03.12 */
    setdir(iSDA, DIR_IN);
/* ======================================== */
    setdir(iSCL, DIR_IN);
/* ======================================== */
/* Version 3.0.0.5 modification, 2008.12.10 */
/* ======================================== */
/* Version 3.0.0.10 modification, 2009.04.22 */
    udelay(iBUF);
/* ========================================= */
/* Version 3.0.0.1 modification, 2007.12.31 */
/* ======================================== */
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
static int GPIO_I2C_GetAck(int iSCL, int iSDA)
/* ======================================== */
/* ======================================== */
{
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.17 modification, 2009.09.07 */
    if (read_bit(iSCL, iSDA, FALSE)) {
/* ========================================= */
/* ======================================== */
/* ======================================== */
        return -EINVAL;
    }
    else {
        return 0;
    }
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
static void GPIO_I2C_SendAck(int iSCL, int iSDA, BOOL bSPI)
/* ========================================= */
/* ======================================== */
/* ======================================== */
{
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
    write_bit(0, iSCL, iSDA, bSPI);
/* ========================================= */
/* ======================================== */
/* ======================================== */
    return;
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
static void GPIO_I2C_SendNack(int iSCL, int iSDA, BOOL bSPI)
/* ========================================= */
/* ======================================== */
/* ======================================== */
{
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
    write_bit(1, iSCL, iSDA, bSPI);
/* ========================================= */
/* ======================================== */
/* ======================================== */
    return;
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
static void GPIO_I2C_WriteByte(unsigned char data, int iSCL, int iSDA, BOOL bSPI)
/* ========================================= */
/* ======================================== */
/* ======================================== */
{
    unsigned char mask;
    int i;

    mask = 0x80;
    for (i=0; i<8; i++)
    {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
        write_bit(data&mask, iSCL, iSDA, bSPI);
/* ========================================= */
/* ======================================== */
/* ======================================== */
        mask = mask >> 1;
    }
    return;
}

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.17 modification, 2009.09.07 */
static void GPIO_I2C_ReadByte(unsigned char *data, int iSCL, int iSDA, BOOL bSPI)
/* ========================================= */
/* ======================================== */
/* ======================================== */
{
    unsigned char mask;
    int i;

    *data = 0;
    mask = 0x80;
    for (i=0; i<8; i++)
    {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.17 modification, 2009.09.07 */
        if (read_bit(iSCL, iSDA, bSPI))
/* ========================================= */
/* ======================================== */
/* ======================================== */
        {
            *data |= mask;
        }
        mask = mask >> 1;
    }
    return;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_I2C_Read
 *
 * Input:   filp    fulesystem pointer
 *      buff    data buffer pointer
 *      count   data numbers
 *
 * Value Returned:  0   Success
 *          others  Failure
 *----------------------------------------------------------------------*/
static ssize_t GPIO_I2C_Read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
/* Version 2.0.0.1 modification, 2006.05.15 */
    TGPIOI2CObjInfo *ptObjInfo = (TGPIOI2CObjInfo *)filp->private_data;
    DWORD dwNum = MINOR(ptObjInfo->pinode->i_rdev);
    int iSCL;
/* ======================================== */
/* Version 3.0.0.6 modification, 2009.03.12 */
    int iSDA;
/* ======================================== */
    int i;
/* Version 3.1.0.0 modification, 2009.11.20 */
    int I2CFlags;
/* ======================================== */
/* Version 3.0.0.17 modification, 2009.09.08 */
/* Version 3.0.0.18 modification, 2009.11.14 */
#ifdef GPIOI2C_SO
    int iBoth;
#endif //GPIOI2C_SO
/* Version 3.1.0.1 modification, 2009.12.03 */
    int intr;
/* ======================================== */
/* ========================================= */
    BOOL bConfigIsDone;
/* ========================================= */
/* Version 3.0.0.10 modification, 2009.04.22 */
    bool bFastMode;

/* Version 3.1.0.0 modification, 2009.11.15 */
    I2CFlags = buff[count+1];
    bFastMode = ((I2CFlags&GPIOI2C_FAST) == GPIOI2C_FAST);
/* ======================================== */
    iHD_STA  = (bFastMode==1) ? 0 : T_HD_STA;
    iLOW  = (bFastMode==1) ? 0 : T_LOW;
    iHIGH = (bFastMode==1) ? 0 : T_HIGH;
    iSU_STA = (bFastMode==1) ? 0 : T_SU_STA;
    iHD_DAT = (bFastMode==1) ? 0 : T_HD_DAT;
    iSU_DAT = (bFastMode==1) ? 0 : T_SU_DAT;
    iSU_STO = (bFastMode==1) ? 0 : T_SU_STO;
    iBUF = (bFastMode==1) ? 0 : T_BUF;
/* ========================================= */

/* Version 3.0.0.17 modification, 2009.09.08 */
    DBG("Read I2C channel %ld\n", dwNum);
    switch (dwNum)
    {
        case 0:
/* Version 3.1.0.2 modification, 2009.12.09 */
            if (iSCL_bus0 != 0)
            {
                iSCL = iSCL_bus0;
            }
            else
            {
                iSCL = GPIOI2C_SCL_0;
            }
/* ======================================== */
        break;
#ifdef  GPIOI2C_SCL_1
        case 1:
            if (iSCL_bus1)
                iSCL = iSCL_bus1;
            else
                iSCL = GPIOI2C_SCL_1;
        break;
#endif // GPIOI2C_SCL_1
#ifdef  GPIOI2C_SCL_2
        case 2:
            iSCL = GPIOI2C_SCL_2;
        break;
#endif // GPIOI2C_SCL_2
#ifdef  GPIOI2C_SCL_3
        case 3:
            iSCL = GPIOI2C_SCL_3;
        break;
#endif // GPIOI2C_SCL_3
        default:
            DBG("There is no I2C channel %ld\n", dwNum);
            return -EINVAL;
        break;
    }
    switch (dwNum)
    {
#ifdef  GPIOI2C_SDA_0
        case 0:
            iSDA = GPIOI2C_SDA_0;
        break;
#endif // GPIOI2C_SDA_0
#ifdef  GPIOI2C_SDA_1
        case 1:
/* Version 3.0.0.8 modification, 2009.04.01 */
            if (iSDA_bus1)
                iSDA = iSDA_bus1;
            else
/* ======================================== */
                iSDA = GPIOI2C_SDA_1;
        break;
#endif // GPIOI2C_SDA_1
#ifdef  GPIOI2C_SDA_2
        case 2:
            iSDA = GPIOI2C_SDA_2;
        break;
#endif // GPIOI2C_SDA_2
        default:
#ifdef  GPIOI2C_SDA
/* Version 3.1.0.2 modification, 2009.12.09 */
            if (iSDA_bus != 0)
            {
                iSDA = iSDA_bus;
            }
            else
            {
                iSDA = GPIOI2C_SDA;
            }
/* ======================================== */
            DBG("(I2C channel, iSDA) : (%ld, %ld)\n", dwNum, iSDA);
#else // GPIOI2C_SDA
            DBG("There is no I2C channel %ld\n", dwNum);
            return -EINVAL;
#endif // GPIOI2C_SDA
    }
    g_GPIO_I2C_devices->iSCL = iSCL;
    g_GPIO_I2C_devices->iSDA = iSDA;
    g_GPIO_I2C_devices->bFastMode = bFastMode;
    g_GPIO_I2C_devices->dwCount = count;
/* Version 3.0.0.18 modification, 2009.11.14 */
    g_GPIO_I2C_devices->bIsWrite = FALSE;
/* Version 3.1.0.0 modification, 2009.11.20 */
/* ======================================== */
/* ========================================= */
    if (count > iBuffLen)
    {
        printk("The buffer size (%d) is not enough !!\n", iBuffLen);
        return -EINVAL;
    }
    else
    {
        for (i=0; i<count; i++)
        {
            *(g_GPIO_I2C_devices->pbyBuff+i) = buff[i];
        }
    }
    bConfigIsDone = FALSE;
    if (bFastMode)
    {
/* Version 3.1.0.0 modification, 2009.11.20 */
#ifndef GPIOI2C_SO
        printk("GPIOI2C_SO is not defined !!\n");
        return -EINVAL;
#endif //GPIOI2C_SO
/* ======================================== */
        count = count - 1;
        if (buff[count+1] == 1)
        {
             // If VSYNC is high, wait falling edge.
/* Version 3.1.0.6 modification, 2011.12.01 */
            //if (readl(GPIO_I2C_Reg + GPIOC_MMR_DATAIN) & GPIOI2C_INTR)
            {
                if (g_GPIO_I2C_devices->dwChipVersion == 0)
/* ======================================== */
                {
                    intr = readl(GPIO_I2C_Reg + GPIOC_MMR_INTREN);
                    writel(intr|(GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_INTREN); //enable interrupt
/* Version 3.1.0.6 modification, 2011.12.01 */
                }
                else
                {
                    enable_irq(VPL_VIC0_VSYNC_IRQ_NUM);
                }
/* ======================================== */
                if (wait_event_interruptible(g_GPIO_I2C_devices->tWaitQueueHead,
/* Version 3.1.0.6 modification, 2011.12.01 */
                    (g_GPIO_I2C_devices->GPIO_intr_occurred != 0)) != 0)
/* ======================================== */
                {
                    DBG("Exit WaitComplete function by signal !!\n");
                    return -EINVAL;
                }

                g_GPIO_I2C_devices->GPIO_intr_occurred = 0;
/* Version 3.0.0.16 modification, 2009.08.18 */
/* Version 3.1.0.1 modification, 2009.12.03 */
                bConfigIsDone = TRUE;
/* ======================================== */
/* ========================================= */
            }
         }
    }
/* ========================================= */
    // get semaphore so that another application can not read/write
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.17 modification, 2009.09.08 */
    if (bConfigIsDone == FALSE)
    {
        down_interruptible(&g_GPIO_I2C_devices->sema);
    }
/* ========================================= */
/* ======================================== */

/* Version 3.0.0.17 modification, 2009.09.08 */
    if (bFastMode == TRUE)
    {
/* Version 3.0.0.18 modification, 2009.11.14 */
#ifdef GPIOI2C_SO
/* ========================================= */
        if (bConfigIsDone == FALSE)
        {
#ifdef GPIOI2C_SEN
            setpinlow(GPIOI2C_SEN);
#endif //GPIOI2C_SEN
            iBoth = iSCL | iSDA;
            setdir(iBoth, DIR_OUT);
            setdir(GPIOI2C_SO, DIR_IN);
            GPIO_I2C_WriteByte(buff[0], iSCL, iSDA, TRUE);
            GPIO_I2C_WriteByte(buff[1], iSCL, iSDA, TRUE);
            for (i=2; i<(count+1); i++)
            {
                GPIO_I2C_ReadByte(&buff[i], iSCL, GPIOI2C_SO, TRUE);
            }
            setpin(iBoth, 1);
            setdir(iBoth, DIR_IN);
#ifdef GPIOI2C_SEN
            setpinhigh(GPIOI2C_SEN);
/* Version 3.1.0.0 modification, 2009.11.21 */
            setdir(GPIOI2C_SEN, DIR_IN);
/* ======================================== */
#endif //GPIOI2C_SEN
        }
/* Version 3.0.0.18 modification, 2009.11.14 */
        else
        {
            for (i=2; i<(count+1); i++)
            {
                buff[i] = *(g_GPIO_I2C_devices->pbyBuff+i);
            }
        }
#endif //GPIOI2C_SO
/* ========================================= */
    }
    else
    {
/* ========================================= */
        GPIO_I2C_Start(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
        DBG("Start signal, r\n");

        // write device address
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
        GPIO_I2C_WriteByte(buff[0], iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
        DBG("Data signal, r\n");
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.1.0.0 modification, 2009.11.15 */
        if ((GPIO_I2C_GetAck(iSCL, iSDA)<0) && ((I2CFlags&GPIOI2C_IGNORE_ACK)==0))
/* ======================================== */
/* ======================================== */
/* ======================================== */
        {
/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.6 modification, 2009.03.12 */
            GPIO_I2C_Stop(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
            up(&g_GPIO_I2C_devices->sema);
/* ======================================== */
            DBG("Read ACK failed, r\n");
            return -EINVAL;
        }
        DBG("ACK signal, r\n");

        for (i=0; i<(count-1); i++)
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.17 modification, 2009.09.07 */
            GPIO_I2C_ReadByte(&buff[i], iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
            DBG("Data signal, r\n");

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
            GPIO_I2C_SendAck(iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
            DBG("ACK signal, r\n");
        }

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.17 modification, 2009.09.07 */
        GPIO_I2C_ReadByte(&buff[i], iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
        DBG("Data signal, r\n");

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
        GPIO_I2C_SendNack(iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
        DBG("No ACK signal, r\n");

/* Version 2.0.0.0 modification, 2004.12.29 */
/* Version 3.0.0.5 modification, 2008.12.04 */
        if ((buff[count+1]&0x01) == GPIOI2C_STOP)
/* ======================================== */
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
            GPIO_I2C_Stop(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
            DBG("Stop signal, r\n");/**/
        }
/* ======================================== */
/* Version 3.0.0.17 modification, 2009.09.08 */
    }
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
    if (bConfigIsDone == FALSE)
    {
        up(&g_GPIO_I2C_devices->sema);
    }
/* ======================================== */
    return count;
}

/*-----------------------------------------------------------------------
 * Function Name: GPIO_I2C_Write
 *
 * Input:   filp    fulesystem pointer
 *      buff    data buffer pointer
 *      count   data numbers
 *
 * Value Returned:  0   Success
 *          others  Failure
 *----------------------------------------------------------------------*/
static ssize_t GPIO_I2C_Write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
/* Version 2.0.0.1 modification, 2006.05.15 */
    TGPIOI2CObjInfo *ptObjInfo = (TGPIOI2CObjInfo *)filp->private_data;
    DWORD dwNum = MINOR(ptObjInfo->pinode->i_rdev);
    int iSCL;
/* ======================================== */
/* Version 3.0.0.6 modification, 2009.03.12 */
    int iSDA;
/* ======================================== */

/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.1.0.1 modification, 2009.12.03 */
    int i;
    int intr;
/* ======================================== */
    int I2CFlags;
/* ========================================= */
/* Version 3.0.0.15 modification, 2009.08.14 */
/* Version 3.0.0.16 modification, 2009.08.18 */
    int iBoth;
    BOOL bConfigIsDone;
/* ========================================= */
/* ========================================= */
/* Version 3.0.0.10 modification, 2009.04.22 */
    bool bFastMode;
/* ========================================= */

    if (count <= 0)
    {
/* Version 2.0.0.1 modification, 2006.05.15 */
        up(&g_GPIO_I2C_devices->sema);
/* ======================================== */
        return -EINVAL;
    }
/* Version 3.0.0.10 modification, 2009.04.22 */
/* Version 3.0.0.11 modification, 2009.05.05 */
    I2CFlags = buff[count+1];
/* Version 3.1.0.0 modification, 2009.11.15 */
    bFastMode = ((I2CFlags&GPIOI2C_FAST) == GPIOI2C_FAST);
/* ======================================== */
    iHD_STA  = (bFastMode==1) ? 0 : T_HD_STA;
    iLOW  = (bFastMode==1) ? 0 : T_LOW;
    iHIGH = (bFastMode==1) ? 0 : T_HIGH;
    iSU_STA = (bFastMode==1) ? 0 : T_SU_STA;
    iHD_DAT = (bFastMode==1) ? 0 : T_HD_DAT;
    iSU_DAT = (bFastMode==1) ? 0 : T_SU_DAT;
    iSU_STO = (bFastMode==1) ? 0 : T_SU_STO;
    iBUF = (bFastMode==1) ? 0 : T_BUF;
/* ========================================= */

/* Version 3.0.0.16 modification, 2009.08.18 */
    switch (dwNum)
    {
        case 0:
/* Version 3.1.0.2 modification, 2009.12.09 */
            if (iSCL_bus0 != 0)
            {
                iSCL = iSCL_bus0;
            }
            else
            {
                iSCL = GPIOI2C_SCL_0;
            }
/* ======================================== */
        break;
#ifdef  GPIOI2C_SCL_1
        case 1:
            if (iSCL_bus1)
                iSCL = iSCL_bus1;
            else
                iSCL = GPIOI2C_SCL_1;
        break;
#endif // GPIOI2C_SCL_1
#ifdef  GPIOI2C_SCL_2
        case 2:
            iSCL = GPIOI2C_SCL_2;
        break;
#endif // GPIOI2C_SCL_2
#ifdef  GPIOI2C_SCL_3
        case 3:
            iSCL = GPIOI2C_SCL_3;
        break;
#endif // GPIOI2C_SCL_3
        default:
            DBG("There is no I2C channel %ld\n", dwNum);
            return -EINVAL;
        break;
    }

    switch (dwNum)
    {
#ifdef  GPIOI2C_SDA_0
        case 0:
            iSDA = GPIOI2C_SDA_0;
        break;
#endif // GPIOI2C_SDA_0
#ifdef  GPIOI2C_SDA_1
        case 1:
            if (iSDA_bus1)
                iSDA = iSDA_bus1;
            else
                iSDA = GPIOI2C_SDA_1;
        break;
#endif // GPIOI2C_SDA_1
#ifdef  GPIOI2C_SDA_2
        case 2:
            iSDA = GPIOI2C_SDA_2;
        break;
#endif // GPIOI2C_SDA_2
        default:
#ifdef  GPIOI2C_SDA
/* Version 3.1.0.2 modification, 2009.12.09 */
            if (iSDA_bus != 0)
            {
                iSDA = iSDA_bus;
            }
            else
            {
                iSDA = GPIOI2C_SDA;
            }
/* ======================================== */
            DBG("(I2C channel, iSDA) : (%ld, %ld)\n", dwNum, iSDA);
#else // GPIOI2C_SDA
            DBG("There is no I2C channel %ld\n", dwNum);
            return -EINVAL;
#endif // GPIOI2C_SDA
    }
    g_GPIO_I2C_devices->iSCL = iSCL;
    g_GPIO_I2C_devices->iSDA = iSDA;
    g_GPIO_I2C_devices->bFastMode = bFastMode;
    g_GPIO_I2C_devices->dwCount = count;
/* Version 3.0.0.18 modification, 2009.11.14 */
    g_GPIO_I2C_devices->bIsWrite = TRUE;
/* ========================================= */
    if (count > iBuffLen)
    {
        printk("The buffer size (%d) is not enough !!\n", iBuffLen);
        return -EINVAL;
    }
    else
    {
        for (i=0; i<count; i++)
        {
            *(g_GPIO_I2C_devices->pbyBuff+i) = buff[i];
        }
    }
    bConfigIsDone = FALSE;
/* ========================================= */
    if (bFastMode)
    {
        count = count - 1;
        if (buff[count+1] == 1)
        {
            // If VSYNC is high, wait falling edge.
             /* Version 3.1.0.3 modification, 2009.04.30 */
            //if (readl(GPIO_I2C_Reg + GPIOC_MMR_DATAIN) & GPIOI2C_INTR)
            {
/* Version 3.1.0.6 modification, 2011.12.01 */
                if (g_GPIO_I2C_devices->dwChipVersion == 0)
                {
/* ======================================== */
                intr = readl(GPIO_I2C_Reg + GPIOC_MMR_INTREN);
                writel(intr|(GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_INTREN); //enable interrupt
/* Version 3.1.0.6 modification, 2011.12.01 */
                }
                else
                {
                    enable_irq(VPL_VIC0_VSYNC_IRQ_NUM);
                }
/* ======================================== */
                if (wait_event_interruptible(g_GPIO_I2C_devices->tWaitQueueHead,
/* Version 3.1.0.6 modification, 2011.12.01 */
                    (g_GPIO_I2C_devices->GPIO_intr_occurred != 0)) != 0)
/* ======================================== */
                {
                    DBG("Exit WaitComplete function by signal !!\n");
                    return -EINVAL;
                }

                g_GPIO_I2C_devices->GPIO_intr_occurred = 0;
/* Version 3.0.0.16 modification, 2009.08.18 */
/* Version 3.1.0.1 modification, 2009.12.03 */
                bConfigIsDone = TRUE;
/* ======================================== */
/* ========================================= */
            }
         }
    }
/* ========================================= */

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.16 modification, 2009.08.18 */
    if (bConfigIsDone == FALSE)
    {
        down_interruptible(&g_GPIO_I2C_devices->sema);
    }
/* ========================================= */
/* ======================================== */

/* Version 3.0.0.7 modification, 2009.03.19 */
#ifdef __ASM_ARCH_PLATFORM_HAYDN_H__
    if (dwNum == 2)
    {
#ifdef  GPIOI2C_SCL_2
        setpinhigh(GPIOI2C_SCL_2);
#endif // GPIOI2C_SCL_2
#ifdef  GPIOI2C_SDA_2
        setpinhigh(GPIOI2C_SDA_2);
#endif // GPIOI2C_SDA_2
/* Version 3.0.0.10 modification, 2009.04.22 */
        udelay(iHD_DAT);
/* ========================================= */
    }
#endif //__ASM_ARCH_PLATFORM_HAYDN_H__
/* ======================================== */

/* Version 3.0.0.0 modification, 2007.11.23 */
/* Version 3.0.0.1 modification, 2007.12.31 */
/* Version 3.0.0.16 modification, 2009.08.18 */
/* ========================================= */
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
    DBG("Write I2C channel %ld\n", dwNum);

/* Version 3.0.0.16 modification, 2009.08.18 */
/* ========================================= */
/* Version 3.0.0.15 modification, 2009.08.14 */
    if (bFastMode == TRUE)
    {
        if (bConfigIsDone == FALSE)
        {
#ifdef GPIOI2C_SEN
            setpinlow(GPIOI2C_SEN);
#endif //GPIOI2C_SEN
            iBoth = iSCL | iSDA;
            setdir(iBoth, DIR_OUT);
            for (i=0; i<(count+1); i++)
            {
                GPIO_I2C_WriteByte(buff[i], iSCL, iSDA, TRUE);
            }
            setpin(iBoth, 1);
            setdir(iBoth, DIR_IN);
#ifdef GPIOI2C_SEN
            setpinhigh(GPIOI2C_SEN);
/* Version 3.1.0.0 modification, 2009.11.21 */
            setdir(GPIOI2C_SEN, DIR_IN);
/* ======================================== */
#endif //GPIOI2C_SEN
        }
    }
    else
    {
/* Version 3.1.0.0 modification, 2009.11.20 */
/* ======================================== */
/* ========================================= */
        GPIO_I2C_Start(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
        DBG("Start signal, w\n");

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
        GPIO_I2C_WriteByte(buff[0], iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
        DBG("Data signal, w\n" );

        /* ACK */
/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.1.0.0 modification, 2009.11.15 */
        if ((I2CFlags&GPIOI2C_NO_ACK)!=GPIOI2C_NO_ACK)
/* ======================================== */
/* ========================================= */
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.1.0.0 modification, 2009.11.15 */
            if ((GPIO_I2C_GetAck(iSCL, iSDA)<0) && ((I2CFlags&GPIOI2C_IGNORE_ACK)==0))
/* ======================================== */
/* ======================================== */
/* ======================================== */
            {
/* Version 3.0.0.0 modification, 2007.09.12 */
/* Version 3.0.0.6 modification, 2009.03.12 */
                GPIO_I2C_Stop(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
                up(&g_GPIO_I2C_devices->sema);
/* ======================================== */
                printk("get ack fail, w:0x%08x\n", buff[0]);
                return -EINVAL;
            }
            DBG("ACK signal, w\n");
        }
/* ======================================== */

        for (i=1; i<count; i++)
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
            GPIO_I2C_WriteByte(buff[i], iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
            DBG("Data signal, w\n");
/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.1.0.0 modification, 2009.11.15 */
            if ((I2CFlags&GPIOI2C_NO_ACK)!=GPIOI2C_NO_ACK)
/* ======================================== */
/* ========================================= */
            {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.1.0.0 modification, 2009.11.15 */
                if ((GPIO_I2C_GetAck(iSCL, iSDA)<0) && ((I2CFlags&GPIOI2C_IGNORE_ACK)==0))
/* ======================================== */
/* ======================================== */
                {
/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.6 modification, 2009.03.12 */
                    GPIO_I2C_Stop(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
                    up(&g_GPIO_I2C_devices->sema);
/* ======================================== */
                    printk("get ack fail, w\n");
                    return -EINVAL;
                }
                DBG("ACK signal, w\n");
            }
/* ======================================== */
        }

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.0.0.15 modification, 2009.08.14 */
        GPIO_I2C_WriteByte(buff[i], iSCL, iSDA, FALSE);
/* ========================================= */
/* ======================================== */
/* ======================================== */
        DBG("Data signal, w\n");

/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.11 modification, 2009.05.05 */
        if (((I2CFlags>>1)&0x01)!=GPIOI2C_NO_ACK)
/* ========================================= */
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
/* Version 3.1.0.0 modification, 2009.11.15 */
            if ((GPIO_I2C_GetAck(iSCL, iSDA)<0) && ((I2CFlags&GPIOI2C_IGNORE_ACK)==0))
/* ======================================== */
/* ======================================== */
/* ======================================== */
            {
/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.6 modification, 2009.03.12 */
                GPIO_I2C_Stop(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
/* Version 2.0.0.1 modification, 2006.05.15 */
                up(&g_GPIO_I2C_devices->sema);
/* ======================================== */
                return -EINVAL;
            }
            DBG("ACK signal, w\n");
        }
/* ======================================== */

/* Version 2.0.0.0 modification, 2004.12.29 */
/* Version 3.0.0.5 modification, 2008.12.04 */
/* Version 3.0.0.11 modification, 2009.05.05 */
        if ((I2CFlags&0x01) == GPIOI2C_STOP)
/* ========================================= */
/* ======================================== */
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.6 modification, 2009.03.12 */
            GPIO_I2C_Stop(iSCL, iSDA);
/* ======================================== */
/* ======================================== */
            DBG("Stop signal, w\n");
        }
/* ======================================== */
/* Version 3.0.0.0 modification, 2007.11.23 */
/* Version 3.0.0.1 modification, 2007.12.31 */
/* Version 3.0.0.5 modification, 2008.12.10 */
/* ======================================== */
/* ======================================== */
/* ======================================== */
/* Version 3.0.0.15 modification, 2009.08.14 */
    }
/* ========================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.0.0.16 modification, 2009.08.18 */
    if (bConfigIsDone == FALSE)
    {
        up(&g_GPIO_I2C_devices->sema);
    }
/* ========================================= */
/* ======================================== */
    return count;
}


/* Version 3.0.0.11 modification, 2009.05.05 */
 static irqreturn_t GPIO_I2C_ISR(int irq, void *dev_id)
{
    volatile unsigned int status;
/* Version 3.0.0.16 modification, 2009.08.18 */
/* Version 3.1.0.1 modification, 2009.12.03 */
/* ======================================== */
    int i, iSCL, iSDA, iBoth;
    BOOL bFastMode;
    DWORD dwCount;
    BYTE *pbyBuff;
/* Version 3.1.0.1 modification, 2009.12.03 */
/* ======================================== */
 /* ========================================= */

/* Version 3.1.0.1 modification, 2009.12.03 */
/* Version 3.1.0.6 modification, 2011.12.01 */
    if (g_GPIO_I2C_devices->dwChipVersion == 0) //Mozart V1
    {
/* ======================================== */
        // only check GPIOI2C_INTR bit.
        status = readl(GPIO_I2C_Reg + GPIOC_MMR_MASKSTATE) & GPIOI2C_INTR;
/* ======================================== */
/* Version 3.1.0.6 modification, 2011.12.01 */
        if (status)
        {
/* ======================================== */
            writel(status, GPIO_I2C_Reg + GPIOC_MMR_INTRCLR);
/* Version 3.1.0.6 modification, 2011.12.01 */
        }
        else
        {
            return IRQ_HANDLED;
        }
    }
/* ======================================== */
    g_GPIO_I2C_devices->GPIO_intr_occurred = 1;
/* Version 3.0.0.16 modification, 2009.08.18 */
 /* ========================================= */
/* Version 3.1.0.6 modification, 2011.12.01 */
/* ======================================== */
/* Version 3.1.0.1 modification, 2009.12.03 */
    bFastMode = g_GPIO_I2C_devices->bFastMode;
    if (bFastMode)
    {
        iSCL = g_GPIO_I2C_devices->iSCL;
        iSDA = g_GPIO_I2C_devices->iSDA;
        pbyBuff = g_GPIO_I2C_devices->pbyBuff;
        dwCount = g_GPIO_I2C_devices->dwCount;
#ifdef GPIOI2C_SEN
        setpinlow(GPIOI2C_SEN);
#endif //GPIOI2C_SEN
        iBoth = iSCL | iSDA;
        setdir(iBoth, DIR_OUT);
        if (g_GPIO_I2C_devices->bIsWrite == TRUE)
        {
            for (i=0; i<dwCount; i++)
            {
                GPIO_I2C_WriteByte(*(pbyBuff+i), iSCL, iSDA, TRUE);
            }
        }
#ifdef GPIOI2C_SO
        else
        {
            setdir(GPIOI2C_SO, DIR_IN);
            GPIO_I2C_WriteByte(*(pbyBuff), iSCL, iSDA, TRUE);
            GPIO_I2C_WriteByte(*(pbyBuff+1), iSCL, iSDA, TRUE);
            for (i=2; i<(dwCount+1); i++)
            {
                GPIO_I2C_ReadByte(pbyBuff+i, iSCL, GPIOI2C_SO, TRUE);
            }
        }
#endif //GPIOI2C_SO
        setpin(iBoth, 1);
        setdir(iBoth, DIR_IN);
#ifdef GPIOI2C_SEN
        setpinhigh(GPIOI2C_SEN);
        setdir(GPIOI2C_SEN, DIR_IN);
#endif //GPIOI2C_SEN
    }
/* Version 3.1.0.6 modification, 2011.12.01 */
    if (g_GPIO_I2C_devices->dwChipVersion == 0) //Mozart V1
    {
/* ======================================== */
        writel((readl(GPIO_I2C_Reg + GPIOC_MMR_INTREN))&(~status), GPIO_I2C_Reg + GPIOC_MMR_INTREN); //disable interrupt
/* Version 3.1.0.6 modification, 2011.12.01 */
    }
    else
    {
        disable_irq(VPL_VIC0_VSYNC_IRQ_NUM);
    }
/* ======================================== */
/* ======================================== */
    wake_up_interruptible(&g_GPIO_I2C_devices->tWaitQueueHead);
/* Version 3.1.0.6 modification, 2011.12.01 */
/* ======================================== */
 /* Version 3.1.0.1 modification, 2009.12.03 */
/* ======================================== */

    return IRQ_HANDLED;
}
 /* ========================================= */



static int GPIO_I2C_Open(struct inode * inode, struct file * filp)
{
/* Version 2.0.0.1 modification, 2006.05.15 */
    TGPIOI2CObjInfo *ptObjInfo;

    if (filp->private_data == NULL)
    {
        if ((filp->private_data=(TGPIOI2CObjInfo *)kmalloc(sizeof(TGPIOI2CObjInfo), GFP_KERNEL)) == NULL)
        {
            DBG("Insufficient kernel memory space !!\n");
            return -EFAULT;
        }
        ptObjInfo = (TGPIOI2CObjInfo *)filp->private_data;
        ptObjInfo->bLock = FALSE;
        ptObjInfo->pinode = inode;
    }
    else
    {
        return -EBUSY;
    }
/* ======================================== */

    DBG("GPIO_I2C module open success\n");
/* Version 3.0.0.0 modification, 2007.09.05 */
    if (try_module_get(THIS_MODULE) == 0)
    {
        return -EBUSY;
    }
/* ======================================== */
    return 0;
}

static int GPIO_I2C_Release(struct inode *inode, struct file *filp)
{
/* Version 2.0.0.1 modification, 2006.05.15 */
    TGPIOI2CObjInfo *ptObjInfo = (TGPIOI2CObjInfo *)filp->private_data;

    if (ptObjInfo != NULL)
    {
        if (ptObjInfo->bLock == TRUE)
        {
/* Version 2.0.0.1 modification, 2006.05.15 */
            up(&g_GPIO_I2C_devices->sema);
/* ======================================== */
            ptObjInfo->bLock = FALSE;
        }
/* Version 3.0.0.2 modification, 2008.01.08 */
        kfree(filp->private_data);
        filp->private_data = NULL;
/* ======================================== */
    }
/* ======================================== */

/* Version 3.0.0.0 modification, 2007.09.05 */
    module_put(THIS_MODULE);
/* ======================================== */
    DBG("GPIO_I2C module close success\n");
    return 0;
}

/* Device File Operations */
/* Note that no ioctl is defined */
static struct file_operations GPIOI2C_fops = {
    open:       GPIO_I2C_Open,
    release:    GPIO_I2C_Release,
    read:       GPIO_I2C_Read,
    write:      GPIO_I2C_Write,
};

struct file_operations *GPIOI2C_get_fops(void)
{
        return &GPIOI2C_fops;
}

EXPORT_SYMBOL(GPIOI2C_get_fops);

#ifdef MODULE
void cleanup_module(void)
#else
void GPIO_I2_cleanup(void)
#endif
{
/* Version 3.1.0.0 modification, 2009.11.20 */
/* Version 3.1.0.5 modification, 2011.11.10 */
/* ======================================== */
/* ======================================== */
/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.1.0.0 modification, 2009.11.22 */
/* Version 3.1.0.5 modification, 2011.11.10 */
/* Version 3.1.0.6 modification, 2011.12.01 */
    if (g_GPIO_I2C_devices->dwChipVersion == 0) //Mozart V1
/* ======================================== */
    {
        free_irq(VPL_GPIOC_IRQ_NUM, g_GPIO_I2C_devices);
    }
    else
    {
        free_irq(VPL_VIC0_VSYNC_IRQ_NUM, g_GPIO_I2C_devices);
    }
/* ======================================== */
/* ======================================== */
/* ========================================= */

    if (g_GPIO_I2C_devices)
    {
/* Version 3.0.0.16 modification, 2009.08.19 */
/* Version 3.1.0.1 modification, 2009.12.03 */
        if (g_GPIO_I2C_devices->pbyBuff)
        {
            kfree(g_GPIO_I2C_devices->pbyBuff);
        }
/* ======================================== */
/* ========================================= */
        kfree(g_GPIO_I2C_devices);
    }
    if (g_GPIO_I2C_major!=0)
    {
/* Version 3.1.0.0 modification, 2009.11.20 */
/* Version 3.1.0.5 modification, 2011.11.10 */
    unregister_chrdev(g_GPIO_I2C_major, "gpio_i2c");
/* ======================================== */
/* ======================================== */
    }
/* Version 3.1.0.5 modification, 2011.11.10 */
    if (SYSC_base)
    {
        iounmap(SYSC_base);
    }
/* ======================================== */
/* ======================================== */
    if (GPIO_I2C_base)
    {
        iounmap(GPIO_I2C_base);
    }
    return;
}

#ifdef MODULE
int init_module(void)
#else
int GPIO_I2C_init(void)
#endif
{
    int ret = 0;

/* Version 2.0.0.1 modification, 2006.05.15 */
    printk("GPIOI2C device driver version %s\n", GPIOI2C_ID_VERSION);
/* ======================================== */

    /* Use ioremap to get a handle on register */
    if ((GPIO_I2C_base = (unsigned long *)ioremap((int)VPL_GPIOC_MMR_BASE,
                    (int)18)) == NULL)
    {
        printk("IOREMAP fail.\n");
        goto fail_init;
    }
/* Version 3.1.0.5 modification, 2011.11.10 */
    if ((SYSC_base = (unsigned long *)ioremap((int)MOZART_SYSC_MMR_BASE, (int)0x100)) == NULL)
    {
        printk("IOREMAP fail.\n");
        goto fail_init;
    }
/* ======================================== */
    GPIO_I2C_Reg = (void *)((unsigned long)GPIO_I2C_base);

    ret = register_chrdev(g_GPIO_I2C_major, "gpio_i2c", &GPIOI2C_fops);
    if (ret < 0)
    {
        DBG(" can't get major %d\n", g_GPIO_I2C_major);
        goto fail_init;
    }

    if (g_GPIO_I2C_major == 0)
    {
        g_GPIO_I2C_major = ret; /* dynamic */
    }
    DBG("GPIOI2C Major Number = %d\n", g_GPIO_I2C_major);
    DBG("make node with 'mknod /dev/gpio_i2c c %d 0'\n", g_GPIO_I2C_major);

    /*******************************************************
    * Allocate space for each devices.
    *******************************************************/
    g_GPIO_I2C_devices = kmalloc(sizeof(GPIO_I2C_Dev), GFP_KERNEL);
    if (!g_GPIO_I2C_devices)
    {
        ret = -ENOMEM;
        goto fail_init;
    }
    memset(g_GPIO_I2C_devices, 0, sizeof(GPIO_I2C_Dev));
    sema_init(&g_GPIO_I2C_devices->sema, 1);

/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.0.0.13 modification, 2009.05.12 */
/* Version 3.0.0.14 modification, 2009.05.12 */
/* Version 3.0.0.18 modification, 2009.11.14 */
/* Version 3.1.0.0 modification, 2009.11.20 */
/* Version 3.1.0.5 modification, 2011.11.10 */
    if (readl(SYSC_base + 0x14) == 0x1000000) //Mozart V1
    {
        fLib_SetIntTrig(VPL_GPIOC_IRQ_NUM, EDGE, H_ACTIVE);
        ret = request_irq(VPL_GPIOC_IRQ_NUM, &GPIO_I2C_ISR, IRQF_SHARED, "GPIO controller", g_GPIO_I2C_devices);
/* Version 3.1.0.6 modification, 2011.12.01 */
        g_GPIO_I2C_devices->dwChipVersion = 0;
/* ======================================== */
    }
    else  // Mozart V2
    {
        fLib_SetIntTrig(VPL_VIC0_VSYNC_IRQ_NUM, EDGE, H_ACTIVE);
        ret = request_irq(VPL_VIC0_VSYNC_IRQ_NUM, &GPIO_I2C_ISR, IRQF_SHARED, "GPIO controller", g_GPIO_I2C_devices);
/* Version 3.1.0.6 modification, 2011.12.01 */
        disable_irq(VPL_VIC0_VSYNC_IRQ_NUM);
        g_GPIO_I2C_devices->dwChipVersion = 1;
/* ======================================== */
    }
/* ======================================== */
/* ======================================== */
/* ========================================= */
/* ========================================= */
/* ========================================= */
    if (ret)
    {
        DBG("Unable to allocate I2C IRQ=0x%X\n", VPL_GPIOC_IRQ_NUM);
        return ret;
    }
    init_waitqueue_head(&g_GPIO_I2C_devices->tWaitQueueHead);
    g_GPIO_I2C_devices->GPIO_intr_occurred = 0;
/* ========================================= */

/* Version 2.0.0.1 modification, 2006.05.15 */
/* Version 3.1.0.2 modification, 2009.12.09 */
    if (iSCL_bus0 != 0)
    {
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(iSCL_bus0);
/* ======================================== */
        setdir(iSCL_bus0, DIR_IN);
    }
    else
    {
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(GPIOI2C_SCL_0);
/* ======================================== */
        setdir(GPIOI2C_SCL_0, DIR_IN);
    }
/* ======================================== */

/* Version 3.0.0.5 modification, 2008.12.10 */
#ifdef GPIOI2C_SCL_1
/* Version 3.0.0.8 modification, 2009.04.01 */
    if (iSCL_bus1) {
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(iSCL_bus1);
/* ======================================== */
        setdir(iSCL_bus1, DIR_IN);
    }
    else
    {
/* ======================================== */
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(GPIOI2C_SCL_1);
/* ======================================== */
        setdir(GPIOI2C_SCL_1, DIR_IN);
    }
#endif //GPIOI2C_SCL_1
#ifdef GPIOI2C_SCL_2
/* Version 3.1.0.7 modification, 2012.09.25 */
    register_gpio(GPIOI2C_SCL_2);
/* ======================================== */
    setdir(GPIOI2C_SCL_2, DIR_IN);
#endif //GPIOI2C_SCL_2
#ifdef GPIOI2C_SCL_3
/* Version 3.1.0.7 modification, 2012.09.25 */
    register_gpio(GPIOI2C_SCL_3);
/* ======================================== */
    setdir(GPIOI2C_SCL_3, DIR_IN);
#endif //GPIOI2C_SCL_3
#ifdef GPIOI2C_SEN
/* Version 3.1.0.7 modification, 2012.09.25 */
    register_gpio(GPIOI2C_SEN);
/* ======================================== */
/* Version 3.1.0.0 modification, 2009.11.21 */
    setdir(GPIOI2C_SEN, DIR_IN);
/* ======================================== */
#endif //GPIOI2C_SEN
/* ======================================== */
/* ======================================== */
/* Version 3.0.0.6 modification, 2009.03.12 */
#ifdef GPIOI2C_SDA
/* ======================================== */
/* Version 3.1.0.2 modification, 2009.12.09 */
    if (iSDA_bus != 0)
    {
/* Version 3.1.0.7 modification, 2012.09.25 */      
	register_gpio(iSDA_bus);
/* ======================================== */
        setdir(iSDA_bus, DIR_IN);
    }
    else
    {
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(GPIOI2C_SDA);
/* ======================================== */
        setdir(GPIOI2C_SDA, DIR_IN);
    }
/* ======================================== */
/* Version 3.0.0.6 modification, 2009.03.12 */
#endif // GPIOI2C_SDA
#ifdef GPIOI2C_SDA_0
/* Version 3.1.0.7 modification, 2012.09.25 */
    register_gpio(GPIOI2C_SDA_0);
/* ======================================== */
    setdir(GPIOI2C_SDA_0, DIR_IN);
#endif // GPIOI2C_SDA_0
#ifdef GPIOI2C_SDA_1
/* Version 3.0.0.8 modification, 2009.04.01 */
    if (iSDA_bus1)
    {
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(iSDA_bus1);
/* ======================================== */
        setdir(iSDA_bus1, DIR_IN);
    }
    else
    {
/* Version 3.1.0.7 modification, 2012.09.25 */
        register_gpio(GPIOI2C_SDA_1);
/* ======================================== */
/* ======================================== */
        setdir(GPIOI2C_SDA_1, DIR_IN);
    }
#endif // GPIOI2C_SDA_1
/* ======================================== */
/* Version 3.0.0.7 modification, 2009.03.19 */
#ifdef GPIOI2C_SDA_2
/* Version 3.1.0.7 modification, 2012.09.25 */
    register_gpio(GPIOI2C_SDA_2);
/* ======================================== */
    setdir(GPIOI2C_SDA_2, DIR_IN);
#endif // GPIOI2C_SDA_2
/* ======================================== */

/* Version 3.0.0.11 modification, 2009.05.05 */
/* Version 3.1.0.6 modification, 2011.12.01 */
    if (g_GPIO_I2C_devices->dwChipVersion == 0) //Mozart V1
    {
/* ======================================== */
        setdir(GPIOI2C_INTR, DIR_IN);
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_INTRMASK);
        writel(ret&(~GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_INTRMASK);
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_INTREN);
        writel(ret&(~GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_INTREN);
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_INTRTRI);
        writel(ret&(~GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_INTRTRI);  //edge trigger
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_INTRRISE);
        writel(ret|(GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_INTRRISE); //low-active
        ret = readl(GPIO_I2C_Reg + GPIOC_MMR_BOTH);
        writel(ret&(~GPIOI2C_INTR), GPIO_I2C_Reg + GPIOC_MMR_BOTH);  //single edge
/* Version 3.1.0.1 modification, 2009.12.03 */
/* ======================================== */
/* Version 3.1.0.6 modification, 2011.12.01 */
    }
/* ======================================== */
 /* ========================================= */

/* Version 3.0.0.16 modification, 2009.08.19 */
    if ((g_GPIO_I2C_devices->pbyBuff=(BYTE*)kmalloc(sizeof(BYTE)*iBuffLen, GFP_KERNEL)) == NULL)
    {
        ret = -ENOMEM;
        goto fail_init;
    }
 /* ========================================= */
    return 0; /* succeed */

fail_init:
    #ifdef MODULE
        cleanup_module();
    #else
        GPIOI2C_cleanup();
    #endif
    return ret;
}

#ifndef MODULE
module_init(GPIO_I2C_init);
module_exit(GPIO_I2C_cleanup);
#endif

/* Version 3.0.0.0 modification, 2007.09.05 */
module_param(g_GPIO_I2C_major, int, 0644);
/* ======================================== */
MODULE_PARM_DESC(g_GPIO_I2C_major, "Major no. for GPIO I2C");
MODULE_AUTHOR("VTCS Inc.");
MODULE_DESCRIPTION("I2C Driver Over GPIO on VTCS SoC");
MODULE_LICENSE("GPL");
/* Version 3.0.0.8 modification, 2009.04.01 */
//EXPORT_NO_SYMBOLS;
/* ======================================== */
