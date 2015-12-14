#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>
#include <malloc.h>
#define VPL_MSHC_NUM PLATFORM_SDCARD_NUM
#define MSHC_TEST_PATTERN_SIZE SD_BLOCK_SIZE
#define MSHC_TEST_BLOCK_INDEX  256

#include <part.h>
#include <linux/list.h>
#include <mmc.h>
#include <div64.h>

//#define MSHC_TEST_DEBUG

int MSHC_do_test(int num)
{
	unsigned char origin_data[MSHC_TEST_PATTERN_SIZE] ;
	unsigned char test_pattern[MSHC_TEST_PATTERN_SIZE] ;
	unsigned char read_pattern[MSHC_TEST_PATTERN_SIZE] = {0} ;
				
	int dev = 0 ; //Default device is 0
	void *ori_addr = (void *)origin_data ;
	void *test_addr = (void *)test_pattern ;
	void *read_addr = (void *)read_pattern ;
	struct mmc *mmc = find_mmc_device(dev);
	int cnt = MSHC_TEST_PATTERN_SIZE / SD_BLOCK_SIZE ;

	int i ;	

	if (!mmc)
		return 1;

	mmc_init(mmc);

	//Init the test pattern
	for(i = 0 ; i < MSHC_TEST_PATTERN_SIZE ; i++) {
		switch(i % 4) {
			case 0 :
				test_pattern[i] = 0x00 ;
				break ;
			case 1 :
				test_pattern[i] = 0xFF ;
				break ;
			case 2 :
				test_pattern[i] = 0x5A ;
				break ;
			case 3 : default :
				test_pattern[i] = 0x69 ;
				break ;	
		} 
	}

	//read ori data	
#ifdef MSHC_TEST_DEBUG	
	printf("=============read ori data      =============== \n") ;
#endif
	mmc->block_dev.block_read(dev, MSHC_TEST_BLOCK_INDEX, cnt, ori_addr);
#ifdef MSHC_TEST_DEBUG	
	for(i = 0 ; i < 10 ; i++) {
		printf("ori[%d] = 0x%x\n", i, origin_data[i]) ;
	}
#endif	
	
	//write test pattern 
#ifdef MSHC_TEST_DEBUG		
	printf("===========write test pattern================= \n") ;
#endif
	mmc->block_dev.block_write(dev, MSHC_TEST_BLOCK_INDEX, cnt, test_addr);

	//read test pattern
#ifdef MSHC_TEST_DEBUG		
	printf("===========read test pattern================= \n") ;
#endif
	mmc->block_dev.block_read(dev, MSHC_TEST_BLOCK_INDEX, cnt, read_addr);
#ifdef MSHC_TEST_DEBUG	
	for(i = 0 ; i < 10 ; i++) {
		printf("read[%d] = 0x%x\n", i, read_pattern[i]) ;
	}
#endif	

	//restore original data 
#ifdef MSHC_TEST_DEBUG		
	printf("==========restore original data==================\n") ;
#endif
	mmc->block_dev.block_write(dev, MSHC_TEST_BLOCK_INDEX, cnt, ori_addr);
	
	//comapre the data
	for(i = 0 ; i < MSHC_TEST_PATTERN_SIZE ; i++) {
		if(test_pattern[i] != read_pattern[i]) {
			printf("[Diff] write[%d] = 0x%x ,but read[%d] = 0x%x. \n", i, test_pattern[i], i, read_pattern[i]) ;
			return 1 ;
		}
	}
	
	return 0;
}

int mshc_test_func(void)
{
	int result = 0;
	int num;
    unsigned reg_val;

    v_outl(VPL_SYSC_MMR_BASE+0x24, v_inl(VPL_SYSC_MMR_BASE+0x24)|(0x1<<27));
    v_outl(VPL_SYSC_MMR_BASE+0x44, v_inl(VPL_SYSC_MMR_BASE+0x44)|(0x1<<11));
    reg_val = v_inl(VPL_SYSC_MMR_BASE+0x30);
    reg_val &= 0xff;
    v_outl(VPL_SYSC_MMR_BASE+0x30, reg_val|0x1500);

	printf( "SD_test_func!!!\n" ) ;
	for(num = 0; num < VPL_MSHC_NUM; num++)
	{
		printf("SD %d testing...\n", num);
		result = MSHC_do_test(num);
		if (result) {
			printf("SD %d Fail!!\n", num);
            result = -1 ;
		}
		else
			printf("SD %d PASS!!\n", num);
	}

	return result ;
}

