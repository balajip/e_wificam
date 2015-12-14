#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>	// aren 20040712

#include <asm/mach/irq.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <mach/irq.h>
#include <asm/system.h>
#include <mach/platform.h>
//#include <asm/arch/rossini/chipset.h>
//#include <asm/arch/rossini/rossini.h>
//#include <asm/arch/rossini/symbol.h>


void MaskIRQ(unsigned int IRQ);
void UnmaskIRQ(unsigned int IRQ);
void SetIRQmode(unsigned int IRQ, unsigned int edge);
void SetIRQlevel(unsigned int IRQ, unsigned int low);


#define IO_BASE			0xF0000000	// VA of IO
/* macro to get at IO space when running virtually */
#define IO_ADDRESS(x) (((x >> 4)&0xffff0000) + (x & 0xffff) + IO_BASE)

#ifndef VA_ROSSINI_INTC_MMR_BASE
#define VA_ROSSINI_INTC_MMR_BASE	IO_ADDRESS(ROSSINI_INTC_MMR_BASE)
#endif


// --------------------------------------------------------------------
//              functions copy from flib
// --------------------------------------------------------------------
void SetIRQmode(unsigned int IRQ, unsigned int edge)
{
	volatile unsigned int *IRQBase;


	IRQBase = (unsigned int *) VA_ROSSINI_INTC_MMR_BASE;

	if (IRQ < 32){
		if (edge)
			IRQBase[(IRQ_MODE_LO_REG / sizeof(unsigned int))] |=
		    	(1 << IRQ);
		else
			IRQBase[(IRQ_MODE_LO_REG / sizeof(unsigned int))] &=
		    	~(1 << IRQ);
	}else{

		IRQ -= 32;

		if (edge)
			IRQBase[(IRQ_MODE_HI_REG / sizeof(unsigned int))] |=
		    	(1 << IRQ);
		else
			IRQBase[(IRQ_MODE_HI_REG / sizeof(unsigned int))] &=
		    	~(1 << IRQ);

	}
}

#if 0
void SetIRQlevel(unsigned int IRQ, unsigned int low)
{

	volatile unsigned int *IRQBase;

	IRQBase = (unsigned int *) VA_ROSSINI_INTC_MMR_BASE;

	if (low)
		IRQBase[(IRQ_LEVEL_REG / sizeof(unsigned int))] |=
		    (1 << IRQ);
	else
		IRQBase[(IRQ_LEVEL_REG / sizeof(unsigned int))] &=
		    ~(1 << IRQ);

}


void SetFIQmode(unsigned int FIQ, unsigned int edge)
{

	volatile unsigned int *FIQBase;


	FIQBase = (unsigned int *) VA_ROSSINI_INTC_MMR_BASE;

	if (edge)
		FIQBase[(FIQ_MODE_REG / sizeof(unsigned int))] |=
		    (1 << FIQ);
	else
		FIQBase[(FIQ_MODE_REG / sizeof(unsigned int))] &=
		    ~(1 << FIQ);

}


void SetFIQlevel(unsigned int FIQ, unsigned int low)
{

	volatile unsigned int *FIQBase;


	FIQBase = (unsigned int *) VA_ROSSINI_INTC_MMR_BASE;

	if (low)
		FIQBase[(FIQ_LEVEL_REG / sizeof(unsigned int))] |=
		    (1 << FIQ);
	else
		FIQBase[(FIQ_LEVEL_REG / sizeof(unsigned int))] &=
		    ~(1 << FIQ);

}


#endif

void fLib_SetIntTrig(u32 intNum, int intMode, int intLevel)
{
	SetIRQmode(intNum, intMode);
}



inline void ClearIRQStatus(u32 irq)
{
	if (irq < 32){
		outl(1 << irq, VA_ROSSINI_INTC_MMR_BASE + IRQ_CLEAR_LO_REG);
	}else{
		irq -= 32;
		outl(1 << irq, VA_ROSSINI_INTC_MMR_BASE + IRQ_CLEAR_HI_REG);
	}
}
#if 0
inline void ClearFIQStatus(u32 irq)
{
	outl(1 << irq, VA_ROSSINI_INTC_MMR_BASE + FIQ_CLEAR_REG);
}
#endif

inline void MaskIRQ(unsigned int irq)
{
	if (irq < 32){
		outl(inl(VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_LO_REG) & ~(1 << irq),
	     	VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_LO_REG);
	}else{
		irq -= 32;
		outl(inl(VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_HI_REG) & ~(1 << irq),
	     	VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_HI_REG);
	}
}

#if 0
inline void MaskFIQ(unsigned int irq)
{
	outl(inl(VA_ROSSINI_INTC_MMR_BASE + FIQ_MASK_REG) & ~(1 << irq),
	     VA_ROSSINI_INTC_MMR_BASE + FIQ_MASK_REG);
}
#endif

/*  Turn the interrupt source on. */
inline void UnmaskIRQ(unsigned int irq)
{
	//printk("[jon]enable IRQ %d\n", irq);
	if (irq < 32){
		outl(inl(VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_LO_REG) | (1 << irq),
	     	VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_LO_REG);
	}else{
		irq -= 32;
		outl(inl(VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_HI_REG) | (1 << irq),
	     	VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_HI_REG);
	}
}
#if 0
inline void UnmaskFIQ(unsigned int irq)
{
	outl(inl(VA_ROSSINI_INTC_MMR_BASE + FIQ_MASK_REG) | (1 << irq),
	     VA_ROSSINI_INTC_MMR_BASE + FIQ_MASK_REG);
}
#endif



void rossini_mask_irq(unsigned int irq)
{
	//debug_puts("rossini_mask_irq()\n");

	MaskIRQ(irq);

}



void rossini_unmask_irq(unsigned int irq)
{
	//debug_puts("rossini_unmask_irq()\n");

	ClearIRQStatus(irq);
	UnmaskIRQ(irq);

}



void rossini_mask_ack_irq(unsigned int irq)
{
	//debug_puts("rossini_mask_ack_irq()\n");
	MaskIRQ(irq);

}


/* for MSI support */
void rossini_msi_unmask_irq(unsigned int irq)
{
    UnmaskIRQ(irq);
}



void rossini_ack_irq(unsigned int irq)
{
    ClearIRQStatus(irq);
}



int rossini_retrigger_irq(unsigned int irq)
{
    if (likely(irq > 31)) {
        outl(1<<(irq - 32), VA_ROSSINI_INTC_MMR_BASE + IRQ_SET_HI_REG);
    } else {
        outl(1<<irq, VA_ROSSINI_INTC_MMR_BASE + IRQ_SET_LO_REG);
    }

    return 1;
}



void rossini_int_init(void)
{
	outl(0, VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_LO_REG);
	outl(0, VA_ROSSINI_INTC_MMR_BASE + IRQ_MASK_HI_REG);
	outl(0xffffffff, VA_ROSSINI_INTC_MMR_BASE + IRQ_CLEAR_LO_REG);
	outl(0xffffffff, VA_ROSSINI_INTC_MMR_BASE + IRQ_CLEAR_HI_REG);
}


EXPORT_SYMBOL(fLib_SetIntTrig);
EXPORT_SYMBOL(ClearIRQStatus);
