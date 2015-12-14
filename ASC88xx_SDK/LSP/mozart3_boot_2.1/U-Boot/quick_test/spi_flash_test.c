#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>
#include <spi.h>
#include <spi_flash.h>
#include <malloc.h>

#define SPIFLASH_TEST_DATA_CONTENT   0x82455282
#define SPIFLASH_TEST_DATA_BLOCK_NUM 144 //Addr = 144 * 64KB = 0x00900000

static char test_pattern[] = { 0xff, 0x00, 0x5a, 0x69, 0xa5, 0x96 } ;

extern struct spi_flash *spi_flash_probe_vpl(struct spi_slave *spi, u8 *idcode);

int spi_flash_test_func(void)
{
	unsigned long block_size ;
	char write_data[256] ;
	char read_data[256] ;
	char *original_data ;
	struct spi_flash *flash ;
	int result = -1 ;
	int i ;

	flash = spi_flash_probe(CONFIG_SPI_FLASH_BUS, 0, CONFIG_SF_DEFAULT_SPEED,
							CONFIG_DEFAULT_SPI_MODE);
	if (!flash)
	{
		printf("probe failed!\n");
		return -1;
	}

	block_size = SZ_64K;

	//backup the original content
	original_data = (char *)mALLOc(block_size * sizeof(char)) ;
	if (spi_flash_read(flash, SPIFLASH_TEST_DATA_BLOCK_NUM * block_size, block_size, original_data))
		goto fail;

	//fill the test pattern
	for( i = 0 ; i < 256 ; i++ ) {
		write_data[i] = test_pattern[i % sizeof(test_pattern)] ;
	}

	if (spi_flash_erase(flash, SPIFLASH_TEST_DATA_BLOCK_NUM * block_size, block_size, 1))
		goto fail;
	printf( "%% Erase SPI Flash block#%3d\n", SPIFLASH_TEST_DATA_BLOCK_NUM ) ;
	//write_data = SPIFLASH_TEST_DATA_CONTENT ;
	printf( "%% Write test data(0x%lx) to SPI Flash block#%3d...", (unsigned long)write_data,
			SPIFLASH_TEST_DATA_BLOCK_NUM) ;
	if (spi_flash_write(flash, SPIFLASH_TEST_DATA_BLOCK_NUM * block_size, sizeof(write_data),
							write_data, 1))
		goto fail;
	printf( "done\n" ) ;
	printf( "%% Read data from SPI Flash block#%3d...", SPIFLASH_TEST_DATA_BLOCK_NUM) ;
	if (spi_flash_read(flash, SPIFLASH_TEST_DATA_BLOCK_NUM * block_size, sizeof(write_data), read_data))
		goto fail;
	printf( "done\n" ) ;

	//compare the data
	result = 1 ;
	for(i = 0 ; i < 256; i++) {
		if(write_data[i] != read_data[i]) {
			result = -1 ;
			break ;
		}
	}

	printf("%% WriteData %c= ReadData\n\n", result < 0 ? '!' : '=');
	printf("Restore the original content....\n") ;
	if (spi_flash_erase(flash, SPIFLASH_TEST_DATA_BLOCK_NUM * block_size, block_size, 1)
	 || spi_flash_write(flash, SPIFLASH_TEST_DATA_BLOCK_NUM * block_size, block_size, original_data, 1)
	 || result < 0)
		goto fail;

	return 0;

fail:
	fREe(original_data);
	return result ;
}
