/*
 * loader.S
 *
 * Copyright 2011 VN Inc.  All rights reserved.
 *
 */

#include <loader.h>

/* millisecond delay */
#define MS_DELAY_CNT_133MHZ_APB   0x20780
#define MS_DELAY_CNT_100MHZ_APB   0x186A0

	.macro  mdelay m
	ldr		r1, =\m
	ldr		r2, =VPL_SYSC_MMR_BASE
0:
	ldr		r3, =0x80000000
	str		r3, [r2, #SYSC_CNT_CTRL]   @ clear cnt
1:
	ldr		r3, [r2, #SYSC_CNT_LOW]
	ldr		r4, =MS_DELAY_CNT_100MHZ_APB
	cmp		r3, r4
	blo		1b
	subs		r1, r1, #1
	bne		0b
	.endm

	.global _start
_start:
#if defined(READYGO) && READYGO == 1
	b	.
#else
	nop
#endif

/*=============================================================================
 * AHB slaves
 *============================================================================*/
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_USBC_MMR))
	ldr	r0, =(VPL_USBC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_PCIEC_MMR))
	ldr	r0, =(VPL_PCIEC_MMR_BASE)
	str	r0, [r5]
	
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_MSHC_0_MEM))
	ldr	r0, =(VPL_MSHC_0_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_MSHC_1_MEM))
	ldr	r0, =(VPL_MSHC_1_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_GMAC_MEM))
	ldr	r0, =(VPL_GMAC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_CAST_NFC_MEM))
	ldr	r0, =(VPL_NFC_MMR_BASE)
	str	r0, [r5]
	
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_PCIEC_MEM))
	ldr	r0, =(VPL_PCIEC_MEM_BASE+0xFF)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_APB3C_MMR))
	ldr	r0, =(VPL_APB3C_DEV_MMR_BASE+0xFF)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_APBC_MMR_MEM))
	ldr	r0, =(VPL_APBC_DEV_MMR_BASE+0xFF)
	str	r0, [r5]
	
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_1_MEM))
	ldr	r0, =(VPL_DDRSDMC_1_MEM_BASE+0xFF)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_0_MEM))
	ldr	r0, =(VPL_DDRSDMC_0_MEM_BASE+0x3)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_1_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_1_MEM))
	ldr	r0, =(VPL_DDRSDMC_1_MEM_BASE+0xFF)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_1_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_0_MEM))
	ldr	r0, =(VPL_DDRSDMC_0_MEM_BASE+0xFF)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_2_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_1_MEM))
	ldr	r0, =(VPL_DDRSDMC_1_MEM_BASE+0xFF)
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_2_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_0_MEM))
	ldr	r0, =(VPL_DDRSDMC_0_MEM_BASE+0xFF)
	str	r0, [r5]
	
/*=============================================================================
 * APB slaves
 *============================================================================*/
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_SSIC))
	ldr	r0, =(VPL_SSI_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_I2SSC0))
	ldr	r0, =(VPL_I2S_0_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_I2SSC1))
	ldr	r0, =(VPL_I2S_1_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_I2SSC2))
	ldr	r0, =(VPL_I2S_2_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_I2SSC3))
	ldr	r0, =(VPL_I2S_3_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_I2SSC4))
	ldr	r0, =(VPL_I2S_4_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_USBC))
	ldr	r0, =(VPL_USB_PHY_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_PCIEC))
	ldr	r0, =(VPL_PCIEC_SSC_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_TMRC))
	ldr	r0, =(VPL_TMRC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_WDTC))
	ldr	r0, =(VPL_WDTC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_GPIOC))
	ldr	r0, =(VPL_GPIOC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_AGPOC))
	ldr	r0, =(VPL_AGPOC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_UARTC0))
	ldr	r0, =(VPL_UARTC0_MMR_BASE+0x4)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_UARTC1))
	ldr	r0, =(VPL_UARTC1_MMR_BASE+0x4)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_UARTC2))
	ldr	r0, =(VPL_UARTC2_MMR_BASE+0x4)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_UARTC3))
	ldr	r0, =(VPL_UARTC3_MMR_BASE+0x4)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_IRDAC))
	ldr	r0, =(VPL_IRDAC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_ABITSPLLC))
	ldr	r0, =(VPL_PLLC_MMR_BASE)
	str	r0, [r5]
	ldr	r5, =(VPL_APBC_MMR_BASE + APBC_SLAVE_BASESIZE(APBC_SLAVE_SYSC))
	ldr	r0, =(VPL_SYSC_MMR_BASE)
	str	r0, [r5]

	ldr	sp, =__fdstack

#if CONFIG_PCIE_EP == 1
	bl	ep_init
#endif /* PCIEC_EP */


