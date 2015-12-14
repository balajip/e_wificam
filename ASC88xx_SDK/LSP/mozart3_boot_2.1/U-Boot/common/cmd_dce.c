/*
 * Command for DCE processing.
 *
 * Copyright (C) 2013 VN Inc
 */
#include <common.h>
#include <malloc.h>
#include <asm/io.h>

#include <asm/arch/platform.h>

static int do_DCE_encrypt_decrypt(int argc, char *argv[])
{
	unsigned long addr;
	unsigned long offset;
	unsigned long len;
	char *endp;
	int ret;
	int time = 0;


	if (argc < 4)
		goto usage;

	addr = simple_strtoul(argv[1], &endp, 16);		//dest
	if (*argv[1] == 0 || *endp != 0)
		goto usage;
		
	offset = simple_strtoul(argv[2], &endp, 16);	//src
	if (*argv[2] == 0 || *endp != 0)
		goto usage;
		
	len = simple_strtoul(argv[3], &endp, 16);
	if (*argv[3] == 0 || *endp != 0)
		goto usage;
	if (len & 0x7F) {
		puts("Length is not 128 bytes aligned\n");
		return 1;
	}

	time = get_timer(0);
	printf("start: %d\n", time );
	
	if (strcmp(argv[0], "enc") == 0)
		ret = DCE_OperationStart( offset, len, addr, 0);
	else
		ret = DCE_OperationStart( offset, len, addr, OPMODE_DEC);

	if (ret) {
		printf(" VMA DCE %s failed\n", argv[0]);
		return 1;
	}
	
	time = get_timer(time);
	printf("end: %d\n", time);
	
	return 0;

usage:
	printf("Usage: dce %s addr offset len\n", argv[0]);
	return 1;
}

static int do_dce_operation(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;
	unsigned int	reg;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;
	
	DCE_init();

	cmd = argv[1];

	if (strcmp(cmd, "enc") == 0 || strcmp(cmd, "dec") == 0)
		return do_DCE_encrypt_decrypt(argc - 1, argv + 1);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	dce,	5,	1,	do_dce_operation,
	"DCE",
	"dce enc addr offset len 	- encrypt `len' bytes starting at\n"
	"				  `PHYSICAL offset' to `PHYSICAL addr'\n"	
	"dce dec addr offset len	- deccrypt `len' bytes from memory\n"
	"				  at `PHYSICAL offset' to `PHYSICAL addr'\n"
);

