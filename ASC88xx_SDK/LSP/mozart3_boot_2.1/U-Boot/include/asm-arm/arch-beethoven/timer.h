#ifndef __BEETHOVEN_TIMER_H__
#define __BEETHOVEN_TIMER_H__

typedef struct
{       
    unsigned long TM0_EN:1;
    unsigned long TM0_OF_EN:1;
    unsigned long TM0_MATCH_EN:1;
    unsigned long TM0_CNT_DIR:1;
    unsigned long TM1_EN:1;
    unsigned long TM1_OF_EN:1;
    unsigned long TM1_MATCH_EN:1;
    unsigned long TM1_CNT_DIR:1;
    unsigned long TM2_EN:1;
    unsigned long TM2_OF_EN:1;
    unsigned long TM2_MATCH_EN:1;
    unsigned long TM2_CNT_DIR:1;
    unsigned long Reserved;
}VPL_TimerControl;

typedef struct 
{
	unsigned long TimerValue;
    unsigned long TimerLoad;
    unsigned long TimerMatch;
}VPL_TimerReg;

#define EVM_TIMER_BASE 0x78000000

#define EVM_TIMER_CTRL 0x04
#define EVM_TIMER_STAT 0x08
#define EVM_TIMER_COUNTER_BASE 0x0c
#define EVM_TIMER_AUTO_RELOAD_VALUE_BASE 0x10
#define EVM_TIMER_MATCH_VALUE_BASE 0x14
#define EVM_TIMER_CTRL_BASE (EVM_TIMER_BASE + 0xc)

#define TIMER_LOAD_VAL 0x80000000

#endif
