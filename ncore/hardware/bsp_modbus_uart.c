#include "bsp_modbus_uart.h"
#include <stdlib.h>
#include <stdio.h>

#if CONFIG_MODULE_MODBUS

UART_T _uart;

static void _uart_send_data(uint8_t ch)
{
        USART_SendData(_uart.uart, (uint8_t) ch);
}

void modbus_uart_data_cpy(uint8_t data[], uint8_t *size)
{
        int cnt;

        for (cnt = 0; cnt < _uart.rx_count; cnt++) {
                data[cnt] = _uart.rx_buf[_uart.rx_read];

                if (++_uart.rx_read >= DATA_BUF_SIZE) {
                        _uart.rx_read = 0;
                }
        }

        *size = _uart.rx_count;
        _uart.rx_count = 0;
}

void CONFIG_MODBUS_UART_IRQHandler(void)
{
        if (USART_GetITStatus(_uart.uart, USART_IT_RXNE) != RESET) {
                uint8_t ch;

                ch = USART_ReceiveData(_uart.uart);
                //printf("========== USART_IT_RXNE ========== ch = 0x%02x\r\n", ch);
                _uart.rx_buf[_uart.rx_write] = ch;

                if (++_uart.rx_write >= DATA_BUF_SIZE) {
                        _uart.rx_write = 0;
                }

                if (_uart.rx_count < DATA_BUF_SIZE) {
                        _uart.rx_count++;
                }

                if (_uart.rx_fn != NULL) {
                        _uart.rx_fn(ch);
                }
        } else if (USART_GetITStatus(_uart.uart, USART_IT_TXE) != RESET) {
                if (_uart.tx_count == 0) {
                        USART_ITConfig(_uart.uart, USART_IT_TXE, DISABLE);
                        USART_ITConfig(_uart.uart, USART_IT_TC, ENABLE);
                } else {
                        _uart_send_data(_uart.tx_buf[_uart.tx_read]);

                        if (++_uart.tx_read >= DATA_BUF_SIZE) {
                                _uart.tx_read = 0;
                        }

                        _uart.tx_count--;
                }
        } else if (USART_GetITStatus(_uart.uart, USART_IT_TC) != RESET) {
                if (_uart.tx_count == 0) {
                        USART_ITConfig(_uart.uart, USART_IT_TC, DISABLE);

                        //send complete
                        if (_uart.tx_done_fn != NULL) {
                                _uart.tx_done_fn();
                        }
                } else {
                        _uart_send_data(_uart.tx_buf[_uart.tx_read]);

                        if (++_uart.tx_read >= DATA_BUF_SIZE) {
                                _uart.tx_read = 0;
                        }

                        _uart.tx_count--;
                }
        }
}

void modbus_uart_rx_dump(void)
{
        int i;

        printf("size = %d\r\n", _uart.rx_count);

        for (i = 0; i < _uart.rx_count; i++) {
                printf("0x%02x\r\n", _uart.rx_buf[i]);
        }
}


void modbus_uart_send(uint8_t ch)
{
        _uart_send_data(ch);
        printf("0x%02x\r\n", ch);

        USART_ITConfig(CONFIG_MODBUS_UART, USART_IT_TC, ENABLE);
}


void modbus_uart_send_buf(uint8_t data[], int size)
{
        int cnt;

        if (size > DATA_BUF_SIZE) {
                printf("\r\n[MODBUS UART] Error: size > %d\r\n", DATA_BUF_SIZE);
                return ;
        }

        for (cnt =0; cnt < size; cnt++) {
                _uart.tx_buf[_uart.tx_write] = data[cnt];

                if (++_uart.tx_write >= DATA_BUF_SIZE) {
                        _uart.tx_write = 0;
                }

                _uart.tx_count++;
        }

        USART_ITConfig(_uart.uart, USART_IT_TXE, ENABLE);
}

static void nvic_config(void)
{
        NVIC_InitTypeDef nvic_init;

        nvic_init.NVIC_IRQChannel = CONFIG_MODBUS_UART_IRQ;
        nvic_init.NVIC_IRQChannelPreemptionPriority = CONFIG_MODBUS_UART_NVIC_PRIO;
        nvic_init.NVIC_IRQChannelSubPriority = 0;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);
}

void modbus_uart_register_tx_done_cb(MODBUS_UART_TX_DONE_CB fn)
{
        _uart.tx_done_fn = fn;
}

void modbus_uart_register_rx_cb(MODBUS_UART_RX_CB fn)
{
        _uart.rx_fn = fn;
}


void modbus_uart_init(void)
{
        GPIO_InitTypeDef gpio_init;
        USART_InitTypeDef uart_init;

        CONFIG_MODBUS_UART_CLK_FUN(CONFIG_MODBUS_UART_CLK, ENABLE);
        CONFIG_MODBUS_UART_TX_GPIO_CLK_FUN(CONFIG_MODBUS_UART_TX_GPIO_CLK, ENABLE);
        CONFIG_MODBUS_UART_RX_GPIO_CLK_FUN(CONFIG_MODBUS_UART_RX_GPIO_CLK, ENABLE);

        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Pin = CONFIG_MODBUS_UART_TX_GPIO_PIN;
        GPIO_Init(CONFIG_MODBUS_UART_TX_GPIO_PORT, &gpio_init);

        gpio_init.GPIO_Pin = CONFIG_MODBUS_UART_RX_GPIO_PIN;
        gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(CONFIG_MODBUS_UART_RX_GPIO_PORT, &gpio_init);

        uart_init.USART_BaudRate = CONFIG_MODBUS_UART_BAUDRATE;
        uart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        uart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        uart_init.USART_Parity = USART_Parity_No;
        uart_init.USART_StopBits = USART_StopBits_1;
        uart_init.USART_WordLength = USART_WordLength_8b;
        USART_Init(CONFIG_MODBUS_UART, &uart_init);

        nvic_config();

        USART_ITConfig(CONFIG_MODBUS_UART, USART_IT_RXNE, ENABLE);

        USART_Cmd(CONFIG_MODBUS_UART, ENABLE);

        _uart.uart = CONFIG_MODBUS_UART;
}

#endif /* CONFIG_MODULE_MODBUS */

