#include <common.h>
#include <command.h>
#include <config.h>
#include <i2c.h>
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

#define RX_MCLK         11289600
#define TX_MCLK         11289600
#define SYSC_I2S_RX_MCLK_DIV 22
#define SYSC_I2S_TX_MCLK_DIV 22
void init_i2s(int enable)
{
    v_outl(I2S0_BASE_ADDR + I2S_MMR_IER,  0) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_IRER, 0) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_ITER, 0) ;

    v_outl(I2S0_BASE_ADDR + I2S_MMR_IMR, 0x0) ;

    //1. set FIFO threshold & flush FIFO
    v_outl(I2S0_BASE_ADDR + I2S_MMR_RFCR, 0x3) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_TFCR, 0x3) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_RFF0, 0x1) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_TFF0, 0x1) ;
    
    //2. set Data width
    v_outl(I2S0_BASE_ADDR + I2S_MMR_RCR0, 0x2) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_TCR0, 0x2) ; 

    //3. set input mclk
    v_outl(EVM_SYSC_BASE + SYSC_MMR_CLKEN_CTRL1, v_inl(EVM_SYSC_BASE + SYSC_MMR_CLKEN_CTRL1) | (0x3 << 11)) ;
    unsigned long clk_gen = v_inl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN) ;
    clk_gen = (clk_gen & ~(0xff << 0)) | (SYSC_I2S_TX_MCLK_DIV << 0) ;
    clk_gen = (clk_gen & ~(0xff << 8)) | (SYSC_I2S_RX_MCLK_DIV << 8) ;
    v_outl(EVM_SYSC_BASE + SYSC_MMR_CLK_GEN, clk_gen) ;
        
    //4. enable blocks/channels/module
    v_outl(I2S0_BASE_ADDR + I2S_MMR_IER,  1) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_IRER, 1) ;
    v_outl(I2S0_BASE_ADDR + I2S_MMR_ITER, 1) ;
}

/*
 * == SSM2603 Test Function ==
 * Because SSM2603 does not have ID registers, we read all registers and compare with their default values.
 */
#define SSM2603_DEVADDR 0x1A 
typedef struct {
    unsigned long sample_rate ;
    unsigned long sr_val ;
} ssm2603_table_entry ;
ssm2603_table_entry ssm2063_lookup_table[] = { { 8000, 0x3}, {16000, 0x5}, {32000, 0x6}, {44100, 0x8}, {48000, 0x0}} ;

void ssm2603_write_data(unsigned long reg, uint data)
{
    uchar buf[2] ;
    buf[0] = (reg << 1) | ((data >> 8) & 0x1);    
    buf[1] = data & 0xff ;
    i2c_write(SSM2603_DEVADDR, 0, 0, buf,2) ;
}

void reset_ssm2603(void)
{
    //printf(" ## Reseting SSM2603, please wait.. ##\n") ;
    ssm2603_write_data(0x0F, 0x0) ;
}

int init_ssm2603(unsigned long sample_rate, int enable, int input_type)//input_type : 1 is line-in, other is mic-in
{
    int i ;
    unsigned long sr_val = 0 ;

    if(enable == 0) {
        ssm2603_write_data(0x06, 0x1ff) ;//power down
        ssm2603_write_data(0x09, 0x0) ;//inactive
        reset_ssm2603() ;
        return 0;
    }

    reset_ssm2603() ;

    //1. get sample rate
    for(i = 0 ; i < (sizeof(ssm2063_lookup_table) / sizeof(ssm2603_table_entry)) ; i++) {
        if(ssm2063_lookup_table[i].sample_rate == sample_rate) {
            sr_val = ssm2063_lookup_table[i].sr_val ;
            break ;
        }
    }
    if(sr_val == 0) {
        printf("[ERR] You use the un-supported sample rate!\n") ;
        return -1 ;
    }

    //2. set sample rate    
    ssm2603_write_data(0x08, (sr_val << 2)) ;

    //3. write data width & format
    ssm2603_write_data(0x07, 0x42) ;

    //4. disable BYPASS mode(default is ENABLE)
    if(input_type == 1) {
        ssm2603_write_data(0x04, 0x110) ;
    }
    else {
        //select mic-in & turn off mic-boost
        ssm2603_write_data(0x04, 0x114) ;
    }

    //5. set digital audio path
    ssm2603_write_data(0x05, 0x0) ;

    //volume
    ssm2603_write_data(0x00, 0x117) ;
    ssm2603_write_data(0x01, 0x117) ;
    ssm2603_write_data(0x02, 0x179) ;
    ssm2603_write_data(0x03, 0x179) ;

    //6. active
    ssm2603_write_data(0x09, 0x1) ;

    //5. power up
    ssm2603_write_data(0x06, 0x0) ;

    udelay(50000);

    return 0 ;
}

