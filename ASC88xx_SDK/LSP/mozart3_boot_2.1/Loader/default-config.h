
/*
 * CONFIG_VERSION
 * 32-bit version number of config file
 */
 
 
#ifndef CONFIG_VERSION
  #define CONFIG_VERSION   (0xFFFFFFFF)
#endif

/*
 * CONFIG_SYSTEM_DDR
 * physical DRAM at which runtime system software runs
 * 0: physical DRAM 0
 * 1: physical DRAM 1
 */
#ifndef CONFIG_SYSTEM_DDR
  #define CONFIG_SYSTEM_DDR   (1)
#endif

/*
 * CONFIG_DDR_PORT_0 / CONFIG_DDR_PORT_1
 * 0: there is no DDR memory on this bus
 * 1: there is a DDR memory on this bus
 */
#ifndef CONFIG_DDR_PORT_0
  #define CONFIG_DDR_PORT_0   (1)
#endif
#ifndef CONFIG_DDR_PORT_1
  #define CONFIG_DDR_PORT_1   (1)
#endif

/*
 * CONFIG_READ_ODT / CONFIG_WRITE_ODT
 * 0: disable ODT
 * else: see file README for valid value
 */
#ifndef CONFIG_READ_ODT
  #define CONFIG_READ_ODT   (0)
#endif
#ifndef CONFIG_WRITE_ODT
  #define CONFIG_WRITE_ODT   (0)
#endif

/*
 * CONFIG_DYNAMIC_READ_ODT
 * 0: read ODT is static
 * 1: read ODT is dynamic
 */
#ifndef CONFIG_DYNAMIC_READ_ODT
  #define CONFIG_DYNAMIC_READ_ODT   (1)
#endif


/* DRAM training option */
#ifndef CONFIG_0_PUB_BUILTIN_TRAINING
  #if CONFIG_DDR_TYPE == DDR3
    #define CONFIG_0_PUB_BUILTIN_TRAINING (n) 
  #else
    #define CONFIG_0_PUB_BUILTIN_TRAINING (y) 
  #endif
#endif
#ifndef CONFIG_1_PUB_BUILTIN_TRAINING
  #if CONFIG_DDR_TYPE == DDR3
    #define CONFIG_1_PUB_BUILTIN_TRAINING (n) 
  #else
    #define CONFIG_1_PUB_BUILTIN_TRAINING (y) 
  #endif
#endif

/*
 * CONFIG_READ_EYE_TRAINING / CONFIG_WRITE_EYE_TRAINING
 * 0: disable training of read/write data eye
 * 1: enable training of read/write data eye
 */
#ifndef CONFIG_READ_EYE_TRAINING
  #define CONFIG_READ_EYE_TRAINING   (1)
#endif

#ifndef CONFIG_WRITE_EYE_TRAINING
  #define CONFIG_WRITE_EYE_TRAINING   (1)
#endif

/*
 * Mode of maximum average periodic refresh time.
 */
#ifndef CONFIG_T_REF_MODE
  #define CONFIG_T_REF_MODE   NORMAL
#endif

/*************************** SYSC ****************************/

/* SYSC_CLK_EN_CTRL_0 */
#ifndef CFG_SSIC_CLK_EN
  #define CFG_SSIC_CLK_EN   (1)
#endif
#ifndef CFG_PLLC_CLK_EN
  #define CFG_PLLC_CLK_EN   (1)
#endif
#ifndef CFG_PCIEC_CLK_EN
  #define CFG_PCIEC_CLK_EN   (1)
#endif
#ifndef CFG_NFC_CLK_EN
  #define CFG_NFC_CLK_EN   (1)
#endif
#ifndef CFG_MSHC_1_CLK_EN
  #define CFG_MSHC_1_CLK_EN   (0)
#endif
#ifndef CFG_MSHC_0_CLK_EN
  #define CFG_MSHC_0_CLK_EN   (1)
#endif
#ifndef CFG_MEAE_CLK_EN
  #define CFG_MEAE_CLK_EN   (0)
#endif
#ifndef CFG_JEBE_CLK_EN
  #define CFG_JEBE_CLK_EN   (0)
#endif
#ifndef CFG_IRE_CLK_EN
  #define CFG_IRE_CLK_EN   (0)
#endif
#ifndef CFG_IRDAC_CLK_EN
  #define CFG_IRDAC_CLK_EN   (0)
#endif
#ifndef CFG_INTC_CLK_EN
  #define CFG_INTC_CLK_EN   (1)
#endif
#ifndef CFG_IBPE_CLK_EN
  #define CFG_IBPE_CLK_EN   (0)
#endif
#ifndef CFG_I2SSC_4_CLK_EN
  #define CFG_I2SSC_4_CLK_EN   (1)
#endif
#ifndef CFG_I2SSC_3_CLK_EN
  #define CFG_I2SSC_3_CLK_EN   (1)
#endif
#ifndef CFG_I2SSC_2_CLK_EN
  #define CFG_I2SSC_2_CLK_EN   (1)
#endif
#ifndef CFG_I2SSC_1_CLK_EN
  #define CFG_I2SSC_1_CLK_EN   (1)
#endif
#ifndef CFG_I2SSC_0_CLK_EN
  #define CFG_I2SSC_0_CLK_EN   (1)
#endif
#ifndef CFG_H4EE_CLK_EN
  #define CFG_H4EE_CLK_EN   (0)
#endif
#ifndef CFG_GPIOC_CLK_EN
  #define CFG_GPIOC_CLK_EN   (1)
#endif
#ifndef CFG_GMAC_CLK_EN
  #define CFG_GMAC_CLK_EN   (1)
#endif
#ifndef CFG_DMAC_CLK_EN
  #define CFG_DMAC_CLK_EN   (1)
#endif
#ifndef CFG_DIE_CLK_EN
  #define CFG_DIE_CLK_EN   (0)
#endif
#ifndef CFG_DCE_CLK_EN
  #define CFG_DCE_CLK_EN   (0)
#endif
#ifndef CFG_BRC_CLK_EN
  #define CFG_BRC_CLK_EN   (1)
#endif
#ifndef CFG_APB3C_CLK_EN
  #define CFG_APB3C_CLK_EN   (1)
#endif
#ifndef CFG_AGPOC_CLK_EN
  #define CFG_AGPOC_CLK_EN   (1)
#endif

/* SYSC_CLK_EN_CTRL_1 */
#ifndef CFG_I2SSC_RX_CLK_EN
  #define CFG_I2SSC_RX_CLK_EN   (1)
#endif
#ifndef CFG_I2SSC_TX_CLK_EN
  #define CFG_I2SSC_TX_CLK_EN   (1)
#endif
#ifndef CFG_WDTC_CLK_EN
  #define CFG_WDTC_CLK_EN   (1)
#endif
#ifndef CFG_VOC_CLK_EN
  #define CFG_VOC_CLK_EN   (0)
#endif
#ifndef CFG_VIC_CLK_EN
  #define CFG_VIC_CLK_EN   (0x0)
#endif
#ifndef CFG_USBC_CLK_EN
  #define CFG_USBC_CLK_EN   (1)
#endif
#ifndef CFG_UARTC_3_CLK_EN
  #define CFG_UARTC_3_CLK_EN   (1)
#endif
#ifndef CFG_UARTC_2_CLK_EN
  #define CFG_UARTC_2_CLK_EN   (1)
#endif
#ifndef CFG_UARTC_1_CLK_EN
  #define CFG_UARTC_1_CLK_EN   (1)
#endif
#ifndef CFG_UARTC_0_CLK_EN
  #define CFG_UARTC_0_CLK_EN   (1)
#endif
#ifndef CFG_TMRC_CLK_EN
  #define CFG_TMRC_CLK_EN   (1)
#endif
#ifndef CFG_SYSC_CLK_EN
  #define CFG_SYSC_CLK_EN   (1)
#endif

/* SYSC_MON_CLK_SEL */
#ifndef CFG_MON_CLK_1
  #define CFG_MON_CLK_1   (0x3F)
#endif
#ifndef CFG_MON_CLK_0
  #define CFG_MON_CLK_0   (0x12)
#endif

/* SYSC_PAD_EN_CTRL */
#ifndef CFG_WDTC_PAD_EN
  #define CFG_WDTC_PAD_EN   (1)
#endif
#ifndef CFG_VOC_PAD_EN
  #define CFG_VOC_PAD_EN   (0x0)
#endif
#ifndef CFG_VIC_DEV_1_PAD_EN
  #define CFG_VIC_DEV_1_PAD_EN   (0)
#endif
#ifndef CFG_VIC_DEV_0_PAD_EN
  #define CFG_VIC_DEV_0_PAD_EN   (0)
#endif
#ifndef CFG_USBC_PAD_EN
  #define CFG_USBC_PAD_EN   (0)
#endif
#ifndef CFG_UARTC_3_PAD_EN
  #define CFG_UARTC_3_PAD_EN   (1)
#endif
#ifndef CFG_UARTC_2_PAD_EN
  #define CFG_UARTC_2_PAD_EN   (1)
#endif
#ifndef CFG_UARTC_1_PAD_EN
  #define CFG_UARTC_1_PAD_EN   (1)
#endif
#ifndef CFG_UARTC_0_PAD_EN
  #define CFG_UARTC_0_PAD_EN   (1)
#endif
#ifndef CFG_SSIC_PAD_EN
  #define CFG_SSIC_PAD_EN   (1)
#endif
#ifndef CFG_NFC_PAD_EN
  #define CFG_NFC_PAD_EN   (1)
#endif
#ifndef CFG_MSHC_1_PAD_EN
  #define CFG_MSHC_1_PAD_EN   (0)
#endif
#ifndef CFG_MSHC_0_PAD_EN
  #define CFG_MSHC_0_PAD_EN   (1)
#endif
#ifndef CFG_I2SSC_4_PAD_EN
  #define CFG_I2SSC_4_PAD_EN   (1)
#endif
#ifndef CFG_I2SSC_3_PAD_EN
  #define CFG_I2SSC_3_PAD_EN   (1)
#endif
#ifndef CFG_I2SSC_2_PAD_EN
  #define CFG_I2SSC_2_PAD_EN   (1)
#endif
#ifndef CFG_I2SSC_1_PAD_EN
  #define CFG_I2SSC_1_PAD_EN   (1)
#endif
#ifndef CFG_I2SSC_0_PAD_EN
  #define CFG_I2SSC_0_PAD_EN   (1)
#endif
#ifndef CFG_HOSTC_PAD_EN
  #define CFG_HOSTC_PAD_EN   (1)
#endif
#ifndef CFG_GMAC_PAD_EN
  #define CFG_GMAC_PAD_EN   (1)
#endif

/* SYSC_SLEW_CTRL */
#ifndef CFG_VOC_SLEW_RATE_CTRL
  #define CFG_VOC_SLEW_RATE_CTRL   (0)
#endif
#ifndef CFG_GMAC_SLEW_RATE_CTRL
  #define CFG_GMAC_SLEW_RATE_CTRL   (0)
#endif

/* SYSC_PULL_CTRL */
#ifndef CFG_WDTC_CMD_PUP_EN
  #define CFG_WDTC_CMD_PUP_EN   (1)
#endif
#ifndef CFG_UARTC_3_SDA_PUP_EN
  #define CFG_UARTC_3_SDA_PUP_EN   (1)
#endif
#ifndef CFG_UARTC_2_SDA_PUP_EN
  #define CFG_UARTC_2_SDA_PUP_EN   (1)
#endif
#ifndef CFG_UARTC_1_SDA_PUP_EN
  #define CFG_UARTC_1_SDA_PUP_EN   (1)
#endif
#ifndef CFG_UARTC_0_SDA_PUP_EN
  #define CFG_UARTC_0_SDA_PUP_EN   (1)
#endif
#ifndef CFG_SSIC_SEL_PUP_EN
  #define CFG_SSIC_SEL_PUP_EN   (1)
#endif
#ifndef CFG_NFC_DATA_PUP_EN
  #define CFG_NFC_DATA_PUP_EN   (1)
#endif
#ifndef CFG_NFC_BUSY_PUP_EN
  #define CFG_NFC_BUSY_PUP_EN   (1)
#endif
#ifndef CFG_NFC_COM_PUP_EN
  #define CFG_NFC_COM_PUP_EN   (1)
#endif
#ifndef CFG_MSHC_1_DATA_PUP_EN
  #define CFG_MSHC_1_DATA_PUP_EN   (1)
#endif
#ifndef CFG_MSHC_1_CMD_PUP_EN
  #define CFG_MSHC_1_CMD_PUP_EN   (1)
#endif
#ifndef CFG_MSHC_1_COM_PUP_EN
  #define CFG_MSHC_1_COM_PUP_EN   (1)
#endif
#ifndef CFG_MSHC_0_DATA_PUP_EN
  #define CFG_MSHC_0_DATA_PUP_EN   (1)
#endif
#ifndef CFG_MSHC_0_CMD_PUP_EN
  #define CFG_MSHC_0_CMD_PUP_EN   (1)
#endif
#ifndef CFG_MSHC_0_COM_PUP_EN
  #define CFG_MSHC_0_COM_PUP_EN   (1)
#endif
#ifndef CFG_HOSTC_COM_PUP_EN
  #define CFG_HOSTC_COM_PUP_EN   (1)
#endif

/* SYSC_DRV_STRENGTH_CTRL_0 / SYSC_DRV_STRENGTH_CTRL_1 */
#define CFG_AGPOC_GPIOC_0_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_1_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_2_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_3_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_4_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_5_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_6_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_7_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_8_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_9_DRV_STR    (0x0)
#define CFG_AGPOC_GPIOC_10_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_11_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_12_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_13_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_14_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_15_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_16_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_17_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_18_DRV_STR   (0x0)
#define CFG_AGPOC_GPIOC_19_DRV_STR   (0x0)

/* SYSC_DRV_STRENGTH_CTRL_2 */
#ifndef CFG_SYSC_DRV_STR
  #define CFG_SYSC_DRV_STR   (0x0)
#endif
#ifndef CFG_WDTC_DRV_STR
  #define CFG_WDTC_DRV_STR   (0x0)
#endif
#ifndef CFG_VOC_DRV_STR
  #define CFG_VOC_DRV_STR   (0x0)
#endif
#ifndef CFG_USBC_DRV_STR
  #define CFG_USBC_DRV_STR   (0x0)
#endif
#ifndef CFG_UARTC_3_DRV_STR
  #define CFG_UARTC_3_DRV_STR   (0x0)
#endif
#ifndef CFG_UARTC_2_DRV_STR
  #define CFG_UARTC_2_DRV_STR   (0x0)
#endif
#ifndef CFG_UARTC_1_DRV_STR
  #define CFG_UARTC_1_DRV_STR   (0x0)
#endif
#ifndef CFG_UARTC_0_DRV_STR
  #define CFG_UARTC_0_DRV_STR   (0x0)
#endif
#ifndef CFG_SSIC_DRV_STR
  #define CFG_SSIC_DRV_STR   (0x0)
#endif
#ifndef CFG_NFC_DRV_STR
  #define CFG_NFC_DRV_STR   (0x0)
#endif
#ifndef CFG_MSHC_1_DRV_STR
  #define CFG_MSHC_1_DRV_STR   (0x0)
#endif
#ifndef CFG_MSHC_0_DRV_STR
  #define CFG_MSHC_0_DRV_STR   (0x0)
#endif
#ifndef CFG_I2SSC_DRV_STR
  #define CFG_I2SSC_DRV_STR   (0x0)
#endif
#ifndef CFG_HOSTC_DRV_STR
  #define CFG_HOSTC_DRV_STR   (0x0)
#endif

/*************************** PUB ****************************/

/* PIR */
#ifndef CFG_INITBYP
  #define CFG_INITBYP   (0)
