#ifndef __BSP_INTERNAL_FLASH_H
#define __BSP_INTERNAL_FLASH_H

#include "config.h"

#define FLASH_PAGE_SIZE         ((uint16_t) 0x400)

void internal_flash_lock(void);
void internal_flash_unlock(void);
int internal_flash_erase(uint32_t start_addr, uint32_t nb_page);
int internal_flash_write(__IO uint32_t *address, uint32_t *data, uint16_t data_length);
void internal_flash_read(__IO uint32_t *address, uint32_t *buf, uint16_t len);

#endif /* __BSP_INTERNAL_FLASH_H */

