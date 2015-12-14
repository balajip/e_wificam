ReadMe
=======================

The tool generated out of generate_firmwar.c is used to create a single system firmware file. The firmware file can be generated  using various individual
binaries such as bootloader, kernel image and reference application rootfs. 

The above mentioned three files are different for SPI and NAND flash systems. Hence proper care should be taken while generating specific filesystem based firmware file. 

Compilation:
=============
The tool/output exe can be generated on windows or on linux with proper environment for compiling c source file. 

Execution:
==========
Below is the example for building and generating the output firmware file on Linux OS
a. compile source file
	$> gcc generate_firmware.c -o generate_firmware
b. help menu
	$> ./generate_firmware
	     Usage: ./generate_firmware SF_bootloader_file Kernel_file  Kilrogg_binary_file
c. Generate system firmware for SPI 
 e.g: $> ./generat_firmware SF_Output.bin zImage rootfs.sqfs_MT9P031_ASC8850
	An output file Output_Firmware will be generated. This is the system firmware which can be used to updated SPI flash for MT9P031 sensor through web-UI

