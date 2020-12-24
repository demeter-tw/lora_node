#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "config.h"

#if CONFIG_LED_ACTIVE_LOW
#define LED_ON(port, pin)               do {\
                                                port->BRR = pin;\
                                        } while (0);
#define LED_OFF(port, pin)              do {\
                                                port->BSRR = pin;\
                                        } while (0);
#else
#define LED_ON(port, pin)               do {\
                                                port->BSRR = pin;\
                                        } while (0);
#define LED_OFF(port, pin)              do {\
                                                port->BRR = pin;\
                                        } while (0);
#endif /* CONFIG_LED_ACTIVE_LOW */

#define LED_TOGGLE(port, pin)           do {\
                                                port->ODR ^= pin;\
                                        } while (0);

#if CONFIG_LED_1
#define LED1_ON                         LED_ON(CONFIG_LED_1_GPIO_PORT, CONFIG_LED_1_GPIO_PIN)
#define LED1_OFF                        LED_OFF(CONFIG_LED_1_GPIO_PORT, CONFIG_LED_1_GPIO_PIN)
#define LED1_TOGGLE                     LED_TOGGLE(CONFIG_LED_1_GPIO_PORT, CONFIG_LED_1_GPIO_PIN)
#endif

#if CONFIG_LED_2
#define LED2_ON                         LED_ON(CONFIG_LED_2_GPIO_PORT, CONFIG_LED_2_GPIO_PIN)
#define LED2_OFF                        LED_OFF(CONFIG_LED_2_GPIO_PORT, CONFIG_LED_2_GPIO_PIN)
#define LED2_TOGGLE                     LED_TOGGLE(CONFIG_LED_2_GPIO_PORT, CONFIG_LED_2_GPIO_PIN)
#endif

void led_init(void);
void led_toggle(GPIO_TypeDef *gpiox, uint16_t pin);
uint8_t led_get_val(GPIO_TypeDef *gpiox, uint16_t pin);
void led_off(GPIO_TypeDef *gpiox, uint16_t pin);
void led_on(GPIO_TypeDef *gpiox, uint16_t pin);

#endif /* __BSP_LED_H__*/


