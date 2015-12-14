
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/msi.h>
#include <mach/irq.h>
#include <asm/mach/pci.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include "pci.h"


extern pdev_ids pdev_id_list[];

static spinlock_t pci_lock = SPIN_LOCK_UNLOCKED;

static struct pci_dev *pci_bridge = NULL;

static int link_fail = 0;

extern void rossini_pci_regmap_init(pdev_ids *);
extern unsigned int rossini_pci_getmask(unsigned char bus, unsigned int devfn, int where);

static void synop_set_sideband(unsigned char bus, unsigned int dev, unsigned int fn)
{
	u32 val = 0;

	val = 0x1;
	if (bus == 1) {
		val |= (bus<<9);
		val |= (dev<<4);
	}
	else {
		val |= (bus<<22);
		val |= (dev<<17);
	}
	outl(val, PCI_ELBI_OUTBOUND_CTRL);
	pci_debug("PCI_ELBI_OUTBOUND_CTRL: 0x%08x (0x%08x)\n", val, inl(PCI_ELBI_OUTBOUND_CTRL));

	val = 0xf<<20;
	if (bus == 1)
		val |= 0x4;
	else
		val |= 0x5;

	outl(val, PCI_ELBI_SLAVE_SIDEBAND);
	pci_debug("PCI_ELBI_SLAVE_SIDEBAND: 0x%08x (0x%08x)\n", val, inl(PCI_ELBI_SLAVE_SIDEBAND));
}

static void inline synop_set_sideband_mem(void)
{
	u32 val = 0;

	outl(0x1, PCI_ELBI_OUTBOUND_CTRL);
	val = 0xf<<20;
	outl(val, PCI_ELBI_SLAVE_SIDEBAND);
}

static int rossini_read_config(unsigned char bus, unsigned int devfn, int where, int size, u32* val)
{
	unsigned long flags;
	u32 des_port = 0;
	unsigned int shift;

	spin_lock_irqsave(&pci_lock, flags);
	if (bus == 0) {
		if ((PCI_SLOT(devfn) == 0) && (PCI_FUNC(devfn) == 0)) { /* bus 0, dev 0, func 0, ie. RC downstrem port*/
			des_port = VA_ROSSINI_PCI_DBI_BASE + (where&0xfffffffc);
			*val = inl(des_port);
		}
		else { /* no other device in virtual bus 0 */
			*val = 0xffffffff;
		}
	}
	else if (bus == 1) { /* bus 1, type 0 CFG read, reply 0xffffffff if !(dev == 0)*/
		if ((PCI_SLOT(devfn) == 0) && (link_fail == 0)) { /* function number always 0 */
			des_port =  VA_ROSSINI_PCI_MEM_BASE + 0x700000 + (PCI_FUNC(devfn)<<16) + (where&0xfffffffc);
			synop_set_sideband(bus, PCI_SLOT(devfn), PCI_FUNC(devfn));
			pci_debug("[Rossini] read des_port: 0x%08x\n", des_port);
			*val = inl(des_port);
			pci_debug("[Rossini] CFG Read %02x:%04x:%x %02x.%d = 0x%x\n", bus, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, *val);
		}
		else { /* return 0xffffffff for non-exist device */
			*val = 0xffffffff;
		}
	}
	else {
		des_port =  VA_ROSSINI_PCI_MEM_BASE + 0x900000 + (PCI_FUNC(devfn)<<16) + (where&0xfffffffc);
		synop_set_sideband(bus, PCI_SLOT(devfn), PCI_FUNC(devfn));
		pci_debug("[Rossini] des_port: 0x%08x\n", des_port);
		*val = inl(des_port);
	}
	spin_unlock_irqrestore(&pci_lock, flags);

	switch (size) {
	case 1:
		shift = (where & 0x3) * 8;
		*val = (*val >> shift) & 0xff;
		break;
	case 2:
		shift = (where & 0x3) * 8;
		*val = (*val >> shift) & 0xffff;
		break;
	case 4:
		break;
	default:
		pci_debug("[Rossini]PCI CFG access alignmnet error!!\n");
		return PCIBIOS_NOT_ALIGN;
	}

	synop_set_sideband_mem();

	return PCIBIOS_SUCCESSFUL;
}