#endif
#ifndef CFG_ZCALBYP
  #define CFG_ZCALBYP   (0)
#endif
#ifndef CFG_DCALBYP
  #define CFG_DCALBYP   (0)
#endif
#ifndef CFG_LOCKBYP
  #define CFG_LOCKBYP   (0)
#endif
#ifndef CFG_CLRSR
  #define CFG_CLRSR   (0)
#endif
#ifndef CFG_CTLDINIT
  #define CFG_CTLDINIT   (0)
#endif
#ifndef CFG_PLLBYP
  #define CFG_PLLBYP   (0)
#endif
#ifndef CFG_ICPC
  #define CFG_ICPC   (0)
#endif
#ifndef CFG_WREYE
  #define CFG_WREYE   (0)
#endif
#ifndef CFG_RDEYE
  #define CFG_RDEYE   (0)
#endif
#ifndef CFG_WRDSKW
  #define CFG_WRDSKW   (0)
#endif
#ifndef CFG_RDDSKW
  #define CFG_RDDSKW   (0)
#endif
#ifndef CFG_WLADJ
  #define CFG_WLADJ   (0)
#endif
#ifndef CFG_QSGATE
  #define CFG_QSGATE   (0)
#endif
#ifndef CFG_WL
  #define CFG_WL   (0)
#endif
#ifndef CFG_DRAMINIT
  #define CFG_DRAMINIT   (0)
#endif
#ifndef CFG_DRAMRST
  #define CFG_DRAMRST   (0)
#endif
#ifndef CFG_PHYRST
  #define CFG_PHYRST   (1)
#endif
#ifndef CFG_DCAL
  #define CFG_DCAL   (1)
#endif
#ifndef CFG_PLLINIT
  #define CFG_PLLINIT   (1)
#endif
#ifndef CFG_ZCAL
  #define CFG_ZCAL   (1)
#endif
#ifndef CFG_INIT
  #define CFG_INIT   (1)
#endif

/* PGCR0 */
#ifndef CFG_CKEN
  #define CFG_CKEN   (0x02)
#endif
#ifndef CFG_DTOSEL
  #define CFG_DTOSEL   (0)
#endif
#ifndef CFG_OSCWDL
  #define CFG_OSCWDL   (0x3)
#endif
#ifndef CFG_OSCDIV
  #define CFG_OSCDIV   (0x7)
#endif
#ifndef CFG_OSCEN
  #define CFG_OSCEN   (0)
#endif
#ifndef CFG_DLTST
  #define CFG_DLTST   (0)
#endif
#ifndef CFG_DLTMODE
  #define CFG_DLTMODE   (0)
#endif
#ifndef CFG_RDBVT
  #define CFG_RDBVT   (1)
#endif
#ifndef CFG_WDBVT
  #define CFG_WDBVT   (1)
#endif
#ifndef CFG_RGLVT
  #define CFG_RGLVT   (1)
#endif
#ifndef CFG_RDLVT
  #define CFG_RDLVT   (1)
#endif
#ifndef CFG_WDLVT
  #define CFG_WDLVT   (1)
#endif
#ifndef CFG_WLLVT
  #define CFG_WLLVT   (1)
#endif

/* PGCR1 */
#ifndef CFG_LBMODE
  #define CFG_LBMODE   (0)
#endif
#ifndef CFG_LBGDQS
  #define CFG_LBGDQS   (0)
#endif
#ifndef CFG_LBDQSS
  #define CFG_LBDQSS   (0x0)
#endif
#ifndef CFG_IOLB
  #define CFG_IOLB   (0)
#endif
#ifndef CFG_INHVT
  #define CFG_INHVT   (0)
#endif
#ifndef CFG_PHYHRST
  #define CFG_PHYHRST   (1)
#endif
#ifndef CFG_ZCKSEL
  #define CFG_ZCKSEL   (0x2)
#endif
#ifndef CFG_DLDLMT
  #define CFG_DLDLMT   (0x01)
#endif
#ifndef CFG_FDEPTH
  #define CFG_FDEPTH   (0x2)
#endif
#ifndef CFG_LPFDEPTH
  #define CFG_LPFDEPTH   (0x0)
#endif
#ifndef CFG_LPFEN
  #define CFG_LPFEN   (1)
#endif
#ifndef CFG_MDLEN_PGCR1
  #define CFG_MDLEN_PGCR1   (0)
#endif
#ifndef CFG_IODDRM
  #define CFG_IODDRM   (0x0)
#endif
#ifndef CFG_WLSELT
  #define CFG_WLSELT   (0)
#endif
#ifndef CFG_WLRANK
  #define CFG_WLRANK   (0x3)
#endif
#ifndef CFG_WLUNCRT
  #define CFG_WLUNCRT   (0)
#endif
#ifndef CFG_WLSTEP
  #define CFG_WLSTEP   (0)
#endif
#ifndef CFG_WLFULL
  #define CFG_WLFULL   (1)
#endif
#ifndef CFG_PDDISDX
  #define CFG_PDDISDX   (1)
#endif

/* PTR0 */
#ifndef CFG_T_PLLPD
  #define CFG_T_PLLPD   (0x0C8)
#endif
#ifndef CFG_T_PLLGS
  #define CFG_T_PLLGS   (0x0FA0)
#endif
#ifndef CFG_T_PHYRST
  #define CFG_T_PHYRST   (0x10)
#endif

/* PTR1 */
#ifndef CFG_T_PLLLOCK
  #define CFG_T_PLLLOCK   (0x0010)
#endif
#ifndef CFG_T_PLLRST
  #define CFG_T_PLLRST   (0x0008)
#endif

/* PTR2 */
#ifndef CFG_T_WLDLYS
  #define CFG_T_WLDLYS   (0x10)
#endif
#ifndef CFG_T_CALH
  #define CFG_T_CALH   (0xF)
#endif
#ifndef CFG_T_CALS
  #define CFG_T_CALS   (0xF)
#endif
#ifndef CFG_T_CALON
  #define CFG_T_CALON   (0xF)
#endif

/* PTR3 */
#ifndef CFG_T_DINIT1
  #define CFG_T_DINIT1   (0xC0)
#endif
#ifndef CFG_T_DINIT0
  #define CFG_T_DINIT0   (0x1)
#endif

/* PTR4 */
#ifndef CFG_T_DINIT3
  #define CFG_T_DINIT3   (0x156)
#endif
#ifndef CFG_T_DINIT2
  #define CFG_T_DINIT2   (0x1)
#endif

/* ZQ0CR1 (must before DXCCR due to CONFIG_READ_ODT) */
#if defined(CONFIG_READ_ODT) && CONFIG_READ_ODT != 0
  #if CONFIG_DDR_TYPE == DDR3
    #if   CONFIG_READ_ODT == 120
      #define CFG_ODT_DIV   (0x1)
    #elif CONFIG_READ_ODT == 60
      #define CFG_ODT_DIV   (0x5)
    #elif CONFIG_READ_ODT == 40
      #define CFG_ODT_DIV   (0x8)
    #else
      #undef CONFIG_READ_ODT
      #define CONFIG_READ_ODT   (0)
      #define CFG_ODT_DIV   (0x7)
    #endif
  #else /* DDR2 */
    #if   CONFIG_READ_ODT == 150
      #define CFG_ODT_DIV   (0x1)
    #elif CONFIG_READ_ODT == 75
      #define CFG_ODT_DIV   (0x4)
    #elif CONFIG_READ_ODT == 50
      #define CFG_ODT_DIV   (0x6)
    #else
      #undef CONFIG_READ_ODT
      #define CONFIG_READ_ODT   (0)
      #define CFG_ODT_DIV   (0x7)
    #endif
  #endif
#else
  #undef CONFIG_READ_ODT
  #define CONFIG_READ_ODT   (0)
  #define CFG_ODT_DIV   (0x7)
#endif
#ifndef CFG_IMPEDANCE_DIV
  #define CFG_IMPEDANCE_DIV   (0xB)
#endif

/* DXCCR (must after ZQ0CR1 due to CONFIG_READ_ODT) */
#ifndef CFG_DXSR
  #define CFG_DXSR   (0x0)
#endif
#ifndef CFG_DQSNRES
  #define CFG_DQSNRES   (0xC)
#endif
#ifndef CFG_DQSRES
  #define CFG_DQSRES   (0x4)
#endif
#ifndef CFG_DXPDR_DXCCR
  #define CFG_DXPDR_DXCCR   (0)
#endif
#ifndef CFG_DXPDD_DXCCR
  #define CFG_DXPDD_DXCCR   (0)
#endif
#ifndef CFG_MDLEN_DXCCR
  #define CFG_MDLEN_DXCCR   (1)
#endif
#ifndef CFG_DXIOM_DXCCR
  #define CFG_DXIOM_DXCCR   (0)
#endif
#ifndef CFG_DXODT
  #define CFG_DXODT   (CONFIG_READ_ODT && 1)
#endif

/* DSGCR */
#ifndef CFG_CKEOE
  #define CFG_CKEOE   (1)
#endif
#ifndef CFG_RSTOE
  #define CFG_RSTOE   (1)
#endif
#ifndef CFG_ODTOE
  #define CFG_ODTOE   (1)
#endif
#ifndef CFG_CKOE
  #define CFG_CKOE   (1)
#endif
#ifndef CFG_ODTPDD
  #define CFG_ODTPDD   (0)
#endif
#ifndef CFG_CKEPDD
  #define CFG_CKEPDD   (0)
#endif
#ifndef CFG_SDRMODE
  #define CFG_SDRMODE   (0)
#endif
#ifndef CFG_RRMODE
  #define CFG_RRMODE   (0)
#endif
#ifndef CFG_DQSGX
  #define CFG_DQSGX   (0)
#endif
#ifndef CFG_CUAEN
  #define CFG_CUAEN   (0)
#endif
#ifndef CFG_LPPLLPD
  #define CFG_LPPLLPD   (1)
#endif
#ifndef CFG_LPIOPD
  #define CFG_LPIOPD   (1)
#endif
#ifndef CFG_ZUEN
  #define CFG_ZUEN   (0)
#endif
#ifndef CFG_BDISEN
  #define CFG_BDISEN   (1)
#endif
#ifndef CFG_PUREN
  #define CFG_PUREN   (1)
#endif

/* DCR */
#ifndef CFG_DCR_RDIMM
  #define CFG_DCR_RDIMM   (0)
#endif
#ifndef CFG_DCR_UDIMM
  #define CFG_DCR_UDIMM   (0)
#endif
#ifndef CFG_DCR_DDR2T
  #define CFG_DCR_DDR2T   (0)
#endif
#ifndef CFG_DCR_NOSRA
  #define CFG_DCR_NOSRA   (0)
#endif
#ifndef CFG_DCR_BYTEMASK
  #define CFG_DCR_BYTEMASK   (1)
#endif
#ifndef CFG_DCR_MPRDQ
  #define CFG_DCR_MPRDQ   (0)
#endif
#ifndef CFG_DCR_PDQ
  #define CFG_DCR_PDQ   (0)
#endif
#ifndef CFG_DCR_DDR8BNK
  #define CFG_DCR_DDR8BNK   (1)
#endif
#undef CFG_DCR_DDRMD
#if CONFIG_DDR_TYPE == DDR3
  #define CFG_DCR_DDRMD   (0x3)
#elif CONFIG_DDR_TYPE == DDR2
  #define CFG_DCR_DDRMD   (0x2)
#else
#error CONFIG_DDR_TYPE not defined!
#endif

/* DTPR0 */
#ifndef CFG_DTPR_T_RC
  #define CFG_DTPR_T_RC   (CFG_T_RC)
#endif
#ifndef CFG_DTPR_T_RRD
  #define CFG_DTPR_T_RRD   (CFG_T_RRD)
#endif
#ifndef CFG_DTPR_T_RAS
  #define CFG_DTPR_T_RAS   (CFG_T_RAS)
#endif
#ifndef CFG_DTPR_T_RCD
  #define CFG_DTPR_T_RCD   (CFG_T_RCD)
#endif
#ifndef CFG_DTPR_T_RP
  #define CFG_DTPR_T_RP   (CFG_T_RP)
#endif
#ifndef CFG_DTPR_T_WTR
  #define CFG_DTPR_T_WTR   (CFG_T_WTR)
#endif
#ifndef CFG_DTPR_T_RTP
  #define CFG_DTPR_T_RTP   (CFG_T_RTP)
#endif

/* DTPR1 */
#ifndef CFG_DTPR_T_AONFD
  #define CFG_DTPR_T_AONFD   (0x0)
#endif
#ifndef CFG_DTPR_T_WLO
  #define CFG_DTPR_T_WLO   (0x3)
#endif
#ifndef CFG_DTPR_T_WLMRD
  #define CFG_DTPR_T_WLMRD   (0x28)
#endif
#ifndef CFG_DTPR_T_RFC
  #define CFG_DTPR_T_RFC   (CFG_T_RFC)
#endif
#ifndef CFG_DTPR_T_FAW
  #define CFG_DTPR_T_FAW   (CFG_T_FAW)
#endif
#ifndef CFG_DTPR_T_MOD
  #if CONFIG_DDR_TYPE == DDR3
    #if CFG_T_MOD >= 12 && CFG_T_MOD <= 17
      #define CFG_DTPR_T_MOD   (CFG_T_MOD - 12)
    #elif CFG_390_T_MOD >= 12 && CFG_390_T_MOD <= 17
      #define CFG_DTPR_T_MOD   (CFG_390_T_MOD - 12)
    #else
      #error Invalid CFG_T_MOD ranage !
    #endif
  #else
    #define CFG_DTPR_T_MOD   (0)
  #endif
#endif
#ifndef CFG_DTPR_T_MRD
  #if CONFIG_DDR_TYPE == DDR3
  	#ifdef CFG_T_MRD
    	#define CFG_DTPR_T_MRD   (CFG_T_MRD - 4)
    #elif defined( CFG_390_T_MRD)
    	#define CFG_DTPR_T_MRD   (CFG_390_T_MRD - 4)
    #endif
  #elif CONFIG_DDR_TYPE == DDR2
    #define CFG_DTPR_T_MRD   (CFG_T_MRD)
  #endif
#endif

/* DTPR2 */
#ifndef CFG_DTPR_T_CCD
  #define CFG_DTPR_T_CCD   (0)
#endif
#ifndef CFG_DTPR_T_RTW
  #define CFG_DTPR_T_RTW   (0)
#endif
#ifndef CFG_DTPR_T_RTODT
  #define CFG_DTPR_T_RTODT   (0)
#endif
#ifndef CFG_DTPR_T_DLLK
  #define CFG_DTPR_T_DLLK   (0x200)
#endif
#ifndef CFG_DTPR_T_CKE
  #define CFG_DTPR_T_CKE   (0x3)
#endif
#ifndef CFG_DTPR_T_XP
  #define CFG_DTPR_T_XP   (0x3)
#endif
#ifndef CFG_DTPR_T_XS
  #define CFG_DTPR_T_XS   (0x44)
#endif

/* ODTCR */
#ifndef CFG_WRODT3
  #define CFG_WRODT3   (0x0)
#endif
#ifndef CFG_WRODT2
  #define CFG_WRODT2   (0x0)
#endif
#ifndef CFG_WRODT1
  #define CFG_WRODT1   (((CONFIG_DDR_PORT_1 && CONFIG_WRITE_ODT) << 1) | (CONFIG_DDR_PORT_0 && CONFIG_WRITE_ODT))
#endif
#ifndef CFG_WRODT0
  #define CFG_WRODT0   (((CONFIG_DDR_PORT_1 && CONFIG_WRITE_ODT) << 1) | (CONFIG_DDR_PORT_0 && CONFIG_WRITE_ODT))
