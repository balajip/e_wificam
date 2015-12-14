/*
 * Name: get_commonlog_time
 *
 * Description: Returns the current time in common log format in a static
 * char buffer.
 *
 * commonlog time is exactly 25 characters long
 * because this is only used in logging, we add " [" before and "] " after
 * making 29 characters
 * "[27/Feb/1998:20:20:04 +0000] "
 *
 * Contrast with rfc822 time:
 * "Sun, 06 Nov 1994 08:49:37 GMT"
 *
 * Altered 10 Jan 2000 by Jon Nelson ala Drew Streib for non UTC logging
 *
 */
#include "serverpush_local.h"

#ifdef _DEBUG
#include <time.h>
#include <string.h>


/* Define if struct tm has a tm_gmtoff member */
#define HAVE_TM_GMTOFF 1

#ifdef HAVE_TM_GMTOFF
#define TIMEZONE_OFFSET(foo) foo->tm_gmtoff
#else
#define TIMEZONE_OFFSET(foo) timezone
#endif

/* Don't need or want the trailing nul for these character arrays */
static const char month_tab[48] =
    "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";
static const char day_tab[28] = "Sun,Mon,Tue,Wed,Thu,Fri,Sat,";


char *get_commonlog_time(void)
{
    struct tm *t;
    char *p;
    unsigned int a;
    static char buf[30];
    int time_offset;
    time_t current_time;

    (void) time(&current_time);

    t = localtime(&current_time);
    time_offset = TIMEZONE_OFFSET(t);

    p = buf + 29;
    *p-- = '\0';
    *p-- = ' ';
    *p-- = ']';
    a = abs(time_offset / 60);
    *p-- = '0' + a % 10;
    a /= 10;
    *p-- = '0' + a % 6;
    a /= 6;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = (time_offset >= 0) ? '+' : '-';
    *p-- = ' ';

    a = t->tm_sec;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = ':';
    a = t->tm_min;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = ':';
    a = t->tm_hour;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = ':';
    a = 1900 + t->tm_year;
    while (a) {
        *p-- = '0' + a % 10;
        a /= 10;
    }
    /* p points to an unused spot */
    *p-- = '/';
    p -= 2;
    memcpy(p--, month_tab + 4 * (t->tm_mon), 3);
    *p-- = '/';
    a = t->tm_mday;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p = '[';
    return p;                   /* should be same as returning buf */

}
#else
inline char *get_commonlog_time(void)
{
    return "";
}
#endif
