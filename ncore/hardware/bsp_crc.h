#ifndef __BSP_CRC_H
#define __BSP_CRC_H

#include "config.h"

void crc_init(void);
uint32_t crc_calc(uint32_t *data, uint32_t len);

#endif /* __BSP_CRC_H */

