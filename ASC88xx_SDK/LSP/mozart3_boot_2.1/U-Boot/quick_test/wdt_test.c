#include <common.h>
#include <command.h>
#include <config.h>
#include <spi.h>
#include <spi_flash.h>
#include <asm/arch/platform.h>

#define WDT_REBOOT_MARK1           0x96A0BCF2
#define WDT_REBOOT_MARK2           0x28791166
#define WDT_ORIGINAL_RELOAD_VALUE  0x5AB9
#define WDT_RELOAD_VALUE           0x300

int WDTC_quick_test(void) 
{
	unsigned long *reg = (void *)VPL_USBC_RAM_BASE + 1000;
	volatile int timeout = 100000 ;


	if (reg[0] == WDT_REBOOT_MARK1 && reg[1] == WDT_REBOOT_MARK2) { // this condition should be true after rebooting
		v_outl(EVM_WDTC_BASE + EVM_WDT_CTRL, 0x0 ) ; //disable wdt
		v_outl(EVM_WDTC_BASE + EVM_WDT_RELOAD_VALUE, WDT_ORIGINAL_RELOAD_VALUE); 
		v_outl(EVM_WDTC_BASE + EVM_WDT_RELOAD_CTRL, WDT_RELOAD_PASSWD );   
		printf( "[ WDT ] ......................................... Pass\n" ) ;
		return 0 ;
	}
	else { // this condition should be true when testing wdt first time
		//unsigned long mark = WDT_REBOOT_MARK;

		reg[0] = WDT_REBOOT_MARK1;
		reg[1] = WDT_REBOOT_MARK2;

		//configure the WDT and restart
		v_outl(EVM_WDTC_BASE + EVM_WDT_CTRL, 0x0 ) ;                   
		v_outl(EVM_WDTC_BASE + EVM_WDT_RELOAD_VALUE, WDT_RELOAD_VALUE ); 
		v_outl(EVM_WDTC_BASE + EVM_WDT_MATCH_VALUE, 0x0 ) ;             
		v_outl(EVM_WDTC_BASE + EVM_WDT_RELOAD_CTRL, WDT_RELOAD_PASSWD ); 
		v_outl(EVM_WDTC_BASE + EVM_WDT_CTRL, 0x7 );//bit#2 : enable/disable WDTC ; bit#1 : enable/disable match ack(interrupt)

                // waiting wdt to reboot the system.  
		while (timeout >= 0) {
			timeout-- ;
		}

		if( timeout < 0 ) {
			v_outl(EVM_WDTC_BASE + EVM_WDT_CTRL, 0x0 ) ; //disable wdt
			printf("[ WDT ] ......................................... Fail\n" ) ;
			printf("        -- WDT can not reboot the system\n" ) ;
			printf("        -- in the limited time.\n" ) ;
			return -1 ;
		}
	}

	return 0 ;
}

int wdt_test_func(void)
{
	int result ;
	
	printf( "wdt_test_func!!!\n" ) ;

	result = WDTC_quick_test() ;

	return  result;
}
