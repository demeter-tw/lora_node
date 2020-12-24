#include "bsp_sht3x.h"
#include "bsp_i2c.h"
#include "errno.h"
#include <stdio.h>

#if CONFIG_MODULE_I2C && CONFIG_DEMETER_SENSOR_AIR_TH

static int i2c_failed_read_count = 0;

static float _sht3x_calc_temperature(uint16_t value)
{
        return 175.0f * (float) value / 65535.0f - 45.0f;
}

static float _sht3x_calc_humidity(uint16_t value)
{
        return 100.0f * (float) value / 65535.0f;
}

void _sht3x_soft_reset(void)
{
    sensor_i2c_byte_write_16bit(SHT3X_I2C_ADDR, CMD_SOFT_RESET, 0, 0);
}

void sht3x_read_serial_number(void)
{
        uint8_t data[4];
        uint32_t serial_no;

        sensor_i2c_buffer_read_16bit(SHT3X_I2C_ADDR, CMD_READ_SERIAL_NO, 4, data);

        serial_no = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        printf("Serial no = %x\r\n", serial_no);
}

void sht3x_th_single_shot_read(float *temp, float *humidity)
{
        uint8_t data[SHT3X_DATA_PAIR_LENGHT];

        sensor_i2c_buffer_read_16bit(SHT3X_I2C_ADDR, CMD_MEAS_CLOCKSTR_L,
                SHT3X_DATA_PAIR_LENGHT, data);

        *humidity = _sht3x_calc_humidity((data[3] << 8) | data[4]);
        *temp = _sht3x_calc_temperature((data[0] << 8) | data[1]);

        /* printf("tmep = %.2f, humidity = %.2f\r\n", temp, humidity); */
}

void sht3x_start_periodic_meas(void)
{
        sensor_i2c_byte_write_16bit(SHT3X_I2C_ADDR, CMD_MEAS_PERI_1_H, 0, 0);
}

int sht3x_th_periodic_read(float *temp, float *humidity)
{
        uint8_t data[SHT3X_DATA_PAIR_LENGHT];
        int rv;

        rv = sensor_i2c_buffer_read_16bit(SHT3X_I2C_ADDR, CMD_FETCH_DATA,
                SHT3X_DATA_PAIR_LENGHT, data);
        if (rv == NCORE_ERR_OK) {
                *humidity = _sht3x_calc_humidity((data[3] << 8) | data[4]);
                *temp = _sht3x_calc_temperature((data[0] << 8) | data[1]);
                i2c_failed_read_count = 0;
        } else {
                if (++i2c_failed_read_count > 5) {
                        printf("\r\n[I2C][INFO] I2C failed counter MAX, reboot system\r\n");
                        NVIC_SystemReset();
                }
        }

        return rv;
}

void sht3x_status_read(uint16_t *val)
{
        uint8_t data[3];

        sensor_i2c_buffer_read_16bit(SHT3X_I2C_ADDR, CMD_READ_STATUS, 3, data);
        *val = (data[0] << 8) | data[1];
}

void sht3x_clear_status(void)
{
        sensor_i2c_byte_write_16bit(SHT3X_I2C_ADDR, CMD_CLEAR_STATUS, 0, 0);
}

void sht3x_heater_switch(uint8_t on)
{
        if (on) {
                sensor_i2c_byte_write_16bit(SHT3X_I2C_ADDR, CMD_HEATER_ENABLE, 0, 0);
        } else {
                sensor_i2c_byte_write_16bit(SHT3X_I2C_ADDR, CMD_HEATER_DISABLE, 0, 0);
        }
}
#endif /* CONFIG_MODULE_I2C && CONFIG_DEMETER_SENSOR_AIR_TH */

