/*
 * Statistics of read/write eye/deskew training format:
 *
 * V2:
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * | Offset |       0x0       |       0x4       |       0x8       |       0xC       |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x00  |    PGSR0 of     |    PGSR0 of     |                 |                 |
 * |        | PHYRST, DCAL,   |  WLADJ,QSGATE   |                 |                 |
 * |        | PLLINIT, ZCAL   |                 |                 |                 |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |        |   Byte Lane 0   |   Byte Lane 1   |   Byte Lane 2   |   Byte Lane 3   |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x10  |      PGSR0      |      PGSR0      |      PGSR0      |      PGSR0      |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x20  |     DX0GSR0     |     DX1GSR0     |     DX2GSR0     |     DX3GSR0     |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x30  |      DTEDR0     |      DTEDR0     |      DTEDR0     |      DTEDR0     |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x40  |      DTEDR1     |      DTEDR1     |      DTEDR1     |      DTEDR1     |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x50  |    DX0LCDLR1    |    DX1LCDLR1    |    DX2LCDLR1    |    DX3LCDLR1    |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 *
 * V3:
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * | Offset |       0x0       |       0x4       |       0x8       |       0xC       |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x00  |    PGSR0 of     |                 |                 |                 |
 * |        | PHYRST, DCAL,   |                 |                 |                 |
 * |        | PLLINIT, ZCAL   |                 |                 |                 |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |        | Rank 0 of PHY 0 | Rank 1 of PHY 0 |                 |                 |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x10  |      PGSR0      |      PGSR0      |                 |                 |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x20  |      DTEDR0     |      DTEDR0     |                 |                 |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 * |  0x30  |      DTEDR1     |      DTEDR1     |                 |                 |
 * +--------+-----------------+-----------------+-----------------+-----------------+
 */

#include <loader.h>

/*******************************************************************************
 * pub_preinit(pub_mmr_base)
 * pub_mmr_base:
 *     register base address of PUB
 ******************************************************************************/
	.global pub_preinit
pub_preinit:
	stmfd	sp!, {r1-r2, lr}

