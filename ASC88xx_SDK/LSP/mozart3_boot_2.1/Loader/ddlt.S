/*
 * ddlt.S
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
 * TestDDL( targetDelayLine, coarsedelay, finedelay)
 * targetDelayLine: 						r3
 *			target delay line
 * coarsedelay									r1
 				testing coarse delay
 * finedelay:										r2
 *			testing tested delay
 ******************************************************************************/
	.global TestDDL
TestDDL:
	stmfd	sp!, {r0-r6, lr}

	//setting delay
	mov 		r1, r1, LSL r5		// fine delay
	mov 		r2, r2, LSL r5
	mov 		r2, r2, LSL#3			// coarse delay
	orr 	r1, r1, r2
	str 	r1, [r0, r3]				// PUB
	
	ldr		r4, [r0, #PGCR0]
	orr		r4, r4,  #OSCEN(1)	//Enable oscillations
	str		r4, [r0, #PGCR0]
	
	ldr		r1, [r0, #PGCR0]
	orr		r1, r1,  #DLTST(1)	//Trigger test measurement
	str		r1, [r0, #PGCR0]
	
#if 0
0:
	ldr		r1, [r0, #PGCR0]
	ands	r1, r1, #DLTST(1)		//Poll for test complete
	bne 	0b
#endif
	
	//Read delay value code
	cmp		r3, #ACBDLR					//AC or DATAX
	bgt 	_DATAXING
_ACING:
	ldr		r1, [r0, #PGSR1]
	tst 	r1, #DLTDONE(1)		//Poll for test complete
	beq 	_ACING
	ldr		r4,		=DLTCODE(0xFFFFFF)
	and		r9, r1, r4		//Read delay value code
	b 		_TESTDONE

_DATAXING:
	cmp 	r3,	#DX0GSR1
	ldrgt	r1, [r0, #DX0GSR1]			// Dx
	cmp 	r3,	#DX1GSR1
	ldrgt	r1, [r0, #DX1GSR1]
	cmp 	r3,	#DX2GSR1
	ldrgt	r1, [r0, #DX2GSR1]
	cmp 	r3,	#DX3GSR1
	ldrgt	r1, [r0, #DX3GSR1]
	
	tst		r1, #DLTDONE(1)			//Poll for test complete
	beq 	_DATAXING
	ldr		r4,		=DLTCODE(0xFFFFFF)
	and		r9, r1, r4		//Read delay value code
	
_TESTDONE:
	ldr		r1, [r0, #PGCR0]
	bic		r1, r1,  #DLTST(1)	// stop test
	str		r1, [r0, #PGCR0]
	
	ldr		r1, [r0, #PGCR0]
	bic		r1, r1,  #OSCEN(1)	//Disable oscillations
	str		r1, [r0, #PGCR0]
	
	//Set DDL to minimum delay
	ldr		r1, =0
	str 	r1, [r0, r3]				//clear delay 
	str 	r1, [r0, r3]				//clear delay code
		
	/* return */
	ldmfd		sp!, {r0-r6, pc}
	
	
/*******************************************************************************
 * DDL_main()
 ******************************************************************************/	
	.global DDL_main
DDL_main:
	stmfd		sp!, {r0-r2, lr}

#if 0
		mov 	r0, #0x2000
		mov		r1, #0xFFFFFFFF
		mov		r2, #0x200
		bl		memset
#endif

#if 1
    LDR r0, =VPL_DDR32PHY_PUB_BASE
    LDR r1, [r0,#0x10]
    LDR r2, =0x00000001
WAIT_CMPT_0:	
    NOP
    NOP
    NOP
    LDR r1, [r0,#0x10]
    AND r1, r1, r2
    CMP r1, r2
    BNE WAIT_CMPT_0

    LDR r0, =VPL_DDR32PHY_PUB_BASE
    LDR r1, =0x00000073
    STR r1, [r0,#0x04]
    LDR r1, [r0,#0x10]
    LDR r2, =0x00000001
WAIT_CLEAR_LOOP_0:
    NOP
    NOP
    NOP
    LDR r1, [r0,#0x10]
    AND r1, r1, r2
    CMP r1, #0
    BNE WAIT_CLEAR_LOOP_0
    
    LDR r1, [r0,#0x10]
    LDR r2, =0x00000001 
WAIT_CMPT_1:
    NOP
    NOP
    NOP
    LDR r1, [r0,#0x10]
    AND r1, r1, r2
    CMP r1, r2
    BNE WAIT_CMPT_1
    
    LDR r0, =VPL_DDRSDMC_0_MMR_BASE
    LDR r1, [r0,#0x04]
	AND r1, r1, #0x100
    STR r1, [r0,#0x04]
    LDR r0, =VPL_DDR32PHY_PUB_BASE
    LDR r1, =0x00001890
    STR r1, [r0,#0x3C]
    LDR r1, =0x02804413
    STR r1, [r0,#0x0C]
    LDR r1, =0xA8003040
    STR r1, [r0,#0x08]
    LDR r1, =0xA8003040
    STR r1, [r0,#0x08]
    LDR r1, =0x00000000
    STR r1, [r0,#0x30]
    LDR r1, =0x00000000
    STR r1, [r0,#0x34]
#if 1
    LDR r1, =0x00FF0000
    STR r1, [r0,#0x30]
    LDR r1, =0x00FFFFFF
    STR r1, [r0,#0x34]
#else
    LDR r1, =0x3FFFFFFF
    STR r1, [r0,#0x28C]
    LDR r1, =0x3FFFFFFF
    STR r1, [r0,#0x290]
    LDR r1, =0x00FFFFFF
    STR r1, [r0,#0x294]
    LDR r1, =0x3FFFFFFF
    STR r1, [r0,#0x298]
    LDR r1, =0x00FFFFFF
    STR r1, [r0,#0x29c]
    
    LDR r1, =0xFFFFFFFF
    STR r1, [r0,#0x2a0]
    LDR r1, =0x00FFFFFF
    STR r1, [r0,#0x2a4]
    LDR r1, =0xFFFFFFFF
    STR r1, [r0,#0x2a8]
    LDR r1, =0x00FF0000
    STR r1, [r0,#0x2ac]
#endif

    LDR r1, =0xA804fE40
    STR r1, [r0,#0x08]
    LDR r1, =0xA804fF40
    STR r1, [r0,#0x08]
    LDR r1, =0xA804fFC0
    STR r1, [r0,#0x08]
    LDR r1, [r0,#0x14]
    LDR r2, =0x00000001
WAIT_CMPT_2:
    NOP
    NOP
    NOP
    LDR r1, [r0,#0x14]
    AND r1, r1, r2
    CMP r1, r2
    BNE WAIT_CMPT_2
#endif

		mov 	r10, #0x0
		mov 	r11, #0x0
		ldr			r0, =VPL_DDR32PHY_PUB_BASE
		
	//Turn off DDL VT drift status from being generated
	ldr			r1, [r0, #PGCR1]
	ldr 		r2,		=DLDLMT(0xFF)
	bic			r1, r1, r2
	str			r1, [r0, #PGCR1]
	
	ldr			r1, [r0, #PGCR0]
	//Configures the DDLs to connect in a ring for testing.
	orr			r1, r1,  #DLTMODE(1)
	//Specifies the factor by which the delay line oscillator mode output is divided down before it is output on the delay line digital test output pin dl_dto.
	bic 		r1, r1,  #OSCDIV(7)				// clear field
	orr			r1, r1,  #OSCDIV(7)				// 0: /1, 7: /65536
	str			r1, [r0, #PGCR0]

	//Set all DDL delay codes to minimum
	bl 			_SetAllDelayTo0

	// Selects the PHY digital test output that should be driven onto PHY digital test output(phy_dto)pin.
	ldr			r1, [r0, #PGCR0]
	orr			r1, r1,  #DTOSEL(0x19)	//19:AC, 18~10:Dx8~Dx0
	str			r1, [r0, #PGCR0]
		
	//for all DLLs
	
	mov 		r8, #0x2000													// r8 point to save record
	adr 		r4, _TestDelayLineSequence					// Run through all DLLs
	ldr 		r3, [r4, #0]
	adrl 		r6, _TestDelayLineBitFieldSequence	// 
	ldr 		r5, [r6, #0]
	
SelectsDelayLines:
	ldr 		r1, =0
	ldr 		r2, =0
FineDelaysChanging:
	bl 		TestDDL									// Run fine delay Test
	
	cmp			r8, #0x2200
	movgt		r8, #0x2000
	cmp			r9, #0x100000
	strgt			r9, [r8]
	addgt			r10, r10, #1
	addgt			r8, r8, #4
	cmp			r9, #0
	addeq			r11, r11, #1
	
	add			r1, r1, #1
	cmp			r1, #8
	bne			FineDelaysChanging
		
	// Set fine delay to minimum

	ldr 		r1, =0
	ldr 		r2, =0
CoarseDelaysChanging:
	bl 			TestDDL									// Run coarse delay test
	
	cmp			r8, #0x2200
	movgt		r8, #0x2000
	cmp			r9, #0x100000
	strgt			r9, [r8]
	addgt			r10, r10, #1
	addgt			r8, r8, #4
	cmp			r9, #0
	addeq			r11, r11, #1
	
	add 		r2, r2, #1
	cmp 		r2, #16
	bne 		CoarseDelaysChanging
		
	//Set DDL to minimum delay
	
	// for all delay lines
	add			r4, r4, #4
	ldr			r3, [r4, #0]
	add 		r6, r6, #4	// 
	ldr 		r5, [r6, #0]
	
#if 1
	ldr			r1, [r0, #PGCR0]
	
	cmp			r4, #ACMDLR
	bic			r1, r1, #DTOSEL(0x1F)			// 0
	orrlt			r1, r1,  #DTOSEL(0x19)	//19:AC, 18~10:Dx8~Dx0
	orrgt		r1, r1,  #DTOSEL(0x10)	//19:AC, 18~10:Dx8~Dx0
	
	cmp			r4, #DX0MDLR
	bic			r1, r1, #DTOSEL(0x1F)			// 0
	orrgt		r1, r1,  #DTOSEL(0x11)	//19:AC, 18~10:Dx8~Dx0
	
	cmp			r4, #DX1MDLR
	bic			r1, r1, #DTOSEL(0x1F)			// 0
	orrgt		r1, r1,  #DTOSEL(0x12)	//19:AC, 18~10:Dx8~Dx0
	
	cmp			r4, #DX2MDLR
	bic			r1, r1, #DTOSEL(0x1F)			// 0
	orrgt		r1, r1,  #DTOSEL(0x13)	//19:AC, 18~10:Dx8~Dx0
	
	str			r1, [r0, #PGCR0]
#endif
	
	adr 		r7, _TestDelayLineSequenceEnd	//4*34	
	cmp 		r4, r7	
	bne 		SelectsDelayLines
	
	ldr			r9, =0x82451282
	str			r9, [r8], #4
	str			r10, [r8], #4
	str			r11, [r8]
	
	ldr			r1, [r0, #PGCR0]
	bic			r1, r1,  #DLTMODE(1)
	bic			r1, r1,  #DTOSEL(0x1F)	//19:AC, 18~10:Dx8~Dx0
	str			r1, [r0, #PGCR0]
	
	/* return */
	ldmfd		sp!, {r0-r2, pc}

/*******************************************************************************
 * _SetAllDelayTo0()
 ******************************************************************************/	
	.global _SetAllDelayTo0
_SetAllDelayTo0:
	ldr	r1, =0x00000000
	str	r1, [r0, #ACMDLR]
	str	r1, [r0, #ACBDLR]

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
	str	r1, [r0, #DX0MDLR]
	
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
	str	r1, [r0, #DX1MDLR]
	
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
	str	r1, [r0, #DX2MDLR]
	
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
	str	r1, [r0, #DX3MDLR]
	
#if 1
	/* DX4BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX4BDLR0]
	str	r1, [r0, #DX4BDLR1]
	str	r1, [r0, #DX4BDLR2]
	str	r1, [r0, #DX4BDLR3]
	str	r1, [r0, #DX4BDLR4]
	/* DX4LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX4LCDLR0]
	str	r1, [r0, #DX4LCDLR1]
	str	r1, [r0, #DX4LCDLR2]
	str	r1, [r0, #DX4MDLR]
	
	/* DX5BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX5BDLR0]
	str	r1, [r0, #DX5BDLR1]
	str	r1, [r0, #DX5BDLR2]
	str	r1, [r0, #DX5BDLR3]
	str	r1, [r0, #DX5BDLR4]
	/* DX5LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX5LCDLR0]
	str	r1, [r0, #DX5LCDLR1]
	str	r1, [r0, #DX5LCDLR2]
	str	r1, [r0, #DX5MDLR]
	
	/* DX6BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX6BDLR0]
	str	r1, [r0, #DX6BDLR1]
	str	r1, [r0, #DX6BDLR2]
	str	r1, [r0, #DX6BDLR3]
	str	r1, [r0, #DX6BDLR4]
	/* DX6LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX6LCDLR0]
	str	r1, [r0, #DX6LCDLR1]
	str	r1, [r0, #DX6LCDLR2]
	str	r1, [r0, #DX6MDLR]
	
	/* DX7BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX7BDLR0]
	str	r1, [r0, #DX7BDLR1]
	str	r1, [r0, #DX7BDLR2]
	str	r1, [r0, #DX7BDLR3]
	str	r1, [r0, #DX7BDLR4]
	/* DX7LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX7LCDLR0]
	str	r1, [r0, #DX7LCDLR1]
	str	r1, [r0, #DX7LCDLR2]
	str	r1, [r0, #DX7MDLR]
	
	/* DX8BDLR0/1/2/3/4, all BDL are 0 */
	ldr	r1, =0x00000000
	str	r1, [r0, #DX8BDLR0]
	str	r1, [r0, #DX8BDLR1]
	str	r1, [r0, #DX8BDLR2]
	str	r1, [r0, #DX8BDLR3]
	str	r1, [r0, #DX8BDLR4]
	/* DX8LCDLR0/1/2, all LCDL are 0 */
	str	r1, [r0, #DX8LCDLR0]
	str	r1, [r0, #DX8LCDLR1]
	str	r1, [r0, #DX8LCDLR2]
	str	r1, [r0, #DX8MDLR]
#endif
	mov pc, lr

_TestDelayLineSequence:
	.word ACMDLR
	.word ACBDLR
	.word ACBDLR
	.word ACBDLR
	.word ACBDLR    
	
	.word DX0LCDLR0
	.word DX0LCDLR1
	.word DX0LCDLR1
	.word DX0LCDLR2
	.word DX0MDLR
	.word DX0LCDLR1
	.word DX0BDLR0
	.word DX0BDLR0
	.word DX0BDLR0
	.word DX0BDLR0
	.word DX0BDLR0
	.word DX0BDLR1
	.word DX0BDLR1
	.word DX0BDLR1
	.word DX0BDLR1
	.word DX0BDLR1
	.word DX0BDLR2
	.word DX0BDLR2
	.word DX0BDLR3
	.word DX0BDLR3
	.word DX0BDLR3
	.word DX0BDLR3
	.word DX0BDLR3
	.word DX0BDLR4
	.word DX0BDLR4
	.word DX0BDLR4
	.word DX0BDLR4
	.word DX0BDLR2
	.word DX0BDLR2
	
	.word DX1LCDLR0
	.word DX1LCDLR1
	.word DX1LCDLR1
	.word DX1LCDLR2
	.word DX1MDLR
	.word DX1LCDLR1
	.word DX1BDLR0
	.word DX1BDLR0
	.word DX1BDLR0
	.word DX1BDLR0
	.word DX1BDLR0
	.word DX1BDLR1
	.word DX1BDLR1
	.word DX1BDLR1
	.word DX1BDLR1
	.word DX1BDLR1
	.word DX1BDLR2
	.word DX1BDLR2
	.word DX1BDLR3
	.word DX1BDLR3
	.word DX1BDLR3
	.word DX1BDLR3
	.word DX1BDLR3
	.word DX1BDLR4
	.word DX1BDLR4
	.word DX1BDLR4
	.word DX1BDLR4
	.word DX1BDLR2
	.word DX1BDLR2
	
	.word DX2LCDLR0
	.word DX2LCDLR1
	.word DX2LCDLR1
	.word DX2LCDLR2
	.word DX2MDLR
	.word DX2LCDLR1
	.word DX2BDLR0
	.word DX2BDLR0
	.word DX2BDLR0
	.word DX2BDLR0
	.word DX2BDLR0
	.word DX2BDLR1
	.word DX2BDLR1
	.word DX2BDLR1
	.word DX2BDLR1
	.word DX2BDLR1
	.word DX2BDLR2
	.word DX2BDLR2
	.word DX2BDLR3
	.word DX2BDLR3
	.word DX2BDLR3
	.word DX2BDLR3
	.word DX2BDLR3
	.word DX2BDLR4
	.word DX2BDLR4
	.word DX2BDLR4
	.word DX2BDLR4
	.word DX2BDLR2
	.word DX2BDLR2
	
	.word DX3LCDLR0
	.word DX3LCDLR1
	.word DX3LCDLR1
	.word DX3LCDLR2
	.word DX3MDLR
	.word DX3LCDLR1
	.word DX3BDLR0
	.word DX3BDLR0
	.word DX3BDLR0
	.word DX3BDLR0
	.word DX3BDLR0
	.word DX3BDLR1
	.word DX3BDLR1
	.word DX3BDLR1
	.word DX3BDLR1
	.word DX3BDLR1
	.word DX3BDLR2
	.word DX3BDLR2
	.word DX3BDLR3
	.word DX3BDLR3
	.word DX3BDLR3
	.word DX3BDLR3
	.word DX3BDLR3
	.word DX3BDLR4
	.word DX3BDLR4
	.word DX3BDLR4
	.word DX3BDLR4
	.word DX3BDLR2
	.word DX3BDLR2
_TestDelayLineSequenceEnd:
_TestDelayLineBitFieldSequence:
	.word MDLD_SHIFT
	.word CK0BD_SHIFT
	.word CK1BD_SHIFT
	.word CK2BD_SHIFT
	.word ACBD_SHIFT
	
	.word R0WLD_SHIFT
	.word WDQD_SHIFT
	.word RDQSD_SHIFT
	.word R0DQSGD_SHIFT
	.word MDLD_SHIFT
	.word RDQSND_SHIFT
	.word DQ0WBD_SHIFT
	.word DQ1WBD_SHIFT
	.word DQ2WBD_SHIFT
	.word DQ3WBD_SHIFT
	.word DQ4WBD_SHIFT
	.word DQ5WBD_SHIFT
	.word DQ6WBD_SHIFT
	.word DQ7WBD_SHIFT
	.word DMWBD_SHIFT
	.word DSWBD_SHIFT
	.word DSOEBD_SHIFT
	.word DQOEBD_SHIFT
	.word DQ0RBD_SHIFT
	.word DQ1RBD_SHIFT
	.word DQ2RBD_SHIFT
	.word DQ3RBD_SHIFT
	.word DQ4RBD_SHIFT
	.word DQ5RBD_SHIFT
	.word DQ6RBD_SHIFT
	.word DQ7RBD_SHIFT
	.word DMRBD_SHIFT
	.word DSRBD_SHIFT
	.word DSNRBD_SHIFT

	.word R0WLD_SHIFT
	.word WDQD_SHIFT
	.word RDQSD_SHIFT
	.word R0DQSGD_SHIFT
	.word MDLD_SHIFT
	.word RDQSND_SHIFT
	.word DQ0WBD_SHIFT
	.word DQ1WBD_SHIFT
	.word DQ2WBD_SHIFT
	.word DQ3WBD_SHIFT
	.word DQ4WBD_SHIFT
	.word DQ5WBD_SHIFT
	.word DQ6WBD_SHIFT
	.word DQ7WBD_SHIFT
	.word DMWBD_SHIFT
	.word DSWBD_SHIFT
	.word DSOEBD_SHIFT
	.word DQOEBD_SHIFT
	.word DQ0RBD_SHIFT
	.word DQ1RBD_SHIFT
	.word DQ2RBD_SHIFT
	.word DQ3RBD_SHIFT
	.word DQ4RBD_SHIFT
	.word DQ5RBD_SHIFT
	.word DQ6RBD_SHIFT
	.word DQ7RBD_SHIFT
	.word DMRBD_SHIFT
	.word DSRBD_SHIFT
	.word DSNRBD_SHIFT
	
	.word R0WLD_SHIFT
	.word WDQD_SHIFT
	.word RDQSD_SHIFT
	.word R0DQSGD_SHIFT
	.word MDLD_SHIFT
	.word RDQSND_SHIFT
	.word DQ0WBD_SHIFT
	.word DQ1WBD_SHIFT
	.word DQ2WBD_SHIFT
	.word DQ3WBD_SHIFT
	.word DQ4WBD_SHIFT
	.word DQ5WBD_SHIFT
	.word DQ6WBD_SHIFT
	.word DQ7WBD_SHIFT
	.word DMWBD_SHIFT
	.word DSWBD_SHIFT
	.word DSOEBD_SHIFT
	.word DQOEBD_SHIFT
	.word DQ0RBD_SHIFT
	.word DQ1RBD_SHIFT
	.word DQ2RBD_SHIFT
	.word DQ3RBD_SHIFT
	.word DQ4RBD_SHIFT
	.word DQ5RBD_SHIFT
	.word DQ6RBD_SHIFT
	.word DQ7RBD_SHIFT
	.word DMRBD_SHIFT
	.word DSRBD_SHIFT
	.word DSNRBD_SHIFT
	
	
	.word R0WLD_SHIFT
	.word WDQD_SHIFT
	.word RDQSD_SHIFT
	.word R0DQSGD_SHIFT
	.word MDLD_SHIFT
	.word RDQSND_SHIFT
	.word DQ0WBD_SHIFT
	.word DQ1WBD_SHIFT
	.word DQ2WBD_SHIFT
	.word DQ3WBD_SHIFT
	.word DQ4WBD_SHIFT
	.word DQ5WBD_SHIFT
	.word DQ6WBD_SHIFT
	.word DQ7WBD_SHIFT
	.word DMWBD_SHIFT
	.word DSWBD_SHIFT
	.word DSOEBD_SHIFT
	.word DQOEBD_SHIFT
	.word DQ0RBD_SHIFT
	.word DQ1RBD_SHIFT
	.word DQ2RBD_SHIFT
	.word DQ3RBD_SHIFT
	.word DQ4RBD_SHIFT
	.word DQ5RBD_SHIFT
	.word DQ6RBD_SHIFT
	.word DQ7RBD_SHIFT
	.word DMRBD_SHIFT
	.word DSRBD_SHIFT
	.word DSNRBD_SHIFT
	
	.ltorg