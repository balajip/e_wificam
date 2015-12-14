/*
 * sf.S
 *
 * Copyright 2011 VN Inc. All rights reserved.
 *
 */

#include <loader.h>

/*******************************************************************************
 * _copy_uboot(dest)
 * @ params :
 *   dest(r0): destination address
 * @ internal :
 *   r9: block#
 *   ip: destination address to which copy
 ******************************************************************************/
	.global _copy_uboot
_copy_uboot:
	stmfd   sp!, {r1, r2, r9, ip, lr}

	mov	ip, r0

	ldr	r9, =0x0   @global counter
TRY_GET_UBOOTSIZE:
	ldr	r0, =(ARMBOOT_SIZE_OFFSET + 1)    @ size, including magic numbers and u-boot size
	mov	r1, #0x10000    @ src addr
	mul	r1, r9, r1
	mov	r2, #0xa00000   @ dst addr -- we choose 0xa00000 to place the image
	bl	_ssic_read

	@ check if we read a boot image sector?
	ldr	r0, =0xa00000
	ldr	r1, [r0]
	ldr	r2, =MG_0
	cmp	r1, r2
	addne	r9, r9, #0x1
	bne	TRY_GET_UBOOTSIZE
	ldr	r1, [r0, #4]
	ldr	r2, =MG_1
	cmp	r1, r2
	addne	r9, r9, #0x1
	bne	TRY_GET_UBOOTSIZE

	/* get U-Boot */
	ldr	r0, [r0, #ARMBOOT_SIZE_OFFSET]   @ size
	@mov	r1, #0x10000
	@sub	r9, r9, #0x1
	@mul	r1, r9, r1   @ find the start section address
	@mov	r9, r1
	ldr	r1, =ARMBOOT_BASE_ADDRESS   @ pointer to source addr
	ldr	r1, [r1]                    @ source addr
	@add	r1, r1, r9                  @ src addr = src addr + section address
	mov	r2, ip                      @ dest addr
	bl	_ssic_read

	ldmfd   sp!, {r1, r2, r9, ip, pc}


/*******************************************************************************
 * _ssic_read()
 * r0: read size
 * r1: source address
 * r2: destination address
 ******************************************************************************/
#define SSIC_READ_MAX_SIZE          0xF000
#define APBC_DMA_DATA_PER_MOVE      0x00000F00
#define APBC_DMA_CYCLE_PER_MOVE     APBC_DMA_DATA_PER_MOVE/4   /* APBC DMA burst length = 4 */
#define APBC_DMA_LLP_BASE           0x00700000
_ssic_read:
	stmfd   sp!, {r3-r8, sl, fp, ip, lr}

	/* round transfer size to multiple of 4 bytes.
	   (burst length of APBC DMA = 4; data with = 8-bit) */
	add	r0, r0, #3
	bic	r0, r0, #3

SSIC_READ_LOOP:
	ldr	fp, =VPL_SSI_MMR_BASE
	ldr	ip, =VPL_APBC_MMR_BASE

	@ Configure SSIC
	ldr	r4, =0x0
	str	r4, [fp, #SSI_SSIENR]   @ Disable SSIC
	str	r4, [fp, #SSI_SER]      @ Disable slave

	ldr	r4, =SSI_CTRLR0_DATA
	str	r4, [fp, #SSI_CTRLR0]

	mov	r6, r0
	cmp	r6, #SSIC_READ_MAX_SIZE
	ble	UNDER_SSIC_READ_MAX
	ldr	r6, =SSIC_READ_MAX_SIZE
UNDER_SSIC_READ_MAX:
	sub	r3, r6, #1
	str	r3, [fp, #SSI_CTRLR1]
	mov	r3, r6

	ldr	r4, =SSI_BAUDR_DATA
	str	r4, [fp, #SSI_BAUDR]

	ldr	r4, =SSI_DMARDLR_DATA           @ DMA Transmit Data Level
	str	r4, [fp, #SSI_DMARDLR]


	/* prepare DMA descriptors */
	ldr	r7, =APBC_DMA_LLP_BASE          @ LLP address
	ldr	r5, =(VPL_SSI_MMR_BASE + SSI_DR)
Loader_PrepareLLP:
	str	r5, [r7]                        @ source address
	str	r2, [r7, #4]                    @ destination address
	add	sl, r7, #16
	str	sl, [r7, #8]                    @ next pointer of LLP
	subs	sl, r6, #APBC_DMA_DATA_PER_MOVE
	ldrhi	r8, =APBC_DMA_CYCLE_PER_MOVE
	movls	r8, r6, lsr#2                   @ divided by burst length 4
	mov	r8, r8, LSL#DMA_CTRL_CYC_OFFSET
	ldr	sl, =DMA_CTRL_DATA|DMA_CTRL_EN
	orr	r8, r8, sl
	str	r8, [r7, #12]                   @ CTRL
	subs	r6, r6, #APBC_DMA_DATA_PER_MOVE
	add	r2, r2, #APBC_DMA_DATA_PER_MOVE
	addhi	r7, r7, #16
	bhi	Loader_PrepareLLP

	mov	sl, #0
	str	sl, [r7, #8]    @ last LLP

	/* setup the first DMA descriptor into APBC */
	ldr	r7, =APBC_DMA_LLP_BASE          @ LLP address
	ldr	sl, [r7], #4
	str	sl, [ip, #APBC_DMA_n_SRC_ADDR(APBC_DMA_CHN_SSIC_RX)]   @ DMA source address
	ldr	sl, [r7], #4
	str	sl, [ip, #APBC_DMA_n_DES_ADDR(APBC_DMA_CHN_SSIC_RX)]   @ DMA destination address
	ldr	sl, [r7], #4
	str	sl, [ip, #APBC_DMA_n_LLP(APBC_DMA_CHN_SSIC_RX)]        @ DMA LLP address
	ldr	sl, [r7], #4
	bic	sl, sl, #DMA_CTRL_EN
	str	sl, [ip, #APBC_DMA_n_CTRL(APBC_DMA_CHN_SSIC_RX)]       @ DMA CTRL (disable)

Start_Xfer:
	@ Enable SSIC and DMA
	ldr	r4, =0x1
	str	r4, [fp, #SSI_SSIENR]

	ldr	r4, =SSI_DMACR_DATA
	str	r4, [fp, #SSI_DMACR]

	@ SSIC send commands and address
	ldr	r4, =SFLASH_FAST_READ   @ fast read command
	str	r4, [fp, #SSI_DR]

	and	r4, r1, #0xFF0000    @ Address[23:16]
	mov	r4, r4, LSR#16
	str	r4, [fp, #SSI_DR]
	and	r4, r1, #0xFF00      @ Address[15:8]
	mov	r4, r4, LSR#8
	str	r4, [fp, #SSI_DR]
	and	r4, r1, #0xFF        @ Address[7:0]
	str	r4, [fp, #SSI_DR]
	str	r4, [fp, #SSI_DR]   @ Dummy Data

	@ Trigger SSIC-slave and DMA
	ldr	r4, [ip, #APBC_DMA_n_CTRL(APBC_DMA_CHN_SSIC_RX)]
	orr	r4, r4, #DMA_CTRL_EN
	str	r4, [ip, #APBC_DMA_n_CTRL(APBC_DMA_CHN_SSIC_RX)]

	ldr	r4, =0x1
	str	r4, [fp, #SSI_SER]

	@ Wait for completed interrupt
DMA_NOT_COMPLETE:
	ldr	r4, [ip, #APBC_DMA_n_CTRL(APBC_DMA_CHN_SSIC_RX)]
	and	r4, r4, #DMA_CTRL_CMPT_OCCUR
	cmp	r4, #DMA_CTRL_CMPT_OCCUR
	bne	DMA_NOT_COMPLETE

	@ Clear APBC intr
	ldr	r4, [ip, #APBC_DMA_n_CTRL(APBC_DMA_CHN_SSIC_RX)]
	mvn	r5, #DMA_CTRL_CMPT_OCCUR
	and	r4, r5, r4
	str	r4, [ip, #APBC_DMA_n_CTRL(APBC_DMA_CHN_SSIC_RX)]

	sub	r0, r0, r3
	add	r1, r1, r3
	cmp	r0, #0
	bne	SSIC_READ_LOOP

	ldmfd   sp!, {r3-r8, sl, fp, ip, pc}


	/*
	 * For dual boot.
	 * The integer is the offset of U-Boot in a valid boot block,
	 * and set to 0x1000 by default for block 0. For other block, it
	 * should be added with block offset by hand.
	 */
	.section .dualboot, "a"
	.ascii DUALBOOT_MARKER
ARMBOOT_BASE_ADDRESS:
	.int ARMBOOT_OFFSET
