/*
 * time.c  Timer functions for VPL TMRC
 */

#include <linux/time.h>
#include <linux/timex.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <mach/time.h>


//#define DEBUG_TIMER
#ifdef DEBUG_TIMER
#    define PDEBUG(fmt, args...) printk(fmt, ## args)
#else
#    define PDEBUG(fmt, args...)
#endif
// --------------------------------------------------------------------
//              functions copy from flib (timer.c)
// --------------------------------------------------------------------
#define FALSE		0
#define TRUE    	1

#ifndef VA_TMRC_MMR_BASE
	#define VA_TMRC_MMR_BASE      IO_ADDRESS(TMRC_MMR_BASE)
#endif

volatile VPL_TMRC *tmrc = (VPL_TMRC *)VA_TMRC_MMR_BASE;

static INT32 __inline__ TMRC_CheckTimer(UINT32 timer)
{
	if (timer > MAX_TIMER)
		return FALSE;
	return TRUE;
}

INT32 TMRC_Ctrl_Config(UINT32 timer, UINT32 set_down_count)
{
	if (TMRC_CheckTimer(timer)!=TRUE)
		return FALSE;

	// configure the timer to down- or up-count.
	set_down_count = (set_down_count) ? 1 : 0;
	set_down_count = set_down_count<<(3+timer*4);
	PDEBUG("TMR_CTRL: %08lx\n",tmrc->TmrCtrl);
	tmrc->TmrCtrl = (tmrc->TmrCtrl & ~(1<<(3+timer*4))) | set_down_count; 
	PDEBUG("TMR_CTRL: %08lx\n",tmrc->TmrCtrl);
	return TRUE;
}

INT32 TMRC_Enable(UINT32 timer)
{
	UINT32 enable;

	if (TMRC_CheckTimer(timer)!=TRUE)
		return FALSE;

	enable = 0x7<<(0+timer*4);
	tmrc->TmrCtrl = tmrc->TmrCtrl | enable; 
	return TRUE;
}

/* This routine stops the specified timer hardware. */
INT32 TMRC_Disable(UINT32 timer)
{
	UINT32 enable;

	if (TMRC_CheckTimer(timer)!=TRUE)
		return FALSE;

	enable = 0x7<<(0+timer*4);
	tmrc->TmrCtrl = tmrc->TmrCtrl & ~enable; 
	return TRUE;
}

INT32 TMRC_InterruptDisable(UINT32 timer)
{
	UINT32 enable;
	unsigned long dwReg;

	if (TMRC_CheckTimer(timer)!=TRUE)
		return FALSE;

	enable = 0x6<<(0+timer*4);
	dwReg = readl(&(tmrc->TmrCtrl)) & ~enable; 
	writel(dwReg, &(tmrc->TmrCtrl)); 
	return TRUE;
}

INT32 TMRC_SetCounter(UINT32 timer, UINT32 value)
{
	if (TMRC_CheckTimer(timer)!=TRUE)
		return FALSE;

	tmrc->TmrReg[timer].TmrCount = value;
	return TRUE;
}

INT32 TMRC_SetAutoReload(UINT32 timer, UINT32 value)
{
	if (TMRC_CheckTimer(timer)!=TRUE)
		return FALSE;

	tmrc->TmrReg[timer].TmrAutoReload = value;
	return TRUE;
}

UINT32 TMRC_ReadCounter(UINT32 timer)
{
	return tmrc->TmrReg[timer].TmrCount;
}

unsigned long VPL_TMRC_gettimeoffset(void)
{
	UINT32 dwCount;

	dwCount = *(UINT32 *)(IO_ADDRESS(HAYDN_TMRC_MMR_BASE+0x24));

/* use variable apb_clk instead of APB_CLK, aren 20060209 */
	dwCount = APB_CLK/HZ - dwCount;

	/* return u-sec */
/* use variable apb_clk instead of APB_CLK, aren 20060209 */
	return dwCount*1000/(APB_CLK/(HZ*10));

}

