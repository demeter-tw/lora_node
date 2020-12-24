#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

#include "config.h"

#define DEBUG_USART_RXNE()        USART_GetFlagStatus(CONFIG_DEBUG_USART, USART_FLAG_RXNE)
#define DEBUG_USART_RECEIVE()   USART_ReceiveData(CONFIG_DEBUG_USART)

void debug_usart_init(void);

#endif /* __BSP_DEBUG_USART_H__ */