static int rossini_write_config(unsigned char bus, unsigned int devfn, int where, int size, u32 val)
{
	unsigned long flags;
	u32 des_port = 0;
	unsigned int shift;
	u32 ori_val;

	spin_lock_irqsave(&pci_lock, flags);
	if (bus == 0) {
		if ((PCI_SLOT(devfn) == 0) && (PCI_FUNC(devfn) == 0)) { /* bus 0, dev 0, func 0, ie. RC downstrem port*/
			des_port = VA_ROSSINI_PCI_DBI_BASE + (where&0xfffffffc);
		}
	}
	else if (bus == 1) {
		if (PCI_SLOT(devfn) == 0) {
			des_port =  VA_ROSSINI_PCI_MEM_BASE + 0x700000 + (PCI_FUNC(devfn)<<16) + (where&0xfffffffc);
			synop_set_sideband(bus, PCI_SLOT(devfn), PCI_FUNC(devfn));
			pci_debug("[Rossini] write des_port: 0x%08x\n", des_port);
			pci_debug("[Rossini] CFG Write %02x:%04x:%x %02x.%d = 0x%x\n", bus, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, val);
		}
	}
	else {
		des_port =  VA_ROSSINI_PCI_MEM_BASE + 0x900000 + (PCI_FUNC(devfn)<<16) + (where&0xfffffffc);
		synop_set_sideband(bus, PCI_SLOT(devfn), PCI_FUNC(devfn));
	}

	if (des_port == 0) {
		spin_unlock_irqrestore(&pci_lock, flags);
		return PCIBIOS_SUCCESSFUL;
	}

	switch (size) {
	case 1:
		shift = (where & 0x3) * 8;
		ori_val = inl(des_port);
		ori_val = (ori_val & ~(0xff << shift)) | (val << shift);
		outl(ori_val, des_port);
		break;
	case 2:
		shift = (where & 0x3) * 8;
		ori_val = inl(des_port);
		ori_val = (ori_val & ~(0xffff << shift)) | (val << shift);
		outl(ori_val, des_port);
		break;
	case 4:
		outl(val, des_port);
		break;
	default:
		pci_debug("[Rossini]PCI CFG access alignmnet error!!\n");
		return PCIBIOS_NOT_ALIGN;
	}
	spin_unlock_irqrestore(&pci_lock, flags);

	synop_set_sideband_mem();

	return PCIBIOS_SUCCESSFUL;
}

static int pci_read_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32* val)
{
	u32 id;
	pdev_ids *pdev_id;

/*	make sure target device will not return CPL with CRS */
	if (bus->number != 0 ){
		int loop = 0;

		pdev_id = pdev_id_list;
		do {
			if (pdev_id->dev_regs && (pdev_id->bus == bus->number) && (pdev_id->devfn == devfn))
				break;
			++pdev_id;
		} while ((pdev_id->vendor_id != 0xffff)&&(pdev_id->device_id != 0xffff)&&(link_fail == 0));

		do {
			++loop;
			rossini_read_config(bus->number, devfn, 0, 4, &id);
		} while ((id != ((pdev_id->device_id<<16)|pdev_id->vendor_id)) && (link_fail == 0));
	}


	return rossini_read_config(bus->number, devfn, where, size, val);
}

static int pci_write_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val)
{
	int ret, loop;
	u32 check;
	unsigned int mask, smask, shift;

	if (bus->number == 0) { /* write to local DBI */
		return rossini_write_config(bus->number, devfn, where, size, val);
	}

	shift = (where&0x3)*8;
	smask = 0xffffffff >> ((4-size)*8);
	mask = rossini_pci_getmask(bus->number, devfn, where);
	if (mask == 0x5a5a5a5a) {
		printk("[Rossini] PCIE: %x, %x, %x\n", bus->number, devfn, where);
		printk("[Rossini] PCIE: should not be here!!\n");
		while(1);
	}

	/* make sure CFG write successfully complete */

	loop = 0;
	do {
		ret = rossini_write_config(bus->number, devfn, where, size, val);
		pci_read_config(bus, devfn, where&~0x3, 4, &check);
		if (loop >= 1) {
			printk("[Rossini] where = 0x%08x, size = %d\n", where, size);
			printk("[Rossini] mask = 0x%08x, smask = 0x%08x, shift = %d\n", mask, smask, shift);
			printk("[Rossini] val = 0x%08x, check=0x%08x\n", val, check);
			printk("[Rossini] PCIE: CFG write retry (%d)!!\n", loop);
		}
		++loop;
	} while ((((check>>shift)&smask)&((mask>>shift)&smask)) != ((val&smask)&((mask>>shift)&smask)));

	return ret;

}

