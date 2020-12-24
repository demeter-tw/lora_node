#include "bsp_rain_detect.h"

#if CONFIG_DEMETER_SENSOR_RAIN

void rain_detect_init(void)
{
        GPIO_InitTypeDef gpio_init;

        RD_GPIO_CLK_FUN(RD_GPIO_CLK, ENABLE);

        gpio_init.GPIO_Mode = GPIO_Mode_IPD;
        gpio_init.GPIO_Pin = RD_GPIO_PIN;
        GPIO_Init(RD_GPIO_PORT, &gpio_init);
}

#endif /* CONFIG_DEMETER_SENSOR_RAIN */

