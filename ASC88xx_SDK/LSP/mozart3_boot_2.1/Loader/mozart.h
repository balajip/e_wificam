/*
 * Copyright 2011 VN Inc. All rights reserved.
 *
 *   Mozart specfic definition
 *
 */


#ifndef _MOZART_H_
#define _MOZART_H_


/*
 *-----------------------------
 * Device Base Address
 *-----------------------------
 */
/* AHB slaves */
#define VPL_BRC_ROM_BASE              0x00000000
#define VPL_BRC_RAM_BASE              (VPL_BRC_ROM_BASE + 0x00001000)
#define VPL_BRC_MMR_BASE              (VPL_BRC_ROM_BASE + 0x00004000)
#define VPL_BRC_ROM_BASE_REMAPPED     0x80000000
#define VPL_BRC_RAM_BASE_REMAPPED     (VPL_BRC_ROM_BASE_REMAPPED + 0x00001000)
#define VPL_BRC_MMR_BASE_REMAPPED     (VPL_BRC_ROM_BASE_REMAPPED + 0x00004000)
#define VPL_DDRSDMC_1_MEM_BASE        0x40000000
                                   /* 0x20000000 */
#define VPL_DDRSDMC_0_MEM_BASE        0x80000000
#define VPL_APBC_DEV_MMR_BASE         0xB0000000
#define VPL_APB3C_DEV_MMR_BASE        0xA0000000
#define VPL_PCIEC_MEM_BASE            0x90000000
                                   /* 0x70000000 */
#define VPL_USBC_MMR_BASE             0x81000000
#define VPL_USBC_RAM_BASE             0x81020000
#define VPL_PCIEC_MMR_BASE            0x82000000
#define VPL_MSHC_0_MMR_BASE           0x83000000
#define VPL_MSHC_1_MMR_BASE           0x84000000
#define VPL_GMAC_MMR_BASE             0x85000000
#define VPL_NFC_MMR_BASE              0x86000000
                                   /* 0xB0000000 */
#define VPL_AHBC_0_MMR_BASE           0xC0000000
#define VPL_AHBC_1_MMR_BASE           0xC1000000
#define VPL_AHBC_2_MMR_BASE           0xC2000000
#define VPL_DDRSDMC_0_MMR_BASE        0xC4000000
#define VPL_DDRSDMC_1_MMR_BASE        0xC5000000
#define VPL_INTC_MMR_BASE             0xC7000000
#define VPL_APBC_MMR_BASE             0xC8000000
#define VPL_DMAC_MMR_BASE             0xC9000000
#define VPL_VIC_MMR_BASE              0xCA000000
#define VPL_VOC_MMR_BASE              0xCC000000
#define VMA_DCE_MMR_BASE              0xD0000000
#define VMA_DIE_MMR_BASE              0xD1000000
#define VMA_IBPE_MMR_BASE             0xD2000000
#define VMA_IRE_MMR_BASE              0xD3000000
#define VMA_MEAE_MMR_BASE             0xD7000000
#define VMA_JEBE_MMR_BASE             0xD8000000
#define VMA_H4EE_MMR_BASE             0xDC000000
                                   /* 0xE0000000 */
                                   /* 0xE1000000 */
                                   /* 0xE2000000 */
                                   /* 0xE3000000 */
                                   /* 0xE4000000 */
                                   /* 0xE5000000 */
                                   /* 0xE6000000 */
                                   /* 0xE7000000 */
                                   /* 0xE8000000 */
                                   /* 0xE9000000 */
                                   /* 0xEA000000 */
                                   /* 0xEB000000 */
                                   /* 0xEC000000 */
                                   /* 0xED000000 */
                                   /* 0xEE000000 */
                                   /* 0xEF000000 */
#define VPL_DUMMY_BASE                0xF0000000

/* APB slaves */
                                   /* 0x40000000 */
#define VPL_SSI_MMR_BASE              0xB0800000
                                   /* 0x41000000 */
#define VPL_I2S_0_MMR_BASE            0xB1800000
#define VPL_I2S_1_MMR_BASE            0xB2000000
#define VPL_I2S_2_MMR_BASE            0xB2800000
#define VPL_I2S_3_MMR_BASE            0xB3000000
#define VPL_I2S_4_MMR_BASE            0xB3800000
                                   /* 0x44000000 */
                                   /* 0x45000000 */
                                   /* 0x46000000 */
#define VPL_USB_PHY_BASE              0xB7000000
#define VPL_PCIEC_SSC_BASE            0xB7800000
#define VPL_TMRC_MMR_BASE             0xB8000000
#define VPL_WDTC_MMR_BASE             0xB8800000
#define VPL_GPIOC_MMR_BASE            0xB9000000
#define VPL_AGPOC_MMR_BASE            0xB9800000
#define VPL_UARTC0_MMR_BASE           0xBA000004
#define VPL_UARTC1_MMR_BASE           0xBA800004
#define VPL_UARTC2_MMR_BASE           0xBB000004
#define VPL_UARTC3_MMR_BASE           0xBB800004
#define VPL_IRDAC_MMR_BASE            0xBC000000
                                   /* 0x4D000000 */
                                   /* 0x4E000000 */
#define VPL_PLLC_MMR_BASE             0xBF000000
#define VPL_SYSC_MMR_BASE             0xBF800000

/* APB 3 slaves */
#define VPL_DDR32PHY_PUB_BASE         0xA0000000


/*=============================================================================
 * DDRSDMC definitions
 *============================================================================*/
#define DDR_VERSION                     0x00000000
#define DDR_CTRL                        0x00000004
#define     FIXED_WRITE_AS_BL8_SHIFT          (19)
#define     FIXED_READ_AS_BL8_SHIFT           (18)
#define     SLAVE_CORE_SYNC_EN_SHIFT          (17)
#define     MCI_IOPHY_UPDATE_WAIT_ACK_SHIFT   (16)
#define     MCI_IOPHY_UPDATE_CNTR_SHIFT       (12)
#define     BYP_PUB_EN_SHIFT                  (11)
#define     DDR3_ENABLE_SHIFT                 (10)
#define     DYNAMIC_ODT_EN_SHIFT              ( 9)
#define     PLL_TEST_SEL_SHIFT                ( 7)
#define     MCI_IOPHY_UPDATE_EN_SHIFT         ( 6)
#define     FAST_SETTING_EN_SHIFT             ( 3)
#define     PWR_ON_SHIFT                      ( 2)
#define     EN_SHIFT                          ( 0)
#define     FIXED_WRITE_AS_BL8(x)             ((x) << FIXED_WRITE_AS_BL8_SHIFT)
#define     FIXED_READ_AS_BL8(x)              ((x) << FIXED_READ_AS_BL8_SHIFT)
#define     SLAVE_CORE_SYNC_EN(x)             ((x) << SLAVE_CORE_SYNC_EN_SHIFT)
#define     MCI_IOPHY_UPDATE_WAIT_ACK(x)      ((x) << MCI_IOPHY_UPDATE_WAIT_ACK_SHIFT)
#define     MCI_IOPHY_UPDATE_CNTR(x)          ((x) << MCI_IOPHY_UPDATE_CNTR_SHIFT)
#define     BYP_PUB_EN(x)                     ((x) << BYP_PUB_EN_SHIFT)
#define     DDR3_ENABLE(x)                    ((x) << DDR3_ENABLE_SHIFT)
#define     DYNAMIC_ODT_EN(x)                 ((x) << DYNAMIC_ODT_EN_SHIFT)
#define     PLL_TEST_SEL(x)                   ((x) << PLL_TEST_SEL_SHIFT)
#define     MCI_IOPHY_UPDATE_EN(x)            ((x) << MCI_IOPHY_UPDATE_EN_SHIFT)
#define     FAST_SETTING_EN(x)                ((x) << FAST_SETTING_EN_SHIFT)
#define     PWR_ON(x)                         ((x) << PWR_ON_SHIFT)
#define     EN(x)                             ((x) << EN_SHIFT)
#define DDR_DELAY                       0x00000008
#define     UDQ_CNT_SHIFT                     (12)
#define     LDQ_CNT_SHIFT                     ( 8)
#define     UDQS_POL_SHIFT                    ( 4)
#define     LDQS_POL_SHIFT                    ( 0)
#define     UDQ_CNT(x)                        ((x) << UDQ_CNT_SHIFT)
#define     LDQ_CNT(x)                        ((x) << LDQ_CNT_SHIFT)
#define     UDQS_POL(x)                       ((x) << UDQS_POL_SHIFT)
#define     LDQS_POL(x)                       ((x) << LDQS_POL_SHIFT)
#define DDR_SIZE                        0x0000000C
#define     BASE_ADDR_SHIFT                   (24)
#define     BANK_NUM_SHIFT                    ( 8)
#define     ROW_WIDTH_SHIFT                   ( 4)
#define     COL_WIDTH_SHIFT                   ( 0)
#define     BASE_ADDR(x)                      ((x) & ( 0xff << BASE_ADDR_SHIFT))
#define     BANK_NUM(x)                       ((x) << BANK_NUM_SHIFT)
#define     ROW_WIDTH(x)                      ((x) << ROW_WIDTH_SHIFT)
#define     COL_WIDTH(x)                      ((x) << COL_WIDTH_SHIFT)
#define DDR_LOAD_MODE_0_1               0x00000010
#if CONFIG_DDR_TYPE == DDR3
#define     MR1_QOFF_SHIFT                    (28)
#define     MR1_TDQS_SHIFT                    (27)
#define     MR1_LEVEL_SHIFT                   (23)
#define     MR1_AL_SHIFT                      (19)
#define     MR1_RTT_NORM_SHIFT                (18)
#define     MR1_DIC_SHIFT                     (17)
#define     MR1_DLL_SHIFT                     (16)
#define     MR0_PPD_SHIFT                     (12)
#define     MR0_WR_SHIFT                      ( 9)
#define     MR0_DLL_SHIFT                     ( 8)
#define     MR0_TM_SHIFT                      ( 7)
#define     MR0_RBT_SHIFT                     ( 3)
#define     MR0_CL_SHIFT                      ( 2)
#define     MR0_BL_SHIFT                      ( 0)
#define     MR1_QOFF(x)                       ((x) << MR1_QOFF_SHIFT)
#define     MR1_TDQS(x)                       ((x) << MR1_TDQS_SHIFT)
#define     MR1_LEVEL(x)                      ((x) << MR1_LEVEL_SHIFT)
#define     MR1_AL(x)                         ((x) << MR1_AL_SHIFT)
#define     MR1_RTT_NORM(x)                   (((((x) << 5) & 0x80) | (((x) << 3) & 0x10) | (((x) & 0x1)))<< MR1_RTT_NORM_SHIFT)
#define     MR1_DIC(x)                        (((((x) << 3) & 0x10) | (((x) & 0x1))) << MR1_DIC_SHIFT)
#define     MR1_DLL(x)                        ((x) << MR1_DLL_SHIFT)
#define     MR0_PPD(x)                        ((x) << MR0_PPD_SHIFT)
#define     MR0_WR(x)                         ((x) << MR0_WR_SHIFT)
#define     MR0_DLL(x)                        ((x) << MR0_DLL_SHIFT)
#define     MR0_TM(x)                         ((x) << MR0_TM_SHIFT)
#define     MR0_RBT(x)                        ((x) << MR0_RBT_SHIFT)
#define     MR0_CL(x)                         (((((x) << 1) & 0x1C) | (((x) & 0x1))) << MR0_CL_SHIFT)
#define     MR0_BL(x)                         ((x) << MR0_BL_SHIFT)
#elif CONFIG_DDR_TYPE == DDR2
#define     MR1_QOFF_SHIFT                    (28)
#define     MR1_RDQS_SHIFT                    (27)
#define     MR1_DOSN_SHIFT                    (26)
#define     MR1_OCD_PROG_SHIFT                (23)
#define     MR1_AL_SHIFT                      (19)
#define     MR1_RTT_NORM_SHIFT                (18)
#define     MR1_DIC_SHIFT                     (17)
#define     MR1_DLL_SHIFT                     (16)
#define     MR0_PD_SHIFT                      (12)
#define     MR0_WR_SHIFT                      ( 9)
#define     MR0_DLL_SHIFT                     ( 8)
#define     MR0_TM_SHIFT                      ( 7)
#define     MR0_CL_SHIFT                      ( 4)
#define     MR0_BT_SHIFT                      ( 3)
#define     MR0_BL_SHIFT                      ( 0)
#define     MR1_QOFF(x)                       ((x) << MR1_QOFF_SHIFT)
#define     MR1_RDQS(x)                       ((x) << MR1_RDQS_SHIFT)
#define     MR1_DOSN(x)                       ((x) << MR1_DOSN_SHIFT)
#define     MR1_OCD_PROG(x)                   ((x) << MR1_OCD_PROG_SHIFT)
#define     MR1_AL(x)                         ((x) << MR1_AL_SHIFT)
#define     MR1_RTT_NORM(x)                   (((((x) << 3) & 0x10) | (((x) & 0x1))) << MR1_RTT_NORM_SHIFT)
#define     MR1_DIC(x)                        ((x) << MR1_DIC_SHIFT)
#define     MR1_DLL(x)                        ((x) << MR1_DLL_SHIFT)
#define     MR0_PD(x)                         ((x) << MR0_PD_SHIFT)
#define     MR0_WR(x)                         ((x) << MR0_WR_SHIFT)
#define     MR0_DLL(x)                        ((x) << MR0_DLL_SHIFT)
#define     MR0_TM(x)                         ((x) << MR0_TM_SHIFT)
#define     MR0_CL(x)                         ((x) << MR0_CL_SHIFT)
#define     MR0_BT(x)                         ((x) << MR0_BT_SHIFT)
#define     MR0_BL(x)                         ((x) << MR0_BL_SHIFT)
#endif /* DDR2 */
#define DDR_LOAD_MODE_2_3               0x00000014
#if CONFIG_DDR_TYPE == DDR3
#define     MR3_MPR_SHIFT                     (18)
#define     MR3_MPR_LOC_SHIFT                 (16)
#define     MR2_RTT_WR_SHIFT                  ( 9)
#define     MR2_SRT_SHIFT                     ( 7)
#define     MR2_ASR_SHIFT                     ( 6)
#define     MR2_CWL_SHIFT                     ( 3)
#define     MR2_PASR_SHIFT                    ( 0)
#define     MR3_MPR(x)                        ((x) << MR3_MPR_SHIFT)
#define     MR3_MPR_LOC(x)                    ((x) << MR3_MPR_LOC_SHIFT)
#define     MR2_RTT_WR(x)                     ((x) << MR2_RTT_WR_SHIFT)
#define     MR2_SRT(x)                        ((x) << MR2_SRT_SHIFT)
#define     MR2_ASR(x)                        ((x) << MR2_ASR_SHIFT)
#define     MR2_CWL(x)                        ((x) << MR2_CWL_SHIFT)
#define     MR2_PASR(x)                       ((x) << MR2_PASR_SHIFT)
#elif CONFIG_DDR_TYPE == DDR2
#define     MR2_SRF_SHIFT                     ( 7)
#define     MR2_DCC_SHIFT                     ( 3)
#define     MR2_PASR_SHIFT                    ( 0)
#define     MR2_SRF(x)                        ((x) << MR2_SRF_SHIFT)
#define     MR2_DCC(x)                        ((x) << MR2_DCC_SHIFT)
#define     MR2_PASR(x)                       ((x) << MR2_PASR_SHIFT)
#endif /* DDR2 */
#define DDR_TIMING0                     0x00000018
#define     T_MOD_SHIFT                       (24)
#define     T_WTR_SHIFT                       (20)
#define     T_RTP_SHIFT                       (16)
#define     T_RCD_SHIFT                       (12)
#define     T_RRD_SHIFT                       ( 8)
#define     T_MRD_SHIFT                       ( 4)
#define     T_RP_SHIFT                        ( 0)
#define     T_MOD(x)                          ((x) << T_MOD_SHIFT)
#define     T_WTR(x)                          ((x) << T_WTR_SHIFT)
#define     T_RTP(x)                          ((x) << T_RTP_SHIFT)
#define     T_RCD(x)                          ((x) << T_RCD_SHIFT)
#define     T_RRD(x)                          ((x) << T_RRD_SHIFT)
#define     T_MRD(x)                          ((x) << T_MRD_SHIFT)
#define     T_RP(x)                           ((x) << T_RP_SHIFT)
#define DDR_TIMING1                     0x0000001C
#define     T_RFC_SHIFT                       (24)
#define     T_FAW_SHIFT                       (16)
#define     T_RC_SHIFT                        ( 8)
#define     T_RAS_SHIFT                       ( 0)
#define     T_RFC(x)                          ((x) << T_RFC_SHIFT)
#define     T_FAW(x)                          ((x) << T_FAW_SHIFT)
#define     T_RC(x)                           ((x) << T_RC_SHIFT)
#define     T_RAS(x)                          ((x) << T_RAS_SHIFT)
#define DDR_TIMING2                     0x00000020
#define     T_IDLE_SHIFT                      (16)
#define     T_REF_SHIFT                       ( 0)
#define     T_IDLE(x)                         ((x) << T_IDLE_SHIFT)
#define     T_REF(x)                          ((x) << T_REF_SHIFT)
#define DDR_TIMING3                     0x00000024
#define     T_DLLRST_SHIFT                    (20)
#define     T_PWRON_SHIFT                     ( 0)
#define     T_DLLRST(x)                       ((x) << T_DLLRST_SHIFT)
#define     T_PWRON(x)                        ((x) << T_PWRON_SHIFT)
#define DDR_ZQ_CTRL                     0x00000028
#define     ZQCL_CYCLE_SHIFT                  (20)
#define     ZQCS_CYCLE_SHIFT                  ( 8)
#define     T_ZQ_EXTEND_SHIFT                 ( 4)
#define     ZQC_FROCE_EN_SHIFT                ( 2)
#define     ZQCL_CYCLE_EN_SHIFT               ( 1)
#define     ZQCS_CYCLE_EN_SHIFT               ( 0)
#define     ZQCL_CYCLE(x)                     ((x) << ZQCL_CYCLE_SHIFT)
#define     ZQCS_CYCLE(x)                     ((x) << ZQCS_CYCLE_SHIFT)
#define     T_ZQ_EXTEND(x)                    ((x) << T_ZQ_EXTEND_SHIFT)
#define     ZQC_FROCE_EN(x)                   ((x) << ZQC_FROCE_EN_SHIFT)
#define     ZQCL_CYCLE_EN(x)                  ((x) << ZQCL_CYCLE_EN_SHIFT)
#define     ZQCS_CYCLE_EN(x)                  ((x) << ZQCS_CYCLE_EN_SHIFT)
#define DDR_PWRDWN_MODE                 0x0000002C
#define     PRECHARGED_DLL_OFF_PD_CYCLE_SHIFT (24)
#define     PRECHARGED_DLL_ON_PD_CYCLE_SHIFT  (16)
#define     ACTIVE_PD_CYCLE_SHIFT             ( 8)
#define     PRECHARGED_DLL_OFF_PD_EN_SHIFT    ( 2)
#define     PRECHARGED_DLL_ON_PD_EN_SHIFT     ( 1)
#define     ACTIVE_PD_EN_SHIFT                ( 0)
#define     PRECHARGED_DLL_OFF_PD_CYCLE(x)    ((x) << PRECHARGED_DLL_OFF_PD_CYCLE_SHIFT)
#define     PRECHARGED_DLL_ON_PD_CYCLE(x)     ((x) << PRECHARGED_DLL_ON_PD_CYCLE_SHIFT)
#define     ACTIVE_PD_CYCLE(x)                ((x) << ACTIVE_PD_CYCLE_SHIFT)
#define     PRECHARGED_DLL_OFF_PD_EN(x)       ((x) << PRECHARGED_DLL_OFF_PD_EN_SHIFT)
#define     PRECHARGED_DLL_ON_PD_EN(x)        ((x) << PRECHARGED_DLL_ON_PD_EN_SHIFT)
#define     ACTIVE_PD_EN(x)                   ((x) << ACTIVE_PD_EN_SHIFT)
#define DDR_SELF_REFRESH                0x00000030
#define     DLL_OFF_SRF_CYCLE_SHIFT           (24)
#define     DLL_ON_SRF_CYCLE_SHIFT            (16)
#define     DLL_OFF_SRF_EN_SHIFT              ( 2)
#define     DLL_ON_SRF_EN_SHIFT               ( 1)
#define     DLL_OFF_SRF_CYCLE(x)              ((x) << DLL_OFF_SRF_CYCLE_SHIFT)
#define     DLL_ON_SRF_CYCLE(x)               ((x) << DLL_ON_SRF_CYCLE_SHIFT)
#define     DLL_OFF_SRF_EN(x)                 ((x) << DLL_OFF_SRF_EN_SHIFT)
#define     DLL_ON_SRF_EN(x)                  ((x) << DLL_ON_SRF_EN_SHIFT)
#define DDR_PWR_SELF_TIMING             0x00000034
#define     T_XPDLL_SHIFT                     (24)
#define     T_DLLK_EXTEND_SHIFT               (23)
#define     T_XP_SHIFT                        (20)
#define     T_PD_SHIFT                        (16)
#define     T_XS_SHIFT                        ( 8)
#define     T_CKSREX_SHIFT                    ( 4)
#define     T_CKESR_SHIFT                     ( 0)
#define     T_XPDLL(x)                        ((x) << T_XPDLL_SHIFT)
#define     T_DLLK_EXTEND(x)                  ((x) << T_DLLK_EXTEND_SHIFT)
#define     T_XP(x)                           ((x) << T_XP_SHIFT)
#define     T_PD(x)                           ((x) << T_PD_SHIFT)
#define     T_XS(x)                           ((x) << T_XS_SHIFT)
#define     T_CKSREX(x)                       ((x) << T_CKSREX_SHIFT)
#define     T_CKESR(x)                        ((x) << T_CKESR_SHIFT)
#define DDR_IO_DYNAMIC_CTRL             0x00000038
#define     DYNAMIC_DQ_DQS_RCV_EN_SHIFT       ( 8)
#define     DYNAMIC_DM_DQ_DQS_DRV_EN_SHIFT    ( 7)
#define     DYNAMIC_ADDR_DRV_EN_SHIFT         ( 6)
#define     DYNAMIC_CMD_DRV_EN_SHIFT          ( 5)
#define     DYNAMIC_CLK_DRV_EN_SHIFT          ( 4)
#define     NOP_AC_OE_DISABLE_SHIFT           ( 3)
#define     DYNAMIC_ADDR_OE_EN_SHIFT          ( 2)
#define     DYNAMIC_CMD_OE_EN_SHIFT           ( 1)
#define     DYNAMIC_CLK_OE_EN_SHIFT           ( 0)
#define     DYNAMIC_DQ_DQS_RCV_EN(x)          ((x) << DYNAMIC_DQ_DQS_RCV_EN_SHIFT)
#define     DYNAMIC_DM_DQ_DQS_DRV_EN(x)       ((x) << DYNAMIC_DM_DQ_DQS_DRV_EN_SHIFT)
#define     DYNAMIC_ADDR_DRV_EN(x)            ((x) << DYNAMIC_ADDR_DRV_EN_SHIFT)
#define     DYNAMIC_CMD_DRV_EN(x)             ((x) << DYNAMIC_CMD_DRV_EN_SHIFT)
#define     DYNAMIC_CLK_DRV_EN(x)             ((x) << DYNAMIC_CLK_DRV_EN_SHIFT)
#define     NOP_AC_OE_DISABLE(x)              ((x) << NOP_AC_OE_DISABLE_SHIFT)
#define     DYNAMIC_ADDR_OE_EN(x)             ((x) << DYNAMIC_ADDR_OE_EN_SHIFT)
#define     DYNAMIC_CMD_OE_EN(x)              ((x) << DYNAMIC_CMD_OE_EN_SHIFT)
#define     DYNAMIC_CLK_OE_EN(x)              ((x) << DYNAMIC_CLK_OE_EN_SHIFT)
#define DDR_IO_BIT_ENABLE               0x0000003C
#define     CKE_BIT_EN_SHIFT                  (18)
#define     ODT_BIT_EN_SHIFT                  (17)
#define     CS_N_BIT_EN_SHIFT                 (16)
#define     ADDR_BIT_EN_SHIFT                 ( 0)
#define     CKE_BIT_EN(x)                     ((x) << CKE_BIT_EN_SHIFT)
#define     ODT_BIT_EN(x)                     ((x) << ODT_BIT_EN_SHIFT)
#define     CS_N_BIT_EN(x)                    ((x) << CS_N_BIT_EN_SHIFT)
#define     ADDR_BIT_EN(x)                    ((x) << ADDR_BIT_EN_SHIFT)
#define DDR_DFI_CTRL                    0x00000040
#define     DFI_LP_WAKEUP_SHIFT               ( 8)
#define     DFI_LP_ACK_SHIFT                  ( 6)
#define     DFI_LP_REQ_SHIFT                  ( 5)
#define     DFI_INIT_COMPLETE_SHIFT           ( 4)
#define     DFI_DRAM_CLK_DISABLE_SHIFT        ( 3)
#define     DFI_DATA_HALFWORDLANE_DISABLE_SHIFT  (1)
#define     DFI_INIT_START_SHIFT              (0)
#define     DFI_LP_WAKEUP(x)                  ((x) << DFI_LP_WAKEUP_SHIFT)
#define     DFI_LP_ACK(x)                     ((x) << DFI_LP_ACK_SHIFT)
#define     DFI_LP_REQ(x)                     ((x) << DFI_LP_REQ_SHIFT)
#define     DFI_INIT_COMPLETE(x)              ((x) << DFI_INIT_COMPLETE_SHIFT)
#define     DFI_DRAM_CLK_DISABLE(x)           ((x) << DFI_DRAM_CLK_DISABLE_SHIFT)
#define     DFI_DATA_HALFWORDLANE_DISABLE(x)  ((x) << DFI_DATA_HALFWORDLANE_DISABLE_SHIFT)
#define     DFI_INIT_START(x)                 ((x) << DFI_INIT_START_SHIFT)
#define DDR_LOAD_MODE_CTRL              0x00000044
#define     LMR_CMD_ID_SHIFT                  ( 4)
#define     LMR_CMD_LAUNCH_SHIFT              ( 0)
#define     LMR_CMD_ID(x)                     ((x) << LMR_CMD_ID_SHIFT)
#define     LMR_CMD_LAUNCH(x)                 ((x) << LMR_CMD_LAUNCH_SHIFT)


