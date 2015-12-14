#ifndef __IMGGEN_CMDLINE_H__
#define __IMGGEN_CMDLINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

//#define DEBUG
#ifdef DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif

#define LOADER_MINIMUM_SIZE 0xE00
#define BOOTIMAGE_MINIMUM_SIZE (128 * 1024)

#define BOOTSTORAGE_SD 1
#define BOOTSTORAGE_SF 2
#define BOOTSTORAGE_NF2K 3
#define BOOTSTORAGE_NF4K 4

#define SD_BLOCKSIZE 512
#define SF_BLOCKSIZE (64 * 1024)
#define NF2K_PAGESIZE 2048
#define NF2K_BLOCKSIZE (128 * 1024)

#define UBOOTSIZE_INDEX 0x80

#define DEFAULT_CONFIG_FILEPATH_SD "SD/Mozart_SD.config"
#define DEFAULT_LOADER_FILEPATH_SD "SD/SD_Loader.bin"
#define DEFAULT_UBOOT_FILEPATH_SD "SD/SD_Uboot.bin"
#define DEFAULT_OUTPUT_FILEPATH_SD "SD/SD_Output.bin"

#define DEFAULT_CONFIG_FILEPATH_SF "SF/Mozart_SF.config"
#define DEFAULT_LOADER_FILEPATH_SF "SF/SF_Loader.bin"
#define DEFAULT_UBOOT_FILEPATH_SF "SF/SF_Uboot.bin"
#define DEFAULT_OUTPUT_FILEPATH_SF "SF/SF_Output.bin"

#define DEFAULT_CONFIG_FILEPATH_NF2K "NF2K/Mozart_NF2K.config"
#define DEFAULT_PRELOADER_FILEPATH_NF2K "NF2K/NF2K_Preloader.bin"
#define DEFAULT_LOADER_FILEPATH_NF2K "NF2K/NF2K_Loader.bin"
#define DEFAULT_UBOOT_FILEPATH_NF2K "NF2K/NF2K_Uboot.bin"
#define DEFAULT_OUTPUT_FILEPATH_NF2K "NF2K/NF2K_Output.bin"

#define MAGICNUMBER1 0x82451282
#define MAGICNUMBER2 0x28791166

static int sf_bootimage_start_block_number = 0 ;

struct genearl_config_struct {
	unsigned long addr ;
	unsigned long val ;
} ;

void print_usage(void)
{
	fprintf(stderr,
		"\nImage Generator Usage :\n"
		" -h                 Display the help information.\n"
		" -v                 Show version number\n"
		" -i                 Use interactive text-based GUI menu.\n"
		"                    It will run text-based GUI and ignore other options in the command line.\n"
		" -t [BOOT-TYPE]     Assign the type of boot storage.\n"
		"                    You can only use one of \"sf\", \"sd\", or \"nf2k\".\n"
		"                    This is the \"NECESSARY\" option.\n"
		" -c [FILE]          Assign the configuration file(if not assigned, use default filename).\n"
		" -l [FILE]          Assign the loader file(if not assigned, use default filename).\n"
		" -p [FILE]          Assign the preloader file(only for -t nf2k).\n"
		" -u [FILE]          Assign the uboot file(if not assigned, use default filename).\n"
		" -o [FILE]          Assign the output file(i.e. boot image)(if not assigned, use default filename).\n"
		" -s [NUMBER]        Assign the \"START\" SF-block number. The SF boot image can start from any\n"
		"                    block, but SD/NF2K only can start from 1st block.\n"
		"\nEx : Generate a SD bootimage..\n"
		"   $>./imagegenerator -t sd -c sd_config -l loader.bin -u uboot.bin -o sd_bootimage.bin\n\n"
		"If you do not add the option \"c\", or \"l\", or \"u\", or \"o\", or \"s\"(if SF), \n"
		"Image Generator will generate the image based on default settings.\n"
		"But you have to add the option \"t\" at least.\n"
		"\nEx : Generate a default SD bootimage..\n"
		"   $>./imagegenerator -t sd\n\n");
}

