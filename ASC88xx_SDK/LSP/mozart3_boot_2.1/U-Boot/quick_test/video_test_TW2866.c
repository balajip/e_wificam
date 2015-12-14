#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

#include "tw2866.h"

DECLARE_GLOBAL_DATA_PTR;

#define SYSC_VIC0_CLK_EN_BIT  7
#define SYSC_VIC1_CLK_EN_BIT  8
#define SYSC_VIC0_PAD_EN_BIT 19
#define SYSC_VIC1_PAD_EN_BIT 20

#define VIC_CTRL_CHANNEL 0x10
#define VIC_OUT_EN_BIT 8
#define VIC_UPDATE_MMR_BIT 9

typedef struct TW2866_reg_addr_data
{
	unsigned long dwRegAddr;
	unsigned long dwData;
} TTW2866RegAddrData;
#define TW2866_TBL_SIZE 30
const TTW2866RegAddrData atTW2866regTbl[TW2866_TBL_SIZE] =
{
		{0x4b, 0x00},		//DAC no power low 

		{0x60,0x15},		//XTI&XTO (27MHz)

		{0x9e,0x52},		// below settings are for 108MHz 1chan D1
		{0xca,0xaa},
		{0xcb,0x00},
		{0xfa,0x4a},
		{0xfb,0x2f},
		{0x6a,0x0a},
		{0x6b,0x0a},
		{0x6c,0x0a},			
		{0x9f,0x00},        // clk delay chn0 
		{0x67,0x00},        // clk delay chn1 
		{0x68,0x00},        // clk delay chn2          	  
		{0x69,0x00},        // clk delay chn3          		
		{0x5b,0xff},
		{0x96,0xe6},
		{0x41,0xd4},	
		{0x97,0xc5},		// free run mode : 60Hz
		
		{0x08,0x02},		//vertical delay
		{0x0a,0x02},		//horizontal delay
		{0x18,0x02},
		{0x1a,0x02},
		{0x28,0x02},
		{0x2a,0x02},
		{0x38,0x02},
		{0x3a,0x02},

		{0x0e, 0x0f},		// disable the shadow registers
		{0x1e, 0x0f},
		{0x2e, 0x0f},
		{0x3e, 0x0f},		
};

/*
 * Init all video-related registers.
 */
void video_init_tw2866(uchar codec_addr)
{
	int i ;
	
	for(i = 0; i < TW2866_TBL_SIZE; i++) {
		tw2866_write_data(codec_addr, atTW2866regTbl[i].dwRegAddr, atTW2866regTbl[i].dwData);
	}		
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

int tw2866x1_video_test_func(void) 
{
	int result ;

	printf("Video Codec TW2866 test..\n") ;
	printf("  - TW2866 ID Verification....\n") ;
	result = tw2866_version_check_func(TW2866x1_CODEC01_ADDR) ;
	if(result != 0) {
		printf("FAIL!!\n") ;
		return -1 ;
	}
	else {
		printf("PASS.\n") ;
	}
	printf("\n") ;
	
	//init tw2866		
	video_init_tw2866(TW2866x1_CODEC01_ADDR) ;	
	
	// init vic in sysc
	video_init_vic_sysc(0);

	// check vic update MMR	
	printf("  - TW2866 Pixel Clock Verification....\n") ;
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

int tw2866x2_video_test_func(void) 
{
	int result, i;

	printf("Video Codec TW2866 test..\n") ;
	printf("  - TW2866 ID Verification....\n") ;
	printf("   - Check 1st codec...") ;	
	result = tw2866_version_check_func(TW2866x2_CODEC01_ADDR) ;
	if(result != 0) {
		printf("FAIL!!\n") ;
		return -1 ;
	}
	else {
		printf("PASS.\n") ;
	}
	printf("   - Check 2nd codec...") ;	
	result = tw2866_version_check_func(TW2866x2_CODEC02_ADDR) ;
	if(result != 0) {
		printf("FAIL!!\n") ;
		return -1 ;
	}
	else {
		printf("PASS.\n") ;
	}
	printf("\n") ;
	
	//init tw2866		
	video_init_tw2866(TW2866x2_CODEC01_ADDR) ;	
	video_init_tw2866(TW2866x2_CODEC02_ADDR) ;	

	// check vic update MMR
	printf("  - TW2866 Pixel Clock Verification....\n") ;
	for (i = 0; i < 2; i++) {		// vic0 and vic1
		// init vic in sysc
		video_init_vic_sysc(i);
		// update mmr
		result = vic_update_mmr(i) ;
		printf("   - Check codec %d...", i) ;			
		if(result != 0) {
			printf("FAIL!!\n") ;
			return -1 ;
		}
		else {
			printf("PASS.\n") ;
		}
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

#if (PLATFORM_VIDEO_CODEC_TYPE == PLATFORM_VIDEO_CODEC_TW2866x1)
	result = tw2866x1_video_test_func() ;
#else
	result = tw2866x2_video_test_func() ;
#endif

	//restore the value
    v_outl(VPL_AGPOC_MMR_BASE+0x10, agpoc_dir) ;

	return result ;
}