/*-----------------------------------------------------------------------------
 * DDR 0 parameters
 *---------------------------------------------------------------------------*/
#define DDR0_CTRL_DATA                  (FIXED_WRITE_AS_BL8(CFG_0_FIXED_WRITE_AS_BL8) \
                                       | FIXED_READ_AS_BL8(CFG_0_FIXED_READ_AS_BL8) \
                                       | SLAVE_CORE_SYNC_EN(CFG_0_SLAVE_CORE_SYNC_EN) \
                                       | MCI_IOPHY_UPDATE_WAIT_ACK(CFG_0_MCI_IOPHY_UPDATE_WAIT_ACK) \
                                       | MCI_IOPHY_UPDATE_CNTR(CFG_0_MCI_IOPHY_UPDATE_CNTR) \
                                       | BYP_PUB_EN(CFG_0_BYP_PUB_EN) \
                                       | DDR3_ENABLE(CFG_0_DDR3_ENABLE) \
                                       | DYNAMIC_ODT_EN(CFG_0_DYNAMIC_ODT_EN) \
                                       | PLL_TEST_SEL(CFG_0_PLL_TEST_SEL) \
                                       | MCI_IOPHY_UPDATE_EN(CFG_0_MCI_IOPHY_UPDATE_EN) \
                                       | FAST_SETTING_EN(CFG_0_FAST_SETTING_EN_SHIFT) \
                                       | PWR_ON(CFG_0_PWR_ON) \
                                       | EN(CFG_0_EN))

#define DDR0_DELAY_DATA                 (UDQ_CNT(CFG_0_UDQ_CNT) \
                                       | LDQ_CNT(CFG_0_LDQ_CNT) \
                                       | UDQS_POL(CFG_0_UDQS_POL) \
                                       | LDQS_POL(CFG_0_LDQS_POL))

#define DDR0_SIZE_DATA                  (BASE_ADDR(CFG_0_BASE_ADDR) \
                                       | BANK_NUM(CFG_0_BANK_NUM) \
                                       | ROW_WIDTH(CFG_0_ROW_WIDTH) \
                                       | COL_WIDTH(CFG_0_COL_WIDTH))

#if CONFIG_DDR_TYPE == DDR3
#define DDR0_LOAD_MODE_0_1_DATA         (MR1_QOFF(CFG_0_MR1_QOFF) \
                                       | MR1_TDQS(CFG_0_MR1_TDQS) \
                                       | MR1_LEVEL(CFG_0_MR1_LEVEL) \
                                       | MR1_AL(CFG_0_MR1_AL) \
                                       | MR1_RTT_NORM(CFG_0_MR1_RTT_NORM) \
                                       | MR1_DIC(CFG_0_MR1_DIC) \
                                       | MR1_DLL(CFG_0_MR1_DLL) \
                                       | MR0_PPD(CFG_0_MR0_PPD) \
                                       | MR0_WR(CFG_0_MR0_WR) \
                                       | MR0_DLL(CFG_0_MR0_DLL) \
                                       | MR0_TM(CFG_0_MR0_TM) \
                                       | MR0_RBT(CFG_0_MR0_RBT) \
                                       | MR0_CL(CFG_0_MR0_CL) \
                                       | MR0_BL(CFG_0_MR0_BL))
#elif CONFIG_DDR_TYPE == DDR2
#define DDR0_LOAD_MODE_0_1_DATA         (MR1_QOFF(CFG_0_MR1_QOFF) \
                                       | MR1_RDQS(CFG_0_MR1_RDQS) \
                                       | MR1_DOSN(CFG_0_MR1_DOSN) \
                                       | MR1_OCD_PROG(CFG_0_MR1_OCD_PROG) \
                                       | MR1_AL(CFG_0_MR1_AL) \
                                       | MR1_RTT_NORM(CFG_0_MR1_RTT_NORM) \
                                       | MR1_DIC(CFG_0_MR1_DIC) \
                                       | MR1_DLL(CFG_0_MR1_DLL) \
                                       | MR0_PD(CFG_0_MR0_PD) \
                                       | MR0_WR(CFG_0_MR0_WR) \
                                       | MR0_DLL(CFG_0_MR0_DLL) \
                                       | MR0_TM(CFG_0_MR0_TM) \
                                       | MR0_CL(CFG_0_MR0_CL) \
                                       | MR0_BT(CFG_0_MR0_BT) \
                                       | MR0_BL(CFG_0_MR0_BL))
#endif /* DDR2 */

#if CONFIG_DDR_TYPE == DDR3
#define DDR0_LOAD_MODE_2_3_DATA         (MR2_RTT_WR(CFG_0_MR2_RTT_WR) \
                                       | MR2_SRT(CFG_0_MR2_SRT) \
                                       | MR2_ASR(CFG_0_MR2_ASR) \
                                       | MR2_CWL(CFG_0_MR2_CWL) \
                                       | MR2_PASR(CFG_0_MR2_PASR) \
                                       | MR3_MPR(CFG_0_MR3_MPR) \
                                       | MR3_MPR_LOC(CFG_0_MR3_MPR_LOC))
#elif CONFIG_DDR_TYPE == DDR2
#define DDR0_LOAD_MODE_2_3_DATA         (MR2_SRF(CFG_0_MR2_SRF) \
                                       | MR2_DCC(CFG_0_MR2_DCC) \
                                       | MR2_PASR(CFG_0_MR2_PASR))
#endif /* DDR2 */

#define DDR0_TIMING0_DATA               (T_MOD(CFG_0_T_MOD) \
                                       | T_WTR(CFG_0_T_WTR) \
                                       | T_RTP(CFG_0_T_RTP) \
                                       | T_RCD(CFG_0_T_RCD) \
                                       | T_RRD(CFG_0_T_RRD) \
                                       | T_MRD(CFG_0_T_MRD) \
                                       | T_RP(CFG_0_T_RP))

#define DDR0_TIMING1_DATA               (T_RFC(CFG_0_T_RFC) \
                                       | T_FAW(CFG_0_T_FAW) \
                                       | T_RC( CFG_0_T_RC ) \
                                       | T_RAS(CFG_0_T_RAS))

#define DDR0_TIMING2_DATA               (T_IDLE(CFG_0_T_IDLE) \
                                       | T_REF(CFG_0_T_REF))

#define DDR0_TIMING3_DATA               (T_DLLRST(CFG_0_T_DLLRST) \
                                       | T_PWRON(CFG_0_T_PWRON))


#ifdef DYNAMIC_LOAD_TIMING	
#define M390A_DDR0_TIMING0_DATA         (T_MOD(CFG_390A_T_MOD) \
                                       | T_WTR(CFG_390A_T_WTR) \
                                       | T_RTP(CFG_390A_T_RTP) \
                                       | T_RCD(CFG_390A_T_RCD) \
                                       | T_RRD(CFG_390A_T_RRD) \
                                       | T_MRD(CFG_390A_T_MRD) \
                                       | T_RP(CFG_390A_T_RP))

#define M390A_DDR0_TIMING1_DATA         (T_RFC(CFG_390A_T_RFC) \
                                       | T_FAW(CFG_390A_T_FAW) \
                                       | T_RC( CFG_390A_T_RC ) \
                                       | T_RAS(CFG_390A_T_RAS))

#define M390A_DDR0_TIMING2_DATA         (T_IDLE(CFG_390A_T_IDLE) \
                                       | T_REF(CFG_390A_T_REF_NORMAL))

#define M390A_DDR0_TIMING3_DATA         (T_DLLRST(CFG_390A_T_DLLRST) \
                                       | T_PWRON(CFG_390A_T_PWRON))
#if CONFIG_DDR_TYPE == DDR3
#define M390_DDR0_TIMING0_DATA          (T_MOD(CFG_390_T_MOD) \
                                       | T_WTR(CFG_390_T_WTR) \
                                       | T_RTP(CFG_390_T_RTP) \
                                       | T_RCD(CFG_390_T_RCD) \
                                       | T_RRD(CFG_390_T_RRD) \
                                       | T_MRD(CFG_390_T_MRD) \
                                       | T_RP(CFG_390_T_RP))
#else
#define M390_DDR0_TIMING0_DATA          (T_MOD(CFG_390_T_MOD) \
                                       | T_WTR(CFG_390_T_WTR) \
                                       | T_RTP(CFG_390_T_RTP) \
                                       | T_RCD(CFG_390_T_RCD) \
                                       | T_RRD(CFG_390_T_RRD) \
                                       | T_MRD(CFG_390_T_MRD) \
                                       | T_RP(CFG_390_T_RP))
#endif

#define M390_DDR0_TIMING1_DATA          (T_RFC(CFG_390_T_RFC) \
                                       | T_FAW(CFG_390_T_FAW) \
                                       | T_RC( CFG_390_T_RC ) \
                                       | T_RAS(CFG_390_T_RAS))

#define M390_DDR0_TIMING2_DATA          (T_IDLE(CFG_390_T_IDLE) \
                                       | T_REF(CFG_390_T_REF_NORMAL))

#define M390_DDR0_TIMING3_DATA          (T_DLLRST(CFG_390_T_DLLRST) \
                                       | T_PWRON(CFG_390_T_PWRON))

#define M370_DDR0_TIMING0_DATA          (T_MOD(CFG_370_T_MOD) \
                                       | T_WTR(CFG_370_T_WTR) \
                                       | T_RTP(CFG_370_T_RTP) \
                                       | T_RCD(CFG_370_T_RCD) \
                                       | T_RRD(CFG_370_T_RRD) \
                                       | T_MRD(CFG_370_T_MRD) \
                                       | T_RP(CFG_370_T_RP))

#define M370_DDR0_TIMING1_DATA          (T_RFC(CFG_370_T_RFC) \
                                       | T_FAW(CFG_370_T_FAW) \
                                       | T_RC( CFG_370_T_RC ) \
                                       | T_RAS(CFG_370_T_RAS))

#define M370_DDR0_TIMING2_DATA          (T_IDLE(CFG_370_T_IDLE) \
                                       | T_REF(CFG_370_T_REF_NORMAL))

#define M370_DDR0_TIMING3_DATA          (T_DLLRST(CFG_370_T_DLLRST) \
                                       | T_PWRON(CFG_370_T_PWRON))

#define M330_DDR0_TIMING0_DATA          (T_MOD(CFG_330_T_MOD) \
                                       | T_WTR(CFG_330_T_WTR) \
                                       | T_RTP(CFG_330_T_RTP) \
                                       | T_RCD(CFG_330_T_RCD) \
                                       | T_RRD(CFG_330_T_RRD) \
                                       | T_MRD(CFG_330_T_MRD) \
                                       | T_RP(CFG_330_T_RP))

#define M330_DDR0_TIMING1_DATA          (T_RFC(CFG_330_T_RFC) \
                                       | T_FAW(CFG_330_T_FAW) \
                                       | T_RC( CFG_330_T_RC ) \
                                       | T_RAS(CFG_330_T_RAS))

#define M330_DDR0_TIMING2_DATA          (T_IDLE(CFG_330_T_IDLE) \
                                       | T_REF(CFG_330_T_REF_NORMAL))

#define M330_DDR0_TIMING3_DATA          (T_DLLRST(CFG_330_T_DLLRST) \
                                       | T_PWRON(CFG_330_T_PWRON))

#endif

#define DDR0_ZQ_CTRL_DATA               (ZQCL_CYCLE(CFG_0_ZQCL_CYCLE) \
                                       | ZQCS_CYCLE(CFG_0_ZQCS_CYCLE) \
                                       | T_ZQ_EXTEND(CFG_0_T_ZQ_EXTEND) \
                                       | ZQC_FROCE_EN(CFG_0_ZQC_FROCE_EN) \
                                       | ZQCL_CYCLE_EN(CFG_0_ZQCL_CYCLE_EN) \
                                       | ZQCS_CYCLE_EN(CFG_0_ZQCS_CYCLE_EN))

#define DDR0_PWRDWN_MODE_DATA           (PRECHARGED_DLL_OFF_PD_CYCLE(CFG_0_PRECHARGED_DLL_OFF_PD_CYCLE) \
                                       | PRECHARGED_DLL_ON_PD_CYCLE(CFG_0_PRECHARGED_DLL_ON_PD_CYCLE) \
                                       | ACTIVE_PD_CYCLE(CFG_0_ACTIVE_PD_CYCLE) \
                                       | PRECHARGED_DLL_OFF_PD_EN(CFG_0_PRECHARGED_DLL_OFF_PD_EN) \
                                       | PRECHARGED_DLL_ON_PD_EN(CFG_0_PRECHARGED_DLL_ON_PD_EN) \
                                       | ACTIVE_PD_EN(CFG_0_ACTIVE_PD_EN))

#define DDR0_SELF_REFRESH_DATA          (DLL_OFF_SRF_CYCLE(CFG_0_DLL_OFF_SRF_CYCLE) \
                                       | DLL_ON_SRF_CYCLE(CFG_0_DLL_ON_SRF_CYCLE) \
                                       | DLL_OFF_SRF_EN(CFG_0_DLL_OFF_SRF_EN) \
                                       | DLL_ON_SRF_EN(CFG_0_DLL_ON_SRF_EN))

#define DDR0_PWR_SELF_TIMING_DATA       (T_XPDLL(CFG_0_T_XPDLL) \
                                       | T_DLLK_EXTEND(CFG_0_T_DLLK_EXTEND) \
                                       | T_XP(CFG_0_T_XP) \
                                       | T_PD(CFG_0_T_PD) \
                                       | T_XS(CFG_0_T_XS) \
                                       | T_CKSREX(CFG_0_T_CKSREX) \
                                       | T_CKESR(CFG_0_T_CKESR))

