#ifndef __MAIN_H
#define __MAIN_H

#include "config.h"

#define MODEL_DEFAULT                   0
#define MODEL_SOIL_TEMP_HUMI_EC         1

#if MODEL_DEFAULT
#define SENSOR_MAX_CNT          2
#elif MODEL_SOIL_TEMP_HUMI_EC
#define SENSOR_MAX_CNT          1
#endif

#define MBUS_TIMEOUT		0x10000

#if CONFIG_RCC_HSI || CONFIG_RCC_HSE
#define MBUS_LONG_TIMEOUT	0x50000
#else
#define MBUS_LONG_TIMEOUT	0x500000
#endif /* CONFIG_RCC_HSI || CONFIG_RCC_HSE */

typedef enum sensor_type_e              sensor_type_t;
enum sensor_type_e {
        Sensor_Type_Undefined = 0x0,
        Sensor_Type_Light_Temp_Humidity = 0x1,
        Sensor_Type_Wind_Direction = 0x2,
        Sensor_Type_Wind_Speed = 0x3,
        Sensor_Type_Ground_Temp_Humidity_EC = 0x4,
        Sensor_Type_Rain = 0x5,
        Sensor_Type_Irradiance = 0x6,
        Sensor_Type_Precipitation = 0x7,
        Sensor_Type_Water_Level = 0x8,
        Sensor_Type_PH = 0x9,
        Sensor_Type_ORP = 0xA,
        Sensor_Type_Water_EC_TEMP = 0xB,
        Sensor_Type_Water_Level_DDI = 0xC,
        Sensor_Type_Wind_Direction_SONBEST = 0xD,
        Sensor_Type_Wind_Speed_SONBEST = 0xE,
        Sensor_Type_Wind_Direction_SONBEST2 = 0xF,
        Sensor_Type_Water_EC = 0x10,
        Sensor_Type_DO = 0x11,
        Sensor_Type_CO2 = 0x12,
        Sensor_Type_Leaf_Humidity = 0x13,
        Sensor_Type_VOC = 0x14,
        Sensor_Type_Soil_Temp_Humidity =0x15,
        Sensor_Type_Soil_EC = 0x16,
        Sensor_Type_Flow = 0x17,
        Sensor_Type_Flow2 = 0x18,
        Sensor_Type_Air_Temp_Humidity_SM7620B = 0x19,
        Sensor_Type_Air_Temp_Humidity_Light_RM5391B = 0x1A,     // 200000 lux
        Sensor_Type_Air_Temp_Humidity_Light_RM5391B2 = 0x1B,    // 65535 lux
        Sensor_Type_Level_Gauge_QDY30A = 0x1C
};

typedef enum device_class_e             Device_Class_e;
enum device_class_e {
	Device_Class_Light = 1,
	Device_Class_Humidity = 2,
	Device_Class_Fertilizer = 3,
	Device_Class_Temperature = 4,
	Device_Class_Key = 5,
	Device_Class_Sprinklers = 6,
	Device_Class_Theft = 7,
	Device_Class_Fan = 8,
	Device_Class_Lamp = 9,
	Device_Class_Motor = 10,
	Device_Class_Pump = 11,
	Device_Class_Irradiance = 12,
	Device_Class_Wind_Dir = 13,
	Device_Class_Wind_Speed = 14,
	Device_Class_Flow_Meter = 15,
	Device_Class_Co2 = 16,
	Device_Class_Alarm = 17,
	Device_Class_Drip_Irrigation = 18,
	Device_Class_Spray = 19,
	Device_Class_Ip_Cam = 20,
	Device_Class_Hold_Button = 21,
	Device_Class_Gear = 22,
	Device_Class_PH = 23,
	Device_Class_EC = 24,
	Device_Class_Rain = 25,
	Device_Class_Precipitation = 26,
	Device_Class_Water_Level = 27,
	Device_Class_ORP = 28,
	Device_Class_Water_Level_DDI = 29,
	Device_Class_DO = 30,
	Device_Class_Leaf_Humidity = 31,
	Device_Class_VOC = 32,
	Device_Class_SV = 33,
	Device_Class_CT = 34,
	Device_Class_WD = 35,
	Device_Class_Flow = 36,
	Device_Class_Feedback = 37,
};

typedef struct {
	uint8_t dc;
	float value;
} sensing_data_t;

typedef struct {
	uint8_t addr;
	uint8_t is_connect: 1;
	uint8_t is_recv: 1;
	uint8_t fail_cnt: 6;
	uint8_t nb_reg;
	uint8_t nb_data;
	uint8_t sensor_type;
	uint16_t start_addr;
	sensing_data_t *data;
} mbus_sensor_t;

void wakeup_sensor_process(void);

#endif /* __MAIN_H */

