#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>
#include <asm/io.h>

#define PCIESSC_CFG_0              (0x4)
#define PCIESSC_CR_0               (0xC)
#define PCIEC_ELBI_CTRL            (0x40000)
#define PCIEC_OUTBOUND_CTRL        (0x40050)
#define PCIEC_AHB_SLAVE_SIDEBAND   (0x400D4)

static int start_link_training(void)
{
	u32 reg;
	int timeout = 0;

	reg = v_inl(VPL_PCIEC_SSC_BASE + PCIESSC_CR_0);
	reg |= 0x8;
	v_outl(VPL_PCIEC_SSC_BASE + PCIESSC_CR_0, reg);
	udelay(1000);
	reg &= ~0x8;
	v_outl(VPL_PCIEC_SSC_BASE + PCIESSC_CR_0, reg); /* reset pcie phy */

	reg = v_inl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_OUT);
	reg &= ~0x40000;
	v_outl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_OUT, reg); /* drive GPIO18 low */	

	reg = v_inl(VPL_GPIOC_MMR_BASE + GPIOC_PIN_DIR);
	reg |= 0x40000; 
	v_outl(VPL_GPIOC_MMR_BASE + GPIOC_PIN_DIR, reg); /* set GPIO18 output */

	reg = v_inl(VPL_PCIEC_SSC_BASE + PCIESSC_CFG_0);
	reg &= ~0x1f;
	reg |= 0x8; /* set as root complex port */
	v_outl(VPL_PCIEC_SSC_BASE + PCIESSC_CFG_0, reg);
	reg |= 0x1; /* config done */
	v_outl(VPL_PCIEC_SSC_BASE + PCIESSC_CFG_0, reg);

	do {
		udelay(100);
		reg = v_inl(VPL_PCIEC_SSC_BASE + PCIESSC_CFG_0);
	} while(!(reg&0x40000000) && (++timeout)<=500);	
	if (timeout > 500) {
		printf("\tPCIEC workable timeout!!\n");
		printf("\tPossibly external clock source issue!!\n");

		return -1;
	}

	v_outl(VPL_PCIEC_MMR_BASE + PCIEC_ELBI_CTRL, 0x1); /* enable LTSSM */
	
	reg = v_inl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_OUT);
	reg |= 0x40000;
	v_outl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_OUT, reg); /* drive GPIO18 high */	

	timeout = 0;
	do {
		udelay(100);
		reg = v_inl(VPL_PCIEC_MMR_BASE + PCIEC_ELBI_CTRL);
	} while((reg&0x1f000000)!=0x11000000 && (++timeout)<=500);
	if (timeout > 500) {
		printf("\tPCIEC LTSSM timeout!!\n");
		
		return -1;
	}

	return 0;
}

int pcie_test_func(void)
{
	int ret;

	printf( "pcie_test_func!!!\n" ) ;

	ret = start_link_training();
	if (ret != 0)
	{
		printf("\tlink training fail!\n");
		return ret;
	}

	/* read Vendor ID, Device ID */
	writel(0x201, VPL_PCIEC_MMR_BASE + PCIEC_OUTBOUND_CTRL);   // OUTBOUND CTRL: [16:9] Cfg0 bus number 0x1, [8:4] device 0x0, Enable outbound address translation
	writel(0x00f00004, VPL_PCIEC_MMR_BASE + PCIEC_AHB_SLAVE_SIDEBAND);   // AHB SLAVE SIDEBAND: Cfg0 TLP
	ret = readl(VPL_PCIEC_MEM_BASE + 0x700000);   // [19:16] function / [11:8] ext. reg / [7:2] reg;
	printf("Vendor ID 0x%04X, Device ID 0x%04X\n", ret & 0xFFFF, ret >> 16);

	return 0;
}