#define DDR0_IO_DYNAMIC_CTRL_DATA       (DYNAMIC_DQ_DQS_RCV_EN(CFG_0_DYNAMIC_DQ_DQS_RCV_EN) \
                                       | DYNAMIC_DM_DQ_DQS_DRV_EN(CFG_0_DYNAMIC_DM_DQ_DQS_DRV_EN) \
                                       | DYNAMIC_ADDR_DRV_EN(CFG_0_DYNAMIC_ADDR_DRV_EN) \
                                       | DYNAMIC_CMD_DRV_EN(CFG_0_DYNAMIC_CMD_DRV_EN) \
                                       | DYNAMIC_CLK_DRV_EN(CFG_0_DYNAMIC_CLK_DRV_EN) \
                                       | NOP_AC_OE_DISABLE(CFG_0_NOP_AC_OE_DISABLE) \
                                       | DYNAMIC_ADDR_OE_EN(CFG_0_DYNAMIC_ADDR_OE_EN) \
                                       | DYNAMIC_CMD_OE_EN(CFG_0_DYNAMIC_CMD_OE_EN) \
                                       | DYNAMIC_CLK_OE_EN(CFG_0_DYNAMIC_CLK_OE_EN))

#define DDR0_IO_BIT_ENABLE_DATA         (CKE_BIT_EN(CFG_0_CKE_BIT_EN) \
                                       | ODT_BIT_EN(CFG_0_ODT_BIT_EN) \
                                       | CS_N_BIT_EN(CFG_0_CS_N_BIT_EN) \
                                       | ADDR_BIT_EN(CFG_0_ADDR_BIT_EN))

#define DDR0_DFI_CTRL_DATA              (DFI_LP_WAKEUP(CFG_0_DFI_LP_WAKEUP) \
                                       | DFI_LP_ACK(CFG_0_DFI_LP_ACK) \
                                       | DFI_LP_REQ(CFG_0_DFI_LP_REQ) \
                                       | DFI_INIT_COMPLETE(CFG_0_DFI_INIT_COMPLETE) \
                                       | DFI_DRAM_CLK_DISABLE(CFG_0_DFI_DRAM_CLK_DISABLE) \
                                       | DFI_DATA_HALFWORDLANE_DISABLE(CFG_0_DFI_DATA_HALFWORDLANE_DISABLE) \
                                       | DFI_INIT_START(CFG_0_DFI_INIT_START))

/*-----------------------------------------------------------------------------
 * DDR 1 parameters
 *---------------------------------------------------------------------------*/
#define DDR1_CTRL_DATA                  (FIXED_WRITE_AS_BL8(CFG_1_FIXED_WRITE_AS_BL8) \
                                       | FIXED_READ_AS_BL8(CFG_1_FIXED_READ_AS_BL8) \
                                       | SLAVE_CORE_SYNC_EN(CFG_1_SLAVE_CORE_SYNC_EN) \
                                       | MCI_IOPHY_UPDATE_WAIT_ACK(CFG_1_MCI_IOPHY_UPDATE_WAIT_ACK) \
                                       | MCI_IOPHY_UPDATE_CNTR(CFG_1_MCI_IOPHY_UPDATE_CNTR) \
                                       | BYP_PUB_EN(CFG_1_BYP_PUB_EN) \
                                       | DDR3_ENABLE(CFG_1_DDR3_ENABLE) \
                                       | DYNAMIC_ODT_EN(CFG_1_DYNAMIC_ODT_EN) \
                                       | PLL_TEST_SEL(CFG_1_PLL_TEST_SEL) \
                                       | MCI_IOPHY_UPDATE_EN(CFG_1_MCI_IOPHY_UPDATE_EN) \
                                       | FAST_SETTING_EN(CFG_1_FAST_SETTING_EN_SHIFT) \
                                       | PWR_ON(CFG_1_PWR_ON) \
                                       | EN(CFG_1_EN))

#define DDR1_DELAY_DATA                 (UDQ_CNT(CFG_1_UDQ_CNT) \
                                       | LDQ_CNT(CFG_1_LDQ_CNT) \
                                       | UDQS_POL(CFG_1_UDQS_POL) \
                                       | LDQS_POL(CFG_1_LDQS_POL))

#define DDR1_SIZE_DATA                  (BASE_ADDR(CFG_1_BASE_ADDR) \
                                       | BANK_NUM(CFG_1_BANK_NUM) \
                                       | ROW_WIDTH(CFG_1_ROW_WIDTH) \
                                       | COL_WIDTH(CFG_1_COL_WIDTH))

#if CONFIG_DDR_TYPE == DDR3
#define DDR1_LOAD_MODE_0_1_DATA         (MR1_QOFF(CFG_1_MR1_QOFF) \
                                       | MR1_TDQS(CFG_1_MR1_TDQS) \
                                       | MR1_LEVEL(CFG_1_MR1_LEVEL) \
                                       | MR1_AL(CFG_1_MR1_AL) \
                                       | MR1_RTT_NORM(CFG_1_MR1_RTT_NORM) \
                                       | MR1_DIC(CFG_1_MR1_DIC) \
                                       | MR1_DLL(CFG_1_MR1_DLL) \
                                       | MR0_PPD(CFG_1_MR0_PPD) \
                                       | MR0_WR(CFG_1_MR0_WR) \
                                       | MR0_DLL(CFG_1_MR0_DLL) \
                                       | MR0_TM(CFG_1_MR0_TM) \
                                       | MR0_RBT(CFG_1_MR0_RBT) \
                                       | MR0_CL(CFG_1_MR0_CL) \
                                       | MR0_BL(CFG_1_MR0_BL))
#elif CONFIG_DDR_TYPE == DDR2
#define DDR1_LOAD_MODE_0_1_DATA         (MR1_QOFF(CFG_1_MR1_QOFF) \
                                       | MR1_RDQS(CFG_1_MR1_RDQS) \
                                       | MR1_DOSN(CFG_1_MR1_DOSN) \
                                       | MR1_OCD_PROG(CFG_1_MR1_OCD_PROG) \
                                       | MR1_AL(CFG_1_MR1_AL) \
                                       | MR1_RTT_NORM(CFG_1_MR1_RTT_NORM) \
                                       | MR1_DIC(CFG_1_MR1_DIC) \
                                       | MR1_DLL(CFG_1_MR1_DLL) \
                                       | MR0_PD(CFG_1_MR0_PD) \
                                       | MR0_WR(CFG_1_MR0_WR) \
                                       | MR0_DLL(CFG_1_MR0_DLL) \
                                       | MR0_TM(CFG_1_MR0_TM) \
                                       | MR0_CL(CFG_1_MR0_CL) \
                                       | MR0_BT(CFG_1_MR0_BT) \
                                       | MR0_BL(CFG_1_MR0_BL))
#endif /* DDR2 */

#if CONFIG_DDR_TYPE == DDR3
#define DDR1_LOAD_MODE_2_3_DATA         (MR2_RTT_WR(CFG_1_MR2_RTT_WR) \
                                       | MR2_SRT(CFG_1_MR2_SRT) \
                                       | MR2_ASR(CFG_1_MR2_ASR) \
                                       | MR2_CWL(CFG_1_MR2_CWL) \
                                       | MR2_PASR(CFG_1_MR2_PASR) \
                                       | MR3_MPR(CFG_1_MR3_MPR) \
                                       | MR3_MPR_LOC(CFG_1_MR3_MPR_LOC))
#elif CONFIG_DDR_TYPE == DDR2
#define DDR1_LOAD_MODE_2_3_DATA         (MR2_SRF(CFG_1_MR2_SRF) \
                                       | MR2_DCC(CFG_1_MR2_DCC) \
                                       | MR2_PASR(CFG_1_MR2_PASR))
#endif /* DDR2 */


#define DDR1_TIMING0_DATA               (T_MOD(CFG_1_T_MOD) \
                                       | T_WTR(CFG_1_T_WTR) \
                                       | T_RTP(CFG_1_T_RTP) \
                                       | T_RCD(CFG_1_T_RCD) \
                                       | T_RRD(CFG_1_T_RRD) \
                                       | T_MRD(CFG_1_T_MRD) \
                                       | T_RP(CFG_1_T_RP))

#define DDR1_TIMING1_DATA               (T_RFC(CFG_1_T_RFC) \
                                       | T_FAW(CFG_1_T_FAW) \
                                       | T_RC( CFG_1_T_RC ) \
                                       | T_RAS(CFG_1_T_RAS))

#define DDR1_TIMING2_DATA               (T_IDLE(CFG_1_T_IDLE) \
                                       | T_REF(CFG_1_T_REF))

#define DDR1_TIMING3_DATA               (T_DLLRST(CFG_1_T_DLLRST) \
                                       | T_PWRON(CFG_1_T_PWRON))

#define DDR1_ZQ_CTRL_DATA               (ZQCL_CYCLE(CFG_1_ZQCL_CYCLE) \
                                       | ZQCS_CYCLE(CFG_1_ZQCS_CYCLE) \
                                       | T_ZQ_EXTEND(CFG_1_T_ZQ_EXTEND) \
                                       | ZQC_FROCE_EN(CFG_1_ZQC_FROCE_EN) \
                                       | ZQCL_CYCLE_EN(CFG_1_ZQCL_CYCLE_EN) \
                                       | ZQCS_CYCLE_EN(CFG_1_ZQCS_CYCLE_EN))

#define DDR1_PWRDWN_MODE_DATA           (PRECHARGED_DLL_OFF_PD_CYCLE(CFG_1_PRECHARGED_DLL_OFF_PD_CYCLE) \
                                       | PRECHARGED_DLL_ON_PD_CYCLE(CFG_1_PRECHARGED_DLL_ON_PD_CYCLE) \
                                       | ACTIVE_PD_CYCLE(CFG_1_ACTIVE_PD_CYCLE) \
                                       | PRECHARGED_DLL_OFF_PD_EN(CFG_1_PRECHARGED_DLL_OFF_PD_EN) \
                                       | PRECHARGED_DLL_ON_PD_EN(CFG_1_PRECHARGED_DLL_ON_PD_EN) \
                                       | ACTIVE_PD_EN(CFG_1_ACTIVE_PD_EN))

#define DDR1_SELF_REFRESH_DATA          (DLL_OFF_SRF_CYCLE(CFG_1_DLL_OFF_SRF_CYCLE) \
                                       | DLL_ON_SRF_CYCLE(CFG_1_DLL_ON_SRF_CYCLE) \
                                       | DLL_OFF_SRF_EN(CFG_1_DLL_OFF_SRF_EN) \
                                       | DLL_ON_SRF_EN(CFG_1_DLL_ON_SRF_EN))

#define DDR1_PWR_SELF_TIMING_DATA       (T_XPDLL(CFG_1_T_XPDLL) \
                                       | T_DLLK_EXTEND(CFG_1_T_DLLK_EXTEND) \
                                       | T_XP(CFG_1_T_XP) \
                                       | T_PD(CFG_1_T_PD) \
                                       | T_XS(CFG_1_T_XS) \
                                       | T_CKSREX(CFG_1_T_CKSREX) \
                                       | T_CKESR(CFG_1_T_CKESR))

#define DDR1_IO_DYNAMIC_CTRL_DATA       (DYNAMIC_DQ_DQS_RCV_EN(CFG_1_DYNAMIC_DQ_DQS_RCV_EN) \
                                       | DYNAMIC_DM_DQ_DQS_DRV_EN(CFG_1_DYNAMIC_DM_DQ_DQS_DRV_EN) \
                                       | DYNAMIC_ADDR_DRV_EN(CFG_1_DYNAMIC_ADDR_DRV_EN) \
                                       | DYNAMIC_CMD_DRV_EN(CFG_1_DYNAMIC_CMD_DRV_EN) \
                                       | DYNAMIC_CLK_DRV_EN(CFG_1_DYNAMIC_CLK_DRV_EN) \
                                       | NOP_AC_OE_DISABLE(CFG_1_NOP_AC_OE_DISABLE) \
                                       | DYNAMIC_ADDR_OE_EN(CFG_1_DYNAMIC_ADDR_OE_EN) \
                                       | DYNAMIC_CMD_OE_EN(CFG_1_DYNAMIC_CMD_OE_EN) \
                                       | DYNAMIC_CLK_OE_EN(CFG_1_DYNAMIC_CLK_OE_EN))

#define DDR1_IO_BIT_ENABLE_DATA         (CKE_BIT_EN(CFG_1_CKE_BIT_EN) \
                                       | ODT_BIT_EN(CFG_1_ODT_BIT_EN) \
                                       | CS_N_BIT_EN(CFG_1_CS_N_BIT_EN) \
                                       | ADDR_BIT_EN(CFG_1_ADDR_BIT_EN))

#define DDR1_DFI_CTRL_DATA              (DFI_LP_WAKEUP(CFG_1_DFI_LP_WAKEUP) \
                                       | DFI_LP_ACK(CFG_1_DFI_LP_ACK) \
                                       | DFI_LP_REQ(CFG_1_DFI_LP_REQ) \
                                       | DFI_INIT_COMPLETE(CFG_1_DFI_INIT_COMPLETE) \
                                       | DFI_DRAM_CLK_DISABLE(CFG_1_DFI_DRAM_CLK_DISABLE) \
                                       | DFI_DATA_HALFWORDLANE_DISABLE(CFG_1_DFI_DATA_HALFWORDLANE_DISABLE) \
                                       | DFI_INIT_START(CFG_1_DFI_INIT_START))


/*=============================================================================
 * AHBC
 *============================================================================*/
 
#define AHB_SLAVE_USBC_MMR              (40)
#define AHB_SLAVE_PCIEC_MMR             (41)
#define AHB_SLAVE_MSHC_0_MEM            (44)
#define AHB_SLAVE_MSHC_1_MEM            (45)
#define AHB_SLAVE_GMAC_MEM	            (46)
#define AHB_SLAVE_CAST_NFC_MEM          (48)
#define AHB_SLAVE_PCIEC_MEM             (57)
#define AHB_SLAVE_APB3C_MMR             (58)
#define AHB_SLAVE_APBC_MMR_MEM          (59)
#define AHB_SLAVE_BRC                   (60)
#define AHB_SLAVE_DDRSDMC_1_MEM         (62)
#define AHB_SLAVE_DDRSDMC_0_MEM         (63)

#define AHBC_CTRL                       (0x04)
#define     REMAP_SHIFT                    (2)
#define     REMAP(x)                       ((x) << REMAP_SHIFT)
#define AHBC_SLAVE_BASESIZE(n)          ((n)*4 + 0x8)


/*=============================================================================
 * BRC
 *============================================================================*/
#define BRC_ROM_SIZE                    (SZ_2K)
#define BRC_RAM_SIZE                    (SZ_4K + SZ_512)

#define BRC_CTRL                        (0x04)


/*=============================================================================
 * USBC
 *============================================================================*/
#define USBC_RAM_SIZE                   (SZ_4K)


/*=============================================================================
 * SYSC
 *============================================================================*/