#endif
#ifndef CFG_RDODT3
  #define CFG_RDODT3   (0x0)
#endif
#ifndef CFG_RDODT2
  #define CFG_RDODT2   (0x0)
#endif
#ifndef CFG_RDODT1
  #define CFG_RDODT1   (((CONFIG_DDR_PORT_1 && CONFIG_READ_ODT) << 1) | (CONFIG_DDR_PORT_0 && CONFIG_READ_ODT))
#endif
#ifndef CFG_RDODT0
  #define CFG_RDODT0   (((CONFIG_DDR_PORT_1 && CONFIG_READ_ODT) << 1) | (CONFIG_DDR_PORT_0 && CONFIG_READ_ODT))
#endif

#ifndef CFG_MPR 
	#if (CONFIG_DDR_TYPE == DDR3)
  #define CFG_MPR (1)
	#else
  #define CFG_MPR (0)
	#endif
#endif

/* DXnGCR */
#if CONFIG_READ_ODT == 0 || CONFIG_DYNAMIC_READ_ODT == 0
  #define CFG_DQRTT    (0)
  #define CFG_DQSRTT   (0)
#else
  #define CFG_DQRTT    (1)
  #define CFG_DQSRTT   (1)
#endif
/* DX0GCR */
#ifndef CFG_DX0_CALBYP
  #ifdef CFG_PUB_DX0_CALBYP
    #define CFG_DX0_CALBYP   CFG_PUB_DX0_CALBYP
  #else
    #define CFG_DX0_CALBYP   (0)
  #endif
#endif
#ifndef CFG_DX0_MDLEN
  #ifdef CFG_PUB_DX0_MDLEN
    #define CFG_DX0_MDLEN   CFG_PUB_DX0_MDLEN
  #else
    #define CFG_DX0_MDLEN   (1)
  #endif
#endif
#ifndef CFG_DX0_WLRKEN
  #ifdef CFG_PUB_DX0_WLRKEN
    #define CFG_DX0_WLRKEN   CFG_PUB_DX0_WLRKEN
  #else
    #define CFG_DX0_WLRKEN   (0x1)
  #endif
#endif
#ifndef CFG_DX0_GSHIFT
  #ifdef CFG_PUB_DX0_GSHIFT
    #define CFG_DX0_GSHIFT   CFG_PUB_DX0_GSHIFT
  #else
    #define CFG_DX0_GSHIFT   (0)
  #endif
#endif
#ifndef CFG_DX0_PLLPD
  #ifdef CFG_PUB_DX0_PLLPD
    #define CFG_DX0_PLLPD   CFG_PUB_DX0_PLLPD
  #else
    #define CFG_DX0_PLLPD   (0)
  #endif
#endif
#ifndef CFG_DX0_PLLRST
  #ifdef CFG_PUB_DX0_PLLRST
    #define CFG_DX0_PLLRST   CFG_PUB_DX0_PLLRST
  #else
    #define CFG_DX0_PLLRST   (0)
  #endif
#endif
#ifndef CFG_DX0_RTTOAL
  #ifdef CFG_PUB_DX0_RTTOAL
    #define CFG_DX0_RTTOAL   CFG_PUB_DX0_RTTOAL
  #else
    #define CFG_DX0_RTTOAL   (0)
  #endif
#endif
#ifndef CFG_DX0_RTTOH
  #ifdef CFG_PUB_DX0_RTTOH
    #define CFG_DX0_RTTOH   CFG_PUB_DX0_RTTOH
  #else
    #define CFG_DX0_RTTOH   (0x1)
  #endif
#endif
#undef CFG_DX0_DQRTT
#undef CFG_DX0_DQSRTT
#if CONFIG_READ_ODT == 0 || CONFIG_DYNAMIC_READ_ODT == 0
  #define CFG_DX0_DQRTT    (0)
  #define CFG_DX0_DQSRTT   (0)
#else
  #define CFG_DX0_DQRTT    (1)
  #define CFG_DX0_DQSRTT   (1)
#endif
#ifndef CFG_DX0_DSEN
  #ifdef CFG_PUB_DX0_DSEN
    #define CFG_DX0_DSEN   CFG_PUB_DX0_DSEN
  #else
    #define CFG_DX0_DSEN   (0x1)
  #endif
#endif
#ifndef CFG_DX0_DQSRPD
  #ifdef CFG_PUB_DX0_DQSRPD
    #define CFG_DX0_DQSRPD   CFG_PUB_DX0_DQSRPD
  #else
    #define CFG_DX0_DQSRPD   (0)
  #endif
#endif
#ifndef CFG_DX0_DXPDR
  #ifdef CFG_PUB_DX0_DXPDR
    #define CFG_DX0_DXPDR   CFG_PUB_DX0_DXPDR
  #else
    #define CFG_DX0_DXPDR   (0)
  #endif
#endif
#ifndef CFG_DX0_DXPDD
  #ifdef CFG_PUB_DX0_DXPDD
    #define CFG_DX0_DXPDD   CFG_PUB_DX0_DXPDD
  #else
    #define CFG_DX0_DXPDD   (0)
  #endif
#endif
#ifndef CFG_DX0_DXIOM
  #ifdef CFG_PUB_DX0_DXIOM
    #define CFG_DX0_DXIOM   CFG_PUB_DX0_DXIOM
  #else
    #define CFG_DX0_DXIOM   (0)
  #endif
#endif
#ifndef CFG_DX0_DQODT
  #ifdef CFG_PUB_DX0_DQODT
    #define CFG_DX0_DQODT   CFG_PUB_DX0_DQODT
  #else
    #define CFG_DX0_DQODT   (0)
  #endif
#endif
#ifndef CFG_DX0_DQSODT
  #ifdef CFG_PUB_DX0_DQSODT
    #define CFG_DX0_DQSODT   CFG_PUB_DX0_DQSODT
  #else
    #define CFG_DX0_DQSODT   (0)
  #endif
#endif
#ifndef CFG_DX0_DXEN
  #ifdef CFG_PUB_DX0_DXEN
    #define CFG_DX0_DXEN   CFG_PUB_DX0_DXEN
  #else
    #define CFG_DX0_DXEN   (1) /* TODO: enabled/disabled */
  #endif
#endif

/* DX1GCR */
#ifndef CFG_DX1_CALBYP
  #ifdef CFG_PUB_DX1_CALBYP
    #define CFG_DX1_CALBYP   CFG_PUB_DX1_CALBYP
  #else
    #define CFG_DX1_CALBYP   (0)
  #endif
#endif
#ifndef CFG_DX1_MDLEN
  #ifdef CFG_PUB_DX1_MDLEN
    #define CFG_DX1_MDLEN   CFG_PUB_DX1_MDLEN
  #else
    #define CFG_DX1_MDLEN   (1)
  #endif
#endif
#ifndef CFG_DX1_WLRKEN
  #ifdef CFG_PUB_DX1_WLRKEN
    #define CFG_DX1_WLRKEN   CFG_PUB_DX1_WLRKEN
  #else
    #define CFG_DX1_WLRKEN   (0x1)
  #endif
#endif
#ifndef CFG_DX1_GSHIFT
  #ifdef CFG_PUB_DX1_GSHIFT
    #define CFG_DX1_GSHIFT   CFG_PUB_DX1_GSHIFT
  #else
    #define CFG_DX1_GSHIFT   (0)
  #endif
#endif
#ifndef CFG_DX1_PLLPD
  #ifdef CFG_PUB_DX1_PLLPD
    #define CFG_DX1_PLLPD   CFG_PUB_DX1_PLLPD
  #else
    #define CFG_DX1_PLLPD   (0)
  #endif
#endif
#ifndef CFG_DX1_PLLRST
  #ifdef CFG_PUB_DX1_PLLRST
    #define CFG_DX1_PLLRST   CFG_PUB_DX1_PLLRST
  #else
    #define CFG_DX1_PLLRST   (0)
  #endif
#endif
#ifndef CFG_DX1_RTTOAL
  #ifdef CFG_PUB_DX1_RTTOAL
    #define CFG_DX1_RTTOAL   CFG_PUB_DX1_RTTOAL
  #else
    #define CFG_DX1_RTTOAL   (0)
  #endif
#endif
#ifndef CFG_DX1_RTTOH
  #ifdef CFG_PUB_DX1_RTTOH
    #define CFG_DX1_RTTOH   CFG_PUB_DX1_RTTOH
  #else
    #define CFG_DX1_RTTOH   (0x1)
  #endif
#endif
#undef CFG_DX1_DQRTT
#undef CFG_DX1_DQSRTT
#if CONFIG_READ_ODT == 0 || CONFIG_DYNAMIC_READ_ODT == 0
  #define CFG_DX1_DQRTT    (0)
  #define CFG_DX1_DQSRTT   (0)
#else
  #define CFG_DX1_DQRTT    (1)
  #define CFG_DX1_DQSRTT   (1)
#endif
#ifndef CFG_DX1_DSEN
  #ifdef CFG_PUB_DX1_DSEN
    #define CFG_DX1_DSEN   CFG_PUB_DX1_DSEN
  #else
    #define CFG_DX1_DSEN   (0x1)
  #endif
#endif
#ifndef CFG_DX1_DQSRPD
  #ifdef CFG_PUB_DX1_DQSRPD
    #define CFG_DX1_DQSRPD   CFG_PUB_DX1_DQSRPD
  #else
    #define CFG_DX1_DQSRPD   (0)
  #endif
#endif
#ifndef CFG_DX1_DXPDR
  #ifdef CFG_PUB_DX1_DXPDR
    #define CFG_DX1_DXPDR   CFG_PUB_DX1_DXPDR
  #else
    #define CFG_DX1_DXPDR   (0)
  #endif
#endif
#ifndef CFG_DX1_DXPDD
  #ifdef CFG_PUB_DX1_DXPDD
    #define CFG_DX1_DXPDD   CFG_PUB_DX1_DXPDD
  #else
    #define CFG_DX1_DXPDD   (0)
  #endif
#endif
#ifndef CFG_DX1_DXIOM
  #ifdef CFG_PUB_DX1_DXIOM
    #define CFG_DX1_DXIOM   CFG_PUB_DX1_DXIOM
  #else
    #define CFG_DX1_DXIOM   (0)
  #endif
#endif
#ifndef CFG_DX1_DQODT
  #ifdef CFG_PUB_DX1_DQODT
    #define CFG_DX1_DQODT   CFG_PUB_DX1_DQODT
  #else
    #define CFG_DX1_DQODT   (0)
  #endif
#endif
#ifndef CFG_DX1_DQSODT
  #ifdef CFG_PUB_DX1_DQSODT
    #define CFG_DX1_DQSODT   CFG_PUB_DX1_DQSODT
  #else
    #define CFG_DX1_DQSODT   (0)
  #endif
#endif
#ifndef CFG_DX1_DXEN
  #ifdef CFG_PUB_DX1_DXEN
    #define CFG_DX1_DXEN   CFG_PUB_DX1_DXEN
  #else
    #define CFG_DX1_DXEN   (1) /* TODO: enabled/disabled */
  #endif
#endif

/* DX2GCR */
#ifndef CFG_DX2_CALBYP
  #ifdef CFG_PUB_DX2_CALBYP
    #define CFG_DX2_CALBYP   CFG_PUB_DX2_CALBYP
  #else
    #define CFG_DX2_CALBYP   (0)
  #endif
#endif
#ifndef CFG_DX2_MDLEN
  #ifdef CFG_PUB_DX2_MDLEN
    #define CFG_DX2_MDLEN   CFG_PUB_DX2_MDLEN
  #else
    #define CFG_DX2_MDLEN   (1)
  #endif
#endif
#ifndef CFG_DX2_WLRKEN
  #ifdef CFG_PUB_DX2_WLRKEN
    #define CFG_DX2_WLRKEN   CFG_PUB_DX2_WLRKEN
  #else
    #define CFG_DX2_WLRKEN   (0x1)
  #endif
#endif
#ifndef CFG_DX2_GSHIFT
  #ifdef CFG_PUB_DX2_GSHIFT
    #define CFG_DX2_GSHIFT   CFG_PUB_DX2_GSHIFT
  #else
    #define CFG_DX2_GSHIFT   (0)
  #endif
#endif
#ifndef CFG_DX2_PLLPD
  #ifdef CFG_PUB_DX2_PLLPD
    #define CFG_DX2_PLLPD   CFG_PUB_DX2_PLLPD
  #else
    #define CFG_DX2_PLLPD   (0)
  #endif
#endif
#ifndef CFG_DX2_PLLRST
  #ifdef CFG_PUB_DX2_PLLRST
    #define CFG_DX2_PLLRST   CFG_PUB_DX2_PLLRST
  #else
    #define CFG_DX2_PLLRST   (0)
  #endif
#endif
#ifndef CFG_DX2_RTTOAL
  #ifdef CFG_PUB_DX2_RTTOAL
    #define CFG_DX2_RTTOAL   CFG_PUB_DX2_RTTOAL
  #else
    #define CFG_DX2_RTTOAL   (0)
  #endif
#endif
#ifndef CFG_DX2_RTTOH
  #ifdef CFG_PUB_DX2_RTTOH
    #define CFG_DX2_RTTOH   CFG_PUB_DX2_RTTOH
  #else
    #define CFG_DX2_RTTOH   (0x1)
  #endif
#endif
#undef CFG_DX2_DQRTT
#undef CFG_DX2_DQSRTT
#if CONFIG_READ_ODT == 0 || CONFIG_DYNAMIC_READ_ODT == 0
  #define CFG_DX2_DQRTT    (0)
  #define CFG_DX2_DQSRTT   (0)
#else
  #define CFG_DX2_DQRTT    (1)
  #define CFG_DX2_DQSRTT   (1)
#endif
#ifndef CFG_DX2_DSEN
  #ifdef CFG_PUB_DX2_DSEN
    #define CFG_DX2_DSEN   CFG_PUB_DX2_DSEN
  #else
    #define CFG_DX2_DSEN   (0x1)
  #endif
#endif
#ifndef CFG_DX2_DQSRPD
  #ifdef CFG_PUB_DX2_DQSRPD
    #define CFG_DX2_DQSRPD   CFG_PUB_DX2_DQSRPD
  #else
    #define CFG_DX2_DQSRPD   (0)
  #endif
#endif
#ifndef CFG_DX2_DXPDR
  #ifdef CFG_PUB_DX2_DXPDR
    #define CFG_DX2_DXPDR   CFG_PUB_DX2_DXPDR
  #else
    #define CFG_DX2_DXPDR   (0)
  #endif
#endif
#ifndef CFG_DX2_DXPDD
  #ifdef CFG_PUB_DX2_DXPDD
    #define CFG_DX2_DXPDD   CFG_PUB_DX2_DXPDD
  #else
    #define CFG_DX2_DXPDD   (0)
  #endif
#endif
#ifndef CFG_DX2_DXIOM
  #ifdef CFG_PUB_DX2_DXIOM
    #define CFG_DX2_DXIOM   CFG_PUB_DX2_DXIOM
  #else
    #define CFG_DX2_DXIOM   (0)
  #endif
#endif
#ifndef CFG_DX2_DQODT
  #ifdef CFG_PUB_DX2_DQODT
    #define CFG_DX2_DQODT   CFG_PUB_DX2_DQODT
  #else
    #define CFG_DX2_DQODT   (0)
  #endif
#endif
#ifndef CFG_DX2_DQSODT
  #ifdef CFG_PUB_DX2_DQSODT
    #define CFG_DX2_DQSODT   CFG_PUB_DX2_DQSODT
  #else
    #define CFG_DX2_DQSODT   (0)
  #endif
