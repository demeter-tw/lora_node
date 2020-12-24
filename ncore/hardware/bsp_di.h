#ifndef __BSP_DI_H
#define __BSP_DI_H

#include "config.h"

#if CONFIG_MODULE_DI

#if CONFIG_DI1
#define DI1_DATA()         GPIO_ReadInputDataBit(CONFIG_DI1_GPIO_PORT, CONFIG_DI1_GPIO_PIN)
#endif /* CONFIG_DI1 */

#if CONFIG_DI2
#define DI2_DATA()        GPIO_ReadInputDataBit(CONFIG_DI2_GPIO_PORT, CONFIG_DI2_GPIO_PIN)
#endif /* CONFIG_DI2 */

void di_init(void);

#endif /* CONFIG_MODULE_DI */

#endif /* __BSP_DI_H */

