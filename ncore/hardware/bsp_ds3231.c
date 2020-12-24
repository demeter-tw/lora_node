#include "bsp_ds3231.h"
#include "sal_i2c.h"
#include "errno.h"
#include "bsp_rcc.h"
#include <stdio.h>
#include <stdlib.h>

#if CONFIG_MODULE_DS3231

#define DS3231_CHK_RESULT(rv)   do { \
                if (rv != NCORE_ERR_OK) { \
                        printf("[DS3231] Error: I2C result = %d\r\n", rv);     \
                        return rv;      \
                }       \
        } while (0)

#define DS3231_MDELAY(rv)       do {    \
                if (rv != NCORE_ERR_OK) {       \
                        mdelay(100);    \
                } \
        } while (0)

ds3231_registers_t ds3231_regs;

int ds3231_load_registers(void)
{
        uint8_t *reg;
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Load DS3231 register...\r\n");

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, 0x00, sizeof(ds3231_registers_t), (unsigned char *) &ds3231_regs);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        reg = (uint8_t *) &ds3231_regs;
        for (int cnt = 0; cnt < sizeof(ds3231_registers_t); cnt++) {
                printf("reg[%02x] = 0x%02x\r\n", cnt, reg[cnt]);
        }

        return rv;
}

int ds3231_32khz_disable(void)
{
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Disable EN32KHz...");

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, DS3231_STATUS_REG, 1, &ds3231_regs.status.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        ds3231_regs.status.bits.en32khz = 0;

        retry = 0;
        do {
                rv = sensor_i2c_byte_write(DS3231_SLAVE_ADDR, DS3231_STATUS_REG, 1, &ds3231_regs.status.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        printf("DONE\r\n");

        return NCORE_ERR_OK;
}


int ds3231_date_get(struct rtc_time *tm)
{
        ds3231_date_t *date = (ds3231_date_t *) calloc (1, sizeof(ds3231_date_t));
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Get DS3231 Date...");

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, DS3231_DATE_REG, sizeof(ds3231_date_t), (unsigned char *) date);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        tm->tm_sec = (date->seconds >> 4) * 10  + (date->seconds & 0x0F);
        tm->tm_min = (date->minutes >> 4) * 10  + (date->minutes & 0x0F);
        tm->tm_hour = (date->hours >> 4) * 10 + (date->hours & 0x0F);
        tm->tm_mday = (date->date >> 4 ) * 10 + (date->date & 0x0F);
        tm->tm_mon = (date->month >> 4) * 10 + (date->month & 0x0F);
        tm->tm_year = (date->year >> 4) * 10 + (date->year & 0x0F) + 2000;
        tm->tm_wday = date->day_of_week;
        printf("Done\r\n");

        free(date);

        return NCORE_ERR_OK;
}

int ds3231_date_set(struct rtc_time *tm)
{
        ds3231_date_t *date = (ds3231_date_t *) calloc (1, sizeof(ds3231_date_t));
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Set DS3231 Date...");

        _gregorian_day(tm);
        tm->tm_year -= 2000;

        date->seconds = (tm->tm_sec / 10) * 16  + (tm->tm_sec % 10);
        date->minutes = (tm->tm_min / 10) * 16  + (tm->tm_min % 10);
        date->hours = (tm->tm_hour / 10) * 16  + (tm->tm_hour % 10);
        date->date = (tm->tm_mday / 10) * 16  + (tm->tm_mday % 10);
        date->month = (tm->tm_mon / 10) * 16  + (tm->tm_mon % 10);
        date->year = (tm->tm_year / 10) * 16  + (tm->tm_year % 10);
        date->day_of_week = tm->tm_wday;

        retry = 0;
        do {
                rv = sensor_i2c_byte_write(DS3231_SLAVE_ADDR, DS3231_DATE_REG, sizeof(ds3231_date_t), (unsigned char *) date);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        printf("Done\r\n");

        free(date);

        return NCORE_ERR_OK;
}

int ds3231_alarm1_enable(int enable)
{
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Enable alarm1...");

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, DS3231_CONTROL_REG, 1, &ds3231_regs.control.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        ds3231_regs.control.bits.a1ie = enable;

        retry = 0;
        do {
                rv = sensor_i2c_byte_write(DS3231_SLAVE_ADDR, DS3231_CONTROL_REG, 1, &ds3231_regs.control.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        printf("Done\r\n");

        return NCORE_ERR_OK;
}

int ds3231_alarm2_enable(int enable)
{
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Enable alarm2...");

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, DS3231_CONTROL_REG, 1, &ds3231_regs.control.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        ds3231_regs.control.bits.a2ie = enable;

        retry = 0;
        do {
                rv = sensor_i2c_byte_write(DS3231_SLAVE_ADDR, DS3231_CONTROL_REG, 1, &ds3231_regs.control.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        printf("Done\r\n");

        return NCORE_ERR_OK;
}

int ds3231_alarm2_set(int hour, int minute)
{
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Set alarm2 (%d:%d)...", hour, minute);

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, DS3231_ALARM2_REG, sizeof(ds3231_alarm2_t), (unsigned char *) &ds3231_regs.alarm2);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        // minutes
        ds3231_regs.alarm2.minutes.bits.mask = 0;
        ds3231_regs.alarm2.minutes.bits.time = (minute / 10) * 16  + (minute % 10);
        // hour
        ds3231_regs.alarm2.hours.bits.mask = 0;
        ds3231_regs.alarm2.hours.bits.time = (hour / 10) * 16  + (hour % 10);
        // day and date (Don't care.)
        ds3231_regs.alarm2.day_date.bits.mask = 1;

        retry = 0;
        do {
                rv = sensor_i2c_byte_write(DS3231_SLAVE_ADDR, DS3231_ALARM2_REG, sizeof(ds3231_alarm2_t), (unsigned char *) &ds3231_regs.alarm2);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        printf("Done\r\n");

        return NCORE_ERR_OK;
}

int ds3231_alarm2_flag_clear(void)
{
        int rv = NCORE_ERR_FAIL;
        int retry;

        printf("Clear alarm2 flag...");

        retry = 0;
        do {
                rv = sensor_i2c_buffer_read(DS3231_SLAVE_ADDR, DS3231_STATUS_REG, 1, &ds3231_regs.status.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        ds3231_regs.status.bits.a2f = 0;

        retry = 0;
        do {
                rv = sensor_i2c_byte_write(DS3231_SLAVE_ADDR, DS3231_STATUS_REG, 1, &ds3231_regs.status.value);
                retry++;
                DS3231_MDELAY(rv);
        } while (rv != NCORE_ERR_OK && retry < 5);
        DS3231_CHK_RESULT(rv);

        printf("Done\r\n");

        return NCORE_ERR_OK;
}

void ds3231_init(void)
{
        ds3231_alarm2_flag_clear();
        ds3231_alarm2_enable(1);

        // Disable EN32KHz
        // ds3231_32khz_disable();
}

#endif /* CONFIG_MODULE_DS3231 */

