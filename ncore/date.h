#ifndef __DATE_H
#define __DATE_H

#include "config.h"

#if CONFIG_MODULE_RTC || CONFIG_MODULE_DS3231

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};
    
void _gregorian_day(struct rtc_time * tm);
uint32_t _mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);

#endif /* CONFIG_MODULE_RTC || CONFIG_MODULE_DS3231 */

#endif /* __DATE_H */


