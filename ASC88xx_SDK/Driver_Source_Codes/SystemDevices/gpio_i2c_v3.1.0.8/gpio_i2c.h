/*
 * $Header: /rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C/gpio_i2c.h 32    12/09/25 3:18p Vincent $
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
 * $History: gpio_i2c.h $
 * 
 * *****************  Version 32  *****************
 * User: Vincent      Date: 12/09/25   Time: 3:18p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
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
 * User: Alan         Date: 09/12/09   Time: 4:23p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * UPDATE: Update version number from 3.1.0.1 to 3.1.0.2 - DONE.
 *
 * *****************  Version 24  *****************
 * User: Alan         Date: 09/12/04   Time: 2:15a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * UPDATE: Update version number from 3.1.0.0 to 3.1.0.1 - DONE.
 *
 * *****************  Version 23  *****************
 * User: Alan         Date: 09/11/21   Time: 8:04p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * MODIFICATION: GPIOI2C_NO_ACK is change to 0x0002 and add
 * GPIOI2C_IGNORE_ACK (0x0008) - DONE.
 *
 * *****************  Version 22  *****************
 * User: Alan         Date: 09/11/14   Time: 11:44p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 21  *****************
 * User: Alan         Date: 09/09/25   Time: 7:08p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 20  *****************
 * User: Alan         Date: 09/08/19   Time: 10:15p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 19  *****************
 * User: Alan         Date: 09/08/18   Time: 11:57a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 18  *****************
 * User: Albert.shen  Date: 09/05/12   Time: 9:48p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 17  *****************
 * User: Albert.shen  Date: 09/05/12   Time: 6:23p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 16  *****************
 * User: Ycchang      Date: 09/05/11   Time: 5:14p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 15  *****************
 * User: Albert.shen  Date: 09/05/05   Time: 10:17p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 14  *****************
 * User: Albert.shen  Date: 09/04/23   Time: 3:32p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 13  *****************
 * User: Albert.shen  Date: 09/04/13   Time: 8:55p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 12  *****************
 * User: Evelyn       Date: 09/04/02   Time: 11:40a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 11  *****************
 * User: Aniki        Date: 09/04/01   Time: 1:54p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 10  *****************
 * User: Evelyn       Date: 09/03/13   Time: 4:37p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 9  *****************
 * User: Albert.shen  Date: 09/02/25   Time: 10:12a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 8  *****************
 * User: Albert.shen  Date: 08/12/10   Time: 3:45p
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 7  *****************
 * User: Ycchang      Date: 08/09/15   Time: 9:06a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 6  *****************
 * User: Ycchang      Date: 08/03/20   Time: 10:55a
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
 * User: Alan         Date: 06/05/15   Time: 9:41a
 * Updated in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 * Update version number from 1.0.0.5 to 2.0.0.1 - DONE.
 *
 * *****************  Version 1  *****************
 * User: Chikuang     Date: 06/01/23   Time: 11:37a
 * Created in $/rd_2/project/SoC/Components/GPIOC/Device_Driver/GPIO_I2C
 *
 * *****************  Version 8  *****************
 * User: Sor          Date: 04/12/29   Time: 4:41p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 * FEATURE: Support non-stop bit read/write - DONE.
 *
 * *****************  Version 7  *****************
 * User: Steve        Date: 04/07/11   Time: 9:19p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 6  *****************
 * User: Steve        Date: 04/07/06   Time: 10:34p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 5  *****************
 * User: Vicky        Date: 04/07/06   Time: 4:36p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 4  *****************
 * User: Vicky        Date: 04/07/06   Time: 3:39p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 3  *****************
 * User: Vicky        Date: 04/07/06   Time: 10:01a
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 2  *****************
 * User: Sor          Date: 04/06/24   Time: 12:33p
 * Updated in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 * *****************  Version 1  *****************
 * User: Vicky        Date: 04/06/12   Time: 10:45a
 * Created in $/rd_2/project/Vivaldi/Device_Drivers/GPIO_I2C/EVM
 *
 */

/* ============================================================================================= */
#ifndef __GPIOI2C_H__
#define __GPIOI2C_H__

/* ============================================================================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
#include "typedef.h"
/* ======================================== */

/* ============================================================================================= */
/* Version 2.0.0.1 modification, 2006.05.15 */
#define GPIOI2C_ID_VERSION "3.1.0.8"
/* ======================================== */

/* ============================================================================================= */
#define GPIOI2C_MAJOR_DEFAULT	243

#define GPIOI2C_MAGIC		0xd0

#define GPIOI2C_MAXNR 		0

/* Version 2.0.0.0 modification, 2004.12.29 */
#define GPIOI2C_STOP	0
/* Version 3.1.0.0 modification, 2009.11.15 */
#define GPIOI2C_NO_STOP	0x0001
/* ======================================== */
/* ======================================== */

/* Version 3.0.0.5 modification, 2008.12.04 */
#define GPIOI2C_ACK		0
/* Version 3.1.0.0 modification, 2009.11.15 */
#define GPIOI2C_NO_ACK	0x0002
/* ======================================== */
/* ======================================== */

/* Version 3.0.0.10 modification, 2009.04.22 */
#define GPIOI2C_NORMAL  0
/* Version 3.1.0.0 modification, 2009.11.15 */
#define GPIOI2C_FAST    0x0004
/* ======================================== */
/* ======================================== */

/* Version 3.1.0.0 modification, 2009.11.15 */
#define GPIOI2C_IGNORE_ACK 0x0008
/* ======================================== */
/* ============================================================================================= */
#endif //__GPIOI2C_H__

/* ============================================================================================= */
