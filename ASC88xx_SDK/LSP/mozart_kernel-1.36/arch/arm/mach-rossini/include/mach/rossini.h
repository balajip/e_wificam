/*
 * $Header:$
 *
 * Copyright (C) 2007-2013 VN Inc. All rights reserved.
 *
 * Description:
 *   ROSSINI specfic definition
 *
 *
 * $History:$
 *
 */
/* ======================================================================== */

#ifndef __ASM_ARCH_PLATFORM_ROSSINI_H__
#define __ASM_ARCH_PLATFORM_ROSSINI_H__

/* AHB slaves */
#define ROSSINI_AHBC_0_BASE              0xC0000000
#define ROSSINI_AHBC_1_BASE              0xC1000000
#define ROSSINI_AHBC_2_BASE              0xC2000000

#define ROSSINI_DDRSDMC_0_MMR_BASE       0xC4000000
#define ROSSINI_DDRSDMC_1_MMR_BASE       0xC5000000

#define ROSSINI_INTC_MMR_BASE            0xC7000000
#define ROSSINI_APBC_MMR_BASE            0xC8000000
#define ROSSINI_DMAC_MMR_BASE            0xC9000000
#define ROSSINI_VIC_MMR_BASE             0xCA000000

#define ROSSINI_VOC_MMR_BASE             0xCC000000

#define ROSSINI_DCE_MMR_BASE             0xD0000000
#define ROSSINI_DIE_MMR_BASE             0xD1000000
#define ROSSINI_IBPE_MMR_BASE            0xD2000000
#define ROSSINI_IRE_MMR_BASE             0xD3000000

#define ROSSINI_MEAE_MMR_BASE            0xD7000000
#define ROSSINI_JEBE_MMR_BASE            0xD8000000

#define ROSSINI_H4EE_MMR_BASE            0xDC000000

#define ROSSINI_USB_MMR_BASE             0x80000000
#define ROSSINI_PCIEC_DBI_BASE           0x84000000

#define ROSSINI_MSHC0_MMR_BASE           0x90000000
#define ROSSINI_MSHC1_MMR_BASE           0x94000000
#define ROSSINI_GMAC_MMR_BASE            0x98000000

#define ROSSINI_NFC_MMR_BASE             0xA0000000

#define ROSSINI_PCIEC_MEM_BASE           0x60000000
#define ROSSINI_APB3C_DEV_MMR_BASE       0x50000000
#define ROSSINI_APBC_DEV_MMR_BASE        0x40000000

#define ROSSINI_BRC_MEM_BASE             0x30000000
//---------------------------------------------------------------------
//[jam patch] Add Nand-Flash and BRC-Ctrl mmr base addresses
#define ROSSINI_SRAM_CTRL_MMR_BASE       (ROSSINI_BRC_MEM_BASE + 0x4004)
#define ROSSINI_SRAM_MMR_BASE            (ROSSINI_BRC_MEM_BASE + 0x1000)
//---------------------------------------------------------------------

/* APB slaves */
#define ROSSINI_SSI_MMR_BASE             0x40800000

#define ROSSINI_I2S_0_MMR_BASE           0x41800000
#define ROSSINI_I2S_1_MMR_BASE           0x42000000
#define ROSSINI_I2S_2_MMR_BASE           0x42800000
#define ROSSINI_I2S_3_MMR_BASE           0x43000000
#define ROSSINI_I2S_4_MMR_BASE           0x43800000

#define ROSSINI_USB_PHY_BASE             0x47000000
#define ROSSINI_PCIEC_SSC_BASE           0x47800000
#define ROSSINI_TMRC_MMR_BASE            0x48000000
#define ROSSINI_WDTC_MMR_BASE            0x48800000
#define ROSSINI_GPIOC_MMR_BASE           0x49000000
#define ROSSINI_AGPOC_MMR_BASE           0x49800000
#define ROSSINI_UARTC0_MMR_BASE          0x4A000004
#define ROSSINI_UARTC1_MMR_BASE          0x4A800004
#define ROSSINI_UARTC2_MMR_BASE          0x4B000004
#define ROSSINI_UARTC3_MMR_BASE          0x4B800004
#define ROSSINI_IRDAC_MMR_BASE           0x4C000000

#define ROSSINI_PLLC_MMR_BASE            0x4F000000
#define ROSSINI_SYSC_MMR_BASE            0x4F800000


/*
 *-----------------------------
 * Device Base Unified MACROs
 *-----------------------------
 */
/* AHB slaves */
#define VPL_AHBC_0_MMR_BASE             ROSSINI_AHBC_0_BASE
#define VPL_AHBC_1_MMR_BASE             ROSSINI_AHBC_1_BASE
#define VPL_AHBC_2_MMR_BASE             ROSSINI_AHBC_2_BASE
#define VPL_DDRSDMC_0_MMR_BASE          ROSSINI_DDRSDMC_0_MMR_BASE
#define VPL_DDRSDMC_1_MMR_BASE          ROSSINI_DDRSDMC_1_MMR_BASE
#define VPL_INTC_MMR_BASE               ROSSINI_INTC_MMR_BASE
#define VPL_APBC_MMR_BASE               ROSSINI_APBC_MMR_BASE
#define VPL_DMAC_MMR_BASE               ROSSINI_DMAC_MMR_BASE
#define VPL_VIC_MMR_BASE                ROSSINI_VIC_MMR_BASE
#define VPL_VOC_MMR_BASE                ROSSINI_VOC_MMR_BASE
#define VPL_USBC_MMR_BASE               ROSSINI_USB_MMR_BASE
#define VPL_PCIEC_DBI_BASE              ROSSINI_PCIEC_DBI_BASE
#define VPL_MSHC_0_MMR_BASE             ROSSINI_MSHC0_MMR_BASE
#define VPL_MSHC_1_MMR_BASE             ROSSINI_MSHC1_MMR_BASE
#define VPL_GMAC_MMR_BASE               ROSSINI_GMAC_MMR_BASE
#define VPL_NFC_MMR_BASE                ROSSINI_NFC_MMR_BASE
#define VPL_PCIEC_MEM_BASE              ROSSINI_PCIEC_MEM_BASE
#define VPL_APB3C_DEV_MMR_BASE          ROSSINI_APB3C_DEV_MMR_BASE
#define VPL_APBC_DEV_MMR_BASE           ROSSINI_APBC_DEV_MMR_BASE
#define VPL_BRC_MEM_BASE                ROSSINI_BRC_MEM_BASE

