#include <common.h>
#include <command.h>
#include <config.h>
#include <i2c.h>
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    This is still under development!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if 1
int video_test_func(void)
{
    int result = 0 ;

    printf("This is still under development!!\n") ;    

    return result ;
}
#else
#define VPL_VIC_MMR_BASE 					0xCA000000
#define VIC_DEVICE_NUM                      0
#define VIC_CHANNEL_NUM                     0
#define VPL_VIC_MMR_CTRL                    0x4
#define VPL_VIC_MMR_CTRL_A_DEVICE           (0X08 + (VIC_DEVICE_NUM * 4))
#define VPL_VIC_MMR_CTRL_A_CHANNEL			(0x10 + ((VIC_DEVICE_NUM * 4) + VIC_CHANNEL_NUM) * 4)
#define VPL_VIC_MMR_STAT_OFFSET				(0X30 + (VIC_DEVICE_NUM * 4))
#define VPL_VIC_MMR_IN_SIZE_OFFSET		    (0x5C + (VIC_DEVICE_NUM * 0xE0) + (VIC_CHANNEL_NUM * 0x38))
#define VPL_VIC_MMR_CAP_H_OFFSET			(0x60 + (VIC_DEVICE_NUM * 0xE0) + (VIC_CHANNEL_NUM * 0x38))
#define VPL_VIC_MMR_CAP_V_OFFSET			(0x64 + (VIC_DEVICE_NUM * 0xE0) + (VIC_CHANNEL_NUM * 0x38))
#define VPL_VIC_MMR_NO_SIGNAL_OFFSET	    0x200

int VIC_test_func(void)
{
    int i, iframe_error=0, ierror=0;
    int result = 0 ;

    printf("VIC testing...\n") ;
	
	v_outl(VPL_VIC_MMR_BASE + VPL_VIC_MMR_IN_SIZE_OFFSET,   (480<<16)|(640&0x00000FFF));//image size is 480 * 640 (H * W)
	v_outl(VPL_VIC_MMR_BASE + VPL_VIC_MMR_CAP_H_OFFSET,     (640<<16)|(0&0x00000FFF));//set horizontal parameters for device
	v_outl(VPL_VIC_MMR_BASE + VPL_VIC_MMR_CAP_V_OFFSET,     (480<<16)|(0&0x00000FFF));//set vertical parameters for device
	v_outl(VPL_VIC_MMR_BASE + VPL_VIC_MMR_NO_SIGNAL_OFFSET, AHB_CLOCK);//number AHB cycles to generate no-singal interrupt
	
	v_outl(VPL_VIC_MMR_BASE + VPL_VIC_MMR_CTRL_A_CHANNEL, 0x913c);//video ctrl register
    v_outl(VPL_VIC_MMR_BASE + VPL_VIC_MMR_CTRL, ((AHB_CLOCK/1000) << 8) | 1);	

    //waiting for ready..
	i=0;
    while( i<100 && 
           ((v_inl(VPL_VIC_MMR_BASE+VPL_VIC_MMR_CTRL_A_CHANNEL) & (1 << 9))==1/*this bit will be cleared after updating*/  
            || (v_inl(VPL_VIC_MMR_BASE+VPL_VIC_MMR_STAT_OFFSET) & 0x1)==0) /*1: VIC complete, 0 : VIC not complete*/
         )
    {
  	    i++;
      	udelay(10000);
  	    //printf("VPL_VIC_MMR_CTRL_A_CHANNEL %x\n", v_inl(VPL_VIC_MMR_BASE+VPL_VIC_MMR_CTRL_A_CHANNEL));
      	//printf("status %x\n", v_inl(VPL_VIC_MMR_BASE+VPL_VIC_MMR_STAT_OFFSET));
    }

    //frame status
	iframe_error = (v_inl(VPL_VIC_MMR_BASE+VPL_VIC_MMR_STAT_OFFSET)&0x10)>>4;

	//If bit#2 is set => Error(No-Signal)!, if bit#1 is set => Error(FIFO full occurs!)
	ierror = v_inl(VPL_VIC_MMR_BASE+VPL_VIC_MMR_STAT_OFFSET)&0x06;
    
	if(iframe_error==1 || ierror>0 )
	{
		result = -1 ;
	}
    else
    {
		result = 0 ;
	}

    return result ;
}

/*
 * == MT9P031 Test Function ==
 * Because MT9P031 has ID registers, we only need to check its ID.
 */
#define MT9P031_DEVADDR (0xBA >> 1)
#define MT9P031_HIGHID 0x18
#define MT9P031_LOWID  0x01
int mt9p031_test_func(void) 
{
    uchar buf[2] ;
    uchar tmp_reg ;

    printf("Video Sensor MT9P031 test..\n") ;

    tmp_reg = 0xff ;
    CONFIG_SOFT_I2C_READ_REPEATED_START = 1 ;
    i2c_read(MT9P031_DEVADDR, tmp_reg, 1, buf, sizeof(buf) / sizeof(uchar)) ;
    CONFIG_SOFT_I2C_READ_REPEATED_START = 0 ;    

    if((buf[0] != MT9P031_HIGHID) || (buf[1] != MT9P031_LOWID))
        return -1 ;
    
    return 0 ;
}

int video_test_func(void)
{
    int result = 0 ;

    //We have to set the SCL/SDA pin numbers at first.
    SOFT_I2C_SCL_PIN = MOZART_VIDEO_SCL ;
    SOFT_I2C_SDA_PIN = MOZART_VIDEO_SDA ;
    i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

    result = mt9p031_test_func() ;    

    if(result != 0)
        return result ;

    result |= VIC_test_func() ;

    return result ;
}
#endif


