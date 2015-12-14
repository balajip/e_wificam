/*
 * Copyright 2000-2005 VN Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <command.h>
#include <asm/setup.h>

DECLARE_GLOBAL_DATA_PTR;

#define tag_size(type)  ((sizeof(struct tag_header) + sizeof(struct type)) >> 2)
#define tag_next(t)     ((struct tag *)((u32 *)(t) + (t)->hdr.size))

extern void setup_start_tag(bd_t *bd);
extern void setup_memory_tags(bd_t *bd);
extern void setup_commandline_tag(bd_t *bd, char *commandline);
extern void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end);
extern void setup_end_tag(bd_t *bd);

extern image_header_t header;           /* from cmd_bootm.c */

#undef DEBUG

/*void boot_linux(cmd_tbl_t *cmdtp,
                bd_t *bd, int flag,
                int argc, char *argv[],
                ulong addr,
                ulong *len_ptr,
                int   verify)
{
    ulong len = 0, checksum;
    ulong initrd_start, initrd_end;
    ulong data;
    char *commandline = getenv("bootargs");
    void (*theKernel)(int zero, int arch);
    image_header_t *hdr = &header;

	u32 ih_ep_save;
	ih_ep_save = SWAP32(hdr->ih_ep);
//	printf("ih_ep_save: %08lx\n",ih_ep_save);
//	printf("commandline: %s\n",commandline);	// aren 20040614

    // Check if there is an initrd image
    if (argc >= 3) 
    {
        addr = simple_strtoul(argv[2], NULL, 16);
//		printf("argv[2]: %s\n",argv[2]);	// aren 20040614

        printf ("## Loading Ramdisk Image at %08lx ...\n", addr);
        
        // Copy header so we can blank CRC field for re-calculation 
        memcpy (&header, (char *)addr, sizeof(image_header_t));
        
        if (SWAP32(hdr->ih_magic) != IH_MAGIC) 
        {
            printf ("Bad Magic Number\n");
            do_reset (cmdtp, bd, flag, argc, argv);
        }
        
        data = (ulong)&header;
        len  = sizeof(image_header_t);
        
        checksum = SWAP32(hdr->ih_hcrc);
        hdr->ih_hcrc = 0;
        
        if (crc32 (0, (char *)data, len) != checksum) 
        {
            printf ("Bad Header Checksum\n");
            do_reset (cmdtp, bd, flag, argc, argv);
        }
        
        print_image_hdr (hdr);
        
        data = addr + sizeof(image_header_t);
        len  = SWAP32(hdr->ih_size);
        
        if (verify) {
            ulong csum = 0;

            printf ("   Verifying Checksum ... ");
            csum = crc32 (0, (char *)data, len);
            if (csum != SWAP32(hdr->ih_dcrc)) {
                printf ("Bad Data CRC\n");
                do_reset (cmdtp, bd, flag, argc, argv);
            }
            printf ("OK\n");
        }
        
        if ((hdr->ih_os   != IH_OS_LINUX)       ||
            (hdr->ih_arch != IH_CPU_ARM)        ||
            (hdr->ih_type != IH_TYPE_RAMDISK)   ) {
            printf ("No Linux ARM Ramdisk Image\n");
            do_reset (cmdtp, bd, flag, argc, argv);
        }
        
        //Now check if we have a multifile image
    } else if ((hdr->ih_type==IH_TYPE_MULTI) && (len_ptr[1])) {
        ulong tail    = SWAP32(len_ptr[0]) % 4;
        int i;
        
        // skip kernel length and terminator 
        data = (ulong)(&len_ptr[2]);
        // skip any additional image length fields 
        for (i=1; len_ptr[i]; ++i)
          data += 4;
        // add kernel length, and align
        data += SWAP32(len_ptr[0]);
        if (tail) {
            data += 4 - tail;
        }
        
        len   = SWAP32(len_ptr[1]);
        
    } else {
         // no initrd image
        data = 0;
    }
    
#ifdef  DEBUG
    if (!data) {
        printf ("No initrd\n");
    }
#endif
    
    if (data) {
//        initrd_start = data;
        initrd_start = SWAP32(hdr->ih_load);	// aren 20040614
        initrd_end   = initrd_start + len;
        printf ("   Loading Ramdisk to %08lx, end %08lx ... ", initrd_start, initrd_end);
//        memmove ((void *)initrd_start, (void *)data, len);
        memcpy ((void *)initrd_start, (void *)data, len);	// aren 20050406
        printf ("OK\n");
    } else {
        initrd_start = 0;
        initrd_end = 0;
    }
    
//    theKernel = (void (*)(int, int))SWAP32(hdr->ih_ep);
//    printf("theKernel: %08lx\n",theKernel);
    theKernel = (void (*)(int, int))ih_ep_save;	// aren 20040614
//    printf("theKernel: %08lx\n",theKernel);

//    theKernel = (void (*)(int, int))SWAP32(hdr->ih_ep);
   
#ifdef DEBUG
    printf ("## Transferring control to Linux (at address %08lx) ...\n",
            (ulong)theKernel);
#endif

    setup_start_tag(bd);
    setup_memory_tags(bd);
    setup_commandline_tag(bd, commandline);
#ifdef not_complete_yet
    setup_initrd_tag(bd, initrd_start, initrd_end);
#endif // end_of_not
#if 0
    setup_ramdisk_tag(bd);
#endif
    setup_end_tag(bd);

    // we assume that the kernel is in place
    printf("\nStarting kernel ...\n\n");

    cleanup_before_linux(bd);

    theKernel(0, bd->bi_arch_number);
}
*/


int do_jump (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    bd_t	*bd = gd->bd;
    //Step 1. Get boot args
    char *commandline = getenv("bootargs");
    ulong addr;
    //Step 2. Create a func-ptr for KERNEL-UNGZIP
    void (*theKernel)(int zero, int arch, int params);

    if (argc < 2) {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    //Step 3. Get the addr of kernel image from the 1st parameter of "jump"
    addr = simple_strtoul(argv[1], NULL, 16);
    //Step 4. Assign the func-ptr
    theKernel = (void (*)(int, int, int))addr;

    printf ("## Booting Linux kernel at 0x%08lx ...\n", addr);


    setup_start_tag(bd);
    setup_memory_tags(bd);
    setup_commandline_tag(bd, commandline);
#ifdef not_complete_yet
    setup_initrd_tag(bd, initrd_start, initrd_end);
#endif /* end_of_not */
    setup_end_tag(bd);

    cleanup_before_linux();

    //Step Final : Execute the kernel.
    theKernel(0, bd->bi_arch_number, bd->bi_boot_params);

    return 0;
}

U_BOOT_CMD(
	jump, 2,	1,	do_jump,
	"Boot Linux kernel image from memory",
	NULL
);

