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
	#define VA_TMRC_MMR_BASE      IO_ADDRESS(VPL_TMRC_MMR_BASE)
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

//[jam trace] We get usec via this function
//usec_in_64timer defined in kernel/timer.c
#if (AHB_CLK == 200000000)
unsigned long VPL_TMRC_gettimeoffset(void)
{
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))
    u64 current_usec, last_usec ;
    UINT32 current_usec_lo, check_usec_lo;
	u64 current_usec_hi;
    UINT32 offset_usec ;

    //usec : Get last usec
    last_usec = usec_in_64timer ;
    //usec : Get current usec
	while(1) {
    	current_usec_lo = v_inl(IO_ADDRESS((VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_LO_DATA))) ;
    	current_usec_hi = v_inl(IO_ADDRESS((VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_HI_DATA))) ;
    	check_usec_lo = v_inl(IO_ADDRESS((VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_LO_DATA))) ;
		if(check_usec_lo > current_usec_lo)
			break;
	}

	current_usec = current_usec_hi << 32 | current_usec_lo;

    //usec : Get the offset of the usec since last tick
    offset_usec = current_usec - last_usec ;

    return offset_usec ;
}

#elif (AHB_CLK == 166666666)
u64 last_jiffies64 = 0 ;
u64 last_cyclenum = 0 ;
unsigned long last_xtimensec = 0 ;
#define APBCLKNUM_TO_1USEC  (APB_CLK / 1000000)
#define LOW_PART_OVERFLOW_TIME_166 0xffffffff
unsigned long VPL_TMRC_gettimeoffset(void)
{
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))
    u64 current_usec, last_usec ;
    //u64 last_jiffies_64, current_jiffies_64 ;
    u64 diff_usec ;
    UINT32 offset_usec ;
	static int ever_exceed = 0 ;

	//jam add
	if((last_jiffies64 != jiffies_64) &&
	   ((last_xtimensec / 1000) < (xtime.tv_nsec / 1000))
	  ) {
		last_jiffies64 = jiffies_64 ;
		last_cyclenum = cyclenum_in_64timer ;
		//restart
		ever_exceed = 0 ;
	}
	last_usec = last_cyclenum ;
	last_xtimensec = xtime.tv_nsec ;

    //usec : Get current usec
    current_usec = v_inl(IO_ADDRESS(VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_HI_DATA)) ;
    current_usec <<= 32 ;
    current_usec = current_usec | v_inl(IO_ADDRESS(VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_LO_DATA)) ;

    if((current_usec - last_usec) >= LOW_PART_OVERFLOW_TIME_166) {
        current_usec = current_usec - LOW_PART_OVERFLOW_TIME_166 ;
    }

    diff_usec = current_usec - last_usec ;

    //APBCLK number -> 1 usec
    do_div(diff_usec, APBCLKNUM_TO_1USEC) ;

    offset_usec = (UINT32)diff_usec ;

    //Because this func should only return 0~9999 usecs
	if((offset_usec >= 10000) || (ever_exceed)) {
		offset_usec = 9999 ;
		ever_exceed = 1 ;
	}

	//tv_nsec is sometimes during 10000001 ~ 19999999!!(But in normal case, it should always be multiple 10msecs)
	unsigned long last_xtimeusec = last_xtimensec / 1000;//1// get usec amount from last xtime nsec
	if((last_xtimeusec % 10000) != 0) {//transform to the amount of 10msec unit
	    //find next "10msec"
		unsigned long next_nsec = (last_xtimensec / 1000) ;
		next_nsec /= 10000 ;
		next_nsec++ ;
		next_nsec *= 10000 ;// 10msec
		//get the offset between "current xtime usec" to "next xtime usec"
		offset_usec = next_nsec - last_xtimeusec ;
	}

    return offset_usec ;

}

