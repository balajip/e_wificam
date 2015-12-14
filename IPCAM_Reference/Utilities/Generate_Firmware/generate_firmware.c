/*
Copyright 2011 VN, Inc.  All rights reserved.
Author: Anil Kulkarni
Description: This application is used to generate a single firmware file out of serial flash loader, 
kernel Image and Kilrogg Application binary. The generated file is used on board to update the firmware. 
*/

#include <stdio.h>
#include <stdlib.h>

void write_binary(char s[100], int fout_offset);

FILE *fin, *fout;
int main(int argc, char* argv[])
{
	unsigned int currpos;
	int offset = 0;	
	system("rm -f Output_firmware");
	if (argc != 4)
        {
                printf("Usage: ./generate_firmware SF_bootloader_file Kernel_file  Kilrogg_binary_file\n");
                exit(1);
        }	
	if ((fout=fopen("Output_firmware","ab")) == NULL)
	{
		printf("Input file can't be opened!!!\n");
	}
	
	// Write SF_Output.bin
	write_binary(argv[1], 0);
	// Write zimage
	currpos=ftell(fout);
	write_binary(argv[2], currpos);
	// Write rootfs
	currpos=ftell(fout);
	write_binary(argv[3], currpos);
	fclose(fout);
	return 0;	
}

void write_binary(char s[100], int fout_offset)
{
	char *buffer;
	unsigned int fileLen;
	fin=fopen(s,"rb");
	if ((fin=fopen(s,"rb")) == NULL)
	{
		printf("Input File %s can't be opened!!!\n", s);
	}
	
	//Get file length
	fseek(fin, 0, SEEK_END);
	fileLen=ftell(fin);
	fseek(fin, 0, SEEK_SET);
	fwrite(&fileLen,1,sizeof(fileLen),fout);
	fseek(fout, fout_offset, 4);

	//Allocate memory
	buffer=(char *)malloc(fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(fin);
		return;
	}

	//Read file contents into buffer
	fread(buffer, fileLen, 1, fin);
	fclose(fin);
	fwrite(buffer,1, fileLen, fout);
	
}