int ssm2603_dataline_test_func(int input_type)//1 is line-in, other is mic-in.
{   
		unsigned int start = 0;
		
    //1. init i2s
    init_i2s(1) ;

    //2. init ssm2603
    init_ssm2603(8000, 1, input_type) ;     

    if(input_type == 1) {
        printf("  - Test ANALOG-IN for 10 secs...\n") ;
        printf("  - (press ANY KEY to stop the test..)\n") ;        
    } else {
        printf("  - Test MIC-IN for 10 secs...\n") ;
        printf("  - (press ANY KEY to stop the test..)\n") ;        
    }

    int delay = 10 ;
    //reset_timer() ;
    start = get_timer(0);
    while((get_timer(start)/1000) < delay)
	{
	    unsigned long left = v_inl(I2S0_BASE_ADDR + I2S_MMR_LRBR0) ;
        unsigned long right =  v_inl(I2S0_BASE_ADDR + I2S_MMR_RRBR0) ;
		v_outl(I2S0_BASE_ADDR + I2S_MMR_LTHR0, left);
		v_outl(I2S0_BASE_ADDR + I2S_MMR_RTHR0, right);
		if(tstc())
		{		   
  	        (void) getc();  /* consume input	*/
			break;
		}
        //printf("%d\n", get_timer_masked()) ;
	}
    printf("  - STOP!\n") ;

    init_ssm2603(0, 0, 0) ;
    init_i2s(0) ;

    return 0 ;
}

uint ssm2603_regs[] = { 0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00F, 0x010, 0x011, 0x012} ;
uint ssm2603_vals[] = { 0x097, 0x097, 0x079, 0x079, 0x00a, 0x008, 0x09f, 0x00a, 0x000, 0x000, 0x000, 0x07b, 0x032, 0x000} ;
int ssm2603_ctrlline_test_func(void)
{
    uchar reg_mask = 0x7F ;//7 bits mask
    uint val_mask = 0x1ff;//9 bits mask

    uchar buf[2] ;
    uchar tmp_reg ;
    uint tmp_val ;
    int alen = 1 ;    
    int i, j;

    reset_ssm2603() ;

    //We will repeate START singal in SSM2603
    CONFIG_SOFT_I2C_READ_REPEATED_START = 1 ;

    for(i = 0 ; i < (sizeof(ssm2603_regs) / sizeof(uint)); i++) {
        tmp_reg = ssm2603_regs[i] & reg_mask ;
        tmp_reg = tmp_reg << 1 ;
        i2c_read(SSM2603_DEVADDR, tmp_reg, alen, buf, sizeof(buf)) ;

        tmp_val = 0 ;
        for(j = 0 ; j < (sizeof(buf) / sizeof(uchar)); j++) {
            tmp_val |= buf[j] << (j * 8) ;
        }

        if((ssm2603_vals[i] & val_mask) != (tmp_val & val_mask)) {
            printf("  - Reg#%02d : Default Value(0x%03x) != Read Value(0x%03x)\n", 
                   ssm2603_regs[i], 
                   (ssm2603_vals[i] & val_mask), 
                   (tmp_val & val_mask)
                  );
            CONFIG_SOFT_I2C_READ_REPEATED_START = 0 ;
            return -1 ;
        }
    }

    CONFIG_SOFT_I2C_READ_REPEATED_START = 0 ;

    return 0 ;
}

int ssm2603_test_func(void) 
{
    int result ;

    printf("Audio Codec SSM2603 test..\n") ;

    printf("  - Audio-Ctrl line testing....") ;
    result = ssm2603_ctrlline_test_func() ;
    if(result != 0) {
        printf("FAIL!!\n") ;
        return -1 ;
    }
    else {
        printf("PASS.\n") ;
    }

#ifdef AUDIO_TEST_LINE_IN
    ssm2603_dataline_test_func(1) ;
#endif
#ifdef AUDIO_TEST_MIC_IN
    ssm2603_dataline_test_func(0) ;
#endif
    
    return result ;
}

int audio_test_func(void)
{
    int result = 0 ;

    //We have to set the SCL/SDA pin numbers at first.
    SOFT_I2C_SCL_PIN = MOZART_AUDIO_SCL ;
    SOFT_I2C_SDA_PIN = MOZART_AUDIO_SDA ;
    i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

    result |= ssm2603_test_func() ;

    return result ;
}



