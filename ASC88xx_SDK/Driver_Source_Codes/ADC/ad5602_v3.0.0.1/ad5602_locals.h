/*
 * $Header: /rd_2/Peripherals/AD5602/Device_Driver/AD5602/ad5602_locals.h 6     12/09/24 1:55p Joe.tu $
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
 * Description:
 *
 * $History: ad5602_locals.h $
 * 
 * *****************  Version 6  *****************
 * User: Joe.tu       Date: 12/09/24   Time: 1:55p
 * Updated in $/rd_2/Peripherals/AD5602/Device_Driver/AD5602
 * 
 * *****************  Version 5  *****************
 * User: Evelyn       Date: 12/01/05   Time: 3:27p
 * Updated in $/rd_2/Peripherals/AD5602/Device_Driver/AD5602
 * 
 * *****************  Version 4  *****************
 * User: Evelyn       Date: 11/06/23   Time: 2:03p
 * Updated in $/rd_2/Peripherals/AD5602/Device_Driver/AD5602
 * 
 * *****************  Version 3  *****************
 * User: Joe.tu       Date: 11/01/21   Time: 11:10a
 * Updated in $/rd_2/Peripherals/AD5602/Device_Driver/AD5602
 * 
 * *****************  Version 2  *****************
 * User: Joe.tu       Date: 11/01/13   Time: 4:43p
 * Updated in $/rd_2/Peripherals/AD5602/Device_Driver/AD5602
 * 
 * *****************  Version 1  *****************
 * User: Evelyn       Date: 10/07/24   Time: 5:59p
 * Created in $/rd_2/Peripherals/AD5602/Device_Driver/AD5602
 * 
 *
 */

/* ============================================================================================== */
#ifndef __IRISCTRL_LOCALS_H__
#define __IRISCTRL_LOCALS_H__

/* ============================================================================================== */
#include "vivo_codec.h"

/* ============================================================================================== */
/* Version 2.0.0.0 modification, 2011.06.20 */
/* ======================================== */
typedef enum dciris_aperture_sz
{
    DCIRIS_APERTURE_CLOSE = 0,
    DCIRIS_APERTURE_MID = 1,
    DCIRIS_APERTURE_OPEN = 2,
} EDCIrisApertureSz;

typedef enum dciris_speed
{
    DCIRIS_ZERO_SPEED = 0,
    DCIRIS_SLOWEST_SPEED = 1,
    DCIRIS_SLOW_SPEED = 2,
    DCIRIS_NORMAL_SPEED = 3,
    DCIRIS_FAST_SPEED = 4,
    DCIRIS_FASTEST_SPEED = 5,
    DCIRIS_SPEED_NUM = 6,
} EDCIrisSpeed;

/* Version 3.0.0.0 modification, 2012.09.20 */
/* ======================================== */

/* ============================================================================================== */
#endif //__IRISCTRL_LOCALS_H__

/* ============================================================================================== */
