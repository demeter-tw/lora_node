#include "bsp_debug_usart.h"
#include <stdio.h>

void debug_usart_init(void)
{
        GPIO_InitTypeDef gpio_init;
        USART_InitTypeDef usart_init;

        CONFIG_DEBUG_USART_CLK_FUNC(CONFIG_DEBUG_USART_CLK, ENABLE);
        CONFIG_DEBUG_USART_TX_GPIO_CLK_FUNC(CONFIG_DEBUG_USART_TX_GPIO_CLK, ENABLE);
        CONFIG_DEBUG_USART_RX_GPIO_CLK_FUNC(CONFIG_DEBUG_USART_RX_GPIO_CLK, ENABLE);

        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Pin = CONFIG_DEBUG_USART_TX_GPIO_PIN;
        GPIO_Init(CONFIG_DEBUG_USART_TX_GPIO_PORT, &gpio_init);

        gpio_init.GPIO_Pin = CONFIG_DEBUG_USART_RX_GPIO_PIN;
        gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(CONFIG_DEBUG_USART_RX_GPIO_PORT, &gpio_init);

        usart_init.USART_BaudRate = CONFIG_DEBUG_USART_BAUDRATE;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        usart_init.USART_Parity = USART_Parity_No;
        usart_init.USART_StopBits = USART_StopBits_1;
        usart_init.USART_WordLength = USART_WordLength_8b;
        USART_Init(CONFIG_DEBUG_USART, &usart_init);

        USART_Cmd(CONFIG_DEBUG_USART, ENABLE);
}


int fputc(int ch, FILE *f)
{
        USART_SendData(CONFIG_DEBUG_USART, (uint8_t) ch);

        while (USART_GetFlagStatus(CONFIG_DEBUG_USART, USART_FLAG_TXE) == RESET);

        return (ch);
}


int fgetc(FILE *f)
{
        int ch;

        while (USART_GetFlagStatus(CONFIG_DEBUG_USART, USART_FLAG_RXNE) == RESET);

        ch = (int) USART_ReceiveData(CONFIG_DEBUG_USART);
        printf("%c", ch);

        return ch;
}
