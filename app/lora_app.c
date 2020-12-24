#include "lora_app.h"
#include <stdio.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>
#include "fw_env.h"
#include "lora_mac.h"
#include <stdlib.h>
#include "bsp_rcc.h"
#include <string.h>
#include "bsp_di.h"
#include "bsp_io_board.h"
#include "main.h"
#include "bsp_ina219.h"

#define SENOSR_FAILED_VALUE     -9999

PROCESS(lora_app_process, "LoRa_App");

static void lora_app_tx_done_cb(void);
static void lora_app_rx_done_cb(lora_addr_t sa, uint8_t identifier, uint8_t *body, uint8_t body_len, uint8_t is_broadcast,int16_t rssi, int8_t snr);
void lora_app_tx_timeout_cb(void);
void lora_app_rx_timeout_cb(void);
void lora_app_rx_error_cb(void);

static lora_callback_t lora_callbacks = {
        lora_app_tx_done_cb,
        lora_app_rx_done_cb,
        lora_app_tx_timeout_cb,
        lora_app_rx_timeout_cb,
        lora_app_rx_error_cb,
    };

lora_app_t *lora_app_ctx;

#if CONFIG_LORA_8B_ADDR
lora_addr_t broadcast_addr = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#else /* CONFIG_LORA_8B_ADDR */
lora_addr_t broadcast_addr = { 0xFF, 0xFF, 0xFF, 0xFF};
#endif /* !CONFIG_LORA_8B_ADDR */

extern mbus_sensor_t _sensors[SENSOR_MAX_CNT];

void convert_f_to_u8_arr(uint8_t arr[], int *index, float val)
{
    uint32_t ftou32;

    ftou32 = *((uint32_t *) &val);

    arr[(*index)++] = (ftou32 & 0xFF000000) >> 24;
    arr[(*index)++] = (ftou32 & 0x00FF0000) >> 16;
    arr[(*index)++] = (ftou32 & 0x0000FF00) >> 8;
    arr[(*index)++] = (ftou32 & 0x000000FF);
}

static void _lora_app_state_change(lora_device_state_t new_state)
{
    lora_app_ctx->prev_state = lora_app_ctx->state;
    lora_app_ctx->state = new_state;

    if (new_state == LORA_DEVICE_STATE_SLEEP_COUNTDOWN) {
        // Reset count down value
        if (lora_app_ctx->prev_state == LORA_DEVICE_STATE_IDLE) {
            lora_app_ctx->sleep_countdown = SLEEP_COUNTS_DOWN / LORA_APP_PROCESS_TIMER;
        } else {
            lora_app_ctx->sleep_countdown = SLEEP_COUNTS_DOWN_SHORT / LORA_APP_PROCESS_TIMER;
        }
    }
}

static void _reset_countdown(void)
{
    printf("[LoRa_APP][INFO] Reset countdown.\r\n");
    lora_app_ctx->sleep_countdown = SLEEP_COUNTS_DOWN / LORA_APP_PROCESS_TIMER;
}

void process_join_resp(lora_body_join_resp_t *join_resp)
{
    char ch_str[10];

    printf("[LoRa_APP][INFO] Join response. (channel = %d)\r\n", join_resp->channel);

    lora_app_ctx->channel = join_resp->channel;
    lora_mac_set_channel(lora_app_ctx->channel);

    sprintf(ch_str, "%d", lora_app_ctx->channel);
    fw_env_write("channel", ch_str);
    fw_env_save();
}

void join_network_start(void)
{
    lora_body_join_req_t body;

    printf("[LoRa_APP][INFO] Join network start.\r\n");

    memset(&body, 0, sizeof(lora_body_join_req_t));

    body.body_header.packet_type = LORA_JOINREQ;
    lora_mac_send(broadcast_addr, 0, (uint8_t *) &body, sizeof(lora_body_join_req_t), 0);
    _lora_app_state_change(LORA_DEVICE_STATE_SENDING);
}