#define VMA_DIE_MMR_BASE                ROSSINI_DIE_MMR_BASE
#define VMA_MEAE_MMR_BASE               ROSSINI_MEAE_MMR_BASE
#define VMA_JEBE_MMR_BASE               ROSSINI_JEBE_MMR_BASE
#define VMA_H4EE_MMR_BASE               ROSSINI_H4EE_MMR_BASE
#define VMA_IRE_MMR_BASE                ROSSINI_IRE_MMR_BASE
#define VMA_IBPE_MMR_BASE               ROSSINI_IBPE_MMR_BASE
#define VMA_DCE_MMR_BASE                ROSSINI_DCE_MMR_BASE

/* APB slaves */
#define VPL_SSI_MMR_BASE                ROSSINI_SSI_MMR_BASE
#define VPL_I2S_0_MMR_BASE              ROSSINI_I2S_0_MMR_BASE
#define VPL_I2S_1_MMR_BASE              ROSSINI_I2S_1_MMR_BASE
#define VPL_I2S_2_MMR_BASE              ROSSINI_I2S_2_MMR_BASE
#define VPL_I2S_3_MMR_BASE              ROSSINI_I2S_3_MMR_BASE
#define VPL_I2S_4_MMR_BASE              ROSSINI_I2S_4_MMR_BASE
#define VPL_USBC_SSC_BASE               ROSSINI_USB_PHY_BASE
#define VPL_PCIEC_SSC_BASE              ROSSINI_PCIEC_SSC_BASE
#define VPL_TMRC_MMR_BASE               ROSSINI_TMRC_MMR_BASE
#define VPL_WDTC_MMR_BASE               ROSSINI_WDTC_MMR_BASE
#define VPL_GPIOC_MMR_BASE              ROSSINI_GPIOC_MMR_BASE
#define VPL_AGPOC_MMR_BASE              ROSSINI_AGPOC_MMR_BASE
#define VPL_UARTC_0_MMR_BASE            ROSSINI_UARTC0_MMR_BASE
#define VPL_UARTC_1_MMR_BASE            ROSSINI_UARTC1_MMR_BASE
#define VPL_UARTC_2_MMR_BASE            ROSSINI_UARTC2_MMR_BASE
#define VPL_UARTC_3_MMR_BASE            ROSSINI_UARTC3_MMR_BASE
#define VPL_IRDAC_MMR_BASE              ROSSINI_IRDAC_MMR_BASE
#define VPL_PLLC_MMR_BASE               ROSSINI_PLLC_MMR_BASE
#define VPL_SYSC_MMR_BASE               ROSSINI_SYSC_MMR_BASE




#define SYS_CLK                         CONFIG_SYS_CLK
#define AHB_CLK                         SYS_CLK
#define MAX_TIMER                       3
#define APB_CLK                         (SYS_CLK / 2)
#define DEFAULT_TICK                    1
#define PCLK_HZ                         (SYS_CLK/4)
/*  System Clock=22.1184MHZ   FCLK=System Clock/2   PCLK=System Clock/4
 *  UART clock =22118400/4=5529600     DLL=5529600/16 * baud
 */
#define UART_CLOCK                      CONFIG_UART_CLK
#define UART_CLK                        CONFIG_UART_CLK
#define ROSSINI_UART_CLOCK               UART_CLK
#define VPL_UART_BAUD_115200            (UART_CLOCK / 1843200)
#define VPL_UART_BAUD_57600             (UART_CLOCK / 921600)
#define VPL_UART_BAUD_38400             (UART_CLOCK / 614400)
#define VPL_UART_BAUD_19200             (UART_CLOCK / 307200)
#define VPL_UART_BAUD_9600              (UART_CLOCK / 153600)
#define VPL_UART_BAUD_4800              (UART_CLOCK / 76800)
#define VPL_UART_BAUD_2400              (UART_CLOCK / 38400)
#define VPL_UART_BAUD_1200              (UART_CLOCK / 19200)
#define VPL_UART_BAUD_300               (UART_CLOCK / 4800)
#define HOST_COMPORT                    IO_ADDRESS(ROSSINI_UARTC0_MMR_BASE)
#define HOST_IRQBIT_NUMBER              IRQ_UART1

/* Default port for use by Operating System or program */
#define OS_COMPORT                      ROSSINI_UART2_BASE
#define OS_IRQBIT_NUMBER                IRQ_UART2
#define DEBUG_COMPORT                   OS_COMPORT
#define DEBUG_IRQBIT                    OS_IRQBIT
#define MAX_UART_NUM                    4
#define DEFAULT_HOST_BAUD               VPL_UART_BAUD_38400
#define ROSSINI_FLASH_BASE               0x10000000  // aren 20040511
#define ROSSINI_FLASH_SIZE               0x00800000
#define FLASH_ADDRESS(x) ((x&0x0fffffff) + IO_BASE)

#endif  // __ASM_ARCH_PLATFORM_ROSSINI_H__
