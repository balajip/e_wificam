#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

#include "tw2866.h"

DECLARE_GLOBAL_DATA_PTR;

static unsigned long I2S_BASE_ARRAY[] = {I2S0_BASE_ADDR, I2S1_BASE_ADDR, I2S2_BASE_ADDR, I2S3_BASE_ADDR, I2S4_BASE_ADDR} ;

static const unsigned char tw2866_all_regs_audiotest[TW2866_CACHEREGNUM] = {
         // 0     // 1    // 2    // 3     // 4    // 5    // 6     // 7   // 8     // 9    // A     // B    // C    // D     // E    // F
/*0x00*/ 0x68,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0F,   0xD0,   0x00,   0x10,   0x07,   0x7F,  
/*0x10*/ 0x87,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0C,   0xD0,   0xD0,   0x00,   0x27,   0x7F,  
/*0x20*/ 0x97,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0C,   0xD0,   0xD0,   0x00,   0x27,   0x7F,  
/*0x30*/ 0x87,   0x00,   0x64,   0x11,   0x80,   0x80,   0x00,   0x02,  0x12,   0xF0,   0x0C,   0xD0,   0x10,   0x00,   0x17,   0x7F,  
/*0x40*/ 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
/*0x50*/ 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x7F,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
/*0x60*/ 0x15,   0x03,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
/*0x70*/ 0x00,   0x00,   0x00,   0x01,   0x01,   0x03,   0xEF,   0x03,  0xDD,   0x03,   0xCA,   0x15,   0x15,   0xE4,   0xA3,   0xB0,
/*0x80*/ 0x00,   0x02,   0x00,   0xCC,   0x00,   0x80,   0x44,   0x50,  0x22,   0x02,   0xD8,   0xBC,   0xB8,   0x44,   0x38,   0x00,
/*0x90*/ 0x00,   0x78,   0x44,   0x3D,   0x14,   0xA5,   0xE0,   0x05,  0x00,   0x28,   0x44,   0x44,   0xA0,   0x8A,   0x42,   0x00,
/*0xA0*/ 0x08,   0x08,   0x08,   0x08,   0x1A,   0x1A,   0x1A,   0x1A,  0x00,   0x00,   0x00,   0xF0,   0xF0,   0xF0,   0xF0,   0x00,
/*0xB0*/ 0x00,   0x4A,   0x00,   0xFF,   0xEF,   0xEF,   0xEF,   0xEF,  0xFF,   0xDD,   0xDD,   0xDD,   0xDB,   0xFF,   0xCA,   0xCB,
/*0xC0*/ 0xCA,   0xCC,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,  0x00,   0x00,   0x01,   0x00,   0x39,   0xE4,   0x00,   0x00,
/*0xD0*/ 0x88,   0x88,   0x00,   0x91,   0x99,   0x99,   0x99,   0x99,  0x99,   0x99,   0x99,   0xE1,   0x0f,   0x00,   0x00,   0xFF,
/*0xE0*/ 0x14,   0xE0,   0xBB,   0xBB,   0x00,   0x11,   0x00,   0x00,  0x11,   0x00,   0x00,   0x11,   0x00,   0x00,   0x11,   0x00,
/*0xF0*/ 0x83,   0xB5,   0x09,   0x78,   0x85,   0x00,   0x01,   0x20,  0xC4,   0x11,   0x4a,   0x2F,   0xFF,   0xF1,   0x00,   0x63
};

