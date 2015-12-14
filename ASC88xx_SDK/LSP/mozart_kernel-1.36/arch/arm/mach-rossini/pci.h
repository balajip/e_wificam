
#include <linux/pci.h>


/*#define _ROSSINI_PCI_DEBUG*/

#ifdef _ROSSINI_PCI_DEBUG
#define pci_debug(fmt,arg...) printk(KERN_EMERG fmt,##arg)
#else
#define pci_debug(fmt,arg...) {}
#endif

#define VA_ROSSINI_PCI_SSC_BASE     IO_ADDRESS(ROSSINI_PCIEC_SSC_BASE)
#define VA_ROSSINI_PCI_DBI_BASE     IO_ADDRESS(ROSSINI_PCIEC_DBI_BASE)
#define VA_ROSSINI_PCI_MEM_BASE     IO_ADDRESS(ROSSINI_PCIEC_MEM_BASE)

#define VA_ROSSINI_PCI_ELBI_BASE	(VA_ROSSINI_PCI_DBI_BASE+0x40000)

#define PCI_SSC_CFG0            	(VA_ROSSINI_PCI_SSC_BASE+0x4)
#define PCI_SSC_CFG1            	(VA_ROSSINI_PCI_SSC_BASE+0x8)
#define PCI_SSC_CR0			(VA_ROSSINI_PCI_SSC_BASE+0xc)
#define PCI_SSC_CR1			(VA_ROSSINI_PCI_SSC_BASE+0x10)

#define PCI_ELBI_CTRL			(VA_ROSSINI_PCI_ELBI_BASE+0x0)
#define PCI_ELBI_OUTBOUND_CTRL		(VA_ROSSINI_PCI_ELBI_BASE+0x50)
#define PCI_ELBI_SLAVE_SIDEBAND		(VA_ROSSINI_PCI_ELBI_BASE+0XD4)
#define PCI_ELBI_MASTER_SIDEBAND	(VA_ROSSINI_PCI_ELBI_BASE+0xD8)

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
