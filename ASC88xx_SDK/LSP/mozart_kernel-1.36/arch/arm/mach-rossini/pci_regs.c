
#include "pci.h"


const unsigned int pci_type0_cfghdr[16] = {
	0x00000000, 0x00000547, 0x00000000, 0x000000ff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
	0xfff00001, 0x00000000, 0x00000000, 0x000000ff,
};

const unsigned int pci_type1_cfghdr[16] = {
	0x00000000, 0x00000547, 0x00000000, 0x000000ff,
	0xfff00000, 0xfff00000, 0x00ffffff, 0x0000f0f0,
	0xfff0fff0, 0xfff0fff0, 0xffffffff, 0xffffffff,
	0xffffffff, 0x00000000, 0xfff00001, 0x000000ff,
};

const unsigned int pci_pmcap_regs[2] = {
	0x00000000, 0x00000003,
};

const unsigned int pci_msicap_regs[4] = {
	0x00010000, 0xfffffffc, 0xffffffff, 0x0000ffff,
};

const unsigned int pci_pciecap_regs[13] = {
	0x00000000, 0x00000000, 0x00007fff, 0x00000000,
	0x000001f3, 0x00000000, 0x00001fff, 0x0000000f,
	0x00000000, 0x00000000, 0x00000010, 0x00000000,
	0x00001f9f,
};

const unsigned long pci_aercap_regs[11] = {
	0x00000000, 0x00000000, 0x001ff011, 0x001ff011,
	0x00000000, 0x000011c1, 0x00000140, 0x00000000,
	0x00000000, 0x00000000, 0x00000000,
};

const unsigned long pci_sncap_regs[3] = {
	0x00000000, 0x00000000, 0x00000000,
};



