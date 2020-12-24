#include "bsp_led.h"

void _led_set_value(GPIO_TypeDef *gpiox, uint16_t pin, uint8_t val)
{
        (val == 1) ? GPIO_SetBits(gpiox, pin) : GPIO_ResetBits(gpiox, pin);
}

void led_on(GPIO_TypeDef *gpiox, uint16_t pin)
{
#if CONFIG_LED_ACTIVE_LOW
        _led_set_value(gpiox, pin, 0);
#else
        _led_set_value(gpiox, pin, 1);
#endif
}

void led_off(GPIO_TypeDef *gpiox, uint16_t pin)
{
#if CONFIG_LED_ACTIVE_LOW
        _led_set_value(gpiox, pin, 1);
#else
        _led_set_value(gpiox, pin, 0);
#endif
}

uint8_t led_get_val(GPIO_TypeDef *gpiox, uint16_t pin)
{
        return GPIO_ReadInputDataBit(gpiox, pin);
}

void led_toggle(GPIO_TypeDef *gpiox, uint16_t pin)
{
        gpiox->ODR ^= pin;
}

void led_init(void)
{
        GPIO_InitTypeDef init;

#if CONFIG_LED_1
        CONFIG_LED_1_GPIO_CLK_FUNC(CONFIG_LED_1_GPIO_CLK, ENABLE);

        init.GPIO_Pin = CONFIG_LED_1_GPIO_PIN;
        init.GPIO_Mode = GPIO_Mode_Out_PP;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(CONFIG_LED_1_GPIO_PORT, &init);
#endif

#if CONFIG_LED_2
        CONFIG_LED_2_GPIO_CLK_FUNC(CONFIG_LED_2_GPIO_CLK, ENABLE);

        init.GPIO_Pin = CONFIG_LED_2_GPIO_PIN;
        init.GPIO_Mode = GPIO_Mode_Out_PP;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(CONFIG_LED_2_GPIO_PORT, &init);
#endif
}

