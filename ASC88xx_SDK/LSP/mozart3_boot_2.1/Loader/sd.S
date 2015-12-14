/*
 * sd.S
 *
 * Copyright 2011 VN Inc. All rights reserved.
 *
 */

#include <loader.h>

#define ARMBOOT_BASE_ADDRESS   (0X1000) /* for sector 0 */


/*******************************************************************************
 * _copy_uboot(dest)
 * @ params :
 *   dest(r0): destination address
 * @ internal :
 *   ip: destination address to which copy
 ******************************************************************************/
        .global _copy_uboot
_copy_uboot:
	stmfd   sp!, {r1-r3, r7, ip, lr}

	mov	ip, r0
	ldr     r0, =VPL_MSHC_0_MMR_BASE

	/* reset controller, FIFO, IDMA */
	ldr     r1, =CTRL_RESET_DATA
	str     r1, [r0, #SDIO_CTRL]
RESET_CHECK_LOOP:
	ldr     r1, [r0, #SDIO_CTRL]
	tst     r1, #CTRL_RESET_DATA
	bne     RESET_CHECK_LOOP

	/* controller power on */
	ldr     r1, =1
	str     r1, [r0, #SDIO_PWREN]
	/* clk disable */
	ldr     r2, =0
	str     r2, [r0, #SDIO_CLKENA]
	/* CMD0: go to idle state */
	ldr     r1, =SD_CMD0
	bl      SD_SET_CMD
	/* clk DIV */
	ldr     r1, =0x20 /* DIV=0x20, 400KHz */
	str     r1, [r0, #SDIO_CLKDIV]
	/* clk source */
	ldr     r1, =0
	str     r1, [r0, #SDIO_CLKSRC]
	/* CMD0: go to idle state */
	ldr     r1, =SD_CMD0
	bl      SD_SET_CMD
	/* clk enable */
	ldr     r1, =1
	str     r1, [r0, #SDIO_CLKENA]
	/* CMD0: go to idle state */
	ldr     r1, =SD_CMD0
	bl      SD_SET_CMD
	/* timeout */
	ldr     r1, =0xFFFFFFFF
	str     r1, [r0, #SDIO_TMOUT]
	/* card type : 1-bit */
	ldr     r1, =0
	str     r1, [r0, #SDIO_CTYPE]
	/* clear interrupt status */
	ldr     r1, =0xFFFFFFFF
	str     r1, [r0, #SDIO_RINTSTS]

	/* CMD0: go to idle state */
	ldr     r1, =SD_CMD0
	bl      SD_SET_CMD
	ldr     r1, =0x80008000 /* reset */
	bl      SD_SET_CMD
	bl      SD_CHECK_CD
	/* CMD8: identify version 2.0 */
	ldr     r2, =0x000001AA
	ldr     r1, =SD_CMD8
	bl      SD_SET_CMD
	bl      SD_CHECK_CD
	ldr     r1, [r0, #SDIO_RESP0]
	and     r1, r1, #0xAA
	cmp     r1, #0xAA
	beq     SD_IDENT_VERSION_OVER
	/* Card not supports version 2.0 */
	ldr     r2, =0
	ldr     r1, =0x80008000
	bl      SD_SET_CMD
	bl      SD_CHECK_CD
	ldr     r1, =RINTSTS_RTO /* clear response timeout RINTSTS */
	str     r1, [r0, #SDIO_RINTSTS]
	ldr     r2, =0x00FF8000
	b       SD_ACMD41_CONFIG_OCR
	/* Card supports version 2.0 */
SD_IDENT_VERSION_OVER:
	ldr     r2, =0x40FF8000
	/* ACMD41: config card OCR */
SD_ACMD41_CONFIG_OCR:
	ldr     r1, =SD_ACMD41
	bl      SD_SET_ACMD
	bl      SD_CHECK_CD
	/* check card power ready */
	ldr     r3, [r0, #SDIO_RESP0]
	tst	r3, #(1 << 31)
	beq     SD_ACMD41_CONFIG_OCR
	/* check SDHC/SDSC (Card Capacity Status) */
	tst	r3, #(1 << 30)   /* check bit[30] */
	ldreq	r7, =0  /* SDSC */
	ldrne	r7, =1  /* SDHC */
SD_IDENT_SDSC:
	/* CMD2: ident state */
	ldr     r2, =0
	ldr     r1, =SD_CMD2
	bl      SD_SET_CMD
	bl      SD_CHECK_CD
	/* CMD3: get RCA */
	ldr     r1, =SD_CMD3
	bl      SD_SET_CMD
	bl      SD_CHECK_CD
	ldr     r2, [r0, #SDIO_RESP0]   /* r2 = card RCA, for the following CMD7 */

	/* clk DIV */
	ldr     r1, =0x0 /* DIV=0x0, 25MHz */
	str     r1, [r0, #SDIO_CLKDIV]

	/* CMD0: go to idle state */
	ldr     r1, =SD_CMD0
	bl      SD_SET_CMD

	/* delay and wait for clk stable */
	ldr     r3, =VPL_SYSC_MMR_BASE
	ldr     r1, =0x80000000
	str     r1, [r3, #SYSC_CNT_CTRL] /* clear cnt, and cnt_div=1 */
MSHC_CLK_STABLE_POLL:
	ldr     r1, [r3, #SYSC_CNT_LOW]
	cmp     r1, #0x200   /* (64/25M)*100M = 0x200 */
	blo     MSHC_CLK_STABLE_POLL

	/* CMD7: go to transfer state */
	ldr     r1, =SD_CMD7
	bl      SD_SET_CMD
	bl      SD_CHECK_CD                     /* check command done */
	/* CMD16: set blk size */
	ldr     r2, =SDCARD_BLKSZ
	str     r2, [r0, #SDIO_BLKSIZ]         /* set blk size */
	ldr     r1, =SD_CMD16
	bl      SD_SET_CMD
	bl      SD_CHECK_CD                     /* check command done */

	/* FIFO threshold */
	ldr     r1, =SDIO_FIFOTH_DATA
	str     r1, [r0, #SDIO_FIFOTH]

TRYTO_GET_UBOOTSIZE:
	/* try to get Uboot size */
	ldr     r1,  =0xa00000              /*dest addr */
	ldr     r2,  =512                   /*size - one block */
	ldr     r3, =0 /*src addr */
	cmp     r7, #1
	bne     SDSC_ADDR_UNIT_BYTE0
	mov     r3, r3, LSR#9
SDSC_ADDR_UNIT_BYTE0:
	bl      SD_CARD_READ

AFTER_GET_UBOOTSIZE:
	/* try to copy UBOOT from SD to DRAM */
	mov	r1, ip
	ldr     r2, =0xa00000 + ARMBOOT_SIZE_OFFSET
	ldr     r2, [r2]
	ldr     r3, =ARMBOOT_BASE_ADDRESS
	cmp     r7, #1
	bne     SDSC_ADDR_UNIT_BYTE1
	mov     r3, r3, LSR#9
SDSC_ADDR_UNIT_BYTE1:
	bl      SD_CARD_READ

	ldmfd   sp!, {r1-r3, r7, ip, pc}


/*----------------------------------------------------------------------------
 *   SD_CARD_READ : SD card read
 * @ param : r0, r1, r2, r3
 * r0 : mshc controller base
 * r1 : destination address (DDR)
 * r2 : transfer size
 * r3 : source address (SD card)
 */
SD_CARD_READ:
	stmfd	sp!, {fp, ip, lr}

	/* transfer size : adjust it to be a multiple of block size */
	ldr     fp, =0x1FF
	and     fp, r2, fp
	cmp     fp, #0
	beq     MULTIPLE_OF_BLK_SZ
	mov     r2, r2, LSR#9
	add     r2, r2, #1
	mov     r2, r2, LSL#9
MULTIPLE_OF_BLK_SZ:
	str     r2, [r0, #SDIO_BYTCNT]
	/* clear interrupt status */
	ldr     fp, =0xFFFFFFFF
	str     fp, [r0, #SDIO_RINTSTS]
	/* read command */
	str     r3, [r0, #SDIO_CMDARG]
	ldr     fp, =SD_READ_MULTI_BLK_CMD
	str     fp, [r0, #SDIO_CMD]
READ_SD_CHECK_CD:
	ldr     fp, [r0, #SDIO_RINTSTS]
	and     fp, fp, #RINTSTS_CD
	cmp     fp, #RINTSTS_CD
	bne     READ_SD_CHECK_CD /* Check Command Done */
	str     fp, [r0, #SDIO_RINTSTS] /* Clear Command Done status */

	/* start to read */
READ_CMPT_CHECK_LOOP:
	/* check DTO first */
	ldr     fp, [r0, #SDIO_RINTSTS]
	tst     fp, #RINTSTS_DTO
	bne     SD_READ_DTO
	/* check RXDR */
	tst     fp, #RINTSTS_RXDR
	beq     READ_CMPT_CHECK_LOOP
	/* read 8 double word to SRAM */
	ldr     ip, =8
READ_CARD_DATA_TO_SRAM_LOOP:
	ldr     fp, [r0, #SDIO_DATA]
	str     fp, [r1], #4
	subs    ip, ip, #1
	bne     READ_CARD_DATA_TO_SRAM_LOOP
	/* clear RXDR interrupt */
	ldr     fp, =RINTSTS_RXDR
	str     fp, [r0, #SDIO_RINTSTS] /* Clear RXDR raw interrupt status */
	b       READ_CMPT_CHECK_LOOP
	/* Data Transfer Over */
SD_READ_DTO:
	ldr     fp, [r0, #SDIO_STATUS]
	ldr     ip, =STATUS_FIFOCOUNT_MASK
	and     fp, fp, ip
	movs    ip, fp, LSR#STATUS_FIFOCOUNT_SHIFT /* the rest count of data in FIFO */
	/* read the rest of data in FIFO */
SD_READ_REST_OF_FIFO:
	beq     SD_READ_REST_OF_FIFO_END
	ldr     fp, [r0, #SDIO_DATA]
	str     fp, [r1], #4
	subs    ip, ip, #1
	b       SD_READ_REST_OF_FIFO
SD_READ_REST_OF_FIFO_END:
	/* clear interrupts */
	ldr     fp, =RINTSTS_DTO
	orr     fp, fp, #RINTSTS_RXDR
	str     fp, [r0, #SDIO_RINTSTS] /* Clear DTO adn RXDR raw interrupt status */

	ldmfd	sp!, {fp, ip, pc}

/*----------------------------------------------------------------------------
 *   SD_SET_CMD : set cmd
 * @ param : r0, r1, r2
 * r0 : mshc controller base
 * r1 : acmd
 * r2 : argument
 */
SD_SET_CMD:
	stmfd	sp!, {fp, lr}

	str     r2, [r0, #SDIO_CMDARG]
	str     r1, [r0, #SDIO_CMD]
SET_CMD_CHECK_LOOP:
	ldr     fp, [r0, #SDIO_CMD]
	ands    fp, fp, #CMD_START_CMD
	bne     SET_CMD_CHECK_LOOP

	ldmfd	sp!, {fp, pc}

/*----------------------------------------------------------------------------
 *   SD_SET_ACMD : set acmd
 * @ param : r0, r1, r2
 * r0 : mshc controller base
 * r1 : acmd
 * r2 : argument
 */
SD_SET_ACMD:
	stmfd	sp!, {fp, lr}

	ldr     fp, =0
	str     fp, [r0, #SDIO_CMDARG]
	ldr     fp, =SD_CMD55
	str     fp, [r0, #SDIO_CMD]
SD_CMD55_CHECK_CD:
	ldr     fp, [r0, #SDIO_RINTSTS]
	ands    fp, fp, #RINTSTS_CD
	beq     SD_CMD55_CHECK_CD
	str     fp, [r0, #SDIO_RINTSTS]

	str     r2, [r0, #SDIO_CMDARG]
	str     r1, [r0, #SDIO_CMD]
SET_ACMD_CHECK_LOOP:
	ldr     fp, [r0, #SDIO_CMD]
	ands    fp, fp, #CMD_START_CMD
	bne     SET_ACMD_CHECK_LOOP

	ldmfd	sp!, {fp, pc}

/*----------------------------------------------------------------------------
 *   SD_CHECK_CD : check command done
 * @ param : r0
 * r0 : mshc controller base
 */
SD_CHECK_CD:
	stmfd	sp!, {fp, lr}
0:
	ldr	fp, [r0, #SDIO_RINTSTS]
	ands	fp, fp, #RINTSTS_CD
	beq	0b
	str	fp, [r0, #SDIO_RINTSTS]

	ldmfd	sp!, {fp, pc}