#endif
#ifndef CFG_DX2_DXEN
  #ifdef CFG_PUB_DX2_DXEN
    #define CFG_DX2_DXEN   CFG_PUB_DX2_DXEN
  #else
    #define CFG_DX2_DXEN   (1) /* TODO: enabled/disabled */
  #endif
#endif

/* DX3GCR */
#ifndef CFG_DX3_CALBYP
  #ifdef CFG_PUB_DX3_CALBYP
    #define CFG_DX3_CALBYP   CFG_PUB_DX3_CALBYP
  #else
    #define CFG_DX3_CALBYP   (0)
  #endif
#endif
#ifndef CFG_DX3_MDLEN
  #ifdef CFG_PUB_DX3_MDLEN
    #define CFG_DX3_MDLEN   CFG_PUB_DX3_MDLEN
  #else
    #define CFG_DX3_MDLEN   (1)
  #endif
#endif
#ifndef CFG_DX3_WLRKEN
  #ifdef CFG_PUB_DX3_WLRKEN
    #define CFG_DX3_WLRKEN   CFG_PUB_DX3_WLRKEN
  #else
    #define CFG_DX3_WLRKEN   (0x1)
  #endif
#endif
#ifndef CFG_DX3_GSHIFT
  #ifdef CFG_PUB_DX3_GSHIFT
    #define CFG_DX3_GSHIFT   CFG_PUB_DX3_GSHIFT
  #else
    #define CFG_DX3_GSHIFT   (0)
  #endif
#endif
#ifndef CFG_DX3_PLLPD
  #ifdef CFG_PUB_DX3_PLLPD
    #define CFG_DX3_PLLPD   CFG_PUB_DX3_PLLPD
  #else
    #define CFG_DX3_PLLPD   (0)
  #endif
#endif
#ifndef CFG_DX3_PLLRST
  #ifdef CFG_PUB_DX3_PLLRST
    #define CFG_DX3_PLLRST   CFG_PUB_DX3_PLLRST
  #else
    #define CFG_DX3_PLLRST   (0)
  #endif
#endif
#ifndef CFG_DX3_RTTOAL
  #ifdef CFG_PUB_DX3_RTTOAL
    #define CFG_DX3_RTTOAL   CFG_PUB_DX3_RTTOAL
  #else
    #define CFG_DX3_RTTOAL   (0)
  #endif
#endif
#ifndef CFG_DX3_RTTOH
  #ifdef CFG_PUB_DX3_RTTOH
    #define CFG_DX3_RTTOH   CFG_PUB_DX3_RTTOH
  #else
    #define CFG_DX3_RTTOH   (0x1)
  #endif
#endif
#undef CFG_DX3_DQRTT
#undef CFG_DX3_DQSRTT
#if CONFIG_READ_ODT == 0 || CONFIG_DYNAMIC_READ_ODT == 0
  #define CFG_DX3_DQRTT    (0)
  #define CFG_DX3_DQSRTT   (0)
#else
  #define CFG_DX3_DQRTT    (1)
  #define CFG_DX3_DQSRTT   (1)
#endif
#ifndef CFG_DX3_DSEN
  #ifdef CFG_PUB_DX3_DSEN
    #define CFG_DX3_DSEN   CFG_PUB_DX3_DSEN
  #else
    #define CFG_DX3_DSEN   (0x1)
  #endif
#endif
#ifndef CFG_DX3_DQSRPD
  #ifdef CFG_PUB_DX3_DQSRPD
    #define CFG_DX3_DQSRPD   CFG_PUB_DX3_DQSRPD
  #else
    #define CFG_DX3_DQSRPD   (0)
  #endif
#endif
#ifndef CFG_DX3_DXPDR
  #ifdef CFG_PUB_DX3_DXPDR
    #define CFG_DX3_DXPDR   CFG_PUB_DX3_DXPDR
  #else
    #define CFG_DX3_DXPDR   (0)
  #endif
#endif
#ifndef CFG_DX3_DXPDD
  #ifdef CFG_PUB_DX3_DXPDD
    #define CFG_DX3_DXPDD   CFG_PUB_DX3_DXPDD
  #else
    #define CFG_DX3_DXPDD   (0)
  #endif
#endif
#ifndef CFG_DX3_DXIOM
  #ifdef CFG_PUB_DX3_DXIOM
    #define CFG_DX3_DXIOM   CFG_PUB_DX3_DXIOM
  #else
    #define CFG_DX3_DXIOM   (0)
  #endif
#endif
#ifndef CFG_DX3_DQODT
  #ifdef CFG_PUB_DX3_DQODT
    #define CFG_DX3_DQODT   CFG_PUB_DX3_DQODT
  #else
    #define CFG_DX3_DQODT   (0)
  #endif
#endif
#ifndef CFG_DX3_DQSODT
  #ifdef CFG_PUB_DX3_DQSODT
    #define CFG_DX3_DQSODT   CFG_PUB_DX3_DQSODT
  #else
    #define CFG_DX3_DQSODT   (0)
  #endif
#endif
#ifndef CFG_DX3_DXEN
  #ifdef CFG_PUB_DX3_DXEN
    #define CFG_DX3_DXEN   CFG_PUB_DX3_DXEN
  #else
    #define CFG_DX3_DXEN   (1) /* TODO: enabled/disabled */
  #endif
#endif



/*************************** DDR 0 ****************************/

#ifndef CFG_0_FIXED_WRITE_AS_BL8
  #ifdef CFG_FIXED_WRITE_AS_BL8
    #define CFG_0_FIXED_WRITE_AS_BL8   CFG_FIXED_WRITE_AS_BL8
  #else
    #define CFG_0_FIXED_WRITE_AS_BL8   (0)
  #endif
#endif

#ifndef CFG_0_FIXED_READ_AS_BL8
  #ifdef CFG_FIXED_READ_AS_BL8
    #define CFG_0_FIXED_READ_AS_BL8   CFG_FIXED_READ_AS_BL8
  #else
    #define CFG_0_FIXED_READ_AS_BL8   (0)
  #endif
#endif

#ifndef CFG_0_SLAVE_CORE_SYNC_EN
  #ifdef CFG_SLAVE_CORE_SYNC_EN
    #define CFG_0_SLAVE_CORE_SYNC_EN   CFG_SLAVE_CORE_SYNC_EN
  #else
    #define CFG_0_SLAVE_CORE_SYNC_EN   (CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1)
  #endif
#endif

#ifndef CFG_0_MCI_IOPHY_UPDATE_WAIT_ACK
  #ifdef CFG_MCI_IOPHY_UPDATE_WAIT_ACK
    #define CFG_0_MCI_IOPHY_UPDATE_WAIT_ACK   CFG_MCI_IOPHY_UPDATE_WAIT_ACK
  #else
    #define CFG_0_MCI_IOPHY_UPDATE_WAIT_ACK   (0)
  #endif
#endif

#ifndef CFG_0_MCI_IOPHY_UPDATE_CNTR
  #ifdef CFG_MCI_IOPHY_UPDATE_CNTR
    #define CFG_0_MCI_IOPHY_UPDATE_CNTR   CFG_MCI_IOPHY_UPDATE_CNTR
  #else
    #define CFG_0_MCI_IOPHY_UPDATE_CNTR   (0)
  #endif
#endif

/*
 * Bypass PUB
 * 0: through PUB
 * 1: bypass PUB
 */
#ifndef CFG_0_BYP_PUB_EN
  #ifdef CFG_BYP_PUB_EN
    #define CFG_0_BYP_PUB_EN   CFG_BYP_PUB_EN
  #else
    #define CFG_0_BYP_PUB_EN   (1)
  #endif
#endif


#ifndef CONFIG_0_DDR_TYPE
  #ifdef CONFIG_DDR_TYPE
    #define CONFIG_0_DDR_TYPE   CONFIG_DDR_TYPE
  #else
    #define CONFIG_0_DDR_TYPE   (DDR3)
  #endif
#endif
#undef CFG_0_DDR3_ENABLE
#define CFG_0_DDR3_ENABLE   CONFIG_0_DDR_TYPE

#undef CFG_0_DYNAMIC_ODT_EN
#if ((defined(CONFIG_0_WRITE_ODT) && CONFIG_0_WRITE_ODT != 0) || (defined(CONFIG_WRITE_ODT) && CONFIG_WRITE_ODT != 0))
  #ifndef CONFIG_0_DYNAMIC_WRITE_ODT
    #ifdef CONFIG_DYNAMIC_WRITE_ODT
      #define CFG_0_DYNAMIC_ODT_EN   0//CONFIG_DYNAMIC_WRITE_ODT
    #else
        #define CONFIG_DYNAMIC_WRITE_ODT   (1)
        #define CFG_0_DYNAMIC_ODT_EN   (1)
    #endif
  #else
    #define CFG_0_DYNAMIC_ODT_EN   CONFIG_0_DYNAMIC_WRITE_ODT
  #endif
#else
  #define CFG_0_DYNAMIC_ODT_EN   (0)
#endif

#ifndef CFG_0_PLL_TEST_SEL
  #ifdef CFG_PLL_TEST_SEL
    #define CFG_0_PLL_TEST_SEL   CFG_PLL_TEST_SEL
  #else
    #define CFG_0_PLL_TEST_SEL   (0)
  #endif
#endif

#ifndef CFG_0_MCI_IOPHY_UPDATE_EN
  #ifdef CFG_MCI_IOPHY_UPDATE_EN
    #define CFG_0_MCI_IOPHY_UPDATE_EN   CFG_MCI_IOPHY_UPDATE_EN
  #else
    #define CFG_0_MCI_IOPHY_UPDATE_EN   (0)
  #endif
#endif

#ifndef CFG_0_FAST_SETTING_EN_SHIFT
  #ifdef CFG_FAST_SETTING_EN_SHIFT
    #define CFG_0_FAST_SETTING_EN_SHIFT   CFG_FAST_SETTING_EN_SHIFT
  #else
    #define CFG_0_FAST_SETTING_EN_SHIFT   (0)
  #endif
#endif

#ifndef CFG_0_PWR_ON
  #ifdef CFG_PWR_ON
    #define CFG_0_PWR_ON   CFG_PWR_ON
  #else
    #define CFG_0_PWR_ON   (0)
  #endif
#endif

#ifndef CFG_0_EN
  #ifdef CFG_EN
    #define CFG_0_EN   CFG_EN
  #else
    #define CFG_0_EN   (0)
  #endif
#endif


#ifndef CFG_0_UDQ_CNT
  #ifdef CFG_UDQ_CNT
    #define CFG_0_UDQ_CNT   CFG_UDQ_CNT
  #else
    #define CFG_0_UDQ_CNT   (0)
  #endif
#endif

#ifndef CFG_0_LDQ_CNT
  #ifdef CFG_LDQ_CNT
    #define CFG_0_LDQ_CNT   CFG_LDQ_CNT
  #else
    #define CFG_0_LDQ_CNT   (0)
  #endif
#endif

#ifndef CFG_0_UDQS_POL
  #ifdef CFG_UDQS_POL
    #define CFG_0_UDQS_POL   CFG_UDQS_POL
  #else
    #define CFG_0_UDQS_POL   (0)
  #endif
#endif

#ifndef CFG_0_LDQS_POL
  #ifdef CFG_LDQS_POL
    #define CFG_0_LDQS_POL   CFG_LDQS_POL
  #else
    #define CFG_0_LDQS_POL   (0)
  #endif
#endif

#if  CONFIG_DDR_PORT_0 == 1 \
  && (  (CONFIG_DDR_PORT_1 == 1 && CONFIG_SYSTEM_DDR == 0) \
     || CONFIG_DDR_PORT_1 == 0)
  #define CFG_0_BASE_ADDR   (0x00000000)
#elif CONFIG_DDR_PORT_1 == 1 \
   && (  (CONFIG_DDR_PORT_0 == 1 && CONFIG_SYSTEM_DDR == 1) \
      || CONFIG_DDR_PORT_0 == 0)
  #define CFG_0_BASE_ADDR   (0x40000000)
#endif

/*
 * CFG_BANK_NUM
 * 1: 4 banks
 * 2: 8 banks
 */
#ifndef CFG_0_BANK_NUM
  #ifdef CFG_BANK_NUM
    #define CFG_0_BANK_NUM  CFG_BANK_NUM
  #else
    #define CFG_0_BANK_NUM    (2)
  #endif
#endif

#ifndef CFG_0_ROW_WIDTH
  #ifdef CFG_ROW_WIDTH
    #define CFG_0_ROW_WIDTH  CFG_ROW_WIDTH
  #else
    #define CFG_0_ROW_WIDTH   (14)
  #endif
#endif

#ifndef CFG_0_COL_WIDTH
  #ifdef CFG_COL_WIDTH
    #define CFG_0_COL_WIDTH  CFG_COL_WIDTH
  #else
    #define CFG_0_COL_WIDTH   (10)
  #endif
#endif