static struct pci_ops synop_pci_ops = {
	.read	= pci_read_config,
	.write	= pci_write_config,
};

static struct resource pci_io = {
	.name = "PCI io",
	.start = 0x6c000000,
	.end = 0x6c00ffff,
	.flags = IORESOURCE_IO,
};

static struct resource non_mem = {
	.name = "PCI non-prefetchable",
	.start = 0x68000000,
	.end = 0x69ffffff,
	.flags = IORESOURCE_MEM,
};

static struct resource pre_mem = {
	.name = "PCI prefetchable",
	.start = 0x6a000000,
	.end = 0x6bffffff,
	.flags = IORESOURCE_MEM | IORESOURCE_PREFETCH,
};

int __init synop_pci_setup_resource(int nr, struct pci_sys_data *sys_data)
{
	if (link_fail)
		return -EIO;

	if (request_resource(&ioport_resource, &pci_io)) {
		printk(KERN_ERR "PCI: unable to allocate io region\n");
		return -EBUSY;
	}

	if (request_resource(&iomem_resource, &non_mem)) {
		printk(KERN_ERR "PCI: unable to allocate non-prefetchable "
		       "memory region\n");
		return -EBUSY;
	}
	if (request_resource(&iomem_resource, &pre_mem)) {
		release_resource(&non_mem);
		printk(KERN_ERR "PCI: unable to allocate prefetchable "
		       "memory region\n");
		return -EBUSY;
	}

	/*
	 * bus->resource[0] is the IO resource for this bus
	 * bus->resource[1] is the mem resource for this bus
	 * bus->resource[2] is the prefetch mem resource for this bus
	 */
	sys_data->resource[0] = &pci_io;
	sys_data->resource[1] = &non_mem;
	sys_data->resource[2] = &pre_mem;

	return 1;
}

#define AHBC_PROFILE_CLR		0x108
#define AHBC_PCIEC_BANDWIDTH	0x10c
#define AHBC_PCIEC_RGINTERVAL	0x148
#define AHBC_PCIEC_REQTIMES		0x184

#if 0 // just for V2 on FPGA
#define AHBC0_MMR_VIRT_BASE		IO_ADDRESS(VPL_AHBC_2_MMR_BASE)
#else
#define AHBC0_MMR_VIRT_BASE		IO_ADDRESS(VPL_AHBC_0_MMR_BASE)
#endif
#define PCIEC_PROFILE_CLR		(1<<1)

#define CYCLE_TIME 				(1000000000/AHB_CLK) /* ns per AHB cycle */

#define WDT_BASE (IO_ADDRESS(ROSSINI_WDTC_MMR_BASE))
#define WDT_LOAD        (WDT_BASE+0x10)
#define WDT_RESTART     (WDT_BASE+0x18)
#define WDT_CR          (WDT_BASE+0x4)
#define WDT_RSTLEN      (WDT_BASE+0x1c)
#define WDT_RESTART_PASSWD 0x28791166
#define WDT_CR_ENABLE   0x4       /* WDT enable */

