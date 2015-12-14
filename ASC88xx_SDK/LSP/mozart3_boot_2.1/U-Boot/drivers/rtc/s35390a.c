/*
  * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <rtc.h>
#include <i2c.h>

#if defined(CONFIG_CMD_DATE)

/* #define DEBUG_S35390A */

#ifdef DEBUG_S35390A
DECLARE_GLOBAL_DATA_PTR;
#endif

#ifdef DEBUG_S35390A
#define PRINTD(fmt,args...) do {    \
    if (gd->have_console)       \
        printf (fmt ,##args);   \
    } while (0)
#else
#define PRINTD(fmt,args...)
#endif

#define S35390A_CMD_STATUS1 0
#define S35390A_CMD_STATUS2 1
#define S35390A_CMD_TIME1   2

#define S35390A_BYTE_YEAR   0
#define S35390A_BYTE_MONTH  1
#define S35390A_BYTE_DAY    2
#define S35390A_BYTE_WDAY   3
#define S35390A_BYTE_HOURS  4
#define S35390A_BYTE_MINS   5
#define S35390A_BYTE_SECS   6

#define S35390A_FLAG_POC    0x01
#define S35390A_FLAG_BLD    0x02
#define S35390A_FLAG_24H    0x40
#define S35390A_FLAG_RESET  0x80
#define S35390A_FLAG_TEST   0x01

static const u8 byte_rev_table[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

static int twentyfourhour;

static unsigned int bcd2bin (unsigned char n)
{
	return ((((n >> 4) & 0x0F) * 10) + (n & 0x0F));
}

static unsigned char bin2bcd (unsigned int n)
{
	return (((n / 10) << 4) | (n % 10));
}

static char s35390a_hr2reg(int hour)
{
    if (twentyfourhour)
        return bin2bcd(hour);
    
    if (hour < 12)
        return bin2bcd(hour);

    return 0x40 | bin2bcd(hour - 12);
}

static int s35390a_reg2hr(char reg)
{
    unsigned hour;

    if (twentyfourhour)
        return bcd2bin(reg & 0x3f);

    hour = bcd2bin(reg & 0x3f);
    if (reg & 0x40)
        hour += 12;

    return hour;
}

static int s35390a_set_reg(int reg, char *buf, int len)
{
	int ret;

	ret = i2c_write(CONFIG_SYS_I2C_RTC_ADDR|reg, 0, 0, (unsigned char *)buf, len);
	if (ret != 0) {
		printf("%s: failed to write\n", __FUNCTION__);
		return ret;
	}

	return ret;
}

static int s35390a_get_reg(int reg, uchar *buf, int len)
{
	int ret;

	ret = i2c_read(CONFIG_SYS_I2C_RTC_ADDR, reg, 0, buf, len);
	if (ret != 0) {
		printf("%s: failed to read\n", __FUNCTION__);
		return ret;
	}

	return ret;
}


/*
 * Get the current time from the RTC
 */
int rtc_get (struct rtc_time *tm)
{
	char buf[7];
	int i, ret;

	memset(buf, 0, sizeof(buf));

    if (s35390a_get_reg(S35390A_CMD_STATUS1, (unsigned char *)buf, sizeof(buf)) < 0) {
		printf("%s: fail to read reg %d\n", __FUNCTION__, S35390A_CMD_STATUS1);
        return 0;
	}
	if (buf[0] & S35390A_FLAG_24H)
		twentyfourhour = 1;
	else
		twentyfourhour = 0;
    
	ret = s35390a_get_reg(S35390A_CMD_TIME1, (unsigned char *)buf, sizeof(buf));
	if (ret != 0)
		return ret;

    for (i = 0; i < 7; ++i) 
        buf[i] = byte_rev_table[(int)buf[i]];

	tm->tm_sec = bcd2bin(buf[S35390A_BYTE_SECS]);
	tm->tm_min = bcd2bin(buf[S35390A_BYTE_MINS]);
	tm->tm_hour = s35390a_reg2hr(buf[S35390A_BYTE_HOURS]);
	tm->tm_wday = bcd2bin(buf[S35390A_BYTE_WDAY]);
	tm->tm_mday = bcd2bin(buf[S35390A_BYTE_DAY]);
	tm->tm_mon = bcd2bin(buf[S35390A_BYTE_MONTH]);
	tm->tm_year = bcd2bin(buf[S35390A_BYTE_YEAR])+2000;

	PRINTD("%s:  tmp is secs=%d, mins=%d, hours=%d, mday=%d, "
		"mon=%x, year=%d, wday=%d\n", __func__, tm->tm_sec,
		tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_mon, tm->tm_year,
		tm->tm_wday);

	return 0;
}

/*
 * Set the RTC
 */
int rtc_set (struct rtc_time *tm)
{
	char buf[7];
	int i, err;

    PRINTD("%s: tm is secs=%d, mins=%d, hours=%d mday=%d, "
        "mon=%d, year=%d, wday=%d\n", __func__, tm->tm_sec,
        tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_mon, tm->tm_year,
        tm->tm_wday);

    buf[S35390A_BYTE_YEAR] = bin2bcd(tm->tm_year-2000);
    buf[S35390A_BYTE_MONTH] = bin2bcd(tm->tm_mon);
    buf[S35390A_BYTE_DAY] = bin2bcd(tm->tm_mday);
    buf[S35390A_BYTE_WDAY] = bin2bcd(tm->tm_wday);
    buf[S35390A_BYTE_HOURS] = s35390a_hr2reg(tm->tm_hour);
    buf[S35390A_BYTE_MINS] = bin2bcd(tm->tm_min);
    buf[S35390A_BYTE_SECS] = bin2bcd(tm->tm_sec);

    /* This chip expects the bits of each byte to be in reverse order */
    for (i = 0; i < 7; ++i) 
        buf[i] = byte_rev_table[(int)buf[i]];

    err = s35390a_set_reg(S35390A_CMD_TIME1, buf, sizeof(buf));

    return err;
}

/*
 * Reset the RTC. We set the date back to 1970-01-01.
 */
void rtc_reset (void)
{
	uchar buf[1];


	if (s35390a_get_reg(S35390A_CMD_STATUS1, buf, sizeof(buf)) < 0) {
		printf("%s: fail to read reg %d\n", __FUNCTION__, S35390A_CMD_STATUS1);
        return;
	}

	if (!(buf[0] & (S35390A_FLAG_POC | S35390A_FLAG_BLD))) {
		printf("RTC is already initialized.\n");
		return;	
	}

	printf("Initialize RTC\n");
	buf[0] |= S35390A_FLAG_RESET;
	buf[0] &= 0xf0;
	s35390a_set_reg(S35390A_CMD_STATUS1, (unsigned char *)buf, sizeof(buf));

#if 0
	/* reset system time */
	/* Jan. 1, 2000 was a Saturday */
	tmp.tm_year = 0;
	tmp.tm_mon = 1;
	tmp.tm_wday= 6;
	tmp.tm_mday= 1;
	tmp.tm_hour = 0;
	tmp.tm_min = 0;
	tmp.tm_sec = 0;

	rtc_set(&tmp);

	printf ("RTC:	%4d-%02d-%02d %2d:%02d:%02d UTC\n",
		tmp.tm_year, tmp.tm_mon, tmp.tm_mday,
		tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
#endif

	return;
}

#endif
