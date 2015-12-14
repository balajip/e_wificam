/* Tipster PCI/Ethernet test chip driver for OS Linux
 *
 * System-dependent register access functions
 *
 * 1 Jun 1999 - G.Kazakov.
 */

#ifndef TIPSTER_REG_INCLUDED
#define TIPSTER_REG_INCLUDED

#define TIPSTER_MEMORY_CSRS  /* if Tipster chip CSRs are mapped to memory address space address Base */
#define xTIPSTER_NO_CSRS      /* NO CSRs!!! (debugging without board available...) */

#ifdef TIPSTER_MEMORY_CSRS

#include <asm/io.h>

#define TR tipster_trace
void tipster_trace(char *format, ...);

static u32 __inline__ TipsterReadConfigReg( Tipster *Dev, u32 Reg )
{
  u32 data = readl(Dev->configBase+Reg);
  TR( KERN_DEBUG "TipsterReadConfigReg(%02x)=%08x\n", Reg, data );
  return data;
}

static void __inline__ TipsterWriteConfigReg( Tipster *Dev, u32 Reg, u32 Data )
{
  TR( KERN_DEBUG "TipsterWriteConfigReg(%02x,%08x)\n", Reg, Data );
  writel(Data,Dev->configBase+Reg);
}

static void __inline__ TipsterSetConfigReg( Tipster *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->configBase+Reg;
  u32 data = readl(addr);
  data |= Data;
  TR( KERN_DEBUG "TipsterSetConfigReg(%02x,%08x)=%08x\n", Reg, Data, data );
  writel(data,addr);
}

static void __inline__ TipsterClearConfigReg( Tipster *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->configBase+Reg;
  u32 data = readl(addr);
  data &= ~Data;
  TR( KERN_DEBUG "TipsterClearConfigReg(%02x,%08x)=%08x\n", Reg, Data, data );
  writel(data,addr);
}

static u32 __inline__ TipsterReadMacReg( Tipster *Dev, u32 Reg )
{
  u32 data = readl(Dev->macBase+Reg);
  TR( KERN_DEBUG "TipsterReadMacReg(%02x)=%08x\n", Reg, data );
  return data;
}

static void __inline__ TipsterWriteMacReg( Tipster *Dev, u32 Reg, u32 Data )
{
  TR( KERN_DEBUG "TipsterWriteMacReg(%02x,%08x)\n", Reg, Data );
  writel(Data,Dev->macBase+Reg);
}

static void __inline__ TipsterSetMacReg( Tipster *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->macBase+Reg;
  u32 data = readl(addr);
  data |= Data;
  TR( KERN_DEBUG "TipsterSetMacReg(%02x,%08x)=%08x\n", Reg, Data, data );
  writel(data,addr);
}

static void __inline__ TipsterClearMacReg( Tipster *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->macBase+Reg;
  u32 data = readl(addr);
  data &= ~Data;
  TR( KERN_DEBUG "TipsterClearMacReg(%02x,%08x)=%08x\n", Reg, Data, data );
  writel(data,addr);
}

static u32 __inline__ TipsterReadDmaReg( Tipster *Dev, u32 Reg )
{
  u32 data = readl(Dev->dmaBase+Reg);
  //printk( "TipsterReadDmaReg(%02x)=%08x\n", Reg, data );
  return data;
}

static void __inline__ TipsterWriteDmaReg( Tipster *Dev, u32 Reg, u32 Data )
{
  TR( KERN_DEBUG "TipsterWriteDmaReg(%02x,%08x)\n", Reg, Data );
  writel(Data,Dev->dmaBase+Reg);
}

static void __inline__ TipsterSetDmaReg( Tipster *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->dmaBase+Reg;
  u32 data = readl(addr);
  data |= Data;
  TR( KERN_DEBUG "TipsterSetDmaReg(%02x,%08x)=%08x\n", Reg, Data, data );
  writel(data,addr);
}

static void __inline__ TipsterClearDmaReg( Tipster *Dev, u32 Reg, u32 Data )
{
  u32 addr = Dev->dmaBase+Reg;
  u32 data = readl(addr);
  data &= ~Data;
  TR( KERN_DEBUG "TipsterClearDmaReg(%02x,%08x)=%08x\n", Reg, Data, data );
  writel(data,addr);
}

#endif /* TIPSTER_MEMORY_CSRS */

#ifdef TIPSTER_NO_CSRS

static u32 __inline__ TipsterReadConfigReg( Tipster *Dev, u32 Reg ) { return 0; }
static void __inline__ TipsterWriteConfigReg( Tipster *Dev, u32 Reg, u32 Data ) {}
static void __inline__ TipsterSetConfigReg( Tipster *Dev, u32 Reg, u32 Data ) {}
static void __inline__ TipsterClearConfigReg( Tipster *Dev, u32 Reg, u32 Data ) {}

static u32 __inline__ TipsterReadMacReg( Tipster *Dev, u32 Reg ) { return 0; }
static void __inline__ TipsterWriteMacReg( Tipster *Dev, u32 Reg, u32 Data ) {}
static void __inline__ TipsterSetMacReg( Tipster *Dev, u32 Reg, u32 Data ) {}
static void __inline__ TipsterClearMacReg( Tipster *Dev, u32 Reg, u32 Data ) {}

static u32 __inline__ TipsterReadDmaReg( Tipster *Dev, u32 Reg ) { return 0; }
static void __inline__ TipsterWriteDmaReg( Tipster *Dev, u32 Reg, u32 Data ) {}
static void __inline__ TipsterSetDmaReg( Tipster *Dev, u32 Reg, u32 Data ) {}
static void __inline__ TipsterClearDmaReg( Tipster *Dev, u32 Reg, u32 Data ) {}

#endif /* TIPSTER_NO_CSRS */

#endif /* TIPSTER_REG_INCLUDED */

