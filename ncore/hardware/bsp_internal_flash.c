#include "bsp_internal_flash.h"
#include "errno.h"
#include <stdio.h>

void internal_flash_lock(void)
{
        FLASH_Lock();
}

void internal_flash_unlock(void)
{
        FLASH_Unlock();
}

int internal_flash_erase(uint32_t start_addr, uint32_t nb_page)
{
        int erase_cnt;
        FLASH_Status flash_status = FLASH_COMPLETE;
        uint32_t delay;

        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

        for (erase_cnt = 0; (erase_cnt < nb_page) && (flash_status == FLASH_COMPLETE); erase_cnt++) {
                delay = 0xFFFF;
                while (--delay);
                printf("Erase delay\r\n");
                flash_status = FLASH_ErasePage(start_addr + (FLASH_PAGE_SIZE * erase_cnt));
        }

        if (erase_cnt == nb_page) {
                return NCORE_ERR_OK;
        } else {
                return NCORE_ERR_FLASH_ERASE;
        }
}

int internal_flash_write(__IO uint32_t *address, uint32_t *data, uint16_t data_length)
{
        uint32_t i = 0;
        FLASH_Status flash_status = FLASH_COMPLETE;

        for (i = 0; (i < data_length) && (*address <= (CONFIG_FW_ENV_END_ADDR - 4)) && (flash_status == FLASH_COMPLETE); i++) {
                if (FLASH_ProgramWord(*address, *(uint32_t *) (data + i)) == FLASH_COMPLETE) {
                        if (*(uint32_t *) *address != *(uint32_t *) (data + i)) {
                                return NCORE_ERR_FLASH_PROGRAM_DATA_NOMATCH;
                        }

                        *address += 4;
                } else {
                        return NCORE_ERR_FLASH_PROGRAM_FAILED;
                }
        }

        return NCORE_ERR_OK;
}

void internal_flash_read(__IO uint32_t *address, uint32_t *buf, uint16_t len)
{
        int cnt = 0;

        while ((len--) && (*address < CONFIG_FW_ENV_END_ADDR - 4)) {
                *(buf + cnt) = *(__IO uint32_t *) *address;
                cnt++;
                *address += 4;
        }
}


