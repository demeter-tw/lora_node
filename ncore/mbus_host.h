#ifndef __MBUS_MASTER_H
#define __MBUS_MASTER_H

#include "bsp_modbus_uart.h"
#include "config.h"

#if CONFIG_MODULE_MODBUS

typedef void (*MBUS_DATA_RECEIVED_CB) (uint8_t *data, int size);

#define MBUS_HOST_BUF_SIZE            128
#define MBUS_HOST_BAUDRATE            9600

#define MBUS_MIN_DATA_SIZE                      5       // addr (1) + func code (1) + byte count (1) + CRC (2)

typedef enum {
        STATE_TX_IDLE,
        STATE_TX_XMIT,
        STATE_RX_INIT,
        STATE_RX_IDLE,
        STATE_RX_RECV,
        STATE_RX_ERROR
} MBUS_STATE;

typedef enum {
        EVNET_TX_DONE,
        EVENT_RX_NEW,
        EVENT_T3_5_EXPIRE,
} MBUS_EVENT;

typedef struct {
        MBUS_STATE tx_state;
        MBUS_STATE rx_state;

        uint8_t tx_buf[MBUS_HOST_BUF_SIZE];
        uint8_t tx_read;
        uint8_t tx_write;
        uint8_t tx_count;

        uint8_t rx_buf[MBUS_HOST_BUF_SIZE];
        uint8_t rx_read;
        uint8_t rx_write;
        uint8_t rx_count;

        MBUS_DATA_RECEIVED_CB fn;
} MBUS_HOST_T;


void mbus_host_init(void);
void mbus_host_register_rx_cb(MBUS_DATA_RECEIVED_CB fn);
void mbus_host_send(uint8_t addr, uint8_t fn_code, uint16_t reg, uint16_t num);
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen);
void mbus_host_send_ack_err(uint8_t addr, uint8_t fn_code, uint8_t err_code);
void mbus_host_send_ack(uint8_t addr, uint8_t fn_code, uint16_t reg_num, uint16_t *data);
void mbus_host_send_nodata_ack(uint8_t addr, uint8_t fn_code);
void mbus_host_send_onedata_ack(uint8_t addr, uint8_t fn_code, uint8_t data);

#endif /* CONFIG_MODULE_MODBUS */

#endif /* __MBUS_MASTER_H */