#ifdef CONFIG_PCIEDEV_E1000E
static void e1000e_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xfffe0000; /* BAR0 */
	dev_regs[5] = 0xfffe0000; /* BAR1 */
	dev_regs[6] = 0xffffffe0; /* BAR2 */
	dev_regs[7] = 0x00000000; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0xfffe0001; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x32]), pci_pmcap_regs, 4*2);	/* PM capability on 0xC8 */
	memcpy(&(dev_regs[0x34]), pci_msicap_regs, 4*4);	/* MSI capability on 0xD0 */
	memcpy(&(dev_regs[0x38]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0xE0 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
	memcpy(&(dev_regs[0x50]), pci_sncap_regs, 4*3); 	/* DSN capability on 0x140 */
}
#endif

#ifdef CONFIG_PCIEDEV_RT3092
static void RT3092_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xffff0000; /* BAR0 */
	dev_regs[5] = 0x00000000; /* BAR1 */
	dev_regs[6] = 0x00000000; /* BAR2 */
	dev_regs[7] = 0x00000000; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0x00000000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x10]), pci_pmcap_regs, 4*2);	/* PM capability on 0x40 */
	memcpy(&(dev_regs[0x14]), pci_msicap_regs, 4*4);	/* MSI capability on 0x50 */
	memcpy(&(dev_regs[0x1c]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0x70 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
	memcpy(&(dev_regs[0x50]), pci_sncap_regs, 4*3); 	/* DSN capability on 0x140 */
}
#endif

#ifdef CONFIG_PCIEDEV_RT3090
static void RT3090_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xffff0000; /* BAR0 */
	dev_regs[5] = 0x00000000; /* BAR1 */
	dev_regs[6] = 0x00000000; /* BAR2 */
	dev_regs[7] = 0x00000000; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0x00000000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x10]), pci_pmcap_regs, 4*2);	/* PM capability on 0x40 */
	memcpy(&(dev_regs[0x14]), pci_msicap_regs, 4*4);	/* MSI capability on 0x50 */
	memcpy(&(dev_regs[0x1c]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0x70 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
	memcpy(&(dev_regs[0x50]), pci_sncap_regs, 4*3); 	/* DSN capability on 0x140 */
}
#endif

#ifdef CONFIG_PCIEDEV_RT5390
static void RT5390_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xffff0000; /* BAR0 */
	dev_regs[5] = 0x00000000; /* BAR1 */
	dev_regs[6] = 0x00000000; /* BAR2 */
	dev_regs[7] = 0x00000000; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0x00000000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x10]), pci_pmcap_regs, 4*2);	    /* PM capability on 0x40 */
	memcpy(&(dev_regs[0x14]), pci_msicap_regs, 4*4);	/* MSI capability on 0x50 */
	memcpy(&(dev_regs[0x1c]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0x70 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
	memcpy(&(dev_regs[0x50]), pci_sncap_regs, 4*3); 	/* DSN capability on 0x140 */
}
#endif

#ifdef CONFIG_PCIEDEV_RTL8176
static void RTL8176_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xFFFFFF00; /* BAR0 */
	dev_regs[5] = 0x00000000; /* BAR1 */
	dev_regs[6] = 0xFFFFC000; /* BAR2 */
	dev_regs[7] = 0xFFFFFFFF; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0x00000000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x10]), pci_pmcap_regs, 4*2);	    /* PM capability on 0x40 */
	memcpy(&(dev_regs[0x14]), pci_msicap_regs, 4*4);	/* MSI capability on 0x50 */
	memcpy(&(dev_regs[0x1c]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0x70 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
	/*memcpy(&(dev_regs[0x50]), pci_xxx_regs, 4*?);*/ 	/* Virtual Channel capability on 0x140 */
	memcpy(&(dev_regs[0x58]), pci_sncap_regs, 4*3); 	/* DSN capability on 0x160 */
}
#endif

#ifdef CONFIG_PCIEDEV_RTL8192
static void RTL8192_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xffffff00; /* BAR0 */
	dev_regs[5] = 0xffffc000; /* BAR1 */
	dev_regs[6] = 0x00000000; /* BAR2 */
	dev_regs[7] = 0x00000000; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0x00000000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x10]), pci_pmcap_regs, 4*2);	/* PM capability on 0x40 */
	memcpy(&(dev_regs[0x14]), pci_msicap_regs, 4*4);	/* MSI capability on 0x50 */
	memcpy(&(dev_regs[0x1c]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0x70 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
	memcpy(&(dev_regs[0x58]), pci_sncap_regs, 4*3); 	/* DSN capability on 0x140 */
}
#endif

#ifdef CONFIG_PCIEDEV_SII3132
static void SiI3132_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0xffffff80; /* BAR0 */
	dev_regs[5] = 0xffffffff; /* BAR1 */
	dev_regs[6] = 0xffffc000; /* BAR2 */
	dev_regs[7] = 0xffffffff; /* BAR3 */
	dev_regs[8] = 0xffffff80; /* BAR4 */
	dev_regs[9] = 0x00000000; /* BAR5 */
	dev_regs[12] = 0xfff80000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x15]), pci_pmcap_regs, 4*2);	    /* PM capability on 0x54 */
	memcpy(&(dev_regs[0x17]), pci_msicap_regs, 4*4);	/* MSI capability on 0x5C */
	memcpy(&(dev_regs[0x1c]), pci_pciecap_regs, 4*13);	/* PCI Express capability on 0x70 */

	/* Set up PCI Express extend configuration space mask (rom 0x100 to 0x1000)*/
	memcpy(&(dev_regs[0x40]), pci_aercap_regs, 4*11);	/* AER capability on 0x100 */
}
#endif

