#ifndef __BSP_SHT3X_H
#define __BSP_SHT3X_H

#include "stm32f10x.h"

#define SHT3X_I2C_ADDR                 0x44

#define SHT3X_DATA_PAIR_LENGHT          6

typedef union {
        uint16_t val;
        struct {
                uint16_t crc_status : 1;
                uint16_t cmd_status : 1;
                uint16_t reserv0 : 2;
                uint16_t reset_detected : 1;
                uint16_t reserv1 : 5;
                uint16_t t_alert : 1;
                uint16_t rh_alert : 1;
                uint16_t reserv2 : 1;
                uint16_t heater : 1;
                uint16_t reserv3 : 1;
                uint16_t alert_pending : 1;
        } reg;
} SHT3X_STATUS;

enum {
        CMD_READ_SERIAL_NO              = 0x3780, // read serial number
        CMD_SOFT_RESET                  = 0x30A2, // soft reset
        CMD_BREAK                       = 0x3093,
        CMD_MEAS_CLOCKSTR_H             = 0x2C06, // measurement: clock stretching, high repeatability
        CMD_MEAS_CLOCKSTR_M             = 0x2C0D, // measurement: clock stretching, medium repeatability
        CMD_MEAS_CLOCKSTR_L             = 0x2C10, // measurement: clock stretching, low repeatability
        CMD_MEAS_PERI_05_H              = 0x2032, // measurement: periodic 0.5 mps, high repeatability
        CMD_MEAS_PERI_05_M              = 0x2024, // measurement: periodic 0.5 mps, medium repeatability
        CMD_MEAS_PERI_05_L              = 0x202F, // measurement: periodic 0.5 mps, low repeatability
        CMD_MEAS_PERI_1_H               = 0x2130, // measurement: periodic 1 mps, high repeatability
        CMD_MEAS_PERI_1_M               = 0x2126, // measurement: periodic 1 mps, medium repeatability
        CMD_MEAS_PERI_1_L               = 0x212D, // measurement: periodic 1 mps, low repeatability
        CMD_MEAS_PERI_2_H               = 0x2236, // measurement: periodic 2 mps, high repeatability
        CMD_MEAS_PERI_2_M               = 0x2220, // measurement: periodic 2 mps, medium repeatability
        CMD_MEAS_PERI_2_L               = 0x222B, // measurement: periodic 2 mps, low repeatability
        CMD_MEAS_PERI_4_H               = 0x2334, // measurement: periodic 4 mps, high repeatability
        CMD_MEAS_PERI_4_M               = 0x2322, // measurement: periodic 4 mps, medium repeatability
        CMD_MEAS_PERI_4_L               = 0x2329, // measurement: periodic 4 mps, low repeatability
        CMD_MEAS_PERI_10_H              = 0x2737, // measurement: periodic 10 mps, high repeatability
        CMD_MEAS_PERI_10_M              = 0x2721, // measurement: periodic 10 mps, medium repeatability
        CMD_MEAS_PERI_10_L              = 0x272A, // measurement: periodic 10 mps, low repeatability
        CMD_FETCH_DATA                  = 0xE000, // readout measurements for periodic mode
        CMD_READ_STATUS                 = 0xF32D, // read status reg
        CMD_CLEAR_STATUS                = 0x3041, // clear status reg
        CMD_HEATER_ENABLE               = 0x306D, // enable heater
        CMD_HEATER_DISABLE              = 0x3066, // disable heater
};


void _sht3x_soft_reset(void);
void sht3x_read_serial_number(void);
void sht3x_th_single_shot_read(float *temp, float *humidity);
void sht3x_start_periodic_meas(void);
int sht3x_th_periodic_read(float *temp, float *humidity);
void sht3x_status_read(uint16_t *val);
void sht3x_clear_status(void);
void sht3x_heater_switch(uint8_t on);

#endif /* __BSP_SHT3X_H */