// There are total 256 regs for TW2866.
// But a lot of them are for Video purposes.
// Here we put Audio-Related-Regs together to prevent from accessing Video regs.
static const unsigned char tw2866_audio_regs_index[] = {
        TW2866_REG_ADACCT1     ,
        TW2866_REG_DAC_LPFBIAS ,
        TW2866_REG_ADACLPF     ,
        TW2866_REG_ANADACTEST  ,
        TW2866_REG_ASAVE       ,
        TW2866_REG_ACLKPOL     ,
        TW2866_REG_AINCTL      ,
        TW2866_REG_AIN5MD      ,
        TW2866_REG_AIGAIN21    ,
        TW2866_REG_AIGAIN43    ,
        TW2866_REG_AIGAIN5     ,
        TW2866_REG_R_MULTCH    ,
        TW2866_REG_R_SEQ10     ,
        TW2866_REG_R_SEQ32     ,
        TW2866_REG_R_SEQ54     ,
        TW2866_REG_R_SEQ76     ,
        TW2866_REG_R_SEQ98     ,
        TW2866_REG_R_SEQBA     ,
        TW2866_REG_R_SEQDC     ,
        TW2866_REG_R_SEQFE     ,
        TW2866_REG_AMASTER     ,
        TW2866_REG_MIX_MUTE    ,
        TW2866_REG_SMD         ,
        TW2866_REG_ADET        ,
        TW2866_REG_ADET_TH21   ,
        TW2866_REG_ADET_TH43   ,
        TW2866_REG_ACKI_L      ,
        TW2866_REG_ACKI_M      ,
        TW2866_REG_ACKI_H      ,
        TW2866_REG_ACKN_L      ,
        TW2866_REG_ACKN_M      ,
        TW2866_REG_ACKN_H      ,
        TW2866_REG_SDIV        ,
        TW2866_REG_LRDIV       ,
        TW2866_REG_ACCNTL      ,
        TW2866_REG_AVDET_ENA   ,
        TW2866_REG_A5NUM       ,
        TW2866_REG_AVDET_STATE ,
        TW2866_REG_A5DETST     ,
        TW2866_REG_SRST        ,
        TW2866_REG_ANAPWDN     ,
        TW2866_REG_PLL1        ,
        TW2866_REG_PLL2        ,
        TW2866_REG_I2SO_RSEL   , 
        TW2866_REG_I2SO_LSEL   , 
        TW2866_REG_RECSEL5     ,
        TW2866_REG_AADCOFS_H   , 
        TW2866_REG_AADC1OFS_L  , 
        TW2866_REG_AADC2OFS_L  , 
        TW2866_REG_AADC3OFS_L  , 
        TW2866_REG_AADC4OFS_L  , 
        TW2866_REG_AADC5OFS_H  , 
        TW2866_REG_AADC50FS_L  ,
        TW2866_REG_AUDADC_H    , 
        TW2866_REG_AUD1ADC_L   , 
        TW2866_REG_AUD2ADC_L   , 
        TW2866_REG_AUD3ADC_L   , 
        TW2866_REG_AUD4ADC_L   , 
        TW2866_REG_AUD5ADC_H   , 
        TW2866_REG_AUD5ADC_L   , 
        TW2866_REG_ADJAADC_H   , 
        TW2866_REG_ADJAADC1_L  , 
        TW2866_REG_ADJAADC2_L  , 
        TW2866_REG_ADJAADC3_L  , 
        TW2866_REG_ADJAADC4_L  , 
        TW2866_REG_ADJAADC5_H  , 
        TW2866_REG_ADJAADC5_L  , 
        TW2866_REG_MRATIOMD    , 
        TW2866_REG_MIX_RATIO21 , 
        TW2866_REG_MIX_RATIO43 , 
        TW2866_REG_MIX_RATIOP  , 
        TW2866_REG_MIX_OUTSEL  ,
        TW2866_REG_CLKOCTL     , 
        TW2866_REG_AVDET_MODE
} ;

static void reset_tw2866(uchar codec_addr, uchar reset_value)
{
    //printf(" ## Reseting tw2866x1, please wait.. ##\n") ;
    tw2866_write_data(codec_addr, TW2866_REG_SRST, reset_value) ;
}

