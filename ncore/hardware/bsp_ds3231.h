#ifndef __BSP_DS3231_H
#define __BSP_DS3231_H

#include "config.h"
#include "date.h"

#if CONFIG_MODULE_DS3231

#define DS3231_SLAVE_ADDR               0x68

#define DS3231_DATE_REG         0x00
#define DS3231_ALARM1_REG       0x07
#define DS3231_ALARM2_REG       0x0B
#define DS3231_CONTROL_REG              0x0E
#define DS3231_STATUS_REG               0x0F

typedef struct {
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        uint8_t day_of_week; // 1 ~ 7
        uint8_t date;   // 1 ~ 31
        uint8_t month;
        uint8_t year;
} ds3231_date_t;

typedef union {
        uint8_t value;
        struct {
                uint8_t time: 7;
                uint8_t mask: 1;
        } bits;
} ds3231_alarm_reg_t;

typedef struct {
        ds3231_alarm_reg_t seconds;
        ds3231_alarm_reg_t minutes;
        ds3231_alarm_reg_t hours;
        ds3231_alarm_reg_t day_date;
} ds3231_alarm1_t;

typedef struct {
        ds3231_alarm_reg_t minutes;
        ds3231_alarm_reg_t hours;
        ds3231_alarm_reg_t day_date;
} ds3231_alarm2_t;

typedef union {
        uint8_t value;
        struct {
                uint8_t a1ie: 1;
                uint8_t a2ie: 1;
                uint8_t intcn: 1;
                uint8_t rs1: 1;
                uint8_t rs2: 1;
                uint8_t conv: 1;
                uint8_t bbsqw: 1;
                uint8_t eosc: 1;
        } bits;
} ds3231_control_reg_t;

typedef union {
        uint8_t value;
        struct {
                uint8_t a1f: 1;
                uint8_t a2f: 1;
                uint8_t bsy: 1;
                uint8_t en32khz: 1;
                uint8_t reserved: 3;
                uint8_t osf: 1;
        } bits;
} ds3231_status_reg_t;

typedef struct {
        ds3231_date_t date;
        ds3231_alarm1_t alarm1;
        ds3231_alarm2_t alarm2;
        ds3231_control_reg_t control;
        ds3231_status_reg_t status;
        uint8_t aging_offset;
        uint8_t msb_temp;
        uint8_t lsb_temp;
} ds3231_registers_t;

void ds3231_init(void);
int ds3231_date_set(struct rtc_time *tm);
int ds3231_date_get(struct rtc_time *tm);
int ds3231_load_registers(void);
int ds3231_alarm2_flag_clear(void);
int ds3231_alarm2_set(int hour, int minute);

#endif /* CONFIG_MODULE_DS3231 */

#endif /* __BSP_DS3231_H */

