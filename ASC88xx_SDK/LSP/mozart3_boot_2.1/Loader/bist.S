/*
 * bist.S
 *
 * Copyright 2013 VN Inc.  All rights reserved.
 *
 */
#include <loader.h>

memset:	//r0:ptr, r1:value, r2:length
	stmfd sp!, {r0-r10, lr}
	add r3, r0, r2
cping:
	cmp 	r0, r3
	strlt	r1, [r0] 
	add		r0, r0, #4
	blt	cping
	
	ldmfd		sp!, {r0-r6, pc}
 
/*******************************************************************************
 * TestBIST()
 ******************************************************************************/
	.global TestBIST
TestBIST:
	stmfd	sp!, {r0-r6, lr}

	ldr r0, =VPL_APB3C_DEV_MMR_BASE
	
	ldr	r1, [r0, #PGCR1]
	bic r1, #PHYHRST(1)
	str	r1, [r0, #PGCR1]
	ldr	r1, [r0, #PGCR1]
	orr r1, #PHYHRST(1)
	str	r1, [r0, #PGCR1]
	
	ldr r1, =BINST(3)
	str	r1, [r0, #BISTRR]
	ldr r1, =BINST(0)
	str	r1, [r0, #BISTRR]
	
	/* configure BIST */
	ldr r10,=BDXSEL(0)
	
DataByteLaneTest:
	ldr r2, =(BDPAT(1) | BDXEN(1) | BSONF(1) | NFAIL(1) | BMODE(1))
	orr r1, r2, r10
	str	r1, [r0, #BISTRR]
	
	ldr r1, =0x20
	str	r1, [r0, #BISTWCR]
	
	ldr r1, =0
	str	r1, [r0, #BISTMSKR0]
	str	r1, [r0, #BISTMSKR1]
	str	r1, [r0, #BISTMSKR2]
	
	ldr r1, =0x1234ABCD
	str	r1, [r0, #BISTLSR]
	
	ldr r1, =BCOL(0) | BROW(0) | BBANK(0)
	str	r1, [r0, #BISTAR0]
	
	cmp r10, #BDXSEL(1)
	ldrle r1, = BAINC(0x8) | BMRANK(1) | BRANK(0)
	ldrgt r1, = BAINC(0x8) | BMRANK(1) | BRANK(1)
	str	r1, [r0, #BISTAR1]
	
	ldr	r1, =BMBANK(0x7) | BMROW(0x3FFF) | BMCOL(0x3FF)
	str	r1, [r0, #BISTAR2]
	
	ldr	r1, =0x55AA00FF
	str	r1, [r0, #BISTUDPR]
	
	/* Start BIST */
	ldr r1, =BINST(1)
	str	r1, [r0, #BISTRR]
	
BISTing:
	ldr r1, [r0, #BISTGSR]
	tst r1, #BDONE(1)
	beq BISTing
	
	
	add r10, r10, #BDXSEL(1)
	cmp r10, #BDXSEL(5)
	bne DataByteLaneTest
	
	/* return */
	ldmfd		sp!, {r0-r6, pc}
	
/*******************************************************************************
 * InitBIST()
 ******************************************************************************/
	.global InitBIST
InitBIST:
	stmfd	sp!, {r0-r6, lr}
	ldr r1, =0
	
  ldr r0, =VPL_DDRSDMC_0_MMR_BASE
  str r1, [r0,#0x28]
  str r1, [r0,#0x2c]
  str r1, [r0,#0x30]
  str r1, [r0,#0x34]
  str r1, [r0,#0x38]

  ldr r0, =VPL_DDRSDMC_1_MMR_BASE
  str r1, [r0,#0x28]
  str r1, [r0,#0x2c]
  str r1, [r0,#0x30]
  str r1, [r0,#0x34]
  str r1, [r0,#0x38]
  
	ldr r1, =0x12345678
CHECK_0:
  ldr r0, =VPL_DDRSDMC_0_MEM_BASE
  str r1, [r0,#0]
  ldr r2, [r0,#0]
  cmp r2, r1
  bne CHECK_0
  
	ldr r1, =0x12345678
CHECK_1:
  ldr r0, =VPL_DDRSDMC_1_MEM_BASE
  str r1, [r0,#0]
  ldr r2, [r0,#0]
  cmp r2, r1
  bne CHECK_1
  
  ldr r0, =VPL_DDRSDMC_0_MMR_BASE
  ldr r1, [r0,#DDR_CTRL]
  bic r1, #0x800
  str r1, [r0,#DDR_CTRL]
  
  ldr r0, =VPL_DDRSDMC_0_MMR_BASE
  ldr r1, [r0,#DDR_CTRL]
  bic r1, #BYP_PUB_EN(1)					// turn off bypass mode
  str r1, [r0,#DDR_CTRL]
  
  ldr r0, =VPL_DDRSDMC_1_MMR_BASE
  ldr r1, [r0,#DDR_CTRL]
  bic r1, #BYP_PUB_EN(1)
  str r1, [r0,#DDR_CTRL]
  
	
	/* return */
	ldmfd		sp!, {r0-r6, pc}
	
/*******************************************************************************
 * BIST_main()
 ******************************************************************************/	
	.global BIST_main
BIST_main: stmfd		sp!, {r0-r2, lr}

	bl 		InitBIST									// Run fine delay Test
	bl 		TestBIST									// Run fine delay Test
	
	/* return */
	ldmfd		sp!, {r0-r2, pc}


	.ltorg