int generate_bootimg_sd( FILE* config_file, FILE* loader_file, FILE* uboot_file, FILE* output_file)
{
	int ret = 0 ;
	char dummy_str[256] ;
	int dummy_int ;
	int i ;
	unsigned char tmp ;

	unsigned long magicnumber1_index, magicnumber1_val ;
	unsigned long magicnumber2_index, magicnumber2_val ;
	unsigned long loaderaddr_index, loaderaddr_val ;
	unsigned long loadersize_index, loadersize_val ;
	unsigned long ubootsize_index, ubootsize_val, ubootaddr_val ;

	unsigned char* bootimage_databuf ;
	int bootimage_size ;
	unsigned long* bootimage_bufptr ;

	// 1. Retrieve info from config_file
	fscanf(config_file,"%s %lx %d", dummy_str, &magicnumber1_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &magicnumber1_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &magicnumber2_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &magicnumber2_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &loaderaddr_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &loaderaddr_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &loadersize_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &loadersize_val);

	// Get the real size of the loader
	fseek(loader_file,0,SEEK_END);
	loadersize_val = ftell(loader_file) ;
	rewind(loader_file) ;

	ubootsize_index = UBOOTSIZE_INDEX ;
	// Get the real size of the uboot
	fseek(uboot_file,0,SEEK_END);
	ubootsize_val = ftell(uboot_file) ;
	rewind(uboot_file) ;
	//adjust uboot size(block aligned)
	if((ubootsize_val % SD_BLOCKSIZE) != 0) {
		ubootsize_val /= SD_BLOCKSIZE ;
		ubootsize_val++ ;
		ubootsize_val = ubootsize_val * SD_BLOCKSIZE ;
	}
	ubootaddr_val = 0x1000 ;

	// 2. Generate the boot image.
	// 2-1. Calculate the size of boot image
	bootimage_size = ubootaddr_val+ ubootsize_val ;
	bootimage_size = (bootimage_size < BOOTIMAGE_MINIMUM_SIZE) ? BOOTIMAGE_MINIMUM_SIZE : bootimage_size ;

	// 2-2. Allocate bootimage data buf
	bootimage_databuf = (unsigned char*)malloc(bootimage_size * sizeof(unsigned char)) ;
	if(bootimage_databuf == NULL) {
		fprintf(stderr, "[ERR] There is no enough memory space to allocate buf..\n") ;
		ret = -1 ;
		goto _sd_exit ;
	}

	// 2-3. Write data to data buf
	// - Write magic#
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + magicnumber1_index) ;
	*(bootimage_bufptr) = magicnumber1_val ;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + magicnumber2_index) ;
	*(bootimage_bufptr) = magicnumber2_val ;

	// - Write loader addr
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + loaderaddr_index) ;
	*(bootimage_bufptr) = loaderaddr_val ;

	// - Write loader size
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + loadersize_index) ;
	*(bootimage_bufptr) = loadersize_val ;

	// - Write loader
	i = loaderaddr_val ;
	tmp = fgetc(loader_file) ;
	while(!feof(loader_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(loader_file) ;
		i++ ;
	}

	// - Write Uboot Size
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + ubootsize_index) ;
	*(bootimage_bufptr) = ubootsize_val ;

	// - Write Uboot
	i = ubootaddr_val ;
	tmp = fgetc(uboot_file) ;
	while(!feof(uboot_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(uboot_file) ;
		i++ ;
	}

	// 2-4. Write data buf to file.
	for(i = 0 ; i < bootimage_size ; i++) {
		fputc( bootimage_databuf[i], output_file) ;
	}

	free(bootimage_databuf) ;

	printf("\nGenerating the boot image..\n") ;

_sd_exit :
	return ret ;
}

