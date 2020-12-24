#ifndef __BSP_RTC_H
#define __BSP_RTC_H

#include "config.h"
#include "date.h"

#if CONFIG_MODULE_RTC

#define RTC_CLOCK_SOURCE_LSE

#define RTC_BKP_DRX	BKP_DR1
#define RTC_BKP_DATA	0xA5A5
#define TIMEZONE		28800

void bsp_rtc_init(void);
void bsp_rtc_set_time(struct rtc_time *tm);
void bsp_rtc_set_alarm(struct rtc_time *tm);

#endif /* CONFIG_MODULE_RTC */

#endif /* __BSP_RTC_H */

