/*
 * nf2k.S
 *
 * Copyright 2011 VN Inc. All rights reserved.
 *
 */

#include <loader.h>

	.macro mod r, a, b
		mov	\r, \a
	1:	sub	\r, \r, \b
		cmp	\r, \b
		bge	1b
	.endm


/*******************************************************************************
 * _copy_uboot(dest)
 * @ params :
 *   dest(r0): destination address
 * @ internal :
 *   sl: total pages of u-boot
 *   fp: page number of whole chip to be copied.
 *   ip: destination address to which copy
 ******************************************************************************/
        .global _copy_uboot
_copy_uboot:
	stmfd   sp!, {r1-r5, sl, fp, ip, lr}

	mov	ip, r0

	ldr	r0, =NFCTRL_BASE
	ldr	r1, =0x00000A01
	str	r1, [r0, #FLCTRL]

	ldr	r1, =ARMBOOT_BASE_BY_PAGE
	ldr	fp, [r1]
	ldr	r1, [r0, #FLADDR0L]   /* fladdr0l should be the page number of bootldr image */
	add	fp, fp, r1, lsr#COL_ADDR_BIT

LOADER_COPY_ARMBOOT_LOOP:
	/*
	 * Check the validality of block.
	 */
	ldr	r0, =NFCTRL_BASE
	mov	r1, fp
	mov	r4, #0
	bl	CHECK_BLOCK_VALIDITY
	cmp	r0, #0
	bne	_invalid   /* invalid block */
	/*
	 * Check if another image begins.
	 */
	ldr	r0, =NFCTRL_BASE
	mov	r1, fp
	mov	r4, #1
	bl	CHECK_BLOCK_VALIDITY
	cmp	r0, #0
	bne	_cpy_pages_in_block
	/* get the size of u-boot */
	ldr	sl, =VPL_BRC_RAM_BASE_REMAPPED
	ldr	sl, [sl, #ARMBOOT_SIZE_OFFSET]   /* size of u-boot (in pages) */
	ldr	r1, =PAGES_PER_BLOCK_MASK
	bic	fp, fp, r1
	ldr	r1, =ARMBOOT_BASE_BY_PAGE
	ldr	r1, [r1]
	add	fp, fp, r1
	mov	r5, ip
	b	_cpy_pages_in_block
_invalid:
	/*
	 * If not valid, goto next block.
	 */
	add	fp, fp, #PAGES_PER_BLOCK
	ldr	r1, =PAGES_PER_BLOCK_MASK
	bic	fp, fp, r1
	b	LOADER_COPY_ARMBOOT_LOOP
	/*
	 * If valid, read u-boot in a block.
	 */
_cpy_pages_in_block:
	ldr	r1, =PAGES_PER_BLOCK_MASK
	and	r1, fp, r1
	rsb	r3, r1, #(PAGES_PER_BLOCK - 1)   /* pages to be copied in a block, skip the last page */
0:
	ldr	r0, =NFCTRL_BASE
	mov	r1, fp
	bl	Loader_NAND_Page_Read
	ldr	r0, =VPL_BRC_RAM_BASE_REMAPPED /* source address */
	ldr	r1, =NAND_PAGE_SIZE            /* size */
	mov	r2, r5                         /* dest address */
	bl	Loader_Copy_SRAM_to_SDRAM
	add	fp, fp, #1
	add	r5, #NAND_PAGE_SIZE
	subs	sl, #1
	beq	1f
	subs	r3, #1
	bne	0b
	add	fp, #1 /* should be the first page of next block */
	b	LOADER_COPY_ARMBOOT_LOOP
1:
	ldmfd   sp!, {r1-r5, sl, fp, ip, pc}


/*----------------------------------------------------------------------------
 * Loader_NAND_Page_Read()
 * @ params :
 *   r0: controller base address
 *   r1: row address (page number)
 */
Loader_NAND_Page_Read:
	stmfd   sp!, {r3, r4, lr}

	/* remap to NFC */
	ldr	r3, =VPL_BRC_MMR_BASE_REMAPPED
	ldr	r4, =1
	str	r4, [r3, #BRC_CTRL]

	/* NF Memory Busy Check */
CTRL_BUSY_CHECK:
	ldr	r4, [r0, #FLSTATE]
	tst	r4, #FLSTATE_FSM_BUSY
	bne	CTRL_BUSY_CHECK
MEM_BUSY_CHECK:
	ldr	r4, [r0, #FLSTATE]
	tst	r4, #FLSTATE_RNB_ST
	beq	MEM_BUSY_CHECK

	/* Address */
	mov	r4, r1, LSL#COL_ADDR_BIT
	str	r4, [r0, #FLADDR0L]

	/* Command */
	ldr	r4, =NANDFLASH_PAGE_READ
	str	r4, [r0, #FLCOMM]

	/* Wait RnB edge high */
WAIT_RNB_EDGE:
	ldr	r4, [r0, #FLSTATE]
	tst	r4, #FLSTATE_RNB_EDGE
	beq	WAIT_RNB_EDGE
READ_CMPT_CHECK:
	ldr	r4, [r0, #FLSTATE]
	tst	r4, #FLSTATE_FSM_BUSY
	bne	READ_CMPT_CHECK

	/* clear RnB Edge */
	ldr	r4, =0x0
	str	r4, [r0, #FLSTATE]

	/* remap to CPU */
	ldr	r4, =0
	str	r4, [r3, #BRC_CTRL]

	ldmfd   sp!, {r3, r4, pc}

/*----------------------------------------------------------------------------
 * CHECK_BLOCK_VALIDITY ()
 * bad block check, or meet another image
 * @ params :
 *   r0 : controller base address
 *   r1 : current page number
 *   r4 : bad block check if 0; image check if 1
 * @ return :
 *   r0 : 0: valid/header block, 1: invalid/non-header block
 */
CHECK_BLOCK_VALIDITY:
	stmfd   sp!, {r2, r3, lr}

	ldr	r3, =PAGES_PER_BLOCK_MASK
	and	r2, r1, r3
	sub	r1, r1, r2   /* get the 1st page of the block to which. */
	cmp	r4, #0
	addeq	r1, r1, #(PAGES_PER_BLOCK - 1)   /* read the last page to check bad block. */
	bl	Loader_NAND_Page_Read
	mov	r0, #0
	/*
	 * Check the magic#
	 */
	ldr     r3, =VPL_BRC_RAM_BASE_REMAPPED
	ldr     r1, [r3]
	ldr	r2, =MG_0
	cmp	r1, r2
	movne	r0, #1
	bne	EXIT_CHECK_BLOCK_VALIDITY
0:
	ldr     r1, [r3, #4]
	ldr     r2, =MG_1
	cmp     r1, r2
	beq	EXIT_CHECK_BLOCK_VALIDITY
	movne	r0, #1

EXIT_CHECK_BLOCK_VALIDITY:
	ldmfd   sp!, {r2, r3, pc}

/*----------------------------------------------------------------------------
 * Loader_Copy_SRAM_to_SDRAM()
 * move one page size data from SRAM to DDRSDRAM.
 * @ params :
 *   r0: source address (SRAM)
 *   r1: size
 *   r2: destination address (DDRSDRAM)
 */
Loader_Copy_SRAM_to_SDRAM:
	stmfd   sp!, {r3-ip, lr}

	add	r1, r0, r1   /* end address */
COPY_LOOP:
	ldmia   r0!, {r3-ip}
	stmia   r2!, {r3-ip}
	cmp	r0, r1
	blo	COPY_LOOP

	ldmfd   sp!, {r3-ip, pc}


	/* for dual boot */
	.section .dualboot, "a"
	.ascii DUALBOOT_MARKER
ARMBOOT_BASE_BY_PAGE:
	.int 0x2
