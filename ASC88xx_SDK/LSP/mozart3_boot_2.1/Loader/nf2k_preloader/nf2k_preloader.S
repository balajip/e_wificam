/*
 * nf2k_preloader/nf2k_preloader.S
 *
 * Copyright 2011 VN Inc. All rights reserved.
 *
 */

#include <config.h>
#include <loader.h>
#undef VPL_USBC_RAM_BASE
#define VPL_USBC_RAM_BASE   0x80020000
#undef NFCTRL_BASE
#define NFCTRL_BASE   0xA0001300

/***
 * r0: page number of the bootldr image.
 */
	.global _start
_start:
	mov	r1, r0                    @; keep the page number of bootldr
	ldr	sp, =VPL_USBC_RAM_BASE    @; destination address
	ldr	fp, =VPL_BRC_RAM_BASE     @; source address
	ldr	ip, =NAND_PAGE_SIZE       @; size
	bl	Preloader_Copy

	ldr	fp, =VPL_USBC_RAM_BASE
	ldr	ip, =VPL_BRC_RAM_BASE
	sub	fp, fp, ip
	add	fp, fp, pc
	mov	pc, fp

	ldr	r2, =NFCTRL_BASE
	ldr	r5, =0x00000A01
	str	r5, [r2, #FLCTRL]

	ldr	r0, =LOADER_SECOND_PAGENUM
	ldr	r0, [r0]
	add	r0, r0, r1
	bl	Preloader_NAND_Page_Read
	ldr	sp, =(VPL_BRC_RAM_BASE + 0x840)   @; destination address
	ldr	fp, =VPL_BRC_RAM_BASE             @; source address
	ldr	ip, =0x800                        @; size
	bl	Preloader_Copy
	ldr	r0, =LOADER_FIRST_PAGENUM
	ldr	r0, [r0]
	add	r0, r0, r1
	bl	Preloader_NAND_Page_Read
	ldr	sp, =(VPL_BRC_RAM_BASE + 0x800)   @; destination address
	ldr	fp, =(VPL_BRC_RAM_BASE + 0x840)   @; source address
	ldr	ip, =0x800                        @; size
	bl	Preloader_Copy


	ldr	r5, =VPL_BRC_RAM_BASE
	add	r5, r5, #LOADER_BASE
	mov	pc, r5
	nop
	nop
	nop

@;----------------------------------------------------------------------------
@;   Preloader_NAND_Page_Read
@; @ params : r0
@;   r0: row address (page number)
@;
Preloader_NAND_Page_Read:
        @; remap to NFC
	ldr	r3, =VPL_BRC_MMR_BASE
	ldr	r4, =1
	str	r4, [r3, #BRC_CTRL]

        @; NF Memory Busy Check
CTRL_BUSY_CHECK  :
	ldr	r4, [r2, #FLSTATE]
	and	r4, r4, #FLSTATE_FSM_BUSY
	cmp	r4, #FLSTATE_FSM_BUSY
	beq	CTRL_BUSY_CHECK
MEM_BUSY_CHECK    :
	ldr	r4, [r2, #FLSTATE]
	and	r4, r4, #FLSTATE_RNB_ST
	cmp	r4, #FLSTATE_RNB_ST
	bne	MEM_BUSY_CHECK

        @; Address
	mov	r4, r0, LSL#COL_ADDR_BIT
	str	r4, [r2, #FLADDR0L]

        @; Command
	ldr	r4, =NANDFLASH_PAGE_READ
	str	r4, [r2, #FLCOMM]

        @; Wait RnB edge high
WAIT_RNB_EDGE:
	ldr	r4, [r2, #FLSTATE]
	and	r4, r4, #FLSTATE_RNB_EDGE
	cmp	r4, #FLSTATE_RNB_EDGE
	bne	WAIT_RNB_EDGE
READ_CMPT_CHECK:
	ldr	r4, [r2, #FLSTATE]
	and	r4, r4, #FLSTATE_FSM_BUSY
	cmp	r4, #FLSTATE_FSM_BUSY
	beq	READ_CMPT_CHECK

        @; clear RnB Edge
	ldr	r4, =0x0
	str	r4, [r2, #FLSTATE]

        @; remap to CPU
	ldr	r4, =0
	str	r4, [r3, #BRC_CTRL]

	mov	pc, lr   @; return


@;----------------------------------------------------------------------------
@;   Preloader_Copy
@; move one page size data from SRAM to DDRSDRAM
@; @ params : fp, ip, sp
@;   fp: source address
@;   ip: size
@;   sp: destination address
@;
Preloader_Copy:
        @; copy Loader from ROM to DDR2 RAM
	add	ip, ip, fp   @; end address

COPY_LOOP:
	ldmia	fp!, {r3-r10}
	stmia	sp!, {r3-r10}
	cmp	fp, ip
	blo	COPY_LOOP

	mov	pc, lr

@;==================================================================================================
	.data
LOADER_FIRST_PAGENUM:
	.word	0x00000000
LOADER_SECOND_PAGENUM:
	.word	0x00000001
