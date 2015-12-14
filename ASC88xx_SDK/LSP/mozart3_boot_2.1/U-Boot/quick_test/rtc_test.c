#include <common.h>
#include <command.h>
#include <config.h>
#include <rtc.h>
#include <i2c.h>
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;
extern const char *weekdays[];

#define RELOC(a)    ((typeof(a))((unsigned long)(a) + gd->reloc_off))

int rtc_test_func(void)
{
	int ret, i;
	struct rtc_time tm_bak, tm;

	printf( "rtc_test_func!!!\n" ) ;

    //We have to set the SCL/SDA pin numbers at first.
	SOFT_I2C_SCL_PIN = (1 << MOZART_RTC_SCL);
	SOFT_I2C_SDA_PIN = (1 << MOZART_RTC_SDA);
    i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	printf("rtc reset...\n");
	rtc_reset();

	printf("back up current setting...\n");
	ret = rtc_get(&tm_bak);	
 	if (ret != 0) {
		puts("## Get date failled\n");
		return -1;
	}
	printf ("Date: %4d-%02d-%02d (%sday)    Time: %2d:%02d:%02d\n",
		tm_bak.tm_year, tm_bak.tm_mon, tm_bak.tm_mday,
		(tm_bak.tm_wday<0 || tm_bak.tm_wday>6) ?
			"unknown " : RELOC(weekdays[tm_bak.tm_wday]),
		tm_bak.tm_hour, tm_bak.tm_min, tm_bak.tm_sec);

	printf("start rtc testing...\n");
	/* happy new year */
	tm.tm_year = 2009;
	tm.tm_mon = 12;
	tm.tm_wday = 4;
	tm.tm_mday = 31;
	tm.tm_hour = 23;
	tm.tm_min = 59;
	tm.tm_sec = 58;
	rtc_set(&tm);
	printf ("return to:%4d-%02d-%02d %2d:%02d:%02d UTC\n",
		tm.tm_year, tm.tm_mon, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("wait 3 secs...\n");
	for (i = 0; i < 3000; ++i) {
		udelay(1000);
	}

	memset(&tm, 0, sizeof(tm));
	ret = rtc_get(&tm);	
 	if (ret != 0) {
		puts("## Get date failled\n");
		goto fail_exit;
	}
	
	if ((tm.tm_year!=2010) || (tm.tm_mon!=1) || (tm.tm_mday!=1) ||
		(tm.tm_hour!=0) || (tm.tm_hour!=0) || (tm.tm_min!=0))
		goto fail_exit;

	printf ("date:	%4d-%02d-%02d %2d:%02d:%02d UTC\n",
		tm.tm_year, tm.tm_mon, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);

	printf("save back current time...\n");
	rtc_set(&tm_bak);
	memset(&tm, 0, sizeof(tm));
	ret = rtc_get(&tm);	
 	if (ret != 0) {
		puts("## Get date failled\n");
		goto fail_exit;
	}

	printf ("Date: %4d-%02d-%02d (%sday)    Time: %2d:%02d:%02d\n",
		tm_bak.tm_year, tm_bak.tm_mon, tm_bak.tm_mday,
		(tm_bak.tm_wday<0 || tm_bak.tm_wday>6) ?
			"unknown " : RELOC(weekdays[tm_bak.tm_wday]),
		tm_bak.tm_hour, tm_bak.tm_min, tm_bak.tm_sec);


	return 0;

fail_exit:
	rtc_set(&tm_bak);
	return -1;
}



