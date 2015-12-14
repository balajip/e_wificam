/*
 * $Header: $
 *
 * Copyright 2000-2005 VN, Inc. All rights reserved.
 *
 *   HAYDN specfic definition
 *
 * $History: $
 *
 */
/* ======================================================================== */
#ifndef __ASM_ARCH_PLATFORM_HAYDN_H__
#define __ASM_ARCH_PLATFORM_HAYDN_H__

/* ======================================================================== */
/*
 *   Handel Memory/IO address map (after remapping)
 *
 *                +==========================================
 *     0x00000000 | DDR-SDRAM (32M/...)
 *     0x0FFFFFFF |
 *                +==========================================
 *     0x10000000 | Flash/SRAM (4M/8M/...)
 *     0x1FFFFFFF |
 *                +==========================================
 *     0x20000000 |APB devices
 *                |0x20000000 Interrupt controller (INTC)
 *                |0x20200000 WatchDog Timer controller (WDTC)
 *                |0x20400000 Timer controller (TMRC)
 *                |0x20600000 Real-Time controller (RTCC)
 *                |0x20800000 GPIO controller (GPIOC)
 *                |0x20A00000 UART controller 0 (UARTC0)
 *                |0x20C00000 UART controller 1 (UARTC1)
 *                |0x20E00000 I2C controller (I2CC)
 *                |
 *                |0x24000000 I2S controller 0 (I2SC0)
 *                |0x24200000 I2S controller 1 (I2SC1)
 *                |0x24400000 PS2 controller (PS2IC)
 *                |
 *                |0x2CE00000 System controller (SYSC)
 *                |
 *                +==========================================
 *     0x30000000 |PCI devices
 *                |0x30000000
 *                |
 *                +==========================================
 *     0x40000000 |AHB bridge 2 memory
 *                |0x40000000
 *                |
 *                +==========================================
 *     0x50000000 |AHB bridge 1 memory
 *                |0x50000000
 *                |
 *                +==========================================
 *     0x90000000 |AHB Controllers' reg
 *                |
 *                |0x90000000 AHB controller 0 (AHBC0)
 *                |0x90100000 Static Memory controller (SMC)
 *                |0x90200000 DDR-SDRAM controller 0 (DDRSDRAMC0)
 *                |0x90300000 APB controller (APBC)
 *                |0x90400000 PCI bridge
 *                |0x90500000 USB
 *                |0x90600000 MAC
 *                |0x90700000 IDE controller (IDEC)
 *                |0x90800000 AHB controller 1 (AHBC1)
 *                |0x90900000 AHB controller 2 (AHBC2)
 *                |0x90A00000 DDR-SDRAM controller 1 (DDRSDRAMC1)
 *                |0x90B00000 DDR-SDRAM controller 2 (DDRSDRAMC2)
 *                |0x90C00000 DMA controller (DMAC)
 *                |0x90D00000 Video Input controller (VIC)
 *                |0x90E00000 Video Output controller (VOC)
 *                |0x90F00000 reserved
 *                |
 *                |0xA0000000 DIE
 *                |0xA0200000 MEAE
 *                |0xA0400000 MEBE
 *                |0xA0600000 MME
 *                |0xA0800000 IPE
 *                |0xA0A00000 MDBE
 *                |
 *                +==========================================
 */

/* ======================================================================== */
//#include 	<linux/config.h>
#include 	"chipset.h"
//Haydn MMR
/* ======================================================================== */
/* AHB 0 slave base addresses */
#define HAYDN_AHBC_0_BASE              0x98000000
#define HAYDN_SMC_MMR_BASE             0x90100000
#define HAYDN_DDRSDMC_0_MMR_BASE       0x98100000
#define HAYDN_APBC_MMR_BASE            0x99500000
//#define HAYDN_PCI_BRG_MMR_BASE         0x90400000
#define HAYDN_USB_MMR_BASE             0x98400000
#define HAYDN_MAC_MMR_BASE             0x98500000
//#define HAYDN_IDEC_MMR_BASE            0x90700000
#define HAYDN_AHBC_1_BASE              0x18000000
//#define HAYDN_AHBC_2_BASE              0x90900000
#define HAYDN_DDRSDMC_1_MMR_BASE       0x10000000
//#define HAYDN_DDRSDMC_2_MMR_BASE       0x90B00000
#define HAYDN_DMAC_MMR_BASE            0x99900000
#define HAYDN_VIC_MMR_BASE             0x99700000
//#define HAYDN_VOC_MMR_BASE             0x90E00000

