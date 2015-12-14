#ifndef __MOZART_DRAM_H__
#define __MOZART_DRAM_H__

#define MAX_DDRSDMC_NUMBERS      2

#define DDR2SDMC0_MMR_BASE       0xC4000000
#define DDR2SDMC1_MMR_BASE       0xC5000000
#define DDR2SDMC_MMR_SIZE        0x0C

#define AHBC0_MMR_BASE                     0xC0000000
#define AHBC1_MMR_BASE                     0xC1000000
#define AHBC_MMR_SLAVE_BASESIZE(slavenum)  (0x08 + ((slavenum) * 4))
#define DDR1_SLAVENUM 62
#define DDR0_SLAVENUM 63


#define CALC_DDR_SIZE(data)   (1UL << (((data)&0x0000000F)+(((data)&0x000000F0)>>4) + (((data)&0x00000300)>>8) + 2))

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

#endif