0:
	ldr	r1, [r0, #PGSR0]
	ands	r1, r1, #IDONE(1)
	beq	0b

	ldr	r1, =0xF000001E   /* DSGCR, PUREN=0 */
	str	r1, [r0, #DSGCR]

	ldr     r1, =PUB_DCR_DATA 
	str     r1, [r0,#DCR]			
	
#ifdef DYNAMIC_LOAD_TIMING
	ldr	r0, =0x4f80003C
	ldr	ip, [r0]
	and	ip, ip, #0x7   @ ip = model
	ldr	r0, =VPL_DDR32PHY_PUB_BASE
	
	cmp	ip, #0x0 @ 390A
	adreq	r1, _390A_DTPR_timing
	cmp	ip, #0x2 @ 390/385
	adreq	r1, _390_DTPR_timing
	cmp	ip, #0x3 @ 370
	adreq	r1, _370_DTPR_timing
	cmp	ip, #0x5 @ 330
	adreq	r1, _330_DTPR_timing
	ldm	r1, {r6-r8}
	str	r6, [r0, #DTPR0]
	str	r7, [r0, #DTPR1]
	str	r8, [r0, #DTPR2]
#else
	ldr	r1, =PUB_DTPR0_DATA
	str	r1, [r0, #DTPR0]
	ldr	r1, =PUB_DTPR1_DATA
	str	r1, [r0, #DTPR1]
	ldr	r1, =PUB_DTPR2_DATA
	str	r1, [r0, #DTPR2]
#endif

	ldr	r1, =PUB_MR0_DATA
	str	r1, [r0, #MR0]
	ldr	r1, =PUB_MR1_DATA
	str	r1, [r0, #MR1]
	ldr	r1, =PUB_MR2_DATA
	str	r1, [r0, #MR2]
	ldr	r1, =PUB_MR3_DATA
	str	r1, [r0, #MR3]

	/* TODO: unavailable rank */
	ldr	r1, =0x0280c613   /* PGCR1, ZCKSEL=2'b01, MDLEN=1, WLRANK=2'b01 */
	str	r1, [r0, #PGCR1]

	ldr	r1, =PUB_PTR0_DATA
	str	r1, [r0, #PTR0]
	ldr	r1, =PUB_PTR1_DATA
	str	r1, [r0, #PTR1]
	ldr	r1, =PUB_PTR2_DATA
	str	r1, [r0, #PTR2]
	ldr	r1, =PUB_PTR3_DATA
	str	r1, [r0, #PTR3]
	ldr	r1, =PUB_PTR4_DATA
	str	r1, [r0, #PTR4]

	ldr	r1, =0x0300150F   /* DTCR, RANKEN=4'b0011, DTCMPD=0, DTRPTN=4'b1111,  */
	str	r1, [r0, #DTCR]

	ldr	r1, =0x4c000e81   /* DX0GCR, WLRKEN=4'b0011 */
	str	r1, [r0, #DX0GCR]
	/* DX0BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX0BDLR0]
	str	r1, [r0, #DX0BDLR1]
	str	r1, [r0, #DX0BDLR2]
	str	r1, [r0, #DX0BDLR3]
	str	r1, [r0, #DX0BDLR4]
	/* DX0LCDLR0/1/2, all LCDL are 0  */
	str	r1, [r0, #DX0LCDLR0]
	str	r1, [r0, #DX0LCDLR1]
	str	r1, [r0, #DX0LCDLR2]

	ldr	r1, =0x4c000e81   /* DX1GCR, WLRKEN=4'b0011 */
	str	r1, [r0, #DX1GCR]
	/* DX1BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX1BDLR0]
	str	r1, [r0, #DX1BDLR1]
	str	r1, [r0, #DX1BDLR2]
	str	r1, [r0, #DX1BDLR3]
	str	r1, [r0, #DX1BDLR4]
	/* DX1LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX1LCDLR0]
	str	r1, [r0, #DX1LCDLR1]
	str	r1, [r0, #DX1LCDLR2]

	ldr	r1, =0x4c000e81   /* DX2GCR, WLRKEN=4'b0011 */
	str	r1, [r0, #DX2GCR]
	/* DX2BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX2BDLR0]
	str	r1, [r0, #DX2BDLR1]
	str	r1, [r0, #DX2BDLR2]
	str	r1, [r0, #DX2BDLR3]
	str	r1, [r0, #DX2BDLR4]
	/* DX2LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX2LCDLR0]
	str	r1, [r0, #DX2LCDLR1]
	str	r1, [r0, #DX2LCDLR2]

	ldr	r1, =0x4c000e81   /* DX3GCR, WLRKEN=4'b0011 */
	str	r1, [r0, #DX3GCR]
	/* DX3BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX3BDLR0]
	str	r1, [r0, #DX3BDLR1]
	str	r1, [r0, #DX3BDLR2]
	str	r1, [r0, #DX3BDLR3]
	str	r1, [r0, #DX3BDLR4]
	/* DX3LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX3LCDLR0]
	str	r1, [r0, #DX3LCDLR1]
	str	r1, [r0, #DX3LCDLR2]


#if CONFIG_DDR_PORT_0 == 0
	ldr	r1, [r0, #DX0GCR]
	bic	r1, #DXEN(1)
	str	r1, [r0, #DX0GCR]
	ldr	r1, [r0, #DX1GCR]
	bic	r1, #DXEN(1)
	str	r1, [r0, #DX1GCR]
#endif
#if CONFIG_DDR_PORT_1 == 0
	ldr	r1, [r0, #DX2GCR]
	bic	r1, #DXEN(1)
	str	r1, [r0, #DX2GCR]
	ldr	r1, [r0, #DX3GCR]
	bic	r1, #DXEN(1)
	str	r1, [r0, #DX3GCR]
#endif

	/* SoC-side ODT & output driver strength */
	ldr	r1, =PUB_DXCCR_DATA
	str	r1, [r0, #DXCCR]
	ldr	r1, =PUB_ZQ0CR1_DATA
	str	r1, [r0, #ZQ0CR1]
	/*
	ldr	r1, =PUB_ODTCR_DATA
	str	r1, [r0, #ODTCR]
	*/

	/* PHY Config Initialization Start... */
	ldr	r1, =(PHYRST(1) | DCAL(1) | PLLINIT(1) | ZCAL(1) | INIT(1))
	str	r1, [r0, #PIR]
	mov	r1, #0x10
	bl	_n_nops
0:
	ldr	r1, [r0, #PGSR0]
#if 0 /* loop infinitely with any abnormality */
	ldr	r2, =(ZCERR(1) | DCERR(1) | PLERR(1) | IERR(1) | \
		      ZCDONE(1) | DCDONE(1) | PLDONE(1) | IDONE(1))
	and	r1, r1, r2
	cmp	r1, #(ZCDONE(1) | DCDONE(1) | PLDONE(1) | IDONE(1))
	bne	0b
#else
	ands	r2, r1, #IDONE(1)
	beq	0b
#endif
	ldr	r2, =TRAINING_STATUS_ADDR
	str	r1, [r2]    /* save PGSR0 */

	/* return */
	ldmfd	sp!, {r1-r2, pc}



/*******************************************************************************
 * pub_postinit(pub_mmr_base)
 * pub_mmr_base:
 *     register base address of PUB
 ******************************************************************************/
	.global pub_postinit
pub_postinit:
	stmfd	sp!, {r1-r7, sl, lr}

	adr	sl, _pub_post
	ldm	sl, {r1-r7}

	str	r1, [r0, #DSGCR]
	str	r2, [r0, #PGCR0]
	str	r3, [r0, #PGCR1]
#if CONFIG_DDR_PORT_0 == 1
	str	r4, [r0, #DX0GCR]
	str	r5, [r0, #DX1GCR]
#endif
#if CONFIG_DDR_PORT_1 == 1
	str	r6, [r0, #DX2GCR]
	str	r7, [r0, #DX3GCR]
#endif

	/* PHYHRST */
	ldr	r1, [r0, #PGCR1]
	bic	r1, r1, #PHYHRST(1)
	str	r1, [r0, #PGCR1]
	orr	r1, r1, #PHYHRST(1)
	str	r1, [r0, #PGCR1]

	/* return */
	ldmfd	sp!, {r1-r7, sl, pc}

_pub_post:
	.int PUB_DSGCR_DATA  /* r1 */
	.int PUB_PGCR0_DATA  /* r2 */
	.int PUB_PGCR1_DATA  /* r3 */
	.int PUB_DX0GCR_DATA /* r4 */
	.int PUB_DX1GCR_DATA /* r5 */
	.int PUB_DX2GCR_DATA /* r6 */
	.int PUB_DX3GCR_DATA /* r7 */



#if (defined(CONFIG_0_PUB_BUILTIN_TRAINING) && CONFIG_0_PUB_BUILTIN_TRAINING == 1 )|| (defined(CONFIG_1_PUB_BUILTIN_TRAINING) && CONFIG_1_PUB_BUILTIN_TRAINING == 1)
/*******************************************************************************
 * pub_builtin_training(pub_mmr_base)
 * pub_mmr_base:
 *     register base address of PUB
 ******************************************************************************/
	.global pub_builtin_training
pub_builtin_training:

#if CONFIG_V3 == 1
	/***************************** V3 *****************************/
	stmfd	sp!, {r1-r4, lr}

#if CONFIG_DDR_PORT_0 == 1
	ldr	r4, =(TRAINING_STATUS_ADDR + 0x10)
#else
	ldr	r4, =(TRAINING_STATUS_ADDR + 0x14)
#endif

#if (CONFIG_DDR_PORT_0 == 1) && (CONFIG_0_PUB_BUILTIN_TRAINING == 1)
_rank_0:
	/* Prepare training DATX8 0/1 */
	mov	r3, #0
	ldr	r1, =RANKEN(1)|0x158F|DTRANK(0)|DTMPR(CFG_MPR)   /* DTCR, RANKEN=4'b0001, DTDBS=4'b0000, DTRANK=2'b00, DTRPTN=4'b1111 */
	str	r1, [r0, #DTCR]
	ldr	r1, =0x44000081   /* DX0GCR, WLRKEN=4'b0001, DXEN=1 */
	str	r1, [r0, #DX0GCR]
	ldr	r1, =0x44000081   /* DX1GCR, WLRKEN=4'b0001, DXEN=1 */
	str	r1, [r0, #DX1GCR]
	ldr	r1, =0x44000080   /* DX2GCR, WLRKEN=4'b0010, DXEN=0 */
	str	r1, [r0, #DX2GCR]
	ldr	r1, =0x44000080   /* DX3GCR, WLRKEN=4'b0010, DXEN=0 */
	str	r1, [r0, #DX3GCR]
	b	_training
#endif
#if (CONFIG_DDR_PORT_1 == 1) && (CONFIG_1_PUB_BUILTIN_TRAINING == 1)
_rank_1:
	mov	r3, #1
	/* Prepare training DATX8 2/3 */
	ldr	r1, =DDR1_LOAD_MODE_0_1_DATA&0xFFFF
	str	r1, [r0, #MR0]
	ldr	r1, =RANKEN(2)|0x158F|DTRANK(1)|DTMPR(CFG_MPR) //0x0202159F   /* DTCR, RANKEN=4'b0010, DTDBS=4'b0010, DTRANK=2'b01, DTRPTN=4'b1111 */
	str	r1, [r0, #DTCR]
	ldr	r1, =0x44000080   /* DX0GCR, WLRKEN=4'b0001, DXEN=0 */
	str	r1, [r0, #DX0GCR]
	ldr	r1, =0x44000080   /* DX1GCR, WLRKEN=4'b0001, DXEN=0 */
	str	r1, [r0, #DX1GCR]
	ldr	r1, =0x44000081   /* DX2GCR, WLRKEN=4'b0010, DXEN=1 */
	str	r1, [r0, #DX2GCR]
	ldr	r1, =0x44000081   /* DX3GCR, WLRKEN=4'b0010, DXEN=1 */
	str	r1, [r0, #DX3GCR]
#endif

_training:
	/* rank based training */
#if 1
	ldr     r1, =(WREYE(1) | RDEYE(1) | WRDSKW(1) | RDDSKW(1) | WLADJ(0) | QSGATE(1) | WL(0) | INIT(1))
	
#elif 0
	ldr	r1, =(WREYE(CFG_WREYE) | RDEYE(CFG_RDEYE) | WRDSKW(CFG_WRDSKW) | RDDSKW(CFG_RDDSKW) | WLADJ(0) | QSGATE(CFG_QSGATE) | WL(0) | INIT(1))
#endif

	str	r1, [r0, #PIR]
	mov	r1, #0x10
	bl	_n_nops
0:
	ldr	r1, [r0, #PGSR0]
#if 0 /* loop infinitely with any abnormality */
	ldr	r2, =(QSGERR(1) | QSGDONE(1) | IDONE(1))
	and	r1, r1, r2
	bic	r2, r2, #QSGERR(1)
	cmp	r1, r2
	bne	0b
#else
	ands	r2, r1, #IDONE(1)
	beq	0b
#endif
	str	r1, [r4], #4    /* save PGSR0 */
	ldr	r1, [r0, #DTEDR0]
	str	r1, [r4, #12]   /* save write eye training margins */
	ldr	r1, [r0, #DTEDR1]
	str	r1, [r4, #28]   /* save read eye training margins */

#if CONFIG_DDR_PORT_1 == 1 && (CONFIG_1_PUB_BUILTIN_TRAINING == 1)
	cmp	r3, #0
	beq	_rank_1
#endif

	/* return */
	ldmfd	sp!, {r1-r4, pc}

#else /* V2 */
	/***************************** V2 *****************************/

	stmfd	sp!, {r1-r9, lr}

	ldr	r7, =(TRAINING_STATUS_ADDR + 0x4)
	ldr	r8, =VPL_DDRSDMC_0_MMR_BASE
	ldr	r9, =VPL_DDRSDMC_1_MMR_BASE

	/* rank based training */
	ldr	r1, =(WLADJ(1) | QSGATE(1) | WL(0) | INIT(1))
	str	r1, [r0, #PIR]
	mov	r1, #0x10
	bl	_n_nops
0:
	ldr	r1, [r0, #PGSR0]
#if 0 /* loop infinitely with any abnormality */
	ldr	r2, =(WLAERR(1) | QSGERR(1) | WLERR(1) | IERR(1) | \
		      WLADONE(1) | QSGDONE(1) | WLDONE(0) | IDONE(1))
	and	r1, r1, r2
	cmp	r1, #(WLADONE(1) | QSGDONE(1) | WLDONE(0) | IDONE(1))
	bne	0b
#else
	ands	r2, r1, #IDONE(1)
	beq	0b
#endif
#if CONFIG_DDR_PORT_0 == 1
	str	r1, [r7], #0xc    /* save PGSR0 */
#else
	str	r1, [r7], #0x14   /* save PGSR0 */
#endif

	/* byte-lane based training */
	ldr	r1, =(MDLEN_DXGCR(1) | WLRKEN(3) | RTTOH(1) | DQRTT(0) | DQSRTT(0) | DSEN(1) | DXEN(0))
	str	r1, [r0, #DX0GCR]
	str	r1, [r0, #DX1GCR]
	str	r1, [r0, #DX2GCR]
	str	r1, [r0, #DX3GCR]

#if CONFIG_DDR_PORT_0 == 1
	/* start from byte lane 0 */
	add	r3, r0, #DX0GCR
	ldr	r4, =0
#elif CONFIG_DDR_PORT_1 == 1
	/* start from byte lane 2 */
	add	r3, r0, #DX2GCR
	ldr	r4, =2
#endif

	/* Increment CWL */
	ldr	r1, [r0, #DX2GTR]
	and	r1, r1, #R0WLSL(1)
	mov	r5, r1, LSR#0x09

	ldr	r1, [r0, #MR2]
	add	r1, r1, r5
	str	r1, [r0, #MR2]

#if CONFIG_DDR_PORT_0 == 1
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(1) | DDR3_ENABLE(CFG_0_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r8, #DDR_CTRL]   // bypass PUB
	ldr	r1, [r8, #DDR_LOAD_MODE_2_3]
	add	r1, r1, r5
	str	r1, [r8, #DDR_LOAD_MODE_2_3]
	ldr	r1, =0x00000021
	str	r1, [r8, #DDR_LOAD_MODE_CTRL]
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(0) | DDR3_ENABLE(CFG_0_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r8, #DDR_CTRL]   // through PUB
#endif
#if CONFIG_DDR_PORT_1 == 1
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(1) | DDR3_ENABLE(CFG_1_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r9, #DDR_CTRL]   // bypass PUB
	ldr	r1, [r9, #DDR_LOAD_MODE_2_3]
	add	r1, r1, r5
	str	r1, [r9, #DDR_LOAD_MODE_2_3]
	ldr	r1, =0x00000021
	str	r1, [r9, #DDR_LOAD_MODE_CTRL]
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(0) | DDR3_ENABLE(CFG_1_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r9, #DDR_CTRL]   // through PUB
#endif

_datx8_loop:
	ldr	r1, =(0x4C000E80 | DXEN(1))   // enable byte lane
	str	r1, [r3] // DXnGCR

	ldr	r1, =(WREYE(1) | RDEYE(1) | WRDSKW(1) | RDDSKW(1) | INIT(1))
	str	r1, [r0, #PIR]
	mov	r1, #0x10
	bl	_n_nops
0:
	ldr	r1, [r0, #PGSR0]
#if 0 /* loop infinitely with any abnormality */
	ldr	r2, =(WEERR(1) | REERR(1) | WDERR(1) | RDERR(1) | \
		      WEDONE(1) | REDONE(1) | WDDONE(1) | RDDONE(1) | IDONE(1))
	and	r1, r1, r2
	bic	r2, r2, #(WEERR(1) | REERR(1) | WDERR(1) | RDERR(1))
	cmp	r1, r2
	bne	0b
#else
	ands	r2, r1, #IDONE(1)
	beq	0b
#endif
	str	r1, [r7], #4    /* save PGSR0 */
	ldr	r1, [r3, #(DX0GSR0 - DX0GCR)]
	str	r1, [r7, #12]   /* save DXnGSR0 */
	ldr	r1, [r0, #DTEDR0]
	str	r1, [r7, #28]   /* save write eye training margins */
	ldr	r1, [r0, #DTEDR1]
	str	r1, [r7, #44]   /* save read eye training margins */
	ldr	r1, [r3, #(DX0LCDLR1 - DX0GCR)]
	str	r1, [r7, #60]   /* save WDQ/RDQS(N) LCDL after training */

	ldr	r1, =(0x4C000E80 | DXEN(0))   // disable byte lane
	str	r1, [r3]

	add	r3, r3, #0x40   // DXnGCR of next byte lane

#if CONFIG_DDR_PORT_1 == 0
	cmp	r4, #1
	add	r4, r4, #1
	blo	_datx8_loop
	bhs	_exit_datx8_loop
#elif CONFIG_DDR_PORT_0 == 0
	cmp	r4, #3
	add	r4, r4, #1
	bhs	_exit_datx8_loop
#else
	cmp	r4, #3
	bhs	_exit_datx8_loop
	cmp	r4, #2
	add	r4, r4, #1
	blo	_datx8_loop
#endif

	/* CWL (only for byte lane 3) (when WSL is different in BL[3:2], add BL3 and sub BL2) */
	ldr	r1, [r0, #DX3GTR]
	and	r1, r1, #0x00001000
	mov	r6, r1, LSR#0x09
	cmp	r5, r6
	beq	_datx8_loop

	ldr	r1, [r0, #MR2]
	sub	r1, r1, r5
	add	r1, r1, r6
	str	r1, [r0, #MR2]

	ldr	r1, =0x00020E41   /* bypass PUB */
	str	r1, [r9, #DDR_CTRL]
	ldr	r1, [r9, #DDR_LOAD_MODE_2_3]
	sub	r1, r1, r5
	add	r1, r1, r6
	str	r1, [r9, #DDR_LOAD_MODE_2_3]
	ldr	r1, =0x00000021
	str	r1, [r9, #DDR_LOAD_MODE_CTRL]
	ldr	r1, =0x00020641   /* through PUB */
	str	r1, [r9, #DDR_CTRL]

	b	_datx8_loop

_exit_datx8_loop:

	/*
	 * Decrement CWL (roll back)
	 */
	ldr	r1, [r0, #MR2]
	sub	r1, r1, r6
	str	r1, [r0, #MR2]

#if CONFIG_DDR_PORT_0 == 1
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(1) | DDR3_ENABLE(CFG_0_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r8, #DDR_CTRL]   // bypass PUB
	ldr	r1, [r8, #DDR_LOAD_MODE_2_3]
	sub	r1, r1, r5
	str	r1, [r8, #DDR_LOAD_MODE_2_3]
	ldr	r1, =0x00000021
	str	r1, [r8, #DDR_LOAD_MODE_CTRL]
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(0) | DDR3_ENABLE(CFG_0_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r8, #DDR_CTRL]   // through PUB
#endif
#if CONFIG_DDR_PORT_1 == 1
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(1) | DDR3_ENABLE(CFG_1_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r9, #DDR_CTRL]   // bypass PUB
	ldr	r1, [r9, #DDR_LOAD_MODE_2_3]
	sub	r1, r1, r5
	str	r1, [r9, #DDR_LOAD_MODE_2_3]
	ldr	r1, =0x00000021
	str	r1, [r9, #DDR_LOAD_MODE_CTRL]
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(0) | DDR3_ENABLE(CFG_1_DDR3_ENABLE) | DYNAMIC_ODT_EN(1) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r9, #DDR_CTRL]   // through PUB
#endif

	/* return */
	ldmfd	sp!, {r1-r9, pc}
#endif /* CONFIG_V3 */
#endif /* CONFIG_PUB_BUILTIN_TRAINING */


_n_nops:
	subs	r1, r1, #1
	bgt	_n_nops
	mov	pc, lr

#ifdef DYNAMIC_LOAD_TIMING
_390A_DTPR_timing:
	.word M390A_PUB_DTPR0_DATA
	.word M390A_PUB_DTPR1_DATA
	.word M390A_PUB_DTPR2_DATA
_390_DTPR_timing:
	.word M390_PUB_DTPR0_DATA
	.word M390_PUB_DTPR1_DATA
	.word M390_PUB_DTPR2_DATA
_370_DTPR_timing:
	.word M370_PUB_DTPR0_DATA
	.word M370_PUB_DTPR1_DATA
	.word M370_PUB_DTPR2_DATA
_330_DTPR_timing:
	.word M330_PUB_DTPR0_DATA
	.word M330_PUB_DTPR1_DATA
	.word M330_PUB_DTPR2_DATA
#endif
	.ltorg