int generate_bootimg_sf( FILE* config_file, FILE* loader_file, FILE* uboot_file, FILE* output_file)
{
	int ret = 0 ;
	char dummy_str[256] ;
	int dummy_int ;
	int i ;
	unsigned char tmp ;

	unsigned long magicnumber1_index, magicnumber1_val ;
	unsigned long magicnumber2_index, magicnumber2_val ;
	unsigned long loaderaddr_index, loaderaddr_val ;
	unsigned long loadersize_index, loadersize_val ;
	unsigned long ubootsize_index, ubootsize_val, ubootaddr_val ;
	unsigned long generalconfig_index, generalconfig_number ;
	struct genearl_config_struct* generalconfig_buf ;

	unsigned char* bootimage_databuf ;
	int bootimage_size ;
	unsigned long* bootimage_bufptr ;

	// 1. Retrieve info from config_file
	fscanf(config_file,"%s %lx %d", dummy_str, &magicnumber1_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &magicnumber1_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &magicnumber2_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &magicnumber2_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &loaderaddr_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &loaderaddr_val);

	fscanf(config_file,"%s %lx %ld", dummy_str, &generalconfig_index, &generalconfig_number);
	generalconfig_buf = (struct genearl_config_struct*)malloc(generalconfig_number * sizeof(struct genearl_config_struct)) ;
	for(i = 0 ; i < generalconfig_number ; i++) {
		fscanf(config_file,"%s %lx %lx", dummy_str, &generalconfig_buf[i].addr, &generalconfig_buf[i].val);
	}

	fscanf(config_file,"%s %lx %d", dummy_str, &loadersize_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &loadersize_val);

	// Get the real size of the loader
	fseek(loader_file,0,SEEK_END);
	loadersize_val = ftell(loader_file) ;
	rewind(loader_file) ;

	ubootsize_index = UBOOTSIZE_INDEX ;
	// Get the real size of the uboot
	fseek(uboot_file,0,SEEK_END);
	ubootsize_val = ftell(uboot_file) ;
	rewind(uboot_file) ;
	// Adjust it to 3840 bytes aligned(DMA max movable size).
	if((ubootsize_val % 3840) != 0) {
		ubootsize_val /= 3840 ;
		ubootsize_val++ ;
		ubootsize_val = ubootsize_val * 3840 ;
	}
	ubootaddr_val = 0x1000 ;

	// 2. Generate the boot image.
	// 2-1. Calculate the size of boot image
	bootimage_size = ubootaddr_val + ubootsize_val ;
	bootimage_size = (bootimage_size < BOOTIMAGE_MINIMUM_SIZE) ? BOOTIMAGE_MINIMUM_SIZE : bootimage_size ;

	// 2-2. Allocate bootimage data buf
	bootimage_databuf = (unsigned char*)malloc(bootimage_size * sizeof(unsigned char)) ;
	if(bootimage_databuf == NULL) {
		fprintf(stderr, "[ERR] There is no enough memory space to allocate buf..\n") ;
		ret = -1 ;
		goto _sf_exit ;
	}

	// 2-3. Write data to data buf
	// - Write magic#
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + magicnumber1_index) ;
	*(bootimage_bufptr) = magicnumber1_val ;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + magicnumber2_index) ;
	*(bootimage_bufptr) = magicnumber2_val ;

	// - Write headers
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + 0x08) ;
	*(bootimage_bufptr) = loaderaddr_index + (sf_bootimage_start_block_number * SF_BLOCKSIZE) ;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + 0x0C) ;
	*(bootimage_bufptr) = generalconfig_index + (sf_bootimage_start_block_number * SF_BLOCKSIZE) ;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + 0x10) ;
	*(bootimage_bufptr) = loadersize_index + (sf_bootimage_start_block_number * SF_BLOCKSIZE) ;

	// - Write loader addr
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + loaderaddr_index) ;
	*(bootimage_bufptr) = loaderaddr_val + (sf_bootimage_start_block_number * SF_BLOCKSIZE) ;

	// - Write loader size
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + loadersize_index) ;
	*(bootimage_bufptr) = loadersize_val ;

	// - Write general config
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + generalconfig_index) ;
	for(i = 0 ; i < generalconfig_number ; i++) {
		*(bootimage_bufptr) = generalconfig_buf[i].addr ;
		bootimage_bufptr++ ;
		*(bootimage_bufptr) = generalconfig_buf[i].val ;
		bootimage_bufptr++ ;
	}

	// - Write loader
	i = loaderaddr_val ;
	tmp = fgetc(loader_file) ;
	while(!feof(loader_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(loader_file) ;
		i++ ;
	}

	// - Update Uboot address
	//   Because the last word of loader image is the address of UBoot, we should update it if necessary.
	//   Get real size of loader.
	fseek(loader_file,0,SEEK_END);
	loadersize_val = ftell(loader_file) ;
	rewind(loader_file) ;
	//   Get its last word in the bootimage_databuf.
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + loaderaddr_val + loadersize_val) ;
	bootimage_bufptr-- ;
	*bootimage_bufptr = *(bootimage_bufptr) + (sf_bootimage_start_block_number * SF_BLOCKSIZE) ;

	// - Write Uboot Size
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + ubootsize_index) ;
	*(bootimage_bufptr) = ubootsize_val ;

	// - Write Uboot
	i = ubootaddr_val ;
	tmp = fgetc(uboot_file) ;
	while(!feof(uboot_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(uboot_file) ;
		i++ ;
	}

	// 2-4. Write data buf to file.
	for(i = 0 ; i < bootimage_size ; i++) {
		fputc( bootimage_databuf[i], output_file) ;
	}

	free(bootimage_databuf) ;

	printf("\nGenerating the boot image..\n") ;

_sf_exit :
	return ret ;
}

