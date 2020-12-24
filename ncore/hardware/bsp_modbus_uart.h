#ifndef __BSP_MODBUS_UART_H
#define __BSP_MODBUS_UART_H

#include "stm32f10x.h"
#include "config.h"

#if CONFIG_MODULE_MODBUS

#define DATA_BUF_SIZE   128

typedef void (*MODBUS_UART_TX_DONE_CB) (void);
typedef void (*MODBUS_UART_RX_CB) (uint8_t ch);


typedef struct {
        USART_TypeDef *uart;

        uint8_t tx_buf[DATA_BUF_SIZE];
        uint8_t rx_buf[DATA_BUF_SIZE];

        uint16_t tx_count;
        uint16_t tx_write;
        uint16_t tx_read;
        MODBUS_UART_TX_DONE_CB tx_done_fn;

        uint16_t rx_count;
        uint16_t rx_write;
        uint16_t rx_read;
        MODBUS_UART_RX_CB rx_fn;
} UART_T;

void modbus_uart_send(uint8_t ch);
void modbus_uart_register_tx_done_cb(MODBUS_UART_TX_DONE_CB fn);
void modbus_uart_register_rx_cb(MODBUS_UART_RX_CB fn);
void modbus_uart_init(void);
void modbus_uart_data_cpy(uint8_t data[], uint8_t *size);
void modbus_uart_send_buf(uint8_t data[], int size);

#endif /* CONFIG_MODULE_MODBUS */

#endif /* __BSP_MODBUS_UART_H */

