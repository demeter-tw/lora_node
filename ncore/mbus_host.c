#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bsp_modbus_uart.h"
#include "bsp_modbus_timer.h"
#include "mbus_host.h"

#if CONFIG_MODULE_MODBUS

const uint8_t s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

const uint8_t s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

MBUS_HOST_T *_mbus;

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen)
{
	uint8_t ucCRCHi = 0xFF;
	uint8_t ucCRCLo = 0xFF;
	uint16_t usIndex;

        while (_usLen--) {
		usIndex = ucCRCHi ^ *_pBuf++;
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}

void mbus_host_recv_done_cb(void)
{
        modbus_uart_data_cpy(_mbus->rx_buf, &_mbus->rx_count);

        _mbus->fn(_mbus->rx_buf, _mbus->rx_count);
}

void mbus_host_t3_5_timer_expired(void)
{
        //printf("[MODBUS] t3.5 timer expired\r\n");

        switch (_mbus->rx_state) {
        case STATE_RX_INIT:
                /* Timer t35 expired. Startup phase is finished. */
                _mbus->rx_state = STATE_RX_IDLE;
                break;
        case STATE_RX_RECV:
                /* A frame was received and t35 expired. Notify the listener that
                 * a new frame was received. */
                _mbus->rx_state = STATE_RX_IDLE;
                mbus_host_recv_done_cb();
                break;
        case STATE_RX_ERROR:
                /* An error occured while receiving the frame */
                _mbus->rx_state = STATE_RX_IDLE;
                break;
        }
}


void mbus_host_t3_5_timer_start(void)
{
        uint32_t timeout;

        //printf("[MODBUS] t3.5 timer start\r\n");

        timeout = 35000000 / MBUS_HOST_BAUDRATE;

        modbus_timer_start(1, timeout, mbus_host_t3_5_timer_expired);
}


void mbus_host_tx_cb(void)
{
        if (_mbus->rx_state != STATE_RX_IDLE) {
                printf("\r\n#######[%s, %d] Error: RX is not IDLE #######\r\n", __func__, __LINE__);
        }

        switch (_mbus->tx_state) {
        case STATE_TX_IDLE:
                break;
        case STATE_TX_XMIT:
                _mbus->tx_state = STATE_TX_IDLE;
                break;
        }
}


void mbus_host_rx_cb(uint8_t ch)
{
        if (_mbus->tx_state != STATE_TX_IDLE) {
                printf("\r\n#######[%s, %d] Error: TX is not IDLE #######\r\n", __func__, __LINE__);
        }

        switch (_mbus->rx_state) {
        case STATE_RX_INIT:
                /* If we have received a character in the init state we have to wait until the frame is finished. */
                mbus_host_t3_5_timer_start();
                break;
        case STATE_RX_ERROR:
                /* If we have received a character in the init state we have to wait until the frame is finished. */
                mbus_host_t3_5_timer_start();
                break;
        case STATE_RX_IDLE:
                /* In the idle state we wait for a new character. If a character
                 * is received the t1.5 and t3.5 timers are started and the
                 * receiver is in the state STATE_RX_RECEIVCE.
                 */
                _mbus->rx_write = 0;
                _mbus->rx_buf[_mbus->rx_write] = ch;
                _mbus->rx_state = STATE_RX_RECV;

                /* Enable t3.5 timer */
                mbus_host_t3_5_timer_start();
                break;
        case STATE_RX_RECV:
                /* We are currently receiving a frame. Reset the timer after
                 * every character received. If more than the maximum possible
                 * number of bytes in a modbus frame is received the frame is
                 * ignored.
                 */
                if (++_mbus->rx_write < MBUS_HOST_BUF_SIZE) {
                        _mbus->rx_buf[_mbus->rx_write] = ch;
                } else {
                        _mbus->rx_state = STATE_RX_ERROR;
                }

                /* Reset t3.5 timer */
                mbus_host_t3_5_timer_start();
                break; 
        }
}

void mbus_host_send(uint8_t addr, uint8_t fn_code, uint16_t reg, uint16_t num)
{
        uint16_t crc = 0;

        if (_mbus->rx_state == STATE_RX_IDLE) {
                _mbus->tx_count = 0;
                _mbus->tx_read = 0;
                _mbus->tx_buf[_mbus->tx_count++] = addr;
                _mbus->tx_buf[_mbus->tx_count++] = fn_code;
                _mbus->tx_buf[_mbus->tx_count++] = reg >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = reg & 0xFF;
                _mbus->tx_buf[_mbus->tx_count++] = num >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = num & 0xFF;

                crc = CRC16_Modbus(_mbus->tx_buf, _mbus->tx_count);

                _mbus->tx_buf[_mbus->tx_count++] = crc >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = crc & 0xFF;

                _mbus->tx_state = STATE_TX_XMIT;
                modbus_uart_send_buf(_mbus->tx_buf, _mbus->tx_count);
        }
}


void mbus_host_send_ack_err(uint8_t addr, uint8_t fn_code, uint8_t err_code)
{
        uint16_t crc = 0;

        if (_mbus->rx_state == STATE_RX_IDLE) {
                _mbus->tx_count = 0;
                _mbus->tx_read = 0;
                _mbus->tx_buf[_mbus->tx_count++] = addr;
                _mbus->tx_buf[_mbus->tx_count++] = fn_code | 0x80;
                _mbus->tx_buf[_mbus->tx_count++] = err_code;

                crc = CRC16_Modbus(_mbus->tx_buf, _mbus->tx_count);

                _mbus->tx_buf[_mbus->tx_count++] = crc >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = crc & 0xFF;

                _mbus->tx_state = STATE_TX_XMIT;
                modbus_uart_send_buf(_mbus->tx_buf, _mbus->tx_count);
        }
}


void mbus_host_send_ack(uint8_t addr, uint8_t fn_code, uint16_t reg_num, uint16_t *data)
{
        uint16_t crc = 0;

        if (_mbus->rx_state == STATE_RX_IDLE) {
                _mbus->tx_count = 0;
                _mbus->tx_read = 0;
                _mbus->tx_buf[_mbus->tx_count++] = addr;
                _mbus->tx_buf[_mbus->tx_count++] = fn_code;
                _mbus->tx_buf[_mbus->tx_count++] = reg_num * 2;

                for (int i = 0; i < reg_num; i++) {
                        _mbus->tx_buf[_mbus->tx_count++] = (data[i] & 0xFF00) >> 8;
                        _mbus->tx_buf[_mbus->tx_count++] = data[i];
                }

                crc = CRC16_Modbus(_mbus->tx_buf, _mbus->tx_count);

                _mbus->tx_buf[_mbus->tx_count++] = crc >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = crc & 0xFF;

                _mbus->tx_state = STATE_TX_XMIT;
                modbus_uart_send_buf(_mbus->tx_buf, _mbus->tx_count);
        }
}


void mbus_host_send_nodata_ack(uint8_t addr, uint8_t fn_code)
{
        uint16_t crc = 0;

        if (_mbus->rx_state == STATE_RX_IDLE) {
                _mbus->tx_count = 0;
                _mbus->tx_read = 0;
                _mbus->tx_buf[_mbus->tx_count++] = addr;
                _mbus->tx_buf[_mbus->tx_count++] = fn_code;

                crc = CRC16_Modbus(_mbus->tx_buf, _mbus->tx_count);

                _mbus->tx_buf[_mbus->tx_count++] = crc >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = crc & 0xFF;

                _mbus->tx_state = STATE_TX_XMIT;
                modbus_uart_send_buf(_mbus->tx_buf, _mbus->tx_count);
        }
}


void mbus_host_send_onedata_ack(uint8_t addr, uint8_t fn_code, uint8_t data)
{
        uint16_t crc = 0;

        if (_mbus->rx_state == STATE_RX_IDLE) {
                _mbus->tx_count = 0;
                _mbus->tx_read = 0;
                _mbus->tx_buf[_mbus->tx_count++] = addr;
                _mbus->tx_buf[_mbus->tx_count++] = fn_code;
                _mbus->tx_buf[_mbus->tx_count++] = data;

                crc = CRC16_Modbus(_mbus->tx_buf, _mbus->tx_count);

                _mbus->tx_buf[_mbus->tx_count++] = crc >> 8;
                _mbus->tx_buf[_mbus->tx_count++] = crc & 0xFF;

                _mbus->tx_state = STATE_TX_XMIT;
                modbus_uart_send_buf(_mbus->tx_buf, _mbus->tx_count);
        }
}


void mbus_host_register_rx_cb(MBUS_DATA_RECEIVED_CB fn)
{
        _mbus->fn = fn;
}


void mbus_host_init(void)
{
        _mbus = (MBUS_HOST_T *) calloc(sizeof(MBUS_HOST_T), 1);
        _mbus->rx_state = STATE_RX_INIT;
        _mbus->tx_state = STATE_TX_IDLE;

        modbus_uart_init();
        modbus_uart_register_tx_done_cb(mbus_host_tx_cb);
        modbus_uart_register_rx_cb(mbus_host_rx_cb);

        modbus_timer_init();
        mbus_host_t3_5_timer_start();
}

#endif /* CONFIG_MODULE_MODBUS */