#if CONFIG_DDR_TYPE == DDR3

  #ifndef CFG_0_MR0_PPD
    #ifdef CFG_MR0_PPD
      #define CFG_0_MR0_PPD   CFG_MR0_PPD
    #else
      #define CFG_0_MR0_PPD   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR0_T_WR
    #ifdef CFG_MR0_T_WR
      #define CFG_0_MR0_T_WR   CFG_MR0_T_WR
    #else
      #define CFG_0_MR0_T_WR   (0x6)
    #endif
  #endif
  #if CFG_0_MR0_T_WR >= 5 && CFG_0_MR0_T_WR <= 8
    #define CFG_0_MR0_WR   (CFG_0_MR0_T_WR - 4)
  #elif CFG_0_MR0_T_WR == 10
    #define CFG_0_MR0_WR   (5)
  #elif CFG_0_MR0_T_WR == 12
    #define CFG_0_MR0_WR   (6)
  #else
    #error Invalid CFG_0_MR0_T_WR !
  #endif

  #ifndef CFG_0_MR0_DLL
    #ifdef CFG_MR0_DLL
      #define CFG_0_MR0_DLL   CFG_MR0_DLL
    #else
      #define CFG_0_MR0_DLL   (0x1)
    #endif
  #endif

  #ifndef CFG_0_MR0_TM
    #ifdef CFG_MR0_TM
      #define CFG_0_MR0_TM   CFG_MR0_TM
    #else
      #define CFG_0_MR0_TM   (0x0)
    #endif
  #endif

  #define TO_MR0_CL(v)   (((v) - 4) << 1)
  #if !( defined (CFG_0_CL) && defined (CONFIG_DYNAMIC_READ_ODT) )
    #ifdef CFG_CL
      #define CFG_0_MR0_CL   TO_MR0_CL(CFG_CL)
    #else
      #define CFG_0_MR0_CL   TO_MR0_CL(6)
    #endif
  #else
    #define CFG_0_MR0_CL   TO_MR0_CL(CFG_0_CL)
  #endif

  #ifndef CFG_0_MR0_RBT
    #ifdef CFG_MR0_RBT
      #define CFG_0_MR0_RBT   CFG_MR0_RBT
    #else
      #define CFG_0_MR0_RBT   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR0_BL
    #ifdef CFG_MR0_BL
      #define CFG_0_MR0_BL   CFG_MR0_BL
    #else
      #define CFG_0_MR0_BL   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_QOFF
    #ifdef CFG_MR1_QOFF
      #define CFG_0_MR1_QOFF   CFG_MR1_QOFF
    #else
      #define CFG_0_MR1_QOFF   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_TDQS
    #ifdef CFG_MR1_TDQS
      #define CFG_0_MR1_TDQS   CFG_MR1_TDQS
    #else
      #define CFG_0_MR1_TDQS   (0x0)
    #endif
  #endif

  /* MR1_RTT_NORM */
  #ifndef CONFIG_0_WRITE_ODT
    #ifdef CONFIG_WRITE_ODT
      #define CONFIG_0_WRITE_ODT   CONFIG_WRITE_ODT
    #else
      #define CONFIG_0_WRITE_ODT   (0x0)
    #endif
  #endif
  #if CONFIG_DYNAMIC_WRITE_ODT == 1
    #define CFG_0_MR1_RTT_NORM   (0x0)
  #elif CONFIG_0_WRITE_ODT == 60
    #define CFG_0_MR1_RTT_NORM   (0x1)
  #elif CONFIG_0_WRITE_ODT == 120
    #define CFG_0_MR1_RTT_NORM   (0x2)
  #elif CONFIG_0_WRITE_ODT == 40
    #define CFG_0_MR1_RTT_NORM   (0x3)
  #elif CONFIG_0_WRITE_ODT == 20
    #define CFG_0_MR1_RTT_NORM   (0x4)
  #elif CONFIG_0_WRITE_ODT == 30
    #define CFG_0_MR1_RTT_NORM   (0x5)
  #else
    #define CFG_0_MR1_RTT_NORM   (0x0)
  #endif

  #ifndef CFG_0_MR1_LEVEL
    #ifdef CFG_MR1_LEVEL
      #define CFG_0_MR1_LEVEL   CFG_MR1_LEVEL
    #else
      #define CFG_0_MR1_LEVEL   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_DIC
    #ifdef CFG_MR1_DIC
      #define CFG_0_MR1_DIC   CFG_MR1_DIC
    #else
      #define CFG_0_MR1_DIC   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_AL
    #ifdef CFG_MR1_AL
      #define CFG_0_MR1_AL   CFG_MR1_AL
    #else
      #define CFG_0_MR1_AL   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_DLL
    #ifdef CFG_MR1_DLL
      #define CFG_0_MR1_DLL   CFG_MR1_DLL
    #else
      #define CFG_0_MR1_DLL   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR2_RTT_WR
    #ifdef CFG_MR2_RTT_WR
      #define CFG_0_MR2_RTT_WR   CFG_MR2_RTT_WR
    #else
      #ifdef CONFIG_DYNAMIC_WRITE_ODT
	#if CONFIG_WRITE_ODT == 40 || CONFIG_WRITE_ODT == 60 
          #define CFG_0_MR2_RTT_WR (0x1)
	#elif CONFIG_WRITE_ODT == 120
	  #define CFG_0_MR2_RTT_WR (0x2)
	#endif
      #else
        #define CFG_0_MR2_RTT_WR   (0x0)
      #endif
    #endif
  #endif

  #ifndef CFG_0_MR2_SRT
    #ifdef CFG_MR2_SRT
      #define CFG_0_MR2_SRT   CFG_MR2_SRT
    #else
      #define CFG_0_MR2_SRT   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR2_ASR
    #ifdef CFG_MR2_ASR
      #define CFG_0_MR2_ASR   CFG_MR2_ASR
    #else
      #define CFG_0_MR2_ASR   (0x0)
    #endif
  #endif

  #define TO_MR2_CWL(v)   ((v) - 5)
  #ifndef CFG_0_CWL
    #ifdef CFG_CWL
      #define CFG_0_MR2_CWL   TO_MR2_CWL(CFG_CWL)
    #else
      #define CFG_0_MR2_CWL   TO_MR2_CWL(5)
    #endif
  #else
    #define CFG_0_MR2_CWL   TO_MR2_CWL(CFG_0_CWL)
  #endif

  #ifndef CFG_0_MR2_PASR
    #ifdef CFG_MR2_PASR
      #define CFG_0_MR2_PASR   CFG_MR2_PASR
    #else
      #define CFG_0_MR2_PASR   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR3_MPR
    #ifdef CFG_MR3_MPR
      #define CFG_0_MR3_MPR   CFG_MR3_MPR
    #else
      #define CFG_0_MR3_MPR   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR3_MPR_LOC
    #ifdef CFG_MR3_MPR_LOC
      #define CFG_0_MR3_MPR_LOC   CFG_MR3_MPR_LOC
    #else
      #define CFG_0_MR3_MPR_LOC   (0x0)
    #endif
  #endif

#elif CONFIG_DDR_TYPE == DDR2

  #ifndef CFG_0_MR0_PD
    #ifdef CFG_MR0_PD
      #define CFG_0_MR0_PD   CFG_MR0_PD
    #else
      #define CFG_0_MR0_PD   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR0_T_WR
    #ifdef CFG_MR0_T_WR
      #define CFG_0_MR0_T_WR   CFG_MR0_T_WR
    #else
      #define CFG_0_MR0_T_WR   (0x7)
    #endif
  #endif
  #if CFG_0_MR0_T_WR >= 2 && CFG_0_MR0_T_WR <= 8
    #define CFG_0_MR0_WR   (CFG_0_MR0_T_WR - 1)
  #else
    #error Invalid CFG_0_MR0_T_WR !
  #endif

  #ifndef CFG_0_MR0_DLL
    #ifdef CFG_MR0_DLL
      #define CFG_0_MR0_DLL   CFG_MR0_DLL
    #else
      #define CFG_0_MR0_DLL   (0x1)
    #endif
  #endif

  #ifndef CFG_0_MR0_TM
    #ifdef CFG_MR0_TM
      #define CFG_0_MR0_TM   CFG_MR0_TM
    #else
      #define CFG_0_MR0_TM   (0x0)
    #endif
  #endif

  #if !( defined (CFG_0_CL) && defined (CONFIG_DYNAMIC_READ_ODT) )
    #ifdef CFG_CL
      #define CFG_0_MR0_CL   CFG_CL
    #else
      #define CFG_0_MR0_CL   (5)
    #endif
  #else
    #define CFG_0_MR0_CL   CFG_0_CL
  #endif

  #ifndef CFG_0_MR0_BT
    #ifdef CFG_MR0_BT
      #define CFG_0_MR0_BT   CFG_MR0_BT
    #else
      #define CFG_0_MR0_BT   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR0_BL
    #ifdef CFG_MR0_BL
      #define CFG_0_MR0_BL   CFG_MR0_BL
    #else
      #define CFG_0_MR0_BL   (0x2)
    #endif
  #endif

  #ifndef CFG_0_MR1_QOFF
    #ifdef CFG_MR1_QOFF
      #define CFG_0_MR1_QOFF   CFG_MR1_QOFF
    #else
      #define CFG_0_MR1_QOFF   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_RDQS
    #ifdef CFG_MR1_RDQS
      #define CFG_0_MR1_RDQS   CFG_MR1_RDQS
    #else
      #define CFG_0_MR1_RDQS   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_DOSN
    #ifdef CFG_MR1_DOSN
      #define CFG_0_MR1_DOSN   CFG_MR1_DOSN
    #else
      #define CFG_0_MR1_DOSN   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_OCD_PROG
    #ifdef CFG_MR1_OCD_PROG
      #define CFG_0_MR1_OCD_PROG   CFG_MR1_OCD_PROG
    #else
      #define CFG_0_MR1_OCD_PROG   (0x7)
    #endif
  #endif

  /* MR1_RTT_NORM */
  #ifndef CONFIG_0_WRITE_ODT
    #ifdef CONFIG_WRITE_ODT
      #define CONFIG_0_WRITE_ODT   CONFIG_WRITE_ODT
    #else
      #define CONFIG_0_WRITE_ODT   (0x0)
    #endif
  #endif
  #if CONFIG_0_WRITE_ODT == 75
    #define CFG_0_MR1_RTT_NORM   (0x1)
  #elif CONFIG_0_WRITE_ODT == 150
    #define CFG_0_MR1_RTT_NORM   (0x2)
  #elif CONFIG_0_WRITE_ODT == 50
    #define CFG_0_MR1_RTT_NORM   (0x3)
  #else
    #define CFG_0_MR1_RTT_NORM   (0x0)
  #endif

  #ifndef CFG_0_MR1_AL
    #ifdef CFG_MR1_AL
      #define CFG_0_MR1_AL   CFG_MR1_AL
    #else
      #define CFG_0_MR1_AL   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_DIC
    #ifdef CFG_MR1_DIC
      #define CFG_0_MR1_DIC   CFG_MR1_DIC
    #else
      #define CFG_0_MR1_DIC   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR1_DLL
    #ifdef CFG_MR1_DLL
      #define CFG_0_MR1_DLL   CFG_MR1_DLL
    #else
      #define CFG_0_MR1_DLL   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR2_SRF
    #ifdef CFG_MR2_SRF
      #define CFG_0_MR2_SRF   CFG_MR2_SRF
    #else
      #define CFG_0_MR2_SRF   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR2_DCC
    #ifdef CFG_MR2_DCC
      #define CFG_0_MR2_DCC   CFG_MR2_DCC
    #else
      #define CFG_0_MR2_DCC   (0x0)
    #endif
  #endif

  #ifndef CFG_0_MR2_PASR
    #ifdef CFG_MR2_PASR
      #define CFG_0_MR2_PASR   CFG_MR2_PASR
    #else
      #define CFG_0_MR2_PASR   (0x0)
    #endif
  #endif

#endif /* DDR2 */

#ifndef CFG_0_T_MOD
  #ifdef CFG_T_MOD
    #define CFG_0_T_MOD   CFG_T_MOD
  #else
    #define CFG_0_T_MOD   (0)
  #endif
#endif

#ifndef CFG_0_T_WTR
  #ifdef CFG_T_WTR
    #define CFG_0_T_WTR   CFG_T_WTR
  #else
    #define CFG_0_T_WTR   (4)
  #endif
#endif

#ifndef CFG_0_T_RTP
  #ifdef CFG_T_RTP
    #define CFG_0_T_RTP   CFG_T_RTP
  #else
    #define CFG_0_T_RTP   (4)
  #endif
#endif

#ifndef CFG_0_T_RCD
  #ifdef CFG_T_RCD
    #define CFG_0_T_RCD   CFG_T_RCD
  #else
    #define CFG_0_T_RCD   (7)
  #endif
#endif

#ifndef CFG_0_T_RRD
  #ifdef CFG_T_RRD
    #define CFG_0_T_RRD   CFG_T_RRD
  #else
    #define CFG_0_T_RRD   (4)
  #endif
#endif

#ifndef CFG_0_T_MRD
  #ifdef CFG_T_MRD
    #define CFG_0_T_MRD   CFG_T_MRD
  #else
    #define CFG_0_T_MRD   (4)
  #endif
#endif

#ifndef CFG_0_T_RP
  #ifdef CFG_T_RP
    #define CFG_0_T_RP   CFG_T_RP
  #else
    #define CFG_0_T_RP   (7)
  #endif
#endif

#ifndef CFG_0_T_RFC
  #ifdef CFG_T_RFC
    #define CFG_0_T_RFC   CFG_T_RFC
  #else
    #define CFG_0_T_RFC   (0x56)
  #endif
#endif

#ifndef CFG_0_T_FAW
  #ifdef CFG_T_FAW
    #define CFG_0_T_FAW   CFG_T_FAW
  #else
    #define CFG_0_T_FAW   (0)
  #endif
#endif

#ifndef CFG_0_T_RC
  #ifdef CFG_T_RC
    #define CFG_0_T_RC   CFG_T_RC
  #else
    #define CFG_0_T_RC   (0x1b)
  #endif
#endif

#ifndef CFG_0_T_RAS
  #ifdef CFG_T_RAS
    #define CFG_0_T_RAS   CFG_T_RAS
  #else
    #define CFG_0_T_RAS   (0x14)
  #endif
#endif

#ifndef CFG_0_T_IDLE
  #ifdef CFG_T_IDLE
    #define CFG_0_T_IDLE   CFG_T_IDLE
  #else
    #define CFG_0_T_IDLE   (0x5b)
  #endif
#endif

#ifndef CFG_0_T_REF
  #ifdef CONFIG_T_REF_MODE
    #define CFG_0_T_REF   xcat(CFG_T_REF_,CONFIG_T_REF_MODE)
  #elif defined(CONFIG_0_T_REF)
    #define CFG_0_T_REF   xcat(CFG_T_REF_,CONFIG_0_T_REF_MODE)
  #else
    #define CFG_0_T_REF   (0x1046)
  #endif
#endif

#ifndef CFG_0_T_DLLRST
  #ifdef CFG_T_DLLRST
    #define CFG_0_T_DLLRST   CFG_T_DLLRST
  #else
    #define CFG_0_T_DLLRST   (0x517)
  #endif
#endif

#ifndef CFG_0_T_PWRON
  #ifdef CFG_T_PWRON
    #define CFG_0_T_PWRON   CFG_T_PWRON
  #else
    #define CFG_0_T_PWRON   (0x208d6)
  #endif
#endif

#ifndef CFG_0_ZQCL_CYCLE
  #ifdef CFG_ZQCL_CYCLE
    #define CFG_0_ZQCL_CYCLE   CFG_ZQCL_CYCLE
  #else
    #define CFG_0_ZQCL_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_ZQCS_CYCLE
  #ifdef CFG_ZQCS_CYCLE
    #define CFG_0_ZQCS_CYCLE   CFG_ZQCS_CYCLE
  #else
    #define CFG_0_ZQCS_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_T_ZQ_EXTEND
  #ifdef CFG_T_ZQ_EXTEND
    #define CFG_0_T_ZQ_EXTEND   CFG_T_ZQ_EXTEND
  #else
    #define CFG_0_T_ZQ_EXTEND   (0)
  #endif
#endif

#ifndef CFG_0_ZQC_FROCE_EN
  #ifdef CFG_ZQC_FROCE_EN
    #define CFG_0_ZQC_FROCE_EN   CFG_ZQC_FROCE_EN
  #else
    #define CFG_0_ZQC_FROCE_EN   (0)
  #endif
#endif

#ifndef CFG_0_ZQCL_CYCLE_EN
  #ifdef CFG_ZQCL_CYCLE_EN
    #define CFG_0_ZQCL_CYCLE_EN   CFG_ZQCL_CYCLE_EN
  #else
    #define CFG_0_ZQCL_CYCLE_EN   (0)
  #endif
#endif

#ifndef CFG_0_ZQCS_CYCLE_EN
  #ifdef CFG_ZQCS_CYCLE_EN
    #define CFG_0_ZQCS_CYCLE_EN   CFG_ZQCS_CYCLE_EN
  #else
    #define CFG_0_ZQCS_CYCLE_EN   (0)
  #endif
#endif

#ifndef CFG_0_PRECHARGED_DLL_OFF_PD_CYCLE
  #ifdef CFG_PRECHARGED_DLL_OFF_PD_CYCLE
    #define CFG_0_PRECHARGED_DLL_OFF_PD_CYCLE   CFG_PRECHARGED_DLL_OFF_PD_CYCLE
  #else
    #define CFG_0_PRECHARGED_DLL_OFF_PD_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_PRECHARGED_DLL_ON_PD_CYCLE
  #ifdef CFG_PRECHARGED_DLL_ON_PD_CYCLE
    #define CFG_0_PRECHARGED_DLL_ON_PD_CYCLE   CFG_PRECHARGED_DLL_ON_PD_CYCLE
  #else
    #define CFG_0_PRECHARGED_DLL_ON_PD_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_ACTIVE_PD_CYCLE
  #ifdef CFG_ACTIVE_PD_CYCLE
    #define CFG_0_ACTIVE_PD_CYCLE   CFG_ACTIVE_PD_CYCLE
  #else
    #define CFG_0_ACTIVE_PD_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_PRECHARGED_DLL_OFF_PD_EN
  #ifdef CFG_PRECHARGED_DLL_OFF_PD_EN
    #define CFG_0_PRECHARGED_DLL_OFF_PD_EN   CFG_PRECHARGED_DLL_OFF_PD_EN
  #else
    #define CFG_0_PRECHARGED_DLL_OFF_PD_EN   (0)
  #endif
#endif

