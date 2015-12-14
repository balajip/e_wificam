/*
 * $Header:$
 *
 * Copyright (C) 2007-2013 VN Inc. All rights reserved.
 *
 * Description:
 *
 *
 *
 * $History:$
 *
 */

#ifndef __ARCH_ARM_MACH_MOZART_MAPS_H__
#define __ARCH_ARM_MACH_MOZART_MAPS_H__

/* AHB slaves */
#define MOZART_BRC_MEM_BASE             0x80000000

#define MOZART_APBC_DEV_MMR_BASE        0xB0000000
#define MOZART_APB3C_DEV_MMR_BASE       0xA0000000
#define MOZART_PCIEC_MEM_BASE           0x90000000

#define MOZART_USB_MMR_BASE             0x81000000
#define MOZART_PCIEC_DBI_BASE           0x82000000
#define MOZART_MSHC0_MMR_BASE           0x83000000
#define MOZART_MSHC1_MMR_BASE           0x84000000
#define MOZART_GMAC_MMR_BASE            0x85000000

#define MOZART_NFC_MMR_BASE             0x86000000

#define MOZART_AHBC_0_BASE              0xC0000000
#define MOZART_AHBC_1_BASE              0xC1000000
#define MOZART_AHBC_2_BASE              0xC2000000

#define MOZART_DDRSDMC_0_MMR_BASE       0xC4000000
#define MOZART_DDRSDMC_1_MMR_BASE       0xC5000000

#define MOZART_INTC_MMR_BASE            0xC7000000
#define MOZART_APBC_MMR_BASE            0xC8000000
#define MOZART_DMAC_MMR_BASE            0xC9000000
#define MOZART_VIC_MMR_BASE             0xCA000000

#define MOZART_VOC_MMR_BASE             0xCC000000

#define MOZART_DCE_MMR_BASE             0xD0000000
#define MOZART_DIE_MMR_BASE             0xD1000000
#define MOZART_IBPE_MMR_BASE            0xD2000000
#define MOZART_IRE_MMR_BASE             0xD3000000

#define MOZART_MEAE_MMR_BASE            0xD7000000
#define MOZART_JEBE_MMR_BASE            0xD8000000

#define MOZART_H4EE_MMR_BASE            0xDC000000

//---------------------------------------------------------------------
//[jam patch] Add Nand-Flash and BRC-Ctrl mmr base addresses
#define MOZART_SRAM_CTRL_MMR_BASE       (MOZART_BRC_MEM_BASE + 0x4004)
#define MOZART_SRAM_MMR_BASE            (MOZART_BRC_MEM_BASE + 0x1000)
//---------------------------------------------------------------------

/* APB slaves */
#define MOZART_SSI_MMR_BASE             0xB0800000

#define MOZART_I2S_0_MMR_BASE           0xB1800000
#define MOZART_I2S_1_MMR_BASE           0xB2000000
#define MOZART_I2S_2_MMR_BASE           0xB2800000
#define MOZART_I2S_3_MMR_BASE           0xB3000000
#define MOZART_I2S_4_MMR_BASE           0xB3800000

#define MOZART_USB_PHY_BASE             0xB7000000
#define MOZART_PCIEC_SSC_BASE           0xB7800000
#define MOZART_TMRC_MMR_BASE            0xB8000000
#define MOZART_WDTC_MMR_BASE            0xB8800000
#define MOZART_GPIOC_MMR_BASE           0xB9000000
#define MOZART_AGPOC_MMR_BASE           0xB9800000
#define MOZART_UARTC0_MMR_BASE          0xBA000004
#define MOZART_UARTC1_MMR_BASE          0xBA800004
#define MOZART_UARTC2_MMR_BASE          0xBB000004
#define MOZART_UARTC3_MMR_BASE          0xBB800004
#define MOZART_IRDAC_MMR_BASE           0xBC000000

#define MOZART_PLLC_MMR_BASE            0xBF000000
#define MOZART_SYSC_MMR_BASE            0xBF800000


/*
 *-----------------------------
 * Device Base Unified MACROs
 *-----------------------------
 */
/* AHB slaves */
#define VPL_AHBC_0_MMR_BASE             MOZART_AHBC_0_BASE
#define VPL_AHBC_1_MMR_BASE             MOZART_AHBC_1_BASE
#define VPL_AHBC_2_MMR_BASE             MOZART_AHBC_2_BASE
#define VPL_DDRSDMC_0_MMR_BASE          MOZART_DDRSDMC_0_MMR_BASE
#define VPL_DDRSDMC_1_MMR_BASE          MOZART_DDRSDMC_1_MMR_BASE
#define VPL_INTC_MMR_BASE               MOZART_INTC_MMR_BASE
#define VPL_APBC_MMR_BASE               MOZART_APBC_MMR_BASE
#define VPL_DMAC_MMR_BASE               MOZART_DMAC_MMR_BASE
#define VPL_VIC_MMR_BASE                MOZART_VIC_MMR_BASE
#define VPL_VOC_MMR_BASE                MOZART_VOC_MMR_BASE
#define VPL_USBC_MMR_BASE               MOZART_USB_MMR_BASE
#define VPL_PCIEC_DBI_BASE              MOZART_PCIEC_DBI_BASE
#define VPL_MSHC_0_MMR_BASE             MOZART_MSHC0_MMR_BASE
#define VPL_MSHC_1_MMR_BASE             MOZART_MSHC1_MMR_BASE
#define VPL_GMAC_MMR_BASE               MOZART_GMAC_MMR_BASE
#define VPL_NFC_MMR_BASE                MOZART_NFC_MMR_BASE
#define VPL_PCIEC_MEM_BASE              MOZART_PCIEC_MEM_BASE
#define VPL_APB3C_DEV_MMR_BASE          MOZART_APB3C_DEV_MMR_BASE
#define VPL_APBC_DEV_MMR_BASE           MOZART_APBC_DEV_MMR_BASE
#define VPL_BRC_MEM_BASE                MOZART_BRC_MEM_BASE

