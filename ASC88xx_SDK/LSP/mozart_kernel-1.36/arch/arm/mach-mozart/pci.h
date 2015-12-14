
#include <linux/pci.h>


//#define _MOZART_PCI_DEBUG

#ifdef _MOZART_PCI_DEBUG
#define pci_debug(fmt,arg...) printk(KERN_EMERG "[PCIE] " fmt, ##arg)
#else
#define pci_debug(fmt,arg...) {}
#endif



/*
 * Vitrual Base Address
 */
#define PCIEC_SSC_BASE     IO_ADDRESS(VPL_PCIEC_SSC_BASE)
#define PCIEC_DBI_BASE     IO_ADDRESS(VPL_PCIEC_DBI_BASE)
#define PCIEC_PL_BASE      (PCIEC_DBI_BASE + 0x700)
#define PCIEC_ELBI_BASE    (PCIEC_DBI_BASE + 0x40000)
#define PCIEC_MEM_BASE     IO_ADDRESS(VPL_PCIEC_MEM_BASE)
#define PCIEC_CFG0_MEM_OFFSET (0x0)
#define PCIEC_CFG1_MEM_OFFSET (0x10000)

#define ssc_readl(r)        inl(PCIEC_SSC_BASE + (r))
#define ssc_writel(v, r)    outl((v), PCIEC_SSC_BASE + (r))
#define dbi_readl(r)        inl(PCIEC_DBI_BASE + (r))
#define dbi_writel(v, r)    outl((v), PCIEC_DBI_BASE + (r))
#define pl_readl(r)         inl(PCIEC_PL_BASE + (r))
#define pl_writel(v, r)     outl((v), PCIEC_PL_BASE + (r))
#define elbi_readl(r)       inl(PCIEC_ELBI_BASE + (r))
#define elbi_writel(v, r)   outl((v), PCIEC_ELBI_BASE + (r))


/*
 * PCIESSC MMRs
 */
#define CFG_0      0x4
#define CFG_1      0x8
#define CR_0       0xC
#define CR_1       0x10

#define PCIEC_SSC_CFG0     (PCIEC_SSC_BASE + 0x4)
#define     PCIEC_RESET_DONE             (30)
#define     PCIE_DEVICE_TYPE_MASK        (0x1EUL)
#define     PCIE_DEVICE_TYPE_ROOTPORT    (0x8UL)
#define     PCIE_DEVICE_TYPE_LEGACY_EP   (0x2UL)
#define     PCIE_DEVICE_TYPE_ENDPOINT    (0x0)
#define     PCIE_PHY_CFG_DONE            (0)
#define PCIEC_SSC_CFG1     (PCIEC_SSC_BASE + 0x8)
#define PCIEC_SSC_CR0      (PCIEC_SSC_BASE + 0xC)
#define PCIEC_SSC_CR1      (PCIEC_SSC_BASE + 0x10)

/*
 * ELBI MMRs
 */
#define ELBI_CTRL                                 (0x0)
#define PCIEC_ELBI_CTRL                           (PCIEC_ELBI_BASE + 0x0)
#define     LTSSM_STATE_SHIFT                     (24)
#define     INBOUND_ATU_SHIFT                     (3)
#define     OUTBOUND_ATU_SHIFT                    (2)
#define     LTSSM_ENABLE_SHIFT                    (0)
#define     LTSSM_STATE                           (0x3F << LTSSM_STATE_SHIFT)
#define     INBOUND_ATU                           (1 << INBOUND_ATU_SHIFT)
#define     OUTBOUND_ATU                          (1 << OUTBOUND_ATU_SHIFT)
#define     SELECT_xATU                           (0)
#define     SELECT_iATU                           (1)
#if defined(INTX_FIFO) && (INTX_FIFO == 1)
#define     INTX_FIFO_ENABLE                      (1)
#endif
#define     APP_LTSSM_ENABLE                      (1 << LTSSM_ENABLE_SHIFT)
#define PCIEC_ELBI_OUTBOUND_CTRL                  (PCIEC_ELBI_BASE + 0x50)
#define PCIEC_ELBI_OUTBOUND_MEM0_BASE_LSB         (PCIEC_ELBI_BASE + 0x54)
#define PCIEC_ELBI_OUTBOUND_MEM1_BASE_LSB         (PCIEC_ELBI_BASE + 0x5C)
#define PCIEC_ELBI_OUTBOUND_IO_BASE_LSB           (PCIEC_ELBI_BASE + 0x64)
#define PCIEC_ELBI_OUTBOUND_AMBA_MEM0_START_LSB   (PCIEC_ELBI_BASE + 0x74)
#define PCIEC_ELBI_OUTBOUND_AMBA_MEM0_LIMIT_LSB   (PCIEC_ELBI_BASE + 0x7C)
#define PCIEC_ELBI_OUTBOUND_AMBA_MEM1_START_LSB   (PCIEC_ELBI_BASE + 0x84)
#define PCIEC_ELBI_OUTBOUND_AMBA_MEM1_LIMIT_LSB   (PCIEC_ELBI_BASE + 0x8C)
#define PCIEC_ELBI_OUTBOUND_AMBA_IO_START_LSB     (PCIEC_ELBI_BASE + 0x94)
#define PCIEC_ELBI_OUTBOUND_AMBA_IO_LIMIT_LSB     (PCIEC_ELBI_BASE + 0x9C)
#define PCIEC_ELBI_SLAVE_SIDEBAND                 (PCIEC_ELBI_BASE + 0XD4)
#define PCIEC_ELBI_MASTER_SIDEBAND                (PCIEC_ELBI_BASE + 0xD8)

#define PCIBIOS_NOT_ALIGN           0x90

//#define __FPGA__

#ifdef __FPGA__
#define PCIE_RESET					9 /* use GPIO 18 as PCIE reset pin */
#else
#define PCIE_RESET					18 /* use GPIO 18 as PCIE reset pin */
#endif

typedef struct {
	unsigned char bus;
	unsigned int devfn;
	unsigned short vendor_id;
	unsigned short device_id;
	unsigned int *dev_regs;
	void (*regmap_init)(unsigned int *);
} pdev_ids;
