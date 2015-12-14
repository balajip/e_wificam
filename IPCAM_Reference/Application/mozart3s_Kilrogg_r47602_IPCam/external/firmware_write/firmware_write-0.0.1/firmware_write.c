/*
Copyright 2011 VN, Inc.  All rights reserved.
Author: Anil Kulkarni
Description: This application is used to generate a serial flash loader, kernel Image and Kilrogg Application binary. 
from the single firmware file. 
The current implementation is based on the SDK memory map & paritions. The size of single firmware file is approx ~12MB.Hence in ASC8850 SDK on /var/ we wil have more than ~24MB. Hence this portion is used to extract the firmware contents.
*/

/* Below is partition refernce used to update firmware. The partion needs to be updated depending on the design
***********SPI*****************
0x00000000-0x00020000 : "bootldr"=>/dev/mtd0
0x00020000-0x00030000 : "environment variables"=>/dev/mtd0 
0x00030000-0x00200000 : "kernel"=>/dev/mtd1
0x00200000-0x00a00000 : "rootfs"=>/dev/mtd2
0x00a00000-0x01000000 : "jffs2"=>/dev/mtd3
***********NAND****************
0x00000000-0x00040000 : "nand-bootldr"=>/dev/mtd4
0x00040000-0x00080000 : "nand-env"=>/dev/mtd5
0x00080000-0x00480000 : "nand-kernel"=>/dev/mtd6
0x00480000-0x01480000 : "nand-rootfs"=>/dev/mtd7
0x01480000-0x07480000 : "nand-ubifs"=>/dev/mtd8
*/
#include <stdio.h>
#include <stdlib.h>
#define SPI_FIRMWARE 1
void create_file(char s[100], int fout_offset);

FILE *fin, *fout;
unsigned int *sf_size, *kernl_size, *rootfs_size;

