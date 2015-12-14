#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

#define DRAM_TEST_START     (96 * 1024 *1024) //96MBytes
#define DRAM_TEST_SIZE      (1024*1024) //1MBytes
static unsigned long test_pattern[] = { 0x00000000, 0xFFFFFFFF, 0x5A5A5A5A, 0x69696969, 0xFF00FF00, 0x00FF00FF} ;

int read_write_test(unsigned long start, unsigned long size)
{
    int array_size = sizeof(test_pattern) / sizeof(unsigned long) ;
    int result = 0 ;
    unsigned long test_addr, tmp_val ;

    //write..
    test_addr = start ;
    while(test_addr < (start + size)) {
        v_outl(test_addr, test_pattern[test_addr % array_size]) ;
        test_addr += 4 ;
    }

    //read & compare..
    test_addr = start ;
    while(test_addr < (start + size)) {
        tmp_val = v_inl(test_addr) ;
        if(tmp_val != (test_pattern[test_addr % array_size])) {
            printf(" !! We write 0x%08lx to addr 0x%08lx, but we get 0x%08lx !!\n", 
                    test_pattern[test_addr % array_size], test_addr, tmp_val
                  ) ; 
            return -1 ;
        }
        
        test_addr += 4 ;
    }
    

    return result ;
}

int dram_test_func(void)
{
    int i, result = 0 ;
    unsigned long dram_start ;


	for (i=0; i < CONFIG_DRAM_NUMBER; i++)
	{
		if (gd->bd->bi_dram[i].size == 0)
			continue;

		printf(" - DRAM#%d\n", i) ;
		dram_start = gd->bd->bi_dram[i].start;
		printf("    test-flow : Writing %d bytes from 0x%08lx, then read and compare...\n",
		       DRAM_TEST_SIZE, dram_start + DRAM_TEST_START);
		result = read_write_test(dram_start + DRAM_TEST_START, DRAM_TEST_SIZE);
		if (result != 0)
		{
		    printf("    test-result : FAIL!!\n");
		    return result;
		}
		else
		    printf("    test-result : PASS.\n");
	}

	return result ;
}