/*	workaround: make sure all data in PCI-e to AHB FIFO was sync-ed into DRAM (ie. no master activity) */
static irqreturn_t pci_intx_irq(int irq, void *devid)
{
	u32 req_time, rg_interval, poll_cycle, poll_time;
	int loop;

	/* reset and re-initial PCIE core when getting uncorrectable errors */
	if ((inl(VA_ROSSINI_PCI_DBI_BASE+0x100+PCI_ERR_ROOT_STATUS)&0x7f) != 0) { /* handle uncorrectable errors */
		printk("[Rossini] PCIE: Uncorrectable errors!!\n");
		printk("[Rossini] PCIE: error stat: 0x%08x\n", inl(VA_ROSSINI_PCI_DBI_BASE+0x100+PCI_ERR_UNCOR_STATUS));
		printk("[Rossini] PCIE: error sum: 0x%08x\n", inl(VA_ROSSINI_PCI_DBI_BASE+0x100+PCI_ERR_ROOT_STATUS));
		printk("[Rossini] system is going to reset...\n");
    	writel(0xffff, WDT_RSTLEN); //set reset signal length
	    writel(0x0, WDT_CR); //disable first
	    writel(10, WDT_LOAD); //count down from 10
	    writel(WDT_RESTART_PASSWD, WDT_RESTART); //load it to counter
	    writel(WDT_CR_ENABLE, WDT_CR); //count down start

		while(1);
	}

	req_time = inl(AHBC0_MMR_VIRT_BASE+AHBC_PCIEC_REQTIMES);
	rg_interval = inl(AHBC0_MMR_VIRT_BASE+AHBC_PCIEC_RGINTERVAL);
	poll_cycle = (req_time == 0) ? 30 : (rg_interval/req_time);
	poll_cycle *= 3;
	poll_time = poll_cycle*CYCLE_TIME;

	loop = 0;
	do {
		outl(PCIEC_PROFILE_CLR, AHBC0_MMR_VIRT_BASE+AHBC_PROFILE_CLR);
		ndelay(poll_time);
		++loop;
	} while(inl(AHBC0_MMR_VIRT_BASE+AHBC_PCIEC_REQTIMES) != 0);

	return IRQ_HANDLED;
}

#define POWERUP_TIMEOUT (5000)
#define LTSSM_TIMEOUT	(500000)

