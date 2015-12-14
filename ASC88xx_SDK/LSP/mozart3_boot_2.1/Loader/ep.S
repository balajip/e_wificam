/*
 * ep.S
 *
 * Copyright 2011 VN Inc.  All rights reserved.
 *
 */

#include <loader.h>

#define WAITING 0xfffff

/*******************************************************************************
 * ep_init()
 ******************************************************************************/
	.global ep_init
ep_init:
	stmfd	sp!, {r0-r2, r5, r6, fp, ip, lr}

	/* === PCI Express End Point Link Training === */
	ldr	ip, =WAITING

	/* set PHY_CFG_DONE to 1 */
	ldr	r0, =VPL_PCIEC_SSC_BASE
	ldr	r1, [r0, #PCIESSC_CFG_0]
	orr	r1, r1, #PHY_CFG_DONE(1)
	str	r1, [r0, #PCIESSC_CFG_0]
	ldr	fp, =0x0
pcie_reset_done_loop:
	add	fp, fp, #1
	cmp	fp, ip
	bgt	1f
	ldr	r1, [r0, #PCIESSC_CFG_0]
	ands	r1, r1, #PCIEC_RESET_DONE(1)
	beq	pcie_reset_done_loop

	/* start and check LTSSM */
	ldr	r0, =PCIEC_ELBI_BASE
	ldr	r1, =(APP_LTSSM_ENABLE(1) | APP_REQ_RETRY_EN(1))
	str	r1, [r0, #PCIEC_ELBI_CTRL]
	ldr	r1, =0x11000000
	ldr	fp, =0x0
ltssm_stat_loop:
	add	fp, fp, #1
	cmp	fp, ip
	bgt	1f
	ldr	r2, [r0, #PCIEC_ELBI_CTRL]
	and	r2, r2, #LTSSM_STATE_MASK
	cmp	r2, r1
	bne	ltssm_stat_loop

	/* Initiail CDM reigsters */
	ldr	r5, =PCIEC_DBI_BAR_MASK_BASE  @ BAR Mask Register
	ldr	r6, =PCIEC_DBI_BASE

	ldr	r1, =BAR_ENABLED(1)
	str	r1, [r5, #DBI_BAR0_MASK]   /* enable BAR0 */
	ldr	r1, =0x1ffffff             /* 32 MB */
	str	r1, [r5, #DBI_BAR0_MASK]
	ldr	r1, =0x0                   /* non-prefetchable(1'b0) / 32-bit(2'b00) / memory space(1'b0) */
	str	r1, [r6, #PCI_BASE_ADDRESS_0]

	ldr	r1, =BAR_ENABLED(1)
	str	r1, [r5, #DBI_BAR1_MASK]   /* enable BAR1 */
	ldr	r1, =0xff                  /* 256 Bytes */
	str	r1, [r5, #DBI_BAR1_MASK]
	ldr	r1, =0x1                   /* I/O space(1'b1) */
	str	r1, [r6, #PCI_BASE_ADDRESS_1]

	ldr	r1, =BAR_ENABLED(0)
	str	r1, [r5, #DBI_BAR2_MASK]   /* disable BAR2 */
	str	r1, [r5, #DBI_BAR3_MASK]   /* disable BAR3 */
	str	r1, [r5, #DBI_BAR4_MASK]   /* disable BAR4 */

	ldr	r1, =BAR_ENABLED(1)
	str	r1, [r5, #DBI_BAR5_MASK]   /* enable BAR5 */
	ldr	r1, =0xfff                 /* 4096 Bytes */
	str	r1, [r5, #DBI_BAR5_MASK]
	ldr	r1, =0x0                   /* non-prefetchable(1'b0) / 32-bit(2'b00) / memory space(1'b0) */
	str	r1, [r6, #PCI_BASE_ADDRESS_5]


	ldr	r1, [r0, #PCIEC_ELBI_CTRL]
	bic	r1, r1, #APP_REQ_RETRY_EN(1)
	str	r1, [r0, #PCIEC_ELBI_CTRL]
1:
	/* return */
	ldmfd	sp!, {r0-r2, r5, r6, fp, ip, pc}
