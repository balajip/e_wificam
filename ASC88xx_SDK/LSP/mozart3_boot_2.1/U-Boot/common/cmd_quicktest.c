/*
 * Copyright (C) 2012  VN Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

typedef struct {
    char *name ;
    int (*func)(void) ; /* return: negative if error, otherwise if successful */
} quick_test_item ;

extern int dram_test_func(void) ;
extern int timer_test_func(void) ;
extern int wdt_test_func(void)   ;
extern int gmac_test_func(void)  ;
extern int dido_test_func(void)  ;
extern int pcie_test_func(void)  ;
extern int rtc_test_func(void)   ;
extern int usbc_test_func(void)  ;
extern int mshc_test_func(void)  ;
extern int audio_test_func(void) ;
extern int video_test_func(void) ;
extern int RS485_test_func(void) ;
void autotest_main_func(void);


quick_test_item quick_test_list[] = {    
#ifdef CONFIG_QUICK_TEST_WDT          
        {"Wdt", &wdt_test_func},
#endif        
#ifdef CONFIG_QUICK_TEST_GMAC            
        {"Gmac", &gmac_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_SPI    
        {"SPI Flash", &spi_flash_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_PCIE
        {"PCIE", &pcie_test_func},
#endif        
#ifdef CONFIG_QUICK_TEST_RTC
        {"RTC", &rtc_test_func},
#endif       
#ifdef CONFIG_QUICK_TEST_UART   
        {"Uart", &uart_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_DRAM   
        {"Dram", &dram_test_func},
#endif        
#ifdef CONFIG_QUICK_TEST_TIMER           
        {"Timer & Intc", &timer_test_func},
#endif        

#ifdef CONFIG_QUICK_TEST_DIDO   
        {"DIDO", &dido_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_NAND           
        {"NAND", &nand_test_func},        
#endif
#ifdef CONFIG_QUICK_TEST_USBC
        {"Usbc", &usbc_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_MSHC
        {"Mshc", &mshc_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_AUDIO
        {"Audio", &audio_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_VIDEO
        {"Video", &video_test_func},
#endif
#ifdef CONFIG_QUICK_TEST_RS485
        {"RS485", &RS485_test_func},
#endif	
        {NULL, NULL}//dummy final item
} ;

#define SPACE_BETWEEN_ITEMS 20
void UI_print_main_menu(void)
{
   int i, j;
   int range = sizeof(quick_test_list) / sizeof(quick_test_item) ;

   i = 0 ;
   while( quick_test_list[i].func != NULL ) {
        if ((i % 3) == 0)
            printf( "\n" ) ;
        
        printf( "%2d : %s", i + 1, quick_test_list[i].name ) ;

        j = SPACE_BETWEEN_ITEMS - strlen(quick_test_list[i].name) ;
        while( j >= 0 ) {
            printf(" ") ;
            j-- ;
        }

        i++ ;
    }
    printf("\n\nSelect #1 ~ #%d functions, separated by space, to test; "
	       "or enter 'a' to test all functions.\n"
	       "Just enter to exit the test program.."
		, range-1 ) ;
}

int UI_get_the_choice(void)
{
    char str[CONFIG_SYS_CBSIZE] ;
    int choice ;
    int range ;
    char *endp ;

    readline_into_buffer("=> ", str) ;
    choice = simple_strtoul(str, &endp, 10);

    range = sizeof(quick_test_list) / sizeof(quick_test_item) ;
    range-- ;//discount the final dummy item
    if ((choice > range) || (choice <= 0)) {
        choice = -1 ;
    }

    return choice - 1 ;
}

void UI_main_func(void)
{
    char str[CONFIG_SYS_CBSIZE], *p;
    int choice, test_result ;
    int range = sizeof(quick_test_list) / sizeof(quick_test_item);
    
	do
	{
        printf( "\n\n\n=======================================================================================\n" ) ;
        printf( "=====================================  Quick Test =====================================\n" ) ;
        printf( "=======================================================================================\n" ) ;  

        UI_print_main_menu();
        readline_into_buffer("=> ", str);
		if (str[0] == 'a')
			autotest_main_func();
		else
		{
			p = str;
			while (*p)
			{
				choice = simple_strtoul(p, &p, 10);
				//printf("choice: %d\n", choice);
				if (choice > 0 && choice < range)
				{
					printf("----------------------\n");
					printf("\n[[Test %s]]\n", quick_test_list[choice-1].name);
					test_result = quick_test_list[choice-1].func();

					if (test_result < 0)
						printf( "[[%s Test]] -- Failed!!\n", quick_test_list[choice-1].name);
					else
						printf( "[[%s Test]] -- Passed!!\n", quick_test_list[choice-1].name);

					printf("\n----------------------\n");
		        }
				else if (*p)
					p++;
			}
		}
	} while (*str);
}

void autotest_main_func(void)
{
    int test_result ;
    int i ;
    int range = sizeof(quick_test_list) / sizeof(quick_test_item) ;
    range-- ;//remember, the final is {NULL, NULL}
    int totalresult[100] ;

    printf( "\n\n\n=======================================================================================\n" ) ;
    printf( "=====================================  Quick Test =====================================\n" ) ;
    printf( "=======================================================================================\n" ) ;  
    for(i = 0 ; i < range ; i++) {        
        printf("----------------------\n");
        printf("\n[[Test %s]]\n", quick_test_list[i].name) ;
        test_result = quick_test_list[i].func() ;
            
        if (test_result < 0) {
            printf( "[[%s Test]] -- Failed!!\n", quick_test_list[i].name) ;
            totalresult[i] = -1 ;
        }
        else {
            printf( "[[%s Test]] -- Passed!!\n", quick_test_list[i].name) ;        
            totalresult[i] = 1 ;
        }

        printf("\n----------------------\n");
    }

    printf("\n##### Total Test Result #####\n") ;
    for(i = 0 ; i < 100 ; i++) {
        if( i >= range )
            break ;

        printf( "[[%s Test]]", quick_test_list[i].name) ;
        int len = strlen(quick_test_list[i].name) ;
        len = 15 - len ;
        while(len > 0) {
            len-- ;
            printf(" ") ;
        }        
        if(totalresult[i] == -1) {
            printf(" -- Failed!!!\n") ;
        }
        else {
            printf(" -- Passed.\n") ;
        }       
    }
    printf("\nEnd the auto test.\n") ;
}


int do_test (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    UI_main_func() ;     

    return 0;
}

U_BOOT_CMD(
	test, 2,	0,	do_test,
	"Show Quick Test Menu",
	NULL
);