#define SYSC_CNT_CTRL                        (0X18)
#define SYSC_CNT_LOW                         (0X1C)
#define SYSC_CNT_HIGH                        (0X20)
#define SYSC_CLK_EN_CTRL_0                   (0x24)
#define     SSIC_CLK_EN_SHIFT                  (31)
#define     PLLC_CLK_EN_SHIFT                  (30)
#define     PCIEC_CLK_EN_SHIFT                 (29)
#define     NFC_CLK_EN_SHIFT                   (28)
#define     MSHC_1_CLK_EN_SHIFT                (27)
#define     MSHC_0_CLK_EN_SHIFT                (26)
#define     MEAE_CLK_EN_SHIFT                  (25)
#define     JEBE_CLK_EN_SHIFT                  (24)
#define     IRE_CLK_EN_SHIFT                   (23)
#define     IRDAC_CLK_EN_SHIFT                 (22)
#define     INTC_CLK_EN_SHIFT                  (21)
#define     IBPE_CLK_EN_SHIFT                  (20)
#define     I2SSC_4_CLK_EN_SHIFT               (19)
#define     I2SSC_3_CLK_EN_SHIFT               (18)
#define     I2SSC_2_CLK_EN_SHIFT               (17)
#define     I2SSC_1_CLK_EN_SHIFT               (16)
#define     I2SSC_0_CLK_EN_SHIFT               (15)
#define     H4EE_CLK_EN_SHIFT                  (13)
#define     GPIOC_CLK_EN_SHIFT                 (12)
#define     GMAC_CLK_EN_SHIFT                  (11)
#define     DMAC_CLK_EN_SHIFT                  (10)
#define     DIE_CLK_EN_SHIFT                   ( 9)
#define     DCE_CLK_EN_SHIFT                   ( 7)
#define     BRC_CLK_EN_SHIFT                   ( 6)
#define     APB3C_CLK_EN_SHIFT                 ( 4)
#define     AGPOC_CLK_EN_SHIFT                 ( 0)
#define     SSIC_CLK_EN(x)                     ((x) << SSIC_CLK_EN_SHIFT)
#define     PLLC_CLK_EN(x)                     ((x) << PLLC_CLK_EN_SHIFT)
#define     PCIEC_CLK_EN(x)                    ((x) << PCIEC_CLK_EN_SHIFT)
#define     NFC_CLK_EN(x)                      ((x) << NFC_CLK_EN_SHIFT)
#define     MSHC_1_CLK_EN(x)                   ((x) << MSHC_1_CLK_EN_SHIFT)
#define     MSHC_0_CLK_EN(x)                   ((x) << MSHC_0_CLK_EN_SHIFT)
#define     MEAE_CLK_EN(x)                     ((x) << MEAE_CLK_EN_SHIFT)
#define     JEBE_CLK_EN(x)                     ((x) << JEBE_CLK_EN_SHIFT)
#define     IRE_CLK_EN(x)                      ((x) << IRE_CLK_EN_SHIFT)
#define     IRDAC_CLK_EN(x)                    ((x) << IRDAC_CLK_EN_SHIFT)
#define     INTC_CLK_EN(x)                     ((x) << INTC_CLK_EN_SHIFT)
#define     IBPE_CLK_EN(x)                     ((x) << IBPE_CLK_EN_SHIFT)
#define     I2SSC_4_CLK_EN(x)                  ((x) << I2SSC_4_CLK_EN_SHIFT)
#define     I2SSC_3_CLK_EN(x)                  ((x) << I2SSC_3_CLK_EN_SHIFT)
#define     I2SSC_2_CLK_EN(x)                  ((x) << I2SSC_2_CLK_EN_SHIFT)
#define     I2SSC_1_CLK_EN(x)                  ((x) << I2SSC_1_CLK_EN_SHIFT)
#define     I2SSC_0_CLK_EN(x)                  ((x) << I2SSC_0_CLK_EN_SHIFT)
#define     H4EE_CLK_EN(x)                     ((x) << H4EE_CLK_EN_SHIFT)
#define     GPIOC_CLK_EN(x)                    ((x) << GPIOC_CLK_EN_SHIFT)
#define     GMAC_CLK_EN(x)                     ((x) << GMAC_CLK_EN_SHIFT)
#define     DMAC_CLK_EN(x)                     ((x) << DMAC_CLK_EN_SHIFT)
#define     DIE_CLK_EN(x)                      ((x) << DIE_CLK_EN_SHIFT)
#define     DCE_CLK_EN(x)                      ((x) << DCE_CLK_EN_SHIFT)
#define     BRC_CLK_EN(x)                      ((x) << BRC_CLK_EN_SHIFT)
#define     APB3C_CLK_EN(x)                    ((x) << APB3C_CLK_EN_SHIFT)
#define     AGPOC_CLK_EN(x)                    ((x) << AGPOC_CLK_EN_SHIFT)
#define SYSC_CLK_EN_CTRL_1                   (0x28)
#define     I2SSC_RX_CLK_EN_SHIFT              (12)
#define     I2SSC_TX_CLK_EN_SHIFT              (11)
#define     WDTC_CLK_EN_SHIFT                  (10)
#define     VOC_CLK_EN_SHIFT                   ( 9)
#define     VIC_CLK_EN_SHIFT                   ( 7)
#define     USBC_CLK_EN_SHIFT                  ( 6)
#define     UARTC_3_CLK_EN_SHIFT               ( 5)
#define     UARTC_2_CLK_EN_SHIFT               ( 4)
#define     UARTC_1_CLK_EN_SHIFT               ( 3)
#define     UARTC_0_CLK_EN_SHIFT               ( 2)
#define     TMRC_CLK_EN_SHIFT                  ( 1)
#define     SYSC_CLK_EN_SHIFT                  ( 0)
#define     I2SSC_RX_CLK_EN(x)                 ((x) << I2SSC_RX_CLK_EN_SHIFT)
#define     I2SSC_TX_CLK_EN(x)                 ((x) << I2SSC_TX_CLK_EN_SHIFT)
#define     WDTC_CLK_EN(x)                     ((x) << WDTC_CLK_EN_SHIFT)
#define     VOC_CLK_EN(x)                      ((x) << VOC_CLK_EN_SHIFT)
#define     VIC_CLK_EN(x)                      ((x) << VIC_CLK_EN_SHIFT)
#define     USBC_CLK_EN(x)                     ((x) << USBC_CLK_EN_SHIFT)
#define     UARTC_3_CLK_EN(x)                  ((x) << UARTC_3_CLK_EN_SHIFT)
#define     UARTC_2_CLK_EN(x)                  ((x) << UARTC_2_CLK_EN_SHIFT)
#define     UARTC_1_CLK_EN(x)                  ((x) << UARTC_1_CLK_EN_SHIFT)
#define     UARTC_0_CLK_EN(x)                  ((x) << UARTC_0_CLK_EN_SHIFT)
#define     TMRC_CLK_EN(x)                     ((x) << TMRC_CLK_EN_SHIFT)
#define     SYSC_CLK_EN(x)                     ((x) << SYSC_CLK_EN_SHIFT)
#define SYSC_MON_CLK_SEL                     (0x30)
#define     MON_CLK_1_SHIFT                    ( 8)
#define     MON_CLK_0_SHIFT                    ( 0)
#define     MON_CLK_1(x)                       ((x) << MON_CLK_1_SHIFT)
#define     MON_CLK_0(x)                       ((x) << MON_CLK_0_SHIFT)
#define SYSC_MODULE			                     (0x3C)
#define SYSC_PAD_EN_CTRL                     (0x44)
#define     WDTC_PAD_EN_SHIFT                  (27)
#define     VOC_PAD_EN_SHIFT                   (24)
#define     VIC_DEV_1_PAD_EN_SHIFT             (20)
#define     VIC_DEV_0_PAD_EN_SHIFT             (19)
#define     USBC_PAD_EN_SHIFT                  (18)
#define     UARTC_3_PAD_EN_SHIFT               (17)
#define     UARTC_2_PAD_EN_SHIFT               (16)
#define     UARTC_1_PAD_EN_SHIFT               (15)
#define     UARTC_0_PAD_EN_SHIFT               (14)
#define     SSIC_PAD_EN_SHIFT                  (13)
#define     NFC_PAD_EN_SHIFT                   (12)
#define     MSHC_1_PAD_EN_SHIFT                (11)
#define     MSHC_0_PAD_EN_SHIFT                (10)
#define     I2SSC_4_PAD_EN_SHIFT               ( 9)
#define     I2SSC_3_PAD_EN_SHIFT               ( 8)
#define     I2SSC_2_PAD_EN_SHIFT               ( 7)
#define     I2SSC_1_PAD_EN_SHIFT               ( 6)
#define     I2SSC_0_PAD_EN_SHIFT               ( 5)
#define     HOSTC_PAD_EN_SHIFT                 ( 4)
#define     GMAC_PAD_EN_SHIFT                  ( 2)
#define     WDTC_PAD_EN(x)                     ((x) << WDTC_PAD_EN_SHIFT)
#define     VOC_PAD_EN(x)                      ((x) << VOC_PAD_EN_SHIFT)
#define     VIC_DEV_1_PAD_EN(x)                ((x) << VIC_DEV_1_PAD_EN_SHIFT)
#define     VIC_DEV_0_PAD_EN(x)                ((x) << VIC_DEV_0_PAD_EN_SHIFT)
#define     USBC_PAD_EN(x)                     ((x) << USBC_PAD_EN_SHIFT)
#define     UARTC_3_PAD_EN(x)                  ((x) << UARTC_3_PAD_EN_SHIFT)
#define     UARTC_2_PAD_EN(x)                  ((x) << UARTC_2_PAD_EN_SHIFT)
#define     UARTC_1_PAD_EN(x)                  ((x) << UARTC_1_PAD_EN_SHIFT)
#define     UARTC_0_PAD_EN(x)                  ((x) << UARTC_0_PAD_EN_SHIFT)
#define     SSIC_PAD_EN(x)                     ((x) << SSIC_PAD_EN_SHIFT)
#define     NFC_PAD_EN(x)                      ((x) << NFC_PAD_EN_SHIFT)
#define     MSHC_1_PAD_EN(x)                   ((x) << MSHC_1_PAD_EN_SHIFT)
#define     MSHC_0_PAD_EN(x)                   ((x) << MSHC_0_PAD_EN_SHIFT)
#define     I2SSC_4_PAD_EN(x)                  ((x) << I2SSC_4_PAD_EN_SHIFT)
#define     I2SSC_3_PAD_EN(x)                  ((x) << I2SSC_3_PAD_EN_SHIFT)
#define     I2SSC_2_PAD_EN(x)                  ((x) << I2SSC_2_PAD_EN_SHIFT)
#define     I2SSC_1_PAD_EN(x)                  ((x) << I2SSC_1_PAD_EN_SHIFT)
#define     I2SSC_0_PAD_EN(x)                  ((x) << I2SSC_0_PAD_EN_SHIFT)
#define     HOSTC_PAD_EN(x)                    ((x) << HOSTC_PAD_EN_SHIFT)
#define     GMAC_PAD_EN(x)                     ((x) << GMAC_PAD_EN_SHIFT)
#define SYSC_SLEW_CTRL                       (0x4C)
#define     VOC_SLEW_RATE_CTRL_SHIFT           ( 1)
#define     GMAC_SLEW_RATE_CTRL_SHIFT          ( 0)
#define     VOC_SLEW_RATE_CTRL(x)              ((x) << VOC_SLEW_RATE_CTRL_SHIFT)
#define     GMAC_SLEW_RATE_CTRL(x)             ((x) << GMAC_SLEW_RATE_CTRL_SHIFT)
#define SYSC_PULL_CTRL                       (0x50)
#define     WDTC_CMD_PUP_EN_SHIFT              (15)
#define     UARTC_3_SDA_PUP_EN_SHIFT           (14)
#define     UARTC_2_SDA_PUP_EN_SHIFT           (13)
#define     UARTC_1_SDA_PUP_EN_SHIFT           (12)
#define     UARTC_0_SDA_PUP_EN_SHIFT           (11)
#define     SSIC_SEL_PUP_EN_SHIFT              (10)
#define     NFC_DATA_PUP_EN_SHIFT              ( 9)
#define     NFC_BUSY_PUP_EN_SHIFT              ( 8)
#define     NFC_COM_PUP_EN_SHIFT               ( 7)
#define     MSHC_1_DATA_PUP_EN_SHIFT           ( 6)
#define     MSHC_1_CMD_PUP_EN_SHIFT            ( 5)
#define     MSHC_1_COM_PUP_EN_SHIFT            ( 4)
#define     MSHC_0_DATA_PUP_EN_SHIFT           ( 3)
#define     MSHC_0_CMD_PUP_EN_SHIFT            ( 2)
#define     MSHC_0_COM_PUP_EN_SHIFT            ( 1)
#define     HOSTC_COM_PUP_EN_SHIFT             ( 0)
#define     WDTC_CMD_PUP_EN(x)                 ((x) << WDTC_CMD_PUP_EN_SHIFT)
#define     UARTC_3_SDA_PUP_EN(x)              ((x) << UARTC_3_SDA_PUP_EN_SHIFT)
#define     UARTC_2_SDA_PUP_EN(x)              ((x) << UARTC_2_SDA_PUP_EN_SHIFT)
#define     UARTC_1_SDA_PUP_EN(x)              ((x) << UARTC_1_SDA_PUP_EN_SHIFT)
#define     UARTC_0_SDA_PUP_EN(x)              ((x) << UARTC_0_SDA_PUP_EN_SHIFT)
#define     SSIC_SEL_PUP_EN(x)                 ((x) << SSIC_SEL_PUP_EN_SHIFT)
#define     NFC_DATA_PUP_EN(x)                 ((x) << NFC_DATA_PUP_EN_SHIFT)
#define     NFC_BUSY_PUP_EN(x)                 ((x) << NFC_BUSY_PUP_EN_SHIFT)
#define     NFC_COM_PUP_EN(x)                  ((x) << NFC_COM_PUP_EN_SHIFT)
#define     MSHC_1_DATA_PUP_EN(x)              ((x) << MSHC_1_DATA_PUP_EN_SHIFT)
#define     MSHC_1_CMD_PUP_EN(x)               ((x) << MSHC_1_CMD_PUP_EN_SHIFT)
#define     MSHC_1_COM_PUP_EN(x)               ((x) << MSHC_1_COM_PUP_EN_SHIFT)
#define     MSHC_0_DATA_PUP_EN(x)              ((x) << MSHC_0_DATA_PUP_EN_SHIFT)
#define     MSHC_0_CMD_PUP_EN(x)               ((x) << MSHC_0_CMD_PUP_EN_SHIFT)
#define     MSHC_0_COM_PUP_EN(x)               ((x) << MSHC_0_COM_PUP_EN_SHIFT)
#define     HOSTC_COM_PUP_EN(x)                ((x) << HOSTC_COM_PUP_EN_SHIFT)
#define SYSC_DRV_STRENGTH_CTRL_0             (0x54)
#define     AGPOC_GPIOC_DRV_STR_0(n)           (((xcat(xcat(CFG_AGPOC_GPIOC_,n),_DRV_STR)) & 0x1) << (n))
#define SYSC_DRV_STRENGTH_CTRL_1             (0x58)
#define     AGPOC_GPIOC_DRV_STR_1(n)           ((((xcat(xcat(CFG_AGPOC_GPIOC_,n),_DRV_STR)) >> 1) & 0x1) << (n))
#define SYSC_DRV_STRENGTH_CTRL_2             (0x5C)
#define     SYSC_DRV_STR_SHIFT                 (26)
#define     WDTC_DRV_STR_SHIFT                 (24)
#define     VOC_DRV_STR_SHIFT                  (22)
#define     USBC_DRV_STR_SHIFT                 (20)
#define     UARTC_3_DRV_STR_SHIFT              (18)
#define     UARTC_2_DRV_STR_SHIFT              (16)
#define     UARTC_1_DRV_STR_SHIFT              (14)
#define     UARTC_0_DRV_STR_SHIFT              (12)
#define     SSIC_DRV_STR_SHIFT                 (10)
#define     NFC_DRV_STR_SHIFT                  ( 8)
#define     MSHC_1_DRV_STR_SHIFT               ( 6)
#define     MSHC_0_DRV_STR_SHIFT               ( 4)
#define     I2SSC_DRV_STR_SHIFT                ( 2)
#define     HOSTC_DRV_STR_SHIFT                ( 0)
#define     SYSC_DRV_STR(x)                    ((x) << SYSC_DRV_STR_SHIFT)
#define     WDTC_DRV_STR(x)                    ((x) << WDTC_DRV_STR_SHIFT)
#define     VOC_DRV_STR(x)                     ((x) << VOC_DRV_STR_SHIFT)
#define     USBC_DRV_STR(x)                    ((x) << USBC_DRV_STR_SHIFT)
#define     UARTC_3_DRV_STR(x)                 ((x) << UARTC_3_DRV_STR_SHIFT)
#define     UARTC_2_DRV_STR(x)                 ((x) << UARTC_2_DRV_STR_SHIFT)
#define     UARTC_1_DRV_STR(x)                 ((x) << UARTC_1_DRV_STR_SHIFT)
#define     UARTC_0_DRV_STR(x)                 ((x) << UARTC_0_DRV_STR_SHIFT)
#define     SSIC_DRV_STR(x)                    ((x) << SSIC_DRV_STR_SHIFT)
#define     NFC_DRV_STR(x)                     ((x) << NFC_DRV_STR_SHIFT)
#define     MSHC_1_DRV_STR(x)                  ((x) << MSHC_1_DRV_STR_SHIFT)
#define     MSHC_0_DRV_STR(x)                  ((x) << MSHC_0_DRV_STR_SHIFT)
#define     I2SSC_DRV_STR(x)                   ((x) << I2SSC_DRV_STR_SHIFT)
#define     HOSTC_DRV_STR(x)                   ((x) << HOSTC_DRV_STR_SHIFT)

#define SYSC_CLK_EN_CTRL_0_DATA         (SSIC_CLK_EN(CFG_SSIC_CLK_EN) \
                                       | PLLC_CLK_EN(CFG_PLLC_CLK_EN) \
                                       | PCIEC_CLK_EN(CFG_PCIEC_CLK_EN) \
                                       | NFC_CLK_EN(CFG_NFC_CLK_EN) \
                                       | MSHC_1_CLK_EN(CFG_MSHC_1_CLK_EN) \
                                       | MSHC_0_CLK_EN(CFG_MSHC_0_CLK_EN) \
                                       | MEAE_CLK_EN(CFG_MEAE_CLK_EN) \
                                       | JEBE_CLK_EN(CFG_JEBE_CLK_EN) \
                                       | IRE_CLK_EN(CFG_IRE_CLK_EN) \
                                       | IRDAC_CLK_EN(CFG_IRDAC_CLK_EN) \
                                       | INTC_CLK_EN(CFG_INTC_CLK_EN) \
                                       | IBPE_CLK_EN(CFG_IBPE_CLK_EN) \
                                       | I2SSC_4_CLK_EN(CFG_I2SSC_4_CLK_EN) \
                                       | I2SSC_3_CLK_EN(CFG_I2SSC_3_CLK_EN) \
                                       | I2SSC_2_CLK_EN(CFG_I2SSC_2_CLK_EN) \
                                       | I2SSC_1_CLK_EN(CFG_I2SSC_1_CLK_EN) \
                                       | I2SSC_0_CLK_EN(CFG_I2SSC_0_CLK_EN) \
                                       | H4EE_CLK_EN(CFG_H4EE_CLK_EN) \
                                       | GPIOC_CLK_EN(CFG_GPIOC_CLK_EN) \
                                       | GMAC_CLK_EN(CFG_GMAC_CLK_EN) \
                                       | DMAC_CLK_EN(CFG_DMAC_CLK_EN) \
                                       | DIE_CLK_EN(CFG_DIE_CLK_EN) \
                                       | DCE_CLK_EN(CFG_DCE_CLK_EN) \
                                       | BRC_CLK_EN(CFG_BRC_CLK_EN) \
                                       | APB3C_CLK_EN(CFG_APB3C_CLK_EN) \
                                       | AGPOC_CLK_EN(CFG_AGPOC_CLK_EN))
#define SYSC_CLK_EN_CTRL_1_DATA         (I2SSC_RX_CLK_EN(CFG_I2SSC_RX_CLK_EN) \
                                       | I2SSC_TX_CLK_EN(CFG_I2SSC_TX_CLK_EN) \
                                       | WDTC_CLK_EN(CFG_WDTC_CLK_EN) \
                                       | VOC_CLK_EN(CFG_VOC_CLK_EN) \
                                       | VIC_CLK_EN(CFG_VIC_CLK_EN) \
                                       | USBC_CLK_EN(CFG_USBC_CLK_EN) \
                                       | UARTC_3_CLK_EN(CFG_UARTC_3_CLK_EN) \
                                       | UARTC_2_CLK_EN(CFG_UARTC_2_CLK_EN) \
                                       | UARTC_1_CLK_EN(CFG_UARTC_1_CLK_EN) \
                                       | UARTC_0_CLK_EN(CFG_UARTC_0_CLK_EN) \
                                       | TMRC_CLK_EN(CFG_TMRC_CLK_EN) \
                                       | SYSC_CLK_EN(CFG_SYSC_CLK_EN))
#define SYSC_MON_CLK_SEL_DATA           (MON_CLK_1(CFG_MON_CLK_1) \
                                       | MON_CLK_0(CFG_MON_CLK_0))
#define SYSC_PAD_EN_CTRL_DATA           (WDTC_PAD_EN(CFG_WDTC_PAD_EN) \
                                       | VOC_PAD_EN(CFG_VOC_PAD_EN) \
                                       | VIC_DEV_1_PAD_EN(CFG_VIC_DEV_1_PAD_EN) \
                                       | VIC_DEV_0_PAD_EN(CFG_VIC_DEV_0_PAD_EN) \
                                       | USBC_PAD_EN(CFG_USBC_PAD_EN) \
                                       | UARTC_3_PAD_EN(CFG_UARTC_3_PAD_EN) \
                                       | UARTC_2_PAD_EN(CFG_UARTC_2_PAD_EN) \
                                       | UARTC_1_PAD_EN(CFG_UARTC_1_PAD_EN) \
                                       | UARTC_0_PAD_EN(CFG_UARTC_0_PAD_EN) \
                                       | SSIC_PAD_EN(CFG_SSIC_PAD_EN) \
                                       | NFC_PAD_EN(CFG_NFC_PAD_EN) \
                                       | MSHC_1_PAD_EN(CFG_MSHC_1_PAD_EN) \
                                       | MSHC_0_PAD_EN(CFG_MSHC_0_PAD_EN) \
                                       | I2SSC_4_PAD_EN(CFG_I2SSC_4_PAD_EN) \
                                       | I2SSC_3_PAD_EN(CFG_I2SSC_3_PAD_EN) \
                                       | I2SSC_2_PAD_EN(CFG_I2SSC_2_PAD_EN) \
                                       | I2SSC_1_PAD_EN(CFG_I2SSC_1_PAD_EN) \
                                       | I2SSC_0_PAD_EN(CFG_I2SSC_0_PAD_EN) \
                                       | HOSTC_PAD_EN(CFG_HOSTC_PAD_EN) \
                                       | GMAC_PAD_EN(CFG_GMAC_PAD_EN))
#define SYSC_SLEW_CTRL_DATA             (VOC_SLEW_RATE_CTRL(CFG_VOC_SLEW_RATE_CTRL) \
                                       | GMAC_SLEW_RATE_CTRL(CFG_GMAC_SLEW_RATE_CTRL))
#define SYSC_PULL_CTRL_DATA             (WDTC_CMD_PUP_EN(CFG_WDTC_CMD_PUP_EN) \
                                       | UARTC_3_SDA_PUP_EN(CFG_UARTC_3_SDA_PUP_EN) \
                                       | UARTC_2_SDA_PUP_EN(CFG_UARTC_2_SDA_PUP_EN) \
                                       | UARTC_1_SDA_PUP_EN(CFG_UARTC_1_SDA_PUP_EN) \
                                       | UARTC_0_SDA_PUP_EN(CFG_UARTC_0_SDA_PUP_EN) \
                                       | SSIC_SEL_PUP_EN(CFG_SSIC_SEL_PUP_EN) \
                                       | NFC_DATA_PUP_EN(CFG_NFC_DATA_PUP_EN) \
                                       | NFC_BUSY_PUP_EN(CFG_NFC_BUSY_PUP_EN) \
                                       | NFC_COM_PUP_EN(CFG_NFC_COM_PUP_EN) \
                                       | MSHC_1_DATA_PUP_EN(CFG_MSHC_1_DATA_PUP_EN) \
                                       | MSHC_1_CMD_PUP_EN(CFG_MSHC_1_CMD_PUP_EN) \
                                       | MSHC_1_COM_PUP_EN(CFG_MSHC_1_COM_PUP_EN) \
                                       | MSHC_0_DATA_PUP_EN(CFG_MSHC_0_DATA_PUP_EN) \
                                       | MSHC_0_CMD_PUP_EN(CFG_MSHC_0_CMD_PUP_EN) \
                                       | MSHC_0_COM_PUP_EN(CFG_MSHC_0_COM_PUP_EN) \
                                       | HOSTC_COM_PUP_EN(CFG_HOSTC_COM_PUP_EN))