void join_network_complete(void)
{
    lora_body_join_compl_t body;

    printf("[LoRa_APP][INFO] Join network complete.\r\n");

    memset(&body, 0, sizeof(lora_body_join_compl_t));

    body.body_header.packet_type = LORA_JOINCOMP;
    lora_mac_send(lora_app_ctx->lora_gw_addr, 0, (uint8_t *) &body, sizeof(lora_body_join_compl_t), 0);
    _lora_app_state_change(LORA_DEVICE_STATE_SENDING);
}

void data_uplink(void)
{
    uint8_t buf[64];
    lora_body_data_resp_t *body = (lora_body_data_resp_t *) &buf[0];
    int len = 0;
    float voltag;
    int rv;

    printf("[LoRa_APP][INFO] Data uplink.\r\n");

    memset(buf, 0, sizeof(buf));

    body->body_header.packet_type = LORA_DATARESP;
    len++;

    for (int i = 0; i < SENSOR_MAX_CNT; i++) {
        for (int d = 0; d < _sensors[i].nb_data; d++) {
            float value;
            if (_sensors[i].is_recv) {
                value = _sensors[i].data[d].value;
            } else {
                value = SENOSR_FAILED_VALUE;
            }
            convert_f_to_u8_arr(buf, &len, value);
        }
    }

    // Add voltage value to response packet
    rv = bsp_ina219_bus_volt_get(&voltag);
    if (rv != NCORE_ERR_OK) {
        voltag = SENOSR_FAILED_VALUE;
    }
    convert_f_to_u8_arr(buf, &len, voltag);

    mdelay(100);

    lora_mac_send(lora_app_ctx->lora_gw_addr,
        lora_app_ctx->gw_pkt_identifier,
        buf, len, 0);
    _lora_app_state_change(LORA_DEVICE_STATE_SENDING);
}

static void lora_app_tx_done_cb(void)
{
    printf("[LoRa_APP][INFO] Packet tx success.\r\n");
    _lora_app_state_change(LORA_DEVICE_STATE_IDLE);
}

static void lora_app_rx_done_cb(lora_addr_t sa, uint8_t identifier, uint8_t *body, uint8_t body_len,
        uint8_t is_broadcast,int16_t rssi, int8_t snr)
{
    lora_body_header_t *lora_body;

    printf("[LoRa_APP][INFO] Packet rx success\r\n");

    if (!is_broadcast) {
        lora_body = (lora_body_header_t *) body;
        switch (lora_body->packet_type) {
        case LORA_DATAREQ:
            printf("[LoRa_APP][INFO] DATAREQ packet received.\r\n");
            _reset_countdown();
            memcpy(lora_app_ctx->lora_gw_addr, sa, sizeof(lora_addr_t));
            lora_app_ctx->gw_pkt_identifier = identifier;
            _lora_app_state_change(LORA_DEVICE_STATE_DATA_UPLINK);
            break;
        case LORA_DATACOMP:
            printf("[LoRa_APP][INFO] DATACOMP packet received.\r\n");
            _reset_countdown();
            _lora_app_state_change(LORA_DEVICE_STATE_IDLE);
            break;
        case LORA_JOINRESP:
            printf("[LoRa_APP][INFO] JOINRESP packet received.\r\n");
            _reset_countdown();
            memcpy(lora_app_ctx->lora_gw_addr, sa, sizeof(lora_addr_t));
            lora_app_ctx->gw_pkt_identifier = identifier;
            process_join_resp((lora_body_join_resp_t *) body);
            _lora_app_state_change(LORA_DEVICE_STATE_JOIN_COMPLETE);
            break;
        case LORA_WAKEUP:
            printf("[LoRa_APP][INFO] WAKEUP packet received.\r\n");
            _reset_countdown();
            wakeup_sensor_process();
            _lora_app_state_change(LORA_DEVICE_STATE_IDLE);
            break;
        }
    } else {
        printf("[LoRa_APP][INFO] This is broadcast pacekt.\r\n");
    }
}

