/*
 * Statistics of read/write eye training format:
 *
 * +-----------------------------------------------------------------------------------+
 * |                         Read Eye Training Statistics                              |
 * +-------------+--------+-----------------+----------------+-------------+-----------+
 * |             | Offset |     Byte 3      |     Byte 2     |    Byte 1   |   Byte 0  |
 * +-------------+--------+-----------------+----------------+-------------------------+
 * |             | 0x00   | Original RDQSND | Original RDQSD | Average RDQSD/RDQSND    |
 * | Byte Lane 0 +--------+-----------------+----------------+-------------------------+
 * |             | 0x04   |      RDQS(N)D Left Boundary      | RDQS(N)D Right Boundary |
 * +-------------+---------------------------------------------------------------------+
 * |             | 0x08   | Original RDQSND | Original RDQSD | Average RDQSD/RDQSND    |
 * | Byte Lane 1 +--------+-----------------+----------------+-------------------------+
 * |             | 0x0C   |      RDQS(N)D Left Boundary      | RDQS(N)D Right Boundary |
 * +-------------+---------------------------------------------------------------------+
 * |             | 0x10   | Original RDQSND | Original RDQSD | Average RDQSD/RDQSND    |
 * | Byte Lane 2 +--------+-----------------+----------------+-------------------------+
 * |             | 0x14   |      RDQS(N)D Left Boundary      | RDQS(N)D Right Boundary |
 * +-------------+---------------------------------------------------------------------+
 * |             | 0x18   | Original RDQSND | Original RDQSD | Average RDQSD/RDQSND    |
 * | Byte Lane 3 +--------+-----------------+----------------+-------------------------+
 * |             | 0x1C   |      RDQS(N)D Left Boundary      | RDQS(N)D Right Boundary |
 * +-------------+---------------------------------------------------------------------+
 * |                         Write Eye Training Statistics                             |
 * +-------------+--------+-----------------+----------------+-------------+-----------+
 * |             | Offset |     Byte 3      |     Byte 2     |    Byte 1   |   Byte 0  |
 * +-------------+--------+-----------------+----------------+-------------------------+
 * |             | 0x20   |   WDQ Margin    | Original WDQD  |      Average WDQD       |
 * | Byte Lane 0 +--------+-----------------+----------------+-------------------------+
 * |             | 0x24   |        WDQD Left Boundary        |   WDQD Right Boundary   |
 * +-------------+---------------------------------------------------------------------+
 * |             | 0x28   |   WDQ Margin    | Original WDQD  |      Average WDQD       |
 * | Byte Lane 1 +--------+-----------------+----------------+-------------------------+
 * |             | 0x2C   |        WDQD Left Boundary        |   WDQD Right Boundary   |
 * +-------------+---------------------------------------------------------------------+
 * |             | 0x30   |   WDQ Margin    | Original WDQD  |      Average WDQD       |
 * | Byte Lane 2 +--------+-----------------+----------------+-------------------------+
 * |             | 0x34   |        WDQD Left Boundary        |   WDQD Right Boundary   |
 * +-------------+---------------------------------------------------------------------+
 * |             | 0x38   |   WDQ Margin    | Original WDQD  |      Average WDQD       |
 * | Byte Lane 3 +--------+-----------------+----------------+-------------------------+
 * |             | 0x3C   |        WDQD Left Boundary        |   WDQD Right Boundary   |
 * +-------------+---------------------------------------------------------------------+
 */

#include "loader.h"


#define F_READ                    (0)
#define F_WRITE                   (1)
#define F_LEFT                    (0)
#define F_RIGHT                   (1)
#define BYTE_LANE_0               (0x01)
#define BYTE_LANE_1               (0x10)
#define RDQSD_MASK                (0xff00)
#define RDQSND_MASK               (0xff0000)
#define WDQD_MASK                 (0xff)

#define DDR_ACCESS_OFFSET         (0x401000)
#define PATTERN_1                 (0x55555555)
#define PATTERN_2                 (0xAAAAAAAA)
#define DDR32PUB_TRAINING_COUNT   (1000)


_rd_train_data:
	.word	(LOADER_VERSION_ADDR + 16)
	/*.word	__fdstack*/
_wr_train_data:
	.word	(LOADER_VERSION_ADDR + 16 + 32)
	/*.word	(__fdstack + 32)*/



