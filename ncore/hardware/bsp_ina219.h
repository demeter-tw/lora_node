#ifndef __BSP_INA219_H
#define __BSP_INA219_H

#include "config.h"

#define INA219_SLAVE_ADDR	0x40

#define INA219_CONF_REG		0x0
#define INA219_SHUNT_VOLT_REG	0x1
#define INA219_BUS_VOLT_REG	0x2
#define INA219_PWR_REG		0x3
#define INA219_CURRENT_REG	0x4
#define INA219_CALIB_REG		0x5

#define INA219_RST_POS		15
#define INA219_RST		(0x1 << INA219_RST_POS)

#define INA219_BRNG_POS		13
#define INA219_BRNG_MASK	(0x1 << INA219_BRNG_POS)

#define INA219_PG_POS		11
#define INA219_PG_MASK		(0x3 << INA219_PG_POS)

#define INA219_BADC_POS		7
#define INA219_BADC_MASK	(0xF << INA219_BADC_POS)

#define INA219_SADC_POS		3
#define INA219_SADC_MASK	(0xF << INA219_SADC_POS)

#define INA219_MODE_POS		0
#define INA219_MODE_MASK	(0x7 << INA219_MODE_POS)

typedef enum {
	INA219_BRNG_16V = 0x0,
	INA219_BRNG_32V = 0x1
} INA219_BRNG;

typedef enum {
	INA219_PG_1_40MV = 0x0,
	INA219_PG_2_80MV = 0x1,
	INA219_PG_4_160MV = 0x2,
	INA219_PG_8_320MV = 0x3
} INA219_PG;

typedef enum {
	INA219_ADCRES_9BIT_84US = 0x0,
	INA219_ADCRES_10BIT_148US = 0x1,
	INA219_ADCRES_11BIT_276US = 0x2,
	INA219_ADCRES_12BIT_532US = 0x3,
	INA219_ADCRES_2S_1060US = 0x9,
	INA219_ADCRES_4S_2130US = 0xA,
	INA219_ADCRES_8S_4260US = 0xB,
	INA219_ADCRES_16S_8510US = 0xC,
	INA219_ADCRES_32S_17020US = 0xD,
	INA219_ADCRES_64S_34050US = 0xE,
	INA219_ADCRES_128S_68100US = 0xF
} INA219_ADC;

typedef enum {
	INA219_MODE_POWER_DOWN = 0x0,
	INA219_MODE_S_VOLT_TRIGGERED = 0x1,
	INA219_MODE_B_VOLT_TRIGGERED = 0x2,
	INA219_MODE_S_B_VOLT_TRIGGERED = 0x3,
	INA219_MODE_ADC_OFF = 0x4,
	INA219_MODE_S_VOLT_CONTINUOUS = 0x5,
	INA219_MODE_B_VOLT_CONTINUOUS = 0x6,
	INA219_MODE_S_B_VOLT_CONTINUOUS = 0x7
} INA219_MODE;

typedef union {
	uint16_t val;
	struct {
		uint16_t mode: 3;
		uint16_t sadc: 4;
		uint16_t badc: 4;
		uint16_t pg: 2;
		uint16_t brng: 1;
		uint16_t reserv: 1;
		uint16_t rst: 1;
	} fields;
} ina219_config_t;

int bsp_ina219_shunt_volt_get(float *voltag);
int bsp_ina219_bus_volt_get(float *voltag);
int bsp_ina219_current_get(float *current);
int bsp_ina219_power_get(float *power);
void bsp_ina219_init(void);

#endif /* __BSP_INA219_H */