#define SYSC_DRV_STRENGTH_CTRL_0_DATA   (AGPOC_GPIOC_DRV_STR_0(0) \
                                       | AGPOC_GPIOC_DRV_STR_0(1) \
                                       | AGPOC_GPIOC_DRV_STR_0(2) \
                                       | AGPOC_GPIOC_DRV_STR_0(3) \
                                       | AGPOC_GPIOC_DRV_STR_0(4) \
                                       | AGPOC_GPIOC_DRV_STR_0(5) \
                                       | AGPOC_GPIOC_DRV_STR_0(6) \
                                       | AGPOC_GPIOC_DRV_STR_0(7) \
                                       | AGPOC_GPIOC_DRV_STR_0(8) \
                                       | AGPOC_GPIOC_DRV_STR_0(9) \
                                       | AGPOC_GPIOC_DRV_STR_0(10) \
                                       | AGPOC_GPIOC_DRV_STR_0(11) \
                                       | AGPOC_GPIOC_DRV_STR_0(12) \
                                       | AGPOC_GPIOC_DRV_STR_0(13) \
                                       | AGPOC_GPIOC_DRV_STR_0(14) \
                                       | AGPOC_GPIOC_DRV_STR_0(15) \
                                       | AGPOC_GPIOC_DRV_STR_0(16) \
                                       | AGPOC_GPIOC_DRV_STR_0(17) \
                                       | AGPOC_GPIOC_DRV_STR_0(18) \
                                       | AGPOC_GPIOC_DRV_STR_0(19))
#define SYSC_DRV_STRENGTH_CTRL_1_DATA   (AGPOC_GPIOC_DRV_STR_1(0) \
                                       | AGPOC_GPIOC_DRV_STR_1(1) \
                                       | AGPOC_GPIOC_DRV_STR_1(2) \
                                       | AGPOC_GPIOC_DRV_STR_1(3) \
                                       | AGPOC_GPIOC_DRV_STR_1(4) \
                                       | AGPOC_GPIOC_DRV_STR_1(5) \
                                       | AGPOC_GPIOC_DRV_STR_1(6) \
                                       | AGPOC_GPIOC_DRV_STR_1(7) \
                                       | AGPOC_GPIOC_DRV_STR_1(8) \
                                       | AGPOC_GPIOC_DRV_STR_1(9) \
                                       | AGPOC_GPIOC_DRV_STR_1(10) \
                                       | AGPOC_GPIOC_DRV_STR_1(11) \
                                       | AGPOC_GPIOC_DRV_STR_1(12) \
                                       | AGPOC_GPIOC_DRV_STR_1(13) \
                                       | AGPOC_GPIOC_DRV_STR_1(14) \
                                       | AGPOC_GPIOC_DRV_STR_1(15) \
                                       | AGPOC_GPIOC_DRV_STR_1(16) \
                                       | AGPOC_GPIOC_DRV_STR_1(17) \
                                       | AGPOC_GPIOC_DRV_STR_1(18) \
                                       | AGPOC_GPIOC_DRV_STR_1(19))
#define SYSC_DRV_STRENGTH_CTRL_2_DATA   (SYSC_DRV_STR(CFG_SYSC_DRV_STR) \
                                       | WDTC_DRV_STR(CFG_WDTC_DRV_STR) \
                                       | VOC_DRV_STR(CFG_VOC_DRV_STR) \
                                       | USBC_DRV_STR(CFG_USBC_DRV_STR) \
                                       | UARTC_3_DRV_STR(CFG_UARTC_3_DRV_STR) \
                                       | UARTC_2_DRV_STR(CFG_UARTC_2_DRV_STR) \
                                       | UARTC_1_DRV_STR(CFG_UARTC_1_DRV_STR) \
                                       | UARTC_0_DRV_STR(CFG_UARTC_0_DRV_STR) \
                                       | SSIC_DRV_STR(CFG_SSIC_DRV_STR) \
                                       | NFC_DRV_STR(CFG_NFC_DRV_STR) \
                                       | MSHC_1_DRV_STR(CFG_MSHC_1_DRV_STR) \
                                       | MSHC_0_DRV_STR(CFG_MSHC_0_DRV_STR) \
                                       | I2SSC_DRV_STR(CFG_I2SSC_DRV_STR) \
                                       | HOSTC_DRV_STR(CFG_HOSTC_DRV_STR))


/*=============================================================================
 * DDR PUB
 *============================================================================*/
#define PIR                                 (0x004)
#define     INITBYP_SHIFT                      (31)
#define     ZCALBYP_SHIFT                      (30)
#define     DCALBYP_SHIFT                      (29)
#define     LOCKBYP_SHIFT                      (28)
#define     CLRSR_SHIFT                        (27)
#define     CTLDINIT_SHIFT                     (18)
#define     PLLBYP_SHIFT                       (17)
#define     ICPC_SHIFT                         (16)
#define     WREYE_SHIFT                        (15)
#define     RDEYE_SHIFT                        (14)
#define     WRDSKW_SHIFT                       (13)
#define     RDDSKW_SHIFT                       (12)
#define     WLADJ_SHIFT                        (11)
#define     QSGATE_SHIFT                       (10)
#define     WL_SHIFT                           ( 9)
#define     DRAMINIT_SHIFT                     ( 8)
#define     DRAMRST_SHIFT                      ( 7)
#define     PHYRST_SHIFT                       ( 6)
#define     DCAL_SHIFT                         ( 5)
#define     PLLINIT_SHIFT                      ( 4)
#define     ZCAL_SHIFT                         ( 1)
#define     INIT_SHIFT                         ( 0)
#define     INITBYP(x)                         ((x) << INITBYP_SHIFT)
#define     ZCALBYP(x)                         ((x) << ZCALBYP_SHIFT)
#define     DCALBYP(x)                         ((x) << DCALBYP_SHIFT)
#define     LOCKBYP(x)                         ((x) << LOCKBYP_SHIFT)
#define     CLRSR(x)                           ((x) << CLRSR_SHIFT)
#define     CTLDINIT(x)                        ((x) << CTLDINIT_SHIFT)
#define     PLLBYP(x)                          ((x) << PLLBYP_SHIFT)
#define     ICPC(x)                            ((x) << ICPC_SHIFT)
#define     WREYE(x)                           ((x) << WREYE_SHIFT)
#define     RDEYE(x)                           ((x) << RDEYE_SHIFT)
#define     WRDSKW(x)                          ((x) << WRDSKW_SHIFT)
#define     RDDSKW(x)                          ((x) << RDDSKW_SHIFT)
#define     WLADJ(x)                           ((x) << WLADJ_SHIFT)
#define     QSGATE(x)                          ((x) << QSGATE_SHIFT)
#define     WL(x)                              ((x) << WL_SHIFT)
#define     DRAMINIT(x)                        ((x) << DRAMINIT_SHIFT)
#define     DRAMRST(x)                         ((x) << DRAMRST_SHIFT)
#define     PHYRST(x)                          ((x) << PHYRST_SHIFT)
#define     DCAL(x)                            ((x) << DCAL_SHIFT)
#define     PLLINIT(x)                         ((x) << PLLINIT_SHIFT)
#define     ZCAL(x)                            ((x) << ZCAL_SHIFT)
#define     INIT(x)                            ((x) << INIT_SHIFT)
#define PGCR0                               (0x008)
#define     CKEN_SHIFT                         (26)
#define     DTOSEL_SHIFT                       (14)
#define     OSCWDL_SHIFT                       (12)
#define     OSCDIV_SHIFT                       ( 9)
#define     OSCEN_SHIFT                        ( 8)
#define     DLTST_SHIFT                        ( 7)
#define     DLTMODE_SHIFT                      ( 6)
#define     RDBVT_SHIFT                        ( 5)
#define     WDBVT_SHIFT                        ( 4)
#define     RGLVT_SHIFT                        ( 3)
#define     RDLVT_SHIFT                        ( 2)
#define     WDLVT_SHIFT                        ( 1)
#define     WLLVT_SHIFT                        ( 0)
#define     CKEN(x)                            ((x) << CKEN_SHIFT)
#define     DTOSEL(x)                          ((x) << DTOSEL_SHIFT)
#define     OSCWDL(x)                          ((x) << OSCWDL_SHIFT)
#define     OSCDIV(x)                          ((x) << OSCDIV_SHIFT)
#define     OSCEN(x)                           ((x) << OSCEN_SHIFT)
#define     DLTST(x)                           ((x) << DLTST_SHIFT)
#define     DLTMODE(x)                         ((x) << DLTMODE_SHIFT)
#define     RDBVT(x)                           ((x) << RDBVT_SHIFT)
#define     WDBVT(x)                           ((x) << WDBVT_SHIFT)
#define     RGLVT(x)                           ((x) << RGLVT_SHIFT)
#define     RDLVT(x)                           ((x) << RDLVT_SHIFT)
#define     WDLVT(x)                           ((x) << WDLVT_SHIFT)
#define     WLLVT(x)                           ((x) << WLLVT_SHIFT)
#define PGCR1                               (0x00c)
#define     LBMODE_SHIFT                       (31)
#define     LBGDQS_SHIFT                       (29)
#define     LBDQSS_SHIFT                       (28)
#define     IOLB_SHIFT                         (27)
#define     INHVT_SHIFT                        (26)
#define     PHYHRST_SHIFT                      (25)
#define     ZCKSEL_SHIFT                       (23)
#define     DLDLMT_SHIFT                       (15)
#define     FDEPTH_SHIFT                       (13)
#define     LPFDEPTH_SHIFT                     (11)
#define     LPFEN_SHIFT                        (10)
#define     MDLEN_PGCR1_SHIFT                  ( 9)
#define     IODDRM_SHIFT                       ( 7)
#define     WLSELT_SHIFT                       ( 6)
#define     WLRANK_SHIFT                       ( 4)
#define     WLUNCRT_SHIFT                      ( 3)
#define     WLSTEP_SHIFT                       ( 2)
#define     WLFULL_SHIFT                       ( 1)
#define     PDDISDX_SHIFT                      ( 0)
#define     LBMODE(x)                          ((x) << LBMODE_SHIFT)
#define     LBGDQS(x)                          ((x) << LBGDQS_SHIFT)
#define     LBDQSS(x)                          ((x) << LBDQSS_SHIFT)
#define     IOLB(x)                            ((x) << IOLB_SHIFT)
#define     INHVT(x)                           ((x) << INHVT_SHIFT)
#define     PHYHRST(x)                         ((x) << PHYHRST_SHIFT)
#define     ZCKSEL(x)                          ((x) << ZCKSEL_SHIFT)
#define     DLDLMT(x)                          ((x) << DLDLMT_SHIFT)
#define     FDEPTH(x)                          ((x) << FDEPTH_SHIFT)
#define     LPFDEPTH(x)                        ((x) << LPFDEPTH_SHIFT)
#define     LPFEN(x)                           ((x) << LPFEN_SHIFT)
#define     MDLEN_PGCR1(x)                     ((x) << MDLEN_PGCR1_SHIFT)
#define     IODDRM(x)                          ((x) << IODDRM_SHIFT)
#define     WLSELT(x)                          ((x) << WLSELT_SHIFT)
#define     WLRANK(x)                          ((x) << WLRANK_SHIFT)
#define     WLUNCRT(x)                         ((x) << WLUNCRT_SHIFT)
#define     WLSTEP(x)                          ((x) << WLSTEP_SHIFT)
#define     WLFULL(x)                          ((x) << WLFULL_SHIFT)
#define     PDDISDX(x)                         ((x) << PDDISDX_SHIFT)
#define PGSR0                               (0x010)
#define     WEERR_SHIFT                        (27)
#define     REERR_SHIFT                        (26)
#define     WDERR_SHIFT                        (25)
#define     RDERR_SHIFT                        (24)
#define     WLAERR_SHIFT                       (23)
#define     QSGERR_SHIFT                       (22)
#define     WLERR_SHIFT                        (21)
#define     ZCERR_SHIFT                        (19)
#define     DCERR_SHIFT                        (18)
#define     PLERR_SHIFT                        (17)
#define     IERR_SHIFT                         (16)
#define     WEDONE_SHIFT                       (11)
#define     REDONE_SHIFT                       (10)
#define     WDDONE_SHIFT                       ( 9)
#define     RDDONE_SHIFT                       ( 8)
#define     WLADONE_SHIFT                      ( 7)
#define     QSGDONE_SHIFT                      ( 6)
#define     WLDONE_SHIFT                       ( 5)
#define     DIDONE_SHIFT                       ( 4)
#define     ZCDONE_SHIFT                       ( 3)
#define     DCDONE_SHIFT                       ( 2)
#define     PLDONE_SHIFT                       ( 1)
#define     IDONE_SHIFT                        ( 0)
#define     WEERR(x)                           ((x) << WEERR_SHIFT)
#define     REERR(x)                           ((x) << REERR_SHIFT)
#define     WDERR(x)                           ((x) << WDERR_SHIFT)
#define     RDERR(x)                           ((x) << RDERR_SHIFT)
#define     WLAERR(x)                          ((x) << WLAERR_SHIFT)
#define     QSGERR(x)                          ((x) << QSGERR_SHIFT)
#define     WLERR(x)                           ((x) << WLERR_SHIFT)
#define     ZCERR(x)                           ((x) << ZCERR_SHIFT)
#define     DCERR(x)                           ((x) << DCERR_SHIFT)
#define     PLERR(x)                           ((x) << PLERR_SHIFT)
#define     IERR(x)                            ((x) << IERR_SHIFT)
#define     WEDONE(x)                          ((x) << WEDONE_SHIFT)
#define     REDONE(x)                          ((x) << REDONE_SHIFT)
#define     WDDONE(x)                          ((x) << WDDONE_SHIFT)
#define     RDDONE(x)                          ((x) << RDDONE_SHIFT)
#define     WLADONE(x)                         ((x) << WLADONE_SHIFT)
#define     QSGDONE(x)                         ((x) << QSGDONE_SHIFT)
#define     WLDONE(x)                          ((x) << WLDONE_SHIFT)
#define     DIDONE(x)                          ((x) << DIDONE_SHIFT)
#define     ZCDONE(x)                          ((x) << ZCDONE_SHIFT)
#define     DCDONE(x)                          ((x) << DCDONE_SHIFT)
#define     PLDONE(x)                          ((x) << PLDONE_SHIFT)
#define     IDONE(x)                           ((x) << IDONE_SHIFT)
#define PGSR1                               	 (0x014)
#define     DLTCODE_SHIFT                      ( 1)
#define     DLTDONE_SHIFT                      ( 0)
#define     DLTCODE(x)                         ((x) << DLTCODE_SHIFT)
#define     DLTDONE(x)                         ((x) << DLTDONE_SHIFT)
#define PTR0                                (0x01C)
#define     T_PLLPD_SHIFT                      (21)
#define     T_PLLGS_SHIFT                      ( 6)
#define     T_PHYRST_SHIFT                     ( 0)
#define     T_PLLPD(x)                         ((x) << T_PLLPD_SHIFT)
#define     T_PLLGS(x)                         ((x) << T_PLLGS_SHIFT)
#define     T_PHYRST(x)                        ((x) << T_PHYRST_SHIFT)
#define PTR1                                (0x020)
#define     T_PLLLOCK_SHIFT                    (16)
#define     T_PLLRST_SHIFT                     ( 0)
#define     T_PLLLOCK(x)                       ((x) << T_PLLLOCK_SHIFT)
#define     T_PLLRST(x)                        ((x) << T_PLLRST_SHIFT)
#define PTR2                                (0x024)
#define     T_WLDLYS_SHIFT                     (15)
#define     T_CALH_SHIFT                       (10)
#define     T_CALS_SHIFT                       ( 5)
#define     T_CALON_SHIFT                      ( 0)
#define     T_WLDLYS(x)                        ((x) << T_WLDLYS_SHIFT)
#define     T_CALH(x)                          ((x) << T_CALH_SHIFT)
#define     T_CALS(x)                          ((x) << T_CALS_SHIFT)
#define     T_CALON(x)                         ((x) << T_CALON_SHIFT)
#define PTR3                                (0x028)
#define     T_DINIT1_SHIFT                     (20)
#define     T_DINIT0_SHIFT                     ( 0)
#define     T_DINIT1(x)                        ((x) << T_DINIT1_SHIFT)
#define     T_DINIT0(x)                        ((x) << T_DINIT0_SHIFT)
#define PTR4                                (0x02C)
#define     T_DINIT3_SHIFT                      (18)
#define     T_DINIT2_SHIFT                      ( 0)
#define     T_DINIT3(x)                         ((x) << T_DINIT3_SHIFT)
#define     T_DINIT2(x)                         ((x) << T_DINIT2_SHIFT)
#define ACMDLR                              (0x030)
#define     IPRD_SHIFT                         ( 0)
#define     TPRD_SHIFT                         ( 8)
#define     MDLD_SHIFT                         (16)
#define     IPRD(x)                            ((x) << IPRD_SHIFT)
#define     TPRD(x)                            ((x) << TPRD_SHIFT)
#define     MDLD(x)                            ((x) << MDLD_SHIFT)
#define ACBDLR                              (0x034)
#define     CK0BD_SHIFT                        ( 0)
#define     CK1BD_SHIFT                        ( 6)
#define     CK2BD_SHIFT                        (12)
#define     ACBD_SHIFT                         (18)
#define     CK0BD(x)                           ((x) << CK0BD_SHIFT)
#define     CK1BD(x)                           ((x) << CK1BD_SHIFT)
#define     CK2BD(x)                           ((x) << CK2BD_SHIFT)
#define     ACBD(x)                            ((x) << ACBD_SHIFT)
#define ACIOCR                              (0x038)
#define     ACSR_SHIFT                         (30)
#define     RSTIOM_SHIFT                       (29)
#define     RSTPDR_SHIFT                       (28)
#define     RSTPDD_SHIFT                       (27)
#define     RSTODT_SHIFT                       (26)
#define     RANKPDR_SHIFT                      (22)
#define     CSPDD_SHIFT                        (18)
#define     RANKODT_SHIFT                      (14)
#define     CKPDR_SHIFT                        (11)
#define     CKPDD_SHIFT                        ( 8)
#define     CKODT_SHIFT                        ( 5)
#define     ACPDR_SHIFT                        ( 4)
#define     ACPDD_SHIFT                        ( 3)
#define     ACODT_SHIFT                        ( 2)
#define     ACOE_SHIFT                         ( 1)
#define     ACIOM_SHIFT                        ( 0)
#define     ACSR(x)                            ((x) << ACSR_SHIFT)
#define     RSTIOM(x)                          ((x) << RSTIOM_SHIFT)
#define     RSTPDR(x)                          ((x) << RSTPDR_SHIFT)
#define     RSTPDD(x)                          ((x) << RSTPDD_SHIFT)
#define     RSTODT(x)                          ((x) << RSTODT_SHIFT)
#define     RANKPDR(x)                         ((x) << RANKPDR_SHIFT)
#define     CSPDD(x)                           ((x) << CSPDD_SHIFT)
#define     RANKODT(x)                         ((x) << RANKODT_SHIFT)
#define     CKPDR(x)                           ((x) << CKPDR_SHIFT)
#define     CKPDD(x)                           ((x) << CKPDD_SHIFT)
#define     CKODT(x)                           ((x) << CKODT_SHIFT)
#define     ACPDR(x)                           ((x) << ACPDR_SHIFT)
#define     ACPDD(x)                           ((x) << ACPDD_SHIFT)
#define     ACODT(x)                           ((x) << ACODT_SHIFT)
#define     ACOE(x)                            ((x) << ACOE_SHIFT)
#define     ACIOM(x)                           ((x) << ACIOM_SHIFT)
#define DXCCR                               (0x03C)
#define     DXSR_SHIFT                         (13)
#define     DQSNRES_SHIFT                      ( 9)
#define     DQSRES_SHIFT                       ( 5)
#define     DXPDR_DXCCR_SHIFT                  ( 4)
#define     DXPDD_DXCCR_SHIFT                  ( 3)
#define     MDLEN_DXCCR_SHIFT                  ( 2)
#define     DXIOM_DXCCR_SHIFT                  ( 1)
#define     DXODT_SHIFT                        ( 0)
#define     DXSR(x)                            ((x) << DXSR_SHIFT)
#define     DQSNRES(x)                         ((x) << DQSNRES_SHIFT)
#define     DQSRES(x)                          ((x) << DQSRES_SHIFT)
#define     DXPDR_DXCCR(x)                     ((x) << DXPDR_DXCCR_SHIFT)
#define     DXPDD_DXCCR(x)                     ((x) << DXPDD_DXCCR_SHIFT)
#define     MDLEN_DXCCR(x)                     ((x) << MDLEN_DXCCR_SHIFT)
#define     DXIOM_DXCCR(x)                     ((x) << DXIOM_DXCCR_SHIFT)
#define     DXODT(x)                           ((x) << DXODT_SHIFT)
#define DSGCR                               (0x040)
#define     CKEOE_SHIFT                        (31)
#define     RSTOE_SHIFT                        (30)
#define     ODTOE_SHIFT                        (29)
#define     CKOE_SHIFT                         (28)
#define     ODTPDD_SHIFT                       (24)
#define     CKEPDD_SHIFT                       (20)
#define     SDRMODE_SHIFT                      (19)
#define     RRMODE_SHIFT                       (18)
#define     DQSGX_SHIFT                        ( 6)
#define     CUAEN_SHIFT                        ( 5)
#define     LPPLLPD_SHIFT                      ( 4)
#define     LPIOPD_SHIFT                       ( 3)
#define     ZUEN_SHIFT                         ( 2)
#define     BDISEN_SHIFT                       ( 1)
#define     PUREN_SHIFT                        ( 0)
#define     CKEOE(x)                           ((x) << CKEOE_SHIFT)
#define     RSTOE(x)                           ((x) << RSTOE_SHIFT)
#define     ODTOE(x)                           ((x) << ODTOE_SHIFT)
#define     CKOE(x)                            ((x) << CKOE_SHIFT)
#define     ODTPDD(x)                          ((x) << ODTPDD_SHIFT)
#define     CKEPDD(x)                          ((x) << CKEPDD_SHIFT)
#define     SDRMODE(x)                         ((x) << SDRMODE_SHIFT)
#define     RRMODE(x)                          ((x) << RRMODE_SHIFT)
#define     DQSGX(x)                           ((x) << DQSGX_SHIFT)
#define     CUAEN(x)                           ((x) << CUAEN_SHIFT)
#define     LPPLLPD(x)                         ((x) << LPPLLPD_SHIFT)
#define     LPIOPD(x)                          ((x) << LPIOPD_SHIFT)
#define     ZUEN(x)                            ((x) << ZUEN_SHIFT)
#define     BDISEN(x)                          ((x) << BDISEN_SHIFT)
#define     PUREN(x)                           ((x) << PUREN_SHIFT)
#define DCR                                    (0x044)
#define     RDIMM_SHIFT                        (30)
#define     UDIMM_SHIFT                        (29)
#define     DDR2T_SHIFT                        (28)
#define     NOSRA_SHIFT                        (27)
#define     BYTEMASK_SHIFT                     (10)
#define     MPRDQ_SHIFT                        (7)
#define     PDQ_SHIFT                          (4)
#define     DDR8BNK_SHIFT                      (3)
#define     DDRMD_SHIFT                        (0)
#define     DCR_RDIMM(x)                       ((x) << RDIMM_SHIFT)
#define     DCR_UDIMM(x)                       ((x) << UDIMM_SHIFT)
#define     DCR_DDR2T(x)                       ((x) << DDR2T_SHIFT)
#define     DCR_NOSRA(x)                       ((x) << NOSRA_SHIFT)
#define     DCR_BYTEMASK(x)                    ((x) << BYTEMASK_SHIFT)
#define     DCR_MPRDQ(x)                       ((x) << MPRDQ_SHIFT)
#define     DCR_PDQ(x)                         ((x) << PDQ_SHIFT)
#define     DCR_DDR8BNK(x)                     ((x) << DDR8BNK_SHIFT)
#define     DCR_DDRMD(x)                       ((x) << DDRMD_SHIFT)
#define DTPR0                               (0x048)
#define     DTPR_T_RC_SHIFT                    (26)
#define     DTPR_T_RRD_SHIFT                   (22)
#define     DTPR_T_RAS_SHIFT                   (16)
#define     DTPR_T_RCD_SHIFT                   (12)
#define     DTPR_T_RP_SHIFT                    ( 8)
#define     DTPR_T_WTR_SHIFT                   ( 4)
#define     DTPR_T_RTP_SHIFT                   ( 0)
#define     DTPR_T_RC(x)                       ((x) << DTPR_T_RC_SHIFT)
#define     DTPR_T_RRD(x)                      ((x) << DTPR_T_RRD_SHIFT)
#define     DTPR_T_RAS(x)                      ((x) << DTPR_T_RAS_SHIFT)
#define     DTPR_T_RCD(x)                      ((x) << DTPR_T_RCD_SHIFT)
#define     DTPR_T_RP(x)                       ((x) << DTPR_T_RP_SHIFT)
#define     DTPR_T_WTR(x)                      ((x) << DTPR_T_WTR_SHIFT)
#define     DTPR_T_RTP(x)                      ((x) << DTPR_T_RTP_SHIFT)
#define DTPR1                               (0x04C)
#define     DTPR_T_AONFD_SHIFT                 (30)
#define     DTPR_T_WLO_SHIFT                   (26)
#define     DTPR_T_WLMRD_SHIFT                 (20)
#define     DTPR_T_RFC_SHIFT                   (11)
#define     DTPR_T_FAW_SHIFT                   ( 5)
#define     DTPR_T_MOD_SHIFT                   ( 2)
#define     DTPR_T_MRD_SHIFT                   ( 0)
#define     DTPR_T_AONFD(x)                    ((x) << DTPR_T_AONFD_SHIFT)
#define     DTPR_T_WLO(x)                      ((x) << DTPR_T_WLO_SHIFT)
#define     DTPR_T_WLMRD(x)                    ((x) << DTPR_T_WLMRD_SHIFT)
#define     DTPR_T_RFC(x)                      ((x) << DTPR_T_RFC_SHIFT)
#define     DTPR_T_FAW(x)                      ((x) << DTPR_T_FAW_SHIFT)
#define     DTPR_T_MOD(x)                      ((x) << DTPR_T_MOD_SHIFT)
#define     DTPR_T_MRD(x)                      ((x) << DTPR_T_MRD_SHIFT)
#define DTPR2                               (0x050)
#define     DTPR_T_CCD_SHIFT                   (31)
#define     DTPR_T_RTW_SHIFT                   (30)
#define     DTPR_T_RTODT_SHIFT                 (29)
#define     DTPR_T_DLLK_SHIFT                  (19)
#define     DTPR_T_CKE_SHIFT                   (15)
#define     DTPR_T_XP_SHIFT                    (10)
#define     DTPR_T_XS_SHIFT                    ( 0)
#define     DTPR_T_CCD(x)                      ((x) << DTPR_T_CCD_SHIFT)
#define     DTPR_T_RTW(x)                      ((x) << DTPR_T_RTW_SHIFT)
#define     DTPR_T_RTODT(x)                    ((x) << DTPR_T_RTODT_SHIFT)
#define     DTPR_T_DLLK(x)                     ((x) << DTPR_T_DLLK_SHIFT)
#define     DTPR_T_CKE(x)                      ((x) << DTPR_T_CKE_SHIFT)
#define     DTPR_T_XP(x)                       ((x) << DTPR_T_XP_SHIFT)
#define     DTPR_T_XS(x)                       ((x) << DTPR_T_XS_SHIFT)
#define MR0                                 (0x054)
#define MR1                                 (0x058)
#define MR2                                 (0x05C)
#define MR3                                 (0x060)
#define ODTCR                               (0x064)
#define     WRODT3_SHIFT                       (28)
#define     WRODT2_SHIFT                       (24)
#define     WRODT1_SHIFT                       (20)
#define     WRODT0_SHIFT                       (16)
#define     RDODT3_SHIFT                       (12)
#define     RDODT2_SHIFT                       ( 8)
#define     RDODT1_SHIFT                       ( 4)
#define     RDODT0_SHIFT                       ( 0)
#define     WRODT3(x)                          ((x) << WRODT3_SHIFT)
#define     WRODT2(x)                          ((x) << WRODT2_SHIFT)
#define     WRODT1(x)                          ((x) << WRODT1_SHIFT)
#define     WRODT0(x)                          ((x) << WRODT0_SHIFT)
#define     RDODT3(x)                          ((x) << RDODT3_SHIFT)
#define     RDODT2(x)                          ((x) << RDODT2_SHIFT)
#define     RDODT1(x)                          ((x) << RDODT1_SHIFT)
#define     RDODT0(x)                          ((x) << RDODT0_SHIFT)
#define DTCR                                (0x068)
#define	    RANKEN_SHIFT		       (24)
#define     DTMPR_SHIFT                         (6)
#define     DTRANK_SHIFT                        (4)
#define     RANKEN(x)                           ((x) << RANKEN_SHIFT)
#define     DTMPR(x)                            ((x) << DTMPR_SHIFT)
#define     DTRANK(x)                           ((x) << DTRANK_SHIFT)
#define DTAR0                               (0x06C)
#define DTAR1                               (0x070)
#define DTAR2                               (0x074)
#define DTAR3                               (0x078)
#define DTDR0                               (0x07C)
#define DTDR1                               (0x080)
#define DTEDR0                              (0x084)
#define DTEDR1                              (0x088)

