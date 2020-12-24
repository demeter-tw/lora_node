#ifndef __LORA_LIB_H
#define __LORA_LIB_H

#include "config.h"

#if CONFIG_MODULE_LORA

#define DEC(x)                          { if (x != 0) x--; }

#define LORA_CHANNEL_BMP(i)                     (0x1 << i)
#define LORA_CHANNEL_BMP_SET(bmp, i)            ((bmp) |= LORA_CHANNEL_BMP(i))
#define LORA_CHANNEL_BMP_UNSET(bmp, i)          ((bmp) &= ~(LORA_CHANNEL_BMP(i)))
#define LORA_CHANNEL_BMP_ISSET(bmp, i)          ((bmp) & LORA_CHANNEL_BMP(i))

#define LORA_DMSG_TITLE                   "[LoRa][%s][%s] "

#if CONFIG_LORA_DEBUG == 1
#define LORA_DMSG(level, tag, fmt, arg...)              do {\
                                                                if (level <= CONFIG_LORA_DEBUG_LEVEL) {\
                                                                        if (level == CONFIG_LORA_DEBUG_LEVEL_INFO) \
                                                                                printf(LORA_DMSG_TITLE fmt, tag, "INFO", ##arg);\
                                                                        else if (level == CONFIG_LORA_DEBUG_LEVEL_WARN) \
                                                                                printf(LORA_DMSG_TITLE fmt, tag, "WARN", ##arg);\
                                                                        else if (level == CONFIG_LORA_DEBUG_LEVEL_ERR) \
                                                                                printf(LORA_DMSG_TITLE fmt, tag, "ERR", ##arg);\
                                                                        else if (level == CONFIG_LORA_DEBUG_LEVEL_DEBUG) \
                                                                                printf(LORA_DMSG_TITLE fmt, tag, "DEBUG", ##arg);\
                                                                        else if (level == CONFIG_LORA_DEBUG_LEVEL_NOTICE) \
                                                                                printf(LORA_DMSG_TITLE fmt, tag, "NOTICE", ##arg);\
                                                                }\
                                                        } while (0);
#else
#define LORA_DMSG(level, tag, fmt, arg...)
#endif

uint16_t htons(uint16_t xx);
void dump_packet(uint8_t *packet, int length);
uint8_t checksum(uint8_t *packet, int lenght);

#endif /* CONFIG_MODULE_LORA */

#endif /* __LORA_LIB_H */