#ifdef CONFIG_PCIEDEV_JMB363
static void JMB363_regmap_init(unsigned int *dev_regs)
{
	/* set up PCI compatible configuration space mask (under 0x40) */
	memcpy(dev_regs, pci_type0_cfghdr, 4*16); /* type 0 configuration space header */
	dev_regs[4] = 0x00000000; /* BAR0 */
	dev_regs[5] = 0x00000000; /* BAR1 */
	dev_regs[6] = 0x00000000; /* BAR2 */
	dev_regs[7] = 0x00000000; /* BAR3 */
	dev_regs[8] = 0x00000000; /* BAR4 */
	dev_regs[9] = 0xFFFFE000; /* BAR5 */
	dev_regs[12] = 0xFFFF0000; /* ROM base */

	/* set up device specific capabilities space mask (from 0x40 to 0x100)*/
	memcpy(&(dev_regs[0x1A]), pci_pmcap_regs, 4*2);    /* PM capability on 0x68 */
	memcpy(&(dev_regs[0x14]), pci_pciecap_regs, 4*13); /* PCI Express capability on 0x50 */

	/* 0x40 */
	dev_regs[0x10] = 0xffffffff; /* configuration space 0x40 */
}
#endif



pdev_ids pdev_id_list[] =
{
#ifdef CONFIG_PCIEDEV_E1000E
	{
		.bus = 1,		// Intel e1000 ethernet card
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x8086,
		.device_id = 0x107d,
		.dev_regs = NULL,
		.regmap_init = e1000e_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_JMB363
	{
		.bus = 1,		// JMicron 20360/20303
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x197B,
		.device_id = 0x2363,
		.dev_regs = NULL,
		.regmap_init = JMB363_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_SII3132
	{
		.bus = 1,		// SiI3132
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x1095,
		.device_id = 0x3132,
		.dev_regs = NULL,
		.regmap_init = SiI3132_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_RT3092
	{
		.bus = 1,		// Ralink 802.11n WiFi card(2T2R)
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x1814,
		.device_id = 0x3092,
		.dev_regs = NULL,
		.regmap_init = RT3092_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_RT3090
	{
		.bus = 1,		// Ralink 802.11n WiFi card(1T1R)
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x1814,
		.device_id = 0x3090,
		.dev_regs = NULL,
		.regmap_init = RT3090_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_RT5390
	{
		.bus = 1,		// Ralink 802.11n WiFi card(1T1R)
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x1814,
		.device_id = 0x5390,
		.dev_regs = NULL,
		.regmap_init = RT5390_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_RTL8192
	{
		.bus = 1,		// Realtek 802.11n WiFi card
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x10EC,
		.device_id = 0x8174,
		.dev_regs = NULL,
		.regmap_init = RTL8192_regmap_init,
	},
#endif
#ifdef CONFIG_PCIEDEV_RTL8176
	{
		.bus = 1,		// Realtek 802.11n WiFi card
		.devfn = PCI_DEVFN(0, 0),
		.vendor_id = 0x10EC,
		.device_id = 0x8176,
		.dev_regs = NULL,
		.regmap_init = RTL8176_regmap_init,
	},
#endif
	{
		.vendor_id = 0xffff,
		.device_id = 0xffff,
	}
};



void rossini_pci_regmap_init(pdev_ids *pdev)
{
	pdev->dev_regs = (unsigned int *)kmalloc(4096, GFP_KERNEL);
	memset(pdev->dev_regs, 0x5a5a5a5a, 1024); /* 1024 dwords */
	pdev->regmap_init(pdev->dev_regs);
	pci_debug("[PCIE] VID 0x%x, PID 0x%x\n", pdev->vendor_id, pdev->device_id);
}

unsigned int rossini_pci_getmask(unsigned char bus, unsigned int devfn, int where)
{
	pdev_ids *pdev_id;
	int found;

	if (where >= 4096)
		return 0x5a5a5a5a;

	pdev_id = pdev_id_list;

	found = 0;
	do {
		if (pdev_id->dev_regs && (pdev_id->bus == bus) && (pdev_id->devfn == devfn)) {
			found = 1;
			break;
		}
		++pdev_id;
	} while ((pdev_id->vendor_id != 0xffff)&&(pdev_id->device_id != 0xffff));

	if (found == 0) {
		printk("[Rossini] PCIE: can not find register mask!!\n");
		while(1);
	}

	return pdev_id->dev_regs[where>>2];
}


EXPORT_SYMBOL(pdev_id_list);