#define BISTRR                              (0x100)
#define     BCCSEL_SHIFT                    ( 25)
#define     BCKSEL_SHIFT                    ( 23)
#define     BDXSEL_SHIFT                    ( 19)
#define     BDPAT_SHIFT                     ( 17)
#define     BDMEN_SHIFT                     ( 16)
#define     BACEN_SHIFT                     ( 15)
#define     BDXEN_SHIFT                     ( 14)
#define     BSONF_SHIFT                     ( 13)
#define     NFAIL_SHIFT                     ( 5)
#define     BINF_SHIFT                      ( 4)
#define     BMODE_SHIFT                     ( 3)
#define     BINST_SHIFT                     ( 0)
#define     BCCSEL(x)                        ((x) << BCCSEL_SHIFT)
#define     BCKSEL(x)                        ((x) << BCKSEL_SHIFT)
#define     BDXSEL(x)                        ((x) << BDXSEL_SHIFT)
#define     BDPAT(x)                        ((x) << BDPAT_SHIFT)
#define     BDMEN(x)                        ((x) << BDMEN_SHIFT)
#define     BACEN(x)                        ((x) << BACEN_SHIFT)
#define     BDXEN(x)                        ((x) << BDXEN_SHIFT)
#define     BSONF(x)                        ((x) << BSONF_SHIFT)
#define     NFAIL(x)                        ((x) << NFAIL_SHIFT)
#define     BINST(x)                        ((x) << BINST_SHIFT)
#define     BINF(x)                        ((x) << BINF_SHIFT)
#define     BMODE(x)                        ((x) << BMODE_SHIFT)
#define     BINST(x)                        ((x) << BINST_SHIFT)

#define BISTWCR                             (0x104)
#define BISTMSKR0                           (0x108)
#define BISTMSKR1                           (0x10C)
#define BISTMSKR2                           (0x110)
#define BISTLSR                             (0x114)

#define BISTAR0                             (0x118)
#define     BBANK_SHIFT                      (28)
#define     BROW_SHIFT                      (12)
#define     BCOL_SHIFT                      ( 0)
#define     BBANK(x)                         ((x) << BBANK_SHIFT)
#define     BROW(x)                         ((x) << BROW_SHIFT)
#define     BCOL(x)                         ((x) << BCOL_SHIFT)
#define BISTAR1                             (0x11c)
#define     BAINC_SHIFT                      ( 4)
#define     BMRANK_SHIFT                      (2)
#define     BRANK_SHIFT                      (0)
#define     BAINC(x)                         ((x) << BAINC_SHIFT)
#define     BMRANK(x)                         ((x) << BMRANK_SHIFT)
#define     BRANK(x)                         ((x) << BRANK_SHIFT)
#define BISTAR2                             (0x120)
#define     BMBANK_SHIFT                      (28)
#define     BMROW_SHIFT                      (12)
#define     BMCOL_SHIFT                      (0)
#define     BMBANK(x)                         ((x) << BMBANK_SHIFT)
#define     BMROW(x)                         ((x) << BMROW_SHIFT)
#define     BMCOL(x)                         ((x) << BMCOL_SHIFT)

#define BISTUDPR                            (0x124)
#define BISTGSR                             (0x128)
#define     BDONE_SHIFT                      (0)
#define     BDONE(x)                         ((x) << BDONE_SHIFT)

#define BISTWER                             (0x12C)
#define BISTBER0                            (0x130)
#define BISTBER1                            (0x134)
#define BISTBER3                            (0x13C)
#define BISTWCSR                            (0x140)
#define BISTFWR0                            (0x144)
#define BISTFWR1                            (0x148)
#define BISTFWR2                            (0x14C)