#elif (AHB_CLK == 133333333)
u64 last_jiffies64 = 0 ;
u64 last_cyclenum = 0 ;
unsigned long last_xtimensec = 0 ;
#define APBCLKNUM_TO_1USEC  (APB_CLK / 1000000)
#define LOW_PART_OVERFLOW_TIME_133 0xffffffff
unsigned long VPL_TMRC_gettimeoffset(void)
{
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))
    u64 current_usec, last_usec ;
    //u64 last_jiffies_64, current_jiffies_64 ;
    u64 diff_usec ;
    UINT32 offset_usec ;
	static int ever_exceed = 0 ;

	//jam add
	if((last_jiffies64 != jiffies_64) &&
	   ((last_xtimensec / 1000) < (xtime.tv_nsec / 1000))
	  ) {
		last_jiffies64 = jiffies_64 ;
		last_cyclenum = cyclenum_in_64timer ;
		//restart
		ever_exceed = 0 ;
	}
	last_usec = last_cyclenum ;
	last_xtimensec = xtime.tv_nsec ;

    //usec : Get current usec
    current_usec = v_inl(IO_ADDRESS(VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_HI_DATA)) ;
    current_usec <<= 32 ;
    current_usec = current_usec | v_inl(IO_ADDRESS(VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_LO_DATA)) ;

    if((current_usec - last_usec) >= LOW_PART_OVERFLOW_TIME_133) {
        current_usec = current_usec - LOW_PART_OVERFLOW_TIME_133 ;
    }

    diff_usec = current_usec - last_usec ;

    //APBCLK number -> 1 usec
    do_div(diff_usec, APBCLKNUM_TO_1USEC) ;

    offset_usec = (UINT32)diff_usec ;

    //Because this func should only return 0~9999 usecs
	if((offset_usec >= 10000) || (ever_exceed)) {
		offset_usec = 9999 ;
		ever_exceed = 1 ;
	}

	//tv_nsec is sometimes during 10000001 ~ 19999999!!(But in normal case, it should always be multiple 10msecs)
	unsigned long last_xtimeusec = last_xtimensec / 1000;//1// get usec amount from last xtime nsec
	if((last_xtimeusec % 10000) != 0) {//transform to the amount of 10msec unit
	    //find next "10msec"
		unsigned long next_nsec = (last_xtimensec / 1000) ;
		next_nsec /= 10000 ;
		next_nsec++ ;
		next_nsec *= 10000 ;// 10msec
		//get the offset between "current xtime usec" to "next xtime usec"
		offset_usec = next_nsec - last_xtimeusec ;
	}

    return offset_usec ;

}

//#elif ((AHB_CLK == 150000000) || (AHB_CLK == 160000000))
#else
#define APBCLKNUM_TO_1USEC  (APB_CLK / 1000000)
#define v_inl(addr)	(*((volatile unsigned long *)(addr)))
unsigned long VPL_TMRC_gettimeoffset(void)
{
    u64 current_64timer_val, last_64timer_val ;
	u64 time_offset ;

	unsigned long hi_data, lo_data_1stread, lo_data_2ndread ;
    unsigned long lo_reg = VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_LO_DATA ;
    unsigned long hi_reg = VPL_SYSC_MMR_BASE + VPL_SYSC_CNT_HI_DATA ;

	last_64timer_val = cyclenum_in_64timer ;

	//Read the value of 64 timer
    while(1) {
	   	lo_data_1stread = v_inl(IO_ADDRESS(lo_reg)) ;
    	hi_data = v_inl(IO_ADDRESS(hi_reg)) ;
		lo_data_2ndread = v_inl(IO_ADDRESS(lo_reg)) ;

		//avoid overflow
		if(lo_data_1stread < lo_data_2ndread)
			break ;
    }

	//assemble the 64-bits data
	current_64timer_val = hi_data ;
    current_64timer_val = current_64timer_val << 32 ;
	current_64timer_val = current_64timer_val | lo_data_1stread ;

	//get the usec difference
	time_offset = current_64timer_val - last_64timer_val ;
	do_div(time_offset, APBCLKNUM_TO_1USEC) ;

    return (unsigned long)time_offset ;

}
#endif