/* Add by Sor */
#define VPL_AHBC_0_MMR_BASE             HAYDN_AHBC_0_BASE        
#define VPL_DDRSDMC_0_MMR_BASE          HAYDN_DDRSDMC_0_MMR_BASE 
#define VPL_APBC_MMR_BASE               HAYDN_APBC_MMR_BASE      
//#define VPL_IDE_MMRC_BASE               HAYDN_IDEC_MMR_BASE      
#define VPL_AHBC_1_MMR_BASE             HAYDN_AHBC_1_BASE        
//#define VPL_AHBC_2_MMR_BASE             HAYDN_AHBC_2_BASE        
#define VPL_DDRSDMC_1_MMR_BASE          HAYDN_DDRSDMC_1_MMR_BASE 
//#define VPL_DDRSDMC_2_MMR_BASE          HAYDN_DDRSDMC_2_MMR_BASE 
#define VPL_DMAC_MMR_BASE               HAYDN_DMAC_MMR_BASE      
#define VPL_VIC_MMR_BASE                HAYDN_VIC_MMR_BASE       
//#define VPL_VOC_MMR_BASE                HAYDN_VOC_MMR_BASE       
/* ========== */

#define HAYDN_DIE_MMR_BASE             0x98800000
#define HAYDN_MEAE_MMR_BASE            0x98C00000
#define HAYDN_JEBE_MMR_BASE            0x99000000
#define HAYDN_MEBE_MMR_BASE            0x99200000
#define HAYDN_MDE_MMR_BASE             0x98A00000
#define HAYDN_MME_MMR_BASE             0x98B00000
#define HAYDN_IBPE_MMR_BASE            0x98900000
#define HAYDN_IRE_MMR_BASE             0x98D00000
#define HAYDN_IPE_MMR_BASE             0xA0800000
#define HAYDN_MDBE_MMR_BASE            0xA0A00000
//jon add 08/12/28
#define HAYDN_DCE_MMR_BASE			   0x99400000

/* Add by Sor */
#define HAYDN_F2BE_MMR_BASE		0x98E00000
#define HAYDN_B2FE_MMR_BASE		0xA0E00000

#define VMA_DIE_MMR_BASE                HAYDN_DIE_MMR_BASE 
#define VMA_MEAE_MMR_BASE               HAYDN_MEAE_MMR_BASE
#define VMA_MEBE_MMR_BASE               HAYDN_MEBE_MMR_BASE
#define VMA_JEBE_MMR_BASE               HAYDN_JEBE_MMR_BASE
#define VMA_MDE_MMR_BASE                HAYDN_MDE_MMR_BASE 
#define VMA_MME_MMR_BASE                HAYDN_MME_MMR_BASE 
#define VMA_IPE_MMR_BASE                HAYDN_IPE_MMR_BASE 
#define VMA_IRE_MMR_BASE                HAYDN_IRE_MMR_BASE 
#define VMA_MDBE_MMR_BASE               HAYDN_MDBE_MMR_BASE
#define VMA_F2BE_MMR_BASE               HAYDN_F2BE_MMR_BASE
#define VMA_B2FE_MMR_BASE               HAYDN_B2FE_MMR_BASE
#define VMA_IBPE_MMR_BASE				HAYDN_IBPE_MMR_BASE
#define VMA_DCE_MMR_BASE				HAYDN_DCE_MMR_BASE
/* ========== */                
                                
/* ------------------------------------------------------------------------ */
#define HAYDN_AHBBRGC_1_MEM_BASE       0x50000000
#define HAYDN_AHBBRGC_2_MEM_BASE       0x40000000
#define HAYDN_PCI_DEV_MMR_BASE         0x30000000
#define HAYDN_APBC_DEV_MMR_BASE        0x20000000
                                                                                
/* ------------------------------------------------------------------------ */
/* APB device base addresses */
#define HAYDN_INTC_MMR_BASE            0x98600000
#define HAYDN_WDTC_MMR_BASE            0x48800000
#define HAYDN_TMRC_MMR_BASE            0x48000000
#define HAYDN_RTCC_MMR_BASE            0x20600000
#define HAYDN_GPIOC_MMR_BASE           0x49000000
#define HAYDN_UARTC0_MMR_BASE          0x49800004//0x98900000//jon 0x20A00000
#define HAYDN_UARTC1_MMR_BASE          0x4a000004//0x98B00000//jon 0x20C00000
#define HAYDN_UARTC2_MMR_BASE          0x4a800004
#define HAYDN_UARTC3_MMR_BASE          0x4b000004

