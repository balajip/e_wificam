//-----------------------------------
// 2011.08.18
// Obsolete code.
// We use mmc subsystem now.
//-----------------------------------

#if 0
#include <common.h>
#include <environment.h>
DECLARE_GLOBAL_DATA_PTR;

/* references to names in env_common.c */
extern uchar default_environment[];
extern int default_environment_size;

extern char * env_name_spec ;
extern env_t *env_ptr;

inline int sd_env_init(void) 
{ 
	gd->env_addr = (ulong)&default_environment[0];
	gd->env_valid = 1;
    env_name_spec = "MMC" ;
	
	return 0 ;
}

int sd_saveenv(void) 
{ 
	u32 saved_size = 0, saved_offset = 0;
	char *saved_buffer = NULL;
	u32 sector = 1;
	int ret;

	if (!env_flash) {
		puts("Environment SPI flash not initialized\n");
		return 1;
	}

	/* Is the sector larger than the env (i.e. embedded) */
	if (config_env_sector_size_spiflash > CONFIG_ENV_SIZE) {
		saved_size = config_env_sector_size_spiflash - CONFIG_ENV_SIZE;
		saved_offset = CONFIG_ENV_OFFSET_MMC_spiflash + CONFIG_ENV_SIZE;
		saved_buffer = malloc(saved_size);
		if (!saved_buffer) {
			ret = 1;
			goto done;
		}
		ret = spi_flash_read(env_flash, saved_offset, saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	if (CONFIG_ENV_SIZE > config_env_sector_size_spiflash) {
		sector = CONFIG_ENV_SIZE / config_env_sector_size_spiflash;
		if (CONFIG_ENV_SIZE % config_env_sector_size_spiflash)
			sector++;
	}

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, CONFIG_ENV_OFFSET_MMC_spiflash, sector * config_env_sector_size_spiflash, 0);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");
	ret = spi_flash_write(env_flash, CONFIG_ENV_OFFSET_MMC_spiflash, CONFIG_ENV_SIZE, env_ptr, 0);
	if (ret)
		goto done;

	if (config_env_sector_size_spiflash > CONFIG_ENV_SIZE) {
		ret = spi_flash_write(env_flash, saved_offset, saved_size, saved_buffer, 0);
		if (ret)
			goto done;
	}

	ret = 0;
	puts("done\n");

 done:
	if (saved_buffer)
		free(saved_buffer);
	return ret; 
}

unsigned char sd_env_get_char_spec(int index) 
{ 
    return *((uchar *)(gd->env_addr + index));
}

void sd_env_relocate_spec(void) 
{
	int ret;

	ret = SD_Read(CONFIG_ENV_OFFSET_MMC_MMC * SD_BLOCK_SIZE, CONFIG_ENV_SIZE, env_ptr) ;
    if (ret != 0)
		goto err_read;

	if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
		goto err_crc;

	gd->env_valid = 1;
    printf("  The environment varialbes stored in MMC are found.\n") ;

	return;

err_read:
	printf("[ERR] SD-Read fails!\n") ;
err_crc:
	puts("*** Warning - bad CRC, using default environment\n\n");

	if (default_environment_size > CONFIG_ENV_SIZE) {
		gd->env_valid = 0;
		puts("*** Error - default environment is too large\n\n");
		return;
	}

	memset(env_ptr, 0, sizeof(env_t));
	memcpy(env_ptr->data, default_environment, default_environment_size);
	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
	gd->env_valid = 1;
}

#endif //#if 0