void __init synop_pci_pre_init(void)
{
	unsigned long flags;
	pdev_ids *pdev_id;
	u32 val;
	int loop, nodev;

	printk("Rossini PCI Express core pre-initializing\n");

	spin_lock_irqsave(&pci_lock, flags);

	if (gpio_request(PCIE_RESET, NULL) != 0) {
		printk("[Rossini] PCIE: request reset pin fail!!\n");
		link_fail = 1;
		spin_unlock_irqrestore(&pci_lock, flags);
		return;
	}

	outl(0x8, PCI_SSC_CR0); /* reset pcie phy */
	outl(0x0, PCI_SSC_CR0);

	gpio_direction_output(PCIE_RESET, 1); /* set reset pin direction out */
	gpio_set_value(PCIE_RESET, 0); /* drive reset pin low */

	outl((inl(PCI_SSC_CFG0)&~0x1f)|0x8, PCI_SSC_CFG0);
	outl(inl(PCI_SSC_CFG0)|0x1, PCI_SSC_CFG0);
	loop = 0;
	while ((inl(PCI_SSC_CFG0)&0x40000000) == 0) /* polling till PCIEC workable */
	{
		if (loop > POWERUP_TIMEOUT) {
			printk("[Rossini] PCIE: power up fail!!\n");
			link_fail = 1;
			spin_unlock_irqrestore(&pci_lock, flags);
			return;
		}

		++loop;
	}

	mdelay(100); /* fix bug #184 */
	gpio_set_value(PCIE_RESET, 1); /* drive reset pin high */
	outl(0x1, PCI_ELBI_CTRL);

	loop = 0;
	while ((inl(PCI_ELBI_CTRL) & 0x1f000000) != 0x11000000)  /* polling LTSSM state */
	{
		if (loop > LTSSM_TIMEOUT) {
			printk("[Rossini] PCIE: LTSSM fail!!\n");
			link_fail = 1;
			spin_unlock_irqrestore(&pci_lock, flags);
			return;
		}

		++loop;
	}

	outl(inl(VA_ROSSINI_PCI_DBI_BASE+0x4)|0x7, VA_ROSSINI_PCI_DBI_BASE+0x4); /* set bridge bus master, I/O, memory enable */
	outl(0x06040001, VA_ROSSINI_PCI_DBI_BASE+0x8); /* set class code to bridge type */

	outl(0, VA_ROSSINI_PCI_DBI_BASE+0x1000+0x10); /* disable BAR0 on bridge */
	outl(0, VA_ROSSINI_PCI_DBI_BASE+0x1000+0x14); /* disable BAR1 on bridge */

	val = inl(VA_ROSSINI_PCI_DBI_BASE+0x70+0xc); /* PCI Express capability, Link Capabilities Register */
	val &= ~(0x3<<10);
	val |= (0x1<<10);
	outl(val, VA_ROSSINI_PCI_DBI_BASE+0x70+0xc); /* disable support for L1 ASPM state */

	/* disable L1 ASPM state, but both L1 and L0s were disabled by default */
	/*
	val = inl(VA_ROSSINI_PCI_DBI_BASE+0x70+0x10);
	val &= ~0x3;
	val |= 0x1;
	outl(val, VA_ROSSINI_PCI_DBI_BASE+0x70+0x10);
	*/

	outl(1, VA_ROSSINI_PCI_DBI_BASE+0x1c); /* set I/O base/limit regs to support 32-bit I/O address */

	outl(inl(VA_ROSSINI_PCI_DBI_BASE+0x71c)|0x170000, VA_ROSSINI_PCI_DBI_BASE+0x71c);

	/* initial outbound translation rule */
	outl(0x68000000, VA_ROSSINI_PCI_ELBI_BASE+0x54);
	outl(0x68000000, VA_ROSSINI_PCI_ELBI_BASE+0x74);
	outl(0x69ffffff, VA_ROSSINI_PCI_ELBI_BASE+0x7c);

	outl(0x6a000000, VA_ROSSINI_PCI_ELBI_BASE+0x5c);
	outl(0x6a000000, VA_ROSSINI_PCI_ELBI_BASE+0x84);
	outl(0x6bffffff, VA_ROSSINI_PCI_ELBI_BASE+0x8c);

	outl(0x6c000000, VA_ROSSINI_PCI_ELBI_BASE+0x64);
	outl(0x6c000000, VA_ROSSINI_PCI_ELBI_BASE+0x94);
	outl(0x6c00ffff, VA_ROSSINI_PCI_ELBI_BASE+0x9c);

	outl(inl(VA_ROSSINI_PCI_DBI_BASE+0x100+PCI_ERR_UNCOR_SEVER)|0x4000, VA_ROSSINI_PCI_DBI_BASE+0x10c); /* Mark Completion Timeout as fatal */
	outl(0x4, VA_ROSSINI_PCI_DBI_BASE+0x100+PCI_ERR_ROOT_COMMAND); /* enable fatal error reporting */

/*	workaround: make sure all devices on bus are ready for being accessed */
	pdev_id = pdev_id_list;
	nodev = 1;
	while ((pdev_id->vendor_id != 0xffff) && (pdev_id->device_id != 0xffff))
	{
#define RETRY_IDENTIFY 1000
		loop = 0;
		do
		{
			val = 0;
			(void)rossini_read_config(pdev_id->bus, pdev_id->devfn, 0, 4, &val);
		} while (val != ((pdev_id->device_id<<16)|(pdev_id->vendor_id)) && ++loop < RETRY_IDENTIFY);

		if (loop < RETRY_IDENTIFY)
		{
			/*  workaround: initial configuration register map to check all CFG write value */
			rossini_pci_regmap_init(pdev_id);
			nodev = 0;
		}
		++pdev_id;
	}
	link_fail = nodev;

	spin_unlock_irqrestore(&pci_lock, flags);
}

void __init synop_pci_post_init(void)
{
	struct pci_dev *dev, *from;
	int ret;

	printk("Rossini PCI Express core post-initializing\n");


	pci_bridge = pci_get_device(0x0907, 0x0100, NULL);
	if (pci_bridge == NULL) {
		printk("[Rossini] Can't find PCI controller!\n");
		return;
	}

	dev = NULL;
	from = NULL;
	while ((dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, from)) != NULL) {
		int cap_offset;
		u32 val;

		from = dev;
/*		printk("\n\t%04x:%04x ", dev->vendor, dev->device); */
		cap_offset = pci_find_capability(dev, PCI_CAP_ID_EXP);
/*		printk("offset 0x%04x\n", cap_offset); */
		if (unlikely(cap_offset == 0)) {
			printk("[Rossini] non-pcie device!!\n");
			continue;
		}
		pci_read_config_dword(dev, cap_offset+PCI_EXP_DEVCTL, &val);
/*		printk("dev_ctl 0x%08x\n", val); */
		val &= ~0x70e0;
		pci_write_config_dword(dev, cap_offset+PCI_EXP_DEVCTL, val); /* set max payload size to 128 bytes */
/*		val = 0;
		pci_read_config_dword(dev, cap_offset+PCI_EXP_DEVCTL, &val);
		printk("dev_ctl 0x%08x\n", val); */

		pci_read_config_dword(dev, cap_offset+PCI_EXP_LNKCTL, &val);
		if ((val&0x3) != 0){
			val &= ~0x3;
			val |= 0x1;
			pci_write_config_dword(dev, cap_offset+PCI_EXP_LNKCTL, val); /* disable L1 ASPM state */
		}
	}