void lora_app_tx_timeout_cb(void)
{
    printf("[LoRa_APP][INFO] Packet tx timeout.\r\n");
}

void lora_app_rx_timeout_cb(void)
{
    printf("[LoRa_APP][INFO] Packet rx timeout.\r\n");
}

void lora_app_rx_error_cb(void)
{
    printf("[LoRa_APP][INFO] Packet rx error.\r\n");
}

void lora_app_state_machine(void)
{
    switch (lora_app_ctx->state) {
    case LORA_DEVICE_STATE_JOIN_START:
        join_network_start();
        break;
    case LORA_DEVICE_STATE_JOIN_COMPLETE:
        join_network_complete();
        break;
    case LORA_DEVICE_STATE_DATA_UPLINK:
        data_uplink();
        break;
    case LORA_DEVICE_STATE_SENDING:
        printf("[LoRa_APP][INFO] Sending...\r\n");
        break;
    case LORA_DEVICE_STATE_SLEEP_COUNTDOWN:
        --lora_app_ctx->sleep_countdown;
        if (lora_app_ctx->sleep_countdown % LORA_APP_PROCESS_TIMER_ONE_SECOND == 0) {
            printf("[LoRa_APP][INFO] Sleep counts down, seconds = %d\r\n", lora_app_ctx->sleep_countdown / LORA_APP_PROCESS_TIMER_ONE_SECOND);
        }
        if (lora_app_ctx->sleep_countdown == 0) {
            printf("[LoRa_APP][WARN] Countdown timer expired, reinit module.\r\n");
            lora_mac_reinit();
            printf("[LoRa_APP][WARN] Reinit module done.\r\n");

            if (!lora_app_ctx->channel) {
                printf("[LoRa_APP][WARN] Still no join any network, enter JOIN_START mode.\r\n");
                _lora_app_state_change(LORA_DEVICE_STATE_JOIN_START);
            } else {
                printf("[LoRa_APP][WARN] Enter IDLE mode.\r\n");
                _lora_app_state_change(LORA_DEVICE_STATE_IDLE);
            }
        }
        break;
    case LORA_DEVICE_STATE_IDLE:
        printf("[LoRa_APP][INFO] IDLE.\r\n");
        _lora_app_state_change(LORA_DEVICE_STATE_SLEEP_COUNTDOWN);
        break;
    }
}

PROCESS_THREAD(lora_app_process, ev, data)
{
    PROCESS_BEGIN();
    static struct etimer lora_process_timer;
    etimer_set(&lora_process_timer, LORA_APP_PROCESS_TIMER);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        lora_app_state_machine();
        etimer_reset(&lora_process_timer);
    }
    PROCESS_END();
}

void lora_app_init(void)
{
    char *channel_str = NULL;
    uint32_t channel;

    /* Clean channel env if PA1 is pull-high */
    if (DI1_DATA()) {
        printf("[LoRa_APP][INFO] Clean channel env\r\n");
        fw_env_write("channel", NULL);
        fw_env_save();
    }

    lora_app_ctx = (lora_app_t *) calloc(1, sizeof(lora_app_t));
    lora_app_ctx->role = LORA_NODE;

    lora_mac_init(&lora_callbacks);

    channel_str = fw_getenv("channel");
    if (channel_str != NULL) {
        sscanf(channel_str, "%d", &channel);
        lora_app_ctx->channel = channel;
        lora_mac_set_channel(lora_app_ctx->channel);
        printf("[LoRa_APP][INFO] Channel: %d\r\n", lora_app_ctx->channel);
        lora_app_ctx->state = LORA_DEVICE_STATE_IDLE;
    } else {
        lora_app_ctx->channel = 0;
        printf("[LoRa_APP][ERR] Channel env is not found.\r\n");
        lora_app_ctx->state = LORA_DEVICE_STATE_JOIN_START;
    }

#if !CONFIG_LORA_TEST_MODE
    process_start(&lora_app_process, NULL);
#endif /* !CONFIG_LORA_TEST_MODE */
}