#define RX_MCLK         11289600
#define TX_MCLK         11289600
#define SYSC_I2S_RX_MCLK_DIV 22
#define SYSC_I2S_TX_MCLK_DIV 22
int init_i2s(int i2s_num, int enable)
{
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_IER,  0) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_IRER, 0) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_ITER, 0) ;

    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_IMR, 0x0) ;

    //1. set FIFO threshold & flush FIFO
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_RFCR, 0x3) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_TFCR, 0x3) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_RFF0, 0x1) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_TFF0, 0x1) ;
    
    //2. set Data width
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_RCR0, 0x2) ;//Each channel has 16bits(format is PCM), and the resolution of I2S receive is 16*2=32bits
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_TCR0, 0x2) ; 

    //4. enable blocks/channels/module
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_IER,  1) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_IRER, 1) ;
    v_outl(I2S_BASE_ARRAY[i2s_num] + I2S_MMR_ITER, 1) ;

	return 0 ;
}

/*
 * Init all audio-related registers.
 */
int audio_init_tw2866(uchar codec_addr)
{
	int i ;
	
	for(i = 0 ; i < ARRAY_SIZE(tw2866_audio_regs_index) ; i++) {
		int reg = tw2866_audio_regs_index[i] ;
		int val = tw2866_all_regs_audiotest[reg] ;
		tw2866_write_data(codec_addr, reg, val);
		//printf("write[0x%x] = 0x%02x\n", reg, val) ;
	}		

	return 0 ;
}

int tw2866_dataline_test_func(uchar codec_addr, int playback_ch_num, int capture_ch_num) 
{
	int delay = 10 ;
	int rx_i2s_num, tx_i2s_num ;
	int rx_i2s_base, tx_i2s_base ;    
	uchar mux_devaddr, mux_data ;
	
	// setup the PLAYBACK channels
#if (PLATFORM_AUDIO_CODEC_TYPE == PLATFORM_AUDIO_CODEC_TW2866x1)	
	if((playback_ch_num < 0) || (playback_ch_num > 3)) {
		printf("[ERR] PLAYBACK channel number is 0 ~ 3!!\n") ;
		return -1 ;
	}	
#else
	if((playback_ch_num < 0) || (playback_ch_num > 7)) {
		printf("[ERR] PLAYBACK channel number is 0 ~ 7!!\n") ;
		return -1 ;
	}
#endif
	mux_devaddr = (playback_ch_num < MUX_OUTPUT_NUM) ? MUX_DEV_ADDR_1ST : MUX_DEV_ADDR_2ND ;
	playback_ch_num = playback_ch_num % MUX_OUTPUT_NUM ;
	mux_data = (1 << playback_ch_num) & 0xF ;	
	if(i2c_write(mux_devaddr, 0, 0, &mux_data, 1)) {
		printf("[ERR]mux write err!!\n");	
		return -1 ;
	}

	// setup the CAPTURE channels	
	if((capture_ch_num < 0) || (capture_ch_num > 3)) {
		printf("[ERR] CAPTURE channel number is 0 ~ 3 for each codec!!\n") ;
		return -1 ;
	}
	tw2866_write_data(codec_addr, TW2866_REG_R_SEQ10, capture_ch_num);	

	printf("   - Test ANALOG-IN for 10 secs...\n") ;
    printf("   - (press ANY KEY to stop the test..)\n") ;

	tx_i2s_num = 0 ;
	if(codec_addr == 0x28) {
		rx_i2s_num = 1 ;
	}
	else {
		rx_i2s_num = 2 ;
	}	

	tx_i2s_base = I2S_BASE_ARRAY[tx_i2s_num] ;
	rx_i2s_base = I2S_BASE_ARRAY[rx_i2s_num] ;	

	init_i2s(tx_i2s_num, 1) ;
	init_i2s(rx_i2s_num, 1) ;
	
	reset_timer() ;
	while((get_timer(0)/1000) < delay) {
		unsigned long left = v_inl(rx_i2s_base + I2S_MMR_LRBR0) ;
        unsigned long right =  v_inl(rx_i2s_base + I2S_MMR_RRBR0) ;
		v_outl(tx_i2s_base + I2S_MMR_LTHR0, left);
		v_outl(tx_i2s_base + I2S_MMR_RTHR0, right);

		if(tstc())
		{		   
  	        (void) getc();  // consume input
			break;
		}
	}
	printf("   - STOP!\n") ;

	//disable i2s
	init_i2s(tx_i2s_num, 0) ;
	init_i2s(rx_i2s_num, 0) ;

	return 0 ;
}