/*******************************************************************************
 * pub_sw_training()
 * r8: PHYAC_BUS_0_1_MMR_BASE + DX0LCDLR1
 * r9: PHYAC_BUS_0_1_MMR_BASE + DX0LCDLR1 + 0x100
 * ip: read/write training data (original/left/right/average)
 ******************************************************************************/
	.global pub_sw_training
pub_sw_training:
	stmfd	sp!, {r0-r4, r8, r9, ip, lr}

	/***** preparation for data training ******/

	ldr	r0, =DDR32SDMC_0_BASE
	ldr	r1, =DDR32SDMC_1_BASE
	mov	r3, #0
	/* Disable power saving mode */
	str	r3, [r0, #DDR_PWRDWN_MODE]
	str	r3, [r0, #DDR_SELF_REFRESH]
	str	r3, [r1, #DDR_PWRDWN_MODE]
	str	r3, [r1, #DDR_SELF_REFRESH]
	/* Disable ZQC */
	str	r3, [r0, #DDR_ZQ_CTRL]
	str	r3, [r1, #DDR_ZQ_CTRL]
	/* Disable IO dynamic control */
	str	r3, [r0, #DDR_IO_DYNAMIC_CTRL]
	str	r3, [r1, #DDR_IO_DYNAMIC_CTRL]

	ldr	r2, =PHYAC_BUS_0_1_MMR_BASE
	/* disable PUREN, DQSGX */
	ldr	r3, [r2, #DSGCR]
	bic	r3, r3, #(DQSGX(1) | PUREN(1))
	str	r3, [r2, #DSGCR]
	/* DXnGCR, disalbe MDLEN */
	ldr	r3, [r2, #DX0GCR]
	bic	r3, r3, #MDLEN(1)
	str	r3, [r2, #DX0GCR]
	ldr	r3, [r2, #DX1GCR]
	bic	r3, r3, #MDLEN(1)
	str	r3, [r2, #DX1GCR]
	ldr	r3, [r2, #DX2GCR]
	bic	r3, r3, #MDLEN(1)
	str	r3, [r2, #DX2GCR]
	ldr	r3, [r2, #DX3GCR]
	bic	r3, r3, #MDLEN(1)
	str	r3, [r2, #DX3GCR]


	/***** start training ******/

	ldr	r8, =(PHYAC_BUS_0_1_MMR_BASE + DX0LCDLR1)
	ldr	r9, =(PHYAC_BUS_0_1_MMR_BASE + DX0LCDLR1 + 0x100)

read_eye_training:
	ldr	ip, _rd_train_data

	/* Backup original RDQS(N)_LCDL */
	mov	r1, r8
	mov	r2, ip
1:
	ldr	r3, [r1], #0x40
	lsl	r3, r3, #8
	lsr	r3, r3, #16
	lsl	r3, r3, #16
	cmp	r1, r9
	str	r3, [r2], #8
	bne	1b


	/* Write 6 burst-8 pattern
	 * TODO: only write enabled DRAM */
	ldr	r1, =(DDR_0_MEM + DDR_ACCESS_OFFSET)
	adr	r0, _pat1
	bl	burst_pattern
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat2
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern
	adr	r0, _pat4
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern

	ldr	r1, =(DDR_1_MEM + DDR_ACCESS_OFFSET)
	adr	r0, _pat1
	bl	burst_pattern
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat2
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern
	adr	r0, _pat4
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern


	/* Decrement RDQS(N)_LCDL to find the left, read any bit incorrect */
#if CONFIG_DDR_PORT_0
	mov	r0, #F_READ
	mov	r1, #F_LEFT
	mov	r2, #0
	bl	rw_eye
#endif
#if CONFIG_DDR_PORT_1
	mov	r0, #F_READ
	mov	r1, #F_LEFT
	mov	r2, #2
	bl	rw_eye
#endif

	/* Record left boundary */
	mov	r1, r8
	add	r2, ip, #4
1:
	ldr	r3, [r1], #0x40
	lsl	r3, r3, #8
	and	r3, r3, #0xff0000
	cmp	r1, r9
	str	r3, [r2], #8
	bne	1b

	/* Restore original RDQS(N)_LCDL */
	mov	r1, r8
	mov	r2, ip
1:
	ldr	r3, [r1]       /* DXnLCDLR1     */
	ldr	r0, [r2], #8   /* rd_train_data */
	bic	r3, r3, #0xff00
	bic	r3, r3, #0xff0000
	orr	r3, r3, r0, lsr #8
	str	r3, [r1], #0x40
	cmp	r1, r9
	bne	1b

	/* Increment RDQS(N)_LCDL to find the right, read any bit incorrect */
#if CONFIG_DDR_PORT_0
	mov	r0, #F_READ
	mov	r1, #F_RIGHT
	mov	r2, #0
	bl	rw_eye
#endif
#if CONFIG_DDR_PORT_1
	mov	r0, #F_READ
	mov	r1, #F_RIGHT
	mov	r2, #2
	bl	rw_eye
#endif

	/* Record right boundary */
	mov	r1, r8
	add	r2, ip, #4
1:
	ldr	r3, [r1], #0x40
	ldr	r0, [r2]
	lsl	r3, r3, #16
	orr	r0, r0, r3, lsr #24
	cmp	r1, r9
	str	r0, [r2], #8
	bne	1b

	/* Set center value for RDQS(N)_LCDL and RDQ_BDL delay */
	mov	r1, r8
	mov	r2, ip
1:
	ldm	r2, {r0, r3}
	add	r3, r3, r3, lsr #16
	and	r3, r3, #0xff
	orr	r0, r0, r3, lsr #1   /* average */
	mov	r4, r0, lsl #16
	orr	r4, r4, r4, lsr #8
	ldr	r3, [r1]
	bic	r3, #0xff00
	bic	r3, #0xff0000
	orr	r3, r3, r4
	str	r3, [r1], #0x40
	cmp	r1, r9
	str	r0, [r2], #8
	bne	1b
	

write_eye_training:
	ldr	ip, _wr_train_data

	/* Backup original WDQ_LCDL */
	mov	r1, r8
	mov	r2, ip
	ldr	r0, =(PHYAC_BUS_0_1_MMR_BASE + DTCR)
 	ldr	r0, [r0]
	lsr	r0, r0, #8
	and	r0, r0, #0xf   /* DTWDQM: training WDQ margin */
1:
	ldr	r3, [r1], #0x40
	and	r3, r3, #0xff
	rsb	r4, r0, r3, lsl #1
	orr	r3, r3, r4, lsl #8
	lsl	r3, r3, #16
	cmp	r1, r9
	str	r3, [r2], #8
	bne	1b

	/* Increment WDQ_LCDL to find the left, read any bit incorrect */
#if CONFIG_DDR_PORT_0
	mov	r0, #F_WRITE
	mov	r1, #F_LEFT
	mov	r2, #0
	bl	rw_eye
#endif
#if CONFIG_DDR_PORT_1
	mov	r0, #F_WRITE
	mov	r1, #F_LEFT
	mov	r2, #2
	bl	rw_eye
#endif

	/* Record left boundary */
	mov	r1, r8
	add	r2, ip, #4
1:
	ldr	r3, [r1], #0x40
	lsl	r3, r3, #16
	and	r3, r3, #0xff0000
	cmp	r1, r9
	str	r3, [r2], #8
	bne	1b

	/* Restore original WDQ_LCDL */
	mov	r1, r8
	mov	r2, ip
1:
	ldr	r3, [r1]       /* DXnLCDLR1     */
	ldr	r0, [r2], #8   /* wr_train_data */
	bic	r3, r3, #0xff
	lsl	r0, r0, #8
	orr	r3, r3, r0, lsr #24
	str	r3, [r1], #0x40
	cmp	r1, r9
	bne	1b

	/* Decrement WDQ_LCDL to find the right, read any bit incorrect */
#if CONFIG_DDR_PORT_0
	mov	r0, #F_WRITE
	mov	r1, #F_RIGHT
	mov	r2, #0
	bl	rw_eye
#endif
#if CONFIG_DDR_PORT_1
	mov	r0, #F_WRITE
	mov	r1, #F_RIGHT
	mov	r2, #2
	bl	rw_eye
#endif

	/* Record right boundary */
	mov	r1, r8
	add	r2, ip, #4
1:
	ldr	r3, [r1], #0x40
	ldr	r0, [r2]
	and	r3, r3, #0xff
	orr	r0, r0, r3
	cmp	r1, r9
	str	r0, [r2], #8
	bne	1b

	/* Set center value for WDQ_LCDL and WDQ_BDL delay */
	mov	r1, r8
	mov	r2, ip
1:
	ldm	r2, {r0, r3}
	add	r3, r3, r3, lsr #16
	and	r3, r3, #0xff
	orr	r0, r0, r3, lsr #1   /* average */
	and	r4, r0, #0xff
	ldr	r3, [r1]
	bic	r3, #0xff
	orr	r3, r3, r4
	str	r3, [r1], #0x40
	cmp	r1, r9
	str	r0, [r2], #8
	bne	1b

	/* return */
	ldmfd	sp!, {r0-r4, r8, r9, ip, pc}


_pat1:
	.word	0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x00000000, 0x00000000, 0x00000000, 0x00000000
_pat2:
	.word	0x55550000, 0x00005555, 0x55555555, 0x55555555, 0x00000000, 0x00000000, 0x00000000, 0x00000000
_pat3:
	.word	0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0x00000000, 0x00000000, 0x00000000, 0x00000000
_pat4:
	.word	0xAAAA0000, 0x0000AAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0x00000000, 0x00000000, 0x00000000, 0x00000000


/*******************************************************************************
 * rw_eye()
 * r0 : rw (read/write)
 * r1 : lr (left/right)
 * r2 : lane (byte lane 0/2 <---- TODO: 2 loops: one for lane0/1, the other for lane2/3)
 * r3 : mem
 * r4 : j(bit 4), i(bit 0)
 * r5 : data
 * r6 : l
 * r7 : data2
 * r8 : data_h
 * r9 : k
 * sl
 * fp
 * ip
 * sp
 * lr : PHYAC_BUS_0_1_MMR_BASE
 * pc
 ******************************************************************************/
rw_eye:
	stmfd	sp!, {r0-ip, lr}

	/* var: mem */
	cmp	r2, #0
	ldreq	r3, =(DDR_0_MEM + DDR_ACCESS_OFFSET)
	ldrne	r3, =(DDR_1_MEM + DDR_ACCESS_OFFSET)

	/* i = 1, j = 1 */
	mov	r4, #(0x11)

scan_loop:
	cmp	r0, #F_WRITE
	bne	1f

	/* Write 6 burst-8 pattern
	 * TODO: only write pattern to tested DRAM */
	ldr	r1, =(DDR_0_MEM + DDR_ACCESS_OFFSET)
	adr	r0, _pat1
	bl	burst_pattern
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat2
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern
	adr	r0, _pat4
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern

	ldr	r1, =(DDR_1_MEM + DDR_ACCESS_OFFSET)
	adr	r0, _pat1
	bl	burst_pattern
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat2
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern
	adr	r0, _pat4
	bl	burst_pattern
	add	r1, r1, #16
	adr	r0, _pat3
	bl	burst_pattern

	/* restore r0, r1 */
	ldr	r0, [sp]
	ldr	r1, [sp, #4]

1:
	ldr	lr, =PHYAC_BUS_0_1_MMR_BASE

	/* Do PHYHRST after delay line tap change. */
	ldr	sl, [lr, #PGCR1]
	bic	sl, sl, #PHYHRST(1)
	str	sl, [lr, #PGCR1]
	orr	sl, sl, #PHYHRST(1)
	str	sl, [lr, #PGCR1]

	/* for (l = 1; l <= 2; l++) */
	mov	r6, #2
pattern_loop:
	cmp	r6, #1
	ldreq	r7, =PATTERN_1
	ldrne	r7, =PATTERN_2
	/* for (k = 0; k < DDR32PUB_TRAINING_COUNT; k++) */
	ldr	r9, =DDR32PUB_TRAINING_COUNT
	add	sl, r3, r6, lsl #4
cmp_loop:
	/* TODO: break if incorrect */
	ldr	r5, [sl]            /* data   */
	ldr	r8, [sl, #4]        /* data_h */
	eor	fp, r5, r7
	mov	ip, fp, lsr #16
	eor	fp, r8, r7
	orr	ip, fp, lsl #16
	ands	fp, ip, #0x000000ff  /* check byte lane 0     */
	andeqs	fp, ip, #0x00ff0000  /* check byte lane 0     */
	bicne	r4, r4, #BYTE_LANE_0 /* incorrect byte lane 0 */
	ands	fp, ip, #0x0000ff00  /* check byte lane 1     */
	andeqs	fp, ip, #0xff000000  /* check byte lane 1     */
	bicne	r4, r4, #BYTE_LANE_1 /* incorrect byte lane 1 */

	subs	r9, r9, #1
	bne	cmp_loop

	subs	r6, r6, #1
	bne	pattern_loop


	/* for (l = 0; l < 2; l++) */
	add	ip, lr, r2, lsl #6
	add	ip, ip, #DX0LCDLR1
	ldr	fp, _wr_train_data
	add	fp, fp, r2, lsl #3        /* WDQD left limit / original */
	mov	r6, #0                    /* l */
lane_loop:
	rsbs	sl, r6, #1
	movcc	sl, #0                    /* ??? if l(r)>=2, sl=0 */
	tst	r4, #BYTE_LANE_0          /* if (i == 1) */
	cmpne	r6, #1                    /* if (l == 0) tricky: l must be either 0 or 1. */
	bne	1f
	tst	r4, #BYTE_LANE_1          /* if (j == 1) */
	cmpne	r6, #0
	beq	6f
1:
	cmp	r0, #F_READ
	ldr	r5, [ip]   /* DXnLCDLR1 */
	bne	_f_write
_f_read: /* F_READ */
	cmp	r1, #F_LEFT
	movne	r7, #0xff00
	moveq	r7, #0
	and	r8, r5, #RDQSD_MASK
	cmp	r7, r8
	beq	_boundary
	cmp	r1, #F_LEFT
	movne	r7, #0xff0000
	moveq	r7, #0
	and	r8, r5, #RDQSND_MASK
	cmp	r7, r8
	bne	_cont_read
	b	_boundary
_f_write: /* F_WRITE */
	cmp	r1, #F_LEFT
	ldreq	r7, [fp, r6, lsl #3]   /* WDQ margin */
	movne	r7, #0
	lsreq	r7, r7, #24
	and	r8, r5, #WDQD_MASK
	cmp	r7, r8
	bne	_cont_write
_boundary:
	cmp	sl, #0
	biceq	r4, r4, #BYTE_LANE_1
	bicne	r4, r4, #BYTE_LANE_0
	b	6f
_cont_read: /* F_READ decrease/increase delay value */
	cmp	r1, #F_LEFT
	ldr	r7, =0x010100
	rsbeq	r7, r7, #0   /* -0x010100 */
	/*
	mvnne	r7, r7
	addne	r7, r7, #1
	*/
	b	_update_delay
_cont_write: /* F_WRITE decrease/increase delay value */
	cmp	r1, #F_LEFT
	moveq	r7, #1
	mvnne	r7, #0
_update_delay:
	add	r5, r5, r7
	str	r5, [ip] /* update MMR */
6:
	add	r6, r6, #1
	cmp	r6, #2
	add	ip, ip, #0x40   /* DXnLCDLR1 */
	bne	lane_loop

	tst	r4, #(BYTE_LANE_1 | BYTE_LANE_0)
	bne	scan_loop

	/* return */
	ldmfd	sp!, {r0-ip, pc}


/*******************************************************************************
 * burst_pattern()
 * r0: source address
 * r1: destination address
 ******************************************************************************/
burst_pattern:
	stmfd	sp!, {r2, r3, lr}

	ldr	r2, =DMAC_BASE

	ldr	r3, =0x20                           /* fixed 32 bytes */
	str	r3, [r2, #DMAC_TRANS_SIZE]          /* transfer size */
	ldr	r3, =0x100
	str	r3, [r2, #DMAC_MAX_BURLST_LENGTH]   /* maximum burst length */

	str	r0, [r2, #DMAC_SRC_ADDR]            /* source address */
	str	r1, [r2, #DMAC_DST_ADDR]            /* destination address */
	ldr	r3, =(DATA_WIDTH_FLAG(0x2) | OP_START(1) | OP_CMPT_ACK_EN(1))
	str	r3, [r2, #DMAC_CTRL]
1:
	ldr	r3, [r2, #DMAC_CTRL]
	ands	r3, r3, #OP_CMPT_ACK(1)
	beq	1b

	ldmfd	sp!, {r2, r3, pc}
