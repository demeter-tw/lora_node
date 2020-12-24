#ifndef __LORA_APP_H
#define __LORA_APP_H

#include "lora_mac.h"

#define SLEEP_COUNTS_DOWN               900000 /* ms */
#define SLEEP_COUNTS_DOWN_SHORT         10000 /* ms */
#define LORA_APP_PROCESS_TIMER          (CLOCK_CONF_SECOND / 200)
#define LORA_APP_PROCESS_TIMER_ONE_SECOND   (1000 / LORA_APP_PROCESS_TIMER)

typedef struct {
        uint8_t packet_type: 5;
        uint8_t reserved: 3;
} lora_body_header_t;

//=============== Node --> Gateway ===================

typedef struct {
        lora_body_header_t body_header;
} lora_body_data_resp_t;

typedef struct {
        lora_body_header_t body_header;
        // No data
} lora_body_join_req_t;

typedef struct {
        lora_body_header_t body_header;
        // No data
} lora_body_join_compl_t;

//=============== Gateway --> Node ===================

typedef struct {
        lora_body_header_t body_header;
        // No data
} lora_body_data_req_t;

typedef struct {
        lora_body_header_t body_header;
        uint8_t channel;
} lora_body_join_resp_t;

typedef enum {
        LORA_RESERVER = 0x00,
        LORA_JOINREQ = 0x01,
        LORA_JOINRESP = 0x02,
        LORA_JOINCOMP = 0x03,
        LORA_DATAREQ = 0x04,
        LORA_DATARESP = 0x05,
        LORA_DATACOMP = 0x06,
        LORA_LEAVEREQ = 0x07,
        LORA_LEAVERESP = 0x08,
        LORA_START = 0x09,
        LORA_ACK = 0x0A,
        LORA_WAKEUP = 0x0B,
} lora_packet_type_t;

typedef struct {
        lora_body_header_t bh;
        uint16_t addr;
        uint8_t channel;
} lora_join_resp_boyd_t;

typedef enum {
        LORA_DEVICE_STATE_START,
        LORA_DEVICE_STATE_JOIN_START,
        LORA_DEVICE_STATE_JOIN_COMPLETE,
        LORA_DEVICE_STATE_LEAVE,
        LORA_DEVICE_STATE_DATA_UPLINK,
        LORA_DEVICE_STATE_SENDING,
        LORA_DEVICE_STATE_WAIT_RESP,
        LORA_DEVICE_STATE_IDLE,
        LORA_DEVICE_STATE_SLEEP_COUNTDOWN,
} lora_device_state_t;

typedef enum {
        LORA_GATEWAY,
        LORA_NODE,
} lora_role_t;

typedef struct {
        uint8_t role;   /* LoRa mac role (gateway or node) */
        lora_device_state_t state; /* LoRa mac state machine */
        lora_device_state_t prev_state; /* LoRa mac state machine */
        uint8_t channel; /* LoRa channel */
        int wait_resp_timeout;  /* Timeout for waiting response from gateway */
        uint32_t tx_retry_count;        /* Retry tx count */
        uint32_t max_tx_retry;  /* The maximum retry if wiat response timeout occur */
        lora_addr_t lora_gw_addr;       /* The gateway id in the LoRa network */
        uint8_t gw_pkt_identifier;     /* The gateway id in the LoRa network */
        uint32_t sleep_countdown;    /* Counts down to enter sleep mode */
} lora_app_t;

void lora_app_init(void);

#endif /* __LORA_APP_H */

