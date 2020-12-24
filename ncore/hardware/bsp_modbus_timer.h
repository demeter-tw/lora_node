#ifndef __BSP_MODBUS_TIMER_H
#define __BSP_MODBUS_TIMER_H

#include "stm32f10x.h"
#include "config.h"

#if CONFIG_MODULE_MODBUS

typedef void (*MODBUS_TIMER_EXPIRED_CB) (void);

void modbus_timer_tick(void);
uint32_t modbus_timer_get(void);
void modbus_timer_start(uint8_t cc, uint32_t timeout, void *cb);
void modbus_timer_init(void);
void modbus_timer_tick(void);

#endif /* CONFIG_MODULE_MODBUS */

#endif /* __BSP_MODBUS_TIMER_H */

