#ifndef __OV9715_H__
#define __OV9715_H__

#include <i2c.h>

#define OV9715_CODEC_ADDR 0x30

static void ov9715_write_data(uchar codec_addr, unsigned long reg, uchar data)
{		
    i2c_write(codec_addr, reg, 1, &data, 1) ;
}

static uchar ov9715_read_data(uchar codec_addr, uchar reg)
{
	uchar data ;
	
	i2c_read(codec_addr, reg, 1, &data, 1) ;
	
	return data ;
}

#endif