/*=============================================================================
 * Initial Settings / Reduce power consumption
 *============================================================================*/
	ldr	r2, =VPL_SYSC_MMR_BASE
	ldr	r0, =SYSC_MON_CLK_SEL_DATA
	str	r0, [r2, #SYSC_MON_CLK_SEL]
	ldr	r0, =SYSC_SLEW_CTRL_DATA
	str	r0, [r2, #SYSC_SLEW_CTRL]
	ldr	r0, =SYSC_PULL_CTRL_DATA
	str	r0, [r2, #SYSC_PULL_CTRL]
	ldr	r0, =SYSC_DRV_STRENGTH_CTRL_0_DATA
	str	r0, [r2, #SYSC_DRV_STRENGTH_CTRL_0]
	ldr	r0, =SYSC_DRV_STRENGTH_CTRL_1_DATA
	str	r0, [r2, #SYSC_DRV_STRENGTH_CTRL_1]
	ldr	r0, =SYSC_DRV_STRENGTH_CTRL_2_DATA
	str	r0, [r2, #SYSC_DRV_STRENGTH_CTRL_2]

	ldr	r0, =VPL_DDR32PHY_PUB_BASE
	bl	pub_preinit
	bl	ddrsdmc_preinit

       //bl    DDL_main

#if (CONFIG_0_PUB_BUILTIN_TRAINING == 1) || (CONFIG_1_PUB_BUILTIN_TRAINING == 1)
	ldr	r0, =VPL_DDR32PHY_PUB_BASE
	bl	pub_builtin_training
#elif CONFIG_PUB_SW_TRAINING == 1
	ldr	r0, =VPL_DDR32PHY_PUB_BASE
	bl	pub_sw_training
#endif

	bl	ddrsdmc_postinit

	ldr	r0, =VPL_DDR32PHY_PUB_BASE
	bl	pub_postinit

	//bl	BIST_main

	/***** Here are the SYSC normal settings *****/
	ldr	r2, =VPL_SYSC_MMR_BASE
	/* set clock enable control register 0 */
	ldr	r0, =SYSC_CLK_EN_CTRL_0_DATA
	str	r0, [r2, #SYSC_CLK_EN_CTRL_0]
	/* set clock enable control register 1 */
	ldr	r0, =SYSC_CLK_EN_CTRL_1_DATA
	str	r0, [r2, #SYSC_CLK_EN_CTRL_1]
	/* set pad enable control register */
	ldr	r0, =SYSC_PAD_EN_CTRL_DATA
	str	r0, [r2, #SYSC_PAD_EN_CTRL]

#ifdef DYNAMIC_LOAD_TIMING	
	ldr	r7, =0x49000000 
	ldr	r8, =0x000fffff
	str	r8, [r7, #0x0C]         @set all GPIO to be output

	@ Version control
	ldr	r8, = 0x82
	str	r8, [r7, #0x4]		@just lighting LED
	mdelay(500);
	ldr	r8, = 0x80
	str	r8, [r7, #0x4]		@just lighting LED
	mdelay(500);
	ldr	r8, = 0x80
	str	r8, [r7, #0x4]		@just lighting LED
	mdelay(500);
	ldr	r8, = 0x81
	str	r8, [r7, #0x4]		@just lighting LED
	mdelay(500);

	@ FT2 step first one
	ldr	r8, = 0x1
	str	r8, [r7, #0x4]		@just lighting LED
#endif

	/* add version number to SRAM base */
	ldr	sl, =LOADER_VERSION_ADDR
	ldr	r2, =LOADER_VERSION
	str	r2, [sl]
	/* keep boot device info */
	ldr	r2, =WHERE_IS_BOOTLOADER
	str	r2, [sl, #4]
	/* keep configuration version */
	ldr	r2, =CONFIG_VERSION
	str	r2, [sl, #8]

/*=============================================================================
 * Self-relocation
 *============================================================================*/
	/* copy Loader from SRAM to system DRAM */
	ldr	r1, =VPL_BRC_RAM_BASE     /* source address */
	ldr	r0, =SYSTEM_DDR_MEM
	add	r0, r0, r1         /* destination address */
	ldr	r2, =LOADER_SIZE   /* end address */
	add	r2, r2, r1
COPY_LOOP_1:
	ldmia	r1!, {r3-r10}
	stmia	r0!, {r3-r10}
	cmp	r1, r2
	blo	COPY_LOOP_1

/*..............................................................................
 *.............................   Boot_Do_Remap   ..............................
 */
#if SYSTEM_DDR_MEM == VPL_DDRSDMC_0_MEM_BASE
	ldr	r0, =(VPL_AHBC_0_MMR_BASE + AHBC_CTRL)
	ldr	r2, =REMAP(1)
	str	r2, [r0]
#else
	/* copy remapper from BRC SRAM to USB FIFO */
	ldr	r1, =__remapper_start
	ldr	r0, =VPL_USBC_RAM_BASE
	ldr	r2, =__remapper_size
	add	r2, r2, r1
0:
	ldmia	r1!, {r3-r10}
	stmia	r0!, {r3-r10}
	cmp	r1, r2
	blo	0b

	/* jump to USB SRAM */
	ldr	r11, =VPL_USBC_RAM_BASE
	mov	pc, r11
_remap:
#endif

	/* remapping AHB addresses */
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_0_MEM))
1:
	mov	r4, r5
	ldr	r0, =(CFG_0_BASE_ADDR & 0xFFF00000 + 0x1ff)
	str	r0, [r4], #-4
	ldr	r0, =(CFG_1_BASE_ADDR & 0xFFF00000 + 0x1ff)
	str	r0, [r4]
	cmp	r4, #VPL_AHBC_2_MMR_BASE
	addlo	r5, r5, #0x01000000 /* AHBC_n_MMR_BASE offset */
	blo	1b


/*..............................................................................
 *.............................   Copy U-Boot   ................................
 */
	ldr	r0, =ARMBOOT_DESTADDR
	bl	_copy_uboot

	@ jump to u-boot
	ldr	pc, =ARMBOOT_DESTADDR

	.ltorg



#if CFG_0_BASE_ADDR != 0x0
	.section .remapper, "ax"
	/* remapping AHB addresses */
	ldr	r5, _ahb_slave_system_dram
	ldr	r0, =0x000001FF
	str	r0, [r5]
	ldr	r5, =(VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_BRC))
	
	ldr	r0, =(VPL_BRC_ROM_BASE_REMAPPED + 0x0)
	str	r0, [r5]

	ldr	pc, =_remap

_ahb_slave_system_dram:
	.int (VPL_AHBC_0_MMR_BASE + AHBC_SLAVE_BASESIZE(AHB_SLAVE_DDRSDMC_1_MEM))
#endif
