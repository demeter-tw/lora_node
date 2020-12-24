#include "lora_lib.h"
#include <stdio.h>

#if CONFIG_MODULE_LORA

uint16_t htons(uint16_t xx)
{
	uint16_t yy;

	yy = (uint16_t)((xx & 0x00ff) << 8);
	yy |= (uint16_t)((xx & 0xff00) >> 8);

	return yy;
}


void dump_packet(uint8_t *packet, int length)
{
        printf("Packet data: \r\n");

        for (int cnt = 0; cnt < length; cnt++) {
                printf("%02x%s", packet[cnt], (cnt % 16 == 15) ? "\r\n" : " ");
        }

        printf("\r\n");
}


uint8_t checksum(uint8_t *packet, int lenght)
{
        uint8_t checksum = 0x00;

        for (int cnt = 0; cnt < lenght; cnt++) {
                checksum += packet[cnt];
        }

        return checksum;
}

#endif /* CONFIG_MODULE_LORA */

