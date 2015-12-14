#ifndef __BEETHOVEN_WDT_H__
#define __BEETHOVEN_WDT_H__

#define EVM_WDTC_BASE 	     0x78800000
#define EVM_WDT_VERSION      0x00
#define EVM_WDT_CTRL         0x04 //bit#2 enable-WDTC = 1 ; disable-WDTC = 0
#define EVM_WDT_STAT         0x08 //RO //bit#0 counter-match=1 ; counter-dismatch=0
#define EVM_WDT_COUNT        0x0c //RO //the value of counter
#define EVM_WDT_RELOAD_VALUE 0x10 //reload value
#define EVM_WDT_MATCH_VALUE  0x14 //match value
#define EVM_WDT_RELOAD_CTRL  0x18 //write reload-passwd to reload counter
#define WDT_RELOAD_PASSWD    0x28791166

#endif
