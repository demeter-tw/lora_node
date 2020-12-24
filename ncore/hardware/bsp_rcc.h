#ifndef __BSP_RCC_H__
#define __BSP_RCC_H__

#include "config.h"
#include "errno.h"

NCORE_RESULT_t rcc_init(void);
void rcc_mco_config(void);

void mdelay(__IO uint32_t nTime);
void delay_decrement(void);

#endif /* __BSP_RCC_H__ */