int main(int argc, char* argv[])
{
	
	char *readfile;
	char execute[200]; 
	int offset = 0;	
//	system ("stopallproc"); //stop all the running processes
	sleep (5); //wait till venc stops
	/*Extracting the bootloader, kernel image and application from the single firmware file*/

	if ((fin=fopen("/var/Output_firmware","rb")) == NULL)
	{
		printf("Input firmware file Output_firmware can't be opened!!!\n");
	}
	
	//Read size of Serial flash bootloader
	fread(&sf_size, sizeof(unsigned int), 1, fin);
	//Create Serial flash bootloader
	create_file("/var/bootloader_image.bin", sf_size);
	sleep(1);
	//update the  bootloader on SPI
#ifdef SPI_FIRMWARE
		sprintf(execute, "mtd_debug erase /dev/mtd0 0 0x%x", sf_size); //erase the /dev/mtd0-> serial flash bootloader
		system(execute);
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd0 0 0x%x /var/bootloader_image.bin", sf_size);  //write new serial flash loader into /dev/mtd0
		system(execute);	
		system("sync");
		sleep (1);
#else 
		//update the  bootloader on NF
		sprintf(execute, "mtd_debug erase /dev/mtd4 0 0x%x", sf_size); //erase the /dev/mtd0-> serial flash bootloader
		system(execute);
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd4 0 0x%x /var/bootloader_image.bin", sf_size);  //write new serial flash loader into /dev/mtd4
		system(execute);	
		system("sync");
		sleep (1);
#endif
	system("rm /var/bootloader_image.bin");

	//Read size of Kernel Image flash on SPI
	fread(&kernl_size, sizeof(unsigned int), 1, fin);
	//Create Kernel image
	create_file("/var/kernel_image", kernl_size);
	sleep(1);
	//Update the kernel image
#ifdef SPI_FIRMWARE
		//update Kernel with the latest
		system("flash_eraseall /dev/mtd1");	//erase the /dev/mtd0-> serial flash bootloader
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd1 0 0x%x /var/kernel_image", kernl_size);  //write new Kernel Image into /dev/mtd1
		system(execute);	
		system("sync");
		sleep (1);
#else 
		//update Kernel with the latest on NF
		system("flash_eraseall /dev/mtd6");	//erase the /dev/mtd0-> serial flash bootloader
		sleep (1);
		sprintf(execute, "nandwrite -p /dev/mtd6 /var/kernel_image");  //write new Kernel Image into /dev/mtd6
		system(execute);	
		system("sync");
		sleep (1);
#endif
	system("rm /var/kernel_image");
	//Read size of rootfs Image 
	fread(&rootfs_size, sizeof(unsigned int), 1, fin);
	//Create application rootfs image
	create_file("/var/application_rootfs.sqfs", rootfs_size);
	sleep(1);
#ifdef SPI_FIRMWARE
		//update rootfs with the latest on SPI
		system("umount /etc/");	//erase the /dev/mtd0-> serial flash bootloader
		sleep (1);
		system("rm -fr /mnt/flash/etc/*");
		sleep (1);
		system("flash_eraseall /dev/mtd2");
		sleep (1);
		system("flash_eraseall /dev/mtd3");
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd2 0 0x%x /var/application_rootfs.sqfs", rootfs_size);  //write rootfs.sqfs into /dev/mtd2
		system(execute);	
		system("sync");
		sleep (1);
#else 
		//update rootfs with the latest on NF
		system("flash_eraseall /dev/mtd7");
		sleep (1);
		system("flash_eraseall /dev/mtd8");
		sleep (1);
		sprintf(execute, "nandwrite -p /dev/mtd7 /var/application_rootfs.sqfs"); //write rootfs.sqfs into /dev/mtd7
		system(execute);	
		system("sync");
		sleep (1);
#endif
	system("rm /var/application_rootfs.sqfs");
	fclose(fin);

/*	Updating the firmware on SF/NF starts from here. The below partition sizes, partition numbers are based on ASC8850 defualt parition. 
	The below partitions, sizes needs to be updated by user if there design is different */
/*
#ifdef SPI_FIRMWARE
		sprintf(execute, "mtd_debug erase /dev/mtd0 0 0x%x", sf_size); //erase the /dev/mtd0-> serial flash bootloader
		system(execute);
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd0 0 0x%x /var/bootloader_image.bin", sf_size);  //write new serial flash loader into /dev/mtd0
		system(execute);	
		system("sync");
		sleep (1);

		//update Kernel with the latest
		system("flash_eraseall /dev/mtd1");	//erase the /dev/mtd0-> serial flash bootloader
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd1 0 0x%x /var/kernel_image", kernl_size);  //write new Kernel Image into /dev/mtd1
		system(execute);	
		system("sync");
		sleep (1);
	
		//update rootfs with the latest
		system("umount /etc/");	//erase the /dev/mtd0-> serial flash bootloader
		sleep (1);
		system("rm -fr /mnt/flash/etc/*");
		sleep (1);
		system("flash_eraseall /dev/mtd2");
		sleep (1);
		system("flash_eraseall /dev/mtd3");
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd2 0 0x%x /var/application_rootfs.sqfs", rootfs_size);  //write rootfs.sqfs into /dev/mtd2
		system(execute);	
		system("sync");
		sleep (1);
#else 
		//Need to add stuff related to NF firmware update
		sprintf(execute, "mtd_debug erase /dev/mtd4 0 0x%x", sf_size); //erase the /dev/mtd0-> serial flash bootloader
		system(execute);
		sleep (1);
		sprintf(execute, "mtd_debug write /dev/mtd4 0 0x%x /var/bootloader_image.bin", sf_size);  //write new serial flash loader into /dev/mtd4
		system(execute);	
		system("sync");
		sleep (1);
		
		//update Kernel with the latest
		system("flash_eraseall /dev/mtd6");	//erase the /dev/mtd0-> serial flash bootloader
		sleep (1);
		sprintf(execute, "nandwrite -p /dev/mtd6 /var/kernel_image");  //write new Kernel Image into /dev/mtd6
		system(execute);	
		system("sync");
		sleep (1);

		//update rootfs with the latest
		system("flash_eraseall /dev/mtd7");
		sleep (1);
		system("flash_eraseall /dev/mtd8");
		sleep (1);
		sprintf(execute, "nandwrite -p /dev/mtd7 /var/application_rootfs.sqfs"); //write rootfs.sqfs into /dev/mtd7
		system(execute);	
		system("sync");
		sleep (1);
#endif*/
	return 0;	
}
void create_file (char s[100], int size)
{
	char *buffer;
	unsigned int fileLen;
	if ((fout=fopen(s, "wb")) == NULL)
	{
		printf("Output File can't be opened!!!\n");
	}
	buffer=(char *)malloc(size+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(fout);
		return;
	}
	fread(buffer, size, 1, fin);
	fwrite(buffer,1, size, fout);
	fclose(fout);
	free(buffer);

}