int generate_bootimg_nf2k( FILE* config_file, FILE* loader_file, FILE* preloader_file, FILE* uboot_file, FILE* output_file)
{
	int ret = 0 ;
	char dummy_str[256] ;
	int dummy_int ;
	int i ;
	unsigned char tmp ;


	unsigned long magicnumber1_index, magicnumber1_val ;
	unsigned long magicnumber2_index, magicnumber2_val ;
	unsigned long preloaderaddr_index, preloaderaddr_val ;
	unsigned long loaderaddr_val ;
	unsigned long ubootsize_index, ubootsize_val;
	unsigned long ubootaddr_val ;
	unsigned long generalconfig_index, generalconfig_number ;
	struct genearl_config_struct* generalconfig_buf ;

	unsigned char* bootimage_databuf ;
	unsigned char* bootimage_datacopy ;
	int bootimage_size ;
	unsigned long* bootimage_bufptr ;

	// 1. Retrieve info from config_file
	fscanf(config_file,"%s %lx %d", dummy_str, &magicnumber1_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &magicnumber1_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &magicnumber2_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &magicnumber2_val);

	fscanf(config_file,"%s %lx %d", dummy_str, &preloaderaddr_index, &dummy_int);
	fscanf(config_file,"%s %s %lx", dummy_str, dummy_str, &loaderaddr_val);

	fscanf(config_file,"%s %lx %ld", dummy_str, &generalconfig_index, &generalconfig_number);
	generalconfig_buf = (struct genearl_config_struct*)malloc(generalconfig_number * sizeof(struct genearl_config_struct)) ;
	for(i = 0 ; i < generalconfig_number ; i++) {
		fscanf(config_file,"%s %lx %lx", dummy_str, &generalconfig_buf[i].addr, &generalconfig_buf[i].val);
	}

	//assign preloader/uboot address.
	preloaderaddr_val = 0xA0 ;
	ubootsize_index = UBOOTSIZE_INDEX ;
	ubootaddr_val = 0x1000 ;
	fseek(uboot_file,0,SEEK_END);
	ubootsize_val = ftell(uboot_file) ;
	rewind(uboot_file) ;
	//ubootsize_val is PAGE-aligned.
	if((ubootsize_val % NF2K_PAGESIZE) != 0) {
		ubootsize_val /= NF2K_PAGESIZE ;
		ubootsize_val++ ;
	}
	else
		ubootsize_val /= NF2K_PAGESIZE ;

	// 2. Generate the boot image.
	// 2-1. Calculate the size of boot image
	bootimage_size = ubootaddr_val + (ubootsize_val * NF2K_PAGESIZE) ;
	//      We should also add N pages aimed to store Magic Number.
	//      NF Loader will skip bad blocks by checking the Magic Number.
	//      "N" is the variable "bootimage_use_block_num".
	unsigned long bootimage_use_block_num = (bootimage_size % NF2K_BLOCKSIZE != 0) ? \
											(bootimage_size / NF2K_BLOCKSIZE) + 1 : \
											(bootimage_size / NF2K_BLOCKSIZE) ;
	bootimage_size += bootimage_use_block_num * NF2K_PAGESIZE ;
	bootimage_size = (bootimage_size < BOOTIMAGE_MINIMUM_SIZE) ? BOOTIMAGE_MINIMUM_SIZE : bootimage_size ;
	//bootimage_size is BLOCK-aligned.
	if((bootimage_size % NF2K_BLOCKSIZE) != 0) {
		bootimage_size /= NF2K_BLOCKSIZE ;
		bootimage_size++ ;
		bootimage_size *= NF2K_BLOCKSIZE ;
	}

	// 2-2. Allocate bootimage data buf
	bootimage_databuf = (unsigned char*)malloc(bootimage_size * sizeof(unsigned char)) ;
	if(bootimage_databuf == NULL) {
		fprintf(stderr, "[ERR] There is no enough memory space to allocate buf..\n") ;
		ret = -1 ;
		goto _nf2k_exit ;
	}
	memset(bootimage_databuf, 0, bootimage_size * sizeof(unsigned char)) ;

	bootimage_datacopy= (unsigned char*)malloc(bootimage_size * sizeof(unsigned char)) ;
	if(bootimage_datacopy == NULL) {
		fprintf(stderr, "[ERR] There is no enough memory space to allocate buf..\n") ;
		ret = -1 ;
		goto _nf2k_exit ;
	}
	memset(bootimage_datacopy, 0, bootimage_size * sizeof(unsigned char)) ;

	// 2-3. Write data to data buf
	// - Write magic#
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + magicnumber1_index) ;
	*(bootimage_bufptr) = magicnumber1_val ;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + magicnumber2_index) ;
	*(bootimage_bufptr) = magicnumber2_val ;

	// - Write headers
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + 0x08) ;
	*(bootimage_bufptr) = preloaderaddr_index ;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + 0x0C) ;
	*(bootimage_bufptr) = generalconfig_index;
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + preloaderaddr_index) ;
	*(bootimage_bufptr) = preloaderaddr_val ;

	bootimage_bufptr = (unsigned long*)(bootimage_databuf + generalconfig_index) ;
	for(i = 0 ; i < generalconfig_number ; i++) {
		*(bootimage_bufptr) = generalconfig_buf[i].addr ;
		bootimage_bufptr++ ;
		*(bootimage_bufptr) = generalconfig_buf[i].val ;
		bootimage_bufptr++ ;
	}

	// - Write preloader
	i = preloaderaddr_val ;
	tmp = fgetc(preloader_file) ;
	while(!feof(preloader_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(preloader_file) ;
		i++ ;
	}

	// - Write loader
	i = loaderaddr_val ;
	tmp = fgetc(loader_file) ;
	while(!feof(loader_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(loader_file) ;
		i++ ;
	}

	// - Write Uboot Size
	bootimage_bufptr = (unsigned long*)(bootimage_databuf + ubootsize_index) ;
	*(bootimage_bufptr) = ubootsize_val ;

	// - Write Uboot
	i = ubootaddr_val ;
	tmp = fgetc(uboot_file) ;
	while(!feof(uboot_file)) {
		bootimage_databuf[i] = tmp ;
		tmp = fgetc(uboot_file) ;
		i++ ;
	}

	// 2-4. Copy the data from bootimage_databuf to bootimage_datacopy.
	//      We will add Magic Number to the last page(2048 Bytes) of each block(128K Bytes).
	int lastpage_startaddr = ((NF2K_BLOCKSIZE / NF2K_PAGESIZE) - 1) * NF2K_PAGESIZE ;
	int databuf_index = 0 ;
	int datacopy_index = 0 ;
	while(datacopy_index < bootimage_size) {
		if((datacopy_index % NF2K_BLOCKSIZE) != lastpage_startaddr) {
			if(databuf_index < (ubootaddr_val + (ubootsize_val * NF2K_PAGESIZE))) {
				bootimage_datacopy[datacopy_index] = bootimage_databuf[databuf_index] ;
				databuf_index++ ;
			}
			datacopy_index++ ;
		}
		else {
			//write magic#
			bootimage_bufptr = (unsigned long*)&bootimage_datacopy[datacopy_index] ;
			*bootimage_bufptr = MAGICNUMBER1 ;
			bootimage_bufptr++ ;
			*bootimage_bufptr = MAGICNUMBER2 ;
			datacopy_index += NF2K_PAGESIZE ;
		}
	}

	// 2-5. Write data buf to file.
	for(i = 0 ; i < bootimage_size ; i++) {
		fputc( bootimage_datacopy[i], output_file) ;
	}

	free(bootimage_databuf) ;
	free(bootimage_datacopy) ;
	fclose(preloader_file) ;

	printf("\nGenerating the boot image..\n") ;

_nf2k_exit :
	return ret ;
}

#endif //end __IMGGEN_CMDLINE_H__