/*	workaround: make sure all data in PCI-e to AHB FIFO was sync-ed into DRAM (ie. no master activity) */
	ret = request_irq(PCIEC_IRQ_NUM, pci_intx_irq, IRQF_SHARED|IRQF_DISABLED, "PCI bus INTx", pci_bridge);
	if (ret) {
		printk("[Rossini] PCI INTx IRQ request fail\n");
		return;
	}

}

struct pci_bus * __init synop_scan_bus(int nr, struct pci_sys_data *sys)
{
	return pci_scan_bus(0, &synop_pci_ops, sys);
}

static u8 __init rossini_swizzle(struct pci_dev *dev, u8 *pinp)
{
	return 0;
}

static int __init rossini_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	return PCIEC_IRQ_NUM;
}

struct hw_pci rossini_pci __initdata = {
	.nr_controllers		= 1,
	.setup				= synop_pci_setup_resource,
	.preinit			= synop_pci_pre_init,
	.postinit			= synop_pci_post_init,
	.scan				= synop_scan_bus,
	.swizzle			= rossini_swizzle,
	.map_irq			= rossini_map_irq,
};


static int __init synop_pci_init(void)
{
	pci_common_init(&rossini_pci);

	return 0;
}

#ifdef CONFIG_PCI_MSI

#define MSI_IRQ_OFFSET 36
#define NR_MSI 8

static u8 rossini_msi_vector[8] = {0};
DEFINE_SPINLOCK(msi_vector_lock);

static u16 rossini_msi_msg_data[8] = {
	0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800
};

#define ROSSINI_MSI_ADDR_HI 0
#define ROSSINI_MSI_ADDR_LO (ROSSINI_INTC_MMR_BASE + 0x30)

static struct irq_chip msi_chip = {
	.name		= "PCI-MSI",
	.unmask		= rossini_msi_unmask_irq,
	.mask 		= rossini_mask_irq,
	.ack		= rossini_ack_irq,
	.retrigger	= rossini_retrigger_irq,
};

int arch_setup_msi_irq(struct pci_dev *dev, struct msi_desc *desc)
{
	struct msi_msg msg;
	unsigned int irq = create_irq();

	if ((irq >= (MSI_IRQ_OFFSET + NR_MSI)) || (irq < MSI_IRQ_OFFSET)) {
		return -EBUSY;
	}

	set_irq_msi(irq, desc);
	msg.address_hi = ROSSINI_MSI_ADDR_HI;
	msg.address_lo = ROSSINI_MSI_ADDR_LO;
	msg.data = (u32) rossini_msi_msg_data[irq - MSI_IRQ_OFFSET];


	write_msi_msg(irq, &msg);

	fLib_SetIntTrig(irq, EDGE, 0);

	set_irq_chip_and_handler_name(irq, &msi_chip, handle_edge_irq,
                      "edge");

	return 0;
}

void arch_teardown_msi_irq(unsigned int irq)
{
	destroy_irq(irq);
}

int create_irq(void)
{
	int irq;
	unsigned long flags;

	spin_lock_irqsave(&msi_vector_lock, flags);
	for (irq = (NR_MSI - 1); irq >= 0; --irq) {
		if (rossini_msi_vector[irq] == 0) {
			rossini_msi_vector[irq] = 1;
			break;
		}
	}
	spin_unlock_irqrestore(&msi_vector_lock, flags);

	if (irq < 0) {
		printk("Unable to allocate MSI\n");
		return irq;
	}

	return irq + MSI_IRQ_OFFSET;
}

void destroy_irq(unsigned int irq)
{
	unsigned long flags;

	spin_lock_irqsave(&msi_vector_lock, flags);
	rossini_msi_vector[irq - MSI_IRQ_OFFSET] = 0;
	spin_unlock_irqrestore(&msi_vector_lock, flags);

	return;
}

#endif

subsys_initcall(synop_pci_init);
