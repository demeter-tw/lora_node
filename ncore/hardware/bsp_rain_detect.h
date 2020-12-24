#ifndef __BSP_RAIN_DETECT_H
#define __BSP_RAIN_DETECT_H

#include "config.h"

#if CONFIG_DEMETER_SENSOR_RAIN

#define RD_GPIO_CLK_FUN         RCC_APB2PeriphClockCmd
#define RD_GPIO_CLK             RCC_APB2Periph_GPIOA
#define RD_GPIO_PORT            GPIOA
#define RD_GPIO_PIN             GPIO_Pin_0

#define RD_DATA                 (RD_GPIO_PORT->IDR & RD_GPIO_PIN)

void rain_detect_init(void);

#endif /* CONFIG_DEMETER_SENSOR_RAIN */

#endif /* __BSP_RAIN_DETECT_H */