#define VMA_DIE_MMR_BASE                MOZART_DIE_MMR_BASE
#define VMA_MEAE_MMR_BASE               MOZART_MEAE_MMR_BASE
#define VMA_JEBE_MMR_BASE               MOZART_JEBE_MMR_BASE
#define VMA_H4EE_MMR_BASE               MOZART_H4EE_MMR_BASE
#define VMA_IRE_MMR_BASE                MOZART_IRE_MMR_BASE
#define VMA_IBPE_MMR_BASE               MOZART_IBPE_MMR_BASE
#define VMA_DCE_MMR_BASE                MOZART_DCE_MMR_BASE

/* APB slaves */
#define VPL_SSI_MMR_BASE                MOZART_SSI_MMR_BASE
#define VPL_I2S_0_MMR_BASE              MOZART_I2S_0_MMR_BASE
#define VPL_I2S_1_MMR_BASE              MOZART_I2S_1_MMR_BASE
#define VPL_I2S_2_MMR_BASE              MOZART_I2S_2_MMR_BASE
#define VPL_I2S_3_MMR_BASE              MOZART_I2S_3_MMR_BASE
#define VPL_I2S_4_MMR_BASE              MOZART_I2S_4_MMR_BASE
#define VPL_USBC_SSC_BASE               MOZART_USB_PHY_BASE
#define VPL_PCIEC_SSC_BASE              MOZART_PCIEC_SSC_BASE
#define VPL_TMRC_MMR_BASE               MOZART_TMRC_MMR_BASE
#define VPL_WDTC_MMR_BASE               MOZART_WDTC_MMR_BASE
#define VPL_GPIOC_MMR_BASE              MOZART_GPIOC_MMR_BASE
#define VPL_AGPOC_MMR_BASE              MOZART_AGPOC_MMR_BASE
#define VPL_UARTC_0_MMR_BASE            MOZART_UARTC0_MMR_BASE
#define VPL_UARTC_1_MMR_BASE            MOZART_UARTC1_MMR_BASE
#define VPL_UARTC_2_MMR_BASE            MOZART_UARTC2_MMR_BASE
#define VPL_UARTC_3_MMR_BASE            MOZART_UARTC3_MMR_BASE
#define VPL_IRDAC_MMR_BASE              MOZART_IRDAC_MMR_BASE
#define VPL_PLLC_MMR_BASE               MOZART_PLLC_MMR_BASE
#define VPL_SYSC_MMR_BASE               MOZART_SYSC_MMR_BASE




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
#define MOZART_UART_CLOCK               UART_CLK
#define VPL_UART_BAUD_115200            (UART_CLOCK / 1843200)
#define VPL_UART_BAUD_57600             (UART_CLOCK / 921600)
#define VPL_UART_BAUD_38400             (UART_CLOCK / 614400)
#define VPL_UART_BAUD_19200             (UART_CLOCK / 307200)
#define VPL_UART_BAUD_9600              (UART_CLOCK / 153600)
#define VPL_UART_BAUD_4800              (UART_CLOCK / 76800)
#define VPL_UART_BAUD_2400              (UART_CLOCK / 38400)
#define VPL_UART_BAUD_1200              (UART_CLOCK / 19200)
#define VPL_UART_BAUD_300               (UART_CLOCK / 4800)
#define HOST_COMPORT                    IO_ADDRESS(MOZART_UARTC0_MMR_BASE)
#define HOST_IRQBIT_NUMBER              IRQ_UART1

/* Default port for use by Operating System or program */
#define OS_COMPORT                      MOZART_UART2_BASE
#define OS_IRQBIT_NUMBER                IRQ_UART2
#define DEBUG_COMPORT                   OS_COMPORT
#define DEBUG_IRQBIT                    OS_IRQBIT
#define MAX_UART_NUM                    4
#define DEFAULT_HOST_BAUD               VPL_UART_BAUD_38400
#define MOZART_FLASH_BASE               0x10000000  // aren 20040511
#define MOZART_FLASH_SIZE               0x00800000
#define FLASH_ADDRESS(x) ((x&0x0fffffff) + IO_BASE)

#endif  // __ARCH_ARM_MACH_MOZART_MAPS_H__
