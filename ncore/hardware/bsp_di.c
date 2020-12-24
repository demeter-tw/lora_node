#include "bsp_di.h"

#if CONFIG_MODULE_DI

void di_init(void)
{
        GPIO_InitTypeDef gpio_init;

#if CONFIG_DI1
        CONFIG_DI1_GPIO_CLK_FUN(CONFIG_DI1_GPIO_CLK, ENABLE);

        gpio_init.GPIO_Mode = GPIO_Mode_IPD;
        gpio_init.GPIO_Pin = CONFIG_DI1_GPIO_PIN;
        GPIO_Init(CONFIG_DI1_GPIO_PORT, &gpio_init);
#endif /* CONFIG_DI1 */

#if CONFIG_DI2
        CONFIG_DI2_GPIO_CLK_FUN(CONFIG_DI2_GPIO_CLK, ENABLE);

        gpio_init.GPIO_Mode = GPIO_Mode_IPD;
        gpio_init.GPIO_Pin = CONFIG_DI2_GPIO_PIN;
        GPIO_Init(CONFIG_DI2_GPIO_PORT, &gpio_init);
#endif /* CONFIG_DI2 */
}

#endif /* CONFIG_MODULE_DI */

