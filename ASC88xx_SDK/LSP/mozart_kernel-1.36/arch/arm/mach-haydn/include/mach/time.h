/*
 * linux/include/asm/arch-haydn/time.h
 */

// --------------------------------------------------------------------
#ifndef __ASM_ARCH_TIME_H__
#define __ASM_ARCH_TIME_H__

// --------------------------------------------------------------------
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <mach/timex.h>
#include <mach/haydn.h>
#include <mach/interrupt.h>
#include <mach/irq.h>

// --------------------------------------------------------------------
#ifndef UINT32
	#define UINT32		unsigned int
#endif
#ifndef INT32
	#define INT32		int
#endif
#ifndef FALSE
	#define FLASE	0
#endif
#ifndef TRUE
	#define TRUE	1
#endif

typedef volatile struct {
	UINT32 Tmr0En:1;	// Timer0 enable bit
	UINT32 Tmr0OfEn:1;	// Timer0 over flow interrupt enable bit
	UINT32 Tmr0MatchEn:1;	// Timer0 Match interrupt enable
	UINT32 Tmr0CntDir:1;	// Timer0 direction (0: increment, 1: decrement)
	UINT32 Tmr1En:1;	// Timer1 enable bit
	UINT32 Tmr1OfEn:1;	// Timer1 over flow interrupt enable bit
	UINT32 Tmr1MatchEn:1;	// Timer1 Match interrupt enable
	UINT32 Tmr1CntDir:1;	// Timer1 direction (0: increment, 1: decrement)
	UINT32 Tmr2En:1;	// Timer2 enable bit
	UINT32 Tmr2OfEn:1;	// Timer2 over flow interrupt enable bit
	UINT32 Tmr2MatchEn:1;	// Timer2 Match interrupt enable
	UINT32 Tmr2CntDir:1;	// Timer2 direction (0: increment, 1: decrement)
	UINT32 Tmr3En:1;	// Timer3 enable bit
	UINT32 Tmr3OfEn:1;	// Timer3 over flow interrupt enable bit
	UINT32 Tmr3MatchEn:1;	// Timer3 Match interrupt enable
	UINT32 Tmr3CntDir:1;	// Timer3 direction (0: increment, 1: decrement)
	UINT32 Tmr4En:1;	// Timer4 enable bit
	UINT32 Tmr4OfEn:1;	// Timer4 over flow interrupt enable bit
	UINT32 Tmr4MatchEn:1;	// Timer4 Match interrupt enable
	UINT32 Tmr4CntDir:1;	// Timer4 direction (0: increment, 1: decrement)
	UINT32 Tmr5En:1;	// Timer5 enable bit
	UINT32 Tmr5OfEn:1;	// Timer5 over flow interrupt enable bit
	UINT32 Tmr5MatchEn:1;	// Timer5 Match interrupt enable
	UINT32 Tmr5CntDir:1;	// Timer5 direction (0: increment, 1: decrement)
	UINT32 Tmr6En:1;	// Timer6 enable bit
	UINT32 Tmr6OfEn:1;	// Timer6 over flow interrupt enable bit
	UINT32 Tmr6MatchEn:1;	// Timer6 Match interrupt enable
	UINT32 Tmr6CntDir:1;	// Timer6 direction (0: increment, 1: decrement)
	UINT32 Tmr7En:1;	// Timer7 enable bit
	UINT32 Tmr7OfEn:1;	// Timer7 over flow interrupt enable bit
	UINT32 Tmr7MatchEn:1;	// Timer7 Match interrupt enable
	UINT32 Tmr7CntDir:1;	// Timer7 direction (0: increment, 1: decrement)
} VPL_TMRC_CTRL;

typedef volatile struct {
	UINT32 TmrCount;
	UINT32 TmrAutoReload;
	UINT32 TmrMatch;
} VPL_TMRC_TmrReg;

typedef struct {
	UINT32 TmrVersion;
	UINT32 TmrCtrl;
//	VPL_TMRC_CTRL TmrCtrl;
	UINT32 TmrStat;
	VPL_TMRC_TmrReg TmrReg[8];
} VPL_TMRC;

extern volatile VPL_TMRC *tmrc;
extern INT32 TMRC_Ctrl_Config(UINT32 timer, UINT32 set_down_count);
extern INT32 TMRC_Enable(UINT32 timer);
extern INT32 TMRC_Disable(UINT32 timer);
extern INT32 TMRC_InterruptDisable(UINT32 timer);
extern INT32 TMRC_SetCounter(UINT32 timer, UINT32 value);
extern INT32 TMRC_SetAutoReload(UINT32 timer, UINT32 value);
extern UINT32 TMRC_ReadCounter(UINT32 timer);
//extern unsigned long haydn_gettimeoffset(void);
extern unsigned long VPL_TMRC_gettimeoffset(void);

//extern struct irqaction timer_irq;

#endif /* __ASM_ARCH_TIME_H__ */