#ifndef CFG_0_PRECHARGED_DLL_ON_PD_EN
  #ifdef CFG_PRECHARGED_DLL_ON_PD_EN
    #define CFG_0_PRECHARGED_DLL_ON_PD_EN   CFG_PRECHARGED_DLL_ON_PD_EN
  #else
    #define CFG_0_PRECHARGED_DLL_ON_PD_EN   (0)
  #endif
#endif

#ifndef CFG_0_ACTIVE_PD_EN
  #ifdef CFG_ACTIVE_PD_EN
    #define CFG_0_ACTIVE_PD_EN   CFG_ACTIVE_PD_EN
  #else
    #define CFG_0_ACTIVE_PD_EN   (0)
  #endif
#endif

#ifndef CFG_0_DLL_OFF_SRF_CYCLE
  #ifdef CFG_DLL_OFF_SRF_CYCLE
    #define CFG_0_DLL_OFF_SRF_CYCLE   CFG_DLL_OFF_SRF_CYCLE
  #else
    #define CFG_0_DLL_OFF_SRF_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_DLL_ON_SRF_CYCLE
  #ifdef CFG_DLL_ON_SRF_CYCLE
    #define CFG_0_DLL_ON_SRF_CYCLE   CFG_DLL_ON_SRF_CYCLE
  #else
    #define CFG_0_DLL_ON_SRF_CYCLE   (0)
  #endif
#endif

#ifndef CFG_0_DLL_OFF_SRF_EN
  #ifdef CFG_DLL_OFF_SRF_EN
    #define CFG_0_DLL_OFF_SRF_EN   CFG_DLL_OFF_SRF_EN
  #else
    #define CFG_0_DLL_OFF_SRF_EN   (0)
  #endif
#endif

#ifndef CFG_0_DLL_ON_SRF_EN
  #ifdef CFG_DLL_ON_SRF_EN
    #define CFG_0_DLL_ON_SRF_EN   CFG_DLL_ON_SRF_EN
  #else
    #define CFG_0_DLL_ON_SRF_EN   (0)
  #endif
#endif

#ifndef CFG_0_T_XPDLL
  #ifdef CFG_T_XPDLL
    #define CFG_0_T_XPDLL   CFG_T_XPDLL
  #else
    #define CFG_0_T_XPDLL   (0)
  #endif
#endif

#ifndef CFG_0_T_DLLK_EXTEND
  #ifdef CFG_T_DLLK_EXTEND
    #define CFG_0_T_DLLK_EXTEND   CFG_T_DLLK_EXTEND
  #else
    #define CFG_0_T_DLLK_EXTEND   (0)
  #endif
#endif

#ifndef CFG_0_T_XP
  #ifdef CFG_T_XP
    #define CFG_0_T_XP   CFG_T_XP
  #else
    #define CFG_0_T_XP   (0)
  #endif
#endif

#ifndef CFG_0_T_PD
  #ifdef CFG_T_PD
    #define CFG_0_T_PD   CFG_T_PD
  #else
    #define CFG_0_T_PD   (0)
  #endif
#endif

#ifndef CFG_0_T_XS
  #ifdef CFG_T_XS
    #define CFG_0_T_XS   CFG_T_XS
  #else
    #define CFG_0_T_XS   (0)
  #endif
#endif

#ifndef CFG_0_T_CKSREX
  #ifdef CFG_T_CKSREX
    #define CFG_0_T_CKSREX   CFG_T_CKSREX
  #else
    #define CFG_0_T_CKSREX   (0)
  #endif
#endif

#ifndef CFG_0_T_CKESR
  #ifdef CFG_T_CKESR
    #define CFG_0_T_CKESR   CFG_T_CKESR
  #else
    #define CFG_0_T_CKESR   (0)
  #endif
#endif

#ifndef CFG_0_DYNAMIC_DQ_DQS_RCV_EN
  #ifdef CFG_DYNAMIC_DQ_DQS_RCV_EN
    #define CFG_0_DYNAMIC_DQ_DQS_RCV_EN   CFG_DYNAMIC_DQ_DQS_RCV_EN
  #else
    #define CFG_0_DYNAMIC_DQ_DQS_RCV_EN   (1)
  #endif
#endif

#undef CFG_0_DYNAMIC_DM_DQ_DQS_DRV_EN
#if CONFIG_READ_ODT == 0
    #define CFG_0_DYNAMIC_DM_DQ_DQS_DRV_EN   (1)
#else
    #define CFG_0_DYNAMIC_DM_DQ_DQS_DRV_EN   (0)
#endif

#ifndef CFG_0_DYNAMIC_ADDR_DRV_EN
  #ifdef CFG_DYNAMIC_ADDR_DRV_EN
    #define CFG_0_DYNAMIC_ADDR_DRV_EN   CFG_DYNAMIC_ADDR_DRV_EN
  #else
    #define CFG_0_DYNAMIC_ADDR_DRV_EN   (1)
  #endif
#endif

#ifndef CFG_0_DYNAMIC_CMD_DRV_EN
  #ifdef CFG_DYNAMIC_CMD_DRV_EN
    #define CFG_0_DYNAMIC_CMD_DRV_EN   CFG_DYNAMIC_CMD_DRV_EN
  #else
    #define CFG_0_DYNAMIC_CMD_DRV_EN   (1)
  #endif
#endif

#ifndef CFG_0_DYNAMIC_CLK_DRV_EN
  #ifdef CFG_DYNAMIC_CLK_DRV_EN
    #define CFG_0_DYNAMIC_CLK_DRV_EN   CFG_DYNAMIC_CLK_DRV_EN
  #else
    #define CFG_0_DYNAMIC_CLK_DRV_EN   (1)
  #endif
#endif

#ifndef CFG_0_NOP_AC_OE_DISABLE
  #ifdef CFG_NOP_AC_OE_DISABLE
    #define CFG_0_NOP_AC_OE_DISABLE   CFG_NOP_AC_OE_DISABLE
  #else
    #define CFG_0_NOP_AC_OE_DISABLE   (1)
  #endif
#endif

#ifndef CFG_0_DYNAMIC_ADDR_OE_EN
  #ifdef CFG_DYNAMIC_ADDR_OE_EN
    #define CFG_0_DYNAMIC_ADDR_OE_EN   CFG_DYNAMIC_ADDR_OE_EN
  #else
    #define CFG_0_DYNAMIC_ADDR_OE_EN   (1)
  #endif
#endif

#ifndef CFG_0_DYNAMIC_CMD_OE_EN
  #ifdef CFG_DYNAMIC_CMD_OE_EN
    #define CFG_0_DYNAMIC_CMD_OE_EN   CFG_DYNAMIC_CMD_OE_EN
  #else
    #define CFG_0_DYNAMIC_CMD_OE_EN   (1)
  #endif
#endif

#ifndef CFG_0_DYNAMIC_CLK_OE_EN
  #ifdef CFG_DYNAMIC_CLK_OE_EN
    #define CFG_0_DYNAMIC_CLK_OE_EN   CFG_DYNAMIC_CLK_OE_EN
  #else
    #define CFG_0_DYNAMIC_CLK_OE_EN   (1)
  #endif
#endif

#ifndef CFG_0_CKE_BIT_EN
  #ifdef CFG_CKE_BIT_EN
    #define CFG_0_CKE_BIT_EN   CFG_CKE_BIT_EN
  #else
    #define CFG_0_CKE_BIT_EN   (1)
  #endif
#endif

#ifndef CFG_0_ODT_BIT_EN
  #ifdef CFG_ODT_BIT_EN
    #define CFG_0_ODT_BIT_EN   CFG_ODT_BIT_EN
  #else
    #define CFG_0_ODT_BIT_EN   (1)
  #endif
#endif

#ifndef CFG_0_CS_N_BIT_EN
  #ifdef CFG_CS_N_BIT_EN
    #define CFG_0_CS_N_BIT_EN   CFG_CS_N_BIT_EN
  #else
    #define CFG_0_CS_N_BIT_EN   (1)
  #endif
#endif

#ifndef CFG_0_ADDR_BIT_EN
  #ifdef CFG_ADDR_BIT_EN
    #define CFG_0_ADDR_BIT_EN   CFG_ADDR_BIT_EN
  #else
    #define CFG_0_ADDR_BIT_EN   (0xffff)
  #endif
#endif

#ifndef CFG_0_DFI_LP_WAKEUP
  #ifdef CFG_DFI_LP_WAKEUP
    #define CFG_0_DFI_LP_WAKEUP   CFG_DFI_LP_WAKEUP
  #else
    #define CFG_0_DFI_LP_WAKEUP   (0)
  #endif
#endif

#ifndef CFG_0_DFI_LP_ACK
  #ifdef CFG_DFI_LP_ACK
    #define CFG_0_DFI_LP_ACK   CFG_DFI_LP_ACK
  #else
    #define CFG_0_DFI_LP_ACK   (0)
  #endif
#endif

#ifndef CFG_0_DFI_LP_REQ
  #ifdef CFG_DFI_LP_REQ
    #define CFG_0_DFI_LP_REQ   CFG_DFI_LP_REQ
  #else
    #define CFG_0_DFI_LP_REQ   (0)
  #endif
#endif

#ifndef CFG_0_DFI_INIT_COMPLETE
  #ifdef CFG_DFI_INIT_COMPLETE
    #define CFG_0_DFI_INIT_COMPLETE   CFG_DFI_INIT_COMPLETE
  #else
    #define CFG_0_DFI_INIT_COMPLETE   (0)
  #endif
#endif

#ifndef CFG_0_DFI_DRAM_CLK_DISABLE
  #ifdef CFG_DFI_DRAM_CLK_DISABLE
    #define CFG_0_DFI_DRAM_CLK_DISABLE   CFG_DFI_DRAM_CLK_DISABLE
  #else
    #define CFG_0_DFI_DRAM_CLK_DISABLE   (0)
  #endif
#endif

#ifndef CFG_0_DFI_DATA_HALFWORDLANE_DISABLE
  #ifdef CFG_DFI_DATA_HALFWORDLANE_DISABLE
    #define CFG_0_DFI_DATA_HALFWORDLANE_DISABLE   CFG_DFI_DATA_HALFWORDLANE_DISABLE
  #else
    #define CFG_0_DFI_DATA_HALFWORDLANE_DISABLE   (0)
  #endif
#endif

#ifndef CFG_0_DFI_INIT_START
  #ifdef CFG_DFI_INIT_START
    #define CFG_0_DFI_INIT_START   CFG_DFI_INIT_START
  #else
    #define CFG_0_DFI_INIT_START   (0)
  #endif
#endif




/*************************** DDR 1 ****************************/

#ifndef CFG_1_FIXED_WRITE_AS_BL8
  #ifdef CFG_FIXED_WRITE_AS_BL8
    #define CFG_1_FIXED_WRITE_AS_BL8   CFG_FIXED_WRITE_AS_BL8
  #else
    #define CFG_1_FIXED_WRITE_AS_BL8   (0)
  #endif
#endif

#ifndef CFG_1_FIXED_READ_AS_BL8
  #ifdef CFG_FIXED_READ_AS_BL8
    #define CFG_1_FIXED_READ_AS_BL8   CFG_FIXED_READ_AS_BL8
  #else
    #define CFG_1_FIXED_READ_AS_BL8   (0)
  #endif
#endif

#ifndef CFG_1_SLAVE_CORE_SYNC_EN
  #ifdef CFG_SLAVE_CORE_SYNC_EN
    #define CFG_1_SLAVE_CORE_SYNC_EN   CFG_SLAVE_CORE_SYNC_EN
  #else
    #define CFG_1_SLAVE_CORE_SYNC_EN   (CONFIG_DDR_PORT_0 && CONFIG_DDR_PORT_1)
  #endif
#endif

#ifndef CFG_1_MCI_IOPHY_UPDATE_WAIT_ACK
  #ifdef CFG_MCI_IOPHY_UPDATE_WAIT_ACK
    #define CFG_1_MCI_IOPHY_UPDATE_WAIT_ACK   CFG_MCI_IOPHY_UPDATE_WAIT_ACK
  #else
    #define CFG_1_MCI_IOPHY_UPDATE_WAIT_ACK   (0)
  #endif
#endif

#ifndef CFG_1_MCI_IOPHY_UPDATE_CNTR
  #ifdef CFG_MCI_IOPHY_UPDATE_CNTR
    #define CFG_1_MCI_IOPHY_UPDATE_CNTR   CFG_MCI_IOPHY_UPDATE_CNTR
  #else
    #define CFG_1_MCI_IOPHY_UPDATE_CNTR   (0)
  #endif
#endif

/*
 * Bypass PUB
 * 0: through PUB
 * 1: bypass PUB
 */
#ifndef CFG_1_BYP_PUB_EN
  #ifdef CFG_BYP_PUB_EN
    #define CFG_1_BYP_PUB_EN   CFG_BYP_PUB_EN
  #else
    #define CFG_1_BYP_PUB_EN   (1)
  #endif
#endif


#ifndef CFG_1_DDR3_ENABLE
  #ifdef CONFIG_DDR_TYPE
    #define CFG_1_DDR3_ENABLE   CONFIG_DDR_TYPE
  #else
    #define CFG_1_DDR3_ENABLE   (DDR3)
  #endif
#endif

#undef CFG_1_DYNAMIC_ODT_EN
#if ((defined(CONFIG_1_WRITE_ODT) && CONFIG_1_WRITE_ODT != 0) || (defined(CONFIG_WRITE_ODT) && CONFIG_WRITE_ODT != 0))
  #ifndef CONFIG_1_DYNAMIC_WRITE_ODT
    #ifdef CONFIG_DYNAMIC_WRITE_ODT
      #define CFG_1_DYNAMIC_ODT_EN   0//CONFIG_DYNAMIC_WRITE_ODT
    #else
        #define CONFIG_DYNAMIC_WRITE_ODT   (1)
        #define CFG_1_DYNAMIC_ODT_EN   (1)
    #endif
  #else
    #define CFG_1_DYNAMIC_ODT_EN   CONFIG_1_DYNAMIC_WRITE_ODT
  #endif
#else
  #define CFG_1_DYNAMIC_ODT_EN   (0)
#endif

#ifndef CFG_1_PLL_TEST_SEL
  #ifdef CFG_PLL_TEST_SEL
    #define CFG_1_PLL_TEST_SEL   CFG_PLL_TEST_SEL
  #else
    #define CFG_1_PLL_TEST_SEL   (0)
  #endif
#endif

#ifndef CFG_1_MCI_IOPHY_UPDATE_EN
  #ifdef CFG_MCI_IOPHY_UPDATE_EN
    #define CFG_1_MCI_IOPHY_UPDATE_EN   CFG_MCI_IOPHY_UPDATE_EN
  #else
    #define CFG_1_MCI_IOPHY_UPDATE_EN   (0)
  #endif
#endif

#ifndef CFG_1_FAST_SETTING_EN_SHIFT
  #ifdef CFG_FAST_SETTING_EN_SHIFT
    #define CFG_1_FAST_SETTING_EN_SHIFT   CFG_FAST_SETTING_EN_SHIFT
  #else
    #define CFG_1_FAST_SETTING_EN_SHIFT   (0)
  #endif
#endif

#ifndef CFG_1_PWR_ON
  #ifdef CFG_PWR_ON
    #define CFG_1_PWR_ON   CFG_PWR_ON
  #else
    #define CFG_1_PWR_ON   (0)
  #endif
#endif

#ifndef CFG_1_EN
  #ifdef CFG_EN
    #define CFG_1_EN   CFG_EN
  #else
    #define CFG_1_EN   (0)
  #endif
#endif


#ifndef CFG_1_UDQ_CNT
  #ifdef CFG_UDQ_CNT
    #define CFG_1_UDQ_CNT   CFG_UDQ_CNT
  #else
    #define CFG_1_UDQ_CNT   (0)
  #endif
