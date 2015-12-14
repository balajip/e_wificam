/*
 * Command for accessing SPI flash.
 *
 * Copyright (C) 2008 Atmel Corporation
 */
#include <common.h>
#include <spi_flash.h>
#include <malloc.h>
#include <asm/io.h>

#ifndef CONFIG_SF_DEFAULT_SPEED
# define CONFIG_SF_DEFAULT_SPEED	1000000
#endif
#ifndef CONFIG_SF_DEFAULT_MODE
# define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#endif

#define SF_DISPLAY_LENGTH 256 //display 256 bytes in SPI-DISPLAY

static struct spi_flash *flash;



#ifdef CONFIG_SPI_FLASH_VPL
extern struct spi_flash * spi_flash_probe_vpl(struct spi_slave *, u8 *);

int init_spi_flash_array(void)
{
	if (SPI_FLASH_NUMBER == 0)
		return 0;

	flash = spi_flash_probe_vpl(NULL, NULL);
	return 0;
}
#endif

static int do_spi_flash_probe(int argc, char *argv[])
{
	unsigned int bus = 0;
	unsigned int cs;
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
	char *endp;
	struct spi_flash *new;

	if (argc < 2)
		goto usage;

	cs = simple_strtoul(argv[1], &endp, 0);
	if (*argv[1] == 0 || (*endp != 0 && *endp != ':'))
		goto usage;
	if (*endp == ':') {
		if (endp[1] == 0)
			goto usage;

		bus = cs;
		cs = simple_strtoul(endp + 1, &endp, 0);
		if (*endp != 0)
			goto usage;
	}

	if (argc >= 3) {
		speed = simple_strtoul(argv[2], &endp, 0);
		if (*argv[2] == 0 || *endp != 0)
			goto usage;
	}
	if (argc >= 4) {
		mode = simple_strtoul(argv[3], &endp, 16);
		if (*argv[3] == 0 || *endp != 0)
			goto usage;
	}

	new = spi_flash_probe(bus, cs, speed, mode);
	if (!new) {
		printf("Failed to initialize SPI flash at %u:%u\n", bus, cs);
		return 1;
	}

#ifdef CONFIG_SPI_FLASH_VPL
	print_size(new->size, "");
	printf(" %s at %u:%u\n", new->name, bus, cs);
	spi_flash_free(new);
#else
	if (flash)
		spi_flash_free(flash);
	flash = new;

	printf("%u KiB %s at %u:%u is now current device\n",
			flash->size >> 10, flash->name, bus, cs);
#endif

	return 0;

usage:
	puts("Usage: sf probe [bus:]cs [hz] [mode]\n");
	return 1;
}

static int do_spi_flash_read_write(int argc, char *argv[])
{
	unsigned long addr;
	unsigned long offset;
	unsigned long len;
	void *buf;
	char *endp;
	int ret;

	if (argc < 4)
		goto usage;

	addr = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		goto usage;
	offset = simple_strtoul(argv[2], &endp, 16);
	if (*argv[2] == 0 || *endp != 0)
		goto usage;
	len = simple_strtoul(argv[3], &endp, 16);
	if (*argv[3] == 0 || *endp != 0)
		goto usage;

	buf = map_physmem(addr, len, MAP_WRBACK);
	if (!buf) {
		puts("Failed to map physical memory\n");
		return 1;
	}

	if (strcmp(argv[0], "read") == 0)
		ret = spi_flash_read(flash, offset, len, buf);
	else
		ret = spi_flash_write(flash, offset, len, buf, 1);

	unmap_physmem(buf, len);

	if (ret) {
		printf("SPI flash %s failed\n", argv[0]);
		return 1;
	}

	return 0;

usage:
	printf("Usage: sf %s addr offset len\n", argv[0]);
	return 1;
}

static int do_spi_flash_erase(int argc, char *argv[])
{
	unsigned long offset;
	unsigned long len;
	char *endp;
	int ret;

	if (argc < 3)
		goto usage;

	offset = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		goto usage;
	len = simple_strtoul(argv[2], &endp, 16);
	if (*argv[2] == 0 || *endp != 0)
		goto usage;

	ret = spi_flash_erase(flash, offset, len, 1);
	if (ret) {
		printf("SPI flash %s failed\n", argv[0]);
		return 1;
	}

	return 0;

usage:
	puts("Usage: sf erase offset len\n");
	return 1;
}

static int do_spi_flash_display(int argc, char *argv[])
{
	unsigned long offset;
	void *buf;
	char *endp;
	int ret;
    
    buf = NULL ;

	if (argc < 1)
		goto usage;

	offset = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		goto usage;

	buf = mALLOc(SF_DISPLAY_LENGTH) ;
	if (!buf) {
		puts("Failed to allocate buffer to do SPI Display\n");
		return 1;
	}   

	ret = spi_flash_read(flash, offset, SF_DISPLAY_LENGTH, buf);    

	if (ret) {
		printf("SPI flash display failed\n");

        if(buf != NULL)
            fREe(buf) ;
        
		return 1;
	}

    print_buffer( offset, buf, 4, SF_DISPLAY_LENGTH/4, 0) ;

    if(buf != NULL)
        fREe(buf) ;

	return 0;

usage:
	printf("Usage: sf display offset\n");
    
	return 1;
}

static int do_spi_flash(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "probe") == 0)
		return do_spi_flash_probe(argc - 1, argv + 1);

	/* The remaining commands require a selected device */
	if (!flash) {
		puts("No SPI flash selected. Please run `sf probe'\n");
		return 1;
	}

	if (strcmp(cmd, "read") == 0 || strcmp(cmd, "write") == 0)
		return do_spi_flash_read_write(argc - 1, argv + 1);
	if (strcmp(cmd, "erase") == 0)
		return do_spi_flash_erase(argc - 1, argv + 1);
    if (strcmp(cmd, "display") == 0)
        return do_spi_flash_display(argc - 1, argv + 1) ;

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	sf,	5,	1,	do_spi_flash,
	"SPI flash sub-system",
	"probe [bus:]cs [hz] [mode]	- init flash device on given SPI bus\n"
	"				  and chip select\n"
	"sf read addr offset len 	- read `len' bytes starting at\n"
	"				  `PHYSICAL offset' to memory at `PHYSICAL addr'\n"	
	"sf write addr offset len	- write `len' bytes from memory\n"
	"				  at `PHYSICAL addr' to flash at `PHYSICAL offset'\n"
	"sf erase offset len		- erase `len' bytes from `PHYSICAL offset'\n"
	"sf display offset - display 256 bytes at `offset'\n");
