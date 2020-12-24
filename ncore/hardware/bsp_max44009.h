#ifndef __BSP_MAX44009_H
#define __BSP_MAX44009_H

#include "config.h"

#if CONFIG_DEMETER_SENSOR_LIGHT

#define MAX44009_ADDR                   0x4A

#define MAX44009_INT_STATU              0x00
#define MAX44009_INT_ENABLE             0x01
#define MAX44009_CONF                   0x02
#define MAX44009_LUX_HI                 0x03
#define MAX44009_LUX_LOW                0x04
#define MAX44009_UP_TH                  0x05
#define MAX44009_LOW_TH                 0x06
#define MAX44009_TH_TIMER               0x07

uint32_t max44009_lux_read(uint32_t *data);

#endif /* CONFIG_DEMETER_SENSOR_LIGHT */

#endif /* __BSP_MAX44009_H */