#endif

#ifndef CFG_1_LDQ_CNT
  #ifdef CFG_LDQ_CNT
    #define CFG_1_LDQ_CNT   CFG_LDQ_CNT
  #else
    #define CFG_1_LDQ_CNT   (0)
  #endif
#endif

#ifndef CFG_1_UDQS_POL
  #ifdef CFG_UDQS_POL
    #define CFG_1_UDQS_POL   CFG_UDQS_POL
  #else
    #define CFG_1_UDQS_POL   (0)
  #endif
#endif

#ifndef CFG_1_LDQS_POL
  #ifdef CFG_LDQS_POL
    #define CFG_1_LDQS_POL   CFG_LDQS_POL
  #else
    #define CFG_1_LDQS_POL   (0)
  #endif
#endif

#if  CONFIG_DDR_PORT_0 == 1 \
  && (  (CONFIG_DDR_PORT_1 == 1 && CONFIG_SYSTEM_DDR == 0) \
     || CONFIG_DDR_PORT_1 == 0)
  #define CFG_1_BASE_ADDR   (0x40000000)
#elif CONFIG_DDR_PORT_1 == 1 \
   && (  (CONFIG_DDR_PORT_0 == 1 && CONFIG_SYSTEM_DDR == 1) \
      || CONFIG_DDR_PORT_0 == 0)
  #define CFG_1_BASE_ADDR   (0x00000000)
#endif

/*
 * CFG_BANK_NUM
 * 1: 4 banks
 * 2: 8 banks
 */
#ifndef CFG_1_BANK_NUM
  #ifdef CFG_BANK_NUM
    #define CFG_1_BANK_NUM  CFG_BANK_NUM
  #else
    #define CFG_1_BANK_NUM    (2)
  #endif
#endif

#ifndef CFG_1_ROW_WIDTH
  #ifdef CFG_ROW_WIDTH
    #define CFG_1_ROW_WIDTH  CFG_ROW_WIDTH
  #else
    #define CFG_1_ROW_WIDTH   (14)
  #endif
#endif

#ifndef CFG_1_COL_WIDTH
  #ifdef CFG_COL_WIDTH
    #define CFG_1_COL_WIDTH  CFG_COL_WIDTH
  #else
    #define CFG_1_COL_WIDTH   (10)
  #endif
#endif

#if CONFIG_DDR_TYPE == DDR3

  #ifndef CFG_1_MR0_PPD
    #ifdef CFG_MR0_PPD
      #define CFG_1_MR0_PPD   CFG_MR0_PPD
    #else
      #define CFG_1_MR0_PPD   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR0_T_WR
    #ifdef CFG_MR0_T_WR
      #define CFG_1_MR0_T_WR   CFG_MR0_T_WR
    #else
      #define CFG_1_MR0_T_WR   (0x6)
    #endif
  #endif
  #if CFG_1_MR0_T_WR >= 5 && CFG_1_MR0_T_WR <= 8
    #define CFG_1_MR0_WR   (CFG_1_MR0_T_WR - 4)
  #elif CFG_1_MR0_T_WR == 10
    #define CFG_1_MR0_WR   (5)
  #elif CFG_1_MR0_T_WR == 12
    #define CFG_1_MR0_WR   (6)
  #else
    #error Invalid CFG_1_MR0_T_WR !
  #endif

  #ifndef CFG_1_MR0_DLL
    #ifdef CFG_MR0_DLL
      #define CFG_1_MR0_DLL   CFG_MR0_DLL
    #else
      #define CFG_1_MR0_DLL   (0x1)
    #endif
  #endif

  #ifndef CFG_1_MR0_TM
    #ifdef CFG_MR0_TM
      #define CFG_1_MR0_TM   CFG_MR0_TM
    #else
      #define CFG_1_MR0_TM   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR0_RBT
    #ifdef CFG_MR0_RBT
      #define CFG_1_MR0_RBT   CFG_MR0_RBT
    #else
      #define CFG_1_MR0_RBT   (0x0)
    #endif
  #endif

  #if !( defined (CFG_1_CL) && defined (CONFIG_DYNAMIC_READ_ODT) )
    #ifdef CFG_CL
      #define CFG_1_MR0_CL   TO_MR0_CL(CFG_CL)
    #else
      #define CFG_1_MR0_CL   TO_MR0_CL(6)
    #endif
  #else
    #define CFG_1_MR0_CL   TO_MR0_CL(CFG_1_CL)
  #endif

  #ifndef CFG_1_MR0_BL
    #ifdef CFG_MR0_BL
      #define CFG_1_MR0_BL   CFG_MR0_BL
    #else
      #define CFG_1_MR0_BL   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_QOFF
    #ifdef CFG_MR1_QOFF
      #define CFG_1_MR1_QOFF   CFG_MR1_QOFF
    #else
      #define CFG_1_MR1_QOFF   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_TDQS
    #ifdef CFG_MR1_TDQS
      #define CFG_1_MR1_TDQS   CFG_MR1_TDQS
    #else
      #define CFG_1_MR1_TDQS   (0x0)
    #endif
  #endif

  /* MR1_RTT_NORM */
  #ifndef CONFIG_1_WRITE_ODT
    #ifdef CONFIG_WRITE_ODT
      #define CONFIG_1_WRITE_ODT   CONFIG_WRITE_ODT
    #else
      #define CONFIG_1_WRITE_ODT   (0x0)
    #endif
  #endif
  #if CONFIG_DYNAMIC_WRITE_ODT == 1
    #define CFG_1_MR1_RTT_NORM   (0x0)
  #elif CONFIG_1_WRITE_ODT == 60
    #define CFG_1_MR1_RTT_NORM   (0x1)
  #elif CONFIG_1_WRITE_ODT == 120
    #define CFG_1_MR1_RTT_NORM   (0x2)
  #elif CONFIG_1_WRITE_ODT == 40
    #define CFG_1_MR1_RTT_NORM   (0x3)
  #elif CONFIG_1_WRITE_ODT == 20
    #define CFG_1_MR1_RTT_NORM   (0x4)
  #elif CONFIG_1_WRITE_ODT == 30
    #define CFG_1_MR1_RTT_NORM   (0x5)
  #else
    #define CFG_1_MR1_RTT_NORM   (0x0)
  #endif

  #ifndef CFG_1_MR1_LEVEL
    #ifdef CFG_MR1_LEVEL
      #define CFG_1_MR1_LEVEL   CFG_MR1_LEVEL
    #else
      #define CFG_1_MR1_LEVEL   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_DIC
    #ifdef CFG_MR1_DIC
      #define CFG_1_MR1_DIC   CFG_MR1_DIC
    #else
      #define CFG_1_MR1_DIC   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_AL
    #ifdef CFG_MR1_AL
      #define CFG_1_MR1_AL   CFG_MR1_AL
    #else
      #define CFG_1_MR1_AL   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_DLL
    #ifdef CFG_MR1_DLL
      #define CFG_1_MR1_DLL   CFG_MR1_DLL
    #else
      #define CFG_1_MR1_DLL   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR2_RTT_WR
    #ifdef CFG_MR2_RTT_WR
      #define CFG_1_MR2_RTT_WR   CFG_MR2_RTT_WR
    #else
      #ifdef CONFIG_DYNAMIC_WRITE_ODT
	#if CONFIG_WRITE_ODT == 40 || CONFIG_WRITE_ODT == 60 
          #define CFG_1_MR2_RTT_WR (0x1)
	#elif CONFIG_WRITE_ODT == 120
	  #define CFG_1_MR2_RTT_WR (0x2)
	#endif
      #else
        #define CFG_1_MR2_RTT_WR   (0x0)
      #endif
    #endif
  #endif

  #ifndef CFG_1_MR2_SRT
    #ifdef CFG_MR2_SRT
      #define CFG_1_MR2_SRT   CFG_MR2_SRT
    #else
      #define CFG_1_MR2_SRT   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR2_ASR
    #ifdef CFG_MR2_ASR
      #define CFG_1_MR2_ASR   CFG_MR2_ASR
    #else
      #define CFG_1_MR2_ASR   (0x0)
    #endif
  #endif

  #ifndef CFG_1_CWL
    #ifdef CFG_CWL
      #define CFG_1_MR2_CWL   TO_MR2_CWL(CFG_CWL)
    #else
      #define CFG_1_MR2_CWL   TO_MR2_CWL(5)
    #endif
  #else
    #define CFG_1_MR2_CWL   TO_MR2_CWL(CFG_1_CWL)
  #endif

  #ifndef CFG_1_MR2_PASR
    #ifdef CFG_MR2_PASR
      #define CFG_1_MR2_PASR   CFG_MR2_PASR
    #else
      #define CFG_1_MR2_PASR   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR3_MPR
    #ifdef CFG_MR3_MPR
      #define CFG_1_MR3_MPR   CFG_MR3_MPR
    #else
      #define CFG_1_MR3_MPR   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR3_MPR_LOC
    #ifdef CFG_MR3_MPR_LOC
      #define CFG_1_MR3_MPR_LOC   CFG_MR3_MPR_LOC
    #else
      #define CFG_1_MR3_MPR_LOC   (0x0)
    #endif
  #endif

#elif CONFIG_DDR_TYPE == DDR2

  #ifndef CFG_1_MR0_PD
    #ifdef CFG_MR0_PD
      #define CFG_1_MR0_PD   CFG_MR0_PD
    #else
      #define CFG_1_MR0_PD   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR0_T_WR
    #ifdef CFG_MR0_T_WR
      #define CFG_1_MR0_T_WR   CFG_MR0_T_WR
    #else
      #define CFG_1_MR0_T_WR   (0x7)
    #endif
  #endif
  #if CFG_1_MR0_T_WR >= 2 && CFG_1_MR0_T_WR <= 8
    #define CFG_1_MR0_WR   (CFG_1_MR0_T_WR - 1)
  #else
    #error Invalid CFG_1_MR0_T_WR !
  #endif

  #ifndef CFG_1_MR0_DLL
    #ifdef CFG_MR0_DLL
      #define CFG_1_MR0_DLL   CFG_MR0_DLL
    #else
      #define CFG_1_MR0_DLL   (0x1)
    #endif
  #endif

  #ifndef CFG_1_MR0_TM
    #ifdef CFG_MR0_TM
      #define CFG_1_MR0_TM   CFG_MR0_TM
    #else
      #define CFG_1_MR0_TM   (0x0)
    #endif
  #endif

  #if !( defined (CFG_1_CL) && defined (CONFIG_DYNAMIC_READ_ODT) )
    #ifdef CFG_CL
      #define CFG_1_MR0_CL   CFG_CL
    #else
      #define CFG_1_MR0_CL   (5)
    #endif
  #else
    #define CFG_1_MR0_CL   CFG_1_CL
  #endif

  #ifndef CFG_1_MR0_BT
    #ifdef CFG_MR0_BT
      #define CFG_1_MR0_BT   CFG_MR0_BT
    #else
      #define CFG_1_MR0_BT   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR0_BL
    #ifdef CFG_MR0_BL
      #define CFG_1_MR0_BL   CFG_MR0_BL
    #else
      #define CFG_1_MR0_BL   (0x2)
    #endif
  #endif

  #ifndef CFG_1_MR1_QOFF
    #ifdef CFG_MR1_QOFF
      #define CFG_1_MR1_QOFF   CFG_MR1_QOFF
    #else
      #define CFG_1_MR1_QOFF   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_RDQS
    #ifdef CFG_MR1_RDQS
      #define CFG_1_MR1_RDQS   CFG_MR1_RDQS
    #else
      #define CFG_1_MR1_RDQS   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_DOSN
    #ifdef CFG_MR1_DOSN
      #define CFG_1_MR1_DOSN   CFG_MR1_DOSN
    #else
      #define CFG_1_MR1_DOSN   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_OCD_PROG
    #ifdef CFG_MR1_OCD_PROG
      #define CFG_1_MR1_OCD_PROG   CFG_MR1_OCD_PROG
    #else
      #define CFG_1_MR1_OCD_PROG   (0x7)
    #endif
  #endif

  /* MR1_RTT_NORM */
  #ifndef CONFIG_1_WRITE_ODT
    #ifdef CONFIG_WRITE_ODT
      #define CONFIG_1_WRITE_ODT   CONFIG_WRITE_ODT
    #else
      #define CONFIG_1_WRITE_ODT   (0x0)
    #endif
  #endif
  #if CONFIG_1_WRITE_ODT == 75
    #define CFG_1_MR1_RTT_NORM   (0x1)
  #elif CONFIG_1_WRITE_ODT == 150
    #define CFG_1_MR1_RTT_NORM   (0x2)
  #elif CONFIG_1_WRITE_ODT == 50
    #define CFG_1_MR1_RTT_NORM   (0x3)
  #else
    #define CFG_1_MR1_RTT_NORM   (0x0)
  #endif

  #ifndef CFG_1_MR1_AL
    #ifdef CFG_MR1_AL
      #define CFG_1_MR1_AL   CFG_MR1_AL
    #else
      #define CFG_1_MR1_AL   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_DIC
    #ifdef CFG_MR1_DIC
      #define CFG_1_MR1_DIC   CFG_MR1_DIC
    #else
      #define CFG_1_MR1_DIC   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR1_DLL
    #ifdef CFG_MR1_DLL
      #define CFG_1_MR1_DLL   CFG_MR1_DLL
    #else
      #define CFG_1_MR1_DLL   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR2_SRF
    #ifdef CFG_MR2_SRF
      #define CFG_1_MR2_SRF   CFG_MR2_SRF
    #else
      #define CFG_1_MR2_SRF   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR2_DCC
    #ifdef CFG_MR2_DCC
      #define CFG_1_MR2_DCC   CFG_MR2_DCC
    #else
      #define CFG_1_MR2_DCC   (0x0)
    #endif
  #endif

  #ifndef CFG_1_MR2_PASR
    #ifdef CFG_MR2_PASR
      #define CFG_1_MR2_PASR   CFG_MR2_PASR
    #else
      #define CFG_1_MR2_PASR   (0x0)
    #endif
  #endif

#endif /* DDR2 */

#ifndef CFG_1_T_MOD
  #ifdef CFG_T_MOD
    #define CFG_1_T_MOD   CFG_T_MOD
  #else
    #define CFG_1_T_MOD   (0)
  #endif
#endif

#ifndef CFG_1_T_WTR
  #ifdef CFG_T_WTR
    #define CFG_1_T_WTR   CFG_T_WTR
  #else
    #define CFG_1_T_WTR   (4)
  #endif
#endif

#ifndef CFG_1_T_RTP
  #ifdef CFG_T_RTP
    #define CFG_1_T_RTP   CFG_T_RTP
  #else
    #define CFG_1_T_RTP   (4)
  #endif
#endif

#ifndef CFG_1_T_RCD
  #ifdef CFG_T_RCD
    #define CFG_1_T_RCD   CFG_T_RCD
  #else
    #define CFG_1_T_RCD   (7)
  #endif
#endif

#ifndef CFG_1_T_RRD
  #ifdef CFG_T_RRD
    #define CFG_1_T_RRD   CFG_T_RRD
  #else
    #define CFG_1_T_RRD   (4)
  #endif
#endif

#ifndef CFG_1_T_MRD
  #ifdef CFG_T_MRD
    #define CFG_1_T_MRD   CFG_T_MRD
  #else
    #define CFG_1_T_MRD   (4)
  #endif
#endif

#ifndef CFG_1_T_RP
  #ifdef CFG_T_RP
    #define CFG_1_T_RP   CFG_T_RP
  #else
    #define CFG_1_T_RP   (7)
  #endif
#endif

