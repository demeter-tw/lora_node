#ifndef __LORA_MAC_H
#define __LORA_MAC_H

#include "config.h"

#include "radio.h"

#define LORA_CHANNLE_NUM                12

#define LORA_WAIT_REQ_TIMEOUT		60000

#if CONFIG_LORA_8B_ADDR
#define DA_ADDR_CHECK(da, sid)          ((da[0] == sid[0]) && (da[1] == sid[1]) && (da[2] == sid[2]) && (da[3] == sid[3]) && (da[4] == sid[4]) && (da[5] == sid[5]) && (da[6] == sid[6]) && (da[7] == sid[7]))
#define IS_BROADCAST_PKT(da)            ((da[0] == 0xFF) && (da[1] == 0xFF) && (da[2] == 0xFF) && (da[3] == 0xFF) && (da[4] == 0xFF) && (da[5] == 0xFF) && (da[6] == 0xFF) && (da[7] == 0xFF))

#define LORA_PACKET_HDR_LEN2		19 /* DA (8) + SA (8) + Version (1) + Identifier (1) + Body Len (1) */
#define LORA_BODY_HDR_LEN2              4 /* Code (1) + Identifier (1) + Lenght (1) + Type (1) */
#define LORA_PACKET_VERSION		5

typedef uint8_t	lora_addr_t[8];
#else /* CONFIG_LORA_8B_ADDR */
#define DA_ADDR_CHECK(da, sid)          ((da[0] == sid[0]) && (da[1] == sid[1]) && (da[2] == sid[2]) && (da[3] == sid[3]))
#define IS_BROADCAST_PKT(da)            ((da[0] == 0xFF) && (da[1] == 0xFF) && (da[2] == 0xFF) && (da[3] == 0xFF))

#define LORA_PACKET_HDR_LEN2		11 /* DA (4) + SA (4) + Version (1) + Identifier (1) + Body Len (1) */
#define LORA_BODY_HDR_LEN2              4 /* Code (1) + Identifier (1) + Lenght (1) + Type (1) */
#define LORA_PACKET_VERSION		5

typedef uint8_t	lora_addr_t[4];
#endif /* !CONFIG_LORA_8B_ADDR */

typedef struct lora_packet_s {
        lora_addr_t da;
        lora_addr_t sa;
        uint8_t version: 4;
        uint8_t reserved: 4;
        uint8_t identifer;
        uint8_t body_len;
        uint8_t body[2];
} lora_packet_t;

typedef struct lora_callback_s {
        void (*tx_done_cb) (void);
        void (*rx_done_cb)(lora_addr_t, uint8_t, uint8_t *, uint8_t, uint8_t, int16_t, int8_t);
        void (*tx_timeout_cb)(void);
        void (*rx_timeout_cb)(void);
        void (*rx_error_cb)(void);
} lora_callback_t;

typedef struct {
        uint32_t tx_cnt;
        uint32_t rx_cnt;
        uint32_t tx_timeout_cnt;
        uint32_t rx_err_cnt;
} lora_stats_t;

typedef struct lora_s   lora_t;
struct lora_s{
        lora_addr_t sid;        /* Device unique id */
        uint16_t pkt_buf_len;   /* Length of packet in pkt_buf */
        uint8_t pkt_buf[256];   /* Buffer containing the data to be sent or received */
        lora_callback_t *callbacks;     /* LoRa mac upper layer event functions */
        radio_events_t radio_events;    /* Radio evnets function pointer */
        lora_stats_t stats;     /* Statistics */
        uint32_t last_tx_time;  /* Last packet transmit time */
        uint32_t last_rx_time;  /* Last packet receive time */
} ;

void lora_mac_set_channel(uint8_t ch);
void lora_mac_send(lora_addr_t da, uint8_t identifier, uint8_t *body, int body_len, uint8_t channel);
void lora_mac_reinit(void);
void lora_mac_init(lora_callback_t *callbacks);

#endif /* __LORA_MAC_H */

