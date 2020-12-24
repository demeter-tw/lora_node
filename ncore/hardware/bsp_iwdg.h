#ifndef __BSP_IWDG_H
#define __BSP_IWDG_H

#include "config.h"

#if CONFIG_MODULE_WATCHDOG

void iwdg_config(uint8_t prv, uint16_t rlv);
void iwdg_feed(void);
void iwdg_init(void);

#endif /* CONFIG_MODULE_WATCHDOG */

#endif /* __BSP_IWDG_H */

