#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

#include "ov9710.h"

DECLARE_GLOBAL_DATA_PTR;

#define SYSC_VIC0_CLK_EN_BIT  7
#define SYSC_VIC1_CLK_EN_BIT  8
#define SYSC_VIC0_PAD_EN_BIT 19
#define SYSC_VIC1_PAD_EN_BIT 20

#define VIC_BASE VPL_VIC_MMR_BASE
#define VIC_CTRL_CHANNEL 0x10
#define VIC_OUT_EN_BIT 8
#define VIC_UPDATE_MMR_BIT 9

typedef struct OV9715_reg_addr_data
{
	unsigned long dwRegAddr;
	unsigned long dwData;
} TOV9715RegAddrData;
#define OV9715_TBL_SIZE 57
const TOV9715RegAddrData atOV9715regTbl[OV9715_TBL_SIZE] =
{
	{0x09,	0x10},
	{0x1E,	0x07},
	{0x5F,			0x18},
	{0x69,			0x04},
	{0x65,			0x2A},
	{0x68,			0x0A},
	{0x39,			0x28},
	{0x4D,			0x90},
	{0xC1,		0x80},
	{0x96,		0xF1},
	{0xBC,		0x68},
	{0x12,	0x00},
	{0x3B,			0x00},
	{0x97,		0x80},
	{0x17,	0x25},
	{0x18,			0xA2},
	{0x19,	0x01},
	{0x1A,			0xCA},
	{0x03,				0x0A},//The default max sutter speed is 30 fps.
	{0x32,				0x07},
	{0x98,		0x00},
	{0x99,		0x00},
	{0x9A,		0x00},
	{0x57,				0x01},
	{0x58,				0xC8},
	{0x59,				0xA0},
	{0x4C,			0x13},
	{0x4B,			0x36},
	{0x3D,				0x3C},
	{0x3E,				0x03},
	{0xBD,		0xA0},
	{0xBE,		0xC8},
	{0x4E,				0x55},
	{0x4F,				0x55},
	{0x50,				0x55},
	{0x51,				0x55},
	{0x24,				0x55},
	{0x25,				0x40},	
	{0x26,				0xA1},
	{0x5C,				0x5A},// 27MHz; 30fps
	{0x5D,				0x00},
	{0x11,		0x00},
	{0x2B,				0x06},
	{0x2A,				0x5E},
	{0x2D,				0x00},
	{0x2E,				0x00},
	{0x13,	0xFD},
	{0x14,	0x88},
	{0x0E,	0x48},
	{0xC2,		0x81},
	{0xCB,		0xAD},
	{0xD0,		0x06},
	{0xD1,		0x4A},
	{0x4A,				0x00},
	{0x49,				0xCF},
	{0x22,	0x04},
	{0x09,	0x00}
};

/*
 * Init all video-related registers.
 */
void video_init_ov9715(uchar codec_addr)
{
	int i ;
	
	for(i = 0; i < OV9715_TBL_SIZE; i++) {
		ov9715_write_data(codec_addr, atOV9715regTbl[i].dwRegAddr, atOV9715regTbl[i].dwData);
	}		
}

/*
 * Check OV9715 ID
 */
static int video_checkID_ov9715(uchar codec_addr)
{
	uchar data;

	ov9715_write_data(codec_addr, 0x12, 0x80);
	udelay(300);
	
	data = ov9715_read_data(codec_addr, 0x0A);
	if (data != 0x97)
		return -1;
	data = ov9715_read_data(codec_addr, 0x0B);
	if (data != 0x11)
		return -1;
	return 0;
}
/*
 * vic sysc settings
 */
void video_init_vic_sysc(int vic_num)
{
	unsigned long data;
	
	// sysc vic pad enable
	data = v_inl(EVM_SYSC_BASE + EVM_SYSC_PAD_EN_CTRL);
	data |= (vic_num==0) ? (1<<SYSC_VIC0_PAD_EN_BIT) : (1<<SYSC_VIC1_PAD_EN_BIT);
	v_outl( EVM_SYSC_BASE + EVM_SYSC_PAD_EN_CTRL, data);
	// sysc vic clk enable
	data = v_inl(EVM_SYSC_BASE + SYSC_MMR_CLKEN_CTRL1);
	data |= (vic_num==0) ? (1<<SYSC_VIC0_CLK_EN_BIT) : (1<<SYSC_VIC1_CLK_EN_BIT);	
	v_outl( EVM_SYSC_BASE + SYSC_MMR_CLKEN_CTRL1, data);

}

int vic_update_mmr(int vic_num) 
{
	int result ;
	unsigned long data, reg;
	
	// vic update mmr
	reg = VPL_VIC_MMR_BASE+VIC_CTRL_CHANNEL+vic_num*0x10;
	data = v_inl(reg);
	data |= (1<<VIC_OUT_EN_BIT);
	data |= (1<<VIC_UPDATE_MMR_BIT);	
	v_outl(reg, data);
	udelay(900);
	data = v_inl(reg);
	if (data & (1<<VIC_UPDATE_MMR_BIT)) {
		result = -1;
	}
	else {
		result = 0;
	}
	
	return result ;
}

int ov9715_video_test_func(void) 
{
	int result ;

	printf("Video Codec OV9715 test..\n") ;

	// check version ID
	printf("  - OV9715 ID Verification....\n") ;
	result = video_checkID_ov9715(OV9715_CODEC_ADDR);
	if(result != 0) {
		printf("FAIL!!\n");
		return -1;
	}
	else {
		printf("PASS.\n") ;
	}
	printf("\n") ;

	//init ov9715		
	video_init_ov9715(OV9715_CODEC_ADDR) ;	

	// init vic in sysc
	video_init_vic_sysc(0);

	// check vic update MMR	
	printf("  - OV9715 Pixel Clock Verification....\n") ;
	result = vic_update_mmr(0) ;
	if(result != 0) {
		printf("FAIL!!\n") ;
		return -1 ;
	}
	else {
		printf("PASS.\n") ;
	}
	printf("\n") ;	
	
	return 0;
}

int video_test_func(void)
{
	int result = 0 ;
	unsigned long agpoc_dir = v_inl(VPL_AGPOC_MMR_BASE+0x10) ;

	//We have to set the SCL/SDA pin numbers at first.
	SOFT_I2C_SCL_PIN = MOZART_VIDEO_SCL ;
	SOFT_I2C_SDA_PIN = MOZART_VIDEO_SDA ;
	i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	//Disable AGPOC Output dir.
	v_outl(VPL_AGPOC_MMR_BASE+0x10, agpoc_dir | SOFT_I2C_SCL_PIN | SOFT_I2C_SDA_PIN) ;

	result = ov9715_video_test_func() ;

	//restore the value
	v_outl(VPL_AGPOC_MMR_BASE+0x10, agpoc_dir) ;

	return result ;
}



