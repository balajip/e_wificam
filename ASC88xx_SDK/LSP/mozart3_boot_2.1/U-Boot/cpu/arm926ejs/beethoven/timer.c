/*
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian.pop@leadtechdesign.com>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/hardware.h>
#include <asm/arch/clk.h>
#include <asm/arch/io.h>
#include <div64.h>

#include <asm/arch/platform.h>

VPL_TimerReg *TimerBase[] ={(VPL_TimerReg *)EVM_TIMER_CTRL_BASE,
                            (VPL_TimerReg *)(EVM_TIMER_CTRL_BASE + sizeof(VPL_TimerReg)),
                            (VPL_TimerReg *)(EVM_TIMER_CTRL_BASE + sizeof(VPL_TimerReg)*2)};

volatile static unsigned long long timestamp;
static ulong lastdec;
static ulong timer_freq;

UINT32 Read_Timer_Counter(UINT32 timer)
{
    volatile VPL_TimerReg *Timer = TimerBase[timer];    

    return Timer->TimerValue;
}

static inline unsigned long long tick_to_time(unsigned long long tick)
{
	tick *= CONFIG_SYS_HZ;
	do_div(tick, timer_freq);

	return tick;
}

static inline unsigned long long usec_to_tick(unsigned long long usec)
{
	usec *= timer_freq;
	do_div(usec, 1000000);

	return usec;
}

void Set_Timer_AutoReloadValue(UINT32 timer, UINT32 value)
{
    volatile VPL_TimerReg *Timer = TimerBase[timer];

    Timer->TimerLoad = value;
}

/* nothing really to do with interrupts, just starts up a counter. */
int timer_init(void)
{
	VPL_TimerControl TimerControl;

	*(ulong *)&TimerControl = 0;
	TimerControl.TM0_EN = 1;
	TimerControl.TM0_CNT_DIR = 1; // 1:decrementing
	*(volatile ulong *)(EVM_TIMER_BASE + 0x04) = *(ulong *)&TimerControl;

	Set_Timer_AutoReloadValue(0, TIMER_LOAD_VAL);

    timer_freq = APB_CLOCK ;

   	return 0;
}

/*
 * timer without interrupts
 */
unsigned long long get_ticks(void)
{
	ulong now = Read_Timer_Counter(0);;

    if (lastdec >= now)
    {
        /* normal mode */
        timestamp += lastdec - now;
    } else {
        /* we have an overflow ... */
        timestamp += lastdec + TIMER_LOAD_VAL - now;
    }
    lastdec = now;

//	printf("time:%x ",now);	// aren 20040614
    return timestamp;
}



void reset_timer_masked(void)
{
	/* reset time */
    lastdec = Read_Timer_Counter(0);/* capture current descrementer value time */
    timestamp = 0;
}

ulong get_timer_masked(void)
{
	return tick_to_time(get_ticks());
}

void udelay(unsigned long usec)
{
	unsigned long long tmp;
	ulong tmo;

	tmo = usec_to_tick(usec);
	tmp = get_ticks() + tmo;	/* get current timestamp */

	while (get_ticks() < tmp)	/* loop till event */
		 /*NOP*/;
}

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer(ulong base)
{
	return get_timer_masked () - base;
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}

/*
 * Reset the cpu by setting up the watchdog timer and let him time out.
 */
#define WDT_RELOAD_VALUE          0x300  
void reset_cpu(ulong ignored)
{
    int timer = 0 ;
    
    v_outl(EVM_WDTC_BASE + EVM_WDT_CTRL, 0x0 ) ;                   
	v_outl(EVM_WDTC_BASE + EVM_WDT_RELOAD_VALUE, WDT_RELOAD_VALUE ); 
	v_outl(EVM_WDTC_BASE + EVM_WDT_MATCH_VALUE, 0x0 ) ;             
    v_outl(EVM_WDTC_BASE + EVM_WDT_RELOAD_CTRL, WDT_RELOAD_PASSWD ); 
	v_outl(EVM_WDTC_BASE + EVM_WDT_CTRL, 0x7 );//bit#2 : enable/disable WDTC ; bit#1 : enable/disable match ack(interrupt)

    while (timer < 100000) {
        timer++ ;
    }
    printf("Timeout!! - WDT reset failed!\n") ;
}
