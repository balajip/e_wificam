#include <loader.h>

/*******************************************************************************
 * ddrsdmc_preinit()
 ******************************************************************************/
	.global ddrsdmc_preinit	
ddrsdmc_preinit:
	stmfd	sp!, {r0-r1, lr}

	/* ----------------- 
	 * DDR 0
	 * ---------------*/	
	ldr	r0, =VPL_DDRSDMC_0_MMR_BASE

#if CONFIG_DDR_PORT_0 == 1
	ldr	r1, =DDR0_SIZE_DATA
	str	r1, [r0, #DDR_SIZE]
	ldr	r1, =DDR0_LOAD_MODE_0_1_DATA
	str	r1, [r0, #DDR_LOAD_MODE_0_1]
	ldr	r1, =DDR0_LOAD_MODE_2_3_DATA
	str	r1, [r0, #DDR_LOAD_MODE_2_3]
	
#ifdef DYNAMIC_LOAD_TIMING
	ldr	r0, =0x4f80003c
	ldr	ip, [r0]
	and	ip, ip, #0x7   @ ip = model
	ldr	r0, =VPL_DDRSDMC_0_MMR_BASE
	
	cmp	ip, #0x0 @ 390A
	adreq	r1, _390A_timing
	cmp	ip, #0x2 @ 390/385
	adreq	r1, _390_timing
	cmp	ip, #0x3 @ 370
	adreq	r1, _370_timing
	cmp	ip, #0x5 @ 330
	adreq	r1, _330_timing
	ldm	r1, {r6-r9}
	str	r6, [r0, #DDR_TIMING0]
	str	r7, [r0, #DDR_TIMING1]
	str	r8, [r0, #DDR_TIMING2]
	str	r9, [r0, #DDR_TIMING3]
#else
	ldr	r1, =DDR0_TIMING0_DATA
	str	r1, [r0, #DDR_TIMING0]
	ldr	r1, =DDR0_TIMING1_DATA
	str	r1, [r0, #DDR_TIMING1]
	ldr	r1, =DDR0_TIMING2_DATA
	str	r1, [r0, #DDR_TIMING2]
	ldr	r1, =DDR0_TIMING3_DATA
	str	r1, [r0, #DDR_TIMING3]
#endif
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(0) | DDR3_ENABLE(CONFIG_DDR_TYPE) | DYNAMIC_ODT_EN(CFG_0_DYNAMIC_ODT_EN) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r0, #DDR_CTRL]
#else
#if 0 /* TODO: hang if disabled ! design issue !? */
	/* disable SSTL IO */
	ldr	r1, =0x0
	str	r1, [r0, #DDR_IO_BIT_ENABLE]
#endif
#endif /* CONFIG_DDR_PORT_0 */



	/* ----------------- 
	 * DDR 1
	 * ---------------*/	
	ldr	r0, =VPL_DDRSDMC_1_MMR_BASE

#if CONFIG_DDR_PORT_1 == 1
	ldr	r1, =DDR1_SIZE_DATA
	str	r1, [r0, #DDR_SIZE]
	ldr	r1, =DDR1_LOAD_MODE_0_1_DATA
	str	r1, [r0, #DDR_LOAD_MODE_0_1]
	ldr	r1, =DDR1_LOAD_MODE_2_3_DATA
	str	r1, [r0, #DDR_LOAD_MODE_2_3]
	
#ifdef DYNAMIC_LOAD_TIMING	
	ldr	r0, =0x4f80003c
	ldr	ip, [r0]
	and	ip, ip, #0x7   @ ip = model
	ldr	r0, =VPL_DDRSDMC_1_MMR_BASE
	
	cmp	ip, #0x0 @ 390A
	adreq	r1, _390A_timing
	cmp	ip, #0x2 @ 390/385
	adreq	r1, _390_timing
	cmp	ip, #0x3 @ 370
	adreq	r1, _370_timing
	cmp	ip, #0x5 @ 330
	adreq	r1, _330_timing
	ldm	r1, {r6-r9}
	str	r6, [r0, #DDR_TIMING0]
	str	r7, [r0, #DDR_TIMING1]
	str	r8, [r0, #DDR_TIMING2]
	str	r9, [r0, #DDR_TIMING3]
#else
	ldr	r1, =DDR1_TIMING0_DATA
	str	r1, [r0, #DDR_TIMING0]
	ldr	r1, =DDR1_TIMING1_DATA
	str	r1, [r0, #DDR_TIMING1]
	ldr	r1, =DDR1_TIMING2_DATA
	str	r1, [r0, #DDR_TIMING2]
	ldr	r1, =DDR1_TIMING3_DATA
	str	r1, [r0, #DDR_TIMING3]
#endif
	ldr	r1, =(SLAVE_CORE_SYNC_EN(CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1) | \
	              BYP_PUB_EN(0) | DDR3_ENABLE(CONFIG_DDR_TYPE) | DYNAMIC_ODT_EN(CFG_1_DYNAMIC_ODT_EN) | \
		      MCI_IOPHY_UPDATE_EN(1) | EN(1))
	str	r1, [r0, #DDR_CTRL]
#else
#if 0 /* TODO: hang if disabled ! design issue !? */
	/* disable SSTL IO */
	ldr	r1, =0x0
	str	r1, [r0, #DDR_IO_BIT_ENABLE]
#endif
#endif /* CONFIG_DDR_PORT_1 */

#if CONFIG_DDR_PORT_0 == 1
	/* Check DDR 0 power-on */
	ldr	r0, =VPL_DDRSDMC_0_MMR_BASE
0:
	ldr	r1, [r0, #DDR_CTRL]
	ands	r1, r1, #PWR_ON(1)
	beq	0b
#endif
#if CONFIG_DDR_PORT_1 == 1
	/* Check DDR 1 power-on */
	ldr	r0, =VPL_DDRSDMC_1_MMR_BASE
1:
	ldr	r1, [r0, #DDR_CTRL]
	ands	r1, r1, #PWR_ON(1)
	beq	1b
#endif

	/* return */
	ldmfd	sp!, {r0-r1, pc}



/*******************************************************************************
 * ddrsdmc_postinit()
 *
 * must set PWR_ON after it asserted
 *
 * TODO: both PWR_ON bits *MUST* be set, even though there is no DDR0 either DDR1!!!
 *       design issue ?????
 ******************************************************************************/
	.global ddrsdmc_postinit
ddrsdmc_postinit:
	stmfd	sp!, {r0-r1, lr}

	ldr	r0, =VPL_DDRSDMC_0_MMR_BASE
#if CONFIG_DDR_PORT_0 == 1
	ldr	r1, =DDR0_DELAY_DATA
	str	r1, [r0, #DDR_DELAY]
	ldr	r1, =DDR0_ZQ_CTRL_DATA
	str	r1, [r0, #DDR_ZQ_CTRL]
	ldr	r1, =DDR0_PWRDWN_MODE_DATA
	str	r1, [r0, #DDR_PWRDWN_MODE]
	ldr	r1, =DDR0_SELF_REFRESH_DATA
	str	r1, [r0, #DDR_SELF_REFRESH]
	ldr	r1, =DDR0_PWR_SELF_TIMING_DATA
	str	r1, [r0, #DDR_PWR_SELF_TIMING]
	ldr	r1, =DDR0_IO_DYNAMIC_CTRL_DATA
	str	r1, [r0, #DDR_IO_DYNAMIC_CTRL]
	ldr	r1, =DDR0_IO_BIT_ENABLE_DATA
	str	r1, [r0, #DDR_IO_BIT_ENABLE]
	ldr	r1, =DDR0_DFI_CTRL_DATA
	str	r1, [r0, #DDR_DFI_CTRL]
#endif
	ldr	r1, =(DDR0_CTRL_DATA | PWR_ON(1) | EN(CONFIG_DDR_PORT_0))
	str	r1, [r0, #DDR_CTRL]

	ldr	r0, =VPL_DDRSDMC_1_MMR_BASE
#if CONFIG_DDR_PORT_1 == 1
	ldr	r1, =DDR1_DELAY_DATA
	str	r1, [r0, #DDR_DELAY]
	ldr	r1, =DDR1_ZQ_CTRL_DATA
	str	r1, [r0, #DDR_ZQ_CTRL]
	ldr	r1, =DDR1_PWRDWN_MODE_DATA
	str	r1, [r0, #DDR_PWRDWN_MODE]
	ldr	r1, =DDR1_SELF_REFRESH_DATA
	str	r1, [r0, #DDR_SELF_REFRESH]
	ldr	r1, =DDR1_PWR_SELF_TIMING_DATA
	str	r1, [r0, #DDR_PWR_SELF_TIMING]
	ldr	r1, =DDR1_IO_DYNAMIC_CTRL_DATA
	str	r1, [r0, #DDR_IO_DYNAMIC_CTRL]
	ldr	r1, =DDR1_IO_BIT_ENABLE_DATA
	str	r1, [r0, #DDR_IO_BIT_ENABLE]
	ldr	r1, =DDR1_DFI_CTRL_DATA
	str	r1, [r0, #DDR_DFI_CTRL]
#endif
	ldr	r1, =(DDR1_CTRL_DATA | PWR_ON(1) | EN(CONFIG_DDR_PORT_1))
	str	r1, [r0, #DDR_CTRL]

	/* return */
	ldmfd	sp!, {r0-r1, pc}

#ifdef DYNAMIC_LOAD_TIMING
_390A_timing:
	.word M390A_DDR0_TIMING0_DATA
	.word M390A_DDR0_TIMING1_DATA
	.word M390A_DDR0_TIMING2_DATA
	.word M390A_DDR0_TIMING3_DATA
_390_timing:
	.word M390_DDR0_TIMING0_DATA
	.word M390_DDR0_TIMING1_DATA
	.word M390_DDR0_TIMING2_DATA
	.word M390_DDR0_TIMING3_DATA
_370_timing:
	.word M370_DDR0_TIMING0_DATA
	.word M370_DDR0_TIMING1_DATA
	.word M370_DDR0_TIMING2_DATA
	.word M370_DDR0_TIMING3_DATA
_330_timing:
	.word M330_DDR0_TIMING0_DATA
	.word M330_DDR0_TIMING1_DATA
	.word M330_DDR0_TIMING2_DATA
	.word M330_DDR0_TIMING3_DATA
#endif
	.ltorg