#define HAYDN_I2C_MMR_BASE             0x20E00000

#define HAYDN_I2S_0_MMR_BASE           0x41000000
#define HAYDN_I2S_1_MMR_BASE           0x24200000
#define HAYDN_PS2IC_MMR_BASE           0x24400000
#define HAYDN_I2S_2_MMR_BASE           0x24600000

#define HAYDN_SSI_MMR_BASE             0x40800000

#define HAYDN_SYSC_MMR_BASE            0x4F800000
                                                                                
/* ========== */                
#define TMRC_MMR_BASE                   HAYDN_TMRC_MMR_BASE
/* ========== */                

/* Add by Sor */
#define VPL_TMRC_MMR_BASE               HAYDN_TMRC_MMR_BASE  
#define VPL_RTCC_MMR_BASE               HAYDN_RTCC_MMR_BASE  
#define VPL_GPIOC_MMR_BASE              HAYDN_GPIOC_MMR_BASE 
#define VPL_UARTC_0_MMR_BASE            HAYDN_UARTC0_MMR_BASE
#define VPL_UARTC_1_MMR_BASE            HAYDN_UARTC1_MMR_BASE
#define VPL_PS2IC_MMR_BASE              HAYDN_PS2IC_MMR_BASE 
#define VPL_SYSC_MMR_BASE               HAYDN_SYSC_MMR_BASE  
/* ========== */                

//#define HAYDN_VVDC_BASE		0x99c00000
//#define HAYDN_VMA_AE_BASE		0xc0500000
//#define HAYDN_VMA_BE_BASE		0xc0600000
//#define HAYDN_VMA_ME_BASE		0xc0700000
//#define HAYDN_VMA_VPE_BASE		0xc0800000

/* ======================================================================== */
#define SYS_CLK                     	CONFIG_SYS_CLK //106000000//26250000 //119808000//110592000//119808000//jon CONFIG_SYS_CLK
#define AHB_CLK                     	SYS_CLK
#define MAX_TIMER                   	3
#define APB_CLK                     	(SYS_CLK / 2)
#define DEFAULT_TICK                	1
#define PCLK_HZ							(SYS_CLK/4)
/*  System Clock=22.1184MHZ   FCLK=System Clock/2   PCLK=System Clock/4
 *  UART clock =22118400/4=5529600     DLL=5529600/16 * baud 
 */
#define UART_CLOCK			CONFIG_UART_CLK
#define UART_CLK			CONFIG_UART_CLK
#define HAYDN_UART_CLOCK			UART_CLK
#define VPL_UART_BAUD_115200		(UART_CLOCK / 1843200)
#define VPL_UART_BAUD_57600		(UART_CLOCK / 921600)
#define VPL_UART_BAUD_38400		(UART_CLOCK / 614400)
#define VPL_UART_BAUD_19200		(UART_CLOCK / 307200)
//#define VPL_UART_BAUD_14400		(UART_CLOCK / 230400)
#define VPL_UART_BAUD_9600		(UART_CLOCK / 153600)
#define VPL_UART_BAUD_4800		(UART_CLOCK / 76800)
#define VPL_UART_BAUD_2400		(UART_CLOCK / 38400)
#define VPL_UART_BAUD_1200		(UART_CLOCK / 19200)
#define VPL_UART_BAUD_300		(UART_CLOCK / 4800)
#define HOST_COMPORT                    IO_ADDRESS(HAYDN_UARTC0_MMR_BASE)
#define HOST_IRQBIT_NUMBER              IRQ_UART1
/* Default port for use by Operating System or program 
 */
#define OS_COMPORT                      HAYDN_UART2_BASE
#define OS_IRQBIT_NUMBER                IRQ_UART2
#define DEBUG_COMPORT                   OS_COMPORT
#define DEBUG_IRQBIT                    OS_IRQBIT
#define MAX_UART_NUM                    2
#define DEFAULT_HOST_BAUD               VPL_UART_BAUD_38400
#define HAYDN_FLASH_BASE		0x10000000	// aren 20040511
#define HAYDN_FLASH_SIZE		0x00800000
#define FLASH_ADDRESS(x) ((x&0x0fffffff) + IO_BASE)


#endif	// __ASM_ARCH_PLATFORM_HAYDN_H__