int tw2866x1_audio_test_func(void) 
{
    int result ;
	int playback_ch_num, capture_ch_num ;

    printf("Audio Codec TW2866 test..\n") ;
	reset_tw2866(TW2866x1_CODEC01_ADDR, TW2866_RESET_AUDIO) ;
	
	printf("  - TW2866 ID Verification....") ;
	result = tw2866_version_check_func(TW2866x1_CODEC01_ADDR) ;
	if(result != 0) {
        printf("FAIL!!\n") ;
        return -1 ;
    }
    else {
        printf("PASS.\n") ;
    }

	printf("\n") ;
	printf("  - Audio-Data line testing....\n") ;
	printf("   - Initing the codec...\n") ;
	//init tw2866		
	audio_init_tw2866(TW2866x1_CODEC01_ADDR) ;	

	playback_ch_num = 0 ;
	capture_ch_num = 1 ;
    result = tw2866_dataline_test_func(TW2866x1_CODEC01_ADDR, playback_ch_num, capture_ch_num) ;
	if(result != 0) {
        printf("FAIL!!\n") ;
        return -1 ;
    }
    else {
        printf("PASS.\n") ;
    }

	playback_ch_num = 0 ;
	capture_ch_num = 0 ;
	result = tw2866_dataline_test_func(TW2866x1_CODEC01_ADDR, playback_ch_num, capture_ch_num) ;
	if(result != 0) {
        printf("FAIL!!\n") ;
        return -1 ;
    }
    else {
        printf("PASS.\n") ;
    }
    
    return result ;
}

int tw2866x2_audio_test_func(void) 
{
    int result ;
	int playback_ch_num, capture_ch_num ;

    printf("Audio Codec TW2866 test..\n") ;
	reset_tw2866(TW2866x2_CODEC01_ADDR, TW2866_RESET_AUDIO) ;
	reset_tw2866(TW2866x2_CODEC02_ADDR, TW2866_RESET_AUDIO) ;
	
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
	printf("  - Audio-Data line testing....\n") ;
	printf("   - Initing the codec...\n") ;
	//init tw2866		
	audio_init_tw2866(TW2866x2_CODEC01_ADDR) ;	
	audio_init_tw2866(TW2866x2_CODEC02_ADDR) ;

	playback_ch_num = 7 ;
	capture_ch_num = 0 ;
    result = tw2866_dataline_test_func(TW2866x2_CODEC01_ADDR, playback_ch_num, capture_ch_num) ;
	if(result != 0) {
        printf("FAIL!!\n") ;
        return -1 ;
    }
    else {
        printf("PASS.\n") ;
    }

	playback_ch_num = 7 ;
	capture_ch_num = 0 ;
	result = tw2866_dataline_test_func(TW2866x2_CODEC02_ADDR, playback_ch_num, capture_ch_num) ;
	if(result != 0) {
        printf("FAIL!!\n") ;
        return -1 ;
    }
    else {
        printf("PASS.\n") ;
    }
    
    return result ;
}

int audio_test_func(void)
{
    int result = 0 ;
    unsigned long agpoc_dir = v_inl(VPL_AGPOC_MMR_BASE+0x10) ;

    //We have to set the SCL/SDA pin numbers at first.
    SOFT_I2C_SCL_PIN = MOZART_AUDIO_SCL ;
    SOFT_I2C_SDA_PIN = MOZART_AUDIO_SDA ;
    i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	//Disable AGPOC Output dir.
    v_outl(VPL_AGPOC_MMR_BASE+0x10, agpoc_dir | SOFT_I2C_SCL_PIN | SOFT_I2C_SDA_PIN) ;

#if (PLATFORM_AUDIO_CODEC_TYPE == PLATFORM_AUDIO_CODEC_TW2866x1)
    result = tw2866x1_audio_test_func() ;
#else
	result = tw2866x2_audio_test_func() ;
#endif

	//restore the value
    v_outl(VPL_AGPOC_MMR_BASE+0x10, agpoc_dir) ;

    return result ;
}