#define ZQ0CR0                              (0x180)
#define ZQ0CR1                              (0x184)
#define     ODT_DIV_SHIFT                      ( 4)
#define     IMPEDANCE_DIV_SHIFT                ( 0)
#define     ODT_DIV(x)                         ((x) << ODT_DIV_SHIFT)
#define     IMPEDANCE_DIV(x)                   ((x) << IMPEDANCE_DIV_SHIFT)
#define ZQ0SR0                              (0x188)
#define ZQ0SR1                              (0x18C)
#define DX0GCR                              (0x1C0)
#define     CALBYP_SHIFT                       (31)
#define     MDLEN_DXGCR_SHIFT                  (30)
#define     WLRKEN_SHIFT                       (26)
#define     GSHIFT_SHIFT                       (18)
#define     PLLPD_SHIFT                        (17)
#define     PLLRST_SHIFT                       (16)
#define     RTTOAL_SHIFT                       (13)
#define     RTTOH_SHIFT                        (11)
#define     DQRTT_SHIFT                        (10)
#define     DQSRTT_SHIFT                       ( 9)
#define     DSEN_SHIFT                         ( 7)
#define     DQSRPD_SHIFT                       ( 6)
#define     DXPDR_DXGCR_SHIFT                  ( 5)
#define     DXPDD_DXGCR_SHIFT                  ( 4)
#define     DXIOM_DXGCR_SHIFT                  ( 3)
#define     DQODT_SHIFT                        ( 2)
#define     DQSODT_SHIFT                       ( 1)
#define     DXEN_SHIFT                         ( 0)
#define     CALBYP(x)                          ((x) << CALBYP_SHIFT)
#define     MDLEN_DXGCR(x)                     ((x) << MDLEN_DXGCR_SHIFT)
#define     WLRKEN(x)                          ((x) << WLRKEN_SHIFT)
#define     GSHIFT(x)                          ((x) << GSHIFT_SHIFT)
#define     PLLPD(x)                           ((x) << PLLPD_SHIFT)
#define     PLLRST(x)                          ((x) << PLLRST_SHIFT)
#define     RTTOAL(x)                          ((x) << RTTOAL_SHIFT)
#define     RTTOH(x)                           ((x) << RTTOH_SHIFT)
#define     DQRTT(x)                           ((x) << DQRTT_SHIFT)
#define     DQSRTT(x)                          ((x) << DQSRTT_SHIFT)
#define     DSEN(x)                            ((x) << DSEN_SHIFT)
#define     DQSRPD(x)                          ((x) << DQSRPD_SHIFT)
#define     DXPDR_DXGCR(x)                     ((x) << DXPDR_DXGCR_SHIFT)
#define     DXPDD_DXGCR(x)                     ((x) << DXPDD_DXGCR_SHIFT)
#define     DXIOM_DXGCR(x)                     ((x) << DXIOM_DXGCR_SHIFT)
#define     DQODT(x)                           ((x) << DQODT_SHIFT)
#define     DQSODT(x)                          ((x) << DQSODT_SHIFT)
#define     DXEN(x)                            ((x) << DXEN_SHIFT)
#define DX0GSR0                             (0x1C4)
#define DX0GSR1                             (0x1C8)
#define DX0BDLR0                            (0x1cc)
#define     DQ0WBD_SHIFT                       ( 0)
#define     DQ1WBD_SHIFT                       ( 6)
#define     DQ2WBD_SHIFT                       (12)
#define     DQ3WBD_SHIFT                       (18)
#define     DQ4WBD_SHIFT                       (24)
#define     DQ0WBD(x)                          ((x) << DQ0WBD_SHIFT)
#define     DQ1WBD(x)                          ((x) << DQ1WBD_SHIFT)
#define     DQ2WBD(x)                          ((x) << DQ2WBD_SHIFT)
#define     DQ3WBD(x)                          ((x) << DQ3WBD_SHIFT)
#define     DQ4WBD(x)                          ((x) << DQ4WBD_SHIFT)
#define DX0BDLR1                            (0x1d0)
#define     DQ5WBD_SHIFT                       ( 0)
#define     DQ6WBD_SHIFT                       ( 6)
#define     DQ7WBD_SHIFT                       (12)
#define     DMWBD_SHIFT                       (18)
#define     DSWBD_SHIFT                       (24)
#define     DQ5WBD(x)                          ((x) << DQ5WBD_SHIFT)
#define     DQ6WBD(x)                          ((x) << DQ6WBD_SHIFT)
#define     DQ7WBD(x)                          ((x) << DQ7WBD_SHIFT)
#define     DMWBD(x)                          ((x) << DMWBD_SHIFT)
#define     DSWBD(x)                          ((x) << DSWBD_SHIFT)
#define DX0BDLR2                            (0x1d4)
#define     DSOEBD_SHIFT                      ( 0)
#define     DQOEBD_SHIFT                      ( 6)
#define     DSRBD_SHIFT        	              (12)
#define     DSNRBD_SHIFT                      (18)
#define     DSOEBD(x)                         ((x) << DSOEBD_SHIFT)
#define     DQOEBD(x)                         ((x) << DQOEBD_SHIFT)
#define     DSRBD(x)                          ((x) << DSRBD_SHIFT)
#define     DSNRBD(x)                         ((x) << DSNRBD_SHIFT)
#define DX0BDLR3                            (0x1d8)
#define     DQ0RBD_SHIFT                       ( 0)
#define     DQ1RBD_SHIFT                       ( 6)
#define     DQ2RBD_SHIFT                       (12)
#define     DQ3RBD_SHIFT                       (18)
#define     DQ4RBD_SHIFT                       (24)
#define     DQ0RBD(x)                          ((x) << DQ0RBD_SHIFT)
#define     DQ1RBD(x)                          ((x) << DQ1RBD_SHIFT)
#define     DQ2RBD(x)                          ((x) << DQ2RBD_SHIFT)
#define     DQ3RBD(x)                          ((x) << DQ3RBD_SHIFT)
#define     DQ4RBD(x)                          ((x) << DQ4RBD_SHIFT)
#define DX0BDLR4                            (0x1dc)
#define     DQ5RBD_SHIFT                       ( 0)
#define     DQ6RBD_SHIFT                       ( 6)
#define     DQ7RBD_SHIFT                       (12)
#define     DMRBD_SHIFT                        (18)
#define     DQ5RBD(x)                          ((x) << DQ5RBD_SHIFT)
#define     DQ6RBD(x)                          ((x) << DQ6RBD_SHIFT)
#define     DQ7RBD(x)                          ((x) << DQ7RBD_SHIFT)
#define     DMRBD(x)                           ((x) << DMRBD_SHIFT)
#define DX0LCDLR0                           (0x1e0)
#define     R0WLD_SHIFT                       ( 0)
#define     R1WLD_SHIFT                       ( 8)
#define     R2WLD_SHIFT                       (16)
#define     R3WLD_SHIFT                       (24)
#define     R0WLD(x)                          ((x) << R0WLD_SHIFT)
#define     R1WLD(x)                          ((x) << R1WLD_SHIFT)
#define     R2WLD(x)                          ((x) << R2WLD_SHIFT)
#define     R3WLD(x)                          ((x) << R3WLD_SHIFT)
#define DX0LCDLR1                           (0x1e4)
#define     WDQD_SHIFT                        ( 0)
#define     RDQSD_SHIFT                       ( 8)
#define     RDQSND_SHIFT                      (16)
#define     WDQD(x)                           ((x) << WDQD_SHIFT)
#define     RDQSD(x)                          ((x) << RDQSD_SHIFT)
#define     RDQSND(x)                         ((x) << RDQSND_SHIFT)
#define DX0LCDLR2                           (0x1e8)
#define     R0DQSGD_SHIFT                       ( 0)
#define     R1DQSGD_SHIFT                       ( 8)
#define     R2DQSGD_SHIFT                       (16)
#define     R3DQSGD_SHIFT                       (24)
#define     R0DQSGD(x)                          ((x) << R0DQSGD_SHIFT)
#define     R1DQSGD(x)                          ((x) << R1DQSGD_SHIFT)
#define     R2DQSGD(x)                          ((x) << R2DQSGD_SHIFT)
#define     R3DQSGD(x)                          ((x) << R3DQSGD_SHIFT)
#define DX0MDLR                             (0x1EC)
#define DX0GTR                              (0x1F0)
#define     R3WLSL_SHIFT                       (18)
#define     R2WLSL_SHIFT                       (16)
#define     R1WLSL_SHIFT                       (14)
#define     R0WLSL_SHIFT                       (12)
#define     R3DGSL_SHIFT                       ( 9)
#define     R2DGSL_SHIFT                       ( 6)
#define     R1DGSL_SHIFT                       ( 3)
#define     R0DGSL_SHIFT                       ( 0)
#define     R3WLSL(x)                          ((x) << R3WLSL_SHIFT)
#define     R2WLSL(x)                          ((x) << R2WLSL_SHIFT)
#define     R1WLSL(x)                          ((x) << R1WLSL_SHIFT)
#define     R0WLSL(x)                          ((x) << R0WLSL_SHIFT)
#define     R3DGSL(x)                          ((x) << R3DGSL_SHIFT)
#define     R2DGSL(x)                          ((x) << R2DGSL_SHIFT)
#define     R1DGSL(x)                          ((x) << R1DGSL_SHIFT)
#define     R0DGSL(x)                          ((x) << R0DGSL_SHIFT)
#define DX1GCR                              (0x200)
#define DX1GSR0                             (0x204)
#define DX1GSR1                             (0x208)
#define DX1BDLR0                            (0x20C)
#define DX1BDLR1                            (0x210)
#define DX1BDLR2                            (0x214)
#define DX1BDLR3                            (0x218)
#define DX1BDLR4                            (0x21C)
#define DX1LCDLR0                           (0x220)
#define DX1LCDLR1                           (0x224)
#define DX1LCDLR2                           (0x228)
#define DX1MDLR                             (0x22C)
#define DX1GTR                              (0x230)
#define DX2GCR                              (0x240)
#define DX2GSR0                             (0x244)
#define DX2GSR1                             (0x248)
#define DX2BDLR0                            (0x24C)
#define DX2BDLR1                            (0x250)
#define DX2BDLR2                            (0x254)
#define DX2BDLR3                            (0x258)
#define DX2BDLR4                            (0x25C)
#define DX2LCDLR0                           (0x260)
#define DX2LCDLR1                           (0x264)
#define DX2LCDLR2                           (0x268)
#define DX2MDLR                             (0x26C)
#define DX2GTR                              (0x270)
#define DX3GCR                              (0x280)
#define DX3GSR0                             (0x284)
#define DX3GSR1                             (0x288)
#define DX3BDLR0                            (0x28C)
#define DX3BDLR1                            (0x290)
#define DX3BDLR2                            (0x294)
#define DX3BDLR3                            (0x298)
#define DX3BDLR4                            (0x29C)
#define DX3LCDLR0                           (0x2a0)
#define DX3LCDLR1                           (0x2a4)
#define DX3LCDLR2                           (0x2a8)
#define DX3MDLR                             (0x2AC)
#define DX3GTR                              (0x2B0)
#define DX4GCR                              (0x2c0)
#define DX4GSR0                             (0x2c4)
#define DX4GSR1                             (0x2c8)
#define DX4BDLR0                            (0x2cC)
#define DX4BDLR1                            (0x2d0)
#define DX4BDLR2                            (0x2d4)
#define DX4BDLR3                            (0x2d8)
#define DX4BDLR4                            (0x2dC)
#define DX4LCDLR0                           (0x2E0)
#define DX4LCDLR1                           (0x2E4)
#define DX4LCDLR2                           (0x2E8)
#define DX4MDLR                             (0x2EC)
#define DX4GTR                              (0x2F0)
#define DX5GCR                              (0x300)
#define DX5GSR0                             (0x304)
#define DX5GSR1                             (0x308)
#define DX5BDLR0                            (0x30C)
#define DX5BDLR1                            (0x310)
#define DX5BDLR2                            (0x314)
#define DX5BDLR3                            (0x318)
#define DX5BDLR4                            (0x31C)
#define DX5LCDLR0                           (0x320)
#define DX5LCDLR1                           (0x324)
#define DX5LCDLR2                           (0x328)
#define DX5MDLR                             (0x32C)
#define DX5GTR                              (0x330)
#define DX6GCR                              (0x340)
#define DX6GSR0                             (0x344)
#define DX6GSR1                             (0x348)
#define DX6BDLR0                            (0x34C)
#define DX6BDLR1                            (0x350)
#define DX6BDLR2                            (0x354)
#define DX6BDLR3                            (0x358)
#define DX6BDLR4                            (0x35C)
#define DX6LCDLR0                           (0x360)
#define DX6LCDLR1                           (0x364)
#define DX6LCDLR2                           (0x368)
#define DX6MDLR                             (0x36C)
#define DX6GTR                              (0x370)
#define DX7GCR                              (0x380)
#define DX7GSR0                             (0x384)
#define DX7GSR1                             (0x388)
#define DX7BDLR0                            (0x38C)
#define DX7BDLR1                            (0x390)
#define DX7BDLR2                            (0x394)
#define DX7BDLR3                            (0x398)
#define DX7BDLR4                            (0x39C)
#define DX7LCDLR0                           (0x3a0)
#define DX7LCDLR1                           (0x3a4)
#define DX7LCDLR2                           (0x3a8)
#define DX7MDLR                             (0x3AC)
#define DX7GTR                              (0x3B0)
#define DX8GCR                              (0x3c0)
#define DX8GSR0                             (0x3c4)
#define DX8GSR1                             (0x3c8)
#define DX8BDLR0                            (0x3cC)
#define DX8BDLR1                            (0x3d0)
#define DX8BDLR2                            (0x3d4)
#define DX8BDLR3                            (0x3d8)
#define DX8BDLR4                            (0x3dC)
#define DX8LCDLR0                           (0x3E0)
#define DX8LCDLR1                           (0x3E4)
#define DX8LCDLR2                           (0x3E8)
#define DX8MDLR                             (0x3EC)
#define DX8GTR                              (0x3F0)

#define PUB_PIR_DATA                    (INITBYP(CFG_INITBYP) \
                                       | ZCALBYP(CFG_ZCALBYP) \
                                       | DCALBYP(CFG_DCALBYP) \
                                       | LOCKBYP(CFG_LOCKBYP) \
                                       | CLRSR(CFG_CLRSR) \
                                       | CTLDINIT(CFG_CTLDINIT) \
                                       | PLLBYP(CFG_PLLBYP) \
                                       | ICPC(CFG_ICPC) \
                                       | WREYE(CFG_WREYE) \
                                       | RDEYE(CFG_RDEYE) \
                                       | WRDSKW(CFG_WRDSKW) \
                                       | RDDSKW(CFG_RDDSKW) \
                                       | WLADJ(CFG_WLADJ) \
                                       | QSGATE(CFG_QSGATE) \
                                       | WL(CFG_WL) \
                                       | DRAMINIT(CFG_DRAMINIT) \
                                       | DRAMRST(CFG_DRAMRST) \
                                       | PHYRST(CFG_PHYRST) \
                                       | DCAL(CFG_DCAL) \
                                       | PLLINIT(CFG_PLLINIT) \
                                       | ZCAL(CFG_ZCAL) \
                                       | INIT(CFG_INIT))

#define PUB_PGCR0_DATA                  (CKEN(CFG_CKEN) \
                                       | DTOSEL(CFG_DTOSEL) \
                                       | OSCWDL(CFG_OSCWDL) \
                                       | OSCDIV(CFG_OSCDIV) \
                                       | OSCEN(CFG_OSCEN) \
                                       | DLTST(CFG_DLTST) \
                                       | DLTMODE(CFG_DLTMODE) \
                                       | RDBVT(CFG_RDBVT) \
                                       | WDBVT(CFG_WDBVT) \
                                       | RGLVT(CFG_RGLVT) \
                                       | RDLVT(CFG_RDLVT) \
                                       | WDLVT(CFG_WDLVT) \
                                       | WLLVT(CFG_WLLVT))

#define PUB_PGCR1_DATA                  (LBMODE(CFG_LBMODE) \
                                       | LBGDQS(CFG_LBGDQS) \
                                       | LBDQSS(CFG_LBDQSS) \
                                       | IOLB(CFG_IOLB) \
                                       | INHVT(CFG_INHVT) \
                                       | PHYHRST(CFG_PHYHRST) \
                                       | ZCKSEL(CFG_ZCKSEL) \
                                       | DLDLMT(CFG_DLDLMT) \
                                       | FDEPTH(CFG_FDEPTH) \
                                       | LPFDEPTH(CFG_LPFDEPTH) \
                                       | LPFEN(CFG_LPFEN) \
                                       | MDLEN_PGCR1(CFG_MDLEN_PGCR1) \
                                       | IODDRM(CFG_IODDRM) \
                                       | WLSELT(CFG_WLSELT) \
                                       | WLRANK(CFG_WLRANK) \
                                       | WLUNCRT(CFG_WLUNCRT) \
                                       | WLSTEP(CFG_WLSTEP) \
                                       | WLFULL(CFG_WLFULL) \
                                       | PDDISDX(CFG_PDDISDX))

#define PUB_PTR0_DATA                   (T_PLLPD(CFG_T_PLLPD) \
                                       | T_PLLGS(CFG_T_PLLGS) \
                                       | T_PHYRST(CFG_T_PHYRST))

#define PUB_PTR1_DATA                   (T_PLLLOCK(CFG_T_PLLLOCK) \
                                       | T_PLLRST(CFG_T_PLLRST))

#define PUB_PTR2_DATA                   (T_WLDLYS(CFG_T_WLDLYS) \
                                       | T_CALH(CFG_T_CALH) \
                                       | T_CALS(CFG_T_CALS) \
                                       | T_CALON(CFG_T_CALON))

#define PUB_PTR3_DATA                   (T_DINIT1(CFG_T_DINIT1) \
                                       | T_DINIT0(CFG_T_DINIT0))

#define PUB_PTR4_DATA                   (T_DINIT3(CFG_T_DINIT3) \
                                       | T_DINIT2(CFG_T_DINIT2))

#define PUB_DXCCR_DATA                  (DXSR(CFG_DXSR) \
                                       | DQSNRES(CFG_DQSNRES) \
                                       | DQSRES(CFG_DQSRES) \
                                       | DXPDR_DXCCR(CFG_DXPDR_DXCCR) \
                                       | DXPDD_DXCCR(CFG_DXPDD_DXCCR) \
                                       | MDLEN_DXCCR(CFG_MDLEN_DXCCR) \
                                       | DXIOM_DXCCR(CFG_DXIOM_DXCCR) \
                                       | DXODT(CFG_DXODT))

#define PUB_DSGCR_DATA                  (CKEOE(CFG_CKEOE) \
                                       | RSTOE(CFG_RSTOE) \
                                       | ODTOE(CFG_ODTOE) \
                                       | CKOE(CFG_CKOE) \
                                       | ODTPDD(CFG_ODTPDD) \
                                       | CKEPDD(CFG_CKEPDD) \
                                       | SDRMODE(CFG_SDRMODE) \
                                       | RRMODE(CFG_RRMODE) \
                                       | DQSGX(CFG_DQSGX) \
                                       | CUAEN(CFG_CUAEN) \
                                       | LPPLLPD(CFG_LPPLLPD) \
                                       | LPIOPD(CFG_LPIOPD) \
                                       | ZUEN(CFG_ZUEN) \
                                       | BDISEN(CFG_BDISEN) \
                                       | PUREN(CFG_PUREN))

#define PUB_DCR_DATA                    (DCR_RDIMM(CFG_DCR_RDIMM) \
                                       | DCR_UDIMM(CFG_DCR_UDIMM) \
                                       | DCR_DDR2T(CFG_DCR_DDR2T) \
                                       | DCR_NOSRA(CFG_DCR_NOSRA) \
                                       | DCR_BYTEMASK(CFG_DCR_BYTEMASK) \
                                       | DCR_MPRDQ(CFG_DCR_MPRDQ) \
                                       | DCR_PDQ(CFG_DCR_PDQ) \
                                       | DCR_DDR8BNK(CFG_DCR_DDR8BNK)) \
                                       | DCR_DDRMD(CFG_DCR_DDRMD)  

#define PUB_DTPR0_DATA                  (DTPR_T_RC(CFG_DTPR_T_RC) \
                                       | DTPR_T_RRD(CFG_DTPR_T_RRD) \
                                       | DTPR_T_RAS(CFG_DTPR_T_RAS) \
                                       | DTPR_T_RCD(CFG_DTPR_T_RCD) \
                                       | DTPR_T_RP(CFG_DTPR_T_RP) \
                                       | DTPR_T_WTR(CFG_DTPR_T_WTR) \
                                       | DTPR_T_RTP(CFG_DTPR_T_RTP))

#define PUB_DTPR1_DATA                  (DTPR_T_AONFD(CFG_DTPR_T_AONFD) \
                                       | DTPR_T_WLO(CFG_DTPR_T_WLO) \
                                       | DTPR_T_WLMRD(CFG_DTPR_T_WLMRD) \
                                       | DTPR_T_RFC(CFG_DTPR_T_RFC) \
                                       | DTPR_T_FAW(CFG_DTPR_T_FAW) \
                                       | DTPR_T_MOD(CFG_DTPR_T_MOD) \
                                       | DTPR_T_MRD(CFG_DTPR_T_MRD))

#define PUB_DTPR2_DATA                  (DTPR_T_CCD(CFG_DTPR_T_CCD) \
                                       | DTPR_T_RTW(CFG_DTPR_T_RTW) \
                                       | DTPR_T_RTODT(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_DLLK(CFG_DTPR_T_DLLK) \
                                       | DTPR_T_CKE(CFG_DTPR_T_CKE) \
                                       | DTPR_T_XP(CFG_DTPR_T_XP) \
                                       | DTPR_T_XS(CFG_DTPR_T_XS))


#ifdef DYNAMIC_LOAD_TIMING	

#define M390A_PUB_DTPR0_DATA                  (DTPR_T_RC(CFG_390A_T_RC) \
                                       | DTPR_T_RRD(CFG_390A_T_RRD) \
                                       | DTPR_T_RAS(CFG_390A_T_RAS) \
                                       | DTPR_T_RCD(CFG_390A_T_RCD) \
                                       | DTPR_T_RP(CFG_390A_T_RP) \
                                       | DTPR_T_WTR(CFG_390A_T_WTR) \
                                       | DTPR_T_RTP(CFG_390A_T_RTP))

#define M390A_PUB_DTPR1_DATA                  (DTPR_T_AONFD(CFG_DTPR_T_AONFD) \
                                       | DTPR_T_WLO(CFG_DTPR_T_WLO) \
                                       | DTPR_T_WLMRD(CFG_DTPR_T_WLMRD) \
                                       | DTPR_T_RFC(CFG_390A_T_RFC) \
                                       | DTPR_T_FAW(CFG_390A_T_FAW) \
                                       | DTPR_T_MOD(CFG_390A_T_MOD) \
                                       | DTPR_T_MRD(CFG_390A_T_MRD))

#define M390A_PUB_DTPR2_DATA                  (DTPR_T_CCD(CFG_DTPR_T_CCD) \
                                       | DTPR_T_RTW(CFG_DTPR_T_RTW) \
                                       | DTPR_T_RTODT(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_DLLK(CFG_DTPR_T_DLLK) \
                                       | DTPR_T_CKE(CFG_DTPR_T_CKE) \
                                       | DTPR_T_XP(CFG_DTPR_T_XP) \
                                       | DTPR_T_XS(CFG_DTPR_T_XS))


#define M390_PUB_DTPR0_DATA                  (DTPR_T_RC(CFG_390_T_RC) \
                                       | DTPR_T_RRD(CFG_390_T_RRD) \
                                       | DTPR_T_RAS(CFG_390_T_RAS) \
                                       | DTPR_T_RCD(CFG_390_T_RCD) \
                                       | DTPR_T_RP(CFG_390_T_RP) \
                                       | DTPR_T_WTR(CFG_390_T_WTR) \
                                       | DTPR_T_RTP(CFG_390_T_RTP))

#define M390_PUB_DTPR1_DATA                  (DTPR_T_AONFD(CFG_DTPR_T_AONFD) \
                                       | DTPR_T_WLO(CFG_DTPR_T_WLO) \
                                       | DTPR_T_WLMRD(CFG_DTPR_T_WLMRD) \
                                       | DTPR_T_RFC(CFG_390_T_RFC) \
                                       | DTPR_T_FAW(CFG_390_T_FAW) \
                                       | DTPR_T_MOD(CFG_DTPR_T_MOD) \
                                       | DTPR_T_MRD(CFG_DTPR_T_MRD))

#define M390_PUB_DTPR2_DATA                  (DTPR_T_CCD(CFG_DTPR_T_CCD) \
                                       | DTPR_T_RTW(CFG_DTPR_T_RTW) \
                                       | DTPR_T_RTODT(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_DLLK(CFG_DTPR_T_DLLK) \
                                       | DTPR_T_CKE(CFG_DTPR_T_CKE) \
                                       | DTPR_T_XP(CFG_DTPR_T_XP) \
                                       | DTPR_T_XS(CFG_DTPR_T_XS))

#define M370_PUB_DTPR0_DATA                  (DTPR_T_RC(CFG_370_T_RC) \
                                       | DTPR_T_RRD(CFG_370_T_RRD) \
                                       | DTPR_T_RAS(CFG_370_T_RAS) \
                                       | DTPR_T_RCD(CFG_370_T_RCD) \
                                       | DTPR_T_RP(CFG_370_T_RP) \
                                       | DTPR_T_WTR(CFG_370_T_WTR) \
                                       | DTPR_T_RTP(CFG_370_T_RTP))