#ifndef CFG_1_T_RFC
  #ifdef CFG_T_RFC
    #define CFG_1_T_RFC   CFG_T_RFC
  #else
    #define CFG_1_T_RFC   (0x56)
  #endif
#endif

#ifndef CFG_1_T_FAW
  #ifdef CFG_T_FAW
    #define CFG_1_T_FAW   CFG_T_FAW
  #else
    #define CFG_1_T_FAW   (0)
  #endif
#endif

#ifndef CFG_1_T_RC
  #ifdef CFG_T_RC
    #define CFG_1_T_RC   CFG_T_RC
  #else
    #define CFG_1_T_RC   (0x1b)
  #endif
#endif

#ifndef CFG_1_T_RAS
  #ifdef CFG_T_RAS
    #define CFG_1_T_RAS   CFG_T_RAS
  #else
    #define CFG_1_T_RAS   (0x14)
  #endif
#endif

#ifndef CFG_1_T_IDLE
  #ifdef CFG_T_IDLE
    #define CFG_1_T_IDLE   CFG_T_IDLE
  #else
    #define CFG_1_T_IDLE   (0x5b)
  #endif
#endif

#ifndef CFG_1_T_REF
  #ifdef CONFIG_T_REF_MODE
    #define CFG_1_T_REF   xcat(CFG_T_REF_,CONFIG_T_REF_MODE)
  #elif defined(CONFIG_1_T_REF)
    #define CFG_1_T_REF   xcat(CFG_T_REF_,CONFIG_1_T_REF_MODE)
  #else
    #define CFG_1_T_REF   (0x1046)
  #endif
#endif

#ifndef CFG_1_T_DLLRST
  #ifdef CFG_T_DLLRST
    #define CFG_1_T_DLLRST   CFG_T_DLLRST
  #else
    #define CFG_1_T_DLLRST   (0x517)
  #endif
#endif

#ifndef CFG_1_T_PWRON
  #ifdef CFG_T_PWRON
    #define CFG_1_T_PWRON   CFG_T_PWRON
  #else
    #define CFG_1_T_PWRON   (0x208d6)
  #endif
#endif

#ifndef CFG_1_ZQCL_CYCLE
  #ifdef CFG_ZQCL_CYCLE
    #define CFG_1_ZQCL_CYCLE   CFG_ZQCL_CYCLE
  #else
    #define CFG_1_ZQCL_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_ZQCS_CYCLE
  #ifdef CFG_ZQCS_CYCLE
    #define CFG_1_ZQCS_CYCLE   CFG_ZQCS_CYCLE
  #else
    #define CFG_1_ZQCS_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_T_ZQ_EXTEND
  #ifdef CFG_T_ZQ_EXTEND
    #define CFG_1_T_ZQ_EXTEND   CFG_T_ZQ_EXTEND
  #else
    #define CFG_1_T_ZQ_EXTEND   (0)
  #endif
#endif

#ifndef CFG_1_ZQC_FROCE_EN
  #ifdef CFG_ZQC_FROCE_EN
    #define CFG_1_ZQC_FROCE_EN   CFG_ZQC_FROCE_EN
  #else
    #define CFG_1_ZQC_FROCE_EN   (0)
  #endif
#endif

#ifndef CFG_1_ZQCL_CYCLE_EN
  #ifdef CFG_ZQCL_CYCLE_EN
    #define CFG_1_ZQCL_CYCLE_EN   CFG_ZQCL_CYCLE_EN
  #else
    #define CFG_1_ZQCL_CYCLE_EN   (0)
  #endif
#endif

#ifndef CFG_1_ZQCS_CYCLE_EN
  #ifdef CFG_ZQCS_CYCLE_EN
    #define CFG_1_ZQCS_CYCLE_EN   CFG_ZQCS_CYCLE_EN
  #else
    #define CFG_1_ZQCS_CYCLE_EN   (0)
  #endif
#endif

#ifndef CFG_1_PRECHARGED_DLL_OFF_PD_CYCLE
  #ifdef CFG_PRECHARGED_DLL_OFF_PD_CYCLE
    #define CFG_1_PRECHARGED_DLL_OFF_PD_CYCLE   CFG_PRECHARGED_DLL_OFF_PD_CYCLE
  #else
    #define CFG_1_PRECHARGED_DLL_OFF_PD_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_PRECHARGED_DLL_ON_PD_CYCLE
  #ifdef CFG_PRECHARGED_DLL_ON_PD_CYCLE
    #define CFG_1_PRECHARGED_DLL_ON_PD_CYCLE   CFG_PRECHARGED_DLL_ON_PD_CYCLE
  #else
    #define CFG_1_PRECHARGED_DLL_ON_PD_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_ACTIVE_PD_CYCLE
  #ifdef CFG_ACTIVE_PD_CYCLE
    #define CFG_1_ACTIVE_PD_CYCLE   CFG_ACTIVE_PD_CYCLE
  #else
    #define CFG_1_ACTIVE_PD_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_PRECHARGED_DLL_OFF_PD_EN
  #ifdef CFG_PRECHARGED_DLL_OFF_PD_EN
    #define CFG_1_PRECHARGED_DLL_OFF_PD_EN   CFG_PRECHARGED_DLL_OFF_PD_EN
  #else
    #define CFG_1_PRECHARGED_DLL_OFF_PD_EN   (0)
  #endif
#endif

#ifndef CFG_1_PRECHARGED_DLL_ON_PD_EN
  #ifdef CFG_PRECHARGED_DLL_ON_PD_EN
    #define CFG_1_PRECHARGED_DLL_ON_PD_EN   CFG_PRECHARGED_DLL_ON_PD_EN
  #else
    #define CFG_1_PRECHARGED_DLL_ON_PD_EN   (0)
  #endif
#endif

#ifndef CFG_1_ACTIVE_PD_EN
  #ifdef CFG_ACTIVE_PD_EN
    #define CFG_1_ACTIVE_PD_EN   CFG_ACTIVE_PD_EN
  #else
    #define CFG_1_ACTIVE_PD_EN   (0)
  #endif
#endif

#ifndef CFG_1_DLL_OFF_SRF_CYCLE
  #ifdef CFG_DLL_OFF_SRF_CYCLE
    #define CFG_1_DLL_OFF_SRF_CYCLE   CFG_DLL_OFF_SRF_CYCLE
  #else
    #define CFG_1_DLL_OFF_SRF_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_DLL_ON_SRF_CYCLE
  #ifdef CFG_DLL_ON_SRF_CYCLE
    #define CFG_1_DLL_ON_SRF_CYCLE   CFG_DLL_ON_SRF_CYCLE
  #else
    #define CFG_1_DLL_ON_SRF_CYCLE   (0)
  #endif
#endif

#ifndef CFG_1_DLL_OFF_SRF_EN
  #ifdef CFG_DLL_OFF_SRF_EN
    #define CFG_1_DLL_OFF_SRF_EN   CFG_DLL_OFF_SRF_EN
  #else
    #define CFG_1_DLL_OFF_SRF_EN   (0)
  #endif
#endif

#ifndef CFG_1_DLL_ON_SRF_EN
  #ifdef CFG_DLL_ON_SRF_EN
    #define CFG_1_DLL_ON_SRF_EN   CFG_DLL_ON_SRF_EN
  #else
    #define CFG_1_DLL_ON_SRF_EN   (0)
  #endif
#endif

#ifndef CFG_1_T_XPDLL
  #ifdef CFG_T_XPDLL
    #define CFG_1_T_XPDLL   CFG_T_XPDLL
  #else
    #define CFG_1_T_XPDLL   (0)
  #endif
#endif

#ifndef CFG_1_T_DLLK_EXTEND
  #ifdef CFG_T_DLLK_EXTEND
    #define CFG_1_T_DLLK_EXTEND   CFG_T_DLLK_EXTEND
  #else
    #define CFG_1_T_DLLK_EXTEND   (0)
  #endif
#endif

#ifndef CFG_1_T_XP
  #ifdef CFG_T_XP
    #define CFG_1_T_XP   CFG_T_XP
  #else
    #define CFG_1_T_XP   (0)
  #endif
#endif

#ifndef CFG_1_T_PD
  #ifdef CFG_T_PD
    #define CFG_1_T_PD   CFG_T_PD
  #else
    #define CFG_1_T_PD   (0)
  #endif
#endif

#ifndef CFG_1_T_XS
  #ifdef CFG_T_XS
    #define CFG_1_T_XS   CFG_T_XS
  #else
    #define CFG_1_T_XS   (0)
  #endif
#endif

#ifndef CFG_1_T_CKSREX
  #ifdef CFG_T_CKSREX
    #define CFG_1_T_CKSREX   CFG_T_CKSREX
  #else
    #define CFG_1_T_CKSREX   (0)
  #endif
#endif

#ifndef CFG_1_T_CKESR
  #ifdef CFG_T_CKESR
    #define CFG_1_T_CKESR   CFG_T_CKESR
  #else
    #define CFG_1_T_CKESR   (0)
  #endif
#endif

#ifndef CFG_1_DYNAMIC_DQ_DQS_RCV_EN
  #ifdef CFG_DYNAMIC_DQ_DQS_RCV_EN
    #define CFG_1_DYNAMIC_DQ_DQS_RCV_EN   CFG_DYNAMIC_DQ_DQS_RCV_EN
  #else
    #define CFG_1_DYNAMIC_DQ_DQS_RCV_EN   (1)
  #endif
#endif

#undef CFG_1_DYNAMIC_DM_DQ_DQS_DRV_EN
#if CONFIG_READ_ODT == 0
    #define CFG_1_DYNAMIC_DM_DQ_DQS_DRV_EN   (1)
#else
    #define CFG_1_DYNAMIC_DM_DQ_DQS_DRV_EN   (0)
#endif

#ifndef CFG_1_DYNAMIC_ADDR_DRV_EN
  #ifdef CFG_DYNAMIC_ADDR_DRV_EN
    #define CFG_1_DYNAMIC_ADDR_DRV_EN   CFG_DYNAMIC_ADDR_DRV_EN
  #else
    #define CFG_1_DYNAMIC_ADDR_DRV_EN   (1)
  #endif
#endif

#ifndef CFG_1_DYNAMIC_CMD_DRV_EN
  #ifdef CFG_DYNAMIC_CMD_DRV_EN
    #define CFG_1_DYNAMIC_CMD_DRV_EN   CFG_DYNAMIC_CMD_DRV_EN
  #else
    #define CFG_1_DYNAMIC_CMD_DRV_EN   (1)
  #endif
#endif

#ifndef CFG_1_DYNAMIC_CLK_DRV_EN
  #ifdef CFG_DYNAMIC_CLK_DRV_EN
    #define CFG_1_DYNAMIC_CLK_DRV_EN   CFG_DYNAMIC_CLK_DRV_EN
  #else
    #define CFG_1_DYNAMIC_CLK_DRV_EN   (1)
  #endif
#endif

#ifndef CFG_1_NOP_AC_OE_DISABLE
  #ifdef CFG_NOP_AC_OE_DISABLE
    #define CFG_1_NOP_AC_OE_DISABLE   CFG_NOP_AC_OE_DISABLE
  #else
    #define CFG_1_NOP_AC_OE_DISABLE   (1)
  #endif
#endif

#ifndef CFG_1_DYNAMIC_ADDR_OE_EN
  #ifdef CFG_DYNAMIC_ADDR_OE_EN
    #define CFG_1_DYNAMIC_ADDR_OE_EN   CFG_DYNAMIC_ADDR_OE_EN
  #else
    #define CFG_1_DYNAMIC_ADDR_OE_EN   (1)
  #endif
#endif

#ifndef CFG_1_DYNAMIC_CMD_OE_EN
  #ifdef CFG_DYNAMIC_CMD_OE_EN
    #define CFG_1_DYNAMIC_CMD_OE_EN   CFG_DYNAMIC_CMD_OE_EN
  #else
    #define CFG_1_DYNAMIC_CMD_OE_EN   (1)
  #endif
#endif

#ifndef CFG_1_DYNAMIC_CLK_OE_EN
  #ifdef CFG_DYNAMIC_CLK_OE_EN
    #define CFG_1_DYNAMIC_CLK_OE_EN   CFG_DYNAMIC_CLK_OE_EN
  #else
    #define CFG_1_DYNAMIC_CLK_OE_EN   (1)
  #endif
#endif

#ifndef CFG_1_CKE_BIT_EN
  #ifdef CFG_CKE_BIT_EN
    #define CFG_1_CKE_BIT_EN   CFG_CKE_BIT_EN
  #else
    #define CFG_1_CKE_BIT_EN   (1)
  #endif
#endif

#ifndef CFG_1_ODT_BIT_EN
  #ifdef CFG_ODT_BIT_EN
    #define CFG_1_ODT_BIT_EN   CFG_ODT_BIT_EN
  #else
    #define CFG_1_ODT_BIT_EN   (1)
  #endif
#endif

#ifndef CFG_1_CS_N_BIT_EN
  #ifdef CFG_CS_N_BIT_EN
    #define CFG_1_CS_N_BIT_EN   CFG_CS_N_BIT_EN
  #else
    #define CFG_1_CS_N_BIT_EN   (1)
  #endif
#endif

#ifndef CFG_1_ADDR_BIT_EN
  #ifdef CFG_ADDR_BIT_EN
    #define CFG_1_ADDR_BIT_EN   CFG_ADDR_BIT_EN
  #else
    #define CFG_1_ADDR_BIT_EN   (0xffff)
  #endif
#endif

#ifndef CFG_1_DFI_LP_WAKEUP
  #ifdef CFG_DFI_LP_WAKEUP
    #define CFG_1_DFI_LP_WAKEUP   CFG_DFI_LP_WAKEUP
  #else
    #define CFG_1_DFI_LP_WAKEUP   (0)
  #endif
#endif

#ifndef CFG_1_DFI_LP_ACK
  #ifdef CFG_DFI_LP_ACK
    #define CFG_1_DFI_LP_ACK   CFG_DFI_LP_ACK
  #else
    #define CFG_1_DFI_LP_ACK   (0)
  #endif
#endif

#ifndef CFG_1_DFI_LP_REQ
  #ifdef CFG_DFI_LP_REQ
    #define CFG_1_DFI_LP_REQ   CFG_DFI_LP_REQ
  #else
    #define CFG_1_DFI_LP_REQ   (0)
  #endif
#endif

#ifndef CFG_1_DFI_INIT_COMPLETE
  #ifdef CFG_DFI_INIT_COMPLETE
    #define CFG_1_DFI_INIT_COMPLETE   CFG_DFI_INIT_COMPLETE
  #else
    #define CFG_1_DFI_INIT_COMPLETE   (0)
  #endif
#endif

#ifndef CFG_1_DFI_DRAM_CLK_DISABLE
  #ifdef CFG_DFI_DRAM_CLK_DISABLE
    #define CFG_1_DFI_DRAM_CLK_DISABLE   CFG_DFI_DRAM_CLK_DISABLE
  #else
    #define CFG_1_DFI_DRAM_CLK_DISABLE   (0)
  #endif
#endif

#ifndef CFG_1_DFI_DATA_HALFWORDLANE_DISABLE
  #ifdef CFG_DFI_DATA_HALFWORDLANE_DISABLE
    #define CFG_1_DFI_DATA_HALFWORDLANE_DISABLE   CFG_DFI_DATA_HALFWORDLANE_DISABLE
  #else
    #define CFG_1_DFI_DATA_HALFWORDLANE_DISABLE   (0)
  #endif
#endif

#ifndef CFG_1_DFI_INIT_START
  #ifdef CFG_DFI_INIT_START
    #define CFG_1_DFI_INIT_START   CFG_DFI_INIT_START
  #else
    #define CFG_1_DFI_INIT_START   (0)
  #endif
#endif