#define M370_PUB_DTPR1_DATA                  (DTPR_T_AONFD(CFG_DTPR_T_AONFD) \
                                       | DTPR_T_WLO(CFG_DTPR_T_WLO) \
                                       | DTPR_T_WLMRD(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_RFC(CFG_370_T_RFC) \
                                       | DTPR_T_FAW(CFG_370_T_FAW) \
                                       | DTPR_T_MOD(CFG_370_T_MOD) \
                                       | DTPR_T_MRD(CFG_370_T_MRD))

#define M370_PUB_DTPR2_DATA                  (DTPR_T_CCD(CFG_DTPR_T_CCD) \
                                       | DTPR_T_RTW(CFG_DTPR_T_RTW) \
                                       | DTPR_T_RTODT(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_DLLK(CFG_DTPR_T_DLLK) \
                                       | DTPR_T_CKE(CFG_DTPR_T_CKE) \
                                       | DTPR_T_XP(CFG_DTPR_T_XP) \
                                       | DTPR_T_XS(CFG_DTPR_T_XS))

#define M330_PUB_DTPR0_DATA                  (DTPR_T_RC(CFG_330_T_RC) \
                                       | DTPR_T_RRD(CFG_330_T_RRD) \
                                       | DTPR_T_RAS(CFG_330_T_RAS) \
                                       | DTPR_T_RCD(CFG_330_T_RCD) \
                                       | DTPR_T_RP(CFG_330_T_RP) \
                                       | DTPR_T_WTR(CFG_330_T_WTR) \
                                       | DTPR_T_RTP(CFG_330_T_RTP))

#define M330_PUB_DTPR1_DATA                  (DTPR_T_AONFD(CFG_DTPR_T_AONFD) \
                                       | DTPR_T_WLO(CFG_DTPR_T_WLO) \
                                       | DTPR_T_WLMRD(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_RFC(CFG_330_T_RFC) \
                                       | DTPR_T_FAW(CFG_330_T_FAW) \
                                       | DTPR_T_MOD(CFG_330_T_MOD) \
                                       | DTPR_T_MRD(CFG_330_T_MRD))

#define M330_PUB_DTPR2_DATA                  (DTPR_T_CCD(CFG_DTPR_T_CCD) \
                                       | DTPR_T_RTW(CFG_DTPR_T_RTW) \
                                       | DTPR_T_RTODT(CFG_DTPR_T_RTODT) \
                                       | DTPR_T_DLLK(CFG_DTPR_T_DLLK) \
                                       | DTPR_T_CKE(CFG_DTPR_T_CKE) \
                                       | DTPR_T_XP(CFG_DTPR_T_XP) \
                                       | DTPR_T_XS(CFG_DTPR_T_XS))
#endif

#define PUB_MR0_DATA                    (DDR0_LOAD_MODE_0_1_DATA & 0xFFFF)
#define PUB_MR1_DATA                    (DDR0_LOAD_MODE_0_1_DATA >> 16)
#define PUB_MR2_DATA                    (DDR0_LOAD_MODE_2_3_DATA & 0xFFFF)
#define PUB_MR3_DATA                    (DDR0_LOAD_MODE_2_3_DATA >> 16)

#define PUB_ODTCR_DATA                  (WRODT3(CFG_WRODT3) \
                                       | WRODT2(CFG_WRODT2) \
                                       | WRODT1(CFG_WRODT1) \
                                       | WRODT0(CFG_WRODT0) \
                                       | RDODT3(CFG_RDODT3) \
                                       | RDODT2(CFG_RDODT2) \
                                       | RDODT1(CFG_RDODT1) \
                                       | RDODT0(CFG_RDODT0))

#define PUB_ZQ0CR1_DATA                 (ODT_DIV(CFG_ODT_DIV) \
                                       | IMPEDANCE_DIV(CFG_IMPEDANCE_DIV))

#define PUB_DX0GCR_DATA                 (CALBYP(CFG_DX0_CALBYP) \
                                       | MDLEN_DXGCR(CFG_DX0_MDLEN) \
                                       | WLRKEN(CFG_DX0_WLRKEN) \
                                       | GSHIFT(CFG_DX0_GSHIFT) \
                                       | PLLPD(CFG_DX0_PLLPD) \
                                       | PLLRST(CFG_DX0_PLLRST) \
                                       | RTTOAL(CFG_DX0_RTTOAL) \
                                       | RTTOH(CFG_DX0_RTTOH) \
                                       | DQRTT(CFG_DX0_DQRTT) \
                                       | DQSRTT(CFG_DX0_DQSRTT) \
                                       | DSEN(CFG_DX0_DSEN) \
                                       | DQSRPD(CFG_DX0_DQSRPD) \
                                       | DXPDR_DXGCR(CFG_DX0_DXPDR) \
                                       | DXPDD_DXGCR(CFG_DX0_DXPDD) \
                                       | DXIOM_DXGCR(CFG_DX0_DXIOM) \
                                       | DQODT(CFG_DX0_DQODT) \
                                       | DQSODT(CFG_DX0_DQSODT) \
                                       | DXEN(CFG_DX0_DXEN))

#define PUB_DX1GCR_DATA                 (CALBYP(CFG_DX1_CALBYP) \
                                       | MDLEN_DXGCR(CFG_DX1_MDLEN) \
                                       | WLRKEN(CFG_DX1_WLRKEN) \
                                       | GSHIFT(CFG_DX1_GSHIFT) \
                                       | PLLPD(CFG_DX1_PLLPD) \
                                       | PLLRST(CFG_DX1_PLLRST) \
                                       | RTTOAL(CFG_DX1_RTTOAL) \
                                       | RTTOH(CFG_DX1_RTTOH) \
                                       | DQRTT(CFG_DX1_DQRTT) \
                                       | DQSRTT(CFG_DX1_DQSRTT) \
                                       | DSEN(CFG_DX1_DSEN) \
                                       | DQSRPD(CFG_DX1_DQSRPD) \
                                       | DXPDR_DXGCR(CFG_DX1_DXPDR) \
                                       | DXPDD_DXGCR(CFG_DX1_DXPDD) \
                                       | DXIOM_DXGCR(CFG_DX1_DXIOM) \
                                       | DQODT(CFG_DX1_DQODT) \
                                       | DQSODT(CFG_DX1_DQSODT) \
                                       | DXEN(CFG_DX1_DXEN))

#define PUB_DX2GCR_DATA                 (CALBYP(CFG_DX2_CALBYP) \
                                       | MDLEN_DXGCR(CFG_DX2_MDLEN) \
                                       | WLRKEN(CFG_DX2_WLRKEN) \
                                       | GSHIFT(CFG_DX2_GSHIFT) \
                                       | PLLPD(CFG_DX2_PLLPD) \
                                       | PLLRST(CFG_DX2_PLLRST) \
                                       | RTTOAL(CFG_DX2_RTTOAL) \
                                       | RTTOH(CFG_DX2_RTTOH) \
                                       | DQRTT(CFG_DX2_DQRTT) \
                                       | DQSRTT(CFG_DX2_DQSRTT) \
                                       | DSEN(CFG_DX2_DSEN) \
                                       | DQSRPD(CFG_DX2_DQSRPD) \
                                       | DXPDR_DXGCR(CFG_DX2_DXPDR) \
                                       | DXPDD_DXGCR(CFG_DX2_DXPDD) \
                                       | DXIOM_DXGCR(CFG_DX2_DXIOM) \
                                       | DQODT(CFG_DX2_DQODT) \
                                       | DQSODT(CFG_DX2_DQSODT) \
                                       | DXEN(CFG_DX2_DXEN))

#define PUB_DX3GCR_DATA                 (CALBYP(CFG_DX3_CALBYP) \
                                       | MDLEN_DXGCR(CFG_DX3_MDLEN) \
                                       | WLRKEN(CFG_DX3_WLRKEN) \
                                       | GSHIFT(CFG_DX3_GSHIFT) \
                                       | PLLPD(CFG_DX3_PLLPD) \
                                       | PLLRST(CFG_DX3_PLLRST) \
                                       | RTTOAL(CFG_DX3_RTTOAL) \
                                       | RTTOH(CFG_DX3_RTTOH) \
                                       | DQRTT(CFG_DX3_DQRTT) \
                                       | DQSRTT(CFG_DX3_DQSRTT) \
                                       | DSEN(CFG_DX3_DSEN) \
                                       | DQSRPD(CFG_DX3_DQSRPD) \
                                       | DXPDR_DXGCR(CFG_DX3_DXPDR) \
                                       | DXPDD_DXGCR(CFG_DX3_DXPDD) \
                                       | DXIOM_DXGCR(CFG_DX3_DXIOM) \
                                       | DQODT(CFG_DX3_DQODT) \
                                       | DQSODT(CFG_DX3_DQSODT) \
                                       | DXEN(CFG_DX3_DXEN))

/*=============================================================================
 * MSHC
 *============================================================================*/
#define SDIO_CTRL                       0x00
#define SDIO_PWREN                      0x04
#define SDIO_CLKDIV                     0x08
#define SDIO_CLKSRC                     0x0c
#define SDIO_CLKENA                     0x10
#define SDIO_TMOUT                      0x14
#define SDIO_CTYPE                      0x18
#define SDIO_BLKSIZ                     0x1c
#define SDIO_BYTCNT                     0x20
#define SDIO_INTMASK                    0x24
#define SDIO_CMDARG                     0x28
#define SDIO_CMD                        0x2c
#define SDIO_RESP0                      0x30
#define SDIO_RESP1                      0x34
#define SDIO_RESP2                      0x38
#define SDIO_RESP3                      0x3c
#define SDIO_MINTSTS                    0x40
#define SDIO_RINTSTS                    0x44
#define SDIO_STATUS                     0x48
#define SDIO_FIFOTH                     0x4c
#define SDIO_CDETECT                    0x50
#define SDIO_WRTPRT                     0x54
#define SDIO_GPIO                       0x58
#define SDIO_TCBCNT                     0x5c
#define SDIO_TBBCNT                     0x60
#define SDIO_DEBNCE                     0x64
#define SDIO_USRID                      0x68
#define SDIO_VERID                      0x6c
#define SDIO_HCON                       0x70
#define SDIO_BMOD                       0x80
#define SDIO_PLDMND                     0x84
#define SDIO_DBADDR                     0x88
#define SDIO_IDSTS                      0x8c
#define SDIO_IDINTEN                    0x90
#define SDIO_DSCADDR                    0x94
#define SDIO_BUFADDR                    0x98
#define SDIO_DATA                       0x100

#define CTRL_CONTROLLER_RESET           0x00000001
#define CTRL_FIFO_RESET                 0x00000002
#define CTRL_RESET_DATA                 (CTRL_CONTROLLER_RESET | CTRL_FIFO_RESET )

#define RINTSTS_CD                      0x4
#define RINTSTS_DTO                     0x8
#define RINTSTS_RXDR                    0x20
#define RINTSTS_RTO                     0x100

#define STATUS_FIFOCOUNT_MASK           0x3FFE0000
#define STATUS_FIFOCOUNT_SHIFT          17

#define SDIO_FIFOTH_DATA                0x00070008

#define CMD_START_CMD                   0x80000000


/*=============================================================================
 * DMAC
 *============================================================================*/
#define DMAC_CTRL                       (0x08)
#define     DATA_WIDTH_FLAG_SHIFT         (10)
#define     DST_BUS_NUM_SHIFT             ( 6)
#define     SRC_BUS_NUM_SHIFT             ( 4)
#define     OP_START_SHIFT                ( 2)
#define     OP_CMPT_ACK_EN_SHIFT          ( 1)
#define     OP_CMPT_ACK_SHIFT             ( 0)
#define     DATA_WIDTH_FLAG(x)            ((x) << DATA_WIDTH_FLAG_SHIFT)
#define     DST_BUS_NUM(x)                ((x) << DST_BUS_NUM_SHIFT)
#define     SRC_BUS_NUM(x)                ((x) << SRC_BUS_NUM_SHIFT)
#define     OP_START(x)                   ((x) << OP_START_SHIFT)
#define     OP_CMPT_ACK_EN(x)             ((x) << OP_CMPT_ACK_EN_SHIFT)
#define     OP_CMPT_ACK(x)                ((x) << OP_CMPT_ACK_SHIFT)
#define DMAC_SRC_ADDR                   (0x10)
#define DMAC_DST_ADDR                   (0x14)
#define DMAC_TRANS_SIZE                 (0x18)
#define DMAC_MAX_BURLST_LENGTH          (0x34)


/*=============================================================================
 * NFC
 *============================================================================*/
#define NFCTRL_BASE                     (VPL_NFC_MMR_BASE + 0x1300)
#define NFCTRL_BUF_BASE                 VPL_BRC_RAM_BASE
#define FLCONF                          0x00
#define FLCTRL                          0x04
#define FLCOMM                          0x08
#define FLADDR0L                        0x0C
#define FLADDR1L                        0x10
#define FLDATA                          0x14
#define FLPB0                           0x18
#define FLSTATE                         0x1C
#define FLLOOKUP0                       0x20
#define FLLOOKUP1                       0x24
#define FLLOOKUP2                       0x28
#define FLLOOKUP3                       0x2C
#define FLLOOKUP4                       0x30
#define FLLOOKUP5                       0x34
#define FLLOOKUP6                       0x38
#define FLLOOKUP7                       0x3C
#define FLECCSTATUS                     0x40
#define FLADDR0H                        0x44
#define FLADDR1H                        0x48
#define FLLOOKUPEN                      0x4C
#define FLDMAADDR                       0x80
#define FLDMACTRL                       0x84
#define FLDMACNTR                       0x88

#define FLCONF_TRP_BIT                  5
#define FLCONF_TWP_BIT                  11
#define FLCONF_TWHR_BIT                 17
#define FLCONF_TWB_BIT                  23

#define FLSTATE_DMA_BUSY                0x80
#define FLSTATE_FSM_BUSY                0x40
#define FLSTATE_RNB_ST                  0x01
#define FLSTATE_RNB_EDGE                0x02

#define FLCONF_INIT_DATA                0x3FFFFFFF

#define FLCTRL_PAGESIZE                 0x800
#define FLCTRL_DATA                     FLCTRL_PAGESIZE

#define FLDMACNTR_DMA_CNTR_BIT          16

#define FLDMACTRL_START_FLAG            0x1000000
#define FLDMACTRL_DMA_DIR               0x10000         // read from internal BUFFER and write to AHB
#define FLDMACTRL_DMA_SIZE              0xC000          // 32-bit data transfer size
#define FLDMACTRL_DMA_BURST             0x1000          // single transfer address increment
#define FLDMACTRL_DATA                  FLDMACTRL_START_FLAG | FLDMACTRL_DMA_DIR | FLDMACTRL_DMA_SIZE | FLDMACTRL_DMA_BURST

/*-----------------------------------------------------------------------------
 * NAND Flash definitions
 *----------------------------------------------------------------------------*/
// command code
#define NANDFLASH_PAGE_READ             0x0030

#define PAGES_PER_BLOCK_SHIFT           (6)
#define PAGES_PER_BLOCK                 (1 << PAGES_PER_BLOCK_SHIFT)
#define PAGES_PER_BLOCK_MASK            (PAGES_PER_BLOCK - 1)
#define NAND_PAGE_SIZE_SHIFT            (11)
#define NAND_PAGE_SIZE                  (1 << NAND_PAGE_SIZE_SHIFT)
#define NAND_PAGE_SIZE_MASK             (NAND_PAGE_SIZE - 1)
#define COL_ADDR_BIT                    12              // NFC only supports (2K+64)B page size
#define PAGE_DATA_SIZE_MASK             0x7FF      // NFC page data size : 2K (not includes of spare area)


/*=============================================================================
 * SSIC
 *============================================================================*/
#define SSI_CTRLR0                      0x0
#define SSI_CTRLR1                      0x4
#define SSI_SSIENR                      0x08
#define SSI_SER                         0x10
#define SSI_BAUDR                       0x14
#define SSI_TXFTLR                      0x18
#define SSI_RXFTLR                      0x1C
#define SSI_SR                          0x28
#define SSI_IMR                         0x2C
#define SSI_DMACR                       0x4C
#define SSI_DMARDLR                     0x54
#define SSI_DR                          0x60


/*=============================================================================
 * APBC
 *============================================================================*/
#define APBC_SLAVE_SSIC				          (1)
#define APBC_SLAVE_I2SSC0			          (3)
#define APBC_SLAVE_I2SSC1			          (4)
#define APBC_SLAVE_I2SSC2			          (5)
#define APBC_SLAVE_I2SSC3			          (6)
#define APBC_SLAVE_I2SSC4			          (7)
#define APBC_SLAVE_USBC				          (14)
#define APBC_SLAVE_PCIEC			          (15)
#define APBC_SLAVE_TMRC				          (16)
#define APBC_SLAVE_WDTC				          (17)
#define APBC_SLAVE_GPIOC 			          (18)
#define APBC_SLAVE_AGPOC			          (19)
#define APBC_SLAVE_UARTC0			          (20)
#define APBC_SLAVE_UARTC1			          (21)
#define APBC_SLAVE_UARTC2			          (22)
#define APBC_SLAVE_UARTC3			          (23)
#define APBC_SLAVE_IRDAC			          (24)
#define APBC_SLAVE_ABITSPLLC				  (30)
#define APBC_SLAVE_SYSC				          (31)
#define APBC_SLAVE_BASESIZE(n)				  ((n)*4)

#define APBC_DMA_n_SRC_ADDR(n)          (0x90 + (n)*16)
#define APBC_DMA_n_DES_ADDR(n)          (0x94 + (n)*16)
#define APBC_DMA_n_LLP(n)               (0x98 + (n)*16)
#define APBC_DMA_n_CTRL(n)              (0x9c + (n)*16)


/*=============================================================================
 * PCIEC
 *============================================================================*/
#define PCIESSC_CFG_0                       (0x00000004)
#define     PCIEC_RESET_DONE_SHIFT                  (30)
#define     PHY_CFG_DONE_SHIFT                      ( 0)
#define     PCIEC_RESET_DONE(x)                     ((x) << PCIEC_RESET_DONE_SHIFT)
#define     PHY_CFG_DONE(x)                         ((x) << PHY_CFG_DONE_SHIFT)

#define PCIEC_DBI_BASE                      VPL_PCIEC_MMR_BASE
#define PCI_BASE_ADDRESS_0                  (0x00000010)
#define PCI_BASE_ADDRESS_1                  (0x00000014)
#define PCI_BASE_ADDRESS_2                  (0x00000018)
#define PCI_BASE_ADDRESS_3                  (0x0000001C)
#define PCI_BASE_ADDRESS_4                  (0x00000020)
#define PCI_BASE_ADDRESS_5                  (0x00000024)

#define PCIEC_DBI_BAR_MASK_BASE             (PCIEC_DBI_BASE + 0x00001000)
#define DBI_BAR0_MASK                       (0x00000010)
#define DBI_BAR1_MASK                       (0x00000014)
#define DBI_BAR2_MASK                       (0x00000018)
#define DBI_BAR3_MASK                       (0x0000001C)
#define DBI_BAR4_MASK                       (0x00000020)
#define DBI_BAR5_MASK                       (0x00000024)
#define     BAR_ENABLED_SHIFT                       ( 0)
#define     BAR_ENABLED(x)                          ((x) << BAR_ENABLED_SHIFT)

#define PCIEC_ELBI_BASE                     (PCIEC_DBI_BASE + 0x40000)
#define PCIEC_ELBI_CTRL                     (0x00000000)
#define     LTSSM_STATE_MASK_SHIFT                  (24)
#define     APP_REQ_RETRY_EN_SHIFT                  ( 8)
#define     APP_LTSSM_ENABLE_SHIFT                  ( 0)
#define     LTSSM_STATE_MASK                        (0x1F << LTSSM_STATE_MASK_SHIFT)
#define     APP_REQ_RETRY_EN(x)                     ((x) << APP_REQ_RETRY_EN_SHIFT)
#define     APP_LTSSM_ENABLE(x)                     ((x) << APP_LTSSM_ENABLE_SHIFT)
#define PCIEC_INBOUND_CTRL                  (0x00000004)


#endif /* !_MOZART_H_ */
